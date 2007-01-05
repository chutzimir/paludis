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

#include <paludis/args/args.hh>
#include <paludis/paludis.hh>
#include <paludis/util/util.hh>

#include <cstdlib>
#include <iostream>
#include <algorithm>

#include <libebt/libebt.hh>
#include <libwrapiter/libwrapiter.hh>

#include "command_line.hh"
#include "stage.hh"
#include "config.h"
#include "stage_builder.hh"

using namespace paludis;
using std::cout;
using std::cerr;
using std::endl;

struct DoVersion
{
};

int main(int argc, char *argv[])
{
    Context context("In main program:");

    try
    {
        CommandLine::get_instance()->run(argc, argv);

        if (CommandLine::get_instance()->a_help.specified())
            throw DoHelp();

        if (CommandLine::get_instance()->a_log_level.specified())
            Log::get_instance()->set_log_level(CommandLine::get_instance()->a_log_level.option());
        else
            Log::get_instance()->set_log_level(ll_qa);

        if (CommandLine::get_instance()->a_version.specified())
            throw DoVersion();

        if (! CommandLine::get_instance()->a_target.specified())
            throw DoHelp("you need to specify a --target");

        std::string stage;
        if (CommandLine::get_instance()->a_stage.specified())
            stage = CommandLine::get_instance()->a_stage.argument();
        else
            stage = "cxx";

        ContrariusStageOptions contrarius_opts(HostTupleName(CommandLine::get_instance()->a_target.argument()),
            CommandLine::get_instance()->a_binutils_name.argument(),
            CommandLine::get_instance()->a_binutils_version.argument(),
            CommandLine::get_instance()->a_gcc_name.argument(),
            CommandLine::get_instance()->a_gcc_version.argument(),
            CommandLine::get_instance()->a_headers_name.argument(),
            CommandLine::get_instance()->a_headers_version.argument(),
            CommandLine::get_instance()->a_libc_name.argument(),
            CommandLine::get_instance()->a_libc_version.argument());

        StageOptions stage_opts(CommandLine::get_instance()->a_pretend.specified(),
            CommandLine::get_instance()->a_fetch.specified(),
            CommandLine::get_instance()->a_always_rebuild.specified());

        OurStageBuilderTask builder(stage_opts);

        do
        {
            builder.queue_stage(StageBase::ConstPointer(new BinutilsStage(contrarius_opts)));

            if (stage == "binutils")
                break;

            if (CommandLine::get_instance()->a_headers.specified())
                builder.queue_stage(StageBase::ConstPointer(new KernelHeadersStage(contrarius_opts)));

            builder.queue_stage(StageBase::ConstPointer(new MinimalStage(contrarius_opts)));

            if (stage == "minimal")
                break;

            if (! CommandLine::get_instance()->a_headers.specified())
                builder.queue_stage(StageBase::ConstPointer(new KernelHeadersStage(contrarius_opts)));

            if (stage == "headers")
                break;

            builder.queue_stage(StageBase::ConstPointer(new LibCStage(contrarius_opts)));

            if (stage == "libc")
                break;

            builder.queue_stage(StageBase::ConstPointer(new FullStage(contrarius_opts)));

        } while (false);

        builder.execute();

        return EXIT_SUCCESS;
    }
    catch (const DoVersion &)
    {
        cout << "contrarius, part of " << PALUDIS_PACKAGE << " " << PALUDIS_VERSION_MAJOR << "."
            << PALUDIS_VERSION_MINOR << "." << PALUDIS_VERSION_MICRO;
        if (! std::string(PALUDIS_SUBVERSION_REVISION).empty())
            cout << " svn " << PALUDIS_SUBVERSION_REVISION;
        cout << endl << endl;
        cout << "Built by " << PALUDIS_BUILD_USER << "@" << PALUDIS_BUILD_HOST
            << " on " << PALUDIS_BUILD_DATE << endl;
        cout << "CXX:         " << PALUDIS_BUILD_CXX
#if defined(__ICC)
            << " " << __ICC
#elif defined(__VERSION__)
            << " " << __VERSION__
#endif
            << endl;
        cout << "CXXFLAGS:    " << PALUDIS_BUILD_CXXFLAGS << endl;
        cout << "LDFLAGS:     " << PALUDIS_BUILD_LDFLAGS << endl;
        cout << "SYSCONFDIR:  " << SYSCONFDIR << endl;
        cout << "LIBEXECDIR:  " << LIBEXECDIR << endl;
        cout << "BIGTEMPDIR:  " << BIGTEMPDIR << endl;
        cout << "stdlib:      "
#if defined(__GLIBCXX__)
#  define XSTRINGIFY(x) #x
#  define STRINGIFY(x) XSTRINGIFY(x)
            << "GNU libstdc++ " << STRINGIFY(__GLIBCXX__)
#endif
            << endl;

        cout << "libebt:      " << LIBEBT_VERSION_MAJOR << "." << LIBEBT_VERSION_MINOR
            << "." << LIBEBT_VERSION_MICRO << endl;
        cout << "libwrapiter: " << LIBWRAPITER_VERSION_MAJOR << "." << LIBWRAPITER_VERSION_MINOR
            << "." << LIBWRAPITER_VERSION_MICRO << endl;

        cout << endl;
        cout << "Paludis comes with ABSOLUTELY NO WARRANTY. Paludis is free software, and you" << endl;
        cout << "are welcome to redistribute it under the terms of the GNU General Public" << endl;
        cout << "License, version 2." << endl;

        return EXIT_SUCCESS;
    }
    catch (const paludis::args::ArgsError & e)
    {
        cerr << "Usage error: " << e.message() << endl;
        cerr << "Try " << argv[0] << " --help" << endl;
        return EXIT_FAILURE;
    }
    catch (const DoHelp & h)
    {
        if (h.message.empty())
        {
            cout << "Usage: " << argv[0] << " [options]" << endl;
            cout << "   or: " << argv[0] << " [package/category ..]" << endl;
            cout << endl;
            cout << *CommandLine::get_instance();
            return EXIT_SUCCESS;
        }
        else
        {
            cerr << "Usage error: " << h.message << endl;
            cerr << "Try " << argv[0] << " --help" << endl;
            return EXIT_FAILURE;
        }
    }
    catch (const Exception & e)
    {
        cout << endl;
        cerr << "Unhandled exception:" << endl
            << "  * " << e.backtrace("\n  * ")
            << e.message() << " (" << e.what() << ")" << endl;
        return EXIT_FAILURE;
    }
    catch (const std::exception & e)
    {
        cout << endl;
        cerr << "Unhandled exception:" << endl
            << "  * " << e.what() << endl;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        cout << endl;
        cerr << "Unhandled exception:" << endl
            << "  * Unknown exception type. Ouch..." << endl;
        return EXIT_FAILURE;
    }
}

