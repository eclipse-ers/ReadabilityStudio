/********************************************************************************
 * Copyright (c) 2005-2026 Blake Madden
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

/*== == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == =*\
||                                                                                              ||
||                    +----------------------------------------------+                          ||
||                    | /   /   /   /   /   /      @@@@@@@@@@@@@@@@@@|                          ||
||                    |                       /      @@@@@@@@@@@@@@@@|                          ||
||                    |                  /        /    @@@@@@@@@@@@@@|                          ||
||                    |              /        /          @@@@@@@@@@@@|                          ||
||                    |          /         /        /      @@@@@@@@@@|                          ||
||                    |      /          /         /    /     @@@@@@@@|                          ||
||                    |              /          /     /     /  @@@@@@|                          ||
||                    |           /           /      /      /    @@@@|                          ||
||                    |@       /            /       /       /      @@|                          ||
||                    |@@@                /        /        /       @|                          ||
||                    |@@@@@@           /         /         /        |                          ||
||                    |@@@@@@@@@                 /          /        |                          ||
||                    |@@@@@@@@@@@              /           /        |                          ||
||                    |@@@@@@@@@@@@@@          /            /        |                          ||
||                    |@@@@@@@@@@@@@@@@@                    /        |                          ||
||                    |@@@@@@@@@@@@@@@@@@@@                 /        |                          ||
||                    |@@@@@@@@@@@@@@@@@@@@@@@                       |                          ||
||                    +----------------------------------------------+                          ||
||                                                                                              ||
||    RRRRR   EEEEE   AAAAA   DDDD    AAAAA   BBBBB   IIIII   L       IIIII   TTTTT    Y   Y    ||
||    R   R   E       A   A   D   D   A   A   B   B     I     L         I       T      Y   Y    ||
||    RRRR    EEEE    AAAAA   D   D   AAAAA   BBBB      I     L         I       T       Y Y     ||
||    R  R    E       A   A   D   D   A   A   B   B     I     L         I       T        Y      ||
||    R   R   EEEEE   A   A   DDDD    A   A   BBBBB   IIIII   LLLLL   IIIII     T        Y      ||
||                                                                                              ||
||                                            SSSS    TTTTT   U   U   DDDD    IIIII   OOOO      ||
||                                            S         T     U   U   D   D     I    O    O     ||
||                                            SSSS      T     U   U   D   D     I    O    O     ||
||                                               S      T     U   U   D   D     I    O    O     ||
||                                            SSSS      T     UUUUU   DDDD    IIIII   OOOO      ||
||                                                                                              ||
\*== == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == =*/

#ifndef WORD_COLLECTION_TEXT_FORMATTING_H
#define WORD_COLLECTION_TEXT_FORMATTING_H

#include "../Wisteria-Dataviz/src/import/html_encode.h"
#include "../Wisteria-Dataviz/src/import/rtf_encode.h"
#include "../app/optionenums.h"
#include "../indexing/word_collection.h"
#include <map>
#include <unordered_map>

