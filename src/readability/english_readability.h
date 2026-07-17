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

#ifndef ENGLISH_READABILITY_H
#define ENGLISH_READABILITY_H

#include "../indexing/syllable.h"
#include "readability.h"

/** Readability Analysis

    General U.S. Grade level information:
    - 18+ Reading level of a Ph.D. (federal tax returns, insurance policies, legal documents)
    - 16 Bachelor's Degree (technical writing for industry, military, or research)
    - 13+ First year of college (usually ignored and/or misunderstood by most adults)
    - 12 High School graduates (Harper's Magazine, The Atlantic Monthly)
    - 11 High School Juniors (Time, The Wall Street Journal)
    - 10 High School Sophomores (Reader's Digest, Saturday Evening Post)
    - 7 Seventh grade (most of the Bible)
    - 6 Sixth grade (too simple to hold the interest of most adults)*/
namespace readability
    {
    constexpr size_t FORCAST_WORD_COEFFICIENT = 150;
    constexpr size_t PSK_WORD_COEFFICIENT = 100;
    constexpr size_t COLEMAN_LIAU_WORD_COEFFICIENT = 100;
    constexpr size_t DALE_CHALL_SAMPLE_SIZE_COEFFICIENT = 100;
    constexpr size_t RAYGOR_SAMPLE_SIZE_COEFFICIENT = 100;

    /// @brief The difficulty level of a document, determined by the Eflaw test.
    enum class eflaw_difficulty
        {
        /// Very easy
        eflaw_very_easy,
        /// Easy
        eflaw_easy,
        /// Confusing
        eflaw_confusing,
        /// Very confusing
        eflaw_very_confusing
        };

    /** @brief Helper function to convert eflaw index to human-readable difficulty level.
        @param index The Eflaw index score to convert.
        @returns The Eflaw difficulty level.*/
    [[nodiscard]]
    inline eflaw_difficulty eflaw_index_to_difficulty(const size_t index) noexcept
        {
        if (index <= 20)
            {
            return eflaw_difficulty::eflaw_very_easy;
            }
        if (index <= 25)
            {
            return eflaw_difficulty::eflaw_easy;
            }
        if (index <= 29)
            {
            return eflaw_difficulty::eflaw_confusing;
            }

        return eflaw_difficulty::eflaw_very_confusing;
        }

    /** McAlpine EFLAW. Test for detecting flawed (i.e., difficult) English as a
        secondary language text. Based on sentence length and miniwords
        (words of one to three letters, excluding punctuation).
        @param[out] difficulty_level The calculated difficulty level.
        @param number_of_words The number of words in the sample.
        @param number_of_mini_words The number of miniwords
            (words containing 1, 2, or 3 characters).
        @param number_of_sentences The number of sentences.
        @returns The calculated index.*/
    [[nodiscard]]
    inline size_t eflaw(eflaw_difficulty& difficulty_level, const uint32_t number_of_words,
                        const uint32_t number_of_mini_words, const uint32_t number_of_sentences)
        {
        NON_UNIT_TEST_ASSERT(number_of_mini_words <= number_of_words);
        if (number_of_sentences == 0)
            {
            throw std::domain_error("invalid sentence count.");
            }
        auto result = static_cast<size_t>(round_to_integer(safe_divide<double>(
            (static_cast<double>(number_of_words) + number_of_mini_words), number_of_sentences)));
        if (result == 0)
            {
            result = 1;
            }

        difficulty_level = eflaw_index_to_difficulty(result);

        return result;
        }

    /** Danielson Bryan #1, which is a grade scale test.
        It seems that this test's purpose was nothing more than a chance to demonstrate
        a faster (non-syllable counting) and word parsing algorithm for UNIVAC computers.
        In the original article, the statistic of "number of spaces between words" is suggested,
        rather than explicitly saying "number of words". Logically, this should be the
        number of words minus 1.
        However, in the article's example, number of spaces between words is the same as
        number of words (even when there are dashes connecting words).
        It is therefore assumed that the authors' intention
        was to count the number of words using the following logic:
            1. Count the number of spaces.
            2. Treat dashes connecting words as spaces.
            3. Add 1, to take into account how there would be one less space than words.
        For this reason, the "number of words" statistic is a more accurate description
        of what the authors intended.
        @param number_of_spaces The number of spaces between words (i.e., number of words).
        @param number_of_characters_and_punctuation The number of characters and punctuation.
        @param number_of_sentences The number of sentences.
        @returns The calculated grade level.*/
    [[nodiscard]]
    inline double danielson_bryan_1(const uint32_t number_of_spaces,
                                    const uint32_t number_of_characters_and_punctuation,
                                    const uint32_t number_of_sentences)
        {
        if (number_of_spaces == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word/sentence count.");
            }
        const auto CPSp =
            safe_divide<double>(number_of_characters_and_punctuation, number_of_spaces);
        const auto CPSt =
            safe_divide<double>(number_of_characters_and_punctuation, number_of_sentences);
        return truncate_k12_plus_grade(1.0364 * CPSp + .0194 * CPSt - .6059);
        }

    /** Danielson Bryan #2, which is a Flesch Reading Ease derivative.
        Refer to note above about number of spaces counting.
        @param number_of_spaces The number of spaces between words (i.e., number of words).
        @param number_of_characters_and_punctuation The number of characters and punctuation.
        @param number_of_sentences The number of sentences.
        @returns The calculated Flesch Reading Ease index.*/
    [[nodiscard]]
    inline double danielson_bryan_2(const uint32_t number_of_spaces,
                                    const uint32_t number_of_characters_and_punctuation,
                                    const uint32_t number_of_sentences)
        {
        if (number_of_spaces == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word/sentence count.");
            }
        const auto CPSp =
            safe_divide<double>(number_of_characters_and_punctuation, number_of_spaces);
        const auto CPSt =
            safe_divide<double>(number_of_characters_and_punctuation, number_of_sentences);
        return std::clamp<double>(131.059 - 10.364 * CPSp - .194 * CPSt, 0, 100);
        }

    /** Easy Listening Formula, or ELF.
        This test is designed for "listenability" and is meant for radio and television broadcasts.
        Rather than penalizing for long sentences, it penalizes high concentrations
            of complex words.
        @param number_of_words The number of words in the sample.
        @param number_of_syllables The number of syllables.
        @param number_of_sentences The number of sentences.
        @returns The calculated grade level.
        @note Technically, you are supposed to go by sentence-by-sentence and calculate their
         ELF scores (tally the number of syllables over one for each word) and then get the average
         of all the sentences' ELFs.
         A shorthand way to do this is simply to subtract the total number of words from
         the total number of syllables (which is basically the same as subtracting one syllable
         from each word), and then divide by the number of sentences.*/
    [[nodiscard]]
    inline double easy_listening_formula(const uint32_t number_of_words,
                                         const uint32_t number_of_syllables,
                                         const uint32_t number_of_sentences)
        {
        if (number_of_sentences == 0)
            {
            throw std::domain_error("invalid sentence count.");
            }
        return truncate_k12_plus_grade(
            safe_divide<double>((number_of_syllables - number_of_words), number_of_sentences));
        }

    /** Automated Readability Index test, also known as "ARI" or "auto."
        For the U.S. Air Force, Smith and Senter (1967) created the Automated Readability Index,
        a U.S. grade level test, designed for technical manuals.
        @code
        ARI = (4.71*(C/W)) + (0.5*(W/S)) - 21.43
        @endcode
        @param number_of_words The number of words in the sample.
        @param number_of_characters The number of letters (and punctuation, like apostrophes).
        @param number_of_sentences The number of sentences.
        @returns The calculated grade level.*/
    [[nodiscard]]
    inline double automated_readability_index(const uint32_t number_of_words,
                                              const uint32_t number_of_characters,
                                              const uint32_t number_of_sentences)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word/sentence count.");
            }
        return truncate_k12_plus_grade(
            (4.71 * (safe_divide<double>(number_of_characters, number_of_words))) +
            (0.5 * (safe_divide<double>(number_of_words, number_of_sentences))) - 21.43);
        }

    /** New Automated Readability Index test.
        Designed for technical manuals, part of the Navy Readability Indices.
        @param number_of_words The number of words in the sample.
        @param number_of_characters The number of characters.
        @param number_of_sentences The number of sentences.
        @returns The calculated grade level.*/
    [[nodiscard]]
    inline double new_automated_readability_index(const uint32_t number_of_words,
                                                  const uint32_t number_of_characters,
                                                  const uint32_t number_of_sentences)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word/sentence count.");
            }
        return truncate_k12_plus_grade(
            (5.84 * safe_divide<double>(number_of_characters, number_of_words)) +
            (0.37 * safe_divide<double>(number_of_words, number_of_sentences)) - 26.01);
        }

    /** New Automated Readability Index (Simplified) test.
        Designed for technical manuals, part of the Navy Readability Indices.
        @param number_of_words The number of words in the sample.
        @param number_of_characters The number of characters.
        @param number_of_sentences The number of sentences.
        @returns The calculated grade level.*/
    [[nodiscard]]
    inline double simplified_automated_readability_index(const uint32_t number_of_words,
                                                         const uint32_t number_of_characters,
                                                         const uint32_t number_of_sentences)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word/sentence count.");
            }
        return truncate_k12_plus_grade(
            (6 * safe_divide<double>(number_of_characters, number_of_words)) +
            (0.4 * safe_divide<double>(number_of_words, number_of_sentences)) - 27.4);
        }

    /** PSK variation of original Dale-Chall formula created in 1958.
        It uses updated criteria based on the newer 1952 McCall-Crabbs tests.
        @param number_of_words The number of words in the sample.
        @param number_of_unfamiliar_words The number of unfamiliar Dale-Chall words.
        @param number_of_sentences The number of sentences.
        @returns The calculated grade level.*/
    [[nodiscard]]
    inline double powers_sumner_kearl_dale_chall(const uint32_t number_of_words,
                                                 const uint32_t number_of_unfamiliar_words,
                                                 const uint32_t number_of_sentences)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("Not enough sentences in formula.");
            }
        const auto ASL = safe_divide<double>(number_of_words, number_of_sentences);
        const double percentOfNonDCWords =
            safe_divide<double>(number_of_unfamiliar_words, number_of_words) * 100;
        return truncate_k12_plus_grade((3.2672 + (.0596 * ASL) + (.1155 * percentOfNonDCWords)));
        }

    /** @brief Dale-Chall table look-up function from the 1995 book.
        @param[out] grade_range_begin The start of the calculated grade range score.
        @param[out] grade_range_end The end of the calculated grade range score.
        @param number_of_words The number of words in the sample.
        @param number_of_unfamiliar_words The number of unfamiliar words.
        @param number_of_sentences The number of sentences.
        @todo Add predicted Cloze score to the results.*/
    inline void new_dale_chall(size_t& grade_range_begin, size_t& grade_range_end,
                               size_t number_of_words, size_t number_of_unfamiliar_words,
                               size_t number_of_sentences)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("Not enough sentences or words in formula.");
            }

        // sample is valid and reasonable, so see what the grade levels are
        const double sampleSizeNormalizationFactor =
            DALE_CHALL_SAMPLE_SIZE_COEFFICIENT / static_cast<double>(number_of_words);
        number_of_sentences =
            static_cast<size_t>(number_of_sentences * sampleSizeNormalizationFactor);
        number_of_unfamiliar_words =
            static_cast<size_t>(number_of_unfamiliar_words * sampleSizeNormalizationFactor);

        /* even after normalization, the text may have too many sentences or
           unfamiliar words--in that case consider it to be very high level reading*/
        if (number_of_sentences > 50 || number_of_unfamiliar_words > 50)
            {
            grade_range_begin = grade_range_end = 16;
            return;
            }

        switch (number_of_sentences)
            {
        case 1:
            grade_range_begin = grade_range_end = 16;
            break;
        case 2:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 2))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 3, 8))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 9, 15))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 3:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 2))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 3, 8))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 9, 14))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 15, 21))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 22, 27))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 4:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 2))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 3, 8))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 9, 14))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 15, 20))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 21, 27))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 28, 33))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 5:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 1))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 2, 6))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 7, 11))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 12, 18))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 19, 24))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 25, 30))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 31, 37))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 6:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 3))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 4, 8))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 9, 14))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 15, 20))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 21, 26))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 27, 33))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 34, 39))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 7:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 1))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 2, 5))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 6, 10))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 11, 15))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 16, 22))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 23, 28))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 29, 34))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 35, 41))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 8:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 2))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 3, 6))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 7, 11))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 12, 17))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 18, 23))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 24, 29))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 30, 36))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 37, 42))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 9:
            if (number_of_unfamiliar_words == 0)
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 1, 3))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 4, 7))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 8, 12))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 13, 18))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 19, 24))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 25, 30))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 31, 37))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 38, 43))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 10:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 1))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 2, 4))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 5, 8))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 9, 13))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 14, 19))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 20, 25))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 26, 31))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 32, 37))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 38, 44))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 11:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 1))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 2, 4))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 5, 9))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 10, 14))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 15, 19))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 20, 26))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 27, 32))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 33, 38))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 39, 44))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 12:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 2))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 3, 5))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 6, 9))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 10, 14))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 15, 20))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 21, 26))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 27, 32))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 33, 39))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 40, 44))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 13:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 2))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 3, 5))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 6, 10))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 11, 15))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 16, 20))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 21, 27))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 28, 33))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 34, 39))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 40, 45))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 14:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 3))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 4, 6))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 7, 10))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 11, 15))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 16, 21))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 22, 27))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 28, 33))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 34, 40))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 41, 46))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 15:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 3))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 4, 6))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 7, 10))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 11, 16))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 17, 21))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 22, 27))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 28, 34))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 35, 40))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 41, 46))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 16:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 3))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 4, 7))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 8, 11))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 12, 16))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 17, 21))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 22, 28))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 29, 34))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 35, 40))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 41, 47))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 17:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 4))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 5, 7))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 8, 11))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 12, 16))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 17, 22))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 23, 28))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 29, 34))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 35, 40))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 41, 47))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 18:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 4))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 5, 7))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 8, 11))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 12, 17))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 18, 22))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 23, 28))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 29, 34))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 35, 41))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 42, 47))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 19:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 4))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 5, 7))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 8, 11))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 12, 17))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 18, 22))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 23, 28))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 29, 35))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 36, 41))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 42, 47))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 20:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 4))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 5, 7))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 8, 12))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 13, 17))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 18, 22))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 23, 28))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 29, 35))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 36, 41))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 42, 47))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 21:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 4))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 5, 8))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 9, 12))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 13, 17))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 18, 22))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 23, 29))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 30, 35))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 36, 41))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 42, 48))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 22:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 5))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 6, 8))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 9, 12))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 13, 17))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 18, 23))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 24, 29))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 30, 35))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 36, 41))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 42, 48))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 23:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 5))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 6, 8))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 9, 12))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 13, 17))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 18, 23))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 24, 29))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 30, 35))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 36, 42))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 43, 48))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 24:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 5))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 6, 8))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 9, 12))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 13, 18))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 19, 23))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 24, 29))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 30, 35))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 36, 42))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 43, 48))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 25:
            [[fallthrough]];
        case 26:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 5))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 6, 8))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 9, 12))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 13, 18))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 19, 23))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 24, 29))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 30, 36))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 37, 42))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 43, 48))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 27:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 5))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 6, 8))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 9, 13))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 14, 18))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 19, 23))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 24, 29))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 30, 36))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 37, 42))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 43, 48))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 28:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 5))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 6, 8))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 9, 13))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 14, 18))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 19, 23))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 24, 30))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 31, 36))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 37, 42))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 43, 48))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 29:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 5))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 6, 9))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 10, 13))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 14, 18))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 19, 23))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 24, 30))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 31, 36))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 37, 42))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 43, 49))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 30:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 5))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 6, 9))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 10, 13))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 14, 18))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 19, 23))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 24, 30))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 31, 36))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 37, 42))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 43, 49))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 31:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 6))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 7, 9))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 10, 13))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 14, 18))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 19, 23))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 24, 30))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 31, 36))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 37, 42))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 43, 49))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 32:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 6))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 7, 9))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 10, 13))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 14, 18))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 19, 24))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 25, 30))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 31, 36))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 37, 42))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 43, 49))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 33:
            [[fallthrough]];
        case 34:
            [[fallthrough]];
        case 35:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 6))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 7, 9))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 10, 13))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 14, 18))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 19, 24))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 25, 30))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 31, 36))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 37, 43))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 44, 49))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 36:
            [[fallthrough]];
        case 37:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 6))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 7, 9))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 10, 13))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 14, 19))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 20, 24))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 25, 30))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 31, 36))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 37, 43))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 44, 49))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 38:
            [[fallthrough]];
        case 39:
            [[fallthrough]];
        case 40:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 6))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 7, 9))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 10, 13))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 14, 19))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 20, 24))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 25, 30))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 31, 37))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 38, 43))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 44, 49))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 41:
            [[fallthrough]];
        case 42:
            [[fallthrough]];
        case 43:
            [[fallthrough]];
        case 44:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 6))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 7, 9))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 10, 14))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 15, 19))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 20, 24))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 25, 30))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 31, 37))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 38, 43))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 44, 49))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 45:
            [[fallthrough]];
        case 46:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 6))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 7, 9))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 10, 14))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 15, 19))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 20, 24))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 25, 31))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 32, 37))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 38, 43))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 44, 49))
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            else
                {
                grade_range_begin = grade_range_end = 16;
                }
            break;
        case 47:
            [[fallthrough]];
        case 48:
            [[fallthrough]];
        case 49:
            [[fallthrough]];
        case 50:
            [[fallthrough]];
        default:
            if (is_within<size_t>(number_of_unfamiliar_words, 0, 6))
                {
                grade_range_begin = grade_range_end = 1;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 7, 10))
                {
                grade_range_begin = grade_range_end = 2;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 11, 14))
                {
                grade_range_begin = grade_range_end = 3;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 15, 19))
                {
                grade_range_begin = grade_range_end = 4;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 20, 24))
                {
                grade_range_begin = 5;
                grade_range_end = 6;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 25, 31))
                {
                grade_range_begin = 7;
                grade_range_end = 8;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 32, 37))
                {
                grade_range_begin = 9;
                grade_range_end = 10;
                }
            else if (is_within<size_t>(number_of_unfamiliar_words, 38, 43))
                {
                grade_range_begin = 11;
                grade_range_end = 12;
                }
            else
                {
                grade_range_begin = 13;
                grade_range_end = 15;
                }
            break;
            };
        }

    /** @brief Spache (Revised, 1974) test. Designed for primary-age materials
            (based on 2nd grade material).
        @param number_of_words The number of words in the sample.
        @param number_of_unfamiliar_words The number of words not on the Spache familiar word list.
        @param number_of_sentences The number of sentences.
        @returns The calculated grade level.
        @note Spache recommends not counting the same unfamiliar word twice if they occur
            in subsequent 100-word samples. Based on this, it's recommended to pass in a unique
            count of unfamiliar words, instead of a raw count.*/
    [[nodiscard]]
    inline double spache(const uint32_t number_of_words, const uint32_t number_of_unfamiliar_words,
                         const uint32_t number_of_sentences)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("Not enough sentences in formula.");
            }
        const auto sampleSizeNormalizationFactor = safe_divide<double>(100, number_of_words);

        const auto ASL = safe_divide<double>(number_of_words, number_of_sentences);
        const double gradeLevel =
            (.121 * ASL) + (.082 * (number_of_unfamiliar_words * sampleSizeNormalizationFactor)) +
            .659;
        return truncate_k12_plus_grade(gradeLevel);
        }

    /** @brief U.S. grade level and predicted close score test,
            meant for secondary ages (4th grade) to college level.
        @details The Coleman-Liau formula is based on text ranging from .4 to 16.3.
            This formula usually yields the lowest grade when applied to technical documents.
            This test is historically applied to 100 word samplings.
        @param number_of_words The number of words in the sample.
        @param number_of_letters The number of letters.
        @param number_of_sentences The number of sentences.
        @param[out] predicted_cloze_score The calculated predicted cloze score
            (in fractional format, multiply by 100 to get the cloze %).
        @returns The calculated grade level.*/
    [[nodiscard]]
    inline double coleman_liau(const size_t number_of_words, const size_t number_of_letters,
                               const size_t number_of_sentences, double& predicted_cloze_score)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word/sentence count.");
            }

        const double sampleSizeCoefficient =
            COLEMAN_LIAU_WORD_COEFFICIENT / static_cast<double>(number_of_words);

        const double normalizedLettersVal = number_of_letters * sampleSizeCoefficient;
        const double normalizedSentenceVal = number_of_sentences * sampleSizeCoefficient;

        predicted_cloze_score =
            (141.8401 - (.214590 * normalizedLettersVal)) + (1.079812 * normalizedSentenceVal);
        // convert to fractional percentage
        predicted_cloze_score =
            std::clamp<double>(safe_divide<double>(predicted_cloze_score, 100), -1.0, 1.0);
        const double gradeLevel = (-27.4004 * predicted_cloze_score) + 23.06395;

        return truncate_k12_plus_grade(gradeLevel);
        }

    /** @brief Bormuth Grade Placement (35) Machine Computation Formula for
            Estimating Passage Readability.
        @details This formula uses the test passages that had ~35% cloze scores
            as its training data.
        @param number_of_words The number of words in the sample.
        @param number_of_familiar_dale_chall_words The number of familiar DC words.
        @param number_of_sentences The number of sentences.
        @param number_of_characters The number of characters.
        @returns The grade level (or placement) of a passage.*/
    [[nodiscard]]
    inline double bormuth_grade_placement_35(const uint32_t number_of_words,
                                             const uint32_t number_of_familiar_dale_chall_words,
                                             const uint32_t number_of_characters,
                                             const uint32_t number_of_sentences)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word or sentence count");
            }
        return truncate_k12_plus_grade(
            3.761864 + (1.053153 * (safe_divide<double>(number_of_characters, number_of_words))) -
            2.138595 *
                std::pow(safe_divide<double>(number_of_familiar_dale_chall_words, number_of_words),
                         3) +
            .152832 * (safe_divide<double>(number_of_words, number_of_sentences)) -
            .002077 * std::pow(safe_divide<double>(number_of_words, number_of_sentences), 2));
        }

    /** @brief Bormuth Cloze Mean Machine Computation Formula for Estimating Passage Readability.
        @param number_of_words The number of words in the sample.
        @param number_of_familiar_dale_chall_words The number of familiar DC words.
        @param number_of_characters The number of characters.
        @param number_of_sentences The number of sentences.
        @returns The (fractional percentage) estimated Cloze score of a passage.*/
    [[nodiscard]]
    inline double bormuth_cloze_mean(const uint32_t number_of_words,
                                     const uint32_t number_of_familiar_dale_chall_words,
                                     const uint32_t number_of_characters,
                                     const uint32_t number_of_sentences)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word or sentence count");
            }
        return std::clamp<double>(
            .886593 - .083640 * (safe_divide<double>(number_of_characters, number_of_words)) +
                .161911 * std::pow(safe_divide<double>(number_of_familiar_dale_chall_words,
                                                       number_of_words),
                                   3) -
                .021401 * (safe_divide<double>(number_of_words, number_of_sentences)) +
                .000577 * std::pow(safe_divide<double>(number_of_words, number_of_sentences), 2) -
                .000005 * std::pow(safe_divide<double>(number_of_words, number_of_sentences), 3),
            -1.0, 1.0);
        }

    /** Degrees of Reading Power, which is just an inverted Bormuth Cloze score.
        @param number_of_words The number of words in the sample.
        @param number_of_familiar_dale_chall_words The number of familiar DC words.
        @param number_of_characters The number of characters.
        @param number_of_sentences The number of sentences.
        @returns The difficulty level (0 = easy, 100 = very difficult).*/
    [[nodiscard]]
    inline double degrees_of_reading_power(const uint32_t number_of_words,
                                           const uint32_t number_of_familiar_dale_chall_words,
                                           const uint32_t number_of_characters,
                                           const uint32_t number_of_sentences)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word or sentence count");
            }
        const double clozeScore =
            bormuth_cloze_mean(number_of_words, number_of_familiar_dale_chall_words,
                               number_of_characters, number_of_sentences) *
            100;
        return std::clamp<double>(100 - clozeScore, 0, 100);
        }

    /** @brief Helper function for converting DRP to grade (Carver)
        @param dprScore The degrees of reading power score.
        @returns The grade level.*/
    [[nodiscard]]
    inline constexpr double degrees_of_reading_power_to_ge(const size_t dprScore) noexcept
        {
        double gradeLevel = 0;
        if (dprScore <= 39)
            {
            gradeLevel = 1.5;
            }
        else if (dprScore == 40)
            {
            gradeLevel = 1.6;
            }
        else if (dprScore == 41)
            {
            gradeLevel = 1.7;
            }
        else if (dprScore == 42)
            {
            gradeLevel = 1.7;
            }
        else if (dprScore == 43)
            {
            gradeLevel = 1.8;
            }
        else if (dprScore == 44)
            {
            gradeLevel = 2.0;
            }
        else if (dprScore == 45)
            {
            gradeLevel = 2.1;
            }
        else if (dprScore == 46)
            {
            gradeLevel = 2.3;
            }
        else if (dprScore == 47)
            {
            gradeLevel = 2.5;
            }
        else if (dprScore == 48)
            {
            gradeLevel = 2.8;
            }
        else if (dprScore == 49)
            {
            gradeLevel = 3.0;
            }
        else if (dprScore == 50)
            {
            gradeLevel = 3.3;
            }
        else if (dprScore == 51)
            {
            gradeLevel = 3.6;
            }
        else if (dprScore == 52)
            {
            gradeLevel = 3.9;
            }
        else if (dprScore == 53)
            {
            gradeLevel = 4.3;
            }
        else if (dprScore == 54)
            {
            gradeLevel = 4.7;
            }
        else if (dprScore == 55)
            {
            gradeLevel = 5.1;
            }
        else if (dprScore == 56)
            {
            gradeLevel = 5.5;
            }
        else if (dprScore == 57)
            {
            gradeLevel = 5.9;
            }
        else if (dprScore == 58)
            {
            gradeLevel = 6.3;
            }
        else if (dprScore == 59)
            {
            gradeLevel = 6.8;
            }
        else if (dprScore == 60)
            {
            gradeLevel = 7.3;
            }
        else if (dprScore == 61)
            {
            gradeLevel = 7.8;
            }
        else if (dprScore == 62)
            {
            gradeLevel = 8.3;
            }
        else if (dprScore == 63)
            {
            gradeLevel = 8.8;
            }
        else if (dprScore == 64)
            {
            gradeLevel = 9.4;
            }
        else if (dprScore == 65)
            {
            gradeLevel = 10.0;
            }
        else if (dprScore == 66)
            {
            gradeLevel = 10.6;
            }
        else if (dprScore == 67)
            {
            gradeLevel = 11.2;
            }
        else if (dprScore == 68)
            {
            gradeLevel = 11.8;
            }
        else if (dprScore == 69)
            {
            gradeLevel = 12.5;
            }
        else if (dprScore == 70)
            {
            gradeLevel = 13.1;
            }
        else if (dprScore == 71)
            {
            gradeLevel = 13.7;
            }
        else if (dprScore == 72)
            {
            gradeLevel = 14.4;
            }
        else if (dprScore == 73)
            {
            gradeLevel = 15.0;
            }
        else if (dprScore == 74)
            {
            gradeLevel = 15.7;
            }
        else if (dprScore == 75)
            {
            gradeLevel = 16.4;
            }
        else if (dprScore == 76)
            {
            gradeLevel = 17.1;
            }
        else if (dprScore == 77)
            {
            gradeLevel = 17.9;
            }
        else if (dprScore >= 78)
            {
            gradeLevel = 18.0;
            }
        return gradeLevel;
        }

    /** @brief Degrees of Reading Power GE, Carver's grade level conversion
            for DPR difficulty scores.
        @param number_of_words The number of words in the sample.
        @param number_of_familiar_dale_chall_words The number of familiar DC words.
        @param number_of_characters The number of characters.
        @param number_of_sentences The number of sentences
        @returns The grade-level score.*/
    [[nodiscard]]
    inline double degrees_of_reading_power_ge(const uint32_t number_of_words,
                                              const uint32_t number_of_familiar_dale_chall_words,
                                              const uint32_t number_of_characters,
                                              const uint32_t number_of_sentences)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word or sentence count");
            }
        const auto dprScore = static_cast<size_t>(round_to_integer(
            degrees_of_reading_power(number_of_words, number_of_familiar_dale_chall_words,
                                     number_of_characters, number_of_sentences)));
        return std::clamp<double>(degrees_of_reading_power_to_ge(dprScore), 0, 18);
        }

    /** SMOG modified to use a lower comprehension rate and to work better with primary-age
            materials. This test differs from the original SMOG:
            * It uses the UNIQUE count of hard words
                ("one should count a polysyllabic word only once").
            * It uses a smaller constant, and that constant is only added to higher scores.
            * The score is rounded, rather than floored.
        The test's article doesn't specify how to handle numbers, so it is best to do what
        the original test does (fully syllabize them).
        @param number_of_big_words Number of 3+ syllable words from the sample.
        @param number_of_sentences Number of sentences in the sample.
        @returns U.S. K-12 grade scale value of the document.*/
    [[nodiscard]]
    inline double modified_smog(const uint32_t number_of_big_words,
                                const uint32_t number_of_sentences)
        {
        if (number_of_sentences == 0)
            {
            throw std::domain_error("invalid sentence count.");
            }
        const auto sentenceNormalizationFactor = safe_divide<double>(30, number_of_sentences);
        const double score = std::clamp<double>(
            round_to_integer(std::sqrt(number_of_big_words * sentenceNormalizationFactor)), 1, 13);
        return (score <= 3) ? score : score + 1;
        }

    /** Simple Measure of Gobbledygook (McLaughlin), which returns the U.S. grade level.
        It is generally appropriate for secondary age readers, and commonly used with patient forms
        in the health care industry.
        Historically, this test is done on 3 random samples of 10 sentences,
        but can be used against the entire body of a corpus.
        SMOG tests for 100% comprehension, whereas most other formulae test for around
        50%-75% comprehension.
        Big words are words consisting of 3 or more syllables.
        @param number_of_big_words Number of 3+ syllable words from the sample.
        @param number_of_sentences Number of sentences in the sample.
        @return U.S. K-12 grade scale value of the document.*/
    [[nodiscard]]
    inline double smog_simplified(const uint32_t number_of_big_words,
                                  const uint32_t number_of_sentences)
        {
        if (number_of_sentences == 0)
            {
            throw std::domain_error("invalid sentence count.");
            }
        const auto sentenceNormalizationFactor = safe_divide<double>(30, number_of_sentences);
        return truncate_k12_plus_grade(
            std::floor(std::sqrt(number_of_big_words * sentenceNormalizationFactor)) + 3.0);
        }

    /** @brief More precise variation of SMOG, which generally returns slightly higher
            (within one grade level) scores.
        @param number_of_big_words Number of 3+ syllable words from the sample.
        @param number_of_sentences Number of sentences in the sample.
        @param truncate_score_to_range Whether score should be boxed into
            the 0-19 range. The SOL formula can calculate high (e.g., 25) scores
            with Spanish (that have to be converted to the English scale),
            so pass in false for situations like that.
        @return U.S. K-12 grade scale value of the document.*/
    [[nodiscard]]
    inline double smog(const uint32_t number_of_big_words, const uint32_t number_of_sentences,
                       const bool truncate_score_to_range = true)
        {
        if (number_of_sentences == 0)
            {
            throw std::domain_error("invalid sentence count.");
            }
        const auto sentenceNormalizationFactor = safe_divide<double>(30, number_of_sentences);
        const double score =
            (1.0430 * std::sqrt(number_of_big_words * sentenceNormalizationFactor)) + 3.1291;
        return truncate_score_to_range ? truncate_k12_plus_grade(score) : score;
        }

    /** @brief Harris-Jacobson Wide Range Readability Formula.
        This test may be used for materials ranging from 1st through
            eighth-reader levels (although is extrapolated to 11th grade).
        @param number_of_words Total number of words from the sample. Note that numbers and words
            from tables and lists are excluded. Words from headers and subheaders are included,
            which differs from the recommendation for other tests.
        @param number_of_hard_words Number of difficult words, which are words not found on the
            HJ familiar word list. Proper nouns are considered familiar also.
            Numbers are simply ignored (neither familiar nor unfamiliar) because they are pulled
            from the overall word count.
        @param number_of_sentences Number of sentences from the document
            (excluding tables and lists).
            Note that headers and subheaders are treated as separate sentences.
        @returns U.S. K-12 grade scale value of the document.
        @note The grade range for this test is between 1 - 13.3.*/
    [[nodiscard]]
    inline double harris_jacobson(const uint32_t number_of_words,
                                  const uint32_t number_of_hard_words,
                                  const uint32_t number_of_sentences)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word or sentence count");
            }
        // step 1
        double V1 = safe_divide<double>(number_of_hard_words, number_of_words) * 100;
        // step 2
        auto V2 = safe_divide<double>(number_of_words, number_of_sentences);
        // step 3
        V1 *= .245;
        // step 4
        V2 *= .160;
        // step 5
        double predictedRawScore = V1 + V2 + .642;
        // step 6
        predictedRawScore =
            std::clamp<double>(round_decimal_place(predictedRawScore, 10), 1.1, 8.0);
        // step 7
        predictedRawScore *= 10; // just so we can switch on this value
        switch (static_cast<int>(predictedRawScore))
            {
        case 11:
            [[fallthrough]];
        case 12:
            [[fallthrough]];
        case 13:
            return 1.0;
        case 14:
            return 1.1;
        case 15:
            return 1.2;
        case 16:
            return 1.3;
        case 17:
            return 1.4;
        case 18:
            return 1.5;
        case 19:
            return 1.7;
        case 20:
            return 1.8;
        case 21:
            return 1.9;
        case 22:
            return 2.0;
        case 23:
            return 2.1;
        case 24:
            return 2.2;
        case 25:
            return 2.3;
        case 26:
            return 2.4;
        case 27:
            return 2.6;
        case 28:
            return 2.7;
        case 29:
            return 2.8;
        case 30:
            return 2.9;
        case 31:
            return 3.1;
        case 32:
            return 3.2;
        case 33:
            return 3.3;
        case 34:
            return 3.4;
        case 35:
            return 3.5;
        case 36:
            return 3.6;
        case 37:
            return 3.7;
        case 38:
            return 3.8;
        case 39:
            return 3.9;
        case 40:
            return 4.0;
        case 41:
            return 4.1;
        case 42:
            return 4.3;
        case 43:
            return 4.5;
        case 44:
            return 4.6;
        case 45:
            return 4.7;
        case 46:
            return 4.8;
        case 47:
            return 5.0;
        case 48:
            return 5.2;
        case 49:
            return 5.4;
        case 50:
            return 5.5;
        case 51:
            return 5.7;
        case 52:
            return 5.9;
        case 53:
            return 6.0;
        case 54:
            return 6.2;
        case 55:
            return 6.4;
        case 56:
            return 6.5;
        case 57:
            return 6.7;
        case 58:
            return 6.9;
        case 59:
            return 7.1;
        case 60:
            return 7.3;
        case 61:
            return 7.5;
        case 62:
            return 7.7;
        case 63:
            return 7.9;
        case 64:
            return 8.1;
        case 65:
            return 8.3;
        case 66:
            return 8.5;
        // (from book) "readability scores above 8.5 have been derived from extrapolation"
        case 67:
            return 8.7;
        case 68:
            return 8.9;
        case 69:
            return 9.1;
        case 70:
            return 9.2;
        case 71:
            return 9.4;
        case 72:
            return 9.6;
        case 73:
            return 9.8;
        case 74:
            return 10.1;
        case 75:
            return 10.3;
        case 76:
            return 10.5;
        case 77:
            return 10.7;
        case 78:
            return 10.9;
        case 79:
            return 11.1;
        case 80:
            [[fallthrough]];
        default:
            return 11.3;
            }
        }

    /** FORCAST (J. Patrick FORd, John S. CAylor, and Thomas G. STicht) test,
        which is generally used for technical documents.
        This was devised for assessing US army technical manuals.
        This test is only appropriate for quizzes, applications, entry forms, etc.
        The results represent a U.S. grade level value.
        Algorithm was originally designed for passages of 150 words, so the
        number of monosyllabic words has to be normalized internally.
        @param number_of_words The number of words.
        @param number_of_single_syllable_words The number of mono-syllabic words.
        @returns The calculated grade level.*/
    [[nodiscard]]
    inline double forcast(const uint32_t number_of_words,
                          const uint32_t number_of_single_syllable_words)
        {
        if (number_of_words == 0)
            {
            throw std::domain_error("invalid word count.");
            }
        const auto normalizationFactor =
            safe_divide<double>(FORCAST_WORD_COEFFICIENT, number_of_words);
        return truncate_k12_plus_grade((
            20 - safe_divide<double>((number_of_single_syllable_words * normalizationFactor), 10)));
        }

    /** @brief Helper function to determine if a words is "easy" according to Gunning Fog.
        @param fogWord The word to review.
        @param syllableCount The number of syllables in the word.
        @returns @c true if words is easy.*/
    [[nodiscard]]
    inline bool is_easy_gunning_fog_word(const std::wstring_view fogWord,
                                         const size_t syllableCount)
        {
        if (fogWord.empty())
            {
            return false;
            }
        if (syllableCount >= 3)
            {
            const std::wstring_view separators = common_lang_constants::COMPOUND_WORD_SEPARATORS;

            const std::size_t dashPos = fogWord.find_first_of(separators);

            if (dashPos != std::wstring_view::npos)
                {
                static grammar::english_syllabize syllabize;

                std::wstring_view remaining{ fogWord };

                while (true)
                    {
                    const std::size_t pos = remaining.find_first_of(separators);

                    if (pos == std::wstring_view::npos)
                        {
                        // final segment after last separator
                        return syllabize(remaining.data(), remaining.size()) < 3;
                        }

                    // segment before separator
                    if (syllabize(remaining.data(), pos) >= 3)
                        {
                        return false;
                        }

                    // skip the separator and continue
                    remaining.remove_prefix(pos + 1);
                    }
                }

            if (syllableCount == 3 && (fogWord.ends_with(L"ded") || fogWord.ends_with(L"ted") ||
                                       fogWord.ends_with(L"shes") || fogWord.ends_with(L"ces") ||
                                       fogWord.ends_with(L"ges") || fogWord.ends_with(L"xes") ||
                                       fogWord.ends_with(L"zes") || fogWord.ends_with(L"ses")))
                {
                return true;
                }

            return false;
            }
        // syllable count < 3
        return true;
        }

    /** @brief U.S. grade level test, designed for business publications.
            (hard words are defined as having 3 or more syllables).
        @code
        GF = .4*(W/S+((SYW/W)*100))
        @endcode
        @param number_of_words The number of words.
        @param number_of_hard_words The number of hard words.
        @param number_of_sentences The number of sentences.
        @returns The calculated grade level.*/
    [[nodiscard]]
    inline double gunning_fog(const uint32_t number_of_words, const uint32_t number_of_hard_words,
                              const uint32_t number_of_sentences)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word or sentence count.");
            }
        return truncate_k12_plus_grade(
            (safe_divide<double>(number_of_words, number_of_sentences) +
             /* '*100' is to convert to percentage value*/
             (safe_divide<double>(number_of_hard_words, number_of_words) * 100)) *
            .4);
        }

    /** @brief PSK variation of Fog.
        @param number_of_words The number of words in the sample.
        @param number_of_hard_words The number of hard words in the sample.
        @param number_of_sentences The number of sentences in the sample.
        @returns The calculated grade-level scores.*/
    [[nodiscard]]
    inline double psk_gunning_fog(const uint32_t number_of_words,
                                  const uint32_t number_of_hard_words,
                                  const uint32_t number_of_sentences)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word or sentence count.");
            }
        const double hardWordPercentage =
            safe_divide<double>(number_of_hard_words, number_of_words) * 100;
        const auto ASL = safe_divide<double>(number_of_words, number_of_sentences);
        const double result = 3.0680 + .0877 * ASL + .0984 * hardWordPercentage;
        return truncate_k12_plus_grade(result);
        }

    /** @brief U.S. grade level test, designed for the Navy's technical documents.
        @details Part of the Navy Readability Indices.
            (hard words are defined as having 3 or more syllables).
        @param number_of_words The number of words in the sample.
        @param number_of_hard_words The number of hard words in the sample.
        @param number_of_sentences The number of sentences in the sample.
        @returns The calculated grade-level scores.*/
    [[nodiscard]]
    inline double new_fog_count(const uint32_t number_of_words, const uint32_t number_of_hard_words,
                                const uint32_t number_of_sentences)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word count.");
            }
        // (((easy words + (3 * hard words)) / sentences ) - 3) / 2
        const size_t numberOfEasyWords = number_of_words - number_of_hard_words;
        return truncate_k12_plus_grade(safe_divide<double>(
            safe_divide<double>(static_cast<double>(numberOfEasyWords) + (3 * number_of_hard_words),
                                number_of_sentences) -
                3,
            2));
        }

    /** Ease of readability, best meant for school text.
        Ranges from 0-100 (the higher the score, the easier to read).
        Average document should be within the range of 60-70.
        @code
        I = 206.835 - (84.6*(SY/W)) - (1.015*(W/S))
        @endcode
        @param number_of_words The number of words in the sample.
        @param number_of_syllables The number of syllables in the sample.
        @param number_of_sentences The number of sentences in the sample.
        @param[out] difficulty_level The resulting difficulty level.
        @returns The Flesch Reading Ease index value (0-100).*/
    [[nodiscard]]
    inline size_t
    flesch_reading_ease(const uint32_t number_of_words, const uint32_t number_of_syllables,
                        const uint32_t number_of_sentences, flesch_difficulty& difficulty_level)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word/sentence count.");
            }
        const size_t result = static_cast<size_t>(round_to_integer(std::clamp<double>(
            206.835 - (1.015 * (safe_divide<double>(number_of_words, number_of_sentences))) -
                (84.6 * (safe_divide<double>(number_of_syllables, number_of_words))),
            0, 100)));

        difficulty_level = flesch_score_to_difficulty_level(result);

        return result;
        }

    /** @brief Farr-Jenkins-Paterson test, which is just a simplified version of
            Flesch Reading Ease.
        @param number_of_words The number of words in the sample.
        @param number_of_monosyllabic_words The number of 1-syllable words in the sample.
        @param number_of_sentences The number of sentences in the sample.
        @param[out] difficulty_level The Flesch difficulty level.
        @returns A Flesch Reading Ease index score.*/
    [[nodiscard]]
    inline size_t farr_jenkins_paterson(const uint32_t number_of_words,
                                        const uint32_t number_of_monosyllabic_words,
                                        const uint32_t number_of_sentences,
                                        flesch_difficulty& difficulty_level)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word or sentence count.");
            }

        const double monoSyllabicWordPercentage =
            safe_divide<double>(number_of_monosyllabic_words, number_of_words) * 100;
        const auto ASL = safe_divide<double>(number_of_words, number_of_sentences);

        const auto result = static_cast<size_t>(std::clamp<double>(
            round_to_integer(-31.517 - (1.015 * ASL) + (1.599 * monoSyllabicWordPercentage)), 0,
            100));
        difficulty_level = flesch_score_to_difficulty_level(result);

        return result;
        }

    /** @brief Kincaid variation of FJP, designed for Naval documents.
        @param number_of_words The number of words in the sample.
        @param number_of_monosyllabic_words The number of 1-syllable words in the sample.
        @param number_of_sentences The number of sentences in the sample.
        @returns The calculated grade-level score.*/
    [[nodiscard]]
    inline double new_farr_jenkins_paterson(const uint32_t number_of_words,
                                            const uint32_t number_of_monosyllabic_words,
                                            const uint32_t number_of_sentences)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word or sentence count.");
            }
        const double monoSyllabicWordPercentage =
            safe_divide<double>(number_of_monosyllabic_words, number_of_words) * 100;
        const auto ASL = safe_divide<double>(number_of_words, number_of_sentences);

        return truncate_k12_plus_grade(22.05 + (.387 * ASL) - (.307 * monoSyllabicWordPercentage));
        }

    /** @brief PSK variation of FJP.
        @param number_of_words The number of words in the sample.
        @param number_of_monosyllabic_words The number of 1-syllable words in the sample.
        @param number_of_sentences The number of sentences in the sample.
        @returns The calculated grade-level score.*/
    [[nodiscard]]
    inline double
    powers_sumner_kearl_farr_jenkins_paterson(const uint32_t number_of_words,
                                              const uint32_t number_of_monosyllabic_words,
                                              const uint32_t number_of_sentences)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word or sentence count.");
            }
        const double monoSyllabicWordPercentage =
            safe_divide<double>(number_of_monosyllabic_words, number_of_words) * 100;
        const auto ASL = safe_divide<double>(number_of_words, number_of_sentences);

        return truncate_k12_plus_grade(8.4335 + (.0923 * ASL) -
                                       (.0648 * monoSyllabicWordPercentage));
        }

    /** @brief Wheeler-Smith is meant for primary-age reading materials and its scores range
            from Primer-4th grade.
        @note this test uses polysyllabic words (2+ syllables), rather than 3+
            like most other tests. This test also uses units instead of sentences.
        @param number_of_words The number of words in the sample.
        @param number_of_polysyllabic_words The number of 1-syllable words in the sample.
        @param number_of_units The number of units (i.e., sentences) in the sample.
        @param[out] index_score The calculated index score.
        @returns The calculated grade-level score.*/
    [[nodiscard]]
    inline size_t wheeler_smith(const uint32_t number_of_words,
                                const uint32_t number_of_polysyllabic_words,
                                const uint32_t number_of_units, double& index_score)
        {
        if (number_of_words == 0 || number_of_units == 0)
            {
            throw std::domain_error("invalid word or unit count.");
            }
        const auto AUL = safe_divide<double>(number_of_words, number_of_units);
        // should remain in fractal format
        const auto PSP = safe_divide<double>(number_of_polysyllabic_words, number_of_words);
        index_score = round_decimal_place(std::clamp(AUL * PSP * 10, 4.0, 34.5), 10);
        if (index_score <= 8.0)
            {
            return 0;
            }
        if (index_score > 8.0 && index_score <= 11.5)
            {
            return 1;
            }
        if (index_score > 11.5 && index_score <= 19)
            {
            return 2;
            }
        if (index_score > 19 && index_score <= 26.5)
            {
            return 3;
            }

        return 4;
        }

    /** U.S. grade level test, designed for technical documents (e.g., adult training manuals).
        The Kincaid formula is based on navy training manuals ranging from
        5.5 to 16.3 in grade level.
        The score reported by the formula tends to be in the mid-range of the four formulae.
        Because it is based on adult training manuals rather than schoolbook text,
        this formula is most applicable to technical documents.
        This is one of three tests in the Navy Readability Indices.
        @code
        GL = ((11.8*SY)/W) + ((.39*W)/S) -15.59
        @endcode
        @param number_of_words The number of words in the sample.
        @param number_of_syllables The number of syllables in the sample.
        @param number_of_sentences The number of sentences in the sample.
        @returns The calculated grade-level score.*/
    [[nodiscard]]
    inline double flesch_kincaid(const uint32_t number_of_words, const uint32_t number_of_syllables,
                                 const uint32_t number_of_sentences)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word/sentence count.");
            }
        return truncate_k12_plus_grade(
            (.39 * safe_divide<double>(number_of_words, number_of_sentences)) +
            (11.8 * safe_divide<double>(number_of_syllables, number_of_words)) - 15.59);
        }

    /** @brief Same as Flesch-Kincaid, but used integers (instead of floating-point numbers)
            in its equation.
        @param number_of_words The number of words in the sample.
        @param number_of_syllables The number of syllables in the sample.
        @param number_of_sentences The number of sentences in the sample.
        @returns The calculated grade-level score.*/
    [[nodiscard]]
    inline double flesch_kincaid_simplified(const uint32_t number_of_words,
                                            const uint32_t number_of_syllables,
                                            const uint32_t number_of_sentences)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word/sentence count.");
            }
        return truncate_k12_plus_grade(
            (.4 * safe_divide<double>(number_of_words, number_of_sentences)) +
            (12 * safe_divide<double>(number_of_syllables, number_of_words)) - 16);
        }

    /** @brief PSK version of Flesch test, which yields a grade level instead of an index.
        @param number_of_words The number of words in the sample.
        @param number_of_syllables The number of syllables.
        @param number_of_sentences The number of sentences.
        @returns The calculated grade level.*/
    [[nodiscard]]
    inline double powers_sumner_kearl_flesch(const uint32_t number_of_words,
                                             const uint32_t number_of_syllables,
                                             const uint32_t number_of_sentences)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word/sentence count.");
            }
        const auto normalizationFactor = safe_divide<double>(PSK_WORD_COEFFICIENT, number_of_words);
        const auto ASL = safe_divide<double>(number_of_words, number_of_sentences);
        const double psk =
            (ASL * .0778) + (normalizationFactor * number_of_syllables * .0455) - 2.2029;
        return truncate_k12_plus_grade(psk);
        }
    } // namespace readability

#endif // ENGLISH_READABILITY_H
