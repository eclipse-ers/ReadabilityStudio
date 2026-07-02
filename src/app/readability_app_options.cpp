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

#include "readability_app_options.h"
#include "../Wisteria-Dataviz/src/base/colorbrewer.h"
#include "../Wisteria-Dataviz/src/import/html_encode.h"
#include "../Wisteria-Dataviz/src/math/mathematics.h"
#include "../ui/dialogs/custom_test_dlg.h"
#include "readability_app.h"
#include <algorithm>
#include <wx/fontenum.h>

wxDECLARE_APP(ReadabilityApp);

//------------------------------------------------
bool PreAppInitOptions::LoadOptionsFile(wxString optionsFile)
    {
    wxString fileContent;
    if (!wxFile::Exists(optionsFile) || !Wisteria::TextStream::ReadFile(optionsFile, fileContent))
        {
        return false;
        }

    tinyxml2::XMLDocument doc;
    doc.Parse(fileContent.utf8_str());
    if (doc.Error())
        {
        // may appear while program is loading
        wxLogError(L"Unable to load configuration file:\n%s", doc.ErrorStr());
        return false;
        }
    // see if it is a valid config file
    auto* node = doc.FirstChildElement(ReadabilityAppOptions::XML_CONFIG_HEADER.data());
    if (node == nullptr)
        {
        wxMessageBox(_(L"Invalid configuration file. Project header section not found."),
                     _(L"Error"), wxOK | wxICON_ERROR);
        return false;
        }
    // read in the configurations
    auto* configRootNode =
        node->FirstChildElement(ReadabilityAppOptions::XML_CONFIGURATIONS.data());
    if (configRootNode == nullptr)
        {
        wxMessageBox(_(L"Invalid configuration file. No configurations found."), _(L"Error"),
                     wxOK | wxICON_ERROR);
        return false;
        }

    // appearance of the program
    auto* appearanceNode =
        configRootNode->FirstChildElement(ReadabilityAppOptions::XML_APPEARANCE.data());
    if (appearanceNode != nullptr)
        {
        const int maximized = appearanceNode->ToElement()->IntAttribute(
            ReadabilityAppOptions::XML_WINDOW_MAXIMIZED.data(), 1);
        m_appWindowMaximized = int_to_bool(maximized);
        // NOTE: DIPs can't be used here because this is called before UI construction
        m_appWindowWidth = appearanceNode->ToElement()->IntAttribute(
            ReadabilityAppOptions::XML_WINDOW_WIDTH.data(), 800);
        m_appWindowHeight = appearanceNode->ToElement()->IntAttribute(
            ReadabilityAppOptions::XML_WINDOW_HEIGHT.data(), 700);
        // make sure the values make sense
        if (m_appWindowWidth < 1)
            {
            m_appWindowWidth = 800;
            }
        if (m_appWindowHeight < 1)
            {
            m_appWindowHeight = 700;
            }
        m_uiLanguage = static_cast<UiLanguage>(appearanceNode->ToElement()->IntAttribute(
            ReadabilityAppOptions::XML_UI_LANGUAGE.data(), static_cast<int>(UiLanguage::Default)));
        }
    // log report settings
    auto* logSettingsNode =
        configRootNode->FirstChildElement(ReadabilityAppOptions::XML_LOG_SETTINGS.data());
    if (logSettingsNode != nullptr)
        {
        auto* logAppendNode =
            logSettingsNode->FirstChildElement(ReadabilityAppOptions::XML_LOG_APPEND_DAILY.data());
        if (logAppendNode != nullptr)
            {
            m_logAppendDailyLog = int_to_bool(logAppendNode->ToElement()->IntAttribute(
                ReadabilityAppOptions::XML_VALUE.data(), 1));
            }
        }
    auto* projectSettings =
        configRootNode->FirstChildElement(ReadabilityAppOptions::XML_PROJECT_SETTINGS.data());
    if (projectSettings != nullptr)
        {
        m_userName = ReadabilityAppOptions::TiXmlNodeAttributeToString(
            projectSettings->FirstChildElement(ReadabilityAppOptions::XML_REVIEWER.data()),
            ReadabilityAppOptions::XML_VALUE.data());
        }
    if (m_userName.empty())
        {
        m_userName = wxGetUserName();
        }
    return true;
    }

//------------------------------------------------
ReadabilityAppOptions::ReadabilityAppOptions()
    {
    SetFonts();
    SetColorsFromSystem();
    BaseProject::InitializeStandardReadabilityTests();
    BaseProject::ResetStandardReadabilityTests(m_readabilityTests);
    // set the warnings system
    WarningManager::AddWarning(WarningMessage(
        _DT(L"project-open-as-read-only"), _(L"Project file will be opened as read only."),
        wxString{}, _(L"Warn about projects being opened as read-only."), wxOK | wxICON_INFORMATION,
        false));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"remove-test-from-project"), wxString{}, wxString{},
        _(L"Prompt when removing a test from a project."), wxICON_INFORMATION, false));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"delete-document-from-batch"), wxString{}, wxString{},
        _(L"Prompt when removing a document from a batch project."), wxICON_INFORMATION, false));
    WarningManager::AddWarning(
        WarningMessage(_DT(L"document-no-words"),
                       _(L"The text that you are analyzing does not contain any valid words. "
                         "No calculations will be performed."),
                       _(L"Warning"), _(L"Warn about documents that do not contain any words."),
                       wxOK | wxICON_EXCLAMATION, false));
    WarningManager::AddWarning(
        WarningMessage(_DT(L"document-less-than-20-words"),
                       _(L"The text that you are analyzing is less than 20 words. "
                         "Most test results will not be meaningful with such a small sample."),
                       _(L"Warning"), _(L"Warn about documents containing less than 20 words."),
                       wxOK | wxICON_EXCLAMATION, false));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"document-less-than-100-words"),
        _(L"The text that you are analyzing is less than 100 words. "
          "Factors, such as word and syllable counts, will be standardized for some tests."),
        _(L"Warning"), _(L"Warn about documents containing less than 100 words."),
        wxOK | wxICON_EXCLAMATION, false));
    WarningManager::AddWarning(
        WarningMessage(_DT(L"sentences-split-by-paragraph-breaks"), wxString{}, _(L"Warning"),
                       _(L"Warn about documents that contain sentences split by paragraph breaks."),
                       wxOK | wxICON_EXCLAMATION, false));
    WarningManager::AddWarning(
        WarningMessage(_DT(L"incomplete-sentences-valid-from-length"), wxString{}, _(L"Warning"),
                       _(L"Warn about documents that contain long incomplete sentences "
                         "that will be included in the analysis."),
                       wxOK | wxICON_EXCLAMATION, false));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"high-count-sentences-being-ignored"),
        _(L"This document contains a large percentage of incomplete sentences that you "
          "have requested to ignore.\n\nDo you wish to change this option and "
          "include these items in the analysis?"),
        _(L"Warning"),
        _(L"Prompt if a document should switch to include sentences in the analysis."),
        wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, false));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"ndc-proper-noun-conflict"),
        _(L"This test's proper-noun settings differ from the standard New Dale-Chall test.\n"
          "Do you wish to adjust this setting to match the standard test?"),
        _(L"Settings Conflict"),
        _(L"Prompt if a custom NDC test's proper noun settings "
          "differ from the standard NDC test."),
        wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, false));
    WarningManager::AddWarning(
        WarningMessage(_DT(L"new-dale-chall-text-exclusion-differs-note"), wxString{}, wxString{},
                       _(L"Prompt about New Dale-Chall using a different text "
                         "exclusion method from the system default."),
                       wxICON_INFORMATION, false));
    WarningManager::AddWarning(
        WarningMessage(_DT(L"harris-jacobson-text-exclusion-differs-note"), wxString{}, wxString{},
                       _(L"Prompt about Harris-Jacobson using a different text exclusion "
                         "method from the system default."),
                       wxICON_INFORMATION, false));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"custom-test-numeral-settings-adjustment-required"),
        _(L"Harris-Jacobson requires numerals to be excluded from the overall word count.\n"
          "Numeral options for this test will be adjusted to take this into account."),
        _(L"Warning"), _(L"Warn when a custom test's numeral settings will be adjusted."),
        wxOK | wxICON_INFORMATION, false));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"german-no-proper-noun-support"),
        _(L"Because German capitalizes all nouns, the program will be "
          "unable to detect proper nouns.\n"
          "Treatment of proper nouns as familiar words will be disabled for this test."),
        _(L"Warning"), _(L"Warn about German stemming not supporting proper noun detection."),
        wxOK | wxICON_INFORMATION, false));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"histogram-unique-values-midpoints-required"),
        _(L"Note: sorting histogram bins by unique values requires midpoint interval display.\n"
          "Midpoint interval display will be enabled."),
        wxString{}, _(L"Warn about unique-value histograms requiring midpoint axis labels."),
        wxOK | wxICON_INFORMATION, true));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"file-autosearch-from-project-directory"), wxString{}, _(L"File Not Found"),
        _(L"Prompt about auto-searching for missing files."), wxYES_NO | wxICON_QUESTION, false));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"linked-document-is-embedded"),
        _(L"This document is embedded in the project.\n"
          "Do you wish to link to the original document instead?"),
        _(L"Link Document"), _(L"Prompt about re-linking to a document that has been embedded."),
        wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION, false));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"no-embedded-text"),
        _(L"No embedded text found in the project. Project will not be created."), _(L"Warning"),
        _(L"Prompt about failing to load a project that is missing its embedded text."),
        wxOK | wxICON_EXCLAMATION, false));
    WarningManager::AddWarning(
        WarningMessage(_DT(L"clear-type-turned-off"),
                       _(L"ClearType is currently turned off. Enabling this will make fonts appear "
                         "smoother and easier to read.\n\n"
                         "Do you wish to enable ClearType?"),
                       _(L"Warning"), _(L"Check if ClearType is turned on (Windows only)."),
                       wxYES_NO | wxICON_QUESTION, false));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"note-export-from-save"),
        _(L"Any window can be exported by selecting \"Export\" from the \"Save\" button."),
        wxString{}, _(L"Prompt about how windows can be exported from the Save button."),
        wxICON_INFORMATION, true));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"click-test-to-view"), _(L"Double click a test to view more information."), wxString{},
        _(L"Prompt about how double-clicking a test can show its help."), wxICON_INFORMATION,
        true));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"bkimage-zoomin-noupscale"),
        _(L"When zooming, background images will not be stretched beyond their original "
          L"sizes."),
        wxString{},
        _(L"Prompt about how background images will not be upscaled beyond their "
          "original size when zooming into a graph."),
        wxICON_INFORMATION, true));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"note-project-properties"),
        _(L"Settings embedded in this project can be edited by "
          "clicking \"Home\" - \"Properties\"."),
        wxString{}, _(L"Prompt about how settings are embedded in projects and how to edit them."),
        wxICON_INFORMATION, true));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"batch-goals"),
        _(L"Documents not passing the project's goals are shown in this window.\n"
          "The recommended min and max values for each goal are displayed,\n"
          "along with an icon indicating whether the document is passing these constraints."),
        wxString{}, _(L"Prompt about how the Goals window works in a batch project."),
        wxICON_INFORMATION, true));
    WarningManager::AddWarning(WarningMessage(
        _DT(L"set-app-exclusion-list-from-project"),
        _(L"Would you like to use this word exclusion list for all future projects?"),
        _(L"Set Global Word Exclusion List"),
        _(L"Prompt about whether to set the application's word exclusion list from a project."),
        wxYES_NO | wxICON_QUESTION, false));
    }

//------------------------------------------------
void ReadabilityAppOptions::SetFonts()
    {
    auto systemFont{ wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT) };
    systemFont.SetFaceName(Wisteria::GraphItems::Label::GetFirstAvailableWordProcessorFont());
    m_editorFont = systemFont.Larger().Larger();
    m_xAxisFont = systemFont;
    m_yAxisFont = systemFont;
    m_topTitleFont = systemFont;
    m_bottomTitleFont = systemFont;
    m_leftTitleFont = systemFont;
    m_rightTitleFont = systemFont;
    m_textViewFont = systemFont.Larger().Larger();
    // fix font issues in case the system is using a hidden font for its default (happens on
    // macOS)
    Wisteria::GraphItems::Label::FixFont(m_editorFont);
    Wisteria::GraphItems::Label::FixFont(m_xAxisFont);
    Wisteria::GraphItems::Label::FixFont(m_yAxisFont);
    Wisteria::GraphItems::Label::FixFont(m_topTitleFont);
    Wisteria::GraphItems::Label::FixFont(m_bottomTitleFont);
    Wisteria::GraphItems::Label::FixFont(m_leftTitleFont);
    Wisteria::GraphItems::Label::FixFont(m_rightTitleFont);
    Wisteria::GraphItems::Label::FixFont(m_textViewFont);
    }

//------------------------------------------------
void ReadabilityAppOptions::SetColorsFromSystem()
    {
    m_fontColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    }

//------------------------------------------------
void ReadabilityAppOptions::ResetSettings()
    {
    SetFonts();

    // default exporting info
    BaseProjectDoc::SetExportTextViewExt(L"htm");
    BaseProjectDoc::SetExportListExt(L"htm");
    BaseProjectDoc::SetExportGraphExt(L"png");
    BaseProjectDoc::ExportHardWordLists(true);
    BaseProjectDoc::ExportSentencesBreakdown(true);
    BaseProjectDoc::ExportGraphs(true);
    BaseProjectDoc::ExportTestResults(true);
    BaseProjectDoc::ExportStatistics(true);
    BaseProjectDoc::ExportWordiness(true);
    BaseProjectDoc::ExportSightWords(true);
    BaseProjectDoc::ExportWarnings(true);
    BaseProjectDoc::ExportLists(true);
    BaseProjectDoc::ExportTextReports(true);

    m_appWindowMaximized = true;
    m_uiLanguage = UiLanguage::Default;
    // theme settings
    SetColorsFromSystem();

    m_editorFontColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    m_editorIndent = true;
    m_editorSpaceAfterNewlines = false;
    m_editorTextAlignment = wxTextAttrAlignment::wxTEXT_ALIGNMENT_JUSTIFIED;
    m_editorLineSpacing = wxTextAttrLineSpacing::wxTEXT_ATTR_LINE_SPACING_NORMAL;

    if (wxGetApp().GetLogFile() != nullptr)
        {
        LogFile::SetVerbose(false);
        }
    m_logAppendDailyLog = false;
    m_luaUnsafeMode = false;
    m_showDeveloperTab = true;
    m_showLogTab = false;
    m_logAutoRefresh = false;
    m_disableGpuAcceleration = false;

    m_textHighlight = TextHighlight::HighlightBackground;
    m_dolchConjunctionsColor = wxColour(255, 255, 0);
    m_dolchPrepositionsColor = wxColour(0, 245, 255);
    m_dolchPronounsColor = wxColour(198, 226, 255);
    m_dolchAdverbsColor = wxColour(0, 250, 154);
    m_dolchAdjectivesColor = wxColour(221, 160, 221);
    m_dolchVerbsColor = wxColour(254, 208, 112);
    m_dolchNounsColor = wxColour(255, 182, 193);
    m_highlightDolchConjunctions = true;
    m_highlightDolchPrepositions = true;
    m_highlightDolchPronouns = true;
    m_highlightDolchAdverbs = true;
    m_highlightDolchAdjectives = true;
    m_highlightDolchVerbs = true;
    m_highlightDolchNouns = false;
    m_textHighlightColor = wxColour(152, 251, 152);
    m_excludedTextHighlightColor = wxColour(175, 175, 175);
    m_duplicateWordHighlightColor = wxColour(255, 128, 128);
    m_wordyPhraseHighlightColor = wxColour(0, 255, 255);
    m_fontColor = wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT);
    m_textSource = TextSource::FromFile;
    m_batchGroupDefault = 2;
    m_longSentenceMethod = LongSentence::LongerThanSpecifiedLength;
    m_difficultSentenceLength = 22;
    m_numeralSyllabicationMethod = NumeralSyllabize::WholeWordIsOneSyllable;
    m_includeExcludedPhraseFirstOccurrence = false;
    m_paragraphsParsingMethod = ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs;
    m_ignoreBlankLinesForParagraphsParser = false;
    m_ignoreIndentingForParagraphsParser = false;
    m_sentenceStartMustBeUppercased = false;
    m_aggressiveExclusion = false;
    m_excludeTrailingCopyrightNoticeParagraphs = true;
    m_excludeTrailingCitations = true;
    m_excludeFileAddresses = false;
    m_excludeNumerals = false;
    m_excludeProperNouns = false;
    m_excludedPhrasesPath.clear();
    m_exclusionBlockTags.clear();
    m_invalidSentenceMethod = InvalidSentence::ExcludeFromAnalysis;
    m_includeIncompleteSentencesIfLongerThan = 15;
    m_spellcheck_ignore_proper_nouns = false;
    m_spellcheck_ignore_uppercased = true;
    m_spellcheck_ignore_numerals = true;
    m_spellcheck_ignore_file_addresses = true;
    m_spellcheck_ignore_programmer_code = false;
    m_allow_colloquialisms = true;
    for (auto& rTest : GetReadabilityTests().get_tests())
        {
        rTest.include(false);
        }
    m_includeDolchSightWords = false;
    m_testRecommendation = TestRecommendation::BasedOnDocumentType;
    m_testsByIndustry = readability::industry_classification::adult_publishing_industry;
    m_testsByDocumentType = readability::document_classification::adult_literature_document;
    GetReadabilityMessageCatalog().SetGradeScale(readability::grade_scale::k12_plus_united_states);
    GetReadabilityMessageCatalog().SetLongGradeScaleFormat(false);
    GetReadabilityMessageCatalog().SetReadingAgeDisplay(
        ReadabilityMessages::ReadingAgeDisplay::ReadingAgeAsARange);
    // document linking information
    m_documentStorageMethod = TextStorage::NoEmbedText;
    // internet
    m_userAgent = _DT(L"Mozilla/5.0 (") + wxGetOsDescription() + _DT(L") WebKit/12.0 WebLion");
    wxGetApp().GetWebHarvester().SetUserAgent(m_userAgent);
    m_disablePeerVerify = false;
    m_useJsCookies = false;
    m_persistJsCookies = false;
    wxGetApp().GetWebHarvester().DisablePeerVerify(m_disablePeerVerify);
    wxGetApp().GetWebHarvester().UseJavaScriptCookies(m_useJsCookies);
    wxGetApp().GetWebHarvester().PersistJavaScriptCookies(m_persistJsCookies);
    // graph information
    m_boxPlotShowAllPoints = false;
    m_boxDisplayLabels = false;
    m_boxConnectMiddlePoints = true;
    m_barDisplayLabels = true;
    m_useGraphBackGroundColorLinearGradient = false;
    m_displayDropShadows = false;
    m_showcaseKeyItems = false;
    m_plotBackGroundImagePath.clear();
    m_graphColorSchemeName = _DT(L"campfire");
    m_watermarkImg.clear();
    m_graphBackGroundColor = wxColour(255, 255, 255);
    m_plotBackGroundColor = wxColour(255, 255, 255);
    m_plotBackGroundImageOpacity = wxALPHA_OPAQUE;
    m_plotBackGroundColorOpacity = wxALPHA_TRANSPARENT;
    m_xAxisFontColor = wxColour(0, 0, 0);
    m_yAxisFontColor = wxColour(0, 0, 0);
    m_topTitleFontColor = wxColour(0, 0, 0);
    m_bottomTitleFontColor = wxColour(0, 0, 0);
    m_leftTitleFontColor = wxColour(0, 0, 0);
    m_rightTitleFontColor = wxColour(0, 0, 0);
    // honeydew
    m_graphInvalidAreaColor = wxColour(193, 205, 193);
    m_fleschChartConnectPoints = true;
    m_fleschChartSyllableRulerDocGroups = false;
    m_useEnglishLabelsGermanLix = false;
    m_histogramBinningMethod = Wisteria::Graphs::Histogram::BinningMethod::BinByIntegerRange;
    m_histogramBinLabelDisplayMethod = Wisteria::BinLabelDisplay::BinValue;
    m_histogramRoundingMethod = Wisteria::RoundingMethod::RoundDown;
    m_histogramIntervalDisplay = Wisteria::Graphs::Histogram::IntervalDisplay::Cutpoints;
    // lavender
    m_histogramBarColor = wxColour(182, 164, 204);
    // rain color
    m_barChartBarColor = wxColour(107, 183, 196);
    m_histogramBarOpacity = wxALPHA_OPAQUE;
    m_histogramBarEffect = Wisteria::BoxEffect::Solid;
    m_barChartOrientation = Wisteria::Orientation::Horizontal;
    m_graphBarOpacity = wxALPHA_OPAQUE;
    m_graphBarEffect = Wisteria::BoxEffect::Solid;
    m_stippleShape = DONTTRANSLATE(L"book");
    m_stippleColor = wxColour{ L"#6082B6" };
    m_stippleImagePath.clear();
    m_commonImagePath.clear();
    m_graphBoxColor = wxColour(0, 128, 64);
    m_graphBoxOpacity = wxALPHA_OPAQUE;
    m_graphBoxEffect = Wisteria::BoxEffect::Solid;
    m_plotBackgroundImageEffect = Wisteria::ImageEffect::NoEffect;
    m_plotBackgroundImageFit = Wisteria::ImageFit::Shrink;
    m_varianceMethod = VarianceMethod::PopulationVariance;
    GetStatisticsReportInfo().Reset();
    GetStatisticsInfo().Reset();
    GetGrammarInfo().EnableAll();
    GetWordsBreakdownInfo().EnableAll();
    GetSentencesBreakdownInfo().EnableAll();
    m_minDocWordCountForBatch = 50;
    m_randomSampleSizeForBatch = 15;
    m_filePathTruncationMode =
        Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode::OnlyShowFileNames;
    m_language = readability::test_language::english_test;
    // Keep reviewer to whatever it was before, don't reset that.
    // This isn't so much as setting, as an identifier for the user that they may be customized,
    // so leave it alone.
    m_realTimeUpdate = false;
    m_appendedDocumentFilePath.clear();
    // page setup
    m_paperId = wxPAPER_LETTER;
    m_paperOrientation = wxLANDSCAPE;
    // headers
    m_leftPrinterHeader.clear();
    m_centerPrinterHeader.clear();
    m_rightPrinterHeader.clear();
    // footers
    m_leftPrinterFooter.clear();
    m_centerPrinterFooter.clear();
    m_rightPrinterFooter.clear();

    m_watermark = Wisteria::Canvas::Watermark{};
    // test inclusion options
    m_includeScoreSummaryReport = true;
    // test options
    m_dcTextExclusion = SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings;
    m_hjTextExclusion = SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings;
    m_dcProperNounCountingMethod = readability::proper_noun_counting_method::
        only_count_first_instance_of_proper_noun_as_unfamiliar;
    m_includeStockerCatholicDCSupplement = false;
    m_fogUseSentenceUnits = true;
    m_fleschNumeralSyllabizeMethod = FleschNumeralSyllabize::NumeralIsOneSyllable;
    m_fleschKincaidNumeralSyllabizeMethod =
        FleschKincaidNumeralSyllabize::FleschKincaidNumeralSoundOutEachDigit;
    m_raygorStyle = Wisteria::Graphs::RaygorStyle::BaldwinKaufman;
    // clear the colors
    m_customColors.clear();
    // reset the warning flags
    WarningManager::EnableWarnings();
    }

//------------------------------------------------
void ReadabilityAppOptions::LoadThemeNode(tinyxml2::XMLElement* appearanceNode)
    {
    if (appearanceNode != nullptr)
        {
        const lily_of_the_valley::html_extract_text filterHtml;

        const int maximized =
            appearanceNode->ToElement()->IntAttribute(XML_WINDOW_MAXIMIZED.data(), 1);
        m_appWindowMaximized = int_to_bool(maximized);
        m_appWindowWidth = appearanceNode->ToElement()->IntAttribute(XML_WINDOW_WIDTH.data(), 800);
        m_appWindowHeight =
            appearanceNode->ToElement()->IntAttribute(XML_WINDOW_HEIGHT.data(), 700);
        // make sure the values make sense
        if (m_appWindowWidth < 1)
            {
            m_appWindowWidth = 800;
            }
        if (m_appWindowHeight < 1)
            {
            m_appWindowHeight = 700;
            }
        m_uiLanguage = static_cast<UiLanguage>(appearanceNode->ToElement()->IntAttribute(
            XML_UI_LANGUAGE.data(), static_cast<int>(UiLanguage::Default)));
        }
    }

//------------------------------------------------
void ReadabilityAppOptions::LoadWarningsNode(tinyxml2::XMLElement* configRootNode)
    {
    auto* warningSettingsNode =
        configRootNode->FirstChildElement(XML_WARNING_MESSAGE_SETTINGS.data());
    if (warningSettingsNode != nullptr)
        {
        auto* warningNode = warningSettingsNode->FirstChildElement(XML_WARNING_MESSAGE.data());
        while (warningNode != nullptr)
            {
            const char* warningStringId = warningNode->ToElement()->Attribute(XML_VALUE.data());
            if (warningStringId != nullptr)
                {
                auto id = Wisteria::TextStream::CharStreamToUnicode(warningStringId,
                                                                    std::strlen(warningStringId));
                auto warningIter = WarningManager::GetWarning(id);
                if (warningIter != WarningManager::GetWarnings().end())
                    {
                    int value = warningNode->ToElement()->IntAttribute(XML_DISPLAY.data(), 1);
                    warningIter->Show(int_to_bool(value));
                    value = warningNode->ToElement()->IntAttribute(XML_PREVIOUS_RESPONSE.data(), 0);
                    warningIter->SetPreviousResponse(value);
                    }
                }
            warningNode = warningNode->NextSiblingElement(XML_WARNING_MESSAGE.data());
            }
        }
    }

