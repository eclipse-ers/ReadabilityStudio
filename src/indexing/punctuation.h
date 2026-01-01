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

#ifndef INDEXING_PUNCTUATION_H
#define INDEXING_PUNCTUATION_H

#include "character_traits.h"
#include "characters.h"
#include <string_view>

/// @brief Namespace for punctuation classes.
namespace punctuation
    {
    /** @brief Extended version of ispunct, which takes into account special characters.*/
    class is_punctuation
        {
      public:
        /** @returns @c true if a character is punctuation.
            @param ch The character to review.*/
        [[nodiscard]]
        constexpr bool operator()(const wchar_t ch) const noexcept
            {
            return characters::is_character::is_punctuation(ch);
            }
        };

    /** @brief Punctuation counting functor utility.*/
    class punctuation_count
        {
      public:
        /** @brief Counts and returns the number of punctuation marks from character stream.
            @details This is an advanced version of ispunct that takes other Unicode
                punctuation into account, as well as special logic for punctuation only words.
            @param text The character stream to analyze.
            @returns The number of punctuation marks from character stream.
            @note If the stream is just a single punctuation that can be a whole word
                (e.g., an ampersand), then it is treated as NOT being punctuation,
                but rather a word.*/
        [[nodiscard]]
        size_t operator()(std::wstring_view text) const
            {
            if (text.empty())
                {
                return 0;
                }
            /* special case where the word is just ('&', '#', '@', or '%');
               treat it like a regular character (e.g., not punctuation)*/
            if (text.length() == 1 && WHOLE_WORD_PUNCTUATION.find(string_util::full_width_to_narrow(
                                          text[0])) != std::wstring_view::npos)
                {
                return 0;
                }
            return std::ranges::count_if(std::as_const(text),
                                         characters::is_character::is_punctuation);
            }

        /// @brief Punctuation marks can be full words by themselves.
        constexpr static std::wstring_view WHOLE_WORD_PUNCTUATION{ L"&#@%" };
        };

    /** @brief Punctuation mark class. Used in tokenizer to keep track of
            punctuation marks in between words.*/
    class punctuation_mark
        {
      public:
        /** @brief Constructor.
            @param mark The punctuation mark.
            @param wordPosition The position into the parent document of this
                mark's following word.
            @param isConnectedToPreviousWord @c true if this mark is connected
                to the word preceding it (e.g., an apostrophe on a plural possessive word).*/
        punctuation_mark(const wchar_t mark, const size_t wordPosition,
                         const bool isConnectedToPreviousWord) noexcept
            : m_mark(mark), m_word_position(wordPosition),
              m_is_connected_to_previous_word(isConnectedToPreviousWord)
            {
            }

        /// @private
        punctuation_mark() noexcept = default;

        /// @returns The word position (in the document) that this punctuation is in front of.
        [[nodiscard]]
        size_t get_word_position() const noexcept
            {
            return m_word_position;
            }

        /// @returns The actual mark (character).
        [[nodiscard]]
        wchar_t get_punctuation_mark() const noexcept
            {
            return m_mark;
            }

        /// @returns Whether this punctuation is connected to the previous word
        ///     (e.g., a quote symbol), rather than being in front of the current word.
        [[nodiscard]]
        bool is_connected_to_previous_word() const noexcept
            {
            return m_is_connected_to_previous_word;
            }

        /** @brief Useful for doing a binary search/sorting on a collection of marks,
                based on word position.
            @note We compare against word position (instead of the actual mark)
                so that we can sort a collection of punctuation marks by the order
                that they appear in the document.
            @param that The punctuation mark to compare against.
            @returns @c true if this mark's word position is less than that's word position.*/
        [[nodiscard]]
        bool operator<(const punctuation_mark& that) const noexcept
            {
            return m_word_position < that.m_word_position;
            }

        /// @brief Compare against a character as a punctuation mark.
        /// @param mark The character to compare against.
        /// @returns @c true if marks are equal.
        [[nodiscard]]
        constexpr bool operator==(const wchar_t mark) const noexcept
            {
            return m_mark == mark;
            }

      private:
        // mark is in front of this word
        wchar_t m_mark{ 32 }; // space, just generic default type
        size_t m_word_position{ 0 };
        bool m_is_connected_to_previous_word{ false };
        };
    } // namespace punctuation

#endif // INDEXING_PUNCTUATION_H
