/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2008 David Leverton
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
#include <paludis/elike_use_requirement.hh>
#include <paludis/package_database.hh>
#include <paludis/environments/test/test_environment.hh>
#include <paludis/repositories/fake/fake_repository.hh>
#include <paludis/repositories/fake/fake_package_id.hh>
#include <test/test_framework.hh>
#include <test/test_runner.hh>

using namespace paludis;
using namespace test;

namespace test_cases
{
    struct SimpleUseRequirementsTest : TestCase
    {
        SimpleUseRequirementsTest() : TestCase("simple use requirements") { }

        void run()
        {
            TestEnvironment env;
            std::tr1::shared_ptr<FakeRepository> fake(new FakeRepository(&env, RepositoryName("fake")));
            env.package_database()->add_repository(1, fake);
            std::tr1::shared_ptr<FakePackageID> id(fake->add_version("cat", "pkg1", "1"));
            id->iuse_key()->set_from_string("enabled disabled", IUseFlagParseOptions());

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req1(
                parse_elike_use_requirement("enabled", std::tr1::shared_ptr<const PackageID>(), ELikeUseRequirementOptions()));
            TEST_CHECK_EQUAL(req1->as_raw_string(), "[enabled]");
            TEST_CHECK_EQUAL(req1->as_human_string(), "Flag 'enabled' enabled");
            TEST_CHECK(req1->requirement_met(&env, *id));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req2(
                parse_elike_use_requirement("disabled", std::tr1::shared_ptr<const PackageID>(), ELikeUseRequirementOptions()));
            TEST_CHECK_EQUAL(req2->as_raw_string(), "[disabled]");
            TEST_CHECK_EQUAL(req2->as_human_string(), "Flag 'disabled' enabled");
            TEST_CHECK(! req2->requirement_met(&env, *id));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req3(
                parse_elike_use_requirement("-enabled", std::tr1::shared_ptr<const PackageID>(), ELikeUseRequirementOptions()));
            TEST_CHECK_EQUAL(req3->as_raw_string(), "[-enabled]");
            TEST_CHECK_EQUAL(req3->as_human_string(), "Flag 'enabled' disabled");
            TEST_CHECK(! req3->requirement_met(&env, *id));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req4(
                parse_elike_use_requirement("-disabled", std::tr1::shared_ptr<const PackageID>(), ELikeUseRequirementOptions()));
            TEST_CHECK_EQUAL(req4->as_raw_string(), "[-disabled]");
            TEST_CHECK_EQUAL(req4->as_human_string(), "Flag 'disabled' disabled");
            TEST_CHECK(req4->requirement_met(&env, *id));
        }
    } test_simple_use_requirements;

    struct SimpleUseRequirementsPortageSyntaxTest : TestCase
    {
        SimpleUseRequirementsPortageSyntaxTest() : TestCase("simple use requirements portage syntax") { }

