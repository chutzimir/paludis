/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2005, 2006 Ciaran McCreesh <ciaran.mccreesh@blueyonder.co.uk>
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

#include <paludis/repositories/portage/portage_repository_profile.hh>
#include <paludis/repositories/portage/portage_repository_exceptions.hh>

#include <paludis/util/log.hh>
#include <paludis/util/tokeniser.hh>
#include <paludis/util/iterator.hh>
#include <paludis/util/save.hh>

#include <paludis/config_file.hh>
#include <paludis/environment.hh>
#include <paludis/match_package.hh>
#include <paludis/hashed_containers.hh>

#include <list>
#include <algorithm>
#include <set>

#include <strings.h>
#include <ctype.h>

using namespace paludis;

namespace paludis
{
    /**
     * Implementation for PortageRepositoryProfile.
     *
     * \ingroup grpportagerepository
     * \see PortageRepositoryProfile
     */
    template<>
    class Implementation<PortageRepositoryProfile> :
        public InternalCounted<PortageRepositoryProfile>
    {
        public:
            ///\name Convenience typedefs
            ///\{

            typedef MakeHashedMap<UseFlagName, UseFlagState>::Type UseMap;
            typedef MakeHashedSet<UseFlagName>::Type UseFlagSet;
            typedef MakeHashedMap<std::string, std::string>::Type EnvironmentVariablesMap;
            typedef MakeHashedMap<QualifiedPackageName,
                    std::list<std::pair<PackageDepAtom::ConstPointer, UseFlagName> > >::Type PackageUseMaskMap;
            typedef MakeHashedMap<QualifiedPackageName, PackageDepAtom::ConstPointer>::Type VirtualsMap;
            typedef MakeHashedMap<QualifiedPackageName, std::list<PackageDepAtom::ConstPointer> >::Type PackageMaskMap;

            ///\}

        private:
            void load_profile_directory_recursively(const FSEntry & dir);
            void load_profile_parent(const FSEntry & dir);
            void load_profile_make_defaults(const FSEntry & dir);
            void load_profile_use_mask_or_force(const FSEntry & dir,
                    const std::string & mask_or_force, UseFlagSet & set);
            void load_profile_package_use_mask_or_force(const FSEntry & dir,
                    const std::string & mask_or_force, PackageUseMaskMap & set);
            void load_profile_packages(const FSEntry & dir);
            void load_profile_virtuals(const FSEntry & dir);
            void load_profile_package_mask(const FSEntry & dir);

            void add_use_expand_to_use();
            void load_system_packages();
            void handle_profile_arch_var();

            std::set<std::string> system_lines;

        public:
            ///\name General variables
            ///\{

            const Environment * const env;

            ///\}

            ///\name Use flags
            ///\{

            UseMap use;
            UseFlagSet use_expand;
            UseFlagSet use_expand_hidden;
            UseFlagSet use_mask;
            UseFlagSet use_force;
            PackageUseMaskMap package_use_mask;
            PackageUseMaskMap package_use_force;

            ///\}

            ///\name Environment variables
            ///\{

            EnvironmentVariablesMap environment_variables;

            ///\}

            ///\name System package set
            ///\{

            AllDepAtom::Pointer system_packages;
            GeneralSetDepTag::Pointer system_tag;

            ///\}

            ///\name Virtuals
            ///\{

            VirtualsMap virtuals;

            ///\}

            ///\name Masks
            ///\{

            PackageMaskMap package_mask;

            ///\}

            ///\name Queries
            ///\{

            bool use_mask_or_force(const UseFlagName & u, const PackageDatabaseEntry * const e,
                    const std::string & mask_or_force, const UseFlagSet & global,
                    const PackageUseMaskMap & package) const;

            ///\}

            ///\name Basic operations
            ///\{

            Implementation(const Environment * const e, const FSEntryCollection & dirs) :
                env(e),
                system_packages(new AllDepAtom),
                system_tag(new GeneralSetDepTag("system"))
            {
                for (FSEntryCollection::Iterator d(dirs.begin()), d_end(dirs.end()) ;
                        d != d_end ; ++d)
                    load_profile_directory_recursively(*d);

                add_use_expand_to_use();
                load_system_packages();
                handle_profile_arch_var();
            }

            ~Implementation()
            {
            }

            ///\}
    };
}

