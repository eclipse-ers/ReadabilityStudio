// cppcheck-suppress-file redundantInitialization

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
#include "../src/indexing/german_syllabize.h"
#include "../src/indexing/word.h"
#include "../src/indexing/word_collection.h"

// clang-format off
// NOLINTBEGIN

using namespace grammar;
using MYWORD = word<traits::case_insensitive_ex,
    stemming::english_stem<std::basic_string<wchar_t, traits::case_insensitive_ex> > >;

extern word_list Stop_list;

TEST_CASE("Document phrases", "[document]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("Proper Phrases With Initial")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"It's George R. R. Martin.";
        // allow the initials to be part of the full name
        doc.set_search_for_proper_phrases(true);
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_word(1).is_proper_noun());
        CHECK(doc.get_word(2).is_proper_noun());
        CHECK(doc.get_word(3).is_proper_noun());
        CHECK(doc.get_word(4).is_proper_noun());
        CHECK(1 == doc.get_proper_phrase_indices().size());
        CHECK(1 == doc.get_proper_phrase_indices()[0].first);
        CHECK(4 == doc.get_proper_phrase_indices()[0].second);
        }
    SECTION("Proper Phrases With Semicolon")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Its new fighter, Street Fighter: Turbo Edition.";
        // don't get tripped up by the colon in the title
        doc.set_search_for_proper_phrases(true);
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_word(3).is_proper_noun());
        CHECK(doc.get_word(4).is_proper_noun());
        CHECK(doc.get_word(5).is_proper_noun());
        CHECK(doc.get_word(6).is_proper_noun());
        CHECK(1 == doc.get_proper_phrase_indices().size());
        CHECK(3 == doc.get_proper_phrase_indices()[0].first);
        CHECK(4 == doc.get_proper_phrase_indices()[0].second);
        }
    SECTION("Proper Phrases All Caps")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Shouting, \"LET'S GO!\" at the end.";
        // exclamatory words, not proper.
        doc.set_search_for_proper_phrases(true);
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_word(1).is_exclamatory());
        CHECK(doc.get_word(2).is_exclamatory());
        CHECK(doc.get_word(1).is_proper_noun() == false);
        CHECK(doc.get_word(2).is_proper_noun() == false);
        CHECK(0 == doc.get_proper_phrase_indices().size());
        }
    SECTION("Proper Phrases Auxiliary Word")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Note: the Spache New Formula is great.";
        // "New" is explicitly seen as never proper, but in this case it will
        // at least be part of a proper phrase
        doc.set_search_for_proper_phrases(true);
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_word(2).is_proper_noun());
        CHECK(doc.get_word(3).is_proper_noun() == false);
        CHECK(doc.get_word(4).is_proper_noun());
        CHECK(1 == doc.get_proper_phrase_indices().size());
        CHECK(2 == doc.get_proper_phrase_indices()[0].first);
        CHECK(3 == doc.get_proper_phrase_indices()[0].second);
        }
    SECTION("Proper Phrases Auxiliary Word At Start")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Note: meet the New York Mets.";
        // "New" is explicitly seen as never proper, but in this case it
        // will at least be part of a proper phrase
        doc.set_search_for_proper_phrases(true);
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_word(3).is_proper_noun() == false);
        CHECK(doc.get_word(4).is_proper_noun());
        CHECK(doc.get_word(5).is_proper_noun());
        CHECK(1 == doc.get_proper_phrase_indices().size());
        CHECK(3 == doc.get_proper_phrase_indices()[0].first);
        CHECK(3 == doc.get_proper_phrase_indices()[0].second);
        }
    SECTION("Proper Phrases At Start Of Quote")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"He said \"The New York Yankees play well.\"";
        // "New" is explicitly seen as never proper,
        // but in this case it will at least be part of a proper phrase.
        // Also, "The" should not be part of the phrase.
        doc.set_search_for_proper_phrases(true);
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_word(2).is_proper_noun() == false);
        CHECK(doc.get_word(3).is_proper_noun() == false);
        CHECK(doc.get_word(4).is_proper_noun());
        CHECK(doc.get_word(5).is_proper_noun());
        CHECK(1 == doc.get_proper_phrase_indices().size());
        CHECK(3 == doc.get_proper_phrase_indices()[0].first);
        CHECK(3 == doc.get_proper_phrase_indices()[0].second);
        }
    SECTION("Proper Phrases Connector Words Trailing")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Note: the King of all he saw.";
        // "King of" will be triggered, but need to step back and
        // cut off "of" which isn't connecting to another proper noun
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_word(2).is_proper_noun());
        CHECK(0 == doc.get_proper_phrase_indices().size());
        }
    SECTION("Proper Phrases Connector Words Proceeding")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"It's in The Crypt.";
        // "The" is capitalized, it should be the start of a proper phrase
        // (caller would maybe reformat it to "Crypt, The")
        doc.set_search_for_proper_phrases(true);
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_word(2).is_proper_noun() == false);
        CHECK(doc.get_word(3).is_proper_noun());
        CHECK(1 == doc.get_proper_phrase_indices().size());
        CHECK(2 == doc.get_proper_phrase_indices()[0].first);
        CHECK(2 == doc.get_proper_phrase_indices()[0].second);
        }
    }

TEST_CASE("Document punctuation", "[document]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("Punctuation")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text =
            L"Here are. The items that I need:\n   - Salt (not too salty!)\n\tPepper, peppercorn\n "
            L"   Vinegar (the good stuff.)\n\nThank you!";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_punctuation().size() == 6);
        CHECK(doc.get_punctuation().at(0).get_word_position() == 7);
        CHECK(doc.get_punctuation().at(0).get_punctuation_mark() == '-');
        CHECK(doc.get_punctuation().at(0).is_connected_to_previous_word() == false);

        CHECK(doc.get_punctuation().at(1).get_word_position() == 8);
        CHECK(doc.get_punctuation().at(1).get_punctuation_mark() == '(');
        CHECK(doc.get_punctuation().at(1).is_connected_to_previous_word() == false);

        CHECK(doc.get_punctuation().at(2).get_word_position() == 11);
        CHECK(doc.get_punctuation().at(2).get_punctuation_mark() == ')');
        CHECK(doc.get_punctuation().at(2).is_connected_to_previous_word() == true);

        CHECK(doc.get_punctuation().at(3).get_word_position() == 12);
        CHECK(doc.get_punctuation().at(3).get_punctuation_mark() == ',');
        CHECK(doc.get_punctuation().at(3).is_connected_to_previous_word() == true);

        CHECK(doc.get_punctuation().at(4).get_word_position() == 14);
        CHECK(doc.get_punctuation().at(4).get_punctuation_mark() == '(');
        CHECK(doc.get_punctuation().at(4).is_connected_to_previous_word() == false);

        CHECK(doc.get_punctuation().at(5).get_word_position() == 17);
        CHECK(doc.get_punctuation().at(5).get_punctuation_mark() == ')');
        CHECK(doc.get_punctuation().at(5).is_connected_to_previous_word() == true);
        }
    SECTION("Punctuation2")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Had $ queasy, thumping";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_punctuation().size() == 2);
        CHECK(doc.get_punctuation().at(0).get_word_position() == 1);
        CHECK(doc.get_punctuation().at(0).get_punctuation_mark() == '$');
        CHECK(doc.get_punctuation().at(0).is_connected_to_previous_word() == false);

        CHECK(doc.get_punctuation().at(1).get_word_position() == 2);
        CHECK(doc.get_punctuation().at(1).get_punctuation_mark() == ',');
        CHECK(doc.get_punctuation().at(1).is_connected_to_previous_word() == true);
        }
    SECTION("Punctuation At EOL")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Text ,.\nAnother sentence.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_punctuation().size() == 1);
        CHECK(doc.get_punctuation().at(0).get_word_position() == 1);
        CHECK(doc.get_punctuation().at(0).get_punctuation_mark() == ',');
        CHECK(doc.get_punctuation().at(0).is_connected_to_previous_word() == true);
        }
    SECTION("Punctuation Single Quote")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"‘e' word";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_punctuation().size() == 2);
        CHECK(doc.get_punctuation().at(0).get_word_position() == 0);
        CHECK(doc.get_punctuation().at(0).get_punctuation_mark() == 8216);
        CHECK(doc.get_punctuation().at(1).get_word_position() == 1);
        CHECK(doc.get_punctuation().at(1).get_punctuation_mark() == '\'');
        CHECK(doc.get_words().at(0) == L"e");
        }
    }

TEST_CASE("Document acronym", "[document]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("Acronym False Positive Attention Getter")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"NOTE: don't open that! INFO: read: :this.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_word(0).is_exclamatory());
        CHECK(doc.get_word(0).is_proper_noun() == false);
        CHECK(doc.get_word(0).is_acronym() == false);

        CHECK(doc.get_word(4).is_exclamatory());
        CHECK(doc.get_word(4).is_proper_noun() == false);
        CHECK(doc.get_word(4).is_acronym() == false);
        }
    SECTION("Acronym Start Of Sentence")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"USSR: don't go there! The USSR is difficult to enter.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_word(0).is_exclamatory() == false);
        CHECK(doc.get_word(0).is_proper_noun());
        CHECK(doc.get_word(0).is_acronym());
        }
    SECTION("Acronym False Positive Attention Getter In Quote")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"He said \"NOTE: don't open that! INFO: read: :this.\"";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_word(2).is_exclamatory());
        CHECK(doc.get_word(2).is_proper_noun() == false);
        CHECK(doc.get_word(2).is_acronym() == false);

        CHECK(doc.get_word(6).is_exclamatory());
        CHECK(doc.get_word(6).is_proper_noun() == false);
        CHECK(doc.get_word(6).is_acronym() == false);
        }
    SECTION("Acronym Start Of Sentence In Quote")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"He said \"USSR: don't go there! The USSR is difficult to enter.\"";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_word(2).is_exclamatory() == false);
        CHECK(doc.get_word(2).is_proper_noun());
        CHECK(doc.get_word(2).is_acronym());
        }
    SECTION("Acronym At Document End")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"I like \"S.C.U.B.A.\".";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 1);
        }
    }

TEST_CASE("Document list", "[document]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("List")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text =
            L"Here are. The items that I need:\n   Salt\n\tPepper\n    Vinegar\n\nThank you!";
        doc.load_document(text, wcslen(text), false, false, false, false);
        // Here are the items that I need:
        CHECK(doc.get_words().at(0).is_valid() == true);
        CHECK(doc.get_words().at(1).is_valid() == true);
        CHECK(doc.get_words().at(2).is_valid() == true);
        CHECK(doc.get_words().at(3).is_valid() == true);
        CHECK(doc.get_words().at(4).is_valid() == true);
        CHECK(doc.get_words().at(5).is_valid() == true);
        CHECK(doc.get_words().at(6).is_valid() == true);
        // Pepper/Salt/Vinegar
        CHECK(doc.get_words().at(7).is_valid() == false);
        CHECK(doc.get_words().at(8).is_valid() == false);
        CHECK(doc.get_words().at(9).is_valid() == false);
        // Thank you!
        CHECK(doc.get_words().at(10).is_valid() == true);
        CHECK(doc.get_words().at(11).is_valid() == true);

        CHECK(doc.get_paragraph_count() == 5);
        CHECK(doc.get_valid_paragraph_count() == 2);
        CHECK(doc.get_sentence_count() == 6);
        CHECK(doc.get_complete_sentence_count() == 3);
        CHECK(doc.get_word_count() == 12);
        CHECK(doc.get_valid_word_count() == 9);
        }
    SECTION("List2")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text =
            L"Here are. The items that I need:   \n   Salt\n\tPepper\n    Vinegar\n\nThank you!";
        doc.load_document(text, wcslen(text), false, false, false, false);
        // Here are the items that I need:
        CHECK(doc.get_words().at(0).is_valid() == true);
        CHECK(doc.get_words().at(1).is_valid() == true);
        CHECK(doc.get_words().at(2).is_valid() == true);
        CHECK(doc.get_words().at(3).is_valid() == true);
        CHECK(doc.get_words().at(4).is_valid() == true);
        CHECK(doc.get_words().at(5).is_valid() == true);
        CHECK(doc.get_words().at(6).is_valid() == true);
        // Pepper/Salt/Vinegar
        CHECK(doc.get_words().at(7).is_valid() == false);
        CHECK(doc.get_words().at(8).is_valid() == false);
        CHECK(doc.get_words().at(9).is_valid() == false);
        // Thank you!
        CHECK(doc.get_words().at(10).is_valid() == true);
        CHECK(doc.get_words().at(11).is_valid() == true);

        CHECK(doc.get_paragraph_count() == 5);
        CHECK(doc.get_valid_paragraph_count() == 2);
        CHECK(doc.get_sentence_count() == 6);
        CHECK(doc.get_complete_sentence_count() == 3);
        CHECK(doc.get_word_count() == 12);
        CHECK(doc.get_valid_word_count() == 9);
        }
    SECTION("List3")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text =
            L"Here are. The items that I need:   Salt\n\tPepper\n    Vinegar\n\nThank you!";
        doc.load_document(text, wcslen(text), false, false, false, false);
        // Here are the items that I need:
        CHECK(doc.get_words().at(0).is_valid() == true);
        CHECK(doc.get_words().at(1).is_valid() == true);

        CHECK(doc.get_words().at(2).is_valid() == false);
        CHECK(doc.get_words().at(3).is_valid() == false);
        CHECK(doc.get_words().at(4).is_valid() == false);
        CHECK(doc.get_words().at(5).is_valid() == false);
        CHECK(doc.get_words().at(6).is_valid() == false);
        CHECK(doc.get_words().at(7).is_valid() == false);
        // Pepper/Vinegar
        CHECK(doc.get_words().at(8).is_valid() == false);
        CHECK(doc.get_words().at(9).is_valid() == false);
        // Thank you!
        CHECK(doc.get_words().at(10).is_valid() == true);
        CHECK(doc.get_words().at(11).is_valid() == true);

        CHECK(doc.get_paragraph_count() == 4);
        CHECK(doc.get_valid_paragraph_count() == 2);
        CHECK(doc.get_sentence_count() == 5);
        CHECK(doc.get_complete_sentence_count() == 2);
        CHECK(doc.get_word_count() == 12);
        CHECK(doc.get_valid_word_count() == 4);
        }
    }

TEST_CASE("Document copyrights", "[document]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("Copyright Notices")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        copyrightPMap.load_phrases(L"all rights reserved\ntrademarks\ncopyright", true, false);
        const wchar_t text[] = L"That results in problems.\nInfo 2008 Copyright.\nBlake says All "
                               L"rights reserved by Blake.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_valid_paragraph_count() == 1);
        CHECK(doc.get_valid_word_count() == 4);
        CHECK(doc.get_sentences()[0].is_valid() == true);
        CHECK(doc.get_sentences()[1].is_valid() == false);
        CHECK(doc.get_sentences()[2].is_valid() == false);
        CHECK(doc.get_words()[0].is_valid() == true);
        CHECK(doc.get_words()[1].is_valid() == true);
        CHECK(doc.get_words()[2].is_valid() == true);
        CHECK(doc.get_words()[3].is_valid() == true);
        CHECK(doc.get_words()[4].is_valid() == false);
        CHECK(doc.get_words()[5].is_valid() == false);
        CHECK(doc.get_words()[6].is_valid() == false);
        CHECK(doc.get_words()[7].is_valid() == false);
        CHECK(doc.get_words()[8].is_valid() == false);
        CHECK(doc.get_words()[9].is_valid() == false);
        CHECK(doc.get_words()[10].is_valid() == false);
        CHECK(doc.get_words()[11].is_valid() == false);
        CHECK(doc.get_words()[12].is_valid() == false);
        CHECK(doc.get_words()[13].is_valid() == false);
        }
    SECTION("Copyright Notices2")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        copyrightPMap.load_phrases(L"all rights reserved\ntrademarks\ncopyright", true, false);
        const wchar_t text[] = L"That results in problems.\nCopyright 2007.\nBlake says that All "
                               L"rights reserved by Blake Corp industries and other parties.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_valid_paragraph_count() == 1);
        CHECK(doc.get_valid_word_count() == 4);
        CHECK(doc.get_sentences()[0].is_valid() == true);
        CHECK(doc.get_sentences()[1].is_valid() == false);
        CHECK(doc.get_sentences()[2].is_valid() == false);
        CHECK(doc.get_words()[0].is_valid() == true);
        CHECK(doc.get_words()[1].is_valid() == true);
        CHECK(doc.get_words()[2].is_valid() == true);
        CHECK(doc.get_words()[3].is_valid() == true);
        CHECK(doc.get_words()[4].is_valid() == false);
        CHECK(doc.get_words()[5].is_valid() == false);
        CHECK(doc.get_words()[6].is_valid() == false);
        CHECK(doc.get_words()[7].is_valid() == false);
        CHECK(doc.get_words()[8].is_valid() == false);
        CHECK(doc.get_words()[9].is_valid() == false);
        CHECK(doc.get_words()[10].is_valid() == false);
        CHECK(doc.get_words()[11].is_valid() == false);
        CHECK(doc.get_words()[12].is_valid() == false);
        CHECK(doc.get_words()[13].is_valid() == false);
        CHECK(doc.get_words()[14].is_valid() == false);
        CHECK(doc.get_words()[15].is_valid() == false);
        CHECK(doc.get_words()[16].is_valid() == false);
        CHECK(doc.get_words()[17].is_valid() == false);
        CHECK(doc.get_words()[18].is_valid() == false);
        }
    SECTION("Copyright Notices3")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        copyrightPMap.load_phrases(L"all rights reserved\ntrademarks\ntrademark\ncopyright", true,
                                   false);
        const wchar_t text[] = L"some text.\nsome text.\nsome text.\nmore text.\nIs a copyright "
                               L"of.\nIs a TRademark of inc.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 4);
        CHECK(doc.get_valid_paragraph_count() == 4);
        CHECK(doc.get_valid_word_count() == 8);
        CHECK(doc.get_sentences()[0].is_valid() == true);
        CHECK(doc.get_sentences()[1].is_valid() == true);
        CHECK(doc.get_sentences()[2].is_valid() == true);
        CHECK(doc.get_sentences()[3].is_valid() == true);
        CHECK(doc.get_sentences()[4].is_valid() == false);
        CHECK(doc.get_sentences()[5].is_valid() == false);
        CHECK(doc.get_words()[0].is_valid() == true);
        CHECK(doc.get_words()[1].is_valid() == true);
        CHECK(doc.get_words()[2].is_valid() == true);
        CHECK(doc.get_words()[3].is_valid() == true);
        CHECK(doc.get_words()[4].is_valid() == true);
        CHECK(doc.get_words()[5].is_valid() == true);
        CHECK(doc.get_words()[6].is_valid() == true);
        CHECK(doc.get_words()[7].is_valid() == true);
        CHECK(doc.get_words()[8].is_valid() == false);
        CHECK(doc.get_words()[9].is_valid() == false);
        CHECK(doc.get_words()[10].is_valid() == false);
        CHECK(doc.get_words()[11].is_valid() == false);
        CHECK(doc.get_words()[12].is_valid() == false);
        CHECK(doc.get_words()[13].is_valid() == false);
        CHECK(doc.get_words()[14].is_valid() == false);
        CHECK(doc.get_words()[15].is_valid() == false);
        CHECK(doc.get_words()[16].is_valid() == false);
        }
    SECTION("Copyright Notices Buried")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        copyrightPMap.load_phrases(L"all rights reserved\ntrademarks\ntrademark\ncopyright", true,
                                   false);
        const wchar_t text[] =
            L"some text.\nsome text.\nsome text.\nmore text.\nIs a copyright of.\nIs a TRademark "
            L"of inc.\ninvalid sentence\n\nAnother invalid sentence";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 4);
        CHECK(doc.get_valid_paragraph_count() == 4);
        CHECK(doc.get_valid_word_count() == 8);
        CHECK(doc.get_sentences()[0].is_valid() == true);
        CHECK(doc.get_sentences()[1].is_valid() == true);
        CHECK(doc.get_sentences()[2].is_valid() == true);
        CHECK(doc.get_sentences()[3].is_valid() == true);
        CHECK(doc.get_sentences()[4].is_valid() == false);
        CHECK(doc.get_sentences()[5].is_valid() == false);
        CHECK(doc.get_sentences()[6].is_valid() == false);
        CHECK(doc.get_sentences()[7].is_valid() == false);
        CHECK(doc.get_words()[0].is_valid() == true);
        CHECK(doc.get_words()[1].is_valid() == true);
        CHECK(doc.get_words()[2].is_valid() == true);
        CHECK(doc.get_words()[3].is_valid() == true);
        CHECK(doc.get_words()[4].is_valid() == true);
        CHECK(doc.get_words()[5].is_valid() == true);
        CHECK(doc.get_words()[6].is_valid() == true);
        CHECK(doc.get_words()[7].is_valid() == true);
        CHECK(doc.get_words()[8].is_valid() == false);
        CHECK(doc.get_words()[9].is_valid() == false);
        CHECK(doc.get_words()[10].is_valid() == false);
        CHECK(doc.get_words()[11].is_valid() == false);
        CHECK(doc.get_words()[12].is_valid() == false);
        CHECK(doc.get_words()[13].is_valid() == false);
        CHECK(doc.get_words()[14].is_valid() == false);
        CHECK(doc.get_words()[15].is_valid() == false);
        CHECK(doc.get_words()[16].is_valid() == false);
        CHECK(doc.get_words()[17].is_valid() == false);
        CHECK(doc.get_words()[18].is_valid() == false);
        CHECK(doc.get_words()[19].is_valid() == false);
        CHECK(doc.get_words()[20].is_valid() == false);
        CHECK(doc.get_words()[21].is_valid() == false);
        }
    }

