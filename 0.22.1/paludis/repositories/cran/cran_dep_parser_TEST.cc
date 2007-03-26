/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2006 Danny van Dyk <kugelfang@gentoo.org>
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

#include <paludis/dep_spec.hh>
#include <paludis/dep_spec_flattener.hh>
#include <paludis/dep_spec_pretty_printer.hh>
#include <paludis/repositories/cran/cran_dep_parser.hh>
#include <paludis/environments/test/test_environment.hh>
#include <paludis/util/system.hh>
#include <test/test_framework.hh>
#include <test/test_runner.hh>

using namespace test;
using namespace paludis;

/** \file
 * Test cases for CRANRepository.
 *
 */

namespace test_cases
{
    /**
     * \test Test CRANDepParser::parse to parse well formed CRAN Depends: strings.
     *
     */
    struct CRANDepParserTest : TestCase
    {
        CRANDepParserTest() : TestCase("DepParser") { }

        void run()
        {
            DepSpecPrettyPrinter d1(0, false), d2(0, false), d3(0, false);
            // test R dependency
            std::string dep1("R (>= 2.0.0)");
            CRANDepParser::parse(dep1, pds_pm_permissive)->accept(&d1);
            TEST_CHECK_EQUAL(stringify(d1), "( >=dev-lang/R-2.0.0 ) ");
            // test varying whitespaces
            std::string dep2("testpackage1   \t(<1.9)");
            CRANDepParser::parse(dep2, pds_pm_permissive)->accept(&d2);
            TEST_CHECK_EQUAL(stringify(d2), "( <cran/testpackage1-1.9 ) ");
            // test for package-name and version normalisation
            std::string dep3("R.matlab (>= 2.3-1)");
            CRANDepParser::parse(dep3, pds_pm_permissive)->accept(&d3);
            TEST_CHECK_EQUAL(stringify(d3), "( >=cran/R-matlab-2.3.1 ) ");
        }
    } test_cran_dep_parser;
}

