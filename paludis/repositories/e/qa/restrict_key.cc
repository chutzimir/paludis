/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2006, 2007, 2008, 2009 Ciaran McCreesh
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

#include "restrict_key.hh"
#include <paludis/qa.hh>
#include <paludis/metadata_key.hh>
#include <paludis/util/log.hh>
#include <paludis/util/instantiation_policy-impl.hh>
#include <paludis/name.hh>
#include <paludis/dep_spec.hh>
#include <paludis/package_id.hh>
#include <paludis/util/fs_entry.hh>
#include <set>

using namespace paludis;
using namespace paludis::erepository;

namespace
{
    struct AllowedRestricts :
        InstantiationPolicy<AllowedRestricts, instantiation_method::SingletonTag>
    {
        std::set<std::string> allowed_restricts;

        AllowedRestricts()
        {
            allowed_restricts.insert("fetch");
            allowed_restricts.insert("mirror");
            allowed_restricts.insert("nomirror");
            allowed_restricts.insert("primaryuri");
            allowed_restricts.insert("nostrip");
            allowed_restricts.insert("strip");
            allowed_restricts.insert("sandbox");
            allowed_restricts.insert("sydbox");
            allowed_restricts.insert("userpriv");
            allowed_restricts.insert("test");
        }
    };

    struct RestrictChecker
    {
        const std::set<std::string> & allowed_restricts;

        const std::tr1::shared_ptr<const MetadataKey> & key;
        const FSEntry entry;
        QAReporter & reporter;
        const std::tr1::shared_ptr<const PackageID> id;
        const std::string name;

        RestrictChecker(
                const std::tr1::shared_ptr<const MetadataKey> & k,
                const FSEntry & f,
                QAReporter & r,
                const std::tr1::shared_ptr<const PackageID> & i,
                const std::string & n) :
            allowed_restricts(AllowedRestricts::get_instance()->allowed_restricts),
            key(k),
            entry(f),
            reporter(r),
            id(i),
            name(n)
        {
        }

        void visit(const PlainTextSpecTree::NodeType<PlainTextLabelDepSpec>::Type & node)
        {
            reporter.message(QAMessage(entry, qaml_normal, name,
                        "Unexpected label '" + stringify(node.spec()->text()) + "' in '" + key->raw_name() + "@")
                    .with_associated_id(id)
                    .with_associated_key(id, key));
        }

        void visit(const PlainTextSpecTree::NodeType<PlainTextDepSpec>::Type & node)
        {
            if (allowed_restricts.end() == allowed_restricts.find(node.spec()->text()))
                reporter.message(QAMessage(entry, qaml_normal, name,
                            "Unrecognised value '" + node.spec()->text() + "' in '" + key->raw_name() + "'")
                        .with_associated_id(id)
                        .with_associated_key(id, key));

            else if (0 == node.spec()->text().compare(0, 2, "no"))
                reporter.message(QAMessage(entry, qaml_minor, name,
                            "Deprecated value '" + node.spec()->text() + "' in '" + key->raw_name() + "' (use '" + node.spec()->text().substr(2) + "' instead)")
                        .with_associated_id(id)
                        .with_associated_key(id, key));
        }

        void visit(const PlainTextSpecTree::NodeType<AllDepSpec>::Type & node)
        {
            std::for_each(indirect_iterator(node.begin()), indirect_iterator(node.end()), accept_visitor(*this));
        }

        void visit(const PlainTextSpecTree::NodeType<ConditionalDepSpec>::Type & node)
        {
            std::for_each(indirect_iterator(node.begin()), indirect_iterator(node.end()), accept_visitor(*this));
        }
    };
}

bool
paludis::erepository::restrict_key_check(
        const FSEntry & entry,
        QAReporter & reporter,
        const std::tr1::shared_ptr<const ERepositoryID> & id,
        const std::string & name)
{
    Context context("When performing check '" + name + "' using restrict_key_check on ID '" + stringify(*id) + "':");
    Log::get_instance()->message("e.qa.restrict_key_check", ll_debug, lc_context) << "restrict_key_check '"
        << entry << "', " << *id << "', " << name << "'";

    if (id->restrict_key())
    {
        RestrictChecker r(id->restrict_key(), entry, reporter, id, name);
        id->restrict_key()->value()->root()->accept(r);
    }

    return true;
}