        void run()
        {
            TestEnvironment env;
            std::tr1::shared_ptr<FakeRepository> fake(new FakeRepository(&env, RepositoryName("fake")));
            env.package_database()->add_repository(1, fake);
            std::tr1::shared_ptr<FakePackageID> id(fake->add_version("cat", "pkg1", "1"));
            id->iuse_key()->set_from_string("enabled disabled", IUseFlagParseOptions());

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req1(
                parse_elike_use_requirement("enabled", std::tr1::shared_ptr<const PackageID>(), ELikeUseRequirementOptions() + euro_portage_syntax));
            TEST_CHECK_EQUAL(req1->as_raw_string(), "[enabled]");
            TEST_CHECK_EQUAL(req1->as_human_string(), "Flag 'enabled' enabled");
            TEST_CHECK(req1->requirement_met(&env, *id));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req2(
                parse_elike_use_requirement("disabled", std::tr1::shared_ptr<const PackageID>(), ELikeUseRequirementOptions() + euro_portage_syntax));
            TEST_CHECK_EQUAL(req2->as_raw_string(), "[disabled]");
            TEST_CHECK_EQUAL(req2->as_human_string(), "Flag 'disabled' enabled");
            TEST_CHECK(! req2->requirement_met(&env, *id));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req3(
                parse_elike_use_requirement("-enabled", std::tr1::shared_ptr<const PackageID>(), ELikeUseRequirementOptions() + euro_portage_syntax));
            TEST_CHECK_EQUAL(req3->as_raw_string(), "[-enabled]");
            TEST_CHECK_EQUAL(req3->as_human_string(), "Flag 'enabled' disabled");
            TEST_CHECK(! req3->requirement_met(&env, *id));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req4(
                parse_elike_use_requirement("-disabled", std::tr1::shared_ptr<const PackageID>(), ELikeUseRequirementOptions() + euro_portage_syntax));
            TEST_CHECK_EQUAL(req4->as_raw_string(), "[-disabled]");
            TEST_CHECK_EQUAL(req4->as_human_string(), "Flag 'disabled' disabled");
            TEST_CHECK(req4->requirement_met(&env, *id));
        }
    } test_simple_use_requirements_portage_syntax;

    struct MultipleUseRequirementsPortageSyntaxTest : TestCase
    {
        MultipleUseRequirementsPortageSyntaxTest() : TestCase("multiple use requirements portage syntax") { }

        void run()
        {
            TestEnvironment env;
            std::tr1::shared_ptr<FakeRepository> fake(new FakeRepository(&env, RepositoryName("fake")));
            env.package_database()->add_repository(1, fake);
            std::tr1::shared_ptr<FakePackageID> id(fake->add_version("cat", "pkg1", "1"));
            id->iuse_key()->set_from_string("enabled disabled", IUseFlagParseOptions());

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req1(
                parse_elike_use_requirement("enabled,-disabled", std::tr1::shared_ptr<const PackageID>(), ELikeUseRequirementOptions() + euro_portage_syntax));
            TEST_CHECK_EQUAL(req1->as_raw_string(), "[enabled,-disabled]");
            TEST_CHECK_EQUAL(req1->as_human_string(), "Flag 'enabled' enabled; Flag 'disabled' disabled");
            TEST_CHECK(req1->requirement_met(&env, *id));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req2(
                parse_elike_use_requirement("enabled,disabled", std::tr1::shared_ptr<const PackageID>(), ELikeUseRequirementOptions() + euro_portage_syntax));
            TEST_CHECK_EQUAL(req2->as_raw_string(), "[enabled,disabled]");
            TEST_CHECK_EQUAL(req2->as_human_string(), "Flag 'enabled' enabled; Flag 'disabled' enabled");
            TEST_CHECK(! req2->requirement_met(&env, *id));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req3(
                parse_elike_use_requirement("-enabled,-disabled", std::tr1::shared_ptr<const PackageID>(), ELikeUseRequirementOptions() + euro_portage_syntax));
            TEST_CHECK_EQUAL(req3->as_raw_string(), "[-enabled,-disabled]");
            TEST_CHECK_EQUAL(req3->as_human_string(), "Flag 'enabled' disabled; Flag 'disabled' disabled");
            TEST_CHECK(! req3->requirement_met(&env, *id));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req4(
                parse_elike_use_requirement("enabled,-disabled,-enabled", std::tr1::shared_ptr<const PackageID>(), ELikeUseRequirementOptions() + euro_portage_syntax));
            TEST_CHECK_EQUAL(req4->as_raw_string(), "[enabled,-disabled,-enabled]");
            TEST_CHECK_EQUAL(req4->as_human_string(), "Flag 'enabled' enabled; Flag 'disabled' disabled; Flag 'enabled' disabled");
            TEST_CHECK(! req4->requirement_met(&env, *id));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req5(
                parse_elike_use_requirement("enabled,-disabled,enabled", std::tr1::shared_ptr<const PackageID>(), ELikeUseRequirementOptions() + euro_portage_syntax));
            TEST_CHECK_EQUAL(req5->as_raw_string(), "[enabled,-disabled,enabled]");
            TEST_CHECK_EQUAL(req5->as_human_string(), "Flag 'enabled' enabled; Flag 'disabled' disabled; Flag 'enabled' enabled");
            TEST_CHECK(req5->requirement_met(&env, *id));
        }
    } test_multiple_use_requirements_portage_syntax;