TEST_CASE("Document copyrights 2", "[document]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("Copyright Notices No Valid Text")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        copyrightPMap.load_phrases(L"all rights reserved\ntrademarks\ncopyright", true, false);
        const wchar_t text[] =
            L"Hi.\n\nInfo 2008 Copyright.\nBlake says All rights reserved by Blake.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_valid_paragraph_count() == 1);
        CHECK(doc.get_valid_word_count() == 1);
        CHECK(doc.get_sentences()[0].is_valid());
        CHECK(doc.get_sentences()[1].is_valid() == false);
        CHECK(doc.get_sentences()[2].is_valid() == false);
        CHECK(doc.get_words()[0].is_valid());
        CHECK(doc.get_words()[1].is_valid() == false);
        CHECK(doc.get_words()[2].is_valid() == false);
        CHECK(doc.get_words()[3].is_valid() == false);
        CHECK(doc.get_words()[4].is_valid() == false);
        CHECK(doc.get_words()[5].is_valid() == false);
        CHECK(doc.get_words()[6].is_valid() == false);
        CHECK(doc.get_words()[7].is_valid() == false);
        CHECK(doc.get_words()[8].is_valid() == false);
        CHECK(doc.get_words()[9].is_valid() == false);
        CHECK(doc.get_words()[10].is_valid() == false);
        }
    SECTION("Copyright Notices No Valid Text On Sentence")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        copyrightPMap.load_phrases(L"all rights reserved\ntrademarks\ncopyright", true, false);
        const wchar_t text[] = L"Hi.\n\nInfo 2008 Copyright.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_valid_paragraph_count() == 1);
        CHECK(doc.get_valid_word_count() == 1);
        CHECK(doc.get_sentences()[0].is_valid());
        CHECK(doc.get_sentences()[1].is_valid() == false);
        CHECK(doc.get_words()[0].is_valid());
        CHECK(doc.get_words()[1].is_valid() == false);
        CHECK(doc.get_words()[2].is_valid() == false);
        CHECK(doc.get_words()[3].is_valid() == false);
        }
    SECTION("Copyright Notices In Second Sentence")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        copyrightPMap.load_phrases(L"all rights reserved\ntrademarks\ncopyright", true, false);
        const wchar_t text[] =
            L"Hi.\n\nThat results in problems. Blake says All rights reserved by Blake.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_valid_paragraph_count() == 1);
        CHECK(doc.get_valid_word_count() == 1);
        CHECK(doc.get_sentences()[0].is_valid());
        CHECK(doc.get_sentences()[1].is_valid() == false);
        CHECK(doc.get_sentences()[2].is_valid() == false);
        CHECK(doc.get_words()[0].is_valid());
        CHECK(doc.get_words()[1].is_valid() == false);
        CHECK(doc.get_words()[2].is_valid() == false);
        CHECK(doc.get_words()[3].is_valid() == false);
        CHECK(doc.get_words()[4].is_valid() == false);
        CHECK(doc.get_words()[5].is_valid() == false);
        CHECK(doc.get_words()[6].is_valid() == false);
        CHECK(doc.get_words()[7].is_valid() == false);
        CHECK(doc.get_words()[8].is_valid() == false);
        CHECK(doc.get_words()[9].is_valid() == false);
        CHECK(doc.get_words()[10].is_valid() == false);
        CHECK(doc.get_words()[11].is_valid() == false);
        }
    }

TEST_CASE("Document copyrights 3", "[document]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("Copyright Notices Symbols In Second Sentence")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        copyrightPMap.load_phrases(L"all rights reserved\ntrademarks\ncopyright", true, false);
        const wchar_t text[] = L"Hi.\n\nThat [results] in problems. Blake© says reserved by Blake.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_valid_paragraph_count() == 1);
        CHECK(doc.get_valid_word_count() == 1);
        CHECK(doc.get_sentences()[0].is_valid());
        CHECK(doc.get_sentences()[1].is_valid() == false);
        CHECK(doc.get_sentences()[2].is_valid() == false);
        CHECK(doc.get_words()[0].is_valid());
        CHECK(doc.get_words()[1].is_valid() == false);
        CHECK(doc.get_words()[2].is_valid() == false);
        CHECK(doc.get_words()[3].is_valid() == false);
        CHECK(doc.get_words()[4].is_valid() == false);
        CHECK(doc.get_words()[5].is_valid() == false);
        CHECK(doc.get_words()[6].is_valid() == false);
        CHECK(doc.get_words()[7].is_valid() == false);
        CHECK(doc.get_words()[8].is_valid() == false);
        CHECK(doc.get_words()[9].is_valid() == false);
        }
    SECTION("Copyright Notices Symbols In Second Sentence In Front Of Word")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        copyrightPMap.load_phrases(L"all rights reserved\ntrademarks\ncopyright", true, false);
        const wchar_t text[] = L"That, (results) in [problems].\n\n©Blake says rsd. by Blake.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_valid_paragraph_count() == 1);
        CHECK(doc.get_valid_word_count() == 4);
        CHECK(doc.get_sentences()[0].is_valid());
        CHECK(doc.get_sentences()[1].is_valid() == false);
        CHECK(doc.get_words()[0].is_valid());
        CHECK(doc.get_words()[1].is_valid());
        CHECK(doc.get_words()[2].is_valid());
        CHECK(doc.get_words()[3].is_valid());
        CHECK(doc.get_words()[4].is_valid() == false);
        CHECK(doc.get_words()[5].is_valid() == false);
        CHECK(doc.get_words()[6].is_valid() == false);
        CHECK(doc.get_words()[7].is_valid() == false);
        CHECK(doc.get_words()[8].is_valid() == false);
        }
    SECTION("Copyright Notices Symbols Sentence Boundaries")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        copyrightPMap.load_phrases(L"all rights reserved\ntrademarks\ncopyright", true, false);
        const wchar_t text[] = L"That results in problems all rights. Reserved says reserved.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 2);
        CHECK(doc.get_valid_paragraph_count() == 1);
        CHECK(doc.get_valid_word_count() == 9);
        CHECK(doc.get_sentences()[0].is_valid() == true);
        CHECK(doc.get_sentences()[1].is_valid() == true);
        CHECK(doc.get_words()[0].is_valid() == true);
        CHECK(doc.get_words()[1].is_valid() == true);
        CHECK(doc.get_words()[2].is_valid() == true);
        CHECK(doc.get_words()[3].is_valid() == true);
        CHECK(doc.get_words()[4].is_valid() == true);
        CHECK(doc.get_words()[5].is_valid() == true);
        CHECK(doc.get_words()[6].is_valid() == true);
        CHECK(doc.get_words()[7].is_valid() == true);
        CHECK(doc.get_words()[8].is_valid() == true);
        }
    SECTION("Copyright Notices At End")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        copyrightPMap.load_phrases(L"all rights reserved\ntrademarks\ncopyright", true, false);
        const wchar_t text[] = L"Hi.\n\nThat results in problems.  Oh, and all rights Reserved.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_valid_paragraph_count() == 1);
        CHECK(doc.get_valid_word_count() == 1);
        CHECK(doc.get_sentences()[0].is_valid());
        CHECK(doc.get_sentences()[1].is_valid() == false);
        CHECK(doc.get_sentences()[2].is_valid() == false);
        CHECK(doc.get_words()[0].is_valid());
        CHECK(doc.get_words()[1].is_valid() == false);
        CHECK(doc.get_words()[2].is_valid() == false);
        CHECK(doc.get_words()[3].is_valid() == false);
        CHECK(doc.get_words()[4].is_valid() == false);
        CHECK(doc.get_words()[5].is_valid() == false);
        CHECK(doc.get_words()[6].is_valid() == false);
        CHECK(doc.get_words()[7].is_valid() == false);
        CHECK(doc.get_words()[8].is_valid() == false);
        CHECK(doc.get_words()[9].is_valid() == false);
        }
    SECTION("Copyright Notices At Front")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        copyrightPMap.load_phrases(L"all rights reserved\ntrademarks\ncopyright", true, false);
        const wchar_t text[] = L"Some Text.\n\n© 2010 oleander software.\n\nThat, (results) in "
                               L"[problems].\n\n. This is the last sentence.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 3);
        CHECK(doc.get_valid_paragraph_count() == 3);
        CHECK(doc.get_valid_word_count() == 11);
        CHECK(doc.get_sentences()[0].is_valid());
        CHECK(doc.get_sentences()[1].is_valid() == false);
        CHECK(doc.get_sentences()[2].is_valid());
        CHECK(doc.get_sentences()[3].is_valid());
        CHECK(doc.get_words()[0].is_valid());
        CHECK(doc.get_words()[1].is_valid());
        CHECK(doc.get_words()[2].is_valid() == false);
        CHECK(doc.get_words()[3].is_valid() == false);
        CHECK(doc.get_words()[4].is_valid() == false);
        CHECK(doc.get_words()[5].is_valid());
        CHECK(doc.get_words()[6].is_valid());
        CHECK(doc.get_words()[7].is_valid());
        CHECK(doc.get_words()[8].is_valid());
        CHECK(doc.get_words()[9].is_valid());
        CHECK(doc.get_words()[10].is_valid());
        CHECK(doc.get_words()[11].is_valid());
        CHECK(doc.get_words()[12].is_valid());
        CHECK(doc.get_words()[13].is_valid());
        }
    SECTION("Copyright Notices At Front2")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        copyrightPMap.load_phrases(L"all rights reserved\ntrademarks\ncopyright", true, false);
        const wchar_t text[] = L"Some Text.\n\nCopyright © oleander software.\n\nThat, (results) "
                               L"in [problems].\n\n. This is the last sentence.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 3);
        CHECK(doc.get_valid_paragraph_count() == 3);
        CHECK(doc.get_valid_word_count() == 11);
        CHECK(doc.get_sentences()[0].is_valid());
        CHECK(doc.get_sentences()[1].is_valid() == false);
        CHECK(doc.get_sentences()[2].is_valid());
        CHECK(doc.get_sentences()[3].is_valid());
        CHECK(doc.get_words()[0].is_valid());
        CHECK(doc.get_words()[1].is_valid());
        CHECK(doc.get_words()[2].is_valid() == false);
        CHECK(doc.get_words()[3].is_valid() == false);
        CHECK(doc.get_words()[4].is_valid() == false);
        CHECK(doc.get_words()[5].is_valid());
        CHECK(doc.get_words()[6].is_valid());
        CHECK(doc.get_words()[7].is_valid());
        CHECK(doc.get_words()[8].is_valid());
        CHECK(doc.get_words()[9].is_valid());
        CHECK(doc.get_words()[10].is_valid());
        CHECK(doc.get_words()[11].is_valid());
        CHECK(doc.get_words()[12].is_valid());
        CHECK(doc.get_words()[13].is_valid());
        }
    SECTION("Copyright Notices At End Of Valid Paragraph")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        copyrightPMap.load_phrases(L"all rights reserved\ntrademarks\ncopyright", true, false);
        const wchar_t text[] = L"Some Text*. Oleander software©.\n\nThis is the last "
                               L"sentence.\n\nOK, this is the last sentence.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentences()[0].is_valid());
        CHECK(doc.get_sentences()[1].is_valid());
        CHECK(doc.get_sentences()[2].is_valid());
        CHECK(doc.get_sentences()[3].is_valid());
        CHECK(doc.get_complete_sentence_count() == 4);
        CHECK(doc.get_valid_paragraph_count() == 3);
        CHECK(doc.get_valid_word_count() == 15);
        }
    }
    
TEST_CASE("Document copyrights 4", "[document]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("Copyright Notices Symbols")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        copyrightPMap.load_phrases(L"all rights reserved\nare trademarks of", true, false);
        const wchar_t text[] = L"That\" results, in problems.\n© 2008 Blake corporation.\nBlake® "
                               L"says reserved, by Blake.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_valid_paragraph_count() == 1);
        CHECK(doc.get_valid_word_count() == 4);
        CHECK(doc.get_sentences()[0].is_valid() == true);
        CHECK(doc.get_sentences()[1].is_valid() == false);
        CHECK(doc.get_sentences()[2].is_valid() == false);
        CHECK(doc.get_words()[0].is_valid() == true);
        CHECK(doc.get_words()[1].is_valid() == true);
        CHECK(doc.get_words()[2].is_valid() == true);
        CHECK(doc.get_words()[3].is_valid() == true);
        CHECK(doc.get_words()[4].is_valid() == false);
        CHECK(doc.get_words()[5].is_valid() == false);
        CHECK(doc.get_words()[6].is_valid() == false);
        CHECK(doc.get_words()[7].is_valid() == false);
        CHECK(doc.get_words()[8].is_valid() == false);
        CHECK(doc.get_words()[9].is_valid() == false);
        CHECK(doc.get_words()[10].is_valid() == false);
        CHECK(doc.get_words()[11].is_valid() == false);
        }
    }

TEST_CASE("Document TOC", "[document]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("TOC Line Ignore Blank Lines")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Welcome........5\n\nWelcome...you.\n\nWelcome again....";
        doc.load_document(text, wcslen(text), false, true, true, false);

        CHECK(doc.get_sentence_count() == 3);
        CHECK(doc.get_complete_sentence_count() == 2);
        }
    SECTION("TOC Line Mid Document")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Some, text (more text) here. Welcome...5\n\nWelcome, "
                              L"overview..7\n\nWelcome again.....";
        doc.load_document(text, wcslen(text), false, true, true, false);

        CHECK(doc.get_sentence_count() == 4);
        CHECK(doc.get_complete_sentence_count() == 2);
        }
    SECTION("TOC Line")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Welcome to WIC........ 5\n\nWelcome...you.\n\nWelcome again....";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentence_count() == 3);
        CHECK(doc.get_complete_sentence_count() == 2);
        }
    SECTION("TOC Line No Space")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Welcome to WIC........5\n\nWelcome...you.\n\nWelcome again....";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentence_count() == 3);
        CHECK(doc.get_complete_sentence_count() == 2);
        }
    SECTION("TOC Line One Dot")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L".5 Welcome to WIC . 5.3\n\nWelcome to WIC .5\n\nWelcome again. "
                              L"5\npeople . 5 more people .";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentence_count() == 5);
        CHECK(doc.get_complete_sentence_count() == 3);
        }
    SECTION("TOC Line Ellipses")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Welcome to WIC…… 5\n\nWelcome...you.\n\nWelcome again....";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentence_count() == 3);
        CHECK(doc.get_complete_sentence_count() == 2);
        }
    }

