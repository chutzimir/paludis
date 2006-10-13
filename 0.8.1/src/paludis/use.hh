/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2006 Ciaran McCreesh <ciaranm@ciaranm.org>
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

#ifndef PALUDIS_GUARD_SRC_PALUDIS_USE_HH
#define PALUDIS_GUARD_SRC_PALUDIS_USE_HH 1

#include <paludis/name.hh>
#include <paludis/environment.hh>
#include <paludis/package_database_entry.hh>
#include <paludis/version_metadata.hh>

/**
 * Make a nice string for outputting USE flags.
 */
std::string
make_pretty_use_flags_string(const paludis::Environment * const env, const paludis::PackageDatabaseEntry & p,
        paludis::VersionMetadata::ConstPointer metadata, const paludis::PackageDatabaseEntry * const other_p = 0)
        PALUDIS_ATTRIBUTE((nonnull(1)));

#endif
