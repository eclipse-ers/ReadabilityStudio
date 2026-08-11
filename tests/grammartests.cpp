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

#include "../src/indexing/abbreviation.h"
#include "../src/indexing/conjunction.h"
#include "../src/indexing/german_syllabize.h"
#include "../src/indexing/negating_word.h"
#include "../src/indexing/passive_voice.h"
#include "../src/indexing/phrase.h"
#include "../src/indexing/plain_language_phrase.h"
#include "../src/indexing/pronoun.h"
#include "../src/indexing/spanish_syllabize.h"
#include "../src/indexing/tokenize.h"
#include "../src/indexing/word_collection.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

// clang-format off
// NOLINTBEGIN

using namespace grammar;

using MYWORD = word<traits::case_insensitive_ex,
    stemming::english_stem<std::basic_string<wchar_t, traits::case_insensitive_ex> > >;

extern word_list Stop_list;
extern grammar::english_syllabize ENsyllabizer;
extern grammar::spanish_syllabize ESsyllabizer;
extern grammar::german_syllabize DEsyllabizer;
extern stemming::english_stem<std::wstring> ENStemmer;
extern grammar::is_english_coordinating_conjunction is_conjunction;
//extern grammar::is_incorrect_english_article is_english_mismatched_article;
extern grammar::phrase_collection pmap;
extern grammar::phrase_collection copyrightPMap;
extern grammar::phrase_collection citationPMap;
std::shared_ptr<grammar::phrase_collection> excludedPMap{
    std::make_shared<grammar::phrase_collection>()
};
std::shared_ptr<grammar::plain_language_phrase_collection> plainLanguagePMap{
    std::make_shared<grammar::plain_language_phrase_collection>()
};
extern word_list Known_proper_nouns;
extern word_list Known_personal_nouns;
extern word_list Known_spellings;
extern word_list Secondary_known_spellings;
extern word_list Programming_known_spellings;

TEST_CASE("Punctuation", "[punctuation]")
    {
    SECTION("Punctuation Functor")
        {
        punctuation::punctuation_count counter;

        CHECK(counter(L"&") == 0);
        CHECK(counter(L"Hello") == 0);
        CHECK(counter(L"it's") == 1);
        CHECK(counter(L"$5.00") == 2);
        CHECK(counter(L"200%") == 1);
        CHECK(counter(L"trash-man") == 1);
        CHECK(counter(L"#7") == 1);
        CHECK(counter(L"blah@email.com") == 2);
        CHECK(counter(L"±7") == 1);
        CHECK(counter(L"company©") == 1);
        CHECK(counter(L"Français") == 0);
        CHECK(counter(L"abcdefghijklmnopqrstuvwxyz") == 0);
        CHECK(counter(L"ABCDEFGHIJKLMNOPQRSTUVWXYZ") == 0);
        CHECK(counter(L"0123456789") == 0);
        CHECK(counter(L"Hi!?.,+-\\/\"`~&") == 12);
        // should not count spaces
        CHECK(counter(L"    012") == 0);
        CHECK(counter(L"àáâãäååæçèéêëìíîïðñòóôõöøùúûüýß") == 0);
        CHECK(counter(L"ÀÁÂÃÄÅÅÆÇÈÉÊËÌÍÎÏÐÑÒÓÔÕÖØÙÚÛÜÝß") == 0);
        // find the quid, euro, and degree symbols
        CHECK(counter(L"ÀÁÂÃÄÅ€ÅÆÇÈÉÊË£ÌÍÎÏÐÑÒÓÔ°ÕÖØÙÚÛÜÝß") == 3);
        }
    }

TEST_CASE("Bullets", "[bullets]")
    {
    SECTION("Null")
        {
        grammar::is_bulleted_text is_indented;
        CHECK_FALSE(is_indented(L"").first);
        CHECK(is_indented(L"").second == 0);
        }
    SECTION("Tab")
        {
        const wchar_t text[] = L"\titem 1";
        grammar::is_bulleted_text is_indented;
        CHECK_FALSE(is_indented(text).first);
        CHECK(is_indented(text).second == 0);
        }
    SECTION("One Space")
        {
        const wchar_t text[] = L" item 1";
        grammar::is_bulleted_text is_indented;
        CHECK_FALSE(is_indented(text).first);
        CHECK(is_indented(text).second == 0);
        }
    SECTION("Two Space")
        {
        const wchar_t text[] = L"  item 1";
        grammar::is_bulleted_text is_indented;
        CHECK_FALSE(is_indented(text).first);
        CHECK(is_indented(text).second == 0);
        }
    SECTION("Spaces")
        {
        const wchar_t text[] = L"   item 1";
        grammar::is_bulleted_text is_indented;
        CHECK_FALSE(is_indented(text).first);
        CHECK(is_indented(text).second == 0);
        }
    SECTION("All Spaces")
        {
        const wchar_t text[] = L"     ";
        grammar::is_bulleted_text is_indented;
        CHECK_FALSE(is_indented(text).first);
        CHECK(is_indented(text).second == 0);
        }
    SECTION("Bullet")
        {
        const wchar_t* text = L"· item 1";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first);
        CHECK(is_indented(text).second == 1);
        text = L"• item 1";
        CHECK(is_indented(text).first);
        CHECK(is_indented(text).second == 1);
        }
    SECTION("Bullet With Indenting")
        {
        const wchar_t* text = L"   · item 1";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first);
        CHECK(is_indented(text).second == 1);
        text = L"\t• item 1";
        CHECK(is_indented(text).first);
        CHECK(is_indented(text).second == 1);
        }
    SECTION("Number Bullet Followed By Colon")
        {
        const wchar_t text[] = L"12: item 1";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first);
        CHECK(is_indented(text).second == 3);
        CHECK_FALSE(is_indented(L"12:").first);//boundary check
        }
    SECTION("Number Bullet Time")
        {
        const wchar_t text[] = L"12:55 item 1";
        grammar::is_bulleted_text is_indented;
        CHECK_FALSE(is_indented(text).first);
        CHECK(is_indented(text).second == 0);
        }
    SECTION("Number Bullet")
        {
        const wchar_t text[] = L"12. item 1";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first);
        CHECK(is_indented(text).second == 3);
        }
    SECTION("Number Bullet With Indenting")
        {
        const wchar_t text[] = L"  \t12. item 1";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first);
        CHECK(is_indented(text).second == 3);
        }
    SECTION("Number Bullet 2")
        {
        const wchar_t text[] = L"1, item 1";
        grammar::is_bulleted_text is_indented;
        CHECK_FALSE(is_indented(text).first);
        CHECK(is_indented(text).second == 0);
        }
    SECTION("Number Bullet 3")
        {
        const wchar_t text[] = L"1";
        grammar::is_bulleted_text is_indented;
        CHECK_FALSE(is_indented(text).first);
        CHECK(is_indented(text).second == 0);
        }
    SECTION("Number List")
        {
        const wchar_t text[] = L" 1 tablespoon\n 2 cups\n  1 1/2 cups\n\n";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first);
        CHECK(is_indented(text).second == 1);
        CHECK(is_indented(text+14).first);
        CHECK(is_indented(text+14).second == 1);
        CHECK(is_indented(text+22).first);
        CHECK(is_indented(text+22).second == 1);
        }
    SECTION("Number List Last Item Not List Item")
        {
        const wchar_t text[] = L"1 tablespoon\n 2 cups or\n1 1/2 cups\r\nSome text";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first);
        CHECK(is_indented(text).second == 1);
        CHECK(is_indented(text+14).first);
        CHECK(is_indented(text+14).second == 1);
        CHECK_FALSE(is_indented(text+24).first);//\r\n will be seen as only one line
        CHECK(is_indented(text+24).second == 0);
        }
    SECTION("Number Number Parenthesis")
        {
        const wchar_t text[] = L"3.) item 1";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first);
        CHECK(is_indented(text).second == 3);
        }
    SECTION("Number Number Parenthesis 2")
        {
        const wchar_t text[] = L"3). item 1";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first);
        CHECK(is_indented(text).second == 3);
        }
    SECTION("Letter")
        {
        const wchar_t text[] = L"a. item 1";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first);
        CHECK(is_indented(text).second == 2);
        }
    SECTION("Letter Parenthesis")
        {
        const wchar_t text[] = L"a.) item 1";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first);
        CHECK(is_indented(text).second == 3);
        }
    SECTION("Letter Not Bullet")
        {
        const wchar_t text[] = L"B. Madden.";
        grammar::is_bulleted_text is_indented;
        CHECK_FALSE(is_indented(text).first);
        CHECK(is_indented(text).second == 0);
        }
    SECTION("Plain Text")
        {
        const wchar_t text[] = L"This is some text";
        grammar::is_bulleted_text is_indented;
        CHECK_FALSE(is_indented(text).first);
        CHECK(is_indented(text).second == 0);
        }
    SECTION("Dash")
        {
        const wchar_t text[] = L"- This is some text";
        grammar::is_bulleted_text is_indented;
        CHECK(is_indented(text).first);
        CHECK(is_indented(text).second == 1);
        }
    }

TEST_CASE("End of line", "[end-of-line]")
    {
    SECTION("NULLs")
        {
        grammar::is_end_of_line isEol;
        const wchar_t* text = L"\n\n\n";
        isEol(nullptr, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 0);
        CHECK(isEol.get_eol_count() == 0);
        isEol(text, nullptr);
        CHECK(isEol.get_characters_skipped_count() == 0);
        CHECK(isEol.get_eol_count() == 0);
        }
    SECTION("Single Char")
        {
        grammar::is_end_of_line isEol;
        CHECK(isEol(L'\n'));
        CHECK(isEol(L'\r'));
        CHECK(isEol(L'\f'));//formfeed
        CHECK_FALSE(isEol(L' '));
        CHECK_FALSE(isEol(L'n'));
        CHECK_FALSE(isEol(L'\t'));
        CHECK_FALSE(isEol(L'r'));
        }
    SECTION("Just New Lines")
        {
        grammar::is_end_of_line isEol;
        const wchar_t* text = L"\n\n\n";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 3);
        CHECK(isEol.get_eol_count() == 3);
        }
    SECTION("One New Line")
        {
        grammar::is_end_of_line isEol;
        const wchar_t* text = L"\nsome text";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 1);
        CHECK(isEol.get_eol_count() == 1);
        }
    SECTION("One Carriage Return")
        {
        grammar::is_end_of_line isEol;
        const wchar_t* text = L"\rsome text";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 1);
        CHECK(isEol.get_eol_count() == 1);
        }
    SECTION("One CRLF Pair")
        {
        grammar::is_end_of_line isEol;
        const wchar_t* text = L"\n\rsome text";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 2);
        CHECK(isEol.get_eol_count() == 1);
        }
    SECTION("Two New Lines")
        {
        grammar::is_end_of_line isEol;
        const wchar_t* text = L"\n\nsome text";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 2);
        CHECK(isEol.get_eol_count() == 2);
        }
    SECTION("Two New Lines With Spaces")
        {
        grammar::is_end_of_line isEol;
        const wchar_t* text = L"\n \nsome text";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 3);
        CHECK(isEol.get_eol_count() == 2);
        }
    SECTION("Four New Lines With Spaces")
        {
        grammar::is_end_of_line isEol;
        const wchar_t* text = L"\n \n\t \t\n  \t\nsome text";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 11);
        CHECK(isEol.get_eol_count() == 4);
        }
    SECTION("Form feeds")
        {
        grammar::is_end_of_line isEol;
        const wchar_t* text = L"\n \n\r\f\n\r\t \t\n  \t\nsome text";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 15);
        CHECK(isEol.get_eol_count() == 6);
        }
    SECTION("One New Lines With Spaces")
        {
        grammar::is_end_of_line isEol;
        const wchar_t* text = L"\n  some text";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 1);
        CHECK(isEol.get_eol_count() == 1);
        }
    SECTION("One New Lines With Spaces No Following Text")
        {
        grammar::is_end_of_line isEol;
        const wchar_t* text = L"\n  ";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 1);
        CHECK(isEol.get_eol_count() == 1);
        }
    SECTION("Two New Lines With Spaces No Following Text")
        {
        grammar::is_end_of_line isEol;
        const wchar_t* text = L"\n  \n";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 4);
        CHECK(isEol.get_eol_count() == 2);
        }
    SECTION("Reset")
        {
        grammar::is_end_of_line isEol;
        const wchar_t* text = L"\n\nsome text";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 2);
        CHECK(isEol.get_eol_count() == 2);
        text = L"\n \n\t \t\n  \t\nsome text";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 11);
        CHECK(isEol.get_eol_count() == 4);
        text = L"\n  ";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 1);
        CHECK(isEol.get_eol_count() == 1);
        text = L"  ";
        isEol(text, text+std::wcslen(text));
        CHECK(isEol.get_characters_skipped_count() == 0);
        CHECK(isEol.get_eol_count() == 0);
        }
    }

