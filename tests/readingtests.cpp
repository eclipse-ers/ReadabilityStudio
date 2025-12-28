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

// clang-format off
// NOLINTBEGIN

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/readability/german_readability.h"
#include "../src/readability/spanish_readability.h"
#include "../src/readability/english_readability.h"
#include "../src/readability/readability_test.h"
#include "../src/readability/readability_project_test.h"

using namespace Catch::Matchers;
using namespace readability;

TEST_CASE("Test collections", "[test-collection][readability-tests]")
    {
    SECTION("Comparisons")
        {
        readability::readability_test LIX(L"lix", 8, L"Lix test", L"Laebarindex", L"This is the lix test", readability::readability_test_type::grade_level, false, L"");
        readability::readability_test LIX2(L"lix", 8, L"LiX", L"Laebarindex", L"blah blah", readability::readability_test_type::grade_level, false, L"");
        readability::readability_test RIX(L"rix", 9, L"Rix test", L"Rate index", L"Rate index is great", readability::readability_test_type::grade_level, false, L"");
        readability::readability_project_test<std::vector<double>> RIX_project(RIX);
        CHECK(LIX == LIX2);
        CHECK(!(LIX == RIX));
        CHECK(LIX == readability::readability_test(L"lIx"));
        CHECK(LIX == readability::readability_test(L"Lix TEST"));
        CHECK(LIX == readability::readability_test(L"LAEBARINDEX"));
        CHECK(LIX < RIX);
        CHECK(!(RIX < LIX));
        CHECK(LIX < RIX_project);
        CHECK(!(RIX_project < LIX));
        }
    SECTION("ProjectWrapperComparisons")
        {
        readability::readability_test LIX(L"lix", 8, L"Lix test", L"Laebarindex", L"This is the lix test", readability::readability_test_type::grade_level, false, L"");
        readability::readability_test LIX2(L"lix", 8, L"LiX", L"Laebarindex", L"blah blah", readability::readability_test_type::grade_level, false, L"");
        readability::readability_test RIX(L"rix", 9, L"Rix test", L"Rate index", L"Rate index is great", readability::readability_test_type::grade_level, false, L"");
        readability::readability_project_test<std::vector<double>> LIX_project(LIX);
        readability::readability_project_test<std::vector<double>> LIX2_project(LIX2);
        readability::readability_project_test<std::vector<double>> RIX_project(RIX);
        CHECK(LIX_project == LIX2_project);
        CHECK(!(LIX_project == RIX_project));
        CHECK(LIX_project == readability::readability_test(L"lIx"));
        CHECK(LIX_project == readability::readability_test(L"Lix TEST"));
        CHECK(LIX_project == readability::readability_test(L"LAEBARINDEX"));
        CHECK(LIX_project < RIX_project);
        CHECK(!(RIX_project < LIX_project));
        CHECK(LIX_project < RIX_project);
        CHECK(!(RIX_project < LIX_project));
        }
    SECTION("ProjectWrapperAssignment")
        {
        readability::readability_test LIX(L"lix", 8, L"Lix test", L"Laebarindex", L"This is the lix test", readability::readability_test_type::grade_level, false, L"");
        readability::readability_test LIX2(L"lix", 8, L"LiX", L"Laebarindex", L"blah blah", readability::readability_test_type::grade_level, false, L"");
        readability::readability_test RIX(L"rix", 9, L"Rix test", L"Rate index", L"Rate index is great", readability::readability_test_type::grade_level, false, L"");
        readability::readability_project_test<std::vector<double>> LIX_project(LIX);
        readability::readability_project_test<std::vector<double>> LIX2_project(RIX);// will be changed in assignment
        LIX2_project = LIX_project;
        readability::readability_project_test<std::vector<double>> RIX_project(RIX);
        CHECK(LIX_project == LIX2_project);
        CHECK(!(LIX2_project == RIX_project));
        CHECK(LIX2_project == readability::readability_test(L"lIx"));
        CHECK(LIX2_project == readability::readability_test(L"Lix TEST"));
        CHECK(LIX2_project == readability::readability_test(L"LAEBARINDEX"));
        CHECK(LIX2_project < RIX_project);
        CHECK(!(RIX_project < LIX2_project));
        CHECK(LIX2_project < RIX_project);
        CHECK(!(RIX_project < LIX2_project));
        }
    SECTION("AddTest")
        {
        readability::readability_test_collection<readability::readability_project_test<std::vector<double>>> col;
        readability::readability_test LIX(L"lix", 8, L"Lix test", L"Laebarindex", L"This is the lix test", readability::readability_test_type::grade_level, false, L"");
        CHECK(static_cast<size_t>(0) == col.get_test_count());
        col.add_test(LIX);
        CHECK(static_cast<size_t>(1) == col.get_test_count());
        // prevent the same test being added
        col.add_test(LIX);
        CHECK(static_cast<size_t>(1) == col.get_test_count());
        readability::readability_test RIX(L"rix", 9, L"Rix test", L"Rate index", L"Rate index is great", readability::readability_test_type::grade_level, false, L"");
        col.add_test(RIX);
        CHECK(static_cast<size_t>(2) == col.get_test_count());
        }
    SECTION("AddTests")
        {
        std::vector<readability::readability_test> tests;
        tests.push_back(readability::readability_test(L"rix", 9, L"Rix test", L"Rate index", L"Rate index is great", readability::readability_test_type::grade_level, false, L""));
        tests.push_back(readability::readability_test(L"lix", 8, L"Lix test", L"Laebarindex", L"This is the lix test", readability::readability_test_type::grade_level, false, L""));
        tests.push_back(readability::readability_test(L"ari", 10, L"ARI test", L"ARI long name", L"ARI is great", readability::readability_test_type::grade_level, false, L""));
        tests.push_back(readability::readability_test(L"lix", 8, L"Lix test", L"Laebarindex", L"This is the lix test", readability::readability_test_type::grade_level, false, L""));//duplicate
        readability::readability_test_collection<readability::readability_project_test<std::vector<double>>> col;
        CHECK(static_cast<size_t>(0) == col.get_test_count());
        col.add_tests(tests);
        CHECK(static_cast<size_t>(3) == col.get_test_count());
        }
    SECTION("IsIncluded")
        {
        readability::readability_test_collection<readability::readability_project_test<std::vector<double>>> col;
        CHECK(col.is_test_included(L"lix") == false);
        readability::readability_test LIX(L"lix", 8, L"Lix test", L"Laebarindex", L"This is the lix test", readability::readability_test_type::grade_level, false, L"");
        readability::readability_test RIX(L"rix", 9, L"Rix test", L"Rate index", L"Rate index is great", readability::readability_test_type::grade_level, false, L"");
        col.add_test(LIX);
        col.add_test(RIX);
        // test will not be included in the project by default
        CHECK(col.is_test_included(L"lix") == false);
        CHECK(col.is_test_included(L"Lix test") == false);
        CHECK(col.is_test_included(L"Laebarindex") == false);
        // now include it and test its inclusion
        CHECK(col.include_test(L"LIX", true));
        CHECK(col.is_test_included(L"liX"));
        CHECK(col.is_test_included(L"Lix TEST"));
        CHECK(col.is_test_included(L"LaebarIndex"));

        CHECK(col.include_test(L"RIX", true));
        CHECK(col.is_test_included(L"riX"));
        CHECK(col.is_test_included(L"Rix TEST"));
        CHECK(col.is_test_included(L"Rate indEX"));
        CHECK(col.include_test(L"RIX", false));
        CHECK(col.is_test_included(L"riX") == false);
        CHECK(col.is_test_included(L"Rix TEST") == false);
        CHECK(col.is_test_included(L"Rate indEX") == false);
        // bogus text that is not in there
        CHECK(col.include_test(L"boffo", true) == false); // have to add it before including it
        CHECK(col.is_test_included(L"boffo") == false);
        }
    SECTION("Find")
        {
        readability::readability_test_collection<readability::readability_project_test<std::vector<double>>> col;
        readability::readability_test RIX(L"rix", 9, L"Rix test", L"Rate index", L"Rate index is great", readability::readability_test_type::grade_level, false, L"");
        readability::readability_test LIX(L"lix", 8, L"Lix test", L"Laebarindex", L"This is the lix test", readability::readability_test_type::grade_level, false, L"");
        readability::readability_test ARI(L"ari", 10, L"ARI test", L"ARI long name", L"ARI is great", readability::readability_test_type::grade_level, false, L"");
        col.add_test(RIX);
        col.add_test(LIX);
        col.add_test(ARI);
        CHECK(col.find_test(L"liX").second);
        CHECK(col.find_test(L"liX").first->get_test().get_id() == L"lix");
        CHECK(col.find_test(L"liX").first->get_test().get_interface_id() == 8);
        CHECK(col.find_test(L"Lix TEST").second);
        CHECK(col.find_test(L"LaebarIndex").second);
        CHECK(col.find_test(L"rIx").second);
        CHECK(col.find_test(L"rIx").first->get_test().get_id() == L"rix");
        CHECK(col.find_test(L"rIx").first->get_test().get_interface_id() == 9);
        CHECK(col.find_test(L"ari").second);
        CHECK(col.find_test(L"ARI").first->get_test().get_id() == L"ari");
        CHECK(col.find_test(L"ARI").first->get_test().get_interface_id() == 10);
        CHECK(col.find_test(L"ARI test").second);
        CHECK(col.find_test(L"ARI test").first->get_test().get_id() == L"ari");
        // interface ID
        CHECK(col.find_test(8).first->get_test().get_id() == L"lix");
        CHECK(col.find_test(9).first->get_test().get_id() == L"rix");
        CHECK(col.find_test(10).first->get_test().get_id() == L"ari");
        CHECK(col.find_test(L"ARI long name").second);
        // items not found
        CHECK(col.find_test(L"boffo").second == false);
        CHECK(col.find_test(0).second == false);
        CHECK(col.find_test(11).second == false);
        }
    SECTION("GetFunctions")
        {
        readability::readability_test_collection<readability::readability_project_test<std::vector<double>>> col;
        readability::readability_test LIX(L"lix", 8, L"Lix test", L"Laebarindex", L"This is the lix test", readability::readability_test_type::grade_level, false, L"");
        readability::readability_test RIX(L"rix", 9, L"Rix test", L"Rate index", L"Rate index is great", readability::readability_test_type::index_value, false, L"");
        col.add_test(LIX);
        col.add_test(RIX);
        CHECK(col.get_test_id(L"lix") == L"lix");
        CHECK(col.get_test_short_name(L"lix") == L"Lix test");
        CHECK(col.get_test_long_name(L"lix") == L"Laebarindex");
        CHECK(col.get_test_description(L"lix") == L"This is the lix test");

        CHECK(col.get_test_id(L"rix") == L"rix");
        CHECK(col.get_test_short_name(L"rix") == L"Rix test");
        CHECK(col.get_test_long_name(L"rix") == L"Rate index");
        CHECK(col.get_test_description(L"rix") == L"Rate index is great");

        CHECK(col.get_test_id(L"boffo") == L"");
        CHECK(col.get_test_short_name(L"boffo") == L"");
        CHECK(col.get_test_long_name(L"") == L"");
        CHECK(col.get_test_description(L"") == L"");
        }
    SECTION("Counts")
        {
        readability::readability_test_collection<readability::readability_project_test<std::vector<double>>> col;
        readability::readability_test LIX(L"lix", 8, L"Lix test", L"Laebarindex", L"This is the lix test", readability::readability_test_type::grade_level, false, L"");
        readability::readability_test RIX(L"rix", 9, L"Rix test", L"Rate index", L"Rate index is great", readability::readability_test_type::index_value_and_grade_level, false, L"");
        readability::readability_test BOFFO(L"Boffo", 10, L"Boffo test", L"Boffo", L"Boffo is great", readability::readability_test_type::index_value, false, L"");
        col.add_test(LIX);
        col.add_test(RIX);
        col.add_test(BOFFO);
        CHECK(col.get_grade_level_test_count() == 2);
        }
    }

