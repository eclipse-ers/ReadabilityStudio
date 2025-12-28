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

#ifndef CUSTOM_READABILITY_TEST_H
#define CUSTOM_READABILITY_TEST_H

#include "../OleanderStemmingLibrary/src/danish_stem.h"
#include "../OleanderStemmingLibrary/src/dutch_stem.h"
#include "../OleanderStemmingLibrary/src/english_stem.h"
#include "../OleanderStemmingLibrary/src/finnish_stem.h"
#include "../OleanderStemmingLibrary/src/french_stem.h"
#include "../OleanderStemmingLibrary/src/german_stem.h"
#include "../OleanderStemmingLibrary/src/italian_stem.h"
#include "../OleanderStemmingLibrary/src/norwegian_stem.h"
#include "../OleanderStemmingLibrary/src/portuguese_stem.h"
#include "../OleanderStemmingLibrary/src/spanish_stem.h"
#include "../OleanderStemmingLibrary/src/swedish_stem.h"
#include "../indexing/word_functional.h"
#include "../indexing/word_list.h"
#include "readability.h"
#include "readability_test.h"
#include <algorithm>
#include <string>
#include <utility>

namespace readability
    {
    /** @brief A user-defined test, which can included its own familiar words
            and stemming method (if using familiar words).*/
    template<typename word_typeT>
    class custom_test final : public test_with_classification, public base_test
        {
      public:
        /// @brief The string type when comparing words against the familiar words list.
        using string_type = traits::case_insensitive_wstring_ex;
        /// @brief The familiar word list type.
        using word_list_type = familiar_word_container<string_type>;

        custom_test(string_type name, string_type formula, const readability_test_type result_type,
                    string_type file_path, const stemming::stemming_type stem_type,
                    const bool inc_custom_familiar_word_list, const bool inc_dale_chall_list,
                    const word_list* dale_chall_list, const bool inc_spache_list,
                    const word_list* spache_list, const bool inc_harris_jacobson_list,
                    const word_list* harris_jacobson_list, const bool inc_stocker_list,
                    const word_list* stocker_list,
                    const bool familiar_words_must_be_on_each_included_list,
                    const readability::proper_noun_counting_method properNounMethod,
                    const bool treat_numeric_as_familiar,
                    // industry
                    const bool industryChildrensPublishingSelected,
                    const bool industryAdultPublishingSelected,
                    const bool industrySecondaryLanguageSelected,
                    const bool industryChildrensHealthCareSelected,
                    const bool industryAdultHealthCareSelected,
                    const bool industryMilitaryGovernmentSelected,
                    const bool industryBroadcastingSelected,
                    // document
                    const bool documentGeneralSelected, const bool documentTechSelected,
                    const bool documentNonNarrativeFormSelected,
                    const bool documentYoungAdultSelected, const bool documentChildrenSelected)
            : test_with_classification(
                  industryChildrensPublishingSelected, industryAdultPublishingSelected,
                  industrySecondaryLanguageSelected, industryChildrensHealthCareSelected,
                  industryAdultHealthCareSelected, industryMilitaryGovernmentSelected,
                  industryBroadcastingSelected, documentGeneralSelected, documentTechSelected,
                  documentNonNarrativeFormSelected, documentYoungAdultSelected,
                  documentChildrenSelected),
              m_name(std::move(name)), m_formula(std::move(formula)), m_result_type(result_type),
              m_familiar_word_list_file_path(std::move(file_path)), m_stemming_type(stem_type),
              m_include_custom_familiar_word_list(inc_custom_familiar_word_list),
              m_familiar_words_ref_counter(new word_list_type),
              m_include_dale_chall_list(inc_dale_chall_list),
              is_dc_word(dale_chall_list,
                         proper_noun_counting_method::all_proper_nouns_are_unfamiliar, false),
              m_include_spache_list(inc_spache_list),
              is_spache_word(spache_list,
                             proper_noun_counting_method::all_proper_nouns_are_unfamiliar, false),
              m_include_harris_jacobson_list(inc_harris_jacobson_list),
              is_harris_jacobson_word(harris_jacobson_list,
                                      proper_noun_counting_method::all_proper_nouns_are_unfamiliar,
                                      false),
              m_include_stocker_list(inc_stocker_list),
              is_stocker_word(stocker_list,
                              proper_noun_counting_method::all_proper_nouns_are_unfamiliar, false),
              m_familiar_words_must_be_on_all_included_list(
                  familiar_words_must_be_on_each_included_list),
              is_word_familiar_proper_or_numeric(nullptr, properNounMethod,
                                                 treat_numeric_as_familiar)

            {
            is_word_familiar_no_stem.set_word_list(m_familiar_words_ref_counter.get());
            is_word_familiar_danish_stem.set_word_list(m_familiar_words_ref_counter.get());
            is_word_familiar_dutch_stem.set_word_list(m_familiar_words_ref_counter.get());
            is_word_familiar_english_stem.set_word_list(m_familiar_words_ref_counter.get());
            is_word_familiar_finnish_stem.set_word_list(m_familiar_words_ref_counter.get());
            is_word_familiar_french_stem.set_word_list(m_familiar_words_ref_counter.get());
            is_word_familiar_german_stem.set_word_list(m_familiar_words_ref_counter.get());
            is_word_familiar_italian_stem.set_word_list(m_familiar_words_ref_counter.get());
            is_word_familiar_norwegian_stem.set_word_list(m_familiar_words_ref_counter.get());
            is_word_familiar_portuguese_stem.set_word_list(m_familiar_words_ref_counter.get());
            is_word_familiar_swedish_stem.set_word_list(m_familiar_words_ref_counter.get());
            is_word_familiar_spanish_stem.set_word_list(m_familiar_words_ref_counter.get());

            /* These other object don't use extended flags (that is handled by the
               is_word_familiar_proper_or_numeric object), so turn these flags off so that these
               object simply tell if a word is on a list.*/
            is_dc_word.set_proper_noun_method(
                proper_noun_counting_method::all_proper_nouns_are_unfamiliar);
            is_dc_word.include_numeric_as_familiar(false);

            is_spache_word.set_proper_noun_method(
                proper_noun_counting_method::all_proper_nouns_are_unfamiliar);
            is_spache_word.include_numeric_as_familiar(false);

            is_harris_jacobson_word.set_proper_noun_method(
                proper_noun_counting_method::all_proper_nouns_are_unfamiliar);
            is_harris_jacobson_word.include_numeric_as_familiar(false);

            is_stocker_word.set_proper_noun_method(
                proper_noun_counting_method::all_proper_nouns_are_unfamiliar);
            is_stocker_word.include_numeric_as_familiar(false);

            is_word_familiar_no_stem.set_proper_noun_method(
                proper_noun_counting_method::all_proper_nouns_are_unfamiliar);
            is_word_familiar_no_stem.include_numeric_as_familiar(false);

            is_word_familiar_danish_stem.set_proper_noun_method(
                proper_noun_counting_method::all_proper_nouns_are_unfamiliar);
            is_word_familiar_danish_stem.include_numeric_as_familiar(false);

            is_word_familiar_dutch_stem.set_proper_noun_method(
                proper_noun_counting_method::all_proper_nouns_are_unfamiliar);
            is_word_familiar_dutch_stem.include_numeric_as_familiar(false);

            is_word_familiar_english_stem.set_proper_noun_method(
                proper_noun_counting_method::all_proper_nouns_are_unfamiliar);
            is_word_familiar_english_stem.include_numeric_as_familiar(false);

            is_word_familiar_finnish_stem.set_proper_noun_method(
                proper_noun_counting_method::all_proper_nouns_are_unfamiliar);
            is_word_familiar_finnish_stem.include_numeric_as_familiar(false);

            is_word_familiar_french_stem.set_proper_noun_method(
                proper_noun_counting_method::all_proper_nouns_are_unfamiliar);
            is_word_familiar_french_stem.include_numeric_as_familiar(false);

            is_word_familiar_german_stem.set_proper_noun_method(
                proper_noun_counting_method::all_proper_nouns_are_unfamiliar);
            is_word_familiar_german_stem.include_numeric_as_familiar(false);

            is_word_familiar_italian_stem.set_proper_noun_method(
                proper_noun_counting_method::all_proper_nouns_are_unfamiliar);
            is_word_familiar_italian_stem.include_numeric_as_familiar(false);

            is_word_familiar_norwegian_stem.set_proper_noun_method(
                proper_noun_counting_method::all_proper_nouns_are_unfamiliar);
            is_word_familiar_norwegian_stem.include_numeric_as_familiar(false);

            is_word_familiar_portuguese_stem.set_proper_noun_method(
                proper_noun_counting_method::all_proper_nouns_are_unfamiliar);
            is_word_familiar_portuguese_stem.include_numeric_as_familiar(false);

            is_word_familiar_swedish_stem.set_proper_noun_method(
                proper_noun_counting_method::all_proper_nouns_are_unfamiliar);
            is_word_familiar_swedish_stem.include_numeric_as_familiar(false);

            is_word_familiar_spanish_stem.set_proper_noun_method(
                proper_noun_counting_method::all_proper_nouns_are_unfamiliar);
            is_word_familiar_spanish_stem.include_numeric_as_familiar(false);
            }

        /// @brief Compares the key information about the test to determine if they are the same.
        /// @private
        [[nodiscard]]
        bool operator==(const custom_test<word_typeT>& that) const
            {
            return (
                (m_name == that.m_name) &&
                (string_util::remove_all_whitespace<string_type>(get_formula()) ==
                 string_util::remove_all_whitespace<string_type>(that.get_formula())) &&
                (m_result_type == that.m_result_type) &&
                (m_familiar_word_list_file_path == that.m_familiar_word_list_file_path) &&
                (m_stemming_type == that.m_stemming_type) &&
                (m_include_custom_familiar_word_list == that.m_include_custom_familiar_word_list) &&
                (m_include_dale_chall_list == that.m_include_dale_chall_list) &&
                (m_include_spache_list == that.m_include_spache_list) &&
                (m_include_harris_jacobson_list == that.m_include_harris_jacobson_list) &&
                (m_include_stocker_list == that.m_include_stocker_list) &&
                (is_word_familiar_proper_or_numeric.get_proper_noun_method() ==
                 that.is_word_familiar_proper_or_numeric.get_proper_noun_method()) &&
                (is_word_familiar_proper_or_numeric.is_including_numeric_as_familiar() ==
                 that.is_word_familiar_proper_or_numeric.is_including_numeric_as_familiar()) &&
                (m_familiar_words_must_be_on_all_included_list ==
                 that.m_familiar_words_must_be_on_all_included_list));
            }

        /// @private
        [[nodiscard]]
        bool operator==(const wchar_t* name) const noexcept
            {
            return (m_name.compare(name) == 0);
            }

        /// @private
        [[nodiscard]]
        bool operator==(const string_type& name) const noexcept
            {
            return (m_name == name);
            }

        /// @brief Sets the stemming method used for the list of custom (familiar) words
        ///     used by the test.
        /// @param stem_type The stemming method to use.
        void set_stemming_type(const stemming::stemming_type stem_type) noexcept
            {
            m_stemming_type = stem_type;
            }

        /// @returns The stemming method .
        [[nodiscard]]
        stemming::stemming_type get_stemming_type() const noexcept
            {
            return m_stemming_type;
            }

        /// Get/Set formula functions
        void set_formula(const string_type& form) { m_formula = form; }

        [[nodiscard]]
        const string_type& get_formula() const noexcept
            {
            return m_formula;
            }

        /// Get/Set result type
        void set_test_type(const readability_test_type type) noexcept { m_result_type = type; }

        [[nodiscard]]
        readability_test_type get_test_type() const noexcept
            {
            return m_result_type;
            }

        /// Get/Set name functions
        void set_name(const string_type& name) { m_name = name; }

        [[nodiscard]]
        const string_type& get_name() const noexcept
            {
            return m_name;
            }

        /// @returns Whether this test is using word lists to determine word familiarity.
        [[nodiscard]]
        bool is_using_familiar_words() const
            {
            return (is_including_custom_familiar_word_list() || is_including_dale_chall_list() ||
                    is_including_spache_list() || is_including_harris_jacobson_list() ||
                    is_including_stocker_list());
            }

        /// Get/Set word list path functions
        void set_familiar_word_list_file_path(const string_type& path)
            {
            m_familiar_word_list_file_path = path;
            }

        [[nodiscard]]
        const string_type& get_familiar_word_list_file_path() const noexcept
            {
            return m_familiar_word_list_file_path;
            }

        // get/set numeric/proper noun inclusion
        [[nodiscard]]
        proper_noun_counting_method get_proper_noun_method() const noexcept
            {
            return is_word_familiar_proper_or_numeric.get_proper_noun_method();
            }

        void set_proper_noun_method(const readability::proper_noun_counting_method proper) noexcept
            {
            is_word_familiar_proper_or_numeric.set_proper_noun_method(proper);
            }

        [[nodiscard]]
        bool is_including_numeric_as_familiar() const noexcept
            {
            return is_word_familiar_proper_or_numeric.is_including_numeric_as_familiar();
            }

        void include_numeric_as_familiar(const bool number_familiar) noexcept
            {
            is_word_familiar_proper_or_numeric.include_numeric_as_familiar(number_familiar);
            }

        void include_custom_familiar_word_list(const bool include) noexcept
            {
            m_include_custom_familiar_word_list = include;
            }

        [[nodiscard]]
        bool is_including_custom_familiar_word_list() const noexcept
            {
            return m_include_custom_familiar_word_list;
            }

        // get/set for DC and Spache lists inclusion
        void include_dale_chall_list(const bool include) noexcept
            {
            m_include_dale_chall_list = include;
            }

        [[nodiscard]]
        bool is_including_dale_chall_list() const noexcept
            {
            return m_include_dale_chall_list;
            }

        void include_spache_list(const bool include) noexcept { m_include_spache_list = include; }

        [[nodiscard]]
        bool is_including_spache_list() const noexcept
            {
            return m_include_spache_list;
            }

        void include_harris_jacobson_list(const bool include) noexcept
            {
            m_include_harris_jacobson_list = include;
            }

        [[nodiscard]]
        bool is_including_harris_jacobson_list() const noexcept
            {
            return m_include_harris_jacobson_list;
            }

        void include_stocker_list(const bool include) noexcept { m_include_stocker_list = include; }

        [[nodiscard]]
        bool is_including_stocker_list() const noexcept
            {
            return m_include_stocker_list;
            }

        void set_familiar_words_must_be_on_each_included_list(const bool include) noexcept
            {
            m_familiar_words_must_be_on_all_included_list = include;
            }

        [[nodiscard]]
        bool is_familiar_words_must_be_on_each_included_list() const noexcept
            {
            return m_familiar_words_must_be_on_all_included_list;
            }

        /** Loads a block of words into the custom familiar word list.
            Also stems the word list and removes any duplicates,
            so set the stemmer prior to calling this.*/
        void load_custom_familiar_words(const wchar_t* text) const
            {
            std::vector<string_type>& wordData = m_familiar_words_ref_counter->get_words();
            wordData.clear();

            string_util::string_tokenize<string_type> tkzr(text, L" \t\n\r;,", true);
            wordData.reserve(tkzr.count_tokens(text));
            while (tkzr.has_more_tokens())
                {
                wordData.emplace_back(tkzr.get_next_token());
                }

            switch (m_stemming_type)
                {
            case stemming::stemming_type::danish:
                std::ranges::for_each(wordData, stemming::danish_stem<string_type>());
                break;
            case stemming::stemming_type::dutch:
                std::ranges::for_each(wordData, stemming::dutch_porter_stem<string_type>());
                break;
            case stemming::stemming_type::english:
                std::ranges::for_each(wordData, stemming::english_stem<string_type>());
                break;
            case stemming::stemming_type::finnish:
                std::ranges::for_each(wordData, stemming::finnish_stem<string_type>());
                break;
            case stemming::stemming_type::french:
                std::ranges::for_each(wordData, stemming::french_stem<string_type>());
                break;
            case stemming::stemming_type::german:
                std::ranges::for_each(wordData, stemming::german_stem<string_type>());
                break;
            case stemming::stemming_type::italian:
                std::ranges::for_each(wordData, stemming::italian_stem<string_type>());
                break;
            case stemming::stemming_type::norwegian:
                std::ranges::for_each(wordData, stemming::norwegian_stem<string_type>());
                break;
            case stemming::stemming_type::portuguese:
                std::ranges::for_each(wordData, stemming::portuguese_stem<string_type>());
                break;
            case stemming::stemming_type::swedish:
                std::ranges::for_each(wordData, stemming::swedish_stem<string_type>());
                break;
            case stemming::stemming_type::spanish:
                std::ranges::for_each(wordData, stemming::spanish_stem<string_type>());
                break;
            default:
                // don't stem anything
                break;
                }

            // remove any duplicates
            std::sort(wordData.begin(), wordData.end());
            auto endOfUniquePos = std::unique(wordData.begin(), wordData.end());
            if (endOfUniquePos != wordData.end())
                {
                wordData.erase(endOfUniquePos, wordData.end());
                }
            }

        /// @returns Whether a word is considered familiar by this test.\n
        ///     This is based on any standard and custom word lists
        ///     included in this test.\n
        ///     The function takes into account the proper noun and
        ///     numeric settings specified by this test.
        /// @param the_word The word to review.
        [[nodiscard]]
        bool is_word_familiar(const word_typeT& the_word) const
            {
            if (is_word_familiar_proper_or_numeric(the_word))
                {
                return true;
                }

            return is_word_familiar_standard(the_word);
            }

        /// Determines whether a word is considered familiar by this test.
        /// This version overrides the test's current proper noun and numeric logic for special
        /// situations. Note that the proper flag is just true or false for simplicity, so "count
        /// first occurrence" option is not available here.
        [[nodiscard]]
        bool is_word_familiar(const word_typeT& theWord, const bool treat_proper_as_familiar,
                              const bool treat_numeric_as_familiar) const
            {
            is_familiar_word<word_typeT, const word_list_type, stemming::no_op_stem<string_type>>
                tempProperNounOrNumericFamiliar(
                    nullptr,
                    treat_proper_as_familiar ?
                        proper_noun_counting_method::all_proper_nouns_are_familiar :
                        proper_noun_counting_method::all_proper_nouns_are_unfamiliar,
                    treat_numeric_as_familiar);
            if (tempProperNounOrNumericFamiliar(theWord))
                {
                return true;
                }

            return is_word_familiar_standard(theWord);
            }

        /// @brief Flushes out the known proper nouns that the familiar
        ///     word lists have already encountered.
        /// @details This should be called if you are getting ready
        ///     to analyze a new sample or document.
        void reset() noexcept
            {
            is_word_familiar_proper_or_numeric.clear_encountered_proper_nouns();
            // the rest of these aren't used, but doesn't hurt to clear them here anyway
            is_dc_word.clear_encountered_proper_nouns();
            is_spache_word.clear_encountered_proper_nouns();
            is_harris_jacobson_word.clear_encountered_proper_nouns();
            is_stocker_word.clear_encountered_proper_nouns();
            is_word_familiar_no_stem.clear_encountered_proper_nouns();
            is_word_familiar_danish_stem.clear_encountered_proper_nouns();
            is_word_familiar_dutch_stem.clear_encountered_proper_nouns();
            is_word_familiar_english_stem.clear_encountered_proper_nouns();
            is_word_familiar_finnish_stem.clear_encountered_proper_nouns();
            is_word_familiar_french_stem.clear_encountered_proper_nouns();
            is_word_familiar_german_stem.clear_encountered_proper_nouns();
            is_word_familiar_italian_stem.clear_encountered_proper_nouns();
            is_word_familiar_norwegian_stem.clear_encountered_proper_nouns();
            is_word_familiar_portuguese_stem.clear_encountered_proper_nouns();
            is_word_familiar_swedish_stem.clear_encountered_proper_nouns();
            is_word_familiar_spanish_stem.clear_encountered_proper_nouns();
            }

        /// @returns Whether the test is included in something
        ///     (currently just used for test bundles).
        [[nodiscard]]
        bool is_included() const noexcept
            {
            return m_included;
            }

        /// @brief Whether to include the test in a test bundle (or any other need).
        /// @param inc @c true to include the test, false to remove.
        void include(const bool inc) noexcept { m_included = inc; }

        /// @returns Direct access to the inclusion flag.
        /// @details Used when an external source needs to edit the inclusion status,
        ///     but can't call include().
        [[nodiscard]]
        bool& get_include_flag() noexcept
            {
            return m_included;
            }

      private:
        /// Doesn't look at proper noun and numeric settings.
        [[nodiscard]]
        bool is_word_familiar_standard(const word_typeT& theWord) const
            {
            // first, see if the word is already on a list (most often the case)
            if (is_familiar_words_must_be_on_each_included_list() &&
                is_word_familiar_all_included_list(theWord))
                {
                return true;
                }
            if (!is_familiar_words_must_be_on_each_included_list() &&
                is_word_familiar_any_included_list(theWord))
                {
                return true;
                }

            // see if the word is a hyphenated (or slashed) compound word
            string_util::string_tokenize<word_typeT> tkzr(
                theWord, common_lang_constants::COMPOUND_WORD_SEPARATORS.c_str(), true);
            // makes sure that there is at least one valid block of text in the string
            bool validTokenFound = false;
            word_typeT currentToken;
            while (tkzr.has_more_tokens())
                {
                currentToken = tkzr.get_next_token();
                // if at least one chunk of text around a '-' is real,
                // then set this flag to true
                if (!currentToken.empty())
                    {
                    validTokenFound = true;
                    }
                // in case we have something like "one-", then the fact that there is
                // no second word after the '-' shouldn't make it unfamiliar
                if (!currentToken.empty())
                    {
                    // if any token is an unfamiliar word, then fail immediately
                    if (is_familiar_words_must_be_on_each_included_list() &&
                        !is_word_familiar_all_included_list(currentToken))
                        {
                        return false;
                        }
                    if (!is_familiar_words_must_be_on_each_included_list() &&
                        !is_word_familiar_any_included_list(currentToken))
                        {
                        return false;
                        }
                    }
                }
            // true if at least one token was found and all tokens found were familiar
            return validTokenFound;
            }

        [[nodiscard]]
        bool is_word_familiar_any_included_list(const word_typeT& theWord) const
            {
            // first, see if the word is on any of the standard lists
            const bool onStandardLists =
                (m_include_dale_chall_list && is_dc_word(theWord)) ||
                (m_include_spache_list && is_spache_word(theWord)) ||
                (m_include_harris_jacobson_list && is_harris_jacobson_word(theWord)) ||
                (m_include_stocker_list && is_stocker_word(theWord));
            if (onStandardLists)
                {
                return true;
                }

            if (m_include_custom_familiar_word_list)
                {
                // if not, then see if it is on the custom list
                switch (m_stemming_type)
                    {
                case stemming::stemming_type::danish:
                    return is_word_familiar_danish_stem(theWord);
                case stemming::stemming_type::dutch:
                    return is_word_familiar_dutch_stem(theWord);
                case stemming::stemming_type::english:
                    return is_word_familiar_english_stem(theWord);
                case stemming::stemming_type::finnish:
                    return is_word_familiar_finnish_stem(theWord);
                case stemming::stemming_type::french:
                    return is_word_familiar_french_stem(theWord);
                case stemming::stemming_type::german:
                    return is_word_familiar_german_stem(theWord);
                case stemming::stemming_type::italian:
                    return is_word_familiar_italian_stem(theWord);
                case stemming::stemming_type::norwegian:
                    return is_word_familiar_norwegian_stem(theWord);
                case stemming::stemming_type::portuguese:
                    return is_word_familiar_portuguese_stem(theWord);
                case stemming::stemming_type::swedish:
                    return is_word_familiar_swedish_stem(theWord);
                case stemming::stemming_type::spanish:
                    return is_word_familiar_spanish_stem(theWord);
                default:
                    return is_word_familiar_no_stem(theWord);
                    }
                }
            return false;
            }

        [[nodiscard]]
        bool is_word_familiar_all_included_list(const word_typeT& theWord) const
            {
            // If word is on all included standard lists then continue.
            // If not, then return false.
            const bool isOnAllIncludedStandardLists =
                (!m_include_dale_chall_list || is_dc_word(theWord)) &&
                (!m_include_spache_list || is_spache_word(theWord)) &&
                (!m_include_harris_jacobson_list || is_harris_jacobson_word(theWord)) &&
                (!m_include_stocker_list || is_stocker_word(theWord));
            if (!isOnAllIncludedStandardLists)
                {
                return false;
                }
            // if the word is on the standard word lists, and we are not using a custom word list,
            // then it is on every list in use, so return true.
            if (!m_include_custom_familiar_word_list)
                {
                return true;
                }

            if (m_include_custom_familiar_word_list)
                {
                switch (m_stemming_type)
                    {
                case stemming::stemming_type::danish:
                    return is_word_familiar_danish_stem(theWord) && isOnAllIncludedStandardLists;
                case stemming::stemming_type::dutch:
                    return is_word_familiar_dutch_stem(theWord) && isOnAllIncludedStandardLists;
                case stemming::stemming_type::english:
                    return is_word_familiar_english_stem(theWord) && isOnAllIncludedStandardLists;
                case stemming::stemming_type::finnish:
                    return is_word_familiar_finnish_stem(theWord) && isOnAllIncludedStandardLists;
                case stemming::stemming_type::french:
                    return is_word_familiar_french_stem(theWord) && isOnAllIncludedStandardLists;
                case stemming::stemming_type::german:
                    return is_word_familiar_german_stem(theWord) && isOnAllIncludedStandardLists;
                case stemming::stemming_type::italian:
                    return is_word_familiar_italian_stem(theWord) && isOnAllIncludedStandardLists;
                case stemming::stemming_type::norwegian:
                    return is_word_familiar_norwegian_stem(theWord) && isOnAllIncludedStandardLists;
                case stemming::stemming_type::portuguese:
                    return is_word_familiar_portuguese_stem(theWord) &&
                           isOnAllIncludedStandardLists;
                case stemming::stemming_type::swedish:
                    return is_word_familiar_swedish_stem(theWord) && isOnAllIncludedStandardLists;
                case stemming::stemming_type::spanish:
                    return is_word_familiar_spanish_stem(theWord) && isOnAllIncludedStandardLists;
                default:
                    return is_word_familiar_no_stem(theWord) && isOnAllIncludedStandardLists;
                    }
                }
            else
                {
                return false;
                }
            }

        string_type m_name;
        string_type m_formula;
        readability_test_type m_result_type{ readability_test_type::grade_level };
        string_type m_familiar_word_list_file_path;
        stemming::stemming_type m_stemming_type{ stemming::stemming_type::no_stemming };

        bool m_include_custom_familiar_word_list{ false };
        std::shared_ptr<word_list_type> m_familiar_words_ref_counter;
        bool m_include_dale_chall_list{ false };
        is_familiar_word<word_typeT, const word_list, stemming::no_op_stem<string_type>> is_dc_word;
        bool m_include_spache_list{ false };
        is_familiar_word<word_typeT, const word_list, stemming::no_op_stem<string_type>>
            is_spache_word;
        bool m_include_harris_jacobson_list{ false };
        is_familiar_word<word_typeT, const word_list, stemming::no_op_stem<string_type>>
            is_harris_jacobson_word;
        bool m_include_stocker_list{ false };
        is_familiar_word<word_typeT, const word_list, stemming::no_op_stem<string_type>>
            is_stocker_word;
        bool m_familiar_words_must_be_on_all_included_list{ false };
        is_familiar_word<word_typeT, const word_list_type, stemming::no_op_stem<string_type>>
            is_word_familiar_proper_or_numeric;
        is_familiar_word<word_typeT, const word_list_type, stemming::no_op_stem<string_type>>
            is_word_familiar_no_stem;
        is_familiar_word<word_typeT, const word_list_type, stemming::danish_stem<string_type>>
            is_word_familiar_danish_stem;
        is_familiar_word<word_typeT, const word_list_type, stemming::dutch_porter_stem<string_type>>
            is_word_familiar_dutch_stem;
        is_familiar_word<word_typeT, const word_list_type, stemming::english_stem<string_type>>
            is_word_familiar_english_stem;
        is_familiar_word<word_typeT, const word_list_type, stemming::finnish_stem<string_type>>
            is_word_familiar_finnish_stem;
        is_familiar_word<word_typeT, const word_list_type, stemming::french_stem<string_type>>
            is_word_familiar_french_stem;
        is_familiar_word<word_typeT, const word_list_type, stemming::german_stem<string_type>>
            is_word_familiar_german_stem;
        is_familiar_word<word_typeT, const word_list_type, stemming::italian_stem<string_type>>
            is_word_familiar_italian_stem;
        is_familiar_word<word_typeT, const word_list_type, stemming::norwegian_stem<string_type>>
            is_word_familiar_norwegian_stem;
        is_familiar_word<word_typeT, const word_list_type, stemming::portuguese_stem<string_type>>
            is_word_familiar_portuguese_stem;
        is_familiar_word<word_typeT, const word_list_type, stemming::swedish_stem<string_type>>
            is_word_familiar_swedish_stem;
        is_familiar_word<word_typeT, const word_list_type, stemming::spanish_stem<string_type>>
            is_word_familiar_spanish_stem;

        // whether the test is included in the project
        bool m_included{ false };
        };
    }; // namespace readability

#endif // CUSTOM_READABILITY_TEST_H
