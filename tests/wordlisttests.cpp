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
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/indexing/word_list.h"
#include "../src/indexing/word.h"
#include "../src/readability/readability.h"
#include "../src/readability/custom_readability_test.h"

// clang-format off
// NOLINTBEGIN

#define UNUSED_WORD_ARGS 0, 0, 0, false, false, false, false, 0, 0

using namespace Catch::Matchers;
using namespace lily_of_the_valley;

using MYWORD = word<traits::case_insensitive_ex,
    stemming::english_stem<std::basic_string<wchar_t, traits::case_insensitive_ex> > >;

TEST_CASE("Custom familiar words tests", "[custom][readability-tests]")
    {
    SECTION("ProperNounOnlyFirst")
        {
        word_list dcLite;
        word_list spacheLite;
        word_list hjLite;
        word_list stockerLite;
        dcLite.load_words(L"apple pie sugar", true, false);
        spacheLite.load_words(L"chocolate cake sugar", true, false);
        readability::custom_test<MYWORD>
            cTest(L"myDC", L"", readability::readability_test_type::grade_level, L"noop", stemming::stemming_type::english,
                true,
                true, &dcLite,
                true, &spacheLite,
                false, &hjLite,
                false, &stockerLite,
                false,
                readability::proper_noun_counting_method::only_count_first_instance_of_proper_noun_as_unfamiliar, true,
                true, true, true, true, true, true, true, true, true, true, true, true);
        cTest.load_custom_familiar_words(L"sugar coffee");
        CHECK(cTest.get_proper_noun_method() == readability::proper_noun_counting_method::only_count_first_instance_of_proper_noun_as_unfamiliar);

        MYWORD mac(L"Mac", 3, UNUSED_WORD_ARGS);
        mac.set_proper_noun(true);
        CHECK(cTest.is_word_familiar(mac) == false);
        // subsequent calls should be true
        CHECK(cTest.is_word_familiar(mac));
        CHECK(cTest.is_word_familiar(mac));

        cTest.set_proper_noun_method(readability::proper_noun_counting_method::all_proper_nouns_are_familiar);
        CHECK(cTest.get_proper_noun_method() == readability::proper_noun_counting_method::all_proper_nouns_are_familiar);
        CHECK(cTest.is_word_familiar(mac));

        cTest.set_proper_noun_method(readability::proper_noun_counting_method::all_proper_nouns_are_unfamiliar);
        CHECK(cTest.get_proper_noun_method() == readability::proper_noun_counting_method::all_proper_nouns_are_unfamiliar);
        CHECK(cTest.is_word_familiar(mac) == false);

        cTest.set_proper_noun_method(readability::proper_noun_counting_method::only_count_first_instance_of_proper_noun_as_unfamiliar);
        CHECK(cTest.is_word_familiar(mac));

        CHECK(cTest == L"myDC");
        }
    SECTION("Stocker")
        {
        word_list dcLite;
        word_list spacheLite;
        word_list hjLite;
        word_list stockerLite;
        dcLite.load_words(L"apple pie", true, false);
        spacheLite.load_words(L"chocolate cake sugar", true, false);
        stockerLite.load_words(L"catholic pastor", true, false);
        readability::custom_test<MYWORD>
            cTest(L"myDC", L"", readability::readability_test_type::grade_level, L"noop", stemming::stemming_type::english,
                false,
                true, &dcLite,
                false, &spacheLite,
                false, &hjLite,
                true, &stockerLite,
                false,
                readability::proper_noun_counting_method::all_proper_nouns_are_familiar, true,
                true, true, true, true, true, true, true, true, true, true, true, true);
        CHECK(cTest.is_including_dale_chall_list());
        CHECK(cTest.is_including_spache_list() == false);
        CHECK(cTest.is_including_stocker_list());
        CHECK(cTest.is_word_familiar(MYWORD(L"aPPle", 5, 0, 0, 0, false, false, false, false, 0, 0)));
        CHECK(cTest.is_word_familiar(MYWORD(L"Pie", 3, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"american", 8, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"cake", 4, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"sugar", 5, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"PAstor", 6, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"CATHolic", 8, UNUSED_WORD_ARGS)));
        cTest.include_stocker_list(false);
        CHECK(cTest.is_including_stocker_list() == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"PAstor", 6, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"CATHolic", 8, UNUSED_WORD_ARGS)) == false);
        cTest.include_stocker_list(true);
        CHECK(cTest.is_including_stocker_list());
        CHECK(cTest.is_word_familiar(MYWORD(L"PAstor", 6, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"CATHolic", 8, UNUSED_WORD_ARGS)));
        cTest.set_familiar_words_must_be_on_each_included_list(true);
        CHECK(cTest.is_including_stocker_list());
        CHECK(cTest.is_word_familiar(MYWORD(L"PAstor", 6, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"CATHolic", 8, UNUSED_WORD_ARGS)) == false);
        cTest.set_familiar_words_must_be_on_each_included_list(false);
        CHECK(cTest.is_word_familiar(MYWORD(L"PAstor", 6, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"CATHolic", 8, UNUSED_WORD_ARGS)));
        cTest.include_dale_chall_list(false);
        CHECK(cTest.is_word_familiar(MYWORD(L"PAstor", 6, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"CATHolic", 8, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_using_familiar_words());
        }
    SECTION("Compare")
        {
        word_list dcLite;
        word_list spacheLite;
        word_list hjLite;
        word_list stockerLite;
        dcLite.load_words(L"apple pie", true, false);
        spacheLite.load_words(L"chocolate cake sugar", true, false);
        readability::custom_test<MYWORD>
            cTest(L"myDC", L"Abs(5)\n\r+2", readability::readability_test_type::grade_level, L"noop", stemming::stemming_type::english,
                true,
                true, &dcLite,
                true, &spacheLite,
                true, &hjLite,
                false, &stockerLite,
                false,
                readability::proper_noun_counting_method::all_proper_nouns_are_familiar, true,
                true, true, true, true, true, true, true, true, true, true, true, true);
        CHECK(cTest == cTest);
        readability::custom_test<MYWORD>
            cTest2(L"myDC2", L"Abs(5)\n\r+2", readability::readability_test_type::grade_level, L"noop", stemming::stemming_type::english,
                true,
                true, &dcLite,
                true, &spacheLite,
                true, &hjLite,
                false, &stockerLite,
                false,
                readability::proper_noun_counting_method::all_proper_nouns_are_familiar, true,
                true, true, true, true, true, true, true, true, true, true, true, true);
        CHECK((cTest == cTest2) == false);

        readability::custom_test<MYWORD>
            dTest(L"myDC", L"ABS(5)+2", readability::readability_test_type::grade_level, L"noop", stemming::stemming_type::english,
                true,
                true, &dcLite,
                true, &spacheLite,
                true, &hjLite,
                false, &stockerLite,
                false,
                readability::proper_noun_counting_method::all_proper_nouns_are_familiar, true,
                true, true, true, true, true, true, true, true, true, true, true, true);
        CHECK(cTest == dTest);

        dTest.set_formula(L"Abs(5)\n\r+23");
        CHECK((cTest == dTest) == false);
        }
    SECTION("Is Using Familiar Words")
        {
        word_list dcLite;
        word_list spacheLite;
        word_list hjLite;
        word_list stockerLite;
        dcLite.load_words(L"apple pie", true, false);
        spacheLite.load_words(L"chocolate cake sugar", true, false);
        readability::custom_test<MYWORD>
            cTest(L"myDC", L"", readability::readability_test_type::grade_level, L"noop", stemming::stemming_type::english,
                true,
                true, &dcLite,
                true, &spacheLite,
                true, &hjLite,
                false, &stockerLite,
                false,
                readability::proper_noun_counting_method::all_proper_nouns_are_familiar, true,
                true, true, true, true, true, true, true, true, true, true, true, true);
        CHECK(cTest.is_using_familiar_words());

        cTest.include_dale_chall_list(false);
        CHECK(cTest.is_using_familiar_words());

        cTest.include_spache_list(false);
        CHECK(cTest.is_using_familiar_words());

        cTest.include_harris_jacobson_list(false);
        CHECK(cTest.is_using_familiar_words());

        cTest.include_custom_familiar_word_list(false);
        CHECK(cTest.is_using_familiar_words() == false);

        cTest.include_numeric_as_familiar(false);
        CHECK(cTest.is_using_familiar_words() == false);

        cTest.set_proper_noun_method(readability::proper_noun_counting_method::all_proper_nouns_are_unfamiliar);
        CHECK(cTest.is_using_familiar_words() == false);

        cTest.set_proper_noun_method(readability::proper_noun_counting_method::only_count_first_instance_of_proper_noun_as_unfamiliar);
        CHECK(cTest.is_using_familiar_words() == false);

        cTest.set_proper_noun_method(readability::proper_noun_counting_method::all_proper_nouns_are_familiar);
        CHECK(cTest.is_using_familiar_words() == false);

        cTest.set_proper_noun_method(readability::proper_noun_counting_method::all_proper_nouns_are_unfamiliar);
        cTest.include_numeric_as_familiar(true);
        CHECK(cTest.is_using_familiar_words() == false);

        cTest.include_spache_list(true);
        CHECK(cTest.is_using_familiar_words());
        }
    SECTION("DC")
        {
        word_list dcLite;
        word_list spacheLite;
        word_list hjLite;
        word_list stockerLite;
        dcLite.load_words(L"apple pie", true, false);
        spacheLite.load_words(L"chocolate cake sugar", true, false);
        readability::custom_test<MYWORD>
            cTest(L"myDC", L"", readability::readability_test_type::grade_level, L"noop", stemming::stemming_type::english,
                false,
                true, &dcLite,
                false, &spacheLite,
                false, &hjLite,
                false, &stockerLite,
                false,
                readability::proper_noun_counting_method::all_proper_nouns_are_familiar, true,
                true, true, true, true, true, true, true, true, true, true, true, true);
        CHECK(cTest.is_including_dale_chall_list());
        CHECK(cTest.is_including_spache_list() == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"aPPle", 5, 0, 0, 0, false, false, false, false, 0, 0)));
        CHECK(cTest.is_word_familiar(MYWORD(L"Pie", 3, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"american", 8, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"cake", 4, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"sugar", 5, UNUSED_WORD_ARGS)) == false);
        }
    SECTION("Multi List Word")
        {
        word_list dcLite;
        word_list spacheLite;
        word_list hjLite;
        word_list stockerLite;
        dcLite.load_words(L"apple pie", true, false);
        spacheLite.load_words(L"chocolate cake sugar", true, false);
        readability::custom_test<MYWORD>
            cTest(L"myDC", L"", readability::readability_test_type::grade_level, L"noop", stemming::stemming_type::english,
                false,
                true, &dcLite,
                true, &spacheLite,
                false, &hjLite,
                false, &stockerLite,
                false,
                readability::proper_noun_counting_method::all_proper_nouns_are_familiar, true,
                true, true, true, true, true, true, true, true, true, true, true, true);
        CHECK(cTest.is_including_dale_chall_list());
        CHECK(cTest.is_including_spache_list());
        CHECK(cTest.is_word_familiar(MYWORD(L"aPPle", 5, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"aPPle-", 6, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"-aPPle", 6, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"Pie", 3, UNUSED_WORD_ARGS)));
        //both parts of this word are from different word lists
        CHECK(cTest.is_word_familiar(MYWORD(L"aPPle-cake", 10, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"aPPle-cake-pie", 14, UNUSED_WORD_ARGS)));
        //where one token is unfamiliar, whole word is unfamiliar
        CHECK(cTest.is_word_familiar(MYWORD(L"aPPle-crisp", 11, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"mud-cake", 8, UNUSED_WORD_ARGS)) == false);
        //no tokens is unfamiliar
        CHECK(cTest.is_word_familiar(MYWORD(L"-", 1, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"", 0, UNUSED_WORD_ARGS)) == false);
        }
    SECTION("Spache")
        {
        word_list dcLite;
        word_list spacheLite;
        word_list hjLite;
        word_list stockerLite;
        dcLite.load_words(L"apple pie", true, false);
        spacheLite.load_words(L"chocolate cake sugar", true, false);
        readability::custom_test<MYWORD>
            cTest(L"myDC", L"", readability::readability_test_type::grade_level, L"noop", stemming::stemming_type::english,
                false,
                false, &dcLite,
                true, &spacheLite,
                false, &hjLite,
                false, &stockerLite,
                false,
                readability::proper_noun_counting_method::all_proper_nouns_are_familiar, true,
                true, true, true, true, true, true, true, true, true, true, true, true);
        CHECK(cTest.is_including_dale_chall_list() == false);
        CHECK(cTest.is_including_spache_list());
        CHECK(cTest.is_word_familiar(MYWORD(L"aPPle", 5, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"Pie", 3, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"american", 8, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"cake", 4, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"sugar", 5, UNUSED_WORD_ARGS)));
        }
    SECTION("Dc And Spache")
        {
        word_list dcLite;
        word_list spacheLite;
        word_list hjLite;
        word_list stockerLite;
        dcLite.load_words(L"apple pie", true, false);
        spacheLite.load_words(L"chocolate cake sugar", true, false);
        readability::custom_test<MYWORD>
            cTest(L"myDC", L"", readability::readability_test_type::grade_level, L"noop", stemming::stemming_type::english,
                false,
                true, &dcLite,
                true, &spacheLite,
                false, &hjLite,
                false, &stockerLite,
                false,
                readability::proper_noun_counting_method::all_proper_nouns_are_familiar, true,
                true, true, true, true, true, true, true, true, true, true, true, true);
        CHECK(cTest.is_including_dale_chall_list());
        CHECK(cTest.is_including_spache_list());
        CHECK(cTest.is_including_harris_jacobson_list() == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"aPPle", 5, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"Pie", 3, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"american", 8, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"cake", 4, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"sugar", 5, UNUSED_WORD_ARGS)));
        }
    SECTION("DcAndSpacheANDed")
        {
        word_list dcLite;
        word_list spacheLite;
        word_list hjLite;
        word_list stockerLite;
        dcLite.load_words(L"apple pie sugar", true, false);
        spacheLite.load_words(L"chocolate cake sugar", true, false);
        readability::custom_test<MYWORD>
            cTest(L"myDC", L"", readability::readability_test_type::grade_level, L"noop", stemming::stemming_type::english,
                false,
                true, &dcLite,
                true, &spacheLite,
                false, &hjLite,
                false, &stockerLite,
                true,
                readability::proper_noun_counting_method::all_proper_nouns_are_familiar, true,
                true, true, true, true, true, true, true, true, true, true, true, true);
        CHECK(cTest.is_including_dale_chall_list());
        CHECK(cTest.is_including_spache_list());
        CHECK(cTest.is_including_custom_familiar_word_list() == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"aPPle", 5, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"Pie", 3, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"american", 8, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"cake", 4, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"sugar", 5, UNUSED_WORD_ARGS)));
        }
    SECTION("DcAndSpacheANDedWithCustomList")
        {
        word_list dcLite;
        word_list spacheLite;
        word_list hjLite;
        word_list stockerLite;
        dcLite.load_words(L"apple pie sugar", true, false);
        spacheLite.load_words(L"chocolate cake sugar", true, false);
        readability::custom_test<MYWORD>
            cTest(L"myDC", L"", readability::readability_test_type::grade_level, L"noop", stemming::stemming_type::english,
                false,
                true, &dcLite,
                true, &spacheLite,
                false, &hjLite,
                false, &stockerLite,
                true,
                readability::proper_noun_counting_method::all_proper_nouns_are_familiar, true,
                true, true, true, true, true, true, true, true, true, true, true, true);
        CHECK(cTest.is_including_dale_chall_list());
        CHECK(cTest.is_including_spache_list());
        CHECK(cTest.is_including_custom_familiar_word_list() == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"aPPle", 5, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"Pie", 3, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"american", 8, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"cake", 4, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"sugar", 5, UNUSED_WORD_ARGS)));
        //now try it with an empty custom list (that sugar is not on).
        cTest.include_custom_familiar_word_list(true);
        CHECK(cTest.is_word_familiar(MYWORD(L"sugar", 5, UNUSED_WORD_ARGS)) == false);
        cTest.load_custom_familiar_words(L"sugar coffee");
        CHECK(cTest.is_word_familiar(MYWORD(L"sugar", 5, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"coffee", 6, UNUSED_WORD_ARGS)) == false);
        cTest.include_dale_chall_list(false);
        CHECK(cTest.is_word_familiar(MYWORD(L"coffee", 6, UNUSED_WORD_ARGS)) == false);
        cTest.include_spache_list(false);
        //no standard lists now to get in the way
        CHECK(cTest.is_word_familiar(MYWORD(L"coffee", 6, UNUSED_WORD_ARGS)));
        }
    SECTION("DcAndSpacheORedWithCustomList")
        {
        word_list dcLite;
        word_list spacheLite;
        word_list hjLite;
        word_list stockerLite;
        dcLite.load_words(L"apple pie sugar", true, false);
        spacheLite.load_words(L"chocolate cake sugar", true, false);
        readability::custom_test<MYWORD>
            cTest(L"myDC", L"", readability::readability_test_type::grade_level, L"noop", stemming::stemming_type::english,
                true,
                true, &dcLite,
                true, &spacheLite,
                false, &hjLite,
                false, &stockerLite,
                false,
                readability::proper_noun_counting_method::all_proper_nouns_are_familiar, true,
                true, true, true, true, true, true, true, true, true, true, true, true);
        cTest.load_custom_familiar_words(L"sugar coffee");
        CHECK(cTest.is_including_dale_chall_list());
        CHECK(cTest.is_including_spache_list());
        CHECK(cTest.is_including_custom_familiar_word_list());
        CHECK(cTest.is_word_familiar(MYWORD(L"aPPle", 5, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"Pie", 3, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"american", 8, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"cake", 4, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"sugar", 5, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"coffee", 6, UNUSED_WORD_ARGS)));
        }
    SECTION("DcAndSpacheWithStemmedCustomList")
        {
        word_list dcLite;
        word_list spacheLite;
        word_list hjLite;
        word_list stockerLite;
        dcLite.load_words(L"apple pie sugar", true, false);
        spacheLite.load_words(L"chocolate cake sugar", true, false);
        readability::custom_test<MYWORD>
            cTest(L"myDC", L"", readability::readability_test_type::grade_level, L"noop", stemming::stemming_type::english,
                true,
                true, &dcLite,
                true, &spacheLite,
                false, &hjLite,
                false, &stockerLite,
                false,
                readability::proper_noun_counting_method::all_proper_nouns_are_familiar, true,
                true, true, true, true, true, true, true, true, true, true, true, true);
        cTest.load_custom_familiar_words(L"sugar coffee");
        CHECK(cTest.is_including_dale_chall_list());
        CHECK(cTest.is_including_spache_list());
        CHECK(cTest.is_including_custom_familiar_word_list());
        CHECK(cTest.is_word_familiar(MYWORD(L"aPPle", 5, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"Pie", 3, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"american", 8, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"caked", 5, UNUSED_WORD_ARGS)) == false);//DC not stemmed
        CHECK(cTest.is_word_familiar(MYWORD(L"sugar", 5, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"coffee", 6, UNUSED_WORD_ARGS)));
        // try any stemmed words from the custom list
        CHECK(cTest.is_word_familiar(MYWORD(L"sugared", 7, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"sugars", 6, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"coffees", 7, UNUSED_WORD_ARGS)));
        }
    SECTION("Proper Noun")
        {
        word_list dcLite;
        word_list spacheLite;
        word_list hjLite;
        word_list stockerLite;
        dcLite.load_words(L"apple pie sugar", true, false);
        spacheLite.load_words(L"chocolate cake sugar", true, false);
        readability::custom_test<MYWORD>
            cTest(L"myDC", L"", readability::readability_test_type::grade_level, L"noop", stemming::stemming_type::english,
                true,
                true, &dcLite,
                true, &spacheLite,
                false, &hjLite,
                false, &stockerLite,
                false,
                readability::proper_noun_counting_method::all_proper_nouns_are_unfamiliar, true,
                true, true, true, true, true, true, true, true, true, true, true, true);
        cTest.load_custom_familiar_words(L"sugar coffee coffee");
        CHECK(cTest.get_proper_noun_method() == readability::proper_noun_counting_method::all_proper_nouns_are_unfamiliar);

        MYWORD mac(L"Mac", 3, UNUSED_WORD_ARGS);
        mac.set_proper_noun(true);
        CHECK(cTest.is_word_familiar(mac) == false);

        cTest.set_proper_noun_method(readability::proper_noun_counting_method::all_proper_nouns_are_familiar);
        CHECK(cTest.get_proper_noun_method() == readability::proper_noun_counting_method::all_proper_nouns_are_familiar);
        CHECK(cTest.is_word_familiar(mac));
        }

    SECTION("ProperNounOverride")
        {
        word_list dcLite;
        word_list spacheLite;
        word_list hjLite;
        word_list stockerLite;
        dcLite.load_words(L"apple pie sugar", true, false);
        spacheLite.load_words(L"chocolate cake sugar", true, false);
        readability::custom_test<MYWORD>
            cTest(L"myDC", L"", readability::readability_test_type::grade_level, L"noop", stemming::stemming_type::english,
                true,
                true, &dcLite,
                true, &spacheLite,
                false, &hjLite,
                false, &stockerLite,
                false,
                readability::proper_noun_counting_method::all_proper_nouns_are_unfamiliar, false,
                true, true, true, true, true, true, true, true, true, true, true, true);
        cTest.load_custom_familiar_words(L"sugar coffee");
        CHECK(cTest.get_proper_noun_method() == readability::proper_noun_counting_method::all_proper_nouns_are_unfamiliar);

        MYWORD mac(L"MacOSX", 6, UNUSED_WORD_ARGS);
        mac.set_proper_noun(true);
        mac.set_numeric(true);
        CHECK(cTest.is_word_familiar(mac) == false);
        //test proper noun override
        CHECK(cTest.is_word_familiar(mac, true, false));
        CHECK(cTest.is_word_familiar(mac, false, false) == false);
        //test numeric override
        CHECK(cTest.is_word_familiar(mac, false, false) == false);
        CHECK(cTest.is_word_familiar(mac, false, true));
        mac.set_numeric(false);
        CHECK(cTest.is_word_familiar(mac, false, true) == false);
        //test a regular word that is on the list
        MYWORD sugar(L"Sugar", 5, UNUSED_WORD_ARGS);
        CHECK(cTest.is_word_familiar(sugar, false, false));
        CHECK(cTest.is_word_familiar(sugar, true, true));
        //quick test of multi-list word
        CHECK(cTest.is_word_familiar(MYWORD(L"Sugar-piE", 9, UNUSED_WORD_ARGS), false, false));
        }
    SECTION("Numeric")
        {
        word_list dcLite;
        word_list spacheLite;
        word_list hjLite;
        word_list stockerLite;
        dcLite.load_words(L"apple pie sugar", true, false);
        spacheLite.load_words(L"chocolate cake sugar", true, false);
        readability::custom_test<MYWORD>
            cTest(L"myDC", L"", readability::readability_test_type::grade_level, L"noop", stemming::stemming_type::english,
                true,
                true, &dcLite,
                true, &spacheLite,
                false, &hjLite,
                false, &stockerLite,
                false,
                readability::proper_noun_counting_method::all_proper_nouns_are_familiar, false,
                true, true, true, true, true, true, true, true, true, true, true, true);
        cTest.load_custom_familiar_words(L"sugar coffee");
        CHECK(cTest.is_including_numeric_as_familiar() == false);

        MYWORD num(L"12.5", 4, UNUSED_WORD_ARGS);
        num.set_numeric(true);
        CHECK(cTest.is_word_familiar(num) == false);

        cTest.include_numeric_as_familiar(true);
        CHECK(cTest.is_including_numeric_as_familiar());
        CHECK(cTest.is_word_familiar(num));
        }
    SECTION("DcSpacheHj")
        {
        word_list dcLite;
        word_list spacheLite;
        word_list hjLite;
        word_list stockerLite;
        dcLite.load_words(L"apple pie", true, false);
        spacheLite.load_words(L"chocolate cake sugar", true, false);
        hjLite.load_words(L"coke pepsi", true, false);
        readability::custom_test<MYWORD>
            cTest(L"myDC", L"", readability::readability_test_type::grade_level, L"noop", stemming::stemming_type::english,
                false,
                true, &dcLite,
                true, &spacheLite,
                true, &hjLite,
                false, &stockerLite,
                false,
                readability::proper_noun_counting_method::all_proper_nouns_are_familiar, true,
                true, true, true, true, true, true, true, true, true, true, true, true);
        CHECK(cTest.is_including_dale_chall_list());
        CHECK(cTest.is_including_spache_list());
        CHECK(cTest.is_including_harris_jacobson_list());
        CHECK(cTest.is_word_familiar(MYWORD(L"aPPle", 5, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"Pie", 3, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"american", 8, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"cake", 4, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"sugar", 5, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"pepsi", 5, UNUSED_WORD_ARGS)));
        CHECK(cTest.is_word_familiar(MYWORD(L"coke", 4, UNUSED_WORD_ARGS)));
        }
    SECTION("DcSpacheHjORed")
        {
        word_list dcLite;
        word_list spacheLite;
        word_list hjLite;
        word_list stockerLite;
        dcLite.load_words(L"coke apple pie", true, false);
        spacheLite.load_words(L"coke chocolate cake sugar", true, false);
        hjLite.load_words(L"coke pepsi", true, false);
        readability::custom_test<MYWORD>
            cTest(L"myDC", L"", readability::readability_test_type::grade_level, L"noop", stemming::stemming_type::english,
                false,
                true, &dcLite,
                true, &spacheLite,
                true, &hjLite,
                false, &stockerLite,
                true,
                readability::proper_noun_counting_method::all_proper_nouns_are_familiar, true,
                true, true, true, true, true, true, true, true, true, true, true, true);
        CHECK(cTest.is_including_dale_chall_list());
        CHECK(cTest.is_including_spache_list());
        CHECK(cTest.is_including_harris_jacobson_list());
        CHECK(cTest.is_word_familiar(MYWORD(L"aPPle", 5, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"Pie", 3, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"american", 8, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"cake", 4, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"sugar", 5, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"pepsi", 5, UNUSED_WORD_ARGS)) == false);
        CHECK(cTest.is_word_familiar(MYWORD(L"coke", 4, UNUSED_WORD_ARGS)));
        }
    }

