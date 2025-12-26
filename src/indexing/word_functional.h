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

#ifndef WORD_FUNCTIONAL_H
#define WORD_FUNCTIONAL_H

#include "../OleanderStemmingLibrary/src/stemming.h"
#include "../Wisteria-Dataviz/src/util/string_util.h"
#include "abbreviation.h"
#include "characters.h"
#include <set>

/** @brief Counting/Searching functor for `std::count_if` or `std::find`
        that counts punctuation marks, based on where punctuation mark is in the word.*/
template<typename Tpunctuation_type>
class punctuation_mark_count_if_word_position
    {
  public:
    /** @brief Constructor, which takes the position of the punctuation in the word.
        @param position The position of the punctuation in the word.*/
    explicit punctuation_mark_count_if_word_position(const size_t position) noexcept
        : m_word_position(position)
        {
        }

    punctuation_mark_count_if_word_position() = delete;

    /** @returns @c true if a valid punctuation mark based on its position in the word.
        @param punct The type of punctuation that this mark is.*/
    [[nodiscard]]
    bool operator()(const Tpunctuation_type& punct) const noexcept
        {
        return punct.get_word_position() == m_word_position;
        }

  private:
    size_t m_word_position{ 0 };
    };

/** @brief Counting functor for `std::count_if` that verifies the syllables in a
        word match a given count.*/
template<typename word_typeT>
class syllable_count_equals
    {
  public:
    /** @brief Constructor that takes the required syllable count and how to treat numerals.
        @param count The expected number of syllables that a passing word should have.
        @param treat_numerals_as_monosyllabic Whether numeric words should be seen as one
            syllable.\n
            Default is @c false.*/
    explicit syllable_count_equals(const size_t count,
                                   const bool treat_numerals_as_monosyllabic = false) noexcept
        : m_count(count), m_treat_numerals_as_monosyllabic(treat_numerals_as_monosyllabic)
        {
        }

    /// @private
    syllable_count_equals() = delete;

    /** @returns @c true if a word contains the predefined syllable count.
        @param the_word The word to review.*/
    [[nodiscard]]
    bool operator()(const word_typeT& the_word) const
        {
        return (m_treat_numerals_as_monosyllabic && the_word.is_numeric()) ?
                   (m_count == 1) :
                   the_word.get_syllable_count() == m_count;
        }

  private:
    size_t m_count{ 0 };
    bool m_treat_numerals_as_monosyllabic{ false };
    };

/** @brief Counting functor for `std::count_if` that verifies the syllables in a word match a given
   count.
    @details This always fails if the passed in word in invalid (i.e., excluded from the
   analysis).*/
template<typename word_typeT>
class valid_syllable_count_equals
    {
  public:
    /** @brief Constructor that takes the required syllable count and how to treat numerals.
        @param count The expected number of syllables that a passing word should have.
        @param treat_numerals_as_monosyllabic Whether numeric words should be seen
            as one syllable.\n
            Default is @c false.*/
    explicit valid_syllable_count_equals(size_t count,
                                         bool treat_numerals_as_monosyllabic = false) noexcept
        : m_count(count), m_treat_numerals_as_monosyllabic(treat_numerals_as_monosyllabic)
        {
        }

    valid_syllable_count_equals() = delete;

    /** @returns @c true if a word contains the predefined syllable count.\n
            Note that this will immediately return
        @c false if the word is invalid.
        @param the_word The word to review.*/
    [[nodiscard]]
    bool operator()(const word_typeT& the_word) const
        {
        if (the_word.is_valid())
            {
            return (m_treat_numerals_as_monosyllabic && the_word.is_numeric()) ?
                       (m_count == 1) :
                       the_word.get_syllable_count() == m_count;
            }

        return false;
        }

  private:
    size_t m_count{ 0 };
    bool m_treat_numerals_as_monosyllabic{ false };
    };

/** Counting functor for `std::count_if` that verifies the syllables in a word is
        greater than or equal to a given count.*/
template<typename word_typeT>
class syllable_count_greater_equal
    {
  public:
    /** @brief Constructor that takes the required syllable count and how to treat numerals.
        @param count The expected number of syllables that a passing word should have.
        @param treat_numerals_as_monosyllabic Whether numeric words should be seen as one
            syllable.\n
            Default is @c false.*/
    explicit syllable_count_greater_equal(const size_t count,
                                          bool treat_numerals_as_monosyllabic = false) noexcept
        : m_count(count), m_treat_numerals_as_monosyllabic(treat_numerals_as_monosyllabic)
        {
        }

    /// @private
    syllable_count_greater_equal() = delete;

    /** @returns @c true if a word contains a greater than or equal to number of syllables
            compared to the predefined syllable count.
        @param the_word The word to review.*/
    [[nodiscard]]
    bool operator()(const word_typeT& the_word) const
        {
        return (m_treat_numerals_as_monosyllabic && the_word.is_numeric()) ?
                   (1 >= m_count) :
                   the_word.get_syllable_count() >= m_count;
        }

  private:
    size_t m_count{ 0 };
    bool m_treat_numerals_as_monosyllabic{ false };
    };

/** Counting functor for std::count_if that verifies the syllables in a valid word is
        greater than or equal to a given count.*/
