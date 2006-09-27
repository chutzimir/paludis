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

#include "find_reverse_deps.hh"
#include "command_line.hh"
#include "colour.hh"

#include <paludis/util/compare.hh>
#include <paludis/util/collection_concrete.hh>
#include <paludis/util/save.hh>

#include <set>
#include <map>
#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace paludis;
using std::cout;
using std::cerr;
using std::endl;

namespace
{
    class ReverseDepChecker :
        public DepAtomVisitorTypes::ConstVisitor
    {
        private:
            PackageDatabase::ConstPointer _db;
            PackageDatabaseEntryCollection::ConstPointer _entries;
            std::string _depname;
            std::string _p;

            bool _in_any;
            bool _in_use;
            std::string _flags;

            bool _found_matches;

        public:
            ReverseDepChecker(PackageDatabase::ConstPointer db, PackageDatabaseEntryCollection::ConstPointer entries,
                    const std::string & p) :
                _db(db),
                _entries(entries),
                _depname(""),
                _p(p),
                _in_any(false),
                _in_use(false),
                _found_matches(false)
            {
            }

            void check(DepAtom::ConstPointer atom, const std::string & depname)
            {
                _depname = depname;
                atom->accept(this);
            }

            bool found_matches()
            {
                return _found_matches;
            }

            void visit(const AllDepAtom * const);

            void visit(const AnyDepAtom * const);

            void visit(const UseDepAtom * const);

            void visit(const PackageDepAtom * const);

            void visit(const PlainTextDepAtom * const); 

            void visit(const BlockDepAtom * const);
    };

    void
    ReverseDepChecker::visit(const AllDepAtom * const a)
    {
        std::for_each(a->begin(), a->end(), accept_visitor(this));
    }

    void
    ReverseDepChecker::visit(const AnyDepAtom * const a)
    {
        Save<bool> in_any_save(&_in_any, true);

        std::for_each(a->begin(), a->end(), accept_visitor(this));
    }

    void
    ReverseDepChecker::visit(const UseDepAtom * const a)
    {
        Save<bool> in_use_save(&_in_use, true);
        Save<std::string> flag_save(&_flags);

        if (! _flags.empty())
            _flags += " ";
        _flags += (a->inverse() ? "!" : "") + stringify(a->flag());

        std::for_each(a->begin(), a->end(), accept_visitor(this));
    }

    void
    ReverseDepChecker::visit(const PackageDepAtom * const a)
    {
        PackageDatabaseEntryCollection::ConstPointer dep_entries(
                _db->query(*a, is_uninstalled_only));
        PackageDatabaseEntryCollection::Pointer matches(new PackageDatabaseEntryCollection::Concrete);

        bool header_written = false;

        for (PackageDatabaseEntryCollection::Iterator e(dep_entries->begin()), e_end(dep_entries->end()) ;
                e != e_end ; ++e)
        {
            if (_entries->find(*e) != _entries->end())
            {
                _found_matches |= true;

                if (! header_written)
                {
                    std::cout << "  " << _p << " " + _depname + " on one of:" << std::endl;
                    header_written = true;
                }
                std::cout << "    " << stringify(*e);

                if (_in_use || _in_any)
                {
                    std::cout << " (";

                    if (_in_any)
                        std::cout << "any-of";

                    if (_in_use && _in_any)
                        std::cout << ", ";

                    if (_in_use)
                        std::cout << "condition USE='" << _flags << "'";

                    std::cout << ")";
                }
                std::cout << std::endl;
            }
        }
    }

    void
    ReverseDepChecker::visit(const PlainTextDepAtom * const)
    {
    }

    void
    ReverseDepChecker::visit(const BlockDepAtom * const)
    {
    }

    void write_repository_header(std::string atom, const std::string &)
    {
        cout << "Reverse dependencies for '" << atom << "':" << std::endl;
    }

