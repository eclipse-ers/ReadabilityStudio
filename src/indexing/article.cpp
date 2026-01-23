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

#include "article.h"
#include "../Wisteria-Dataviz/src/util/string_util.h"
#include "abbreviation.h"
#include "character_traits.h"
#include <set>

word_list grammar::is_incorrect_english_article::m_a_exceptions;
word_list grammar::is_incorrect_english_article::m_an_exceptions;

//-------------------------------------------------------------
bool grammar::is_incorrect_english_article::operator()(std::wstring_view article,
                                                       std::wstring_view word) const
    {
    // we only look at "a" and "an"
    if (article.empty() || word.empty() || article.length() > 2 ||
        !traits::case_insensitive_ex::eq(article[0], L'a') ||
        (article.length() == 2 && !traits::case_insensitive_ex::eq(article[1], L'n')) ||
        (word.empty() || characters::is_character::is_punctuation(word[0])))
        {
        return false;
        }

    // words that are debatable, just return that they match
    if ((word.length() == 3 && traits::case_insensitive_ex::compare(word, L"URL", 3) == 0) ||
        // the pronunciation of Xavier is debated, so ignore that word entirely
        (word.length() >= 3 && traits::case_insensitive_ex::compare(word, L"Xav", 3) == 0))
        {
        return false;
        }

    if (const size_t hyphenPos = word.find_first_of(L"-\uFF0D");
        hyphenPos != std::wstring_view::npos)
        {
        word = word.substr(0, hyphenPos);
        }
    // word starting with five consonants is without a doubt sounded out
    const bool startsWith5Consonants =
        (word.length() >= 5 && characters::is_character::is_consonant(word[0]) &&
         characters::is_character::is_consonant(word[1]) &&
         characters::is_character::is_consonant(word[2]) &&
         characters::is_character::is_consonant(word[3]) &&
         characters::is_character::is_consonant(word[4]));
    // or weird consonant combination at the start of an acronym (e.g., "LGA", "NCAA")
    const bool oddAcronymConsonantCombo =
        (word.length() >= 2 && characters::is_character::is_upper(word[0]) &&
         characters::is_character::is_upper(word[1]) &&
         // only certain consonant combinations make sense to look like a word you would pronounce
         ((word[0] == 'F' && characters::is_character::is_consonant(word[1]) &&
           (word[1] != 'L' && word[1] != 'R')) ||
          ((word.length() >= 3 && word[0] == 'F' && word[1] == 'L' && word[2] == 'T')) ||
          (word[0] == 'L' && characters::is_character::is_consonant(word[1])) ||
          (word[0] == 'M' && characters::is_character::is_consonant(word[1])) ||
          (word[0] == 'N' && characters::is_character::is_consonant(word[1])) ||
          (word[0] == 'R' && characters::is_character::is_consonant(word[1])) ||
          (word[0] == 'S' && characters::is_character::is_consonant(word[1]) &&
           (word[1] != 'C' && word[1] != 'H' && word[1] != 'L' && word[1] != 'M' &&
            word[1] != 'T' && word[1] != 'W'))));
    constexpr grammar::is_acronym IS_ACRONYM;
    const bool useLetterSoundedOut =
        (startsWith5Consonants || (word.length() == 1) ||
         // 2-letter acronym
         (word.length() == 2 && characters::is_character::is_upper(word[0]) &&
          characters::is_character::is_upper(word[1])) ||
         oddAcronymConsonantCombo || characters::is_character::is_punctuation(word[1]) ||
         characters::is_character::is_numeric(word[0]) ||
         characters::is_character::is_numeric(word[1]) ||
         // if an acronym and no vowels (including wide versions), then each letter should be
         // sounded out
         (IS_ACRONYM(word) &&
          word.find_first_of(L"aeiouy\uFF41\uFF45\uFF49\uFF4F\uFF55\uFF59AEIOUY\uFF21\uFF25"
                             "\uFF29\uFF2F\uFF35\uFF39") == std::wstring_view::npos));
    const bool isYear = (word.length() == 4 && characters::is_character::is_numeric(word[0]) &&
                         characters::is_character::is_numeric(word[1]) &&
                         characters::is_character::is_numeric(word[2]) &&
                         characters::is_character::is_numeric(word[3]));
    const bool isTime = (word.length() == 5 && characters::is_character::is_numeric(word[0]) &&
                         characters::is_character::is_numeric(word[1]) &&
                         traits::case_insensitive_ex::eq(word[2], L':') &&
                         characters::is_character::is_numeric(word[3]) &&
                         characters::is_character::is_numeric(word[4]));
    const bool is2Digit = ((word.length() == 2 && characters::is_character::is_numeric(word[0]) &&
                            characters::is_character::is_numeric(word[1])) ||
                           (word.length() > 2 && characters::is_character::is_numeric(word[0]) &&
                            characters::is_character::is_numeric(word[1]) &&
                            characters::is_character::is_punctuation(word[2])));
    const bool isSoundingOutStartingNumbers = (isYear || isTime || is2Digit);
    // if something like "1,800,00" then don't bother analyzing, will be too complicated
    if (!isSoundingOutStartingNumbers && characters::is_character::is_numeric(word[0]) &&
        word.find_first_of(L",.") != std::wstring_view::npos)
        {
        return false;
        }
    // a 2nd, a 1st, a 3rd
    if (!isSoundingOutStartingNumbers && word.length() >= 3 &&
        characters::is_character::is_numeric(word[word.length() - 3]) &&
        (traits::case_insensitive_ex::compare(word.substr(word.length() - 2), L"st", 2) == 0 ||
         traits::case_insensitive_ex::compare(word.substr(word.length() - 2), L"rd", 2) == 0 ||
         traits::case_insensitive_ex::compare(word.substr(word.length() - 2), L"nd", 2) == 0))
        {
        return article.length() != 1 || !traits::case_insensitive_ex::eq(article[0], L'a');
        }
    // a 5th, etc.
    if (!isSoundingOutStartingNumbers && word.length() >= 3 &&
        characters::is_character::is_numeric(word[word.length() - 3]) &&
        traits::case_insensitive_ex::compare(word.substr(word.length() - 2), L"th", 2) == 0)
        {
        // a 5th
        if (word[word.length() - 3] == L'2' || word[word.length() - 3] == L'3' ||
            word[word.length() - 3] == L'4' || word[word.length() - 3] == L'5' ||
            word[word.length() - 3] == L'6' || word[word.length() - 3] == L'7' ||
            word[word.length() - 3] == L'9' || word[word.length() - 3] == L'0')
            {
            return article.length() != 1 || !traits::case_insensitive_ex::eq(article[0], L'a');
            }
        // an 8th, an 11th
        if (word[word.length() - 3] == L'1' || word[word.length() - 3] == L'8')
            {
            return article.length() != 2 || !traits::case_insensitive_ex::eq(article[0], L'a') ||
                   !traits::case_insensitive_ex::eq(article[1], L'n');
            }
        }
    if (article.length() == 1 && traits::case_insensitive_ex::eq(article[0], L'a'))
        {
        // with years and times, you sound out the first two digits together
        if (isSoundingOutStartingNumbers)
            {
            return ((traits::case_insensitive_ex::eq(word[0], L'1') &&
                     traits::case_insensitive_ex::eq(word[1], L'1')) ||
                    (traits::case_insensitive_ex::eq(word[0], L'1') &&
                     traits::case_insensitive_ex::eq(word[1], L'8')) ||
                    traits::case_insensitive_ex::eq(word[0], L'8'));
            }
        // check for consonants that would cause this to be an error
        if (useLetterSoundedOut)
            {
            return (!is_a_exception(word) && (traits::case_insensitive_ex::eq(word[0], L'8') ||
                                              traits::case_insensitive_ex::eq(word[0], L'a') ||
                                              traits::case_insensitive_ex::eq(word[0], L'e') ||
                                              traits::case_insensitive_ex::eq(word[0], L'i') ||
                                              traits::case_insensitive_ex::eq(word[0], L'o') ||
                                              // u is actually correct (e.g., "a u-turn")
                                              traits::case_insensitive_ex::eq(word[0], L'f') ||
                                              traits::case_insensitive_ex::eq(word[0], L'h') ||
                                              traits::case_insensitive_ex::eq(word[0], L'l') ||
                                              traits::case_insensitive_ex::eq(word[0], L'm') ||
                                              traits::case_insensitive_ex::eq(word[0], L'n') ||
                                              traits::case_insensitive_ex::eq(word[0], L'r') ||
                                              traits::case_insensitive_ex::eq(word[0], L's') ||
                                              traits::case_insensitive_ex::eq(word[0], L'x')));
            }
        // if starts with a vowel (excluding 'y'),
        // then it should be wrong (unless a known exception)
        if (!traits::case_insensitive_ex::eq(word[0], L'y') &&
            characters::is_character::is_vowel(word[0]))
            {
            return !is_a_exception(word);
            }
        // if starts with a consonant, then it should be correct
        // (unless a known exception that should actually go with an "an")
        if (characters::is_character::is_consonant(word[0]))
            {
            return is_an_exception(word);
            }
        return false;
        }
    if (article.length() == 2 && traits::case_insensitive_ex::eq(article[0], L'a') &&
        traits::case_insensitive_ex::eq(article[1], L'n'))
        {
        if (isSoundingOutStartingNumbers)
            {
            return !((traits::case_insensitive_ex::eq(word[0], L'1') &&
                      traits::case_insensitive_ex::eq(word[1], L'1')) ||
                     (traits::case_insensitive_ex::eq(word[0], L'1') &&
                      traits::case_insensitive_ex::eq(word[1], L'8')) ||
                     traits::case_insensitive_ex::eq(word[0], L'8'));
            }
        if (useLetterSoundedOut)
            {
            return (!is_an_exception(word) && ((traits::case_insensitive_ex::ge(word[0], L'0') &&
                                                traits::case_insensitive_ex::le(word[0], L'7')) ||
                                               traits::case_insensitive_ex::eq(word[0], L'9') ||
                                               traits::case_insensitive_ex::eq(word[0], L'u') ||
                                               traits::case_insensitive_ex::eq(word[0], L'b') ||
                                               traits::case_insensitive_ex::eq(word[0], L'c') ||
                                               traits::case_insensitive_ex::eq(word[0], L'd') ||
                                               traits::case_insensitive_ex::eq(word[0], L'g') ||
                                               traits::case_insensitive_ex::eq(word[0], L'j') ||
                                               traits::case_insensitive_ex::eq(word[0], L'k') ||
                                               traits::case_insensitive_ex::eq(word[0], L'p') ||
                                               traits::case_insensitive_ex::eq(word[0], L'q') ||
                                               traits::case_insensitive_ex::eq(word[0], L't') ||
                                               traits::case_insensitive_ex::eq(word[0], L'v') ||
                                               traits::case_insensitive_ex::eq(word[0], L'w') ||
                                               traits::case_insensitive_ex::eq(word[0], L'y') ||
                                               traits::case_insensitive_ex::eq(word[0], L'z')));
            }
        // check for any consonants that would be OK after an "an"
        if (characters::is_character::is_consonant(word[0]))
            {
            return !is_an_exception(word);
            }
        // handle any 'an' exceptions that may start with a vowel
        if (is_an_exception(word))
            {
            return false;
            }
        // if starts with a vowel (excluding 'y'),
        // then it should be correct (unless a known exception that should actually go with an 'a')
        if (!traits::case_insensitive_ex::eq(word[0], L'y') &&
            characters::is_character::is_vowel(word[0]))
            {
            return is_a_exception(word);
            }
        return false;
        }
    return false;
    }