//------------------------------------------------
void ReadabilityAppOptions::LoadExportNode(tinyxml2::XMLElement* configRootNode)
    {
    auto* exportSettingsNode = configRootNode->FirstChildElement(XML_EXPORT.data());
    if (exportSettingsNode != nullptr)
        {
        auto* exportExtNode = exportSettingsNode->FirstChildElement(XML_EXPORT_LIST_EXT.data());
        if (exportExtNode != nullptr)
            {
            const char* extString = exportExtNode->ToElement()->Attribute(XML_VALUE.data());
            if (extString != nullptr)
                {
                BaseProjectDoc::SetExportListExt(
                    Wisteria::TextStream::CharStreamToUnicode(extString, std::strlen(extString)));
                }
            }
        exportExtNode = exportSettingsNode->FirstChildElement(XML_EXPORT_TEXT_EXT.data());
        if (exportExtNode != nullptr)
            {
            const char* extString = exportExtNode->ToElement()->Attribute(XML_VALUE.data());
            if (extString != nullptr)
                {
                BaseProjectDoc::SetExportTextViewExt(
                    Wisteria::TextStream::CharStreamToUnicode(extString, std::strlen(extString)));
                }
            }
        exportExtNode = exportSettingsNode->FirstChildElement(XML_EXPORT_GRAPH_EXT.data());
        if (exportExtNode != nullptr)
            {
            const char* extString = exportExtNode->ToElement()->Attribute(XML_VALUE.data());
            if (extString != nullptr)
                {
                BaseProjectDoc::SetExportGraphExt(
                    Wisteria::TextStream::CharStreamToUnicode(extString, std::strlen(extString)));
                }
            }
        auto* exportNode = exportSettingsNode->FirstChildElement(XML_EXPORT_LISTS.data());
        if (exportNode != nullptr)
            {
            BaseProjectDoc::ExportHardWordLists(
                int_to_bool(exportNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
            }
        exportNode = exportSettingsNode->FirstChildElement(XML_EXPORT_SENTENCES_BREAKDOWN.data());
        if (exportNode != nullptr)
            {
            BaseProjectDoc::ExportSentencesBreakdown(
                int_to_bool(exportNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
            }
        exportNode = exportSettingsNode->FirstChildElement(XML_EXPORT_GRAPHS.data());
        if (exportNode != nullptr)
            {
            BaseProjectDoc::ExportGraphs(
                int_to_bool(exportNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
            }
        exportNode = exportSettingsNode->FirstChildElement(XML_EXPORT_TEST_RESULTS.data());
        if (exportNode != nullptr)
            {
            BaseProjectDoc::ExportTestResults(
                int_to_bool(exportNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
            }
        exportNode = exportSettingsNode->FirstChildElement(XML_EXPORT_STATS.data());
        if (exportNode != nullptr)
            {
            BaseProjectDoc::ExportStatistics(
                int_to_bool(exportNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
            }
        exportNode = exportSettingsNode->FirstChildElement(XML_EXPORT_GRAMMAR.data());
        if (exportNode != nullptr)
            {
            BaseProjectDoc::ExportWordiness(
                int_to_bool(exportNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
            }
        exportNode = exportSettingsNode->FirstChildElement(XML_EXPORT_DOLCH_WORDS.data());
        if (exportNode != nullptr)
            {
            BaseProjectDoc::ExportSightWords(
                int_to_bool(exportNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
            }
        exportNode = exportSettingsNode->FirstChildElement(XML_EXPORT_WARNINGS.data());
        if (exportNode != nullptr)
            {
            BaseProjectDoc::ExportWarnings(
                int_to_bool(exportNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
            }
        }
    }

//------------------------------------------------
void ReadabilityAppOptions::LoadInternetNode(tinyxml2::XMLElement* configRootNode)
    {
    auto* userAgentNode = configRootNode->FirstChildElement(XML_USER_AGENT.data());
    if (userAgentNode != nullptr)
        {
        const wxString userAgent =
            TiXmlNodeAttributeToString(userAgentNode, XML_VALUE.data(), GetUserAgent());
        SetUserAgent(userAgent);
        wxGetApp().GetWebHarvester().SetUserAgent(userAgent);
        }

    auto* downloadReplaceExistingNode =
        configRootNode->FirstChildElement(XML_DOWNLOAD_REPLACE_EXISTING.data());
    if (downloadReplaceExistingNode != nullptr)
        {
        wxGetApp().GetWebHarvester().ReplaceExistingFiles(
            int_to_bool(downloadReplaceExistingNode->ToElement()->IntAttribute(
                XML_VALUE.data(),
                bool_to_int(wxGetApp().GetWebHarvester().IsReplacingExistingFiles()))));
        }

    auto* downloadWebFolderStructureNode =
        configRootNode->FirstChildElement(XML_DOWNLOAD_KEEP_FOLDER_STRUCTURE.data());
    if (downloadWebFolderStructureNode != nullptr)
        {
        wxGetApp().GetWebHarvester().KeepWebPathWhenDownloading(
            int_to_bool(downloadWebFolderStructureNode->ToElement()->IntAttribute(
                XML_VALUE.data(),
                bool_to_int(wxGetApp().GetWebHarvester().IsKeepingWebPathWhenDownloading()))));
        }

    auto* downloadMinFileSizeNode =
        configRootNode->FirstChildElement(XML_DOWNLOAD_MIN_FILESIZE.data());
    if (downloadMinFileSizeNode != nullptr)
        {
        wxGetApp().GetWebHarvester().SetMinimumDownloadFileSizeInKilobytes(
            downloadMinFileSizeNode->ToElement()->IntAttribute(XML_VALUE.data(), 0));
        }

    auto* useJsCookiesNode = configRootNode->FirstChildElement(XML_USE_JS_COOKIES.data());
    if (useJsCookiesNode != nullptr)
        {
        const int value = useJsCookiesNode->ToElement()->IntAttribute(XML_VALUE.data(), 0);
        m_useJsCookies = int_to_bool(value);
        wxGetApp().GetWebHarvester().UseJavaScriptCookies(m_useJsCookies);
        }

    auto* persistCookiesNode = configRootNode->FirstChildElement(XML_PERSIST_COOKIES.data());
    if (persistCookiesNode != nullptr)
        {
        const int value = persistCookiesNode->ToElement()->IntAttribute(XML_VALUE.data(), 0);
        m_persistJsCookies = int_to_bool(value);
        wxGetApp().GetWebHarvester().PersistJavaScriptCookies(m_persistJsCookies);
        }

    auto* disablePeerVerifyNode = configRootNode->FirstChildElement(XML_DISABLE_PEER_VERIFY.data());
    if (disablePeerVerifyNode != nullptr)
        {
        const int value = disablePeerVerifyNode->ToElement()->IntAttribute(XML_VALUE.data(), 0);
        m_disablePeerVerify = int_to_bool(value);
        wxGetApp().GetWebHarvester().DisablePeerVerify(m_disablePeerVerify);
        }
    }

//------------------------------------------------
void ReadabilityAppOptions::LoadTestBundlesNode(tinyxml2::XMLElement* projectSettings)
    {
    auto* testBundlesNode = projectSettings->FirstChildElement(XML_TEST_BUNDLES.data());
    if (testBundlesNode != nullptr)
        {
        auto* testBundleNode = testBundlesNode->FirstChildElement(XML_TEST_BUNDLE.data());
        while (testBundleNode != nullptr)
            {
            // bundle name
            auto* bundleNameNode = testBundleNode->FirstChildElement(XML_TEST_BUNDLE_NAME.data());
            const wxString bundleName =
                TiXmlNodeAttributeToString(bundleNameNode, XML_VALUE.data());
            if (bundleName.empty())
                {
                testBundleNode = testBundleNode->NextSiblingElement(XML_TEST_BUNDLE.data());
                continue;
                }
            TestBundle bundle(bundleName.wc_str());
            // bundle description
            auto* bundleDescriptionNode =
                testBundleNode->FirstChildElement(XML_TEST_BUNDLE_DESCRIPTION.data());
            if (bundleDescriptionNode != nullptr)
                {
                bundle.SetDescription(
                    TiXmlNodeAttributeToString(bundleDescriptionNode, XML_VALUE.data()).wc_str());
                }
            // get the included tests
            auto* testNamesNode = testBundleNode->FirstChildElement(XML_TEST_NAMES.data());
            if (testNamesNode == nullptr)
                {
                testBundleNode = testBundleNode->NextSiblingElement(XML_TEST_BUNDLE.data());
                continue;
                }
            auto* testNameNode = testNamesNode->FirstChildElement(XML_TEST_NAME.data());
            while (testNameNode != nullptr)
                {
                const wxString testName =
                    TiXmlNodeAttributeToString(testNameNode, XML_VALUE.data());
                if (!testName.empty())
                    {
                    const auto minGoal =
                        TiXmlNodeToDouble(testNameNode, XML_GOAL_MIN_VAL_GOAL.data());
                    const auto maxGoal =
                        TiXmlNodeToDouble(testNameNode, XML_GOAL_MAX_VAL_GOAL.data());
                    bundle.GetTestGoals().insert({ testName.wc_str(), minGoal, maxGoal });
                    }
                testNameNode = testNameNode->NextSiblingElement(XML_TEST_NAME.data());
                }
            // get the included stats
            auto* statsNode = testBundleNode->FirstChildElement(XML_BUNDLE_STATISTICS.data());
            if (statsNode == nullptr)
                {
                testBundleNode = testBundleNode->NextSiblingElement(XML_TEST_BUNDLE.data());
                continue;
                }
            auto* statNode = statsNode->FirstChildElement(XML_BUNDLE_STATISTIC.data());
            while (statNode != nullptr)
                {
                const wxString statName = TiXmlNodeAttributeToString(statNode, XML_VALUE.data());
                if (!statName.empty())
                    {
                    auto minGoal = TiXmlNodeToDouble(statNode, XML_GOAL_MIN_VAL_GOAL.data());
                    auto maxGoal = TiXmlNodeToDouble(statNode, XML_GOAL_MAX_VAL_GOAL.data());
                    bundle.GetStatGoals().insert({ statName.wc_str(), minGoal, maxGoal });
                    }
                statNode = statNode->NextSiblingElement(XML_BUNDLE_STATISTIC.data());
                }
            BaseProject::m_testBundles.insert(bundle);
            dynamic_cast<MainFrame*>(wxGetApp().GetMainFrame())->AddTestBundleToMenus(bundleName);

            testBundleNode = testBundleNode->NextSiblingElement(XML_TEST_BUNDLE.data());
            }
        }
    }

//------------------------------------------------
void ReadabilityAppOptions::LoadCustomTestsNode(tinyxml2::XMLElement* projectSettings)
    {
    lily_of_the_valley::html_extract_text filterHtml;
    auto* customTestsNode = projectSettings->FirstChildElement(XML_CUSTOM_TESTS.data());
    if (customTestsNode != nullptr)
        {
        auto* customReadabilityTestNode =
            customTestsNode->FirstChildElement(XML_CUSTOM_FAMILIAR_WORD_TEST.data());
        while (customReadabilityTestNode != nullptr)
            {
            // test name
            auto* testNameNode = customReadabilityTestNode->FirstChildElement(XML_TEST_NAME.data());
            const char* testNameData = testNameNode->ToElement()->Attribute(XML_VALUE.data());
            if (testNameData == nullptr)
                {
                customReadabilityTestNode = customReadabilityTestNode->NextSiblingElement(
                    XML_CUSTOM_FAMILIAR_WORD_TEST.data());
                continue;
                }
            auto testNameStr =
                Wisteria::TextStream::CharStreamToUnicode(testNameData, std::strlen(testNameData));
            const wchar_t* filteredText =
                filterHtml(testNameStr.c_str(), testNameStr.length(), true, false);
            if (filteredText == nullptr)
                {
                customReadabilityTestNode = customReadabilityTestNode->NextSiblingElement(
                    XML_CUSTOM_FAMILIAR_WORD_TEST.data());
                continue;
                }
            const wxString testName{ filteredText };
            // file path
            wxString filePath;
            auto* filePathNode =
                customReadabilityTestNode->FirstChildElement(XML_FAMILIAR_WORD_FILE_PATH.data());
            if (filePathNode != nullptr)
                {
                const char* filePathData = filePathNode->ToElement()->Attribute(XML_VALUE.data());
                if (filePathData != nullptr)
                    {
                    auto filePathStr = Wisteria::TextStream::CharStreamToUnicode(
                        filePathData, std::strlen(filePathData));
                    filteredText =
                        filterHtml(filePathStr.c_str(), filePathStr.length(), true, false);
                    if (filteredText != nullptr)
                        {
                        filePath = filteredText;
                        }
                    }
                }
            // test type
            int testType{ 0 };
            auto* testTypeNode = customReadabilityTestNode->FirstChildElement(XML_TEST_TYPE.data());
            if (testTypeNode != nullptr)
                {
                testType = testTypeNode->ToElement()->IntAttribute(XML_VALUE.data(), testType);
                }
            if (testType < 0 ||
                testType >= static_cast<int>(readability::readability_test_type::TEST_TYPE_COUNT))
                {
                testType = 0;
                }
            // stemming type
            int stemmingType{ 0 };
            auto* stemmingNode =
                customReadabilityTestNode->FirstChildElement(XML_STEMMING_TYPE.data());
            if (stemmingNode != nullptr)
                {
                stemmingType = stemmingNode->ToElement()->IntAttribute(XML_VALUE.data(), 0);
                }
            if (stemmingType < 0 ||
                stemmingType >= static_cast<int>(stemming::stemming_type::STEMMING_TYPE_COUNT))
                {
                stemmingType = 0;
                }
            // formula type (for backward compatibility)
            int formulaType = 0;
            auto* formulaNode =
                customReadabilityTestNode->FirstChildElement(XML_TEST_FORMULA_TYPE.data());
            if (formulaNode != nullptr)
                {
                formulaType = formulaNode->ToElement()->IntAttribute(XML_VALUE.data(), 0);
                }
            if (formulaType != 0 && formulaType != 1)
                {
                formulaType = 0;
                }
            // the formula
            wxString formula;
            formulaNode = customReadabilityTestNode->FirstChildElement(XML_TEST_FORMULA.data());
            if (formulaNode != nullptr)
                {
                const char* formulaData = formulaNode->ToElement()->Attribute(XML_VALUE.data());
                if (formulaData != nullptr)
                    {
                    auto formulaStr = Wisteria::TextStream::CharStreamToUnicode(
                        formulaData, std::strlen(formulaData));
                    filteredText = filterHtml(formulaStr.c_str(), formulaStr.length(), true, false);
                    if (filteredText != nullptr)
                        {
                        // need to format formula from U.S. format to current locale format
                        // for the parser to understand it
                        formula = FormulaFormat::FormatMathExpressionFromUS(filteredText);
                        }
                    }
                }
            string_util::remove_blank_lines(formula);
            // formula string not in the file, so fall back to old formula type value
            if (formula.empty())
                {
                if (formulaType == 1)
                    {
                    formula = ReadabilityFormulaParser::GetCustomSpacheSignature();
                    }
                else
                    {
                    formula = ReadabilityFormulaParser::GetCustomNewDaleChallSignature();
                    }
                }
            // whether proper nouns and numbers should be included
            int includeProperNouns = 1;
            bool includeNumeric = false;
            auto* properNounNode =
                customReadabilityTestNode->FirstChildElement(XML_INCLUDE_PROPER_NOUNS.data());
            if (properNounNode != nullptr)
                {
                includeProperNouns =
                    properNounNode->ToElement()->IntAttribute(XML_VALUE.data(), includeProperNouns);
                }
            auto* numericNode =
                customReadabilityTestNode->FirstChildElement(XML_INCLUDE_NUMERIC.data());
            if (numericNode != nullptr)
                {
                includeNumeric =
                    int_to_bool(numericNode->ToElement()->IntAttribute(XML_VALUE.data(), 0));
                }
            // whether DC and Spache lists should also be included with this test
            bool includeCustomWordList = true /* best for backward compatibility */,
                 includeDCTest = false, includeSpacheTest = false, includeHJList = false,
                 includeStockerList = false, familiarWordsMustBeOnAllLists = false;
            auto* otherTestNode =
                customReadabilityTestNode->FirstChildElement(XML_INCLUDE_CUSTOM_WORD_LIST.data());
            if (otherTestNode != nullptr)
                {
                includeCustomWordList =
                    int_to_bool(otherTestNode->ToElement()->IntAttribute(XML_VALUE.data(), 0));
                }
            otherTestNode =
                customReadabilityTestNode->FirstChildElement(XML_INCLUDE_DC_LIST.data());
            if (otherTestNode != nullptr)
                {
                includeDCTest =
                    int_to_bool(otherTestNode->ToElement()->IntAttribute(XML_VALUE.data(), 0));
                }
            otherTestNode =
                customReadabilityTestNode->FirstChildElement(XML_INCLUDE_SPACHE_LIST.data());
            if (otherTestNode != nullptr)
                {
                includeSpacheTest =
                    int_to_bool(otherTestNode->ToElement()->IntAttribute(XML_VALUE.data(), 0));
                }
            otherTestNode = customReadabilityTestNode->FirstChildElement(
                XML_INCLUDE_HARRIS_JACOBSON_LIST.data());
            if (otherTestNode != nullptr)
                {
                includeHJList =
                    int_to_bool(otherTestNode->ToElement()->IntAttribute(XML_VALUE.data(), 0));
                }
            otherTestNode =
                customReadabilityTestNode->FirstChildElement(XML_INCLUDE_STOCKER_LIST.data());
            if (otherTestNode != nullptr)
                {
                includeStockerList =
                    int_to_bool(otherTestNode->ToElement()->IntAttribute(XML_VALUE.data(), 0));
                }
            auto* familiarWordsMustBeOnAllListsNode =
                customReadabilityTestNode->FirstChildElement(XML_FAMILIAR_WORDS_ALL_LISTS.data());
            if (familiarWordsMustBeOnAllListsNode != nullptr)
                {
                familiarWordsMustBeOnAllLists =
                    int_to_bool(familiarWordsMustBeOnAllListsNode->ToElement()->IntAttribute(
                        XML_VALUE.data(), 0));
                }
            // industry
            bool industryChildrensPublishingSelected = false;
            auto* industryNode = customReadabilityTestNode->FirstChildElement(
                XML_INDUSTRY_CHILDRENS_PUBLISHING.data());
            if (industryNode != nullptr)
                {
                industryChildrensPublishingSelected =
                    int_to_bool(industryNode->ToElement()->IntAttribute(XML_VALUE.data(), 0));
                }
            bool industryAdultPublishingSelected = false;
            industryNode =
                customReadabilityTestNode->FirstChildElement(XML_INDUSTRY_ADULTPUBLISHING.data());
            if (industryNode != nullptr)
                {
                industryAdultPublishingSelected =
                    int_to_bool(industryNode->ToElement()->IntAttribute(XML_VALUE.data(), 0));
                }
            bool industrySecondaryLanguageSelected = false;
            industryNode = customReadabilityTestNode->FirstChildElement(
                XML_INDUSTRY_SECONDARY_LANGUAGE.data());
            if (industryNode != nullptr)
                {
                industrySecondaryLanguageSelected =
                    int_to_bool(industryNode->ToElement()->IntAttribute(XML_VALUE.data(), 0));
                }
            bool industryChildrensHealthCareSelected = false;
            industryNode = customReadabilityTestNode->FirstChildElement(
                XML_INDUSTRY_CHILDRENS_HEALTHCARE.data());
            if (industryNode != nullptr)
                {
                industryChildrensHealthCareSelected =
                    int_to_bool(industryNode->ToElement()->IntAttribute(XML_VALUE.data(), 0));
                }
            bool industryAdultHealthCareSelected = false;
            industryNode =
                customReadabilityTestNode->FirstChildElement(XML_INDUSTRY_ADULT_HEALTHCARE.data());
            if (industryNode != nullptr)
                {
                industryAdultHealthCareSelected =
                    int_to_bool(industryNode->ToElement()->IntAttribute(XML_VALUE.data(), 0));
                }
            bool industryMilitaryGovernmentSelected = false;
            industryNode = customReadabilityTestNode->FirstChildElement(
                XML_INDUSTRY_MILITARY_GOVERNMENT.data());
            if (industryNode != nullptr)
                {
                industryMilitaryGovernmentSelected =
                    int_to_bool(industryNode->ToElement()->IntAttribute(XML_VALUE.data(), 0));
                }
            bool industryBroadcastingSelected = false;
            industryNode =
                customReadabilityTestNode->FirstChildElement(XML_INDUSTRY_BROADCASTING.data());
            if (industryNode != nullptr)
                {
                industryBroadcastingSelected = int_to_bool(industryNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), static_cast<int>(industryBroadcastingSelected)));
                }
            // document
            bool documentGeneralSelected = false;
            auto* documentNode =
                customReadabilityTestNode->FirstChildElement(XML_DOCUMENT_GENERAL.data());
            if (documentNode != nullptr)
                {
                documentGeneralSelected = int_to_bool(documentNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), static_cast<int>(documentGeneralSelected)));
                }
            bool documentTechSelected = false;
            documentNode =
                customReadabilityTestNode->FirstChildElement(XML_DOCUMENT_TECHNICAL.data());
            if (documentNode != nullptr)
                {
                documentTechSelected = int_to_bool(documentNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), static_cast<int>(documentTechSelected)));
                }
            bool documentFormSelected = false;
            documentNode = customReadabilityTestNode->FirstChildElement(XML_DOCUMENT_FORM.data());
            if (documentNode != nullptr)
                {
                documentFormSelected = int_to_bool(documentNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), static_cast<int>(documentFormSelected)));
                }
            bool documentYoungAdultSelected = false;
            documentNode =
                customReadabilityTestNode->FirstChildElement(XML_DOCUMENT_YOUNGADULT.data());
            if (documentNode != nullptr)
                {
                documentYoungAdultSelected = int_to_bool(documentNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), static_cast<int>(documentYoungAdultSelected)));
                }
            bool documentChildrenSelected = false;
            documentNode =
                customReadabilityTestNode->FirstChildElement(XML_DOCUMENT_CHILDREN_LIT.data());
            if (documentNode != nullptr)
                {
                documentChildrenSelected = int_to_bool(documentNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), static_cast<int>(documentChildrenSelected)));
                }
            CustomReadabilityTest cTest(
                testName.wc_str(), formula.wc_str(),
                static_cast<readability::readability_test_type>(testType), filePath.wc_str(),
                static_cast<stemming::stemming_type>(stemmingType), includeCustomWordList,
                includeDCTest, &BaseProject::m_dale_chall_word_list, includeSpacheTest,
                &BaseProject::m_spache_word_list, includeHJList,
                &BaseProject::m_harris_jacobson_word_list, includeStockerList,
                &BaseProject::m_stocker_catholic_word_list, familiarWordsMustBeOnAllLists,
                static_cast<readability::proper_noun_counting_method>(includeProperNouns),
                includeNumeric, industryChildrensPublishingSelected,
                industryAdultPublishingSelected, industrySecondaryLanguageSelected,
                industryChildrensHealthCareSelected, industryAdultHealthCareSelected,
                industryMilitaryGovernmentSelected, industryBroadcastingSelected,
                documentGeneralSelected, documentTechSelected, documentFormSelected,
                documentYoungAdultSelected, documentChildrenSelected);
            BaseProjectDoc::AddGlobalCustomReadabilityTest(cTest);

            customReadabilityTestNode =
                customReadabilityTestNode->NextSiblingElement(XML_CUSTOM_FAMILIAR_WORD_TEST.data());
            }
        }
    }

//------------------------------------------------
void ReadabilityAppOptions::LoadStatsNode(tinyxml2::XMLElement* projectSettings)
    {
    lily_of_the_valley::html_extract_text filterHtml;
    auto* statsDefaultsNode = projectSettings->FirstChildElement(XML_STATISTICS_SECTION.data());
    if (statsDefaultsNode != nullptr)
        {
        // variance method
        auto* varianceMethod = statsDefaultsNode->FirstChildElement(XML_VARIANCE_METHOD.data());
        if (varianceMethod != nullptr)
            {
            SetVarianceMethod(static_cast<VarianceMethod>(varianceMethod->ToElement()->IntAttribute(
                XML_METHOD.data(), static_cast<int>(GetVarianceMethod()))));
            }
        // stats report
        auto* statsNode = statsDefaultsNode->FirstChildElement(XML_STATISTICS_RESULTS.data());
        if (statsNode != nullptr)
            {
            const char* statsChars = statsNode->ToElement()->Attribute(XML_VALUE.data());
            if (statsChars != nullptr)
                {
                const auto statsStr =
                    Wisteria::TextStream::CharStreamToUnicode(statsChars, std::strlen(statsChars));
                const wchar_t* convertedStr =
                    filterHtml(statsStr.c_str(), statsStr.length(), true, false);
                if (convertedStr != nullptr)
                    {
                    GetStatisticsInfo().Set(convertedStr);
                    }
                }
            }
        // stats report
        auto* statsReportNode = statsDefaultsNode->FirstChildElement(XML_STATISTICS_REPORT.data());
        if (statsReportNode != nullptr)
            {
            const char* statsReportChars =
                statsReportNode->ToElement()->Attribute(XML_VALUE.data());
            if (statsReportChars != nullptr)
                {
                const auto statsReportStr = Wisteria::TextStream::CharStreamToUnicode(
                    statsReportChars, std::strlen(statsReportChars));
                const wchar_t* convertedStr =
                    filterHtml(statsReportStr.c_str(), statsReportStr.length(), true, false);
                if (convertedStr != nullptr)
                    {
                    GetStatisticsReportInfo().Set(convertedStr);
                    }
                }
            }
        }
    }

//------------------------------------------------
void ReadabilityAppOptions::LoadGraphsNode(tinyxml2::XMLElement* projectSettings)
    {
    lily_of_the_valley::html_extract_text filterHtml;
    auto* graphDefaultsNode = projectSettings->FirstChildElement(XML_GRAPH_SETTINGS.data());
    if (graphDefaultsNode != nullptr)
        {
        // graph backgrounds
        SetPlotBackGroundImagePath(TiXmlNodeAttributeToString(
            graphDefaultsNode->FirstChildElement(XML_GRAPH_PLOT_BACKGROUND_IMAGE_PATH.data()),
            XML_VALUE.data()));
        // color scheme
        SetGraphColorScheme(TiXmlNodeAttributeToString(
            graphDefaultsNode->FirstChildElement(XML_GRAPH_COLOR_SCHEME.data()), XML_VALUE.data(),
            GetGraphColorScheme()));
        // graph background colors
        SetBackGroundColor(TiXmlNodeToColor(
            graphDefaultsNode->FirstChildElement(XML_GRAPH_BACKGROUND_COLOR.data()),
            GetBackGroundColor()));
        auto* backgroundImageEffectNode =
            graphDefaultsNode->FirstChildElement(XML_GRAPH_PLOT_BACKGROUND_IMAGE_EFFECT.data());
        if (backgroundImageEffectNode != nullptr)
            {
            int value = backgroundImageEffectNode->ToElement()->IntAttribute(
                XML_VALUE.data(), static_cast<int>(GetPlotBackGroundImageEffect()));
            if (value < 0 ||
                value >= static_cast<decltype(value)>(Wisteria::ImageEffect::IMAGE_EFFECTS_COUNT))
                {
                value = static_cast<decltype(value)>(Wisteria::ImageEffect::NoEffect);
                }
            SetPlotBackGroundImageEffect(static_cast<Wisteria::ImageEffect>(value));
            }
        auto* backgroundImageFitNode =
            graphDefaultsNode->FirstChildElement(XML_GRAPH_PLOT_BACKGROUND_IMAGE_FIT.data());
        if (backgroundImageFitNode != nullptr)
            {
            int value = backgroundImageFitNode->ToElement()->IntAttribute(
                XML_VALUE.data(), static_cast<int>(GetPlotBackGroundImageFit()));
            if (value < 0 ||
                value >= static_cast<decltype(value)>(Wisteria::ImageFit::IMAGE_FIT_COUNT))
                {
                value = static_cast<decltype(value)>(Wisteria::ImageFit::Shrink);
                }
            SetPlotBackGroundImageFit(static_cast<Wisteria::ImageFit>(value));
            }

        SetPlotBackGroundColor(TiXmlNodeToColor(
            graphDefaultsNode->FirstChildElement(XML_GRAPH_PLOT_BACKGROUND_COLOR.data()),
            GetPlotBackGroundColor()));

        auto* opacityNode =
            graphDefaultsNode->FirstChildElement(XML_GRAPH_PLOT_BACKGROUND_IMAGE_OPACITY.data());
        if (opacityNode != nullptr)
            {
            SetPlotBackGroundImageOpacity(
                static_cast<uint8_t>(opacityNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), GetPlotBackGroundImageOpacity())));
            }
        opacityNode =
            graphDefaultsNode->FirstChildElement(XML_GRAPH_PLOT_BACKGROUND_COLOR_OPACITY.data());
        if (opacityNode != nullptr)
            {
            SetPlotBackGroundColorOpacity(
                static_cast<uint8_t>(opacityNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), GetPlotBackGroundColorOpacity())));
            }
        // linear gradient of backgrounds
        auto* gradientNode =
            graphDefaultsNode->FirstChildElement(XML_GRAPH_BACKGROUND_LINEAR_GRADIENT.data());
        if (gradientNode != nullptr)
            {
            SetGraphBackGroundLinearGradient(int_to_bool(gradientNode->ToElement()->IntAttribute(
                XML_VALUE.data(), bool_to_int(GetGraphBackGroundLinearGradient()))));
            }
        // stipple image
        SetStippleImagePath(TiXmlNodeAttributeToString(
            graphDefaultsNode->FirstChildElement(XML_GRAPH_STIPPLE_PATH.data()), XML_VALUE.data()));
        // common image
        SetGraphCommonImagePath(TiXmlNodeAttributeToString(
            graphDefaultsNode->FirstChildElement(XML_GRAPH_COMMON_IMAGE_PATH.data()),
            XML_VALUE.data()));
        // stipple shape
        SetStippleShape(TiXmlNodeAttributeToString(
            graphDefaultsNode->FirstChildElement(XML_GRAPH_STIPPLE_SHAPE.data()), XML_VALUE.data(),
            GetStippleShape()));
        // stipple color
        SetStippleShapeColor(
            TiXmlNodeToColor(graphDefaultsNode->FirstChildElement(XML_GRAPH_STIPPLE_COLOR.data()),
                             GetStippleShapeColor()));

        // whether drop shadows should be shown
        auto* dropShadowNode = graphDefaultsNode->FirstChildElement(XML_DISPLAY_DROP_SHADOW.data());
        if (dropShadowNode != nullptr)
            {
            DisplayDropShadows(int_to_bool(dropShadowNode->ToElement()->IntAttribute(
                XML_VALUE.data(), bool_to_int(IsDisplayingDropShadows()))));
            }
        // whether to draw attention to the complex word groups in syllable graphs
        auto* showcaseComplexWordsNode =
            graphDefaultsNode->FirstChildElement(XML_SHOWCASE_KEY_ITEMS.data());
        if (showcaseComplexWordsNode != nullptr)
            {
            ShowcaseKeyItems(int_to_bool(showcaseComplexWordsNode->ToElement()->IntAttribute(
                XML_VALUE.data(), bool_to_int(IsShowcasingKeyItems()))));
            }
        // watermark used to be stored with the graphs info, so read that legacy value
        // if upgrading a settings file
        wxString legacyWatermark = TiXmlNodeAttributeToString(
            graphDefaultsNode->FirstChildElement(XML_GRAPH_WATERMARK.data()), XML_VALUE.data());
        if (!legacyWatermark.empty())
            {
            auto currentWaterMark = GetWatermark();
            currentWaterMark.m_label = std::move(legacyWatermark);
            SetWatermark(currentWaterMark);
            }

        SetWatermarkLogo(TiXmlNodeAttributeToString(
            graphDefaultsNode->FirstChildElement(XML_GRAPH_WATERMARK_LOGO_IMAGE_PATH.data()),
            XML_VALUE.data()));
        // histogram settings
        auto* histogramNode = graphDefaultsNode->FirstChildElement(XML_HISTOGRAM_SETTINGS.data());
        if (histogramNode != nullptr)
            {
            auto* catNode = histogramNode->FirstChildElement(XML_GRAPH_BINNING_METHOD.data());
            if (catNode != nullptr)
                {
                int value = catNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), static_cast<int>(GetHistogramBinningMethod()));
                if (value < 0 ||
                    value >= static_cast<decltype(value)>(
                                 Wisteria::Graphs::Histogram::BinningMethod::BINNING_METHOD_COUNT))
                    {
                    value = static_cast<decltype(value)>(
                        Wisteria::Graphs::Histogram::BinningMethod::BinByIntegerRange);
                    }
                SetHistogramBinningMethod(
                    static_cast<Wisteria::Graphs::Histogram::BinningMethod>(value));
                }
            auto* roundNode = histogramNode->FirstChildElement(XML_GRAPH_ROUNDING_METHOD.data());
            if (roundNode != nullptr)
                {
                int value = roundNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), static_cast<decltype(value)>(GetHistogramRoundingMethod()));
                if (value < 0 || value >= static_cast<decltype(value)>(
                                              Wisteria::RoundingMethod::ROUNDING_METHOD_COUNT))
                    {
                    value = static_cast<decltype(value)>(Wisteria::RoundingMethod::RoundDown);
                    }
                SetHistogramRoundingMethod(static_cast<Wisteria::RoundingMethod>(value));
                }
            auto* intervalNode =
                histogramNode->FirstChildElement(XML_GRAPH_INTERVAL_DISPLAY.data());
            if (intervalNode != nullptr)
                {
                int value = intervalNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), static_cast<decltype(value)>(GetHistogramIntervalDisplay()));
                if (value < 0 ||
                    value >=
                        static_cast<decltype(value)>(
                            Wisteria::Graphs::Histogram::IntervalDisplay::INTERVAL_METHOD_COUNT))
                    {
                    value = static_cast<decltype(value)>(
                        Wisteria::Graphs::Histogram::IntervalDisplay::Cutpoints);
                    }
                SetHistogramIntervalDisplay(
                    static_cast<Wisteria::Graphs::Histogram::IntervalDisplay>(value));
                }
            auto* catLabelNode =
                histogramNode->FirstChildElement(XML_GRAPH_BINNING_LABEL_DISPLAY.data());
            if (catLabelNode != nullptr)
                {
                int value = catLabelNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), static_cast<decltype(value)>(GetHistogramBinLabelDisplay()));
                if (value < 0 || value >= static_cast<decltype(value)>(
                                              Wisteria::BinLabelDisplay::BIN_LABEL_DISPLAY_COUNT))
                    {
                    value = static_cast<decltype(value)>(Wisteria::BinLabelDisplay::BinValue);
                    }
                SetHistogramBinLabelDisplay(static_cast<Wisteria::BinLabelDisplay>(value));
                }

            SetHistogramBarColor(TiXmlNodeToColor(
                histogramNode->FirstChildElement(XML_GRAPH_COLOR.data()), GetHistogramBarColor()));

            auto* opacityNodeHisto = histogramNode->FirstChildElement(XML_GRAPH_OPACITY.data());
            if (opacityNodeHisto != nullptr)
                {
                SetHistogramBarOpacity(
                    static_cast<uint8_t>(opacityNodeHisto->ToElement()->IntAttribute(
                        XML_VALUE.data(), GetHistogramBarOpacity())));
                }
            auto* barEffectNode = histogramNode->FirstChildElement(XML_BAR_EFFECT.data());
            if (barEffectNode != nullptr)
                {
                int value = barEffectNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), static_cast<int>(GetHistogramBarEffect()));
                if (value < 0 ||
                    value >= static_cast<decltype(value)>(Wisteria::BoxEffect::EFFECTS_COUNT))
                    {
                    value = static_cast<decltype(value)>(Wisteria::BoxEffect::Solid);
                    }
                SetHistogramBarEffect(static_cast<Wisteria::BoxEffect>(value));
                }
            }
        // bar chart settings
        auto* barChartNode = graphDefaultsNode->FirstChildElement(XML_BAR_CHART_SETTINGS.data());
        if (barChartNode != nullptr)
            {
            SetBarChartBarColor(TiXmlNodeToColor(
                barChartNode->FirstChildElement(XML_GRAPH_COLOR.data()), GetBarChartBarColor()));

            auto* orientationNode = barChartNode->FirstChildElement(XML_BAR_ORIENTATION.data());
            if (orientationNode != nullptr)
                {
                int value = orientationNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), static_cast<int>(GetBarChartOrientation()));
                if (value < 0 ||
                    value >= static_cast<decltype(value)>(Wisteria::Orientation::ORIENTATION_COUNT))
                    {
                    value = static_cast<decltype(value)>(Wisteria::Orientation::Vertical);
                    }
                SetBarChartOrientation(static_cast<Wisteria::Orientation>(value));
                }
            auto* opacityNodeBarChart = barChartNode->FirstChildElement(XML_GRAPH_OPACITY.data());
            if (opacityNodeBarChart != nullptr)
                {
                SetGraphBarOpacity(
                    static_cast<uint8_t>(opacityNodeBarChart->ToElement()->IntAttribute(
                        XML_VALUE.data(), GetGraphBarOpacity())));
                }
            auto* barEffectNode = barChartNode->FirstChildElement(XML_BAR_EFFECT.data());
            if (barEffectNode != nullptr)
                {
                int value = barEffectNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), static_cast<int>(GetGraphBarEffect()));
                if (value < 0 ||
                    value >= static_cast<decltype(value)>(Wisteria::BoxEffect::EFFECTS_COUNT))
                    {
                    value = static_cast<decltype(value)>(Wisteria::BoxEffect::Solid);
                    }
                SetGraphBarEffect(static_cast<Wisteria::BoxEffect>(value));
                }
            auto* barDisplayLabelNode =
                barChartNode->FirstChildElement(XML_BAR_DISPLAY_LABELS.data());
            if (barDisplayLabelNode != nullptr)
                {
                DisplayBarChartLabels(int_to_bool(barDisplayLabelNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), bool_to_int(IsDisplayingBarChartLabels()))));
                }
            }
        // box plots settings
        auto* boxPlotNode = graphDefaultsNode->FirstChildElement(XML_BOX_PLOT_SETTINGS.data());
        if (boxPlotNode != nullptr)
            {
            SetGraphBoxColor(TiXmlNodeToColor(
                boxPlotNode->FirstChildElement(XML_GRAPH_COLOR.data()), GetGraphBoxColor()));

            auto* opacityNodeBoxPlot = boxPlotNode->FirstChildElement(XML_GRAPH_OPACITY.data());
            if (opacityNodeBoxPlot != nullptr)
                {
                SetGraphBoxOpacity(
                    static_cast<uint8_t>(opacityNodeBoxPlot->ToElement()->IntAttribute(
                        XML_VALUE.data(), GetGraphBoxOpacity())));
                }
            auto* boxEffectNode = boxPlotNode->FirstChildElement(XML_BOX_EFFECT.data());
            if (boxEffectNode != nullptr)
                {
                int value = boxEffectNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), static_cast<int>(GetGraphBoxEffect()));
                if (value < 0 ||
                    value >= static_cast<decltype(value)>(Wisteria::BoxEffect::EFFECTS_COUNT))
                    {
                    value = static_cast<decltype(value)>(Wisteria::BoxEffect::Solid);
                    }
                SetGraphBoxEffect(static_cast<Wisteria::BoxEffect>(value));
                }
            auto* boxPlotShowAllPointsNode =
                boxPlotNode->FirstChildElement(XML_BOX_PLOT_SHOW_ALL_POINTS.data());
            if (boxPlotShowAllPointsNode != nullptr)
                {
                ShowAllBoxPlotPoints(
                    int_to_bool(boxPlotShowAllPointsNode->ToElement()->IntAttribute(
                        XML_VALUE.data(), bool_to_int(IsShowingAllBoxPlotPoints()))));
                }
            auto* boxConnectMiddlePointsNode =
                boxPlotNode->FirstChildElement(XML_BOX_CONNECT_MIDDLE_POINTS.data());
            if (boxConnectMiddlePointsNode != nullptr)
                {
                ConnectBoxPlotMiddlePoints(
                    int_to_bool(boxConnectMiddlePointsNode->ToElement()->IntAttribute(
                        XML_VALUE.data(), bool_to_int(IsConnectingBoxPlotMiddlePoints()))));
                }
            auto* boxDisplayLabelNode =
                boxPlotNode->FirstChildElement(XML_BOX_DISPLAY_LABELS.data());
            if (boxDisplayLabelNode != nullptr)
                {
                DisplayBoxPlotLabels(int_to_bool(boxDisplayLabelNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), bool_to_int(IsDisplayingBoxPlotLabels()))));
                }
            }
        // Readability graph options
        // Flesch chart
        auto* fleschChartSettingsNode =
            graphDefaultsNode->FirstChildElement(XML_FLESCH_CHART_SETTINGS.data());
        if (fleschChartSettingsNode != nullptr)
            {
            auto* connectionLine =
                fleschChartSettingsNode->FirstChildElement(XML_INCLUDE_CONNECTION_LINE.data());
            if (connectionLine != nullptr)
                {
                ConnectFleschPoints(int_to_bool(connectionLine->ToElement()->IntAttribute(
                    XML_INCLUDE.data(), bool_to_int(IsConnectingFleschPoints()))));
                }
            auto* rulerDocGroup =
                fleschChartSettingsNode->FirstChildElement(XML_FLESCH_RULER_DOC_GROUPS.data());
            if (rulerDocGroup != nullptr)
                {
                IncludeFleschRulerDocGroups(int_to_bool(rulerDocGroup->ToElement()->IntAttribute(
                    XML_VALUE.data(), bool_to_int(IsIncludingFleschRulerDocGroups()))));
                }
            }
        // Lix
        auto* lixSettingsNode = graphDefaultsNode->FirstChildElement(XML_LIX_SETTINGS.data());
        if (lixSettingsNode != nullptr)
            {
            auto* useEnglishLabels =
                lixSettingsNode->FirstChildElement(XML_USE_ENGLISH_LABELS.data());
            if (useEnglishLabels != nullptr)
                {
                UseEnglishLabelsForGermanLix(
                    int_to_bool(useEnglishLabels->ToElement()->IntAttribute(
                        XML_INCLUDE.data(), bool_to_int(IsUsingEnglishLabelsForGermanLix()))));
                }
            }
        // Fry/Raygor/GPM/Schwartz
        auto* fryRaygorNode = graphDefaultsNode->FirstChildElement(XML_FRY_RAYGOR_SETTINGS.data());
        if (fryRaygorNode != nullptr)
            {
            // invalid area colors
            SetInvalidAreaColor(
                TiXmlNodeToColor(fryRaygorNode->FirstChildElement(XML_INVALID_AREA_COLOR.data()),
                                 GetInvalidAreaColor()));

            // Raygor appearance
            auto* raygorStyleNode = fryRaygorNode->FirstChildElement(XML_RAYGOR_STYLE.data());
            if (raygorStyleNode != nullptr)
                {
                int value = raygorStyleNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), static_cast<int>(GetRaygorStyle()));
                if (value < 0 || value >= static_cast<decltype(value)>(
                                              Wisteria::Graphs::RaygorStyle::RAYGOR_STYLE_COUNT))
                    {
                    value =
                        static_cast<decltype(value)>(Wisteria::Graphs::RaygorStyle::BaldwinKaufman);
                    }
                SetRaygorStyle(static_cast<Wisteria::Graphs::RaygorStyle>(value));
                }
            }
        // axis options
        auto* axisNode = graphDefaultsNode->FirstChildElement(XML_AXIS_SETTINGS.data());
        if (axisNode != nullptr)
            {
            auto* xAxisNode = axisNode->FirstChildElement(XML_X_AXIS.data());
            if (xAxisNode != nullptr)
                {
                // font color
                SetXAxisFontColor(TiXmlNodeToColor(
                    xAxisNode->FirstChildElement(XML_FONT_COLOR.data()), GetXAxisFontColor()));

                // font
                auto* fontNode = xAxisNode->FirstChildElement(XML_FONT.data());
                if (fontNode != nullptr)
                    {
                    const int pointSize = fontNode->ToElement()->IntAttribute(
                        XmlFormat::FONT_POINT_SIZE_TAG.data(),
                        wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize());
                    const int style = fontNode->ToElement()->IntAttribute(
                        XmlFormat::FONT_STYLE_TAG.data(), wxFONTSTYLE_NORMAL);
                    const int weight = fontNode->ToElement()->IntAttribute(
                        XmlFormat::FONT_WEIGHT_TAG.data(), wxFONTWEIGHT_NORMAL);
                    const int underlined = fontNode->ToElement()->IntAttribute(
                        XmlFormat::FONT_UNDERLINE_TAG.data(), 0);
                    // get the font point size
                    m_xAxisFont.SetPointSize(
                        (pointSize > 0) ?
                            pointSize :
                            wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize());
                    // get the font style
                    m_xAxisFont.SetStyle(static_cast<wxFontStyle>(style));
                    // get the font weight
                    m_xAxisFont.SetWeight(static_cast<wxFontWeight>(weight));
                    // get the font underlining
                    m_xAxisFont.SetUnderlined(int_to_bool(underlined));
                    // get the font facename
                    const char* faceName =
                        fontNode->ToElement()->Attribute(XmlFormat::FONT_FACE_NAME_TAG.data());
                    if (faceName != nullptr)
                        {
                        const auto faceNameStr = Wisteria::TextStream::CharStreamToUnicode(
                            faceName, std::strlen(faceName));
                        const wchar_t* filteredText =
                            filterHtml(faceNameStr.c_str(), faceNameStr.length(), true, false);
                        if ((filteredText != nullptr) &&
                            wxFontEnumerator::IsValidFacename(filteredText))
                            {
                            m_xAxisFont.SetFaceName(wxString(filteredText));
                            }
                        }
                    }
                }
            auto* yAxisNode = axisNode->FirstChildElement(XML_Y_AXIS.data());
            if (yAxisNode != nullptr)
                {
                // font color
                SetYAxisFontColor(TiXmlNodeToColor(
                    yAxisNode->FirstChildElement(XML_FONT_COLOR.data()), GetYAxisFontColor()));

                // font
                auto* fontNode = yAxisNode->FirstChildElement(XML_FONT.data());
                if (fontNode != nullptr)
                    {
                    const int pointSize = fontNode->ToElement()->IntAttribute(
                        XmlFormat::FONT_POINT_SIZE_TAG.data(),
                        wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize());
                    const int style = fontNode->ToElement()->IntAttribute(
                        XmlFormat::FONT_STYLE_TAG.data(), wxFONTSTYLE_NORMAL);
                    const int weight = fontNode->ToElement()->IntAttribute(
                        XmlFormat::FONT_WEIGHT_TAG.data(), wxFONTWEIGHT_NORMAL);
                    const int underlined = fontNode->ToElement()->IntAttribute(
                        XmlFormat::FONT_UNDERLINE_TAG.data(), 0);
                    // get the font point size
                    m_yAxisFont.SetPointSize(
                        (pointSize > 0) ?
                            pointSize :
                            wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize());
                    // get the font style
                    m_yAxisFont.SetStyle(static_cast<wxFontStyle>(style));
                    // get the font weight
                    m_yAxisFont.SetWeight(static_cast<wxFontWeight>(weight));
                    // get the font underlining
                    m_yAxisFont.SetUnderlined(int_to_bool(underlined));
                    // get the font facename
                    const char* faceName =
                        fontNode->ToElement()->Attribute(XmlFormat::FONT_FACE_NAME_TAG.data());
                    if (faceName != nullptr)
                        {
                        const auto faceNameStr = Wisteria::TextStream::CharStreamToUnicode(
                            faceName, std::strlen(faceName));
                        const wchar_t* filteredText =
                            filterHtml(faceNameStr.c_str(), faceNameStr.length(), true, false);
                        if ((filteredText != nullptr) &&
                            wxFontEnumerator::IsValidFacename(filteredText))
                            {
                            m_yAxisFont.SetFaceName(wxString(filteredText));
                            }
                        }
                    }
                }
            }
        // title options
        auto* titleNode = graphDefaultsNode->FirstChildElement(XML_TITLE_SETTINGS.data());
        if (titleNode != nullptr)
            {
            auto* topTitleNode = titleNode->FirstChildElement(XML_TOP_TITLE.data());
            if (topTitleNode != nullptr)
                {
                // font color
                SetGraphTopTitleFontColor(
                    TiXmlNodeToColor(topTitleNode->FirstChildElement(XML_FONT_COLOR.data()),
                                     GetGraphTopTitleFontColor()));

                // font
                auto* fontNode = topTitleNode->FirstChildElement(XML_FONT.data());
                if (fontNode != nullptr)
                    {
                    const int pointSize = fontNode->ToElement()->IntAttribute(
                        XmlFormat::FONT_POINT_SIZE_TAG.data(),
                        wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize());
                    const int style = fontNode->ToElement()->IntAttribute(
                        XmlFormat::FONT_STYLE_TAG.data(), wxFONTSTYLE_NORMAL);
                    const int weight = fontNode->ToElement()->IntAttribute(
                        XmlFormat::FONT_WEIGHT_TAG.data(), wxFONTWEIGHT_NORMAL);
                    const int underlined = fontNode->ToElement()->IntAttribute(
                        XmlFormat::FONT_UNDERLINE_TAG.data(), 0);
                    // get the font point size
                    m_topTitleFont.SetPointSize(
                        (pointSize > 0) ?
                            pointSize :
                            wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize());
                    // get the font style
                    m_topTitleFont.SetStyle(static_cast<wxFontStyle>(style));
                    // get the font weight
                    m_topTitleFont.SetWeight(static_cast<wxFontWeight>(weight));
                    // get the font underlining
                    m_topTitleFont.SetUnderlined(int_to_bool(underlined));
                    // get the font facename
                    const char* faceName =
                        fontNode->ToElement()->Attribute(XmlFormat::FONT_FACE_NAME_TAG.data());
                    if (faceName != nullptr)
                        {
                        const auto faceNameStr = Wisteria::TextStream::CharStreamToUnicode(
                            faceName, std::strlen(faceName));
                        const wchar_t* filteredText =
                            filterHtml(faceNameStr.c_str(), faceNameStr.length(), true, false);
                        if ((filteredText != nullptr) &&
                            wxFontEnumerator::IsValidFacename(filteredText))
                            {
                            m_topTitleFont.SetFaceName(wxString(filteredText));
                            }
                        }
                    }
                }
            auto* bottomTitleNode = titleNode->FirstChildElement(XML_BOTTOM_TITLE.data());
            if (bottomTitleNode != nullptr)
                {
                // font color
                SetGraphBottomTitleFontColor(
                    TiXmlNodeToColor(bottomTitleNode->FirstChildElement(XML_FONT_COLOR.data()),
                                     GetGraphBottomTitleFontColor()));

                // font
                auto* fontNode = bottomTitleNode->FirstChildElement(XML_FONT.data());
                if (fontNode != nullptr)
                    {
                    const int pointSize = fontNode->ToElement()->IntAttribute(
                        XmlFormat::FONT_POINT_SIZE_TAG.data(),
                        wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize());
                    const int style = fontNode->ToElement()->IntAttribute(
                        XmlFormat::FONT_STYLE_TAG.data(), wxFONTSTYLE_NORMAL);
                    const int weight = fontNode->ToElement()->IntAttribute(
                        XmlFormat::FONT_WEIGHT_TAG.data(), wxFONTWEIGHT_NORMAL);
                    const int underlined = fontNode->ToElement()->IntAttribute(
                        XmlFormat::FONT_UNDERLINE_TAG.data(), 0);
                    // get the font point size
                    m_bottomTitleFont.SetPointSize(
                        (pointSize > 0) ?
                            pointSize :
                            wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize());
                    // get the font style
                    m_bottomTitleFont.SetStyle(static_cast<wxFontStyle>(style));
                    // get the font weight
                    m_bottomTitleFont.SetWeight(static_cast<wxFontWeight>(weight));
                    // get the font underlining
                    m_bottomTitleFont.SetUnderlined(int_to_bool(underlined));
                    // get the font facename
                    const char* faceName =
                        fontNode->ToElement()->Attribute(XmlFormat::FONT_FACE_NAME_TAG.data());
                    if (faceName != nullptr)
                        {
                        const auto faceNameStr = Wisteria::TextStream::CharStreamToUnicode(
                            faceName, std::strlen(faceName));
                        const wchar_t* filteredText =
                            filterHtml(faceNameStr.c_str(), faceNameStr.length(), true, false);
                        if ((filteredText != nullptr) &&
                            wxFontEnumerator::IsValidFacename(filteredText))
                            {
                            m_bottomTitleFont.SetFaceName(wxString(filteredText));
                            }
                        }
                    }
                }
            auto* leftTitleNode = titleNode->FirstChildElement(XML_LEFT_TITLE.data());
            if (leftTitleNode != nullptr)
                {
                // font color
                SetGraphLeftTitleFontColor(
                    TiXmlNodeToColor(leftTitleNode->FirstChildElement(XML_FONT_COLOR.data()),
                                     GetGraphLeftTitleFontColor()));

                // font
                auto* fontNode = leftTitleNode->FirstChildElement(XML_FONT.data());
                if (fontNode != nullptr)
                    {
                    const int pointSize = fontNode->ToElement()->IntAttribute(
                        XmlFormat::FONT_POINT_SIZE_TAG.data(),
                        wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize());
                    const int style = fontNode->ToElement()->IntAttribute(
                        XmlFormat::FONT_STYLE_TAG.data(), wxFONTSTYLE_NORMAL);
                    const int weight = fontNode->ToElement()->IntAttribute(
                        XmlFormat::FONT_WEIGHT_TAG.data(), wxFONTWEIGHT_NORMAL);
                    const int underlined = fontNode->ToElement()->IntAttribute(
                        XmlFormat::FONT_UNDERLINE_TAG.data(), 0);
                    // get the font point size
                    m_leftTitleFont.SetPointSize(
                        (pointSize > 0) ?
                            pointSize :
                            wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize());
                    // get the font style
                    m_leftTitleFont.SetStyle(static_cast<wxFontStyle>(style));
                    // get the font weight
                    m_leftTitleFont.SetWeight(static_cast<wxFontWeight>(weight));
                    // get the font underlining
                    m_leftTitleFont.SetUnderlined(int_to_bool(underlined));
                    // get the font facename
                    const char* faceName =
                        fontNode->ToElement()->Attribute(XmlFormat::FONT_FACE_NAME_TAG.data());
                    if (faceName != nullptr)
                        {
                        const auto faceNameStr = Wisteria::TextStream::CharStreamToUnicode(
                            faceName, std::strlen(faceName));
                        const wchar_t* filteredText =
                            filterHtml(faceNameStr.c_str(), faceNameStr.length(), true, false);
                        if ((filteredText != nullptr) &&
                            wxFontEnumerator::IsValidFacename(filteredText))
                            {
                            m_leftTitleFont.SetFaceName(wxString(filteredText));
                            }
                        }
                    }
                }
            auto* rightTitleNode = titleNode->FirstChildElement(XML_RIGHT_TITLE.data());
            if (rightTitleNode != nullptr)
                {
                // font color
                SetGraphRightTitleFontColor(
                    TiXmlNodeToColor(rightTitleNode->FirstChildElement(XML_FONT_COLOR.data()),
                                     GetGraphRightTitleFontColor()));

                // font
                auto* fontNode = rightTitleNode->FirstChildElement(XML_FONT.data());
                if (fontNode != nullptr)
                    {
                    const int pointSize = fontNode->ToElement()->IntAttribute(
                        XmlFormat::FONT_POINT_SIZE_TAG.data(),
                        wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize());
                    const int style = fontNode->ToElement()->IntAttribute(
                        XmlFormat::FONT_STYLE_TAG.data(), wxFONTSTYLE_NORMAL);
                    const int weight = fontNode->ToElement()->IntAttribute(
                        XmlFormat::FONT_WEIGHT_TAG.data(), wxFONTWEIGHT_NORMAL);
                    const int underlined = fontNode->ToElement()->IntAttribute(
                        XmlFormat::FONT_UNDERLINE_TAG.data(), 0);
                    // get the font point size
                    m_rightTitleFont.SetPointSize(
                        (pointSize > 0) ?
                            pointSize :
                            wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize());
                    // get the font style
                    m_rightTitleFont.SetStyle(static_cast<wxFontStyle>(style));
                    // get the font weight
                    m_rightTitleFont.SetWeight(static_cast<wxFontWeight>(weight));
                    // get the font underlining
                    m_rightTitleFont.SetUnderlined(int_to_bool(underlined));
                    // get the font facename
                    const char* faceName =
                        fontNode->ToElement()->Attribute(XmlFormat::FONT_FACE_NAME_TAG.data());
                    if (faceName != nullptr)
                        {
                        const auto faceNameStr = Wisteria::TextStream::CharStreamToUnicode(
                            faceName, std::strlen(faceName));
                        const wchar_t* filteredText =
                            filterHtml(faceNameStr.c_str(), faceNameStr.length(), true, false);
                        if ((filteredText != nullptr) &&
                            wxFontEnumerator::IsValidFacename(filteredText))
                            {
                            m_rightTitleFont.SetFaceName(wxString(filteredText));
                            }
                        }
                    }
                }
            }
        }
    }

