/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2005, 2006 Ciaran McCreesh <ciaranm@ciaranm.org>
 *
 * This file is part of the Paludis package manager. Paludis is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * Paludis is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <paludis/dep_atom.hh>
#include <paludis/match_package.hh>
#include <paludis/package_database.hh>
#include <paludis/util/iterator.hh>
#include <paludis/util/stringify.hh>
#include <paludis/util/collection_concrete.hh>
#include <paludis/util/compare.hh>

#include <list>
#include <map>
#include <set>

/** \file
 * Implementation of PackageDatabase.
 */

using namespace paludis;

#include "package_database_entry-sr.cc"

std::ostream &
paludis::operator<< (std::ostream & s, const PackageDatabaseEntry & v)
{
    s << v.name << "-" << v.version << "::" << v.repository;
    return s;
}

PackageDatabaseError::PackageDatabaseError(const std::string & our_message) throw () :
    Exception(our_message)
{
}

PackageDatabaseLookupError::PackageDatabaseLookupError(const std::string & our_message) throw () :
    PackageDatabaseError(our_message)
{
}

DuplicateRepositoryError::DuplicateRepositoryError(const std::string & name) throw () :
    PackageDatabaseError("A repository named '" + name + "' already exists")
{
}

NoSuchPackageError::NoSuchPackageError(const std::string & our_name) throw () :
    PackageDatabaseLookupError("Could not find '" + our_name + "'"),
    _name(our_name)
{
}

NoSuchRepositoryError::NoSuchRepositoryError(const std::string & name) throw () :
    PackageDatabaseLookupError("Could not find repository '" + name + "'")
{
}

/**
 * Name data for an AmbiguousPackageNameError.
 *
 * \ingroup grpexceptions
 */
struct AmbiguousPackageNameError::NameData
{
    /// Our query name
    std::string name;

    /// Our match names
    std::list<std::string> names;
};

template <typename I_>
AmbiguousPackageNameError::AmbiguousPackageNameError(const std::string & our_name,
        I_ begin, const I_ end) throw () :
    PackageDatabaseLookupError("Ambiguous package name '" + our_name + "' (candidates are " +
            join(begin, end, ", ") + ")"),
    _name_data(new NameData)
{
    _name_data->name = our_name;
    std::transform(begin, end, std::back_inserter(_name_data->names),
            &stringify<typename std::iterator_traits<I_>::value_type>);
}

AmbiguousPackageNameError::AmbiguousPackageNameError(const AmbiguousPackageNameError & other) :
    PackageDatabaseLookupError(other),
    _name_data(new NameData)
{
    _name_data->name = other._name_data->name;
    _name_data->names = other._name_data->names;
}

AmbiguousPackageNameError::~AmbiguousPackageNameError() throw ()
{
    delete _name_data;
}

AmbiguousPackageNameError::OptionsIterator
AmbiguousPackageNameError::begin_options() const
{
    return OptionsIterator(_name_data->names.begin());
}

AmbiguousPackageNameError::OptionsIterator
AmbiguousPackageNameError::end_options() const
{
    return OptionsIterator(_name_data->names.end());
}

const std::string &
AmbiguousPackageNameError::name() const
{
    return _name_data->name;
}

namespace paludis
{
    /**
     * Implementation data for a PackageDatabase.
     *
     * \ingroup grppackagedatabase
     */
    template<>
    struct Implementation<PackageDatabase> :
        InternalCounted<Implementation<PackageDatabase> >
    {
        /**
         * Our Repository instances.
         */
        std::list<Repository::Pointer> repositories;

        /// Our environment.
        const Environment * environment;
    };
}

PackageDatabase::PackageDatabase(const Environment * const e) :
    PrivateImplementationPattern<PackageDatabase>(new Implementation<PackageDatabase>)
{
    _imp->environment = e;
}

PackageDatabase::~PackageDatabase()
{
}

void
PackageDatabase::add_repository(const Repository::Pointer r)
{
    Context c("When adding a repository named '" + stringify(r->name()) + "':");

    IndirectIterator<std::list<Repository::Pointer>::const_iterator, const Repository>
        r_c(_imp->repositories.begin()),
        r_end(_imp->repositories.end());
    for ( ; r_c != r_end ; ++r_c)
        if (r_c->name() == r->name())
            throw DuplicateRepositoryError(stringify(r->name()));

    _imp->repositories.push_back(r);
}

