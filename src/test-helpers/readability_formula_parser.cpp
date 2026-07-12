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

#include "readability_formula_parser.h"
#include "../projects/base_project.h"

/// @returns The total number of numerals from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double NumeralCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalNumerals();
    }

/// @returns The number of unique words from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double UniqueWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalUniqueWords();
    }

/// @returns The total number of unfamiliar Spache words from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double UnfamiliarSpacheWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalHardWordsSpache();
    }

/// @returns The total number of unique unfamiliar Spache words from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double UniqueUnfamiliarSpacheWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))
        ->GetProject()
        ->GetTotalUniqueHardWordsSpache();
    }

/// @returns The total number of familiar Spache words from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double FamiliarSpacheWordCount(const te_expr* context)
    {
    const BaseProject* project = (dynamic_cast<const FormulaProject*>(context))->GetProject();
    return project->GetTotalWords() - project->GetTotalHardWordsSpache();
    }

/// @returns The total number of words consisting of six or more character from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double SixCharacterPlusWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalLongWords();
    }

/// @returns The number of unique words consisting of six or more character from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double UniqueSixCharacterPlusWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))
        ->GetProject()
        ->GetTotalUnique6CharsPlusWords();
    }

/// @returns The total number of words consisting of seven or more character from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double SevenCharacterPlusWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalHardLixRixWords();
    }

/// @returns The total number of miniwords from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double MiniWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalMiniWords();
    }

/// @returns The total number of hard Fog words from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double HardFogWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalHardWordsFog();
    }

/// @returns The total number of unfamiliar Dale-Chall words from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double UnfamiliarDaleChallWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))
        ->GetProject()
        ->GetTotalHardWordsDaleChall();
    }

/// @returns The total number of unique unfamiliar Dale-Chall words from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double UniqueUnfamiliarDaleChallWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))
        ->GetProject()
        ->GetTotalUniqueDCHardWords();
    }

/// @returns The project and current custom test name from the expression context.
/// @throws std::runtime_error if the custom test cannot be found.
[[nodiscard]]
static std::pair<const BaseProject*, wxString> GetProjectAndCustomTestName(const te_expr* context)
    {
    const BaseProject* project = (dynamic_cast<const FormulaProject*>(context))->GetProject();
    wxString testName = project->GetCurrentCustomTest();
    if (!project->HasCustomTest(testName))
        {
        throw std::runtime_error(
            _(L"Internal error: unable to find custom test by name.").ToUTF8());
        }
    return { project, std::move(testName) };
    }

/// Performs a New Dale-Chall test with a custom familiar word list.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double CustomNewDaleChall(const te_expr* context)
    {
    const auto [project, testName] = GetProjectAndCustomTestName(context);
    if (!project->GetCustomTest(testName)->GetIterator()->is_using_familiar_words())
        {
        throw std::runtime_error(_(L"Test has not defined what an unfamiliar word is. "
                                   "Custom unfamiliar word test cannot be calculated.")
                                     .ToUTF8());
        }
    if (project->GetProjectLanguage() != readability::test_language::english_test)
        {
        throw std::runtime_error(
            wxString::Format(_(L"%s function can only be used for English projects."),
                             ReadabilityFormulaParser::GetCustomNewDaleChallSignature().ToUTF8()));
        }
    // lowest grade for DC
    double gradeValue = 0;
    try
        {
        size_t gradeBegin = 0, gradeEnd = 0;
        // use whichever calculation regular DC is using
        if (project->GetDaleChallTextExclusionMode() ==
            SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings)
            {
            readability::new_dale_chall(
                gradeBegin, gradeEnd, project->GetTotalWordsFromCompleteSentencesAndHeaders(),
                project->GetCustomTest(testName)->GetUnfamiliarWordCount(),
                project->GetTotalSentencesFromCompleteSentencesAndHeaders());
            }
        else
            {
            readability::new_dale_chall(gradeBegin, gradeEnd, project->GetTotalWords(),
                                        project->GetCustomTest(testName)->GetUnfamiliarWordCount(),
                                        project->GetTotalSentences());
            }
        // Grade range will be combined into a single double value.
        // Client will need to split this value
        gradeValue = static_cast<double>(
            join_int32s(static_cast<uint32_t>(gradeBegin), static_cast<uint32_t>(gradeEnd)));
        }
    catch (const std::domain_error&)
        {
        throw std::runtime_error(_(L"Unable to calculate custom New Dale Chall.").ToUTF8());
        }
    return gradeValue;
    }

