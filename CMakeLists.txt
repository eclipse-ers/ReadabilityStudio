#############################################################################
# Name:        CMakeLists.txt
# Purpose:     Build script for Readability Studio
# Author:      Blake Madden
# Created:     2023-07-31
# Copyright:   (c) 2026 Blake Madden
# License:     Eclipse Public License 2.0
#############################################################################

# Build targets:
# all: builds the program, doxygen help, and dashboard targets
# readstudio: builds the main application (included with ALL)
# doxygen-docs: builds the API documentation (included with ALL)
# manuals: builds the manuals and HTML help
# g11n-dashboard: builds the globalization dashboard, showing the current translation statuses (included with ALL)
# docs-dashboard: builds the documentation dashboard, showing the current documentation statuses
#     (e.g., coverage, translation completion, etc.) (included with ALL)
# tests-dashboard: builds the tests and quality dashboard (included with ALL) 

# Options:
# ENABLE_SANITIZERS to use ASAN & UBSAN (ignored in release build)
# ALLOW_DIRTY_SUBMODULES to allow building even if submodules have local changes (OFF in debug build, ON in release)

# Requirements
cmake_minimum_required(VERSION 3.26)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(MIN_GCC_REQUIRED "12.2.1")
set(MIN_APPLECLANG_REQUIRED "15.3")
set(MIN_MSVC_TOOLSET_REQUIRED "19.30") # VS 2022 (1930 is the build number, which is what _MSC_VER will be set to)
set(MIN_MSVC_REQUIRED "2022") # Just used in the help
Set(MIN_WXWIDGETS_REQUIRED "3.3.3")

# https://blog.kitware.com/cmake-and-the-default-build-type/
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(default_build_type "Debug")
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

option(ALLOW_DIRTY_SUBMODULES "Allow building even if submodules have local changes" OFF)
# Always allow dirty submodules for debug, but use user's selection for release.
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(ALLOW_DIRTY_SUBMODULES ON CACHE BOOL "Allow building even if submodules have local changes" FORCE)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(ALLOW_DIRTY_SUBMODULES OFF CACHE BOOL "Allow building even if submodules have local changes")
endif()

message(STATUS "Build: ${CMAKE_BUILD_TYPE}")
message(STATUS "Generator: ${CMAKE_GENERATOR}")

if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "macOS Architecture")
    # Enable hardened runtime:
	# https://developer.apple.com/documentation/security/hardened_runtime
    # Note that this will need to be set via codesign later because we are not
    # codesigning via CMake. This here is more for bookkeeping.
    set(CMAKE_XCODE_ATTRIBUTE_ENABLE_HARDENED_RUNTIME YES)
endif()