//-------------------------------------------------------------
bool grammar::is_incorrect_english_article::is_an_exception(std::wstring_view word)
    {
    assert(!word.empty());
    if (word.empty())
        {
        return false;
        }

    // if a known (full-word, case-insensitive) exception
    if (get_an_exceptions().contains(word))
        {
        return true;
        }
    if (traits::case_insensitive_ex::compare(word, L"honor", 5) == 0 ||
        traits::case_insensitive_ex::compare(word, L"honour", 6) == 0 ||
        traits::case_insensitive_ex::compare(word, L"hour", 4) == 0 ||
        traits::case_insensitive_ex::compare(word, L"heir", 4) == 0 ||
        traits::case_insensitive_ex::compare(word, L"html", 4) == 0 ||
        traits::case_insensitive_ex::compare(word, L"honest", 6) == 0 ||
        // an HRESULT
        (word.length() >= 2 && traits::case_insensitive_ex::eq(word[0], L'H') &&
         characters::is_character::is_consonant(word[1])) ||
        // treat SAT and sat differently
        (word.length() == 3 &&
         traits::case_insensitive_ex::compare_case_sensitive(word, L"SAT", 3) == 0))
        {
        return true;
        }
    // an XML, an XBox
    if (word.length() >= 2 && traits::case_insensitive_ex::eq(word[0], L'x') &&
        characters::is_character::is_consonant(word[1]))
        {
        return true;
        }
    // an NTSTATUS
    if (traits::case_insensitive_ex::compare(word, L"nt", 2) == 0)
        {
        return true;
        }
    return false;
    }

