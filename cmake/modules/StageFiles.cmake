#############################################################################
# Name:        StageFiles.txt
# Purpose:     Build script for Readability Studio
# Author:      Blake Madden
# Created:     2025-10-26
# Copyright:   (c) 2025 Blake Madden
# License:     Eclipse Public License 2.0
#############################################################################

# ==========================================================================================
#  Function: copy_translations
#  ------------------------------------------------------------------------------------------
#  Copies compiled translation (.mo) files for a given language into all staging locations:
#   • The macOS staging directory
#   • The AppImage staging directory (for Linux packaging)
#   • The Windows staging directory
#
#  It also copies wxWidgets stock translations (wxstd.mo) if available.
#
#  Each copy operation prints an echo line at build time for visibility.
#  If a translation file is missing, a configure-time warning is issued.
#
#  Usage:
#      copy_translations(<LANG>)
#
#  Example:
#      copy_translations(es)
#      copy_translations(fr)
#
#  Arguments:
#      LANG
#          The language code (e.g., "es", "fr", "de") corresponding to the
#          compiled gettext file <LANG>.mo located under:
#              ${CMAKE_CURRENT_SOURCE_DIR}/locale/<LANG>.mo
#
#  Requirements:
#      • Must be called after the target (${PROJECT_NAME}) is defined.
#      • Variables used:
#            PROJECT_NAME           – name of the CMake target
#            RESOURCE_FOLDER        – relative path to the runtime resource directory
#            APPIMAGE_STAGING_DIR   – path to the AppImage binary directory (usually usr/bin)
#            APPIMAGE_LOCALE_DIR    – path to the AppImage locale directory (usually usr/share/locale)
#            WINDOWS_STAGING_DIR    – path to the Windows packaging root
# ==========================================================================================
# Requires: CMake's FindGettext module (built-in)
# Will set GETTEXT_MSGFMT_EXECUTABLE if found.
find_package(Gettext QUIET)