# Always run these, even if not APPLE, as we use ${CMAKE_OSX_DEPLOYMENT_TARGET} for the docs.
#
# If no deployment target has been set default to the minimum supported
# OS version (this has to be set before the first project() call)
if(APPLE AND CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(CMAKE_OSX_DEPLOYMENT_TARGET 12.0 CACHE STRING "iOS Deployment Target")
else()
    set(CMAKE_OSX_DEPLOYMENT_TARGET 13.3 CACHE STRING "macOS Deployment Target")
endif()
message(STATUS "macOS deployment target: ${CMAKE_OSX_DEPLOYMENT_TARGET}")

# Only used for the docs
set(WIN32_DEPLOYMENT_TARGET 10)
set(UNIX_ARCHITECTURE "x86_64")
set(MIN_RAM "2 GB")
set(RAM_RECOMMENDED "4 GB")

# Edit these values when bumping the release version
project(readstudio VERSION 2026.0.0.1 DESCRIPTION "Readability Studio" HOMEPAGE_URL https://eclipse.dev/ers)
enable_language(CXX)
if(APPLE)
    enable_language(OBJCXX)
endif()
set(PROGRAM_LONGNAME "Eclipse ${readstudio_DESCRIPTION}")
set(PROGRAM_NUMBERED_VERSION 9.0)
set(PUBLISHER "Eclipse Foundation AISBL")
set(PROGRAM_AUTHOR "Blake Madden")
set(PROGRAM_AUTHOR_MLA "Madden, Blake")
set(PROGRAM_AUTHOR_APA "Madden, B.")
set(PROGRAM_COMMENTS "Readability analysis software")
set(PROGRAM_RDNS_IDENTIFIER "org.eclipse.readabilitystudio")
set(GIT_REPO "https://github.com/eclipse-ers/ReadabilityStudio.git")
string(REGEX REPLACE "[.]git$" "" ISSUE_PAGE "${GIT_REPO}")
string(APPEND ISSUE_PAGE "/issues")
set(REMOTE_VERSION_FILE "${readstudio_HOMEPAGE_URL}")
string(APPEND REMOTE_VERSION_FILE "/download/current-version.txt")
set(DOWNLOAD_PAGE "${readstudio_HOMEPAGE_URL}")
string(APPEND DOWNLOAD_PAGE "/download")
set(INNOSETUP_GUID "{FBFBEAB6-622A-4E16-893D-216C25FF6A69}")
set(PROGRAM_LICENSE "Eclipse Public License 2.0")

include(cmake/modules/FindGit.cmake)
include(cmake/modules/GetGitInfo.cmake)
if(Git_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} --no-pager log -n 1 HEAD --pretty=format:%H
    OUTPUT_VARIABLE BUILD_VERSION
    RESULT_VARIABLE result
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(NOT result EQUAL 0)
    message(WARNING "git not found. Unable to generate build version.")
  else()
    string(TOUPPER ${BUILD_VERSION} BUILD_VERSION)
  endif()
endif()

set(FULL_VERSION ${readstudio_VERSION_MAJOR}.${readstudio_VERSION_MINOR}.${readstudio_VERSION_PATCH}.${readstudio_VERSION_TWEAK})
# Build a displayable version string that only includes the necessary parts
set(FULL_DISPLAY_VERSION ${readstudio_VERSION_MAJOR})
if(NOT ${readstudio_VERSION_MINOR} EQUAL 0 OR NOT ${readstudio_VERSION_PATCH} EQUAL 0 OR NOT ${readstudio_VERSION_TWEAK} EQUAL 0)
    string(APPEND FULL_DISPLAY_VERSION ".${readstudio_VERSION_MINOR}")
endif()
if(NOT ${readstudio_VERSION_PATCH} EQUAL 0 OR NOT ${readstudio_VERSION_TWEAK} EQUAL 0)
    string(APPEND FULL_DISPLAY_VERSION ".${readstudio_VERSION_PATCH}")
endif()
if(NOT ${readstudio_VERSION_TWEAK} EQUAL 0)
    string(APPEND FULL_DISPLAY_VERSION ".${readstudio_VERSION_TWEAK}")
endif()
string(TIMESTAMP BUILD_YEAR "%Y")
string(TIMESTAMP BUILD_DATE "%Y-%m-%d")

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${MIN_GCC_REQUIRED})
  message(FATAL_ERROR "Insufficient GCC version. ${MIN_GCC_REQUIRED} is required, but only ${CMAKE_CXX_COMPILER_VERSION} found.")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${MIN_MSVC_TOOLSET_REQUIRED})
  message(FATAL_ERROR "Insufficient Visual Studio version. ${MIN_MSVC_TOOLSET_REQUIRED} toolset is required, but only ${CMAKE_CXX_COMPILER_VERSION} found.")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "APPLECLANG" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS ${MIN_APPLECLANG_REQUIRED})
  message(FATAL_ERROR "Insufficient AppleClang version. ${MIN_APPLECLANG_REQUIRED} is required, but only ${CMAKE_CXX_COMPILER_VERSION} found.")
endif()
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(cmake/modules/CheckASAN.cmake)
    include(cmake/modules/EnableSanitizers.cmake)
endif()

# Read implementation of Snowball used by stemming library (this appears in the help)
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/src/OleanderStemmingLibrary/src/stemming.h" STEM_CONTENT)
string(REGEX MATCH "SNOWBALL_MAJOR_VERSION = ([0-9]+)" _ ${STEM_CONTENT})
set(SNOWBALL_MAJOR_VERSION ${CMAKE_MATCH_1})
string(REGEX MATCH "SNOWBALL_MINOR_VERSION = ([0-9]+)" _ ${STEM_CONTENT})
set(SNOWBALL_MINOR_VERSION ${CMAKE_MATCH_1})
string(REGEX MATCH "SNOWBALL_PATCH_VERSION = ([0-9]+)" _ ${STEM_CONTENT})
set(SNOWBALL_PATCH_VERSION ${CMAKE_MATCH_1})
message(STATUS "Snowball standard in use: ${SNOWBALL_MAJOR_VERSION}.${SNOWBALL_MINOR_VERSION}.${SNOWBALL_PATCH_VERSION}")

message(STATUS "${readstudio_DESCRIPTION}")
message(STATUS "${PROGRAM_COMMENTS}")
message(STATUS "Full version: ${readstudio_VERSION_MAJOR}.${readstudio_VERSION_MINOR}.${readstudio_VERSION_PATCH}.${readstudio_VERSION_TWEAK}")
message(STATUS "Display version: ${FULL_DISPLAY_VERSION}")
message(STATUS "Numbered Version: ${PROGRAM_NUMBERED_VERSION}")
message(STATUS "Build: ${BUILD_VERSION}")
message(STATUS "Built in: ${BUILD_YEAR}")
message(STATUS "Authored by: ${PROGRAM_AUTHOR}")
message(STATUS "Published by: ${PUBLISHER}")
message(STATUS "Homepage: ${readstudio_HOMEPAGE_URL}")
message(STATUS "Output binary: ${CMAKE_PROJECT_NAME}")
message(STATUS "Writing program info to build peripheral files")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/src/app/version.h")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/citations.bib.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/citations.bib")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/citations.mla.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/citations.mla")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/citations.apa.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/citations.apa")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/resource.h.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/resource.h")
if(UNIX AND NOT APPLE)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/installers/unix/AppDir/org.eclipse.readabilitystudio.desktop.in"
                   "${CMAKE_CURRENT_SOURCE_DIR}/installers/unix/AppDir/org.eclipse.readabilitystudio.desktop")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/installers/unix/AppDir/usr/share/metainfo/readstudio.metainfo.xml.in"
                   "${CMAKE_CURRENT_SOURCE_DIR}/installers/unix/AppDir/usr/share/metainfo/readstudio.metainfo.xml")
