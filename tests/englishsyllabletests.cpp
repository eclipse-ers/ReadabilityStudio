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
#include "../src/indexing/syllable.h"
#include "../src/indexing/word.h"
#include "../src/indexing/word_functional.h"
#include "read_dictionaries.h"

// clang-format off
// NOLINTBEGIN

using namespace grammar;

TEST_CASE("English syllabizer baselines", "[syllable]")
    {
    english_syllabize syllabize;
    const auto [loaded, engSyllableBaselineStr] = read_utf8_file("EnglishSyllableBaseline.txt");
    size_t baselinePos{ 0 };
    std::wstring toFixLater;
    std::wstring passingStrings;
    std::wstring_view engSyllableBaseline{ engSyllableBaselineStr };
    while (baselinePos != std::wstring::npos)
        {
        size_t previousStart = baselinePos;
        baselinePos = engSyllableBaseline.find(L'\n', baselinePos);
        if (baselinePos == std::wstring::npos)
            { break; }
        const auto currentLine =
            engSyllableBaseline.substr(previousStart, (baselinePos - previousStart));
        auto tabPos = currentLine.find(L'\t');
        if (tabPos == std::wstring::npos)
            {
            ++baselinePos;
            continue;
            }
        const auto currentWord = currentLine.substr(0, tabPos);
        const auto currentSyllableCount =
            std::wcstod(currentLine.substr(++tabPos).data(), nullptr);
        if (const auto result = syllabize(currentWord.data(), currentWord.length());
            result != currentSyllableCount)
            {
            toFixLater += std::wstring{ currentWord } + L'\t' +
                std::to_wstring(static_cast<int>(currentSyllableCount)) + L'\t' +
                std::to_wstring(static_cast<int>(result)) + L'\n';
            }
        else
            {
            passingStrings += std::wstring{ currentWord } + L'\t' +
                std::to_wstring(static_cast<int>(currentSyllableCount)) + L'\n';
            }
        CHECK(syllabize(currentWord.data(), currentWord.length()) == currentSyllableCount);
        ++baselinePos;
        }
    std::wofstream passing("Passing.txt");
    passing << passingStrings;

    std::wofstream failing("Failing.txt");
    failing << toFixLater;
    }

