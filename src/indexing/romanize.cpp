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

#include "romanize.h"

namespace text_transform
    {
    const romanization_conversion_table romanize::m_conversionTable;

    //------------------------------------------------
    romanization_conversion_table::romanization_conversion_table()
        : // German letters
          m_replacements{ std::make_pair(static_cast<wchar_t>(0xF6), L"oe"),
                          std::make_pair(static_cast<wchar_t>(0xD6), L"Oe"),
                          std::make_pair(static_cast<wchar_t>(0xFC), L"ue"),
                          std::make_pair(static_cast<wchar_t>(0xDC), L"Ue"),
                          std::make_pair(static_cast<wchar_t>(0xE4), L"ae"),
                          std::make_pair(static_cast<wchar_t>(0xC4), L"Ae"),
                          std::make_pair(static_cast<wchar_t>(0xDF), L"ss"),
                          // Spanish letters (that have a Latin equivalent)
                          std::make_pair(static_cast<wchar_t>(241), L"nn"),
                          std::make_pair(static_cast<wchar_t>(209), L"Nn"),
                          // Scandinavian letters (that have a Latin equivalent)
                          std::make_pair(static_cast<wchar_t>(229), L"aa"),
                          std::make_pair(static_cast<wchar_t>(197), L"Aa"),
                          std::make_pair(static_cast<wchar_t>(198), L"Ae"),
                          std::make_pair(static_cast<wchar_t>(230), L"ae"),
                          std::make_pair(static_cast<wchar_t>(0x0152), L"Oe"),
                          std::make_pair(static_cast<wchar_t>(140), L"Oe"),
                          std::make_pair(static_cast<wchar_t>(0x0153), L"oe"),
                          std::make_pair(static_cast<wchar_t>(156), L"oe"),
                          // characters that regrettably just have to have their accents stripped
                          std::make_pair(static_cast<wchar_t>(192), L"A"),
                          std::make_pair(static_cast<wchar_t>(193), L"A"),
                          std::make_pair(static_cast<wchar_t>(194), L"A"),
                          std::make_pair(static_cast<wchar_t>(195), L"A"),
                          std::make_pair(static_cast<wchar_t>(197), L"A"),
                          std::make_pair(static_cast<wchar_t>(199), L"C"),
                          std::make_pair(static_cast<wchar_t>(200), L"E"),
                          std::make_pair(static_cast<wchar_t>(201), L"E"),
                          std::make_pair(static_cast<wchar_t>(202), L"E"),
                          std::make_pair(static_cast<wchar_t>(203), L"E"),
                          std::make_pair(static_cast<wchar_t>(204), L"I"),
                          std::make_pair(static_cast<wchar_t>(205), L"I"),
                          std::make_pair(static_cast<wchar_t>(206), L"I"),
                          std::make_pair(static_cast<wchar_t>(207), L"I"),
                          std::make_pair(static_cast<wchar_t>(210), L"O"),
                          std::make_pair(static_cast<wchar_t>(211), L"O"),
                          std::make_pair(static_cast<wchar_t>(212), L"O"),
                          std::make_pair(static_cast<wchar_t>(213), L"O"),
                          std::make_pair(static_cast<wchar_t>(216), L"O"),
                          std::make_pair(static_cast<wchar_t>(217), L"U"),
                          std::make_pair(static_cast<wchar_t>(218), L"U"),
                          std::make_pair(static_cast<wchar_t>(219), L"U"),
                          std::make_pair(static_cast<wchar_t>(221), L"Y"),
                          std::make_pair(static_cast<wchar_t>(224), L"a"),
                          std::make_pair(static_cast<wchar_t>(225), L"a"),
                          std::make_pair(static_cast<wchar_t>(226), L"a"),
                          std::make_pair(static_cast<wchar_t>(227), L"a"),
                          std::make_pair(static_cast<wchar_t>(229), L"a"),
                          std::make_pair(static_cast<wchar_t>(231), L"c"),
                          std::make_pair(static_cast<wchar_t>(232), L"e"),
                          std::make_pair(static_cast<wchar_t>(233), L"e"),
                          std::make_pair(static_cast<wchar_t>(234), L"e"),
                          std::make_pair(static_cast<wchar_t>(235), L"e"),
                          std::make_pair(static_cast<wchar_t>(236), L"i"),
                          std::make_pair(static_cast<wchar_t>(237), L"i"),
                          std::make_pair(static_cast<wchar_t>(238), L"i"),
                          std::make_pair(static_cast<wchar_t>(239), L"i"),
                          std::make_pair(static_cast<wchar_t>(242), L"o"),
                          std::make_pair(static_cast<wchar_t>(243), L"o"),
                          std::make_pair(static_cast<wchar_t>(244), L"o"),
                          std::make_pair(static_cast<wchar_t>(245), L"o"),
                          std::make_pair(static_cast<wchar_t>(248), L"o"),
                          std::make_pair(static_cast<wchar_t>(249), L"u"),
                          std::make_pair(static_cast<wchar_t>(250), L"u"),
                          std::make_pair(static_cast<wchar_t>(251), L"u"),
                          std::make_pair(static_cast<wchar_t>(253), L"y"),
                          std::make_pair(static_cast<wchar_t>(255), L"y"),
                          // bullets
                          std::make_pair(static_cast<wchar_t>(0x2022), std::wstring(1, 0xB7)),
                          std::make_pair(static_cast<wchar_t>(149), std::wstring(1, 0xB7)),
                          // smart (single) quotes
                          std::make_pair(static_cast<wchar_t>(0x201A), L"\'"),
                          std::make_pair(static_cast<wchar_t>(0x2039), L"\'"),
                          std::make_pair(static_cast<wchar_t>(0x203A), L"\'"),
                          std::make_pair(static_cast<wchar_t>(0x2018), L"\'"),
                          std::make_pair(static_cast<wchar_t>(0x2019), L"\'"),
                          std::make_pair(static_cast<wchar_t>(96), L"\'"),
                          std::make_pair(static_cast<wchar_t>(130), L"\'"),
                          std::make_pair(static_cast<wchar_t>(139), L"\'"),
                          std::make_pair(static_cast<wchar_t>(145), L"\'"),
                          std::make_pair(static_cast<wchar_t>(146), L"\'"),
                          std::make_pair(static_cast<wchar_t>(155), L"\'"),
                          // smart (double) quotes
                          std::make_pair(static_cast<wchar_t>(0x201E), L"\""),
                          std::make_pair(static_cast<wchar_t>(0x201C), L"\""),
                          std::make_pair(static_cast<wchar_t>(0x201D), L"\""),
                          std::make_pair(static_cast<wchar_t>(132), L"\""),
                          std::make_pair(static_cast<wchar_t>(171), L"\""),
                          std::make_pair(static_cast<wchar_t>(187), L"\""),
                          std::make_pair(static_cast<wchar_t>(147), L"\""),
                          std::make_pair(static_cast<wchar_t>(148), L"\""),
                          // ligatures
                          std::make_pair(static_cast<wchar_t>(0xFB00), L"ff"),
                          std::make_pair(static_cast<wchar_t>(0xFB01), L"fi"),
                          std::make_pair(static_cast<wchar_t>(0xFB02), L"fl"),
                          std::make_pair(static_cast<wchar_t>(0xFB03), L"ffi"),
                          std::make_pair(static_cast<wchar_t>(0xFB04), L"ffl"),
                          std::make_pair(static_cast<wchar_t>(0xFB05), L"ft"),
                          std::make_pair(static_cast<wchar_t>(0xFB06), L"st"),
                          // quneiform-suppress-begin
                          // ellipsis
                          std::make_pair(static_cast<wchar_t>(0x2026), L"..."),
                          std::make_pair(static_cast<wchar_t>(133), L"..."),
                          // quneiform-suppress-end
                          // dashes
                          std::make_pair(static_cast<wchar_t>(0x2012), L"--"),
                          std::make_pair(static_cast<wchar_t>(0x2013), L"--"),
                          std::make_pair(static_cast<wchar_t>(0x2014), L"--"),
                          std::make_pair(static_cast<wchar_t>(0x2015), L"--"),
                          std::make_pair(static_cast<wchar_t>(150), L"--"),
                          std::make_pair(static_cast<wchar_t>(151), L"--"),
                          // soft hyphen
                          std::make_pair(static_cast<wchar_t>(0x00AD), L"-"),
                          // copyright symbols
                          std::make_pair(static_cast<wchar_t>(169), L"(c)"),
                          std::make_pair(static_cast<wchar_t>(174), L"(r)"),
                          std::make_pair(static_cast<wchar_t>(0x2122), L"(tm)"),
                          std::make_pair(static_cast<wchar_t>(153), L"(tm)"),
                          // fractions
                          std::make_pair(static_cast<wchar_t>(188), L"1/4"),
                          std::make_pair(static_cast<wchar_t>(189), L"1/2"),
                          std::make_pair(static_cast<wchar_t>(190), L"3/4"),
                          // other math symbols
                          std::make_pair(static_cast<wchar_t>(177), L"+-") }
        {
        }

    //------------------------------------------------
    std::wstring romanize::operator()(std::wstring_view text,
                                      const bool replace_extended_ascii_characters,
                                      const bool remove_ellipses, const bool remove_bullets,
                                      const bool narrow_full_width_characters) const
        {
        std::wstring encodedText;
        if (text.empty())
            {
            return encodedText;
            }
        encodedText.reserve(text.length() * 2);

        for (size_t i = 0; i < text.length(); ++i)
            {
            // this should be done before anything else because of how it scans ahead
            if (remove_bullets)
                {
                if (i == 0 || text[i] == L'\n' || text[i] == L'\r')
                    {
                    while (i < text.length() && characters::is_character::is_space(text[i]))
                        // copy over any whitespace (including the newline we are on)
                        {
                        encodedText += text[i++];
                        }
                    const std::pair<bool, size_t> bullet = isBullet(text.substr(i));
                    if (bullet.first)
                        {
                        // if there is not a preceding character in front of this bullet
                        // already, then replace the bullet with a tab
                        if (encodedText.empty() || encodedText[encodedText.length() - 1] != L'\t')
                            {
                            encodedText += L'\t';
                            }
                        i += bullet.second; // skip full length of the bullet
                        if (i >= text.length())
                            {
                            break;
                            }
                        }
                    }
                }
            if (remove_ellipses)
                {
                size_t lookAhead = i;
                if (text[i] == 0x2026 || text[i] == 133)
                    {
                    lookAhead = i + 1;
                    }
                else if (text.length() > 2 && i < text.length() - 2 &&
                         traits::case_insensitive_ex::eq(text[i], L'.') &&
                         traits::case_insensitive_ex::eq(text[i + 1], L'.') &&
                         traits::case_insensitive_ex::eq(text[i + 2], L'.'))
                    {
                    lookAhead = i + 3;
                    // skip the following periods when the main loop restarts
                    i += 2;
                    }
                // if on an ellipse, then turn it into a period
                // (for end of sentence) or a space
                if (lookAhead > i)
                    {
                    while (lookAhead < text.length() &&
                           characters::is_character::is_space(text[lookAhead]))
                        {
                        ++lookAhead;
                        }
                    // if ellipsis is a valid sentence terminator, then replace with a period
                    if (lookAhead >= text.length() ||
                        isEndOfSentence.can_character_begin_sentence(text[lookAhead]))
                        {
                        encodedText += L'.';
                        continue;
                        }
                    // otherwise, make it a space
                    encodedText += L' ';
                    continue;
                    }
                }
            if (replace_extended_ascii_characters)
                {
                const auto replacementPos = m_conversionTable.get_table().find(text[i]);
                if (replacementPos != m_conversionTable.get_table().cend())
                    {
                    encodedText += replacementPos->second;
                    }
                else if (narrow_full_width_characters)
                    {
                    encodedText += string_util::full_width_to_narrow(text[i]);
                    }
                else
                    {
                    encodedText += text[i];
                    }
                }
            else if (narrow_full_width_characters)
                {
                encodedText += string_util::full_width_to_narrow(text[i]);
                }
            else
                {
                encodedText += text[i];
                }
            }
        return encodedText;
        }
    } // namespace text_transform