elseif(WIN32)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/installers/windows/setup.isl.in"
                   "${CMAKE_CURRENT_SOURCE_DIR}/installers/windows/setup.isl")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/installers/windows/icons.isl.in"
                   "${CMAKE_CURRENT_SOURCE_DIR}/installers/windows/icons.isl")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/installers/windows/readstudio.iss.in"
                   "${CMAKE_CURRENT_SOURCE_DIR}/installers/windows/readstudio.iss")
endif()
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/docs/_variables.yml.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/docs/_variables.yml")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/docs/sysadmin/build-commands.txt.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/docs/sysadmin/build-commands.txt")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/docs/sysadmin/windows-silent.txt.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/docs/sysadmin/windows-silent.txt")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/docs/readability-studio-manual/_quarto.yml.in"
               "${CMAKE_CURRENT_SOURCE_DIR}/docs/readability-studio-manual/_quarto.yml")

if(NOT CMAKE_CONFIGURATION_TYPES)
    get_property(HAVE_MULTI_CONFIG_GENERATOR GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
    # Set default configuration types for multi-config generators
    if(HAVE_MULTI_CONFIG_GENERATOR)
        SET(CMAKE_CONFIGURATION_TYPES "Debug;Release")
    endif()
endif()

if(TARGET manuals)
    message(STATUS "Help build has been requested.")
endif()
if(BUILD_TESTING)
    message(STATUS "Unit test run has been requested.")
endif()

if(APPLE)
    message(STATUS "Supported architectures: ${CMAKE_OSX_ARCHITECTURES}")
else()
    message(STATUS "Architecture: ${CMAKE_SYSTEM_PROCESSOR}" )
endif()

# Build the project management dashboards
include(cmake/modules/CheckTranslations.cmake)
check_all_translations()
include(cmake/G11NDashboard.cmake)
include(cmake/DocsDashboard.cmake)
include(cmake/TestsDashboard.cmake)

# Build source and resource file lists to include
include(cmake/ListFiles.cmake)

# Setup wxWidgets (can be 1-5 folders up from here)
##############################################################
include(src/Wisteria-Dataviz/cmake/folders.cmake)

set(PROJECT_WX_DEBUG_LEVEL 1)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(PROJECT_WX_DEBUG_LEVEL 0)
else()
    set(PROJECT_WX_DEBUG_LEVEL 1)
endif()
message(STATUS "wxWidgets debug level: ${PROJECT_WX_DEBUG_LEVEL}")

# Add wxWidgets to the build
set(wxUSE_WEBVIEW_EDGE ON)
set(wxUSE_WEBVIEW_EDGE_STATIC ON)
set(wxBUILD_SHARED OFF)
set(wxBUILD_DEBUG_LEVEL ${PROJECT_WX_DEBUG_LEVEL})
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(wxBUILD_OPTIMISE ON)
    set(wxBUILD_STRIPPED_RELEASE ON)
endif()
find_directory("wxWidgets" wxWidgets_BASEDIR)
if(NOT EXISTS "${wxWidgets_BASEDIR}")
    message(FATAL_ERROR "wxWidgets base directory not found: ${wxWidgets_BASEDIR}")
endif()
message(STATUS "wxWidgets found in: ${wxWidgets_BASEDIR}")
add_subdirectory(${wxWidgets_BASEDIR} ${CMAKE_CURRENT_SOURCE_DIR}/wxWidgets_lib EXCLUDE_FROM_ALL)
set(wxWidgets_LIBRARIES wx::core wx::base wx::xrc wx::propgrid wx::xml
                        wx::aui wx::ribbon wx::qa wx::stc wx::net wx::richtext wx::webview)
# Apply compiler warning suppressions to wxWidgets targets
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU|AppleClang")
    foreach(tgt IN LISTS wxWidgets_LIBRARIES)
        if(TARGET ${tgt})
            get_target_property(is_imported ${tgt} IMPORTED)
            get_target_property(is_alias ${tgt} ALIASED_TARGET)
            if(NOT is_imported AND NOT is_alias)
                target_compile_options(${tgt} PRIVATE
                        -Wno-alloc-size-larger-than
                        -Wno-format
                        -Wno-deprecated-declarations)
            endif()
        endif()
    endforeach()
endif()
message(STATUS "Adding wxWidgets libraries: ${wxWidgets_LIBRARIES}")

if(wxVERSION VERSION_LESS ${MIN_WXWIDGETS_REQUIRED})
  message(FATAL_ERROR "Insufficient wxWidgets version. ${MIN_WXWIDGETS_REQUIRED} is required, but only ${wxVERSION} found.")
endif()

if(UNIX AND NOT APPLE)
    # GTK 3 dependency
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

    # WebKitGTK dependency (required for wxWebView)
    pkg_check_modules(WEBKIT2 REQUIRED webkit2gtk-4.1)

    # Fontconfig dependency (required by wxPDFDocument's font manager on GTK)
    pkg_check_modules(FONTCONFIG REQUIRED fontconfig)

    # Setup CMake to use GTK+ and WebKitGTK, tell the compiler where to look for headers
    # and to the linker where to look for libraries
    include_directories(${GTK3_INCLUDE_DIRS} ${WEBKIT2_INCLUDE_DIRS} ${FONTCONFIG_INCLUDE_DIRS})
    link_directories(${GTK3_LIBRARY_DIRS} ${WEBKIT2_LIBRARY_DIRS} ${FONTCONFIG_LIBRARY_DIRS})
endif()

# List of submodules (relative paths)
set(SUBMODULES
    src/Catch2
    src/lua
    src/OleanderStemmingLibrary
    src/tinyexpr-plusplus
    src/tinyxml2
    src/Wisteria-Dataviz
    src/Wisteria-Dataviz/src/CRCpp
    src/Wisteria-Dataviz/src/easyexif
    src/Wisteria-Dataviz/src/utfcpp
    src/Wisteria-Dataviz/src/wxpdfdoc
    src/Wisteria-Dataviz/src/wxSimpleJSON
    src/Wisteria-Dataviz/src/wxStartPage
    ${wxWidgets_BASEDIR}
    ${wxWidgets_BASEDIR}/src/expat
    ${wxWidgets_BASEDIR}/src/jpeg
    ${wxWidgets_BASEDIR}/src/png
    ${wxWidgets_BASEDIR}/src/tiff
    ${wxWidgets_BASEDIR}/src/zlib
    ${wxWidgets_BASEDIR}/src/stc/lexilla
    ${wxWidgets_BASEDIR}/src/stc/scintilla
    ${wxWidgets_BASEDIR}/3rdparty/libwebp
    ${wxWidgets_BASEDIR}/3rdparty/nanosvg
    ${wxWidgets_BASEDIR}/3rdparty/pcre)

set(SUBMODULE_HASHES "")

foreach(PATH IN LISTS SUBMODULES)
    get_filename_component(NAME "${PATH}" NAME)
    string(REGEX REPLACE "[^A-Za-z0-9_]" "_" SAFE_NAME "${NAME}")
    string(TOUPPER "${SAFE_NAME}" VAR_NAME)

    get_git_submodule_hash("${PATH}" HASH)
    get_git_submodule_origin("${PATH}" ORIGIN)

    # Attempt to get version tag/description
    set(DESCRIPTION "")
    get_git_submodule_description("${PATH}" DESCRIPTION)

    # Always define HASH and ORIGIN
    string(APPEND SUBMODULE_HASHES
        "#define ${VAR_NAME}_HASH \"${HASH}\"\n"
        "#define ${VAR_NAME}_ORIGIN \"${ORIGIN}\"\n")

    # Only define VERSION_TAG if DESCRIPTION is not empty
    if(DESCRIPTION)
        string(APPEND SUBMODULE_HASHES
            "#define ${VAR_NAME}_VERSION_TAG \"${DESCRIPTION}\"\n")
    endif()

    # Separate entries
    string(APPEND SUBMODULE_HASHES "\n")
endforeach()

string(REGEX REPLACE "[ \t\r\n]+$" "" SUBMODULE_HASHES "${SUBMODULE_HASHES}")

configure_file(
    "${CMAKE_SOURCE_DIR}/src/sbom.h.in"
    "${CMAKE_SOURCE_DIR}/src/app/sbom.h"
    @ONLY)

# Run ALL unit-test suites for this program and its submodules
##############################################################
if(BUILD_TESTING)
    set(BUILD_TEMP ${CMAKE_CURRENT_SOURCE_DIR}/build-temp)
    if(EXISTS ${BUILD_TEMP})
        file(REMOVE_RECURSE ${BUILD_TEMP})
    endif()
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test-results)
        file(REMOVE_RECURSE ${CMAKE_CURRENT_SOURCE_DIR}/test-results)
    endif()
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/test-results)

    # submodule test suites
    function(run_test_suite TEST_PROJECT TEST_FOLDER TEST_RUNNER)
        execute_process(COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                                -S ${CMAKE_CURRENT_SOURCE_DIR}/src/${TEST_PROJECT}/${TEST_FOLDER} -B ${BUILD_TEMP})
        execute_process(COMMAND ${CMAKE_COMMAND} --build ${BUILD_TEMP} --config ${CMAKE_BUILD_TYPE})
        set(TEST_OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/${TEST_PROJECT}/${TEST_FOLDER}/bin)
        # MSVC tends to make a "Debug" or "Release" in your "bin" folder and put the binaries
        # in there instead of just the "bin" folder like you tell it to.
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/${TEST_PROJECT}/${TEST_FOLDER}/bin/${CMAKE_BUILD_TYPE})
            set(TEST_OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/${TEST_PROJECT}/${TEST_FOLDER}/bin)
        endif()
        message(STATUS "Running ${TEST_OUT_DIR}/${TEST_RUNNER}...")
        execute_process(COMMAND ${TEST_OUT_DIR}/${TEST_RUNNER}
                                --reporter=junit --out=${CMAKE_CURRENT_SOURCE_DIR}/test-results/${TEST_RUNNER}-results.xml
                        WORKING_DIRECTORY "${TEST_OUT_DIR}")
        file(REMOVE_RECURSE ${BUILD_TEMP})
    endfunction()

    # test suites for RS specifically
    function(run_rs_test_suite TEST_FOLDER TEST_RUNNER)
        execute_process(COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                                -S ${CMAKE_CURRENT_SOURCE_DIR}/${TEST_FOLDER} -B ${BUILD_TEMP})
        execute_process(COMMAND ${CMAKE_COMMAND} --build ${BUILD_TEMP} --config ${CMAKE_BUILD_TYPE})
        set(TEST_OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/${TEST_PROJECT}/${TEST_FOLDER}/bin)
        if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/${TEST_PROJECT}/${TEST_FOLDER}/bin/${CMAKE_BUILD_TYPE})
            set(TEST_OUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/${TEST_PROJECT}/${TEST_FOLDER}/bin)
        endif()
        execute_process(COMMAND ${TEST_OUT_DIR}/${TEST_RUNNER}
                                --reporter=junit --out=${CMAKE_CURRENT_SOURCE_DIR}/test-results/${TEST_RUNNER}-results.xml
                        WORKING_DIRECTORY "${TEST_OUT_DIR}")
        file(REMOVE_RECURSE ${BUILD_TEMP})
    endfunction()

    run_test_suite(OleanderStemmingLibrary tests OleanderStemTestRunner)
    run_test_suite(tinyexpr-plusplus tests TETestRunner)
    run_test_suite(Wisteria-Dataviz tests WisteriaTestRunner)
    run_test_suite(Wisteria-Dataviz tests/gui-tests WisteriaGuiTestRunner)
    run_rs_test_suite(tests RSTestRunner)
    run_rs_test_suite(tests/gui-tests RSGuiTestRunner)
    message(STATUS "Unit test results available in ${CMAKE_CURRENT_SOURCE_DIR}/test-results")
