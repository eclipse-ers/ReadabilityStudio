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

#ifndef RS_PROJECT_VIEW_H
#define RS_PROJECT_VIEW_H

#include "../Wisteria-Dataviz/src/graphs/inflesz.h"
#include "../Wisteria-Dataviz/src/math/statistics.h"
#include "../Wisteria-Dataviz/src/util/windowcontainer.h"
#include "../graphs/frygraph.h"
#include "../ui/dialogs/edit_text_dlg.h"
#include "base_project_view.h"

class WXDLLIMPEXP_FWD_WEBVIEW wxWebView;
class WXDLLIMPEXP_FWD_WEBVIEW wxWebViewEvent;

/// @brief The results window interface for a standard project.
class ProjectView final : public BaseProjectView
    {
  public:
    /// ProjectDoc has friend access to this class.
    friend class ProjectDoc;
    /// @brief Constructor.
    ProjectView();
    ProjectView(const ProjectView&) = delete;
    ProjectView& operator=(const ProjectView&) = delete;

    /** @brief Saves all the results to an HTML report.
        @param filePath The path to save the report to.
        @param graphExt The file extension to save graphs as (e.g., PNG).
        @param includeWordsBreakdown @c true to save the words breakdown section.
        @param includeSentencesBreakdown @c true to save the sentences breakdown section.
        @param includeTestScores @c true to save the test scores section.
        @param includeStatistics @c true to save the statistics section.
        @param includeGrammar @c true to save the grammar section.
        @param includeSightWords @c true to save the Dolch Sight Words section.
        @param includeLists @c true to save lists (not recommended for large source documents).
        @param includeTextReports @c true to save text reports
            (not recommended for large source documents).
        @param graphOptions Additional options for how to export the graphs.*/
    bool ExportAllToHtml(const wxFileName& filePath, wxString graphExt,
                         const bool includeWordsBreakdown, const bool includeSentencesBreakdown,
                         const bool includeTestScores, const bool includeStatistics,
                         const bool includeGrammar, const bool includeSightWords,
                         const bool includeLists, const bool includeTextReports,
                         const Wisteria::UI::ImageExportOptions& graphOptions);

    /** @brief Saves all the results to a set of files in a folder.
        @param folder The folder to save the results to.
        @param listExt The file extension to save the lists as (HTML or TXT).
        @param textExt The file extension to save the text windows as (RTF or TXT).
        @param summaryReportExt The file extension to save summary reports
            (e.g., test scores, statistics) as (HTML or PDF).
        @param graphExt The file extension to save graphs as (e.g., PNG).
        @param includeWordsBreakdown @c true to save the words breakdown section.
        @param includeSentencesBreakdown @c true to save the sentences breakdown section.
        @param includeTestScores @c true to save the test scores section.
        @param includeStatistics @c true to save the statistics section.
        @param includeGrammar @c true to save the grammar section.
        @param includeSightWords @c true to save the Dolch Sight Words section.
        @param includeLists @c true to save lists (not recommended for large source documents).
        @param includeTextReports @c true to save text reports
            (not recommended for large source documents).
        @param graphOptions Additional options for how to export the graphs.*/
    bool ExportAll(const wxString& folder, wxString listExt, wxString textExt,
                   wxString summaryReportExt, wxString graphExt, const bool includeWordsBreakdown,
                   const bool includeSentencesBreakdown, const bool includeTestScores,
                   const bool includeStatistics, const bool includeGrammar,
                   const bool includeSightWords, const bool includeLists,
                   const bool includeTextReports,
                   const Wisteria::UI::ImageExportOptions& graphOptions);

    // view classes
    [[nodiscard]]
    WindowContainer& GetReadabilityResultsView() noexcept
        {
        return m_readabilityResultsView;
        }

    [[nodiscard]]
    const WindowContainer& GetReadabilityResultsView() const noexcept
        {
        return m_readabilityResultsView;
        }

    [[nodiscard]]
    WindowContainer& GetSummaryView() noexcept
        {
        return m_summaryView;
        }

    [[nodiscard]]
    const WindowContainer& GetSummaryView() const noexcept
        {
        return m_summaryView;
        }

    [[nodiscard]]
    WindowContainer& GetWordsBreakdownView() noexcept
        {
        return m_WordsBreakdownView;
        }

    [[nodiscard]]
    const WindowContainer& GetWordsBreakdownView() const noexcept
        {
        return m_WordsBreakdownView;
        }

    [[nodiscard]]
    WindowContainer& GetSentencesBreakdownView() noexcept
        {
        return m_sentencesBreakdownView;
        }

    [[nodiscard]]
    const WindowContainer& GetSentencesBreakdownView() const noexcept
        {
        return m_sentencesBreakdownView;
        }

    [[nodiscard]]
    WindowContainer& GetGrammarView() noexcept
        {
        return m_grammarView;
        }

    [[nodiscard]]
    const WindowContainer& GetGrammarView() const noexcept
        {
        return m_grammarView;
        }

    [[nodiscard]]
    WindowContainer& GetDolchSightWordsView() noexcept
        {
        return m_sightWordView;
        }

    [[nodiscard]]
    const WindowContainer& GetDolchSightWordsView() const noexcept
        {
        return m_sightWordView;
        }

    // Shortcuts to the sub-windows
    [[nodiscard]]
    std::shared_ptr<Wisteria::UI::ListCtrlExDataProvider>& GetSummaryStatisticsListData()
        {
        return m_statsListData;
        }

    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExDataProvider>&
    GetSummaryStatisticsListData() const
        {
        return m_statsListData;
        }

    [[nodiscard]]
    ExplanationListCtrl* GetReadabilityScoresList()
        {
        return dynamic_cast<ExplanationListCtrl*>(
            GetReadabilityResultsView().FindWindowById(READABILITY_SCORES_PAGE_ID));
        }

    void OnAddTest(wxCommandEvent& event);

  private:
    /// @brief Determines whether @p window is a highlighted-text window.
    /// @details A highlighted-text window is a plain @c wxWebView identified by a hit
    ///     in the document's buffer registry (as opposed to the statistics/report
    ///     webviews). That registry hit is what distinguishes it and enables saving
    ///     and copying its paper-white RTF.
    /// @param window The window to test.
    /// @returns @c true if @p window is a highlighted-text window.
    [[nodiscard]]
    bool IsHighlightedTextWindow(const wxWindow* window) const;

    /// @brief Saves a report webview's content to @p savePathNoExt, appending the
    ///     extension for @p textExt (HTM, RTF, or PDF).
    /// @details RTF comes from the paper-white buffer that only highlighted-text windows have,
    ///     and only the sections made up of those windows are ever asked for it. Requesting it
    ///     for anything else means the configured extension is out of range (e.g., a hand-edited
    ///     options file); that is logged and saved as HTML rather than refused.
    /// @param webview The webview to save.
    /// @param savePathNoExt The full save path, without an extension.
    /// @param textExt The requested extension (e.g., L".htm", L".rtf", L".pdf").
    void SaveWebViewReport(wxWebView* webview, const wxString& savePathNoExt,
                           const wxString& textExt) const;

    // menu and ribbon commands
    void OnTextWindowColorsChange([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnTextWindowFontChange([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnGradeScale(wxCommandEvent& event);
    void OnExportAll([[maybe_unused]] wxCommandEvent& event);
    void OnExportFilteredDocument([[maybe_unused]] wxCommandEvent& event);
    void OnLongFormat([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnTestDelete([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnTestDeleteMenu([[maybe_unused]] wxCommandEvent& event);
    void OnAddToDictionary([[maybe_unused]] wxCommandEvent& event);
    void OnLaunchSourceFile([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnRealTimeUpdate([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnEditGraphOptions(wxCommandEvent& event);
    // handles more generic events that may need to be handled by the window itself
    //     (e.g., a graph printing)
    void OnMenuCommand(wxCommandEvent& event);
    void OnRibbonButtonCommand(wxRibbonButtonBarEvent& event);
    void OnItemSelected(wxCommandEvent& event);
    bool OnCreate(wxDocument* doc, long flags) final;
    void OnListDblClick(wxListEvent& event);
    void OnTestListDblClick([[maybe_unused]] wxListEvent& event);
    void OnHyperlinkClicked(wxHtmlLinkEvent& event);
    // handles link clicks in the explanation wxWebView (the 'rsexplain://' scheme)
    void OnExplanationNavigating(wxWebViewEvent& event);
    // Routes an in-app navigation link (e.g. "#fry-graph") to the matching
    // sidebar window or help topic. Returns true if the link was handled.
    [[nodiscard]]
    bool NavigateToHref(const wxString& href);
    void OnSummation([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnFind(wxFindDialogEvent& event);
    // re-themes the stats rows when the OS color scheme changes,
    // since their background is blended from system colors
    void OnReadabilityScoresSysColourChanged(wxSysColourChangedEvent& event);

    void UpdateSideBarIcons();
    void UpdateStatistics();
    void RefreshStatRowColours();
    void UpdateRibbonState() final;
    // view classes
    WindowContainer m_readabilityResultsView;
    WindowContainer m_summaryView;
    WindowContainer m_WordsBreakdownView;
    WindowContainer m_sentencesBreakdownView;
    WindowContainer m_sightWordView;
    WindowContainer m_grammarView;

    std::shared_ptr<Wisteria::UI::ListCtrlExDataProvider> m_statsListData{ nullptr };

    EditTextDlg* m_embeddedTextEditor{ nullptr };

    wxDECLARE_DYNAMIC_CLASS(ProjectView);
    };

#endif // RS_PROJECT_VIEW_H
