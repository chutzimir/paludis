/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2006 Richard Brown <mynamewasgone@gmail.com>
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

#include <paludis_ruby.hh>
#include <paludis/qa/check.hh>
#include <paludis/qa/package_dir_check.hh>
#include <paludis/util/stringify.hh>
#include <paludis/qa/qa.hh>
#include <ruby.h>
#include <cstdlib>
#include <iostream>

using namespace paludis;
using namespace paludis::qa;
using namespace paludis::ruby;
using std::cerr;

#define RUBY_FUNC_CAST(x) reinterpret_cast<VALUE (*)(...)>(x)

namespace
{
    static VALUE c_ebuild_check_data;
    static VALUE c_package_dir_check;
    static VALUE c_package_dir_check_maker;
    static VALUE c_file_check;
    static VALUE c_file_check_maker;
    static VALUE c_ebuild_check;
    static VALUE c_ebuild_check_maker;

    VALUE
    ebuild_check_data_init(int, VALUE *, VALUE self)
    {
        return self;
    }

    VALUE
    ebuild_check_data_new(int argc, VALUE *argv, VALUE self)
    {
        EbuildCheckData * ptr(0);
        try
        {
            if (3 == argc)
            {
                ptr = new EbuildCheckData(
                    value_to_qualified_package_name(argv[0]),
                    value_to_version_spec(argv[1]),
                    value_to_environment_data(argv[2])->env_ptr
                );
            }
            else
            {
                rb_raise(rb_eArgError, "EbuildCheckData expects three arguments, but got %d",argc);
            }
            VALUE tdata(Data_Wrap_Struct(self, 0, &Common<EbuildCheckData>::free, ptr));
            rb_obj_call_init(tdata, argc, argv);
            return tdata;
        }
        catch (const std::exception & e)
        {
            delete ptr;
            exception_to_ruby_exception(e);
        }
    }

    template <typename T_>
    struct CheckStruct
    {
        static
        VALUE describe(VALUE self)
        {
            T_ * ptr;
            Data_Get_Struct(self, T_, ptr);
            return rb_str_new2((*ptr)->describe().c_str());
        }
        static
        VALUE
        is_important(VALUE self)
        {
            T_ * ptr;
            Data_Get_Struct(self, T_, ptr);
            return (*ptr)->is_important() ? Qtrue : Qfalse;
        }
    };

    template <typename T_, typename S_>
    VALUE
    check_check(VALUE check, S_ arg)
    {
        try
        {
            T_ * ptr;
            Data_Get_Struct(check, T_, ptr);
            return check_result_to_value((**ptr)(arg));
        }
        catch (const std::exception & e)
        {
            exception_to_ruby_exception(e);
        }
    }

    VALUE
    package_dir_check_check(VALUE self, VALUE f)
    {
        try
        {
            return (check_check <PackageDirCheck::Pointer, FSEntry> (self, FSEntry(StringValuePtr(f))));
        }
        catch (const std::exception & e)
        {
            exception_to_ruby_exception(e);
        }
    }

    VALUE
    file_check_check(VALUE self, VALUE f)
    {
        try
        {
            return (check_check <FileCheck::Pointer, FSEntry> (self, FSEntry(StringValuePtr(f))));
        }
        catch (const std::exception & e)
        {
            exception_to_ruby_exception(e);
        }
    }

    VALUE
    ebuild_check_check(VALUE self, VALUE f)
    {
        try
        {
            return (check_check <EbuildCheck::Pointer, EbuildCheckData> (self, value_to_ebuild_check_data(f)));
        }
        catch (const std::exception & e)
        {
            exception_to_ruby_exception(e);
        }
    }

    template <typename T_>
    struct CheckMakerStruct
    {
        static
        VALUE keys(VALUE)
        {
            std::list<std::string> checks;
            T_::get_instance()->copy_keys(std::back_inserter(checks));
            VALUE result(rb_ary_new());
            for (std::list<std::string>::const_iterator i(checks.begin()),
                i_end(checks.end()) ; i != i_end ; ++i)
            {
                rb_ary_push(result, rb_str_new2((*i).c_str()));
            }
            return result;
        }
    };

