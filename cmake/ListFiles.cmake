cmake_minimum_required(VERSION 3.25)

# edit this regex to change which files are being filtered out
set(SRC_FILES_TO_REMOVE_FILTER "(wxSimpleJSON|demo.cpp|main.cpp|Wisteria-Dataviz/tests|wxpdfdoc|\
Wisteria-Dataviz/src/app/|Wisteria-Dataviz/src/ui/dialogs/editors/|\
Wisteria-Dataviz/src/ui/dialogs/projectsettingsdlg.cpp|\
Catch2|OleanderStemmingLibrary/tests|tinyexpr-plusplus/tests|utfcpp/tests|\
utfcpp/samples|utfcpp/extern|cxxopts|xmltest.cpp|html5-printer.cpp|\
textclassifier.cpp|candlestickplot.cpp|ganttchart.cpp|lrroadmap.cpp|proconroadmap.cpp|\
roadmap.cpp|sankeydiagram.cpp|table.cpp|wcurveplot.cpp|variableselectdlg.cpp|reportbuilder.cpp|reporttableloader.cpp|datasetimportdlg.cpp|\
win_loss_sparkline.cpp|multi_series_lineplot.cpp|likertchart.cpp|waffle_chart.cpp|\
pivot.cpp|subset.cpp|join.cpp|clone.cpp|xrc_menu_strings.cpp)")

# these files get compiled into larger ones that are included with the distribution
set(WORD_FILES_TO_REMOVE_FILTER "(common-dictionary|base-english-dictionary.txt|base-non-personal.txt|base-personal.txt|base-english.txt|\
base-german.txt|base-spanish.txt|common-errors.txt|sql.txt|visual-basic.txt|programming/r.txt|python.txt|java.txt|\
html.txt|csharp.txt|cpp.txt|assembly.txt)")

