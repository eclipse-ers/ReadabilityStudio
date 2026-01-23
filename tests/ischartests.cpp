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

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/indexing/word_collection.h"
#include "../src/indexing/word.h"

// clang-format off
// NOLINTBEGIN

using namespace grammar;
using namespace characters;

using MYWORD = word<traits::case_insensitive_ex,
    stemming::english_stem<std::basic_string<wchar_t, traits::case_insensitive_ex> > >;

extern word_list Stop_list;

TEST_CASE("ischaracter", "[ischaracter]")
    {
    std::wstring lowerLettersEnglish = L"abcdefghijklmnopqrstuvwxyz";
    std::wstring upperLettersEnglish = L"ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    std::wstring lowerLettersEnglishFullWidth = L"ａｂｃｄｅｆｇｈｉｊｋｌｍｎｏｐｑｒｓｔｕｖｗｘｙｚ";
    std::wstring upperLettersEnglishFullWidth = L"ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯＰＱＲＳＴＵＶＷＸＹＺ";

    std::wstring vowelsWestern = L"aeiouyàáâãäåæèéêëìíîïòóôõöøùúûüœAEIOUYÀÁÂÃÄÅÆÈÉÊËÌÍÎÏÒÓÔÕÖØÙÚÛÜŒ";
    std::wstring vowelsWesternFullWidth = L"ａｅｉｏｕｙＡＥＩＯＵＹ";
    std::wstring consonantsWestern = L"bcdfghjklmnpqrstvwxzðçñýþÿBCDFGHJKLMNPQRSTVWXZÐÇÑÝÞŸß";
    std::wstring consonantsWesternFullWidth = L"ｂｃｄｆｇｈｊｋｌｍｎｐｑｒｓｔｖｗｘｚＢＣＤＦＧＨＪＫＬＭＮＰＱＲＳＴＶＷＸＺ";
    std::wstring lowerLettersWestern = L"abcdefghijklmnopqrstuvwxyzàáâãäåæçèéêëìíîïðñòóôõöøùúûüýþÿœ";
    std::wstring upperLettersWestern = L"ABCDEFGHIJKLMNOPQRSTUVWXYZÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝÞŸŒ";
    std::wstring otherLettersWestern = L"ß";

    std::wstring lowerLettersRussian = L"абвгдеёжзийклмнопрстуфхцчшщъыьэюя";
    std::wstring upperLettersRussian = L"АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ";
    std::wstring vowelsRussian = L"АЯЭЕУЮОЁЫИЙаяэеуюоёыий";
    std::wstring consonantsRussian = L"бвгджзклмнпрстфхцчшщъьБВГДЖЗКЛМНПРСТФХЦЧШЩЪЬ";

    std::wstring numbersWestern = L"0123456789";
    std::wstring numbersJapanese = L"０１２３４５６７８９";

    SECTION("Western Numbers")
        {
        for (size_t i = 0 ; i < numbersWestern.length(); ++i)
            {
            CHECK(is_character::is_numeric(numbersWestern[i]));
            CHECK(is_character::is_numeric_simple(numbersWestern[i]));
            }
        }

    SECTION("Japanese Numbers")
        {
        for (size_t i = 0 ; i < numbersJapanese.length(); ++i)
            {
            CHECK(is_character::is_numeric(numbersJapanese[i]));
            CHECK(is_character::is_numeric_simple(numbersJapanese[i]));
            // not included in 8-bit version
            CHECK_FALSE(string_util::is_numeric_8bit(numbersJapanese[i]));
            }
        }

    SECTION("Is Alpha Russian")
        {
        for (size_t i = 0 ; i < lowerLettersRussian.length(); ++i)
            {
            CHECK(is_character::is_alpha(lowerLettersRussian[i]));
            CHECK_FALSE(is_character::is_alpha_8bit(lowerLettersRussian[i]));
            }
        for (size_t i = 0 ; i < upperLettersRussian.length(); ++i)
            {
            CHECK(is_character::is_alpha(upperLettersRussian[i]));
            CHECK_FALSE(is_character::is_alpha_8bit(upperLettersRussian[i]));
            }
        }

    SECTION("Is Vowel Russian")
        {
        for (size_t i = 0 ; i < vowelsRussian.length(); ++i)
            {
            CHECK(is_character::is_vowel(vowelsRussian[i]));
            }
        for (size_t i = 0 ; i < consonantsRussian.length(); ++i)
            {
            CHECK_FALSE(is_character::is_vowel(consonantsRussian[i]));
            }
        }

    SECTION("Is Lowe rRussian")
        {
        for (size_t i = 0 ; i < lowerLettersRussian.length(); ++i)
            {
            CHECK(is_character::is_lower(lowerLettersRussian[i]));
            CHECK_FALSE(is_character::is_upper(lowerLettersRussian[i]));
            }
        }

    SECTION("Is Upper Russian")
        {
        for (size_t i = 0 ; i < upperLettersRussian.length(); ++i)
            {
            CHECK_FALSE(is_character::is_lower(upperLettersRussian[i]));
            CHECK(is_character::is_upper(upperLettersRussian[i]));
            }
        }

    SECTION("Is Consonants Russian")
        {
        for (size_t i = 0 ; i < vowelsRussian.length(); ++i)
            {
            CHECK_FALSE(is_character::is_consonant(vowelsRussian[i]));
            }
        for (size_t i = 0 ; i < consonantsRussian.length(); ++i)
            {
            CHECK(is_character::is_consonant(consonantsRussian[i]));
            }
        }

    SECTION("To Lower Russian")
        {
        for (size_t i = 0 ; i < lowerLettersRussian.length(); ++i)
            {
            CHECK(is_character::to_lower(lowerLettersRussian[i]) == lowerLettersRussian[i]);
            }
        for (size_t i = 0 ; i < upperLettersRussian.length(); ++i)
            {
            CHECK(is_character::to_lower(upperLettersRussian[i]) == lowerLettersRussian[i]);
            }
        }

    SECTION("Is Vowel Western")
        {
        for (size_t i = 0 ; i < vowelsWestern.length(); ++i)
            {
            CHECK(is_character::is_vowel(vowelsWestern[i]));
            }
        for (size_t i = 0 ; i < consonantsWestern.length(); ++i)
            {
            CHECK_FALSE(is_character::is_vowel(consonantsWestern[i]));
            }
        for (size_t i = 0 ; i < vowelsWesternFullWidth.length(); ++i)
            {
            CHECK(is_character::is_vowel(vowelsWesternFullWidth[i]));
            }
        for (size_t i = 0 ; i < consonantsWesternFullWidth.length(); ++i)
            {
            CHECK_FALSE(is_character::is_vowel(consonantsWesternFullWidth[i]));
            }
        }

    SECTION("Is Consonants Western")
        {
        for (size_t i = 0 ; i < vowelsWestern.length(); ++i)
            {
            CHECK_FALSE(is_character::is_consonant(vowelsWestern[i]));
            }
        for (size_t i = 0 ; i < consonantsWestern.length(); ++i)
            {
            CHECK(is_character::is_consonant(consonantsWestern[i]));
            }
        for (size_t i = 0 ; i < vowelsWesternFullWidth.length(); ++i)
            {
            CHECK_FALSE(is_character::is_consonant(vowelsWesternFullWidth[i]));
            }
        for (size_t i = 0 ; i < consonantsWesternFullWidth.length(); ++i)
            {
            CHECK(is_character::is_consonant(consonantsWesternFullWidth[i]));
            }
        }

    SECTION("To Lower Western")
        {
        for (size_t i = 0 ; i < lowerLettersWestern.length(); ++i)
            {
            CHECK(is_character::to_lower(lowerLettersWestern[i]) == lowerLettersWestern[i]);
            }
        for (size_t i = 0 ; i < upperLettersWestern.length(); ++i)
            {
            CHECK(is_character::to_lower(upperLettersWestern[i]) == lowerLettersWestern[i]);
            }
        for (size_t i = 0 ; i < otherLettersWestern.length(); ++i)
            {
            CHECK(is_character::to_lower(otherLettersWestern[i]) == otherLettersWestern[i]);
            }
        for (size_t i = 0 ; i < lowerLettersEnglishFullWidth.length(); ++i)
            {
            CHECK(is_character::to_lower(lowerLettersEnglishFullWidth[i]) == lowerLettersEnglishFullWidth[i]);
            }
        for (size_t i = 0 ; i < upperLettersEnglishFullWidth.length(); ++i)
            {
            CHECK(is_character::to_lower(upperLettersEnglishFullWidth[i]) == lowerLettersEnglishFullWidth[i]);
            }
        }

    SECTION("Is Alpha English")
        {
        for (size_t i = 0 ; i < lowerLettersEnglish.length(); ++i)
            {
            CHECK(is_character::is_alpha(lowerLettersEnglish[i]));
            CHECK(is_character::is_alpha_8bit(lowerLettersEnglish[i]));
            }
        for (size_t i = 0 ; i < upperLettersEnglish.length(); ++i)
            {
            CHECK(is_character::is_alpha(upperLettersEnglish[i]));
            CHECK(is_character::is_alpha_8bit(upperLettersEnglish[i]));
            }
        // full width
        for (size_t i = 0 ; i < lowerLettersEnglishFullWidth.length(); ++i)
            {
            CHECK(is_character::is_alpha(lowerLettersEnglishFullWidth[i]));
            // not simple
            CHECK_FALSE(is_character::is_alpha_8bit(lowerLettersEnglishFullWidth[i]));
            }
        for (size_t i = 0 ; i < upperLettersEnglishFullWidth.length(); ++i)
            {
            CHECK(is_character::is_alpha(upperLettersEnglishFullWidth[i]));
            // not simple
            CHECK_FALSE(is_character::is_alpha_8bit(upperLettersEnglishFullWidth[i]));
            }
        }

    SECTION("Is Alpha Western")
        {
        for (size_t i = 0 ; i < lowerLettersWestern.length(); ++i)
            {
            CHECK(is_character::is_alpha(lowerLettersWestern[i]));
            //won't support French/German/etc. characters
            if (i <= 25)
                { CHECK(is_character::is_alpha_8bit(lowerLettersWestern[i])); }
            else
                { CHECK_FALSE(is_character::is_alpha_8bit(lowerLettersWestern[i])); }
            }
        for (size_t i = 0 ; i < upperLettersWestern.length(); ++i)
            {
            CHECK(is_character::is_alpha(upperLettersWestern[i]));
            if (i <= 25)
                { CHECK(is_character::is_alpha_8bit(upperLettersWestern[i])); }
            else
                { CHECK_FALSE(is_character::is_alpha_8bit(upperLettersWestern[i])); }
            }
        for (size_t i = 0 ; i < otherLettersWestern.length(); ++i)
            {
            CHECK(is_character::is_alpha(otherLettersWestern[i]));
            }
        for (size_t i = 0 ; i < lowerLettersEnglishFullWidth.length(); ++i)
            {
            CHECK(is_character::is_alpha(lowerLettersEnglishFullWidth[i]));
            }
        for (size_t i = 0 ; i < upperLettersEnglishFullWidth.length(); ++i)
            {
            CHECK(is_character::is_alpha(upperLettersEnglishFullWidth[i]));
            }
        }

    SECTION("Is Lower Western")
        {
        for (size_t i = 0 ; i < lowerLettersWestern.length(); ++i)
            {
            CHECK(is_character::is_lower(lowerLettersWestern[i]));
            CHECK_FALSE(is_character::is_upper(lowerLettersWestern[i]));
            }
        for (size_t i = 0 ; i < lowerLettersEnglishFullWidth.length(); ++i)
            {
            CHECK(is_character::is_lower(lowerLettersEnglishFullWidth[i]));
            CHECK_FALSE(is_character::is_upper(lowerLettersEnglishFullWidth[i]));
            }
        }

    SECTION("Is Upper Western")
        {
        for (size_t i = 0 ; i < upperLettersWestern.length(); ++i)
            {
            CHECK_FALSE(is_character::is_lower(upperLettersWestern[i]));
            CHECK(is_character::is_upper(upperLettersWestern[i]));
            // doesn't have a lowercase variation, so do it here
            CHECK_FALSE(is_character::is_lower(L'Ÿ'));
            CHECK(is_character::is_upper(L'Ÿ'));
            }
        for (size_t i = 0 ; i < upperLettersEnglishFullWidth.length(); ++i)
            {
            CHECK_FALSE(is_character::is_lower(upperLettersEnglishFullWidth[i]));
            CHECK(is_character::is_upper(upperLettersEnglishFullWidth[i]));
            }
        }

    SECTION("Vietnamese O With Dot Below")
        {
        // U+1ECD = ọ (lowercase o with dot below)
        // U+1ECC = Ọ (uppercase O with dot below)
        constexpr wchar_t LATIN_SMALL_O_DOT_BELOW = L'\x1ECD';
        constexpr wchar_t LATIN_CAPITAL_O_DOT_BELOW = L'\x1ECC';

        // is_lower
        CHECK(is_character::is_lower(LATIN_SMALL_O_DOT_BELOW));
        CHECK_FALSE(is_character::is_lower(LATIN_CAPITAL_O_DOT_BELOW));

        // is_upper
        CHECK_FALSE(is_character::is_upper(LATIN_SMALL_O_DOT_BELOW));
        CHECK(is_character::is_upper(LATIN_CAPITAL_O_DOT_BELOW));

        // is_vowel (both are variants of 'o')
        CHECK(is_character::is_vowel(LATIN_SMALL_O_DOT_BELOW));
        CHECK(is_character::is_vowel(LATIN_CAPITAL_O_DOT_BELOW));

        // is_consonant (should be false for vowels)
        CHECK_FALSE(is_character::is_consonant(LATIN_SMALL_O_DOT_BELOW));
        CHECK_FALSE(is_character::is_consonant(LATIN_CAPITAL_O_DOT_BELOW));

        // is_alpha
        CHECK(is_character::is_alpha(LATIN_SMALL_O_DOT_BELOW));
        CHECK(is_character::is_alpha(LATIN_CAPITAL_O_DOT_BELOW));

        // to_lower (uppercase should convert to lowercase)
        CHECK(is_character::to_lower(LATIN_CAPITAL_O_DOT_BELOW) == LATIN_SMALL_O_DOT_BELOW);
        CHECK(is_character::to_lower(LATIN_SMALL_O_DOT_BELOW) == LATIN_SMALL_O_DOT_BELOW);
        }
    }
// NOLINTEND
// clang-format on