    VALUE package_dir_check_maker_find_maker(VALUE, VALUE maker)
    {
        try
        {
            PackageDirCheck::Pointer p = (PackageDirCheckMaker::get_instance()->find_maker(StringValuePtr(maker)))();
            return package_dir_check_to_value(p);
        }
        catch (const std::exception & e)
        {
            exception_to_ruby_exception(e);
        }
    }

    VALUE file_check_maker_find_maker(VALUE, VALUE maker)
    {
        try
        {
            FileCheck::Pointer p = (FileCheckMaker::get_instance()->find_maker(StringValuePtr(maker)))();
            return file_check_to_value(p);
        }
        catch (const std::exception & e)
        {
            exception_to_ruby_exception(e);
        }
    }

    VALUE ebuild_check_maker_find_maker(VALUE, VALUE maker)
    {
        try
        {
            EbuildCheck::Pointer p = (EbuildCheckMaker::get_instance()->find_maker(StringValuePtr(maker)))();
            return ebuild_check_to_value(p);
        }
        catch (const std::exception & e)
        {
            exception_to_ruby_exception(e);
        }
    }

    void do_register_check()
    {
        rb_require("singleton");

        c_ebuild_check_data = rb_define_class_under(paludis_qa_module(), "EbuildCheckData", rb_cObject);
        rb_define_singleton_method(c_ebuild_check_data, "new", RUBY_FUNC_CAST(&ebuild_check_data_new),-1);
        rb_define_method(c_ebuild_check_data, "initialize", RUBY_FUNC_CAST(&ebuild_check_data_init),-1);

        c_package_dir_check = rb_define_class_under(paludis_qa_module(), "PackageDirCheck", rb_cObject);
        rb_funcall(c_package_dir_check, rb_intern("private_class_method"), 1, rb_str_new2("new"));
        rb_define_method(c_package_dir_check, "check", RUBY_FUNC_CAST(&package_dir_check_check),1);
        rb_define_method(c_package_dir_check, "describe", RUBY_FUNC_CAST(&CheckStruct<PackageDirCheck::Pointer>::describe),0);
        rb_define_method(c_package_dir_check, "is_important?", RUBY_FUNC_CAST(&CheckStruct<PackageDirCheck::Pointer>::is_important),0);

        c_file_check = rb_define_class_under(paludis_qa_module(), "FileCheck", rb_cObject);
        rb_funcall(c_file_check, rb_intern("private_class_method"), 1, rb_str_new2("new"));
        rb_define_method(c_file_check, "check", RUBY_FUNC_CAST(&file_check_check),1);
        rb_define_method(c_file_check, "describe", RUBY_FUNC_CAST(&CheckStruct<FileCheck::Pointer>::describe),0);
        rb_define_method(c_file_check, "is_important?", RUBY_FUNC_CAST(&CheckStruct<FileCheck::Pointer>::is_important),0);

        c_ebuild_check = rb_define_class_under(paludis_qa_module(), "EbuildCheck", rb_cObject);
        rb_funcall(c_ebuild_check, rb_intern("private_class_method"), 1, rb_str_new2("new"));
        rb_define_method(c_ebuild_check, "check", RUBY_FUNC_CAST(&ebuild_check_check),1);
        rb_define_method(c_ebuild_check, "describe", RUBY_FUNC_CAST(&CheckStruct<EbuildCheck::Pointer>::describe),0);
        rb_define_method(c_ebuild_check, "is_important?", RUBY_FUNC_CAST(&CheckStruct<EbuildCheck::Pointer>::is_important),0);

        c_package_dir_check_maker = rb_define_class_under(paludis_qa_module(), "PackageDirCheckMaker", rb_cObject);
        rb_funcall(c_package_dir_check_maker, rb_intern("private_class_method"), 1, rb_str_new2("new"));
        rb_funcall(rb_const_get(rb_cObject, rb_intern("Singleton")), rb_intern("included"), 1, c_package_dir_check_maker);
        rb_define_method(c_package_dir_check_maker, "keys", RUBY_FUNC_CAST(CheckMakerStruct<PackageDirCheckMaker>::keys),0);
        rb_define_method(c_package_dir_check_maker, "find_maker", RUBY_FUNC_CAST(&package_dir_check_maker_find_maker),1);

        c_file_check_maker = rb_define_class_under(paludis_qa_module(), "FileCheckMaker", rb_cObject);
        rb_funcall(c_file_check_maker, rb_intern("private_class_method"), 1, rb_str_new2("new"));
        rb_funcall(rb_const_get(rb_cObject, rb_intern("Singleton")), rb_intern("included"), 1, c_file_check_maker);
        rb_define_method(c_file_check_maker, "keys", RUBY_FUNC_CAST(CheckMakerStruct<FileCheckMaker>::keys),0);
        rb_define_method(c_file_check_maker, "find_maker", RUBY_FUNC_CAST(&file_check_maker_find_maker),1);

        c_ebuild_check_maker = rb_define_class_under(paludis_qa_module(), "EbuildCheckMaker", rb_cObject);
        rb_funcall(c_ebuild_check_maker, rb_intern("private_class_method"), 1, rb_str_new2("new"));
        rb_funcall(rb_const_get(rb_cObject, rb_intern("Singleton")), rb_intern("included"), 1, c_ebuild_check_maker);
        rb_define_method(c_ebuild_check_maker, "keys", RUBY_FUNC_CAST(&CheckMakerStruct<EbuildCheckMaker>::keys),0);
        rb_define_method(c_ebuild_check_maker, "find_maker", RUBY_FUNC_CAST(&ebuild_check_maker_find_maker),1);
    }
}

