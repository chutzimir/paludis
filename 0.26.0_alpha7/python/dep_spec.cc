/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2007 Piotr Jaroszyński
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

#include "dep_spec.hh"
#include <python/paludis_python.hh>
#include <python/exception.hh>
#include <python/options.hh>
#include <python/nice_names-nn.hh>

#include <paludis/dep_tag.hh>
#include <paludis/version_requirements.hh>
#include <paludis/use_requirements.hh>
#include <paludis/util/clone-impl.hh>
#include <paludis/util/private_implementation_pattern-impl.hh>
#include <paludis/util/visitor-impl.hh>
#include <paludis/util/save.hh>
#include <paludis/util/stringify.hh>
#include <paludis/util/tr1_type_traits.hh>
#include <paludis/util/wrapped_forward_iterator-impl.hh>
#include <paludis/util/make_shared_ptr.hh>

#include <list>

using namespace paludis;
using namespace paludis::python;
namespace bp = boost::python;

template class ConstVisitor<PythonDepSpecVisitorTypes>;
template class ConstAcceptInterface<PythonDepSpecVisitorTypes>;

template class ConstAcceptInterfaceVisitsThis<PythonDepSpecVisitorTypes, PythonAllDepSpec>;
template class ConstAcceptInterfaceVisitsThis<PythonDepSpecVisitorTypes, PythonAnyDepSpec>;
template class ConstAcceptInterfaceVisitsThis<PythonDepSpecVisitorTypes, PythonUseDepSpec>;
template class ConstAcceptInterfaceVisitsThis<PythonDepSpecVisitorTypes, PythonPackageDepSpec>;
template class ConstAcceptInterfaceVisitsThis<PythonDepSpecVisitorTypes, PythonBlockDepSpec>;
template class ConstAcceptInterfaceVisitsThis<PythonDepSpecVisitorTypes, PythonPlainTextDepSpec>;
template class ConstAcceptInterfaceVisitsThis<PythonDepSpecVisitorTypes, PythonSimpleURIDepSpec>;
template class ConstAcceptInterfaceVisitsThis<PythonDepSpecVisitorTypes, PythonFetchableURIDepSpec>;
template class ConstAcceptInterfaceVisitsThis<PythonDepSpecVisitorTypes, PythonLicenseDepSpec>;
template class ConstAcceptInterfaceVisitsThis<PythonDepSpecVisitorTypes, PythonURILabelsDepSpec>;
template class ConstAcceptInterfaceVisitsThis<PythonDepSpecVisitorTypes, PythonDependencyLabelsDepSpec>;
template class ConstAcceptInterfaceVisitsThis<PythonDepSpecVisitorTypes, PythonNamedSetDepSpec>;

template class Visits<const PythonAllDepSpec>;
template class Visits<const PythonAnyDepSpec>;
template class Visits<const PythonUseDepSpec>;
template class Visits<const PythonPackageDepSpec>;
template class Visits<const PythonBlockDepSpec>;
template class Visits<const PythonPlainTextDepSpec>;
template class Visits<const PythonSimpleURIDepSpec>;
template class Visits<const PythonFetchableURIDepSpec>;
template class Visits<const PythonLicenseDepSpec>;
template class Visits<const PythonURILabelsDepSpec>;
template class Visits<const PythonDependencyLabelsDepSpec>;
template class Visits<const PythonNamedSetDepSpec>;

template class WrappedForwardIterator<PythonCompositeDepSpec::ConstIteratorTag,
         const tr1::shared_ptr<const PythonDepSpec> >;

PythonDepSpec::PythonDepSpec()
{
}

PythonDepSpec::~PythonDepSpec()
{
}

const PythonUseDepSpec *
PythonDepSpec::as_use_dep_spec() const
{
    return 0;
}

const PythonPackageDepSpec *
PythonDepSpec::as_package_dep_spec() const
{
    return 0;
}

namespace paludis
{
    template<>
    struct Implementation<PythonCompositeDepSpec>
    {
        std::list<tr1::shared_ptr<const PythonDepSpec> > children;
    };

    template<>
    struct Implementation<PythonPackageDepSpec>
    {
        tr1::shared_ptr<const QualifiedPackageName> package_ptr;
        tr1::shared_ptr<const CategoryNamePart> category_name_part_ptr;
        tr1::shared_ptr<const PackageNamePart> package_name_part_ptr;
        tr1::shared_ptr<VersionRequirements> version_requirements;
        VersionRequirementsMode version_requirements_mode;
        tr1::shared_ptr<const SlotName> slot;
        tr1::shared_ptr<const RepositoryName> repository;
        tr1::shared_ptr<const UseRequirements> use_requirements;
        tr1::shared_ptr<const DepTag> tag;
        const std::string str;

        Implementation(
                const tr1::shared_ptr<const QualifiedPackageName> & q,
                const tr1::shared_ptr<const CategoryNamePart> & c,
                const tr1::shared_ptr<const PackageNamePart> & p,
                const tr1::shared_ptr<VersionRequirements> & v,
                const VersionRequirementsMode m,
                const tr1::shared_ptr<const SlotName> & s,
                const tr1::shared_ptr<const RepositoryName> & r,
                const tr1::shared_ptr<const UseRequirements> & u,
                const tr1::shared_ptr<const DepTag> & t,
                const std::string & st) :
            package_ptr(q),
            category_name_part_ptr(c),
            package_name_part_ptr(p),
            version_requirements(v),
            version_requirements_mode(m),
            slot(s),
            repository(r),
            use_requirements(u),
            tag(t),
            str(st)
        {
        }
    };
}

PythonCompositeDepSpec::PythonCompositeDepSpec() :
    PrivateImplementationPattern<PythonCompositeDepSpec>(new Implementation<PythonCompositeDepSpec>)
{
}

PythonCompositeDepSpec::~PythonCompositeDepSpec()
{
}

