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
#include <paludis/dep_atom_flattener.hh>
#include <paludis/dep_list.hh>
#include <paludis/match_package.hh>
#include <paludis/util/collection_concrete.hh>
#include <paludis/util/iterator.hh>
#include <paludis/util/join.hh>
#include <paludis/util/log.hh>
#include <paludis/util/save.hh>
#include <paludis/util/stringify.hh>

#include <algorithm>
#include <functional>
#include <vector>
#include <set>

using namespace paludis;

#include <paludis/dep_list_entry-sr.cc>

std::ostream &
paludis::operator<< (std::ostream & s, const DepListEntry & e)
{
    s << e.name << "-" << e.version << ":"
        << e.metadata->slot << "::" << e.repository;
    return s;
}

DepListError::DepListError(const std::string & m) throw () :
    Exception(m)
{
}

DepListStackTooDeepError::DepListStackTooDeepError(int level) throw () :
    DepListError("DepList stack too deep (" + stringify(level) + " entries)")
{
}

NoResolvableOptionError::NoResolvableOptionError() throw () :
    DepListError("No resolvable || ( ) option")
{
}

template <typename I_>
NoResolvableOptionError::NoResolvableOptionError(I_ i, I_ end) throw () :
    DepListError("No resolvable || ( ) option." + (i == end ?
                std::string("") :  " Failure messages are '" + join(i, end, "', '") + "'"))
{
}

AllMaskedError::AllMaskedError(const std::string & query) throw () :
    DepListError("Error searching for '" + query + "': no available versions"),
    _query(query)
{
}

UseRequirementsNotMetError::UseRequirementsNotMetError(const std::string & query) throw () :
    DepListError("Error searching for '" + query + "': use requirements are not met"),
    _query(query)
{
}

BlockError::BlockError(const std::string & msg) throw () :
    DepListError("Block: " + msg)
{
}

template <typename I_>
CircularDependencyError::CircularDependencyError(I_ begin, const I_ end) throw () :
    DepListError("Circular dependency: " + join(begin, end, " -> ")),
    _cycle_size(std::distance(begin, end))
{
}

namespace paludis
{
    /**
     * Implementation data for DepList.
     */
    template<>
    struct Implementation<DepList> :
        InstantiationPolicy<Implementation<DepList>, instantiation_method::NonCopyableTag>,
        InternalCounted<Implementation<DepList> >
    {
        ///\name Provided data
        ///{
        const Environment * const environment;
        ///}

        ///\name Generated data
        ///{
        std::list<DepListEntry> merge_list;
        std::list<DepListEntry>::iterator merge_list_insert_pos;
        bool check_existing_only;
        bool match_found;
        const DepListEntry * current_package;
        ///}

        ///\name Settings
        ///{
        DepListRdependOption rdepend_post;
        bool recursive_deps;
        bool drop_circular;
        bool drop_self_circular;
        bool drop_all;
        bool ignore_installed;
        bool reinstall;
        bool no_unnecessary_upgrades;
        ///}

        ///\name Stack
        ///{
        int stack_depth;
        int max_stack_depth;
        ///}

        /// Constructor.
        Implementation(const Environment * const e) :
            environment(e),
            check_existing_only(false),
            match_found(false),
            current_package(0),
            rdepend_post(dlro_as_needed),
            recursive_deps(true),
            drop_circular(false),
            drop_self_circular(false),
            drop_all(false),
            ignore_installed(false),
            reinstall(true),
            stack_depth(0),
            max_stack_depth(100)
        {
        }
    };
}

DepList::DepList(const Environment * const e) :
    PrivateImplementationPattern<DepList>(new Implementation<DepList>(e))
{
}

DepList::~DepList()
{
}

namespace
{
    struct IsSkip
    {
        bool operator() (const DepListEntry & e) const
        {
            return e.flags[dlef_skip];
        }
    };
}

