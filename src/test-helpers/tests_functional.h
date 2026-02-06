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

#ifndef TESTS_FUNCTIONAL_H
#define TESTS_FUNCTIONAL_H

#include "../indexing/word_collection.h"
#include "../indexing/word_functional.h"
#include "../readability/custom_readability_test.h"
#include "../readability/dolch.h"
#include "../readability/readability.h"
#include <wx/string.h>

using CaseInSensitiveNonStemmingDocument = document<word_case_insensitive_no_stem>;

using CustomReadabilityTest = readability::custom_test<word_case_insensitive_no_stem>;

template<typename word_typeT>
class SyllableCountGreaterEqualWithHighlighting final
    : public syllable_count_greater_equal<word_typeT>
    {
  public:
    SyllableCountGreaterEqualWithHighlighting(size_t count, bool treat_numerals_as_monosyllabic,
                                              wxString beginHighlight, wxString endHighlight)
        : syllable_count_greater_equal<word_typeT>(count, treat_numerals_as_monosyllabic),
          m_beginHighlight(std::move(beginHighlight)), m_endHighlight(std::move(endHighlight))
        {
        }

    /// @private
    SyllableCountGreaterEqualWithHighlighting() = delete;

    /// @returns The tag opening a highlighted section.
    [[nodiscard]]
    const wxString& GetHighlightBegin() const noexcept
        {
        return m_beginHighlight;
        }

    /// @returns The tag closing a highlighted section.
    [[nodiscard]]
    const wxString& GetHighlightEnd() const noexcept
        {
        return m_endHighlight;
        }

    /// @internal No-op, just needed for interface contract.
    /// @private
    // cppcheck-suppress functionStatic
    void Reset() {}

  private:
    wxString m_beginHighlight;
    wxString m_endHighlight;
    };

template<typename word_typeT>
class WordLengthGreaterEqualsWithHighlighting final
    : public word_length_excluding_punctuation_greater_equals<word_typeT>
    {
  public:
    WordLengthGreaterEqualsWithHighlighting(const size_t wordLength, wxString beginHighlight,
                                            wxString endHighlight)
        : word_length_excluding_punctuation_greater_equals<word_typeT>(wordLength),
          m_beginHighlight(std::move(beginHighlight)), m_endHighlight(std::move(endHighlight))
        {
        }

    /// @private
    WordLengthGreaterEqualsWithHighlighting() = delete;

    /// @returns The tag opening a highlighted section.
    [[nodiscard]]
    const wxString& GetHighlightBegin() const noexcept
        {
        return m_beginHighlight;
        }

    /// @returns The tag closing a highlighted section.
    [[nodiscard]]
    const wxString& GetHighlightEnd() const noexcept
        {
        return m_endHighlight;
        }

    /// @internal No-op, just needed for interface contract.
    /// @private
    // cppcheck-suppress functionStatic
    void Reset() {}

  private:
    wxString m_beginHighlight;
    wxString m_endHighlight;
    };

/** @brief Determinant for whether a word is unfamiliar,
        used for highlighting unfamiliar words in a document. */
template<typename word_typeT, typename wordlistT, typename stemmerT>
class IsNotFamiliarWordWithHighlighting
    {
  public:
    /** @brief Constructor.
        @param wList the list of familiar words.
        @param beginHighlight the beginning tag of a highlight
            (can be HTML or RTF code, for example).
        @param endHighlight the ending tag of a highlight
            (can be HTML or RTF code, for example).
        @param properNounMethod for determining whether a proper noun
            (not on the word list) should be familiar or not.*/
    IsNotFamiliarWordWithHighlighting(const wordlistT* wList, wxString beginHighlight,
                                      wxString endHighlight,
                                      readability::proper_noun_counting_method properNounMethod)
        : m_is_FamiliarWord(wList, properNounMethod, true),
          m_beginHighlight(std::move(beginHighlight)), m_endHighlight(std::move(endHighlight))
        {
        }

    /// @private
    IsNotFamiliarWordWithHighlighting() = delete;

    /// @returns @c true if word is not familiar.
    /// @param theWord The word to review.
    [[nodiscard]]
    bool operator()(const word_typeT& theWord) const
        {
        return !m_is_FamiliarWord(theWord);
        }

    /// @returns The tag opening a highlighted section.
    [[nodiscard]]
    const wxString& GetHighlightBegin() const noexcept
        {
        return m_beginHighlight;
        }

    /// @returns The tag closing a highlighted section.
    [[nodiscard]]
    const wxString& GetHighlightEnd() const noexcept
        {
        return m_endHighlight;
        }

    /// @brief Resets the list of encountered proper nouns.
    void Reset() { m_is_FamiliarWord.clear_encountered_proper_nouns(); }

  private:
    readability::is_familiar_word<word_typeT, wordlistT, stemmerT> m_is_FamiliarWord;
    wxString m_beginHighlight;
    wxString m_endHighlight;
    };

