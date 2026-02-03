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

#include "base_project.h"
#include "../Wisteria-Dataviz/src/import/cpp_extract_text.h"
#include "../Wisteria-Dataviz/src/import/docx_extract_text.h"
#include "../Wisteria-Dataviz/src/import/hhc_hhk_extract_text.h"
#include "../Wisteria-Dataviz/src/import/idl_extract_text.h"
#include "../Wisteria-Dataviz/src/import/markdown_extract_text.h"
#include "../Wisteria-Dataviz/src/import/odt_odp_extract_text.h"
#include "../Wisteria-Dataviz/src/import/postscript_extract_text.h"
#include "../Wisteria-Dataviz/src/import/pptx_extract_text.h"
#include "../Wisteria-Dataviz/src/utfcpp/source/utf8.h"
#include "../app/readability_app.h"
#include "../indexing/diacritics.h"
#include "../indexing/romanize.h"
#include "../results-format/project_report_format.h"
#include "../results-format/word_collectiont_text_formatting.h"
#include "base_project_view.h"
#include <wx/richmsgdlg.h>

wxDECLARE_APP(ReadabilityApp);

grammar::phrase_collection BaseProject::m_englishWordyPhrases;
grammar::phrase_collection BaseProject::m_spanishWordyPhrases;
grammar::phrase_collection BaseProject::m_germanWordyPhrases;
grammar::phrase_collection BaseProject::m_copyrightNoticePhrases;
grammar::phrase_collection BaseProject::m_citationPhrases;
word_list BaseProject::m_knownProperNouns;
word_list BaseProject::m_knownPersonalNouns;
word_list BaseProject::known_english_spellings;
word_list BaseProject::known_programming_spellings;
word_list BaseProject::known_custom_english_spellings;
word_list BaseProject::known_spanish_spellings;
word_list BaseProject::known_custom_spanish_spellings;
word_list BaseProject::known_german_spellings;
word_list BaseProject::known_custom_german_spellings;
word_list BaseProject::m_dale_chall_word_list;
word_list BaseProject::m_dale_chall_plus_stocker_catholic_word_list;
word_list BaseProject::m_stocker_catholic_word_list;
word_list BaseProject::m_spache_word_list;
word_list BaseProject::m_harris_jacobson_word_list;
word_list BaseProject::english_stoplist;
word_list BaseProject::spanish_stoplist;
word_list BaseProject::german_stoplist;
word_list_with_replacements BaseProject::dale_chall_replacement_list;
word_list_with_replacements BaseProject::spache_replacement_list;
word_list_with_replacements BaseProject::harris_jacobson_replacement_list;
word_list_with_replacements BaseProject::difficult_word_replacement_list;
readability::dolch_word_list BaseProject::m_dolch_word_list;
CustomReadabilityTestCollection BaseProject::m_custom_word_tests;
readability::readability_test_collection<readability::readability_test>
    BaseProject::m_defaultReadabilityTestsTemplate;
std::map<wxString, int> BaseProject::m_testIdMap;
std::set<TestBundle> BaseProject::m_testBundles;
// defines the stat goals' internal labels, display labels, and lambdas to calculate them
std::map<comparable_first_pair<Goal::string_type, Goal::string_type>,
         std::function<double(const BaseProject*)>>
    BaseProject::m_statGoalLabels = {
        { { _DT(L"average-sentence-length"), _(L"Average Sentence Length").wc_str() },
          [](const BaseProject* proj)
          { return safe_divide<double>(proj->GetTotalWords(), proj->GetTotalSentences()); } },
        { { _DT(L"sentences-per-100-words"), _(L"Sentences per 100 Words").wc_str() },
          [](const BaseProject* proj)
          { return safe_divide<double>(100, proj->GetTotalWords()) * proj->GetTotalSentences(); } },
        { { _DT(L"syllables-per-100-words"), _(L"Syllables per 100 Words").wc_str() },
          [](const BaseProject* proj)
          { return safe_divide<double>(100, proj->GetTotalWords()) * proj->GetTotalSyllables(); } }
    };

//-------------------------------------------------------
bool BaseProject::LoadAppendedDocument()
    {
    if (!GetAppendedDocumentFilePath().empty())
        {
        if (!wxFile::Exists(GetAppendedDocumentFilePath()))
            {
            wxString fileBySameNameInProjectDirectory;
            if (FindMissingFile(GetAppendedDocumentFilePath(), fileBySameNameInProjectDirectory))
                {
                SetAppendedDocumentFilePath(fileBySameNameInProjectDirectory);
                }
            else
                {
                LogMessage(wxString::Format(_(L"\"%s\": appended template file not found."),
                                            GetAppendedDocumentFilePath()),
                           wxGetApp().GetAppName(), wxOK | wxICON_EXCLAMATION);
                SetAppendedDocumentText(std::wstring{});
                return false;
                }
            }
        MemoryMappedFile sourceFile(GetAppendedDocumentFilePath(), true, true);
        std::pair<bool, std::wstring> extractResult = ExtractRawText(
            { static_cast<const char*>(sourceFile.GetStream()), sourceFile.GetMapSize() },
            wxFileName(GetAppendedDocumentFilePath()).GetExt());
        SetAppendedDocumentText(extractResult.first ? std::move(extractResult.second) :
                                                      std::wstring{});
        return extractResult.first;
        }
    SetAppendedDocumentText(std::wstring{});
    return true;
    }

//-------------------------------------------------------
void BaseProject::UpdateDocumentSettings()
    {
    // cppcheck-suppress assertWithSideEffect
    assert(GetWords() != nullptr && L"Invalid word collection when updating document settings!");
    GetWords()->set_allowable_incomplete_sentence_size(
        GetIncludeIncompleteSentencesIfLongerThanValue());
    GetWords()->set_aggressive_exclusion(IsExcludingAggressively());
    GetWords()->ignore_trailing_copyright_notice_paragraphs(
        IsExcludingTrailingCopyrightNoticeParagraphs());
    GetWords()->ignore_citation_sections(IsExcludingTrailingCitations());
    GetWords()->treat_header_words_as_valid(GetInvalidSentenceMethod() ==
                                            InvalidSentence::ExcludeExceptForHeadings);
    GetWords()->treat_eol_as_eos(m_paragraphsParsingMethod ==
                                 ParagraphParse::EachNewLineIsAParagraph);
    GetWords()->ignore_blank_lines_when_determining_paragraph_split(
        IsIgnoringBlankLinesForParagraphsParser());
    GetWords()->ignore_indenting_when_determining_paragraph_split(
        IsIgnoringIndentingForParagraphsParser());
    GetWords()->sentence_start_must_be_uppercased(GetSentenceStartMustBeUppercased());
    GetWords()->set_copyright_phrase_function(&m_copyrightNoticePhrases);
    GetWords()->set_citation_phrase_function(&m_citationPhrases);
    GetWords()->set_known_proper_nouns(&m_knownProperNouns);
    GetWords()->set_known_personal_nouns(&m_knownPersonalNouns);
    GetWords()->get_spell_checker().set_programmer_word_list(&known_programming_spellings);
    GetWords()->get_spell_checker().ignore_proper_nouns(SpellCheckIsIgnoringProperNouns());
    GetWords()->get_spell_checker().ignore_uppercased(SpellCheckIsIgnoringUppercased());
    GetWords()->get_spell_checker().ignore_numerals(SpellCheckIsIgnoringNumerals());
    GetWords()->get_spell_checker().ignore_file_addresses(SpellCheckIsIgnoringFileAddresses());
    GetWords()->get_spell_checker().ignore_programmer_code(SpellCheckIsIgnoringProgrammerCode());
    GetWords()->get_spell_checker().allow_colloquialisms(SpellCheckIsAllowingColloquialisms());
    GetWords()->get_spell_checker().ignore_social_media_tags(SpellCheckIsIgnoringSocialMediaTags());
    GetWords()->exclude_file_addresses(IsExcludingFileAddresses());
    GetWords()->exclude_numerals(IsExcludingNumerals());
    GetWords()->exclude_proper_nouns(IsExcludingProperNouns());
    GetWords()->set_excluded_phrase_function(m_excluded_phrases);
    GetWords()->include_excluded_phrase_first_occurrence(
        IsIncludingExcludedPhraseFirstOccurrence());
    GetWords()->clear_exclusion_block_tags();
    for (const auto& exclusionBlockTag : m_exclusionBlockTags)
        {
        GetWords()->add_exclusion_block_tags(exclusionBlockTag.first, exclusionBlockTag.second);
        }
    // language-specific settings
    if (GetProjectLanguage() == readability::test_language::spanish_test)
        {
        GetWords()->set_syllabizer(&m_spanish_syllabize);
        GetWords()->set_stemmer(&m_spanish_stem);
        GetWords()->set_stop_list(&GetStopList());
        GetWords()->set_conjunction_function(&m_spanish_conjunction);
        GetWords()->set_known_phrase_function(&m_spanishWordyPhrases);
        GetWords()->get_spell_checker().set_word_list(&known_spanish_spellings);
        GetWords()->get_spell_checker().set_secondary_word_list(&known_custom_spanish_spellings);
        GetWords()->set_search_for_proper_nouns(true);
        GetWords()->set_mismatched_article_function(nullptr);
        GetWords()->set_search_for_passive_voice(false);
        }
    else if (GetProjectLanguage() == readability::test_language::german_test)
        {
        GetWords()->set_syllabizer(&m_german_syllabize);
        GetWords()->set_stemmer(&m_german_stem);
        GetWords()->set_stop_list(&GetStopList());
        GetWords()->set_conjunction_function(&m_german_conjunction);
        GetWords()->set_known_phrase_function(&m_germanWordyPhrases);
        GetWords()->get_spell_checker().set_word_list(&known_german_spellings);
        GetWords()->get_spell_checker().set_secondary_word_list(&known_custom_german_spellings);
        GetWords()->set_search_for_proper_nouns(false);
        GetWords()->set_mismatched_article_function(nullptr);
        GetWords()->set_search_for_passive_voice(false);
        }
    else
        {
        GetWords()->set_syllabizer(&m_english_syllabize);
        GetWords()->set_stemmer(&m_english_stem);
        GetWords()->set_stop_list(&GetStopList());
        GetWords()->set_conjunction_function(&m_english_conjunction);
        GetWords()->set_known_phrase_function(&m_englishWordyPhrases);
        GetWords()->get_spell_checker().set_word_list(&known_english_spellings);
        GetWords()->get_spell_checker().set_secondary_word_list(&known_custom_english_spellings);
        GetWords()->set_search_for_proper_nouns(true);
        GetWords()->set_mismatched_article_function(&m_english_mismatched_article);
        GetWords()->set_search_for_passive_voice(true);
        }
    }

//-------------------------------------------------------
void BaseProject::LogMessage(wxString message, const wxString& title, const int icon,
                             const wxString& messageId /*= wxString{}*/,
                             const bool queue /*= false*/)
    {
    // skip warning if it was asked to be suppressed already
    const auto warningIter = WarningManager::GetWarning(messageId);
    if (warningIter != WarningManager::GetWarnings().end() && !warningIter->ShouldBeShown())
        {
        return;
        }

    if (queue)
        {
        AddQueuedMessage(WarningMessage(messageId, message, title, wxString{}, icon));
        }
    else if (HasUI())
        {
        wxRichMessageDialog msg(wxGetApp().GetParentingWindow(), message,
                                (!title.empty() ? title : wxGetApp().GetAppDisplayName()), icon);
        msg.ShowCheckBox(!messageId.empty() ? _(L"Do not show this again") : wxString{});
        const int dlgResponse = msg.ShowModal();
        if (dlgResponse != wxID_NO && warningIter != WarningManager::GetWarnings().end() &&
            msg.IsCheckBoxChecked())
            {
            warningIter->Show(false);
            warningIter->SetPreviousResponse(dlgResponse);
            }
        }
    else
        {
        AddQuietSubProjectMessage(message, icon);
        }
    message.Replace(L"\n", L" ", true);
    if ((icon & wxICON_ERROR) != 0)
        {
        wxLogError(L"%s", message);
        }
    else if ((icon & wxICON_EXCLAMATION) != 0)
        {
        wxLogWarning(L"%s", message);
        }
    else
        {
        wxLogVerbose(L"%s", message);
        }
    }

//-------------------------------------------------------
void BaseProject::FormatFilteredText(std::wstring& text, const bool romanizeText,
                                     const bool removeEllipses, const bool removeBullets,
                                     const bool removeFilePaths, const bool stripAbbreviations,
                                     const bool narrowFullWithText) const
    {
    auto project = std::make_unique<BaseProject>();
    project->CopySettings(*this);
    project->SetAppendedDocumentText(GetAppendedDocumentText());
    project->ShareExcludePhrases(*this);
    project->SetUIMode(false);
    project->SetOriginalDocumentFilePath(GetOriginalDocumentFilePath());

    if (project->GetDocumentStorageMethod() == TextStorage::NoEmbedText)
        {
        project->FreeDocumentText();
        }
    if (!project->LoadDocumentAsSubProject(project->GetOriginalDocumentFilePath(),
                                           project->GetDocumentText(), 1) ||
        project->GetDocumentText().empty())
        {
        return;
        }
    text.reserve(project->GetDocumentText().length());
    FormatFilteredWordCollection(
        project->GetWords(), text,
        (GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences) ?
            InvalidTextFilterFormat::IncludeAllText :
            InvalidTextFilterFormat::IncludeOnlyValidText,
        removeFilePaths, stripAbbreviations);
    string_util::trim(text);
    text.insert(0, L"\t");

    const text_transform::romanize romanize;
    text = romanize(std::wstring_view{ text }, romanizeText, removeEllipses, removeBullets,
                    narrowFullWithText);
    }

//-------------------------------------------------------
bool BaseProject::IsIncludingClozeTest() const
    {
    for (const auto& rTests : GetReadabilityTests().get_tests())
        {
        if (rTests.is_included() &&
            (rTests.get_test().get_test_type() ==
                 readability::readability_test_type::predicted_cloze_score ||
             rTests.get_test().get_test_type() ==
                 readability::readability_test_type::grade_level_and_predicted_cloze_score))
            {
            return true;
            }
        }

    return std::ranges::any_of(GetCustTestsInUse(),
                               [](const auto& customTest)
                               {
                                   return customTest.GetIterator()->get_test_type() ==
                                          readability::readability_test_type::predicted_cloze_score;
                               });
    }

//-------------------------------------------------------
bool BaseProject::IsIncludingGradeTest() const
    {
    for (const auto& rTests : GetReadabilityTests().get_tests())
        {
        if (rTests.is_included() &&
            (rTests.get_test().get_test_type() == readability::readability_test_type::grade_level ||
             rTests.get_test().get_test_type() ==
                 readability::readability_test_type::index_value_and_grade_level ||
             rTests.get_test().get_test_type() ==
                 readability::readability_test_type::grade_level_and_predicted_cloze_score))
            {
            return true;
            }
        }

    return std::ranges::any_of(GetCustTestsInUse(),
                               [](const auto& customTest)
                               {
                                   return customTest.GetIterator()->get_test_type() ==
                                          readability::readability_test_type::grade_level;
                               });
    }

//-------------------------------------------------------
BaseProject::BaseProject()
    : m_minDocWordCountForBatch(wxGetApp().GetAppOptions()->GetMinDocWordCountForBatch()),
      m_includeIncompleteSentencesIfLongerThan(
          wxGetApp().GetAppOptions()->GetIncludeIncompleteSentencesIfLongerThanValue()),

      m_difficultSentenceLength(wxGetApp().GetAppOptions()->GetDifficultSentenceLength()),

      m_numeralSyllabicationMethod(wxGetApp().GetAppOptions()->GetNumeralSyllabicationMethod()),
      m_longSentenceMethod(wxGetApp().GetAppOptions()->GetLongSentenceMethod()),
      m_paragraphsParsingMethod(wxGetApp().GetAppOptions()->GetParagraphsParsingMethod()),
      m_invalidSentenceMethod(wxGetApp().GetAppOptions()->GetInvalidSentenceMethod()),
      m_textSource(wxGetApp().GetAppOptions()->GetTextSource()),
      m_documentStorageMethod(wxGetApp().GetAppOptions()->GetDocumentStorageMethod()),
      m_varianceMethod(wxGetApp().GetAppOptions()->GetVarianceMethod()),
      // readability scores options
      m_includeScoreSummaryReport(wxGetApp().GetAppOptions()->IsIncludingScoreSummaryReport()),
      // test options
      m_hjTextExclusion(wxGetApp().GetAppOptions()->GetHarrisJacobsonTextExclusionMode()),
      m_dcTextExclusion(wxGetApp().GetAppOptions()->GetDaleChallTextExclusionMode()),
      m_dcProperNounCountingMethod(
          wxGetApp().GetAppOptions()->GetDaleChallProperNounCountingMethod()),
      m_fleschNumeralSyllabizeMethod(wxGetApp().GetAppOptions()->GetFleschNumeralSyllabizeMethod()),
      m_fleschKincaidNumeralSyllabizeMethod(
          wxGetApp().GetAppOptions()->GetFleschKincaidNumeralSyllabizeMethod()),
      // language options
      m_language(wxGetApp().GetAppOptions()->GetProjectLanguage()),
      m_reviewer(wxGetApp().GetAppOptions()->GetReviewer()),

      // grammar
      m_spellcheck_ignore_proper_nouns(
          wxGetApp().GetAppOptions()->SpellCheckIsIgnoringProperNouns()),
      m_spellcheck_ignore_uppercased(wxGetApp().GetAppOptions()->SpellCheckIsIgnoringUppercased()),
      m_spellcheck_ignore_numerals(wxGetApp().GetAppOptions()->SpellCheckIsIgnoringNumerals()),
      m_spellcheck_ignore_file_addresses(
          wxGetApp().GetAppOptions()->SpellCheckIsIgnoringFileAddresses()),
      m_spellcheck_ignore_programmer_code(
          wxGetApp().GetAppOptions()->SpellCheckIsIgnoringProgrammerCode()),
      m_allow_colloquialisms(wxGetApp().GetAppOptions()->SpellCheckIsAllowingColloquialisms()),
      m_spellcheck_ignore_social_media_tags(
          wxGetApp().GetAppOptions()->SpellCheckIsIgnoringSocialMediaTags()),

      // analysis
      m_ignoreBlankLinesForParagraphsParser(
          wxGetApp().GetAppOptions()->IsIgnoringBlankLinesForParagraphsParser()),
      m_ignoreIndentingForParagraphsParser(
          wxGetApp().GetAppOptions()->IsIgnoringIndentingForParagraphsParser()),
      m_sentenceStartMustBeUppercased(
          wxGetApp().GetAppOptions()->GetSentenceStartMustBeUppercased()),
      m_aggressiveExclusion(wxGetApp().GetAppOptions()->IsExcludingAggressively()),
      m_excludeTrailingCopyrightNoticeParagraphs(
          wxGetApp().GetAppOptions()->IsExcludingTrailingCopyrightNoticeParagraphs()),
      m_excludeTrailingCitations(wxGetApp().GetAppOptions()->IsExcludingTrailingCitations()),
      m_excludeFileAddresses(wxGetApp().GetAppOptions()->IsExcludingFileAddresses()),
      m_excludeNumerals(wxGetApp().GetAppOptions()->IsExcludingNumerals()),
      m_excludeProperNouns(wxGetApp().GetAppOptions()->IsExcludingProperNouns()),
      m_includeExcludedPhraseFirstOccurrence(
          wxGetApp().GetAppOptions()->IsIncludingExcludedPhraseFirstOccurrence()),

      m_fogUseSentenceUnits(wxGetApp().GetAppOptions()->IsFogUsingSentenceUnits()),
      m_includeStockerCatholicDCSupplement(
          wxGetApp().GetAppOptions()->IsIncludingStockerCatholicSupplement()),

      m_statsInfo(wxGetApp().GetAppOptions()->GetStatisticsInfo()),
      m_statsReportInfo(wxGetApp().GetAppOptions()->GetStatisticsReportInfo()),
      m_grammarInfo(wxGetApp().GetAppOptions()->GetGrammarInfo()),
      m_wordsBreakdownInfo(wxGetApp().GetAppOptions()->GetWordsBreakdownInfo()),
      m_sentencesBreakdownInfo(wxGetApp().GetAppOptions()->GetSentencesBreakdownInfo()),
      m_excludedPhrasesPath(wxGetApp().GetAppOptions()->GetExcludedPhrasesPath()),
      m_exclusionBlockTags(wxGetApp().GetAppOptions()->GetExclusionBlockTags()),
      m_appendedDocumentFilePath(wxGetApp().GetAppOptions()->GetAppendedDocumentFilePath())
    {
    ResetStandardReadabilityTests(m_readabilityTests);
    // bind the standard test functions with their respective IDs
    m_standardTestFunctions.reserve(GetDefaultReadabilityTestsTemplate().get_test_count());
    // DEGREES_OF_READING_POWER
    auto testPos = m_testIdMap.find(ReadabilityMessages::DEGREES_OF_READING_POWER());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddDegreesOfReadingPowerTest));
        }
    // DEGREES_OF_READING_POWER_GE
    testPos = m_testIdMap.find(ReadabilityMessages::DEGREES_OF_READING_POWER_GE());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddDegreesOfReadingPowerGeTest));
        }
    // SOL_SPANISH
    testPos = m_testIdMap.find(ReadabilityMessages::SOL_SPANISH());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddSolSpanishTest));
        }
    // CRAWFORD
    testPos = m_testIdMap.find(ReadabilityMessages::CRAWFORD());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddCrawfordTest));
        }
    // INFLESZ
    testPos = m_testIdMap.find(ReadabilityMessages::INFLESZ());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddInfleszTest));
        }
    // FRASE
    testPos = m_testIdMap.find(ReadabilityMessages::FRASE());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(std::make_pair(testPos->second, &BaseProject::AddFraseTest));
        }
    // GPM_FRY
    testPos = m_testIdMap.find(ReadabilityMessages::GPM_FRY());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddGilliamPenaMountainFryTest));
        }
    // PSK_FARR_JENKINS_PATERSON
    testPos = m_testIdMap.find(ReadabilityMessages::PSK_FARR_JENKINS_PATERSON());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddPskFarrJenkinsPatersonTest));
        }
    // SPACHE
    testPos = m_testIdMap.find(ReadabilityMessages::SPACHE());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddSpacheTest));
        }
    // HARRIS_JACOBSON
    testPos = m_testIdMap.find(ReadabilityMessages::HARRIS_JACOBSON());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddHarrisJacobsonTest));
        }
    // ARI
    testPos = m_testIdMap.find(ReadabilityMessages::ARI());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(std::make_pair(testPos->second, &BaseProject::AddAriTest));
        }
    // GUNNING_FOG
    testPos = m_testIdMap.find(ReadabilityMessages::GUNNING_FOG());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(std::make_pair(testPos->second, &BaseProject::AddFogTest));
        }
    // LIX
    testPos = m_testIdMap.find(ReadabilityMessages::LIX());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(std::make_pair(testPos->second, &BaseProject::AddLixTest));
        }
    // RIX
    testPos = m_testIdMap.find(ReadabilityMessages::RIX());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(std::make_pair(testPos->second, &BaseProject::AddRixTest));
        }
    // DALE_CHALL
    testPos = m_testIdMap.find(ReadabilityMessages::DALE_CHALL());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddNewDaleChallTest));
        }
    // COLEMAN_LIAU
    testPos = m_testIdMap.find(ReadabilityMessages::COLEMAN_LIAU());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddColemanLiauTest));
        }
    // FORCAST
    testPos = m_testIdMap.find(ReadabilityMessages::FORCAST());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddForcastTest));
        }
    // FLESCH
    testPos = m_testIdMap.find(ReadabilityMessages::FLESCH());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddFleschTest));
        }
    // NEW_FOG
    testPos = m_testIdMap.find(ReadabilityMessages::NEW_FOG());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddNewFogCountTest));
        }
    // FLESCH_KINCAID_SIMPLIFIED
    testPos = m_testIdMap.find(ReadabilityMessages::FLESCH_KINCAID_SIMPLIFIED());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddFleschKincaidSimplifiedTest));
        }
    // FLESCH_KINCAID
    testPos = m_testIdMap.find(ReadabilityMessages::FLESCH_KINCAID());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddFleschKincaidTest));
        }
    // EFLAW
    testPos = m_testIdMap.find(ReadabilityMessages::EFLAW());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(std::make_pair(testPos->second, &BaseProject::AddEflawTest));
        }
    // SMOG
    testPos = m_testIdMap.find(ReadabilityMessages::SMOG());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(std::make_pair(testPos->second, &BaseProject::AddSmogTest));
        }
    // SMOG_BAMBERGER_VANECEK
    testPos = m_testIdMap.find(ReadabilityMessages::SMOG_BAMBERGER_VANECEK());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddSmogBambergerVanecekTest));
        }
    // SCHWARTZ
    testPos = m_testIdMap.find(ReadabilityMessages::SCHWARTZ());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddSchwartzTest));
        }
    // QU
    testPos = m_testIdMap.find(ReadabilityMessages::QU());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddQuBambergerVanecekTest));
        }
    // MODIFIED_SMOG
    testPos = m_testIdMap.find(ReadabilityMessages::MODIFIED_SMOG());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddModifiedSmogTest));
        }
    // SMOG_SIMPLIFIED
    testPos = m_testIdMap.find(ReadabilityMessages::SMOG_SIMPLIFIED());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddSmogSimplifiedTest));
        }
    // SIMPLE_ARI
    testPos = m_testIdMap.find(ReadabilityMessages::SIMPLE_ARI());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddSimplifiedAriTest));
        }
    // NEW_ARI
    testPos = m_testIdMap.find(ReadabilityMessages::NEW_ARI());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddNewAriTest));
        }
    // PSK_FLESCH
    testPos = m_testIdMap.find(ReadabilityMessages::PSK_FLESCH());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddPskFleschTest));
        }
    // FRY
    testPos = m_testIdMap.find(ReadabilityMessages::FRY());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(std::make_pair(testPos->second, &BaseProject::AddFryTest));
        }
    // RAYGOR
    testPos = m_testIdMap.find(ReadabilityMessages::RAYGOR());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddRaygorTest));
        }
    // PSK_DALE_CHALL
    testPos = m_testIdMap.find(ReadabilityMessages::PSK_DALE_CHALL());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddPskDaleChallTest));
        }
    // BORMUTH_CLOZE_MEAN
    testPos = m_testIdMap.find(ReadabilityMessages::BORMUTH_CLOZE_MEAN());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddBormuthClozeMeanTest));
        }
    // BORMUTH_GRADE_PLACEMENT_35
    testPos = m_testIdMap.find(ReadabilityMessages::BORMUTH_GRADE_PLACEMENT_35());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddBormuthGradePlacement35Test));
        }
    // FARR_JENKINS_PATERSON
    testPos = m_testIdMap.find(ReadabilityMessages::FARR_JENKINS_PATERSON());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddFarrJenkinsPatersonTest));
        }
    // PSK_GUNNING_FOG
    testPos = m_testIdMap.find(ReadabilityMessages::PSK_GUNNING_FOG());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddPskFogTest));
        }
    // NEW_FARR_JENKINS_PATERSON
    testPos = m_testIdMap.find(ReadabilityMessages::NEW_FARR_JENKINS_PATERSON());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddNewFarrJenkinsPatersonTest));
        }
    // WHEELER_SMITH
    testPos = m_testIdMap.find(ReadabilityMessages::WHEELER_SMITH());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddWheelerSmithTest));
        }
    // AMSTAD
    testPos = m_testIdMap.find(ReadabilityMessages::AMSTAD());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddAmstadTest));
        }
    // WHEELER_SMITH_BAMBERGER_VANECEK
    testPos = m_testIdMap.find(ReadabilityMessages::WHEELER_SMITH_BAMBERGER_VANECEK());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddWheelerSmithBambergerVanecekTest));
        }
    // NEUE_WIENER_SACHTEXTFORMEL1
    testPos = m_testIdMap.find(ReadabilityMessages::NEUE_WIENER_SACHTEXTFORMEL1());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddNeueWienerSachtextformel1));
        }
    // NEUE_WIENER_SACHTEXTFORMEL2
    testPos = m_testIdMap.find(ReadabilityMessages::NEUE_WIENER_SACHTEXTFORMEL2());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddNeueWienerSachtextformel2));
        }
    // NEUE_WIENER_SACHTEXTFORMEL3
    testPos = m_testIdMap.find(ReadabilityMessages::NEUE_WIENER_SACHTEXTFORMEL3());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddNeueWienerSachtextformel3));
        }
    // LIX_GERMAN_CHILDRENS_LITERATURE
    testPos = m_testIdMap.find(ReadabilityMessages::LIX_GERMAN_CHILDRENS_LITERATURE());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddLixGermanChildrensLiterature));
        }
    // LIX_GERMAN_TECHNICAL
    testPos = m_testIdMap.find(ReadabilityMessages::LIX_GERMAN_TECHNICAL());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddLixGermanTechnical));
        }
    // RIX_GERMAN_FICTION
    testPos = m_testIdMap.find(ReadabilityMessages::RIX_GERMAN_FICTION());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddRixGermanFiction));
        }
    // RIX_GERMAN_NONFICTION
    testPos = m_testIdMap.find(ReadabilityMessages::RIX_GERMAN_NONFICTION());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddRixGermanNonFiction));
        }
    // ELF
    testPos = m_testIdMap.find(ReadabilityMessages::ELF());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(std::make_pair(testPos->second, &BaseProject::AddElfTest));
        }
    // DANIELSON_BRYAN_1
    testPos = m_testIdMap.find(ReadabilityMessages::DANIELSON_BRYAN_1());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddDanielsonBryan1Test));
        }
    // DANIELSON_BRYAN_2
    testPos = m_testIdMap.find(ReadabilityMessages::DANIELSON_BRYAN_2());
    if (testPos != m_testIdMap.cend())
        {
        m_standardTestFunctions.insert(
            std::make_pair(testPos->second, &BaseProject::AddDanielsonBryan2Test));
        }

    m_readMessages.SetReadingAgeDisplay(
        wxGetApp().GetAppOptions()->GetReadabilityMessageCatalog().GetReadingAgeDisplay());
    m_readMessages.SetGradeScale(
        wxGetApp().GetAppOptions()->GetReadabilityMessageCatalog().GetGradeScale());
    m_readMessages.SetLongGradeScaleFormat(
        wxGetApp().GetAppOptions()->GetReadabilityMessageCatalog().IsUsingLongGradeScaleFormat());
    }

//-------------------------------------------------------
void BaseProject::ResetStandardReadabilityTests(TestCollectionType& readabilityTests)
    {
    readabilityTests.clear();
    readabilityTests.reserve(m_defaultReadabilityTestsTemplate.get_test_count());
    readabilityTests.add_tests(m_defaultReadabilityTestsTemplate.get_tests());
    }

