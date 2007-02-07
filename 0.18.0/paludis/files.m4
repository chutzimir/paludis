dnl vim: set ft=m4 et :
dnl This file is used by Makefile.am.m4 and paludis.hh.m4. You should
dnl use the provided autogen.bash script to do all the hard work.
dnl
dnl This file is used to avoid having to make lots of repetitive changes in
dnl Makefile.am every time we add a source or test file. The first parameter is
dnl the base filename with no extension; later parameters can be `hh', `cc',
dnl `test', `impl', `testscript'. Note that there isn't much error checking done
dnl on this file at present...

add(`about',                             `hh', `test')
add(`config_file',                       `hh', `cc', `test', `testscript')
add(`contents',                          `hh', `cc')
add(`dep_atom',                          `hh', `cc', `test')
add(`dep_atom_flattener',                `hh', `cc')
add(`dep_atom_pretty_printer',           `hh', `cc')
add(`dep_tag',                           `hh', `cc', `sr')
add(`ebuild',                            `hh', `cc', `sr')
add(`environment',                       `hh', `cc')
add(`hashed_containers',                 `hhx', `cc', `test')
add(`host_tuple_name',                   `hh', `cc', `sr', `test')
add(`mask_reasons',                      `hh', `cc')
add(`match_package',                     `hh', `cc')
add(`name',                              `hh', `cc', `test', `sr')
add(`package_database',                  `hh', `cc', `test')
add(`package_database_entry',            `hh', `cc', `sr')
add(`paludis',                           `hh', `cc')
add(`portage_dep_lexer',                 `hh', `cc', `test')
add(`portage_dep_parser',                `hh', `cc', `test')
add(`repository',                        `hh', `cc', `sr')
add(`repository_name_cache',             `hh', `cc', `test', `testscript')
add(`syncer',                            `hh', `cc', `sr')
add(`version_metadata',                  `hh', `cc', `sr')
add(`version_operator',                  `hh', `cc', `test')
add(`version_requirements',              `hh', `cc', `sr')
add(`version_spec',                      `hh', `cc', `sr', `test')