void
DepList::add(DepAtom::ConstPointer atom)
{
    Context context("When adding dependencies:");

    std::list<DepListEntry> save_merge_list(_imp->merge_list.begin(),
            _imp->merge_list.end());

    _imp->merge_list_insert_pos = _imp->merge_list.end();
    _add(atom);

    try
    {
        std::list<DepListEntry>::iterator i(_imp->merge_list.begin());
        _imp->merge_list_insert_pos = _imp->merge_list.end();
        while (i != _imp->merge_list.end())
        {
            if (! i->flags[dlef_has_predeps] && ! _imp->drop_all)
                throw InternalError(PALUDIS_HERE, "dle_has_predeps not set for " + stringify(*i));

            else if (! i->flags[dlef_has_trypredeps] && ! _imp->drop_all)
            {
                Save<const DepListEntry *> save_current_package(
                        &_imp->current_package, &*i);
                _add_in_role(_imp->environment->package_database()->fetch_repository(
                            i->repository)->version_metadata(
                            i->name, i->version)->deps.run_depend(),
                        "runtime dependencies");
                i->flags.set(dlef_has_trypredeps);
            }

            else if (! i->flags[dlef_has_postdeps] && ! _imp->drop_all)
            {
                Save<const DepListEntry *> save_current_package(
                        &_imp->current_package, &*i);
                _add_in_role(_imp->environment->package_database()->fetch_repository(
                            i->repository)->version_metadata(
                            i->name, i->version)->deps.post_depend(),
                        "post dependencies");
                i->flags.set(dlef_has_postdeps);
            }
            else
                ++i;
        }

        /* remove skip entries */
        _imp->merge_list.remove_if(IsSkip());
    }
    catch (...)
    {
        _imp->merge_list.swap(save_merge_list);
        throw;
    }
}

void
DepList::_add_raw(const DepAtom * const atom)
{
#if 0
    /// \bug VV this is debug code. remove it once we're sure this works
    std::list<DepListEntry> backup_merge_list(_imp->merge_list.begin(),
            _imp->merge_list.end());
#endif

    /* keep track of stack depth */
    Save<int> old_stack_depth(&_imp->stack_depth,
            _imp->stack_depth + 1);
    if (_imp->stack_depth > _imp->max_stack_depth)
        throw DepListStackTooDeepError(_imp->stack_depth);

    /* we need to make sure that merge_list doesn't get h0rked in the
     * event of a failure. */
    bool merge_list_was_empty(_imp->merge_list.empty()), irange_begin_is_begin(false);
    std::list<DepListEntry>::iterator save_last, save_first, save_irange_begin, save_irange_end;
    if (! merge_list_was_empty)
    {
        save_first = _imp->merge_list.begin();
        save_last = previous(_imp->merge_list.end());

        save_irange_end = _imp->merge_list_insert_pos;
        if (_imp->merge_list_insert_pos == _imp->merge_list.begin())
            irange_begin_is_begin = true;
        else
            save_irange_begin = previous(_imp->merge_list_insert_pos);
    }

    try
    {
        atom->accept(this);
    }
    catch (const InternalError &)
    {
        throw;
    }
    catch (...)
    {
        if (merge_list_was_empty)
            _imp->merge_list.clear();
        else
        {
            _imp->merge_list.erase(next(save_last), _imp->merge_list.end());
            _imp->merge_list.erase(_imp->merge_list.begin(), save_first);
            _imp->merge_list.erase(
                    irange_begin_is_begin ? _imp->merge_list.begin() : next(save_irange_begin),
                    save_irange_end);
        }

#if 0
        /// \bug VV this is debug code. remove it once we're sure this works
        if (backup_merge_list != _imp->merge_list)
        {
            Log::get_instance()->message(ll_warning, "Old merge_list: " + join(backup_merge_list.begin(),
                        backup_merge_list.end(), " -> "));
            Log::get_instance()->message(ll_warning, "New merge_list: " + join(_imp->merge_list.begin(),
                        _imp->merge_list.end(), " -> "));
            throw InternalError(PALUDIS_HERE, "merge list restore failed");
        }
#endif
        throw;
    }
}

