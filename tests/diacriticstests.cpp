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

#include "../src/indexing/diacritics.h"
#include <catch2/catch_test_macros.hpp>

// clang-format off
// NOLINTBEGIN

TEST_CASE("Diacritics", "[diacritics]")
    {
    grammar::convert_ligatures_and_diacritics cld;

    SECTION("Boundaries")
        {
        CHECK(cld(L"e\u0301clair"));
        CHECK(cld.get_conversion() == L"éclair");

        CHECK(cld(L"claire\u0301"));
        CHECK(cld.get_conversion() == L"clairé");

        CHECK(cld(L"clae\u0301ir"));
        CHECK(cld.get_conversion() == L"claéir");
        }

    SECTION("graves")
        {
        CHECK(cld(L"a\u0300A\u0300e\u0300E\u0300i\u0300I\u0300o\u0300O\u0300u\u0300U\u0300"));
        CHECK(cld.get_conversion() == L"àÀèÈìÌòÒùÙ");
        }
    SECTION("acutes")
        {
        CHECK(cld(L"a\u0301A\u0301e\u0301E\u0301i\u0301I\u0301o\u0301O\u0301u\u0301U\u0301y\u0301Y\u0301c\u0301C\u0301s\u0301S\u0301z\u0301Z\u0301"));
        CHECK(cld.get_conversion() == L"áÁéÉíÍóÓúÚýÝćĆśŚźŹ");
        }
    SECTION("hats")
        {
        CHECK(cld(L"a\u0302A\u0302e\u0302E\u0302i\u0302I\u0302o\u0302O\u0302u\u0302U\u0302"));
        CHECK(cld.get_conversion() == L"âÂêÊîÎôÔûÛ");
        }
    SECTION("umlauts")
        {
        CHECK(cld(L"a\u0308A\u0308e\u0308E\u0308i\u0308I\u0308o\u0308O\u0308u\u0308U\u0308"));
        CHECK(cld.get_conversion() == L"äÄëËïÏöÖüÜ");
        }
    SECTION("rings")
        {
        CHECK(cld(L"a\u030AA\u030Au\u030AU\u030A"));
        CHECK(cld.get_conversion() == std::wstring{ L"åÅůŮ" });
        }
    SECTION("short solidus")
        {
        CHECK(cld(L"l\u0337L\u0337"));
        CHECK(cld.get_conversion() == std::wstring{ L"łŁ" });
        }
    SECTION("long solidus")
        {
        CHECK(cld(L"o\u0338O\u0338"));
        CHECK(cld.get_conversion() == std::wstring{ L"øØ" });
        }
    SECTION("tildes")
        {
        CHECK(cld(L"n\u0303N\u0303"));
        CHECK(cld.get_conversion() == std::wstring{ L"ñÑ" });
        }
    SECTION("caron")
        {
        CHECK(cld(L"c\u030CC\u030Cn\u030CN\u030Cr\u030CR\u030Cs\u030CS\u030Cz\u030CZ\u030Ct\u030CT\u030Cd\u030CD\u030Ce\u030CE\u030C"));
        CHECK(cld.get_conversion() == std::wstring{ L"čČňŇřŘšŠžŽťŤďĎěĚ" });
        }
    SECTION("cedilla")
        {
        CHECK(cld(L"c\u0327C\u0327"));
        CHECK(cld.get_conversion() == std::wstring{ L"çÇ" });
        }
    SECTION("retroflex hook")
        {
        CHECK(cld(L"e\u0322E\u0322"));
        CHECK(cld.get_conversion() == std::wstring{ L"ęĘ" });
        }
    SECTION("dot above")
        {
        CHECK(cld(L"z\u0307Z\u0307"));
        CHECK(cld.get_conversion() == std::wstring{ L"żŻ" });
        }
    SECTION("dot below")
        {
        CHECK(cld(L"o\u0323O\u0323"));
        CHECK(cld.get_conversion() == std::wstring{ L"\x1ECD\x1ECC" });
        }
    }

TEST_CASE("Ligatures", "[diacritics]")
    {
    grammar::convert_ligatures_and_diacritics cld;

    SECTION("Boundaries")
        {
        CHECK(cld(L"ﬄoat"));
        CHECK(cld.get_conversion() == L"ffloat");

        CHECK(cld(L"Rootbeer ﬄoat!"));
        CHECK(cld.get_conversion() == L"Rootbeer ffloat!");

        CHECK(cld(L"ﬀ"));
        CHECK(cld.get_conversion() == L"ff");
        // no text
        CHECK_FALSE(cld(L""));
        CHECK(cld.get_conversion().empty());
        // no conversion
        CHECK_FALSE(cld(L"ff"));
        CHECK(cld.get_conversion().empty());
        }

    SECTION("All")
        {
        CHECK(cld(L"ﬀﬁﬂﬃﬄﬅﬆ"));
        CHECK(cld.get_conversion() == std::wstring{ L"fffiflffifflftst" });
        }
    }

// NOLINTEND
// clang-format on