/** @brief Determinant for whether a word is unfamiliar,
        used for highlighting unfamiliar words in a document.
    @details Will also mark numbers as excluded (some tests forcibly ignore all numbers).*/
template<typename word_typeT, typename wordlistT, typename stemmerT>
class IsNotFamiliarWordExcludeNumeralsWithHighlighting
    {
  public:
    /** @brief Constructor.
        @param wList the list of familiar words.
        @param beginHighlight the beginning tag of a highlight
            (can be HTML or RTF code, for example).
        @param endHighlight the ending tag of a highlight
            (can be HTML or RTF code, for example).
        @param beginExcludeHighlight the beginning tag of an excluded text highlight
            (can be HTML or RTF code, for example).
        @param endExcludeHighlight the ending tag of an excluded text highlight
            (can be HTML or RTF code, for example).
        @param properNounMethod for determining whether a proper noun (not on the word list)
            should be familiar or not.*/
    IsNotFamiliarWordExcludeNumeralsWithHighlighting(
        const wordlistT* wList, wxString beginHighlight, wxString endHighlight,
        wxString beginExcludeHighlight, wxString endExcludeHighlight,
        readability::proper_noun_counting_method properNounMethod)
        : m_isFamiliarWord(wList, properNounMethod, true),
          m_beginHighlight(std::move(beginHighlight)), m_endHighlight(std::move(endHighlight)),
          m_beginExcludeHighlight(std::move(beginExcludeHighlight)),
          m_endExcludeHighlight(std::move(endExcludeHighlight))
        {
        }

    /// @private
    IsNotFamiliarWordExcludeNumeralsWithHighlighting() = delete;

    /// @returns @c true if word is not familiar.
    /// @param theWord The word to review.
    [[nodiscard]]
    bool operator()(const word_typeT& theWord) const
        {
        // reset
        m_inExcludeState = false;
        if (theWord.is_numeric())
            {
            m_inExcludeState = true;
            return true;
            }
        return !m_isFamiliarWord(theWord);
        }

    /// @returns The tag opening a highlighted section.
    [[nodiscard]]
    const wxString& GetHighlightBegin() const noexcept
        {
        return m_inExcludeState ? m_beginExcludeHighlight : m_beginHighlight;
        }

    /// @returns The tag closing a highlighted section.
    [[nodiscard]]
    const wxString& GetHighlightEnd() const noexcept
        {
        return m_inExcludeState ? m_endExcludeHighlight : m_endHighlight;
        }

    /// @brief Resets the previously encountered proper nouns.
    void Reset() { m_isFamiliarWord.clear_encountered_proper_nouns(); }

  private:
    readability::is_familiar_word<word_typeT, wordlistT, stemmerT> m_isFamiliarWord;
    wxString m_beginHighlight;
    wxString m_endHighlight;
    wxString m_beginExcludeHighlight;
    wxString m_endExcludeHighlight;
    mutable bool m_inExcludeState{ false };
    };

template<typename word_typeT>
class IsNeverFamiliarWordWithHighlighting final
    : public readability::is_never_familiar_word<word_typeT>
    {
  public:
    /// @brief Constructor.
    IsNeverFamiliarWordWithHighlighting() = default;

    /// @returns The tag opening a highlighted section.
    [[nodiscard]]
    const wxString& GetHighlightBegin() const noexcept
        {
        return m_emptyStr;
        }

    /// @returns The tag closing a highlighted section.
    [[nodiscard]]
    const wxString& GetHighlightEnd() const noexcept
        {
        return m_emptyStr;
        }

  private:
    wxString m_emptyStr;
    };