void
DepList::_add_in_role_raw(const DepAtom * const atom, const std::string & role)
{
    Context context("When adding " + role + ":");
    _add_raw(atom);
}

DepList::Iterator
DepList::begin() const
{
    return Iterator(_imp->merge_list.begin());
}

DepList::Iterator
DepList::end() const
{
    return Iterator(_imp->merge_list.end());
}

void
DepList::visit(const AllDepAtom * const v)
{
    std::for_each(v->begin(), v->end(), accept_visitor(
                static_cast<DepAtomVisitorTypes::ConstVisitor *>(this)));
}

#ifndef DOXYGEN
struct DepListEntryMatcher :
    public std::unary_function<bool, const DepListEntry &>
{
    const Environment * const env;
    const PackageDepAtom & atom;

    DepListEntryMatcher(const Environment * const e, const PackageDepAtom & p) :
        env(e),
        atom(p)
    {
    }

    bool operator() (const DepListEntry & e) const
    {
        return match_package(env, atom, e);
    }
};
#endif

void
DepList::visit(const PackageDepAtom * const p)
{
    Context context("When resolving package dependency '" + stringify(*p) + "':");

    PackageDatabaseEntryCollection::ConstPointer installed(
            _imp->environment->package_database()->query(*p, is_installed_only));

    /* if we're installed and we don't want to upgrade unnecessarily,
     * stop if we're not on a top level target */
    if ((! installed->empty()) && (_imp->no_unnecessary_upgrades))
        if (0 != _imp->current_package)
            return;

    /* are we already on the merge list? */
    {
        std::list<DepListEntry>::iterator i;
        if (_imp->merge_list.end() != ((i = std::find_if(
                            _imp->merge_list.begin(),
                            _imp->merge_list.end(),
                            DepListEntryMatcher(_imp->environment, *p)))))
        {
            /* what's our status? */
            if (! i->flags[dlef_has_predeps])
            {
                if (! installed->empty())
                    return;

                else if (_imp->drop_circular)
                {
                    if (_imp->current_package)
                        Log::get_instance()->message(ll_warning, lc_context, "Dropping circular dependency on " +
                                stringify(_imp->current_package->name) + "-" +
                                stringify(_imp->current_package->version));
                    return;
                }

                else if (_imp->current_package && _imp->drop_self_circular &&
                        match_package(_imp->environment, p, _imp->current_package))
                {
                    Log::get_instance()->message(ll_warning, lc_context, "Dropping self-circular dependency on " +
                            stringify(_imp->current_package->name) + "-" +
                            stringify(_imp->current_package->version));
                    return;
                }

                else
                    throw CircularDependencyError(i, next(i));
            }

            if (p->tag())
                i->tag->insert(p->tag());
            return;
        }
    }

    /* are we allowed to install things? */
    if (_imp->check_existing_only)
    {
        _imp->match_found = ! installed->empty();
        return;
    }

    /* find the matching package */
    const PackageDatabaseEntry * match(0);
    VersionMetadata::ConstPointer metadata(0);
    PackageDatabaseEntryCollection::Pointer matches(0);

    matches = _imp->environment->package_database()->query(*p, is_uninstalled_only);
    for (PackageDatabaseEntryCollection::ReverseIterator e(matches->rbegin()),
            e_end(matches->rend()) ; e != e_end ; ++e)
    {
        /* if we're already installed, only include us if we're a better version or
         * if we're a top level target */
        /// \todo SLOTs?
        if ((! _imp->ignore_installed) && ((0 != _imp->current_package) || (! _imp->reinstall)))
            if (! installed->empty())
                if (e->version <= installed->last()->version)
                    continue;

        /* check masks */
        if (_imp->environment->mask_reasons(*e).any())
            continue;

        metadata = _imp->environment->package_database()->fetch_repository(
                e->repository)->version_metadata(e->name, e->version);
        match = &*e;
        break;
    }

    std::list<DepListEntry>::iterator merge_entry;
    SortedCollection<DepTag::ConstPointer, DepTag::Comparator>::Pointer tags(
            new SortedCollection<DepTag::ConstPointer, DepTag::Comparator>::Concrete);
    if (p->tag())
        tags->insert(p->tag());
    if (! match)
    {
        if (! installed->empty())
        {
            if (_imp->recursive_deps)
            {
                metadata = _imp->environment->package_database()->fetch_repository(
                        installed->last()->repository)->version_metadata(
                        installed->last()->name, installed->last()->version);
                DepListEntryFlags flags;
                flags.set(dlef_has_predeps);
                flags.set(dlef_skip);
                merge_entry = _imp->merge_list.insert(_imp->merge_list_insert_pos,
                        DepListEntry(installed->last()->name,
                            installed->last()->version, metadata,
                            installed->last()->repository, flags, tags));
            }
            else
                return;
        }
        else if (p->use_requirements_ptr())
        {
            /* if we *could* have a match except for the AllMaskedError,
             * throw a UseRequirementsNotMetError error instead. */
            if (! _imp->environment->package_database()->query(
                        p->without_use_requirements(), is_either)->empty())
                throw UseRequirementsNotMetError(stringify(*p));
            else
                throw AllMaskedError(stringify(*p));
        }
        else
            throw AllMaskedError(stringify(*p));
    }
    else
    {
        DepListEntryFlags flags;
        merge_entry = _imp->merge_list.insert(_imp->merge_list_insert_pos,
                DepListEntry(match->name, match->version,
                    metadata, match->repository, flags, tags));
    }

    /* if we provide things, also insert them. */
    if ((metadata->get_ebuild_interface()) && ! merge_entry->flags[dlef_skip])
    {
        DepAtom::ConstPointer provide(metadata->get_ebuild_interface()->provide());

        CountedPtr<PackageDatabaseEntry, count_policy::ExternalCountTag> e(0);

        if (_imp->current_package)
            e = CountedPtr<PackageDatabaseEntry, count_policy::ExternalCountTag>(
                    new PackageDatabaseEntry(
                        _imp->current_package->name,
                        _imp->current_package->version,
                        _imp->current_package->repository));

        DepAtomFlattener f(_imp->environment, e.raw_pointer(), provide);

        for (DepAtomFlattener::Iterator p(f.begin()), p_end(f.end()) ; p != p_end ; ++p)
        {
            PackageDepAtom pp(QualifiedPackageName((*p)->text()));
            if (_imp->merge_list.end() != std::find_if(
                        _imp->merge_list.begin(), _imp->merge_list.end(),
                        DepListEntryMatcher(_imp->environment, pp)))
                continue;

#if 0
            VersionMetadata::Pointer p_metadata(new VersionMetadata::Ebuild(
                        merge_entry->metadata->deps.parser));
            p_metadata->slot = merge_entry->metadata->slot;
            p_metadata->get_ebuild_interface()->virtual_for = stringify(merge_entry->name);
#else
            VersionMetadata::ConstPointer p_metadata(0);
            try
            {
                p_metadata = _imp->environment->package_database()->fetch_repository(RepositoryName(
                            "virtuals"))->version_metadata(pp.package(), merge_entry->version);
            }
            catch (const NoSuchPackageError & e)
            {
                Log::get_instance()->message(ll_warning, lc_context, "Error '" + stringify(e.message()) + "' ("
                        + stringify(e.what()) + ") when looking for virtual '" + stringify(pp.package())
                        + "' for merge entry '" + stringify(*merge_entry));
            }
#endif

            if (p_metadata)
            {
                DepListEntryFlags flags;
                flags.set(dlef_has_predeps);
                flags.set(dlef_has_trypredeps);
                flags.set(dlef_has_postdeps);
                _imp->merge_list.insert(next(merge_entry),
                        DepListEntry(pp.package(), merge_entry->version,
                            p_metadata, RepositoryName("virtuals"), flags,
                            SortedCollection<DepTag::ConstPointer, DepTag::Comparator>::Pointer(
                                new SortedCollection<DepTag::ConstPointer, DepTag::Comparator>::Concrete)));
            }
        }
    }

    Save<std::list<DepListEntry>::iterator> old_merge_list_insert_pos(
            &_imp->merge_list_insert_pos, merge_entry);

    Context resolved_context("Dependency resolution is '" + stringify(*merge_entry) + "':");

    /* new current package */
    Save<const DepListEntry *> old_current_package(&_imp->current_package,
            &*merge_entry);

    /* merge depends */
    if ((! merge_entry->flags[dlef_has_predeps]) && ! (_imp->drop_all))
        _add_in_role(metadata->deps.build_depend(), "build dependencies");
    merge_entry->flags.set(dlef_has_predeps);

    /* merge rdepends */
    if (! merge_entry->flags[dlef_has_trypredeps] && dlro_always != _imp->rdepend_post
            && ! _imp->drop_all)
    {
        try
        {
            _add_in_role(metadata->deps.run_depend(), "runtime dependencies");
            merge_entry->flags.set(dlef_has_trypredeps);
        }
        catch (const CircularDependencyError &)
        {
            if (dlro_never == _imp->rdepend_post)
                throw;
            else if (_imp->current_package)
                Log::get_instance()->message(ll_warning, lc_context, "Couldn't resolve runtime dependencies "
                        "for " + stringify(_imp->current_package->name) + "-" +
                        stringify(_imp->current_package->version) + " as build dependencies, "
                        "trying them as post dependencies");
        }
    }
}