function(copy_translations LANG)
    if(NOT GETTEXT_MSGFMT_EXECUTABLE)
        set(_programfiles "$ENV{ProgramFiles}")
        set(_poedit_msgfmt "${_programfiles}/Poedit/GettextTools/bin/msgfmt.exe")

        if(EXISTS "${_poedit_msgfmt}")
            set(GETTEXT_MSGFMT_EXECUTABLE "${_poedit_msgfmt}" CACHE FILEPATH "Path to msgfmt from Poedit" FORCE)
            set(GETTEXT_FOUND TRUE CACHE BOOL "Gettext found through Poedit" FORCE)
            message(STATUS "Found msgfmt.exe via Poedit: ${_poedit_msgfmt}")
        else()
            message(STATUS "gettext(msgfmt) not found. PO files will not be compiled unless precompiled MO files exist.")
        endif()
    endif()
    set(_MSGFMT "${GETTEXT_MSGFMT_EXECUTABLE}")

    # ---- Project translation (readstudio.mo) ----
    set(_SRC_MO "${CMAKE_CURRENT_SOURCE_DIR}/locale/${LANG}.mo")
    set(_SRC_PO "${CMAKE_CURRENT_SOURCE_DIR}/locale/${LANG}.po")
    set(_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/locale")
    set(_BIN_MO  "${_BIN_DIR}/${LANG}.mo")  # if we need to compile from .po

    # Determine the source .mo we'll copy (build from .po if needed/possible)
    set(_USE_MO "")
    set(_WILL_BUILD_MO FALSE)

    if(EXISTS "${_SRC_MO}")
        # Use prebuilt .mo from source tree
        set(_USE_MO "${_SRC_MO}")
    elseif(EXISTS "${_SRC_PO}" AND _MSGFMT)
        # Build .mo from .po into the build tree
        set(_USE_MO "${_BIN_MO}")
        set(_WILL_BUILD_MO TRUE)
    else()
        # Neither .mo present nor can we build from .po
        if(NOT EXISTS "${_SRC_PO}")
            message(STATUS
                "No source translation (.po) or precompiled (.mo) found for '${LANG}'. Skipping.")
        elseif(NOT _MSGFMT)
            message(STATUS
                "gettext(msgfmt) not found; cannot compile '${_SRC_PO}'.")
        endif()
    endif()

    if(_USE_MO)
        # Optional compile step if building from .po
        if(_WILL_BUILD_MO)
            add_custom_command(TARGET ${PROJECT_NAME}
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E echo "Compiling ${LANG}.po → ${_BIN_MO}"
                COMMAND ${CMAKE_COMMAND} -E make_directory "${_BIN_DIR}"
                COMMAND "${_MSGFMT}" -o "${_BIN_MO}" "${_SRC_PO}")
        endif()

        # AppImage destination (standard Linux path is usr/share/locale)
        if(DEFINED APPIMAGE_LOCALE_DIR)
            set(_APPIMAGE_MO_DEST "${APPIMAGE_LOCALE_DIR}/${LANG}/LC_MESSAGES/readstudio.mo")
            set(_APPIMAGE_MO_DIR  "${APPIMAGE_LOCALE_DIR}/${LANG}/LC_MESSAGES")
        else()
            set(_APPIMAGE_MO_DEST "${APPIMAGE_STAGING_DIR}/${LANG}/readstudio.mo")
            set(_APPIMAGE_MO_DIR  "${APPIMAGE_STAGING_DIR}/${LANG}")
        endif()

        # Always copy .mo file to destinations
        add_custom_command(TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Copying ${LANG} translations to build and installer folders."
            COMMAND ${CMAKE_COMMAND} -E copy "${_USE_MO}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/${LANG}/readstudio.mo"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${_APPIMAGE_MO_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy "${_USE_MO}" "${_APPIMAGE_MO_DEST}"
            COMMAND ${CMAKE_COMMAND} -E copy "${_USE_MO}" "${WINDOWS_STAGING_DIR}/resources/${LANG}/readstudio.mo")
    endif()

    # ---- wxWidgets stock translation (wxstd.mo) ----
    set(_WX_SRC_MO "${CMAKE_CURRENT_SOURCE_DIR}/locale/${LANG}/wxstd.mo")
    set(_WX_SRC_PO "${wxWidgets_BASEDIR}/locale/${LANG}.po")
    set(_WX_BIN_DIR "${CMAKE_CURRENT_BINARY_DIR}/locale/${LANG}")
    set(_WX_BIN_MO  "${_WX_BIN_DIR}/wxstd.mo")

    set(_WX_USE_MO "")
    set(_WX_WILL_BUILD_MO FALSE)

    if(EXISTS "${_WX_SRC_MO}")
        set(_WX_USE_MO "${_WX_SRC_MO}")
    elseif(EXISTS "${_WX_SRC_PO}" AND _MSGFMT)
        set(_WX_USE_MO "${_WX_BIN_MO}")
        set(_WX_WILL_BUILD_MO TRUE)
    else()
        if(NOT EXISTS "${_WX_SRC_PO}")
            message(STATUS
                "wxWidgets '${LANG}' translations not found. "
                "Expected '${_WX_SRC_MO}' or '${_WX_SRC_PO}'.")
        elseif(NOT _MSGFMT)
            message(STATUS
                "gettext(msgfmt) not found; cannot compile '${_WX_SRC_PO}'.")
        endif()
    endif()

    if(_WX_USE_MO)
        # Optional compile step if building from .po
        if(_WX_WILL_BUILD_MO)
            add_custom_command(TARGET ${PROJECT_NAME}
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E echo "Compiling wxstd ${LANG}.po → ${_WX_BIN_MO}"
                COMMAND ${CMAKE_COMMAND} -E make_directory "${_WX_BIN_DIR}"
                COMMAND "${_MSGFMT}" -o "${_WX_BIN_MO}" "${_WX_SRC_PO}")
        endif()

        # AppImage destination (standard Linux path is usr/share/locale)
        if(DEFINED APPIMAGE_LOCALE_DIR)
            set(_APPIMAGE_WXMO_DEST "${APPIMAGE_LOCALE_DIR}/${LANG}/LC_MESSAGES/wxstd.mo")
            set(_APPIMAGE_WXMO_DIR  "${APPIMAGE_LOCALE_DIR}/${LANG}/LC_MESSAGES")
        else()
            set(_APPIMAGE_WXMO_DEST "${APPIMAGE_STAGING_DIR}/${LANG}/wxstd.mo")
            set(_APPIMAGE_WXMO_DIR  "${APPIMAGE_STAGING_DIR}/${LANG}")
        endif()

        # Always copy wxstd.mo if available
        add_custom_command(TARGET ${PROJECT_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Copying wxWidgets ${LANG} translations to build and installer folders."
            COMMAND ${CMAKE_COMMAND} -E copy "${_WX_USE_MO}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/${LANG}/wxstd.mo"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${_APPIMAGE_WXMO_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy "${_WX_USE_MO}" "${_APPIMAGE_WXMO_DEST}"
            COMMAND ${CMAKE_COMMAND} -E copy "${_WX_USE_MO}" "${WINDOWS_STAGING_DIR}/resources/${LANG}/wxstd.mo")
    endif()
endfunction()

# ==========================================================================================
#  Function: copy_dir_to_staging
#  ------------------------------------------------------------------------------------------
#  Configures post-build commands to copy a directory (and its contents) into all packaging
#  and staging locations for the current project:
#
#      • macOS staging directory
#      • AppImage staging directory (for Linux packaging)
#      • Windows staging directory
#
#  For each destination, this function:
#      1. Echoes the copy action to the build log for visibility.
#      2. Ensures the target destination directory exists.
#      3. Recursively copies all files from the source directory.
#
#  Usage:
#      copy_dir_to_staging(<SRC> <DEST_REL>)
#
#  Example:
#      copy_dir_to_staging("docs/readability-studio-manual/docs-manual" "readability-studio-manual")
#      copy_dir_to_staging("resources/report-themes" "report-themes")
#      copy_dir_to_staging("resources/scripting" "")
#
#  Arguments:
#      SRC
#          Path (relative to ${CMAKE_CURRENT_SOURCE_DIR}) to the source directory to copy.
#
#      DEST_REL
#          Relative subpath inside each destination root where the folder should be copied.
#          Use an empty string ("") to copy directly into the resource root.
#
#  Behavior:
#      • If the source directory does not exist, a configure-time warning is printed and
#        the copy step is skipped.
#      • The build step echoes each resolved destination path (macOS, AppImage, Windows).
#      • Directories are created automatically before copying.
#
#  Requirements:
#      • Must be called after the target (${PROJECT_NAME}) is defined.
#      • The following variables must be defined before use:
#            PROJECT_NAME           – name of the build target
#            RESOURCE_FOLDER        – relative path to resources in the macOS AppBundle
#            APPIMAGE_STAGING_DIR   – path to AppImage staging directory
#            WINDOWS_STAGING_DIR    – path to Windows staging directory
# ==========================================================================================
function(copy_dir_to_staging SRC DEST_REL)
    set(_SRC "${CMAKE_CURRENT_SOURCE_DIR}/${SRC}")

    if(NOT EXISTS "${_SRC}")
        message(FATAL_ERROR "Source folder '${_SRC}' not found and will not be copied to staging areas.")
        return()
    endif()

    # Build the three destination paths
    set(_MACOS_DEST "$<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/$<IF:$<STREQUAL:${DEST_REL},>,,${DEST_REL}>")
    set(_APPIMAGE_DEST "${APPIMAGE_STAGING_DIR}/$<IF:$<STREQUAL:${DEST_REL},>,,${DEST_REL}>")
    set(_WINDOWS_DEST "${WINDOWS_STAGING_DIR}/resources/$<IF:$<STREQUAL:${DEST_REL},>,,${DEST_REL}>")

    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD

        # Echo each destination
        COMMAND ${CMAKE_COMMAND} -E echo "Copying '${SRC}' → ${_MACOS_DEST}"
        COMMAND ${CMAKE_COMMAND} -E echo "Copying '${SRC}' → ${_APPIMAGE_DEST}"
        COMMAND ${CMAKE_COMMAND} -E echo "Copying '${SRC}' → ${_WINDOWS_DEST}"

        # Ensure dirs exist, then copy
        COMMAND ${CMAKE_COMMAND} -E make_directory "${_MACOS_DEST}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${_SRC}" "${_MACOS_DEST}"

        COMMAND ${CMAKE_COMMAND} -E make_directory "${_APPIMAGE_DEST}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${_SRC}" "${_APPIMAGE_DEST}"

        COMMAND ${CMAKE_COMMAND} -E make_directory "${_WINDOWS_DEST}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${_SRC}" "${_WINDOWS_DEST}"
    )
endfunction()

# ==========================================================================================
#  Function: copy_citation_file
#  ------------------------------------------------------------------------------------------
#  Configures post-build steps to copy a citation file into all target staging locations:
#    • The macOS staging directory
#    • The AppImage staging directory (for Linux packaging)
#    • The Windows staging directory
#
#  Each invocation sets up a custom post-build command for ${PROJECT_NAME} that:
#    1. Prints an echo message during the build for visibility.
#    2. Copies the citation file into all relevant staging locations.
#
#  Usage:
#      copy_citation_file(<FILENAME>)
#
#  Example:
#      copy_citation_file("citations.bib")
#      copy_citation_file("citations.mla")
#      copy_citation_file("citations.apa")
#
#  Arguments:
#      FILENAME
#          The relative path to the citation file (usually located in the
#          project's source directory). The file will be copied as-is
#          to the build, AppImage, and Windows resource directories.
#
#  Behavior:
#      • Prints a configure-time message announcing setup of the copy rule.
#      • Prints a build-time message when the copy actually runs.
#      • Fails if ${PROJECT_NAME} is not yet defined (must be called after
#        add_executable() or add_library()).
#
#  Requirements:
#      Variables expected to be defined before calling:
#          PROJECT_NAME           – the current target name
#          RESOURCE_FOLDER        – path to macOS AppBundle resource directory
#          APPIMAGE_STAGING_DIR   – path to AppImage packaging directory
#          WINDOWS_STAGING_DIR    – path to Windows packaging directory
# ==========================================================================================
function(copy_citation_file FILENAME)
    get_filename_component(BASENAME "${FILENAME}" NAME)

    message(STATUS "Configuring post-build copy for citation file: ${BASENAME}")

    add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        # Echo to the build log
        COMMAND ${CMAKE_COMMAND} -E echo "Copying ${BASENAME} to output and staging folders."
        # Actual copy operations
        COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/${BASENAME}"
        COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}"
            "${APPIMAGE_STAGING_DIR}/${BASENAME}"
        COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/${FILENAME}"
            "${WINDOWS_STAGING_DIR}/resources/${BASENAME}"
    )
endfunction()