template<typename word_typeT>
class valid_syllable_count_greater_equal
    {
  public:
    /** @brief Constructor that takes the required syllable count and how to treat numerals.
        @param count The expected number of syllables that a passing word should have.
        @param treat_numerals_as_monosyllabic Whether numeric words should be seen as one
            syllable.\n
            Default is @c false.*/
    explicit valid_syllable_count_greater_equal(
        const size_t count, bool treat_numerals_as_monosyllabic = false) noexcept
        : m_count(count), m_treat_numerals_as_monosyllabic(treat_numerals_as_monosyllabic)
        {
        }

    /// @private
    valid_syllable_count_greater_equal() = delete;

    /** @returns @c true if a valid word contains a greater than or equal to number of syllables
            compared to the predefined syllable count.
        @param the_word The word to review.*/
    [[nodiscard]]
    bool operator()(const word_typeT& the_word) const
        {
        if (the_word.is_valid())
            {
            return (m_treat_numerals_as_monosyllabic && the_word.is_numeric()) ?
                       (1 >= m_count) :
                       the_word.get_syllable_count() >= m_count;
            }

        return false;
        }

  private:
    size_t m_count{ 0 };
    bool m_treat_numerals_as_monosyllabic{ false };
    };

/** Counting functor for `std::count_if` that verifies the syllables in a valid word is
        greater than or equal to a given count.\n
        Note that this ignores numerals.*/
template<typename word_typeT>
class syllable_count_greater_equal_ignore_numerals
    {
  public:
    /** @brief Constructor that takes the required syllable count and how to treat numerals.
        @param count The expected number of syllables that a passing word should have.*/
    explicit syllable_count_greater_equal_ignore_numerals(const size_t count) noexcept
        : m_count(count)
        {
        }

    /// @private
    syllable_count_greater_equal_ignore_numerals() = delete;

    /** @returns @c true if a word contains a greater than or equal to number of syllables
            compared to the predefined syllable count.
        @param the_word The word to review.*/
    [[nodiscard]]
    bool operator()(const word_typeT& the_word) const
        {
        return the_word.is_numeric() ? false : the_word.get_syllable_count() >= m_count;
        }

  private:
    size_t m_count{ 0 };
    };

/// use this when ignoring words from bullet points and headers
template<typename word_typeT>
class valid_syllable_count_greater_equal_ignore_numerals
    {
  public:
    explicit valid_syllable_count_greater_equal_ignore_numerals(const size_t count) noexcept
        : m_count(count)
        {
        }

    valid_syllable_count_greater_equal_ignore_numerals() = delete;

    [[nodiscard]]
    bool operator()(const word_typeT& the_word) const
        {
        if (the_word.is_valid())
            {
            return the_word.is_numeric() ? false : the_word.get_syllable_count() >= m_count;
            }

        return false;
        }

  private:
    size_t m_count{ 0 };
    };

//---------------------------
template<typename word_typeT>
class syllable_count_greater_equal_ignore_numerals_and_proper_nouns
    {
  public:
    explicit syllable_count_greater_equal_ignore_numerals_and_proper_nouns(
        const size_t count) noexcept
        : m_count(count)
        {
        }

    syllable_count_greater_equal_ignore_numerals_and_proper_nouns() = delete;

    [[nodiscard]]
    bool operator()(const word_typeT& the_word) const
        {
        return (the_word.is_numeric() || the_word.is_proper_noun()) ?
                   false :
                   the_word.get_syllable_count() >= m_count;
        }

  private:
    size_t m_count{ 0 };
    };

/// @brief Use this when ignoring words from bullet points and headers.
template<typename word_typeT>
class valid_syllable_count_greater_equal_ignore_numerals_and_proper_nouns
    {
  public:
    explicit valid_syllable_count_greater_equal_ignore_numerals_and_proper_nouns(
        const size_t count) noexcept
        : m_count(count)
        {
        }

    valid_syllable_count_greater_equal_ignore_numerals_and_proper_nouns() = delete;

    [[nodiscard]]
    bool operator()(const word_typeT& the_word) const
        {
        if (the_word.is_valid())
            {
            return (the_word.is_numeric() || the_word.is_proper_noun()) ?
                       false :
                       the_word.get_syllable_count() >= m_count;
            }

        return false;
        }

  private:
    size_t m_count{ 0 };
    };

//---------------------------
template<typename word_typeT>
class syllable_count_greater
    {
  public:
    explicit syllable_count_greater(size_t count,
                                    bool treat_numerals_as_monosyllabic = false) noexcept
        : m_count(count), m_treat_numerals_as_monosyllabic(treat_numerals_as_monosyllabic)
        {
        }

    syllable_count_greater() = delete;

    [[nodiscard]]
    bool operator()(const word_typeT& the_word) const
        {
        return (m_treat_numerals_as_monosyllabic && the_word.is_numeric()) ?
                   (1 > m_count) :
                   the_word.get_syllable_count() > m_count;
        }

  private:
    size_t m_count{ 0 };
    bool m_treat_numerals_as_monosyllabic{ false };
    };

/// @brief Use this when ignoring words from bullet points and headers.
template<typename word_typeT>
class valid_syllable_count_greater
    {
  public:
    explicit valid_syllable_count_greater(const size_t count,
                                          bool treat_numerals_as_monosyllabic = false) noexcept
        : m_count(count), m_treat_numerals_as_monosyllabic(treat_numerals_as_monosyllabic)
        {
        }

    valid_syllable_count_greater() = delete;

    [[nodiscard]]
    bool operator()(const word_typeT& the_word) const
        {
        if (the_word.is_valid())
            {
            return (m_treat_numerals_as_monosyllabic && the_word.is_numeric()) ?
                       (1 > m_count) :
                       the_word.get_syllable_count() > m_count;
            }

        return false;
        }

  private:
    size_t m_count{ 0 };
    bool m_treat_numerals_as_monosyllabic{ false };
    };

// used for std::accumulate
template<typename word_typeT>
class add_syllable_size
    {
  public:
    explicit add_syllable_size(const bool treat_numerals_as_monosyllabic = false) noexcept
        : m_treat_numerals_as_monosyllabic(treat_numerals_as_monosyllabic)
        {
        }

    add_syllable_size() = delete;

    [[nodiscard]]
    size_t operator()(const size_t result, const word_typeT& the_word) const
        {
        return static_cast<size_t>((m_treat_numerals_as_monosyllabic && the_word.is_numeric()) ?
                                       1 :
                                       the_word.get_syllable_count()) +
               result;
        }

  private:
    bool m_treat_numerals_as_monosyllabic{ false };
    };