QualifiedPackageName
PackageDatabase::fetch_unique_qualified_package_name(
        const PackageNamePart & p) const
{
    Context context("When disambiguating package name '" + stringify(p) + "':");

    QualifiedPackageNameCollection::Pointer result(new QualifiedPackageNameCollection::Concrete);

    IndirectIterator<std::list<Repository::Pointer>::const_iterator, const Repository>
        r(_imp->repositories.begin()),
        r_end(_imp->repositories.end());
    for ( ; r != r_end ; ++r)
    {
        Context local_context("When looking in repository '" + stringify(r->name()) + "':");

        CategoryNamePartCollection::ConstPointer cats(r->category_names_containing_package(p));
        for (CategoryNamePartCollection::Iterator c(cats->begin()), c_end(cats->end()) ;
                c != c_end ; ++c)
            result->insert(*c + p);
    }

    if (result->empty())
        throw NoSuchPackageError(stringify(p));
    if (result->size() > 1)
        throw AmbiguousPackageNameError(stringify(p), result->begin(), result->end());

    return *(result->begin());
}

PackageDatabaseEntryCollection::Pointer
PackageDatabase::query(const PackageDepAtom & a, const InstallState installed_state) const
{
    return query(a, installed_state, qo_order_by_version);
}

PackageDatabaseEntryCollection::Pointer
PackageDatabase::query(const PackageDepAtom & a, const InstallState installed_state,
        const QueryOrder query_order) const
{
    PackageDatabaseEntryCollection::Concrete::Pointer result(new PackageDatabaseEntryCollection::Concrete);

    IndirectIterator<std::list<Repository::Pointer>::const_iterator, const Repository>
        r(_imp->repositories.begin()),
        r_end(_imp->repositories.end());
    for ( ; r != r_end ; ++r)
    {
        if ((installed_state == is_installed_only) && ! r->installed_interface)
            continue;

        if ((installed_state == is_installable_only) && ! r->installable_interface)
            continue;

        VersionSpecCollection::ConstPointer versions(r->version_specs(a.package()));
        VersionSpecCollection::Iterator v(versions->begin()), v_end(versions->end());
        for ( ; v != v_end ; ++v)
        {
            PackageDatabaseEntry e(a.package(), *v, r->name());
            if (! match_package(_imp->environment, a, e))
                continue;

            result->push_back(e);
        }
    }

    do
    {
        switch (query_order)
        {
            case qo_order_by_version:
                _sort_package_database_entry_collection(*result);
                continue;

            case qo_group_by_slot:
                _sort_package_database_entry_collection(*result);
                _group_package_database_entry_collection(*result);
                continue;

            case qo_whatever:
                continue;

            case last_query_order:
                break;
        };

        throw InternalError(PALUDIS_HERE, "Bad query_order");
    }
    while (false);

    return result;
}

Repository::ConstPointer
PackageDatabase::fetch_repository(const RepositoryName & n) const
{
    std::list<Repository::Pointer>::const_iterator
        r(_imp->repositories.begin()),
        r_end(_imp->repositories.end());
    for ( ; r != r_end ; ++r)
        if ((*r)->name() == n)
            return *r;

    throw NoSuchRepositoryError(stringify(n));
}

Repository::Pointer
PackageDatabase::fetch_repository(const RepositoryName & n)
{
    std::list<Repository::Pointer>::const_iterator
        r(_imp->repositories.begin()),
        r_end(_imp->repositories.end());
    for ( ; r != r_end ; ++r)
        if ((*r)->name() == n)
            return *r;

    throw NoSuchRepositoryError(stringify(n));
}

RepositoryName
PackageDatabase::favourite_repository() const
{
    for (RepositoryIterator r(_imp->repositories.begin()), r_end(_imp->repositories.end()) ;
            r != r_end ; ++r)
        if ((*r)->can_be_favourite_repository())
            return (*r)->name();

    return RepositoryName("unnamed");
}

