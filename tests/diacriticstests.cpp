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
        // Portuguese/Spanish tilde vowels and consonants
        CHECK(cld(L"a\u0303A\u0303n\u0303N\u0303o\u0303O\u0303"));
        CHECK(cld.get_conversion() == std::wstring{ L"ãÃñÑõÕ" });
        }
    SECTION("caron")
        {
        CHECK(cld(L"c\u030CC\u030Cn\u030CN\u030Cr\u030CR\u030Cs\u030CS\u030Cz\u030CZ\u030Ct\u030CT\u030Cd\u030CD\u030Ce\u030CE\u030C"));
        CHECK(cld.get_conversion() == std::wstring{ L"čČňŇřŘšŠžŽťŤďĎěĚ" });
        }
    SECTION("cedilla")
        {
        CHECK(cld(L"c\u0327C\u0327s\u0327S\u0327t\u0327T\u0327"));
        CHECK(cld.get_conversion() == std::wstring{ L"çÇşŞţŢ" });
        }
    SECTION("ogonek")
        {
        // ogonek mark (used in Polish/Lithuanian)
        CHECK(cld(L"a\u0328A\u0328e\u0328E\u0328i\u0328I\u0328u\u0328U\u0328"));
        CHECK(cld.get_conversion() == std::wstring{ L"ąĄęĘįĮųŲ" });
        }
    SECTION("dot above")
        {
        CHECK(cld(L"z\u0307Z\u0307"));
        CHECK(cld.get_conversion() == std::wstring{ L"żŻ" });
        }
    SECTION("dot below")
        {
        CHECK(cld(L"a\u0323A\u0323e\u0323E\u0323i\u0323I\u0323o\u0323O\u0323u\u0323U\u0323"));
        CHECK(cld.get_conversion() == std::wstring{ L"ạẠẹẸịỊọỌụỤ" });
        }
    SECTION("macron")
        {
        CHECK(cld(L"a\u0304A\u0304e\u0304E\u0304i\u0304I\u0304o\u0304O\u0304u\u0304U\u0304"));
        CHECK(cld.get_conversion() == std::wstring{ L"āĀēĒīĪōŌūŪ" });
        }
    SECTION("breve")
        {
        CHECK(cld(L"a\u0306A\u0306"));
        CHECK(cld.get_conversion() == std::wstring{ L"ăĂ" });
        }
    SECTION("double acute")
        {
        CHECK(cld(L"o\u030BO\u030Bu\u030BU\u030B"));
        CHECK(cld.get_conversion() == std::wstring{ L"őŐűŰ" });
        }
    SECTION("comma below")
        {
        CHECK(cld(L"s\u0326S\u0326t\u0326T\u0326"));
        CHECK(cld.get_conversion() == std::wstring{ L"șȘțȚ" });
        }
    SECTION("hook above")
        {
        // Vietnamese hook above
        CHECK(cld(L"a\u0309A\u0309e\u0309E\u0309i\u0309I\u0309o\u0309O\u0309u\u0309U\u0309y\u0309Y\u0309"));
        CHECK(cld.get_conversion() == std::wstring{ L"ảẢẻẺỉỈỏỎủỦỷỶ" });
        }
    SECTION("horn")
        {
        // Vietnamese horn
        CHECK(cld(L"o\u031BO\u031Bu\u031BU\u031B"));
        CHECK(cld.get_conversion() == std::wstring{ L"ơƠưƯ" });
        }
    SECTION("Turkish breve")
        {
        // Turkish G with breve
        CHECK(cld(L"g\u0306G\u0306"));
        CHECK(cld.get_conversion() == std::wstring{ L"ğĞ" });
        }
    SECTION("Turkish dot above")
        {
        // Turkish I with dot above (capital only - lowercase i already has dot)
        CHECK(cld(L"I\u0307"));
        CHECK(cld.get_conversion() == std::wstring{ L"İ" });
        }
    SECTION("Polish N acute")
        {
        CHECK(cld(L"n\u0301N\u0301"));
        CHECK(cld.get_conversion() == std::wstring{ L"ńŃ" });
        }
    SECTION("Slovak L caron")
        {
        CHECK(cld(L"l\u030CL\u030C"));
        CHECK(cld.get_conversion() == std::wstring{ L"ľĽ" });
        }
    SECTION("Slovak L acute")
        {
        CHECK(cld(L"l\u0301L\u0301"));
        CHECK(cld.get_conversion() == std::wstring{ L"ĺĹ" });
        }
    SECTION("Welsh circumflex")
        {
        // Welsh W and Y with circumflex
        CHECK(cld(L"w\u0302W\u0302y\u0302Y\u0302"));
        CHECK(cld.get_conversion() == std::wstring{ L"ŵŴŷŶ" });
        }
    SECTION("Maltese dot above")
        {
        // Maltese C and G with dot above
        CHECK(cld(L"c\u0307C\u0307g\u0307G\u0307"));
        CHECK(cld.get_conversion() == std::wstring{ L"ċĊġĠ" });
        }
    SECTION("Maltese H stroke")
        {
        // Maltese H with stroke
        CHECK(cld(L"h\u0335H\u0335"));
        CHECK(cld.get_conversion() == std::wstring{ L"ħĦ" });
        }
    SECTION("Slovak R acute")
        {
        // Slovak R with acute
        CHECK(cld(L"r\u0301R\u0301"));
        CHECK(cld.get_conversion() == std::wstring{ L"ŕŔ" });
        }
    SECTION("Croatian D stroke")
        {
        // Croatian D with stroke
        CHECK(cld(L"d\u0335D\u0335"));
        CHECK(cld.get_conversion() == std::wstring{ L"đĐ" });
        }
    SECTION("Welsh W grave")
        {
        // Welsh W with grave
        CHECK(cld(L"w\u0300W\u0300"));
        CHECK(cld.get_conversion() == std::wstring{ L"ẁẀ" });
        }
    SECTION("Welsh W acute")
        {
        // Welsh W with acute
        CHECK(cld(L"w\u0301W\u0301"));
        CHECK(cld.get_conversion() == std::wstring{ L"ẃẂ" });
        }
    SECTION("Welsh W diaeresis")
        {
        // Welsh W with diaeresis
        CHECK(cld(L"w\u0308W\u0308"));
        CHECK(cld.get_conversion() == std::wstring{ L"ẅẄ" });
        }
    SECTION("Welsh Y grave")
        {
        // Welsh Y with grave
        CHECK(cld(L"y\u0300Y\u0300"));
        CHECK(cld.get_conversion() == std::wstring{ L"ỳỲ" });
        }
    SECTION("Latvian cedilla")
        {
        // Latvian G, K, L, N with cedilla
        CHECK(cld(L"g\u0327G\u0327k\u0327K\u0327l\u0327L\u0327n\u0327N\u0327"));
        CHECK(cld.get_conversion() == std::wstring{ L"ģĢķĶļĻņŅ" });
        }
    SECTION("Lithuanian E dot above")
        {
        // Lithuanian E with dot above
        CHECK(cld(L"e\u0307E\u0307"));
        CHECK(cld.get_conversion() == std::wstring{ L"ėĖ" });
        }
    SECTION("Esperanto circumflex")
        {
        // Esperanto C, G, H, J, S with circumflex
        CHECK(cld(L"c\u0302C\u0302g\u0302G\u0302h\u0302H\u0302j\u0302J\u0302s\u0302S\u0302"));
        CHECK(cld.get_conversion() == std::wstring{ L"ĉĈĝĜĥĤĵĴŝŜ" });
        }
    SECTION("Esperanto U breve")
        {
        // Esperanto U with breve
        CHECK(cld(L"u\u0306U\u0306"));
        CHECK(cld.get_conversion() == std::wstring{ L"ŭŬ" });
        }
    SECTION("Pinyin caron vowels")
        {
        // Pinyin A, I, O, U with caron
        CHECK(cld(L"a\u030CA\u030Ci\u030CI\u030Co\u030CO\u030Cu\u030CU\u030C"));
        CHECK(cld.get_conversion() == std::wstring{ L"ǎǍǐǏǒǑǔǓ" });
        }
    SECTION("French Y diaeresis")
        {
        // French Y with diaeresis
        CHECK(cld(L"y\u0308Y\u0308"));
        CHECK(cld.get_conversion() == std::wstring{ L"ÿŸ" });
        }
    SECTION("Sami T stroke")
        {
        // Sami T with stroke
        CHECK(cld(L"t\u0335T\u0335"));
        CHECK(cld.get_conversion() == std::wstring{ L"ŧŦ" });
        }
    SECTION("Old Irish dot above")
        {
        // Old Irish/Gaelic consonants with dot above (lenition marker)
        CHECK(cld(L"b\u0307B\u0307d\u0307D\u0307f\u0307F\u0307m\u0307M\u0307p\u0307P\u0307s\u0307S\u0307t\u0307T\u0307"));
        CHECK(cld.get_conversion() == std::wstring{ L"ḃḂḋḊḟḞṁṀṗṖṡṠṫṪ" });
        }
    SECTION("Russian diaeresis")
        {
        // Russian Е/е + diaeresis -> Ё/ё
        CHECK(cld(L"е\u0308Е\u0308"));
        CHECK(cld.get_conversion() == std::wstring{ L"ёЁ" });
        }
    SECTION("Russian breve")
        {
        // Russian И/и + breve -> Й/й
        CHECK(cld(L"и\u0306И\u0306"));
        CHECK(cld.get_conversion() == std::wstring{ L"йЙ" });
        }
    SECTION("Ukrainian diaeresis")
        {
        // Ukrainian І/і + diaeresis -> Ї/ї
        CHECK(cld(L"і\u0308І\u0308"));
        CHECK(cld.get_conversion() == std::wstring{ L"їЇ" });
        }
    SECTION("Belarusian breve")
        {
        // Belarusian У/у + breve -> Ў/ў
        CHECK(cld(L"у\u0306У\u0306"));
        CHECK(cld.get_conversion() == std::wstring{ L"ўЎ" });
        }
    SECTION("Macedonian acute")
        {
        // Macedonian Г/г + acute -> Ѓ/ѓ, К/к + acute -> Ќ/ќ
        CHECK(cld(L"г\u0301Г\u0301к\u0301К\u0301"));
        CHECK(cld.get_conversion() == std::wstring{ L"ѓЃќЌ" });
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
