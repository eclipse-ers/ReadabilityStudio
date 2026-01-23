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
// cppcheck-suppress-file redundantInitialization

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "../src/indexing/tokenize.h"
#include "../src/indexing/word_list.h"

TEST_CASE("C++ tokenize", "[document]")
    {
    SECTION("Regular")
        {
        std::wstring text{ L"C++. We also." };
        tokenize::document_tokenize<> tokenize(text.data(), text.length(), false, false, false,
                                               false);
        auto pos = tokenize();
        std::wstring firstWord(pos, tokenize.get_current_word_length());
        CHECK(firstWord == L"C++");
        CHECK(tokenize.get_current_sentence_index() == 0);
        pos = tokenize();
        CHECK(tokenize.get_current_sentence_index() == 1);
        }
    SECTION("Not word")
        {
        std::wstring text{ L"C++Plz. We also." };
        tokenize::document_tokenize<> tokenize(text.data(), text.length(), false, false, false,
                                               false);
        auto pos = tokenize();
        CHECK(std::wstring{ pos, tokenize.get_current_word_length() } == L"C");
        CHECK(tokenize.get_current_sentence_index() == 0);
        pos = tokenize();
        CHECK(std::wstring{ pos, tokenize.get_current_word_length() } == L"Plz");
        CHECK(tokenize.get_current_sentence_index() == 0);
        pos = tokenize();
        CHECK(std::wstring{ pos, tokenize.get_current_word_length() } == L"We");
        CHECK(tokenize.get_current_sentence_index() == 1);
        }
    SECTION("At end")
        {
        std::wstring text{ L"C++" };
        tokenize::document_tokenize<> tokenize(text.data(), text.length(), false, false, false,
                                               false);
        auto pos = tokenize();
        std::wstring firstWord(pos, tokenize.get_current_word_length());
        CHECK(firstWord == L"C++");
        CHECK(tokenize.get_current_sentence_index() == 0);
        }
    }

