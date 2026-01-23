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
#include "../src/indexing/article.h"
#include "../src/indexing/word_collection.h"
#include "../src/indexing/spanish_syllabize.h"
#include "../src/indexing/german_syllabize.h"
#include "../src/indexing/word.h"
#include "../src/indexing/word_functional.h"

// clang-format off
// NOLINTBEGIN

using namespace grammar;

using MYWORD = word<traits::case_insensitive_ex,
    stemming::english_stem<std::basic_string<wchar_t, traits::case_insensitive_ex> > >;

extern word_list Stop_list;

TEST_CASE("Spell checker", "[spellchecker]")
    {
    SECTION("Hashtag")
        {
        word_list knownWords;
        word_list customKnownWords;
        knownWords.load_words(L"the cat in cat is all about that", true, true);
        is_correctly_spelled_word<MYWORD, word_list> spellCheck(&knownWords, &customKnownWords, nullptr, false, false, false, false, false, true, false);
        is_social_media_tag<MYWORD> isSmTag;
        MYWORD theWord(L"#WeAreOne");
        theWord.set_social_media_tag(isSmTag(theWord));
        CHECK(!spellCheck(theWord));
        spellCheck.ignore_social_media_tags(true);
        CHECK(spellCheck(theWord));
        }
    SECTION("Known Words")
        {
        word_list knownWords;
        word_list customKnownWords;
        knownWords.load_words(L"the cat in cat is all about that", true, true);
        is_correctly_spelled_word<MYWORD,word_list> spellCheck(&knownWords, &customKnownWords, nullptr, false, false, false, false, false, true, true);
        CHECK(spellCheck(MYWORD(L"THE")));
        CHECK(spellCheck(MYWORD(L"Cat")));
        CHECK(spellCheck(MYWORD(L"cats")) == false);
        CHECK(spellCheck(MYWORD(L"in")));
        CHECK(spellCheck(MYWORD(L"is")));
        CHECK(spellCheck(MYWORD(L"All")));
        CHECK(spellCheck(MYWORD(L"aBout")));
        CHECK(spellCheck(MYWORD(L"tHat")));
        CHECK(spellCheck(MYWORD(L"")) == false);
        }
    SECTION("Custom Words")
        {
        word_list knownWords;
        word_list customKnownWords;
        knownWords.load_words(L"the cat in cat is all about that", true, true);
        customKnownWords.add_word(L"cats");
        customKnownWords.add_word(L"dogs");
        is_correctly_spelled_word<MYWORD,word_list> spellCheck(&knownWords, &customKnownWords, nullptr, false, false, false, false, false, true, true);
        CHECK(spellCheck(MYWORD(L"THE")));
        CHECK(spellCheck(MYWORD(L"Cat")));
        CHECK(spellCheck(MYWORD(L"cats")));
        CHECK(spellCheck(MYWORD(L"dogs")));
        CHECK(spellCheck(MYWORD(L"in")));
        CHECK(spellCheck(MYWORD(L"is")));
        CHECK(spellCheck(MYWORD(L"All")));
        CHECK(spellCheck(MYWORD(L"aBout")));
        CHECK(spellCheck(MYWORD(L"tHat")));
        CHECK(spellCheck(MYWORD(L"")) == false);
        }
    SECTION("Programmer Code")
        {
        word_list knownWords;
        word_list customKnownWords;
        word_list programmerWords;
        knownWords.load_words(L"the cat in cat is all about that", true, true);
        programmerWords.load_words(L"printf is.null true", true, true);
        is_correctly_spelled_word<MYWORD,word_list> spellCheck(&knownWords, &customKnownWords, &programmerWords, false, false, false, false, true, true, true);
        CHECK(spellCheck(MYWORD(L"camelCaseWord")));
        CHECK(spellCheck(MYWORD(L"printf")));
        CHECK(spellCheck(MYWORD(L"is.null")));
        CHECK(spellCheck(MYWORD(L"graph2D")));
        CHECK(spellCheck(MYWORD(L"Text1")));
        CHECK(spellCheck(MYWORD(L"CAPS",4,0,0,0,false,true,false,true,1,0)));
        CHECK(spellCheck(MYWORD(L"Za",4,0,0,0,false,true,false,false,1,0)) == false);
        CHECK(spellCheck(MYWORD(L"ZZ",4,0,0,0,false,true,false,true,1,0)));
        CHECK(spellCheck(MYWORD(L"500",4,0,0,0,true,true,false,false,1,0)));
        CHECK(spellCheck(MYWORD(L"_MyVar")));
        CHECK(spellCheck(MYWORD(L"%s%d")));
        CHECK_FALSE(spellCheck(MYWORD(L"Value")));
        CHECK(spellCheck(MYWORD(L"$Value")));
        CHECK_FALSE(spellCheck(MYWORD(L"pragma")));
        CHECK(spellCheck(MYWORD(L"#pragma")));
        // Powershell
        CHECK(spellCheck(MYWORD(L"Copy-PScommand")));
        CHECK_FALSE(spellCheck(MYWORD(L"Copy-Pcommand")));
        CHECK_FALSE(spellCheck(MYWORD(L"Copy-command")));
        CHECK_FALSE(spellCheck(MYWORD(L"Copy-P")));
        CHECK_FALSE(spellCheck(MYWORD(L"y-P")));
        // UI strings should have & removed and then spell checked
        CHECK(spellCheck(MYWORD(L"&about")));
        CHECK(spellCheck(MYWORD(L"a&bout")));
        CHECK(spellCheck(MYWORD(L"a&bout-cat")));
        CHECK(spellCheck(MYWORD(L"&&")));
        CHECK(spellCheck(MYWORD(L"the\\ncat")));
        CHECK_FALSE(spellCheck(MYWORD(L"the\\ncatz")));
        }
    SECTION("UUID Placeholder")
        {
        word_list knownWords;
        word_list customKnownWords;
        word_list programmerWords;
        knownWords.load_words(L"the cat in cat is all about that", true, true);
        is_correctly_spelled_word<MYWORD,word_list> spellCheck(&knownWords, &customKnownWords, &programmerWords, false, false, false, false, true, true, true);
        CHECK(spellCheck(MYWORD(L"xxxxxxxx")));
        // UUID-like placeholders should not be flagged as misspellings
        CHECK(spellCheck(MYWORD(L"xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx")));
        CHECK(spellCheck(MYWORD(L"XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX")));
        // Actual UUID with hex characters
        CHECK(spellCheck(MYWORD(L"550e8400-e29b-41d4-a716-446655440000")));
        CHECK(spellCheck(MYWORD(L"550E8400-E29B-41D4-A716-446655440000")));
        }
    SECTION("Time With Meridiem")
        {
        word_list knownWords;
        word_list customKnownWords;
        word_list programmerWords;
        // Disable all other checks to isolate time meridiem testing
        is_correctly_spelled_word<MYWORD,word_list> spellCheck(&knownWords, &customKnownWords, &programmerWords,
            false, false, false, false, false, false, false);

        // === VALID TIMES (should pass) ===
        // Single digit hour
        CHECK(spellCheck(MYWORD(L"4:00p.m.")));
        CHECK(spellCheck(MYWORD(L"5:30a.m.")));
        CHECK(spellCheck(MYWORD(L"1:00p.m.")));
        CHECK(spellCheck(MYWORD(L"9:59a.m.")));
        // Double digit hour
        CHECK(spellCheck(MYWORD(L"10:00p.m.")));
        CHECK(spellCheck(MYWORD(L"12:00a.m.")));
        CHECK(spellCheck(MYWORD(L"11:59p.m.")));
        // Leading zero on hour
        CHECK(spellCheck(MYWORD(L"09:00a.m.")));
        CHECK(spellCheck(MYWORD(L"01:30p.m.")));
        // Case variations
        CHECK(spellCheck(MYWORD(L"4:00P.M.")));
        CHECK(spellCheck(MYWORD(L"4:00A.M.")));
        CHECK(spellCheck(MYWORD(L"4:00p.M.")));
        CHECK(spellCheck(MYWORD(L"4:00P.m.")));
        CHECK(spellCheck(MYWORD(L"12:00A.M.")));
        CHECK(spellCheck(MYWORD(L"12:00a.M.")));

        // === INVALID - Wrong length (should fail) ===
        // Too short (7 chars)
        CHECK_FALSE(spellCheck(MYWORD(L"4:0p.m.")));
        CHECK_FALSE(spellCheck(MYWORD(L":00p.m.")));
        // Too long (10 chars)
        CHECK_FALSE(spellCheck(MYWORD(L"123:00p.m.")));
        CHECK_FALSE(spellCheck(MYWORD(L"12:000p.m.")));

        // === INVALID - Missing/wrong punctuation (should fail) ===
        // Missing trailing period
        CHECK_FALSE(spellCheck(MYWORD(L"4:00p.m")));
        CHECK_FALSE(spellCheck(MYWORD(L"12:00p.m")));
        // Missing middle period
        CHECK_FALSE(spellCheck(MYWORD(L"4:00pm.")));
        CHECK_FALSE(spellCheck(MYWORD(L"12:00pm.")));
        // Missing colon
        CHECK_FALSE(spellCheck(MYWORD(L"400p.m..")));
        CHECK_FALSE(spellCheck(MYWORD(L"1200p.m..")));

        // === INVALID - Wrong characters (should fail) ===
        // Invalid meridiem (not 'a' or 'p')
        CHECK_FALSE(spellCheck(MYWORD(L"4:00x.m.")));
        CHECK_FALSE(spellCheck(MYWORD(L"4:00b.m.")));
        CHECK_FALSE(spellCheck(MYWORD(L"12:00z.m.")));
        // Wrong final letter (not 'm')
        CHECK_FALSE(spellCheck(MYWORD(L"4:00p.x.")));
        CHECK_FALSE(spellCheck(MYWORD(L"4:00a.n.")));
        CHECK_FALSE(spellCheck(MYWORD(L"12:00p.z.")));
        // Letter instead of digit in hour
        CHECK_FALSE(spellCheck(MYWORD(L"x:00p.m.")));
        CHECK_FALSE(spellCheck(MYWORD(L"ab:00p.m.")));
        // Letter instead of digit in minutes
        CHECK_FALSE(spellCheck(MYWORD(L"4:x0p.m.")));
        CHECK_FALSE(spellCheck(MYWORD(L"4:0xp.m.")));
        CHECK_FALSE(spellCheck(MYWORD(L"4:xxp.m.")));
        CHECK_FALSE(spellCheck(MYWORD(L"12:abp.m.")));

        // === INVALID - Boundary malformations (should fail) ===
        // Colon in wrong position
        CHECK_FALSE(spellCheck(MYWORD(L"40:0p.m.")));
        CHECK_FALSE(spellCheck(MYWORD(L"400:p.m.")));
        // Period in wrong position
        CHECK_FALSE(spellCheck(MYWORD(L"4:00.pm.")));
        CHECK_FALSE(spellCheck(MYWORD(L"4:.00pm.")));
        // Just meridiem, no time
        CHECK_FALSE(spellCheck(MYWORD(L"p.m.....")));
        CHECK_FALSE(spellCheck(MYWORD(L"a.m.....")));
        // All periods
        CHECK_FALSE(spellCheck(MYWORD(L"........")));
        CHECK_FALSE(spellCheck(MYWORD(L".........")));
        // All digits with periods
        CHECK_FALSE(spellCheck(MYWORD(L"1234.5..")));
        CHECK_FALSE(spellCheck(MYWORD(L"12345.6..")));
        }
    }