void
Implementation<PortageRepositoryProfile>::load_profile_directory_recursively(const FSEntry & dir)
{
    Context context("When adding profile directory '" + stringify(dir) + ":");

    if (! dir.is_directory())
    {
        Log::get_instance()->message(ll_warning, lc_context,
                "Profile component '" + stringify(dir) + "' is not a directory");
        return;
    }

    load_profile_parent(dir);
    load_profile_make_defaults(dir);
    load_profile_use_mask_or_force(dir, "mask", use_mask);
    load_profile_use_mask_or_force(dir, "force", use_force);
    load_profile_package_use_mask_or_force(dir, "mask", package_use_mask);
    load_profile_package_use_mask_or_force(dir, "force", package_use_force);
    load_profile_packages(dir);
    load_profile_virtuals(dir);
    load_profile_package_mask(dir);
}

void
Implementation<PortageRepositoryProfile>::load_profile_parent(const FSEntry & dir)
{
    Context context("When handling parent file for profile directory '" + stringify(dir) + ":");

    if (! (dir / "parent").exists())
        return;

    LineConfigFile file(dir / "parent");

    LineConfigFile::Iterator i(file.begin()), i_end(file.end());
    if (i == i_end)
        Log::get_instance()->message(ll_warning, lc_context, "parent file is empty");
    else
        for ( ; i != i_end ; ++i)
        {
            FSEntry parent_dir(dir);
            do
            {
                try
                {
                    parent_dir = (parent_dir / *i).realpath();
                }
                catch (const FSError & e)
                {
                    Log::get_instance()->message(ll_warning, lc_context, "Skipping parent '"
                            + *i + "' due to exception: " + e.message() + " (" + e.what() + ")");
                    continue;
                }

                load_profile_directory_recursively(parent_dir);

            } while (false);
        }
}

void
Implementation<PortageRepositoryProfile>::load_profile_make_defaults(const FSEntry & dir)
{
    Context context("When handling make.defaults file for profile directory '" + stringify(dir) + ":");

    if (! (dir / "make.defaults").exists())
        return;

    KeyValueConfigFile file(dir / "make.defaults");

    try
    {
        std::list<std::string> uses;
        WhitespaceTokeniser::get_instance()->tokenise(file.get("USE"), std::back_inserter(uses));

        for (std::list<std::string>::const_iterator u(uses.begin()), u_end(uses.end()) ;
                u != u_end ; ++u)
            if ('-' == u->at(0))
                use[UseFlagName(u->substr(1))] = use_disabled;
            else
                use[UseFlagName(*u)] = use_enabled;
    }
    catch (const NameError & e)
    {
        Log::get_instance()->message(ll_warning, lc_context, "Loading USE failed due to exception: "
                + e.message() + " (" + e.what() + ")");
    }

    try
    {
        WhitespaceTokeniser::get_instance()->tokenise(
                file.get("USE_EXPAND"), create_inserter<UseFlagName>(
                    std::inserter(use_expand, use_expand.begin())));
    }
    catch (const NameError & e)
    {
        Log::get_instance()->message(ll_warning, lc_context, "Loading USE_EXPAND failed due to exception: "
                + e.message() + " (" + e.what() + ")");
    }

    try
    {
        WhitespaceTokeniser::get_instance()->tokenise(
                file.get("USE_EXPAND_HIDDEN"), create_inserter<UseFlagName>(
                    std::inserter(use_expand_hidden, use_expand_hidden.begin())));
    }
    catch (const NameError & e)
    {
        Log::get_instance()->message(ll_warning, lc_context, "Loading USE_EXPAND_HIDDEN failed due to exception: "
                + e.message() + " (" + e.what() + ")");
    }

    for (KeyValueConfigFile::Iterator k(file.begin()), k_end(file.end()) ;
            k != k_end ; ++k)
        environment_variables[k->first] = k->second;
}

