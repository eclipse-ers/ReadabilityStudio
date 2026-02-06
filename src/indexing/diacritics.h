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

#ifndef DIACRITICS_H
#define DIACRITICS_H

#include <map>
#include <string>
#include <string_view>

namespace grammar
    {
    /// @brief Converts typographic ligatures to the regular ASCII characters, as well as combining
    ///        two-byte combining diacritical sequences into a single character.
    /// @details Supports all European alphabets.
    class convert_ligatures_and_diacritics
        {
      public:
        /// @brief Converts the ligatures and diacritic in a stream and copies the results
        ///     to an internal buffer.
        ///     @param input The input stream to convert (if necessary).
        /// @returns @c true if the input text had ligatures or diacritics to convert.\n
        ///     Call get_conversion() to get the converted buffer if this returns @c true.
        [[nodiscard(
            "Indicates whether the input contained characters that needed to be converted.")]]
        bool operator()(std::wstring_view input);

        /// @returns The converted buffer from the previous string processing.
        /// @note This will be empty if there was nothing to convert.
        [[nodiscard("Returns the converted content from the last call to ().")]]
        const std::wstring& get_conversion() const noexcept
            {
            return m_convertedBuffer;
            }

        /// @returns The (writable) converted buffer from the previous string processing.
        /// @details This is useful for moving this buffer into another string.
        /// @note This will be empty if there was nothing to convert.
        [[nodiscard("Returns the converted content from the last call to ().")]]
        std::wstring& get_conversion()
            {
            return m_convertedBuffer;
            }

      private:
        static std::map<wchar_t, std::wstring_view> m_ligatures;
        static std::map<std::pair<wchar_t, wchar_t>, wchar_t> m_combined_diacritics;
        std::wstring m_convertedBuffer;
        };
    } // namespace grammar

#endif // DIACRITICS_H
