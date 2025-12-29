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

#ifndef INDEXING_WORD_H
#define INDEXING_WORD_H

#include "../OleanderStemmingLibrary/src/english_stem.h"
#include "character_traits.h"
#include "characters.h"
#include "syllable.h"
#include "word_functional.h"
#include <algorithm>
#include <bitset>
#include <cassert>
#include <functional>
#include <map>
#include <string>
#include <vector>

/// @brief Flags used to describe a word.
enum class word_flags
    {
    numeric_flag,          /*!< Number (or mixed letters and numbers [predominately numbers]).*/
    is_valid_flag,         /*!< Word that is being excluded from a text analysis system.*/
    proper_noun_flag,      /*!< proper noun (may be a person's name, place, or title).*/
    personal_flag,         /*!< Proper noun that is a person's name.*/
    contraction_flag,      /*!< Contraction (e.g., it's).*/
    acronym_flag,          /*!< Acronym (e.g., KAOS).*/
    exclamatory_flag,      /*!< Uppercased words that aren't acronyms (e.g., WATCH OUT!).*/
    file_address_flag,     /*!< File path or internet address.*/
    custom_tagged_flag,    /*!< User-defined tag applies to this word.*/
    social_media_tag_flag, /*!< A hashtagged word.*/
    abbreviation_flag,     /*!< Abbreviation.*/
    WORD_FLAG_COUNT        /*!< The number of word flags available.*/
    };

/** @brief Word structure.*/
template<typename Tchar_traits = traits::case_insensitive_ex,
         typename Tstemmer = stemming::english_stem<std::basic_string<wchar_t, Tchar_traits>>>