TEST_CASE("Passive Voice", "[passive-voice]")
    {
    std::vector<std::wstring> m_strings;

    SECTION("Ignore Proper Nouns")
        {
        grammar::is_english_passive_voice pasV;
        size_t wordCount;

        m_strings.clear();
        m_strings.push_back(L"am");
        m_strings.push_back(L"Holden");//proper name that looks like a verb
        CHECK_FALSE(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 0);

        m_strings.clear();
        m_strings.push_back(L"am");
        m_strings.push_back(L"holden");//verb
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"AM");
        m_strings.push_back(L"BOLDEN");//exclamatory? Assume this is a verb then
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);
        }
    SECTION("ED Exceptions")
        {
        grammar::is_english_passive_voice pasV;
        size_t wordCount;
        m_strings.clear();
        m_strings.push_back(L"am");
        m_strings.push_back(L"excited");
        CHECK_FALSE(pasV(m_strings.begin(), m_strings.size(), wordCount));

        m_strings.push_back(L"from");
        CHECK_FALSE(pasV(m_strings.begin(), m_strings.size(), wordCount));

        m_strings[2] = L"by";
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"am");
        m_strings.push_back(L"not");
        m_strings.push_back(L"excited");
        m_strings.push_back(L"by");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 4);

        m_strings[3] = L"bye";
        CHECK_FALSE(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 0);

        m_strings.clear();
        m_strings.push_back(L"am");
        m_strings.push_back(L"not");
        m_strings.push_back(L"excited");
        CHECK_FALSE(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 0);
        }
    SECTION("Simple")
        {
        grammar::is_english_passive_voice pasV;
        size_t wordCount;

        m_strings.clear();
        m_strings.push_back(L"be");
        m_strings.push_back(L"noted");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"am");
        m_strings.push_back(L"given");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"was");
        m_strings.push_back(L"given");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"wasn’t");
        m_strings.push_back(L"given");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"are");
        m_strings.push_back(L"portrayed");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"aren’t");
        m_strings.push_back(L"portrayed");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"been");
        m_strings.push_back(L"saved");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"being");
        m_strings.push_back(L"saved");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"is");
        m_strings.push_back(L"paved");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"isn't");
        m_strings.push_back(L"paved");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"were");
        m_strings.push_back(L"taken");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"weren't");
        m_strings.push_back(L"taken");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"AM");
        m_strings.push_back(L"GIVEN");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"WAS");
        m_strings.push_back(L"GIVEN");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"WASN’T");
        m_strings.push_back(L"GIVEN");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"ARE");
        m_strings.push_back(L"PORTRAYED");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"AREN’T");
        m_strings.push_back(L"PORTRAYED");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"BEEN");
        m_strings.push_back(L"SAVED");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"BEING");
        m_strings.push_back(L"SAVED");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"IS");
        m_strings.push_back(L"PAVED");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"ISN'T");
        m_strings.push_back(L"PAVED");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"WERE");
        m_strings.push_back(L"TAKEN");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);

        m_strings.clear();
        m_strings.push_back(L"WEREN'T");
        m_strings.push_back(L"TAKEN");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 2);
        }
    SECTION("Negative Simple")
        {
        grammar::is_english_passive_voice pasV;
        size_t wordCount;

        m_strings.clear();
        m_strings.push_back(L"am");
        m_strings.push_back(L"not");
        m_strings.push_back(L"given");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"was");
        m_strings.push_back(L"not");
        m_strings.push_back(L"given");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"are");
        m_strings.push_back(L"not");
        m_strings.push_back(L"portrayed");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"been");
        m_strings.push_back(L"not");
        m_strings.push_back(L"saved");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"being");
        m_strings.push_back(L"not");
        m_strings.push_back(L"saved");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"is");
        m_strings.push_back(L"not");
        m_strings.push_back(L"paved");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"were");
        m_strings.push_back(L"not");
        m_strings.push_back(L"taken");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"AM");
        m_strings.push_back(L"NOT");
        m_strings.push_back(L"GIVEN");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"WAS");
        m_strings.push_back(L"NOT");
        m_strings.push_back(L"GIVEN");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"ARE");
        m_strings.push_back(L"NOT");
        m_strings.push_back(L"PORTRAYED");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"BEEN");
        m_strings.push_back(L"NOT");
        m_strings.push_back(L"SAVED");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"BEING");
        m_strings.push_back(L"NOT");
        m_strings.push_back(L"SAVED");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"IS");
        m_strings.push_back(L"NOT");
        m_strings.push_back(L"PAVED");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);

        m_strings.clear();
        m_strings.push_back(L"WERE");
        m_strings.push_back(L"NOT");
        m_strings.push_back(L"TAKEN");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 3);
        }
    SECTION("Not To Be Verbs")
        {
        grammar::is_english_passive_voice pasV;
        size_t wordCount;

        m_strings.clear();
        m_strings.push_back(L"sam");
        m_strings.push_back(L"given");
        CHECK_FALSE(pasV(m_strings.begin(), m_strings.size(), wordCount));

        m_strings.clear();
        m_strings.push_back(L"waste");
        m_strings.push_back(L"given");
        CHECK_FALSE(pasV(m_strings.begin(), m_strings.size(), wordCount));

        m_strings.clear();
        m_strings.push_back(L"art");
        m_strings.push_back(L"portrayed");
        CHECK_FALSE(pasV(m_strings.begin(), m_strings.size(), wordCount));

        m_strings.clear();
        m_strings.push_back(L"beat");
        m_strings.push_back(L"saved");
        CHECK_FALSE(pasV(m_strings.begin(), m_strings.size(), wordCount));

        m_strings.clear();
        m_strings.push_back(L"beats");
        m_strings.push_back(L"saved");
        CHECK_FALSE(pasV(m_strings.begin(), m_strings.size(), wordCount));

        m_strings.clear();
        m_strings.push_back(L"ist");
        m_strings.push_back(L"paved");
        CHECK_FALSE(pasV(m_strings.begin(), m_strings.size(), wordCount));

        m_strings.clear();
        m_strings.push_back(L"worm");
        m_strings.push_back(L"taken");
        CHECK_FALSE(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 0);
        }
    SECTION("Not Past Participles")
        {
        grammar::is_english_passive_voice pasV;
        size_t wordCount;

        m_strings.clear();
        m_strings.push_back(L"am");
        m_strings.push_back(L"gists");
        CHECK_FALSE(pasV(m_strings.begin(), m_strings.size(), wordCount));

        m_strings.clear();
        m_strings.push_back(L"was");
        m_strings.push_back(L"gists");
        CHECK_FALSE(pasV(m_strings.begin(), m_strings.size(), wordCount));

        m_strings.clear();
        m_strings.push_back(L"are");
        m_strings.push_back(L"portrayer");
        CHECK_FALSE(pasV(m_strings.begin(), m_strings.size(), wordCount));

        m_strings.clear();
        m_strings.push_back(L"been");
        m_strings.push_back(L"saver");
        CHECK_FALSE(pasV(m_strings.begin(), m_strings.size(), wordCount));

        m_strings.clear();
        m_strings.push_back(L"being");
        m_strings.push_back(L"saver");
        CHECK_FALSE(pasV(m_strings.begin(), m_strings.size(), wordCount));

        m_strings.clear();
        m_strings.push_back(L"is");
        m_strings.push_back(L"pavor");
        CHECK_FALSE(pasV(m_strings.begin(), m_strings.size(), wordCount));

        m_strings.clear();
        m_strings.push_back(L"were");
        m_strings.push_back(L"taker");
        CHECK_FALSE(pasV(m_strings.begin(), m_strings.size(), wordCount));

        m_strings.clear();
        m_strings.push_back(L"were");
        m_strings.push_back(L"eighteen");
        CHECK_FALSE(pasV(m_strings.begin(), m_strings.size(), wordCount));

        m_strings.clear();
        m_strings.push_back(L"were");
        m_strings.push_back(L"seven");
        m_strings.push_back(L"by");
        CHECK_FALSE(pasV(m_strings.begin(), m_strings.size(), wordCount));

        m_strings.clear();
        m_strings.push_back(L"were");
        m_strings.push_back(L"sponsored");
        m_strings.push_back(L"by");
        CHECK(pasV(m_strings.begin(), m_strings.size(), wordCount));

        m_strings.clear();
        m_strings.push_back(L"were");
        m_strings.push_back(L"Heaven");
        CHECK_FALSE(pasV(m_strings.begin(), m_strings.size(), wordCount));

        m_strings.clear();
        m_strings.push_back(L"were");
        m_strings.push_back(L"alien");
        CHECK_FALSE(pasV(m_strings.begin(), m_strings.size(), wordCount));

        m_strings.clear();
        m_strings.push_back(L"were");
        m_strings.push_back(L"haven");
        CHECK_FALSE(pasV(m_strings.begin(), m_strings.size(), wordCount));

        m_strings.clear();
        m_strings.push_back(L"were");
        m_strings.push_back(L"infrared");
        CHECK_FALSE(pasV(m_strings.begin(), m_strings.size(), wordCount));

        m_strings.clear();
        m_strings.push_back(L"were");
        m_strings.push_back(L"sacred");
        CHECK_FALSE(pasV(m_strings.begin(), m_strings.size(), wordCount));

        m_strings.clear();
        m_strings.push_back(L"were");
        m_strings.push_back(L"not");
        m_strings.push_back(L"eighteen");
        CHECK_FALSE(pasV(m_strings.begin(), m_strings.size(), wordCount));
        CHECK(wordCount == 0);
        }
    }

TEST_CASE("Copy member if", "[copy-member]")
    {
    SECTION("Copy Member")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter 1\n\nIt was a dark and stormy night.\n\nThe End";
        doc.load_document(text, wcslen(text), false, false, false, false);

        std::vector<size_t> sentence_sizes(doc.get_sentences().size() );
        copy_member(doc.get_sentences().begin(),
            doc.get_sentences().end(),
            sentence_sizes.begin(),
            [](const grammar::sentence_info& s) { return s.get_word_count(); } );

        CHECK(sentence_sizes.size() == 3);
        CHECK(sentence_sizes[0] == 2);
        CHECK(sentence_sizes[1] == 7);
        CHECK(sentence_sizes[2] == 2);
        }
    SECTION("Copy Member If")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter 1\n\nIt was a dark and stormy night.\n\nThe End";
        doc.load_document(text, wcslen(text), false, false, false, false);

        std::vector<size_t> sentence_sizes(doc.get_sentences().size() );
        std::vector<size_t>::iterator pos =
            copy_member_if(doc.get_sentences().begin(),
                doc.get_sentences().end(),
                sentence_sizes.begin(),
                [](const grammar::sentence_info& s) { return s.is_valid(); },
                [](const grammar::sentence_info& s) { return s.get_valid_word_count(); } );
        sentence_sizes.erase(pos, sentence_sizes.end());

        CHECK(sentence_sizes.size() == 1);
        CHECK(sentence_sizes[0] == 7);
        }
    }

