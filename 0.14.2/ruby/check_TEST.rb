#!/usr/bin/env ruby
# vim: set sw=4 sts=4 et tw=80 :

#
# Copyright (c) 2006 Richard Brown <mynamewasgone@gmail.com>
#
# This file is part of the Paludis package manager. Paludis is free software;
# you can redistribute it and/or modify it under the terms of the GNU General
# Public License version 2, as published by the Free Software Foundation.
#
# Paludis is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 59 Temple
# Place, Suite 330, Boston, MA  02111-1307  USA
#

require 'test/unit'
require 'Paludis'

Paludis::Log.instance.log_level = Paludis::LogLevel::Warning

module Paludis
    module QA
        class TestCase_FileCheckMaker < Test::Unit::TestCase
            def instance
                FileCheckMaker.instance
            end

            def test_instance
                assert_equal FileCheckMaker.instance.__id__, FileCheckMaker.instance.__id__
                assert_kind_of FileCheckMaker, FileCheckMaker.instance
            end

            def test_no_create
                assert_raise NoMethodError do
                    x = FileCheckMaker.new()
                end
            end

            def test_respond_to
                assert_respond_to instance, :keys
                assert_respond_to instance, :find_maker
                assert_respond_to instance, :check_names
                assert_respond_to instance, :find_check
            end

            def test_keys
                assert_kind_of Array, instance.keys
                assert_not_equal 0, instance.keys
            end

            def test_find_maker
                name = instance.keys.first
                assert_kind_of FileCheck, instance.find_maker(name)
            end
            def test_check_names
                assert_kind_of Array, instance.check_names
                assert_equal instance.keys, instance.check_names
            end

            def test_find_check
                name = instance.keys.first
                assert_equal instance.find_maker(name).describe, instance.find_check(name).describe
            end
        end

        class TestCase_PackageDirCheckMaker < Test::Unit::TestCase
            def instance
                PackageDirCheckMaker.instance
            end

            def test_instance
                assert_equal PackageDirCheckMaker.instance.__id__, PackageDirCheckMaker.instance.__id__
                assert_kind_of PackageDirCheckMaker, PackageDirCheckMaker.instance
            end

            def test_no_create
                assert_raise NoMethodError do
                    x = PackageDirCheckMaker.new()
                end
            end

            def test_respond_to
                assert_respond_to instance, :keys
                assert_respond_to instance, :find_maker
                assert_respond_to instance, :check_names
                assert_respond_to instance, :find_check
            end

            def test_keys
                assert_kind_of Array, instance.keys
                assert_not_equal 0, instance.keys
            end

            def test_find_maker
                name = instance.keys.first
                assert_kind_of PackageDirCheck, instance.find_maker(name)
            end
            def test_check_names
                assert_kind_of Array, instance.check_names
                assert_equal instance.keys, instance.check_names
            end

            def test_find_check
                name = instance.keys.first
                assert_equal instance.find_maker(name).describe, instance.find_check(name).describe
            end
        end

        class TestCase_EbuildCheckMaker < Test::Unit::TestCase
            def instance
                EbuildCheckMaker.instance
            end

            def test_instance
                assert_equal EbuildCheckMaker.instance.__id__, EbuildCheckMaker.instance.__id__
                assert_kind_of EbuildCheckMaker, EbuildCheckMaker.instance
            end

            def test_no_create
                assert_raise NoMethodError do
                    x = EbuildCheckMaker.new()
                end
            end

            def test_respond_to
                assert_respond_to instance, :keys
                assert_respond_to instance, :find_maker
                assert_respond_to instance, :check_names
                assert_respond_to instance, :find_check
            end

            def test_keys
                assert_kind_of Array, instance.keys
                assert_not_equal 0, instance.keys
            end

            def test_find_maker
                name = instance.keys.first
                assert_kind_of EbuildCheck, instance.find_maker(name)
            end
            def test_check_names
                assert_kind_of Array, instance.check_names
                assert_equal instance.keys, instance.check_names
            end

            def test_find_check
                name = instance.keys.first
                assert_equal instance.find_maker(name).describe, instance.find_check(name).describe
            end
        end

        class TestCase_PerProfileEbuildCheckMaker < Test::Unit::TestCase
            def instance
                PerProfileEbuildCheckMaker.instance
            end

            def test_instance
                assert_equal PerProfileEbuildCheckMaker.instance.__id__, PerProfileEbuildCheckMaker.instance.__id__
                assert_kind_of PerProfileEbuildCheckMaker, PerProfileEbuildCheckMaker.instance
            end

            def test_no_create
                assert_raise NoMethodError do
                    x = PerProfileEbuildCheckMaker.new()
                end
            end

            def test_respond_to
                assert_respond_to instance, :keys
                assert_respond_to instance, :find_maker
                assert_respond_to instance, :check_names
                assert_respond_to instance, :find_check
            end

            def test_keys
                assert_kind_of Array, instance.keys
                assert_not_equal 0, instance.keys
            end

            def test_find_maker
                name = instance.keys.first
                assert_kind_of PerProfileEbuildCheck, instance.find_maker(name)
            end
            def test_check_names
                assert_kind_of Array, instance.check_names
                assert_equal instance.keys, instance.check_names
            end

            def test_find_check
                name = instance.keys.first
                assert_equal instance.find_maker(name).describe, instance.find_check(name).describe
            end
        end

        class TestCase_ProfilesCheckMaker < Test::Unit::TestCase
            def instance
                ProfilesCheckMaker.instance
            end

            def test_instance
                assert_equal ProfilesCheckMaker.instance.__id__, ProfilesCheckMaker.instance.__id__
                assert_kind_of ProfilesCheckMaker, ProfilesCheckMaker.instance
            end

            def test_no_create
                assert_raise NoMethodError do
                    x = ProfilesCheckMaker.new()
                end
            end

            def test_respond_to
                assert_respond_to instance, :keys
                assert_respond_to instance, :find_maker
                assert_respond_to instance, :check_names
                assert_respond_to instance, :find_check
            end

            def test_keys
                assert_kind_of Array, instance.keys
                assert_not_equal 0, instance.keys
            end

            def test_find_maker
                name = instance.keys.first
                assert_kind_of ProfilesCheck, instance.find_maker(name)
            end
            def test_check_names
                assert_kind_of Array, instance.check_names
                assert_equal instance.keys, instance.check_names
            end

            def test_find_check
                name = instance.keys.first
                assert_equal instance.find_maker(name).describe, instance.find_check(name).describe
            end
        end

        class TestCase_ProfileCheckMaker < Test::Unit::TestCase
            def instance
                ProfileCheckMaker.instance
            end

            def test_instance
                assert_equal ProfileCheckMaker.instance.__id__, ProfileCheckMaker.instance.__id__
                assert_kind_of ProfileCheckMaker, ProfileCheckMaker.instance
            end

            def test_no_create
                assert_raise NoMethodError do
                    x = ProfileCheckMaker.new()
                end
            end

            def test_respond_to
                assert_respond_to instance, :keys
                assert_respond_to instance, :find_maker
                assert_respond_to instance, :check_names
                assert_respond_to instance, :find_check
            end

            def test_keys
                assert_kind_of Array, instance.keys
                assert_not_equal 0, instance.keys
            end

            def test_find_maker
                name = instance.keys.first
                assert_kind_of ProfileCheck, instance.find_maker(name)
            end
            def test_check_names
                assert_kind_of Array, instance.check_names
                assert_equal instance.keys, instance.check_names
            end

            def test_find_check
                name = instance.keys.first
                assert_equal instance.find_maker(name).describe, instance.find_check(name).describe
            end
        end

        class TestCase_EbuildCheckData < Test::Unit::TestCase
            def test_create
                env = QAEnvironment.new('check_TEST_dir/repo1')
                ecd = EbuildCheckData.new('cat-one/pkg-one', "1", env)
            end
        end

        class TestCase_PerProfileEbuildCheckData < Test::Unit::TestCase
            def test_create
                env = QAEnvironment.new('check_TEST_dir/repo1')
                ecd = PerProfileEbuildCheckData.new('cat-one/pkg-one',
                    "1",
                    env,
                    'check_TEST_dir/repo1/profiles/profile')
            end
        end

        class TestCase_ProfileCheckData < Test::Unit::TestCase
            def test_create
                env = QAEnvironment.new('check_TEST_dir/repo1')
                ecd = EbuildCheckData.new('cat-one/pkg-one', "1", env)
            end
        end

        class TestCase_PackageDirCheck < Test::Unit::TestCase
            def get_check
                PackageDirCheckMaker.instance.find_maker('package_name')
            end

            def test_no_create
                assert_raise NoMethodError do
                    x = PackageDirCheck.new()
                end
            end

            def test_respond_to
                check = get_check
                assert_respond_to check, :describe
                assert_respond_to check, :is_important?
                assert_respond_to check, :check
            end

            def test_describe
                check = get_check
                assert_equal "Checks that the category/package name is valid", check.describe
            end

            def test_is_important
                check = get_check
                assert_equal true, check.is_important?
            end

            def test_check
                check = get_check
                assert_nothing_raised do
                    cr = check.check('cat-one/pkg-one')
                end
            end
        end

        class TestCase_FileCheck < Test::Unit::TestCase
            def get_check
                FileCheckMaker.instance.find_maker('whitespace')
            end

            def test_no_create
                assert_raise NoMethodError do
                    x = FileCheck.new()
                end
            end

            def test_respond_to
                check = get_check
                assert_respond_to check, :describe
                assert_respond_to check, :is_important?
                assert_respond_to check, :check
            end

            def test_describe
                check = get_check
                assert_equal "Checks whitespace", check.describe
            end

            def test_is_important
                check = get_check
                assert_equal false, check.is_important?
            end

            def test_check
                check = get_check
                assert_nothing_raised do
                    cr = check.check('cat-one/pkg-one/pkg-one-1.ebuild')
                end
            end
        end

        class TestCase_EbuildCheck < Test::Unit::TestCase
            def get_ecd
                env = QAEnvironment.new('check_TEST_dir/repo1')
                ecd = EbuildCheckData.new('cat-one/pkg-one', "1", env)
            end

            def get_check
                EbuildCheckMaker.instance.find_maker('create_metadata')
            end

            def test_no_create
                assert_raise NoMethodError do
                    x = EbuildCheck.new()
                end
            end

            def test_respond_to
                check = get_check
                assert_respond_to check, :describe
                assert_respond_to check, :is_important?
                assert_respond_to check, :check
            end

            def test_describe
                check = get_check
                assert_equal "Checks that the metadata can be generated", check.describe
            end

            def test_is_important
                check = get_check
                assert_equal true, check.is_important?
            end

            def test_check
                check = get_check
                assert_nothing_raised do
                    cr = check.check(get_ecd)
                end
            end
        end

        class TestCase_PerProfileEbuildCheck < Test::Unit::TestCase
            def get_ecd
                env = QAEnvironment.new('check_TEST_dir/repo1')
                PerProfileEbuildCheckData.new('cat-one/pkg-one', "1", env, 'check_TEST_dir/repo1/profiles/profile')
            end

            def get_check
                PerProfileEbuildCheckMaker.instance.find_maker('deps_visible')
            end

            def test_no_create
                assert_raise NoMethodError do
                    x = PerProfileEbuildCheck.new()
                end
            end

            def test_respond_to
                check = get_check
                assert_respond_to check, :describe
                assert_respond_to check, :is_important?
                assert_respond_to check, :check
            end

            def test_describe
                check = get_check
                assert_equal "Checks that packages in *DEPEND are visible", check.describe
            end

            def test_is_important
                check = get_check
                assert_equal false, check.is_important?
            end

            def test_check
                check = get_check
                assert_nothing_raised do
                    cr = check.check(get_ecd)
                end
            end
        end

        class TestCase_ProfilesCheck < Test::Unit::TestCase
            def get_check
                ProfilesCheckMaker.instance.find_maker('repo_name')
            end

            def test_no_create
                assert_raise NoMethodError do
                    x = ProfilesCheck.new()
                end
            end

            def test_respond_to
                check = get_check
                assert_respond_to check, :describe
                assert_respond_to check, :is_important?
                assert_respond_to check, :check
            end

            def test_describe
                check = get_check
                assert_equal "Checks that repo_name is sane", check.describe
            end

            def test_is_important
                check = get_check
                assert_equal false, check.is_important?
            end

            def test_check
                check = get_check
                assert_nothing_raised do
                    cr = check.check('check_TEST_dir/repo1/profiles')
                end
            end
        end

        class TestCase_ProfileCheck < Test::Unit::TestCase
            def get_pcd
                env = QAEnvironment.new('check_TEST_dir/repo1')
                ProfileCheckData.new('check_TEST_dir/repo1/profiles', env.portage_repository.profiles.first)
            end

            def get_check
                ProfileCheckMaker.instance.find_maker('profile_paths_exist')
            end

            def test_no_create
                assert_raise NoMethodError do
                    x = ProfileCheck.new()
                end
            end

            def test_respond_to
                check = get_check
                assert_respond_to check, :describe
                assert_respond_to check, :is_important?
                assert_respond_to check, :check
            end

            def test_describe
                check = get_check
                assert_equal "Checks that profile paths exist", check.describe
            end

            def test_is_important
                check = get_check
                assert_equal true, check.is_important?
            end

            def test_check
                check = get_check
                assert_nothing_raised do
                    cr = check.check(get_pcd)
                end
            end
        end
    end
end

