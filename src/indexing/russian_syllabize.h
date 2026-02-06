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

#ifndef RUSSIAN_SYLLABLE_H
#define RUSSIAN_SYLLABLE_H

#include "syllable.h"
#include <utility>

namespace grammar
    {
    /// Counts the number of syllables in a (single-digit) number.
    class syllabize_russian_number
        {
      public:
        size_t operator()(const wchar_t number) const noexcept
            {
            /// @todo change syllable count to language-specific values.
            return ((number == common_lang_constants::NUMBER_2 ||
                     number == common_lang_constants::NUMBER_3 ||
                     number == common_lang_constants::NUMBER_6) ||
                    (number == common_lang_constants::NUMBER_2_FULL_WIDTH ||
                     number == common_lang_constants::NUMBER_3_FULL_WIDTH ||
                     number == common_lang_constants::NUMBER_6_FULL_WIDTH)) ?
                       1 :
                   ((number == common_lang_constants::NUMBER_0 ||
                     number == common_lang_constants::NUMBER_1 ||
                     number == common_lang_constants::NUMBER_4 ||
                     number == common_lang_constants::NUMBER_5 ||
                     number == common_lang_constants::NUMBER_8) ||
                    (number == common_lang_constants::NUMBER_0_FULL_WIDTH ||
                     number == common_lang_constants::NUMBER_1_FULL_WIDTH ||
                     number == common_lang_constants::NUMBER_4_FULL_WIDTH ||
                     number == common_lang_constants::NUMBER_5_FULL_WIDTH ||
                     number == common_lang_constants::NUMBER_8_FULL_WIDTH)) ?
                       2 :
                   ((number == common_lang_constants::NUMBER_7 ||
                     number == common_lang_constants::NUMBER_9) ||
                    (number == common_lang_constants::NUMBER_7_FULL_WIDTH ||
                     number == common_lang_constants::NUMBER_9_FULL_WIDTH)) ?
                       3 :
                       0;
            }
        };

    /* Syllable counting functor utility
       @warning This is experimental and not in use.*/
    template<typename Tcharacter_traits = traits::case_insensitive>
    class russian_syllabize : public base_syllabize
        {
      public:
        russian_syllabize() {}

        /// Main interface for syllabizing a block of text.
        size_t operator()(const wchar_t* start, const size_t length)
            {
            // reset our data
            reset();
            if (start == nullptr || length == 0)
                {
                return 0;
                }

            m_length = length;
            adjust_length_if_possessive(start);
            const wchar_t* end = start + m_length;

            if (syllabize_if_contains_periods<russian_syllabize<Tcharacter_traits>>(start, end))
                {
                return m_syllable_count;
                }

            if (syllabize_if_contains_dashes<russian_syllabize<Tcharacter_traits>>(start))
                {
                return m_syllable_count;
                }

            const std::pair<size_t, size_t> prefixResult = get_prefix_length(start, m_length);
            if (prefixResult.second > 0)
                {
                start += prefixResult.second;
                m_length -= prefixResult.second;
                m_syllable_count += prefixResult.first;
                }

            m_previous_block_vowel = m_previous_vowel = m_length;
            bool is_in_vowel_block = false;
            bool current_char_is_vowel = false;
            const wchar_t* current_char = start;

            while (current_char != end)
                {
                current_char_is_vowel = isChar.is_vowel(current_char[0]);

                bool next_char_is_vowel = false;
                /* if last letter, then there is no next letter*/
                if ((m_length - 1) == static_cast<size_t>(current_char - start))
                    {
                    next_char_is_vowel = false;
                    }
                else
                    {
                    next_char_is_vowel = isChar.is_vowel(current_char[1]);
                    }

                is_in_vowel_block = current_char_is_vowel && next_char_is_vowel;
                // if it's a vowel and it's the first one in this block
                if (current_char_is_vowel && !is_in_vowel_block)
                    {
                    ++m_syllable_count;
                    m_previous_vowel = current_char - start;
                    }
                else if (current_char_is_vowel && is_in_vowel_block)
                    {
                    const wchar_t* start_of_block = current_char;
                    while (current_char != end && isChar.is_vowel(current_char[1]))
                        {
                        ++current_char;
                        }
                    // if it is two consecutive vowels then make sure they
                    // aren't separate syllables
                    if (is_vowels_separate_syllables(
                            start, start_of_block - start, (current_char + 1) - start_of_block,
                            m_previous_block_vowel > static_cast<size_t>(start_of_block - start)))
                        {
                        m_syllable_count += 2;
                        }
                    else
                        {
                        ++m_syllable_count;
                        }
                    m_previous_vowel = current_char - start;
                    }
                // syllabize numbers
                else if (characters::is_character::is_numeric(current_char[0]))
                    {
                    size_t characters_counted = 0;
                    m_syllable_count += syllabify_numeral<syllabize_russian_number>(
                        current_char, end, characters_counted, common_lang_constants::PERIOD,
                        common_lang_constants::COMMA);
                    current_char += characters_counted;
                    if (current_char >= end)
                        {
                        break;
                        }
                    // else, we already moved to the next character to analyze, so just restart loop
                    else
                        {
                        continue;
                        }
                    }
                // syllabize any pertinent symbols
                m_syllable_count += get_symbol_syllable_count(start, end, current_char);
                if (!current_char_is_vowel)
                    {
                    m_previous_block_vowel = m_previous_vowel;
                    }
                ++current_char;
                }

            // all words are at least one syllable (even all consonant acronyms)
            m_syllable_count = (m_syllable_count > 0) ? m_syllable_count : 1;
            return m_syllable_count;
            }

      protected:
        /// Sees if a word begins with a special prefixes which should always end as a syllable
        /// division. Note that some of the prefixes from the article are omitted because they
        /// actually produce incorrect results.
        /// @returns A pair with the syllable count and length of the prefix
        inline static std::pair<size_t, size_t> get_prefix_length(const wchar_t* start,
                                                                  const size_t length)
            {
            /// @todo dummy holder
            return std::pair<size_t, size_t>(0, 0);
            }

        //--------------------------------------------------
        bool is_vowels_separate_syllables(const wchar_t word[], const size_t position,
                                          const size_t vowel_block_size,
                                          const bool is_first_vowel_block_in_word) const
            {
            return false; ///@todo dummy holder
            }
        };
    } // namespace grammar

#endif // RUSSIAN_SYLLABLE_H
