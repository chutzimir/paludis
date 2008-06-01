/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2005, 2006, 2007, 2008 Ciaran McCreesh
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

#include <paludis/environment_implementation.hh>
#include <paludis/package_id.hh>
#include <paludis/metadata_key.hh>
#include <paludis/package_database.hh>
#include <paludis/util/visitor-impl.hh>
#include <paludis/util/log.hh>
#include <paludis/util/save.hh>
#include <paludis/util/set.hh>
#include <paludis/util/make_shared_ptr.hh>
#include <paludis/util/system.hh>
#include <paludis/util/kc.hh>
#include <paludis/hook.hh>
#include <paludis/distribution.hh>
#include <paludis/selection.hh>
#include <algorithm>
#include "config.h"

using namespace paludis;

EnvironmentImplementation::~EnvironmentImplementation()
{
}


std::tr1::shared_ptr<const FSEntrySequence>
EnvironmentImplementation::bashrc_files() const
{
    return make_shared_ptr(new FSEntrySequence);
}

std::tr1::shared_ptr<const FSEntrySequence>
EnvironmentImplementation::syncers_dirs() const
{
    std::tr1::shared_ptr<FSEntrySequence> result(new FSEntrySequence);
    result->push_back(FSEntry(DATADIR "/paludis/syncers"));
    result->push_back(FSEntry(LIBEXECDIR "/paludis/syncers"));
    return result;
}

std::tr1::shared_ptr<const FSEntrySequence>
EnvironmentImplementation::fetchers_dirs() const
{
    std::tr1::shared_ptr<FSEntrySequence> result(new FSEntrySequence);
    std::string fetchers_dir(getenv_with_default("PALUDIS_FETCHERS_DIR", ""));
    if (fetchers_dir.empty())
    {
        result->push_back(FSEntry(DATADIR "/paludis/fetchers"));
        result->push_back(FSEntry(LIBEXECDIR "/paludis/fetchers"));
    }
    else
        result->push_back(FSEntry(fetchers_dir));
    return result;
}

std::tr1::shared_ptr<const DestinationsSet>
EnvironmentImplementation::default_destinations() const
{
    std::tr1::shared_ptr<DestinationsSet> result(new DestinationsSet);

    for (PackageDatabase::RepositoryConstIterator r(package_database()->begin_repositories()),
            r_end(package_database()->end_repositories()) ;
            r != r_end ; ++r)
        if ((**r)[k::destination_interface()])
            if ((**r)[k::destination_interface()]->is_default_destination())
                result->insert(*r);

    return result;
}

std::tr1::shared_ptr<SetSpecTree::ConstItem>
EnvironmentImplementation::set(const SetName & s) const
{
    {
        std::tr1::shared_ptr<SetSpecTree::ConstItem> l(local_set(s));
        if (l)
        {
            Log::get_instance()->message("environment_implementation.local_set", ll_debug, lc_context) << "Set '" << s << "' is a local set";
            return l;
        }
    }

    std::tr1::shared_ptr<ConstTreeSequence<SetSpecTree, AllDepSpec> > result;

    /* these sets always exist, even if empty */
    if (s.data() == "everything" || s.data() == "system" || s.data() == "world" || s.data() == "security")
    {
        Log::get_instance()->message("environment_implementation.standard_set", ll_debug, lc_context) << "Set '" << s << "' is a standard set";
        result.reset(new ConstTreeSequence<SetSpecTree, AllDepSpec>(std::tr1::shared_ptr<AllDepSpec>(new AllDepSpec)));
    }

    for (PackageDatabase::RepositoryConstIterator r(package_database()->begin_repositories()),
            r_end(package_database()->end_repositories()) ;
            r != r_end ; ++r)
    {
        if (! (**r)[k::sets_interface()])
            continue;

        std::tr1::shared_ptr<SetSpecTree::ConstItem> add((**r)[k::sets_interface()]->package_set(s));
        if (add)
        {
            Log::get_instance()->message("environment_implementation.set_found_in_repository", ll_debug, lc_context)
                << "Set '" << s << "' found in '" << (*r)->name() << "'";
            if (! result)
                result.reset(new ConstTreeSequence<SetSpecTree, AllDepSpec>(std::tr1::shared_ptr<AllDepSpec>(new AllDepSpec)));
            result->add(add);
        }
    }

    if ("everything" == s.data() || "world" == s.data())
        result->add(make_shared_ptr(new TreeLeaf<SetSpecTree, NamedSetDepSpec>(
                        make_shared_ptr(new NamedSetDepSpec(SetName("system"))))));

    if ("world" == s.data())
    {
        std::tr1::shared_ptr<SetSpecTree::ConstItem> w(world_set());
        if (w)
            result->add(w);
    }

    if (! result)
        Log::get_instance()->message("environment_implementation.no_match_for_set", ll_debug, lc_context) << "No match for set '" << s << "'";
    return result;
}

bool
EnvironmentImplementation::query_use(const UseFlagName & f, const PackageID & e) const
{
    if ((*e.repository())[k::use_interface()])
    {
        if ((*e.repository())[k::use_interface()]->query_use_mask(f, e))
            return false;
        if ((*e.repository())[k::use_interface()]->query_use_force(f, e))
            return true;

        switch ((*e.repository())[k::use_interface()]->query_use(f, e))
        {
            case use_disabled:
            case use_unspecified:
                return false;

            case use_enabled:
                return true;

            case last_use:
                ;
        }

        throw InternalError(PALUDIS_HERE, "bad state");
    }
    else
        return false;
}

std::string
EnvironmentImplementation::default_distribution() const
{
    static const std::string result(getenv_with_default("PALUDIS_DISTRIBUTION", DEFAULT_DISTRIBUTION));
    return result;
}

std::tr1::shared_ptr<const SetNameSet>
EnvironmentImplementation::set_names() const
{
    return make_shared_ptr(new SetNameSet);
}

bool
EnvironmentImplementation::is_paludis_package(const QualifiedPackageName & n) const
{
    return stringify(n) == (*DistributionData::get_instance()->distribution_from_string(default_distribution()))[k::paludis_package()];
}

std::tr1::shared_ptr<PackageIDSequence>
EnvironmentImplementation::operator[] (const Selection & selection) const
{
    return selection.perform_select(this);
}