endif()

# Help Files
##############################################################

include(cmake/modules/FindR.cmake)
if(NOT R_FOUND)
    message(WARNING "R not found. Help manuals cannot be built.")
endif()

add_custom_target(manuals
                COMMAND "${R_SCRIPT}"
                    "${CMAKE_CURRENT_SOURCE_DIR}/docs/build-help-projects.R"
                WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/docs/")

# Build the API documentation
##############################################################
find_package(Doxygen)
if(NOT DOXYGEN_FOUND)
    # Just warning about API docs not being built.
    # We will try to include it with ALL, but not necessary for end users.
    message(WARNING "Doxygen not found. API documentation cannot be built.")
else()
    set(DOXYGEN_ALL_OPTION_MAYBE_UNAVAILABLE ALL)
endif()

add_custom_target(doxygen-docs
            ${DOXYGEN_ALL_OPTION_MAYBE_UNAVAILABLE}
            COMMAND ${CMAKE_COMMAND} -E echo "Generating API documentation with Doxygen."
            COMMAND ${DOXYGEN_EXECUTABLE} "${CMAKE_SOURCE_DIR}/docs/doxygen/Doxyfile"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/docs/doxygen"
            VERBATIM)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# Build binary
##############################################################
# (re-configure project to update this)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/includes/files.cmake)

# Add wxPDFDocument submodule (lives under the Wisteria-Dataviz submodule).
# Reuse Wisteria's canonical source list, but re-root the paths to the submodule
# location. wxpdfdoc is excluded from the source glob in ListFiles.cmake so that
# these files are added once, here, with their warning suppressions.
include(${CMAKE_CURRENT_SOURCE_DIR}/src/Wisteria-Dataviz/cmake/includes/wxpdf.cmake)
# Barcode support is unused and would drag in the bundled zint C library, so drop
# the two barcode sources (pdfbarcode.cpp is the only consumer of pdfbarcodezint.cpp,
# which is the only consumer of zint).
list(REMOVE_ITEM WXPDF_SRC
    src/wxpdfdoc/src/pdfbarcode.cpp
    src/wxpdfdoc/src/pdfbarcodezint.cpp)
list(TRANSFORM WXPDF_SRC PREPEND "src/Wisteria-Dataviz/")
list(APPEND APP_SRC_FILES ${WXPDF_SRC})
# Suppress warnings from wxPDFDocument
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # -Wno-maybe-uninitialized is GCC-only (Clang has no equivalent flag)
    set_source_files_properties(${WXPDF_SRC} PROPERTIES
        COMPILE_OPTIONS "-Wno-unused-but-set-variable;-Wno-unused-variable;-Wno-unused-parameter;-Wno-shadow;-Wno-ignored-qualifiers;-Wno-maybe-uninitialized")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang")
    set_source_files_properties(${WXPDF_SRC} PROPERTIES
        COMPILE_OPTIONS "-Wno-unused-but-set-variable;-Wno-unused-variable;-Wno-unused-parameter;-Wno-shadow;-Wno-ignored-qualifiers")
elseif(MSVC)
    # /wd4189 — local variable initialized but not referenced
    # /wd4100 — unreferenced formal parameter
    # /wd4101 — unreferenced local variable
    # /wd4267 — conversion from 'size_t' to 'uint32_t', possible loss of data
    set_source_files_properties(${WXPDF_SRC} PROPERTIES
        COMPILE_OPTIONS "/wd4189;/wd4100;/wd4101;/wd4267")
endif()

if(WIN32)
    # Include an RC file and HiDPI manifest for Windows (provided by wxWidgets)
    set(wxUSE_DPI_AWARE_MANIFEST 1)
    list(APPEND APP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/readstudio.rc)
    list(APPEND APP_SRC_FILES "${wxWidgets_BASEDIR}/include/wx/msw/wx_dpi_aware.manifest")
elseif(APPLE)
    # Add an icon for the apple .app file
    list(APPEND APP_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/app-logo.icns)
endif()
add_executable(${PROJECT_NAME} WIN32 MACOSX_BUNDLE ${APP_SRC_FILES})

# wxPDFDocument include paths (zint/barcode support is disabled, see above)
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Wisteria-Dataviz/src/wxpdfdoc/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Wisteria-Dataviz/src/wxpdfdoc/thirdparty/woff2/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Wisteria-Dataviz/src/wxpdfdoc/thirdparty/woff2/brotli/include)

