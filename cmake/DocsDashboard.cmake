#############################################################################
# Name:        DocsDashboard.txt
# Purpose:     Build script for Readability Studio
# Author:      Blake Madden
# Created:     2023-07-31
# Copyright:   (c) 2026 Blake Madden
# License:     Eclipse Public License 2.0
#############################################################################

# Generate the Documentation & Knowledge dashboard

# Status inputs (per-doc JSONs)
set(DOC_STATUS_DIR ${CMAKE_SOURCE_DIR}/project-management/docs/status)

# Output (Markdown)
set(DOC_OUT_MD ${CMAKE_SOURCE_DIR}/project-management/docs/DASHBOARD.md)

# Project name for header (you can override this before including)
if(NOT DEFINED DOCS_PROJECT_NAME)
    set(DOCS_PROJECT_NAME ${readstudio_DESCRIPTION})
endif()

# Inputs that trigger regeneration
set(DOC_INPUTS
        ${CMAKE_SOURCE_DIR}/cmake/modules/BuildDocsDashboard.cmake
        ${DOC_STATUS_DIR}/user-manual.json
        ${DOC_STATUS_DIR}/admin-guide.json
        ${DOC_STATUS_DIR}/programming-reference.json
        ${DOC_STATUS_DIR}/doxygen.json)

add_custom_command(
        OUTPUT ${DOC_OUT_MD}
        COMMAND ${CMAKE_COMMAND}
        -DROOT=${CMAKE_SOURCE_DIR}
        -DDOC_STATUS_DIR=${DOC_STATUS_DIR}
        -DDOC_OUT_MD=${DOC_OUT_MD}
        -DPROJECT_NAME=${DOCS_PROJECT_NAME}
        -P ${CMAKE_SOURCE_DIR}/cmake/modules/BuildDocsDashboard.cmake
        DEPENDS ${DOC_INPUTS}
        COMMENT "Generating Documentation & Knowledge dashboard"
        VERBATIM)

add_custom_target(docs-dashboard ALL
    DEPENDS ${DOC_OUT_MD})
