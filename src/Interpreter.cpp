#include "Interpreter.h"

#include <boost/log/core.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

namespace logging = boost::log;
namespace expr = boost::log::expressions;

Interpreter::Interpreter(LockMgr* lockMgr, ThreadMgr* threadMgr, const char* logFile)
	: lockMgr_(lockMgr), threadMgr_(threadMgr), logFile_(logFile) {
	initLogger();
}

void Interpreter::initLogger() const {

	logging::add_file_log(
		logging::keywords::file_name = logFile_,
		logging::keywords::format = (
		expr::stream
			<< expr::attr< unsigned int >("LineID")
			<< ": <" << logging::trivial::severity
			<< "> " << expr::smessage
		)	
	);

	logging::core::get()->set_filter
	(
		logging::trivial::severity >= logging::trivial::trace
	);

	logging::add_common_attributes();
}