VALUE
paludis::ruby::file_check_to_value(FileCheck::Pointer m)
{
    FileCheck::Pointer * m_ptr(0);
    try
    {
        m_ptr = new FileCheck::Pointer(m);
        return Data_Wrap_Struct(c_file_check, 0, &Common<FileCheck::Pointer>::free, m_ptr);
    }
    catch (const std::exception & e)
    {
        delete m_ptr;
        exception_to_ruby_exception(e);
    }
}

VALUE
paludis::ruby::package_dir_check_to_value(PackageDirCheck::Pointer m)
{
    PackageDirCheck::Pointer * m_ptr(0);
    try
    {
        m_ptr = new PackageDirCheck::Pointer(m);
        return Data_Wrap_Struct(c_package_dir_check, 0, &Common<PackageDirCheck::Pointer>::free, m_ptr);
    }
    catch (const std::exception & e)
    {
        delete m_ptr;
        exception_to_ruby_exception(e);
    }
}

VALUE
paludis::ruby::ebuild_check_to_value(EbuildCheck::Pointer m)
{
    EbuildCheck::Pointer * m_ptr(0);
    try
    {
        m_ptr = new EbuildCheck::Pointer(m);
        return Data_Wrap_Struct(c_ebuild_check, 0, &Common<EbuildCheck::Pointer>::free, m_ptr);
    }
    catch (const std::exception & e)
    {
        delete m_ptr;
        exception_to_ruby_exception(e);
    }
}

VALUE
paludis::ruby::ebuild_check_data_to_value(const EbuildCheckData & v)
{
    EbuildCheckData * vv(new EbuildCheckData(v));
    return Data_Wrap_Struct(c_ebuild_check_data, 0, &Common<EbuildCheckData>::free, vv);
}


EbuildCheckData
paludis::ruby::value_to_ebuild_check_data(VALUE v)
{
    if (rb_obj_is_kind_of(v, c_ebuild_check_data))
    {
        EbuildCheckData * v_ptr;
        Data_Get_Struct(v, EbuildCheckData, v_ptr);
        return *v_ptr;
    }
    else
    {
        rb_raise(rb_eTypeError, "Can't convert %s into EbuildCheckData", rb_obj_classname(v));
    }
}

RegisterRubyClass::Register paludis_ruby_register_check PALUDIS_ATTRIBUTE((used))
    (&do_register_check);