void
Implementation<PortageRepositoryProfile>::load_profile_use_mask_or_force(const FSEntry & dir,
        const std::string & mask_or_force, UseFlagSet & set)
{
    Context context("When handling use." + mask_or_force + " file for profile directory '" +
            stringify(dir) + ":");

    if (! (dir / ("use." + mask_or_force)).exists())
        return;

    LineConfigFile file(dir / ("use." + mask_or_force));
    try
    {
        for (LineConfigFile::Iterator line(file.begin()), line_end(file.end()) ;
                line != line_end ; ++line)
        {
            if ('-' == line->at(0))
                set.erase(UseFlagName(line->substr(1)));
            else
                set.insert(UseFlagName(*line));
        }
    }
    catch (const NameError & e)
    {
        Log::get_instance()->message(ll_warning, lc_context, "Loading use." + mask_or_force +
                " failed due to exception: " + e.message() + " (" + e.what() + ")");
    }
}

void
Implementation<PortageRepositoryProfile>::load_profile_package_use_mask_or_force(const FSEntry & dir,
        const std::string & mask_or_force, PackageUseMaskMap & set)
{
    Context context("When handling package.use." + mask_or_force + " file for profile directory '" +
            stringify(dir) + ":");

    if (! (dir / ("package.use." + mask_or_force)).exists())
        return;

    LineConfigFile file(dir / ("package.use." + mask_or_force));
    try
    {
        for (LineConfigFile::Iterator line(file.begin()), line_end(file.end()) ;
                line != line_end ; ++line)
        {
            std::list<std::string> tokens;
            WhitespaceTokeniser::get_instance()->tokenise(*line, std::back_inserter(tokens));
            if (tokens.size() < 2)
                continue;

            std::list<std::string>::const_iterator t(tokens.begin()), t_end(tokens.end());
            PackageDepAtom::ConstPointer d(new PackageDepAtom(*t++));
            QualifiedPackageName p(d->package());

            PackageUseMaskMap::iterator i(set.find(p));
            if (set.end() == i)
                i = set.insert(std::make_pair(p, std::list<std::pair<PackageDepAtom::ConstPointer,
                            UseFlagName> >())).first;

            for ( ; t != t_end ; ++t)
                i->second.push_back(std::make_pair(d, UseFlagName(*t)));
        }
    }
    catch (const NameError & e)
    {
        Log::get_instance()->message(ll_warning, lc_context, "Loading package.use." + mask_or_force +
                " failed due to exception: " + e.message() + " (" + e.what() + ")");
    }
}

void
Implementation<PortageRepositoryProfile>::load_profile_packages(const FSEntry & dir)
{
    Context context("When loading packages file in '" + stringify(dir) + "':");

    if (! (dir / "packages").exists())
        return;

    LineConfigFile file(dir / "packages");
    for (LineConfigFile::Iterator line(file.begin()), line_end(file.end()) ;
            line != line_end ; ++line)
    {
        if (line->empty())
            continue;

        Context context_line("When reading line '" + *line + "':");

        if ('*' == line->at(0))
            system_lines.insert(line->substr(1));
        else if (0 == line->compare(0, 2, "-*"))
            if (0 == system_lines.erase(line->substr(2)))
                Log::get_instance()->message(ll_qa, lc_context,
                        "Trying to remove packages line '" + line->substr(2) +
                        "' that doesn't exist");
    }
}