TEST_CASE("English syllabizer", "[syllable]")
    {
    SECTION("Silent E And Number")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"base64", 6) == 3);
        CHECK(syllabize(L"base8", 5) == 2);
        CHECK(syllabize(L"e64", 3) == 3);
        CHECK(syllabize(L"e6", 2) == 2);
        CHECK(syllabize(L"se64", 4) == 3);
    
        CHECK(syllabize(L"BASE64", 6) == 3);
        CHECK(syllabize(L"BASE8", 5) == 2);
        CHECK(syllabize(L"E64", 3) == 3);
        CHECK(syllabize(L"E6", 2) == 2);
        CHECK(syllabize(L"SE64", 4) == 3);
        }
    SECTION("Business")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"busily", 6) == 3);
        CHECK(syllabize(L"business", 8) == 2);
        CHECK(syllabize(L"nonbusiness", 11) == 3);
        CHECK(syllabize(L"businesses", 10) == 3);

        CHECK(syllabize(L"BUSILY", 6) == 3);
        CHECK(syllabize(L"BUSINESS", 8) == 2);
        CHECK(syllabize(L"NONBUSINESS", 11) == 3);
        CHECK(syllabize(L"BUSINESSES", 10) == 3);
        }
    SECTION("SIAL")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"controversial", 13) == 4);
        CHECK(syllabize(L"controversialist", 16) == 5);
        CHECK(syllabize(L"carnassial", 10) == 4);

        CHECK(syllabize(L"CONTROVERSIAL", 13) == 4);
        CHECK(syllabize(L"CONTROVERSIALIST", 16) == 5);
        CHECK(syllabize(L"CARNASSIAL", 10) == 4);
        }
    SECTION("TIAN")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"dietitian", 9) == 4);
        CHECK(syllabize(L"alsatian", 8) == 3);
        CHECK(syllabize(L"alsatians", 9) == 3);
        CHECK(syllabize(L"christianity", 12) == 5);
        CHECK(syllabize(L"negotiant", 9) == 4);

        CHECK(syllabize(L"DIETITIAN", 9) == 4);
        CHECK(syllabize(L"ALSATIAN", 8) == 3);
        CHECK(syllabize(L"ALSATIANS", 9) == 3);
        CHECK(syllabize(L"CHRISTIANITY", 12) == 5);
        CHECK(syllabize(L"NEGOTIANT", 9) == 4);
        }
    SECTION("Middle E Not Silent")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"harlequin", 9) == 3);
        CHECK(syllabize(L"harlequins", 10) == 3);
        CHECK(syllabize(L"algebra", 7) == 3);

        CHECK(syllabize(L"ALGEBRA", 7) == 3);
        CHECK(syllabize(L"HARLEQUIN", 9) == 3);
        CHECK(syllabize(L"HARLEQUINS", 10) == 3);
        }
    SECTION("EO One Syllable")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"georg", 5) == 1);
        CHECK(syllabize(L"george", 6) == 1);
        CHECK(syllabize(L"georgette", 9) == 2);
        CHECK(syllabize(L"geoff", 5) == 1);

        CHECK(syllabize(L"GEORG", 5) == 1);
        CHECK(syllabize(L"GEORGE", 6) == 1);
        CHECK(syllabize(L"GEORGETTE", 9) == 2);
        CHECK(syllabize(L"GEOFF", 5) == 1);
        }
    SECTION("Middle YE")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"flyer", 5) == 2);
        CHECK(syllabize(L"fruity", 6) == 2);

        CHECK(syllabize(L"FLYER", 5) == 2);
        CHECK(syllabize(L"FRUITY", 6) == 2);
        }
    SECTION("IENCE")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"nutrience", 9) == 3);
        CHECK(syllabize(L"experience", 10) == 4);
        CHECK(syllabize(L"patience", 8) == 2);
        CHECK(syllabize(L"convenience", 11) == 3);
        CHECK(syllabize(L"convenienced", 12) == 3);
        CHECK(syllabize(L"sentience", 9) == 2);
        CHECK(syllabize(L"ebullience", 10) == 3);
        CHECK(syllabize(L"ebulliency", 10) == 4);
        CHECK(syllabize(L"lenience", 8) == 3);
        CHECK(syllabize(L"salience", 8) == 3);
        CHECK(syllabize(L"inconveniencing", 15) == 5);
        CHECK(syllabize(L"geosciences", 11) == 5);
        CHECK(syllabize(L"unscientific", 12) == 5);
        CHECK(syllabize(L"nescient", 8) == 2);
        CHECK(syllabize(L"prescient", 9) == 2);
        CHECK(syllabize(L"omniscient", 10) == 3);

        CHECK(syllabize(L"NUTRIENCE", 9) == 3);
        CHECK(syllabize(L"EXPERIENCE", 10) == 4);
        CHECK(syllabize(L"PATIENCE", 8) == 2);
        CHECK(syllabize(L"CONVENIENCE", 11) == 3);
        CHECK(syllabize(L"CONVENIENCED", 12) == 3);
        CHECK(syllabize(L"SENTIENCE", 9) == 2);
        CHECK(syllabize(L"EBULLIENCE", 10) == 3);
        CHECK(syllabize(L"EBULLIENCY", 10) == 4);
        CHECK(syllabize(L"LENIENCE", 8) == 3);
        CHECK(syllabize(L"SALIENCE", 8) == 3);
        CHECK(syllabize(L"INCONVENIENCING", 15) == 5);
        CHECK(syllabize(L"GEOSCIENCES", 11) == 5);
        CHECK(syllabize(L"UNSCIENTIFIC", 12) == 5);
        CHECK(syllabize(L"NESCIENT", 8) == 2);
        CHECK(syllabize(L"PRESCIENT", 9) == 2);
        CHECK(syllabize(L"OMNISCIENT", 10) == 3);
        }
    SECTION("IEN")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"ancient", 7) == 2);
        CHECK(syllabize(L"client", 6) == 2);
        CHECK(syllabize(L"science", 7) == 2);
        CHECK(syllabize(L"lien", 4) == 1);
        CHECK(syllabize(L"alien", 5) == 3);
        CHECK(syllabize(L"patient", 7) == 2);
        CHECK(syllabize(L"patients", 8) == 2);
        CHECK(syllabize(L"convenient", 10) == 3);
        CHECK(syllabize(L"sentient", 8) == 2);
        CHECK(syllabize(L"nutrient", 8) == 3);
        CHECK(syllabize(L"friend", 6) == 1);
        CHECK(syllabize(L"lenient", 7) == 3);
        CHECK(syllabize(L"salient", 7) == 3);
        CHECK(syllabize(L"equestrienne", 12) == 4);
        CHECK(syllabize(L"resilient", 9) == 3);
        CHECK(syllabize(L"conscientious", 13) == 4);

        CHECK(syllabize(L"ANCIENT", 7) == 2);
        CHECK(syllabize(L"CLIENT", 6) == 2);
        CHECK(syllabize(L"SCIENCE", 7) == 2);
        CHECK(syllabize(L"LIEN", 4) == 1);
        CHECK(syllabize(L"ALIEN", 5) == 3);
        CHECK(syllabize(L"PATIENT", 7) == 2);
        CHECK(syllabize(L"PATIENTS", 8) == 2);
        CHECK(syllabize(L"CONVENIENT", 10) == 3);
        CHECK(syllabize(L"SENTIENT", 8) == 2);
        CHECK(syllabize(L"NUTRIENT", 8) == 3);
        CHECK(syllabize(L"FRIEND", 6) == 1);
        CHECK(syllabize(L"LENIENT", 7) == 3);
        CHECK(syllabize(L"SALIENT", 7) == 3);
        CHECK(syllabize(L"EQUESTRIENNE", 12) == 4);
        CHECK(syllabize(L"RESILIENT", 9) == 3);
        CHECK(syllabize(L"CONSCIENTIOUS", 13) == 4);
        }
    SECTION("OE")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"phoenix", 7) == 2);
        CHECK(syllabize(L"schoen", 6) == 1);
        CHECK(syllabize(L"Chloe", 5) == 2);
        CHECK(syllabize(L"Chloe's", 7) == 2);
        CHECK(syllabize(L"coercion", 8) == 3);
        CHECK(syllabize(L"scion", 5) == 2);
        CHECK(syllabize(L"scions", 6) == 2);
        CHECK(syllabize(L"suspicion", 9) == 3);
        CHECK(syllabize(L"doer", 4) == 2);
        CHECK(syllabize(L"toe", 3) == 1);
        CHECK(syllabize(L"amoeba", 6) == 3);
        CHECK(syllabize(L"amoebas", 7) == 3);
        }
    SECTION("OI")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"doing", 5) == 2);
        CHECK(syllabize(L"choir", 5) == 1);
        CHECK(syllabize(L"autoing", 7) == 3);
        CHECK(syllabize(L"anoint", 5) == 2);
        CHECK(syllabize(L"android", 7) == 2);
        CHECK(syllabize(L"heroic", 6) == 3);
        CHECK(syllabize(L"ghettoized", 10) == 3);
        CHECK(syllabize(L"coincide", 8) == 3);
        CHECK(syllabize(L"coin", 4) == 1);

        CHECK(syllabize(L"DOING", 5) == 2);
        CHECK(syllabize(L"CHOIR", 5) == 1);
        CHECK(syllabize(L"AUTOING", 7) == 3);
        CHECK(syllabize(L"ANOINT", 5) == 2);
        CHECK(syllabize(L"ANDROID", 7) == 2);
        CHECK(syllabize(L"HEROIC", 6) == 3);
        CHECK(syllabize(L"GHETTOIZED", 10) == 3);
        CHECK(syllabize(L"COINCIDE", 8) == 3);
        CHECK(syllabize(L"COIN", 4) == 1);
        }
    SECTION("ED")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"hated", 5) == 2);
        CHECK(syllabize(L"embedded", 8) == 3);
        CHECK(syllabize(L"graphed", 7) == 1);
        CHECK(syllabize(L"trekked", 7) == 1);

        CHECK(syllabize(L"HATED", 5) == 2);
        CHECK(syllabize(L"EMBEDDED", 8) == 3);
        CHECK(syllabize(L"GRAPHED", 7) == 1);
        CHECK(syllabize(L"TREKKED", 7) == 1);
        }
    SECTION("Slash")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"and/or", 6) == 2);
        CHECK(syllabize(L"AND/OR", 6) == 2);
        }
    SECTION("ES")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"gaffes", 6) == 1);
        CHECK(syllabize(L"gazelles", 8) == 2);
        CHECK(syllabize(L"ashes", 5) == 2);
        CHECK(syllabize(L"ages", 4) == 2);
        CHECK(syllabize(L"axes", 4) == 2);
        CHECK(syllabize(L"faces", 5) == 2);
        CHECK(syllabize(L"hazes", 5) == 2);
        CHECK(syllabize(L"assesses", 8) == 3);

        CHECK(syllabize(L"GAFFES", 6) == 1);
        CHECK(syllabize(L"GAZELLES", 8) == 2);
        CHECK(syllabize(L"ASHES", 5) == 2);
        CHECK(syllabize(L"AGES", 4) == 2);
        CHECK(syllabize(L"AXES", 4) == 2);
        CHECK(syllabize(L"FACES", 5) == 2);
        CHECK(syllabize(L"HAZES", 5) == 2);
        CHECK(syllabize(L"ASSESSES", 8) == 3);
        }
    SECTION("Oa")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"boat", 4) == 1);
        CHECK(syllabize(L"Boa", 3) == 2);
        CHECK(syllabize(L"Cocoa", 5) == 2);

        CHECK(syllabize(L"BOAT", 4) == 1);
        CHECK(syllabize(L"BOA", 3) == 2);
        CHECK(syllabize(L"COCOA", 5) == 2);
        }
    SECTION("Possessive")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"ape's", 5) == 1);
        CHECK(syllabize(L"cable's", 7) == 2);
        CHECK(syllabize(L"cables'", 7) == 2);
        CHECK(syllabize(L"bombardier", 10) == 3);
        CHECK(syllabize(L"frontier", 8) == 2);
        CHECK(syllabize(L"soldier", 7) == 2);
        CHECK(syllabize(L"soldier's", 9) == 2);
        CHECK(syllabize(L"soldier´s", 9) == 2); // fancy quote
        CHECK(syllabize(L"frontiersman", 11) == 3);

        CHECK(syllabize(L"APE'S", 5) == 1);
        CHECK(syllabize(L"CABLE'S", 7) == 2);
        CHECK(syllabize(L"CABLES'", 7) == 2);
        CHECK(syllabize(L"BOMBARDIER", 10) == 3);
        CHECK(syllabize(L"FRONTIER", 8) == 2);
        CHECK(syllabize(L"SOLDIER", 7) == 2);
        CHECK(syllabize(L"SOLDIER'S", 9) == 2);
        CHECK(syllabize(L"SOLDIER´S", 9) == 2); // fancy quote
        CHECK(syllabize(L"FRONTIERSMAN", 11) == 3);
        }
    SECTION("Evangel")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"evangel", 7) == 3);
        CHECK(syllabize(L"evangelism", 10) == 4);
        CHECK(syllabize(L"evangelist", 10) == 4);
        CHECK(syllabize(L"pricelist", 8) == 2); // separate logic for this
        CHECK(syllabize(L"evangelists", 11) == 4);
        CHECK(syllabize(L"evangelistic", 12) == 5);

        CHECK(syllabize(L"EVANGEL", 7) == 3);
        CHECK(syllabize(L"EVANGELISM", 10) == 4);
        CHECK(syllabize(L"EVANGELIST", 10) == 4);
        CHECK(syllabize(L"PRICELIST", 8) == 2); // separate logic for this
        CHECK(syllabize(L"EVANGELISTS", 11) == 4);
        CHECK(syllabize(L"EVANGELISTIC", 12) == 5);
        }
    SECTION("Every")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"ever", 4) == 2);
        CHECK(syllabize(L"every", 5) == 2);
        CHECK(syllabize(L"everyone", 8) == 3);
        CHECK(syllabize(L"everyone's", 10) == 3);
        CHECK(syllabize(L"everything", 10) == 3);

        CHECK(syllabize(L"EVER", 4) == 2);
        CHECK(syllabize(L"EVERY", 5) == 2);
        CHECK(syllabize(L"EVERYONE", 8) == 3);
        CHECK(syllabize(L"EVERYONE'S", 10) == 3);
        CHECK(syllabize(L"EVERYTHING", 10) == 3);
        }
    SECTION("Acronym")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"N.A.R.C.", 8) == 4);
        }
    SECTION("Acronym2")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"B.C.", 4) == 2);
        }
    SECTION("Acronym No Dots")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"SATURN", 6) == 2);
        }
    SECTION("Initial")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"C.", 2) == 1);
        }
    SECTION("Dash")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"tired-looking", 13) == 3);
        CHECK(syllabize(L"V-shaped", 8) == 2);
        CHECK(syllabize(L"toys-r-us", 9) == 3);
        CHECK(syllabize(L"jack-o-lantern", 14) == 4);
        CHECK(syllabize(L"jack-o-", 7) == 2);
        CHECK(syllabize(L"-jack-o", 7) == 2);
        CHECK(syllabize(L"-", 1) == 1);

        CHECK(syllabize(L"TIRED-LOOKING", 13) == 3);
        CHECK(syllabize(L"V-SHAPED", 8) == 2);
        CHECK(syllabize(L"TOYS-R-US", 9) == 3);
        CHECK(syllabize(L"JACK-O-LANTERN", 14) == 4);
        CHECK(syllabize(L"JACK-O-", 7) == 2);
        CHECK(syllabize(L"-JACK-O", 7) == 2);
        }
    SECTION("SHES")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"lashes",6) == 2);
        CHECK(syllabize(L"ashes",5) == 2);

        CHECK(syllabize(L"LASHES",6) == 2);
        CHECK(syllabize(L"ASHES",5) == 2);
        }
    SECTION("CHES")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"peaches",7) == 2);
        CHECK(syllabize(L"ache",4) == 1);
        CHECK(syllabize(L"ached",5) == 1);
        CHECK(syllabize(L"acheron",7) == 3);
        CHECK(syllabize(L"moustache",9) == 2);
        CHECK(syllabize(L"moustaches",10) == 3);
        CHECK(syllabize(L"moustached",10) == 2);
        CHECK(syllabize(L"aches",5) == 1);
        CHECK(syllabize(L"backached",9) == 2);
        CHECK(syllabize(L"backaches",9) == 2);
        CHECK(syllabize(L"heartaches",10) == 2);
        CHECK(syllabize(L"earaches",8) == 2);
        CHECK(syllabize(L"earache",7) == 2);
        // CAPPED
        CHECK(syllabize(L"PEACHES",7) == 2);
        CHECK(syllabize(L"ACHE",4) == 1);
        CHECK(syllabize(L"ACHED",5) == 1);
        CHECK(syllabize(L"ACHERON",7) == 3);
        CHECK(syllabize(L"MOUSTACHE",9) == 2);
        CHECK(syllabize(L"MOUSTACHES",10) == 3);
        CHECK(syllabize(L"MOUSTACHED",10) == 2);
        CHECK(syllabize(L"ACHES",5) == 1);
        CHECK(syllabize(L"BACKACHED",9) == 2);
        CHECK(syllabize(L"BACKACHES",9) == 2);
        CHECK(syllabize(L"HEARTACHES",10) == 2);
        CHECK(syllabize(L"EARACHES",8) == 2);
        CHECK(syllabize(L"EARACHE",7) == 2);
        }
    SECTION("CATEgory")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"category", 8) == 4);
        CHECK(syllabize(L"categorical", 11) == 5);
        CHECK(syllabize(L"categories", 10) == 4);
        CHECK(syllabize(L"scategories", 11) == 4);

        CHECK(syllabize(L"CATEGORY", 8));
        CHECK(syllabize(L"CATEGORICAL", 11) == 5);
        CHECK(syllabize(L"CATEGORIES", 10) == 4);
        CHECK(syllabize(L"SCATEGORIES", 11) == 4);
        }
    SECTION("Y")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"accompanying", 12) == 5);
        CHECK(syllabize(L"dryer", 5) == 2);
        CHECK(syllabize(L"lawyer", 6) == 2);
        CHECK(syllabize(L"polyester", 9) == 4);

        CHECK(syllabize(L"ACCOMPANYING", 12) == 5);
        CHECK(syllabize(L"DRYER", 5) == 2);
        CHECK(syllabize(L"LAWYER", 6) == 2);
        CHECK(syllabize(L"POLYESTER", 9) == 4);
        }
    SECTION("EO")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"jeopardy", 8) == 3);
        CHECK(syllabize(L"tiredness", 6) == 2);
        CHECK(syllabize(L"people", 6) == 2);
        CHECK(syllabize(L"peon", 4) == 2);
        CHECK(syllabize(L"Peoria", 6) == 4);

        CHECK(syllabize(L"JEOPARDY", 8) == 3);
        CHECK(syllabize(L"TIREDNESS", 6) == 2);
        CHECK(syllabize(L"PEOPLE", 6) == 2);
        CHECK(syllabize(L"PEON", 4) == 2);
        CHECK(syllabize(L"PEORIA", 6) == 4);
        }
    SECTION("ION")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"io", 2) == 2);
        CHECK(syllabize(L"ion", 3) == 2);
        CHECK(syllabize(L"scion", 5) == 2);
        CHECK(syllabize(L"prior", 5) == 2);
        CHECK(syllabize(L"radio", 5) == 3);
        CHECK(syllabize(L"lion", 4) == 2);
        CHECK(syllabize(L"billion", 7) == 2);

        CHECK(syllabize(L"IO", 2) == 2);
        CHECK(syllabize(L"ION", 3) == 2);
        CHECK(syllabize(L"SCION", 5) == 2);
        CHECK(syllabize(L"PRIOR", 5) == 2);
        CHECK(syllabize(L"RADIO", 5) == 3);
        CHECK(syllabize(L"LION", 4) == 2);
        CHECK(syllabize(L"BILLION", 7) == 2);
        }
    SECTION("EI")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"corporeity", 10) == 5);
        CHECK(syllabize(L"CORPOREITY", 10) == 5);
        }
    SECTION("IE")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"hygiene", 7) == 2);
        CHECK(syllabize(L"HYGIENE", 7) == 2);
        }
    SECTION("NonAffectingSuffix")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"jukeboxes", 9) == 3);
        CHECK(syllabize(L"JUKEBOXES", 9) == 3);
        }
    SECTION("OA")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"psychoanalyse", 13) == 5);
        CHECK(syllabize(L"PSYCHOANALYSE", 13) == 5);

        CHECK(syllabize(L"oasts", 5) == 1);
        CHECK(syllabize(L"OASTS", 5) == 1);
        CHECK(syllabize(L"oasis", 5) == 3);
        CHECK(syllabize(L"OASIS", 5) == 3);

        CHECK(syllabize(L"joanne", 6) == 2);
        CHECK(syllabize(L"joan", 4) == 1);
        CHECK(syllabize(L"JOANNE", 6) == 2);
        CHECK(syllabize(L"JOAN", 4) == 1);
        }
    SECTION("EA")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"idealise", 8) == 4);
        CHECK(syllabize(L"preach", 6) == 1);
        CHECK(syllabize(L"prearrange", 10) == 3);
        CHECK(syllabize(L"preamble", 8) == 3);

        CHECK(syllabize(L"IDEALISE", 8) == 4);
        CHECK(syllabize(L"PREACH", 6) == 1);
        CHECK(syllabize(L"PREARRANGE", 10) == 3);
        CHECK(syllabize(L"PREAMBLE", 8) == 3);
        }
    SECTION("UI")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"juicy", 5) == 2);
        CHECK(syllabize(L"JUICY", 5) == 2);
        }
    SECTION("REUN")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"reu", 3) == 1);
        CHECK(syllabize(L"reunion", 7) == 3);
        CHECK(syllabize(L"reunification", 13) == 6);
        CHECK(syllabize(L"reuters", 7) == 2);

        CHECK(syllabize(L"REU", 3) == 1);
        CHECK(syllabize(L"REUNION", 7) == 3);
        CHECK(syllabize(L"REUNIFICATION", 13) == 6);
        CHECK(syllabize(L"REUTERS", 7) == 2);
        }
    SECTION("Silent E")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"phoebe", 6) == 2);
        CHECK(syllabize(L"gobble", 6) == 2);
        CHECK(syllabize(L"gobbledegook", 12) == 4);
        CHECK(syllabize(L"gobbledygook", 12) == 4);
        CHECK(syllabize(L"wherever", 8) == 3);
        CHECK(syllabize(L"omega", 5) == 3);
        CHECK(syllabize(L"adequate", 8) == 3);
        CHECK(syllabize(L"achemi", 6) == 3);
        CHECK(syllabize(L"achemistry", 10) == 4);
        CHECK(syllabize(L"temped", 6) == 1);
        CHECK(syllabize(L"agues", 5) == 2);
        CHECK(syllabize(L"doorstep", 8) == 2);
        CHECK(syllabize(L"doorsteps", 9) == 2);
        CHECK(syllabize(L"see", 3) == 1);
        CHECK(syllabize(L"seer", 4) == 2);
        CHECK(syllabize(L"plagues", 7) == 1);
        CHECK(syllabize(L"placebo", 7) == 3);
        CHECK(syllabize(L"queue", 5) == 1);
        CHECK(syllabize(L"queues", 6) == 1);
        CHECK(syllabize(L"frambesia", 9) == 3);
        CHECK(syllabize(L"travelogues", 11) == 3);
        CHECK(syllabize(L"tempestuous", 11) == 4);
        CHECK(syllabize(L"permed", 6) == 1);
        CHECK(syllabize(L"therewith", 9) == 2);
        CHECK(syllabize(L"stovetop", 8) == 2);
        CHECK(syllabize(L"honesty", 7) == 3);
        CHECK(syllabize(L"honesties", 9) == 3);
        CHECK(syllabize(L"permeable", 9) == 4);
        CHECK(syllabize(L"fineable", 8) == 3);
        CHECK(syllabize(L"celebrate", 9) == 3);
        CHECK(syllabize(L"tipewriters", 11) == 3);
        CHECK(syllabize(L"teletypewriters", 15) == 5);
        CHECK(syllabize(L"graveyard", 9) == 2);
        CHECK(syllabize(L"corresponding", 13) == 4);
        CHECK(syllabize(L"offed", 5) == 1);
        CHECK(syllabize(L"offer", 5) == 2);
        CHECK(syllabize(L"protester", 9) == 3);
        CHECK(syllabize(L"protesters", 10) == 3);
        CHECK(syllabize(L"protestingly", 12) == 4);
        CHECK(syllabize(L"protegee", 8) == 3);
        CHECK(syllabize(L"prerecord", 9) == 3);
        CHECK(syllabize(L"predecease", 10) == 3);
        CHECK(syllabize(L"programmes", 10) == 2);
        CHECK(syllabize(L"midinettes", 10) == 3);
        CHECK(syllabize(L"hereafter", 9) == 3);
        CHECK(syllabize(L"hereby", 6) == 2);
        CHECK(syllabize(L"heresy", 6) == 3);
        CHECK(syllabize(L"eavesdrop", 9) == 2);
        CHECK(syllabize(L"corpse", 6) == 1);
        CHECK(syllabize(L"squelched", 9) == 1);
        CHECK(syllabize(L"arpeggiated", 11) == 5);
        CHECK(syllabize(L"interestingly", 13) == 5);
        CHECK(syllabize(L"giardia", 7) == 4);
        CHECK(syllabize(L"plagiarize", 10) == 3);
        CHECK(syllabize(L"collegiate", 10) == 3);
        CHECK(syllabize(L"inelegant", 9) == 4);
        CHECK(syllabize(L"lifeboat", 8) == 2);
        CHECK(syllabize(L"jukebox", 7) == 2);
        CHECK(syllabize(L"itemize", 7) == 3);
        CHECK(syllabize(L"irrelevancy", 11) == 5);
        CHECK(syllabize(L"inebriates", 10) == 4);
        CHECK(syllabize(L"ices", 4) == 2);
        CHECK(syllabize(L"houses", 6) == 2);
        CHECK(syllabize(L"housed", 6) == 1);
        CHECK(syllabize(L"iceman", 6) == 2);
        CHECK(syllabize(L"horserace", 9) == 2);
        CHECK(syllabize(L"housemaids", 10) == 2);
        CHECK(syllabize(L"coughes", 7) == 1);
        CHECK(syllabize(L"coughed", 7) == 1);
        CHECK(syllabize(L"sombrero", 8) == 3);
        CHECK(syllabize(L"sometime", 8) == 2);
        CHECK(syllabize(L"someone", 7) == 2);
        CHECK(syllabize(L"magnetism", 9) == 3);
        CHECK(syllabize(L"ninety", 6) == 2);
        CHECK(syllabize(L"safety", 6) == 2);
        CHECK(syllabize(L"boredom", 7) == 2);
        CHECK(syllabize(L"something", 9) == 2);
        CHECK(syllabize(L"competent", 9) == 3);
        CHECK(syllabize(L"swayed", 6) == 1);
        CHECK(syllabize(L"carmelized", 10) == 3);
        CHECK(syllabize(L"horseshoe", 9) == 2);
        CHECK(syllabize(L"horsewhip", 9) == 2);
        CHECK(syllabize(L"processes", 9) == 3);
        CHECK(syllabize(L"dissociates", 11) == 4);
        CHECK(syllabize(L"persnickety", 11) == 4);
        CHECK(syllabize(L"bedevil", 7) == 3);
        CHECK(syllabize(L"hate", 4) == 1);
        CHECK(syllabize(L"completely", 10) == 3);
        CHECK(syllabize(L"privilege", 9) == 3);
        CHECK(syllabize(L"privileges", 10) == 4);
        CHECK(syllabize(L"privileged", 10) == 3);
        CHECK(syllabize(L"celebrates", 10) == 3);
        CHECK(syllabize(L"disequilibrated", 15) == 6);
        CHECK(syllabize(L"disseminate", 11) == 4);
        CHECK(syllabize(L"discontented", 12) == 4);
        CHECK(syllabize(L"domestic", 8) == 3);
        CHECK(syllabize(L"angelic", 7) == 3);
        CHECK(syllabize(L"competitiveness", 15) == 5);
        CHECK(syllabize(L"competitor", 10) == 4);
        CHECK(syllabize(L"complimented", 12) == 4);
        CHECK(syllabize(L"alleviates", 10) == 4);
        CHECK(syllabize(L"forest", 6) == 2);
        CHECK(syllabize(L"foresters", 9) == 3);
        CHECK(syllabize(L"forex", 5) == 2);
        CHECK(syllabize(L"phosphorescent", 14) == 4);
        CHECK(syllabize(L"obsolescence", 12) == 4);
        CHECK(syllabize(L"obelisk", 7) == 3);
        CHECK(syllabize(L"prayer", 6) == 1);
        CHECK(syllabize(L"prayers", 7) == 1);
        CHECK(syllabize(L"triumphed", 9) == 2);
        CHECK(syllabize(L"tradesfolk", 10) == 2);
        CHECK(syllabize(L"trademark", 9) == 2);
        CHECK(syllabize(L"dished", 6) == 1);
        CHECK(syllabize(L"artefact", 8) == 3);

        CHECK(syllabize(L"PHOEBE", 6) == 2);
        CHECK(syllabize(L"GOBBLE", 6) == 2);
        CHECK(syllabize(L"GOBBLEDEGOOK", 12) == 4);
        CHECK(syllabize(L"GOBBLEDYGOOK", 12) == 4);
        CHECK(syllabize(L"WHEREVER", 8) == 3);
        CHECK(syllabize(L"OMEGA", 5) == 3);
        CHECK(syllabize(L"ADEQUATE", 8) == 3);
        CHECK(syllabize(L"ACHEMI", 6) == 3);
        CHECK(syllabize(L"ACHEMISTRY", 10) == 4);
        CHECK(syllabize(L"TEMPED", 6) == 1);
        CHECK(syllabize(L"AGUES", 5) == 2);
        CHECK(syllabize(L"DOORSTEP", 8) == 2);
        CHECK(syllabize(L"DOORSTEPS", 9) == 2);
        CHECK(syllabize(L"SEE", 3) == 1);
        CHECK(syllabize(L"SEER", 4) == 2);
        CHECK(syllabize(L"PLAGUES", 7) == 1);
        CHECK(syllabize(L"PLACEBO", 7) == 3);
        CHECK(syllabize(L"QUEUE", 5) == 1);
        CHECK(syllabize(L"QUEUES", 6) == 1);
        CHECK(syllabize(L"FRAMBESIA", 9) == 3);
        CHECK(syllabize(L"TRAVELOGUES", 11) == 3);
        CHECK(syllabize(L"TEMPESTUOUS", 11) == 4);
        CHECK(syllabize(L"PERMED", 6) == 1);
        CHECK(syllabize(L"THEREWITH", 9) == 2);
        CHECK(syllabize(L"STOVETOP", 8) == 2);
        CHECK(syllabize(L"HONESTY", 7) == 3);
        CHECK(syllabize(L"HONESTIES", 9) == 3);
        CHECK(syllabize(L"PERMEABLE", 9) == 4);
        CHECK(syllabize(L"FINEABLE", 8) == 3);
        CHECK(syllabize(L"CELEBRATE", 9) == 3);
        CHECK(syllabize(L"TIPEWRITERS", 11) == 3);
        CHECK(syllabize(L"TELETYPEWRITERS", 15) == 5);
        CHECK(syllabize(L"GRAVEYARD", 9) == 2);
        CHECK(syllabize(L"CORRESPONDING", 13) == 4);
        CHECK(syllabize(L"OFFED", 5) == 1);
        CHECK(syllabize(L"OFFER", 5) == 2);
        CHECK(syllabize(L"PROTESTER", 9) == 3);
        CHECK(syllabize(L"PROTESTERS", 10) == 3);
        CHECK(syllabize(L"PROTESTINGLY", 12) == 4);
        CHECK(syllabize(L"PROTEGEE", 8) == 3);
        CHECK(syllabize(L"PRERECORD", 9) == 3);
        CHECK(syllabize(L"PREDECEASE", 10) == 3);
        CHECK(syllabize(L"PROGRAMMES", 10) == 2);
        CHECK(syllabize(L"MIDINETTES", 10) == 3);
        CHECK(syllabize(L"HEREAFTER", 9) == 3);
        CHECK(syllabize(L"HEREBY", 6) == 2);
        CHECK(syllabize(L"HERESY", 6) == 3);
        CHECK(syllabize(L"EAVESDROP", 9) == 2);
        CHECK(syllabize(L"CORPSE", 6) == 1);
        CHECK(syllabize(L"SQUELCHED", 9) == 1);
        CHECK(syllabize(L"ARPEGGIATED", 11) == 5);
        CHECK(syllabize(L"INTERESTINGLY", 13) == 5);
        CHECK(syllabize(L"GIARDIA", 7) == 4);
        CHECK(syllabize(L"PLAGIARIZE", 10) == 3);
        CHECK(syllabize(L"COLLEGIATE", 10) == 3);
        CHECK(syllabize(L"INELEGANT", 9) == 4);
        CHECK(syllabize(L"LIFEBOAT", 8) == 2);
        CHECK(syllabize(L"JUKEBOX", 7) == 2);
        CHECK(syllabize(L"ITEMIZE", 7) == 3);
        CHECK(syllabize(L"IRRELEVANCY", 11) == 5);
        CHECK(syllabize(L"INEBRIATES", 10) == 4);
        CHECK(syllabize(L"ICES", 4) == 2);
        CHECK(syllabize(L"HOUSES", 6) == 2);
        CHECK(syllabize(L"HOUSED", 6) == 1);
        CHECK(syllabize(L"ICEMAN", 6) == 2);
        CHECK(syllabize(L"HORSERACE", 9) == 2);
        CHECK(syllabize(L"HOUSEMAIDS", 10) == 2);
        CHECK(syllabize(L"COUGHES", 7) == 1);
        CHECK(syllabize(L"COUGHED", 7) == 1);
        CHECK(syllabize(L"SOMBRERO", 8) == 3);
        CHECK(syllabize(L"SOMETIME", 8) == 2);
        CHECK(syllabize(L"SOMEONE", 7) == 2);
        CHECK(syllabize(L"MAGNETISM", 9) == 3);
        CHECK(syllabize(L"NINETY", 6) == 2);
        CHECK(syllabize(L"SAFETY", 6) == 2);
        CHECK(syllabize(L"BOREDOM", 7) == 2);
        CHECK(syllabize(L"SOMETHING", 9) == 2);
        CHECK(syllabize(L"COMPETENT", 9) == 3);
        CHECK(syllabize(L"SWAYED", 6) == 1);
        CHECK(syllabize(L"CARMELIZED", 10) == 3);
        CHECK(syllabize(L"HORSESHOE", 9) == 2);
        CHECK(syllabize(L"HORSEWHIP", 9) == 2);
        CHECK(syllabize(L"PROCESSES", 9) == 3);
        CHECK(syllabize(L"DISSOCIATES", 11) == 4);
        CHECK(syllabize(L"PERSNICKETY", 11) == 4);
        CHECK(syllabize(L"BEDEVIL", 7) == 3);
        CHECK(syllabize(L"HATE", 4) == 1);
        CHECK(syllabize(L"COMPLETELY", 10) == 3);
        CHECK(syllabize(L"PRIVILEGE", 9) == 3);
        CHECK(syllabize(L"PRIVILEGES", 10) == 4);
        CHECK(syllabize(L"PRIVILEGED", 10) == 3);
        CHECK(syllabize(L"CELEBRATES", 10) == 3);
        CHECK(syllabize(L"DISEQUILIBRATED", 15) == 6);
        CHECK(syllabize(L"DISSEMINATE", 11) == 4);
        CHECK(syllabize(L"DISCONTENTED", 12) == 4);
        CHECK(syllabize(L"DOMESTIC", 8) == 3);
        CHECK(syllabize(L"ANGELIC", 7) == 3);
        CHECK(syllabize(L"COMPETITIVENESS", 15) == 5);
        CHECK(syllabize(L"COMPETITOR", 10) == 4);
        CHECK(syllabize(L"COMPLIMENTED", 12) == 4);
        CHECK(syllabize(L"ALLEVIATES", 10) == 4);
        CHECK(syllabize(L"FOREST", 6) == 2);
        CHECK(syllabize(L"FORESTERS", 9) == 3);
        CHECK(syllabize(L"FOREX", 5) == 2);
        CHECK(syllabize(L"PHOSPHORESCENT", 14) == 4);
        CHECK(syllabize(L"OBSOLESCENCE", 12) == 4);
        CHECK(syllabize(L"OBELISK", 7) == 3);
        CHECK(syllabize(L"PRAYER", 6) == 1);
        CHECK(syllabize(L"PRAYERS", 7) == 1);
        CHECK(syllabize(L"TRIUMPHED", 9) == 2);
        CHECK(syllabize(L"TRADESFOLK", 10) == 2);
        CHECK(syllabize(L"DISHED", 6) == 1);
        CHECK(syllabize(L"ARTEFACT", 8) == 3);
        }
    SECTION("FOR")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"forensic", 8) == 3);
        CHECK(syllabize(L"forever", 7) == 3);
        CHECK(syllabize(L"forest", 6) == 2);
        CHECK(syllabize(L"forested", 8) == 3);
        CHECK(syllabize(L"forestall", 9) == 2);
        CHECK(syllabize(L"foresight", 8) == 2);

        CHECK(syllabize(L"FORENSIC", 8) == 3);
        CHECK(syllabize(L"FOREVER", 7) == 3);
        CHECK(syllabize(L"FOREST", 6) == 2);
        CHECK(syllabize(L"FORESTED", 8) == 3);
        CHECK(syllabize(L"FORESTALL", 9) == 2);
        CHECK(syllabize(L"FORESIGHT", 8) == 2);
        }
    SECTION("Prefix")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"preowned", 8) == 2);
        CHECK(syllabize(L"PREOWNED", 8) == 2);
        }
    SECTION("Suffix")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"procedure", 9) == 3);
        CHECK(syllabize(L"PROCEDURE", 9) == 3);
        }
    SECTION("Syllable Embedded E")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"especially", 10) == 4);
        CHECK(syllabize(L"ESPECIALLY", 10) == 4);
        }
    SECTION("Syllable Blocks")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"fruit", 5) == 1);
        CHECK(syllabize(L"fruits", 6) == 1);
        CHECK(syllabize(L"fruitiest", 9) == 3);
        CHECK(syllabize(L"incongruities", 13) == 5);
        CHECK(syllabize(L"highflyer", 9) == 3);
        CHECK(syllabize(L"glacier", 7) == 2);
        CHECK(syllabize(L"glaciers", 8) == 2);
        CHECK(syllabize(L"marria", 6) == 3);
        CHECK(syllabize(L"acquiesce", 9) == 3);
        CHECK(syllabize(L"really", 6) == 3);
        CHECK(syllabize(L"reminisce", 9) == 3);
        CHECK(syllabize(L"rebellious", 10) == 3);
        CHECK(syllabize(L"undead", 6) == 2);
        CHECK(syllabize(L"overdeveloped", 13) == 5);
        CHECK(syllabize(L"carriage", 8) == 2);
        CHECK(syllabize(L"leant", 5) == 1);
        CHECK(syllabize(L"nicked", 6) == 1);
        CHECK(syllabize(L"preagricultural", 15) == 6);
        CHECK(syllabize(L"miscreant", 9) == 3);
        CHECK(syllabize(L"permeate", 8) == 3);
        CHECK(syllabize(L"nursemaid", 9) == 2);
        CHECK(syllabize(L"pandemonium", 11) == 5);
        CHECK(syllabize(L"nickelodeon", 11) == 5);
        CHECK(syllabize(L"novelization", 12) == 5);
        CHECK(syllabize(L"caribbean", 9) == 4);
        CHECK(syllabize(L"soybean", 7) == 2);
        CHECK(syllabize(L"radioactive", 11) == 5);
        CHECK(syllabize(L"radioing", 8) == 4);
        CHECK(syllabize(L"acidemia", 8) == 5);
        CHECK(syllabize(L"suave", 5) == 1);
        CHECK(syllabize(L"persuasion", 10) == 3);
        CHECK(syllabize(L"punctilious", 11) == 4);
        CHECK(syllabize(L"theatrical", 10) == 4);
        CHECK(syllabize(L"shirtwaist", 10) == 2);
        CHECK(syllabize(L"theists", 7) == 2);
        CHECK(syllabize(L"unmusically", 11) == 5);
        CHECK(syllabize(L"overeager", 9) == 4);
        CHECK(syllabize(L"outliers", 8) == 3);
        CHECK(syllabize(L"poleax", 6) == 2);
        CHECK(syllabize(L"preex", 5) == 2);
        CHECK(syllabize(L"preexist", 8) == 3);
        CHECK(syllabize(L"european", 8) == 4);
        CHECK(syllabize(L"demean", 6) == 2);
        CHECK(syllabize(L"advisee", 7) == 3);
        CHECK(syllabize(L"museum", 6) == 3);
        CHECK(syllabize(L"abbreviate", 10) == 4);
        CHECK(syllabize(L"choreographs", 12) == 4);
        CHECK(syllabize(L"foreordain", 10) == 3);
        CHECK(syllabize(L"bludgeon", 8) == 2);
        CHECK(syllabize(L"jeopardy", 8) == 3);
        CHECK(syllabize(L"recreates", 9) == 3);
        CHECK(syllabize(L"creatu", 6) == 2);
        CHECK(syllabize(L"eaten", 5) == 2);
        CHECK(syllabize(L"oak", 3) == 1);
        CHECK(syllabize(L"oasis", 5) == 3);
        CHECK(syllabize(L"vacuum", 6) == 3);
        CHECK(syllabize(L"muumuu", 6) == 2);
        CHECK(syllabize(L"union", 5) == 2);
        CHECK(syllabize(L"deify", 5) == 3);
        CHECK(syllabize(L"deifies", 7) == 2);
        CHECK(syllabize(L"deification", 11) == 5);
        CHECK(syllabize(L"priest", 6) == 1);
        CHECK(syllabize(L"happiest", 8) == 3);
        CHECK(syllabize(L"queue", 5) == 1);
        CHECK(syllabize(L"reappointment", 13) == 4);
        CHECK(syllabize(L"rearranged", 10) == 3);
        CHECK(syllabize(L"reenter", 7) == 3);

        CHECK(syllabize(L"FRUIT", 5) == 1);
        CHECK(syllabize(L"FRUITS", 6) == 1);
        CHECK(syllabize(L"FRUITIEST", 9) == 3);
        CHECK(syllabize(L"INCONGRUITIES", 13) == 5);
        CHECK(syllabize(L"HIGHFLYER", 9) == 3);
        CHECK(syllabize(L"GLACIER", 7) == 2);
        CHECK(syllabize(L"GLACIERS", 8) == 2);
        CHECK(syllabize(L"MARRIA", 6) == 3);
        CHECK(syllabize(L"ACQUIESCE", 9) == 3);
        CHECK(syllabize(L"REALLY", 6) == 3);
        CHECK(syllabize(L"REMINISCE", 9) == 3);
        CHECK(syllabize(L"REBELLIOUS", 10) == 3);
        CHECK(syllabize(L"UNDEAD", 6) == 2);
        CHECK(syllabize(L"OVERDEVELOPED", 13) == 5);
        CHECK(syllabize(L"CARRIAGE", 8) == 2);
        CHECK(syllabize(L"LEANT", 5) == 1);
        CHECK(syllabize(L"NICKED", 6) == 1);
        CHECK(syllabize(L"PREAGRICULTURAL", 15) == 6);
        CHECK(syllabize(L"MISCREANT", 9) == 3);
        CHECK(syllabize(L"PERMEATE", 8) == 3);
        CHECK(syllabize(L"NURSEMAID", 9) == 2);
        CHECK(syllabize(L"PANDEMONIUM", 11) == 5);
        CHECK(syllabize(L"NICKELODEON", 11) == 5);
        CHECK(syllabize(L"NOVELIZATION", 12) == 5);
        CHECK(syllabize(L"CARIBBEAN", 9) == 4);
        CHECK(syllabize(L"SOYBEAN", 7) == 2);
        CHECK(syllabize(L"RADIOACTIVE", 11) == 5);
        CHECK(syllabize(L"RADIOING", 8) == 4);
        CHECK(syllabize(L"ACIDEMIA", 8) == 5);
        CHECK(syllabize(L"SUAVE", 5) == 1);
        CHECK(syllabize(L"PERSUASION", 10) == 3);
        CHECK(syllabize(L"PUNCTILIOUS", 11) == 4);
        CHECK(syllabize(L"THEATRICAL", 10) == 4);
        CHECK(syllabize(L"SHIRTWAIST", 10) == 2);
        CHECK(syllabize(L"THEISTS", 7) == 2);
        CHECK(syllabize(L"UNMUSICALLY", 11) == 5);
        CHECK(syllabize(L"OVEREAGER", 9) == 4);
        CHECK(syllabize(L"OUTLIERS", 8) == 3);
        CHECK(syllabize(L"POLEAX", 6) == 2);
        CHECK(syllabize(L"PREEX", 5) == 2);
        CHECK(syllabize(L"PREEXIST", 8) == 3);
        CHECK(syllabize(L"EUROPEAN", 8) == 4);
        CHECK(syllabize(L"DEMEAN", 6) == 2);
        CHECK(syllabize(L"ADVISEE", 7) == 3);
        CHECK(syllabize(L"MUSEUM", 6) == 3);
        CHECK(syllabize(L"ABBREVIATE", 10) == 4);
        CHECK(syllabize(L"CHOREOGRAPHS", 12) == 4);
        CHECK(syllabize(L"FOREORDAIN", 10) == 3);
        CHECK(syllabize(L"BLUDGEON", 8) == 2);
        CHECK(syllabize(L"JEOPARDY", 8) == 3);
        CHECK(syllabize(L"RECREATES", 9) == 3);
        CHECK(syllabize(L"CREATU", 6) == 2);
        CHECK(syllabize(L"EATEN", 5) == 2);
        CHECK(syllabize(L"OAK", 3) == 1);
        CHECK(syllabize(L"OASIS", 5) == 3);
        CHECK(syllabize(L"VACUUM", 6) == 3);
        CHECK(syllabize(L"MUUMUU", 6) == 2);
        CHECK(syllabize(L"UNION", 5) == 2);
        CHECK(syllabize(L"DEIFY", 5) == 3);
        CHECK(syllabize(L"DEIFIES", 7) == 2);
        CHECK(syllabize(L"DEIFICATION", 11) == 5);
        CHECK(syllabize(L"PRIEST", 6) == 1);
        CHECK(syllabize(L"HAPPIEST", 8) == 3);
        CHECK(syllabize(L"QUEUE", 5) == 1);
        CHECK(syllabize(L"REAPPOINTMENT", 13) == 4);
        CHECK(syllabize(L"REARRANGED", 10) == 3);
        CHECK(syllabize(L"REENTER", 7) == 3);
        }
    SECTION("Contraction RE")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"they're", 7) == 1);
        CHECK(syllabize(L"we're", 5) == 1);
        CHECK(syllabize(L"you're", 6) == 1);

        CHECK(syllabize(L"THEY'RE", 7) == 1);
        CHECK(syllabize(L"WE'RE", 5) == 1);
        CHECK(syllabize(L"YOU'RE", 6) == 1);
        }
    SECTION("Contraction VE")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"they've", 7) == 1);
        CHECK(syllabize(L"we've", 5) == 1);
        CHECK(syllabize(L"you've", 6) == 1);

        CHECK(syllabize(L"THEY'VE", 7) == 1);
        CHECK(syllabize(L"WE'VE", 5) == 1);
        CHECK(syllabize(L"YOU'VE", 6) == 1);
        }
    SECTION("Contraction LL")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"that'll", 7) == 2);
        CHECK(syllabize(L"what'll", 7) == 2);
        CHECK(syllabize(L"they'll", 7) == 2);
        CHECK(syllabize(L"it'll", 5) == 2);
        CHECK(syllabize(L"I'll", 4) == 1);
        CHECK(syllabize(L"he'll", 5) == 1);
        CHECK(syllabize(L"we'll", 5) == 1);
        CHECK(syllabize(L"she'll", 6) == 1);

        CHECK(syllabize(L"THAT'LL", 7) == 2);
        CHECK(syllabize(L"WHAT'LL", 7) == 2);
        CHECK(syllabize(L"THEY'LL", 7) == 2);
        CHECK(syllabize(L"IT'LL", 5) == 2);
        CHECK(syllabize(L"I'LL", 4) == 1);
        CHECK(syllabize(L"HE'LL", 5) == 1);
        CHECK(syllabize(L"WE'LL", 5) == 1);
        CHECK(syllabize(L"SHE'LL", 6) == 1);
        }
    SECTION("Contractions NT")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"aren´t", 6) == 1);
        CHECK(syllabize(L"don´t", 5) == 1);
        CHECK(syllabize(L"wouldn't", 8) == 2);
        CHECK(syllabize(L"wouldn´t", 8) == 2);
        CHECK(syllabize(L"mightn´t", 8) == 2);
        CHECK(syllabize(L"doesn't", 7) == 2);
        CHECK(syllabize(L"hasn't", 6) == 2);
        CHECK(syllabize(L"mustn't", 7) == 2);
        CHECK(syllabize(L"needn't", 7) == 2);
        CHECK(syllabize(L"hadn't", 6) == 2);
        CHECK(syllabize(L"oughtn't", 8) == 2);
        CHECK(syllabize(L"isn't", 5) == 2);
        CHECK(syllabize(L"didn't", 6) == 2);
        CHECK(syllabize(L"shouldn't", 9) == 2);
        CHECK(syllabize(L"couldn't", 8) == 2);
        CHECK(syllabize(L"wasn't", 6) == 2);
        CHECK(syllabize(L"haven't", 7) == 2);

        CHECK(syllabize(L"AREN´T", 6) == 1);
        CHECK(syllabize(L"DON´T", 5) == 1);
        CHECK(syllabize(L"WOULDN'T", 8) == 2);
        CHECK(syllabize(L"WOULDN´T", 8) == 2);
        CHECK(syllabize(L"MIGHTN´T", 8) == 2);
        CHECK(syllabize(L"DOESN'T", 7) == 2);
        CHECK(syllabize(L"HASN'T", 6) == 2);
        CHECK(syllabize(L"MUSTN'T", 7) == 2);
        CHECK(syllabize(L"NEEDN'T", 7) == 2);
        CHECK(syllabize(L"HADN'T", 6) == 2);
        CHECK(syllabize(L"OUGHTN'T", 8) == 2);
        CHECK(syllabize(L"ISN'T", 5) == 2);
        CHECK(syllabize(L"DIDN'T", 6) == 2);
        CHECK(syllabize(L"SHOULDN'T", 9) == 2);
        CHECK(syllabize(L"COULDN'T", 8) == 2);
        CHECK(syllabize(L"WASN'T", 6) == 2);
        CHECK(syllabize(L"HAVEN'T", 7) == 2);
        }
    SECTION("Mc")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"Mc", 2) == 1);
        CHECK(syllabize(L"McDonalds", 9) == 3);

        CHECK(syllabize(L"MC", 2) == 1);
        CHECK(syllabize(L"MCDONALDS", 9) == 3);
        }
    SECTION("Numeric String")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"10/26/1974", 10) == 10);
        CHECK(syllabize(L"369-8777", 8) == 10);
        CHECK(syllabize(L"$1,224.76", 9) == 10);
        CHECK(syllabize(L"£1,224.76", 9) == 9);
        CHECK(syllabize(L"#770", 4) == 7);
        CHECK(syllabize(L"50¢", 3) == 4);
        CHECK(syllabize(L"50%", 3) == 5);
        CHECK(syllabize(L"±5", 2) == 4);
        CHECK(syllabize(L"5±", 2) == 1);
        CHECK(syllabize(L"98.6°", 5) == 6);
        CHECK(syllabize(L"€1,224.76", 9) == 10);
        CHECK(syllabize(L"8%", 2) == 3);
        CHECK(syllabize(L"8¼", 2) == 3);
        CHECK(syllabize(L"8¾", 2) == 3);
        CHECK(syllabize(L"8½", 2) == 3);
        CHECK(syllabize(L"e²", 2) == 2);
        CHECK(syllabize(L"e³", 2) == 2);
        CHECK(syllabize(L"e¹", 2) == 2);
        CHECK(syllabize(L"e⁰", 2) == 2);
        CHECK(syllabize(L"e⁴", 2) == 2);
        CHECK(syllabize(L"e⁹", 2) == 2);
        CHECK(syllabize(L"e₀", 2) == 2);
        CHECK(syllabize(L"e₉", 2) == 2);
        }
    SECTION("Helper Functors")
        {
        english_syllabize mySllablizer;
        word<std::char_traits<wchar_t>, stemming::english_stem<> > money(L"$1,224.76", 9, 0, 0, 0, true, true, false, false, mySllablizer(L"$1,224.76", 9), 0);
        syllable_count_equals<word<std::char_traits<wchar_t>, stemming::english_stem<> > > syllableCountEqualsNumbersMono(10, true);
        syllable_count_equals<word<std::char_traits<wchar_t>, stemming::english_stem<> > > syllableCountEqualsNumbersNotMono(10, false);
        CHECK(syllableCountEqualsNumbersMono(money) == false);
        CHECK(syllableCountEqualsNumbersNotMono(money) == true);

        syllable_count_greater<word<std::char_traits<wchar_t>, stemming::english_stem<> > > syllableCountGreaterNumbersMono(3, true);
        syllable_count_greater<word<std::char_traits<wchar_t>, stemming::english_stem<> > > syllableCountGreaterNumbersNotMono(3, false);
        CHECK(syllableCountGreaterNumbersMono(money) == false);
        CHECK(syllableCountGreaterNumbersNotMono(money) == true);

        syllable_count_greater_equal<word<std::char_traits<wchar_t>, stemming::english_stem<> > > syllableCountGreaterEqualsNumbersMono(3, true);
        syllable_count_greater_equal<word<std::char_traits<wchar_t>, stemming::english_stem<> > > syllableCountGreaterEqualsNumbersNotMono(3, false);
        CHECK(syllableCountGreaterEqualsNumbersMono(money) == false);
        CHECK(syllableCountGreaterEqualsNumbersNotMono(money) == true);

        syllable_count_greater_equal_ignore_numerals<word<std::char_traits<wchar_t>, stemming::english_stem<> > > syllableCountGreaterEqualsNoNumbers(3);
        CHECK(syllableCountGreaterEqualsNoNumbers(money) == false);

        add_syllable_size<word<std::char_traits<wchar_t>, stemming::english_stem<> > > syllableAddNumbersMono(true);
        add_syllable_size<word<std::char_traits<wchar_t>, stemming::english_stem<> > > syllableAddNumbersNotMono(false);
        CHECK(syllableAddNumbersMono(0, money) == 1);
        CHECK(syllableAddNumbersNotMono(0, money) == 10);

        add_syllable_size_ignore_numerals<word<std::char_traits<wchar_t>, stemming::english_stem<> > > syllableAddNoNumbers;
        CHECK(syllableAddNoNumbers(0, money) == 0);
        }

    SECTION("REAL")
        {
        english_syllabize syllabize;
        CHECK(syllabize(L"real", 4) == 1);
        CHECK(syllabize(L"unreal", 6) == 2);
        CHECK(syllabize(L"really", 6) == 3);
        CHECK(syllabize(L"realty", 6) == 3);
        CHECK(syllabize(L"realist", 7) == 3);
        CHECK(syllabize(L"realistic", 9) == 4);
        CHECK(syllabize(L"realign", 7) == 3);
        CHECK(syllabize(L"realm", 5));
        CHECK(syllabize(L"netherealm", 10) == 3);
        CHECK(syllabize(L"netherealms", 11) == 3);
        CHECK(syllabize(L"netherealm's", 12) == 3);

        CHECK(syllabize(L"REAL", 4) == 1);
        CHECK(syllabize(L"UNREAL", 6) == 2);
        CHECK(syllabize(L"REALLY", 6) == 3);
        CHECK(syllabize(L"REALTY", 6) == 3);
        CHECK(syllabize(L"REALIST", 7) == 3);
        CHECK(syllabize(L"REALISTIC", 9) == 4);
        CHECK(syllabize(L"REALIGN", 7) == 3);
        CHECK(syllabize(L"REALM", 5));
        CHECK(syllabize(L"NETHEREALM", 10) == 3);
        CHECK(syllabize(L"NETHEREALMS", 11) == 3);
        CHECK(syllabize(L"NETHEREALM'S", 12) == 3);
        }

    SECTION("Helper Functors No Stemmer")
        {
        english_syllabize mySllablizer;
        word<std::char_traits<wchar_t>, stemming::no_op_stem<std::basic_string<wchar_t,std::char_traits<wchar_t> > > > money(L"$1,224.76", 9, 0, 0, 0, true, true, false, false, mySllablizer(L"$1,224.76", 9), 0);
        syllable_count_equals<word<std::char_traits<wchar_t>, stemming::no_op_stem<std::basic_string<wchar_t,std::char_traits<wchar_t> > > > > syllableCountEqualsNumbersMono(10, true);
        syllable_count_equals<word<std::char_traits<wchar_t>, stemming::no_op_stem<std::basic_string<wchar_t,std::char_traits<wchar_t> > > > > syllableCountEqualsNumbersNotMono(10, false);
        CHECK(syllableCountEqualsNumbersMono(money) == false);
        CHECK(syllableCountEqualsNumbersNotMono(money) == true);

        syllable_count_greater<word<std::char_traits<wchar_t>, stemming::no_op_stem<std::basic_string<wchar_t,std::char_traits<wchar_t> > > > > syllableCountGreaterNumbersMono(3, true);
        syllable_count_greater<word<std::char_traits<wchar_t>, stemming::no_op_stem<std::basic_string<wchar_t,std::char_traits<wchar_t> > > > > syllableCountGreaterNumbersNotMono(3, false);
        CHECK(syllableCountGreaterNumbersMono(money) == false);
        CHECK(syllableCountGreaterNumbersNotMono(money) == true);

        syllable_count_greater_equal<word<std::char_traits<wchar_t>, stemming::no_op_stem<std::basic_string<wchar_t,std::char_traits<wchar_t> > > > > syllableCountGreaterEqualsNumbersMono(3, true);
        syllable_count_greater_equal<word<std::char_traits<wchar_t>, stemming::no_op_stem<std::basic_string<wchar_t,std::char_traits<wchar_t> > > > > syllableCountGreaterEqualsNumbersNotMono(3, false);
        CHECK(syllableCountGreaterEqualsNumbersMono(money) == false);
        CHECK(syllableCountGreaterEqualsNumbersNotMono(money) == true);

        syllable_count_greater_equal_ignore_numerals<word<std::char_traits<wchar_t>, stemming::no_op_stem<std::basic_string<wchar_t,std::char_traits<wchar_t> > > > > syllableCountGreaterEqualsNoNumbers(3);
        CHECK(syllableCountGreaterEqualsNoNumbers(money) == false);

        add_syllable_size<word<std::char_traits<wchar_t>, stemming::no_op_stem<std::basic_string<wchar_t,std::char_traits<wchar_t> > > > > syllableAddNumbersMono(true);
        add_syllable_size<word<std::char_traits<wchar_t>, stemming::no_op_stem<std::basic_string<wchar_t,std::char_traits<wchar_t> > > > > syllableAddNumbersNotMono(false);
        CHECK(syllableAddNumbersMono(0, money) == 1);
        CHECK(syllableAddNumbersNotMono(0, money) == 10);

        add_syllable_size_ignore_numerals<word<std::char_traits<wchar_t>, stemming::no_op_stem<std::basic_string<wchar_t,std::char_traits<wchar_t> > > > > syllableAddNoNumbers;
        CHECK(syllableAddNoNumbers(0, money) == 0);
        }
    SECTION("Non-Western European Languages (ENG)")
        {
        // the English syllabizer is not designed for non-Western European languages,
        // but will count vowel blocks with the English logic at least and be close
        english_syllabize syllabize;

        // Greek
        CHECK(syllabize(L"α", 1) == 1); // alpha
        CHECK(syllabize(L"αβγ", 3) == 1); // alpha-beta-gamma
        CHECK(syllabize(L"λόγος", 5) == 2); // logos
        CHECK(syllabize(L"Ωμέγα", 5) == 3); // omega
        CHECK(syllabize(L"φιλοσοφία", 9) == 4); // philosophia

        // Russian (Cyrillic)
        CHECK(syllabize(L"привет", 6) == 2); // privet (hello)
        CHECK(syllabize(L"Москва", 6) == 2); // Moskva (Moscow)
        CHECK(syllabize(L"Россия", 6) == 2); // Rossiya (Russia)
        CHECK(syllabize(L"спасибо", 7) == 3); // spasibo (thank you)

        // Ukrainian (Cyrillic)
        CHECK(syllabize(L"Київ", 4) == 1); // Kyiv
        CHECK(syllabize(L"Україна", 7) == 3); // Ukraina (Ukraine)

        // Georgian
        CHECK(syllabize(L"საქართველო", 10) == 4); // Sakartvelo (Georgia)
        CHECK(syllabize(L"თბილისი", 7) == 3); // Tbilisi
        }
    SECTION("Non-Western European Languages (DE)")
        {
        // the German syllabizer is not designed for non-Western European languages,
        // but will count vowel blocks with the English logic at least and be close
        german_syllabize syllabize;

        // Greek
        CHECK(syllabize(L"α", 1) == 1); // alpha
        CHECK(syllabize(L"αβγ", 3) == 1); // alpha-beta-gamma
        CHECK(syllabize(L"λόγος", 5) == 2); // logos
        CHECK(syllabize(L"Ωμέγα", 5) == 3); // omega
        CHECK(syllabize(L"φιλοσοφία", 9) == 4); // philosophia

        // Russian (Cyrillic)
        CHECK(syllabize(L"привет", 6) == 2); // privet (hello)
        CHECK(syllabize(L"Москва", 6) == 2); // Moskva (Moscow)
        CHECK(syllabize(L"Россия", 6) == 2); // Rossiya (Russia)
        CHECK(syllabize(L"спасибо", 7) == 3); // spasibo (thank you)

        // Ukrainian (Cyrillic)
        CHECK(syllabize(L"Київ", 4) == 1); // Kyiv
        CHECK(syllabize(L"Україна", 7) == 3); // Ukraina (Ukraine)

        // Georgian
        CHECK(syllabize(L"საქართველო", 10) == 4); // Sakartvelo (Georgia)
        CHECK(syllabize(L"თბილისი", 7) == 3); // Tbilisi
        }
    SECTION("Non-Western European Languages (ES)")
        {
        // the Spanish syllabizer is not designed for non-Western European languages,
        // but will count vowel blocks with the English logic at least and be close
        spanish_syllabize syllabize;

        // Greek
        CHECK(syllabize(L"α", 1) == 1); // alpha
        CHECK(syllabize(L"αβγ", 3) == 1); // alpha-beta-gamma
        CHECK(syllabize(L"λόγος", 5) == 2); // logos
        CHECK(syllabize(L"Ωμέγα", 5) == 3); // omega
        CHECK(syllabize(L"φιλοσοφία", 9) == 4); // philosophia

        // Russian (Cyrillic)
        CHECK(syllabize(L"привет", 6) == 2); // privet (hello)
        CHECK(syllabize(L"Москва", 6) == 2); // Moskva (Moscow)
        CHECK(syllabize(L"Россия", 6) == 2); // Rossiya (Russia)
        CHECK(syllabize(L"спасибо", 7) == 3); // spasibo (thank you)

        // Ukrainian (Cyrillic)
        CHECK(syllabize(L"Київ", 4) == 1); // Kyiv
        CHECK(syllabize(L"Україна", 7) == 3); // Ukraina (Ukraine)

        // Georgian
        CHECK(syllabize(L"საქართველო", 10) == 4); // Sakartvelo (Georgia)
        CHECK(syllabize(L"თბილისი", 7) == 3); // Tbilisi
        }

    SECTION("Japanese")
        {
        english_syllabize syllabize;
        
        // example from docs
        CHECK(syllabize(L"ドキュメンテーション", 10) == 8);

        // pure hiragana (each character = 1 syllable)
        CHECK(syllabize(L"あ", 1) == 1);
        CHECK(syllabize(L"あい", 2) == 2);
        CHECK(syllabize(L"さくら", 3) == 3); // sakura
        CHECK(syllabize(L"ひらがな", 4) == 4); // hiragana
        CHECK(syllabize(L"おはよう", 4) == 4); // ohayou

        // pure katakana
        CHECK(syllabize(L"アイ", 2) == 2);
        CHECK(syllabize(L"カタカナ", 4) == 4); // katakana
        CHECK(syllabize(L"トウキョウ", 5) == 4); // toukyou

        // halfwidth katakana
        CHECK(syllabize(L"ｱｲ", 2) == 2);

        // pure kanji
        CHECK(syllabize(L"日", 1) == 1);
        CHECK(syllabize(L"日本", 2) == 2); // nihon
        CHECK(syllabize(L"東京", 2) == 2); // toukyou
        CHECK(syllabize(L"山水火木", 4) == 4);

        // mixed hiragana and katakana
        CHECK(syllabize(L"あアい", 3) == 3);

        // mixed kanji and hiragana
        CHECK(syllabize(L"食べる", 3) == 3); // taberu

        // mixed: Japanese first, then English letters
        CHECK(syllabize(L"東京test", 6) == 3); // 2 kanji + "test" (1 syllable)

        // mixed: English letters first, then Japanese
        CHECK(syllabize(L"test東京", 6) == 3); // "test" (1) + 2 kanji

        CHECK(syllabize(L"東te京st", 6) == 3); // weird mix, just do what we can
        }
    }
// NOLINTEND
// clang-format on
