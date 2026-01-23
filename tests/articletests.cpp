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

// clang-format off
// NOLINTBEGIN

using namespace grammar;

using MYWORD = word<traits::case_insensitive_ex,
    stemming::english_stem<std::basic_string<wchar_t, traits::case_insensitive_ex>>>;

extern word_list Stop_list;

TEST_CASE("English article", "[articles]")
    {
    grammar::is_incorrect_english_article artc;

    SECTION("NULL")
        {
        CHECK_FALSE(artc(L"and", L"Irish"));
        CHECK_FALSE(artc(L"", L"Orange"));
        CHECK_FALSE(artc(L"a", L""));
        }
    SECTION("UX")
        {
        CHECK_FALSE(artc(L"a", L"UX/UI"));
        CHECK(artc(L"an", L"UX/UI"));
        }
    SECTION("UA")
        {
        CHECK_FALSE(artc(L"a", L"UAA-compliant"));
        CHECK(artc(L"an", L"UAA-compliant"));
        }
    SECTION("Histor")
        {
        CHECK_FALSE(artc(L"a", L"historic"));
        CHECK_FALSE(artc(L"a", L"history"));
        CHECK(artc(L"an", L"historic"));
        CHECK(artc(L"an", L"history"));
        }
    SECTION("Honolulu")
        {
        // "Honolulu" has a pronounced H, unlike "honor/honour"
        // So "a Honolulu-based" should be correct
        CHECK_FALSE(artc(L"a", L"Honolulu"));
        CHECK_FALSE(artc(L"a", L"Honolulu-based"));
        CHECK(artc(L"an", L"Honolulu"));
        CHECK(artc(L"an", L"Honolulu-based"));
        }
    SECTION("are")
        {
        CHECK(artc(L"a", L"are"));
        CHECK_FALSE(artc(L"an", L"are"));
        CHECK(artc(L"a", L"ARE"));
        CHECK_FALSE(artc(L"an", L"ARE"));
        }
    SECTION("SHELLEXECUTEINFO")
        {
        CHECK(artc(L"an", L"SHELLEXECUTEINFO"));
        CHECK_FALSE(artc(L"a", L"SHELLEXECUTEINFO"));
        }
    SECTION("UC")
        {
        CHECK(artc(L"an", L"UCX_USBDEVICE_CHARACTERISTIC"));
        CHECK_FALSE(artc(L"a", L"UCX_USBDEVICE_CHARACTERISTIC"));

        CHECK(artc(L"an", L"UCM_TYPEC_PARTNER"));
        CHECK_FALSE(artc(L"a", L"UCM_TYPEC_PARTNER"));
        }
    SECTION("NTSTATUS")
        {
        // would always say it like "En Tee"
        CHECK_FALSE(artc(L"an", L"NTSTATUS"));
        CHECK(artc(L"a", L"NTSTATUS"));
        CHECK_FALSE(artc(L"an", L"ntstatus"));
        CHECK(artc(L"a", L"ntstatus"));
        }
    SECTION("FLT")
        {
        CHECK_FALSE(artc(L"an", L"FLT_IO"));
        CHECK(artc(L"a", L"FLT_IO"));
        CHECK_FALSE(artc(L"an", L"FLT"));
        CHECK(artc(L"a", L"FLT"));
        // would pronounce like "float"
        CHECK(artc(L"an", L"flt_io"));
        CHECK_FALSE(artc(L"a", L"flt_io"));
        }
    SECTION("RLT")
        {
        CHECK_FALSE(artc(L"an", L"RLT_IO"));
        CHECK(artc(L"a", L"RLT_IO"));
        // would pronounce like "rloat"
        CHECK(artc(L"an", L"rlt_io"));
        CHECK_FALSE(artc(L"a", L"rlt_io"));
        }
    SECTION("Acronyms")
        {
        CHECK(artc(L"a", L"UND"));
        CHECK(artc(L"a", L"AMP"));
        CHECK(artc(L"a", L"EMP"));
        CHECK(artc(L"a", L"IMP"));
        CHECK(artc(L"a", L"OMP"));
        CHECK(artc(L"a", L"SAT"));
        CHECK(artc(L"a", L"STT"));
        CHECK(artc(L"a", L"STT-AT"));
        CHECK_FALSE(artc(L"an", L"SAT"));
        }
    SECTION("Punctuation")
        {
        CHECK_FALSE(artc(L"a", L"-verse"));
        CHECK_FALSE(artc(L"an", L"'verse"));
        }
    SECTION("Acronyms Hyphenated")
        {
        CHECK_FALSE(artc(L"a", L"u-verse"));

        CHECK(artc(L"a", L"XML"));
        CHECK(artc(L"a", L"XML-compatible"));

        CHECK(artc(L"an", L"XAML"));
        CHECK(artc(L"an", L"XAML-compatible"));

        CHECK(artc(L"a", L"RTF"));
        CHECK(artc(L"a", L"RTF-license"));
        }
    SECTION("Odd Combos")
        {
        CHECK(artc(L"a", L"LDA")); // wrong with A
        CHECK_FALSE(artc(L"an", L"LDA")); // correct with AN

        CHECK(artc(L"a", L"FPG")); // wrong with A
        CHECK_FALSE(artc(L"an", L"FPG")); // correct with AN

        CHECK(artc(L"a", L"FSA")); // wrong with A
        CHECK_FALSE(artc(L"an", L"FSA")); // correct with AN

        CHECK_FALSE(artc(L"a", L"FLY")); // correct with A
        CHECK(artc(L"an", L"FLY")); // wrong with AN

        CHECK_FALSE(artc(L"a", L"FRY")); // correct with A
        CHECK(artc(L"an", L"FRY")); // wrong with AN

        CHECK(artc(L"a", L"NDA")); // wrong with A
        CHECK_FALSE(artc(L"an", L"NDA")); // correct with AN

        CHECK(artc(L"a", L"SBA")); // wrong with A
        CHECK_FALSE(artc(L"an", L"SBA")); // correct with AN
        }
    SECTION("2-Letter Word")
        {
        CHECK_FALSE(artc(L"a", L"UD")); // correct with A
        CHECK(artc(L"an", L"UD")); // wrong with AN

        // will actually separate this and look at just "re" as a separate word
        CHECK_FALSE(artc(L"a", L"re-implementation")); // correct with A
        CHECK(artc(L"an", L"re-implementation")); // wrong with AN

        // behaviour is different with an acronym "RE"
        CHECK(artc(L"a", L"RE-implementation")); // wrong with A
        CHECK_FALSE(artc(L"an", L"RE-implementation")); // correct with AN

        // behavior will be different with more than one letter
        CHECK(artc(L"a", L"UDS")); // wrong with A
        CHECK_FALSE(artc(L"an", L"UDS")); // correct with AN
        }
    SECTION("Acronyms Mixed")
        {
        CHECK_FALSE(artc(L"a", L"STReport"));
        CHECK(artc(L"an", L"STReport"));

        CHECK(artc(L"a", L"SPBReport"));
        CHECK_FALSE(artc(L"an", L"SPBReport"));

        CHECK(artc(L"a", L"NPV"));
        CHECK_FALSE(artc(L"an", L"NPV"));
        }
    SECTION("No Vowel Word")
        {
        CHECK(artc(L"an", L"Np")); // wrong with an
        CHECK_FALSE(artc(L"a", L"ZPP")); // correct
        CHECK(artc(L"a", L"NZPD")); // should be "an NYPD"
        CHECK(artc(L"an", L"ZPP")); // should be "a ZPP"
        CHECK_FALSE(artc(L"an", L"NZPD")); // correct
        }
    SECTION("Known A Acronyms")
        {
        CHECK_FALSE(artc(L"a", L"UTF"));
        CHECK(artc(L"an", L"UTF"));
        CHECK_FALSE(artc(L"a", L"UNC"));
        CHECK(artc(L"an", L"UNC"));

        CHECK(artc(L"a", L"UNCLE"));
        CHECK_FALSE(artc(L"an", L"UNCLE"));
        }
    SECTION("Schwartz")
        {
        CHECK_FALSE(artc(L"a", L"Schwartz"));
        }
    SECTION("Digit 2 Number")
        {
        CHECK_FALSE(artc(L"a", L"10-hole"));
        CHECK(artc(L"a", L"11-hole"));
        CHECK_FALSE(artc(L"a", L"12-hole"));
        CHECK_FALSE(artc(L"a", L"13-hole"));
        CHECK_FALSE(artc(L"a", L"14-hole"));
        CHECK_FALSE(artc(L"a", L"15-hole"));
        CHECK_FALSE(artc(L"a", L"16-hole"));
        CHECK_FALSE(artc(L"a", L"17-hole"));
        CHECK(artc(L"a", L"18-hole"));
        CHECK_FALSE(artc(L"a", L"19-hole"));
        CHECK_FALSE(artc(L"a", L"20-hole"));
        CHECK_FALSE(artc(L"a", L"21-hole"));
        CHECK_FALSE(artc(L"a", L"31-hole"));
        CHECK_FALSE(artc(L"a", L"41-hole"));
        CHECK_FALSE(artc(L"a", L"51-hole"));
        CHECK_FALSE(artc(L"a", L"61-hole"));
        CHECK_FALSE(artc(L"a", L"71-hole"));
        CHECK(artc(L"a", L"81-hole"));
        CHECK_FALSE(artc(L"a", L"91-hole"));

        CHECK(artc(L"an", L"10-hole"));
        CHECK_FALSE(artc(L"an", L"11-hole"));
        CHECK(artc(L"an", L"12-hole"));
        CHECK(artc(L"an", L"13-hole"));
        CHECK(artc(L"an", L"14-hole"));
        CHECK(artc(L"an", L"15-hole"));
        CHECK(artc(L"an", L"16-hole"));
        CHECK(artc(L"an", L"17-hole"));
        CHECK_FALSE(artc(L"an", L"18-hole"));
        CHECK(artc(L"an", L"19-hole"));
        CHECK(artc(L"an", L"20-hole"));
        CHECK(artc(L"an", L"21-hole"));
        CHECK(artc(L"an", L"31-hole"));
        CHECK(artc(L"an", L"41-hole"));
        CHECK(artc(L"an", L"51-hole"));
        CHECK(artc(L"an", L"61-hole"));
        CHECK(artc(L"an", L"71-hole"));
        CHECK_FALSE(artc(L"an", L"81-hole"));
        CHECK(artc(L"an", L"91-hole"));
        }
    SECTION("Time")
        {
        CHECK_FALSE(artc(L"a", L"10:87"));
        CHECK(artc(L"a", L"11:87"));
        CHECK_FALSE(artc(L"a", L"12:87"));
        CHECK_FALSE(artc(L"a", L"13:87"));
        CHECK_FALSE(artc(L"a", L"14:87"));
        CHECK_FALSE(artc(L"a", L"15:87"));
        CHECK_FALSE(artc(L"a", L"16:87"));
        CHECK_FALSE(artc(L"a", L"17:87"));
        CHECK(artc(L"a", L"18:87"));
        CHECK_FALSE(artc(L"a", L"19:87"));
        CHECK_FALSE(artc(L"a", L"20:87"));

        CHECK(artc(L"an", L"10:87"));
        CHECK_FALSE(artc(L"an", L"11:87"));
        CHECK(artc(L"an", L"12:87"));
        CHECK(artc(L"an", L"13:87"));
        CHECK(artc(L"an", L"14:87"));
        CHECK(artc(L"an", L"15:87"));
        CHECK(artc(L"an", L"16:87"));
        CHECK(artc(L"an", L"17:87"));
        CHECK_FALSE(artc(L"an", L"18:87"));
        CHECK(artc(L"an", L"19:87"));
        CHECK(artc(L"an", L"20:87"));
        }
    SECTION("A Simple")
        {
        CHECK_FALSE(artc(L"a", L"plane"));
        CHECK_FALSE(artc(L"a", L"zoo"));
        CHECK_FALSE(artc(L"a", L"car"));
        CHECK_FALSE(artc(L"a", L"dog"));
        CHECK_FALSE(artc(L"a", L"net"));

        CHECK_FALSE(artc(L"a", L"Plane"));
        CHECK_FALSE(artc(L"a", L"Zoo"));
        CHECK_FALSE(artc(L"a", L"Car"));
        CHECK_FALSE(artc(L"a", L"Dog"));
        CHECK_FALSE(artc(L"a", L"Net"));
        }
    SECTION("A Vowels")
        {
        CHECK(artc(L"a", L"airplane"));
        CHECK(artc(L"a", L"english"));
        CHECK(artc(L"a", L"irish"));
        CHECK(artc(L"a", L"orange"));
        CHECK(artc(L"a", L"undertow"));
        CHECK(artc(L"a", L"onerous"));
        CHECK_FALSE(artc(L"a", L"year"));

        CHECK(artc(L"a", L"Airplane"));
        CHECK(artc(L"a", L"English"));
        CHECK(artc(L"a", L"Irish"));
        CHECK(artc(L"a", L"Orange"));
        CHECK(artc(L"a", L"Undertow"));
        CHECK(artc(L"a", L"Onerous"));
        CHECK_FALSE(artc(L"a", L"Year"));

        CHECK(artc(L"a", L"on-campus"));
        CHECK_FALSE(artc(L"an", L"on-campus"));
        }
    }