/// Performs a Spache Revised test with a custom familiar word list.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double CustomSpache(const te_expr* context)
    {
    const auto [project, testName] = GetProjectAndCustomTestName(context);
    if (!project->GetCustomTest(testName)->GetIterator()->is_using_familiar_words())
        {
        throw std::runtime_error(_(L"Test has not defined what an unfamiliar word is. "
                                   "Custom unfamiliar word test cannot be calculated.")
                                     .ToUTF8());
        }
    if (project->GetProjectLanguage() != readability::test_language::english_test)
        {
        throw std::runtime_error(
            wxString::Format(_(L"%s function can only be used for English projects."),
                             ReadabilityFormulaParser::GetCustomSpacheSignature())
                .ToUTF8());
        }
    // lowest grade for Spache
    double gradeValue = 0;
    try
        {
        gradeValue =
            readability::spache(project->GetTotalWords(),
                                project->GetCustomTest(testName)->GetUniqueUnfamiliarWordCount(),
                                project->GetTotalSentences());
        }
    catch (const std::domain_error&)
        {
        throw std::runtime_error(_(L"Unable to calculate custom Spache.").ToUTF8());
        }
    return gradeValue;
    }

/// Performs a Harris-Jacobson test with a custom familiar word list.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double CustomHarrisJacobson(const te_expr* context)
    {
    const auto [project, testName] = GetProjectAndCustomTestName(context);
    if (!project->GetCustomTest(testName)->GetIterator()->is_using_familiar_words())
        {
        throw std::runtime_error(_(L"Test has not defined what an unfamiliar word is. "
                                   "Custom unfamiliar word test cannot be calculated.")
                                     .ToUTF8());
        }
    if (project->GetProjectLanguage() != readability::test_language::english_test)
        {
        throw std::runtime_error(
            wxString::Format(_(L"%s function can only be used for English projects."),
                             ReadabilityFormulaParser::GetCustomHarrisJacobsonSignature())
                .ToUTF8());
        }
    double gradeValue = 1; // lowest grade for HJ
    try
        {
        if (project->GetHarrisJacobsonTextExclusionMode() ==
            SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings)
            {
            gradeValue = readability::harris_jacobson(
                project->GetTotalWordsFromCompleteSentencesAndHeaders() -
                    project->GetTotalNumeralsFromCompleteSentencesAndHeaders(),
                project->GetCustomTest(testName)->GetUniqueUnfamiliarWordCount(),
                project->GetTotalSentencesFromCompleteSentencesAndHeaders());
            }
        else
            {
            gradeValue = readability::harris_jacobson(
                project->GetTotalWords() - project->GetTotalNumerals(),
                project->GetCustomTest(testName)->GetUniqueUnfamiliarWordCount(),
                project->GetTotalSentences());
            }
        }
    catch (const std::domain_error&)
        {
        throw std::runtime_error(_(L"Unable to calculate custom Harris-Jacobson.").ToUTF8());
        }
    return gradeValue;
    }

/// @returns The total number of unfamiliar words (from a custom list) the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double UnfamiliarWordCount(const te_expr* context)
    {
    const auto [project, testName] = GetProjectAndCustomTestName(context);
    return project->GetCustomTest(testName)->GetUnfamiliarWordCount();
    }

