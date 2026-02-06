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

#include "double_words.h"

const std::set<grammar::is_double_word_exception::string_type>
    grammar::is_double_word_exception::m_double_word_exceptions = {
        L"bang",                     // A gun goes bang bang
        L"bison",                    // Bison bison is American bison
        L"Bom",                      // Bom Bom ST5 is a cosmetic product
        L"Boola",                    // "Boola Boola" is the Yale football song
        L"choo",                     // The train goes "choo choo"
        L"bye",                      // "Say bye bye to grandma."
        L"das",      L"der", L"die", // German articles, which can appear repeated
        L"Doki",                     // A videogame
        L"ha",                       // "ha ha ha," she laughed at Blake's hilarious joke
        L"had",                      // he had had a car for sale
        L"ho",                       // "Ho ho ho," said Santa
        L"hush",                     // Let's keep that hush hush
        L"Kappa",                    // Kappa Kappa Gamma is a sorority
        L"Sapiens",                  // Homo Sapiens Sapiens is modern humans
        L"Hool",                     // from a Chief Joseph speech
        L"ice",                      // "Ice Ice Baby" is (arguably) as song
        L"la",                       // La La Land
        L"Leng",                     // common name to appear doubled
        L"log",                      // log-log function in math
        L"long",                     // long long integers
        L"mephitis",                 // Mephitis mephitis is the white skunk
        L"no",                       // "Using C macros in new code is a no no!"
        L"now",                      // "now now, take it easy"
        L"Pago",                     // capital of American Samoa
        L"pee",                      // "Do you need to pee pee?" asked the urologist
        L"plus",                     // C plus plus (it sometimes gets spelled out like this)
        L"sie",                      // German word that can be repeated
        L"sigma",                    // Sigma Sigma is a fraternity at U. of Cincinnati
        L"Sirhan",                   // "Sirhan Sirhan" was RFJ's assassin
        L"snip",                     // "snip snip," said the urologist
        L"ta",                       // "ta ta" means "goodbye"
        L"that",                     // I know that that car if for sale
        L"uh",                       // variation of "no"
        L"Walla"                     // Walla Walla, Washington
    };

//--------------------------------------------------
bool grammar::is_double_word_exception::operator()(const std::wstring_view text) const
    {
    if (text.empty() || // if empty, then I suppose it should be an exception
        (text.length() == 1 && characters::is_character::is_punctuation(text[0])))
        {
        return true;
        }
    return m_double_word_exceptions.contains(string_type{ text.data(), text.length() });
    }
