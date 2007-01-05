#!/bin/bash
# vim: set sw=4 sts=4 et :

# Copyright (c) 2006 Ciaran McCreesh <ciaranm@ciaranm.org>

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

builtin_init_bin()
{
    export ROOT="${ROOT//+(\/)//}"

    local a
    for a in P PV PR PN PVR PF CATEGORY PKGDIR \
        KV PALUDIS_TMPDIR PALUDIS_EBUILD_LOG_LEVEL PALUDIS_EBUILD_DIR \
        USERLAND KERNEL ARCH CHOST PALUDIS_COMMAND ROOT ; do
        [[ -z "${!a}" ]] && die "\$${a} unset or empty"
        declare -r ${a}="${!a}"
    done

    if [[ -e "${PALUDIS_TMPDIR}/${CATEGORY}/${PF}" ]] ; then
        rm -fr "${PALUDIS_TMPDIR}/${CATEGORY}/${PF}" || die "Couldn't remove previous work"
    fi

    export WORKDIR="${PALUDIS_TMPDIR}/${CATEGORY}/${PF}/work"
    mkdir -p "${WORKDIR}" || die "Couldn't create \$WORKDIR (\"${WORKDIR}\")"
    declare -r WORKDIR="${WORKDIR}"

    export T="${PALUDIS_TMPDIR}/${CATEGORY}/${PF}/temp/"
    mkdir -p "${T}" || die "Couldn't create \$T (\"${T}\")"
    declare -r T="${T}"
    export HOME="${T}"

    export D="${PALUDIS_TMPDIR}/${CATEGORY}/${PF}/image/"
    export D="${D//+(\/)//}"
    mkdir -p "${D}" || die "Couldn't create \$D (\"${D}\")"
    declare -r D="${D}"

    export IMAGE="${D}"
    declare -r IMAGE="${IMAGE}"

    export S="${WORKDIR}/${P}"
}

ebuild_f_init_bin()
{
    if hasq "init_bin" ${RESTRICT} ; then
        ebuild_section "Skipping builtin_init_bin (RESTRICT)"
    elif hasq "init_bin" ${SKIP_FUNCTIONS} ; then
        ebuild_section "Skipping builtin_init_bin (SKIP_FUNCTIONS)"
    else
        ebuild_section "Starting builtin_init_bin"
        builtin_init_bin
        ebuild_section "Done builtin_init_bin"
    fi
}