void
PythonCompositeDepSpec::add_child(const tr1::shared_ptr<const PythonDepSpec> c)
{
    _imp->children.push_back(c);
}

PythonCompositeDepSpec::ConstIterator
PythonCompositeDepSpec::begin() const
{
    return ConstIterator(_imp->children.begin());
}

PythonCompositeDepSpec::ConstIterator
PythonCompositeDepSpec::end() const
{
    return ConstIterator(_imp->children.end());
}

PythonAnyDepSpec::PythonAnyDepSpec()
{
}

PythonAnyDepSpec::PythonAnyDepSpec(const AnyDepSpec &)
{
}

PythonAllDepSpec::PythonAllDepSpec()
{
}

PythonAllDepSpec::PythonAllDepSpec(const AllDepSpec &)
{
}

PythonUseDepSpec::PythonUseDepSpec(const UseFlagName & our_flag, bool is_inverse) :
    _flag(our_flag),
    _inverse(is_inverse)
{
}

PythonUseDepSpec::PythonUseDepSpec(const UseDepSpec & d) :
    _flag(d.flag()),
    _inverse(d.inverse())
{
}

const PythonUseDepSpec *
PythonUseDepSpec::as_use_dep_spec() const
{
    return this;
}

UseFlagName
PythonUseDepSpec::flag() const
{
    return _flag;
}

bool
PythonUseDepSpec::inverse() const
{
    return _inverse;
}

PythonStringDepSpec::PythonStringDepSpec(const std::string & s) :
    _str(s)
{
}

PythonStringDepSpec::PythonStringDepSpec(const StringDepSpec & d) :
    _str(d.text())
{
}

PythonStringDepSpec::~PythonStringDepSpec()
{
}

void
PythonStringDepSpec::set_text(const std::string & t)
{
    _str = t;
}

std::string
PythonStringDepSpec::text() const
{
    return _str;
}

template <typename T_>
tr1::shared_ptr<T_>
deep_copy(const tr1::shared_ptr<const T_> & x)
{
    if (x)
        return tr1::shared_ptr<T_>(new T_(*x));
    else
        return tr1::shared_ptr<T_>();
}

PythonPackageDepSpec::PythonPackageDepSpec(const PackageDepSpec & p) :
    PythonStringDepSpec(p.text()),
    PrivateImplementationPattern<PythonPackageDepSpec>(new Implementation<PythonPackageDepSpec>(
                deep_copy(p.package_ptr()),
                deep_copy(p.category_name_part_ptr()),
                deep_copy(p.package_name_part_ptr()),
                make_shared_ptr(new VersionRequirements),
                p.version_requirements_mode(),
                deep_copy(p.slot_ptr()),
                deep_copy(p.repository_ptr()),
                deep_copy(p.use_requirements_ptr()),
                p.tag(),
                stringify(p)))
{
    if (p.version_requirements_ptr())
    {
        std::copy(p.version_requirements_ptr()->begin(), p.version_requirements_ptr()->end(),
            _imp->version_requirements->back_inserter());
    }
}

PythonPackageDepSpec::PythonPackageDepSpec(const PythonPackageDepSpec & p) :
    PythonStringDepSpec(p.text()),
    PrivateImplementationPattern<PythonPackageDepSpec>(new Implementation<PythonPackageDepSpec>(
                deep_copy(p.package_ptr()),
                deep_copy(p.category_name_part_ptr()),
                deep_copy(p.package_name_part_ptr()),
                make_shared_ptr(new VersionRequirements),
                p.version_requirements_mode(),
                deep_copy(p.slot_ptr()),
                deep_copy(p.repository_ptr()),
                deep_copy(p.use_requirements_ptr()),
                p.tag(),
                p.py_str()))
{
    std::copy(p.version_requirements_ptr()->begin(), p.version_requirements_ptr()->end(),
            _imp->version_requirements->back_inserter());
}

PythonPackageDepSpec::~PythonPackageDepSpec()
{
}

PythonPackageDepSpec::operator PackageDepSpec() const
{
    PartiallyMadePackageDepSpec p;

    if (package_ptr())
        p.package(*package_ptr());

    if (category_name_part_ptr())
        p.category_name_part(*category_name_part_ptr());

    if (package_name_part_ptr())
        p.package_name_part(*package_name_part_ptr());

    p.version_requirements_mode(version_requirements_mode());

    if (slot_ptr())
        p.slot(*slot_ptr());

    if (repository_ptr())
        p.repository(*repository_ptr());

    if (use_requirements_ptr())
    {
        for (UseRequirements::ConstIterator i(use_requirements_ptr()->begin()),
                i_end(use_requirements_ptr()->end()) ; i != i_end ; ++i)
            p.use_requirement(*i);
    }

    if (version_requirements_ptr())
    {
        for (VersionRequirements::ConstIterator i(version_requirements_ptr()->begin()),
                i_end(version_requirements_ptr()->end()) ; i != i_end ; ++i)
            p.version_requirement(*i);
    }

    return p.to_package_dep_spec();
}


PythonPackageDepSpec::operator tr1::shared_ptr<PackageDepSpec>() const
{
    return make_shared_ptr(new PackageDepSpec(*this));
}

const PythonPackageDepSpec *
PythonPackageDepSpec::as_package_dep_spec() const
{
    return this;
}

const tr1::shared_ptr<const PythonPackageDepSpec>
PythonPackageDepSpec::without_use_requirements() const
{
    PackageDepSpec p(*this);

    return make_shared_ptr(new PythonPackageDepSpec(*p.without_use_requirements()));
}

tr1::shared_ptr<const QualifiedPackageName>
PythonPackageDepSpec::package_ptr() const
{
    return _imp->package_ptr;
}

tr1::shared_ptr<const PackageNamePart>
PythonPackageDepSpec::package_name_part_ptr() const
{
    return _imp->package_name_part_ptr;
}