/// use this when ignoring words from bullet points and headers
template<typename word_typeT>
class add_valid_syllable_size
    {
  public:
    explicit add_valid_syllable_size(const bool treat_numerals_as_monosyllabic = false) noexcept
        : m_treat_numerals_as_monosyllabic(treat_numerals_as_monosyllabic)
        {
        }

    add_valid_syllable_size() = delete;

    [[nodiscard]]
    size_t operator()(const size_t result, const word_typeT& the_word) const
        {
        if (the_word.is_valid())
            {
            return static_cast<size_t>((m_treat_numerals_as_monosyllabic && the_word.is_numeric()) ?
                                           1 :
                                           the_word.get_syllable_count()) +
                   result;
            }

        return result;
        }

  private:
    bool m_treat_numerals_as_monosyllabic{ false };
    };

template<typename word_typeT>
class add_syllable_size_ignore_numerals
    {
  public:
    [[nodiscard]]
    size_t operator()(const size_t result, const word_typeT& the_word) const
        {
        return static_cast<size_t>(the_word.is_numeric() ? 0 : the_word.get_syllable_count()) +
               result;
        }
    };

/// @brief Use this when ignoring words from bullet points and headers.
template<typename word_typeT>
class add_valid_syllable_size_ignore_numerals
    {
  public:
    [[nodiscard]]
    size_t operator()(size_t result, const word_typeT& the_word) const
        {
        if (the_word.is_valid())
            {
            return static_cast<size_t>(the_word.is_numeric() ? 0 : the_word.get_syllable_count()) +
                   result;
            }

        return result;
        }
    };

template<typename word_typeT>
class add_syllable_size_ignore_numerals_and_proper_nouns
    {
  public:
    [[nodiscard]]
    size_t operator()(const size_t result, const word_typeT& the_word) const
        {
        return static_cast<size_t>((the_word.is_numeric() || the_word.is_proper_noun()) ?
                                       0 :
                                       the_word.get_syllable_count()) +
               result;
        }
    };

/// @brief Use this when ignoring words from bullet points and headers.
template<typename word_typeT>
class add_valid_syllable_size_ignore_numerals_and_proper_nouns
    {
  public:
    [[nodiscard]]
    size_t operator()(const size_t result, const word_typeT& the_word) const
        {
        if (the_word.is_valid())
            {
            return static_cast<size_t>((the_word.is_numeric() || the_word.is_proper_noun()) ?
                                           0 :
                                           the_word.get_syllable_count()) +
                   result;
            }

        return result;
        }
    };

/** Proper nouns functor used for `std::count_if`.
    Use this when ignoring words from bullet points and headers.*/
template<typename word_typeT>
class is_proper_noun
    {
  public:
    is_proper_noun() = default;

    /** @brief Reviews a word to see if it is proper.
        @param the_word The word to review.
        @returns @c true if @c the_word is proper.*/
    [[nodiscard]]
    bool operator()(const word_typeT& the_word) const noexcept
        {
        return the_word.is_proper_noun();
        }
    };

/** Proper nouns functor used for `std::count_if`.
    Use this when ignoring words from bullet points and headers.*/
template<typename word_typeT>
class is_valid_proper_noun
    {
  public:
    is_valid_proper_noun() = default;

    /* @returns @c true if a word is proper.
       @param the_word The word to review.*/
    [[nodiscard]]
    bool operator()(const word_typeT& the_word) const noexcept
        {
        if (the_word.is_valid())
            {
            return the_word.is_proper_noun();
            }

        return false;
        }
    };

/** @brief Numerals functors used for `std::count_if`.*/
template<typename word_typeT>
class is_numeric
    {
  public:
    is_numeric() = default;

    /** @returns @c true if a word is numeric.
        @param the_word The word to review.*/
    [[nodiscard]]
    bool operator()(const word_typeT& the_word) const noexcept
        {
        return the_word.is_numeric();
        }
    };

/** @brief Numeric word functor used for `std::count_if`.
    @details Use this when ignoring words from bullet points and headers.*/
template<typename word_typeT>
class is_valid_numeric
    {
  public:
    is_valid_numeric() = default;

    /** @returns @c true if a word is numeric.
        @param the_word The word to review.*/
    [[nodiscard]]
    bool operator()(const word_typeT& the_word) const noexcept
        {
        if (the_word.is_valid())
            {
            return the_word.is_numeric();
            }

        return false;
        }
    };

/** @brief Word length comparison functor, which includes internal punctuation marks.
    @details Used for `std::count_if`.*/
template<typename word_typeT>
class word_length_equals
    {
  public:
    /** @brief Constructor
        @param count The size that word lengths will be expected to be.*/
    explicit word_length_equals(const size_t count) noexcept : m_count(count) {}

    word_length_equals() = delete;

    /** @returns @c true if words are the same length.
        @param the_word The word to review.*/
    [[nodiscard]]
    bool operator()(const word_typeT& the_word) const noexcept
        {
        return the_word.length() == m_count;
        }

  private:
    size_t m_count{ 0 };
    };

/** @brief Word length comparison functor for valid words, which includes internal punctuation
   marks.
    @details Used for `std::count_if`.*/
template<typename word_typeT>
class valid_word_length_equals
    {
  public:
    /** @brief Constructor
        @param count The word size to compare against.*/
    explicit valid_word_length_equals(const size_t count) noexcept : m_count(count) {}

    valid_word_length_equals() = delete;

    /** @returns @c true if a word is valid and its length equals our specified number.
        @param the_word The word to review.*/
    [[nodiscard]]
    bool operator()(const word_typeT& the_word) const noexcept
        {
        if (the_word.is_valid())
            {
            return the_word.length() == m_count;
            }

        return false;
        }

  private:
    size_t m_count{ 0 };
    };