TEST_CASE("Base test tests", "[base-test][readability-tests]")
    {
    SECTION("Langs")
        {
        readability::readability_test lix(L"lix", 8, L"Lix test", L"Laebarindex", L"This is the lix test", readability::readability_test_type::grade_level, false, L"");
        lix.add_language(readability::test_language::english_test);
        CHECK(lix.has_language(readability::test_language::english_test));
        CHECK(lix.has_language(readability::test_language::spanish_test) == false);
        CHECK(lix.has_language(readability::test_language::german_test) == false);
        lix.reset_languages();
        CHECK(lix.has_language(readability::test_language::english_test) == false);
        CHECK(lix.has_language(readability::test_language::spanish_test) == false);
        CHECK(lix.has_language(readability::test_language::german_test) == false);
        lix.add_language(readability::test_language::spanish_test);
        CHECK(lix.has_language(readability::test_language::english_test) == false);
        CHECK(lix.has_language(readability::test_language::spanish_test));
        CHECK(lix.has_language(readability::test_language::german_test) == false);
        lix.add_language(readability::test_language::german_test);
        CHECK(lix.has_language(readability::test_language::english_test) == false);
        CHECK(lix.has_language(readability::test_language::spanish_test));
        CHECK(lix.has_language(readability::test_language::german_test));
        lix.add_language(readability::test_language::english_test);
        CHECK(lix.has_language(readability::test_language::english_test));
        CHECK(lix.has_language(readability::test_language::spanish_test));
        CHECK(lix.has_language(readability::test_language::german_test));
        }
    SECTION("Testing Classification")
        {
        readability::test_with_classification ts;
        CHECK(ts.has_teaching_level(readability::test_teaching_level::primary_grade) == false);
        CHECK(ts.has_teaching_level(readability::test_teaching_level::secondary_grade) == false);
        CHECK(ts.has_teaching_level(readability::test_teaching_level::adult_level) == false);
        CHECK(ts.has_teaching_level(readability::test_teaching_level::second_language) == false);

        ts.add_teaching_level(readability::test_teaching_level::primary_grade);
        CHECK(ts.has_teaching_level(readability::test_teaching_level::primary_grade));
        CHECK(ts.has_teaching_level(readability::test_teaching_level::secondary_grade) == false);
        CHECK(ts.has_teaching_level(readability::test_teaching_level::adult_level) == false);
        CHECK(ts.has_teaching_level(readability::test_teaching_level::second_language) == false);

        ts.add_teaching_level(readability::test_teaching_level::secondary_grade);
        CHECK(ts.has_teaching_level(readability::test_teaching_level::primary_grade));
        CHECK(ts.has_teaching_level(readability::test_teaching_level::secondary_grade));
        CHECK(ts.has_teaching_level(readability::test_teaching_level::adult_level) == false);
        CHECK(ts.has_teaching_level(readability::test_teaching_level::second_language) == false);

        ts.add_teaching_level(readability::test_teaching_level::adult_level);
        CHECK(ts.has_teaching_level(readability::test_teaching_level::primary_grade));
        CHECK(ts.has_teaching_level(readability::test_teaching_level::secondary_grade));
        CHECK(ts.has_teaching_level(readability::test_teaching_level::adult_level));
        CHECK(ts.has_teaching_level(readability::test_teaching_level::second_language) == false);

        ts.add_teaching_level(readability::test_teaching_level::second_language);
        CHECK(ts.has_teaching_level(readability::test_teaching_level::primary_grade));
        CHECK(ts.has_teaching_level(readability::test_teaching_level::secondary_grade));
        CHECK(ts.has_teaching_level(readability::test_teaching_level::adult_level));
        CHECK(ts.has_teaching_level(readability::test_teaching_level::second_language));
        }
    SECTION("Doc Classification")
        {
        readability::test_with_classification ts;
        CHECK(ts.has_document_classification(readability::document_classification::general_document) == false);
        CHECK(ts.has_document_classification(readability::document_classification::technical_document) == false);
        CHECK(ts.has_document_classification(readability::document_classification::nonnarrative_document) == false);
        CHECK(ts.has_document_classification(readability::document_classification::adult_literature_document) == false);
        CHECK(ts.has_document_classification(readability::document_classification::childrens_literature_document) == false);

        ts.add_document_classification(readability::document_classification::general_document, true);
        CHECK(ts.has_document_classification(readability::document_classification::general_document));
        CHECK(ts.has_document_classification(readability::document_classification::technical_document) == false);
        CHECK(ts.has_document_classification(readability::document_classification::nonnarrative_document) == false);
        CHECK(ts.has_document_classification(readability::document_classification::adult_literature_document) == false);
        CHECK(ts.has_document_classification(readability::document_classification::childrens_literature_document) == false);

        ts.add_document_classification(readability::document_classification::technical_document, true);
        CHECK(ts.has_document_classification(readability::document_classification::general_document));
        CHECK(ts.has_document_classification(readability::document_classification::technical_document));
        CHECK(ts.has_document_classification(readability::document_classification::nonnarrative_document) == false);
        CHECK(ts.has_document_classification(readability::document_classification::adult_literature_document) == false);
        CHECK(ts.has_document_classification(readability::document_classification::childrens_literature_document) == false);

        ts.add_document_classification(readability::document_classification::nonnarrative_document, true);
        CHECK(ts.has_document_classification(readability::document_classification::general_document));
        CHECK(ts.has_document_classification(readability::document_classification::technical_document));
        CHECK(ts.has_document_classification(readability::document_classification::nonnarrative_document));
        CHECK(ts.has_document_classification(readability::document_classification::adult_literature_document) == false);
        CHECK(ts.has_document_classification(readability::document_classification::childrens_literature_document) == false);

        ts.add_document_classification(readability::document_classification::adult_literature_document, true);
        CHECK(ts.has_document_classification(readability::document_classification::general_document));
        CHECK(ts.has_document_classification(readability::document_classification::technical_document));
        CHECK(ts.has_document_classification(readability::document_classification::nonnarrative_document));
        CHECK(ts.has_document_classification(readability::document_classification::adult_literature_document));
        CHECK(ts.has_document_classification(readability::document_classification::childrens_literature_document) == false);

        ts.add_document_classification(readability::document_classification::childrens_literature_document, true);
        CHECK(ts.has_document_classification(readability::document_classification::general_document));
        CHECK(ts.has_document_classification(readability::document_classification::technical_document));
        CHECK(ts.has_document_classification(readability::document_classification::nonnarrative_document));
        CHECK(ts.has_document_classification(readability::document_classification::adult_literature_document));
        CHECK(ts.has_document_classification(readability::document_classification::childrens_literature_document));

        ts.add_document_classification(readability::document_classification::childrens_literature_document, false);
        CHECK(ts.has_document_classification(readability::document_classification::childrens_literature_document) == false);
        }
    SECTION("Industry Classification")
        {
        readability::test_with_classification ts;
        CHECK(ts.has_industry_classification(readability::industry_classification::childrens_publishing_industry) == false);
        CHECK(ts.has_industry_classification(readability::industry_classification::adult_publishing_industry) == false);
        CHECK(ts.has_industry_classification(readability::industry_classification::childrens_healthcare_industry) == false);
        CHECK(ts.has_industry_classification(readability::industry_classification::adult_healthcare_industry) == false);
        CHECK(ts.has_industry_classification(readability::industry_classification::military_government_industry) == false);
        CHECK(ts.has_industry_classification(readability::industry_classification::secondary_language_industry) == false);

        ts.add_industry_classification(readability::industry_classification::childrens_publishing_industry, true);
        CHECK(ts.has_industry_classification(readability::industry_classification::childrens_publishing_industry));
        CHECK(ts.has_industry_classification(readability::industry_classification::adult_publishing_industry) == false);
        CHECK(ts.has_industry_classification(readability::industry_classification::childrens_healthcare_industry) == false);
        CHECK(ts.has_industry_classification(readability::industry_classification::adult_healthcare_industry) == false);
        CHECK(ts.has_industry_classification(readability::industry_classification::military_government_industry) == false);
        CHECK(ts.has_industry_classification(readability::industry_classification::secondary_language_industry) == false);

        ts.add_industry_classification(readability::industry_classification::adult_publishing_industry, true);
        CHECK(ts.has_industry_classification(readability::industry_classification::childrens_publishing_industry));
        CHECK(ts.has_industry_classification(readability::industry_classification::adult_publishing_industry));
        CHECK(ts.has_industry_classification(readability::industry_classification::childrens_healthcare_industry) == false);
        CHECK(ts.has_industry_classification(readability::industry_classification::adult_healthcare_industry) == false);
        CHECK(ts.has_industry_classification(readability::industry_classification::military_government_industry) == false);
        CHECK(ts.has_industry_classification(readability::industry_classification::secondary_language_industry) == false);

        ts.add_industry_classification(readability::industry_classification::childrens_healthcare_industry, true);
        CHECK(ts.has_industry_classification(readability::industry_classification::childrens_publishing_industry));
        CHECK(ts.has_industry_classification(readability::industry_classification::adult_publishing_industry));
        CHECK(ts.has_industry_classification(readability::industry_classification::childrens_healthcare_industry));
        CHECK(ts.has_industry_classification(readability::industry_classification::adult_healthcare_industry) == false);
        CHECK(ts.has_industry_classification(readability::industry_classification::military_government_industry) == false);
        CHECK(ts.has_industry_classification(readability::industry_classification::secondary_language_industry) == false);

        ts.add_industry_classification(readability::industry_classification::adult_healthcare_industry, true);
        CHECK(ts.has_industry_classification(readability::industry_classification::childrens_publishing_industry));
        CHECK(ts.has_industry_classification(readability::industry_classification::adult_publishing_industry));
        CHECK(ts.has_industry_classification(readability::industry_classification::childrens_healthcare_industry));
        CHECK(ts.has_industry_classification(readability::industry_classification::adult_healthcare_industry));
        CHECK(ts.has_industry_classification(readability::industry_classification::military_government_industry) == false);
        CHECK(ts.has_industry_classification(readability::industry_classification::secondary_language_industry) == false);

        ts.add_industry_classification(readability::industry_classification::military_government_industry, true);
        CHECK(ts.has_industry_classification(readability::industry_classification::childrens_publishing_industry));
        CHECK(ts.has_industry_classification(readability::industry_classification::adult_publishing_industry));
        CHECK(ts.has_industry_classification(readability::industry_classification::childrens_healthcare_industry));
        CHECK(ts.has_industry_classification(readability::industry_classification::adult_healthcare_industry));
        CHECK(ts.has_industry_classification(readability::industry_classification::military_government_industry));
        CHECK(ts.has_industry_classification(readability::industry_classification::secondary_language_industry) == false);

        ts.add_industry_classification(readability::industry_classification::secondary_language_industry, true);
        CHECK(ts.has_industry_classification(readability::industry_classification::childrens_publishing_industry));
        CHECK(ts.has_industry_classification(readability::industry_classification::adult_publishing_industry));
        CHECK(ts.has_industry_classification(readability::industry_classification::childrens_healthcare_industry));
        CHECK(ts.has_industry_classification(readability::industry_classification::adult_healthcare_industry));
        CHECK(ts.has_industry_classification(readability::industry_classification::military_government_industry));
        CHECK(ts.has_industry_classification(readability::industry_classification::secondary_language_industry));

        ts.add_industry_classification(readability::industry_classification::secondary_language_industry, false);
        CHECK(ts.has_industry_classification(readability::industry_classification::secondary_language_industry) == false);
        }
    }

