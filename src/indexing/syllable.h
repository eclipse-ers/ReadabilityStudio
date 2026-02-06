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

#ifndef ENGLISH_SYLLABLE_H
#define ENGLISH_SYLLABLE_H

#include "../OleanderStemmingLibrary/src/common_lang_constants.h"
#include "../Wisteria-Dataviz/src/util/string_util.h"
#include "character_traits.h"
#include "characters.h"
#include <set>
#include <utility>

namespace grammar
    {
    /// @brief Base syllable counting functor class.
    class base_syllabize
        {
      public:
        base_syllabize() noexcept = default;

        base_syllabize(const base_syllabize& that) = delete;
        base_syllabize& operator=(const base_syllabize& that) = delete;

        virtual ~base_syllabize() = default;

        virtual size_t operator()(const wchar_t* start, const size_t length) = 0;

      protected:
        void reset() noexcept
            {
            m_syllable_count = m_length = m_previous_vowel = m_previous_block_vowel = 0;
            }

        /** @brief Counts Japanese ideographs in a word, treating each as one syllable.
            @details If the word is entirely Japanese ideographs, the syllable count
                is set to the word length and @c true is returned.
                If the word is a mix of Japanese and non-Japanese characters,
                the Japanese characters are counted and added to the syllable count,
                and @c false is returned so the caller can handle the rest.
            @param theWord The word to analyze.
            @returns @c true if the entire word was Japanese and fully syllabized.*/
        bool syllabize_japanese(std::wstring_view theWord);

        /** @brief Special case mathematical terms that need to be counted differently.
            @param theWord The word to analyze.
            @returns A pair indicating whether this is a special case,
                and if so the syllable count.*/
        [[nodiscard]]
        static std::pair<bool, size_t> is_special_math_word(std::wstring_view theWord) noexcept;

        /** @brief Determines the number of syllables for a numeric string.
            @param numeral_string The string to analyze.
            @param end_of_string The sentinel of the main string (can't read beyond that point).
            @param[out] characters_counted The number of characters in the string
                that were a numeric value
            @param thousands_separator The character used for the thousands separator.
            @param decimal_separator The character used for the decimal separator.
            @returns The number of syllables for a numeric string.*/
        template<typename Tsyllabize_number>
        [[nodiscard]]
        static size_t syllabify_numeral(const wchar_t* numeral_string, const wchar_t* end_of_string,
                                        size_t& characters_counted,
                                        const wchar_t thousands_separator,
                                        const wchar_t decimal_separator) noexcept
            {
            assert(numeral_string);
            assert(end_of_string);
            characters_counted = 0;
            if (numeral_string == nullptr || end_of_string == nullptr)
                {
                return 0;
                }
            Tsyllabize_number syllabifyNumber{};
            const wchar_t* const start = numeral_string;
            size_t syllableCount{ 0 };
            while (numeral_string[0] && numeral_string < end_of_string)
                {
                // skip thousands separator
                if (traits::case_insensitive_ex::eq(numeral_string[0], thousands_separator))
                    {
                    ++numeral_string;
                    continue;
                    }
                // decimal separator counts as one syllable ("point" or "dot" in most languages)
                if (traits::case_insensitive_ex::eq(numeral_string[0], decimal_separator))
                    {
                    ++syllableCount;
                    ++numeral_string;
                    continue;
                    }
                if (characters::is_character::is_numeric_simple(numeral_string[0]))
                    {
                    syllableCount += syllabifyNumber(numeral_string[0]);
                    }
                else
                    {
                    break;
                    }
                ++numeral_string;
                }
            characters_counted = (numeral_string - start);
            return syllableCount;
            }

        /** @brief Syllabizes a symbol, dependent on its position in the word.
            @param start The start of the text under review.
            @param end The end of the text under review.
            @param current_char The current position (of the symbol) in the
                text under review to analyze.
            @returns The syllable count of the symbol.*/
        [[nodiscard]]
        static size_t get_symbol_syllable_count(const wchar_t* start, const wchar_t* end,
                                                const wchar_t* current_char) noexcept;
        /** @brief For possessive words (ape's), this will adjust the length to
                ignore the possessive part of the word.
            @note This adjustment is done by shortening the internal length variable,
                it does not change the text buffer.
            @param start The text being reviewed.*/
        void adjust_length_if_possessive(const wchar_t* start);

        /** @brief If there are any periods in this word, then break it up into smaller words.
            @returns @c true if word was split and syllabized.
            @param start The start of the word.
            @param end The end of the word.*/
        template<typename Tsyllabizer>
        bool syllabize_if_contains_periods(const wchar_t* start, const wchar_t* end)
            {
            size_t periodPos = std::wstring_view{ start, m_length }.find_first_of(L".\uFF0E\uFF61");
            if (periodPos != std::wstring_view::npos)
                {
                const wchar_t* period{ start + periodPos };
                Tsyllabizer dotSyllabize{};
                size_t periodCount{ 0 };
                size_t separateSectionsSyllableCount{ 0 };
                const wchar_t* currentSection{ start };

                while (period != nullptr)
                    {
                    /* don't consider this a valid dot in the word if it is at the end and is the
                       only one in the word. In this case it would be an initial.*/
                    if (periodCount == 0 && (end - 1) == period)
                        { /*NOOP*/
                        }
                    else
                        {
                        ++periodCount;
                        }
                    separateSectionsSyllableCount +=
                        dotSyllabize(currentSection, period - currentSection);
                    currentSection = period + 1;
                    if (std::cmp_greater_equal(currentSection - start, m_length))
                        {
                        break;
                        }
                    periodPos = std::wstring_view{ ++period, m_length - (currentSection - start) }
                                    .find_first_of(L".\uFF0E\uFF61");
                    if (periodPos == std::wstring_view::npos)
                        {
                        break;
                        }
                    std::advance(period, periodPos);
                    }
                separateSectionsSyllableCount +=
                    dotSyllabize(currentSection, m_length - (currentSection - start));
                m_syllable_count =
                    (separateSectionsSyllableCount > 0) ? separateSectionsSyllableCount : 1;
                /* if there was a single period in the word (that was not at the end) then
                   count it as "dot" or "point" like in a number. If more than one period then
                   this is more than likely an acronym and we don't count these.*/
                if (periodCount == 1)
                    {
                    ++m_syllable_count;
                    }
                return true;
                }

            return false;
            }

        /** @brief If there are any dashes in this word, then break it up into smaller words.
            @returns @c true if word was split and syllabized.
            @param start The start of the word.*/
        template<typename Tsyllabizer>
        [[nodiscard]]
        bool syllabize_if_contains_dashes(const wchar_t* start)
            {
            size_t dashPos = std::wstring_view{ start, m_length }.find_first_of(L"-\uFF0D");
            if (dashPos != std::wstring_view::npos)
                {
                const wchar_t* dash{ start + dashPos };
                Tsyllabizer dashSyllabize{};
                size_t separateSectionsSyllableCount{ 0 };
                const wchar_t* currentSection{ start };

                while (dash != nullptr)
                    {
                    separateSectionsSyllableCount +=
                        dashSyllabize(currentSection, dash - currentSection);
                    currentSection = dash + 1;
                    if (std::cmp_greater_equal(currentSection - start, m_length))
                        {
                        break;
                        }
                    dashPos = std::wstring_view{ ++dash, m_length - (currentSection - start) }
                                  .find_first_of(L"-\uFF0D");
                    if (dashPos == std::wstring_view::npos)
                        {
                        break;
                        }
                    std::advance(dash, dashPos);
                    }
                separateSectionsSyllableCount +=
                    dashSyllabize(currentSection, m_length - (currentSection - start));
                m_syllable_count =
                    (separateSectionsSyllableCount > 0) ? separateSectionsSyllableCount : 1;
                return true;
                }

            return false;
            }

        /** @brief Determines if a 'y' is a consonant.
            @param word The text being examined.
            @param position Where the 'y' being examined is in the block of text.
            @returns @c true if the letter at "position" is a consonant 'y'.
            @bug benzoyl is broken*/
        [[nodiscard]]
        bool is_consonant_y(const wchar_t* word, size_t position) const;
        characters::is_character isChar;
        size_t m_syllable_count{ 0 };
        size_t m_length{ 0 };
        size_t m_previous_vowel{ 0 };
        size_t m_previous_block_vowel{ 0 };
        };

    /// @brief Counts the number of (English) syllables in a (single-digit) number.
    class syllabize_english_number
        {
      public:
        [[nodiscard]]
        constexpr size_t operator()(const wchar_t number) const noexcept
            {
            return ((number >= common_lang_constants::NUMBER_1 &&
                     number <= common_lang_constants::NUMBER_6) ||
                    (number >= common_lang_constants::NUMBER_1_FULL_WIDTH &&
                     number <= common_lang_constants::NUMBER_6_FULL_WIDTH)) ?
                       1 :
                   ((number == common_lang_constants::NUMBER_8 ||
                     number == common_lang_constants::NUMBER_9) ||
                    (number == common_lang_constants::NUMBER_8_FULL_WIDTH ||
                     number == common_lang_constants::NUMBER_9_FULL_WIDTH)) ?
                       1 :
                   ((number == common_lang_constants::NUMBER_0 ||
                     number == common_lang_constants::NUMBER_7) ||
                    (number == common_lang_constants::NUMBER_0_FULL_WIDTH ||
                     number == common_lang_constants::NUMBER_7_FULL_WIDTH)) ?
                       2 :
                       0;
            }
        };

    /// @brief English syllable counting functor utility.
    class english_syllabize final : public base_syllabize
        {
      public:
        /// Constructor.
        english_syllabize() noexcept = default;

        /** @brief Counts the syllables in a block of text.
            @param start The start of the block of text.
            @param length The length of the text.
            @returns The number of syllables in the text.*/
        [[nodiscard]]
        size_t operator()(const wchar_t* start, const size_t length) final;

      private:
        /// @brief Analyzes the overall string for special situations that
        ///     they standard syllabizer would have missed.
        /// @param start The section of text to finalize.
        void finalize_special_cases(const wchar_t* start);
        /** @brief Determines if a 'u' at given position is silent.
            @param word The current word being analyzed (must be the start of the word).
            @param position The position of the vowel.
            @returns @c true if 'u' at @position is silent.*/
        [[nodiscard]]
        bool is_silent_u(const wchar_t* word, size_t position) const;
        //--------------------------------------------------
        [[nodiscard]]
        bool does_prefix_silence_e(const wchar_t* word, size_t prefix_length) const;
        //--------------------------------------------------
        [[nodiscard]]
        static bool does_prefix_disconnect_e(const wchar_t* word, size_t prefix_length);
        //--------------------------------------------------
        [[nodiscard]]
        static bool is_none_affecting_suffix(const wchar_t* word, size_t suffix_length);
        /// @returns @c true if suffix makes the preceding 'e' NOT silent
        [[nodiscard]]
        static bool does_suffix_negate_silent_e(const wchar_t* suffix, const size_t suffix_length,
                                                const size_t next_vowel_index);
        /** @returns @c true if a single (non-'e') vowel
                (i.e., vowel surrounded by consonants) should not be seen as a syllable break.
                There are fairly uncommon.
            @param word The current word being analyzed (must be the start of the word).
            @param position The position of the vowel.
            @note Do not use this for 'e's; is_silent_e() should be called for those.*/
        [[nodiscard]]
        bool is_single_non_e_vowel_ignored(const wchar_t* word, const size_t position) const;
        /** @brief Determines if an 'e' at given position is silent.
            @param word The current word being analyzed (must be the start of the word).
            @param position The position of the vowel.
            @returns @c true if 'e' at @position is silent.*/
        [[nodiscard]]
        bool is_silent_e(const wchar_t* word, const size_t position) const;
        //--------------------------------------------------
        [[nodiscard]]
        bool is_vowels_separate_syllables(const wchar_t* word, const size_t position,
                                          const size_t vowel_block_size,
                                          const bool is_first_vowel_block_in_word) const;
        /** @returns @c true if a consonant block can be joined with the
                preceding vowels to form a single syllable.*/
        [[nodiscard]]
        static bool can_consonants_end_sound(const wchar_t* consonants, size_t block_length);
        //--------------------------------------------------
        [[nodiscard]]
        static bool can_consonants_be_modified_by_following_e(const wchar_t* consonants,
                                                              size_t block_length);
        //--------------------------------------------------
        [[nodiscard]]
        static bool can_consonants_begin_sound(const wchar_t* consonants, size_t block_length);
        /// @brief Sees if a word begins with a special prefixes which should always end
        ///     as a syllable division.
        /// @returns A pair with the syllable count and length of the prefix
        [[nodiscard]]
        static std::pair<size_t, size_t> get_prefix_length(const wchar_t* start,
                                                           const size_t length);

        static const std::set<traits::case_insensitive_wstring_ex> m_e_disconnecting_prefixes;
        static const std::set<traits::case_insensitive_wstring_ex> m_non_affecting_suffixes_4;

        bool m_was_last_vowel_block_separable_vowels{ false };
        bool m_ends_with_nt_contraction{ false };
        };
    } // namespace grammar

#endif // ENGLISH_SYLLABLE_H