target_compile_definitions(${PROJECT_NAME} PUBLIC
    $<$<CONFIG:Debug>:
        ENABLED_SANITIZERS_STR="${ENABLED_SANITIZERS_STR}"
        SANITIZER_ENV_HINTS_STR="${SANITIZER_ENV_HINTS_STR}">)

# Platform-specific settings
# (definitions, warnings, and optimizations)
##############################################################

# Define wxDEBUG_LEVEL consistently for all platforms
target_compile_definitions(${PROJECT_NAME} PUBLIC 
    wxDEBUG_LEVEL=${PROJECT_WX_DEBUG_LEVEL})

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        __WXMSW__ CRCPP_USE_CPP11 _CRT_SECURE_NO_WARNINGS _UNICODE
        $<$<CONFIG:Debug>:__WXDEBUG__ ENABLE_PROFILING ${SANITIZER_DEFS}>
        $<$<CONFIG:Release>:NDEBUG>)
    target_compile_options(${PROJECT_NAME} PUBLIC
        /Zc:__cplusplus /MP /W3 /WX /wd6211 /openmp /permissive- /sdl
        $<$<CONFIG:Debug>:/Od ${SANITIZER_FLAGS}>
        $<$<CONFIG:Release>:/O2>)

elseif(MINGW OR MSYS)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        __WXMSW__ CRCPP_USE_CPP11 _CRT_SECURE_NO_WARNINGS _UNICODE
        $<$<CONFIG:Debug>:__WXDEBUG__ ENABLE_PROFILING>
        $<$<CONFIG:Release>:NDEBUG>)
    target_compile_options(${PROJECT_NAME} PUBLIC
        -Wall -Wextra -Wpedantic -Wshadow -Werror
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-fstack-protector-strong>
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-D_FORTIFY_SOURCE=2>
        $<$<CONFIG:Debug>:-Og -g3 ${SANITIZER_FLAGS} ${SANITIZER_EXTRA_FLAGS}>
        $<$<CONFIG:Release>:-O2>)
    target_link_options(${PROJECT_NAME} PUBLIC
        $<$<CONFIG:Debug>:${SANITIZER_LINK_FLAGS}>)