TEST_CASE("English article 2", "[articles]")
    {
    grammar::is_incorrect_english_article artc;

    SECTION("A Exceptions")
        {
        artc.get_a_exceptions().add_word(L"and");
        artc.get_a_exceptions().add_word(L"are");
        artc.get_a_exceptions().add_word(L"is");
        artc.get_a_exceptions().add_word(L"nor");
        artc.get_a_exceptions().add_word(L"of");
        artc.get_a_exceptions().add_word(L"once");
        artc.get_a_exceptions().add_word(L"one");
        artc.get_a_exceptions().add_word(L"onetime");
        artc.get_a_exceptions().add_word(L"UAW");
        artc.get_a_exceptions().add_word(L"UCSF");
        artc.get_a_exceptions().add_word(L"USB");
        artc.get_a_exceptions().add_word(L"USPTO");

        CHECK_FALSE(artc(L"a", L"are"));
        CHECK(artc(L"a", L"area"));

        CHECK_FALSE(artc(L"a", L"is"));
        CHECK(artc(L"a", L"istanbul"));

        CHECK_FALSE(artc(L"a", L"and"));
        CHECK(artc(L"a", L"anderson"));

        CHECK(artc(L"a", L"ore"));
        CHECK_FALSE(artc(L"a", L"or"));

        CHECK_FALSE(artc(L"a", L"Euro"));
        CHECK_FALSE(artc(L"a", L"one"));
        // these 2 should be wrong
        CHECK(artc(L"a", L"onerous"));
        CHECK(artc(L"a", L"on-campus"));
        CHECK_FALSE(artc(L"a", L"one-sided"));
        CHECK_FALSE(artc(L"a", L"unicycle"));
        CHECK_FALSE(artc(L"a", L"universal"));
        CHECK_FALSE(artc(L"a", L"united"));
        CHECK_FALSE(artc(L"a", L"unix"));
        CHECK_FALSE(artc(L"a", L"useful"));
        CHECK_FALSE(artc(L"a", L"Utopia"));
        CHECK_FALSE(artc(L"a", L"Ukrainian"));
        CHECK_FALSE(artc(L"a", L"uterus"));
        CHECK_FALSE(artc(L"a", L"urethra"));
        CHECK_FALSE(artc(L"a", L"u-turn"));
        CHECK_FALSE(artc(L"a", L"utility"));
        CHECK_FALSE(artc(L"a", L"using"));
        CHECK(artc(L"a", L"uninteresting"));
        CHECK(artc(L"a", L"unimpressed"));
        CHECK(artc(L"a", L"unidentified"));
        CHECK_FALSE(artc(L"a", L"unilateral"));
        // a Sat (like Saturday) is OK
        CHECK_FALSE(artc(L"a", L"Sat"));
        // a SAT (like the text) is wrong
        CHECK(artc(L"a", L"SAT"));

        CHECK(artc(L"an", L"utility"));
        CHECK(artc(L"an", L"u-turn"));
        CHECK(artc(L"an", L"Euro"));
        CHECK(artc(L"an", L"one"));
        CHECK(artc(L"an", L"one-sided"));
        CHECK(artc(L"an", L"unicycle"));
        CHECK(artc(L"an", L"useful"));
        CHECK(artc(L"an", L"Utopia"));
        CHECK(artc(L"an", L"Ukrainian"));
        CHECK(artc(L"an", L"uterus"));
        CHECK(artc(L"an", L"urethra"));
        CHECK(artc(L"an", L"using"));
        CHECK_FALSE(artc(L"an", L"uninteresting"));
        CHECK_FALSE(artc(L"an", L"unimpressed"));
        CHECK_FALSE(artc(L"an", L"unidentified"));
        CHECK(artc(L"an", L"unilateral"));
        // a Sat (like Saturday) is OK
        CHECK(artc(L"an", L"Sat"));
        // a SAT (like the text) is wrong
        CHECK_FALSE(artc(L"an", L"SAT"));

        // reset
        artc.get_a_exceptions().clear();
        }
    SECTION("A Letters")
        {
        CHECK(artc(L"a", L"x-ray"));
        CHECK(artc(L"a", L"x'x"));
        CHECK_FALSE(artc(L"a", L"u"));
        CHECK_FALSE(artc(L"a", L"u.s."));
        CHECK(artc(L"a", L"h1n1"));

        CHECK_FALSE(artc(L"an", L"x-ray"));
        CHECK_FALSE(artc(L"an", L"x'x"));
        CHECK(artc(L"an", L"u"));
        CHECK(artc(L"an", L"u.s."));
        CHECK_FALSE(artc(L"an", L"h1n1"));
        }
    SECTION("AN Simple")
        {
        CHECK(artc(L"an", L"plane"));
        CHECK(artc(L"an", L"zoo"));
        CHECK(artc(L"an", L"car"));
        CHECK(artc(L"an", L"dog"));
        CHECK(artc(L"an", L"net"));

        CHECK(artc(L"an", L"Plane"));
        CHECK(artc(L"an", L"Zoo"));
        CHECK(artc(L"an", L"Car"));
        CHECK(artc(L"an", L"Dog"));
        CHECK(artc(L"an", L"Net"));
        }
    SECTION("AN Vowels")
        {
        CHECK_FALSE(artc(L"an", L"airplane"));
        CHECK_FALSE(artc(L"an", L"english"));
        CHECK_FALSE(artc(L"an", L"irish"));
        CHECK_FALSE(artc(L"an", L"orange"));
        CHECK_FALSE(artc(L"an", L"undertow"));
        CHECK_FALSE(artc(L"an", L"year"));

        CHECK_FALSE(artc(L"an", L"Airplane"));
        CHECK_FALSE(artc(L"an", L"English"));
        CHECK_FALSE(artc(L"an", L"Irish"));
        CHECK_FALSE(artc(L"an", L"Orange"));
        CHECK_FALSE(artc(L"an", L"Undertow"));
        CHECK_FALSE(artc(L"an", L"Year"));
        }
    SECTION("AN Exceptions")
        {
        CHECK_FALSE(artc(L"a", L"hysterical"));
        CHECK_FALSE(artc(L"an", L"honour"));
        CHECK(artc(L"an", L"honey"));
        CHECK_FALSE(artc(L"an", L"hour"));
        CHECK_FALSE(artc(L"an", L"honest"));
        CHECK(artc(L"an", L"house"));
        CHECK(artc(L"an", L"historical"));
        CHECK(artc(L"an", L"history"));
        CHECK_FALSE(artc(L"an", L"heir"));

        CHECK_FALSE(artc(L"a", L"Hysterical"));
        CHECK_FALSE(artc(L"an", L"Honour"));
        CHECK(artc(L"an", L"Honey"));
        CHECK_FALSE(artc(L"an", L"Hour"));
        CHECK_FALSE(artc(L"an", L"Honest"));
        CHECK(artc(L"an", L"House"));
        CHECK(artc(L"an", L"Historical"));
        CHECK(artc(L"an", L"History"));
        CHECK_FALSE(artc(L"an", L"Heir"));

        CHECK(artc(L"an", L"hysterical"));
        CHECK(artc(L"a", L"honour"));
        CHECK_FALSE(artc(L"a", L"honey"));
        CHECK(artc(L"a", L"hour"));
        CHECK(artc(L"a", L"honest"));
        CHECK_FALSE(artc(L"a", L"house"));
        CHECK_FALSE(artc(L"a", L"historical"));
        CHECK_FALSE(artc(L"a", L"history"));
        CHECK(artc(L"a", L"heir"));
        }
    SECTION("AN Acronyms And Letters")
        {
        CHECK_FALSE(artc(L"an", L"HRESULT"));
        CHECK_FALSE(artc(L"an", L"HMS"));
        CHECK_FALSE(artc(L"an", L"LLC"));
        CHECK_FALSE(artc(L"an", L"MST"));
        CHECK_FALSE(artc(L"an", L"NWP"));
        CHECK_FALSE(artc(L"an", L"RR"));
        CHECK_FALSE(artc(L"an", L"SS"));
        CHECK_FALSE(artc(L"an", L"XBT"));
        // the vowel makes it be seen as a word
        CHECK(artc(L"an", L"HAM"));

        CHECK_FALSE(artc(L"an", L"H1N1"));

        CHECK_FALSE(artc(L"an", L"f"));
        CHECK_FALSE(artc(L"an", L"h"));
        CHECK_FALSE(artc(L"an", L"l"));
        CHECK_FALSE(artc(L"an", L"m"));
        CHECK_FALSE(artc(L"an", L"n"));
        CHECK_FALSE(artc(L"an", L"r"));
        CHECK_FALSE(artc(L"an", L"s"));
        CHECK_FALSE(artc(L"an", L"x"));
        CHECK(artc(L"an", L"u"));

        CHECK_FALSE(artc(L"an", L"F"));
        CHECK_FALSE(artc(L"an", L"H"));
        CHECK_FALSE(artc(L"an", L"L"));
        CHECK_FALSE(artc(L"an", L"M"));
        CHECK_FALSE(artc(L"an", L"N"));
        CHECK_FALSE(artc(L"an", L"R"));
        CHECK_FALSE(artc(L"an", L"S"));
        CHECK_FALSE(artc(L"an", L"X"));
        CHECK(artc(L"an", L"U"));
        }
    SECTION("Numbers")
        {
        CHECK(artc(L"an", L"0-char"));
        CHECK_FALSE(artc(L"a", L"0-char"));

        CHECK(artc(L"an", L"1-char"));
        CHECK_FALSE(artc(L"a", L"1-char"));

        CHECK(artc(L"an", L"2-char"));
        CHECK_FALSE(artc(L"a", L"2-char"));

        CHECK(artc(L"an", L"3-char"));
        CHECK_FALSE(artc(L"a", L"3-char"));

        CHECK(artc(L"an", L"4-char"));
        CHECK_FALSE(artc(L"a", L"4-char"));

        CHECK(artc(L"an", L"5-char"));
        CHECK_FALSE(artc(L"a", L"5-char"));

        CHECK(artc(L"an", L"6-char"));
        CHECK_FALSE(artc(L"a", L"6-char"));

        CHECK(artc(L"an", L"7-char"));
        CHECK_FALSE(artc(L"a", L"7-char"));

        CHECK_FALSE(artc(L"an", L"8-char"));
        CHECK(artc(L"a", L"8-char"));

        CHECK(artc(L"an", L"9-char"));
        CHECK_FALSE(artc(L"a", L"9-char"));

        CHECK(artc(L"an", L"0char"));
        CHECK_FALSE(artc(L"a", L"0char"));

        CHECK(artc(L"an", L"1char"));
        CHECK_FALSE(artc(L"a", L"1char"));

        CHECK(artc(L"an", L"2char"));
        CHECK_FALSE(artc(L"a", L"2char"));

        CHECK(artc(L"an", L"3char"));
        CHECK_FALSE(artc(L"a", L"3char"));

        CHECK(artc(L"an", L"4char"));
        CHECK_FALSE(artc(L"a", L"4char"));

        CHECK(artc(L"an", L"5char"));
        CHECK_FALSE(artc(L"a", L"5char"));

        CHECK(artc(L"an", L"6char"));
        CHECK_FALSE(artc(L"a", L"6char"));

        CHECK(artc(L"an", L"7char"));
        CHECK_FALSE(artc(L"a", L"7char"));

        CHECK_FALSE(artc(L"an", L"8char"));
        CHECK(artc(L"a", L"8char"));

        CHECK(artc(L"an", L"9char"));
        CHECK_FALSE(artc(L"a", L"9char"));

        // don't bother with more complicated numbers
        CHECK_FALSE(artc(L"an", L"1,800"));
        CHECK_FALSE(artc(L"a", L"1,800"));

        CHECK(artc(L"an", L"1st"));
        CHECK_FALSE(artc(L"a", L"1st"));

        CHECK(artc(L"an", L"21st"));
        CHECK_FALSE(artc(L"a", L"21st"));

        CHECK(artc(L"an", L"2nd"));
        CHECK_FALSE(artc(L"a", L"2nd"));

        CHECK(artc(L"an", L"3rd"));
        CHECK_FALSE(artc(L"a", L"3rd"));

        CHECK_FALSE(artc(L"an", L"11th"));
        CHECK(artc(L"a", L"11th"));

        CHECK_FALSE(artc(L"an", L"8th"));
        CHECK(artc(L"a", L"8th"));

        CHECK(artc(L"an", L"4th"));
        CHECK_FALSE(artc(L"a", L"4th"));
        }
    SECTION("Year")
        {
        CHECK_FALSE(artc(L"a", L"1087"));
        CHECK(artc(L"a", L"1187"));
        CHECK_FALSE(artc(L"a", L"1287"));
        CHECK_FALSE(artc(L"a", L"1387"));
        CHECK_FALSE(artc(L"a", L"1487"));
        CHECK_FALSE(artc(L"a", L"1587"));
        CHECK_FALSE(artc(L"a", L"1687"));
        CHECK_FALSE(artc(L"a", L"1787"));
        CHECK(artc(L"a", L"1887"));
        CHECK_FALSE(artc(L"a", L"1987"));
        CHECK_FALSE(artc(L"a", L"2087"));

        CHECK(artc(L"an", L"1087"));
        CHECK_FALSE(artc(L"an", L"1187"));
        CHECK(artc(L"an", L"1287"));
        CHECK(artc(L"an", L"1387"));
        CHECK(artc(L"an", L"1487"));
        CHECK(artc(L"an", L"1587"));
        CHECK(artc(L"an", L"1687"));
        CHECK(artc(L"an", L"1787"));
        CHECK_FALSE(artc(L"an", L"1887"));
        CHECK(artc(L"an", L"1987"));
        CHECK(artc(L"an", L"2087"));
        }
    }

TEST_CASE("English article document level", "[articles]")
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

    // this one requires higher level testing
    SECTION("Punctuation Between Words")
        {
        document<MYWORD> doc(std::wstring{}, &ENsyllabizer, &ENStemmer, &is_conjunction,
            &pmap, &copyrightPMap, &citationPMap, &Known_proper_nouns, &Known_personal_nouns,
            &Known_spellings, &Secondary_known_spellings, &Programming_known_spellings,
            &Stop_list);
        // comma indicates that the 'a' is not connected to 'auto'
        const wchar_t* text = L"a auto";
        doc.load_document(text, wcslen(text), false, false, false, false);
        CHECK(doc.get_incorrect_article_indices().size() == 0);
        }
    }

// NOLINTEND
// clang-format on