template<typename documentT, typename highlightDeterminantT>
size_t FormatWordCollectionHighlightedWords(
    const std::shared_ptr<documentT>& theDocument, const highlightDeterminantT& shouldHighlight,
    std::wstring& text, const std::wstring& headerSection, const std::wstring& endSection,
    const std::wstring& legend, const std::wstring& ignoreHighlightBegin,
    const std::wstring& ignoreHighlightEnd, const std::wstring& tabSymbol,
    const std::wstring& newLine, const bool highlightIncompleteSentences,
    const bool considerOnlyListItemsAsCompleteSentences, const bool highlightInvalidWords,
    const bool useRtfEncoding)
    {
    text.clear();
    text.append(headerSection).append(legend);

    // punctuation markers
    auto punctPos = theDocument->get_punctuation().begin();
    auto punctEnd = theDocument->get_punctuation().end();
    // temp word
    const lily_of_the_valley::rtf_encode_text rtfEncode;
    std::wstring currentWord;
    for (const auto& currentParagraph : theDocument->get_paragraphs())
        {
        // wrap each paragraph in HTML mode so it can get a hover effect in the CSS
        if (!useRtfEncoding)
            {
            text += L"<p class=\"text-paragraph\">";
            }
        // add a tab at the beginning of the paragraph
        text += tabSymbol;
        // go through the current paragraph's sentences
        for (size_t j = currentParagraph.get_first_sentence_index();
             j <= currentParagraph.get_last_sentence_index(); ++j)
            {
            if (j >= theDocument->get_sentences().size())
                {
                continue;
                } // this should not happen, this is just a sanity trap
            const grammar::sentence_info currentSentence = theDocument->get_sentences().at(j);
            const bool currentSentenceShouldBeHighlightedAsInvalid =
                (highlightIncompleteSentences && considerOnlyListItemsAsCompleteSentences &&
                 !currentSentence.is_valid() &&
                 (currentSentence.get_type() != grammar::sentence_paragraph_type::header)) ||
                (highlightIncompleteSentences && !considerOnlyListItemsAsCompleteSentences &&
                 !currentSentence.is_valid());
            if (currentSentenceShouldBeHighlightedAsInvalid)
                {
                text += ignoreHighlightBegin;
                }
            // go through the current sentence's words
            bool atFirstWordInSentence = true;
            bool sentenceTerminatorAppendedAlready = false;
            currentWord.clear();
            for (size_t i = currentSentence.get_first_word_index();
                 i <= currentSentence.get_last_word_index(); ++i)
                {
                if (i >= theDocument->get_word_count())
                    {
                    continue;
                    } // shouldn't happen, this is a sanity trap
                currentWord = theDocument->get_word(i).c_str();
                if (!atFirstWordInSentence)
                    {
                    // space between this and previous word
                    text += L' ';
                    }
                atFirstWordInSentence = false;
                if (useRtfEncoding && rtfEncode.needs_to_be_encoded(currentWord))
                    {
                    currentWord = rtfEncode(currentWord);
                    }
                else if (!useRtfEncoding &&
                         lily_of_the_valley::html_encode_text::needs_to_be_simple_encoded(
                             currentWord))
                    {
                    currentWord = lily_of_the_valley::html_encode_text::simple_encode(currentWord);
                    }
                // append any punctuation that should be in front of this word
                while (punctPos != punctEnd && punctPos->get_word_position() == i)
                    {
                    std::wstring punct(1, punctPos->get_punctuation_mark());
                    if (useRtfEncoding && rtfEncode.needs_to_be_encoded(punct))
                        {
                        punct = rtfEncode(punct);
                        }
                    else if (!useRtfEncoding &&
                             lily_of_the_valley::html_encode_text::needs_to_be_simple_encoded(
                                 punct))
                        {
                        punct = lily_of_the_valley::html_encode_text::simple_encode(punct);
                        }
                    text += punct;
                    ++punctPos;
                    }
                // highlight if this word is invalid and not part of an incomplete sentence
                if (currentSentence.is_valid() && highlightInvalidWords &&
                    !theDocument->get_word(i).is_valid())
                    {
                    text.append(ignoreHighlightBegin)
                        .append(currentWord)
                        .append(ignoreHighlightEnd);
                    }
                // or highlight if this word meets our criteria for highlighting
                else if (!currentSentenceShouldBeHighlightedAsInvalid &&
                         shouldHighlight(theDocument->get_word(i)))
                    {
                    text.append(shouldHighlight.GetHighlightBegin().wc_str())
                        .append(currentWord)
                        .append(shouldHighlight.GetHighlightEnd().wc_str());
                    }
                else
                    {
                    text += currentWord;
                    }

                // append any punctuation that should be after this word
                while (punctPos != punctEnd && punctPos->get_word_position() == i + 1 &&
                       punctPos->is_connected_to_previous_word())
                    {
                    auto nextPunctPos = (punctPos + 1);
                    // if last word in the sentence AND the last punctuation mark in the document OR
                    // the next punctuation mark is not connected to this word then handle the
                    // sentence termination here.
                    if (i == currentSentence.get_last_word_index() &&
                        (nextPunctPos == punctEnd || nextPunctPos->get_word_position() != i + 1 ||
                         !nextPunctPos->is_connected_to_previous_word()))
                        {
                        std::wstring punct(1, punctPos->get_punctuation_mark());
                        std::wstring endingPunctuation(1, currentSentence.get_ending_punctuation());
                        if (useRtfEncoding)
                            {
                            if (rtfEncode.needs_to_be_encoded(endingPunctuation))
                                {
                                endingPunctuation = rtfEncode(endingPunctuation);
                                }
                            if (rtfEncode.needs_to_be_encoded(punct))
                                {
                                punct = rtfEncode(punct);
                                }
                            }
                        else
                            {
                            endingPunctuation = lily_of_the_valley::html_encode_text::simple_encode(
                                endingPunctuation);
                            punct = lily_of_the_valley::html_encode_text::simple_encode(punct);
                            }
                        // flip the last punctuation and sentence terminator if the
                        // punctuation is a quote (i.e., ". becomes .") or the only
                        // trailing post-sentence marker (e.g., a lone footnote asterisk)
                        const bool isOnlyConnectedPunct =
                            (punctPos == theDocument->get_punctuation().cbegin() ||
                             std::prev(punctPos)->get_word_position() != i + 1 ||
                             !std::prev(punctPos)->is_connected_to_previous_word());
                        if (characters::is_character::is_quote(punctPos->get_punctuation_mark()) ||
                            (grammar::is_end_of_sentence::is_post_sentence_marker(
                                 punctPos->get_punctuation_mark()) &&
                             isOnlyConnectedPunct))
                            {
                            text.append(endingPunctuation).append(punct);
                            }
                        else
                            {
                            text.append(punct).append(endingPunctuation);
                            }
                        sentenceTerminatorAppendedAlready = true;
                        ++punctPos;
                        break;
                        }
                    std::wstring punct(1, punctPos->get_punctuation_mark());
                    if (useRtfEncoding && rtfEncode.needs_to_be_encoded(punct))
                        {
                        punct = rtfEncode(punct);
                        }
                    else if (!useRtfEncoding &&
                             lily_of_the_valley::html_encode_text::needs_to_be_simple_encoded(
                                 punct))
                        {
                        punct = lily_of_the_valley::html_encode_text::simple_encode(punct);
                        }
                    text += punct;
                    ++punctPos;
                    }
                }

            // append sentence terminator if not done already
            if (!sentenceTerminatorAppendedAlready)
                {
                // watch out for abbreviations at end of sentence
                if (!currentWord.empty() && currentWord.back() == L'.')
                    { /*noop*/
                    }
                else
                    {
                    std::wstring endingPunctuation(1, currentSentence.get_ending_punctuation());
                    if (useRtfEncoding && rtfEncode.needs_to_be_encoded(endingPunctuation))
                        {
                        endingPunctuation = rtfEncode(endingPunctuation);
                        }
                    else if (!useRtfEncoding &&
                             lily_of_the_valley::html_encode_text::needs_to_be_simple_encoded(
                                 endingPunctuation))
                        {
                        endingPunctuation =
                            lily_of_the_valley::html_encode_text::simple_encode(endingPunctuation);
                        }
                    text += endingPunctuation;
                    }
                }

            if (currentSentenceShouldBeHighlightedAsInvalid)
                {
                text += ignoreHighlightEnd;
                }

            // add a space at the end of the current sentence
            text += L"  ";
            }
        if (currentParagraph.get_sentence_count() > 0)
            {
            text.erase(text.end() - 2, text.cend());
            }
        if (!useRtfEncoding)
            {
            text += L"</p>";
            }
        // add the paragraph line feed
        for (size_t i = 0; i < currentParagraph.get_leading_end_of_line_count(); ++i)
            {
            text += newLine;
            }
        }

    text += endSection;

    return text.length();
    }

