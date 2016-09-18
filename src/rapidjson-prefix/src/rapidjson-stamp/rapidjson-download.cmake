

set(command "/usr/bin/cmake;-P;/home/nath/Desktop/pcvInterpreter/src/rapidjson-prefix/tmp/rapidjson-gitclone.cmake")
execute_process(
  COMMAND ${command}
  RESULT_VARIABLE result
  OUTPUT_FILE "/home/nath/Desktop/pcvInterpreter/src/rapidjson-prefix/src/rapidjson-stamp/rapidjson-download-out.log"
  ERROR_FILE "/home/nath/Desktop/pcvInterpreter/src/rapidjson-prefix/src/rapidjson-stamp/rapidjson-download-err.log"
  )
if(result)
  set(msg "Command failed: ${result}\n")
  foreach(arg IN LISTS command)
    set(msg "${msg} '${arg}'")
  endforeach()
  set(msg "${msg}\nSee also\n  /home/nath/Desktop/pcvInterpreter/src/rapidjson-prefix/src/rapidjson-stamp/rapidjson-download-*.log\n")
  message(FATAL_ERROR "${msg}")
else()
  set(msg "rapidjson download command succeeded.  See also /home/nath/Desktop/pcvInterpreter/src/rapidjson-prefix/src/rapidjson-stamp/rapidjson-download-*.log\n")
  message(STATUS "${msg}")
endif()
