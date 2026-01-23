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

#include "diacritics.h"

namespace grammar
    {
    std::map<wchar_t, std::wstring_view> convert_ligatures_and_diacritics::m_ligatures = {
        { 0xFB00 /* ﬀ */, std::wstring_view{ L"ff" } },
        { 0xFB01 /* ﬁ */, std::wstring_view{ L"fi" } },
        { 0xFB02 /* ﬂ */, std::wstring_view{ L"fl" } },
        { 0xFB03 /* ﬃ */, std::wstring_view{ L"ffi" } },
        { 0xFB04 /* ﬄ */, std::wstring_view{ L"ffl" } },
        { 0xFB05 /* ﬅ */, std::wstring_view{ L"ft" } },
        { 0xFB06 /* ﬆ */, std::wstring_view{ L"st" } }
    };

    std::map<std::pair<wchar_t, wchar_t>, wchar_t>
        convert_ligatures_and_diacritics::m_combined_diacritics = {
            // grave
            { { L'a', 0x0300 }, L'à' },
            { { L'A', 0x0300 }, L'À' },
            { { L'e', 0x0300 }, L'è' },
            { { L'E', 0x0300 }, L'È' },
            { { L'i', 0x0300 }, L'ì' },
            { { L'I', 0x0300 }, L'Ì' },
            { { L'o', 0x0300 }, L'ò' },
            { { L'O', 0x0300 }, L'Ò' },
            { { L'u', 0x0300 }, L'ù' },
            { { L'U', 0x0300 }, L'Ù' },
            // acute
            { { L'a', 0x0301 }, L'á' },
            { { L'A', 0x0301 }, L'Á' },
            { { L'e', 0x0301 }, L'é' },
            { { L'E', 0x0301 }, L'É' },
            { { L'i', 0x0301 }, L'í' },
            { { L'I', 0x0301 }, L'Í' },
            { { L'o', 0x0301 }, L'ó' },
            { { L'O', 0x0301 }, L'Ó' },
            { { L'u', 0x0301 }, L'ú' },
            { { L'U', 0x0301 }, L'Ú' },
            { { L'y', 0x0301 }, L'ý' },
            { { L'Y', 0x0301 }, L'Ý' },
            { { L'c', 0x0301 }, L'ć' },
            { { L'C', 0x0301 }, L'Ć' },
            { { L's', 0x0301 }, L'ś' },
            { { L'S', 0x0301 }, L'Ś' },
            { { L'z', 0x0301 }, L'ź' },
            { { L'Z', 0x0301 }, L'Ź' },
            // circumflex ("hat")
            { { L'a', 0x0302 }, L'â' },
            { { L'A', 0x0302 }, L'Â' },
            { { L'e', 0x0302 }, L'ê' },
            { { L'E', 0x0302 }, L'Ê' },
            { { L'i', 0x0302 }, L'î' },
            { { L'I', 0x0302 }, L'Î' },
            { { L'o', 0x0302 }, L'ô' },
            { { L'O', 0x0302 }, L'Ô' },
            { { L'u', 0x0302 }, L'û' },
            { { L'U', 0x0302 }, L'Û' },
            // diaeresis (umlauts, trema)
            { { L'a', 0x0308 }, L'ä' },
            { { L'A', 0x0308 }, L'Ä' },
            { { L'e', 0x0308 }, L'ë' },
            { { L'E', 0x0308 }, L'Ë' },
            { { L'i', 0x0308 }, L'ï' },
            { { L'I', 0x0308 }, L'Ï' },
            { { L'o', 0x0308 }, L'ö' },
            { { L'O', 0x0308 }, L'Ö' },
            { { L'u', 0x0308 }, L'ü' },
            { { L'U', 0x0308 }, L'Ü' },
            // dot above
            { { L'z', 0x0307 }, L'ż' },
            { { L'Z', 0x0307 }, L'Ż' },
            // ring
            { { L'a', 0x030A }, L'å' },
            { { L'A', 0x030A }, L'Å' },
            { { L'u', 0x030A }, L'ů' },
            { { L'U', 0x030A }, L'Ů' },
            // caron
            { { L'c', 0x030C }, L'č' },
            { { L'C', 0x030C }, L'Č' },
            { { L'n', 0x030C }, L'ň' },
            { { L'N', 0x030C }, L'Ň' },
            { { L'r', 0x030C }, L'ř' },
            { { L'R', 0x030C }, L'Ř' },
            { { L's', 0x030C }, L'š' },
            { { L'S', 0x030C }, L'Š' },
            { { L'z', 0x030C }, L'ž' },
            { { L'Z', 0x030C }, L'Ž' },
            { { L't', 0x030C }, L'ť' },
            { { L'T', 0x030C }, L'Ť' },
            { { L'd', 0x030C }, L'ď' },
            { { L'D', 0x030C }, L'Ď' },
            { { L'e', 0x030C }, L'ě' },
            { { L'E', 0x030C }, L'Ě' },
            // short solidus
            { { L'l', 0x0337 }, L'ł' },
            { { L'L', 0x0337 }, L'Ł' },
            // long solidus
            { { L'o', 0x0338 }, L'ø' },
            { { L'O', 0x0338 }, L'Ø' },
            // tilde
            { { L'n', 0x0303 }, L'ñ' },
            { { L'N', 0x0303 }, L'Ñ' },
            // retroflex hook below
            { { L'e', 0x0322 }, L'ę' },
            { { L'E', 0x0322 }, L'Ę' },
            // cedilla
            { { L'c', 0x0327 }, L'ç' },
            { { L'C', 0x0327 }, L'Ç' },
            // dot below (Vietnamese)
            { { L'o', 0x0323 }, L'\x1ECD' },
            { { L'O', 0x0323 }, L'\x1ECC' }
        };

    //-------------------------------------------------------------
    bool convert_ligatures_and_diacritics::operator()(std::wstring_view input)
        {
        m_convertedBuffer.clear();
        if (input.empty())
            {
            return false;
            }

        size_t lastStart{ 0 };
        for (size_t i = 0; i < input.length(); /* handled in loop */)
            {
            // if a ligature, replace with the two- or three-character ASCII sequence
            if (const auto ligPos = m_ligatures.find(input[i]); ligPos != m_ligatures.cend())
                {
                m_convertedBuffer.append(input.substr(lastStart, i - lastStart))
                    .append(ligPos->second);
                lastStart = ++i;
                continue;
                }
            // ...otherwise, review the current two-character sequence...
            if ((i + 1) < input.length())
                {
                // ...if an ASCII character and diacritic sequence...
                const auto diaPos =
                    m_combined_diacritics.find(std::make_pair(input[i], input[i + 1]));
                if (diaPos != m_combined_diacritics.cend())
                    {
                    // ...replace with the corresponding accented character
                    // and step over the two-character sequence
                    m_convertedBuffer.append(input.substr(lastStart, i - lastStart));
                    m_convertedBuffer += diaPos->second;
                    i += 2;
                    lastStart = i;
                    continue;
                    }
                // nothing to convert, keep going
                ++i;
                continue;
                }
            // on the last character, just increment and then end the loop
            ++i;
            }

        // copy over remainder (if other content was copied)
        if (!m_convertedBuffer.empty() && lastStart < input.length())
            {
            m_convertedBuffer.append(input.substr(lastStart));
            }

        // return whether anything was converted and copied to the buffer
        return !m_convertedBuffer.empty();
        }
    } // namespace grammar