/// @returns The total number of unique unfamiliar words (from a custom list) from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double UniqueUnfamiliarWordCount(const te_expr* context)
    {
    const auto [project, testName] = GetProjectAndCustomTestName(context);
    return project->GetCustomTest(testName)->GetUniqueUnfamiliarWordCount();
    }

/// @returns The total number of familiar words (from a custom list) the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double FamiliarWordCount(const te_expr* context)
    {
    const auto [project, testName] = GetProjectAndCustomTestName(context);
    return project->GetTotalWords() - project->GetCustomTest(testName)->GetUnfamiliarWordCount();
    }

/// @returns The total number of unfamiliar Harris-Jacobson words from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double UnfamiliarHarrisJacobsonWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))
        ->GetProject()
        ->GetTotalHardWordsHarrisJacobson();
    }

/// @returns The total number of unique unfamiliar Harris-Jacobson words from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double UniqueUnfamiliarHarrisJacobsonWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))
        ->GetProject()
        ->GetTotalUniqueHarrisJacobsonHardWords();
    }

/// @returns The total number of familiar Harris-Jacobson words from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double FamiliarHarrisJacobsonWordCount(const te_expr* context)
    {
    const BaseProject* project = (dynamic_cast<const FormulaProject*>(context))->GetProject();
    if (project->GetHarrisJacobsonTextExclusionMode() ==
        SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings)
        {
        return (project->GetTotalWordsFromCompleteSentencesAndHeaders() -
                project->GetTotalNumeralsFromCompleteSentencesAndHeaders()) -
               project->GetTotalHardWordsHarrisJacobson();
        }
    return (project->GetTotalWords() - project->GetTotalNumerals()) -
           project->GetTotalHardWordsHarrisJacobson();
    }

/// @returns The number of unique monosyllabic words from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double UniqueOneSyllableWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))
        ->GetProject()
        ->GetTotalUniqueMonoSyllabicWords();
    }

/// @returns The total number of familiar Dale-Chall words from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double FamiliarDaleChallWordCount(const te_expr* context)
    {
    const BaseProject* project = (dynamic_cast<const FormulaProject*>(context))->GetProject();
    if (project->GetDaleChallTextExclusionMode() ==
        SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings)
        {
        return project->GetTotalWordsFromCompleteSentencesAndHeaders() -
               project->GetTotalHardWordsDaleChall();
        }
    return project->GetTotalWords() - project->GetTotalHardWordsDaleChall();
    }

/// @returns The total number of monosyllabic words from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double OneSyllableWordCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))
        ->GetProject()
        ->GetTotalMonoSyllabicWords();
    }

/// @returns The total number of units/independent clauses from the document.
/// A unit is defined as a section of text ending with a period, exclamation mark,
///     question mark, interrobang, colon, semicolon, or dash.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double IndependentClauseCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalSentenceUnits();
    }

/// @returns The total number of characters and punctuation from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double CharacterPlusPunctuationCount(const te_expr* context)
    {
    return (dynamic_cast<const FormulaProject*>(context))
        ->GetProject()
        ->GetTotalCharactersPlusPunctuation();
    }

/// @returns The total number of proper nouns from the document.
/// @param context The TinyEpr++ expression object.
[[nodiscard]]
static double ProperNounCount(const te_expr* context)
    {
    if (dynamic_cast<const FormulaProject*>(context)->GetProject()->GetProjectLanguage() ==
        readability::test_language::german_test)
        {
        throw std::runtime_error(
            _(L"ProperNounCount() function not supported for German projects.").ToUTF8());
        }
    return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalProperNouns();
    }

