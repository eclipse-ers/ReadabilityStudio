/********************************************************************************
 * Copyright (c) 2005-2026 Blake Madden
 *
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * https://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 *   Blake Madden - initial implementation
 ********************************************************************************/

/*== == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == =*\
||                                                                                              ||
||                    +----------------------------------------------+                          ||
||                    | /   /   /   /   /   /      @@@@@@@@@@@@@@@@@@|                          ||
||                    |                       /      @@@@@@@@@@@@@@@@|                          ||
||                    |                  /        /    @@@@@@@@@@@@@@|                          ||
||                    |              /        /          @@@@@@@@@@@@|                          ||
||                    |          /         /        /      @@@@@@@@@@|                          ||
||                    |      /          /         /    /     @@@@@@@@|                          ||
||                    |              /          /     /     /  @@@@@@|                          ||
||                    |           /           /      /      /    @@@@|                          ||
||                    |@       /            /       /       /      @@|                          ||
||                    |@@@                /        /        /       @|                          ||
||                    |@@@@@@           /         /         /        |                          ||
||                    |@@@@@@@@@                 /          /        |                          ||
||                    |@@@@@@@@@@@              /           /        |                          ||
||                    |@@@@@@@@@@@@@@          /            /        |                          ||
||                    |@@@@@@@@@@@@@@@@@                    /        |                          ||
||                    |@@@@@@@@@@@@@@@@@@@@                 /        |                          ||
||                    |@@@@@@@@@@@@@@@@@@@@@@@                       |                          ||
||                    +----------------------------------------------+                          ||
||                                                                                              ||
||    RRRRR   EEEEE   AAAAA   DDDD    AAAAA   BBBBB   IIIII   L       IIIII   TTTTT    Y   Y    ||
||    R   R   E       A   A   D   D   A   A   B   B     I     L         I       T      Y   Y    ||
||    RRRR    EEEE    AAAAA   D   D   AAAAA   BBBB      I     L         I       T       Y Y     ||
||    R  R    E       A   A   D   D   A   A   B   B     I     L         I       T        Y      ||
||    R   R   EEEEE   A   A   DDDD    A   A   BBBBB   IIIII   LLLLL   IIIII     T        Y      ||
||                                                                                              ||
||                                            SSSS    TTTTT   U   U   DDDD    IIIII   OOOO      ||
||                                            S         T     U   U   D   D     I    O    O     ||
||                                            SSSS      T     U   U   D   D     I    O    O     ||
||                                               S      T     U   U   D   D     I    O    O     ||
||                                            SSSS      T     UUUUU   DDDD    IIIII   OOOO      ||
||                                                                                              ||
\*== == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == =*/

#ifndef FILEPATH_RESOLVER
#define FILEPATH_RESOLVER

#include "../Wisteria-Dataviz/src/util/fileutil.h"
#include <wx/string.h>
#include <wx/wx.h>

/** @brief Class to determine which sort of filepath a string may resemble.
    @details This is useful for determining if a string is a file to a file or URL,
        and determining specifically which sort of path it is.*/
class FilePathResolver final : public FilePathResolverBase
    {
  public:
    /// @brief Default constructor.
    FilePathResolver() = default;

    /** @brief Resolves a string to see if it is a file path.
        @param path The string to resolve to a file path.
        @param attemptToConnect Whether to attempt to connect the file (via the Internet)
            to see if it's an URL. This is final fallback when the filepath type
            can't be easily determined.
        @param pathsToSearch A list of local paths to look in if @c path is a relative local path.
        @note Set @c attemptToConnect to @c false if performance is a concern.*/
    explicit FilePathResolver(
        const wxString& path, const bool attemptToConnect,
        std::initializer_list<wxString> pathsToSearch = std::initializer_list<wxString>{})
        {
        ResolvePath(path, attemptToConnect, pathsToSearch);
        }

    /** Resolves a string to see if it is a file path.
        @param path The string to resolve to a file path.
        @param attemptToConnect Whether to attempt to connect the file (via the Internet)
            to see if it's an URL. This is final fallback when the filepath type
            can't be easily determined.
        @param pathsToSearch A list of local paths to look in if @c path is a relative local path.
        @note Set @c attemptToConnect to @c false if performance is a concern.*/
    wxString
    ResolvePath(const wxString& path, const bool attemptToConnect,
                std::initializer_list<wxString> pathsToSearch = std::initializer_list<wxString>{});
    };

#endif // FILEPATH_RESOLVER