/** @brief >= comparison of word lengths from valid words, including their internal punctuation
   marks.
    @details Used for `std::count_if`.*/
template<typename word_typeT>
class word_length_greater_equals
    {
  public:
    /** @brief Constructor.
        @param count The word size to compare against.*/
    explicit word_length_greater_equals(const size_t count) noexcept : m_count(count) {}

    word_length_greater_equals() = delete;

    /** @returns @c true if a word is valid and its length is greater than or equals our specified
       number.
        @param the_word The word to review.
        @returns @c true if current word's length is greater than or equal to the seeded length.*/
    [[nodiscard]]
    bool operator()(const word_typeT& the_word) const noexcept
        {
        return the_word.length() >= m_count;
        }

  private:
    size_t m_count{ 0 };
    };

/** @brief >= comparison of word lengths from  words, excluding their internal punctuation marks.
    @details Used for `std::count_if`.*/
template<typename word_typeT>
class word_length_excluding_punctuation_greater_equals
    {
  public:
    /** @brief Constructor
        @param count The word size to compare against.*/
    explicit word_length_excluding_punctuation_greater_equals(const size_t count) noexcept
        : m_count(count)
        {
        }

    word_length_excluding_punctuation_greater_equals() = delete;

    /** @brief Comparison operator.
        @param the_word The word to review.
        @returns @c true if current word's length is greater than or equal to the
            seeded length (ignoring punctuation in the word)*/
    [[nodiscard]]
    bool operator()(const word_typeT& the_word) const noexcept
        {
        return the_word.get_length_excluding_punctuation() >= m_count;
        }

  private:
    size_t m_count{ 0 };
    };

/** @brief >= comparison of word lengths from valid words, including their
        internal punctuation marks.
    @details Used for std::count_if. Use this when ignoring words from bullet points and headers.*/
template<typename word_typeT>
class valid_word_length_greater_equals
    {
  public:
    /** @brief Constructor.
        @param count The word size to compare against.*/
    explicit valid_word_length_greater_equals(const size_t count) noexcept : m_count(count) {}

    valid_word_length_greater_equals() = delete;

    /** @brief Comparison operator.
        @param the_word The word to review.
        @returns @c true if current word's length is greater than or equal to the
            seeded length (if word is valid).*/
    [[nodiscard]]
    bool operator()(const word_typeT& the_word) const noexcept
        {
        if (the_word.is_valid())
            {
            return the_word.length() >= m_count;
            }

        return false;
        }

  private:
    size_t m_count{ 0 };
    };

/** @brief >= comparison of word lengths from valid words, excluding their
        internal punctuation marks.
    @details Used for std::count_if. Use this when ignoring words from bullet points and headers.*/
template<typename word_typeT>
class valid_word_length_excluding_punctuation_greater_equals
    {
  public:
    /** @brief Constructor.
        @param count The word size to compare against.*/
    explicit valid_word_length_excluding_punctuation_greater_equals(const size_t count) noexcept
        : m_count(count)
        {
        }

    valid_word_length_excluding_punctuation_greater_equals() = delete;

    /** @brief Comparison operator.
        @param the_word The word to review.
        @returns @c true if current word's length is greater than or equal to
            the seeded length (ignoring punctuation in the word, and if word is valid).*/
    [[nodiscard]]
    bool operator()(const word_typeT& the_word) const noexcept
        {
        if (the_word.is_valid())
            {
            return the_word.get_length_excluding_punctuation() >= m_count;
            }

        return false;
        }

  private:
    size_t m_count{ 0 };
    };

/** @brief Accumulates word lengths from words, including their internal punctuation marks.
    @details Used for `std::accumulate`.*/
template<typename word_typeT>
class add_word_size
    {
  public:
    /** @brief Accumulation interface.
        @param result The previous result to add to.
        @param the_word The word whose length will be added to the result.
        @returns The seeded result plus the current word's length.*/
    [[nodiscard]]
    size_t operator()(const size_t result, const word_typeT& the_word) const noexcept
        {
        return static_cast<size_t>(the_word.length()) + result;
        }
    };

/** @brief Accumulates word lengths from words, excluding their internal punctuation marks.*/
template<typename word_typeT>
class add_word_size_excluding_punctuation
    {
  public:
    /** @brief Accumulation interface.
        @param result The previous result to add to.
        @param the_word The word whose length will be added to the result.
        @returns The seeded result plus the current word's length
            (excluding any punctuation in the word).*/
    [[nodiscard]]
    size_t operator()(const size_t result, const word_typeT& the_word) const
        {
        return static_cast<size_t>(the_word.get_length_excluding_punctuation()) + result;
        }
    };

/** @brief Accumulates word lengths from valid words, including their internal punctuation marks.*/
template<typename word_typeT>
class add_valid_word_size
    {
  public:
    /** @brief Accumulation interface.
        @param result The previous result to add to.
        @param the_word The word whose length will be added to the result.
        @returns The seeded result plus the current word's length (if valid).*/
    [[nodiscard]]
    size_t operator()(const size_t result, const word_typeT& the_word) const
        {
        if (the_word.is_valid())
            {
            return static_cast<size_t>(the_word.length()) + result;
            }

        return result;
        }
    };

/** Accumulates word lengths from valid words, excluding their internal punctuation marks.
    Use this when ignoring words from bullet points and headers.*/
