#!/bin/bash
# vim: set sw=4 sts=4 et :

# Copyright (c) 2006, 2007, 2008 Ciaran McCreesh
#
# This file is part of the Paludis package manager. Paludis is free software;
# you can redistribute it and/or modify it under the terms of the GNU General
# Public License, version 2, as published by the Free Software Foundation.
#
# Paludis is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 59 Temple
# Place, Suite 330, Boston, MA  02111-1307  USA

builtin_prepare()
{
    export ROOT="${ROOT//+(\/)//}"

    local a
    for a in PF CATEGORY PALUDIS_TMPDIR  ; do
        [[ -z "${!a}" ]] && die "\$${a} unset or empty"
    done

    if [[ -e "${PALUDIS_TMPDIR}/${CATEGORY}/${PF}" ]] ; then
        if type -p chflags &>/dev/null; then
            chflags -R 0 "${PALUDIS_TMPDIR}/${CATEGORY}/${PF}" || die "Couldn't remove flags from workdir"
        fi
        rm -fr "${PALUDIS_TMPDIR}/${CATEGORY}/${PF}" || die "Couldn't remove previous work"
    fi
}

exheres_internal_prepare()
{
    if hasq "prepare" ${RESTRICT} ; then
        ebuild_section "Skipping builtin_prepare (RESTRICT)"
    elif hasq "prepare" ${SKIP_FUNCTIONS} ; then
        ebuild_section "Skipping builtin_prepare (SKIP_FUNCTIONS)"
    else
        ebuild_section "Starting builtin_prepare"
        builtin_prepare
        ebuild_section "Done builtin_prepare"
    fi
}

ebuild_f_prepare()
{
    exheres_internal_prepare ""
}

