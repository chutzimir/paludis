/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2006 Ciaran McCreesh <ciaranm@ciaranm.org>
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

#ifndef PALUDIS_GUARD_SRC_GTKPALUDIS_MAIN_WINDOW_HH
#define PALUDIS_GUARD_SRC_GTKPALUDIS_MAIN_WINDOW_HH 1

#include <gtkmm/window.h>
#include <paludis/util/instantiation_policy.hh>
#include <paludis/util/private_implementation_pattern.hh>

namespace gtkpaludis
{
    class MainWindow :
        public paludis::InstantiationPolicy<MainWindow, paludis::instantiation_method::SingletonAsNeededTag>,
        private paludis::PrivateImplementationPattern<MainWindow>,
        public Gtk::Window
    {
        friend class paludis::InstantiationPolicy<MainWindow, paludis::instantiation_method::SingletonAsNeededTag>;

        private:
            MainWindow();
            virtual ~MainWindow();

            void _set_lock(bool value);

        public:
            virtual void populate();

            void lock_controls();
            void maybe_unlock_controls();

            void show_exception(const std::string & what, const std::string & message, bool fatal);
            void message(const std::string &);
            void push_status(const std::string &);
            void pop_status();

            void show_messages_page();
    };
}

#endif
