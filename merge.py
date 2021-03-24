#!/bin/python3

import os

out = 'odbctemplate_all.hpp'

files = [
'odbc_inc.hpp',
'odbc_def.hpp',
'odbc_error.hpp',
'odbc_dbc.hpp',
'odbc_manager.hpp',
'odbc_type.hpp',
'odbc_helper.hpp',
'odbc_stmt.hpp',
'odbc_fetcher.hpp',
'odbc_prepared_stmt.hpp',
'odbc_reseted_stmt.hpp',
'odbc_connect.hpp',
'odbc_connect_interface.hpp',
'odbc_connect_builder.hpp']

with open('./odbctemplate_all.hpp', 'w') as outfile:
    for file in files:
        with open(file) as infile:
            if file == 'odbc_inc.hpp':
                for line in infile:
                    outfile.write(line)
                outfile.write('\n')
            else:
                for i, line in enumerate(infile):
                    if i == 0 or i == 1 or i == 2:
                        continue
                    outfile.write(line)
                outfile.write('\n')