void
DepList::visit(const UseDepAtom * const u)
{
    CountedPtr<PackageDatabaseEntry, count_policy::ExternalCountTag> e(0);

    if (_imp->current_package)
        e = CountedPtr<PackageDatabaseEntry, count_policy::ExternalCountTag>(
                new PackageDatabaseEntry(
                    _imp->current_package->name,
                    _imp->current_package->version,
                    _imp->current_package->repository));

    if (_imp->environment->query_use(u->flag(), e.raw_pointer()) ^ u->inverse())
        std::for_each(u->begin(), u->end(), std::bind1st(std::mem_fun(&DepList::_add), this));
}

#ifndef DOXYGEN
struct IsViable :
    public std::unary_function<bool, DepAtom::ConstPointer>
{
    const Implementation<DepList> & _impl;
    CountedPtr<PackageDatabaseEntry, count_policy::ExternalCountTag> e;

    IsViable(const Implementation<DepList> & impl) :
        _impl(impl),
        e(0)
    {
        if (_impl.current_package)
            e = CountedPtr<PackageDatabaseEntry, count_policy::ExternalCountTag>(
                    new PackageDatabaseEntry(
                        _impl.current_package->name,
                        _impl.current_package->version,
                        _impl.current_package->repository));
    }

    bool operator() (DepAtom::ConstPointer a)
    {
        const UseDepAtom * const u(a->as_use_dep_atom());
        if (0 != u)
            return _impl.environment->query_use(u->flag(), e.raw_pointer()) ^ u->inverse();
        else
            return true;
    }
};
#endif