//-----------------------------------------------------------
template<typename documentT, typename tagBuilderT>
size_t FormatWordCollectionHighlightedGrammarIssues(
    const std::shared_ptr<documentT>& theDocument, const size_t longSentenceValue,
    std::wstring& text, const std::wstring& headerSection, const std::wstring& endSection,
    const std::wstring& legend, const std::wstring& highlightBegin,
    const std::wstring& highlightEnd, const std::wstring& misspelledHighlightBegin,
    const std::wstring& duplicateWordHighlightBegin,
    const std::wstring& mismatchedArticleHighlightBegin,
    // raw (tooltip-less) opening tags for the phrase-based issues; a specific tooltip
    // (category + suggested fix, when available) is built for each occurrence via tagBuilder
    const std::wstring& errorHighlightBegin, const std::wstring& phraseHighlightBegin,
    const std::wstring& wordyPhraseLabel, const std::wstring& redundantPhraseLabel,
    const std::wstring& clicheLabel, const std::wstring& phraseErrorLabel,
    const std::wstring& passiveVoiceHighlightBegin, const std::wstring& ignoreHighlightBegin,
    const std::wstring& boldBegin, const std::wstring& boldEnd, const std::wstring& tabSymbol,
    const std::wstring& newLine, const bool highlightIncompleteSentences,
    const bool highlightInvalidWords, const bool useRtfEncoding, const tagBuilderT& tagBuilder)
    {
    text.clear();
    text.append(headerSection).append(legend);

    const auto& dupWordIndices = theDocument->get_duplicate_word_indices();
    const auto& mismatchedArticleIndices = theDocument->get_incorrect_article_indices();
    auto passiveVoicesIter = theDocument->get_passive_voice_indices().cbegin();
    const auto& misspelledWordIndices = theDocument->get_misspelled_words();
    auto wordyIndicesIter = theDocument->get_known_phrase_indices().cbegin();
    const auto& wordyPhrases = theDocument->get_known_phrases().get_phrases();
    // whether we are in phrase highlight mode and the "countdown" value
    // (0 means to close the highlighting)
    std::pair<bool, int> currentPhraseMode{ false, 0 };
    // punctuation markers
    auto punctPos = theDocument->get_punctuation().cbegin();
    auto punctEnd = theDocument->get_punctuation().cend();
    // temp word
    const lily_of_the_valley::rtf_encode_text rtfEncode;
    std::wstring currentWord;
    std::wstring wordCountStr;
    for (const auto& currentParagraph : theDocument->get_paragraphs())
        {
        if (!useRtfEncoding)
            {
            text += L"<p class=\"text-paragraph\">";
            }
        // add a tab at the beginning of the paragraph
        text += tabSymbol;
        // go through the current paragraph's sentences
        for (size_t j = currentParagraph.get_first_sentence_index();
             j <= currentParagraph.get_last_sentence_index(); ++j)
            {
            if (j >= theDocument->get_sentences().size())
                {
                continue;
                } // this should not happen, this is just a sanity trap
            const grammar::sentence_info currentSentence = theDocument->get_sentences().at(j);
            const size_t currentSentenceLength = highlightInvalidWords ?
                                                     currentSentence.get_valid_word_count() :
                                                     currentSentence.get_word_count();
            const bool currentSentenceShouldBeHighlightedAsInvalid =
                (highlightIncompleteSentences && !currentSentence.is_valid());
            bool currentSentenceIsOverlyLong = false;
            if (currentSentenceShouldBeHighlightedAsInvalid)
                {
                text += ignoreHighlightBegin;
                }
            // highlight the sentence if too long
            else if (currentSentenceLength > longSentenceValue)
                {
                currentSentenceIsOverlyLong = true;
                text += highlightBegin;
                }
            // go through the current sentence's words
            bool atFirstWordInSentence = true;
            bool sentenceTerminatorAppendedAlready = false;
            currentWord.clear();
            for (size_t i = currentSentence.get_first_word_index();
                 i <= currentSentence.get_last_word_index(); ++i)
                {
                if (i >= theDocument->get_word_count())
                    {
                    continue;
                    } // shouldn't happen, this is a sanity trap
                currentWord = theDocument->get_word(i).c_str();
                const bool wordIsInvalid =
                    highlightInvalidWords && !theDocument->get_word(i).is_valid();
                if (!atFirstWordInSentence)
                    {
                    // space between this and previous word
                    text += L' ';
                    }
                atFirstWordInSentence = false;
                // append any punctuation that should be in front of this word
                while (punctPos != punctEnd && punctPos->get_word_position() == i)
                    {
                    std::wstring punct(1, punctPos->get_punctuation_mark());
                    if (useRtfEncoding && rtfEncode.needs_to_be_encoded(punct))
                        {
                        punct = rtfEncode(punct);
                        }
                    else if (!useRtfEncoding &&
                             lily_of_the_valley::html_encode_text::needs_to_be_simple_encoded(
                                 punct))
                        {
                        punct = lily_of_the_valley::html_encode_text::simple_encode(punct);
                        }
                    text += punct;
                    ++punctPos;
                    }

                // highlight phrases:
                // if not already in phrase highlight mode (avoids overlapping phrases)
                if (!currentPhraseMode.first)
                    {
                    // wordy items
                    while (wordyIndicesIter != theDocument->get_known_phrase_indices().end() &&
                           wordyIndicesIter->first < i)
                        {
                        ++wordyIndicesIter;
                        }
                    if (wordyIndicesIter != theDocument->get_known_phrase_indices().end() &&
                        wordyIndicesIter->first == i)
                        {
                        const std::wstring suggestion{
                            wordyPhrases[wordyIndicesIter->second].second.c_str()
                        };
                        switch (wordyPhrases[wordyIndicesIter->second].first.get_type())
                            {
                        case grammar::phrase_type::phrase_error:
                            text += tagBuilder(errorHighlightBegin, phraseErrorLabel, suggestion);
                            break;
                        case grammar::phrase_type::phrase_redundant:
                            text +=
                                tagBuilder(phraseHighlightBegin, redundantPhraseLabel, suggestion);
                            break;
                        case grammar::phrase_type::phrase_cliche:
                            text += tagBuilder(phraseHighlightBegin, clicheLabel, suggestion);
                            break;
                        case grammar::phrase_type::phrase_wordy:
                        default:
                            text += tagBuilder(phraseHighlightBegin, wordyPhraseLabel, suggestion);
                            break;
                            }
                        currentPhraseMode.first = true;
                        currentPhraseMode.second = static_cast<int>(
                            wordyPhrases[wordyIndicesIter->second].first.get_word_count());
                        }
                    }
                // passive voice
                if (!currentPhraseMode.first)
                    {
                    while (passiveVoicesIter != theDocument->get_passive_voice_indices().end() &&
                           passiveVoicesIter->first < i)
                        {
                        ++passiveVoicesIter;
                        }
                    if (passiveVoicesIter != theDocument->get_passive_voice_indices().end() &&
                        passiveVoicesIter->first == i)
                        {
                        text += passiveVoiceHighlightBegin;
                        currentPhraseMode.first = true;
                        currentPhraseMode.second = static_cast<int>(passiveVoicesIter->second);
                        }
                    }
                // highlight grammar issues
                bool isErrorWord = false;
                // valid sentence, but word is invalid
                if (currentSentence.is_valid() && wordIsInvalid)
                    {
                    text += ignoreHighlightBegin;
                    }
                else if (std::binary_search(dupWordIndices.cbegin(), dupWordIndices.cend(), i))
                    {
                    text += duplicateWordHighlightBegin;
                    isErrorWord = true;
                    }
                else if (std::binary_search(mismatchedArticleIndices.cbegin(),
                                            mismatchedArticleIndices.cend(), i))
                    {
                    text += mismatchedArticleHighlightBegin;
                    isErrorWord = true;
                    }
                else if (std::binary_search(misspelledWordIndices.cbegin(),
                                            misspelledWordIndices.cend(), i))
                    {
                    text += misspelledHighlightBegin;
                    isErrorWord = true;
                    }
                // copy over the word
                if (useRtfEncoding && rtfEncode.needs_to_be_encoded(currentWord))
                    {
                    currentWord = rtfEncode(currentWord);
                    }
                else if (!useRtfEncoding &&
                         lily_of_the_valley::html_encode_text::needs_to_be_simple_encoded(
                             currentWord))
                    {
                    currentWord = lily_of_the_valley::html_encode_text::simple_encode(currentWord);
                    }
                text += currentWord;
                // unhighlight grammar error/excluded word
                if (isErrorWord || (currentSentence.is_valid() && wordIsInvalid))
                    {
                    text += highlightEnd;
                    }
                if (currentPhraseMode.first)
                    {
                    --currentPhraseMode.second;
                    if (currentPhraseMode.second <= 0)
                        {
                        // out of phrase highlight mode now
                        currentPhraseMode.first = false;
                        text += highlightEnd;
                        }
                    }

                // append any punctuation that should be after this word
                while (punctPos != punctEnd && punctPos->get_word_position() == i + 1 &&
                       punctPos->is_connected_to_previous_word())
                    {
                    auto nextPunctPos = (punctPos + 1);
                    // if last word in the sentence AND the last punctuation mark in the document OR
                    // the next punctuation mark is not connected to this word then handle
                    // the sentence termination here.
                    if (i == currentSentence.get_last_word_index() &&
                        (nextPunctPos == punctEnd || nextPunctPos->get_word_position() != i + 1 ||
                         !nextPunctPos->is_connected_to_previous_word()))
                        {
                        std::wstring punct(1, punctPos->get_punctuation_mark());
                        std::wstring endingPunctuation(1, currentSentence.get_ending_punctuation());
                        if (useRtfEncoding)
                            {
                            if (rtfEncode.needs_to_be_encoded(endingPunctuation))
                                {
                                endingPunctuation = rtfEncode(endingPunctuation);
                                }
                            if (rtfEncode.needs_to_be_encoded(punct))
                                {
                                punct = rtfEncode(punct);
                                }
                            }
                        else
                            {
                            endingPunctuation = lily_of_the_valley::html_encode_text::simple_encode(
                                endingPunctuation);
                            punct = lily_of_the_valley::html_encode_text::simple_encode(punct);
                            }
                        // flip the last punctuation and sentence terminator if the
                        // punctuation is a quote (i.e., ". becomes .") or the only
                        // trailing post-sentence marker (e.g., a lone footnote asterisk)
                        const bool isOnlyConnectedPunct =
                            (punctPos == theDocument->get_punctuation().cbegin() ||
                             std::prev(punctPos)->get_word_position() != i + 1 ||
                             !std::prev(punctPos)->is_connected_to_previous_word());
                        if (characters::is_character::is_quote(punctPos->get_punctuation_mark()) ||
                            (grammar::is_end_of_sentence::is_post_sentence_marker(
                                 punctPos->get_punctuation_mark()) &&
                             isOnlyConnectedPunct))
                            {
                            text.append(endingPunctuation).append(punct);
                            }
                        else
                            {
                            text.append(punct).append(endingPunctuation);
                            }
                        sentenceTerminatorAppendedAlready = true;
                        ++punctPos;
                        break;
                        }
                    std::wstring punct(1, punctPos->get_punctuation_mark());
                    if (useRtfEncoding && rtfEncode.needs_to_be_encoded(punct))
                        {
                        punct = rtfEncode(punct);
                        }
                    else if (!useRtfEncoding &&
                             lily_of_the_valley::html_encode_text::needs_to_be_simple_encoded(
                                 punct))
                        {
                        punct = lily_of_the_valley::html_encode_text::simple_encode(punct);
                        }
                    text += punct;
                    ++punctPos;
                    }
                }

            // append sentence terminator if not done already
            if (!sentenceTerminatorAppendedAlready)
                {
                if (!currentWord.empty() && currentWord.back() == L'.')
                    { /*noop*/
                    }
                else
                    {
                    // watch out for abbreviations at end of sentence
                    std::wstring endingPunctuation(1, currentSentence.get_ending_punctuation());
                    if (useRtfEncoding && rtfEncode.needs_to_be_encoded(endingPunctuation))
                        {
                        endingPunctuation = rtfEncode(endingPunctuation);
                        }
                    else if (!useRtfEncoding &&
                             lily_of_the_valley::html_encode_text::needs_to_be_simple_encoded(
                                 endingPunctuation))
                        {
                        endingPunctuation =
                            lily_of_the_valley::html_encode_text::simple_encode(endingPunctuation);
                        }
                    text += endingPunctuation;
                    }
                }

            wordCountStr.clear();
            wordCountStr.append(L" ")
                .append(boldBegin)
                .append(L"(")
                .append(std::to_wstring(currentSentenceLength))
                .append(L")")
                .append(boldEnd);
            if (currentSentenceShouldBeHighlightedAsInvalid || currentSentenceIsOverlyLong)
                {
                if (currentSentenceIsOverlyLong)
                    {
                    text.append(highlightEnd).append(wordCountStr);
                    }
                else
                    {
                    text += highlightEnd;
                    }
                }
            else
                {
                text += wordCountStr;
                }

            // add a space at the end of the current sentence
            text += L"  ";
            }
        if (currentParagraph.get_sentence_count() > 0)
            {
            text.erase(text.end() - 2, text.cend());
            }
        if (!useRtfEncoding)
            {
            text += L"</p>";
            }
        // add the paragraph line feed
        for (size_t i = 0; i < currentParagraph.get_leading_end_of_line_count(); ++i)
            {
            text += newLine;
            }
        }

    text += endSection;

    return text.length();
    }