get_filename_component(FILE_SRC_PATH "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

message(STATUS "Generating file lists for build system.")

# Don't touch the file unless it is out of date or missing.
# CMake generators may break if build files get updated but their content didn't actually change.
function(update_file_if_needed FILE_MANIFEST_PATH FILE_CONTENT)
    if(NOT EXISTS "${FILE_MANIFEST_PATH}")
        message(STATUS "${FILE_MANIFEST_PATH}: creating list.")
        file(WRITE "${FILE_MANIFEST_PATH}" "${FILE_CONTENT}")
    else()
        file(READ "${FILE_MANIFEST_PATH}" ORIGINAL_FILE_CONTENT)
        string(COMPARE NOTEQUAL "${FILE_CONTENT}" "${ORIGINAL_FILE_CONTENT}" FILES_DIFFERENT)
        if (FILES_DIFFERENT)
            message(STATUS "${FILE_MANIFEST_PATH}: updating list.")
            file(WRITE "${FILE_MANIFEST_PATH}" "${FILE_CONTENT}")
        endif()
    endif()
endfunction()

# source files to include in the build
block()
    file(GLOB_RECURSE LISTED_SRC_FILES LIST_DIRECTORIES false RELATIVE ${FILE_SRC_PATH} "${FILE_SRC_PATH}/src/*.cpp")
    file(GLOB_RECURSE C_SRC_FILES LIST_DIRECTORIES false RELATIVE ${FILE_SRC_PATH} "${FILE_SRC_PATH}/src/onelua_no_warnings[.]c")
    list(APPEND LISTED_SRC_FILES ${C_SRC_FILES})
if(APPLE)
    file(GLOB_RECURSE OBJ_C_SRC_FILES LIST_DIRECTORIES false RELATIVE ${FILE_SRC_PATH} "${FILE_SRC_PATH}/src/*.mm")
    list(APPEND LISTED_SRC_FILES ${OBJ_C_SRC_FILES})
endif()
    list(SORT LISTED_SRC_FILES CASE INSENSITIVE)
    # filter out the files that we don't want, such as tests, unused submodules, etc.
    list(FILTER LISTED_SRC_FILES EXCLUDE REGEX ${SRC_FILES_TO_REMOVE_FILTER})

    set(FILE_CONTENT "# Automatically generated from 'list-files.cmake'\
\n\# DO NOT MODIFY MANUALLY!\n\nSET(APP_SRC_FILES")
    foreach(CURR_FILE IN LISTS LISTED_SRC_FILES)
        string(APPEND FILE_CONTENT "\n    ${CURR_FILE}")
    endforeach()
    string(STRIP "${FILE_CONTENT}" FILE_CONTENT)
    string(APPEND FILE_CONTENT ")")

    set(FILE_MANIFEST_PATH "${FILE_SRC_PATH}/cmake/includes/files.cmake")
    update_file_if_needed("${FILE_MANIFEST_PATH}" "${FILE_CONTENT}")
endblock()

# flat list of images to include in the resource zip file
block()
    file(GLOB_RECURSE LISTED_IMG_FILES LIST_DIRECTORIES false RELATIVE "${FILE_SRC_PATH}/resources/images" "${FILE_SRC_PATH}/resources/images/*.svg")
    file(GLOB_RECURSE LISTED_OTHER_SRC_FILES LIST_DIRECTORIES false RELATIVE "${FILE_SRC_PATH}/resources/images" "${FILE_SRC_PATH}/resources/images/*.png")
    list(APPEND LISTED_IMG_FILES ${LISTED_OTHER_SRC_FILES})
    file(GLOB_RECURSE LISTED_OTHER_SRC_FILES LIST_DIRECTORIES false RELATIVE "${FILE_SRC_PATH}/resources/images" "${FILE_SRC_PATH}/resources/images/*.jpg")
    list(APPEND LISTED_IMG_FILES ${LISTED_OTHER_SRC_FILES})
    file(GLOB_RECURSE LISTED_OTHER_SRC_FILES LIST_DIRECTORIES false RELATIVE "${FILE_SRC_PATH}/resources/images" "${FILE_SRC_PATH}/resources/images/*.xrc")
    list(APPEND LISTED_IMG_FILES ${LISTED_OTHER_SRC_FILES})
    list(SORT LISTED_IMG_FILES CASE INSENSITIVE)

    set(FILE_CONTENT "")
    foreach(CURR_FILE IN LISTS LISTED_IMG_FILES)
        string(APPEND FILE_CONTENT "${CURR_FILE}\n")
    endforeach()
    string(STRIP "${FILE_CONTENT}" FILE_CONTENT)

    set(FILE_MANIFEST_PATH "${FILE_SRC_PATH}/cmake/includes/images.cmake")
    update_file_if_needed("${FILE_MANIFEST_PATH}" "${FILE_CONTENT}")
endblock()

# flat list of word files to include in the resource zip file
block()
    file(GLOB_RECURSE LISTED_WORD_FILES LIST_DIRECTORIES false RELATIVE "${FILE_SRC_PATH}/resources/words" "${FILE_SRC_PATH}/resources/words/*.txt")
    list(SORT LISTED_WORD_FILES CASE INSENSITIVE)
    # filter out the files that we don't want, such as the base files that are merged into lager ones
    # (the larger ones are what gets included in the final product)
    list(FILTER LISTED_WORD_FILES EXCLUDE REGEX ${WORD_FILES_TO_REMOVE_FILTER})

    set(FILE_CONTENT "")
    foreach(CURR_FILE IN LISTS LISTED_WORD_FILES)
        string(APPEND FILE_CONTENT "${CURR_FILE}\n")
    endforeach()
    string(STRIP "${FILE_CONTENT}" FILE_CONTENT)

    set(FILE_MANIFEST_PATH "${FILE_SRC_PATH}/cmake/includes/words.cmake")
    update_file_if_needed("${FILE_MANIFEST_PATH}" "${FILE_CONTENT}")
endblock()

# Extract translatable strings from XRC files for gettext
block()
    set(TRANSLATABLE_TAGS label help title tooltip longhelp)

    set(OUTPUT_CONTENT "// Auto-generated from XRC files for gettext extraction\n")
    string(APPEND OUTPUT_CONTENT "// DO NOT EDIT - regenerate by running CMake\n\n")
    string(APPEND OUTPUT_CONTENT "#include <wx/intl.h>\n\n")
    string(APPEND OUTPUT_CONTENT "void _xrc_menu_strings()\n")
    string(APPEND OUTPUT_CONTENT "    {\n")

    file(GLOB XRC_FILES "${FILE_SRC_PATH}/resources/images/menus/*.xrc")
    foreach(XRC_FILE ${XRC_FILES})
        file(READ "${XRC_FILE}" XRC_CONTENT)
        # escape semicolons to prevent CMake from treating them as list separators
        string(REPLACE ";" "<<SEMICOLON>>" XRC_CONTENT "${XRC_CONTENT}")
        get_filename_component(XRC_FILENAME "${XRC_FILE}" NAME)
        string(APPEND OUTPUT_CONTENT "    // ${XRC_FILENAME}\n")

        foreach(TAG ${TRANSLATABLE_TAGS})
            string(REGEX MATCHALL "<${TAG}>[^<]+</${TAG}>" MATCHES "${XRC_CONTENT}")
            foreach(MATCH ${MATCHES})
                string(REGEX REPLACE "<${TAG}>([^<]+)</${TAG}>" "\\1" TEXT "${MATCH}")
                # restore semicolons
                string(REPLACE "<<SEMICOLON>>" ";" TEXT "${TEXT}")
                string(STRIP "${TEXT}" TEXT)
                if(TEXT)
                    string(REPLACE "\\" "\\\\" TEXT "${TEXT}")
                    string(REPLACE "\"" "\\\"" TEXT "${TEXT}")
                    string(REPLACE "\n" "\\n" TEXT "${TEXT}")
                    string(APPEND OUTPUT_CONTENT "    _(\"${TEXT}\");\n")
                endif()
            endforeach()
        endforeach()
    endforeach()

    string(APPEND OUTPUT_CONTENT "    }\n")

    set(FILE_MANIFEST_PATH "${FILE_SRC_PATH}/src/xrc_menu_strings.cpp")
    update_file_if_needed("${FILE_MANIFEST_PATH}" "${OUTPUT_CONTENT}")
endblock()