TEST_CASE("Rix tests", "[rix][readability-tests]")
    {
    SECTION("RIX")
        {
        size_t grade_level;
        CHECK_THAT(readability::rix(grade_level, 70206, 48544), WithinRel(1.4, 1e-1));
        CHECK_THAT(readability::rix(grade_level, 33373, 10514), WithinRel(3.2, 1e-1));
        CHECK_THAT(readability::rix(grade_level, 221, 78), WithinRel(2.8, 1e-1));
        }
    SECTION("Grade Levels")
        {
        size_t grade_level;
        [[maybe_unused]] auto b = readability::rix(grade_level, 10, 150);
        CHECK(grade_level == 1);

        b = readability::rix(grade_level, 30, 100);
        CHECK(grade_level == 2);

        b = readability::rix(grade_level, 70, 100);
        CHECK(grade_level == 3);

        b = readability::rix(grade_level, 80, 78);
        CHECK(grade_level == 4);

        b = readability::rix(grade_level, 70206, 48544);
        CHECK(grade_level == 5);

        b = readability::rix(grade_level, 170, 78);
        CHECK(grade_level == 6);

        b = readability::rix(grade_level, 221, 78);
        CHECK(grade_level == 7);

        b = readability::rix(grade_level, 33373, 10514);
        CHECK(grade_level == 8);

        b = readability::rix(grade_level, 300, 78);
        CHECK(grade_level == 9);

        b = readability::rix(grade_level, 400, 85);
        CHECK(grade_level == 10);

        b = readability::rix(grade_level, 420, 78);
        CHECK(grade_level == 11);

        b = readability::rix(grade_level, 500, 78);
        CHECK(grade_level == 12);

        b = readability::rix(grade_level, 600, 78);
        CHECK(grade_level == 13);
        }
    SECTION("Exceptions")
        {
        size_t grade_level;
        CHECK_THROWS(readability::rix(grade_level, 945, 0));
        }
    }

TEST_CASE("Flesch tests", "[flesch][readability-tests]")
    {
    SECTION("Difficulty Levels")
        {
        for (size_t i = 0; i < 30; ++i)
            { CHECK(flesch_difficulty::flesch_very_difficult == flesch_score_to_difficulty_level(i)); }
        for (size_t i = 30; i < 49; ++i)
            { CHECK(flesch_difficulty::flesch_difficult == flesch_score_to_difficulty_level(i)); }
        for (size_t i = 50; i < 59; ++i)
            { CHECK(flesch_difficulty::flesch_fairly_difficult == flesch_score_to_difficulty_level(i)); }
        for (size_t i = 60; i < 69; ++i)
            { CHECK(flesch_difficulty::flesch_standard == flesch_score_to_difficulty_level(i)); }
        for (size_t i = 70; i < 79; ++i)
            { CHECK(flesch_difficulty::flesch_fairly_easy == flesch_score_to_difficulty_level(i)); }
        for (size_t i = 80; i < 89; ++i)
            { CHECK(flesch_difficulty::flesch_easy == flesch_score_to_difficulty_level(i)); }
        for (size_t i = 90; i < 100; ++i)
            { CHECK(flesch_difficulty::flesch_very_easy == flesch_score_to_difficulty_level(i)); }
        }
    SECTION("FleschRange")
        {
        readability::flesch_difficulty level;
        CHECK(100 == readability::flesch_reading_ease(100, 100, 100, level));
        CHECK(0 == readability::flesch_reading_ease(100, 1000, 1, level));
        }
    SECTION("Flesch")
        {
        readability::flesch_difficulty level;
        CHECK(74 == readability::flesch_reading_ease(4770, 7020, 550, level));
        CHECK(74 == readability::flesch_reading_ease(4770 * 2, 7020 * 2, 550 * 2, level));
        }
    SECTION("Flesch 2")
        {
        readability::flesch_difficulty level;
        CHECK(36 == readability::flesch_reading_ease(213, 339, 6, level));
        CHECK(36 == readability::flesch_reading_ease(213 * 2, 339 * 2, 6 * 2, level));
        }
    SECTION("Flesch Exceptions 1")
        {
        readability::flesch_difficulty level;
        CHECK_THROWS(readability::flesch_reading_ease(0, 550, 7020, level));
        }
    SECTION("Flesch Exceptions 2")
        {
        readability::flesch_difficulty level;
        CHECK_THROWS(readability::flesch_reading_ease(4770, 550, 0, level));
        }
    }

TEST_CASE("Lix German tests", "[lix][german][readability-tests]")
    {
    SECTION("Lix German Technical Documents Conversion")
        {
        CHECK(readability::lix_index_to_german_technical_literature_grade_level(0) == 3);
        CHECK(readability::lix_index_to_german_technical_literature_grade_level(30) == 3);
        CHECK(readability::lix_index_to_german_technical_literature_grade_level(31) == 4);
        CHECK(readability::lix_index_to_german_technical_literature_grade_level(33) == 4);
        CHECK(readability::lix_index_to_german_technical_literature_grade_level(34) == 5);
        CHECK(readability::lix_index_to_german_technical_literature_grade_level(37) == 5);
        CHECK(readability::lix_index_to_german_technical_literature_grade_level(38) == 6);
        CHECK(readability::lix_index_to_german_technical_literature_grade_level(40) == 6);
        CHECK(readability::lix_index_to_german_technical_literature_grade_level(41) == 7);
        CHECK(readability::lix_index_to_german_technical_literature_grade_level(43) == 7);
        CHECK(readability::lix_index_to_german_technical_literature_grade_level(44) == 8);
        CHECK(readability::lix_index_to_german_technical_literature_grade_level(47) == 8);
        CHECK(readability::lix_index_to_german_technical_literature_grade_level(48) == 9);
        CHECK(readability::lix_index_to_german_technical_literature_grade_level(50) == 9);
        CHECK(readability::lix_index_to_german_technical_literature_grade_level(51) == 10);
        CHECK(readability::lix_index_to_german_technical_literature_grade_level(53) == 10);
        CHECK(readability::lix_index_to_german_technical_literature_grade_level(54) == 11);
        CHECK(readability::lix_index_to_german_technical_literature_grade_level(56) == 11);
        CHECK(readability::lix_index_to_german_technical_literature_grade_level(57) == 12);
        CHECK(readability::lix_index_to_german_technical_literature_grade_level(59) == 12);
        CHECK(readability::lix_index_to_german_technical_literature_grade_level(60) == 13);
        CHECK(readability::lix_index_to_german_technical_literature_grade_level(63) == 13);
        CHECK(readability::lix_index_to_german_technical_literature_grade_level(64) == 14);
        CHECK(readability::lix_index_to_german_technical_literature_grade_level(69) == 14);
        CHECK(readability::lix_index_to_german_technical_literature_grade_level(70) == 15);
        CHECK(readability::lix_index_to_german_technical_literature_grade_level(101) == 15);
        }
    SECTION("Lix German Childrens Books Conversion")
        {
        CHECK(readability::lix_index_to_german_childrens_literature_grade_level(7) == 1);
        CHECK(readability::lix_index_to_german_childrens_literature_grade_level(19) == 1);
        CHECK(readability::lix_index_to_german_childrens_literature_grade_level(23) == 1);
        CHECK(readability::lix_index_to_german_childrens_literature_grade_level(24) == 2);
        CHECK(readability::lix_index_to_german_childrens_literature_grade_level(26) == 2);
        CHECK(readability::lix_index_to_german_childrens_literature_grade_level(27) == 3);
        CHECK(readability::lix_index_to_german_childrens_literature_grade_level(29) == 3);
        CHECK(readability::lix_index_to_german_childrens_literature_grade_level(30) == 4);
        CHECK(readability::lix_index_to_german_childrens_literature_grade_level(31) == 4);
        CHECK(readability::lix_index_to_german_childrens_literature_grade_level(32) == 5);
        CHECK(readability::lix_index_to_german_childrens_literature_grade_level(33) == 5);
        CHECK(readability::lix_index_to_german_childrens_literature_grade_level(34) == 6);
        CHECK(readability::lix_index_to_german_childrens_literature_grade_level(35) == 6);
        CHECK(readability::lix_index_to_german_childrens_literature_grade_level(36) == 7);
        CHECK(readability::lix_index_to_german_childrens_literature_grade_level(37) == 7);
        CHECK(readability::lix_index_to_german_childrens_literature_grade_level(38) == 8);
        CHECK(readability::lix_index_to_german_childrens_literature_grade_level(70) == 8);
        }
    SECTION("Difficulty Levels")
        {
        for (size_t i = 0; i < 29; ++i)
            { CHECK(readability::german_lix_difficulty::german_lix_very_easy == german_lix_index_to_difficulty_level(i)); }
        for (size_t i = 30; i < 34; ++i)
            { CHECK(readability::german_lix_difficulty::german_lix_children_and_youth == german_lix_index_to_difficulty_level(i)); }
        for (size_t i = 35; i < 29; ++i)
            { CHECK(readability::german_lix_difficulty::german_lix_easy == german_lix_index_to_difficulty_level(i)); }
        for (size_t i = 40; i < 44; ++i)
            { CHECK(readability::german_lix_difficulty::german_lix_adult_fiction == german_lix_index_to_difficulty_level(i)); }
        for (size_t i = 45; i < 49; ++i)
            { CHECK(readability::german_lix_difficulty::german_lix_average == german_lix_index_to_difficulty_level(i)); }
        for (size_t i = 50; i < 54; ++i)
            { CHECK(readability::german_lix_difficulty::german_lix_nonfiction == german_lix_index_to_difficulty_level(i)); }
        for (size_t i = 55; i < 59; ++i)
            { CHECK(readability::german_lix_difficulty::german_lix_difficult == german_lix_index_to_difficulty_level(i)); }
        for (size_t i = 60; i < 64; ++i)
            { CHECK(readability::german_lix_difficulty::german_lix_technical == german_lix_index_to_difficulty_level(i)); }
        for (size_t i = 65; i < 100; ++i)
            { CHECK(readability::german_lix_difficulty::german_lix_very_difficult == german_lix_index_to_difficulty_level(i)); }
        }

    SECTION("German Lix")
        {
        // should return the same index value as regular Lix
        german_lix_difficulty diffLevel;
        CHECK(24 == readability::german_lix(diffLevel, 490582, 70206, 48544));
        CHECK(38 == readability::german_lix(diffLevel, 128409, 33373, 10514));
        CHECK(36 == readability::german_lix(diffLevel, 945, 221, 78));
        }
    }