TEST_CASE("Split word", "[split-word]")
    {
    SECTION("Split Words")
        {
        Known_spellings.load_words(L"darkly night pumpkin head", true, false);
        document<word_case_insensitive_no_stem> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Dark Night\n\nBy Blake pumpkin-\n\rhead\n\nIt was a night. It was a dark-\nly nig-\rht.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_word_count() == 14);
        CHECK(doc.get_words().at(4) == L"pumpkin-head");
        CHECK(doc.get_words().at(12) == L"darkly");
        CHECK(doc.get_words().at(13) == L"night");
        }
    SECTION("Split Words By Spaces")
        {
        Known_spellings.load_words(L"darkly night pumpkin head", true, false);
        document<word_case_insensitive_no_stem> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Dark Night\n\nBy Blake pumpkin- head\n\nIt was a night. It was a dark-  ly nig-\rht.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_word_count() == 14);
        CHECK(doc.get_words().at(4) == L"pumpkin-head");
        CHECK(doc.get_words().at(12) == L"darkly");
        CHECK(doc.get_words().at(13) == L"night");
        }
    SECTION("Split Words With Dashes")
        {
        Known_spellings.load_words(L"darkly night", true, false);
        document<word_case_insensitive_no_stem> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Dark Night\n\nBy Blake--\n Madden\n\nIt was a night. It was a dark-\nly nig-\rht.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_word_count() == 14);
        CHECK(doc.get_words().at(12) == L"darkly");
        CHECK(doc.get_words().at(13) == L"night");
        }
    SECTION("No Split But Has Hyphens")
        {
        document<word_case_insensitive_no_stem> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Dark-night\n\nBy Blake\n Madden\n\nIt was a night--it was a dark night.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_word_count() == 13);
        CHECK(doc.get_words().at(7) == L"night");
        CHECK(doc.get_words().at(8) == L"it");
        }
    }

TEST_CASE("Phrases", "[phrase]")
    {
    SECTION("Null")
        {
        phrase_collection phrases;
        phrases.load_phrases(nullptr, false, false);
        CHECK(phrases.get_phrases().size() == 0);
        }
    SECTION("Like Phrases")
        {
        phrase_collection phrases;
        phrases.load_phrases(L"Street Fighter\nStreet Fighter 2\nStreet Fighter 3\nStreet Fighter 4\nStreet Fighter Four", false, false);
    
        std::vector<std::basic_string<wchar_t, traits::case_insensitive_ex>> positive;
        positive.push_back(L"Street");
        positive.push_back(L"Fighter");
        CHECK(0 == phrases(positive.begin(), 0, positive.size(), true));

        positive.push_back(L"4");
        CHECK(3 == phrases(positive.begin(), 0, positive.size(), true));

        positive[2] = L"Four";
        CHECK(4 == phrases(positive.begin(), 0, positive.size(), true));

        // should match "Street Fighter", then short circuit on "Street Fighter 2" and stop comparing against the other phrases
        positive[2] = L"1";
        CHECK(0 == phrases(positive.begin(), 0, positive.size(), true));

        // will go through the whole list, but stay on the first "Street Fighter"
        positive[2] = L"X";
        CHECK(0 == phrases(positive.begin(), 0, positive.size(), true));
        }
    SECTION("Empty")
        {
        phrase_collection phrases;
        phrases.load_phrases(L"", false, false);
        CHECK(phrases.get_phrases().size() == 0);
        }
    SECTION("Search")
        {
        std::vector<std::basic_string<wchar_t, traits::case_insensitive_ex>> positive;
        positive.push_back(L"copyright");
        positive.push_back(L"notice");

        phrase_collection phrases;
        phrases.load_phrases(L"all rights reserved\ntrademarks\ncopyright notice", true, false);
        CHECK(phrases(positive.begin(), 0, positive.size(), true) == 1);
        }
    SECTION("Search Similar Matches")
        {
        std::vector<std::basic_string<wchar_t, traits::case_insensitive_ex>> positive;
        positive.push_back(L"all");
        positive.push_back(L"rights");
        positive.push_back(L"reserved");

        phrase_collection phrases;
        phrases.load_phrases(L"all rights\ncopyright\nall rights reserved", true, false);
        CHECK(phrases(positive.begin(), 0, positive.size(), true) == 1);
        }
    SECTION("Search Single Word")
        {
        std::vector<std::basic_string<wchar_t, traits::case_insensitive_ex>> positive;
        positive.push_back(L"trademarks");

        phrase_collection phrases;
        phrases.load_phrases(L"all rights reserved\ntrademarks\ncopyright notice", true, false);
        CHECK(phrases(positive.begin(), 0, positive.size(), true) == 2);
        }
    SECTION("Loading Phrases")
        {
        phrase_collection phrases;
        phrases.load_phrases(L"all rights reserved\ntrademarks\ncopyright notice", false, false);
        CHECK(phrases.get_phrases().at(0).first.get_word_count() == 3);
        CHECK(phrases.get_phrases().at(0).first.to_string() == L"all rights reserved");
        //test the defaults
        CHECK(phrases.get_phrases().at(0).second == L"");
        CHECK(phrases.get_phrases().at(0).first.get_type() == grammar::phrase_type::phrase_wordy);

        CHECK(phrases.get_phrases().at(1).first.get_word_count() == 1);
        CHECK(phrases.get_phrases().at(1).first.to_string() == L"trademarks");
        CHECK(phrases.get_phrases().at(1).second == L"");
        CHECK(phrases.get_phrases().at(1).first.get_type() == grammar::phrase_type::phrase_wordy);

        CHECK(phrases.get_phrases().at(2).first.get_word_count() == 2);
        CHECK(phrases.get_phrases().at(2).first.to_string() == L"copyright notice");
        CHECK(phrases.get_phrases().at(2).second == L"");
        CHECK(phrases.get_phrases().at(2).first.get_type() == grammar::phrase_type::phrase_wordy);
        CHECK(phrases.get_phrases().size() == 3);
        CHECK_FALSE(phrases.is_sorted());
        }
    SECTION("Loading Phrases Preserve")
        {
        phrase_collection phrases;
        phrases.load_phrases(L"all rights reserved", false, false);
        phrases.load_phrases(L"trademarks\ncopyright notice", false, true);
        CHECK(phrases.get_phrases().at(0).first.get_word_count() == 3);
        CHECK(phrases.get_phrases().at(0).first.to_string() == L"all rights reserved");
        // test the defaults
        CHECK(phrases.get_phrases().at(0).second == L"");
        CHECK(phrases.get_phrases().at(0).first.get_type() == grammar::phrase_type::phrase_wordy);

        CHECK(phrases.get_phrases().at(1).first.get_word_count() == 1);
        CHECK(phrases.get_phrases().at(1).first.to_string() == L"trademarks");
        CHECK(phrases.get_phrases().at(1).second == L"");
        CHECK(phrases.get_phrases().at(1).first.get_type() == grammar::phrase_type::phrase_wordy);

        CHECK(phrases.get_phrases().at(2).first.get_word_count() == 2);
        CHECK(phrases.get_phrases().at(2).first.to_string() == L"copyright notice");
        CHECK(phrases.get_phrases().at(2).second == L"");
        CHECK(phrases.get_phrases().at(2).first.get_type() == grammar::phrase_type::phrase_wordy);
        CHECK(phrases.get_phrases().size() == 3);
        CHECK_FALSE(phrases.is_sorted());
        }
    SECTION("Loading Phrases One With Extra Columns")
        {
        phrase_collection phrases;
        phrases.load_phrases(L"all rights reserved\tARR\t1\ntrademarks\ncopyright notice", false, false);
        CHECK(phrases.get_phrases().at(0).first.get_word_count() == 3);
        CHECK(phrases.get_phrases().at(0).first.to_string() == L"all rights reserved");
        CHECK(phrases.get_phrases().at(0).second == L"ARR");
        CHECK(phrases.get_phrases().at(0).first.get_type() == grammar::phrase_type::phrase_redundant);

        CHECK(phrases.get_phrases().at(1).first.get_word_count() == 1);
        CHECK(phrases.get_phrases().at(1).first.to_string() == L"trademarks");
        // test the defaults
        CHECK(phrases.get_phrases().at(1).second == L"");
        CHECK(phrases.get_phrases().at(1).first.get_type() == grammar::phrase_type::phrase_wordy);

        CHECK(phrases.get_phrases().at(2).first.get_word_count() == 2);
        CHECK(phrases.get_phrases().at(2).first.to_string() == L"copyright notice");
        CHECK(phrases.get_phrases().at(2).second == L"");
        CHECK(phrases.get_phrases().at(2).first.get_type() == grammar::phrase_type::phrase_wordy);
        CHECK(phrases.get_phrases().size() == 3);
        }
    SECTION("Loading Phrases With Suggestions")
        {
        phrase_collection phrases;
        phrases.load_phrases(L"all rights reserved\tARR\ntrademarks\ncopyright notice\tNotice", false, false);
        CHECK(phrases.get_phrases().at(0).first.get_word_count() == 3);
        CHECK(phrases.get_phrases().at(0).first.to_string() == L"all rights reserved");
        CHECK(phrases.get_phrases().at(0).second == L"ARR");
        CHECK(phrases.get_phrases().at(1).first.get_word_count() == 1);
        CHECK(phrases.get_phrases().at(1).first.to_string() == L"trademarks");
        CHECK(phrases.get_phrases().at(1).second == L"");
        CHECK(phrases.get_phrases().at(2).first.get_word_count() == 2);
        CHECK(phrases.get_phrases().at(2).first.to_string() == L"copyright notice");
        CHECK(phrases.get_phrases().at(2).second == L"Notice");
        CHECK(phrases.get_phrases().size() == 3);
        }
    SECTION("Loading Phrases With Types")
        {
        phrase_collection phrases;
        phrases.load_phrases(L"all rights reserved\tARR\t1\ntrademarks\tT\t2\ncopyright notice\tNotice\t3", false, false);
        CHECK(phrases.get_phrases().at(0).first.get_word_count() == 3);
        CHECK(phrases.get_phrases().at(0).first.to_string() == L"all rights reserved");
        CHECK(phrases.get_phrases().at(0).second == L"ARR");
        CHECK(phrases.get_phrases().at(0).first.get_type() == grammar::phrase_type::phrase_redundant);
        CHECK(phrases.get_phrases().at(1).first.get_word_count() == 1);
        CHECK(phrases.get_phrases().at(1).first.to_string() == L"trademarks");
        CHECK(phrases.get_phrases().at(1).second == L"T");
        CHECK(phrases.get_phrases().at(1).first.get_type() == grammar::phrase_type::phrase_cliche);
        CHECK(phrases.get_phrases().at(2).first.get_word_count() == 2);
        CHECK(phrases.get_phrases().at(2).first.to_string() == L"copyright notice");
        CHECK(phrases.get_phrases().at(2).second == L"Notice");
        CHECK(phrases.get_phrases().at(2).first.get_type() == grammar::phrase_type::phrase_error);
        CHECK(phrases.get_phrases().size() == 3);
        }
    SECTION("Loading Phrases With Proceeding Exceptions")
        {
        phrase_collection phrases;
        phrases.load_phrases(L"all rights reserved\tARR\t1\tPexcept\ntrademarks\tT\t2\ncopyright notice\tNotice\t3\tpExt", false, false);
        CHECK(phrases.get_phrases().at(0).first.get_word_count() == 3);
        CHECK(phrases.get_phrases().at(0).first.to_string() == L"all rights reserved");
        CHECK(phrases.get_phrases().at(0).second == L"ARR");
        CHECK(phrases.get_phrases().at(0).first.get_type() == grammar::phrase_type::phrase_redundant);
        CHECK(*phrases.get_phrases().at(0).first.get_preceding_exceptions().cbegin() == L"Pexcept");
        CHECK(phrases.get_phrases().at(1).first.get_word_count() == 1);
        CHECK(phrases.get_phrases().at(1).first.to_string() == L"trademarks");
        CHECK(phrases.get_phrases().at(1).second == L"T");
        CHECK(phrases.get_phrases().at(1).first.get_type() == grammar::phrase_type::phrase_cliche);
        CHECK(phrases.get_phrases().at(2).first.get_word_count() == 2);
        CHECK(phrases.get_phrases().at(2).first.to_string() == L"copyright notice");
        CHECK(phrases.get_phrases().at(2).second == L"Notice");
        CHECK(phrases.get_phrases().at(2).first.get_type() == grammar::phrase_type::phrase_error);
        CHECK(*phrases.get_phrases().at(2).first.get_preceding_exceptions().cbegin() == L"pExt");
        CHECK(phrases.get_phrases().size() == 3);
        }
    SECTION("Loading Phrases With Trialing Exceptions")
        {
        phrase_collection phrases;
        phrases.load_phrases(L"all rights reserved\tARR\t1\t\tTexcept\ntrademarks\tT\t2\ncopyright notice\tNotice\t3\t\tTExt", false, false);
        CHECK(phrases.get_phrases().at(0).first.get_word_count() == 3);
        CHECK(phrases.get_phrases().at(0).first.to_string() == L"all rights reserved");
        CHECK(phrases.get_phrases().at(0).second == L"ARR");
        CHECK(phrases.get_phrases().at(0).first.get_type() == grammar::phrase_type::phrase_redundant);
        CHECK(*phrases.get_phrases().at(0).first.get_trailing_exceptions().cbegin() == L"Texcept");
        CHECK(phrases.get_phrases().at(1).first.get_word_count() == 1);
        CHECK(phrases.get_phrases().at(1).first.to_string() == L"trademarks");
        CHECK(phrases.get_phrases().at(1).second == L"T");
        CHECK(phrases.get_phrases().at(1).first.get_type() == grammar::phrase_type::phrase_cliche);
        CHECK(phrases.get_phrases().at(2).first.get_word_count() == 2);
        CHECK(phrases.get_phrases().at(2).first.to_string() == L"copyright notice");
        CHECK(phrases.get_phrases().at(2).second == L"Notice");
        CHECK(phrases.get_phrases().at(2).first.get_type() == grammar::phrase_type::phrase_error);
        CHECK(*phrases.get_phrases().at(2).first.get_trailing_exceptions().cbegin() == L"TExt");
        CHECK(phrases.get_phrases().size() == 3);
        }
    SECTION("Loading Phrases Duplicates")
        {
        phrase_collection phrases;
        phrases.load_phrases(L"all rights reserved\tARR\t1\ntrademarks\tT\t2\nall rights reserved\tarr\t3", false, false);
        CHECK(phrases.get_phrases().at(0).first.get_word_count() == 3);
        CHECK(phrases.get_phrases().at(0).first.to_string() == L"all rights reserved");
        CHECK(phrases.get_phrases().at(1).first.get_word_count() == 1);
        CHECK(phrases.get_phrases().at(1).first.to_string() == L"trademarks");
        CHECK(phrases.get_phrases().at(2).first.get_word_count() == 3);
        CHECK(phrases.get_phrases().at(2).first.to_string() == L"all rights reserved");
        CHECK(phrases.get_phrases().size() == 3);

        phrases.remove_duplicates();
        CHECK(phrases.get_phrases().size() == 2);
        CHECK(phrases.is_sorted());
        }
    SECTION("Loading Phrases Sort")
        {
        phrase_collection phrases;
        phrases.load_phrases(L"all rights reserved\ntrademarks\ncopyright notice", true, false);
        CHECK(phrases.get_phrases().at(0).first.get_word_count() == 3);
        CHECK(phrases.get_phrases().at(0).first.to_string() == L"all rights reserved");
        CHECK(phrases.get_phrases().at(1).first.get_word_count() == 2);
        CHECK(phrases.get_phrases().at(1).first.to_string() == L"copyright notice");
        CHECK(phrases.get_phrases().at(2).first.get_word_count() == 1);
        CHECK(phrases.get_phrases().at(2).first.to_string() == L"trademarks");
        CHECK(phrases.get_phrases().size() == 3);
        CHECK(phrases.is_sorted());
        }
    }