TEST_CASE("Document citations", "[document]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("Citations Exclude After List")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        citationPMap.load_phrases(L"citations\nworks sited\nreferences", true, false);
        const wchar_t text[] = L"Here is a bunch of text in my file that is here for filler, this "
                               L"make it so that it works.\n\n1. Item\n2. Item\n3. Item\n\nMy "
                               L"citations\n\nMy book.\n\nAnother book.\n\nLast Book.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_valid_word_count() == 21);
        CHECK(doc.get_valid_paragraph_count() == 1);
        CHECK(doc.get_sentences().at(0).get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_sentences().at(1).get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences().at(2).get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences().at(3).get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences().at(4).get_type() == sentence_paragraph_type::header);
        CHECK(doc.get_sentences().at(5).get_type() == sentence_paragraph_type::incomplete);
        CHECK(doc.get_sentences().at(6).get_type() == sentence_paragraph_type::incomplete);

        CHECK(doc.get_paragraphs().at(0).get_type() == sentence_paragraph_type::complete);
        CHECK(doc.get_paragraphs().at(1).get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_paragraphs().at(2).get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_paragraphs().at(3).get_type() == sentence_paragraph_type::list_item);
        CHECK(doc.get_paragraphs().at(4).get_type() == sentence_paragraph_type::header);
        CHECK(doc.get_paragraphs().at(5).get_type() == sentence_paragraph_type::incomplete);
        CHECK(doc.get_paragraphs().at(6).get_type() == sentence_paragraph_type::incomplete);
        }
    SECTION("Citations With Date Exclude")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        citationPMap.load_phrases(L"citations\nworks sited\nreferences", true, false);
        const wchar_t text[] = L"Title\n\nHi, this is the body of the document with some citations "
                               L"in it.\n\nMy citations:\n\nPew Research. (2013). Millennials: "
                               L"confident. Summary: Pew Trends. Retrieved from "
                               L"http://www.pewsocialtrends.org/2010/02/24/"
                               L"millennials-confident-connected-open-tochange/";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_valid_word_count() == 13);
        CHECK(doc.get_valid_paragraph_count() == 1);
        CHECK(doc.get_words()[0].is_valid() == false);
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
        CHECK(doc.get_words()[14].is_valid() == false);
        CHECK(doc.get_words()[15].is_valid() == false);
        CHECK(doc.get_words()[16].is_valid() == false);
        CHECK(doc.get_words()[17].is_valid() == false);
        CHECK(doc.get_words()[18].is_valid() == false);
        CHECK(doc.get_words()[19].is_valid() == false);
        CHECK(doc.get_words()[20].is_valid() == false);
        CHECK(doc.get_words()[21].is_valid() == false);
        }
    SECTION("Citations Boundary")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        citationPMap.load_phrases(L"citations\nworks sited\nreferences", true, false);
        const wchar_t text[] = L"Title\n\nHi, this is the body of the document with some citations "
                               L"in it.\n\nMy citations:";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_valid_word_count() == 13);
        CHECK(doc.get_valid_paragraph_count() == 1);
        CHECK(doc.get_words()[0].is_valid() == false);
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
        CHECK(doc.get_words()[14].is_valid() == false);
        CHECK(doc.get_words()[15].is_valid() == false);
        }
    SECTION("Citations Exclude")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        citationPMap.load_phrases(L"citations\nworks sited\nreferences", true, false);
        const wchar_t text[] = L"Title\n\nHi, this is the body of the document.\n\nMy "
                               L"citations:\n\nMy book.\n\nAnother book.\n\nLast Book.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_valid_word_count() == 8);
        CHECK(doc.get_valid_paragraph_count() == 1);
        CHECK(doc.get_words()[0].is_valid() == false);
        CHECK(doc.get_words()[1].is_valid());
        CHECK(doc.get_words()[2].is_valid());
        CHECK(doc.get_words()[3].is_valid());
        CHECK(doc.get_words()[4].is_valid());
        CHECK(doc.get_words()[5].is_valid());
        CHECK(doc.get_words()[6].is_valid());
        CHECK(doc.get_words()[7].is_valid());
        CHECK(doc.get_words()[8].is_valid());
        CHECK(doc.get_words()[9].is_valid() == false);
        CHECK(doc.get_words()[10].is_valid() == false);
        CHECK(doc.get_words()[11].is_valid() == false);
        CHECK(doc.get_words()[12].is_valid() == false);
        CHECK(doc.get_words()[13].is_valid() == false);
        CHECK(doc.get_words()[14].is_valid() == false);
        CHECK(doc.get_words()[15].is_valid() == false);
        CHECK(doc.get_words()[16].is_valid() == false);
        }
    SECTION("Citations Dont Exclude Because In Upper Half Of Document")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        citationPMap.load_phrases(L"citations\nworks sited\nreferences", true, false);
        const wchar_t text[] = L"Hi.\n\nHello there.\n\nWorks Sited\n\nMy book.\n\nAnother book.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 4);
        CHECK(doc.get_valid_word_count() == 7);
        CHECK(doc.get_valid_paragraph_count() == 4);
        CHECK(doc.get_words()[0].is_valid());
        CHECK(doc.get_words()[1].is_valid());
        CHECK(doc.get_words()[2].is_valid());
        CHECK(doc.get_words()[3].is_valid() == false);
        CHECK(doc.get_words()[4].is_valid() == false);
        CHECK(doc.get_words()[5].is_valid());
        CHECK(doc.get_words()[6].is_valid());
        CHECK(doc.get_words()[7].is_valid());
        CHECK(doc.get_words()[8].is_valid());
        }
    }

TEST_CASE("Document citations 2", "[document]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("Citations Exclude Aggressive")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        citationPMap.load_phrases(L"citations\nworks sited\nreferences", true, false);
        const wchar_t text[] = L"Hi.\n\nHello there.\n\nWorks Sited\n\nMy book.\n\nAnother book.";
        doc.set_aggressive_exclusion(true);
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 2);
        CHECK(doc.get_valid_word_count() == 3);
        CHECK(doc.get_valid_paragraph_count() == 2);
        CHECK(doc.get_words()[0].is_valid());
        CHECK(doc.get_words()[1].is_valid());
        CHECK(doc.get_words()[2].is_valid());
        CHECK(doc.get_words()[3].is_valid() == false);
        CHECK(doc.get_words()[4].is_valid() == false);
        CHECK(doc.get_words()[5].is_valid() == false);
        CHECK(doc.get_words()[6].is_valid() == false);
        CHECK(doc.get_words()[7].is_valid() == false);
        CHECK(doc.get_words()[8].is_valid() == false);
        }
    SECTION("Citations Exclude With Long Citation")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        citationPMap.load_phrases(L"citations\nworks sited\nreferences", true, false);
        const wchar_t text[] = L"Title\n\nThis is the body of the document and its long.\n\nMy "
                               L"citations:\n\nMy really long citation with many words";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_valid_word_count() == 10);
        CHECK(doc.get_valid_paragraph_count() == 1);
        CHECK(doc.get_words()[0].is_valid() == false);
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
        CHECK(doc.get_words()[11].is_valid() == false);
        CHECK(doc.get_words()[12].is_valid() == false);
        CHECK(doc.get_words()[13].is_valid() == false);
        CHECK(doc.get_words()[14].is_valid() == false);
        CHECK(doc.get_words()[15].is_valid() == false);
        CHECK(doc.get_words()[16].is_valid() == false);
        CHECK(doc.get_words()[17].is_valid() == false);
        CHECK(doc.get_words()[18].is_valid() == false);
        CHECK(doc.get_words()[19].is_valid() == false);
        }
    SECTION("Citations Exclude Skip Trailing Junk")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        citationPMap.load_phrases(L"citations\nworks sited\nreferences", true, false);
        const wchar_t text[] = L"Title\n\nThis is the body of the document and its long.\n\nMy "
                               L"citations:\n\nMy citation.\n\nCopyright 2008";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_valid_word_count() == 10);
        CHECK(doc.get_valid_paragraph_count() == 1);
        CHECK(doc.get_words()[0].is_valid() == false);
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
        CHECK(doc.get_words()[11].is_valid() == false);
        CHECK(doc.get_words()[12].is_valid() == false);
        CHECK(doc.get_words()[13].is_valid() == false);
        CHECK(doc.get_words()[14].is_valid() == false);
        CHECK(doc.get_words()[15].is_valid() == false);
        CHECK(doc.get_words()[16].is_valid() == false);
        }
    SECTION("Citations Include")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        citationPMap.load_phrases(L"citations\nworks sited\nreferences", true, false);
        const wchar_t text[] = L"Title\n\nThis is the body of the document and its long.\n\nWorks "
                               L"Sited\n\nMy book.\n\nAnother book.";
        doc.ignore_citation_sections(false);
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 3);
        CHECK(doc.get_valid_word_count() == 14);
        CHECK(doc.get_valid_paragraph_count() == 3);
        CHECK(doc.get_words()[0].is_valid() == false);
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
        CHECK(doc.get_words()[11].is_valid() == false);
        CHECK(doc.get_words()[12].is_valid() == false);
        CHECK(doc.get_words()[13].is_valid());
        CHECK(doc.get_words()[14].is_valid());
        CHECK(doc.get_words()[15].is_valid());
        CHECK(doc.get_words()[16].is_valid());
        }
    }

TEST_CASE("Document tagged exclude", "[document]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("Tagged Block Exclude Same Tags")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.add_exclusion_block_tags(L'^', L'^');
        const wchar_t text[] = L"Title Here.\n\n^This is the body of the document and it's "
                               L"long.\n\nWorks Sited in My book.^\n\nAnother book.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 2);
        CHECK(doc.get_valid_word_count() == 4);
        CHECK(doc.get_valid_paragraph_count() == 2);
        }
    SECTION("Tagged Block Exclude Different Tags")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.add_exclusion_block_tags(L'^', L']');
        const wchar_t text[] = L"Title Here.\n\n^This is the body of the document and it's "
                               L"long.\n\nWorks Sited in My book.]\n\nAnother book.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 2);
        CHECK(doc.get_valid_word_count() == 4);
        CHECK(doc.get_valid_paragraph_count() == 2);
        }
    SECTION("Tagged Block Exclude Multiple Tags")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.add_exclusion_block_tags(L'^', L'^');
        doc.add_exclusion_block_tags(L'<', L'>');
        const wchar_t text[] = L"Title Here.\n\n^This is the body of the document and it's "
                               L"long.^\n\n<Works Sited in My book.>\n\nAnother book.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 2);
        CHECK(doc.get_valid_word_count() == 4);
        CHECK(doc.get_valid_paragraph_count() == 2);
        }
    SECTION("Tagged Block Exclude Multiple Tags Overlap")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.add_exclusion_block_tags(L'^', L'^');
        doc.add_exclusion_block_tags(L'<', L'>');
        const wchar_t text[] = L"Title Here.\n\n^This is the body <of the document and it's "
                               L"long.^\n\nWorks Sited in My book.>\n\nAnother book.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 2);
        CHECK(doc.get_valid_word_count() == 4);
        CHECK(doc.get_valid_paragraph_count() == 2);
        }
    SECTION("Tagged Block Exclude Same Tags Missing End Tag")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.add_exclusion_block_tags(L'^', L'^');
        const wchar_t text[] = L"Title Here.\n\n^This is the body of the document and it's "
                               L"long.\n\nWorks Sited in My book.\n\nAnother book.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 4);
        CHECK(doc.get_valid_word_count() == 19);
        CHECK(doc.get_valid_paragraph_count() == 4);
        }
    SECTION("Tagged Block Exclude Same Tags No Words Between Them")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.add_exclusion_block_tags(L'^', L'^');
        const wchar_t text[] = L"Title Here.\n\n^^This is the^ ^body of the document and it's "
                               L"long.\n\nWorks Sited in My book.\n\nAnother book.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 4);
        CHECK(doc.get_valid_word_count() == 19);
        CHECK(doc.get_valid_paragraph_count() == 4);
        }
    }

TEST_CASE("Document tagged exclude 2", "[document]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("Tagged Block Exclude Same Tags Mid Sentence")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.add_exclusion_block_tags(L'^', L'^');
        const wchar_t text[] = L"Title Here.\n\nThis ^is the body of the document and it's "
                               L"long.\n\nWorks Sited^ in My book.\n\nAnother book.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 4);
        CHECK(doc.get_valid_word_count() == 8);
        CHECK(doc.get_valid_paragraph_count() == 4);
        }
    SECTION("Tagged Block Exclude Same Tags Single Words")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.add_exclusion_block_tags(L'^', L'^');
        const wchar_t text[] = L"Title Here.\n\nThis ^is^ the body of the document and it's "
                               L"long.\n\nWorks ^Sited^ in My book.\n\nAnother book.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 4);
        CHECK(doc.get_valid_word_count() == 17);
        CHECK(doc.get_valid_paragraph_count() == 4);
        }
    SECTION("Tagged Block Exclude Same Tags Multiple Words")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.add_exclusion_block_tags(L'^', L'^');
        const wchar_t text[] = L"Title Here.\n\nThis ^is the^ body of the document and it's "
                               L"long.\n\nWorks ^Sited in^ My book.\n\nAnother book.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 4);
        CHECK(doc.get_valid_word_count() == 15);
        CHECK(doc.get_valid_paragraph_count() == 4);
        }
    SECTION("Tagged Block Exclude Same Tags At End Of Document")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.add_exclusion_block_tags(L'^', L'^');
        const wchar_t text[] = L"Title Here.\n\nThis is the body of the document and it's "
                               L"long.\n\nWorks Sited in My book.\n\nAnother ^book^.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 4);
        CHECK(doc.get_valid_word_count() == 18);
        CHECK(doc.get_valid_paragraph_count() == 4);
        }
    SECTION("Tagged Block Exclude Same Tags At Start Of Document")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.add_exclusion_block_tags(L'^', L'^');
        const wchar_t text[] = L"^Title^ Here.\n\nThis is the body of the document and it's "
                               L"long.\n\nWorks Sited in My book.\n\nAnother book.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 4);
        CHECK(doc.get_valid_word_count() == 18);
        CHECK(doc.get_valid_paragraph_count() == 4);
        }
    }

TEST_CASE("Document misspellings", "[document]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("Misspelling And Double Word")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        Known_spellings.load_words(L"that is a", true, false);
        const wchar_t text[] = L"That is a a armilllo.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 1);
        CHECK(doc.get_misspelled_words().at(0) == 4);
        CHECK(doc.get_duplicate_word_indices().size() == 1);
        CHECK(doc.get_duplicate_word_indices().at(0) == 3);
        }
    SECTION("Misspelling And Double Word With Punctuation")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        Known_spellings.load_words(L"captain", true, false);
        const wchar_t text[] = L"Ay, ay captain.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 2);
        CHECK(doc.get_misspelled_words().at(0) == 0);
        CHECK(doc.get_misspelled_words().at(1) == 1);
        CHECK(doc.get_duplicate_word_indices().size() == 0); // the comma negates that
        }
    SECTION("Misspelling And Article Error")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        Known_spellings.load_words(L"that is a", true, false);
        const wchar_t text[] = L"That is a armilllo.";
        doc.set_mismatched_article_function(&is_english_mismatched_article);
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 1);
        CHECK(doc.get_misspelled_words().at(0) == 3);
        CHECK(doc.get_incorrect_article_indices().size() == 1);
        CHECK(doc.get_incorrect_article_indices().at(0) == 2);
        }
    SECTION("Misspellings")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        Known_spellings.load_words(L"the a and hat in cat", true, false);
        const wchar_t text[] = L"The catz in the hat. The cat hadd a hat.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 2);
        CHECK(doc.get_misspelled_words().at(0) == 1);
        CHECK(doc.get_misspelled_words().at(1) == 7);
        }
    SECTION("Misspellings End Of Sentence")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        Known_spellings.load_words(L"the a and hat had in cat", true, false);
        const wchar_t text[] = L"The cat in the hatz. The cat had a hatf.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 2);
        CHECK(doc.get_misspelled_words().at(0) == 4);
        CHECK(doc.get_misspelled_words().at(1) == 9);
        }
    SECTION("Misspellings Sorting")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        Known_spellings.load_words(L"", false, false);
        CHECK(Known_spellings.is_sorted()); // should be sorted since it is empty
        Known_spellings.load_words(L"a and cat hat the in", false, false);
        CHECK(Known_spellings.is_sorted() == false); // should be out of order
        Known_spellings.sort();
        CHECK(Known_spellings.is_sorted());
        const wchar_t text[] = L"The catz in the hat. The cat hadd a hat.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 2);
        CHECK(doc.get_misspelled_words().at(0) == 1);
        CHECK(doc.get_misspelled_words().at(1) == 7);
        }
    SECTION("Misspellings Custom List")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        Known_spellings.load_words(L"the a and hat in cat", true, false);
        Secondary_known_spellings.clear();
        const wchar_t text[] = L"The catz in the hat. The cat hadd a hat.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 2);
        CHECK(doc.get_misspelled_words().at(0) == 1);
        CHECK(doc.get_misspelled_words().at(1) == 7);
        // add the misspellings to the custom list of correct spellings
        Secondary_known_spellings.load_words(L"catz hadd", true, false);
        // word list changed
        doc.get_spell_checker().invalidate_cache();
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 0);
        Secondary_known_spellings.clear();
        }
    SECTION("Misspellings Custom List With Dashes")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        Known_spellings.load_words(L"the a and hat in cat", true, false);
        Secondary_known_spellings.clear();
        const wchar_t text[] = L"The catz-in-the-hat. The-cat hadd a hat.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 2);
        CHECK(doc.get_misspelled_words().at(0) == 1);
        CHECK(doc.get_misspelled_words().at(1) == 3);
        // add the misspellings to the custom list of correct spellings
        Secondary_known_spellings.load_words(L"catz hadd", true, false);
        // word list changed
        doc.get_spell_checker().invalidate_cache();
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 0);
        Secondary_known_spellings.clear();
        }
    SECTION("Misspellings Hyphenated")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        Known_spellings.load_words(L"the a and hat in cat", true, false);
        const wchar_t text[] = L"The catz in the hat. The cat-in-the-hat.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 1);
        CHECK(doc.get_misspelled_words().at(0) == 1);
        }
    }

TEST_CASE("Document misspellings 2", "[document]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("Misspellings Numerals")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        Known_spellings.load_words(L"the a and hat had in cats", true, false);
        const wchar_t text[] = L"The 5 cats in the hat had $3.50.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 0);
        }
    SECTION("Misspellings Numerals Mixed Words")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        Known_spellings.load_words(L"the tailed cat", true, false);
        const wchar_t text[] = L"The 200-tailed cat.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 0);
        }
    SECTION("Misspellings Acronym")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.get_spell_checker().ignore_proper_nouns(false);
        doc.get_spell_checker().ignore_uppercased(true);
        Known_spellings.load_words(L"the a and hat had in cat", true, false);
        const wchar_t text[] = L"The cat had a UNIVAC hat.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 0);
        }
    SECTION("Misspellings Acronym In Header")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.get_spell_checker().ignore_proper_nouns(false);
        doc.get_spell_checker().ignore_uppercased(true);
        Known_spellings.load_words(L"report from the office", true, false);
        const wchar_t text[] = L"Report from the WCC.\n\nWCC";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 0);
        }
    SECTION("Misspellings Proper Noun")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.get_spell_checker().ignore_proper_nouns(true);
        Known_spellings.load_words(L"the a and hat had in cat", true, false);
        const wchar_t text[] = L"The cat had a Univac hat.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 0);
        }
    SECTION("Misspellings Initials")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.get_spell_checker().ignore_proper_nouns(true);
        Known_spellings.load_words(L"is that", true, false);
        const wchar_t text[] = L"Is that P.T. Barnum?";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 0);
        doc.get_spell_checker().ignore_proper_nouns(false);
        doc.get_spell_checker().ignore_uppercased(false);
        doc.load_document(text, wcslen(text), false, false, false, false);
        // the real test--only Barnum should be unknown.
        // "P.T." isn't a known word, but we ignore initials.
        CHECK(doc.get_misspelled_words().size() == 1);
        CHECK(doc.get_misspelled_words().at(0) == 3);
        }
    }