TEST_CASE("Rix German tests", "[rix][german][readability-tests]")
    {
    SECTION("Rix German Non Fiction Conversion")
        {
        CHECK(readability::rix_index_to_german_nonfiction_grade_level(1) == 4);
        CHECK(readability::rix_index_to_german_nonfiction_grade_level(20) == 4);
        CHECK(readability::rix_index_to_german_nonfiction_grade_level(26) == 4);
        CHECK(readability::rix_index_to_german_nonfiction_grade_level(26.1) == 5);
        CHECK(readability::rix_index_to_german_nonfiction_grade_level(32) == 5);
        CHECK(readability::rix_index_to_german_nonfiction_grade_level(32.1) == 6);
        CHECK(readability::rix_index_to_german_nonfiction_grade_level(38) == 6);
        CHECK(readability::rix_index_to_german_nonfiction_grade_level(38.1) == 7);
        CHECK(readability::rix_index_to_german_nonfiction_grade_level(45) == 7);
        CHECK(readability::rix_index_to_german_nonfiction_grade_level(45.1) == 8);
        CHECK(readability::rix_index_to_german_nonfiction_grade_level(52) == 8);
        CHECK(readability::rix_index_to_german_nonfiction_grade_level(52.1) == 9);
        CHECK(readability::rix_index_to_german_nonfiction_grade_level(57) == 9);
        CHECK(readability::rix_index_to_german_nonfiction_grade_level(57.1) == 10);
        CHECK(readability::rix_index_to_german_nonfiction_grade_level(66) == 10);
        CHECK(readability::rix_index_to_german_nonfiction_grade_level(66.1) == 11);
        CHECK(readability::rix_index_to_german_nonfiction_grade_level(75) == 11);
        CHECK(readability::rix_index_to_german_nonfiction_grade_level(75.1) == 12);
        CHECK(readability::rix_index_to_german_nonfiction_grade_level(84) == 12);
        CHECK(readability::rix_index_to_german_nonfiction_grade_level(84.1) == 13);
        CHECK(readability::rix_index_to_german_nonfiction_grade_level(100) == 13);
        CHECK(readability::rix_index_to_german_nonfiction_grade_level(100.1) == 14);
        CHECK(readability::rix_index_to_german_nonfiction_grade_level(200) == 14);
        }
    SECTION("Rix German Fiction Conversion")
        {
        CHECK(readability::rix_index_to_german_fiction_grade_level(7) == 1);
        CHECK(readability::rix_index_to_german_fiction_grade_level(9) == 1);
        CHECK(readability::rix_index_to_german_fiction_grade_level(13.5) == 1);
        CHECK(readability::rix_index_to_german_fiction_grade_level(13.51) == 2);
        CHECK(readability::rix_index_to_german_fiction_grade_level(17) == 2);
        CHECK(readability::rix_index_to_german_fiction_grade_level(17.1) == 3);
        CHECK(readability::rix_index_to_german_fiction_grade_level(20.5) == 3);
        CHECK(readability::rix_index_to_german_fiction_grade_level(20.51) == 4);
        CHECK(readability::rix_index_to_german_fiction_grade_level(24) == 4);
        CHECK(readability::rix_index_to_german_fiction_grade_level(24.1) == 5);
        CHECK(readability::rix_index_to_german_fiction_grade_level(27.5) == 5);
        CHECK(readability::rix_index_to_german_fiction_grade_level(27.51) == 6);
        CHECK(readability::rix_index_to_german_fiction_grade_level(31) == 6);
        CHECK(readability::rix_index_to_german_fiction_grade_level(31.1) == 7);
        CHECK(readability::rix_index_to_german_fiction_grade_level(34.5) == 7);
        CHECK(readability::rix_index_to_german_fiction_grade_level(34.51) == 8);
        CHECK(readability::rix_index_to_german_fiction_grade_level(38) == 8);
        CHECK(readability::rix_index_to_german_fiction_grade_level(38.1) == 9);
        CHECK(readability::rix_index_to_german_fiction_grade_level(41.5) == 9);
        CHECK(readability::rix_index_to_german_fiction_grade_level(41.51) == 10);
        CHECK(readability::rix_index_to_german_fiction_grade_level(45) == 10);
        CHECK(readability::rix_index_to_german_fiction_grade_level(45.1) == 11);
        CHECK(readability::rix_index_to_german_fiction_grade_level(300) == 11);
        }
    SECTION("Rix German Exceptions")
        {
        CHECK_THROWS(readability::rix_bamberger_vanecek(0, 100, 5));
        CHECK_THROWS(readability::rix_bamberger_vanecek(500, 100, 0));
        }
    }

TEST_CASE("Sol tests", "[sol][readability-tests]")
    {
    SECTION("SolRange")
        {
        CHECK_THAT(0, WithinRel(readability::sol_spanish(0, 10), 1e-1));
        CHECK_THAT(19, WithinRel(readability::sol_spanish(440, 10), 1e-2));
        }
    SECTION("Sol")
        {
        //15->8.59 (precision is a little different from article because
        // difficult to get exactly 15 with high-precision smog)
        CHECK_THAT(8.6, WithinRel(readability::sol_spanish(130, 30), 1e-1));
        //25->15.99
        CHECK_THAT(15.99, WithinRel(readability::sol_spanish(440, 30), 1e-2));
        }
    SECTION("Sol Exceptions")
        {
        CHECK_THROWS(readability::sol_spanish(520, 0));
        }
    }

TEST_CASE("FORCAST tests", "[forcast][readability-tests]")
    {
    SECTION("FORCAST")
        {
        CHECK_THAT(10.4, WithinRel(readability::forcast(150, 96), 1e-1)); // from the article
        CHECK_THAT(12.1, WithinRel(readability::forcast(150, 79), 1e-1)); // from the secondary article
        CHECK_THAT(9.1, WithinRel(readability::forcast(150, 109), 1e-1));
        CHECK_THAT(9.1, WithinRel(readability::forcast(300, 218), 1e-1));
        CHECK_THAT(9.8, WithinRel(readability::forcast(4770, 3230), 1e-1));
        }
    SECTION("FORCAST Range")
        {
        //5 is as low as this formula can logically get without throwing garbage into it
        CHECK_THAT(5, WithinRel(readability::forcast(150, 150), 1e-1));
        CHECK_THAT(19, WithinRel(readability::forcast(150, 0), 1e-1));
        }
    SECTION("FORCAST Exceptions")
        {
        CHECK_THROWS(readability::forcast(0, 109));
        }
    }

TEST_CASE("Lix tests", "[lix][readability-tests]")
    {
    SECTION("LIX Indices")
        {
        CHECK(readability::lix_index_to_difficulty_level(5) == lix_difficulty::lix_very_easy);
        CHECK(readability::lix_index_to_difficulty_level(21) == lix_difficulty::lix_very_easy);
        CHECK(readability::lix_index_to_difficulty_level(25) == lix_difficulty::lix_very_easy);
        CHECK(readability::lix_index_to_difficulty_level(29) == lix_difficulty::lix_very_easy);
        CHECK(readability::lix_index_to_difficulty_level(30) == lix_difficulty::lix_easy);
        CHECK(readability::lix_index_to_difficulty_level(34) == lix_difficulty::lix_easy);
        CHECK(readability::lix_index_to_difficulty_level(35) == lix_difficulty::lix_easy);
        CHECK(readability::lix_index_to_difficulty_level(39) == lix_difficulty::lix_easy);
        CHECK(readability::lix_index_to_difficulty_level(40) == lix_difficulty::lix_average);
        CHECK(readability::lix_index_to_difficulty_level(44) == lix_difficulty::lix_average);
        CHECK(readability::lix_index_to_difficulty_level(45) == lix_difficulty::lix_average);
        CHECK(readability::lix_index_to_difficulty_level(49) == lix_difficulty::lix_average);
        CHECK(readability::lix_index_to_difficulty_level(50) == lix_difficulty::lix_difficult);
        CHECK(readability::lix_index_to_difficulty_level(54) == lix_difficulty::lix_difficult);
        CHECK(readability::lix_index_to_difficulty_level(55) == lix_difficulty::lix_difficult);
        CHECK(readability::lix_index_to_difficulty_level(59) == lix_difficulty::lix_difficult);
        CHECK(readability::lix_index_to_difficulty_level(60) == lix_difficulty::lix_very_difficult);
        CHECK(readability::lix_index_to_difficulty_level(99) == lix_difficulty::lix_very_difficult);
        }
    SECTION("LIX")
        {
        size_t grade_level;
        readability::lix_difficulty diffLevel;
        CHECK(24 == readability::lix(diffLevel, grade_level, 490582, 70206, 48544));
        CHECK(lix_difficulty::lix_very_easy == diffLevel);
        CHECK(38 == readability::lix(diffLevel, grade_level, 128409, 33373, 10514));
        CHECK(lix_difficulty::lix_easy == diffLevel);

        CHECK(readability::lix(diffLevel, grade_level, 945, 221, 78) == 36);
        CHECK(diffLevel == lix_difficulty::lix_easy);
        }
    SECTION("LIXGradeLevels")
        {
        size_t grade_level;
        readability::lix_difficulty diffLevel;

        [[maybe_unused]] auto res = readability::lix(diffLevel, grade_level, 945, 10, 150);
        CHECK(grade_level == 1);

        res = readability::lix(diffLevel, grade_level, 945, 10, 100);
        CHECK(grade_level == 2);

        res = readability::lix(diffLevel, grade_level, 945, 70, 100);
        CHECK(grade_level == 3);

        res = readability::lix(diffLevel, grade_level, 945, 80, 78);
        CHECK(grade_level == 4);

        res = readability::lix(diffLevel, grade_level, 490582, 70206, 48544);
        CHECK(grade_level == 5);

        res = readability::lix(diffLevel, grade_level, 945, 170, 78);
        CHECK(grade_level == 6);

        res = readability::lix(diffLevel, grade_level, 955, 221, 78);
        CHECK(grade_level == 7);

        res = readability::lix(diffLevel, grade_level, 128409, 33373, 10514);
        CHECK(grade_level == 8);

        res = readability::lix(diffLevel, grade_level, 965, 300, 78);
        CHECK(grade_level == 9);

        res = readability::lix(diffLevel, grade_level, 945, 320, 85);
        CHECK(grade_level == 10);

        res = readability::lix(diffLevel, grade_level, 945, 350, 78);
        CHECK(grade_level == 11);

        res = readability::lix(diffLevel, grade_level, 945, 400, 78);
        CHECK(grade_level == 12);

        res = readability::lix(diffLevel, grade_level, 945, 500, 78);
        CHECK(grade_level == 13);
        }
    SECTION("LIX Exceptions 1")
        {
        size_t grade_level;
        readability::lix_difficulty diffLevel;
        CHECK_THROWS(readability::lix(diffLevel, grade_level, 0, 221, 78));
        }
    SECTION("LIX Exceptions 2")
        {
        size_t grade_level;
        readability::lix_difficulty diffLevel;
        CHECK_THROWS(readability::lix(diffLevel, grade_level, 945, 224, 0));
        }
    }