elseif(APPLE)
    target_compile_definitions(${PROJECT_NAME} PUBLIC
        __WXOSX__ CRCPP_USE_CPP11
        $<$<CONFIG:Debug>:__WXDEBUG__ ENABLE_PROFILING>
        $<$<CONFIG:Release>:NDEBUG>)
    target_compile_options(${PROJECT_NAME} PUBLIC
        -Wall -Wextra -Wpedantic -Wshadow -Werror
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-fstack-protector-strong>
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-D_FORTIFY_SOURCE=2>
        $<$<CONFIG:Debug>:-Og -g3 ${SANITIZER_FLAGS} ${SANITIZER_EXTRA_FLAGS}>
        $<$<CONFIG:Release>:-O2>)
    target_link_options(${PROJECT_NAME} PUBLIC
        $<$<CONFIG:Debug>:${SANITIZER_LINK_FLAGS}>)

elseif(UNIX)
    # strip dead code
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

    target_compile_definitions(${PROJECT_NAME} PUBLIC
        __WXGTK__ CRCPP_USE_CPP11
        $<$<CONFIG:Debug>:__WXDEBUG__ ENABLE_PROFILING>
        $<$<CONFIG:Release>:NDEBUG>)
    target_compile_options(${PROJECT_NAME} PUBLIC
        -Wall -Wextra -Wpedantic -Wshadow -Werror -fopenmp
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-fstack-protector-strong>
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-D_FORTIFY_SOURCE=2>
        $<$<CONFIG:Debug>:-Og -g3 ${SANITIZER_FLAGS} ${SANITIZER_EXTRA_FLAGS}>
        $<$<CONFIG:Release>:-O2>)
    target_link_options(${PROJECT_NAME} PUBLIC
        $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-Wl,-z,relro,-z,now>
        $<$<CONFIG:Debug>:${SANITIZER_LINK_FLAGS}>)