TEST_CASE("Phrase comparison", "[phrass]")
    {
    SECTION("Equal To Words Results")
        {
        grammar::phrase<> ph;
        ph.add_word(L"Street"); ph.add_word(L"Fighter"); ph.add_word(L"Alpha");

        std::vector<std::basic_string<wchar_t, traits::case_insensitive_ex>> cmpWords;
        cmpWords.push_back(L"Street");
        cmpWords.push_back(L"Fighter");

        CHECK_FALSE(ph.equal_to_words(cmpWords.begin(), 0, cmpWords.size()).first);
        CHECK(ph.equal_to_words(cmpWords.begin(), 0, cmpWords.size()).second ==
            grammar::phrase_comparison_result::phrase_longer_than);

        cmpWords.push_back(L"A");
        CHECK_FALSE(ph.equal_to_words(cmpWords.begin(), 0, cmpWords.size()).first);
        CHECK(ph.equal_to_words(cmpWords.begin(), 0, cmpWords.size()).second ==
            grammar::phrase_comparison_result::phrase_greater_than);

        cmpWords[2] = L"Two";
        CHECK_FALSE(ph.equal_to_words(cmpWords.begin(), 0, cmpWords.size()).first);
        CHECK(ph.equal_to_words(cmpWords.begin(), 0, cmpWords.size()).second ==
            grammar::phrase_comparison_result::phrase_less_than);

        cmpWords[2] = L"Alpha";
        CHECK(ph.equal_to_words(cmpWords.begin(), 0, cmpWords.size()).first);
        CHECK(ph.equal_to_words(cmpWords.begin(), 0, cmpWords.size()).second ==
            grammar::phrase_comparison_result::phrase_equal);

        ph.clear_words();
        ph.set_trailing_exceptions(std::set<std::basic_string<wchar_t, traits::case_insensitive_ex>>{L"Alpha"});
        ph.add_word(L"Street"); ph.add_word(L"Fighter");
        CHECK_FALSE(ph.equal_to_words(cmpWords.begin(), 0, cmpWords.size()).first);
        CHECK(ph.equal_to_words(cmpWords.begin(), 0, cmpWords.size()).second ==
            grammar::phrase_comparison_result::phrase_rule_exception);
        }
    SECTION("Equal To Words")
        {
        grammar::phrase<> ph;
        ph.add_word(L"I");
        ph.add_word(L"had");
        ph.add_word(L"had");
        ph.set_preceding_exceptions(std::set<std::basic_string<wchar_t, traits::case_insensitive_ex>>{L"WRONG"});
        ph.set_trailing_exceptions(std::set<std::basic_string<wchar_t, traits::case_insensitive_ex>>{L"it"});

        std::vector<std::basic_string<wchar_t, traits::case_insensitive_ex>> positive;
        positive.push_back(L"I");
        positive.push_back(L"had");
        positive.push_back(L"had");
        positive.push_back(L"gone");

        std::vector<std::basic_string<wchar_t, traits::case_insensitive_ex>> falsePositive;
        falsePositive.push_back(L"I");
        falsePositive.push_back(L"had");
        falsePositive.push_back(L"had");
        falsePositive.push_back(L"it");

        std::vector<std::basic_string<wchar_t, traits::case_insensitive_ex>> falsePositive2;
        falsePositive2.push_back(L"wrong");
        falsePositive2.push_back(L"I");
        falsePositive2.push_back(L"had");
        falsePositive2.push_back(L"had");
        falsePositive2.push_back(L"gone");

        CHECK(ph.equal_to_words(positive.begin(), 0, positive.size()).first);
        CHECK_FALSE(ph.equal_to_words(falsePositive.begin(), 0, falsePositive.size()).first);
        CHECK_FALSE(ph.equal_to_words(falsePositive2.begin()+1, 1, falsePositive2.size()-1).first);
        }
    SECTION("To String")
        {
        grammar::phrase<> ph;
        ph.add_word(L"time");
        ph.add_word(L"of");
        ph.add_word(L"your");
        ph.add_word(L"life");

        CHECK(ph.to_string() == L"time of your life");
        }
    SECTION("Less Than Or Equal To")
        {
        grammar::phrase<> ph;
        ph.add_word(L"time");
        ph.add_word(L"of");
        ph.add_word(L"your");
        ph.add_word(L"life");

        grammar::phrase<> ph2;
        // partial copy
        ph2.copy_words(ph.get_words(), 2);
        CHECK((ph2 < ph));
        CHECK_FALSE(ph < ph2);
        CHECK_FALSE(ph == ph2);
        CHECK_FALSE(ph2 == ph);
        // full copy (they should be the same then)
        ph2.copy_words(ph.get_words(), ph.get_word_count());
        CHECK_FALSE(ph2 < ph);
        CHECK_FALSE(ph < ph2);
        CHECK((ph == ph2));
        CHECK((ph2 == ph));
        // should be case INsensitive
        ph2.get_words().at(0) = L"TIME";
        CHECK_FALSE(ph2 < ph);
        CHECK_FALSE(ph < ph2);
        CHECK((ph == ph2));
        CHECK((ph2 == ph));
        // last word in phrase being bigger will make whole phrase bigger than the other
        ph2.get_words().at(3) = L"lite";
        CHECK_FALSE(ph2 < ph);
        CHECK((ph < ph2));
        CHECK_FALSE(ph == ph2);
        CHECK_FALSE(ph2 == ph);
        // phrase that starts with "zoo" should be bigger,
        // even though it has less words than the other phrase
        ph2.clear_words();
        ph2.add_word(L"zoo");
        CHECK_FALSE(ph2 < ph);
        CHECK((ph < ph2));
        CHECK_FALSE(ph == ph2);
        CHECK_FALSE(ph2 == ph);
        }
    SECTION("Copy Words")
        {
        grammar::phrase<> ph;
        ph.add_word(L"time");
        ph.add_word(L"of");
        ph.add_word(L"your");
        ph.add_word(L"life");

        grammar::phrase<> ph2;
        // test partial copy
        ph2.copy_words(ph.get_words(), 2);
        CHECK(ph2.get_word_count() == 2);
        CHECK(ph2.get_words()[0] == L"time");
        CHECK(ph2.get_words()[1] == L"of");
        // test full copy
        ph2.copy_words(ph.get_words(), ph.get_word_count());
        CHECK(ph2.get_word_count() == 4);
        CHECK(ph2.get_words()[0] == L"time");
        CHECK(ph2.get_words()[1] == L"of");
        CHECK(ph2.get_words()[2] == L"your");
        CHECK(ph2.get_words()[3] == L"life");
        // test partial copy again, now destination phrase should be truncated
        ph2.copy_words(ph.get_words(), 2);
        CHECK(ph2.get_word_count() == 2);
        CHECK(ph2.get_words()[0] == L"time");
        CHECK(ph2.get_words()[1] == L"of");
        }
    SECTION("Get Set Type")
        {
        grammar::phrase<> ph;
        ph.add_word(L"time");
        ph.add_word(L"of");
        CHECK(ph.get_type() == grammar::phrase_type::phrase_wordy);// this is the default
        ph.set_type(grammar::phrase_type::phrase_cliche);
        CHECK(ph.get_type() == grammar::phrase_type::phrase_cliche);
        ph.set_type(grammar::phrase_type::phrase_redundant);
        CHECK(ph.get_type() == grammar::phrase_type::phrase_redundant);
        }
    SECTION("Add Word")
        {
        grammar::phrase<> ph;
        ph.add_word(L"time");
        ph.add_word(L"of");
        ph.add_word(L"your");
        ph.add_word(L"life");
        CHECK(ph.get_word_count() == 4);
        CHECK(ph.get_words()[0] == L"time");
        CHECK(ph.get_words()[1] == L"of");
        CHECK(ph.get_words()[2] == L"your");
        CHECK(ph.get_words()[3] == L"life");
        }
    SECTION("Resize")
        {
        grammar::phrase<> ph;
        ph.add_word(L"time");
        ph.add_word(L"of");
        ph.add_word(L"your");
        ph.add_word(L"life");
        CHECK(ph.get_word_count() == 4);
        CHECK(ph.get_words()[0] == L"time");
        CHECK(ph.get_words()[1] == L"of");
        CHECK(ph.get_words()[2] == L"your");
        CHECK(ph.get_words()[3] == L"life");
        ph.resize(2);
        CHECK(ph.get_word_count() == 2);
        CHECK(ph.get_words()[0] == L"time");
        CHECK(ph.get_words()[1] == L"of");
        ph.resize(0);
        CHECK(ph.get_word_count() == 0);
        }
    SECTION("Clear")
        {
        grammar::phrase<> ph;
        ph.add_word(L"time");
        ph.add_word(L"of");
        ph.add_word(L"your");
        ph.add_word(L"life");
        CHECK_FALSE(ph.is_empty());
        ph.clear_words();
        CHECK(ph.get_word_count() == 0);
        CHECK(ph.is_empty());
        }
    }