TEST_CASE("Familiar words", "[word-list]")
    {
    SECTION("Numeric")
        {
        word_list dcLite;
        dcLite.load_words(L"chocolate cake sugar one thirty trash-man", true, false);
        readability::is_familiar_word<MYWORD, word_list,
            stemming::no_op_stem<std::basic_string<wchar_t, traits::case_insensitive_ex>> >
            is_fam(&dcLite, readability::proper_noun_counting_method::all_proper_nouns_are_familiar, false);
        MYWORD numberWord(L"7.2", 3, UNUSED_WORD_ARGS);
        numberWord.set_numeric(true);

        is_fam.include_numeric_as_familiar(false);
        CHECK(is_fam(numberWord) == false);
        is_fam.include_numeric_as_familiar(true);
        CHECK(is_fam(numberWord));
        }
    SECTION("Proper Noun")
        {
        word_list dcLite;
        dcLite.load_words(L"chocolate cake sugar one thirty trash-man", true, false);
        readability::is_familiar_word<MYWORD, word_list,
            stemming::no_op_stem<std::basic_string<wchar_t, traits::case_insensitive_ex>> >
            is_fam(&dcLite, readability::proper_noun_counting_method::all_proper_nouns_are_familiar, false);
        MYWORD numberWord(L"Jimbo", 5, UNUSED_WORD_ARGS);
        numberWord.set_proper_noun(true);

        is_fam.set_proper_noun_method(readability::proper_noun_counting_method::all_proper_nouns_are_unfamiliar);
        CHECK(is_fam(numberWord) == false);

        is_fam.set_proper_noun_method(readability::proper_noun_counting_method::only_count_first_instance_of_proper_noun_as_unfamiliar);
        CHECK(is_fam(numberWord) == false);//first instance unfamiliar
        CHECK(is_fam(numberWord));
        CHECK(is_fam(numberWord));
        is_fam.clear_encountered_proper_nouns();
        CHECK(is_fam(numberWord) == false);//first instance unfamiliar
        CHECK(is_fam(numberWord));

        is_fam.set_proper_noun_method(readability::proper_noun_counting_method::all_proper_nouns_are_familiar);
        CHECK(is_fam(numberWord));
        }
    SECTION("Hyphen Word")
        {
        word_list dcLite;
        dcLite.load_words(L"chocolate cake sugar one thirty trash-man", true, false);
        readability::is_familiar_word<MYWORD, word_list,
            stemming::no_op_stem<std::basic_string<wchar_t, traits::case_insensitive_ex>> >
            is_fam(&dcLite, readability::proper_noun_counting_method::all_proper_nouns_are_familiar, false);
        CHECK(is_fam(MYWORD(L"one-thirty", 10, UNUSED_WORD_ARGS)));
        CHECK(is_fam(MYWORD(L"one-thirteen", 12, UNUSED_WORD_ARGS)) == false);
        CHECK(is_fam(MYWORD(L"uno-thirty", 10, UNUSED_WORD_ARGS)) == false);
        CHECK(is_fam(MYWORD(L"-", 1, UNUSED_WORD_ARGS)) == false);
        CHECK(is_fam(MYWORD(L"--", 2, UNUSED_WORD_ARGS)) == false);
        CHECK(is_fam(MYWORD(L"one-", 4, UNUSED_WORD_ARGS)));
        CHECK(is_fam(MYWORD(L"-one", 4, UNUSED_WORD_ARGS)));
        CHECK(is_fam(MYWORD(L"chocolate-cake-sugar", 20, UNUSED_WORD_ARGS)));
        CHECK(is_fam(MYWORD(L"trash-man", 9, UNUSED_WORD_ARGS)));
        CHECK(is_fam(MYWORD(L"trash", 5, UNUSED_WORD_ARGS)) == false);
        }
    SECTION("Hyphen Word With Stemming")
        {
        word_list dcLite;
        dcLite.load_words(L"document depart", true, false);
        readability::is_familiar_word<MYWORD, word_list,
            stemming::english_stem<std::basic_string<wchar_t, traits::case_insensitive_ex>>>
            is_fam(&dcLite, readability::proper_noun_counting_method::all_proper_nouns_are_familiar, false);
        CHECK(is_fam(MYWORD(L"document", 8, UNUSED_WORD_ARGS)));
        CHECK(is_fam(MYWORD(L"documents", 9, UNUSED_WORD_ARGS)));
        CHECK(is_fam(MYWORD(L"documentation-department", 24, UNUSED_WORD_ARGS)));
        CHECK(is_fam(MYWORD(L"documentation-departments", 25, UNUSED_WORD_ARGS)));
        CHECK(is_fam(MYWORD(L"documentation-depo", 18, UNUSED_WORD_ARGS)) == false);
        }
    }