class word : public std::basic_string<wchar_t, Tchar_traits>
    {
  public:
    word(const wchar_t* word_start, const size_t word_length, const size_t sentence_index,
         const size_t sentence_position, const size_t paragraph_index,
         const bool is_date_time_money, const bool isInsideCompleteSentence,
         const bool isProperNoun, const bool isAcronym, const size_t syllable_count,
         const size_t punctuation_count)
        : std::basic_string<wchar_t, Tchar_traits>(word_start, word_length),
          m_stem(word_start, word_length),
          m_sentence_index(sentence_index) /*sentence that the word is in*/,
          m_sentence_position(sentence_position) /*word's position in the sentence*/,
          m_paragraph_index(paragraph_index) /*paragraph that the word is in*/,
          m_syllable_count(syllable_count), m_punctuation_count(punctuation_count)
        {
        set_numeric(is_date_time_money);
        set_valid(isInsideCompleteSentence);
        set_proper_noun(isProperNoun);
        set_acronym(isAcronym);
        set_exclamatory(false);
        stem(m_stem);
        }

    word(const wchar_t* word_start, const size_t word_length)
        : std::basic_string<wchar_t, Tchar_traits>(word_start, word_length),
          m_stem(word_start, word_length)
        {
        stem(m_stem);
        }

    explicit word(const wchar_t* word_start)
        : std::basic_string<wchar_t, Tchar_traits>(word_start), m_stem(word_start)
        {
        stem(m_stem);
        }

    /// @private
    word() noexcept = default;

    /// @returns Whether this word doesn't equal @c that.
    /// @param that The word to compare against.
    [[nodiscard]]
    bool operator!=(const word<Tchar_traits, Tstemmer>& that) const noexcept
        {
        return compare(that) != 0;
        }

    /// @returns Whether this word is less than @c that.
    /// @param that The word to compare against.
    [[nodiscard]]
    bool operator<(const word<Tchar_traits, Tstemmer>& that) const noexcept
        {
        return compare(that) < 0;
        }

    /// @returns Whether the words are the same, compared by stems.
    /// @param that The word to compare against.
    [[nodiscard]]
    bool operator==(const word<Tchar_traits, Tstemmer>& that) const noexcept
        {
        return compare(that) == 0;
        }

    /// @returns Whether the words are the same, compared by stems.
    /// @param that The word to compare against.
    [[nodiscard]]
    bool operator==(const wchar_t* that) const
        {
        return compare(that) == 0;
        }

    /// @returns The comparison result against other word, comparing by the stems.
    /// @param that The word to compare against.
    /// @note This is the main comparison function that all other compare() and
    ///     operators functions rely on.
    [[nodiscard]]
    int compare(const word<Tchar_traits, Tstemmer>& that) const noexcept
        {
        return m_stem.compare(that.m_stem);
        }

    /// @returns The comparison result against other word, comparing by the stems.
    /// @param that The word to compare against.
    [[nodiscard]]
    int compare(const wchar_t* that) const
        {
        return compare(word<Tchar_traits, Tstemmer>(that));
        }

    /// @returns The length of the word, excluding punctuation like apostrophes.
    [[nodiscard]]
    size_t get_length_excluding_punctuation() const noexcept
        {
        return std::basic_string<wchar_t, Tchar_traits>::length() - m_punctuation_count;
        }

    /// @returns The number of punctuation marks in the word.
    [[nodiscard]]
    size_t get_punctuation_count() const noexcept
        {
        return m_punctuation_count;
        }

    void set_syllable_count(const size_t count) { m_syllable_count = count; }

    [[nodiscard]]
    size_t get_syllable_count() const noexcept
        {
        return m_syllable_count;
        }

    [[nodiscard]]
    size_t get_sentence_index() const noexcept
        {
        return m_sentence_index;
        }

    [[nodiscard]]
    size_t get_paragraph_index() const noexcept
        {
        return m_paragraph_index;
        }

    [[nodiscard]]
    size_t get_sentence_position() const noexcept
        {
        return m_sentence_position;
        }

    [[nodiscard]]
    const wchar_t* get_stem() const noexcept
        {
        return m_stem.c_str();
        }

    [[nodiscard]]
    bool is_capitalized() const
        {
        return (std::basic_string<wchar_t, Tchar_traits>::length() == 0) ?
                   false :
                   characters::is_character::is_upper(
                       std::basic_string<wchar_t, Tchar_traits>::operator[](0));
        }

    [[nodiscard]]
    bool is_capitalized_not_in_caps() const
        {
        return (std::basic_string<wchar_t, Tchar_traits>::length() == 0) ?
                   false :
               (std::basic_string<wchar_t, Tchar_traits>::length() == 1) ?
                   characters::is_character::is_upper(
                       std::basic_string<wchar_t, Tchar_traits>::operator[](0)) :
                   (characters::is_character::is_upper(
                        std::basic_string<wchar_t, Tchar_traits>::operator[](0)) &&
                    !characters::is_character::is_upper(
                        std::basic_string<wchar_t, Tchar_traits>::operator[](1)));
        }

    // flags
    void set_numeric(const bool enable)
        {
        flags.set(static_cast<size_t>(word_flags::numeric_flag), enable);
        }

    [[nodiscard]]
    bool is_numeric() const noexcept
        {
        return flags[static_cast<size_t>(word_flags::numeric_flag)];
        }

    void set_valid(const bool enable)
        {
        flags.set(static_cast<size_t>(word_flags::is_valid_flag), enable);
        }

    [[nodiscard]]
    bool is_valid() const noexcept
        {
        return flags[static_cast<size_t>(word_flags::is_valid_flag)];
        }

    void set_proper_noun(const bool enable)
        {
        flags.set(static_cast<size_t>(word_flags::proper_noun_flag), enable);
        }

    [[nodiscard]]
    inline bool is_proper_noun() const noexcept
        {
        return flags[static_cast<size_t>(word_flags::proper_noun_flag)];
        }

    void set_personal(const bool enable)
        {
        flags.set(static_cast<size_t>(word_flags::personal_flag), enable);
        }

    [[nodiscard]]
    bool is_personal() const
        {
        return flags[static_cast<size_t>(word_flags::personal_flag)];
        }

    void set_contraction(const bool enable)
        {
        flags.set(static_cast<size_t>(word_flags::contraction_flag), enable);
        }

    [[nodiscard]]
    bool is_contraction() const noexcept
        {
        return flags[static_cast<size_t>(word_flags::contraction_flag)];
        }

    void set_acronym(const bool enable)
        {
        flags.set(static_cast<size_t>(word_flags::acronym_flag), enable);
        }

    [[nodiscard]]
    bool is_acronym() const noexcept
        {
        return flags[static_cast<size_t>(word_flags::acronym_flag)];
        }

    void set_exclamatory(const bool enable)
        {
        flags.set(static_cast<size_t>(word_flags::exclamatory_flag), enable);
        }

    [[nodiscard]]
    bool is_exclamatory() const noexcept
        {
        return flags[static_cast<size_t>(word_flags::exclamatory_flag)];
        }

    void set_file_address(const bool enable)
        {
        flags.set(static_cast<size_t>(word_flags::file_address_flag), enable);
        }

    [[nodiscard]]
    bool is_file_address() const noexcept
        {
        return flags[static_cast<size_t>(word_flags::file_address_flag)];
        }

    void set_custom_tagged(const bool enable)
        {
        flags.set(static_cast<size_t>(word_flags::custom_tagged_flag), enable);
        }

    [[nodiscard]]
    bool is_custom_tagged() const
        {
        return flags[static_cast<size_t>(word_flags::custom_tagged_flag)];
        }

    void set_social_media_tag(const bool enable)
        {
        flags.set(static_cast<size_t>(word_flags::social_media_tag_flag), enable);
        }

    [[nodiscard]]
    bool is_social_media_tag() const
        {
        return flags[static_cast<size_t>(word_flags::social_media_tag_flag)];
        }

    void set_abbreviation_tag(const bool enable)
        {
        flags.set(static_cast<size_t>(word_flags::abbreviation_flag), enable);
        }

    [[nodiscard]]
    bool is_abbreviation_tag() const
        {
        return flags[static_cast<size_t>(word_flags::abbreviation_flag)];
        }

  private:
    std::basic_string<wchar_t, Tchar_traits> m_stem;
    // functors
    Tstemmer stem;

    size_t m_sentence_index{ 0 };
    size_t m_sentence_position{ 0 };
    size_t m_paragraph_index{ 0 };
    size_t m_syllable_count{ 0 };
    size_t m_punctuation_count{ 0 };

    // flags
    std::bitset<static_cast<size_t>(word_flags::WORD_FLAG_COUNT)> flags{ 0 };
    };

