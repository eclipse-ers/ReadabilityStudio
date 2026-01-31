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

#ifndef INDEXING_CHARACTERS_H
#define INDEXING_CHARACTERS_H

#include "../Wisteria-Dataviz/src/util/string_util.h"
#include <cassert>

/// @brief Namespace for punctuation classes.
/// @details Recognizes the following character sets:
///     - Western European languages
///     - Russian
///     - Eastern European WORK IN PROGRESS
namespace characters
    {
    /// @brief This is the central interface for word character deductions and comparisons.
    class is_character
        {
      public:
        /// @brief Determines if a given value is either of two other given values.
        /// @param value The value to compare with.
        /// @param first The first value to compare against.
        /// @param second The second value to compare against.
        /// @returns @c true if value is either of the other values.
        template<typename T>
        [[nodiscard]]
        constexpr static bool is_either(const T value, const T first, const T second) noexcept
            {
            return (value == first || value == second);
            }

        /** @returns @c true if a character is a letter.
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool is_alpha(const wchar_t ch) noexcept
            {
            return (is_upper(ch) || is_lower(ch) || is_japanese_script(ch));
            }

        /** @returns @c true if a character is a letter (English alphabet only,
                and no full-width characters).
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool is_alpha_8bit(const wchar_t ch) noexcept
            {
            return (((ch >= 0x41 /*'A'*/) && (ch <= 0x5A /*'Z'*/)) ||
                    ((ch >= 0x61 /*'a'*/) && (ch <= 0x7A /*'z'*/)));
            }

        /** @returns @c true if a character is an uppercased letter.
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool is_upper(const wchar_t ch) noexcept
            {
            return (
                // A-Z
                (ch >= 0x41 && ch <= 0x5A) ||
                // A-Z, full-width
                (ch >= 0xFF21 && ch <= 0xFF3A) ||
                // uppercase extended ASCII set
                (ch >= 0xC0 && ch <= 0xD6) || (ch >= 0xD8 && ch <= 0xDE) ||
                (ch == 0x0112) || // E with macron
                // Y with umlaut
                (ch == 0x0178) ||
                // OE ligature
                (ch == 0x0152) ||
                // Eastern European
                // C with acute, R with acute, L with stroke, R with charon, A with breve, C with
                // charon, A with ogonek, O with double accent
                (ch == 0x0106 || ch == 0x0154 || ch == 0x0141 || ch == 0x0158 || ch == 0x0102 ||
                 ch == 0x010C || ch == 0x0104 || ch == 0x0150) ||
                // Polish/Lithuanian ogonek: Ą Ę Į Ų
                (ch == 0x0118 || ch == 0x012E || ch == 0x0172) ||
                // Macron vowels: Ā Ī Ō Ū (Ē already covered by 0x0112)
                (ch == 0x0100 || ch == 0x012A || ch == 0x014C || ch == 0x016A) ||
                // Hungarian double acute: Ű
                (ch == 0x0170) ||
                // Turkish/Romanian cedilla: Ş Ţ
                (ch == 0x015E || ch == 0x0162) ||
                // Romanian comma below: Ș Ț
                (ch == 0x0218 || ch == 0x021A) ||
                // Polish acute consonants: Ś Ź
                (ch == 0x015A || ch == 0x0179) ||
                // Polish dot above: Ż
                (ch == 0x017B) ||
                // Czech ring: Ů
                (ch == 0x016E) ||
                // Caron consonants: Ň Š Ž Ť Ď
                (ch == 0x0147 || ch == 0x0160 || ch == 0x017D || ch == 0x0164 || ch == 0x010E) ||
                // Caron vowel: Ě
                (ch == 0x011A) ||
                // Vietnamese O with dot below
                (ch == 0x1ECC) ||
                // Vietnamese dot below: Ạ Ẹ Ị Ụ
                (ch == 0x1EA0 || ch == 0x1EB8 || ch == 0x1ECA || ch == 0x1EE4) ||
                // Vietnamese hook above: Ả Ẻ Ỉ Ỏ Ủ Ỷ
                (ch == 0x1EA2 || ch == 0x1EBA || ch == 0x1EC8 || ch == 0x1ECE || ch == 0x1EE6 ||
                 ch == 0x1EF6) ||
                // Vietnamese horn: Ơ Ư
                (ch == 0x01A0 || ch == 0x01AF) ||
                // Turkish: Ğ İ
                (ch == 0x011E || ch == 0x0130) ||
                // Polish: Ń
                (ch == 0x0143) ||
                // Slovak: Ľ Ĺ
                (ch == 0x013D || ch == 0x0139) ||
                // Welsh: Ŵ Ŷ
                (ch == 0x0174 || ch == 0x0176) ||
                // Maltese: Ċ Ġ Ħ
                (ch == 0x010A || ch == 0x0120 || ch == 0x0126) ||
                // Croatian: Đ
                (ch == 0x0110) ||
                // Sami: Ŧ Ŋ
                (ch == 0x0166 || ch == 0x014A) ||
                // German: ẞ (capital eszett)
                (ch == 0x1E9E) ||
                // Old Irish dot above: Ḃ Ḋ Ḟ Ṁ Ṗ Ṡ Ṫ
                (ch == 0x1E02 || ch == 0x1E0A || ch == 0x1E1E || ch == 0x1E40 || ch == 0x1E56 ||
                 ch == 0x1E60 || ch == 0x1E6A) ||
                // Welsh: Ẁ Ẃ Ẅ Ỳ
                (ch == 0x1E80 || ch == 0x1E82 || ch == 0x1E84 || ch == 0x1EF2) ||
                // Latvian cedilla: Ģ Ķ Ļ Ņ
                (ch == 0x0122 || ch == 0x0136 || ch == 0x013B || ch == 0x0145) ||
                // Lithuanian: Ė
                (ch == 0x0116) ||
                // Esperanto circumflex: Ĉ Ĝ Ĥ Ĵ Ŝ
                (ch == 0x0108 || ch == 0x011C || ch == 0x0124 || ch == 0x0134 || ch == 0x015C) ||
                // Esperanto breve: Ŭ
                (ch == 0x016C) ||
                // Pinyin caron: Ǎ Ǐ Ǒ Ǔ
                (ch == 0x01CD || ch == 0x01CF || ch == 0x01D1 || ch == 0x01D3) ||
                // Russian
                (ch >= 0x0410 && ch <= 0x042F) || (ch == 0x0401) ||
                // Ukrainian: Ґ Є І Ї
                (ch == 0x0490 || ch == 0x0404 || ch == 0x0406 || ch == 0x0407) ||
                // Serbian: Ђ Ј Љ Њ Ћ Џ
                (ch == 0x0402 || ch == 0x0408 || ch == 0x0409 || ch == 0x040A || ch == 0x040B ||
                 ch == 0x040F) ||
                // Belarusian: Ў
                (ch == 0x040E) ||
                // Macedonian: Ѓ Ќ Ѕ
                (ch == 0x0403 || ch == 0x040C || ch == 0x0405) ||
                // Historical Russian: Ѣ Ѳ Ѵ
                (ch == 0x0462 || ch == 0x0472 || ch == 0x0474) ||
                // Greek uppercase: Α-Ρ (U+0391-U+03A1), Σ-Ω (U+03A3-U+03A9)
                (ch >= 0x0391 && ch <= 0x03A1) || (ch >= 0x03A3 && ch <= 0x03A9) ||
                // Greek uppercase with tonos: Ά Έ Ή Ί Ό Ύ Ώ
                (ch == 0x0386) || (ch >= 0x0388 && ch <= 0x038A) || (ch == 0x038C) ||
                (ch == 0x038E) || (ch == 0x038F) ||
                // Greek uppercase with dialytika: Ϊ Ϋ
                (ch == 0x03AA || ch == 0x03AB));
            }

        /** @returns @c true if a character is a lowercased letter.
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool is_lower(const wchar_t ch) noexcept
            {
            return (
                // a-z
                (ch >= 0x61 && ch <= 0x7A) ||
                // a-z, full-width
                (ch >= 0xFF41 && ch <= 0xFF5A) ||
                // lowercase extended ASCII set
                (ch >= 0xE0 && ch <= 0xF6) || (ch >= 0xF8 && ch <= 0xFF) ||
                (ch == 0x0113) || // e with macron
                // OE ligature
                (ch == 0x0153) ||
                // superscript letters
                string_util::is_superscript_lowercase(ch) || // n
                // subscript letters
                (ch >= 0x2090 && ch <= 0x209C) ||
                // German eszett (not exactly lowercase, but words never begin with these)
                (ch == 0xDF) ||
                // Eastern European
                // C with acute, R with acute, L with stroke, R with charon, A with breve, C with
                // charon, A with ogonek, O with double accent
                (ch == 0x0107 || ch == 0x0155 || ch == 0x0142 || ch == 0x0159 || ch == 0x0103 ||
                 ch == 0x010D || ch == 0x0105 || ch == 0x0151) ||
                // Polish/Lithuanian ogonek: ę į ų
                (ch == 0x0119 || ch == 0x012F || ch == 0x0173) ||
                // Macron vowels: ā ī ō ū (ē already covered by 0x0113)
                (ch == 0x0101 || ch == 0x012B || ch == 0x014D || ch == 0x016B) ||
                // Hungarian double acute: ű
                (ch == 0x0171) ||
                // Turkish/Romanian cedilla: ş ţ
                (ch == 0x015F || ch == 0x0163) ||
                // Romanian comma below: ș ț
                (ch == 0x0219 || ch == 0x021B) ||
                // Polish acute consonants: ś ź
                (ch == 0x015B || ch == 0x017A) ||
                // Polish dot above: ż
                (ch == 0x017C) ||
                // Czech ring: ů
                (ch == 0x016F) ||
                // Caron consonants: ň š ž ť ď
                (ch == 0x0148 || ch == 0x0161 || ch == 0x017E || ch == 0x0165 || ch == 0x010F) ||
                // Caron vowel: ě
                (ch == 0x011B) ||
                // Vietnamese o with dot below
                (ch == 0x1ECD) ||
                // Vietnamese dot below: ạ ẹ ị ụ
                (ch == 0x1EA1 || ch == 0x1EB9 || ch == 0x1ECB || ch == 0x1EE5) ||
                // Vietnamese hook above: ả ẻ ỉ ỏ ủ ỷ
                (ch == 0x1EA3 || ch == 0x1EBB || ch == 0x1EC9 || ch == 0x1ECF || ch == 0x1EE7 ||
                 ch == 0x1EF7) ||
                // Vietnamese horn: ơ ư
                (ch == 0x01A1 || ch == 0x01B0) ||
                // Turkish: ğ ı
                (ch == 0x011F || ch == 0x0131) ||
                // Polish: ń
                (ch == 0x0144) ||
                // Slovak: ľ ĺ
                (ch == 0x013E || ch == 0x013A) ||
                // Welsh: ŵ ŷ
                (ch == 0x0175 || ch == 0x0177) ||
                // Maltese: ċ ġ ħ
                (ch == 0x010B || ch == 0x0121 || ch == 0x0127) ||
                // Croatian: đ
                (ch == 0x0111) ||
                // Sami: ŧ ŋ
                (ch == 0x0167 || ch == 0x014B) ||
                // Greenlandic: ĸ (Kra, no uppercase exists)
                (ch == 0x0138) ||
                // Old Irish dot above: ḃ ḋ ḟ ṁ ṗ ṡ ṫ
                (ch == 0x1E03 || ch == 0x1E0B || ch == 0x1E1F || ch == 0x1E41 || ch == 0x1E57 ||
                 ch == 0x1E61 || ch == 0x1E6B) ||
                // Welsh: ẁ ẃ ẅ ỳ
                (ch == 0x1E81 || ch == 0x1E83 || ch == 0x1E85 || ch == 0x1EF3) ||
                // Latvian cedilla: ģ ķ ļ ņ
                (ch == 0x0123 || ch == 0x0137 || ch == 0x013C || ch == 0x0146) ||
                // Lithuanian: ė
                (ch == 0x0117) ||
                // Esperanto circumflex: ĉ ĝ ĥ ĵ ŝ
                (ch == 0x0109 || ch == 0x011D || ch == 0x0125 || ch == 0x0135 || ch == 0x015D) ||
                // Esperanto breve: ŭ
                (ch == 0x016D) ||
                // Pinyin caron: ǎ ǐ ǒ ǔ
                (ch == 0x01CE || ch == 0x01D0 || ch == 0x01D2 || ch == 0x01D4) ||
                // Russian
                (ch >= 0x0430 && ch <= 0x044F) || (ch == 0x0451) ||
                // Ukrainian: ґ є і ї
                (ch == 0x0491 || ch == 0x0454 || ch == 0x0456 || ch == 0x0457) ||
                // Serbian: ђ ј љ њ ћ џ
                (ch == 0x0452 || ch == 0x0458 || ch == 0x0459 || ch == 0x045A || ch == 0x045B ||
                 ch == 0x045F) ||
                // Belarusian: ў
                (ch == 0x045E) ||
                // Macedonian: ѓ ќ ѕ
                (ch == 0x0453 || ch == 0x045C || ch == 0x0455) ||
                // Historical Russian: ѣ ѳ ѵ
                (ch == 0x0463 || ch == 0x0473 || ch == 0x0475) ||
                // Greek lowercase: α-ρ (U+03B1-U+03C1), ς (U+03C2), σ-ω (U+03C3-U+03C9)
                (ch >= 0x03B1 && ch <= 0x03C1) || (ch >= 0x03C2 && ch <= 0x03C9) ||
                // Greek lowercase with tonos: ά έ ή ί ό ύ ώ
                (ch == 0x03AC) || (ch >= 0x03AD && ch <= 0x03AF) || (ch == 0x03CC) ||
                (ch == 0x03CD) || (ch == 0x03CE) ||
                // Greek lowercase with dialytika: ϊ ϋ
                (ch == 0x03CA || ch == 0x03CB) ||
                // Greek lowercase with dialytika and tonos: ΐ ΰ
                (ch == 0x0390 || ch == 0x03B0) ||
                // Georgian Mkhedruli (unicameral, treat as lowercase): U+10D0-U+10F0
                (ch >= 0x10D0 && ch <= 0x10F0));
            }

        /** @returns @c true if a character is a Greek letter (uppercase or lowercase).
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool is_greek_letter(const wchar_t ch) noexcept
            {
            return (
                // Greek uppercase: Α-Ρ (U+0391-U+03A1), Σ-Ω (U+03A3-U+03A9)
                (ch >= 0x0391 && ch <= 0x03A1) || (ch >= 0x03A3 && ch <= 0x03A9) ||
                // Greek uppercase with tonos: Ά Έ Ή Ί Ό Ύ Ώ
                (ch == 0x0386) || (ch >= 0x0388 && ch <= 0x038A) || (ch == 0x038C) ||
                (ch == 0x038E) || (ch == 0x038F) ||
                // Greek uppercase with dialytika: Ϊ Ϋ
                (ch == 0x03AA || ch == 0x03AB) ||
                // Greek lowercase: α-ρ (U+03B1-U+03C1), ς (U+03C2), σ-ω (U+03C3-U+03C9)
                (ch >= 0x03B1 && ch <= 0x03C1) || (ch >= 0x03C2 && ch <= 0x03C9) ||
                // Greek lowercase with tonos: ά έ ή ί ό ύ ώ
                (ch == 0x03AC) || (ch >= 0x03AD && ch <= 0x03AF) || (ch == 0x03CC) ||
                (ch == 0x03CD) || (ch == 0x03CE) ||
                // Greek lowercase with dialytika: ϊ ϋ
                (ch == 0x03CA || ch == 0x03CB) ||
                // Greek lowercase with dialytika and tonos: ΐ ΰ
                (ch == 0x0390 || ch == 0x03B0));
            }

        /** @returns @c true if a character is a Western European (Latin-based) letter.
            This includes basic Latin a-z/A-Z and extended Latin with diacritics
            used in Western European languages (French, German, Spanish,
            Portuguese, Italian, Scandinavian, etc.).
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool is_western_european_letter(const wchar_t ch) noexcept
            {
            return (
                // Basic Latin: A-Z, a-z
                (ch >= 0x41 && ch <= 0x5A) || (ch >= 0x61 && ch <= 0x7A) ||
                // Full-width Latin: Ａ-Ｚ, ａ-ｚ
                (ch >= 0xFF21 && ch <= 0xFF3A) || (ch >= 0xFF41 && ch <= 0xFF5A) ||
                // Extended ASCII (covers French, German, Spanish, Portuguese,
                // Italian, Scandinavian): À-Ö, Ø-ß, à-ö, ø-ÿ
                (ch >= 0xC0 && ch <= 0xD6) || (ch >= 0xD8 && ch <= 0xFF) ||
                // OE ligature: Œ/œ
                (ch == 0x0152 || ch == 0x0153) ||
                // German capital eszett: ẞ
                (ch == 0x1E9E) ||
                // Y with umlaut uppercase: Ÿ
                (ch == 0x0178));
            }

        /** @returns @c true if a character is a Hiragana character (U+3040-U+309F).
            @param ch The character to be reviewed.*/
        [[nodiscard]]
        constexpr static bool is_hiragana(const wchar_t ch) noexcept
            {
            return (ch >= 0x3040 && ch <= 0x309F);
            }

        /** @returns @c true if a character is a Katakana character
                (U+30A0-U+30FF or halfwidth U+FF65-U+FF9F).
            @param ch The character to be reviewed.*/
        [[nodiscard]]
        constexpr static bool is_katakana(const wchar_t ch) noexcept
            {
            return (ch >= 0x30A0 && ch <= 0x30FF) || // Katakana
                   (ch >= 0x31F0 && ch <= 0x31FF) || // Katakana Phonetic Extensions (Ainu)
                   (ch >= 0xFF65 && ch <= 0xFF9F);   // Halfwidth
            }

        /** @returns @c true if a character is a CJK Unified Ideograph
                (U+4E00-U+9FFF or Extension A U+3400-U+4DBF).
            @param ch The character to be reviewed.*/
        [[nodiscard]]
        constexpr static bool is_cjk_unified_ideograph(const wchar_t ch) noexcept
            {
            return (ch >= 0x4E00 && ch <= 0x9FFF) || (ch >= 0x3400 && ch <= 0x4DBF);
            }

        /** @returns @c true if a character is a Japanese ideograph
                (Hiragana, Katakana, or CJK Unified Ideograph).
            @param ch The character to be reviewed.
            @note This implementation supports Standard Modern Japanese (Jōyō/Jinmeiyō Kanji)
                as defined in the Unicode BMP.
                It does not support rare name variants (Extensions B-I) or non-BMP characters
                like Emojis.*/
        [[nodiscard]]
        constexpr static bool is_japanese_script(const wchar_t ch) noexcept
            {
            return is_hiragana(ch) || is_katakana(ch) || is_cjk_unified_ideograph(ch);
            }

        /** @returns The lowercased version of a letter, or the letter itself
                     if it can't be lowercased.
            @param ch The letter to be lowered.*/
        [[nodiscard]]
        constexpr static wchar_t to_lower(const wchar_t ch) noexcept
            {
            return (
                (ch >= 0x41 && ch <= 0x5A) ||
                (ch >= 0xC0 && ch <= 0xD6) ||
                (ch >= 0xD8 && ch <= 0xDE) ||
                // full-width A-Z
                (ch >= 0xFF21 && ch <= 0xFF3A) ||
                // basic Russian alphabet
                (ch >= 0x0410 && ch <= 0x042F)) ?
                    (ch + 32) :
                // characters that aren't 32 from their respective lowercase value
                (ch == 0x0401) ? 0x0451 : // Russian E with umlaut
                (ch == 0x0178) ? 0x00FF : // Y with umlaut
                (ch == 0x0152) ? 0x0153 : // OE ligature
                (ch == 0x0112) ? 0x0113 : // E with macron
                // Polish/Lithuanian ogonek
                (ch == 0x0104) ? 0x0105 : // Ą -> ą
                (ch == 0x0118) ? 0x0119 : // Ę -> ę
                (ch == 0x012E) ? 0x012F : // Į -> į
                (ch == 0x0172) ? 0x0173 : // Ų -> ų
                // Macron vowels
                (ch == 0x0100) ? 0x0101 : // Ā -> ā
                (ch == 0x012A) ? 0x012B : // Ī -> ī
                (ch == 0x014C) ? 0x014D : // Ō -> ō
                (ch == 0x016A) ? 0x016B : // Ū -> ū
                // Hungarian double acute
                (ch == 0x0150) ? 0x0151 : // Ő -> ő
                (ch == 0x0170) ? 0x0171 : // Ű -> ű
                // Romanian breve
                (ch == 0x0102) ? 0x0103 : // Ă -> ă
                // Turkish/Romanian cedilla
                (ch == 0x015E) ? 0x015F : // Ş -> ş
                (ch == 0x0162) ? 0x0163 : // Ţ -> ţ
                // Romanian comma below
                (ch == 0x0218) ? 0x0219 : // Ș -> ș
                (ch == 0x021A) ? 0x021B : // Ț -> ț
                // Vietnamese O with dot below
                (ch == 0x1ECC) ? 0x1ECD : // Ọ -> ọ
                // Vietnamese dot below
                (ch == 0x1EA0) ? 0x1EA1 : // Ạ -> ạ
                (ch == 0x1EB8) ? 0x1EB9 : // Ẹ -> ẹ
                (ch == 0x1ECA) ? 0x1ECB : // Ị -> ị
                (ch == 0x1EE4) ? 0x1EE5 : // Ụ -> ụ
                // Vietnamese hook above
                (ch == 0x1EA2) ? 0x1EA3 : // Ả -> ả
                (ch == 0x1EBA) ? 0x1EBB : // Ẻ -> ẻ
                (ch == 0x1EC8) ? 0x1EC9 : // Ỉ -> ỉ
                (ch == 0x1ECE) ? 0x1ECF : // Ỏ -> ỏ
                (ch == 0x1EE6) ? 0x1EE7 : // Ủ -> ủ
                (ch == 0x1EF6) ? 0x1EF7 : // Ỷ -> ỷ
                // Vietnamese horn
                (ch == 0x01A0) ? 0x01A1 : // Ơ -> ơ
                (ch == 0x01AF) ? 0x01B0 : // Ư -> ư
                // Polish acute consonants
                (ch == 0x0106) ? 0x0107 : // Ć -> ć
                (ch == 0x015A) ? 0x015B : // Ś -> ś
                (ch == 0x0179) ? 0x017A : // Ź -> ź
                // Polish dot above
                (ch == 0x017B) ? 0x017C : // Ż -> ż
                // Czech ring
                (ch == 0x016E) ? 0x016F : // Ů -> ů
                // Caron characters
                (ch == 0x010C) ? 0x010D : // Č -> č
                (ch == 0x0147) ? 0x0148 : // Ň -> ň
                (ch == 0x0158) ? 0x0159 : // Ř -> ř
                (ch == 0x0160) ? 0x0161 : // Š -> š
                (ch == 0x017D) ? 0x017E : // Ž -> ž
                (ch == 0x0164) ? 0x0165 : // Ť -> ť
                (ch == 0x010E) ? 0x010F : // Ď -> ď
                (ch == 0x011A) ? 0x011B : // Ě -> ě
                // Polish stroke
                (ch == 0x0141) ? 0x0142 : // Ł -> ł
                // Turkish
                (ch == 0x011E) ? 0x011F : // Ğ -> ğ
                (ch == 0x0130) ? 0x0069 : // İ -> i (standard Unicode case folding)
                // Polish
                (ch == 0x0143) ? 0x0144 : // Ń -> ń
                // Slovak
                (ch == 0x013D) ? 0x013E : // Ľ -> ľ
                (ch == 0x0139) ? 0x013A : // Ĺ -> ĺ
                // Welsh
                (ch == 0x0174) ? 0x0175 : // Ŵ -> ŵ
                (ch == 0x0176) ? 0x0177 : // Ŷ -> ŷ
                // Maltese
                (ch == 0x010A) ? 0x010B : // Ċ -> ċ
                (ch == 0x0120) ? 0x0121 : // Ġ -> ġ
                (ch == 0x0126) ? 0x0127 : // Ħ -> ħ
                // Slovak
                (ch == 0x0154) ? 0x0155 : // Ŕ -> ŕ
                // Croatian
                (ch == 0x0110) ? 0x0111 : // Đ -> đ
                // Sami
                (ch == 0x0166) ? 0x0167 : // Ŧ -> ŧ
                (ch == 0x014A) ? 0x014B : // Ŋ -> ŋ
                // German
                (ch == 0x1E9E) ? 0x00DF : // ẞ -> ß
                // Old Irish dot above
                (ch == 0x1E02) ? 0x1E03 : // Ḃ -> ḃ
                (ch == 0x1E0A) ? 0x1E0B : // Ḋ -> ḋ
                (ch == 0x1E1E) ? 0x1E1F : // Ḟ -> ḟ
                (ch == 0x1E40) ? 0x1E41 : // Ṁ -> ṁ
                (ch == 0x1E56) ? 0x1E57 : // Ṗ -> ṗ
                (ch == 0x1E60) ? 0x1E61 : // Ṡ -> ṡ
                (ch == 0x1E6A) ? 0x1E6B : // Ṫ -> ṫ
                // Welsh
                (ch == 0x1E80) ? 0x1E81 : // Ẁ -> ẁ
                (ch == 0x1E82) ? 0x1E83 : // Ẃ -> ẃ
                (ch == 0x1E84) ? 0x1E85 : // Ẅ -> ẅ
                (ch == 0x1EF2) ? 0x1EF3 : // Ỳ -> ỳ
                // Latvian cedilla
                (ch == 0x0122) ? 0x0123 : // Ģ -> ģ
                (ch == 0x0136) ? 0x0137 : // Ķ -> ķ
                (ch == 0x013B) ? 0x013C : // Ļ -> ļ
                (ch == 0x0145) ? 0x0146 : // Ņ -> ņ
                // Lithuanian
                (ch == 0x0116) ? 0x0117 : // Ė -> ė
                // Esperanto circumflex
                (ch == 0x0108) ? 0x0109 : // Ĉ -> ĉ
                (ch == 0x011C) ? 0x011D : // Ĝ -> ĝ
                (ch == 0x0124) ? 0x0125 : // Ĥ -> ĥ
                (ch == 0x0134) ? 0x0135 : // Ĵ -> ĵ
                (ch == 0x015C) ? 0x015D : // Ŝ -> ŝ
                // Esperanto breve
                (ch == 0x016C) ? 0x016D : // Ŭ -> ŭ
                // Pinyin caron
                (ch == 0x01CD) ? 0x01CE : // Ǎ -> ǎ
                (ch == 0x01CF) ? 0x01D0 : // Ǐ -> ǐ
                (ch == 0x01D1) ? 0x01D2 : // Ǒ -> ǒ
                (ch == 0x01D3) ? 0x01D4 : // Ǔ -> ǔ
                // Ukrainian
                (ch == 0x0490) ? 0x0491 : // Ґ -> ґ
                (ch == 0x0404) ? 0x0454 : // Є -> є
                (ch == 0x0406) ? 0x0456 : // І -> і
                (ch == 0x0407) ? 0x0457 : // Ї -> ї
                // Serbian
                (ch == 0x0402) ? 0x0452 : // Ђ -> ђ
                (ch == 0x0408) ? 0x0458 : // Ј -> ј
                (ch == 0x0409) ? 0x0459 : // Љ -> љ
                (ch == 0x040A) ? 0x045A : // Њ -> њ
                (ch == 0x040B) ? 0x045B : // Ћ -> ћ
                (ch == 0x040F) ? 0x045F : // Џ -> џ
                // Belarusian
                (ch == 0x040E) ? 0x045E : // Ў -> ў
                // Macedonian
                (ch == 0x0403) ? 0x0453 : // Ѓ -> ѓ
                (ch == 0x040C) ? 0x045C : // Ќ -> ќ
                (ch == 0x0405) ? 0x0455 : // Ѕ -> ѕ
                // Historical Russian
                (ch == 0x0462) ? 0x0463 : // Ѣ -> ѣ
                (ch == 0x0472) ? 0x0473 : // Ѳ -> ѳ
                (ch == 0x0474) ? 0x0475 : // Ѵ -> ѵ
                // Greek basic alphabet (offset of 32)
                (ch >= 0x0391 && ch <= 0x03A1) ? (ch + 32) :
                (ch >= 0x03A3 && ch <= 0x03A9) ? (ch + 32) :
                // Greek uppercase with tonos
                (ch == 0x0386) ? 0x03AC : // Ά -> ά
                (ch == 0x0388) ? 0x03AD : // Έ -> έ
                (ch == 0x0389) ? 0x03AE : // Ή -> ή
                (ch == 0x038A) ? 0x03AF : // Ί -> ί
                (ch == 0x038C) ? 0x03CC : // Ό -> ό
                (ch == 0x038E) ? 0x03CD : // Ύ -> ύ
                (ch == 0x038F) ? 0x03CE : // Ώ -> ώ
                // Greek uppercase with dialytika
                (ch == 0x03AA) ? 0x03CA : // Ϊ -> ϊ
                (ch == 0x03AB) ? 0x03CB : // Ϋ -> ϋ
                    ch;
            }

        /** @returns @c true if a character is a vowel.
            @note 'Y' is included here as it usually used as a vowel.\n
                It is recommended to handle 'y' in a special way whenever
                analyzing text.
            @param letter The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool is_vowel(const wchar_t letter) noexcept
            {
            return ((letter == L'a') || (letter == L'e') || (letter == L'i') || (letter == L'o') ||
                    (letter == L'u') || (letter == L'y') || (letter == L'A') || (letter == L'E') ||
                    (letter == L'I') || (letter == L'O') || (letter == L'U') || (letter == L'Y') ||
                    (letter == 0x1D43) || // superscript a
                    (letter == 0x1D49) || // superscript e
                    (letter == 0x2071) || // superscript i
                    (letter == 0x1D52) || // superscript o
                    (letter == 0x1D58) || // superscript u
                    (letter == 0x02B8) || // superscript y
                    (letter == 0x2090) || // subscript a
                    (letter == 0x2091) || // subscript e
                    (letter == 0x2092) || // subscript o
                    (letter == 0x2094) || // subscript upsidedown e
                    // full-width a,e,i,o,u,y
                    (letter == 0xFF21) || (letter == 0xFF25) || (letter == 0xFF29) ||
                    (letter == 0xFF2F) || (letter == 0xFF35) || (letter == 0xFF39) ||
                    (letter == 0xFF41) || (letter == 0xFF45) || (letter == 0xFF49) ||
                    (letter == 0xFF4F) || (letter == 0xFF55) || (letter == 0xFF59) ||
                    // Extended ASCII Western European letters
                    (letter >= 0xC0 && letter <= 0xC6) || (letter >= 0xC8 && letter <= 0xCF) ||
                    (letter >= 0xD2 && letter <= 0xD6) || (letter >= 0xD8 && letter <= 0xDC) ||
                    (letter >= 0xE0 && letter <= 0xE6) || (letter >= 0xE8 && letter <= 0xEF) ||
                    (letter >= 0xF2 && letter <= 0xF6) || (letter >= 0xF8 && letter <= 0xFC) ||
                    (letter >= 0x152 && letter <= 0x153) || // OE ligature
                    // e with macron
                    (letter == 0x0112 || letter == 0x0113) ||
                    // Polish/Lithuanian ogonek vowels: ą Ą ę Ę į Į ų Ų
                    is_either<wchar_t>(letter, 0x0104, 0x0105) ||
                    is_either<wchar_t>(letter, 0x0118, 0x0119) ||
                    is_either<wchar_t>(letter, 0x012E, 0x012F) ||
                    is_either<wchar_t>(letter, 0x0172, 0x0173) ||
                    // Macron vowels: ā Ā ī Ī ō Ō ū Ū
                    is_either<wchar_t>(letter, 0x0100, 0x0101) ||
                    is_either<wchar_t>(letter, 0x012A, 0x012B) ||
                    is_either<wchar_t>(letter, 0x014C, 0x014D) ||
                    is_either<wchar_t>(letter, 0x016A, 0x016B) ||
                    // Romanian breve: ă Ă
                    is_either<wchar_t>(letter, 0x0102, 0x0103) ||
                    // Hungarian double acute: ő Ő ű Ű
                    is_either<wchar_t>(letter, 0x0150, 0x0151) ||
                    is_either<wchar_t>(letter, 0x0170, 0x0171) ||
                    // Czech ring: ů Ů
                    is_either<wchar_t>(letter, 0x016E, 0x016F) ||
                    // Czech caron vowel: ě Ě
                    is_either<wchar_t>(letter, 0x011A, 0x011B) ||
                    // Portuguese tilde: ã Ã õ Õ
                    is_either<wchar_t>(letter, 0x00C3, 0x00E3) ||
                    is_either<wchar_t>(letter, 0x00D5, 0x00F5) ||
                    // Vietnamese dot below: ạ Ạ ẹ Ẹ ị Ị ụ Ụ
                    is_either<wchar_t>(letter, 0x1EA0, 0x1EA1) ||
                    is_either<wchar_t>(letter, 0x1EB8, 0x1EB9) ||
                    is_either<wchar_t>(letter, 0x1ECA, 0x1ECB) ||
                    is_either<wchar_t>(letter, 0x1EE4, 0x1EE5) ||
                    // Vietnamese hook above: ả Ả ẻ Ẻ ỉ Ỉ ỏ Ỏ ủ Ủ ỷ Ỷ
                    is_either<wchar_t>(letter, 0x1EA2, 0x1EA3) ||
                    is_either<wchar_t>(letter, 0x1EBA, 0x1EBB) ||
                    is_either<wchar_t>(letter, 0x1EC8, 0x1EC9) ||
                    is_either<wchar_t>(letter, 0x1ECE, 0x1ECF) ||
                    is_either<wchar_t>(letter, 0x1EE6, 0x1EE7) ||
                    is_either<wchar_t>(letter, 0x1EF6, 0x1EF7) ||
                    // Vietnamese horn: ơ Ơ ư Ư
                    is_either<wchar_t>(letter, 0x01A0, 0x01A1) ||
                    is_either<wchar_t>(letter, 0x01AF, 0x01B0) ||
                    // basic Russian alphabet
                    is_either<wchar_t>(letter, 0x0401, 0x0451) ||
                    is_either<wchar_t>(letter, 0x0410, 0x0430) ||
                    is_either<wchar_t>(letter, 0x0415, 0x0435) ||
                    is_either<wchar_t>(letter, 0x0418, 0x0438) ||
                    is_either<wchar_t>(letter, 0x0419, 0x0439) ||
                    is_either<wchar_t>(letter, 0x041E, 0x043E) ||
                    is_either<wchar_t>(letter, 0x0423, 0x0443) ||
                    is_either<wchar_t>(letter, 0x042B, 0x044B) ||
                    is_either<wchar_t>(letter, 0x042D, 0x044D) ||
                    is_either<wchar_t>(letter, 0x042E, 0x044E) ||
                    is_either<wchar_t>(letter, 0x042F, 0x044F) ||
                    // Vietnamese O with dot below
                    is_either<wchar_t>(letter, 0x1ECC, 0x1ECD) ||
                    // Turkish: İ ı
                    is_either<wchar_t>(letter, 0x0130, 0x0131) ||
                    // Welsh: Ŷ ŷ Ỳ ỳ
                    is_either<wchar_t>(letter, 0x0176, 0x0177) ||
                    is_either<wchar_t>(letter, 0x1EF2, 0x1EF3) ||
                    // Y with acute: Ý ý (Czech, Slovak, Icelandic, Vietnamese)
                    is_either<wchar_t>(letter, 0x00DD, 0x00FD) ||
                    // Y with diaeresis: Ÿ ÿ (French)
                    is_either<wchar_t>(letter, 0x0178, 0x00FF) ||
                    // Lithuanian: Ė ė
                    is_either<wchar_t>(letter, 0x0116, 0x0117) ||
                    // Esperanto breve: Ŭ ŭ
                    is_either<wchar_t>(letter, 0x016C, 0x016D) ||
                    // Pinyin caron: Ǎ ǎ Ǐ ǐ Ǒ ǒ Ǔ ǔ
                    is_either<wchar_t>(letter, 0x01CD, 0x01CE) ||
                    is_either<wchar_t>(letter, 0x01CF, 0x01D0) ||
                    is_either<wchar_t>(letter, 0x01D1, 0x01D2) ||
                    is_either<wchar_t>(letter, 0x01D3, 0x01D4) ||
                    // Ukrainian vowels: Є/є І/і Ї/ї
                    is_either<wchar_t>(letter, 0x0404, 0x0454) ||
                    is_either<wchar_t>(letter, 0x0406, 0x0456) ||
                    is_either<wchar_t>(letter, 0x0407, 0x0457) ||
                    // Belarusian: Ў/ў (short U, functions as vowel)
                    is_either<wchar_t>(letter, 0x040E, 0x045E) ||
                    // Historical Russian vowels: Ѣ/ѣ (Yat) Ѵ/ѵ (Izhitsa)
                    is_either<wchar_t>(letter, 0x0462, 0x0463) ||
                    is_either<wchar_t>(letter, 0x0474, 0x0475) ||
                    // Greek vowels: Α/α Ε/ε Η/η Ι/ι Ο/ο Υ/υ Ω/ω
                    is_either<wchar_t>(letter, 0x0391, 0x03B1) || // Α/α
                    is_either<wchar_t>(letter, 0x0395, 0x03B5) || // Ε/ε
                    is_either<wchar_t>(letter, 0x0397, 0x03B7) || // Η/η
                    is_either<wchar_t>(letter, 0x0399, 0x03B9) || // Ι/ι
                    is_either<wchar_t>(letter, 0x039F, 0x03BF) || // Ο/ο
                    is_either<wchar_t>(letter, 0x03A5, 0x03C5) || // Υ/υ
                    is_either<wchar_t>(letter, 0x03A9, 0x03C9) || // Ω/ω
                    // Greek vowels with tonos: Ά/ά Έ/έ Ή/ή Ί/ί Ό/ό Ύ/ύ Ώ/ώ
                    is_either<wchar_t>(letter, 0x0386, 0x03AC) || // Ά/ά
                    is_either<wchar_t>(letter, 0x0388, 0x03AD) || // Έ/έ
                    is_either<wchar_t>(letter, 0x0389, 0x03AE) || // Ή/ή
                    is_either<wchar_t>(letter, 0x038A, 0x03AF) || // Ί/ί
                    is_either<wchar_t>(letter, 0x038C, 0x03CC) || // Ό/ό
                    is_either<wchar_t>(letter, 0x038E, 0x03CD) || // Ύ/ύ
                    is_either<wchar_t>(letter, 0x038F, 0x03CE) || // Ώ/ώ
                    // Greek vowels with dialytika: Ϊ/ϊ Ϋ/ϋ
                    is_either<wchar_t>(letter, 0x03AA, 0x03CA) || // Ϊ/ϊ
                    is_either<wchar_t>(letter, 0x03AB, 0x03CB) || // Ϋ/ϋ
                    // Greek vowels with dialytika and tonos: ΐ ΰ (lowercase only)
                    (letter == 0x0390) || (letter == 0x03B0) ||
                    // Georgian vowels: ა ე ი ო უ
                    (letter == 0x10D0) || // ა (a)
                    (letter == 0x10D4) || // ე (e)
                    (letter == 0x10D8) || // ი (i)
                    (letter == 0x10DD) || // ო (o)
                    (letter == 0x10E3));  // უ (u)
            }

        /** @returns @c true if a character is a consonant.
            @note 'Y' is not included here as it usually used as a vowel.\n
                It is recommended to handle 'y' in a special way whenever
                analyzing text.
            @param letter The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool is_consonant(const wchar_t letter) noexcept
            {
            return (
                (letter >= L'B' && letter <= L'D') || (letter >= L'F' && letter <= L'H') ||
                (letter >= L'J' && letter <= L'N') || (letter >= L'P' && letter <= L'T') ||
                // just treat 'y' as a vowel for the sake of argument
                (letter >= L'V' && letter <= L'X') || (letter == L'Z') ||
                (letter >= L'b' && letter <= L'd') || (letter >= L'f' && letter <= L'h') ||
                (letter >= L'j' && letter <= L'n') || (letter >= L'p' && letter <= L't') ||
                // just treat 'y' as a vowel for the sake of argument
                (letter >= L'v' && letter <= L'x') || (letter == L'z') ||
                // superscripts
                (letter == 0x1D47) || (letter == 0x1D9C) || (letter == 0x1D48) ||
                (letter == 0x1DA0) || (letter == 0x1D4D) || (letter == 0x02B0) ||
                (letter == 0x02B2) || (letter == 0x1D4F) || (letter == 0x02E1) ||
                (letter == 0x1D50) || (letter == 0x207F) || (letter == 0x1D56) ||
                (letter == 0x02B3) || (letter == 0x02E2) || (letter == 0x1D57) ||
                (letter == 0x1D5B) || (letter == 0x02B7) || (letter == 0x02E3) ||
                (letter == 0x1DBB) || (letter == 0x2093) || // subscript x
                (letter >= 0x2095 &&
                 letter <= 0x209C) || // subscript h - t (not all those letters, though)
                // full-width letters
                (letter >= 0xFF22 && letter <= 0xFF24) ||
                (letter >= 0xFF26 && letter <= 0xFF28) || (letter >= 0xFF2A && letter <= 0xFF2E) ||
                (letter >= 0xFF30 && letter <= 0xFF34) ||
                // just treat 'y' as a vowel for the sake of argument
                (letter >= 0xFF36 && letter <= 0xFF38) || (letter == 0xFF3A) ||
                (letter >= 0xFF42 && letter <= 0xFF44) || (letter >= 0xFF46 && letter <= 0xFF48) ||
                (letter >= 0xFF4A && letter <= 0xFF4E) || (letter >= 0xFF50 && letter <= 0xFF54) ||
                // just treat 'y' as a vowel for the sake of argument
                (letter >= 0xFF56 && letter <= 0xFF58) || (letter == 0xFF5A) ||
                // Extended ASCII Western European letters
                (letter == 0xC7) ||                   // upper C with cedilla
                (letter == 0xD0) ||                   // upper Eth
                (letter == 0xD1) ||                   // upper N with tilde
                (letter >= 0xDE && letter <= 0xDF) || // upper Thorn and Eszett
                (letter == 0xE7) ||                   // lower C with cedilla
                (letter == 0xF0) ||                   // lower Eth
                (letter == 0xF1) ||                   // lower N with tilde
                (letter == 0xFE) ||                   // lower thorn
                // basic Russian alphabet
                (letter >= 0x0411 && letter <= 0x414) || (letter >= 0x0416 && letter <= 0x417) ||
                (letter >= 0x041A && letter <= 0x041D) || (letter >= 0x041F && letter <= 0x0422) ||
                (letter >= 0x0424 && letter <= 0x042A) ||
                is_either<wchar_t>(letter, 0x042C, 0x044C) ||
                (letter >= 0x0431 && letter <= 0x434) || (letter >= 0x0436 && letter <= 0x437) ||
                (letter >= 0x043A && letter <= 0x043D) || (letter >= 0x043F && letter <= 0x0442) ||
                (letter >= 0x0444 && letter <= 0x044A) ||
                // Turkish/Romanian cedilla: ş Ş ţ Ţ
                is_either<wchar_t>(letter, 0x015E, 0x015F) ||
                is_either<wchar_t>(letter, 0x0162, 0x0163) ||
                // Romanian comma below: ș Ș ț Ț
                is_either<wchar_t>(letter, 0x0218, 0x0219) ||
                is_either<wchar_t>(letter, 0x021A, 0x021B) ||
                // Polish acute consonants: Ć/ć Ś/ś Ź/ź
                is_either<wchar_t>(letter, 0x0106, 0x0107) ||
                is_either<wchar_t>(letter, 0x015A, 0x015B) ||
                is_either<wchar_t>(letter, 0x0179, 0x017A) ||
                // Polish dot above: Ż/ż
                is_either<wchar_t>(letter, 0x017B, 0x017C) ||
                // Caron consonants: Č/č Ň/ň Ř/ř Š/š Ž/ž Ť/ť Ď/ď
                is_either<wchar_t>(letter, 0x010C, 0x010D) ||
                is_either<wchar_t>(letter, 0x0147, 0x0148) ||
                is_either<wchar_t>(letter, 0x0158, 0x0159) ||
                is_either<wchar_t>(letter, 0x0160, 0x0161) ||
                is_either<wchar_t>(letter, 0x017D, 0x017E) ||
                is_either<wchar_t>(letter, 0x0164, 0x0165) ||
                is_either<wchar_t>(letter, 0x010E, 0x010F) ||
                // Polish stroke: Ł/ł
                is_either<wchar_t>(letter, 0x0141, 0x0142) ||
                // Turkish: Ğ/ğ
                is_either<wchar_t>(letter, 0x011E, 0x011F) ||
                // Polish: Ń/ń
                is_either<wchar_t>(letter, 0x0143, 0x0144) ||
                // Slovak: Ľ/ľ Ĺ/ĺ
                is_either<wchar_t>(letter, 0x013D, 0x013E) ||
                is_either<wchar_t>(letter, 0x0139, 0x013A) ||
                // Welsh: Ŵ/ŵ
                is_either<wchar_t>(letter, 0x0174, 0x0175) ||
                // Maltese: Ċ/ċ Ġ/ġ Ħ/ħ
                is_either<wchar_t>(letter, 0x010A, 0x010B) ||
                is_either<wchar_t>(letter, 0x0120, 0x0121) ||
                is_either<wchar_t>(letter, 0x0126, 0x0127) ||
                // Slovak: Ŕ/ŕ
                is_either<wchar_t>(letter, 0x0154, 0x0155) ||
                // Croatian: Đ/đ
                is_either<wchar_t>(letter, 0x0110, 0x0111) ||
                // Sami: Ŧ/ŧ Ŋ/ŋ
                is_either<wchar_t>(letter, 0x0166, 0x0167) ||
                is_either<wchar_t>(letter, 0x014A, 0x014B) ||
                // Greenlandic: ĸ (Kra, lowercase only)
                (letter == 0x0138) ||
                // Old Irish dot above: Ḃ/ḃ Ḋ/ḋ Ḟ/ḟ Ṁ/ṁ Ṗ/ṗ Ṡ/ṡ Ṫ/ṫ
                is_either<wchar_t>(letter, 0x1E02, 0x1E03) ||
                is_either<wchar_t>(letter, 0x1E0A, 0x1E0B) ||
                is_either<wchar_t>(letter, 0x1E1E, 0x1E1F) ||
                is_either<wchar_t>(letter, 0x1E40, 0x1E41) ||
                is_either<wchar_t>(letter, 0x1E56, 0x1E57) ||
                is_either<wchar_t>(letter, 0x1E60, 0x1E61) ||
                is_either<wchar_t>(letter, 0x1E6A, 0x1E6B) ||
                // German: ẞ (capital eszett, lowercase ß already covered by 0xDF above)
                (letter == 0x1E9E) ||
                // Welsh: Ẁ/ẁ Ẃ/ẃ Ẅ/ẅ
                is_either<wchar_t>(letter, 0x1E80, 0x1E81) ||
                is_either<wchar_t>(letter, 0x1E82, 0x1E83) ||
                is_either<wchar_t>(letter, 0x1E84, 0x1E85) ||
                // Latvian cedilla: Ģ/ģ Ķ/ķ Ļ/ļ Ņ/ņ
                is_either<wchar_t>(letter, 0x0122, 0x0123) ||
                is_either<wchar_t>(letter, 0x0136, 0x0137) ||
                is_either<wchar_t>(letter, 0x013B, 0x013C) ||
                is_either<wchar_t>(letter, 0x0145, 0x0146) ||
                // Esperanto circumflex: Ĉ/ĉ Ĝ/ĝ Ĥ/ĥ Ĵ/ĵ Ŝ/ŝ
                is_either<wchar_t>(letter, 0x0108, 0x0109) ||
                is_either<wchar_t>(letter, 0x011C, 0x011D) ||
                is_either<wchar_t>(letter, 0x0124, 0x0125) ||
                is_either<wchar_t>(letter, 0x0134, 0x0135) ||
                is_either<wchar_t>(letter, 0x015C, 0x015D) ||
                // Ukrainian: Ґ/ґ
                is_either<wchar_t>(letter, 0x0490, 0x0491) ||
                // Serbian: Ђ/ђ Ј/ј Љ/љ Њ/њ Ћ/ћ Џ/џ
                is_either<wchar_t>(letter, 0x0402, 0x0452) ||
                is_either<wchar_t>(letter, 0x0408, 0x0458) ||
                is_either<wchar_t>(letter, 0x0409, 0x0459) ||
                is_either<wchar_t>(letter, 0x040A, 0x045A) ||
                is_either<wchar_t>(letter, 0x040B, 0x045B) ||
                is_either<wchar_t>(letter, 0x040F, 0x045F) ||
                // Macedonian: Ѓ/ѓ Ќ/ќ Ѕ/ѕ
                is_either<wchar_t>(letter, 0x0403, 0x0453) ||
                is_either<wchar_t>(letter, 0x040C, 0x045C) ||
                is_either<wchar_t>(letter, 0x0405, 0x0455) ||
                // Historical Russian: Ѳ/ѳ (Fita)
                is_either<wchar_t>(letter, 0x0472, 0x0473) ||
                // Greek consonants: Β Γ Δ Ζ Θ Κ Λ Μ Ν Ξ Π Ρ Σ Τ Φ Χ Ψ
                is_either<wchar_t>(letter, 0x0392, 0x03B2) ||                     // Β/β
                is_either<wchar_t>(letter, 0x0393, 0x03B3) ||                     // Γ/γ
                is_either<wchar_t>(letter, 0x0394, 0x03B4) ||                     // Δ/δ
                is_either<wchar_t>(letter, 0x0396, 0x03B6) ||                     // Ζ/ζ
                is_either<wchar_t>(letter, 0x0398, 0x03B8) ||                     // Θ/θ
                is_either<wchar_t>(letter, 0x039A, 0x03BA) ||                     // Κ/κ
                is_either<wchar_t>(letter, 0x039B, 0x03BB) ||                     // Λ/λ
                is_either<wchar_t>(letter, 0x039C, 0x03BC) ||                     // Μ/μ
                is_either<wchar_t>(letter, 0x039D, 0x03BD) ||                     // Ν/ν
                is_either<wchar_t>(letter, 0x039E, 0x03BE) ||                     // Ξ/ξ
                is_either<wchar_t>(letter, 0x03A0, 0x03C0) ||                     // Π/π
                is_either<wchar_t>(letter, 0x03A1, 0x03C1) ||                     // Ρ/ρ
                (letter == 0x03A3) || (letter == 0x03C3) || (letter == 0x03C2) || // Σ/σ/ς
                is_either<wchar_t>(letter, 0x03A4, 0x03C4) ||                     // Τ/τ
                is_either<wchar_t>(letter, 0x03A6, 0x03C6) ||                     // Φ/φ
                is_either<wchar_t>(letter, 0x03A7, 0x03C7) ||                     // Χ/χ
                is_either<wchar_t>(letter, 0x03A8, 0x03C8) ||                     // Ψ/ψ
                // Georgian consonants (28 total, all letters except 5 vowels)
                // ბ გ დ ვ ზ თ კ ლ მ ნ პ ჟ რ ს ტ ფ ქ ღ ყ შ ჩ ც ძ წ ჭ ხ ჯ ჰ
                (letter == 0x10D1) || // ბ (b)
                (letter == 0x10D2) || // გ (g)
                (letter == 0x10D3) || // დ (d)
                (letter == 0x10D5) || // ვ (v)
                (letter == 0x10D6) || // ზ (z)
                (letter == 0x10D7) || // თ (t)
                (letter == 0x10D9) || // კ (k)
                (letter == 0x10DA) || // ლ (l)
                (letter == 0x10DB) || // მ (m)
                (letter == 0x10DC) || // ნ (n)
                (letter == 0x10DE) || // პ (p)
                (letter == 0x10DF) || // ჟ (zh)
                (letter == 0x10E0) || // რ (r)
                (letter == 0x10E1) || // ს (s)
                (letter == 0x10E2) || // ტ (t')
                (letter == 0x10E4) || // ფ (p')
                (letter == 0x10E5) || // ქ (k')
                (letter == 0x10E6) || // ღ (gh)
                (letter == 0x10E7) || // ყ (q)
                (letter == 0x10E8) || // შ (sh)
                (letter == 0x10E9) || // ჩ (ch)
                (letter == 0x10EA) || // ც (ts)
                (letter == 0x10EB) || // ძ (dz)
                (letter == 0x10EC) || // წ (ts')
                (letter == 0x10ED) || // ჭ (ch')
                (letter == 0x10EE) || // ხ (kh)
                (letter == 0x10EF) || // ჯ (j)
                (letter == 0x10F0));  // ჰ (h)
            }

        /** @returns @c true if a character is a lowercased consonant.
            @param letter The letter to be reviewed.
            @note These functions are finalized for all languages. The functions above
                need to be expanded when adding a new character set.*/
        [[nodiscard]]
        constexpr static bool is_lower_consonant(const wchar_t letter) noexcept
            {
            return (is_lower(letter) && is_consonant(letter));
            }

        /** @returns @c true if a character can begin a word.
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool can_character_begin_word(const wchar_t ch) noexcept
            {
            // clang-format off
            return is_either<wchar_t>(ch, 35, 0xFF03) ? // #
                    true :
                   is_either<wchar_t>(ch, 37, 0xFF05) ? // % (could be the entire word)
                    true :
                   is_either<wchar_t>(ch, 36, 0xFF04) ? // $
                    true :
                   is_either<wchar_t>(ch, L'&', 0xFF06) ?
                    true :
                   is_numeric_simple(ch) ?
                    true :
                   // don't allow words to start with super/subscripts or fractions
                   (is_alpha(ch) && !string_util::is_superscript(ch) &&
                    !string_util::is_subscript(ch)) ?
                    true :
                   // Doxygen tags (e.g., @note) or used as a whole word
                   // (e.g., "meet @ 5:00")
                   is_either<wchar_t>(ch, L'@', 0xFF20) ? true :
                   is_either<wchar_t>(ch, 163, 0xFFE1) ? // Pound Sterling
                    true :
                   is_either<wchar_t>(ch, 0x80, 0x20AC) ? // Euro
                    true :
                   (ch == 0x20B1) ? // Cuban peso
                    true :
                   (ch == 0x20A9) ? // Korean Won (currency)
                    true :
                   (ch == 177); // plus/minus±
            // clang-format on
            }

        /** @returns @c true if a character is an uppercased letter that
                normally can start a sentence.
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool can_character_begin_word_uppercase(const wchar_t ch) noexcept
            {
            // clang-format off
            return is_either<wchar_t>(ch, 35, 0xFF03) ? // #
                    true :
                   is_either<wchar_t>(ch, 36, 0xFF04) ? // $
                    true :
                   is_either<wchar_t>(ch, L'&', 0xFF06) ? true :
                   is_numeric(ch)                       ? true :
                   is_upper(ch)                         ? true :
                   is_either<wchar_t>(ch, L'@', 0xFF20) ? true :
                   is_either<wchar_t>(ch, 163, 0xFFE1) ? // Pound Sterling
                    true :
                   is_either<wchar_t>(ch, 0x80, 0x20AC) ? // Euro
                    true :
                   (ch == 0x20B1) ? // Cuban peso
                    true :
                   (ch == 0x20A9) ? // Korean Won (currency)
                    true :
                   (ch == 177); // plus/minus
            // clang-format on
            }

        /** @returns @c true if a (non-numeric) character can appear at the end of a word.
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool can_character_end_word(const wchar_t ch) noexcept
            {
            // clang-format off
            return is_either<wchar_t>(ch, 35, 0xFF03) ? // #
                    true :
                   is_either<wchar_t>(ch, 37, 0xFF05) ? // %
                    true :
                   is_either<wchar_t>(ch, L'&', 0xFF06) ? // &
                    true :
                   (ch == 46) ? // .
                    true :
                   is_either<wchar_t>(ch, 47, 0xFF0F) ? // '/'
                    true :
                   is_numeric(ch)    ? true :
                   is_alpha(ch)      ? true :
                   is_apostrophe(ch) ? true :
                   // could be an entire word
                   is_either<wchar_t>(ch, L'@', 0xFF20) ?
                    true :
                   is_either<wchar_t>(ch, 92, 0xFF3C) ? /*\*/
                    true :
                   is_either<wchar_t>(ch, 162, 0xFFE0) ? // cent
                    true :
                   is_either<wchar_t>(ch, 176, 0xFFEE); // degree
            // clang-format on
            }

        /** @returns @c true if a (non-numeric) character can appear at the end of a number.
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool can_character_end_numeral(const wchar_t ch) noexcept
            {
            // clang-format off
            return is_either<wchar_t>(ch, 37, 0xFF05) ? // %
                    true :
                   is_either<wchar_t>(ch, 162, 0xFFE0) ? // cent
                    true :
                   is_either<wchar_t>(ch, 176, 0xFFEE); // degree
            // clang-format on
            }

        /** @returns @c true if a character can appear inside the word.
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr bool operator()(const wchar_t ch) const noexcept
            {
            // clang-format off
            return is_either<wchar_t>(ch, 35, 0xFF03) ? // #
                    true :
                   is_either<wchar_t>(ch, 37, 0xFF05) ? // %
                    true :
                   is_either<wchar_t>(ch, 38, 0xFF06) ? // &
                    true :
                   (ch == 46) ? // .
                    true :
                   is_either<wchar_t>(ch, 47, 0xFF0F) ? // /
                    true :
                   is_either<wchar_t>(ch, 58, 0xFF1A) ? // :
                    true :
                   is_numeric(ch)                       ? true :
                   is_alpha(ch)                         ? true :
                   is_hyphen(ch)                        ? true :
                   is_apostrophe(ch)                    ? true :
                   is_either<wchar_t>(ch, L'@', 0xFF20) ? true :
                   is_either<wchar_t>(ch, 92, 0xFF3C) ? /*\*/
                    true :
                   (ch >= 0x5F && ch <= 0x60) ? // _`
                    true :
                   (ch >= 0xFF3F && ch <= 0xFF40) ? // full-width _`
                    true :
                   (ch == 126) ? // tilde (usually appear inside a file path)
                    true :
                   is_either<wchar_t>(ch, 162, 0xFFE0) ? // cent
                    true :
                   is_either<wchar_t>( ch, 176, 0xFFEE); // degree
            // clang-format on
            }

        /** @returns @c true if a character is a hyphen.
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool is_hyphen(const wchar_t ch) noexcept
            {
            return (ch == 0x002D /*hyphen*/ || ch == 0x00AD /*soft hyphen*/ ||
                    ch == 0xFF0D /*full-width hyphen*/);
            }

        /** @returns @c true if a character is a hyphen or dash.
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool is_dash_or_hyphen(const wchar_t ch) noexcept
            {
            return (is_hyphen(ch) || is_dash(ch));
            }

        /** @returns @c true if a character is a dash.
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool is_dash(const wchar_t ch) noexcept
            {
            // clang-format off
            return (ch == 0x2012) ? // figure dash
                    true :
                   (ch == 0x2013) ? // en dash
                    true :
                   (ch == 0x2014) ? // em dash
                    true :
                   (ch == 0x2015) ? // horizontal bar
                    true :
                   (ch == 0x2E17) ? // Japanese double oblique hyphen
                    true :
                   (ch == 0x30A0) ? // Katakana-Hiragana double hyphen
                    true :
                   (ch == 0x301C); // Japanese wave dash
            // clang-format on
            }

        /** @returns @c true if a character is an apostrophe (includes straight single quotes).
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool is_apostrophe(const wchar_t ch) noexcept
            {
            // clang-format off
            return (ch == 39) ? // '
                    true :
                   (ch == 146) ? // apostrophe
                    true :
                   (ch == 180) ? // apostrophe
                    true :
                   (ch == 0xFF07) ? // full-width apostrophe
                    true :
                   (ch == 0x2019); // right single apostrophe
            // clang-format on
            }

        /** @returns @c true if a character is a period.
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool is_period(const wchar_t ch) noexcept
            {
            return (ch == L'.' || ch == 0xFF0E /*full-width*/ ||
                    ch == 0x3002 /*ideographic full stop*/ ||
                    ch == 0xFF61 /*half-width ideographic period*/);
            }

        /** @returns @c true if a (non-numeric) character can appear in front of a number
                (e.g., a dollar sign).
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool can_character_prefix_numeral(const wchar_t ch) noexcept
            {
            // clang-format off
            return is_either<wchar_t>(ch, 35, 0xFF03) ? // #
                    true :
                   is_either<wchar_t>(ch, 36, 0xFF04) ? // $
                    true :
                   (ch >= 43 && ch <= 46) ? //+,-.
                    true :
                   (ch >= 0xFF0B && ch <= 0xFF0E) ? // full-width +,-.
                    true :
                   is_either<wchar_t>(ch, 0x80, 0x20AC) ? // Euro
                    true :
                   is_either<wchar_t>(ch, 163, 0xFFE1) ? // Pound Sterling
                    true :
                   (ch == 165) ? // Yen
                    true :
                   (ch == 177) ? // plus/minus
                    true :
                   (ch == 0x20B1) ? // Cuban peso
                    true :
                   (ch == 0x20A9); // Korean Won (currency)
            // clang-format on
            }

        /** @returns @c true if a character stream is an ellipsis, and the number of periods
                     making up the ellipsis (if constructed out of periods, zero for
                     Unicode ellipsis or if not an ellipsis).
            @param text The stream to be reviewed.*/
        [[nodiscard]]
        static std::pair<bool, size_t> is_ellipsis(std::wstring_view text) noexcept
            {
            if (text.empty())
                {
                return std::make_pair(false, 0);
                }
            if (text.length() == 1)
                {
                return std::make_pair(is_either<wchar_t>(text[0], 0x85, 0x2026), 0);
                }
            size_t periodCount = 0;
            for (size_t i = 0; i < text.length(); ++i)
                {
                if (text[i] == 0)
                    {
                    break;
                    }
                if (text[i] == L'.')
                    {
                    ++periodCount;
                    }
                }
            return std::make_pair((periodCount > 1), periodCount);
            }

        /** @returns @c true if a punctuation character can appear inside a date/time string.
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool can_character_form_date_time(const wchar_t ch) noexcept
            {
            // clang-format off
            return (ch >= 44 && ch <= 47) ? // ,-./
                    true :
                   (ch >= 0xFF0C && ch <= 0xFF0F) ? // full-width ,-./
                    true :
                   is_either<wchar_t>(ch, 58, 0xFF1A); // :
            // clang-format on
            }

        /** @returns @c true if a punctuation character can be a thousands or
                radix separator in a number.
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool can_character_form_monetary(const wchar_t ch) noexcept
            {
            // ,. (includes full width)
            return ch == 44 || ch == 46 || ch == 0xFF0C || ch == 0xFF0E;
            }

        /** @returns @c true if a punctuation mark can appear between a word and
                sentence ending punctuation with the punctuation being part of the
                word (the way that money and date/time punctuation do).
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool can_character_appear_between_word_and_eol(const wchar_t ch) noexcept
            {
            /* asterisk, copyright, registration, trademark*/
            return (ch == 0x2A || ch == 0xA9 || ch == 0xAE || ch == 0x2122);
            }

        /** @returns @c true if a character is a quote
                (includes double and single quotes, and smart variations).
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool is_quote(const wchar_t ch) noexcept
            {
            return (is_single_quote(ch) || is_double_quote(ch));
            }

        /** @returns @c true if a character is a single quote (includes Unicode and smart quotes).
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool is_single_quote(const wchar_t ch) noexcept
            {
            // clang-format off
            return (ch == 39) ? // '
                    true :
                   (ch == 0xFF07) ? // full-width apostrophe
                    true :
                   (ch == 96) ? // `
                    true :
                   (ch == 130 || ch == 0x201A) ? // ‚ curved single quote
                    true :
                   (ch == 139 || ch == 0x2039) ? // ‹ left single quote (European)
                    true :
                   (ch == 155 || ch == 0x203A) ? // › right single quote (European)
                    true :
                   (ch == 145 || ch == 146) ? // Windows 1252 quote surrogates (single)
                    true :
                   (ch >= 0x2018 && ch <= 0x201B) ? // smart single quotes
                    true :
                   (ch == 0x300C || ch == 0x300D); // Japanese single quotes
            // clang-format on
            }

        /** @returns @c true if a character is a double quote (includes Unicode and smart quotes).
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool is_double_quote(const wchar_t ch) noexcept
            {
            // clang-format off
            return (ch == 34) ? // " straight double quote
                    true :
                   (ch == 132) ? // „ curved double quote
                    true :
                   (ch == 171 || ch == 187) ? // «» left/right double quote (European)
                    true :
                   (ch >= 147 && ch <= 148) ? // Windows 1252 quote surrogates (double)
                    true :
                   (ch >= 0x201C && ch <= 0x201F) ? // smart double quotes
                    true :
                   (ch == 0x300E || ch == 0x300F); // Japanese double quotes
            // clang-format on
            }

        /** @returns Whether a character sequence is a number (works with wide Unicode numbers too).
            @param word The character stream to be reviewed*/
        [[nodiscard]]
        static bool is_numeric(std::wstring_view word) noexcept
            {
            if (word.empty())
                {
                return false;
                }
            if (word.length() == 1)
                {
                return is_numeric(word[0]);
                }
            if (can_character_prefix_numeral(word[0]) && is_numeric(word[1]))
                {
                return true;
                }

            // if at least half of the characters in the word are numbers,
            // then mark the word as numeric
            size_t numberCount = 0;
            for (size_t i = 0; i < word.length(); ++i)
                {
                assert(word[i]);
                if (is_numeric(word[i]))
                    {
                    ++numberCount;
                    }
                // something like "10000-year" is an exception,
                // should be seen as a regular word
                else if (numberCount > 0 && is_dash_or_hyphen(word[i]) && i + 2 < word.length() &&
                         is_alpha(word[i + 1]) && is_alpha(word[i + 2]))
                    {
                    return false;
                    }
                }
            return (numberCount >= (word.length() / 2));
            }

        /** @returns Whether a character is a number
                (0-9 [wide and narrow], superscripts, subscripts, and fractions).
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool is_numeric(const wchar_t ch) noexcept
            {
            return is_numeric_simple(ch) ?
                       // superscripts and fractions
                       true :
                       is_extended_numeric(ch);
            }

        /** @returns Whether a character is a superscript, subscript, or fraction.
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool is_extended_numeric(const wchar_t ch) noexcept
            {
            return string_util::is_fraction(ch) || string_util::is_superscript_number(ch) ||
                   string_util::is_subscript_number(ch);
            }

        /** @returns Whether a character is a number
                (0-9 characters only, narrow and wide versions).
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool is_numeric_simple(const wchar_t ch) noexcept
            {
            return (ch >= L'0' && ch <= L'9') ?
                       // full-width Unicode numbers 0-9
                       true :
                       ch >= 0xFF10 && ch <= 0xFF19;
            }

        /** @returns Whether a character is a space, tab, newline, carriage return, or form feed.
                Also includes double-width and no-break spaces.
            @param ch The letter to be reviewed.
            @sa is_space_horizontal() and is_space_vertical().*/
        [[nodiscard]]
        constexpr static bool is_space(const wchar_t ch) noexcept
            {
            return (is_space_vertical(ch) || is_space_horizontal(ch));
            }

        /** @returns Whether a character is a horizontal space or tab.
                Also includes double-width and no-break spaces.
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool is_space_horizontal(const wchar_t ch) noexcept
            {
            // clang-format off
            return (ch == 0x20) ? // regular space
                    true :
                   (ch == 0x09) ? // tab
                    true :
                   (ch == 0xA0 || ch == 0x202F) ? // no-break space, narrow
                    true :
                   (ch == 0x3000) ? // Japanese Ideographic Space
                                    // En quad, thin space, hair space, em space,
                                    // zero-width non-joiner (word separator), etc.
                    true :
                   ch >= 0x2000 && ch <= 0x200C;
            // clang-format on
            }

        /** @returns Whether a character is a horizontal space.
                Also includes double-width and no-break spaces.
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool is_space_horizontal_except_tab(const wchar_t ch) noexcept
            {
            // clang-format off
            return (ch == 0x20) ? // regular space
                    true :
                   (ch == 0xA0 || ch == 0x202F) ? // no-break space, narrow
                    true :
                   (ch == 0x3000) ? // Japanese Ideographic Space
                                    // En quad, thin space, hair space, em space,
                                    // zero-width non-joiner (word separator), etc.
                    true :
                   ch >= 0x2000 && ch <= 0x200C;
            // clang-format on
            }

        /** @returns Whether a character is a line-ending type character.
                Also includes carriage returns, line feeds, and form feeds.
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool is_space_vertical(const wchar_t ch) noexcept
            {
            // clang-format off
            return (ch == 0x0D) ? true :
                   (ch == 0x0A) ? true :
                   (ch == 0x0C) ? // form feed
                    true :
                   (ch == 0x2028) ? // line separator
                    true :
                   (ch == 0x2029); // paragraph separator
            // clang-format on
            }

        /** @returns Whether a character is a punctuation mark.
            @param ch The letter to be reviewed.*/
        [[nodiscard]]
        constexpr static bool is_punctuation(const wchar_t ch) noexcept
            {
            // see if it is either a space, control character, or alphanumeric and negate that
            return !(is_numeric(ch) || is_alpha(ch) || is_space(ch) ||
                     // control characters
                     (ch <= 0x20));
            }
        };
    } // namespace characters

#endif // INDEXING_CHARACTERS_H
