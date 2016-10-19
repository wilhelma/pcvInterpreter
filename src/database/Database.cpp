/**
 *
 *    @file  Database.cpp
 *   @brief  
 *
 *    @date  10/17/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "Database.h"

#include "DBManager.h"
#include "SQLException.h"
#include "SQLStatementIterator.h"

// Tables
#include "AccessTable.h"
#include "CallTable.h"
#include "FileTable.h"
#include "FunctionTable.h"
#include "InstructionTable.h"
#include "LoopTable.h"
#include "LoopExecutionTable.h"
#include "LoopIterationTable.h"
#include "ReferenceTable.h"
#include "SegmentTable.h"
#include "ThreadTable.h"
#include "DBTable.h"

#include <memory>
#include <string>

// logging system
#include "easylogging++.h"

// Helper function to fill the tables
template<typename IdT, typename T>
inline void fill(const std::string& query_string, const DBManager& db, DBTable<IdT, T>& table) {
	std::copy(SQLStatementIterator<T>(db.query(query_string)),
			  SQLStatementIterator<T>::end(), inserter(table));
}

std::unique_ptr<const Database> load_database(const std::string& DBPath) {
	// try to open the database
	DBManager sql_input_db;
	try {
		sql_input_db.open(DBPath);
	} catch (const SQLException& e) {
		LOG(FATAL) <<e.what();
		std::abort();
	}

    auto db = std::make_unique<Database>();

    // Read from all the tables in the database
    fill("SELECT * from Access;",        sql_input_db, db->accessTable());
    fill("SELECT * from Call;",          sql_input_db, db->callTable());
    fill("SELECT * from File;",          sql_input_db, db->fileTable());
    fill("SELECT * from Function;",      sql_input_db, db->functionTable());
    fill("SELECT * from Instruction;",   sql_input_db, db->instructionTable());
    fill("SELECT * from Loop;",          sql_input_db, db->loopTable());
    fill("SELECT * from LoopExecution;", sql_input_db, db->loopExecutionTable());
    fill("SELECT * from LoopIteration;", sql_input_db, db->loopIterationTable());
    fill("SELECT * from Reference;",     sql_input_db, db->referenceTable());
    fill("SELECT * from Segment;",       sql_input_db, db->segmentTable());
    fill("SELECT * from Thread;",        sql_input_db, db->threadTable());

    LOG(TRACE) << "Rows in Access:        " << db->accessTable().size();
    LOG(TRACE) << "Rows in Call:          " << db->callTable().size();
    LOG(TRACE) << "Rows in File:          " << db->fileTable().size();
    LOG(TRACE) << "Rows in Function:      " << db->functionTable().size();
    LOG(TRACE) << "Rows in Instruction:   " << db->instructionTable().size();
    LOG(TRACE) << "Rows in Loop:          " << db->loopTable().size();
    LOG(TRACE) << "Rows in LoopExecution: " << db->loopExecutionTable().size();
    LOG(TRACE) << "Rows in LoopIteration: " << db->loopIterationTable().size();
    LOG(TRACE) << "Rows in Reference:     " << db->referenceTable().size();
    LOG(TRACE) << "Rows in Segment:       " << db->segmentTable().size();
    LOG(TRACE) << "Rows in Thread:        " << db->threadTable().size();

    return std::unique_ptr<const Database>(std::move(db));
}
