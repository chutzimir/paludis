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

#include <paludis/util/iterator.hh>
#include <paludis/util/tokeniser.hh>
#include <paludis/version_metadata.hh>
#include <paludis/portage_dep_parser.hh>
#include <vector>

/** \file
 * Implementation of VersionMetadata.
 *
 * \ingroup grpversions
 */

using namespace paludis;

VersionMetadataDeps::VersionMetadataDeps(ParserFunction p) :
    MakeSmartRecord<VersionMetadataDepsTag>::Type(MakeSmartRecord<VersionMetadataDepsTag>::Type::create((
                    param<vmd_build_depend_string>(""),
                    param<vmd_run_depend_string>(""),
                    param<vmd_post_depend_string>(""),
                    param<vmd_parser>(p))))
{
}

DepAtom::ConstPointer
VersionMetadataDeps::build_depend() const
{
    return get<vmd_parser>()(get<vmd_build_depend_string>());
}

DepAtom::ConstPointer
VersionMetadataDeps::run_depend() const
{
    return get<vmd_parser>()(get<vmd_run_depend_string>());
}

DepAtom::ConstPointer
VersionMetadataDeps::post_depend() const
{
    return get<vmd_parser>()(get<vmd_post_depend_string>());
}

VersionMetadata::CRAN::CRAN(ParserFunction f) :
    VersionMetadata(f, &_c, 0, 0),
    _c(CRANVersionMetadata::create((
                    param<cranvm_keywords>(""),
                    param<cranvm_package>(""),
                    param<cranvm_version>(""),
                    param<cranvm_is_bundle>(false))))
{
}

VersionMetadata::Ebuild::Ebuild(ParserFunction f) :
    VersionMetadata(f, 0, &_e, 0),
    _e()
{
}

VersionMetadata::Ebin::Ebin(ParserFunction f) :
    VersionMetadata(f, 0, &_e, &_eb),
    _e(),
    _eb()
{
}

VersionMetadata::~VersionMetadata()
{
}

VersionMetadata::VersionMetadata(ParserFunction p) :
    MakeSmartRecord<VersionMetadataTag>::Type(MakeSmartRecord<VersionMetadataTag>::Type::create((
                    param<vm_slot>(SlotName("unset")),
                    param<vm_deps>(VersionMetadataDeps(p)),
                    param<vm_homepage>(""),
                    param<vm_license>(""),
                    param<vm_description>(""),
                    param<vm_eapi>("UNSET"),
                    param<vm_license>("")
                    ))),
    _cran_if(0),
    _ebuild_if(0),
    _ebin_if(0)
{
}

VersionMetadata::VersionMetadata(ParserFunction p,
        CRANVersionMetadata * cran_if,
        EbuildVersionMetadata * ebuild_if,
        EbinVersionMetadata * ebin_if) :
    MakeSmartRecord<VersionMetadataTag>::Type(MakeSmartRecord<VersionMetadataTag>::Type::create((
                    param<vm_slot>(SlotName("unset")),
                    param<vm_deps>(VersionMetadataDeps(p)),
                    param<vm_homepage>(""),
                    param<vm_license>(""),
                    param<vm_description>(""),
                    param<vm_eapi>("UNSET"),
                    param<vm_license>("")
                    ))),
     _cran_if(cran_if),
     _ebuild_if(ebuild_if),
     _ebin_if(ebin_if)
{
}

EbuildVersionMetadata::EbuildVersionMetadata() :
    MakeSmartRecord<EbuildVersionMetadataTag>::Type((EbuildVersionMetadata::create((
                        param<evm_src_uri>(""),
                        param<evm_restrict>(""),
                        param<evm_keywords>(""),
                        param<evm_inherited>(""),
                        param<evm_iuse>(""),
                        param<evm_inherited>(""),
                        param<evm_provide>(""),
                        param<evm_virtual>("")))))
{
}

EbinVersionMetadata::EbinVersionMetadata() :
    MakeSmartRecord<EbinVersionMetadataTag>::Type((EbinVersionMetadata::create((
                        param<ebvm_bin_uri>(""),
                        param<ebvm_src_repository>(RepositoryName("unset"))))))
{
}

DepAtom::ConstPointer
EbuildVersionMetadata::provide() const
{
    return PortageDepParser::parse(get<evm_provide>(), PortageDepParserPolicy<PackageDepAtom,
            false>::get_instance());
}

DepAtom::ConstPointer
VersionMetadata::license() const
{
    return PortageDepParser::parse(get<vm_license>(), PortageDepParserPolicy<PlainTextDepAtom,
            true>::get_instance());
}