void
Implementation<PortageRepositoryProfile>::load_profile_virtuals(const FSEntry & dir)
{
    Context context("When loading virtuals file in '" + stringify(dir) + "':");

    if (! (dir / "virtuals").exists())
        return;

    LineConfigFile file(dir / "virtuals");
    for (LineConfigFile::Iterator line(file.begin()), line_end(file.end()) ;
            line != line_end ; ++line)
    {
        std::vector<std::string> tokens;
        WhitespaceTokeniser::get_instance()->tokenise(*line, std::back_inserter(tokens));
        if (tokens.size() < 2)
            continue;

        QualifiedPackageName v(tokens[0]);
        virtuals.erase(v);
        virtuals.insert(std::make_pair(v, PackageDepAtom::Pointer(new PackageDepAtom(tokens[1]))));
    }
}

void
Implementation<PortageRepositoryProfile>::load_profile_package_mask(const FSEntry & dir)
{
    Context context("When loading package.mask file in '" + stringify(dir) + "':");

    if (! (dir / "package.mask").exists())
        return;

    LineConfigFile file(dir / "package.mask");
    for (LineConfigFile::Iterator line(file.begin()), line_end(file.end()) ;
            line != line_end ; ++line)
    {
        PackageDepAtom::ConstPointer a(new PackageDepAtom(*line));
        package_mask[a->package()].push_back(a);
    }
}

bool
Implementation<PortageRepositoryProfile>::use_mask_or_force(
        const UseFlagName & u, const PackageDatabaseEntry * const e, const std::string & mask_or_force,
        const UseFlagSet & global, const PackageUseMaskMap & package) const
{
    Context context("When querying profile use " + mask_or_force + " status of '" + stringify(u) +
            (e ? "' for '" + stringify(*e) + "'" : "'"));

    if (global.end() != global.find(u))
        return true;

    if (0 == e)
        return false;

    PackageUseMaskMap::const_iterator i(package.find(e->get<pde_name>()));
    if (package.end() == i)
        return false;

    for (std::list<std::pair<PackageDepAtom::ConstPointer, UseFlagName> >::const_iterator
            j(i->second.begin()), j_end(i->second.end()) ; j != j_end ; ++j)
    {
        static int depth(0);
        if (depth > 3)
        {
            Log::get_instance()->message(ll_warning, lc_context,
                    "depth > 3 on entry '" + stringify(*j->first) + "'");

            if (j->first->use_requirements_ptr())
                continue;
            if (u == j->second && match_package(env, j->first, e))
                return true;
        }
        else
        {
            Save<int> save_depth(&depth, depth + 1);
            if (u == j->second && match_package(env, j->first, e))
                return true;
        }
    }

    return false;
}

void
Implementation<PortageRepositoryProfile>::add_use_expand_to_use()
{
    Context context("When adding USE_EXPAND to USE:");

    for (UseFlagSet::const_iterator x(use_expand.begin()), x_end(use_expand.end()) ;
            x != x_end ; ++x)
    {
        std::string lower_x;
        std::transform(x->data().begin(), x->data().end(), std::back_inserter(lower_x),
                &::tolower);

        std::list<std::string> uses;
        WhitespaceTokeniser::get_instance()->tokenise(environment_variables[stringify(*x)],
                std::back_inserter(uses));
        for (std::list<std::string>::const_iterator u(uses.begin()), u_end(uses.end()) ;
                u != u_end ; ++u)
            use[UseFlagName(lower_x + "_" + *u)] = use_enabled;
    }
}

void
Implementation<PortageRepositoryProfile>::load_system_packages()
{
    Context context("When loading system packages:");

    for (std::set<std::string>::const_iterator i(system_lines.begin()),
            i_end(system_lines.end()) ; i != i_end ; ++i)
    {
        Context context_atom("When parsing package '" + *i + "':");
        PackageDepAtom::Pointer atom(new PackageDepAtom(*i));
        atom->set_tag(system_tag);
        system_packages->add_child(atom);
    }
}