TEST_CASE("Wheeler Smith German tests", "[wheeler-smith][readability-tests]")
    {
    SECTION("WheelerSmithRangeHelperTest")
        {
        double score;
        CHECK(readability::wheeler_smith_get_score_from_range(4.8, 2.5, 6, 1, score));
        CHECK_THAT(1.6, WithinRel(score, 1e-4));
        }
    SECTION("WheelerSmithRange")
        {
        double indexScore;
        CHECK_THAT(1, WithinRel(readability::wheeler_smith_bamberger_vanecek(100, 0, 50, indexScore), 1e-4));
        CHECK_THAT(10.9, WithinRel(readability::wheeler_smith_bamberger_vanecek(100, 100, 2, indexScore), 1e-4));
        }
    SECTION("WheelerSmithTest")
        {
        double indexScore;
        CHECK_THAT(1, WithinRel(readability::wheeler_smith_bamberger_vanecek(1000, 28, 108, indexScore), 1e-4));
        CHECK_THAT(2.5, WithinRel(indexScore, 1e-4));

        CHECK_THAT(1.6, WithinRel(readability::wheeler_smith_bamberger_vanecek(1000, 58, 119, indexScore), 1e-4));
        CHECK_THAT(4.8, WithinRel(indexScore, 1e-4));

        //book says 4.3, but really 4.4
        CHECK_THAT(1.5, WithinRel(readability::wheeler_smith_bamberger_vanecek(1000, 53, 120, indexScore), 1e-4));
        CHECK_THAT(4.4, WithinRel(indexScore, 1e-4));

        CHECK_THAT(2.6, WithinRel(readability::wheeler_smith_bamberger_vanecek(1000, 82, 105, indexScore), 1e-4));
        CHECK_THAT(7.8, WithinRel(indexScore, 1e-4));

        //book says 6.5 and 2.2, but should be 6.4 and 2.1. You would only get the numbers
        //they report if the index score was rounded, but they truncate everywhere else.
        //it must be a typo in the book.
        CHECK_THAT(2.1, WithinRel(readability::wheeler_smith_bamberger_vanecek(1000, 72, 111, indexScore), 1e-4));
        CHECK_THAT(6.4, WithinRel(indexScore, 1e-4));

        CHECK_THAT(3.4, WithinRel(readability::wheeler_smith_bamberger_vanecek(1000, 110, 105, indexScore), 1e-4));
        CHECK_THAT(10.4, WithinRel(indexScore, 1e-4));

        //book says 9.2, but really 9.3
        CHECK_THAT(3.1, WithinRel(readability::wheeler_smith_bamberger_vanecek(1000, 85, 91, indexScore), 1e-4));
        CHECK_THAT(9.3, WithinRel(indexScore, 1e-4));

        //book says 4.8 score and index score of 15.5, but a simple (12*13)/10
        //of course yields 15.6.  Obviously a math error in the book, we will test against
        //the correct numbers
        CHECK_THAT(4.9, WithinRel(readability::wheeler_smith_bamberger_vanecek(1000, 130, 83, indexScore), 1e-4));
        CHECK_THAT(15.6, WithinRel(indexScore, 1e-4));

        CHECK_THAT(4.1, WithinRel(readability::wheeler_smith_bamberger_vanecek(1000, 120, 95, indexScore), 1e-4));
        CHECK_THAT(12.6, WithinRel(indexScore, 1e-4));

        CHECK_THAT(5.7, WithinRel(readability::wheeler_smith_bamberger_vanecek(1000, 150, 80, indexScore), 1e-4));
        CHECK_THAT(18.7, WithinRel(indexScore, 1e-4));

        //The book reports a score of 5.2 and 16.5, but that's not even close.
        //(11.8*13)/10 is 15.3, not 16.5.  The book is just egregiously wrong on this example,
        //so we will just use the correct numbers here.
        CHECK_THAT(4.8, WithinRel(readability::wheeler_smith_bamberger_vanecek(10000, 1300, 847, indexScore), 1e-4));
        CHECK_THAT(15.3, WithinRel(indexScore, 1e-4));

        //book says 6.3, but should be 6.4: (21.7-20.1 = 1.6; 1.6/4(the range) = .4; 6+.4=6.4)
        CHECK_THAT(6.4, WithinRel(readability::wheeler_smith_bamberger_vanecek(1000, 150, 69, indexScore), 1e-4));
        CHECK_THAT(21.7, WithinRel(indexScore, 1e-4));

        //book says 6.6, but should be 6.7: (23-20.1=2.9; 2.9/4 = .725; 6+7.25 (round or truncated) = 6.7
        CHECK_THAT(6.7, WithinRel(readability::wheeler_smith_bamberger_vanecek(1000000, 130000, 56497, indexScore), 1e-4));
        CHECK_THAT(23, WithinRel(indexScore, 1e-4));
        }
    SECTION("Wheeler Smith Test Exceptions 1")
        {
        double index;
        CHECK_THROWS(readability::wheeler_smith_bamberger_vanecek(0, 20, 12, index));
        }
    SECTION("Wheeler Smith Test Exceptions 2")
        {
        double index;
        CHECK_THROWS(readability::wheeler_smith_bamberger_vanecek(10, 10, 0, index));
        }
    }

TEST_CASE("PSK tests", "[psk][readability-tests]")
    {
    SECTION("PSKTest")
        {
        double grade = readability::powers_sumner_kearl_flesch(4770, 7020, 550);
        CHECK_THAT(5.2, WithinRel(grade, 1e-1));
        }
    SECTION("PSKFleschRangeTest")
        {
        CHECK_THAT(0, WithinRel(readability::powers_sumner_kearl_flesch(100,0,100), 1e-1));
        CHECK_THAT(19, WithinRel(readability::powers_sumner_kearl_flesch(100,1000,2), 1e-1));
        }
    SECTION("PSKDCRangeTest")
        {
        CHECK_THAT(3.3, WithinRel(readability::powers_sumner_kearl_dale_chall(1000,0,1000), 1e-1));//equation's bare min result is 3.26
        CHECK_THAT(19, WithinRel(readability::powers_sumner_kearl_dale_chall(100,1000,2), 1e-1));
        }
    SECTION("PSK Test Exceptions 1")
        {
        CHECK_THROWS(readability::powers_sumner_kearl_flesch(0, 550, 7020));
        }
    SECTION("PSK Test Exceptions 2")
        {
        CHECK_THROWS(readability::powers_sumner_kearl_flesch(4770, 550, 0));
        }
    SECTION("PSK DC Test Exceptions 1")
        {
        CHECK_THROWS(readability::powers_sumner_kearl_dale_chall(0, 550, 7020));
        }
    SECTION("PSKTDCestExceptions2")
        {
        CHECK_THROWS(readability::powers_sumner_kearl_dale_chall(4770, 5, 0));
        }
    }

TEST_CASE("DRP tests", "[drp][readability-tests]")
    {
    SECTION("Drp Ge Range")
        {
        CHECK_THAT(1.5, WithinRel(readability::degrees_of_reading_power_ge(100, 100, 100, 100), 1e-1));//as low as equation can go
        CHECK_THAT(18, WithinRel(readability::degrees_of_reading_power_ge(100,0,10000,1), 1e-1));
        }
    SECTION("Drp Ge Exceptions")
        {
        CHECK_THROWS(readability::degrees_of_reading_power_ge(0,1,1,1));
        CHECK_THROWS(readability::degrees_of_reading_power_ge(1,1,1,0));
        }
    SECTION("Drp Range")
        {
        CHECK_THAT(5.5, WithinRel(readability::degrees_of_reading_power(100, 100, 100, 100), 1e-1));//5.5 is as low as you can locially go
        CHECK_THAT(100, WithinRel(readability::degrees_of_reading_power(100,0,2000,1), 1e-1));
        }
    SECTION("Drp Exceptions")
        {
        CHECK_THROWS(readability::degrees_of_reading_power(0,1,1,1));
        CHECK_THROWS(readability::degrees_of_reading_power(1,1,1,0));
        }
    SECTION("Conversions")
        {
        CHECK_THAT(readability::degrees_of_reading_power_to_ge(40), WithinRel(1.6, 1e-2));
        CHECK_THAT(readability::degrees_of_reading_power_to_ge(44), WithinRel(2.0, 1e-2));
        CHECK_THAT(readability::degrees_of_reading_power_to_ge(49), WithinRel(3.0, 1e-2));
        CHECK_THAT(readability::degrees_of_reading_power_to_ge(51), WithinRel(3.6, 1e-2));
        CHECK_THAT(readability::degrees_of_reading_power_to_ge(53), WithinRel(4.3, 1e-2));
        CHECK_THAT(readability::degrees_of_reading_power_to_ge(56), WithinRel(5.5, 1e-2));
        CHECK_THAT(readability::degrees_of_reading_power_to_ge(57), WithinRel(5.9, 1e-2));
        CHECK_THAT(readability::degrees_of_reading_power_to_ge(58), WithinRel(6.3, 1e-2));
        CHECK_THAT(readability::degrees_of_reading_power_to_ge(59), WithinRel(6.8, 1e-2));
        CHECK_THAT(readability::degrees_of_reading_power_to_ge(60), WithinRel(7.3, 1e-2));
        CHECK_THAT(readability::degrees_of_reading_power_to_ge(61), WithinRel(7.8, 1e-2));
        CHECK_THAT(readability::degrees_of_reading_power_to_ge(64), WithinRel(9.4, 1e-2));
        }
    }

