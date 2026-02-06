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

template<typename documentT, typename highlightDeterminantT>
static size_t FormatWordCollectionHighlightedWords(
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
template<typename documentT>
static size_t FormatWordCollectionHighlightedGrammarIssues(
    const std::shared_ptr<documentT>& theDocument, const size_t longSentenceValue,
    std::wstring& text, const std::wstring& headerSection, const std::wstring& endSection,
    const std::wstring& legend, const std::wstring& highlightBegin,
    const std::wstring& highlightEnd, const std::wstring& errorHighlightBegin,
    const std::wstring& phraseHighlightBegin, const std::wstring& ignoreHighlightBegin,
    const std::wstring& boldBegin, const std::wstring& boldEnd, const std::wstring& tabSymbol,
    const std::wstring& newLine, const bool highlightIncompleteSentences,
    const bool highlightInvalidWords, const bool useRtfEncoding)
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
                        if (wordyPhrases[wordyIndicesIter->second].first.get_type() ==
                            grammar::phrase_type::phrase_error)
                            {
                            text += errorHighlightBegin;
                            }
                        else
                            {
                            text += phraseHighlightBegin;
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
                        text += phraseHighlightBegin;
                        currentPhraseMode.first = true;
                        currentPhraseMode.second = static_cast<int>(passiveVoicesIter->second);
                        }
                    }
                // highlight grammar issues
                const bool isErrorWord =
                    (std::binary_search(dupWordIndices.cbegin(), dupWordIndices.cend(), i)) ||
                    (std::binary_search(mismatchedArticleIndices.cbegin(),
                                        mismatchedArticleIndices.cend(), i)) ||
                    (std::binary_search(misspelledWordIndices.cbegin(),
                                        misspelledWordIndices.cend(), i));
                // valid sentence, but word is invalid
                if (currentSentence.is_valid() && wordIsInvalid)
                    {
                    text += ignoreHighlightBegin;
                    }
                else if (isErrorWord)
                    {
                    text += errorHighlightBegin;
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
                .append(L" (")
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
        // add the paragraph line feed
        for (size_t i = 0; i < currentParagraph.get_leading_end_of_line_count(); ++i)
            {
            text += newLine;
            }
        }

    text += endSection;

    return text.length();
    }

//------------------------------------------------
template<typename documentT>
static size_t
FormatFilteredWordCollection(const std::shared_ptr<documentT>& theDocument, std::wstring& text,
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
