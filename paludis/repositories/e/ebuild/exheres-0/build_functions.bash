#!/usr/bin/env bash
# vim: set sw=4 sts=4 et :

# Copyright (c) 2006, 2007, 2008, 2009 Ciaran McCreesh
# Copyright (c) 2008 Bo Ørsted Andresen
# Copyright (c) 2009 David Leverton
#
# Based in part upon ebuild.sh from Portage, which is Copyright 1995-2005
# Gentoo Foundation and distributed under the terms of the GNU General
# Public License v2.
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

alias die_unless_nonfatal='paludis_die_unless_nonfatal_func "$FUNCNAME" "$LINENO"'
alias assert_unless_nonfatal='_pipestatus="${PIPESTATUS[*]}"; [[ -z "${_pipestatus//[ 0]/}" ]] || paludis_die_unless_nonfatal_func "$FUNCNAME" "$LINENO" "$_pipestatus"'

nonfatal()
{
    PALUDIS_FAILURE_IS_FATAL= PALUDIS_FAILURE_IS_NONFATAL=yes "${@}"
}

is_nonfatal()
{
    [[ -n ${PALUDIS_FAILURE_IS_NONFATAL} ]]
}

expatch()
{
    if [[ "${EBUILD_PHASE}" != "prepare" ]] ; then
        die "expatch called in EBUILD_PHASE ${EBUILD_PHASE}"
    fi

    local recognise= patchlevel= options=() cmd= appliedpatches=0 dirpatches=()

    if [[ ${1} == "--recognised-suffixes" ]]; then
        recognise=true
        shift
    fi

    while [[ $# -gt 0 ]]; do
        if [[ ${1} == -p*([[:digit:]]) ]]; then
            patchlevel="${1}"
        elif [[ ${1} == -+([^[:space:]]) ]]; then
            options+=("${1}")
        elif [[ -d ${1} ]]; then
            dirpatches=("${1}"/*)
            [[ -f ${dirpatches[0]} ]] || die "expatch called with empty directory $1"
            expatch --recognised-suffixes ${patchlevel} "${options[@]}" "${1}"/* || return 247
            ((appliedpatches++))
        else
            case "${1}" in
                *.bz2)
                cmd="bzip2 -dc"
                ;;
                *.gz|*.Z|*.z)
                cmd="gzip -dc"
                ;;
                *.zip|*.ZIP|*.jar)
                cmd="unzip -p"
                ;;
                *.diff|*.patch)
                cmd="cat"
                ;;
                *)
                if [[ -n ${recognise} ]]; then
                    shift
                    continue
                else
                    cmd="cat"
                fi
                ;;
            esac

            echo "${cmd} -- '${1}' | patch -s -f ${patchlevel:--p1} ${options[@]}" 1>&2
            ${cmd} -- "${1}" | patch -s -f ${patchlevel:--p1} "${options[@]}"
            paludis_assert_unless_nonfatal "applying '${1}' failed" || return 247
            ((appliedpatches++))
        fi
        shift
    done
    # Die if no patches were applied and no directories were supplied. Since
    # directories get handled recursively by separate instances of expatch we cannot
    # reliably count applied patches when directories were supplied.
    [[ ${appliedpatches} -gt 0 || -n ${recognise} ]] || paludis_die_unless_nonfatal "No patches applied." || return 247
}

econf()
{
    if [[ "${EBUILD_PHASE}" != "configure" ]] ; then
        die "econf called in EBUILD_PHASE ${EBUILD_PHASE}"
    fi

    local LOCAL_EXTRA_ECONF="${EXTRA_ECONF}"
    local LOCAL_ECONF_WRAPPER="${ECONF_WRAPPER}"

    [[ -z "${ECONF_SOURCE}" ]] && ECONF_SOURCE=.

    if [[ -x "${ECONF_SOURCE}/configure" ]] ; then
        if [[ -d /usr/share/gnuconfig ]] ; then
            local f
            find "${WORKBASE}" -type f -name config.guess -or -name config.sub | while read f; do
                echo "econf: updating ${f} with /usr/share/gnuconfig/${f##*/}"
                cp -f "/usr/share/gnuconfig/${f##*/}" "${f}"
            done
        fi

        [[ -z "${CBUILD}" ]] || LOCAL_EXTRA_ECONF="--build=${CBUILD} ${LOCAL_EXTRA_ECONF}"
        [[ -z "${CTARGET}" ]] || LOCAL_EXTRA_ECONF="--target=${CTARGET} ${LOCAL_EXTRA_ECONF}"

        # If the ebuild passed in --prefix, use that to set --libdir. KDE at least needs this.

        ECONF_PREFIX=/usr
        for i in "$@"; do
            if [[ ${i} == --prefix=* ]]; then
                ECONF_PREFIX=${i#--prefix=}
            elif [[ ${i} == --exec-prefix=* ]]; then
                ECONF_PREFIX=${i#--exec-prefix=}
            fi
        done

        local libcmd=
        if [[ -n "${ABI}" ]] ; then
            local v="LIBDIR_${ABI}"
            if [[ -n "${!v}" ]] ; then
                libcmd="--libdir=${ECONF_PREFIX}/$(ebuild_get_libdir)"
            fi
        fi

        echo ${LOCAL_ECONF_WRAPPER} "${ECONF_SOURCE}"/configure \
            --prefix=/usr \
            --host=${CHOST} \
            --mandir=/usr/share/man \
            --infodir=/usr/share/info \
            --datadir=/usr/share \
            --sysconfdir=/etc \
            --localstatedir=/var/lib \
            --disable-dependency-tracking \
            --enable-fast-install \
            ${libcmd} "$@" ${LOCAL_EXTRA_ECONF} 1>&2

        ${LOCAL_ECONF_WRAPPER} "${ECONF_SOURCE}"/configure \
            --prefix=/usr \
            --host=${CHOST} \
            --mandir=/usr/share/man \
            --infodir=/usr/share/info \
            --datadir=/usr/share \
            --sysconfdir=/etc \
            --localstatedir=/var/lib \
            --disable-dependency-tracking \
            --enable-fast-install \
            ${libcmd} "$@" ${LOCAL_EXTRA_ECONF} || paludis_die_unless_nonfatal "econf failed" || return 247

    else
        paludis_die_unless_nonfatal "No configure script for econf" || return 247
    fi
}

einstall()
{
    if [[ "${EBUILD_PHASE}" != "install" ]] ; then
        die "einstall called in EBUILD_PHASE ${EBUILD_PHASE}"
    fi

    if [[ -f Makefile ]] || [[ -f makefile ]] || [[ -f GNUmakefile ]] ; then
        local makecmd=""
        type -p gmake &>/dev/null && makecmd="gmake" || makecmd="make"
        local cmd="${EINSTALL_WRAPPER} ${makecmd} prefix=${IMAGE}/usr"
        cmd="${cmd} mandir=${IMAGE}/usr/share/man"
        cmd="${cmd} infodir=${IMAGE}/usr/share/info"
        cmd="${cmd} datadir=${IMAGE}/usr/share"
        cmd="${cmd} sysconfdir=${IMAGE}/etc"
        cmd="${cmd} localstatedir=${IMAGE}/var/lib"
        cmd="${cmd} libdir=${IMAGE}/usr/$(ebuild_get_libdir)"
        cmd="${cmd} ${EXTRA_EINSTALL} ${@} install"
        echo "${cmd}" 1>&2
        ${cmd} || paludis_die_unless_nonfatal "einstall failed" || return 247
    else
        paludis_die_unless_nonfatal "No Makefile for einstall" || return 247
    fi
}

emagicdocs()
{
    done_docs=
    old_set=$(shopt | grep 'nocaseglob[[:space:]]*on')
    shopt -s nocaseglob
    for d in '' "${DEFAULT_SRC_INSTALL_EXTRA_SUBDIRS[@]}" ; do
        if [[ -n ${d} ]]; then
            [[ -d ${d} ]] || die "${d} is not a dir"
            pushd "${d}" > /dev/null || die "Failed to enter ${d}"
            local docdesttree="${DOCDESTTREE}"
            docinto "${d}"
        fi
        for f in README Change{,s,Log} AUTHORS NEWS TODO ABOUT THANKS {KNOWN_,}BUGS SUBMITTING \
            HACKING FAQ CREDITS PKG-INFO HISTORY PACKAGING MAINTAINER{,S} CONTRIBUT{E,OR,ORS} RELEASE \
            ANNOUNCE PORTING NOTES PROBLEMS NOTICE "${DEFAULT_SRC_INSTALL_EXTRA_DOCS[@]}"; do
            for p in "${DEFAULT_SRC_INSTALL_EXTRA_PREFIXES[@]}" '' ; do
                for doc in "${p}"*([[:digit:]])"${f}"{,+([._-])*} ; do
                    if [[ -s "${doc}" ]] ; then
                        for e in "${DEFAULT_SRC_INSTALL_EXCLUDE[@]}" ; do
                            [[ ${doc} == ${e} ]] && continue 2
                        done
                        done_docs="${done_docs} ${d%/}${d:+/}${doc}"
                        dodoc "${doc}"
                    fi
                done
            done
        done
        if [[ -n ${d} ]]; then
            docinto "${docdesttree}"
            popd > /dev/null || die "Failed to leave ${d}"
        fi
    done
    if [[ -n "${done_docs}" ]] ; then
        echo "Installed docs ${done_docs# }"
    else
        echo "Didn't find any docs to install"
    fi
    [[ -n ${old_set} ]] || shopt -u nocaseglob
}