template<typename word_typeT>
class add_valid_word_size_excluding_punctuation
    {
  public:
    /** @brief Accumulation interface.
        @param result The previous result to add to.
        @param the_word The word whose length will be added to the result.
        @returns The seeded result plus the current word's length
            (if valid, and excluding any punctuation in the word).*/
    [[nodiscard]]
    size_t operator()(const size_t result, const word_typeT& the_word) const
        {
        if (the_word.is_valid())
            {
            return static_cast<size_t>(the_word.get_length_excluding_punctuation()) + result;
            }

        return result;
        }
    };

/** Determines if a word is a social media tag (i.e., a "hashtag" or "at" handle).*/
template<typename word_typeT>
class is_social_media_tag
    {
  public:
    /** @brief Main interface for determining if a word is a hashtag.
        @param the_word The word to review.
        @returns @c true if a hashtag.*/
    [[nodiscard]]
    bool operator()(const word_typeT& the_word) const
        {
        // start with '#' or '@', but not a number like "#12".
        return (!the_word.is_numeric() && the_word.get_length_excluding_punctuation() > 4 &&
                (the_word[0] == L'#' || the_word[0] == L'@') &&
                characters::is_character::is_alpha(the_word[1]));
        }
    };

class is_file_extension
    {
  public:
    /// @returns @c true if string is a known file extension.
    /// @param str The string to review.
    [[nodiscard]]
    static bool is_extension(const traits::case_insensitive_wstring_ex& str)
        {
        return m_file_extensions.contains(str);
        }

  private:
    static std::set<traits::case_insensitive_wstring_ex> m_file_extensions;
    };

/** @brief Used for spell checking.
    @details Performs a binary search through a list of known words to see if the provided
        value is in there. Also takes into account whether the word is
        proper, numeric, or uppercased.*/