TEST_CASE("Paragraph parser", "[paragraphs]")
    {
    SECTION("Headers")
        {
        const wchar_t testText[] = L"Header  \n\nNext paragraph";
        tokenize::document_tokenize<> tokenize(testText, wcslen(testText), false, false, false, false);
        while (tokenize() ) {};
        CHECK(tokenize.get_current_paragraph_index() == 1);
        }
    SECTION("NoSentence Ending")
        {
        const wchar_t testText[] = L"Some text.\n\nTHE END";
        tokenize::document_tokenize<> tokenize(testText, wcslen(testText), false, false, false, false);
        while (tokenize() ) {};
        CHECK(tokenize.get_current_sentence_ending_punctuation() == ' ');
        //zero based, so this means there are two sentences
        CHECK(tokenize.get_current_sentence_index() == 1);
        }
    SECTION("NoSentence Ending With Crlf")
        {
        const wchar_t testText[] = L"Some text.\n\nTHE END\n";
        tokenize::document_tokenize<> tokenize(testText, wcslen(testText), false, false, false, false);
        while (tokenize() ) {};
        CHECK(tokenize.get_current_sentence_ending_punctuation() == ' ');
        }
    SECTION("Mixed Sentence Ending")
        {
        const wchar_t testText[] = L"Some text.\n\nTHE END?";
        tokenize::document_tokenize<> tokenize(testText, wcslen(testText), false, false, false, false);
        while (tokenize() ) {};
        CHECK(tokenize.get_current_sentence_ending_punctuation() == '?');
        }
    SECTION("Sentence End Parenthesis")
        {
        const wchar_t testText[] = L"(I am Mr. Madden.) Hello";
        tokenize::document_tokenize<> tokenize(testText, wcslen(testText), false, false, false, false);
        while (tokenize() ) {};
        // zero based, so this means there are two sentences
        CHECK(tokenize.get_current_sentence_index() == 1);
        }
    SECTION("Sentence End Parenthesis 2")
        {
        const wchar_t testText[] = L"(I am Mr. Madden). Hello";
        tokenize::document_tokenize<> tokenize(testText, wcslen(testText), false, false, false, false);
        while (tokenize() ) {};
        // zero based, so this means there are two sentences
        CHECK(tokenize.get_current_sentence_index() == 1);
        }
    SECTION("Sentence End Parenthesis With Spaces")
        {
        const wchar_t testText[] = L"(I am Mr. Madden) \t. Hello";
        tokenize::document_tokenize<> tokenize(testText, wcslen(testText), false, false, false, false);
        while (tokenize() ) {};
        //zero based, so this means there are two sentences
        CHECK(tokenize.get_current_sentence_index() == 1);
        }
    SECTION("Sentence End Parenthesis With Spaces 2")
        {
        const wchar_t testText[] = L"(I am Mr. Madden) \t";
        tokenize::document_tokenize<> tokenize(testText, wcslen(testText), false, false, false, false);
        while (tokenize() ) {};
        // zero based, so this means there one sentence
        CHECK(tokenize.get_current_sentence_index() == 0);
        }
    }

TEST_CASE("Double words", "[double words]")
    {
    SECTION("English")
        {
        is_double_word_exception exp;
        CHECK(exp(L"ha"));
        CHECK_FALSE(exp(L"hat"));
        CHECK_FALSE(exp(L"h"));
        CHECK(exp(L"had"));
        CHECK(exp(L"that"));
        }
    SECTION("German")
        {
        is_double_word_exception exp;
        CHECK(exp(L"das"));
        CHECK_FALSE(exp(L"dast"));
        CHECK_FALSE(exp(L"da"));
        CHECK(exp(L"der"));
        CHECK(exp(L"die"));
        CHECK(exp(L"sie"));
        }
    SECTION("Single Letter")
        {
        is_double_word_exception exp;
        CHECK_FALSE(exp(L"a"));
        }
    SECTION("Single Punctuation And Number")
        {
        is_double_word_exception exp;
        CHECK(exp(L"#"));
        CHECK(exp(L"$"));
        }
    }

TEST_CASE("Indent", "[indent]")
    {
    SECTION("Null")
        {
        grammar::is_indented_text is_indented;
        CHECK_FALSE(is_indented(L"").first);
        CHECK(is_indented(L"").second == 0);
        }
    SECTION("Tab")
        {
        const wchar_t text[] = L"\titem 1";
        grammar::is_indented_text is_indented;
        CHECK(is_indented(text).first);
        CHECK(is_indented(text).second == 1);
        }
    SECTION("Space Tab")
        {
        const wchar_t text[] = L" \t item 1";
        grammar::is_indented_text is_indented;
        CHECK(is_indented(text).first);
        CHECK(is_indented(text).second == 3);
        }
    SECTION("No Space")
        {
        const wchar_t text[] = L"item 1";
        grammar::is_indented_text is_indented;
        CHECK_FALSE(is_indented(text).first);
        CHECK(is_indented(text).second == 0);
        }
    SECTION("One Space")
        {
        const wchar_t text[] = L" item 1";
        grammar::is_indented_text is_indented;
        CHECK_FALSE(is_indented(text).first);
        CHECK(is_indented(text).second == 0);
        }
    SECTION("Two Space")
        {
        const wchar_t text[] = L"  item 1";
        grammar::is_indented_text is_indented;
        CHECK_FALSE(is_indented(text).first);
        CHECK(is_indented(text).second == 0);
        }
    SECTION("Spaces")
        {
        const wchar_t text[] = L"   item 1";
        grammar::is_indented_text is_indented;
        CHECK(is_indented(text).first);
        CHECK(is_indented(text).second == 3);
        }
    SECTION("All Spaces")
        {
        const wchar_t text[] = L"     ";
        grammar::is_indented_text is_indented;
        CHECK(is_indented(text).first);
        CHECK(is_indented(text).second == 5);
        }
    }

TEST_CASE("Abbreviations simple", "[abbreviations]")
    {
    grammar::is_abbreviation abb;
    abb.get_abbreviations().add_word(L"std.");
    abb.get_abbreviations().add_word(L"dept.");
    abb.get_abbreviations().add_word(L"mar.");
    abb.get_abbreviations().add_word(L"jan.");

    SECTION("Test Nulls")
        {
        CHECK_FALSE(abb(L""));
        }
    SECTION("Known")
        {
        CHECK(abb(L"std."));
        CHECK(abb(L"dept."));
        CHECK_FALSE(abb(L"stand."));
        }
    SECTION("Unknown")
        {
        CHECK(abb(L"stdr.")); // all consonants
        }
    SECTION("Slash")
        {
        CHECK(abb(L"class/dept.")); // dept. is an abbreviation
        CHECK_FALSE(abb(L"class/depart.")); // depart. is not an abbreviation
        CHECK_FALSE(abb(L"class/")); // nothing there
        }
    }

TEST_CASE("Personal pronouns", "[pronouns]")
    {
    SECTION("Null")
        {
        is_personal_pronoun isPersonalPronoun;
        CHECK_FALSE(isPersonalPronoun(L""));
        }
    SECTION("Most")
        {
        is_personal_pronoun isPersonalPronoun;
        CHECK(isPersonalPronoun(L"hE"));
        CHECK(isPersonalPronoun(L"hIM"));
        CHECK(isPersonalPronoun(L"i"));
        CHECK(isPersonalPronoun(L"Them"));
        CHECK(isPersonalPronoun(L"WE"));
        CHECK(isPersonalPronoun(L"uS"));
        }
    SECTION("None")
        {
        is_personal_pronoun isPersonalPronoun;
        CHECK_FALSE(isPersonalPronoun(L"An"));
        CHECK_FALSE(isPersonalPronoun(L"a"));
        CHECK_FALSE(isPersonalPronoun(L"a"));
        CHECK_FALSE(isPersonalPronoun(L"anderson"));
        CHECK_FALSE(isPersonalPronoun(L"butt"));
        CHECK_FALSE(isPersonalPronoun(L"ore"));
        CHECK_FALSE(isPersonalPronoun(L"some"));
        }
    }

TEST_CASE("Negating words", "[negating]")
    {
    SECTION("Null")
        {
        is_negating isNegating;
        CHECK_FALSE(isNegating(L""));
        }
    SECTION("Most")
        {
        is_negating isNegating;
        CHECK(isNegating(L"aIn't"));
        CHECK(isNegating(L"nOWhere"));
        CHECK(isNegating(L"no"));
        CHECK(isNegating(L"non"));
        CHECK(isNegating(L"woULDn't"));
        CHECK(isNegating(L"mUstnt"));
        CHECK(isNegating(L"dont"));
        }
    SECTION("None")
        {
        is_negating isNegating;
        CHECK_FALSE(isNegating(L"An"));
        CHECK_FALSE(isNegating(L"a"));
        CHECK_FALSE(isNegating(L"a"));
        CHECK_FALSE(isNegating(L"anderson"));
        CHECK_FALSE(isNegating(L"butt"));
        CHECK_FALSE(isNegating(L"ore"));
        CHECK_FALSE(isNegating(L"some"));
        }
    }

TEST_CASE("Abbreviations", "[abbreviations]")
    {
    grammar::is_abbreviation isAbbreviation;
    isAbbreviation.get_abbreviations().load_words(L"dr.\nmr.\nmrs.\nms.\nstd.\nmar.\njan.", true, true);

    SECTION("Known Abbreviations")
        {
        const wchar_t* text = L"Hi Mr. Smith, are you and Ms. Smith going to MRU. ";
        CHECK(isAbbreviation({ text + 3, 3 }));
        CHECK(isAbbreviation({ text + 26, 3 }));
        CHECK_FALSE(isAbbreviation({ text + 45, 4 }));
        }
    SECTION("Unknown Abbreviations")
        {
        const wchar_t* text = L"Go to the Rptr. room";
        CHECK(isAbbreviation({ text + 10, 5 }));
        }
    SECTION("Actually Acronym")
        {
        const wchar_t* text = L"Go to the RPTR. room";
        CHECK_FALSE(isAbbreviation({ text + 10, 5 }));
        }
    SECTION("Hyphenated Abbreviations")
        {
        const wchar_t* text = L"Standard/std. deviation.";
        CHECK(isAbbreviation({ text, 13 }));
        }
    SECTION("Double Abbreviation")
        {
        const wchar_t* text = L"st.dev.";
        CHECK(isAbbreviation(text));
        }
    SECTION("Not Abbreviation Too Short To Deduce")
        {
        const wchar_t* text = L"zt.";
        CHECK_FALSE(isAbbreviation(text));
        }
    SECTION("Not Abbreviation Really Acronym")
        {
        const wchar_t* text = L"k.a.o.s.";
        CHECK_FALSE(isAbbreviation(text));
        }
    SECTION("Not Abbreviation Really Initial")
        {
        const wchar_t* text = L"P.";
        CHECK_FALSE(isAbbreviation(text));
        }
    }