/// @returns The total number of words from the document.
/// @param context The TinyEpr++ expression object.
/// @param wordType The type of word to count.
[[nodiscard]]
static double WordCount(const te_expr* context, const double wordType)
    {
    if (std::isnan(wordType) || wordType == 0 /*Default*/)
        {
        return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalWords();
        }
    if (wordType == 1 /*DaleChall*/)
        {
        const BaseProject* project = (dynamic_cast<const FormulaProject*>(context))->GetProject();
        return (project->GetDaleChallTextExclusionMode() ==
                SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                   project->GetTotalWordsFromCompleteSentencesAndHeaders() :
                   project->GetTotalWords();
        }
    if (wordType == 2 /*HarrisJacobson*/)
        {
        const BaseProject* project = (dynamic_cast<const FormulaProject*>(context))->GetProject();
        return (project->GetHarrisJacobsonTextExclusionMode() ==
                SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                   project->GetTotalWordsFromCompleteSentencesAndHeaders() :
                   project->GetTotalWords();
        }
    throw std::runtime_error(
        wxString::Format(_(L"Invalid value used in %s"), wxString{ __func__ }).ToUTF8());
    }

/// @returns The total number of words consisting of three or more syllables from the document.
/// @param context The TinyEpr++ expression object.
/// @param wordType The type of word to count.
[[nodiscard]]
static double ThreeSyllablePlusWordCount(const te_expr* context, const double wordType)
    {
    if (std::isnan(wordType) || wordType == 0 /*Default*/)
        {
        return (dynamic_cast<const FormulaProject*>(context))
            ->GetProject()
            ->GetTotal3PlusSyllabicWords();
        }
    if (wordType == 1 /*NumeralsFullySyllabized*/)
        {
        return (dynamic_cast<const FormulaProject*>(context))
            ->GetProject()
            ->GetTotal3PlusSyllabicWordsNumeralsFullySyllabized();
        }
    throw std::runtime_error(
        wxString::Format(_(L"Invalid value used in %s"), wxString{ __func__ }).ToUTF8());
    }

/// @returns The number of unique words consisting of three or more syllables from the document.
/// @param context The TinyEpr++ expression object.
/// @param wordType The type of word to count.
[[nodiscard]]
static double UniqueThreeSyllablePlusWordCount(const te_expr* context, const double wordType)
    {
    if (std::isnan(wordType) || wordType == 0 /*Default*/)
        {
        return (dynamic_cast<const FormulaProject*>(context))
            ->GetProject()
            ->GetTotalUnique3PlusSyllableWords();
        }
    if (wordType == 1 /*NumeralsFullySyllabized*/)
        {
        return (dynamic_cast<const FormulaProject*>(context))
            ->GetProject()
            ->GetUnique3PlusSyllabicWordsNumeralsFullySyllabized();
        }
    throw std::runtime_error(
        wxString::Format(_(L"Invalid value used in %s"), wxString{ __func__ }).ToUTF8());
    }

/// @returns The total number of syllables from the document.
/// @param context The TinyEpr++ expression object.
/// @param wordType The type of word to count.
[[nodiscard]]
static double SyllableCount(const te_expr* context, const double wordType)
    {
    if (std::isnan(wordType) || wordType == 0 /*Default*/)
        {
        return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalSyllables();
        }
    if (wordType == 1 /*NumeralsFullySyllabized*/)
        {
        return (dynamic_cast<const FormulaProject*>(context))
            ->GetProject()
            ->GetTotalSyllablesNumeralsFullySyllabized();
        }
    if (wordType == 2 /*NumeralsAreOneSyllable*/)
        {
        return (dynamic_cast<const FormulaProject*>(context))
            ->GetProject()
            ->GetTotalSyllablesNumeralsOneSyllable();
        }
    throw std::runtime_error(
        wxString::Format(_(L"Invalid value used in %s"), wxString{ __func__ }).ToUTF8());
    }

/// @returns The total number of characters (i.e., letters and numbers) from the document.
/// @param context The TinyEpr++ expression object.
/// @param wordType The type of word to count.
[[nodiscard]]
static double CharacterCount(const te_expr* context, const double wordType)
    {
    if (std::isnan(wordType) || wordType == 0 /*Default*/)
        {
        return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalCharacters();
        }
    if (wordType == 1 /*DaleChall*/)
        {
        const BaseProject* project = (dynamic_cast<const FormulaProject*>(context))->GetProject();
        return (project->GetDaleChallTextExclusionMode() ==
                SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                   project->GetTotalCharactersFromCompleteSentencesAndHeaders() :
                   project->GetTotalCharacters();
        }
    if (wordType == 2 /*HarrisJacobson*/)
        {
        const BaseProject* project = (dynamic_cast<const FormulaProject*>(context))->GetProject();
        return (project->GetHarrisJacobsonTextExclusionMode() ==
                SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                   project->GetTotalCharactersFromCompleteSentencesAndHeaders() :
                   project->GetTotalCharacters();
        }
    throw std::runtime_error(
        wxString::Format(_(L"Invalid value used in %s"), wxString{ __func__ }).ToUTF8());
    }

/// @returns The total number of sentences from the document.
/// @param context The TinyEpr++ expression object.
/// @param wordType The type of word to count.
[[nodiscard]]
static double SentenceCount(const te_expr* context, const double wordType)
    {
    if (std::isnan(wordType) || wordType == 0 /*Default*/)
        {
        return (dynamic_cast<const FormulaProject*>(context))->GetProject()->GetTotalSentences();
        }
    if (wordType == 1 /*DaleChall*/)
        {
        const BaseProject* project = (dynamic_cast<const FormulaProject*>(context))->GetProject();
        return (project->GetDaleChallTextExclusionMode() ==
                SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                   project->GetTotalSentencesFromCompleteSentencesAndHeaders() :
                   project->GetTotalSentences();
        }
    if (wordType == 2 /*HarrisJacobson*/)
        {
        const BaseProject* project = (dynamic_cast<const FormulaProject*>(context))->GetProject();
        return (project->GetHarrisJacobsonTextExclusionMode() ==
                SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                   project->GetTotalSentencesFromCompleteSentencesAndHeaders() :
                   project->GetTotalSentences();
        }
    // Fog has special rules for units vs. traditional sentences
    if (wordType == 3 /*GunningFog*/)
        {
        const BaseProject* project = (dynamic_cast<const FormulaProject*>(context))->GetProject();
        return project->IsFogUsingSentenceUnits() ? project->GetTotalSentenceUnits() :
                                                    project->GetTotalSentences();
        }
    throw std::runtime_error(
        wxString::Format(_(L"Invalid value used in %s"), wxString{ __func__ }).ToUTF8());
    }

//------------------------------------------------
void ReadabilityFormulaParser::UpdateVariables()
    {
    const double dcWordCount =
        (m_formulaProject.GetProject()->GetDaleChallTextExclusionMode() ==
         SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
            (m_formulaProject.GetProject()->GetTotalWordsFromCompleteSentencesAndHeaders() -
             m_formulaProject.GetProject()->GetTotalHardWordsDaleChall()) :
            (m_formulaProject.GetProject()->GetTotalWords() -
             m_formulaProject.GetProject()->GetTotalHardWordsDaleChall());

    set_constant(_DT("D"), dcWordCount);
    }

//------------------------------------------------
ReadabilityFormulaParser::ReadabilityFormulaParser(const BaseProject* project,
                                                   const wchar_t decimalSeparator,
                                                   const wchar_t listSeparator)
    : m_formulaProject(project)
    {
    if (project == nullptr)
        {
        throw std::runtime_error(_("Invalid project in formula parser.").ToUTF8());
        }
    wxASSERT(decimalSeparator <= 255);
    wxASSERT(listSeparator <= 255);
    if (decimalSeparator <= 255 && listSeparator <= 255)
        {
        set_decimal_separator(static_cast<char>(decimalSeparator));
        set_list_separator(static_cast<char>(listSeparator));
        }

    const double dcWordCount =
        (m_formulaProject.GetProject()->GetDaleChallTextExclusionMode() ==
         SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
            (m_formulaProject.GetProject()->GetTotalWordsFromCompleteSentencesAndHeaders() -
             m_formulaProject.GetProject()->GetTotalHardWordsDaleChall()) :
            (m_formulaProject.GetProject()->GetTotalWords() -
             m_formulaProject.GetProject()->GetTotalHardWordsDaleChall());

    set_variables_and_functions(std::set<te_variable>{
        // note that these constants must be char* (not wchar_t*)
        { _DT("UNIQUETHREESYLLABLEPLUSWORDCOUNT"),
          static_cast<te_confun1>(UniqueThreeSyllablePlusWordCount), TE_DEFAULT,
          &m_formulaProject },
        { _DT("THREESYLLABLEPLUSWORDCOUNT"), static_cast<te_confun1>(ThreeSyllablePlusWordCount),
          TE_DEFAULT, &m_formulaProject },
        { _DT("SYLLABLECOUNT"), static_cast<te_confun1>(SyllableCount), TE_DEFAULT,
          &m_formulaProject },
        { _DT("CHARACTERCOUNT"), static_cast<te_confun1>(CharacterCount), TE_DEFAULT,
          &m_formulaProject },
        { _DT("SENTENCECOUNT"), static_cast<te_confun1>(SentenceCount), TE_DEFAULT,
          &m_formulaProject },
        { _DT("WORDCOUNT"), static_cast<te_confun1>(WordCount), TE_DEFAULT, &m_formulaProject },
        { _DT("MINIWORDCOUNT"), static_cast<te_confun0>(MiniWordCount), TE_DEFAULT,
          &m_formulaProject },
        { _DT("HARDFOGWORDCOUNT"), static_cast<te_confun0>(HardFogWordCount), TE_DEFAULT,
          &m_formulaProject },
        { _DT("CHARACTERPLUSPUNCTUATIONCOUNT"),
          static_cast<te_confun0>(CharacterPlusPunctuationCount), TE_DEFAULT, &m_formulaProject },
        { _DT("NUMERALCOUNT"), static_cast<te_confun0>(NumeralCount), TE_DEFAULT,
          &m_formulaProject },
        { _DT("UNIQUEWORDCOUNT"), static_cast<te_confun0>(UniqueWordCount), TE_DEFAULT,
          &m_formulaProject },
        { _DT("UNIQUESIXCHARACTERPLUSWORDCOUNT"),
          static_cast<te_confun0>(UniqueSixCharacterPlusWordCount), TE_DEFAULT, &m_formulaProject },
        { _DT("UNIQUEONESYLLABLEWORDCOUNT"), static_cast<te_confun0>(UniqueOneSyllableWordCount),
          TE_DEFAULT, &m_formulaProject },
        { _DT("FAMILIARWORDCOUNT"), static_cast<te_confun0>(FamiliarWordCount), TE_DEFAULT,
          &m_formulaProject },
        { _DT("UNFAMILIARWORDCOUNT"), static_cast<te_confun0>(UnfamiliarWordCount), TE_DEFAULT,
          &m_formulaProject },
        { _DT("UNIQUEUNFAMILIARWORDCOUNT"), static_cast<te_confun0>(UniqueUnfamiliarWordCount),
          TE_DEFAULT, &m_formulaProject },
        { _DT("UNFAMILIARHARRISJACOBSONWORDCOUNT"),
          static_cast<te_confun0>(UnfamiliarHarrisJacobsonWordCount), TE_DEFAULT,
          &m_formulaProject },
        { _DT("UNIQUEUNFAMILIARHARRISJACOBSONWORDCOUNT"),
          static_cast<te_confun0>(UniqueUnfamiliarHarrisJacobsonWordCount), TE_DEFAULT,
          &m_formulaProject },
        { _DT("FAMILIARHARRISJACOBSONWORDCOUNT"),
          static_cast<te_confun0>(FamiliarHarrisJacobsonWordCount), TE_DEFAULT, &m_formulaProject },
        { _DT("UNFAMILIARDALECHALLWORDCOUNT"),
          static_cast<te_confun0>(UnfamiliarDaleChallWordCount), TE_DEFAULT, &m_formulaProject },
        { _DT("UNIQUEUNFAMILIARDALECHALLWORDCOUNT"),
          static_cast<te_confun0>(UniqueUnfamiliarDaleChallWordCount), TE_DEFAULT,
          &m_formulaProject },
        { _DT("FAMILIARDALECHALLWORDCOUNT"), static_cast<te_confun0>(FamiliarDaleChallWordCount),
          TE_DEFAULT, &m_formulaProject },
        { _DT("UNFAMILIARSPACHEWORDCOUNT"), static_cast<te_confun0>(UnfamiliarSpacheWordCount),
          TE_DEFAULT, &m_formulaProject },
        { _DT("UNIQUEUNFAMILIARSPACHEWORDCOUNT"),
          static_cast<te_confun0>(UniqueUnfamiliarSpacheWordCount), TE_DEFAULT, &m_formulaProject },
        { _DT("FAMILIARSPACHEWORDCOUNT"), static_cast<te_confun0>(FamiliarSpacheWordCount),
          TE_DEFAULT, &m_formulaProject },
        { _DT("SIXCHARACTERPLUSWORDCOUNT"), static_cast<te_confun0>(SixCharacterPlusWordCount),
          TE_DEFAULT, &m_formulaProject },
        { _DT("SEVENCHARACTERPLUSWORDCOUNT"), static_cast<te_confun0>(SevenCharacterPlusWordCount),
          TE_DEFAULT, &m_formulaProject },
        { _DT("ONESYLLABLEWORDCOUNT"), static_cast<te_confun0>(OneSyllableWordCount), TE_DEFAULT,
          &m_formulaProject },
        { _DT("INDEPENDENTCLAUSECOUNT"), static_cast<te_confun0>(IndependentClauseCount),
          TE_DEFAULT, &m_formulaProject },
        { _DT("PROPERNOUNCOUNT"), static_cast<te_confun0>(ProperNounCount), TE_DEFAULT,
          &m_formulaProject },
        { _DT("CUSTOMHARRISJACOBSON"), static_cast<te_confun0>(CustomHarrisJacobson), TE_DEFAULT,
          &m_formulaProject },
        { _DT("CUSTOMSPACHE"), static_cast<te_confun0>(CustomSpache), TE_DEFAULT,
          &m_formulaProject },
        { _DT("CUSTOMNEWDALECHALL"), static_cast<te_confun0>(CustomNewDaleChall), TE_DEFAULT,
          &m_formulaProject },
        // shortcuts
        { "B", &project->GetTotalSyllables() },
        { "S", &project->GetTotalSentences() },
        { "W", &project->GetTotalWords() },
        // a dynamic constant, call UpdateVariables() prior to interpret() to update
        { "D", dcWordCount },
        { "R", &project->GetTotalCharacters() },
        { _DT("RP"), &project->GetTotalCharactersPlusPunctuation() },
        { "M", &project->GetTotalMonoSyllabicWords() },
        { "C", &project->GetTotal3PlusSyllabicWords() },
        { "L", &project->GetTotalLongWords() },
        { "X", &project->GetTotalHardLixRixWords() },
        { "U", &project->GetTotalSentenceUnits() },
        { _DT("UDC"), &project->GetTotalHardWordsDaleChall() },
        { _DT("UUS"), &project->GetTotalUniqueHardWordsSpache() },
        { "T", &project->GetTotalMiniWords() },
        { "F", &project->GetTotalHardWordsFog() },
        // constants for text exclusion/word & sentence counting
        { _DT("Default"), 0.0 },
        { "DaleChall", 1.0 },
        { "HarrisJacobson", 2.0 },
        { "GunningFog", 3.0 },
        // constants for syllable counting
        { "NumeralsFullySyllabized", 1.0 },
        { "NumeralsAreOneSyllable", 2.0 } });
    }
