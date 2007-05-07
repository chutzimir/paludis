/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2006, 2007 Ciaran McCreesh <ciaranm@ciaranm.org>
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

#ifndef PALUDIS_GUARD_PALUDIS_TASKS_EXCEPTIONS_HH
#define PALUDIS_GUARD_PALUDIS_TASKS_EXCEPTIONS_HH 1

#include <paludis/util/exception.hh>

namespace paludis
{
    /**
     * Thrown if multiple set targets are specified.
     *
     * \ingroup grpexceptions
     */
    class PALUDIS_VISIBLE MultipleSetTargetsSpecified :
        public Exception
    {
        public:
            MultipleSetTargetsSpecified() throw ();
    };

    /**
     * Thrown if both sets and packages are specified as targets.
     *
     * \ingroup grpexceptions
     */
    class PALUDIS_VISIBLE HadBothPackageAndSetTargets :
        public Exception
    {
        public:
            HadBothPackageAndSetTargets() throw ();
    };
}

#endif
