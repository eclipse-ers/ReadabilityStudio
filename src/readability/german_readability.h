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

#ifndef GERMAN_READABILITY_H
#define GERMAN_READABILITY_H

#include "readability.h"

namespace readability
    {
    /// @brief Difficulty levels defined for Bamberger/Vanecek's reinterpretation of Lix.
    enum class german_lix_difficulty
        {
        german_lix_very_easy,          /*!< Very easy.*/
        german_lix_children_and_youth, /*!< Children and YA readers.*/
        german_lix_easy,               /*!< Easy.*/
        german_lix_adult_fiction,      /*!< Adult fiction.*/
        german_lix_average,            /*!< Average.*/
        german_lix_nonfiction,         /*!< Non-fiction.*/
        german_lix_difficult,          /*!< Difficult.*/
        german_lix_technical,          /*!< Technical text.*/
        german_lix_very_difficult      /*!< Very difficult.*/
        };

    /** @brief Helper function to convert Lix index to German difficulty.
        @param index The calculated Lix index value to convert.
        @returns The German Lix difficulty level.*/
    [[nodiscard]]
    inline german_lix_difficulty german_lix_index_to_difficulty_level(const size_t index) noexcept
        {
        if (index < 29)
            {
            return german_lix_difficulty::german_lix_very_easy;
            }
        if (index >= 30 && index <= 34)
            {
            return german_lix_difficulty::german_lix_children_and_youth;
            }
        if (index >= 35 && index <= 39)
            {
            return german_lix_difficulty::german_lix_easy;
            }
        if (index >= 40 && index <= 44)
            {
            return german_lix_difficulty::german_lix_adult_fiction;
            }
        if (index >= 45 && index <= 49)
            {
            return german_lix_difficulty::german_lix_average;
            }
        if (index >= 50 && index <= 54)
            {
            return german_lix_difficulty::german_lix_nonfiction;
            }
        if (index >= 55 && index <= 59)
            {
            return german_lix_difficulty::german_lix_difficult;
            }
        if (index >= 60 && index <= 64)
            {
            return german_lix_difficulty::german_lix_technical;
            }

        return german_lix_difficulty::german_lix_very_difficult;
        }

    /** @brief Recalculation of Rix for specific subgenres of German text.
        @param number_of_words Total number of words from the document.
        @param number_of_long_words Number of @c 7 (or more) character words. Note that they use
            @c 7 (like Lix and Rix do), as stated in the book:
            (*"Woerter mit mehr als sechs buchstaben"*).
        @param number_of_sentence_units The number of sentence units
            (sentence ending with `.?!;:`).\n
            Not stated in the book, but that is what standard Rix uses
            (instead of traditional sentences).
        @returns An index value score.
            This value should be converted to a more meaningful grade level.
        @throws std::domain_error If either @c number_of_words or
            @c number_of_sentence_units is @c 0, throws an exception.*/
    [[nodiscard]]
    inline double rix_bamberger_vanecek(const uint32_t number_of_words,
                                        const uint32_t number_of_long_words,
                                        const uint32_t number_of_sentence_units)
        {
        if (number_of_words == 0 || number_of_sentence_units == 0)
            {
            throw std::domain_error("invalid word/sentence count.");
            }
        const double percentOfLongWords =
            safe_divide<double>(number_of_long_words, number_of_words) * 100;
        const double sentencesPer100Words =
            safe_divide<double>(100, number_of_words) * number_of_sentence_units;
        return safe_divide<double>(percentOfLongWords, sentencesPer100Words) * 10;
        }

    /** Converts a Rix index value (from the Bamberger/Vanecek variation, not the Anderson version)
            to a grade level. This assumes German fiction text.
        @param index The (German) Rix index value to convert.
        @returns The grade level.*/
    [[nodiscard]]
    inline size_t rix_index_to_german_fiction_grade_level(const double index) noexcept
        {
        if (index < 9)
            {
            return 1;
            }
        // between 9 - 13.5
        if (is_within<double>(index, 9, 13.5))
            {
            return 1;
            }
        // between 13.51 - 17
        if (index <= 17)
            {
            return 2;
            }
        // between 17.1 - 20.5
        if (index <= 20.5)
            {
            return 3;
            }
        // between 20.51 = 24
        if (index <= 24)
            {
            return 4;
            }
        // between 24.1 = 27.5
        if (index <= 27.5)
            {
            return 5;
            }
        // between 27.51 - 31
        if (index <= 31)
            {
            return 6;
            }
        // between 31.1 - 34.5
        if (index <= 34.5)
            {
            return 7;
            }
        // between 34.51 - 38
        if (index <= 38)
            {
            return 8;
            }
        // between 38.1 - 41.5
        if (index <= 41.5)
            {
            return 9;
            }
        // between 41.51 - 45
        if (index <= 45)
            {
            return 10;
            }
        // > 45

        return 11;
        }

    /** @brief Bamberger & Vanecek variation of Lix.
        @param[out] difficulty_level The (German) Lix difficulty level.
        @param number_of_words The number of words in the text.
        @param number_of_longwords The number of words that are @c 7 or more characters
            (minus punctuation, based on Anderson's recommendation for Rix
             [which seems logical for Lix as well]).
        @param number_of_sentences The number of sentences.\n
            Anderson states to use sentence units in Rix, but Björnsson never says
            explicitly explains how to parse sentences.
            Because using units can be quick a bit different from other tests,
            it's a better assumption to use regular sentences for this test.
        @returns An index value.
        @note Use lix_index_to_german_childrens_literature_grade_level() or
            lix_index_to_german_technical_literature_grade_level()
            to get the grade-level score.*/
    [[nodiscard]]
    inline size_t german_lix(german_lix_difficulty& difficulty_level, const size_t number_of_words,
                             const size_t number_of_longwords, const size_t number_of_sentences)
        {
        lix_difficulty notUsed{ lix_difficulty::lix_very_easy };
        size_t notUsed2{ 0 };
        // don't use Lix's difficulty explanation,
        // Bamberger & Vanecek use an adjusted explanation for German text
        const auto indexValue =
            lix(notUsed, notUsed2, number_of_words, number_of_longwords, number_of_sentences);
        difficulty_level = german_lix_index_to_difficulty_level(indexValue);

        return indexValue;
        }

    /** Converts a Rix index value (from the Bamberger/Vanecek variation, not the Anderson version)
            to a grade level. This assumes German nonfiction text.
        @param index The (German) Rix index value to convert.
        @returns The grade level.*/
    [[nodiscard]]
    inline size_t rix_index_to_german_nonfiction_grade_level(const double index) noexcept
        {
        if (index < 20)
            {
            return 4;
            }
        // between 20 - 26
        if (is_within<double>(index, 20, 26))
            {
            return 4;
            }
        // between 26.1 - 32
        if (index <= 32)
            {
            return 5;
            }
        // between 32.1 - 38
        if (index <= 38)
            {
            return 6;
            }
        // between 38.1 - 45
        if (index <= 45)
            {
            return 7;
            }
        // between  45.1 - 52
        if (index <= 52)
            {
            return 8;
            }
        // between 52.1 - 57
        // (typo in the book says "67," but that's clearly wrong)
        if (index <= 57)
            {
            return 9;
            }
        // between 57.1 - 66
        if (index <= 66)
            {
            return 10;
            }
        // between 66.1 - 75
        if (index <= 75)
            {
            return 11;
            }
        // between 75.1- 84
        if (index <= 84)
            {
            return 12;
            }
        // between 84.1 - 100
        if (index <= 100)
            {
            return 13;
            }
        // between > 100

        return 14;
        }

    /** Specialized grade level calculation for Lix index value for
            German children's literature (Bamberger and Vanecek.)
        @param index The Lix index value to convert.
        @returns The grade level.*/
    [[nodiscard]]
    inline size_t lix_index_to_german_childrens_literature_grade_level(const size_t index) noexcept
        {
        if (index < 24)
            {
            return 1;
            }
        if (is_within<size_t>(index, 24, 26))
            {
            return 2;
            }
        if (is_within<size_t>(index, 27, 29))
            {
            return 3;
            }
        if (is_within<size_t>(index, 30, 31))
            {
            return 4;
            }
        if (is_within<size_t>(index, 32, 33))
            {
            return 5;
            }
        if (is_within<size_t>(index, 34, 35))
            {
            return 6;
            }
        if (is_within<size_t>(index, 36, 37))
            {
            return 7;
            }

        return 8;
        }

    /** Specialized grade level calculation for Lix index value for
            German technical works (Bamberger and Vanecek).
        @param index The Lix index value to convert.
        @returns The grade level.*/
    [[nodiscard]]
    inline size_t lix_index_to_german_technical_literature_grade_level(const size_t index) noexcept
        {
        if (index < 31)
            {
            return 3;
            }
        if (is_within<size_t>(index, 31, 33))
            {
            return 4;
            }
        if (is_within<size_t>(index, 34, 37))
            {
            return 5;
            }
        if (is_within<size_t>(index, 38, 40))
            {
            return 6;
            }
        if (is_within<size_t>(index, 41, 43))
            {
            return 7;
            }
        if (is_within<size_t>(index, 44, 47))
            {
            return 8;
            }
        if (is_within<size_t>(index, 48, 50))
            {
            return 9;
            }
        if (is_within<size_t>(index, 51, 53))
            {
            return 10;
            }
        if (is_within<size_t>(index, 54, 56))
            {
            return 11;
            }
        if (is_within<size_t>(index, 57, 59))
            {
            return 12;
            }
        if (is_within<size_t>(index, 60, 63))
            {
            return 13;
            }
        if (is_within<size_t>(index, 64, 69))
            {
            return 14;
            }
        // grade level 15 is mentioned on page 187 (Bamberger and Vanecek), but not page 64.

        return 15;
        }

    /** 1. Neue Wiener Sachtextformel (1.nWS), used for evaluating German non fiction
            ("formal text").\n
            (best for 6-10 grade materials
             [*"Besonders guenstig fuer Texte zwischen der 6 und 10 Schulstufe"*]).\n
            Created by Richard Bamberger and Erich Vanecek.
        @param number_of_words Total number of words from the document.
        @param number_of_monosyllabic_words Number of one-syllable words.
        @param number_of_hard_words Number of @c 3 (or more) syllable words.
        @param number_of_long_words Number of @c 7 (or more) character words. Note that they use
            @c 7 (like Lix and Rix do [*"Woerter mit mehr als sechs buchstaben"*]).
        @param number_of_sentences Number of sentences.
        @returns The US grade level of the document.
        @throws std::domain_error If either @c number_of_words or
            @c number_of_sentence_units is @c 0, throws an exception.*/
    [[nodiscard]]
    inline double neue_wiener_sachtextformel_1(const uint32_t number_of_words,
                                               const uint32_t number_of_monosyllabic_words,
                                               const uint32_t number_of_hard_words,
                                               const uint32_t number_of_long_words,
                                               const uint32_t number_of_sentences)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word/sentence count.");
            }
        return std::clamp<double>(
            0.1935 * (safe_divide<double>(number_of_hard_words, number_of_words) * 100) +
                0.1672 * (safe_divide<double>(number_of_words, number_of_sentences)) +
                0.1297 * (safe_divide<double>(number_of_long_words, number_of_words) * 100) -
                0.0327 *
                    (safe_divide<double>(number_of_monosyllabic_words, number_of_words) * 100) -
                0.875,
            1, 19);
        }

    /** 2. Neue Wiener Sachtextformel (2.nWS), used for evaluating German non fiction
            ("formal text").\n
            (Best for very light materials up to 5th grade
            [*"Besonders guenstig fuer sehr leichte Sachtext bis zur 5 Schulstufe"*]).\n
            Created by Richard Bamberger and Erich Vanecek.
        @param number_of_words Total number of words from the document.
        @param number_of_hard_words Number of @c 3 (or more) syllable words.
        @param number_of_long_words Number of @c 7 (or more) character words. Note that they use
            @c 7 (like Lix and Rix do ["Woerter mit mehr als sechs buchstaben"]).
        @param number_of_sentences Number of sentences.
        @returns The US grade level of the document.
        @throws std::domain_error If either @c number_of_words or
            @c number_of_sentence_units is @c 0, throws an exception.*/
    [[nodiscard]]
    inline double neue_wiener_sachtextformel_2(const uint32_t number_of_words,
                                               const uint32_t number_of_hard_words,
                                               const uint32_t number_of_long_words,
                                               const uint32_t number_of_sentences)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word/sentence count.");
            }
        return std::clamp<double>(
            0.2007 * (safe_divide<double>(number_of_hard_words, number_of_words) * 100) +
                0.1682 * (safe_divide<double>(number_of_words, number_of_sentences)) +
                0.1373 * (safe_divide<double>(number_of_long_words, number_of_words) * 100) - 2.779,
            1, 19);
        }

    /** 3. Neue Wiener Sachtextformel (3.nWS), used for evaluating German non fiction
            ("formal text").\n
            (Best for very light materials up to 5th grade
            [*"Besonders guenstig fuer sehr leichte Sachtext bis zur 5 Schulstufe"*]).\n
            Created by Richard Bamberger and Erich Vanecek.
        @param number_of_words Total number of words from the document.
        @param number_of_hard_words Number of @c 3 (or more) syllable words.
        @param number_of_sentences Number of sentences.
        @returns The US grade level of the document.
        @throws std::domain_error If either @c number_of_words or
            @c number_of_sentence_units is @c 0, throws an exception.*/
    [[nodiscard]]
    inline double neue_wiener_sachtextformel_3(const uint32_t number_of_words,
                                               const uint32_t number_of_hard_words,
                                               const uint32_t number_of_sentences)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word/sentence count.");
            }
        return std::clamp<double>(
            0.2963 * (safe_divide<double>(number_of_hard_words, number_of_words) * 100) +
                0.1905 * (safe_divide<double>(number_of_words, number_of_sentences)) - 1.1144,
            1, 19);
        }

    /** @brief Helper function for German Wheeler-Smith.
        @param index_score The index_score to convert.
        @param range_start The start of the range.
        @param range_end the end of the range.
        @param grade_level The grade level.
        @param[out] score The calculated WS score.
        @returns @c true if score could be calculated from the provided index value and range.*/
    [[nodiscard]]
    inline bool wheeler_smith_get_score_from_range(const double index_score,
                                                   const double range_start, const double range_end,
                                                   const size_t grade_level, double& score) noexcept
        {
        if (index_score > range_start - 0.1 /*a bit of wiggle room for trailing precision*/ &&
            index_score <= range_end)
            {
            // Note that the month level is rounded here, not truncated like the index score.
            // Don't understand the inconsistency, but that is what they do in the
            // book judging from their numbers.
            score =
                grade_level +
                round_decimal_place(safe_divide<double>((index_score - range_start),
                                                        truncate_decimal_place(
                                                            (range_end + 0.1) - range_start, 100)),
                                    10);
            return true;
            }

        score = 0;
        return false;
        }

    /** @brief German adaptation of Wheeler-Smith.
        @note This test considers 3+ syllable words "polysyllabic,"
            unlike the English Wheeler-Smith.
            This is actually an error in the book, because they state that the English formula
            considers 3+ syllable words "polysyllabic," which is wrong; however, the instructions
            for the German variation should follow this logic, even though it is based on
            flawed information.\n
            The book doesn't specify how to count sentences, so the "unit" method described
            in the original Wheeler-Smith article should probably be used.\n
            This variation also extends to the 10th grade, which contradicts the
            "primary-age" audience that the English counterpart is only designed for.
        @param number_of_words The number of words in the sample.
        @param number_of_polysyllabic_words the number of 3+ syllable words.
        @param number_of_units The number of units (i.e., sentences).
        @param[out] index_score The calculated index score.
        @returns The calculated grade level (between 1 - 10.9).
        @throws std::domain_error If either @c number_of_words or
            @c number_of_sentence_units is @c 0, throws an exception.*/
    [[nodiscard]]
    inline double wheeler_smith_bamberger_vanecek(const uint32_t number_of_words,
                                                  const uint32_t number_of_polysyllabic_words,
                                                  const uint32_t number_of_units,
                                                  double& index_score)
        {
        if (number_of_words == 0 || number_of_units == 0)
            {
            throw std::domain_error("invalid word or unit count.");
            }
        const auto AUL = safe_divide<double>(number_of_words, number_of_units);
        // should remain in percentage format
        const double PSP = safe_divide<double>(number_of_polysyllabic_words, number_of_words) * 100;
        // note that the score is truncated, not rounded. The book doesn't say to do that, but
        // judging from the example numbers in the book that is what they do.
        index_score = truncate_decimal_place(
            std::clamp<double>(safe_divide<double>((AUL * PSP), 10.0), 2.5, 42.0), 10);
        double grade_score = 0;
        if (wheeler_smith_get_score_from_range(index_score, 2.5, 6, 1, grade_score))
            {
            return grade_score;
            }
        if (wheeler_smith_get_score_from_range(index_score, 6.1, 9, 2, grade_score))
            {
            return grade_score;
            }
        if (wheeler_smith_get_score_from_range(index_score, 9.1, 12, 3, grade_score))
            {
            return grade_score;
            }
        if (wheeler_smith_get_score_from_range(index_score, 12.1, 16, 4, grade_score))
            {
            return grade_score;
            }
        if (wheeler_smith_get_score_from_range(index_score, 16.1, 20, 5, grade_score))
            {
            return grade_score;
            }
        if (wheeler_smith_get_score_from_range(index_score, 20.1, 24, 6, grade_score))
            {
            return grade_score;
            }
        if (wheeler_smith_get_score_from_range(index_score, 24.1, 29, 7, grade_score))
            {
            return grade_score;
            }
        if (wheeler_smith_get_score_from_range(index_score, 29.1, 34, 8, grade_score))
            {
            return grade_score;
            }
        if (wheeler_smith_get_score_from_range(index_score, 34.1, 38, 9, grade_score))
            {
            return grade_score;
            }
        // the highest index score will add a full year to the score of 10, so truncate it
        // to 10.9 to keep the score with the test's 1-10 grade level specification
        if (wheeler_smith_get_score_from_range(index_score, 38.1, 42, 10, grade_score))
            {
            return std::clamp<double>(grade_score, 1, 10.9);
            }

        throw std::domain_error("error in WS range check.");
        }

    /** @brief German variation of SMOG.
        @details Grade = SQRT(MS) - 2 (based on 30 sentence sample).\n
            Hard words are words consisting of 3 or more syllables.
        @param number_of_hard_words The number of words in the sample.
        @param number_of_sentences The number of sentences. If not 30, then this is used
         to adjust @c number_of_hard_words to what it would be in a 30-sentence sample.
        @returns The grade-level score.
        @throws std::domain_error If @c number_of_sentence_units is @c 0,
                throws an exception.*/
    [[nodiscard]]
    inline double smog_bamberger_vanecek(const uint32_t number_of_hard_words,
                                         const uint32_t number_of_sentences)
        {
        if (number_of_sentences == 0)
            {
            throw std::domain_error("invalid sentence count.");
            }
        const auto sentenceNormalizationFactor = safe_divide<double>(30, number_of_sentences);
        return truncate_k12_plus_grade(truncate_decimal_place(
            std::sqrt(number_of_hard_words * sentenceNormalizationFactor) - 2.0, 10));
        }

    /** @brief Grade = SQRT((MS/S)*30) - 2 (based on 100-word sample)
        @details Hard words are words consisting of 3 or more syllables.
        @param number_of_words The number of words in the sample.
        @param number_of_hard_words The number of 3+ syllable words (Drei- und Mehrsilber).
        @param number_of_sentences The number of sentences.
        @returns The grade-level score.
        @throws std::domain_error If either @c number_of_words or @c number_of_sentence_units is @c
       0, throws an exception.*/
    [[nodiscard]]
    inline double quadratwurzelverfahren_bamberger_vanecek(const uint32_t number_of_words,
                                                           const uint32_t number_of_hard_words,
                                                           const uint32_t number_of_sentences)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word/sentence count.");
            }
        const auto sampleSizeNormalizationFactor = safe_divide<double>(100, number_of_words);
        const double MS = number_of_hard_words * sampleSizeNormalizationFactor;
        const double S100 = number_of_sentences * sampleSizeNormalizationFactor;
        return truncate_k12_plus_grade(
            truncate_decimal_place(std::sqrt(safe_divide<double>(MS, S100) * 30) - 2.0, 10));
        }

    /** @brief German variation of Flesch Reading Ease.
        @details Ranges from 0-100 (the higher the score, the easier to read).\n
            Average document should be within the range of 60-70.
        @note The dissertation mentions using a 100-word sample,
            but that doesn't affect the equation.
        @code
        I = 180 - (W/S) - (58.5*(SY/W))
        @endcode
        @param number_of_words The number of words in the sample.
        @param number_of_syllables The number of syllables.
        @param number_of_sentences The number of sentences.
        @param[out] difficulty_level The calculated difficult level.
        @returns The difficulty level as an index value.
        @throws std::domain_error If either @c number_of_words or
            @c number_of_sentence_units is @c 0, throws an exception.*/
    [[nodiscard]]
    inline size_t amstad(const uint32_t number_of_words, const uint32_t number_of_syllables,
                         const uint32_t number_of_sentences, flesch_difficulty& difficulty_level)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word/sentence count.");
            }
        const size_t result = static_cast<size_t>(round_to_integer(std::clamp<double>(
            180 - safe_divide<double>(number_of_words, number_of_sentences) -
                (58.5 * safe_divide<double>(number_of_syllables, number_of_words)),
            0, 100)));

        difficulty_level = flesch_score_to_difficulty_level(result);

        return result;
        }
    } // namespace readability

#endif // GERMAN_READABILITY_H
