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

#include "syllable.h"
#include <utility>

namespace grammar
    {
    const std::set<traits::case_insensitive_wstring_ex>
        english_syllabize::m_e_disconnecting_prefixes = {
            L"corr",  L"minn", L"diss", L"disc", L"comm", L"dysp", L"ind", L"cer", L"alg", L"coh",
            L"phoeb", L"art",  L"ben",  L"tel",  L"dis",  L"irr",  L"all", L"alp", L"amn", L"arr",
            L"agg",   L"ass",  L"eff",  L"ell",  L"ill",  L"imm",  L"un",  L"dand"
        };

    const std::set<traits::case_insensitive_wstring_ex>
        english_syllabize::m_non_affecting_suffixes_4 = { L"ness", L"ment", L"room",
                                                          L"shoe", L"pick", L"maid",
                                                          L"yard", L"book", L"hill" };

    //----------------------------------------------
    bool base_syllabize::syllabize_japanese(const std::wstring_view word)
        {
        if (word.empty())
            {
            return false;
            }

        size_t japaneseCount{ 0 };
        size_t syllableCount{ 0 };
        for (const auto ch : word)
            {
            if (characters::is_character::is_japanese_script(ch))
                {
                ++japaneseCount;
                if (!characters::is_character::is_small_kana(ch))
                    {
                    ++syllableCount;
                    }
                }
            }

        if (japaneseCount == 0)
            {
            return false;
            }

        if (std::cmp_equal(japaneseCount, word.length()))
            {
            m_syllable_count = (syllableCount > 0) ? syllableCount : 1;
            return true;
            }

        // mixed: count Japanese characters as syllables,
        // let the caller handle the rest
        m_syllable_count += syllableCount;
        return false;
        }

    //----------------------------------------------
    std::pair<bool, size_t> base_syllabize::is_special_math_word(const wchar_t* start,
                                                                 const size_t length) noexcept
        {
        assert(start);
        if (length == 2 && characters::is_character::is_numeric_simple(start[0]) &&
            traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_D))
            {
            return std::make_pair(true, 2);
            }
        return std::make_pair(false, 0);
        }

    //----------------------------------------------
    size_t base_syllabize::get_symbol_syllable_count(const wchar_t* start, const wchar_t* end,
                                                     const wchar_t* current_char) noexcept
        {
        assert(start);
        assert(end);
        assert(current_char);
        if (current_char[0] == common_lang_constants::POUND)
            {
            return 1;
            }
        // "plus/minus" if symbol is at front of word
        if (start == current_char && traits::case_insensitive_ex::eq(current_char[0], 177))
            {
            return 3;
            }
        // "dollars" or "pesos" (Cuban) if dollar symbol is at front of word
        if (start == current_char &&
            (traits::case_insensitive_ex::eq(current_char[0], common_lang_constants::DOLLAR_SIGN) ||
             traits::case_insensitive_ex::eq(current_char[0], 0x20B1)))
            {
            return 2;
            }
        // Euro
        if (start == current_char && traits::case_insensitive_ex::eq(current_char[0], 0x20AC))
            {
            return 2;
            }
        // Korean "won" symbol
        if (start == current_char && traits::case_insensitive_ex::eq(current_char[0], 0x20A9))
            {
            return 1;
            }
        // Pound Sterling/"quid" if symbol is at front of word
        if (start == current_char && traits::case_insensitive_ex::eq(current_char[0], 163))
            {
            return 1;
            }
        // "cents" if  symbol is at end of word
        if ((end - 1) == current_char && traits::case_insensitive_ex::eq(current_char[0], 162))
            {
            return 1;
            }
        // "percent"/"degrees"/"one-half"/"one-fourth"/"three-fourth" if at end of word
        if ((end - 1) == current_char &&
            (traits::case_insensitive_ex::eq(current_char[0],
                                             common_lang_constants::PERCENTAGE_SIGN) ||
             traits::case_insensitive_ex::eq(current_char[0], 176) ||
             traits::case_insensitive_ex::eq(current_char[0], 188) ||
             traits::case_insensitive_ex::eq(current_char[0], 189) ||
             traits::case_insensitive_ex::eq(current_char[0], 190)))
            {
            return 2;
            }
        // fractions, super and subscripts
        if (string_util::is_fraction(current_char[0]) ||
            string_util::is_superscript(current_char[0]) ||
            string_util::is_subscript(current_char[0]))
            {
            return 1;
            }
        return 0;
        }

    //----------------------------------------------
    void base_syllabize::adjust_length_if_possessive(const wchar_t* start)
        {
        assert(start);
        if (start == nullptr)
            {
            return;
            }
        if (m_length >= 3 && characters::is_character::is_apostrophe(start[m_length - 2]) &&
            traits::case_insensitive_ex::eq(start[m_length - 1], common_lang_constants::LOWER_S))
            {
            m_length -= 2;
            }
        else if (m_length >= 2 && characters::is_character::is_apostrophe(start[m_length - 1]))
            {
            --m_length;
            }
        }

    //----------------------------------------------
    bool base_syllabize::is_consonant_y(const wchar_t* word, size_t position) const
        {
        assert(word);
        assert(traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_Y));
        if (word == nullptr)
            {
            return false;
            }
        if (!traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_Y))
            {
            return false;
            }
        // if 'y' begins word then it cannot be a vowel sound
        if (position == 0)
            {
            return true;
            }
        // or if after another vowel (alloyed)
        if (characters::is_character::is_vowel(word[position - 1]))
            {
            // boy
            return true;
            }
        // tanya, anyu
        if (position + 2 == m_length && characters::is_character::is_vowel(word[position + 1]))
            {
            return true;
            }
        // lAwyer, but watch out for compounds like "hIghflYer"
        if ((m_previous_vowel != m_length) && (position - m_previous_vowel) < 3 &&
            (position + 2 < m_length) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_R))
            {
            return true;
            }
        // churchyard
        if ((position + 3 < m_length) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_A) &&
            traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_R) &&
            traits::case_insensitive_ex::eq(word[position + 3], common_lang_constants::LOWER_D))
            {
            return true;
            }
        return false;
        }

    //----------------------------------------------
    size_t english_syllabize::operator()(const wchar_t* start, const size_t length)
        {
        // reset our data
        reset();
        m_previous_vowel = 0;
        m_previous_block_vowel = 0;
        m_was_last_vowel_block_separable_vowels = false;
        m_ends_with_nt_contraction = false;
        // sanity checks
        assert(start && "null string passed to syllable parser");
        if (start == nullptr || length == 0)
            {
            return 0;
            }

        m_length = length;

        if (syllabize_japanese({ start, m_length }))
            {
            return m_syllable_count;
            }

        adjust_length_if_possessive(start);

        m_ends_with_nt_contraction =
            (m_length >= 4 &&
             traits::case_insensitive_ex::eq(start[m_length - 3], common_lang_constants::LOWER_N) &&
             characters::is_character::is_apostrophe(start[m_length - 2]) &&
             traits::case_insensitive_ex::eq(start[m_length - 1], common_lang_constants::LOWER_T));

        const std::pair<bool, size_t> mathResult = is_special_math_word(start, m_length);
        if (mathResult.first)
            {
            return m_syllable_count = mathResult.second;
            }

        if (syllabize_if_contains_periods<english_syllabize>(start, std::next(start, m_length)))
            {
            return m_syllable_count;
            }

        if (syllabize_if_contains_dashes<english_syllabize>(start))
            {
            return m_syllable_count;
            }

        const std::pair<size_t, size_t> prefixResult = get_prefix_length(start, m_length);
        if (prefixResult.second > 0)
            {
            start += prefixResult.second;
            m_length -= prefixResult.second;
            m_syllable_count += prefixResult.first;
            }

        m_previous_block_vowel = m_previous_vowel = m_length;
        bool isInVowelBlock = false;
        bool currentCharIsVowel = false;
        const wchar_t* currentChar = start;
        const wchar_t* end = std::next(start, m_length);
        bool wasLastVowelSilentE = false;

        while (currentChar != end)
            {
            currentCharIsVowel = characters::is_character::is_vowel(currentChar[0]);
            currentCharIsVowel =
                (traits::case_insensitive_ex::eq(currentChar[0], common_lang_constants::LOWER_Y) &&
                 is_consonant_y(start, currentChar - start)) ?
                    false :
                    currentCharIsVowel;
            currentCharIsVowel =
                (traits::case_insensitive_ex::eq(currentChar[0], common_lang_constants::LOWER_U) &&
                 is_silent_u(start, currentChar - start)) ?
                    false :
                    currentCharIsVowel;

            bool nextCharIsVowel = false;
            /* if last letter, then there is no next letter*/
            if ((m_length - 1) == static_cast<size_t>(currentChar - start))
                {
                nextCharIsVowel = false;
                }
            else if (std::next(currentChar) < end)
                {
                nextCharIsVowel = characters::is_character::is_vowel(currentChar[1]);
                nextCharIsVowel = (traits::case_insensitive_ex::eq(
                                       currentChar[1], common_lang_constants::LOWER_Y) &&
                                   is_consonant_y(start, (currentChar + 1) - start)) ?
                                      false :
                                      nextCharIsVowel;
                nextCharIsVowel = (traits::case_insensitive_ex::eq(
                                       currentChar[1], common_lang_constants::LOWER_U) &&
                                   is_silent_u(start, (currentChar + 1) - start)) ?
                                      false :
                                      nextCharIsVowel;
                }

            isInVowelBlock = currentCharIsVowel && nextCharIsVowel;
            // if it's a vowel and it's the only one in this block
            if (currentCharIsVowel && !isInVowelBlock)
                {
                // only check an 'e' for silence if there was already a previous vowel
                if (m_previous_vowel != m_length && !wasLastVowelSilentE &&
                    (traits::case_insensitive_ex::eq(currentChar[0],
                                                     common_lang_constants::LOWER_E) &&
                     is_silent_e(start, (currentChar - start))))
                    {
                    // if 'e' is silent then treat it as part of the previous sound
                    // and not part of a multiple vowel sound
                    wasLastVowelSilentE = true;
                    }
                else if (!traits::case_insensitive_ex::eq(currentChar[0],
                                                          common_lang_constants::LOWER_E) &&
                         is_single_non_e_vowel_ignored(start, (currentChar - start)))
                    { /* NOOP, just skip this vowel*/
                    }
                else
                    {
                    ++m_syllable_count;
                    wasLastVowelSilentE = false;
                    }
                m_was_last_vowel_block_separable_vowels = false;
                m_previous_vowel = currentChar - start;
                }
            else if (currentCharIsVowel && isInVowelBlock)
                {
                wasLastVowelSilentE = false;
                const wchar_t* startOfBlock{ currentChar };
                while ((std::next(currentChar) < end) &&
                       characters::is_character::is_vowel(currentChar[1]))
                    {
                    ++currentChar;
                    }
                // if it is two consecutive vowels then make sure they
                // aren't separate syllables
                // io[ai] is 3 syllables
                if (((currentChar + 1) - startOfBlock) == 3 &&
                    traits::case_insensitive_ex::eq(start[startOfBlock - start],
                                                    common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(start[(startOfBlock - start) + 1],
                                                    common_lang_constants::LOWER_O) &&
                    (traits::case_insensitive_ex::eq(start[(startOfBlock - start) + 2],
                                                     common_lang_constants::LOWER_I) ||
                     traits::case_insensitive_ex::eq(start[(startOfBlock - start) + 2],
                                                     common_lang_constants::LOWER_A)))
                    {
                    m_was_last_vowel_block_separable_vowels = true;
                    m_syllable_count += 3;
                    }
                else if (is_vowels_separate_syllables(
                             start, startOfBlock - start, (currentChar + 1) - startOfBlock,
                             std::cmp_greater(m_previous_block_vowel, startOfBlock - start)))
                    {
                    m_was_last_vowel_block_separable_vowels = true;
                    m_syllable_count += 2;
                    }
                else
                    {
                    ++m_syllable_count;
                    m_was_last_vowel_block_separable_vowels = false;
                    }
                m_previous_vowel = currentChar - start;
                }
            // syllabize numbers
            else if (characters::is_character::is_numeric_simple(currentChar[0]))
                {
                size_t charactersCounted{ 0 };
                m_syllable_count += syllabify_numeral<syllabize_english_number>(
                    currentChar, end, charactersCounted, common_lang_constants::COMMA,
                    common_lang_constants::PERIOD);
                currentChar += charactersCounted;
                if (charactersCounted == 0)
                    {
                    break;
                    }
                if (currentChar >= end)
                    {
                    break;
                    }
                // else, we already moved to the next character to analyze, so just restart loop
                continue;
                }
            // syllabize any pertinent symbols
            m_syllable_count += get_symbol_syllable_count(start, end, currentChar);
            if (!currentCharIsVowel)
                {
                m_previous_block_vowel = m_previous_vowel;
                }
            ++currentChar;
            }

        finalize_special_cases(start);
        // all words are at least one syllable (even all consonant acronyms)
        m_syllable_count = (m_syllable_count > 0) ? m_syllable_count : 1;
        return m_syllable_count;
        }

    //----------------------------------------------
    void english_syllabize::finalize_special_cases(const wchar_t* start)
        {
        if (start == nullptr)
            {
            return;
            }
        // Irish names with preceding "Mc" is a separate syllable
        if (m_length >= 2 &&
            traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_M) &&
            traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_C))
            {
            ++m_syllable_count;
            return;
            }
        // that'll, it'll, what'll, they'll are 2 syllables
        if (m_length >= 5 && characters::is_character::is_apostrophe(start[m_length - 3]) &&
            traits::case_insensitive_ex::eq(start[m_length - 2], common_lang_constants::LOWER_L) &&
            traits::case_insensitive_ex::eq(start[m_length - 1], common_lang_constants::LOWER_L))
            {
            // that, it, what, they
            if ((traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_T) &&
                 traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_H) &&
                 traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A) &&
                 traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_T)) ||
                (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_I) &&
                 traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_T)) ||
                (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_W) &&
                 traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_H) &&
                 traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A) &&
                 traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_T)) ||
                (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_T) &&
                 traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_H) &&
                 traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_E) &&
                 traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_Y)))
                {
                ++m_syllable_count;
                return;
                }
            }
        // Special case contractions
        else if (m_ends_with_nt_contraction)
            {
            // should(n't)
            if (m_length >= 6 &&
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_H) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_U) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_L) &&
                traits::case_insensitive_ex::eq(start[5], common_lang_constants::LOWER_D))
                {
                ++m_syllable_count;
                return;
                }
            // might, would, could, ought
            if (m_length >= 5 &&
                // might
                ((traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_M) &&
                  traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_I) &&
                  traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_G) &&
                  traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_H) &&
                  traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_T)) ||
                 // would/could
                 ((traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_W) ||
                   traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_C)) &&
                  traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_O) &&
                  traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_U) &&
                  traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_L) &&
                  traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_D)) ||
                 // ought
                 (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_O) &&
                  traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_U) &&
                  traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_G) &&
                  traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_H) &&
                  traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_T))))
                {
                ++m_syllable_count;
                return;
                }
            if (m_length >= 4 &&
                // does
                ((traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_D) &&
                  traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_O) &&
                  traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_E) &&
                  traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_S)) ||
                 // must
                 (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_M) &&
                  traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_U) &&
                  traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_S) &&
                  traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_T)) ||
                 // need
                 (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_N) &&
                  traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                  traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_E) &&
                  traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_D)) ||
                 // have
                 (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_H) &&
                  traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_A) &&
                  traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_V) &&
                  traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_E))))
                {
                ++m_syllable_count;
                return;
                }
            if (m_length >= 3 &&
                // has
                ((traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_H) &&
                  traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_A) &&
                  traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_S)) ||
                 // had
                 (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_H) &&
                  traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_A) &&
                  traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_D)) ||
                 // did
                 (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_D) &&
                  traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_I) &&
                  traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_D)) ||
                 // was
                 (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_W) &&
                  traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_A) &&
                  traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_S))))
                {
                ++m_syllable_count;
                return;
                }
            if (m_length >= 2 &&
                // is
                traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_S))
                {
                ++m_syllable_count;
                return;
                }
            }
        }

    //----------------------------------------------
    bool english_syllabize::is_silent_u(const wchar_t* word, size_t position) const
        {
        assert(word);
        // verify that is a 'u' first
        assert(traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_U));
        if (word == nullptr)
            {
            return false;
            }
        if (!traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_U))
            {
            return false;
            }
        // "qu" makes the 'u' silent
        if ((position > 0) &&
            traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_Q))
            {
            return true;
            }
        // "gu[vowel]" makes the 'u' silent (forms a "gw" sound)
        // exceptions exists though, such as "ambiguity"
        if ((position > 0) && (position + 2 <= m_length) &&
            traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_G) &&
            (characters::is_character::is_vowel(word[position + 1])))
            {
            // watch out for out vowel combinations such as "guous"
            if (position + 3 <= m_length &&
                characters::is_character::is_vowel(word[position + 1]) &&
                characters::is_character::is_vowel(word[position + 2]))
                {
                return false;
                }
            // ambiguity
            if (position + 3 <= m_length &&
                traits::case_insensitive_ex::eq(word[position + 1],
                                                common_lang_constants::LOWER_I) &&
                // position+1 already determined to be a consonant
                traits::case_insensitive_ex::eq(word[position + 3], common_lang_constants::LOWER_Y))
                {
                return false;
                }
            return true;
            }
        return false;
        }

    //----------------------------------------------
    bool english_syllabize::does_prefix_silence_e(const wchar_t* word, size_t prefix_length) const
        {
        assert(word);
        if (word == nullptr)
            {
            return false;
            }
        if (prefix_length == 3)
            {
            // fore
            if (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_F) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_R))
                {
                // exceptions: forest...
                if (m_length >= 6 &&
                    traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(word[5], common_lang_constants::LOWER_T))
                    {
                    // fore-stall
                    return m_length >= 7 &&
                           traits::case_insensitive_ex::eq(word[6], common_lang_constants::LOWER_A);
                    }
                // forex
                if (m_length >= 5 &&
                    traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_X))
                    {
                    return false;
                    }
                // for-en-sic
                if (m_length >= 8 &&
                    traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(word[5], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(word[6], common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[7], common_lang_constants::LOWER_C))
                    {
                    return false;
                    }
                // for-e-ver
                if (m_length >= 5 &&
                    traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_V))
                    {
                    return false;
                    }
                // fore-sight
                return true;
                }
            return false;
            }
        return false;
        }

    //----------------------------------------------
    bool english_syllabize::does_prefix_disconnect_e(const wchar_t* word, size_t prefix_length)
        {
        return m_e_disconnecting_prefixes.contains(
            traits::case_insensitive_wstring_ex(word, prefix_length));
        }

    //----------------------------------------------
    bool english_syllabize::is_none_affecting_suffix(const wchar_t* word, size_t suffix_length)
        {
        assert(word);
        if (suffix_length >= 4 &&
            (m_non_affecting_suffixes_4.contains(traits::case_insensitive_wstring_ex(word, 4))))
            {
            return true;
            }
        if (suffix_length >= 3 &&
            ((traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_M) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_N)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_B) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_X))))
            {
            return true;
            }
        if (suffix_length >= 2 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_L) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_Y))
            {
            return true;
            }
        return false;
        }

    //----------------------------------------------
    bool english_syllabize::does_suffix_negate_silent_e(const wchar_t* suffix,
                                                        const size_t suffix_length,
                                                        const size_t next_vowel_index)
        {
        assert(suffix);
        if (suffix == nullptr)
            {
            return false;
            }
        // Check for explicitly known suffixes that can follow a silent 'e'.
        // Just return true from here for these and the parent logic will further attempt to figure
        // out whether this is really a silent 'e.'
        if (suffix_length >= 5 &&
            traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_T) &&
            traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_H) &&
            traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_N) &&
            traits::case_insensitive_ex::eq(suffix[4], common_lang_constants::LOWER_G))
            {
            return false;
            }
        if (suffix_length >= 5 &&
            traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_L) &&
            traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_S) &&
            traits::case_insensitive_ex::eq(suffix[4], common_lang_constants::LOWER_T))
            {
            return false;
            }
        if (suffix_length >= 4 &&
            traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_W) &&
            traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_R) &&
            traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_T))
            {
            return false;
            }
        // The rest will negate
        // harv-e-ster
        if (suffix_length >= 4 &&
            traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_S) &&
            traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_T) &&
            traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_R))
            {
            return true;
            }
        // Pam-e-la
        if (suffix_length == 2 &&
            traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_L) &&
            traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_A))
            {
            return true;
            }
        // Pam-e-las
        if (suffix_length == 3 &&
            traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_L) &&
            traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_A) &&
            traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_S))
            {
            return true;
            }
        // Cam-e-lot, oc-e-lot
        if (suffix_length >= 3 &&
            traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_L) &&
            traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_O) &&
            traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_T))
            {
            return true;
            }
        // inebriate
        if ((suffix_length - next_vowel_index) >= 4 &&
            ((traits::case_insensitive_ex::eq(suffix[next_vowel_index],
                                              common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(suffix[next_vowel_index + 1],
                                              common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(suffix[next_vowel_index + 2],
                                              common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(suffix[next_vowel_index + 3],
                                              common_lang_constants::LOWER_E)) ||
             (traits::case_insensitive_ex::eq(suffix[next_vowel_index],
                                              common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(suffix[next_vowel_index + 1],
                                              common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(suffix[next_vowel_index + 2],
                                              common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(suffix[next_vowel_index + 3],
                                              common_lang_constants::LOWER_Y))))
            {
            return true;
            }
        // celebrate, procedure
        if ((suffix_length - next_vowel_index) >= 3 &&
            ((traits::case_insensitive_ex::eq(suffix[next_vowel_index],
                                              common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(suffix[next_vowel_index + 1],
                                              common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(suffix[next_vowel_index + 2],
                                              common_lang_constants::LOWER_E)) ||
             (traits::case_insensitive_ex::eq(suffix[next_vowel_index],
                                              common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(suffix[next_vowel_index + 1],
                                              common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(suffix[next_vowel_index + 2],
                                              common_lang_constants::LOWER_I)) ||
             (traits::case_insensitive_ex::eq(suffix[next_vowel_index],
                                              common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(suffix[next_vowel_index + 1],
                                              common_lang_constants::LOWER_C) &&
              traits::case_insensitive_ex::eq(suffix[next_vowel_index + 2],
                                              common_lang_constants::LOWER_T)) ||
             (traits::case_insensitive_ex::eq(suffix[next_vowel_index],
                                              common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(suffix[next_vowel_index + 1],
                                              common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(suffix[next_vowel_index + 2],
                                              common_lang_constants::LOWER_D)) ||
             (traits::case_insensitive_ex::eq(suffix[next_vowel_index],
                                              common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(suffix[next_vowel_index + 1],
                                              common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(suffix[next_vowel_index + 2],
                                              common_lang_constants::LOWER_S)) ||
             (traits::case_insensitive_ex::eq(suffix[next_vowel_index],
                                              common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(suffix[next_vowel_index + 1],
                                              common_lang_constants::LOWER_Z) &&
              traits::case_insensitive_ex::eq(suffix[next_vowel_index + 2],
                                              common_lang_constants::LOWER_E)) ||
             (traits::case_insensitive_ex::eq(suffix[next_vowel_index],
                                              common_lang_constants::LOWER_U) &&
              traits::case_insensitive_ex::eq(suffix[next_vowel_index + 1],
                                              common_lang_constants::LOWER_R) &&
              traits::case_insensitive_ex::eq(suffix[next_vowel_index + 2],
                                              common_lang_constants::LOWER_E)) ||
             (traits::case_insensitive_ex::eq(suffix[next_vowel_index],
                                              common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(suffix[next_vowel_index + 1],
                                              common_lang_constants::LOWER_N) &&
              traits::case_insensitive_ex::eq(suffix[next_vowel_index + 2],
                                              common_lang_constants::LOWER_T)) ||
             (traits::case_insensitive_ex::eq(suffix[next_vowel_index],
                                              common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(suffix[next_vowel_index + 1],
                                              common_lang_constants::LOWER_N) &&
              traits::case_insensitive_ex::eq(suffix[next_vowel_index + 2],
                                              common_lang_constants::LOWER_C)) ||
             (traits::case_insensitive_ex::eq(suffix[next_vowel_index],
                                              common_lang_constants::LOWER_U) &&
              traits::case_insensitive_ex::eq(suffix[next_vowel_index + 1],
                                              common_lang_constants::LOWER_T))))
            {
            return true;
            }
        // angelic, generic, benefice, bedevil
        if ((suffix_length - next_vowel_index) >= 2 &&
            traits::case_insensitive_ex::eq(suffix[next_vowel_index],
                                            common_lang_constants::LOWER_I) &&
            (traits::case_insensitive_ex::eq(suffix[next_vowel_index + 1],
                                             common_lang_constants::LOWER_C) ||
             traits::case_insensitive_ex::eq(suffix[next_vowel_index + 1],
                                             common_lang_constants::LOWER_T) ||
             traits::case_insensitive_ex::eq(suffix[next_vowel_index + 1],
                                             common_lang_constants::LOWER_L)))
            {
            return true;
            }
        /*if the word ends like "e[consonant block]e" then the front 'e' must be part of the last
        syllable and the last 'e' will be silent*/
        if ((suffix_length - next_vowel_index) == 1 &&
            traits::case_insensitive_ex::eq(suffix[next_vowel_index],
                                            common_lang_constants::LOWER_E))
            {
            return true;
            }
        /*if the word ends like "e[consonant block][es][er][ed][or][ing]" then the front 'e' must be
        part of the last syllable*/
        if ((suffix_length - next_vowel_index) == 2 &&
            ((traits::case_insensitive_ex::eq(suffix[next_vowel_index],
                                              common_lang_constants::LOWER_E) &&
              (traits::case_insensitive_ex::eq(suffix[next_vowel_index + 1],
                                               common_lang_constants::LOWER_S) ||
               traits::case_insensitive_ex::eq(suffix[next_vowel_index + 1],
                                               common_lang_constants::LOWER_D) ||
               traits::case_insensitive_ex::eq(suffix[next_vowel_index + 1],
                                               common_lang_constants::LOWER_R))) ||
             (traits::case_insensitive_ex::eq(suffix[next_vowel_index],
                                              common_lang_constants::LOWER_O) &&
              traits::case_insensitive_ex::eq(suffix[next_vowel_index + 1],
                                              common_lang_constants::LOWER_R))))
            {
            return true;
            }
        // forEsting--the middle 'e' is part of a separate vowel
        if ((suffix_length - next_vowel_index) == 3 &&
            traits::case_insensitive_ex::eq(suffix[next_vowel_index],
                                            common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(suffix[next_vowel_index + 1],
                                            common_lang_constants::LOWER_N) &&
            traits::case_insensitive_ex::eq(suffix[next_vowel_index + 2],
                                            common_lang_constants::LOWER_G))
            {
            return true;
            }
        if (suffix_length >= 6 &&
            traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_S) &&
            traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_C) &&
            traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_N) &&
            traits::case_insensitive_ex::eq(suffix[4], common_lang_constants::LOWER_C) &&
            traits::case_insensitive_ex::eq(suffix[5], common_lang_constants::LOWER_E))
            {
            return true;
            }
        if (suffix_length >= 5 &&
            ((traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_M) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(suffix[4], common_lang_constants::LOWER_C)) ||
             // scend, scent
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_S) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_C) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_N) &&
              (traits::case_insensitive_ex::eq(suffix[4], common_lang_constants::LOWER_D) ||
               traits::case_insensitive_ex::eq(suffix[4], common_lang_constants::LOWER_T))) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_L) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(suffix[4], common_lang_constants::LOWER_E)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_S) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_R) &&
              traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(suffix[4], common_lang_constants::LOWER_L)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_L) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_C) &&
              traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(suffix[4], common_lang_constants::LOWER_L))))
            {
            return true;
            }
        if (suffix_length >= 4 &&
            ((traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_L) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_S) &&
              traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_M)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_L) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_Z) &&
              traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_E)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_S) &&
              traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_M)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_D) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_R) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_L)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_Q) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_U) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_N)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_N) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_L)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_P) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_O) &&
              traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_R)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_S) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_O) &&
              traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_R)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_S) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_R) &&
              traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_Y)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_H) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_R)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_R) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_C)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_Q) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_U) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_N)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_V) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_S) &&
              traits::case_insensitive_ex::eq(suffix[3], common_lang_constants::LOWER_M))))
            {
            return true;
            }
        if (suffix_length >= 3 &&
            ((traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_M) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_C)) ||
             // cat-E-gor-y ('e' is not silent)
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_G) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_O) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_R)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_S) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_S)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_C)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_R)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_S)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_N)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_R) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_Y)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_U) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_M)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_V) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_R)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_V) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_K)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_M) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_A)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_G) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(suffix[2], common_lang_constants::LOWER_S))))
            {
            return true;
            }
        if (suffix_length >= 2 &&
            ((traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_G) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_A)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_B) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_O)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_F) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_Y)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_M) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_A)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_M) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_Y)) ||
             (traits::case_insensitive_ex::eq(suffix[0], common_lang_constants::LOWER_R) &&
              traits::case_insensitive_ex::eq(suffix[1], common_lang_constants::LOWER_R))))
            {
            return true;
            }
        return false;
        }

    //----------------------------------------------
    bool english_syllabize::is_single_non_e_vowel_ignored(const wchar_t* word,
                                                          const size_t position) const
        {
        assert(word);
        if (word == nullptr)
            {
            return false;
            }
        assert(!traits::case_insensitive_ex::eq(word[position], L'e') &&
               "is_single_non_e_vowel_ignored() should not be reviewing an 'e', use is_silent_e()");
        // busi-ness (pronounced "biz-nis")
        return (
            position >= 3 && m_length >= 8 && position + 4 < m_length &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
            // prefix and suffix
            traits::case_insensitive_ex::eq(word[position - 3], common_lang_constants::LOWER_B) &&
            traits::case_insensitive_ex::eq(word[position - 2], common_lang_constants::LOWER_U) &&
            traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_S) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_N) &&
            traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[position + 3], common_lang_constants::LOWER_S) &&
            traits::case_insensitive_ex::eq(word[position + 4], common_lang_constants::LOWER_S));
        }

    //----------------------------------------------
    bool english_syllabize::is_silent_e(const wchar_t* word, const size_t position) const
        {
        assert(word);
        // verify that it's an 'e' first and
        // not the first letter (first letter can't be silent)
        if (position == 0 ||
            (!traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E)))
            {
            return false;
            }
        // [letters]e[number] would be silent (e.g., "base64")
        if (position + 1 < m_length &&
            characters::is_character::is_numeric_simple(word[position + 1]))
            {
            return true;
            }
        // if 'e' is second to last letter and NOT followed by 's' or 'd' then it's not silent
        if (position + 2 == m_length &&
            !traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_S) &&
            !traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_D))
            {
            return false;
            }
        // special case for "aren't" and "weren't"
        if (position == 2 && m_ends_with_nt_contraction && m_length == 6)
            {
            return true;
            }
        if (position == 3 && m_ends_with_nt_contraction && m_length == 7)
            {
            return true;
            }
        // special case for "'re" and "'ve" contractions
        if (position == m_length - 1 && m_length > 3 &&
            (traits::case_insensitive_ex::eq(word[m_length - 2], common_lang_constants::LOWER_R) ||
             traits::case_insensitive_ex::eq(word[m_length - 2], common_lang_constants::LOWER_V)) &&
            characters::is_character::is_apostrophe(word[m_length - 3]))
            {
            return true;
            }
        // e-sty, e-sties
        if (position <= m_length - 3 &&
            traits::case_insensitive_ex::eq(word[m_length - 3], common_lang_constants::LOWER_S) &&
            traits::case_insensitive_ex::eq(word[m_length - 2], common_lang_constants::LOWER_T) &&
            (traits::case_insensitive_ex::eq(word[m_length - 1], common_lang_constants::LOWER_Y) ||
             traits::case_insensitive_ex::eq(word[m_length - 1], common_lang_constants::LOWER_I)))
            {
            return false;
            }
        // see if there are any proceeding vowels
        size_t nextVowel = m_length;
        if (position + 2 < m_length)
            {
            // start from last position and backtrack
            for (nextVowel = (position + 1); nextVowel < m_length; ++nextVowel)
                {
                // found another vowel before end of word,
                // so this 'e' is still a candidate
                if (characters::is_character::is_vowel(word[nextVowel]))
                    {
                    // watch out for "ey", this could still be a silent y
                    if (traits::case_insensitive_ex::eq(word[nextVowel],
                                                        common_lang_constants::LOWER_Y) &&
                        is_consonant_y(word, nextVowel))
                        {
                        continue;
                        }
                    break;
                    }
                }
            if (nextVowel == m_length || nextVowel == position + 1)
                {
                // no proceeding vowels or 'e' is directly proceeded by another vowel,
                // so this fails
                return false;
                }
            }
        // make sure that there are previous vowels;
        // otherwise, an 'e' could not be a silent modifier
        if (m_previous_vowel == m_length)
            {
            // no previous vowels, so this fails
            return false;
            }
        // special case for "surveyor"
        if (position <= m_length - 4 &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_Y) &&
            traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_O) &&
            traits::case_insensitive_ex::eq(word[position + 3], common_lang_constants::LOWER_R))
            {
            return false;
            }
        // wesleyan
        if (position <= m_length - 4 &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_Y) &&
            traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_A) &&
            traits::case_insensitive_ex::eq(word[position + 3], common_lang_constants::LOWER_N))
            {
            return false;
            }
        // special case for "honeyest"
        if (position <= m_length - 5 &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_Y) &&
            traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[position + 3], common_lang_constants::LOWER_S) &&
            traits::case_insensitive_ex::eq(word[position + 4], common_lang_constants::LOWER_T))
            {
            return false;
            }
        // special case for "eve-ry"
        if (position == 2 && m_length >= 5 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_V) &&
            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_R) &&
            traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_Y))
            {
            return true;
            }
        // special case for "equEstrienne" (this is not silent)
        if (position == 3 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_Q) &&
            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_U))
            {
            return false;
            }
        // special case when 'y' is the vowel that may be getting modified
        if (traits::case_insensitive_ex::eq(word[m_previous_vowel],
                                            common_lang_constants::LOWER_Y) &&
            m_previous_vowel > 0 /*'y' as beginning of the word would be a consonant*/)
            {
            // oxyhemoglobin, system
            if (traits::case_insensitive_ex::eq(word[m_previous_vowel - 1],
                                                common_lang_constants::LOWER_X) ||
                traits::case_insensitive_ex::eq(word[m_previous_vowel - 1],
                                                common_lang_constants::LOWER_S))
                {
                return false;
                }
            }

        // special case where "tente" and "tante" see last 'e' as silent
        if (position == (m_length - 1) && position >= 4 &&
            traits::case_insensitive_ex::eq(word[position - 4], common_lang_constants::LOWER_T) &&
            (traits::case_insensitive_ex::eq(word[position - 3], common_lang_constants::LOWER_A) ||
             traits::case_insensitive_ex::eq(word[position - 3], common_lang_constants::LOWER_E)) &&
            traits::case_insensitive_ex::eq(word[position - 2], common_lang_constants::LOWER_N) &&
            traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_T))
            {
            return true;
            }
        if (position == 3 && does_prefix_silence_e(word, position))
            {
            return true;
            }
        if (position >= 2 && does_prefix_disconnect_e(word, position))
            {
            return false;
            }
        // gobbledEgook
        if (position >= 5 &&
            traits::case_insensitive_ex::eq(word[position - 5], common_lang_constants::LOWER_B) &&
            traits::case_insensitive_ex::eq(word[position - 4], common_lang_constants::LOWER_B) &&
            traits::case_insensitive_ex::eq(word[position - 3], common_lang_constants::LOWER_L) &&
            traits::case_insensitive_ex::eq(word[position - 2], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_D))
            {
            return false;
            }
        // medical terms with "ane" prefix (anemia)
        if (m_length >= 4 && position == 2 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_A) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_N))
            {
            return false;
            }
        // somE...
        if (position == 3 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_M))
            {
            // somErsault, 'er' is separate sound
            return m_length < 5 ||
                   !traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_R);
            // otherwise, it is somEbody or somEtimes
            }
        // special case for "graphed"
        if ((position + 1 < m_length) && position > 3 &&
            traits::case_insensitive_ex::eq(word[position - 3], common_lang_constants::LOWER_A) &&
            traits::case_insensitive_ex::eq(word[position - 2], common_lang_constants::LOWER_P) &&
            traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_H) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_D))
            {
            return true;
            }
        // make sure not following separable consonants
        if (!can_consonants_be_modified_by_following_e(word + m_previous_vowel + 1,
                                                       (position - 1) - m_previous_vowel))
            {
            return false;
            }
        // verify that the following consonants can start a new syllable. if not,
        // then this 'e' would have to be connected to the first following consonant
        if ((position + 2 < m_length))
            {
            if (nextVowel != m_length &&
                !can_consonants_begin_sound(word + position + 1, (nextVowel - 1) - position))
                {
                return false;
                }
            }

        // uneven, unearthed, decelerate
        if (position == 2 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_U) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_N))
            {
            return false;
            }
        // unde always splits
        if (position == 3 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_U) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_N) &&
            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_D))
            {
            return false;
            }
        // alchemist
        if (position == 4 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_A) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_L) &&
            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_C) &&
            traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_H))
            {
            return false;
            }
        // un-der-e always splits
        if (position == 5 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_U) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_N) &&
            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_D) &&
            traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_R))
            {
            return false;
            }
        // horse... and house... also silent
        if (position == 4 && position < (m_length - 2) &&
            // past
            ((traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_S) &&
              traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_T)) ||
             // peac
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_C)) ||
             // hors
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_H) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_R) &&
              traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_S)) ||
             // hous
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_H) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_U) &&
              traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_S)) ||
             // ston
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_O) &&
              traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_N)) ||
             // stal
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_L)) ||
             // spac
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_P) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_C)) ||
             // skat
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_K) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_T)) ||
             // shak
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_H) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_K)) ||
             // shap
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_H) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_P)) ||
             // wher
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_W) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_H) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_R)) ||
             // valu
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_V) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_L) &&
              traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_U)) ||
             // shak
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_H) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_K)) ||
             // ther
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_H) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_R)) ||
             // stov
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_O) &&
              traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_V)) ||
             // trad
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_R) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_D))))
            {
            return true;
            }
        if (position == 4 && position < (m_length - 2) &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_L) &&
            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_A) &&
            traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_C))
            {
            // except for "placebo"
            return position >= (m_length - 2) ||
                   !traits::case_insensitive_ex::eq(word[position + 1],
                                                    common_lang_constants::LOWER_B) ||
                   !traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_O);
            }

        /// life... is always silent
        if (position == 3 && position < (m_length - 2) &&
            ((traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_L) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_F)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_L) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_V)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_W) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_R)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_R)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_M)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_D)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_F) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_R)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_D) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_R)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_V)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_F) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_L)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_H) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_D)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_H) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_M)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_N) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_M)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_N) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_N)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_N) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_S)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_L) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_N)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_Y) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_U) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_L)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_W) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_N)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_V) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_C)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_C)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_D) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_V)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_K))))
            {
            return true;
            }
        // esp-e, int-er, wat-er, re-de-fine, re-fer, re-new
        if (position == 3 &&
            ((traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_S) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_P)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_N) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_T)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_W) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_T)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_U) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_P)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_D) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_M)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_D) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_R)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_D) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_S)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_D) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_T)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_D) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_V)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_V) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_H)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_V) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_N)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_D)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_R)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_N) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_V)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_R) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_D)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_R) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_F)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_R) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_N)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_N) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_S))))
            {
            return false;
            }
        // pre-re-cord, pre-de-cease, pro-test, ver-te-brae
        if (position == 4 && m_length > 5 &&
            ((traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_R) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_E) &&
              (traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_R) ||
               traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_D))) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_R) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_O) &&
              traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_T)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_V) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_R) &&
              traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_T)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_R) &&
              traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_P)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_R) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_V)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_R) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_P)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_N) &&
              traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_D))))
            {
            return false;
            }
        // nickelodeon
        if (position == 4 && m_length > 5 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_N) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_C) &&
            traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_K) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_L))
            {
            return false;
            }
        if (position == 4 && m_length > 5 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_T) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_M) &&
            traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_P))
            {
            // exception: temp
            return m_length == 6 && traits::case_insensitive_ex::eq(word[position + 1],
                                                                    common_lang_constants::LOWER_D);
            }

        if (position == 4 && m_length > 5 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_R) &&
            traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_M))
            {
            // exception: permed
            return m_length == 6 && traits::case_insensitive_ex::eq(word[m_length - 1],
                                                                    common_lang_constants::LOWER_D);
            }

        // int-er-est
        if (position == 5 &&
            ((traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_N) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_R)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_T) &&
              traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_R) &&
              traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_T))))
            {
            return false;
            }
        // ice.. always silent
        if (position == 2 && position < (m_length - 2) &&
            ((traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_I) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_C)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_E) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_Y))))
            {
            return true;
            }
        // ine... and ite... 'e' is always new sound
        if (position == 2 &&
            ((traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_I) &&
              (traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_N) ||
               traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_T))) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_E) &&
              (traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_L) ||
               traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_X))) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_A) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_D)) ||
             (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_O) &&
              traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_B))))
            {
            return false;
            }
        if (position == 3 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_H) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_R))
            {
            if (m_length > 5)
                {
                // here-after
                if (characters::is_character::is_vowel(word[position + 1]))
                    {
                    return true;
                    }
                // here-by
                if (position + 2 < m_length &&
                    ((traits::case_insensitive_ex::eq(word[position + 1],
                                                      common_lang_constants::LOWER_B) &&
                      traits::case_insensitive_ex::eq(word[position + 2],
                                                      common_lang_constants::LOWER_Y)) ||
                     (traits::case_insensitive_ex::eq(word[position + 1],
                                                      common_lang_constants::LOWER_T) &&
                      traits::case_insensitive_ex::eq(word[position + 2],
                                                      common_lang_constants::LOWER_O))))
                    {
                    return true;
                    }
                if (position + 4 < m_length &&
                    traits::case_insensitive_ex::eq(word[position + 1],
                                                    common_lang_constants::LOWER_W) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position + 3],
                                                    common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(word[position + 4],
                                                    common_lang_constants::LOWER_H))
                    {
                    return true;
                    }
                if (position + 6 < m_length &&
                    traits::case_insensitive_ex::eq(word[position + 1],
                                                    common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(word[position + 3],
                                                    common_lang_constants::LOWER_F) &&
                    traits::case_insensitive_ex::eq(word[position + 4],
                                                    common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(word[position + 5],
                                                    common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(word[position + 6],
                                                    common_lang_constants::LOWER_E))
                    {
                    return true;
                    }
                // her-e-sy
                return false;
                }
            }

        if ((m_length - (position + 1)) >= 2)
            {
            if (is_none_affecting_suffix(word + (position + 1), m_length - (position + 1)))
                {
                return true;
                }
            if (does_suffix_negate_silent_e(word + (position + 1), m_length - (position + 1),
                                            nextVowel - (position + 1)))
                {
                return false;
                }
            }

        // special logic for "elist". The 'e' here is almost never silent, but have to watch out for
        // special cases
        if (position + 4 < m_length &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_L) &&
            traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(word[position + 3], common_lang_constants::LOWER_S) &&
            traits::case_insensitive_ex::eq(word[position + 4], common_lang_constants::LOWER_T))
            {
            // pricelist--'e' is silent
            return position == 4 &&
                   traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
                   traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_R) &&
                   traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_I) &&
                   traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_C);
            }

        // start analyzing
        // [consonant][consonant]e
        if ((position >= 3) && (position - m_previous_vowel) > 2)
            {
            if (!characters::is_character::is_vowel(word[position - 2]) &&
                !traits::case_insensitive_ex::eq(word[position - 1], word[position - 2]))
                {
                // [consonant]re causes a separate syllable
                // acre, area, hatred, sacred
                if (traits::case_insensitive_ex::eq(word[position - 1],
                                                    common_lang_constants::LOWER_R))
                    {
                    return false;
                    }
                // [consonant]le causes a separate syllable
                // able, ible, example
                if (traits::case_insensitive_ex::eq(word[position - 1],
                                                    common_lang_constants::LOWER_L) &&
                    (traits::case_insensitive_ex::eq(word[position - 1],
                                                     common_lang_constants::LOWER_B) ||
                     traits::case_insensitive_ex::eq(word[position - 1],
                                                     common_lang_constants::LOWER_P)))
                    {
                    return false;
                    }
                }
            }

        // EX
        if (m_length >= (position + 2) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_X))
            {
            // cortex
            return false;
            }
        // EZ (only at end of word is this 100% nonsilent)
        if (m_length == (position + 2) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_Z))
            {
            // cortez
            return false;
            }
        // ES
        if ((position + 2) <= m_length &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_S))
            {
            // if it is something like "processes", "ages", "hoses", "hazes": it is a new syllable
            if (traits::case_insensitive_ex::eq(word[position - 1],
                                                common_lang_constants::LOWER_G) ||
                traits::case_insensitive_ex::eq(word[position - 1],
                                                common_lang_constants::LOWER_C) ||
                traits::case_insensitive_ex::eq(word[position - 1],
                                                common_lang_constants::LOWER_X) ||
                traits::case_insensitive_ex::eq(word[position - 1],
                                                common_lang_constants::LOWER_Z) ||
                traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_S))
                {
                return false;
                }
            // ...esia
            if ((position + 3) <= m_length &&
                traits::case_insensitive_ex::eq(word[position + 2],
                                                common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(word[position + 3], common_lang_constants::LOWER_A))
                {
                return false;
                }
            //[vowel]rest[vowel]
            if ((position + 4) <= m_length && position >= 1 &&
                traits::case_insensitive_ex::eq(word[position - 1],
                                                common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[position + 2],
                                                common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(word[position + 3], common_lang_constants::LOWER_E))
                {
                return false;
                }
            // features
            if (position >= 2 && characters::is_character::is_vowel(word[position - 2]))
                {
                return true;
                }
            // clothes, tastes, midinettes
            if (position > 1 &&
                ((traits::case_insensitive_ex::eq(word[position - 2],
                                                  common_lang_constants::LOWER_T) &&
                  traits::case_insensitive_ex::eq(word[position - 1],
                                                  common_lang_constants::LOWER_H)) ||
                 (traits::case_insensitive_ex::eq(word[position - 2],
                                                  common_lang_constants::LOWER_G) &&
                  traits::case_insensitive_ex::eq(word[position - 1],
                                                  common_lang_constants::LOWER_H)) ||
                 (traits::case_insensitive_ex::eq(word[position - 2],
                                                  common_lang_constants::LOWER_S) &&
                  traits::case_insensitive_ex::eq(word[position - 1],
                                                  common_lang_constants::LOWER_T)) ||
                 (traits::case_insensitive_ex::eq(word[position - 2],
                                                  common_lang_constants::LOWER_T) &&
                  traits::case_insensitive_ex::eq(word[position - 1],
                                                  common_lang_constants::LOWER_T)) ||
                 (traits::case_insensitive_ex::eq(word[position - 2],
                                                  common_lang_constants::LOWER_M) &&
                  traits::case_insensitive_ex::eq(word[position - 1],
                                                  common_lang_constants::LOWER_M))))
                {
                return true;
                }
            // halves, elkes ,dwarves
            if (position > 1 && ((traits::case_insensitive_ex::eq(word[position - 2],
                                                                  common_lang_constants::LOWER_L) &&
                                  traits::case_insensitive_ex::eq(
                                      word[position - 1], common_lang_constants::LOWER_V)) ||
                                 (traits::case_insensitive_ex::eq(word[position - 2],
                                                                  common_lang_constants::LOWER_L) &&
                                  traits::case_insensitive_ex::eq(
                                      word[position - 1], common_lang_constants::LOWER_K)) ||
                                 (traits::case_insensitive_ex::eq(word[position - 2],
                                                                  common_lang_constants::LOWER_Q) &&
                                  traits::case_insensitive_ex::eq(
                                      word[position - 1], common_lang_constants::LOWER_U)) ||
                                 (traits::case_insensitive_ex::eq(word[position - 2],
                                                                  common_lang_constants::LOWER_R) &&
                                  traits::case_insensitive_ex::eq(word[position - 1],
                                                                  common_lang_constants::LOWER_V))))
                {
                return true;
                }
            // andes, candescent
            if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position - 1],
                                                common_lang_constants::LOWER_D) &&
                traits::case_insensitive_ex::eq(word[position - 2],
                                                common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(word[position - 3], common_lang_constants::LOWER_A))
                {
                return false;
                }
            // CHES
            if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position - 2],
                                                common_lang_constants::LOWER_C) &&
                traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_H))
                {
                // aches is silent
                if (position == 3 && position + 1 < m_length)
                    {
                    return true;
                    }
                // backaches is silent
                if (position + 2 == m_length && position == 7 &&
                    traits::case_insensitive_ex::eq(word[position - 7],
                                                    common_lang_constants::LOWER_B) &&
                    traits::case_insensitive_ex::eq(word[position - 6],
                                                    common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(word[position - 5],
                                                    common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(word[position - 4],
                                                    common_lang_constants::LOWER_K) &&
                    traits::case_insensitive_ex::eq(word[position - 3],
                                                    common_lang_constants::LOWER_A))
                    {
                    return true;
                    }
                // heartaches is silent
                if (position + 2 == m_length && position == 8 &&
                    traits::case_insensitive_ex::eq(word[position - 8],
                                                    common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(word[position - 7],
                                                    common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position - 6],
                                                    common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(word[position - 5],
                                                    common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(word[position - 4],
                                                    common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(word[position - 3],
                                                    common_lang_constants::LOWER_A))
                    {
                    return true;
                    }
                // earaches is silent
                if (position + 2 == m_length && position == 6 &&
                    traits::case_insensitive_ex::eq(word[position - 6],
                                                    common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position - 5],
                                                    common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(word[position - 4],
                                                    common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(word[position - 3],
                                                    common_lang_constants::LOWER_A))
                    {
                    return true;
                    }
                // peaches, inches
                return false;
                }
            // lashes, ashes
            if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position - 2],
                                                common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_H))
                {
                return false;
                }
            // ben-nes
            if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position - 2],
                                                common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_N))
                {
                return false;
                }
            // ter-rest-tri-al
            if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position - 2],
                                                common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_R))
                {
                return false;
                }
            // con-de-scen-sion
            if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position - 2],
                                                common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_D))
                {
                return false;
                }
            // court-es-y
            if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position - 2],
                                                common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_T))
                {
                return false;
                }
            // mag-nes
            if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position - 2],
                                                common_lang_constants::LOWER_G) &&
                traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_N))
                {
                return false;
                }
            // ar-gues
            if (position >= 4 &&
                traits::case_insensitive_ex::eq(word[position - 3],
                                                common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[position - 2],
                                                common_lang_constants::LOWER_G) &&
                traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_U))
                {
                return false;
                }
            // a-gues
            if (position == 3 &&
                traits::case_insensitive_ex::eq(word[position - 3],
                                                common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[position - 2],
                                                common_lang_constants::LOWER_G) &&
                traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_U))
                {
                return false;
                }
            // a-chill-es
            if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position - 3],
                                                common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(word[position - 2],
                                                common_lang_constants::LOWER_L) &&
                traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_L))
                {
                return false;
                }
            // bur-lesqu
            if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position - 2],
                                                common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_L))
                {
                return false;
                }
            // mea-sles
            if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position - 2],
                                                common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_L))
                {
                return false;
                }
            // par-mes-an
            if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position - 2],
                                                common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_M))
                {
                return false;
                }
            return true;
            }
        // ER
        if (m_length >= (position + 2) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_R))
            {
            return false;
            }
        // CHEMI
        if (m_length >= (position + 3) && position >= 2 &&
            traits::case_insensitive_ex::eq(word[position - 2], common_lang_constants::LOWER_C) &&
            traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_H) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_M) &&
            traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_I))
            {
            return false;
            }
        // EP (door-step)
        if (m_length == (position + 2) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_P))
            {
            return false;
            }
        // EPS (door-steps)
        if (m_length == (position + 3) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_P) &&
            traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_S))
            {
            return false;
            }
        // EN (heath-en, chos-en)
        if (m_length >= (position + 2) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_N))
            {
            return false;
            }
        // EL
        if (m_length >= (position + 2) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_L))
            {
            // lovely
            if (traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_Y))
                {
                return true;
                }
            // travel
            if (position + 2 == m_length)
                {
                return false;
                }
            // ape-look (two consecutive vowels cannot start syllable)
            if ((position + 3 < m_length) &&
                characters::is_character::is_vowel(word[position + 2]) &&
                characters::is_character::is_vowel(word[position + 3]))
                {
                return true;
                }
            return true;
            }
        // ET
        if (m_length >= (position + 2) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_T))
            {
            if (m_length >= (position + 3))
                {
                // first, check for explicitly known suffixes that can follow a silent 'e'
                if (position + 5 < m_length &&
                    traits::case_insensitive_ex::eq(word[position + 1],
                                                    common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(word[position + 3],
                                                    common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position + 4],
                                                    common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(word[position + 5],
                                                    common_lang_constants::LOWER_G))
                    {
                    return true;
                    }
                if (position + 4 < m_length &&
                    ((traits::case_insensitive_ex::eq(word[position + 1],
                                                      common_lang_constants::LOWER_T) &&
                      traits::case_insensitive_ex::eq(word[position + 2],
                                                      common_lang_constants::LOWER_A) &&
                      traits::case_insensitive_ex::eq(word[position + 3],
                                                      common_lang_constants::LOWER_K) &&
                      traits::case_insensitive_ex::eq(word[position + 4],
                                                      common_lang_constants::LOWER_E)) ||
                     (traits::case_insensitive_ex::eq(word[position + 1],
                                                      common_lang_constants::LOWER_T) &&
                      traits::case_insensitive_ex::eq(word[position + 2],
                                                      common_lang_constants::LOWER_I) &&
                      traits::case_insensitive_ex::eq(word[position + 3],
                                                      common_lang_constants::LOWER_M) &&
                      traits::case_insensitive_ex::eq(word[position + 4],
                                                      common_lang_constants::LOWER_E))))
                    {
                    return true;
                    }
                // "ninety" and "safety" are special cases that set the 'e' to be silent
                if (position >= 3 && m_length >= 6 &&
                    ((traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_N) &&
                      traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_I) &&
                      traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_N) &&
                      traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_E) &&
                      traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_T) &&
                      traits::case_insensitive_ex::eq(word[5], common_lang_constants::LOWER_Y)) ||
                     (traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
                      traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_A) &&
                      traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_F) &&
                      traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_E) &&
                      traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_T) &&
                      traits::case_insensitive_ex::eq(word[5], common_lang_constants::LOWER_Y))))
                    {
                    return true;
                    }
                // verify that it is not something like "someEthing" or "etr"
                if (nextVowel != m_length && (nextVowel - 2) > position)
                    {
                    return true;
                    }
                return false;
                }
            // puppEt, comEth
            return false;
            }
        // ED
        if (m_length >= (position + 2) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_D))
            {
            // if "ted" and "ded" then 'ed' is a new syllable
            //(e.g., "exited", "refuted", "acted", "embedded")
            if (traits::case_insensitive_ex::eq(word[position - 1],
                                                common_lang_constants::LOWER_T) ||
                traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_D))
                {
                return false;
                }
            // boredom
            if (position + 3 < m_length &&
                traits::case_insensitive_ex::eq(word[position + 1],
                                                common_lang_constants::LOWER_D) &&
                traits::case_insensitive_ex::eq(word[position + 2],
                                                common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[position + 3], common_lang_constants::LOWER_M))
                {
                return true;
                }
            // tragedy
            if (traits::case_insensitive_ex::eq(word[position + 2],
                                                common_lang_constants::LOWER_Y) ||
                traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_I))
                {
                return false;
                }
            if (m_length >= (position + 3))
                {
                // verify that it is not something like "edr"
                return nextVowel != m_length && (nextVowel - 2) > position;
                }
            // something like "raked"
            return true;
            }
        // E and various consonants at end of word
        if (position + 2 == m_length &&
            (traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_B) ||
             traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_C) ||
             traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_K) ||
             traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_M) ||
             traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_N) ||
             traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_W) ||
             traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_Y)))
            {
            return false;
            }
        /// E consonant O at end of word
        /// toledo, imported words (e.g. Italian words)
        if (position + 3 == m_length && !characters::is_character::is_vowel(word[position + 1]) &&
            traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_O))
            {
            return false;
            }
        if (position >= 4)
            {
            if (traits::case_insensitive_ex::eq(word[position - 4],
                                                common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[position - 3],
                                                common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[position - 2],
                                                common_lang_constants::LOWER_G) &&
                traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_U))
                {
                return false;
                }
            }
        return true;
        }

    //----------------------------------------------
    bool
    english_syllabize::is_vowels_separate_syllables(const wchar_t* word, const size_t position,
                                                    const size_t vowel_block_size,
                                                    const bool is_first_vowel_block_in_word) const
        {
        assert(word);
        // queue
        if (vowel_block_size == 3 &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_U) &&
            traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_E))
            {
            return false;
            }
        // re-unification, re-use, but not 'reu-ters'
        if (vowel_block_size == 2 && position == 1 && m_length >= 4 &&
            traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_R) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_U) &&
            (traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_N) ||
             traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_S)))
            {
            return true;
            }
        // nious will always split
        if (vowel_block_size == 3 && position > 0 && m_length >= 5 &&
            traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_N) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_O) &&
            traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_U) &&
            traits::case_insensitive_ex::eq(word[position + 3], common_lang_constants::LOWER_S))
            {
            return true;
            }
        // se-er
        if (position > 0 && (position <= m_length - 3) && (vowel_block_size == 2) &&
            traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_S) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_R))
            {
            return true;
            }
        if (vowel_block_size == 2 &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_A) &&
            traits::case_insensitive_ex::eq(word[position + 1],
                                            common_lang_constants::LOWER_I_UMLAUTS))
            {
            return true;
            }
        // Check for vowel followed by "ism"--this always splits
        // truism, heroism
        if (vowel_block_size == 2 && position + 3 < m_length &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_S) &&
            traits::case_insensitive_ex::eq(word[position + 3], common_lang_constants::LOWER_M))
            {
            return true;
            }
        // Check for vowel followed by "ist"--this always splits
        // egoist
        if (m_syllable_count >= 1 && vowel_block_size == 2 && position + 3 < m_length &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_S) &&
            traits::case_insensitive_ex::eq(word[position + 3], common_lang_constants::LOWER_T))
            {
            // exception: shirt-waist
            return position <= 0 ||
                   !traits::case_insensitive_ex::eq(word[position - 1],
                                                    common_lang_constants::LOWER_W) ||
                   !traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_A);
            }
        // theist
        if (vowel_block_size == 2 && position + 3 < m_length && position == 2 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_T) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_H) &&
            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_S) &&
            traits::case_insensitive_ex::eq(word[5], common_lang_constants::LOWER_T))
            {
            // exception: shirt-waist
            return position <= 0 ||
                   !traits::case_insensitive_ex::eq(word[position - 1],
                                                    common_lang_constants::LOWER_W) ||
                   !traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_A);
            }
        // Check for vowel followed by "ic"--this always splits
        // stoic, heroic
        if (vowel_block_size == 2 && position + 2 < m_length &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_C) &&
            (position + 3 == m_length ||
             !traits::case_insensitive_ex::eq(word[position + 3], common_lang_constants::LOWER_E)))
            {
            // juice and sluice are exceptions
            return !traits::case_insensitive_ex::eq(word[position],
                                                    common_lang_constants::LOWER_U) ||
                   position <= 0 ||
                   (!traits::case_insensitive_ex::eq(word[position - 1],
                                                     common_lang_constants::LOWER_J) &&
                    !traits::case_insensitive_ex::eq(word[position - 1],
                                                     common_lang_constants::LOWER_L));
            }
        // [vowel]ing always splits
        if (vowel_block_size == 2 && position + 3 < m_length &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_N) &&
            traits::case_insensitive_ex::eq(word[position + 3], common_lang_constants::LOWER_G))
            {
            return true;
            }
        // poly[vowel] always splits
        if (vowel_block_size >= 2 && position == 3 &&
            traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
            traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
            traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_L) &&
            traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_Y))
            {
            return true;
            }
        // eex always splits
        if (vowel_block_size == 2 && position + 2 < m_length &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_X))
            {
            return true;
            }
        // ooe always splits
        if (vowel_block_size == 2 && position + 2 < m_length &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_O) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_O) &&
            traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_E))
            {
            return true;
            }
        // fry-er, fly-er
        if (vowel_block_size == 2 && position >= 2 &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_Y) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_E) &&
            (traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_L) ||
             traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_R)))
            {
            return true;
            }

        size_t nextVowel{ 0 };
        // start from last position and backtrack
        for (nextVowel = (position + vowel_block_size); nextVowel < m_length; ++nextVowel)
            {
            /* found another vowel before end of word,
               so this 'e' is still a candidate*/
            if (characters::is_character::is_vowel(word[nextVowel]))
                {
                break;
                }
            }
        if (vowel_block_size > 2)
            {
            // beau, bourgeois, adieu, ciao
            if (vowel_block_size == 3 &&
                ((traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
                  traits::case_insensitive_ex::eq(word[position + 1],
                                                  common_lang_constants::LOWER_A) &&
                  traits::case_insensitive_ex::eq(word[position + 2],
                                                  common_lang_constants::LOWER_U)) ||
                 (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
                  traits::case_insensitive_ex::eq(word[position + 1],
                                                  common_lang_constants::LOWER_O) &&
                  traits::case_insensitive_ex::eq(word[position + 2],
                                                  common_lang_constants::LOWER_I)) ||
                 (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
                  traits::case_insensitive_ex::eq(word[position + 1],
                                                  common_lang_constants::LOWER_A) &&
                  traits::case_insensitive_ex::eq(word[position + 2],
                                                  common_lang_constants::LOWER_O)) ||
                 (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
                  traits::case_insensitive_ex::eq(word[position + 1],
                                                  common_lang_constants::LOWER_E) &&
                  traits::case_insensitive_ex::eq(word[position + 2],
                                                  common_lang_constants::LOWER_U)) ||
                 (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_O) &&
                  traits::case_insensitive_ex::eq(word[position + 1],
                                                  common_lang_constants::LOWER_O) &&
                  traits::case_insensitive_ex::eq(word[position + 2],
                                                  common_lang_constants::LOWER_E))))
                {
                return false;
                }
            if (vowel_block_size == 3 &&
                traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[position + 1],
                                                common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_U))
                {
                // herbaceous, advantageous, righteous
                return position < 1 ||
                       (!traits::case_insensitive_ex::eq(word[position - 1],
                                                         common_lang_constants::LOWER_C) &&
                        !traits::case_insensitive_ex::eq(word[position - 1],
                                                         common_lang_constants::LOWER_G) &&
                        !traits::case_insensitive_ex::eq(word[position - 1],
                                                         common_lang_constants::LOWER_R));
                }
            if (vowel_block_size == 3 &&
                traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(word[position + 1],
                                                common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_U))
                {
                // superCili-ous, puncTili-ous
                if (position >= 3 &&
                    (traits::case_insensitive_ex::eq(word[position - 3],
                                                     common_lang_constants::LOWER_C) ||
                     traits::case_insensitive_ex::eq(word[position - 3],
                                                     common_lang_constants::LOWER_T)) &&
                    traits::case_insensitive_ex::eq(word[position - 2],
                                                    common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position - 1],
                                                    common_lang_constants::LOWER_L))
                    {
                    return true;
                    }
                // re-li-gious, capricious, obnoxious, ambitious, bilious
                if (position > 0 &&
                    (traits::case_insensitive_ex::eq(word[position - 1],
                                                     common_lang_constants::LOWER_C) ||
                     traits::case_insensitive_ex::eq(word[position - 1],
                                                     common_lang_constants::LOWER_G) ||
                     traits::case_insensitive_ex::eq(word[position - 1],
                                                     common_lang_constants::LOWER_L) ||
                     traits::case_insensitive_ex::eq(word[position - 1],
                                                     common_lang_constants::LOWER_T) ||
                     traits::case_insensitive_ex::eq(word[position - 1],
                                                     common_lang_constants::LOWER_X)))
                    {
                    return false;
                    }
                // no-tor-i-ous
                return true;
                }
            // any other combination probably splits
            return true;
            }
        // chaos
        if ((position + 1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_A) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_O))
            {
            // exception: gaol
            return !(position == 1 && m_length >= 4 &&
                     traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_G) &&
                     traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_L));
            }
        // coliseum
        if (position + 3 <= m_length &&
            (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
             traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_U)) &&
            m_syllable_count >= 1 /* Zeus*/ &&
            (traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_M) ||
             (traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_S) &&
              // diseuse
              !traits::case_insensitive_ex::eq(word[position - 1],
                                               common_lang_constants::LOWER_S))))
            {
            return true;
            }
        // boa, boat
        if ((position + 1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_O) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_A))
            {
            // oar, oak
            if (position == 0)
                {
                // special case for oasis
                return m_length > 3 &&
                       traits::case_insensitive_ex::eq(word[position + 2],
                                                       common_lang_constants::LOWER_S) &&
                       traits::case_insensitive_ex::eq(word[position + 3],
                                                       common_lang_constants::LOWER_I);
                }
            // first chance logic for special case "coa[consonant]"
            if (position >= 1 &&
                traits::case_insensitive_ex::eq(word[position - 1],
                                                common_lang_constants::LOWER_C) &&
                position + 3 <= m_length)
                {
                // coagulate, coadjutant
                if (traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_D) ||
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_G))
                    {
                    return true;
                    }
                // coacervate, coacH
                if ((position + 3 == m_length ||
                     !traits::case_insensitive_ex::eq(word[position + 3],
                                                      common_lang_constants::LOWER_H)) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_C))
                    {
                    return true;
                    }
                // coalesce, coal (coaler, coaled)
                if (position + 4 <= m_length &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_L) &&
                    characters::is_character::is_vowel(word[position + 3]))
                    {
                    return position + 5 > m_length ||
                           !traits::case_insensitive_ex::eq(word[position + 3],
                                                            common_lang_constants::LOWER_E) ||
                           (!traits::case_insensitive_ex::eq(word[position + 4],
                                                             common_lang_constants::LOWER_R) &&
                            !traits::case_insensitive_ex::eq(word[position + 4],
                                                             common_lang_constants::LOWER_D));
                    }
                }

            // jOAnne
            if (position + 3 <= m_length &&
                traits::case_insensitive_ex::eq(word[position + 1],
                                                common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(word[position + 2],
                                                common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(word[position + 3], common_lang_constants::LOWER_N))
                {
                return true;
                }

            // if at the end of the word then they split (boa)
            if (position + 2 == m_length)
                {
                // "cocoa" is and exception
                return !(m_length == 5 &&
                         traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_C) &&
                         traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
                         traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_C));
                }
            // psychoa splots
            if (position == 5 &&
                traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_Y) &&
                traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_C) &&
                traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_H))
                {
                return true;
                }
            // ...otherwise it's one sound
            return false;
            }
        // duet, issuer, fuel
        if ((position + 1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_U) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_E))
            {
            /// @bug Puert-o Rico is broken.
            if ((position + 2) < m_length &&
                (traits::case_insensitive_ex::eq(word[position + 2],
                                                 common_lang_constants::LOWER_T) ||
                 traits::case_insensitive_ex::eq(word[position + 2],
                                                 common_lang_constants::LOWER_R) ||
                 traits::case_insensitive_ex::eq(word[position + 2],
                                                 common_lang_constants::LOWER_L)))
                {
                return true;
                }
            if ((position + 3) < m_length)
                {
                // affluence, fluent
                if (traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_N) &&
                    (traits::case_insensitive_ex::eq(word[position + 3],
                                                     common_lang_constants::LOWER_C) ||
                     traits::case_insensitive_ex::eq(word[position + 3],
                                                     common_lang_constants::LOWER_T)))
                    {
                    return true;
                    }
                // bluest
                if (traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(word[position + 3],
                                                    common_lang_constants::LOWER_T))
                    {
                    return true;
                    }
                return false;
                }
            return false;
            }
        // simplifying
        if ((position + 1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_Y) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_I))
            {
            return true;
            }
        // OI
        if ((position + 1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_O) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_I))
            {
            // O-IZE
            if ((position + 3 < m_length) &&
                traits::case_insensitive_ex::eq(word[position + 2],
                                                common_lang_constants::LOWER_Z) &&
                traits::case_insensitive_ex::eq(word[position + 3], common_lang_constants::LOWER_E))
                {
                return true;
                }
            // co-in-cide
            if ((position + 3 < m_length) && (position >= 1) &&
                traits::case_insensitive_ex::eq(word[position - 1],
                                                common_lang_constants::LOWER_C) &&
                traits::case_insensitive_ex::eq(word[position + 2],
                                                common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(word[position + 3], common_lang_constants::LOWER_C))
                {
                return true;
                }
            // anything else is like "an-droid"  or "join"
            return false;
            }
        // rye, goodbye
        if ((position + 1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_Y) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_E))
            {
            // dryer
            return position + 3 == m_length &&
                   traits::case_insensitive_ex::eq(word[position + 2],
                                                   common_lang_constants::LOWER_R);
            }
        // bryan
        if (traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_Y))
            {
            return true;
            }
        // IO
        if ((position + 1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_O))
            {
            // if at the end of the word then always split (e.g., bio, radio, io)
            if (position + 2 == m_length ||
                // ...or this "io" is the only vowel block in the word.
                // (e.g, lion, dion, pion, scion)
                (m_previous_vowel == m_length))
                {
                return true;
                }
            if ((position > 0) && ((position + 3) < m_length) &&
                (traits::case_insensitive_ex::eq(word[position - 1],
                                                 common_lang_constants::LOWER_C) &&
                 traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
                 traits::case_insensitive_ex::eq(word[position + 1],
                                                 common_lang_constants::LOWER_O) &&
                 traits::case_insensitive_ex::eq(word[position + 2],
                                                 common_lang_constants::LOWER_U) &&
                 traits::case_insensitive_ex::eq(word[position + 3],
                                                 common_lang_constants::LOWER_S)))
                {
                return false;
                }
            // legion, nation, union
            if (position > 0 && position + 2 < m_length &&
                traits::case_insensitive_ex::eq(word[position + 2],
                                                common_lang_constants::LOWER_N) &&
                (traits::case_insensitive_ex::eq(word[position - 1],
                                                 common_lang_constants::LOWER_L) ||
                 traits::case_insensitive_ex::eq(word[position - 1],
                                                 common_lang_constants::LOWER_N) ||
                 traits::case_insensitive_ex::eq(word[position - 1],
                                                 common_lang_constants::LOWER_S) ||
                 traits::case_insensitive_ex::eq(word[position - 1],
                                                 common_lang_constants::LOWER_G) ||
                 traits::case_insensitive_ex::eq(word[position - 1],
                                                 common_lang_constants::LOWER_T) ||
                 traits::case_insensitive_ex::eq(word[position - 1],
                                                 common_lang_constants::LOWER_X)))
                {
                return false;
                }
            // coerCION does not split, but acfiCIONado does
            if (position > 0 && position + 2 < m_length &&
                traits::case_insensitive_ex::eq(word[position + 2],
                                                common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(word[position - 1],
                                                common_lang_constants::LOWER_C) &&
                (position + 3 == m_length ||
                 !traits::case_insensitive_ex::eq(word[position + 3],
                                                  common_lang_constants::LOWER_A)))
                {
                return false;
                }
            // fashion
            if ((position > 1) && ((position + 2) < m_length) &&
                ((traits::case_insensitive_ex::eq(word[position - 2],
                                                  common_lang_constants::LOWER_S) &&
                  traits::case_insensitive_ex::eq(word[position - 1],
                                                  common_lang_constants::LOWER_H) &&
                  traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
                  traits::case_insensitive_ex::eq(word[position + 1],
                                                  common_lang_constants::LOWER_O) &&
                  traits::case_insensitive_ex::eq(word[position + 2],
                                                  common_lang_constants::LOWER_N)) ||
                 (traits::case_insensitive_ex::eq(word[position - 2],
                                                  common_lang_constants::LOWER_C) &&
                  traits::case_insensitive_ex::eq(word[position - 1],
                                                  common_lang_constants::LOWER_H) &&
                  traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
                  traits::case_insensitive_ex::eq(word[position + 1],
                                                  common_lang_constants::LOWER_O) &&
                  traits::case_insensitive_ex::eq(word[position + 2],
                                                  common_lang_constants::LOWER_N))))
                {
                return false;
                }
            return true;
            }
        // IA
        if ((position + 1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_A))
            {
            // [consonant]ia[consonant]
            if ((position > 0) && (position + 2 < m_length))
                {
                // partial
                if (traits::case_insensitive_ex::eq(word[position - 1],
                                                    common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_L))
                    {
                    return false;
                    }
                // christI-ANIty
                if ((position + 3 < m_length) &&
                    traits::case_insensitive_ex::eq(word[position - 1],
                                                    common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_N) &&
                    (traits::case_insensitive_ex::eq(word[position + 3],
                                                     common_lang_constants::LOWER_I) ||
                     traits::case_insensitive_ex::eq(word[position + 3],
                                                     common_lang_constants::LOWER_T)))
                    {
                    return true;
                    }
                // christian
                if (traits::case_insensitive_ex::eq(word[position - 1],
                                                    common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_N))
                    {
                    return false;
                    }
                // technician, special
                if (traits::case_insensitive_ex::eq(word[position - 1],
                                                    common_lang_constants::LOWER_C) &&
                    (traits::case_insensitive_ex::eq(word[position + 2],
                                                     common_lang_constants::LOWER_N) ||
                     traits::case_insensitive_ex::eq(word[position + 2],
                                                     common_lang_constants::LOWER_L)))
                    {
                    return false;
                    }
                // cartesian
                if (traits::case_insensitive_ex::eq(word[position - 1],
                                                    common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_N))
                    {
                    return false;
                    }
                // carnaSSI-AL
                if ((position >= 2) &&
                    traits::case_insensitive_ex::eq(word[position - 2],
                                                    common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(word[position - 1],
                                                    common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_L))
                    {
                    return true;
                    }
                // controver-SIAL
                if (traits::case_insensitive_ex::eq(word[position - 1],
                                                    common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_L))
                    {
                    return false;
                    }
                // marriage
                if (position >= 3 &&
                    traits::case_insensitive_ex::eq(word[position - 3],
                                                    common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(word[position - 2],
                                                    common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(word[position - 1],
                                                    common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_G))
                    {
                    return false;
                    }
                // co-lle-giate, pla-giar-ize
                if (traits::case_insensitive_ex::eq(word[position - 1],
                                                    common_lang_constants::LOWER_G) &&
                    (traits::case_insensitive_ex::eq(word[position + 2],
                                                     common_lang_constants::LOWER_T) ||
                     traits::case_insensitive_ex::eq(word[position + 2],
                                                     common_lang_constants::LOWER_R)))
                    {
                    if (position == 1)
                        {
                        return true;
                        }
                    if (position >= 2 && traits::case_insensitive_ex::eq(
                                             word[position - 2], common_lang_constants::LOWER_G))
                        {
                        return true;
                        }
                    return false;
                    }
                // historian
                return true;
                }
            // diabetes
            if (position + 2 < m_length && position > 0 &&
                (traits::case_insensitive_ex::eq(word[position + 2],
                                                 common_lang_constants::LOWER_B)))
                {
                return false;
                }
            ///@bug ammonia is broken
            // fantasia
            if (position + 2 == m_length && position > 0 &&
                traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_S))
                {
                return false;
                }
            // dial, pia

            return true;
            }
        // EE
        if ((position + 1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_E))
            {
            ///@bug seer is broken
            return false;
            }
        // EI
        if ((position + 1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_I))
            {
            // reign
            if (position + 3 < m_length &&
                traits::case_insensitive_ex::eq(word[position + 2],
                                                common_lang_constants::LOWER_G) &&
                traits::case_insensitive_ex::eq(word[position + 3], common_lang_constants::LOWER_N))
                {
                return false;
                }
            // cor-por-e-it-y
            if ((position == m_length - 4) &&
                traits::case_insensitive_ex::eq(word[position + 2],
                                                common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(word[position + 3], common_lang_constants::LOWER_Y))
                {
                return true;
                }
            if (position == 1)
                {
                // reintroduce
                if (traits::case_insensitive_ex::eq(word[position - 1],
                                                    common_lang_constants::LOWER_R))
                    {
                    return true;
                    }
                // deity, deirdre
                if (position + 3 < m_length &&
                    traits::case_insensitive_ex::eq(word[position - 1],
                                                    common_lang_constants::LOWER_D) &&
                    !traits::case_insensitive_ex::eq(word[position + 2],
                                                     common_lang_constants::LOWER_R))
                    {
                    // watch out for deify. "deify" splits, but some other forms don't
                    if (traits::case_insensitive_ex::eq(word[position + 2],
                                                        common_lang_constants::LOWER_F))
                        {
                        return traits::case_insensitive_ex::eq(word[position + 3],
                                                               common_lang_constants::LOWER_Y) ||
                               (position + 4 < m_length &&
                                traits::case_insensitive_ex::eq(word[position + 3],
                                                                common_lang_constants::LOWER_I) &&
                                traits::case_insensitive_ex::eq(word[position + 4],
                                                                common_lang_constants::LOWER_C));
                        }
                    return true;
                    }
                return false;
                }
            return false;
            }
        if ((position + 1 < m_length) &&
            traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(word[position + 1], common_lang_constants::LOWER_A))
            {
            // EA[letter][letter]...
            if (position == 0)
                {
                // eaten (only a consonant in front can cause a split)
                return false;
                }
            // i-de-a, i-de-al
            if (position == 2 &&
                traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_D))
                {
                return true;
                }
            // pre-arr-ange, pre-am-ble
            if (position == 2 && m_length >= 6 &&
                traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) &&
                traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_R) &&
                (traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_R) ||
                 traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_M) ||
                 traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_G)))
                {
                return true;
                }
            // EA at end of word
            if (position + 2 == m_length)
                {
                // plea -> false
                // achillea, nausea -> true
                return m_syllable_count != 0;
                }
            // EA[letter]...
            if (position + 3 == m_length ||
                (position + 4 == m_length &&
                 traits::case_insensitive_ex::eq(word[position + 3],
                                                 common_lang_constants::LOWER_S)))
                {
                // EAN
                if (traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_N))
                    {
                    // If "ea" is the first vowels then it is one sound
                    // bean
                    if (m_previous_block_vowel == m_length)
                        {
                        return false;
                        }
                    // boolean
                    if (position >= 3 &&
                        traits::case_insensitive_ex::eq(word[position - 3],
                                                        common_lang_constants::LOWER_O) &&
                        traits::case_insensitive_ex::eq(word[position - 2],
                                                        common_lang_constants::LOWER_O) &&
                        traits::case_insensitive_ex::eq(word[position - 1],
                                                        common_lang_constants::LOWER_L))
                        {
                        return true;
                        }
                    // aegean
                    if (position >= 1 && traits::case_insensitive_ex::eq(
                                             word[position - 1], common_lang_constants::LOWER_G))
                        {
                        return true;
                        }
                    // ocean
                    if (m_syllable_count == 1 &&
                        (m_previous_block_vowel == position - 2 ||
                         can_consonants_begin_sound(word + (m_previous_block_vowel + 1),
                                                    position - (m_previous_block_vowel + 1))))
                        {
                        return false;
                        }
                    // demean, crustacean
                    if (position >= 1 && (traits::case_insensitive_ex::eq(
                                              word[position - 1], common_lang_constants::LOWER_M) ||
                                          traits::case_insensitive_ex::eq(
                                              word[position - 1], common_lang_constants::LOWER_C)))
                        {
                        return false;
                        }
                    // soybean
                    if (position >= 1 && traits::case_insensitive_ex::eq(
                                             word[position - 1], common_lang_constants::LOWER_B))
                        {
                        // exception: Caribbean
                        return position >= 2 &&
                               traits::case_insensitive_ex::eq(word[position - 2],
                                                               common_lang_constants::LOWER_B);
                        }
                    // cyclopean
                    if (position >= 1 && traits::case_insensitive_ex::eq(
                                             word[position - 1], common_lang_constants::LOWER_P))
                        {
                        return true;
                        }
                    // korean
                    if (position >= 1 && traits::case_insensitive_ex::eq(
                                             word[position - 1], common_lang_constants::LOWER_R))
                        {
                        return true;
                        }
                    // european
                    return true;
                    }
                // surreal, cereal, real
                if ((position > 1) &&
                    traits::case_insensitive_ex::eq(word[position - 1],
                                                    common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_L))
                    {
                    // exception: unreal
                    return position != 3 ||
                           !traits::case_insensitive_ex::eq(word[position - 3],
                                                            common_lang_constants::LOWER_U) ||
                           !traits::case_insensitive_ex::eq(word[position - 2],
                                                            common_lang_constants::LOWER_N);
                    }
                }
            else if (position + 3 < m_length)
                {
                // react
                if (traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(word[position + 3],
                                                    common_lang_constants::LOWER_T))
                    {
                    return true;
                    }
                // miscreant
                if (position >= 2 &&
                    traits::case_insensitive_ex::eq(word[position - 2],
                                                    common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(word[position - 1],
                                                    common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(word[position + 3],
                                                    common_lang_constants::LOWER_T))
                    {
                    return true;
                    }
                // MEAB
                if (position > 0 &&
                    traits::case_insensitive_ex::eq(word[position - 1],
                                                    common_lang_constants::LOWER_M) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_B))
                    {
                    return true;
                    }
                // re-al-i-ty
                if ((position == 1) &&
                    traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_R))
                    {
                    if (traits::case_insensitive_ex::eq(word[position + 2],
                                                        common_lang_constants::LOWER_L))
                        {
                        // always splits (except for "realm")
                        return !(traits::case_insensitive_ex::eq(word[position + 3],
                                                                 common_lang_constants::LOWER_M));
                        }
                    // reappointment
                    if (traits::case_insensitive_ex::eq(word[position + 2],
                                                        common_lang_constants::LOWER_P) &&
                        traits::case_insensitive_ex::eq(word[position + 3],
                                                        common_lang_constants::LOWER_P))
                        {
                        return true;
                        }
                    // rearranged
                    if (traits::case_insensitive_ex::eq(word[position + 2],
                                                        common_lang_constants::LOWER_R) &&
                        traits::case_insensitive_ex::eq(word[position + 3],
                                                        common_lang_constants::LOWER_R))
                        {
                        return true;
                        }
                    return false;
                    }
                // ne-ther-realms
                if (position > 1 && traits::case_insensitive_ex::eq(word[position + 3],
                                                                    common_lang_constants::LOWER_M))
                    {
                    return false;
                    }
                // mile-age
                if (traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_G) &&
                    traits::case_insensitive_ex::eq(word[position + 3],
                                                    common_lang_constants::LOWER_E))
                    {
                    // except for "eag-er"
                    return position + 4 >= m_length ||
                           !traits::case_insensitive_ex::eq(word[position + 4],
                                                            common_lang_constants::LOWER_R);
                    }
                if (traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_T))
                    {
                    // creator
                    if (position > 1 &&
                        traits::case_insensitive_ex::eq(word[position - 2],
                                                        common_lang_constants::LOWER_C) &&
                        traits::case_insensitive_ex::eq(word[position - 1],
                                                        common_lang_constants::LOWER_R))
                        {
                        // "creature" special case
                        return position + 3 >= m_length ||
                               !traits::case_insensitive_ex::eq(word[position + 3],
                                                                common_lang_constants::LOWER_U);
                        }
                    // permeate
                    if (position >= 2 &&
                        traits::case_insensitive_ex::eq(word[position - 2],
                                                        common_lang_constants::LOWER_R) &&
                        traits::case_insensitive_ex::eq(word[position - 1],
                                                        common_lang_constants::LOWER_M) &&
                        position + 3 < m_length &&
                        traits::case_insensitive_ex::eq(word[position + 2],
                                                        common_lang_constants::LOWER_T) &&
                        traits::case_insensitive_ex::eq(word[position + 3],
                                                        common_lang_constants::LOWER_E))
                        {
                        return true;
                        }
                    // theatrical
                    if (position + 4 < m_length &&
                        traits::case_insensitive_ex::eq(word[position + 3],
                                                        common_lang_constants::LOWER_R) &&
                        traits::case_insensitive_ex::eq(word[position + 4],
                                                        common_lang_constants::LOWER_I))
                        {
                        return true;
                        }
                    // eaten
                    return false;
                    }
                if (traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_N))
                    {
                    // oleander, leann
                    return traits::case_insensitive_ex::eq(word[position + 3],
                                                           common_lang_constants::LOWER_D) ||
                           traits::case_insensitive_ex::eq(word[position + 3],
                                                           common_lang_constants::LOWER_N);
                    }
                // surreal, cereal, realize
                if ((position > 1) &&
                    traits::case_insensitive_ex::eq(word[position - 1],
                                                    common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_L))
                    {
                    return true;
                    }
                }
            else
                {
                return false;
                }
            }
        else if ((position + 1 < m_length) &&
                 traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
                 traits::case_insensitive_ex::eq(word[position + 1],
                                                 common_lang_constants::LOWER_U))
            {
            return true;
            }
        // EO
        else if ((position + 1 < m_length) &&
                 traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_E) &&
                 traits::case_insensitive_ex::eq(word[position + 1],
                                                 common_lang_constants::LOWER_O))
            {
            // foreordain
            if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position - 3],
                                                common_lang_constants::LOWER_F) &&
                traits::case_insensitive_ex::eq(word[position - 2],
                                                common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_R))
                {
                return false;
                }
            // george
            if (position == 1 && position + 3 < m_length &&
                traits::case_insensitive_ex::eq(word[position - 1],
                                                common_lang_constants::LOWER_G) &&
                traits::case_insensitive_ex::eq(word[position + 2],
                                                common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(word[position + 3], common_lang_constants::LOWER_G))
                {
                return false;
                }
            // geoff
            if (position == 1 && position + 3 < m_length &&
                traits::case_insensitive_ex::eq(word[position - 1],
                                                common_lang_constants::LOWER_G) &&
                traits::case_insensitive_ex::eq(word[position + 2],
                                                common_lang_constants::LOWER_F) &&
                traits::case_insensitive_ex::eq(word[position + 3], common_lang_constants::LOWER_F))
                {
                return false;
                }
            // where-on
            if (position >= 3 &&
                traits::case_insensitive_ex::eq(word[position - 3],
                                                common_lang_constants::LOWER_H) &&
                traits::case_insensitive_ex::eq(word[position - 2],
                                                common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(word[position - 1], common_lang_constants::LOWER_R))
                {
                return false;
                }
            // bludgeon
            if (position >= 1 && position + 2 < m_length &&
                (traits::case_insensitive_ex::eq(word[position - 1],
                                                 common_lang_constants::LOWER_G) &&
                 traits::case_insensitive_ex::eq(word[position + 2],
                                                 common_lang_constants::LOWER_N)))
                {
                return false;
                }
            // PEO and JEO
            if (position >= 1 && (traits::case_insensitive_ex::eq(word[position - 1],
                                                                  common_lang_constants::LOWER_P) ||
                                  traits::case_insensitive_ex::eq(word[position - 1],
                                                                  common_lang_constants::LOWER_J)))
                {
                // people and jeopardy will not split on the "eo"
                return position + 2 >= m_length ||
                       !traits::case_insensitive_ex::eq(word[position + 2],
                                                        common_lang_constants::LOWER_P);
                // everything else (e.g., "peon" and "Peoria" will split)
                }
            // someone -> false
            // geometric, neon -> true
            return !(position == 3 &&
                     traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
                     traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
                     traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_M));
            }
        // UI
        else if ((position + 1 < m_length) &&
                 traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_U) &&
                 traits::case_insensitive_ex::eq(word[position + 1],
                                                 common_lang_constants::LOWER_I))
            {
            if ((position + 3) <= m_length &&
                traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_T))
                {
                // fruit(s)
                if (position + 3 == m_length ||
                    (position + 4 == m_length &&
                     traits::case_insensitive_ex::eq(word[position + 3],
                                                     common_lang_constants::LOWER_S)))
                    {
                    return false;
                    }
                // acuity
                if (traits::case_insensitive_ex::eq(word[position + 3],
                                                    common_lang_constants::LOWER_Y))
                    {
                    // except "fruity"
                    return position != 2 ||
                           !traits::case_insensitive_ex::eq(word[0],
                                                            common_lang_constants::LOWER_F) ||
                           !traits::case_insensitive_ex::eq(word[1],
                                                            common_lang_constants::LOWER_R);
                    }
                // uities
                if (position + 6 == m_length &&
                    traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(word[position + 3],
                                                    common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position + 4],
                                                    common_lang_constants::LOWER_E) &&
                    traits::case_insensitive_ex::eq(word[position + 5],
                                                    common_lang_constants::LOWER_S))
                    {
                    return true;
                    }
                // intuitive
                if (position + 5 < m_length &&
                    traits::case_insensitive_ex::eq(word[position + 3],
                                                    common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position + 4],
                                                    common_lang_constants::LOWER_V) &&
                    traits::case_insensitive_ex::eq(word[position + 5],
                                                    common_lang_constants::LOWER_E))
                    {
                    return true;
                    }
                // intuition
                if (position + 5 < m_length &&
                    traits::case_insensitive_ex::eq(word[position + 3],
                                                    common_lang_constants::LOWER_I) &&
                    traits::case_insensitive_ex::eq(word[position + 4],
                                                    common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(word[position + 5],
                                                    common_lang_constants::LOWER_N))
                    {
                    return true;
                    }
                // suite, suited
                return false;
                }
            // guide, fluid, ruin
            if ((traits::case_insensitive_ex::eq(word[position + 2],
                                                 common_lang_constants::LOWER_D) ||
                 traits::case_insensitive_ex::eq(word[position + 2],
                                                 common_lang_constants::LOWER_N)) &&
                !is_silent_u(word, position))
                {
                return true;
                }
            // bluish
            if ((position + 4) <= m_length &&
                traits::case_insensitive_ex::eq(word[position + 2],
                                                common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(word[position + 3], common_lang_constants::LOWER_H))
                {
                return true;
                }
            return false;
            }
        // UO
        else if ((position + 1 < m_length) &&
                 traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_U) &&
                 traits::case_insensitive_ex::eq(word[position + 1],
                                                 common_lang_constants::LOWER_O))
            {
            // fluoridate
            return position + 2 >= m_length ||
                   !traits::case_insensitive_ex::eq(word[position + 2],
                                                    common_lang_constants::LOWER_R);
            // duo
            }
        // OO
        else if ((position + 1 < m_length) &&
                 traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_O) &&
                 traits::case_insensitive_ex::eq(word[position + 1],
                                                 common_lang_constants::LOWER_O))
            {
            // zoologist
            if (m_length >= 4 && position == 1 &&
                traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_Z) &&
                (traits::case_insensitive_ex::eq(word[position + 2],
                                                 common_lang_constants::LOWER_G) ||
                 traits::case_insensitive_ex::eq(word[position + 2],
                                                 common_lang_constants::LOWER_L) ||
                 traits::case_insensitive_ex::eq(word[position + 2],
                                                 common_lang_constants::LOWER_P)) &&
                nextVowel != m_length)
                {
                return true;
                }
            // coordinate, coop
            if (m_length > 6 && position == 1 &&
                traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_C) &&
                (traits::case_insensitive_ex::eq(word[position + 2],
                                                 common_lang_constants::LOWER_R) ||
                 traits::case_insensitive_ex::eq(word[position + 2],
                                                 common_lang_constants::LOWER_P) ||
                 traits::case_insensitive_ex::eq(word[position + 2],
                                                 common_lang_constants::LOWER_W)) &&
                nextVowel != m_length)
                {
                return true;
                }
            // coowner
            if (m_length >= 4 && position == 1 &&
                traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_C) &&
                traits::case_insensitive_ex::eq(word[position + 2],
                                                common_lang_constants::LOWER_W) &&
                nextVowel != m_length)
                {
                return true;
                }
            return false;
            }
        // OE
        else if ((position + 1 < m_length) &&
                 traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_O) &&
                 traits::case_insensitive_ex::eq(word[position + 1],
                                                 common_lang_constants::LOWER_E))
            {
            // toe-nail, woe-ful
            if (position == 1 && (traits::case_insensitive_ex::eq(word[position - 1],
                                                                  common_lang_constants::LOWER_T) ||
                                  traits::case_insensitive_ex::eq(word[position - 1],
                                                                  common_lang_constants::LOWER_W)))
                {
                return false;
                }
            // doe, toe
            if (position + 2 == m_length)
                {
                // Chloe is an exception here
                return position >= 3 &&
                       (traits::case_insensitive_ex::eq(word[position - 3],
                                                        common_lang_constants::LOWER_C) &&
                        traits::case_insensitive_ex::eq(word[position - 2],
                                                        common_lang_constants::LOWER_H) &&
                        traits::case_insensitive_ex::eq(word[position - 1],
                                                        common_lang_constants::LOWER_L));
                }
            // does, hoed
            if (position + 3 == m_length &&
                (traits::case_insensitive_ex::eq(word[position + 2],
                                                 common_lang_constants::LOWER_S) ||
                 traits::case_insensitive_ex::eq(word[position + 2],
                                                 common_lang_constants::LOWER_D)))
                {
                return false;
                }
            // doesn't
            if (m_ends_with_nt_contraction && m_length == 7 && position == 1 &&
                traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_S))
                {
                return false;
                }
            // phoenix
            if (position >= 2 && (traits::case_insensitive_ex::eq(word[position - 2],
                                                                  common_lang_constants::LOWER_P) &&
                                  traits::case_insensitive_ex::eq(word[position - 1],
                                                                  common_lang_constants::LOWER_H)))
                {
                return false;
                }
            // shoe
            if (position >= 2 && (traits::case_insensitive_ex::eq(word[position - 2],
                                                                  common_lang_constants::LOWER_S) &&
                                  traits::case_insensitive_ex::eq(word[position - 1],
                                                                  common_lang_constants::LOWER_H)))
                {
                return false;
                }
            // Schoen
            if (position >= 3 && (traits::case_insensitive_ex::eq(word[position - 3],
                                                                  common_lang_constants::LOWER_S) &&
                                  traits::case_insensitive_ex::eq(word[position - 2],
                                                                  common_lang_constants::LOWER_C) &&
                                  traits::case_insensitive_ex::eq(word[position - 1],
                                                                  common_lang_constants::LOWER_H)))
                {
                return false;
                }
            // amoeba
            if (position >= 2 && position + 3 <= m_length &&
                (traits::case_insensitive_ex::eq(word[position - 2],
                                                 common_lang_constants::LOWER_A) &&
                 traits::case_insensitive_ex::eq(word[position - 1],
                                                 common_lang_constants::LOWER_M) &&
                 traits::case_insensitive_ex::eq(word[position + 2],
                                                 common_lang_constants::LOWER_B)))
                {
                return false;
                }
            // coerce, coedit
            // Also, "oe"s not handle here are usually a part of compound word that would split
            return true;
            }
        /// IE
        else if ((position + 1 < m_length) &&
                 traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_I) &&
                 traits::case_insensitive_ex::eq(word[position + 1],
                                                 common_lang_constants::LOWER_E))
            {
            // hottie, tie
            if (position + 2 == m_length)
                {
                return false;
                }
            // hy-giene
            if (position > 0 && position + 2 < m_length &&
                traits::case_insensitive_ex::eq(word[position - 1],
                                                common_lang_constants::LOWER_G) &&
                traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_N))
                {
                return false;
                }
            // acqui-esce, "quies" at the end won't split
            if (position >= 2 && position + 3 < m_length &&
                traits::case_insensitive_ex::eq(word[position - 2],
                                                common_lang_constants::LOWER_Q) &&
                traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_S))
                {
                return true;
                }
            // fiend
            if ((position + 3 < m_length) &&
                traits::case_insensitive_ex::eq(word[position + 2],
                                                common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(word[position + 3], common_lang_constants::LOWER_D))
                {
                return false;
                }
            // happiest, driest
            if ((position + 3 < m_length) &&
                traits::case_insensitive_ex::eq(word[position + 2],
                                                common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(word[position + 3], common_lang_constants::LOWER_T))
                {
                // special case for "priest"
                return (position != 2) ||
                       !traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_P) ||
                       !traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_R);
                }
            // soviet
            if ((position + 2 < m_length) &&
                traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_T))
                {
                return true;
                }
            // IEG
            if ((position + 2 < m_length) &&
                traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_G))
                {
                // blitzkrieg
                if (position + 3 == m_length)
                    {
                    return false;
                    }
                // Diego
                if (traits::case_insensitive_ex::eq(word[position + 3],
                                                    common_lang_constants::LOWER_O))
                    {
                    return true;
                    }
                // siege
                return false;
                }
            // IEN
            if ((position + 2 < m_length) &&
                traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_N))
                {
                // "science", "client"
                if (is_first_vowel_block_in_word && position > 1)
                    {
                    return true;
                    }
                // watch out for "cien"
                if (position > 0 && traits::case_insensitive_ex::eq(word[position - 1],
                                                                    common_lang_constants::LOWER_C))
                    {
                    // sci-en[tc] words should split
                    if (position > 1 &&
                        traits::case_insensitive_ex::eq(word[position - 2],
                                                        common_lang_constants::LOWER_S) &&
                        (position + 3 < m_length) &&
                        (traits::case_insensitive_ex::eq(word[position + 3],
                                                         common_lang_constants::LOWER_T) ||
                         traits::case_insensitive_ex::eq(word[position + 3],
                                                         common_lang_constants::LOWER_C)))
                        {
                        // exception for "ne-scient"
                        if (position == 4 &&
                            traits::case_insensitive_ex::eq(word[0],
                                                            common_lang_constants::LOWER_N) &&
                            traits::case_insensitive_ex::eq(word[1],
                                                            common_lang_constants::LOWER_E))
                            {
                            return false;
                            }
                        // exception for "pre-scient"
                        if (position == 5 &&
                            traits::case_insensitive_ex::eq(word[0],
                                                            common_lang_constants::LOWER_P) &&
                            traits::case_insensitive_ex::eq(word[1],
                                                            common_lang_constants::LOWER_R) &&
                            traits::case_insensitive_ex::eq(word[2],
                                                            common_lang_constants::LOWER_E))
                            {
                            return false;
                            }
                        // exception for "om-ni-scient"
                        if (position == 6 &&
                            traits::case_insensitive_ex::eq(word[0],
                                                            common_lang_constants::LOWER_O) &&
                            traits::case_insensitive_ex::eq(word[1],
                                                            common_lang_constants::LOWER_M) &&
                            traits::case_insensitive_ex::eq(word[2],
                                                            common_lang_constants::LOWER_N) &&
                            traits::case_insensitive_ex::eq(word[3],
                                                            common_lang_constants::LOWER_I))
                            {
                            return false;
                            }
                        return true;
                        }
                    return false;
                    }
                // IENCE
                if (position > 0 && position + 5 <= m_length &&
                    traits::case_insensitive_ex::eq(word[position + 3],
                                                    common_lang_constants::LOWER_C) &&
                    (traits::case_insensitive_ex::eq(word[position + 4],
                                                     common_lang_constants::LOWER_E) ||
                     traits::case_insensitive_ex::eq(word[position + 4],
                                                     common_lang_constants::LOWER_Y) ||
                     traits::case_insensitive_ex::eq(word[position + 4],
                                                     common_lang_constants::LOWER_I)))
                    {
                    // nu-tri-ence
                    if (position > 1 &&
                        traits::case_insensitive_ex::eq(word[position - 2],
                                                        common_lang_constants::LOWER_T) &&
                        traits::case_insensitive_ex::eq(word[position - 1],
                                                        common_lang_constants::LOWER_R))
                        {
                        return true;
                        }
                    // "[ltn]ienc[ye]" will be one sound. "pat-ience", "conv-en-ience",
                    // "sent-ience", "e-bull-ience"
                    if (traits::case_insensitive_ex::eq(word[position - 1],
                                                        common_lang_constants::LOWER_L) ||
                        traits::case_insensitive_ex::eq(word[position - 1],
                                                        common_lang_constants::LOWER_T) ||
                        traits::case_insensitive_ex::eq(word[position - 1],
                                                        common_lang_constants::LOWER_N))
                        {
                        // special exception for "len-i-ence"
                        if (position == 3 &&
                            traits::case_insensitive_ex::eq(word[0],
                                                            common_lang_constants::LOWER_L) &&
                            traits::case_insensitive_ex::eq(word[1],
                                                            common_lang_constants::LOWER_E))
                            {
                            return true;
                            }
                        // special exception for "sal-i-ence"
                        if (position == 3 &&
                            traits::case_insensitive_ex::eq(word[0],
                                                            common_lang_constants::LOWER_S) &&
                            traits::case_insensitive_ex::eq(word[1],
                                                            common_lang_constants::LOWER_A) &&
                            traits::case_insensitive_ex::eq(word[2],
                                                            common_lang_constants::LOWER_L))
                            {
                            return true;
                            }
                        return false;
                        }
                    return true;
                    }
                // "[ltn]ient" will be one sound. "pat-ient", "conv-en-ient", "sent-ient"
                if (position > 0 && position + 4 <= m_length &&
                    traits::case_insensitive_ex::eq(word[position + 3],
                                                    common_lang_constants::LOWER_T) &&
                    (traits::case_insensitive_ex::eq(word[position - 1],
                                                     common_lang_constants::LOWER_L) ||
                     traits::case_insensitive_ex::eq(word[position - 1],
                                                     common_lang_constants::LOWER_T) ||
                     traits::case_insensitive_ex::eq(word[position - 1],
                                                     common_lang_constants::LOWER_N)))
                    {
                    // special exception for "len-i-ent"
                    if (position == 3 &&
                        traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_L) &&
                        traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_E))
                        {
                        return true;
                        }
                    // special exception for "sal-i-ent"
                    if (position == 3 &&
                        traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
                        traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_A) &&
                        traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_L))
                        {
                        return true;
                        }
                    return false;
                    }
                // a-per-i-ent, nu-tri-ent
                if (position > 0 && traits::case_insensitive_ex::eq(word[position - 1],
                                                                    common_lang_constants::LOWER_R))
                    {
                    // ...except for "friend"
                    return position + 3 >= m_length ||
                           !traits::case_insensitive_ex::eq(word[position - 1],
                                                            common_lang_constants::LOWER_D);
                    }
                // [?]lien
                if (position > 1 && traits::case_insensitive_ex::eq(word[position - 1],
                                                                    common_lang_constants::LOWER_L))
                    {
                    // a-li-en, cli-ent
                    return true;
                    }
                // lien
                if (position == 1 && traits::case_insensitive_ex::eq(
                                         word[position - 1], common_lang_constants::LOWER_L))
                    {
                    return false;
                    }
                return true;
                }
            // IER
            if ((position + 2 < m_length) &&
                traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_R))
                {
                // bombardier, frontier, and soldier are special cases
                if (position == 4 &&
                    traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_L) &&
                    traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_D))
                    {
                    return false;
                    }
                if (position == 4 &&
                    traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_G) &&
                    traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_L) &&
                    traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_C))
                    {
                    return false;
                    }
                if (position == 5 &&
                    traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_F) &&
                    traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_T))
                    {
                    return false;
                    }
                if (position == 7 &&
                    traits::case_insensitive_ex::eq(word[0], common_lang_constants::LOWER_B) &&
                    traits::case_insensitive_ex::eq(word[1], common_lang_constants::LOWER_O) &&
                    traits::case_insensitive_ex::eq(word[2], common_lang_constants::LOWER_M) &&
                    traits::case_insensitive_ex::eq(word[3], common_lang_constants::LOWER_B) &&
                    traits::case_insensitive_ex::eq(word[4], common_lang_constants::LOWER_A) &&
                    traits::case_insensitive_ex::eq(word[5], common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(word[6], common_lang_constants::LOWER_D))
                    {
                    return false;
                    }

                if (position > 1 /*"pier" won't split*/ && (position + 3 == m_length))
                    {
                    // "ier" may be replacing a lower 'y' at the end of a word
                    return true;
                    }
                // out-li-ers
                if (position > 1 && // "piers" won't split
                    position + 4 == m_length &&
                    traits::case_insensitive_ex::eq(word[position + 3],
                                                    common_lang_constants::LOWER_S))
                    {
                    return true;
                    }
                // "ier" is inside the word
                if (characters::is_character::is_vowel(word[position + 3]))
                    {
                    // antierosion
                    return true;
                    }
                if (nextVowel != m_length)
                    {
                    // apierce
                    return !can_consonants_end_sound(word + position + 2,
                                                     (nextVowel - 1) - (position + 1));
                    }
                // rest of word is consonants, so this is one sound at the end
                return false;
                }
            return false;
            }
        // UA
        else if ((position + 1) < m_length &&
                 traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_U) &&
                 traits::case_insensitive_ex::eq(word[position + 1],
                                                 common_lang_constants::LOWER_A))
            {
            // persuade
            if ((position + 2) < m_length &&
                traits::case_insensitive_ex::eq(word[position + 2], common_lang_constants::LOWER_D))
                {
                return false;
                }
            // persuasion, suave
            if (position > 0 &&
                traits::case_insensitive_ex::eq(word[position - 1],
                                                common_lang_constants::LOWER_S) &&
                (position + 2) < m_length &&
                (traits::case_insensitive_ex::eq(word[position + 2],
                                                 common_lang_constants::LOWER_S) ||
                 traits::case_insensitive_ex::eq(word[position + 2],
                                                 common_lang_constants::LOWER_V)))
                {
                return false;
                }
            // dual, manual
            return true;
            }
        // UU
        else if ((position + 1) < m_length &&
                 traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_U) &&
                 traits::case_insensitive_ex::eq(word[position + 1],
                                                 common_lang_constants::LOWER_U))
            {
            // muumuu seems to be the only special case where it does not split
            return position <= 0 || !traits::case_insensitive_ex::eq(
                                        word[position - 1], common_lang_constants::LOWER_M);
            // everything else, such as vacuum, duumvir
            }
        // embryo
        else if (position > 0 &&
                 traits::case_insensitive_ex::eq(word[position], common_lang_constants::LOWER_Y) &&
                 !characters::is_character::is_vowel(word[position - 1]))
            {
            return true;
            }
        else
            {
            return false;
            }

        return false;
        }

    //----------------------------------------------
    bool english_syllabize::can_consonants_end_sound(const wchar_t* consonants, size_t block_length)
        {
        assert(consonants);
        if (consonants == nullptr)
            {
            return false;
            }
        if (block_length > 3)
            {
            return false;
            }
        if (block_length == 3)
            {
            return (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_R)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_M) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_P) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_T)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_H)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_H)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_G) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_U)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_Q) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_U)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_T)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_H)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_G) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_U)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_Q) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_U)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_H)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_H)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_R)) ?
                       true :
                       traits::case_insensitive_ex::eq(consonants[0],
                                                       common_lang_constants::LOWER_T) &&
                           traits::case_insensitive_ex::eq(consonants[1],
                                                           common_lang_constants::LOWER_C) &&
                           traits::case_insensitive_ex::eq(consonants[2],
                                                           common_lang_constants::LOWER_H);
            }
        if (block_length == 2)
            {
            return (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_B) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_B)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_C) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_H) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_K) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_R))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_D) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_D) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_G))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_F) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_F) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_R))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_G) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_G) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_H) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_R) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_U))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_K) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_K)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_L) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_C) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_F) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_G) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_K) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_L) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_M) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_N) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_P) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_T) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_V))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_M) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_B) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_M) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_P))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_N) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_C) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_D) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_G) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_K) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_N) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_S) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_T))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_P) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_P) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_R) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_S))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_Q) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_U)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_R) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_B) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_C) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_D) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_F) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_G) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_K) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_L) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_M) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_N) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_P) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_Q) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_R) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_S) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_T) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_V))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_S) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_H) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_K) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_L) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_M) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_N) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_P) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_S) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_T))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_T) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_H) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_T))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_W) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_N) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_S))) ?
                       true :
                       traits::case_insensitive_ex::eq(consonants[0],
                                                       common_lang_constants::LOWER_Y) &&
                           traits::case_insensitive_ex::eq(consonants[1],
                                                           common_lang_constants::LOWER_C);
            }
        if (block_length == 1)
            {
            return true;
            }
        return false;
        }

    //----------------------------------------------
    bool english_syllabize::can_consonants_be_modified_by_following_e(const wchar_t* consonants,
                                                                      size_t block_length)
        {
        assert(consonants);
        if (consonants == nullptr)
            {
            return false;
            }
        if (block_length > 3)
            {
            return false;
            }
        if (block_length == 3)
            {
            return (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_R)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_L) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_H)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_L) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_H)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_L) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_Z)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_P) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_S)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_M) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_P) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_H)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_L) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_Q) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_U)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_M) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_P) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_S)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_M) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_P) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_T)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_H)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_H)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_G) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_U)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_N) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_Q) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_U)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_P) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_R)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_T)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_H)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_G) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_U)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_Q) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_U)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_R) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_H)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_H)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_R)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_R)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_R)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_Q) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_U)) ?
                       true :
                       traits::case_insensitive_ex::eq(consonants[0],
                                                       common_lang_constants::LOWER_T) &&
                           traits::case_insensitive_ex::eq(consonants[1],
                                                           common_lang_constants::LOWER_C) &&
                           traits::case_insensitive_ex::eq(consonants[2],
                                                           common_lang_constants::LOWER_H);
            }
        if (block_length == 2)
            {
            return (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_B) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_B)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_K) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_K)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_C) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_H) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_K) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_R))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_D) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_D) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_G))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_F) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_F) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_R))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_G) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_G) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_H) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_N) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_R) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_U))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_L) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_C) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_F) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_G) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_K) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_L) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_M) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_N) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_P) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_S) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_T) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_V))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_M) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_B) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_M) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_N) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_P))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_N) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_C) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_D) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_G) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_K) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_N) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_S) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_Z))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_P) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_P) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_R) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_S))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_Q) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_U)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_R) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_B) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_C) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_D) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_F) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_G) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_K) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_L) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_M) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_N) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_P) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_Q) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_R) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_S) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_T) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_V) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_Z))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_S) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_C) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_H) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_K) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_L) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_N) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_P) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_S) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_T))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_T) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_H) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_T) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_Z))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_W) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_L) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_N) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_S) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_K))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_Y) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_L)) ? // gargoyle
                       true :
                       traits::case_insensitive_ex::eq(consonants[0],
                                                       common_lang_constants::LOWER_Z) &&
                           traits::case_insensitive_ex::eq(consonants[1],
                                                           common_lang_constants::LOWER_Z);
            }
        if (block_length == 1)
            {
            return true;
            }
        return false;
        }

    //----------------------------------------------
    bool english_syllabize::can_consonants_begin_sound(const wchar_t* consonants,
                                                       size_t block_length)
        {
        assert(consonants);
        if (consonants == nullptr)
            {
            return false;
            }
        /*if not a valid letter or length, then it is definitely a new section of the word
        It is probably a hyphen or period.*/
        if (block_length == 0)
            {
            return false;
            }
        if (!characters::is_character::is_alpha(consonants[0]))
            {
            return true;
            }
        if (block_length > 3)
            {
            return false;
            }
        if (block_length == 3)
            {
            // true if chr, phr, sch, shr, spr, str,scr, thr, sdr
            return (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_R)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_P) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_R)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_H)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_R)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_P) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_R)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_R)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_C) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_R)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_T) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_H) &&
                    traits::case_insensitive_ex::eq(consonants[2],
                                                    common_lang_constants::LOWER_R)) ?
                       true :
                       traits::case_insensitive_ex::eq(consonants[0],
                                                       common_lang_constants::LOWER_S) &&
                           traits::case_insensitive_ex::eq(consonants[1],
                                                           common_lang_constants::LOWER_D) &&
                           traits::case_insensitive_ex::eq(consonants[2],
                                                           common_lang_constants::LOWER_R);
            }
        if (block_length == 2)
            {
            // true if bl, br, ch, cl, cr, fl, fr, gn, gu, gr, pl, pr, th, tr, wr, wh, sc
            // sh, sk, sl, sm, sn, sp, sq, st, sw, sf, sb, sd
            return (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_B) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_L) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_R))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_C) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_H) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_L) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_R))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_D) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_R)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_F) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_L) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_R))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_G) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_N) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_U) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_R))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_K) &&
                    traits::case_insensitive_ex::eq(consonants[1],
                                                    common_lang_constants::LOWER_N)) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_P) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_L) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_R))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_T) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_H) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_R))) ?
                       true :
                   (traits::case_insensitive_ex::eq(consonants[0],
                                                    common_lang_constants::LOWER_W) &&
                    (traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_R) ||
                     traits::case_insensitive_ex::eq(consonants[1],
                                                     common_lang_constants::LOWER_H))) ?
                       true :
                       traits::case_insensitive_ex::eq(consonants[0],
                                                       common_lang_constants::LOWER_S) &&
                           (traits::case_insensitive_ex::eq(consonants[1],
                                                            common_lang_constants::LOWER_C) ||
                            traits::case_insensitive_ex::eq(consonants[1],
                                                            common_lang_constants::LOWER_H) ||
                            traits::case_insensitive_ex::eq(consonants[1],
                                                            common_lang_constants::LOWER_K) ||
                            traits::case_insensitive_ex::eq(consonants[1],
                                                            common_lang_constants::LOWER_L) ||
                            traits::case_insensitive_ex::eq(consonants[1],
                                                            common_lang_constants::LOWER_M) ||
                            traits::case_insensitive_ex::eq(consonants[1],
                                                            common_lang_constants::LOWER_N) ||
                            traits::case_insensitive_ex::eq(consonants[1],
                                                            common_lang_constants::LOWER_P) ||
                            traits::case_insensitive_ex::eq(consonants[1],
                                                            common_lang_constants::LOWER_Q) ||
                            traits::case_insensitive_ex::eq(consonants[1],
                                                            common_lang_constants::LOWER_T) ||
                            traits::case_insensitive_ex::eq(consonants[1],
                                                            common_lang_constants::LOWER_W) ||
                            traits::case_insensitive_ex::eq(consonants[1],
                                                            common_lang_constants::LOWER_F) ||
                            traits::case_insensitive_ex::eq(consonants[1],
                                                            common_lang_constants::LOWER_B) ||
                            traits::case_insensitive_ex::eq(consonants[1],
                                                            common_lang_constants::LOWER_D));
            }
        if (block_length == 1)
            {
            return (
                !traits::case_insensitive_ex::eq(consonants[0], common_lang_constants::LOWER_X));
            }
        return false;
        }

    //----------------------------------------------
    std::pair<size_t, size_t> english_syllabize::get_prefix_length(const wchar_t* start,
                                                                   const size_t length)
        {
        assert(start);
        if (start == nullptr)
            {
            return std::make_pair(0, 0);
            }
        // prayer
        if (length >= 6 &&
            traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_P) &&
            traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_R) &&
            traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A) &&
            traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_Y) &&
            traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(start[5], common_lang_constants::LOWER_R))
            {
            return std::make_pair(1, 6);
            }
        if (length >= 5)
            {
            // where
            if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_W) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_H) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_E))
                {
                // exception: wherever
                if (length >= 6 &&
                    traits::case_insensitive_ex::eq(start[5], common_lang_constants::LOWER_V))
                    {
                    return std::make_pair(2, 6);
                    }
                return std::make_pair(1, 5);
                }
            // readj
            if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_D) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_J))
                {
                return std::make_pair(2, 5);
                }
            // rearm, readm
            if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A) &&
                (traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_R) ||
                 traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_D)) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_M))
                {
                return std::make_pair(2, 5);
                }
            // reall
            if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_L) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_L))
                {
                if (length == 6 &&
                    traits::case_insensitive_ex::eq(start[5], common_lang_constants::LOWER_Y))
                    {
                    return std::make_pair(3, 6);
                    }
                return std::make_pair(2, 5);
                }
            // reapp
            if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_P) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_P))
                {
                return std::make_pair(2, 5);
                }
            // sales
            if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_L) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_S))
                {
                return std::make_pair(1, 5);
                }
            // intra
            if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_A))
                {
                return std::make_pair(2, 5);
                }
            // reass
            if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_S))
                {
                return std::make_pair(2, 5);
                }
            // reatt
            if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_T))
                {
                return std::make_pair(2, 5);
                }
            // retro
            if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_O))
                {
                return std::make_pair(2, 5);
                }
            // supra
            if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_U) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_P) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_A))
                {
                return std::make_pair(2, 5);
                }
            // ultra
            if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_U) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_L) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_A))
                {
                return std::make_pair(2, 5);
                }
            // video
            if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_V) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_D) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_O))
                {
                return std::make_pair(3, 5);
                }
            // under
            if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_U) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_D) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_R))
                {
                return std::make_pair(2, 5);
                }
            // inade
            if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_D) &&
                traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_E))
                {
                return std::make_pair(3, 5);
                }
            }
        if (length >= 4)
            {
            // over
            if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_O) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_V) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_R))
                {
                return std::make_pair(2, 4);
                }
            // reen
            if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_N))
                {
                return std::make_pair(2, 4);
                }
            // reaf
            if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_F))
                {
                return std::make_pair(2, 4);
                }
            // gyne
            if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_G) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_Y) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_N) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_E))
                {
                return std::make_pair(2, 4);
                }
            // real
            if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_R) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_A) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_L))
                {
                // exceptions: realm, real
                if (length >= 4 &&
                    traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_M))
                    {
                    return std::make_pair(0, 0);
                    }
                if (length == 4)
                    {
                    return std::make_pair(0, 0);
                    }
                return std::make_pair(2, 4);
                }
            // blue
            if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_B) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_L) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_U) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_E))
                {
                if (length == 5 &&
                    (traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_R) ||
                     traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_T)))
                    {
                    return std::make_pair(2, 5);
                    }
                if (length == 6 &&
                    traits::case_insensitive_ex::eq(start[4], common_lang_constants::LOWER_S) &&
                    traits::case_insensitive_ex::eq(start[5], common_lang_constants::LOWER_T))
                    {
                    return std::make_pair(2, 6);
                    }
                return std::make_pair(1, 4);
                }
            // ecto
            if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_C) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_T) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_O))
                {
                return std::make_pair(2, 4);
                }
            // seis
            if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_S) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_I) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_S))
                {
                return std::make_pair(1, 4);
                }
            // hemi
            if (traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_H) &&
                traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_E) &&
                traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_M) &&
                traits::case_insensitive_ex::eq(start[3], common_lang_constants::LOWER_I))
                {
                return std::make_pair(2, 4);
                }
            }
        // exo
        if (length >= 3 &&
            traits::case_insensitive_ex::eq(start[0], common_lang_constants::LOWER_E) &&
            traits::case_insensitive_ex::eq(start[1], common_lang_constants::LOWER_X) &&
            traits::case_insensitive_ex::eq(start[2], common_lang_constants::LOWER_O))
            {
            return std::make_pair(2, 3);
            }
        return std::make_pair(0, 0);
        }
    } // namespace grammar