//------------------------------------------------
void ReadabilityAppOptions::LoadDocAnalysisNode(tinyxml2::XMLElement* projectSettings)
    {
    lily_of_the_valley::html_extract_text filterHtml;
    auto* documentAnalysisNode =
        projectSettings->FirstChildElement(XML_DOCUMENT_ANALYSIS_LOGIC.data());
    if (documentAnalysisNode != nullptr)
        {
        // determinant for what a long sentence is
        auto* longSentenceNode =
            documentAnalysisNode->FirstChildElement(XML_LONG_SENTENCE_METHOD.data());
        if (longSentenceNode != nullptr)
            {
            m_longSentenceMethod =
                static_cast<LongSentence>(longSentenceNode->ToElement()->IntAttribute(
                    XML_METHOD.data(), static_cast<int>(m_longSentenceMethod)));
            // verify that this is a sensical value
            if (m_longSentenceMethod != LongSentence::LongerThanSpecifiedLength &&
                m_longSentenceMethod != LongSentence::OutlierLength)
                {
                m_longSentenceMethod = LongSentence::LongerThanSpecifiedLength;
                }
            }
        auto* longSentenceLengthNode =
            documentAnalysisNode->FirstChildElement(XML_LONG_SENTENCE_LENGTH.data());
        if (longSentenceLengthNode != nullptr)
            {
            m_difficultSentenceLength = longSentenceLengthNode->ToElement()->IntAttribute(
                XML_VALUE.data(), m_difficultSentenceLength);
            // verify that this is a sensical value
            if (m_difficultSentenceLength <= 0)
                {
                m_difficultSentenceLength = 22;
                }
            }
        // determinant for how to syllabize numerals
        auto* numSyllNode =
            documentAnalysisNode->FirstChildElement(XML_NUMERAL_SYLLABICATION_METHOD.data());
        if (numSyllNode != nullptr)
            {
            m_numeralSyllabicationMethod =
                static_cast<NumeralSyllabize>(numSyllNode->ToElement()->IntAttribute(
                    XML_METHOD.data(), static_cast<int>(m_numeralSyllabicationMethod)));
            // verify that this is a sensical value
            if (m_numeralSyllabicationMethod != NumeralSyllabize::WholeWordIsOneSyllable &&
                m_numeralSyllabicationMethod != NumeralSyllabize::SoundOutEachDigit)
                {
                m_numeralSyllabicationMethod = NumeralSyllabize::WholeWordIsOneSyllable;
                }
            }
        // whether we should ignore blank lines when parsing paragraphs
        auto* ignoreBlankLines = documentAnalysisNode->FirstChildElement(
            XML_IGNORE_BLANK_LINES_FOR_PARAGRAPH_PARSING.data());
        if (ignoreBlankLines != nullptr)
            {
            m_ignoreBlankLinesForParagraphsParser =
                int_to_bool(ignoreBlankLines->ToElement()->IntAttribute(
                    XML_VALUE.data(), bool_to_int(m_ignoreBlankLinesForParagraphsParser)));
            }
        // whether we should ignore indenting when parsing paragraphs
        auto* ignoreIndents = documentAnalysisNode->FirstChildElement(
            XML_IGNORE_INDENTING_FOR_PARAGRAPH_PARSING.data());
        if (ignoreIndents != nullptr)
            {
            m_ignoreIndentingForParagraphsParser =
                int_to_bool(ignoreIndents->ToElement()->IntAttribute(
                    XML_VALUE.data(), bool_to_int(m_ignoreIndentingForParagraphsParser)));
            }
        // whether sentences must start capitalized
        auto* sentenceStartMustBeUppercased =
            documentAnalysisNode->FirstChildElement(XML_SENTENCES_MUST_START_CAPITALIZED.data());
        if (sentenceStartMustBeUppercased != nullptr)
            {
            m_sentenceStartMustBeUppercased =
                int_to_bool(sentenceStartMustBeUppercased->ToElement()->IntAttribute(
                    XML_VALUE.data(), bool_to_int(m_sentenceStartMustBeUppercased)));
            }
        // file path to phrases to exclude from analysis
        auto* excludedPhrasesFilePath =
            documentAnalysisNode->FirstChildElement(XML_EXCLUDED_PHRASES_PATH.data());
        if (excludedPhrasesFilePath != nullptr)
            {
            const char* filePathChars =
                excludedPhrasesFilePath->ToElement()->Attribute(XML_VALUE.data());
            if (filePathChars != nullptr)
                {
                const auto filePathStr = Wisteria::TextStream::CharStreamToUnicode(
                    filePathChars, std::strlen(filePathChars));
                const wchar_t* convertedStr =
                    filterHtml(filePathStr.c_str(), filePathStr.length(), true, false);
                if (convertedStr != nullptr)
                    {
                    SetExcludedPhrasesPath(convertedStr);
                    }
                }
            }
        // whether to include first occurrence of excluded phrases
        auto* includeExcludedPhraseFirstOccurrenceNode = documentAnalysisNode->FirstChildElement(
            XML_EXCLUDED_PHRASES_INCLUDE_FIRST_OCCURRENCE.data());
        if (includeExcludedPhraseFirstOccurrenceNode != nullptr)
            {
            IncludeExcludedPhraseFirstOccurrence(
                int_to_bool(includeExcludedPhraseFirstOccurrenceNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), bool_to_int(IsIncludingExcludedPhraseFirstOccurrence()))));
            }
        auto* excludedBlockTagsNode =
            documentAnalysisNode->FirstChildElement(XML_EXCLUDE_BLOCK_TAGS.data());
        if (excludedBlockTagsNode != nullptr)
            {
            auto* excludedBlockTagNode =
                excludedBlockTagsNode->FirstChildElement(XML_EXCLUDE_BLOCK_TAG.data());
            while (excludedBlockTagNode != nullptr)
                {
                const wxString blockTags =
                    TiXmlNodeAttributeToString(excludedBlockTagNode, XML_VALUE.data());
                if (blockTags.length() >= 2)
                    {
                    m_exclusionBlockTags.emplace_back(blockTags[0], blockTags[1]);
                    }
                excludedBlockTagNode =
                    excludedBlockTagNode->NextSiblingElement(XML_EXCLUDE_BLOCK_TAG.data());
                }
            }
        // whether to ignore proper nouns
        auto* ignoreProperNounsNode =
            documentAnalysisNode->FirstChildElement(XML_IGNORE_PROPER_NOUNS.data());
        if (ignoreProperNounsNode != nullptr)
            {
            m_excludeProperNouns = int_to_bool(ignoreProperNounsNode->ToElement()->IntAttribute(
                XML_VALUE.data(), bool_to_int(m_excludeProperNouns)));
            }
        // whether to ignore numerals
        auto* ignoreNumerals = documentAnalysisNode->FirstChildElement(XML_IGNORE_NUMERALS.data());
        if (ignoreNumerals != nullptr)
            {
            m_excludeNumerals = int_to_bool(ignoreNumerals->ToElement()->IntAttribute(
                XML_VALUE.data(), bool_to_int(m_excludeNumerals)));
            }
        // whether to ignore file address
        auto* ignoreFileAddresses =
            documentAnalysisNode->FirstChildElement(XML_IGNORE_FILE_ADDRESSES.data());
        if (ignoreFileAddresses != nullptr)
            {
            m_excludeFileAddresses = int_to_bool(ignoreFileAddresses->ToElement()->IntAttribute(
                XML_VALUE.data(), bool_to_int(m_excludeFileAddresses)));
            }
        // whether to ignore citations
        auto* ignoreCitations =
            documentAnalysisNode->FirstChildElement(XML_IGNORE_CITATIONS.data());
        if (ignoreCitations != nullptr)
            {
            m_excludeTrailingCitations = int_to_bool(ignoreCitations->ToElement()->IntAttribute(
                XML_VALUE.data(), bool_to_int(m_excludeTrailingCitations)));
            }
        // whether to aggressively exclude
        auto* aggressivelyDeducingLists =
            documentAnalysisNode->FirstChildElement(XML_AGGRESSIVE_EXCLUSION.data());
        if (aggressivelyDeducingLists != nullptr)
            {
            m_aggressiveExclusion =
                int_to_bool(aggressivelyDeducingLists->ToElement()->IntAttribute(
                    XML_VALUE.data(), bool_to_int(m_aggressiveExclusion)));
            }
        // whether to ignore copyright notices
        auto* ignoreCopyrightNotices =
            documentAnalysisNode->FirstChildElement(XML_IGNORE_COPYRIGHT_NOTICES.data());
        if (ignoreCopyrightNotices != nullptr)
            {
            m_excludeTrailingCopyrightNoticeParagraphs =
                int_to_bool(ignoreCopyrightNotices->ToElement()->IntAttribute(
                    XML_VALUE.data(), bool_to_int(m_excludeTrailingCopyrightNoticeParagraphs)));
            }
        // determinant for how to text is parsed into paragraphs
        auto* paraParsingNode =
            documentAnalysisNode->FirstChildElement(XML_PARAGRAPH_PARSING_METHOD.data());
        if (paraParsingNode != nullptr)
            {
            m_paragraphsParsingMethod =
                static_cast<ParagraphParse>(paraParsingNode->ToElement()->IntAttribute(
                    XML_METHOD.data(), static_cast<int>(m_paragraphsParsingMethod)));
            // verify that this is a sensical value
            if (m_paragraphsParsingMethod !=
                    ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs &&
                m_paragraphsParsingMethod != ParagraphParse::EachNewLineIsAParagraph)
                {
                m_paragraphsParsingMethod =
                    ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs;
                }
            }

        // determinant for how to handle headers, titles, bullet points, and lists
        auto* invalidSentenceNode =
            documentAnalysisNode->FirstChildElement(XML_INVALID_SENTENCE_METHOD.data());
        if (invalidSentenceNode != nullptr)
            {
            m_invalidSentenceMethod =
                static_cast<InvalidSentence>(invalidSentenceNode->ToElement()->IntAttribute(
                    XML_METHOD.data(), static_cast<int>(m_invalidSentenceMethod)));
            if (static_cast<int>(m_invalidSentenceMethod) < 0 ||
                static_cast<int>(m_invalidSentenceMethod) >=
                    static_cast<int>(InvalidSentence::INVALID_SENTENCE_METHOD_COUNT))
                {
                m_invalidSentenceMethod = InvalidSentence::ExcludeFromAnalysis;
                }
            }

        // number of words that will make an incomplete sentence actually complete
        auto* includeIncompleteSentencesIfLongerThanNode = documentAnalysisNode->FirstChildElement(
            XML_INCLUDE_INCOMPLETE_SENTENCES_LONGER_THAN.data());
        if (includeIncompleteSentencesIfLongerThanNode != nullptr)
            {
            m_includeIncompleteSentencesIfLongerThan =
                includeIncompleteSentencesIfLongerThanNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), m_includeIncompleteSentencesIfLongerThan);
            }
        }
    } //------------------------------------------------