    struct ComplexUseRequirementsTest : TestCase
    {
        ComplexUseRequirementsTest() : TestCase("complex use requirements") { }

        void run()
        {
            TestEnvironment env;
            std::tr1::shared_ptr<FakeRepository> fake(new FakeRepository(&env, RepositoryName("fake")));
            env.package_database()->add_repository(1, fake);
            std::tr1::shared_ptr<FakePackageID> id(fake->add_version("cat", "enabled", "1"));
            id->iuse_key()->set_from_string("pkgname", IUseFlagParseOptions());
            std::tr1::shared_ptr<FakePackageID> id2(fake->add_version("cat", "disabled", "1"));
            id2->iuse_key()->set_from_string("pkgname", IUseFlagParseOptions());

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req1(
                parse_elike_use_requirement("pkgname?", id, ELikeUseRequirementOptions() + euro_allow_self_deps));
            TEST_CHECK_EQUAL(req1->as_raw_string(), "[pkgname?]");
            TEST_CHECK_EQUAL(req1->as_human_string(), "Flag 'pkgname' enabled if it is enabled for 'cat/enabled-1:0::fake'");
            TEST_CHECK(req1->requirement_met(&env, *id));
            TEST_CHECK(! req1->requirement_met(&env, *id2));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req2(
                parse_elike_use_requirement("pkgname?", id2, ELikeUseRequirementOptions() + euro_allow_self_deps));
            TEST_CHECK_EQUAL(req2->as_raw_string(), "[pkgname?]");
            TEST_CHECK_EQUAL(req2->as_human_string(), "Flag 'pkgname' enabled if it is enabled for 'cat/disabled-1:0::fake'");
            TEST_CHECK(req2->requirement_met(&env, *id));
            TEST_CHECK(req2->requirement_met(&env, *id2));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req3(
                parse_elike_use_requirement("-pkgname?", id, ELikeUseRequirementOptions() + euro_allow_self_deps));
            TEST_CHECK_EQUAL(req3->as_raw_string(), "[-pkgname?]");
            TEST_CHECK_EQUAL(req3->as_human_string(), "Flag 'pkgname' disabled if it is enabled for 'cat/enabled-1:0::fake'");
            TEST_CHECK(! req3->requirement_met(&env, *id));
            TEST_CHECK(req3->requirement_met(&env, *id2));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req4(
                parse_elike_use_requirement("-pkgname?", id2, ELikeUseRequirementOptions() + euro_allow_self_deps));
            TEST_CHECK_EQUAL(req4->as_raw_string(), "[-pkgname?]");
            TEST_CHECK_EQUAL(req4->as_human_string(), "Flag 'pkgname' disabled if it is enabled for 'cat/disabled-1:0::fake'");
            TEST_CHECK(req4->requirement_met(&env, *id));
            TEST_CHECK(req4->requirement_met(&env, *id2));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req5(
                parse_elike_use_requirement("pkgname!?", id, ELikeUseRequirementOptions() + euro_allow_self_deps));
            TEST_CHECK_EQUAL(req5->as_raw_string(), "[pkgname!?]");
            TEST_CHECK_EQUAL(req5->as_human_string(), "Flag 'pkgname' enabled if it is disabled for 'cat/enabled-1:0::fake'");
            TEST_CHECK(req5->requirement_met(&env, *id));
            TEST_CHECK(req5->requirement_met(&env, *id2));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req6(
                parse_elike_use_requirement("pkgname!?", id2, ELikeUseRequirementOptions() + euro_allow_self_deps));
            TEST_CHECK_EQUAL(req6->as_raw_string(), "[pkgname!?]");
            TEST_CHECK_EQUAL(req6->as_human_string(), "Flag 'pkgname' enabled if it is disabled for 'cat/disabled-1:0::fake'");
            TEST_CHECK(req6->requirement_met(&env, *id));
            TEST_CHECK(! req6->requirement_met(&env, *id2));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req7(
                parse_elike_use_requirement("-pkgname!?", id, ELikeUseRequirementOptions() + euro_allow_self_deps));
            TEST_CHECK_EQUAL(req7->as_raw_string(), "[-pkgname!?]");
            TEST_CHECK_EQUAL(req7->as_human_string(), "Flag 'pkgname' disabled if it is disabled for 'cat/enabled-1:0::fake'");
            TEST_CHECK(req7->requirement_met(&env, *id));
            TEST_CHECK(req7->requirement_met(&env, *id2));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req8(
                parse_elike_use_requirement("-pkgname!?", id2, ELikeUseRequirementOptions() + euro_allow_self_deps));
            TEST_CHECK_EQUAL(req8->as_raw_string(), "[-pkgname!?]");
            TEST_CHECK_EQUAL(req8->as_human_string(), "Flag 'pkgname' disabled if it is disabled for 'cat/disabled-1:0::fake'");
            TEST_CHECK(! req8->requirement_met(&env, *id));
            TEST_CHECK(req8->requirement_met(&env, *id2));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req9(
                parse_elike_use_requirement("pkgname=", id, ELikeUseRequirementOptions() + euro_allow_self_deps));
            TEST_CHECK_EQUAL(req9->as_raw_string(), "[pkgname=]");
            TEST_CHECK_EQUAL(req9->as_human_string(), "Flag 'pkgname' enabled or disabled like it is for 'cat/enabled-1:0::fake'");
            TEST_CHECK(req9->requirement_met(&env, *id));
            TEST_CHECK(! req9->requirement_met(&env, *id2));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req10(
                parse_elike_use_requirement("pkgname=", id2, ELikeUseRequirementOptions() + euro_allow_self_deps));
            TEST_CHECK_EQUAL(req10->as_raw_string(), "[pkgname=]");
            TEST_CHECK_EQUAL(req10->as_human_string(), "Flag 'pkgname' enabled or disabled like it is for 'cat/disabled-1:0::fake'");
            TEST_CHECK(! req10->requirement_met(&env, *id));
            TEST_CHECK(req10->requirement_met(&env, *id2));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req11(
                parse_elike_use_requirement("pkgname!=", id, ELikeUseRequirementOptions() + euro_allow_self_deps));
            TEST_CHECK_EQUAL(req11->as_raw_string(), "[pkgname!=]");
            TEST_CHECK_EQUAL(req11->as_human_string(), "Flag 'pkgname' enabled or disabled opposite to how it is for 'cat/enabled-1:0::fake'");
            TEST_CHECK(! req11->requirement_met(&env, *id));
            TEST_CHECK(req11->requirement_met(&env, *id2));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req12(
                parse_elike_use_requirement("pkgname!=", id2, ELikeUseRequirementOptions() + euro_allow_self_deps));
            TEST_CHECK_EQUAL(req12->as_raw_string(), "[pkgname!=]");
            TEST_CHECK_EQUAL(req12->as_human_string(), "Flag 'pkgname' enabled or disabled opposite to how it is for 'cat/disabled-1:0::fake'");
            TEST_CHECK(req12->requirement_met(&env, *id));
            TEST_CHECK(! req12->requirement_met(&env, *id2));
        }
    } test_complex_use_requirements;

