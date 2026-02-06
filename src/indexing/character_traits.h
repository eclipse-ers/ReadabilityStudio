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

#ifndef CHARACTER_TRAITS_H
#define CHARACTER_TRAITS_H

#include "../Wisteria-Dataviz/src/util/string_util.h"
#include "characters.h"
#include <cassert>
#include <string>
#include <string_view>

/// @brief String traits use special comparison logic.
namespace traits
    {
    /** @brief Case-insensitive comparison traits for std::basic_string<>, which also has special
            logic for comparing apostrophes and full-width characters.
        @details Also includes helper functions for case-sensitive comparison which includes the
            additional full-width and apostrophe logic.*/
    class case_insensitive_ex : public std::char_traits<wchar_t>
        {
      public:
        //-------------------------------------------------------------
        constexpr static bool eq_int_type(const int_type& i1, const int_type& i2) noexcept
            {
            return tolower(i1) == tolower(i2);
            }

        //-------------------------------------------------------------
        constexpr static char_type to_char_type(const int_type& i) noexcept
            {
            return static_cast<char_type>(i);
            }

        //-------------------------------------------------------------
        constexpr static int_type to_int_type(const char_type& c) noexcept
            {
            return static_cast<unsigned char>(c);
            }

        //-------------------------------------------------------------
        static bool eq(const char_type& first, const char_type& second) noexcept
            {
            // special logic for apostrophes
            if (characters::is_character::is_apostrophe(first) &&
                characters::is_character::is_apostrophe(second))
                {
                return true;
                }
            return (tolower(first) == tolower(second));
            }

        /// @todo Unit test
        //-------------------------------------------------------------
        constexpr static bool eq_case_sensitive(const char_type& first,
                                                const char_type& second) noexcept
            {
            // special logic for apostrophes
            if (characters::is_character::is_apostrophe(first) &&
                characters::is_character::is_apostrophe(second))
                {
                return true;
                }
            return (string_util::full_width_to_narrow(first) ==
                    string_util::full_width_to_narrow(second));
            }

        //-------------------------------------------------------------
        static bool lt(const char_type& first, const char_type& second) noexcept
            {
            // special logic for apostrophes
            if (characters::is_character::is_apostrophe(first) &&
                characters::is_character::is_apostrophe(second))
                {
                return false;
                }
            if (characters::is_character::is_apostrophe(first))
                {
                return (L'\'' < tolower(second));
                }
            if (characters::is_character::is_apostrophe(second))
                {
                return (tolower(first) < L'\'');
                }
            return (tolower(first) < tolower(second));
            }

        /// @todo unit test!
        //-------------------------------------------------------------
        static bool lt_case_sensitive(const char_type& first, const char_type& second) noexcept
            {
            // special logic for apostrophes
            if (characters::is_character::is_apostrophe(first) &&
                characters::is_character::is_apostrophe(second))
                {
                return false;
                }
            if (characters::is_character::is_apostrophe(first))
                {
                return (L'\'' < string_util::full_width_to_narrow(second));
                }
            if (characters::is_character::is_apostrophe(second))
                {
                return (string_util::full_width_to_narrow(first) < L'\'');
                }
            return (string_util::full_width_to_narrow(first) <
                    string_util::full_width_to_narrow(second));
            }

        /// @todo Unit test
        //-------------------------------------------------------------
        static bool le(const char_type& first, const char_type& second) noexcept
            {
            // special logic for apostrophes
            if (characters::is_character::is_apostrophe(first) &&
                characters::is_character::is_apostrophe(second))
                {
                return false;
                }
            if (characters::is_character::is_apostrophe(first))
                {
                return (L'\'' <= tolower(second));
                }
            if (characters::is_character::is_apostrophe(second))
                {
                return (tolower(first) <= L'\'');
                }
            return (tolower(first) <= tolower(second));
            }

        /// @todo Unit test
        //-------------------------------------------------------------
        static bool ge(const char_type& first, const char_type& second) noexcept
            {
            // special logic for apostrophes
            if (characters::is_character::is_apostrophe(first) &&
                characters::is_character::is_apostrophe(second))
                {
                return false;
                }
            if (characters::is_character::is_apostrophe(first))
                {
                return (L'\'' >= tolower(second));
                }
            if (characters::is_character::is_apostrophe(second))
                {
                return (tolower(first) >= L'\'');
                }
            return (tolower(first) >= tolower(second));
            }

        //-------------------------------------------------------------
        constexpr static char_type tolower(const char_type& ch) noexcept
            {
            return characters::is_character::to_lower(string_util::full_width_to_narrow(ch));
            }

        //-------------------------------------------------------------
        static int compare(const char_type* s1, const char_type* s2, size_t n) noexcept
            {
            assert(s1);
            assert(s2);
            if (s1 == nullptr)
                {
                return -1;
                }
            if (s2 == nullptr)
                {
                return 1;
                }
            for (size_t i = 0; i < n; ++i)
                {
                // cppcheck-suppress arrayIndexOutOfBounds
                if (!eq(s1[i], s2[i]))
                    {
                    return lt(s1[i], s2[i]) ? -1 : 1;
                    }
                }
            return 0;
            }

        //-------------------------------------------------------------
        static int compare_case_sensitive(const char_type* s1, const char_type* s2,
                                          size_t n) noexcept
            {
            assert(s1);
            assert(s2);
            if (s1 == nullptr)
                {
                return -1;
                }
            if (s2 == nullptr)
                {
                return 1;
                }
            for (size_t i = 0; i < n; ++i)
                {
                if (!eq_case_sensitive(s1[i], s2[i]))
                    {
                    return lt_case_sensitive(s1[i], s2[i]) ? -1 : 1;
                    }
                }
            return 0;
            }

        //-------------------------------------------------------------
        static int compare(const std::basic_string_view<char_type> s1, const char_type* s2,
                           size_t n) noexcept
            {
            if (s1.empty())
                {
                return -1;
                }
            if (s2 == nullptr)
                {
                return 1;
                }
            for (size_t i = 0; i < n; ++i)
                {
                // first string isn't long enough, so it is less than the other
                if (i == s1.length())
                    {
                    return -1;
                    }
                if (!eq(s1[i], s2[i]))
                    {
                    return lt(s1[i], s2[i]) ? -1 : 1;
                    }
                }
            return 0;
            }

        //-------------------------------------------------------------
        static int compare_case_sensitive(const std::basic_string_view<char_type> s1,
                                          const char_type* s2, size_t n) noexcept
            {
            if (s1.empty())
                {
                return -1;
                }
            if (s2 == nullptr)
                {
                return 1;
                }
            for (size_t i = 0; i < n; ++i)
                {
                // first string isn't long enough, so it is less than the other
                if (i == s1.length())
                    {
                    return -1;
                    }
                if (!eq_case_sensitive(s1[i], s2[i]))
                    {
                    return lt_case_sensitive(s1[i], s2[i]) ? -1 : 1;
                    }
                }
            return 0;
            }

        //-------------------------------------------------------------
        static const char_type* find(const char_type* s1, size_t n, const char_type ch) noexcept
            {
            assert(s1);
            if (s1 == nullptr)
                {
                return nullptr;
                }
            for (size_t i = 0; i < n; ++i)
                {
                if (eq(s1[i], ch))
                    {
                    return s1 + i;
                    }
                }
            return nullptr;
            }

        //-------------------------------------------------------------
        static const char_type* find(const char_type* s1, size_t n1, const char_type* s2,
                                     size_t n2) noexcept
            {
            assert(n1 && n2);
            assert(s1);
            assert(s2);
            if (s1 == nullptr || s2 == nullptr || (n2 > n1))
                {
                return nullptr;
                }
            size_t j = 1;
            for (size_t i = 0; i < n1; i += j)
                {
                // if the first character of the substring matches then start comparing
                if (eq(s1[i], s2[0]))
                    {
                    // if only looking for one character then return
                    if (n2 == 1)
                        {
                        return s1 + i;
                        }
                    // already know the first chars match, so start at next one
                    for (j = 1; j < n2; ++j)
                        {
                        if (!eq(s1[i + j], s2[j]))
                            {
                            break;
                            }
                        }
                    // if every character matched then return it
                    if (n2 == j)
                        {
                        return s1 + i;
                        }
                    }
                }
            return nullptr;
            }
        };

    using case_insensitive_wstring_ex = std::basic_string<wchar_t, traits::case_insensitive_ex>;
    } // namespace traits

#endif // CHARACTER_TRAITS_H