tr1::shared_ptr<const CategoryNamePart>
PythonPackageDepSpec::category_name_part_ptr() const
{
    return _imp->category_name_part_ptr;
}

tr1::shared_ptr<const VersionRequirements>
PythonPackageDepSpec::version_requirements_ptr() const
{
    return _imp->version_requirements;
}

VersionRequirementsMode
PythonPackageDepSpec::version_requirements_mode() const
{
    return _imp->version_requirements_mode;
}

void
PythonPackageDepSpec::set_version_requirements_mode(const VersionRequirementsMode m)
{
    _imp->version_requirements_mode = m;
}

tr1::shared_ptr<const SlotName>
PythonPackageDepSpec::slot_ptr() const
{
    return _imp->slot;
}

tr1::shared_ptr<const RepositoryName>
PythonPackageDepSpec::repository_ptr() const
{
    return _imp->repository;
}

tr1::shared_ptr<const UseRequirements>
PythonPackageDepSpec::use_requirements_ptr() const
{
    return _imp->use_requirements;
}

tr1::shared_ptr<const DepTag>
PythonPackageDepSpec::tag() const
{
    return _imp->tag;
}

std::string
PythonPackageDepSpec::py_str() const
{
    return _imp->str;
}

void
PythonPackageDepSpec::set_tag(const tr1::shared_ptr<const DepTag> & s)
{
    _imp->tag = s;
}

PythonPlainTextDepSpec::PythonPlainTextDepSpec(const std::string & s) :
    PythonStringDepSpec(s)
{
}

PythonPlainTextDepSpec::PythonPlainTextDepSpec(const PlainTextDepSpec & d) :
    PythonStringDepSpec(d.text())
{
}

PythonPlainTextDepSpec::PythonPlainTextDepSpec(const PythonPlainTextDepSpec & d) :
    PythonStringDepSpec(d.text())
{
}

PythonNamedSetDepSpec::PythonNamedSetDepSpec(const SetName & s) :
    PythonStringDepSpec(stringify(s)),
    _name(s)
{
}

const SetName
PythonNamedSetDepSpec::name() const
{
    return _name;
}

PythonNamedSetDepSpec::PythonNamedSetDepSpec(const NamedSetDepSpec & d) :
    PythonStringDepSpec(d.text()),
    _name(d.name())
{
}

PythonLicenseDepSpec::PythonLicenseDepSpec(const std::string & s) :
    PythonStringDepSpec(s)
{
}

PythonLicenseDepSpec::PythonLicenseDepSpec(const LicenseDepSpec & d) :
    PythonStringDepSpec(d.text())
{
}

PythonSimpleURIDepSpec::PythonSimpleURIDepSpec(const std::string & s) :
    PythonStringDepSpec(s)
{
}

PythonSimpleURIDepSpec::PythonSimpleURIDepSpec(const SimpleURIDepSpec & d) :
    PythonStringDepSpec(d.text())
{
}

PythonBlockDepSpec::PythonBlockDepSpec(tr1::shared_ptr<const PythonPackageDepSpec> & a) :
    PythonStringDepSpec("!" + a->text()),
    _spec(a)
{
}

PythonBlockDepSpec::PythonBlockDepSpec(const BlockDepSpec & d) :
    PythonStringDepSpec(d.text()),
    _spec(make_shared_ptr(new PythonPackageDepSpec(*d.blocked_spec())))
{
}

tr1::shared_ptr<const PythonPackageDepSpec>
PythonBlockDepSpec::blocked_spec() const
{
    return _spec;
}

PythonFetchableURIDepSpec::PythonFetchableURIDepSpec(const std::string & s) :
    PythonStringDepSpec(s)
{
}

PythonFetchableURIDepSpec::PythonFetchableURIDepSpec(const FetchableURIDepSpec & d) :
    PythonStringDepSpec(d.text())
{
}

std::string
PythonFetchableURIDepSpec::original_url() const
{
    std::string::size_type p(text().find(" -> "));
    if (std::string::npos == p)
        return text();
    else
        return text().substr(0, p);
}

std::string
PythonFetchableURIDepSpec::renamed_url_suffix() const
{
    std::string::size_type p(text().find(" -> "));
    if (std::string::npos == p)
        return "";
    else
        return text().substr(p + 4);
}

PythonURILabelsDepSpec::PythonURILabelsDepSpec(const std::string &)
{
}

PythonURILabelsDepSpec::PythonURILabelsDepSpec(const URILabelsDepSpec &)
{
}

PythonDependencyLabelsDepSpec::PythonDependencyLabelsDepSpec(const std::string &)
{
}

PythonDependencyLabelsDepSpec::PythonDependencyLabelsDepSpec(const DependencyLabelsDepSpec &)
{
}

SpecTreeToPython::SpecTreeToPython() :
    _current_parent(new PythonAllDepSpec())
{
}

SpecTreeToPython::~SpecTreeToPython()
{
}

void
SpecTreeToPython::visit_sequence(const AllDepSpec & d,
        GenericSpecTree::ConstSequenceIterator cur,
        GenericSpecTree::ConstSequenceIterator end)
{
    tr1::shared_ptr<PythonAllDepSpec> py_cds(new PythonAllDepSpec(d));
    _current_parent->add_child(py_cds);
    Save<tr1::shared_ptr<PythonCompositeDepSpec> > old_parent(&_current_parent, py_cds);
    std::for_each(cur, end, accept_visitor(*this));
}

void
SpecTreeToPython::visit_sequence(const AnyDepSpec & d,
        GenericSpecTree::ConstSequenceIterator cur,
        GenericSpecTree::ConstSequenceIterator end)
{
    tr1::shared_ptr<PythonAnyDepSpec> py_cds(new PythonAnyDepSpec(d));
    _current_parent->add_child(py_cds);
    Save<tr1::shared_ptr<PythonCompositeDepSpec> > old_parent(&_current_parent, py_cds);
    std::for_each(cur, end, accept_visitor(*this));
}