/** @brief Specialized version of word that does not use a stemmer at all.
    @details get_stem() is just included for compatibility and returns original string.
        Use this if stemming is not needed, as this will not store an extra string
        for the stem.*/
template<typename Tchar_traits>
class word<Tchar_traits, stemming::no_op_stem<std::basic_string<wchar_t, Tchar_traits>>>
    : public std::basic_string<wchar_t, Tchar_traits>
    {
  public:
    word(const wchar_t* word_start, const size_t word_length, const size_t sentence_index,
         const size_t sentence_position, const size_t paragraph_index,
         const bool is_date_time_money, const bool isInsideCompleteSentence,
         const bool isProperNoun, const bool isAcronym, const size_t syllable_count,
         const size_t punctuation_count)
        : std::basic_string<wchar_t, Tchar_traits>(word_start, word_length),
          m_sentence_index(sentence_index) /*sentence that the word is in*/,
          m_sentence_position(sentence_position) /*word's position in the sentence*/,
          m_paragraph_index(paragraph_index) /*paragraph that the word is in*/,
          m_syllable_count(syllable_count), m_punctuation_count(punctuation_count)
        {
        set_numeric(is_date_time_money);
        set_valid(isInsideCompleteSentence);
        set_proper_noun(isProperNoun);
        set_acronym(isAcronym);
        set_exclamatory(false);
        }

    word(const wchar_t* word_start, const size_t word_length)
        : std::basic_string<wchar_t, Tchar_traits>(word_start, word_length)
        {
        }

    explicit word(const wchar_t* word_start) : std::basic_string<wchar_t, Tchar_traits>(word_start)
        {
        }

    /// @private
    word() noexcept = default;

    [[nodiscard]]
    size_t get_length_excluding_punctuation() const noexcept
        {
        return std::basic_string<wchar_t, Tchar_traits>::length() - m_punctuation_count;
        }

    [[nodiscard]]
    size_t get_punctuation_count() const noexcept
        {
        return m_punctuation_count;
        }

    void set_syllable_count(const size_t count) noexcept { m_syllable_count = count; }

    [[nodiscard]]
    size_t get_syllable_count() const noexcept
        {
        return m_syllable_count;
        }

    [[nodiscard]]
    size_t get_sentence_index() const noexcept
        {
        return m_sentence_index;
        }

    [[nodiscard]]
    size_t get_paragraph_index() const noexcept
        {
        return m_paragraph_index;
        }

    [[nodiscard]]
    size_t get_sentence_position() const noexcept
        {
        return m_sentence_position;
        }

    [[nodiscard]]
    const wchar_t* get_stem() const
        {
        return std::basic_string<wchar_t, Tchar_traits>::c_str();
        }

    [[nodiscard]]
    bool is_capitalized() const
        {
        return (std::basic_string<wchar_t, Tchar_traits>::length() == 0) ?
                   false :
                   characters::is_character::is_upper(
                       std::basic_string<wchar_t, Tchar_traits>::operator[](0));
        }

    [[nodiscard]]
    bool is_capitalized_not_in_caps() const
        {
        return (std::basic_string<wchar_t, Tchar_traits>::length() == 0) ?
                   false :
               (std::basic_string<wchar_t, Tchar_traits>::length() == 1) ?
                   characters::is_character::is_upper(
                       std::basic_string<wchar_t, Tchar_traits>::operator[](0)) :
                   (characters::is_character::is_upper(
                        std::basic_string<wchar_t, Tchar_traits>::operator[](0)) &&
                    !characters::is_character::is_upper(
                        std::basic_string<wchar_t, Tchar_traits>::operator[](1)));
        }

    // flags
    void set_numeric(const bool enable)
        {
        flags.set(static_cast<size_t>(word_flags::numeric_flag), enable);
        }

    [[nodiscard]]
    bool is_numeric() const noexcept
        {
        return flags[static_cast<size_t>(word_flags::numeric_flag)];
        }

    void set_valid(const bool enable)
        {
        flags.set(static_cast<size_t>(word_flags::is_valid_flag), enable);
        }

    [[nodiscard]]
    bool is_valid() const noexcept
        {
        return flags[static_cast<size_t>(word_flags::is_valid_flag)];
        }

    void set_proper_noun(const bool enable)
        {
        flags.set(static_cast<size_t>(word_flags::proper_noun_flag), enable);
        }

    [[nodiscard]]
    bool is_proper_noun() const noexcept
        {
        return flags[static_cast<size_t>(word_flags::proper_noun_flag)];
        }

    void set_personal(const bool enable)
        {
        flags.set(static_cast<size_t>(word_flags::personal_flag), enable);
        }

    [[nodiscard]]
    bool is_personal() const
        {
        return flags[static_cast<size_t>(word_flags::personal_flag)];
        }

    void set_contraction(const bool enable)
        {
        flags.set(static_cast<size_t>(word_flags::contraction_flag), enable);
        }

    [[nodiscard]]
    bool is_contraction() const
        {
        return flags[static_cast<size_t>(word_flags::contraction_flag)];
        }

    void set_acronym(const bool enable)
        {
        flags.set(static_cast<size_t>(word_flags::acronym_flag), enable);
        }

    [[nodiscard]]
    bool is_acronym() const noexcept
        {
        return flags[static_cast<size_t>(word_flags::acronym_flag)];
        }

    void set_exclamatory(const bool enable)
        {
        flags.set(static_cast<size_t>(word_flags::exclamatory_flag), enable);
        }

    [[nodiscard]]
    bool is_exclamatory() const noexcept
        {
        return flags[static_cast<size_t>(word_flags::exclamatory_flag)];
        }

    void set_file_address(const bool enable)
        {
        flags.set(static_cast<size_t>(word_flags::file_address_flag), enable);
        }

    [[nodiscard]]
    bool is_file_address() const noexcept
        {
        return flags[static_cast<size_t>(word_flags::file_address_flag)];
        }

    void set_custom_tagged(const bool enable)
        {
        flags.set(static_cast<size_t>(word_flags::custom_tagged_flag), enable);
        }

    [[nodiscard]]
    bool is_custom_tagged() const
        {
        return flags[static_cast<size_t>(word_flags::custom_tagged_flag)];
        }

    void set_social_media_tag(const bool enable)
        {
        flags.set(static_cast<size_t>(word_flags::social_media_tag_flag), enable);
        }

    [[nodiscard]]
    bool is_social_media_tag() const
        {
        return flags[static_cast<size_t>(word_flags::social_media_tag_flag)];
        }

    void set_abbreviation_tag(const bool enable)
        {
        flags.set(static_cast<size_t>(word_flags::abbreviation_flag), enable);
        }

    [[nodiscard]]
    bool is_abbreviation_tag() const
        {
        return flags[static_cast<size_t>(word_flags::abbreviation_flag)];
        }

  private:
    size_t m_sentence_index{ 0 };
    size_t m_sentence_position{ 0 };
    size_t m_paragraph_index{ 0 };
    size_t m_syllable_count{ 0 };
    size_t m_punctuation_count{ 0 };

    // flags
    std::bitset<static_cast<size_t>(word_flags::WORD_FLAG_COUNT)> flags{ 0 };
    };

using word_case_insensitive_no_stem =
    word<traits::case_insensitive_ex,
         stemming::no_op_stem<std::basic_string<wchar_t, traits::case_insensitive_ex>>>;

#endif // INDEXING_WORD_H