TEST_CASE("Fog tests", "[fog][readability-tests]")
    {
    SECTION("FOG")
        {
        CHECK_THAT(readability::gunning_fog(129, 26, 10), WithinRel(13.2, 1e-1));
        CHECK_THAT(readability::gunning_fog(102, 9, 6), WithinRel(10.3, 1e-1));
        CHECK_THAT(readability::gunning_fog(102, 13, 4), WithinRel(15.3, 1e-1));
        CHECK_THAT(readability::gunning_fog(104, 1, 14), WithinRel(3.4, 1e-1));
        CHECK_THAT(readability::gunning_fog(101, 6, 6), WithinRel(9.1, 1e-1));
        CHECK_THAT(readability::gunning_fog(94, 2, 6), WithinRel(7.1, 1e-1));
        }
    SECTION("FOG Range")
        {
        CHECK_THAT(std::floor(readability::gunning_fog(100, 0, 100)), WithinRel(0, 1e-1));
        CHECK_THAT(readability::gunning_fog(100, 100, 2), WithinRel(19, 1e-1));
        }
    SECTION("FOG Exceptions1")
        {
        CHECK_THROWS(readability::gunning_fog(0, 6, 9));
        }
    SECTION("FOG Exceptions2")
        {
        CHECK_THROWS(readability::gunning_fog(102, 6, 0));
        }
    SECTION("Psk FOG Exceptions")
        {
        CHECK_THROWS(readability::psk_gunning_fog(0, 6, 5));
        CHECK_THROWS(readability::psk_gunning_fog(5, 6, 0));
        }
    SECTION("New FOG Exceptions")
        {
        CHECK_THROWS(readability::new_fog_count(0, 6, 5));
        CHECK_THROWS(readability::new_fog_count(5, 6, 0));
        }
    SECTION("Psk FOG Range")
        {
        // as low as it can go
        CHECK_THAT(readability::psk_gunning_fog(100, 0, 100), WithinRel(3.1, 1e-1));
        CHECK_THAT(readability::psk_gunning_fog(100, 100, 1), WithinRel(19, 1e-1));
        }
    SECTION("New FOG Range")
        {
        CHECK_THAT(readability::new_fog_count(100, 0, 100), WithinRel(0, 1e-1));
        CHECK_THAT(readability::new_fog_count(100, 100, 2), WithinRel(19, 1e-1));
        }
    SECTION("HardWords")
        {
        CHECK(readability::is_easy_gunning_fog_word(L"reallybigword", 4) == false);
        }
    SECTION("Easy Words")
        {
        CHECK(readability::is_easy_gunning_fog_word(L"simple", 2));
        CHECK(readability::is_easy_gunning_fog_word(L"easy", 2));
        // from the book
        CHECK(readability::is_easy_gunning_fog_word(L"created", 3));
        CHECK(readability::is_easy_gunning_fog_word(L"trespasses", 3));
        // es rules
        CHECK(readability::is_easy_gunning_fog_word(L"ashashes", 3));
        CHECK(readability::is_easy_gunning_fog_word(L"araces", 3));
        CHECK(readability::is_easy_gunning_fog_word(L"enrages", 3));
        CHECK(readability::is_easy_gunning_fog_word(L"ahexes", 3));
        CHECK(readability::is_easy_gunning_fog_word(L"amazes", 3));
        CHECK(readability::is_easy_gunning_fog_word(L"azases", 3));
        CHECK(readability::is_easy_gunning_fog_word(L"amazases", 4) == false);//4 syllables fail
                                                                                 //ed rule
        CHECK(readability::is_easy_gunning_fog_word(L"ahated", 3));
        CHECK(readability::is_easy_gunning_fog_word(L"ahaded", 3));
        // 4 syllables fail
        CHECK(readability::is_easy_gunning_fog_word(L"adahaded", 4) == false);
        }
    SECTION("Hyphenated Words")
        {
        // 1 and 2 syllables, should be easy
        CHECK(readability::is_easy_gunning_fog_word(L"high-roller", 3));
        // 1, 1, and 2 syllables, should be easy
        CHECK(readability::is_easy_gunning_fog_word(L"rock-of-ages", 4));
        // 3 and 2 syllables, should be hard
        CHECK(readability::is_easy_gunning_fog_word(L"highroller-man", 4) == false);
        // 2 and 3 syllables, should be hard
        CHECK(readability::is_easy_gunning_fog_word(L"man-highroller", 4) == false);
        }
    SECTION("Hyphenated Words Stray Hyphens")
        {
        // 1 and 2 syllables, should be easy
        CHECK(readability::is_easy_gunning_fog_word(L"high-roller-", 3));
        // 1 and 2 syllables, should be easy
        CHECK(readability::is_easy_gunning_fog_word(L"-high-roller", 3));
        }
    SECTION("Slashed Words")
        {
        // 1 and 2 syllables, should be easy
        CHECK(readability::is_easy_gunning_fog_word(L"high/roller", 3));
        // 1, 1, and 2 syllables, should be easy
        CHECK(readability::is_easy_gunning_fog_word(L"rock/of-ages", 4));
        // 3 and 2 syllables, should be hard
        CHECK(readability::is_easy_gunning_fog_word(L"highroller/man", 4) == false);
        // 2 and 3 syllables, should be hard
        CHECK(readability::is_easy_gunning_fog_word(L"man/highroller", 4) == false);
        }
    SECTION("Slashed Words Stray Hyphens")
        {
        // 1 and 2 syllables, should be easy
        CHECK(readability::is_easy_gunning_fog_word(L"high/roller/", 3));
        // 1 and 2 syllables, should be easy
        CHECK(readability::is_easy_gunning_fog_word(L"/high/roller", 3));
        }
    }

TEST_CASE("nWS tests", "[nWS][readability-tests]")
    {
   SECTION("nWS1 Range")
        {
        CHECK_THAT(1, WithinRel(readability::neue_wiener_sachtextformel_1(100,100,0,0,50), 1e-1));
        CHECK_THAT(19, WithinRel(readability::neue_wiener_sachtextformel_1(100,0,100,50,2), 1e-1));
        }
   SECTION("nWS1 Exceptions 1")
        {
        CHECK_THROWS(readability::neue_wiener_sachtextformel_1(0, 51, 23, 28, 8));
        }
   SECTION("nWS1 Exceptions 2")
        {
        CHECK_THROWS(readability::neue_wiener_sachtextformel_1(100, 51, 23, 28, 0));
        }
   SECTION("nWS2 Range")
        {
        CHECK_THAT(1, WithinRel(readability::neue_wiener_sachtextformel_2(100,0,0,50), 1e-1));
        CHECK_THAT(19, WithinRel(readability::neue_wiener_sachtextformel_2(100,100,100,2), 1e-1));
        }
   SECTION("nWS2 Exceptions 1")
        {
        CHECK_THROWS(readability::neue_wiener_sachtextformel_2(0, 51, 23, 8));
        }
   SECTION("nWS2 Exceptions 2")
        {   
        CHECK_THROWS(readability::neue_wiener_sachtextformel_2(100, 51, 23, 0));
        }
   SECTION("nWS3 Range")
        {
        CHECK_THAT(1, WithinRel(readability::neue_wiener_sachtextformel_3(100,0,50), 1e-1));
        CHECK_THAT(19, WithinRel(readability::neue_wiener_sachtextformel_3(100,100,2), 1e-1));
        }
   SECTION("nWS3 Exceptions 1")
        {
        CHECK_THROWS(readability::neue_wiener_sachtextformel_3(0, 51, 8));
        }
   SECTION("nWS3 Exceptions 2")
        {
        CHECK_THROWS(readability::neue_wiener_sachtextformel_3(100, 51, 0));
        }
    }

TEST_CASE("Coleman Liau tests", "[coleman-liau][readability-tests]")
    {
    SECTION("Coleman Liau Range")
        {
        double predScore;
        CHECK_THAT(0, WithinRel(std::floor(readability::coleman_liau(100, 100, 50, predScore)), 1e-1));
        CHECK_THAT(1.0, WithinRel(predScore, 1e-1));
        CHECK_THAT(19, WithinRel(std::floor(readability::coleman_liau(100, 10000, 2, predScore)), 1e-1));
        CHECK_THAT(-1.0, WithinRel(predScore, 1e-1));
        }
    SECTION("ColemanLiauExceptions1")
        {
        double predScore;
        CHECK_THROWS(readability::coleman_liau(0, 114, 550, predScore));
        }
    SECTION("ColemanLiauExceptions2")
        {
        double predScore;
        CHECK_THROWS(readability::coleman_liau(100, 114, 0, predScore));
        }
    }

TEST_CASE("ELF tests", "[elf][readability-tests]")
    {
    SECTION("ELF Range")
        {
        CHECK_THAT(0, WithinRel(readability::easy_listening_formula(100, 100, 50), 1e-2));
        CHECK_THAT(19, WithinRel(readability::easy_listening_formula(100, 1000, 2), 1e-2));
        }
    SECTION("ELF")
        {
        CHECK_THAT(2, WithinRel(readability::easy_listening_formula(9, 11, 1), 1e-2));
        CHECK_THAT(5, WithinRel(readability::easy_listening_formula(10, 15, 1), 1e-2));
        CHECK_THAT(3.5, WithinRel(readability::easy_listening_formula(19, 26, 2), 1e-2));
        }
    }

TEST_CASE("WS tests", "[ws][readability-tests]")
    {
    SECTION("WheelerSmithTest")
        {
        double index;
        // from article
        CHECK(2 == readability::wheeler_smith(106, 20, 12, index));
        CHECK_THAT(16.7, WithinRel(index, 1e-1));
        }
    SECTION("WheelerSmithTestExceptions1")
        {
        double index;
        CHECK_THROWS(readability::wheeler_smith(0, 20, 12, index));
        }
    SECTION("WheelerSmithTestExceptions2")
        {
        double index;
        CHECK_THROWS(readability::wheeler_smith(10, 10, 0, index));
        }
    }

TEST_CASE("DB tests", "[db][readability-tests]")
    {
    SECTION("DB1Range")
        {
        CHECK_THAT(0, WithinRel(readability::danielson_bryan_1(200, 100, 100), 1e-1));
        CHECK_THAT(19, WithinRel(readability::danielson_bryan_1(100, 1000, 1), 1e-1));
        }
    SECTION("DB2Range")
        {
        CHECK_THAT(100, WithinRel(readability::danielson_bryan_2(100, 200, 100), 1e-1));
        CHECK_THAT(0, WithinRel(readability::danielson_bryan_2(100, 1000, 1), 1e-1));
        }
    SECTION("DB1")
        {
        CHECK_THAT(6.1f, WithinRel(readability::danielson_bryan_1(270, 1164, 10), 1e-1));
        }
    SECTION("DB2")
        {
        //published score is 63.9, but this was hand calculated to be 63.79. Must have been a rounding issue
        //with the original article.
        CHECK_THAT(63.8f, WithinRel(readability::danielson_bryan_2(270, 1164, 10), 1e-1));
        }
    }

TEST_CASE("Test helpers tests", "[readability-tests]")
    {
    SECTION("Grade Truncate")
        {
        CHECK_THAT(readability::truncate_k12_plus_grade(19.1), WithinRel(19.0, 1e-1));
        CHECK_THAT(readability::truncate_k12_plus_grade(-1.5), WithinRel(0, 1e-1));
        CHECK_THAT(readability::truncate_k12_plus_grade(0), WithinRel(0, 1e-1));
        CHECK_THAT(readability::truncate_k12_plus_grade(0.1), WithinRel(0.1, 1e-1));
        CHECK_THAT(readability::truncate_k12_plus_grade(5.8), WithinRel(5.8, 1e-1));
        CHECK_THAT(readability::truncate_k12_plus_grade(17.6), WithinRel(17.6, 1e-1));
        CHECK_THAT(readability::truncate_k12_plus_grade(12.0), WithinRel(12.0, 1e-1));
        }
    SECTION("Grade Split")
        {
        size_t grade, month;
        readability::split_k12_plus_grade_score(19.1, grade, month);
        CHECK(grade == 19);
        CHECK(month == 0); // 19.1 should be truncated to 19

        readability::split_k12_plus_grade_score(12.0, grade, month);
        CHECK(grade == 12);
        CHECK(month == 0);

        readability::split_k12_plus_grade_score(17.8, grade, month);
        CHECK(grade == 17);
        CHECK(month == 8);

        readability::split_k12_plus_grade_score(-1, grade, month);
        CHECK(grade == 0);
        CHECK(month == 0);

        readability::split_k12_plus_grade_score(0, grade, month);
        CHECK(grade == 0);
        CHECK(month == 0);

        readability::split_k12_plus_grade_score(8.9, grade, month);
        CHECK(grade == 8);
        CHECK(month == 9);

        readability::split_k12_plus_grade_score(3.1, grade, month);
        CHECK(grade == 3);
        CHECK(month == 1);

        readability::split_k12_plus_grade_score(5.5, grade, month);
        CHECK(grade == 5);
        CHECK(month == 5);
        }
    }