endif()

if(DEFINED wxBUILD_DEBUG_LEVEL AND NOT "${wxBUILD_DEBUG_LEVEL}" STREQUAL "${PROJECT_WX_DEBUG_LEVEL}")
    message(FATAL_ERROR "Mismatch: wxBUILD_DEBUG_LEVEL=${wxBUILD_DEBUG_LEVEL} but your code uses wxDEBUG_LEVEL=${PROJECT_WX_DEBUG_LEVEL}")
endif()

# Print per-target summary
if(ENABLE_SANITIZERS AND ENABLED_SANITIZERS_SUMMARY)
    message(STATUS "Target ${PROJECT_NAME} built with sanitizers (Debug): ${ENABLED_SANITIZERS_SUMMARY}")
endif()
include(cmake/modules/PrintCompilerFlags.cmake)
print_compile_flags_summary(${PROJECT_NAME})
target_compile_definitions(${PROJECT_NAME} PUBLIC
    COMPILE_FLAGS_SUMMARY_STR="${COMPILE_FLAGS_SUMMARY_STR}")

# Link required libraries to the executable
if(WIN32 OR APPLE)
    # AppleClang dropped support for OpenMP
    # Including TBB is not necessary for macOS and actually breaks hardened runtime
    # because that library is not included by default with the OS (XCode installs it)
    # and it is not signed.
    target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
elseif(UNIX)
    # Add OpenMP for multithreading.
    # TBB is required for GCC linkage sometimes, although this app is not explicitly using it,
    # so try to link it in.
    find_package(OpenMP)
    find_package(TBB)
    if(OpenMP_FOUND AND TBB_FOUND)
        message(STATUS "OpenMP: ${OpenMP_VERSION}")
        message(STATUS "Intel Threading Building Blocks: ${TBB_VERSION}")
        target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} TBB::tbb OpenMP::OpenMP_CXX)
    elseif(OpenMP_FOUND)
        message(STATUS "OpenMP: ${OpenMP_VERSION}")
        target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} OpenMP::OpenMP_CXX)
    elseif(TBB_FOUND)
        message(STATUS "Intel Threading Building Blocks: ${TBB_VERSION}")
        target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES} TBB::tbb)
    else()
        target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
    endif()
    # Fontconfig is required by wxPDFDocument's font manager on GTK
    target_link_libraries(${PROJECT_NAME} ${FONTCONFIG_LIBRARIES})
endif()

# Copy help, scripts, and resources into build folder
##############################################################
message(STATUS "Copying application resources...")

# make staging area directories if missing to avoid a build error
if(WIN32 AND NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/installers/windows/release/")
    add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E make_directory
                   "${CMAKE_CURRENT_SOURCE_DIR}/installers/windows/release/")
endif()
if(APPLE AND NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/installers/macos/release/")
    add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E make_directory
                   "${CMAKE_CURRENT_SOURCE_DIR}/installers/macos/release/")
endif()