    int check_one_package(const Environment & env, const Repository & r,
            const PackageDatabaseEntryCollection & entries, const QualifiedPackageName & p)
    {
        Context context("When checking package '" + stringify(p) + "':");

        cerr << xterm_title("Checking " + stringify(p) + " - adjutrix");

        PackageDatabaseEntryCollection::Pointer p_entries(env.package_database()->query(
                PackageDepAtom::Pointer(new PackageDepAtom(stringify(p))), is_uninstalled_only));

        bool found_matches(false);

        for (PackageDatabaseEntryCollection::Iterator e(p_entries->begin()), e_end(p_entries->end()) ;
                e != e_end ; ++e)
        {
            try
            {
                VersionMetadata::ConstPointer metadata(r.version_metadata(e->name, e->version));
                ReverseDepChecker checker(env.package_database(), PackageDatabaseEntryCollection::ConstPointer(&entries),
                        stringify(p) + "-" + stringify(e->version));

                checker.check(metadata->deps.parser(metadata->deps.build_depend_string), std::string("DEPEND"));
                checker.check(metadata->deps.parser(metadata->deps.run_depend_string), std::string("RDEPEND"));
                checker.check(metadata->deps.parser(metadata->deps.post_depend_string), std::string("PDEPEND"));

                found_matches |= checker.found_matches();
            }
            catch (Exception & exception)
            {
                cerr << "Caught exception:" << endl;
                cerr << "  * " << exception.backtrace("\n  * ") << endl;
                cerr << "  * " << exception.message() << " (" << exception.what() << ")" << endl;
                return (found_matches ? 0 : 1) | 2;
            }
        }

        return found_matches ? 0 : 1;
    }
}

int do_find_reverse_deps(NoConfigEnvironment & env)
{
    Context context("When performing find-reverse-deps action:");

    env.set_profile(env.main_repository_dir() / "profiles" / "default-linux" / "amd64" / "2006.1");

    PackageDepAtom::Pointer atom(0);
    try
    {
        if (std::string::npos == CommandLine::get_instance()->begin_parameters()->find('/'))
        {
            atom.assign(new PackageDepAtom(env.package_database()->fetch_unique_qualified_package_name(
                            PackageNamePart(*CommandLine::get_instance()->begin_parameters()))));
        }
        else
            atom.assign(new PackageDepAtom(*CommandLine::get_instance()->begin_parameters()));
    }
    catch (const AmbiguousPackageNameError & e)
    {
        cout << endl;
        cerr << "Query error:" << endl;
        cerr << "  * " << e.backtrace("\n  * ");
        cerr << "Ambiguous package name '" << e.name() << "'. Did you mean:" << endl;
        for (AmbiguousPackageNameError::OptionsIterator o(e.begin_options()),
                o_end(e.end_options()) ; o != o_end ; ++o)
            cerr << "    * " << colour(cl_package_name, *o) << endl;
        cerr << endl;
        return 4;
    }

    PackageDatabaseEntryCollection::Pointer entries(env.package_database()->query(atom, is_either));
    int ret(0);

    if (entries->empty())
        return 1;

    for (IndirectIterator<PackageDatabase::RepositoryIterator, const Repository>
            r(env.package_database()->begin_repositories()),
            r_end(env.package_database()->end_repositories()) ; r != r_end ; ++r)
    {
        if (r->name() == RepositoryName("virtuals"))
            continue;

        write_repository_header(stringify(*atom), stringify(r->name()));

        CategoryNamePartCollection::ConstPointer cat_names(r->category_names());
        for (CategoryNamePartCollection::Iterator c(cat_names->begin()), c_end(cat_names->end()) ;
                c != c_end ; ++c)
        {
            if (CommandLine::get_instance()->a_category.specified())
                if (CommandLine::get_instance()->a_category.args_end() == std::find(
                            CommandLine::get_instance()->a_category.args_begin(),
                            CommandLine::get_instance()->a_category.args_end(),
                            stringify(*c)))
                    continue;

            QualifiedPackageNameCollection::ConstPointer pkg_names(r->package_names(*c));
            for (QualifiedPackageNameCollection::Iterator p(pkg_names->begin()), p_end(pkg_names->end()) ;
                    p != p_end ; ++p)
            {
                if (CommandLine::get_instance()->a_package.specified())
                    if (CommandLine::get_instance()->a_package.args_end() == std::find(
                                CommandLine::get_instance()->a_package.args_begin(),
                                CommandLine::get_instance()->a_package.args_end(),
                                stringify(p->package)))
                        continue;

                ret |= check_one_package(env, *r, *entries, *p);
            }
        }
    }

    return ret;
}