TEST_CASE("Document misspellings 3", "[document]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("Misspellings Initials In Header")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        Known_spellings.load_words(L"is that yes it", true, false);
        const wchar_t text[] = L"Is that P.T.\n\nYes, it is.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        //"P.T." isn't a known word, but we ignore initials.
        CHECK(doc.get_misspelled_words().size() == 0);
        }
    SECTION("Misspellings Exclamatory")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.get_spell_checker().ignore_proper_nouns(true);
        doc.get_spell_checker().ignore_uppercased(true);
        Known_spellings.load_words(L"the a and hat had in cat", true, false);
        const wchar_t text[] = L"The CAT HAD A FETE HAT.";
        // FETE is a misspelling, but should be ignored.
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 0);
        // now see FETE as misspelling
        doc.get_spell_checker().ignore_uppercased(false);
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 1);
        CHECK(doc.get_misspelled_words().at(0) == 4);
        }
    SECTION("Misspellings Domains")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.get_spell_checker().ignore_file_addresses(true);
        Known_spellings.load_words(L"the a and hat had is cat site nice", true, false);
        const wchar_t text[] = L"The www.catinthehat.com site is nice.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 0);
        }
    SECTION("Misspellings Programmer Code")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.get_spell_checker().ignore_programmer_code(false);
        Known_spellings.load_words(L"the a and hat had is cat site nice", true, false);
        const wchar_t text[] = L"RetVal = ExcelSPreadsheet.Open() + Mypath + aD + Ia";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 5);

        doc.get_spell_checker().ignore_programmer_code(true);
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 3 /*Mypath and aD and Ia at the end*/);
        }
    SECTION("Misspellings Programmer Code With Periods")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.get_spell_checker().ignore_programmer_code(false);
        Known_spellings.load_words(L"the a and hat had is cat site nice", true, false);
        const wchar_t text[] = L"RetVal = Excel.Open() + Mypat1.A + aD + Ia";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 5);

        doc.get_spell_checker().ignore_programmer_code(true);
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 2 /*aD and Ia at the end*/);
        }
    }

TEST_CASE("Document misspellings 4", "[document]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("Wordy Phrases And Misspelling")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        pmap.load_phrases(L"results in\tresults", true, false);
        const wchar_t text[] = L"That results in problesms.";
        Known_spellings.load_words(L"That results in problems", true, false);
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_known_phrase_indices().size() == 1);
        CHECK(doc.get_known_phrase_indices()[0].first == 1);
        CHECK(doc.get_misspelled_words().size() == 1);
        CHECK(doc.get_misspelled_words().at(0) == 3);
        }

    SECTION("Misspellings Compound Word With Acronym")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        Known_spellings.load_words(L"certified", true, false);
        const wchar_t text[] = L"LEED LEED-certified.";
        doc.get_spell_checker().ignore_uppercased(true);
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 0);
        doc.get_spell_checker().ignore_uppercased(false);
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 2);
        }

    SECTION("Misspellings Programmer Code Weird Uppercasing")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.get_spell_checker().ignore_programmer_code(false);
        Known_spellings.load_words(L"the a and hat had is cat site nice", true, false);
        const wchar_t text[] = L"RetVal = ExcelSPreadsheet.Open() + PPValue + aD + Ia";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 5);

        doc.get_spell_checker().ignore_programmer_code(true);
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 2 /*aD and Ia at the end*/);
        }
    SECTION("Misspellings Programmer Code Ends In Number")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.get_spell_checker().ignore_programmer_code(false);
        Known_spellings.load_words(L"the a and hat had is cat site nice", true, false);
        const wchar_t text[] = L"RetVal = Excel.Open() + Myvalue1";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 3);

        doc.get_spell_checker().ignore_programmer_code(true);
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(0 == doc.get_misspelled_words().size());
        }
    SECTION("Misspellings Programmer Code Starts With Programmer Symbol")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.get_spell_checker().ignore_programmer_code(false);
        doc.get_spell_checker().ignore_social_media_tags(false);
        Known_spellings.load_words(L"file include val", true, false);
        const wchar_t text[] = L"#include <file>\n\n$val = 5\nPP_val = 7";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(3 == doc.get_misspelled_words().size());

        doc.get_spell_checker().ignore_programmer_code(true);
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(0 == doc.get_misspelled_words().size());
        }
    SECTION("Misspellings Time With No Space Before Meridiem")
        {
        // "4:00p.m." should not be flagged as a misspelling
        // The lack of space between '0' and 'p' should not cause "p.m." to be seen as misspelled
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        Known_spellings.load_words(L"the meeting is at", true, false);
        const wchar_t text[] = L"The meeting is at 4:00p.m.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 0);
        }
    SECTION("Misspellings Time With Meridiem Valid Cases")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        Known_spellings.load_words(L"at", true, false);

        // Single digit hour with p.m.
        const wchar_t text1[] = L"at 4:00p.m.";
        doc.load_document(text1, wcslen(text1), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 0);

        // Single digit hour with a.m.
        const wchar_t text2[] = L"at 5:30a.m.";
        doc.load_document(text2, wcslen(text2), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 0);

        // Double digit hour with p.m.
        const wchar_t text3[] = L"at 12:00p.m.";
        doc.load_document(text3, wcslen(text3), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 0);

        // Double digit hour with a.m.
        const wchar_t text4[] = L"at 10:45a.m.";
        doc.load_document(text4, wcslen(text4), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 0);

        // Uppercase P.M.
        const wchar_t text5[] = L"at 4:00P.M.";
        doc.load_document(text5, wcslen(text5), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 0);

        // Uppercase A.M.
        const wchar_t text6[] = L"at 9:15A.M.";
        doc.load_document(text6, wcslen(text6), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 0);

        // Mixed case p.M.
        const wchar_t text7[] = L"at 3:30p.M.";
        doc.load_document(text7, wcslen(text7), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 0);

        // Mixed case P.m.
        const wchar_t text8[] = L"at 6:00P.m.";
        doc.load_document(text8, wcslen(text8), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 0);

        // Leading zero on hour (09:00a.m.)
        const wchar_t text9[] = L"at 09:00a.m.";
        doc.load_document(text9, wcslen(text9), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 0);

        // Boundary: 1:00a.m. (smallest single digit)
        const wchar_t text10[] = L"at 1:00a.m.";
        doc.load_document(text10, wcslen(text10), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 0);

        // Boundary: 9:59p.m. (largest single digit hour, largest minutes)
        const wchar_t text11[] = L"at 9:59p.m.";
        doc.load_document(text11, wcslen(text11), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 0);

        // Multiple times in one sentence
        const wchar_t text12[] = L"at 9:00a.m. at 5:00p.m.";
        doc.load_document(text12, wcslen(text12), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 0);
        }
    }

TEST_CASE("Document articles", "[document]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("Mismatched Article Separated By Punctuation")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.set_mismatched_article_function(&is_english_mismatched_article);
        const wchar_t* text = L"a) experiment";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_incorrect_article_indices().size() == 0);

        text = L"a, experiment";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_incorrect_article_indices().size() == 0);

        text = L"a: experiment";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_incorrect_article_indices().size() == 0);

        text = L"a. Experiment";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_incorrect_article_indices().size() == 0);
        }
    SECTION("Mismatched Article Followed By Punctuation")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.set_mismatched_article_function(&is_english_mismatched_article);
        const wchar_t* text = L"a experiment'";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_incorrect_article_indices().size() == 1);

        text = L"a experiment\"";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_incorrect_article_indices().size() == 1);

        text = L"a experiment:";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_incorrect_article_indices().size() == 1);
        }
    SECTION("Mismatched Article Following Punctuation")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.set_mismatched_article_function(&is_english_mismatched_article);
        const wchar_t* text = L",a experiment";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_incorrect_article_indices().size() == 1);

        text = L":a experiment";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_incorrect_article_indices().size() == 1);

        text = L"'a experiment";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_incorrect_article_indices().size() == 1);

        text = L"\"a experiment'";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_incorrect_article_indices().size() == 1);

        text = L",a experiment'";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_incorrect_article_indices().size() == 1);
        }
    SECTION("Mismatched Article Quoted")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.set_mismatched_article_function(&is_english_mismatched_article);
        const wchar_t* text = L"a 'experiment'";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_incorrect_article_indices().size() == 1);

        text = L"a \"experiment\"";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_incorrect_article_indices().size() == 1);

        text = L"a “experiment”";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_incorrect_article_indices().size() == 1);

        text = L"a experiment\" \"here\"";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_incorrect_article_indices().size() == 1);
        }
    SECTION("Mismatched Article Itself Quoted")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.set_mismatched_article_function(&is_english_mismatched_article);
        const wchar_t* text = L"a' experiment'";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_incorrect_article_indices().size() == 0);

        text = L"a\" experiment\"";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_incorrect_article_indices().size() == 0);

        text = L"a“ experiment”";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_incorrect_article_indices().size() == 0);

        text = L"an\" experiment";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_incorrect_article_indices().size() == 0);
        }
    SECTION("Mismatched Article That Is Really Proper")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"D and A experiment.";
        doc.set_mismatched_article_function(&is_english_mismatched_article);
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_incorrect_article_indices().size() == 0);
        }
    SECTION("Mismatched Article Start Of Sentence")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"A experiment.";
        doc.set_mismatched_article_function(&is_english_mismatched_article);
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_incorrect_article_indices().size() == 1);
        }
    }

TEST_CASE("Document repeated words", "[document]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("Repeated Words")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"double double words";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_duplicate_word_indices().size() == 1);
        }
    SECTION("Repeated Words False Positive")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"He had had it";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_duplicate_word_indices().size() == 0);
        }
    SECTION("Repeated Words False Positive2")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Die die Auto";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_duplicate_word_indices().size() == 0);
        }
    SECTION("Repeated Words New Line")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"double\ndouble words";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_duplicate_word_indices().size() == 1);
        }
    SECTION("Repeated Words Separated By Punctuation")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"double, double words";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_duplicate_word_indices().size() == 0);
        }

    SECTION("Repeated Words Separated By Punctuation2")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"double--\ndouble words";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_duplicate_word_indices().size() == 0);
        }
    }

TEST_CASE("Document colloquialism", "[document]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("Colloquialism ING")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        Known_spellings.load_words(L"Me be thinking you not trying", true, false);
        doc.get_spell_checker().allow_colloquialisms(false);
        const wchar_t text[] = L"Me be thinkin' you not be tryin'. thinkin’";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_misspelled_words().size() == 3);

        doc.get_spell_checker().allow_colloquialisms(true);
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(0 == doc.get_misspelled_words().size());
        }
    SECTION("Colloquialism Er To A")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        Known_spellings.load_words(
            L"I'm going to get you player You all are players What is that name are those names",
            true, false);
        doc.get_spell_checker().allow_colloquialisms(false);
        const wchar_t text[] = L"I'm going to get you, playa! You all are playas! What is that "
                               L"playa's name? What are those playas' names?";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(4 == doc.get_misspelled_words().size());

        doc.get_spell_checker().allow_colloquialisms(true);
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(0 == doc.get_misspelled_words().size());
        }
    SECTION("Colloquialism Stretched Letter")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        Known_spellings.load_words(L"No he yelled Nope O", true, false);
        doc.get_spell_checker().allow_colloquialisms(false);
        const wchar_t* text = L"Noooooooo, he yelled.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(1 == doc.get_misspelled_words().size());

        doc.get_spell_checker().allow_colloquialisms(true);
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(0 == doc.get_misspelled_words().size());

        text = L"Oooooooo, he yelled. O."; // this should be stripped down to "o"
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(0 == doc.get_misspelled_words().size());

        text = L"Nooope, he yelled."; // Same as "nope"
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(0 == doc.get_misspelled_words().size());

        text = L"Noope, he yelled."; // 3+ repeat letters needed to activate this feature
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(1 == doc.get_misspelled_words().size());
        }
    SECTION("Colloquialism ING2")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        Known_spellings.load_words(
            L"He's been taking that test for five hours He ain't never going to finish", true,
            false);
        doc.get_spell_checker().allow_colloquialisms(false);
        const wchar_t text[] =
            L"He's been takin' that test for five hours. He ain't never goin' to finish.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(2 == doc.get_misspelled_words().size());

        doc.get_spell_checker().allow_colloquialisms(true);
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(0 == doc.get_misspelled_words().size());
        }
    }

TEST_CASE("Document overly used words", "[document]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("Overly Used Words By Sentence Sentence Too Long")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Hello. The robot entered the room, as the other robots looked on.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(0 == doc.get_overused_words_by_sentence().size());
        }
    SECTION("Overly Used Words By Sentence")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Hello. The robot entered the room, as other robots looked on.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(1 == doc.get_overused_words_by_sentence().size());
        CHECK(1 == doc.get_overused_words_by_sentence()[0].first); // second sentence
        auto firstWord = doc.get_overused_words_by_sentence()[0].second.cbegin();
        CHECK(2 == *firstWord);     // third word in collection
        CHECK(8 == *(++firstWord)); // eighth word in collection
        }
    SECTION("Overly Used Words By Sentence Double Word")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"'HO HO HO,' said Santa.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(0 == doc.get_overused_words_by_sentence().size());
        }
    }

TEST_CASE("Document lowercased sentences", "[document]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("Lowercased Sentences")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t text[] =
            L"I am Mr. Smithe.  and my name is C. Blake Smithe.  but I usually go by Blake.  Or "
            L"the Blakeman. Mr. blake is OK too. Seriously, call me Blake. 7 is my favorite "
            L"number.  seven is another way to spell that";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_lowercase_beginning_sentences().size() == 3);
        CHECK(doc.get_lowercase_beginning_sentences()[0] == 1);
        CHECK(doc.get_lowercase_beginning_sentences()[1] == 2);
        CHECK(doc.get_lowercase_beginning_sentences()[2] == 7);
        }
    SECTION("Lowercased Sentences Letter Bullet Point")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t text[] = L"a. Here is a long bullet point that begins with a letter that "
                               L"should be seen as lower case. Another sentence.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_lowercase_beginning_sentences().size() == 0);
        }
    SECTION("Lowercased Sentences List Items OK To Be Lowercased")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t text[] =
            L"the Header\n\nHere is some text.\n\n\titem 1\n\titem 2\n\titem 3\n\nMore text.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_lowercase_beginning_sentences().size() == 1);
        CHECK(doc.get_lowercase_beginning_sentences()[0] == 0);
        }
    SECTION("Lowercased Sentences Starts With Number")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t text[] = L"1-Power is a word that is sort of numeric for our purposes. Here "
                               L"is another sentence.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_lowercase_beginning_sentences().size() == 0);
        }
    SECTION("Lowercased Sentences Starts With Letter DashWord")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t text[] = L"t-Test is not meant to be capitalized. Here is another sentence.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_lowercase_beginning_sentences().size() == 0);
        }
    SECTION("Lowercased Sentences Equation")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t text[] = L"abc = 5+2\n\nHere is a real sentence.\n\nz = 9;";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_lowercase_beginning_sentences().size() == 0);
        }
    SECTION("Lowercased Sentences One Letter")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t text[] = L"a is not meant to be capitalized. Here is another sentence.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_lowercase_beginning_sentences().size() == 1);
        }
    SECTION("Lowercased Sentences Start With Punctuation")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t text[] = L"# of iterations. Here is another sentence.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_lowercase_beginning_sentences().size() == 0);
        }
    }

