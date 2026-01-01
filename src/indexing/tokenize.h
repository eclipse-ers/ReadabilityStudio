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

#ifndef INDEXING_TOKENIZE_H
#define INDEXING_TOKENIZE_H

#include "../OleanderStemmingLibrary/src/common_lang_constants.h"
#include "../Wisteria-Dataviz/src/debug/debug_profile.h"
#include "../Wisteria-Dataviz/src/util/i18n_string_util.h"
#include "abbreviation.h"
#include "characters.h"
#include "punctuation.h"
#include "sentence.h"
#include <vector>

namespace tokenize
    {
    /** @brief Class for tokenizing a text block into words, sentences, and paragraphs.*/
    template<typename is_characterT = characters::is_character,
             typename is_punctuationT = punctuation::is_punctuation>
    class document_tokenize
        {
      public:
        /** @brief Constructor.
            @param text The text block to analyze.
            @param length The length of the text block.
            @param always_treat_linefeeds_as_end_of_paragraph Whether to consider newlines as
                the end of a paragraph. If @c false, then parser will intelligently deduce
                when a paragraph starts.
                This is recommended for text split by newlines to fit a page.
            @param ignore_blank_lines_when_determining_paragraph_split Whether to ignore blank lines
                between lines when deducing if two lines are part of the same paragraph.
            @param ignore_indenting_when_determining_paragraph_split Whether to review line
                indenting when deducing if a line is a bullet point.
            @param sentence_start_must_be_uppercased Whether sentences must be uppercased.\n
                Recommended to be false when analyzing sloppy writing.*/
        document_tokenize(const wchar_t* text, const size_t length,
                          const bool always_treat_linefeeds_as_end_of_paragraph,
                          const bool ignore_blank_lines_when_determining_paragraph_split,
                          const bool ignore_indenting_when_determining_paragraph_split,
                          const bool sentence_start_must_be_uppercased)
            : m_current_char(text), m_text_block_beginning(text),
              m_text_block_end(text + length) /* null terminator included*/,
              m_text_block_length(length),
              // enables "smart" paragraph separation if false
              m_treat_eol_as_eop(always_treat_linefeeds_as_end_of_paragraph),
              m_ignore_blank_lines(ignore_blank_lines_when_determining_paragraph_split),
              m_ignore_indenting(ignore_indenting_when_determining_paragraph_split),
              isEndOfSentence(sentence_start_must_be_uppercased)
            {
            // skip anything like "***" at the start of the file.
            const std::pair<bool, size_t> lineSepResult =
                is_formatted_line_separator(m_current_char);
            if (lineSepResult.first)
                {
                m_current_char += lineSepResult.second;
                }
            }

        document_tokenize() = delete;

        /** @brief Read to the next word in the text.
            @returns The start of the next word, or null if at the end of the document.*/
        [[nodiscard]]
        const wchar_t* operator()()
            {
            PROFILE();
            // should only happen if text was null to begin with
            if (m_current_char == nullptr)
                {
                return nullptr;
                }

            m_is_previous_word_numeric = m_is_numeric;
            m_is_numeric = m_is_split_word = m_is_tabbed = false;
            m_current_leading_end_of_line_count = 0;
            bool isWordCPP{ false };

            // move past any leading trash characters
            bool whiteSpaceEncountered = false;
            bool encounteredSentenceEnder = false;
            bool encounteredNewLine = false;
            bool isUrl{ false };
            // scan ahead and look for the first legit character to stop on
            const wchar_t* scanPosition = m_current_char;
            while (scanPosition != m_text_block_end)
                {
                if (is_character.can_character_begin_word(scanPosition[0]))
                    {
                    break;
                    }
                // record whether there are any newlines between previous and next word.
                // this will help deduce whether the punctuation should be connected to the previous
                // or next word.
                if (m_moved_past_beginning_nontext && isEol(scanPosition[0]))
                    {
                    encounteredNewLine = true;
                    // If not already noted to be at the start of a new sentence, see if the
                    // previous character before this newline can end a sentence. If so, then update
                    // the sentence information. Note that this should be a strict review of the
                    // character.
                    if (!m_is_at_end_of_sentence && scanPosition > m_current_char &&
                        grammar::is_end_of_sentence::can_character_end_sentence_strict(
                            scanPosition[-1]))
                        {
                        m_is_at_end_of_sentence = true;
                        m_pending_sentence_ending_punctuation_pos = scanPosition - 1;
                        }
                    }
                ++scanPosition;
                }
            // Skip anything like "***" or "---" after that last word if we are coming up to the end
            // of a line. Also force a new paragraph if we encounter this.
            const std::pair<bool, size_t> lineSepResult =
                is_trailing_formatted_line_separator(m_current_char);
            if (lineSepResult.first)
                {
                m_current_char += lineSepResult.second;
                m_at_eol = m_is_at_end_of_sentence = true;
                m_current_sentence_ending_punctuation = L' ';
                }
            while (m_current_char < scanPosition)
                {
                if (is_character.can_character_begin_word(m_current_char[0]))
                    {
                    break;
                    }
                if (m_moved_past_beginning_nontext && isEol(m_current_char[0]))
                    {
                    m_at_eol = true;
                    break;
                    }
                // see if we have hit any whitespace yet between the upcoming new word and the
                // previous one
                if (characters::is_character::is_space(m_current_char[0]))
                    {
                    if (m_current_char[0] == common_lang_constants::TAB)
                        {
                        m_is_tabbed = true;
                        }
                    whiteSpaceEncountered = true;
                    ++m_current_char;
                    continue;
                    }
                // pick up any punctuation (or usual numbers like superscripts and fraction) between
                // the words
                if (isPunctuation(m_current_char[0]) ||
                    characters::is_character::is_extended_numeric(m_current_char[0]))
                    {
                    /* If it is the sentence terminating punctuation, then skip it (that is handled
                       elsewhere). If we are at EOS, haven't hit the sentence stop character yet,
                       and the current punctuation is the stop character, then set the flag that we
                       are at EOS and have hit the stop punctuation.*/
                    if (m_is_at_end_of_sentence && !encounteredSentenceEnder &&
                        m_pending_sentence_ending_punctuation_pos == m_current_char &&
                        grammar::is_end_of_sentence::can_character_end_sentence_passive(
                            m_current_char[0]))
                        {
                        encounteredSentenceEnder = true;
                        }
                    // otherwise, add this punctuation to the list
                    else
                        {
                        if (encounteredNewLine)
                            {
                            m_punctuation.emplace_back(m_current_char[0], m_word_count, true);
                            }
                        else
                            {
                            m_punctuation.emplace_back(m_current_char[0], m_word_count,
                                                       !whiteSpaceEncountered);
                            }
                        }
                    }
                ++m_current_char;
                }
            m_moved_past_beginning_nontext = true;

            const auto lengthToBlockEnd =
                [&textBlockEnd = std::as_const(m_text_block_end)](const auto* startOfBlock)
            { return static_cast<size_t>(std::distance(startOfBlock, textBlockEnd)); };

            // see if this might be a header paragraph
            if (is_at_eol() && (!m_is_at_end_of_sentence))
                {
                isEol(m_current_char, m_text_block_end);
                assert(isEol.get_eol_count() > 0);
                const wchar_t* const originalStop = m_current_char;
                m_current_char += isEol.get_characters_skipped_count();
                m_current_leading_end_of_line_count = isEol.get_eol_count();
                /* If previous line was a TOC line (e.g., "Overview....17") then start a new
                   sentence and paragraph. Otherwise, if previous line doesn't have a valid sentence
                   ender, but followed by more than one line feed, then assume that this is a
                   header. Treat it like it really is a sentence and a separate paragraph.
                   If we are always treating a line feed as a new sentence
                   then just do that.
                   Also, see if the next line is indented, bulleted, or a line of symbols uses as a
                   line separator. If so, then treat it as a new sentence and paragraph too.*/
                if (m_word_count > 0 && m_is_previous_word_numeric &&
                    get_punctuation().size() > 2 &&
                    get_punctuation().at(get_punctuation().size() - 1).get_word_position() ==
                        m_word_count - 1 &&
                    get_punctuation().at(get_punctuation().size() - 2).get_word_position() ==
                        m_word_count - 1 &&
                    string_util::is_either<wchar_t>(
                        string_util::full_width_to_narrow(get_punctuation()
                                                              .at(get_punctuation().size() - 1)
                                                              .get_punctuation_mark()),
                        L'.', common_lang_constants::ELLIPSE) &&
                    string_util::is_either<wchar_t>(
                        string_util::full_width_to_narrow(get_punctuation()
                                                              .at(get_punctuation().size() - 2)
                                                              .get_punctuation_mark()),
                        L'.', common_lang_constants::ELLIPSE))
                    {
                    ++m_current_sentence_index;
                    ++m_current_paragraph_index;
                    m_current_sentence_ending_punctuation = L' ';
                    // reset for the next word (its position will be the first in the next sentence)
                    m_sentence_position = 0;
                    }
                else if (m_treat_eol_as_eop ||
                         (!m_ignore_blank_lines && isEol.get_eol_count() > 1) ||
                         (!m_ignore_indenting &&
                          is_indented({ m_current_char, lengthToBlockEnd(m_current_char) })
                              .first) ||
                         is_formatted_line_separator(m_current_char).first)
                    {
                    ++m_current_sentence_index;
                    ++m_current_paragraph_index;
                    m_current_sentence_ending_punctuation = L' ';
                    // reset for the next word (its position will be the first in the next sentence)
                    m_sentence_position = 0;
                    }
                else if (is_bulleted({ m_current_char, lengthToBlockEnd(m_current_char) }).first)
                    {
                    bool nextToOtherBulletedLine = false;
                    // look forward to next line and see if it is a bullet
                    const size_t nextNewlineOffset = std::wcscspn(m_current_char, L"\r\n");
                    if (nextNewlineOffset != std::wstring::npos &&
                        ((m_current_char + nextNewlineOffset) < m_text_block_end))
                        {
                        const wchar_t* nextNewline = (m_current_char + nextNewlineOffset);
                        while (string_util::is_either<wchar_t>(*nextNewline, L'\n', L'\r'))
                            {
                            ++nextNewline;
                            }
                        if ((*nextNewline != 0) && (nextNewline < m_text_block_end) &&
                            is_bulleted({ nextNewline, lengthToBlockEnd(nextNewline) }).first)
                            {
                            nextToOtherBulletedLine = true;
                            }
                        }
                    // if next line is not a bullet, then see if previous line was one
                    if (!nextToOtherBulletedLine && (originalStop > m_text_block_beginning))
                        {
                        const size_t previousNewlineOffset =
                            std::wstring_view{ m_text_block_beginning,
                                               lengthToBlockEnd(m_text_block_beginning) }
                                .find_last_of(L"\r\n", (originalStop - m_text_block_beginning) - 1);
                        // previous newline or start of text
                        const wchar_t* previousNewline =
                            (previousNewlineOffset != std::wstring::npos) ?
                                (m_text_block_beginning + previousNewlineOffset) :
                                m_text_block_beginning;
                        assert(previousNewline);
                        assert(previousNewline < m_text_block_end);
                        while (string_util::is_either<wchar_t>(*previousNewline, L'\n', L'\r'))
                            {
                            ++previousNewline;
                            }
                        if ((*previousNewline != 0) && (previousNewline < m_text_block_end) &&
                            is_bulleted({ previousNewline, lengthToBlockEnd(previousNewline) })
                                .first)
                            {
                            nextToOtherBulletedLine = true;
                            }
                        }
                    if (nextToOtherBulletedLine)
                        {
                        ++m_current_sentence_index;
                        ++m_current_paragraph_index;
                        m_current_sentence_ending_punctuation = L' ';
                        // reset for the next word (its position will be the first in the next
                        // sentence)
                        m_sentence_position = 0;
                        }
                    }
                /* If we moved to the end of the document by skipping
                   ending carriage returns and wasn't terminated by punctuation
                   then reset the punctuation marker to a space.*/
                if (m_current_char == m_text_block_end)
                    {
                    m_current_sentence_ending_punctuation = L' ';
                    return nullptr;
                    }

                m_at_eol = false;
                }
            else if (m_is_at_end_of_sentence)
                {
                m_current_sentence_ending_punctuation =
                    (m_pending_sentence_ending_punctuation_pos != nullptr) ?
                        *m_pending_sentence_ending_punctuation_pos :
                        L'.';
                ++m_current_sentence_index;
                // reset for the next word
                m_sentence_position = 0;
                m_is_at_end_of_sentence = false;
                /* move past any leading trash characters and
                   see if previous sentence was the end of a paragraph*/
                while (m_current_char != m_text_block_end)
                    {
                    /* only count one new paragraph from last sentence.
                       Sometimes there may be many blank lines between paragraphs,
                       so don't count extra carriage returns as different paragraphs*/
                    if (isEol(m_current_char[0]))
                        {
                        isEol(m_current_char, m_text_block_end);
                        assert(isEol.get_eol_count() > 0);
                        m_current_char += isEol.get_characters_skipped_count();
                        m_current_leading_end_of_line_count = isEol.get_eol_count();

                        ++m_current_paragraph_index;
                        break;
                        }
                    if (is_character.can_character_begin_word(m_current_char[0]))
                        {
                        break;
                        }
                    ++m_current_char;
                    }
                if (m_current_char == m_text_block_end)
                    {
                    return nullptr;
                    }
                }
            else
                {
                /* Reset this because the first word of the new sentence has already been parsed
                   and previous sentence and paragraph info has already been passed back to the
                   caller*/
                m_current_sentence_ending_punctuation = L' ';
                }

            // move past any formatted line breaks that we will be skipped
            if (encounteredNewLine)
                {
                const std::pair<bool, size_t> currentLineSepResult =
                    is_formatted_line_separator(m_current_char);
                if (currentLineSepResult.first)
                    {
                    m_current_char += currentLineSepResult.second;
                    }
                }
            // move past any leading trash characters
            while (m_current_char < m_text_block_end)
                {
                if (is_character.can_character_begin_word(m_current_char[0]))
                    {
                    // If the first character is punctuation (e.g., '$') and not followed by a
                    // character that can be in a word (more than likely a space), then fall
                    // through to let the punctuation indexer pick it up.
                    if (isPunctuation(m_current_char[0]) &&
                        // a few punctuation marks can be full words by themselves
                        punctuation::punctuation_count::WHOLE_WORD_PUNCTUATION.find(
                            string_util::full_width_to_narrow(m_current_char[0])) ==
                            std::wstring_view::npos &&
                        std::next(m_current_char) < m_text_block_end &&
                        !is_character(m_current_char[1]))
                        { /*noop*/
                        }
                    // Otherwise, the first character is a real character or punctuation that can
                    // start a word (e.g., '$') and followed immediately by text, so quit stepping
                    // over text
                    // --we are on the word now.
                    else
                        {
                        break;
                        }
                    }
                if (isPunctuation(m_current_char[0]))
                    {
                    // only count punctuation if it is not sentence
                    // terminating punctuation (that is handled elsewhere)
                    if (!m_is_at_end_of_sentence ||
                        // if at the end of a sentence, make sure we still count " or ( after the
                        // period
                        (!grammar::is_end_of_sentence::can_character_end_sentence_strict(
                            m_current_char[0])))
                        {
                        m_punctuation.emplace_back(m_current_char[0], m_word_count, false);
                        }
                    }
                ++m_current_char;
                }

            if (m_current_char == m_text_block_end)
                {
                return nullptr;
                }
            // reset from previous return
            m_at_eol = false;

            const wchar_t* const wordStart = m_current_char++;

            while (m_current_char != m_text_block_end)
                {
                // see if it's a character (it is one that can end a sentence, then handle that
                // later)
                if (is_character(m_current_char[0]) &&
                    !grammar::is_end_of_sentence::can_character_end_sentence_strict(
                        m_current_char[0]))
                    {
                    // Hyphen is a special case: it must be followed by a valid character or CRLF
                    // (unless we are in an URL, then just keep going.)
                    if (characters::is_character::is_hyphen(m_current_char[0]) && !isUrl &&
                        !i18n_string_util::is_url(
                            { wordStart, static_cast<size_t>(m_current_char - wordStart) }))
                        {
                        // if a dash (double hyphen) then quit
                        if (m_current_char + 1 != m_text_block_end &&
                            (characters::is_character::is_hyphen(m_current_char[1])))
                            {
                            break;
                            }

                        // skip current hyphen and then step over any newlines/spaces to connect
                        // this word
                        ++m_current_char;
                        while (m_current_char != m_text_block_end)
                            {
                            if (characters::is_character::is_space(m_current_char[0]))
                                {
                                m_is_split_word = true;
                                ++m_current_char;
                                }
                            else
                                {
                                break;
                                }
                            }
                        }
                    // Make sure that super/subscripts aren't just trailing citations
                    // after the word. If they are, let the indexer pick them
                    // as punctuation in the next pass.
                    else if (string_util::is_superscript_number(m_current_char[0]) ||
                             string_util::is_subscript_number(m_current_char[0]))
                        {
                        const auto* scanAhead = m_current_char;
                        // step over any more super/sub scripts
                        while (scanAhead < m_text_block_end &&
                               (string_util::is_superscript_number(scanAhead[0]) ||
                                string_util::is_subscript_number(scanAhead[0])))
                            {
                            ++scanAhead;
                            }
                        if (scanAhead == m_text_block_end)
                            {
                            // If at the end of the text and what's in front less
                            // than 4 characters, then it's probably part of an equation.
                            // In that case, step over the superscripts and include it in the word.
                            if (m_current_char - wordStart < 4)
                                {
                                m_current_char = scanAhead;
                                }
                            // otherwise, if 4 or more characters proceed this, then pick up as
                            // punctuation.
                            break;
                            }
                        // If what's next is a legit character for a word, then this superscript is
                        // not trailing, so keep going. (Watch out for citation after the last word
                        // in a sentence though.)
                        if (is_character(scanAhead[0]) &&
                            !grammar::is_end_of_sentence::can_character_end_sentence_strict(
                                scanAhead[0]))
                            {
                            m_current_char = scanAhead;
                            }
                        // we are at the end of the word, so if a short word then
                        // include superscript as part of the word
                        else if (m_current_char - wordStart < 4)
                            {
                            m_current_char = scanAhead;
                            }
                        // or word is too long, so it's probably a citation.
                        // Stop and let the indexer see it as punctuation.
                        else
                            {
                            break;
                            }
                        }
                    else
                        {
                        ++m_current_char;
                        }
                    }
                // see if it is the end of a sentence
                else if (grammar::is_end_of_sentence::can_character_end_sentence_strict(
                             m_current_char[0]))
                    {
                    // if it really is a sentence
                    if (isEndOfSentence(m_text_block_beginning, m_text_block_length,
                                        m_current_char - m_text_block_beginning,
                                        wordStart - m_text_block_beginning, m_sentence_position))
                        {
                        m_is_at_end_of_sentence = true;
                        m_pending_sentence_ending_punctuation_pos =
                            m_text_block_beginning + (m_current_char - m_text_block_beginning);
                        break;
                        }
                    /* for situations like "now?' he", where a non-standard quote symbol
                       is used after a sentence inside a quote but if followed by a fragment*/
                    if (m_current_char + 1 != m_text_block_end &&
                        is_character.is_quote(m_current_char[1]) &&
                        (m_current_char + 2 == m_text_block_end ||
                         !traits::case_insensitive_ex::eq(m_current_char[2], L's')))
                        {
                        break;
                        }
                    // see if it is an ellipses that failed to terminate the sentence.
                    // if so, then just chop it off and let the punctuation indexer get it
                    if (characters::is_character::is_period(m_current_char[0]) &&
                        m_current_char + 1 != m_text_block_end &&
                        characters::is_character::is_period(m_current_char[1]))
                        {
                        break;
                        }
                    // same goes for Unicode ellipses symbol
                    if (m_current_char[0] == common_lang_constants::ELLIPSE)
                        {
                        break;
                        }
                    // if a colon in the middle of the text then let the punctuation
                    // indexer pick it up
                    if (traits::case_insensitive_ex::eq(m_current_char[0], L':') &&
                        m_current_char + 1 != m_text_block_end &&
                        characters::is_character::is_space(m_current_char[1]))
                        {
                        break;
                        }
                    // ...or an acronym or initial or bullet number
                    ++m_current_char;
                    }
                // obscure situations like a dash then double quote then hard return marking the end
                // of a sentence
                else if (grammar::is_end_of_sentence::can_character_end_sentence_passive(
                             m_current_char[0]) &&
                         isEndOfSentence(m_text_block_beginning, m_text_block_length,
                                         m_current_char - m_text_block_beginning,
                                         wordStart - m_text_block_beginning, m_sentence_position))
                    {
                    m_is_at_end_of_sentence = true;
                    m_pending_sentence_ending_punctuation_pos =
                        m_text_block_beginning + (m_current_char - m_text_block_beginning);
                    break;
                    }
                // or see if it's a parenthetical clause/quote/copyright symbol/asterisk at the
                // end of a sentence, or trailing copyright symbol on word
                else if (grammar::is_end_of_sentence::
                             can_character_begin_or_end_parenthetical_or_quoted_section(
                                 m_current_char[0]) ||
                         is_character.can_character_appear_between_word_and_eol(m_current_char[0]))
                    {
                    const wchar_t* nextChar = m_current_char + 1;
                    // eat any white space or other braces/parenthesis between the ')' and next
                    // character
                    while (
                        characters::is_character::is_space(nextChar[0]) ||
                        grammar::is_end_of_sentence::
                            can_character_begin_or_end_parenthetical_or_quoted_section(nextChar[0]))
                        {
                        ++nextChar;
                        }
                    // now see if the next valid character is a sentence ending punctuation mark
                    if ((nextChar < m_text_block_end) &&
                        isEndOfSentence(m_text_block_beginning, m_text_block_length,
                                        nextChar - m_text_block_beginning,
                                        wordStart - m_text_block_beginning, m_sentence_position))
                        {
                        m_is_at_end_of_sentence = true;
                        m_pending_sentence_ending_punctuation_pos =
                            m_text_block_beginning + (nextChar - m_text_block_beginning);
                        break;
                        }
                    /* otherwise, we still need to break and return the current word because
                       we are on a punctuation mark.*/
                    break;
                    }
                else if (isEol(m_current_char[0]))
                    {
                    m_at_eol = true;
                    break;
                    }
                /* It's a punctuation character (other than '.'), but it could be a date/time or
                   monetary separator*/
                else if (m_current_char + 1 != m_text_block_end &&
                         (is_character.can_character_form_date_time(m_current_char[0]) ||
                          is_character.can_character_form_monetary(m_current_char[0])))
                    {
                    // full numbers, not super/subscripts or fractions or anything like that
                    if (is_character.is_numeric_simple(m_current_char[1]))
                        {
                        // it's part of a date/time or monetary value (keep going)
                        ++m_current_char;
                        }
                    // also, might be part of an URL
                    else if (isUrl && m_current_char + 1 < m_text_block_end &&
                             !characters::is_character::is_space(m_current_char[0]))
                        {
                        ++m_current_char;
                        }
                    else if (i18n_string_util::is_url(
                                 { wordStart, static_cast<size_t>(m_current_char - wordStart) }) &&
                             m_current_char + 1 < m_text_block_end &&
                             !characters::is_character::is_space(m_current_char[0]))
                        {
                        isUrl = true;
                        ++m_current_char;
                        }
                    else
                        {
                        break;
                        }
                    }
                /* We need to scan ahead for situations where there is a space or dash between the
                   last word of a sentence and the period.*/
                else if (is_character.is_space_horizontal_except_tab(m_current_char[0]) ||
                         is_character.is_dash_or_hyphen(m_current_char[0]))
                    {
                    const wchar_t* currentPeekChar = m_current_char + 1;
                    // eat any spaces, dashes, or right brackets/parentheses
                    while (currentPeekChar < m_text_block_end &&
                           (is_character.is_space_horizontal_except_tab(currentPeekChar[0]) ||
                            is_character.is_dash_or_hyphen(currentPeekChar[0]) ||
                            traits::case_insensitive_ex::eq(
                                currentPeekChar[0], common_lang_constants::RIGHT_PARENTHESIS) ||
                            traits::case_insensitive_ex::eq(currentPeekChar[0],
                                                            common_lang_constants::RIGHT_BRACKET)))
                        {
                        ++currentPeekChar;
                        }
                    // If just spaces up until the end of the stream, then stop and have the
                    // indexer pick this up in the next pass.
                    if (currentPeekChar == m_text_block_end)
                        {
                        break;
                        }
                    /* If what we moved to is a line or sentence terminator then move the
                       current char to where we scanned ahead to and set the line/sentence state for
                       the next call to this function. Otherwise, just stop and leave the current
                       char where it was.*/
                    if (grammar::is_end_of_sentence::can_character_end_sentence_strict(
                            currentPeekChar[0]))
                        {
                        if (isEndOfSentence(m_text_block_beginning, m_text_block_length,
                                            currentPeekChar - m_text_block_beginning,
                                            wordStart - m_text_block_beginning,
                                            m_sentence_position))
                            {
                            m_is_at_end_of_sentence = true;
                            m_pending_sentence_ending_punctuation_pos =
                                m_text_block_beginning + (currentPeekChar - m_text_block_beginning);
                            // the punctuation between this word and sentence terminator
                            // will be picked up in the run
                            break;
                            }
                        // hit a period or something, but it's not the end of a sentence. Just stop
                        // and pick it up as the next word later.
                        break;
                        }
                    if (isEol(currentPeekChar[0]))
                        {
                        m_at_eol = true;
                        m_current_char = currentPeekChar;
                        break;
                        }
                    // "50 %" will be seen as one word
                    if (is_character.can_character_end_numeral(currentPeekChar[0]) &&
                        m_current_char > wordStart &&
                        is_character.is_numeric(*std::prev(m_current_char, 1)))
                        {
                        m_current_char = currentPeekChar;
                        continue;
                        }
                    // if just spaces (and maybe following dashes) after the word, but it is
                    // not terminated by a line end or sentence terminator, then just stop on this
                    // word and have the indexer scan these spaces or dashes in the next pass.
                    // Note that hyphens that can actually
                    // be part of a word will tell the parser to keep scanning.
                    if (!is_character(m_current_char[0]))
                        {
                        break;
                        }
                    }
                // might be part of an URL (a PHP request would the '=' here)
                else if (m_current_char[0] == L'=' || m_current_char[0] == L'+' ||
                         m_current_char[0] == L';')
                    {
                    if (isUrl && m_current_char + 1 < m_text_block_end &&
                        !characters::is_character::is_space(m_current_char[1]))
                        {
                        ++m_current_char;
                        }
                    else if (i18n_string_util::is_url(
                                 { wordStart, static_cast<size_t>(m_current_char - wordStart) }) &&
                             m_current_char + 1 < m_text_block_end &&
                             !characters::is_character::is_space(m_current_char[1]))
                        {
                        isUrl = true;
                        ++m_current_char;
                        }
                    // '+' is only part of a word if an URL, except for the word "C++".
                    // Even trailing + (e.g., a charge on an element) will be picked up as
                    // punctuation, but "C++" should be seen as a word. The irony of not indexing
                    // that correctly as a word is just too much.
                    else if (std::distance(wordStart, m_current_char) == 1 && *wordStart == L'C' &&
                             m_current_char + 1 < m_text_block_end && m_current_char[0] == L'+' &&
                             m_current_char[1] == L'+' &&
                             (m_current_char + 2 == m_text_block_end ||
                              (!characters::is_character::is_alpha(m_current_char[2]) &&
                               !characters::is_character::is_numeric(m_current_char[2]))))
                        {
                        m_current_char += 2;
                        isWordCPP = true;
                        }
                    else
                        {
                        break;
                        }
                    }
                // otherwise consider this the end of the word
                else
                    {
                    break;
                    }
                }

            // return the current word
            if (m_current_char > wordStart)
                {
                const wchar_t* wordEnd = wordStart + ((m_current_char - wordStart) - 1);
                if (!isWordCPP)
                    {
                    // step back if the last character cannot syntactically end a word
                    while (wordEnd > wordStart && !is_character.can_character_end_word(*wordEnd))
                        {
                        --wordEnd;
                        }
                    }

                /* Next, see if the word ends with an apostrophe (plural possessive).
                   If so, then set this trailing quote as a quote punctuation, rather
                   than an apostrophe that is part of the word. It will appear the same
                   in the end, but will prevent trailing apostrophes from breaking word comparisons.
                   We do this by simply moving the current position back one so that the next
                   tokenize call will pick up the punctuation.

                   Note that we need to loop over multiple single quotes as some text uses two
                   single quotes as a double quote.*/
                while (wordEnd > wordStart && is_character.is_single_quote(*wordEnd))
                    {
                    // ...allow things like "[be]in'" (colloquial "[be]ing") to be a full word
                    // though
                    if ((wordEnd - wordStart) >= 4 &&
                        traits::case_insensitive_ex::eq(wordEnd[-1], L'n') &&
                        traits::case_insensitive_ex::eq(wordEnd[-2], L'i') &&
                        // but watch out for legit words like "domain"
                        (m_known_spellings == nullptr ||
                         !m_known_spellings->contains(
                             std::wstring_view(wordStart, wordEnd - wordStart))))
                        {
                        break;
                        }

                    --m_current_char;
                    --wordEnd;
                    }
                m_current_word_length = (wordEnd - wordStart) + 1;
                /* If truly at the end of a sentence, then see if the last word is an acronym or
                   abbreviation. If so, then include the trailing period as part of the word. Note
                   that at this point, abbreviations that are mid-sentence will have already been
                   returned from here. So now we are just seeing if the last word of a sentence is
                   an abbreviation (that is rare, but possible) and adding the period to the word if
                   that's the case.*/
                if (m_is_at_end_of_sentence &&
                    characters::is_character::is_period(m_current_char[0]))
                    {
                    if (isAbbreviation({ wordStart, m_current_word_length + 1 }) ||
                        grammar::is_acronym::is_dotted_acronym(
                            { wordStart, m_current_word_length + 1 }))
                        {
                        ++m_current_word_length;
                        }
                    }

                // See if the word is a numeral
                m_is_numeric = is_character.is_numeric({ wordStart, m_current_word_length });

                ++m_sentence_position;
                ++m_word_count;

                return wordStart;
                }

            return nullptr;
            }

        /** @returns The index (into the document) of the last parsed paragraph.
                This is zero-based.*/
        [[nodiscard]]
        size_t get_current_sentence_index() const noexcept
            {
            return m_current_sentence_index;
            }

        /** @returns The index (into the document) of the last parsed sentence.
                This is zero-based.*/
        [[nodiscard]]
        size_t get_current_paragraph_index() const noexcept
            {
            return m_current_paragraph_index;
            }

        /** @returns The position in the current sentence that the last parsed word is in.
            @note This internally is 1-based index because the counter is incremented before the
                  current word is returned instead of the next time operator() is called. This is
                  just easier because then you don't have to mess with a state flag just
                  for this one variable.*/
        [[nodiscard]]
        size_t get_sentence_position() const noexcept
            {
            return (m_sentence_position == 0) ? 0 : m_sentence_position - 1;
            }

        /** @returns The length of the last word parsed.
            @note This will not include punctuation surrounding the word.*/
        [[nodiscard]]
        size_t get_current_word_length() const noexcept
            {
            return m_current_word_length;
            }

        /** @returns @c true if the last parsed word is at the end of a line.
            @note If the word as at the end of a sentence, then the newline won't be seen until
                  the next tokenize. In this situation, call
                  get_current_leading_end_of_line_count to see how many newlines are preceding
                  the next word.*/
        [[nodiscard]]
        bool is_at_eol() const noexcept
            {
            return m_at_eol;
            }

        /// @returns @c true if the last read word has a tab in front of it.
        [[nodiscard]]
        bool is_tabbed() const noexcept
            {
            return m_is_tabbed;
            }

        /** @returns @c true if the last word parsed was numeric.*/
        [[nodiscard]]
        bool is_numeric() const noexcept
            {
            return m_is_numeric;
            }

        /** @returns @c true if the last word is split
                (e.g., a hyphenated word, split by a newline).
                In this case the call will need to strip the newline(s)/space(s) out.*/
        [[nodiscard]]
        bool is_split_word() const noexcept
            {
            return m_is_split_word;
            }

        /** @returns The sentence ending punctuation from the last word parse
                (assuming that the last word was the last word in a sentence).
                If not at the end of a sentence, then a space is returned.
            @note This won't get read until you tokenize the first word in the next sentence.*/
        [[nodiscard]]
        wchar_t get_current_sentence_ending_punctuation() const noexcept
            {
            return m_current_sentence_ending_punctuation;
            }

        /** @returns The number of newlines in front of the last word parsed.*/
        [[nodiscard]]
        size_t get_current_leading_end_of_line_count() const noexcept
            {
            return m_current_leading_end_of_line_count;
            }

        /** @returns The punctuation indexed so far.
            @note This accumulates with every tokenizing.*/
        [[nodiscard]]
        const std::vector<punctuation::punctuation_mark>& get_punctuation() const noexcept
            {
            return m_punctuation;
            }

        /** Determines if text is a formatted line break (e.g., "----------") at the start of a new
                line. Three or more consecutive line break characters will trigger this.
            @param next_line The line of text to review.
            @returns A pair indicating whether it is a line break or not and the length of the
                line break if @c true.*/
        [[nodiscard]]
        std::pair<bool, size_t> is_formatted_line_separator(const wchar_t* next_line) const noexcept
            {
            if (next_line == nullptr)
                {
                return std::make_pair(false, 0);
                }
            const wchar_t* const start = next_line;
            size_t charCounts = 0;
            while (next_line < m_text_block_end && next_line[0] != 0 &&
                   characters::is_character::is_space(next_line[0]))
                {
                ++next_line;
                }
            while (next_line < m_text_block_end && next_line[0] != 0 &&
                   is_formatted_line_separator_char(next_line[0]))
                {
                ++charCounts;
                ++next_line;
                }
            return std::make_pair((charCounts >= 3), (charCounts >= 3) ? (next_line - start) : 0);
            }

        /** @brief Determines if text is a formatted line break (e.g., "Note***\n")
                at the end of a line.
            @details Three or more consecutive line break characters (followed by a newline)
                will trigger this.
            @details Three or more consecutive line break characters (followed by a newline)
                will trigger this.
            @param next_line The line of text to review.
            @returns A pair indicating whether it is a line break or not and the length of the
                 line break if @c true.*/
        [[nodiscard]]
        std::pair<bool, size_t>
        is_trailing_formatted_line_separator(const wchar_t* next_line) const noexcept
            {
            if (next_line == nullptr)
                {
                return std::make_pair(false, 0);
                }
            const wchar_t* const start = next_line;
            size_t charCounts = 0;
            while (next_line < m_text_block_end && next_line[0] != 0 &&
                   is_formatted_line_separator_char(next_line[0]))
                {
                ++charCounts;
                ++next_line;
                }
            const bool atEol = (next_line >= m_text_block_end) ||
                               characters::is_character::is_space_vertical(next_line[0]);
            return std::make_pair((charCounts >= 3 && atEol),
                                  (charCounts >= 3 && atEol) ? (next_line - start) : 0);
            }

        /** @brief Sets a list of known (spelled correctly) words.
            @param spellList The list of known words.*/
        void set_known_spellings(const word_list* spellList) noexcept
            {
            m_known_spellings = spellList;
            }

        /// @brief punctuation marks in between words.
        /// @note This is public for when an analysis system needs to std::move its content
        ///     after this tokenizer is finished.
        std::vector<punctuation::punctuation_mark> m_punctuation;

      protected:
        /** @returns @c true if character is something that can be used to create a
                formatted line break (e.g., "----------").
            @param ch The character to analyze.*/
        [[nodiscard]]
        constexpr static bool is_formatted_line_separator_char(const wchar_t ch) noexcept
            {
            return string_util::is_one_of(string_util::full_width_to_narrow(ch), L"+@-#*");
            }

        grammar::is_end_of_line isEol;
        is_characterT is_character;
        is_punctuationT isPunctuation;
        grammar::is_abbreviation isAbbreviation;
        grammar::is_acronym isAcronym;
        grammar::is_bulleted_text is_bulleted;
        grammar::is_indented_text is_indented;
        size_t m_sentence_position{ 0 };
        size_t m_current_sentence_index{ 0 };
        size_t m_current_paragraph_index{ 0 };
        const wchar_t* m_current_char{ nullptr };
        const wchar_t* const m_text_block_beginning{ nullptr };
        const wchar_t* const m_text_block_end{ nullptr };
        size_t m_text_block_length{ 0 };
        size_t m_current_word_length{ 0 };
        size_t m_word_count{ 0 };
        bool m_at_eol{ false };
        bool m_is_at_end_of_sentence{ false };
        bool m_is_tabbed{ false };
        bool m_is_previous_word_numeric{ false };
        bool m_is_numeric{ false };
        bool m_is_split_word{ false };
        wchar_t m_current_sentence_ending_punctuation{ L' ' };
        const wchar_t* m_pending_sentence_ending_punctuation_pos{ nullptr };
        size_t m_current_leading_end_of_line_count{ 0 };
        bool m_moved_past_beginning_nontext{ false };
        bool m_treat_eol_as_eop{ false };
        bool m_ignore_blank_lines{ false };
        bool m_ignore_indenting{ false };
        grammar::is_end_of_sentence isEndOfSentence;
        const word_list* m_known_spellings{ nullptr };
        };
    } // namespace tokenize

#endif // INDEXING_TOKENIZE_H
