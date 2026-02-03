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

using MYWORD = word<traits::case_insensitive_ex,
    stemming::english_stem<std::basic_string<wchar_t, traits::case_insensitive_ex> > >;

extern word_list Stop_list;

TEST_CASE("Acronyms", "[acronyms]")
    {
    grammar::english_syllabize ENsyllabizer;
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
    SECTION("Possessive")
        {
        grammar::is_possessive isPossessive;
        CHECK(isPossessive(L"Carl's"));
        CHECK(isPossessive(L"Carl's"));
        CHECK(isPossessive(L"Carl＇s"));
        CHECK(isPossessive(L"Carl'S"));
        CHECK_FALSE(isPossessive(L"Carl"));
        CHECK_FALSE(isPossessive(L"s"));
        CHECK_FALSE(isPossessive(L""));
        CHECK_FALSE(isPossessive(L"'"));
        }
    SECTION("Acronym")
        {
        const wchar_t text[] = L"I live U.S. of A. right now.";
        grammar::is_end_of_sentence sent(false);
        CHECK_FALSE(sent(text, wcslen(text), 10, 7, 1));
        }
    SECTION("Acronym2")
        {
        is_acronym isAcronym;
        CHECK_FALSE(isAcronym(L"A"));
        CHECK_FALSE(isAcronym(L"A."));
        CHECK_FALSE(isAcronym(L"86L"));
        CHECK_FALSE(isAcronym(L"Hello"));
        CHECK_FALSE(isAcronym(L"HeLlo"));
        CHECK(isAcronym(L"ID's"));
        CHECK(isAcronym(L"KAOS"));
        CHECK(isAcronym(L"UN"));
        CHECK(isAcronym(L"U.N."));
        CHECK(isAcronym(L"UN"));
        CHECK_FALSE(isAcronym(L"Is"));
        CHECK_FALSE(isAcronym(L"iOS"));
        CHECK(isAcronym(L"IOs"));
        CHECK_FALSE(isAcronym(L""));
        }
    SECTION("Acronym Dotted")
        {
        CHECK_FALSE(is_acronym::is_dotted_acronym(L""));
        CHECK(is_acronym::is_dotted_acronym(L"N.A.R.C."));
        CHECK_FALSE(is_acronym::is_dotted_acronym(L"image.bmp."));
        CHECK_FALSE(is_acronym::is_dotted_acronym(L"MR."));
        CHECK_FALSE(is_acronym::is_dotted_acronym(L"N.A"));
        CHECK_FALSE(is_acronym::is_dotted_acronym(L"NARC"));
        CHECK_FALSE(is_acronym::is_dotted_acronym(L"ETC..."));
        CHECK(is_acronym::is_dotted_acronym(L"U.N."));
        CHECK_FALSE(is_acronym::is_dotted_acronym(L"n.d.)."));
        CHECK(is_acronym::is_dotted_acronym(L"F.A.K.K.2"));
        CHECK(is_acronym::is_dotted_acronym(L"F.A.K.K.²"));
        CHECK(is_acronym::is_dotted_acronym(L"F.A.K.K.21"));
        }
    SECTION("Acronym Dot Count")
        {
        is_acronym isAcronym;
        CHECK(isAcronym.get_dot_count() == 0);
        CHECK(isAcronym(L"N.A.R.C."));
        CHECK(isAcronym.get_dot_count() == 4);
        CHECK_FALSE(isAcronym(L"image.bmp."));
        // not an acronym, so dots don't get counted
        CHECK(isAcronym.get_dot_count() == 0);
        CHECK(isAcronym(L"MR."));
        CHECK(isAcronym.get_dot_count() == 1);
        CHECK(isAcronym(L"N.A"));
        CHECK(isAcronym.get_dot_count() == 1);
        CHECK(isAcronym(L"NARC"));
        CHECK(isAcronym.get_dot_count() == 0);
        CHECK(isAcronym(L"ETC..."));
        CHECK(isAcronym.get_dot_count() == 3);
        CHECK(isAcronym(L"U.N."));
        CHECK(isAcronym.get_dot_count() == 2);
        }
    SECTION("Acronym Ends With S")
        {
        is_acronym isAcronym;
        CHECK(isAcronym(L"N.A.R.C.s"));
        CHECK(isAcronym.ends_with_lower_s());
        CHECK(isAcronym(L"N.A.R.C."));
        CHECK_FALSE(isAcronym.ends_with_lower_s());
        CHECK_FALSE(isAcronym(L"is"));
        CHECK_FALSE(isAcronym.ends_with_lower_s());
        }
    }

// NOLINTEND
// clang-format on
