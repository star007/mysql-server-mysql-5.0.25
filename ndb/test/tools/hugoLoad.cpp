/* Copyright (C) 2003 MySQL AB

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */


#include <NdbOut.hpp>
#include <NdbApi.hpp>
#include <NdbSleep.h>
#include <NDBT.hpp>
#include <HugoTransactions.hpp>
#include <getarg.h>


int main(int argc, const char** argv){
  ndb_init();

  int _records = 0;
  const char* _tabname = NULL;
  int _help = 0;
  int _batch = 512;
  const char* db = 0;

  struct getargs args[] = {
    { "records", 'r', arg_integer, &_records, "Number of records", "recs" },
    { "batch", 'b', arg_integer, &_batch, "Number of operations in each transaction", "batch" },
    { "database", 'd', arg_string, &db, "Database", "" },
    { "usage", '?', arg_flag, &_help, "Print help", "" }
  };
  int num_args = sizeof(args) / sizeof(args[0]);
  int optind = 0;
  char desc[] = 
    "tabname\n"\
    "This program will load one table in Ndb with calculated data. \n"\
    "This means that it is possible to check the validity of the data \n"\
    "at a later time. The last column in each table is used as an update \n"\
    "counter, it's initialised to zero and should be incremented for each \n"\
    "update of the record. \n";
  
  if(getarg(args, num_args, argc, argv, &optind) ||
     argv[optind] == NULL || _records == 0 || _help) {
    arg_printusage(args, num_args, argv[0], desc);
    return NDBT_ProgramExit(NDBT_WRONGARGS);
  }
  _tabname = argv[optind];
  
  // Connect to Ndb
  Ndb_cluster_connection con;
  if(con.connect(12, 5, 1) != 0)
  {
    return NDBT_ProgramExit(NDBT_FAILED);
  }
  Ndb MyNdb( &con, db ? db : "TEST_DB" );

  if(MyNdb.init() != 0){
    ERR(MyNdb.getNdbError());
    return NDBT_ProgramExit(NDBT_FAILED);
  }

  // Connect to Ndb and wait for it to become ready
  while(MyNdb.waitUntilReady() != 0)
    ndbout << "Waiting for ndb to become ready..." << endl;
   
  // Check if table exists in db
  const NdbDictionary::Table* pTab = NDBT_Table::discoverTableFromDb(&MyNdb, _tabname);
  if(pTab == NULL){
    ndbout << " Table " << _tabname << " does not exist!" << endl;
    return NDBT_ProgramExit(NDBT_WRONGARGS);
  }

  HugoTransactions hugoTrans(*pTab);
  if (hugoTrans.loadTable(&MyNdb, 
			   _records,
			  _batch) != 0){
    return NDBT_ProgramExit(NDBT_FAILED);
  }

  return NDBT_ProgramExit(NDBT_OK);
}