void
DepList::visit(const AnyDepAtom * const a)
{
    /* try to resolve each of our children in return. note the annoying
     * special case for use? () flags:
     *
     *   || ( ) -> nothing
     *   || ( off1? ( blah1 ) off2? ( blah2 ) blah3 ) -> blah3
     *   || ( off1? ( blah1 ) off2? ( blah2 ) ) -> nothing
     *   || ( ( off1? ( blah1 ) ) blah2 ) -> nothing
     *
     * we handle this by keeping a list of 'viable children'.
     */

    std::list<DepAtom::ConstPointer> viable_children;
    std::copy(a->begin(), a->end(), filter_inserter(
                std::back_inserter(viable_children), IsViable(*_imp)));

    if (viable_children.empty())
    {
        if (_imp->current_package)
            Log::get_instance()->message(ll_qa, lc_context, "Package '" + stringify(*_imp->current_package)
                    + "' has suspicious || ( ) block that resolves to empty");
        return;
    }

    bool found(false);
    std::list<DepAtom::ConstPointer>::iterator found_i;
    for (std::list<DepAtom::ConstPointer>::iterator i(viable_children.begin()),
            i_end(viable_children.end()) ; i != i_end ; ++i)
    {
        Save<bool> save_check(&_imp->check_existing_only, true);
        Save<bool> save_match(&_imp->match_found, true);
        _add(*i);
        if ((found = _imp->match_found))
        {
            found_i = i;
            break;
        }
    }
    if (found)
    {
        if (_imp->recursive_deps && ! _imp->check_existing_only)
            _add(*found_i);
        return;
    }

    if (_imp->check_existing_only)
    {
        _imp->match_found = false;
        return;
    }

    /* try to merge each of our viable children in turn. */
    std::list<std::string> errors;
    for (CompositeDepAtom::Iterator i(viable_children.begin()), i_end(viable_children.end()) ;
            i != i_end ; ++i)
    {
        try
        {
            _add(*i);
            return;
        }
        catch (const DepListStackTooDeepError &)
        {
            /* don't work around a stack too deep error. our item may be
             * resolvable with a deeper stack. */
            throw;
        }
        catch (const DepListError & e)
        {
            errors.push_back(e.message() + " (" + e.what() + ")");
        }
    }

    /* no match */
    throw NoResolvableOptionError(errors.begin(), errors.end());
}

