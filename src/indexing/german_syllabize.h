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

#ifndef GERMAN_SYLLABLE_H
#define GERMAN_SYLLABLE_H

#include "syllable.h"
#include <utility>

namespace grammar
    {
    /// @brief Counts the number of (German) syllables in a (single-digit) number.
    class syllabize_german_number
        {
      public:
        /** @returns The number of syllables for a given number.
            @param number The number to review.*/
        [[nodiscard]]
        size_t operator()(const wchar_t number) const noexcept
            {
            if (!characters::is_character::is_numeric_simple(number))
                {
                return 0;
                }
            return traits::case_insensitive_ex::eq(number, common_lang_constants::NUMBER_7) ? 2 : 1;
            }
        };

    /// @brief German syllable counting functor utility.
    class german_syllabize final : public base_syllabize
        {
      public:
        german_syllabize() noexcept = default;

        /** @brief Main interface for syllabizing a block of text.
            @param start The start of the text to parse.
            @param length The length of the text.
            @returns The number of syllables counted.*/
        [[nodiscard]]
        size_t operator()(const wchar_t* start, const size_t length) final
            {
            assert(start && std::wcslen(start) >= length);
            // reset our data
            reset();
            if (start == nullptr || length == 0)
                {
                return 0;
                }

            m_length = length;

            if (syllabize_japanese({ start, m_length }))
                {
                return m_syllable_count;
                }

            adjust_length_if_possessive(start);
            const wchar_t* end = start + m_length;

            const std::pair<bool, size_t> mathResult = is_special_math_word({ start, m_length });
            if (mathResult.first)
                {
                return m_syllable_count = mathResult.second;
                }

            if (syllabize_if_contains_periods<german_syllabize>(start, end))
                {
                return m_syllable_count;
                }

            if (syllabize_if_contains_dashes<german_syllabize>(start))
                {
                return m_syllable_count;
                }

            const std::pair<size_t, size_t> prefixResult = get_prefix_length({ start, m_length });
            if (prefixResult.second > 0)
                {
                start += prefixResult.second;
                m_length -= prefixResult.second;
                m_syllable_count += prefixResult.first;
                }
            // if past tense form of verb, skip "ge" prefix
            if (has_past_tense_prefix({ start, m_length }))
                {
                start += 2;
                ++m_syllable_count;
                }

            m_previous_block_vowel = m_previous_vowel = m_length;
            const wchar_t* currentChar = start;

            while (currentChar != end)
                {
                const bool currentCharIsVowel = characters::is_character::is_vowel(currentChar[0]);

                bool nextCharIsVowel = false;
                /* if last letter, then there is no next letter*/
                if ((m_length - 1) == static_cast<size_t>(currentChar - start))
                    {
                    nextCharIsVowel = false;
                    }
                else
                    {
                    nextCharIsVowel = characters::is_character::is_vowel(currentChar[1]);
                    }

                const bool isInVowelBlock = currentCharIsVowel && nextCharIsVowel;
                // if it's a vowel and it's the first one in this block
                if (currentCharIsVowel && !isInVowelBlock)
                    {
                    ++m_syllable_count;
                    m_previous_vowel = currentChar - start;
                    }
                else if (currentCharIsVowel)
                    {
                    const wchar_t* startOfBlock = currentChar;
                    while (currentChar != end && characters::is_character::is_vowel(currentChar[1]))
                        {
                        ++currentChar;
                        }
                    // some vowels blocks split into separate syllables, so count that.
                    m_syllable_count += get_vowel_block_syllable_count(
                        start, startOfBlock - start, (currentChar + 1) - startOfBlock,
                        std::cmp_greater(m_previous_block_vowel, startOfBlock - start));
                    m_previous_vowel = currentChar - start;
                    }
                // syllabize numbers
                else if (characters::is_character::is_numeric_simple(currentChar[0]))
                    {
                    size_t charactersCounted = 0;
                    m_syllable_count += syllabify_numeral<syllabize_german_number>(
                        currentChar, end, charactersCounted, common_lang_constants::COMMA,
                        common_lang_constants::PERIOD);
                    if (charactersCounted == 0)
                        {
                        break;
                        }
                    currentChar += charactersCounted;
                    if (currentChar >= end)
                        {
                        break;
                        }
                    // else, we already moved to the next character to analyze, so just restart loop
                    continue;
                    }
                // syllabize any pertinent symbols
                m_syllable_count += get_symbol_syllable_count(start, end, currentChar);
                if (!currentCharIsVowel)
                    {
                    m_previous_block_vowel = m_previous_vowel;
                    }
                ++currentChar;
                }

            // all words are at least one syllable (even all consonant acronyms)
            m_syllable_count = (m_syllable_count > 0) ? m_syllable_count : 1;
            return m_syllable_count;
            }

      protected:
        /** @brief Sees if a word begins with a "ge" prefix which should always end as
                a syllable division.
            @param theWord The word to parse.
            @returns @c true if the word begins with a valid past tense ("ge") prefix.*/
        [[nodiscard]]
        static bool has_past_tense_prefix(const std::wstring_view theWord) noexcept
            {
            if (theWord.length() >= 2 &&
                traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_G) &&
                traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_E))
                {
                // watch out for "geis" or "geiß"
                return theWord.length() < 4 ||
                       !traits::case_insensitive_ex::eq(theWord[2],
                                                        common_lang_constants::LOWER_I) ||
                       (!traits::case_insensitive_ex::eq(theWord[3],
                                                         common_lang_constants::LOWER_S) &&
                        theWord[3] != common_lang_constants::ESZETT);
                }

            return false;
            }

        /** @brief Sees if a word begins with a special prefix which should always
                end as a syllable division.
            @param theWord The word to parse.
            @returns A pair with the syllable count and length of the prefix.*/
        [[nodiscard]]
        static std::pair<size_t, size_t> get_prefix_length(const std::wstring_view theWord) noexcept
            {
            if (theWord.length() >= 9 &&
                traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_G) &&
                traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_B) &&
                traits::case_insensitive_ex::eq(theWord[3], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(theWord[4], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(theWord[5],
                                                common_lang_constants::LOWER_U_UMLAUTS) &&
                traits::case_insensitive_ex::eq(theWord[6], common_lang_constants::LOWER_B) &&
                traits::case_insensitive_ex::eq(theWord[7], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(theWord[8], common_lang_constants::LOWER_R))
                {
                return std::make_pair(4, 9);
                }
            if (theWord.length() >= 8)
                {
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(theWord[3], common_lang_constants::LOWER_G) &&
                    traits::case_insensitive_ex::eq(theWord[4], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(theWord[5], common_lang_constants::LOWER_B) &&
                    traits::case_insensitive_ex::eq(theWord[6], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(theWord[7], common_lang_constants::LOWER_N))
                    {
                    return std::make_pair(3, 8);
                    }
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_Z) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_W) &&
                    traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(theWord[3], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(theWord[4], common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(theWord[5], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(theWord[6], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(theWord[7], common_lang_constants::LOWER_N))
                    {
                    return std::make_pair(2, 8);
                    }
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_Z) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_U) &&
                    traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(theWord[3], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(theWord[4], common_lang_constants::LOWER_M) &&
                    traits::case_insensitive_ex::eq(theWord[5], common_lang_constants::LOWER_M) &&
                    traits::case_insensitive_ex::eq(theWord[6], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(theWord[7], common_lang_constants::LOWER_N))
                    {
                    return std::make_pair(3, 8);
                    }
                }
            if (theWord.length() >= 7 &&
                traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(theWord[3], common_lang_constants::LOWER_L) &&
                traits::case_insensitive_ex::eq(theWord[4], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(theWord[5], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(theWord[6], common_lang_constants::LOWER_G))
                {
                return std::make_pair(2, 7);
                }
            if (theWord.length() >= 6)
                {
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_Z) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_U) &&
                    traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(theWord[3],
                                                    common_lang_constants::LOWER_U_UMLAUTS) &&
                    traits::case_insensitive_ex::eq(theWord[4], common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(theWord[5], common_lang_constants::LOWER_K))
                    {
                    return std::make_pair(2, 6);
                    }
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_G) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_L) &&
                    traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(theWord[3], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(theWord[4], common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(theWord[5], common_lang_constants::LOWER_H))
                    {
                    return std::make_pair(1, 6);
                    }
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(theWord[3], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(theWord[4], common_lang_constants::LOWER_U) &&
                    (traits::case_insensitive_ex::eq(theWord[5], common_lang_constants::LOWER_F) ||
                     traits::case_insensitive_ex::eq(theWord[5], common_lang_constants::LOWER_S)))
                    {
                    return std::make_pair(2, 6);
                    }
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(theWord[3], common_lang_constants::LOWER_W) &&
                    traits::case_insensitive_ex::eq(theWord[4], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(theWord[5], common_lang_constants::LOWER_G))
                    {
                    return std::make_pair(2, 6);
                    }
                }
            if (theWord.length() >= 5)
                {
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_B) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(theWord[3], common_lang_constants::LOWER_R) &&
                    (traits::case_insensitive_ex::eq(theWord[4], common_lang_constants::LOWER_D) ||
                     traits::case_insensitive_ex::eq(theWord[4], common_lang_constants::LOWER_B)))
                    {
                    return std::make_pair(2, 5);
                    }
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_D) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_B) &&
                    traits::case_insensitive_ex::eq(theWord[3], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(theWord[4], common_lang_constants::LOWER_I))
                    {
                    return std::make_pair(2, 5);
                    }
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_D) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(theWord[3], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(theWord[4], common_lang_constants::LOWER_N))
                    {
                    return std::make_pair(2, 5);
                    }
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_D) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_U) &&
                    traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(theWord[3], common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(theWord[4], common_lang_constants::LOWER_H))
                    {
                    return std::make_pair(1, 5);
                    }
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_M) &&
                    traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_P) &&
                    traits::case_insensitive_ex::eq(theWord[3], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(theWord[4], common_lang_constants::LOWER_R))
                    {
                    return std::make_pair(2, 5);
                    }
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(theWord[3], common_lang_constants::LOWER_Z) &&
                    traits::case_insensitive_ex::eq(theWord[4], common_lang_constants::LOWER_U))
                    {
                    return std::make_pair(2, 5);
                    }
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(theWord[3], common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(theWord[4], common_lang_constants::LOWER_T))
                    {
                    return std::make_pair(1, 5);
                    }
                }
            if (theWord.length() >= 4)
                {
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_B) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_E) &&
                    (traits::case_insensitive_ex::eq(theWord[3], common_lang_constants::LOWER_N) ||
                     traits::case_insensitive_ex::eq(theWord[3], common_lang_constants::LOWER_H)))
                    {
                    return std::make_pair(2, 4);
                    }
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_F) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(theWord[3], common_lang_constants::LOWER_L))
                    {
                    return std::make_pair(1, 4);
                    }
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_F) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(theWord[3], common_lang_constants::LOWER_T))
                    {
                    return std::make_pair(1, 4);
                    }
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_F) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(theWord[3], common_lang_constants::LOWER_T))
                    {
                    return std::make_pair(1, 4);
                    }
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(theWord[3], common_lang_constants::LOWER_H))
                    {
                    return std::make_pair(1, 4);
                    }
                }
            if (theWord.length() >= 3)
                {
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_U) &&
                    (traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_F) ||
                     traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_S)))
                    {
                    return std::make_pair(1, 3);
                    }
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_B) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_I))
                    {
                    return std::make_pair(1, 3);
                    }
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_N))
                    {
                    return std::make_pair(1, 3);
                    }
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_N))
                    {
                    return std::make_pair(1, 3);
                    }
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_R))
                    {
                    return std::make_pair(1, 3);
                    }
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_L) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_S))
                    {
                    return std::make_pair(1, 3);
                    }
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_M) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_T))
                    {
                    return std::make_pair(1, 3);
                    }
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_V) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_R))
                    {
                    return std::make_pair(1, 3);
                    }
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_W) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_G))
                    {
                    return std::make_pair(1, 3);
                    }
                }
            if (theWord.length() >= 2)
                {
                // Irish names with preceding "Mc" is a separate syllable
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_M) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_C))
                    {
                    return std::make_pair(1, 2);
                    }
                // separable prefixes, which need to be stripped in case the word is past tense and
                // has a GE in it
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_N))
                    {
                    return std::make_pair(1, 2);
                    }
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_B))
                    {
                    return std::make_pair(1, 2);
                    }
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_D) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_A))
                    {
                    if (theWord.length() >= 3 &&
                        traits::case_insensitive_ex::eq(theWord[2], common_lang_constants::LOWER_U))
                        {
                        return std::make_pair(0, 0);
                        }
                    return std::make_pair(1, 2);
                    }
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_R))
                    {
                    return std::make_pair(1, 2);
                    }
                if (traits::case_insensitive_ex::eq(theWord[0], common_lang_constants::LOWER_Z) &&
                    traits::case_insensitive_ex::eq(theWord[1], common_lang_constants::LOWER_U))
                    {
                    return std::make_pair(1, 2);
                    }
                }
            return std::make_pair(0, 0);
            }

        /** @brief Sees how many syllables are in a block of vowels.
            @param word The original text block under review.
            @param position The position in @c word where the vowel block under review is at.
            @param vowel_block_size How many vowels are in this block to review.
            @param is_first_vowel_block_in_word Whether this is the first block of
                vowels encountered in @c word.
            @returns The number of syllables in this vowel block.*/
        [[nodiscard]]
        size_t
        get_vowel_block_syllable_count(const wchar_t* word, const size_t position,
                                       const size_t vowel_block_size,
                                       const bool is_first_vowel_block_in_word) const noexcept
            {
            assert(word);
            if (position + vowel_block_size > m_length)
                {
                return 1;
                }
            // EOAU (a rare combination)
            if (vowel_block_size == 4 &&
                traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[position + 1],
                                                common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[position + 2],
                                                common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[position + 3], common_lang_constants::LOWER_U))
                {
                return 3;
                }
            // other four consecutive vowels would be odd, so just return a syllable count of two
            if (vowel_block_size > 3)
                {
                return 2;
                }
            if (vowel_block_size == 3)
                {
                // EAU (imported from French) needs to be overridden here first as not
                // splitting.
                if (traits::case_insensitive_ex::eq(word[position],
                                                    common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position + 1],
                                                    common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_U))
                    {
                    return 1;
                    }
                // IEE
                if (traits::case_insensitive_ex::eq(word[position],
                                                    common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position + 1],
                                                    common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_E))
                    {
                    return 2;
                    }
                // EEI
                if (traits::case_insensitive_ex::eq(word[position],
                                                    common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position + 1],
                                                    common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_I))
                    {
                    return 2;
                    }
                // EEA (very rare, on in compound words)
                if (traits::case_insensitive_ex::eq(word[position],
                                                    common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position + 1],
                                                    common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_A))
                    {
                    return 2;
                    }
                // AUE
                if (traits::case_insensitive_ex::eq(word[position],
                                                    common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(word[position + 1],
                                                    common_lang_constants::LOWER_U) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_E))
                    {
                    return 2;
                    }
                // AUI
                if (traits::case_insensitive_ex::eq(word[position],
                                                    common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(word[position + 1],
                                                    common_lang_constants::LOWER_U) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_I))
                    {
                    return 2;
                    }
                // EUE
                if (traits::case_insensitive_ex::eq(word[position],
                                                    common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position + 1],
                                                    common_lang_constants::LOWER_U) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_E))
                    {
                    return 2;
                    }
                // EIE
                if (traits::case_insensitive_ex::eq(word[position],
                                                    common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position + 1],
                                                    common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_E))
                    {
                    return 2;
                    }
                // EOÜ (a rare combination)
                if (traits::case_insensitive_ex::eq(word[position],
                                                    common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position + 1],
                                                    common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_U_UMLAUTS))
                    {
                    return 3;
                    }

                return 1;
                }
            if (vowel_block_size == 2)
                {
                // UELL
                if (m_length >= 4 && position <= m_length - 4 &&
                    traits::case_insensitive_ex::eq(word[position],
                                                    common_lang_constants::LOWER_U) &&
                    traits::case_insensitive_ex::eq(word[position + 1],
                                                    common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_L) &&
                    traits::case_insensitive_ex::eq(word[position + 3],
                                                    common_lang_constants::LOWER_L))
                    {
                    // except for "quell"
                    if (position >= 1 && traits::case_insensitive_ex::eq(
                                             word[position - 1], common_lang_constants::LOWER_Q))
                        {
                        return 1;
                        }

                    return 2;
                    }
                // UAR
                if (m_length >= 3 && position <= m_length - 3 &&
                    traits::case_insensitive_ex::eq(word[position],
                                                    common_lang_constants::LOWER_U) &&
                    traits::case_insensitive_ex::eq(word[position + 1],
                                                    common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_R))
                    {
                    // except for "quar"
                    if (position >= 1 && traits::case_insensitive_ex::eq(
                                             word[position - 1], common_lang_constants::LOWER_Q))
                        {
                        return 1;
                        }

                    return 2;
                    }
                // IO always splits, even in a "TION" ending of a word (unlike English)
                if (traits::case_insensitive_ex::eq(word[position],
                                                    common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position + 1],
                                                    common_lang_constants::LOWER_O))
                    {
                    return 2;
                    }
                // IA always splits
                if (traits::case_insensitive_ex::eq(word[position],
                                                    common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position + 1],
                                                    common_lang_constants::LOWER_A))
                    {
                    return 2;
                    }
                // EO
                if (traits::case_insensitive_ex::eq(word[position],
                                                    common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position + 1],
                                                    common_lang_constants::LOWER_O))
                    {
                    return 2;
                    }
                if (traits::case_insensitive_ex::eq(word[position],
                                                    common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position + 1],
                                                    common_lang_constants::LOWER_O_UMLAUTS))
                    {
                    return 2;
                    }
                // kOEfficient (special case)
                if (position > 0 && m_length >= 5 && position <= m_length - 4 &&
                    traits::case_insensitive_ex::eq(word[position - 1],
                                                    common_lang_constants::LOWER_K) &&
                    traits::case_insensitive_ex::eq(word[position],
                                                    common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(word[position + 1],
                                                    common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_F) &&
                    traits::case_insensitive_ex::eq(word[position + 3],
                                                    common_lang_constants::LOWER_F))
                    {
                    return 2;
                    }
                // EA
                if (traits::case_insensitive_ex::eq(word[position],
                                                    common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position + 1],
                                                    common_lang_constants::LOWER_A))
                    {
                    return 2;
                    }
                if (traits::case_insensitive_ex::eq(word[position],
                                                    common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position + 1],
                                                    common_lang_constants::LOWER_A_UMLAUTS))
                    {
                    return 2;
                    }
                // OA
                if (traits::case_insensitive_ex::eq(word[position],
                                                    common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(word[position + 1],
                                                    common_lang_constants::LOWER_A))
                    {
                    // TOAST would be one syllable
                    if (position == 1 && traits::case_insensitive_ex::eq(
                                             word[position - 1], common_lang_constants::LOWER_T))
                        {
                        return 1;
                        }

                    return 2;
                    }
                // IEN
                if (!is_first_vowel_block_in_word && m_length >= 4 && position == m_length - 3 &&
                    traits::case_insensitive_ex::eq(word[position],
                                                    common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position + 1],
                                                    common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_N))
                    {
                    return 2;
                    }
                // IENS
                if (!is_first_vowel_block_in_word && m_length >= 5 && position == m_length - 4 &&
                    traits::case_insensitive_ex::eq(word[position],
                                                    common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position + 1],
                                                    common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(word[position + 3],
                                                    common_lang_constants::LOWER_S))
                    {
                    return 2;
                    }
                // IELL
                if (!is_first_vowel_block_in_word && m_length >= 6 && position <= m_length - 4 &&
                    traits::case_insensitive_ex::eq(word[position],
                                                    common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position + 1],
                                                    common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_L) &&
                    traits::case_insensitive_ex::eq(word[position + 3],
                                                    common_lang_constants::LOWER_L))
                    {
                    return 2;
                    }

                return 1;
                }

            return 1;
            }
        };
    } // namespace grammar

#endif // GERMAN_SYLLABLE_H