TEST_CASE("Spanish conjunction", "[conjunction]")
    {
    SECTION("All")
        {
        is_spanish_coordinating_conjunction isConj;
        CHECK(isConj(L"y"));
        CHECK(isConj(L"e"));
        CHECK(isConj(L"o"));
        CHECK(isConj(L"u"));
        CHECK(isConj(L"Y"));
        CHECK(isConj(L"E"));
        CHECK(isConj(L"O"));
        CHECK(isConj(L"U"));
        CHECK(isConj(L"peRo"));
        CHECK(isConj(L"SIno"));
        CHECK(isConj(L"nI"));
        }
    SECTION("NonConjunctions")
        {
        is_spanish_coordinating_conjunction isConj;
        CHECK_FALSE(isConj(L"Perot"));
        CHECK_FALSE(isConj(L"Ye"));
        CHECK_FALSE(isConj(L"sin"));
        CHECK_FALSE(isConj(L"nin"));
        }
    }

TEST_CASE("English conjunction", "[conjunction]")
    {
    SECTION("All")
        {
        is_english_coordinating_conjunction isConj;
        CHECK(isConj(L"anD"));
        CHECK(isConj(L"BUt"));
        CHECK(isConj(L"Or"));
        CHECK(isConj(L"yeT"));
        CHECK(isConj(L"nOr"));
        CHECK(isConj(L"So"));
        }
    SECTION("NonConjunctions")
        {
        is_english_coordinating_conjunction isConj;
        CHECK_FALSE(isConj(L"An"));
        CHECK_FALSE(isConj(L"a"));
        CHECK_FALSE(isConj(L"a"));
        CHECK_FALSE(isConj(L"anderson"));
        CHECK_FALSE(isConj(L"butt"));
        CHECK_FALSE(isConj(L"ore"));
        CHECK_FALSE(isConj(L"some"));
        }
    }

TEST_CASE("German conjunction", "[conjunction]")
    {
    SECTION("All")
        {
        is_german_coordinating_conjunction isConj;
        CHECK(isConj(L"und"));
        CHECK(isConj(L"oder"));
        CHECK(isConj(L"denn"));
        CHECK(isConj(L"aber"));
        CHECK(isConj(L"sondern"));
        CHECK(isConj(L"UND"));
        CHECK(isConj(L"ODER"));
        CHECK(isConj(L"DENN"));
        CHECK(isConj(L"ABER"));
        CHECK(isConj(L"SONDERN"));
        }
    SECTION("NonConjunctions")
        {
        is_german_coordinating_conjunction isConj;
        CHECK_FALSE(isConj(L"undie"));
        CHECK_FALSE(isConj(L"ode"));
        CHECK_FALSE(isConj(L"abers"));
        CHECK_FALSE(isConj(L"sond"));
        }
    }

TEST_CASE("Exclude words", "[excludewords]")
    {
    SECTION("Phrases")
        {
        excludedPMap->load_phrases(L"star wars\nfor real", true, false);
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"The Star Wars prequels were awful star. Wars prequels were bad, for real. Star, wars prequels weren't good.";
        doc.set_excluded_phrase_function(excludedPMap);
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_words()[0].is_valid());
        CHECK_FALSE(doc.get_words()[1].is_valid());
        CHECK_FALSE(doc.get_words()[2].is_valid());
        CHECK(doc.get_words()[3].is_valid());
        CHECK(doc.get_words()[4].is_valid());
        CHECK(doc.get_words()[5].is_valid());
        CHECK(doc.get_words()[6].is_valid());
        CHECK(doc.get_words()[7].is_valid());
        CHECK(doc.get_words()[8].is_valid());
        CHECK(doc.get_words()[9].is_valid());
        CHECK(doc.get_words()[10].is_valid());
        CHECK_FALSE(doc.get_words()[11].is_valid());
        CHECK_FALSE(doc.get_words()[12].is_valid());
        CHECK(doc.get_words()[13].is_valid());
        CHECK(doc.get_words()[14].is_valid());
        CHECK(doc.get_words()[15].is_valid());
        CHECK(doc.get_words()[16].is_valid());
        CHECK(doc.get_words()[17].is_valid());
        CHECK(doc.get_sentences()[0].get_word_count() == 7);
        CHECK(doc.get_sentences()[0].get_valid_word_count() == 5);
        CHECK(doc.get_sentences()[1].get_word_count() == 6);
        CHECK(doc.get_sentences()[1].get_valid_word_count() == 4);
        CHECK(doc.get_sentences()[2].get_word_count() == 5);
        CHECK(doc.get_sentences()[2].get_valid_word_count() == 5);
        CHECK(doc.get_word_count() == 18);
        CHECK(doc.get_valid_word_count() == 14);
        CHECK(doc.get_punctuation_count() == 2);
        CHECK(doc.get_valid_punctuation_count() == 2);
        }
    SECTION("Phrases Start AndEnd Of Sentences")
        {
        excludedPMap->load_phrases(L"star wars\nfor real\nprequels", true, false);
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Star Wars prequels were awful star. Wars prequels were bad, for real.";
        doc.set_excluded_phrase_function(excludedPMap);
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK_FALSE(doc.get_words()[0].is_valid());
        CHECK_FALSE(doc.get_words()[1].is_valid());
        CHECK_FALSE(doc.get_words()[2].is_valid());
        CHECK(doc.get_words()[3].is_valid());
        CHECK(doc.get_words()[4].is_valid());
        CHECK(doc.get_words()[5].is_valid());
        CHECK(doc.get_words()[6].is_valid());
        CHECK_FALSE(doc.get_words()[7].is_valid());
        CHECK(doc.get_words()[8].is_valid());
        CHECK(doc.get_words()[9].is_valid());
        CHECK_FALSE(doc.get_words()[10].is_valid());
        CHECK_FALSE(doc.get_words()[11].is_valid());
        CHECK(doc.get_sentences()[0].get_word_count() == 6);
        CHECK(doc.get_sentences()[0].get_valid_word_count() == 3);
        CHECK(doc.get_sentences()[1].get_word_count() == 6);
        CHECK(doc.get_sentences()[1].get_valid_word_count() == 3);
        CHECK(doc.get_word_count() == 12);
        CHECK(doc.get_valid_word_count() == 6);
        CHECK(doc.get_punctuation_count() == 1);
        CHECK(doc.get_valid_punctuation_count() == 1);
        }
    SECTION("Phrases Include First Instance")
        {
        excludedPMap->load_phrases(L"star wars\nfor real\rbad", true, false);
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"The Star Wars prequels were awful star. Wars prequels were bad, for real. Star, wars prequels weren't for real, so bad!";
        doc.set_excluded_phrase_function(excludedPMap);
        doc.include_excluded_phrase_first_occurrence(true);
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_words()[0].is_valid());
        CHECK(doc.get_words()[1].is_valid());
        CHECK(doc.get_words()[2].is_valid());
        CHECK(doc.get_words()[3].is_valid());
        CHECK(doc.get_words()[4].is_valid());
        CHECK(doc.get_words()[5].is_valid());
        CHECK(doc.get_words()[6].is_valid());
        CHECK(doc.get_words()[7].is_valid());
        CHECK(doc.get_words()[8].is_valid());
        CHECK(doc.get_words()[9].is_valid());
        CHECK(doc.get_words()[10].is_valid());
        CHECK(doc.get_words()[11].is_valid());
        CHECK(doc.get_words()[12].is_valid());
        CHECK(doc.get_words()[13].is_valid());
        CHECK(doc.get_words()[14].is_valid());
        CHECK(doc.get_words()[15].is_valid());
        CHECK(doc.get_words()[16].is_valid());
        CHECK_FALSE(doc.get_words()[17].is_valid());
        CHECK_FALSE(doc.get_words()[18].is_valid());
        CHECK(doc.get_words()[19].is_valid());
        CHECK_FALSE(doc.get_words()[20].is_valid());
        CHECK(doc.get_sentences()[0].get_word_count() == 7);
        CHECK(doc.get_sentences()[0].get_valid_word_count() == 7);
        CHECK(doc.get_sentences()[1].get_word_count() == 6);
        CHECK(doc.get_sentences()[1].get_valid_word_count() == 6);
        CHECK(doc.get_sentences()[2].get_word_count() == 8);
        CHECK(doc.get_sentences()[2].get_valid_word_count() == 5);
        CHECK(doc.get_word_count() == 21);
        CHECK(doc.get_valid_word_count() == 18);
        CHECK(doc.get_punctuation_count() == 3);
        CHECK(doc.get_valid_punctuation_count() == 3);
        }
    SECTION("Punctuation")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Info, Instructions\n\nGo to the website (mycompany.com).";
        doc.exclude_file_addresses(true);
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK_FALSE(doc.get_words()[0].is_valid());
        CHECK_FALSE(doc.get_words()[1].is_valid());
        CHECK(doc.get_words()[2].is_valid());
        CHECK(doc.get_words()[3].is_valid());
        CHECK(doc.get_words()[4].is_valid());
        CHECK(doc.get_words()[5].is_valid());
        CHECK_FALSE(doc.get_words()[6].is_valid());
        CHECK(doc.get_sentences()[0].get_word_count() == 2);
        CHECK(doc.get_sentences()[0].get_valid_word_count() == 0);
        CHECK(doc.get_sentences()[1].get_word_count() == 5);
        CHECK(doc.get_sentences()[1].get_valid_word_count() == 4);
        CHECK(doc.get_word_count() == 7);
        CHECK(doc.get_valid_word_count() == 4);
        CHECK(doc.get_punctuation_count() == 3);
        CHECK(doc.get_valid_punctuation_count() == 2);
        }
    SECTION("Punctuation Headers Are Valid")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Info, Instructions\n\nGo to the website (mycompany.com).\n\nItem , 1\n\nItem , 2\n\nItem , 3";
        doc.exclude_file_addresses(true);
        doc.treat_header_words_as_valid(true);
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_words()[0].is_valid());
        CHECK(doc.get_words()[1].is_valid());
        CHECK(doc.get_words()[2].is_valid());
        CHECK(doc.get_words()[3].is_valid());
        CHECK(doc.get_words()[4].is_valid());
        CHECK(doc.get_words()[5].is_valid());
        CHECK_FALSE(doc.get_words()[6].is_valid());
        CHECK_FALSE(doc.get_words()[7].is_valid());
        CHECK_FALSE(doc.get_words()[8].is_valid());
        CHECK_FALSE(doc.get_words()[9].is_valid());
        CHECK_FALSE(doc.get_words()[10].is_valid());
        CHECK_FALSE(doc.get_words()[11].is_valid());
        CHECK_FALSE(doc.get_words()[12].is_valid());
        CHECK(doc.get_sentences()[0].get_word_count() == 2);
        CHECK(doc.get_sentences()[0].get_valid_word_count() == 2);
        CHECK(doc.get_sentences()[1].get_word_count() == 5);
        CHECK(doc.get_sentences()[1].get_valid_word_count() == 4);
        CHECK(doc.get_sentences()[2].get_word_count() == 2);
        CHECK(doc.get_sentences()[2].get_valid_word_count() == 0);
        CHECK(doc.get_sentences()[3].get_word_count() == 2);
        CHECK(doc.get_sentences()[3].get_valid_word_count() == 0);
        CHECK(doc.get_sentences()[4].get_word_count() == 2);
        CHECK(doc.get_sentences()[4].get_valid_word_count() == 0);
        CHECK(doc.get_word_count() == 13);
        CHECK(doc.get_valid_word_count() == 6);
        CHECK(doc.get_punctuation_count() == 6);
        CHECK(doc.get_valid_punctuation_count() == 3);
        }
    SECTION("Citation Marks")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"A word¹ and another word.ˣˣⁱ Word.ˣˣⁱ";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_words()[0].is_valid());
        CHECK(doc.get_words()[1].is_valid());
        CHECK(doc.get_words()[2].is_valid());
        CHECK(doc.get_words()[3].is_valid());
        CHECK(doc.get_words()[4].is_valid());
        CHECK(doc.get_words()[5].is_valid());

        CHECK(doc.get_words()[1] == L"word");
        CHECK(doc.get_words()[4] == L"word");
        CHECK(doc.get_words()[5] == L"Word");
        CHECK(L'¹' == doc.get_punctuation()[0].get_punctuation_mark());
        CHECK(L'ˣ' == doc.get_punctuation()[1].get_punctuation_mark());
        CHECK(L'ˣ' == doc.get_punctuation()[2].get_punctuation_mark());
        CHECK(L'ⁱ' == doc.get_punctuation()[3].get_punctuation_mark());
        CHECK(L'ˣ' == doc.get_punctuation()[4].get_punctuation_mark());
        CHECK(L'ˣ' == doc.get_punctuation()[5].get_punctuation_mark());
        CHECK(L'ⁱ' == doc.get_punctuation()[6].get_punctuation_mark());
        }
    SECTION("File Paths")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Go to mycompany.com and get the file c:\\mytext.txt.\n\nA Footer";
        doc.exclude_file_addresses(true);
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_words()[0].is_valid());
        CHECK(doc.get_words()[1].is_valid());
        CHECK_FALSE(doc.get_words()[2].is_valid());
        CHECK(doc.get_words()[3].is_valid());
        CHECK(doc.get_words()[4].is_valid());
        CHECK(doc.get_words()[5].is_valid());
        CHECK(doc.get_words()[6].is_valid());
        CHECK_FALSE(doc.get_words()[7].is_valid());
        CHECK_FALSE(doc.get_words()[8].is_valid());
        CHECK_FALSE(doc.get_words()[9].is_valid());
        CHECK(doc.get_sentences()[0].get_word_count() == 8);
        CHECK(doc.get_sentences()[0].get_valid_word_count() == 6);
        CHECK(doc.get_sentences()[1].get_word_count() == 2);
        CHECK(doc.get_sentences()[1].get_valid_word_count() == 0);
        CHECK(doc.get_word_count() == 10);
        CHECK(doc.get_valid_word_count() == 6);
        }
    SECTION("Exclude Numerals")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Go to $5 and get the value 3.14.\n\nA Footer";
        doc.exclude_numerals(true);
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_words()[0].is_valid());
        CHECK(doc.get_words()[1].is_valid());
        CHECK_FALSE(doc.get_words()[2].is_valid());
        CHECK(doc.get_words()[3].is_valid());
        CHECK(doc.get_words()[4].is_valid());
        CHECK(doc.get_words()[5].is_valid());
        CHECK(doc.get_words()[6].is_valid());
        CHECK_FALSE(doc.get_words()[7].is_valid());
        CHECK_FALSE(doc.get_words()[8].is_valid());
        CHECK_FALSE(doc.get_words()[9].is_valid());
        CHECK(doc.get_sentences()[0].get_word_count() == 8);
        CHECK(doc.get_sentences()[0].get_valid_word_count() == 6);
        CHECK(doc.get_sentences()[1].get_word_count() == 2);
        CHECK(doc.get_sentences()[1].get_valid_word_count() == 0);
        CHECK(doc.get_word_count() == 10);
        CHECK(doc.get_valid_word_count() == 6);

        doc.exclude_numerals(false);
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_words()[0].is_valid());
        CHECK(doc.get_words()[1].is_valid());
        CHECK(doc.get_words()[2].is_valid());
        CHECK(doc.get_words()[3].is_valid());
        CHECK(doc.get_words()[4].is_valid());
        CHECK(doc.get_words()[5].is_valid());
        CHECK(doc.get_words()[6].is_valid());
        CHECK(doc.get_words()[7].is_valid());
        CHECK_FALSE(doc.get_words()[8].is_valid());
        CHECK_FALSE(doc.get_words()[9].is_valid());
        CHECK(doc.get_sentences()[0].get_word_count() == 8);
        CHECK(doc.get_sentences()[0].get_valid_word_count() == 8);
        CHECK(doc.get_sentences()[1].get_word_count() == 2);
        CHECK(doc.get_sentences()[1].get_valid_word_count() == 0);
        CHECK(doc.get_word_count() == 10);
        CHECK(doc.get_valid_word_count() == 8);
        }
    SECTION("Exclude Proper Nouns")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"I like talking to Bob Smith. Bob is nice.";
        doc.exclude_proper_nouns(true);
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_words()[0].is_valid());
        CHECK(doc.get_words()[1].is_valid());
        CHECK(doc.get_words()[2].is_valid());
        CHECK(doc.get_words()[3].is_valid());
        CHECK_FALSE(doc.get_words()[4].is_valid());
        CHECK_FALSE(doc.get_words()[5].is_valid());
        CHECK_FALSE(doc.get_words()[6].is_valid());
        CHECK(doc.get_words()[7].is_valid());
        CHECK(doc.get_words()[8].is_valid());
        CHECK(doc.get_sentences()[0].get_word_count() == 6);
        CHECK(doc.get_sentences()[0].get_valid_word_count() == 4);
        CHECK(doc.get_sentences()[1].get_word_count() == 3);
        CHECK(doc.get_sentences()[1].get_valid_word_count() == 2);
        CHECK(doc.get_word_count() == 9);
        CHECK(doc.get_valid_word_count() == 6);
        }
    }

