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
add(`action',                            `hh', `cc', `fwd')
add(`action_names',                      `hh', `cc', `fwd')
add(`condition_tracker',                 `hh', `cc')
add(`choice',                            `hh', `cc', `fwd')
add(`contents',                          `hh', `cc', `fwd')
add(`dep_label',                         `hh', `cc', `fwd')
add(`dep_list',                          `hh', `cc', `fwd', `sr', `test', `fwd')
add(`dep_list_exceptions',               `hh', `cc')
add(`dep_list_options',                  `hh', `cc', `se')
add(`dep_spec',                          `hh', `cc', `test', `fwd')
add(`dep_spec_flattener',                `hh', `cc')
add(`dep_tree',                          `hh', `cc', `fwd')
add(`dep_tag',                           `hh', `cc', `fwd', `sr')
add(`distribution',                      `hh', `cc', `impl', `fwd')
add(`elike_choices',                     `hh', `cc', `fwd')
add(`elike_dep_parser',                  `hh', `cc', `fwd', `test')
add(`elike_conditional_dep_spec',        `hh', `cc', `fwd')
add(`elike_package_dep_spec',            `hh', `cc', `fwd', `se')
add(`elike_slot_requirement',            `hh', `cc', `fwd')
add(`elike_use_requirement',             `hh', `cc', `fwd', `se', `test')
add(`environment',                       `hh', `fwd', `cc')
add(`environment_factory',               `hh', `fwd', `cc')
add(`environment_implementation',        `hh', `cc')
add(`filter',                            `hh', `cc', `fwd', `test')
add(`filter_handler',                    `hh', `cc', `fwd')
add(`filtered_generator',                `hh', `cc', `fwd', `test')
add(`find_unused_packages_task',         `hh', `cc')
add(`fuzzy_finder',                      `hh', `cc', `test')
add(`formatter',                         `hh', `fwd', `cc')
add(`generator',                         `hh', `cc', `fwd', `test')
add(`generator_handler',                 `hh', `cc', `fwd')
add(`handled_information',               `hh', `fwd', `cc')
add(`hook',                              `hh', `cc', `fwd', `se')
add(`hooker',                            `hh', `cc', `test', `testscript')
add(`install_task',                      `hh', `cc', `se')
add(`literal_metadata_key',              `hh', `cc')
add(`mask',                              `hh', `cc', `fwd')
add(`match_package',                     `hh', `cc', `se', `fwd')
add(`merger',                            `hh', `cc', `fwd', `se', `test', `testscript')
add(`merger_entry_type',                 `hh', `cc', `se')
add(`metadata_key',                      `hh', `cc', `se', `fwd')
add(`metadata_key_holder',               `hh', `cc', `fwd')
add(`name',                              `hh', `cc', `fwd', `test', `sr')
add(`ndbam',                             `hh', `cc', `fwd')
add(`ndbam_merger',                      `hh', `cc')
add(`ndbam_unmerger',                    `hh', `cc')
add(`override_functions',                `hh', `cc')
add(`package_database',                  `hh', `cc', `fwd', `test')
add(`package_id',                        `hh', `cc', `fwd', `se')
add(`paludis',                           `hh')
add(`qa',                                `hh', `cc', `fwd', `se')
add(`query_visitor',                     `hh', `cc')
add(`range_rewriter',                    `hh', `cc', `test')
add(`report_task',                       `hh', `cc')
add(`repository',                        `hh', `fwd', `cc')
add(`repository_factory',                `hh', `fwd', `cc')
add(`repository_name_cache',             `hh', `cc', `test', `testscript')
add(`selection',                         `hh', `cc', `fwd', `test')
add(`selection_handler',                 `hh', `cc', `fwd')
add(`set_file',                          `hh', `cc', `se', `sr', `test', `testscript')
add(`show_suggest_visitor',              `hh', `cc')
add(`slot_requirement',                  `hh', `fwd', `cc')
add(`stringify_formatter',               `hh', `cc', `fwd', `impl', `test')
add(`stripper',                          `hh', `cc', `fwd', `test', `testscript')
add(`syncer',                            `hh', `cc')
add(`sync_task',                         `hh', `cc')
add(`tasks_exceptions',                  `hh', `cc')
add(`unchoices_key',                     `hh', `cc', `fwd')
add(`uninstall_list',                    `hh', `cc', `se', `sr', `test')
add(`uninstall_task',                    `hh', `cc')
add(`unmerger',                          `hh', `cc')
add(`user_dep_spec',                     `hh', `cc', `se', `fwd', `test')
add(`version_operator',                  `hh', `cc', `fwd', `se', `test')
add(`version_requirements',              `hh', `cc', `fwd', `sr')
add(`version_spec',                      `hh', `cc', `se', `fwd', `test')

