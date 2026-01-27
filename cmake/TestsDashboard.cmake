#############################################################################
# Name:        TestsDashboard.txt
# Purpose:     Build script for Readability Studio
# Author:      Blake Madden
# Created:     2025-11-06
# Copyright:   (c) 2026 Blake Madden
# License:     Eclipse Public License 2.0
#############################################################################

if(NOT DEFINED TESTS_PROJECT_NAME)
    set(TESTS_PROJECT_NAME ${readstudio_DESCRIPTION})
endif()

set(TEST_BADGES_MD  "${CMAKE_SOURCE_DIR}/project-management/tests/status/badges.md")
set(TESTS_OUT_MD    "${CMAKE_SOURCE_DIR}/project-management/tests/DASHBOARD.md")

# You can override this before including if you want a different label
if(NOT DEFINED TESTS_PROJECT_NAME)
  set(TESTS_PROJECT_NAME "${CMAKE_PROJECT_NAME}")
endif()

set(TESTS_INPUTS
  ${CMAKE_SOURCE_DIR}/cmake/modules/BuildTestsDashboard.cmake
  ${TEST_BADGES_MD})

add_custom_command(
  OUTPUT ${TESTS_OUT_MD}
  COMMAND ${CMAKE_COMMAND}
          -DROOT=${CMAKE_SOURCE_DIR}
          -DTEST_BADGES_MD=${TEST_BADGES_MD}
          -DTESTS_OUT_MD=${TESTS_OUT_MD}
          -DPROJECT_NAME=${TESTS_PROJECT_NAME}
          -P ${CMAKE_SOURCE_DIR}/cmake/modules/BuildTestsDashboard.cmake
  DEPENDS ${TESTS_INPUTS}
  COMMENT "Generating Testing & Quality dashboard"
  VERBATIM)

add_custom_target(tests-dashboard ALL
  DEPENDS ${TESTS_OUT_MD})

message(STATUS "[TESTS] rules loaded → ${TESTS_OUT_MD}")
