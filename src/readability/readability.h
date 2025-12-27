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

#ifndef READABILITY_H
#define READABILITY_H

#include "../OleanderStemmingLibrary/src/stemming.h"
#include "../Wisteria-Dataviz/src/math/mathematics.h"
#include "../Wisteria-Dataviz/src/util/string_util.h"
#include "grade_scales.h"
#include "readability_enums.h"
#include <algorithm>
#include <cmath>
#include <functional>
#include <set>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace readability
    {
    /** @brief Performs a binary search through a list of familiar words to see if the provided
            value is in there.
        @details Also takes into account whether the word is proper or numeric,
            because those are sometimes familiar.

        @note This uses the stem of the value passed in (unless you specify a no-op stemmer),
            so be sure that the word list you pass in is pre-stemmed.*/
    template<typename word_typeT, typename wordlistT, typename stemmerT>
    class is_familiar_word
        {
      public:
        /** @brief Constructor.
            @param wlist The list of familiar words.
            @param properNounMethod The method to handle proper nouns.
            @param treat_numeric_as_familiar Whether to always treat numbers as familiar.*/
        is_familiar_word(const wordlistT* wlist,
                         const readability::proper_noun_counting_method properNounMethod,
                         const bool treat_numeric_as_familiar)
            : m_wordlist(wlist), m_treat_numeric_as_familiar(treat_numeric_as_familiar),
              m_properNounMethod(properNounMethod)
            {
            }

        /// @private
        is_familiar_word() = default;

        /** @brief Sets the familiar word list.
            @param wlist The familiar word list.*/
        void set_word_list(const wordlistT* wlist) noexcept { m_wordlist = wlist; }

        /** @returns The method being used for handling proper nouns.*/
        [[nodiscard]]
        readability::proper_noun_counting_method get_proper_noun_method() const noexcept
            {
            return m_properNounMethod;
            }

        /** @brief Sets the method to handle proper nouns.
            @param properMethod The proper noun method to use.*/
        void
        set_proper_noun_method(const readability::proper_noun_counting_method properMethod) noexcept
            {
            m_properNounMethod = properMethod;
            }

        /** @returns Whether numeric words should always be familiar*/
        [[nodiscard]]
        bool is_including_numeric_as_familiar() const noexcept
            {
            return m_treat_numeric_as_familiar;
            }

        /** @brief Sets whether numeric words should always be familiar.
            @param should_include Whether numeric words should always be familiar.*/
        void include_numeric_as_familiar(const bool should_include) noexcept
            {
            m_treat_numeric_as_familiar = should_include;
            }

        /** @brief Main interface for determining if a word is familiar.
            @param the_word The word to review.
            @returns Whether the word is familiar.*/
        [[nodiscard]]
        inline bool operator()(const word_typeT& the_word) const
            {
            // treat numeric words as familiar (if caller asked for that)
            if (m_treat_numeric_as_familiar && the_word.is_numeric())
                {
                return true;
                }
            // ...else, if we are treating all proper as familiar, then true if familiar;
            // otherwise return whether it is on the list
            if (m_properNounMethod == proper_noun_counting_method::all_proper_nouns_are_familiar)
                {
                if (the_word.is_proper_noun())
                    {
                    return true;
                    }
                return is_on_list(the_word);
                }
            // if proper are always unfamiliar, then just return whether it is on the list
            if (m_properNounMethod == proper_noun_counting_method::all_proper_nouns_are_unfamiliar)
                {
                return is_on_list(the_word);
                }
            // only_count_first_instance_of_proper_noun_as_unfamiliar
            // already on the list? then don't bother seeing if it is proper
            if (is_on_list(the_word))
                {
                return true;
                }
            // not on the list, so if not proper then it's unfamiliar
            if (!the_word.is_proper_noun())
                {
                return false;
                }
            // ...or if it's proper, then add it to the proper nouns already encountered.
            // If already encountered, then it is familiar.
            auto pos = m_usedProperNouns.insert(the_word);
            // this being false means that it was already in the set
            return !pos.second;
            }

        /** @brief Call this to reset the list of known proper nouns encountered in the document.
            @details This is only necessary if you are using the same object to analyze multiple
                documents or samples.*/
        void clear_encountered_proper_nouns() noexcept { m_usedProperNouns.clear(); }

      private:
        [[nodiscard]]
        inline bool is_on_list(const word_typeT& the_word) const
            {
            if (m_wordlist == nullptr || the_word.length() == 0)
                {
                return false;
                }

            // first, see if the full word is already familiar
            if (StemText.get_language() == stemming::stemming_type::no_stemming)
                {
                if (std::binary_search(m_wordlist->get_words().cbegin(),
                                       m_wordlist->get_words().cend(), the_word))
                    {
                    return true;
                    }
                }
            else
                {
                word_typeT compValue{ the_word };
                StemText(compValue);
                if (std::binary_search(m_wordlist->get_words().cbegin(),
                                       m_wordlist->get_words().cend(), compValue))
                    {
                    return true;
                    }
                }

            // see if the word is a hyphenated (or slashed) compound word
            string_util::string_tokenize<word_typeT> tkzr(
                the_word, common_lang_constants::COMPOUND_WORD_SEPARATORS.c_str(), true);
            if (tkzr.has_more_delimiters())
                {
                // makes sure that there is at least one valid block of text in the string
                bool validTokenFound = false;
                while (tkzr.has_more_tokens())
                    {
                    auto compValue = tkzr.get_next_token();
                    // if at least one chunk of text around a '-' is real,
                    // then set this flag to true
                    if (compValue.length() > 0)
                        {
                        validTokenFound = true;
                        }
                    StemText(compValue);
                    // in case we have something like "one-", then the fact that there
                    // is no second word after the '-' shouldn't make it unfamiliar
                    if (compValue.length() > 0 &&
                        !std::binary_search(m_wordlist->get_words().cbegin(),
                                            m_wordlist->get_words().cend(), compValue))
                        {
                        return false;
                        }
                    }
                // true if at least one token was found and all tokens found were familiar
                return validTokenFound;
                }
            return false;
            }

        const wordlistT* m_wordlist{ nullptr };
        bool m_treat_numeric_as_familiar{ true };
        mutable stemmerT StemText;
        readability::proper_noun_counting_method m_properNounMethod{
            proper_noun_counting_method::all_proper_nouns_are_familiar
        };
        mutable std::set<word_typeT> m_usedProperNouns;
        };

    /** @brief Generic interface for use defined word familiar word lists.
        @details Word functors perform a binary search on this, so be sure to
            specify the comparison traits that you expect (e.g., case insensitive comparison)
            in your @c word_typeT argument.*/
    template<typename string_typeT>
    class familiar_word_container
        {
      public:
        /// @brief Constructor.
        familiar_word_container() noexcept = default;

        using word_type = string_typeT;

        /// @private
        [[nodiscard]]
        const std::vector<string_typeT>& get_words() const noexcept
            {
            return words;
            }

        /// @returns The list of familiar words.
        [[nodiscard]]
        std::vector<string_typeT>& get_words() noexcept
            {
            return words;
            }

        /// @returns The number of words in the list.
        [[nodiscard]]
        size_t get_list_size() const noexcept
            {
            return words.size();
            }

      private:
        std::vector<string_typeT> words;
        };

    /// @brief noop functor used in place of is_familiar_word for special situations.
    template<typename word_typeT>
    class is_never_familiar_word
        {
      public:
        /// @brief Functor interface, simply returns false.
        /// @param theWord Not used.
        [[nodiscard]]
        inline bool operator()([[maybe_unused]] const word_typeT& theWord) const noexcept
            {
            return false;
            }
        };

    /// @brief Difficulty levels defined by Lix/Rix.
    enum class lix_difficulty
        {
        lix_very_easy,     /*!< Very easy.*/
        lix_easy,          /*!< Easy.*/
        lix_average,       /*!< Average.*/
        lix_difficult,     /*!< Difficult.*/
        lix_very_difficult /*!< Very difficult.*/
        };

    /// @brief Difficulty levels defined by Flesch Reading Ease.
    enum class flesch_difficulty
        {
        flesch_very_easy,        /*!< Very easy.*/
        flesch_easy,             /*!< Easy.*/
        flesch_fairly_easy,      /*!< Fairly easy.*/
        flesch_standard,         /*!< Average.*/
        flesch_fairly_difficult, /*!< Fairly difficult.*/
        flesch_difficult,        /*!< Difficult.*/
        flesch_very_difficult    /*!< Very difficult.*/
        };

    /// @brief Difficulty levels defined by INFLESZ.
    enum class inflesz_difficulty
        {
        very_easy,        /*!< Very easy.*/
        fairly_easy,      /*!< Fairly easy.*/
        normal,           /*!< Average.*/
        fairly_difficult, /*!< Fairly difficult.*/
        very_difficult    /*!< Very difficult.*/
        };

    /** @brief Rate Index (variation of Lix).
        @param[out] grade_level The grade level.
        @param number_of_longwords The number of words that are @c 7 or
            more characters (minus punctuation).
        @param number_of_sentence_units The number of sentence units (sentence ending with `.?!;:`).
        @returns An index value.
        @throws std::domain_error If @c number_of_sentence_units is @c 0,
                throws an exception.*/
    [[nodiscard]]
    inline double rix(size_t& grade_level, const size_t number_of_longwords,
                      const size_t number_of_sentence_units)
        {
        if (number_of_sentence_units == 0)
            {
            throw std::domain_error("invalid sentence count.");
            }
        const double index = number_of_longwords / static_cast<double>(number_of_sentence_units);

        if (index >= 7.2)
            {
            grade_level = 13;
            }
        else if (index >= 6.2)
            {
            grade_level = 12;
            }
        else if (index >= 5.3)
            {
            grade_level = 11;
            }
        else if (index >= 4.5)
            {
            grade_level = 10;
            }
        else if (index >= 3.7)
            {
            grade_level = 9;
            }
        else if (index >= 3.0)
            {
            grade_level = 8;
            }
        else if (index >= 2.4)
            {
            grade_level = 7;
            }
        else if (index >= 1.8)
            {
            grade_level = 6;
            }
        else if (index >= 1.3)
            {
            grade_level = 5;
            }
        else if (index >= 0.8)
            {
            grade_level = 4;
            }
        else if (index >= 0.5)
            {
            grade_level = 3;
            }
        else if (index >= 0.2)
            {
            grade_level = 2;
            }
        else
            {
            grade_level = 1;
            }

        return index;
        }

    /** @brief Helper function to convert Lix index to difficulty.
        @param index The calculated Lix index value to convert.
        @returns The Lix difficulty level.*/
    [[nodiscard]]
    inline lix_difficulty lix_index_to_difficulty_level(const size_t index) noexcept
        {
        if (index < 30)
            {
            return lix_difficulty::lix_very_easy;
            }
        if (index <= 39)
            {
            return lix_difficulty::lix_easy;
            }
        if (index <= 49)
            {
            return lix_difficulty::lix_average;
            }
        if (index <= 59)
            {
            return lix_difficulty::lix_difficult;
            }
        return lix_difficulty::lix_very_difficult;
        }

    /** @brief Laesbarhedsindex (Lix) formula by Björnsson (Sweden).
        @param[out] difficulty_level The LIX difficulty level.
        @param[out] grade_level The grade level.
        @param number_of_words The number of words in the text.
        @param number_of_longwords The number of words that are @c 7 or more characters
            (minus punctuation, based on Anderson's recommendation for Rix
            [which seems logical for Lix as well]).
        @param number_of_sentences The number of sentences.\n
            Anderson states to use sentence units in Rix, but Björnsson never
            explicitly explains how to parse sentences.
            Because using units can be quick a bit different from other tests,
            it's a better assumption to use regular sentences for this test.
        @returns An index value.
        @throws std::domain_error If either @c number_of_words or @c number_of_sentences is @c 0,
                throws an exception.*/
    [[nodiscard]]
    inline size_t lix(lix_difficulty& difficulty_level, size_t& grade_level,
                      const size_t number_of_words, const size_t number_of_longwords,
                      const size_t number_of_sentences)
        {
        if (number_of_words == 0 || number_of_sentences == 0)
            {
            throw std::domain_error("invalid word/sentence parameter.");
            }
        const auto index = static_cast<size_t>(
            round_to_integer(number_of_words / static_cast<double>(number_of_sentences) +
                             (100 * static_cast<double>(number_of_longwords /
                                                        static_cast<double>(number_of_words)))));

        difficulty_level = lix_index_to_difficulty_level(index);

        if (index < 10)
            {
            grade_level = 1;
            }
        else if (is_within<size_t>(index, 10, 14))
            {
            grade_level = 2;
            }
        else if (is_within<size_t>(index, 15, 19))
            {
            grade_level = 3;
            }
        else if (is_within<size_t>(index, 20, 23))
            {
            grade_level = 4;
            }
        else if (is_within<size_t>(index, 24, 27))
            {
            grade_level = 5;
            }
        else if (is_within<size_t>(index, 28, 31))
            {
            grade_level = 6;
            }
        else if (is_within<size_t>(index, 32, 35))
            {
            grade_level = 7;
            }
        else if (is_within<size_t>(index, 36, 39))
            {
            grade_level = 8;
            }
        else if (is_within<size_t>(index, 40, 43))
            {
            grade_level = 9;
            }
        else if (is_within<size_t>(index, 44, 47))
            {
            grade_level = 10;
            }
        else if (is_within<size_t>(index, 48, 51))
            {
            grade_level = 11;
            }
        else if (is_within<size_t>(index, 52, 55))
            {
            grade_level = 12;
            }
        else
            {
            grade_level = 13;
            }

        return index;
        }

    /** @brief Helper function to convert a Flesch score into a human readable difficulty level.
        @param result The 0-100 calculated Flesch Reading Ease index.
        @returns The difficulty level of the index value.
        @note In the book, it states that 0-30 is "very difficult," and then 30-50 is "difficult."
            Which category 30 should fall into becomes debatable. It does later state that 90-100
            is "very easy," so it's best to assume that 80-89 (not 80-90) would be "easy." Hence,
            our ranges will go from 0-29 ("very difficult") and such.*/
    [[nodiscard]]
    inline flesch_difficulty flesch_score_to_difficulty_level(const size_t result) noexcept
        {
        if (result <= 29)
            {
            return flesch_difficulty::flesch_very_difficult;
            }
        if (result <= 49)
            {
            return flesch_difficulty::flesch_difficult;
            }
        if (result <= 59)
            {
            return flesch_difficulty::flesch_fairly_difficult;
            }
        if (result <= 69)
            {
            return flesch_difficulty::flesch_standard;
            }
        if (result <= 79)
            {
            return flesch_difficulty::flesch_fairly_easy;
            }
        if (result <= 89)
            {
            return flesch_difficulty::flesch_easy;
            }
        // 90 or above
        return flesch_difficulty::flesch_very_easy;
        }
    } // namespace readability

#endif // READABILITY_H
