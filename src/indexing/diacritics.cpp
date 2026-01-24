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
            { { L'w', 0x0300 }, L'ẁ' },
            { { L'W', 0x0300 }, L'Ẁ' },
            { { L'y', 0x0300 }, L'ỳ' },
            { { L'Y', 0x0300 }, L'Ỳ' },
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
            { { L'n', 0x0301 }, L'ń' },
            { { L'N', 0x0301 }, L'Ń' },
            { { L'l', 0x0301 }, L'ĺ' },
            { { L'L', 0x0301 }, L'Ĺ' },
            { { L'r', 0x0301 }, L'ŕ' },
            { { L'R', 0x0301 }, L'Ŕ' },
            { { L'w', 0x0301 }, L'ẃ' },
            { { L'W', 0x0301 }, L'Ẃ' },
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
            { { L'w', 0x0302 }, L'ŵ' },
            { { L'W', 0x0302 }, L'Ŵ' },
            { { L'y', 0x0302 }, L'ŷ' },
            { { L'Y', 0x0302 }, L'Ŷ' },
            // Esperanto circumflex
            { { L'c', 0x0302 }, L'ĉ' },
            { { L'C', 0x0302 }, L'Ĉ' },
            { { L'g', 0x0302 }, L'ĝ' },
            { { L'G', 0x0302 }, L'Ĝ' },
            { { L'h', 0x0302 }, L'ĥ' },
            { { L'H', 0x0302 }, L'Ĥ' },
            { { L'j', 0x0302 }, L'ĵ' },
            { { L'J', 0x0302 }, L'Ĵ' },
            { { L's', 0x0302 }, L'ŝ' },
            { { L'S', 0x0302 }, L'Ŝ' },
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
            { { L'w', 0x0308 }, L'ẅ' },
            { { L'W', 0x0308 }, L'Ẅ' },
            // French Y with diaeresis
            { { L'y', 0x0308 }, L'ÿ' },
            { { L'Y', 0x0308 }, L'Ÿ' },
            // dot above
            { { L'z', 0x0307 }, L'ż' },
            { { L'Z', 0x0307 }, L'Ż' },
            { { L'c', 0x0307 }, L'ċ' },
            { { L'C', 0x0307 }, L'Ċ' },
            { { L'g', 0x0307 }, L'ġ' },
            { { L'G', 0x0307 }, L'Ġ' },
            { { L'I', 0x0307 }, L'İ' },
            // Lithuanian
            { { L'e', 0x0307 }, L'ė' },
            { { L'E', 0x0307 }, L'Ė' },
            // Old Irish dot above (lenition)
            { { L'b', 0x0307 }, L'ḃ' },
            { { L'B', 0x0307 }, L'Ḃ' },
            { { L'd', 0x0307 }, L'ḋ' },
            { { L'D', 0x0307 }, L'Ḋ' },
            { { L'f', 0x0307 }, L'ḟ' },
            { { L'F', 0x0307 }, L'Ḟ' },
            { { L'm', 0x0307 }, L'ṁ' },
            { { L'M', 0x0307 }, L'Ṁ' },
            { { L'p', 0x0307 }, L'ṗ' },
            { { L'P', 0x0307 }, L'Ṗ' },
            { { L's', 0x0307 }, L'ṡ' },
            { { L'S', 0x0307 }, L'Ṡ' },
            { { L't', 0x0307 }, L'ṫ' },
            { { L'T', 0x0307 }, L'Ṫ' },
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
            { { L'l', 0x030C }, L'ľ' },
            { { L'L', 0x030C }, L'Ľ' },
            // Pinyin caron vowels
            { { L'a', 0x030C }, L'ǎ' },
            { { L'A', 0x030C }, L'Ǎ' },
            { { L'i', 0x030C }, L'ǐ' },
            { { L'I', 0x030C }, L'Ǐ' },
            { { L'o', 0x030C }, L'ǒ' },
            { { L'O', 0x030C }, L'Ǒ' },
            { { L'u', 0x030C }, L'ǔ' },
            { { L'U', 0x030C }, L'Ǔ' },
            // short solidus
            { { L'l', 0x0337 }, L'ł' },
            { { L'L', 0x0337 }, L'Ł' },
            // long solidus
            { { L'o', 0x0338 }, L'ø' },
            { { L'O', 0x0338 }, L'Ø' },
            // short stroke overlay (Maltese, Croatian, Sami)
            { { L'h', 0x0335 }, L'ħ' },
            { { L'H', 0x0335 }, L'Ħ' },
            { { L'd', 0x0335 }, L'đ' },
            { { L'D', 0x0335 }, L'Đ' },
            { { L't', 0x0335 }, L'ŧ' },
            { { L'T', 0x0335 }, L'Ŧ' },
            // tilde (Portuguese, Spanish)
            { { L'a', 0x0303 }, L'ã' },
            { { L'A', 0x0303 }, L'Ã' },
            { { L'n', 0x0303 }, L'ñ' },
            { { L'N', 0x0303 }, L'Ñ' },
            { { L'o', 0x0303 }, L'õ' },
            { { L'O', 0x0303 }, L'Õ' },
            // ogonek (Polish, Lithuanian)
            { { L'a', 0x0328 }, L'ą' },
            { { L'A', 0x0328 }, L'Ą' },
            { { L'e', 0x0328 }, L'ę' },
            { { L'E', 0x0328 }, L'Ę' },
            { { L'i', 0x0328 }, L'į' },
            { { L'I', 0x0328 }, L'Į' },
            { { L'u', 0x0328 }, L'ų' },
            { { L'U', 0x0328 }, L'Ų' },
            // cedilla
            { { L'c', 0x0327 }, L'ç' },
            { { L'C', 0x0327 }, L'Ç' },
            // Latvian cedilla
            { { L'g', 0x0327 }, L'ģ' },
            { { L'G', 0x0327 }, L'Ģ' },
            { { L'k', 0x0327 }, L'ķ' },
            { { L'K', 0x0327 }, L'Ķ' },
            { { L'l', 0x0327 }, L'ļ' },
            { { L'L', 0x0327 }, L'Ļ' },
            { { L'n', 0x0327 }, L'ņ' },
            { { L'N', 0x0327 }, L'Ņ' },
            // dot below (Vietnamese)
            { { L'a', 0x0323 }, L'ạ' },
            { { L'A', 0x0323 }, L'Ạ' },
            { { L'e', 0x0323 }, L'ẹ' },
            { { L'E', 0x0323 }, L'Ẹ' },
            { { L'i', 0x0323 }, L'ị' },
            { { L'I', 0x0323 }, L'Ị' },
            { { L'o', 0x0323 }, L'ọ' },
            { { L'O', 0x0323 }, L'Ọ' },
            { { L'u', 0x0323 }, L'ụ' },
            { { L'U', 0x0323 }, L'Ụ' },
            // macron (long vowels)
            { { L'a', 0x0304 }, L'ā' },
            { { L'A', 0x0304 }, L'Ā' },
            { { L'e', 0x0304 }, L'ē' },
            { { L'E', 0x0304 }, L'Ē' },
            { { L'i', 0x0304 }, L'ī' },
            { { L'I', 0x0304 }, L'Ī' },
            { { L'o', 0x0304 }, L'ō' },
            { { L'O', 0x0304 }, L'Ō' },
            { { L'u', 0x0304 }, L'ū' },
            { { L'U', 0x0304 }, L'Ū' },
            // breve (Romanian, Vietnamese, Turkish)
            { { L'a', 0x0306 }, L'ă' },
            { { L'A', 0x0306 }, L'Ă' },
            { { L'g', 0x0306 }, L'ğ' },
            { { L'G', 0x0306 }, L'Ğ' },
            // Esperanto U with breve
            { { L'u', 0x0306 }, L'ŭ' },
            { { L'U', 0x0306 }, L'Ŭ' },
            // double acute (Hungarian)
            { { L'o', 0x030B }, L'ő' },
            { { L'O', 0x030B }, L'Ő' },
            { { L'u', 0x030B }, L'ű' },
            { { L'U', 0x030B }, L'Ű' },
            // cedilla extended (Turkish, Romanian)
            { { L's', 0x0327 }, L'ş' },
            { { L'S', 0x0327 }, L'Ş' },
            { { L't', 0x0327 }, L'ţ' },
            { { L'T', 0x0327 }, L'Ţ' },
            // comma below (Romanian)
            { { L's', 0x0326 }, L'ș' },
            { { L'S', 0x0326 }, L'Ș' },
            { { L't', 0x0326 }, L'ț' },
            { { L'T', 0x0326 }, L'Ț' },
            // hook above (Vietnamese)
            { { L'a', 0x0309 }, L'ả' },
            { { L'A', 0x0309 }, L'Ả' },
            { { L'e', 0x0309 }, L'ẻ' },
            { { L'E', 0x0309 }, L'Ẻ' },
            { { L'i', 0x0309 }, L'ỉ' },
            { { L'I', 0x0309 }, L'Ỉ' },
            { { L'o', 0x0309 }, L'ỏ' },
            { { L'O', 0x0309 }, L'Ỏ' },
            { { L'u', 0x0309 }, L'ủ' },
            { { L'U', 0x0309 }, L'Ủ' },
            { { L'y', 0x0309 }, L'ỷ' },
            { { L'Y', 0x0309 }, L'Ỷ' },
            // horn (Vietnamese)
            { { L'o', 0x031B }, L'ơ' },
            { { L'O', 0x031B }, L'Ơ' },
            { { L'u', 0x031B }, L'ư' },
            { { L'U', 0x031B }, L'Ư' }
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