TEST_CASE("Document wordy phrases", "[document]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("Wordy Phrases At End Of Sentence")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        pmap.load_phrases(L"difficult\thard\ndifficult to grasp\thard to learn", true, false);
        const wchar_t text[] = L"That is difficult. It is difficult to grasp.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_known_phrase_indices().size() == 2);
        CHECK(doc.get_known_phrase_indices()[0].first == 2);
        CHECK(doc.get_known_phrase_indices()[0].second == 0);
        CHECK(doc.get_known_phrase_indices()[1].first == 5);
        CHECK(doc.get_known_phrase_indices()[1].second == 1);
        }
    SECTION("Wordy Phrases At End Of Sentence2")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        pmap.load_phrases(L"difficult\thard\ndifficult to grasp\thard to learn", true, false);
        const wchar_t text[] = L"That is difficult.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_known_phrase_indices().size() == 1);
        CHECK(doc.get_known_phrase_indices()[0].first == 2);
        CHECK(doc.get_known_phrase_indices()[0].second == 0);
        }
    SECTION("Wordy Phrases One Word")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        pmap.load_phrases(L"difficult\thard\nDifficult to grasp\thard to learn", true, false);
        const wchar_t text[] = L"That is difficult and complex. It is difficult to grasp.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_known_phrase_indices().size() == 2);
        CHECK(doc.get_known_phrase_indices()[0].first == 2);
        CHECK(doc.get_known_phrase_indices()[0].second == 0);
        CHECK(doc.get_known_phrase_indices()[1].first == 7);
        CHECK(doc.get_known_phrase_indices()[1].second == 1);
        }
    SECTION("Wordy Phrases One Word With Punctuation")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        pmap.load_phrases(L"difficult\thard\ndifficult to grasp\thard to learn", true, false);
        const wchar_t text[] = L"That is difficult, and complex. It is difficult to grasp.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_known_phrase_indices().size() == 2);
        CHECK(doc.get_known_phrase_indices()[0].first == 2);
        CHECK(doc.get_known_phrase_indices()[0].second == 0);
        CHECK(doc.get_known_phrase_indices()[1].first == 7);
        CHECK(doc.get_known_phrase_indices()[1].second == 1);
        }
    SECTION("Wordy Phrases")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        pmap.load_phrases(L"results in\tresults", true, false);
        const wchar_t text[] =
            L"That results in problems.  The results, in the window, are bad.  This results in bad "
            L"results. The results, in the window, are bad.  The results? In the window.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_known_phrase_indices().size() == 2);
        CHECK(doc.get_known_phrase_indices()[0].first == 1);
        CHECK(doc.get_known_phrase_indices()[1].first == 12);
        }
    SECTION("Wordy Phrases2")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        pmap.load_phrases(L"results in\tresults\nfell down\tfell", true, false);
        const wchar_t text[] =
            L"That results in problems.  The results, in the window, are bad.  This results in bad "
            L"results. The results, in the window, are bad.  I fell down.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_known_phrase_indices().size() == 3);
        CHECK(doc.get_known_phrase_indices()[0].first == 1);
        CHECK(doc.get_known_phrase_indices()[0].second == 1);
        CHECK(doc.get_known_phrase_indices()[1].first == 12);
        CHECK(doc.get_known_phrase_indices()[1].second == 1);
        CHECK(doc.get_known_phrase_indices()[2].first == 24);
        CHECK(doc.get_known_phrase_indices()[2].second == 0);
        CHECK(pmap.get_phrases()[0].first.get_words()[0] == L"fell");
        CHECK(pmap.get_phrases()[0].first.get_words()[1] == L"down");
        CHECK(pmap.get_phrases()[0].second == L"fell");
        CHECK(pmap.get_phrases()[1].first.get_words()[0] == L"results");
        CHECK(pmap.get_phrases()[1].first.get_words()[1] == L"in");
        CHECK(pmap.get_phrases()[1].second == L"results");
        }
    SECTION("Error Phrases")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        pmap.load_phrases(
            L"results in\tresults\t0\nfell down\tfell\t0\nFrench benefit\tfringe benefit\t3", true,
            false);
        const wchar_t text[] = L"That's a nice French benefit";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_known_phrase_indices().size() == 1);
        CHECK(doc.get_known_phrase_indices()[0].first == 3);
        CHECK(doc.get_known_phrase_indices()[0].second == 1);
        CHECK(pmap.get_phrases()[1].first.get_words()[0] == L"French");
        CHECK(pmap.get_phrases()[1].first.get_words()[1] == L"benefit");
        CHECK(pmap.get_phrases()[1].second == L"fringe benefit");
        CHECK(pmap.get_phrases()[1].first.get_type() == phrase_type::phrase_error);
        }
    SECTION("Wordy Phrases Overlapping Phrases")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        pmap.load_phrases(L"results in that\tresulting\nresults in\tresults", true, false);
        const wchar_t text[] = L"That results in problems.  That results in that problems.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_known_phrase_indices().size() == 2);
        CHECK(doc.get_known_phrase_indices()[0].first == 1);
        CHECK(doc.get_known_phrase_indices()[1].first == 5);
        CHECK(doc.get_known_phrase_indices()[0].second == 0);
        CHECK(doc.get_known_phrase_indices()[1].second == 1);
        CHECK(pmap.get_phrases()[0].first.get_words()[0] == L"results");
        CHECK(pmap.get_phrases()[0].first.get_words()[1] == L"in");
        CHECK(pmap.get_phrases()[0].second == L"results");
        CHECK(pmap.get_phrases()[1].first.get_words()[0] == L"results");
        CHECK(pmap.get_phrases()[1].first.get_words()[1] == L"in");
        CHECK(pmap.get_phrases()[1].first.get_words()[2] == L"that");
        CHECK(pmap.get_phrases()[1].second == L"resulting");
        }
    SECTION("Wordy Phrases Overlapping Phrases2")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        pmap.load_phrases(L"results in\tresults\nresults in that\tresulting", true, false);
        const wchar_t text[] = L"That results in problems.  That results in that problems.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_known_phrase_indices().size() == 2);
        CHECK(doc.get_known_phrase_indices()[0].first == 1);
        CHECK(doc.get_known_phrase_indices()[1].first == 5);
        CHECK(doc.get_known_phrase_indices()[0].second == 0);
        CHECK(doc.get_known_phrase_indices()[1].second == 1);
        CHECK(pmap.get_phrases()[0].first.get_words()[0] == L"results");
        CHECK(pmap.get_phrases()[0].first.get_words()[1] == L"in");
        CHECK(pmap.get_phrases()[0].second == L"results");
        CHECK(pmap.get_phrases()[1].first.get_words()[0] == L"results");
        CHECK(pmap.get_phrases()[1].first.get_words()[1] == L"in");
        CHECK(pmap.get_phrases()[1].first.get_words()[2] == L"that");
        CHECK(pmap.get_phrases()[1].second == L"resulting");
        }
    }

TEST_CASE("Document long incomplete", "[document]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("Long Incomplete Sentences Will Not Count")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter 1\n\nIt was a dark and stormy night\n\nThe End";
        doc.load_document(text, wcslen(text), false, false, false, false);
        // Chapter 1
        CHECK(doc.get_words().at(0).is_valid() == false);
        CHECK(doc.get_words().at(1).is_valid() == false);
        // It was a dark and stormy night
        CHECK(doc.get_words().at(2).is_valid() == false);
        CHECK(doc.get_words().at(3).is_valid() == false);
        CHECK(doc.get_words().at(4).is_valid() == false);
        CHECK(doc.get_words().at(5).is_valid() == false);
        CHECK(doc.get_words().at(6).is_valid() == false);
        CHECK(doc.get_words().at(7).is_valid() == false);
        CHECK(doc.get_words().at(8).is_valid() == false);
        // The End
        CHECK(doc.get_words().at(9).is_valid() == false);
        CHECK(doc.get_words().at(10).is_valid() == false);

        CHECK(doc.get_paragraph_count() == 3);
        CHECK(doc.get_valid_paragraph_count() == 0);
        CHECK(doc.get_sentence_count() == 3);
        CHECK(doc.get_complete_sentence_count() == 0);
        CHECK(doc.get_word_count() == 11);
        CHECK(doc.get_valid_word_count() == 0);
        }
    SECTION("Long Incomplete Sentences")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.set_allowable_incomplete_sentence_size(
            6); // middle (incomplete) sentence should count now
        const wchar_t* text = L"Chapter 1\n\nIt was a dark and stormy night\n\nThe End";
        doc.load_document(text, wcslen(text), false, false, false, false);
        // Chapter 1
        CHECK(doc.get_words().at(0).is_valid() == false);
        CHECK(doc.get_words().at(1).is_valid() == false);
        // It was a dark and stormy night
        CHECK(doc.get_words().at(2).is_valid() == true);
        CHECK(doc.get_words().at(3).is_valid() == true);
        CHECK(doc.get_words().at(4).is_valid() == true);
        CHECK(doc.get_words().at(5).is_valid() == true);
        CHECK(doc.get_words().at(6).is_valid() == true);
        CHECK(doc.get_words().at(7).is_valid() == true);
        CHECK(doc.get_words().at(8).is_valid() == true);
        // The End
        CHECK(doc.get_words().at(9).is_valid() == false);
        CHECK(doc.get_words().at(10).is_valid() == false);

        CHECK(doc.get_paragraph_count() == 3);
        CHECK(doc.get_valid_paragraph_count() == 1);
        CHECK(doc.get_sentence_count() == 3);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_word_count() == 11);
        CHECK(doc.get_valid_word_count() == 7);
        }
    SECTION("Long Incomplete Sentences Equation")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        // a mathematical function should not be seen as a valid sentence, even if it has a long
        // length
        doc.set_allowable_incomplete_sentence_size(6);
        const wchar_t* text = L"f(x) = G[( n+ 1)/ 2] /G( n/2) *(n* p)- 1/2 *[1 +(x2/n)-( n+ 1)/ 2";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_words().at(0).is_valid() == false);
        CHECK(doc.get_words().at(1).is_valid() == false);
        CHECK(doc.get_words().at(2).is_valid() == false);
        CHECK(doc.get_words().at(3).is_valid() == false);
        CHECK(doc.get_words().at(4).is_valid() == false);
        CHECK(doc.get_words().at(5).is_valid() == false);
        CHECK(doc.get_words().at(6).is_valid() == false);
        CHECK(doc.get_words().at(7).is_valid() == false);
        CHECK(doc.get_words().at(8).is_valid() == false);
        CHECK(doc.get_words().at(9).is_valid() == false);
        CHECK(doc.get_words().at(10).is_valid() == false);
        CHECK(doc.get_words().at(11).is_valid() == false);
        CHECK(doc.get_words().at(12).is_valid() == false);
        CHECK(doc.get_words().at(13).is_valid() == false);
        CHECK(doc.get_words().at(14).is_valid() == false);
        CHECK(doc.get_words().at(15).is_valid() == false);

        CHECK(1 == doc.get_paragraph_count());
        CHECK(0 == doc.get_valid_paragraph_count());
        CHECK(1 == doc.get_sentence_count());
        CHECK(0 == doc.get_complete_sentence_count());
        CHECK(16 == doc.get_word_count());
        CHECK(0 == doc.get_valid_word_count());
        }
    SECTION("Long Incomplete Sentences Aggressive")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        doc.set_aggressive_exclusion(true);
        const wchar_t* text = L"Chapter 1\n\nIt was a dark and stormy night\n\nThe End";
        doc.load_document(text, wcslen(text), false, false, false, false);
        // Chapter 1
        CHECK(doc.get_words().at(0).is_valid() == false);
        CHECK(doc.get_words().at(1).is_valid() == false);
        // It was a dark and stormy night
        CHECK(doc.get_words().at(2).is_valid() == false);
        CHECK(doc.get_words().at(3).is_valid() == false);
        CHECK(doc.get_words().at(4).is_valid() == false);
        CHECK(doc.get_words().at(5).is_valid() == false);
        CHECK(doc.get_words().at(6).is_valid() == false);
        CHECK(doc.get_words().at(7).is_valid() == false);
        CHECK(doc.get_words().at(8).is_valid() == false);
        // The End
        CHECK(doc.get_words().at(9).is_valid() == false);
        CHECK(doc.get_words().at(10).is_valid() == false);

        CHECK(doc.get_paragraph_count() == 3);
        CHECK(doc.get_valid_paragraph_count() == 0);
        CHECK(doc.get_sentence_count() == 3);
        CHECK(doc.get_complete_sentence_count() == 0);
        CHECK(doc.get_word_count() == 11);
        CHECK(doc.get_valid_word_count() == 0);
        }
    SECTION("Long Incomplete Sentences At End Of Paragraph Before Header")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter 1\n\nIt was a dark and stormy night. The End\n\nA header";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_paragraph_count() == 3);
        CHECK(doc.get_valid_paragraph_count() == 1);
        CHECK(doc.get_sentence_count() == 4);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_word_count() == 13);
        CHECK(doc.get_valid_word_count() == 7);
        CHECK(doc.get_sentences()[0].get_type() == grammar::sentence_paragraph_type::header);
        CHECK(doc.get_sentences()[1].get_type() == grammar::sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[2].get_type() == grammar::sentence_paragraph_type::incomplete);
        CHECK(doc.get_sentences()[3].get_type() == grammar::sentence_paragraph_type::header);
        }
    SECTION("Long Incomplete Sentences At End Of Paragraph Before Two Headers")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text =
            L"Chapter 1\n\nIt was a dark and stormy night. The End\n\nA header\n\nA subheader";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_paragraph_count() == 4);
        CHECK(doc.get_valid_paragraph_count() == 1);
        CHECK(doc.get_sentence_count() == 5);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_word_count() == 15);
        CHECK(doc.get_valid_word_count() == 7);
        CHECK(doc.get_sentences()[0].get_type() == grammar::sentence_paragraph_type::header);
        CHECK(doc.get_sentences()[1].get_type() == grammar::sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[2].get_type() == grammar::sentence_paragraph_type::incomplete);
        CHECK(doc.get_sentences()[3].get_type() == grammar::sentence_paragraph_type::header);
        CHECK(doc.get_sentences()[4].get_type() == grammar::sentence_paragraph_type::header);
        }
    SECTION("Long Incomplete Sentences At End Of Paragraph Before List")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text =
            L"Chapter 1\n\nIt was a dark and stormy night. The End\n\nList 1\n\nList 2\n\nList 3";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_paragraph_count() == 5);
        CHECK(doc.get_valid_paragraph_count() == 1);
        CHECK(doc.get_sentence_count() == 6);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_word_count() == 17);
        CHECK(doc.get_valid_word_count() == 7);
        CHECK(doc.get_sentences()[0].get_type() == grammar::sentence_paragraph_type::header);
        CHECK(doc.get_sentences()[1].get_type() == grammar::sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[2].get_type() == grammar::sentence_paragraph_type::incomplete);
        CHECK(doc.get_sentences()[3].get_type() == grammar::sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[4].get_type() == grammar::sentence_paragraph_type::list_item);
        CHECK(doc.get_sentences()[5].get_type() == grammar::sentence_paragraph_type::list_item);
        }
    SECTION("Long Incomplete Sentences At End Of Paragraph Before Normal Paragraph")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter 1\n\nIt was a dark and stormy night. The End\n\nA regular "
                              L"paragraph. A normal sentence.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_paragraph_count() == 3);
        CHECK(doc.get_valid_paragraph_count() == 2);
        CHECK(doc.get_sentence_count() == 5);
        CHECK(doc.get_complete_sentence_count() == 3);
        CHECK(doc.get_word_count() == 17);
        CHECK(doc.get_valid_word_count() == 13);
        CHECK(doc.get_sentences()[0].get_type() == grammar::sentence_paragraph_type::header);
        CHECK(doc.get_sentences()[1].get_type() == grammar::sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[2].get_type() == grammar::sentence_paragraph_type::incomplete);
        CHECK(doc.get_sentences()[3].get_type() == grammar::sentence_paragraph_type::complete);
        CHECK(doc.get_sentences()[4].get_type() == grammar::sentence_paragraph_type::complete);
        }
    }

TEST_CASE("Document sentence unit", "[document]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("Sentence Unit")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t text[] =
            L"I—am Mr. Smithe my name is C. Blake Smithe.  I usually go by Blake: or the "
            L"Blakeman; Mr. blake is OK too. Seriously, call me Blake.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 3);
        CHECK(doc.get_sentences()[0].get_unit_count() == 2);
        CHECK(doc.get_sentences()[1].get_unit_count() == 3);
        CHECK(doc.get_sentences()[2].get_unit_count() == 1);
        }
    SECTION("Sentence Unit Consecutive Delims")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t text[] =
            L"I am Mr. Smithe--my name is C. Blake Smithe.  I usually go by Blake:; or the "
            L"Blakeman; Mr. blake is OK too. Seriously, call me Blake.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 3);
        CHECK(doc.get_sentences()[0].get_unit_count() == 2);
        CHECK(doc.get_sentences()[1].get_unit_count() == 3);
        CHECK(doc.get_sentences()[2].get_unit_count() == 1);
        }
    SECTION("Sentence Unit No Unit Delims")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t text[] =
            L"I am Mr. Smithe my name is C. Blake Smithe.  I usually go by Blake or the Blakeman "
            L"Mr. blake is OK too. Seriously, call me Blake.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 3);
        CHECK(doc.get_sentences()[0].get_unit_count() == 1);
        CHECK(doc.get_sentences()[1].get_unit_count() == 1);
        CHECK(doc.get_sentences()[2].get_unit_count() == 1);
        }
    SECTION("Sentence Unit No Unit Delims With Hyphens")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t text[] = L"I am a happy-go-lucky guy most of the time.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 1);
        CHECK(doc.get_sentences()[0].get_unit_count() == 1);
        }
    SECTION("Sentence Unit Double Hyphens As Dash")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t text[] = L"I am a happy--most of the time--guy.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 1);
        CHECK(doc.get_sentences()[0].get_unit_count() == 3);
        }
    SECTION("Sentence Unit No Sentences")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t text[] = L"   ";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 0);
        }
    }

