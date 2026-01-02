/********************************************************************************
 * Copyright (c) 2005-2025 Blake Madden
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

#ifndef CHARACTER_SIMPLIFY_H
#define CHARACTER_SIMPLIFY_H

#include "character_traits.h"
#include "sentence.h"
#include <map>

namespace text_transform
    {
    /// @brief Lookup table for romanization.
    class romanization_conversion_table
        {
      public:
        romanization_conversion_table();

        [[nodiscard]]
        const std::map<wchar_t, std::wstring>& get_table() const noexcept
            {
            return m_replacements;
            }

      private:
        std::map<wchar_t, std::wstring> m_replacements;
        };

    /** @brief Class to encode a string into Romanized text.
        @details This includes replacing "fancy" punctuation and accented characters,
            such as changing smart quotes to straight quotes, German eszetts to 'ss,' etc.
        @note Legacy Windows-1252 character mappings are also supported.*/
    class romanize
        {
      public:
        romanize() noexcept
            : isEndOfSentence(
                  true /* used for ellipses, this says that sentences must be uppercased. */)
            {
            }

        /** @brief Normalizes text by Romanizing characters and simplifying typography.
            @param text The input text to normalize.
            @param normalizeTypography Whether to replace accented letters, ligatures,
                smart punctuation, symbols, and other non-ASCII typography with simpler
                Latin or ASCII equivalents.
            @param removeEllipses Whether ellipses should be collapsed. If an ellipsis
                represents the end of a sentence, it is replaced with a period;
                otherwise it is replaced with a single space.
            @param removeBullets Whether to normalize bulleted or numbered list markers.
                Leading bullets are removed and replaced with a tab character to
                preserve list structure.
            @param narrowFullWidthCharacters Whether full-width Unicode characters
                should be converted to their narrow (half-width) equivalents.
            @returns A normalized string with simplified typography suitable for
                plain-text processing.*/
        [[nodiscard]]
        std::wstring operator()(std::wstring_view text, const bool normalizeTypography,
                                const bool removeEllipses, const bool removeBullets,
                                const bool narrowFullWidthCharacters) const;

      private:
        static const romanization_conversion_table& conversion_table()
            {
            static const romanization_conversion_table table;
            return table;
            }

        grammar::is_end_of_sentence isEndOfSentence;
        grammar::is_bulleted_text isBullet;
        };
    } // namespace text_transform

#endif // CHARACTER_SIMPLIFY_H