void
SpecTreeToPython::visit_sequence(const UseDepSpec & d,
        GenericSpecTree::ConstSequenceIterator cur,
        GenericSpecTree::ConstSequenceIterator end)
{
    tr1::shared_ptr<PythonUseDepSpec> py_cds(new PythonUseDepSpec(d));
    _current_parent->add_child(py_cds);
    Save<tr1::shared_ptr<PythonCompositeDepSpec> > old_parent(&_current_parent, py_cds);
    std::for_each(cur, end, accept_visitor(*this));
}

void
SpecTreeToPython::visit_leaf(const PackageDepSpec & d)
{
    _current_parent->add_child(make_shared_ptr(new PythonPackageDepSpec(d)));
}

void
SpecTreeToPython::visit_leaf(const PlainTextDepSpec & d)
{
    _current_parent->add_child(make_shared_ptr(new PythonPlainTextDepSpec(d)));
}

void
SpecTreeToPython::visit_leaf(const NamedSetDepSpec & d)
{
    _current_parent->add_child(make_shared_ptr(new PythonNamedSetDepSpec(d)));
}

void
SpecTreeToPython::visit_leaf(const LicenseDepSpec & d)
{
    _current_parent->add_child(make_shared_ptr(new PythonLicenseDepSpec(d)));
}

void
SpecTreeToPython::visit_leaf(const SimpleURIDepSpec & d)
{
    _current_parent->add_child(make_shared_ptr(new PythonSimpleURIDepSpec(d)));
}

void
SpecTreeToPython::visit_leaf(const FetchableURIDepSpec & d)
{
    _current_parent->add_child(make_shared_ptr(new PythonFetchableURIDepSpec(d)));
}

void
SpecTreeToPython::visit_leaf(const BlockDepSpec & d)
{
    _current_parent->add_child(make_shared_ptr(new PythonBlockDepSpec(d)));
}

void
SpecTreeToPython::visit_leaf(const URILabelsDepSpec & d)
{
    _current_parent->add_child(make_shared_ptr(new PythonURILabelsDepSpec(d)));
}

void
SpecTreeToPython::visit_leaf(const DependencyLabelsDepSpec & d)
{
    _current_parent->add_child(make_shared_ptr(new PythonDependencyLabelsDepSpec(d)));
}

const tr1::shared_ptr<const PythonDepSpec>
SpecTreeToPython::result() const
{
    return *_current_parent->begin();
}

template <typename H_>
struct AllowedTypes;

template<>
struct AllowedTypes<LicenseSpecTree>
{
    AllowedTypes(const AllDepSpec &) {};
    AllowedTypes(const AnyDepSpec &) {};
    AllowedTypes(const UseDepSpec &) {};
    AllowedTypes(const LicenseDepSpec &) {};
};

template<>
struct AllowedTypes<FetchableURISpecTree>
{
    AllowedTypes(const AllDepSpec &) {};
    AllowedTypes(const UseDepSpec &) {};
    AllowedTypes(const FetchableURISpecTree &) {};
    AllowedTypes(const URILabelsDepSpec &) {};
};

template<>
struct AllowedTypes<SimpleURISpecTree>
{
    AllowedTypes(const AllDepSpec &) {};
    AllowedTypes(const UseDepSpec &) {};
    AllowedTypes(const SimpleURIDepSpec &) {};
};

template<>
struct AllowedTypes<ProvideSpecTree>
{
    AllowedTypes(const AllDepSpec &) {};
    AllowedTypes(const UseDepSpec &) {};
    AllowedTypes(const PackageDepSpec &) {};
};

template<>
struct AllowedTypes<RestrictSpecTree>
{
    AllowedTypes(const AllDepSpec &) {};
    AllowedTypes(const UseDepSpec &) {};
    AllowedTypes(const PlainTextDepSpec &) {};
};

template<>
struct AllowedTypes<DependencySpecTree>
{
    AllowedTypes(const AllDepSpec &) {};
    AllowedTypes(const AnyDepSpec &) {};
    AllowedTypes(const UseDepSpec &) {};
    AllowedTypes(const PackageDepSpec &) {};
    AllowedTypes(const BlockDepSpec &) {};
    AllowedTypes(const DependencyLabelsDepSpec &) {};
    AllowedTypes(const NamedSetDepSpec &) {};
};

template<>
struct AllowedTypes<SetSpecTree>
{
    AllowedTypes(const AllDepSpec &) {};
    AllowedTypes(const PackageDepSpec &) {};
    AllowedTypes(const NamedSetDepSpec &) {};
};

class PALUDIS_VISIBLE NotAllowedInThisHeirarchy :
    public Exception
{
    public:
        NotAllowedInThisHeirarchy(const std::string & msg) throw () :
            Exception(msg)
    {
    }
};

template <typename H_, typename D_, typename PyD_, bool>
struct Dispatcher;

template <typename H_, typename D_, typename PyD_>
struct Dispatcher<H_, D_, PyD_, true>
{
    static void do_dispatch(SpecTreeFromPython<H_> * v, const PyD_ & d)
    {
        v->real_visit(d);
    }
};

template <typename H_, typename D_, typename PyD_>
struct Dispatcher<H_, D_, PyD_, false>
{
    static void do_dispatch(SpecTreeFromPython<H_> *, const PyD_ &) PALUDIS_ATTRIBUTE((noreturn));
};

template <typename H_, typename D_, typename PyD_>
void
Dispatcher<H_, D_, PyD_, false>::do_dispatch(SpecTreeFromPython<H_> *, const PyD_ &)
{
    throw NotAllowedInThisHeirarchy(std::string("Spec parts of type '") + NiceNames<D_>::name +
            " are not allowed in a heirarchy of type '" + NiceNames<H_>::name + "'");
}

