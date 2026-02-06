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

#ifndef SPANISH_READABILITY_H
#define SPANISH_READABILITY_H

#include "../Wisteria-Dataviz/src/math/mathematics.h"
#include "english_readability.h"
#include "grade_scales.h"
#include <algorithm>
#include <stdexcept>

namespace readability
    {
    /** @brief Crawford Spanish grade level test.
        @details This test is meant for primary-age reading materials.
            There is also a Crawford graph, which basically just plots
            the score where the factors' values intersect.
        @param number_of_words The number of words.
        @param number_of_syllables The number of syllables.
        @param number_of_sentences The number of sentences.
        @returns The grade-level score.
        @throws std::domain_error If @c number_of_words is @c 0,
                throws an exception.*/
    [[nodiscard]]
    inline double crawford(const uint32_t number_of_words, const uint32_t number_of_syllables,
                           const uint32_t number_of_sentences)
        {
        if (number_of_words == 0)
            {
            throw std::domain_error("invalid word count");
            }
        const auto normalizationFactor = safe_divide<double>(100, number_of_words);

        const double result = (-.205 * (number_of_sentences * normalizationFactor)) +
                              (.049 * (number_of_syllables * normalizationFactor)) - 3.407;
        return truncate_k12_plus_grade(result);
        }

    /** @brief Spanish SMOG test.
        @details This test calculates a (high-precision) SMOG score from Spanish text
            and then adjusts to the appropriate grade level.\n
            This test explicitly excludes lists, but includes headers and footers
            (SMOG does not explicitly state this).\n
            Numbers should be sounded out, just like in English SMOG.
        @param number_of_big_words The number of 3+ syllable words.
        @param number_of_sentences The number of sentences.
        @returns The grade-level score.
        @note Number of sentences is not a part of the SMOG formulas because they
            expect 10-sentence samples. @c number_of_sentences is therefore used to
            standardize @c number_of_big_words to what it would be if it came
            from a 10-sentence sample.
        @throws std::domain_error If @c number_of_sentences is @c 0,
                throws an exception.*/
    [[nodiscard]]
    inline double sol_spanish(const uint32_t number_of_big_words,
                              const uint32_t number_of_sentences)
        {
        if (number_of_sentences == 0)
            {
            throw std::domain_error("invalid sentence count.");
            }
        // get the raw SMOG grade score (which can be high [e.g., 25]) then plug into the
        // Spanish -> English grade level formula (below)...
        const double solScore = -2.51 + .74 * smog(number_of_big_words, number_of_sentences, false);
        // ...then clip to the regular 0-19 grade level range
        return truncate_k12_plus_grade(solScore);
        }

    /** @brief Szigriszt-Pazos Perspicuity scale.
        @details This value is used for both the original Szigriszt-Pazos chart and the
            INFLESZ chart.
        @code
        INFLESZ = 206.835 - (62.3*(SY/W)) - (W/S)
        @endcode
        @param number_of_words The number of words in the sample.
        @param number_of_syllables The number of syllables in the sample.
        @param number_of_sentences The number of sentences in the sample.
        @note The original article uses the intercept of @c 207 for both this formula and
            the FRE formula. FRE uses @c 206.835, so it is assumed
            that @c 206.835 was intended here, but @c 207 was used for shorthand.
            Barrio-Cantalejo (et al.) drew the same conclusion and used 206.835 in their
            interpretation of this formula.
        @returns The Szigriszt-Pazos index value (0-100).*/
    [[nodiscard]]
    inline size_t szigriszt_pazos_perspicuity(const uint32_t number_of_words,
                                              const uint32_t number_of_syllables,
                                              const uint32_t number_of_sentences)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word/sentence count.");
            }
        const size_t result = static_cast<size_t>(round_to_integer(std::clamp<double>(
            206.835 - safe_divide<double>(number_of_words, number_of_sentences) -
                (62.3 * (safe_divide<double>(number_of_syllables, number_of_words))),
            0, 100)));

        return result;
        }

    /** @brief Helper function to convert a Szigriszt-Pazos Perspicuity score
            into a INFLESZ scale difficulty level.
        @param result The 0-100 calculated INFLESZ index.
        @returns The difficulty level of the index value, mapping to the INFLESZ column.
        @note The categorization of values in the article following the same contradictory
            logic of saying 0-40 is "very difficult," then 40-55 is "fairly difficult", etc.
            that the Flesch book does. However, it does eventually say that "> 80" is very easy.
            Because of this, we take the opposite approach of FRE, instead saying that 0-40
            is "very difficult," then 41-55 is "fairly difficult," etc.*/
    [[nodiscard]]
    inline inflesz_difficulty
    szigriszt_pazos_perspicuity_score_to_difficulty_inflesz_level(const size_t result) noexcept
        {
        if (result <= 40)
            {
            return inflesz_difficulty::very_difficult;
            }
        if (result <= 55)
            {
            return inflesz_difficulty::fairly_difficult;
            }
        if (result <= 65)
            {
            return inflesz_difficulty::normal;
            }
        if (result <= 80)
            {
            return inflesz_difficulty::fairly_easy;
            }
        // above 80
        return inflesz_difficulty::very_easy;
        }
    } // namespace readability

#endif // SPANISH_READABILITY_H
