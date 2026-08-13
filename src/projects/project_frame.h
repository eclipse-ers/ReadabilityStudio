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

#ifndef PROJECT_FRAME_H
#define PROJECT_FRAME_H

#include <wx/aui/aui.h>
#include <wx/colordlg.h>
#include <wx/docmdi.h>
#include <wx/filename.h>
#include <wx/html/htmlcell.h>
#include <wx/html/htmlwin.h>
#include <wx/mimetype.h>
#include <wx/ribbon/bar.h>
#include <wx/ribbon/buttonbar.h>
#include <wx/splitter.h>
#include <wx/string.h>
#include <wx/wx.h>

class BaseProjectView;
class ProjectView;
class BatchProjectView;
class MainFrame;

/// @brief SDI frame for project documents.
class ProjectDocChildFrame : public wxDocChildFrame
    {
    friend BaseProjectView;
    friend ProjectView;
    friend BatchProjectView;
    friend MainFrame;

  public:
    ProjectDocChildFrame() = delete;
    ProjectDocChildFrame(const ProjectDocChildFrame&) = delete;
    ProjectDocChildFrame& operator=(const ProjectDocChildFrame&) = delete;

    ProjectDocChildFrame(
        wxDocument* doc, wxView* view, wxFrame* parent, wxWindowID id, const wxString& title,
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_FRAME_STYLE,
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-array-to-pointer-decay,hicpp-no-array-decay)
        const wxString& name = wxASCII_STR(wxFrameNameStr));

    void OnCustomTest(wxCommandEvent& event);

  private:
    // Captures menu events meant for standard or batch project views
    // and dispatches the events to their handlers. This is necessary
    // under GTK+, where it appears that wxViews cannot trap menu events
    // (not the case under MSW or macOS).
    void OnMenuCapture(wxCommandEvent& event);

    void OnEditGraphColorScheme([[maybe_unused]] wxRibbonButtonBarEvent& event);

    void OnEditGraphColor(wxCommandEvent& event);

    void OnInvalidRegionColor(wxRibbonButtonBarEvent& event);

    void OnDictionaryDropdown(wxRibbonButtonBarEvent& evt);

    void OnPrimaryAgeTestsDropdown(wxRibbonButtonBarEvent& evt);

    void OnSecondaryAgeTestsDropdown(wxRibbonButtonBarEvent& evt);

    void OnAdultTestsDropdown(wxRibbonButtonBarEvent& evt);

    void OnSecondLanguageTestsDropdown(wxRibbonButtonBarEvent& evt);

    void OnBoxSelectCommonImage([[maybe_unused]] wxCommandEvent& event);

    void OnHistoBarSelectStippleShape([[maybe_unused]] wxCommandEvent& event);

    void OnShowcaseKeyItems([[maybe_unused]] wxRibbonButtonBarEvent& event);

    void OnBarStyleSelected(wxCommandEvent& event);

    void OnHistoBarStyleSelected(wxCommandEvent& event);

    void OnBarSelectCommonImage([[maybe_unused]] wxCommandEvent& event);

    void OnBoxStyleSelected(wxCommandEvent& event);

    void OnEditPlotBackgroundImageEffect(wxCommandEvent& event);

    void OnBarSelectStippleBrush([[maybe_unused]] wxCommandEvent& event);

    void OnEditPlotBackgroundImageFit(wxCommandEvent& event);

    void OnBoxSelectStippleBrush([[maybe_unused]] wxCommandEvent& event);

    void OnBarSelectStippleShape([[maybe_unused]] wxCommandEvent& event);

    void OnGraphColorFade([[maybe_unused]] wxCommandEvent& event);

    void OnBoxSelectStippleShape([[maybe_unused]] wxCommandEvent& event);

    void OnHistoBarSelectStippleBrush([[maybe_unused]] wxCommandEvent& event);

    void OnHistoBarSelectCommonImage([[maybe_unused]] wxCommandEvent& event);

    void OnEditGraphRaygorStyleDropdown(wxRibbonButtonBarEvent& event)
        {
        event.PopupMenu(&m_raygorStyleMenu);
        }

    void OnTextExclusionDropdown(wxRibbonButtonBarEvent& event)
        {
        event.PopupMenu(&m_textExclusionMenu);
        }

    void OnDocumentRefreshMenu([[maybe_unused]] wxCommandEvent& event)
        {
        wxRibbonButtonBarEvent cmd;
        OnDocumentRefresh(cmd);
        }

    void OnOpenDropdown(wxRibbonButtonBarEvent& evt) { evt.PopupMenu(&m_fileOpenMenu); }

    void OnSaveDropdown(wxRibbonButtonBarEvent& evt);

    void OnExportDropdown(wxRibbonButtonBarEvent& evt) { evt.PopupMenu(&m_exportMenu); }

    void OnLongSentencesDropdown(wxRibbonButtonBarEvent& event)
        {
        event.PopupMenu(&m_longSentencesMenu);
        }

    void OnLineEndsDropdown(wxRibbonButtonBarEvent& event) { event.PopupMenu(&m_lineEndsMenu); }

    void OnNumeralSyllabicationDropdown(wxRibbonButtonBarEvent& event)
        {
        event.PopupMenu(&m_numeralSyllabicationMenu);
        }

    void OnExclusionTagsDropdown(wxRibbonButtonBarEvent& event)
        {
        event.PopupMenu(&m_exclusionTagsMenu);
        }

    void OnHistoBarsLabelsDropdown(wxRibbonButtonBarEvent& event)
        {
        event.PopupMenu(&m_histoBarLabelsMenu);
        }

    void OnZoomButtonDropdown(wxRibbonButtonBarEvent& event) { event.PopupMenu(&m_zoomMenu); }

    void OnEditGraphBackgroundDropdown(wxRibbonButtonBarEvent& event)
        {
        event.PopupMenu(&m_graphBackgroundMenu);
        }

    void OnHistoBarStyleDropdown(wxRibbonButtonBarEvent& event)
        {
        event.PopupMenu(&m_histoBarStyleMenu);
        }

    void OnBarOrientationDropdown(wxRibbonButtonBarEvent& event)
        {
        event.PopupMenu(&m_barOrientationMenu);
        }

    void OnBarStyleDropdown(wxRibbonButtonBarEvent& event) { event.PopupMenu(&m_barStyleMenu); }

    void OnBoxStyleDropdown(wxRibbonButtonBarEvent& event) { event.PopupMenu(&m_boxStyleMenu); }

    void OnEditGraphFontsDropdown(wxRibbonButtonBarEvent& event)
        {
        event.PopupMenu(&m_graphFontsMenu);
        }

    void OnCopyDropdown(wxRibbonButtonBarEvent& evt) { evt.PopupMenu(&m_copyMenu); }

    void OnWordListDropdown(wxRibbonButtonBarEvent& evt) { evt.PopupMenu(&m_wordListMenu); }

    void OnBlankGraphDropdown(wxRibbonButtonBarEvent& evt) { evt.PopupMenu(&m_blankGraphMenu); }

    void OnCustomTestsDropdown(wxRibbonButtonBarEvent& evt) { evt.PopupMenu(&m_customTestsMenu); }

    void OnTestBundlesDropdown(wxRibbonButtonBarEvent& evt) { evt.PopupMenu(&m_testsBundleMenu); }

    void OnPrintDropdown(wxRibbonButtonBarEvent& evt) { evt.PopupMenu(&m_printMenu); }

    void OnExampleDropdown(wxRibbonButtonBarEvent& evt) { evt.PopupMenu(&m_exampleMenu); }

    void OnGraphSortDropdown(wxRibbonButtonBarEvent& evt) { evt.PopupMenu(&m_graphSortMenu); }

    void OnGradeScaleDropdown(wxRibbonButtonBarEvent& evt) { evt.PopupMenu(&m_gradeScaleMenu); }

    void OnPlainLanguageGuideListDropdown(wxRibbonButtonBarEvent& evt)
        {
        evt.PopupMenu(&m_plainLanguageGuideListMenu);
        }

    void OnReportThemeDropdown(wxRibbonButtonBarEvent& evt);

    void OnBarLabelsButton([[maybe_unused]] wxRibbonButtonBarEvent& event);

    void OnDropShadow([[maybe_unused]] wxRibbonButtonBarEvent& event);

    void OnGraphLogo([[maybe_unused]] wxRibbonButtonBarEvent& event);

    void OnEnglishLabels(wxRibbonButtonBarEvent& event);

    void OnProjectSettings([[maybe_unused]] wxRibbonButtonBarEvent& event);

    void OnToolsOptions([[maybe_unused]] wxRibbonButtonBarEvent& event);

    void OnEditDictionaryButton([[maybe_unused]] wxRibbonButtonBarEvent& event);

    void OnIgnoreCopyrights([[maybe_unused]] wxRibbonButtonBarEvent& event);

    void OnIgnoreBlankLines([[maybe_unused]] wxRibbonButtonBarEvent& event);

    void OnIgnoreIndenting([[maybe_unused]] wxRibbonButtonBarEvent& event);

    void OnStrictCapitalization([[maybe_unused]] wxRibbonButtonBarEvent& event);

    void OnExcludeWordsList([[maybe_unused]] wxRibbonButtonBarEvent& event);

    void OnIgnoreProperNouns([[maybe_unused]] wxRibbonButtonBarEvent& event);

    void OnIgnoreCitations([[maybe_unused]] wxRibbonButtonBarEvent& event);

    void OnFleschConnectLinesButton([[maybe_unused]] wxRibbonButtonBarEvent& event);

    void OnIncompleteThreshold([[maybe_unused]] wxRibbonButtonBarEvent& event);

    void OnIgnoreFileAddresses([[maybe_unused]] wxRibbonButtonBarEvent& event);

    void OnIgnoreNumerals([[maybe_unused]] wxRibbonButtonBarEvent& event);

    void OnAggressivelyExclude([[maybe_unused]] wxRibbonButtonBarEvent& event);

    void OnEditStatsReportButton([[maybe_unused]] wxRibbonButtonBarEvent& event);

    void OnPlainLanguageGuideListSelected(wxCommandEvent& event);

    void OnReportThemeSelected(wxCommandEvent& event);

    void OnBoxPlotShowAllPointsButton([[maybe_unused]] wxRibbonButtonBarEvent& event);

    void OnBoxPlotShowLabelsButton([[maybe_unused]] wxRibbonButtonBarEvent& event);

    void OnPrintWatermark([[maybe_unused]] wxCommandEvent& event);

    void OnAddCustomTestBundle(wxCommandEvent& event);

    void OnRaygorStyleSelected([[maybe_unused]] wxCommandEvent& event);

    void OnFindMenu([[maybe_unused]] wxCommandEvent& event);

    void OnFindNext([[maybe_unused]] wxCommandEvent& event);

    void OnLineEndOptions(wxCommandEvent& event);

    void OnBarOrientationSelected(wxCommandEvent& event);

    void OnAddCustomTest(wxCommandEvent& event);

    void OnRemoveCustomTestBundle(wxCommandEvent& event);

    void OnEditEnglishDictionary([[maybe_unused]] wxCommandEvent& event);

    void OnHistoBarLabelSelected(wxCommandEvent& event);

    void OnEditDictionaryProjectSettings([[maybe_unused]] wxCommandEvent& event);

    void OnEditCustomTest(wxCommandEvent& event);

    void OnRemoveCustomTest(wxCommandEvent& event);

    void OnEditGraphOpacity(wxCommandEvent& event);

    void OnEditPlotBackgroundImage([[maybe_unused]] wxCommandEvent& event);

    void OnMergePlotBackgroundImages([[maybe_unused]] wxCommandEvent& event);

    void OnLongSentencesOptions([[maybe_unused]] wxCommandEvent& event);

    void OnEditDictionary([[maybe_unused]] wxCommandEvent& event);

    void OnDocumentRefresh([[maybe_unused]] wxRibbonButtonBarEvent& event);

    void OnEditGraphFont(wxCommandEvent& event);

    void OnTestBundle(wxCommandEvent& event);

    void OnNumeralSyllabicationOptions([[maybe_unused]] wxCommandEvent& event);

    void OnEditCustomTestBundle(wxCommandEvent& event);

    void OnTextExclusionOptions([[maybe_unused]] wxCommandEvent& event);

    void OnExclusionTagsOptions([[maybe_unused]] wxCommandEvent& event);

    void OnCustomTestBundle(wxCommandEvent& event);

    wxMenu* m_testsBundleRegularMenu{ nullptr };
    wxMenu* m_customTestsRegularMenu{ nullptr };
    wxMenu m_fileOpenMenu;
    wxMenu m_saveMenu;
    wxMenu m_exportMenu;
    wxMenu m_longSentencesMenu;
    wxMenu m_histoBarLabelsMenu;
    wxMenu m_numeralSyllabicationMenu;
    wxMenu m_textExclusionMenu;
    wxMenu m_exclusionTagsMenu;
    wxMenu m_lineEndsMenu;
    wxMenu m_zoomMenu;
    wxMenu m_graphBackgroundMenu;
    wxMenu m_histoBarStyleMenu;
    wxMenu m_barOrientationMenu;
    wxMenu m_barStyleMenu;
    wxMenu m_boxStyleMenu;
    wxMenu m_graphFontsMenu;
    wxMenu m_copyMenu;
    wxMenu m_wordListMenu;
    wxMenu m_blankGraphMenu;
    wxMenu m_customTestsMenu;
    wxMenu m_testsBundleMenu;
    wxMenu m_printMenu;
    wxMenu m_graphSortMenu;
    wxMenu m_exampleMenu;
    wxMenu m_gradeScaleMenu;
    wxMenu m_primaryAgeTestsMenu;
    wxMenu m_secondaryAgeTestsMenu;
    wxMenu m_adultTestsMenu;
    wxMenu m_secondLanguageTestsMenu;
    wxMenu m_raygorStyleMenu;
    wxMenu m_plainLanguageGuideListMenu;
    wxMenu m_reportThemeMenu;
    };

#endif // PROJECT_FRAME_H
