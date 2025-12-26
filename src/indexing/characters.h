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
            return (is_upper(ch) || is_lower(ch));
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
                // Russian
                (ch >= 0x0410 && ch <= 0x042F) || (ch == 0x0401));
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
                // Russian
                (ch >= 0x0430 && ch <= 0x044F) || (ch == 0x0451));
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
                (ch == 0x0112) ? 0x0113 : // E with macro
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
                    is_either<wchar_t>(letter, 0x042F, 0x044F));
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
                (letter >= 0xDD && letter <= 0xDF) || // upper Y with acute to Eszett
                (letter == 0xE7) ||                   // lower C with cedilla
                (letter == 0xF0) ||                   // lower Eth
                (letter == 0xF1) ||                   // lower N with tilde
                (letter >= 0xFD && letter <= 0xFF) || // lower Y with acute to Y with umlauts
                (letter == 0x0178) ||
                // basic Russian alphabet
                (letter >= 0x0411 && letter <= 0x414) || (letter >= 0x0416 && letter <= 0x417) ||
                (letter >= 0x041A && letter <= 0x041D) || (letter >= 0x041F && letter <= 0x0422) ||
                (letter >= 0x0424 && letter <= 0x042A) ||
                is_either<wchar_t>(letter, 0x042C, 0x044C) ||
                (letter >= 0x0431 && letter <= 0x434) || (letter >= 0x0436 && letter <= 0x437) ||
                (letter >= 0x043A && letter <= 0x043D) || (letter >= 0x043F && letter <= 0x0442) ||
                (letter >= 0x0444 && letter <= 0x044A));
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
                   (ch == 0x9F) ? // Y with diaeresis
                    true :
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
                   (ch == 0x9F) ? // Y with diaeresis
                    true :
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
                   (ch == 0x9F) ? // Y with diaeresis
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
                   (ch == 159) ? // Y with diaeresis
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
                    ch == 0xFF61 /*half-width, more commonly used*/);
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