//-----------------------------------------------------------
/// @brief The rendered output of a plain language guide.
///     This contains the document text (left pane, with unexplained technical phrases highlighted)
///     and the note cards (right pane, one per unexplained phrase, in the order
///     that they first appear in the document).
struct PlainLanguageGuideOutput
    {
    std::wstring documentHtml;
    std::wstring noteCardsHtml;
    };

/// @brief Converts backtick-delimited code spans (e.g., `` `std::vector` ``) in
///     already HTML-encoded text into `<tt>`/`</tt>` tags.
/// @param text HTML-encoded text (backticks aren't touched by HTML encoding,
///     so this is safe to run afterward).
/// @returns The text with backtick pairs replaced by `<tt>`/`</tt>`.
[[nodiscard]]
inline std::wstring ConvertBackticksToTypeWriter(std::wstring text)
    {
    bool openTag{ false };
    for (size_t pos = text.find(L'`'); pos != std::wstring::npos; pos = text.find(L'`', pos))
        {
        const std::wstring_view tag{ openTag ? L"</tt>" : L"<tt>" };
        text.replace(pos, 1, tag);
        pos += tag.length();
        openTag = !openTag;
        }
    return text;
    }

/// @brief Builds the Plain Language Guide report. Highlights every occurrence of an
///     "unexplained" technical phrase (one whose plain-language replacement was never
///     found nearby, see document::analyze_plain_language_guide()). Adds a note card
///     for each such phrase (phrase and detailed explanation), in first-occurrence order.
/// @details Phrases whose replacement WAS found nearby at least once are never
///     highlighted and get no note (get_plain_language_phrase_explained()).
template<typename documentT>
PlainLanguageGuideOutput
FormatWordCollectionPlainLanguageGuide(const std::shared_ptr<documentT>& theDocument,
                                       const std::wstring& highlightClass,
                                       const std::wstring& tabSymbol, const std::wstring& newLine)
    {
    PlainLanguageGuideOutput output;

    const auto& occurrences = theDocument->get_plain_language_phrase_indices();
    const auto& explainedFlags = theDocument->get_plain_language_phrase_explained();
    const auto& phrases = theDocument->get_plain_language_phrases().get_phrases();

    // assign each unexplained phrase a stable note ID (in first-occurrence order), and
    // map every occurrence (word index) of it to that ID for the highlighting pass below

    // phrase-collection index -> note ID
    std::map<size_t, size_t> phraseIndexToNoteId;
    // word index -> note ID
    std::unordered_map<size_t, size_t> occurrenceToNoteId;
    for (const auto& occurrence : occurrences)
        {
        const size_t phraseIdx = occurrence.second;
        if (phraseIdx >= explainedFlags.size() || explainedFlags[phraseIdx])
            {
            continue; // explained -- never highlighted, no note
            }
        const auto [iter, inserted] =
            phraseIndexToNoteId.try_emplace(phraseIdx, phraseIndexToNoteId.size() + 1);
        occurrenceToNoteId[occurrence.first] = iter->second;
        }

    // note cards, in first-occurrence (i.e., note ID) order
    std::vector<size_t> phraseIdxByNoteId(phraseIndexToNoteId.size());
    for (const auto& indexAndNoteId : phraseIndexToNoteId)
        {
        phraseIdxByNoteId[indexAndNoteId.second - 1] = indexAndNoteId.first;
        }
    for (size_t noteId = 1; noteId <= phraseIdxByNoteId.size(); ++noteId)
        {
        const auto& entry = phrases[phraseIdxByNoteId[noteId - 1]];
        std::wstring phraseText{ entry.first.to_string().c_str() };
        std::wstring explanation{ entry.second.explanation.c_str() };
        if (lily_of_the_valley::html_encode_text::needs_to_be_simple_encoded(phraseText))
            {
            phraseText = lily_of_the_valley::html_encode_text::simple_encode(phraseText);
            }
        if (lily_of_the_valley::html_encode_text::needs_to_be_simple_encoded(explanation))
            {
            explanation = lily_of_the_valley::html_encode_text::simple_encode(explanation);
            }
        phraseText = ConvertBackticksToTypeWriter(std::move(phraseText));
        explanation = ConvertBackticksToTypeWriter(std::move(explanation));
        output.noteCardsHtml.append(L"<div class=\"pl-guide-note-card\" id=\"pl-note-")
            .append(std::to_wstring(noteId))
            .append(L"\">\n<p class=\"pl-guide-note-phrase\">")
            .append(phraseText)
            .append(L"</p>\n<p>")
            .append(explanation)
            .append(L"</p>\n</div>\n");
        }

    // the document text, with unexplained phrases highlighted
    auto punctPos = theDocument->get_punctuation().cbegin();
    auto punctEnd = theDocument->get_punctuation().cend();
    // (phrase highlight "countdown": how many more words are left in the phrase
    //  currently being highlighted; 0 means not currently highlighting)
    size_t phraseHighlightWordsRemaining{ 0 };
    std::wstring currentWord;
    for (const auto& currentParagraph : theDocument->get_paragraphs())
        {
        output.documentHtml += L"<p class=\"pl-guide-paragraph\">";
        output.documentHtml += tabSymbol;
        for (size_t j = currentParagraph.get_first_sentence_index();
             j <= currentParagraph.get_last_sentence_index(); ++j)
            {
            if (j >= theDocument->get_sentences().size())
                {
                // sanity trap, shouldn't happen
                continue;
                }
            const grammar::sentence_info currentSentence = theDocument->get_sentences().at(j);
            bool atFirstWordInSentence = true;
            bool sentenceTerminatorAppendedAlready = false;
            currentWord.clear();
            for (size_t i = currentSentence.get_first_word_index();
                 i <= currentSentence.get_last_word_index(); ++i)
                {
                if (i >= theDocument->get_word_count())
                    {
                    continue;
                    }
                currentWord = theDocument->get_word(i).c_str();
                if (!atFirstWordInSentence)
                    {
                    output.documentHtml += L' ';
                    }
                atFirstWordInSentence = false;
                // punctuation in front of this word
                while (punctPos != punctEnd && punctPos->get_word_position() == i)
                    {
                    std::wstring punct(1, punctPos->get_punctuation_mark());
                    if (lily_of_the_valley::html_encode_text::needs_to_be_simple_encoded(punct))
                        {
                        punct = lily_of_the_valley::html_encode_text::simple_encode(punct);
                        }
                    output.documentHtml += punct;
                    ++punctPos;
                    }

                // start highlighting an unexplained phrase's occurrence here?
                if (phraseHighlightWordsRemaining == 0)
                    {
                    const auto noteIdPos = occurrenceToNoteId.find(i);
                    if (noteIdPos != occurrenceToNoteId.cend())
                        {
                        const size_t phraseIdx = phraseIdxByNoteId[noteIdPos->second - 1];
                        output.documentHtml.append(L"<a class=\"")
                            .append(highlightClass)
                            .append(L"\" href=\"#pl-note-")
                            .append(std::to_wstring(noteIdPos->second))
                            .append(L"\" data-plain-language-id=\"")
                            .append(std::to_wstring(noteIdPos->second))
                            .append(L"\">");
                        phraseHighlightWordsRemaining = phrases[phraseIdx].first.get_word_count();
                        }
                    }

                if (lily_of_the_valley::html_encode_text::needs_to_be_simple_encoded(currentWord))
                    {
                    currentWord = lily_of_the_valley::html_encode_text::simple_encode(currentWord);
                    }
                output.documentHtml += currentWord;

                if (phraseHighlightWordsRemaining > 0)
                    {
                    --phraseHighlightWordsRemaining;
                    if (phraseHighlightWordsRemaining == 0)
                        {
                        output.documentHtml += L"</a>";
                        }
                    }

                // punctuation connected after this word
                while (punctPos != punctEnd && punctPos->get_word_position() == i + 1 &&
                       punctPos->is_connected_to_previous_word())
                    {
                    auto nextPunctPos = (punctPos + 1);
                    // only pair with the terminator if this is truly the last connected
                    // mark, or breaking out here drops the rest (e.g. half a surrogate pair)
                    if (i == currentSentence.get_last_word_index() &&
                        (nextPunctPos == punctEnd || nextPunctPos->get_word_position() != i + 1 ||
                         !nextPunctPos->is_connected_to_previous_word()))
                        {
                        std::wstring endingPunctuation(1, currentSentence.get_ending_punctuation());
                        std::wstring punct(1, punctPos->get_punctuation_mark());
                        endingPunctuation =
                            lily_of_the_valley::html_encode_text::simple_encode(endingPunctuation);
                        punct = lily_of_the_valley::html_encode_text::simple_encode(punct);
                        if (characters::is_character::is_quote(punctPos->get_punctuation_mark()))
                            {
                            output.documentHtml.append(endingPunctuation).append(punct);
                            }
                        else
                            {
                            output.documentHtml.append(punct).append(endingPunctuation);
                            }
                        sentenceTerminatorAppendedAlready = true;
                        ++punctPos;
                        break;
                        }
                    std::wstring punct(1, punctPos->get_punctuation_mark());
                    if (lily_of_the_valley::html_encode_text::needs_to_be_simple_encoded(punct))
                        {
                        punct = lily_of_the_valley::html_encode_text::simple_encode(punct);
                        }
                    output.documentHtml += punct;
                    ++punctPos;
                    }
                }

            if (!sentenceTerminatorAppendedAlready)
                {
                if (!currentWord.empty() && currentWord.back() == L'.')
                    { /*noop*/
                    }
                else
                    {
                    std::wstring endingPunctuation(1, currentSentence.get_ending_punctuation());
                    if (lily_of_the_valley::html_encode_text::needs_to_be_simple_encoded(
                            endingPunctuation))
                        {
                        endingPunctuation =
                            lily_of_the_valley::html_encode_text::simple_encode(endingPunctuation);
                        }
                    output.documentHtml += endingPunctuation;
                    }
                }
            output.documentHtml += L"  ";
            }
        if (currentParagraph.get_sentence_count() > 0)
            {
            output.documentHtml.erase(output.documentHtml.end() - 2, output.documentHtml.cend());
            }
        output.documentHtml += L"</p>";
        for (size_t i = 0; i < currentParagraph.get_leading_end_of_line_count(); ++i)
            {
            output.documentHtml += newLine;
            }
        }

    return output;
    }

