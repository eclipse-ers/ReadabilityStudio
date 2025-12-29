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

#ifndef WORD_COLLECTION_H
#define WORD_COLLECTION_H

#include "../OleanderStemmingLibrary/src/common_lang_constants.h"
#include "../Wisteria-Dataviz/src/debug/debug_profile.h"
#include "../Wisteria-Dataviz/src/math/mathematics.h"
#include "../Wisteria-Dataviz/src/util/frequencymap.h"
#include "../Wisteria-Dataviz/src/util/i18n_string_util.h"
#include "abbreviation.h"
#include "article.h"
#include "character_traits.h"
#include "conjunction.h"
#include "contraction.h"
#include "double_words.h"
#include "negating_word.h"
#include "paragraph.h"
#include "passive_voice.h"
#include "phrase.h"
#include "pronoun.h"
#include "sentence.h"
#include "stop_lists.h"
#include "syllable.h"
#include "tokenize.h"
#include "word_functional.h"
#include <cstdlib>
#include <limits>
#include <map>
#include <numeric>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

/** @brief Class representing a document.
    @details This includes a full breakdown of its sentences,
     paragraphs, punctuation, grammar errors, etc.*/
template<typename Tword_type>
class document
    {
  public:
    document(const std::basic_string<wchar_t>& name, grammar::base_syllabize* syllabizer,
             stemming::stem<>* stemmer, const grammar::is_coordinating_conjunction* isConjunction,
             const grammar::phrase_collection* known_phrases,
             const grammar::phrase_collection* copyright_phrases,
             const grammar::phrase_collection* citation_phrases,
             const word_list* known_proper_nouns, const word_list* known_personal_nouns,
             const word_list* known_spellings, const word_list* secondary_known_spellings,
             const word_list* programming_known_spellings, const word_list* stop_list)
        : m_name(name), syllabize(syllabizer), stem_word(stemmer), is_conjunction(isConjunction),
          is_known_phrase(known_phrases), is_known_proper_nouns(known_proper_nouns),
          is_known_personal_nouns(known_personal_nouns), is_copyright_phrase(copyright_phrases),
          is_citation_phrase(citation_phrases), m_stop_list(stop_list),
          is_correctly_spelled(known_spellings, secondary_known_spellings,
                               programming_known_spellings, false, true, true, true, false, true,
                               true)
        {
        }

#ifdef __UNITTEST
    void load_document(const wchar_t* words, const size_t length, const bool treatEolAsEos,
                       const bool ignoreBlankLinesWhenDeterminingParagraphSplit,
                       const bool ignoreIndentingWhenDeterminingParagraphSplit,
                       const bool sentenceStartMustBeUppercased)
        {
        treat_eol_as_eos(treatEolAsEos);
        ignore_blank_lines_when_determining_paragraph_split(
            ignoreBlankLinesWhenDeterminingParagraphSplit);
        ignore_indenting_when_determining_paragraph_split(
            ignoreIndentingWhenDeterminingParagraphSplit);
        sentence_start_must_be_uppercased(sentenceStartMustBeUppercased);
        load(words, length);
        }
#endif

    void load(const wchar_t* words, const size_t length)
        {
        PROFILE();
        reset();
        // on average, every 5 or 6 characters in a text stream is a single word
        reserve_word_size(length / 5);

        tokenize::document_tokenize<> tokenizeText(
            words, length, m_treat_eol_as_eos,
            m_ignore_blank_lines_when_determining_paragraph_split,
            m_ignore_indenting_when_determining_paragraph_split,
            m_sentence_start_must_be_uppercased);

        tokenizeText.set_known_spellings(is_correctly_spelled.get_word_list());

        grammar::base_syllabize& countSyllables = *syllabize;
        const punctuation::punctuation_count countPunctuation;

        const wchar_t* currentChar = nullptr;

        while ((currentChar = tokenizeText()) != nullptr)
            {
            if (!tokenizeText.is_split_word())
                {
                m_words.emplace_back(
                    currentChar, tokenizeText.get_current_word_length(),
                    tokenizeText.get_current_sentence_index(), tokenizeText.get_sentence_position(),
                    tokenizeText.get_current_paragraph_index(), tokenizeText.is_numeric(),
                    // these are really calculated after the entire document is loaded,
                    // so just set reasonable default values for now
                    true, false, false,
                    countSyllables(currentChar, tokenizeText.get_current_word_length()),
                    countPunctuation({ currentChar, tokenizeText.get_current_word_length() }));
                }
            // Review words at the end of the line that are hyphenated,
            // because they are usually connected to the word on the next line.
            else
                {
                PROFILE_SECTION_START("document::load(): split word");
                // Review versions of the word with and without hyphens to see which one we should
                // use. Also strip out newlines.
                Tword_type currentWord{};
                currentWord.reserve(tokenizeText.get_current_word_length());

                Tword_type currentWordNoHyphens{};
                currentWordNoHyphens.reserve(tokenizeText.get_current_word_length());
                for (size_t i = 0; i < tokenizeText.get_current_word_length(); ++i)
                    {
                    if (!characters::is_character::is_space(currentChar[i]))
                        {
                        currentWord.push_back(currentChar[i]);
                        }
                    if (!characters::is_character::is_space(currentChar[i]) &&
                        !characters::is_character::is_hyphen(currentChar[i]))
                        {
                        currentWordNoHyphens.push_back(currentChar[i]);
                        }
                    }
                if (is_correctly_spelled(currentWordNoHyphens))
                    {
                    m_words.emplace_back(
                        currentWordNoHyphens.c_str(), currentWordNoHyphens.length(),
                        tokenizeText.get_current_sentence_index(),
                        tokenizeText.get_sentence_position(),
                        tokenizeText.get_current_paragraph_index(), tokenizeText.is_numeric(),
                        // these are really calculated after the entire document is loaded,
                        // so just set reasonable default values for now
                        true, false, false,
                        countSyllables(currentChar, tokenizeText.get_current_word_length()),
                        countPunctuation({ currentChar, tokenizeText.get_current_word_length() }));
                    }
                else
                    {
                    m_words.emplace_back(
                        currentWord.c_str(), currentWord.length(),
                        tokenizeText.get_current_sentence_index(),
                        tokenizeText.get_sentence_position(),
                        tokenizeText.get_current_paragraph_index(), tokenizeText.is_numeric(),
                        // these are really calculated after the entire document is loaded,
                        // so just set reasonable default values for now
                        true, false, false,
                        countSyllables(currentChar, tokenizeText.get_current_word_length()),
                        countPunctuation({ currentChar, tokenizeText.get_current_word_length() }));
                    }
                PROFILE_SECTION_END();
                }
            update_sentence_paragraph_info(tokenizeText.get_current_sentence_index(),
                                           tokenizeText.get_current_paragraph_index(),
                                           tokenizeText.get_current_sentence_ending_punctuation(),
                                           tokenizeText.get_current_leading_end_of_line_count());
            }

        m_punctuation = std::move(tokenizeText.m_punctuation);

        finalize(tokenizeText.get_current_sentence_ending_punctuation());
        }

    /** Allocates space for the information structures.
        Their sizes are approximated based on the size of the document.*/
    void reserve_word_size(const size_t size)
        {
        m_words.reserve(size);
        // assume a piece of punctuation every 5 words (on average)
        m_punctuation.reserve(safe_divide<size_t>(size, 5));
        // assume 20 words per sentence
        m_sentences.reserve(safe_divide<size_t>(size, 20));
        // assume 4 sentences per paragraph
        m_paragraphs.reserve(safe_divide<size_t>(size, 80));
        }

    void update_sentence_paragraph_info(const size_t current_sentence_index,
                                        const size_t current_paragraph_index,
                                        const wchar_t sentence_ending_punctuation,
                                        const size_t leading_end_of_line_count)
        {
        if (m_sentences.size() != current_sentence_index)
            {
            /*a new sentence has started, so load information representing
            the previous sentence. Note that we need to step back an extra word here
            because we just moved the "words" collection to the first word of the next
            sentence.*/
            m_sentences.push_back(grammar::sentence_info(
                m_current_sentence_begin, (m_words.size() > 1) ? (m_words.size() - 2) : (0),
                sentence_ending_punctuation));
            m_current_sentence_begin = m_words.size() - 1;
            /*now see if a new paragraph has started. It only makes sense for a
            new paragraph to start if we just came to the end of a sentence*/
            if (m_paragraphs.size() != current_paragraph_index)
                {
                /*a new paragraph has started, so load information representing
                the previous paragraph*/
                m_paragraphs.emplace_back(
                    m_current_paragraph_begin, !m_sentences.empty() ? (m_sentences.size() - 1) : 0,
                    leading_end_of_line_count,
                    !m_sentences.empty() ?
                        m_sentences[m_sentences.size() - 1].ends_with_valid_punctuation() :
                        false);
                m_current_paragraph_begin = m_sentences.size();
                }
            }
        }

    /** Call this after loading all the words into the document.
        This will catalog all the grammar errors and proper nouns,
        update sentence information, etc.*/
    void finalize(const wchar_t sentence_ending_punctuation)
        {
        PROFILE();
        if (m_words.empty())
            {
            return;
            }
        const grammar::is_coordinating_conjunction& isConjunction = *is_conjunction;
        m_valid_paragraph_count = m_complete_sentence_count = m_valid_word_count = 0;
        // count words at the end of the file as a final sentence and paragraph
        if (m_current_sentence_begin < m_words.size())
            {
            m_sentences.push_back(grammar::sentence_info(
                m_current_sentence_begin, !m_words.empty() ? (m_words.size() - 1) : (0),
                sentence_ending_punctuation));

            m_paragraphs.emplace_back(
                m_current_paragraph_begin, !m_sentences.empty() ? (m_sentences.size() - 1) : (0), 0,
                !m_sentences.empty() ?
                    m_sentences[m_sentences.size() - 1].ends_with_valid_punctuation() :
                    false);
            }
        // finish tagging the words
        //-------------------------
        // file address
        for_each(m_words.begin(), m_words.end(),
                 [](auto& word)
                 {
                     word.set_file_address(
                         i18n_string_util::is_file_address({ word.c_str(), word.length() }));
                 });
        // social media
        is_social_media_tag<Tword_type> isSmTag;
        for_each(m_words.begin(), m_words.end(), [&isSmTag = std::as_const(isSmTag)](auto& word)
                 { word.set_social_media_tag(isSmTag(word)); });
        // abbreviation
        grammar::is_abbreviation isAbbreviation;
        for_each(m_words.begin(), m_words.end(),
                 [&isAbbreviation = std::as_const(isAbbreviation)](auto& word)
                 { word.set_abbreviation_tag(isAbbreviation({ word.c_str(), word.length() })); });
            // contractions
            {
            auto punctPos =
                (!m_punctuation.empty()) ? m_punctuation.cbegin() : m_punctuation.cend();
            const grammar::is_contraction isContraction;
            for (size_t i = 0;
                 // no normal writing would have a contraction as the last word
                 i < m_words.size() - 1; ++i)
                {
                // if there is punctuation between a possible contraction and the next word, then
                // don't allow the following word to be used in the contraction analysis.
                while (punctPos != m_punctuation.end() && (punctPos->get_word_position() < i + 1))
                    {
                    ++punctPos;
                    }
                if (punctPos != m_punctuation.end() && punctPos->get_word_position() == i + 1 &&
                    punctPos->is_connected_to_previous_word())
                    {
                    m_words[i].set_contraction(
                        isContraction({ m_words[i].c_str(), m_words[i].length() }));
                    }
                else
                    {
                    // ...otherwise, we should include the following word in the analysis
                    m_words[i].set_contraction(
                        isContraction({ m_words[i].c_str(), m_words[i].length() },
                                      { m_words[i + 1].c_str(), m_words[i + 1].length() }));
                    }
                }
            }
        // allocate sizes for the collections
        m_known_phrase_indices.reserve(safe_divide<size_t>(m_words.size(), 3));
        m_n_grams_indices.reserve(safe_divide<size_t>(m_words.size(), 3));
        m_proper_phrase_indices.reserve(safe_divide<size_t>(m_words.size(), 3));
        m_negating_phrase_indices.reserve(safe_divide<size_t>(m_words.size(), 3));
        m_duplicate_word_indices.reserve(safe_divide<size_t>(m_words.size(), 2));
        m_incorrect_articles.reserve(safe_divide<size_t>(m_words.size(), 2));
        m_passive_voices.reserve(safe_divide<size_t>(m_words.size(), 2));
        m_misspelled_words.reserve(m_words.size());
        m_conjunction_beginning_sentences.reserve(m_sentences.size());
        m_lowercase_beginning_sentences.reserve(m_sentences.size());
        m_overused_words_by_sentence.reserve(safe_divide<size_t>(m_words.size(), 10));

        // go through the sentences and mark the incomplete ones as either a header or a list item
        auto paragraphPos = (!m_paragraphs.empty()) ? m_paragraphs.cbegin() : m_paragraphs.cend();
        for (auto sentIter = m_sentences.begin(); sentIter != m_sentences.end();
             /*handled in loop*/)
            {
            /* If an incomplete sentence then look ahead and see how many more follow.
               If there are just one or two consecutive incomplete sentences then they are
               more than likely a header (and sub-header). Three or more incomplete sentences
               is more than likely a list or table of some sort.*/
            if (!sentIter->is_valid())
                {
                // look forward and see how many consecutive incomplete sentences there
                // are from here
                size_t consecutiveIncompleteSentences = 0;
                auto nextSentIter = sentIter;
                while (nextSentIter != m_sentences.end() && !nextSentIter->is_valid())
                    {
                    ++consecutiveIncompleteSentences;
                    ++nextSentIter;
                    }
                // see if any of these incomplete sentences need to be either a header or list item
                size_t adjustedConsecutiveIncompleteSentences = consecutiveIncompleteSentences;
                for (size_t i = 0; i < consecutiveIncompleteSentences; ++i)
                    {
                    // if in a paragraph that has more than one sentence, then it can't be a header,
                    // mark it as incomplete.
                    while (paragraphPos != m_paragraphs.end() &&
                           !paragraphPos->contains_sentence(sentIter - m_sentences.begin()))
                        {
                        ++paragraphPos;
                        }
                    if (paragraphPos != m_paragraphs.end() &&
                        paragraphPos->get_sentence_count() > 1)
                        {
                        sentIter->set_type(grammar::sentence_paragraph_type::incomplete);
                        --adjustedConsecutiveIncompleteSentences;
                        }
                    // a list item if more than 2 incomplete sentences in a row
                    // (which would have to be on separate lines)
                    else if (adjustedConsecutiveIncompleteSentences > 2)
                        {
                        sentIter->set_type(grammar::sentence_paragraph_type::list_item);
                        }
                    // otherwise, it is a header
                    else
                        {
                        if (m_treat_header_words_as_valid)
                            {
                            sentIter->set_valid(true);
                            // individual words will be updated to valid later
                            }
                        sentIter->set_type(grammar::sentence_paragraph_type::header);
                        }
                    ++sentIter;
                    }
                }
            else
                {
                ++sentIter;
                }
            }

        // Review the paragraphs for any that are just a single abbreviation,
        // which would appear like a sentence but won't really be that. For example, "vs."
        // being a single line should not be a real paragraph, consider it to be a header.
        for (auto& theParagraph : m_paragraphs)
            {
            if (theParagraph.get_sentence_count() == 1 &&
                m_sentences[theParagraph.get_first_sentence_index()].is_valid() &&
                m_sentences[theParagraph.get_first_sentence_index()].get_word_count() == 1)
                {
                const Tword_type& theWord =
                    m_words[m_sentences[theParagraph.get_first_sentence_index()]
                                .get_first_word_index()];
                if (isAbbreviation({ theWord.c_str(), theWord.length() }))
                    {
                    // individual words will be updated to valid later
                    m_sentences[theParagraph.get_first_sentence_index()].set_valid(
                        m_treat_header_words_as_valid);
                    m_sentences[theParagraph.get_first_sentence_index()].set_type(
                        grammar::sentence_paragraph_type::header);
                    }
                }
            }

        // Go through the sentences and mark as valid if ending with a semicolon. This is a special
        // case where we will treat list items ending with a semicolon as a regular sentence (same
        // as we would with a list item ending with a period).
        if (!is_exclusion_aggressive())
            {
            auto punctPosNonConst =
                (!m_punctuation.empty()) ? m_punctuation.begin() : m_punctuation.end();
            for (auto& theSentence : m_sentences)
                {
                if (!theSentence.is_valid())
                    {
                    while (punctPosNonConst != m_punctuation.end() &&
                           (punctPosNonConst->get_word_position() <=
                            theSentence.get_last_word_index()))
                        {
                        ++punctPosNonConst;
                        }
                    if (punctPosNonConst == m_punctuation.end())
                        {
                        break;
                        }
                    // we will be on the first word of the NEXT sentence. Review the punctuation
                    // connected to the last word of our current sentence...
                    while (punctPosNonConst != m_punctuation.end() &&
                           punctPosNonConst->is_connected_to_previous_word() &&
                           punctPosNonConst->get_word_position() ==
                               theSentence.get_last_word_index() + 1)
                        {
                        // Semicolon at the end of an invalid sentence will now be valid
                        // (it will be a list item that should be like a regular sentence).
                        // It can still be seen as a list item or header, just mark it as valid.
                        // The semicolon will also be moved from the punctuation data onto the
                        // sentence.
                        if (traits::case_insensitive_ex::eq(
                                punctPosNonConst->get_punctuation_mark(),
                                common_lang_constants::SEMICOLON))
                            {
                            theSentence.set_valid(true);
                            theSentence.set_ending_punctuation(common_lang_constants::SEMICOLON);
                            punctPosNonConst = m_punctuation.erase(punctPosNonConst);
                            break;
                            }

                        ++punctPosNonConst;
                        }
                    }
                }
            }

        // Go through the sentences and mark them as valid if they are invalid but contain
        // a certain number of words. This fixes any valid sentences that simply were missing a
        // terminating period.
        // Note that we don't do this with list items if in aggressive exclusion mode; that will
        // override the allowable incomplete sentence threshold logic.
        for (auto& theParagraph : m_paragraphs)
            {
            // go through the sentences
            for (size_t sentenceCounter = theParagraph.get_first_sentence_index();
                 sentenceCounter <= theParagraph.get_last_sentence_index(); ++sentenceCounter)
                {
                if (!m_sentences[sentenceCounter].is_valid() &&
                    (!is_exclusion_aggressive() ||
                     m_sentences[sentenceCounter].get_type() !=
                         grammar::sentence_paragraph_type::list_item) &&
                    m_sentences[sentenceCounter].get_word_count() >
                        get_allowable_incomplete_sentence_size())
                    {
                    // See how many "real words" (i.e., not numeric, having more than one letter)
                    // are in the sentence. If more than a quarter of this sentence is "real words,"
                    // then we really will count it as a legit sentence. If it fails this criterion,
                    // then this "sentence" is really a mathematical equation (or something similar)
                    // and we will continue to see it as an invalid sentence.
                    size_t regularWordCount = 0;
                    for (size_t wordCounter = m_sentences[sentenceCounter].get_first_word_index();
                         wordCounter <= m_sentences[sentenceCounter].get_last_word_index();
                         ++wordCounter)
                        {
                        if (!m_words[wordCounter].is_numeric() &&
                            m_words[wordCounter].get_length_excluding_punctuation() > 1)
                            {
                            ++regularWordCount;
                            }
                        }
                    if (regularWordCount >
                        safe_divide<size_t>(m_sentences[sentenceCounter].get_word_count(), 4))
                        {
                        theParagraph.set_valid(true);
                        m_sentences[sentenceCounter].set_valid(true);
                        if (m_sentences[sentenceCounter].get_type() ==
                            grammar::sentence_paragraph_type::incomplete)
                            {
                            m_sentences[sentenceCounter].set_type(
                                grammar::sentence_paragraph_type::complete);
                            }
                        // go through the words
                        for (size_t wordCounter =
                                 m_sentences[sentenceCounter].get_first_word_index();
                             wordCounter <= m_sentences[sentenceCounter].get_last_word_index();
                             ++wordCounter)
                            {
                            m_words[wordCounter].set_valid(true);
                            }
                        }
                    }
                }
            }

        /* Now, do the opposite with terminated sentences. If they contain nothing but
           numeric words (yet is terminated), it is some sort of equation or list of numbers
           followed by a period. This is not a real sentence and should be made invalid. Also, if
           using aggressive exclusion, exclude single-word sentences that end with a colon.*/
        for (auto& theParagraph : m_paragraphs)
            {
            // go through the sentences
            for (size_t sentenceCounter = theParagraph.get_first_sentence_index();
                 sentenceCounter <= theParagraph.get_last_sentence_index(); ++sentenceCounter)
                {
                if (m_sentences[sentenceCounter].is_valid())
                    {
                    // See how many "real words" (not numeric and contains at least 2 letters) are
                    // in the sentence. If none, then this "sentence" is really an equation (or list
                    // of initials or the like) and we will set it as an invalid sentence.
                    size_t regularWordCount = 0;
                    for (size_t wordCounter = m_sentences[sentenceCounter].get_first_word_index();
                         wordCounter <= m_sentences[sentenceCounter].get_last_word_index();
                         ++wordCounter)
                        {
                        if (!m_words[wordCounter].is_numeric() &&
                            m_words[wordCounter].get_length_excluding_punctuation() > 1)
                            {
                            ++regularWordCount;
                            }
                        }
                    if (regularWordCount == 0 ||
                        // a sentence like "Page:" will be excluded if being aggressive
                        (regularWordCount == 1 && is_exclusion_aggressive() &&
                         traits::case_insensitive_ex::eq(
                             m_sentences[sentenceCounter].get_ending_punctuation(),
                             common_lang_constants::COLON)))
                        {
                        theParagraph.set_valid(false);
                        m_sentences[sentenceCounter].set_valid(false);
                        if (m_sentences[sentenceCounter].get_type() ==
                            grammar::sentence_paragraph_type::complete)
                            {
                            m_sentences[sentenceCounter].set_type(
                                grammar::sentence_paragraph_type::incomplete);
                            }
                        // go through the words
                        for (size_t wordCounter =
                                 m_sentences[sentenceCounter].get_first_word_index();
                             wordCounter <= m_sentences[sentenceCounter].get_last_word_index();
                             ++wordCounter)
                            {
                            m_words[wordCounter].set_valid(false);
                            }
                        }
                    }
                }
            }

        // go through the sentences and mark single sentences within blocks of
        // invalid sentences as invalid.
        for (auto sentPos = m_sentences.begin(); sentPos != m_sentences.end();
             /*in loop*/)
            {
            if (!sentPos->is_valid())
                {
                // look forward and see how many consecutive incomplete sentences there
                // are from here
                size_t consecutiveIncompleteSentences = 0;
                auto nextSentIter = sentPos;
                while (nextSentIter != m_sentences.end() && !nextSentIter->is_valid())
                    {
                    ++consecutiveIncompleteSentences;
                    ++nextSentIter;
                    }
                const size_t surroundingInvalidSentenceCountCriterion =
                    is_exclusion_aggressive() ? 1 : 3;
                // if on a valid sentence after 3 invalid sentences (or 1 if in aggressive mode)...
                if (consecutiveIncompleteSentences >= surroundingInvalidSentenceCountCriterion &&
                    nextSentIter != m_sentences.end())
                    {
                    // move current position to the valid sentence and scan to next sentence
                    sentPos = nextSentIter++;
                    consecutiveIncompleteSentences = 0;
                    while (nextSentIter != m_sentences.end() && !nextSentIter->is_valid())
                        {
                        ++consecutiveIncompleteSentences;
                        ++nextSentIter;
                        }
                    // If there are at least 3 following invalid sentences, then this sentence is
                    // buried in a block of invalid sentences--mark it as invalid and part of the
                    // list. Note that this sentence must also be less than 5 words (otherwise, it's
                    // unlikely to be a list item). Also note that if we are being aggressive, then
                    // it just needs to be surrounded by one invalid sentence and the length
                    // criterion is moved up to less than 10.
                    if (consecutiveIncompleteSentences >=
                            surroundingInvalidSentenceCountCriterion &&
                        ((is_exclusion_aggressive() && sentPos->get_word_count() < 10) ||
                         (sentPos->get_word_count() < 5)))
                        {
                        sentPos->set_valid(false); // paragraph will be set to invalid later
                        sentPos->set_type(grammar::sentence_paragraph_type::list_item);
                        // If aggressively setting list items, then change surrounding sentences to
                        // list items also (in case they were headers or incomplete sentences).
                        if (is_exclusion_aggressive())
                            {
                            // proceeding sentence(s)
                            auto surroundingSentIter = (sentPos - 1);
                            while (!surroundingSentIter->is_valid())
                                {
                                assert(surroundingSentIter->get_type() !=
                                       grammar::sentence_paragraph_type::complete);
                                surroundingSentIter->set_type(
                                    grammar::sentence_paragraph_type::list_item);
                                if (surroundingSentIter == m_sentences.begin())
                                    {
                                    break;
                                    }
                                --surroundingSentIter;
                                }
                            // following sentence(s)
                            surroundingSentIter = (sentPos + 1);
                            while (surroundingSentIter != m_sentences.end() &&
                                   !surroundingSentIter->is_valid())
                                {
                                assert(surroundingSentIter->get_type() !=
                                       grammar::sentence_paragraph_type::complete);
                                surroundingSentIter->set_type(
                                    grammar::sentence_paragraph_type::list_item);
                                ++surroundingSentIter;
                                }
                            }
                        }
                    }
                else
                    {
                    ++sentPos;
                    }
                }
            else
                {
                ++sentPos;
                }
            }

        // ...then go through the paragraphs and mark them as valid or invalid,
        // depending on the validity of their sentences.
        // The sentences will already be set to valid if a header and we are treating headers as
        // valid, so we just need to set the type and sentence validity flag to the parent
        // paragraph. This will also mark paragraphs that have no valid sentences as invalid as
        // well.
        for (auto& theParagraph : m_paragraphs)
            {
            if (theParagraph.get_sentence_count() == 1)
                {
                theParagraph.set_type(
                    m_sentences[theParagraph.get_first_sentence_index()].get_type());
                }
            // Go through the sentences in the paragraph.
            // If it has at least one valid sentence then mark it as valid; otherwise mark it as
            // invalid.
            for (size_t sentenceCounter = theParagraph.get_first_sentence_index();
                 sentenceCounter <= theParagraph.get_last_sentence_index(); ++sentenceCounter)
                {
                if (m_sentences[sentenceCounter].is_valid())
                    {
                    theParagraph.set_valid(true);
                    break;
                    }
                theParagraph.set_valid(false);
                }
            }

        // ignore trailing copyright notices (this will override items tagged as headers and set
        // them to incomplete, if need be).
        if (m_ignore_trailing_copyright_notice_paragraphs)
            {
            if (!m_paragraphs.empty())
                {
                ignore_copyright_notice_paragraphs_simple();
                // find the last valid paragraph and then deduce from there if the last 2 (or 1)
                // paragraphs are copyright notices
                long paraIter = static_cast<long>(m_paragraphs.size() - 1);
                for (/*initialized outside of loop*/; paraIter >= 0; --paraIter)
                    {
                    if (m_paragraphs[paraIter].is_valid())
                        {
                        break;
                        }
                    }
                // review the last two paragraphs
                if (paraIter > 1)
                    {
                    ignore_paragraph_if_copyright_notice_aggressive(m_paragraphs[paraIter - 1]);
                    ignore_paragraph_if_copyright_notice_aggressive(m_paragraphs[paraIter]);
                    }
                // ...unless the last valid one is at second paragraph, then just look at the last
                // valid one so that we will at least have one valid paragraph after removing
                // copyrights
                else if (paraIter == 1)
                    {
                    ignore_paragraph_if_copyright_notice_aggressive(m_paragraphs[paraIter]);
                    }
                // Note that if the last valid paragraph is the only paragraph, then don't bother
                // reviewing it. This way you can review a single paragraph that might have
                // copyright information in it. The point of this feature it to remove "trailing"
                // copyright notices, not the actual document.
                }
            }

        update_citation_info();

        auto punctPos = (!m_punctuation.empty()) ? m_punctuation.cbegin() : m_punctuation.cend();
        // Update the extra info attached to individual words and tally up the valid sentence and
        // paragraph counts. Start with going through the paragraphs and sort out invalid and
        // grammatically questionable sentences.
        for (const auto& theParagraph : m_paragraphs)
            {
            // go through the sentences
            for (size_t sentenceCounter = theParagraph.get_first_sentence_index();
                 sentenceCounter <= theParagraph.get_last_sentence_index(); ++sentenceCounter)
                {
                if (sentenceCounter < m_sentences.size())
                    {
                    const bool isCurrentSentenceComplete = m_sentences[sentenceCounter].is_valid();
                    // go through the words
                    for (size_t i = m_sentences[sentenceCounter].get_first_word_index();
                         i <= m_sentences[sentenceCounter].get_last_word_index(); ++i)
                        {
                        auto& word = m_words[i];
                        word.set_valid(isCurrentSentenceComplete);
                        }
                    // see if this sentence begins with a conjunction and if it is capitalized
                    if (m_sentences[sentenceCounter].get_word_count() >= 1)
                        {
                        const Tword_type& firstWord =
                            m_words[m_sentences[sentenceCounter].get_first_word_index()];
                        if (isConjunction(
                                std::wstring_view{ firstWord.c_str(), firstWord.length() }))
                            {
                            m_conjunction_beginning_sentences.push_back(sentenceCounter);
                            }
                        // see if the first word of the sentence is followed by a "=". If so, then
                        // it's an equation, so that will be OK to start a sentence and still be
                        // lowercased.
                        while (punctPos != m_punctuation.end() &&
                               (punctPos->get_word_position() <=
                                m_sentences[sentenceCounter].get_first_word_index()))
                            {
                            ++punctPos;
                            }
                        if (punctPos != m_punctuation.end() &&
                            punctPos->get_word_position() ==
                                m_sentences[sentenceCounter].get_first_word_index() + 1 &&
                            traits::case_insensitive_ex::eq(punctPos->get_punctuation_mark(), L'='))
                            {
                            continue;
                            }
                        // If not a number (not even starting with a number) and not capitalized,
                        // then this sentence begins with a lowercased word.
                        // Also, if it is a list bullet then it is OK to be lowercased.
                        // Headers should be uppercased though.
                        // Also, something like "p-Level" or "t-Test" is OK to start a sentence,
                        // those are not meant to be capitalized.
                        if (firstWord.length() && !firstWord.is_numeric() &&
                            !characters::is_character::is_numeric(firstWord[0]) &&
                            !firstWord.is_capitalized() &&
                            !characters::is_character::is_punctuation(firstWord[0]) &&
                            (firstWord.length() == 1 ||
                             !characters::is_character::is_dash_or_hyphen(firstWord[1])) &&
                            // "a." type of bullet
                            (firstWord.length() == 1 ||
                             !traits::case_insensitive_ex::eq(firstWord[1],
                                                              common_lang_constants::PERIOD)) &&
                            m_sentences[sentenceCounter].get_type() !=
                                grammar::sentence_paragraph_type::list_item)
                            {
                            m_lowercase_beginning_sentences.push_back(sentenceCounter);
                            }
                        }
                    }
                }
            }

        // Establish which words are proper. This must be done before any other grammar analysis.
        if (searches_for_proper_nouns())
            {
            search_for_proper_nouns();
            }

        search_for_sentences_with_overused_words_by_sentence();
        search_for_excluded_words();
        ignore_tagged_blocks();
        analyze_grammar();

        if (searches_for_negated_phrases() || searches_for_proper_phrases() ||
            get_n_gram_sizes_to_auto_detect().size())
            {
            if (searches_for_proper_phrases())
                {
                search_for_proper_noun_phrases();
                }
            if (searches_for_negated_phrases())
                {
                search_for_negated_phrases();
                }
            if (get_n_gram_sizes_to_auto_detect().size())
                {
                using phrasePositions = std::vector<std::pair<size_t, size_t>>;
                std::vector<phrasePositions> foundPhrases;
                foundPhrases.resize(get_n_gram_sizes_to_auto_detect().size());
                /// @todo maybe make this parallel
                for (size_t i = 0; i < get_n_gram_sizes_to_auto_detect().size(); ++i)
                    {
                    search_for_n_grams(foundPhrases[i], get_n_gram_sizes_to_auto_detect().at(i));
                    }
                // combine all word combination sets
                for (const auto& fPhrases : foundPhrases)
                    {
                    m_n_grams_indices.insert(m_n_grams_indices.end(), fPhrases.cbegin(),
                                             fPhrases.cend());
                    }
                }
            }

        calculate_sentence_units_and_punctuation();
        update_valid_words_count();
        update_sentence_valid_words_count();
        update_paragraph_valid_sentence_count();
        }

    [[nodiscard]]
    const auto& get_words() const noexcept
        {
        return m_words;
        }

    [[nodiscard]]
    const Tword_type& get_word(const size_t index) const
        {
        return m_words[index];
        }

    [[nodiscard]]
    const auto& get_sentences() const noexcept
        {
        return m_sentences;
        }

    [[nodiscard]]
    const auto& get_paragraphs() const noexcept
        {
        return m_paragraphs;
        }

    [[nodiscard]]
    const auto& get_punctuation() const noexcept
        {
        return m_punctuation;
        }

    [[nodiscard]]
    const auto& get_duplicate_word_indices() const noexcept
        {
        return m_duplicate_word_indices;
        }

    [[nodiscard]]
    const auto& get_incorrect_article_indices() const noexcept
        {
        return m_incorrect_articles;
        }

    [[nodiscard]]
    const auto& get_passive_voice_indices() const noexcept
        {
        return m_passive_voices;
        }

    [[nodiscard]]
    const auto& get_overused_words_by_sentence() const noexcept
        {
        return m_overused_words_by_sentence;
        }

    [[nodiscard]]
    const auto& get_misspelled_words() const noexcept
        {
        return m_misspelled_words;
        }

    void clear_misspelled_words() { m_misspelled_words.clear(); }

    [[nodiscard]]
    const auto& get_conjunction_beginning_sentences() const noexcept
        {
        return m_conjunction_beginning_sentences;
        }

    [[nodiscard]]
    const auto& get_lowercase_beginning_sentences() const noexcept
        {
        return m_lowercase_beginning_sentences;
        }

    /// @returns The word index (into the document) and the index into is_known_phrase's phrase
    /// list.
    ///     These are phrases found against a list of known, predefined phrases.
    /// @note These phrases' sizes can differ, so the second value should be used to look up the
    /// phrase
    ///     from the known phrase list to see how many words it is. This differs from the other
    ///     phrase vectors (which are auto-detected) that simply store the word count as the second
    ///     value.
    [[nodiscard]]
    const auto& get_known_phrase_indices() const noexcept
        {
        return m_known_phrase_indices;
        }

    /// @returns The indices of heuristically detected phrases in the document.
    ///     Basically, this will be combinations of words that appear together more than once
    ///     (that don't begin or end with auxiliary words).\n
    ///     This is broken down by word index (into the document) and the number of words
    ///     that the phrase takes up. These are detected during the analysis stage.
    [[nodiscard]]
    const auto& get_n_grams_indices() const noexcept
        {
        return m_n_grams_indices;
        }

    /// @returns The indices of heuristically detected proper phrases in the document.
    ///     This is broken down by word index (into the document) and the number of words that the
    ///     phrase takes up. These are detected during the analysis stage.
    [[nodiscard]]
    const auto& get_proper_phrase_indices() const noexcept
        {
        return m_proper_phrase_indices;
        }

    /// @returns The indices of heuristically detected negating phrases in the document.
    /// This is broken down by word index (into the document) and the number of words that the
    /// phrase takes up. These are detected during the analysis stage.
    [[nodiscard]]
    const auto& get_negating_phrase_indices() const noexcept
        {
        return m_negating_phrase_indices;
        }

    /// @returns Information about which word count combinations the phrase
    /// auto detector is looking for.
    [[nodiscard]]
    const auto& get_n_gram_sizes_to_auto_detect() const noexcept
        {
        return m_n_gram_sizes_to_auto_detect;
        }

    /// Adds a word count combination that the phrase auto detector will look for.
    /// @param wordCount The word count combinations to look for.
    void add_n_gram_size_to_auto_detect(const size_t wordCount)
        {
        assert(wordCount > 1);
        // only add it if not already in there and larger than 1.
        if (wordCount > 1 && !is_n_gram_size_being_searched_for(wordCount))
            {
            m_n_gram_sizes_to_auto_detect.push_back(wordCount);
            }
        }

    /// @returns Whether or not a specific n-gram size (i.e., word count)
    /// is set to be auto-detected.
    /// @param wordCount The word count of the n-gram.
    [[nodiscard]]
    bool is_n_gram_size_being_searched_for(const size_t wordCount) const
        {
        return std::find(m_n_gram_sizes_to_auto_detect.begin(), m_n_gram_sizes_to_auto_detect.end(),
                         wordCount) != m_n_gram_sizes_to_auto_detect.end();
        }

    /// @returns The number of *all* sentences (both valid and invalid).
    [[nodiscard]]
    size_t get_sentence_count() const noexcept
        {
        return m_sentences.size();
        }

    /// @returns The number of sentences words.
    [[nodiscard]]
    size_t get_complete_sentence_count() const noexcept
        {
        return m_complete_sentence_count;
        }

    /// @returns The number of *all* paragraphs (both valid and invalid).
    [[nodiscard]]
    size_t get_paragraph_count() const noexcept
        {
        return m_paragraphs.size();
        }

    /// @returns The number of paragraphs words.
    [[nodiscard]]
    size_t get_valid_paragraph_count() const noexcept
        {
        return m_valid_paragraph_count;
        }

    [[nodiscard]]
    size_t get_punctuation_count() const noexcept
        {
        return m_punctuation.size();
        }

    [[nodiscard]]
    size_t get_valid_punctuation_count() const noexcept
        {
        return m_valid_punctuation_count;
        }

    /// @returns The number of *all* words (both valid and invalid).
    [[nodiscard]]
    size_t get_word_count() const noexcept
        {
        return m_words.size();
        }

    /// @returns The number of valid words.
    [[nodiscard]]
    size_t get_valid_word_count() const noexcept
        {
        return m_valid_word_count;
        }

    void treat_eol_as_eos(const bool eol_as_eos) noexcept { m_treat_eol_as_eos = eol_as_eos; }

    void ignore_blank_lines_when_determining_paragraph_split(const bool ignore) noexcept
        {
        m_ignore_blank_lines_when_determining_paragraph_split = ignore;
        }

    void ignore_indenting_when_determining_paragraph_split(const bool ignore) noexcept
        {
        m_ignore_indenting_when_determining_paragraph_split = ignore;
        }

    void sentence_start_must_be_uppercased(const bool mustBeUppercased) noexcept
        {
        m_sentence_start_must_be_uppercased = mustBeUppercased;
        }

    void ignore_trailing_copyright_notice_paragraphs(const bool ignore) noexcept
        {
        m_ignore_trailing_copyright_notice_paragraphs = ignore;
        }

    void ignore_citation_sections(const bool ignore) noexcept
        {
        m_ignore_citation_sections = ignore;
        }

    void treat_header_words_as_valid(const bool treatAsValid) noexcept
        {
        m_treat_header_words_as_valid = treatAsValid;
        }

    [[nodiscard]]
    bool is_exclusion_aggressive() const noexcept
        {
        return m_aggressive_exclusion;
        }

    void set_aggressive_exclusion(const bool aggressive) noexcept
        {
        m_aggressive_exclusion = aggressive;
        }

    [[nodiscard]]
    bool searches_for_proper_nouns() const noexcept
        {
        return m_search_for_proper_nouns;
        }

    void set_search_for_proper_nouns(const bool search) noexcept
        {
        m_search_for_proper_nouns = search;
        }

    /// @returns Whether to search heuristically for proper phrases.
    /// @note searches_for_proper_nouns() should be enabled for this to work.
    [[nodiscard]]
    bool searches_for_proper_phrases() const noexcept
        {
        return m_search_for_proper_phrases;
        }

    /// Sets whether to search heuristically for proper phrases.
    /// @param search Whether to search heuristically for proper phrases.
    void set_search_for_proper_phrases(const bool search) noexcept
        {
        m_search_for_proper_phrases = search;
        }

    /// @returns Whether to search heuristically for negated phrases (e.g., "won't help").
    /// @note searches_for_proper_nouns() should be enabled for this to work.
    [[nodiscard]]
    bool searches_for_negated_phrases() const noexcept
        {
        return m_search_for_negated_phrases;
        }

    /// Sets whether to search heuristically for negated phrases.
    /// @param search Whether to search heuristically for negated phrases.
    void set_search_for_negated_phrases(const bool search) noexcept
        {
        m_search_for_negated_phrases = search;
        }

    [[nodiscard]]
    bool searches_for_passive_voice() const noexcept
        {
        return m_search_passive_voice;
        }

    void set_search_for_passive_voice(const bool search) noexcept
        {
        m_search_passive_voice = search;
        }

    void set_allowable_incomplete_sentence_size(const size_t size) noexcept
        {
        m_allowable_incomplete_sentence_size = size;
        }

    [[nodiscard]]
    size_t get_allowable_incomplete_sentence_size() const noexcept
        {
        return m_allowable_incomplete_sentence_size;
        }

    void set_syllabizer(grammar::base_syllabize* syllabizer) noexcept { syllabize = syllabizer; }

    void set_stemmer(stemming::stem<>* stemmer) { stem_word = stemmer; }

    void set_conjunction_function(const grammar::is_coordinating_conjunction* isConjunction)
        {
        is_conjunction = isConjunction;
        }

    void set_mismatched_article_function(
        const grammar::is_incorrect_article* isMismatchedArticle) noexcept
        {
        is_mismatched_article = isMismatchedArticle;
        }

    void set_known_phrase_function(const grammar::phrase_collection* known_phrase)
        {
        is_known_phrase = known_phrase;
        }

    void set_copyright_phrase_function(const grammar::phrase_collection* copyright_phrase)
        {
        is_copyright_phrase = copyright_phrase;
        }

    void set_citation_phrase_function(const grammar::phrase_collection* citation_phrase)
        {
        is_citation_phrase = citation_phrase;
        }

    void set_excluded_phrase_function(
        std::shared_ptr<const grammar::phrase_collection> excluded_phrase) noexcept
        {
        is_excluded_phrase = std::move(excluded_phrase);
        }

    void set_known_proper_nouns(const word_list* known_proper_nouns)
        {
        is_known_proper_nouns = known_proper_nouns;
        }

    void set_known_personal_nouns(const word_list* known_personal_nouns)
        {
        is_known_personal_nouns = known_personal_nouns;
        }

    void set_stop_list(const word_list* stop_list) noexcept { m_stop_list = stop_list; }

    /** @returns Whether the word at \c index is on the common word stop list.
        @note If no stop list is defined (or \c index is at an invalid position)
        then this will return @c false.*/
    [[nodiscard]]
    bool is_word_common(const size_t index) const
        {
        assert(index < m_words.size());
        return index >= m_words.size() ? false : is_word_common(m_words[index].c_str());
        }

    /** @returns Whether a given word is on list of common words.
        @param theWord The word to search for. This can be any word
            (i.e., does not have to be related to the document itself).
        @note Will return false if stop list has not been set.*/
    [[nodiscard]]
    bool is_word_common(const wchar_t* theWord) const
        {
        assert(theWord);
        return ((theWord != nullptr) && (m_stop_list != nullptr) && m_stop_list->contains(theWord));
        }

    [[nodiscard]]
    is_correctly_spelled_word<Tword_type, word_list>& get_spell_checker() noexcept
        {
        return is_correctly_spelled;
        }

    [[nodiscard]]
    bool is_excluding_file_addresses() const noexcept
        {
        return m_exclude_file_addresses;
        }

    void exclude_file_addresses(const bool exclude) noexcept { m_exclude_file_addresses = exclude; }

    [[nodiscard]]
    bool is_excluding_numerals() const noexcept
        {
        return m_exclude_numerals;
        }

    void exclude_numerals(const bool exclude) noexcept { m_exclude_numerals = exclude; }

    [[nodiscard]]
    bool is_excluding_proper_nouns() const noexcept
        {
        return m_exclude_proper_nouns;
        }

    void exclude_proper_nouns(const bool exclude) noexcept { m_exclude_proper_nouns = exclude; }

    [[nodiscard]]
    bool is_including_excluded_phrase_first_occurrence() const noexcept
        {
        return m_include_excluded_phrase_first_occurrence;
        }

    void include_excluded_phrase_first_occurrence(const bool exclude) noexcept
        {
        m_include_excluded_phrase_first_occurrence = exclude;
        }

    /// Adds a pair of punctuation marks that will indicate that the words between
    /// them should be excluded.
    /// @param tag1 The starting tag.
    /// @param tag2 The ending tag.
    void add_exclusion_block_tags(const wchar_t tag1, const wchar_t tag2)
        {
        assert(
            grammar::is_end_of_sentence::can_character_begin_or_end_parenthetical_or_quoted_section(
                tag1));
        assert(
            grammar::is_end_of_sentence::can_character_begin_or_end_parenthetical_or_quoted_section(
                tag2));
        if (grammar::is_end_of_sentence::can_character_begin_or_end_parenthetical_or_quoted_section(
                tag1) &&
            grammar::is_end_of_sentence::can_character_begin_or_end_parenthetical_or_quoted_section(
                tag2))
            {
            m_exclusion_block_tags.emplace_back(tag1, tag2);
            }
        }

    /// @returns The punctuation marks that indicate where sections of text should be excluded.
    [[nodiscard]]
    const auto& get_exclusion_block_tags() const noexcept
        {
        return m_exclusion_block_tags;
        }

    /// Removes the punctuation marks that indicate where blocks of text should be excluded.
    void clear_exclusion_block_tags() { m_exclusion_block_tags.clear(); }

    /// @returns The (known, predefined) phrases data within the document.
    [[nodiscard]]
    const grammar::phrase_collection& get_known_phrases() const noexcept
        {
        return *is_known_phrase;
        }

    /// @returns The aggregated set of tokens (words and known/negated phrases), along with their
    ///     respective word positions and frequencies from the document.
    /// @note Call aggregate_tokens() prior to using this.
    ///     aggregate_tokens() should be called after the document is loaded.
    [[nodiscard]]
    const multi_value_aggregate_map<grammar::phrase<Tword_type>, size_t>&
    get_aggregated_tokens() const noexcept
        {
        return m_aggregated_tokens;
        }

    /// Aggregates all the words, known phrases, and negated phrases into a set of unique values
    /// (with frequencies).
    /// @note This needs to be called by the client, load() will not implicitly call this because
    ///     it may not be needed by some analysis systems.
    void aggregate_tokens()
        {
        m_aggregated_tokens.clear();

        auto knownPhrases = get_known_phrase_indices().begin();
        auto negatedPhrases = get_negating_phrase_indices().begin();

        for (auto wordPos = get_words().begin(); wordPos != get_words().end(); /*in loop*/)
            {
            const size_t currentWordIndex = wordPos - get_words().begin();
            while (negatedPhrases != get_negating_phrase_indices().end() &&
                   (negatedPhrases->first < currentWordIndex))
                {
                ++negatedPhrases;
                }
            while (knownPhrases != get_known_phrase_indices().end() &&
                   (knownPhrases->first < currentWordIndex))
                {
                ++knownPhrases;
                }
            // is it a negating phrase
            if (negatedPhrases != get_negating_phrase_indices().end() &&
                negatedPhrases->first == currentWordIndex)
                {
                grammar::phrase<Tword_type> currentPhrase;
                currentPhrase.copy_words(get_words().begin() + negatedPhrases->first,
                                         negatedPhrases->second);
                m_aggregated_tokens.insert(currentPhrase, currentWordIndex);
                wordPos += negatedPhrases->second;
                }
            // or a known phrase that we are looking for
            else if (is_known_phrase && knownPhrases != get_known_phrase_indices().end() &&
                     knownPhrases->first == currentWordIndex)
                {
                assert(knownPhrases->second < is_known_phrase->get_phrases().size() &&
                       "Invalid access into known phrases when aggregating tokens.");
                grammar::phrase<Tword_type> currentPhrase;
                const size_t phraseWordCount =
                    is_known_phrase->get_phrases()[knownPhrases->second].first.get_word_count();
                currentPhrase.copy_words(get_words().begin() + knownPhrases->first,
                                         phraseWordCount);
                m_aggregated_tokens.insert(currentPhrase, currentWordIndex);
                wordPos += phraseWordCount;
                }
            // or a word not on the stop list
            else if (!is_word_common(currentWordIndex))
                {
                m_aggregated_tokens.insert(grammar::phrase<Tword_type>{ *wordPos++ },
                                           currentWordIndex);
                }
            else
                {
                ++wordPos;
                }
            }
        }

  private:
    void search_for_excluded_words()
        {
        PROFILE();
        for (auto wordPos = m_words.begin(); wordPos != m_words.end(); ++wordPos)
            {
            if (is_excluding_file_addresses() && wordPos->is_file_address())
                {
                wordPos->set_valid(false);
                }
            else if (is_excluding_numerals() && wordPos->is_numeric())
                {
                wordPos->set_valid(false);
                }
            else if (searches_for_proper_nouns() && is_excluding_proper_nouns() &&
                     wordPos->is_proper_noun())
                {
                wordPos->set_valid(false);
                }
            }
        // go through the user-defined excluded phrases (if any)
        if (is_excluded_phrase && !is_excluded_phrase->get_phrases().empty())
            {
            std::set<size_t> alreadyEncounteredExcludedPhrases;
            const grammar::phrase_collection& isExcludedPhrase = *is_excluded_phrase;
            auto punctPos =
                (!m_punctuation.empty()) ? m_punctuation.cbegin() : m_punctuation.cend();
            // search for user-defined excluded phrases throughout the sentences
            for (const auto& theSentence : m_sentences)
                {
                // go through each word in the sentence (up to the last word) and compare it
                // (and its following words) to the phrases
                for (size_t wordCounter = theSentence.get_first_word_index();
                     wordCounter <= theSentence.get_last_word_index(); ++wordCounter)
                    {
                    assert(wordCounter < m_words.size());
                    // don't bother with file paths, these wouldn't be in a phrase file
                    if ((m_words.begin() + wordCounter)->is_file_address())
                        {
                        continue;
                        }
                    // move forward until we reach either the end of the punctuation or a
                    // punctuation connected to the current word
                    while (punctPos != m_punctuation.end() &&
                           (punctPos->get_word_position() <= wordCounter))
                        {
                        ++punctPos;
                        }
                    // If there is punctuation between the current word and the next one then it
                    // can't start a phrase. In this case, tell it to only look for single-word
                    // entries in the phrase collection.
                    const bool searchForSingleWordOnly =
                        (punctPos != m_punctuation.end() &&
                         punctPos->get_word_position() == wordCounter + 1);
                    const size_t phraseResult =
                        isExcludedPhrase(m_words.begin() + wordCounter,
                                         (theSentence.get_first_word_index() - wordCounter),
                                         searchForSingleWordOnly ?
                                             1 :
                                             (theSentence.get_last_word_index() - wordCounter) + 1,
                                         true);
                    if (phraseResult != grammar::phrase_collection::npos)
                        {
                        const size_t wordCountInCurrentPhrase =
                            isExcludedPhrase.get_phrases()[phraseResult].first.get_word_count();
                        if (!is_including_excluded_phrase_first_occurrence())
                            {
                            for (size_t phraseWordPositionCounter = 0;
                                 phraseWordPositionCounter < wordCountInCurrentPhrase;
                                 ++phraseWordPositionCounter)
                                {
                                m_words[wordCounter + phraseWordPositionCounter].set_valid(false);
                                }
                            }
                        // if the first occurrence should not be excluded then skip excluding it.
                        else if (!alreadyEncounteredExcludedPhrases.insert(phraseResult).second)
                            {
                            for (size_t phraseWordPositionCounter = 0;
                                 phraseWordPositionCounter < wordCountInCurrentPhrase;
                                 ++phraseWordPositionCounter)
                                {
                                m_words[wordCounter + phraseWordPositionCounter].set_valid(false);
                                }
                            }
                        // just skip the rest of the words in this phrase
                        // (-1 to take the loop increment into account)
                        wordCounter += wordCountInCurrentPhrase - 1;
                        }
                    }
                }
            }
        }

    /** Find the proper nouns, first looking for any capitalized words *not* starting a sentence.
        The word must also be in a complete sentence. If it isn't then it is more than likely in
        a header where all the words would usually be capitalized.

        Note that during the second pass--which looks for proper nouns--the comparison against
        known proper nouns must be case-sensitive. If a word in all caps is encountered we
        will want to prevent it from making other instances of the word that are not in all caps
        from being erroneously seen as the same word.*/
    void search_for_proper_nouns()
        {
        PROFILE();
        constexpr grammar::is_acronym IS_ACRONYM;
        // this collection is forced to be case-sensitive because it relies on words being
        // capitalized to match
        frequency_set<Tword_type, string_util::less_basic_string_compare<Tword_type>> properWords;
        frequency_set<Tword_type, string_util::less_basic_string_i_compare<Tword_type>>
            nonProperWords;
        auto punctPos = !m_punctuation.empty() ? m_punctuation.cbegin() : m_punctuation.cend();
        size_t currentWordIndex{ 0 };
        m_quoteStartWords.clear();
        bool wordIsAtStartOfQuote = false;
        for (auto wordPos = m_words.begin(); wordPos != m_words.end(); ++wordPos)
            {
            // see if it is a word in all caps used to simply get the reader's attention.
            // first, see if a previous loop set this to exclamatory already while "looking ahead"
            if (wordPos->is_exclamatory())
                {
                continue;
                }
            // see if in all caps...
            if (IS_ACRONYM({ wordPos->c_str(), wordPos->length() }))
                {
                // ...if it has more than one period in it and more than half uppercase
                // letters then it absolutely must be an acronym.
                // Same for if it ends with a lowercase 's' (e.g., "ATMs").
                if (IS_ACRONYM.get_dot_count() > 1 || IS_ACRONYM.ends_with_lower_s())
                    {
                    wordPos->set_exclamatory(false);
                    wordPos->set_acronym(true);
                    }
                // ...or look at the surrounding words to see if it is
                // an acronym or simply uppercased word
                else if (m_words.size() > 1)
                    {
                    // if first word, then just look at following word
                    if (wordPos == m_words.begin())
                        {
                        auto otherWord{ std::next(wordPos) };
                        assert(otherWord != m_words.end());
                        // words need to be in the same sentence
                        if (otherWord->get_sentence_index() == wordPos->get_sentence_index() &&
                            IS_ACRONYM({ otherWord->c_str(), otherWord->length() }))
                            {
                            // following word is uppercased too, so don't treat this as
                            // an acronym or proper
                            wordPos->set_exclamatory(true);
                            wordPos->set_acronym(false);
                            continue;
                            }
                        }
                    // ...else, if last word then just look at the word in front of it
                    else if (wordPos == m_words.end() - 1)
                        {
                        auto otherWord{ std::prev(wordPos) };
                        // words need to be in the same sentence
                        if (otherWord->get_sentence_index() == wordPos->get_sentence_index() &&
                            IS_ACRONYM({ otherWord->c_str(), otherWord->length() }))
                            {
                            // proceeding word is uppercased too, so don't treat this as
                            // an acronym or proper
                            wordPos->set_exclamatory(true);
                            wordPos->set_acronym(false);
                            continue;
                            }
                        }
                    // otherwise, look at the word before and after and if either are
                    // uppercased then it's not an acronym or proper (it's exclamatory)
                    else if (m_words.size() > 2) // between two other words
                        {
                        auto otherWord{ std::prev(wordPos) };
                        // words need to be in the same sentence
                        if (otherWord->get_sentence_index() == wordPos->get_sentence_index() &&
                            otherWord->is_exclamatory())
                            {
                            // proceeding word was uppercased too, so don't treat this as
                            // an acronym or proper
                            wordPos->set_exclamatory(true);
                            wordPos->set_acronym(false);
                            continue;
                            }
                        otherWord = std::next(wordPos);
                        assert(otherWord != m_words.end());
                        // words need to be in the same sentence
                        if (otherWord->get_sentence_index() == wordPos->get_sentence_index() &&
                            IS_ACRONYM({ otherWord->c_str(), otherWord->length() }))
                            {
                            // following word is uppercased too, so don't treat this as
                            // an acronym or proper
                            wordPos->set_exclamatory(true);
                            wordPos->set_acronym(false);
                            continue;
                            }
                        }
                    }
                // if we get this far then the word really is an acronym
                wordPos->set_exclamatory(false);
                wordPos->set_acronym(true);
                }
            else
                {
                wordPos->set_exclamatory(false);
                wordPos->set_acronym(false);
                }
            // now review whether the word is proper
            if (wordPos->length() >
                    1 && // an initial would have a period after it or 'I' aren't considered proper
                wordPos->is_capitalized() &&
                // headers might be considered valid, but for deducing proper nouns we should
                // only look at "real" sentences because headers generally uppercase each important
                // word, not just proper nouns.
                m_sentences[wordPos->get_sentence_index()].get_type() ==
                    grammar::sentence_paragraph_type::complete &&
                (wordPos->get_sentence_position() != 0))
                {
                // Watch our for numeric bullet-point lists. If this is a complete sentence that is
                // really a single sentence paragraph with a "1." in front and this is the first
                // word after that, then this is a list and this capitalized word is really the
                // first (real) word in this sentence. Therefore, it may not really be proper.
                const auto previousWord = (wordPos - 1);
                if (wordPos->get_sentence_position() == 1 && previousWord->length() > 0 &&
                    previousWord->is_numeric() &&
                    (characters::is_character::is_period(
                         previousWord->at(previousWord->length() - 1)) ||
                     traits::case_insensitive_ex::eq(previousWord->at(previousWord->length() - 1),
                                                     L')')))
                    {
                    auto paragraphPos =
                        !m_paragraphs.empty() ? m_paragraphs.cbegin() : m_paragraphs.cend();
                    while (paragraphPos != m_paragraphs.end() &&
                           !paragraphPos->contains_sentence(wordPos->get_sentence_index()))
                        {
                        ++paragraphPos;
                        }
                    if (paragraphPos != m_paragraphs.end() &&
                        paragraphPos->get_sentence_count() == 1)
                        {
                        continue;
                        }
                    }
                wordIsAtStartOfQuote = false;
                currentWordIndex = wordPos - m_words.begin();
                // move forward until we reach either the end of the punctuation or a punctuation
                // connected to the current word
                while (punctPos != m_punctuation.end() &&
                       (punctPos->get_word_position() < currentWordIndex))
                    {
                    ++punctPos;
                    }
                // now handle all the punctuation marks with the current word (if any)
                while (punctPos != m_punctuation.end() &&
                       punctPos->get_word_position() == currentWordIndex)
                    {
                    /* If the word has a double quote *in front* of it then we will deal with it
                       later. Treat it as if it were the first word of a sentence (basically, we see
                       it as dialogue). */
                    if (!punctPos->is_connected_to_previous_word() &&
                        characters::is_character::is_double_quote(punctPos->get_punctuation_mark()))
                        {
                        m_quoteStartWords.push_back(currentWordIndex);
                        wordIsAtStartOfQuote = true;
                        }
                    /* If the word has a single quote *in front* of it then we will maybe deal with
                       it later. If the closing single quote is more than 3 words away or the next
                       punctuation not a single quote, then we will treat it as if it were the first
                       word of a sentence (basically, we see it as embedded dialogue). */
                    else if (!punctPos->is_connected_to_previous_word() &&
                             characters::is_character::is_single_quote(
                                 punctPos->get_punctuation_mark()) &&
                             punctPos + 1 != m_punctuation.end() &&
                             ((punctPos + 1)->get_word_position() > currentWordIndex + 3 ||
                              !characters::is_character::is_single_quote(
                                  (punctPos + 1)->get_punctuation_mark())))
                        {
                        m_quoteStartWords.push_back(currentWordIndex);
                        wordIsAtStartOfQuote = true;
                        }
                    ++punctPos;
                    }
                // mark the word as proper if it meets our criteria now
                if (!wordIsAtStartOfQuote)
                    {
                    // watch out for auxiliary words which are not really proper
                    if (!grammar::is_non_proper_word::get_word_list().contains(wordPos->c_str()))
                        {
                        wordPos->set_proper_noun(true);
                        properWords.insert(Tword_type(wordPos->c_str()));
                        }
                    }
                }
            // it's uncapitalized, not in caps, and mid-sentence. It must be non-proper.
            else if (!wordPos->is_capitalized() && !wordPos->is_exclamatory() &&
                     !wordPos->is_acronym())
                {
                nonProperWords.insert(Tword_type(wordPos->c_str()));
                }
            }

        // Second pass, going over words whose capitalization can be ambiguous
        // (first word of sentence, etc.)
        punctPos = !m_punctuation.empty() ? m_punctuation.begin() :
                                            m_punctuation.end(); // reset for second pass
        for (const auto& currentSentence : m_sentences)
            {
            /* If an incomplete sentence then go through its words and set them to proper if they
               were capitalized and found to have been proper elsewhere. Because all words in most
               headers are capitalized, we have to do treat these words differently than complete
               sentences. Note that headers may be considered valid sentences, so explicitly look at
               the sentence's type. */
            if (currentSentence.get_type() != grammar::sentence_paragraph_type::complete)
                {
                for (size_t i = currentSentence.get_first_word_index();
                     i <= currentSentence.get_last_word_index(); ++i)
                    {
                    Tword_type& currentWord = m_words[i];
                    if (!currentWord.is_proper_noun() && !currentWord.is_exclamatory() &&
                        (currentWord.is_capitalized() || currentWord.is_acronym()) &&
                        currentWord.length() > 1 && !currentWord.is_numeric())
                        {
                        if (is_known_proper_nouns->contains(currentWord.c_str()))
                            {
                            currentWord.set_proper_noun(true);
                            }
                        const auto propPos = properWords.get_data().find(currentWord);
                        if (propPos != properWords.get_data().cend())
                            {
                            const auto nonPropPos = nonProperWords.get_data().find(currentWord);
                            if (nonPropPos == nonProperWords.get_data().cend() ||
                                propPos->second > nonPropPos->second)
                                {
                                currentWord.set_proper_noun(true);
                                }
                            }
                        }
                    }
                }
            /* Otherwise, go through the words that begin each sentence.
               If they are found to have been a proper noun elsewhere,
               then mark them as proper. */
            else
                {
                // move punctuation index to be after the current word,
                // whether it is the next actual word or beyond.
                while (punctPos != m_punctuation.end() &&
                       (punctPos->get_word_position() <= currentSentence.get_first_word_index()))
                    {
                    ++punctPos;
                    }
                Tword_type& currentWord =
                    m_words.operator[](currentSentence.get_first_word_index());
                if (currentWord.is_proper_noun())
                    {
                    continue;
                    }
                if (currentWord.is_capitalized() && currentWord.length() > 1 &&
                    !currentWord.is_numeric())
                    {
                    if (is_known_proper_nouns->contains(currentWord.c_str()))
                        {
                        currentWord.set_proper_noun(true);
                        continue;
                        }
                    // it's in the proper nouns that we detected...
                    const auto propPos = properWords.get_data().find(currentWord);
                    if (propPos != properWords.get_data().cend())
                        {
                        const auto nonPropPos = nonProperWords.get_data().find(currentWord);
                        if (nonPropPos == nonProperWords.get_data().cend() ||
                            propPos->second > nonPropPos->second)
                            {
                            currentWord.set_proper_noun(true);
                            continue;
                            }
                        }
                    }
                // Any acronym leftovers should be made proper. This can happen if any acronym
                // begins a sentence, but isn't displayed anywhere else in the text.
                if (currentWord.is_acronym() && !currentWord.is_proper_noun())
                    {
                    // "NOTE: some important info..." Here, "NOTE:" is an exclamatory "attention
                    // getter," so don't mark it as proper, and change it from an acronym to an
                    // exclamatory word. Note that if "NOTE" was found elsewhere (mid-sentence) in
                    // the document, then it will be marked as proper above and we won't get this
                    // far.
                    if (punctPos != m_punctuation.end() &&
                        punctPos->get_word_position() ==
                            currentSentence.get_first_word_index() + 1 &&
                        traits::case_insensitive_ex::eq(punctPos->get_punctuation_mark(), L':'))
                        {
                        currentWord.set_exclamatory(true);
                        currentWord.set_acronym(false);
                        }
                    else
                        {
                        currentWord.set_exclamatory(false);
                        currentWord.set_proper_noun(true);
                        continue;
                        }
                    }
                // see if the word is in the form of Aa-Bb, which would look like two names
                // joined, like a maiden-married combo or two people's names representing a
                // collaboration.
                const size_t dashPos = currentWord.find(L'-');
                if (currentWord.length() > 5 && dashPos != Tword_type::npos && dashPos > 1 &&
                    dashPos < currentWord.length() - 3 &&
                    characters::is_character::is_upper(currentWord[0]) &&
                    characters::is_character::is_upper(currentWord[dashPos + 1]))
                    {
                    properWords.insert(currentWord);
                    currentWord.set_proper_noun(true);
                    continue;
                    }
                }
            }
        /* Go through the words that begin quotes that were not the beginning of a sentence.
           If they are found to have been a proper noun elsewhere, then mark them as proper.*/
        punctPos = !m_punctuation.empty() ? m_punctuation.begin() :
                                            // reset for final pass
                                            m_punctuation.end();
        for (auto quoteStartWord : m_quoteStartWords)
            {
            // move punctuation index to be after the current word, whether it is the next actual
            // word or beyond.
            while (punctPos != m_punctuation.end() &&
                   (punctPos->get_word_position() <= quoteStartWord))
                {
                ++punctPos;
                }
            Tword_type& currentWord = m_words.operator[](quoteStartWord);
            if (currentWord.is_proper_noun())
                {
                continue;
                }
            if (currentWord.is_capitalized() && currentWord.length() > 1 &&
                !currentWord.is_numeric() &&
                (properWords.get_data().find(currentWord) != properWords.get_data().cend() ||
                 is_known_proper_nouns->contains({ currentWord.c_str(), currentWord.length() })))
                {
                currentWord.set_proper_noun(true);
                continue;
                }
            // Any acronym leftovers should be made proper. This can happen if any acronym begins a
            // sentence, but isn't displayed anywhere else in the text.
            if (currentWord.is_acronym() && !currentWord.is_proper_noun())
                {
                // "NOTE: some important info..." Here, "NOTE:" is an exclamatory "attention
                // getter," so don't mark it as proper, and change it from an acronym to an
                // exclamatory word. Note that if "NOTE" was found elsewhere (mid-sentence) in the
                // document, then it will be marked as proper above and we won't get this far.
                if (punctPos != m_punctuation.end() &&
                    punctPos->get_word_position() == quoteStartWord + 1 &&
                    traits::case_insensitive_ex::eq(punctPos->get_punctuation_mark(), L':'))
                    {
                    currentWord.set_exclamatory(true);
                    currentWord.set_acronym(false);
                    }
                else
                    {
                    currentWord.set_exclamatory(false);
                    currentWord.set_proper_noun(true);
                    continue;
                    }
                }
            // see if the word is in the form of Aa-Bb, which would look like two names
            // joined together, like a maiden-married combo or two people's names representing a
            // collaboration.
            const size_t dashPos = currentWord.find(L'-');
            if (currentWord.length() > 5 && dashPos != Tword_type::npos && dashPos > 1 &&
                dashPos < currentWord.length() - 3 &&
                characters::is_character::is_upper(currentWord[0]) &&
                characters::is_character::is_upper(currentWord[dashPos + 1]))
                {
                properWords.insert(currentWord);
                currentWord.set_proper_noun(true);
                continue;
                }
            }
        // Finally...
        for (auto wordPos = m_words.begin(); wordPos != m_words.end(); ++wordPos)
            {
            /* Pass over the exclamatory words (uppercased words that aren't acronyms)
               and double-check that they really aren't proper.*/
            if (wordPos->is_exclamatory() && !wordPos->is_proper_noun())
                {
                // perform a case-insensitive search here because all of these words will be in all
                // caps.
                auto propPos =
                    std::find_if(properWords.get_data().cbegin(), properWords.get_data().cend(),
                                 string_util::equal_basic_string_i_compare_map<Tword_type, size_t>(
                                     Tword_type(wordPos->c_str())));
                if (propPos != properWords.get_data().cend() ||
                    // note that the *known* proper nouns are already case-insensitive.
                    is_known_proper_nouns->contains(wordPos->c_str()))
                    {
                    wordPos->set_proper_noun(true);
                    if (propPos != properWords.get_data().cend())
                        {
                        wordPos->set_acronym(
                            IS_ACRONYM({ propPos->first.c_str(), propPos->first.length() }));
                        }
                    // none of the known proper nouns are acronyms
                    else
                        {
                        wordPos->set_acronym(false);
                        }
                    }
                }
            // Mark a word personal if proper and a known personal name
            if (wordPos->is_proper_noun() && !wordPos->is_acronym() &&
                is_known_personal_nouns->contains({ wordPos->c_str(), wordPos->length() }))
                {
                wordPos->set_personal(true);
                }
            }
        }

    /// Searches for negated phrases (e.g., "not happy", "doesn't care").
    void search_for_negated_phrases()
        {
        PROFILE();
        constexpr grammar::is_negating IS_NEGATING;
        assert(searches_for_negated_phrases() &&
               "Negated phrase searching should be enabled if searching for negated phrases.");
        m_negating_phrase_indices.clear();
        auto punctPos = !m_punctuation.empty() ? m_punctuation.cbegin() : m_punctuation.cend();
        for (const auto& currentSentence : m_sentences)
            {
            // go through each word in the sentence
            for (size_t wordCounter = currentSentence.get_first_word_index();
                 // analysis that must include two words will go up to the SECOND TO LAST WORD IN
                 // THE SENTENCE
                 wordCounter < currentSentence.get_last_word_index(); ++wordCounter)
                {
                // move punctuation index to be AFTER the current word, whether it is the next
                // actual word or beyond.
                while (punctPos != m_punctuation.end() &&
                       (punctPos->get_word_position() <= wordCounter))
                    {
                    ++punctPos;
                    }
                if (IS_NEGATING({ m_words[wordCounter].c_str(), m_words[wordCounter].length() }) &&
                    // if next word has punctuation in front of it then do not count this.
                    !(punctPos != m_punctuation.end() &&
                      punctPos->get_word_position() == wordCounter + 1))
                    {
                    auto startWord = wordCounter++;
                    // scan the following words until one is not common, or punctuation separates
                    // the words, or we reach the end of the sentence.
                    while (wordCounter <= currentSentence.get_last_word_index() &&
                           wordCounter < m_words.size() &&
                           (punctPos == m_punctuation.end() ||
                            punctPos->get_word_position() > wordCounter))
                        {
                        ++wordCounter;
                        // If word was common, then keep going. Otherwise, feed in the last word
                        // that was uncommon and stop scanning.
                        if (!is_word_common(wordCounter - 1))
                            {
                            if (m_words[wordCounter - 1].is_proper_noun())
                                {
                                // if next word is proper (if current one is proper too),
                                // then feed in the rest of the proper nouns after it to complete
                                // the proper noun phrase that's part of this larger phrase.
                                while (wordCounter <= currentSentence.get_last_word_index() &&
                                       wordCounter < m_words.size() &&
                                       (punctPos == m_punctuation.end() ||
                                        punctPos->get_word_position() > wordCounter) &&
                                       (m_words[wordCounter].is_proper_noun() ||
                                        m_words[wordCounter].is_capitalized()))
                                    {
                                    ++wordCounter;
                                    }
                                break;
                                }

                            break;
                            }
                        }
                    // should certainly be the case
                    if ((wordCounter - startWord) > 1)
                        {
                        m_negating_phrase_indices.emplace_back(startWord, wordCounter - startWord);
                        }
                    // step back, loop will increment this again when it restarts
                    --wordCounter;
                    }
                }
            }
        }

    /// Searches for full-length proper names
    /// (i.e., more than one consecutive proper noun that makes up a larger word).
    void search_for_proper_noun_phrases()
        {
        PROFILE();
        assert(searches_for_proper_nouns() &&
               "Proper nouns searching should be enabled if searching for proper phrases.");
        m_proper_phrase_indices.clear();
        auto punctPos = !m_punctuation.empty() ? m_punctuation.cbegin() : m_punctuation.cend();
        auto quotePos =
            !m_quoteStartWords.empty() ? m_quoteStartWords.begin() : m_quoteStartWords.end();
        for (const auto& theSentence : m_sentences)
            {
            // go through each word in the sentence
            for (size_t wordCounter = theSentence.get_first_word_index();
                 // analysis that must include two words will go up to THE SECOND TO LAST WORD IN
                 // THE SENTENCE
                 wordCounter < theSentence.get_last_word_index(); ++wordCounter)
                {
                // move the quote starts to be up to the current word
                // (or beyond, if the current word doesn't start a quote)
                while (quotePos != m_quoteStartWords.end() && (*quotePos < wordCounter))
                    {
                    ++quotePos;
                    }
                const bool isAtStartOfQuote =
                    (quotePos != m_quoteStartWords.end() && *quotePos == wordCounter);
                // move punctuation index to be AFTER the current word, whether it is the next
                // actual word or beyond.
                while (punctPos != m_punctuation.end() &&
                       (punctPos->get_word_position() <= wordCounter))
                    {
                    ++punctPos;
                    }
                if ((m_words[wordCounter].length() > 1 && m_words[wordCounter + 1].length() > 1) &&
                    (m_words[wordCounter].is_proper_noun() ||
                     // ...or if current work is NOT starting a quote or sentence and is capitalized
                     ((!isAtStartOfQuote && m_words[wordCounter].get_sentence_position() > 0) &&
                      m_words[wordCounter].is_capitalized_not_in_caps())) &&
                    // ...and following word is capitalized or proper
                    (m_words[wordCounter + 1].is_proper_noun() ||
                     m_words[wordCounter + 1].is_capitalized_not_in_caps()) &&
                    // if next word has punctuation in front of it then do not count this.
                    !(punctPos != m_punctuation.end() &&
                      punctPos->get_word_position() == wordCounter + 1 &&
                      punctPos->get_punctuation_mark() != L':'))
                    {
                    const auto startWord = wordCounter++;
                    // scan the following words until one is not proper/capitalized,
                    // or punctuation separates the words, or we reach the end of the sentence.
                    while (wordCounter <= theSentence.get_last_word_index() &&
                           wordCounter < m_words.size() && m_words[wordCounter].length() > 1 &&
                           (m_words[wordCounter].is_proper_noun() ||
                            m_words[wordCounter].is_capitalized_not_in_caps()) &&
                           (punctPos == m_punctuation.end() ||
                            (punctPos->get_word_position() > wordCounter ||
                             punctPos->get_punctuation_mark() == L':')))
                        {
                        ++wordCounter;
                        }
                    // should certainly be the case
                    if ((wordCounter - startWord) > 1)
                        {
                        m_proper_phrase_indices.emplace_back(startWord, wordCounter - startWord);
                        }
                    // step back, loop will increment this again when it restarts
                    --wordCounter;
                    }
                }
            }
        }

    /** @brief Searches for n-grams (word groupings) that frequently occur.
        @param phraseIndices Where to store the found phrases (n-grams).\n
            This will be the indices into the words (main sort key) and word count.
        @param phraseSize The size of the n-grams to search for.*/
    void search_for_n_grams(std::vector<std::pair<size_t, size_t>>& phraseIndices,
                            const size_t phraseSize) const
        {
        PROFILE();
        auto punctPos = !m_punctuation.empty() ? m_punctuation.cbegin() : m_punctuation.cend();
        for (const auto& theSentence : m_sentences)
            {
            // go through each word in the sentence
            for (size_t wordCounter = theSentence.get_first_word_index();
                 // make sure the word + phrase size fits within the current sentence
                 wordCounter + (phraseSize - 1) <= theSentence.get_last_word_index(); ++wordCounter)
                {
                // move punctuation index to be AFTER the current word, whether it is the next
                // actual word or beyond.
                while (punctPos != m_punctuation.end() &&
                       (punctPos->get_word_position() <= wordCounter))
                    {
                    ++punctPos;
                    }
                if (!(punctPos != m_punctuation.end() &&
                      punctPos->get_word_position() == wordCounter + 1))
                    {
                    const auto startWord = wordCounter++;
                    // scan the following words if the phrase size is complete,
                    // or punctuation separates the words, or we reach the end of the sentence.
                    while (wordCounter < m_words.size() && (wordCounter - startWord) < phraseSize &&
                           (punctPos == m_punctuation.end() ||
                            punctPos->get_word_position() > wordCounter))
                        {
                        ++wordCounter;
                        }
                    // make sure we were able to read in a phrase of the requested size before
                    // punctuation short circuits it
                    if ((wordCounter - startWord) == phraseSize &&
                        ((m_stop_list == nullptr) ||
                         (!m_stop_list->contains(m_words[startWord].c_str()) &&
                          !m_stop_list->contains(m_words[wordCounter - 1].c_str()))))
                        {
                        phraseIndices.emplace_back(startWord, wordCounter - startWord);
                        }
                    // step back to the beginning of the phrase, loop will increment to the
                    // following word
                    wordCounter = startWord;
                    }
                }
            }
        }

    /// @brief Searches for grammar issues
    void analyze_grammar()
        {
        PROFILE();
        const grammar::phrase_collection& isKnownPhrase = *is_known_phrase;
        auto punctPos = !m_punctuation.empty() ? m_punctuation.cbegin() : m_punctuation.cend();
        // NOLINTNEXTLINE(misc-const-correctness): written to as a parameter later
        size_t currentPassiveVoiceWordCount{ 0 };
        for (const auto& currentSentence : m_sentences)
            {
            // go through each word in the sentence
            for (size_t wordCounter = currentSentence.get_first_word_index();
                 wordCounter <= currentSentence.get_last_word_index(); ++wordCounter)
                {
                // move punctuation index to be after the current word, whether it is the next
                // actual word or beyond.
                while (punctPos != m_punctuation.end() &&
                       (punctPos->get_word_position() <= wordCounter))
                    {
                    ++punctPos;
                    }
                // analysis that must include two words will go up to the second to last word in the
                // sentence
                if (wordCounter < currentSentence.get_last_word_index())
                    {
                    // double words
                    if ((m_words[wordCounter].length() == m_words[wordCounter + 1].length()) &&
                        string_util::stricmp<wchar_t>(m_words[wordCounter].c_str(),
                                                      m_words[wordCounter + 1].c_str()) == 0)
                        {
                        // If next word has punctuation in front of it then do not count this.
                        // Also, initials (e.g., "L. L. Bean"), numbers and various exceptions are
                        // OK to be repeated. Also, if there is punctuation between the words, then
                        // it's likely not a typo. Otherwise, it is a double word typo.
                        if (!(punctPos != m_punctuation.end() &&
                              punctPos->get_word_position() == wordCounter + 1) &&
                            !(m_words[wordCounter].length() == 2 &&
                              characters::is_character::is_period(m_words[wordCounter][1])) &&
                            !m_words[wordCounter].is_numeric() &&
                            !is_double_word_allowed(
                                { m_words[wordCounter].c_str(), m_words[wordCounter].length() }))
                            {
                            m_duplicate_word_indices.push_back(wordCounter + 1);
                            // This is an error, so no reason to do any further analysis. Go to the
                            // next word.
                            continue;
                            }
                        }
                    // article mismatching
                    else if (is_mismatched_article &&
                             is_mismatched_article->operator()(
                                 { m_words[wordCounter].c_str(), m_words[wordCounter].length() },
                                 { m_words[wordCounter + 1].c_str(),
                                   m_words[wordCounter + 1].length() }))
                        {
                        // capital 'A' in the middle of a sentence is sometimes the name of a group
                        // or initial, so ignore it.
                        if (m_words[wordCounter].get_sentence_position() > 0 &&
                            m_words[wordCounter].length() == 1 &&
                            m_words[wordCounter].is_capitalized())
                            {
                            // noop, this is OK. Just prevent this from being reported as an error
                            }
                        // No punctuation in front of next word, then this is good to check
                        else if (!(punctPos != m_punctuation.end() &&
                                   punctPos->get_word_position() == wordCounter + 1))
                            {
                            m_incorrect_articles.push_back(wordCounter);
                            // This is an error, so no reason to do any further analysis. Go to the
                            // next word.
                            continue;
                            }
                        // Or if punctuation in front of next word is a quote, then this is a valid
                        // check too. Otherwise, all other punctuation between these words would
                        // make this an invalid check, so ignore it. Note that if the article itself
                        // is quoted, then ignore it.
                        else if ((punctPos != m_punctuation.end() &&
                                  punctPos->get_word_position() == wordCounter + 1) &&
                                 characters::is_character::is_quote(
                                     punctPos->get_punctuation_mark()) &&
                                 !punctPos->is_connected_to_previous_word())
                            {
                            m_incorrect_articles.push_back(wordCounter);
                            // This is an error, so no reason to do any further analysis. Go to the
                            // next word.
                            continue;
                            }
                        }
                    // passive voice (will fall through to other analyses below because this isn't
                    // an error)
                    else if (is_passive_voice(
                                 m_words.begin() + wordCounter,
                                 (currentSentence.get_last_word_index() - wordCounter) + 1,
                                 currentPassiveVoiceWordCount))
                        {
                        // if next word has punctuation in front of it then do not count this.
                        if (!(punctPos != m_punctuation.end() &&
                              punctPos->get_word_position() == wordCounter + 1))
                            {
                            m_passive_voices.emplace_back(wordCounter,
                                                          currentPassiveVoiceWordCount);
                            // the past participle in this passive phrase can be misspelled,
                            // so we won't skip the rest of this phrase in the next loop analysis
                            }
                        }
                    }
                    // wording issues
                    {
                    auto currentWordPos{ m_words.cbegin() };
                    std::advance(currentWordPos, wordCounter);
                    // don't bother with file paths, these wouldn't be in a phrase file
                    if (currentWordPos->is_file_address())
                        {
                        continue;
                        }
                    // If there is punctuation between the current word and the next one then it
                    // can't start a phrase. In this case, tell it to only look for single-word
                    // entries in the phrase collection.
                    const bool searchForSingleWordOnly =
                        (punctPos != m_punctuation.end() &&
                         punctPos->get_word_position() == wordCounter + 1);
                    const size_t phraseResult = isKnownPhrase(
                        currentWordPos, (wordCounter - currentSentence.get_first_word_index()),
                        searchForSingleWordOnly ?
                            1 :
                            (currentSentence.get_last_word_index() - wordCounter) + 1,
                        true);
                    if (phraseResult != grammar::phrase_collection::npos)
                        {
                        m_known_phrase_indices.emplace_back(wordCounter, phraseResult);
                        // just skip the rest of the words in this phrase (-1 to take the loop
                        // increment into account)
                        wordCounter +=
                            (isKnownPhrase.get_phrases()[phraseResult].first.get_word_count()) - 1;
                        // no reason to spell check below if a known phrase
                        continue;
                        }
                    }
                // misspellings
                if (!is_correctly_spelled(m_words[wordCounter]))
                    {
                    m_misspelled_words.push_back(wordCounter);
                    }
                }
            }
        }

    /// @brief Total up the number of valid words.
    void update_valid_words_count()
        {
        m_valid_word_count = 0;
        for (const auto& word : m_words)
            {
            if (word.is_valid())
                {
                ++m_valid_word_count;
                }
            }
        }

    /// Sets the number of valid words in each sentence (some sentences may contain excluded words
    /// that an analysis would ignore). Also sets them to invalid if they have no valid words.
    void update_sentence_valid_words_count()
        {
        m_complete_sentence_count = 0;
        for (auto& theSentence : m_sentences)
            {
            size_t validWordCountCurrentSentence = 0;
            for (size_t wordCounter = theSentence.get_first_word_index();
                 wordCounter <= theSentence.get_last_word_index(); ++wordCounter)
                {
                if (m_words[wordCounter].is_valid())
                    {
                    ++validWordCountCurrentSentence;
                    }
                }
            theSentence.set_valid_word_count(validWordCountCurrentSentence);
            // all words excluded? then this is an invalid sentence.
            if (theSentence.get_valid_word_count() == 0)
                {
                theSentence.set_valid(false);
                }
            else
                {
                ++m_complete_sentence_count;
                }
            }
        }

    /// Sets paragraphs to invalid if they contain not valid sentences.
    void update_paragraph_valid_sentence_count()
        {
        m_valid_paragraph_count = 0;
        for (auto& theParagraph : m_paragraphs)
            {
            size_t validSentenceCountCurrentParagraph = 0;
            // go through the sentences
            for (size_t sentenceCounter = theParagraph.get_first_sentence_index();
                 sentenceCounter <= theParagraph.get_last_sentence_index(); ++sentenceCounter)
                {
                if (sentenceCounter < m_sentences.size())
                    {
                    if (m_sentences[sentenceCounter].is_valid())
                        {
                        ++validSentenceCountCurrentParagraph;
                        }
                    }
                }
            if (validSentenceCountCurrentParagraph == 0)
                {
                theParagraph.set_valid(false);
                }
            else
                {
                ++m_valid_paragraph_count;
                }
            }
        }

    /// @brief Counts the number of units (sections separated by dash, colon, or semicolon)
    /// in each sentence.
    void calculate_sentence_units_and_punctuation()
        {
        PROFILE();
        m_valid_punctuation_count = 0;
        size_t currentSentence = 0;
        // sentences will have at least one unit
        size_t currentUnitCount = 1;
        auto currentWordPos = static_cast<size_t>(-1);
        for (const auto& punctIter : m_punctuation)
            {
            // if punctuation is connected to previous word, then see if that word is valid
            // and increment valid count accordingly
            if (punctIter.is_connected_to_previous_word() && punctIter.get_word_position() > 0 &&
                m_sentences[m_words[punctIter.get_word_position() - 1].get_sentence_index()]
                    .is_valid())
                {
                ++m_valid_punctuation_count;
                }
            // ...or if punctuation is on current word, then see if current word is valid.
            else if (!punctIter.is_connected_to_previous_word() &&
                     // watch out for trailing punctuation at the end of the document
                     punctIter.get_word_position() < m_words.size() &&
                     m_sentences[m_words[punctIter.get_word_position()].get_sentence_index()]
                         .is_valid())
                {
                ++m_valid_punctuation_count;
                }
            if (traits::case_insensitive_ex::eq(punctIter.get_punctuation_mark(),
                                                common_lang_constants::COLON) ||
                traits::case_insensitive_ex::eq(punctIter.get_punctuation_mark(),
                                                common_lang_constants::SEMICOLON) ||
                characters::is_character::is_dash_or_hyphen(punctIter.get_punctuation_mark()))
                {
                // avoid consecutive unit delimiters (i.e., ones that are attached to the same word)
                if (punctIter.get_word_position() == currentWordPos)
                    {
                    continue;
                    }
                // in case we are on a punctuation trailing the last word of the document
                if (punctIter.get_word_position() >= m_words.size())
                    {
                    currentWordPos = m_words.size() - 1;
                    }
                else
                    {
                    currentWordPos = punctIter.get_word_position();
                    }
                const size_t sentenceIndex = m_words[currentWordPos].get_sentence_index();
                if (sentenceIndex == currentSentence)
                    {
                    ++currentUnitCount;
                    }
                // we are in a new sentence now
                else
                    {
                    m_sentences[currentSentence].set_unit_count(currentUnitCount);
                    // reset to minimum one unit + the current unit delimiter
                    currentUnitCount = 2;
                    currentSentence = sentenceIndex;
                    }
                }
            }
        // update the final sentence with a unit delimiter in it
        if (!m_sentences.empty())
            {
            m_sentences[currentSentence].set_unit_count(currentUnitCount);
            }
        }

    /// @brief Looks for repeated (uncommon) words on a sentence-by-sentence basis.
    void search_for_sentences_with_overused_words_by_sentence()
        {
        PROFILE();
        std::wstring stemmedWord;
        multi_value_aggregate_map<traits::case_insensitive_wstring_ex, size_t> uncommonWords;
        // go through the sentences
        for (auto sentIter = m_sentences.cbegin(); sentIter != m_sentences.cend(); ++sentIter)
            {
            uncommonWords.clear();
            // go through each word in the sentence
            for (size_t wordCounter = sentIter->get_first_word_index();
                 wordCounter <= sentIter->get_last_word_index(); ++wordCounter)
                {
                auto& currentWord{ m_words[wordCounter] };
                // If more than three letters, not a number, not OK to be repeated (e.g., "ha ha"),
                // not proper, and not on list of common words, then (optionally) stem it and
                // add to a frequency set.
                if (currentWord.get_length_excluding_punctuation() > 3 &&
                    !currentWord.is_numeric() && !currentWord.is_proper_noun() &&
                    !currentWord.is_acronym() &&
                    !is_double_word_allowed({ currentWord.c_str(), currentWord.length() }) &&
                    ((m_stop_list == nullptr) ||
                     !m_stop_list->contains({ currentWord.c_str(), currentWord.length() })))
                    {
                    if (stem_word != nullptr)
                        {
                        stemmedWord.assign(currentWord.c_str());
                        (*stem_word)(stemmedWord);
                        uncommonWords.insert(stemmedWord.c_str(), wordCounter);
                        }
                    else
                        {
                        uncommonWords.insert(currentWord.c_str(), wordCounter);
                        }
                    }
                }
            for (const auto& uncommonWord : uncommonWords.get_data())
                {
                // Word appeared more than once in the sentence.
                if (uncommonWord.second.second > 1)
                    {
                    // If something like "He coughed and coughed.", then just ignore it.
                    if (uncommonWord.second.first.size() == 2)
                        {
                        const auto firstWord = uncommonWord.second.first.cbegin();
                        const auto secondWord = std::next(firstWord);
                        if (((*firstWord) + 1) == *secondWord || ((*firstWord) + 2) == *secondWord)
                            {
                            continue;
                            }
                        }
                    // word appears in the sentence with a 1:5 ratio (i.e., once every fifth word)
                    if (std::ceil(safe_divide<double>(
                            static_cast<double>(sentIter->get_valid_word_count()),
                            static_cast<double>(uncommonWord.second.first.size()))) <= 5)
                        {
                        m_overused_words_by_sentence.emplace_back(
                            // Insert the sentence and word indices of the
                            // overly-used uncommon word.
                            sentIter - m_sentences.cbegin(), uncommonWord.second.first);
                        }
                    }
                }
            }
        }

    /// Examines a paragraph to see if starts with a copyright symbol and excludes it if it does.
    /// Note that this function is fairly simple in its deduction and is safe to use on any
    /// paragraph throughout the document. (Even in aggressive exclusion, it will just allow one
    /// more sentence and still be fairly strict.)
    void ignore_copyright_notice_paragraphs_simple()
        {
        PROFILE();
        const size_t maxSentenceCount = (is_exclusion_aggressive() ? 3 : 2);
        for (auto& currentParagraph : m_paragraphs)
            {
            if (!currentParagraph.is_valid())
                {
                continue;
                }
            if (currentParagraph.get_sentence_count() >= 1 &&
                currentParagraph.get_sentence_count() <= maxSentenceCount)
                {
                const grammar::sentence_info& firstSentenceInParagraph =
                    m_sentences[currentParagraph.get_first_sentence_index()];
                // look at the beginning punctuation mark in the paragraph and see if it is a
                // copyright symbol.
                if (!m_punctuation.empty())
                    {
                    auto punctPos = std::lower_bound(
                        m_punctuation.cbegin(), m_punctuation.cend(),
                        punctuation::punctuation_mark(
                            0, firstSentenceInParagraph.get_first_word_index(), false));
                    // special logic for first word in the paragraph,
                    // skip any punctuation that is actually connected to the last paragraph.
                    while (punctPos != m_punctuation.end() &&
                           punctPos->get_word_position() ==
                               firstSentenceInParagraph.get_first_word_index() &&
                           punctPos->is_connected_to_previous_word())
                        {
                        ++punctPos;
                        }
                    if (punctPos != m_punctuation.end() &&
                        ((punctPos->get_word_position() ==
                          firstSentenceInParagraph.get_first_word_index()) ||
                         (punctPos->get_word_position() ==
                          firstSentenceInParagraph.get_first_word_index() + 1)))
                        {
                        if (punctPos->get_punctuation_mark() ==
                            common_lang_constants::COPYRIGHT_SYMBOL)
                            {
                            ignore_paragraph(currentParagraph);
                            }
                        }
                    }
                }
            }
        }

    /// Examines a paragraph to see if it contains copyright information and excludes it if it does.
    /// Note that this function is fairly aggressive in its deduction and should only be used on the
    /// trailing paragraphs of a document.
    void ignore_paragraph_if_copyright_notice_aggressive(grammar::paragraph_info& currentParagraph)
        {
        PROFILE();
        const grammar::phrase_collection& isCopyrightPhrase = *is_copyright_phrase;
        if (currentParagraph.get_sentence_count() >= 1 &&
            currentParagraph.get_sentence_count() <= 2)
            {
            const grammar::sentence_info& firstSentenceInParagraph =
                m_sentences[currentParagraph.get_first_sentence_index()];
            const grammar::sentence_info& lastSentenceInParagraph =
                m_sentences[currentParagraph.get_last_sentence_index()];
            // search the paragraph for copyright, registered, or trademark symbols.
            if (!m_punctuation.empty())
                {
                auto punctPos = std::lower_bound(
                    m_punctuation.cbegin(), m_punctuation.cend(),
                    punctuation::punctuation_mark(
                        0, firstSentenceInParagraph.get_first_word_index(), false));
                // Special logic for first word in the paragraph,
                // skip any punctuation that is actually connected to the last paragraph.
                while (punctPos != m_punctuation.end() &&
                       punctPos->get_word_position() ==
                           firstSentenceInParagraph.get_first_word_index() &&
                       punctPos->is_connected_to_previous_word())
                    {
                    ++punctPos;
                    }
                while (punctPos != m_punctuation.end() &&
                       (punctPos->get_word_position() <=
                        lastSentenceInParagraph.get_last_word_index()))
                    {
                    if (punctPos->get_punctuation_mark() ==
                            common_lang_constants::COPYRIGHT_SYMBOL ||
                        punctPos->get_punctuation_mark() ==
                            common_lang_constants::REGISTERED_SYMBOL ||
                        punctPos->get_punctuation_mark() == common_lang_constants::TRADEMARK_SYMBOL)
                        {
                        ignore_paragraph(currentParagraph);
                        return;
                        }
                    ++punctPos;
                    }
                }
            // Go through each word in the paragraph and compare it (and its following words) to the
            // copyright phrases. Phrases here are allowed to be one word (e.g., "copyright").
            for (size_t sentenceCounter = currentParagraph.get_first_sentence_index();
                 sentenceCounter <= currentParagraph.get_last_sentence_index(); ++sentenceCounter)
                {
                for (size_t wordCounter = m_sentences[sentenceCounter].get_first_word_index();
                     wordCounter <= m_sentences[sentenceCounter].get_last_word_index();
                     ++wordCounter)
                    {
                    const size_t phraseResult = isCopyrightPhrase(
                        m_words.begin() + wordCounter,
                        (m_sentences[sentenceCounter].get_first_word_index() - wordCounter),
                        (m_sentences[sentenceCounter].get_last_word_index() - wordCounter) + 1,
                        true);
                    if (phraseResult != grammar::phrase_collection::npos)
                        {
                        ignore_paragraph(currentParagraph);
                        return;
                        }
                    }
                }
            }
        }

    /** @brief Sets a paragraph, its sentences, and words to be excluded.
        @param theParagraph The paragraph to exclude.*/
    void ignore_paragraph(grammar::paragraph_info& theParagraph)
        {
        for (size_t sentenceCounter = theParagraph.get_first_sentence_index();
             sentenceCounter <= theParagraph.get_last_sentence_index(); ++sentenceCounter)
            {
            m_sentences[sentenceCounter].set_valid(false);
            // go through the words in the sentence
            for (size_t wordCounter = m_sentences[sentenceCounter].get_first_word_index();
                 wordCounter <= m_sentences[sentenceCounter].get_last_word_index(); ++wordCounter)
                {
                m_words[wordCounter].set_valid(false);
                }
            }
        theParagraph.set_valid(false);
        }

    /** @returns @c true if a paragraph appears to be a citation.
        @param theParagraph The paragraph to review.

        If a paragraph has more than 4 sentences, then this will more than likely be a real
       paragraph. If that's the case, this is really a References section in the middle of the
       document, followed by real content.

        Four sentences is the rule here because most citations will look like this:

        Burbary, K. (2011, March 7).
        Facebook demographics revisited –2001 statistics.
        Social Media Today.
        Retrieved from
        http ://www.kenburbary.com/2011/03/facebook-demographics-revisited-2011-statistics-2/

        Here, we have 4 sections of the citation that look like sentences.

        A 5-sentence paragraph can be seen as a citation if it contains a sentence that is just a
       year (or "n.d."), or the first sentence ends with a year.

        We will also stop if we encounter a header while looking through the citations;
        this clearly marks the start of a new section not related to references as well.

        @warning This assumes that you have already hit a "References" header and you are simply
                 trying to determine if what comes after it is a citation. This should not be called
                 on any arbitrary paragraph.*/
    [[nodiscard]]
    bool is_paragraph_citation(const grammar::paragraph_info& theParagraph) const noexcept
        {
        // is a word a year in a citation?
        const auto isYear = [](const Tword_type& theWord) noexcept
        {
            return (theWord.length() == 4 &&
                    // a year or "n.d." (No Date)
                    (theWord.is_numeric() ||
                     traits::case_insensitive_ex::compare(theWord.c_str(), L"n.d.", 4) == 0));
        };

        // if there is a sentence that just consists of a year,
        // or if the first or last sentence end with a year.
        const auto containsYearSentence = [this, &theParagraph, &isYear]() noexcept
        {
            if (theParagraph.get_sentence_count() >= 1 && theParagraph.get_sentence_count() <= 6)
                {
                // does the first sentence end with a year
                if (isYear(get_word(get_sentences()[theParagraph.get_first_sentence_index()]
                                        .get_last_word_index())))
                    {
                    return true;
                    }
                // ... or the last sentence
                if (isYear(get_word(get_sentences()[theParagraph.get_last_sentence_index()]
                                        .get_last_word_index())))
                    {
                    return true;
                    }
                }
            // is there a year sentence (5 or fewer words and one year)
            for (size_t sentIndex = theParagraph.get_first_sentence_index();
                 sentIndex <= theParagraph.get_last_sentence_index(); ++sentIndex)
                {
                if (get_sentences()[sentIndex].get_word_count() <= 5)
                    {
                    bool hasYear{ false };
                    for (size_t wordCounter = m_sentences[sentIndex].get_first_word_index();
                         wordCounter <= m_sentences[sentIndex].get_last_word_index(); ++wordCounter)
                        {
                        if (isYear(m_words[wordCounter]))
                            {
                            hasYear = true;
                            }
                        }
                    if (hasYear)
                        {
                        return true;
                        }
                    }
                }
            return false;
        };

        // If a header or list item, true if less than 5 words. Some citations may not have a single
        // period in them and may be seen as a header, so this will make sure it is
        // more than likely a header for a new section of text.
        const auto isShortHeader = [this, &theParagraph]() noexcept
        {
            return ((theParagraph.get_type() == grammar::sentence_paragraph_type::header ||
                     theParagraph.get_type() == grammar::sentence_paragraph_type::list_item) &&
                    theParagraph.get_sentence_count() == 1 &&
                    get_sentences()[theParagraph.get_first_sentence_index()].get_word_count() <= 5);
        };

        // returns true if paragraph contains 3 or more sentences consisting of
        // 10 or more words; this would be more like a real block of text than a citation.
        const auto containsManyLongSentences = [this, &theParagraph]() noexcept
        {
            size_t longSentences{ 0 };
            for (size_t sentIndex = theParagraph.get_first_sentence_index();
                 sentIndex <= theParagraph.get_last_sentence_index(); ++sentIndex)
                {
                if (get_sentences()[sentIndex].get_word_count() > 10)
                    {
                    ++longSentences;
                    }
                }
            return (longSentences >= 3);
        };

        return (!isShortHeader() && !containsManyLongSentences() &&
                (theParagraph.get_sentence_count() <= 4 ||
                 (theParagraph.get_sentence_count() <= 6 && containsYearSentence())));
        }

    void ignore_tagged_blocks()
        {
        PROFILE();
        for (auto blockTagPos = get_exclusion_block_tags().cbegin();
             blockTagPos != get_exclusion_block_tags().cend(); ++blockTagPos)
            {
            auto punctPos = m_punctuation.cbegin();
            auto endPunctPos = m_punctuation.cend();
            while ((punctPos = std::find(punctPos, m_punctuation.cend(), blockTagPos->first)) !=
                   m_punctuation.cend())
                {
                endPunctPos = std::find(punctPos + 1, m_punctuation.cend(), blockTagPos->second);
                if (endPunctPos == m_punctuation.cend())
                    {
                    break;
                    }
                // marks are on different words, so exclude what's between them
                if (endPunctPos->get_word_position() > punctPos->get_word_position())
                    {
                    for (size_t wordCounter = punctPos->get_word_position();
                         wordCounter < endPunctPos->get_word_position(); ++wordCounter)
                        {
                        m_words[wordCounter].set_valid(false);
                        }
                    }
                // if the start and end tags are the same, then start the next block search after
                // the ending one here
                if (traits::case_insensitive_ex::eq(punctPos->get_punctuation_mark(),
                                                    endPunctPos->get_punctuation_mark()))
                    {
                    punctPos = ++endPunctPos;
                    }
                else
                    {
                    ++punctPos;
                    }
                }
            }
        }

    /// @brief Looks for a trailing References section in the document and excludes it.
    void update_citation_info()
        {
        PROFILE();
        const auto citationBlockStopPoint =
            safe_divide<size_t>(m_words.size(), is_exclusion_aggressive() ? 4 : 2);
        // first, move to the last valid paragraph as our starting point. This will help us skip
        // trailing copyright notices or footers.
        auto paraIter = m_paragraphs.rbegin();
        for (/*already initialized*/; paraIter != m_paragraphs.rend(); ++paraIter)
            {
            if (paraIter->get_type() != grammar::sentence_paragraph_type::header &&
                paraIter->get_type() != grammar::sentence_paragraph_type::list_item &&
                paraIter->is_valid())
                {
                break;
                }
            }
        for (/*already initialized*/; paraIter != m_paragraphs.rend(); ++paraIter)
            {
            // Is this sentence above the cutoff point? Then stop looking upward.
            if (m_sentences[paraIter->get_first_sentence_index()].get_first_word_index() <
                citationBlockStopPoint)
                {
                break;
                }
            /* Examine the next header (moving upwards in the document) to see if it's a citation
               header. Note that we also look at any single-sentence paragraph which ends with a ':'
               (the indexing engine will not see those as headers).
               The header must be 3 words or fewer to help avoid a false positive.*/
            if ((paraIter->get_type() == grammar::sentence_paragraph_type::header ||
                 paraIter->get_type() == grammar::sentence_paragraph_type::list_item ||
                 (paraIter->get_sentence_count() == 1 &&
                  traits::case_insensitive_ex::eq(
                      m_sentences[paraIter->get_first_sentence_index()].get_ending_punctuation(),
                      common_lang_constants::COLON))) &&
                m_sentences[paraIter->get_first_sentence_index()].get_word_count() <= 3)
                {
                const grammar::phrase_collection& isCitationPhrase = *is_citation_phrase;
                const grammar::sentence_info& sentenceInParagraph =
                    m_sentences[paraIter->get_first_sentence_index()];
                bool foundCitationSection = false;
                for (size_t wordCounter = sentenceInParagraph.get_first_word_index();
                     wordCounter <= sentenceInParagraph.get_last_word_index(); ++wordCounter)
                    {
                    const size_t phraseResult = isCitationPhrase(
                        m_words.begin() + wordCounter,
                        (sentenceInParagraph.get_first_word_index() - wordCounter),
                        (sentenceInParagraph.get_last_word_index() - wordCounter) + 1, true);
                    if (phraseResult != grammar::phrase_collection::npos)
                        {
                        foundCitationSection = true;
                        break;
                        }
                    }
                if (foundCitationSection)
                    {
                    // set the "References:" paragraph to be a header (just in case it was a list
                    // item from before).
                    paraIter->set_type(grammar::sentence_paragraph_type::header);
                    for (size_t i = paraIter->get_first_sentence_index();
                         i <= paraIter->get_last_sentence_index(); ++i)
                        {
                        m_sentences[i].set_type(grammar::sentence_paragraph_type::header);
                        }
                    if (m_ignore_citation_sections)
                        {
                        ignore_paragraph(*paraIter);
                        }

                    // Create a forward iterator to one paragraph AFTER the header that we just
                    // dealt with, and then work around way downward through those citations. Note
                    // that assigning base() from a reverse_iterator to a forward_iterator
                    // automatically points it to the next item (yes, this is how it works).
                    auto paragraphIgnoreIter{ paraIter.base() };
                    /* If the citation header word position in the document is in the lower half of
                       the document, then proceed with exclusion. If it is in the upper half (or
                       fourth) of the document, then don't start excluding everything below it
                       (helps avoid false positives).*/
                    if (m_ignore_citation_sections &&
                        m_sentences[paraIter->get_first_sentence_index()].get_first_word_index() >=
                            citationBlockStopPoint)
                        {
                        while (paragraphIgnoreIter != m_paragraphs.end() &&
                               is_paragraph_citation(*paragraphIgnoreIter))
                            {
                            ignore_paragraph(*paragraphIgnoreIter);
                            ++paragraphIgnoreIter;
                            }
                        }
                    break;
                    }
                }
            }
        }

    void reset() noexcept
        {
        m_name.clear();
        m_known_phrase_indices.clear();
        m_n_grams_indices.clear();
        m_proper_phrase_indices.clear();
        m_negating_phrase_indices.clear();
        m_sentences.clear();
        m_paragraphs.clear();
        m_punctuation.clear();
        m_words.clear();
        m_duplicate_word_indices.clear();
        m_incorrect_articles.clear();
        m_passive_voices.clear();
        m_overused_words_by_sentence.clear();
        m_conjunction_beginning_sentences.clear();
        m_lowercase_beginning_sentences.clear();
        m_misspelled_words.clear();
        m_aggregated_tokens.clear();
        m_valid_punctuation_count = m_current_sentence_begin = m_current_paragraph_begin =
            m_valid_paragraph_count = m_complete_sentence_count = m_valid_word_count = 0;
        }

    std::wstring m_name;
    grammar::base_syllabize* syllabize{ nullptr };
    stemming::stem<>* stem_word{ nullptr };
    const grammar::is_incorrect_article* is_mismatched_article{ nullptr };
    grammar::is_english_passive_voice is_passive_voice;
    const grammar::is_coordinating_conjunction* is_conjunction{ nullptr };
    const grammar::phrase_collection* is_known_phrase{
        nullptr
    }; // this should be shared from a parent
    const word_list* is_known_proper_nouns{ nullptr };
    const word_list* is_known_personal_nouns{ nullptr };
    const grammar::phrase_collection* is_copyright_phrase{
        nullptr
    }; // this should be shared from a parent
    const grammar::phrase_collection* is_citation_phrase{
        nullptr
    }; // this should be shared from a parent
    std::shared_ptr<const grammar::phrase_collection> is_excluded_phrase{
        nullptr
    }; // this should be shared from a parent

    const word_list* m_stop_list{ nullptr };
    is_correctly_spelled_word<Tword_type, word_list> is_correctly_spelled;
    characters::is_character is_character;
    grammar::is_non_proper_word non_proper;
    grammar::is_double_word_exception is_double_word_allowed;
    std::vector<std::pair<wchar_t, wchar_t>> m_exclusion_block_tags;
    // data structures and values
    //--------------------------
    std::vector<size_t> m_quoteStartWords;
    // the index into the words and the index into the list of known phrases
    // (which is inside "is_known_phrase")
    std::vector<comparable_first_pair<size_t, size_t>> m_known_phrase_indices;
    std::vector<std::pair<size_t, size_t>> m_proper_phrase_indices;
    std::vector<std::pair<size_t, size_t>> m_negating_phrase_indices;
    // the index into the words and the number of words that this phrase takes up
    std::vector<std::pair<size_t, size_t>> m_n_grams_indices;
    std::vector<size_t> m_n_gram_sizes_to_auto_detect;
    std::vector<grammar::sentence_info> m_sentences;
    std::vector<grammar::paragraph_info> m_paragraphs;
    std::vector<size_t> m_duplicate_word_indices;
    std::vector<size_t> m_conjunction_beginning_sentences;
    std::vector<size_t> m_lowercase_beginning_sentences;
    std::vector<size_t> m_misspelled_words;
    std::vector<size_t> m_incorrect_articles;
    std::vector<std::pair<size_t, size_t>> m_passive_voices;
    std::vector<std::pair<size_t, std::set<size_t>>> m_overused_words_by_sentence;
    std::vector<punctuation::punctuation_mark> m_punctuation;
    std::vector<Tword_type> m_words;
    multi_value_aggregate_map<grammar::phrase<Tword_type>, size_t> m_aggregated_tokens;
    size_t m_current_sentence_begin{ 0 };
    size_t m_current_paragraph_begin{ 0 };
    // specialized statistics
    size_t m_valid_punctuation_count{ 0 };
    size_t m_valid_paragraph_count{ 0 };
    size_t m_complete_sentence_count{ 0 };
    size_t m_valid_word_count{ 0 };
    // flags
    bool m_treat_eol_as_eos{ false };
    bool m_ignore_blank_lines_when_determining_paragraph_split{ false };
    bool m_ignore_indenting_when_determining_paragraph_split{ false };
    bool m_sentence_start_must_be_uppercased{ false };
    bool m_ignore_trailing_copyright_notice_paragraphs{ true };
    bool m_ignore_citation_sections{ true };
    bool m_treat_header_words_as_valid{ false };
    bool m_aggressive_exclusion{ false };
    bool m_search_for_proper_nouns{ true };
    bool m_search_passive_voice{ true };
    bool m_exclude_file_addresses{ false };
    bool m_exclude_numerals{ false };
    bool m_exclude_proper_nouns{ false };
    bool m_include_excluded_phrase_first_occurrence{ false };
    bool m_search_for_proper_phrases{ false };
    bool m_search_for_negated_phrases{ false };
    size_t m_allowable_incomplete_sentence_size{ 15 };
    };

#endif // WORD_COLLECTION_H