void
DepList::visit(const BlockDepAtom * const d)
{
    Context context("When checking block '!" + stringify(*(d->blocked_atom())) + "':");

    /* special case: the provider of virtual/blah can DEPEND upon !virtual/blah. */
    /* special case: foo/bar can DEPEND upon !foo/bar. */

    /* are we already installed? */
    PackageDatabaseEntryCollection::ConstPointer installed(_imp->environment->package_database()->
            query(d->blocked_atom(), is_installed_only));
    if (! installed->empty())
    {
        if (! _imp->current_package)
            throw BlockError("'" + stringify(*(d->blocked_atom())) + "' blocked by installed package '"
                    + stringify(*installed->last()) + "' (no current package)");

        for (PackageDatabaseEntryCollection::Iterator ii(installed->begin()),
                ii_end(installed->end()) ; ii != ii_end ; ++ii)
        {
            if (_imp->current_package->name == ii->name)
            {
                Log::get_instance()->message(ll_qa, lc_context, "Package '" + stringify(*_imp->current_package)
                        + "' has suspicious block upon '!" + stringify(*d->blocked_atom()) + "'");
                continue;
            }

            DepAtom::ConstPointer provide(new AllDepAtom);
            if (_imp->current_package->metadata->get_ebuild_interface())
                provide = _imp->current_package->metadata->get_ebuild_interface()->provide();

            CountedPtr<PackageDatabaseEntry, count_policy::ExternalCountTag> e(0);

            e = CountedPtr<PackageDatabaseEntry, count_policy::ExternalCountTag>(
                    new PackageDatabaseEntry(
                        _imp->current_package->name,
                        _imp->current_package->version,
                        _imp->current_package->repository));

            DepAtomFlattener f(_imp->environment, e.raw_pointer(), provide);

            bool skip(false);
            for (IndirectIterator<DepAtomFlattener::Iterator, const StringDepAtom> i(f.begin()),
                    i_end(f.end()) ; i != i_end ; ++i)
                if (QualifiedPackageName(i->text()) == d->blocked_atom()->package())
                {
                    skip = true;
                    break;
                }

            if (skip)
                Log::get_instance()->message(ll_qa, lc_context, "Ignoring block on '" +
                        stringify(*(d->blocked_atom())) + "' in '" +
                        stringify(*_imp->current_package) +
                        "' which is blocked by installed package '" + stringify(*ii) +
                        "' due to PROVIDE");
            else
                throw BlockError("'" + stringify(*(d->blocked_atom())) + "' blocked by installed package '"
                        + stringify(*installed->last()) + "' when trying to install package '" +
                        stringify(*_imp->current_package) + "'");
        }
    }

    /* will we be installed by this point? */
    std::list<DepListEntry>::iterator m(_imp->merge_list.begin());
    while (m != _imp->merge_list.end())
    {
        if (_imp->merge_list.end() != ((m = std::find_if(m, _imp->merge_list.end(),
                    DepListEntryMatcher(_imp->environment, *(d->blocked_atom()))))))
        {
            if (! _imp->current_package)
                throw BlockError("'" + stringify(*(d->blocked_atom())) + "' blocked by pending package '"
                        + stringify(*m) + "' (no current package)");

            if (*_imp->current_package == *m)
            {
                Log::get_instance()->message(ll_qa,lc_context,  "Package '" + stringify(*_imp->current_package)
                        + "' has suspicious block upon '!" + stringify(*d->blocked_atom()) + "'");
                ++m;
                continue;
            }

            DepAtom::ConstPointer provide(new AllDepAtom);
            if (_imp->current_package->metadata->get_ebuild_interface())
                provide = _imp->current_package->metadata->get_ebuild_interface()->provide();

            CountedPtr<PackageDatabaseEntry, count_policy::ExternalCountTag> e(0);

            e = CountedPtr<PackageDatabaseEntry, count_policy::ExternalCountTag>(
                    new PackageDatabaseEntry(
                        _imp->current_package->name,
                        _imp->current_package->version,
                        _imp->current_package->repository));

            DepAtomFlattener f(_imp->environment, e.raw_pointer(), provide);

            bool skip(false);
            for (IndirectIterator<DepAtomFlattener::Iterator, const StringDepAtom> i(f.begin()),
                    i_end(f.end()) ; i != i_end ; ++i)
                if (QualifiedPackageName(i->text()) == d->blocked_atom()->package())
                {
                    skip = true;
                    break;
                }

            if (skip)
                Log::get_instance()->message(ll_qa, lc_context, "Ignoring block on '" +
                        stringify(*(d->blocked_atom())) + "' in '" +
                        stringify(*_imp->current_package) +
                        "' which is blocked by pending package '" + stringify(*m) +
                        "' due to PROVIDE");
            else
                throw BlockError("'" + stringify(*(d->blocked_atom())) + "' blocked by pending package '"
                        + stringify(*m) + "' when trying to install '"
                        + stringify(*_imp->current_package) + "'");

            ++m;
        }
    }
}

void
DepList::set_rdepend_post(const DepListRdependOption value)
{
    _imp->rdepend_post = value;
}

void
DepList::set_drop_circular(const bool value)
{
    _imp->drop_circular = value;
}

void
DepList::set_drop_self_circular(const bool value)
{
    _imp->drop_self_circular = value;
}

void
DepList::set_drop_all(const bool value)
{
    _imp->drop_all = value;
}

void
DepList::set_ignore_installed(const bool value)
{
    _imp->ignore_installed = value;
}

void
DepList::set_recursive_deps(const bool value)
{
    _imp->recursive_deps = value;
}

void
DepList::set_max_stack_depth(const int value)
{
    _imp->max_stack_depth = value;
}

void
DepList::visit(const PlainTextDepAtom * const t)
{
    throw InternalError(PALUDIS_HERE, "Got unexpected PlainTextDepAtom '" + t->text() + "'");
}

void
DepList::set_reinstall(const bool value)
{
    _imp->reinstall = value;
}

void
DepList::set_no_unnecessary_upgrades(const bool value)
{
    _imp->no_unnecessary_upgrades = value;
}