//------------------------------------------------
template<typename documentT>
size_t FormatFilteredWordCollection(const std::shared_ptr<documentT>& theDocument,
                                    std::wstring& text,
                                    const InvalidTextFilterFormat validTextFormatting,
                                    const bool removeFilePaths, const bool stripAbbreviations)
    {
    text.clear();
    // punctuation markers
    auto punctPos = theDocument->get_punctuation().cbegin();
    auto punctEnd = theDocument->get_punctuation().cend();
    // temp word
    std::wstring currentWord;
    for (const auto& currentParagraph : theDocument->get_paragraphs())
        {
        // add a tab at the beginning of the paragraph
        text += L'\t';
        // go through the current paragraph's sentences
        size_t formattedSentencesInCurrentParagraph = 0;
        for (size_t j = currentParagraph.get_first_sentence_index();
             j <= currentParagraph.get_last_sentence_index(); ++j)
            {
            // this should not happen, this is just a sanity trap
            if (j >= theDocument->get_sentences().size())
                {
                continue;
                }
            const grammar::sentence_info currentSentence = theDocument->get_sentences().at(j);
            if (validTextFormatting == InvalidTextFilterFormat::IncludeOnlyValidText)
                {
                if (!currentSentence.is_valid())
                    {
                    continue;
                    }
                }
            else if (validTextFormatting == InvalidTextFilterFormat::IncludeOnlyInvalidText)
                {
                if (currentSentence.is_valid())
                    {
                    continue;
                    }
                }
            ++formattedSentencesInCurrentParagraph;
            // go through the current sentence's words
            bool atFirstWordInSentence = true;
            bool sentenceTerminatorAppendedAlready = false;
            currentWord.clear();
            for (size_t i = currentSentence.get_first_word_index();
                 i <= currentSentence.get_last_word_index(); ++i)
                {
                // shouldn't happen, this is a sanity trap
                if (i >= theDocument->get_word_count())
                    {
                    continue;
                    }
                if (removeFilePaths && theDocument->get_word(i).is_file_address())
                    {
                    continue;
                    }
                if (validTextFormatting == InvalidTextFilterFormat::IncludeOnlyValidText &&
                    !theDocument->get_word(i).is_valid())
                    {
                    continue;
                    }
                currentWord = theDocument->get_word(i).c_str();
                if (stripAbbreviations && !theDocument->get_word(i).is_numeric() &&
                    // last word in the sentence will need to keep its period (if an abbreviation)
                    i < currentSentence.get_last_word_index() && currentWord.length() &&
                    currentWord.back() == L'.')
                    {
                    currentWord.pop_back();
                    }
                if (!atFirstWordInSentence)
                    {
                    // space between this and previous word
                    text += L' ';
                    }
                atFirstWordInSentence = false;
                // append any punctuation that should be in front of this word
                while (punctPos != punctEnd && punctPos->get_word_position() < i)
                    // this is needed because a skipped sentence could prevent us from
                    // comparing against every word position
                    {
                    ++punctPos;
                    }
                while (punctPos != punctEnd && punctPos->get_word_position() == i)
                    {
                    std::wstring punct(1, punctPos->get_punctuation_mark());
                    text += punct;
                    ++punctPos;
                    }

                // copy over the word
                text += currentWord;

                // append any punctuation that should be after this word
                while (punctPos != punctEnd && punctPos->get_word_position() == i + 1 &&
                       punctPos->is_connected_to_previous_word())
                    {
                    auto nextPunctPos = (punctPos + 1);
                    // if last word in the sentence AND the last punctuation mark in the document OR
                    // the next punctuation mark is not connected to this word then handle
                    // the sentence termination here.
                    if (i == currentSentence.get_last_word_index() &&
                        (nextPunctPos == punctEnd || nextPunctPos->get_word_position() != i + 1 ||
                         !nextPunctPos->is_connected_to_previous_word()))
                        {
                        const std::wstring punct(1, punctPos->get_punctuation_mark());
                        const std::wstring endingPunctuation(
                            1, currentSentence.get_ending_punctuation());
                        // flip the last punctuation and sentence terminator if the
                        // punctuation is a quote (i.e., ". becomes .") or the only
                        // trailing post-sentence marker (e.g., a lone footnote asterisk)
                        const bool isOnlyConnectedPunct =
                            (punctPos == theDocument->get_punctuation().cbegin() ||
                             std::prev(punctPos)->get_word_position() != i + 1 ||
                             !std::prev(punctPos)->is_connected_to_previous_word());
                        if (characters::is_character::is_quote(punctPos->get_punctuation_mark()) ||
                            (grammar::is_end_of_sentence::is_post_sentence_marker(
                                 punctPos->get_punctuation_mark()) &&
                             isOnlyConnectedPunct))
                            {
                            text.append(endingPunctuation).append(punct);
                            }
                        else
                            {
                            text.append(punct).append(endingPunctuation);
                            }
                        sentenceTerminatorAppendedAlready = true;
                        ++punctPos;
                        break;
                        }
                    const std::wstring punct(1, punctPos->get_punctuation_mark());
                    text += punct;
                    ++punctPos;
                    }
                }

            // append sentence terminator if not done already
            if (!sentenceTerminatorAppendedAlready)
                {
                if (!currentWord.empty() && currentWord.back() == L'.')
                    { /*noop*/
                    }
                else
                    {
                    // watch out for abbreviations at end of sentence
                    const std::wstring endingPunctuation(1,
                                                         currentSentence.get_ending_punctuation());
                    text += endingPunctuation;
                    }
                }

            // add a space at the end of the current sentence
            text += L"  ";
            }
        if (formattedSentencesInCurrentParagraph > 0 && text.length() > 2)
            {
            text.erase(text.end() - 2, text.cend());
            }
        // add the paragraph line feed(s)
        for (size_t i = 0; i < currentParagraph.get_leading_end_of_line_count(); ++i)
            {
            text += L'\n';
            }
        }

    return text.length();
    }

#endif // WORD_COLLECTION_TEXT_FORMATTING_H
