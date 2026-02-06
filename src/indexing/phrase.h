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

#ifndef INDEXING_PHRASE_H
#define INDEXING_PHRASE_H

#include "../Wisteria-Dataviz/src/import/text_matrix.h"
#include "../Wisteria-Dataviz/src/import/text_preview.h"
#include "../Wisteria-Dataviz/src/math/mathematics.h"
#include "../Wisteria-Dataviz/src/util/string_util.h"
#include "character_traits.h"
#include <vector>

/// @brief Namespace for grammar analysis.
namespace grammar
    {
    /** @brief Enumeration for phrase types.*/
    enum class phrase_type
        {
        phrase_wordy,     /*!< A phrase that could be expressed more simply. */
        phrase_redundant, /*!< A phrase that expresses the same idea twice. */
        phrase_cliche,    /*!< A tired, overused expression. */
        phrase_error      /*!< A grammar error. */
        };

    /** @brief Results that calls to equal_to_words() can produce.*/
    enum class phrase_comparison_result
        {
        phrase_longer_than,   /*!< The phrase has more words than stream of the
                                   words it was compared to.
                                   This will be an immediate short circuit. */
        phrase_equal,         /*!< The phrase is equal to the words it was compared to. */
        phrase_less_than,     /*!< The phrase is less than (lexicographically) the
                                   words it was compared to. */
        phrase_greater_than,  /*!< The phrase is greater than (lexicographically) the
                                   words it was compared to. */
        phrase_rule_exception /*!< The phrase doesn't equal the text because of trailing
                                   or preceding text rule. */
        };

    /** A class for storing a phrase (string of words), along with extended information about the
        phrase. For example, it can store information about the phrase, such as whether it is a
        grammar error.*/
    template<typename word_typeT = traits::case_insensitive_wstring_ex>
    class phrase
        {
      public:
        /// @private
        phrase() = default;

        /** @brief Simple 1-word "phrase."
            @param src The word to initialize the phrase to.*/
        explicit phrase(word_typeT src) { m_words.push_back(std::move(src)); }

        /** @returns The phrase_type of the phrase.
            For example, it could be redundant or a cliché.*/
        [[nodiscard]]
        phrase_type get_type() const noexcept
            {
            return m_phrase_type;
            }

        /** @brief Sets the phrase_type of the phrase.
            For example, it could be redundant or a cliché.
            @param type The phrase_type to set the phrase to.*/
        void set_type(const phrase_type type) noexcept { m_phrase_type = type; }

        /** @brief Adds a word to the end of the phrase.
            @param newWord The word to add to the end of the phrase.*/
        void add_word(const wchar_t* newWord) { m_words.emplace_back(newWord); }

        /** @brief Removes all the words from the phrase.*/
        void clear_words() noexcept { m_words.clear(); }

        /** @brief Resizes the number of words.
            @details This function is useful for when something else writes new
                words without removing the old ones.
            @note If you shrink it, then trailing words will be lost.
            @param count The new size (i.e., word count) of the phrase.*/
        void resize(const size_t count) { m_words.resize(count); }

        /** @brief Copies a vector of words into the phrase.
            @note The current contents of the phrase will be cleared.
            @param src The string vector to copy from.
            @param count The number of items from the source vector to copy over.*/
        void copy_words(const std::vector<word_typeT>& src, const size_t count)
            {
            assert(count <= src.size());
            m_words.resize(count);
            std::copy(src.cbegin(), src.cbegin() + count, m_words.begin());
            }

        /** @brief Copies a vector of words into the phrase.
            @note The current contents of the phrase will be cleared.
            @param src The start of the string vector to copy from.
            @param count The number of items from the source vector to copy over.*/
        void copy_words(std::vector<word_typeT>::const_iterator src, const size_t count)
            {
            m_words.resize(count);
            std::copy(src, src + count, m_words.begin());
            }

        /** @brief Indicates whether this phrase is less than another phrase.
            @details A phrase is less than other phrase if sorts alphabetically
                before the other phrase.\n
                The number of words in the phrase only matters if all the words
                match between two phrases, except that one phrase has more words
                (in which case that phrase is "bigger").
            @param that The phrase to compare against.
            @returns @c true if less than other phrase, @c false if equal
                to or greater than the other phrase.*/
        [[nodiscard]]
        bool operator<(const phrase& that) const
            {
            size_t i = 0;
            for (i = 0; i < get_word_count(); ++i)
                {
                /* if all the previous words in these phrases were the same,
                   but now we have reached the last of the words
                   in the other phrase, then this phrase must be bigger*/
                if (i >= that.get_word_count())
                    {
                    return false;
                    }
                const int cmpResult = m_words[i].compare(that.m_words[i]);
                if (cmpResult != 0)
                    {
                    return (cmpResult < 0);
                    }
                }
            // if we went through every word in this phrase and it matched every word in the other
            // phrase, but there are still more words in the other phrase, then this phrase is less.
            return static_cast<bool>(i == get_word_count() &&
                                     get_word_count() < that.get_word_count());
            }

        /** @returns @c true if all the words between the two phrases are the same.*/
        [[nodiscard]]
        bool operator==(const phrase& that) const
            {
            if (get_word_count() != that.get_word_count())
                {
                return false;
                }
            for (size_t i = 0; i < get_word_count(); ++i)
                {
                if (m_words[i] != that.m_words[i])
                    {
                    return false;
                    }
                }
            return true;
            }

        /** @brief Compares a range of words to see if the first few words matches this phrase.
            @details The word range can be bigger than the phrase, we just want to compare the
                phrase against the first few words.
            @param words An iterator to a container of words
                (should have the same interface as `std::basic_string`).
            @param position The position in the sentence of word sequence.\n
                If this is zero (meaning the passed in sequence is the start of the sentence),
                then the preceding exception logic will not be used.
            @param max_word_count The maximum number of words from the sequence to compare against.
                This would usually be the number of words in the sequence until the
                end of the sentence.
            @returns A pair containing a bool (indicating if it was a match) and a
                phrase_comparison_result.*/
        template<typename Tword_iter>
        [[nodiscard]]
        std::pair<bool, phrase_comparison_result> equal_to_words(const Tword_iter& words,
                                                                 const size_t position,
                                                                 const size_t max_word_count) const
            {
            if (get_word_count() > max_word_count)
                {
                return std::make_pair(false, phrase_comparison_result::phrase_longer_than);
                }
            size_t i = 0;
            for (i = 0; i < get_word_count(); ++i)
                {
                auto compRes = m_words[i].compare((words + i)->c_str());
                if (compRes != 0)
                    {
                    return std::make_pair(false, compRes < 0 ?
                                                     phrase_comparison_result::phrase_less_than :
                                                     phrase_comparison_result::phrase_greater_than);
                    }
                }
            // before returning true, make sure there isn't a preceding word exception
            if (get_preceding_exceptions().size() && position > 0)
                {
                if (get_preceding_exceptions().contains((words - 1)->c_str()))
                    {
                    return std::make_pair(false, phrase_comparison_result::phrase_rule_exception);
                    }
                }
            // ...or trailing exception
            if (get_trailing_exceptions().size() && get_word_count() + 1 <= max_word_count)
                {
                if (get_trailing_exceptions().contains((words + i)->c_str()))
                    {
                    return std::make_pair(false, phrase_comparison_result::phrase_rule_exception);
                    }
                }
            return std::make_pair(true, phrase_comparison_result::phrase_equal);
            }

        /** @returns Whether or not the phrase is empty (i.e., doesn't have any words).*/
        [[nodiscard]]
        bool is_empty() const noexcept
            {
            return m_words.empty();
            }

        /** @returns The number of words in the phrase.*/
        [[nodiscard]]
        size_t get_word_count() const noexcept
            {
            return m_words.size();
            }

        /** @returns The words in the phrase as a vector of strings.*/
        [[nodiscard]]
        const std::vector<word_typeT>& get_words() const noexcept
            {
            return m_words;
            }

        /** @returns Direct access to the internal vector of strings.*/
        [[nodiscard]]
        std::vector<word_typeT>& get_words() noexcept
            {
            return m_words;
            }

        /** @returns The phrase formatted into a string.*/
        [[nodiscard]]
        traits::case_insensitive_wstring_ex to_string() const
            {
            traits::case_insensitive_wstring_ex fullString;
            for (size_t i = 0; i < get_word_count(); ++i)
                {
                fullString += m_words[i].c_str();
                fullString += L' ';
                }
            if (!fullString.empty())
                {
                fullString.erase(fullString.length() - 1);
                }
            return fullString;
            }

        /** @brief Sets the trailing exceptions, which are used in equal_to_words().
            @details If equal_to_words() determines if a sequence of strings
                matches the phrase, then it will be equal unless the next word in
                the sequence is one of these trailing exceptions.
                This helps prevent false positives when searching for grammar errors.
            @param except The trailing exception to use.*/
        void set_trailing_exceptions(const std::set<word_typeT>& except)
            {
            m_trailing_exceptions = except;
            }

        /// @private
        void set_trailing_exceptions(std::set<word_typeT>&& except)
            {
            m_trailing_exceptions = std::move(except);
            }

        /** @returns The trailing exception.*/
        [[nodiscard]]
        const std::set<word_typeT>& get_trailing_exceptions() const noexcept
            {
            return m_trailing_exceptions;
            }

        /** @returns The trailing exception.*/
        [[nodiscard]]
        std::set<word_typeT>& get_trailing_exceptions() noexcept
            {
            return m_trailing_exceptions;
            }

        /** @brief Sets the preceding exceptions, which are used in equal_to_words().
            @details If equal_to_words() determines if a sequence of strings matches
                the phrase, then it will be equal unless the word before the sequence
                is one of these preceding exceptions.
                This helps prevent false positives when searching for grammar errors.
            @param except The preceding exception to use.*/
        void set_preceding_exceptions(const std::set<word_typeT>& except)
            {
            m_preceding_exceptions = except;
            }

        /// @private
        void set_preceding_exceptions(std::set<word_typeT>&& except)
            {
            m_preceding_exceptions = std::move(except);
            }

        /** @returns The preceding exception.*/
        [[nodiscard]]
        const std::set<word_typeT>& get_preceding_exceptions() const noexcept
            {
            return m_preceding_exceptions;
            }

        /** @returns The preceding exception.*/
        [[nodiscard]]
        std::set<word_typeT>& get_preceding_exceptions() noexcept
            {
            return m_preceding_exceptions;
            }

      private:
        std::vector<word_typeT> m_words;
        phrase_type m_phrase_type{ phrase_type::phrase_wordy };
        std::set<word_typeT> m_trailing_exceptions;
        std::set<word_typeT> m_preceding_exceptions;
        };

    /** Wrapper for a collection of phrases that can be easily searched,
            compared against (with variable word-length phrases), sorted, and load from a file.*/
    class phrase_collection
        {
      public:
        using phrase_word_pair = comparable_first_pair<phrase<traits::case_insensitive_wstring_ex>,
                                                       traits::case_insensitive_wstring_ex>;
        constexpr static size_t npos = static_cast<size_t>(-1);

        /** @brief Compares a range of words to see if it matches any phrases in this collection.
            @details The word range can be bigger than the phrases,
                we just want to compare the phrases against the first few words.
            @param words An iterator to a container of words
                (should have the same interface as `std::basic_string`).
            @param position The position in the sentence of word sequence.
                If this is zero (meaning the passed in sequence is the start of the sentence),
                then the preceding exception logic will not be used.
            @param max_word_count The maximum number of words from the sequence to compare against.
                This would usually be the number of words in the sequence until the
                end of the sentence.
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
                return std::string::npos;
                }
            phrase<traits::case_insensitive_wstring_ex> searchPhrase;
            searchPhrase.set_type(phrase_type::phrase_wordy);
            searchPhrase.add_word(words[0].c_str());
            // try to make the minimal search key from the first two words from the text
            // if we are only looking for 2+ word phrases (instead of including single words)
            if (!allow_one_word_phrase)
                {
                searchPhrase.add_word(words[1].c_str());
                }
            const phrase_word_pair searchValue(searchPhrase, traits::case_insensitive_wstring_ex());

            // find the first phrase that begins with the first word(s)
            auto foundPhrase = std::lower_bound(m_phrases.cbegin(), m_phrases.cend(), searchValue);
            if (foundPhrase == m_phrases.end())
                {
                return npos;
                }
            while (foundPhrase != m_phrases.end() && foundPhrase->first.get_words()[0] == words[0])
                {
                if (foundPhrase->first.equal_to_words(words, position, max_word_count).first)
                    {
                    // look ahead to see if there are any bigger phrases that match these
                    // words before returning this index
                    auto nextPhrase = foundPhrase + 1;
                    while (nextPhrase != m_phrases.end() &&
                           nextPhrase->first.get_words()[0] == words[0])
                        {
                        const auto nextPhraseCompareResult =
                            nextPhrase->first.equal_to_words(words, position, max_word_count);
                        // If the next phrase matches, then move the found iterator.
                        // Note that we will keep going though in case there is yet another larger,
                        // matching phrase.
                        if (nextPhraseCompareResult.first)
                            {
                            foundPhrase = nextPhrase++;
                            }
                        // The current phrase in the list doesn't match the rest of the text,
                        // but it's either lesser than it (lexicographically),
                        // or it short-circuited by a rule or the next phrase is too long.
                        // Move onto the next phrase in the list and see if that matches.
                        else if (nextPhraseCompareResult.second !=
                                 phrase_comparison_result::phrase_greater_than)
                            {
                            ++nextPhrase;
                            }
                        // If the next phrase in the list is greater than this text block,
                        // then we are finally done.
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

        /** @returns A vector of the phrases.*/
        [[nodiscard]]
        const std::vector<phrase_word_pair>& get_phrases() const noexcept
            {
            return m_phrases;
            }

        /** @brief Loads phrases from a text stream.
            Each row in this text should be a phrase, and the column values (tab-delimited) are:
            - Phrase
            - Suggestion (optional)
            - Type (optional, should be the numeric equivalent of a phrase_type)
            - Preceding exceptions (optional, can be multiple values delimited by ';' or ',')
            - Trailing exceptions (optional, can be multiple values delimited by ';' or ',')
            @param text The text stream to load the phrases from.
            @param sort_phrases Whether to sort the phrases after loading them.
                If loading multiple streams, then it is more optimal to set this to @c false
                and to call sort() after loading all other streams.
            @param preserve_phrases Whether phrases already in the list should be kept.
                @c false will clear the old list while loading the new phrases,
                @c true will preserve them.*/
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
            phrase_word_pair newPhrasePair;

            // do a preview of the text and reserve some more memory for the new phrases
            lily_of_the_valley::text_preview preview;
            const size_t lineCount = preview(text, L'\t', true, false);
            if (lineCount == 0)
                {
                return;
                }
            m_phrases.reserve(m_phrases.size() + lineCount);

            // the phrase, the suggestion, its type (optional),
            // and preceding/trailing exceptions (optional)
            std::vector<traits::case_insensitive_wstring_ex> rowStrings(5);
            lily_of_the_valley::standard_delimited_character_column tabbedColumn(
                lily_of_the_valley::text_column_delimited_character_parser{ L'\t' }, 5);
            lily_of_the_valley::text_row<traits::case_insensitive_wstring_ex> row(std::nullopt);
            row.treat_consecutive_delimiters_as_one(false);
            row.add_column(tabbedColumn);
            row.set_values(&rowStrings);

            lily_of_the_valley::standard_delimited_character_column spacedColumn(
                lily_of_the_valley::text_column_delimited_character_parser{ L' ' }, std::nullopt);
            lily_of_the_valley::text_row<traits::case_insensitive_wstring_ex> phraseRow(
                std::nullopt);
            // just grabbing list of words, extra spaces can be ignored
            phraseRow.treat_consecutive_delimiters_as_one(true);
            phraseRow.allow_column_resizing();
            phraseRow.add_column(spacedColumn);
            phraseRow.set_values(&newPhrasePair.first.get_words());

            do
                {
                text = row.read(text);
                if (row.get_number_of_columns_last_read() < 1)
                    {
                    continue;
                    }

                // phrase should have at least one word (normally more than one,
                // but we will be flexible and allow one-word "phrases")
                phraseRow.read(rowStrings[0].c_str());
                if (phraseRow.get_number_of_columns_last_read() < 1)
                    {
                    continue;
                    }

                newPhrasePair.first.resize(phraseRow.get_number_of_columns_last_read());
                // if a suggestion phrase is available (the norm) then include it;
                // otherwise, leave it blank.
                if (row.get_number_of_columns_last_read() >= 2)
                    {
                    newPhrasePair.second = rowStrings[1].c_str();
                    }
                else
                    {
                    newPhrasePair.second = L"";
                    }
                // The type column is optional.
                // They can put 0 (wordy), 1 (redundant), 2 (cliche),
                // or 3 (wording error) in there to classify
                // the phrase. It will default to WORDY if the row doesn't specify this.
                if (row.get_number_of_columns_last_read() < 3)
                    {
                    newPhrasePair.first.set_type(phrase_type::phrase_wordy);
                    }
                else if (rowStrings[2] == L"0")
                    {
                    newPhrasePair.first.set_type(phrase_type::phrase_wordy);
                    }
                else if (rowStrings[2] == L"1")
                    {
                    newPhrasePair.first.set_type(phrase_type::phrase_redundant);
                    }
                else if (rowStrings[2] == L"2")
                    {
                    newPhrasePair.first.set_type(phrase_type::phrase_cliche);
                    }
                else if (rowStrings[2] == L"3")
                    {
                    newPhrasePair.first.set_type(phrase_type::phrase_error);
                    }
                else
                    {
                    newPhrasePair.first.set_type(phrase_type::phrase_wordy);
                    }
                // Preceding exception rule is optional.
                // This would be a word before the phrase that would negate our
                // normal comparison logic.
                // For example, the phrase "I are" is normally wrong, but in the case of
                // "and I are" it is correct. So "and" would be our preceding word exception.
                if (row.get_number_of_columns_last_read() > 3)
                    {
                    string_util::string_tokenize<traits::case_insensitive_wstring_ex> tkzr(
                        rowStrings[3], L";,", true);
                    std::set<traits::case_insensitive_wstring_ex> expts;
                    while (tkzr.has_more_tokens())
                        {
                        auto nTok = tkzr.get_next_token();
                        if (!nTok.empty())
                            {
                            expts.emplace(std::move(nTok));
                            }
                        }
                    newPhrasePair.first.set_preceding_exceptions(std::move(expts));
                    }
                else
                    {
                    newPhrasePair.first.get_preceding_exceptions().clear();
                    }
                // Trailing exception rule is optional.
                // This would be a word after the phrase that would negate our
                // normal comparison logic. For example, the phrase "could of" is normally
                // meant to be "could have", but in the case of "could of course" it is correct.
                // So "course" would be our trailing word exception.
                if (row.get_number_of_columns_last_read() > 4)
                    {
                    string_util::string_tokenize<traits::case_insensitive_wstring_ex> tkzr(
                        rowStrings[4], L";,", true);
                    std::set<traits::case_insensitive_wstring_ex> expts;
                    while (tkzr.has_more_tokens())
                        {
                        auto nTok = tkzr.get_next_token();
                        if (!nTok.empty())
                            {
                            expts.emplace(std::move(nTok));
                            }
                        }
                    newPhrasePair.first.set_trailing_exceptions(std::move(expts));
                    }
                else
                    {
                    newPhrasePair.first.get_trailing_exceptions().clear();
                    }

                m_phrases.push_back(newPhrasePair);
                } while (text != nullptr);

            if (sort_phrases)
                {
                sort();
                }
            }

        /// @returns The phrases, formatted as an exportable text file.
        [[nodiscard]]
        std::wstring to_string() const
            {
            std::wstring outputStr;
            std::wstring expStr;
            for (const auto& phrase : get_phrases())
                {
                outputStr += phrase.first.to_string().c_str();
                // don't bother exporting blank columns
                if (!phrase.second.empty() || !phrase.first.get_preceding_exceptions().empty() ||
                    !phrase.first.get_trailing_exceptions().empty())
                    {
                    outputStr += L'\t';
                    outputStr += phrase.second.c_str();
                    outputStr += L'\t';
                    outputStr += std::to_wstring(static_cast<int>(phrase.first.get_type()));
                    outputStr += L'\t';
                    expStr.clear();
                    for (const auto& exp : phrase.first.get_preceding_exceptions())
                        {
                        expStr.append(exp.c_str()).append(L";");
                        }
                    if (expStr.ends_with(L";"))
                        {
                        expStr.pop_back();
                        }
                    outputStr += expStr;
                    outputStr += L'\t';

                    expStr.clear();
                    for (const auto& exp : phrase.first.get_trailing_exceptions())
                        {
                        expStr.append(exp.c_str()).append(L";");
                        }
                    if (expStr.ends_with(L";"))
                        {
                        expStr.pop_back();
                        }
                    outputStr += expStr;
                    }
                outputStr += L"\r\n";
                }
            return outputStr;
            }

        /** @brief Sorts the phrases.
            @details If loading multiple lists (from load_phrases()), this it is usually
                optimal to not sort while loading them, but instead sort them afterward.*/
        void sort() { std::sort(m_phrases.begin(), m_phrases.end()); }

        /** @brief Removes all duplicate phrases (the strings in the phrases are compared,
                other fields are ignored).
            @note This will also sort the phrases.*/
        void remove_duplicates()
            {
            std::sort(m_phrases.begin(), m_phrases.end());
            const auto endOfUniquePos = std::unique(m_phrases.begin(), m_phrases.end());
            if (endOfUniquePos != m_phrases.end())
                {
                m_phrases.erase(endOfUniquePos, m_phrases.end());
                }
            }

        /** @brief Removes all phrases from the container.*/
        void clear_phrases() noexcept { m_phrases.clear(); }

        /** @returns @c true if the list is sorted (in ascending order).*/
        [[nodiscard]]
        bool is_sorted() const
            {
            return std::is_sorted(m_phrases.cbegin(), m_phrases.cend());
            }

      protected:
        std::vector<phrase_word_pair> m_phrases;
        };
    } // namespace grammar

#endif // INDEXING_PHRASE_H