TEST_CASE("Spache tests", "[spache][readability-tests]")
    {
    SECTION("Spache")
        {
        CHECK_THAT(1.9, WithinRel(readability::spache(4770, 114, 550), 1e-1));
        }
    SECTION("SpacheRange")
        {
        CHECK_THAT(0, WithinRel(std::floor(readability::spache(100, 0, 500)), 1e-1));//.659 will always be added in formula
        CHECK_THAT(19, WithinRel(readability::spache(100, 100, 1), 1e-1));
        }
    SECTION("SpacheExceptions1")
        {
        CHECK_THROWS(readability::spache(0, 114, 550));
        }
    SECTION("SpacheExceptions2")
        {
        CHECK_THROWS(readability::spache(4770, 114, 0));
        }
    }

TEST_CASE("Eflaw tests", "[eflaw][readability-tests]")
    {
    SECTION("Eflaw")
        {
        for (size_t i = 0; i < 21; ++i)
            {CHECK(eflaw_difficulty::eflaw_very_easy == eflaw_index_to_difficulty(i)); }
        for (size_t i = 21; i < 26; ++i)
            {CHECK(eflaw_difficulty::eflaw_easy == eflaw_index_to_difficulty(i)); }
        for (size_t i = 26; i < 30; ++i)
            {CHECK(eflaw_difficulty::eflaw_confusing == eflaw_index_to_difficulty(i)); }
        for (size_t i = 30; i < 50; ++i)
            {CHECK(eflaw_difficulty::eflaw_very_confusing == eflaw_index_to_difficulty(i)); }
        }
    SECTION("Eflaw Exceptions")
        {
        readability::eflaw_difficulty diff;
        CHECK_THROWS(readability::eflaw(diff, 1,1,0));
        }
    }

TEST_CASE("ARI tests", "[ari][readability-tests]")
    {
    SECTION("ARI")
        {
        CHECK_THAT(10.7, WithinRel(readability::automated_readability_index(282, 1396, 16), 1e-1));
        }
    SECTION("ARIRange")
        {
        CHECK_THAT(0, WithinRel(readability::automated_readability_index(100,100,50), 1e-1));
        CHECK_THAT(19, WithinRel(readability::automated_readability_index(100,1000,2), 1e-1));
        }
    SECTION("ARIExceptions1")
        {
        CHECK_THROWS(readability::automated_readability_index(0, 16, 1396));
        }
    SECTION("ARIExceptions2")
        {
        CHECK_THROWS(readability::automated_readability_index(282, 16, 0));
        }
    }

TEST_CASE("New ARI tests", "[ari][readability-tests]")
    {
    SECTION("NewARIRange")
        {
        CHECK_THAT(0, WithinRel(readability::new_automated_readability_index(100,100,50), 1e-1));
        CHECK_THAT(19, WithinRel(readability::new_automated_readability_index(100,1000,2), 1e-1));
        }
    SECTION("NewARIExceptions1")
        {
        CHECK_THROWS(readability::new_automated_readability_index(0, 16, 1396));
        }
    SECTION("NewARIExceptions2")
        {
        CHECK_THROWS(readability::new_automated_readability_index(282, 16, 0));
        }
    }

TEST_CASE("Simple ARI tests", "[ari][readability-tests]")
    {
    SECTION("SimpleARIRange")
        {
        CHECK_THAT(0, WithinRel(readability::simplified_automated_readability_index(100,100,50), 1e-1));
        CHECK_THAT(19, WithinRel(readability::simplified_automated_readability_index(100,1000,2), 1e-1));
        }
    SECTION("SimpleARIExceptions1")
        {
        CHECK_THROWS(readability::simplified_automated_readability_index(0, 16, 1396));
        }
    SECTION("SimpleARIExceptions2")
        {
        CHECK_THROWS(readability::simplified_automated_readability_index(282, 16, 0));
        }
    }

TEST_CASE("FJP tests", "[fjp][readability-tests]")
    {
    SECTION("FJP")
        {
        readability::flesch_difficulty difflevel;
        CHECK(57 == farr_jenkins_paterson(100, 71, 4, difflevel));
        }
    SECTION("FJPRange")
        {
        readability::flesch_difficulty difflevel;
        CHECK(100 == farr_jenkins_paterson(100, 100, 50, difflevel));
        CHECK(0 == farr_jenkins_paterson(100, 0, 2, difflevel));
        }
    SECTION("FJPExceptions1")
        {
        readability::flesch_difficulty difflevel;
        CHECK_THROWS(readability::farr_jenkins_paterson(0, 71, 4, difflevel));
        }
    SECTION("FJPExceptions2")
        {
        readability::flesch_difficulty difflevel;
        CHECK_THROWS(readability::farr_jenkins_paterson(100, 71, 0, difflevel));
        }
    SECTION("NewFJPRange")
        {
        CHECK_THAT(0, WithinRel(readability::new_farr_jenkins_paterson(100, 100, 50), 1e-1));
        CHECK_THAT(19,WithinRel( readability::new_farr_jenkins_paterson(100, 0, 2), 1e-1));
        }
    SECTION("NewFJPExceptions1")
        {
        CHECK_THROWS(readability::new_farr_jenkins_paterson(0, 71, 4));
        }
    SECTION("NewFJPExceptions2")
        {
        CHECK_THROWS(readability::new_farr_jenkins_paterson(100, 71, 0));
        }
    SECTION("PskFJPRange")
        {
        // won't go below 2 in the equation unless you through in nonsensical values
        CHECK_THAT(2, WithinRel(std::floor(readability::powers_sumner_kearl_farr_jenkins_paterson(100, 100, 100)), 1e-1));
        CHECK_THAT(19, WithinRel(readability::powers_sumner_kearl_farr_jenkins_paterson(1000, 0, 1), 1e-1));
        }
    SECTION("PskFJPExceptions1")
        {
        CHECK_THROWS(readability::powers_sumner_kearl_farr_jenkins_paterson(0, 71, 4));
        }
    SECTION("PskFJPExceptions2")
        {
        CHECK_THROWS(readability::powers_sumner_kearl_farr_jenkins_paterson(100, 71, 0));
        }
    }

TEST_CASE("NDC tests", "[ndc][readability-tests]")
    {
    SECTION("DC")
        {
        size_t begin{ 0 }, end{ 0 };
        // extreme examples
        readability::new_dale_chall(begin, end, 100, 44, 50);
        CHECK(13 == begin);
        CHECK(15 == end);

        readability::new_dale_chall(begin, end, 100, 44, 51);
        CHECK(16 == begin);
        CHECK(16 == end);

        // examples from the book
        readability::new_dale_chall(begin, end, 60, 0, 6);
        CHECK(1 == begin);
        CHECK(1 == end);

        readability::new_dale_chall(begin, end, 100, 3, 12);
        CHECK(2 == begin);
        CHECK(2 == end);

        readability::new_dale_chall(begin, end, 100, 3, 8);
        CHECK(3 == begin);
        CHECK(3 == end);

        readability::new_dale_chall(begin, end, 100, 8, 9);
        CHECK(4 == begin);
        CHECK(4 == end);

        readability::new_dale_chall(begin, end, 100, 11, 6);
        CHECK(5 == begin);
        CHECK(6 == end);

        readability::new_dale_chall(begin, end, 100, 19, 7);
        CHECK(7 == begin);
        CHECK(8 == end);

        readability::new_dale_chall(begin, end, 100, 23, 5);
        CHECK(9 == begin);
        CHECK(10 == end);

        readability::new_dale_chall(begin, end, 100, 23, 4);
        CHECK(11 == begin);
        CHECK(12 == end);

        readability::new_dale_chall(begin, end, 100, 35, 5);
        CHECK(13 == begin);
        CHECK(15 == end);

        readability::new_dale_chall(begin, end, 100, 37, 2);
        CHECK(16 == begin);
        CHECK(16 == end);
        }
    SECTION("DC Exceptions")
        {
        size_t begin, end;
        CHECK_THROWS(readability::new_dale_chall(begin, end, 0, 0, 10));
        CHECK_THROWS(readability::new_dale_chall(begin, end, 10, 0, 0));
        }
    }

TEST_CASE("Crawford tests", "[crawford][readability-tests]")
    {
    SECTION("CrawfordRange")
        {
        CHECK_THAT(0, WithinRel(readability::crawford(100, 100, 50), 1e-1));
        CHECK_THAT(19, WithinRel(readability::crawford(100, 600, 1),1e-1));
        }
    SECTION("Crawford")
        {
        // example from article
        CHECK_THAT(3.7, WithinRel(readability::crawford(100, 175, 7), 1e-1));
        CHECK_THAT(3.7, WithinRel(readability::crawford(200, 350, 14), 1e-1));
        }
    SECTION("Exception")
        {
        CHECK_THROWS(readability::crawford(0, 51, 69));
        }
    }

TEST_CASE("Bormuth tests", "[bormuth][readability-tests]")
    {
    SECTION("Bormuth 35 Range")
        {
        CHECK_THAT(2.8, WithinRel(readability::bormuth_grade_placement_35(100, 100, 100, 100), 1e-1));//can't go any lower with sensical values
        CHECK_THAT(19, WithinRel(readability::bormuth_grade_placement_35(100,0,10000,1), 1e-1));
        }
    SECTION("Bormuth 35 Exceptions")
        {
        CHECK_THROWS(readability::bormuth_grade_placement_35(0,1,1,1));
        CHECK_THROWS(readability::bormuth_grade_placement_35(1,1,1,0));
        }
    SECTION("Bormuth Cloze Range")
        {
        CHECK_THAT(1, WithinRel(readability::bormuth_cloze_mean(100, 100, 100, 100), 1e-1));
        CHECK_THAT(-1, WithinRel(readability::bormuth_cloze_mean(100,0,1000,1), 1e-1));
        }
    SECTION("Bormuth Cloze Exceptions")
        {
        CHECK_THROWS(readability::bormuth_cloze_mean(0,1,1,1));
        CHECK_THROWS(readability::bormuth_cloze_mean(1,1,1,0));
        }
    }

TEST_CASE("HJ tests", "[hj][readability-tests]")
    {
    SECTION("HJ")
        { CHECK_THAT(2.9, WithinRel(readability::harris_jacobson(207, 8, 24), 1e-1)); }
    SECTION("HJ 2")
        { CHECK_THAT(3.3, WithinRel(readability::harris_jacobson(205, 10, 22), 1e-1)); }
    SECTION("Exceptions 1")
        { CHECK_THROWS(readability::harris_jacobson(0, 1, 7020)); }
    SECTION("Exceptions 2")
        { CHECK_THROWS(readability::harris_jacobson(10, 1, 0)); }
    SECTION("Range Truncation")
        {
        // too easy, bump up to 1.1
        CHECK_THAT(1, WithinRel(readability::harris_jacobson(300, 1, 150), 1e-1));
        // too hard, clip to 11.3
        CHECK_THAT(11.3, WithinRel(readability::harris_jacobson(10, 10, 1), 1e-1));
        }
    }