//------------------------------------------------
void ReadabilityAppOptions::LoadReadabilityTestsNode(tinyxml2::XMLElement* projectSettings)
    {
    lily_of_the_valley::html_extract_text filterHtml;
    auto* readabilityTestsNode =
        projectSettings->FirstChildElement(XML_READABILITY_TESTS_SECTION.data());
    if (readabilityTestsNode != nullptr)
        {
        // grade scale display
        auto* gradeScaleNode = readabilityTestsNode->FirstChildElement(
            XML_READABILITY_TEST_GRADE_SCALE_DISPLAY.data());
        if (gradeScaleNode != nullptr)
            {
            GetReadabilityMessageCatalog().SetGradeScale(
                static_cast<readability::grade_scale>(gradeScaleNode->ToElement()->IntAttribute(
                    XML_VALUE.data(),
                    static_cast<int>(GetReadabilityMessageCatalog().GetGradeScale()))));
            }

        auto* gradeScaleLongFormatNode = readabilityTestsNode->FirstChildElement(
            XML_READABILITY_TEST_GRADE_SCALE_LONG_FORMAT.data());
        if (gradeScaleLongFormatNode != nullptr)
            {
            GetReadabilityMessageCatalog().SetLongGradeScaleFormat(
                int_to_bool(gradeScaleLongFormatNode->ToElement()->IntAttribute(
                    XML_VALUE.data(),
                    bool_to_int(GetReadabilityMessageCatalog().IsUsingLongGradeScaleFormat()))));
            }

        auto* readingAgeFormatNode =
            readabilityTestsNode->FirstChildElement(XML_READING_AGE_FORMAT.data());
        if (readingAgeFormatNode != nullptr)
            {
            GetReadabilityMessageCatalog().SetReadingAgeDisplay(
                static_cast<ReadabilityMessages::ReadingAgeDisplay>(
                    readingAgeFormatNode->ToElement()->IntAttribute(
                        XML_VALUE.data(),
                        static_cast<int>(GetReadabilityMessageCatalog().GetReadingAgeDisplay()))));
            }

        auto* includeScoreSummaryReportNode =
            readabilityTestsNode->FirstChildElement(XML_INCLUDE_SCORES_SUMMARY_REPORT.data());
        if (includeScoreSummaryReportNode != nullptr)
            {
            IncludeScoreSummaryReport(
                int_to_bool(includeScoreSummaryReportNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), bool_to_int(IsIncludingScoreSummaryReport()))));
            }

        // test-specific options
        auto* fleschKincaidOptionsNode =
            readabilityTestsNode->FirstChildElement(XML_FLESCH_KINCAID_OPTIONS.data());
        if (fleschKincaidOptionsNode != nullptr)
            {
            auto* numeralMethod = fleschKincaidOptionsNode->FirstChildElement(
                XML_NUMERAL_SYLLABICATION_METHOD.data());
            if (numeralMethod != nullptr)
                {
                SetFleschKincaidNumeralSyllabizeMethod(static_cast<FleschKincaidNumeralSyllabize>(
                    numeralMethod->ToElement()->IntAttribute(
                        XML_VALUE.data(),
                        static_cast<int>(GetFleschKincaidNumeralSyllabizeMethod()))));
                }
            }

        auto* fleschOptionsNode =
            readabilityTestsNode->FirstChildElement(XML_FLESCH_OPTIONS.data());
        if (fleschOptionsNode != nullptr)
            {
            auto* numeralMethod =
                fleschOptionsNode->FirstChildElement(XML_NUMERAL_SYLLABICATION_METHOD.data());
            if (numeralMethod != nullptr)
                {
                SetFleschNumeralSyllabizeMethod(
                    static_cast<FleschNumeralSyllabize>(numeralMethod->ToElement()->IntAttribute(
                        XML_VALUE.data(), static_cast<int>(GetFleschNumeralSyllabizeMethod()))));
                }
            }

        auto* fogOptionsNode =
            readabilityTestsNode->FirstChildElement(XML_GUNNING_FOG_OPTIONS.data());
        if (fogOptionsNode != nullptr)
            {
            auto* useUnits = fogOptionsNode->FirstChildElement(XML_USE_SENTENCE_UNITS.data());
            if (useUnits != nullptr)
                {
                FogUseSentenceUnits(int_to_bool(useUnits->ToElement()->IntAttribute(
                    XML_VALUE.data(), bool_to_int(IsFogUsingSentenceUnits()))));
                }
            }

        auto* hjOptionsNode =
            readabilityTestsNode->FirstChildElement(XML_HARRIS_JACOBSON_OPTIONS.data());
        if (hjOptionsNode != nullptr)
            {
            auto* hjTextExclusion = hjOptionsNode->FirstChildElement(XML_TEXT_EXCLUSION.data());
            if (hjTextExclusion != nullptr)
                {
                int value = hjTextExclusion->ToElement()->IntAttribute(
                    XML_VALUE.data(), static_cast<int>(GetHarrisJacobsonTextExclusionMode()));
                if (value < 0 ||
                    value >=
                        static_cast<int>(
                            SpecializedTestTextExclusion::SPECIALIZED_TEST_TEXT_EXCLUSION_COUNT))
                    {
                    value = static_cast<int>(
                        SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings);
                    }
                SetHarrisJacobsonTextExclusionMode(
                    static_cast<SpecializedTestTextExclusion>(value));
                }
            }

        auto* dcOptionsNode =
            readabilityTestsNode->FirstChildElement(XML_NEW_DALE_CHALL_OPTIONS.data());
        if (dcOptionsNode != nullptr)
            {
            auto* includeStockerListNode =
                dcOptionsNode->FirstChildElement(XML_STOCKER_LIST.data());
            if (includeStockerListNode != nullptr)
                {
                IncludeStockerCatholicSupplement(
                    int_to_bool(includeStockerListNode->ToElement()->IntAttribute(
                        XML_VALUE.data(), bool_to_int(IsIncludingStockerCatholicSupplement()))));
                }
            auto* dcTextExclusion = dcOptionsNode->FirstChildElement(XML_TEXT_EXCLUSION.data());
            if (dcTextExclusion != nullptr)
                {
                int value = dcTextExclusion->ToElement()->IntAttribute(
                    XML_VALUE.data(), static_cast<int>(GetDaleChallTextExclusionMode()));
                if (value < 0 ||
                    value >=
                        static_cast<int>(
                            SpecializedTestTextExclusion::SPECIALIZED_TEST_TEXT_EXCLUSION_COUNT))
                    {
                    value = static_cast<int>(
                        SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings);
                    }
                SetDaleChallTextExclusionMode(static_cast<SpecializedTestTextExclusion>(value));
                }
            auto* dcProperNoun =
                dcOptionsNode->FirstChildElement(XML_PROPER_NOUN_COUNTING_METHOD.data());
            if (dcProperNoun != nullptr)
                {
                int value = dcProperNoun->ToElement()->IntAttribute(
                    XML_VALUE.data(), static_cast<int>(GetDaleChallProperNounCountingMethod()));
                if (value < 0 ||
                    value >= static_cast<int>(readability::proper_noun_counting_method::
                                                  PROPERNOUNCOUNTINGMETHOD_COUNT))
                    {
                    value = static_cast<int>(
                        readability::proper_noun_counting_method::
                            only_count_first_instance_of_proper_noun_as_unfamiliar);
                    }
                SetDaleChallProperNounCountingMethod(
                    static_cast<readability::proper_noun_counting_method>(value));
                }
            }

        // read in the standard tests
        for (auto& rTest : GetReadabilityTests().get_tests())
            {
            auto* test = readabilityTestsNode->FirstChildElement(
                wxString{ rTest.get_test().get_id().c_str() }.utf8_str());
            /* if attribute is not found then "includeValue" is set to zero for us,
               so no need to check the return value here*/
            if (test != nullptr)
                {
                rTest.include(int_to_bool(test->ToElement()->IntAttribute(
                    XML_INCLUDE.data(), bool_to_int(rTest.is_included()))));
                }
            }

        auto* test = readabilityTestsNode->FirstChildElement(XML_DOLCH_SUITE.data());
        if (test != nullptr)
            {
            m_includeDolchSightWords = int_to_bool(test->ToElement()->IntAttribute(
                XML_INCLUDE.data(), bool_to_int(m_includeDolchSightWords)));
            }
        // custom test
        GetIncludedCustomTests().clear();
        test = readabilityTestsNode->FirstChildElement(XML_CUSTOM_TEST.data());
        while (test != nullptr)
            {
            const char* testId = test->ToElement()->Attribute(XML_VALUE.data());
            if (testId != nullptr)
                {
                const auto testStr =
                    Wisteria::TextStream::CharStreamToUnicode(testId, std::strlen(testId));
                const wchar_t* convertedStr =
                    filterHtml(testStr.c_str(), testStr.length(), true, false);
                if (convertedStr != nullptr)
                    {
                    GetIncludedCustomTests().emplace_back(convertedStr);
                    }
                }
            test = test->NextSiblingElement(XML_CUSTOM_TEST.data());
            }
        }
    }

