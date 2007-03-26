#!/usr/bin/env ruby
# vim: set sw=4 sts=4 et tw=80 :
#
# Copyright (c) 2006, 2007 Richard Brown <rbrown@gentoo.org>
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

module Paludis
    class TestCase_ContentsEntry < Test::Unit::TestCase
        def test_no_create
            assert_raise NoMethodError do
                ce = ContentsEntry.new('test')
            end
        end
    end

    class TestCase_ContentsFileEntry < Test::Unit::TestCase
        def get_ce
            ContentsFileEntry.new('test')
        end

        def test_create
            assert_kind_of ContentsEntry, get_ce
        end

        def test_create_error
            assert_raise ArgumentError do
                ContentsFileEntry.new
            end

            assert_raise ArgumentError do
                ContentsFileEntry.new('a','b')
            end

            assert_raise TypeError do
                ContentsFileEntry.new(1)
            end
        end

        def test_respond_to
            ce = get_ce
            assert_respond_to ce, :name
        end

        def test_name
            ce = get_ce
            assert_equal 'test', ce.name
        end

        def test_to_s
            ce = get_ce
            assert_equal 'test', ce.to_s
        end
    end

    class TestCase_ContentsDirEntry < Test::Unit::TestCase
        def get_ce
            ContentsDirEntry.new('test')
        end

        def test_create
            assert_kind_of ContentsEntry, get_ce
        end

        def test_create_error
            assert_raise ArgumentError do
                ContentsDirEntry.new
            end

            assert_raise ArgumentError do
                ContentsDirEntry.new('a','b')
            end

            assert_raise TypeError do
                ContentsDirEntry.new(1)
            end
        end

        def test_respond_to
            ce = get_ce
            assert_respond_to ce, :name
        end

        def test_name
            ce = get_ce
            assert_equal 'test', ce.name
        end

        def test_to_s
            ce = get_ce
            assert_equal 'test', ce.to_s
        end
    end

    class TestCase_ContentsMiscEntry < Test::Unit::TestCase
        def get_ce
            ContentsMiscEntry.new('test')
        end

        def test_create
            assert_kind_of ContentsEntry, get_ce
        end

        def test_create_error
            assert_raise ArgumentError do
                ContentsMiscEntry.new
            end

            assert_raise ArgumentError do
                ContentsMiscEntry.new('a','b')
            end

            assert_raise TypeError do
                ContentsMiscEntry.new(1)
            end
        end

        def test_respond_to
            ce = get_ce
            assert_respond_to ce, :name
        end

        def test_name
            ce = get_ce
            assert_equal 'test', ce.name
        end

        def test_to_s
            ce = get_ce
            assert_equal 'test', ce.to_s
        end
    end

    class TestCase_ContentsSymEntry < Test::Unit::TestCase
        def get_ce
            ContentsSymEntry.new('test_name', 'test_target')
        end

        def test_create
            assert_kind_of ContentsEntry, get_ce
        end

        def test_create_error
            assert_raise ArgumentError do
                ContentsSymEntry.new
            end

            assert_raise ArgumentError do
                ContentsSymEntry.new('a')
            end

            assert_raise ArgumentError do
                ContentsSymEntry.new('a','b','c')
            end

            assert_raise TypeError do
                ContentsSymEntry.new('a',1)
            end

            assert_raise TypeError do
                ContentsSymEntry.new(1,'b')
            end
        end

        def test_respond_to
            ce = get_ce
            assert_respond_to ce, :name
            assert_respond_to ce, :target
        end

        def test_name
            ce = get_ce
            assert_equal 'test_name', ce.name
        end

        def test_target
            ce = get_ce
            assert_equal 'test_target', ce.target
        end

        def test_to_s
            ce = get_ce
            assert_equal "test_name -> test_target", ce.to_s
        end
    end
    class TestCase_Contents < Test::Unit::TestCase
        def get_cfe
            ContentsFileEntry.new('test')
        end

        def get_cse
            ContentsSymEntry.new('test_name', 'test_target')
        end

        def get_c
            c = Contents.new
        end

        def test_create
            get_c
        end

        def test_responds_to
            c = get_c
            [:add, :entries, :each].each {|sym| assert_respond_to c, sym}
        end

        def test_add
            assert_nothing_raised do
                get_c().add(get_cfe)
            end
        end

        def test_entries
            c = get_c

            assert_equal true, c.entries.empty?

            cfe = get_cfe
            cse = get_cse
            c.add(cfe)

            assert_equal 1, c.entries.length
            assert_equal cfe.to_s, c.entries.first.to_s

            c.add(cse)

            assert_equal 2, c.entries.length
            assert_equal cfe.to_s, c.entries.first.to_s
            assert_equal cse.to_s, c.entries.last.to_s

        end
    end
end