    struct ComplexUseRequirementsPortageSyntaxTest : TestCase
    {
        ComplexUseRequirementsPortageSyntaxTest() : TestCase("complex use requirements portage syntax") { }

        void run()
        {
            TestEnvironment env;
            std::tr1::shared_ptr<FakeRepository> fake(new FakeRepository(&env, RepositoryName("fake")));
            env.package_database()->add_repository(1, fake);
            std::tr1::shared_ptr<FakePackageID> id(fake->add_version("cat", "enabled", "1"));
            id->iuse_key()->set_from_string("pkgname", IUseFlagParseOptions());
            std::tr1::shared_ptr<FakePackageID> id2(fake->add_version("cat", "disabled", "1"));
            id2->iuse_key()->set_from_string("pkgname", IUseFlagParseOptions());

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req1(
                parse_elike_use_requirement("pkgname?", id, ELikeUseRequirementOptions() + euro_allow_self_deps + euro_portage_syntax));
            TEST_CHECK_EQUAL(req1->as_raw_string(), "[pkgname?]");
            TEST_CHECK_EQUAL(req1->as_human_string(), "Flag 'pkgname' enabled if it is enabled for 'cat/enabled-1:0::fake'");
            TEST_CHECK(req1->requirement_met(&env, *id));
            TEST_CHECK(! req1->requirement_met(&env, *id2));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req2(
                parse_elike_use_requirement("pkgname?", id2, ELikeUseRequirementOptions() + euro_allow_self_deps + euro_portage_syntax));
            TEST_CHECK_EQUAL(req2->as_raw_string(), "[pkgname?]");
            TEST_CHECK_EQUAL(req2->as_human_string(), "Flag 'pkgname' enabled if it is enabled for 'cat/disabled-1:0::fake'");
            TEST_CHECK(req2->requirement_met(&env, *id));
            TEST_CHECK(req2->requirement_met(&env, *id2));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req7(
                parse_elike_use_requirement("!pkgname?", id, ELikeUseRequirementOptions() + euro_allow_self_deps + euro_portage_syntax));
            TEST_CHECK_EQUAL(req7->as_raw_string(), "[!pkgname?]");
            TEST_CHECK_EQUAL(req7->as_human_string(), "Flag 'pkgname' disabled if it is disabled for 'cat/enabled-1:0::fake'");
            TEST_CHECK(req7->requirement_met(&env, *id));
            TEST_CHECK(req7->requirement_met(&env, *id2));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req8(
                parse_elike_use_requirement("!pkgname?", id2, ELikeUseRequirementOptions() + euro_allow_self_deps + euro_portage_syntax));
            TEST_CHECK_EQUAL(req8->as_raw_string(), "[!pkgname?]");
            TEST_CHECK_EQUAL(req8->as_human_string(), "Flag 'pkgname' disabled if it is disabled for 'cat/disabled-1:0::fake'");
            TEST_CHECK(! req8->requirement_met(&env, *id));
            TEST_CHECK(req8->requirement_met(&env, *id2));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req9(
                parse_elike_use_requirement("pkgname=", id, ELikeUseRequirementOptions() + euro_allow_self_deps + euro_portage_syntax));
            TEST_CHECK_EQUAL(req9->as_raw_string(), "[pkgname=]");
            TEST_CHECK_EQUAL(req9->as_human_string(), "Flag 'pkgname' enabled or disabled like it is for 'cat/enabled-1:0::fake'");
            TEST_CHECK(req9->requirement_met(&env, *id));
            TEST_CHECK(! req9->requirement_met(&env, *id2));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req10(
                parse_elike_use_requirement("pkgname=", id2, ELikeUseRequirementOptions() + euro_allow_self_deps + euro_portage_syntax));
            TEST_CHECK_EQUAL(req10->as_raw_string(), "[pkgname=]");
            TEST_CHECK_EQUAL(req10->as_human_string(), "Flag 'pkgname' enabled or disabled like it is for 'cat/disabled-1:0::fake'");
            TEST_CHECK(! req10->requirement_met(&env, *id));
            TEST_CHECK(req10->requirement_met(&env, *id2));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req11(
                parse_elike_use_requirement("!pkgname=", id, ELikeUseRequirementOptions() + euro_allow_self_deps + euro_portage_syntax));
            TEST_CHECK_EQUAL(req11->as_raw_string(), "[!pkgname=]");
            TEST_CHECK_EQUAL(req11->as_human_string(), "Flag 'pkgname' enabled or disabled opposite to how it is for 'cat/enabled-1:0::fake'");
            TEST_CHECK(! req11->requirement_met(&env, *id));
            TEST_CHECK(req11->requirement_met(&env, *id2));

            std::tr1::shared_ptr<const AdditionalPackageDepSpecRequirement> req12(
                parse_elike_use_requirement("!pkgname=", id2, ELikeUseRequirementOptions() + euro_allow_self_deps + euro_portage_syntax));
            TEST_CHECK_EQUAL(req12->as_raw_string(), "[!pkgname=]");
            TEST_CHECK_EQUAL(req12->as_human_string(), "Flag 'pkgname' enabled or disabled opposite to how it is for 'cat/disabled-1:0::fake'");
            TEST_CHECK(req12->requirement_met(&env, *id));
            TEST_CHECK(! req12->requirement_met(&env, *id2));
        }
    } test_complex_use_requirements_portage_syntax;

