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

    SECTION("Polish Ogonek")
        {
        CHECK(is_character::is_lower(L'ą'));
        CHECK(is_character::is_upper(L'Ą'));
        CHECK(is_character::is_lower(L'ę'));
        CHECK(is_character::is_upper(L'Ę'));
        CHECK(is_character::is_vowel(L'ą'));
        CHECK(is_character::is_vowel(L'Ą'));
        CHECK(is_character::is_vowel(L'ę'));
        CHECK(is_character::is_vowel(L'Ę'));
        CHECK_FALSE(is_character::is_consonant(L'ą'));
        CHECK_FALSE(is_character::is_consonant(L'Ą'));
        CHECK_FALSE(is_character::is_consonant(L'ę'));
        CHECK_FALSE(is_character::is_consonant(L'Ę'));
        CHECK(is_character::to_lower(L'Ą') == L'ą');
        CHECK(is_character::to_lower(L'Ę') == L'ę');
        }

    SECTION("Lithuanian Ogonek")
        {
        CHECK(is_character::is_lower(L'į'));
        CHECK(is_character::is_upper(L'Į'));
        CHECK(is_character::is_lower(L'ų'));
        CHECK(is_character::is_upper(L'Ų'));
        CHECK(is_character::is_vowel(L'į'));
        CHECK(is_character::is_vowel(L'Į'));
        CHECK(is_character::is_vowel(L'ų'));
        CHECK(is_character::is_vowel(L'Ų'));
        CHECK_FALSE(is_character::is_consonant(L'į'));
        CHECK_FALSE(is_character::is_consonant(L'Į'));
        CHECK_FALSE(is_character::is_consonant(L'ų'));
        CHECK_FALSE(is_character::is_consonant(L'Ų'));
        CHECK(is_character::to_lower(L'Į') == L'į');
        CHECK(is_character::to_lower(L'Ų') == L'ų');
        }

    SECTION("Portuguese Tilde Vowels")
        {
        CHECK(is_character::is_lower(L'ã'));
        CHECK(is_character::is_upper(L'Ã'));
        CHECK(is_character::is_lower(L'õ'));
        CHECK(is_character::is_upper(L'Õ'));
        CHECK(is_character::is_vowel(L'ã'));
        CHECK(is_character::is_vowel(L'Ã'));
        CHECK(is_character::is_vowel(L'õ'));
        CHECK(is_character::is_vowel(L'Õ'));
        CHECK_FALSE(is_character::is_consonant(L'ã'));
        CHECK_FALSE(is_character::is_consonant(L'Ã'));
        CHECK_FALSE(is_character::is_consonant(L'õ'));
        CHECK_FALSE(is_character::is_consonant(L'Õ'));
        CHECK(is_character::to_lower(L'Ã') == L'ã');
        CHECK(is_character::to_lower(L'Õ') == L'õ');
        }

    SECTION("Macron Vowels")
        {
        CHECK(is_character::is_lower(L'ā'));
        CHECK(is_character::is_upper(L'Ā'));
        CHECK(is_character::is_lower(L'ē'));
        CHECK(is_character::is_upper(L'Ē'));
        CHECK(is_character::is_lower(L'ī'));
        CHECK(is_character::is_upper(L'Ī'));
        CHECK(is_character::is_lower(L'ō'));
        CHECK(is_character::is_upper(L'Ō'));
        CHECK(is_character::is_lower(L'ū'));
        CHECK(is_character::is_upper(L'Ū'));
        CHECK(is_character::is_vowel(L'ā'));
        CHECK(is_character::is_vowel(L'Ā'));
        CHECK(is_character::is_vowel(L'ē'));
        CHECK(is_character::is_vowel(L'Ē'));
        CHECK(is_character::is_vowel(L'ī'));
        CHECK(is_character::is_vowel(L'Ī'));
        CHECK(is_character::is_vowel(L'ō'));
        CHECK(is_character::is_vowel(L'Ō'));
        CHECK(is_character::is_vowel(L'ū'));
        CHECK(is_character::is_vowel(L'Ū'));
        CHECK_FALSE(is_character::is_consonant(L'ā'));
        CHECK_FALSE(is_character::is_consonant(L'Ā'));
        CHECK_FALSE(is_character::is_consonant(L'ē'));
        CHECK_FALSE(is_character::is_consonant(L'Ē'));
        CHECK_FALSE(is_character::is_consonant(L'ī'));
        CHECK_FALSE(is_character::is_consonant(L'Ī'));
        CHECK_FALSE(is_character::is_consonant(L'ō'));
        CHECK_FALSE(is_character::is_consonant(L'Ō'));
        CHECK_FALSE(is_character::is_consonant(L'ū'));
        CHECK_FALSE(is_character::is_consonant(L'Ū'));
        CHECK(is_character::to_lower(L'Ā') == L'ā');
        CHECK(is_character::to_lower(L'Ē') == L'ē');
        CHECK(is_character::to_lower(L'Ī') == L'ī');
        CHECK(is_character::to_lower(L'Ō') == L'ō');
        CHECK(is_character::to_lower(L'Ū') == L'ū');
        }

    SECTION("Romanian Breve")
        {
        CHECK(is_character::is_lower(L'ă'));
        CHECK(is_character::is_upper(L'Ă'));
        CHECK(is_character::is_vowel(L'ă'));
        CHECK(is_character::is_vowel(L'Ă'));
        CHECK_FALSE(is_character::is_consonant(L'ă'));
        CHECK_FALSE(is_character::is_consonant(L'Ă'));
        CHECK(is_character::to_lower(L'Ă') == L'ă');
        }

    SECTION("Hungarian Double Acute")
        {
        CHECK(is_character::is_lower(L'ő'));
        CHECK(is_character::is_upper(L'Ő'));
        CHECK(is_character::is_lower(L'ű'));
        CHECK(is_character::is_upper(L'Ű'));
        CHECK(is_character::is_vowel(L'ő'));
        CHECK(is_character::is_vowel(L'Ő'));
        CHECK(is_character::is_vowel(L'ű'));
        CHECK(is_character::is_vowel(L'Ű'));
        CHECK_FALSE(is_character::is_consonant(L'ő'));
        CHECK_FALSE(is_character::is_consonant(L'Ő'));
        CHECK_FALSE(is_character::is_consonant(L'ű'));
        CHECK_FALSE(is_character::is_consonant(L'Ű'));
        CHECK(is_character::to_lower(L'Ő') == L'ő');
        CHECK(is_character::to_lower(L'Ű') == L'ű');
        }

    SECTION("Turkish Cedilla")
        {
        CHECK(is_character::is_lower(L'ş'));
        CHECK(is_character::is_upper(L'Ş'));
        CHECK(is_character::is_lower(L'ţ'));
        CHECK(is_character::is_upper(L'Ţ'));
        CHECK(is_character::is_consonant(L'ş'));
        CHECK(is_character::is_consonant(L'Ş'));
        CHECK(is_character::is_consonant(L'ţ'));
        CHECK(is_character::is_consonant(L'Ţ'));
        CHECK_FALSE(is_character::is_vowel(L'ş'));
        CHECK_FALSE(is_character::is_vowel(L'Ş'));
        CHECK_FALSE(is_character::is_vowel(L'ţ'));
        CHECK_FALSE(is_character::is_vowel(L'Ţ'));
        CHECK(is_character::to_lower(L'Ş') == L'ş');
        CHECK(is_character::to_lower(L'Ţ') == L'ţ');
        }

    SECTION("Romanian Comma Below")
        {
        CHECK(is_character::is_lower(L'ș'));
        CHECK(is_character::is_upper(L'Ș'));
        CHECK(is_character::is_lower(L'ț'));
        CHECK(is_character::is_upper(L'Ț'));
        CHECK(is_character::is_consonant(L'ș'));
        CHECK(is_character::is_consonant(L'Ș'));
        CHECK(is_character::is_consonant(L'ț'));
        CHECK(is_character::is_consonant(L'Ț'));
        CHECK_FALSE(is_character::is_vowel(L'ș'));
        CHECK_FALSE(is_character::is_vowel(L'Ș'));
        CHECK_FALSE(is_character::is_vowel(L'ț'));
        CHECK_FALSE(is_character::is_vowel(L'Ț'));
        CHECK(is_character::to_lower(L'Ș') == L'ș');
        CHECK(is_character::to_lower(L'Ț') == L'ț');
        }

    SECTION("Vietnamese Dot Below Extended")
        {
        // ạ Ạ ẹ Ẹ ị Ị ụ Ụ (ọ Ọ already tested)
        CHECK(is_character::is_lower(L'ạ'));
        CHECK(is_character::is_upper(L'Ạ'));
        CHECK(is_character::is_lower(L'ẹ'));
        CHECK(is_character::is_upper(L'Ẹ'));
        CHECK(is_character::is_lower(L'ị'));
        CHECK(is_character::is_upper(L'Ị'));
        CHECK(is_character::is_lower(L'ụ'));
        CHECK(is_character::is_upper(L'Ụ'));
        CHECK(is_character::is_vowel(L'ạ'));
        CHECK(is_character::is_vowel(L'Ạ'));
        CHECK(is_character::is_vowel(L'ẹ'));
        CHECK(is_character::is_vowel(L'Ẹ'));
        CHECK(is_character::is_vowel(L'ị'));
        CHECK(is_character::is_vowel(L'Ị'));
        CHECK(is_character::is_vowel(L'ụ'));
        CHECK(is_character::is_vowel(L'Ụ'));
        CHECK_FALSE(is_character::is_consonant(L'ạ'));
        CHECK_FALSE(is_character::is_consonant(L'Ạ'));
        CHECK_FALSE(is_character::is_consonant(L'ẹ'));
        CHECK_FALSE(is_character::is_consonant(L'Ẹ'));
        CHECK_FALSE(is_character::is_consonant(L'ị'));
        CHECK_FALSE(is_character::is_consonant(L'Ị'));
        CHECK_FALSE(is_character::is_consonant(L'ụ'));
        CHECK_FALSE(is_character::is_consonant(L'Ụ'));
        CHECK(is_character::to_lower(L'Ạ') == L'ạ');
        CHECK(is_character::to_lower(L'Ẹ') == L'ẹ');
        CHECK(is_character::to_lower(L'Ị') == L'ị');
        CHECK(is_character::to_lower(L'Ụ') == L'ụ');
        }

    SECTION("Vietnamese Hook Above")
        {
        CHECK(is_character::is_lower(L'ả'));
        CHECK(is_character::is_upper(L'Ả'));
        CHECK(is_character::is_lower(L'ẻ'));
        CHECK(is_character::is_upper(L'Ẻ'));
        CHECK(is_character::is_lower(L'ỉ'));
        CHECK(is_character::is_upper(L'Ỉ'));
        CHECK(is_character::is_lower(L'ỏ'));
        CHECK(is_character::is_upper(L'Ỏ'));
        CHECK(is_character::is_lower(L'ủ'));
        CHECK(is_character::is_upper(L'Ủ'));
        CHECK(is_character::is_lower(L'ỷ'));
        CHECK(is_character::is_upper(L'Ỷ'));
        CHECK(is_character::is_vowel(L'ả'));
        CHECK(is_character::is_vowel(L'Ả'));
        CHECK(is_character::is_vowel(L'ẻ'));
        CHECK(is_character::is_vowel(L'Ẻ'));
        CHECK(is_character::is_vowel(L'ỉ'));
        CHECK(is_character::is_vowel(L'Ỉ'));
        CHECK(is_character::is_vowel(L'ỏ'));
        CHECK(is_character::is_vowel(L'Ỏ'));
        CHECK(is_character::is_vowel(L'ủ'));
        CHECK(is_character::is_vowel(L'Ủ'));
        CHECK(is_character::is_vowel(L'ỷ'));
        CHECK(is_character::is_vowel(L'Ỷ'));
        CHECK_FALSE(is_character::is_consonant(L'ả'));
        CHECK_FALSE(is_character::is_consonant(L'Ả'));
        CHECK_FALSE(is_character::is_consonant(L'ẻ'));
        CHECK_FALSE(is_character::is_consonant(L'Ẻ'));
        CHECK_FALSE(is_character::is_consonant(L'ỉ'));
        CHECK_FALSE(is_character::is_consonant(L'Ỉ'));
        CHECK_FALSE(is_character::is_consonant(L'ỏ'));
        CHECK_FALSE(is_character::is_consonant(L'Ỏ'));
        CHECK_FALSE(is_character::is_consonant(L'ủ'));
        CHECK_FALSE(is_character::is_consonant(L'Ủ'));
        CHECK_FALSE(is_character::is_consonant(L'ỷ'));
        CHECK_FALSE(is_character::is_consonant(L'Ỷ'));
        CHECK(is_character::to_lower(L'Ả') == L'ả');
        CHECK(is_character::to_lower(L'Ẻ') == L'ẻ');
        CHECK(is_character::to_lower(L'Ỉ') == L'ỉ');
        CHECK(is_character::to_lower(L'Ỏ') == L'ỏ');
        CHECK(is_character::to_lower(L'Ủ') == L'ủ');
        CHECK(is_character::to_lower(L'Ỷ') == L'ỷ');
        }

    SECTION("Vietnamese Horn")
        {
        CHECK(is_character::is_lower(L'ơ'));
        CHECK(is_character::is_upper(L'Ơ'));
        CHECK(is_character::is_lower(L'ư'));
        CHECK(is_character::is_upper(L'Ư'));
        CHECK(is_character::is_vowel(L'ơ'));
        CHECK(is_character::is_vowel(L'Ơ'));
        CHECK(is_character::is_vowel(L'ư'));
        CHECK(is_character::is_vowel(L'Ư'));
        CHECK_FALSE(is_character::is_consonant(L'ơ'));
        CHECK_FALSE(is_character::is_consonant(L'Ơ'));
        CHECK_FALSE(is_character::is_consonant(L'ư'));
        CHECK_FALSE(is_character::is_consonant(L'Ư'));
        CHECK(is_character::to_lower(L'Ơ') == L'ơ');
        CHECK(is_character::to_lower(L'Ư') == L'ư');
        }
    }
// NOLINTEND
// clang-format on