# On macOS, move the resource files up from the binary folder to the resources folder
# (note that iOS has a flat folder structure, so only do this for macOS).
# Some of these files are compiled post build, and others are dynamic folders,
# so that is why we copy them like this post build instead of including them
# via RESOURCE like we do the icon.
if(APPLE AND NOT CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(RESOURCE_FOLDER "/../resources")
else()
    set(RESOURCE_FOLDER "")
endif()

set(APPIMAGE_STAGING_DIR "${CMAKE_CURRENT_SOURCE_DIR}/installers/unix/AppDir/usr/bin")
set(APPIMAGE_LOCALE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/installers/unix/AppDir/usr/share/locale")
set(WINDOWS_STAGING_DIR "${CMAKE_CURRENT_SOURCE_DIR}/installers/windows")

include(cmake/modules/StageFiles.cmake)

# Copy resource and doc folders to staging areas for build and installers
copy_dir_to_staging("docs/readability-studio-manual/docs-manual" "readability-studio-manual")
copy_dir_to_staging("docs/readability-studio-manual/docs-online" "readability-studio-manual/online")
copy_dir_to_staging("docs/shortcuts-cheatsheet/docs" "readability-studio-manual")
copy_dir_to_staging("docs/readability-test-reference/docs" "readability-studio-manual")
copy_dir_to_staging("docs/release-notes/docs" "readability-studio-manual")
copy_dir_to_staging("docs/sysadmin/docs" "readability-studio-manual")
copy_dir_to_staging("docs/readability-studio-api/docs" "readability-studio-manual")
copy_dir_to_staging("docs/lua-5.4/doc" "readability-studio-manual/lua-5.4/doc")
copy_dir_to_staging("resources/scripting" "")
copy_dir_to_staging("resources/report-themes" "report-themes")
copy_dir_to_staging("examples" "examples")

# Translations
copy_translations("es")

# Icons
if(UNIX AND NOT APPLE)
    # Icon used for Linux
    add_custom_command(TARGET ${PROJECT_NAME}
                       POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy
                           "${CMAKE_CURRENT_SOURCE_DIR}/resources/images/ribbon/app-logo.svg" "${CMAKE_CURRENT_SOURCE_DIR}/installers/unix/AppDir/app-logo.svg")
    # Application screenshot
    add_custom_command(TARGET ${PROJECT_NAME}
                       POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy
                           "${CMAKE_CURRENT_SOURCE_DIR}/docs/readability-studio-manual/images/frygraph.png" "${CMAKE_CURRENT_SOURCE_DIR}/installers/unix/AppDir/frygraph.png")
endif()

# License file
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                       "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE" "$<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/LICENSE"
                   COMMAND ${CMAKE_COMMAND} -E copy
                       "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE" "${APPIMAGE_STAGING_DIR}/LICENSE"
                   COMMAND ${CMAKE_COMMAND} -E copy
                       "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE" "${WINDOWS_STAGING_DIR}/resources/LICENSE")

# Citation files
copy_citation_file("citations.bib")
copy_citation_file("citations.mla")
copy_citation_file("citations.apa")

# Zip up images and menu resources
file(MAKE_DIRECTORY "${WINDOWS_STAGING_DIR}/resources/")
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E echo "Building resource bundle and copying to build and installer folders."
                   COMMAND ${CMAKE_COMMAND} -E tar "cvf" "$<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/res.wad" --format=zip --files-from="${CMAKE_CURRENT_SOURCE_DIR}/cmake/includes/images.cmake"
                   COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/res.wad" "${APPIMAGE_STAGING_DIR}/res.wad"
                   COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/res.wad" "${WINDOWS_STAGING_DIR}/resources/res.wad"
                   WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/resources/images")

# Word lists
add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E echo "Building word lists and copying to build and installer folders."
                   COMMAND ${CMAKE_COMMAND} -E tar "cvf" "$<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/words.wad" --format=zip --files-from="${CMAKE_CURRENT_SOURCE_DIR}/cmake/includes/words.cmake"
                   COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/words.wad" "${APPIMAGE_STAGING_DIR}/words.wad"
                   COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE_DIR:${PROJECT_NAME}>${RESOURCE_FOLDER}/words.wad" "${WINDOWS_STAGING_DIR}/resources/words.wad"
                   WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/resources/words")

if(UNIX)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        C_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN TRUE)
endif()

if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RESOURCE app-logo.icns
        OUTPUT_NAME ${PROJECT_NAME}
        XCODE_ATTRIBUTE_EXECUTABLE_NAME ${PROJECT_NAME} 
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in)

        message(STATUS "App Bundle: ${OUTPUT_NAME}")
endif()

# Copy application (and resources) to installer folders (if a production build)
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    if(WIN32)
        add_custom_command(TARGET ${PROJECT_NAME}
                           POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E echo "Copying binary to installer folder."
                           COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${PROJECT_NAME}>" "${WINDOWS_STAGING_DIR}/release/")
    elseif(UNIX AND NOT APPLE)
        add_custom_command(TARGET ${PROJECT_NAME}
                           POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E echo "Copying binary AppImage staging folder."
                           COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${PROJECT_NAME}>" "${APPIMAGE_STAGING_DIR}/${PROJECT_NAME}")
    elseif(APPLE)
        add_custom_command(TARGET ${PROJECT_NAME}
                           POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E echo "Copying files to DMG staging folder."
                           COMMAND ${CMAKE_COMMAND} -E copy_directory "$<TARGET_FILE_DIR:${PROJECT_NAME}>" "${CMAKE_CURRENT_SOURCE_DIR}/installers/macos/release/")
    endif()
endif()