/** @brief Functor used for the text highlighted windows.
    @details Specifically, highlights words that are Dolch words.*/
template<typename word_typeT>
class IsDolchWordWithLevelHighlighting final : public readability::is_dolch_word<word_typeT>
    {
  public:
    IsDolchWordWithLevelHighlighting(const readability::dolch_word_list* wlist,
                                     wxString beginConjunctions, wxString beginPrepositions,
                                     wxString beginPronouns, wxString beginAdverbs,
                                     wxString beginAdjectives, wxString beginVerbs,
                                     wxString beginNouns, wxString endHighlight)
        : readability::is_dolch_word<word_typeT>(wlist),
          m_beginConjunctions(std::move(beginConjunctions)),
          m_beginPrepositions(std::move(beginPrepositions)),
          m_beginPronouns(std::move(beginPronouns)), m_beginAdverbs(std::move(beginAdverbs)),
          m_beginAdjectives(std::move(beginAdjectives)), m_beginVerbs(std::move(beginVerbs)),
          m_beginNouns(std::move(beginNouns)), m_endHighlight(std::move(endHighlight))
        {
        }

    /// @private
    IsDolchWordWithLevelHighlighting() = delete;

    /// @returns The tag opening a highlighted section.
    [[nodiscard]]
    const wxString& GetHighlightBegin() const
        {
        if (readability::is_dolch_word<word_typeT>::get_last_search_result() !=
            readability::is_dolch_word<word_typeT>::get_word_list()->get_words().cend())
            {
            switch (readability::is_dolch_word<word_typeT>::get_last_search_result()->get_type())
                {
            case readability::sight_word_type::conjunction:
                return m_beginConjunctions;
            case readability::sight_word_type::preposition:
                return m_beginPrepositions;
            case readability::sight_word_type::pronoun:
                return m_beginPronouns;
            case readability::sight_word_type::adverb:
                return m_beginAdverbs;
            case readability::sight_word_type::adjective:
                return m_beginAdjectives;
            case readability::sight_word_type::verb:
                return m_beginVerbs;
            case readability::sight_word_type::noun:
                return m_beginNouns;
            default:
                return m_emptyStr;
                }
            }
        else
            {
            return m_emptyStr;
            }
        }

    /// @returns The tag closing a highlighted section.\n
    ///     If the current word being highlighted had highlighting
    ///     turned off then just return blank
    [[nodiscard]]
    const wxString& GetHighlightEnd() const
        {
        return (GetHighlightBegin().empty()) ? m_emptyStr : m_endHighlight;
        }

    /// @internal No-op, just needed for interface contract.
    /// @private
    // cppcheck-suppress functionStatic
    void Reset() {}

  private:
    wxString m_beginConjunctions;
    wxString m_beginPrepositions;
    wxString m_beginPronouns;
    wxString m_beginAdverbs;
    wxString m_beginAdjectives;
    wxString m_beginVerbs;
    wxString m_beginNouns;
    wxString m_endHighlight;
    wxString m_emptyStr;
    };

/** @brief Functor used for the text highlighted windows.
    @details Specifically, highlights words that are not Dolch words.*/
template<typename word_typeT>
class IsNotDolchWordWithLevelHighlighting
    {
  public:
    IsNotDolchWordWithLevelHighlighting(const readability::dolch_word_list* wlist,
                                        wxString beginHighlight, wxString endHighlight)
        : isDolch(wlist), m_beginHighlight(std::move(beginHighlight)),
          m_endHighlight(std::move(endHighlight))
        {
        }

    /// @private
    IsNotDolchWordWithLevelHighlighting() = delete;

    /// @returns @c true if word is not Dolch familiar.
    /// @param theWord The word to review.
    [[nodiscard]]
    bool operator()(const word_typeT& theWord) const
        {
        return !isDolch(theWord);
        }

    /// @returns The tag opening a highlighted section.
    [[nodiscard]]
    const wxString& GetHighlightBegin() const noexcept
        {
        return m_beginHighlight;
        }

    /// @returns The tag closing a highlighted section.
    [[nodiscard]]
    const wxString& GetHighlightEnd() const noexcept
        {
        return m_endHighlight;
        }

    /// @internal No-op, just needed for interface contract.
    /// @private
    // cppcheck-suppress functionStatic
    void Reset() {}

  private:
    readability::is_dolch_word<word_typeT> isDolch;
    wxString m_beginHighlight;
    wxString m_endHighlight;
    };