template <typename H_, typename D_, typename PyD_>
void dispatch(SpecTreeFromPython<H_> * const v, const PyD_ & d)
{
    Dispatcher<H_, D_, PyD_, tr1::is_convertible<D_, AllowedTypes<H_> >::value>::do_dispatch(v, d);
}

template <typename H_>
SpecTreeFromPython<H_>::SpecTreeFromPython() :
    _result(new ConstTreeSequence<H_, AllDepSpec>(tr1::shared_ptr<AllDepSpec>(new AllDepSpec()))),
    _add(tr1::bind(&ConstTreeSequence<H_, AllDepSpec>::add, _result, tr1::placeholders::_1))
{
}

template <typename H_>
SpecTreeFromPython<H_>::~SpecTreeFromPython()
{
}

template <typename H_>
void
SpecTreeFromPython<H_>::visit(const PythonAllDepSpec & d)
{
    dispatch<H_, AllDepSpec>(this, d);
}

template <typename H_>
void
SpecTreeFromPython<H_>::visit(const PythonAnyDepSpec & d)
{
    dispatch<H_, AnyDepSpec>(this, d);
}

template <typename H_>
void
SpecTreeFromPython<H_>::visit(const PythonUseDepSpec & d)
{
    dispatch<H_, UseDepSpec>(this, d);
}

template <typename H_>
void
SpecTreeFromPython<H_>::visit(const PythonPackageDepSpec & d)
{
    dispatch<H_, PackageDepSpec>(this, d);
}

template <typename H_>
void
SpecTreeFromPython<H_>::visit(const PythonSimpleURIDepSpec & d)
{
    dispatch<H_, SimpleURIDepSpec>(this, d);
}

template <typename H_>
void
SpecTreeFromPython<H_>::visit(const PythonLicenseDepSpec & d)
{
    dispatch<H_, LicenseDepSpec>(this, d);
}

template <typename H_>
void
SpecTreeFromPython<H_>::visit(const PythonPlainTextDepSpec & d)
{
    dispatch<H_, PlainTextDepSpec>(this, d);
}

template <typename H_>
void
SpecTreeFromPython<H_>::visit(const PythonNamedSetDepSpec & d)
{
    dispatch<H_, NamedSetDepSpec>(this, d);
}

template <typename H_>
void
SpecTreeFromPython<H_>::visit(const PythonFetchableURIDepSpec & d)
{
    dispatch<H_, FetchableURIDepSpec>(this, d);
}

template <typename H_>
void
SpecTreeFromPython<H_>::visit(const PythonURILabelsDepSpec & d)
{
    dispatch<H_, URILabelsDepSpec>(this, d);
}

template <typename H_>
void
SpecTreeFromPython<H_>::visit(const PythonDependencyLabelsDepSpec & d)
{
    dispatch<H_, DependencyLabelsDepSpec>(this, d);
}

template <typename H_>
void
SpecTreeFromPython<H_>::visit(const PythonBlockDepSpec & d)
{
    dispatch<H_, BlockDepSpec>(this, d);
}

template <typename H_>
void
SpecTreeFromPython<H_>::real_visit(const PythonAllDepSpec & d)
{
    tr1::shared_ptr<ConstTreeSequence<H_, AllDepSpec> > cds(
                new ConstTreeSequence<H_, AllDepSpec>(make_shared_ptr(new AllDepSpec())));

    _add(cds);

    Save<tr1::function<void (tr1::shared_ptr<ConstAcceptInterface<H_> >)> > old_add(&_add,
            tr1::bind(&ConstTreeSequence<H_, AllDepSpec>::add, cds, tr1::placeholders::_1));
    std::for_each(IndirectIterator<PythonCompositeDepSpec::ConstIterator, const PythonDepSpec>(d.begin()),
            IndirectIterator<PythonCompositeDepSpec::ConstIterator, const PythonDepSpec>(d.end()),
            accept_visitor(*this));
}

template <typename H_>
void
SpecTreeFromPython<H_>::real_visit(const PythonAnyDepSpec & d)
{
    tr1::shared_ptr<ConstTreeSequence<H_, AnyDepSpec> > cds(
                new ConstTreeSequence<H_, AnyDepSpec>(make_shared_ptr(new AnyDepSpec())));

    _add(cds);

    Save<tr1::function<void (tr1::shared_ptr<ConstAcceptInterface<H_> >)> > old_add(&_add,
            tr1::bind(&ConstTreeSequence<H_, AnyDepSpec>::add, cds, tr1::placeholders::_1));
    std::for_each(IndirectIterator<PythonCompositeDepSpec::ConstIterator, const PythonDepSpec>(d.begin()),
            IndirectIterator<PythonCompositeDepSpec::ConstIterator, const PythonDepSpec>(d.end()),
            accept_visitor(*this));
}

template <typename H_>
void
SpecTreeFromPython<H_>::real_visit(const PythonUseDepSpec & d)
{
    tr1::shared_ptr<ConstTreeSequence<H_, UseDepSpec> > cds(
                new ConstTreeSequence<H_, UseDepSpec>(make_shared_ptr(
                        new UseDepSpec(d.flag(), d.inverse()))));

    _add(cds);

    Save<tr1::function<void (tr1::shared_ptr<ConstAcceptInterface<H_> >)> > old_add(&_add,
            tr1::bind(&ConstTreeSequence<H_, UseDepSpec>::add, cds, tr1::placeholders::_1));
    std::for_each(IndirectIterator<PythonCompositeDepSpec::ConstIterator, const PythonDepSpec>(d.begin()),
            IndirectIterator<PythonCompositeDepSpec::ConstIterator, const PythonDepSpec>(d.end()),
            accept_visitor(*this));
}

