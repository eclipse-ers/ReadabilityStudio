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

    std::wstring vowelsWestern = L"aeiouyàáâãäåæèéêëìíîïòóôõöøùúûüœýÿAEIOUYÀÁÂÃÄÅÆÈÉÊËÌÍÎÏÒÓÔÕÖØÙÚÛÜŒÝŸ";
    std::wstring vowelsWesternFullWidth = L"ａｅｉｏｕｙＡＥＩＯＵＹ";
    std::wstring consonantsWestern = L"bcdfghjklmnpqrstvwxzðçñþBCDFGHJKLMNPQRSTVWXZÐÇÑÞß";
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

    SECTION("Polish Acute Consonants")
        {
        // Ć/ć
        CHECK(is_character::is_lower(L'ć'));
        CHECK(is_character::is_upper(L'Ć'));
        CHECK(is_character::is_consonant(L'ć'));
        CHECK(is_character::is_consonant(L'Ć'));
        CHECK_FALSE(is_character::is_vowel(L'ć'));
        CHECK_FALSE(is_character::is_vowel(L'Ć'));
        CHECK(is_character::to_lower(L'Ć') == L'ć');
        // Ś/ś
        CHECK(is_character::is_lower(L'ś'));
        CHECK(is_character::is_upper(L'Ś'));
        CHECK(is_character::is_consonant(L'ś'));
        CHECK(is_character::is_consonant(L'Ś'));
        CHECK_FALSE(is_character::is_vowel(L'ś'));
        CHECK_FALSE(is_character::is_vowel(L'Ś'));
        CHECK(is_character::to_lower(L'Ś') == L'ś');
        // Ź/ź
        CHECK(is_character::is_lower(L'ź'));
        CHECK(is_character::is_upper(L'Ź'));
        CHECK(is_character::is_consonant(L'ź'));
        CHECK(is_character::is_consonant(L'Ź'));
        CHECK_FALSE(is_character::is_vowel(L'ź'));
        CHECK_FALSE(is_character::is_vowel(L'Ź'));
        CHECK(is_character::to_lower(L'Ź') == L'ź');
        }

    SECTION("Polish Dot Above")
        {
        // Ż/ż
        CHECK(is_character::is_lower(L'ż'));
        CHECK(is_character::is_upper(L'Ż'));
        CHECK(is_character::is_consonant(L'ż'));
        CHECK(is_character::is_consonant(L'Ż'));
        CHECK_FALSE(is_character::is_vowel(L'ż'));
        CHECK_FALSE(is_character::is_vowel(L'Ż'));
        CHECK(is_character::to_lower(L'Ż') == L'ż');
        }

    SECTION("Czech Ring")
        {
        // Ů/ů
        CHECK(is_character::is_lower(L'ů'));
        CHECK(is_character::is_upper(L'Ů'));
        CHECK(is_character::is_vowel(L'ů'));
        CHECK(is_character::is_vowel(L'Ů'));
        CHECK_FALSE(is_character::is_consonant(L'ů'));
        CHECK_FALSE(is_character::is_consonant(L'Ů'));
        CHECK(is_character::to_lower(L'Ů') == L'ů');
        }

    SECTION("Czech Caron Vowel")
        {
        // Ě/ě
        CHECK(is_character::is_lower(L'ě'));
        CHECK(is_character::is_upper(L'Ě'));
        CHECK(is_character::is_vowel(L'ě'));
        CHECK(is_character::is_vowel(L'Ě'));
        CHECK_FALSE(is_character::is_consonant(L'ě'));
        CHECK_FALSE(is_character::is_consonant(L'Ě'));
        CHECK(is_character::to_lower(L'Ě') == L'ě');
        }

    SECTION("Caron Consonants")
        {
        // Č/č
        CHECK(is_character::is_lower(L'č'));
        CHECK(is_character::is_upper(L'Č'));
        CHECK(is_character::is_consonant(L'č'));
        CHECK(is_character::is_consonant(L'Č'));
        CHECK_FALSE(is_character::is_vowel(L'č'));
        CHECK_FALSE(is_character::is_vowel(L'Č'));
        CHECK(is_character::to_lower(L'Č') == L'č');
        // Ň/ň
        CHECK(is_character::is_lower(L'ň'));
        CHECK(is_character::is_upper(L'Ň'));
        CHECK(is_character::is_consonant(L'ň'));
        CHECK(is_character::is_consonant(L'Ň'));
        CHECK_FALSE(is_character::is_vowel(L'ň'));
        CHECK_FALSE(is_character::is_vowel(L'Ň'));
        CHECK(is_character::to_lower(L'Ň') == L'ň');
        // Ř/ř
        CHECK(is_character::is_lower(L'ř'));
        CHECK(is_character::is_upper(L'Ř'));
        CHECK(is_character::is_consonant(L'ř'));
        CHECK(is_character::is_consonant(L'Ř'));
        CHECK_FALSE(is_character::is_vowel(L'ř'));
        CHECK_FALSE(is_character::is_vowel(L'Ř'));
        CHECK(is_character::to_lower(L'Ř') == L'ř');
        // Š/š
        CHECK(is_character::is_lower(L'š'));
        CHECK(is_character::is_upper(L'Š'));
        CHECK(is_character::is_consonant(L'š'));
        CHECK(is_character::is_consonant(L'Š'));
        CHECK_FALSE(is_character::is_vowel(L'š'));
        CHECK_FALSE(is_character::is_vowel(L'Š'));
        CHECK(is_character::to_lower(L'Š') == L'š');
        // Ž/ž
        CHECK(is_character::is_lower(L'ž'));
        CHECK(is_character::is_upper(L'Ž'));
        CHECK(is_character::is_consonant(L'ž'));
        CHECK(is_character::is_consonant(L'Ž'));
        CHECK_FALSE(is_character::is_vowel(L'ž'));
        CHECK_FALSE(is_character::is_vowel(L'Ž'));
        CHECK(is_character::to_lower(L'Ž') == L'ž');
        // Ť/ť
        CHECK(is_character::is_lower(L'ť'));
        CHECK(is_character::is_upper(L'Ť'));
        CHECK(is_character::is_consonant(L'ť'));
        CHECK(is_character::is_consonant(L'Ť'));
        CHECK_FALSE(is_character::is_vowel(L'ť'));
        CHECK_FALSE(is_character::is_vowel(L'Ť'));
        CHECK(is_character::to_lower(L'Ť') == L'ť');
        // Ď/ď
        CHECK(is_character::is_lower(L'ď'));
        CHECK(is_character::is_upper(L'Ď'));
        CHECK(is_character::is_consonant(L'ď'));
        CHECK(is_character::is_consonant(L'Ď'));
        CHECK_FALSE(is_character::is_vowel(L'ď'));
        CHECK_FALSE(is_character::is_vowel(L'Ď'));
        CHECK(is_character::to_lower(L'Ď') == L'ď');
        }

    SECTION("Polish Stroke")
        {
        // Ł/ł
        CHECK(is_character::is_lower(L'ł'));
        CHECK(is_character::is_upper(L'Ł'));
        CHECK(is_character::is_consonant(L'ł'));
        CHECK(is_character::is_consonant(L'Ł'));
        CHECK_FALSE(is_character::is_vowel(L'ł'));
        CHECK_FALSE(is_character::is_vowel(L'Ł'));
        CHECK(is_character::to_lower(L'Ł') == L'ł');
        }

    SECTION("Turkish G With Breve")
        {
        // Ğ/ğ
        CHECK(is_character::is_lower(L'ğ'));
        CHECK(is_character::is_upper(L'Ğ'));
        CHECK(is_character::is_consonant(L'ğ'));
        CHECK(is_character::is_consonant(L'Ğ'));
        CHECK_FALSE(is_character::is_vowel(L'ğ'));
        CHECK_FALSE(is_character::is_vowel(L'Ğ'));
        CHECK(is_character::to_lower(L'Ğ') == L'ğ');
        }

    SECTION("Turkish Dotted I and Dotless I")
        {
        // İ/ı - special Turkish I
        CHECK(is_character::is_lower(L'ı'));
        CHECK(is_character::is_upper(L'İ'));
        CHECK(is_character::is_vowel(L'ı'));
        CHECK(is_character::is_vowel(L'İ'));
        CHECK_FALSE(is_character::is_consonant(L'ı'));
        CHECK_FALSE(is_character::is_consonant(L'İ'));
        CHECK(is_character::to_lower(L'İ') == L'i');
        }

    SECTION("Polish N With Acute")
        {
        // Ń/ń
        CHECK(is_character::is_lower(L'ń'));
        CHECK(is_character::is_upper(L'Ń'));
        CHECK(is_character::is_consonant(L'ń'));
        CHECK(is_character::is_consonant(L'Ń'));
        CHECK_FALSE(is_character::is_vowel(L'ń'));
        CHECK_FALSE(is_character::is_vowel(L'Ń'));
        CHECK(is_character::to_lower(L'Ń') == L'ń');
        }

    SECTION("Slovak L With Caron")
        {
        // Ľ/ľ
        CHECK(is_character::is_lower(L'ľ'));
        CHECK(is_character::is_upper(L'Ľ'));
        CHECK(is_character::is_consonant(L'ľ'));
        CHECK(is_character::is_consonant(L'Ľ'));
        CHECK_FALSE(is_character::is_vowel(L'ľ'));
        CHECK_FALSE(is_character::is_vowel(L'Ľ'));
        CHECK(is_character::to_lower(L'Ľ') == L'ľ');
        }

    SECTION("Slovak L With Acute")
        {
        // Ĺ/ĺ
        CHECK(is_character::is_lower(L'ĺ'));
        CHECK(is_character::is_upper(L'Ĺ'));
        CHECK(is_character::is_consonant(L'ĺ'));
        CHECK(is_character::is_consonant(L'Ĺ'));
        CHECK_FALSE(is_character::is_vowel(L'ĺ'));
        CHECK_FALSE(is_character::is_vowel(L'Ĺ'));
        CHECK(is_character::to_lower(L'Ĺ') == L'ĺ');
        }

    SECTION("Welsh W With Circumflex")
        {
        // Ŵ/ŵ
        CHECK(is_character::is_lower(L'ŵ'));
        CHECK(is_character::is_upper(L'Ŵ'));
        CHECK(is_character::is_consonant(L'ŵ'));
        CHECK(is_character::is_consonant(L'Ŵ'));
        CHECK_FALSE(is_character::is_vowel(L'ŵ'));
        CHECK_FALSE(is_character::is_vowel(L'Ŵ'));
        CHECK(is_character::to_lower(L'Ŵ') == L'ŵ');
        }

    SECTION("Welsh Y With Circumflex")
        {
        // Ŷ/ŷ
        CHECK(is_character::is_lower(L'ŷ'));
        CHECK(is_character::is_upper(L'Ŷ'));
        CHECK(is_character::is_vowel(L'ŷ'));
        CHECK(is_character::is_vowel(L'Ŷ'));
        CHECK_FALSE(is_character::is_consonant(L'ŷ'));
        CHECK_FALSE(is_character::is_consonant(L'Ŷ'));
        CHECK(is_character::to_lower(L'Ŷ') == L'ŷ');
        }

    SECTION("Maltese C With Dot Above")
        {
        // Ċ/ċ
        CHECK(is_character::is_lower(L'ċ'));
        CHECK(is_character::is_upper(L'Ċ'));
        CHECK(is_character::is_consonant(L'ċ'));
        CHECK(is_character::is_consonant(L'Ċ'));
        CHECK_FALSE(is_character::is_vowel(L'ċ'));
        CHECK_FALSE(is_character::is_vowel(L'Ċ'));
        CHECK(is_character::to_lower(L'Ċ') == L'ċ');
        }

    SECTION("Maltese G With Dot Above")
        {
        // Ġ/ġ
        CHECK(is_character::is_lower(L'ġ'));
        CHECK(is_character::is_upper(L'Ġ'));
        CHECK(is_character::is_consonant(L'ġ'));
        CHECK(is_character::is_consonant(L'Ġ'));
        CHECK_FALSE(is_character::is_vowel(L'ġ'));
        CHECK_FALSE(is_character::is_vowel(L'Ġ'));
        CHECK(is_character::to_lower(L'Ġ') == L'ġ');
        }

    SECTION("Maltese H With Stroke")
        {
        // Ħ/ħ
        CHECK(is_character::is_lower(L'ħ'));
        CHECK(is_character::is_upper(L'Ħ'));
        CHECK(is_character::is_consonant(L'ħ'));
        CHECK(is_character::is_consonant(L'Ħ'));
        CHECK_FALSE(is_character::is_vowel(L'ħ'));
        CHECK_FALSE(is_character::is_vowel(L'Ħ'));
        CHECK(is_character::to_lower(L'Ħ') == L'ħ');
        }

    SECTION("Slovak R With Acute")
        {
        // Ŕ/ŕ
        CHECK(is_character::is_lower(L'ŕ'));
        CHECK(is_character::is_upper(L'Ŕ'));
        CHECK(is_character::is_consonant(L'ŕ'));
        CHECK(is_character::is_consonant(L'Ŕ'));
        CHECK_FALSE(is_character::is_vowel(L'ŕ'));
        CHECK_FALSE(is_character::is_vowel(L'Ŕ'));
        CHECK(is_character::to_lower(L'Ŕ') == L'ŕ');
        }

    SECTION("Croatian D With Stroke")
        {
        // Đ/đ
        CHECK(is_character::is_lower(L'đ'));
        CHECK(is_character::is_upper(L'Đ'));
        CHECK(is_character::is_consonant(L'đ'));
        CHECK(is_character::is_consonant(L'Đ'));
        CHECK_FALSE(is_character::is_vowel(L'đ'));
        CHECK_FALSE(is_character::is_vowel(L'Đ'));
        CHECK(is_character::to_lower(L'Đ') == L'đ');
        }

    SECTION("Welsh W With Grave")
        {
        // Ẁ/ẁ
        CHECK(is_character::is_lower(L'ẁ'));
        CHECK(is_character::is_upper(L'Ẁ'));
        CHECK(is_character::is_consonant(L'ẁ'));
        CHECK(is_character::is_consonant(L'Ẁ'));
        CHECK_FALSE(is_character::is_vowel(L'ẁ'));
        CHECK_FALSE(is_character::is_vowel(L'Ẁ'));
        CHECK(is_character::to_lower(L'Ẁ') == L'ẁ');
        }

    SECTION("Welsh W With Acute")
        {
        // Ẃ/ẃ
        CHECK(is_character::is_lower(L'ẃ'));
        CHECK(is_character::is_upper(L'Ẃ'));
        CHECK(is_character::is_consonant(L'ẃ'));
        CHECK(is_character::is_consonant(L'Ẃ'));
        CHECK_FALSE(is_character::is_vowel(L'ẃ'));
        CHECK_FALSE(is_character::is_vowel(L'Ẃ'));
        CHECK(is_character::to_lower(L'Ẃ') == L'ẃ');
        }

    SECTION("Welsh W With Diaeresis")
        {
        // Ẅ/ẅ
        CHECK(is_character::is_lower(L'ẅ'));
        CHECK(is_character::is_upper(L'Ẅ'));
        CHECK(is_character::is_consonant(L'ẅ'));
        CHECK(is_character::is_consonant(L'Ẅ'));
        CHECK_FALSE(is_character::is_vowel(L'ẅ'));
        CHECK_FALSE(is_character::is_vowel(L'Ẅ'));
        CHECK(is_character::to_lower(L'Ẅ') == L'ẅ');
        }

    SECTION("Welsh Y With Grave")
        {
        // Ỳ/ỳ
        CHECK(is_character::is_lower(L'ỳ'));
        CHECK(is_character::is_upper(L'Ỳ'));
        CHECK(is_character::is_vowel(L'ỳ'));
        CHECK(is_character::is_vowel(L'Ỳ'));
        CHECK_FALSE(is_character::is_consonant(L'ỳ'));
        CHECK_FALSE(is_character::is_consonant(L'Ỳ'));
        CHECK(is_character::to_lower(L'Ỳ') == L'ỳ');
        }
    SECTION("Y With Acute")
        {
        // Ý/ý (Czech, Slovak, Icelandic, Vietnamese)
        CHECK(is_character::is_lower(L'ý'));
        CHECK(is_character::is_upper(L'Ý'));
        CHECK(is_character::is_vowel(L'ý'));
        CHECK(is_character::is_vowel(L'Ý'));
        CHECK_FALSE(is_character::is_consonant(L'ý'));
        CHECK_FALSE(is_character::is_consonant(L'Ý'));
        CHECK(is_character::to_lower(L'Ý') == L'ý');
        }
    SECTION("Y With Diaeresis")
        {
        // Ÿ/ÿ (French)
        CHECK(is_character::is_lower(L'ÿ'));
        CHECK(is_character::is_upper(L'Ÿ'));
        CHECK(is_character::is_vowel(L'ÿ'));
        CHECK(is_character::is_vowel(L'Ÿ'));
        CHECK_FALSE(is_character::is_consonant(L'ÿ'));
        CHECK_FALSE(is_character::is_consonant(L'Ÿ'));
        CHECK(is_character::to_lower(L'Ÿ') == L'ÿ');
        }
    SECTION("Latvian G With Cedilla")
        {
        // Ģ/ģ
        CHECK(is_character::is_lower(L'ģ'));
        CHECK(is_character::is_upper(L'Ģ'));
        CHECK(is_character::is_consonant(L'ģ'));
        CHECK(is_character::is_consonant(L'Ģ'));
        CHECK_FALSE(is_character::is_vowel(L'ģ'));
        CHECK_FALSE(is_character::is_vowel(L'Ģ'));
        CHECK(is_character::to_lower(L'Ģ') == L'ģ');
        }
    SECTION("Latvian K With Cedilla")
        {
        // Ķ/ķ
        CHECK(is_character::is_lower(L'ķ'));
        CHECK(is_character::is_upper(L'Ķ'));
        CHECK(is_character::is_consonant(L'ķ'));
        CHECK(is_character::is_consonant(L'Ķ'));
        CHECK_FALSE(is_character::is_vowel(L'ķ'));
        CHECK_FALSE(is_character::is_vowel(L'Ķ'));
        CHECK(is_character::to_lower(L'Ķ') == L'ķ');
        }
    SECTION("Latvian L With Cedilla")
        {
        // Ļ/ļ
        CHECK(is_character::is_lower(L'ļ'));
        CHECK(is_character::is_upper(L'Ļ'));
        CHECK(is_character::is_consonant(L'ļ'));
        CHECK(is_character::is_consonant(L'Ļ'));
        CHECK_FALSE(is_character::is_vowel(L'ļ'));
        CHECK_FALSE(is_character::is_vowel(L'Ļ'));
        CHECK(is_character::to_lower(L'Ļ') == L'ļ');
        }
    SECTION("Latvian N With Cedilla")
        {
        // Ņ/ņ
        CHECK(is_character::is_lower(L'ņ'));
        CHECK(is_character::is_upper(L'Ņ'));
        CHECK(is_character::is_consonant(L'ņ'));
        CHECK(is_character::is_consonant(L'Ņ'));
        CHECK_FALSE(is_character::is_vowel(L'ņ'));
        CHECK_FALSE(is_character::is_vowel(L'Ņ'));
        CHECK(is_character::to_lower(L'Ņ') == L'ņ');
        }
    SECTION("Lithuanian E With Dot Above")
        {
        // Ė/ė
        CHECK(is_character::is_lower(L'ė'));
        CHECK(is_character::is_upper(L'Ė'));
        CHECK(is_character::is_vowel(L'ė'));
        CHECK(is_character::is_vowel(L'Ė'));
        CHECK_FALSE(is_character::is_consonant(L'ė'));
        CHECK_FALSE(is_character::is_consonant(L'Ė'));
        CHECK(is_character::to_lower(L'Ė') == L'ė');
        }
    SECTION("Esperanto C With Circumflex")
        {
        // Ĉ/ĉ
        CHECK(is_character::is_lower(L'ĉ'));
        CHECK(is_character::is_upper(L'Ĉ'));
        CHECK(is_character::is_consonant(L'ĉ'));
        CHECK(is_character::is_consonant(L'Ĉ'));
        CHECK_FALSE(is_character::is_vowel(L'ĉ'));
        CHECK_FALSE(is_character::is_vowel(L'Ĉ'));
        CHECK(is_character::to_lower(L'Ĉ') == L'ĉ');
        }
    SECTION("Esperanto G With Circumflex")
        {
        // Ĝ/ĝ
        CHECK(is_character::is_lower(L'ĝ'));
        CHECK(is_character::is_upper(L'Ĝ'));
        CHECK(is_character::is_consonant(L'ĝ'));
        CHECK(is_character::is_consonant(L'Ĝ'));
        CHECK_FALSE(is_character::is_vowel(L'ĝ'));
        CHECK_FALSE(is_character::is_vowel(L'Ĝ'));
        CHECK(is_character::to_lower(L'Ĝ') == L'ĝ');
        }
    SECTION("Esperanto H With Circumflex")
        {
        // Ĥ/ĥ
        CHECK(is_character::is_lower(L'ĥ'));
        CHECK(is_character::is_upper(L'Ĥ'));
        CHECK(is_character::is_consonant(L'ĥ'));
        CHECK(is_character::is_consonant(L'Ĥ'));
        CHECK_FALSE(is_character::is_vowel(L'ĥ'));
        CHECK_FALSE(is_character::is_vowel(L'Ĥ'));
        CHECK(is_character::to_lower(L'Ĥ') == L'ĥ');
        }
    SECTION("Esperanto J With Circumflex")
        {
        // Ĵ/ĵ
        CHECK(is_character::is_lower(L'ĵ'));
        CHECK(is_character::is_upper(L'Ĵ'));
        CHECK(is_character::is_consonant(L'ĵ'));
        CHECK(is_character::is_consonant(L'Ĵ'));
        CHECK_FALSE(is_character::is_vowel(L'ĵ'));
        CHECK_FALSE(is_character::is_vowel(L'Ĵ'));
        CHECK(is_character::to_lower(L'Ĵ') == L'ĵ');
        }
    SECTION("Esperanto S With Circumflex")
        {
        // Ŝ/ŝ
        CHECK(is_character::is_lower(L'ŝ'));
        CHECK(is_character::is_upper(L'Ŝ'));
        CHECK(is_character::is_consonant(L'ŝ'));
        CHECK(is_character::is_consonant(L'Ŝ'));
        CHECK_FALSE(is_character::is_vowel(L'ŝ'));
        CHECK_FALSE(is_character::is_vowel(L'Ŝ'));
        CHECK(is_character::to_lower(L'Ŝ') == L'ŝ');
        }
    SECTION("Esperanto U With Breve")
        {
        // Ŭ/ŭ
        CHECK(is_character::is_lower(L'ŭ'));
        CHECK(is_character::is_upper(L'Ŭ'));
        CHECK(is_character::is_vowel(L'ŭ'));
        CHECK(is_character::is_vowel(L'Ŭ'));
        CHECK_FALSE(is_character::is_consonant(L'ŭ'));
        CHECK_FALSE(is_character::is_consonant(L'Ŭ'));
        CHECK(is_character::to_lower(L'Ŭ') == L'ŭ');
        }
    SECTION("Pinyin A With Caron")
        {
        // Ǎ/ǎ
        CHECK(is_character::is_lower(L'ǎ'));
        CHECK(is_character::is_upper(L'Ǎ'));
        CHECK(is_character::is_vowel(L'ǎ'));
        CHECK(is_character::is_vowel(L'Ǎ'));
        CHECK_FALSE(is_character::is_consonant(L'ǎ'));
        CHECK_FALSE(is_character::is_consonant(L'Ǎ'));
        CHECK(is_character::to_lower(L'Ǎ') == L'ǎ');
        }
    SECTION("Pinyin I With Caron")
        {
        // Ǐ/ǐ
        CHECK(is_character::is_lower(L'ǐ'));
        CHECK(is_character::is_upper(L'Ǐ'));
        CHECK(is_character::is_vowel(L'ǐ'));
        CHECK(is_character::is_vowel(L'Ǐ'));
        CHECK_FALSE(is_character::is_consonant(L'ǐ'));
        CHECK_FALSE(is_character::is_consonant(L'Ǐ'));
        CHECK(is_character::to_lower(L'Ǐ') == L'ǐ');
        }
    SECTION("Pinyin O With Caron")
        {
        // Ǒ/ǒ
        CHECK(is_character::is_lower(L'ǒ'));
        CHECK(is_character::is_upper(L'Ǒ'));
        CHECK(is_character::is_vowel(L'ǒ'));
        CHECK(is_character::is_vowel(L'Ǒ'));
        CHECK_FALSE(is_character::is_consonant(L'ǒ'));
        CHECK_FALSE(is_character::is_consonant(L'Ǒ'));
        CHECK(is_character::to_lower(L'Ǒ') == L'ǒ');
        }
    SECTION("Pinyin U With Caron")
        {
        // Ǔ/ǔ
        CHECK(is_character::is_lower(L'ǔ'));
        CHECK(is_character::is_upper(L'Ǔ'));
        CHECK(is_character::is_vowel(L'ǔ'));
        CHECK(is_character::is_vowel(L'Ǔ'));
        CHECK_FALSE(is_character::is_consonant(L'ǔ'));
        CHECK_FALSE(is_character::is_consonant(L'Ǔ'));
        CHECK(is_character::to_lower(L'Ǔ') == L'ǔ');
        }
    SECTION("Sami T With Stroke")
        {
        // Ŧ/ŧ (U+0166/U+0167)
        CHECK(is_character::is_lower(L'ŧ'));
        CHECK(is_character::is_upper(L'Ŧ'));
        CHECK(is_character::is_consonant(L'ŧ'));
        CHECK(is_character::is_consonant(L'Ŧ'));
        CHECK_FALSE(is_character::is_vowel(L'ŧ'));
        CHECK_FALSE(is_character::is_vowel(L'Ŧ'));
        CHECK(is_character::to_lower(L'Ŧ') == L'ŧ');
        }
    SECTION("German Capital Eszett")
        {
        // ẞ/ß (U+1E9E/U+00DF) - capital eszett added to German in 2017
        CHECK(is_character::is_lower(L'ß'));
        CHECK(is_character::is_upper(L'ẞ'));
        CHECK(is_character::is_consonant(L'ß'));
        CHECK(is_character::is_consonant(L'ẞ'));
        CHECK_FALSE(is_character::is_vowel(L'ß'));
        CHECK_FALSE(is_character::is_vowel(L'ẞ'));
        CHECK(is_character::to_lower(L'ẞ') == L'ß');
        }
    SECTION("Sami Eng")
        {
        // Ŋ/ŋ (U+014A/U+014B) - Eng letter used in Sami languages
        CHECK(is_character::is_lower(L'ŋ'));
        CHECK(is_character::is_upper(L'Ŋ'));
        CHECK(is_character::is_consonant(L'ŋ'));
        CHECK(is_character::is_consonant(L'Ŋ'));
        CHECK_FALSE(is_character::is_vowel(L'ŋ'));
        CHECK_FALSE(is_character::is_vowel(L'Ŋ'));
        CHECK(is_character::to_lower(L'Ŋ') == L'ŋ');
        }
    SECTION("Greenlandic Kra")
        {
        // ĸ (U+0138) - Kra, lowercase only (no uppercase in Unicode), archaic
        CHECK(is_character::is_lower(L'ĸ'));
        CHECK_FALSE(is_character::is_upper(L'ĸ'));
        CHECK(is_character::is_consonant(L'ĸ'));
        CHECK_FALSE(is_character::is_vowel(L'ĸ'));
        CHECK(is_character::to_lower(L'ĸ') == L'ĸ');
        }
    SECTION("Old Irish B With Dot Above")
        {
        // Ḃ/ḃ (U+1E02/U+1E03)
        CHECK(is_character::is_lower(L'ḃ'));
        CHECK(is_character::is_upper(L'Ḃ'));
        CHECK(is_character::is_consonant(L'ḃ'));
        CHECK(is_character::is_consonant(L'Ḃ'));
        CHECK_FALSE(is_character::is_vowel(L'ḃ'));
        CHECK_FALSE(is_character::is_vowel(L'Ḃ'));
        CHECK(is_character::to_lower(L'Ḃ') == L'ḃ');
        }
    SECTION("Old Irish D With Dot Above")
        {
        // Ḋ/ḋ (U+1E0A/U+1E0B)
        CHECK(is_character::is_lower(L'ḋ'));
        CHECK(is_character::is_upper(L'Ḋ'));
        CHECK(is_character::is_consonant(L'ḋ'));
        CHECK(is_character::is_consonant(L'Ḋ'));
        CHECK_FALSE(is_character::is_vowel(L'ḋ'));
        CHECK_FALSE(is_character::is_vowel(L'Ḋ'));
        CHECK(is_character::to_lower(L'Ḋ') == L'ḋ');
        }
    SECTION("Old Irish F With Dot Above")
        {
        // Ḟ/ḟ (U+1E1E/U+1E1F)
        CHECK(is_character::is_lower(L'ḟ'));
        CHECK(is_character::is_upper(L'Ḟ'));
        CHECK(is_character::is_consonant(L'ḟ'));
        CHECK(is_character::is_consonant(L'Ḟ'));
        CHECK_FALSE(is_character::is_vowel(L'ḟ'));
        CHECK_FALSE(is_character::is_vowel(L'Ḟ'));
        CHECK(is_character::to_lower(L'Ḟ') == L'ḟ');
        }
    SECTION("Old Irish M With Dot Above")
        {
        // Ṁ/ṁ (U+1E40/U+1E41)
        CHECK(is_character::is_lower(L'ṁ'));
        CHECK(is_character::is_upper(L'Ṁ'));
        CHECK(is_character::is_consonant(L'ṁ'));
        CHECK(is_character::is_consonant(L'Ṁ'));
        CHECK_FALSE(is_character::is_vowel(L'ṁ'));
        CHECK_FALSE(is_character::is_vowel(L'Ṁ'));
        CHECK(is_character::to_lower(L'Ṁ') == L'ṁ');
        }
    SECTION("Old Irish P With Dot Above")
        {
        // Ṗ/ṗ (U+1E56/U+1E57)
        CHECK(is_character::is_lower(L'ṗ'));
        CHECK(is_character::is_upper(L'Ṗ'));
        CHECK(is_character::is_consonant(L'ṗ'));
        CHECK(is_character::is_consonant(L'Ṗ'));
        CHECK_FALSE(is_character::is_vowel(L'ṗ'));
        CHECK_FALSE(is_character::is_vowel(L'Ṗ'));
        CHECK(is_character::to_lower(L'Ṗ') == L'ṗ');
        }
    SECTION("Old Irish S With Dot Above")
        {
        // Ṡ/ṡ (U+1E60/U+1E61)
        CHECK(is_character::is_lower(L'ṡ'));
        CHECK(is_character::is_upper(L'Ṡ'));
        CHECK(is_character::is_consonant(L'ṡ'));
        CHECK(is_character::is_consonant(L'Ṡ'));
        CHECK_FALSE(is_character::is_vowel(L'ṡ'));
        CHECK_FALSE(is_character::is_vowel(L'Ṡ'));
        CHECK(is_character::to_lower(L'Ṡ') == L'ṡ');
        }
    SECTION("Old Irish T With Dot Above")
        {
        // Ṫ/ṫ (U+1E6A/U+1E6B)
        CHECK(is_character::is_lower(L'ṫ'));
        CHECK(is_character::is_upper(L'Ṫ'));
        CHECK(is_character::is_consonant(L'ṫ'));
        CHECK(is_character::is_consonant(L'Ṫ'));
        CHECK_FALSE(is_character::is_vowel(L'ṫ'));
        CHECK_FALSE(is_character::is_vowel(L'Ṫ'));
        CHECK(is_character::to_lower(L'Ṫ') == L'ṫ');
        }

    // =========================================================================
    // EXTENDED CYRILLIC SUPPORT
    // The following tests are for Cyrillic letters used in Ukrainian, Serbian,
    // Belarusian, Macedonian, and historical/archaic Russian orthography.
    // =========================================================================

    SECTION("Ukrainian G With Upturn")
        {
        // Ґ/ґ (U+0490/U+0491)
        CHECK(is_character::is_lower(L'ґ'));
        CHECK(is_character::is_upper(L'Ґ'));
        CHECK(is_character::is_consonant(L'ґ'));
        CHECK(is_character::is_consonant(L'Ґ'));
        CHECK_FALSE(is_character::is_vowel(L'ґ'));
        CHECK_FALSE(is_character::is_vowel(L'Ґ'));
        CHECK(is_character::to_lower(L'Ґ') == L'ґ');
        }
    SECTION("Ukrainian Ye")
        {
        // Є/є (U+0404/U+0454)
        CHECK(is_character::is_lower(L'є'));
        CHECK(is_character::is_upper(L'Є'));
        CHECK(is_character::is_vowel(L'є'));
        CHECK(is_character::is_vowel(L'Є'));
        CHECK_FALSE(is_character::is_consonant(L'є'));
        CHECK_FALSE(is_character::is_consonant(L'Є'));
        CHECK(is_character::to_lower(L'Є') == L'є');
        }
    SECTION("Ukrainian Belarusian I")
        {
        // І/і (U+0406/U+0456) - also used in historical Russian
        CHECK(is_character::is_lower(L'і'));
        CHECK(is_character::is_upper(L'І'));
        CHECK(is_character::is_vowel(L'і'));
        CHECK(is_character::is_vowel(L'І'));
        CHECK_FALSE(is_character::is_consonant(L'і'));
        CHECK_FALSE(is_character::is_consonant(L'І'));
        CHECK(is_character::to_lower(L'І') == L'і');
        }
    SECTION("Ukrainian Yi")
        {
        // Ї/ї (U+0407/U+0457)
        CHECK(is_character::is_lower(L'ї'));
        CHECK(is_character::is_upper(L'Ї'));
        CHECK(is_character::is_vowel(L'ї'));
        CHECK(is_character::is_vowel(L'Ї'));
        CHECK_FALSE(is_character::is_consonant(L'ї'));
        CHECK_FALSE(is_character::is_consonant(L'Ї'));
        CHECK(is_character::to_lower(L'Ї') == L'ї');
        }
    SECTION("Serbian Dje")
        {
        // Ђ/ђ (U+0402/U+0452)
        CHECK(is_character::is_lower(L'ђ'));
        CHECK(is_character::is_upper(L'Ђ'));
        CHECK(is_character::is_consonant(L'ђ'));
        CHECK(is_character::is_consonant(L'Ђ'));
        CHECK_FALSE(is_character::is_vowel(L'ђ'));
        CHECK_FALSE(is_character::is_vowel(L'Ђ'));
        CHECK(is_character::to_lower(L'Ђ') == L'ђ');
        }
    SECTION("Serbian Je")
        {
        // Ј/ј (U+0408/U+0458)
        CHECK(is_character::is_lower(L'ј'));
        CHECK(is_character::is_upper(L'Ј'));
        CHECK(is_character::is_consonant(L'ј'));
        CHECK(is_character::is_consonant(L'Ј'));
        CHECK_FALSE(is_character::is_vowel(L'ј'));
        CHECK_FALSE(is_character::is_vowel(L'Ј'));
        CHECK(is_character::to_lower(L'Ј') == L'ј');
        }
    SECTION("Serbian Lje")
        {
        // Љ/љ (U+0409/U+0459)
        CHECK(is_character::is_lower(L'љ'));
        CHECK(is_character::is_upper(L'Љ'));
        CHECK(is_character::is_consonant(L'љ'));
        CHECK(is_character::is_consonant(L'Љ'));
        CHECK_FALSE(is_character::is_vowel(L'љ'));
        CHECK_FALSE(is_character::is_vowel(L'Љ'));
        CHECK(is_character::to_lower(L'Љ') == L'љ');
        }
    SECTION("Serbian Nje")
        {
        // Њ/њ (U+040A/U+045A)
        CHECK(is_character::is_lower(L'њ'));
        CHECK(is_character::is_upper(L'Њ'));
        CHECK(is_character::is_consonant(L'њ'));
        CHECK(is_character::is_consonant(L'Њ'));
        CHECK_FALSE(is_character::is_vowel(L'њ'));
        CHECK_FALSE(is_character::is_vowel(L'Њ'));
        CHECK(is_character::to_lower(L'Њ') == L'њ');
        }
    SECTION("Serbian Tshe")
        {
        // Ћ/ћ (U+040B/U+045B)
        CHECK(is_character::is_lower(L'ћ'));
        CHECK(is_character::is_upper(L'Ћ'));
        CHECK(is_character::is_consonant(L'ћ'));
        CHECK(is_character::is_consonant(L'Ћ'));
        CHECK_FALSE(is_character::is_vowel(L'ћ'));
        CHECK_FALSE(is_character::is_vowel(L'Ћ'));
        CHECK(is_character::to_lower(L'Ћ') == L'ћ');
        }
    SECTION("Serbian Dzhe")
        {
        // Џ/џ (U+040F/U+045F)
        CHECK(is_character::is_lower(L'џ'));
        CHECK(is_character::is_upper(L'Џ'));
        CHECK(is_character::is_consonant(L'џ'));
        CHECK(is_character::is_consonant(L'Џ'));
        CHECK_FALSE(is_character::is_vowel(L'џ'));
        CHECK_FALSE(is_character::is_vowel(L'Џ'));
        CHECK(is_character::to_lower(L'Џ') == L'џ');
        }
    SECTION("Belarusian Short U")
        {
        // Ў/ў (U+040E/U+045E)
        CHECK(is_character::is_lower(L'ў'));
        CHECK(is_character::is_upper(L'Ў'));
        CHECK(is_character::is_vowel(L'ў'));
        CHECK(is_character::is_vowel(L'Ў'));
        CHECK_FALSE(is_character::is_consonant(L'ў'));
        CHECK_FALSE(is_character::is_consonant(L'Ў'));
        CHECK(is_character::to_lower(L'Ў') == L'ў');
        }
    SECTION("Macedonian Gje")
        {
        // Ѓ/ѓ (U+0403/U+0453)
        CHECK(is_character::is_lower(L'ѓ'));
        CHECK(is_character::is_upper(L'Ѓ'));
        CHECK(is_character::is_consonant(L'ѓ'));
        CHECK(is_character::is_consonant(L'Ѓ'));
        CHECK_FALSE(is_character::is_vowel(L'ѓ'));
        CHECK_FALSE(is_character::is_vowel(L'Ѓ'));
        CHECK(is_character::to_lower(L'Ѓ') == L'ѓ');
        }
    SECTION("Macedonian Kje")
        {
        // Ќ/ќ (U+040C/U+045C)
        CHECK(is_character::is_lower(L'ќ'));
        CHECK(is_character::is_upper(L'Ќ'));
        CHECK(is_character::is_consonant(L'ќ'));
        CHECK(is_character::is_consonant(L'Ќ'));
        CHECK_FALSE(is_character::is_vowel(L'ќ'));
        CHECK_FALSE(is_character::is_vowel(L'Ќ'));
        CHECK(is_character::to_lower(L'Ќ') == L'ќ');
        }
    SECTION("Macedonian Dze")
        {
        // Ѕ/ѕ (U+0405/U+0455)
        CHECK(is_character::is_lower(L'ѕ'));
        CHECK(is_character::is_upper(L'Ѕ'));
        CHECK(is_character::is_consonant(L'ѕ'));
        CHECK(is_character::is_consonant(L'Ѕ'));
        CHECK_FALSE(is_character::is_vowel(L'ѕ'));
        CHECK_FALSE(is_character::is_vowel(L'Ѕ'));
        CHECK(is_character::to_lower(L'Ѕ') == L'ѕ');
        }
    SECTION("Historical Russian Yat")
        {
        // Ѣ/ѣ (U+0462/U+0463) - used in pre-1918 Russian orthography
        CHECK(is_character::is_lower(L'ѣ'));
        CHECK(is_character::is_upper(L'Ѣ'));
        CHECK(is_character::is_vowel(L'ѣ'));
        CHECK(is_character::is_vowel(L'Ѣ'));
        CHECK_FALSE(is_character::is_consonant(L'ѣ'));
        CHECK_FALSE(is_character::is_consonant(L'Ѣ'));
        CHECK(is_character::to_lower(L'Ѣ') == L'ѣ');
        }
    SECTION("Historical Russian Fita")
        {
        // Ѳ/ѳ (U+0472/U+0473) - used in pre-1918 Russian orthography
        CHECK(is_character::is_lower(L'ѳ'));
        CHECK(is_character::is_upper(L'Ѳ'));
        CHECK(is_character::is_consonant(L'ѳ'));
        CHECK(is_character::is_consonant(L'Ѳ'));
        CHECK_FALSE(is_character::is_vowel(L'ѳ'));
        CHECK_FALSE(is_character::is_vowel(L'Ѳ'));
        CHECK(is_character::to_lower(L'Ѳ') == L'ѳ');
        }
    SECTION("Historical Russian Izhitsa")
        {
        // Ѵ/ѵ (U+0474/U+0475) - used in pre-1918 Russian orthography
        CHECK(is_character::is_lower(L'ѵ'));
        CHECK(is_character::is_upper(L'Ѵ'));
        CHECK(is_character::is_vowel(L'ѵ'));
        CHECK(is_character::is_vowel(L'Ѵ'));
        CHECK_FALSE(is_character::is_consonant(L'ѵ'));
        CHECK_FALSE(is_character::is_consonant(L'Ѵ'));
        CHECK(is_character::to_lower(L'Ѵ') == L'ѵ');
        }

    // =========================================================================
    // GREEK ALPHABET SUPPORT
    // Basic Greek: Uppercase U+0391-U+03A9, Lowercase U+03B1-U+03C9
    // Plus final sigma ς (U+03C2), accented vowels, and diaeresis forms.
    // =========================================================================

    SECTION("Greek Basic Alphabet")
        {
        // Uppercase: first (Α), middle (Μ), last (Ω)
        CHECK(is_character::is_upper(L'Α'));
        CHECK_FALSE(is_character::is_lower(L'Α'));
        CHECK(is_character::is_alpha(L'Α'));
        CHECK(is_character::is_upper(L'Μ'));
        CHECK_FALSE(is_character::is_lower(L'Μ'));
        CHECK(is_character::is_alpha(L'Μ'));
        CHECK(is_character::is_upper(L'Ω'));
        CHECK_FALSE(is_character::is_lower(L'Ω'));
        CHECK(is_character::is_alpha(L'Ω'));

        // Lowercase: first (α), middle (μ), last (ω)
        CHECK(is_character::is_lower(L'α'));
        CHECK_FALSE(is_character::is_upper(L'α'));
        CHECK(is_character::is_alpha(L'α'));
        CHECK(is_character::is_lower(L'μ'));
        CHECK_FALSE(is_character::is_upper(L'μ'));
        CHECK(is_character::is_alpha(L'μ'));
        CHECK(is_character::is_lower(L'ω'));
        CHECK_FALSE(is_character::is_upper(L'ω'));
        CHECK(is_character::is_alpha(L'ω'));

        // Final sigma (ς) - lowercase only
        CHECK(is_character::is_lower(L'ς'));
        CHECK_FALSE(is_character::is_upper(L'ς'));
        CHECK(is_character::is_alpha(L'ς'));
        CHECK(is_character::is_consonant(L'ς'));
        }
    SECTION("Greek Vowels")
        {
        // 7 vowels: Α/α, Ε/ε, Η/η, Ι/ι, Ο/ο, Υ/υ, Ω/ω
        CHECK(is_character::is_vowel(L'Α'));
        CHECK(is_character::is_vowel(L'α'));
        CHECK_FALSE(is_character::is_consonant(L'Α'));
        CHECK_FALSE(is_character::is_consonant(L'α'));

        CHECK(is_character::is_vowel(L'Ε'));
        CHECK(is_character::is_vowel(L'ε'));
        CHECK_FALSE(is_character::is_consonant(L'Ε'));
        CHECK_FALSE(is_character::is_consonant(L'ε'));

        CHECK(is_character::is_vowel(L'Η'));
        CHECK(is_character::is_vowel(L'η'));
        CHECK_FALSE(is_character::is_consonant(L'Η'));
        CHECK_FALSE(is_character::is_consonant(L'η'));

        CHECK(is_character::is_vowel(L'Ι'));
        CHECK(is_character::is_vowel(L'ι'));
        CHECK_FALSE(is_character::is_consonant(L'Ι'));
        CHECK_FALSE(is_character::is_consonant(L'ι'));

        CHECK(is_character::is_vowel(L'Ο'));
        CHECK(is_character::is_vowel(L'ο'));
        CHECK_FALSE(is_character::is_consonant(L'Ο'));
        CHECK_FALSE(is_character::is_consonant(L'ο'));

        CHECK(is_character::is_vowel(L'Υ'));
        CHECK(is_character::is_vowel(L'υ'));
        CHECK_FALSE(is_character::is_consonant(L'Υ'));
        CHECK_FALSE(is_character::is_consonant(L'υ'));

        CHECK(is_character::is_vowel(L'Ω'));
        CHECK(is_character::is_vowel(L'ω'));
        CHECK_FALSE(is_character::is_consonant(L'Ω'));
        CHECK_FALSE(is_character::is_consonant(L'ω'));
        }
    SECTION("Greek Consonants")
        {
        // All 17 Greek consonants: Β Γ Δ Ζ Θ Κ Λ Μ Ν Ξ Π Ρ Σ Τ Φ Χ Ψ
        CHECK(is_character::is_consonant(L'Β'));
        CHECK(is_character::is_consonant(L'β'));
        CHECK_FALSE(is_character::is_vowel(L'Β'));
        CHECK_FALSE(is_character::is_vowel(L'β'));

        CHECK(is_character::is_consonant(L'Γ'));
        CHECK(is_character::is_consonant(L'γ'));
        CHECK_FALSE(is_character::is_vowel(L'Γ'));
        CHECK_FALSE(is_character::is_vowel(L'γ'));

        CHECK(is_character::is_consonant(L'Δ'));
        CHECK(is_character::is_consonant(L'δ'));
        CHECK_FALSE(is_character::is_vowel(L'Δ'));
        CHECK_FALSE(is_character::is_vowel(L'δ'));

        CHECK(is_character::is_consonant(L'Ζ'));
        CHECK(is_character::is_consonant(L'ζ'));
        CHECK_FALSE(is_character::is_vowel(L'Ζ'));
        CHECK_FALSE(is_character::is_vowel(L'ζ'));

        CHECK(is_character::is_consonant(L'Θ'));
        CHECK(is_character::is_consonant(L'θ'));
        CHECK_FALSE(is_character::is_vowel(L'Θ'));
        CHECK_FALSE(is_character::is_vowel(L'θ'));

        CHECK(is_character::is_consonant(L'Κ'));
        CHECK(is_character::is_consonant(L'κ'));
        CHECK_FALSE(is_character::is_vowel(L'Κ'));
        CHECK_FALSE(is_character::is_vowel(L'κ'));

        CHECK(is_character::is_consonant(L'Λ'));
        CHECK(is_character::is_consonant(L'λ'));
        CHECK_FALSE(is_character::is_vowel(L'Λ'));
        CHECK_FALSE(is_character::is_vowel(L'λ'));

        CHECK(is_character::is_consonant(L'Μ'));
        CHECK(is_character::is_consonant(L'μ'));
        CHECK_FALSE(is_character::is_vowel(L'Μ'));
        CHECK_FALSE(is_character::is_vowel(L'μ'));

        CHECK(is_character::is_consonant(L'Ν'));
        CHECK(is_character::is_consonant(L'ν'));
        CHECK_FALSE(is_character::is_vowel(L'Ν'));
        CHECK_FALSE(is_character::is_vowel(L'ν'));

        CHECK(is_character::is_consonant(L'Ξ'));
        CHECK(is_character::is_consonant(L'ξ'));
        CHECK_FALSE(is_character::is_vowel(L'Ξ'));
        CHECK_FALSE(is_character::is_vowel(L'ξ'));

        CHECK(is_character::is_consonant(L'Π'));
        CHECK(is_character::is_consonant(L'π'));
        CHECK_FALSE(is_character::is_vowel(L'Π'));
        CHECK_FALSE(is_character::is_vowel(L'π'));

        CHECK(is_character::is_consonant(L'Ρ'));
        CHECK(is_character::is_consonant(L'ρ'));
        CHECK_FALSE(is_character::is_vowel(L'Ρ'));
        CHECK_FALSE(is_character::is_vowel(L'ρ'));

        CHECK(is_character::is_consonant(L'Σ'));
        CHECK(is_character::is_consonant(L'σ'));
        CHECK(is_character::is_consonant(L'ς')); // final sigma
        CHECK_FALSE(is_character::is_vowel(L'Σ'));
        CHECK_FALSE(is_character::is_vowel(L'σ'));
        CHECK_FALSE(is_character::is_vowel(L'ς'));

        CHECK(is_character::is_consonant(L'Τ'));
        CHECK(is_character::is_consonant(L'τ'));
        CHECK_FALSE(is_character::is_vowel(L'Τ'));
        CHECK_FALSE(is_character::is_vowel(L'τ'));

        CHECK(is_character::is_consonant(L'Φ'));
        CHECK(is_character::is_consonant(L'φ'));
        CHECK_FALSE(is_character::is_vowel(L'Φ'));
        CHECK_FALSE(is_character::is_vowel(L'φ'));

        CHECK(is_character::is_consonant(L'Χ'));
        CHECK(is_character::is_consonant(L'χ'));
        CHECK_FALSE(is_character::is_vowel(L'Χ'));
        CHECK_FALSE(is_character::is_vowel(L'χ'));

        CHECK(is_character::is_consonant(L'Ψ'));
        CHECK(is_character::is_consonant(L'ψ'));
        CHECK_FALSE(is_character::is_vowel(L'Ψ'));
        CHECK_FALSE(is_character::is_vowel(L'ψ'));
        }
    SECTION("Greek To Lower")
        {
        CHECK(is_character::to_lower(L'Α') == L'α');
        CHECK(is_character::to_lower(L'Β') == L'β');
        CHECK(is_character::to_lower(L'Μ') == L'μ');
        CHECK(is_character::to_lower(L'Σ') == L'σ');
        CHECK(is_character::to_lower(L'Ω') == L'ω');
        // Lowercase should stay lowercase
        CHECK(is_character::to_lower(L'α') == L'α');
        CHECK(is_character::to_lower(L'ω') == L'ω');
        CHECK(is_character::to_lower(L'ς') == L'ς');
        }
    SECTION("Greek Accented Vowels Tonos")
        {
        // Lowercase with tonos: ά έ ή ί ό ύ ώ
        CHECK(is_character::is_lower(L'ά'));
        CHECK(is_character::is_vowel(L'ά'));
        CHECK(is_character::is_lower(L'έ'));
        CHECK(is_character::is_vowel(L'έ'));
        CHECK(is_character::is_lower(L'ή'));
        CHECK(is_character::is_vowel(L'ή'));
        CHECK(is_character::is_lower(L'ί'));
        CHECK(is_character::is_vowel(L'ί'));
        CHECK(is_character::is_lower(L'ό'));
        CHECK(is_character::is_vowel(L'ό'));
        CHECK(is_character::is_lower(L'ύ'));
        CHECK(is_character::is_vowel(L'ύ'));
        CHECK(is_character::is_lower(L'ώ'));
        CHECK(is_character::is_vowel(L'ώ'));

        // Uppercase with tonos: Ά Έ Ή Ί Ό Ύ Ώ
        CHECK(is_character::is_upper(L'Ά'));
        CHECK(is_character::is_vowel(L'Ά'));
        CHECK(is_character::is_upper(L'Έ'));
        CHECK(is_character::is_vowel(L'Έ'));
        CHECK(is_character::is_upper(L'Ή'));
        CHECK(is_character::is_vowel(L'Ή'));
        CHECK(is_character::is_upper(L'Ί'));
        CHECK(is_character::is_vowel(L'Ί'));
        CHECK(is_character::is_upper(L'Ό'));
        CHECK(is_character::is_vowel(L'Ό'));
        CHECK(is_character::is_upper(L'Ύ'));
        CHECK(is_character::is_vowel(L'Ύ'));
        CHECK(is_character::is_upper(L'Ώ'));
        CHECK(is_character::is_vowel(L'Ώ'));

        // Case conversion for accented
        CHECK(is_character::to_lower(L'Ά') == L'ά');
        CHECK(is_character::to_lower(L'Έ') == L'έ');
        CHECK(is_character::to_lower(L'Ή') == L'ή');
        CHECK(is_character::to_lower(L'Ί') == L'ί');
        CHECK(is_character::to_lower(L'Ό') == L'ό');
        CHECK(is_character::to_lower(L'Ύ') == L'ύ');
        CHECK(is_character::to_lower(L'Ώ') == L'ώ');
        }
    SECTION("Greek Dialytika")
        {
        // Iota/upsilon with diaeresis: ϊ ϋ Ϊ Ϋ
        CHECK(is_character::is_lower(L'ϊ'));
        CHECK(is_character::is_vowel(L'ϊ'));
        CHECK(is_character::is_lower(L'ϋ'));
        CHECK(is_character::is_vowel(L'ϋ'));
        CHECK(is_character::is_upper(L'Ϊ'));
        CHECK(is_character::is_vowel(L'Ϊ'));
        CHECK(is_character::is_upper(L'Ϋ'));
        CHECK(is_character::is_vowel(L'Ϋ'));

        CHECK(is_character::to_lower(L'Ϊ') == L'ϊ');
        CHECK(is_character::to_lower(L'Ϋ') == L'ϋ');

        // Combined dialytika + tonos: ΐ ΰ (lowercase only)
        CHECK(is_character::is_lower(L'ΐ'));
        CHECK(is_character::is_vowel(L'ΐ'));
        CHECK(is_character::is_lower(L'ΰ'));
        CHECK(is_character::is_vowel(L'ΰ'));
        }
    SECTION("Georgian Alphabet")
        {
        // Georgian Mkhedruli is unicameral (no case distinction)
        // All letters should be treated as lowercase for is_alpha to work
        // Range: U+10D0 to U+10F0

        // Test that Georgian letters are recognized as alphabetic
        CHECK(is_character::is_alpha(L'ა')); // U+10D0
        CHECK(is_character::is_alpha(L'ბ')); // U+10D1
        CHECK(is_character::is_alpha(L'გ')); // U+10D2
        CHECK(is_character::is_alpha(L'ჰ')); // U+10F0 (last letter)

        // Georgian is unicameral - treat as lowercase
        CHECK(is_character::is_lower(L'ა'));
        CHECK(is_character::is_lower(L'ბ'));
        CHECK(is_character::is_lower(L'გ'));
        CHECK(is_character::is_lower(L'ჰ'));

        // No uppercase in Georgian
        CHECK_FALSE(is_character::is_upper(L'ა'));
        CHECK_FALSE(is_character::is_upper(L'ბ'));
        CHECK_FALSE(is_character::is_upper(L'ჰ'));

        // to_lower should return the same character
        CHECK(is_character::to_lower(L'ა') == L'ა');
        CHECK(is_character::to_lower(L'ბ') == L'ბ');
        CHECK(is_character::to_lower(L'ჰ') == L'ჰ');
        }
    SECTION("Georgian Vowels")
        {
        // Georgian vowels: ა ე ი ო უ (5 vowels)
        CHECK(is_character::is_vowel(L'ა')); // a - U+10D0
        CHECK_FALSE(is_character::is_consonant(L'ა'));

        CHECK(is_character::is_vowel(L'ე')); // e - U+10D4
        CHECK_FALSE(is_character::is_consonant(L'ე'));

        CHECK(is_character::is_vowel(L'ი')); // i - U+10D8
        CHECK_FALSE(is_character::is_consonant(L'ი'));

        CHECK(is_character::is_vowel(L'ო')); // o - U+10DD
        CHECK_FALSE(is_character::is_consonant(L'ო'));

        CHECK(is_character::is_vowel(L'უ')); // u - U+10E3
        CHECK_FALSE(is_character::is_consonant(L'უ'));
        }
    SECTION("Georgian Consonants")
        {
        // Georgian consonants (28 total)
        // ბ გ დ ვ ზ თ კ ლ მ ნ პ ჟ რ ს ტ ფ ქ ღ ყ შ ჩ ც ძ წ ჭ ხ ჯ ჰ

        CHECK(is_character::is_consonant(L'ბ')); // b - U+10D1
        CHECK_FALSE(is_character::is_vowel(L'ბ'));

        CHECK(is_character::is_consonant(L'გ')); // g - U+10D2
        CHECK_FALSE(is_character::is_vowel(L'გ'));

        CHECK(is_character::is_consonant(L'დ')); // d - U+10D3
        CHECK_FALSE(is_character::is_vowel(L'დ'));

        CHECK(is_character::is_consonant(L'ვ')); // v - U+10D5
        CHECK_FALSE(is_character::is_vowel(L'ვ'));

        CHECK(is_character::is_consonant(L'ზ')); // z - U+10D6
        CHECK_FALSE(is_character::is_vowel(L'ზ'));

        CHECK(is_character::is_consonant(L'თ')); // t - U+10D7
        CHECK_FALSE(is_character::is_vowel(L'თ'));

        CHECK(is_character::is_consonant(L'კ')); // k - U+10D9
        CHECK_FALSE(is_character::is_vowel(L'კ'));

        CHECK(is_character::is_consonant(L'ლ')); // l - U+10DA
        CHECK_FALSE(is_character::is_vowel(L'ლ'));

        CHECK(is_character::is_consonant(L'მ')); // m - U+10DB
        CHECK_FALSE(is_character::is_vowel(L'მ'));

        CHECK(is_character::is_consonant(L'ნ')); // n - U+10DC
        CHECK_FALSE(is_character::is_vowel(L'ნ'));

        CHECK(is_character::is_consonant(L'პ')); // p - U+10DE
        CHECK_FALSE(is_character::is_vowel(L'პ'));

        CHECK(is_character::is_consonant(L'ჟ')); // zh - U+10DF
        CHECK_FALSE(is_character::is_vowel(L'ჟ'));

        CHECK(is_character::is_consonant(L'რ')); // r - U+10E0
        CHECK_FALSE(is_character::is_vowel(L'რ'));

        CHECK(is_character::is_consonant(L'ს')); // s - U+10E1
        CHECK_FALSE(is_character::is_vowel(L'ს'));

        CHECK(is_character::is_consonant(L'ტ')); // t' - U+10E2
        CHECK_FALSE(is_character::is_vowel(L'ტ'));

        CHECK(is_character::is_consonant(L'ფ')); // p' - U+10E4
        CHECK_FALSE(is_character::is_vowel(L'ფ'));

        CHECK(is_character::is_consonant(L'ქ')); // k' - U+10E5
        CHECK_FALSE(is_character::is_vowel(L'ქ'));

        CHECK(is_character::is_consonant(L'ღ')); // gh - U+10E6
        CHECK_FALSE(is_character::is_vowel(L'ღ'));

        CHECK(is_character::is_consonant(L'ყ')); // q - U+10E7
        CHECK_FALSE(is_character::is_vowel(L'ყ'));

        CHECK(is_character::is_consonant(L'შ')); // sh - U+10E8
        CHECK_FALSE(is_character::is_vowel(L'შ'));

        CHECK(is_character::is_consonant(L'ჩ')); // ch - U+10E9
        CHECK_FALSE(is_character::is_vowel(L'ჩ'));

        CHECK(is_character::is_consonant(L'ც')); // ts - U+10EA
        CHECK_FALSE(is_character::is_vowel(L'ც'));

        CHECK(is_character::is_consonant(L'ძ')); // dz - U+10EB
        CHECK_FALSE(is_character::is_vowel(L'ძ'));

        CHECK(is_character::is_consonant(L'წ')); // ts' - U+10EC
        CHECK_FALSE(is_character::is_vowel(L'წ'));

        CHECK(is_character::is_consonant(L'ჭ')); // ch' - U+10ED
        CHECK_FALSE(is_character::is_vowel(L'ჭ'));

        CHECK(is_character::is_consonant(L'ხ')); // kh - U+10EE
        CHECK_FALSE(is_character::is_vowel(L'ხ'));

        CHECK(is_character::is_consonant(L'ჯ')); // j - U+10EF
        CHECK_FALSE(is_character::is_vowel(L'ჯ'));

        CHECK(is_character::is_consonant(L'ჰ')); // h - U+10F0
        CHECK_FALSE(is_character::is_vowel(L'ჰ'));
        }

    // =========================================================================
    // GREEK LETTER DETECTION (is_greek_letter)
    // Tests for detecting any Greek letter (uppercase or lowercase)
    // Used to avoid flagging Greek letters as improper lowercase sentence starts
    // =========================================================================

    SECTION("Greek Letter Detection Basic Uppercase")
        {
        // All 24 Greek uppercase letters: Α-Ρ (U+0391-U+03A1), Σ-Ω (U+03A3-U+03A9)
        CHECK(is_character::is_greek_letter(L'Α')); // Alpha
        CHECK(is_character::is_greek_letter(L'Β')); // Beta
        CHECK(is_character::is_greek_letter(L'Γ')); // Gamma
        CHECK(is_character::is_greek_letter(L'Δ')); // Delta
        CHECK(is_character::is_greek_letter(L'Ε')); // Epsilon
        CHECK(is_character::is_greek_letter(L'Ζ')); // Zeta
        CHECK(is_character::is_greek_letter(L'Η')); // Eta
        CHECK(is_character::is_greek_letter(L'Θ')); // Theta
        CHECK(is_character::is_greek_letter(L'Ι')); // Iota
        CHECK(is_character::is_greek_letter(L'Κ')); // Kappa
        CHECK(is_character::is_greek_letter(L'Λ')); // Lambda
        CHECK(is_character::is_greek_letter(L'Μ')); // Mu
        CHECK(is_character::is_greek_letter(L'Ν')); // Nu
        CHECK(is_character::is_greek_letter(L'Ξ')); // Xi
        CHECK(is_character::is_greek_letter(L'Ο')); // Omicron
        CHECK(is_character::is_greek_letter(L'Π')); // Pi
        CHECK(is_character::is_greek_letter(L'Ρ')); // Rho
        CHECK(is_character::is_greek_letter(L'Σ')); // Sigma
        CHECK(is_character::is_greek_letter(L'Τ')); // Tau
        CHECK(is_character::is_greek_letter(L'Υ')); // Upsilon
        CHECK(is_character::is_greek_letter(L'Φ')); // Phi
        CHECK(is_character::is_greek_letter(L'Χ')); // Chi
        CHECK(is_character::is_greek_letter(L'Ψ')); // Psi
        CHECK(is_character::is_greek_letter(L'Ω')); // Omega
        }
    SECTION("Greek Letter Detection Basic Lowercase")
        {
        // All 24 Greek lowercase letters: α-ρ (U+03B1-U+03C1), ς, σ-ω (U+03C3-U+03C9)
        CHECK(is_character::is_greek_letter(L'α')); // alpha
        CHECK(is_character::is_greek_letter(L'β')); // beta
        CHECK(is_character::is_greek_letter(L'γ')); // gamma
        CHECK(is_character::is_greek_letter(L'δ')); // delta
        CHECK(is_character::is_greek_letter(L'ε')); // epsilon
        CHECK(is_character::is_greek_letter(L'ζ')); // zeta
        CHECK(is_character::is_greek_letter(L'η')); // eta
        CHECK(is_character::is_greek_letter(L'θ')); // theta
        CHECK(is_character::is_greek_letter(L'ι')); // iota
        CHECK(is_character::is_greek_letter(L'κ')); // kappa
        CHECK(is_character::is_greek_letter(L'λ')); // lambda
        CHECK(is_character::is_greek_letter(L'μ')); // mu
        CHECK(is_character::is_greek_letter(L'ν')); // nu
        CHECK(is_character::is_greek_letter(L'ξ')); // xi
        CHECK(is_character::is_greek_letter(L'ο')); // omicron
        CHECK(is_character::is_greek_letter(L'π')); // pi
        CHECK(is_character::is_greek_letter(L'ρ')); // rho
        CHECK(is_character::is_greek_letter(L'ς')); // final sigma
        CHECK(is_character::is_greek_letter(L'σ')); // sigma
        CHECK(is_character::is_greek_letter(L'τ')); // tau
        CHECK(is_character::is_greek_letter(L'υ')); // upsilon
        CHECK(is_character::is_greek_letter(L'φ')); // phi
        CHECK(is_character::is_greek_letter(L'χ')); // chi
        CHECK(is_character::is_greek_letter(L'ψ')); // psi
        CHECK(is_character::is_greek_letter(L'ω')); // omega
        }
    SECTION("Greek Letter Detection With Tonos")
        {
        // Uppercase with tonos: Ά Έ Ή Ί Ό Ύ Ώ
        CHECK(is_character::is_greek_letter(L'Ά')); // Alpha with tonos
        CHECK(is_character::is_greek_letter(L'Έ')); // Epsilon with tonos
        CHECK(is_character::is_greek_letter(L'Ή')); // Eta with tonos
        CHECK(is_character::is_greek_letter(L'Ί')); // Iota with tonos
        CHECK(is_character::is_greek_letter(L'Ό')); // Omicron with tonos
        CHECK(is_character::is_greek_letter(L'Ύ')); // Upsilon with tonos
        CHECK(is_character::is_greek_letter(L'Ώ')); // Omega with tonos

        // Lowercase with tonos: ά έ ή ί ό ύ ώ
        CHECK(is_character::is_greek_letter(L'ά')); // alpha with tonos
        CHECK(is_character::is_greek_letter(L'έ')); // epsilon with tonos
        CHECK(is_character::is_greek_letter(L'ή')); // eta with tonos
        CHECK(is_character::is_greek_letter(L'ί')); // iota with tonos
        CHECK(is_character::is_greek_letter(L'ό')); // omicron with tonos
        CHECK(is_character::is_greek_letter(L'ύ')); // upsilon with tonos
        CHECK(is_character::is_greek_letter(L'ώ')); // omega with tonos
        }
    SECTION("Greek Letter Detection With Dialytika")
        {
        // Uppercase with dialytika: Ϊ Ϋ
        CHECK(is_character::is_greek_letter(L'Ϊ')); // Iota with dialytika
        CHECK(is_character::is_greek_letter(L'Ϋ')); // Upsilon with dialytika

        // Lowercase with dialytika: ϊ ϋ
        CHECK(is_character::is_greek_letter(L'ϊ')); // iota with dialytika
        CHECK(is_character::is_greek_letter(L'ϋ')); // upsilon with dialytika

        // Lowercase with dialytika and tonos: ΐ ΰ
        CHECK(is_character::is_greek_letter(L'ΐ')); // iota with dialytika and tonos
        CHECK(is_character::is_greek_letter(L'ΰ')); // upsilon with dialytika and tonos
        }
    SECTION("Greek Letter Detection Non-Greek Characters")
        {
        // Latin letters should NOT be detected as Greek
        CHECK_FALSE(is_character::is_greek_letter(L'A'));
        CHECK_FALSE(is_character::is_greek_letter(L'a'));
        CHECK_FALSE(is_character::is_greek_letter(L'Z'));
        CHECK_FALSE(is_character::is_greek_letter(L'z'));

        // Cyrillic letters should NOT be detected as Greek
        // (some look similar to Greek but are different code points)
        CHECK_FALSE(is_character::is_greek_letter(L'А')); // Cyrillic A (U+0410)
        CHECK_FALSE(is_character::is_greek_letter(L'а')); // Cyrillic a (U+0430)
        CHECK_FALSE(is_character::is_greek_letter(L'Я')); // Cyrillic Ya
        CHECK_FALSE(is_character::is_greek_letter(L'я')); // Cyrillic ya

        // Numbers should NOT be detected as Greek
        CHECK_FALSE(is_character::is_greek_letter(L'0'));
        CHECK_FALSE(is_character::is_greek_letter(L'9'));

        // Punctuation should NOT be detected as Greek
        CHECK_FALSE(is_character::is_greek_letter(L'.'));
        CHECK_FALSE(is_character::is_greek_letter(L','));
        CHECK_FALSE(is_character::is_greek_letter(L'!'));
        CHECK_FALSE(is_character::is_greek_letter(L' '));

        // Georgian letters should NOT be detected as Greek
        CHECK_FALSE(is_character::is_greek_letter(L'ა')); // Georgian a
        CHECK_FALSE(is_character::is_greek_letter(L'ბ')); // Georgian b
        }

    // =========================================================================
    // WESTERN EUROPEAN LETTER DETECTION (is_western_european_letter)
    // Tests for detecting Latin-based letters - used by spell checker to
    // identify words outside the scope of English spell checking
    // =========================================================================

    SECTION("Western European Letter Detection Basic Latin")
        {
        // Basic Latin letters should be detected as Western European
        CHECK(is_character::is_western_european_letter(L'a'));
        CHECK(is_character::is_western_european_letter(L'z'));
        CHECK(is_character::is_western_european_letter(L'A'));
        CHECK(is_character::is_western_european_letter(L'Z'));
        CHECK(is_character::is_western_european_letter(L'm'));
        CHECK(is_character::is_western_european_letter(L'M'));
        }
    SECTION("Western European Letter Detection Extended Latin")
        {
        // French
        CHECK(is_character::is_western_european_letter(L'é'));
        CHECK(is_character::is_western_european_letter(L'è'));
        CHECK(is_character::is_western_european_letter(L'ê'));
        CHECK(is_character::is_western_european_letter(L'ë'));
        CHECK(is_character::is_western_european_letter(L'à'));
        CHECK(is_character::is_western_european_letter(L'ç'));
        CHECK(is_character::is_western_european_letter(L'œ'));

        // Spanish
        CHECK(is_character::is_western_european_letter(L'ñ'));
        CHECK(is_character::is_western_european_letter(L'Ñ'));

        // German
        CHECK(is_character::is_western_european_letter(L'ü'));
        CHECK(is_character::is_western_european_letter(L'ö'));
        CHECK(is_character::is_western_european_letter(L'ä'));
        CHECK(is_character::is_western_european_letter(L'ß'));
        CHECK(is_character::is_western_european_letter(L'ẞ')); // capital eszett

        // Scandinavian
        CHECK(is_character::is_western_european_letter(L'ø'));
        CHECK(is_character::is_western_european_letter(L'å'));
        CHECK(is_character::is_western_european_letter(L'æ'));
        CHECK(is_character::is_western_european_letter(L'Ø'));
        CHECK(is_character::is_western_european_letter(L'Å'));
        CHECK(is_character::is_western_european_letter(L'Æ'));
        }
    SECTION("Western European Letter Detection Non-Western")
        {
        // Greek should NOT be detected as Western European
        CHECK_FALSE(is_character::is_western_european_letter(L'α')); // alpha
        CHECK_FALSE(is_character::is_western_european_letter(L'β')); // beta
        CHECK_FALSE(is_character::is_western_european_letter(L'ω')); // omega
        CHECK_FALSE(is_character::is_western_european_letter(L'Α')); // Alpha
        CHECK_FALSE(is_character::is_western_european_letter(L'Ω')); // Omega
        CHECK_FALSE(is_character::is_western_european_letter(L'ά')); // alpha with tonos

        // Cyrillic should NOT be detected as Western European
        CHECK_FALSE(is_character::is_western_european_letter(L'а')); // Cyrillic a
        CHECK_FALSE(is_character::is_western_european_letter(L'я')); // Cyrillic ya
        CHECK_FALSE(is_character::is_western_european_letter(L'А')); // Cyrillic A
        CHECK_FALSE(is_character::is_western_european_letter(L'Я')); // Cyrillic Ya
        CHECK_FALSE(is_character::is_western_european_letter(L'ї')); // Ukrainian yi
        CHECK_FALSE(is_character::is_western_european_letter(L'ђ')); // Serbian dje

        // Georgian should NOT be detected as Western European
        CHECK_FALSE(is_character::is_western_european_letter(L'ა')); // Georgian a
        CHECK_FALSE(is_character::is_western_european_letter(L'ბ')); // Georgian b
        }
    SECTION("Western European Letter Detection Non-Letters")
        {
        // Numbers should NOT be detected as Western European letters
        CHECK_FALSE(is_character::is_western_european_letter(L'0'));
        CHECK_FALSE(is_character::is_western_european_letter(L'9'));

        // Punctuation should NOT be detected
        CHECK_FALSE(is_character::is_western_european_letter(L'.'));
        CHECK_FALSE(is_character::is_western_european_letter(L' '));
        CHECK_FALSE(is_character::is_western_european_letter(L'-'));
        CHECK_FALSE(is_character::is_western_european_letter(L'!'));
        }
    }
// NOLINTEND
// clang-format on
