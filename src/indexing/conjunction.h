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

#ifndef INDEXING_CONJUNCTION_H
#define INDEXING_CONJUNCTION_H

#include "../Wisteria-Dataviz/src/util/string_util.h"
#include "character_traits.h"
#include <set>
#include <string_view>

namespace grammar
    {
    /// @brief Base class for determining if a word is a coordinating conjunction.
    class is_coordinating_conjunction
        {
      public:
        /// @private
        virtual ~is_coordinating_conjunction() = default;

        /** @brief Determines if a word is a coordinating conjunction.
            @param text The word to review.
            @returns Whether this word is a coordinating conjunction.*/
        virtual bool operator()(const std::wstring_view text) const = 0;

      protected:
        /// @private
        using string_type = std::basic_string_view<wchar_t, traits::case_insensitive_ex>;
        };

    /** @brief Predicate for determining if a word is an
            English coordinating conjunction (case-insensitive).*/
    class is_english_coordinating_conjunction final : public is_coordinating_conjunction
        {
      public:
        /** @brief Determines if a word is an English coordinating conjunction.
            @param text The word to review.
            @returns Whether this word is a coordinating conjunction.*/
        [[nodiscard]]
        bool operator()(const std::wstring_view text) const final
            {
            if (text.empty())
                {
                return false;
                }
            return m_conjunctions.contains(string_type(text.data(), text.length()));
            }

      private:
        static const std::set<string_type> m_conjunctions;
        };

    /** Predicate for determining if a word is a Spanish coordinating
            conjunction (case-insensitive).*/
    class is_spanish_coordinating_conjunction final : public is_coordinating_conjunction
        {
      public:
        /** @brief Determines if a word is a Spanish coordinating conjunction.
            @param text The word to review.
            @returns Whether this word is a coordinating conjunction.*/
        [[nodiscard]]
        bool operator()(const std::wstring_view text) const final
            {
            if (text.empty())
                {
                return false;
                }
            return m_conjunctions.contains(string_type(text.data(), text.length()));
            }

      private:
        static const std::set<string_type> m_conjunctions;
        };

    /** @brief Predicate for determining if a word is a German coordinating
            conjunction (case-insensitive).*/
    class is_german_coordinating_conjunction final : public is_coordinating_conjunction
        {
      public:
        /** @brief Determines if a word is a German coordinating conjunction.
            @param text The word to review.
            @returns Whether this word is a coordinating conjunction.*/
        [[nodiscard]]
        bool operator()(const std::wstring_view text) const final
            {
            if (text.empty())
                {
                return false;
                }
            return m_conjunctions.contains(string_type(text.data(), text.length()));
            }

      private:
        static const std::set<string_type> m_conjunctions;
        };

    /// @brief Predicate for determining if a word is a Russian coordinating
    ///     conjunction (case-insensitive).
    /// @todo add '&'
    /// @todo make this a binary searchable set
    class is_russian_coordinating_conjunction final : public is_coordinating_conjunction
        {
      public:
        /** @brief Determines if a word is a Russian coordinating conjunction.
            @param text The word to review.

            @returns Whether this word is a coordinating conjunction.*/
        [[nodiscard]]
        bool operator()(const std::wstring_view text) const noexcept final
            {
            if (text.empty())
                {
                return false;
                }
            // coordinating conjunctions
            return (text.length() == 1 &&
                    // i (and/both...and)
                    string_util::is_either<wchar_t>(text[0], 0x0418, 0x0438)) ||
                   // a (but)
                   (text.length() == 1 &&
                    string_util::is_either<wchar_t>(text[0], 0x0410, 0x0430)) ||
                   (text.length() == 2 &&
                    string_util::is_either<wchar_t>(text[0], 0x041D, 0x043D) &&
                    // ni (neither...nor)
                    string_util::is_either<wchar_t>(text[1], 0x0418, 0x0438)) ||
                   (text.length() == 2 &&
                    string_util::is_either<wchar_t>(text[0], 0x041D, 0x043D) &&
                    // no (but)
                    string_util::is_either<wchar_t>(text[1], 0x041E, 0x043E)) ||
                   (text.length() == 2 &&
                    string_util::is_either<wchar_t>(text[0], 0x0414, 0x0434) &&
                    // da (and)
                    string_util::is_either<wchar_t>(text[1], 0x0410, 0x0430)) ||
                   (text.length() == 2 &&
                    string_util::is_either<wchar_t>(text[0], 0x0422, 0x0442) &&
                    // to (first...then)
                    string_util::is_either<wchar_t>(text[1], 0x041E, 0x043E)) ||
                   (text.length() == 3 &&
                    string_util::is_either<wchar_t>(text[0], 0x041A, 0x043A) &&
                    string_util::is_either<wchar_t>(text[1], 0x0422, 0x0442) &&
                    // kto (some...other)
                    string_util::is_either<wchar_t>(text[2], 0x041E, 0x043E)) ||
                   (text.length() == 3 &&
                    string_util::is_either<wchar_t>(text[0], 0x0418, 0x0438) &&
                    string_util::is_either<wchar_t>(text[1], 0x041B, 0x043B) &&
                    // ili (either...or)
                    string_util::is_either<wchar_t>(text[2], 0x0418, 0x0438));
            }
        };
    } // namespace grammar

#endif // INDEXING_CONJUNCTION_H
