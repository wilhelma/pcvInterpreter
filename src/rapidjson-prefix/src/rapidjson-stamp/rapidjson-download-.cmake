

set(command "/usr/local/Cellar/cmake/3.7.0/bin/cmake;-P;/Users/nath/pcvInterpreter/src/rapidjson-prefix/tmp/rapidjson-gitclone.cmake")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/Users/nath/pcvInterpreter/src/rapidjson-prefix/src/rapidjson-stamp/rapidjson-download-out.log"
  ERROR_FILE "/Users/nath/pcvInterpreter/src/rapidjson-prefix/src/rapidjson-stamp/rapidjson-download-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /Users/nath/pcvInterpreter/src/rapidjson-prefix/src/rapidjson-stamp/rapidjson-download-*.log")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "rapidjson download command succeeded.  See also /Users/nath/pcvInterpreter/src/rapidjson-prefix/src/rapidjson-stamp/rapidjson-download-*.log")
  message(STATUS "${msg}")
endif()