void
Implementation<PortageRepositoryProfile>::handle_profile_arch_var()
{
    Context context("When handling profile ARCH variable:");

    std::string arch_s(environment_variables["ARCH"]);
    if (arch_s.empty())
        throw PortageRepositoryConfigurationError("ARCH variable is unset or empty");

    try
    {
        UseFlagName arch(arch_s);

        use[arch] = use_enabled;
        use_force.insert(arch);
        if (use_mask.end() != use_mask.find(arch))
            throw PortageRepositoryConfigurationError("ARCH USE '" + arch_s + "' is use masked");
    }
    catch (const NameError & e)
    {
        throw PortageRepositoryConfigurationError("ARCH variable has invalid value '" + arch_s + "'");
    }
}

PortageRepositoryProfile::PortageRepositoryProfile(
        const Environment * const env, const FSEntryCollection & location) :
    PrivateImplementationPattern<PortageRepositoryProfile>(
            new Implementation<PortageRepositoryProfile>(env, location))
{
}

PortageRepositoryProfile::~PortageRepositoryProfile()
{
}

bool
PortageRepositoryProfile::use_masked(const UseFlagName & u,
        const PackageDatabaseEntry * const e) const
{
    return _imp->use_mask_or_force(u, e, "mask", _imp->use_mask, _imp->package_use_mask);
}

bool
PortageRepositoryProfile::use_forced(const UseFlagName & u,
        const PackageDatabaseEntry * const e) const
{
    return _imp->use_mask_or_force(u, e, "force", _imp->use_force, _imp->package_use_force);
}

UseFlagState
PortageRepositoryProfile::use_state_ignoring_masks(const UseFlagName & u) const
{
    Implementation<PortageRepositoryProfile>::UseMap::const_iterator p(_imp->use.find(u));
    if (_imp->use.end() == p)
        return use_unspecified;
    return p->second;
}

std::string
PortageRepositoryProfile::environment_variable(const std::string & s) const
{
    Implementation<PortageRepositoryProfile>::EnvironmentVariablesMap::const_iterator i(
            _imp->environment_variables.find(s));
    if (_imp->environment_variables.end() == i)
        return "";
    else
        return i->second;
}

AllDepAtom::Pointer
PortageRepositoryProfile::system_packages() const
{
    return _imp->system_packages;
}

PortageRepositoryProfile::UseExpandIterator
PortageRepositoryProfile::begin_use_expand() const
{
    return UseExpandIterator(_imp->use_expand.begin());
}

PortageRepositoryProfile::UseExpandIterator
PortageRepositoryProfile::end_use_expand() const
{
    return UseExpandIterator(_imp->use_expand.end());
}

PortageRepositoryProfile::UseExpandIterator
PortageRepositoryProfile::begin_use_expand_hidden() const
{
    return UseExpandIterator(_imp->use_expand_hidden.begin());
}

PortageRepositoryProfile::UseExpandIterator
PortageRepositoryProfile::end_use_expand_hidden() const
{
    return UseExpandIterator(_imp->use_expand_hidden.end());
}

PortageRepositoryProfile::VirtualsIterator
PortageRepositoryProfile::begin_virtuals() const
{
    return VirtualsIterator(_imp->virtuals.begin());
}

PortageRepositoryProfile::VirtualsIterator
PortageRepositoryProfile::end_virtuals() const
{
    return VirtualsIterator(_imp->virtuals.end());
}

bool
PortageRepositoryProfile::profile_masked(const QualifiedPackageName & n,
        const VersionSpec & v, const RepositoryName & r) const
{
    Implementation<PortageRepositoryProfile>::PackageMaskMap::const_iterator rr(
            _imp->package_mask.find(n));
    if (_imp->package_mask.end() == rr)
        return false;
    else
    {
        PackageDatabaseEntry dbe(n, v, r);
        for (std::list<PackageDepAtom::ConstPointer>::const_iterator k(rr->second.begin()),
                k_end(rr->second.end()) ; k != k_end ; ++k)
            if (match_package(_imp->env, **k, dbe))
                return true;
    }

    return false;
}