TEST_CASE("Document", "[document]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("Citation Superscripts")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Restrictive 56 H⁷ legislation.⁶⁷ Lack of progress.⁷ Another sentence.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(3 == doc.get_sentence_count());
        CHECK(9 == doc.get_valid_word_count());
        CHECK(9 == doc.get_word_count());
        }
    SECTION("Ellipses")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Uh…Oh!";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_word_count() == 2);
        CHECK(doc.get_punctuation().size() == 1);
        CHECK(doc.get_punctuation().at(0).get_punctuation_mark() == L'…');
        }
    SECTION("Ellipses Dots")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Uh...Oh!";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_word_count() == 2);
        }
    SECTION("Empty Parentheses")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"A FunctionCall(). Another sentence.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentence_count() == 2);
        CHECK(doc.get_complete_sentence_count() == 2);
        }
    SECTION("Superscripts")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Word,² another word. E=mc² Smith² H²²O mc²";
        doc.load_document(text, wcslen(text), false, true, true, false);

        CHECK(std::wstring(L"Word") == std::wstring(doc.get_word(0).c_str()));
        CHECK(std::wstring(L"mc²") == std::wstring(doc.get_word(4).c_str()));
        CHECK(std::wstring(L"Smith") == std::wstring(doc.get_word(5).c_str()));
        CHECK(std::wstring(L"H²²O") == std::wstring(doc.get_word(6).c_str()));
        CHECK(std::wstring(L"mc²") == std::wstring(doc.get_word(7).c_str()));

        CHECK(L',' == doc.get_punctuation()[0].get_punctuation_mark());
        CHECK(1 == doc.get_punctuation()[0].get_word_position());
        CHECK(L'²' == doc.get_punctuation()[1].get_punctuation_mark());
        CHECK(1 == doc.get_punctuation()[1].get_word_position());
        CHECK(L'=' == doc.get_punctuation()[2].get_punctuation_mark());
        CHECK(4 == doc.get_punctuation()[2].get_word_position());
        CHECK(L'²' == doc.get_punctuation()[3].get_punctuation_mark());
        CHECK(6 == doc.get_punctuation()[3].get_word_position());
        CHECK(doc.get_punctuation()[0].is_connected_to_previous_word());
        CHECK(doc.get_punctuation()[1].is_connected_to_previous_word());
        CHECK(doc.get_punctuation()[2].is_connected_to_previous_word());
        CHECK(doc.get_punctuation()[3].is_connected_to_previous_word());

        text = L"Smith²";
        doc.load_document(text, wcslen(text), false, true, true, false);

        CHECK(std::wstring(L"Smith") == std::wstring(doc.get_word(0).c_str()));
        CHECK(L'²' == doc.get_punctuation()[0].get_punctuation_mark());
        }
    SECTION("Url")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"An url https://www.kff.org/health-reform/state-indicator/state-activity-around-expanding-medicaid-under-the-affordable-care-act/?currentTimeframe=0&sortModel=%7B%22colId%22:%22Location%22,%22sort%22:%22asc%22%7D#note-1";
        doc.load_document(text, wcslen(text), false, true, true, false);

        CHECK(3 == doc.get_word_count());
        CHECK(std::wstring(L"https://www.kff.org/health-reform/state-indicator/state-activity-around-expanding-medicaid-under-the-affordable-care-act/?currentTimeframe=0&sortModel=%7B%22colId%22:%22Location%22,%22sort%22:%22asc%22%7D#note-1") == std::wstring(doc.get_word(2).c_str()));
        }
    SECTION("Url2")
        {
        const auto text = L"He is an Url: http://www.unicef.org/search/search.phpen=Percentage+of+children+Aged+5-14+engaged+in+child+labour&go.x=0&go.y=0";

        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_word_count() == 5);
        CHECK(std::wstring(L"http://www.unicef.org/search/search.phpen=Percentage+of+children+Aged+5-14+engaged+in+child+labour&go.x=0&go.y=0") == std::wstring(doc.get_word(4).c_str()));
        }

    SECTION("Spaces Paran Sentence End")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"A sentence (text )? Another sentence.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_complete_sentence_count() == 2);
        CHECK(doc.get_punctuation().size() == 2);
        CHECK(doc.get_sentences().at(0).get_ending_punctuation() == L'?');
        CHECK(doc.get_sentences().at(1).get_ending_punctuation() == L'.');
        CHECK(doc.get_punctuation().at(0).get_punctuation_mark() == L'(');
        CHECK(doc.get_punctuation().at(1).get_punctuation_mark() == L')');

        text = L"A sentence (text ). Another sentence.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_complete_sentence_count() == 2);
        CHECK(doc.get_punctuation().size() == 2);
        CHECK(doc.get_sentences().at(0).get_ending_punctuation() == L'.');
        CHECK(doc.get_sentences().at(1).get_ending_punctuation() == L'.');
        CHECK(doc.get_punctuation().at(0).get_punctuation_mark() == L'(');
        CHECK(doc.get_punctuation().at(1).get_punctuation_mark() == L')');

        text = L"A sentence ([text ]). Another sentence.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_complete_sentence_count() == 2);
        CHECK(doc.get_punctuation().size() == 4);
        CHECK(doc.get_sentences().at(0).get_ending_punctuation() == L'.');
        CHECK(doc.get_sentences().at(1).get_ending_punctuation() == L'.');
        CHECK(doc.get_punctuation().at(0).get_punctuation_mark() == L'(');
        CHECK(doc.get_punctuation().at(1).get_punctuation_mark() == L'[');
        CHECK(doc.get_punctuation().at(2).get_punctuation_mark() == L']');
        CHECK(doc.get_punctuation().at(3).get_punctuation_mark() == L')');
        }
    SECTION("Paren Same Sentence")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Document ave. (right pane) blah?";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentence_count() == 1);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_punctuation().size() == 2);
        CHECK(doc.get_sentences().at(0).get_ending_punctuation() == L'?');

        text = L"Document ave. [right pane] blah?";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentence_count() == 1);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_punctuation().size() == 2);
        CHECK(doc.get_sentences().at(0).get_ending_punctuation() == L'?');
        }
    SECTION("Paren Same Sentence Spaces")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Document dr.    (right pane) blah?";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentence_count() == 1);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_punctuation().size() == 2);
        CHECK(doc.get_sentences().at(0).get_ending_punctuation() == L'?');

        text = L"Document dr.    [right pane] blah?";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentence_count() == 1);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_punctuation().size() == 2);
        CHECK(doc.get_sentences().at(0).get_ending_punctuation() == L'?');
        }
    SECTION("Paren Same Sentence Initial")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Spache G. (1955) article.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentence_count() == 1);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_punctuation().size() == 2);
        CHECK(doc.get_sentences().at(0).get_ending_punctuation() == L'.');

        text = L"Spache G. [1955] article.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentence_count() == 1);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_punctuation().size() == 2);
        CHECK(doc.get_sentences().at(0).get_ending_punctuation() == L'.');
        }
    SECTION("Paren Two Sentences")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Document list. (right pane) blah?"; // "list" is not an abbreviation
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentence_count() == 2);
        CHECK(doc.get_complete_sentence_count() == 2);
        CHECK(doc.get_punctuation().size() == 2);
        CHECK(doc.get_sentences().at(0).get_ending_punctuation() == L'.');
        CHECK(doc.get_sentences().at(1).get_ending_punctuation() == L'?');
        }
    SECTION("Paren New Sentence New Line")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Document list.\n\n(right pane) blah?";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentence_count() == 2);
        CHECK(doc.get_complete_sentence_count() == 2);
        CHECK(doc.get_punctuation().size() == 2);
        CHECK(doc.get_sentences().at(0).get_ending_punctuation() == L'.');
        CHECK(doc.get_sentences().at(1).get_ending_punctuation() == L'?');
        }
    SECTION("Paren New Sentence")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Document list. (Right pane) blah?";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentence_count() == 2);
        CHECK(doc.get_complete_sentence_count() == 2);
        CHECK(doc.get_punctuation().size() == 2);
        CHECK(doc.get_sentences().at(0).get_ending_punctuation() == L'.');
        CHECK(doc.get_sentences().at(1).get_ending_punctuation() == L'?');
        }
    SECTION("Paren Same Sentence Abbreviation")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Document ave. (Right pane)?";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentence_count() == 1);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_punctuation().size() == 2);
        CHECK(doc.get_sentences().at(0).get_ending_punctuation() == L'?');
        }
    SECTION("Paren Two Sentence Bad Abbreviation")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Document ave! (Right pane)?";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentence_count() == 2);
        CHECK(doc.get_complete_sentence_count() == 2);
        CHECK(doc.get_punctuation().size() == 2);
        CHECK(doc.get_sentences().at(0).get_ending_punctuation() == L'!');
        CHECK(doc.get_sentences().at(1).get_ending_punctuation() == L'?');
        }
    SECTION("Passive Voice")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"The book was given to him. It had been perused a lot.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_passive_voice_indices().size() == 2);
        CHECK(doc.get_passive_voice_indices().at(0).first == 2);
        CHECK(doc.get_passive_voice_indices().at(1).first == 8);
        }
    SECTION("Passive Voice Separated By Punctuation")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"The book was, given to him. It had been. Perused a lot.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_passive_voice_indices().size() == 0);
        }
    SECTION("Passive Voice End Of Sentence")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"The book was given.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_passive_voice_indices().size() == 1);
        CHECK(doc.get_passive_voice_indices().at(0).first == 2);

        text = L"The book was not given.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_passive_voice_indices().size() == 1);
        CHECK(doc.get_passive_voice_indices().at(0).first == 2);

        text = L"Was not given.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_passive_voice_indices().size() == 1);
        CHECK(doc.get_passive_voice_indices().at(0).first == 0);

        text = L"Was given.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_passive_voice_indices().size() == 1);
        CHECK(doc.get_passive_voice_indices().at(0).first == 0);
        }
    SECTION("Brace Two Sentences")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Director of Digital Engagement for the company. [email protected]"; // "company" is not an abbreviation
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_sentence_count() == 2);
        CHECK(doc.get_complete_sentence_count() == 1);
        REQUIRE(!doc.get_punctuation().empty());
        CHECK(doc.get_sentences().at(0).get_ending_punctuation() == L'.');
        }
    
    SECTION("Grammar")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        doc.set_mismatched_article_function(&is_english_mismatched_article);
        const wchar_t* text = L"Here, is is an \"double\", word, word here is OK. OK, this sentence is \"OK\" too. An new sentence, with a 'article' problems, well an, few problems. and, a casing problem. M. A. Jones is grammatically correct. M A Jones is arguably correct. This is An problem too.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_duplicate_word_indices().size() == 1);
        CHECK(doc.get_incorrect_article_indices().size() == 4);
        CHECK(doc.get_lowercase_beginning_sentences().size() == 1);
        }
    
    SECTION("Sentences With Following Dash No Quote")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"\"Hello.--The author.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 2);
        CHECK(doc.get_complete_sentence_count() == 2);
        }
    SECTION("Sentences With Following Dash No Quote Acronym")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"\"Hi Dr.--A doctor?";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 1);
        CHECK(doc.get_complete_sentence_count() == 1);
        }
    SECTION("Sentences With File Name At End")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Image.bmp. Here is a new sentence.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 2);
        }
    SECTION("Sentences Ending With Acronym")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Hello Dr.\n\nThat is dr. Smith.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 2);
        CHECK(std::wcscmp(doc.get_words().at(4).c_str(), L"dr.") == 0);
        CHECK(std::wcscmp(doc.get_words().at(1).c_str(), L"Dr.") == 0);
        }
    
    SECTION("Indenting")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t text[] = L"This is some\n   on a second line\n\tand then a third line\n\n\n\tthis is a whole new paragraph.";
        // default behavior--indented newlines are new paragraphs
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_paragraph_count() == 4);
        CHECK(doc.get_sentence_count() == 4);
        // ignore indenting. first 3 lines are 1 sentence and last 1 is a new paragraph
        doc.load_document(text, wcslen(text), false, false, true, false);
        CHECK(doc.get_paragraph_count() == 2);
        CHECK(doc.get_sentence_count() == 2);
        // ignore indenting and blank lines--should be one giant sentence
        doc.load_document(text, wcslen(text), false, true, true, false);
        CHECK(doc.get_paragraph_count() == 1);
        CHECK(doc.get_sentence_count() == 1);
        }
    SECTION("Ignore Indenting Bullet Points")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t text[] = L"- Item 1\n- Item 2\n   More about Item 2.";
        // default behavior--indented newlines are new paragraphs
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(3 == doc.get_paragraph_count());
        CHECK(3 == doc.get_sentence_count());
        // ignore indenting. Bullets are still separate, but last line gets merged with last bullet point
        doc.load_document(text, wcslen(text), false, false, true, false);
        CHECK(2 == doc.get_paragraph_count());
        CHECK(2 == doc.get_sentence_count());
        }

    SECTION("Exclude colon sentence")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
                             &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
                             &Known_spellings, &Secondary_known_spellings,
                             &Programming_known_spellings, &Stop_list);
        const wchar_t text[] = L"Page:\n\nAnother sentence.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        // count it
        CHECK(doc.get_complete_sentence_count() == 2);
        CHECK(doc.get_valid_word_count() == 3);
        CHECK(doc.get_valid_paragraph_count() == 2);
        // exclude it
        doc.set_aggressive_exclusion(true);
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_valid_word_count() == 2);
        CHECK(doc.get_valid_paragraph_count() == 1);
        }

    SECTION("Double Word With Punctuation")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t text[] = L"Ay, ay captain.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_duplicate_word_indices().size() == 0); // the comma negates that
        }
    SECTION("Double Word Initials")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t text[] = L"L. L. Bean.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_duplicate_word_indices().size() == 0); // really initial
        }
    SECTION("Double Word Numbers")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t text[] = L" 5 5 are good numbers";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_duplicate_word_indices().size() == 0); // ok to repeat numbers
        }
    
    
    SECTION("Numerals")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t text[] = L"The 5 cats in the hat had $3.50 at 5% and $3USD.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_words().at(1).is_numeric());
        CHECK(doc.get_words().at(7).is_numeric());
        CHECK(doc.get_words().at(9).is_numeric());
        CHECK(doc.get_words().at(11).is_numeric());
        }
    SECTION("Word With Numbers")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t text[] = L"This is a 2d graph.  That is a 2d-graph.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_words().at(3).is_numeric());
        CHECK(doc.get_words().at(8).is_numeric() == false);
        }
    SECTION("Unicode Numerals")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t text[] = L"The \xFF15 cats in the hat had $\xFF13.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_words().at(1).is_numeric());
        CHECK(doc.get_words().at(7).is_numeric());
        }
    SECTION("Numerals Equation")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"5 + 1.4 = x1";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_words().at(0).is_numeric());
        CHECK(doc.get_words().at(1).is_numeric());
        CHECK(doc.get_words().at(2).is_numeric());

        text = L"5 + 1.4 = mod3"; // mod3 doesn't have enough numbers in it to be numeric
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_words().at(0).is_numeric());
        CHECK(doc.get_words().at(1).is_numeric());
        CHECK(doc.get_words().at(2).is_numeric() == false);

        text = L"5 + 1.4 = mod369";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_words().at(0).is_numeric());
        CHECK(doc.get_words().at(1).is_numeric());
        CHECK(doc.get_words().at(2).is_numeric());
        }
    SECTION("Wordy Phrases Proceeding Exception")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        pmap.load_phrases(L"i are\ti am\t1\tand\t", true, false);
        const wchar_t text[] = L"You and I are going. I are one and. I are one.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_known_phrase_indices().size() == 2);
        CHECK(doc.get_known_phrase_indices()[0].first == 5);
        CHECK(doc.get_known_phrase_indices()[0].second == 0);
        CHECK(doc.get_known_phrase_indices()[1].first == 9);
        CHECK(doc.get_known_phrase_indices()[1].second == 0);
        }
    SECTION("Wordy Phrases Trailing Exception")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        pmap.load_phrases(L"Could of\tcould have\t1\t\tCourse", true, false);
        const wchar_t text[] = L"We could of course do that. We could of done that. Could of.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_known_phrase_indices().size() == 2);
        CHECK(doc.get_known_phrase_indices()[0].first == 7);
        CHECK(doc.get_known_phrase_indices()[0].second == 0);
        CHECK(doc.get_known_phrase_indices()[1].first == 11);
        CHECK(doc.get_known_phrase_indices()[1].second == 0);
        }
    // "IGN" should be seen as proper if only acronym in text.
    SECTION("Proper Single Acronym Start Of Sentence")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t text[] = L"IGN Magazine: I think Chun Li is the best fighter in Street Fighter. Cammy, Ryu, and Ken are cool too. Sincerely, Joe Franklyn Cool, Alan Smithee.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_words()[0].is_proper_noun());
        CHECK(doc.get_words()[1].is_proper_noun());
        CHECK(doc.get_words()[4].is_proper_noun());
        CHECK(doc.get_words()[5].is_proper_noun());
        CHECK(doc.get_words()[11].is_proper_noun());
        CHECK(doc.get_words()[12].is_proper_noun());
        CHECK(doc.get_words()[14].is_proper_noun());
        CHECK(doc.get_words()[16].is_proper_noun());
        CHECK(doc.get_words()[21].is_proper_noun());
        CHECK(doc.get_words()[22].is_proper_noun());
        CHECK(doc.get_words()[23].is_proper_noun());
        }
    SECTION("Proper Phrases")
        {
        /* Tests:
           Proper phrases at start of document/sentence.
           Midsentence.
           End of sentence.
           Variable length phrases.
           Adjacent phrases, separated by punctuation.
           Single proper nouns, separated by punctuation.
           End of document.*/
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t text[] = L"IGN Magazine: I think Chun Li is the best fighter in Street Fighter. Cammy, Ryu, and Ken are cool to. Sincerely, Joe Franklyn Cool, Alan Smithee.";
        doc.set_search_for_proper_phrases(true);
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_words()[0].is_proper_noun());
        CHECK(doc.get_words()[1].is_proper_noun());
        CHECK(doc.get_words()[4].is_proper_noun());
        CHECK(doc.get_words()[5].is_proper_noun());
        CHECK(doc.get_words()[11].is_proper_noun());
        CHECK(doc.get_words()[12].is_proper_noun());
        CHECK(doc.get_words()[14].is_proper_noun());
        CHECK(doc.get_words()[16].is_proper_noun());
        CHECK(doc.get_words()[21].is_proper_noun());
        CHECK(doc.get_words()[22].is_proper_noun());
        CHECK(doc.get_words()[23].is_proper_noun());
        CHECK(5 == doc.get_proper_phrase_indices().size());
        CHECK(0 == doc.get_proper_phrase_indices()[0].first);
        CHECK(2 == doc.get_proper_phrase_indices()[0].second);

        CHECK(4 == doc.get_proper_phrase_indices()[1].first);
        CHECK(2 == doc.get_proper_phrase_indices()[1].second);

        CHECK(11 == doc.get_proper_phrase_indices()[2].first);
        CHECK(2 == doc.get_proper_phrase_indices()[2].second);

        CHECK(21 == doc.get_proper_phrase_indices()[3].first);
        CHECK(3 == doc.get_proper_phrase_indices()[3].second);

        CHECK(24 == doc.get_proper_phrase_indices()[4].first);
        CHECK(2 == doc.get_proper_phrase_indices()[4].second);
        }
    SECTION("Auto Detected Phrases Stemmed")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction,
            &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns,
            &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings,
            &Programming_known_spellings, &Stop_list);
        doc.set_search_for_proper_phrases(true);
        doc.add_n_gram_size_to_auto_detect(2);
        doc.add_n_gram_size_to_auto_detect(3);
        doc.add_n_gram_size_to_auto_detect(4);
        doc.add_n_gram_size_to_auto_detect(5);
        const wchar_t text[] = L"The good things about this game are something. Another good thing about this game is the sound. Nothing but good things to say.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(8 == doc.get_n_grams_indices().size());

        CHECK(1 == doc.get_n_grams_indices()[0].first);
        CHECK(2 == doc.get_n_grams_indices()[0].second);

        CHECK(9 == doc.get_n_grams_indices()[1].first);
        CHECK(2 == doc.get_n_grams_indices()[1].second);

        CHECK(19 == doc.get_n_grams_indices()[2].first);
        CHECK(2 == doc.get_n_grams_indices()[2].second);

        CHECK(2 == doc.get_n_grams_indices()[3].first);
        CHECK(4 == doc.get_n_grams_indices()[3].second);

        CHECK(10 == doc.get_n_grams_indices()[4].first);
        CHECK(4 == doc.get_n_grams_indices()[4].second);

        CHECK(13 == doc.get_n_grams_indices()[5].first);
        CHECK(4 == doc.get_n_grams_indices()[5].second);

        CHECK(1 == doc.get_n_grams_indices()[6].first);
        CHECK(5 == doc.get_n_grams_indices()[6].second);

        CHECK(9 == doc.get_n_grams_indices()[7].first);
        CHECK(5 == doc.get_n_grams_indices()[7].second);
        }
    SECTION("Auto Detected Negated Phrases")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        doc.set_search_for_negated_phrases(true);
        const wchar_t text[] = L"I am NOT a happy camper. I am not happy with this. She is not a happy camper.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(3 == doc.get_negating_phrase_indices().size());
        CHECK(2 == doc.get_negating_phrase_indices()[0].first); // "I am not a happy"
        CHECK(3 == doc.get_negating_phrase_indices()[0].second);

        CHECK(8 == doc.get_negating_phrase_indices()[1].first); // "I am not happy"
        CHECK(2 == doc.get_negating_phrase_indices()[1].second);

        CHECK(14 == doc.get_negating_phrase_indices()[2].first); // "She is not a happy"
        CHECK(3 == doc.get_negating_phrase_indices()[2].second);
        }
    SECTION("Auto Detected Negated Step Back Boundary Check")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction,
            &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        doc.set_search_for_negated_phrases(true);
        // rest of the document will be the negative phrase
        const wchar_t text[] = L"It isn't that great.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(1 == doc.get_negating_phrase_indices().size());

        CHECK(1 == doc.get_negating_phrase_indices()[0].first);
        CHECK(3 == doc.get_negating_phrase_indices()[0].second);
        }
    SECTION("Auto Detected Negated Phrases2")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        doc.set_search_for_negated_phrases(true);
        const wchar_t text[] = L"One big combo doesn't constitute depth.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(1 == doc.get_negating_phrase_indices().size());
        CHECK(3 == doc.get_negating_phrase_indices()[0].first); // "doesn't constitute depth"
        CHECK(3 == doc.get_negating_phrase_indices()[0].second);
        }
    SECTION("Auto Detected Negated Phrases3")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        doc.set_search_for_negated_phrases(true);
        const wchar_t text[] = L"I said that I don't know. Lately he doesn't help much.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(2 == doc.get_negating_phrase_indices().size());

        CHECK(4 == doc.get_negating_phrase_indices()[0].first);
        CHECK(2 == doc.get_negating_phrase_indices()[0].second);

        CHECK(8 == doc.get_negating_phrase_indices()[1].first);
        CHECK(2 == doc.get_negating_phrase_indices()[1].second);
        }
    SECTION("AutoDetected Proper Phrases")
         {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        doc.set_search_for_proper_phrases(true);
        const wchar_t text[] = L"George W. Bush and Dick Cheney were elected in 2000. George W. Bush and Dick Cheney were elected in 2004. Specifically, George W. Bush, Dick Cheney, were elected.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(6 == doc.get_proper_phrase_indices().size());

        CHECK(0 == doc.get_proper_phrase_indices()[0].first);
        CHECK(3 == doc.get_proper_phrase_indices()[0].second);

        CHECK(4 == doc.get_proper_phrase_indices()[1].first);
        CHECK(2 == doc.get_proper_phrase_indices()[1].second);

        CHECK(10 == doc.get_proper_phrase_indices()[2].first);
        CHECK(3 == doc.get_proper_phrase_indices()[2].second);

        CHECK(14 == doc.get_proper_phrase_indices()[3].first);
        CHECK(2 == doc.get_proper_phrase_indices()[3].second);

        CHECK(21 == doc.get_proper_phrase_indices()[4].first);
        CHECK(3 == doc.get_proper_phrase_indices()[4].second);

        CHECK(24 == doc.get_proper_phrase_indices()[5].first);
        CHECK(2 == doc.get_proper_phrase_indices()[5].second);
        }
    SECTION("Auto Detected Phrases Stop Words And Sentence Boundary")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        doc.add_n_gram_size_to_auto_detect(3);

        const wchar_t text[] = L"Material’s hard and tough. Material's hard and definitely abrasive.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(1 == doc.get_n_grams_indices().size());
        CHECK(1 == doc.get_n_grams_indices()[0].first);
        // "hard and tough" ("Material’s hard and" is ignored because it ends with a stop word)
        CHECK(3 == doc.get_n_grams_indices()[0].second);
        // "definitely" is also a stop word
        }
    SECTION("Aggregated Words No Stop Words")
        {
        phrase_collection kpmap;
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &kpmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, nullptr, nullptr);
        kpmap.load_phrases(L"difficult to grasp\n\thard\nhard to learn\thard", true, false);
        const wchar_t text[] = L"That is difficult. It is difficult to grasp for me. It is hard to learn for us.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        doc.aggregate_tokens();
        CHECK(9 == doc.get_aggregated_tokens().get_data().size());
        auto tokIter = doc.get_aggregated_tokens().get_data().begin();
        CHECK(tokIter->first.to_string() == L"difficult");
        CHECK(1 == tokIter->second.second);

        CHECK((++tokIter)->first.to_string() == L"difficult to grasp");
        CHECK(1 == tokIter->second.second);

        CHECK((++tokIter)->first.to_string() == L"for");
        CHECK(2 == tokIter->second.second);
    
        CHECK((++tokIter)->first.to_string() == L"hard to learn");
        CHECK(1 == tokIter->second.second);
    
        CHECK((++tokIter)->first.to_string() == L"is");
        CHECK(3 == tokIter->second.second);
    
        CHECK((++tokIter)->first.to_string() == L"It");
        CHECK(2 == tokIter->second.second);
    
        CHECK((++tokIter)->first.to_string() == L"me");
        CHECK(1 == tokIter->second.second);
    
        CHECK((++tokIter)->first.to_string() == L"That");
        CHECK(1 == tokIter->second.second);
    
        CHECK((++tokIter)->first.to_string() == L"us");
        CHECK(1 == tokIter->second.second);
        }
    SECTION("Aggregated Words With Stop Words")
        {
        phrase_collection kpmap;
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &kpmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        kpmap.load_phrases(L"difficult to grasp\n\thard\nhard to learn\thard", true, false);
        const wchar_t text[] = L"That is difficult. It is difficult to grasp for me. It is hard to learn for us.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        doc.aggregate_tokens();
        CHECK(3 == doc.get_aggregated_tokens().get_data().size());
        auto tokIter = doc.get_aggregated_tokens().get_data().begin();
        CHECK(tokIter->first.to_string() == L"difficult");
        CHECK(1 == tokIter->second.second);

        CHECK((++tokIter)->first.to_string() == L"difficult to grasp");
        CHECK(1 == tokIter->second.second);

        CHECK((++tokIter)->first.to_string() == L"hard to learn");
        CHECK(1 == tokIter->second.second);
        }
    SECTION("Aggregated Words Negating Phrases")
        {
        phrase_collection kpmap;
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &kpmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        kpmap.load_phrases(L"difficult to grasp\n\thard\nhard to learn\thard", true, false);
        const wchar_t text[] = L"That is difficult. It is difficult to grasp for me. I will never do that again.";
        doc.set_search_for_negated_phrases(true);
        doc.load_document(text, wcslen(text), false, false, false, false);
        doc.aggregate_tokens();
        CHECK(3 == doc.get_aggregated_tokens().get_data().size());
        auto tokIter = doc.get_aggregated_tokens().get_data().begin();
        CHECK(tokIter->first.to_string() == L"difficult");
        CHECK(1 == tokIter->second.second);

        CHECK((++tokIter)->first.to_string() == L"difficult to grasp");
        CHECK(1 == tokIter->second.second);

        CHECK((++tokIter)->first.to_string() == L"never do that again");
        CHECK(1 == tokIter->second.second);
        }

    SECTION("Conjunction")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t text[] = L"I am Mr. Smithe.  And name is C. Blake Smithe.  BUT I usually go by Blake.  Or the Blakeman.  Seriously, call me Blake.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_conjunction_beginning_sentences().size() == 3);
        CHECK(doc.get_conjunction_beginning_sentences()[0] == 1);
        CHECK(doc.get_conjunction_beginning_sentences()[1] == 2);
        CHECK(doc.get_conjunction_beginning_sentences()[2] == 3);
        }

    SECTION("Sentence Positions")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"C# is a programming. Language";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_words().at(0).get_sentence_position() == 0);
        CHECK(doc.get_words().at(1).get_sentence_position() == 1);
        CHECK(doc.get_words().at(2).get_sentence_position() == 2);
        CHECK(doc.get_words().at(3).get_sentence_position() == 3);
        CHECK(doc.get_words().at(4).get_sentence_position() == 0);
        }
    SECTION("Capitalized")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"C# is a Microsoft programming. Language";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_words().at(0).is_capitalized() == true);
        CHECK(doc.get_words().at(1).is_capitalized() == false);
        CHECK(doc.get_words().at(2).is_capitalized() == false);
        CHECK(doc.get_words().at(3).is_capitalized() == true);
        CHECK(doc.get_words().at(4).is_capitalized() == false);
        CHECK(doc.get_words().at(5).is_capitalized() == true);
        }
    SECTION("Plural Possessive")
        {
        document<word_case_insensitive_no_stem> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"students' word";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_punctuation().size() == 1);
        CHECK(doc.get_punctuation().at(0).get_word_position() == 1);
        CHECK(doc.get_punctuation().at(0).get_punctuation_mark() == L'\'');
        CHECK(doc.get_words().at(0) == L"students");
        }
    SECTION("Plural Possessive New Line")
        {
        document<word_case_insensitive_no_stem> doc(L"", &ENsyllabizer, &ENStemmer,
            &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns,
            &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"students' \nword";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(doc.get_words().at(0) == L"students");
        }
    SECTION("Proper")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"My C# Explanation By Blake\n\nHi, I’m Blake and C# is a popular Microsoft language. I like C#!  However, it say, I use also C.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_words().at(0).is_proper_noun() == false);
        CHECK(doc.get_words().at(1).is_proper_noun() == true);
        CHECK(doc.get_words().at(2).is_proper_noun() == false);
        CHECK(doc.get_words().at(3).is_proper_noun() == false);
        CHECK(doc.get_words().at(4).is_proper_noun() == true);

        CHECK(doc.get_words().at(5).is_proper_noun() == false);
        CHECK(doc.get_words().at(6).is_proper_noun() == false);
        CHECK(doc.get_words().at(7).is_proper_noun() == true);
        CHECK(doc.get_words().at(8).is_proper_noun() == false);
        CHECK(doc.get_words().at(9).is_proper_noun() == true);
        CHECK(doc.get_words().at(10).is_proper_noun() == false);
        CHECK(doc.get_words().at(11).is_proper_noun() == false);
        CHECK(doc.get_words().at(12).is_proper_noun() == false);
        CHECK(doc.get_words().at(13).is_proper_noun() == true);
        CHECK(doc.get_words().at(14).is_proper_noun() == false);

        CHECK(doc.get_words().at(15).is_proper_noun() == false);
        CHECK(doc.get_words().at(16).is_proper_noun() == false);
        CHECK(doc.get_words().at(17).is_proper_noun() == true);

        CHECK(doc.get_words().at(18).is_proper_noun() == false);
        CHECK(doc.get_words().at(19).is_proper_noun() == false);
        CHECK(doc.get_words().at(20).is_proper_noun() == false);
        CHECK(doc.get_words().at(21).is_proper_noun() == false);
        CHECK(doc.get_words().at(22).is_proper_noun() == false);
        CHECK(doc.get_words().at(23).is_proper_noun() == false);
        CHECK(doc.get_words().at(24).is_proper_noun() == false);
        }
    SECTION("Unknown Proper")
        {
        Known_proper_nouns.clear();

        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction,
            &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Ann had a good idea. George and Blake have a good idea. Blake has a better idea.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        // first, try it with regular deduction
        CHECK(doc.get_words().at(0).is_proper_noun() == false);
        CHECK(doc.get_words().at(1).is_proper_noun() == false);
        CHECK(doc.get_words().at(2).is_proper_noun() == false);
        CHECK(doc.get_words().at(3).is_proper_noun() == false);
        CHECK(doc.get_words().at(4).is_proper_noun() == false);
        CHECK(doc.get_words().at(5).is_proper_noun() == false);
        CHECK(doc.get_words().at(6).is_proper_noun() == false);
        CHECK(doc.get_words().at(7).is_proper_noun() == true);
        CHECK(doc.get_words().at(8).is_proper_noun() == false);
        CHECK(doc.get_words().at(9).is_proper_noun() == false);
        CHECK(doc.get_words().at(10).is_proper_noun() == false);
        CHECK(doc.get_words().at(11).is_proper_noun() == false);
        CHECK(doc.get_words().at(12).is_proper_noun() == true);
        CHECK(doc.get_words().at(13).is_proper_noun() == false);
        CHECK(doc.get_words().at(14).is_proper_noun() == false);
        CHECK(doc.get_words().at(15).is_proper_noun() == false);
        CHECK(doc.get_words().at(16).is_proper_noun() == false);
        }
    SECTION("Known Proper")
        {
        Known_proper_nouns.load_words(L"ANN\nGeorge", true, false);

        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Ann had a good idea. George and Blake have a good idea. Blake has a better idea.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        // first, try it with regular deduction
        CHECK(doc.get_words().at(0).is_proper_noun() == true);
        CHECK(doc.get_words().at(1).is_proper_noun() == false);
        CHECK(doc.get_words().at(2).is_proper_noun() == false);
        CHECK(doc.get_words().at(3).is_proper_noun() == false);
        CHECK(doc.get_words().at(4).is_proper_noun() == false);
        CHECK(doc.get_words().at(5).is_proper_noun() == true);
        CHECK(doc.get_words().at(6).is_proper_noun() == false);
        CHECK(doc.get_words().at(7).is_proper_noun() == true);
        CHECK(doc.get_words().at(8).is_proper_noun() == false);
        CHECK(doc.get_words().at(9).is_proper_noun() == false);
        CHECK(doc.get_words().at(10).is_proper_noun() == false);
        CHECK(doc.get_words().at(11).is_proper_noun() == false);
        CHECK(doc.get_words().at(12).is_proper_noun() == true);
        CHECK(doc.get_words().at(13).is_proper_noun() == false);
        CHECK(doc.get_words().at(14).is_proper_noun() == false);
        CHECK(doc.get_words().at(15).is_proper_noun() == false);
        CHECK(doc.get_words().at(16).is_proper_noun() == false);
        }
    SECTION("Known Proper Exclamatory")
        {
        Known_proper_nouns.load_words(L"George\nann", true, false);

        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
            &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings,
            &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"HAS ANN A GOOD IDEA! GEORGE and Blake have a good idea. Blake has a better idea.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_words().at(0).is_proper_noun() == false);
        CHECK(doc.get_words().at(1).is_proper_noun() == true);
        CHECK(doc.get_words().at(2).is_proper_noun() == false);
        CHECK(doc.get_words().at(3).is_proper_noun() == false);
        CHECK(doc.get_words().at(4).is_proper_noun() == false);
        CHECK(doc.get_words().at(5).is_proper_noun() == true);
        CHECK(doc.get_words().at(6).is_proper_noun() == false);
        CHECK(doc.get_words().at(7).is_proper_noun() == true);
        CHECK(doc.get_words().at(8).is_proper_noun() == false);
        CHECK(doc.get_words().at(9).is_proper_noun() == false);
        CHECK(doc.get_words().at(10).is_proper_noun() == false);
        CHECK(doc.get_words().at(11).is_proper_noun() == false);
        CHECK(doc.get_words().at(12).is_proper_noun() == true);
        CHECK(doc.get_words().at(13).is_proper_noun() == false);
        CHECK(doc.get_words().at(14).is_proper_noun() == false);
        CHECK(doc.get_words().at(15).is_proper_noun() == false);
        CHECK(doc.get_words().at(16).is_proper_noun() == false);
        }
    SECTION("Known Proper In Header")
        {
        Known_proper_nouns.load_words(L"george\nAnn", true, false);

        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Ann Book 7 Chapter 1\n\nGEORGE and Blake have a good idea. Blake has a better idea.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_words().at(0).is_proper_noun() == true);
        CHECK(doc.get_words().at(1).is_proper_noun() == false);
        CHECK(doc.get_words().at(2).is_proper_noun() == false);
        CHECK(doc.get_words().at(3).is_proper_noun() == false);
        CHECK(doc.get_words().at(4).is_proper_noun() == false);
        CHECK(doc.get_words().at(5).is_proper_noun() == true);
        CHECK(doc.get_words().at(6).is_proper_noun() == false);
        CHECK(doc.get_words().at(7).is_proper_noun() == true);
        CHECK(doc.get_words().at(8).is_proper_noun() == false);
        CHECK(doc.get_words().at(9).is_proper_noun() == false);
        CHECK(doc.get_words().at(10).is_proper_noun() == false);
        CHECK(doc.get_words().at(11).is_proper_noun() == false);
        CHECK(doc.get_words().at(12).is_proper_noun() == true);
        CHECK(doc.get_words().at(13).is_proper_noun() == false);
        CHECK(doc.get_words().at(14).is_proper_noun() == false);
        CHECK(doc.get_words().at(15).is_proper_noun() == false);
        CHECK(doc.get_words().at(16).is_proper_noun() == false);
        }
    SECTION("Known Proper In Quote")
        {
        Known_proper_nouns.load_words(L"Ann\nGeorge", true, false);

        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
            &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings,
            &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"GEORGE said \"Ann had a good idea\" and Blake had a good idea. Blake has a better idea.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_words().at(0).is_proper_noun() == true);
        CHECK(doc.get_words().at(1).is_proper_noun() == false);
        CHECK(doc.get_words().at(2).is_proper_noun() == true);
        CHECK(doc.get_words().at(3).is_proper_noun() == false);
        CHECK(doc.get_words().at(4).is_proper_noun() == false);
        CHECK(doc.get_words().at(5).is_proper_noun() == false);
        CHECK(doc.get_words().at(6).is_proper_noun() == false);
        CHECK(doc.get_words().at(7).is_proper_noun() == false);
        CHECK(doc.get_words().at(8).is_proper_noun() == true);
        CHECK(doc.get_words().at(9).is_proper_noun() == false);
        CHECK(doc.get_words().at(10).is_proper_noun() == false);
        CHECK(doc.get_words().at(11).is_proper_noun() == false);
        CHECK(doc.get_words().at(12).is_proper_noun() == false);
        CHECK(doc.get_words().at(13).is_proper_noun() == true);
        CHECK(doc.get_words().at(14).is_proper_noun() == false);
        CHECK(doc.get_words().at(15).is_proper_noun() == false);
        CHECK(doc.get_words().at(16).is_proper_noun() == false);
        CHECK(doc.get_words().at(17).is_proper_noun() == false);
        }
    SECTION("Proper With Quotes")
        {
        Known_proper_nouns.clear();
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap,
            &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings,
            &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"I'm Joe and C# is a popular programming language. I like 'C#'!  I always say: \"You should use C\" also.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_words().at(0).is_proper_noun() == false);
        CHECK(doc.get_words().at(1).is_proper_noun() == true);
        CHECK(doc.get_words().at(2).is_proper_noun() == false);
        CHECK(doc.get_words().at(3).is_proper_noun() == true);
        CHECK(doc.get_words().at(4).is_proper_noun() == false);
        CHECK(doc.get_words().at(5).is_proper_noun() == false);
        CHECK(doc.get_words().at(6).is_proper_noun() == false);
        CHECK(doc.get_words().at(7).is_proper_noun() == false);
        CHECK(doc.get_words().at(8).is_proper_noun() == false);

        CHECK(doc.get_words().at(9).is_proper_noun() == false);
        CHECK(doc.get_words().at(10).is_proper_noun() == false);
        CHECK(doc.get_words().at(11).is_proper_noun() == true);

        CHECK(doc.get_words().at(12).is_proper_noun() == false);
        CHECK(doc.get_words().at(13).is_proper_noun() == false);
        CHECK(doc.get_words().at(14).is_proper_noun() == false);

        CHECK(doc.get_words().at(15).is_proper_noun() == false);
        CHECK(doc.get_words().at(16).is_proper_noun() == false);
        CHECK(doc.get_words().at(17).is_proper_noun() == false);
        CHECK(doc.get_words().at(18).is_proper_noun() == false);
        CHECK(doc.get_words().at(19).is_proper_noun() == false);
        }
    SECTION("File Path")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction,
            &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns,
            &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings,
            &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"http://www.nature.com/nature/peerreview/debate/index.html";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_words().at(0) == L"http://www.nature.com/nature/peerreview/debate/index.html");
        }
    SECTION("End Of Document")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"The End. \n   ";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 1);
        CHECK(doc.get_complete_sentence_count() == 1);
        }
    SECTION("End Of Document2")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"The End \n   ";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 1);
        CHECK(doc.get_complete_sentence_count() == 0);
        }
    SECTION("Odd Words")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"C# is a programming language";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_words().at(0) == L"C#");
        }
    SECTION("Sentences With Ellipse And Question Mark")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"‘you …?’ When Hunter didn't go on, his father asked, “What is it, Buddy?”";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 2);
        }
    SECTION("Sentences With Ellipse And Question Mark2")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"‘you...?’ When Hunter didn't go on, his father asked, “What is it, Buddy?”";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 2);
        }
    SECTION("Sentences With Dash And Question Mark")
        {
        document<word_case_insensitive_no_stem> doc(L"", &ENsyllabizer, &ENStemmer,
            &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns,
            &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings,
            &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"U -? When Hunter didn't go on?";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 2);
        CHECK(doc.get_words().at(0) == L"U");
        CHECK(doc.get_words().at(1) == L"When");
        }
    SECTION("Sentences With Dash And Question Mark2")
        {
        document<word_case_insensitive_no_stem> doc(L"", &ENsyllabizer, &ENStemmer,
            &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns,
            &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings,
            &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"U–? When Hunter didn't go on?";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 2);
        CHECK(doc.get_words().at(0) == L"U");
        CHECK(doc.get_words().at(1) == L"When");
        }
    SECTION("Sentences With Dashes And Quote At EOL")
        {
        document<word_case_insensitive_no_stem> doc(L"", &ENsyllabizer, &ENStemmer,
            &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns,
            &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings,
            &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"how about–\"\nThe man interrupted him.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(2 == doc.get_sentence_count());
        CHECK(doc.get_sentences()[0].ends_with_valid_punctuation());
        CHECK(doc.get_sentences()[1].ends_with_valid_punctuation());
        CHECK(1 == doc.get_punctuation_count());
        CHECK(L'\"' == doc.get_punctuation()[0].get_punctuation_mark());
        CHECK(L'–' == doc.get_sentences()[0].get_ending_punctuation());
        CHECK(L'.' == doc.get_sentences()[1].get_ending_punctuation());

        // new line after quote is needed to see these as 2 sentences
        text = L"how about–\" James trailed off.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(1 == doc.get_sentence_count());
        CHECK(2 == doc.get_punctuation_count());
        CHECK(L'–' == doc.get_punctuation()[0].get_punctuation_mark());
        CHECK(L'\"' == doc.get_punctuation()[1].get_punctuation_mark());
        CHECK(L'.' == doc.get_sentences()[0].get_ending_punctuation());

        // quote is needed to see these as 2 sentences
        text = L"how about–\nJames trailed off.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(1 == doc.get_sentence_count());

        text = L"how about–\n\nJames trailed off.";
        doc.load_document(text, wcslen(text), false, true/*ignore blank line*/, false, false);
        CHECK(1 == doc.get_sentence_count());

        // make sure punctuation gets loaded correctly
        text = L"Em dash(—).";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(1 == doc.get_sentence_count());
        CHECK(doc.get_sentences()[0].ends_with_valid_punctuation());
        CHECK(3 == doc.get_punctuation_count());
        CHECK(L'(' == doc.get_punctuation()[0].get_punctuation_mark());
        CHECK(L'—' == doc.get_punctuation()[1].get_punctuation_mark());
        CHECK(L')' == doc.get_punctuation()[2].get_punctuation_mark());
        CHECK(L'.' == doc.get_sentences()[0].get_ending_punctuation());
        }
    SECTION("Sentences With Dashes")
        {
        document<word_case_insensitive_no_stem> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Him–over there–he works part-time hours.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_words().at(0) == L"Him");
        CHECK(doc.get_words().at(1) == L"over");
        CHECK(doc.get_words().at(2) == L"there");
        CHECK(doc.get_words().at(3) == L"he");
        CHECK(doc.get_words().at(4) == L"works");
        CHECK(doc.get_words().at(5) == L"part-time");
        CHECK(doc.get_words().at(6) == L"hours");
        CHECK(doc.get_sentence_count() == 1);
        }
    SECTION("Sentences Separated By Tabs")
        {
        document<word_case_insensitive_no_stem> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Hours\t5:00 to Eight\tClosed";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 0);
        CHECK(doc.get_sentence_count() == 1);
        CHECK(doc.get_sentences()[0].get_word_count() == 5);
        }
    SECTION("Sentences Separated By Tabs With Spaces")
        {
        document<word_case_insensitive_no_stem> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Hours \t5:00 to Eight  \tClosed";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 0);
        CHECK(doc.get_sentence_count() == 1);
        CHECK(doc.get_sentences()[0].get_word_count() == 5);
        }
    SECTION("Sentences Separated By Tabs Bulleted")
        {
        document<word_case_insensitive_no_stem> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"1.\tHours\n·\t5:00 to Eight\n12\tClosed";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 0);
        CHECK(doc.get_sentence_count() == 3);
        CHECK(doc.get_sentences()[0].get_word_count() == 2);
        CHECK(doc.get_sentences()[1].get_word_count() == 3);
        CHECK(doc.get_sentences()[2].get_word_count() == 2);
        }
    SECTION("Sentences Bulleted And Number At End Of Sentence")
        {
        document<word_case_insensitive_no_stem> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Only grab\n1. Leave some for others.\nHere is a list\n1. Item 1\n2. Item 2\n3. Item 3";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(2 == doc.get_complete_sentence_count());
        CHECK(6 == doc.get_sentence_count());
        CHECK(3 == doc.get_sentences()[0].get_word_count());
        CHECK(4 == doc.get_sentences()[1].get_word_count());
        CHECK(4 == doc.get_sentences()[2].get_word_count());
        CHECK(3 == doc.get_sentences()[3].get_word_count());
        CHECK(3 == doc.get_sentences()[4].get_word_count());
        CHECK(3 == doc.get_sentences()[5].get_word_count());
        }

    SECTION("Sentences Bulleted And Number At End Of Sentence After List")
        {
        document<word_case_insensitive_no_stem> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"A list:\n1. one\n2. two\n\nOnly grab\n1. Leave some for others.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(3 == doc.get_complete_sentence_count());
        CHECK(5 == doc.get_sentence_count());
        CHECK(2 == doc.get_sentences()[0].get_word_count());
        CHECK(2 == doc.get_sentences()[1].get_word_count());
        CHECK(2 == doc.get_sentences()[2].get_word_count());
        CHECK(3 == doc.get_sentences()[3].get_word_count()); // Only grab\n1
        CHECK(4 == doc.get_sentences()[4].get_word_count()); // Leave some for others.
        }
    SECTION("Sentences With Float Numbers")
        {
        document<word_case_insensitive_no_stem> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"I have .4 percent.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 1);
        CHECK(doc.get_words().at(2) == L"4");

        CHECK(doc.get_punctuation().at(0).get_punctuation_mark() == '.');
        CHECK(doc.get_punctuation().at(0).get_word_position() == 2);
        CHECK(doc.get_punctuation().at(0).is_connected_to_previous_word() == false);
        }
    SECTION("Sentences With Ending Spaces")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"You  ";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_complete_sentence_count() == 0);
        }
    SECTION("Sentences With Odd Quote")
        {
        document<word_case_insensitive_no_stem> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"‘Yes, yes, how was it now?’ he thought, going over his dream.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 1);
        CHECK(doc.get_words().at(5) == L"now");
        }
    SECTION("Sentences")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"4”.";
        // all numeric sentence will be seen as some sort of equation or whatnot
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 1);
        CHECK(doc.get_complete_sentence_count() == 0);
        CHECK(doc.get_word_count() == 1);
        CHECK(doc.get_valid_word_count() == 0);
        }
    SECTION("Sentences With Following Dash")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"\"Hello.\"--The author.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 2);
        CHECK(doc.get_complete_sentence_count() == 2);
        }
    SECTION("Sentences With Following Dash2")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Hello. --The author.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 2);
        CHECK(doc.get_complete_sentence_count() == 2);
        }
    SECTION("Sentences With Quote")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Hi, I said \"Hi\".";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 1);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_word_count() == 4);
        }
    SECTION("Sentences Odd Second Sentence With Newline")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"This is a sentence.\n[Insert stuff here]";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 2);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_word_count() == 7);

        CHECK(doc.get_words().at(0).is_valid() == true);
        CHECK(doc.get_words().at(1).is_valid() == true);
        CHECK(doc.get_words().at(2).is_valid() == true);
        CHECK(doc.get_words().at(3).is_valid() == true);
        CHECK(doc.get_words().at(4).is_valid() == false);
        CHECK(doc.get_words().at(5).is_valid() == false);
        CHECK(doc.get_words().at(6).is_valid() == false);
        }
    SECTION("Sentences Odd Second Sentence With Newline2")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction,
            &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"This is a sentence. \n[Insert stuff here]";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 2);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_word_count() == 7);

        CHECK(doc.get_words().at(0).is_valid() == true);
        CHECK(doc.get_words().at(1).is_valid() == true);
        CHECK(doc.get_words().at(2).is_valid() == true);
        CHECK(doc.get_words().at(3).is_valid() == true);
        CHECK(doc.get_words().at(4).is_valid() == false);
        CHECK(doc.get_words().at(5).is_valid() == false);
        CHECK(doc.get_words().at(6).is_valid() == false);
        }
    SECTION("Sentences Odd Second Sentence With Space")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction,
            &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"This is a sentence.  [Insert stuff here]";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 2);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_word_count() == 7);

        CHECK(doc.get_words().at(0).is_valid() == true);
        CHECK(doc.get_words().at(1).is_valid() == true);
        CHECK(doc.get_words().at(2).is_valid() == true);
        CHECK(doc.get_words().at(3).is_valid() == true);
        CHECK(doc.get_words().at(4).is_valid() == false);
        CHECK(doc.get_words().at(5).is_valid() == false);
        CHECK(doc.get_words().at(6).is_valid() == false);
        }
    
    SECTION("Headers")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction,
            &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns,
            &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings,
            &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Chapter 1\n\nIt was a dark and stormy night.\n\nThe End";
        doc.load_document(text, wcslen(text), false, false, false, false);
        // Chapter 1
        CHECK(doc.get_words().at(0).is_valid() == false);
        CHECK(doc.get_words().at(1).is_valid() == false);
        // It was a dark and stormy night.
        CHECK(doc.get_words().at(2).is_valid() == true);
        CHECK(doc.get_words().at(3).is_valid() == true);
        CHECK(doc.get_words().at(4).is_valid() == true);
        CHECK(doc.get_words().at(5).is_valid() == true);
        CHECK(doc.get_words().at(6).is_valid() == true);
        CHECK(doc.get_words().at(7).is_valid() == true);
        CHECK(doc.get_words().at(8).is_valid() == true);
        // The End
        CHECK(doc.get_words().at(9).is_valid() == false);
        CHECK(doc.get_words().at(10).is_valid() == false);

        CHECK(doc.get_paragraph_count() == 3);
        CHECK(doc.get_valid_paragraph_count() == 1);
        CHECK(doc.get_sentence_count() == 3);
        CHECK(doc.get_complete_sentence_count() == 1);
        CHECK(doc.get_word_count() == 11);
        CHECK(doc.get_valid_word_count() == 7);
        }

    SECTION("Bracket")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction,
            &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"This [UCLA].";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_words().at(0) == L"This");
        CHECK(doc.get_words().at(1) == L"UCLA");

        CHECK(doc.get_punctuation().size() == 2);
        CHECK(doc.get_punctuation().at(0).get_word_position() == 1);
        CHECK(doc.get_punctuation().at(1).get_word_position() == 2);

        CHECK(doc.get_punctuation().at(0).get_punctuation_mark() == '[');
        CHECK(doc.get_punctuation().at(1).get_punctuation_mark() == ']');
        }
    SECTION("Bracket2")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction,
            &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns,
            &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings,
            &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"This [UCLA]..";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_words().at(0) == L"This");
        CHECK(doc.get_words().at(1) == L"UCLA");

        CHECK(doc.get_punctuation().size() == 3);
        CHECK(doc.get_punctuation().at(0).get_word_position() == 1);
        CHECK(doc.get_punctuation().at(1).get_word_position() == 2);
        CHECK(doc.get_punctuation().at(2).get_word_position() == 2);

        CHECK(doc.get_punctuation().at(0).get_punctuation_mark() == '[');
        CHECK(doc.get_punctuation().at(1).get_punctuation_mark() == ']');
        CHECK(doc.get_punctuation().at(2).get_punctuation_mark() == '.');
        }
    SECTION("Ellispes")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Uh... oh!";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_words().at(0) == L"Uh");
        CHECK(doc.get_words().at(1) == L"oh");

        CHECK(doc.get_punctuation().size() == 3);
        CHECK(doc.get_punctuation().at(0).get_word_position() == 1);
        CHECK(doc.get_punctuation().at(1).get_word_position() == 1);
        CHECK(doc.get_punctuation().at(2).get_word_position() == 1);

        CHECK(doc.get_punctuation().at(0).get_punctuation_mark() == '.');
        CHECK(doc.get_punctuation().at(1).get_punctuation_mark() == '.');
        CHECK(doc.get_punctuation().at(2).get_punctuation_mark() == '.');

        CHECK(doc.get_punctuation().at(0).is_connected_to_previous_word() == true);
        CHECK(doc.get_punctuation().at(1).is_connected_to_previous_word() == true);
        CHECK(doc.get_punctuation().at(2).is_connected_to_previous_word() == true);
        }
    SECTION("Ellispes2")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings,
            &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Uh...oh!";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_words().at(0) == L"Uh");
        CHECK(doc.get_words().at(1) == L"oh");

        CHECK(doc.get_punctuation().size() == 3);
        CHECK(doc.get_punctuation().at(0).get_word_position() == 1);
        CHECK(doc.get_punctuation().at(1).get_word_position() == 1);
        CHECK(doc.get_punctuation().at(1).get_word_position() == 1);

        CHECK(doc.get_punctuation().at(0).get_punctuation_mark() == '.');
        CHECK(doc.get_punctuation().at(1).get_punctuation_mark() == '.');
        CHECK(doc.get_punctuation().at(1).get_punctuation_mark() == '.');

        CHECK(doc.get_punctuation().at(0).is_connected_to_previous_word() == true);
        CHECK(doc.get_punctuation().at(1).is_connected_to_previous_word() == true);
        CHECK(doc.get_punctuation().at(1).is_connected_to_previous_word() == true);
        }
    SECTION("Contractions")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"Frank's got a new car. Frank's a happy man. Frank's car is nice. The car of Frank's, a nice car. The car of Frank's; a nice car.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(std::wcscmp(doc.get_word(0).c_str(), L"Frank's") == 0);
        CHECK(doc.get_word(0).is_contraction());
        CHECK(std::wcscmp(doc.get_word(5).c_str(), L"Frank's") == 0);
        CHECK(doc.get_word(5).is_contraction());
        CHECK(std::wcscmp(doc.get_word(9).c_str(), L"Frank's") == 0);
        CHECK(doc.get_word(9).is_contraction() == false);
        CHECK(std::wcscmp(doc.get_word(16).c_str(), L"Frank's") == 0);
        CHECK(doc.get_word(16).is_contraction() == false);
        CHECK(std::wcscmp(doc.get_word(23).c_str(), L"Frank's") == 0);
        CHECK(doc.get_word(23).is_contraction() == false);
        }

    SECTION("Contractions leading apos")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap,
            &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"'Twas the night before Christmas, and it 'twould be a nice night.";
        doc.load_document(text, wcslen(text), false, false, false, false);

        CHECK(std::wcscmp(doc.get_word(0).c_str(), L"Twas") == 0);
        CHECK(doc.get_word(0).is_contraction());
        CHECK(!doc.get_word(1).is_contraction());
        CHECK(!doc.get_word(2).is_contraction());
        CHECK(!doc.get_word(3).is_contraction());
        CHECK(!doc.get_word(4).is_contraction());
        CHECK(!doc.get_word(5).is_contraction());
        CHECK(!doc.get_word(6).is_contraction());
        CHECK(std::wcscmp(doc.get_word(7).c_str(), L"twould") == 0);
        }
    }

