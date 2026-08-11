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

#ifndef INDEXING_PLAIN_LANGUAGE_PHRASE_H
#define INDEXING_PLAIN_LANGUAGE_PHRASE_H

#include "../Wisteria-Dataviz/src/import/text_matrix.h"
#include "../Wisteria-Dataviz/src/import/text_preview.h"
#include "../Wisteria-Dataviz/src/math/mathematics.h"
#include "character_traits.h"
#include "phrase.h"
#include <vector>

namespace grammar
    {
    /** @brief The information paired with a technical phrase in a
            plain_language_phrase_collection. A plain-language replacement phrase
            (used for the proximity check) and a detailed explanation (shown in a
            sidebar note if the phrase is never found explained nearby).*/
    struct plain_language_entry
        {
        /// @brief The plain-language replacement for the technical phrase.
        phrase<traits::case_insensitive_wstring_ex> replacement;
        /// @brief A detailed explanation of the technical phrase.
        traits::case_insensitive_wstring_ex explanation;
        };

    /** @brief Wrapper for a collection of "technical phrase -> plain-language
            replacement and explanation" entries, used by the Plain Language Guide report.
        @details Modeled on @c phrase_collection, but purpose-built for the simpler
            3-column format (no phrase type or preceding/trailing exceptions).*/
    class plain_language_phrase_collection
        {
      public:
        using plain_language_pair =
            comparable_first_pair<phrase<traits::case_insensitive_wstring_ex>,
                                  plain_language_entry>;
        constexpr static size_t npos = static_cast<size_t>(-1);

        /** @brief Compares a range of words to see if it matches any technical
                phrases in this collection.
            @param words An iterator to a container of words
                (should have the same interface as `std::basic_string`).
            @param position The position in the sentence of word sequence.
            @param max_word_count The maximum number of words from the sequence to
                compare against.
            @param allow_one_word_phrase Whether any of our phrases only contain one word.
            @returns The index into the phrase collection of the matching phrase,
                or @c npos if no match is found.*/
        template<typename Tword_iter>
        [[nodiscard]]
        size_t operator()(const Tword_iter& words, const size_t position,
                          const size_t max_word_count, const bool allow_one_word_phrase) const
            {
            if (max_word_count < 1)
                {
                return npos;
                }
            if (!allow_one_word_phrase && max_word_count < 2)
                {
                return npos;
                }
            phrase<traits::case_insensitive_wstring_ex> searchPhrase;
            searchPhrase.add_word(words[0].c_str());
            if (!allow_one_word_phrase)
                {
                searchPhrase.add_word(words[1].c_str());
                }
            const plain_language_pair searchValue(searchPhrase, plain_language_entry());

            auto foundPhrase = std::lower_bound(m_phrases.cbegin(), m_phrases.cend(), searchValue);
            if (foundPhrase == m_phrases.end())
                {
                return npos;
                }
            while (foundPhrase != m_phrases.end() && foundPhrase->first.get_words()[0] == words[0])
                {
                if (foundPhrase->first.equal_to_words(words, position, max_word_count).first)
                    {
                    // look ahead for a bigger, matching phrase before returning this index
                    auto nextPhrase = foundPhrase + 1;
                    while (nextPhrase != m_phrases.end() &&
                           nextPhrase->first.get_words()[0] == words[0])
                        {
                        const auto nextPhraseCompareResult =
                            nextPhrase->first.equal_to_words(words, position, max_word_count);
                        if (nextPhraseCompareResult.first)
                            {
                            foundPhrase = nextPhrase++;
                            }
                        else if (nextPhraseCompareResult.second !=
                                 phrase_comparison_result::phrase_greater_than)
                            {
                            ++nextPhrase;
                            }
                        else
                            {
                            break;
                            }
                        }
                    return (foundPhrase - m_phrases.begin());
                    }
                ++foundPhrase;
                }
            return npos;
            }

        /** @returns A vector of the technical phrase / plain-language entries.*/
        [[nodiscard]]
        const std::vector<plain_language_pair>& get_phrases() const noexcept
            {
            return m_phrases;
            }

        /** @brief Loads phrases from a text stream.
            Each row in this text should be tab-delimited, with the columns:
            - Technical phrase
            - Plain-language replacement (can be multiple words)
            - Detailed explanation
            @param text The text stream to load the phrases from.
            @param sort_phrases Whether to sort the phrases after loading them.
            @param preserve_phrases Whether phrases already in the list should be kept.*/
        void load_phrases(const wchar_t* text, const bool sort_phrases, const bool preserve_phrases)
            {
            if (text == nullptr)
                {
                return;
                }
            if (!preserve_phrases)
                {
                clear_phrases();
                }
            plain_language_pair newPair;

            lily_of_the_valley::text_preview preview;
            const size_t lineCount = preview(text, L'\t', true, false);
            if (lineCount == 0)
                {
                return;
                }
            m_phrases.reserve(m_phrases.size() + lineCount);

            // technical phrase, plain-language replacement, detailed explanation
            std::vector<traits::case_insensitive_wstring_ex> rowStrings(3);
            lily_of_the_valley::standard_delimited_character_column tabbedColumn(
                lily_of_the_valley::text_column_delimited_character_parser{ L'\t' }, 3);
            lily_of_the_valley::text_row<traits::case_insensitive_wstring_ex> row(std::nullopt);
            row.treat_consecutive_delimiters_as_one(false);
            row.add_column(tabbedColumn);
            row.set_values(&rowStrings);

            lily_of_the_valley::standard_delimited_character_column spacedColumn(
                lily_of_the_valley::text_column_delimited_character_parser{ L' ' }, std::nullopt);

            lily_of_the_valley::text_row<traits::case_insensitive_wstring_ex> phraseRow(
                std::nullopt);
            phraseRow.treat_consecutive_delimiters_as_one(true);
            phraseRow.allow_column_resizing();
            phraseRow.add_column(spacedColumn);

            lily_of_the_valley::text_row<traits::case_insensitive_wstring_ex> replacementRow(
                std::nullopt);
            replacementRow.treat_consecutive_delimiters_as_one(true);
            replacementRow.allow_column_resizing();
            replacementRow.add_column(spacedColumn);

            do
                {
                text = row.read(text);
                if (row.get_number_of_columns_last_read() < 1)
                    {
                    continue;
                    }

                // technical phrase should have at least one word
                phraseRow.set_values(&newPair.first.get_words());
                phraseRow.read(rowStrings[0].c_str());
                if (phraseRow.get_number_of_columns_last_read() < 1)
                    {
                    continue;
                    }
                newPair.first.resize(phraseRow.get_number_of_columns_last_read());

                // plain-language replacement is optional (a list author may rely purely
                // on the explanation column); an empty replacement phrase simply means
                // the proximity check will never find it nearby.
                newPair.second.replacement.clear_words();
                if (row.get_number_of_columns_last_read() >= 2 && !rowStrings[1].empty())
                    {
                    replacementRow.set_values(&newPair.second.replacement.get_words());
                    replacementRow.read(rowStrings[1].c_str());
                    newPair.second.replacement.resize(
                        replacementRow.get_number_of_columns_last_read());
                    }

                newPair.second.explanation = (row.get_number_of_columns_last_read() >= 3) ?
                                                 rowStrings[2] :
                                                 traits::case_insensitive_wstring_ex();

                m_phrases.push_back(newPair);
                } while (text != nullptr);

            if (sort_phrases)
                {
                sort();
                }
            }

        /** @brief Sorts the phrases (by technical phrase).*/
        void sort() { std::sort(m_phrases.begin(), m_phrases.end()); }

        /** @brief Removes all phrases from the container.*/
        void clear_phrases() noexcept { m_phrases.clear(); }

      private:
        std::vector<plain_language_pair> m_phrases;
        };
    } // namespace grammar

#endif // INDEXING_PLAIN_LANGUAGE_PHRASE_H
