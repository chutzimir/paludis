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

#include "fake_repository.hh"

using namespace paludis;

FakeRepository::FakeRepository(const RepositoryName & our_name) :
    FakeRepositoryBase(our_name, RepositoryCapabilities::create()
            .installable_interface(this)
            .installed_interface(0)
            .mask_interface(this)
            .news_interface(0)
            .sets_interface(0)
            .syncable_interface(0)
            .uninstallable_interface(0)
            .use_interface(this)
            .world_interface(0)
            .mirrors_interface(0)
            .environment_variable_interface(0)
            .provides_interface(0)
            .virtuals_interface(0))
{
}

void
FakeRepository::do_install(const QualifiedPackageName &, const VersionSpec &,
        const InstallOptions &) const
{
}