TEST_CASE("Smog German tests", "[smog][readability-tests]")
    {
    SECTION("Smog Range")
        {
        CHECK_THAT(0.0, WithinRel(readability::smog_bamberger_vanecek(1, 30), 1e-6));
        CHECK_THAT(19.0, WithinRel(readability::smog_bamberger_vanecek(1000, 30), 1e-6));
        }
    SECTION("Smog")
        {
        CHECK_THAT(2.3, WithinRel(readability::smog_bamberger_vanecek(19, 30), 1e-6));
        CHECK_THAT(2.3, WithinRel(readability::smog_bamberger_vanecek(19 * 2, 30 * 2), 1e-6));
        CHECK_THAT(4.6, WithinRel(readability::smog_bamberger_vanecek(44, 30), 1e-6));
        CHECK_THAT(4.6, WithinRel(readability::smog_bamberger_vanecek(44 * 2, 30 * 2), 1e-6));
        CHECK_THAT(4.6, WithinRel(readability::smog_bamberger_vanecek(static_cast<size_t>(44 * .5), static_cast<size_t>(30 * .5)), 1e-6));
        }
    SECTION("Smog Exceptions")
        {
        CHECK_THROWS(readability::smog_bamberger_vanecek(550, 0));
        }
    }

TEST_CASE("Qu tests", "[qu][readability-tests]")
    {
    SECTION("Qu")
        {
        CHECK_THAT(5.8, WithinRel(readability::quadratwurzelverfahren_bamberger_vanecek(1000, 140, 68), 1e-6));
        CHECK_THAT(3.1, WithinRel(readability::quadratwurzelverfahren_bamberger_vanecek(1000, 83, 93), 1e-6));
        CHECK_THAT(4.8, WithinRel(readability::quadratwurzelverfahren_bamberger_vanecek(1000, 130, 83), 1e-6));
        CHECK_THAT(8.3, WithinRel(readability::quadratwurzelverfahren_bamberger_vanecek(1000, 200, 56), 1e-6));
        }
    SECTION("Qu Range")
        {
        CHECK_THAT(0, WithinRel(readability::quadratwurzelverfahren_bamberger_vanecek(100,0,100), 1e-6));
        CHECK_THAT(19, WithinRel(readability::quadratwurzelverfahren_bamberger_vanecek(100,100,2), 1e-6));
        }
    SECTION("Qu Exceptions")
        {
        CHECK_THROWS(readability::quadratwurzelverfahren_bamberger_vanecek(0, 550, 7020));
        CHECK_THROWS(readability::quadratwurzelverfahren_bamberger_vanecek(10, 550, 0));
        }
    }

TEST_CASE("Amstad tests", "[amstad][readability-tests]")
    {
    SECTION("Amstad Range")
        {
        readability::flesch_difficulty level;
        CHECK(100 == readability::amstad(100, 100, 50, level));
        CHECK(0 == readability::amstad(100, 1000, 2, level));
        }
    SECTION("Amstad")
        {
        readability::flesch_difficulty level;
        CHECK(readability::amstad(100, 135, 50, level) == 99);
        CHECK(readability::amstad(100, 220, 2, level) == 1);
        CHECK(readability::amstad(100, 185, 5, level) == 52);
        // from the book (we round it, though, book uses precision which
        // isn't really necessary for a Flesch scale).
        CHECK(readability::amstad(100 * 2, 135 * 2, 50 * 2, level) == 99);
        CHECK(readability::amstad(100 * 2, 220 * 2, 2 * 2, level) == 1);
        CHECK(readability::amstad(100 * 2, 185 * 2, 5 * 2, level) == 52);
        }
    SECTION("Amstad Exceptions 1")
        {
        readability::flesch_difficulty level;
        CHECK_THROWS(readability::amstad(0, 550, 7020, level));
        }
    SECTION("Amstad Exceptions 2")
        {
        readability::flesch_difficulty level;
        CHECK_THROWS(readability::amstad(4770, 550, 0, level));
        }
    }

TEST_CASE("Flesch Kincaid tests", "[flesch-kincaid][readability-tests]")
    {
    SECTION("Flesch Kincaid")
        {
        CHECK_THAT(5.2, WithinRel(readability::flesch_kincaid(4770, 7020, 550), 1e-1));
        }
    SECTION("Flesch Kincaid Range")
        {
        CHECK_THAT(0, WithinRel(readability::flesch_kincaid(100, 100, 50), 1e-1));
        CHECK_THAT(19, WithinRel(readability::flesch_kincaid(100, 1000, 2), 1e-1));
        CHECK_THAT(0, WithinRel(readability::flesch_kincaid_simplified(100, 100, 50), 1e-1));
        CHECK_THAT(19, WithinRel(readability::flesch_kincaid_simplified(100, 1000, 2), 1e-1));
        }
    SECTION("Flesch KincaidE xceptions 1")
        {
        CHECK_THROWS(readability::flesch_kincaid(0, 550, 7020));
        CHECK_THROWS(readability::flesch_kincaid_simplified(0, 550, 7020));
        }
    SECTION("Flesch Kincaid Exceptions 2")
        {
        CHECK_THROWS(readability::flesch_kincaid(4770, 550, 0));
        CHECK_THROWS(readability::flesch_kincaid_simplified(4770, 550, 0));
        }
    }

TEST_CASE("Smog tests", "[smog][readability-tests]")
    {
    SECTION("SMOG Range")
        {
        // Smog has min values of 3
        CHECK_THAT(3, WithinRel(readability::smog_simplified(0, 30), 1e-1));
        CHECK_THAT(19, WithinRel(readability::smog_simplified(1000, 30), 1e-1));
        CHECK_THAT(3.1291, WithinRel(readability::smog(0, 30), 1e-1));
        CHECK_THAT(19, WithinRel(readability::smog(1000, 30), 1e-1));
        }
    SECTION("SMOG EN")
        {
        CHECK_THAT(11.0, WithinRel(readability::smog_simplified(64, 30), 1e-1));
        // From article
        CHECK_THAT(18, WithinRel(readability::smog_simplified(244, 30), 1e-1));
        CHECK_THAT(16, WithinRel(readability::smog_simplified(169, 30), 1e-1));
        CHECK_THAT(15, WithinRel(readability::smog_simplified(150, 30), 1e-1));
        CHECK_THAT(15, WithinRel(readability::smog_simplified(147, 30), 1e-1));
        CHECK_THAT(15, WithinRel(readability::smog_simplified(145, 30), 1e-1));
        CHECK_THAT(13, WithinRel(readability::smog_simplified(112, 30), 1e-1));
        CHECK_THAT(11, WithinRel(readability::smog_simplified(72, 30), 1e-1));
        CHECK_THAT(9, WithinRel(readability::smog_simplified(39, 30), 1e-1));
        CHECK_THAT(8.0, WithinRel(readability::smog_simplified(520, 550), 1e-1));
        }
    // these results aren't published, but are verified
    // to be close to results from the generalized formula
    SECTION("SMOG Precision")
        {
        CHECK_THAT(16.6, WithinRel(readability::smog(28, 5), 1e-1));
        CHECK_THAT(14.2, WithinRel(readability::smog(112, 30), 1e-1));
        CHECK_THAT(14.2, WithinRel(readability::smog(224, 60), 1e-1));
        CHECK_THAT(11.9, WithinRel(readability::smog(72, 30), 1e-1));
        CHECK_THAT(19.0, WithinRel(readability::smog(244, 30), 1e-1));
        CHECK_THAT(9.6, WithinRel(readability::smog(39, 30), 1e-1));
        }
    SECTION("SMOG Modified")
        {
        // From article
        CHECK(readability::modified_smog(0, 30) == 1);
        CHECK(readability::modified_smog(1, 30) == 1);
        CHECK(readability::modified_smog(2, 30) == 1);
        CHECK(readability::modified_smog(3, 30) == 2);
        CHECK(readability::modified_smog(6, 30) == 2);
        CHECK(readability::modified_smog(7, 30) == 3);
        CHECK(readability::modified_smog(12, 30) == 3);
        CHECK(readability::modified_smog(13, 30) == 5);
        CHECK(readability::modified_smog(20, 30) == 5);
        CHECK(readability::modified_smog(21, 30) == 6);
        CHECK(readability::modified_smog(30, 30) == 6);
        CHECK(readability::modified_smog(31, 30) == 7);
        CHECK(readability::modified_smog(42, 30) == 7);
        CHECK(readability::modified_smog(43, 30) == 8);
        CHECK(readability::modified_smog(56, 30) == 8);
        CHECK(readability::modified_smog(57, 30) == 9);
        CHECK(readability::modified_smog(72, 30) == 9);
        CHECK(readability::modified_smog(73, 30) == 10);
        CHECK(readability::modified_smog(90, 30) == 10);
        CHECK(readability::modified_smog(91, 30) == 11);
        CHECK(readability::modified_smog(110, 30) == 11);
        CHECK(readability::modified_smog(111, 30) == 12);
        CHECK(readability::modified_smog(132, 30) == 12);
        CHECK(readability::modified_smog(133, 30) == 13);
        CHECK(readability::modified_smog(156, 30) == 13);

        CHECK(readability::modified_smog(0, 60) == 1);
        CHECK(readability::modified_smog(2, 60) == 1);
        CHECK(readability::modified_smog(4, 60) == 1);
        CHECK(readability::modified_smog(6, 60) == 2);
        CHECK(readability::modified_smog(12, 60) == 2);
        CHECK(readability::modified_smog(14, 60) == 3);
        CHECK(readability::modified_smog(24, 60) == 3);
        CHECK(readability::modified_smog(26, 60) == 5);
        CHECK(readability::modified_smog(40, 60) == 5);
        CHECK(readability::modified_smog(42, 60) == 6);
        CHECK(readability::modified_smog(60, 60) == 6);
        CHECK(readability::modified_smog(62, 60) == 7);
        CHECK(readability::modified_smog(84, 60) == 7);
        CHECK(readability::modified_smog(86, 60) == 8);
        CHECK(readability::modified_smog(112, 60) == 8);
        CHECK(readability::modified_smog(114, 60) == 9);
        CHECK(readability::modified_smog(144, 60) == 9);
        CHECK(readability::modified_smog(146, 60) == 10);
        CHECK(readability::modified_smog(180, 60) == 10);
        CHECK(readability::modified_smog(182, 60) == 11);
        CHECK(readability::modified_smog(220, 60) == 11);
        CHECK(readability::modified_smog(222, 60) == 12);
        CHECK(readability::modified_smog(234, 60) == 12);
        CHECK(readability::modified_smog(266, 60) == 13);
        CHECK(readability::modified_smog(312, 60) == 13);
        }
    SECTION("SMOG EN Exceptions 1")
        {
        CHECK_THROWS(readability::smog(520, 0));
        CHECK_THROWS(readability::modified_smog(520, 0));
        }
    }
// NOLINTEND
// clang-format on