template<typename word_typeT, typename wordlistT>
class is_correctly_spelled_word
    {
  public:
    /** @brief Constructor.
        @param wlist The list of known words.
        @param secondary_wlist A user-defined list of known words.
        @param programmer_wlist A programming word list (only used by is_programmer_code())
        @param should_ignore_proper_nouns Whether proper nouns are checked or not.
        @param should_ignore_uppercased Whether all uppercased words are checked or not.
        @param should_ignore_numerals Whether numerals are checked or not.
        @param should_ignore_file_addresses Whether file addresses are checked or not.
        @param should_ignore_programmer_code Whether programmer code is checked or not.
        @param should_allow_colloquialisms Whether to ignore things like "in'" replacing "ing".
        @param should_ignore_social_media_tags Whether to ignore things like `#Flyers`.*/
    is_correctly_spelled_word(
        const wordlistT* wlist, const wordlistT* secondary_wlist, const wordlistT* programmer_wlist,
        const bool should_ignore_proper_nouns, const bool should_ignore_uppercased,
        const bool should_ignore_numerals, const bool should_ignore_file_addresses,
        const bool should_ignore_programmer_code, const bool should_allow_colloquialisms,
        const bool should_ignore_social_media_tags) noexcept
        : m_wordlist(wlist), m_secondary_wordlist(secondary_wlist),
          m_programmer_wordlist(programmer_wlist),
          m_ignore_proper_nouns(should_ignore_proper_nouns),
          m_ignore_uppercased(should_ignore_uppercased), m_ignore_numerals(should_ignore_numerals),
          m_ignore_file_addresses(should_ignore_file_addresses),
          m_ignore_programmer_code(should_ignore_programmer_code),
          m_allow_colloquialisms(should_allow_colloquialisms),
          m_ignore_social_media_tags(should_ignore_social_media_tags)
        {
        }

    is_correctly_spelled_word() = default;

    /** @brief Sets the list of known (spelled correctly) words.
        @param wlist The list of known words.*/
    void set_word_list(const wordlistT* wlist) noexcept { m_wordlist = wlist; }

    /// @returns The list of known (spelled correctly) words.
    [[nodiscard]]
    const wordlistT* get_word_list() const noexcept
        {
        return m_wordlist;
        }

    /** @brief Sets the user-defined list of known words.
        @param wlist The list of user-defined known words.*/
    void set_secondary_word_list(const wordlistT* wlist) noexcept { m_secondary_wordlist = wlist; }

    /// @returns The list of user-defined known (spelled correctly) words.
    [[nodiscard]]
    const wordlistT* get_secondary_word_list() const noexcept
        {
        return m_secondary_wordlist;
        }

    /** @brief Sets the list of known programming words.
        @param wlist The list of programming known words.*/
    void set_programmer_word_list(const wordlistT* wlist) noexcept
        {
        m_programmer_wordlist = wlist;
        }

    /// @returns The list of known (spelled correctly) programming words.
    [[nodiscard]]
    const wordlistT* get_programmer_word_list() const noexcept
        {
        return m_programmer_wordlist;
        }

    /** @returns Whether proper nouns are checked or not. If not, then
            proper nouns will always be considered spelled correctly.*/
    [[nodiscard]]
    bool is_ignoring_proper_nouns() const noexcept
        {
        return m_ignore_proper_nouns;
        }

    /** @brief Set whether proper nouns are checked or not. If not, then
            proper nouns will always be considered spelled correctly.
        @param ignore Whether proper nouns should be checked.*/
    void ignore_proper_nouns(const bool ignore) noexcept { m_ignore_proper_nouns = ignore; }

    /** @returns Whether all uppercased words are checked or not. If not, then
            uppercased words will always be considered spelled correctly.*/
    [[nodiscard]]
    bool is_ignoring_uppercased() const noexcept
        {
        return m_ignore_uppercased;
        }

    /** @brief Set whether all uppercased words are checked or not. If not, then
            uppercased words will always be considered spelled correctly.
        @param ignore Whether uppercased words should be checked.*/
    void ignore_uppercased(const bool ignore) noexcept { m_ignore_uppercased = ignore; }

    /** @returns Whether numerals are checked or not. If not, then
            numerals will always be considered spelled correctly.*/
    [[nodiscard]]
    bool is_ignoring_numerals() const noexcept
        {
        return m_ignore_numerals;
        }

    /** @brief Set whether numerals are checked or not. If not, then
            numerals will always be considered spelled correctly.
        @param ignore Whether numerals should be checked.*/
    void ignore_numerals(const bool ignore) noexcept { m_ignore_numerals = ignore; }

    /** @returns Whether file addresses are checked or not. If not, then
            file addresses will always be considered spelled correctly.*/
    [[nodiscard]]
    bool is_ignoring_file_addresses() const noexcept
        {
        return m_ignore_file_addresses;
        }

    /** @brief Set whether file addresses are checked or not. If not, then
            file addresses will always be considered spelled correctly.
        @param ignore Whether file addresses should be checked.*/
    void ignore_file_addresses(const bool ignore) noexcept { m_ignore_file_addresses = ignore; }

    /** @returns Whether programmer code are checked or not. If not, then
            programmer code will always be considered spelled correctly.*/
    [[nodiscard]]
    bool is_ignoring_programmer_code() const noexcept
        {
        return m_ignore_programmer_code;
        }

    /** @brief Set whether programmer code are checked or not. If not, then
            programmer code will always be considered spelled correctly.
        @param ignore Whether programmer code should be checked.*/
    void ignore_programmer_code(const bool ignore) noexcept { m_ignore_programmer_code = ignore; }

    /** @returns Whether colloquialisms are acceptable. If so, then things like
            "tryin'" would be considered spelled correctly.*/
    [[nodiscard]]
    bool is_allowing_colloquialisms() const noexcept
        {
        return m_allow_colloquialisms;
        }

    /** Set whether colloquialisms are checked or not. If so, then things like
            "tryin'" will always be considered spelled correctly.
        @param allow Whether colloquialisms should be considered acceptable spelling.*/
    void allow_colloquialisms(const bool allow) noexcept { m_allow_colloquialisms = allow; }

    /** @returns Whether hashtagged words are being ignored.*/
    [[nodiscard]]
    bool is_ignoring_social_media_tags() const noexcept
        {
        return m_ignore_social_media_tags;
        }

    /** @brief Set whether hashtagged words should be ignored.
        @param ignore Whether to ignore hashtagged words.*/
    void ignore_social_media_tags(const bool ignore) noexcept
        {
        m_ignore_social_media_tags = ignore;
        }

    /** @brief Main interface for determining if a word is spelled correctly.
        @param the_word The word to review.
        @returns @c true if spelled correctly.*/
    [[nodiscard]]
    bool operator()(const word_typeT& the_word) const
        {
        // clang-format off
        if ((is_ignoring_numerals() && the_word.is_numeric()) || // see if word is a number
                                                                 // uppercased words
            (is_ignoring_uppercased() && (the_word.is_acronym() || the_word.is_exclamatory())) ||
            // file address
            (is_ignoring_file_addresses() &&
             (the_word.is_file_address() ||
              is_file_extension::is_extension(the_word.c_str()))) ||
            // hashtags
            (is_ignoring_social_media_tags() && the_word.is_social_media_tag()) ||
            // proper noun
            (is_ignoring_proper_nouns() && the_word.is_proper_noun()) ||
            // or programmer code
            (is_ignoring_programmer_code() && is_programmer_code(the_word)) ||
            // or colloquialisms
            (is_allowing_colloquialisms() && is_colloquialisms(the_word)) ||
            // if initials (or dotted acronym) then always consider it spelled properly
            ((the_word.is_exclamatory() || the_word.is_acronym()) &&
             ((the_word.length() == 2 && the_word.operator[](1) == common_lang_constants::PERIOD) ||
              (the_word.length() >= 4 && the_word.operator[](1) == common_lang_constants::PERIOD &&
               the_word.operator[](3) == common_lang_constants::PERIOD))))
            {
            return true;
            }
        return is_on_list(the_word);
        // clang-format on
        }

  private:
    [[nodiscard]]
    bool is_colloquialisms(const word_typeT& the_word) const
        {
        typename wordlistT::word_type alteredWord = the_word.c_str();
        // strip off "'s" to make things simple
        if (alteredWord.length() >= 3 &&
            traits::case_insensitive_ex::eq(alteredWord[alteredWord.length() - 1], L's') &&
            characters::is_character::is_single_quote(alteredWord[alteredWord.length() - 2]))
            {
            alteredWord.erase(alteredWord.length() - 2);
            }
        if (alteredWord.length() < 2)
            {
            return false;
            }
        // Condense triple (or even longer) same-letter sequences.
        // This will make "Nooooooooo!" look like "No!".
        size_t currentLetterSequenceLength = 1;
        for (size_t i = 0; i < alteredWord.length() - 1; ++i)
            {
            if (traits::case_insensitive_ex::eq(alteredWord[i], alteredWord[i + 1]))
                {
                ++currentLetterSequenceLength;
                }
            else
                {
                if (currentLetterSequenceLength >= 3)
                    {
                    alteredWord.erase(i - (currentLetterSequenceLength - 1),
                                      (currentLetterSequenceLength - 1));
                    }
                currentLetterSequenceLength = 1;
                }
            }
        if (currentLetterSequenceLength >= 3)
            {
            alteredWord.erase(alteredWord.length() - (currentLetterSequenceLength - 1),
                              (currentLetterSequenceLength - 1));
            }
        //"ing" -> "in'"
        if (alteredWord.length() >= 4 &&
            characters::is_character::is_single_quote(alteredWord[alteredWord.length() - 1]) &&
            traits::case_insensitive_ex::eq(alteredWord[alteredWord.length() - 2], L'n') &&
            traits::case_insensitive_ex::eq(alteredWord[alteredWord.length() - 3], L'i'))
            {
            alteredWord[alteredWord.length() - 1] = L'g';
            }
        //"er" -> "a"
        else if (alteredWord.length() >= 4 &&
                 traits::case_insensitive_ex::eq(alteredWord[alteredWord.length() - 1], L'a') &&
                 characters::is_character::is_consonant(alteredWord[alteredWord.length() - 2]) &&
                 characters::is_character::is_consonant(alteredWord[alteredWord.length() - 3]) &&
                 characters::is_character::is_vowel(alteredWord[alteredWord.length() - 4]) &&
                 !is_on_list(alteredWord))
            {
            alteredWord.replace(alteredWord.length() - 1, 1, L"er");
            }
        else if (alteredWord.length() >= 5 &&
                 traits::case_insensitive_ex::eq(alteredWord[alteredWord.length() - 1], L's') &&
                 traits::case_insensitive_ex::eq(alteredWord[alteredWord.length() - 2], L'a') &&
                 characters::is_character::is_consonant(alteredWord[alteredWord.length() - 3]) &&
                 characters::is_character::is_consonant(alteredWord[alteredWord.length() - 4]) &&
                 characters::is_character::is_vowel(alteredWord[alteredWord.length() - 5]) &&
                 !is_on_list(alteredWord))
            {
            alteredWord.replace(alteredWord.length() - 2, 2, L"ers");
            }
        // "player" -> "playa"
        else if (alteredWord.length() >= 4 &&
                 traits::case_insensitive_ex::eq(alteredWord[alteredWord.length() - 1], L'a') &&
                 characters::is_character::is_either<wchar_t>(alteredWord[alteredWord.length() - 2],
                                                              L'y', L'Y') &&
                 characters::is_character::is_vowel(alteredWord[alteredWord.length() - 3]) &&
                 !is_on_list(alteredWord))
            {
            alteredWord.replace(alteredWord.length() - 1, 1, L"er");
            }
        // "players" -> "playas"
        else if (alteredWord.length() >= 5 &&
                 traits::case_insensitive_ex::eq(alteredWord[alteredWord.length() - 1], L's') &&
                 traits::case_insensitive_ex::eq(alteredWord[alteredWord.length() - 2], L'a') &&
                 characters::is_character::is_either<wchar_t>(alteredWord[alteredWord.length() - 3],
                                                              L'y', L'Y') &&
                 characters::is_character::is_vowel(alteredWord[alteredWord.length() - 4]) &&
                 !is_on_list(alteredWord))
            {
            alteredWord.replace(alteredWord.length() - 2, 2, L"ers");
            }
        return is_on_list(alteredWord);
        }

    /// @returns @c true if @c the_word looks like computer programmer code that a
    ///     spell checker might want to skip.
    /// @param the_word The word to review.
    /// @note This function does not narrow full-width characters,
    ///     it assumes programmer code is ASCII text.\n
    ///     It is also sensitive to casing, unlike other functions in the spell checker.
    [[nodiscard]]
    bool is_programmer_code(const word_typeT& the_word) const
        {
        // Single uppercase word (not in a block of other uppercased words)
        // is probably a constant or macro command.
        // Ignore numbers explicitly too.
        if (the_word.is_acronym() || the_word.is_numeric())
            {
            return true;
            }
        // ignore UI ampersand with escaping ampersand in front of it.
        if (the_word.length() == 2 && the_word[0] == L'&' && the_word[1] == L'&')
            {
            return true;
            }
        // a hexadecimal string
        if (the_word.length() >= 3 && the_word[0] == L'0' &&
            string_util::is_either(the_word[1], L'x', L'X') &&
            string_util::is_hex_digit(the_word[2]))
            {
            return true;
            }
        // a possible printf command (e.g., "%d")
        if (the_word.length() == 2 && the_word[0] == L'%' &&
            characters::is_character::is_alpha(the_word[1]))
            {
            return true;
            }
        // C/C++ preprocessor commands start with '#'
        // Visual Basic 6 variables can start with '$'
        // Doxygen commands start with '@'
        // C/C++ reference var may have '&' in front
        if (the_word.length() > 1 && string_util::is_one_of(the_word[0], L"#$@&"))
            {
            return true;
            }
        // at this point, if less than 3 characters then we don't have
        // any text to make a determination, then return false
        if (the_word.length() < 3)
            {
            return false;
            }
        // words ending with a number are usually a variable name (e.g., "Text1").
        if (characters::is_character::is_numeric(the_word[the_word.length() - 1]))
            {
            return true;
            }
        // if it contains an underscore (common for variables),
        // percent (like a printf command), or accessors
        if (the_word.find(L'_') != word_typeT::npos || the_word.find(L'%') != word_typeT::npos ||
            // C++ accessors
            the_word.find(L"::") != word_typeT::npos || the_word.find(L"->") != word_typeT::npos)
            {
            return true;
            }
        // first two letters in caps is probably a variable (e.g., "PValue")
        if (characters::is_character::is_upper(the_word[0]) &&
            characters::is_character::is_upper(the_word[1]))
            {
            return true;
            }

        // see if the word has a [lowercase][UCASE], [letter][number],
        // [letter/number][:.][letter], [letter]-[UCASE][UCASE] pattern in it.
        for (size_t i = 0; i < the_word.length() - 1; ++i)
            {
            if (characters::is_character::is_lower(the_word.operator[](i)) &&
                characters::is_character::is_upper(the_word.operator[](i + 1)))
                {
                return true;
                }
            if (characters::is_character::is_alpha(the_word.operator[](i)) &&
                string_util::is_numeric_8bit(the_word.operator[](i + 1)))
                {
                return true;
                }
            if (the_word.length() >= 3 && i < the_word.length() - 2 &&
                (characters::is_character::is_alpha(the_word.operator[](i)) ||
                 characters::is_character::is_numeric(the_word.operator[](i))) &&
                characters::is_character::is_either<wchar_t>(the_word.operator[](i + 1), L'.',
                                                             L':') &&
                characters::is_character::is_alpha(the_word.operator[](i + 2)))
                {
                return true;
                }
            if (the_word.length() >= 4 && i < the_word.length() - 3 &&
                characters::is_character::is_alpha(the_word.operator[](i)) &&
                the_word.operator[](i + 1) == L'-' &&
                characters::is_character::is_upper(the_word.operator[](i + 2)) &&
                characters::is_character::is_upper(the_word.operator[](i + 3)))
                {
                return true;
                }
            }
        // is the word in our custom list of programming-specific words?
        typename wordlistT::word_type strippedWord{ the_word.c_str() };
        if (m_programmer_wordlist &&
            std::binary_search(m_programmer_wordlist->get_words().begin(),
                               m_programmer_wordlist->get_words().end(), strippedWord))
            {
            return true;
            }
        // remove ampersands in case this is UI string with accelerator keys in it
        string_util::remove_all(strippedWord, L'&');
        // ...strip trailing "(.?!)\(nrt)"
        if (strippedWord.length() > 3 &&
            (strippedWord[strippedWord.length() - 3] == L'.' ||
             strippedWord[strippedWord.length() - 3] == L'?' ||
             strippedWord[strippedWord.length() - 3] == L'!') &&
            strippedWord[strippedWord.length() - 2] == L'\\' &&
            (strippedWord[strippedWord.length() - 1] == L'n' ||
             strippedWord[strippedWord.length() - 1] == L'r' ||
             strippedWord[strippedWord.length() - 1] == L't'))
            {
            strippedWord.erase(strippedWord.length() - 3);
            }
        // ...then, also change any embedded "\n" in the string to a dash
        // (is_on_list() will tokenize it into smaller words),
        // and then return whether it is a known word or not.
        string_util::replace_all(strippedWord, L"\\n", 2, L"-");
        return is_on_list(strippedWord);
        }

    /// @returns @c true if the given string is on either of the supplied lists.
    /// @param the_word The word to search for.
    template<typename T>
    [[nodiscard]]
    bool is_on_list(const T& the_word) const
        {
        if (m_wordlist == nullptr || m_secondary_wordlist == nullptr || the_word.length() == 0)
            {
            return false;
            } // no list or word is blank, then don't bother looking

        // first, see if the full word is already in our dictionaries
        typename wordlistT::word_type compValue{ the_word.c_str() };
        if (std::binary_search(m_wordlist->get_words().begin(), m_wordlist->get_words().end(),
                               compValue) ||
            std::binary_search(m_secondary_wordlist->get_words().begin(),
                               m_secondary_wordlist->get_words().end(), compValue))
            {
            return true;
            }

        // see if the word is a hyphenated (or slashed) compound word
        string_util::string_tokenize<T> tkzr(
            the_word, common_lang_constants::COMPOUND_WORD_SEPARATORS.c_str(), true);
        if (tkzr.has_more_delimiters()) // don't bother splitting word into tokens if no delimiters
            {
            // makes sure that there is at least one valid block of text in the string
            bool validTokenFound = false;
            while (tkzr.has_more_tokens())
                {
                compValue = tkzr.get_next_token().c_str();
                // if at least one chunk of text around a '-' is real, then set this flag to true
                if (compValue.length() > 0)
                    {
                    validTokenFound = true;
                    }
                // see if this chunk is just a number
                if (characters::is_character::is_numeric({ compValue.c_str(), compValue.length() }))
                    {
                    continue;
                    }
                // if ignoring uppercase, then see if this part of the word is an acronym
                if (is_ignoring_uppercased() &&
                    isAcronym({ compValue.c_str(), compValue.length() }))
                    {
                    continue;
                    }
                // in case we have something like "one-", then the fact that there is no second word
                // after the '-' shouldn't make it unfamiliar
                if (compValue.length() > 0 &&
                    !std::binary_search(m_wordlist->get_words().begin(),
                                        m_wordlist->get_words().end(), compValue) &&
                    !std::binary_search(m_secondary_wordlist->get_words().begin(),
                                        m_secondary_wordlist->get_words().end(), compValue))
                    {
                    return false;
                    }
                }
            // true if at least one token was found and all tokens found were on our lists
            return validTokenFound;
            }
        return false;
        }

    const wordlistT* m_wordlist{ nullptr };
    // a user-supplied word list
    const wordlistT* m_secondary_wordlist{ nullptr };
    // a word list of programmer words (only used by is_programmer_code())
    const wordlistT* m_programmer_wordlist{ nullptr };
    bool m_ignore_proper_nouns{ false };
    bool m_ignore_uppercased{ true };
    bool m_ignore_numerals{ true };
    bool m_ignore_file_addresses{ true };
    bool m_ignore_programmer_code{ false };
    bool m_allow_colloquialisms{ true };
    bool m_ignore_social_media_tags{ true };
    grammar::is_acronym isAcronym;
    };

/** @brief Calls a member function of elements in a container for each
        element in another container.*/
template<typename inT, typename outT, typename member_extract_functorT>
outT copy_member(inT begin, inT end, outT dest, member_extract_functorT get_value)
    {
    for (; begin != end; ++dest, ++begin)
        {
        *dest = get_value(*begin);
        }
    return (dest);
    }

/// @brief Copies a member value between objects based on specified criteria.
template<typename inT, typename outT, typename Pr, typename member_extract_functorT>
outT copy_member_if(inT begin, inT end, outT dest, Pr meets_criteria,
                    member_extract_functorT get_value)
    {
    for (; begin != end; ++begin)
        {
        if (meets_criteria(*begin))
            {
            *dest = get_value(*begin);
            ++dest;
            }
        }
    return (dest);
    }

#endif // WORD_FUNCTIONAL_H