template <typename H_>
void
SpecTreeFromPython<H_>::real_visit(const PythonPackageDepSpec & d)
{
    _add(make_shared_ptr(new TreeLeaf<H_, PackageDepSpec>(make_shared_ptr(new PackageDepSpec(d)))));
}

template <typename H_>
void
SpecTreeFromPython<H_>::real_visit(const PythonLicenseDepSpec & d)
{
    _add(make_shared_ptr(new TreeLeaf<H_, LicenseDepSpec>(make_shared_ptr(new LicenseDepSpec(d.text())))));
}

template <typename H_>
void
SpecTreeFromPython<H_>::real_visit(const PythonSimpleURIDepSpec & d)
{
    _add(make_shared_ptr(new TreeLeaf<H_, SimpleURIDepSpec>(make_shared_ptr(new SimpleURIDepSpec(d.text())))));
}

template <typename H_>
void
SpecTreeFromPython<H_>::real_visit(const PythonPlainTextDepSpec & d)
{
    _add(make_shared_ptr(new TreeLeaf<H_, PlainTextDepSpec>(make_shared_ptr(new PlainTextDepSpec(d.text())))));
}

template <typename H_>
void
SpecTreeFromPython<H_>::real_visit(const PythonNamedSetDepSpec & d)
{
    _add(make_shared_ptr(new TreeLeaf<H_, NamedSetDepSpec>(make_shared_ptr(new NamedSetDepSpec(d.name())))));
}

template <typename H_>
void
SpecTreeFromPython<H_>::real_visit(const PythonFetchableURIDepSpec & d)
{
    _add(make_shared_ptr(new TreeLeaf<H_, FetchableURIDepSpec>(
                    make_shared_ptr(new FetchableURIDepSpec(d.text())))));
}

template <typename H_>
void
SpecTreeFromPython<H_>::real_visit(const PythonURILabelsDepSpec &)
{
    _add(make_shared_ptr(new TreeLeaf<H_, URILabelsDepSpec>(make_shared_ptr(new URILabelsDepSpec))));
}

template <typename H_>
void
SpecTreeFromPython<H_>::real_visit(const PythonDependencyLabelsDepSpec &)
{
    _add(make_shared_ptr(new TreeLeaf<H_, DependencyLabelsDepSpec>(make_shared_ptr(
                        new DependencyLabelsDepSpec))));
}

template <typename H_>
void
SpecTreeFromPython<H_>::real_visit(const PythonBlockDepSpec & d)
{
    _add(make_shared_ptr(new TreeLeaf<H_, BlockDepSpec>(make_shared_ptr(
                        new BlockDepSpec(make_shared_ptr(new PackageDepSpec(*d.blocked_spec())))))));
}

template <typename H_>
tr1::shared_ptr<typename H_::ConstItem>
SpecTreeFromPython<H_>::result() const
{
    return _result;
}

template <typename T_>
struct RegisterSpecTreeToPython
{
    RegisterSpecTreeToPython()
    {
        bp::to_python_converter<tr1::shared_ptr<typename T_::ConstItem>, RegisterSpecTreeToPython<T_> >();
    }

    static PyObject *
    convert(const tr1::shared_ptr<typename T_::ConstItem> & n)
    {
        SpecTreeToPython v;
        n->accept(v);
        return bp::incref(bp::object(v.result()).ptr());
    }
};

template <typename From_, typename To_>
struct RegisterDepSpecToPython
{
    RegisterDepSpecToPython()
    {
        bp::to_python_converter<From_, RegisterDepSpecToPython<From_, To_> >();
        bp::to_python_converter<tr1::shared_ptr<const From_>, RegisterDepSpecToPython<From_, To_> >();
    }

    static PyObject *
    convert(const From_ & d)
    {
        To_ pyd(d);
        return bp::incref(bp::object(pyd).ptr());
    }

    static PyObject *
    convert(const tr1::shared_ptr<const From_> & d)
    {
        To_ pyd(*d);
        return bp::incref(bp::object(pyd).ptr());
    }
};

template <typename H_>
struct RegisterSpecTreeSharedPtrFromPython
{
    RegisterSpecTreeSharedPtrFromPython()
    {
        bp::converter::registry::push_back(&convertible, &construct,
                boost::python::type_id<tr1::shared_ptr<const typename H_::ConstItem> >());
    }

    static void *
    convertible(PyObject * obj_ptr)
    {
        if (bp::extract<PythonDepSpec *>(obj_ptr).check())
            return obj_ptr;
        else
            return 0;
    }

    static void
    construct(PyObject * obj_ptr, bp::converter::rvalue_from_python_stage1_data * data)
    {
        typedef bp::converter::rvalue_from_python_storage<tr1::shared_ptr<const typename H_::ConstItem> > Storage;
        void * storage = reinterpret_cast<Storage *>(data)->storage.bytes;

        SpecTreeFromPython<H_> v;
        PythonDepSpec * p = bp::extract<PythonDepSpec *>(obj_ptr);
        p->accept(v);

        new (storage) tr1::shared_ptr<const typename H_::ConstItem>(v.result());
        data->convertible = storage;
    }
};

