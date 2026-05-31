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

#ifndef BASE_PROJECT_VIEW_H
#define BASE_PROJECT_VIEW_H

#include "../Wisteria-Dataviz/src/ui/controls/formattedtextctrl.h"
#include "../Wisteria-Dataviz/src/ui/controls/listctrlex.h"
#include "../Wisteria-Dataviz/src/ui/controls/searchpanel.h"
#include "../Wisteria-Dataviz/src/ui/controls/sidebar.h"
#include "../Wisteria-Dataviz/src/util/formulaformat.h"
#include "../Wisteria-Dataviz/src/util/idhelpers.h"
#include "../Wisteria-Dataviz/src/util/parentblocker.h"
#include "../Wisteria-Dataviz/src/util/warningmanager.h"
#include "../results-format/readability_messages.h"
#include "../ui/controls/explanation_listctrl.h"
#include "project_frame.h"
#include <wx/infobar.h>

/// @brief Base view for batch and standard projects.
class BaseProjectView : public wxView
    {
  public:
    BaseProjectView() = default;
    BaseProjectView(const BaseProjectView&) = delete;
    BaseProjectView& operator=(const BaseProjectView&) = delete;

    ~BaseProjectView() override;

    void SetLeftPaneSize(const int leftPaneWidth)
        {
        m_splitter->SetMinimumPaneSize(leftPaneWidth);
        m_splitter->SplitVertically(m_splitter->GetWindow1(), m_splitter->GetWindow2(),
                                    leftPaneWidth);
        }

    void ShowSideBar(const bool show = true);

    [[nodiscard]]
    bool IsSideBarShown() const noexcept
        {
        return m_sidebarShown;
        }

    [[nodiscard]]
    wxMenuBar* GetMenuBar() noexcept
        {
        return m_menuBar;
        }

    [[nodiscard]]
    wxSplitterWindow* GetSplitter() noexcept
        {
        return m_splitter;
        }

    [[nodiscard]]
    wxBoxSizer* GetWorkSpaceSizer() noexcept
        {
        return m_workSpaceSizer;
        }

    [[nodiscard]]
    Wisteria::UI::SideBar* GetSideBar() noexcept
        {
        return m_sideBar;
        }

    [[nodiscard]]
    const Wisteria::UI::SideBar* GetSideBar() const noexcept
        {
        return m_sideBar;
        }

    [[nodiscard]]
    wxInfoBar* GetInfoBar() noexcept
        {
        return m_infoBar;
        }

    [[nodiscard]]
    const wxWindow* GetActiveProjectWindow() const noexcept
        {
        return m_activeWindow;
        }

    [[nodiscard]]
    wxWindow* GetActiveProjectWindow() noexcept
        {
        return m_activeWindow;
        }

    /// Fills the menus in the ribbon and checks/toggles all the options on the
    /// ribbon to match the project's settings.
    /// Also autofits the sidebar and makes the project the active document/window.
    void Present();

    // update toggle states of ribbon buttons after ribbon is created or
    // project was changed from properties dialog
    virtual void UpdateRibbonState() {}

    void ShowInfoMessage(const WarningMessage& message);

    bool OnCreate(wxDocument* doc, [[maybe_unused]] long flags) override;

    [[nodiscard]]
    static ProjectDocChildFrame* CreateChildFrame(wxDocument* doc, wxView* view);

    [[nodiscard]]
    ProjectDocChildFrame* GetDocFrame() noexcept
        {
        return m_frame;
        }

    [[nodiscard]]
    const ProjectDocChildFrame* GetDocFrame() const noexcept
        {
        return m_frame;
        }

    [[nodiscard]]
    wxRibbonBar* GetRibbon() noexcept
        {
        return m_ribbon;
        }

    [[nodiscard]]
    wxAuiToolBar* GetQuickToolbar() noexcept
        {
        return m_quickToolbar;
        }

    [[nodiscard]]
    Wisteria::UI::SearchPanel* GetSearchPanel() noexcept
        {
        return m_searchCtrl;
        }

    // labels for the icons on the left-side pane used for project navigation
    [[nodiscard]]
    static wxString GetFormattedReportLabel()
        {
        return _(L"Formatted Report");
        }

    [[nodiscard]]
    static wxString GetTabularReportLabel()
        {
        return _(L"Tabular Report");
        }

    [[nodiscard]]
    static wxString GetWordsBreakdownLabel()
        {
        return _(L"Words Breakdown");
        }

    [[nodiscard]]
    static wxString GetSentencesBreakdownLabel()
        {
        return _(L"Sentences Breakdown");
        }

    [[nodiscard]]
    static wxString GetDifficultWordsLabel()
        {
        return _(L"Difficult Words");
        }

    [[nodiscard]]
    static wxString GetReadabilityScoresLabel()
        {
        return _(L"Readability Scores");
        }

    [[nodiscard]]
    static wxString GetHighlightedReportsLabel()
        {
        return _(L"Highlighted Reports");
        }

    [[nodiscard]]
    static wxString GetGraphsLabel()
        {
        return _(L"Graphs");
        }

    [[nodiscard]]
    static wxString GetGrammarLabel()
        {
        return _(L"Grammar");
        }

    [[nodiscard]]
    static wxString GetDolchLabel()
        {
        return _(L"Sight Words");
        }

    [[nodiscard]]
    static wxString GetBoxPlotsLabel()
        {
        return _(L"Box Plots");
        }

    [[nodiscard]]
    static wxString GetTestGraphsLabel()
        {
        return _(L"Readability Graphs");
        }

    [[nodiscard]]
    static wxString GetWarningLabel()
        {
        return _(L"Warnings");
        }

    // label functions
    [[nodiscard]]
    static wxString GetSentenceStartingWithConjunctionsLabel()
        {
        return _(L"Sentences that begin with conjunctions");
        }

    [[nodiscard]]
    static wxString GetSentenceStartingWithConjunctionsTabLabel()
        {
        return _(L"Conjunction-starting Sentences");
        }

    [[nodiscard]]
    static wxString GetSentenceStartingWithLowercaseLabel()
        {
        return _(L"Sentences that begin with lowercased words");
        }

    [[nodiscard]]
    static wxString GetSentenceStartingWithLowercaseTabLabel()
        {
        return _(L"Lowercased Sentences");
        }

    [[nodiscard]]
    static wxString GetWordyPhrasesTabLabel()
        {
        return _(L"Wordy Items");
        }

    [[nodiscard]]
    static wxString GetRedundantPhrasesTabLabel()
        {
        return _(L"Redundant Phrases");
        }

    [[nodiscard]]
    static wxString GetClichesTabLabel()
        {
        return _(L"Clich\u00E9s");
        }

    [[nodiscard]]
    static wxString GetPhrasingErrorsTabLabel()
        {
        return _(L"Errors & Misspellings");
        }

    [[nodiscard]]
    static wxString GetSummaryStatisticsLabel()
        {
        return _(L"Summary Statistics");
        }

    [[nodiscard]]
    static wxString GetHistogramsLabel()
        {
        return _(L"Histograms");
        }

    [[nodiscard]]
    static wxString GetCoverageChartTabLabel()
        {
        return _(L"Coverage Chart");
        }

    [[nodiscard]]
    static wxString GetWordCountsLabel()
        {
        return _(L"Word Counts");
        }

    [[nodiscard]]
    static wxString GetSyllableCountsLabel()
        {
        return _(L"Syllable Counts");
        }

    [[nodiscard]]
    static wxString GetSentenceLengthBoxPlotLabel()
        {
        return _(L"Lengths (Spread)");
        }

    [[nodiscard]]
    static wxString GetSentenceLengthHistogramLabel()
        {
        return _(L"Lengths (Distribution)");
        }

    [[nodiscard]]
    static wxString GetSentenceLengthHeatmapLabel()
        {
        return _(L"Lengths (Density)");
        }

    [[nodiscard]]
    static wxString GetWordCloudLabel()
        {
        return _(L"Key Word Cloud");
        }

    // Dolch labels
    [[nodiscard]]
    static wxString GetDolchWordTabLabel()
        {
        return _(L"Dolch Words");
        }

    [[nodiscard]]
    static wxString GetNonDolchWordTabLabel()
        {
        return _(L"Non-Dolch Words");
        }

    [[nodiscard]]
    static wxString GetUnusedDolchWordTabLabel()
        {
        return _(L"Unused Dolch Words");
        }

    [[nodiscard]]
    static wxString GetDolchCoverageTabLabel()
        {
        return _(L"Dolch Word Coverage");
        }

    // batch tab labels
    [[nodiscard]]
    static wxString GetRawScoresTabLabel()
        {
        return _(L"Raw Scores");
        }

    // stat headers
    [[nodiscard]]
    static wxString GetAverageLabel()
        {
        return _(L"Average (Mean)");
        }

    [[nodiscard]]
    static wxString GetMedianLabel()
        {
        return _(L"Median");
        }

    [[nodiscard]]
    static wxString GetModeLabel()
        {
        // TRANSLATORS: Statistical modes. (Most frequently occurring values in a data range.)
        return _(L"Modes");
        }

    [[nodiscard]]
    static wxString GetStdDevLabel()
        {
        return _(L"Std. Dev.");
        }

    // errors
    [[nodiscard]]
    static wxString GetFailedLabel()
        {
        // TRANSLATORS: This is an adjective
        return L"\u26A0 " + _(L"Failed");
        } // includes warning emoji

    // tab labels
    [[nodiscard]]
    static wxString GetAllWordsLabel()
        {
        return _(L"All Words");
        }

    [[nodiscard]]
    static wxString GetKeyWordsLabel()
        {
        // TRANSLATORS: "Key" as in important.
        return _(L"Key Words");
        }

    [[nodiscard]]
    static wxString GetRepeatedWordsLabel()
        {
        return _(L"Repeated Words");
        }

    [[nodiscard]]
    static wxString GetLongSentencesLabel()
        {
        return _(L"Long Sentences");
        }

    [[nodiscard]]
    static wxString GetRaygorLabel()
        {
        return _(L"Raygor Estimate");
        }

    [[nodiscard]]
    static wxString GetFryLabel()
        {
        return _(L"Fry Graph");
        }

    [[nodiscard]]
    static wxString GetGilliamPenaMountainFryLabel()
        {
        return _(L"Gilliam-Pe\U000000F1a-Mountain Graph");
        }

    [[nodiscard]]
    static wxString GetFraseLabel()
        {
        return _(L"FRASE Graph");
        }

    [[nodiscard]]
    static wxString GetSchwartzLabel()
        {
        return _(L"Schwartz Graph");
        }

    [[nodiscard]]
    static wxString GetLixGaugeLabel()
        {
        return _(L"Lix Gauge");
        }

    [[nodiscard]]
    static wxString GetGermanLixGaugeLabel()
        {
        return _(L"German Lix Gauge");
        }

    [[nodiscard]]
    static wxString GetFleschChartLabel()
        {
        return _(L"Flesch Chart");
        }

    [[nodiscard]]
    static wxString GetInfleszScaleLabel()
        {
        return _(L"INFLESZ Scale");
        }

    [[nodiscard]]
    static wxString GetDB2Label()
        {
        return _DT(L"Danielson-Bryan 2");
        }

    [[nodiscard]]
    static wxString GetCrawfordGraphLabel()
        {
        return _(L"Crawford Graph");
        }

    [[nodiscard]]
    static wxString GetSixCharWordsLabel()
        {
        return _(L"6+ Characters");
        }

    [[nodiscard]]
    static wxString GetSixCharWordsListLabel()
        {
        return _(L"6+ Characters List");
        }

    [[nodiscard]]
    static wxString GetSixCharWordsReportLabel()
        {
        return _(L"6+ Characters Report");
        }

    [[nodiscard]]
    static wxString GetThreeSyllableWordsLabel()
        {
        return _(L"3+ Syllables");
        }

    [[nodiscard]]
    static wxString GetThreeSyllableListWordsLabel()
        {
        return _(L"3+ Syllables List");
        }

    [[nodiscard]]
    static wxString GetThreeSyllableReportWordsLabel()
        {
        return _(L"3+ Syllables Report");
        }

    [[nodiscard]]
    static wxString GetProperNounsLabel()
        {
        return _(L"Proper Nouns");
        }

    [[nodiscard]]
    static wxString GetContractionsLabel()
        {
        return _(L"Contractions");
        }

    [[nodiscard]]
    static wxString GetOverusedWordsBySentenceLabel()
        {
        return _(L"Overused Words (x Sentence)");
        }

    [[nodiscard]]
    static wxString GetMisspellingsLabel()
        {
        return _(L"Possible Misspellings");
        }

    [[nodiscard]]
    static wxString GetArticleMismatchesLabel()
        {
        return _(L"Article Mismatches");
        }

    [[nodiscard]]
    static wxString GetPassiveLabel()
        {
        return _(L"Passive Voice");
        }

    [[nodiscard]]
    static wxString GetDaleChallLabel()
        {
        return _DT(L"Dale-Chall");
        }

    [[nodiscard]]
    static wxString GetSpacheLabel()
        {
        return _DT(L"Spache");
        }

    [[nodiscard]]
    static wxString GetHarrisJacobsonLabel()
        {
        return _DT(L"Harris-Jacobson");
        }

    // icon emojis used for goals, reports, and test results
    [[nodiscard]]
    static wxString GetCheckmarkEmoji()
        {
        return L"\u2714";
        }

    [[nodiscard]]
    static wxString GetWarningEmoji()
        {
        return L"\u26A0";
        }

    [[nodiscard]]
    static wxString GetNoteEmoji()
        {
        return L"\U0001F4DD";
        }

    [[nodiscard]]
    long GetMaxColumnWidth() const noexcept
        {
        return m_maxColumnWidth;
        }

    // list IDs
    //----------
    constexpr static int SENTENCES_CONJUNCTION_START_LIST_PAGE_ID = wxID_HIGHEST;
    constexpr static int SENTENCES_LOWERCASE_START_LIST_PAGE_ID = wxID_HIGHEST + 1;
    constexpr static int WORDY_PHRASES_LIST_PAGE_ID = wxID_HIGHEST + 2;
    constexpr static int REDUNDANT_PHRASE_LIST_PAGE_ID = wxID_HIGHEST + 3;
    constexpr static int CLICHES_LIST_PAGE_ID = wxID_HIGHEST + 4;
    constexpr static int DUPLICATES_LIST_PAGE_ID = wxID_HIGHEST + 5;
    constexpr static int INCORRECT_ARTICLE_PAGE_ID = wxID_HIGHEST + 6;
    constexpr static int PASSIVE_VOICE_PAGE_ID = wxID_HIGHEST + 7;
    constexpr static int LONG_SENTENCES_LIST_PAGE_ID = wxID_HIGHEST + 8;
    constexpr static int HARD_WORDS_LIST_PAGE_ID = wxID_HIGHEST + 9;
    constexpr static int LONG_WORDS_LIST_PAGE_ID = wxID_HIGHEST + 10;
    constexpr static int DC_WORDS_LIST_PAGE_ID = wxID_HIGHEST + 11;
    constexpr static int SPACHE_WORDS_LIST_PAGE_ID = wxID_HIGHEST + 12;
    constexpr static int ALL_WORDS_LIST_PAGE_ID = wxID_HIGHEST + 13;
    constexpr static int ALL_WORDS_CONDENSED_LIST_PAGE_ID = wxID_HIGHEST + 14;
    constexpr static int PROPER_NOUNS_LIST_PAGE_ID = wxID_HIGHEST + 15;
    constexpr static int CONTRACTIONS_LIST_PAGE_ID = wxID_HIGHEST + 16;
    constexpr static int OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID = wxID_HIGHEST + 17;
    constexpr static int HARRIS_JACOBSON_WORDS_LIST_PAGE_ID = wxID_HIGHEST + 18;
    constexpr static int MISSPELLED_WORD_LIST_PAGE_ID = wxID_HIGHEST + 19;
    constexpr static int WORDING_ERRORS_LIST_PAGE_ID = wxID_HIGHEST + 20;
    // graph IDs
    //----------
    constexpr static int WORD_BREAKDOWN_PAGE_ID = wxID_HIGHEST + 30;
    constexpr static int FRY_PAGE_ID = wxID_HIGHEST + 31;
    constexpr static int RAYGOR_PAGE_ID = wxID_HIGHEST + 32;
    constexpr static int FRASE_PAGE_ID = wxID_HIGHEST + 33;
    constexpr static int SCHWARTZ_PAGE_ID = wxID_HIGHEST + 34;
    constexpr static int LIX_GAUGE_PAGE_ID = wxID_HIGHEST + 35;
    constexpr static int LIX_GAUGE_GERMAN_PAGE_ID = wxID_HIGHEST + 36;
    constexpr static int DB2_PAGE_ID = wxID_HIGHEST + 37;
    constexpr static int FLESCH_CHART_PAGE_ID = wxID_HIGHEST + 38;
    constexpr static int CRAWFORD_GRAPH_PAGE_ID = wxID_HIGHEST + 39;
    constexpr static int DOLCH_COVERAGE_CHART_PAGE_ID = wxID_HIGHEST + 40;
    constexpr static int DOLCH_BREAKDOWN_PAGE_ID = wxID_HIGHEST + 41;
    constexpr static int GPM_FRY_PAGE_ID = wxID_HIGHEST + 42;
    constexpr static int SENTENCE_BOX_PLOT_PAGE_ID = wxID_HIGHEST + 43;
    constexpr static int SENTENCE_HISTOGRAM_PAGE_ID = wxID_HIGHEST + 44;
    constexpr static int SYLLABLE_HISTOGRAM_PAGE_ID = wxID_HIGHEST + 45;
    constexpr static int SYLLABLE_PIECHART_PAGE_ID = wxID_HIGHEST + 46;
    constexpr static int SENTENCE_HEATMAP_PAGE_ID = wxID_HIGHEST + 47;
    constexpr static int WORD_CLOUD_PAGE_ID = wxID_HIGHEST + 48;
    constexpr static int INFLESZ_GRAPH_PAGE_ID = wxID_HIGHEST + 49;
    // batch IDs
    //----------
    constexpr static int ID_SCORE_LIST_PAGE_ID = wxID_HIGHEST + 60;
    constexpr static int ID_DIFFICULT_WORDS_LIST_PAGE_ID = wxID_HIGHEST + 61;
    constexpr static int ID_DOLCH_COVERAGE_LIST_PAGE_ID = wxID_HIGHEST + 62;
    constexpr static int ID_DOLCH_WORDS_LIST_PAGE_ID = wxID_HIGHEST + 63;
    constexpr static int ID_NON_DOLCH_WORDS_LIST_PAGE_ID = wxID_HIGHEST + 64;
    // only window in the warnings section,
    // so we will reuse this ID for the sidebar section also (below)
    constexpr static int ID_WARNING_LIST_PAGE_ID = wxID_HIGHEST + 65;
    constexpr static int ID_SCORE_STATS_LIST_PAGE_ID = wxID_HIGHEST + 66;
    constexpr static int ID_AGGREGATED_DOC_SCORES_LIST_PAGE_ID = wxID_HIGHEST + 67;
    constexpr static int ID_AGGREGATED_CLOZE_SCORES_LIST_PAGE_ID = wxID_HIGHEST + 68;
    // Dolch IDs
    //----------
    constexpr static int DOLCH_WORDS_LIST_PAGE_ID = wxID_HIGHEST + 70;
    constexpr static int NON_DOLCH_WORDS_LIST_PAGE_ID = wxID_HIGHEST + 71;
    constexpr static int UNUSED_DOLCH_WORDS_LIST_PAGE_ID = wxID_HIGHEST + 72;
    constexpr static int DOLCH_WORDS_TEXT_PAGE_ID = wxID_HIGHEST + 73;
    constexpr static int NON_DOLCH_WORDS_TEXT_PAGE_ID = wxID_HIGHEST + 74;
    constexpr static int DOLCH_STATS_PAGE_ID = wxID_HIGHEST + 75;
    // stats/scores IDs
    //----------
    constexpr static int STATS_REPORT_PAGE_ID = wxID_HIGHEST + 80;
    constexpr static int STATS_LIST_PAGE_ID = wxID_HIGHEST + 81;
    constexpr static int READABILITY_SCORES_PAGE_ID = wxID_HIGHEST + 82;
    constexpr static int READABILITY_SCORES_SUMMARY_REPORT_PAGE_ID = wxID_HIGHEST + 83;
    constexpr static int READABILITY_GOALS_PAGE_ID = wxID_HIGHEST + 84;
    // highlighted words IDs
    //----------
    constexpr static int LONG_WORDS_TEXT_PAGE_ID = wxID_HIGHEST + 85;
    constexpr static int DC_WORDS_TEXT_PAGE_ID = wxID_HIGHEST + 86;
    constexpr static int SPACHE_WORDS_TEXT_PAGE_ID = wxID_HIGHEST + 87;
    constexpr static int HARRIS_JACOBSON_WORDS_TEXT_PAGE_ID = wxID_HIGHEST + 88;
    constexpr static int HARD_WORDS_TEXT_PAGE_ID = wxID_HIGHEST + 89;
    constexpr static int LONG_SENTENCES_AND_WORDINESS_TEXT_PAGE_ID = wxID_HIGHEST + 90;
    // sidebar IDs (main categories)
    //----------
    constexpr static int SIDEBAR_WORDS_BREAKDOWN_SECTION_ID = wxID_HIGHEST + 91;
    constexpr static int SIDEBAR_READABILITY_SCORES_SECTION_ID = wxID_HIGHEST + 92;
    constexpr static int SIDEBAR_GRAMMAR_SECTION_ID = wxID_HIGHEST + 93;
    constexpr static int SIDEBAR_DOLCH_SECTION_ID = wxID_HIGHEST + 94;
    constexpr static int SIDEBAR_WARNINGS_SECTION_ID = ID_WARNING_LIST_PAGE_ID;
    constexpr static int SIDEBAR_BOXPLOTS_SECTION_ID = wxID_HIGHEST + 95;
    constexpr static int SIDEBAR_HISTOGRAMS_SECTION_ID = wxID_HIGHEST + 96;
    constexpr static int SIDEBAR_STATS_SUMMARY_SECTION_ID = wxID_HIGHEST + 97;
    constexpr static int SIDEBAR_SENTENCES_BREAKDOWN_SECTION_ID = wxID_HIGHEST + 98;
    // Sidebar IDs for custom test windows.
    // Don't add any IDs between here and 1,000 above it; we will lock this whole range for that.
    constexpr static int SIDEBAR_CUSTOM_TESTS_START_ID = wxID_HIGHEST + 100;

    static IdRange& GetCustomTestSidebarIdRange() noexcept { return m_customTestSidebarIdRange; }

  protected:
    wxWindow* m_activeWindow{ nullptr };
    static IdRange m_customTestSidebarIdRange;
    Wisteria::UI::SearchPanel* m_searchCtrl{ nullptr };
    wxInfoBar* m_infoBar{ nullptr };
    wxMenuBar* m_menuBar{ nullptr };

    constexpr static int LEFT_PANE = wxID_HIGHEST + 78;
    constexpr static int SPLITTER_ID = wxID_HIGHEST + 79;

  private:
    void OnActivateView(bool activate, wxView* oldView, wxView* newView) override;

    bool OnClose(bool deleteWindow) override;

    void OnDraw([[maybe_unused]] wxDC* dc) override {}

    void OnDClickRibbonBar([[maybe_unused]] wxRibbonBarEvent& event);

    void OnClickRibbonBar(wxRibbonBarEvent& event);

    void OnCloseInfoBar([[maybe_unused]] wxCommandEvent& event);

    /// @return the messages that won't be shown until client asks from them to be shown.
    [[nodiscard]]
    const std::set<WarningMessage>& GetQueuedMessages() const noexcept
        {
        return m_queuedMessages;
        }

    /// @brief Saves a message to be shown later, when ShowQueuedMessages() is called.
    /// @param message The message to queue.
    void AddQueuedMessage(const WarningMessage& message) { m_queuedMessages.insert(message); }

    ProjectDocChildFrame* m_frame{ nullptr };
    wxRibbonBar* m_ribbon{ nullptr };
    Wisteria::UI::SideBar* m_sideBar{ nullptr };
    wxAuiToolBar* m_quickToolbar{ nullptr };
    wxSplitterWindow* m_splitter{ nullptr };
    wxBoxSizer* m_workSpaceSizer{ nullptr };
    // status flags
    bool m_presentedSuccessfully{ false };
    bool m_sidebarShown{ true };

    long m_maxColumnWidth{ 200 };

    // logged messages
    std::set<WarningMessage> m_queuedMessages;

    wxString m_lastShownMessageId;

    wxDECLARE_DYNAMIC_CLASS(BaseProjectView);
    };

#endif // BASE_PROJECT_VIEW_H