TEST_CASE("Word lists", "[word-list]")
    {
    SECTION("Null")
        {
        word_list_with_replacements WL;
        WL.load_words(nullptr, false);
        CHECK(WL.get_list_size() == 0);
        }
    SECTION("Empty(")
        {
        word_list_with_replacements WL;
        WL.load_words(L"", false);
        CHECK(WL.get_list_size() == 0);
        }
    SECTION("Loading")
        {
        word_list_with_replacements WL;
        WL.load_words(L"simple\teasy\ndifficult\thard", false);
        CHECK(WL.get_list_size() == 2);
        CHECK(WL.get_words().begin()->first == L"difficult");
        CHECK(WL.get_words().begin()->second == L"hard");
        CHECK((++WL.get_words().begin())->first == L"simple");
        CHECK((++WL.get_words().begin())->second == L"easy");
        }
    SECTION("Preserve")
        {
        word_list_with_replacements WL;
        WL.load_words(L"simple\teasy\ndifficult\thard", false);
        CHECK(WL.get_list_size() == 2);
        CHECK(WL.get_words().begin()->first == L"difficult");
        CHECK(WL.get_words().begin()->second == L"hard");
        CHECK((++WL.get_words().begin())->first == L"simple");
        CHECK((++WL.get_words().begin())->second == L"easy");
        WL.load_words(L"ask\ttell", true);
        CHECK(WL.get_list_size() == 3);
        CHECK(WL.get_words().begin()->first == L"ask");
        CHECK(WL.get_words().begin()->second == L"tell");
        CHECK((++WL.get_words().begin())->first == L"difficult");
        CHECK((++WL.get_words().begin())->second == L"hard");
        CHECK((++(++WL.get_words().begin()))->first == L"simple");
        CHECK((++(++WL.get_words().begin()))->second == L"easy");
        WL.load_words(L"ask\ttell", false);
        CHECK(WL.get_list_size() == 1);
        CHECK(WL.get_words().begin()->first == L"ask");
        CHECK(WL.get_words().begin()->second == L"tell");
        }
    SECTION("Find")
        {
        word_list_with_replacements WL;
        WL.load_words(L"simple\teasy\ndifficult\thard", false);
        CHECK(WL.find(L"difficult").first == true);
        CHECK(WL.find(L"difficult").second == L"hard");
        CHECK(WL.find(L"simple").first == true);
        CHECK(WL.find(L"simple").second == L"easy");
        CHECK(WL.find(L"easy").first == false);
        CHECK(WL.find(L"easy").second == L"");
        }

    SECTION("WL Null")
        {
        word_list WL;
        WL.load_words(nullptr, false, false);
        CHECK(WL.get_list_size() == 0);
        }
    SECTION("WL Empty")
        {
        word_list WL;
        WL.load_words(L"", false, false);
        CHECK(WL.get_list_size() == 0);
        // just delimiters, but should pick up empty tokens after delims
        WL.load_words(L" ", false, false);
        CHECK(WL.get_list_size() == 1);
        WL.load_words(L" \t", false, false);
        // skips empty tokens between delims
        CHECK(WL.get_list_size() == 1);
        }
    SECTION("WL NoSort")
        {
        word_list WL;
        WL.load_words(L"the\na\nby\ndo", false, false);
        CHECK(WL.get_list_size() == 4);
        CHECK(WL.get_words().at(0) == L"the");
        CHECK(WL.get_words().at(1) == L"a");
        CHECK(WL.get_words().at(2) == L"by");
        CHECK(WL.get_words().at(3) == L"do");
        CHECK(WL.is_sorted() == false);
        }
    SECTION("WL Sort")
        {
        word_list WL;
        WL.load_words(L"the\na\nby\ndo", true, false);
        CHECK(WL.get_list_size() == 4);
        CHECK(WL.get_words().at(0) == L"a");
        CHECK(WL.get_words().at(1) == L"by");
        CHECK(WL.get_words().at(2) == L"do");
        CHECK(WL.get_words().at(3) == L"the");
        CHECK(WL.is_sorted());
        }
    SECTION("WL NoPreserve")
        {
        word_list WL;
        WL.load_words(L"the\na\nby\ndo", false, false);
        CHECK(WL.get_list_size() == 4);
        CHECK(WL.get_words().at(0) == L"the");
        CHECK(WL.get_words().at(1) == L"a");
        CHECK(WL.get_words().at(2) == L"by");
        CHECK(WL.get_words().at(3) == L"do");
        WL.load_words(L"words\nmore", false, false);
        CHECK(WL.get_list_size() == 2);
        CHECK(WL.get_words().at(0) == L"words");
        CHECK(WL.get_words().at(1) == L"more");
        CHECK(WL.is_sorted() == false);
        }
    SECTION("WL Preserve")
        {
        word_list WL;
        WL.load_words(L"the\na\nby\ndo", false, false);
        CHECK(WL.get_list_size() == 4);
        CHECK(WL.get_words().at(0) == L"the");
        CHECK(WL.get_words().at(1) == L"a");
        CHECK(WL.get_words().at(2) == L"by");
        CHECK(WL.get_words().at(3) == L"do");
        WL.load_words(L"words\nmore", false, true);
        CHECK(WL.get_list_size() == 6);
        CHECK(WL.get_words().at(0) == L"the");
        CHECK(WL.get_words().at(1) == L"a");
        CHECK(WL.get_words().at(2) == L"by");
        CHECK(WL.get_words().at(3) == L"do");
        CHECK(WL.get_words().at(4) == L"words");
        CHECK(WL.get_words().at(5) == L"more");
        CHECK(WL.is_sorted() == false);
        }
    SECTION("WL Preserve And Sort")
        {
        word_list WL;
        WL.load_words(L"the\na\nby\ndo", false, false);
        CHECK(WL.get_list_size() == 4);
        CHECK(WL.get_words().at(0) == L"the");
        CHECK(WL.get_words().at(1) == L"a");
        CHECK(WL.get_words().at(2) == L"by");
        CHECK(WL.get_words().at(3) == L"do");
        WL.load_words(L"words\nmore", true, true);
        CHECK(WL.get_list_size() == 6);
        CHECK(WL.get_words().at(0) == L"a");
        CHECK(WL.get_words().at(1) == L"by");
        CHECK(WL.get_words().at(2) == L"do");
        CHECK(WL.get_words().at(3) == L"more");
        CHECK(WL.get_words().at(4) == L"the");
        CHECK(WL.get_words().at(5) == L"words");
        CHECK(WL.is_sorted());
        WL.clear();
        CHECK(WL.get_list_size() == 0);
        }
    SECTION("WL Add Word")
        {
        word_list WL;
        WL.load_words(L"the\na\nby\ndo", true, false);
        CHECK(WL.get_list_size() == 4);
        CHECK(WL.get_words().at(0) == L"a");
        CHECK(WL.get_words().at(1) == L"by");
        CHECK(WL.get_words().at(2) == L"do");
        CHECK(WL.get_words().at(3) == L"the");
        WL.add_word(L"words");
        CHECK(WL.get_list_size() == 5);
        WL.add_word(L"more");
        CHECK(WL.get_list_size() == 6);
        CHECK(WL.get_words().at(0) == L"a");
        CHECK(WL.get_words().at(1) == L"by");
        CHECK(WL.get_words().at(2) == L"do");
        CHECK(WL.get_words().at(3) == L"more");
        CHECK(WL.get_words().at(4) == L"the");
        CHECK(WL.get_words().at(5) == L"words");
        CHECK(WL.is_sorted());
        }
    SECTION("WL Add Words")
        {
        word_list WL;
        WL.load_words(L"the\na\nby\ndo", true, false);
        CHECK(WL.get_list_size() == 4);
        CHECK(WL.get_words().at(0) == L"a");
        CHECK(WL.get_words().at(1) == L"by");
        CHECK(WL.get_words().at(2) == L"do");
        CHECK(WL.get_words().at(3) == L"the");
        std::vector<word_list::word_type> newWords;
        WL.add_words(newWords);
        // should have no effect
        CHECK(WL.get_list_size() == 4);
        newWords.push_back(L"words");
        newWords.push_back(L"more");
        WL.add_words(newWords);
        CHECK(WL.get_list_size() == 6);
        CHECK(WL.get_words().at(0) == L"a");
        CHECK(WL.get_words().at(1) == L"by");
        CHECK(WL.get_words().at(2) == L"do");
        CHECK(WL.get_words().at(3) == L"more");
        CHECK(WL.get_words().at(4) == L"the");
        CHECK(WL.get_words().at(5) == L"words");
        CHECK(WL.is_sorted());
        }
    SECTION("WL Find")
        {
        word_list WL;
        WL.load_words(L"the\na\nby\ndo", true, false);
        CHECK(WL.contains(L"a"));
        CHECK(WL.contains(L"By"));
        CHECK(WL.contains(L"DO"));
        CHECK(WL.contains(L"tHe"));
        CHECK(WL.contains(L"d") == false);
        CHECK(WL.contains(L"then") == false);
        }
    SECTION("WL Remove Duplicates")
        {
        word_list WL;
        WL.load_words(L"the\na\nby\ndo\nthe\na", true, false);
        CHECK(WL.get_list_size() == 6);
        CHECK(WL.get_words().at(0) == L"a");
        CHECK(WL.get_words().at(1) == L"a");
        CHECK(WL.get_words().at(2) == L"by");
        CHECK(WL.get_words().at(3) == L"do");
        CHECK(WL.get_words().at(4) == L"the");
        CHECK(WL.get_words().at(5) == L"the");
        WL.remove_duplicates();
        CHECK(WL.get_words().at(0) == L"a");
        CHECK(WL.get_words().at(1) == L"by");
        CHECK(WL.get_words().at(2) == L"do");
        CHECK(WL.get_words().at(3) == L"the");
        }
    }
// NOLINTEND
// clang-format on