    struct MalformedUseRequirementsTest : TestCase
    {
        MalformedUseRequirementsTest() : TestCase("malformed use requirements") { }

        void run()
        {
            TestEnvironment env;
            std::tr1::shared_ptr<FakeRepository> fake(new FakeRepository(&env, RepositoryName("fake")));
            env.package_database()->add_repository(1, fake);
            std::tr1::shared_ptr<FakePackageID> id(fake->add_version("cat", "enabled", "1"));
            id->iuse_key()->set_from_string("pkgname", IUseFlagParseOptions());

            TEST_CHECK_THROWS(parse_elike_use_requirement("te/st", id, ELikeUseRequirementOptions() + euro_allow_self_deps), NameError);

            TEST_CHECK_THROWS(parse_elike_use_requirement("", id, ELikeUseRequirementOptions() + euro_allow_self_deps), ELikeUseRequirementError);
            TEST_CHECK_THROWS(parse_elike_use_requirement("-", id, ELikeUseRequirementOptions() + euro_allow_self_deps), ELikeUseRequirementError);
            TEST_CHECK_THROWS(parse_elike_use_requirement("?", id, ELikeUseRequirementOptions() + euro_allow_self_deps), ELikeUseRequirementError);
            TEST_CHECK_THROWS(parse_elike_use_requirement("-?", id, ELikeUseRequirementOptions() + euro_allow_self_deps), ELikeUseRequirementError);
            TEST_CHECK_THROWS(parse_elike_use_requirement("!?", id, ELikeUseRequirementOptions() + euro_allow_self_deps), ELikeUseRequirementError);
            TEST_CHECK_THROWS(parse_elike_use_requirement("-!?", id, ELikeUseRequirementOptions() + euro_allow_self_deps), ELikeUseRequirementError);
            TEST_CHECK_THROWS(parse_elike_use_requirement("=", id, ELikeUseRequirementOptions() + euro_allow_self_deps), ELikeUseRequirementError);
            TEST_CHECK_THROWS(parse_elike_use_requirement("!=", id, ELikeUseRequirementOptions() + euro_allow_self_deps), ELikeUseRequirementError);

            TEST_CHECK_THROWS(parse_elike_use_requirement("!test?", id, ELikeUseRequirementOptions() + euro_allow_self_deps), NameError);
            TEST_CHECK_THROWS(parse_elike_use_requirement("!test=", id, ELikeUseRequirementOptions() + euro_allow_self_deps), NameError);
            TEST_CHECK_THROWS(parse_elike_use_requirement("test1,test2", id, ELikeUseRequirementOptions() + euro_allow_self_deps), NameError);
        }
    } test_malformed_use_requirements;