/** @brief Functor used for the text highlighted windows.
    @details Specifically, highlights words that are not familiar
        to custom tests.*/
template<typename CustomReadabilityTestInterfaceT>
class IsNotCustomFamiliarWordWithHighlighting
    {
  public:
    IsNotCustomFamiliarWordWithHighlighting(CustomReadabilityTestInterfaceT wordTest,
                                            wxString beginHighlight, wxString endHighlight)
        : m_wordTest(wordTest), m_beginHighlight(std::move(beginHighlight)),
          m_endHighlight(std::move(endHighlight))
        {
        }

    /// @returns @c true if word is not familiar.
    /// @param theWord The word to review.
    [[nodiscard]]
    bool operator()(const word_case_insensitive_no_stem& theWord) const
        {
        return !m_wordTest->operator()(theWord);
        }

    /// @returns The tag opening a highlighted section.
    [[nodiscard]]
    const wxString& GetHighlightBegin() const noexcept
        {
        return m_beginHighlight;
        }

    /// @returns The tag closing a highlighted section.
    [[nodiscard]]
    const wxString& GetHighlightEnd() const noexcept
        {
        return m_endHighlight;
        }

    /// @brief Clears the cached first occurrences of proper nouns.
    void Reset() { m_wordTest->ResetIterator(); }

  private:
    CustomReadabilityTestInterfaceT m_wordTest;
    wxString m_beginHighlight;
    wxString m_endHighlight;
    };

/** @brief Functor used for the text highlighted windows.
    @details Specifically, highlights words that are not familiar
        to custom tests (excluding numerals).*/
template<typename CustomReadabilityTestInterfaceT>
class IsNotCustomFamiliarWordExcludeNumeralsWithHighlighting
    {
  public:
    IsNotCustomFamiliarWordExcludeNumeralsWithHighlighting(CustomReadabilityTestInterfaceT wordTest,
                                                           wxString beginHighlight,
                                                           wxString endHighlight,
                                                           wxString beginExcludeHighlight,
                                                           wxString endExcludeHighlight)
        : m_wordTest(wordTest), m_beginHighlight(std::move(beginHighlight)),
          m_endHighlight(std::move(endHighlight)),
          m_beginExcludeHighlight(std::move(beginExcludeHighlight)),
          m_endExcludeHighlight(std::move(endExcludeHighlight))
        {
        }

    /// @returns @c true if word is not familiar.
    /// @param theWord The word to review.
    [[nodiscard]]
    bool operator()(const word_case_insensitive_no_stem& theWord) const
        {
        // reset
        m_inExcludeState = false;
        if (theWord.is_numeric())
            {
            m_inExcludeState = true;
            return true;
            }
        return !m_wordTest->operator()(theWord);
        }

    /// @returns The tag opening a highlighted section.
    [[nodiscard]]
    const wxString& GetHighlightBegin() const noexcept
        {
        return m_inExcludeState ? m_beginExcludeHighlight : m_beginHighlight;
        }

    /// @returns The tag closing a highlighted section.
    [[nodiscard]]
    const wxString& GetHighlightEnd() const noexcept
        {
        return m_inExcludeState ? m_endExcludeHighlight : m_endHighlight;
        }

    /// @brief Resets the internal iterators.
    void Reset() { m_wordTest->ResetIterator(); }

  private:
    CustomReadabilityTestInterfaceT m_wordTest;
    wxString m_beginHighlight;
    wxString m_endHighlight;
    wxString m_beginExcludeHighlight;
    wxString m_endExcludeHighlight;
    mutable bool m_inExcludeState{ false };
    };

#endif // TESTS_FUNCTIONAL_H