TEST_CASE("Document 2", "[document]")
    {
    grammar::english_syllabize ENsyllabizer;
    stemming::english_stem<std::wstring> ENStemmer;
    grammar::is_english_coordinating_conjunction is_conjunction;
    grammar::is_incorrect_english_article is_english_mismatched_article;
    grammar::phrase_collection pmap;
    grammar::phrase_collection copyrightPMap;
    grammar::phrase_collection citationPMap;
    grammar::phrase_collection excludedPMap;
    word_list Known_proper_nouns;
    word_list Known_personal_nouns;
    word_list Known_spellings;
    word_list Secondary_known_spellings;
    word_list Programming_known_spellings;

    SECTION("Spaced Percent")
        {
        document<MYWORD> doc(L"", &ENsyllabizer, &ENStemmer, &is_conjunction, &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns, &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings, &Stop_list);
        const wchar_t* text = L"I have 5 %. What % do you have? That is a %.";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_sentence_count() == 3);
        CHECK(doc.get_words()[2] == L"5 %");
        CHECK(doc.get_words()[4] == L"%");
        CHECK(doc.get_words()[11] == L"%");
        CHECK(doc.get_valid_word_count() == 12);
        CHECK(doc.get_word_count() == 12);
        }
    }
// NOLINTEND
// clang-format on