TEST_CASE("Plain language phrase collection", "[plainlanguage]")
    {
    SECTION("Load Basic")
        {
        grammar::plain_language_phrase_collection phrases;
        phrases.load_phrases(
            L"myocardial infarction\theart attack\tA blockage of blood flow to the heart muscle.",
            false, false);
        CHECK(phrases.get_phrases().size() == 1);
        CHECK(phrases.get_phrases().at(0).first.get_word_count() == 2);
        CHECK(phrases.get_phrases().at(0).first.to_string() == L"myocardial infarction");
        CHECK(phrases.get_phrases().at(0).second.replacement.get_word_count() == 2);
        CHECK(phrases.get_phrases().at(0).second.replacement.to_string() == L"heart attack");
        CHECK(phrases.get_phrases().at(0).second.explanation ==
              L"A blockage of blood flow to the heart muscle.");
        }
    SECTION("Load Missing Optional Columns")
        {
        // replacement and explanation are both optional
        grammar::plain_language_phrase_collection phrases;
        phrases.load_phrases(L"myocardial infarction", false, false);
        CHECK(phrases.get_phrases().size() == 1);
        CHECK(phrases.get_phrases().at(0).second.replacement.get_word_count() == 0);
        CHECK(phrases.get_phrases().at(0).second.explanation.empty());
        }
    SECTION("Load Empty")
        {
        grammar::plain_language_phrase_collection phrases;
        phrases.load_phrases(L"", false, false);
        CHECK(phrases.get_phrases().size() == 0);
        }
    SECTION("Load Null")
        {
        grammar::plain_language_phrase_collection phrases;
        phrases.load_phrases(nullptr, false, false);
        CHECK(phrases.get_phrases().size() == 0);
        }
    SECTION("Matching")
        {
        grammar::plain_language_phrase_collection phrases;
        phrases.load_phrases(
            L"myocardial infarction\theart attack\nhypertension\thigh blood pressure", true, false);

        std::vector<std::basic_string<wchar_t, traits::case_insensitive_ex>> words{ L"myocardial",
                                                                                     L"infarction" };
        size_t matchIdx = phrases(words.begin(), 0, words.size(), true);
        REQUIRE(matchIdx != grammar::plain_language_phrase_collection::npos);
        CHECK(phrases.get_phrases().at(matchIdx).first.to_string() == L"myocardial infarction");
        CHECK(phrases.get_phrases().at(matchIdx).second.replacement.to_string() ==
              L"heart attack");

        words = { L"hypertension" };
        matchIdx = phrases(words.begin(), 0, words.size(), true);
        REQUIRE(matchIdx != grammar::plain_language_phrase_collection::npos);
        CHECK(phrases.get_phrases().at(matchIdx).first.to_string() == L"hypertension");
        CHECK(phrases.get_phrases().at(matchIdx).second.replacement.to_string() ==
              L"high blood pressure");

        words = { L"aspirin" };
        CHECK(phrases(words.begin(), 0, words.size(), true) ==
              grammar::plain_language_phrase_collection::npos);
        }
    }