    struct MalformedUseRequirementsPortageSyntaxTest : TestCase
    {
        MalformedUseRequirementsPortageSyntaxTest() : TestCase("malformed use requirements portage syntax") { }

        void run()
        {
            TestEnvironment env;
            std::tr1::shared_ptr<FakeRepository> fake(new FakeRepository(&env, RepositoryName("fake")));
            env.package_database()->add_repository(1, fake);
            std::tr1::shared_ptr<FakePackageID> id(fake->add_version("cat", "enabled", "1"));
            id->iuse_key()->set_from_string("pkgname", IUseFlagParseOptions());

            TEST_CHECK_THROWS(parse_elike_use_requirement("te/st", id, ELikeUseRequirementOptions() + euro_allow_self_deps + euro_portage_syntax), NameError);

            TEST_CHECK_THROWS(parse_elike_use_requirement("", id, ELikeUseRequirementOptions() + euro_allow_self_deps + euro_portage_syntax), ELikeUseRequirementError);
            TEST_CHECK_THROWS(parse_elike_use_requirement("-", id, ELikeUseRequirementOptions() + euro_allow_self_deps + euro_portage_syntax), ELikeUseRequirementError);
            TEST_CHECK_THROWS(parse_elike_use_requirement("?", id, ELikeUseRequirementOptions() + euro_allow_self_deps + euro_portage_syntax), ELikeUseRequirementError);
            TEST_CHECK_THROWS(parse_elike_use_requirement("!?", id, ELikeUseRequirementOptions() + euro_allow_self_deps + euro_portage_syntax), ELikeUseRequirementError);
            TEST_CHECK_THROWS(parse_elike_use_requirement("=", id, ELikeUseRequirementOptions() + euro_allow_self_deps + euro_portage_syntax), ELikeUseRequirementError);
            TEST_CHECK_THROWS(parse_elike_use_requirement("!=", id, ELikeUseRequirementOptions() + euro_allow_self_deps + euro_portage_syntax), ELikeUseRequirementError);

            TEST_CHECK_THROWS(parse_elike_use_requirement(",", id, ELikeUseRequirementOptions() + euro_allow_self_deps + euro_portage_syntax), ELikeUseRequirementError);
            TEST_CHECK_THROWS(parse_elike_use_requirement("test,", id, ELikeUseRequirementOptions() + euro_allow_self_deps + euro_portage_syntax), ELikeUseRequirementError);
            TEST_CHECK_THROWS(parse_elike_use_requirement(",test", id, ELikeUseRequirementOptions() + euro_allow_self_deps + euro_portage_syntax), ELikeUseRequirementError);
            TEST_CHECK_THROWS(parse_elike_use_requirement("test,,test", id, ELikeUseRequirementOptions() + euro_allow_self_deps + euro_portage_syntax), ELikeUseRequirementError);

            TEST_CHECK_THROWS(parse_elike_use_requirement("test!?", id, ELikeUseRequirementOptions() + euro_allow_self_deps + euro_portage_syntax), NameError);
            TEST_CHECK_THROWS(parse_elike_use_requirement("-test?", id, ELikeUseRequirementOptions() + euro_allow_self_deps + euro_portage_syntax), NameError);
            TEST_CHECK_THROWS(parse_elike_use_requirement("-test!?", id, ELikeUseRequirementOptions() + euro_allow_self_deps + euro_portage_syntax), NameError);
            TEST_CHECK_THROWS(parse_elike_use_requirement("test!=", id, ELikeUseRequirementOptions() + euro_allow_self_deps + euro_portage_syntax), NameError);
        }
    } test_malformed_use_requirements_portage_syntax;
}
