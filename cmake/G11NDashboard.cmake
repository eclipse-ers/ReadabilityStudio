#############################################################################
# Name:        G11NDashboard.txt
# Purpose:     Build script for Readability Studio
# Author:      Blake Madden
# Created:     2025-11-06
# Copyright:   (c) 2026 Blake Madden
# License:     Eclipse Public License 2.0
#############################################################################

# Generate the Globalization dashboard

# L10n status inputs
set(G11N_STATUS_DIR ${CMAKE_SOURCE_DIR}/project-management/g11n/status)

# I18n status inputs
set(G11N_STATUS_I18N_DIR ${CMAKE_SOURCE_DIR}/project-management/g11n/status/i18n)

# Outputs
set(G11N_OUT_MD     ${CMAKE_SOURCE_DIR}/project-management/g11n/DASHBOARD.md)

# Roll-up weights
set(G11N_WEIGHTS ui=10,docs=5,installer=1,art=1,license=1)

# Inputs that trigger regeneration (l10n + i18n + script)
set(G11N_INPUTS
  ${G11N_STATUS_DIR}/ui.json
  ${G11N_STATUS_DIR}/docs.json
  ${G11N_STATUS_DIR}/windows-installer.json
  ${G11N_STATUS_DIR}/art.json
  ${G11N_STATUS_DIR}/license.json
  ${G11N_STATUS_I18N_DIR}/ui.json
  ${G11N_STATUS_I18N_DIR}/docs.json
  ${G11N_STATUS_I18N_DIR}/windows-installer.json
  ${G11N_STATUS_I18N_DIR}/art.json
  ${G11N_STATUS_I18N_DIR}/license.json
  ${CMAKE_SOURCE_DIR}/cmake/modules/BuildG11NDashboard.cmake)

# Pretty project name
set(G11N_PROJECT_NAME ${readstudio_DESCRIPTION})

add_custom_command(
  OUTPUT ${G11N_OUT_MD}
  COMMAND ${CMAKE_COMMAND}
          -DROOT=${CMAKE_SOURCE_DIR}
          -DSTATUS_DIR=${G11N_STATUS_DIR}
          -DSTATUS_DIR_I18N=${G11N_STATUS_I18N_DIR}
          -DOUT_MD=${G11N_OUT_MD}
          -DWEIGHTS=${G11N_WEIGHTS}
          -DPROJECT_NAME=${G11N_PROJECT_NAME}
          -P ${CMAKE_SOURCE_DIR}/cmake/modules/BuildG11NDashboard.cmake
  DEPENDS ${G11N_INPUTS}
  COMMENT "Generating Globalization dashboard"
  VERBATIM)

add_custom_target(g11n-dashboard ALL
  DEPENDS ${G11N_OUT_MD})