bool
PackageDatabase::more_important_than(const RepositoryName & lhs,
        const RepositoryName & rhs) const
{
    std::map<std::string, int> rank;
    int x(0);
    for (PackageDatabase::RepositoryIterator r(begin_repositories()), r_end(end_repositories()) ;
            r != r_end ; ++r)
        rank.insert(std::make_pair(stringify((*r)->name()), ++x));

    std::map<std::string, int>::const_iterator l(rank.find(stringify(lhs)));
    if (l == rank.end())
        throw InternalError(PALUDIS_HERE, "lhs.repository '" + stringify(lhs) + "' not in rank");

    std::map<std::string, int>::const_iterator r(rank.find(stringify(rhs)));
    if (r == rank.end())
        throw InternalError(PALUDIS_HERE, "rhs.repository '" + stringify(rhs) + "' not in rank");

    return l->second > r->second;

    throw InternalError(PALUDIS_HERE, "better_repository called on non-owned repositories");
}

namespace
{
    struct PDEComparator
    {
        const PackageDatabase * const pde;
        std::map<std::string, int> rank;

        PDEComparator(const PackageDatabase * const p) :
            pde(p)
        {
            int x(0);
            for (PackageDatabase::RepositoryIterator r(pde->begin_repositories()), r_end(pde->end_repositories()) ;
                    r != r_end ; ++r)
                rank.insert(std::make_pair(stringify((*r)->name()), ++x));
        }

        bool operator() (const PackageDatabaseEntry & lhs, const PackageDatabaseEntry & rhs) const
        {
            switch (compare(lhs.name, rhs.name))
            {
                case -1:
                    return true;

                case 1:
                    return false;
            }

            switch (compare(lhs.version, rhs.version))
            {
                case -1:
                    return true;

                case 1:
                    return false;
            }

            std::map<std::string, int>::const_iterator l(rank.find(stringify(lhs.repository)));
            if (l == rank.end())
                throw InternalError(PALUDIS_HERE, "lhs.repository '" + stringify(lhs.repository) + "' not in rank");

            std::map<std::string, int>::const_iterator r(rank.find(stringify(rhs.repository)));
            if (r == rank.end())
                throw InternalError(PALUDIS_HERE, "rhs.repository '" + stringify(rhs.repository) + "' not in rank");

            switch (compare(l->second, r->second))
            {
                case -1:
                    return true;

                case 1:
                    return false;
            }

            return false;
        }
    };
}

void
PackageDatabase::_sort_package_database_entry_collection(PackageDatabaseEntryCollection::Concrete & p) const
{
    if (! p.empty())
        p.sort(PDEComparator(this));
}

void
PackageDatabase::_group_package_database_entry_collection(PackageDatabaseEntryCollection::Concrete & p) const
{
    if (p.empty())
        return;

    for (std::list<PackageDatabaseEntry>::reverse_iterator r(p.list.rbegin()) ;
            r != p.list.rend() ; ++r)
    {
        SlotName r_slot(fetch_repository(r->repository)->version_metadata(r->name, r->version)->slot);

        for (std::list<PackageDatabaseEntry>::reverse_iterator rr(next(r)) ;
                rr != p.list.rend() ; ++rr)
        {
            SlotName rr_slot(fetch_repository(rr->repository)->version_metadata(rr->name, rr->version)->slot);
            if (rr_slot != r_slot)
                continue;

            p.list.splice(previous(r.base()), p.list, previous(rr.base()));
            if (p.list.rend() == ((rr = ++r)))
                return;
        }
    }
}

PackageDatabase::RepositoryIterator
PackageDatabase::begin_repositories() const
{
    return RepositoryIterator(_imp->repositories.begin());
}

PackageDatabase::RepositoryIterator
PackageDatabase::end_repositories() const
{
    return RepositoryIterator(_imp->repositories.end());
}

std::ostream &
paludis::operator<< (std::ostream & o, const InstallState & s)
{
    do
    {
        switch (s)
        {
            case is_installed_only:
                o << "installed_only";
                continue;

            case is_installable_only:
                o << "installable_only";
                continue;

            case is_any:
                o << "any";
                continue;

            case last_install_state:
                ;
        }

        throw InternalError(PALUDIS_HERE, "Bad InstallState");
    } while (false);

    return o;
}