TEST_CASE("Document tokenize", "[document]")
    {
    SECTION("NULL")
        {
        tokenize::document_tokenize<> tokenize(nullptr, 5, false, false, false, false);
        [[maybe_unused]] auto b = tokenize();//just see if we don't crash
        }
    SECTION("Double Apos")
        {
        tokenize::document_tokenize<> tokenize(L"''Schools'' are nice.", 30, false, false, false, false);
        auto pos = tokenize();
        std::wstring firstWord(pos, tokenize.get_current_word_length());
        CHECK(firstWord == L"Schools");
        //move to next word
        pos = tokenize();
        auto punct = tokenize.get_punctuation();
        CHECK(4 == punct.size());
        CHECK(L'\'' == punct[2].get_punctuation_mark());
        CHECK(L'\'' == punct[3].get_punctuation_mark());
        CHECK(punct[2].is_connected_to_previous_word());
        CHECK(punct[3].is_connected_to_previous_word());
        CHECK(std::wstring(pos, tokenize.get_current_word_length()) == L"are");
        }
    SECTION("Single Apos Quoted Word")
        {
        // trailing apostrophe should be punctuation, not part of the word
        word_list knownSpellings;
        knownSpellings.add_word(L"main");
        tokenize::document_tokenize<> tokenizer(L"from 'main' branch", 18, false, false, false, false);
        tokenizer.set_known_spellings(&knownSpellings);
        auto pos = tokenizer();
        CHECK(std::wstring(pos, tokenizer.get_current_word_length()) == std::wstring{ L"from" });
        pos = tokenizer();
        // 'main' should be tokenized as just "main", not "main'"
        CHECK(std::wstring(pos, tokenizer.get_current_word_length()) == std::wstring{ L"main" });
        pos = tokenizer();
        CHECK(std::wstring(pos, tokenizer.get_current_word_length()) == std::wstring{ L"branch" });
        }
    SECTION("Character Line Breaks")
        {
        tokenize::document_tokenize<> tokenize(L"Here\n+++\nA new paragraph here.", 30, false, false, false, false);
        [[maybe_unused]] auto b = tokenize();
        CHECK(tokenize.get_current_sentence_index() == 0);
        CHECK(tokenize.get_current_paragraph_index() == 0);
        b = tokenize();
        CHECK(tokenize.get_current_sentence_index() == 1);
        CHECK(tokenize.get_current_paragraph_index() == 1);
        CHECK(tokenize.get_punctuation().size() == 0);//throw out the "+++"
        }
    SECTION("Character Line Breaks Start Of File")
        {
        tokenize::document_tokenize<> tokenize(L"+++\nA new paragraph here.", 25, false, false, false, false);
        const wchar_t* token = tokenize();
        CHECK(std::wcsncmp(token, L"A", tokenize.get_current_word_length()) == 0);
        while (tokenize() ) {};
        CHECK(tokenize.get_punctuation().size() == 0);//throw out the "+++"
        }
    SECTION("Character Line Breaks End Of File")
        {
        tokenize::document_tokenize<> tokenize(L"Here+++", 7, false, false, false, false);
        const wchar_t* token = tokenize();
        CHECK(std::wcsncmp(token, L"Here", tokenize.get_current_word_length()) == 0);
        while (tokenize() ) {};
        CHECK(tokenize.get_punctuation().size() == 0);//throw out the "+++"
        }
    SECTION("Character Line Breaks End Of File With Eol")
        {
        tokenize::document_tokenize<> tokenize(L"Here+++\n", 8, false, false, false, false);
        const wchar_t* token = tokenize();
        CHECK(std::wcsncmp(token, L"Here", tokenize.get_current_word_length()) == 0);
        while (tokenize() ) {};
        CHECK(tokenize.get_punctuation().size() == 0);//throw out the "+++"
        }
    SECTION("Character Line Breaks With Space")
        {
        tokenize::document_tokenize<> tokenize(L"Here\n +++\nAn new paragraph here.", 32, false, false, false, false);
        [[maybe_unused]] auto b = tokenize();
        CHECK(tokenize.get_current_sentence_index() == 0);
        CHECK(tokenize.get_current_paragraph_index() == 0);
        const wchar_t* token = tokenize();
        CHECK(std::wcsncmp(token, L"An", tokenize.get_current_word_length()) == 0);
        CHECK(tokenize.get_current_sentence_index() == 1);
        CHECK(tokenize.get_current_paragraph_index() == 1);
        CHECK(tokenize.get_punctuation().size() == 0);//throw out the "+++"
        }
    SECTION("Character Line Breaks At End Of Line Simple")
        {
        const wchar_t* value = L"***\nA new paragraph here.";
        tokenize::document_tokenize<> tokenize(value, std::wcslen(value), false, false, false, false);
        [[maybe_unused]] auto b = tokenize();
        CHECK(tokenize.is_trailing_formatted_line_separator(value).first);
        CHECK(tokenize.is_trailing_formatted_line_separator(value).second == 3);
        }
    SECTION("Character Line Breaks At End Of Line Simple Not At Eol")
        {
        const wchar_t* value = L"***A new paragraph here.";
        tokenize::document_tokenize<> tokenize(value, std::wcslen(value), false, false, false, false);
        [[maybe_unused]] auto b = tokenize();
        CHECK(tokenize.is_trailing_formatted_line_separator(value).first == false);
        CHECK(tokenize.is_trailing_formatted_line_separator(value).second == 0);
        }
    SECTION("Character Line Breaks At End Of Line Simple At Eod")
        {
        const wchar_t* value = L"***\n";
        tokenize::document_tokenize<> tokenize(value, std::wcslen(value), false, false, false, false);
        [[maybe_unused]] auto b = tokenize();
        CHECK(tokenize.is_trailing_formatted_line_separator(value).first);
        CHECK(tokenize.is_trailing_formatted_line_separator(value).second == 3);
        }
    SECTION("Character Line Breaks At End Of Line Simple At Eod No Eol")
        {
        const wchar_t* value = L"***";
        tokenize::document_tokenize<> tokenize(value, std::wcslen(value), false, false, false, false);
        [[maybe_unused]] auto b = tokenize();
        CHECK(tokenize.is_trailing_formatted_line_separator(value).first);
        CHECK(tokenize.is_trailing_formatted_line_separator(value).second == 3);
        }
    SECTION("Character Line Breaks At End Of Line")
        {
        tokenize::document_tokenize<> tokenize(L"Note***\nA new paragraph here.", 29, false, false, false, false);
        [[maybe_unused]] auto b = tokenize();
        CHECK(tokenize.get_current_sentence_index() == 0);
        CHECK(tokenize.get_current_paragraph_index() == 0);
        b = tokenize();
        CHECK(tokenize.get_current_sentence_index() == 1);
        CHECK(tokenize.get_current_paragraph_index() == 1);
        CHECK(tokenize.get_punctuation().size() == 0);//throw out the "***"
        }
    SECTION("EOL")
        {
        const wchar_t testText[] = L"Header  \n\nNext paragraph.  Another sentence\nright here.";
        tokenize::document_tokenize<> tokenize(testText, wcslen(testText), false, false, false, false);
        [[maybe_unused]] auto b = tokenize();
        CHECK(tokenize.is_at_eol());
        b = tokenize();
        CHECK(tokenize.is_at_eol() == false);
        b = tokenize();
        CHECK(tokenize.is_at_eol() == false);
        b = tokenize();
        CHECK(tokenize.is_at_eol() == false);
        b = tokenize();
        CHECK(tokenize.is_at_eol());
        b = tokenize();
        CHECK(tokenize.is_at_eol() == false);
        b = tokenize();
        CHECK(tokenize.is_at_eol() == false);
        }
    SECTION("Leading EOL Count")
        {
        const wchar_t testText[] = L"Header  \n\nNext paragraph.  Another sentence.\nright here.";
        tokenize::document_tokenize<> tokenize(testText, wcslen(testText), false, false, false, false);
        [[maybe_unused]] auto b = tokenize();
        CHECK(tokenize.get_current_leading_end_of_line_count() == 0);
        b = tokenize();
        CHECK(tokenize.get_current_leading_end_of_line_count() == 2);
        b = tokenize();
        CHECK(tokenize.get_current_leading_end_of_line_count() == 0);
        b = tokenize();
        CHECK(tokenize.get_current_leading_end_of_line_count() == 0);
        b = tokenize();
        CHECK(tokenize.get_current_leading_end_of_line_count() == 0);
        b = tokenize();
        CHECK(tokenize.get_current_leading_end_of_line_count() == 1);
        b = tokenize();
        CHECK(tokenize.get_current_leading_end_of_line_count() == 0);
        }
    SECTION("EOS Punctuation")
        {
        const wchar_t testText[] = L"Header  \n\nNext paragraph.  Another sentence?\nright here!";
        tokenize::document_tokenize<> tokenize(testText, wcslen(testText), false, false, false, false);
        [[maybe_unused]] auto b = tokenize();
        CHECK(tokenize.get_current_sentence_ending_punctuation() == L' ');
        b = tokenize();
        CHECK(tokenize.get_current_sentence_ending_punctuation() == L' ');
        b = tokenize();
        CHECK(tokenize.get_current_sentence_ending_punctuation() == L' ');
        b = tokenize();
        CHECK(tokenize.get_current_sentence_ending_punctuation() == L'.');
        b = tokenize();
        CHECK(tokenize.get_current_sentence_ending_punctuation() == L' ');
        b = tokenize();
        CHECK(tokenize.get_current_sentence_ending_punctuation() == L'?');
        b = tokenize();
        CHECK(tokenize.get_current_sentence_ending_punctuation() == L' ');
        b = tokenize();
        CHECK(tokenize.get_current_sentence_ending_punctuation() == L'!');
        }
    SECTION("Is Numeric")
        {
        const wchar_t testText[] = L"Header  \n\nNext 7.2.  Another sentence?\n73 here! 86.";
        tokenize::document_tokenize<> tokenize(testText, wcslen(testText), false, false, false, false);
        [[maybe_unused]] auto b = tokenize();
        CHECK(tokenize.is_numeric() == false);
        b = tokenize();
        CHECK(tokenize.is_numeric() == false);
        b = tokenize();
        CHECK(tokenize.is_numeric());
        b = tokenize();
        CHECK(tokenize.is_numeric() == false);
        b = tokenize();
        CHECK(tokenize.is_numeric() == false);
        b = tokenize();
        CHECK(tokenize.is_numeric());
        b = tokenize();
        CHECK(tokenize.is_numeric() == false);
        b = tokenize();
        CHECK(tokenize.is_numeric());
        b = tokenize();
        CHECK(tokenize.is_numeric() == false);
        }
    SECTION("Hyphenated")
        {
        const wchar_t testText[] = L"Header \n\nNext pump-\nkin Another sent-\n\nence?\nLast sentence.";
        tokenize::document_tokenize<> tokenize(testText, wcslen(testText), false, false, false, false);
        [[maybe_unused]] auto b = tokenize();
        CHECK(tokenize.is_split_word() == false);
        b = tokenize();
        CHECK(tokenize.is_split_word() == false);
        b = tokenize();
        CHECK(tokenize.is_split_word());//pump-\nkin
        b = tokenize();
        CHECK(tokenize.is_split_word() == false);
        b = tokenize();
        CHECK(tokenize.is_split_word());//sent-\n\nence
        b = tokenize();
        CHECK(tokenize.is_split_word() == false);
        b = tokenize();
        CHECK(tokenize.is_split_word() == false);
        }
    SECTION("Hyphenated Same Line")
        {
        const wchar_t testText[] = L"pro- gramming.";
        tokenize::document_tokenize<> tokenize(testText, wcslen(testText), false, false, false, false);
        [[maybe_unused]] auto b = tokenize();
        CHECK(tokenize.is_split_word());
        }
    SECTION("Punctuation")
        {
        const wchar_t testText[] = L"Header, \n\nNext, pump-\nkin 7.3 sent-\n\nence?\nLast sentence.";
        tokenize::document_tokenize<> tokenize(testText, wcslen(testText), false, false, false, false);
        while (tokenize() ) {};
        CHECK(tokenize.get_current_paragraph_index() == 2);
        }
    SECTION("Word Length")
        {
        const wchar_t testText[] = L"Header,  \n\nNext 7.2.  Another sentence?\n73 here!";
        tokenize::document_tokenize<> tokenize(testText, wcslen(testText), false, false, false, false);
        [[maybe_unused]] auto b = tokenize();
        CHECK(tokenize.get_current_word_length() == 6);
        b = tokenize();
        CHECK(tokenize.get_current_word_length() == 4);
        b = tokenize();
        CHECK(tokenize.get_current_word_length() == 3);
        b = tokenize();
        CHECK(tokenize.get_current_word_length() == 7);
        b = tokenize();
        CHECK(tokenize.get_current_word_length() == 8);
        b = tokenize();
        CHECK(tokenize.get_current_word_length() == 2);
        b = tokenize();
        CHECK(tokenize.get_current_word_length() == 4);
        }
    SECTION("Sentence Position")
        {
        const wchar_t testText[] = L"Header/ \n\nNext pump-\nkin Another sent-\n\nence?\nLast sentence.";
        tokenize::document_tokenize<> tokenize(testText, wcslen(testText), false, false, false, false);
        [[maybe_unused]] auto b = tokenize();
        CHECK(tokenize.get_sentence_position() == 0);
        b = tokenize();
        CHECK(tokenize.get_sentence_position() == 0);
        b = tokenize();
        CHECK(tokenize.get_sentence_position() == 1);
        b = tokenize();
        CHECK(tokenize.get_sentence_position() == 2);
        b = tokenize();
        CHECK(tokenize.get_sentence_position() == 3);
        b = tokenize();
        CHECK(tokenize.get_sentence_position() == 0);
        b = tokenize();
        CHECK(tokenize.get_sentence_position() == 1);
        }
    SECTION("Sentence Index")
        {
        const wchar_t testText[] = L"Header/ \n\nNext pump-\nkin Another sent-\n\nence?\nLast sentence.";
        tokenize::document_tokenize<> tokenize(testText, wcslen(testText), false, false, false, false);
        [[maybe_unused]] auto b = tokenize();
        CHECK(tokenize.get_current_sentence_index() == 0);
        b = tokenize();
        CHECK(tokenize.get_current_sentence_index() == 1);
        b = tokenize();
        CHECK(tokenize.get_current_sentence_index() == 1);
        b = tokenize();
        CHECK(tokenize.get_current_sentence_index() == 1);
        b = tokenize();
        CHECK(tokenize.get_current_sentence_index() == 1);
        b = tokenize();
        CHECK(tokenize.get_current_sentence_index() == 2);
        b = tokenize();
        CHECK(tokenize.get_current_sentence_index() == 2);
        }
    SECTION("Paragraph Index")
        {
        const wchar_t testText[] = L"Header/ \n\nNext pump-\nkin Another sent-\n\nence?\nLast sentence.";
        tokenize::document_tokenize<> tokenize(testText, wcslen(testText), false, false, false, false);
        [[maybe_unused]] auto b = tokenize();
        CHECK(tokenize.get_current_paragraph_index() == 0);
        b = tokenize();
        CHECK(tokenize.get_current_paragraph_index() == 1);
        b = tokenize();
        CHECK(tokenize.get_current_paragraph_index() == 1);
        b = tokenize();
        CHECK(tokenize.get_current_paragraph_index() == 1);
        b = tokenize();
        CHECK(tokenize.get_current_paragraph_index() == 1);
        b = tokenize();
        CHECK(tokenize.get_current_paragraph_index() == 2);
        b = tokenize();
        CHECK(tokenize.get_current_paragraph_index() == 2);
        }
    SECTION("Sentences Must Be Uppercased")
        {
        const wchar_t testText[] = L"Header\n\nNext pumpkin. another sentence?\nLast sentence.";
            {
            tokenize::document_tokenize<> tokenize(testText, wcslen(testText), false, false, false, true);
            [[maybe_unused]] auto b = tokenize();
            CHECK(tokenize.get_current_sentence_index() == 0);
            b = tokenize();
            CHECK(tokenize.get_current_sentence_index() == 1);
            b = tokenize();
            CHECK(tokenize.get_current_sentence_index() == 1);
            b = tokenize();
            CHECK(tokenize.get_current_sentence_index() == 1);
            b = tokenize();
            CHECK(tokenize.get_current_sentence_index() == 1);
            b = tokenize();
            CHECK(tokenize.get_current_sentence_index() == 2);
            b = tokenize();
            CHECK(tokenize.get_current_sentence_index() == 2);
            }
            {
            tokenize::document_tokenize<> tokenize(testText, wcslen(testText), false, false, false, false);
            [[maybe_unused]] auto b = tokenize();
            CHECK(tokenize.get_current_sentence_index() == 0);
            b = tokenize();
            CHECK(tokenize.get_current_sentence_index() == 1);
            b = tokenize();
            CHECK(tokenize.get_current_sentence_index() == 1);
            b = tokenize();
            CHECK(tokenize.get_current_sentence_index() == 2);
            b = tokenize();
            CHECK(tokenize.get_current_sentence_index() == 2);
            b = tokenize();
            CHECK(tokenize.get_current_sentence_index() == 3);
            b = tokenize();
            CHECK(tokenize.get_current_sentence_index() == 3);
            }
        }
    SECTION("Sentences Can Be Lowercased With Space")
        {
        const wchar_t testText[] = L"Header\n\nNext pumpkin.  another sentence?\nLast sentence.";
        tokenize::document_tokenize<> tokenize(testText, wcslen(testText), false, false, false, false);
        [[maybe_unused]] auto b = tokenize();
        CHECK(tokenize.get_current_sentence_index() == 0);
        b = tokenize();
        CHECK(tokenize.get_current_sentence_index() == 1);
        b = tokenize();
        CHECK(tokenize.get_current_sentence_index() == 1);
        b = tokenize();
        CHECK(tokenize.get_current_sentence_index() == 2);
        b = tokenize();
        CHECK(tokenize.get_current_sentence_index() == 2);
        b = tokenize();
        CHECK(tokenize.get_current_sentence_index() == 3);
        b = tokenize();
        CHECK(tokenize.get_current_sentence_index() == 3);
        }
    SECTION("Ignore Indenting")
        {
        const wchar_t testText[] = L"Header\n\tNext pumpkin Another sentence?\nLast sentence.";
            {
            tokenize::document_tokenize<> tokenize(testText, wcslen(testText), false, false, true, false);
            [[maybe_unused]] auto b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 0);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 0);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 0);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 0);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 0);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 1);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 1);
            }
            {
            tokenize::document_tokenize<> tokenize(testText, wcslen(testText), false, false, false, false);
            [[maybe_unused]] auto b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 0);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 1);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 1);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 1);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 1);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 2);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 2);
            }
        }
    SECTION("Blank Lines")
        {
        const wchar_t testText[] = L"Header\n\nNext pumpkin Another sentence?\nLast sentence.";
            {
            tokenize::document_tokenize<> tokenize(testText, wcslen(testText), false, true, false, false);
            [[maybe_unused]] auto b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 0);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 0);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 0);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 0);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 0);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 1);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 1);
            }
            {
            tokenize::document_tokenize<> tokenize(testText, wcslen(testText), false, false, false, false);
            [[maybe_unused]] auto b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 0);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 1);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 1);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 1);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 1);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 2);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 2);
            }
        }
    SECTION("New Line Is New Paragraph")
        {
        const wchar_t testText[] = L"Header\n\nNext pumpkin Another sentence\nsame paragraph.";
            {
            tokenize::document_tokenize<> tokenize(testText, wcslen(testText), true, false, false, false);
            [[maybe_unused]] auto b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 0);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 1);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 1);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 1);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 1);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 2);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 2);
            }
            {
            tokenize::document_tokenize<> tokenize(testText, wcslen(testText), false, false, false, false);
            [[maybe_unused]] auto b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 0);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 1);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 1);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 1);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 1);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 1);
            b = tokenize();
            CHECK(tokenize.get_current_paragraph_index() == 1);
            }
        }
    }

// NOLINTEND
// clang-format on
