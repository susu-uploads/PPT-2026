if(NOT DEFINED CASE_FILE OR NOT EXISTS "${CASE_FILE}")
  message(FATAL_ERROR "Test configuration is missing: ${CASE_FILE}")
endif()
include("${CASE_FILE}")

execute_process(COMMAND ${PROGRAM_COMMAND}
  INPUT_FILE "${CASE_DIRECTORY}/stdin.txt"
  OUTPUT_FILE "${CASE_DIRECTORY}/stdout.txt"
  ERROR_FILE "${CASE_DIRECTORY}/stderr.txt"
  WORKING_DIRECTORY "${CASE_DIRECTORY}"
  TIMEOUT 30 RESULT_VARIABLE result)
file(WRITE "${CASE_DIRECTORY}/exit-code.txt" "${result}\n")
file(READ "${CASE_DIRECTORY}/stdout.txt" output)
file(READ "${CASE_DIRECTORY}/stderr.txt" errors)

if(NOT "${result}" MATCHES "^[0-9]+$" OR NOT "${result}" STREQUAL "${EXPECTED_EXIT}")
  message(FATAL_ERROR
    "Command: ${PROGRAM_COMMAND}\nExpected exit: ${EXPECTED_EXIT}; actual: ${result}\nstdout:\n${output}\nstderr:\n${errors}")
endif()

set(check_input "${CASE_DIRECTORY}/stdout.txt")
if(NOT EXPECTED_EXIT EQUAL 0)
  if(NOT "${output}" STREQUAL "")
    message(FATAL_ERROR "Unexpected output for rejected input:\n${output}")
  endif()
  set(check_input "${CASE_DIRECTORY}/stderr.txt")
endif()
execute_process(COMMAND ${CHECK_COMMAND} INPUT_FILE "${check_input}"
  OUTPUT_FILE "${CASE_DIRECTORY}/check-stdout.txt"
  ERROR_FILE "${CASE_DIRECTORY}/check-stderr.txt"
  WORKING_DIRECTORY "${CASE_DIRECTORY}"
  TIMEOUT 5 RESULT_VARIABLE checked)
if(NOT "${checked}" STREQUAL "0")
  file(READ "${CASE_DIRECTORY}/check-stderr.txt" explanation)
  message(FATAL_ERROR
    "Check: ${CHECK_COMMAND}\nCheck result: ${checked}\n${explanation}\nstdout:\n${output}\nstderr:\n${errors}")
endif()