//-------------------------------------------------------------
bool grammar::is_incorrect_english_article::is_a_exception(std::wstring_view word)
    {
    assert(!word.empty());
    if (word.empty())
        {
        return false;
        }

    static const std::set<traits::case_insensitive_wstring_ex> caseIU3Prefixes = {
        L"uac", L"ubi", L"udf", L"uef", L"uie", L"uin" /* a uint_32 is correct */,
        L"ukr", L"ulo", /* a ULONG_PTR */
        L"ure", L"uri", L"uro", L"usa", L"usb", L"use",
        L"usi", L"usn", L"usu", L"utc", L"utf", L"uti",
        L"uto", L"uue", /* a uuencoded */
        L"uui",         /* a UUID */
        L"uwo"
    };

    // if a known (full-word, case-insensitive) exception
    if (get_a_exceptions().contains(word))
        {
        return true;
        }
    // a
    if (traits::case_insensitive_ex::eq(word[0], L'a'))
        {
        return false;
        }
    // e
    if (traits::case_insensitive_ex::eq(word[0], L'e'))
        {
        return (traits::case_insensitive_ex::compare(word, L"eu", 2) == 0);
        }
    // i
    if (traits::case_insensitive_ex::eq(word[0], L'i'))
        {
        return false;
        }
    // o
    if (traits::case_insensitive_ex::eq(word[0], L'o'))
        {
        if (traits::case_insensitive_ex::compare(word, L"one-", 4) == 0 ||
            traits::case_insensitive_ex::compare(word, L"once-", 5) == 0)
            {
            return true;
            }
        // "A or B" is OK, but "a OR in the hospital" is wrong, so do this case sensitively
        if (word.length() == 2 &&
            traits::case_insensitive_ex::compare_case_sensitive(word, L"or", 2) == 0)
            {
            return true;
            }
        return false;
        }
    // u
    if (traits::case_insensitive_ex::eq(word[0], L'u'))
        {
        if ((word.length() >= 3 &&
             // NOLINTNEXTLINE
             caseIU3Prefixes.contains(traits::case_insensitive_wstring_ex(word.data(), 3))) ||
            // UNC, but not UNCLE
            (word.length() == 3 &&
             traits::case_insensitive_ex::compare_case_sensitive(word, L"UNC", 3) == 0) ||
            traits::case_insensitive_ex::compare(word, L"uter", 4) == 0)
            {
            return true;
            }
        // "a UX" (User eXperience) is correct
        if (word.length() >= 2 && traits::case_insensitive_ex::eq(word[1], L'x'))
            {
            return true;
            }
        // "a UAA-compliant" is correct
        if (word.length() >= 2 && traits::case_insensitive_ex::eq(word[1], L'a'))
            {
            return true;
            }
        // "a UCX_USBDEVICE_CHARACTERISTIC" and "a UCM_TYPEC_PARTNER" are correct
        if (traits::case_insensitive_ex::compare(word, L"ucm", 3) == 0 ||
            traits::case_insensitive_ex::compare(word, L"ucx", 3) == 0)
            {
            return true;
            }
        if (traits::case_insensitive_ex::compare(word, L"uni", 3) == 0)
            {
            // unimpressed, uninteresting, unignored, uninitialized
            // should have "an" in front
            if (word.length() > 4 && traits::case_insensitive_ex::eq(word[3], L'n'))
                {
                return false;
                }
            // ...but "unimolecular" should be an 'a'
            if (word.length() > 4 && (traits::case_insensitive_ex::eq(word[3], L'm') ||
                                      traits::case_insensitive_ex::eq(word[3], L'g')))
                {
                return characters::is_character::is_vowel(word[4]);
                }
            // "a unidimensional" is correct, "a undetermined" is wrong
            if (word.length() > 4 && (traits::case_insensitive_ex::eq(word[3], L'd')))
                {
                return traits::case_insensitive_ex::eq(word[4], L'i');
                }
            // unillegal (for the sake of argument, let's say that's a word)
            if (word.length() > 4 &&
                (traits::case_insensitive_ex::eq(word[3], L'l') &&
                 // but should have a consonant following, because "a unilateral" is correct
                 characters::is_character::is_consonant(word[4])))
                {
                return false;
                }
            // universal, united, unicycle should have "a" in front
            return true;
            }
        // a uranium
        if (traits::case_insensitive_ex::compare(word, L"ura", 3) == 0)
            {
            return true;
            }
        // "a unanimous decision" is correct
        return (traits::case_insensitive_ex::compare(word, L"unani", 5) == 0);
        }
    return false;
    }