//-------------------------------------------------------
void BaseProject::InitializeStandardGermanReadabilityTests()
    {
        // Amstad
        {
        readability::readability_test test(
            ReadabilityMessages::AMSTAD(), XRCID("ID_AMSTAD"), _DT(L"Amstad"), _DT(L"Amstad"),
            // TRANSLATORS: "Flesch Reading Ease" should not be translated.
            _(L"Amstad is a recalculation of Flesch Reading Ease for German text. "
              "It is meant for secondary and adult-level text. "
              "Scores range from 0-100 (the higher the score, the easier to read). "
              "Average documents should be within the range of 60-70."),
            readability::readability_test_type::index_value, true,
            _DT(L"180 - (W/S) - (58.5*(B/W))"));
        test.add_document_classification(readability::document_classification::general_document,
                                         true);
        test.add_document_classification(readability::document_classification::technical_document,
                                         true);
        test.add_document_classification(
            readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::adult_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_healthcare_industry, true);
        test.add_industry_classification(
            readability::industry_classification::adult_healthcare_industry, true);
        test.add_industry_classification(
            readability::industry_classification::military_government_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_complexity);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // Schwartz (German graph)
        {
        readability::readability_test test(
            ReadabilityMessages::SCHWARTZ(), XRCID("ID_SCHWARTZ"), _DT(L"Schwartz"),
            _DT(L"Schwartz German Readability Graph"),
            _(L"The <a href=\"#schwartz\">Schwartz German Readability Graph</a> "
              "is designed for classroom instructional materials."),
            readability::readability_test_type::grade_level, true, L"");
        test.add_document_classification(
            readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_complexity);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // Neue Wiener Sachtextformel 1
        {
        readability::readability_test test(
            ReadabilityMessages::NEUE_WIENER_SACHTEXTFORMEL1(),
            XRCID("ID_NEUE_WIENER_SACHTEXTFORMEL1"), _DT(L"1.nWS"),
            _DT(L"Neue Wiener Sachtextformel (1)"),
            _(L"This test is meant for secondary-age non-fiction "
              "(specifically, 6-10 grade materials)."),
            readability::readability_test_type::grade_level, false,
            _DT(L"0.1935*((C/W)*100) + 0.1672*(W/S) + 0.1297*((X/W)*100) "
                "- 0.0327*((M/W)*100) - 0.875"));
        test.add_document_classification(
            readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_complexity_3_plus_syllables);
        test.add_factor(readability::test_factor::word_length_7_plus);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // Neue Wiener Sachtextformel 2
        {
        readability::readability_test test(
            ReadabilityMessages::NEUE_WIENER_SACHTEXTFORMEL2(),
            XRCID("ID_NEUE_WIENER_SACHTEXTFORMEL2"), _DT(L"2.nWS"),
            _DT(L"Neue Wiener Sachtextformel (2)"),
            _(L"This test is meant for primary-age non-fiction "
              "(specifically, light materials up to the 5th grade)."),
            readability::readability_test_type::grade_level, false,
            _DT(L"0.2007*((C/W)*100) + 0.1682*(W/S) + 0.1373*((X/W)*100) - 2.779"));
        test.add_document_classification(
            readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_complexity_3_plus_syllables);
        test.add_factor(readability::test_factor::word_length_7_plus);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // Neue Wiener Sachtextformel 3
        {
        readability::readability_test test(
            ReadabilityMessages::NEUE_WIENER_SACHTEXTFORMEL3(),
            XRCID("ID_NEUE_WIENER_SACHTEXTFORMEL3"), _DT(L"3.nWS"),
            _DT(L"Neue Wiener Sachtextformel (3)"),
            _(L"This test is meant for primary-age non-fiction "
              "(specifically, light materials up to the 5th grade)."),
            readability::readability_test_type::grade_level, false,
            _DT(L"0.2963*((C/W)*100) + 0.1905*(W/S) - 1.1144"));
        test.add_document_classification(
            readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_complexity_3_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // wheeler-smith (BV)
        {
        readability::readability_test test(
            ReadabilityMessages::WHEELER_SMITH_BAMBERGER_VANECEK(),
            XRCID("ID_WHEELER_SMITH_BAMBERGER_VANECEK"), _DT(L"Wheeler-Smith (Bamberger-Vanecek)"),
            _DT(L"Wheeler-Smith (Bamberger-Vanecek)"),
            _(L"German variation of Wheeler-Smith, which is meant for "
              "primary-age reading materials."),
            readability::readability_test_type::grade_level, false, L"");
        test.add_document_classification(
            readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_healthcare_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_complexity_3_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // rix (German fiction)
        {
        readability::readability_test test(
            ReadabilityMessages::RIX_GERMAN_FICTION(), XRCID("ID_RIX_GERMAN_FICTION"),
            _(L"Rix (German fiction)"), _(L"Rix (German fiction)"),
            _(L"This test is a variation of Rix meant for German fiction "
              "(specifically, grades 1-11)."),
            readability::readability_test_type::index_value_and_grade_level, true,
            // needs a table to convert formula to grade level, so leave this blank
            L"");
        test.add_document_classification(
            readability::document_classification::childrens_literature_document, true);
        test.add_document_classification(
            readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::adult_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_length_7_plus);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // rix (German non-fiction)
        {
        readability::readability_test test(
            ReadabilityMessages::RIX_GERMAN_NONFICTION(), XRCID("ID_RIX_GERMAN_NONFICTION"),
            _(L"Rix (German non-fiction)"), _(L"Rix (German non-fiction)"),
            _(L"This test is a variation of Rix meant for German non-fiction "
              "(specifically, secondary-age to adult level materials)."),
            readability::readability_test_type::index_value_and_grade_level, true, L"");
        test.add_document_classification(
            readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::adult_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_length_7_plus);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // lix (German children's literature)
        {
        readability::readability_test test(
            ReadabilityMessages::LIX_GERMAN_CHILDRENS_LITERATURE(),
            XRCID("ID_LIX_GERMAN_CHILDRENS_LITERATURE"), _(L"Lix (German children's literature)"),
            _(L"Lix (German children's literature)"),
            _(L"This test is a variation of Lix meant for German children's "
              "literature (specifically, grades 1-8). "
              "A <a href=\"#german-lix-gauge\">chart</a> is also available to "
              "visualize the score."),
            readability::readability_test_type::index_value_and_grade_level, true, L"");
        test.add_document_classification(
            readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_length_7_plus);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // lix (German technical books)
        {
        readability::readability_test test(
            ReadabilityMessages::LIX_GERMAN_TECHNICAL(), XRCID("ID_LIX_GERMAN_TECHNICAL"),
            _(L"Lix (German technical literature)"), _(L"Lix (German technical literature)"),
            _(L"This test is a variation of Lix meant for German technical/non-fiction literature. "
              "A <a href=\"#german-lix-gauge\">chart</a> is also available to "
              "visualize the score."),
            readability::readability_test_type::index_value_and_grade_level, true, L"");
        test.add_document_classification(readability::document_classification::technical_document,
                                         true);
        test.add_document_classification(
            readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::adult_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_length_7_plus);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // Qu (german)
        {
        readability::readability_test test(ReadabilityMessages::QU(), XRCID("ID_QU"), _DT(L"Qu"),
                                           _DT(L"Qu (Quadratwurzelverfahren)"),
                                           _(L"Qu is generally appropriate for secondary age "
                                             "(4th grade to college level) readers."),
                                           readability::readability_test_type::grade_level, false,
                                           _DT(L"TRUNC(SQRT((C*(100/W))/(S*(100/W))*30) - 2)"));
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_complexity_3_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // smog (german)
        {
        readability::readability_test test(
            ReadabilityMessages::SMOG_BAMBERGER_VANECEK(), XRCID("ID_SMOG_BAMBERGER_VANECEK"),
            _DT(L"SMOG (Bamberger-Vanecek)"), _DT(L"SMOG (Bamberger-Vanecek)"),
            _(L"A German adaption of SMOG, which is generally appropriate for secondary age "
              "(4th grade to college level) readers. SMOG tests for 100% comprehension, "
              "whereas most formulas test for around 50%-75% comprehension."),
            readability::readability_test_type::grade_level, false,
            _DT(L"TRUNC(SQRT(C*(30/S)) - 2)"));
        test.add_document_classification(readability::document_classification::general_document,
                                         true);
        test.add_document_classification(
            readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::adult_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_healthcare_industry, true);
        test.add_industry_classification(
            readability::industry_classification::adult_healthcare_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_complexity_3_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }
    }

//-------------------------------------------------------
void BaseProject::InitializeStandardSpanishReadabilityTests()
    {
        // SOL (Spanish SMOG)
        {
        readability::readability_test test(
            ReadabilityMessages::SOL_SPANISH(), XRCID("ID_SOL_SPANISH"), _DT(L"SOL"),
            _DT(L"SOL (Spanish SMOG)"),
            _(L"SOL is meant for secondary-age (4th grade to college level) "
              "Spanish reading materials. "
              "It is a modified version of SMOG that was recalibrated for Spanish text."),
            readability::readability_test_type::grade_level, false,
            _DT(L"(1.0430*SQRT(C*(30/S)) + 3.1291)*.74 - 2.51"));
        test.add_document_classification(readability::document_classification::general_document,
                                         true);
        test.add_document_classification(
            readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::adult_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_healthcare_industry, true);
        test.add_industry_classification(
            readability::industry_classification::adult_healthcare_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::spanish_test);
        test.add_factor(readability::test_factor::word_complexity_3_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // GPM (Spanish) fry graph
        {
        readability::readability_test test(
            ReadabilityMessages::GPM_FRY(), XRCID("ID_GPM_FRY"),
            _DT(L"Gilliam-Pe\U000000F1a-Mountain"), _DT(L"Gilliam-Pe\U000000F1a-Mountain Graph"),
            _(L"The <a href=\"#GPMFryGraph\">Gilliam-Pe\U000000F1a-Mountain graph</a> "
              "is designed for most text, including literature and technical documents."),
            readability::readability_test_type::grade_level, true, L"");
        test.add_document_classification(readability::document_classification::general_document,
                                         true);
        test.add_document_classification(
            readability::document_classification::adult_literature_document, true);
        test.add_document_classification(
            readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::adult_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_healthcare_industry, true);
        test.add_industry_classification(
            readability::industry_classification::adult_healthcare_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::spanish_test);
        test.add_factor(readability::test_factor::word_complexity);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // Frase (Spanish graph)
        {
        readability::readability_test test(
            ReadabilityMessages::FRASE(), XRCID("ID_FRASE"), _DT(L"FRASE"), _DT(L"FRASE Graph"),
            _(L"The <a href=\"#FRASE\">FRASE graph</a> is designed for educational materials "
              "(primarily Spanish as a secondary language)."),
            readability::readability_test_type::index_value, true, L"");
        test.add_document_classification(
            readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::secondary_language_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::second_language);
        test.add_language(readability::test_language::spanish_test);
        test.add_factor(readability::test_factor::word_complexity);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // Crawford
        {
        readability::readability_test test(
            ReadabilityMessages::CRAWFORD(), XRCID("ID_CRAWFORD"), _DT(L"Crawford"),
            _DT(L"Crawford"),
            _(L"The Crawford test is designed for primary-age Spanish reading materials. "
              "A <a href=\"#crawford-graph\">chart</a> is also available to visualize the score."),
            readability::readability_test_type::grade_level, false,
            _DT(L"(S*-.205) + (B*.049) - 3.407"));
        test.add_document_classification(
            readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_healthcare_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_language(readability::test_language::spanish_test);
        test.add_factor(readability::test_factor::word_complexity);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // INFLESZ
        {
        readability::readability_test test(
            ReadabilityMessages::INFLESZ(), XRCID("ID_INFLESZ"), _DT(L"INFLESZ"), _DT(L"INFLESZ"),
            _(L"The <a href=\"#inflesz\">INFLESZ Scale</a> is a Flesch Reading Ease "
              "like graph for Spanish. It is designed as a general purpose formula for most text."),
            readability::readability_test_type::index_value, true,
            // Szigriszt-Pazos Perspicuity scale, which needs to be plotted onto the INFLESZ Scale
            _DT(L"ROUND(CLAMP(206.835 - (W/S) - (62.3 * (B/W)), 0, 100))"));
        // general purpose test, just like FRE
        test.add_document_classification(readability::document_classification::general_document,
                                         true);
        test.add_document_classification(readability::document_classification::technical_document,
                                         true);
        test.add_document_classification(
            readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::adult_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_healthcare_industry, true);
        test.add_industry_classification(
            readability::industry_classification::adult_healthcare_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::spanish_test);
        test.add_factor(readability::test_factor::word_complexity);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }
    }

//-------------------------------------------------------
void BaseProject::InitializeStandardEnglishFamiliarWordReadabilityTests()
    {
        // Harris-Jacobson
        {
        readability::readability_test test(
            ReadabilityMessages::HARRIS_JACOBSON(), XRCID("ID_HARRIS_JACOBSON"),
            _DT(L"Harris-Jacobson"), _DT(L"Harris-Jacobson Wide Range Formula"),
            _(L"Harris-Jacobson is generally used for primary and secondary age "
              "(Kindergarten to 11th grade) readers."),
            readability::readability_test_type::grade_level, false, L"");
        test.add_document_classification(
            readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry, true);
        test.add_document_classification(
            readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_healthcare_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_familiarity_harris_jacobson);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // Spache
        {
        readability::readability_test test(
            ReadabilityMessages::SPACHE(), XRCID("ID_SPACHE"), _DT(L"Spache Revised"),
            _DT(L"Spache Revised"),
            _(L"Spache is generally used for primary age (Kindergarten to 7th grade) "
              "readers to help classify school textbooks and literature."),
            readability::readability_test_type::grade_level, false,
            _DT(L"(.121 * (W/S)) + (.082 * UUS*(100/W)) + .659"));
        test.add_document_classification(
            readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_familiarity_spache);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // New Dale-Chall
        {
        readability::readability_test test(
            ReadabilityMessages::DALE_CHALL(), XRCID("ID_NEW_DALE_CHALL"), _DT(L"New Dale-Chall"),
            _DT(L"New Dale-Chall"),
            // TRANSLATORS: "New Dale-Chall" should not be translated; it is a test name.
            _(L"New Dale-Chall is generally used for primary and secondary age readers "
              "to help classify school text books and literature."),
            readability::readability_test_type::grade_level, false, L"");
        test.add_document_classification(
            readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::adult_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_healthcare_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_familiarity_dale_chall);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // PSK (new dale-chall)
        {
        readability::readability_test test(
            ReadabilityMessages::PSK_DALE_CHALL(), XRCID("ID_PSK_DALE_CHALL"),
            _DT(L"PSK Dale-Chall"), _DT(L"Powers-Sumner-Kearl (Dale-Chall)"),
            _(L"PSK Dale-Chall is generally used for primary and secondary age readers "
              "to help classify school text books and literature."),
            readability::readability_test_type::grade_level, false,
            _DT(L"3.2672 + (.0596*(W/S)) + (.1155*((UDC/W)*100))"));
        // don't associate this test with anything, users should just be offered
        // New Dale-Chall from the project wizards instead
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_familiarity_dale_chall);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }
    }

//-------------------------------------------------------
void BaseProject::InitializeStandardEnglishGraphReadabilityTests()
    {
        // Fry
        {
        readability::readability_test test(
            ReadabilityMessages::FRY(), XRCID("ID_FRY"), _DT(L"Fry"), _DT(L"Fry"),
            _(L"The <a href=\"#FryGraph\">Fry graph</a> is designed for most text, "
              "including literature and technical documents."),
            readability::readability_test_type::grade_level, true, L"");
        test.add_document_classification(readability::document_classification::general_document,
                                         true);
        test.add_document_classification(
            readability::document_classification::adult_literature_document, true);
        test.add_document_classification(
            readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::adult_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_healthcare_industry, true);
        test.add_industry_classification(
            readability::industry_classification::adult_healthcare_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // Raygor
        {
        readability::readability_test test(
            ReadabilityMessages::RAYGOR(), XRCID("ID_RAYGOR"), _DT(L"Raygor Estimate"),
            _DT(L"Raygor Estimate"),
            _(L"The <a href=\"#RaygorGraph\">Raygor estimate graph</a> is designed for most text, "
              "including literature and technical documents."),
            readability::readability_test_type::grade_level, true, L"");
        test.add_document_classification(readability::document_classification::general_document,
                                         true);
        test.add_document_classification(
            readability::document_classification::adult_literature_document, true);
        test.add_document_classification(
            readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::adult_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_healthcare_industry, true);
        test.add_industry_classification(
            readability::industry_classification::adult_healthcare_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_length_6_plus);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }
    }

//-------------------------------------------------------
void BaseProject::InitializeStandardEnglishRegressionReadabilityTests()
    {
        // degrees of reading power
        {
        readability::readability_test test(
            ReadabilityMessages::DEGREES_OF_READING_POWER(), XRCID("ID_DEGREES_OF_READING_POWER"),
            _DT(L"Degrees of Reading Power"), _DT(L"Degrees of Reading Power"),
            // TRANSLATORS: "Degrees of Reading Power" should not be translated.
            _(L"Degrees of Reading Power is designed for matching documents to a student's "
              "reading ability (based on his/her DRP score). This test is influenced by "
              "sentence length, word length, and number of familiar Dale-Chall words."),
            readability::readability_test_type::index_value, true,
            _DT(L"ROUND(100 - 100*(.886593 - .083640*(R/W) + .161911*POWER((D/W),3) -\n"
                ".021401*(W/S) + .000577*POWER((W/S), 2) - .000005*POWER((W/S), 3)))"));
        test.add_industry_classification(
            readability::industry_classification::adult_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_familiarity_dale_chall);
        test.add_factor(readability::test_factor::word_length);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // degrees of reading power (grade equivalent)
        {
        readability::readability_test test(
            ReadabilityMessages::DEGREES_OF_READING_POWER_GE(),
            XRCID("ID_DEGREES_OF_READING_POWER_GE"), _DT(L"Degrees of Reading Power (GE)"),
            _DT(L"Degrees of Reading Power (grade equivalent)"),
            // TRANSLATORS: "Degrees of Reading Power" should not be translated.
            _(L"Degrees of Reading Power (GE) is designed for matching documents to a student's "
              "reading ability (based on his/her DRP score). This test is a conversion of a "
              "DRP (difficulty) score into a grade level."),
            readability::readability_test_type::grade_level, false, L"");
        test.add_industry_classification(
            readability::industry_classification::adult_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_familiarity_dale_chall);
        test.add_factor(readability::test_factor::word_length);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // Bormuth grade placement 35
        {
        readability::readability_test test(
            ReadabilityMessages::BORMUTH_GRADE_PLACEMENT_35(),
            XRCID("ID_BORMUTH_GRADE_PLACEMENT_35"), _DT(L"Bormuth Grade Placement"),
            _DT(L"Bormuth Grade Placement"),
            // TRANSLATORS: "Bormuth Grade Placement" should not be translated.
            _(L"Bormuth Grade Placement is designed for students and is "
              "highly regarded for its accuracy because "
              "it uses three variables (rather than the traditional two variables)."),
            readability::readability_test_type::grade_level, false,
            _DT(L"3.761864 + 1.053153*(R/W) - 2.138595*POWER((D/W),3) +\n"
                ".152832*(W/S) - .002077*POWER((W/S), 2)"));
        test.add_document_classification(
            readability::document_classification::adult_literature_document, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_familiarity_dale_chall);
        test.add_factor(readability::test_factor::word_length);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // Bormuth cloze mean
        {
        readability::readability_test test(
            ReadabilityMessages::BORMUTH_CLOZE_MEAN(), XRCID("ID_BORMUTH_CLOZE_MEAN"),
            _DT(L"Bormuth Cloze Mean"), _DT(L"Bormuth Cloze Mean"),
            // TRANSLATORS: "Bormuth Cloze Mean" should not be translated.
            _(L"Bormuth Cloze Mean is designed for students and is highly "
              "regarded for its accuracy because it uses "
              "three variables (rather than the traditional two variables)."),
            readability::readability_test_type::predicted_cloze_score, true,
            _DT(L"(.886593 - .083640*(R/W) + .161911*POWER((D/W),3) -\n"
                ".021401*(W/S) + .000577*POWER((W/S), 2) - .000005*POWER((W/S), 3))*100"));
        test.add_document_classification(
            readability::document_classification::adult_literature_document, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_familiarity_dale_chall);
        test.add_factor(readability::test_factor::word_length);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // wheeler-smith
        {
        readability::readability_test test(
            ReadabilityMessages::WHEELER_SMITH(), XRCID("ID_WHEELER_SMITH"), _DT(L"Wheeler-Smith"),
            _DT(L"Wheeler-Smith"),
            _(L"Wheeler-Smith is meant for primary-age (Kindergarten to 4th grade) "
              L"reading materials."),
            readability::readability_test_type::grade_level, false, L"");
        test.add_document_classification(
            readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_healthcare_industry, true);
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity_2_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // ari
        {
        readability::readability_test test(
            ReadabilityMessages::ARI(), XRCID("ID_ARI"), _DT(L"ARI"),
            _DT(L"Automated Readability Index"),
            // TRANSLATORS: "Automated Readability Index" should not be translated.
            _(L"Automated Readability Index, also known as \"ARI\" or \"auto\", "
              "was originally created for U.S. Air Force materials and was designed "
              "for technical documents and manuals."),
            readability::readability_test_type::grade_level, false,
            _DT(L"(4.71 * (RP/W)) + (0.5 * (W/S)) - 21.43"));
        test.add_document_classification(readability::document_classification::technical_document,
                                         true);
        // Air Force
        test.add_industry_classification(
            readability::industry_classification::military_government_industry, true);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_length);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // ELF
        {
        readability::readability_test test(
            ReadabilityMessages::ELF(), XRCID("ID_ELF"), _DT(L"ELF"),
            _DT(L"Easy Listening Formula"),
            _(L"ELF is designed for \"listenability\" and is meant for "
              "radio and television broadcasts."),
            readability::readability_test_type::grade_level, false, _DT(L"(B-W) / S"));
        test.add_industry_classification(
            readability::industry_classification::broadcasting_industry, true);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity_density);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // fog
        {
        readability::readability_test test(
            ReadabilityMessages::GUNNING_FOG(), XRCID("ID_FOG"), _DT(L"Gunning Fog"),
            _DT(L"Gunning Fog"),
            // TRANSLATORS: "Gunning Fog" should not be translated; it is a test name
            _(L"Gunning Fog is generally recommended for "
              "business publications and journals."),
            readability::readability_test_type::grade_level, false,
            _DT(L".4 * (WordCount()/SentenceCount(GunningFog) + "
                "((HardFogWordCount()/WordCount())*100))"));
        test.add_document_classification(readability::document_classification::technical_document,
                                         true);
        test.add_document_classification(readability::document_classification::general_document,
                                         true);
        test.add_document_classification(
            readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::adult_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_healthcare_industry, true);
        test.add_industry_classification(
            readability::industry_classification::adult_healthcare_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity_3_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // psk fog
        {
        readability::readability_test test(
            ReadabilityMessages::PSK_GUNNING_FOG(), XRCID("ID_PSK_FOG"), _DT(L"PSK Gunning Fog"),
            _DT(L"Powers-Sumner-Kearl (Gunning Fog)"),
            // TRANSLATORS: "PSK Gunning Fog" should not be translated.
            _(L"PSK Gunning Fog is generally recommended for "
              "business publications and journals."),
            readability::readability_test_type::grade_level, false,
            _DT(L"3.0680 + (.0877*(WordCount(Default)/SentenceCount(GunningFog))) +\n"
                "(.0984*((HardFogWordCount()/WordCount(Default))*100))"));
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity_3_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // lix
        {
        readability::readability_test test(
            ReadabilityMessages::LIX(), XRCID("ID_LIX"), _DT(L"Lix"),
            _DT(L"L\U000000E4sbarhetsindex (Lix)"),
            _(L"The L\U000000E4sbarhetsindex (Lix) formula can be used on "
              "documents of any Western European language. "
              "A <a href=\"#lix-gauge\">chart</a> is also available to visualize the score."),
            readability::readability_test_type::index_value_and_grade_level, true, L"");
        test.add_document_classification(
            readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::adult_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_language(readability::test_language::french_test);
        test.add_language(readability::test_language::spanish_test);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_length_7_plus);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // rix
        {
        readability::readability_test test(
            ReadabilityMessages::RIX(), XRCID("ID_RIX"), _DT(L"Rix"), _DT(L"Rate Index (Rix)"),
            // TRANSLATORS: "Rate Index" should not be translated.
            _(L"The Rate Index (Rix) formula is a variation of Lix and can be used on "
              "documents of any Western European language."),
            readability::readability_test_type::index_value_and_grade_level, false, L"");
        test.add_document_classification(
            readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::adult_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_language(readability::test_language::french_test);
        test.add_language(readability::test_language::spanish_test);
        test.add_language(readability::test_language::german_test);
        test.add_factor(readability::test_factor::word_length_7_plus);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // Coleman-Liau
        {
        readability::readability_test test(
            ReadabilityMessages::COLEMAN_LIAU(), XRCID("ID_COLEMAN_LIAU"), _DT(L"Coleman-Liau"),
            _DT(L"Coleman-Liau"),
            _(L"Coleman-Liau is meant for secondary age (4th grade to college level) readers. "
              "This formula is based on text from the .4 to 16.3 grade level range. "
              "This test usually yields the lowest grade when applied to technical documents."),
            readability::readability_test_type::grade_level_and_predicted_cloze_score, false,
            // just returns the grade level, not the predicted Cloze score
            _DT(L"(-27.4004*(((141.8401 - (.214590*R*(100/W))) + "
                "(1.079812*S*(100/W)))/100)) + 23.06395"));
        test.add_document_classification(
            readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::adult_publishing_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_length);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // Danielson-Bryan 1
        {
        readability::readability_test test(
            ReadabilityMessages::DANIELSON_BRYAN_1(), XRCID("ID_DANIELSON_BRYAN_1"),
            _DT(L"Danielson-Bryan 1"), _DT(L"Danielson-Bryan 1"),
            _(L"Danielson-Bryan 1 is designed for student materials."),
            readability::readability_test_type::grade_level, false,
            _DT(L"1.0364*(RP/W) + .0194*(RP/S) - .6059"));
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_length);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // Danielson-Bryan 2
        {
        readability::readability_test test(
            ReadabilityMessages::DANIELSON_BRYAN_2(), XRCID("ID_DANIELSON_BRYAN_2"),
            _DT(L"Danielson-Bryan 2"), _DT(L"Danielson-Bryan 2"),
            _(L"Danielson-Bryan 2 is designed for student materials. It is a variation of "
              "Flesch Reading Ease that uses character counts instead of syllable counts. "
              "A <a href=\"#DB2\">chart</a> is also available to visualize the score."),
            readability::readability_test_type::index_value, true,
            _DT(L"ROUND(131.059 - 10.364*(RP/W) - .194*(RP/S))"));
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_length);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // FORCAST
        {
        readability::readability_test test(
            ReadabilityMessages::FORCAST(), XRCID("ID_FORCAST"), _DT(L"FORCAST"), _DT(L"FORCAST"),
            _(L"FORCAST was devised for assessing U.S. Army technical manuals and forms. "
              "It is the only test not designed for running narrative, so it is mostly "
              "appropriate for multiple-choice quizzes, applications, entrance forms, etc."),
            readability::readability_test_type::grade_level, false, _DT(L"20-(M/10)"));
        test.add_document_classification(
            readability::document_classification::nonnarrative_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_healthcare_industry, true);
        test.add_industry_classification(
            readability::industry_classification::adult_healthcare_industry, true);
        // Army
        test.add_industry_classification(
            readability::industry_classification::military_government_industry, true);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity_2_plus_syllables);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // Farr-Jenkins-Paterson
        {
        readability::readability_test test(
            ReadabilityMessages::FARR_JENKINS_PATERSON(), XRCID("ID_FARR_JENKINS_PATERSON"),
            _DT(L"Farr-Jenkins-Paterson"), _DT(L"Farr-Jenkins-Paterson"),
            // TRANSLATORS: "Flesch Reading Ease" should not be translated.
            _(L"Farr-Jenkins-Paterson is a variation of the Flesch Reading Ease test, "
              "which uses the mono-syllabic word count instead of the overall syllable count. "
              "Scores range from 0-100 (the higher the score, the easier to read). "
              "Average documents should be within the range of 60-70."),
            readability::readability_test_type::index_value, true,
            _DT(L"ROUND(-31.517 - (1.015*(W/S)) + (1.599*((M/W)*100)))"));
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity_2_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // New Farr-Jenkins-Paterson (Kincaid)
        {
        readability::readability_test test(
            ReadabilityMessages::NEW_FARR_JENKINS_PATERSON(), XRCID("ID_NEW_FARR_JENKINS_PATERSON"),
            _DT(L"New Farr-Jenkins-Paterson"), _DT(L"New Farr-Jenkins-Paterson (Kincaid)"),
            _(L"A modified version of Farr-Jenkins-Paterson designed for "
              "U.S. Navy technical manuals and forms."),
            readability::readability_test_type::grade_level, true,
            _DT(L"22.05 + (.387*(W/S)) - (.307*((M/W)*100))"));
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity_2_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // Farr-Jenkins-Paterson (PSK)
        {
        readability::readability_test test(
            ReadabilityMessages::PSK_FARR_JENKINS_PATERSON(), XRCID("ID_PSK_FARR_JENKINS_PATERSON"),
            _DT(L"PSK Farr-Jenkins-Paterson"), _DT(L"Powers-Sumner-Kearl (Farr-Jenkins-Paterson)"),
            _(L"PSK Farr-Jenkins-Paterson is a variation of the Farr-Jenkins-Paterson test, "
              "which returns a grade score instead of a Flesch difficulty level."),
            readability::readability_test_type::grade_level, false,
            _DT(L"8.4335 + (.0923*(W/S)) - (.0648*((M/W)*100))"));
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity_2_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // Flesch Reading Ease
        {
        readability::readability_test test(
            ReadabilityMessages::FLESCH(), XRCID("ID_FLESCH"), _DT(L"Flesch Reading Ease"),
            _DT(L"Flesch Reading Ease"),
            // TRANSLATORS: "Flesch Reading Ease" should not be translated.
            _(L"Flesch Reading Ease is meant for secondary and adult-level text and is a standard "
              "used by many U.S. government agencies, including the U.S. Department of Defense. "
              "Scores range from 0-100 (the higher the score, the easier to read). "
              "Average documents should be within the range of 60-70. "
              "A <a href=\"#flesch-chart\">chart</a> is also available to visualize the score."),
            readability::readability_test_type::index_value, true,
            _DT(L"ROUND(206.835 - (84.6*(B/W)) - (1.015*(W/S)))"));
        test.add_document_classification(readability::document_classification::general_document,
                                         true);
        test.add_document_classification(readability::document_classification::technical_document,
                                         true);
        test.add_document_classification(
            readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::adult_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_healthcare_industry, true);
        test.add_industry_classification(
            readability::industry_classification::adult_healthcare_industry, true);
        // used by U.S. DoD
        test.add_industry_classification(
            readability::industry_classification::military_government_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // Flesch-Kincaid
        {
        readability::readability_test test(
            ReadabilityMessages::FLESCH_KINCAID(), XRCID("ID_FLESCH_KINCAID"),
            _DT(L"Flesch-Kincaid"), _DT(L"Flesch-Kincaid"),
            _(L"Flesch-Kincaid is designed for technical documents and is mostly applicable "
              "to manuals and forms, rather than schoolbook text or literary works.\n\n"
              "This test is part of the Kincaid Navy Personnel collection of tests."),
            readability::readability_test_type::grade_level, false,
            _DT(L"(11.8*(B/W)) + (.39*(W/S)) - 15.59"));
        test.add_document_classification(readability::document_classification::general_document,
                                         true);
        test.add_document_classification(readability::document_classification::technical_document,
                                         true);
        // Navy
        test.add_industry_classification(
            readability::industry_classification::military_government_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // Flesch-Kincaid (simplified)
        {
        readability::readability_test test(
            ReadabilityMessages::FLESCH_KINCAID_SIMPLIFIED(), XRCID("ID_FLESCH_KINCAID_SIMPLIFIED"),
            _DT(L"Flesch-Kincaid (simplified)"), _DT(L"Flesch-Kincaid (simplified)"),
            _(L"Flesch-Kincaid is designed for technical documents and is mostly applicable "
              "to manuals and forms, rather than schoolbook text or literary works.\n\n"
              "This test is part of the Kincaid Navy Personnel collection of tests."),
            readability::readability_test_type::grade_level, false,
            _DT(L"(12*(B/W)) + (.4*(W/S)) - 16"));
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // new fog count
        {
        readability::readability_test test(
            ReadabilityMessages::NEW_FOG(), XRCID("ID_NEW_FOG_COUNT"), _DT(L"New Fog Count"),
            _DT(L"New Fog Count (Kincaid)"),
            // TRANSLATORS: "New Fog Count" should not be translated; it is a test name.
            _(L"New Fog Count is a modified version of Gunning Fog created for the "
              "U.S. Navy and was designed for technical documents and manuals.\n\n"
              "This test is part of the Kincaid Navy Personnel collection of tests."),
            readability::readability_test_type::grade_level, false,
            _DT(L"(((((WordCount(Default)-HardFogWordCount())) + "
                "(3 * HardFogWordCount())) / SentenceCount(GunningFog)) - 3) / 2"));
        // Navy
        test.add_industry_classification(
            readability::industry_classification::military_government_industry, true);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity_3_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // EFLAW
        {
        readability::readability_test test(
            ReadabilityMessages::EFLAW(), XRCID("ID_EFLAW"), _DT(L"EFLAW"), _DT(L"McAlpine EFLAW"),
            _(L"The McAlpine EFLAW formula is used to determine the ease of reading "
              "English text for ESL/EFL (English as a Second/Foreign Language) readers. "
              "This test is influenced by sentence length and words consisting of "
              "three or less characters. "
              "To lower a score, try using shorter sentences and longer, less colloquial words."),
            readability::readability_test_type::index_value, true, _DT(L"ROUND((W+T)/S)"));
        test.add_industry_classification(
            readability::industry_classification::secondary_language_industry, true);
        test.add_teaching_level(readability::test_teaching_level::second_language);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_length_3_less);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // SMOG
        {
        readability::readability_test test(
            ReadabilityMessages::SMOG(), XRCID("ID_SMOG"), _DT(L"SMOG"), _DT(L"SMOG"),
            _(L"SMOG is generally "
              "appropriate for secondary age (4th grade to college level) readers. "
              "SMOG tests for 100% comprehension, whereas most formulas test for "
              "around 50%-75% comprehension."),
            readability::readability_test_type::grade_level, false,
            _DT(L"1.0430*SQRT(C*(30/S)) + 3.1291"));
        test.add_document_classification(readability::document_classification::general_document,
                                         true);
        test.add_document_classification(
            readability::document_classification::adult_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::adult_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_healthcare_industry, true);
        test.add_industry_classification(
            readability::industry_classification::adult_healthcare_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity_3_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // SMOG (simplified)
        {
        readability::readability_test test(
            ReadabilityMessages::SMOG_SIMPLIFIED(), XRCID("ID_SMOG_SIMPLIFIED"),
            _(L"SMOG (simplified)"), _(L"SMOG (simplified)"),
            _(L"SMOG is generally appropriate for secondary age (4th grade to college level) "
              "readers. SMOG tests for 100% comprehension, whereas most formulas test for "
              "around 50%-75% comprehension."),
            readability::readability_test_type::grade_level, false,
            _DT(L"FLOOR(SQRT(C*(30/S))) + 3"));
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity_3_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // SMOG (modified for primary readers)
        {
        readability::readability_test test(
            ReadabilityMessages::MODIFIED_SMOG(), XRCID("ID_SMOG_MODIFIED"), _DT(L"Modified SMOG"),
            _DT(L"Modified SMOG"),
            // TRANSLATORS: "Modified SMOG" should not be translated; it is a test name.
            _(L"Modified SMOG is a variation of SMOG that is adjusted for primary-age materials."),
            readability::readability_test_type::grade_level, false, L"");
        test.add_teaching_level(readability::test_teaching_level::primary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity_3_plus_syllables);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // New ARI (simplified)
        {
        readability::readability_test test(
            ReadabilityMessages::SIMPLE_ARI(), XRCID("ID_SIMPLIFIED_ARI"), _DT(L"SARI"),
            // TRANSLATORS: "New Automated Readability Index" should not be translated;
            // it is a test name.
            _DT(L"New Automated Readability Index (Kincaid, simplified)"),
            _(L"New Automated Readability Index is a modified version of ARI created for "
              "U.S. Navy materials and was designed for technical documents and manuals.\n\n"
              "This test is part of the Kincaid Navy Personnel collection of tests."),
            readability::readability_test_type::grade_level, false,
            _DT(L"(6*(RP/W)) + (.4*(W/S)) - 27.4"));
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_length);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // New ARI
        {
        readability::readability_test test(
            ReadabilityMessages::NEW_ARI(), XRCID("ID_NEW_ARI"), _DT(L"New ARI"),
            _DT(L"New Automated Readability Index (Kincaid)"),
            // TRANSLATORS: "New Automated Readability Index" should not be translated;
            // it is a test name.
            _(L"New Automated Readability Index is a modified version of ARI created for "
              "U.S. Navy materials and was designed for technical documents and manuals.\n\n"
              "This test is part of the Kincaid Navy Personnel collection of tests."),
            readability::readability_test_type::grade_level, false,
            _DT(L"(5.84*(RP/W)) + (.37*(W/S)) - 26.01"));
        // Navy
        test.add_industry_classification(
            readability::industry_classification::military_government_industry, true);
        test.add_teaching_level(readability::test_teaching_level::adult_level);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_length);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }

        // PSK Flesch
        {
        readability::readability_test test(
            ReadabilityMessages::PSK_FLESCH(), XRCID("ID_PSK_FLESCH"), _DT(L"PSK Flesch"),
            _DT(L"Powers-Sumner-Kearl (Flesch)"), _(L"PSK Flesch is used for student readers."),
            readability::readability_test_type::grade_level, false,
            _DT(L"((W/S) * .0778) + ((B*(100/W)) * .0455) - 2.2029"));
        test.add_document_classification(
            readability::document_classification::childrens_literature_document, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry, true);
        test.add_industry_classification(
            readability::industry_classification::childrens_healthcare_industry, true);
        test.add_teaching_level(readability::test_teaching_level::secondary_grade);
        test.add_language(readability::test_language::english_test);
        test.add_factor(readability::test_factor::word_complexity);
        test.add_factor(readability::test_factor::sentence_length);
        m_defaultReadabilityTestsTemplate.add_test(test);
        m_testIdMap.insert(std::make_pair(test.get_id().c_str(), test.get_interface_id()));
        }
    }

//-------------------------------------------------------
void BaseProject::InitializeStandardReadabilityTests()
    {
    wxASSERT_MSG(m_defaultReadabilityTestsTemplate.get_test_count() == 0,
                 L"InitializeStandardReadabilityTests called twice?");
    m_defaultReadabilityTestsTemplate.clear();
    m_testIdMap.clear();

    InitializeStandardEnglishFamiliarWordReadabilityTests();
    InitializeStandardEnglishGraphReadabilityTests();
    InitializeStandardEnglishRegressionReadabilityTests();
    InitializeStandardSpanishReadabilityTests();
    InitializeStandardGermanReadabilityTests();
    }

//-------------------------------------------------------
void BaseProject::AddCustomReadabilityTests()
    {
    // run through the tests
    for (const auto& pos : m_customTestsInUse)
        {
        AddCustomReadabilityTest(pos.GetTestName(), true);
        }
    }

//-------------------------------------------------------
void BaseProject::LoadHardWords()
    {
    PROFILE();
    // complex words (3+ syllable)
    if (HasUI())
        {
        if (Get3SyllablePlusData() == nullptr)
            {
            m_3SybPlusData = std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>();
            }
        Get3SyllablePlusData()->DeleteAllItems();
        Get3SyllablePlusData()->SetSize(m_word_frequency_map->get_data().size(), 4);
        }

    m_total3plusSyllableWords = 0;
    m_unique3PlusSyllableWords = 0;

    // long words (6+ characters)
    if (HasUI())
        {
        if (Get6CharacterPlusData() == nullptr)
            {
            m_6CharPlusData = std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>();
            }
        Get6CharacterPlusData()->DeleteAllItems();
        Get6CharacterPlusData()->SetSize(m_word_frequency_map->get_data().size(), 4);
        }

    m_totalLongWords = 0;
    m_totalSixPlusCharacterWordsIgnoringNumerals = 0;
    m_unique6CharsPlusWords = 0;

    // hard words (DC)
    if (HasUI())
        {
        if (GetDaleChallHardWordData() == nullptr)
            {
            m_DCHardWordsData = std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>();
            }
        GetDaleChallHardWordData()->DeleteAllItems();
        GetDaleChallHardWordData()->SetSize(m_word_frequency_map->get_data().size(), 3);
        }

    const readability::is_familiar_word<word_case_insensitive_no_stem, const word_list,
                                        stemming::no_op_stem<word_case_insensitive_no_stem>>
        isDCWord(IsIncludingStockerCatholicSupplement() ?
                     &m_dale_chall_plus_stocker_catholic_word_list :
                     &m_dale_chall_word_list,
                 // because we are reviewing a frequency set, turn off proper noun support
                 // and handle it ourselves
                 readability::proper_noun_counting_method::all_proper_nouns_are_unfamiliar, true);
    m_totalHardWordsDaleChall = 0;
    m_uniqueDCHardWords = 0;

    m_total3PlusSyllabicWordsNumeralsFullySyllabized = 0;
    m_totalHardWordsSol = 0;
    m_unique3PlusSyllabicWordsNumeralsFullySyllabized = 0;
    m_totalHardWordsFog = 0;
    m_uniqueHardFogWords = 0;

    // hard words (Spache)
    if (HasUI())
        {
        if (GetSpacheHardWordData() == nullptr)
            {
            m_SpacheHardWordsData = std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>();
            }
        GetSpacheHardWordData()->DeleteAllItems();
        GetSpacheHardWordData()->SetSize(m_word_frequency_map->get_data().size(), 3);
        }

    const readability::is_familiar_word<word_case_insensitive_no_stem, const word_list,
                                        stemming::no_op_stem<word_case_insensitive_no_stem>>
        isSpacheWord(&m_spache_word_list,
                     readability::proper_noun_counting_method::all_proper_nouns_are_unfamiliar,
                     true);
    m_totalHardWordsSpache = 0;
    m_uniqueSpacheHardWords = 0;

    m_uniqueWords = 0;

    // mini words (EFLAW)
    m_uniqueMiniWords = 0;
    m_totalMiniWords = 0;

    // Monosyllabic words
    m_uniqueMonoSyllabicWords = 0;

    // hard words (Harris-Jacobson)
    const readability::is_familiar_word<word_case_insensitive_no_stem, const word_list,
                                        stemming::no_op_stem<word_case_insensitive_no_stem>>
        isHarrisJacobsonWord(
            &m_harris_jacobson_word_list,
            readability::proper_noun_counting_method::all_proper_nouns_are_unfamiliar, true);
    m_uniqueHarrisJacobsonHardWords = m_totalHardWordsHarrisJacobson = 0;
    if (HasUI())
        {
        if (GetHarrisJacobsonHardWordDataData() == nullptr)
            {
            m_harrisJacobsonHardWordsData =
                std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>();
            }
        GetHarrisJacobsonHardWordDataData()->DeleteAllItems();
        GetHarrisJacobsonHardWordDataData()->SetSize(m_word_frequency_map->get_data().size(), 3);
        }

    // dolch words
    if (HasUI())
        {
        if (GetDolchWordData() == nullptr)
            {
            m_dolchWordsData = std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>();
            }
        GetDolchWordData()->DeleteAllItems();
        GetDolchWordData()->SetSize(m_word_frequency_map->get_data().size(), 4);
        }
    size_t uniqueDolchWords = 0;

    if (HasUI())
        {
        if (GetNonDolchWordData() == nullptr)
            {
            m_nonDolchWordsData = std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>();
            }
        GetNonDolchWordData()->DeleteAllItems();
        GetNonDolchWordData()->SetSize(m_word_frequency_map->get_data().size(), 2);
        }
    size_t nonUniqueDolchWords = 0;

    m_dolchConjunctionCounts = m_dolchPrepositionCounts = m_dolchPronounCounts =
        m_dolchAdverbCounts = m_dolchAdjectiveCounts = m_dolchVerbCounts = m_dolchNounCounts =
            std::pair<size_t, size_t>(0, 0);

    // reset the custom tests
    for (auto& customTest : m_customTestsInUse)
        {
        customTest.Reset();
        if (HasUI() && customTest.GetIterator()->is_using_familiar_words())
            {
            customTest.GetListViewData()->SetSize(m_word_frequency_map->get_data().size(), 2);
            }
        }

    // calculate the Dolch level completions
    std::set<readability::sight_word> unusedDolchWords;
    for (const auto& dolchPos : m_dolch_word_list.get_words())
        {
        if (!m_word_frequency_map->get_data().contains(word_case_insensitive_no_stem(
                dolchPos.get_word().c_str(), dolchPos.get_word().length(),
                /* filler arguments not used*/ 0, 0, 0, false, false, false, false, 0, 0)))
            {
            unusedDolchWords.insert(dolchPos);
            }
        }

    m_unusedDolchConjunctions = m_unusedDolchPrepositions = m_unusedDolchPronouns =
        m_unusedDolchAdverbs = m_unusedDolchAdjectives = m_unusedDolchVerbs =
            m_unusedDolchNounsWords = 0;

    if (HasUI())
        {
        if (GetUnusedDolchWordData() == nullptr)
            {
            m_unusedDolchWordsData = std::make_shared<Wisteria::UI::ListCtrlExDataProvider>();
            }
        GetUnusedDolchWordData()->DeleteAllItems();
        GetUnusedDolchWordData()->SetSize(unusedDolchWords.size(), 2);
        }
    size_t wordCounter = 0;
    for (auto pos = unusedDolchWords.cbegin(); pos != unusedDolchWords.cend(); ++pos, ++wordCounter)
        {
        wxString classificationLabel;
        switch (pos->get_type())
            {
        case readability::sight_word_type::conjunction:
            classificationLabel = _(L"Conjunction");
            ++m_unusedDolchConjunctions;
            break;
        case readability::sight_word_type::preposition:
            classificationLabel = _(L"Preposition");
            ++m_unusedDolchPrepositions;
            break;
        case readability::sight_word_type::pronoun:
            classificationLabel = _(L"Pronoun");
            ++m_unusedDolchPronouns;
            break;
        case readability::sight_word_type::adverb:
            classificationLabel = _(L"Adverb");
            ++m_unusedDolchAdverbs;
            break;
        case readability::sight_word_type::adjective:
            classificationLabel = _(L"Adjective");
            ++m_unusedDolchAdjectives;
            break;
        case readability::sight_word_type::verb:
            classificationLabel = _(L"Verb");
            ++m_unusedDolchVerbs;
            break;
        case readability::sight_word_type::noun:
            classificationLabel = _(L"Noun");
            ++m_unusedDolchNounsWords;
            break;
            };
        if (HasUI())
            {
            GetUnusedDolchWordData()->SetItemText(
                wordCounter, 0, pos->get_word().c_str(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            GetUnusedDolchWordData()->SetItemText(
                wordCounter, 1, classificationLabel,
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        }

    if (HasUI())
        {
        if (GetProperNounsData() == nullptr)
            {
            m_ProperNounsData = std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>();
            }
        GetProperNounsData()->DeleteAllItems();
        GetProperNounsData()->SetSize(GetWordsWithFrequencies()->get_data().size(), 3);

        if (GetContractionsData() == nullptr)
            {
            m_contractionsData = std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>();
            }
        GetContractionsData()->DeleteAllItems();
        GetContractionsData()->SetSize(GetWordsWithFrequencies()->get_data().size(), 2);

        if (GetAllWordsBaseData() == nullptr)
            {
            m_AllWordsBaseData = std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>();
            }
        GetAllWordsBaseData()->DeleteAllItems();
        GetAllWordsBaseData()->SetSize(GetWordsWithFrequencies()->get_data().size(), 4);
        }

    m_uniqueWords = m_word_frequency_map->get_data().size();

    // Load proper nouns and all words lists
    multi_value_frequency_aggregate_map<traits::case_insensitive_wstring_ex,
                                        traits::case_insensitive_wstring_ex>
        keyWordsStemmedWithCounts;

    auto stemmer = CreateStemmer();

    size_t uniqueProperNouns{ 0 }, uniqueContractions{ 0 };
    size_t i = 0;
    for (auto wordPos = GetWordsWithFrequencies()->get_data().begin();
         wordPos != GetWordsWithFrequencies()->get_data().end(); ++wordPos, ++i)
        {
        // the values are frequency count (first) and the number
        // of those that are proper (second)
        assert(wordPos->second.first >= wordPos->second.second);
        /* subtract number of times word is proper from total count of word
           to see if at least on instance is NOT proper. If they are equal, then
           all instances are proper and therefore cannot be an unfamiliar word.*/
        const bool allInstancesAreProper = (wordPos->second.first == wordPos->second.second);
        const size_t nonProperCount = (wordPos->second.first - wordPos->second.second);
        if (wordPos->first.get_syllable_count() == 1 ||
            (wordPos->first.is_numeric() &&
             GetNumeralSyllabicationMethod() == NumeralSyllabize::WholeWordIsOneSyllable))
            {
            ++m_uniqueMonoSyllabicWords;
            }
        // proper
        if (HasUI() && wordPos->second.second > 0)
            {
            // Make sure first letter is capitalized (just looks nicer).
            // Can't use MakeCapitalized() because it messes up Roman numerals.
            wxString theWord(wordPos->first.c_str());
            if (!theWord.empty())
                {
                theWord[0] = std::towupper(theWord[0]);
                }
            GetProperNounsData()->SetItemText(
                uniqueProperNouns, 0, theWord,
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            // if proper occurrences are less than the total occurrences, then there must be
            // non-proper instances
            if (wordPos->second.first > wordPos->second.second)
                {
                GetProperNounsData()->SetItemText(
                    uniqueProperNouns, 1,
                    wxString::Format(_(L"%zu (%zu additional occurrences not proper)"),
                                     wordPos->second.second,
                                     (wordPos->second.first - wordPos->second.second)),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    wordPos->second.second);
                }
            else
                {
                GetProperNounsData()->SetItemValue(uniqueProperNouns, 1, wordPos->second.second);
                }
            GetProperNounsData()->SetItemText(
                uniqueProperNouns, 2, wordPos->first.is_personal() ? _(L"Yes") : _(L"No"),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            ++uniqueProperNouns;
            }
        if (HasUI() && wordPos->first.is_contraction())
            {
            GetContractionsData()->SetItemText(
                uniqueContractions, 0, wordPos->first.c_str(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            GetContractionsData()->SetItemValue(uniqueContractions, 1, wordPos->second.first);
            ++uniqueContractions;
            }
        // all words
        if (HasUI())
            {
            GetAllWordsBaseData()->SetItemText(
                i, 0, wordPos->first.c_str(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            GetAllWordsBaseData()->SetItemValue(i, 1, wordPos->second.first);
            /* if a numeric string then check to see if we are supposed to
               treat numerals as monosyllabic*/
            if (wordPos->first.is_numeric() &&
                GetNumeralSyllabicationMethod() == NumeralSyllabize::WholeWordIsOneSyllable)
                {
                GetAllWordsBaseData()->SetItemValue(i, 2, 1);
                }
            else
                {
                GetAllWordsBaseData()->SetItemValue(i, 2, wordPos->first.get_syllable_count());
                }
            GetAllWordsBaseData()->SetItemValue(i, 3,
                                                wordPos->first.get_length_excluding_punctuation());
            }
        // important (stemmed) words with a list of their variations occurring in the document
        if (HasUI() &&
            (GetWordsBreakdownInfo().IsWordCloudEnabled() ||
             GetWordsBreakdownInfo().IsKeyWordsEnabled()) &&
            !wordPos->first.is_file_address() && !wordPos->first.is_numeric() &&
            !GetWords()->is_word_common(wordPos->first.c_str()))
            {
            auto knownStems = GetWords()->get_cached_stems().find(stemmer->get_language());
            if (knownStems != GetWords()->get_cached_stems().cend())
                {
                // avoid re-stemming by looking for cached stems from the same stemmer
                if (const auto foundStem = knownStems->second.find(
                        std::wstring_view{ wordPos->first.c_str(), wordPos->first.length() });
                    foundStem != knownStems->second.cend())
                    {
                    keyWordsStemmedWithCounts.insert(
                        // the stem and original word
                        traits::case_insensitive_wstring_ex{ foundStem->second.c_str(),
                                                             foundStem->second.length() },
                        wordPos->first,
                        // overall frequency of current word
                        wordPos->second.first);
                    }
                else
                    {
                    traits::case_insensitive_wstring_ex stemmedWord(wordPos->first.c_str(),
                                                                    wordPos->first.length());
                    (*stemmer)(stemmedWord);
                    // wasn't stemmed and indexed before, so add it now
                    knownStems->second.emplace(
                        std::wstring{ wordPos->first.c_str(), wordPos->first.length() },
                        std::wstring{ stemmedWord.c_str(), stemmedWord.length() });
                    keyWordsStemmedWithCounts.insert(
                        // the stem and original word
                        std::move(stemmedWord), wordPos->first,
                        // overall frequency of current word
                        wordPos->second.first);
                    }
                }
            // shouldn't happen, this means the stemmer wasn't connected to the document
            // (just a sanity check fallback)
            else
                {
                traits::case_insensitive_wstring_ex stemmedWord(wordPos->first.c_str(),
                                                                wordPos->first.length());
                (*stemmer)(stemmedWord);
                keyWordsStemmedWithCounts.insert(
                    // the stem and original word
                    std::move(stemmedWord), wordPos->first,
                    // overall frequency of current word
                    wordPos->second.first);
                }
            }
        // dolch sight words
        auto dolchIter = m_dolch_word_list(wordPos->first.c_str());
        if (dolchIter != m_dolch_word_list.get_words().end())
            {
            wxString classificationLabel;
            switch (dolchIter->get_type())
                {
            case readability::sight_word_type::conjunction:
                classificationLabel = _(L"Conjunction");
                ++m_dolchConjunctionCounts.first;
                m_dolchConjunctionCounts.second += wordPos->second.first;
                break;
            case readability::sight_word_type::preposition:
                classificationLabel = _(L"Preposition");
                ++m_dolchPrepositionCounts.first;
                m_dolchPrepositionCounts.second += wordPos->second.first;
                break;
            case readability::sight_word_type::pronoun:
                classificationLabel = _(L"Pronoun");
                ++m_dolchPronounCounts.first;
                m_dolchPronounCounts.second += wordPos->second.first;
                break;
            case readability::sight_word_type::adverb:
                classificationLabel = _(L"Adverb");
                ++m_dolchAdverbCounts.first;
                m_dolchAdverbCounts.second += wordPos->second.first;
                break;
            case readability::sight_word_type::adjective:
                classificationLabel = _(L"Adjective");
                ++m_dolchAdjectiveCounts.first;
                m_dolchAdjectiveCounts.second += wordPos->second.first;
                break;
            case readability::sight_word_type::verb:
                classificationLabel = _(L"Verb");
                ++m_dolchVerbCounts.first;
                m_dolchVerbCounts.second += wordPos->second.first;
                break;
            case readability::sight_word_type::noun:
                classificationLabel = _(L"Noun");
                ++m_dolchNounCounts.first;
                m_dolchNounCounts.second += wordPos->second.first;
                break;
                };
            if (HasUI())
                {
                GetDolchWordData()->SetItemText(
                    uniqueDolchWords, 0, wordPos->first.c_str(),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                GetDolchWordData()->SetItemValue(uniqueDolchWords, 1, wordPos->second.first);
                GetDolchWordData()->SetItemText(
                    uniqueDolchWords, 2, classificationLabel,
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            ++uniqueDolchWords;
            }
        else
            {
            if (HasUI())
                {
                GetNonDolchWordData()->SetItemText(
                    nonUniqueDolchWords, 0, wordPos->first.c_str(),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                GetNonDolchWordData()->SetItemValue(nonUniqueDolchWords, 1, wordPos->second.first);
                }
            ++nonUniqueDolchWords;
            }
        // 3+ syllable words
        if (wordPos->first.get_syllable_count() >= 3)
            {
            /* if not a numeric string then it is OK. If it is numeric then check
               to see if we are supposed to treat numerals as monosyllabic*/
            if (!wordPos->first.is_numeric() ||
                GetNumeralSyllabicationMethod() != NumeralSyllabize::WholeWordIsOneSyllable)
                {
                // only load the data for standard projects
                if (HasUI())
                    {
                    Get3SyllablePlusData()->SetItemText(
                        GetTotalUnique3PlusSyllableWords(), 0, wordPos->first.c_str(),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    Get3SyllablePlusData()->SetItemValue(GetTotalUnique3PlusSyllableWords(), 1,
                                                         wordPos->first.get_syllable_count());
                    Get3SyllablePlusData()->SetItemValue(GetTotalUnique3PlusSyllableWords(), 2,
                                                         wordPos->second.first);
                    const std::pair<bool, word_list_with_replacements::word_type> replacement =
                        difficult_word_replacement_list.find(wordPos->first.c_str());
                    if (replacement.first)
                        {
                        Get3SyllablePlusData()->SetItemText(
                            GetTotalUnique3PlusSyllableWords(), 3, replacement.second.c_str(),
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        }
                    }
                ++m_unique3PlusSyllableWords;
                m_total3plusSyllableWords += wordPos->second.first;
                }
            }
        // Fog
        if (wordPos->first.get_syllable_count() >= 3 &&
            // if all instances are proper then it can't be a hard Fog word
            !allInstancesAreProper)
            {
            /* if not a numeric string then it is OK. If it is numeric then check
               to see if we are supposed to treat numerals as monosyllabic*/
            if (wordPos->first.is_numeric() &&
                GetNumeralSyllabicationMethod() == NumeralSyllabize::WholeWordIsOneSyllable)
                { /* NOOP*/
                }
            else if (!readability::is_easy_gunning_fog_word(
                         { wordPos->first.c_str(), wordPos->first.length() },
                         wordPos->first.get_syllable_count()))
                {
                ++m_uniqueHardFogWords;
                m_totalHardWordsFog += nonProperCount;
                }
            }
        // SMOG
        if (wordPos->first.get_syllable_count() >= 3)
            {
            ++m_unique3PlusSyllabicWordsNumeralsFullySyllabized;
            m_total3PlusSyllabicWordsNumeralsFullySyllabized += wordPos->second.first;
            }
        if (wordPos->first.get_length_excluding_punctuation() >= 6)
            {
            // only load the data for standard projects
            if (HasUI())
                {
                Get6CharacterPlusData()->SetItemText(
                    GetTotalUnique6CharsPlusWords(), 0, wordPos->first.c_str(),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                Get6CharacterPlusData()->SetItemValue(
                    GetTotalUnique6CharsPlusWords(), 1,
                    wordPos->first.get_length_excluding_punctuation());
                Get6CharacterPlusData()->SetItemValue(GetTotalUnique6CharsPlusWords(), 2,
                                                      wordPos->second.first);
                auto replacement = difficult_word_replacement_list.find(wordPos->first.c_str());
                if (replacement.first)
                    {
                    Get6CharacterPlusData()->SetItemText(
                        GetTotalUnique6CharsPlusWords(), 3, replacement.second.c_str(),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    }
                }
            ++m_unique6CharsPlusWords;
            m_totalLongWords += wordPos->second.first;
            if (!wordPos->first.is_numeric())
                {
                m_totalSixPlusCharacterWordsIgnoringNumerals += wordPos->second.first;
                }
            }
        if (wordPos->first.get_length_excluding_punctuation() <= 3 && !wordPos->first.is_numeric())
            {
            ++m_uniqueMiniWords;
            m_totalMiniWords += wordPos->second.first;
            }
        if (!allInstancesAreProper &&
            (GetHarrisJacobsonTextExclusionMode() ==
             SpecializedTestTextExclusion::UseSystemDefault) &&
            !isHarrisJacobsonWord(wordPos->first))
            {
            // only load the data for standard projects
            if (HasUI())
                {
                GetHarrisJacobsonHardWordDataData()->SetItemText(
                    GetTotalUniqueHarrisJacobsonHardWords(), 0, wordPos->first.c_str(),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                if (wordPos->second.first == nonProperCount)
                    {
                    GetHarrisJacobsonHardWordDataData()->SetItemValue(
                        GetTotalUniqueHarrisJacobsonHardWords(), 1, nonProperCount);
                    }
                else
                    {
                    GetHarrisJacobsonHardWordDataData()->SetItemText(
                        GetTotalUniqueHarrisJacobsonHardWords(), 1,
                        wxString::Format(
                            // TRANSLATORS: %zu are word total placeholders
                            _(L"%zu (%zu total occurrences, %zu proper and familiar, "
                              "%zu non-proper and unfamiliar)"),
                            nonProperCount, wordPos->second.first,
                            (wordPos->second.first - nonProperCount), nonProperCount),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        nonProperCount);
                    }
                auto replacement = harris_jacobson_replacement_list.find(wordPos->first.c_str());
                if (replacement.first)
                    {
                    GetHarrisJacobsonHardWordDataData()->SetItemText(
                        GetTotalUniqueHarrisJacobsonHardWords(), 2, replacement.second.c_str(),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    }
                }
            ++m_uniqueHarrisJacobsonHardWords;
            m_totalHardWordsHarrisJacobson += nonProperCount;
            }
        if ((GetDaleChallTextExclusionMode() == SpecializedTestTextExclusion::UseSystemDefault) &&
            !isDCWord(wordPos->first))
            {
            // all forms of word are proper and proper words are familiar?
            // Then it can't be a hard word at all.
            if (allInstancesAreProper &&
                GetDaleChallProperNounCountingMethod() ==
                    readability::proper_noun_counting_method::all_proper_nouns_are_familiar)
                { /* no-op*/
                ;
                }
            else if (GetDaleChallProperNounCountingMethod() ==
                     readability::proper_noun_counting_method::all_proper_nouns_are_familiar)
                {
                // only load the data for standard projects
                if (HasUI())
                    {
                    GetDaleChallHardWordData()->SetItemText(
                        GetTotalUniqueDCHardWords(), 0, wordPos->first.c_str(),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    if (wordPos->second.first == nonProperCount)
                        {
                        GetDaleChallHardWordData()->SetItemValue(GetTotalUniqueDCHardWords(), 1,
                                                                 nonProperCount);
                        }
                    else
                        {
                        GetDaleChallHardWordData()->SetItemText(
                            GetTotalUniqueDCHardWords(), 1,
                            wxString::Format(
                                // TRANSLATORS: %zu are word total placeholders
                                _(L"%zu (%zu total occurrences, %zu proper and familiar, "
                                  "%zu non-proper and unfamiliar)"),
                                nonProperCount, wordPos->second.first,
                                (wordPos->second.first - nonProperCount), nonProperCount),
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            nonProperCount);
                        }
                    auto replacement = dale_chall_replacement_list.find(wordPos->first.c_str());
                    if (replacement.first)
                        {
                        GetDaleChallHardWordData()->SetItemText(
                            GetTotalUniqueDCHardWords(), 2, replacement.second.c_str(),
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        }
                    }
                ++m_uniqueDCHardWords;
                m_totalHardWordsDaleChall += nonProperCount;
                }
            else if (GetDaleChallProperNounCountingMethod() ==
                     readability::proper_noun_counting_method::all_proper_nouns_are_unfamiliar)
                {
                // only load the data for standard projects
                if (HasUI())
                    {
                    GetDaleChallHardWordData()->SetItemText(
                        GetTotalUniqueDCHardWords(), 0, wordPos->first.c_str(),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    GetDaleChallHardWordData()->SetItemValue(GetTotalUniqueDCHardWords(), 1,
                                                             wordPos->second.first);
                    auto replacement = dale_chall_replacement_list.find(wordPos->first.c_str());
                    if (replacement.first)
                        {
                        GetDaleChallHardWordData()->SetItemText(
                            GetTotalUniqueDCHardWords(), 2, replacement.second.c_str(),
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        }
                    }
                ++m_uniqueDCHardWords;
                m_totalHardWordsDaleChall += wordPos->second.first;
                }
            else if (GetDaleChallProperNounCountingMethod() ==
                     readability::proper_noun_counting_method::
                         only_count_first_instance_of_proper_noun_as_unfamiliar)
                {
                // only load the data for standard projects
                if (HasUI())
                    {
                    GetDaleChallHardWordData()->SetItemText(
                        GetTotalUniqueDCHardWords(), 0, wordPos->first.c_str(),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    if (wordPos->second.first == nonProperCount || wordPos->second.first == 1)
                        {
                        GetDaleChallHardWordData()->SetItemValue(GetTotalUniqueDCHardWords(), 1,
                                                                 wordPos->second.first);
                        }
                    else if (nonProperCount == 0)
                        {
                        GetDaleChallHardWordData()->SetItemText(
                            GetTotalUniqueDCHardWords(), 1,
                            wxString::Format(
                                _(L"1 (%zu total occurrences, only first occurrence unfamiliar)"),
                                wordPos->second.first),
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            1);
                        }
                    else
                        {
                        GetDaleChallHardWordData()->SetItemText(
                            GetTotalUniqueDCHardWords(), 1,
                            wxString::Format(
                                // TRANSLATORS: %zu are word total placeholders
                                _(L"%zu (%zu total occurrences. First proper occurrence "
                                  "unfamiliar, %zu non-proper and unfamiliar)"),
                                nonProperCount + 1, wordPos->second.first, nonProperCount),
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            nonProperCount + 1);
                        }
                    auto replacement = dale_chall_replacement_list.find(wordPos->first.c_str());
                    if (replacement.first)
                        {
                        GetDaleChallHardWordData()->SetItemText(
                            GetTotalUniqueDCHardWords(), 2, replacement.second.c_str(),
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        }
                    }
                ++m_uniqueDCHardWords;
                m_totalHardWordsDaleChall += (wordPos->second.first == nonProperCount) ?
                                                 wordPos->second.first :
                                                 nonProperCount + 1;
                }
            }
        if (!allInstancesAreProper && !isSpacheWord(wordPos->first))
            {
            // only load the data for standard projects
            if (HasUI())
                {
                GetSpacheHardWordData()->SetItemText(
                    GetTotalUniqueHardWordsSpache(), 0, wordPos->first.c_str(),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                if (wordPos->second.first == nonProperCount)
                    {
                    GetSpacheHardWordData()->SetItemValue(GetTotalUniqueHardWordsSpache(), 1,
                                                          nonProperCount);
                    }
                else
                    {
                    GetSpacheHardWordData()->SetItemText(
                        GetTotalUniqueHardWordsSpache(), 1,
                        wxString::Format(
                            // TRANSLATORS: %zu are word total placeholders
                            _(L"%zu (%zu total occurrences, %zu proper and familiar, "
                              "%zu non-proper and unfamiliar)"),
                            nonProperCount, wordPos->second.first,
                            (wordPos->second.first - nonProperCount), nonProperCount),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        nonProperCount);
                    }
                auto replacement = spache_replacement_list.find(wordPos->first.c_str());
                if (replacement.first)
                    {
                    GetSpacheHardWordData()->SetItemText(
                        GetTotalUniqueHardWordsSpache(), 2, replacement.second.c_str(),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    }
                }
            ++m_uniqueSpacheHardWords;
            m_totalHardWordsSpache += nonProperCount;
            }
        // go through the custom tests
        for (auto& customTest : m_customTestsInUse)
            {
            /* if not using familiar word then skip. Also need to skip if using CustomHJ or
               CustomDC (if regular DC is excluding only list items)--
               in that case, it will be handled in next loop.*/
            if (!customTest.GetIterator()->is_using_familiar_words() ||
                (customTest.IsHarrisJacobsonFormula() &&
                 GetHarrisJacobsonTextExclusionMode() ==
                     SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ||
                (customTest.IsDaleChallFormula() &&
                 GetDaleChallTextExclusionMode() ==
                     SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings))
                {
                continue;
                }
            if (!customTest.GetIterator()->is_word_familiar(
                    wordPos->first, false,
                    customTest.GetIterator()->is_including_numeric_as_familiar()))
                {
                if (allInstancesAreProper &&
                    customTest.GetIterator()->get_proper_noun_method() ==
                        readability::proper_noun_counting_method::all_proper_nouns_are_familiar)
                    {
                    continue;
                    }
                if (customTest.GetIterator()->get_proper_noun_method() ==
                    readability::proper_noun_counting_method::all_proper_nouns_are_familiar)
                    {
                    // only load the data if user asked for this view
                    if (HasUI())
                        {
                        customTest.GetListViewData()->SetItemText(
                            customTest.GetUniqueUnfamiliarWordCount(), 0, wordPos->first.c_str(),
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        if (wordPos->second.first == nonProperCount)
                            {
                            customTest.GetListViewData()->SetItemValue(
                                customTest.GetUniqueUnfamiliarWordCount(), 1, nonProperCount);
                            }
                        else
                            {
                            customTest.GetListViewData()->SetItemText(
                                customTest.GetUniqueUnfamiliarWordCount(), 1,
                                wxString::Format(
                                    // TRANSLATORS: %zu are word total placeholders
                                    _(L"%zu (%zu total occurrences, %zu proper and familiar, "
                                      "%zu non-proper and unfamiliar)"),
                                    nonProperCount, wordPos->second.first,
                                    (wordPos->second.first - nonProperCount), nonProperCount),
                                Wisteria::NumberFormatInfo{
                                    Wisteria::NumberFormatInfo::NumberFormatType::
                                        StandardFormatting },
                                nonProperCount);
                            }
                        }
                    customTest.IncrementUnfamiliarWordCount(nonProperCount);
                    }
                else if (customTest.GetIterator()->get_proper_noun_method() ==
                         readability::proper_noun_counting_method::all_proper_nouns_are_unfamiliar)
                    {
                    // only load the data if user asked for this view
                    if (HasUI())
                        {
                        customTest.GetListViewData()->SetItemText(
                            customTest.GetUniqueUnfamiliarWordCount(), 0, wordPos->first.c_str(),
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        customTest.GetListViewData()->SetItemValue(
                            customTest.GetUniqueUnfamiliarWordCount(), 1, wordPos->second.first);
                        }
                    customTest.IncrementUnfamiliarWordCount(wordPos->second.first);
                    }
                else if (customTest.GetIterator()->get_proper_noun_method() ==
                         readability::proper_noun_counting_method::
                             only_count_first_instance_of_proper_noun_as_unfamiliar)
                    {
                    // only load the data if user asked for this view
                    if (HasUI())
                        {
                        customTest.GetListViewData()->SetItemText(
                            customTest.GetUniqueUnfamiliarWordCount(), 0, wordPos->first.c_str(),
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        if (wordPos->second.first == nonProperCount || wordPos->second.first == 1)
                            {
                            customTest.GetListViewData()->SetItemValue(
                                customTest.GetUniqueUnfamiliarWordCount(), 1,
                                wordPos->second.first);
                            }
                        else if (nonProperCount == 0)
                            {
                            customTest.GetListViewData()->SetItemText(
                                customTest.GetUniqueUnfamiliarWordCount(), 1,
                                wxString::Format(_(L"1 (%zu total occurrences, only first "
                                                   "occurrence unfamiliar)"),
                                                 wordPos->second.first),
                                Wisteria::NumberFormatInfo{
                                    Wisteria::NumberFormatInfo::NumberFormatType::
                                        StandardFormatting },
                                1);
                            }
                        else
                            {
                            customTest.GetListViewData()->SetItemText(
                                customTest.GetUniqueUnfamiliarWordCount(), 1,
                                wxString::Format(
                                    // TRANSLATORS: %zu are word total placeholders
                                    _(L"%zu (%zu total occurrences. "
                                      "First proper occurrence unfamiliar, %zu "
                                      "non-proper and unfamiliar)"),
                                    nonProperCount + 1, wordPos->second.first, nonProperCount),
                                Wisteria::NumberFormatInfo{
                                    Wisteria::NumberFormatInfo::NumberFormatType::
                                        StandardFormatting },
                                nonProperCount + 1);
                            }
                        }
                    customTest.IncrementUnfamiliarWordCount(
                        (wordPos->second.first == nonProperCount) ? wordPos->second.first :
                                                                    nonProperCount + 1);
                    }
                customTest.IncrementUniqueUnfamiliarWordCount();
                }
            }
        }

    // keywords (uncommon words removed, remaining stemmed and combined)
    size_t uniqueImportWordsCount{ 0 };
    if (HasUI())
        {
        // keywords list
        if (GetKeyWordsBaseData() == nullptr)
            {
            m_keyWordsBaseData = std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>();
            }
        GetKeyWordsBaseData()->DeleteAllItems();
        GetKeyWordsBaseData()->SetSize(keyWordsStemmedWithCounts.get_data().size(), 2);

        // word cloud
        if (m_keyWordsDataset == nullptr)
            {
            m_keyWordsDataset = std::make_shared<Wisteria::Data::Dataset>();
            }
        m_keyWordsDataset->Clear();
        m_keyWordsDataset->AddCategoricalColumn(GetWordsColumnName());
        m_keyWordsDataset->AddContinuousColumn(GetWordsCountsColumnName());
        assert(m_keyWordsDataset->GetCategoricalColumns().size() == 1 &&
               L"Hard word dataset invalid!");
        assert(m_keyWordsDataset->GetRowCount() == 0 && L"Hard word dataset should be empty!");
        m_keyWordsDataset->Resize(keyWordsStemmedWithCounts.get_data().size());
        auto keyWordsColumn = m_keyWordsDataset->GetCategoricalColumn(GetWordsColumnName());
        auto keydWordsFreqColumn =
            m_keyWordsDataset->GetContinuousColumn(GetWordsCountsColumnName());

        wxString allValuesStr;
        size_t wordCloudWordsCount{ 0 };
        for (const auto& [keyWordStem, keyWordFreqInfo] : keyWordsStemmedWithCounts.get_data())
            {
            if (GetWordsBreakdownInfo().IsKeyWordsEnabled())
                {
                // aggregate all the variations of the current word that share a common stem
                allValuesStr.clear();
                for (const auto& subWord : keyWordFreqInfo.first.get_data())
                    {
                    allValuesStr.append(subWord.first.c_str()).append(L"; ");
                    }
                allValuesStr.Trim().RemoveLast();
                assert(!allValuesStr.empty() && L"Empty word list from stemmed word?!");

                GetKeyWordsBaseData()->SetItemText(
                    uniqueImportWordsCount, 0, allValuesStr,
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                GetKeyWordsBaseData()->SetItemValue(uniqueImportWordsCount++, 1,
                                                    keyWordFreqInfo.second);
                }

            if (GetWordsBreakdownInfo().IsWordCloudEnabled())
                {
                // which variation of the current stem occurs the most often
                auto mostFrequentWordVariation = std::ranges::max_element(
                    keyWordFreqInfo.first.get_data(), [](const auto& lhv, const auto& rhv) noexcept
                    { return lhv.second < rhv.second; });
                assert(mostFrequentWordVariation != keyWordFreqInfo.first.get_data().cend() &&
                       L"Empty word list for stemmed word?!");
                // add the next word to the dataset's string table
                const auto nextKey = keyWordsColumn->GetNextKey();
                if (mostFrequentWordVariation != keyWordFreqInfo.first.get_data().cend())
                    {
                    keyWordsColumn->GetStringTable().insert(
                        std::make_pair(nextKey, mostFrequentWordVariation->first.c_str()));
                    }
                // could never happen, but for robustness's sake use the stem word
                // if the word list for the stem is empty
                else
                    {
                    keyWordsColumn->GetStringTable().insert(
                        std::make_pair(nextKey, keyWordStem.c_str()));
                    }
                // add the new string table ID (i.e., the current word) and
                // respective frequency to the current row
                keyWordsColumn->SetValue(wordCloudWordsCount, nextKey);
                keydWordsFreqColumn->SetValue(wordCloudWordsCount++, keyWordFreqInfo.second);
                }
            }
        }

    // special handling for tests that include headers, but not lists
    m_totalWordsFromCompleteSentencesAndHeaders = 0;
    m_totalSentencesFromCompleteSentencesAndHeaders = 0;
    m_totalNumeralsFromCompleteSentencesAndHeaders = 0;
    m_totalCharactersFromCompleteSentencesAndHeaders = 0;
    double_frequency_set<word_case_insensitive_no_stem> completeSentAndHeaderWordFrequencyMap;
    for (auto sentPos = GetWords()->get_sentences().cbegin();
         sentPos != GetWords()->get_sentences().cend(); ++sentPos)
        {
        if (sentPos->is_valid() || sentPos->get_type() == grammar::sentence_paragraph_type::header)
            {
            ++m_totalSentencesFromCompleteSentencesAndHeaders;
            // go through the words in the current sentence and add them to the map
            for (size_t wordIter = sentPos->get_first_word_index();
                 wordIter <= sentPos->get_last_word_index(); ++wordIter)
                {
                ++m_totalWordsFromCompleteSentencesAndHeaders;
                if (GetWords()->get_words()[wordIter].is_numeric())
                    {
                    ++m_totalNumeralsFromCompleteSentencesAndHeaders;
                    }
                m_totalCharactersFromCompleteSentencesAndHeaders +=
                    GetWords()->get_words()[wordIter].get_length_excluding_punctuation();
                completeSentAndHeaderWordFrequencyMap.insert(
                    GetWords()->get_words()[wordIter],
                    GetWords()->get_words()[wordIter].is_proper_noun());
                }
            }
        }
    // resize the difficult word vectors
    if (HasUI() && (GetHarrisJacobsonTextExclusionMode() ==
                    SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings))
        {
        if (GetHarrisJacobsonHardWordDataData() == nullptr)
            {
            m_harrisJacobsonHardWordsData =
                std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>();
            }
        GetHarrisJacobsonHardWordDataData()->DeleteAllItems();
        GetHarrisJacobsonHardWordDataData()->SetSize(
            completeSentAndHeaderWordFrequencyMap.get_data().size(), 3);
        }
    if (HasUI() && (GetDaleChallTextExclusionMode() ==
                    SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings))
        {
        if (GetDaleChallHardWordData() == nullptr)
            {
            m_DCHardWordsData = std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>();
            }
        GetDaleChallHardWordData()->DeleteAllItems();
        GetDaleChallHardWordData()->SetSize(completeSentAndHeaderWordFrequencyMap.get_data().size(),
                                            3);
        }
    for (auto& customTest : m_customTestsInUse)
        {
        if ((customTest.IsHarrisJacobsonFormula() &&
             GetHarrisJacobsonTextExclusionMode() ==
                 SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ||
            (customTest.IsDaleChallFormula() &&
             GetDaleChallTextExclusionMode() ==
                 SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings))
            {
            customTest.Reset();
            if (HasUI() && customTest.GetIterator()->is_using_familiar_words())
                {
                customTest.GetListViewData()->SetSize(
                    completeSentAndHeaderWordFrequencyMap.get_data().size(), 2);
                }
            }
        }
    for (auto wordPos = completeSentAndHeaderWordFrequencyMap.get_data().cbegin();
         wordPos != completeSentAndHeaderWordFrequencyMap.get_data().cend(); ++wordPos)
        {
        assert(wordPos->second.first >= wordPos->second.second);
        /* subtract number of times word is proper from total count of word
           to see if at least on instance is NOT proper. If they are equal, then
           all instances are proper and therefore cannot be an unfamiliar word.*/
        const bool allInstancesAreProper = (wordPos->second.first == wordPos->second.second);
        const size_t nonProperCount = (wordPos->second.first - wordPos->second.second);
        // SOL (same as SMOG, but needs to include headers)
        if (wordPos->first.get_syllable_count() >= 3)
            {
            m_totalHardWordsSol += wordPos->second.first;
            }
        if (!allInstancesAreProper &&
            (GetHarrisJacobsonTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) &&
            !isHarrisJacobsonWord(wordPos->first))
            {
            // only load the data for standard projects
            if (HasUI())
                {
                GetHarrisJacobsonHardWordDataData()->SetItemText(
                    GetTotalUniqueHarrisJacobsonHardWords(), 0, wordPos->first.c_str(),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                if (wordPos->second.first == nonProperCount)
                    {
                    GetHarrisJacobsonHardWordDataData()->SetItemValue(
                        GetTotalUniqueHarrisJacobsonHardWords(), 1, nonProperCount);
                    }
                else
                    {
                    GetHarrisJacobsonHardWordDataData()->SetItemText(
                        GetTotalUniqueHarrisJacobsonHardWords(), 1,
                        wxString::Format(
                            // TRANSLATORS: %zu are word total placeholders
                            _(L"%zu (%zu total occurrences, %zu proper and familiar, "
                              "%zu non-proper and unfamiliar)"),
                            nonProperCount, wordPos->second.first,
                            (wordPos->second.first - nonProperCount), nonProperCount),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        nonProperCount);
                    }
                auto replacement = harris_jacobson_replacement_list.find(wordPos->first.c_str());
                if (replacement.first)
                    {
                    GetHarrisJacobsonHardWordDataData()->SetItemText(
                        GetTotalUniqueHarrisJacobsonHardWords(), 2, replacement.second.c_str(),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    }
                }
            ++m_uniqueHarrisJacobsonHardWords;
            m_totalHardWordsHarrisJacobson += nonProperCount;
            }
        if ((GetDaleChallTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) &&
            !isDCWord(wordPos->first))
            {
            // all forms of word are proper and proper words are familiar?
            // Then it can't be a hard word at all.
            if (allInstancesAreProper &&
                GetDaleChallProperNounCountingMethod() ==
                    readability::proper_noun_counting_method::all_proper_nouns_are_familiar)
                { /* no-op*/
                ;
                }
            else if (GetDaleChallProperNounCountingMethod() ==
                     readability::proper_noun_counting_method::all_proper_nouns_are_familiar)
                {
                // only load the data for standard projects
                if (HasUI())
                    {
                    GetDaleChallHardWordData()->SetItemText(
                        GetTotalUniqueDCHardWords(), 0, wordPos->first.c_str(),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    if (wordPos->second.first == nonProperCount)
                        {
                        GetDaleChallHardWordData()->SetItemValue(GetTotalUniqueDCHardWords(), 1,
                                                                 nonProperCount);
                        }
                    else
                        {
                        GetDaleChallHardWordData()->SetItemText(
                            GetTotalUniqueDCHardWords(), 1,
                            wxString::Format(
                                // TRANSLATORS: %zu are word total placeholders
                                _(L"%zu (%zu total occurrences, %zu proper and familiar, "
                                  "%zu non-proper and unfamiliar)"),
                                nonProperCount, wordPos->second.first,
                                (wordPos->second.first - nonProperCount), nonProperCount),
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            nonProperCount);
                        }
                    auto replacement = dale_chall_replacement_list.find(wordPos->first.c_str());
                    if (replacement.first)
                        {
                        GetDaleChallHardWordData()->SetItemText(
                            GetTotalUniqueDCHardWords(), 2, replacement.second.c_str(),
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        }
                    }
                ++m_uniqueDCHardWords;
                m_totalHardWordsDaleChall += nonProperCount;
                }
            else if (GetDaleChallProperNounCountingMethod() ==
                     readability::proper_noun_counting_method::all_proper_nouns_are_unfamiliar)
                {
                // only load the data for standard projects
                if (HasUI())
                    {
                    GetDaleChallHardWordData()->SetItemText(
                        GetTotalUniqueDCHardWords(), 0, wordPos->first.c_str(),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    GetDaleChallHardWordData()->SetItemValue(GetTotalUniqueDCHardWords(), 1,
                                                             wordPos->second.first);
                    auto replacement = dale_chall_replacement_list.find(wordPos->first.c_str());
                    if (replacement.first)
                        {
                        GetDaleChallHardWordData()->SetItemText(
                            GetTotalUniqueDCHardWords(), 2, replacement.second.c_str(),
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        }
                    }
                ++m_uniqueDCHardWords;
                m_totalHardWordsDaleChall += wordPos->second.first;
                }
            else if (GetDaleChallProperNounCountingMethod() ==
                     readability::proper_noun_counting_method::
                         only_count_first_instance_of_proper_noun_as_unfamiliar)
                {
                // only load the data for standard projects
                if (HasUI())
                    {
                    GetDaleChallHardWordData()->SetItemText(
                        GetTotalUniqueDCHardWords(), 0, wordPos->first.c_str(),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    if (wordPos->second.first == nonProperCount || wordPos->second.first == 1)
                        {
                        GetDaleChallHardWordData()->SetItemValue(GetTotalUniqueDCHardWords(), 1,
                                                                 wordPos->second.first);
                        }
                    else if (nonProperCount == 0)
                        {
                        GetDaleChallHardWordData()->SetItemText(
                            GetTotalUniqueDCHardWords(), 1,
                            wxString::Format(
                                // TRANSLATORS: %zu are word total placeholders
                                _(L"1 (%zu total occurrences, only first occurrence unfamiliar)"),
                                wordPos->second.first),
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            1);
                        }
                    else
                        {
                        GetDaleChallHardWordData()->SetItemText(
                            GetTotalUniqueDCHardWords(), 1,
                            wxString::Format(
                                // TRANSLATORS: %zu are word total placeholders
                                _(L"%zu (%zu total occurrences. "
                                  "First proper occurrence unfamiliar, %zu "
                                  "non-proper and unfamiliar)"),
                                nonProperCount + 1, wordPos->second.first, nonProperCount),
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            nonProperCount + 1);
                        }
                    auto replacement = dale_chall_replacement_list.find(wordPos->first.c_str());
                    if (replacement.first)
                        {
                        GetDaleChallHardWordData()->SetItemText(
                            GetTotalUniqueDCHardWords(), 2, replacement.second.c_str(),
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        }
                    }
                ++m_uniqueDCHardWords;
                m_totalHardWordsDaleChall += (wordPos->second.first == nonProperCount) ?
                                                 wordPos->second.first :
                                                 nonProperCount + 1;
                }
            }
        // go through the custom tests
        for (auto& customTest : m_customTestsInUse)
            {
            // if not using familiar word then skip.
            if (!customTest.GetIterator()->is_using_familiar_words())
                {
                continue;
                }

            // If using CustomHJ or CustomDC (if regular DC/HJ are excluding only list items)
            // load hard words from this buffer
            if ((customTest.IsHarrisJacobsonFormula() &&
                 GetHarrisJacobsonTextExclusionMode() ==
                     SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ||
                (customTest.IsDaleChallFormula() &&
                 GetDaleChallTextExclusionMode() ==
                     SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings))
                {
                if (!customTest.GetIterator()->is_word_familiar(
                        wordPos->first,
                        // proper noun logic is handled separately below
                        false, customTest.GetIterator()->is_including_numeric_as_familiar()))
                    {
                    if (allInstancesAreProper &&
                        customTest.GetIterator()->get_proper_noun_method() ==
                            readability::proper_noun_counting_method::all_proper_nouns_are_familiar)
                        {
                        continue;
                        }
                    if (customTest.GetIterator()->get_proper_noun_method() ==
                        readability::proper_noun_counting_method::all_proper_nouns_are_familiar)
                        {
                        // only load the data if user asked for this view
                        if (HasUI())
                            {
                            customTest.GetListViewData()->SetItemText(
                                customTest.GetUniqueUnfamiliarWordCount(), 0,
                                wordPos->first.c_str(),
                                Wisteria::NumberFormatInfo{
                                    Wisteria::NumberFormatInfo::NumberFormatType::
                                        StandardFormatting },
                                std::numeric_limits<double>::quiet_NaN());
                            if (wordPos->second.first == nonProperCount)
                                {
                                customTest.GetListViewData()->SetItemValue(
                                    customTest.GetUniqueUnfamiliarWordCount(), 1, nonProperCount);
                                }
                            else
                                {
                                customTest.GetListViewData()->SetItemText(
                                    customTest.GetUniqueUnfamiliarWordCount(), 1,
                                    wxString::Format(
                                        // TRANSLATORS: %zu are word total placeholders
                                        _(L"%zu (%zu total occurrences, %zu proper and familiar, "
                                          "%zu non-proper and unfamiliar)"),
                                        nonProperCount, wordPos->second.first,
                                        (wordPos->second.first - nonProperCount), nonProperCount),
                                    Wisteria::NumberFormatInfo{
                                        Wisteria::NumberFormatInfo::NumberFormatType::
                                            StandardFormatting },
                                    nonProperCount);
                                }
                            }
                        customTest.IncrementUnfamiliarWordCount(nonProperCount);
                        }
                    else if (customTest.GetIterator()->get_proper_noun_method() ==
                             readability::proper_noun_counting_method::
                                 all_proper_nouns_are_unfamiliar)
                        {
                        // only load the data if user asked for this view
                        if (HasUI())
                            {
                            customTest.GetListViewData()->SetItemText(
                                customTest.GetUniqueUnfamiliarWordCount(), 0,
                                wordPos->first.c_str(),
                                Wisteria::NumberFormatInfo{
                                    Wisteria::NumberFormatInfo::NumberFormatType::
                                        StandardFormatting },
                                std::numeric_limits<double>::quiet_NaN());
                            customTest.GetListViewData()->SetItemValue(
                                customTest.GetUniqueUnfamiliarWordCount(), 1,
                                wordPos->second.first);
                            }
                        customTest.IncrementUnfamiliarWordCount(wordPos->second.first);
                        }
                    else if (customTest.GetIterator()->get_proper_noun_method() ==
                             readability::proper_noun_counting_method::
                                 only_count_first_instance_of_proper_noun_as_unfamiliar)
                        {
                        // only load the data if user asked for this view
                        if (HasUI())
                            {
                            customTest.GetListViewData()->SetItemText(
                                customTest.GetUniqueUnfamiliarWordCount(), 0,
                                wordPos->first.c_str(),
                                Wisteria::NumberFormatInfo{
                                    Wisteria::NumberFormatInfo::NumberFormatType::
                                        StandardFormatting },
                                std::numeric_limits<double>::quiet_NaN());
                            if (wordPos->second.first == nonProperCount ||
                                wordPos->second.first == 1)
                                {
                                customTest.GetListViewData()->SetItemValue(
                                    customTest.GetUniqueUnfamiliarWordCount(), 1,
                                    wordPos->second.first);
                                }
                            else if (nonProperCount == 0)
                                {
                                customTest.GetListViewData()->SetItemText(
                                    customTest.GetUniqueUnfamiliarWordCount(), 1,
                                    wxString::Format(
                                        // TRANSLATORS: %zu are word total placeholders
                                        _(L"1 (%zu total occurrences, only first "
                                          "occurrence unfamiliar)"),
                                        wordPos->second.first),
                                    Wisteria::NumberFormatInfo{
                                        Wisteria::NumberFormatInfo::NumberFormatType::
                                            StandardFormatting },
                                    1);
                                }
                            else
                                {
                                customTest.GetListViewData()->SetItemText(
                                    customTest.GetUniqueUnfamiliarWordCount(), 1,
                                    wxString::Format(
                                        // TRANSLATORS: %zu are word total placeholders
                                        _(L"%zu (%zu total occurrences. "
                                          "First proper occurrence unfamiliar, "
                                          "%zu non-proper and unfamiliar)"),
                                        nonProperCount + 1, wordPos->second.first, nonProperCount),
                                    Wisteria::NumberFormatInfo{
                                        Wisteria::NumberFormatInfo::NumberFormatType::
                                            StandardFormatting },
                                    nonProperCount + 1);
                                }
                            }
                        customTest.IncrementUnfamiliarWordCount(
                            (wordPos->second.first == nonProperCount) ? wordPos->second.first :
                                                                        nonProperCount + 1);
                        }
                    customTest.IncrementUniqueUnfamiliarWordCount();
                    }
                }
            }
        }

    // finalize the size of the vectors
    if (HasUI())
        {
        if (GetProperNounsData())
            {
            GetProperNounsData()->SetSize(uniqueProperNouns);
            GetProperNounsData()->ShrinkToFit();
            }
        if (GetContractionsData())
            {
            GetContractionsData()->SetSize(uniqueContractions);
            GetContractionsData()->ShrinkToFit();
            }
        if (GetKeyWordsBaseData())
            {
            GetKeyWordsBaseData()->SetSize(uniqueImportWordsCount);
            GetKeyWordsBaseData()->ShrinkToFit();
            }
        if (Get3SyllablePlusData())
            {
            Get3SyllablePlusData()->SetSize(GetTotalUnique3PlusSyllableWords());
            Get3SyllablePlusData()->ShrinkToFit();
            }
        if (Get6CharacterPlusData())
            {
            Get6CharacterPlusData()->SetSize(GetTotalUnique6CharsPlusWords());
            Get6CharacterPlusData()->ShrinkToFit();
            }
        if (GetDaleChallHardWordData())
            {
            GetDaleChallHardWordData()->SetSize(GetTotalUniqueDCHardWords());
            GetDaleChallHardWordData()->ShrinkToFit();
            }
        if (GetSpacheHardWordData())
            {
            GetSpacheHardWordData()->SetSize(GetTotalUniqueHardWordsSpache());
            GetSpacheHardWordData()->ShrinkToFit();
            }
        if (GetHarrisJacobsonHardWordDataData())
            {
            GetHarrisJacobsonHardWordDataData()->SetSize(GetTotalUniqueHarrisJacobsonHardWords());
            GetHarrisJacobsonHardWordDataData()->ShrinkToFit();
            }
        // already the right size
        if (GetUnusedDolchWordData())
            {
            GetUnusedDolchWordData()->ShrinkToFit();
            }
        if (GetDolchWordData())
            {
            GetDolchWordData()->SetSize(uniqueDolchWords);
            GetDolchWordData()->ShrinkToFit();
            }
        if (GetNonDolchWordData())
            {
            GetNonDolchWordData()->SetSize(nonUniqueDolchWords);
            GetNonDolchWordData()->ShrinkToFit();
            }
        }
    }

//-------------------------------------------------------
void BaseProject::CalculateStatisticsIgnoringInvalidSentences()
    {
    m_totalWords = GetWords()->get_valid_word_count();
    m_totalSentences = GetWords()->get_complete_sentence_count();
    m_totalParagraphs = GetWords()->get_valid_paragraph_count();

    // Number of total syllables
    m_totalSyllables = std::accumulate(
        GetWords()->get_words().begin(), GetWords()->get_words().end(), static_cast<size_t>(0),
        add_valid_syllable_size<word_case_insensitive_no_stem>(
            GetNumeralSyllabicationMethod() == NumeralSyllabize::WholeWordIsOneSyllable));
    // Number of syllables, numerals just one syllable
    m_totalSyllablesNumeralsOneSyllable = std::accumulate(
        GetWords()->get_words().begin(), GetWords()->get_words().end(), static_cast<size_t>(0),
        add_valid_syllable_size<word_case_insensitive_no_stem>(true));
    // Number of syllables, numerals fully syllabized
    m_totalSyllablesNumeralsFullySyllabized = std::accumulate(
        GetWords()->get_words().begin(), GetWords()->get_words().end(), static_cast<size_t>(0),
        add_valid_syllable_size<word_case_insensitive_no_stem>(false));
    // Number of syllables, ignoring numeric strings
    m_totalSyllablesIgnoringNumerals = std::accumulate(
        GetWords()->get_words().begin(), GetWords()->get_words().end(), static_cast<size_t>(0),
        add_valid_syllable_size_ignore_numerals<word_case_insensitive_no_stem>());
    // Number of syllables, ignoring numeric and proper strings
    m_totalSyllablesIgnoringNumeralsAndProperNouns = std::accumulate(
        GetWords()->get_words().begin(), GetWords()->get_words().end(), static_cast<size_t>(0),
        add_valid_syllable_size_ignore_numerals_and_proper_nouns<word_case_insensitive_no_stem>());
    // Number of total characters
    m_totalCharacters = std::accumulate(
        GetWords()->get_words().begin(), GetWords()->get_words().end(), static_cast<size_t>(0),
        add_valid_word_size_excluding_punctuation<word_case_insensitive_no_stem>());
    // Number of total characters and punctuation
    // (this includes punctuation that is part of the words and between them)
    m_totalCharactersPlusPunctuation =
        std::accumulate(GetWords()->get_words().begin(), GetWords()->get_words().end(),
                        static_cast<size_t>(0),
                        add_valid_word_size<word_case_insensitive_no_stem>()) +
        GetWords()->get_valid_punctuation_count();
    // Monosyllabic words
    m_totalMonoSyllabic = std::ranges::count_if(
        GetWords()->get_words(),
        valid_syllable_count_equals<word_case_insensitive_no_stem>(
            1, GetNumeralSyllabicationMethod() == NumeralSyllabize::WholeWordIsOneSyllable));
    // Numerals
    m_totalNumerals = std::ranges::count_if(GetWords()->get_words(),
                                            is_valid_numeric<word_case_insensitive_no_stem>());
    // proper nouns
    m_totalProperNouns = std::ranges::count_if(
        GetWords()->get_words(), is_valid_proper_noun<word_case_insensitive_no_stem>());
    // hard lix/rix words
    m_totalHardWordsLixRix = std::ranges::count_if(
        GetWords()->get_words(),
        valid_word_length_excluding_punctuation_greater_equals<word_case_insensitive_no_stem>(7));

    // load the unique words and their frequencies.
    m_word_frequency_map = std::make_shared<double_frequency_set<word_case_insensitive_no_stem>>();
    for (const auto& word : GetWords()->get_words())
        {
        if (word.is_valid())
            {
            // go through the words and add them to the map
            m_word_frequency_map->insert(word, word.is_proper_noun());
            }
        }

    // difficult sentences
    if (m_longSentenceMethod == LongSentence::OutlierLength)
        {
        if (GetTotalSentences() > 0)
            {
            // if to be calculated by outlier range
            std::vector<double> statisticSizes(GetTotalSentences());

            // load a vector of the sentence lengths
            copy_member_if(
                GetWords()->get_sentences().begin(), GetWords()->get_sentences().end(),
                statisticSizes.begin(),
                [](const grammar::sentence_info& s) noexcept { return s.is_valid(); },
                [](const grammar::sentence_info& s) noexcept { return s.get_valid_word_count(); });
            const statistics::find_outliers fos(statisticSizes);
            m_difficultSentenceLength = static_cast<int>(fos.get_upper_outlier_boundary());
            }
        else
            {
            LogMessage(_(L"No sentences found. Unable to calculate sentence length outlier range."),
                       _(L"Warning"), wxOK | wxICON_EXCLAMATION);
            m_difficultSentenceLength = 0;
            }
        }

    m_totalInterrogativeSentences = m_totalExclamatorySentences = m_totalSentenceUnits = 0;
    if (GetTotalSentences() > 0)
        {
        // Number of overly long sentences
        m_totalOverlyLongSentences = std::ranges::count_if(
            GetWords()->get_sentences(),
            grammar::complete_sentence_length_greater_than(m_difficultSentenceLength));

        for (const auto& sent : GetWords()->get_sentences())
            {
            if (sent.is_valid())
                {
                const wchar_t endingChar = sent.get_ending_punctuation();
                if (endingChar == common_lang_constants::QUESTION_MARK ||
                    endingChar == common_lang_constants::QUESTION_MARK_FULL_WIDTH)
                    {
                    ++m_totalInterrogativeSentences;
                    }
                else if (endingChar == common_lang_constants::EXCLAMATION_MARK ||
                         endingChar == common_lang_constants::EXCLAMATION_MARK_FULL_WIDTH)
                    {
                    ++m_totalExclamatorySentences;
                    }
                else if (endingChar == common_lang_constants::INTERROBANG)
                    {
                    ++m_totalExclamatorySentences;
                    }
                // count the sentence units
                m_totalSentenceUnits += sent.get_unit_count();
                }
            }

        auto longestSent = std::ranges::max_element(GetWords()->get_sentences(),
                                                    grammar::complete_sentence_length_less{});
        m_longestSentence = longestSent->get_valid_word_count();
        // be sure to add 1 to make it one-indexed when being displayed
        m_longestSentenceIndex = (longestSent - GetWords()->get_sentences().begin());
        }
    // No valid sentences? Just reset these values then.
    else
        {
        m_totalSentenceUnits = 0;
        m_totalOverlyLongSentences = 0;
        m_longestSentence = 0;
        m_longestSentenceIndex = 0;
        }

    // we count these in ignored sentences too because they are just
    // general grammar issues to improve the overall document
    SetDuplicateWordCount(GetWords()->get_duplicate_word_indices().size());
    SetMismatchedArticleCount(GetWords()->get_incorrect_article_indices().size());
    SetPassiveVoicesCount(GetWords()->get_passive_voice_indices().size());
    SetMisspelledWordCount(GetWords()->get_misspelled_words().size());
    SetOverusedWordsBySentenceCount(GetWords()->get_overused_words_by_sentence().size());

    m_wordyPhraseCount = m_redundantPhraseCount = m_wordingErrorCount = m_clicheCount = 0;
    const auto& wordyIndices = GetWords()->get_known_phrase_indices();
    const auto& wordyPhrases = GetWords()->get_known_phrases().get_phrases();
    for (const auto& wordyIndex : wordyIndices)
        {
        switch (wordyPhrases[wordyIndex.second].first.get_type())
            {
        case grammar::phrase_type::phrase_wordy:
            ++m_wordyPhraseCount;
            break;
        case grammar::phrase_type::phrase_redundant:
            ++m_redundantPhraseCount;
            break;
        case grammar::phrase_type::phrase_cliche:
            ++m_clicheCount;
            break;
        case grammar::phrase_type::phrase_error:
            ++m_wordingErrorCount;
            break;
            };
        }

    m_sentenceStartingWithConjunctionsCount =
        GetWords()->get_conjunction_beginning_sentences().size();
    m_sentenceStartingWithLowercaseCount = GetWords()->get_lowercase_beginning_sentences().size();

    ReviewStatGoals();
    }

//-------------------------------------------------------
void BaseProject::CalculateStatistics()
    {
    m_totalWords = GetWords()->get_word_count();
    m_totalSentences = GetWords()->get_sentence_count();
    m_totalParagraphs = GetWords()->get_paragraph_count();

    // Number of total syllables
    m_totalSyllables = std::accumulate(
        GetWords()->get_words().begin(), GetWords()->get_words().end(), static_cast<size_t>(0),
        add_syllable_size<word_case_insensitive_no_stem>(GetNumeralSyllabicationMethod() ==
                                                         NumeralSyllabize::WholeWordIsOneSyllable));
    // Number of syllables, numerals just one syllable
    m_totalSyllablesNumeralsOneSyllable = std::accumulate(
        GetWords()->get_words().begin(), GetWords()->get_words().end(), static_cast<size_t>(0),
        add_syllable_size<word_case_insensitive_no_stem>(true));
    // Number of syllables, numerals fully syllabized
    m_totalSyllablesNumeralsFullySyllabized = std::accumulate(
        GetWords()->get_words().begin(), GetWords()->get_words().end(), static_cast<size_t>(0),
        add_syllable_size<word_case_insensitive_no_stem>(false));
    // Number of syllables, ignoring numeric strings
    m_totalSyllablesIgnoringNumerals = std::accumulate(
        GetWords()->get_words().begin(), GetWords()->get_words().end(), static_cast<size_t>(0),
        add_syllable_size_ignore_numerals<word_case_insensitive_no_stem>());
    // Number of syllables, ignoring numeric and proper strings
    m_totalSyllablesIgnoringNumeralsAndProperNouns = std::accumulate(
        GetWords()->get_words().begin(), GetWords()->get_words().end(), static_cast<size_t>(0),
        add_syllable_size_ignore_numerals_and_proper_nouns<word_case_insensitive_no_stem>());
    // Number of total characters
    m_totalCharacters = std::accumulate(
        GetWords()->get_words().begin(), GetWords()->get_words().end(), static_cast<size_t>(0),
        add_word_size_excluding_punctuation<word_case_insensitive_no_stem>());
    // Number of total characters and punctuation
    // (this includes punctuation that is part of the words and between them)
    m_totalCharactersPlusPunctuation =
        std::accumulate(GetWords()->get_words().begin(), GetWords()->get_words().end(),
                        static_cast<size_t>(0), add_word_size<word_case_insensitive_no_stem>()) +
        GetWords()->get_punctuation_count();
    // Monosyllabic words
    m_totalMonoSyllabic = std::ranges::count_if(
        GetWords()->get_words(),
        syllable_count_equals<word_case_insensitive_no_stem>(
            1, GetNumeralSyllabicationMethod() == NumeralSyllabize::WholeWordIsOneSyllable));
    // Numerals
    m_totalNumerals =
        std::ranges::count_if(GetWords()->get_words(), is_numeric<word_case_insensitive_no_stem>());
    // proper nouns
    m_totalProperNouns = std::ranges::count_if(GetWords()->get_words(),
                                               is_proper_noun<word_case_insensitive_no_stem>());
    // hard lix/rix words
    m_totalHardWordsLixRix = std::ranges::count_if(
        GetWords()->get_words(),
        word_length_excluding_punctuation_greater_equals<word_case_insensitive_no_stem>(7));

    // load the unique words and their frequencies
    m_word_frequency_map = std::make_shared<double_frequency_set<word_case_insensitive_no_stem>>();
    for (auto sentPos = GetWords()->get_sentences().cbegin();
         sentPos != GetWords()->get_sentences().cend(); ++sentPos)
        {
        // go through the words in the current sentence and add them to the map
        for (size_t i = sentPos->get_first_word_index(); i <= sentPos->get_last_word_index(); ++i)
            {
            auto currentWord = GetWords()->get_words().begin() + i;
            m_word_frequency_map->insert(*currentWord, currentWord->is_proper_noun());
            }
        }

    // difficult sentences
    if (m_longSentenceMethod == LongSentence::OutlierLength)
        {
        if (GetTotalSentences() > 0)
            {
            // if to be calculated by outlier range
            std::vector<double> statisticSizes(GetTotalSentences());

            // load a vector of the sentence lengths
            copy_member(GetWords()->get_sentences().begin(), GetWords()->get_sentences().end(),
                        statisticSizes.begin(), [](const grammar::sentence_info& s) noexcept
                        { return s.get_word_count(); });
            const statistics::find_outliers fos(statisticSizes);
            m_difficultSentenceLength = static_cast<int>(fos.get_upper_outlier_boundary());
            }
        else
            {
            LogMessage(_(L"No sentences found. Unable to calculate sentence length outlier range."),
                       _(L"Warning"), wxOK | wxICON_EXCLAMATION);
            m_difficultSentenceLength = 0;
            }
        }

    m_totalInterrogativeSentences = m_totalExclamatorySentences = m_totalSentenceUnits = 0;
    if (GetTotalSentences() > 0)
        {
        // Number of overly long sentences
        m_totalOverlyLongSentences =
            std::ranges::count_if(GetWords()->get_sentences(),
                                  grammar::sentence_length_greater_than(m_difficultSentenceLength));

        for (const auto& sent : GetWords()->get_sentences())
            {
            const wchar_t endingChar = sent.get_ending_punctuation();
            if (endingChar == common_lang_constants::QUESTION_MARK ||
                endingChar == common_lang_constants::QUESTION_MARK_FULL_WIDTH)
                {
                ++m_totalInterrogativeSentences;
                }
            else if (endingChar == common_lang_constants::EXCLAMATION_MARK ||
                     endingChar == common_lang_constants::EXCLAMATION_MARK_FULL_WIDTH)
                {
                ++m_totalExclamatorySentences;
                }
            else if (endingChar == common_lang_constants::INTERROBANG)
                {
                ++m_totalExclamatorySentences;
                }
            // count the sentence units
            m_totalSentenceUnits += sent.get_unit_count();
            }

        auto longestSent = std::max_element(GetWords()->get_sentences().cbegin(),
                                            GetWords()->get_sentences().cend());
        m_longestSentence = longestSent->get_word_count();
        // be sure to add 1 to make it one-indexed when being displayed
        m_longestSentenceIndex = (longestSent - GetWords()->get_sentences().begin());
        }
    // No valid sentences? Just reset these values then.
    else
        {
        m_totalSentenceUnits = 0;
        m_totalOverlyLongSentences = 0;
        m_longestSentence = 0;
        m_longestSentenceIndex = 0;
        }

    SetDuplicateWordCount(GetWords()->get_duplicate_word_indices().size());
    SetMismatchedArticleCount(GetWords()->get_incorrect_article_indices().size());
    SetPassiveVoicesCount(GetWords()->get_passive_voice_indices().size());
    SetMisspelledWordCount(GetWords()->get_misspelled_words().size());
    SetOverusedWordsBySentenceCount(GetWords()->get_overused_words_by_sentence().size());

    m_wordyPhraseCount = m_redundantPhraseCount = m_wordingErrorCount = m_clicheCount = 0;
    const auto& wordyIndices = GetWords()->get_known_phrase_indices();
    const auto& wordyPhrases = GetWords()->get_known_phrases().get_phrases();
    for (const auto& wordyIndex : wordyIndices)
        {
        switch (wordyPhrases[wordyIndex.second].first.get_type())
            {
        case grammar::phrase_type::phrase_wordy:
            ++m_wordyPhraseCount;
            break;
        case grammar::phrase_type::phrase_redundant:
            ++m_redundantPhraseCount;
            break;
        case grammar::phrase_type::phrase_cliche:
            ++m_clicheCount;
            break;
        case grammar::phrase_type::phrase_error:
            ++m_wordingErrorCount;
            break;
            };
        }

    m_sentenceStartingWithConjunctionsCount =
        GetWords()->get_conjunction_beginning_sentences().size();
    m_sentenceStartingWithLowercaseCount = GetWords()->get_lowercase_beginning_sentences().size();

    ReviewStatGoals();
    }

//------------------------------------------------
std::pair<bool, std::wstring>
BaseProject::ExtractRawTextWithEncoding(const std::wstring& sourceFileText,
                                        const wxString& fileExtension, const wxFileName& fileName,
                                        wxString& label)
    {
    grammar::convert_ligatures_and_diacritics convertDiacritics;
    lily_of_the_valley::html_extract_text filterHtml;
    filterHtml.set_log_message_separator(L", ");
    const WebPageExtension isHtmlExtension;

    try
        {
        if (isHtmlExtension(fileExtension.wc_str()))
            {
            std::wstring filteredText{ filterHtml(sourceFileText.c_str(), sourceFileText.length(),
                                                  true, false) };
            if (!filterHtml.get_log().empty())
                {
                wxLogWarning(L"%s: %s", fileName.GetFullPath(), filterHtml.get_log());
                }
            if (convertDiacritics(filteredText))
                {
                filteredText = convertDiacritics.get_conversion();
                }
            label = coalesce<wchar_t>({ filterHtml.get_subject(), filterHtml.get_title(),
                                        filterHtml.get_keywords(), filterHtml.get_description(),
                                        filterHtml.get_author() });
            return std::make_pair(true, std::move(filteredText));
            }

        return std::make_pair(true, sourceFileText);
        }
    catch (...)
        {
        wxLogWarning(L"An unknown error occurred while importing %s.", fileName.GetFullPath());
        return std::make_pair(false, std::wstring{});
        }
    }

//------------------------------------------------
std::pair<bool, std::wstring> BaseProject::ExtractDocxRawText(std::string_view sourceFileText)
    {
    grammar::convert_ligatures_and_diacritics convertDiacritics;
    lily_of_the_valley::word2007_extract_text filterDocx;
    filterDocx.set_log_message_separator(L", ");
    const Wisteria::ZipCatalog archive(sourceFileText.data(), sourceFileText.length());
    const std::wstring docxMetaFileText = archive.ReadTextFile(L"docProps/core.xml");
    if (!docxMetaFileText.empty())
        {
        filterDocx.read_meta_data(docxMetaFileText.c_str(), docxMetaFileText.length());
        SetOriginalDocumentDescription(
            coalesce<wchar_t>({ GetOriginalDocumentDescription().wc_str(), filterDocx.get_subject(),
                                filterDocx.get_title(), filterDocx.get_keywords(),
                                filterDocx.get_description(), filterDocx.get_author(),
                                wxFileName(GetOriginalDocumentFilePath()).GetName().wc_str() }));
        }
    if (archive.Find(L"word/document.xml") == nullptr)
        {
        LogMessage(_(L"Unable to open Word document, "
                     "file is either password-protected or corrupt."),
                   wxGetApp().GetAppDisplayName(), wxICON_EXCLAMATION | wxOK);
        return std::make_pair(false, std::wstring{});
        }

    const std::wstring docxFileText = archive.ReadTextFile(L"word/document.xml");
    try
        {
        filterDocx(docxFileText.c_str(), docxFileText.length());
        if (!filterDocx.get_log().empty())
            {
            wxLogWarning(L"%s: %s", GetOriginalDocumentFilePath(), filterDocx.get_log());
            }
        std::wstring extractedText = filterDocx.get_filtered_buffer();
        if (convertDiacritics(extractedText))
            {
            extractedText = convertDiacritics.get_conversion();
            }
        return std::make_pair(true, std::move(extractedText));
        }
    catch (...)
        {
        LogMessage(_(L"An unknown error occurred while importing. "
                     "Unable to continue creating project."),
                   _(L"Import Error"), wxOK | wxICON_EXCLAMATION);
        return std::make_pair(false, std::wstring{});
        }
    }

//------------------------------------------------
std::pair<bool, std::wstring> BaseProject::ExtractDocRawText(std::string_view sourceFileText)
    {
    try
        {
        grammar::convert_ligatures_and_diacritics convertDiacritics;
        lily_of_the_valley::word1997_extract_text filterWord; // Word 97-2003
        filterWord.set_log_message_separator(L", ");
        filterWord(sourceFileText.data(), sourceFileText.length());
        if (!filterWord.get_log().empty())
            {
            wxLogWarning(L"%s: %s", GetOriginalDocumentFilePath(), filterWord.get_log());
            }
        SetOriginalDocumentDescription(
            coalesce<wchar_t>({ GetOriginalDocumentDescription().wc_str(), filterWord.get_subject(),
                                filterWord.get_title(), filterWord.get_keywords(),
                                filterWord.get_comments(), filterWord.get_author(),
                                wxFileName(GetOriginalDocumentFilePath()).GetName().wc_str() }));
        std::wstring extractedText = filterWord.get_filtered_buffer();
        if (convertDiacritics(extractedText))
            {
            extractedText = convertDiacritics.get_conversion();
            }
        return std::make_pair(true, std::move(extractedText));
        }
    catch (const lily_of_the_valley::rtf_extract_text::rtfparse_exception&)
        {
        LogMessage(_(L"Invalid RTF file."), _(L"Import Error"), wxOK | wxICON_EXCLAMATION);
        return std::make_pair(false, std::wstring{});
        }
    catch (const lily_of_the_valley::word1997_extract_text::cfb_bad_bat&)
        {
        LogMessage(_(L"Word file is corrupted and cannot be read."), _(L"Import Error"),
                   wxOK | wxICON_EXCLAMATION);
        return std::make_pair(false, std::wstring{});
        }
    catch (const lily_of_the_valley::word1997_extract_text::cfb_bad_bat_entry&)
        {
        LogMessage(_(L"Word file is corrupted and cannot be read."), _(L"Import Error"),
                   wxOK | wxICON_EXCLAMATION);
        return std::make_pair(false, std::wstring{});
        }
    catch (const lily_of_the_valley::word1997_extract_text::cfb_bad_xbat&)
        {
        LogMessage(_(L"Word file is corrupted and cannot be read."), _(L"Import Error"),
                   wxOK | wxICON_EXCLAMATION);
        return std::make_pair(false, std::wstring{});
        }
    catch (const lily_of_the_valley::word1997_extract_text::cfb_bad_xbat_entry&)
        {
        LogMessage(_(L"Word file is corrupted and cannot be read."), _(L"Import Error"),
                   wxOK | wxICON_EXCLAMATION);
        return std::make_pair(false, std::wstring{});
        }
    catch (const lily_of_the_valley::word1997_extract_text::msword_encrypted&)
        {
        LogMessage(_(L"Word file is encrypted and cannot be read."), _(L"Import Error"),
                   wxOK | wxICON_EXCLAMATION);
        return std::make_pair(false, std::wstring{});
        }
    catch (const lily_of_the_valley::word1997_extract_text::msword_corrupted&)
        {
        LogMessage(_(L"Word file is corrupted and cannot be read."), _(L"Import Error"),
                   wxOK | wxICON_EXCLAMATION);
        return std::make_pair(false, std::wstring{});
        }
    catch (const lily_of_the_valley::word1997_extract_text::msword_fastsaved&)
        {
        LogMessage(_(L"Word file is \"fast-saved\" and could not be read.\n"
                     "Please save file with \"fast-saving\" turned off and try again."),
                   _(L"Import Error"), wxOK | wxICON_EXCLAMATION);
        return std::make_pair(false, std::wstring{});
        }
    catch (const lily_of_the_valley::word1997_extract_text::msword_header_not_found&)
        {
        LogMessage(_(L"File does not appear to be a valid Word file."), _(L"Import Error"),
                   wxOK | wxICON_EXCLAMATION);
        return std::make_pair(false, std::wstring{});
        }
    catch (const lily_of_the_valley::word1997_extract_text::msword_root_entry_not_found&)
        {
        LogMessage(_(L"File does not appear to be a valid Word file."), _(L"Import Error"),
                   wxOK | wxICON_EXCLAMATION);
        return std::make_pair(false, std::wstring{});
        }
    catch (...)
        {
        LogMessage(_(L"An unknown error occurred while importing. "
                     "Unable to continue creating project."),
                   _(L"Import Error"), wxOK | wxICON_EXCLAMATION);
        return std::make_pair(false, std::wstring{});
        }
    }

//------------------------------------------------
std::pair<bool, std::wstring> BaseProject::ExtractRtfRawText(std::string_view sourceFileText)
    {
    try
        {
        grammar::convert_ligatures_and_diacritics convertDiacritics;
        lily_of_the_valley::rtf_extract_text filterRtf;
        filterRtf.set_log_message_separator(L", ");
        filterRtf(sourceFileText.data(), sourceFileText.length());
        if (!filterRtf.get_log().empty())
            {
            wxLogWarning(L"%s: %s", GetOriginalDocumentFilePath(), filterRtf.get_log());
            }
        SetOriginalDocumentDescription(
            coalesce<wchar_t>({ GetOriginalDocumentDescription().wc_str(), filterRtf.get_subject(),
                                filterRtf.get_title(), filterRtf.get_keywords(),
                                filterRtf.get_comments(), filterRtf.get_author(),
                                wxFileName(GetOriginalDocumentFilePath()).GetName().wc_str() }));
        std::wstring extractedText = filterRtf.get_filtered_buffer();
        if (convertDiacritics(extractedText))
            {
            extractedText = convertDiacritics.get_conversion();
            }
        return std::make_pair(true, std::move(extractedText));
        }
    catch (const lily_of_the_valley::rtf_extract_text::rtfparse_stack_underflow&)
        {
        LogMessage(_(L"Unable to import RTF file. Stack underflow."), _(L"Import Error"),
                   wxOK | wxICON_EXCLAMATION);
        return std::make_pair(false, std::wstring{});
        }
    catch (const lily_of_the_valley::rtf_extract_text::rtfparse_stack_overflow&)
        {
        LogMessage(_(L"Unable to import RTF file. Stack overflow."), _(L"Import Error"),
                   wxOK | wxICON_EXCLAMATION);
        return std::make_pair(false, std::wstring{});
        }
    catch (const lily_of_the_valley::rtf_extract_text::rtfparse_unmatched_brace&)
        {
        LogMessage(_(L"Unable to import RTF file. Unmatched brace."), _(L"Import Error"),
                   wxOK | wxICON_EXCLAMATION);
        return std::make_pair(false, std::wstring{});
        }
    catch (const lily_of_the_valley::rtf_extract_text::rtfparse_assertion&)
        {
        LogMessage(_(L"Unable to import RTF file. Assertion."), _(L"Import Error"),
                   wxOK | wxICON_EXCLAMATION);
        return std::make_pair(false, std::wstring{});
        }
    catch (const lily_of_the_valley::rtf_extract_text::rtfparse_invalid_hex&)
        {
        LogMessage(_(L"Unable to import RTF file. Invalid hex value."), _(L"Import Error"),
                   wxOK | wxICON_EXCLAMATION);
        return std::make_pair(false, std::wstring{});
        }
    catch (const lily_of_the_valley::rtf_extract_text::rtfparse_bad_table&)
        {
        LogMessage(_(L"Unable to import RTF file. Bad table."), _(L"Import Error"),
                   wxOK | wxICON_EXCLAMATION);
        return std::make_pair(false, std::wstring{});
        }
    catch (...)
        {
        LogMessage(_(L"An unknown error occurred while importing. "
                     "Unable to continue creating project."),
                   _(L"Import Error"), wxOK | wxICON_EXCLAMATION);
        return std::make_pair(false, std::wstring{});
        }
    }

//------------------------------------------------
std::pair<bool, std::wstring> BaseProject::ExtractPostscriptRawText(std::string_view sourceFileText)
    {
    lily_of_the_valley::postscript_extract_text filterPs;
    filterPs.set_log_message_separator(L", ");
    try
        {
        filterPs(sourceFileText.data(), sourceFileText.length());
        if (!filterPs.get_log().empty())
            {
            wxLogWarning(L"%s: %s", GetOriginalDocumentFilePath(), filterPs.get_log());
            }
        SetOriginalDocumentDescription(
            coalesce({ GetOriginalDocumentDescription(), wxString(filterPs.get_title()),
                       wxFileName(GetOriginalDocumentFilePath()).GetName() }));
        return std::make_pair(true, std::move(filterPs.get_filtered_buffer()));
        }
    catch (const lily_of_the_valley::postscript_extract_text::postscript_header_not_found&)
        {
        LogMessage(_(L"File does not appear to be a valid Postscript file."), _(L"Import Error"),
                   wxOK | wxICON_EXCLAMATION);
        return std::make_pair(false, std::wstring{});
        }
    catch (const lily_of_the_valley::postscript_extract_text::postscript_version_not_supported&)
        {
        LogMessage(_(L"Only PostScript versions 1-2 are supported. "
                     "Unable to import file."),
                   _(L"Import Error"), wxOK | wxICON_EXCLAMATION);
        return std::make_pair(false, std::wstring{});
        }
    catch (...)
        {
        LogMessage(_(L"An unknown error occurred while importing. "
                     "Unable to continue creating project."),
                   _(L"Import Error"), wxOK | wxICON_EXCLAMATION);
        return std::make_pair(false, std::wstring{});
        }
    }

//------------------------------------------------
std::pair<bool, std::wstring>
BaseProject::ExtractOpenDocumentRawText(std::string_view sourceFileText)
    {
    grammar::convert_ligatures_and_diacritics convertDiacritics;
    lily_of_the_valley::odt_odp_extract_text filterOdt;
    filterOdt.set_log_message_separator(L", ");
    const Wisteria::ZipCatalog archive(sourceFileText.data(), sourceFileText.length());
    const std::wstring odtMetaFileText = archive.ReadTextFile(L"meta.xml");
    if (!odtMetaFileText.empty())
        {
        filterOdt.read_meta_data(odtMetaFileText.c_str(), odtMetaFileText.length());
        SetOriginalDocumentDescription(
            coalesce<wchar_t>({ GetOriginalDocumentDescription().wc_str(), filterOdt.get_subject(),
                                filterOdt.get_title(), filterOdt.get_keywords(),
                                filterOdt.get_description(), filterOdt.get_author(),
                                wxFileName(GetOriginalDocumentFilePath()).GetName().wc_str() }));
        }
    const std::wstring odtFileText = archive.ReadTextFile(L"content.xml");
    if (odtFileText.empty() && !archive.GetMessages().empty())
        {
        LogMessage(wxString::Format(_(L"Unable to open ODT/ODP document: %s"),
                                    archive.GetMessages().at(0).m_message),
                   wxGetApp().GetAppDisplayName(), wxICON_EXCLAMATION | wxOK);
        return std::make_pair(false, std::wstring{});
        }
    try
        {
        filterOdt(odtFileText.c_str(), odtFileText.length());
        if (!filterOdt.get_log().empty())
            {
            wxLogWarning(L"%s: %s", GetOriginalDocumentFilePath(), filterOdt.get_log());
            }
        std::wstring extractedText = filterOdt.get_filtered_buffer();
        if (convertDiacritics(extractedText))
            {
            extractedText = convertDiacritics.get_conversion();
            }
        return std::make_pair(true, std::move(extractedText));
        }
    catch (...)
        {
        LogMessage(_(L"An unknown error occurred while importing. "
                     "Unable to continue creating project."),
                   _(L"Import Error"), wxOK | wxICON_EXCLAMATION);
        return std::make_pair(false, std::wstring{});
        }
    }

//------------------------------------------------
std::pair<bool, std::wstring>
BaseProject::ExtractHtmlRawText(std::string_view sourceFileText,
                                const WebPageExtension& isHtmlExtension,
                                const wxString& fileExtension)
    {
    if (isHtmlExtension.IsDynamicExtension(fileExtension))
        {
        const size_t bomStartLength =
            (utf8::starts_with_bom(sourceFileText.data(),
                                   sourceFileText.data() + sourceFileText.length()) ?
                 3 :
                 0);
        size_t firstCharIndex{ bomStartLength };
        while (firstCharIndex < sourceFileText.length() &&
               characters::is_character::is_space(sourceFileText[firstCharIndex]))
            {
            ++firstCharIndex;
            }
        // See if it's not HTML (based on if first character is a '<').
        if (firstCharIndex >= sourceFileText.length() || sourceFileText[firstCharIndex] != '<')
            {
            wxLogVerbose(L"Dynamic webpage is not valid HTML. Document will not be loaded.");
            return std::make_pair(false, std::wstring{});
            }
        }
    std::pair<bool, std::wstring> extractResult(false, std::wstring{});
    wxString label;
    // if UTF-8 or simply 7-bit ASCII, then just convert as UTF-8 and run the HTML parser on it
    if (utf8::is_valid(sourceFileText.data(), sourceFileText.data() + sourceFileText.length()))
        {
        extractResult =
            ExtractRawTextWithEncoding(Wisteria::TextStream::CharStreamToUnicode(
                                           sourceFileText.data(), sourceFileText.length()),
                                       L"html", GetOriginalDocumentFilePath(), label);
        }
    // Otherwise, need to search for the encoding in the HTML itself and convert using that,
    // then run the HTML parser on it
    else
        {
        extractResult =
            ExtractRawTextWithEncoding(Wisteria::TextStream::CharStreamToUnicode(
                                           sourceFileText.data(), sourceFileText.length(),
                                           WebHarvester::GetCharsetFromPageContent(
                                               { sourceFileText.data(), sourceFileText.length() })),
                                       L"html", GetOriginalDocumentFilePath(), label);
        }
    if (!extractResult.first)
        {
        LogMessage(_(L"An unknown error occurred while importing. "
                     "Unable to continue creating project."),
                   _(L"Import Error"), wxOK | wxICON_EXCLAMATION);
        return std::make_pair(false, std::wstring{});
        }
    // set the description to the title
    // (unless user specified a description already, e.g., from a batch project).
    SetOriginalDocumentDescription(
        coalesce({ GetOriginalDocumentDescription(), label,
                   wxFileName(GetOriginalDocumentFilePath()).GetName() }));

    // note that ExtractRawTextWithEncoding() handled the diacritic/ligature conversions already
    return std::make_pair(true, std::move(extractResult.second));
    }

//------------------------------------------------
std::pair<bool, std::wstring> BaseProject::ExtractPowerPointRawText(std::string_view sourceFileText)
    {
    grammar::convert_ligatures_and_diacritics convertDiacritics;
    lily_of_the_valley::pptx_extract_text filterPptx;
    filterPptx.set_log_message_separator(L", ");
    std::wstring pptParsedText;
    const Wisteria::ZipCatalog archive(sourceFileText.data(), sourceFileText.length());
    const std::wstring pptMetaFileText = archive.ReadTextFile(L"docProps/core.xml");
    if (!pptMetaFileText.empty())
        {
        filterPptx.read_meta_data(pptMetaFileText.c_str(), pptMetaFileText.length());
        SetOriginalDocumentDescription(
            coalesce<wchar_t>({ GetOriginalDocumentDescription().wc_str(), filterPptx.get_subject(),
                                filterPptx.get_title(), filterPptx.get_keywords(),
                                filterPptx.get_description(), filterPptx.get_author(),
                                wxFileName(GetOriginalDocumentFilePath()).GetName().wc_str() }));
        }
    if (archive.Find(L"ppt/slides/slide1.xml") == nullptr)
        {
        LogMessage(_(L"Unable to open PowerPoint document; "
                     "file is either password-protected or corrupt."),
                   wxGetApp().GetAppDisplayName(), wxICON_EXCLAMATION | wxOK);
        return std::make_pair(false, std::wstring{});
        }
    for (size_t i = 1; /*breaks when no more pages are found*/; ++i)
        {
        if (archive.Find(wxString::Format(L"ppt/slides/slide%zu.xml", i)) != nullptr)
            {
            const std::wstring pptxFileText =
                archive.ReadTextFile(wxString::Format(L"ppt/slides/slide%zu.xml", i));
            try
                {
                pptParsedText += filterPptx(pptxFileText.c_str(), pptxFileText.length());
                pptParsedText += L"\n";
                if (!filterPptx.get_log().empty())
                    {
                    wxLogWarning(L"%s: %s", GetOriginalDocumentFilePath(), filterPptx.get_log());
                    }
                }
            catch (...)
                {
                LogMessage(_(L"An unknown error occurred while importing. "
                             "Unable to continue creating project."),
                           _(L"Import Error"), wxOK | wxICON_EXCLAMATION);
                return std::make_pair(false, std::wstring{});
                }
            }
        else
            {
            break;
            }
        }

    if (convertDiacritics(pptParsedText))
        {
        pptParsedText = convertDiacritics.get_conversion();
        }
    return std::make_pair(true, std::move(pptParsedText));
    }

//------------------------------------------------
std::pair<bool, std::wstring> BaseProject::ExtractWorkshopRawText(std::string_view sourceFileText)
    {
    try
        {
        grammar::convert_ligatures_and_diacritics convertDiacritics;
        lily_of_the_valley::hhc_hhk_extract_text filterHhcHhk;
        filterHhcHhk.set_log_message_separator(L", ");
        const std::wstring hhStr = Wisteria::TextStream::CharStreamToUnicode(
            sourceFileText.data(), sourceFileText.length());
        filterHhcHhk(hhStr.c_str(), hhStr.length());
        if (!filterHhcHhk.get_log().empty())
            {
            wxLogWarning(L"%s: %s", GetOriginalDocumentFilePath(), filterHhcHhk.get_log());
            }
        SetOriginalDocumentDescription(
            coalesce({ GetOriginalDocumentDescription(),
                       wxFileName(GetOriginalDocumentFilePath()).GetName() }));
        std::wstring extractedText = filterHhcHhk.get_filtered_buffer();
        if (convertDiacritics(extractedText))
            {
            extractedText = convertDiacritics.get_conversion();
            }
        return std::make_pair(true, std::move(extractedText));
        }
    catch (...)
        {
        LogMessage(_(L"An unknown error occurred while importing. "
                     "Unable to continue creating project."),
                   _(L"Import Error"), wxOK | wxICON_EXCLAMATION);
        return std::make_pair(false, std::wstring{});
        }
    }

//------------------------------------------------
std::pair<bool, std::wstring> BaseProject::ExtractIdlRawText(std::string_view sourceFileText)
    {
    // override user settings, we would want to ignore code here
    SpellCheckIgnoreProgrammerCode(true);
    grammar::convert_ligatures_and_diacritics convertDiacritics;
    const std::wstring unicodeStr =
        Wisteria::TextStream::CharStreamToUnicode(sourceFileText.data(), sourceFileText.length());
    SetOriginalDocumentDescription(coalesce(
        { GetOriginalDocumentDescription(), wxFileName(GetOriginalDocumentFilePath()).GetName() }));
    lily_of_the_valley::idl_extract_text filterIdl;
    filterIdl({ unicodeStr.c_str(), unicodeStr.length() });

    std::wstring extractedText = filterIdl.get_filtered_buffer();
    if (convertDiacritics(extractedText))
        {
        extractedText = convertDiacritics.get_conversion();
        }
    return std::make_pair(true, std::move(extractedText));
    }

//------------------------------------------------
std::pair<bool, std::wstring> BaseProject::ExtractCppRawText(std::string_view sourceFileText)
    {
    SpellCheckIgnoreProgrammerCode(true);
    grammar::convert_ligatures_and_diacritics convertDiacritics;
    lily_of_the_valley::cpp_extract_text filterCpp;
    filterCpp.include_all_comments(true);
    const std::wstring unicodeStr =
        Wisteria::TextStream::CharStreamToUnicode(sourceFileText.data(), sourceFileText.length());
    SetOriginalDocumentDescription(
        coalesce<wchar_t>({ GetOriginalDocumentDescription().wc_str(), filterCpp.get_author(),
                            wxFileName(GetOriginalDocumentFilePath()).GetName().wc_str() }));
    filterCpp(unicodeStr.c_str(), unicodeStr.length());

    std::wstring extractedText = filterCpp.get_filtered_buffer();
    if (convertDiacritics(extractedText))
        {
        extractedText = convertDiacritics.get_conversion();
        }
    return std::make_pair(true, std::move(extractedText));
    }

//------------------------------------------------
std::pair<bool, std::wstring> BaseProject::ExtractMarkdownRawText(std::string_view sourceFileText)
    {
    grammar::convert_ligatures_and_diacritics convertDiacritics;
    const std::wstring unicodeStr =
        Wisteria::TextStream::CharStreamToUnicode(sourceFileText.data(), sourceFileText.length());
    SetOriginalDocumentDescription(coalesce(
        { GetOriginalDocumentDescription(), wxFileName(GetOriginalDocumentFilePath()).GetName() }));
    lily_of_the_valley::markdown_extract_text filterMd;
    filterMd({ unicodeStr.c_str(), unicodeStr.length() });

    std::wstring extractedText = filterMd.get_filtered_buffer();
    if (convertDiacritics(extractedText))
        {
        extractedText = convertDiacritics.get_conversion();
        }
    return std::make_pair(true, std::move(extractedText));
    }

//------------------------------------------------
std::pair<bool, std::wstring> BaseProject::ExtractRawText(std::string_view sourceFileText,
                                                          const wxString& fileExtension)
    {
    PROFILE();
    if (sourceFileText.empty())
        {
        return std::make_pair(false, std::wstring{});
        }
    wxLogVerbose(L"%s parser being called.", fileExtension.Upper());

    const WebPageExtension isHtmlExtension;

    if (fileExtension.CmpNoCase(L"rtf") == 0)
        {
        return ExtractRtfRawText(sourceFileText);
        }
    if (fileExtension.CmpNoCase(L"doc") == 0 || fileExtension.CmpNoCase(L"dot") == 0)
        {
        return ExtractDocRawText(sourceFileText);
        }
    if (fileExtension.CmpNoCase(L"docx") == 0 || fileExtension.CmpNoCase(L"docm") == 0)
        {
        return ExtractDocxRawText(sourceFileText);
        }
    if (fileExtension.CmpNoCase(L"hhc") == 0 || fileExtension.CmpNoCase(L"hhk") == 0)
        {
        return ExtractWorkshopRawText(sourceFileText);
        }
    if (isHtmlExtension(fileExtension))
        {
        return ExtractHtmlRawText(sourceFileText, isHtmlExtension, fileExtension);
        }
    if (fileExtension.CmpNoCase(L"ps") == 0)
        {
        return ExtractPostscriptRawText(sourceFileText);
        }
    // OpenDocument text or presentation files
    if (fileExtension.CmpNoCase(L"odt") == 0 || fileExtension.CmpNoCase(L"ott") == 0 ||
        fileExtension.CmpNoCase(L"odp") == 0 || fileExtension.CmpNoCase(L"otp") == 0)
        {
        return ExtractOpenDocumentRawText(sourceFileText);
        }
    if (fileExtension.CmpNoCase(L"pptx") == 0 || fileExtension.CmpNoCase(L"pptm") == 0)
        {
        return ExtractPowerPointRawText(sourceFileText);
        }
    if (fileExtension.CmpNoCase(L"idl") == 0)
        {
        return ExtractIdlRawText(sourceFileText);
        }
    if (fileExtension.CmpNoCase(L"cpp") == 0 || fileExtension.CmpNoCase(L"c") == 0 ||
        fileExtension.CmpNoCase(L"h") == 0)
        {
        return ExtractCppRawText(sourceFileText);
        }
    if (fileExtension.CmpNoCase(L"md") == 0 || fileExtension.CmpNoCase(L"rmd") == 0 ||
        fileExtension.CmpNoCase(L"qmd") == 0)
        {
        return ExtractMarkdownRawText(sourceFileText);
        }
    if (fileExtension.CmpNoCase(L"txt") == 0)
        {
        SetOriginalDocumentDescription(
            coalesce({ GetOriginalDocumentDescription(),
                       wxFileName(GetOriginalDocumentFilePath()).GetName() }));
        std::wstring extractedText = Wisteria::TextStream::CharStreamToUnicode(
            sourceFileText.data(), sourceFileText.length());
        grammar::convert_ligatures_and_diacritics convertDiacritics;
        if (convertDiacritics(extractedText))
            {
            extractedText = convertDiacritics.get_conversion();
            }
        return std::make_pair(true, std::move(extractedText));
        }
    if (fileExtension.CmpNoCase(L"pdf") == 0)
        {
        LogMessage(_(L"PDF files are not supported."), _(L"Import Error"),
                   wxOK | wxICON_EXCLAMATION);
        return std::make_pair(false, std::wstring{});
        }
    // Unknown (or no) extension

    // See if it is HTML, given that a lot of web pages may not use a known file extension
    if (string_util::stristr(sourceFileText.data(), "<html") != nullptr)
        {
        std::pair<bool, std::wstring> extractResult(false, std::wstring{});
        wxString title;
        // if UTF-8 or simply 7-bit ASCII, then just convert as UTF-8 and
        // run the HTML parser on it
        if (utf8::is_valid(sourceFileText.data(), sourceFileText.data() + sourceFileText.length()))
            {
            extractResult =
                ExtractRawTextWithEncoding(Wisteria::TextStream::CharStreamToUnicode(
                                               sourceFileText.data(), sourceFileText.length()),
                                           L"html", GetOriginalDocumentFilePath(), title);
            }
        // Otherwise, need to search for the encoding in the HTML itself and convert using that,
        // then run the HTML parser on it
        else
            {
            const std::wstring str = Wisteria::TextStream::CharStreamToUnicode(
                sourceFileText.data(), sourceFileText.length(),
                WebHarvester::GetCharsetFromPageContent(
                    { sourceFileText.data(), sourceFileText.length() }));
            extractResult =
                ExtractRawTextWithEncoding(str, L"html", GetOriginalDocumentFilePath(), title);
            }
        SetOriginalDocumentDescription(
            coalesce({ GetOriginalDocumentDescription(), title,
                       wxFileName(GetOriginalDocumentFilePath()).GetName() }));
        return std::make_pair(true, std::move(extractResult.second));
        }
    // ...otherwise, just load it as regular text

    // This should be logged instead of shown to the user;
    // otherwise, they will see this warning every time they refresh.
    wxLogWarning(L"Unknown file extension. File will be imported as plain text.");
    return std::make_pair(true, Wisteria::TextStream::CharStreamToUnicode(sourceFileText.data(),
                                                                          sourceFileText.length()));
    }

//------------------------------------------------
bool BaseProject::LoadExternalDocument()
    {
    FilePathResolver resolvePath;
    // This will "fix" the file path in case it has "file:///". Also fixes slash problem,
    // appends "http" if it just says "www", etc.
    SetOriginalDocumentFilePath(
        resolvePath.ResolvePath(GetOriginalDocumentFilePath(), true, { GetProjectDirectory() }));
    if (resolvePath.IsHTTPFile() || resolvePath.IsHTTPSFile())
        {
        wxString content, contentType, statusText;
        int responseCode{ 404 };
        wxString urlPath = GetOriginalDocumentFilePath();
        std::pair<bool, std::wstring> extractResult;
        wxString title;

        if (WebHarvester::IsOneDriveDocument(GetOriginalDocumentFilePath()))
            {
            if (wxGetApp().GetWebHarvester().ReadWebDocument(urlPath, statusText, responseCode))
                {
                extractResult = ExtractRawText(
                    std::string_view{
                        wxGetApp().GetWebHarvester().GetDownloader().GetLastRead().data(),
                        wxGetApp().GetWebHarvester().GetDownloader().GetLastRead().size() },
                    wxFileName{
                        wxGetApp().GetWebHarvester().GetDownloader().GetLastOneDriveFileName() }
                        .GetExt());
                title = wxGetApp().GetWebHarvester().GetDownloader().GetLastOneDriveFileName();
                }
            else
                {
                LogMessage(wxString::Format(
                               _(L"Unable to open webpage. The following error occurred:\n%s\n%s"),
                               QueueDownload::GetResponseMessage(responseCode), statusText),
                           _(L"Error"), wxOK | wxICON_EXCLAMATION);
                return false;
                }
            }
        else if (wxGetApp().GetWebHarvester().ReadWebPage(urlPath, content, contentType, statusText,
                                                          responseCode, false))
            {
            extractResult = ExtractRawTextWithEncoding(
                content.wc_string(), WebHarvester::GetFileTypeFromContentType(contentType),
                GetOriginalDocumentFilePath(), title);
            }
        else
            {
            LogMessage(wxString::Format(
                           _(L"Unable to open webpage. The following error occurred:\n%s\n%s"),
                           QueueDownload::GetResponseMessage(responseCode), statusText),
                       _(L"Error"), wxOK | wxICON_EXCLAMATION);
            return false;
            }

        // load whatever webpage or document that we retrieved into the indexing engine
        if (!extractResult.first)
            {
            return false;
            }
        if (GetOriginalDocumentDescription().empty())
            {
            SetOriginalDocumentDescription(title);
            }

        try
            {
            SetDocumentText(std::move(extractResult.second));
            LoadDocument();
            }
        catch (...)
            {
            LogMessage(_(L"An unknown error occurred while analyzing the document. "
                         "Unable to create project."),
                       _(L"Import Error"), wxOK | wxICON_EXCLAMATION);
            return false;
            }
        // set the name of the project from the title of the page
        string_util::remove_extra_spaces(title);
        title = StripIllegalFileCharacters(title);
        title.Replace(L".", wxString{}, true);
        SetDocumentTitle(title);
        }
    else if (resolvePath.IsInvalidFile())
        {
        LogMessage(_(L"Invalid file specified."), _(L"Invalid File"), wxOK | wxICON_EXCLAMATION);
        return false;
        }
    else if (resolvePath.IsLocalOrNetworkFile())
        {
        // make sure the file exists first
        if (!wxFile::Exists(GetOriginalDocumentFilePath()))
            {
            // first try to find the document from the project file's folder structure,
            // then ask the user to search for it manually
            wxString fileBySameNameInProjectDirectory;
            if (FindMissingFile(GetOriginalDocumentFilePath(), fileBySameNameInProjectDirectory))
                {
                SetOriginalDocumentFilePath(fileBySameNameInProjectDirectory);
                }

            if (!wxFile::Exists(GetOriginalDocumentFilePath()))
                {
                if (wxMessageBox(wxString::Format(_(L"%s:\n\nDocument could not be located. "
                                                    "Do you wish to search for it?"),
                                                  GetOriginalDocumentFilePath()),
                                 _(L"Document Not Found"), wxYES_NO | wxICON_QUESTION) == wxYES)
                    {
                    const wxFileName fn(StripIllegalFileCharacters(GetOriginalDocumentFilePath()));
                    wxFileDialog dialog(nullptr, _(L"Select Document to Analyze"), fn.GetPath(),
                                        fn.GetName(), fn.GetFullName(),
                                        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);

                    if (dialog.ShowModal() != wxID_OK)
                        {
                        return false;
                        }

                    SetOriginalDocumentFilePath(dialog.GetPath());
                    SetModifiedFlag();
                    }
                else
                    {
                    // Give it one last try. If it is set to embedded (e.g., from a parent Batch
                    // project), then try to load the embedded text.
                    if (GetDocumentStorageMethod() == TextStorage::EmbedText)
                        {
                        // there is embedded text (that may have been passed from a batch project),
                        // so load that here.
                        if (!GetDocumentText().empty())
                            {
                            LoadDocument();
                            return true;
                            }
                        // otherwise, fail.

                        if (WarningManager::HasWarning(_DT(L"no-embedded-text")))
                            {
                            LogMessage(*WarningManager::GetWarning(_DT(L"no-embedded-text")));
                            }
                        return false;
                        }

                    return false;
                    }
                }
            }

        // read in the text from the file
        try
            {
            MemoryMappedFile sourceFile(GetOriginalDocumentFilePath(), true, true);
            // NOLINTBEGIN(misc-const-correctness)
            std::pair<bool, std::wstring> extractResult = ExtractRawText(
                { static_cast<const char*>(sourceFile.GetStream()), sourceFile.GetMapSize() },
                wxFileName(GetOriginalDocumentFilePath()).GetExt());
            // NOLINTEND(misc-const-correctness)
            if (extractResult.first)
                {
                SetDocumentText(std::move(extractResult.second));
                try
                    {
                    LoadDocument();
                    }
                catch (...)
                    {
                    LogMessage(_(L"An unknown error occurred while analyzing the document. "
                                 "Unable to create project."),
                               _(L"Error"), wxOK | wxICON_EXCLAMATION);
                    return false;
                    }
                return true;
                }

            return false;
            }
        // weird exception that auto-buffering won't help, so explain it to the user
        catch (const MemoryMappedFileCloudFileError&)
            {
            LogMessage(wxString::Format(_(L"%s:\n\nUnable to open file from Cloud service."),
                                        GetOriginalDocumentFilePath()),
                       _(L"Error"), wxOK | wxICON_EXCLAMATION);
            return false;
            }
        catch (const std::exception& exp)
            {
            LogMessage(
                wxString::Format(L"%s:\n\n%s", GetOriginalDocumentFilePath(), wxString(exp.what())),
                _(L"Error"), wxOK | wxICON_EXCLAMATION);
            return false;
            }
        catch (...)
            {
            LogMessage(wxString::Format(
                           _(L"%s:\n\nAn unknown error occurred while analyzing the document. "
                             "Unable to create project."),
                           GetOriginalDocumentFilePath()),
                       _(L"Error"), wxOK | wxICON_EXCLAMATION);
            return false;
            }
        }
    // if a file is in an archive then extract it and analyze it
    else if (resolvePath.IsArchivedFile())
        {
        const size_t poundInFile = GetOriginalDocumentFilePath().Lower().find(_DT(L".zip#"));
        wxFileName poundFn(GetOriginalDocumentFilePath().substr(0, poundInFile + 4));
        if (!wxFile::Exists(poundFn.GetFullPath()))
            {
            // first try to find the document from the project file's folder structure
            wxString fileBySameNameInProjectDirectory;
            if (FindMissingFile(poundFn.GetFullPath(), fileBySameNameInProjectDirectory))
                {
                poundFn.Assign(fileBySameNameInProjectDirectory);
                }
            else
                {
                LogMessage(
                    wxString::Format(_(L"%s:\n\nUnable to open file."), poundFn.GetFullPath()),
                    _(L"Error"), wxOK | wxICON_EXCLAMATION);
                return false;
                }
            }
        const Wisteria::ZipCatalog zc(poundFn.GetFullPath());
        wxMemoryOutputStream memstream;
        if (!zc.ReadFile(GetOriginalDocumentFilePath().substr(poundInFile + 5), memstream) &&
            !zc.GetMessages().empty())
            {
            LogMessage(zc.GetMessages().back().m_message, poundFn.GetFullPath(),
                       zc.GetMessages().back().m_icon);
            }
        // NOLINTBEGIN(misc-const-correctness)
        std::pair<bool, std::wstring> extractResult = ExtractRawText(
            { static_cast<const char*>(memstream.GetOutputStreamBuffer()->GetBufferStart()),
              static_cast<size_t>(memstream.GetLength()) },
            wxFileName(GetOriginalDocumentFilePath()).GetExt());
        // NOLINTEND(misc-const-correctness)
        if (extractResult.first)
            {
            SetDocumentText(std::move(extractResult.second));
            try
                {
                LoadDocument();
                }
            catch (...)
                {
                LogMessage(_(L"An unknown error occurred while analyzing the document. "
                             "Unable to create project."),
                           _(L"Error"), wxOK | wxICON_EXCLAMATION);
                return false;
                }
            return true;
            }

        return false;
        }
    // or a cell in an Excel file
    else if (resolvePath.IsExcelCell())
        {
        const size_t excelTag = GetOriginalDocumentFilePath().Lower().find(_DT(L".xlsx#"));
        wxFileName poundFn(GetOriginalDocumentFilePath().substr(0, excelTag + 5));
        if (!wxFile::Exists(poundFn.GetFullPath()))
            {
            // first try to find the document from the project file's folder structure
            wxString fileBySameNameInProjectDirectory;
            if (FindMissingFile(poundFn.GetFullPath(), fileBySameNameInProjectDirectory))
                {
                poundFn.Assign(fileBySameNameInProjectDirectory);
                }
            else
                {
                LogMessage(
                    wxString::Format(_(L"%s:\n\nUnable to open file."), poundFn.GetFullPath()),
                    _(L"Error"), wxOK | wxICON_EXCLAMATION);
                return false;
                }
            }
        wxString worksheetName = GetOriginalDocumentFilePath().substr(excelTag + 6);
        const size_t slash = worksheetName.find_last_of(L'#');
        if (slash != wxString::npos)
            {
            const wxString cellName = worksheetName.substr(slash + 1);
            worksheetName.Truncate(slash);
            lily_of_the_valley::xlsx_extract_text filterXlsx{ false };
            const Wisteria::ZipCatalog zc(poundFn.GetFullPath());
            // read in the worksheets
            const std::wstring workBookFileText = zc.ReadTextFile(L"xl/workbook.xml");
            filterXlsx.read_worksheet_names(workBookFileText.c_str(), workBookFileText.length());
            // read workbook relationships
            const std::wstring workbookRels = zc.ReadTextFile(L"xl/_rels/workbook.xml.rels");
            filterXlsx.read_relative_paths(workbookRels.c_str(), workbookRels.length());
            // resolve worksheet names to XML paths
            filterXlsx.map_workbook_paths();
            // read in the string table
            const std::wstring sharedStrings = zc.ReadTextFile(L"xl/sharedStrings.xml");
            if (sharedStrings.empty())
                {
                return false;
                }
            // find the sheet to get the cells from
            const auto& worksheetPaths = filterXlsx.get_worksheet_paths();
            const auto sheetPos =
                std::ranges::find_if(worksheetPaths, [&](const auto& wsPath)
                                     { return wsPath.first == worksheetName.wc_string(); });
            if (sheetPos != worksheetPaths.end())
                {
                const std::wstring sheetFile = zc.ReadTextFile(sheetPos->second);
                SetDocumentText(filterXlsx.get_cell_text(cellName.wc_str(), sharedStrings.c_str(),
                                                         sharedStrings.length(), sheetFile.c_str(),
                                                         sheetFile.length()));
                LoadDocument();
                return true;
                }

            LogMessage(wxString::Format(_(L"Unable to find the worksheet \"%s\" in \"%s\"."),
                                        worksheetName, poundFn.GetFullPath()),
                       _(L"Error"), wxOK | wxICON_EXCLAMATION);
            return false;
            }
        }

    return true;
    }

//-------------------------------------------------------
bool BaseProject::LoadDocumentAsSubProject(const wxString& path, const std::wstring& text,
                                           const size_t minWordCount)
    {
    if (!path.empty())
        {
        wxLogMessage(L"Analyzing %s", path);
        }
    SetOriginalDocumentFilePath(path);
    SetDocumentStorageMethod(text.empty() ? TextStorage::NoEmbedText : TextStorage::EmbedText);

    DeleteUniqueWordMap();
    ClearSubProjectMessages();
    CreateWords();

    if (GetDocumentStorageMethod() == TextStorage::NoEmbedText)
        {
        if (!LoadExternalDocument())
            {
            SetLoadingOriginalTextSucceeded(false);
            return false;
            }
        }
    else
        {
        SetDocumentText(text);
        if (text.empty())
            {
            SetLoadingOriginalTextSucceeded(false);
            return false;
            }
        try
            {
            LoadDocument();
            }
        catch (...)
            {
            LogMessage(_(L"An unknown error occurred while analyzing the document. "
                         "Unable to create project."),
                       _(L"Error"), wxOK | wxICON_EXCLAMATION);
            SetLoadingOriginalTextSucceeded(false);
            return false;
            }
        }

    if (GetWords() == nullptr)
        {
        LogMessage(_(L"An unknown error occurred while loading the document."), _(L"Error"),
                   wxOK | wxICON_EXCLAMATION);
        SetLoadingOriginalTextSucceeded(false);
        return false;
        }

    /** See if there is an inordinate amount of titles/headers/bullets, and if they
        are asking to exclude these then make sure they understand that a large part of the
        document will be ignored.
        Note that we don't bother with this check with webpages because they normally
        contain lists for things like menus that we would indeed want to ignore.*/
    const FilePathResolver resolvePath(GetOriginalDocumentFilePath(), false);
    if (GetWords()->get_sentence_count() > 0 && !resolvePath.IsWebFile())
        {
        /* if document is nothing but valid sentences then it is OK.*/
        if (GetWords()->get_sentence_count() == GetWords()->get_complete_sentence_count())
            {
            // NOOP
            }
        else
            {
            const auto numberOfInvalidSentencesPercentage = safe_divide<double>(
                (GetWords()->get_sentence_count() - GetWords()->get_complete_sentence_count()),
                GetWords()->get_sentence_count());
            if (numberOfInvalidSentencesPercentage > 0.60 &&
                (GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis ||
                 GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings))
                {
                if (WarningManager::HasWarning(_DT(L"high-count-sentences-being-ignored")))
                    {
                    WarningMessage warningMsg =
                        *WarningManager::GetWarning(_DT(L"high-count-sentences-being-ignored"));
                    warningMsg.SetMessage(
                        _(L"This document contains a large percentage of incomplete sentences "
                          "that you have requested to ignore."));
                    LogMessage(warningMsg);
                    }
                }
            }
        }

    if (GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis ||
        GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings)
        {
        CalculateStatisticsIgnoringInvalidSentences();
        }
    else if (GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences)
        {
        CalculateStatistics();
        }
    if (GetTotalWords() == 0)
        {
        LogMessage(_(L"No words were found in this file."), _(L"Import Error"),
                   wxOK | wxICON_EXCLAMATION);
        SetLoadingOriginalTextSucceeded(false);
        return false;
        }
    if (GetTotalWords() < minWordCount)
        {
        LogMessage(
            wxString::Format(_(L"The text that you are analyzing is less than %zu words. "
                               "Test results will not be meaningful with such a small sample; "
                               "therefore, this document will not be analyzed."),
                             minWordCount),
            _(L"Error"), wxOK | wxICON_EXCLAMATION);
        SetLoadingOriginalTextSucceeded(false);
        return false;
        }
    if (GetTotalWords() < 100)
        {
        if (WarningManager::HasWarning(_DT(L"document-less-than-100-words")))
            {
            LogMessage(*WarningManager::GetWarning(_DT(L"document-less-than-100-words")));
            }
        }
    // check for sentences that got broken up by paragraph breaks and warn if there are a lot of
    // them, this indicates a messed up file.
    size_t paragraphBrokenSentences = 0;
    for (auto pos = GetWords()->get_lowercase_beginning_sentences().cbegin();
         pos != GetWords()->get_lowercase_beginning_sentences().cend(); ++pos)
        {
        // if there is a complete, 3-word or more sentence starting with a lowercased letter
        // following an incomplete sentence (that would not be a list item or header),
        // then this might be a sentence accidentally split by two lines (e.g., a paragraph break)
        if (*pos > 0 && GetWords()->get_sentences()[*pos].get_word_count() > 3 &&
            GetWords()->get_sentences()[(*pos)].get_type() ==
                grammar::sentence_paragraph_type::complete &&
            GetWords()->get_sentences()[(*pos) - 1].get_type() ==
                grammar::sentence_paragraph_type::incomplete)
            {
            ++paragraphBrokenSentences;
            }
        }
    if (paragraphBrokenSentences >= 5)
        {
        if (WarningManager::HasWarning(_DT(L"sentences-split-by-paragraph-breaks")))
            {
            WarningMessage warningMsg =
                *WarningManager::GetWarning(_DT(L"sentences-split-by-paragraph-breaks"));
            warningMsg.SetMessage(wxString::Format(
                _(L"This document contains at least %zu sentences that appear to be "
                  "split by paragraph breaks. "
                  "This may lead to incorrect results.\n"
                  "Please review your document to verify that this is intentional."),
                paragraphBrokenSentences));
            LogMessage(warningMsg);
            }
        }
    // Go through the sentences and see if any are not complete but considered valid because of
    // their length. If any are found, then mention it to the user.
    size_t sentencesMissingEndingPunctuationsConsideredCompleteBecauseOfLength = 0;
    for (const auto& sentPos : GetWords()->get_sentences())
        {
        if (sentPos.is_valid() && !sentPos.ends_with_valid_punctuation() &&
            !characters::is_character::is_semicolon(sentPos.get_ending_punctuation()) &&
            sentPos.get_word_count() > GetIncludeIncompleteSentencesIfLongerThanValue())
            {
            ++sentencesMissingEndingPunctuationsConsideredCompleteBecauseOfLength;
            }
        }
    if (sentencesMissingEndingPunctuationsConsideredCompleteBecauseOfLength > 0)
        {
        if (WarningManager::HasWarning(_DT(L"incomplete-sentences-valid-from-length")))
            {
            WarningMessage warningMsg =
                *WarningManager::GetWarning(_DT(L"incomplete-sentences-valid-from-length"));
            warningMsg.SetMessage(wxString::Format(
                // TRANSLATORS: %s is an arrow character and should not be moved
                _(L"This document contains %zu incomplete sentences longer than %zu words which "
                  "will be included in the analysis.\n\nTo change this, increase the "
                  "\"Include incomplete sentences containing more than...\" "
                  "option under \"Project Properties\"%s\"Document Indexing\"."),
                sentencesMissingEndingPunctuationsConsideredCompleteBecauseOfLength,
                GetIncludeIncompleteSentencesIfLongerThanValue(), L" \u00BB "));
            LogMessage(warningMsg);
            }
        }

    LoadHardWords();

    SetLoadingOriginalTextSucceeded(true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddBormuthClozeMeanTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::BORMUTH_CLOZE_MEAN();

    if ((GetDaleChallTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings &&
         (GetTotalWordsFromCompleteSentencesAndHeaders() == 0.0)) ||
        (GetTotalWords() == 0.0))
        {
        LogMessage(wxString::Format(
                       _(L"Unable to calculate %s: at least one word must be present in document."),
                       GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
                   _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }
    if ((GetDaleChallTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings &&
         (GetTotalSentencesFromCompleteSentencesAndHeaders() == 0.0)) ||
        (GetTotalSentences() == 0.0))
        {
        LogMessage(
            wxString::Format(
                _(L"Unable to calculate %s: at least one sentence must be present in document."),
                GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
            _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }

    try
        {
        const double val = round_to_integer(
            readability::bormuth_cloze_mean(
                (GetDaleChallTextExclusionMode() ==
                 SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                    GetTotalWordsFromCompleteSentencesAndHeaders() :
                    GetTotalWords(),
                (GetDaleChallTextExclusionMode() ==
                 SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                    (GetTotalWordsFromCompleteSentencesAndHeaders() -
                     GetTotalHardWordsDaleChall()) :
                    (GetTotalWords() - GetTotalHardWordsDaleChall()),

                (GetDaleChallTextExclusionMode() ==
                 SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                    GetTotalCharactersFromCompleteSentencesAndHeaders() :
                    GetTotalCharacters(),
                (GetDaleChallTextExclusionMode() ==
                 SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                    GetTotalSentencesFromCompleteSentencesAndHeaders() :
                    GetTotalSentences()) *
            100 /* convert to integer percentage*/);

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          ReadabilityMessages::GetPredictedClozeDescription(round_to_integer(val)),
                          theTest.first->get_test(),
                          ReadabilityMessages::GetPredictedClozeNote() + L"<br /><br />" +
                              ReadabilityMessages::GetPunctuationIgnoredNote()) :
                      wxString{};

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxString{}), wxString{},
            std::numeric_limits<double>::quiet_NaN(), val, setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddBormuthGradePlacement35Test(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::BORMUTH_GRADE_PLACEMENT_35();

    if ((GetDaleChallTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings &&
         (GetTotalWordsFromCompleteSentencesAndHeaders() == 0.0)) ||
        (GetTotalWords() == 0.0))
        {
        LogMessage(wxString::Format(
                       _(L"Unable to calculate %s: at least one word must be present in document."),
                       GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
                   _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }
    if ((GetDaleChallTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings &&
         (GetTotalSentencesFromCompleteSentencesAndHeaders() == 0.0)) ||
        (GetTotalSentences() == 0.0))
        {
        LogMessage(
            wxString::Format(
                _(L"Unable to calculate %s: at least one sentence must be present in document."),
                GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
            _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::bormuth_grade_placement_35(
            (GetDaleChallTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                GetTotalWordsFromCompleteSentencesAndHeaders() :
                GetTotalWords(),
            (GetDaleChallTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                (GetTotalWordsFromCompleteSentencesAndHeaders() - GetTotalHardWordsDaleChall()) :
                (GetTotalWords() - GetTotalHardWordsDaleChall()),

            (GetDaleChallTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                GetTotalCharactersFromCompleteSentencesAndHeaders() :
                GetTotalCharacters(),
            (GetDaleChallTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                GetTotalSentencesFromCompleteSentencesAndHeaders() :
                GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ?
                ProjectReportFormat::FormatTestResult(
                    GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                    theTest.first->get_test(), ReadabilityMessages::GetPunctuationIgnoredNote()) :
                wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddPskDaleChallTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::PSK_DALE_CHALL();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        const double gradeValue = readability::powers_sumner_kearl_dale_chall(
            (GetDaleChallTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                GetTotalWordsFromCompleteSentencesAndHeaders() :
                GetTotalWords(),

            GetTotalHardWordsDaleChall(),

            (GetDaleChallTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                GetTotalSentencesFromCompleteSentencesAndHeaders() :
                GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test()) :
                      wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddNewDaleChallTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::DALE_CHALL();

    if ((GetDaleChallTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings &&
         (GetTotalWordsFromCompleteSentencesAndHeaders() == 0.0)) ||
        (GetTotalWords() == 0.0))
        {
        LogMessage(wxString::Format(
                       _(L"Unable to calculate %s: at least one word must be present in document."),
                       GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
                   _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }
    if ((GetDaleChallTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings &&
         (GetTotalSentencesFromCompleteSentencesAndHeaders() == 0.0)) ||
        (GetTotalSentences() == 0.0))
        {
        LogMessage(
            wxString::Format(
                _(L"Unable to calculate %s: at least one sentence must be present in document."),
                GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
            _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }

    try
        {
        size_t gradeBegin{ 0 }, gradeEnd{ 0 };
        if (GetDaleChallTextExclusionMode() ==
            SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings)
            {
            readability::new_dale_chall(
                gradeBegin, gradeEnd, GetTotalWordsFromCompleteSentencesAndHeaders(),
                GetTotalHardWordsDaleChall(), GetTotalSentencesFromCompleteSentencesAndHeaders());
            }
        else
            {
            readability::new_dale_chall(gradeBegin, gradeEnd, GetTotalWords(),
                                        GetTotalHardWordsDaleChall(), GetTotalSentences());
            }

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        // if a range between different grades then it needs to be displayed differently
        wxString gradeValue, explanation;
        if (gradeBegin != gradeEnd)
            {
            gradeValue = wxString::Format(L"%zu-%zu", gradeBegin, gradeEnd);
            explanation = ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeBegin, gradeEnd),
                theTest.first->get_test());
            }
        else
            {
            gradeValue = std::to_wstring(gradeBegin);
            if (gradeBegin == 16)
                {
                gradeValue += L"+";
                }
            explanation = ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeBegin),
                theTest.first->get_test());
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), explanation,
            std::make_pair(safe_divide<double>(gradeBegin + gradeEnd, 2), gradeValue),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeBegin, gradeEnd, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddDegreesOfReadingPowerGeTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::DEGREES_OF_READING_POWER_GE();

    if ((GetDaleChallTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings &&
         (GetTotalWordsFromCompleteSentencesAndHeaders() == 0.0)) ||
        (GetTotalWords() == 0.0))
        {
        LogMessage(wxString::Format(
                       _(L"Unable to calculate %s: at least one word must be present in document."),
                       GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
                   _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }
    if ((GetDaleChallTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings &&
         (GetTotalSentencesFromCompleteSentencesAndHeaders() == 0.0)) ||
        (GetTotalSentences() == 0.0))
        {
        LogMessage(
            wxString::Format(
                _(L"Unable to calculate %s: at least one sentence must be present in document."),
                GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
            _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::degrees_of_reading_power_ge(
            (GetDaleChallTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                GetTotalWordsFromCompleteSentencesAndHeaders() :
                GetTotalWords(),
            (GetDaleChallTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                (GetTotalWordsFromCompleteSentencesAndHeaders() - GetTotalHardWordsDaleChall()) :
                (GetTotalWords() - GetTotalHardWordsDaleChall()),

            (GetDaleChallTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                GetTotalCharactersFromCompleteSentencesAndHeaders() :
                GetTotalCharacters(),
            (GetDaleChallTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                GetTotalSentencesFromCompleteSentencesAndHeaders() :
                GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ?
                ProjectReportFormat::FormatTestResult(
                    GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                    theTest.first->get_test(), ReadabilityMessages::GetPunctuationIgnoredNote()) :
                wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        // Carver table stops at 18
        if (gradeValue == 18)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddDegreesOfReadingPowerTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::DEGREES_OF_READING_POWER();

    if ((GetDaleChallTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings &&
         (GetTotalWordsFromCompleteSentencesAndHeaders() == 0.0)) ||
        (GetTotalWords() == 0.0))
        {
        LogMessage(wxString::Format(
                       _(L"Unable to calculate %s: at least one word must be present in document."),
                       GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
                   _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }
    if ((GetDaleChallTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings &&
         (GetTotalSentencesFromCompleteSentencesAndHeaders() == 0.0)) ||
        (GetTotalSentences() == 0.0))
        {
        LogMessage(
            wxString::Format(
                _(L"Unable to calculate %s: at least one sentence must be present in document."),
                GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
            _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }

    try
        {
        const size_t val = round_to_integer(readability::degrees_of_reading_power(
            (GetDaleChallTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                GetTotalWordsFromCompleteSentencesAndHeaders() :
                GetTotalWords(),
            (GetDaleChallTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                (GetTotalWordsFromCompleteSentencesAndHeaders() - GetTotalHardWordsDaleChall()) :
                (GetTotalWords() - GetTotalHardWordsDaleChall()),

            (GetDaleChallTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                GetTotalCharactersFromCompleteSentencesAndHeaders() :
                GetTotalCharacters(),
            (GetDaleChallTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                GetTotalSentencesFromCompleteSentencesAndHeaders() :
                GetTotalSentences()));

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ?
                ProjectReportFormat::FormatTestResult(
                    ReadabilityMessages::GetDrpUnitDescription(val), theTest.first->get_test(),
                    ReadabilityMessages::GetDrpNote() + L"<br /><br />" +
                        ReadabilityMessages::GetPunctuationIgnoredNote()) :
                wxString{};

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxString{}), wxString{}, val,
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddSolSpanishTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::SOL_SPANISH();

    if (GetTotalSentencesFromCompleteSentencesAndHeaders() == 0)
        {
        LogMessage(
            wxString::Format(
                _(L"Unable to calculate %s: at least one sentence must be present in document."),
                GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
            _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::sol_spanish(
            GetTotalHardWordsSol(), GetTotalSentencesFromCompleteSentencesAndHeaders());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ?
                ProjectReportFormat::FormatTestResult(
                    GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                    theTest.first->get_test(),
                    _(L"Numerals are fully syllabized (i.e., sounded out) for this test. "
                      "Headers and footers are also included and counted as separate sentences.")) :
                wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddElfTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::ELF();

    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        const double gradeValue = readability::easy_listening_formula(
            GetTotalWords(), GetTotalSyllables(), GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test()) :
                      wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddSmogSimplifiedTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::SMOG_SIMPLIFIED();

    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        const double gradeValue = readability::smog_simplified(
            GetTotal3PlusSyllabicWordsNumeralsFullySyllabized(), GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString numeralLabel =
            (GetTotalNumerals() > 0) ?
                _(L"Numerals are fully syllabized (i.e., sounded out) for this test.") :
                wxString{};
        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test(), numeralLabel) :
                      wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddModifiedSmogTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::MODIFIED_SMOG();

    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        const double gradeValue = readability::modified_smog(
            GetUnique3PlusSyllabicWordsNumeralsFullySyllabized(), GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString numeralLabel =
            (GetTotalNumerals() > 0) ?
                _(L"Numerals are fully syllabized (i.e., sounded out) for this test.") :
                wxString{};
        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test(), numeralLabel) :
                      wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 13)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddQuBambergerVanecekTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::QU();

    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        const double gradeValue = readability::quadratwurzelverfahren_bamberger_vanecek(
            GetTotalWords(), GetTotal3PlusSyllabicWordsNumeralsFullySyllabized(),
            GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString numeralLabel =
            (GetTotalNumerals() > 0) ?
                _(L"Numerals are fully syllabized (i.e., sounded out) for this test.") :
                wxString{};
        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test(), numeralLabel) :
                      wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddSmogBambergerVanecekTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::SMOG_BAMBERGER_VANECEK();

    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        const double gradeValue = readability::smog_bamberger_vanecek(
            GetTotal3PlusSyllabicWordsNumeralsFullySyllabized(), GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString numeralLabel =
            (GetTotalNumerals() > 0) ?
                _(L"Numerals are fully syllabized (i.e., sounded out) for this test.") :
                wxString{};
        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test(), numeralLabel) :
                      wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddSmogTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::SMOG();

    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        const double gradeValue = readability::smog(
            GetTotal3PlusSyllabicWordsNumeralsFullySyllabized(), GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString numeralLabel =
            (GetTotalNumerals() > 0) ?
                _(L"Numerals are fully syllabized (i.e., sounded out) for this test.") :
                wxString{};
        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test(), numeralLabel) :
                      wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddInfleszTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::INFLESZ();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        const size_t val = readability::szigriszt_pazos_perspicuity(
            GetTotalWords(), GetTotalSyllables(), GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const readability::inflesz_difficulty diffLevel =
            readability::szigriszt_pazos_perspicuity_score_to_difficulty_inflesz_level(val);

        const wxString description = HasUI() ?
                                         ProjectReportFormat::FormatTestResult(
                                             ReadabilityMessages::GetInfleszDescription(diffLevel),
                                             theTest.first->get_test()) :
                                         wxString{};

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxString{}), wxString{}, val,
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    AddInfleszGraph(setFocus);

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddCrawfordTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::CRAWFORD();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        const double gradeValue =
            readability::crawford(GetTotalWords(), GetTotalSyllables(), GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString numeralLabel =
            (GetTotalNumerals() > 0) ?
                _(L"Numerals are fully syllabized (i.e., sounded out) for this test.") :
                wxString{};
        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test(), numeralLabel) :
                      wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    AddCrawfordGraph(setFocus);

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddNeueWienerSachtextformel1(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::NEUE_WIENER_SACHTEXTFORMEL1();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        const double gradeValue = readability::neue_wiener_sachtextformel_1(
            GetTotalWords(), GetTotalMonoSyllabicWords(), GetTotal3PlusSyllabicWords(),
            GetTotalHardLixRixWords(), GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test()) :
                      wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddNeueWienerSachtextformel2(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::NEUE_WIENER_SACHTEXTFORMEL2();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        const double gradeValue = readability::neue_wiener_sachtextformel_2(
            GetTotalWords(), GetTotal3PlusSyllabicWords(), GetTotalHardLixRixWords(),
            GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test()) :
                      wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddNeueWienerSachtextformel3(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::NEUE_WIENER_SACHTEXTFORMEL3();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        const double gradeValue = readability::neue_wiener_sachtextformel_3(
            GetTotalWords(), GetTotal3PlusSyllabicWords(), GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test()) :
                      wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddWheelerSmithBambergerVanecekTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::WHEELER_SMITH_BAMBERGER_VANECEK();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (GetTotalSentenceUnits() == 0.0)
        {
        LogMessage(
            wxString::Format(
                _(L"Unable to calculate %s: at least one sentence must be present in document."),
                GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
            _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }

    try
        {
        double indexScore(0);
        const size_t gradeValue = readability::wheeler_smith_bamberger_vanecek(
            GetTotalWords(),
            // German version uses 3+ syllable words, not 2+
            GetTotal3PlusSyllabicWords(), GetTotalSentenceUnits(), indexScore);

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test(), ReadabilityMessages::GetUnitNote()) :
                      wxString{};

        wxString displayableGradeValue = wxNumberFormatter::ToString(
            gradeValue, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 10)
            {
            displayableGradeValue += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeValue),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            indexScore, std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddWheelerSmithTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::WHEELER_SMITH();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (GetTotalSentenceUnits() == 0.0)
        {
        LogMessage(
            wxString::Format(
                _(L"Unable to calculate %s: at least one sentence must be present in document."),
                GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
            _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }

    try
        {
        double indexScore(0);
        const size_t gradeValue = readability::wheeler_smith(
            GetTotalWords(),
            // Polysyllabic here means 2+ syllable words (or non-monosyllabic)
            GetTotalWords() - GetTotalMonoSyllabicWords(), GetTotalSentenceUnits(), indexScore);

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test(), ReadabilityMessages::GetUnitNote()) :
                      wxString{};

        wxString displayableGradeValue = wxNumberFormatter::ToString(
            gradeValue, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 4)
            {
            displayableGradeValue += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeValue),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            indexScore, std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddColemanLiauTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::COLEMAN_LIAU();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        double predictedClozeScore(0);
        const double gradeValue = readability::coleman_liau(
            GetTotalWords(), GetTotalCharacters(), GetTotalSentences(), predictedClozeScore);
        // convert to a percentage because the test returns a fractal value
        predictedClozeScore *= 100;

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue) +
                              L"<br /><br />" +
                              ReadabilityMessages::GetPredictedClozeDescription(
                                  round_to_integer(predictedClozeScore)),
                          theTest.first->get_test(),
                          ReadabilityMessages::GetPredictedClozeNote() + L"<br /><br />" +
                              ReadabilityMessages::GetPunctuationIgnoredNote()) :
                      wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), predictedClozeScore, setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

/// Generic interface to add a test based on name or ID
//-------------------------------------------------------
bool BaseProject::AddStandardReadabilityTest(const wxString& id, const bool setFocus /*= true*/)
    {
    PROFILE();
    const auto theTest = GetReadabilityTests().get_test(id);
    if (!theTest.second)
        {
        return false;
        }
    if (HasUI() && theTest.first->get_test().get_id() == _DT(L"dale-chall-test") &&
        GetDaleChallTextExclusionMode() != SpecializedTestTextExclusion::UseSystemDefault)
        {
        if (WarningManager::HasWarning(_DT(L"new-dale-chall-text-exclusion-differs-note")))
            {
            auto warningMsg =
                *WarningManager::GetWarning(_DT(L"new-dale-chall-text-exclusion-differs-note"));
            warningMsg.SetMessage(wxString::Format(
                _(L"NOTE: %s uses a specialized method for excluding text that may differ "
                  "from the system default.\n"
                  "This behavior can be changed from the \"Readability Scores\"%s"
                  "\"Test Options\" page of the "
                  "Options and Project Properties dialogs."),
                GetReadabilityTests().get_test_long_name(id).c_str(), L" \u00BB "));
            LogMessage(warningMsg, true);
            }
        }
    if (HasUI() && theTest.first->get_test().get_id() == _DT(L"harris-jacobson") &&
        GetHarrisJacobsonTextExclusionMode() != SpecializedTestTextExclusion::UseSystemDefault)
        {
        if (WarningManager::HasWarning(_DT(L"harris-jacobson-text-exclusion-differs-note")))
            {
            auto warningMsg =
                *WarningManager::GetWarning(_DT(L"harris-jacobson-text-exclusion-differs-note"));
            warningMsg.SetMessage(wxString::Format(
                _(L"NOTE: %s uses a specialized method for excluding text that may differ "
                  "from the system default.\n"
                  "This behavior can be changed from the \"Readability Scores\"%s"
                  "\"Test Options\" page of the "
                  "Options and Project Properties dialogs."),
                GetReadabilityTests().get_test_long_name(id).c_str(), L" \u00BB "));
            LogMessage(warningMsg, true);
            }
        }

    const auto addTestFunction =
        m_standardTestFunctions.find(theTest.first->get_test().get_interface_id());
    if (addTestFunction != m_standardTestFunctions.cend() &&
        addTestFunction->first == theTest.first->get_test().get_interface_id() &&
        addTestFunction->second != nullptr)
        {
        return (this->*addTestFunction->second)(setFocus);
        }

    wxFAIL_MSG(wxString::Format(L"%s test function pointer not found, unable to add test.", id));
    return false;
    }

//-------------------------------------------------------
bool BaseProject::AddSpacheTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::SPACHE();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        const double gradeValue = readability::spache(
            GetTotalWords(), GetTotalUniqueHardWordsSpache(), GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test()) :
                      wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddNewFogCountTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::NEW_FOG();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if ((IsFogUsingSentenceUnits() && GetTotalSentenceUnits() == 0) || GetTotalSentences() == 0)
        {
        LogMessage(
            wxString::Format(
                _(L"Unable to calculate %s: at least one sentence must be present in document."),
                GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
            _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::new_fog_count(
            GetTotalWords(), GetTotalHardWordsFog(),
            IsFogUsingSentenceUnits() ? GetTotalSentenceUnits() : GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString numeralLabel =
            (GetTotalNumerals() > 0 || GetTotalProperNouns() > 0) ?
                _(L"All numerals and proper nouns are treated as easy words for this test.") :
                wxString{};
        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test(), numeralLabel) :
                      wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddPskFogTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::PSK_GUNNING_FOG();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if ((IsFogUsingSentenceUnits() && GetTotalSentenceUnits() == 0) || GetTotalSentences() == 0)
        {
        LogMessage(
            wxString::Format(
                _(L"Unable to calculate %s: at least one sentence must be present in document."),
                GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
            _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::psk_gunning_fog(
            GetTotalWords(), GetTotalHardWordsFog(),
            IsFogUsingSentenceUnits() ? GetTotalSentenceUnits() : GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString numeralLabel =
            (GetTotalNumerals() > 0 || GetTotalProperNouns() > 0) ?
                _(L"All numerals and proper nouns are treated as easy words for this test.") :
                wxString{};
        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test(), numeralLabel) :
                      wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddFogTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::GUNNING_FOG();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if ((IsFogUsingSentenceUnits() && GetTotalSentenceUnits() == 0) || GetTotalSentences() == 0)
        {
        LogMessage(
            wxString::Format(
                _(L"Unable to calculate %s: at least one sentence must be present in document."),
                GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
            _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }

    try
        {
        const double gradeValue = readability::gunning_fog(
            GetTotalWords(), GetTotalHardWordsFog(),
            IsFogUsingSentenceUnits() ? GetTotalSentenceUnits() : GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString numeralLabel =
            (GetTotalNumerals() > 0 || GetTotalProperNouns() > 0) ?
                _(L"All numerals and proper nouns are treated as easy words for this test.") :
                wxString{};
        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test(), numeralLabel) :
                      wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddForcastTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::FORCAST();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        const double gradeValue =
            readability::forcast(GetTotalWords(), GetTotalMonoSyllabicWords());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test(), ReadabilityMessages::GetForcastNote()) :
                      wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddAmstadTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::AMSTAD();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        readability::flesch_difficulty diffLevel{};
        const size_t val = readability::amstad(
            GetTotalWords(),
            (GetFleschNumeralSyllabizeMethod() == FleschNumeralSyllabize::NumeralIsOneSyllable) ?
                GetTotalSyllablesNumeralsOneSyllable() :
                GetTotalSyllables(),
            GetTotalSentences(), diffLevel);

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description = HasUI() ?
                                         ProjectReportFormat::FormatTestResult(
                                             ReadabilityMessages::GetFleschDescription(diffLevel),
                                             theTest.first->get_test()) :
                                         wxString{};

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxString{}), wxString{}, val,
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);

    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddDanielsonBryan1Test(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::DANIELSON_BRYAN_1();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        const double gradeValue = readability::danielson_bryan_1(
            GetTotalWords(), GetTotalCharactersPlusPunctuation(), GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test()) :
                      wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddDanielsonBryan2Test(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::DANIELSON_BRYAN_2();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        const size_t val = readability::danielson_bryan_2(
            GetTotalWords(), GetTotalCharactersPlusPunctuation(), GetTotalSentences());
        const readability::flesch_difficulty diffLevel =
            readability::flesch_score_to_difficulty_level(val);

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          ReadabilityMessages::GetDanielsonBryan2Description(diffLevel),
                          theTest.first->get_test()) :
                      wxString{};

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxString{}), wxString{}, val,
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    AddDB2Plot(setFocus);

    GetReadabilityTests().include_test(currentTestKey, true);

    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddFleschTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::FLESCH();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        readability::flesch_difficulty diffLevel{};
        const size_t val = readability::flesch_reading_ease(
            GetTotalWords(),
            (GetFleschNumeralSyllabizeMethod() == FleschNumeralSyllabize::NumeralIsOneSyllable) ?
                GetTotalSyllablesNumeralsOneSyllable() :
                GetTotalSyllables(),
            GetTotalSentences(), diffLevel);

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description = HasUI() ?
                                         ProjectReportFormat::FormatTestResult(
                                             ReadabilityMessages::GetFleschDescription(diffLevel),
                                             theTest.first->get_test()) :
                                         wxString{};

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxString{}), wxString{}, val,
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    AddFleschChart(setFocus);

    GetReadabilityTests().include_test(currentTestKey, true);

    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddFarrJenkinsPatersonTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::FARR_JENKINS_PATERSON();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        readability::flesch_difficulty diffLevel{};
        const size_t val = readability::farr_jenkins_paterson(
            GetTotalWords(), GetTotalMonoSyllabicWords(), GetTotalSentences(), diffLevel);

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description = HasUI() ?
                                         ProjectReportFormat::FormatTestResult(
                                             ReadabilityMessages::GetFleschDescription(diffLevel),
                                             theTest.first->get_test()) :
                                         wxString{};

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxString{}), wxString{}, val,
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddNewFarrJenkinsPatersonTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::NEW_FARR_JENKINS_PATERSON();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        const double gradeValue = readability::new_farr_jenkins_paterson(
            GetTotalWords(), GetTotalMonoSyllabicWords(), GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test()) :
                      wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddPskFarrJenkinsPatersonTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::PSK_FARR_JENKINS_PATERSON();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        const double gradeValue = readability::powers_sumner_kearl_farr_jenkins_paterson(
            GetTotalWords(), GetTotalMonoSyllabicWords(), GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test()) :
                      wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddFleschKincaidSimplifiedTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::FLESCH_KINCAID_SIMPLIFIED();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        const double gradeValue = readability::flesch_kincaid_simplified(
            GetTotalWords(),
            (GetFleschKincaidNumeralSyllabizeMethod() ==
             FleschKincaidNumeralSyllabize::FleschKincaidNumeralSoundOutEachDigit) ?
                GetTotalSyllablesNumeralsFullySyllabized() :
                GetTotalSyllables(),
            GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test()) :
                      wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddFleschKincaidTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::FLESCH_KINCAID();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        const double gradeValue = readability::flesch_kincaid(
            GetTotalWords(),
            (GetFleschKincaidNumeralSyllabizeMethod() ==
             FleschKincaidNumeralSyllabize::FleschKincaidNumeralSoundOutEachDigit) ?
                GetTotalSyllablesNumeralsFullySyllabized() :
                GetTotalSyllables(),
            GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test()) :
                      wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddPskFleschTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::PSK_FLESCH();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        const double gradeValue = readability::powers_sumner_kearl_flesch(
            GetTotalWords(),
            (GetFleschNumeralSyllabizeMethod() == FleschNumeralSyllabize::NumeralIsOneSyllable) ?
                GetTotalSyllablesNumeralsOneSyllable() :
                GetTotalSyllables(),
            GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test()) :
                      wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, wxString(theTest.first->get_test().get_long_name().c_str()),
            description, std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddAriTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::ARI();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        const double gradeValue = readability::automated_readability_index(
            GetTotalWords(), GetTotalCharactersPlusPunctuation(), GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            {
            displayableGradeLevel += L"+";
            }

        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test()) :
                      wxString{};

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddNewAriTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::NEW_ARI();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        const double gradeValue = readability::new_automated_readability_index(
            GetTotalWords(), GetTotalCharactersPlusPunctuation(), GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test()) :
                      wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddSimplifiedAriTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::SIMPLE_ARI();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        const double gradeValue = readability::simplified_automated_readability_index(
            GetTotalWords(), GetTotalCharactersPlusPunctuation(), GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test()) :
                      wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 19)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddEflawTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::EFLAW();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    readability::eflaw_difficulty diffLevel{};

    try
        {
        const size_t val = readability::eflaw(diffLevel, GetTotalWords(), GetTotalMiniWords(),
                                              GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ?
                ProjectReportFormat::FormatTestResult(
                    ReadabilityMessages::GetEflawDescription(diffLevel), theTest.first->get_test(),
                    ReadabilityMessages::GetPunctuationIgnoredNote()) :
                wxString{};

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxString{}), wxString{}, val,
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddHarrisJacobsonTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::HARRIS_JACOBSON();

    if ((GetHarrisJacobsonTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings &&
         (GetTotalWordsFromCompleteSentencesAndHeaders() == 0.0)) ||
        (GetTotalWords() == 0.0))
        {
        LogMessage(wxString::Format(
                       _(L"Unable to calculate %s: at least one word must be present in document."),
                       GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
                   _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }
    if ((GetHarrisJacobsonTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings &&
         (GetTotalSentencesFromCompleteSentencesAndHeaders() == 0.0)) ||
        (GetTotalSentences() == 0.0))
        {
        LogMessage(
            wxString::Format(
                _(L"Unable to calculate %s: at least one sentence must be present in document."),
                GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
            _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }

    try
        {
        double gradeValue = 1;
        if (GetHarrisJacobsonTextExclusionMode() ==
            SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings)
            {
            gradeValue =
                readability::harris_jacobson(GetTotalWordsFromCompleteSentencesAndHeaders() -
                                                 GetTotalNumeralsFromCompleteSentencesAndHeaders(),
                                             GetTotalUniqueHarrisJacobsonHardWords(),
                                             GetTotalSentencesFromCompleteSentencesAndHeaders());
            }
        else
            {
            gradeValue = readability::harris_jacobson(GetTotalWords() - GetTotalNumerals(),
                                                      GetTotalUniqueHarrisJacobsonHardWords(),
                                                      GetTotalSentences());
            }

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ? ProjectReportFormat::FormatTestResult(
                          GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue),
                          theTest.first->get_test(), ReadabilityMessages::GetHarrisJacobsonNote()) :
                      wxString{};

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 11.3)
            {
            displayableGradeLevel += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddRixTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::RIX();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (GetTotalSentenceUnits() == 0.0)
        {
        LogMessage(
            wxString::Format(
                _(L"Unable to calculate %s: at least one sentence must be present in document."),
                GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
            _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }

    try
        {
        size_t gradeLevel{ 1 };
        const double val =
            readability::rix(gradeLevel, GetTotalHardLixRixWords(), GetTotalSentenceUnits());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ?
                ProjectReportFormat::FormatTestResult(
                    GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeLevel),
                    theTest.first->get_test(), ReadabilityMessages::GetPunctuationIgnoredNote()) :
                wxString{};

        wxString displayableScore = wxNumberFormatter::ToString(
            gradeLevel, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeLevel == 13)
            {
            displayableScore += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeLevel, displayableScore),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeLevel, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            val, std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddRixGermanFiction(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::RIX_GERMAN_FICTION();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (GetTotalSentenceUnits() == 0.0)
        {
        LogMessage(
            wxString::Format(
                _(L"Unable to calculate %s: at least one sentence must be present in document."),
                GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
            _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }

    try
        {
        const double val = readability::rix_bamberger_vanecek(
            GetTotalWords(), GetTotalHardLixRixWords(), GetTotalSentenceUnits());
        const size_t gradeLevel = readability::rix_index_to_german_fiction_grade_level(val);

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ?
                ProjectReportFormat::FormatTestResult(
                    GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeLevel),
                    theTest.first->get_test(), ReadabilityMessages::GetPunctuationIgnoredNote()) :
                wxString{};

        wxString displayableScore = wxNumberFormatter::ToString(
            gradeLevel, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeLevel == 11)
            {
            displayableScore += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeLevel, displayableScore),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeLevel, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            val, std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddRixGermanNonFiction(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::RIX_GERMAN_NONFICTION();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (GetTotalSentenceUnits() == 0.0)
        {
        LogMessage(
            wxString::Format(
                _(L"Unable to calculate %s: at least one sentence must be present in document."),
                GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
            _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }

    try
        {
        const double val = readability::rix_bamberger_vanecek(
            GetTotalWords(), GetTotalHardLixRixWords(), GetTotalSentenceUnits());
        const size_t gradeLevel = readability::rix_index_to_german_nonfiction_grade_level(val);

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ?
                ProjectReportFormat::FormatTestResult(
                    GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeLevel),
                    theTest.first->get_test(), ReadabilityMessages::GetPunctuationIgnoredNote()) :
                wxString{};

        wxString displayableScore = wxNumberFormatter::ToString(
            gradeLevel, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeLevel == 14)
            {
            displayableScore += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeLevel, displayableScore),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeLevel, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            val, std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddLixGermanChildrensLiterature(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::LIX_GERMAN_CHILDRENS_LITERATURE();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        readability::german_lix_difficulty diffLevel{};
        const size_t val = readability::german_lix(diffLevel, GetTotalWords(),
                                                   GetTotalHardLixRixWords(), GetTotalSentences());
        const size_t gradeLevel =
            readability::lix_index_to_german_childrens_literature_grade_level(val);

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ?
                ProjectReportFormat::FormatTestResult(
                    ReadabilityMessages::GetLixDescription(diffLevel), theTest.first->get_test(),
                    ReadabilityMessages::GetPunctuationIgnoredNote()) :
                wxString{};

        wxString displayableScore = wxNumberFormatter::ToString(
            gradeLevel, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeLevel == 8)
            {
            displayableScore += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeLevel, displayableScore),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeLevel, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            val, std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    AddLixGermanGauge(setFocus);

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddLixGermanTechnical(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::LIX_GERMAN_TECHNICAL();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        readability::german_lix_difficulty diffLevel{};
        const size_t val = readability::german_lix(diffLevel, GetTotalWords(),
                                                   GetTotalHardLixRixWords(), GetTotalSentences());
        const size_t gradeLevel =
            readability::lix_index_to_german_technical_literature_grade_level(val);

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ?
                ProjectReportFormat::FormatTestResult(
                    ReadabilityMessages::GetLixDescription(diffLevel), theTest.first->get_test(),
                    ReadabilityMessages::GetPunctuationIgnoredNote()) :
                wxString{};

        wxString displayableScore = wxNumberFormatter::ToString(
            gradeLevel, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeLevel == 15)
            {
            displayableScore += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeLevel, displayableScore),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeLevel, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            val, std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    AddLixGermanGauge(setFocus);

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
bool BaseProject::AddLixTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::LIX();

    if (!VerifyTotalWordsForTest(currentTestKey))
        {
        return false;
        }
    if (!VerifyTotalSentencesForTest(currentTestKey))
        {
        return false;
        }

    try
        {
        readability::lix_difficulty diffLevel{};
        size_t gradeLevel{ 1 };
        const size_t val = readability::lix(diffLevel, gradeLevel, GetTotalWords(),
                                            GetTotalHardLixRixWords(), GetTotalSentences());

        const auto theTest = GetReadabilityTests().find_test(currentTestKey);
        if (!VerifyTestBeforeAdding(theTest))
            {
            return false;
            }

        const wxString description =
            HasUI() ?
                ProjectReportFormat::FormatTestResult(
                    ReadabilityMessages::GetLixDescription(diffLevel), theTest.first->get_test(),
                    ReadabilityMessages::GetPunctuationIgnoredNote()) :
                wxString{};

        wxString displayableScore = wxNumberFormatter::ToString(
            gradeLevel, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeLevel == 13)
            {
            displayableScore += L"+";
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeLevel, displayableScore),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeLevel, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            val, std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    catch (...)
        {
        HandleFailedTestCalculation(currentTestKey);
        return false;
        }

    AddLixGauge(setFocus);

    GetReadabilityTests().include_test(currentTestKey, true);
    return true;
    }

//-------------------------------------------------------
void BaseProject::SyncCustomTests()
    {
    for (auto pos = m_customTestsInUse.begin(); pos != m_customTestsInUse.end();
         /* in loop*/)
        {
        // NOLINTBEGIN(modernize-use-ranges)
        auto testIter =
            std::find(m_custom_word_tests.begin(), m_custom_word_tests.end(), pos->GetTestName());
        // NOLINTEND(modernize-use-ranges)
        // shouldn't happen, but remove test from project if not found in the global system
        if (testIter == m_custom_word_tests.end())
            {
            pos = m_customTestsInUse.erase(pos);
            continue;
            }
        pos->SetIterator(testIter);
        pos->SetIsDaleChallFormula(
            pos->GetIterator()->get_formula().find(
                ReadabilityFormulaParser::GetCustomNewDaleChallSignature()) != std::wstring::npos);
        pos->SetIsHarrisJacobsonFormula(
            pos->GetIterator()->get_formula().find(
                ReadabilityFormulaParser::GetCustomHarrisJacobsonSignature()) !=
            std::wstring::npos);
        ++pos;
        }
    }

//-------------------------------------------------------
bool BaseProject::AddCustomReadabilityTest(const wxString& name, const bool calculate /*= false*/)
    {
    ClearReadabilityTestResult();

    // first, see if this is a legit custom test that is loaded in the global system
    // NOLINTNEXTLINE(modernize-use-ranges)
    const auto testIter = std::find(m_custom_word_tests.cbegin(), m_custom_word_tests.cend(), name);
    if (testIter == m_custom_word_tests.cend())
        {
        return false;
        }

    // NOLINTNEXTLINE(modernize-use-ranges)
    auto pos = std::find(m_customTestsInUse.begin(), m_customTestsInUse.end(), name);
    // see if test needs to be included
    if (pos == m_customTestsInUse.end())
        {
        m_customTestsInUse.emplace_back(name);
        // reset the internal iterators that point to the global tests
        SyncCustomTests();
        // NOLINTNEXTLINE(modernize-use-ranges)
        pos = std::find(m_customTestsInUse.begin(), m_customTestsInUse.end(), name);
        if (pos == m_customTestsInUse.end())
            {
            return false;
            }
        }

    if (calculate)
        {
        if (GetTotalWords() == 0)
            {
            LogMessage(_(L"Unable to calculate custom readability test: "
                         "at least one word must be present in document."),
                       _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
            return false;
            }
        try
            {
            // put together a description detailing the formula and stemming type used by this test
            wxString customDescription =
                wxString::Format(L"<p>%s</p>\r\n<p>&nbsp;&nbsp;&nbsp;&nbsp;%s</p>",
                                 _(L"This is a custom test using the following formula:"),
                                 ProjectReportFormat::FormatFormulaToHtml(
                                     pos->GetIterator()->get_formula().c_str()));
            if (pos->GetIterator()->is_using_familiar_words())
                {
                customDescription += wxString::Format(
                    L"<p>%s</p>\r\n<ul>\r\n",
                    _(L"This test uses the following criteria to determine word familiarity:"));
                if (pos->GetIterator()->is_including_dale_chall_list())
                    {
                    customDescription +=
                        wxString(L"<li>") +
                        wxString::Format(_(L"%s familiar word list"), _DT(L"New Dale Chall")) +
                        wxString(L"</li>");
                    }
                if (pos->GetIterator()->is_including_spache_list())
                    {
                    customDescription +=
                        wxString(L"<li>") +
                        wxString::Format(_(L"%s familiar word list"), _DT(L"Spache Revised")) +
                        wxString(L"</li>");
                    }
                if (pos->GetIterator()->is_including_harris_jacobson_list())
                    {
                    customDescription +=
                        wxString(L"<li>") +
                        wxString::Format(_(L"%s familiar word list"), _DT(L"Harris-Jacobson")) +
                        wxString(L"</li>");
                    }
                if (pos->GetIterator()->is_including_stocker_list())
                    {
                    customDescription += wxString(L"<li>") +
                                         _(L"Stocker's Catholic supplementary word list") +
                                         wxString(L"</li>");
                    }
                if (pos->GetIterator()->is_including_custom_familiar_word_list())
                    {
                    // if word list is in the same folder as the project,
                    // then just show the file name
                    wxString customFilePath{
                        pos->GetIterator()->get_familiar_word_list_file_path().c_str()
                    };
                    if (GetDocumentStorageMethod() == TextStorage::LoadFromExternalDocument)
                        {
                        if (wxFileName(customFilePath)
                                .GetPath()
                                .IsSameAs(wxFileName(GetOriginalDocumentFilePath()).GetPath(),
                                          wxFileName::IsCaseSensitive()))
                            {
                            customFilePath = wxFileName(customFilePath).GetFullName();
                            }
                        }
                    customDescription += wxString(L"<li>");
                    if (pos->GetIterator()->get_stemming_type() ==
                        stemming::stemming_type::no_stemming)
                        {
                        customDescription +=
                            _(L"A custom word list:") +
                            wxString::Format(L"<br />&nbsp;&nbsp;&nbsp;&nbsp;&ldquo;"
                                             "<span style=\"font-style:italic;\">%s</span>&rdquo;",
                                             customFilePath);
                        }
                    else
                        {
                        customDescription +=
                            wxString::Format(_(L"A custom word list (that is using %s stemming):"),
                                             ProjectReportFormat::GetStemmingDisplayName(
                                                 pos->GetIterator()->get_stemming_type())) +
                            wxString::Format(L"<br />&nbsp;&nbsp;&nbsp;&nbsp;&ldquo;"
                                             "<span style=\"font-style:italic;\">%s</span>&rdquo;",
                                             customFilePath);
                        }
                    customDescription += wxString(L"</li>");
                    }
                if (pos->GetIterator()->get_proper_noun_method() ==
                    readability::proper_noun_counting_method::all_proper_nouns_are_familiar)
                    {
                    customDescription +=
                        wxString(L"<li>") + _(L"Proper nouns") + wxString(L"</li>");
                    }
                else if (pos->GetIterator()->get_proper_noun_method() ==
                         readability::proper_noun_counting_method::
                             only_count_first_instance_of_proper_noun_as_unfamiliar)
                    {
                    customDescription += wxString(L"<li>") +
                                         _(L"Proper nouns (except first occurrence)") +
                                         wxString(L"</li>");
                    }
                if (pos->GetIterator()->is_including_numeric_as_familiar())
                    {
                    customDescription += wxString(L"<li>") + _(L"Numerals") + wxString(L"</li>");
                    }
                customDescription += wxString(L"\r\n</ul>");
                }
            customDescription = L"<tr><td>" + customDescription + L"</td></tr>";
            SetCurrentCustomTest(pos->GetIterator()->get_name().c_str());

            try
                {
                GetFormulaParser().UpdateVariables();
                [[maybe_unused]]
                auto notUsedRes = GetFormulaParser().evaluate(
                    wxString(pos->GetIterator()->get_formula().c_str()).ToStdString());
                if (!GetFormulaParser().success())
                    {
                    SetReadabilityTestResult(
                        wxString(pos->GetIterator()->get_name().c_str()),
                        wxString(pos->GetIterator()->get_name().c_str()), customDescription,
                        std::make_pair(
                            std::numeric_limits<double>::quiet_NaN(),
                            wxString::Format(
                                _(L"Syntax error in formula at position %s."),
                                std::to_wstring(GetFormulaParser().get_last_error_position()))),
                        wxString{}, std::numeric_limits<double>::quiet_NaN(),
                        std::numeric_limits<double>::quiet_NaN(), false);
                    }
                else if (pos->GetIterator()->get_test_type() ==
                         readability::readability_test_type::grade_level)
                    {
                    // custom DC test may will be using range values instead of a score
                    if (pos->IsDaleChallFormula())
                        {
                        uint32_t gradeBegin{ 0 }, gradeEnd{ 0 };
                        split_int64(static_cast<uint64_t>(GetFormulaParser().get_result()),
                                    gradeBegin, gradeEnd);
                        if (gradeBegin == gradeEnd)
                            {
                            SetReadabilityTestResult(
                                wxString(pos->GetIterator()->get_name().c_str()),
                                wxString(pos->GetIterator()->get_name().c_str()),
                                L"<tr><td>" +
                                    GetReadabilityMessageCatalog().GetGradeScaleDescription(
                                        static_cast<size_t>(gradeBegin)) +
                                    L"</td></tr>" + customDescription,
                                std::make_pair(
                                    gradeBegin,
                                    wxNumberFormatter::ToString(
                                        gradeBegin, 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes)),
                                ReadabilityMessages::GetAgeFromUSGrade(
                                    gradeBegin, gradeEnd,
                                    GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
                                std::numeric_limits<double>::quiet_NaN(),
                                std::numeric_limits<double>::quiet_NaN(), false);
                            }
                        else
                            {
                            SetReadabilityTestResult(
                                wxString(pos->GetIterator()->get_name().c_str()),
                                wxString(pos->GetIterator()->get_name().c_str()),
                                L"<tr><td>" +
                                    GetReadabilityMessageCatalog().GetGradeScaleDescription(
                                        gradeBegin, gradeEnd) +
                                    L"</td></tr>\n" + customDescription,
                                std::make_pair(
                                    safe_divide<double>(gradeBegin + gradeEnd, 2),
                                    wxNumberFormatter::ToString(
                                        gradeBegin, 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                                        wxString(L"-") +
                                        wxNumberFormatter::ToString(
                                            gradeEnd, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes)),
                                ReadabilityMessages::GetAgeFromUSGrade(
                                    gradeBegin, gradeEnd,
                                    GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
                                std::numeric_limits<double>::quiet_NaN(),
                                std::numeric_limits<double>::quiet_NaN(), false);
                            }
                        }
                    else
                        {
                        const double score =
                            readability::truncate_k12_plus_grade(GetFormulaParser().get_result());
                        SetReadabilityTestResult(
                            wxString(pos->GetIterator()->get_name().c_str()),
                            wxString(pos->GetIterator()->get_name().c_str()),
                            L"<tr><td>" +
                                GetReadabilityMessageCatalog().GetGradeScaleDescription(score) +
                                L"</td></tr>\n" + customDescription,
                            std::make_pair(
                                score,
                                wxNumberFormatter::ToString(
                                    score, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes)),
                            ReadabilityMessages::GetAgeFromUSGrade(
                                score, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
                            std::numeric_limits<double>::quiet_NaN(),
                            std::numeric_limits<double>::quiet_NaN(), false);
                        }
                    }
                else if (pos->GetIterator()->get_test_type() ==
                         readability::readability_test_type::index_value)
                    {
                    const double score = GetFormulaParser().get_result();
                    SetReadabilityTestResult(
                        wxString(pos->GetIterator()->get_name().c_str()),
                        wxString(pos->GetIterator()->get_name().c_str()),
                        L"<tr><td>" + _(L"Score: ") +
                            wxNumberFormatter::ToString(
                                score, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                            L"</td></tr>\n" + customDescription,
                        std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxString{}),
                        wxString{}, score, std::numeric_limits<double>::quiet_NaN(), false);
                    }
                else if (pos->GetIterator()->get_test_type() ==
                         readability::readability_test_type::predicted_cloze_score)
                    {
                    const double score = GetFormulaParser().get_result();
                    SetReadabilityTestResult(
                        wxString(pos->GetIterator()->get_name().c_str()),
                        wxString(pos->GetIterator()->get_name().c_str()),
                        L"<tr><td>" + _(L"Predicted cloze score: ") +
                            wxNumberFormatter::ToString(
                                score, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                            L"</td></tr>\n" + customDescription,
                        std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxString{}),
                        wxString{}, std::numeric_limits<double>::quiet_NaN(), score, false);
                    }
                }
            catch (const std::exception& ex)
                {
                SetReadabilityTestResult(
                    wxString(pos->GetIterator()->get_name().c_str()),
                    wxString(pos->GetIterator()->get_name().c_str()), customDescription,
                    std::make_pair(std::numeric_limits<double>::quiet_NaN(), ex.what()), wxString{},
                    std::numeric_limits<double>::quiet_NaN(),
                    std::numeric_limits<double>::quiet_NaN(), false);
                }
            catch (...)
                {
                SetReadabilityTestResult(
                    wxString(pos->GetIterator()->get_name().c_str()),
                    wxString(pos->GetIterator()->get_name().c_str()), wxString{},
                    std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxString{}),
                    wxString{}, std::numeric_limits<double>::quiet_NaN(),
                    std::numeric_limits<double>::quiet_NaN(), false);
                }
            }
        catch (...)
            {
            LogMessage(wxString::Format(_(L"Unable to calculate %s."),
                                        pos->GetIterator()->get_name().c_str()),
                       _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
            return false;
            }
        }
    return true;
    }

//-------------------------------------------------------
std::vector<CustomReadabilityTestInterface>::iterator
BaseProject::RemoveCustomReadabilityTest(const wxString& testName, [[maybe_unused]] const int Id)
    {
    // NOLINTNEXTLINE(modernize-use-ranges)
    const auto testPos = std::find(m_customTestsInUse.begin(), m_customTestsInUse.end(), testName);
    if (testPos == m_customTestsInUse.end())
        {
        return testPos;
        }
    return m_customTestsInUse.erase(testPos);
    }

//-------------------------------------------------------
bool BaseProject::RemoveTest(const wxString& name)
    {
    if (name == ReadabilityMessages::GetDolchLabel())
        {
        m_includeDolchSightWords = false;
        return true;
        }
    if (GetReadabilityTests().include_test(name, false))
        {
        // remove from the test's goals too
        const auto [sTest, found] = GetReadabilityTests().find_test(name.wc_str());
        if (found)
            {
            auto foundTest = GetTestGoals().find(TestGoal(sTest->get_test().get_id()));
            if (foundTest != GetTestGoals().end())
                {
                GetTestGoals().erase(foundTest);
                }
            }
        return true;
        }
    if (HasCustomTest(name))
        {
        RemoveCustomReadabilityTest(name, GetCustomTest(name)->GetIterator()->get_interface_id());
        // remove from the test's goals too
        auto foundTest = GetTestGoals().find(TestGoal(name.wc_str()));
        if (foundTest != GetTestGoals().end())
            {
            GetTestGoals().erase(foundTest);
            }
        return true;
        }

    return false;
    }

//------------------------------------------------------
bool BaseProject::FindMissingFile(const wxString& filePath,
                                  wxString& fileBySameNameInProjectDirectory)
    {
    // if file not found, then try to search for it in the subdirectories from where the project is
    fileBySameNameInProjectDirectory =
        FindFileInMatchingDirStructure(GetProjectDirectory(), filePath);
    if (wxFile::Exists(fileBySameNameInProjectDirectory))
        {
        auto warningIter =
            WarningManager::GetWarning(_DT(L"file-autosearch-from-project-directory"));
        // if they want to be prompted for this...
        if (warningIter != WarningManager::GetWarnings().end() && warningIter->ShouldBeShown())
            {
            wxRichMessageDialog msg(
                wxGetApp().GetParentingWindow(),
                wxString::Format(
                    _(L"%s:\n\nFile could not be located. However, a file by the same name "
                      "was found at:\n\n%s\n\nDo you wish to use this file instead?"),
                    filePath, fileBySameNameInProjectDirectory),
                warningIter->GetTitle(), warningIter->GetFlags());
            msg.ShowCheckBox(_(L"Remember my answer"));
            const int dlgResponse = msg.ShowModal();
            // save the checkbox status
            if (msg.IsCheckBoxChecked())
                {
                warningIter->Show(false);
                warningIter->SetPreviousResponse(dlgResponse);
                }
            // now see if they said "Yes" or "No"
            if (dlgResponse == wxID_YES)
                {
                SetModifiedFlag();
                return true;
                }

            fileBySameNameInProjectDirectory.Clear();
            return false;
            }
        // or if they said "yes" before, then use the found path
        if (warningIter != WarningManager::GetWarnings().end() &&
            warningIter->GetPreviousResponse() == wxID_YES)
            {
            SetModifiedFlag();
            return true;
            }
        // if they said "no" before, then don't bother using the found path

        fileBySameNameInProjectDirectory.Clear();
        return false;
        }
    // alternate file path not found

    fileBySameNameInProjectDirectory.Clear();
    return false;
    }

//------------------------------------------------------
void BaseProject::CopySettings(const BaseProject& that)
    {
    m_hasUI = that.m_hasUI;

    m_language = that.m_language;
    m_reviewer = that.GetReviewer();
    m_status = that.GetStatus();
    m_appendedDocumentFilePath = that.GetAppendedDocumentFilePath();

    // this is only needed if we need to go searching for a missing document file
    SetProjectDirectory(that.GetProjectDirectory());

    // phrases to be excluded
    m_excludedPhrasesPath = that.GetExcludedPhrasesPath();

    m_exclusionBlockTags = that.GetExclusionBlockTags();

    // indexing and project options
    m_difficultSentenceLength = that.GetDifficultSentenceLength();
    m_longSentenceMethod = that.GetLongSentenceMethod();
    m_numeralSyllabicationMethod = that.GetNumeralSyllabicationMethod();
    m_ignoreBlankLinesForParagraphsParser = that.IsIgnoringBlankLinesForParagraphsParser();
    m_ignoreIndentingForParagraphsParser = that.IsIgnoringIndentingForParagraphsParser();
    m_sentenceStartMustBeUppercased = that.GetSentenceStartMustBeUppercased();
    m_aggressiveExclusion = that.IsExcludingAggressively();
    m_excludeTrailingCopyrightNoticeParagraphs =
        that.IsExcludingTrailingCopyrightNoticeParagraphs();
    m_excludeTrailingCitations = that.IsExcludingTrailingCitations();
    m_excludeFileAddresses = that.IsExcludingFileAddresses();
    m_excludeNumerals = that.IsExcludingNumerals();
    m_excludeProperNouns = that.IsExcludingProperNouns();
    m_includeExcludedPhraseFirstOccurrence = that.IsIncludingExcludedPhraseFirstOccurrence();
    m_paragraphsParsingMethod = that.GetParagraphsParsingMethod();
    m_invalidSentenceMethod = that.GetInvalidSentenceMethod();
    m_includeIncompleteSentencesIfLongerThan =
        that.GetIncludeIncompleteSentencesIfLongerThanValue();
    m_textSource = that.GetTextSource();
    m_documentStorageMethod = that.GetDocumentStorageMethod();
    if (that.GetDocumentStorageMethod() == TextStorage::EmbedText)
        {
        SetDocumentText(that.GetDocumentText());
        }
    m_varianceMethod = that.GetVarianceMethod();
    m_minDocWordCountForBatch = that.m_minDocWordCountForBatch;

    // grammar
    m_spellcheck_ignore_proper_nouns = that.m_spellcheck_ignore_proper_nouns;
    m_spellcheck_ignore_uppercased = that.m_spellcheck_ignore_uppercased;
    m_spellcheck_ignore_numerals = that.m_spellcheck_ignore_numerals;
    m_spellcheck_ignore_file_addresses = that.m_spellcheck_ignore_file_addresses;
    m_spellcheck_ignore_programmer_code = that.m_spellcheck_ignore_programmer_code;
    m_allow_colloquialisms = that.m_allow_colloquialisms;
    m_spellcheck_ignore_social_media_tags = that.m_spellcheck_ignore_social_media_tags;

    // tests inclusion
    m_readabilityTests.clear();
    m_readabilityTests = that.m_readabilityTests;

    m_testGoals = that.m_testGoals;
    m_statGoals = that.m_statGoals;

    m_includeDolchSightWords = that.m_includeDolchSightWords;

    m_readMessages = that.m_readMessages;

    m_includeScoreSummaryReport = that.IsIncludingScoreSummaryReport();

    // test-specific options
    m_hjTextExclusion = that.m_hjTextExclusion;
    m_dcTextExclusion = that.m_dcTextExclusion;
    m_dcProperNounCountingMethod = that.m_dcProperNounCountingMethod;
    m_includeStockerCatholicDCSupplement = that.m_includeStockerCatholicDCSupplement;
    m_fogUseSentenceUnits = that.m_fogUseSentenceUnits;
    m_fleschNumeralSyllabizeMethod = that.m_fleschNumeralSyllabizeMethod;
    m_fleschKincaidNumeralSyllabizeMethod = that.m_fleschKincaidNumeralSyllabizeMethod;

    m_statsReportInfo = that.GetStatisticsReportInfo();
    m_statsInfo = that.GetStatisticsInfo();
    m_grammarInfo = that.GetGrammarInfo();
    m_wordsBreakdownInfo = that.GetWordsBreakdownInfo();
    m_sentencesBreakdownInfo = that.GetSentencesBreakdownInfo();

    // Remove any custom tests that this project has that the other one doesn't.
    // We don't want to arbitrarily clear this out, because we want to retain the
    // statistics for any custom tests that will remain in here.
    for (auto pos = GetCustTestsInUse().cbegin(); pos != GetCustTestsInUse().cend();
         /* handled in the loop*/)
        {
        if (!that.HasCustomTest(pos->GetTestName()))
            {
            pos = RemoveCustomReadabilityTest(pos->GetTestName(),
                                              pos->GetIterator()->get_interface_id());
            }
        else
            {
            ++pos;
            }
        }
    /* add any custom tests that other project has. If this project already has the test then leave
       it alone and have its statistics kept intact.*/
    for (const auto& cTestInUse : that.GetCustTestsInUse())
        {
        auto customTestPos = std::find(GetCustTestsInUse().cbegin(), GetCustTestsInUse().cend(),
                                       cTestInUse.GetTestName());
        // see if test needs to be added
        if (customTestPos == GetCustTestsInUse().end())
            {
            m_customTestsInUse.emplace_back(cTestInUse.GetTestName());
            }
        }
    // reset the internal iterators that point to the global tests
    SyncCustomTests();
    }

//------------------------------------------------
bool BaseProject::VerifyTestBeforeAdding(
    const std::pair<std::vector<ProjectTestType>::const_iterator, bool>& theTest)
    {
    // see if the test was found in the list of known tests (this shouldn't be an issue)
    assert(theTest.second);
    if (!theTest.second)
        {
        throw std::exception();
        }
    // see if test relates to the language for the project
    if (!theTest.first->get_test().has_language(GetProjectLanguage()))
        {
        LogMessage(wxString::Format(_(L"\"%s\" is not compatible with this project's language. "
                                      "This test will be removed."),
                                    theTest.first->get_test().get_short_name().c_str()),
                   _(L"Warning"), wxOK | wxICON_EXCLAMATION);
        GetReadabilityTests().include_test(theTest.first->get_test().get_id().c_str(), false);
        return false;
        }
    return true;
    }

//------------------------------------------------
void BaseProject::HandleFailedTestCalculation(const wxString& testName)
    {
    LogMessage(wxString::Format(_(L"Unable to calculate %s."),
                                GetReadabilityTests().get_test_short_name(testName).c_str()),
               _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
    GetReadabilityTests().include_test(testName, false);
    }

//------------------------------------------------
bool BaseProject::ReviewTestGoal(const wxString& testName, const double score)
    {
    assert((!std::isnan(score) ||
            // doesn't have an actual score
            testName == ReadabilityMessages::GetDolchLabel() ||
            // these tests can fail
            testName == ReadabilityMessages::FRASE() || testName == ReadabilityMessages::FRY() ||
            testName == ReadabilityMessages::GPM_FRY() ||
            testName == ReadabilityMessages::RAYGOR() ||
            testName == ReadabilityMessages::SCHWARTZ()) &&
           L"Score should not be NaN!");
    if (GetTestGoals().empty())
        {
        return false;
        }

    auto foundTest = GetTestGoals().find(TestGoal(testName.wc_str()));
    if (foundTest != GetTestGoals().end())
        {
        auto node = GetTestGoals().extract(foundTest);
        node.value().GetPassFailFlags().set(); // set to passing
        // set to failure if there is a goal and score doesn't meet it
        // (or is NaN, like when Fry fails to plot)
        if (!std::isnan(node.value().GetMinGoal()) &&
            (std::isnan(score) || score < node.value().GetMinGoal()))
            {
            node.value().GetPassFailFlags().set(0, false);
            }
        if (!std::isnan(node.value().GetMaxGoal()) &&
            (std::isnan(score) || score > node.value().GetMaxGoal()))
            {
            node.value().GetPassFailFlags().set(1, false);
            }
        GetTestGoals().insert(std::move(node));
        return true;
        }
    return false;
    }

//------------------------------------------------
void BaseProject::ReviewStatGoals()
    {
    for (const auto& statGoal : GetStatGoals())
        {
        const auto statPos =
            GetStatGoalLabels().find(comparable_first_pair(statGoal.GetName(), statGoal.GetName()));
        if (statPos != GetStatGoalLabels().cend())
            {
            ReviewStatGoal(statGoal.GetName().c_str(), statPos->second(this));
            }
        }
    }

//------------------------------------------------
bool BaseProject::ReviewStatGoal(const wxString& statName, const double value)
    {
    assert(!std::isnan(value) && L"Stat value should not be NaN!");
    if (GetStatGoals().empty())
        {
        return false;
        }

    auto foundStat = GetStatGoals().find(StatGoal(statName.wc_str()));
    if (foundStat != GetStatGoals().end())
        {
        auto node = GetStatGoals().extract(foundStat);
        node.value().GetPassFailFlags().set(); // set to passing
        // set to failure if there is a goal and value doesn't meet it
        if (!std::isnan(node.value().GetMinGoal()) &&
            (std::isnan(value) || value < node.value().GetMinGoal()))
            {
            node.value().GetPassFailFlags().set(0, false);
            }
        if (!std::isnan(node.value().GetMaxGoal()) &&
            (std::isnan(value) || value > node.value().GetMaxGoal()))
            {
            node.value().GetPassFailFlags().set(1, false);
            }
        GetStatGoals().insert(std::move(node));
        return true;
        }

    wxLogError(L"%s: unable to find statistic for goal review.", statName);

    return false;
    }