TEST_CASE("Plain language guide analysis", "[plainlanguage]")
    {
    SECTION("Unexplained Phrase Highlighted Every Occurrence")
        {
        plainLanguagePMap->load_phrases(
            L"myocardial infarction\theart attack\tA blockage of blood flow.", true, false);
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings,
                             &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        doc.set_plain_language_phrase_function(plainLanguagePMap);
        const wchar_t* text =
            L"The patient suffered a myocardial infarction. Later, another myocardial "
            L"infarction occurred.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        // "The(0) patient(1) suffered(2) a(3) myocardial(4) infarction(5) Later(6)
        // another(7) myocardial(8) infarction(9) occurred(10)"
        // every occurrence is recorded, explained or not
        REQUIRE(doc.get_plain_language_phrase_indices().size() == 2);
        CHECK(doc.get_plain_language_phrase_indices()[0].first == 4);
        CHECK(doc.get_plain_language_phrase_indices()[0].second == 0);
        CHECK(doc.get_plain_language_phrase_indices()[1].first == 8);
        CHECK(doc.get_plain_language_phrase_indices()[1].second == 0);
        CHECK(doc.get_word(4) == L"myocardial");
        CHECK(doc.get_word(5) == L"infarction");
        CHECK(doc.get_word(8) == L"myocardial");
        CHECK(doc.get_word(9) == L"infarction");
        REQUIRE(doc.get_plain_language_phrase_explained().size() == 1);
        // replacement never appears in the text, so the phrase is never explained
        CHECK_FALSE(doc.get_plain_language_phrase_explained()[0]);
        }
    SECTION("Explained Phrase Never Flagged")
        {
        plainLanguagePMap->load_phrases(
            L"myocardial infarction\theart attack\tA blockage of blood flow.", true, false);
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings,
                             &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        doc.set_plain_language_phrase_function(plainLanguagePMap);
        const wchar_t* text =
            L"The patient suffered a heart attack, also known as a myocardial infarction.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        // "The(0) patient(1) suffered(2) a(3) heart(4) attack(5) also(6) known(7) as(8)
        // a(9) myocardial(10) infarction(11)"
        REQUIRE(doc.get_plain_language_phrase_indices().size() == 1);
        CHECK(doc.get_plain_language_phrase_indices()[0].first == 10);
        CHECK(doc.get_plain_language_phrase_indices()[0].second == 0);
        CHECK(doc.get_word(10) == L"myocardial");
        CHECK(doc.get_word(11) == L"infarction");
        REQUIRE(doc.get_plain_language_phrase_explained().size() == 1);
        CHECK(doc.get_plain_language_phrase_explained()[0]);
        }
    SECTION("Explained Near Later Occurrence Counts For All")
        {
        // the replacement is only near the SECOND occurrence, but once a phrase is
        // explained anywhere, it's explained everywhere in the document
        plainLanguagePMap->load_phrases(
            L"myocardial infarction\theart attack\tA blockage of blood flow.", true, false);
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        doc.set_plain_language_phrase_function(plainLanguagePMap);
        const wchar_t* text =
            L"He had a myocardial infarction last year. This year, he had another "
            L"myocardial infarction, which doctors call a heart attack.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        // "He(0) had(1) a(2) myocardial(3) infarction(4) last(5) year(6) This(7) year(8)
        // he(9) had(10) another(11) myocardial(12) infarction(13) which(14) doctors(15)
        // call(16) a(17) heart(18) attack(19)"
        REQUIRE(doc.get_plain_language_phrase_indices().size() == 2);
        CHECK(doc.get_plain_language_phrase_indices()[0].first == 3);
        CHECK(doc.get_plain_language_phrase_indices()[1].first == 12);
        CHECK(doc.get_word(3) == L"myocardial");
        CHECK(doc.get_word(4) == L"infarction");
        CHECK(doc.get_word(12) == L"myocardial");
        CHECK(doc.get_word(13) == L"infarction");
        CHECK(doc.get_word(18) == L"heart");
        CHECK(doc.get_word(19) == L"attack");
        REQUIRE(doc.get_plain_language_phrase_explained().size() == 1);
        CHECK(doc.get_plain_language_phrase_explained()[0]);
        }
    SECTION("Replacement Within Ten Words Counts")
        {
        // exactly 8 filler words between the technical phrase and its replacement,
        // putting the replacement's last word exactly 10 words after the phrase's last word
        plainLanguagePMap->load_phrases(
            L"myocardial infarction\theart attack\tA blockage of blood flow.", true, false);
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings,
                             &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        doc.set_plain_language_phrase_function(plainLanguagePMap);
        const wchar_t* text = L"myocardial infarction widget widget widget widget widget widget "
                              L"widget widget heart attack.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        // "myocardial(0) infarction(1) widget(2..9) heart(10) attack(11)"
        REQUIRE(doc.get_plain_language_phrase_indices().size() == 1);
        CHECK(doc.get_plain_language_phrase_indices()[0].first == 0);
        CHECK(doc.get_word(0) == L"myocardial");
        CHECK(doc.get_word(1) == L"infarction");
        CHECK(doc.get_word(10) == L"heart");
        CHECK(doc.get_word(11) == L"attack");
        REQUIRE(doc.get_plain_language_phrase_explained().size() == 1);
        CHECK(doc.get_plain_language_phrase_explained()[0]);
        }
    SECTION("Replacement Just Beyond Ten Words Does Not Count")
        {
        // one more filler word than the previous case pushes the replacement's last
        // word one word past the proximity window
        plainLanguagePMap->load_phrases(
            L"myocardial infarction\theart attack\tA blockage of blood flow.", true, false);
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings,
                             &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        doc.set_plain_language_phrase_function(plainLanguagePMap);
        const wchar_t* text =
            L"myocardial infarction widget widget widget widget widget widget widget "
            L"widget widget heart attack.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        // "myocardial(0) infarction(1) widget(2..10) heart(11) attack(12)"
        REQUIRE(doc.get_plain_language_phrase_indices().size() == 1);
        CHECK(doc.get_plain_language_phrase_indices()[0].first == 0);
        CHECK(doc.get_word(0) == L"myocardial");
        CHECK(doc.get_word(1) == L"infarction");
        CHECK(doc.get_word(11) == L"heart");
        CHECK(doc.get_word(12) == L"attack");
        REQUIRE(doc.get_plain_language_phrase_explained().size() == 1);
        // replacement is one word past the proximity window, so it doesn't count
        CHECK_FALSE(doc.get_plain_language_phrase_explained()[0]);
        }
    SECTION("Replacement Before Technical Phrase Within Ten Words Counts")
        {
        // same as "Replacement Within Ten Words Counts," but with the replacement
        // preceding the technical phrase instead of following it
        plainLanguagePMap->load_phrases(
            L"myocardial infarction\theart attack\tA blockage of blood flow.", true, false);
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings,
                             &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        doc.set_plain_language_phrase_function(plainLanguagePMap);
        const wchar_t* text = L"heart attack widget widget widget widget widget widget widget "
                             L"widget myocardial infarction.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        // "heart(0) attack(1) widget(2..9) myocardial(10) infarction(11)"
        REQUIRE(doc.get_plain_language_phrase_indices().size() == 1);
        CHECK(doc.get_plain_language_phrase_indices()[0].first == 10);
        CHECK(doc.get_word(0) == L"heart");
        CHECK(doc.get_word(1) == L"attack");
        CHECK(doc.get_word(10) == L"myocardial");
        CHECK(doc.get_word(11) == L"infarction");
        REQUIRE(doc.get_plain_language_phrase_explained().size() == 1);
        CHECK(doc.get_plain_language_phrase_explained()[0]);
        }
    SECTION("Replacement Before Technical Phrase Just Beyond Ten Words Does Not Count")
        {
        // one more filler word than the previous case pushes the replacement's first
        // word one word before the proximity window
        plainLanguagePMap->load_phrases(
            L"myocardial infarction\theart attack\tA blockage of blood flow.", true, false);
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings,
                             &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        doc.set_plain_language_phrase_function(plainLanguagePMap);
        const wchar_t* text =
            L"heart attack widget widget widget widget widget widget widget widget "
            L"widget myocardial infarction.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        // "heart(0) attack(1) widget(2..10) myocardial(11) infarction(12)"
        REQUIRE(doc.get_plain_language_phrase_indices().size() == 1);
        CHECK(doc.get_plain_language_phrase_indices()[0].first == 11);
        CHECK(doc.get_word(0) == L"heart");
        CHECK(doc.get_word(1) == L"attack");
        CHECK(doc.get_word(11) == L"myocardial");
        CHECK(doc.get_word(12) == L"infarction");
        REQUIRE(doc.get_plain_language_phrase_explained().size() == 1);
        // replacement is one word before the proximity window, so it doesn't count
        CHECK_FALSE(doc.get_plain_language_phrase_explained()[0]);
        }
    SECTION("Replacement Overlapping Technical Phrase Does Not Count")
        {
        // "plain language" (the replacement) immediately precedes "language guide"
        // (the technical phrase) and shares its first word ("language"). That shared
        // word must not let the replacement be mistaken for a real, independent nearby
        // occurrence -- otherwise a phrase could effectively "explain itself."
        plainLanguagePMap->load_phrases(
            L"language guide\tplain language\tA guide written in plain language.", true, false);
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings,
                             &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        doc.set_plain_language_phrase_function(plainLanguagePMap);
        const wchar_t* text = L"Read the plain language guide for more information.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        // "Read(0) the(1) plain(2) language(3) guide(4) for(5) more(6) information(7)"
        REQUIRE(doc.get_plain_language_phrase_indices().size() == 1);
        CHECK(doc.get_plain_language_phrase_indices()[0].first == 3);
        CHECK(doc.get_word(2) == L"plain");
        CHECK(doc.get_word(3) == L"language");
        CHECK(doc.get_word(4) == L"guide");
        REQUIRE(doc.get_plain_language_phrase_explained().size() == 1);
        // replacement overlaps the technical phrase's own words, so it doesn't count
        CHECK_FALSE(doc.get_plain_language_phrase_explained()[0]);
        }
    SECTION("Technical Phrase At End Of Document")
        {
        // the technical phrase's last word is also the document's last word, so the
        // proximity window's upper bound (matchEnd + 10) must clamp to the end of
        // m_words instead of reading past it
        plainLanguagePMap->load_phrases(
            L"myocardial infarction\theart attack\tA blockage of blood flow.", true, false);
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        doc.set_plain_language_phrase_function(plainLanguagePMap);
        const wchar_t* text = L"The patient suffered a myocardial infarction.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        // "The(0) patient(1) suffered(2) a(3) myocardial(4) infarction(5)"
        REQUIRE(doc.get_plain_language_phrase_indices().size() == 1);
        CHECK(doc.get_plain_language_phrase_indices()[0].first == 4);
        CHECK(doc.get_word(4) == L"myocardial");
        CHECK(doc.get_word(5) == L"infarction");
        REQUIRE(doc.get_plain_language_phrase_explained().size() == 1);
        // replacement never appears, so the phrase is unexplained (and, more importantly,
        // the clamp must keep this from reading past the end of the document's word array)
        CHECK_FALSE(doc.get_plain_language_phrase_explained()[0]);
        }
    SECTION("Technical Phrase At Start Of Document")
        {
        // the technical phrase's first word is also the document's first word, so the
        // proximity window's lower bound (matchStart - 10) must clamp to 0 instead of
        // underflowing (matchStart is unsigned)
        plainLanguagePMap->load_phrases(
            L"myocardial infarction\theart attack\tA blockage of blood flow.", true, false);
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        doc.set_plain_language_phrase_function(plainLanguagePMap);
        const wchar_t* text = L"Myocardial infarction is a serious condition.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        // "Myocardial(0) infarction(1) is(2) a(3) serious(4) condition(5)"
        REQUIRE(doc.get_plain_language_phrase_indices().size() == 1);
        CHECK(doc.get_plain_language_phrase_indices()[0].first == 0);
        CHECK(doc.get_word(0) == L"Myocardial");
        CHECK(doc.get_word(1) == L"infarction");
        REQUIRE(doc.get_plain_language_phrase_explained().size() == 1);
        // replacement never appears, so the phrase is unexplained (and, more importantly,
        // the clamp must keep matchStart - 10 from underflowing a size_t)
        CHECK_FALSE(doc.get_plain_language_phrase_explained()[0]);
        }
    SECTION("Multiple Phrases Tracked Independently")
        {
        plainLanguagePMap->load_phrases(
            L"myocardial infarction\theart attack\tExplanation one.\nhypertension\thigh blood "
            L"pressure\tExplanation two.",
            true, false);
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap,
                             &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.set_plain_language_phrase_function(plainLanguagePMap);
        const wchar_t* text =
            L"The patient has hypertension, also known as high blood pressure. He also "
            L"suffered a myocardial infarction.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        // "The(0) patient(1) has(2) hypertension(3) also(4) known(5) as(6) high(7) blood(8)
        // pressure(9) He(10) also(11) suffered(12) a(13) myocardial(14) infarction(15)"
        const auto& loadedPhrases = doc.get_plain_language_phrases().get_phrases();
        size_t hypertensionIdx{ loadedPhrases.size() }, infarctionIdx{ loadedPhrases.size() };
        for (size_t i = 0; i < loadedPhrases.size(); ++i)
            {
            if (loadedPhrases[i].first.to_string() == L"hypertension")
                {
                hypertensionIdx = i;
                }
            else if (loadedPhrases[i].first.to_string() == L"myocardial infarction")
                {
                infarctionIdx = i;
                }
            }
        REQUIRE(hypertensionIdx < loadedPhrases.size());
        REQUIRE(infarctionIdx < loadedPhrases.size());
        // each phrase's own replacement and explanation must stay paired with it,
        // not get crossed with the other row
        CHECK(loadedPhrases[hypertensionIdx].second.replacement.to_string() ==
              L"high blood pressure");
        CHECK(loadedPhrases[hypertensionIdx].second.explanation == L"Explanation two.");
        CHECK(loadedPhrases[infarctionIdx].second.replacement.to_string() == L"heart attack");
        CHECK(loadedPhrases[infarctionIdx].second.explanation == L"Explanation one.");

        REQUIRE(doc.get_plain_language_phrase_indices().size() == 2);
        CHECK(doc.get_plain_language_phrase_indices()[0].first == 3);
        CHECK(doc.get_plain_language_phrase_indices()[0].second == hypertensionIdx);
        CHECK(doc.get_plain_language_phrase_indices()[1].first == 14);
        CHECK(doc.get_plain_language_phrase_indices()[1].second == infarctionIdx);
        CHECK(doc.get_word(3) == L"hypertension");
        CHECK(doc.get_word(14) == L"myocardial");
        CHECK(doc.get_word(15) == L"infarction");

        REQUIRE(doc.get_plain_language_phrase_explained().size() == loadedPhrases.size());
        CHECK(doc.get_plain_language_phrase_explained()[hypertensionIdx]);
        CHECK_FALSE(doc.get_plain_language_phrase_explained()[infarctionIdx]);
        }
    SECTION("No List Set Is A No-Op")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings,
                             &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"The patient suffered a myocardial infarction.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_plain_language_phrase_indices().size() == 0);
        CHECK(doc.get_plain_language_phrase_explained().size() == 0);
        }
    }
// NOLINTEND
// clang-format on