void expose_dep_spec()
{
    /**
     * Exceptions
     */
    ExceptionRegister::get_instance()->add_exception<PackageDepSpecError>
        ("PackageDepSpecError", "BaseException",
         "Thrown if an invalid package dep spec specification is encountered.");

    ExceptionRegister::get_instance()->add_exception<NotAllowedInThisHeirarchy>
        ("NotAllowedInThisHeirarchy", "BaseException",
         "Thrown if a spec part not suitable for a particular heirarchy is present.");

    /**
     * Enums
     */
    enum_auto("UserPackageDepSpecOption", last_updso,
            "Options for parse_user_package_dep_spec.");

    /**
     * Options
     */
    class_options<UserPackageDepSpecOptions>("UserPackageDepSpecOptions", "UserPackageDepSpecOption",
            "Options for parse_user_package_dep_spec.");

    RegisterSpecTreeToPython<DependencySpecTree>();
    RegisterSpecTreeToPython<ProvideSpecTree>();
    RegisterSpecTreeToPython<RestrictSpecTree>();
    RegisterSpecTreeToPython<FetchableURISpecTree>();
    RegisterSpecTreeToPython<SimpleURISpecTree>();
    RegisterSpecTreeToPython<LicenseSpecTree>();
    RegisterSpecTreeToPython<SetSpecTree>();

    RegisterSpecTreeSharedPtrFromPython<DependencySpecTree>();
    RegisterSpecTreeSharedPtrFromPython<ProvideSpecTree>();
    RegisterSpecTreeSharedPtrFromPython<RestrictSpecTree>();
    RegisterSpecTreeSharedPtrFromPython<FetchableURISpecTree>();
    RegisterSpecTreeSharedPtrFromPython<SimpleURISpecTree>();
    RegisterSpecTreeSharedPtrFromPython<LicenseSpecTree>();
    RegisterSpecTreeSharedPtrFromPython<SetSpecTree>();

    /**
     * DepSpec
     */
    register_shared_ptrs_to_python<PythonDepSpec>();
    bp::class_<PythonDepSpec, boost::noncopyable>
        (
         "DepSpec",
         "Base class for a dependency spec.",
         bp::no_init
        )
        .def("as_use_dep_spec", &PythonDepSpec::as_use_dep_spec,
                bp::return_value_policy<bp::reference_existing_object>(),
                "as_use_dep_spec() -> UseDepSpec\n"
                "Return us as a UseDepSpec, or None if we are not a UseDepSpec."
            )

        .def("as_package_dep_spec", &PythonDepSpec::as_package_dep_spec,
                bp::return_value_policy<bp::reference_existing_object>(),
                "as_package_dep_spec() -> PackageDepSpec\n"
                "Return us as a PackageDepSpec, or None if we are not a PackageDepSpec."
            )
        ;

    /**
     * CompositeDepSpec
     */
    register_shared_ptrs_to_python<PythonCompositeDepSpec>();
    bp::class_<PythonCompositeDepSpec, bp::bases<PythonDepSpec>, boost::noncopyable>
        (
         "CompositeDepSpec",
         "Iterable class for dependency specs that have a number of child dependency specs.",
         bp::no_init
        )
        .def("add_child", &PythonCompositeDepSpec::add_child)

        .def("__iter__", bp::range(&PythonCompositeDepSpec::begin, &PythonCompositeDepSpec::end))
        ;

    /**
     * AnyDepSpec
     */
    bp::class_<PythonAnyDepSpec, bp::bases<PythonCompositeDepSpec>, boost::noncopyable>
        (
         "AnyDepSpec",
         "Represents a \"|| ( )\" dependency block.",
         bp::init<>("__init__()")
        );

    /**
     * AllDepSpec
     */
    bp::class_<PythonAllDepSpec, bp::bases<PythonCompositeDepSpec>, boost::noncopyable>
        (
         "AllDepSpec",
         "Represents a ( first second third ) or top level group of dependency specs.",
         bp::init<>("__init__()")
        );

    /**
     * UseDepSpec
     */
    bp::class_<PythonUseDepSpec, bp::bases<PythonCompositeDepSpec>, boost::noncopyable>
        (
         "UseDepSpec",
         "Represents a use? ( ) dependency spec.",
         bp::init<const UseFlagName &, bool>("__init__(UseFlagName, inverse_bool)")
        )
        .add_property("flag", &UseDepSpec::flag,
                "[ro] UseFlagName\n"
                "Our use flag name."
                )

        .add_property("inverse", &UseDepSpec::inverse,
                "[ro] bool\n"
                "Are we a ! flag?"
                )
        ;

    /**
     * StringDepSpec
     */
    bp::class_<PythonStringDepSpec, bp::bases<PythonDepSpec>, boost::noncopyable>
        (
         "StringDepSpec",
         "A StringDepSpec represents a non-composite dep spec with an associated piece of text.",
         bp::no_init
        )
        .add_property("text", &PythonStringDepSpec::text,
                "[ro] string\n"
                "Our text."
                )
        ;

    /**
     * PackageDepSpec
     */

    bp::def("parse_user_package_dep_spec", &parse_user_package_dep_spec,
            "parse_user_package_dep_spec(str, options=UserPackageDepSpecOptions()) -> PackageDepSpec\n"
            "Create a PackageDepSpec from user input."
           );

    bp::implicitly_convertible<PythonPackageDepSpec, PackageDepSpec>();
    bp::implicitly_convertible<PythonPackageDepSpec, tr1::shared_ptr<PackageDepSpec> >();
    bp::implicitly_convertible<tr1::shared_ptr<PackageDepSpec>, tr1::shared_ptr<const PackageDepSpec> >();
    RegisterDepSpecToPython<PackageDepSpec, PythonPackageDepSpec>();

    bp::class_<PythonPackageDepSpec, tr1::shared_ptr<const PythonPackageDepSpec>, bp::bases<PythonStringDepSpec> >
        (
         "PackageDepSpec",
         "A PackageDepSpec represents a package name (for example, 'app-editors/vim'),"
         " possibly with associated version and SLOT restrictions.",
         bp::no_init
        )

        .add_property("package", &PythonPackageDepSpec::package_ptr,
                "[ro] QualifiedPackageName\n"
                "Qualified package name."
                )

        .add_property("package_name_part", &PythonPackageDepSpec::package_name_part_ptr,
                "[ro] PackageNamePart\n"
                "Package name part (may be None)"
                )

        .add_property("category_name_part", &PythonPackageDepSpec::category_name_part_ptr,
                "[ro] CategoryNamePart\n"
                "Category name part (may be None)."
                )

        .add_property("version_requirements", &PythonPackageDepSpec::version_requirements_ptr,
                "[ro] VersionRequirements\n"
                "Version requirements (may be None)."
                )

        .add_property("version_requirements_mode", &PythonPackageDepSpec::version_requirements_mode,
                "[ro] VersionRequirementsMode\n"
                "Version requirements mode."
                )

        .add_property("slot", &PythonPackageDepSpec::slot_ptr,
                "[ro] SlotName\n"
                "Slot name (may be None)."
                )

        .add_property("repository", &PythonPackageDepSpec::repository_ptr,
                "[ro] RepositoryName\n"
                "Repository name (may be None)."
                )

        .add_property("use_requirements", &PythonPackageDepSpec::use_requirements_ptr,
                "[ro] UseRequirements\n"
                "Use requirements (may be None)."
                )

        .def("without_use_requirements", &PythonPackageDepSpec::without_use_requirements,
                "without_use_requirements() -> PackageDepSpec\n"
                "Fetch a copy of ourself without the USE requirements."
            )

        .def("__str__", &PythonPackageDepSpec::py_str)
        ;

    /**
     * PlainTextDepSpec
     */
    RegisterDepSpecToPython<PlainTextDepSpec, PythonPlainTextDepSpec>();
    bp::class_<PythonPlainTextDepSpec, bp::bases<PythonStringDepSpec> >
        (
         "PlainTextDepSpec",
         "A PlainTextDepSpec represents a plain text entry (for example, a RESTRICT keyword).",
         bp::init<const std::string &>("__init__(string)")
        )
        .def("__str__", &PythonPlainTextDepSpec::text)
        ;

    /**
     * NamedSetDepSpec
     */
    RegisterDepSpecToPython<NamedSetDepSpec, PythonNamedSetDepSpec>();
    bp::class_<PythonNamedSetDepSpec, bp::bases<PythonStringDepSpec>, boost::noncopyable>
        (
         "NamedSetDepSpec",
         "A NamedSetDepSpec represents a named package set.",
         bp::init<const SetName &>("__init__(SetName)")
        )
        .def("__str__", &PythonNamedSetDepSpec::text)
        .add_property("name", &PythonNamedSetDepSpec::name,
                "[ro] SetName"
                )
        ;

    /**
     * LicenseDepSpec
     */
    RegisterDepSpecToPython<LicenseDepSpec, PythonLicenseDepSpec>();
    bp::class_<PythonLicenseDepSpec, bp::bases<PythonStringDepSpec>, boost::noncopyable>
        (
         "LicenseDepSpec",
         "A LicenseDepSpec represents a license.",
         bp::init<const std::string &>("__init__(string)")
        )
        .def("__str__", &PythonLicenseDepSpec::text)
        ;

    /**
     * SimpleURIDepSpec
     */
    RegisterDepSpecToPython<SimpleURIDepSpec, PythonSimpleURIDepSpec>();
    bp::class_<PythonSimpleURIDepSpec, bp::bases<PythonStringDepSpec>, boost::noncopyable>
        (
         "SimpleURIDepSpec",
         "A SimpleURIDepSpec represents a simple URI.",
         bp::init<const std::string &>("__init__(string)")
        )
        .def("__str__", &PythonSimpleURIDepSpec::text)
        ;

    /**
     * FetchableURIDepSpec
     */
    RegisterDepSpecToPython<FetchableURIDepSpec, PythonFetchableURIDepSpec>();
    bp::class_<PythonFetchableURIDepSpec, bp::bases<PythonStringDepSpec>, boost::noncopyable>
        (
         "FetchableURIDepSpec",
         "A FetchableURIDepSpec represents a fetchable URI part.",
         bp::init<const std::string &>("__init__(str)")
        )
        .add_property("original_url", &PythonFetchableURIDepSpec::original_url,
                "[ro] str"
                )

        .add_property("renamed_url_suffix", &PythonFetchableURIDepSpec::renamed_url_suffix,
                "[ro] str"
                )
        ;

    /**
     * URILabelsDepSpec
     */
    RegisterDepSpecToPython<URILabelsDepSpec, PythonURILabelsDepSpec>();
    bp::class_<PythonURILabelsDepSpec, bp::bases<PythonDepSpec>, boost::noncopyable>
        (
         "URILabelsDepSpec",
         "A URILabelsDepSpec represents a URI label.",
         bp::init<const std::string &>("__init__(str)")
        )
        ;

    /**
     * DependencyLabelsDepSpec
     */
    RegisterDepSpecToPython<DependencyLabelsDepSpec, PythonDependencyLabelsDepSpec>();
    bp::class_<PythonDependencyLabelsDepSpec, bp::bases<PythonDepSpec>, boost::noncopyable>
        (
         "DependencyLabelsDepSpec",
         "A DependencyLabelsDepSpec represents a dependency label.",
         bp::init<const std::string &>("__init__(str)")
        )
        ;

    /**
     * BlockDepSpec
     */
    RegisterDepSpecToPython<BlockDepSpec, PythonBlockDepSpec>();
    bp::class_<PythonBlockDepSpec, bp::bases<PythonStringDepSpec>, boost::noncopyable >
        (
         "BlockDepSpec",
         "A BlockDepSpec represents a block on a package name (for example, 'app-editors/vim'), \n"
         "possibly with associated version and SLOT restrictions.",
         bp::init<tr1::shared_ptr<const PackageDepSpec> >("__init__(PackageDepSpec)")
        )
        .add_property("blocked_spec", &PythonBlockDepSpec::blocked_spec,
                "[ro] PackageDepSpec\n"
                "The spec we're blocking."
                )

        //Work around epydoc bug
        .add_property("text", &PythonBlockDepSpec::text,
                "[ro] string\n"
                "Our text."
                )
        ;
}

