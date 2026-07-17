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

#ifndef DOLCH_H
#define DOLCH_H

#include "../Wisteria-Dataviz/src/import/text_matrix.h"
#include "../Wisteria-Dataviz/src/util/donttranslate.h"
#include "../Wisteria-Dataviz/src/util/string_util.h"
#include "../indexing/character_traits.h"
#include <algorithm>
#include <set>

namespace readability
    {
    /// @brief A sight word's part of speech.
    enum class sight_word_type
        {
        /// @brief Joins list of words.
        conjunction,
        /// @brief Describes something's locational relation to something else.
        preposition,
        /// @brief He, she, they, etc.
        pronoun,
        /// @brief Describes an action or adjective.
        adverb,
        /// @brief Describes a noun.
        adjective,
        /// @brief Action word.
        verb,
        /// @brief Person, place, or thing.
        noun
        };

    /// @brief A word that an early reader can recognize by sight.
    class sight_word
        {
      public:
        /// @brief The string type for the word.
        using word_type = traits::case_insensitive_wstring_ex;

        /// @brief Constructor.
        /// @param sightWord The string to assign.
        explicit sight_word(word_type sightWord) : m_word(std::move(sightWord)) {}

        /// @brief Constructor.
        /// @param sightWord The string to assign.
        /// @param wordType The word's part of speech.
        sight_word(word_type sightWord, const sight_word_type wordType)
            : m_word(std::move(sightWord)), m_sight_word_type(wordType)
            {
            }

        /// @private
        sight_word() = delete;

        /// @private
        [[nodiscard]]
        bool operator<(const sight_word& that) const noexcept
            {
            return m_word < that.m_word;
            }

        /// @private
        [[nodiscard]]
        const word_type& get_word() const noexcept
            {
            return m_word;
            }

        /// @private
        [[nodiscard]]
        sight_word_type get_type() const noexcept
            {
            return m_sight_word_type;
            }

      private:
        word_type m_word;
        sight_word_type m_sight_word_type{ sight_word_type::noun };
        };

    /// @brief The list of Dolch's Sight Words.
    class dolch_word_list
        {
      public:
        /// @brief The list's string type.
        using word_type = sight_word::word_type;

        /** @brief Loads a block of text as a word list.
            @param text The text to load.
            @details The text should be a two-column, tab-delimited block of text.\n
                The first column is the Dolch word, the second column is its
                part of speech. This should be one of the following:
                - CONJUNCTION
                - PREPOSITION
                - PRONOUN
                - ADVERB
                - ADJECTIVE
                - VERB
        */
        void load_words(const wchar_t* text)
            {
            if (text == nullptr)
                {
                return;
                }
            // the word and classification
            std::vector<sight_word::word_type> rowStrings(2);
            lily_of_the_valley::standard_delimited_character_column tabbedColumn(
                lily_of_the_valley::text_column_delimited_character_parser{ L'\t' }, 2);
            lily_of_the_valley::text_row<sight_word::word_type> row(std::nullopt);
            row.add_column(tabbedColumn);
            row.set_values(&rowStrings);
            // prevent reading empty columns, we should have two filled-in columns
            row.treat_consecutive_delimiters_as_one(true);

            do
                {
                text = row.read(text);
                if (row.get_number_of_columns_last_read() < 2)
                    {
                    continue;
                    }

                sight_word_type sightWord{ sight_word_type::noun };
                if (string_util::stricmp(rowStrings[1].c_str(),
                                         _DT(L"CONJUNCTION", DTExplanation::InternalKeyword)) == 0)
                    {
                    sightWord = sight_word_type::conjunction;
                    }
                else if (string_util::stricmp(rowStrings[1].c_str(), _DT(L"PREPOSITION")) == 0)
                    {
                    sightWord = sight_word_type::preposition;
                    }
                else if (string_util::stricmp(rowStrings[1].c_str(), _DT(L"PRONOUN")) == 0)
                    {
                    sightWord = sight_word_type::pronoun;
                    }
                else if (string_util::stricmp(rowStrings[1].c_str(), _DT(L"ADVERB")) == 0)
                    {
                    sightWord = sight_word_type::adverb;
                    }
                else if (string_util::stricmp(rowStrings[1].c_str(), _DT(L"ADJECTIVE")) == 0)
                    {
                    sightWord = sight_word_type::adjective;
                    }
                else if (string_util::stricmp(rowStrings[1].c_str(), _DT(L"VERB")) == 0)
                    {
                    sightWord = sight_word_type::verb;
                    }
                else
                    {
                    sightWord = sight_word_type::noun;
                    }
                m_sight_words.insert(sight_word(rowStrings[0], sightWord));
                } while (text != nullptr);
            }

        /// @private
        [[nodiscard]]
        std::set<sight_word>::const_iterator operator()(const wchar_t* s_word) const
            {
            return m_sight_words.find(sight_word(s_word, sight_word_type::noun));
            }

        /// @private
        [[nodiscard]]
        const std::set<sight_word>& get_words() const noexcept
            {
            return m_sight_words;
            }

        /// @private
        [[nodiscard]]
        size_t get_list_size() const noexcept
            {
            return m_sight_words.size();
            }

      private:
        std::set<sight_word> m_sight_words;
        };

    /** @brief Functor that returns @c true if a word is a dolch word
            (caller needs to supply a dolch list).*/
    template<typename word_typeT>
    class is_dolch_word
        {
      public:
        /// @brief Constructor.
        /// @param wlist The Dolch word list to use.
        /// @note This functor will **not** take ownership of this.
        explicit is_dolch_word(const readability::dolch_word_list* wlist)
            : m_wordlist(wlist), m_currentFoundPos(wlist->get_words().end())
            {
            }

        /** @brief Interface to search for a word on the list.
            @param the_word The word to search for.
            @returns @c true if the word being searched for is on the list.
            @sa get_last_search_result().*/
        [[nodiscard]]
        bool operator()(const word_typeT& the_word) const
            {
            m_currentFoundPos = m_wordlist->operator()(the_word.c_str());
            return (m_currentFoundPos != m_wordlist->get_words().end());
            }

        /// @returns An iterator to the last searched for word.
        [[nodiscard]]
        std::set<readability::sight_word>::const_iterator get_last_search_result() const noexcept
            {
            return m_currentFoundPos;
            }

        /// @returns The internal word list.
        [[nodiscard]]
        const readability::dolch_word_list* get_word_list() const noexcept
            {
            return m_wordlist;
            }

        /// @brief Sets the internal word list to compare other words against.
        /// @param wlist The word list to use.
        /// @note This functor will **not** take ownership of this.
        void set_word_list(const readability::dolch_word_list* wlist) { m_wordlist = wlist; }

      private:
        const readability::dolch_word_list* m_wordlist{ nullptr };
        mutable std::set<readability::sight_word>::const_iterator m_currentFoundPos;
        };
    } // namespace readability

#endif // DOLCH_H