TEST_CASE("Social Media", "[social-media]")
    {
    SECTION("Non Hashtag")
        {
        is_social_media_tag<MYWORD> isSmTag;
        CHECK_FALSE(isSmTag(MYWORD(L"THE")));
        CHECK_FALSE(isSmTag(MYWORD(L"THE#ME")));
        }

    SECTION("Hashtag")
        {
        is_social_media_tag<MYWORD> isSmTag;
        CHECK(isSmTag(MYWORD(L"#WeAreOne")));
        CHECK_FALSE(isSmTag(MYWORD(L"#We"))); // too short
        CHECK_FALSE(isSmTag(MYWORD(L"#1254785"))); // number
        CHECK_FALSE(isSmTag(MYWORD(L"##########"))); // all hashtags is nonsense
        MYWORD longNumber(L"#WeAre1");
        longNumber.set_numeric(
            characters::is_character::is_numeric({ longNumber.c_str(), longNumber.length() }));
        CHECK(isSmTag(longNumber));
        }

    SECTION("Handle")
        {
        is_social_media_tag<MYWORD> isSmTag;
        CHECK(isSmTag(MYWORD(L"@AskLibreOffice")));
        CHECK_FALSE(isSmTag(MYWORD(L"@Me"))); // too short
        CHECK_FALSE(isSmTag(MYWORD(L"@1254785"))); // number
        CHECK_FALSE(isSmTag(MYWORD(L"@@@@@@@@"))); // all @ is nonsense
        }

    SECTION("Number")
        {
        is_social_media_tag<MYWORD> isSmTag;
        CHECK_FALSE(isSmTag(MYWORD(L"#1")));
        MYWORD longNumber(L"#10025");
        longNumber.set_numeric(
            characters::is_character::is_numeric({ longNumber.c_str(), longNumber.length() }));
        CHECK_FALSE(isSmTag(longNumber));
        CHECK_FALSE(isSmTag(MYWORD(L"#")));
        }
    }

