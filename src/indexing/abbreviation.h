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

#ifndef ABBREVIATION_H
#define ABBREVIATION_H

#include "characters.h"
#include "word_list.h"

/// @brief Namespace for grammar analysis.
namespace grammar
    {
    // @brief Predicate for determining if a word is an abbreviation (case-insensitive).
    class is_abbreviation
        {
      public:
        /** @returns @c true if text block is an abbreviation
                (based on predefined list of abbreviations, or if text block is four
                consecutive consonants in front of a period).
            @param text The text block to analyze.
            @note The length of the text block to analyze should be
                the start of the text block up to the period.*/
        [[nodiscard]]
        bool operator()(const std::wstring_view text) const;

        /** @returns The list of abbreviations.
                This is where you can specify what is considered an abbreviation.
            @warning The abbreviations you add must include the trailing period.*/
        [[nodiscard]]
        static word_list& get_abbreviations() noexcept
            {
            return m_abbreviations;
            }

        /** @returns The list of words they may be seen as abbreviations but are not.
                This is where you can override the classifier.
            @warning The non-abbreviations you add must include the trailing period.*/
        [[nodiscard]]
        static word_list& get_non_abbreviations() noexcept
            {
            return m_nonAbbreviations;
            }

      private:
        static word_list m_abbreviations;
        static word_list m_nonAbbreviations;
        };

    /// @brief Predicate for determining if a word is an acronym.
    class is_acronym
        {
      public:
        /** @returns @c true if more than half of the letters are uppercased.
            @param text The text stream to analyze.*/
        [[nodiscard]]
        bool operator()(const std::wstring_view text) const noexcept;

        /** @returns The number of periods counted from the last call to operator().*/
        [[nodiscard]]
        size_t get_dot_count() const noexcept
            {
            return m_dot_count;
            }

        /** @returns Whether the last operator() ended with an 's'.
            @note This only applies to words longer than 2 letters,
                so "Is" would actually return false here because it is
                not an acronym and is irrelevant.*/
        [[nodiscard]]
        bool ends_with_lower_s() const noexcept
            {
            return m_ends_with_lower_s;
            }

        /** @returns Whether a word follows the Letter/Number and Dot pattern (e.g., "K.A.O.S.").
            @param text The text stream to read.*/
        [[nodiscard]]
        static bool is_dotted_acronym(const std::wstring_view text) noexcept;

      private:
        mutable size_t m_dot_count{ 0 };
        mutable bool m_ends_with_lower_s{ false };
        };

    // @brief Predicate for determining if a word is a possessive.
    class is_possessive
        {
      public:
        /** @returns @c true if text block is possessive (i.e., ends with "'s").
            @param text The text block to analyze.*/
        [[nodiscard]]
        bool operator()(const std::wstring_view text) const
            {
            return (text.length() >= 2 &&
                    characters::is_character::is_either(text.back(), L's', L'S') &&
                    characters::is_character::is_apostrophe(text[text.length() - 2]));
            }
        };
    } // namespace grammar

#endif // ABBREVIATION_H