//------------------------------------------------
bool ReadabilityAppOptions::LoadOptionsFile(wxString optionsFile,
                                            const bool loadOnlyGeneralOptions /*= false*/,
                                            const bool writeChangesBackToThisFile /*= true*/)
    {
    if (writeChangesBackToThisFile)
        {
        // this is where we will save to later
        m_optionsFile = optionsFile;
        }

    wxString fileContent;
    if (!wxFile::Exists(optionsFile) || !Wisteria::TextStream::ReadFile(optionsFile, fileContent))
        {
        return false;
        }

    ResetSettings();

    m_appWindowWidth = wxSystemSettings::GetMetric(wxSYS_SCREEN_X) - 100;
    m_appWindowHeight = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y) - 100;

    lily_of_the_valley::html_extract_text filterHtml;

    const auto readString = [&filterHtml](const tinyxml2::XMLElement* node,
                                          const std::string_view xmlId,
                                          const wxString& fallbackValue)
    {
        const auto* childNode = node->FirstChildElement(xmlId.data());
        if (childNode != nullptr)
            {
            const char* stringVal =
                childNode->ToElement()->Attribute(ReadabilityAppOptions::XML_VALUE.data());
            if (stringVal != nullptr)
                {
                const auto streamedText =
                    Wisteria::TextStream::CharStreamToUnicode(stringVal, std::strlen(stringVal));
                const wchar_t* convertedStr =
                    filterHtml(streamedText.c_str(), streamedText.length(), true, false);
                if (convertedStr != nullptr)
                    {
                    return wxString{ convertedStr };
                    }
                }
            }
        return fallbackValue;
    };

    tinyxml2::XMLDocument doc;
    doc.Parse(fileContent.utf8_str());
    if (doc.Error())
        {
        // may appear while program is loading
        wxLogError(L"Unable to load configuration file:\n%s", doc.ErrorStr());
        return false;
        }
    // see if it is a valid config file
    auto* node = doc.FirstChildElement(XML_CONFIG_HEADER.data());
    if (node == nullptr)
        {
        wxMessageBox(_(L"Invalid configuration file. Project header section not found."),
                     _(L"Error"), wxOK | wxICON_ERROR);
        return false;
        }
    // read in the configurations
    auto* configRootNode = node->FirstChildElement(XML_CONFIGURATIONS.data());
    if (configRootNode == nullptr)
        {
        wxMessageBox(_(L"Invalid configuration file. No configurations found."), _(L"Error"),
                     wxOK | wxICON_ERROR);
        return false;
        }

    // general app information
    auto* customColorsNode = configRootNode->FirstChildElement(XML_CUSTOM_COLORS.data());
    if (customColorsNode != nullptr)
        {
        GetCustomColors().clear();
        GetCustomColors().reserve(MAX_CUSTOM_COLORS);
        const std::string colorStr{ _DT("color") };
        for (int i = 0; i < MAX_CUSTOM_COLORS; ++i)
            {
            const auto currentColor{ std::string{ colorStr + std::to_string(i) } };
            const auto* colorNode = customColorsNode->FirstChildElement(currentColor.c_str());
            if (colorNode != nullptr)
                {
                GetCustomColors().push_back(TiXmlNodeToColor(colorNode));
                }
            else
                {
                break;
                }
            }
        }

    // appearance of the program
    auto* appearanceNode = configRootNode->FirstChildElement(XML_APPEARANCE.data());
    LoadThemeNode(appearanceNode);

    LoadInternetNode(configRootNode);

    auto* filePathsNode = configRootNode->FirstChildElement(XML_FILE_OPEN_PATHS.data());
    if (filePathsNode != nullptr)
        {
        auto* wordlistPathNode =
            filePathsNode->FirstChildElement(XML_FILE_OPEN_WORDLIST_PATH.data());
        if (wordlistPathNode != nullptr)
            {
            const char* wordlistPathString =
                wordlistPathNode->ToElement()->Attribute(XML_VALUE.data());
            if (wordlistPathString != nullptr)
                {
                const auto wordlistPathStr = Wisteria::TextStream::CharStreamToUnicode(
                    wordlistPathString, std::strlen(wordlistPathString));
                const wchar_t* convertedStr =
                    filterHtml(wordlistPathStr.c_str(), wordlistPathStr.length(), true, false);
                if (convertedStr != nullptr)
                    {
                    SetWordListPath(convertedStr);
                    }
                }
            }
        auto* projectPathNode = filePathsNode->FirstChildElement(XML_FILE_OPEN_PROJECT_PATH.data());
        if (projectPathNode != nullptr)
            {
            const char* projectPathString =
                projectPathNode->ToElement()->Attribute(XML_VALUE.data());
            if (projectPathString != nullptr)
                {
                const auto projectPathStr = Wisteria::TextStream::CharStreamToUnicode(
                    projectPathString, std::strlen(projectPathString));
                const wchar_t* convertedStr =
                    filterHtml(projectPathStr.c_str(), projectPathStr.length(), true, false);
                if (convertedStr != nullptr)
                    {
                    SetProjectPath(convertedStr);
                    }
                }
            }
        SetImagePath(readString(filePathsNode, XML_FILE_OPEN_IMAGE_PATH,
                                wxStandardPaths::Get().GetUserDir(wxStandardPaths::Dir_Pictures)));
        SetDownloadsPath(
            readString(filePathsNode, XML_DOWNLOADS_PATH,
                       wxStandardPaths::Get().GetUserDir(wxStandardPaths::Dir_Downloads)));
        }
    // log report settings
    auto* logSettingsNode = configRootNode->FirstChildElement(XML_LOG_SETTINGS.data());
    if (logSettingsNode != nullptr)
        {
        auto* logVerboseNode = logSettingsNode->FirstChildElement(XML_LOG_VERBOSE.data());
        if (logVerboseNode != nullptr)
            {
            LogFile::SetVerbose(
                int_to_bool(logVerboseNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
            }
        auto* logAppendNode = logSettingsNode->FirstChildElement(XML_LOG_APPEND_DAILY.data());
        if (logAppendNode != nullptr)
            {
            AppendDailyLog(
                int_to_bool(logAppendNode->ToElement()->IntAttribute(XML_VALUE.data(), 0)));
            }
        auto* luaUnsafeModeNode = logSettingsNode->FirstChildElement(XML_LUA_UNSAFE_MODE.data());
        if (luaUnsafeModeNode != nullptr)
            {
            EnableLuaUnsafeMode(
                int_to_bool(luaUnsafeModeNode->ToElement()->IntAttribute(XML_VALUE.data(), 0)));
            }
        auto* showDeveloperTabNode =
            logSettingsNode->FirstChildElement(XML_SHOW_DEVELOPER_TAB.data());
        if (showDeveloperTabNode != nullptr)
            {
            ShowDeveloperTab(
                int_to_bool(showDeveloperTabNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
            }
        auto* showLogTabNode = logSettingsNode->FirstChildElement(XML_SHOW_LOG_TAB.data());
        if (showLogTabNode != nullptr)
            {
            ShowLogTab(int_to_bool(showLogTabNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
            }
        auto* logAutoRefreshNode = logSettingsNode->FirstChildElement(XML_LOG_AUTO_REFRESH.data());
        if (logAutoRefreshNode != nullptr)
            {
            SetLogAutoRefresh(
                int_to_bool(logAutoRefreshNode->ToElement()->IntAttribute(XML_VALUE.data(), 0)));
            }
        auto* disableGpuAccelerationNode =
            logSettingsNode->FirstChildElement(XML_DISABLE_GPU_ACCELERATION.data());
        if (disableGpuAccelerationNode != nullptr)
            {
            DisableGpuAcceleration(int_to_bool(
                disableGpuAccelerationNode->ToElement()->IntAttribute(XML_VALUE.data(), 0)));
            }
        }
    // printer settings
    auto* printerSettingsNode = configRootNode->FirstChildElement(XML_PRINTER_SETTINGS.data());
    if (printerSettingsNode != nullptr)
        {
        int value{ 0 };
        auto* printerNode = printerSettingsNode->FirstChildElement(XML_PRINTER_ID.data());
        if (printerNode != nullptr)
            {
            value = printerNode->ToElement()->IntAttribute(XML_VALUE.data(), value);
            SetPaperId(static_cast<wxPaperSize>(value));
            }
        value = 0;
        printerNode = printerSettingsNode->FirstChildElement(XML_PRINTER_ORIENTATION.data());
        if (printerNode != nullptr)
            {
            value = printerNode->ToElement()->IntAttribute(XML_VALUE.data(), value);
            SetPaperOrientation(static_cast<wxPrintOrientation>(value));
            }
        printerNode = printerSettingsNode->FirstChildElement(XML_PRINTER_LEFT_HEADER.data());
        if (printerNode != nullptr)
            {
            SetLeftPrinterHeader(TiXmlNodeAttributeToString(printerNode, XML_VALUE.data()));
            }
        printerNode = printerSettingsNode->FirstChildElement(XML_PRINTER_CENTER_HEADER.data());
        if (printerNode != nullptr)
            {
            SetCenterPrinterHeader(TiXmlNodeAttributeToString(printerNode, XML_VALUE.data()));
            }
        printerNode = printerSettingsNode->FirstChildElement(XML_PRINTER_RIGHT_HEADER.data());
        if (printerNode != nullptr)
            {
            SetRightPrinterHeader(TiXmlNodeAttributeToString(printerNode, XML_VALUE.data()));
            }
        printerNode = printerSettingsNode->FirstChildElement(XML_PRINTER_LEFT_FOOTER.data());
        if (printerNode != nullptr)
            {
            SetLeftPrinterFooter(TiXmlNodeAttributeToString(printerNode, XML_VALUE.data()));
            }
        printerNode = printerSettingsNode->FirstChildElement(XML_PRINTER_CENTER_FOOTER.data());
        if (printerNode != nullptr)
            {
            SetCenterPrinterFooter(TiXmlNodeAttributeToString(printerNode, XML_VALUE.data()));
            }
        printerNode = printerSettingsNode->FirstChildElement(XML_PRINTER_RIGHT_FOOTER.data());
        if (printerNode != nullptr)
            {
            SetRightPrinterFooter(TiXmlNodeAttributeToString(printerNode, XML_VALUE.data()));
            }
        auto currentWaterMark = GetWatermark();
        currentWaterMark.m_label = TiXmlNodeAttributeToString(
            printerSettingsNode->FirstChildElement(XML_GRAPH_WATERMARK.data()), XML_VALUE.data());
        string_util::remove_blank_lines(currentWaterMark.m_label);
        SetWatermark(currentWaterMark);
        }
    // editor settings
    auto* editorSettingsNode = configRootNode->FirstChildElement(XML_EDITOR.data());
    if (editorSettingsNode != nullptr)
        {
        m_editorFontColor = TiXmlNodeToColor(
            editorSettingsNode->FirstChildElement(XML_EDITOR_FONTCOLOR.data()), m_editorFontColor);
        // font
        auto* fontNode = editorSettingsNode->FirstChildElement(XML_EDITOR_FONT.data());
        if (fontNode != nullptr)
            {
            const int pointSize = fontNode->ToElement()->IntAttribute(
                XmlFormat::FONT_POINT_SIZE_TAG.data(),
                wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize());
            const int style = fontNode->ToElement()->IntAttribute(XmlFormat::FONT_STYLE_TAG.data(),
                                                                  wxFONTSTYLE_NORMAL);
            const int weight = fontNode->ToElement()->IntAttribute(
                XmlFormat::FONT_WEIGHT_TAG.data(), wxFONTWEIGHT_NORMAL);
            const int underlined =
                fontNode->ToElement()->IntAttribute(XmlFormat::FONT_UNDERLINE_TAG.data(), 0);
            // get the font point size
            m_editorFont.SetPointSize(
                (pointSize > 0) ? pointSize :
                                  wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize());
            // get the font style
            m_editorFont.SetStyle(static_cast<wxFontStyle>(style));
            // get the font weight
            m_editorFont.SetWeight(static_cast<wxFontWeight>(weight));
            // get the font underlining
            m_editorFont.SetUnderlined(int_to_bool(underlined));
            // get the font facename
            m_editorFont.SetFaceName(TiXmlNodeAttributeToString(
                fontNode, XmlFormat::FONT_FACE_NAME_TAG.data(), m_editorFont.GetFaceName()));
            }
        auto* indentNode = editorSettingsNode->FirstChildElement(XML_EDITOR_INDENT.data());
        if (indentNode != nullptr)
            {
            IndentEditor(int_to_bool(indentNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
            }
        auto* spaceAfterParagraphNode =
            editorSettingsNode->FirstChildElement(XML_EDITOR_SPACE_AFTER_PARAGRAPH.data());
        if (spaceAfterParagraphNode != nullptr)
            {
            AddParagraphSpaceInEditor(int_to_bool(
                spaceAfterParagraphNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
            }
        auto* textAlignNode =
            editorSettingsNode->FirstChildElement(XML_EDITOR_TEXT_ALIGNMENT.data());
        if (textAlignNode != nullptr)
            {
            SetEditorTextAlignment(static_cast<wxTextAttrAlignment>(
                textAlignNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
            }
        auto* lineSpacingNode =
            editorSettingsNode->FirstChildElement(XML_EDITOR_LINE_SPACING.data());
        if (lineSpacingNode != nullptr)
            {
            SetEditorLineSpacing(static_cast<wxTextAttrLineSpacing>(
                lineSpacingNode->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
            }
        }

    // if only loading general info, then quit after reading this node
    if (loadOnlyGeneralOptions)
        {
        return true;
        }

    LoadWarningsNode(configRootNode);
    LoadExportNode(configRootNode);

    // New Project Settings section
    auto* projectSettings = configRootNode->FirstChildElement(XML_PROJECT_SETTINGS.data());
    if (projectSettings != nullptr)
        {
        SetReviewer(TiXmlNodeAttributeToString(
            projectSettings->FirstChildElement(XML_REVIEWER.data()), XML_VALUE.data()));
        if (GetReviewer().empty())
            {
            SetReviewer(wxGetUserName());
            }
        auto* realTimeRefresh = projectSettings->FirstChildElement(XML_REALTIME_UPDATE.data());
        if (realTimeRefresh != nullptr)
            {
            UseRealTimeUpdate(
                int_to_bool(realTimeRefresh->ToElement()->IntAttribute(XML_VALUE.data(), 1)));
            }
        auto* appendedDocPath = projectSettings->FirstChildElement(XML_APPENDED_DOC_PATH.data());
        if (appendedDocPath != nullptr)
            {
            const char* appendedDocChars =
                appendedDocPath->ToElement()->Attribute(XML_VALUE.data());
            if (appendedDocChars != nullptr)
                {
                const auto appendedDocStr = Wisteria::TextStream::CharStreamToUnicode(
                    appendedDocChars, std::strlen(appendedDocChars));
                const wchar_t* convertedStr =
                    filterHtml(appendedDocStr.c_str(), appendedDocStr.length(), true, false);
                if (convertedStr != nullptr)
                    {
                    SetAppendedDocumentFilePath(convertedStr);
                    }
                }
            }
        // document storage/linking
        auto* docStorageNode =
            projectSettings->FirstChildElement(XML_DOCUMENT_STORAGE_METHOD.data());
        if (docStorageNode != nullptr)
            {
            m_documentStorageMethod =
                static_cast<TextStorage>(docStorageNode->ToElement()->IntAttribute(
                    XML_METHOD.data(), static_cast<int>(m_documentStorageMethod)));
            // verify that this is a sensical value
            if (m_documentStorageMethod != TextStorage::EmbedText &&
                m_documentStorageMethod != TextStorage::NoEmbedText)
                {
                m_documentStorageMethod = TextStorage::NoEmbedText;
                }
            }
        auto* projectLang = projectSettings->FirstChildElement(XML_PROJECT_LANGUAGE.data());
        if (projectLang != nullptr)
            {
            int value = projectLang->ToElement()->IntAttribute(XML_VALUE.data(),
                                                               static_cast<int>(m_language));
            if (value < 0 || value >= static_cast<decltype(value)>(
                                          readability::test_language::TEST_LANGUAGE_COUNT))
                {
                value = static_cast<decltype(value)>(readability::test_language::english_test);
                }
            m_language = static_cast<readability::test_language>(value);
            }
        auto* randomSampleSizeNode =
            projectSettings->FirstChildElement(XML_RANDOM_SAMPLE_SIZE.data());
        if (randomSampleSizeNode != nullptr)
            {
            int value = randomSampleSizeNode->ToElement()->IntAttribute(
                XML_VALUE.data(), GetBatchRandomSamplingSize());
            // verify that this is a sensical value
            if (value < 1 || value > 100)
                {
                value = 25;
                }
            SetBatchRandomSamplingSize(static_cast<size_t>(value));
            }
        auto* randomSampleEnabledNode =
            projectSettings->FirstChildElement(XML_RANDOM_SAMPLE_ENABLED.data());
        if (randomSampleEnabledNode != nullptr)
            {
            EnableRandomSampling(int_to_bool(randomSampleEnabledNode->ToElement()->IntAttribute(
                XML_VALUE.data(), static_cast<int>(IsRandomSampling()))));
            }
        auto* minDocSizeNode =
            projectSettings->FirstChildElement(XML_MIN_DOC_SIZE_FOR_BATCH.data());
        if (minDocSizeNode != nullptr)
            {
            int value = minDocSizeNode->ToElement()->IntAttribute(XML_VALUE.data(),
                                                                  GetMinDocWordCountForBatch());
            // verify that this is a sensical value
            value = std::max(value, 1);
            SetMinDocWordCountForBatch(static_cast<size_t>(value));
            }
        auto* filePathTruncModeNode =
            projectSettings->FirstChildElement(XML_FILE_PATH_TRUNC_MODE.data());
        if (filePathTruncModeNode != nullptr)
            {
            int value = filePathTruncModeNode->ToElement()->IntAttribute(
                XML_VALUE.data(), static_cast<int>(GetFilePathTruncationMode()));
            // verify that this is a sensical value
            if (value < 0 ||
                value >= static_cast<decltype(value)>(
                             Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode::
                                 COLUMN_FILE_PATHS_TRUNCATION_MODE_COUNT))
                {
                value = static_cast<decltype(value)>(
                    Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode::
                        OnlyShowFileNames);
                }
            SetFilePathTruncationMode(
                static_cast<Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode>(
                    value));
            }
        // sentences breakdown
        auto* sentencesBreakdownNode =
            projectSettings->FirstChildElement(XML_SENTENCES_BREAKDOWN.data());
        if (sentencesBreakdownNode != nullptr)
            {
            // which options are included
            GetSentencesBreakdownInfo().Set(TiXmlNodeAttributeToString(
                sentencesBreakdownNode->FirstChildElement(XML_SENTENCES_BREAKDOWN_INFO.data()),
                XML_VALUE.data(), GetSentencesBreakdownInfo().ToString()));
            }
        // words breakdown
        auto* wordsBreakdownNode = projectSettings->FirstChildElement(XML_WORDS_BREAKDOWN.data());
        if (wordsBreakdownNode != nullptr)
            {
            // which options are included
            GetWordsBreakdownInfo().Set(TiXmlNodeAttributeToString(
                wordsBreakdownNode->FirstChildElement(XML_WORDS_BREAKDOWN_INFO.data()),
                XML_VALUE.data(), GetWordsBreakdownInfo().ToString()));
            }
        // grammar
        auto* grammarNode = projectSettings->FirstChildElement(XML_GRAMMAR.data());
        if (grammarNode != nullptr)
            {
            // spell checking options
            auto* spellCheckNode =
                grammarNode->FirstChildElement(XML_SPELLCHECK_IGNORE_PROPER_NOUNS.data());
            if (spellCheckNode != nullptr)
                {
                SpellCheckIgnoreProperNouns(int_to_bool(spellCheckNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), bool_to_int(SpellCheckIsIgnoringProperNouns()))));
                }
            spellCheckNode =
                grammarNode->FirstChildElement(XML_SPELLCHECK_IGNORE_UPPERCASED.data());
            if (spellCheckNode != nullptr)
                {
                SpellCheckIgnoreUppercased(int_to_bool(spellCheckNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), bool_to_int(SpellCheckIsIgnoringUppercased()))));
                }
            spellCheckNode = grammarNode->FirstChildElement(XML_SPELLCHECK_IGNORE_NUMERALS.data());
            if (spellCheckNode != nullptr)
                {
                SpellCheckIgnoreNumerals(int_to_bool(spellCheckNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), bool_to_int(SpellCheckIsIgnoringNumerals()))));
                }
            spellCheckNode =
                grammarNode->FirstChildElement(XML_SPELLCHECK_IGNORE_FILE_ADDRESSES.data());
            if (spellCheckNode != nullptr)
                {
                SpellCheckIgnoreFileAddresses(int_to_bool(spellCheckNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), bool_to_int(SpellCheckIsIgnoringFileAddresses()))));
                }
            spellCheckNode =
                grammarNode->FirstChildElement(XML_SPELLCHECK_IGNORE_PROGRAMMER_CODE.data());
            if (spellCheckNode != nullptr)
                {
                SpellCheckIgnoreProgrammerCode(
                    int_to_bool(spellCheckNode->ToElement()->IntAttribute(
                        XML_VALUE.data(), bool_to_int(SpellCheckIsIgnoringProgrammerCode()))));
                }
            spellCheckNode =
                grammarNode->FirstChildElement(XML_SPELLCHECK_ALLOW_COLLOQUIALISMS.data());
            if (spellCheckNode != nullptr)
                {
                SpellCheckAllowColloquialisms(int_to_bool(spellCheckNode->ToElement()->IntAttribute(
                    XML_VALUE.data(), bool_to_int(SpellCheckIsAllowingColloquialisms()))));
                }
            spellCheckNode =
                grammarNode->FirstChildElement(XML_SPELLCHECK_IGNORE_SOCIAL_MEDIA_TAGS.data());
            if (spellCheckNode != nullptr)
                {
                SpellCheckIgnoreSocialMediaTags(
                    int_to_bool(spellCheckNode->ToElement()->IntAttribute(
                        XML_VALUE.data(), bool_to_int(SpellCheckIsIgnoringSocialMediaTags()))));
                }
            // which grammar options are included
            auto* grammarInfoNode = grammarNode->FirstChildElement(XML_GRAMMAR_INFO.data());
            if (grammarInfoNode != nullptr)
                {
                GetGrammarInfo().Set(TiXmlNodeAttributeToString(grammarInfoNode, XML_VALUE.data(),
                                                                GetGrammarInfo().ToString()));
                }
            }

        LoadDocAnalysisNode(projectSettings);
        LoadTestBundlesNode(projectSettings);
        LoadCustomTestsNode(projectSettings);
        LoadGraphsNode(projectSettings);
        LoadStatsNode(projectSettings);

        // wizard page defaults
        auto* wizardPageDefaultsNode =
            projectSettings->FirstChildElement(XML_WIZARD_PAGES_SETTINGS.data());
        if (wizardPageDefaultsNode != nullptr)
            {
            // batch grouping method
            auto* batchGrouping =
                wizardPageDefaultsNode->FirstChildElement(XML_WIZARD_BATCH_GROUP.data());
            if (batchGrouping != nullptr)
                {
                m_batchGroupDefault = batchGrouping->ToElement()->IntAttribute(XML_METHOD.data(),
                                                                               m_batchGroupDefault);
                }
            // text source
            auto* textSource = wizardPageDefaultsNode->FirstChildElement(XML_TEXT_SOURCE.data());
            if (textSource != nullptr)
                {
                m_textSource = static_cast<TextSource>(textSource->ToElement()->IntAttribute(
                    XML_METHOD.data(), static_cast<int>(m_textSource)));
                if (m_textSource != TextSource::FromFile && m_textSource != TextSource::EnteredText)
                    {
                    m_textSource = TextSource::FromFile;
                    }
                }
            // test recommendation
            auto* testRecommendation =
                wizardPageDefaultsNode->FirstChildElement(XML_TEST_RECOMMENDATION.data());
            if (testRecommendation != nullptr)
                {
                m_testRecommendation =
                    static_cast<TestRecommendation>(testRecommendation->ToElement()->IntAttribute(
                        XML_METHOD.data(), static_cast<int>(m_testRecommendation)));
                if (m_testRecommendation >= TestRecommendation::TEST_RECOMMENDATION_COUNT)
                    {
                    m_testRecommendation = TestRecommendation::BasedOnDocumentType;
                    }
                }
            // tests by industry
            auto* testsByIndustry =
                wizardPageDefaultsNode->FirstChildElement(XML_TEST_BY_INDUSTRY.data());
            if (testsByIndustry != nullptr)
                {
                m_testsByIndustry = static_cast<readability::industry_classification>(
                    testsByIndustry->ToElement()->IntAttribute(
                        XML_METHOD.data(), static_cast<int>(m_testsByIndustry)));
                if (m_testsByIndustry ==
                    readability::industry_classification::INDUSTRY_CLASSIFICATION_COUNT)
                    {
                    m_testsByIndustry =
                        readability::industry_classification::adult_publishing_industry;
                    }
                }
            // tests by document type
            auto* testsByDocumentType =
                wizardPageDefaultsNode->FirstChildElement(XML_TEST_BY_DOCUMENT_TYPE.data());
            if (testsByDocumentType != nullptr)
                {
                m_testsByDocumentType = static_cast<readability::document_classification>(
                    testsByDocumentType->ToElement()->IntAttribute(
                        XML_METHOD.data(), static_cast<int>(m_testsByDocumentType)));
                if (m_testsByDocumentType ==
                    readability::document_classification::DOCUMENT_CLASSIFICATION_COUNT)
                    {
                    m_testsByDocumentType =
                        readability::document_classification::adult_literature_document;
                    }
                }
            // tests by bundle
            SetSelectedTestBundle(TiXmlNodeAttributeToString(
                wizardPageDefaultsNode->FirstChildElement(XML_SELECTED_TEST_BUNDLE.data()),
                XML_VALUE.data(), GetSelectedTestBundle()));
            }

        LoadReadabilityTestsNode(projectSettings);

        // text views
        auto* textViewNode = projectSettings->FirstChildElement(XML_TEXT_VIEWS_SECTION.data());
        if (textViewNode != nullptr)
            {
            // how highlighting is done
            auto* highlightMethod = textViewNode->FirstChildElement(XML_HIGHLIGHT_METHOD.data());
            if (highlightMethod != nullptr)
                {
                m_textHighlight =
                    static_cast<TextHighlight>(highlightMethod->ToElement()->IntAttribute(
                        XML_METHOD.data(), static_cast<int>(m_textHighlight)));
                if (m_textHighlight != TextHighlight::HighlightBackground &&
                    m_textHighlight != TextHighlight::HighlightForeground)
                    {
                    m_textHighlight = TextHighlight::HighlightBackground;
                    }
                }
            // the highlight color
            m_textHighlightColor = TiXmlNodeToColor(
                textViewNode->FirstChildElement(XML_HIGHLIGHTCOLOR.data()), m_textHighlightColor);

            // the highlight color for dolch words
            auto* dolchConjunctionsColorNode =
                textViewNode->FirstChildElement(XML_DOLCH_CONJUNCTIONS_HIGHLIGHTCOLOR.data());
            if (dolchConjunctionsColorNode != nullptr)
                {
                SetDolchConjunctionsColor(
                    TiXmlNodeToColor(dolchConjunctionsColorNode, GetDolchConjunctionsColor()));

                m_highlightDolchConjunctions =
                    int_to_bool(dolchConjunctionsColorNode->ToElement()->IntAttribute(
                        XML_INCLUDE.data(), bool_to_int(m_highlightDolchConjunctions)));
                }
            auto* dolchPrepositionsColorNode =
                textViewNode->FirstChildElement(XML_DOLCH_PREPOSITIONS_HIGHLIGHTCOLOR.data());
            if (dolchPrepositionsColorNode != nullptr)
                {
                SetDolchPrepositionsColor(
                    TiXmlNodeToColor(dolchPrepositionsColorNode, GetDolchPrepositionsColor()));

                m_highlightDolchPrepositions =
                    int_to_bool(dolchPrepositionsColorNode->ToElement()->IntAttribute(
                        XML_INCLUDE.data(), bool_to_int(m_highlightDolchPrepositions)));
                }
            auto* dolchPronounsColorNode =
                textViewNode->FirstChildElement(XML_DOLCH_PRONOUNS_HIGHLIGHTCOLOR.data());
            if (dolchPronounsColorNode != nullptr)
                {
                SetDolchPronounsColor(
                    TiXmlNodeToColor(dolchPronounsColorNode, GetDolchPronounsColor()));

                m_highlightDolchPronouns =
                    int_to_bool(dolchPronounsColorNode->ToElement()->IntAttribute(
                        XML_INCLUDE.data(), bool_to_int(m_highlightDolchPronouns)));
                }
            auto* dolchAdverbsColorNode =
                textViewNode->FirstChildElement(XML_DOLCH_ADVERBS_HIGHLIGHTCOLOR.data());
            if (dolchAdverbsColorNode != nullptr)
                {
                SetDolchAdverbsColor(
                    TiXmlNodeToColor(dolchAdverbsColorNode, GetDolchAdverbsColor()));

                m_highlightDolchAdverbs =
                    int_to_bool(dolchAdverbsColorNode->ToElement()->IntAttribute(
                        XML_INCLUDE.data(), bool_to_int(m_highlightDolchAdverbs)));
                }
            auto* dolchAdjectivesColorNode =
                textViewNode->FirstChildElement(XML_DOLCH_ADJECTIVES_HIGHLIGHTCOLOR.data());
            if (dolchAdjectivesColorNode != nullptr)
                {
                SetDolchAdjectivesColor(
                    TiXmlNodeToColor(dolchAdjectivesColorNode, GetDolchAdjectivesColor()));

                m_highlightDolchAdjectives =
                    int_to_bool(dolchAdjectivesColorNode->ToElement()->IntAttribute(
                        XML_INCLUDE.data(), bool_to_int(m_highlightDolchAdjectives)));
                }
            auto* dolchVerbColorNode =
                textViewNode->FirstChildElement(XML_DOLCH_VERBS_HIGHLIGHTCOLOR.data());
            if (dolchVerbColorNode != nullptr)
                {
                SetDolchVerbsColor(TiXmlNodeToColor(dolchVerbColorNode, GetDolchVerbsColor()));

                m_highlightDolchVerbs = int_to_bool(dolchVerbColorNode->ToElement()->IntAttribute(
                    XML_INCLUDE.data(), bool_to_int(m_highlightDolchVerbs)));
                }
            auto* dolchNounColorNode =
                textViewNode->FirstChildElement(XML_DOLCH_NOUNS_HIGHLIGHTCOLOR.data());
            if (dolchNounColorNode != nullptr)
                {
                SetDolchNounsColor(TiXmlNodeToColor(dolchNounColorNode, GetDolchNounsColor()));

                m_highlightDolchNouns = int_to_bool(dolchNounColorNode->ToElement()->IntAttribute(
                    XML_INCLUDE.data(), bool_to_int(m_highlightDolchNouns)));
                }
            // the highlight color for wordy items
            m_wordyPhraseHighlightColor = TiXmlNodeToColor(
                textViewNode->FirstChildElement(XML_WORDY_PHRASE_HIGHLIGHTCOLOR.data()),
                m_wordyPhraseHighlightColor);

            // the highlight color for repeated words
            m_duplicateWordHighlightColor = TiXmlNodeToColor(
                textViewNode->FirstChildElement(XML_DUP_WORD_HIGHLIGHTCOLOR.data()),
                m_duplicateWordHighlightColor);

            // the highlight color for ignored sentences
            m_excludedTextHighlightColor = TiXmlNodeToColor(
                textViewNode->FirstChildElement(XML_EXCLUDED_HIGHLIGHTCOLOR.data()),
                m_excludedTextHighlightColor);

            // document display font information
            m_fontColor = TiXmlNodeToColor(
                textViewNode->FirstChildElement(XML_DOCUMENT_DISPLAY_FONTCOLOR.data()),
                m_fontColor);

            // font
            auto* fontNode = textViewNode->FirstChildElement(XML_DOCUMENT_DISPLAY_FONT.data());
            if (fontNode != nullptr)
                {
                const int pointSize = fontNode->ToElement()->IntAttribute(
                    XmlFormat::FONT_POINT_SIZE_TAG.data(),
                    wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize());
                const int style = fontNode->ToElement()->IntAttribute(
                    XmlFormat::FONT_STYLE_TAG.data(), wxFONTSTYLE_NORMAL);
                const int weight = fontNode->ToElement()->IntAttribute(
                    XmlFormat::FONT_WEIGHT_TAG.data(), wxFONTWEIGHT_NORMAL);
                const int underlined =
                    fontNode->ToElement()->IntAttribute(XmlFormat::FONT_UNDERLINE_TAG.data(), 0);
                // get the font point size
                m_textViewFont.SetPointSize(
                    (pointSize > 0) ?
                        pointSize :
                        wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT).GetPointSize());
                // get the font style
                m_textViewFont.SetStyle(static_cast<wxFontStyle>(style));
                // get the font weight
                m_textViewFont.SetWeight(static_cast<wxFontWeight>(weight));
                // get the font underlining
                m_textViewFont.SetUnderlined(int_to_bool(underlined));
                // get the font facename
                const char* faceName =
                    fontNode->ToElement()->Attribute(XmlFormat::FONT_FACE_NAME_TAG.data());
                if (faceName != nullptr)
                    {
                    const auto faceNameStr =
                        Wisteria::TextStream::CharStreamToUnicode(faceName, std::strlen(faceName));
                    const wchar_t* filteredText =
                        filterHtml(faceNameStr.c_str(), faceNameStr.length(), true, false);
                    if ((filteredText != nullptr) &&
                        wxFontEnumerator::IsValidFacename(filteredText))
                        {
                        m_textViewFont.SetFaceName(wxString(filteredText));
                        }
                    }
                }
            }
        }

    // fix any issues with items loaded from this file
    Wisteria::GraphItems::Label::FixFont(m_editorFont);
    Wisteria::GraphItems::Label::FixFont(m_xAxisFont);
    Wisteria::GraphItems::Label::FixFont(m_yAxisFont);
    Wisteria::GraphItems::Label::FixFont(m_topTitleFont);
    Wisteria::GraphItems::Label::FixFont(m_bottomTitleFont);
    Wisteria::GraphItems::Label::FixFont(m_leftTitleFont);
    Wisteria::GraphItems::Label::FixFont(m_rightTitleFont);
    Wisteria::GraphItems::Label::FixFont(m_textViewFont);
    return true;
    }

//------------------------------------------------
bool ReadabilityAppOptions::SaveOptionsFile(const wxString& optionsFile /*= wxString{}*/)
    {
    wxASSERT_MSG(!m_optionsFile.empty(), L"Options filepath is empty in SaveOptionsFile()!");

    constexpr lily_of_the_valley::html_encode_text ENCODE;

    tinyxml2::XMLDocument doc;
    doc.InsertEndChild(doc.NewDeclaration());

    auto* root = doc.NewElement(XML_CONFIG_HEADER.data());
    root->SetAttribute(XML_VERSION.data(), wxGetApp().GetAppVersion().utf8_str());

    auto* configSection = doc.NewElement(XML_CONFIGURATIONS.data());

    auto* customColors = doc.NewElement(XML_CUSTOM_COLORS.data());
    const std::string colorStr{ _DT("color") };
    for (int i = 0; static_cast<size_t>(i) < GetCustomColors().size(); ++i)
        {
        const auto currentColor{ std::string{ colorStr + std::to_string(i) } };
        auto* customColor = doc.NewElement(currentColor.c_str());
        customColor->SetAttribute(XmlFormat::RED_TAG.data(), GetCustomColors().at(i).IsOk() ?
                                                                 GetCustomColors().at(i).Red() :
                                                                 255);
        customColor->SetAttribute(XmlFormat::GREEN_TAG.data(), GetCustomColors().at(i).IsOk() ?
                                                                   GetCustomColors().at(i).Green() :
                                                                   255);
        customColor->SetAttribute(XmlFormat::BLUE_TAG.data(), GetCustomColors().at(i).IsOk() ?
                                                                  GetCustomColors().at(i).Blue() :
                                                                  255);
        customColors->InsertEndChild(customColor);
        }
    configSection->InsertEndChild(customColors);

    // general app information
    auto* appearance = doc.NewElement(XML_APPEARANCE.data());
    appearance->SetAttribute(XML_WINDOW_MAXIMIZED.data(), bool_to_int(IsAppWindowMaximized()));
    appearance->SetAttribute(XML_WINDOW_WIDTH.data(), GetAppWindowWidth());
    appearance->SetAttribute(XML_WINDOW_HEIGHT.data(), GetAppWindowHeight());
    appearance->SetAttribute(XML_UI_LANGUAGE.data(), static_cast<int>(GetUiLanguage()));

    configSection->InsertEndChild(appearance);

    auto* userAgent = doc.NewElement(XML_USER_AGENT.data());
    userAgent->SetAttribute(XML_VALUE.data(),
                            wxString(ENCODE({ GetUserAgent().wc_str() }, false)).utf8_str());
    configSection->InsertEndChild(userAgent);

    auto* downloadReplaceExistingNode = doc.NewElement(XML_DOWNLOAD_REPLACE_EXISTING.data());
    downloadReplaceExistingNode->SetAttribute(
        XML_VALUE.data(), bool_to_int(wxGetApp().GetWebHarvester().IsReplacingExistingFiles()));
    configSection->InsertEndChild(downloadReplaceExistingNode);

    auto* downloadWebFolderStructureNode =
        doc.NewElement(XML_DOWNLOAD_KEEP_FOLDER_STRUCTURE.data());
    downloadWebFolderStructureNode->SetAttribute(
        XML_VALUE.data(),
        bool_to_int(wxGetApp().GetWebHarvester().IsKeepingWebPathWhenDownloading()));
    configSection->InsertEndChild(downloadWebFolderStructureNode);

    auto* downloadMinSize = doc.NewElement(XML_DOWNLOAD_MIN_FILESIZE.data());
    downloadMinSize->SetAttribute(
        XML_VALUE.data(),
        wxGetApp().GetWebHarvester().GetMinimumDownloadFileSizeInKilobytes().value_or(5));
    configSection->InsertEndChild(downloadMinSize);

    auto* jsCookies = doc.NewElement(XML_USE_JS_COOKIES.data());
    jsCookies->SetAttribute(XML_VALUE.data(), bool_to_int(IsUsingJavaScriptCookies()));
    configSection->InsertEndChild(jsCookies);

    auto* persistCookies = doc.NewElement(XML_PERSIST_COOKIES.data());
    persistCookies->SetAttribute(XML_VALUE.data(), bool_to_int(IsPersistingJavaScriptCookies()));
    configSection->InsertEndChild(persistCookies);

    auto* disablePv = doc.NewElement(XML_DISABLE_PEER_VERIFY.data());
    disablePv->SetAttribute(XML_VALUE.data(), bool_to_int(IsPeerVerifyDisabled()));
    configSection->InsertEndChild(disablePv);

    // last opened file locations
    auto* filePaths = doc.NewElement(XML_FILE_OPEN_PATHS.data());
    // project path
    auto* projectPath = doc.NewElement(XML_FILE_OPEN_PROJECT_PATH.data());
    projectPath->SetAttribute(XML_VALUE.data(),
                              wxString(ENCODE({ GetProjectPath().wc_str() }, false)).utf8_str());
    filePaths->InsertEndChild(projectPath);
    // image path
    auto* imagePath = doc.NewElement(XML_FILE_OPEN_IMAGE_PATH.data());
    imagePath->SetAttribute(XML_VALUE.data(),
                            wxString(ENCODE({ GetImagePath().wc_str() }, false)).utf8_str());
    filePaths->InsertEndChild(imagePath);
    // downloads path
    auto* downloadsPath = doc.NewElement(XML_DOWNLOADS_PATH.data());
    downloadsPath->SetAttribute(
        XML_VALUE.data(), wxString(ENCODE({ GetDownloadsPath().wc_str() }, false)).utf8_str());
    filePaths->InsertEndChild(downloadsPath);
    // word list path
    auto* wordlistPath = doc.NewElement(XML_FILE_OPEN_WORDLIST_PATH.data());
    wordlistPath->SetAttribute(XML_VALUE.data(),
                               wxString(ENCODE({ GetWordListPath().wc_str() }, false)).utf8_str());
    filePaths->InsertEndChild(wordlistPath);

    configSection->InsertEndChild(filePaths);

    // warning settings
    auto* warningSection = doc.NewElement(XML_WARNING_MESSAGE_SETTINGS.data());
    for (const auto& warning : WarningManager::GetWarnings())
        {
        auto* warningItem = doc.NewElement(XML_WARNING_MESSAGE.data());
        warningItem->SetAttribute(XML_VALUE.data(), warning.GetId().utf8_str());
        warningItem->SetAttribute(XML_DISPLAY.data(), bool_to_int(warning.ShouldBeShown()));
        warningItem->SetAttribute(XML_PREVIOUS_RESPONSE.data(), warning.GetPreviousResponse());
        warningSection->InsertEndChild(warningItem);
        }
    configSection->InsertEndChild(warningSection);

    // export settings
    auto* exportSection = doc.NewElement(XML_EXPORT.data());
    // extensions
    auto* listExt = doc.NewElement(XML_EXPORT_LIST_EXT.data());
    listExt->SetAttribute(XML_VALUE.data(), BaseProjectDoc::GetExportListExt().utf8_str());
    exportSection->InsertEndChild(listExt);
    auto* textExt = doc.NewElement(XML_EXPORT_TEXT_EXT.data());
    textExt->SetAttribute(XML_VALUE.data(), BaseProjectDoc::GetExportTextViewExt().utf8_str());
    exportSection->InsertEndChild(textExt);
    auto* graphExt = doc.NewElement(XML_EXPORT_GRAPH_EXT.data());
    graphExt->SetAttribute(XML_VALUE.data(), BaseProjectDoc::GetExportGraphExt().utf8_str());
    exportSection->InsertEndChild(graphExt);
    // item inclusions
    auto* includeLists = doc.NewElement(XML_EXPORT_LISTS.data());
    includeLists->SetAttribute(XML_VALUE.data(),
                               bool_to_int(BaseProjectDoc::IsExportingHardWordLists()));
    exportSection->InsertEndChild(includeLists);
    auto* includeSentencesBreakdown = doc.NewElement(XML_EXPORT_SENTENCES_BREAKDOWN.data());
    includeSentencesBreakdown->SetAttribute(
        XML_VALUE.data(), bool_to_int(BaseProjectDoc::IsExportingSentencesBreakdown()));
    exportSection->InsertEndChild(includeSentencesBreakdown);
    auto* includeGraphs = doc.NewElement(XML_EXPORT_GRAPHS.data());
    includeGraphs->SetAttribute(XML_VALUE.data(), bool_to_int(BaseProjectDoc::IsExportingGraphs()));
    exportSection->InsertEndChild(includeGraphs);
    auto* includeTestResults = doc.NewElement(XML_EXPORT_TEST_RESULTS.data());
    includeTestResults->SetAttribute(XML_VALUE.data(),
                                     bool_to_int(BaseProjectDoc::IsExportingTestResults()));
    exportSection->InsertEndChild(includeTestResults);
    auto* includeStats = doc.NewElement(XML_EXPORT_STATS.data());
    includeStats->SetAttribute(XML_VALUE.data(),
                               bool_to_int(BaseProjectDoc::IsExportingStatistics()));
    exportSection->InsertEndChild(includeStats);
    auto* includeWordiness = doc.NewElement(XML_EXPORT_GRAMMAR.data());
    includeWordiness->SetAttribute(XML_VALUE.data(),
                                   bool_to_int(BaseProjectDoc::IsExportingWordiness()));
    exportSection->InsertEndChild(includeWordiness);
    auto* includeSightWords = doc.NewElement(XML_EXPORT_DOLCH_WORDS.data());
    includeSightWords->SetAttribute(XML_VALUE.data(),
                                    bool_to_int(BaseProjectDoc::IsExportingSightWords()));
    exportSection->InsertEndChild(includeSightWords);
    auto* includeWarnings = doc.NewElement(XML_EXPORT_WARNINGS.data());
    includeWarnings->SetAttribute(XML_VALUE.data(),
                                  bool_to_int(BaseProjectDoc::IsExportingWarnings()));
    exportSection->InsertEndChild(includeWarnings);

    configSection->InsertEndChild(exportSection);

    // log settings
    auto* logSection = doc.NewElement(XML_LOG_SETTINGS.data());
    auto* logVerbose = doc.NewElement(XML_LOG_VERBOSE.data());
    logVerbose->SetAttribute(XML_VALUE.data(), bool_to_int(wxLog::GetVerbose()));
    logSection->InsertEndChild(logVerbose);

    auto* logAppend = doc.NewElement(XML_LOG_APPEND_DAILY.data());
    logAppend->SetAttribute(XML_VALUE.data(), bool_to_int(IsAppendingDailyLog()));
    logSection->InsertEndChild(logAppend);

    auto* luaUnsafeMode = doc.NewElement(XML_LUA_UNSAFE_MODE.data());
    luaUnsafeMode->SetAttribute(XML_VALUE.data(), bool_to_int(IsLuaUnsafeModeEnabled()));
    logSection->InsertEndChild(luaUnsafeMode);

    auto* showDeveloperTab = doc.NewElement(XML_SHOW_DEVELOPER_TAB.data());
    showDeveloperTab->SetAttribute(XML_VALUE.data(), bool_to_int(IsShowingDeveloperTab()));
    logSection->InsertEndChild(showDeveloperTab);

    auto* showLogTab = doc.NewElement(XML_SHOW_LOG_TAB.data());
    showLogTab->SetAttribute(XML_VALUE.data(), bool_to_int(IsShowingLogTab()));
    logSection->InsertEndChild(showLogTab);

    auto* logAutoRefresh = doc.NewElement(XML_LOG_AUTO_REFRESH.data());
    logAutoRefresh->SetAttribute(XML_VALUE.data(), bool_to_int(IsLogAutoRefresh()));
    logSection->InsertEndChild(logAutoRefresh);

    auto* disableGpuAcceleration = doc.NewElement(XML_DISABLE_GPU_ACCELERATION.data());
    disableGpuAcceleration->SetAttribute(XML_VALUE.data(),
                                         bool_to_int(IsGpuAccelerationDisabled()));
    logSection->InsertEndChild(disableGpuAcceleration);

    configSection->InsertEndChild(logSection);

    // printer settings
    auto* printerSection = doc.NewElement(XML_PRINTER_SETTINGS.data());
    // page setup
    auto* paperId = doc.NewElement(XML_PRINTER_ID.data());
    paperId->SetAttribute(XML_VALUE.data(), static_cast<int>(GetPaperId()));
    printerSection->InsertEndChild(paperId);
    auto* paperOrientation = doc.NewElement(XML_PRINTER_ORIENTATION.data());
    paperOrientation->SetAttribute(XML_VALUE.data(), static_cast<int>(GetPaperOrientation()));
    printerSection->InsertEndChild(paperOrientation);
    // headers
    auto* printerLeftHeader = doc.NewElement(XML_PRINTER_LEFT_HEADER.data());
    printerLeftHeader->SetAttribute(
        XML_VALUE.data(), wxString(ENCODE({ GetLeftPrinterHeader().wc_str() }, false)).utf8_str());
    printerSection->InsertEndChild(printerLeftHeader);
    auto* printerCenterHeader = doc.NewElement(XML_PRINTER_CENTER_HEADER.data());
    printerCenterHeader->SetAttribute(
        XML_VALUE.data(),
        wxString(ENCODE({ GetCenterPrinterHeader().wc_str() }, false)).utf8_str());
    printerSection->InsertEndChild(printerCenterHeader);
    auto* printerRightHeader = doc.NewElement(XML_PRINTER_RIGHT_HEADER.data());
    printerRightHeader->SetAttribute(
        XML_VALUE.data(), wxString(ENCODE({ GetRightPrinterHeader().wc_str() }, false)).utf8_str());
    printerSection->InsertEndChild(printerRightHeader);
    // footers
    auto* printerLeftFooter = doc.NewElement(XML_PRINTER_LEFT_FOOTER.data());
    printerLeftFooter->SetAttribute(
        XML_VALUE.data(), wxString(ENCODE({ GetLeftPrinterFooter().wc_str() }, false)).utf8_str());
    printerSection->InsertEndChild(printerLeftFooter);
    auto* printerCenterFooter = doc.NewElement(XML_PRINTER_CENTER_FOOTER.data());
    printerCenterFooter->SetAttribute(
        XML_VALUE.data(),
        wxString(ENCODE({ GetCenterPrinterFooter().wc_str() }, false)).utf8_str());
    printerSection->InsertEndChild(printerCenterFooter);
    auto* printerRightFooter = doc.NewElement(XML_PRINTER_RIGHT_FOOTER.data());
    printerRightFooter->SetAttribute(
        XML_VALUE.data(), wxString(ENCODE({ GetRightPrinterFooter().wc_str() }, false)).utf8_str());
    printerSection->InsertEndChild(printerRightFooter);
    auto* printerWatermark = doc.NewElement(XML_GRAPH_WATERMARK.data());
    printerWatermark->SetAttribute(
        XML_VALUE.data(), wxString(ENCODE({ GetWatermark().m_label.wc_str() }, false)).utf8_str());
    printerSection->InsertEndChild(printerWatermark);
    configSection->InsertEndChild(printerSection);

        // editor section
        {
        auto* editorSection = doc.NewElement(XML_EDITOR.data());
        auto* fontcolor = doc.NewElement(XML_EDITOR_FONTCOLOR.data());
        fontcolor->SetAttribute(XmlFormat::RED_TAG.data(), m_editorFontColor.Red());
        fontcolor->SetAttribute(XmlFormat::GREEN_TAG.data(), m_editorFontColor.Green());
        fontcolor->SetAttribute(XmlFormat::BLUE_TAG.data(), m_editorFontColor.Blue());
        editorSection->InsertEndChild(fontcolor);

        auto* font = doc.NewElement(XML_EDITOR_FONT.data());
        font->SetAttribute(XmlFormat::FONT_POINT_SIZE_TAG.data(), m_editorFont.GetPointSize());
        font->SetAttribute(XmlFormat::FONT_STYLE_TAG.data(),
                           static_cast<int>(m_editorFont.GetStyle()));
        font->SetAttribute(XmlFormat::FONT_WEIGHT_TAG.data(),
                           static_cast<int>(m_editorFont.GetWeight()));
        font->SetAttribute(XmlFormat::FONT_UNDERLINE_TAG.data(),
                           bool_to_int(m_editorFont.GetUnderlined()));
        font->SetAttribute(
            XmlFormat::FONT_FACE_NAME_TAG.data(),
            wxString(ENCODE({ m_editorFont.GetFaceName().wc_str() }, false)).utf8_str());
        editorSection->InsertEndChild(font);

        auto* indent = doc.NewElement(XML_EDITOR_INDENT.data());
        indent->SetAttribute(XML_VALUE.data(), bool_to_int(IsEditorIndenting()));
        editorSection->InsertEndChild(indent);

        auto* spaceAfterParagraph = doc.NewElement(XML_EDITOR_SPACE_AFTER_PARAGRAPH.data());
        spaceAfterParagraph->SetAttribute(XML_VALUE.data(),
                                          bool_to_int(IsEditorShowSpaceAfterParagraph()));
        editorSection->InsertEndChild(spaceAfterParagraph);

        auto* textAlignment = doc.NewElement(XML_EDITOR_TEXT_ALIGNMENT.data());
        textAlignment->SetAttribute(XML_VALUE.data(), static_cast<int>(GetEditorTextAlignment()));
        editorSection->InsertEndChild(textAlignment);

        auto* lineSpacing = doc.NewElement(XML_EDITOR_LINE_SPACING.data());
        lineSpacing->SetAttribute(XML_VALUE.data(), static_cast<int>(GetEditorLineSpacing()));
        editorSection->InsertEndChild(lineSpacing);

        configSection->InsertEndChild(editorSection);
        }

    // New Project Settings
    auto* projectSettings = doc.NewElement(XML_PROJECT_SETTINGS.data());

    // reviewer and status
    auto* docReviewer = doc.NewElement(XML_REVIEWER.data());
    docReviewer->SetAttribute(XML_VALUE.data(),
                              wxString(ENCODE({ GetReviewer().wc_str() }, false)).utf8_str());
    projectSettings->InsertEndChild(docReviewer);

    auto* realTimeRefresh = doc.NewElement(XML_REALTIME_UPDATE.data());
    realTimeRefresh->SetAttribute(XML_VALUE.data(), bool_to_int(IsRealTimeUpdating()));
    projectSettings->InsertEndChild(realTimeRefresh);

    // appended file
    auto* appendedDocPath = doc.NewElement(XML_APPENDED_DOC_PATH.data());
    appendedDocPath->SetAttribute(
        XML_VALUE.data(),
        wxString(ENCODE({ GetAppendedDocumentFilePath().wc_str() }, false)).utf8_str());
    projectSettings->InsertEndChild(appendedDocPath);

    // document storage/linking
    auto* docStorageMethod = doc.NewElement(XML_DOCUMENT_STORAGE_METHOD.data());
    docStorageMethod->SetAttribute(XML_METHOD.data(), static_cast<int>(m_documentStorageMethod));
    projectSettings->InsertEndChild(docStorageMethod);

    // Project language
    auto* projectLang = doc.NewElement(XML_PROJECT_LANGUAGE.data());
    projectLang->SetAttribute(XML_VALUE.data(), static_cast<int>(GetProjectLanguage()));
    projectSettings->InsertEndChild(projectLang);

    // min doc size
    auto* minDocSize = doc.NewElement(XML_MIN_DOC_SIZE_FOR_BATCH.data());
    minDocSize->SetAttribute(XML_VALUE.data(), static_cast<int>(GetMinDocWordCountForBatch()));
    projectSettings->InsertEndChild(minDocSize);

    // how file paths are shown in batch projects
    auto* filePathTruncMode = doc.NewElement(XML_FILE_PATH_TRUNC_MODE.data());
    filePathTruncMode->SetAttribute(XML_VALUE.data(),
                                    static_cast<int>(GetFilePathTruncationMode()));
    projectSettings->InsertEndChild(filePathTruncMode);

    // random sampling size for batch projects
    auto* randSampleSize = doc.NewElement(XML_RANDOM_SAMPLE_SIZE.data());
    randSampleSize->SetAttribute(XML_VALUE.data(), static_cast<int>(GetBatchRandomSamplingSize()));
    projectSettings->InsertEndChild(randSampleSize);

    auto* randSampleEnabled = doc.NewElement(XML_RANDOM_SAMPLE_ENABLED.data());
    randSampleEnabled->SetAttribute(XML_VALUE.data(), bool_to_int(IsRandomSampling()));
    projectSettings->InsertEndChild(randSampleEnabled);

    // sentences breakdown
    auto* sentencesBreakdownSection = doc.NewElement(XML_SENTENCES_BREAKDOWN.data());
    // which features are included
    auto* sentencesBreakdownInfo = doc.NewElement(XML_SENTENCES_BREAKDOWN_INFO.data());
    sentencesBreakdownInfo->SetAttribute(XML_VALUE.data(),
                                         GetSentencesBreakdownInfo().ToString().utf8_str());
    sentencesBreakdownSection->InsertEndChild(sentencesBreakdownInfo);
    projectSettings->InsertEndChild(sentencesBreakdownSection);

    // words breakdown
    auto* wordsBreakdownSection = doc.NewElement(XML_WORDS_BREAKDOWN.data());
    // which features are included
    auto* wordsBreakdownInfo = doc.NewElement(XML_WORDS_BREAKDOWN_INFO.data());
    wordsBreakdownInfo->SetAttribute(XML_VALUE.data(),
                                     GetWordsBreakdownInfo().ToString().utf8_str());
    wordsBreakdownSection->InsertEndChild(wordsBreakdownInfo);
    projectSettings->InsertEndChild(wordsBreakdownSection);

    // grammar settings
    auto* grammarSection = doc.NewElement(XML_GRAMMAR.data());

    auto* spellCheckIgnoreProperNounsMethod =
        doc.NewElement(XML_SPELLCHECK_IGNORE_PROPER_NOUNS.data());
    spellCheckIgnoreProperNounsMethod->SetAttribute(XML_VALUE.data(),
                                                    bool_to_int(SpellCheckIsIgnoringProperNouns()));
    grammarSection->InsertEndChild(spellCheckIgnoreProperNounsMethod);

    auto* spellCheckIgnoreUppercasedMethod =
        doc.NewElement(XML_SPELLCHECK_IGNORE_UPPERCASED.data());
    spellCheckIgnoreUppercasedMethod->SetAttribute(XML_VALUE.data(),
                                                   bool_to_int(SpellCheckIsIgnoringUppercased()));
    grammarSection->InsertEndChild(spellCheckIgnoreUppercasedMethod);

    auto* spellCheckIgnoreNumeralsMethod = doc.NewElement(XML_SPELLCHECK_IGNORE_NUMERALS.data());
    spellCheckIgnoreNumeralsMethod->SetAttribute(XML_VALUE.data(),
                                                 bool_to_int(SpellCheckIsIgnoringNumerals()));
    grammarSection->InsertEndChild(spellCheckIgnoreNumeralsMethod);

    auto* spellCheckIgnoreFileAddressMethod =
        doc.NewElement(XML_SPELLCHECK_IGNORE_FILE_ADDRESSES.data());
    spellCheckIgnoreFileAddressMethod->SetAttribute(
        XML_VALUE.data(), bool_to_int(SpellCheckIsIgnoringFileAddresses()));
    grammarSection->InsertEndChild(spellCheckIgnoreFileAddressMethod);

    auto* spellCheckIgnoreProgrammerCodeMethod =
        doc.NewElement(XML_SPELLCHECK_IGNORE_PROGRAMMER_CODE.data());
    spellCheckIgnoreProgrammerCodeMethod->SetAttribute(
        XML_VALUE.data(), bool_to_int(SpellCheckIsIgnoringProgrammerCode()));
    grammarSection->InsertEndChild(spellCheckIgnoreProgrammerCodeMethod);

    auto* spellCheckAllowColloquialismsMethod =
        doc.NewElement(XML_SPELLCHECK_ALLOW_COLLOQUIALISMS.data());
    spellCheckAllowColloquialismsMethod->SetAttribute(
        XML_VALUE.data(), bool_to_int(SpellCheckIsAllowingColloquialisms()));
    grammarSection->InsertEndChild(spellCheckAllowColloquialismsMethod);

    auto* spellCheckIgnoreSocialMediaTagsMethod =
        doc.NewElement(XML_SPELLCHECK_IGNORE_SOCIAL_MEDIA_TAGS.data());
    spellCheckIgnoreSocialMediaTagsMethod->SetAttribute(
        XML_VALUE.data(), bool_to_int(SpellCheckIsIgnoringSocialMediaTags()));
    grammarSection->InsertEndChild(spellCheckIgnoreSocialMediaTagsMethod);

    // which grammar features are included
    auto* grammarInfo = doc.NewElement(XML_GRAMMAR_INFO.data());
    grammarInfo->SetAttribute(XML_VALUE.data(), GetGrammarInfo().ToString().utf8_str());
    grammarSection->InsertEndChild(grammarInfo);

    projectSettings->InsertEndChild(grammarSection);

    // document analysis settings
    auto* documentAnalysisSection = doc.NewElement(XML_DOCUMENT_ANALYSIS_LOGIC.data());

    // sentence information
    auto* sentenceMethod = doc.NewElement(XML_LONG_SENTENCE_METHOD.data());
    sentenceMethod->SetAttribute(XML_METHOD.data(), static_cast<int>(GetLongSentenceMethod()));
    documentAnalysisSection->InsertEndChild(sentenceMethod);

    auto* sentenceLength = doc.NewElement(XML_LONG_SENTENCE_LENGTH.data());
    sentenceLength->SetAttribute(XML_VALUE.data(), GetDifficultSentenceLength());
    documentAnalysisSection->InsertEndChild(sentenceLength);

    // numeral syllabication
    auto* numSyllMethod = doc.NewElement(XML_NUMERAL_SYLLABICATION_METHOD.data());
    numSyllMethod->SetAttribute(XML_METHOD.data(), static_cast<int>(m_numeralSyllabicationMethod));
    documentAnalysisSection->InsertEndChild(numSyllMethod);

    // whether to ignore blank lines when figuring out if an
    // incomplete sentences is end of paragraph
    auto* ignoreBlankLinesMethod =
        doc.NewElement(XML_IGNORE_BLANK_LINES_FOR_PARAGRAPH_PARSING.data());
    ignoreBlankLinesMethod->SetAttribute(XML_VALUE.data(),
                                         bool_to_int(m_ignoreBlankLinesForParagraphsParser));
    documentAnalysisSection->InsertEndChild(ignoreBlankLinesMethod);

    // whether we should ignore indenting when parsing paragraphs
    auto* ignoreIndentsMethod = doc.NewElement(XML_IGNORE_INDENTING_FOR_PARAGRAPH_PARSING.data());
    ignoreIndentsMethod->SetAttribute(XML_VALUE.data(),
                                      bool_to_int(m_ignoreIndentingForParagraphsParser));
    documentAnalysisSection->InsertEndChild(ignoreIndentsMethod);

    // whether sentences must start capitalized
    auto* sentenceStartMustBeUppercased =
        doc.NewElement(XML_SENTENCES_MUST_START_CAPITALIZED.data());
    sentenceStartMustBeUppercased->SetAttribute(XML_VALUE.data(),
                                                bool_to_int(GetSentenceStartMustBeUppercased()));
    documentAnalysisSection->InsertEndChild(sentenceStartMustBeUppercased);

    // file path to phrases to exclude from analysis
    auto* excludedPhrasesFilePath = doc.NewElement(XML_EXCLUDED_PHRASES_PATH.data());
    excludedPhrasesFilePath->SetAttribute(
        XML_VALUE.data(),
        wxString(ENCODE({ GetExcludedPhrasesPath().wc_str() }, false)).utf8_str());
    documentAnalysisSection->InsertEndChild(excludedPhrasesFilePath);

    // exclusion block tags
    auto* excludeTagsSection = doc.NewElement(XML_EXCLUDE_BLOCK_TAGS.data());
    for (const auto& exclusionBlockTag : m_exclusionBlockTags)
        {
        auto* excludeTags = doc.NewElement(XML_EXCLUDE_BLOCK_TAG.data());
        const std::array<wchar_t, 3> excludeTagsStr{ exclusionBlockTag.first,
                                                     exclusionBlockTag.second, L'\0' };

        const wxString excludeTagsEncoded = ENCODE({ excludeTagsStr.data(), 2 }, false).c_str();
        excludeTags->SetAttribute(XML_VALUE.data(), excludeTagsEncoded.utf8_str());
        excludeTagsSection->InsertEndChild(excludeTags);
        }
    documentAnalysisSection->InsertEndChild(excludeTagsSection);

    // whether to include first occurrence of excluded phrases
    auto* includeExcludedPhraseFirstOccurrence =
        doc.NewElement(XML_EXCLUDED_PHRASES_INCLUDE_FIRST_OCCURRENCE.data());
    includeExcludedPhraseFirstOccurrence->SetAttribute(
        XML_VALUE.data(), bool_to_int(IsIncludingExcludedPhraseFirstOccurrence()));
    documentAnalysisSection->InsertEndChild(includeExcludedPhraseFirstOccurrence);

    // whether to ignore Proper Nouns
    auto* ignoreProperNouns = doc.NewElement(XML_IGNORE_PROPER_NOUNS.data());
    ignoreProperNouns->SetAttribute(XML_VALUE.data(), bool_to_int(IsExcludingProperNouns()));
    documentAnalysisSection->InsertEndChild(ignoreProperNouns);

    // whether to ignore numerals
    auto* ignoreNumerals = doc.NewElement(XML_IGNORE_NUMERALS.data());
    ignoreNumerals->SetAttribute(XML_VALUE.data(), bool_to_int(IsExcludingNumerals()));
    documentAnalysisSection->InsertEndChild(ignoreNumerals);

    // whether to ignore file addresses
    auto* ignoreFileAddress = doc.NewElement(XML_IGNORE_FILE_ADDRESSES.data());
    ignoreFileAddress->SetAttribute(XML_VALUE.data(), bool_to_int(IsExcludingFileAddresses()));
    documentAnalysisSection->InsertEndChild(ignoreFileAddress);

    // whether to ignore citations
    auto* ignoreCitations = doc.NewElement(XML_IGNORE_CITATIONS.data());
    ignoreCitations->SetAttribute(XML_VALUE.data(), bool_to_int(IsExcludingTrailingCitations()));
    documentAnalysisSection->InsertEndChild(ignoreCitations);

    // whether to aggressively exclude
    auto* aggressiveExclusion = doc.NewElement(XML_AGGRESSIVE_EXCLUSION.data());
    aggressiveExclusion->SetAttribute(XML_VALUE.data(), bool_to_int(IsExcludingAggressively()));
    documentAnalysisSection->InsertEndChild(aggressiveExclusion);

    // whether to ignore copyright notices
    auto* ignoreCopyrightNotices = doc.NewElement(XML_IGNORE_COPYRIGHT_NOTICES.data());
    ignoreCopyrightNotices->SetAttribute(
        XML_VALUE.data(), bool_to_int(IsExcludingTrailingCopyrightNoticeParagraphs()));
    documentAnalysisSection->InsertEndChild(ignoreCopyrightNotices);

    // paragraph parsing
    auto* paraParsingMethod = doc.NewElement(XML_PARAGRAPH_PARSING_METHOD.data());
    paraParsingMethod->SetAttribute(XML_METHOD.data(), static_cast<int>(m_paragraphsParsingMethod));
    documentAnalysisSection->InsertEndChild(paraParsingMethod);

    // Number of words that will make an incomplete sentence actually complete
    auto* includeIncompleteSentencesIfLongerThanValue =
        doc.NewElement(XML_INCLUDE_INCOMPLETE_SENTENCES_LONGER_THAN.data());
    includeIncompleteSentencesIfLongerThanValue->SetAttribute(
        XML_VALUE.data(), static_cast<int>(GetIncludeIncompleteSentencesIfLongerThanValue()));
    documentAnalysisSection->InsertEndChild(includeIncompleteSentencesIfLongerThanValue);

    // how to handle lists and header sentences
    auto* invalidSentenceMethod = doc.NewElement(XML_INVALID_SENTENCE_METHOD.data());
    invalidSentenceMethod->SetAttribute(XML_METHOD.data(),
                                        static_cast<int>(GetInvalidSentenceMethod()));
    documentAnalysisSection->InsertEndChild(invalidSentenceMethod);
    projectSettings->InsertEndChild(documentAnalysisSection);

    // test bundles
    auto* testBundlesSection = doc.NewElement(XML_TEST_BUNDLES.data());
    for (const auto& currentBundle : BaseProject::m_testBundles)
        {
        // don't bother saving system bundles, those are dynamically added
        // by the program when it starts up
        if (currentBundle.IsLocked())
            {
            continue;
            }
        auto* testBundle = doc.NewElement(XML_TEST_BUNDLE.data());
        // name
        auto* testBundleName = doc.NewElement(XML_TEST_BUNDLE_NAME.data());
        const wxString testBundleNameEncoded =
            // NOLINTNEXTLINE(readability-redundant-string-cstr)
            ENCODE({ currentBundle.GetName().c_str() }, false).c_str();
        testBundleName->SetAttribute(XML_VALUE.data(), testBundleNameEncoded.utf8_str());
        testBundle->InsertEndChild(testBundleName);
        // description
        auto* testBundleDescription = doc.NewElement(XML_TEST_BUNDLE_DESCRIPTION.data());
        const wxString testBundleDescriptionEncoded =
            // NOLINTNEXTLINE(readability-redundant-string-cstr)
            ENCODE({ currentBundle.GetDescription().c_str() }, false).c_str();
        testBundleDescription->SetAttribute(XML_VALUE.data(),
                                            testBundleDescriptionEncoded.utf8_str());
        testBundle->InsertEndChild(testBundleDescription);
        // included tests
        auto* testNames = doc.NewElement(XML_TEST_NAMES.data());
        for (const auto& bundledTest : currentBundle.GetTestGoals())
            {
            auto* testName = doc.NewElement(XML_TEST_NAME.data());
            const wxString testNameEncoded =
                // NOLINTNEXTLINE(readability-redundant-string-cstr)
                ENCODE({ bundledTest.GetName().c_str() }, false).c_str();
            testName->SetAttribute(XML_VALUE.data(), testNameEncoded.utf8_str());
            testName->SetAttribute(
                XML_GOAL_MIN_VAL_GOAL.data(),
                (std::isnan(bundledTest.GetMinGoal()) ?
                     "" :
                     wxString::FromCDouble(bundledTest.GetMinGoal()).utf8_str()));
            testName->SetAttribute(
                XML_GOAL_MAX_VAL_GOAL.data(),
                (std::isnan(bundledTest.GetMaxGoal()) ?
                     "" :
                     wxString::FromCDouble(bundledTest.GetMaxGoal()).utf8_str()));
            testNames->InsertEndChild(testName);
            }
        testBundle->InsertEndChild(testNames);
        // stats goals
        auto* statGoals = doc.NewElement(XML_BUNDLE_STATISTICS.data());
        for (const auto& bundledStat : currentBundle.GetStatGoals())
            {
            auto* statGoal = doc.NewElement(XML_BUNDLE_STATISTIC.data());
            const wxString testNameEncoded =
                // NOLINTNEXTLINE(readability-redundant-string-cstr)
                ENCODE({ bundledStat.GetName().c_str() }, false).c_str();
            statGoal->SetAttribute(XML_VALUE.data(), testNameEncoded.utf8_str());
            statGoal->SetAttribute(
                XML_GOAL_MIN_VAL_GOAL.data(),
                (std::isnan(bundledStat.GetMinGoal()) ?
                     "" :
                     wxString::FromCDouble(bundledStat.GetMinGoal()).utf8_str()));
            statGoal->SetAttribute(
                XML_GOAL_MAX_VAL_GOAL.data(),
                (std::isnan(bundledStat.GetMaxGoal()) ?
                     "" :
                     wxString::FromCDouble(bundledStat.GetMaxGoal()).utf8_str()));
            statGoals->InsertEndChild(statGoal);
            }
        testBundle->InsertEndChild(statGoals);

        testBundlesSection->InsertEndChild(testBundle);
        }
    projectSettings->InsertEndChild(testBundlesSection);

    // custom tests
    auto* customTestsSection = doc.NewElement(XML_CUSTOM_TESTS.data());
    // add all global custom familiar word lists
    for (const auto& customWordTest : BaseProject::m_custom_word_tests)
        {
        auto* customFamWordTest = doc.NewElement(XML_CUSTOM_FAMILIAR_WORD_TEST.data());
        // name
        auto* testName = doc.NewElement(XML_TEST_NAME.data());
        wxString testNameEncoded(customWordTest.get_name().c_str());
        testNameEncoded = ENCODE({ testNameEncoded.wc_str() }, false).c_str();
        testName->SetAttribute(XML_VALUE.data(), testNameEncoded.utf8_str());
        customFamWordTest->InsertEndChild(testName);
        // file path
        auto* filePath = doc.NewElement(XML_FAMILIAR_WORD_FILE_PATH.data());
        wxString filePathEncoded(customWordTest.get_familiar_word_list_file_path().c_str());
        filePathEncoded = ENCODE({ filePathEncoded.wc_str() }, false).c_str();
        filePath->SetAttribute(XML_VALUE.data(), filePathEncoded.utf8_str());
        customFamWordTest->InsertEndChild(filePath);
        // formula (needs to be stored in U.S. format for portability)
        auto* formula = doc.NewElement(XML_TEST_FORMULA.data());
        wxString formulaEncoded(
            FormulaFormat::FormatMathExpressionToUS(customWordTest.get_formula().c_str()));
        formulaEncoded = ENCODE({ formulaEncoded.wc_str() }, false).c_str();
        formula->SetAttribute(XML_VALUE.data(), formulaEncoded.utf8_str());
        customFamWordTest->InsertEndChild(formula);
        // formula type (this is just needed for forward compatibility)
        const int formulaTypeValue =
            (wxString(customWordTest.get_formula().c_str())
                 .CmpNoCase(ReadabilityFormulaParser::GetCustomSpacheSignature()) == 0) ?
                1 :
                0;
        auto* formulaType = doc.NewElement(XML_TEST_FORMULA_TYPE.data());
        formulaType->SetAttribute(XML_VALUE.data(), formulaTypeValue);
        customFamWordTest->InsertEndChild(formulaType);
        // test type
        auto* testType = doc.NewElement(XML_TEST_TYPE.data());
        testType->SetAttribute(XML_VALUE.data(), static_cast<int>(customWordTest.get_test_type()));
        customFamWordTest->InsertEndChild(testType);
        // stemming type
        auto* stemming = doc.NewElement(XML_STEMMING_TYPE.data());
        stemming->SetAttribute(XML_VALUE.data(),
                               static_cast<int>(customWordTest.get_stemming_type()));
        customFamWordTest->InsertEndChild(stemming);
        // include proper nouns
        auto* includeProperNoun = doc.NewElement(XML_INCLUDE_PROPER_NOUNS.data());
        includeProperNoun->SetAttribute(XML_VALUE.data(),
                                        static_cast<int>(customWordTest.get_proper_noun_method()));
        customFamWordTest->InsertEndChild(includeProperNoun);
        // include numeric
        auto* includeNumeric = doc.NewElement(XML_INCLUDE_NUMERIC.data());
        includeNumeric->SetAttribute(
            XML_VALUE.data(), static_cast<int>(customWordTest.is_including_numeric_as_familiar()));
        customFamWordTest->InsertEndChild(includeNumeric);
        // including custom list
        auto* customWordList = doc.NewElement(XML_INCLUDE_CUSTOM_WORD_LIST.data());
        customWordList->SetAttribute(
            XML_VALUE.data(),
            static_cast<int>(customWordTest.is_including_custom_familiar_word_list()));
        customFamWordTest->InsertEndChild(customWordList);
        // including DC list
        auto* dcTest = doc.NewElement(XML_INCLUDE_DC_LIST.data());
        dcTest->SetAttribute(XML_VALUE.data(),
                             static_cast<int>(customWordTest.is_including_dale_chall_list()));
        customFamWordTest->InsertEndChild(dcTest);
        // including Spache list
        auto* spacheTest = doc.NewElement(XML_INCLUDE_SPACHE_LIST.data());
        spacheTest->SetAttribute(XML_VALUE.data(),
                                 static_cast<int>(customWordTest.is_including_spache_list()));
        customFamWordTest->InsertEndChild(spacheTest);
        // including Stocker list
        auto* stockerList = doc.NewElement(XML_INCLUDE_STOCKER_LIST.data());
        stockerList->SetAttribute(XML_VALUE.data(),
                                  static_cast<int>(customWordTest.is_including_stocker_list()));
        customFamWordTest->InsertEndChild(stockerList);
        // including HJ list
        auto* hjList = doc.NewElement(XML_INCLUDE_HARRIS_JACOBSON_LIST.data());
        hjList->SetAttribute(XML_VALUE.data(),
                             static_cast<int>(customWordTest.is_including_harris_jacobson_list()));
        customFamWordTest->InsertEndChild(hjList);
        // whether familiar words have to be on each included list
        auto* famAllLists = doc.NewElement(XML_FAMILIAR_WORDS_ALL_LISTS.data());
        famAllLists->SetAttribute(
            XML_VALUE.data(),
            static_cast<int>(customWordTest.is_familiar_words_must_be_on_each_included_list()));
        customFamWordTest->InsertEndChild(famAllLists);
        // industry association
        auto* industryChildrenPub = doc.NewElement(XML_INDUSTRY_CHILDRENS_PUBLISHING.data());
        industryChildrenPub->SetAttribute(
            XML_VALUE.data(),
            static_cast<int>(customWordTest.has_industry_classification(
                readability::industry_classification::childrens_publishing_industry)));
        customFamWordTest->InsertEndChild(industryChildrenPub);

        auto* industryAdultPublishing = doc.NewElement(XML_INDUSTRY_ADULTPUBLISHING.data());
        industryAdultPublishing->SetAttribute(
            XML_VALUE.data(),
            static_cast<int>(customWordTest.has_industry_classification(
                readability::industry_classification::adult_publishing_industry)));
        customFamWordTest->InsertEndChild(industryAdultPublishing);

        auto* industrySecondaryLanguage = doc.NewElement(XML_INDUSTRY_SECONDARY_LANGUAGE.data());
        industrySecondaryLanguage->SetAttribute(
            XML_VALUE.data(),
            static_cast<int>(customWordTest.has_industry_classification(
                readability::industry_classification::secondary_language_industry)));
        customFamWordTest->InsertEndChild(industrySecondaryLanguage);

        auto* industryChildrensHealthCare =
            doc.NewElement(XML_INDUSTRY_CHILDRENS_HEALTHCARE.data());
        industryChildrensHealthCare->SetAttribute(
            XML_VALUE.data(),
            static_cast<int>(customWordTest.has_industry_classification(
                readability::industry_classification::childrens_healthcare_industry)));
        customFamWordTest->InsertEndChild(industryChildrensHealthCare);

        auto* industryAdultHealthCare = doc.NewElement(XML_INDUSTRY_ADULT_HEALTHCARE.data());
        industryAdultHealthCare->SetAttribute(
            XML_VALUE.data(),
            static_cast<int>(customWordTest.has_industry_classification(
                readability::industry_classification::adult_healthcare_industry)));
        customFamWordTest->InsertEndChild(industryAdultHealthCare);

        auto* industryMilitary = doc.NewElement(XML_INDUSTRY_MILITARY_GOVERNMENT.data());
        industryMilitary->SetAttribute(
            XML_VALUE.data(),
            static_cast<int>(customWordTest.has_industry_classification(
                readability::industry_classification::military_government_industry)));
        customFamWordTest->InsertEndChild(industryMilitary);

        auto* industryBroadCasting = doc.NewElement(XML_INDUSTRY_BROADCASTING.data());
        industryBroadCasting->SetAttribute(
            XML_VALUE.data(), static_cast<int>(customWordTest.has_industry_classification(
                                  readability::industry_classification::broadcasting_industry)));
        customFamWordTest->InsertEndChild(industryBroadCasting);

        auto* industryGeneralDocument = doc.NewElement(XML_DOCUMENT_GENERAL.data());
        industryGeneralDocument->SetAttribute(
            XML_VALUE.data(), static_cast<int>(customWordTest.has_document_classification(
                                  readability::document_classification::general_document)));
        customFamWordTest->InsertEndChild(industryGeneralDocument);

        auto* industryTechnicalDocument = doc.NewElement(XML_DOCUMENT_TECHNICAL.data());
        industryTechnicalDocument->SetAttribute(
            XML_VALUE.data(), static_cast<int>(customWordTest.has_document_classification(
                                  readability::document_classification::technical_document)));
        customFamWordTest->InsertEndChild(industryTechnicalDocument);

        auto* industryForm = doc.NewElement(XML_DOCUMENT_FORM.data());
        industryForm->SetAttribute(
            XML_VALUE.data(), static_cast<int>(customWordTest.has_document_classification(
                                  readability::document_classification::nonnarrative_document)));
        customFamWordTest->InsertEndChild(industryForm);

        auto* industryYoungAdultAndAdultLiterature = doc.NewElement(XML_DOCUMENT_YOUNGADULT.data());
        industryYoungAdultAndAdultLiterature->SetAttribute(
            XML_VALUE.data(),
            static_cast<int>(customWordTest.has_document_classification(
                readability::document_classification::adult_literature_document)));
        customFamWordTest->InsertEndChild(industryYoungAdultAndAdultLiterature);

        auto* industryChildrensLiterature = doc.NewElement(XML_DOCUMENT_CHILDREN_LIT.data());
        industryChildrensLiterature->SetAttribute(
            XML_VALUE.data(),
            static_cast<int>(customWordTest.has_document_classification(
                readability::document_classification::childrens_literature_document)));
        customFamWordTest->InsertEndChild(industryChildrensLiterature);

        customTestsSection->InsertEndChild(customFamWordTest);
        }
    projectSettings->InsertEndChild(customTestsSection);

    // graph defaults
    auto* graphDefaultsSection = doc.NewElement(XML_GRAPH_SETTINGS.data());
    // color scheme
    auto* graphColorScheme = doc.NewElement(XML_GRAPH_COLOR_SCHEME.data());
    graphColorScheme->SetAttribute(
        XML_VALUE.data(), wxString(ENCODE({ GetGraphColorScheme().wc_str() }, false)).utf8_str());
    graphDefaultsSection->InsertEndChild(graphColorScheme);
    // background image
    auto* graphBackgroundImage = doc.NewElement(XML_GRAPH_PLOT_BACKGROUND_IMAGE_PATH.data());
    graphBackgroundImage->SetAttribute(
        XML_VALUE.data(),
        wxString(ENCODE({ GetPlotBackGroundImagePath().wc_str() }, false)).utf8_str());
    graphDefaultsSection->InsertEndChild(graphBackgroundImage);

    auto* backgroundImageEffect = doc.NewElement(XML_GRAPH_PLOT_BACKGROUND_IMAGE_EFFECT.data());
    backgroundImageEffect->SetAttribute(XML_VALUE.data(),
                                        static_cast<int>(GetPlotBackGroundImageEffect()));
    graphDefaultsSection->InsertEndChild(backgroundImageEffect);

    auto* backgroundImageFit = doc.NewElement(XML_GRAPH_PLOT_BACKGROUND_IMAGE_FIT.data());
    backgroundImageFit->SetAttribute(XML_VALUE.data(),
                                     static_cast<int>(GetPlotBackGroundImageFit()));
    graphDefaultsSection->InsertEndChild(backgroundImageFit);
    // background colors
    if (GetBackGroundColor().IsOk())
        {
        auto* backgroundColor = doc.NewElement(XML_GRAPH_BACKGROUND_COLOR.data());
        backgroundColor->SetAttribute(XmlFormat::RED_TAG.data(), GetBackGroundColor().Red());
        backgroundColor->SetAttribute(XmlFormat::GREEN_TAG.data(), GetBackGroundColor().Green());
        backgroundColor->SetAttribute(XmlFormat::BLUE_TAG.data(), GetBackGroundColor().Blue());
        graphDefaultsSection->InsertEndChild(backgroundColor);
        }
    if (GetPlotBackGroundColor().IsOk())
        {
        auto* plotBackgroundColor = doc.NewElement(XML_GRAPH_PLOT_BACKGROUND_COLOR.data());
        plotBackgroundColor->SetAttribute(XmlFormat::RED_TAG.data(),
                                          GetPlotBackGroundColor().Red());
        plotBackgroundColor->SetAttribute(XmlFormat::GREEN_TAG.data(),
                                          GetPlotBackGroundColor().Green());
        plotBackgroundColor->SetAttribute(XmlFormat::BLUE_TAG.data(),
                                          GetPlotBackGroundColor().Blue());
        graphDefaultsSection->InsertEndChild(plotBackgroundColor);
        }
    // plot background image opacity
    auto* graphBackgroundOpacity = doc.NewElement(XML_GRAPH_PLOT_BACKGROUND_IMAGE_OPACITY.data());
    graphBackgroundOpacity->SetAttribute(XML_VALUE.data(), GetPlotBackGroundImageOpacity());
    graphDefaultsSection->InsertEndChild(graphBackgroundOpacity);
    // plot background color opacity
    auto* graphPlotBackgroundOpacity =
        doc.NewElement(XML_GRAPH_PLOT_BACKGROUND_COLOR_OPACITY.data());
    graphPlotBackgroundOpacity->SetAttribute(XML_VALUE.data(), GetPlotBackGroundColorOpacity());
    graphDefaultsSection->InsertEndChild(graphPlotBackgroundOpacity);
    // background linear gradient
    auto* graphBackgroundGradient = doc.NewElement(XML_GRAPH_BACKGROUND_LINEAR_GRADIENT.data());
    graphBackgroundGradient->SetAttribute(XML_VALUE.data(),
                                          bool_to_int(GetGraphBackGroundLinearGradient()));
    graphDefaultsSection->InsertEndChild(graphBackgroundGradient);
    // stipple image path
    auto* stipplePath = doc.NewElement(XML_GRAPH_STIPPLE_PATH.data());
    stipplePath->SetAttribute(
        XML_VALUE.data(), wxString(ENCODE({ GetStippleImagePath().wc_str() }, false)).utf8_str());
    graphDefaultsSection->InsertEndChild(stipplePath);
    // common image path
    auto* commonImagePath = doc.NewElement(XML_GRAPH_COMMON_IMAGE_PATH.data());
    commonImagePath->SetAttribute(
        XML_VALUE.data(),
        wxString(ENCODE({ GetGraphCommonImagePath().wc_str() }, false)).utf8_str());
    graphDefaultsSection->InsertEndChild(commonImagePath);
    // stipple shape
    auto* stippleShape = doc.NewElement(XML_GRAPH_STIPPLE_SHAPE.data());
    stippleShape->SetAttribute(XML_VALUE.data(),
                               wxString(ENCODE({ GetStippleShape().wc_str() }, false)).utf8_str());
    graphDefaultsSection->InsertEndChild(stippleShape);

    if (GetStippleShapeColor().IsOk())
        {
        auto* stippleShapeColor = doc.NewElement(XML_GRAPH_STIPPLE_COLOR.data());
        stippleShapeColor->SetAttribute(XmlFormat::RED_TAG.data(), GetStippleShapeColor().Red());
        stippleShapeColor->SetAttribute(XmlFormat::GREEN_TAG.data(),
                                        GetStippleShapeColor().Green());
        stippleShapeColor->SetAttribute(XmlFormat::BLUE_TAG.data(), GetStippleShapeColor().Blue());
        graphDefaultsSection->InsertEndChild(stippleShapeColor);
        }

    // whether drop shadows should be shown
    auto* graphDisplayDropShadow = doc.NewElement(XML_DISPLAY_DROP_SHADOW.data());
    graphDisplayDropShadow->SetAttribute(XML_VALUE.data(), bool_to_int(IsDisplayingDropShadows()));
    graphDefaultsSection->InsertEndChild(graphDisplayDropShadow);
    // whether to draw attention to the complex word groups in syllable graphs
    auto* showcaseKeyItems = doc.NewElement(XML_SHOWCASE_KEY_ITEMS.data());
    showcaseKeyItems->SetAttribute(XML_VALUE.data(), bool_to_int(IsShowcasingKeyItems()));
    graphDefaultsSection->InsertEndChild(showcaseKeyItems);
    // watermark
    auto* graphWatermarkLogo = doc.NewElement(XML_GRAPH_WATERMARK_LOGO_IMAGE_PATH.data());
    graphWatermarkLogo->SetAttribute(
        XML_VALUE.data(), wxString(ENCODE({ GetWatermarkLogo().wc_str() }, false)).utf8_str());
    graphDefaultsSection->InsertEndChild(graphWatermarkLogo);

    // histogram settings
    auto* histogramSettings = doc.NewElement(XML_HISTOGRAM_SETTINGS.data());
    // categorization method
    auto* hCatMode = doc.NewElement(XML_GRAPH_BINNING_METHOD.data());
    hCatMode->SetAttribute(XML_VALUE.data(), static_cast<int>(GetHistogramBinningMethod()));
    histogramSettings->InsertEndChild(hCatMode);
    // rounding method
    auto* hRoundMode = doc.NewElement(XML_GRAPH_ROUNDING_METHOD.data());
    hRoundMode->SetAttribute(XML_VALUE.data(), static_cast<int>(GetHistogramRoundingMethod()));
    histogramSettings->InsertEndChild(hRoundMode);
    // interval display
    auto* hIntervalDisplay = doc.NewElement(XML_GRAPH_INTERVAL_DISPLAY.data());
    hIntervalDisplay->SetAttribute(XML_VALUE.data(),
                                   static_cast<int>(GetHistogramIntervalDisplay()));
    histogramSettings->InsertEndChild(hIntervalDisplay);
    // categorization label display
    auto* hCatLabelDisplay = doc.NewElement(XML_GRAPH_BINNING_LABEL_DISPLAY.data());
    hCatLabelDisplay->SetAttribute(XML_VALUE.data(),
                                   static_cast<int>(GetHistogramBinLabelDisplay()));
    histogramSettings->InsertEndChild(hCatLabelDisplay);
    // bar color
    auto* hbarColor = doc.NewElement(XML_GRAPH_COLOR.data());
    hbarColor->SetAttribute(XmlFormat::RED_TAG.data(), GetHistogramBarColor().Red());
    hbarColor->SetAttribute(XmlFormat::GREEN_TAG.data(), GetHistogramBarColor().Green());
    hbarColor->SetAttribute(XmlFormat::BLUE_TAG.data(), GetHistogramBarColor().Blue());
    histogramSettings->InsertEndChild(hbarColor);
    // bar opacity
    auto* hbarOpacity = doc.NewElement(XML_GRAPH_OPACITY.data());
    hbarOpacity->SetAttribute(XML_VALUE.data(), GetHistogramBarOpacity());
    histogramSettings->InsertEndChild(hbarOpacity);
    // bar effect
    auto* hbarEffect = doc.NewElement(XML_BAR_EFFECT.data());
    hbarEffect->SetAttribute(XML_VALUE.data(), static_cast<int>(GetHistogramBarEffect()));
    histogramSettings->InsertEndChild(hbarEffect);

    graphDefaultsSection->InsertEndChild(histogramSettings);

    // bar chart settings
    auto* barChartSettings = doc.NewElement(XML_BAR_CHART_SETTINGS.data());
    // bar color
    auto* barChartbarColor = doc.NewElement(XML_GRAPH_COLOR.data());
    barChartbarColor->SetAttribute(XmlFormat::RED_TAG.data(), GetBarChartBarColor().Red());
    barChartbarColor->SetAttribute(XmlFormat::GREEN_TAG.data(), GetBarChartBarColor().Green());
    barChartbarColor->SetAttribute(XmlFormat::BLUE_TAG.data(), GetBarChartBarColor().Blue());
    barChartSettings->InsertEndChild(barChartbarColor);
    // bar orientation
    auto* barOrientation = doc.NewElement(XML_BAR_ORIENTATION.data());
    barOrientation->SetAttribute(XML_VALUE.data(), static_cast<int>(GetBarChartOrientation()));
    barChartSettings->InsertEndChild(barOrientation);
    // bar opacity
    auto* barOpacity = doc.NewElement(XML_GRAPH_OPACITY.data());
    barOpacity->SetAttribute(XML_VALUE.data(), GetGraphBarOpacity());
    barChartSettings->InsertEndChild(barOpacity);
    // bar display labels
    auto* barDisplayLabels = doc.NewElement(XML_BAR_DISPLAY_LABELS.data());
    barDisplayLabels->SetAttribute(XML_VALUE.data(),
                                   static_cast<int>(IsDisplayingBarChartLabels()));
    barChartSettings->InsertEndChild(barDisplayLabels);
    // bar effect
    auto* barEffect = doc.NewElement(XML_BAR_EFFECT.data());
    barEffect->SetAttribute(XML_VALUE.data(), static_cast<int>(GetGraphBarEffect()));
    barChartSettings->InsertEndChild(barEffect);

    graphDefaultsSection->InsertEndChild(barChartSettings);

    // box plot settings
    auto* boxPlotSettings = doc.NewElement(XML_BOX_PLOT_SETTINGS.data());
    // box color
    auto* boxColor = doc.NewElement(XML_GRAPH_COLOR.data());
    boxColor->SetAttribute(XmlFormat::RED_TAG.data(), GetGraphBoxColor().Red());
    boxColor->SetAttribute(XmlFormat::GREEN_TAG.data(), GetGraphBoxColor().Green());
    boxColor->SetAttribute(XmlFormat::BLUE_TAG.data(), GetGraphBoxColor().Blue());
    boxPlotSettings->InsertEndChild(boxColor);
    // box opacity
    auto* boxOpacity = doc.NewElement(XML_GRAPH_OPACITY.data());
    boxOpacity->SetAttribute(XML_VALUE.data(), GetGraphBoxOpacity());
    boxPlotSettings->InsertEndChild(boxOpacity);
    // box effect
    auto* boxEffect = doc.NewElement(XML_BOX_EFFECT.data());
    boxEffect->SetAttribute(XML_VALUE.data(), static_cast<int>(GetGraphBoxEffect()));
    boxPlotSettings->InsertEndChild(boxEffect);
    // box plot points
    auto* boxPlotAllPoints = doc.NewElement(XML_BOX_PLOT_SHOW_ALL_POINTS.data());
    boxPlotAllPoints->SetAttribute(XML_VALUE.data(), static_cast<int>(IsShowingAllBoxPlotPoints()));
    boxPlotSettings->InsertEndChild(boxPlotAllPoints);
    // box connections
    auto* boxConnectMiddlePoints = doc.NewElement(XML_BOX_CONNECT_MIDDLE_POINTS.data());
    boxConnectMiddlePoints->SetAttribute(XML_VALUE.data(),
                                         static_cast<int>(IsConnectingBoxPlotMiddlePoints()));
    boxPlotSettings->InsertEndChild(boxConnectMiddlePoints);
    // box display labels
    auto* boxDisplayLabels = doc.NewElement(XML_BOX_DISPLAY_LABELS.data());
    boxDisplayLabels->SetAttribute(XML_VALUE.data(), static_cast<int>(IsDisplayingBoxPlotLabels()));
    boxPlotSettings->InsertEndChild(boxDisplayLabels);

    graphDefaultsSection->InsertEndChild(boxPlotSettings);

    // Readability graph settings
    // Lix
    auto* lixSettings = doc.NewElement(XML_LIX_SETTINGS.data());
    auto* useEnglishLabels = doc.NewElement(XML_USE_ENGLISH_LABELS.data());
    useEnglishLabels->SetAttribute(XML_INCLUDE.data(),
                                   bool_to_int(IsUsingEnglishLabelsForGermanLix()));
    lixSettings->InsertEndChild(useEnglishLabels);
    graphDefaultsSection->InsertEndChild(lixSettings);
    // Flesch chart
    auto* fleschChartSettings = doc.NewElement(XML_FLESCH_CHART_SETTINGS.data());
    auto* connectionLine = doc.NewElement(XML_INCLUDE_CONNECTION_LINE.data());
    connectionLine->SetAttribute(XML_INCLUDE.data(), bool_to_int(IsConnectingFleschPoints()));
    fleschChartSettings->InsertEndChild(connectionLine);

    auto* rulerDocGroup = doc.NewElement(XML_FLESCH_RULER_DOC_GROUPS.data());
    rulerDocGroup->SetAttribute(XML_VALUE.data(), bool_to_int(IsIncludingFleschRulerDocGroups()));
    fleschChartSettings->InsertEndChild(rulerDocGroup);

    graphDefaultsSection->InsertEndChild(fleschChartSettings);
    // Fry/Raygor
    auto* fryRaygor = doc.NewElement(XML_FRY_RAYGOR_SETTINGS.data());
    // invalid area colors
    auto* invalidAreaColor = doc.NewElement(XML_INVALID_AREA_COLOR.data());
    invalidAreaColor->SetAttribute(XmlFormat::RED_TAG.data(), GetInvalidAreaColor().Red());
    invalidAreaColor->SetAttribute(XmlFormat::GREEN_TAG.data(), GetInvalidAreaColor().Green());
    invalidAreaColor->SetAttribute(XmlFormat::BLUE_TAG.data(), GetInvalidAreaColor().Blue());
    fryRaygor->InsertEndChild(invalidAreaColor);

    auto* raygorStyle = doc.NewElement(XML_RAYGOR_STYLE.data());
    raygorStyle->SetAttribute(XML_VALUE.data(), static_cast<int>(GetRaygorStyle()));
    fryRaygor->InsertEndChild(raygorStyle);

    graphDefaultsSection->InsertEndChild(fryRaygor);

    // x axis
    auto* axis = doc.NewElement(XML_AXIS_SETTINGS.data());
    auto* xAxis = doc.NewElement(XML_X_AXIS.data());
    // x-axis font color
    auto* xAxisFontColor = doc.NewElement(XML_FONT_COLOR.data());
    xAxisFontColor->SetAttribute(XmlFormat::RED_TAG.data(), GetXAxisFontColor().Red());
    xAxisFontColor->SetAttribute(XmlFormat::GREEN_TAG.data(), GetXAxisFontColor().Green());
    xAxisFontColor->SetAttribute(XmlFormat::BLUE_TAG.data(), GetXAxisFontColor().Blue());
    // x-axis font
    auto* xAxisFont = doc.NewElement(XML_FONT.data());
    xAxisFont->SetAttribute(XmlFormat::FONT_POINT_SIZE_TAG.data(), GetXAxisFont().GetPointSize());
    xAxisFont->SetAttribute(XmlFormat::FONT_STYLE_TAG.data(),
                            static_cast<int>(GetXAxisFont().GetStyle()));
    xAxisFont->SetAttribute(XmlFormat::FONT_WEIGHT_TAG.data(),
                            static_cast<int>(GetXAxisFont().GetWeight()));
    xAxisFont->SetAttribute(XmlFormat::FONT_UNDERLINE_TAG.data(),
                            bool_to_int(GetXAxisFont().GetUnderlined()));
    xAxisFont->SetAttribute(
        XmlFormat::FONT_FACE_NAME_TAG.data(),
        wxString(ENCODE({ GetXAxisFont().GetFaceName().wc_str() }, false)).utf8_str());
    // put it all together
    xAxis->InsertEndChild(xAxisFontColor);
    xAxis->InsertEndChild(xAxisFont);
    axis->InsertEndChild(xAxis);

    // y axis
    auto* yAxis = doc.NewElement(XML_Y_AXIS.data());
    // y-axis font color
    auto* yAxisFontColor = doc.NewElement(XML_FONT_COLOR.data());
    yAxisFontColor->SetAttribute(XmlFormat::RED_TAG.data(), GetYAxisFontColor().Red());
    yAxisFontColor->SetAttribute(XmlFormat::GREEN_TAG.data(), GetYAxisFontColor().Green());
    yAxisFontColor->SetAttribute(XmlFormat::BLUE_TAG.data(), GetYAxisFontColor().Blue());
    // y-axis font
    auto* yAxisFont = doc.NewElement(XML_FONT.data());
    yAxisFont->SetAttribute(XmlFormat::FONT_POINT_SIZE_TAG.data(), GetYAxisFont().GetPointSize());
    yAxisFont->SetAttribute(XmlFormat::FONT_STYLE_TAG.data(),
                            static_cast<int>(GetYAxisFont().GetStyle()));
    yAxisFont->SetAttribute(XmlFormat::FONT_WEIGHT_TAG.data(),
                            static_cast<int>(GetYAxisFont().GetWeight()));
    yAxisFont->SetAttribute(XmlFormat::FONT_UNDERLINE_TAG.data(),
                            bool_to_int(GetYAxisFont().GetUnderlined()));
    yAxisFont->SetAttribute(
        XmlFormat::FONT_FACE_NAME_TAG.data(),
        wxString(ENCODE({ GetYAxisFont().GetFaceName().wc_str() }, false)).utf8_str());
    // put it all together
    yAxis->InsertEndChild(yAxisFontColor);
    yAxis->InsertEndChild(yAxisFont);
    axis->InsertEndChild(yAxis);
    graphDefaultsSection->InsertEndChild(axis);

    // title settings
    auto* titleSettings = doc.NewElement(XML_TITLE_SETTINGS.data());
    auto* topTitle = doc.NewElement(XML_TOP_TITLE.data());
    // top title font color
    auto* topTitleFontColor = doc.NewElement(XML_FONT_COLOR.data());
    topTitleFontColor->SetAttribute(XmlFormat::RED_TAG.data(), GetGraphTopTitleFontColor().Red());
    topTitleFontColor->SetAttribute(XmlFormat::GREEN_TAG.data(),
                                    GetGraphTopTitleFontColor().Green());
    topTitleFontColor->SetAttribute(XmlFormat::BLUE_TAG.data(), GetGraphTopTitleFontColor().Blue());
    // top title font
    auto* topTitleFont = doc.NewElement(XML_FONT.data());
    topTitleFont->SetAttribute(XmlFormat::FONT_POINT_SIZE_TAG.data(),
                               GetGraphTopTitleFont().GetPointSize());
    topTitleFont->SetAttribute(XmlFormat::FONT_STYLE_TAG.data(),
                               static_cast<int>(GetGraphTopTitleFont().GetStyle()));
    topTitleFont->SetAttribute(XmlFormat::FONT_WEIGHT_TAG.data(),
                               static_cast<int>(GetGraphTopTitleFont().GetWeight()));
    topTitleFont->SetAttribute(XmlFormat::FONT_UNDERLINE_TAG.data(),
                               bool_to_int(GetGraphTopTitleFont().GetUnderlined()));
    topTitleFont->SetAttribute(
        XmlFormat::FONT_FACE_NAME_TAG.data(),
        wxString(ENCODE({ GetGraphTopTitleFont().GetFaceName().wc_str() }, false).c_str())
            .utf8_str());
    // put it all together
    topTitle->InsertEndChild(topTitleFontColor);
    topTitle->InsertEndChild(topTitleFont);
    titleSettings->InsertEndChild(topTitle);

    auto* bottomTitle = doc.NewElement(XML_BOTTOM_TITLE.data());
    // bottom title font color
    auto* bottomTitleFontColor = doc.NewElement(XML_FONT_COLOR.data());
    bottomTitleFontColor->SetAttribute(XmlFormat::RED_TAG.data(),
                                       GetGraphBottomTitleFontColor().Red());
    bottomTitleFontColor->SetAttribute(XmlFormat::GREEN_TAG.data(),
                                       GetGraphBottomTitleFontColor().Green());
    bottomTitleFontColor->SetAttribute(XmlFormat::BLUE_TAG.data(),
                                       GetGraphBottomTitleFontColor().Blue());
    // bottom title font
    auto* bottomTitleFont = doc.NewElement(XML_FONT.data());
    bottomTitleFont->SetAttribute(XmlFormat::FONT_POINT_SIZE_TAG.data(),
                                  GetGraphBottomTitleFont().GetPointSize());
    bottomTitleFont->SetAttribute(XmlFormat::FONT_STYLE_TAG.data(),
                                  static_cast<int>(GetGraphBottomTitleFont().GetStyle()));
    bottomTitleFont->SetAttribute(XmlFormat::FONT_WEIGHT_TAG.data(),
                                  static_cast<int>(GetGraphBottomTitleFont().GetWeight()));
    bottomTitleFont->SetAttribute(XmlFormat::FONT_UNDERLINE_TAG.data(),
                                  bool_to_int(GetGraphBottomTitleFont().GetUnderlined()));
    bottomTitleFont->SetAttribute(
        XmlFormat::FONT_FACE_NAME_TAG.data(),
        wxString(ENCODE({ GetGraphBottomTitleFont().GetFaceName().wc_str() }, false).c_str())
            .utf8_str());
    // put it all together
    bottomTitle->InsertEndChild(bottomTitleFontColor);
    bottomTitle->InsertEndChild(bottomTitleFont);
    titleSettings->InsertEndChild(bottomTitle);

    auto* leftTitle = doc.NewElement(XML_LEFT_TITLE.data());
    // left title font color
    auto* leftTitleFontColor = doc.NewElement(XML_FONT_COLOR.data());
    leftTitleFontColor->SetAttribute(XmlFormat::RED_TAG.data(), GetGraphLeftTitleFontColor().Red());
    leftTitleFontColor->SetAttribute(XmlFormat::GREEN_TAG.data(),
                                     GetGraphLeftTitleFontColor().Green());
    leftTitleFontColor->SetAttribute(XmlFormat::BLUE_TAG.data(),
                                     GetGraphLeftTitleFontColor().Blue());
    // left title font
    auto* leftTitleFont = doc.NewElement(XML_FONT.data());
    leftTitleFont->SetAttribute(XmlFormat::FONT_POINT_SIZE_TAG.data(),
                                GetGraphLeftTitleFont().GetPointSize());
    leftTitleFont->SetAttribute(XmlFormat::FONT_STYLE_TAG.data(),
                                static_cast<int>(GetGraphLeftTitleFont().GetStyle()));
    leftTitleFont->SetAttribute(XmlFormat::FONT_WEIGHT_TAG.data(),
                                static_cast<int>(GetGraphLeftTitleFont().GetWeight()));
    leftTitleFont->SetAttribute(XmlFormat::FONT_UNDERLINE_TAG.data(),
                                bool_to_int(GetGraphLeftTitleFont().GetUnderlined()));
    leftTitleFont->SetAttribute(
        XmlFormat::FONT_FACE_NAME_TAG.data(),
        wxString(ENCODE({ GetGraphLeftTitleFont().GetFaceName().wc_str() }, false).c_str())
            .utf8_str());
    // put it all together
    leftTitle->InsertEndChild(leftTitleFontColor);
    leftTitle->InsertEndChild(leftTitleFont);
    titleSettings->InsertEndChild(leftTitle);

    auto* rightTitle = doc.NewElement(XML_RIGHT_TITLE.data());
    // right title font color
    auto* rightTitleFontColor = doc.NewElement(XML_FONT_COLOR.data());
    rightTitleFontColor->SetAttribute(XmlFormat::RED_TAG.data(),
                                      GetGraphRightTitleFontColor().Red());
    rightTitleFontColor->SetAttribute(XmlFormat::GREEN_TAG.data(),
                                      GetGraphRightTitleFontColor().Green());
    rightTitleFontColor->SetAttribute(XmlFormat::BLUE_TAG.data(),
                                      GetGraphRightTitleFontColor().Blue());
    // right title font
    auto* rightTitleFont = doc.NewElement(XML_FONT.data());
    rightTitleFont->SetAttribute(XmlFormat::FONT_POINT_SIZE_TAG.data(),
                                 GetGraphRightTitleFont().GetPointSize());
    rightTitleFont->SetAttribute(XmlFormat::FONT_STYLE_TAG.data(),
                                 static_cast<int>(GetGraphRightTitleFont().GetStyle()));
    rightTitleFont->SetAttribute(XmlFormat::FONT_WEIGHT_TAG.data(),
                                 static_cast<int>(GetGraphRightTitleFont().GetWeight()));
    rightTitleFont->SetAttribute(XmlFormat::FONT_UNDERLINE_TAG.data(),
                                 bool_to_int(GetGraphRightTitleFont().GetUnderlined()));
    rightTitleFont->SetAttribute(
        XmlFormat::FONT_FACE_NAME_TAG.data(),
        wxString(ENCODE({ GetGraphRightTitleFont().GetFaceName().wc_str() }, false).c_str())
            .utf8_str());
    // put it all together
    rightTitle->InsertEndChild(rightTitleFontColor);
    rightTitle->InsertEndChild(rightTitleFont);
    titleSettings->InsertEndChild(rightTitle);
    graphDefaultsSection->InsertEndChild(titleSettings);

    projectSettings->InsertEndChild(graphDefaultsSection);

    // stats defaults
    auto* statsDefaultsSection = doc.NewElement(XML_STATISTICS_SECTION.data());
    // variance method
    auto* varianceMethod = doc.NewElement(XML_VARIANCE_METHOD.data());
    varianceMethod->SetAttribute(XML_METHOD.data(), static_cast<int>(GetVarianceMethod()));
    statsDefaultsSection->InsertEndChild(varianceMethod);
    // stats results
    auto* statsResults = doc.NewElement(XML_STATISTICS_RESULTS.data());
    statsResults->SetAttribute(XML_VALUE.data(), GetStatisticsInfo().ToString().utf8_str());
    statsDefaultsSection->InsertEndChild(statsResults);
    // stats report
    auto* statsReport = doc.NewElement(XML_STATISTICS_REPORT.data());
    statsReport->SetAttribute(XML_VALUE.data(), GetStatisticsReportInfo().ToString().utf8_str());
    statsDefaultsSection->InsertEndChild(statsReport);
    projectSettings->InsertEndChild(statsDefaultsSection);

    // wizard page defaults
    auto* wizardPageDefaultsSection = doc.NewElement(XML_WIZARD_PAGES_SETTINGS.data());

    auto* batchGrouping = doc.NewElement(XML_WIZARD_BATCH_GROUP.data());
    batchGrouping->SetAttribute(XML_METHOD.data(), m_batchGroupDefault);
    wizardPageDefaultsSection->InsertEndChild(batchGrouping);

    // Text Source
    auto* textSource = doc.NewElement(XML_TEXT_SOURCE.data());
    textSource->SetAttribute(XML_METHOD.data(), static_cast<int>(m_textSource));
    wizardPageDefaultsSection->InsertEndChild(textSource);

    // Test Recommendation
    auto* testRecommendation = doc.NewElement(XML_TEST_RECOMMENDATION.data());
    testRecommendation->SetAttribute(XML_METHOD.data(), static_cast<int>(m_testRecommendation));
    wizardPageDefaultsSection->InsertEndChild(testRecommendation);

    // Tests by Industry
    auto* testsByIndustry = doc.NewElement(XML_TEST_BY_INDUSTRY.data());
    testsByIndustry->SetAttribute(XML_METHOD.data(), static_cast<int>(m_testsByIndustry));
    wizardPageDefaultsSection->InsertEndChild(testsByIndustry);

    // Tests By Document Type
    auto* testsByDocumentType = doc.NewElement(XML_TEST_BY_DOCUMENT_TYPE.data());
    testsByDocumentType->SetAttribute(XML_METHOD.data(), static_cast<int>(m_testsByDocumentType));
    wizardPageDefaultsSection->InsertEndChild(testsByDocumentType);

    // tests by bundle
    auto* testsByBundle = doc.NewElement(XML_SELECTED_TEST_BUNDLE.data());
    testsByBundle->SetAttribute(XML_VALUE.data(), GetSelectedTestBundle().utf8_str());
    wizardPageDefaultsSection->InsertEndChild(testsByBundle);

    projectSettings->InsertEndChild(wizardPageDefaultsSection);

    // readability tests information
    auto* readabilityTestSection = doc.NewElement(XML_READABILITY_TESTS_SECTION.data());

    // options inclusions
    auto* isIncludingScoreSummary = doc.NewElement(XML_INCLUDE_SCORES_SUMMARY_REPORT.data());
    isIncludingScoreSummary->SetAttribute(XML_VALUE.data(),
                                          bool_to_int(IsIncludingScoreSummaryReport()));
    readabilityTestSection->InsertEndChild(isIncludingScoreSummary);

    // grade scale/reading age display
    auto* readingAgeFormat = doc.NewElement(XML_READING_AGE_FORMAT.data());
    readingAgeFormat->SetAttribute(
        XML_VALUE.data(), static_cast<int>(GetReadabilityMessageCatalog().GetReadingAgeDisplay()));
    readabilityTestSection->InsertEndChild(readingAgeFormat);

    auto* gradeScale = doc.NewElement(XML_READABILITY_TEST_GRADE_SCALE_DISPLAY.data());
    gradeScale->SetAttribute(XML_VALUE.data(),
                             static_cast<int>(GetReadabilityMessageCatalog().GetGradeScale()));
    readabilityTestSection->InsertEndChild(gradeScale);

    auto* gradeScaleLongFormat =
        doc.NewElement(XML_READABILITY_TEST_GRADE_SCALE_LONG_FORMAT.data());
    gradeScaleLongFormat->SetAttribute(
        XML_VALUE.data(),
        bool_to_int(GetReadabilityMessageCatalog().IsUsingLongGradeScaleFormat()));
    readabilityTestSection->InsertEndChild(gradeScaleLongFormat);

    // test-specific options
    //----------------------
    // Flesch-Kincaid options
    auto* fleschKincaidOptions = doc.NewElement(XML_FLESCH_KINCAID_OPTIONS.data());
    auto* fleschKincaidNumeralMethod = doc.NewElement(XML_NUMERAL_SYLLABICATION_METHOD.data());
    fleschKincaidNumeralMethod->SetAttribute(
        XML_VALUE.data(), static_cast<int>(GetFleschKincaidNumeralSyllabizeMethod()));
    fleschKincaidOptions->InsertEndChild(fleschKincaidNumeralMethod);
    readabilityTestSection->InsertEndChild(fleschKincaidOptions);
    // Flesch options
    auto* fleschOptions = doc.NewElement(XML_FLESCH_OPTIONS.data());
    auto* fleschNumeralMethod = doc.NewElement(XML_NUMERAL_SYLLABICATION_METHOD.data());
    fleschNumeralMethod->SetAttribute(XML_VALUE.data(),
                                      static_cast<int>(GetFleschNumeralSyllabizeMethod()));
    fleschOptions->InsertEndChild(fleschNumeralMethod);
    readabilityTestSection->InsertEndChild(fleschOptions);
    // Gunning fog options
    auto* fogOptions = doc.NewElement(XML_GUNNING_FOG_OPTIONS.data());
    auto* useUnits = doc.NewElement(XML_USE_SENTENCE_UNITS.data());
    useUnits->SetAttribute(XML_VALUE.data(), bool_to_int(IsFogUsingSentenceUnits()));
    fogOptions->InsertEndChild(useUnits);
    readabilityTestSection->InsertEndChild(fogOptions);
    // HJ options
    auto* hjOptions = doc.NewElement(XML_HARRIS_JACOBSON_OPTIONS.data());
    // HJ text exclusion
    auto* hjTextExclusionMode = doc.NewElement(XML_TEXT_EXCLUSION.data());
    hjTextExclusionMode->SetAttribute(XML_VALUE.data(),
                                      static_cast<int>(GetHarrisJacobsonTextExclusionMode()));
    hjOptions->InsertEndChild(hjTextExclusionMode);
    readabilityTestSection->InsertEndChild(hjOptions);
    // DC options
    auto* dcOptions = doc.NewElement(XML_NEW_DALE_CHALL_OPTIONS.data());
    // stocker list
    auto* stockerList = doc.NewElement(XML_STOCKER_LIST.data());
    stockerList->SetAttribute(XML_VALUE.data(),
                              bool_to_int(IsIncludingStockerCatholicSupplement()));
    dcOptions->InsertEndChild(stockerList);
    // dc text exclusion
    auto* dcTextExclusionMode = doc.NewElement(XML_TEXT_EXCLUSION.data());
    dcTextExclusionMode->SetAttribute(XML_VALUE.data(),
                                      static_cast<int>(GetDaleChallTextExclusionMode()));
    dcOptions->InsertEndChild(dcTextExclusionMode);
    // proper nouns
    auto* dcProperNouns = doc.NewElement(XML_PROPER_NOUN_COUNTING_METHOD.data());
    dcProperNouns->SetAttribute(XML_VALUE.data(),
                                static_cast<int>(GetDaleChallProperNounCountingMethod()));
    dcOptions->InsertEndChild(dcProperNouns);
    readabilityTestSection->InsertEndChild(dcOptions);

    for (auto& rTest : GetReadabilityTests().get_tests())
        {
        auto* currentTest = doc.NewElement(wxString(rTest.get_test().get_id().c_str()).utf8_str());
        currentTest->SetAttribute(XML_INCLUDE.data(), bool_to_int(rTest.is_included()));
        readabilityTestSection->InsertEndChild(currentTest);
        }

    // Dolch
    auto* dolch = doc.NewElement(XML_DOLCH_SUITE.data());
    dolch->SetAttribute(XML_INCLUDE.data(), bool_to_int(IsDolchSelected()));
    readabilityTestSection->InsertEndChild(dolch);
    // all the custom tests being included
    for (const auto& includedCustomTest : GetIncludedCustomTests())
        {
        auto* customTest = doc.NewElement(XML_CUSTOM_TEST.data());
        customTest->SetAttribute(
            XML_VALUE.data(), wxString(ENCODE({ includedCustomTest.wc_str() }, false)).utf8_str());
        readabilityTestSection->InsertEndChild(customTest);
        }
    projectSettings->InsertEndChild(readabilityTestSection);

    // text views
    auto* textViewsSection = doc.NewElement(XML_TEXT_VIEWS_SECTION.data());
    // how highlighting is done
    auto* highlightMethod = doc.NewElement(XML_HIGHLIGHT_METHOD.data());
    highlightMethod->SetAttribute(XML_METHOD.data(), static_cast<int>(m_textHighlight));
    textViewsSection->InsertEndChild(highlightMethod);

    // highlighting information
    auto* highlight = doc.NewElement(XML_HIGHLIGHTCOLOR.data());
    highlight->SetAttribute(XmlFormat::RED_TAG.data(), m_textHighlightColor.Red());
    highlight->SetAttribute(XmlFormat::GREEN_TAG.data(), m_textHighlightColor.Green());
    highlight->SetAttribute(XmlFormat::BLUE_TAG.data(), m_textHighlightColor.Blue());
    textViewsSection->InsertEndChild(highlight);

    // highlight color for wordy items
    auto* wordyPhrasesHighlight = doc.NewElement(XML_WORDY_PHRASE_HIGHLIGHTCOLOR.data());
    wordyPhrasesHighlight->SetAttribute(XmlFormat::RED_TAG.data(),
                                        m_wordyPhraseHighlightColor.Red());
    wordyPhrasesHighlight->SetAttribute(XmlFormat::GREEN_TAG.data(),
                                        m_wordyPhraseHighlightColor.Green());
    wordyPhrasesHighlight->SetAttribute(XmlFormat::BLUE_TAG.data(),
                                        m_wordyPhraseHighlightColor.Blue());
    textViewsSection->InsertEndChild(wordyPhrasesHighlight);

    // highlight color for repeated words
    auto* dupWordsHighlight = doc.NewElement(XML_DUP_WORD_HIGHLIGHTCOLOR.data());
    dupWordsHighlight->SetAttribute(XmlFormat::RED_TAG.data(), m_duplicateWordHighlightColor.Red());
    dupWordsHighlight->SetAttribute(XmlFormat::GREEN_TAG.data(),
                                    m_duplicateWordHighlightColor.Green());
    dupWordsHighlight->SetAttribute(XmlFormat::BLUE_TAG.data(),
                                    m_duplicateWordHighlightColor.Blue());
    textViewsSection->InsertEndChild(dupWordsHighlight);

    // highlight color for dolch words
    auto* dolchConjunctionsHighlight = doc.NewElement(XML_DOLCH_CONJUNCTIONS_HIGHLIGHTCOLOR.data());
    dolchConjunctionsHighlight->SetAttribute(XmlFormat::RED_TAG.data(),
                                             GetDolchConjunctionsColor().Red());
    dolchConjunctionsHighlight->SetAttribute(XmlFormat::GREEN_TAG.data(),
                                             GetDolchConjunctionsColor().Green());
    dolchConjunctionsHighlight->SetAttribute(XmlFormat::BLUE_TAG.data(),
                                             GetDolchConjunctionsColor().Blue());
    dolchConjunctionsHighlight->SetAttribute(XML_INCLUDE.data(),
                                             bool_to_int(IsHighlightingDolchConjunctions()));
    textViewsSection->InsertEndChild(dolchConjunctionsHighlight);

    auto* dolchPrepositionsHighlight = doc.NewElement(XML_DOLCH_PREPOSITIONS_HIGHLIGHTCOLOR.data());
    dolchPrepositionsHighlight->SetAttribute(XmlFormat::RED_TAG.data(),
                                             GetDolchPrepositionsColor().Red());
    dolchPrepositionsHighlight->SetAttribute(XmlFormat::GREEN_TAG.data(),
                                             GetDolchPrepositionsColor().Green());
    dolchPrepositionsHighlight->SetAttribute(XmlFormat::BLUE_TAG.data(),
                                             GetDolchPrepositionsColor().Blue());
    dolchPrepositionsHighlight->SetAttribute(XML_INCLUDE.data(),
                                             bool_to_int(IsHighlightingDolchPrepositions()));
    textViewsSection->InsertEndChild(dolchPrepositionsHighlight);

    auto* dolchPronounHighlight = doc.NewElement(XML_DOLCH_PRONOUNS_HIGHLIGHTCOLOR.data());
    dolchPronounHighlight->SetAttribute(XmlFormat::RED_TAG.data(), GetDolchPronounsColor().Red());
    dolchPronounHighlight->SetAttribute(XmlFormat::GREEN_TAG.data(),
                                        GetDolchPronounsColor().Green());
    dolchPronounHighlight->SetAttribute(XmlFormat::BLUE_TAG.data(), GetDolchPronounsColor().Blue());
    dolchPronounHighlight->SetAttribute(XML_INCLUDE.data(),
                                        bool_to_int(IsHighlightingDolchPronouns()));
    textViewsSection->InsertEndChild(dolchPronounHighlight);

    auto* dolchAdverbHighlight = doc.NewElement(XML_DOLCH_ADVERBS_HIGHLIGHTCOLOR.data());
    dolchAdverbHighlight->SetAttribute(XmlFormat::RED_TAG.data(), GetDolchAdverbsColor().Red());
    dolchAdverbHighlight->SetAttribute(XmlFormat::GREEN_TAG.data(), GetDolchAdverbsColor().Green());
    dolchAdverbHighlight->SetAttribute(XmlFormat::BLUE_TAG.data(), GetDolchAdverbsColor().Blue());
    dolchAdverbHighlight->SetAttribute(XML_INCLUDE.data(),
                                       bool_to_int(IsHighlightingDolchAdverbs()));
    textViewsSection->InsertEndChild(dolchAdverbHighlight);

    auto* dolchAdjectiveHighlight = doc.NewElement(XML_DOLCH_ADJECTIVES_HIGHLIGHTCOLOR.data());
    dolchAdjectiveHighlight->SetAttribute(XmlFormat::RED_TAG.data(),
                                          GetDolchAdjectivesColor().Red());
    dolchAdjectiveHighlight->SetAttribute(XmlFormat::GREEN_TAG.data(),
                                          GetDolchAdjectivesColor().Green());
    dolchAdjectiveHighlight->SetAttribute(XmlFormat::BLUE_TAG.data(),
                                          GetDolchAdjectivesColor().Blue());
    dolchAdjectiveHighlight->SetAttribute(XML_INCLUDE.data(),
                                          bool_to_int(IsHighlightingDolchAdjectives()));
    textViewsSection->InsertEndChild(dolchAdjectiveHighlight);

    auto* dolchVerbHighlight = doc.NewElement(XML_DOLCH_VERBS_HIGHLIGHTCOLOR.data());
    dolchVerbHighlight->SetAttribute(XmlFormat::RED_TAG.data(), GetDolchVerbsColor().Red());
    dolchVerbHighlight->SetAttribute(XmlFormat::GREEN_TAG.data(), GetDolchVerbsColor().Green());
    dolchVerbHighlight->SetAttribute(XmlFormat::BLUE_TAG.data(), GetDolchVerbsColor().Blue());
    dolchVerbHighlight->SetAttribute(XML_INCLUDE.data(), bool_to_int(IsHighlightingDolchVerbs()));
    textViewsSection->InsertEndChild(dolchVerbHighlight);

    auto* dolchNounHighlight = doc.NewElement(XML_DOLCH_NOUNS_HIGHLIGHTCOLOR.data());
    dolchNounHighlight->SetAttribute(XmlFormat::RED_TAG.data(), GetDolchNounsColor().Red());
    dolchNounHighlight->SetAttribute(XmlFormat::GREEN_TAG.data(), GetDolchNounsColor().Green());
    dolchNounHighlight->SetAttribute(XmlFormat::BLUE_TAG.data(), GetDolchNounsColor().Blue());
    dolchNounHighlight->SetAttribute(XML_INCLUDE.data(), bool_to_int(IsHighlightingDolchNouns()));
    textViewsSection->InsertEndChild(dolchNounHighlight);

    // highlight color for excluded text
    auto* excludedHighlight = doc.NewElement(XML_EXCLUDED_HIGHLIGHTCOLOR.data());
    excludedHighlight->SetAttribute(XmlFormat::RED_TAG.data(), m_excludedTextHighlightColor.Red());
    excludedHighlight->SetAttribute(XmlFormat::GREEN_TAG.data(),
                                    m_excludedTextHighlightColor.Green());
    excludedHighlight->SetAttribute(XmlFormat::BLUE_TAG.data(),
                                    m_excludedTextHighlightColor.Blue());
    textViewsSection->InsertEndChild(excludedHighlight);

    // document display font information
    auto* fontcolor = doc.NewElement(XML_DOCUMENT_DISPLAY_FONTCOLOR.data());
    fontcolor->SetAttribute(XmlFormat::RED_TAG.data(), m_fontColor.Red());
    fontcolor->SetAttribute(XmlFormat::GREEN_TAG.data(), m_fontColor.Green());
    fontcolor->SetAttribute(XmlFormat::BLUE_TAG.data(), m_fontColor.Blue());
    textViewsSection->InsertEndChild(fontcolor);

    auto* font = doc.NewElement(XML_DOCUMENT_DISPLAY_FONT.data());
    font->SetAttribute(XmlFormat::FONT_POINT_SIZE_TAG.data(), m_textViewFont.GetPointSize());
    font->SetAttribute(XmlFormat::FONT_STYLE_TAG.data(),
                       static_cast<int>(m_textViewFont.GetStyle()));
    font->SetAttribute(XmlFormat::FONT_WEIGHT_TAG.data(),
                       static_cast<int>(m_textViewFont.GetWeight()));
    font->SetAttribute(XmlFormat::FONT_UNDERLINE_TAG.data(),
                       bool_to_int(m_textViewFont.GetUnderlined()));
    font->SetAttribute(
        XmlFormat::FONT_FACE_NAME_TAG.data(),
        wxString(ENCODE({ m_textViewFont.GetFaceName().wc_str() }, false)).utf8_str());
    textViewsSection->InsertEndChild(font);

    projectSettings->InsertEndChild(textViewsSection);

    configSection->InsertEndChild(projectSettings);

    root->InsertEndChild(configSection);

    doc.InsertEndChild(root);

    // serializes whole doc into UTF-8 memory buffer
    tinyxml2::XMLPrinter xmlPrinter;
    doc.Print(&xmlPrinter);

    wxFile file(optionsFile.empty() ? m_optionsFile : optionsFile, wxFile::write);
    if (file.IsOpened() && file.Write(wxString::FromUTF8(xmlPrinter.CStr())))
        {
        file.Close();
        return true;
        }

    wxMessageBox(wxString::Format(_(L"Unable to save configuration file: %s"), doc.ErrorStr()),
                 _(L"Error"), wxOK | wxICON_ERROR);
    return false;
    }

//--------------------------------------------
wxColour
ReadabilityAppOptions::TiXmlNodeToColor(const tinyxml2::XMLNode* colorNode,
                                        const wxColour& defaultColor /*= wxColour{ 0, 0, 0 }*/)
    {
    if (colorNode != nullptr)
        {
        const auto red = static_cast<wxColour::ChannelType>(
            colorNode->ToElement()->IntAttribute(XmlFormat::RED_TAG.data(), 255));
        const auto green = static_cast<wxColour::ChannelType>(
            colorNode->ToElement()->IntAttribute(XmlFormat::GREEN_TAG.data(), 255));
        const auto blue = static_cast<wxColour::ChannelType>(
            colorNode->ToElement()->IntAttribute(XmlFormat::BLUE_TAG.data(), 255));
        return { red, green, blue };
        }
    return defaultColor;
    }

//--------------------------------------------
double ReadabilityAppOptions::TiXmlNodeToDouble(const tinyxml2::XMLNode* node,
                                                const wxString& tagToRead)
    {
    if (node == nullptr)
        {
        return std::numeric_limits<double>::quiet_NaN();
        }

    lily_of_the_valley::html_extract_text filterHtml;

    const char* stringData = node->ToElement()->Attribute(tagToRead.utf8_str());
    if (stringData == nullptr)
        {
        return std::numeric_limits<double>::quiet_NaN();
        }
    auto convertedStr =
        Wisteria::TextStream::CharStreamToUnicode(stringData, std::strlen(stringData));
    const wchar_t* filteredText =
        filterHtml(convertedStr.c_str(), convertedStr.length(), true, false);
    if (filteredText == nullptr)
        {
        return std::numeric_limits<double>::quiet_NaN();
        }
    const wxString dValueStr(filteredText);
    double dValue{ 0 };
    if (dValueStr.ToCDouble(&dValue))
        {
        return dValue;
        }

    return std::numeric_limits<double>::quiet_NaN();
    }

//--------------------------------------------
wxString
ReadabilityAppOptions::TiXmlNodeAttributeToString(const tinyxml2::XMLNode* node,
                                                  const wxString& tagToRead,
                                                  const wxString& fallbackValue /*= wxString{}*/)
    {
    if (node == nullptr)
        {
        return fallbackValue;
        }

    lily_of_the_valley::html_extract_text filterHtml;

    const char* stringData = node->ToElement()->Attribute(tagToRead.utf8_str());
    if (stringData == nullptr)
        {
        return fallbackValue;
        }
    auto convertedStr =
        Wisteria::TextStream::CharStreamToUnicode(stringData, std::strlen(stringData));
    const wchar_t* filteredText =
        filterHtml(convertedStr.c_str(), convertedStr.length(), true, false);
    if (filteredText == nullptr)
        {
        return fallbackValue;
        }
    return { filteredText };
    }

//------------------------------------------------
wxString ReadabilityAppOptions::GetDocumentFilter()
    {
    const wxString textFilter = _DT(L"*.txt");
    const wxString htmlFiles = _DT(L"*.htm;*.html;*.xhtml;*.sgml;*.php;*.php3;*.php4;*.aspx;*.asp");
    const wxString wordFilter = _DT(L"*.doc;*.docx;*.docm;*.dot;*.wri");
    const wxString powerPointFilter = _DT(L"*.pptx;*.pptm");
    const wxString openDocPrezFilter = _DT(L"*.odp;*.otp");
    const wxString openDocFilter = _DT(L"*.odt;*.ott");
    const wxString rtfFilter = _DT(L"*.rtf");
    const wxString markdownFilter = _DT(L"*.md;*.qmd;*.rmd");
    const wxString psFilter = _DT(L"*.ps");
    const wxString idlFilter = _DT(L"*.idl");
    const wxString cppFilter = _DT(L"*.cpp;*.c;*.h");
    const wxString allDocumentsFilter = wxString::Format(
        // TRANSLATORS: %s are file filters
        _(L"Documents (%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s)|%s;%s;%s;%s;%s;%s;%s;%s;%s;%s;%s"),
        textFilter, htmlFiles, wordFilter, powerPointFilter, openDocPrezFilter, openDocFilter,
        rtfFilter, markdownFilter, psFilter, idlFilter, cppFilter, textFilter, htmlFiles,
        wordFilter, powerPointFilter, openDocPrezFilter, openDocFilter, rtfFilter, markdownFilter,
        psFilter, idlFilter, cppFilter);
    return wxString::Format(
        // TRANSLATORS: %s are file filters
        _(L"%s|Text files (%s)|%s|HTML files (%s)|%s|Word files (%s)|%s|"
          "PowerPoint files (%s)|%s|OpenDocument Presentation files (%s)|%s|"
          "OpenDocument files (%s)|%s|Rich Text files (%s)|%s|"
          "Markdown files (%s)|%s|Postscript files (%s)|%s|"
          "Interface Definition Language files (%s)|%s|C++ source files (%s)|%s"),
        allDocumentsFilter, textFilter, textFilter, htmlFiles, htmlFiles, wordFilter, wordFilter,
        powerPointFilter, powerPointFilter, openDocPrezFilter, openDocPrezFilter, openDocFilter,
        openDocFilter, rtfFilter, rtfFilter, markdownFilter, markdownFilter, psFilter, psFilter,
        idlFilter, idlFilter, cppFilter, cppFilter);
    }

//------------------------------------------------
void ReadabilityAppOptions::UpdateGraphOptions(Wisteria::Canvas* graphCanvas)
    {
    if (graphCanvas == nullptr)
        {
        return;
        }
    // load the graph images
    if (wxFile::Exists(GetPlotBackGroundImagePath()))
        {
        m_graphBackgroundImage =
            wxBitmapBundle(wxGetApp()
                               .GetResourceManager()
                               .GetBitmap(GetPlotBackGroundImagePath(), wxBITMAP_TYPE_ANY)
                               .ConvertToImage());
        }
    if (wxFile::Exists(GetWatermarkLogo()))
        {
        m_waterMarkImage = wxBitmapBundle(wxGetApp()
                                              .GetResourceManager()
                                              .GetBitmap(GetWatermarkLogo(), wxBITMAP_TYPE_ANY)
                                              .ConvertToImage());
        }
    if (wxFile::Exists(GetStippleImagePath()))
        {
        m_graphStippleImage =
            wxBitmapBundle(wxGetApp()
                               .GetResourceManager()
                               .GetBitmap(GetStippleImagePath(), wxBITMAP_TYPE_ANY)
                               .ConvertToImage());
        }

    graphCanvas->SetExportResources(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                                    L"online/publishing.html");
    graphCanvas->SetPrinterSettings(*wxGetApp().GetPrintData());
    graphCanvas->SetLeftPrinterHeader(ReadabilityAppOptions::GetLeftPrinterHeader());
    graphCanvas->SetCenterPrinterHeader(ReadabilityAppOptions::GetCenterPrinterHeader());
    graphCanvas->SetRightPrinterHeader(ReadabilityAppOptions::GetRightPrinterHeader());
    graphCanvas->SetLeftPrinterFooter(ReadabilityAppOptions::GetLeftPrinterFooter());
    graphCanvas->SetCenterPrinterFooter(ReadabilityAppOptions::GetCenterPrinterFooter());
    graphCanvas->SetRightPrinterFooter(ReadabilityAppOptions::GetRightPrinterFooter());

    graphCanvas->SetBackgroundColor(GetBackGroundColor(), GetGraphBackGroundLinearGradient());
    graphCanvas->SetBackgroundImage(m_graphBackgroundImage, GetPlotBackGroundImageOpacity());

    auto plot =
        std::dynamic_pointer_cast<Wisteria::Graphs::Graph2D>(graphCanvas->GetFixedObject(0, 0));
    plot->SetPlotBackgroundColor(Wisteria::Colors::ColorContrast::ChangeOpacity(
        GetPlotBackGroundColor(), GetPlotBackGroundColorOpacity()));

    plot->SetStippleBrush(m_graphStippleImage);
    plot->SetImageScheme(m_graphImageScheme);
    plot->GetBottomXAxis().SetFont(GetXAxisFont());
    plot->GetBottomXAxis().SetFontColor(GetXAxisFontColor());
    plot->GetLeftYAxis().SetFont(GetYAxisFont());
    plot->GetLeftYAxis().SetFontColor(GetYAxisFontColor());
    for (auto& customAxis : plot->GetCustomAxes())
        {
        customAxis.SetFont(GetYAxisFont());
        customAxis.SetFontColor(GetYAxisFontColor());
        }
    for (auto& topTitle : graphCanvas->GetTopTitles())
        {
        topTitle.GetFont() = GetGraphTopTitleFont();
        topTitle.SetFontColor(GetGraphTopTitleFontColor());
        }
    for (auto& bottomTitle : graphCanvas->GetBottomTitles())
        {
        bottomTitle.GetFont() = GetGraphBottomTitleFont();
        bottomTitle.SetFontColor(GetGraphBottomTitleFontColor());
        }
    for (auto& leftTitle : graphCanvas->GetLeftTitles())
        {
        leftTitle.GetFont() = GetGraphLeftTitleFont();
        leftTitle.SetFontColor(GetGraphLeftTitleFontColor());
        }
    for (auto& rightTitle : graphCanvas->GetRightTitles())
        {
        rightTitle.GetFont() = GetGraphRightTitleFont();
        rightTitle.SetFontColor(GetGraphRightTitleFontColor());
        }
    }
