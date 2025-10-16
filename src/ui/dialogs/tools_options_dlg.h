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

#ifndef TOOLS_OPTIONS_DIALOG_H
#define TOOLS_OPTIONS_DIALOG_H

#include "../../Wisteria-Dataviz/src/base/image.h"
#include "../../Wisteria-Dataviz/src/ui/controls/sidebarbook.h"
#include "../../Wisteria-Dataviz/src/util/backupvariable.h"
#include "../../Wisteria-Dataviz/src/util/warningmanager.h"
#include "../../projects/batch_project_view.h"
#include "../../projects/standard_project_view.h"
#include <limits>
#include <map>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/propgrid.h>
#include <wx/treebook.h>

class BaseProjectDoc;
class BatchProjectDoc;
class ProjectDoc;
class ReadabilityApp;

class ToolsOptionsDlg final : public wxDialog
    {
    friend ReadabilityApp;

  public:
    enum ToolSections
        {
        ProjectSection = 0x0010,
        GraphsSection = 0x0020,
        TextSection = 0x0040,
        ScoresSection = 0x0080,
        DocumentIndexing = 0x0100,
        Grammar = 0x0200,
        Statistics = 0x0400,
        WordsBreakdown = 0x0800,
        SentencesBreakdown = 0x1000,
        UNUSED1 = 0x2000,
        UNUSED2 = 0x4000,
        AllSections = (ProjectSection | GraphsSection | ScoresSection | TextSection |
        DocumentIndexing | Grammar | Statistics | WordsBreakdown | SentencesBreakdown)
        };

    /// Constructor.
    /// @param parent The parent of this dialog.
    /// @param project The project to connect this dialog to. If null, then this is
    ///     treating as a global settings dialog.
    /// @param sectionsToInclude The sections of options to include. This useful
    ///     for only showing a single section (e.g., graphs), thus making this
    ///     a simplified options' editor.
    explicit ToolsOptionsDlg(wxWindow* parent, BaseProjectDoc* project = nullptr,
                             const ToolSections sectionsToInclude = AllSections);

    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& caption = _(L"Options"),
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

    /// @brief Selects the default page when opening the dialog.
    /// @param pageId The ID of the page to select.
    void SelectPage(const int pageId);

    // should be public so that clients can specify which page to select
    constexpr static int GENERAL_SETTINGS_PAGE = wxID_HIGHEST;
    constexpr static int PROJECT_SETTINGS_PAGE = wxID_HIGHEST + 1;

    constexpr static int DOCUMENT_DISPLAY_GENERAL_PAGE = wxID_HIGHEST + 2;
    constexpr static int DOCUMENT_DISPLAY_DOLCH_PAGE = wxID_HIGHEST + 3;

    constexpr static int SCORES_TEST_OPTIONS_PAGE = wxID_HIGHEST + 4;
    constexpr static int SCORES_DISPLAY_PAGE = wxID_HIGHEST + 5;

    constexpr static int ANALYSIS_INDEXING_PAGE = wxID_HIGHEST + 6;
    constexpr static int GRAMMAR_PAGE = wxID_HIGHEST + 7;
    constexpr static int ANALYSIS_STATISTICS_PAGE = wxID_HIGHEST + 8;
    constexpr static int WORDS_BREAKDOWN_PAGE = wxID_HIGHEST + 9;
    constexpr static int SENTENCES_BREAKDOWN_PAGE = wxID_HIGHEST + 10;

    constexpr static int GRAPH_GENERAL_PAGE = wxID_HIGHEST + 11;
    constexpr static int GRAPH_AXIS_PAGE = wxID_HIGHEST + 12;
    constexpr static int GRAPH_TITLES_PAGE = wxID_HIGHEST + 13;
    constexpr static int GRAPH_READABILITY_GRAPHS_PAGE = wxID_HIGHEST + 14;
    constexpr static int GRAPH_BAR_CHART_PAGE = wxID_HIGHEST + 15;
    constexpr static int GRAPH_HISTOGRAM_PAGE = wxID_HIGHEST + 16;
    constexpr static int GRAPH_BOX_PLOT_PAGE = wxID_HIGHEST + 17;

  private:
    constexpr static int ID_HIGHLIGHT_COLOR_BUTTON = wxID_HIGHEST + 18;
    constexpr static int ID_EXCLUDED_HIGHLIGHT_COLOR_BUTTON = wxID_HIGHEST + 19;
    constexpr static int ID_FONT_BUTTON = wxID_HIGHEST + 20;
    constexpr static int ID_DOCUMENT_STORAGE_RADIO_BOX = wxID_HIGHEST + 21;
    constexpr static int ID_FILE_BROWSE_BUTTON = wxID_HIGHEST + 22;
    constexpr static int ID_X_AXIS_FONT_BUTTON = wxID_HIGHEST + 23;
    constexpr static int ID_Y_AXIS_FONT_BUTTON = wxID_HIGHEST + 24;
    constexpr static int ID_GRAPH_TOP_TITLE_FONT_BUTTON = wxID_HIGHEST + 25;
    constexpr static int ID_GRAPH_BOTTOM_TITLE_FONT_BUTTON = wxID_HIGHEST + 26;
    constexpr static int ID_GRAPH_LEFT_TITLE_FONT_BUTTON = wxID_HIGHEST + 27;
    constexpr static int ID_GRAPH_RIGHT_TITLE_FONT_BUTTON = wxID_HIGHEST + 28;
    constexpr static int ID_PARAGRAPH_PARSE = wxID_HIGHEST + 29;
    constexpr static int ID_DUP_WORD_COLOR_BUTTON = wxID_HIGHEST + 30;
    constexpr static int ID_WORDY_PHRASE_COLOR_BUTTON = wxID_HIGHEST + 31;
    constexpr static int ID_DOLCH_CONJUNCTIONS_COLOR_BUTTON = wxID_HIGHEST + 32;
    constexpr static int ID_DOLCH_PREPOSITIONS_COLOR_BUTTON = wxID_HIGHEST + 33;
    constexpr static int ID_DOLCH_PRONOUNS_COLOR_BUTTON = wxID_HIGHEST + 34;
    constexpr static int ID_DOLCH_ADVERBS_COLOR_BUTTON = wxID_HIGHEST + 35;
    constexpr static int ID_DOLCH_ADJECTIVES_COLOR_BUTTON = wxID_HIGHEST + 36;
    constexpr static int ID_DOLCH_NOUN_COLOR_BUTTON = wxID_HIGHEST + 37;
    constexpr static int ID_EXPORT_SETTINGS_BUTTON = wxID_HIGHEST + 38;
    constexpr static int ID_LOAD_SETTINGS_BUTTON = wxID_HIGHEST + 39;
    constexpr static int ID_RESET_SETTINGS_BUTTON = wxID_HIGHEST + 40;
    constexpr static int ID_TEXT_EXCLUDE_METHOD = wxID_HIGHEST + 41;
    constexpr static int ID_SENTENCE_LONGER_THAN_BUTTON = wxID_HIGHEST + 42;
    constexpr static int ID_SENTENCE_OUTLIER_LENGTH_BUTTON = wxID_HIGHEST + 43;
    constexpr static int ID_IGNORE_BLANK_LINES_BUTTON = wxID_HIGHEST + 44;
    constexpr static int ID_IGNORE_INDENTING_BUTTON = wxID_HIGHEST + 45;
    constexpr static int ID_WARNING_MESSAGES_BUTTON = wxID_HIGHEST + 46;
    constexpr static int ID_ADD_FILE_BUTTON = wxID_HIGHEST + 47;
    constexpr static int ID_DELETE_FILE_BUTTON = wxID_HIGHEST + 48;
    constexpr static int ID_NUMBER_SYLLABIZE_METHOD = wxID_HIGHEST + 49;
    constexpr static int ID_DOLCH_VERBS_COLOR_BUTTON = wxID_HIGHEST + 50;
    constexpr static int ID_AGGRESSIVE_LIST_DEDUCTION_CHECKBOX = wxID_HIGHEST + 51;
    constexpr static int ID_EXCLUDED_PHRASES_FILE_EDIT_BUTTON = wxID_HIGHEST + 52;
    constexpr static int ID_EXCLUDE_COPYRIGHT_CHECKBOX = wxID_HIGHEST + 53;
    constexpr static int ID_EXCLUDE_CITATIONS_CHECKBOX = wxID_HIGHEST + 54;
    constexpr static int ID_EXCLUDE_FILE_ADDRESS_CHECKBOX = wxID_HIGHEST + 55;
    constexpr static int ID_EXCLUDE_NUMERALS_CHECKBOX = wxID_HIGHEST + 56;
    constexpr static int ID_EXCLUDE_PROPER_NOUNS_CHECKBOX = wxID_HIGHEST + 57;
    constexpr static int ID_INCOMPLETE_SENTENCE_VALID_VALUE_BOX = wxID_HIGHEST + 58;
    constexpr static int ID_APPLY_GRAPH_BACKGROUND_COLOR_FADE_CHECK_BOX = wxID_HIGHEST + 59;
    constexpr static int ID_GRAPH_BACKGROUND_OPACITY_SLIDER_LABEL = wxID_HIGHEST + 60;
    constexpr static int ID_GRAPH_BACKGROUND_OPACITY_SLIDER_PLUS_LABEL = wxID_HIGHEST + 61;
    constexpr static int ID_PLOT_COLOR_LABEL = wxID_HIGHEST + 62;
    constexpr static int ID_WATERMARK_LABEL = wxID_HIGHEST + 63;
    constexpr static int ID_WATERMARK_TEXT_BOX = wxID_HIGHEST + 64;
    constexpr static int ID_GRAPH_LOGO_LABEL = wxID_HIGHEST + 65;
    constexpr static int ID_BAR_CHART_EFFECTS_OPTIONS = wxID_HIGHEST + 66;
    constexpr static int ID_BAR_CHART_HORIZONTAL_OPTION = wxID_HIGHEST + 67;
    constexpr static int ID_BAR_CHART_VERTICAL_OPTION = wxID_HIGHEST + 68;
    constexpr static int ID_DOCUMENT_FONT_LABEL = wxID_HIGHEST + 69;
    constexpr static int ID_GRAPH_OPTIONS_PROPERTYGRID = wxID_HIGHEST + 70;
    constexpr static int ID_BARCHART_OPTIONS_PROPERTYGRID = wxID_HIGHEST + 71;
    constexpr static int ID_INCLUDE_FIRST_OCCURRENCE_EXCLUDE_CHECKBOX = wxID_HIGHEST + 72;
    constexpr static int ID_INCOMPLETE_SENTENCE_VALID_LABEL_START = wxID_HIGHEST + 73;
    constexpr static int ID_INCOMPLETE_SENTENCE_VALID_LABEL_END = wxID_HIGHEST + 74;
    constexpr static int ID_EXCLUSION_TAG_BLOCK_LABEL = wxID_HIGHEST + 75;
    constexpr static int ID_EXCLUSION_TAG_BLOCK_SELECTION = wxID_HIGHEST + 76;
    constexpr static int ID_ADDITIONAL_FILE_BROWSE_BUTTON = wxID_HIGHEST + 77;
    constexpr static int ID_ADD_FILES_BUTTON = wxID_HIGHEST + 78;
    constexpr static int ID_THEME_COMBO = wxID_HIGHEST + 79;
    constexpr static int ID_ADDITIONAL_FILE_FIELD = wxID_HIGHEST + 80;
    constexpr static int ID_SENTENCES_MUST_BE_CAP_BUTTON = wxID_HIGHEST + 81;
    constexpr static int ID_REALTIME_UPDATE_BUTTON = wxID_HIGHEST + 82;
    constexpr static int ID_DOCUMENT_DESCRIPTION_FIELD = wxID_HIGHEST + 83;
    constexpr static int ID_FILE_LIST = wxID_HIGHEST + 84;
    constexpr static int ID_DOCUMENT_PATH_FIELD = wxID_HIGHEST + 85;
    constexpr static int ID_JS_COOKIES_CHECKBOX = wxID_HIGHEST + 86;

    // button events
    void OnExportSettings([[maybe_unused]] wxCommandEvent& event);
    void OnImportSettings([[maybe_unused]] wxCommandEvent& event);
    void OnResetSettings([[maybe_unused]] wxCommandEvent& event);
    void OnParagraphParseChange([[maybe_unused]] wxCommandEvent& event);
    void OnHighlightColorSelect([[maybe_unused]] wxCommandEvent& event);
    void OnExcludedHighlightColorSelect([[maybe_unused]] wxCommandEvent& event);
    void OnDupWordHighlightColorSelect([[maybe_unused]] wxCommandEvent& event);
    void OnWordyPhraseHighlightColorSelect([[maybe_unused]] wxCommandEvent& event);
    void OnFontSelect(wxCommandEvent& event);
    void OnDocumentStorageRadioButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnFileBrowseButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnAdditionalDocumentFileBrowseButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnIncompleteSentencesChange([[maybe_unused]] wxCommandEvent& event);
    void OnExcludedPhrasesFileEditButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnExcludeNumeralsCheck(wxCommandEvent& event);
    // dolch color buttons
    void OnDolchConjunctionsHighlightColorSelect([[maybe_unused]] wxCommandEvent& event);
    void OnDolchPrepositionsHighlightColorSelect([[maybe_unused]] wxCommandEvent& event);
    void OnDolchPronounsHighlightColorSelect([[maybe_unused]] wxCommandEvent& event);
    void OnDolchAdverbsHighlightColorSelect([[maybe_unused]] wxCommandEvent& event);
    void OnDolchHighlightColorSelect(wxCommandEvent& event);
    void OnDolchNounHighlightColorSelect([[maybe_unused]] wxCommandEvent& event);
    // general events
    void OnWarningMessagesButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnHelp([[maybe_unused]] wxCommandEvent& event);
    void OnContextHelp([[maybe_unused]] wxHelpEvent& event);
    void OnOK([[maybe_unused]] wxCommandEvent& event);
    void OnAddFilesClick([[maybe_unused]] wxCommandEvent& event);
    void OnAddFileClick([[maybe_unused]] wxCommandEvent& event);
    void OnDeleteFileClick([[maybe_unused]] wxCommandEvent& event);
    void OnNumberSyllabizeChange([[maybe_unused]] wxCommandEvent& event);
    void OnExclusionBlockTagChange([[maybe_unused]] wxCommandEvent& event);

    // state functions
    [[nodiscard]]
    bool IsGeneralSettings() const noexcept;
    [[nodiscard]]
    bool IsStandardProjectSettings() const;
    [[nodiscard]]
    bool IsBatchProjectSettings() const;

    // saving functions
    bool ValidateOptions();
    void SaveOptions();
    void SaveProjectGraphOptions();
    void SaveTextWindowOptions();
    void SaveStatisticsOptions();
    [[nodiscard]]
    bool HaveOptionsChanged() const;
    [[nodiscard]]
    bool HaveDocumentOptionsChanged() const;
    [[nodiscard]]
    bool HaveGraphOptionsChanged() const;
    [[nodiscard]]
    bool HaveTextViewOptionsChanged() const noexcept;
    [[nodiscard]]
    bool HaveTestDisplayOptionsChanged() const;
    [[nodiscard]]
    bool HaveStatisticsOptionsChanged() const;
    [[nodiscard]]
    bool HaveWordsBreakdownOptionsChanged() const;
    [[nodiscard]]
    bool HaveSentencesBreakdownOptionsChanged() const;

    [[nodiscard]]
    Wisteria::UI::ListCtrlEx* GetFileList() const noexcept
        {
        return m_fileList;
        }

    /// Creates the controls and sizers
    void CreateControls();
    void CreateGraphSection();

    [[nodiscard]]
    static wxString GetCustomTestsLabel()
        {
        return _(L"Custom Tests");
        }

    [[nodiscard]]
    static wxString GetInvalidRegionsColorLabel()
        {
        return _(L"Invalid regions color");
        }

    [[nodiscard]]
    static wxString GetRaygorStyleLabel()
        {
        return _(L"Raygor style");
        }

    [[nodiscard]]
    static wxString GetFleschChartConnectPointsLabel()
        {
        return _(L"Connect points");
        }

    [[nodiscard]]
    static wxString GetFleschSyllableRulerDocGroupsLabel()
        {
        return _(L"Display grouped documents on syllable ruler");
        }

    [[nodiscard]]
    static wxString GetUseEnglishLabelsForGermanLixLabel()
        {
        return _(L"Use English translations for German Lix gauge");
        }

    [[nodiscard]]
    static wxString GetColorLabel()
        {
        return _(L"Color");
        }

    [[nodiscard]]
    static wxString GetBackgroundColorLabel()
        {
        return _DT(L"BGCOLOR");
        }

    [[nodiscard]]
    static wxString GetEffectLabel()
        {
        return _(L"Effect");
        }

    [[nodiscard]]
    static wxString GetFitLabel()
        {
        return _(L"Fit");
        }

    [[nodiscard]]
    static wxString GetOpacityLabel()
        {
        return _(L"Opacity");
        }

    [[nodiscard]]
    static wxString GetImageFitLabel()
        {
        return GetImageLabel() + L"." + GetFitLabel();
        }

    [[nodiscard]]
    static wxString GetColorOpacityLabel()
        {
        return GetColorLabel() + L"." + GetOpacityLabel();
        }

    [[nodiscard]]
    static wxString GetImageOpacityLabel()
        {
        return GetImageLabel() + L"." + GetOpacityLabel();
        }

    [[nodiscard]]
    static wxString GetImageEffectLabel()
        {
        return GetImageLabel() + L"." + GetEffectLabel();
        }

    [[nodiscard]]
    static wxString GetLabelsOnBarsLabel()
        {
        return _(L"Display labels on bars");
        }

    [[nodiscard]]
    static wxString GetLabelsOnBoxesLabel()
        {
        return _(L"Display box & whisker labels");
        }

    [[nodiscard]]
    static wxString GetConnectBoxesLabel()
        {
        return _(L"Connect middle points");
        }

    [[nodiscard]]
    static wxString GetShowAllDataPointsLabel()
        {
        return _(L"Show all data points");
        }

    [[nodiscard]]
    static wxString GeOrientationLabel()
        {
        return _(L"Orientation");
        }

    [[nodiscard]]
    static wxString GetBinSortingLabel()
        {
        return _(L"Sorting");
        }

    [[nodiscard]]
    static wxString GetGradeLevelRoundingLabel()
        {
        return _(L"Grade level/index value rounding");
        }

    [[nodiscard]]
    static wxString GetIntervalDisplayLabel()
        {
        return _(L"Interval display");
        }

    [[nodiscard]]
    static wxString GetBinLabelsLabel()
        {
        return _(L"Labels");
        }

    [[nodiscard]]
    static wxString GetDisplayDropShadowsLabel()
        {
        return _(L"Display drop shadows");
        }

    [[nodiscard]]
    static wxString GetShowcaseKeyItemsLabel()
        {
        // TRANSLATORS: "key" as in important.
        return _(L"Showcase key items");
        }

    [[nodiscard]]
    static wxString GetStippleImageLabel()
        {
        return _(L"Stipple image");
        }

    [[nodiscard]]
    static wxString GetStippleShapeLabel()
        {
        return _(L"Stipple shape");
        }

    [[nodiscard]]
    static wxString GetStippleShapeColorLabel()
        {
        return _(L"Stipple shape") + L"." + GetColorLabel();
        }

    [[nodiscard]]
    static wxString GetCommonImageLabel()
        {
        return _(L"Common image");
        }

    [[nodiscard]]
    static wxString GetWatermarkLabel()
        {
        return _(L"Watermark");
        }

    [[nodiscard]]
    static wxString GetLogoImageLabel()
        {
        return _(L"Logo image");
        }

    [[nodiscard]]
    static wxString GetImageLabel()
        {
        return _(L"Image");
        }

    [[nodiscard]]
    static wxString GetApplyFadeLabel()
        {
        return _(L"Fade");
        }

    [[nodiscard]]
    static wxString GetBackgroundColorFadeLabel()
        {
        return GetBackgroundColorLabel() + L"." + GetApplyFadeLabel();
        }

    [[nodiscard]]
    static wxString GetBackgroundLabel()
        {
        return _(L"Background");
        }

    [[nodiscard]]
    static wxString GetIgnoreProperNounsLabel()
        {
        return _(L"Ignore proper nouns");
        }

    [[nodiscard]]
    static wxString GetIgnoreUppercasedWordsLabel()
        {
        return _(L"Ignore UPPERCASED words");
        }

    [[nodiscard]]
    static wxString GetIgnoreNumeralsLabel()
        {
        return _(L"Ignore numerals");
        }

    [[nodiscard]]
    static wxString GetIgnoreFileAddressesLabel()
        {
        return _(L"Ignore Internet and file addresses");
        }

    [[nodiscard]]
    static wxString GetIgnoreProgrammerCodeLabel()
        {
        return _(L"Ignore programmer code");
        }

    [[nodiscard]]
    static wxString GetAllowColloquialismsLabel()
        {
        return _(L"Allow colloquialisms");
        }

    [[nodiscard]]
    static wxString GetIgnoreSocialMediaLabel()
        {
        return _(L"Ignore social media hashtags");
        }

    [[nodiscard]]
    static wxString GetResultsLabel()
        {
        return _(L"Results");
        }

    [[nodiscard]]
    static wxString GetGrammarHighlightedReportLabel()
        {
        return _(L"Highlighted Report");
        }

    [[nodiscard]]
    static wxString GetSpellCheckerLabel()
        {
        return _(L"Spell Checker");
        }

    [[nodiscard]]
    static wxString GetDolchStatisticsReportLabel()
        {
        return _(L"Dolch Report");
        }

    [[nodiscard]]
    static wxString GetCoverageLabel()
        {
        return _(L"Coverage");
        }

    [[nodiscard]]
    static wxString GetDolchWordsLabel()
        {
        return _DT(L"DOLCHWORDS");
        }

    [[nodiscard]]
    static wxString GetDolchExplanationLabel()
        {
        return _(L"Explanation");
        }

    [[nodiscard]]
    static wxString GetStatisticsReportLabel()
        {
        return _(L"Summary Report");
        }

    [[nodiscard]]
    static wxString GetParagraphsLabel()
        {
        return _(L"Paragraphs");
        }

    [[nodiscard]]
    static wxString GetSentencesLabel()
        {
        return _(L"Sentences");
        }

    [[nodiscard]]
    static wxString GetWordsLabel()
        {
        return _(L"Words");
        }

    [[nodiscard]]
    static wxString GetExtendedWordsLabel()
        {
        return _(L"Extended Words");
        }

    [[nodiscard]]
    static wxString GetGrammarLabel()
        {
        return _(L"Grammar");
        }

    [[nodiscard]]
    static wxString GetNotesLabel()
        {
        return _(L"Notes");
        }

    [[nodiscard]]
    static wxString GetExtendedInformationLabel()
        {
        return _(L"Extended Information");
        }

    [[nodiscard]]
    static wxString GetFryLikeLabel()
        {
        return _DT(L"Fry/GPM/Raygor/Schwartz");
        }

    [[nodiscard]]
    static wxString GetFleschChartLabel()
        {
        return _(L"Flesch Chart");
        }

    [[nodiscard]]
    static wxString GetLixGaugeLabel()
        {
        return _(L"Lix Gauge");
        }

    [[nodiscard]]
    static wxString GetBarAppearanceLabel()
        {
        return _(L"Bar Appearance");
        }

    [[nodiscard]]
    static wxString GetBinningOptionsLabel()
        {
        // TRANSLATORS: "Bins" are data ranges in a histogram that values fall into.
        return _(L"Binning Options");
        }

    [[nodiscard]]
    static wxString GetBinDisplayLabel()
        {
        // TRANSLATORS: "Bins" are data ranges in a histogram that values fall into.
        return _(L"Bin Display");
        }

    [[nodiscard]]
    static wxString GetBoxAppearanceLabel()
        {
        // TRANSLATORS: The boxes in a box plot.
        return _(L"Box Appearance");
        }

    [[nodiscard]]
    static wxString GetBoxOptionsLabel()
        {
        // TRANSLATORS: The boxes in a box plot.
        return _(L"Box Options");
        }

    [[nodiscard]]
    static wxString GetGraphColorSchemeLabel()
        {
        return _(L"Color Scheme");
        }

    [[nodiscard]]
    static wxString GetGraphBackgroundLabel()
        {
        return _(L"Background");
        }

    [[nodiscard]]
    static wxString GetPlotAreaBackgroundLabel()
        {
        return _(L"Plot Background");
        }

    [[nodiscard]]
    static wxString GetWatermarksLogosLabel()
        {
        return _(L"Watermarks & Logos");
        }

    [[nodiscard]]
    static wxString GetEffectsLabel()
        {
        return _(L"Effects");
        }

    [[nodiscard]]
    static wxString GetGradeScaleLabel()
        {
        return _(L"Grade scale");
        }

    [[nodiscard]]
    static wxString GetGradeLabel()
        {
        return _(L"Grade Display");
        }

    [[nodiscard]]
    static wxString GetGradesLongFormatLabel()
        {
        return _(L"Display scores in long format");
        }

    [[nodiscard]]
    static wxString GetReadingAgeLabel()
        {
        return _(L"Reading Age Display");
        }

    [[nodiscard]]
    static wxString GetScoreResultsLabel()
        {
        return _(L"Results");
        }

    [[nodiscard]]
    static wxString GetIncludeScoreSummaryLabel()
        {
        return _(L"Include test-summary report");
        }

    [[nodiscard]]
    static wxString GetCalculationLabel()
        {
        return _(L"Calculation");
        }

    [[nodiscard]]
    static wxString GetCountIndependentClausesLabel()
        {
        return _(L"Count independent clauses");
        }

    [[nodiscard]]
    static wxString GetNumeralSyllabicationLabel()
        {
        return _(L"Numeral syllabication");
        }

    [[nodiscard]]
    static wxString GetFleschNumeralSyllabicationLabel()
        {
        return _DT(L"FLESCHNUMSYL");
        }

    [[nodiscard]]
    static wxString GetFleschKincaidNumeralSyllabicationLabel()
        {
        return _DT(L"FLESCHKINCAIDNUMSYL");
        }

    [[nodiscard]]
    static wxString GetIncludeStockerLabel()
        {
        return _(L"Include Catholic Supplement");
        }

    [[nodiscard]]
    static wxString GetProperNounsLabel()
        {
        return _(L"Proper nouns");
        }

    [[nodiscard]]
    static wxString GetTextExclusionLabel()
        {
        return _(L"Text exclusion");
        }

    [[nodiscard]]
    static wxString GetDCTextExclusionLabel()
        {
        return _DT(L"DCTEXTEXCLUSION");
        }

    [[nodiscard]]
    static wxString GetHJCTextExclusionLabel()
        {
        return _DT(L"HJTEXTEXCLUSION");
        }

    [[nodiscard]]
    static wxString GetReadabilityScoresLabel()
        {
        return _(L"Readability Scores");
        }

    [[nodiscard]]
    static wxString GetGeneralSettingsLabel()
        {
        return _(L"General Settings");
        }

    [[nodiscard]]
    static wxString GetStatisticsLabel()
        {
        return _(L"Statistics");
        }

    [[nodiscard]]
    static wxString GetAnalysisLabel()
        {
        return _(L"Analysis");
        }

    [[nodiscard]]
    static wxString GetDolchSightWordsLabel()
        {
        return _(L"Dolch Sight Words");
        }

    [[nodiscard]]
    static wxString GetDocumentViewsLabel()
        {
        return _(L"Document Views");
        }

    [[nodiscard]]
    static wxString GetDifficultWordListsLabel()
        {
        return _(L"Difficult word lists");
        }

    [[nodiscard]]
    static wxString GetGraphsLabel()
        {
        return ProjectView::GetGraphsLabel();
        }

    [[nodiscard]]
    static wxString GetGeneralLabel()
        {
        return _(L"General");
        }

    [[nodiscard]]
    static wxString GetAxisSettingsLabel()
        {
        return _(L"Axes");
        }

    [[nodiscard]]
    static wxString GetTitlesLabel()
        {
        return _(L"Titles");
        }

    [[nodiscard]]
    static wxString GetReadabilityGraphLabel()
        {
        return _(L"Readability Graphs");
        }

    [[nodiscard]]
    static wxString GetBarChartLabel()
        {
        return _(L"Bar Charts");
        }

    [[nodiscard]]
    static wxString GetBoxPlotLabel()
        {
        return _(L"Box Plots");
        }

    [[nodiscard]]
    static wxString GetHistogramsLabel()
        {
        return _(L"Histograms");
        }

    [[nodiscard]]
    static wxString GetProjectSettingsLabel()
        {
        return _(L"Project Settings");
        }

    [[nodiscard]]
    static wxString GetSentencesWordsLabel()
        {
        return _(L"Sentences && Words");
        }

    [[nodiscard]]
    static wxString GetTestOptionsLabel()
        {
        return _(L"Test Options");
        }

    [[nodiscard]]
    static wxString GetScoreDisplayLabel()
        {
        return _(L"Scores");
        }

    /// Replaces various filepath shortcuts to the full pathway
    [[nodiscard]]
    static wxString ExpandPath(wxString path) ;

    [[nodiscard]]
    ToolSections GetSectionsBeingShown() const noexcept
        {
        return m_sectionsBeingShown;
        }

    static bool IsPropertyAvailable(const wxPropertyGridPage* propGrid,
                                    const wxString& propertyName)
        {
        return ((propGrid != nullptr) && (propGrid->GetProperty(propertyName) != nullptr));
        }

    static void CreateLabelHeader(wxWindow* parent, wxSizer* parentSizer, const wxString& title,
                           const bool addSidePadding = false) ;

    BaseProjectDoc* m_readabilityProjectDoc{ nullptr };
    wxRadioBox* m_docStorageRadioBox{ nullptr };
    wxCheckBox* m_persistCookiesCheck{ nullptr };
    wxCheckBox* m_realTimeUpdateCheckBox{ nullptr };
    wxCheckBox* m_ignoreCopyrightsCheckBox{ nullptr };
    wxCheckBox* m_ignoreCitationsCheckBox{ nullptr };
    wxCheckBox* m_ignoreFileAddressesCheckBox{ nullptr };
    wxCheckBox* m_ignoreNumeralsCheckBox{ nullptr };
    wxCheckBox* m_ignoreProperNounsCheckBox{ nullptr };
    wxCheckBox* m_includeExcludedPhraseFirstOccurrenceCheckBox{ nullptr };
    wxTextCtrl* m_excludedPhrasesPathFilePathEdit{ nullptr };
    wxBitmapButton* m_excludedPhrasesEditBrowseButton{ nullptr };
    wxCheckBox* m_aggressiveExclusionCheckBox{ nullptr };
    wxCheckBox* m_ignoreBlankLinesCheckBox{ nullptr };
    wxCheckBox* m_ignoreIndentingCheckBox{ nullptr };
    wxButton* m_highlightColorButton{ nullptr };
    wxButton* m_excludedHighlightColorButton{ nullptr };
    wxButton* m_duplicateWordHighlightColorButton{ nullptr };
    wxButton* m_wordyPhraseHighlightColorButton{ nullptr };
    wxButton* m_FontButton{ nullptr };
    wxButton* m_xAxisFontButton{ nullptr };
    wxButton* m_yAxisFontButton{ nullptr };
    wxButton* m_graphTopTitleFontButton{ nullptr };
    wxButton* m_graphBottomTitleFontButton{ nullptr };
    wxButton* m_graphLeftTitleFontButton{ nullptr };
    wxButton* m_graphRightTitleFontButton{ nullptr };
    Wisteria::UI::SideBarBook* m_sideBar{ nullptr };
    wxTextCtrl* m_stippleFilePathEdit{ nullptr };
    wxTextCtrl* m_filePathEdit{ nullptr };
    wxStaticText* m_includeIncompleteSentSizeIncludeIncompleteLabel{ nullptr };
    wxStaticText* m_includeIncompleteSentSizeWordsLabel{ nullptr };
    wxBitmapButton* m_fileBrowseButton{ nullptr };
    wxStaticText* m_syllableLabel{ nullptr };
    wxChoice* m_syllableCombo{ nullptr };
    wxChoice* m_exclusionBlockTagsCombo{ nullptr };
    wxStaticText* m_exclusionBlockTagsLabel{ nullptr };
    // dolch buttons
    wxBitmapButton* m_DolchConjunctionsColorButton{ nullptr };
    wxBitmapButton* m_DolchPrepositionsColorButton{ nullptr };
    wxBitmapButton* m_DolchPronounsColorButton{ nullptr };
    wxBitmapButton* m_DolchAdverbsColorButton{ nullptr };
    wxBitmapButton* m_DolchAdjectivesColorButton{ nullptr };
    wxBitmapButton* m_DolchVerbsColorButton{ nullptr };
    wxBitmapButton* m_DolchNounsColorButton{ nullptr };

    // property grid controls
    wxPropertyGridPage* m_readabilityTestsPropertyGrid{ nullptr };
    wxPropertyGridPage* m_gradeLevelPropertyGrid{ nullptr };
    wxPropertyGridPage* m_grammarPropertyGrid{ nullptr };
    wxPropertyGridPage* m_wordsBreakdownPropertyGrid{ nullptr };
    wxPropertyGridPage* m_sentencesBreakdownPropertyGrid{ nullptr };
    wxPropertyGridPage* m_statisticsPropertyGrid{ nullptr };
    wxPropertyGridPage* m_generalGraphPropertyGrid{ nullptr };
    wxPropertyGridPage* m_readabilityGraphPropertyGrid{ nullptr };
    wxPropertyGridPage* m_barChartPropertyGrid{ nullptr };
    wxPropertyGridPage* m_histogramPropertyGrid{ nullptr };
    wxPropertyGridPage* m_boxPlotsPropertyGrid{ nullptr };

    wxStaticText* m_readTestsSyllableLabel{ nullptr };
    wxStaticText* m_textExclusionLabel{ nullptr };

    wxBitmapButton* m_addFilesButton{ nullptr };
    wxBitmapButton* m_addFileButton{ nullptr };
    wxBitmapButton* m_deleteFileButton{ nullptr };
    Wisteria::UI::ListCtrlEx* m_fileList{ nullptr };
    std::shared_ptr<Wisteria::UI::ListCtrlExDataProvider> m_fileData{
        std::make_shared<Wisteria::UI::ListCtrlExDataProvider>()
    };

    BackupVariable<wxString> m_userAgent;
    BackupVariable<bool> m_disablePeerVerify;
    BackupVariable<bool> m_useJsCookies;
    BackupVariable<bool> m_persistJsCookies;

    BackupVariable<int> m_uiLanguage;

    // log settings
    BackupVariable<bool> m_logVerbose;
    BackupVariable<bool> m_logAppendDailyLog;

    // project settings
    BackupVariable<int> m_projectLanguage;
    BackupVariable<wxString> m_reviewer;
    BackupVariable<wxString> m_status;
    BackupVariable<wxString> m_description;
    BackupVariable<wxString> m_appendedDocumentFilePath;
    BackupVariable<bool> m_realTimeUpdate;

    // which options are we showing
    ToolSections m_sectionsBeingShown;

    // document storage/linking information
    BackupVariable<int> m_documentStorageMethod;
    BackupVariable<wxString> m_filePath;

    BackupVariable<int> m_textHighlightMethod;
    BackupVariable<wxColour> m_highlightedColor;
    BackupVariable<wxColour> m_excludedTextHighlightColor;
    BackupVariable<wxColour> m_duplicateWordHighlightColor;
    BackupVariable<wxColour> m_wordyPhraseHighlightColor;
    BackupVariable<wxFont> m_font;
    BackupVariable<wxColour> m_fontColor;

    // dolch highlighting
    BackupVariable<wxColour> m_dolchConjunctionsColor;
    BackupVariable<wxColour> m_dolchPrepositionsColor;
    BackupVariable<wxColour> m_dolchPronounsColor;
    BackupVariable<wxColour> m_dolchAdverbsColor;
    BackupVariable<wxColour> m_dolchAdjectivesColor;
    BackupVariable<wxColour> m_dolchVerbsColor;
    BackupVariable<wxColour> m_dolchNounsColor;

    BackupVariable<bool> m_highlightDolchConjunctions;
    BackupVariable<bool> m_highlightDolchPrepositions;
    BackupVariable<bool> m_highlightDolchPronouns;
    BackupVariable<bool> m_highlightDolchAdverbs;
    BackupVariable<bool> m_highlightDolchAdjectives;
    BackupVariable<bool> m_highlightDolchVerbs;
    BackupVariable<bool> m_highlightDolchNouns;

    BackupVariable<bool> m_longSentencesNumberOfWords;
    BackupVariable<int> m_sentenceLength;
    BackupVariable<bool> m_longSentencesOutliers;

    BackupVariable<int> m_minDocWordCountForBatch;
    BackupVariable<int> m_filePathTruncationMode;

    BackupVariable<int> m_syllabicationMethod;
    BackupVariable<int> m_paragraphParsingMethod;
    BackupVariable<bool> m_ignoreBlankLinesForParagraphsParser;
    BackupVariable<bool> m_ignoreIndentingForParagraphsParser;
    BackupVariable<bool> m_sentenceStartMustBeUppercased;
    BackupVariable<bool> m_aggressiveExclusion;
    BackupVariable<bool> m_excludeTrailingCopyrightNoticeParagraphs;
    BackupVariable<bool> m_excludeTrailingCitations;
    BackupVariable<bool> m_excludeFileAddresses;
    BackupVariable<bool> m_excludeNumerals;
    BackupVariable<bool> m_excludeProperNouns;
    BackupVariable<wxString> m_excludedPhrasesPath;
    bool m_excludedPhrasesEdited{ false };
    BackupVariable<bool> m_includeExcludedPhraseFirstOccurrence;
    BackupVariable<std::vector<std::pair<wchar_t, wchar_t>>> m_exclusionBlockTags;
    int m_exclusionBlockTagsOption{ 0 };
    BackupVariable<int> m_includeIncompleteSentencesIfLongerThan;
    BackupVariable<int> m_textExclusionMethod;

    // graph options
    BackupVariable<wxColour> m_xAxisFontColor;
    BackupVariable<wxFont> m_xAxisFont;
    BackupVariable<wxColour> m_yAxisFontColor;
    BackupVariable<wxFont> m_yAxisFont;
    BackupVariable<wxColour> m_topTitleFontColor;
    BackupVariable<wxFont> m_topTitleFont;
    BackupVariable<wxColour> m_bottomTitleFontColor;
    BackupVariable<wxFont> m_bottomTitleFont;
    BackupVariable<wxColour> m_leftTitleFontColor;
    BackupVariable<wxFont> m_leftTitleFont;
    BackupVariable<wxColour> m_rightTitleFontColor;
    BackupVariable<wxFont> m_rightTitleFont;

    wxDECLARE_CLASS(ToolsOptionsDlg);
    };

#endif // TOOLS_OPTIONS_DIALOG_H
