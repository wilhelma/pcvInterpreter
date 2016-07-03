/**
 *
 *    @file  ThreadTable.cpp
 *   @brief  
 *
 *    @date  06/26/16
 *  @author  Paolo Di Giglio (github.com/pdigiglio),
 *           <p.digiglio91@gmail.com>
 *
 */

#include "ThreadTable.h"

const std::pair<ThreadTable::iterator, bool> ThreadTable::fill(sqlite3_stmt *sqlstmt) {
	TRD_ID id               = static_cast<TRD_ID>(sqlite3_column_int(sqlstmt, 0));
	TIME_STRING start_time(reinterpret_cast<const char*>(sqlite3_column_text(sqlstmt, 1)));
	TIME_STRING end_time(reinterpret_cast<const char*>(sqlite3_column_text(sqlstmt, 2)));
	NUM_CYCLES num_cycles   = static_cast<NUM_CYCLES>(sqlite3_column_int(sqlstmt, 3));
	INS_ID create_instr_id  = static_cast<INS_ID>(sqlite3_column_int(sqlstmt, 4));
	INS_ID join_instr_id    = static_cast<INS_ID>(sqlite3_column_int(sqlstmt, 5));
	TRD_ID parent_thread_id = static_cast<TRD_ID>(sqlite3_column_int(sqlstmt, 6));
	PID process_id          = static_cast<PID>(sqlite3_column_int(sqlstmt, 7));

	thread_t *tmp = new thread_t(id,
			start_time,
			end_time,
			num_cycles,
			create_instr_id,
			join_instr_id,
			parent_thread_id,
			process_id);

	return insert(id, *tmp);
}
