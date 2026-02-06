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

#include "abbreviation.h"
#include "../OleanderStemmingLibrary/src/common_lang_constants.h"
#include "../Wisteria-Dataviz/src/util/string_util.h"
#include "character_traits.h"
#include "characters.h"
#include <algorithm>

namespace grammar
    {
    word_list is_abbreviation::m_abbreviations;
    word_list is_abbreviation::m_nonAbbreviations;

    //------------------------------------------
    bool is_abbreviation::operator()(const std::wstring_view text) const
        {
        if (text.empty())
            {
            return false;
            }

        word_list::word_type cmpKey(text.data(), text.length());
        // should this word never be an abbreviation?
        if (std::ranges::binary_search(get_non_abbreviations().get_words(), cmpKey))
            {
            return false;
            }

        if (characters::is_character::is_consonant(text[0]))
            {
            size_t i{ 1 }; // NOLINT
            // go through everything (except the last character) and stop if we hit something other
            // than a consonant
            for (i = 1; i < text.length() - 1; ++i)
                {
                // if [consonant][consonant].[more letters].
                // then this will be a "compound abbreviation" (e.g., "std.err.").
                if (text.length() >= 5 &&
                    traits::case_insensitive_ex::eq(text[i], common_lang_constants::PERIOD) &&
                    i > 1)
                    {
                    return true;
                    }
                // we would want the rest of the consonants to be lowercase (e.g., Dpt.); otherwise,
                // it might be an acronym.
                if (!characters::is_character::is_lower_consonant(text[i]))
                    {
                    break;
                    }
                }
            // if everything up to the last character is a consonant, the last char is a period, and
            // there are at least 4 consonants in front of the period then this more than likely an
            // abbreviation.
            if (text.length() >= 5 && i == (text.length() - 1) &&
                (traits::case_insensitive_ex::eq(text.back(), common_lang_constants::PERIOD)))
                {
                return true;
                }
            }
        // 16:27P.M, 5:00A.M.
        else if (characters::is_character::is_numeric(text[0]) &&
                 (text.length() == 8 || text.length() == 9))
            {
            if (traits::case_insensitive_ex::eq(text[text.length() - 1], L'.') &&
                traits::case_insensitive_ex::eq(text[text.length() - 2], L'M') &&
                traits::case_insensitive_ex::eq(text[text.length() - 3], L'.') &&
                (traits::case_insensitive_ex::eq(text[text.length() - 4], L'P') ||
                 traits::case_insensitive_ex::eq(text[text.length() - 4], L'A')) &&
                characters::is_character::is_numeric(text[text.length() - 5]) &&
                characters::is_character::is_numeric(text[text.length() - 6]) &&
                traits::case_insensitive_ex::eq(text[text.length() - 7], L':') &&
                characters::is_character::is_numeric(text[text.length() - 8]))
                {
                return true;
                }
            }
        // 12P.M., 5A.M.
        else if (characters::is_character::is_numeric(text[0]) &&
                 (text.length() == 5 || text.length() == 6))
            {
            if (traits::case_insensitive_ex::eq(text[text.length() - 1], L'.') &&
                traits::case_insensitive_ex::eq(text[text.length() - 2], L'M') &&
                traits::case_insensitive_ex::eq(text[text.length() - 3], L'.') &&
                (traits::case_insensitive_ex::eq(text[text.length() - 4], L'P') ||
                 traits::case_insensitive_ex::eq(text[text.length() - 4], L'A')) &&
                characters::is_character::is_numeric(text[text.length() - 5]))
                {
                return true;
                }
            }

        bool result = std::ranges::binary_search(get_abbreviations().get_words(), cmpKey);
        // if not found, then try to see if this is more than one word combined by '/',
        // followed by an abbreviation.
        if (!result)
            {
            const size_t lastSlash = text.find_last_of(L'/');
            if (lastSlash != std::wstring_view::npos && lastSlash != text.length() - 1)
                {
                cmpKey.assign(text.data() + (lastSlash + 1), text.length() - (lastSlash + 1));
                result = std::ranges::binary_search(get_abbreviations().get_words(), cmpKey);
                }
            }
        return result;
        }

    //------------------------------------------
    bool is_acronym::operator()(const std::wstring_view text) const noexcept
        {
        m_dot_count = 0;
        m_ends_with_lower_s = false;
        if (text.length() < 2)
            {
            return false;
            }
        size_t letterCount{ 0 }, upperCaseLetterCount{ 0 }, lowerCaseLetterCount{ 0 };
        for (size_t i = 0; i < text.length(); ++i)
            {
            if (text[i] == 0)
                {
                break;
                }
            if (characters::is_character::is_upper(text[i]))
                {
                ++letterCount;
                ++upperCaseLetterCount;
                }
            else if (characters::is_character::is_lower(text[i]))
                {
                ++letterCount;
                ++lowerCaseLetterCount;
                // If first character is a lower case letter then this can't possibly be an
                // acronym. Same goes for if there is more than one lowercased letter.
                if (i == 0 || lowerCaseLetterCount > 1)
                    {
                    return false;
                    }
                }
            else if (traits::case_insensitive_ex::eq(text[i], common_lang_constants::PERIOD))
                {
                ++m_dot_count;
                }
            }
        if (letterCount < 2)
            {
            return false;
            }
        m_ends_with_lower_s =
            (string_util::full_width_to_narrow(text.back()) == common_lang_constants::LOWER_S);
        return (upperCaseLetterCount * 2 > letterCount);
        }

    //------------------------------------------
    bool is_acronym::is_dotted_acronym(const std::wstring_view text) noexcept
        {
        if (text.length() < 4)
            {
            return false;
            }
        size_t i = 0;
        for (; i + 1 < text.length(); i += 2)
            {
            if (characters::is_character::is_alpha(text[i]) &&
                traits::case_insensitive_ex::eq(text[i + 1], common_lang_constants::PERIOD))
                {
                continue;
                }
            break;
            }
        // see if followed by numbers (that can be part of the acronym, such as F.A.K.K.2)
        if (i < text.length())
            {
            while (i < text.length() && characters::is_character::is_numeric(text[i]))
                {
                ++i;
                }
            }
        return (i == text.length());
        }
    } // namespace grammar
