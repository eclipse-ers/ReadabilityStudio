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

#ifndef WORD_LIST_H
#define WORD_LIST_H

#include "../Wisteria-Dataviz/src/import/text_matrix.h"
#include "../Wisteria-Dataviz/src/import/text_preview.h"
#include "../Wisteria-Dataviz/src/util/string_util.h"
#include "character_traits.h"
#include <algorithm>
#include <map>

/** @brief Container class for encapsulating a list of words.*/
class word_list
    {
  public:
    /// @private
    word_list() = default;
    /// @private
    word_list(const word_list&) = delete;
    /// @private
    word_list& operator=(const word_list&) = delete;
    /// @private
    using word_type = traits::case_insensitive_wstring_ex;

    /** @brief Loads the words from a text stream.
        @details Words are delimited by any whitespace and are optionally sorted.
        @param text The text stream containing the words to parse.
        @param sort_list Whether the list of words should be sorted after
            loaded from the text stream.
        @param preserve_words Whether words already in the list should be kept.
            @c false will clear the old list while loading the new words,
            @c true will preserve them.*/
    void load_words(const wchar_t* text, const bool sort_list, const bool preserve_words)
        {
        if (!preserve_words)
            {
            m_words.clear();
            }

        if (text == nullptr)
            {
            return;
            }

        string_util::string_tokenize<word_type> tkzr(text, L" \t\n\r", true);
        m_words.reserve(tkzr.count_tokens(text));
        while (tkzr.has_more_tokens())
            {
            m_words.emplace_back(tkzr.get_next_token());
            }

        if (sort_list)
            {
            sort();
            }
        }

    /** @returns The internal word list as a vector.*/
    [[nodiscard]]
    const std::vector<word_type>& get_words() const noexcept
        {
        return m_words;
        }

    /** @returns The number of words in the list.*/
    [[nodiscard]]
    size_t get_list_size() const noexcept
        {
        return m_words.size();
        }

    /** @brief Determines if a given string is in the list.
        @param theWord The word to search for.
        @returns @c true if the word is found.
        @warning The list should be sorted before calling this.*/
    [[nodiscard]]
    bool contains(std::wstring_view theWord) const
        {
        return std::binary_search(get_words().cbegin(), get_words().cend(),
                                  word_type(theWord.data(), theWord.length()));
        }

    /** @brief Adds a word to the list, inserted at the proper sorted position.
        @details This assumes that the word list has already been sorted.
        @param theWord The word to be added.*/
    void add_word(const word_type& theWord)
        {
        const auto insertionPoint = std::lower_bound(m_words.cbegin(), m_words.cend(), theWord);
        m_words.insert(insertionPoint, theWord);
        }

    /** @brief Adds a vector of words and sorts them in.
        @param theWords A list of words to add.*/
    void add_words(const std::vector<word_type>& theWords)
        {
        const size_t previousSize = get_list_size();
        m_words.resize(m_words.size() + theWords.size());
        std::ranges::copy(theWords, m_words.begin() + previousSize);
        sort();
        }

    /** @brief Sorts the word list (in A-Z [ascending] order).*/
    void sort() noexcept { std::ranges::sort(m_words); }

    /** @brief Sorts and removes any duplicate words in the list.*/
    void remove_duplicates()
        {
        sort();
        const auto endOfUniquePos = std::unique(m_words.begin(), m_words.end());
        if (endOfUniquePos != m_words.end())
            {
            m_words.erase(endOfUniquePos, m_words.end());
            }
        }

    /** @brief Clears the word list.*/
    void clear() noexcept { m_words.clear(); }

    /** @returns Whether the list is sorted (in ascending order).*/
    [[nodiscard]]
    bool is_sorted() const
        {
        if (m_words.size() <= 1)
            {
            return true;
            }
        for (size_t i = 0; i < m_words.size() - 1; ++i)
            {
            if (m_words[i] > m_words[i + 1])
                {
                return false;
                }
            }
        return true;
        }

  private:
    std::vector<word_type> m_words;
    };

/** @brief Container class for encapsulating a list of words, with suggested replacements.*/
class word_list_with_replacements
    {
  public:
    /// @private
    word_list_with_replacements() = default;
    /// @private
    word_list_with_replacements(const word_list&) = delete;
    /// @private
    word_list_with_replacements& operator=(const word_list_with_replacements&) = delete;
    /// @private
    using word_type = traits::case_insensitive_wstring_ex;

    /** @brief Loads the words from a text stream.
        @details Words appear in two columns, the first column the key word and
            the second column its synonym.\n
            Columns must be delimited by a tab.
        @param text The text stream containing the words to parse.
        @param preserve_words Whether words already in the list should be kept.
            @c false will clear the old list while loading the new words,
            @c true will preserve them.*/
    void load_words(const wchar_t* text, const bool preserve_words)
        {
        if (!preserve_words)
            {
            clear();
            }

        if (text == nullptr)
            {
            return;
            }
        std::vector<std::vector<word_type>> words;

        // import two columns of text (tab delimited)
        lily_of_the_valley::standard_delimited_character_column tabbedColumn(
            lily_of_the_valley::text_column_delimited_character_parser{ L'\t' }, 2);
        lily_of_the_valley::text_row<word_type> row(std::nullopt);
        row.add_column(tabbedColumn);
        // just reading in a single list of words, so ignore extra tabs and spaces and whatnot
        row.treat_consecutive_delimiters_as_one(true);

        // define the rows
        lily_of_the_valley::text_matrix<word_type> importer(&words);
        importer.add_row_definition(row);

        // preview, read, and write
        lily_of_the_valley::text_preview importPreview;
        const size_t rowCount = importPreview(text, L'\t', true, false);
        importer.read(text, rowCount, 2, true);
        for (const auto& word : words)
            {
            m_word_map.insert(std::make_pair(word.at(0), word.at(1)));
            }
        }

    /** @brief Searches for a word in the list.
        @returns @c true (with its respective replacement) if found,
            @c false and blank string otherwise.*/
    [[nodiscard]]
    std::pair<bool, word_type> find(const word_type& word_to_find) const
        {
        const auto pos = m_word_map.find(word_to_find);
        if (pos == m_word_map.cend())
            {
            return std::make_pair(false, word_type(L""));
            }
        return std::make_pair(true, pos->second);
        }

    /** @returns The internal word list as a map.*/
    [[nodiscard]]
    const std::map<word_type, word_type>& get_words() const noexcept
        {
        return m_word_map;
        }

    /** @returns The number of words in the map.*/
    [[nodiscard]]
    size_t get_list_size() const noexcept
        {
        return m_word_map.size();
        }

    /** @brief Clears the word map.*/
    void clear() noexcept { m_word_map.clear(); }

  private:
    std::map<word_type, word_type> m_word_map;
    };

#endif // WORD_LIST_H
