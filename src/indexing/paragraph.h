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

#ifndef INDEXING_PARAGRAPH_H
#define INDEXING_PARAGRAPH_H

#include "sentence.h"

namespace grammar
    {
    /** @brief Paragraph information structure.*/
    class paragraph_info
        {
      public:
        /** @brief Constructor
            @param begin_index The starting sentence of the paragraph (by index into the document).
            @param end_index The ending sentence of the paragraph (by index into the document).
            @param leading_end_of_line_count The number of newlines in front of this paragraph.
            @param ends_with_complete_sentence Whether the last sentence is properly terminated.*/
        paragraph_info(const size_t begin_index, const size_t end_index,
                       const size_t leading_end_of_line_count,
                       const bool ends_with_complete_sentence) noexcept
            : m_begin_index(begin_index), m_end_index(end_index),
              m_size((end_index - begin_index) + 1),
              m_leading_end_of_line_count(leading_end_of_line_count),
              m_ends_with_complete_sentence(ends_with_complete_sentence)
            {
            if (get_sentence_count() <= 1 && !m_ends_with_complete_sentence)
                {
                m_is_valid = false;
                m_para_type = sentence_paragraph_type::incomplete;
                }
            else
                {
                m_is_valid = true;
                m_para_type = sentence_paragraph_type::complete;
                }
            }

        /// @private
        paragraph_info() = delete;

        // used for sorting
        /// @private
        [[nodiscard]]
        bool operator<(const size_t sentence_position) const noexcept
            {
            return m_begin_index < sentence_position;
            }

        /** @returns Whether the specified sentence (by index into the document)
                is inside of this paragraph.
            @param sentence_position The sentence index to see if it is in this paragraph.*/
        [[nodiscard]]
        bool contains_sentence(const size_t sentence_position) const noexcept
            {
            return sentence_position >= m_begin_index && sentence_position <= m_end_index;
            }

        /** @returns The index (into the document) of the first sentence in this paragraph.*/
        [[nodiscard]]
        size_t get_first_sentence_index() const noexcept
            {
            return m_begin_index;
            }

        /** @returns The index (into the document) of the last sentence in this paragraph.*/
        [[nodiscard]]
        size_t get_last_sentence_index() const noexcept
            {
            return m_end_index;
            }

        /// @returns The number of sentences in the paragraph.
        [[nodiscard]]
        size_t get_sentence_count() const noexcept
            {
            return m_size;
            }

        /** @returns The number of newlines in front of this paragraph.*/
        [[nodiscard]]
        size_t get_leading_end_of_line_count() const noexcept
            {
            return m_leading_end_of_line_count;
            }

        /** @brief Sets whether this paragraph contains any valid sentences or not.
            @param valid Whether the paragraph is valid or not.*/
        void set_valid(const bool valid) noexcept
            {
            m_is_valid = valid;
            // If is valid now, but previous type was incomplete, then set it to complete.
            // If it was a header or list item, then just leave the type alone.
            if (is_valid() && get_type() == sentence_paragraph_type::incomplete)
                {
                set_type(sentence_paragraph_type::complete);
                }
            // ...and vice versa.
            else if (!is_valid() && get_type() == sentence_paragraph_type::complete)
                {
                set_type(sentence_paragraph_type::incomplete);
                }
            }

        /** @returns Whether this paragraph contains any valid sentences or not.*/
        [[nodiscard]]
        bool is_valid() const noexcept
            {
            return m_is_valid;
            }

        /** @brief Sets what type of paragraph this is in terms of being complete or header, etc...
            @param type The type to set this paragraph to.*/
        void set_type(const sentence_paragraph_type type) noexcept { m_para_type = type; }

        /** @returns What type of paragraph this is in terms of being complete or header, etc...*/
        [[nodiscard]]
        sentence_paragraph_type get_type() const noexcept
            {
            return m_para_type;
            }

      private:
        size_t m_begin_index{ 0 };
        size_t m_end_index{ 0 };
        size_t m_size{ 0 };
        size_t m_leading_end_of_line_count{ 0 };
        bool m_is_valid{ false };
        bool m_ends_with_complete_sentence{ false };
        sentence_paragraph_type m_para_type{ sentence_paragraph_type::complete };
        };
    } // namespace grammar

#endif // INDEXING_PARAGRAPH_H