TEST_CASE("Word Functors", "[word]")
    {
    grammar::english_syllabize ENsyllabizer;
    grammar::spanish_syllabize ESsyllabizer;
    grammar::german_syllabize DEsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("PunctuationGetWord")
        {
        punctuation::punctuation_mark puncts[4] = { punctuation::punctuation_mark(L'?', 0, true),
                                        punctuation::punctuation_mark(L'.', 2, true),
                                        punctuation::punctuation_mark(L'?', 2, false),
                                        punctuation::punctuation_mark(L'!', 7, true) };

        punctuation_mark_count_if_word_position<punctuation::punctuation_mark> functor(2);
        CHECK(functor(puncts[0]) == false);
        CHECK(functor(puncts[1]) == true);
        CHECK(functor(puncts[2]) == true);
        CHECK(functor(puncts[3]) == false);
        }
    SECTION("Letter Counters")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter 1\n\nIt was a dark and stormy night.\n\nThe End";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(std::accumulate(doc.get_words().begin(),doc.get_words().end(),(size_t)0,add_word_size<MYWORD >()) == 38);
        CHECK(std::accumulate(doc.get_words().begin(),doc.get_words().end(),(size_t)0,add_valid_word_size<MYWORD >()) == 24);
        }
    SECTION("CharacterCounterIncludePunctuation")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter 1\n\nIt's a dark and stormy night at Bob's house.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(std::accumulate(doc.get_words().begin(),doc.get_words().end(),(size_t)0,add_word_size<MYWORD >()) == 43);
        CHECK(std::accumulate(doc.get_words().begin(),doc.get_words().end(),(size_t)0,add_valid_word_size<MYWORD >()) == 35);
        }
    SECTION("CharacterCounterIncludeAllPunctuation")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter, 1;\n\nIt's a dark, stormy night (at Bob's house):\n\nThe' end";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(std::accumulate(doc.get_words().begin(),doc.get_words().end(),(size_t)0,add_word_size<MYWORD >()) == 46);
        CHECK(std::accumulate(doc.get_words().begin(),doc.get_words().end(),(size_t)0,add_valid_word_size<MYWORD >()) == 40);

        CHECK(doc.get_punctuation_count() == 5);
        CHECK(doc.get_valid_punctuation_count() == 4);
        }
    SECTION("CharacterCounterIncludeAllPunctuationTrailingPunctuation")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter, 1;\n\nIt's a dark, stormy night (at Bob's house).\n\n--";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_punctuation_count() == 6);
        CHECK(doc.get_valid_punctuation_count() == 4);
        }
    SECTION("CharacterCounterIncludeAllPunctuationWithColonEndingSentence")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter, 1:\n\nIt's a dark, stormy night (at Bob's house).\n\n:The end";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(std::accumulate(doc.get_words().begin(),doc.get_words().end(),(size_t)0,add_word_size<MYWORD >()) == 46);
        CHECK(std::accumulate(doc.get_words().begin(),doc.get_words().end(),(size_t)0,add_valid_word_size<MYWORD >()) == 40);

        CHECK(doc.get_punctuation_count() == 5);
        CHECK(doc.get_valid_punctuation_count() == 4);
        }
    SECTION("LetterCountersExcludingPunctuation")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter 1.0\n\nIt's a dark and stormy night at 12:00.\n\nThe End";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(std::accumulate(doc.get_words().begin(),doc.get_words().end(),(size_t)0,add_word_size_excluding_punctuation<MYWORD >()) == 43);
        CHECK(std::accumulate(doc.get_words().begin(),doc.get_words().end(),(size_t)0,add_valid_word_size_excluding_punctuation<MYWORD >()) == 28);
        }
    SECTION("LetterCountGreaterEquals")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter 1\n\nIt was a dark and stormy night.\n\nThe End";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(std::count_if(doc.get_words().begin(),doc.get_words().end(),word_length_greater_equals<MYWORD >(3)) == 8);
        CHECK(std::count_if(doc.get_words().begin(),doc.get_words().end(),valid_word_length_greater_equals<MYWORD >(3)) == 5);
        }
    SECTION("LetterCountGreaterEqualsExcludingPunctuation")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter's Start\n\nIt's a dark and stormy night.\n\nThe End";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(std::count_if(doc.get_words().begin(),doc.get_words().end(),word_length_greater_equals<MYWORD >(4)) == 6);
        CHECK(std::count_if(doc.get_words().begin(),doc.get_words().end(),valid_word_length_greater_equals<MYWORD >(4)) == 4);
        CHECK(std::count_if(doc.get_words().begin(),doc.get_words().end(),word_length_excluding_punctuation_greater_equals<MYWORD >(4)) == 5);
        CHECK(std::count_if(doc.get_words().begin(),doc.get_words().end(),valid_word_length_excluding_punctuation_greater_equals<MYWORD >(4)) == 3);
        }
    SECTION("Letter Count Equals")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter 1\n\nIt was a dark and stormy night.\n\nThe End";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(std::count_if(doc.get_words().begin(),doc.get_words().end(),word_length_equals<MYWORD >(3)) == 4);
        CHECK(std::count_if(doc.get_words().begin(),doc.get_words().end(),valid_word_length_equals<MYWORD >(3)) == 2);
        }
    SECTION("Numeric")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter 1\n\nIt was a dark and stormy night on July 5, 1974.\n\nThe End (1974)";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(std::count_if(doc.get_words().begin(),doc.get_words().end(),is_numeric<MYWORD >()) == 4);
        CHECK(std::count_if(doc.get_words().begin(),doc.get_words().end(),is_valid_numeric<MYWORD >()) == 2);
        }
    SECTION("Proper Nouns At Start Of Sentence")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Danielson-Bryan is a good test.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(1 == std::count_if(doc.get_words().begin(),doc.get_words().end(),is_proper_noun<MYWORD >()));
        }
    SECTION("ProperNounsAtStartOfSentenceAndExclamatory")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Danielson-Bryan is a good test. WE USE DANIELSON-BRYAN OFTEN!";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(2 == std::count_if(doc.get_words().begin(),doc.get_words().end(),is_proper_noun<MYWORD >()));
        }
    SECTION("ProperNounsAtStartOfSentenceNotReallyProper")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Data-test is a good test.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(0 == std::count_if(doc.get_words().begin(),doc.get_words().end(),is_proper_noun<MYWORD>()));
        }
    SECTION("ProperNounsAtStartOfSingleQuoteNestedQuote")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"\"Well, she said \'Hello there, who you?\'\". It's a \'Method of greeting\'. \"Well, she said \'Hello there who you?\'\" \'Hello?";
        doc.load_document(text, wcslen(text), false, false, false, false);
        /*Method, not Hello (any of them)*/
        CHECK(1 == std::count_if(doc.get_words().begin(),doc.get_words().end(),is_proper_noun<MYWORD>()));
        }
    SECTION("ProperNounsAtStartOfQuote")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"He said \"Danielson-Bryan is a good test.\"";//hyphen makes it seen as proper
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(std::count_if(doc.get_words().begin(),doc.get_words().end(),is_proper_noun<MYWORD>()) == 1);
        }
    SECTION("ProperNounsAtStartOfQuoteNotProper")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"He said \"Hello there.\"";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(0 == std::count_if(doc.get_words().begin(),doc.get_words().end(),is_proper_noun<MYWORD>()));
        }
    SECTION("ProperNounsAtStartOfSingleQuote")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Call him \'Fred\'.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(1 == std::count_if(doc.get_words().begin(),doc.get_words().end(),is_proper_noun<MYWORD>()));
        }
    SECTION("ProperNounsAtStartOfQuoteAndExclamatory")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"He said \"Danielson-Bryan is a good test.\" WE USE DANIELSON-BRYAN OFTEN!";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(2 == std::count_if(doc.get_words().begin(),doc.get_words().end(),is_proper_noun<MYWORD>()));
        }
    SECTION("ProperNounsAtStartOfListItemNumber")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Whaterever, this is my first sentence. And this whaterever is another sentence, Jim.\n\n1. Whaterever, this is a bullet point.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(3 == doc.get_complete_sentence_count());
        CHECK(1 == std::count_if(doc.get_words().begin(),doc.get_words().end(),is_proper_noun<MYWORD>()));
        CHECK(1 == std::count_if(doc.get_words().begin(),doc.get_words().end(),is_valid_proper_noun<MYWORD>()));
        }
    SECTION("ProperNounsAtStartOfListItemNumberWithParan")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Whaterever, this is my first sentence. And this whaterever is another sentence, Jim.\n\n1.) Whaterever, this is a bullet point.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(3 == doc.get_complete_sentence_count());
        CHECK(1 == std::count_if(doc.get_words().begin(),doc.get_words().end(),is_proper_noun<MYWORD>()));
        CHECK(1 == std::count_if(doc.get_words().begin(),doc.get_words().end(),is_valid_proper_noun<MYWORD>()));
        }
    SECTION("ProperNounsAtStartOfListItemNumberAndBullet")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Whaterever, this is my first sentence. And this whaterever is another sentence, Jim.\n\n1. Whaterever, this is a bullet point.\n\n5 French cars are nice to have.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(4 == doc.get_complete_sentence_count());
        CHECK(2 == std::count_if(doc.get_words().begin(),doc.get_words().end(),is_proper_noun<MYWORD>()));
        CHECK(2 == std::count_if(doc.get_words().begin(),doc.get_words().end(),is_valid_proper_noun<MYWORD>()));
        }
    SECTION("ProperNounsAtStartOfListItemBullet")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Whaterever, this is my first sentence. And this whaterever is another sentence, Jim.\n\n- Whaterever, this is a bullet point.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(3 == doc.get_complete_sentence_count());
        CHECK(1 == std::count_if(doc.get_words().begin(),doc.get_words().end(),is_proper_noun<MYWORD>()));
        CHECK(1 == std::count_if(doc.get_words().begin(),doc.get_words().end(),is_valid_proper_noun<MYWORD>()));
        }
    SECTION("Proper Nouns")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter 1\n\nIt was a dark and stormy night on July 5, 1974.\n\nThe End of July (1974)";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(std::count_if(doc.get_words().begin(),doc.get_words().end(),is_proper_noun<MYWORD >()) == 2);
        CHECK(std::count_if(doc.get_words().begin(),doc.get_words().end(),is_valid_proper_noun<MYWORD >()) == 1);
        }
    SECTION("Proper Nouns 2")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"JOE LIKES THE BOOK FRANKENSTEIN! I and Joe like the book Frankenstein. I LIKE THE BOOK FRANKENSTEIN.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_words().operator[](0).is_proper_noun() == true);
        CHECK(doc.get_words().operator[](0).is_acronym() == false);
        CHECK(doc.get_words().operator[](1).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](2).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](3).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](4).is_proper_noun() == true);
        CHECK(doc.get_words().operator[](4).is_acronym() == false);

        CHECK(doc.get_words().operator[](5).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](6).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](7).is_proper_noun() == true);
        CHECK(doc.get_words().operator[](7).is_acronym() == false);
        CHECK(doc.get_words().operator[](8).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](9).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](10).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](11).is_proper_noun() == true);
        CHECK(doc.get_words().operator[](11).is_acronym() == false);

        CHECK(doc.get_words().operator[](12).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](13).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](14).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](15).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](16).is_proper_noun() == true);
        CHECK(doc.get_words().operator[](16).is_acronym() == false);
        }
    SECTION("Proper Nouns 3")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"He works for U.N., AKA NATO.  The UN is a nice place to work, as is NATO.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_words().operator[](0).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](1).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](2).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](3).is_proper_noun() == true);
        CHECK(doc.get_words().operator[](3).is_acronym() == true);
        CHECK(doc.get_words().operator[](4).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](4).is_acronym() == false);
        CHECK(doc.get_words().operator[](5).is_proper_noun() == true);
        CHECK(doc.get_words().operator[](5).is_acronym() == true);

        CHECK(doc.get_words().operator[](6).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](7).is_proper_noun() == true);
        CHECK(doc.get_words().operator[](7).is_acronym() == true);
        CHECK(doc.get_words().operator[](8).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](9).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](10).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](11).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](12).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](13).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](14).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](15).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](16).is_proper_noun() == true);
        CHECK(doc.get_words().operator[](16).is_acronym() == true);
        }
    SECTION("Proper Nouns Conflicting Casing")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Yes, I am here. No, let's play Yes or No. He said 'yes', and she said 'no', he said 'yes'. Micky is here, Micky is Micky!";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_words().operator[](0).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](1).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](2).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](3).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](4).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](5).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](6).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](7).is_proper_noun());
        CHECK(doc.get_words().operator[](8).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](9).is_proper_noun());
        CHECK(doc.get_words().operator[](10).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](11).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](12).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](13).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](14).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](15).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](16).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](17).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](18).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](19).is_proper_noun() == false);
        // 3 Mickys (2 midsentence, 1 at front of sentence). One at front of sentence will be proper.
        CHECK(doc.get_words().operator[](20).is_proper_noun());
        CHECK(doc.get_words().operator[](21).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](22).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](23).is_proper_noun());
        CHECK(doc.get_words().operator[](24).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](25).is_proper_noun());
        }
    SECTION("ProperNounsConflictingCasingInHeader")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Yes, I am here\n\nNo, let's play Yes or No. He said 'yes', and she said 'no', he said 'yes'. Micky is here, Micky is Micky!";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_words().operator[](0).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](1).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](2).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](3).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](4).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](5).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](6).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](7).is_proper_noun());
        CHECK(doc.get_words().operator[](8).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](9).is_proper_noun());
        CHECK(doc.get_words().operator[](10).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](11).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](12).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](13).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](14).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](15).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](16).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](17).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](18).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](19).is_proper_noun() == false);
        // 3 Mickys (2 midsentence, 1 at front of sentence). One at front of sentence will be proper.
        CHECK(doc.get_words().operator[](20).is_proper_noun());
        CHECK(doc.get_words().operator[](21).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](22).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](23).is_proper_noun());
        CHECK(doc.get_words().operator[](24).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](25).is_proper_noun());
        }
    SECTION("ProperNounsWithPluralAcronym")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"THERE ARE THE ATMs.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_words().operator[](0).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](1).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](2).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](3).is_proper_noun() == true);
        CHECK(doc.get_words().operator[](3).is_acronym() == true);
        }
    SECTION("Proper Nouns With Possessive Acronym")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"A's ARE THE ATM's.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_words().operator[](0).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](0).is_acronym() == false);
        CHECK(doc.get_words().operator[](1).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](2).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](3).is_proper_noun() == true);
        CHECK(doc.get_words().operator[](3).is_acronym() == true);
        }
    SECTION("ProperNounsIgnoreHeadersWhenDeducingProperNouns")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        doc.treat_header_words_as_valid(true);
        const wchar_t* text = L"The Book Frankenstein\n\nI and Joe like the book Frankenstein.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_words().operator[](0).is_valid() == true);
        CHECK(doc.get_words().operator[](1).is_valid() == true);
        CHECK(doc.get_words().operator[](2).is_valid() == true);
        CHECK(doc.get_words().operator[](3).is_valid() == true);
        CHECK(doc.get_words().operator[](4).is_valid() == true);
        CHECK(doc.get_words().operator[](5).is_valid() == true);
        CHECK(doc.get_words().operator[](6).is_valid() == true);
        CHECK(doc.get_words().operator[](7).is_valid() == true);
        CHECK(doc.get_words().operator[](8).is_valid() == true);
        CHECK(doc.get_words().operator[](9).is_valid() == true);

        CHECK(doc.get_words().operator[](0).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](1).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](2).is_proper_noun() == true);
        CHECK(doc.get_words().operator[](3).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](4).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](5).is_proper_noun() == true);
        CHECK(doc.get_words().operator[](6).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](7).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](8).is_proper_noun() == false);
        CHECK(doc.get_words().operator[](9).is_proper_noun() == true);
        }
    SECTION("Syllable Ignoring Numerals Counters")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter 1\n\nIt was a dark and stormy night on July 5, 1974.\n\nThe End (1974)";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(std::accumulate(doc.get_words().begin(),doc.get_words().end(),(size_t)0,add_syllable_size_ignore_numerals<MYWORD >()) == 15);
        CHECK(std::accumulate(doc.get_words().begin(),doc.get_words().end(),(size_t)0,add_valid_syllable_size_ignore_numerals<MYWORD >()) == 11);
        }
    SECTION("Syllable Ignoring Numerals And Proper Nouns Counters")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter 1\n\nIt was a dark and stormy night on July 5, 1974.\n\nThe End of July (1974)";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(std::accumulate(doc.get_words().begin(),doc.get_words().end(),(size_t)0,add_syllable_size_ignore_numerals_and_proper_nouns<MYWORD >()) == 14);
        CHECK(std::accumulate(doc.get_words().begin(),doc.get_words().end(),(size_t)0,add_valid_syllable_size_ignore_numerals_and_proper_nouns<MYWORD >()) == 9);
        }
    SECTION("Syllable Counters")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter 1\n\nIt was a dark and stormy night on July 5, 1974.\n\nThe End (1974)";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(std::accumulate(doc.get_words().begin(),doc.get_words().end(),(size_t)0,add_syllable_size<MYWORD >(false)) == 27);
        CHECK(std::accumulate(doc.get_words().begin(),doc.get_words().end(),(size_t)0,add_valid_syllable_size<MYWORD >(false)) == 17);

        CHECK(std::accumulate(doc.get_words().begin(),doc.get_words().end(),(size_t)0,add_syllable_size<MYWORD >(true)) == 19);
        CHECK(std::accumulate(doc.get_words().begin(),doc.get_words().end(),(size_t)0,add_valid_syllable_size<MYWORD >(true)) == 13);
        }
    SECTION("Syllable Count Greater")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter 1\n\nIt was a dark and stormy night on July 5, 1974.\n\nThe End (1974)";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(std::count_if(doc.get_words().begin(),doc.get_words().end(),syllable_count_greater<MYWORD >(1, false)) == 5);
        CHECK(std::count_if(doc.get_words().begin(),doc.get_words().end(),valid_syllable_count_greater<MYWORD >(1, false)) == 3);

        CHECK(std::count_if(doc.get_words().begin(),doc.get_words().end(),syllable_count_greater<MYWORD >(1, true)) == 3);
        CHECK(std::count_if(doc.get_words().begin(),doc.get_words().end(),valid_syllable_count_greater<MYWORD >(1, true)) == 2);
        }
    SECTION("Syllable Count Greater Equal")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter 1\n\nIt was a dark and stormy night on July 5, 1974.\n\nThe End (1974)";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(std::count_if(doc.get_words().begin(),doc.get_words().end(),syllable_count_greater_equal_ignore_numerals<MYWORD >(2)) == 3);
        CHECK(std::count_if(doc.get_words().begin(),doc.get_words().end(),valid_syllable_count_greater_equal_ignore_numerals<MYWORD >(2)) == 2);
        CHECK(std::count_if(doc.get_words().begin(),doc.get_words().end(),valid_syllable_count_greater_equal<MYWORD >(1)) == 11);
        CHECK(std::count_if(doc.get_words().begin(),doc.get_words().end(),valid_syllable_count_greater_equal<MYWORD >(1, true)) == 11);
        }
    SECTION("Syllable Count Greater Equal2")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter 1\n\nIt was a dark and stormy night on December 5, 1974.\n\nThe Unbelievable End (1974)";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(std::count_if(doc.get_words().begin(),doc.get_words().end(),syllable_count_greater_equal_ignore_numerals_and_proper_nouns<MYWORD >(2)) == 3);
        CHECK(std::count_if(doc.get_words().begin(),doc.get_words().end(),valid_syllable_count_greater_equal_ignore_numerals_and_proper_nouns<MYWORD >(2)) == 1);
        }
    SECTION("Syllable Count Equal")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter 7\n\nIt was a dark and stormy night on July 7, 1974.\n\nThe End (1974)";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(std::count_if(doc.get_words().begin(),doc.get_words().end(),syllable_count_equals<MYWORD >(2, false)) == 5);
        CHECK(std::count_if(doc.get_words().begin(),doc.get_words().end(),valid_syllable_count_equals<MYWORD >(2, false)) == 3);

        CHECK(std::count_if(doc.get_words().begin(),doc.get_words().end(),syllable_count_equals<MYWORD >(2, true)) == 3);
        CHECK(std::count_if(doc.get_words().begin(),doc.get_words().end(),valid_syllable_count_equals<MYWORD >(2, true)) == 2);
        }
    }
// NOLINTEND
// clang-format on
