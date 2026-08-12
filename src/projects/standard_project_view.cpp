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

#include "standard_project_view.h"
#include "../Wisteria-Dataviz/src/graphs/danielsonbryan2plot.h"
#include "../Wisteria-Dataviz/src/graphs/heatmap.h"
#include "../Wisteria-Dataviz/src/graphs/lixgauge.h"
#include "../Wisteria-Dataviz/src/graphs/lixgaugegerman.h"
#include "../Wisteria-Dataviz/src/graphs/piechart.h"
#include "../Wisteria-Dataviz/src/graphs/wordcloud.h"
#include "../Wisteria-Dataviz/src/import/html_encode.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/gridexportdlg.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/radioboxdlg.h"
#include "../Wisteria-Dataviz/src/util/clipboard_rtf.h"
#include "../app/readability_app.h"
#include "../graphs/schwartzgraph.h"
#include "../results-format/project_report_format.h"
#include "../ui/dialogs/export_all_dlg.h"
#include "../ui/dialogs/filtered_text_export_options_dlg.h"
#include "../ui/dialogs/filtered_text_preview_dlg.h"
#include "../ui/dialogs/tools_options_dlg.h"
#include "project_navigation_links.h"
#include "standard_project_doc.h"
#include "wx/richmsgdlg.h"
#include <wx/base64.h>
#include <wx/clipbrd.h>
#include <wx/mstream.h>
#include <wx/webview.h>

wxDECLARE_APP(ReadabilityApp);

wxIMPLEMENT_DYNAMIC_CLASS(ProjectView, BaseProjectView)

    //------------------------------------------------------
    ProjectView::ProjectView()
    : m_statsListData(new Wisteria::UI::ListCtrlExDataProvider)
    {
    Bind(wxEVT_COMMAND_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this,
         SIDEBAR_CUSTOM_TESTS_START_ID, SIDEBAR_CUSTOM_TESTS_START_ID + 1000);

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRealTimeUpdate, this,
         XRCID("ID_REALTIME_UPDATE"));

    Bind(Wisteria::UI::wxEVT_SIDEBAR_CLICK, &ProjectView::OnItemSelected, this,
         BaseProjectView::LEFT_PANE);

    Bind(wxEVT_WISTERIA_CANVAS_DCLICK, &ProjectView::OnEditGraphOptions, this);

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnTestDelete, this, XRCID("ID_REMOVE_TEST"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnLongFormat, this, XRCID("ID_LONG_FORMAT"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnTextWindowColorsChange, this,
         XRCID("ID_TEXT_WINDOW_COLORS"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnLaunchSourceFile, this,
         XRCID("ID_LAUNCH_SOURCE_FILE"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnSummation, this, XRCID("ID_SUMMATION"));

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this, wxID_SELECTALL);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this, wxID_NEW);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this, wxID_OPEN);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this, wxID_SAVE);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this, wxID_COPY);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this, wxID_PREVIEW);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this, wxID_PRINT);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this, wxID_ZOOM_IN);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this, wxID_ZOOM_OUT);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this, wxID_ZOOM_FIT);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this,
         XRCID("ID_EXCLUDE_SELECTED"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this,
         XRCID("ID_SORT_DESCENDING"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this,
         XRCID("ID_SORT_ASCENDING"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this,
         XRCID("ID_LIST_SORT"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this,
         XRCID("ID_VIEW_ITEM"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this,
         XRCID("ID_COPY_FIRST_COLUMN"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this,
         XRCID("ID_COPY_WITH_COLUMN_HEADERS"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this,
         XRCID("ID_COPY_ALL"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &ProjectView::OnRibbonButtonCommand, this,
         XRCID("ID_SAVE_ITEM"));

    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this, HARD_WORDS_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this, LONG_WORDS_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this, DC_WORDS_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this,
         HARRIS_JACOBSON_WORDS_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this, SPACHE_WORDS_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this, ALL_WORDS_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this,
         ALL_WORDS_CONDENSED_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this, PROPER_NOUNS_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this,
         OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this, DOLCH_WORDS_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this,
         NON_DOLCH_WORDS_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this,
         LONG_SENTENCES_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this,
         SENTENCES_CONJUNCTION_START_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this,
         SENTENCES_LOWERCASE_START_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this, DUPLICATES_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this, INCORRECT_ARTICLE_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this, PASSIVE_VOICE_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this,
         MISSPELLED_WORD_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this, CLICHES_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this, WORDY_PHRASES_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this,
         REDUNDANT_PHRASE_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnListDblClick, this,
         WORDING_ERRORS_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_ACTIVATED, &ProjectView::OnTestListDblClick, this,
         READABILITY_SCORES_PAGE_ID);

    Bind(wxEVT_FIND, &ProjectView::OnFind, this);
    Bind(wxEVT_FIND_NEXT, &ProjectView::OnFind, this);
    Bind(wxEVT_FIND_CLOSE, &ProjectView::OnFind, this);

    Bind(wxEVT_HTML_LINK_CLICKED, &ProjectView::OnHyperlinkClicked, this);
    Bind(wxEVT_WEBVIEW_NAVIGATING, &ProjectView::OnExplanationNavigating, this);

    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, XRCID("ID_PRINT"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_K12_US"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_K12_NEWFOUNDLAND"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_K12_BC"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_K12_NEW_BRUNSWICK"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_K12_NOVA_SCOTIA"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_K12_ONTARIO"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_K12_SASKATCHEWAN"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_K12_PE"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_K12_MANITOBA"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_K12_NT"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_K12_ALBERTA"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_K12_NUNAVUT"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_QUEBEC"));
    Bind(wxEVT_MENU, &ProjectView::OnGradeScale, this, XRCID("ID_ENGLAND"));

    Bind(wxEVT_MENU, &ProjectView::OnAddToDictionary, this, XRCID("ID_ADD_ITEM_TO_DICTIONARY"));
    Bind(wxEVT_MENU, &ProjectView::OnTestDeleteMenu, this, XRCID("ID_REMOVE_TEST"));
    Bind(wxEVT_MENU, &ProjectView::OnExportFilteredDocument, this,
         XRCID("ID_EXPORT_FILTERED_DOCUMENT"));
    Bind(wxEVT_MENU, &ProjectView::OnExportAll, this, XRCID("ID_EXPORT_ALL"));
    // not actually a test (it doesn't have an ID), but we'll add it in this function
    Bind(wxEVT_MENU, &ProjectView::OnAddTest, this, XRCID("ID_DOLCH"));

    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, wxID_SELECTALL);
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, wxID_COPY);
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, wxID_PREVIEW);
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, wxID_PRINT);
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, wxID_ZOOM_IN);
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, wxID_ZOOM_OUT);
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, wxID_ZOOM_FIT);
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, XRCID("ID_SAVE_ITEM"));
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, XRCID("ID_COPY_ALL"));
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, XRCID("ID_COPY_WITH_COLUMN_HEADERS"));
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, XRCID("ID_COPY_FIRST_COLUMN"));
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, XRCID("ID_VIEW_ITEM"));
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, XRCID("ID_LIST_SORT"));
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, XRCID("ID_SORT_ASCENDING"));
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, XRCID("ID_SORT_DESCENDING"));
    Bind(wxEVT_MENU, &ProjectView::OnMenuCommand, this, XRCID("ID_EXCLUDE_SELECTED"));

    Bind(
        wxEVT_RIBBONBUTTONBAR_CLICKED,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            auto* projDoc = dynamic_cast<ProjectDoc*>(GetDocument());
            if (projDoc != nullptr)
                {
                projDoc->Save();
                }
        },
        XRCID("ID_SAVE_PROJECT"));
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            auto* projDoc = dynamic_cast<ProjectDoc*>(GetDocument());
            if (projDoc != nullptr)
                {
                projDoc->Save();
                }
        },
        // don't use wxID_SAVE for a hybrid ribbon button because it becomes disabled
        // when the document isn't dirty, and then you can't access the export menu
        XRCID("ID_SAVE_PROJECT"));
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            auto* projDoc = dynamic_cast<ProjectDoc*>(GetDocument());
            if (projDoc != nullptr)
                {
                projDoc->SaveAs();
                }
        },
        XRCID("ID_SAVE_PROJECT_AS"));
    }

//------------------------------------------------------
void ProjectView::OnRealTimeUpdate([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto* projDoc = dynamic_cast<ProjectDoc*>(GetDocument());
    if (projDoc != nullptr)
        {
        if (projDoc->IsRealTimeUpdating())
            {
            projDoc->UseRealTimeUpdate(false);
            projDoc->StopRealtimeUpdate();
            }
        else
            {
            projDoc->UseRealTimeUpdate(true);
            projDoc->RestartRealtimeUpdate();
            }
        }
    }

//------------------------------------------------------
void ProjectView::OnSummation([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    const auto* doc = dynamic_cast<const ProjectDoc*>(GetDocument());
    Wisteria::UI::ListCtrlItemViewDlg viewDlg;
    if (GetActiveProjectWindow()->GetId() == MISSPELLED_WORD_LIST_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total Possible Misspellings"),
            wxNumberFormatter::ToString(doc->GetMisspelledWordData()->GetColumnSum(1), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == PASSIVE_VOICE_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total Passive Phrases"),
            wxNumberFormatter::ToString(doc->GetPassiveVoiceData()->GetColumnSum(1), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == HARD_WORDS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total 3+ Syllable Words"),
            wxNumberFormatter::ToString(doc->Get3SyllablePlusData()->GetColumnSum(2), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == LONG_WORDS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total 6+ Character Words"),
            wxNumberFormatter::ToString(doc->Get6CharacterPlusData()->GetColumnSum(2), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == DC_WORDS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total Dale-Chall Unfamiliar Words"),
            wxNumberFormatter::ToString(doc->GetDaleChallHardWordData()->GetColumnSum(1), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == SPACHE_WORDS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total Spache Unfamiliar Words"),
            wxNumberFormatter::ToString(doc->GetSpacheHardWordData()->GetColumnSum(1), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == HARRIS_JACOBSON_WORDS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(_(L"Total Harris-Jacobson Unfamiliar Words"),
                         wxNumberFormatter::ToString(
                             doc->GetHarrisJacobsonHardWordDataData()->GetColumnSum(1), 0,
                             wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                 wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == ALL_WORDS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total Words"),
            wxNumberFormatter::ToString(doc->GetAllWordsBaseData()->GetColumnSum(1), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == ALL_WORDS_CONDENSED_LIST_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total Words"),
            wxNumberFormatter::ToString(doc->GetKeyWordsBaseData()->GetColumnSum(1), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == PROPER_NOUNS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total Proper Nouns"),
            wxNumberFormatter::ToString(doc->GetProperNounsData()->GetColumnSum(1), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == DOLCH_WORDS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total Dolch Words"),
            wxNumberFormatter::ToString(doc->GetDolchWordData()->GetColumnSum(1), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else if (GetActiveProjectWindow()->GetId() == NON_DOLCH_WORDS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total Non-Dolch Words"),
            wxNumberFormatter::ToString(doc->GetNonDolchWordData()->GetColumnSum(1), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));
        }
    else
        {
        wxString customTestName;
        for (auto& customWordTest : BaseProject::m_custom_word_tests)
            {
            if (customWordTest.get_interface_id() == GetActiveProjectWindow()->GetId())
                {
                customTestName = customWordTest.get_name().c_str();
                break;
                }
            }
        if (doc->HasCustomTest(customTestName))
            {
            viewDlg.AddValue(
                wxString::Format(_(L"Total %s Unfamiliar Words"), customTestName),
                wxNumberFormatter::ToString(
                    doc->GetCustomTest(customTestName)->GetListViewData()->GetColumnSum(1), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes |
                        wxNumberFormatter::Style::Style_WithThousandsSep));
            }
        }
    viewDlg.Create(GetDocFrame(), wxID_ANY, _(L"Column Summations"));
    viewDlg.ShowModal();
    }

//------------------------------------------------------
void ProjectView::OnExportAll([[maybe_unused]] wxCommandEvent& event)
    {
    auto* doc = dynamic_cast<ProjectDoc*>(GetDocument());
    if (!doc->IsSafeToUpdate())
        {
        return;
        }
    wxArrayString choices, descriptions;
    choices.Add(_(L"Single report"));
    descriptions.Add(
        _(L"Saves the results into a single <span style='font-weight: bold;'>HTML</span> report."));
    // TRANSLATORS: Different (multiple) files. "Separate" is not a verb here.
    choices.Add(_(L"Separate files"));
    descriptions.Add(_(L"Saves each result window to a separate file."));
    Wisteria::UI::RadioBoxDlg exportTypesDlg(GetDocFrame(), _(L"Select How to Export"), wxString{},
                                             _(L"Export methods:"), _(L"Export All"), choices,
                                             descriptions);
    if (exportTypesDlg.ShowModal() != wxID_OK)
        {
        return;
        }

    ExportAllDlg dlg(GetDocFrame(), doc, (exportTypesDlg.GetSelection() == 0));
    if ((m_activeWindow != nullptr) && m_activeWindow->GetClientSize().IsFullySpecified())
        {
        dlg.GetImageExportOptions().m_imageSize = m_activeWindow->GetClientSize();
        }
    dlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(), L"online/publishing.html");
    if (dlg.ShowModal() != wxID_OK || dlg.GetFolderPath().empty())
        {
        return;
        }

    if (exportTypesDlg.GetSelection() == 0)
        {
        ExportAllToHtml(dlg.GetFilePath(), dlg.GetExportGraphExt(), dlg.IsExportingHardWordLists(),
                        dlg.IsExportingSentencesBreakdown(), dlg.IsExportingTestResults(),
                        dlg.IsExportingStatistics(), dlg.IsExportingGrammar(),
                        dlg.IsExportingPlainLanguageGuide(), dlg.IsExportingSightWords(),
                        dlg.IsExportingLists(), dlg.IsExportingTextReports(),
                        dlg.GetImageExportOptions());
        }
    else
        {
        ExportAll(dlg.GetFolderPath(), dlg.GetExportListExt(), dlg.GetExportTextViewExt(),
                  dlg.GetExportSummaryReportExt(), dlg.GetExportGraphExt(),
                  dlg.IsExportingHardWordLists(), dlg.IsExportingSentencesBreakdown(),
                  dlg.IsExportingTestResults(), dlg.IsExportingStatistics(),
                  dlg.IsExportingGrammar(), dlg.IsExportingPlainLanguageGuide(),
                  dlg.IsExportingSightWords(), dlg.IsExportingLists(), dlg.IsExportingTextReports(),
                  dlg.GetImageExportOptions());
        }
    doc->SetExportFile(dlg.GetFilePath());
    doc->SetExportFolder(dlg.GetFolderPath());
    ProjectDoc::SetExportListExt(dlg.GetExportListExt());
    ProjectDoc::SetExportTextViewExt(dlg.GetExportTextViewExt());
    ProjectDoc::SetExportSummaryReportExt(dlg.GetExportSummaryReportExt());
    ProjectDoc::SetExportGraphExt(dlg.GetExportGraphExt());
    ProjectDoc::ExportHardWordLists(dlg.IsExportingHardWordLists());
    ProjectDoc::ExportSentencesBreakdown(dlg.IsExportingSentencesBreakdown());
    ProjectDoc::ExportTestResults(dlg.IsExportingTestResults());
    ProjectDoc::ExportStatistics(dlg.IsExportingStatistics());
    ProjectDoc::ExportWordiness(dlg.IsExportingGrammar());
    ProjectDoc::ExportPlainLanguageGuide(dlg.IsExportingPlainLanguageGuide());
    ProjectDoc::ExportSightWords(dlg.IsExportingSightWords());
    ProjectDoc::ExportLists(dlg.IsExportingLists());
    ProjectDoc::ExportTextReports(dlg.IsExportingTextReports());
    ProjectDoc::GetImageExportOptions() = dlg.GetImageExportOptions();
    // export folder paths may have changed
    doc->SetModifiedFlag();
    }

//------------------------------------------------------
void ProjectView::OnExportFilteredDocument([[maybe_unused]] wxCommandEvent& event)
    {
    const auto* doc = dynamic_cast<const BaseProjectDoc*>(GetDocument());
    wxFileDialog fileDialog(GetDocFrame(), _(L"Export Filtered Document"), wxString{},
                            doc->GetTitle(), _(L"Text Files (*.txt)|*.txt"),
                            wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (fileDialog.ShowModal() != wxID_OK)
        {
        return;
        }

    FilteredTextExportOptionsDlg optDlg(GetDocFrame());
    optDlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(), L"online/publishing.html");
    if (optDlg.ShowModal() != wxID_OK)
        {
        return;
        }

    std::wstring validDocText;
    doc->FormatFilteredText(validDocText, optDlg.IsReplacingCharacters(),
                            optDlg.IsRemovingEllipses(), optDlg.IsRemovingBullets(),
                            optDlg.IsRemovingFilePaths(), optDlg.IsStrippingAbbreviations(),
                            optDlg.IsNarrowingFullWidthCharacters());

    FilteredTextPreviewDlg dlg(GetDocFrame(), doc->GetInvalidSentenceMethod(),
                               doc->IsExcludingTrailingCopyrightNoticeParagraphs(),
                               doc->IsExcludingTrailingCitations(), optDlg.IsReplacingCharacters(),
                               optDlg.IsRemovingEllipses(), optDlg.IsRemovingBullets(),
                               optDlg.IsRemovingFilePaths(), optDlg.IsStrippingAbbreviations());
    dlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(), L"online/analysis-notes.html");
    dlg.SetFilteredValue(validDocText);
    if (dlg.ShowModal() == wxID_OK)
        {
        wxFileName(fileDialog.GetPath()).SetPermissions(wxS_DEFAULT);
        wxFile filteredFile(fileDialog.GetPath(), wxFile::write);
        if (!filteredFile.Write(validDocText))
            {
            wxMessageBox(_(L"Unable to write to output file."), _(L"Error"),
                         wxOK | wxICON_EXCLAMATION);
            }
        }
    }

//------------------------------------------------------
void ProjectView::OnEditGraphOptions(wxCommandEvent& event)
    {
    ToolsOptionsDlg optionsDlg(GetDocFrame(), dynamic_cast<ProjectDoc*>(GetDocument()),
                               ToolsOptionsDlg::GraphsSection);
    switch (event.GetId())
        {
    case FRY_PAGE_ID:
        [[fallthrough]];
    case RAYGOR_PAGE_ID:
        [[fallthrough]];
    case GPM_FRY_PAGE_ID:
        [[fallthrough]];
    case FRASE_PAGE_ID:
        [[fallthrough]];
    case FLESCH_CHART_PAGE_ID:
        [[fallthrough]];
    case INFLESZ_GRAPH_PAGE_ID:
        [[fallthrough]];
    case CRAWFORD_GRAPH_PAGE_ID:
        [[fallthrough]];
    case SCHWARTZ_PAGE_ID:
        [[fallthrough]];
    case LIX_GAUGE_PAGE_ID:
        [[fallthrough]];
    case LIX_GAUGE_GERMAN_PAGE_ID:
        [[fallthrough]];
    case DB2_PAGE_ID:
        optionsDlg.SelectPage(ToolsOptionsDlg::GRAPH_READABILITY_GRAPHS_PAGE);
        break;
    case WORD_BREAKDOWN_PAGE_ID:
        [[fallthrough]];
    case DOLCH_COVERAGE_CHART_PAGE_ID:
        [[fallthrough]];
    case DOLCH_BREAKDOWN_PAGE_ID:
        optionsDlg.SelectPage(ToolsOptionsDlg::GRAPH_BAR_CHART_PAGE);
        break;
    case SENTENCE_BOX_PLOT_PAGE_ID:
        optionsDlg.SelectPage(ToolsOptionsDlg::GRAPH_BOX_PLOT_PAGE);
        break;
    case SENTENCE_HISTOGRAM_PAGE_ID:
        [[fallthrough]];
    case SYLLABLE_HISTOGRAM_PAGE_ID:
        optionsDlg.SelectPage(ToolsOptionsDlg::GRAPH_HISTOGRAM_PAGE);
        break;
    default:
        optionsDlg.SelectPage(ToolsOptionsDlg::GRAPH_GENERAL_PAGE);
        };
    // don't refresh the whole project, just update the graphs
    if (optionsDlg.ShowModal() == wxID_OK)
        {
        dynamic_cast<ProjectDoc*>(GetDocument())->RefreshGraphs();
        }
    }

//------------------------------------------------------
bool ProjectView::NavigateToHref(const wxString& href)
    {
    // non-anchor links are help topics or URLs; hand them to the help system
    if (href.empty() || href.GetChar(0) != L'#')
        {
        if (!href.empty())
            {
            wxGetApp().GetMainFrame()->DisplayHelp(href);
            }
        return true;
        }

    // compare the bare anchor (without '#') against the centralized link names
    const std::wstring hrefStr{ href.wc_str() };
    if (hrefStr.size() < 2)
        {
        return false;
        }
    const std::wstring_view anchor{ std::wstring_view{ hrefStr }.substr(1) };

    if (anchor == NavLink::UnusedDolchWords)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(UNUSED_DOLCH_WORDS_LIST_PAGE_ID));
        }
    else if (anchor == NavLink::Dolch)
        {
        GetSideBar()->SelectFolder(GetSideBar()->FindFolder(SIDEBAR_DOLCH_SECTION_ID));
        }
    else if (anchor == NavLink::SelectStatistics)
        {
        auto* theProject = dynamic_cast<BaseProjectDoc*>(GetDocument());
        ToolsOptionsDlg optionsDlg(GetDocFrame(), theProject, ToolsOptionsDlg::Statistics);
        optionsDlg.SelectPage(ToolsOptionsDlg::ANALYSIS_STATISTICS_PAGE);
        if (optionsDlg.ShowModal() == wxID_OK)
            {
            theProject->RefreshStatisticsReports();
            }
        }
    else if (anchor == NavLink::FryGraph)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(FRY_PAGE_ID));
        }
    else if (anchor == NavLink::FleschChart)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(FLESCH_CHART_PAGE_ID));
        }
    else if (anchor == NavLink::DanielsonBryan2)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(DB2_PAGE_ID));
        }
    else if (anchor == NavLink::LixGauge)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(LIX_GAUGE_PAGE_ID));
        }
    else if (anchor == NavLink::GermanLixGauge)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(LIX_GAUGE_GERMAN_PAGE_ID));
        }
    else if (anchor == NavLink::CrawfordGraph)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(CRAWFORD_GRAPH_PAGE_ID));
        }
    else if (anchor == NavLink::Inflesz)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(INFLESZ_GRAPH_PAGE_ID));
        }
    else if (anchor == NavLink::GilliamPenaMountainGraph)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(GPM_FRY_PAGE_ID));
        }
    else if (anchor == NavLink::FraseGraph)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(FRASE_PAGE_ID));
        }
    else if (anchor == NavLink::Schwartz)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(SCHWARTZ_PAGE_ID));
        }
    else if (anchor == NavLink::RaygorGraph)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(RAYGOR_PAGE_ID));
        }
    else if (anchor == NavLink::DifficultSentences)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(LONG_SENTENCES_LIST_PAGE_ID));
        }
    else if (anchor == NavLink::SentenceStartingWithConjunctions)
        {
        GetSideBar()->SelectSubItem(
            GetSideBar()->FindSubItem(SENTENCES_CONJUNCTION_START_LIST_PAGE_ID));
        }
    else if (anchor == NavLink::SentenceStartingWithLowercase)
        {
        GetSideBar()->SelectSubItem(
            GetSideBar()->FindSubItem(SENTENCES_LOWERCASE_START_LIST_PAGE_ID));
        }
    else if (anchor == NavLink::Misspellings)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(MISSPELLED_WORD_LIST_PAGE_ID));
        }
    else if (anchor == NavLink::RepeatedWords)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(DUPLICATES_LIST_PAGE_ID));
        }
    else if (anchor == NavLink::MismatchedArticles)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(INCORRECT_ARTICLE_PAGE_ID));
        }
    else if (anchor == NavLink::PassiveVoice)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(PASSIVE_VOICE_PAGE_ID));
        }
    else if (anchor == NavLink::WordyPhrases)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(WORDY_PHRASES_LIST_PAGE_ID));
        }
    else if (anchor == NavLink::RedundantPhrases)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(REDUNDANT_PHRASE_LIST_PAGE_ID));
        }
    else if (anchor == NavLink::OverusedWordsBySentence)
        {
        GetSideBar()->SelectSubItem(
            GetSideBar()->FindSubItem(OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID));
        }
    else if (anchor == NavLink::WordingErrors)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(WORDING_ERRORS_LIST_PAGE_ID));
        }
    else if (anchor == NavLink::Cliches)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(CLICHES_LIST_PAGE_ID));
        }
    else if (anchor == NavLink::LongWords)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(LONG_WORDS_LIST_PAGE_ID));
        }
    else if (anchor == NavLink::HardWords)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(HARD_WORDS_LIST_PAGE_ID));
        }
    else if (anchor == NavLink::DaleChallWords)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(DC_WORDS_LIST_PAGE_ID));
        }
    else if (anchor == NavLink::SpacheWords)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(SPACHE_WORDS_LIST_PAGE_ID));
        }
    else if (anchor == NavLink::HarrisJacobsonWords)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(HARRIS_JACOBSON_WORDS_LIST_PAGE_ID));
        }
    else if (anchor == NavLink::FogHelp)
        {
        wxGetApp().GetMainFrame()->DisplayHelp(L"online/readability-tests-english.html");
        }
    else
        {
        // unrecognized in-app anchor
        return false;
        }
    return true;
    }

//------------------------------------------------------
void ProjectView::OnHyperlinkClicked(wxHtmlLinkEvent& event)
    {
    if (!NavigateToHref(event.GetLinkInfo().GetHref()))
        {
        event.Skip();
        }
    }

//------------------------------------------------------
void ProjectView::OnExplanationNavigating(wxWebViewEvent& event)
    {
    const wxString url = event.GetURL();
    const wxString scheme{ NavLink::ExplanationScheme.data(), NavLink::ExplanationScheme.length() };
    // let the initial page load and any non-app navigations proceed
    if (!url.StartsWith(scheme))
        {
        event.Skip();
        return;
        }
    // an in-app explanation link was clicked; handle it ourselves instead of
    // letting the web view navigate to the (fake) custom-scheme URL
    event.Veto();
    wxString anchor = url.substr(scheme.length());
    // some backends append a trailing slash to a bare authority
    if (anchor.ends_with(L"/"))
        {
        anchor.RemoveLast();
        }
    // navigation is already vetoed, so whether the anchor is recognized or not
    // makes no difference here
    [[maybe_unused]]
    const bool handled = NavigateToHref(wxString{ L"#" } + anchor);
    }

//------------------------------------------------------
void ProjectView::OnTestListDblClick([[maybe_unused]] wxListEvent& event)
    {
    const wxString selectedTest =
        GetReadabilityScoresList()->GetResultsListCtrl()->GetSelectedText();
    const auto testIter =
        std::find(BaseProject::m_custom_word_tests.begin(), // NOLINT(*-use-ranges)
                  BaseProject::m_custom_word_tests.end(), selectedTest);
    const std::pair<std::vector<readability::readability_test>::const_iterator, bool> testPos =
        BaseProject::GetDefaultReadabilityTestsTemplate().find_test(selectedTest);

    if (testIter != BaseProject::m_custom_word_tests.end())
        {
        wxGetApp().EditCustomTest(*testIter);
        }
    else if (testPos.second)
        {
        if (testPos.first->has_language(readability::test_language::spanish_test))
            {
            wxGetApp().GetMainFrame()->DisplayHelp(L"online/readability-tests-spanish.html");
            }
        else if (testPos.first->has_language(readability::test_language::german_test))
            {
            wxGetApp().GetMainFrame()->DisplayHelp(L"online/readability-tests-german.html");
            }
        else
            {
            wxGetApp().GetMainFrame()->DisplayHelp(L"online/readability-tests-english.html");
            }
        }
    else if (selectedTest == ReadabilityMessages::GetDolchLabel())
        {
        wxGetApp().GetMainFrame()->DisplayHelp(L"online/reviewing-standard-projects.html");
        }
    }

/// Double-clicking on an item in the hard word list will jump to the respective text window and
/// find the word that you clicked on.
//------------------------------------------------------
void ProjectView::OnListDblClick(wxListEvent& event)
    {
    wxString searchText;
    wxString replacementText;
    const wxWindow* foundWindow{ nullptr };
    wxWindowID textId{ wxNOT_FOUND };

    const auto readSuggestionColumn = [&replacementText](const auto listCtrl)
    {
        replacementText.clear();
        const long selected = listCtrl->GetFirstSelected();
        if (selected != wxNOT_FOUND)
            {
            replacementText = listCtrl->GetItemText(selected, listCtrl->GetColumnCount() - 1);
            const auto tokens = wxStringTokenize(replacementText, L";,");
            if (!tokens.empty())
                {
                replacementText = tokens[0];
                }
            }
    };

    switch (event.GetId())
        {
    case HARD_WORDS_LIST_PAGE_ID:
        foundWindow = GetWordsBreakdownView().FindWindowById(event.GetId());
        if (foundWindow != nullptr)
            {
            const auto* listCtrl{ dynamic_cast<const Wisteria::UI::ListCtrlEx*>(foundWindow) };
            searchText = listCtrl->GetSelectedText();
            readSuggestionColumn(listCtrl);
            }
        textId = HARD_WORDS_TEXT_PAGE_ID;
        break;
    case LONG_WORDS_LIST_PAGE_ID:
        foundWindow = GetWordsBreakdownView().FindWindowById(event.GetId());
        if (foundWindow != nullptr)
            {
            const auto* listCtrl{ dynamic_cast<const Wisteria::UI::ListCtrlEx*>(foundWindow) };
            searchText = listCtrl->GetSelectedText();
            readSuggestionColumn(listCtrl);
            }
        textId = LONG_WORDS_TEXT_PAGE_ID;
        break;
    case DC_WORDS_LIST_PAGE_ID:
        foundWindow = GetWordsBreakdownView().FindWindowById(event.GetId());
        if (foundWindow != nullptr)
            {
            const auto* listCtrl{ dynamic_cast<const Wisteria::UI::ListCtrlEx*>(foundWindow) };
            searchText = listCtrl->GetSelectedText();
            readSuggestionColumn(listCtrl);
            }
        textId = DC_WORDS_TEXT_PAGE_ID;
        break;
    case HARRIS_JACOBSON_WORDS_LIST_PAGE_ID:
        foundWindow = GetWordsBreakdownView().FindWindowById(event.GetId());
        if (foundWindow != nullptr)
            {
            const auto* listCtrl{ dynamic_cast<const Wisteria::UI::ListCtrlEx*>(foundWindow) };
            searchText = listCtrl->GetSelectedText();
            readSuggestionColumn(listCtrl);
            }
        textId = HARRIS_JACOBSON_WORDS_TEXT_PAGE_ID;
        break;
    case SPACHE_WORDS_LIST_PAGE_ID:
        foundWindow = GetWordsBreakdownView().FindWindowById(event.GetId());
        if (foundWindow != nullptr)
            {
            const auto* listCtrl{ dynamic_cast<const Wisteria::UI::ListCtrlEx*>(foundWindow) };
            searchText = listCtrl->GetSelectedText();
            readSuggestionColumn(listCtrl);
            }
        textId = SPACHE_WORDS_TEXT_PAGE_ID;
        break;
    case ALL_WORDS_CONDENSED_LIST_PAGE_ID:
        [[fallthrough]];
    case ALL_WORDS_LIST_PAGE_ID:
        [[fallthrough]];
    case PROPER_NOUNS_LIST_PAGE_ID:
        foundWindow = GetWordsBreakdownView().FindWindowById(event.GetId());
        if (foundWindow != nullptr)
            {
            searchText =
                dynamic_cast<const Wisteria::UI::ListCtrlEx*>(foundWindow)->GetSelectedText();
            }
        textId = HARD_WORDS_TEXT_PAGE_ID;
        break;
    case OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID:
        foundWindow = GetGrammarView().FindWindowById(event.GetId());
        if (foundWindow != nullptr)
            {
            const auto selectedItem =
                dynamic_cast<const Wisteria::UI::ListCtrlEx*>(foundWindow)->GetFirstSelected();
            searchText = (selectedItem == wxNOT_FOUND) ?
                             wxString{} :
                             dynamic_cast<const Wisteria::UI::ListCtrlEx*>(foundWindow)
                                 ->GetItemTextEx(selectedItem, 1);
            }
        textId = HARD_WORDS_TEXT_PAGE_ID;
        break;
    case LONG_SENTENCES_LIST_PAGE_ID:
        foundWindow = GetSentencesBreakdownView().FindWindowById(event.GetId());
        if (foundWindow != nullptr)
            {
            searchText =
                dynamic_cast<const Wisteria::UI::ListCtrlEx*>(foundWindow)->GetSelectedText();
            }
        textId = LONG_SENTENCES_AND_WORDINESS_TEXT_PAGE_ID;
        break;
    // grammar lists will just use the same text window
    case SENTENCES_CONJUNCTION_START_LIST_PAGE_ID:
        [[fallthrough]];
    case SENTENCES_LOWERCASE_START_LIST_PAGE_ID:
        [[fallthrough]];
    case DUPLICATES_LIST_PAGE_ID:
        [[fallthrough]];
    case INCORRECT_ARTICLE_PAGE_ID:
        [[fallthrough]];
    case PASSIVE_VOICE_PAGE_ID:
        [[fallthrough]];
    case MISSPELLED_WORD_LIST_PAGE_ID:
        [[fallthrough]];
    case WORDY_PHRASES_LIST_PAGE_ID:
        [[fallthrough]];
    case REDUNDANT_PHRASE_LIST_PAGE_ID:
        [[fallthrough]];
    case WORDING_ERRORS_LIST_PAGE_ID:
        [[fallthrough]];
    case CLICHES_LIST_PAGE_ID:
        foundWindow = GetGrammarView().FindWindowById(event.GetId());
        if (foundWindow != nullptr)
            {
            searchText =
                dynamic_cast<const Wisteria::UI::ListCtrlEx*>(foundWindow)->GetSelectedText();
            }
        textId = LONG_SENTENCES_AND_WORDINESS_TEXT_PAGE_ID;
        break;
    case DOLCH_WORDS_LIST_PAGE_ID:
        foundWindow = GetDolchSightWordsView().FindWindowById(event.GetId());
        if (foundWindow != nullptr)
            {
            searchText =
                dynamic_cast<const Wisteria::UI::ListCtrlEx*>(foundWindow)->GetSelectedText();
            }
        textId = DOLCH_WORDS_TEXT_PAGE_ID;
        break;
    case NON_DOLCH_WORDS_LIST_PAGE_ID:
        foundWindow = GetDolchSightWordsView().FindWindowById(event.GetId());
        if (foundWindow != nullptr)
            {
            searchText =
                dynamic_cast<const Wisteria::UI::ListCtrlEx*>(foundWindow)->GetSelectedText();
            }
        textId = NON_DOLCH_WORDS_TEXT_PAGE_ID;
        break;
    default:
        // custom test
        foundWindow = GetWordsBreakdownView().FindWindowById(event.GetId());
        if (foundWindow != nullptr)
            {
            searchText =
                dynamic_cast<const Wisteria::UI::ListCtrlEx*>(foundWindow)->GetSelectedText();
            }
        textId = event.GetId();
        }

    searchText.Trim(true);

    // if the embedded editor is open, then select the text in there
    if (m_embeddedTextEditor != nullptr && m_embeddedTextEditor->IsShown())
        {
        m_embeddedTextEditor->SelectString(searchText, replacementText);
        }
    else
        {
        // Find the first occurrence of the selected word.
        // First, look in the word breakdown section for the respective test window,
        // then the grammar section and finally the Dolch section.
        wxWindow* theWindow = GetWordsBreakdownView().FindWindowById(textId, CLASSINFO(wxWebView));
        if (theWindow == nullptr)
            {
            theWindow = GetGrammarView().FindWindowById(textId, CLASSINFO(wxWebView));
            }
        if (theWindow == nullptr)
            {
            theWindow = GetDolchSightWordsView().FindWindowById(textId, CLASSINFO(wxWebView));
            }
        if ((theWindow != nullptr) && theWindow->IsKindOf(wxCLASSINFO(wxWebView)))
            {
            auto* textWindow = dynamic_cast<wxWebView*>(theWindow);
            textWindow->ClearSelection();
            // If looking for an entire sentence, then don't use whole-word search.
            // Whole-word search behaves differently between platforms and won't work for
            // sentences under GTK+ as expected (because of the terminal period).
            if (event.GetId() == LONG_SENTENCES_LIST_PAGE_ID ||
                event.GetId() == SENTENCES_LOWERCASE_START_LIST_PAGE_ID ||
                event.GetId() == SENTENCES_CONJUNCTION_START_LIST_PAGE_ID ||
                event.GetId() == OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID)
                {
                textWindow->Find(searchText, wxWEBVIEW_FIND_WRAP);
                }
            else
                {
                textWindow->Find(searchText, wxWEBVIEW_FIND_WRAP | wxWEBVIEW_FIND_ENTIRE_WORD);
                }
            // Search by label for custom word-list tests (the list and report have the same ID);
            // otherwise, search by ID.
            GetSideBar()->SelectSubItem((event.GetId() == textId) ?
                                            GetSideBar()->FindSubItem(textWindow->GetLabel()) :
                                            GetSideBar()->FindSubItem(textId));
            }

        // update the search panel to remember the string we searched for
        m_searchCtrl->SetFindString(searchText);
        }
    }

//-------------------------------------------------------
void ProjectView::OnLaunchSourceFile([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto* doc = dynamic_cast<ProjectDoc*>(GetDocument());
    if (doc != nullptr)
        {
        const FilePathResolver resolvePath(doc->GetOriginalDocumentFilePath(), false);
        if (doc->GetDocumentStorageMethod() == TextStorage::EmbedText)
            {
            // if the text was embedded but actually came from a file (i.e., not manually entered)
            // then ask if they prefer to relink to the file and then edit that, rather than editing
            // the embedded text. Also, don't bother asking if the file is a file inside an
            // archive.
            if (doc->GetTextSource() == TextSource::FromFile && !resolvePath.IsArchivedFile())
                {
                auto warningIter = WarningManager::GetWarning(_DT(L"linked-document-is-embedded"));
                // if they want to be prompted for this...
                if (warningIter != WarningManager::GetWarnings().end() &&
                    warningIter->ShouldBeShown())
                    {
                    wxRichMessageDialog msg(GetDocFrame(), warningIter->GetMessage(),
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
                        doc->SetModifiedFlag();
                        doc->SetDocumentStorageMethod(TextStorage::NoEmbedText);
                        wxLaunchDefaultApplication(doc->GetOriginalDocumentFilePath());
                        return;
                        }
                    }
                // or if they said "yes" before, then use the found path
                else if (warningIter != WarningManager::GetWarnings().end() &&
                         warningIter->GetPreviousResponse() == wxID_YES)
                    {
                    doc->SetModifiedFlag();
                    doc->SetDocumentStorageMethod(TextStorage::NoEmbedText);
                    wxLaunchDefaultApplication(doc->GetOriginalDocumentFilePath());
                    return;
                    }
                }
            if (m_embeddedTextEditor == nullptr)
                {
                m_embeddedTextEditor = new EditTextDlg(
                    GetDocFrame(), doc, doc->GetDocumentText(), wxID_ANY,
                    _(L"Edit Embedded Document"),
                    (!doc->GetAppendedDocumentText().empty()) ?
                        _(L"Note: The appended template document is not included here.\n"
                          "Only the embedded text is editable from this dialog.") :
                        wxString{});
                }

            m_embeddedTextEditor->Show();
            }
        else
            {
            if (resolvePath.IsArchivedFile())
                {
                wxMessageBox(_(L"Files inside of archives files cannot be edited."), wxString{},
                             wxOK | wxICON_INFORMATION);
                return;
                }
            if (resolvePath.IsExcelCell())
                {
                const size_t excelTag =
                    resolvePath.GetResolvedPath().MakeLower().find(_DT(L".xlsx#"));
                const wxFileName fn(resolvePath.GetResolvedPath().substr(0, excelTag + 5));
                wxLaunchDefaultApplication(fn.GetFullPath());
                }
            else if (resolvePath.IsOdsCell())
                {
                const size_t odsTag = resolvePath.GetResolvedPath().MakeLower().find(_DT(L".ods#"));
                const wxFileName fn(resolvePath.GetResolvedPath().substr(0, odsTag + 4));
                wxLaunchDefaultApplication(fn.GetFullPath());
                }
            else
                {
                wxLaunchDefaultApplication(resolvePath.GetResolvedPath());
                }
            }
        }
    }

//-------------------------------------------------------
bool ProjectView::IsHighlightedTextWindow(const wxWindow* window) const
    {
    const auto* doc = dynamic_cast<const ProjectDoc*>(GetDocument());
    return (doc != nullptr && window != nullptr &&
            doc->GetHighlightedTextBuffers().Find(window->GetId()) != nullptr);
    }

//-------------------------------------------------------
void ProjectView::SaveWebViewReport(wxWebView* webview, const wxString& savePathNoExt,
                                    const wxString& textExt) const
    {
    const auto* doc = dynamic_cast<const ProjectDoc*>(GetDocument());
    // only highlighted-text windows have a paper-white RTF buffer registered
    const auto* highlightedBuffers =
        (doc != nullptr) ? doc->GetHighlightedTextBuffers().Find(webview->GetId()) : nullptr;
    // don't silently save a different format than what was asked for
    if (textExt.CmpNoCase(L".rtf") == 0 && highlightedBuffers == nullptr)
        {
        wxLogWarning(L"'%s' cannot be saved as RTF; saving as HTML instead.", webview->GetName());
        }

    if (textExt.CmpNoCase(L".pdf") == 0)
        {
        webview->PrintToPDF(savePathNoExt + L".pdf");
        }
    else if (textExt.CmpNoCase(L".rtf") == 0 && highlightedBuffers != nullptr)
        {
        const wxString savePath = savePathNoExt + L".rtf";
        wxFileName{ savePath }.SetPermissions(wxS_DEFAULT);
        wxFile file{ savePath, wxFile::write };
        if (!file.Write(highlightedBuffers->m_rtf))
            {
            wxLogError(L"Failed to save report: (%s).", savePath);
            }
        }
    else
        {
        const wxString savePath = savePathNoExt + L".htm";
        wxString htmlText = webview->GetPageSource();
        ProjectReportFormat::StripBackToTopButton(htmlText);
        lily_of_the_valley::html_format::strip_hyperlinks(htmlText);
        if (!htmlText.starts_with(L"<!DOCTYPE"))
            {
            htmlText.insert(0, L"<!DOCTYPE html>\n");
            }
        wxFileName{ savePath }.SetPermissions(wxS_DEFAULT);
        wxFile file{ savePath, wxFile::write };
        if (!file.Write(htmlText))
            {
            wxLogError(L"Failed to save report: (%s).", savePath);
            }
        }
    }

// add/remove the icon to the list view on the side panel
//-------------------------------------------------------
void ProjectView::UpdateSideBarIcons()
    {
    wxGetApp().ApplyThemeToSideBar(GetSideBar());

    GetSideBar()->SaveState();
    GetSideBar()->DeleteAllFolders();

    const auto checkGraphType = [](wxWindow* window, const wxClassInfo* className)
    {
        const auto* canvas = dynamic_cast<Wisteria::Canvas*>(window);
        wxASSERT_MSG(canvas, L"Window is not a canvas!");
        return (canvas != nullptr) ? canvas->GetFixedObject(0, 0)->IsKindOf(className) : false;
    };

    // readability tests
    //-----------------
    // Note: refer to ReadabilityApp::InitProjectSidebar() for the icon indices.
    if (GetReadabilityResultsView().GetWindowCount() > 0)
        {
        GetSideBar()->InsertItem(GetSideBar()->GetFolderCount(), GetReadabilityScoresLabel(),
                                 SIDEBAR_READABILITY_SCORES_SECTION_ID, 1);

        for (auto* window : GetReadabilityResultsView().GetWindows())
            {
            const bool isGraph = window->IsKindOf(wxCLASSINFO(Wisteria::Canvas));

            GetSideBar()->InsertSubItemById(
                SIDEBAR_READABILITY_SCORES_SECTION_ID, window->GetName(), window->GetId(),
                window->GetId() == READABILITY_SCORES_SUMMARY_REPORT_PAGE_ID ? 17 :
                window->GetId() == READABILITY_SCORES_PAGE_ID                ? 23 :
                window->GetId() == READABILITY_GOALS_PAGE_ID                 ? 28 :
                (isGraph && checkGraphType(window, wxCLASSINFO(Wisteria::Graphs::FleschChart))) ?
                                                               18 :
                (isGraph && checkGraphType(window, wxCLASSINFO(Wisteria::Graphs::FraseGraph))) ?
                                                               19 :
                (isGraph && checkGraphType(window, wxCLASSINFO(Wisteria::Graphs::FryGraph))) ? 20 :
                (isGraph && checkGraphType(window, wxCLASSINFO(Wisteria::Graphs::RaygorGraph))) ?
                                                                                               21 :
                (isGraph && checkGraphType(window, wxCLASSINFO(Wisteria::Graphs::CrawfordGraph))) ?
                                                                                               22 :
                (isGraph && checkGraphType(window, wxCLASSINFO(Wisteria::Graphs::SchwartzGraph))) ?
                                                                                               25 :
                (isGraph && checkGraphType(window, wxCLASSINFO(Wisteria::Graphs::LixGauge))) ? 26 :
                (isGraph && checkGraphType(window, wxCLASSINFO(Wisteria::Graphs::LixGaugeGerman))) ?
                                                                                               26 :
                (isGraph &&
                 checkGraphType(window, wxCLASSINFO(Wisteria::Graphs::DanielsonBryan2Plot))) ?
                                                                                               27 :
                (isGraph && checkGraphType(window, wxCLASSINFO(Wisteria::Graphs::InfleszScale))) ?
                                                                                               31 :
                                                                                               9);
            }
        }

    // Summary statistics window
    if (GetSummaryView().GetWindowCount() > 0)
        {
        GetSideBar()->InsertItem(GetSideBar()->GetFolderCount(), GetSummaryStatisticsLabel(),
                                 SIDEBAR_STATS_SUMMARY_SECTION_ID, 2);
        for (const auto* window : GetSummaryView().GetWindows())
            {
            GetSideBar()->InsertSubItemById(
                SIDEBAR_STATS_SUMMARY_SECTION_ID, window->GetName(), window->GetId(),
                window->IsKindOf(wxCLASSINFO(wxWebView))                ? 17 :
                window->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx)) ? 15 :
                                                                          9);
            }
        }

    // Plain Language Guide window
    if (GetPlainLanguageGuideView().GetWindowCount() > 0)
        {
        GetSideBar()->InsertItem(GetSideBar()->GetFolderCount(), GetPlainLanguageGuideLabel(),
                                 SIDEBAR_PLAIN_LANGUAGE_GUIDE_SECTION_ID, 32);
        }

    // Words breakdown
    if (GetWordsBreakdownView().GetWindowCount() > 0)
        {
        GetSideBar()->InsertItem(GetSideBar()->GetFolderCount(), GetWordsBreakdownLabel(),
                                 SIDEBAR_WORDS_BREAKDOWN_SECTION_ID, 13);
        for (auto* window : GetWordsBreakdownView().GetWindows())
            {
            const bool isGraph = window->IsKindOf(wxCLASSINFO(Wisteria::Canvas));

            GetSideBar()->InsertSubItemById(
                SIDEBAR_WORDS_BREAKDOWN_SECTION_ID, window->GetName(), window->GetId(),
                IsHighlightedTextWindow(window)                                               ? 0 :
                window->IsKindOf(wxCLASSINFO(wxWebView))                                      ? 17 :
                window->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx))                       ? 15 :
                (isGraph && checkGraphType(window, wxCLASSINFO(Wisteria::Graphs::Histogram))) ? 6 :
                (isGraph && checkGraphType(window, wxCLASSINFO(Wisteria::Graphs::BarChart)))  ? 16 :
                (isGraph && checkGraphType(window, wxCLASSINFO(Wisteria::Graphs::WordCloud))) ? 29 :
                (isGraph && checkGraphType(window, wxCLASSINFO(Wisteria::Graphs::PieChart)))  ? 30 :
                                                                                                9);
            }
        }

    // Sentences breakdown
    if (GetSentencesBreakdownView().GetWindowCount() > 0)
        {
        GetSideBar()->InsertItem(GetSideBar()->GetFolderCount(), GetSentencesBreakdownLabel(),
                                 SIDEBAR_SENTENCES_BREAKDOWN_SECTION_ID, 14);
        for (auto* window : GetSentencesBreakdownView().GetWindows())
            {
            const bool isGraph = window->IsKindOf(wxCLASSINFO(Wisteria::Canvas));

            GetSideBar()->InsertSubItemById(
                SIDEBAR_SENTENCES_BREAKDOWN_SECTION_ID, window->GetName(), window->GetId(),
                window->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx))                       ? 15 :
                (isGraph && checkGraphType(window, wxCLASSINFO(Wisteria::Graphs::BoxPlot)))   ? 7 :
                (isGraph && checkGraphType(window, wxCLASSINFO(Wisteria::Graphs::Histogram))) ? 6 :
                (isGraph && checkGraphType(window, wxCLASSINFO(Wisteria::Graphs::HeatMap)))   ? 24 :
                                                                                                9);
            }
        }

    // grammar windows
    if (GetGrammarView().GetWindowCount() > 0)
        {
        GetSideBar()->InsertItem(GetSideBar()->GetFolderCount(), GetGrammarLabel(),
                                 SIDEBAR_GRAMMAR_SECTION_ID, 4);
        for (const auto* window : GetGrammarView().GetWindows())
            {
            GetSideBar()->InsertSubItemById(
                SIDEBAR_GRAMMAR_SECTION_ID, window->GetName(), window->GetId(),
                IsHighlightedTextWindow(window)                         ? 0 :
                window->IsKindOf(wxCLASSINFO(wxWebView))                ? 17 :
                window->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx)) ? 15 :
                                                                          9);
            }
        }

    // sight words
    if (GetDolchSightWordsView().GetWindowCount() > 0)
        {
        GetSideBar()->InsertItem(GetSideBar()->GetFolderCount(), GetDolchLabel(),
                                 SIDEBAR_DOLCH_SECTION_ID, 5);
        for (auto* window : GetDolchSightWordsView().GetWindows())
            {
            const bool isGraph = window->IsKindOf(wxCLASSINFO(Wisteria::Canvas));

            GetSideBar()->InsertSubItemById(
                SIDEBAR_DOLCH_SECTION_ID, window->GetName(), window->GetId(),
                IsHighlightedTextWindow(window)                                              ? 0 :
                window->IsKindOf(wxCLASSINFO(wxWebView))                                     ? 17 :
                (isGraph && checkGraphType(window, wxCLASSINFO(Wisteria::Graphs::BarChart))) ? 16 :
                window->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx))                      ? 15 :
                                                                                               9);
            }
        }

    GetSideBar()->ResetState();
    }

//-------------------------------------------------------
void ProjectView::OnAddTest(wxCommandEvent& event)
    {
    auto* doc = dynamic_cast<ProjectDoc*>(GetDocument());
    if (!doc->IsSafeToUpdate())
        {
        return;
        }

    if (event.GetId() == XRCID("ID_DOLCH"))
        {
        doc->AddDolchSightWords();
        }
    else
        {
        doc->GetReadabilityTests().include_test(
            doc->GetReadabilityTests().get_test_id(event.GetId()).c_str(), true);
        // refresh
        doc->RefreshRequired(ProjectRefresh::Minimal);
        doc->RefreshProject();
        const long testToSelect = GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(
            doc->GetReadabilityTests().get_test_long_name(event.GetId()).c_str());
        if (testToSelect != wxNOT_FOUND)
            {
            GetReadabilityScoresList()->GetResultsListCtrl()->Select(testToSelect);
            }
        GetSideBar()->SelectSubItem(
            GetSideBar()->FindSubItem(BaseProjectView::READABILITY_SCORES_PAGE_ID));
        // show any warning messages from the test being run
        doc->ShowQueuedMessages();
        if (WarningManager::HasWarning(_DT(L"click-test-to-view")))
            {
            ShowInfoMessage(*WarningManager::GetWarning(_DT(L"click-test-to-view")));
            }
        }
    }

//---------------------------------------------------
void ProjectView::OnGradeScale(wxCommandEvent& event)
    {
    auto* doc = dynamic_cast<ProjectDoc*>(GetDocument());
    if (!doc->IsSafeToUpdate())
        {
        return;
        }
    const wxWindowUpdateLocker noUpdates(doc->GetDocumentWindow());
    const BaseProjectProcessingLock processingLock(doc);

    for (size_t i = 0; i < GetDocFrame()->m_gradeScaleMenu.GetMenuItemCount(); ++i)
        {
        GetDocFrame()->m_gradeScaleMenu.FindItemByPosition(i)->Check(false);
        }

    readability::grade_scale gs = doc->GetReadabilityMessageCatalog().GetGradeScale();
    if (event.GetId() == XRCID("ID_K12_US"))
        {
        gs = readability::grade_scale::k12_plus_united_states;
        }
    else if (event.GetId() == XRCID("ID_K12_NEWFOUNDLAND"))
        {
        gs = readability::grade_scale::k12_plus_newfoundland_and_labrador;
        }
    else if (event.GetId() == XRCID("ID_K12_BC"))
        {
        gs = readability::grade_scale::k12_plus_british_columbia;
        }
    else if (event.GetId() == XRCID("ID_K12_NEW_BRUNSWICK"))
        {
        gs = readability::grade_scale::k12_plus_newbrunswick;
        }
    else if (event.GetId() == XRCID("ID_K12_NOVA_SCOTIA"))
        {
        gs = readability::grade_scale::k12_plus_nova_scotia;
        }
    else if (event.GetId() == XRCID("ID_K12_ONTARIO"))
        {
        gs = readability::grade_scale::k12_plus_ontario;
        }
    else if (event.GetId() == XRCID("ID_K12_SASKATCHEWAN"))
        {
        gs = readability::grade_scale::k12_plus_saskatchewan;
        }
    else if (event.GetId() == XRCID("ID_K12_PE"))
        {
        gs = readability::grade_scale::k12_plus_prince_edward_island;
        }
    else if (event.GetId() == XRCID("ID_K12_MANITOBA"))
        {
        gs = readability::grade_scale::k12_plus_manitoba;
        }
    else if (event.GetId() == XRCID("ID_K12_NT"))
        {
        gs = readability::grade_scale::k12_plus_northwest_territories;
        }
    else if (event.GetId() == XRCID("ID_K12_ALBERTA"))
        {
        gs = readability::grade_scale::k12_plus_alberta;
        }
    else if (event.GetId() == XRCID("ID_K12_NUNAVUT"))
        {
        gs = readability::grade_scale::k12_plus_nunavut;
        }
    else if (event.GetId() == XRCID("ID_QUEBEC"))
        {
        gs = readability::grade_scale::quebec;
        }
    else if (event.GetId() == XRCID("ID_ENGLAND"))
        {
        gs = readability::grade_scale::key_stages_england_wales;
        }
    GetDocFrame()->m_gradeScaleMenu.Check(event.GetId(), true);
    doc->GetReadabilityMessageCatalog().SetGradeScale(gs);
    const long selectedTest = GetReadabilityScoresList()->GetResultsListCtrl()->GetFirstSelected();
    doc->DisplayReadabilityScores(false);
    GetReadabilityScoresList()->GetResultsListCtrl()->Select(selectedTest);
    doc->SetModifiedFlag();
    }

//---------------------------------------------------
void ProjectView::OnAddToDictionary([[maybe_unused]] wxCommandEvent& event)
    {
    const Wisteria::UI::ListCtrlEx* listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        GetGrammarView().FindWindowById(MISSPELLED_WORD_LIST_PAGE_ID));
    if (listView != nullptr)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(MISSPELLED_WORD_LIST_PAGE_ID));

        long item = wxNOT_FOUND;
        wxArrayString newWords;
        while (true)
            {
            item = listView->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            if (item == wxNOT_FOUND)
                {
                break;
                }
            newWords.Add(listView->GetItemText(item));
            }
        if (newWords.GetCount() == 0)
            {
            wxMessageBox(_(L"Please select a word (or words) to add to your dictionary."),
                         _(L"Add to Dictionary"), wxOK | wxICON_INFORMATION);
            return;
            }
        wxGetApp().AddWordsToDictionaries(
            newWords, dynamic_cast<BaseProjectDoc*>(GetDocument())->GetProjectLanguage());
        const wxList docs = wxGetApp().GetDocManager()->GetDocuments();
        for (size_t i = 0; i < docs.GetCount(); ++i)
            {
            auto* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
            if (doc != nullptr)
                {
                doc->RemoveMisspellings(newWords);
                }
            }
        }
    else
        {
        wxMessageBox(_(L"There are no misspellings in this document."), _(L"Add to Dictionary"),
                     wxOK | wxICON_INFORMATION);
        }
    }

//---------------------------------------------------
void ProjectView::OnRibbonButtonCommand(wxRibbonButtonBarEvent& event)
    {
    wxCommandEvent cmd(wxEVT_MENU, event.GetId());
    // the document frame needs to handle document events
    if (event.GetId() == wxID_OPEN || event.GetId() == wxID_NEW || event.GetId() == wxID_SAVE)
        {
        GetDocFrame()->ProcessWindowEvent(cmd);
        }
    else
        {
        OnMenuCommand(cmd);
        }
    }

// Handles all menu events for the document and propagates to the active window
//---------------------------------------------------
void ProjectView::OnMenuCommand(wxCommandEvent& event)
    {
    auto* doc = dynamic_cast<ProjectDoc*>(GetDocument());
    if (!doc->IsSafeToUpdate())
        {
        return;
        }
    // show a message about zooming in and how the background image won't scale larger than its size
    if (event.GetId() == wxID_ZOOM_IN)
        {
        if (WarningManager::HasWarning(_DT(L"bkimage-zoomin-noupscale")))
            {
            ShowInfoMessage(*WarningManager::GetWarning(_DT(L"bkimage-zoomin-noupscale")));
            }
        }

    // propagate standard save command to active subwindow if "export window" option selected
    if (event.GetId() == XRCID("ID_SAVE_ITEM"))
        {
        event.SetId(wxID_SAVE);
        }
    else if (event.GetId() == XRCID("ID_PRINT"))
        {
        event.SetId(wxID_PRINT);
        }
    else if (event.GetId() == XRCID("ID_SORT_ASCENDING") && (GetActiveProjectWindow() != nullptr) &&
             GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(Wisteria::Canvas)) &&
             (dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())
                  ->GetFixedObject(0, 0)
                  ->IsKindOf(wxCLASSINFO(Wisteria::Graphs::BarChart)) ||
              dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())
                  ->GetFixedObject(0, 0)
                  ->IsKindOf(wxCLASSINFO(Wisteria::Graphs::Histogram))))
        {
        auto* barChart = dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow());
        std::dynamic_pointer_cast<Wisteria::Graphs::BarChart>(barChart->GetFixedObject(0, 0))
            ->SortBars(Wisteria::Graphs::BarChart::BarSortComparison::SortByBarLength,
                       Wisteria::SortDirection::SortAscending);
        barChart->Refresh();
        barChart->Update();
        return;
        }
    else if (event.GetId() == XRCID("ID_SORT_DESCENDING") &&
             (GetActiveProjectWindow() != nullptr) &&
             GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(Wisteria::Canvas)) &&
             (dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())
                  ->GetFixedObject(0, 0)
                  ->IsKindOf(wxCLASSINFO(Wisteria::Graphs::BarChart)) ||
              dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())
                  ->GetFixedObject(0, 0)
                  ->IsKindOf(wxCLASSINFO(Wisteria::Graphs::Histogram))))
        {
        auto* barChart = dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow());
        std::dynamic_pointer_cast<Wisteria::Graphs::BarChart>(barChart->GetFixedObject(0, 0))
            ->SortBars(Wisteria::Graphs::BarChart::BarSortComparison::SortByBarLength,
                       Wisteria::SortDirection::SortDescending);
        barChart->Refresh();
        barChart->Update();
        return;
        }

    if ((GetActiveProjectWindow() != nullptr) &&
        GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx)))
        {
        auto* list = dynamic_cast<Wisteria::UI::ListCtrlEx*>(GetActiveProjectWindow());
        /* just in case this is a print or preview command, update the window's headers
           and footer to whatever the global options currently are*/
        doc->UpdateListOptions(list);
        // in case we are exporting the window, set its label to include the name of the document,
        // and then reset it
        list->SetLabel(wxString::Format(L"%s [%s]", list->GetName(),
                                        wxFileName::StripExtension(doc->GetTitle())));
        if (event.GetId() == XRCID("ID_EXCLUDE_SELECTED"))
            {
            if (list->GetSelectedItemCount() == 0)
                {
                wxMessageBox(_(L"Please select an item to exclude."), _(L"Error"),
                             wxOK | wxICON_WARNING);
                return;
                }
            if (doc->GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences)
                {
                if (wxMessageBox(_(L"This project is not currently excluding text. "
                                   "Do you wish to change this?"),
                                 _(L"Text Exclusion Method"), wxYES_NO | wxICON_QUESTION) == wxYES)
                    {
                    doc->SetInvalidSentenceMethod(InvalidSentence::ExcludeFromAnalysis);
                    }
                else
                    {
                    wxMessageBox(_(L"Text exclusion not enabled. Items will not be excluded."),
                                 _(L"Error"), wxOK | wxICON_WARNING);
                    return;
                    }
                }
            if (doc->GetExcludedPhrasesPath().empty())
                {
                wxFileDialog dialog(GetActiveProjectWindow(),
                                    _(L"Specify Where to Save Word Exclusion List"), wxString{},
                                    wxString{}, _(L"Text files (*.txt)|*.txt"),
                                    wxFD_SAVE | wxFD_PREVIEW);
                if (dialog.ShowModal() != wxID_OK)
                    {
                    return;
                    }

                doc->SetExcludedPhrasesPath(dialog.GetPath());

                // if no application-level exclusion list is being used, then ask
                // if we want this new one to used for that as well
                if (wxGetApp().GetAppOptions()->GetExcludedPhrasesPath().empty())
                    {
                    auto warningIter =
                        WarningManager::GetWarning(_DT(L"set-app-exclusion-list-from-project"));
                    if (warningIter != WarningManager::GetWarnings().end() &&
                        warningIter->ShouldBeShown())
                        {
                        wxRichMessageDialog msg(GetFrame(), warningIter->GetMessage(),
                                                warningIter->GetTitle(), warningIter->GetFlags());
                        msg.ShowCheckBox(_(L"Remember my answer"));
                        const int dlgResponse = msg.ShowModal();
                        if (warningIter != WarningManager::GetWarnings().end() &&
                            msg.IsCheckBoxChecked())
                            {
                            warningIter->Show(false);
                            warningIter->SetPreviousResponse(dlgResponse);
                            }
                        if (dlgResponse == wxID_YES)
                            {
                            wxGetApp().GetAppOptions()->SetExcludedPhrasesPath(dialog.GetPath());
                            }
                        }
                    }
                }

            wxString selectedStrings;
            long item = wxNOT_FOUND;
            for (;;)
                {
                item = list->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
                if (item == wxNOT_FOUND)
                    {
                    break;
                    }
                selectedStrings += list->GetItemTextEx(item, 0) + L"\n";
                }

            wxString buffer;
            wxString filePath = doc->GetExcludedPhrasesPath();
            if (!Wisteria::TextStream::ReadFile(filePath, buffer))
                {
                wxMessageBox(_(L"Error loading excluded word list file."), _(L"Error"),
                             wxOK | wxICON_EXCLAMATION);
                return;
                }
            grammar::phrase_collection phrases;
            phrases.load_phrases(buffer, false, false);
            phrases.load_phrases(selectedStrings, true, true);
            phrases.remove_duplicates();

            wxFileName(filePath).SetPermissions(wxS_DEFAULT);
            wxFile outputFile(filePath, wxFile::write);
            if (!outputFile.IsOpened())
                {
                wxMessageBox(wxString::Format(
                                 _(L"Unable to save \"%s\".\nVerify that you have write access to "
                                   "this file or that it is not in use."),
                                 filePath),
                             _(L"Error"), wxOK | wxICON_ERROR);
                }
            else
                {
                outputFile.Write(phrases.to_string(), wxConvUTF8);
                doc->SetExcludedPhrasesPath(filePath);
                }

            list->DeselectAll();
            doc->RefreshRequired(ProjectRefresh::RefreshRequirement::FullReindexing);
            doc->RefreshProject();
            }
        else
            {
            const ParentEventBlocker blocker(list);
            list->ProcessWindowEvent(event);
            }
        }
    else if ((GetActiveProjectWindow() != nullptr) &&
             GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(wxWebView)))
        {
        auto* webview = dynamic_cast<wxWebView*>(GetActiveProjectWindow());
        webview->SetLabel(wxString::Format(L"%s [%s]", webview->GetName(),
                                           wxFileName::StripExtension(doc->GetTitle())));
        // a registry hit (see IsHighlightedTextWindow) both marks this as a highlighted-text
        // window and yields the buffers whose paper-white RTF backs saving and copying
        const auto* highlightedBuffers = doc->GetHighlightedTextBuffers().Find(webview->GetId());
        if (event.GetId() == wxID_SAVE)
            {
            // the file dialog's filter is the "RTF or HTML?" prompt; a registry hit
            // adds the RTF option (filter order HTML, RTF, PDF); otherwise HTML, PDF
            const wxString wildcard =
                (highlightedBuffers != nullptr) ?
                    _DT(L"HTML (*.htm;*.html)|*.htm;*.html|RTF (*.rtf)|*.rtf|PDF (*.pdf)|*.pdf") :
                    _DT(L"HTML (*.htm;*.html)|*.htm;*.html|PDF (*.pdf)|*.pdf");
            wxFileDialog dialog(GetFrame(), _(L"Save As"), wxString{}, webview->GetLabel(),
                                wildcard, wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
            if (dialog.ShowModal() != wxID_OK)
                {
                return;
                }
            wxFileName filePath = dialog.GetPath();
            const int filterIndex = dialog.GetFilterIndex();
            const bool isRtf = (highlightedBuffers != nullptr && filterIndex == 1);
            const bool isPdf =
                (highlightedBuffers != nullptr) ? (filterIndex == 2) : (filterIndex == 1);
            if (isPdf)
                {
                // GTK/macOS don't rewrite the filename's extension when the filter
                // changes, so force it to match the chosen format
                if (filePath.GetExt().CmpNoCase(L"pdf") != 0)
                    {
                    filePath.SetExt(L"pdf");
                    }
                webview->PrintToPDF(filePath.GetFullPath());
                }
            else if (isRtf)
                {
                if (filePath.GetExt().CmpNoCase(L"rtf") != 0)
                    {
                    filePath.SetExt(L"rtf");
                    }
                wxFileName{ filePath }.SetPermissions(wxS_DEFAULT);
                wxFile file{ filePath.GetFullPath(), wxFile::write };
                if (file.IsOpened())
                    {
                    file.Write(highlightedBuffers->m_rtf);
                    }
                }
            else
                {
                if (filePath.GetExt().CmpNoCase(L"htm") != 0 &&
                    filePath.GetExt().CmpNoCase(L"html") != 0)
                    {
                    filePath.SetExt(L"htm");
                    }
                wxString htmlText = webview->GetPageSource();
                ProjectReportFormat::StripBackToTopButton(htmlText);
                lily_of_the_valley::html_format::strip_hyperlinks(htmlText);
                if (!htmlText.starts_with(L"<!DOCTYPE"))
                    {
                    htmlText.insert(0, L"<!DOCTYPE html>\n");
                    }
                wxFileName{ filePath }.SetPermissions(wxS_DEFAULT);
                wxFile file{ filePath.GetFullPath(), wxFile::write };
                if (file.IsOpened())
                    {
                    file.Write(htmlText);
                    }
                }
            }
        else if (event.GetId() == wxID_COPY)
            {
            // copy the entire window to preserve formatting
            if (wxTheClipboard->Open())
                {
                wxString html = webview->GetPageSource();
                ProjectReportFormat::StripBackToTopButton(html);
                lily_of_the_valley::html_extract_text htmlExtract;
                const wchar_t* extracted = htmlExtract(html.wc_str(), html.length(), true, true);
                const wxString plainText{ (extracted != nullptr) ? extracted : L"" };

                auto* clipboardData = new wxDataObjectComposite();
                // For a highlighted-text window the paper-white RTF is the preferred
                // paste target. HTML and plain text back it up (some platforms won't
                // paste RTF).
                if (highlightedBuffers != nullptr && !highlightedBuffers->m_rtf.empty())
                    {
                    clipboardData->Add(new wxRtfDataObject(highlightedBuffers->m_rtf), true);
                    }
                clipboardData->Add(new wxHTMLDataObject(html));
                clipboardData->Add(new wxTextDataObject(plainText));
                wxTheClipboard->SetData(clipboardData);
                wxTheClipboard->Close();
                }
            }
        else if (event.GetId() == wxID_SELECTALL)
            {
            webview->SelectAll();
            }
        else
            {
            webview->Print();
            }
        }
    else if ((GetActiveProjectWindow() != nullptr) &&
             GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(ExplanationListCtrl)))
        {
        auto* elist = dynamic_cast<ExplanationListCtrl*>(GetActiveProjectWindow());
        doc->UpdateExplanationListOptions(elist);
        elist->SetLabel(wxString::Format(L"%s [%s]", elist->GetName(),
                                         wxFileName::StripExtension(doc->GetTitle())));
        const ParentEventBlocker blocker(elist);
        elist->ProcessWindowEvent(event);
        }
    else if ((GetActiveProjectWindow() != nullptr) &&
             GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(Wisteria::Canvas)))
        {
        auto* graph = dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow());
        doc->UpdateGraphOptions(graph);
        graph->SetLabel(wxString::Format(L"%s [%s]", graph->GetName(),
                                         wxFileName::StripExtension(doc->GetTitle())));
        const ParentEventBlocker blocker(graph);
        graph->ProcessWindowEvent(event);
        }
    }

//---------------------------------------------------
void ProjectView::OnFind(wxFindDialogEvent& event)
    {
    if (GetActiveProjectWindow() == nullptr)
        {
        return;
        }

    if (GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(wxWebView)))
        {
        auto* webview = dynamic_cast<wxWebView*>(GetActiveProjectWindow());
        if (event.GetEventType() == wxEVT_COMMAND_FIND_CLOSE)
            {
            webview->Find(wxString{});
            return;
            }
        int webFlags = wxWEBVIEW_FIND_WRAP | wxWEBVIEW_FIND_HIGHLIGHT_RESULT;
        const int frFlags = event.GetFlags();
        if ((frFlags & wxFR_MATCHCASE) != 0)
            {
            webFlags |= wxWEBVIEW_FIND_MATCH_CASE;
            }
        if ((frFlags & wxFR_WHOLEWORD) != 0)
            {
            webFlags |= wxWEBVIEW_FIND_ENTIRE_WORD;
            }
        if ((frFlags & wxFR_DOWN) == 0)
            {
            webFlags |= wxWEBVIEW_FIND_BACKWARDS;
            }
        if (webview->Find(event.GetFindString(), webFlags) == wxNOT_FOUND)
            {
            wxMessageBox(_(L"The text could not be found."), _(L"Text Not Found"));
            }
        }
    else
        {
        if (event.GetEventType() == wxEVT_COMMAND_FIND_CLOSE)
            {
            return;
            }
        const ParentEventBlocker blocker(GetActiveProjectWindow());
        GetActiveProjectWindow()->ProcessWindowEvent(event);
        }
    }

//-------------------------------------------------------
void ProjectView::UpdateRibbonState()
    {
    auto* projDoc = dynamic_cast<ProjectDoc*>(GetDocument());
    wxWindow* projectButtonBarWindow =
        GetRibbon()->FindWindow(MainFrame::ID_PROJECT_RIBBON_BUTTON_BAR);
    if ((projDoc != nullptr) && (projectButtonBarWindow != nullptr) &&
        projectButtonBarWindow->IsKindOf(wxCLASSINFO(wxRibbonButtonBar)))
        {
        auto* projBar = dynamic_cast<wxRibbonButtonBar*>(projectButtonBarWindow);
        wxASSERT(projBar);
        if (projBar != nullptr)
            {
            projBar->ToggleButton(XRCID("ID_REALTIME_UPDATE"), projDoc->IsRealTimeUpdating());
            projBar->EnableButton(XRCID("ID_REALTIME_UPDATE"),
                                  projDoc->GetDocumentStorageMethod() ==
                                      TextStorage::LoadFromExternalDocument);
            }
        }

    if (projDoc != nullptr)
        {
        MainFrame::FillPlainLanguageGuideListMenu(GetDocFrame()->m_plainLanguageGuideListMenu,
                                                  projDoc->GetPlainLanguageGuideListName());
        }
    }

//-------------------------------------------------------
bool ProjectView::OnCreate(wxDocument* doc, long flags)
    {
    if (!BaseProjectView::OnCreate(doc, flags))
        {
        return false;
        }

    // Results view
    auto* readabilityScoresView = new ExplanationListCtrl(
        GetSplitter(), READABILITY_SCORES_PAGE_ID, wxDefaultPosition, wxDefaultSize, _(L"Scores"));
    readabilityScoresView->Hide();
    readabilityScoresView->GetDataProvider()->SetNumberFormatter(
        dynamic_cast<BaseProjectDoc*>(doc)->GetReadabilityMessageCatalogPtr());
    readabilityScoresView->GetResultsListCtrl()->SetVirtualDataSize(0, 5);
    readabilityScoresView->GetResultsListCtrl()->InsertColumn(0, _(L"Test"), wxLIST_FORMAT_LEFT,
                                                              wxLIST_AUTOSIZE_USEHEADER);
    readabilityScoresView->GetResultsListCtrl()->InsertColumn(
        1, _(L"Grade Level"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE_USEHEADER);
    readabilityScoresView->GetResultsListCtrl()->InsertColumn(
        2, _(L"Reader Age"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE_USEHEADER);
    readabilityScoresView->GetResultsListCtrl()->InsertColumn(
        3, _(L"Scale Value"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE_USEHEADER);
    readabilityScoresView->GetResultsListCtrl()->InsertColumn(
        4, _(L"Predicted Cloze Score"), wxLIST_FORMAT_LEFT, wxLIST_AUTOSIZE_USEHEADER);
    if (readabilityScoresView->GetExplanationView() != nullptr)
        {
        readabilityScoresView->GetExplanationView()->SetPage(
            wxString::Format(
                _DT(L"<!DOCTYPE html><html><head><meta name='color-scheme' content='light dark' "
                    "/><style>body{background-color:Canvas;color:CanvasText;}</style></head>"
                    "<body>%s</body></html>"),
                _(L"No readability test results currently available.")),
            wxString{});
        }
    readabilityScoresView->GetResultsListCtrl()->AssignContextMenu(
        wxXmlResource::Get()->LoadMenu(L"IDM_READABILITY_SCORE_LIST"));
    readabilityScoresView->SetPrinterSettings(wxGetApp().GetPrintData());
    readabilityScoresView->SetLeftPrinterHeader(wxGetApp().GetAppOptions()->GetLeftPrinterHeader());
    readabilityScoresView->SetCenterPrinterHeader(
        wxGetApp().GetAppOptions()->GetCenterPrinterHeader());
    readabilityScoresView->SetRightPrinterHeader(
        wxGetApp().GetAppOptions()->GetRightPrinterHeader());
    readabilityScoresView->SetLeftPrinterFooter(wxGetApp().GetAppOptions()->GetLeftPrinterFooter());
    readabilityScoresView->SetCenterPrinterFooter(
        wxGetApp().GetAppOptions()->GetCenterPrinterFooter());
    readabilityScoresView->SetRightPrinterFooter(
        wxGetApp().GetAppOptions()->GetRightPrinterFooter());
    readabilityScoresView->SetResources(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                                        L"online/customizing-results.html");
    // the stats row background is blended from system colors, so re-apply it
    // whenever the OS color scheme changes
    readabilityScoresView->GetResultsListCtrl()->Bind(
        wxEVT_SYS_COLOUR_CHANGED, &ProjectView::OnReadabilityScoresSysColourChanged, this);
    GetReadabilityResultsView().AddWindow(readabilityScoresView);
    GetSplitter()->SplitVertically(GetSideBar(), readabilityScoresView,
                                   GetSideBar()->GetMinWidth());

#ifdef __WXOSX__
    // just load the menubar right now, we will set it in Present after the document has
    // successfully loaded
    m_menuBar = wxXmlResource::Get()->LoadMenuBar(L"ID_DOCMENUBAR");
    #ifdef APP_STORE_BUILD
    if (m_menuBar != nullptr)
        {
        if (wxMenu * checkForUpdatesMenu{ nullptr };
            m_menuBar->FindItem(XRCID("ID_CHECK_FOR_UPDATES"), &checkForUpdatesMenu) != nullptr)
            {
            checkForUpdatesMenu->Destroy(XRCID("ID_CHECK_FOR_UPDATES"));
            }
        }
    #endif
#endif

    // connect the test events
    for (const auto& rTest :
         dynamic_cast<const BaseProjectDoc*>(doc)->GetReadabilityTests().get_tests())
        {
        Connect(rTest.get_test().get_interface_id(), wxEVT_MENU,
                wxCommandEventHandler(ProjectView::OnAddTest));
        }

    return true;
    }

//-------------------------------------------------------
void ProjectView::UpdateStatistics()
    {
    auto* doc = dynamic_cast<ProjectDoc*>(GetDocument());

    const wxString selectedItem =
        GetReadabilityScoresList()->GetResultsListCtrl()->GetSelectedText();
    // remove stats rows if already in here because we have to recalculate everything
    long statIconLocation =
        GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(GetAverageLabel());
    if (statIconLocation != wxNOT_FOUND)
        {
        GetReadabilityScoresList()->GetResultsListCtrl()->DeleteItem(statIconLocation);
        }
    statIconLocation = GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(GetMedianLabel());
    if (statIconLocation != wxNOT_FOUND)
        {
        GetReadabilityScoresList()->GetResultsListCtrl()->DeleteItem(statIconLocation);
        }
    statIconLocation = GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(GetModeLabel());
    if (statIconLocation != wxNOT_FOUND)
        {
        GetReadabilityScoresList()->GetResultsListCtrl()->DeleteItem(statIconLocation);
        }
    statIconLocation = GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(GetStdDevLabel());
    if (statIconLocation != wxNOT_FOUND)
        {
        GetReadabilityScoresList()->GetResultsListCtrl()->DeleteItem(statIconLocation);
        }
    // update the averages of the scores
    const int rowCount = GetReadabilityScoresList()->GetResultsListCtrl()->GetItemCount();
    if (rowCount > 1)
        {
        std::vector<double> grades, ages, clozeScores;
        // tally up the numbers in the age and grade columns
        for (int i = 0; i < rowCount; ++i)
            {
            double value = 0;
            if (ReadabilityMessages::GetScoreValue(
                    GetReadabilityScoresList()->GetResultsListCtrl()->GetItemTextEx(i, 1), value))
                {
                grades.push_back(value);
                }
            if (ReadabilityMessages::GetScoreValue(
                    GetReadabilityScoresList()->GetResultsListCtrl()->GetItemTextEx(i, 2), value))
                {
                ages.push_back(value);
                }
            if (ReadabilityMessages::GetScoreValue(
                    GetReadabilityScoresList()->GetResultsListCtrl()->GetItemTextEx(i, 4), value))
                {
                clozeScores.push_back(value);
                }
            }

        wxString gradeAverage(_(L"N/A")), ageAverage(_(L"N/A")), clozeAverage(_(L"N/A")),
            gradeMedian(_(L"N/A")), ageMedian(_(L"N/A")), clozeMedian(_(L"N/A")),
            gradeMode(_(L"N/A")), ageMode(_(L"N/A")), clozeMode(_(L"N/A"));
        // get the average grade
        if (!grades.empty())
            {
            gradeAverage = wxNumberFormatter::ToString(
                statistics::mean(grades), 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
            gradeMedian = wxNumberFormatter::ToString(
                statistics::median(grades), 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
            const std::set<double> modes = statistics::mode(grades, floor_value<double>{});
            gradeMode.Clear();
            for (const double mode : modes)
                {
                gradeMode +=
                    doc->GetReadabilityMessageCatalog().GetFormattedValue(
                        wxNumberFormatter::ToString(
                            mode, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::CustomFormatting }) +
                    L"; ";
                }
            // chop off the last "; "
            if (gradeMode.length() > 2)
                {
                gradeMode.RemoveLast(2);
                }
            }
        // get the average grade level
        if (!ages.empty())
            {
            ageAverage = wxNumberFormatter::ToString(
                statistics::mean(ages), 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
            ageMedian = wxNumberFormatter::ToString(
                statistics::median(ages), 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
            const std::set<double> modes = statistics::mode(ages, floor_value<double>{});
            ageMode.Clear();
            for (const double mode : modes)
                {
                ageMode += wxNumberFormatter::ToString(
                               mode, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                           L"; ";
                }
            // chop off the last "; "
            if (ageMode.length() > 2)
                {
                ageMode.RemoveLast(2);
                }
            }
        // get the average cloze score
        if (!clozeScores.empty())
            {
            clozeAverage = wxNumberFormatter::ToString(
                statistics::mean(clozeScores), 2, wxNumberFormatter::Style::Style_NoTrailingZeroes);
            clozeMedian =
                wxNumberFormatter::ToString(statistics::median(clozeScores), 2,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes);
            const std::set<double> modes = statistics::mode(clozeScores, floor_value<double>{});
            clozeMode.Clear();
            for (const double mode : modes)
                {
                clozeMode += wxNumberFormatter::ToString(
                                 mode, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                             L"; ";
                }
            // chop off the last "; "
            if (clozeMode.length() > 2)
                {
                clozeMode.RemoveLast(2);
                }
            }

        const int firstStatLocation =
            GetReadabilityScoresList()->GetResultsListCtrl()->AddRow(GetAverageLabel());
        GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(
            firstStatLocation, 1, gradeAverage,
            Wisteria::NumberFormatInfo(
                Wisteria::NumberFormatInfo::NumberFormatType::CustomFormatting, 1));
        GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(firstStatLocation, 2,
                                                                      ageAverage);
        GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(firstStatLocation, 3,
                                                                      _(L"N/A"));
        GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(firstStatLocation, 4,
                                                                      clozeAverage);

        // format the explanation of the averages
        wxString explanationString =
            L"<div class='explanation-card'>"
            "<div class='explanation-card-header'>" +
            _(L"Averages") + L"</div><div class='explanation-card-body'>" +
            wxString::Format(
                _(L"<p>Average grade level: %s<br />Average reading age: %s<br />"
                  "Average predicted cloze score: %s</p><p>Note that an average of the "
                  "scale values is not applicable because the scales used between tests are "
                  "different.</p>"),
                doc->GetReadabilityMessageCatalog().GetFormattedValue(
                    gradeAverage,
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::CustomFormatting }),
                ageAverage, clozeAverage) +
            L"</div></div>";

        GetReadabilityScoresList()->GetExplanations()[GetAverageLabel()] = explanationString;

        if (doc->GetStatisticsReportInfo().IsExtendedInformationEnabled())
            {
            // Mode
            long statLocation =
                GetReadabilityScoresList()->GetResultsListCtrl()->AddRow(GetModeLabel());
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 1,
                                                                          gradeMode);
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 2, ageMode);
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 3,
                                                                          _(L"N/A"));
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 4,
                                                                          clozeMode);

            // format the explanation of the modes
            explanationString =
                L"<div class='explanation-card'>"
                "<div class='explanation-card-header'>" +
                _(L"Modes") + L"</div><div class='explanation-card-body'>" +
                wxString::Format(
                    _(L"<p>Grade level modes: %s<br />Reading age modes: %s<br />"
                      "Predicted cloze score modes: %s</p><p>The mode is the most frequently "
                      "occurring value in a range of data. Note that grade-level scores are "
                      "rounded down when searching for the mode.</p><p>Note that a mode of the "
                      "scale values is not applicable because the scales used between "
                      "tests are different.</p>"),
                    gradeMode, ageMode, clozeMode) +
                L"</div></div>";

            GetReadabilityScoresList()->GetExplanations()[GetModeLabel()] = explanationString;

            // Median
            statLocation =
                GetReadabilityScoresList()->GetResultsListCtrl()->AddRow(GetMedianLabel());
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(
                statLocation, 1, gradeMedian,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::CustomFormatting, 1));
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 2,
                                                                          ageMedian);
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 3,
                                                                          _(L"N/A"));
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 4,
                                                                          clozeMedian);

            // format the explanation of the medians
            explanationString =
                L"<div class='explanation-card'>"
                "<div class='explanation-card-header'>" +
                _(L"Medians") + L"</div><div class='explanation-card-body'>" +
                wxString::Format(
                    _(L"<p>Grade level median: %s<br />Reading age median: %s<br />"
                      "Predicted cloze score median: %s</p><p>The median is the midpoint of a "
                      "given range of values that divides them into lower and higher halves.</p><p>"
                      "Note that a median of the scale values is not applicable because the scales "
                      "used between tests are different.</p>"),
                    doc->GetReadabilityMessageCatalog().GetFormattedValue(
                        gradeMedian,
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::CustomFormatting }),
                    ageMedian, clozeMedian) +
                L"</div></div>";

            GetReadabilityScoresList()->GetExplanations()[GetMedianLabel()] = explanationString;

            // get the standard deviation
            const wxString gradeStdDev =
                ((grades.size() < 2) ?
                     _(L"N/A") :
                     wxNumberFormatter::ToString(
                         statistics::standard_deviation(grades, doc->GetVarianceMethod() ==
                                                                    VarianceMethod::SampleVariance),
                         1, wxNumberFormatter::Style::Style_NoTrailingZeroes));

            const wxString ageStdDev =
                ((ages.size() < 2) ?
                     _(L"N/A") :
                     wxNumberFormatter::ToString(
                         statistics::standard_deviation(ages, doc->GetVarianceMethod() ==
                                                                  VarianceMethod::SampleVariance),
                         1, wxNumberFormatter::Style::Style_NoTrailingZeroes));

            const wxString clozeStdDev =
                ((clozeScores.size() < 2) ?
                     _(L"N/A") :
                     wxNumberFormatter::ToString(
                         statistics::standard_deviation(clozeScores,
                                                        doc->GetVarianceMethod() ==
                                                            VarianceMethod::SampleVariance),
                         2, wxNumberFormatter::Style::Style_NoTrailingZeroes));

            statLocation =
                GetReadabilityScoresList()->GetResultsListCtrl()->AddRow(GetStdDevLabel());
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(
                statLocation, 1, gradeStdDev,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::CustomFormatting, 1));
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 2,
                                                                          ageStdDev);
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 3,
                                                                          _(L"N/A"));
            GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(statLocation, 4,
                                                                          clozeStdDev);

            // format the explanation of the variances
            explanationString =
                L"<div class='explanation-card'>"
                "<div class='explanation-card-header'>" +
                _(L"Standard Deviations") + L"</div><div class='explanation-card-body'>" +
                wxString::Format(
                    // TRANSLATORS: %s are formatted numbers.
                    // Also, "std. dev." is standard deviation.
                    _(L"<p>Grade level std. dev.: %s<br />Reading age std. dev.: %s<br />"
                      "Predicted cloze score std. dev.: %s</p><p>"
                      "Standard deviation is the measurement of how far values in a range "
                      "of data are spread apart from each other.</p><p>Note that at least two "
                      "valid test scores are required to have any standard deviation. "
                      "Also note that a standard deviation of the scale values is not applicable "
                      "because the scales used between tests are different.</p>"),
                    doc->GetReadabilityMessageCatalog().GetFormattedValue(
                        gradeStdDev,
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::CustomFormatting }),
                    ageStdDev, clozeStdDev) +
                L"</div></div>";

            GetReadabilityScoresList()->GetExplanations()[GetStdDevLabel()] = explanationString;
            }

        GetReadabilityScoresList()->GetResultsListCtrl()->SetSortableRange(0,
                                                                           firstStatLocation - 1);
        }
    else
        {
        GetReadabilityScoresList()->GetResultsListCtrl()->SetSortableRange(0, 0);
        }

    if (GetReadabilityScoresList()->GetResultsListCtrl()->GetSortedColumn() == -1)
        {
        GetReadabilityScoresList()->GetResultsListCtrl()->SetSortedColumn(
            0, Wisteria::SortDirection::SortAscending);
        }
    GetReadabilityScoresList()->GetResultsListCtrl()->Resort();

    RefreshStatRowColours();

    // select the item user had selected before the update
    const auto selectedItemLocation =
        GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(selectedItem);
    if (selectedItemLocation != wxNOT_FOUND)
        {
        GetReadabilityScoresList()->GetResultsListCtrl()->Select(selectedItemLocation);
        }
    }

//-------------------------------------------------------
void ProjectView::RefreshStatRowColours()
    {
    wxListItemAttr statRowAttribs;
    statRowAttribs.SetBackgroundColour(ExplanationListCtrl::GetStatRowBackgroundColour(
        GetReadabilityScoresList()->GetResultsListCtrl()->GetBackgroundColour()));

    for (const auto& label :
         { GetAverageLabel(), GetMedianLabel(), GetModeLabel(), GetStdDevLabel() })
        {
        const long statLocation = GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(label);
        if (statLocation != wxNOT_FOUND)
            {
            GetReadabilityScoresList()->GetResultsListCtrl()->SetRowAttributes(statLocation,
                                                                               statRowAttribs);
            }
        }
    GetReadabilityScoresList()->GetResultsListCtrl()->Refresh();
    }

//-------------------------------------------------------
void ProjectView::OnReadabilityScoresSysColourChanged(wxSysColourChangedEvent& event)
    {
    RefreshStatRowColours();
    event.Skip();
    }

//-------------------------------------------------------
void ProjectView::OnTextWindowColorsChange([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto* doc = dynamic_cast<ProjectDoc*>(GetDocument());
    if (!doc->IsSafeToUpdate())
        {
        return;
        }

    ToolsOptionsDlg optionsDlg(GetDocFrame(), dynamic_cast<ProjectDoc*>(GetDocument()),
                               ToolsOptionsDlg::TextSection);
    if (GetSideBar()->GetSelectedFolderId() == SIDEBAR_DOLCH_SECTION_ID)
        {
        optionsDlg.SelectPage(ToolsOptionsDlg::DOCUMENT_DISPLAY_DOLCH_PAGE);
        }
    else
        {
        optionsDlg.SelectPage(ToolsOptionsDlg::DOCUMENT_DISPLAY_GENERAL_PAGE);
        }

    if (optionsDlg.ShowModal() == wxID_OK)
        {
        const wxWindowUpdateLocker noUpdates(doc->GetDocumentWindow());
        const BaseProjectProcessingLock processingLock(doc);
        const wxBusyCursor wait;
        doc->DisplayHighlightedText(doc->GetTextHighlightColor(), doc->GetTextViewFont());
        doc->ResetRefreshRequired();
        doc->SetModifiedFlag();
        }
    }

//-------------------------------------------------------
void ProjectView::OnLongFormat([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto* doc = dynamic_cast<ProjectDoc*>(GetDocument());
    if (!doc->IsSafeToUpdate())
        {
        return;
        }
    const wxWindowUpdateLocker noUpdates(doc->GetDocumentWindow());
    const BaseProjectProcessingLock processingLock(doc);

    doc->GetReadabilityMessageCatalog().SetLongGradeScaleFormat(
        !doc->GetReadabilityMessageCatalog().IsUsingLongGradeScaleFormat());
    GetReadabilityScoresList()->GetResultsListCtrl()->Refresh();
    GetReadabilityScoresList()->GetResultsListCtrl()->SetColumnWidth(
        1, GetReadabilityScoresList()->GetResultsListCtrl()->EstimateColumnWidth(1));
    doc->SetModifiedFlag();
    }

// A test result is being removed from the project
//-------------------------------------------------------
void ProjectView::OnTestDeleteMenu([[maybe_unused]] wxCommandEvent& event)
    {
    wxRibbonButtonBarEvent cmd;
    OnTestDelete(cmd);
    }

//-------------------------------------------------------
void ProjectView::OnTestDelete([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    if ((GetActiveProjectWindow() != nullptr) &&
        GetActiveProjectWindow()->GetId() != READABILITY_SCORES_PAGE_ID)
        {
        const auto [parentId, childId] = GetSideBar()->FindSubItem(
            SIDEBAR_READABILITY_SCORES_SECTION_ID, READABILITY_SCORES_PAGE_ID);
        if (!childId.has_value())
            {
            return;
            }
        GetSideBar()->SelectSubItem(parentId.value(), childId.value());
        }
    const auto selectedIndex = GetReadabilityScoresList()->GetResultsListCtrl()->GetFirstSelected();
    if (selectedIndex != wxNOT_FOUND)
        {
        const wxString testToRemove =
            GetReadabilityScoresList()->GetResultsListCtrl()->GetItemText(selectedIndex);
        if (testToRemove == GetAverageLabel() || testToRemove == GetMedianLabel() ||
            testToRemove == GetStdDevLabel() || testToRemove == GetModeLabel())
            {
            return;
            }

        auto warningIter = WarningManager::GetWarning(_DT(L"remove-test-from-project"));
        // if they really want to remove this test
        if (warningIter != WarningManager::GetWarnings().end() && warningIter->ShouldBeShown())
            {
            wxRichMessageDialog msg(
                GetDocFrame(),
                wxString::Format(_(L"Do you wish to remove \"%s\" from the project?"),
                                 testToRemove),
                _(L"Remove Test"), wxYES_NO | wxNO_DEFAULT | wxICON_QUESTION);
            msg.SetEscapeId(wxID_NO);
            msg.ShowCheckBox(_(L"Always delete without prompting"));
            const int dlgResponse = msg.ShowModal();
            // save the checkbox status
            if (msg.IsCheckBoxChecked() && (dlgResponse == wxID_YES))
                {
                warningIter->Show(false);
                warningIter->SetPreviousResponse(dlgResponse);
                }
            // now see if they said "Yes" or "No"
            if (dlgResponse == wxID_NO)
                {
                return;
                }
            }

        GetReadabilityScoresList()->GetResultsListCtrl()->DeleteItem(selectedIndex);
        UpdateStatistics();
        if (GetReadabilityScoresList()->GetExplanationView() != nullptr)
            {
            GetReadabilityScoresList()->GetExplanationView()->SetPage(wxString{}, wxString{});
            }

        auto* doc = dynamic_cast<ProjectDoc*>(GetDocument());
        doc->RemoveTest(testToRemove);
        // if removing Dolch, we need remove the Dolch section
        if (testToRemove == ReadabilityMessages::GetDolchLabel())
            {
            GetDolchSightWordsView().Clear();
            }
        // remove the Averages row if there are no tests left
        if (GetReadabilityScoresList()->GetResultsListCtrl()->GetItemCount() == 1)
            {
            const long location =
                GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(GetAverageLabel(), 0);
            if (location != wxNOT_FOUND)
                {
                GetReadabilityScoresList()->GetResultsListCtrl()->DeleteItem(location);
                }
            }
        if (GetReadabilityScoresList()->GetResultsListCtrl()->GetItemCount() == 0)
            {
            if (GetReadabilityScoresList()->GetExplanationView() != nullptr)
                {
                GetReadabilityScoresList()->GetExplanationView()->SetPage(
                    wxString::Format(
                        _DT(L"<!DOCTYPE html><html><head>"
                            "<meta name='color-scheme' content='light dark' />"
                            "<style>body{background-color:Canvas;color:CanvasText;}</style>"
                            "</head><body>%s</body></html>"),
                        _(L"No readability test results currently available.")),
                    wxString{});
                }
            }
        // which tests are included may affect which stats and bars on the bar chart are included
        doc->RefreshRequired(ProjectRefresh::Minimal);
        doc->RefreshProject();
        }
    else
        {
        wxMessageBox(_(L"Please select a test to remove."), wxGetApp().GetAppName(),
                     wxOK | wxICON_INFORMATION);
        }
    }

/// project view sidebar was clicked
//-------------------------------------------------------
void ProjectView::OnItemSelected(wxCommandEvent& event)
    {
    wxASSERT(GetRibbon() != nullptr);
    const auto hideEditPanel = [this](const wxWindowID windowId)
    {
        wxWindow* editButtonBarWindow = GetRibbon()->FindWindow(windowId);
        wxASSERT(editButtonBarWindow != nullptr);
        wxASSERT(editButtonBarWindow->IsKindOf(wxCLASSINFO(wxRibbonPanel)));
        editButtonBarWindow->Show(false);
        return dynamic_cast<wxRibbonPanel*>(editButtonBarWindow);
    };

    const auto getEditButtonBar = [](wxRibbonPanel* panel)
    {
        auto* buttonBar = panel->FindWindow(MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);
        wxASSERT(buttonBar != nullptr && buttonBar->IsKindOf(wxCLASSINFO(wxRibbonButtonBar)));
        return dynamic_cast<wxRibbonButtonBar*>(buttonBar);
    };

    const auto resetActiveCanvasResizeDelay = [this]()
    {
        if (GetActiveProjectWindow() != nullptr &&
            GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(Wisteria::Canvas)))
            {
            wxASSERT(dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow()));
            dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())->ResetResizeDelay();
            }
    };

    wxRibbonPanel* editListButtonBarWindow = hideEditPanel(MainFrame::ID_EDIT_RIBBON_LIST_PANEL);
    wxRibbonPanel* editSummaryReportButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_SUMMARY_REPORT_PANEL);
    wxRibbonPanel* editExpListButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_EXPLANATION_LIST_PANEL);
    wxRibbonPanel* editReportButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_REPORT_PANEL);
    wxRibbonPanel* editStatsListButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_STATS_LIST_PANEL);
    wxRibbonPanel* editStatsReportButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_STATS_SUMMARY_REPORT_PANEL);
    wxRibbonPanel* editPlainLanguageGuideButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_PLAIN_LANGUAGE_GUIDE_PANEL);
    wxRibbonPanel* editSimpleListWithSummationButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_LIST_SIMPLE_WITH_SUM_PANEL);
    wxRibbonPanel* editSimpleListWithSummationAndExcludeButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_LIST_SIMPLE_WITH_SUM_AND_EXCLUDE_PANEL);
    wxRibbonPanel* editSimpleListButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_LIST_SIMPLE_PANEL);
    wxRibbonPanel* editBarChartButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_BAR_CHART_PANEL);
    wxRibbonPanel* editBoxPlotButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_BOX_PLOT_PANEL);
    wxRibbonPanel* editHistogramButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_HISTOGRAM_PANEL);
    wxRibbonPanel* editSyllableHistogramButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_SYLLABLE_HISTOGRAM_PANEL);
    wxRibbonPanel* editPieChartButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_PIE_CHART_PANEL);
    wxRibbonPanel* editWordCloudButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_WORDCLOUD_PANEL);
    wxRibbonPanel* editGraphButtonBarWindow = hideEditPanel(MainFrame::ID_EDIT_RIBBON_GRAPH_PANEL);
    wxRibbonPanel* editLixGermanButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_LIX_GERMAN_PANEL);
    wxRibbonPanel* editLixButtonBarWindow = hideEditPanel(MainFrame::ID_EDIT_RIBBON_LIX_PANEL);
    wxRibbonPanel* editRaygorButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_RAYGOR_PANEL);
    wxRibbonPanel* editFrySchwartzButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_FRY_PANEL);
    wxRibbonPanel* editFleschButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_FLESCH_PANEL);
    wxRibbonPanel* editDB2ButtonBarWindow = hideEditPanel(MainFrame::ID_EDIT_RIBBON_DB2_PANEL);
    wxRibbonPanel* editGeneralReadabilityButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_GENERAL_READABILITY_GRAPH_PANEL);
    // hide batch panels that we don't use here
    hideEditPanel(MainFrame::ID_EDIT_RIBBON_LIST_CSVSS_PANEL);
    hideEditPanel(MainFrame::ID_EDIT_RIBBON_LIST_TEST_SCORES_PANEL);
    hideEditPanel(MainFrame::ID_EDIT_RIBBON_HISTOGRAM_BATCH_PANEL);

    if (event.GetInt() == READABILITY_SCORES_PAGE_ID ||
        event.GetInt() == READABILITY_SCORES_SUMMARY_REPORT_PAGE_ID ||
        event.GetInt() == READABILITY_GOALS_PAGE_ID)
        {
        m_activeWindow = GetReadabilityResultsView().FindWindowById(event.GetInt());
        resetActiveCanvasResizeDelay();
        wxASSERT(m_activeWindow != nullptr);

        if (GetActiveProjectWindow() != nullptr)
            {
            GetSplitter()->GetWindow2()->Hide();
            GetSplitter()->ReplaceWindow(GetSplitter()->GetWindow2(), GetActiveProjectWindow());
            GetActiveProjectWindow()->Show();

            if (GetRibbon() != nullptr)
                {
                if (GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(wxHtmlWindow)) ||
                    GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(wxWebView)))
                    {
                    editSummaryReportButtonBarWindow->Show();
                    }
                else if (GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(ExplanationListCtrl)))
                    {
                    editExpListButtonBarWindow->Show();
                    getEditButtonBar(editExpListButtonBarWindow)
                        ->ToggleButton(XRCID("ID_LONG_FORMAT"),
                                       dynamic_cast<ProjectDoc*>(GetDocument())
                                           ->GetReadabilityMessageCatalog()
                                           .IsUsingLongGradeScaleFormat());
                    }
                else if (GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx)))
                    {
                    editListButtonBarWindow->Show();
                    }
                }
            }
        }
    else if (event.GetInt() == FLESCH_CHART_PAGE_ID || event.GetInt() == DB2_PAGE_ID ||
             event.GetInt() == FRY_PAGE_ID || event.GetInt() == RAYGOR_PAGE_ID ||
             event.GetInt() == CRAWFORD_GRAPH_PAGE_ID || event.GetInt() == FRASE_PAGE_ID ||
             event.GetInt() == INFLESZ_GRAPH_PAGE_ID || event.GetInt() == SCHWARTZ_PAGE_ID ||
             event.GetInt() == LIX_GAUGE_PAGE_ID || event.GetInt() == LIX_GAUGE_GERMAN_PAGE_ID ||
             event.GetInt() == GPM_FRY_PAGE_ID)
        {
        m_activeWindow = GetReadabilityResultsView().FindWindowById(event.GetInt());
        resetActiveCanvasResizeDelay();
        wxASSERT(m_activeWindow != nullptr);

        if (GetActiveProjectWindow() != nullptr)
            {
            GetSplitter()->GetWindow2()->Hide();
            GetSplitter()->ReplaceWindow(GetSplitter()->GetWindow2(), GetActiveProjectWindow());
            GetActiveProjectWindow()->Show();

            if (GetRibbon() != nullptr)
                {
                const auto graph =
                    dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())->GetFixedObject(0, 0);

                if (graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::LixGaugeGerman)))
                    {
                    editLixGermanButtonBarWindow->Show();
                    getEditButtonBar(editLixGermanButtonBarWindow)
                        ->ToggleButton(XRCID("ID_USE_ENGLISH_LABELS"),
                                       dynamic_cast<ProjectDoc*>(GetDocument())
                                           ->IsUsingEnglishLabelsForGermanLix());
                    getEditButtonBar(editLixGermanButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_EDIT_GRAPH_SHOWCASE_KEY_ITEMS"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsShowcasingKeyItems());
                    }
                else if (graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::LixGauge)))
                    {
                    editLixButtonBarWindow->Show();
                    getEditButtonBar(editLixButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_EDIT_GRAPH_SHOWCASE_KEY_ITEMS"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsShowcasingKeyItems());
                    }
                else if (graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::RaygorGraph)))
                    {
                    editRaygorButtonBarWindow->Show();
                    }
                else if (graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::FleschChart)))
                    {
                    editFleschButtonBarWindow->Show();
                    getEditButtonBar(editFleschButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_FLESCH_DISPLAY_LINES"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsConnectingFleschPoints());
                    }
                else if (graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::FryGraph)) ||
                         graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::SchwartzGraph)))
                    {
                    editFrySchwartzButtonBarWindow->Show();
                    getEditButtonBar(editFrySchwartzButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_EDIT_GRAPH_SHOWCASE_KEY_ITEMS"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsShowcasingKeyItems());
                    }
                else if (graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::DanielsonBryan2Plot)))
                    {
                    editDB2ButtonBarWindow->Show();
                    getEditButtonBar(editDB2ButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_EDIT_GRAPH_SHOWCASE_KEY_ITEMS"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsShowcasingKeyItems());
                    }
                else if (GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(Wisteria::Canvas)))
                    {
                    editGeneralReadabilityButtonBarWindow->Show();
                    }
                }
            }
        }
    else if (event.GetExtraLong() == SIDEBAR_SENTENCES_BREAKDOWN_SECTION_ID)
        {
        m_activeWindow = GetSentencesBreakdownView().FindWindowById(event.GetInt());
        resetActiveCanvasResizeDelay();
        wxASSERT(m_activeWindow != nullptr);

        if (GetActiveProjectWindow() != nullptr)
            {
            GetSplitter()->GetWindow2()->Hide();
            GetSplitter()->ReplaceWindow(GetSplitter()->GetWindow2(), GetActiveProjectWindow());
            GetActiveProjectWindow()->Show();

            if (GetRibbon() != nullptr)
                {
                if (GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(Wisteria::Canvas)) &&
                    dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())
                        ->GetFixedObject(0, 0)
                        ->IsKindOf(wxCLASSINFO(Wisteria::Graphs::BoxPlot)))
                    {
                    editBoxPlotButtonBarWindow->Show();
                    getEditButtonBar(editBoxPlotButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_BOX_PLOT_DISPLAY_ALL_POINTS"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsShowingAllBoxPlotPoints());
                    getEditButtonBar(editBoxPlotButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_BOX_PLOT_DISPLAY_LABELS"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingBoxPlotLabels());
                    getEditButtonBar(editBoxPlotButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_DROP_SHADOW"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingDropShadows());
                    }
                else if (GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(Wisteria::Canvas)) &&
                         dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())
                             ->GetFixedObject(0, 0)
                             ->IsKindOf(wxCLASSINFO(Wisteria::Graphs::Histogram)))
                    {
                    editHistogramButtonBarWindow->Show();
                    getEditButtonBar(editHistogramButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_DROP_SHADOW"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingDropShadows());
                    }
                else if (GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(Wisteria::Canvas)))
                    {
                    editGraphButtonBarWindow->Show();
                    getEditButtonBar(editGraphButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_DROP_SHADOW"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingDropShadows());
                    }
                else if (GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx)))
                    {
                    editListButtonBarWindow->Show();
                    }
                }
            }
        }
    else if (event.GetExtraLong() == SIDEBAR_STATS_SUMMARY_SECTION_ID)
        {
        m_activeWindow = GetSummaryView().FindWindowById(event.GetInt());
        resetActiveCanvasResizeDelay();
        wxASSERT(m_activeWindow != nullptr);

        if (GetActiveProjectWindow() != nullptr)
            {
            GetSplitter()->GetWindow2()->Hide();
            GetSplitter()->ReplaceWindow(GetSplitter()->GetWindow2(), GetActiveProjectWindow());
            GetActiveProjectWindow()->Show();

            if (GetRibbon() != nullptr)
                {
                if (GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx)))
                    {
                    editStatsListButtonBarWindow->Show();
                    }
                else if (GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(wxWebView)))
                    {
                    editStatsReportButtonBarWindow->Show();
                    }
                }
            }
        }
    else if (event.GetInt() == SIDEBAR_PLAIN_LANGUAGE_GUIDE_SECTION_ID)
        {
        m_activeWindow = GetPlainLanguageGuideView().GetWindow(0);
        resetActiveCanvasResizeDelay();
        wxASSERT(m_activeWindow != nullptr);

        if (GetActiveProjectWindow() != nullptr)
            {
            GetSplitter()->GetWindow2()->Hide();
            GetSplitter()->ReplaceWindow(GetSplitter()->GetWindow2(), GetActiveProjectWindow());
            GetActiveProjectWindow()->Show();

            if (GetRibbon() != nullptr &&
                GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(wxWebView)))
                {
                editPlainLanguageGuideButtonBarWindow->Show();
                }
            }
        }
    else if (event.GetExtraLong() == SIDEBAR_WORDS_BREAKDOWN_SECTION_ID)
        {
        // Note that word-list tests can have a list control and highlighted report
        // with the same integral ID, so rely on searching by the ID and name of the window.
        m_activeWindow =
            GetWordsBreakdownView().FindWindowByIdAndLabel(event.GetInt(), event.GetString());
        if (GetActiveProjectWindow() == nullptr)
            {
            m_activeWindow = GetWordsBreakdownView().FindWindowById(event.GetInt());
            }
        resetActiveCanvasResizeDelay();
        wxASSERT(m_activeWindow != nullptr);

        if (GetActiveProjectWindow() != nullptr)
            {
            GetSplitter()->GetWindow2()->Hide();
            GetSplitter()->ReplaceWindow(GetSplitter()->GetWindow2(), GetActiveProjectWindow());
            GetActiveProjectWindow()->Show();

            if (GetRibbon() != nullptr)
                {
                if (GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(Wisteria::Canvas)) &&
                    dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())
                        ->GetFixedObject(0, 0)
                        ->IsKindOf(wxCLASSINFO(Wisteria::Graphs::Histogram)))
                    {
                    editSyllableHistogramButtonBarWindow->Show();
                    getEditButtonBar(editSyllableHistogramButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_EDIT_GRAPH_SHOWCASE_KEY_ITEMS"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsShowcasingKeyItems());
                    getEditButtonBar(editSyllableHistogramButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_DROP_SHADOW"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingDropShadows());
                    }
                else if (GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(Wisteria::Canvas)) &&
                         dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())
                             ->GetFixedObject(0, 0)
                             ->IsKindOf(wxCLASSINFO(Wisteria::Graphs::BarChart)))
                    {
                    editBarChartButtonBarWindow->Show();
                    getEditButtonBar(editBarChartButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_DROP_SHADOW"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingDropShadows());
                    getEditButtonBar(editBarChartButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_EDIT_BAR_LABELS"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingBarChartLabels());
                    }
                else if (GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(Wisteria::Canvas)) &&
                         dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())
                             ->GetFixedObject(0, 0)
                             ->IsKindOf(wxCLASSINFO(Wisteria::Graphs::PieChart)))
                    {
                    editPieChartButtonBarWindow->Show();
                    getEditButtonBar(editPieChartButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_EDIT_GRAPH_SHOWCASE_KEY_ITEMS"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsShowcasingKeyItems());
                    }
                else if (GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(Wisteria::Canvas)) &&
                         dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())
                             ->GetFixedObject(0, 0)
                             ->IsKindOf(wxCLASSINFO(Wisteria::Graphs::WordCloud)))
                    {
                    editWordCloudButtonBarWindow->Show();
                    }
                else if (GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(Wisteria::Canvas)))
                    {
                    editGraphButtonBarWindow->Show();
                    getEditButtonBar(editGraphButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_DROP_SHADOW"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingDropShadows());
                    }
                else if (GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx)))
                    {
                    // compressed list of words combine stemmed words into a list,
                    // so it's not a list of individual words that a user can ignore
                    if (event.GetInt() == ALL_WORDS_CONDENSED_LIST_PAGE_ID)
                        {
                        editSimpleListWithSummationButtonBarWindow->Show();
                        }
                    else
                        {
                        editSimpleListWithSummationAndExcludeButtonBarWindow->Show();
                        }
                    }
                else if (GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(wxWebView)))
                    {
                    editReportButtonBarWindow->Show();
                    }
                }
            }
        }
    else if (event.GetExtraLong() == SIDEBAR_GRAMMAR_SECTION_ID)
        {
        m_activeWindow = GetGrammarView().FindWindowById(event.GetInt());
        resetActiveCanvasResizeDelay();
        wxASSERT(m_activeWindow != nullptr);

        if (GetActiveProjectWindow() != nullptr)
            {
            GetSplitter()->GetWindow2()->Hide();
            GetSplitter()->ReplaceWindow(GetSplitter()->GetWindow2(), GetActiveProjectWindow());
            GetActiveProjectWindow()->Show();

            if (GetRibbon() != nullptr)
                {
                if (GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(wxWebView)))
                    {
                    editReportButtonBarWindow->Show();
                    }
                else
                    {
                    if (event.GetInt() == OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID)
                        {
                        editListButtonBarWindow->Show();
                        }
                    else if (event.GetInt() == MISSPELLED_WORD_LIST_PAGE_ID ||
                             event.GetInt() == PASSIVE_VOICE_PAGE_ID ||
                             event.GetInt() == PROPER_NOUNS_LIST_PAGE_ID)
                        {
                        editSimpleListWithSummationButtonBarWindow->Show();
                        }
                    else
                        {
                        editSimpleListButtonBarWindow->Show();
                        }
                    }
                }
            }
        }
    else if (event.GetExtraLong() == SIDEBAR_DOLCH_SECTION_ID)
        {
        m_activeWindow = GetDolchSightWordsView().FindWindowById(event.GetInt());
        resetActiveCanvasResizeDelay();
        wxASSERT(m_activeWindow != nullptr);

        if (GetActiveProjectWindow() != nullptr)
            {
            GetSplitter()->GetWindow2()->Hide();
            GetSplitter()->ReplaceWindow(GetSplitter()->GetWindow2(), GetActiveProjectWindow());
            GetActiveProjectWindow()->Show();
            if (GetMenuBar() != nullptr)
                {
                MenuBarEnableAll(GetMenuBar(), wxID_SELECTALL, true);
                }
            if (GetRibbon() != nullptr)
                {
                if (IsHighlightedTextWindow(GetActiveProjectWindow()))
                    {
                    editReportButtonBarWindow->Show();
                    }
                else if (GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(wxWebView)))
                    {
                    editStatsReportButtonBarWindow->Show();
                    }
                else if (GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx)))
                    {
                    if (event.GetInt() == NON_DOLCH_WORDS_LIST_PAGE_ID ||
                        event.GetInt() == DOLCH_WORDS_LIST_PAGE_ID)
                        {
                        editSimpleListWithSummationButtonBarWindow->Show();
                        }
                    else
                        {
                        editSimpleListButtonBarWindow->Show();
                        }
                    }
                else if (GetActiveProjectWindow()->IsKindOf(wxCLASSINFO(Wisteria::Canvas)))
                    {
                    editBarChartButtonBarWindow->Show();
                    getEditButtonBar(editBarChartButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_DROP_SHADOW"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingDropShadows());
                    getEditButtonBar(editBarChartButtonBarWindow)
                        ->ToggleButton(
                            XRCID("ID_EDIT_BAR_LABELS"),
                            dynamic_cast<ProjectDoc*>(GetDocument())->IsDisplayingBarChartLabels());
                    }
                }
            }
        }

    // add the label for the window type to the export menu item
    wxASSERT(GetActiveProjectWindow());
    if (auto* exportMenuItem{ GetDocFrame()->m_exportMenu.FindChildItem(XRCID("ID_SAVE_ITEM")) };
        exportMenuItem != nullptr && GetActiveProjectWindow() != nullptr)
        {
        exportMenuItem->SetItemLabel(
            wxString::Format(_(L"Export %s..."), GetActiveProjectWindow()->GetName()));
        }
    if (GetMenuBar() != nullptr)
        {
        GetMenuBar()->SetLabel(
            XRCID("ID_SAVE_ITEM"),
            wxString::Format(_(L"Export %s..."), GetActiveProjectWindow()->GetName()));
        MenuBarEnableAll(GetMenuBar(), wxID_SELECTALL, true);
        }

    GetRibbon()->Realize();
    GetRibbon()->Layout();

    event.Skip();
    }

//-------------------------------------------------------
bool ProjectView::ExportAll(const wxString& folder, wxString listExt, wxString textExt,
                            wxString summaryReportExt, wxString graphExt,
                            const bool includeWordsBreakdown, const bool includeSentencesBreakdown,
                            const bool includeTestScores, const bool includeStatistics,
                            const bool includeGrammar, const bool includePlainLanguageGuide,
                            const bool includeSightWords, const bool includeLists,
                            const bool includeTextReports,
                            const Wisteria::UI::ImageExportOptions& graphOptions)
    {
    const auto* doc = dynamic_cast<const ProjectDoc*>(GetDocument());

    if (!wxFileName::DirExists(folder))
        {
        if (folder.empty() || !wxFileName::Mkdir(folder, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            return false;
            }
        }
    // validate the extensions
    if (listExt.empty())
        {
        listExt = L".htm";
        }
    else if (listExt[0] != L'.')
        {
        listExt.insert(0, L".");
        }

    if (textExt.empty())
        {
        textExt = L".htm";
        }
    else if (textExt[0] != L'.')
        {
        textExt.insert(0, L".");
        }

    if (summaryReportExt.empty())
        {
        summaryReportExt = L".htm";
        }
    else if (summaryReportExt[0] != L'.')
        {
        summaryReportExt.insert(0, L".");
        }

    if (graphExt.empty())
        {
        graphExt = L".png";
        }
    else if (graphExt[0] != L'.')
        {
        graphExt.insert(0, L".");
        }

    const BaseProjectProcessingLock processingLock(dynamic_cast<ProjectDoc*>(GetDocument()));

    const wxBusyCursor bc;
    const wxBusyInfo bi(wxBusyInfoFlags().Text(_(L"Exporting project...")).Parent(GetDocFrame()));
#ifdef __WXGTK__
    wxMilliSleep(100);
    wxGetApp().Yield();
#endif

    // the results window
    if (includeTestScores)
        {
        if (!wxFileName::Mkdir(folder + wxFileName::GetPathSeparator() +
                                   _DT(L"Readability Scores", DTExplanation::FilePath),
                               wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            wxMessageBox(
                wxString::Format(_(L"Unable to create \"%s\" folder."), _DT(L"Readability Scores")),
                wxGetApp().GetAppName(), wxOK | wxICON_EXCLAMATION);
            }
        else
            {
            for (size_t i = 0; i < GetReadabilityResultsView().GetWindowCount(); ++i)
                {
                wxWindow* activeWindow = GetReadabilityResultsView().GetWindow(i);
                if (activeWindow != nullptr)
                    {
                    if (activeWindow->IsKindOf(wxCLASSINFO(ExplanationListCtrl)))
                        {
                        auto* list = dynamic_cast<ExplanationListCtrl*>(activeWindow);
                        list->SetLabel(
                            wxString::Format(L"%s [%s]", list->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        const wxString savePath =
                            folder + wxFileName::GetPathSeparator() + _DT(L"Readability Scores") +
                            wxFileName::GetPathSeparator() + list->GetLabel() + L".htm";
                        if (!list->Save(savePath, ExplanationListExportOptions::ExportGrid))
                            {
                            wxLogError(L"Failed to save list: (%s).", savePath);
                            }
                        }
                    else if (activeWindow->IsKindOf(wxCLASSINFO(Wisteria::Canvas)))
                        {
                        auto* graphWindow = dynamic_cast<Wisteria::Canvas*>(activeWindow);
                        graphWindow->SetLabel(
                            wxString::Format(L"%s [%s]", graphWindow->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        const wxString savePath =
                            folder + wxFileName::GetPathSeparator() + _DT(L"Readability Scores") +
                            wxFileName::GetPathSeparator() + graphWindow->GetLabel() + graphExt;
                        if (!graphWindow->Save(savePath, graphOptions))
                            {
                            wxLogError(L"Failed to save graph: (%s).", savePath);
                            }
                        }
                    else if (activeWindow->IsKindOf(wxCLASSINFO(wxWebView)))
                        {
                        auto* webview = dynamic_cast<wxWebView*>(activeWindow);
                        webview->SetLabel(
                            wxString::Format(L"%s [%s]", webview->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        const wxString savePathNoExt =
                            folder + wxFileName::GetPathSeparator() + _DT(L"Readability Scores") +
                            wxFileName::GetPathSeparator() + webview->GetLabel();
                        SaveWebViewReport(webview, savePathNoExt, summaryReportExt);
                        }
                    else if (activeWindow->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx)))
                        {
                        auto* list = dynamic_cast<Wisteria::UI::ListCtrlEx*>(activeWindow);
                        list->SetLabel(
                            wxString::Format(L"%s [%s]", list->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        const wxString savePath =
                            folder + wxFileName::GetPathSeparator() + _DT(L"Readability Scores") +
                            wxFileName::GetPathSeparator() + list->GetLabel() + listExt;
                        if (!list->Save(savePath, Wisteria::UI::GridExportOptions()))
                            {
                            wxLogError(L"Failed to save list: (%s).", savePath);
                            }
                        }
                    }
                }
            }
        }
    // the statistics
    if (includeStatistics && (GetSummaryView().GetWindowCount() != 0U))
        {
        if (!wxFileName::Mkdir(folder + wxFileName::GetPathSeparator() + _DT(L"Summary Statistics"),
                               wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            wxMessageBox(
                wxString::Format(_(L"Unable to create \"%s\" folder."), _DT(L"Summary Statistics")),
                wxGetApp().GetAppName(), wxOK | wxICON_EXCLAMATION);
            }
        else
            {
            for (size_t i = 0; i < GetSummaryView().GetWindowCount(); ++i)
                {
                wxWindow* activeWindow = GetSummaryView().GetWindow(i);
                if (activeWindow != nullptr)
                    {
                    if (activeWindow->IsKindOf(wxCLASSINFO(wxWebView)))
                        {
                        auto* webview = dynamic_cast<wxWebView*>(activeWindow);
                        webview->SetLabel(
                            wxString::Format(L"%s [%s]", webview->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        const wxString savePathNoExt =
                            folder + wxFileName::GetPathSeparator() + _DT(L"Summary Statistics") +
                            wxFileName::GetPathSeparator() + webview->GetLabel();
                        SaveWebViewReport(webview, savePathNoExt, summaryReportExt);
                        }
                    else if (activeWindow->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx)) &&
                             includeLists)
                        {
                        auto* list = dynamic_cast<Wisteria::UI::ListCtrlEx*>(activeWindow);
                        list->SetLabel(
                            wxString::Format(L"%s [%s]", list->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        const wxString savePath =
                            folder + wxFileName::GetPathSeparator() + _DT(L"Summary Statistics") +
                            wxFileName::GetPathSeparator() + list->GetLabel() + listExt;
                        if (!list->Save(savePath, Wisteria::UI::GridExportOptions()))
                            {
                            wxLogError(L"Failed to save list: (%s).", savePath);
                            }
                        }
                    else if (activeWindow->IsKindOf(wxCLASSINFO(Wisteria::Canvas)))
                        {
                        auto* graphWindow = dynamic_cast<Wisteria::Canvas*>(activeWindow);
                        graphWindow->SetLabel(
                            wxString::Format(L"%s [%s]", graphWindow->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        const wxString savePath =
                            folder + wxFileName::GetPathSeparator() + _DT(L"Summary Statistics") +
                            wxFileName::GetPathSeparator() + graphWindow->GetLabel() + graphExt;
                        if (!graphWindow->Save(savePath, graphOptions))
                            {
                            wxLogError(L"Failed to save graph: (%s).", savePath);
                            }
                        }
                    }
                }
            }
        }
    if (includeSentencesBreakdown && (GetSentencesBreakdownView().GetWindowCount() != 0U))
        {
        if (!wxFileName::Mkdir(folder + wxFileName::GetPathSeparator() +
                                   _DT(L"Sentences Breakdown"),
                               wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            wxMessageBox(wxString::Format(_(L"Unable to create \"%s\" folder."),
                                          _DT(L"Sentences Breakdown")),
                         wxGetApp().GetAppName(), wxOK | wxICON_EXCLAMATION);
            }
        else
            {
            for (size_t i = 0; i < GetSentencesBreakdownView().GetWindowCount(); ++i)
                {
                wxWindow* activeWindow = GetSentencesBreakdownView().GetWindow(i);
                if (activeWindow != nullptr)
                    {
                    if (activeWindow->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx)) &&
                        includeLists)
                        {
                        auto* list = dynamic_cast<Wisteria::UI::ListCtrlEx*>(activeWindow);
                        list->SetLabel(
                            wxString::Format(L"%s [%s]", list->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        const wxString savePath =
                            folder + wxFileName::GetPathSeparator() + _DT(L"Sentences Breakdown") +
                            wxFileName::GetPathSeparator() + list->GetLabel() + listExt;
                        if (!list->Save(savePath, Wisteria::UI::GridExportOptions()))
                            {
                            wxLogError(L"Failed to save list: (%s).", savePath);
                            }
                        }
                    else if (activeWindow->IsKindOf(wxCLASSINFO(Wisteria::Canvas)))
                        {
                        auto* graphWindow = dynamic_cast<Wisteria::Canvas*>(activeWindow);
                        graphWindow->SetLabel(
                            wxString::Format(L"%s [%s]", graphWindow->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        const wxString savePath =
                            folder + wxFileName::GetPathSeparator() + _DT(L"Sentences Breakdown") +
                            wxFileName::GetPathSeparator() + graphWindow->GetLabel() + graphExt;
                        if (!graphWindow->Save(savePath, graphOptions))
                            {
                            wxLogError(L"Failed to save graph: (%s).", savePath);
                            }
                        }
                    }
                }
            }
        }
    // the words breakdown section
    if (includeWordsBreakdown && (GetWordsBreakdownView().GetWindowCount() != 0U))
        {
        if (!wxFileName::Mkdir(folder + wxFileName::GetPathSeparator() + _DT(L"Words Breakdown"),
                               wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            wxMessageBox(
                wxString::Format(_(L"Unable to create \"%s\" folder."), _DT(L"Words Breakdown")),
                wxGetApp().GetAppName(), wxOK | wxICON_EXCLAMATION);
            }
        else
            {
            for (size_t i = 0; i < GetWordsBreakdownView().GetWindowCount(); ++i)
                {
                wxWindow* activeWindow = GetWordsBreakdownView().GetWindow(i);
                if (activeWindow != nullptr)
                    {
                    if (activeWindow->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx)) &&
                        includeLists)
                        {
                        auto* list = dynamic_cast<Wisteria::UI::ListCtrlEx*>(activeWindow);
                        list->SetLabel(
                            wxString::Format(L"%s [%s]", list->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        list->Save(folder + wxFileName::GetPathSeparator() +
                                       _DT(L"Words Breakdown") + wxFileName::GetPathSeparator() +
                                       list->GetLabel() + listExt,
                                   Wisteria::UI::GridExportOptions());
                        }
                    else if (activeWindow->IsKindOf(wxCLASSINFO(Wisteria::Canvas)))
                        {
                        auto* graphWindow = dynamic_cast<Wisteria::Canvas*>(activeWindow);
                        graphWindow->SetLabel(
                            wxString::Format(L"%s [%s]", graphWindow->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        graphWindow->Save(
                            folder + wxFileName::GetPathSeparator() + _DT(L"Words Breakdown") +
                                wxFileName::GetPathSeparator() + graphWindow->GetLabel() + graphExt,
                            graphOptions);
                        }
                    else if (activeWindow->IsKindOf(wxCLASSINFO(wxWebView)) && includeTextReports)
                        {
                        auto* webview = dynamic_cast<wxWebView*>(activeWindow);
                        webview->SetLabel(
                            wxString::Format(L"%s [%s]", webview->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        const wxString savePathNoExt =
                            folder + wxFileName::GetPathSeparator() + _DT(L"Words Breakdown") +
                            wxFileName::GetPathSeparator() + webview->GetLabel();
                        SaveWebViewReport(webview, savePathNoExt, textExt);
                        }
                    }
                }
            }
        }
    // grammar
    if (includeGrammar && (GetGrammarView().GetWindowCount() != 0U))
        {
        if (!wxFileName::Mkdir(folder + wxFileName::GetPathSeparator() + _DT(L"Grammar"),
                               wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            wxMessageBox(wxString::Format(_(L"Unable to create \"%s\" folder."), _DT(L"Grammar")),
                         wxGetApp().GetAppName(), wxOK | wxICON_EXCLAMATION);
            }
        else
            {
            for (size_t i = 0; i < GetGrammarView().GetWindowCount(); ++i)
                {
                wxWindow* activeWindow = GetGrammarView().GetWindow(i);
                if (activeWindow != nullptr)
                    {
                    if (activeWindow->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx)) &&
                        includeLists)
                        {
                        auto* list = dynamic_cast<Wisteria::UI::ListCtrlEx*>(activeWindow);
                        list->SetLabel(
                            wxString::Format(L"%s [%s]", list->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        list->Save(folder + wxFileName::GetPathSeparator() + _DT(L"Grammar") +
                                       wxFileName::GetPathSeparator() + list->GetLabel() + listExt,
                                   Wisteria::UI::GridExportOptions());
                        }
                    else if (activeWindow->IsKindOf(wxCLASSINFO(wxWebView)) && includeTextReports)
                        {
                        auto* webview = dynamic_cast<wxWebView*>(activeWindow);
                        webview->SetLabel(
                            wxString::Format(L"%s [%s]", webview->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        const wxString savePathNoExt =
                            folder + wxFileName::GetPathSeparator() + _DT(L"Grammar") +
                            wxFileName::GetPathSeparator() + webview->GetLabel();
                        SaveWebViewReport(webview, savePathNoExt, textExt);
                        }
                    }
                }
            }
        }
    // Plain Language Guide
    if (includePlainLanguageGuide && (GetPlainLanguageGuideView().GetWindowCount() != 0U))
        {
        if (!wxFileName::Mkdir(folder + wxFileName::GetPathSeparator() +
                                   BaseProjectView::GetPlainLanguageGuideLabel(),
                               wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            wxMessageBox(wxString::Format(_(L"Unable to create \"%s\" folder."),
                                          BaseProjectView::GetPlainLanguageGuideLabel()),
                         wxGetApp().GetAppName(), wxOK | wxICON_EXCLAMATION);
            }
        else
            {
            for (size_t i = 0; i < GetPlainLanguageGuideView().GetWindowCount(); ++i)
                {
                wxWindow* activeWindow = GetPlainLanguageGuideView().GetWindow(i);
                if (activeWindow != nullptr && activeWindow->IsKindOf(wxCLASSINFO(wxWebView)) &&
                    includeTextReports)
                    {
                    auto* webview = dynamic_cast<wxWebView*>(activeWindow);
                    webview->SetLabel(
                        wxString::Format(L"%s [%s]", webview->GetName(),
                                         wxFileName::StripExtension(doc->GetTitle())));
                    const wxString savePathNoExt = folder + wxFileName::GetPathSeparator() +
                                                   BaseProjectView::GetPlainLanguageGuideLabel() +
                                                   wxFileName::GetPathSeparator() +
                                                   webview->GetLabel();
                    SaveWebViewReport(webview, savePathNoExt, textExt);
                    }
                }
            }
        }
    // Sight Words
    if (includeSightWords && (GetDolchSightWordsView().GetWindowCount() != 0U))
        {
        if (!wxFileName::Mkdir(folder + wxFileName::GetPathSeparator() + _DT(L"Sight Words"),
                               wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            wxMessageBox(
                wxString::Format(_(L"Unable to create \"%s\" folder."), _DT(L"Sight Words")),
                wxGetApp().GetAppName(), wxOK | wxICON_EXCLAMATION);
            }
        else
            {
            for (size_t i = 0; i < GetDolchSightWordsView().GetWindowCount(); ++i)
                {
                wxWindow* activeWindow = GetDolchSightWordsView().GetWindow(i);
                if (activeWindow != nullptr)
                    {
                    if (activeWindow->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx)) &&
                        includeLists)
                        {
                        auto* list = dynamic_cast<Wisteria::UI::ListCtrlEx*>(activeWindow);
                        list->SetLabel(
                            wxString::Format(L"%s [%s]", list->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        list->Save(folder + wxFileName::GetPathSeparator() + _DT(L"Sight Words") +
                                       wxFileName::GetPathSeparator() + list->GetLabel() + listExt,
                                   Wisteria::UI::GridExportOptions());
                        }
                    // this section mixes highlighted-text windows with the Dolch summary
                    // report, and they follow different export formats
                    else if (IsHighlightedTextWindow(activeWindow) && includeTextReports)
                        {
                        auto* webview = dynamic_cast<wxWebView*>(activeWindow);
                        webview->SetLabel(
                            wxString::Format(L"%s [%s]", webview->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        const wxString savePathNoExt =
                            folder + wxFileName::GetPathSeparator() + _DT(L"Sight Words") +
                            wxFileName::GetPathSeparator() + webview->GetLabel();
                        SaveWebViewReport(webview, savePathNoExt, textExt);
                        }
                    else if (!IsHighlightedTextWindow(activeWindow) &&
                             activeWindow->IsKindOf(wxCLASSINFO(wxWebView)))
                        {
                        auto* webview = dynamic_cast<wxWebView*>(activeWindow);
                        webview->SetLabel(
                            wxString::Format(L"%s [%s]", webview->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        const wxString savePathNoExt =
                            folder + wxFileName::GetPathSeparator() + _DT(L"Sight Words") +
                            wxFileName::GetPathSeparator() + webview->GetLabel();
                        SaveWebViewReport(webview, savePathNoExt, summaryReportExt);
                        }
                    else if (activeWindow->IsKindOf(wxCLASSINFO(Wisteria::Canvas)))
                        {
                        auto* graph = dynamic_cast<Wisteria::Canvas*>(activeWindow);
                        graph->SetLabel(
                            wxString::Format(L"%s [%s]", graph->GetName(),
                                             wxFileName::StripExtension(doc->GetTitle())));
                        graph->Save(folder + wxFileName::GetPathSeparator() + _DT(L"Sight Words") +
                                        wxFileName::GetPathSeparator() + graph->GetLabel() +
                                        graphExt,
                                    graphOptions);
                        }
                    }
                }
            }
        }
    return true;
    }

//-------------------------------------------------------
bool ProjectView::ExportAllToHtml(const wxFileName& filePath, wxString graphExt,
                                  const bool includeWordsBreakdown,
                                  const bool includeSentencesBreakdown,
                                  const bool includeTestScores, const bool includeStatistics,
                                  const bool includeGrammar, const bool includePlainLanguageGuide,
                                  const bool includeSightWords, const bool includeLists,
                                  const bool includeTextReports,
                                  const Wisteria::UI::ImageExportOptions& graphOptions)
    {
    if (filePath.GetPath().empty())
        {
        return false;
        }
    const auto* doc = dynamic_cast<const ProjectDoc*>(GetDocument());

    if (!wxFileName::DirExists(filePath.GetPathWithSep() + _DT(L"images")))
        {
        if (!wxFileName::Mkdir(filePath.GetPathWithSep() + _DT(L"images"), wxS_DIR_DEFAULT,
                               wxPATH_MKDIR_FULL))
            {
            return false;
            }
        }
    // validate the extension
    if (graphExt.empty())
        {
        graphExt = L".png";
        }
    else if (graphExt[0] != L'.')
        {
        graphExt.insert(0, L".");
        }

    const BaseProjectProcessingLock processingLock(dynamic_cast<ProjectDoc*>(GetDocument()));

    const wxBusyCursor bc;
    const wxBusyInfo bi(wxBusyInfoFlags().Text(_(L"Exporting project...")).Parent(GetDocFrame()));
#ifdef __WXGTK__
    wxMilliSleep(100);
    wxGetApp().Yield();
#endif

    lily_of_the_valley::html_encode_text htmlEncode;
    wxString outputText, textWindowStyleSection;
    // the stylesheet is embedded (see below) rather than linked, so the report is a single
    // self-contained file; the closing </head> is added once the style block is in place
    const wxString headSection =
        L"<head>" +
        wxString::Format(
            L"\n    <meta name='generator' content='%s %s' />"
            "\n    <title>%s</title>"
            "\n    <meta http-equiv='content-type' content='text/html; charset=utf-8' />"
            "\n    <meta name='color-scheme' content='light dark' />",
            wxGetApp().GetAppDisplayName(), wxGetApp().GetAppVersion(), doc->GetTitle());

    size_t sectionCounter = 0;
    size_t figureCounter = 0;
    size_t tableCounter = 0;

    const wxString pageBreak = L"<div class='page-break'></div><br />\n";

    const auto formatImageOutput =
        [&outputText, &sectionCounter, &figureCounter, pageBreak, doc, htmlEncode, graphExt,
         graphOptions, filePath](Wisteria::Canvas* canvas, const bool includeLeadingPageBreak)
    {
        if (canvas == nullptr)
            {
            return;
            }
        canvas->SetLabel(wxString::Format(L"%s [%s]", canvas->GetName(),
                                          wxFileName::StripExtension(doc->GetTitle())));
        canvas->Save(filePath.GetPathWithSep() + _DT(L"images") + wxFileName::GetPathSeparator() +
                         canvas->GetLabel() + graphExt,
                     graphOptions);

        outputText += wxString::Format(
            L"%s\n<div class='minipage figure'>\n<img src='images\\%s' />\n"
            "<div class='caption'>%s</div>\n</div>\n",
            (includeLeadingPageBreak ? pageBreak : wxString{}), canvas->GetLabel() + graphExt,
            wxString::Format(_(L"Figure %zu.%zu: %s"), sectionCounter, figureCounter++,
                             htmlEncode({ canvas->GetName().wc_str() }, true).c_str()));
    };

    const auto formatList =
        [&doc, &outputText, &htmlEncode, &sectionCounter, &tableCounter,
         pageBreak](Wisteria::UI::ListCtrlEx* list, const bool includeLeadingPageBreak)
    {
        if (list == nullptr)
            {
            return;
            }

        doc->UpdateListOptions(list);
        wxString buffer;
        list->FormatToHtml(
            buffer, true, Wisteria::UI::ListCtrlEx::ExportRowSelection::ExportAll, 0, -1, 0, -1,
            true, false,
            wxString::Format(_(L"Table %zu.%zu: %s"), sectionCounter, tableCounter++,
                             htmlEncode({ list->GetLabel().wc_str() }, true).c_str()));
        lily_of_the_valley::html_format::strip_hyperlinks(buffer);

        outputText += (includeLeadingPageBreak ? pageBreak : wxString{}) +
                      lily_of_the_valley::html_extract_text::get_body(buffer.ToStdWstring());
    };

    bool highlightStylesCaptured{ false };
    const auto formatWebViewReport =
        [this, &outputText, &htmlEncode, &textWindowStyleSection, &highlightStylesCaptured,
         pageBreak](wxWebView* webview, const bool includeLeadingPageBreak)
    {
        if (webview == nullptr)
            {
            return;
            }
        wxString pageSource = webview->GetPageSource();
        ProjectReportFormat::StripBackToTopButton(pageSource);
        std::wstring htmlText = pageSource.ToStdWstring();
        lily_of_the_valley::html_format::strip_hyperlinks(htmlText);
        // a highlighted-text window emits its .hl-* styles as the first <style> block (ahead
        // of the theme CSS); collect them once into the shared stylesheet so the combined
        // report renders the highlighting. every such window in a project shares the same
        // project-wide colors and full category set, so one capture covers them all. the
        // other report webviews rely on the theme CSS in default.css
        if (!highlightStylesCaptured && IsHighlightedTextWindow(webview))
            {
            textWindowStyleSection +=
                L"\n" +
                wxString{ lily_of_the_valley::html_extract_text::get_style_section(htmlText) };
            highlightStylesCaptured = true;
            }
        htmlText = lily_of_the_valley::html_extract_text::get_body(htmlText);
        outputText +=
            wxString::Format(L"\n%s<div class='caption'>%s</div>\n%s\n",
                             (includeLeadingPageBreak ? pageBreak : wxString{}),
                             htmlEncode({ webview->GetName().wc_str() }, true).c_str(), htmlText);
    };

    bool hasSections{ false };

    // scores section
    if (includeTestScores && (GetReadabilityResultsView().GetWindowCount() != 0U))
        {
        // the first output in this section will not have a leading page break, but the rest will
        bool includeLeadingPageBreak{ false };
        // indicates that a section has already been written out after the TOC so that we
        // know if we need to insert a page break in front of the next section
        hasSections = true;
        // update/reset counters for sections, tables, and figures
        ++sectionCounter;
        figureCounter = tableCounter = 1;
        outputText += wxString::Format(
            _DT(L"\n\n<details class='report-section' open><summary><a name='scores'></a><span "
                L"class='n'>%02zu</span>%s</summary>\n"),
            sectionCounter, htmlEncode({ GetReadabilityScoresLabel().wc_str() }, true).c_str());
        for (size_t i = 0; i < GetReadabilityResultsView().GetWindowCount(); ++i)
            {
            wxWindow* activeWindow = GetReadabilityResultsView().GetWindow(i);
            if (activeWindow != nullptr)
                {
                if (activeWindow->IsKindOf(wxCLASSINFO(ExplanationListCtrl)))
                    {
                    auto* resultsList =
                        dynamic_cast<ExplanationListCtrl*>(activeWindow)->GetResultsListCtrl();
                    resultsList->SetLabel(GetReadabilityScoresLabel());
                    formatList(resultsList, includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(wxCLASSINFO(Wisteria::Canvas)))
                    {
                    formatImageOutput(dynamic_cast<Wisteria::Canvas*>(activeWindow),
                                      includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(wxCLASSINFO(wxWebView)))
                    {
                    formatWebViewReport(dynamic_cast<wxWebView*>(activeWindow),
                                        includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx)))
                    {
                    formatList(dynamic_cast<Wisteria::UI::ListCtrlEx*>(activeWindow),
                               includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                }
            }
        outputText += L"\n</details>\n";
        }
    // the statistics
    if (includeStatistics && (GetSummaryView().GetWindowCount() != 0U))
        {
        bool includeLeadingPageBreak{ false };
        ++sectionCounter;
        figureCounter = tableCounter = 1;
        outputText += wxString::Format(
            _DT(L"\n\n%s<details class='report-section' open><summary><a name='stats'></a><span "
                L"class='n'>%02zu</span>%s</summary>\n"),
            (hasSections ? pageBreak : wxString{}), sectionCounter,
            htmlEncode({ GetSummaryStatisticsLabel().wc_str() }, true).c_str());
        hasSections = true;
        for (size_t i = 0; i < GetSummaryView().GetWindowCount(); ++i)
            {
            wxWindow* activeWindow = GetSummaryView().GetWindow(i);
            if (activeWindow != nullptr)
                {
                if (activeWindow->IsKindOf(wxCLASSINFO(wxWebView)))
                    {
                    formatWebViewReport(dynamic_cast<wxWebView*>(activeWindow),
                                        includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx)) &&
                         includeLists)
                    {
                    formatList(dynamic_cast<Wisteria::UI::ListCtrlEx*>(activeWindow),
                               includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(wxCLASSINFO(Wisteria::Canvas)))
                    {
                    formatImageOutput(dynamic_cast<Wisteria::Canvas*>(activeWindow),
                                      includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                }
            }
        outputText += L"\n</details>\n";
        }
    // words breakdown section
    if (includeWordsBreakdown && (GetWordsBreakdownView().GetWindowCount() != 0U))
        {
        bool includeLeadingPageBreak{ false };
        ++sectionCounter;
        figureCounter = tableCounter = 1;
        outputText +=
            wxString::Format(_DT(L"\n\n%s<details class='report-section' open>"
                                 "<summary><a name='wordsbreakdown'></a>"
                                 "<span class='n'>%02zu</span>%s</summary>\n"),
                             (hasSections ? pageBreak : wxString{}), sectionCounter,
                             htmlEncode({ GetWordsBreakdownLabel().wc_str() }, true).c_str());
        hasSections = true;
        for (size_t i = 0; i < GetWordsBreakdownView().GetWindowCount(); ++i)
            {
            wxWindow* activeWindow = GetWordsBreakdownView().GetWindow(i);
            if (activeWindow != nullptr)
                {
                if (activeWindow->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx)) && includeLists)
                    {
                    formatList(dynamic_cast<Wisteria::UI::ListCtrlEx*>(activeWindow),
                               includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(wxCLASSINFO(Wisteria::Canvas)))
                    {
                    formatImageOutput(dynamic_cast<Wisteria::Canvas*>(activeWindow),
                                      includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(wxCLASSINFO(wxWebView)) && includeTextReports)
                    {
                    formatWebViewReport(dynamic_cast<wxWebView*>(activeWindow),
                                        includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                }
            }
        outputText += L"\n</details>\n";
        }
    // sentence section
    if (includeSentencesBreakdown && (GetSentencesBreakdownView().GetWindowCount() != 0U))
        {
        bool includeLeadingPageBreak{ false };
        ++sectionCounter;
        figureCounter = tableCounter = 1;
        outputText +=
            wxString::Format(_DT(L"\n\n%s<details class='report-section' open>"
                                 "<summary><a name='sentencesbreakdown'></a>"
                                 "<span class='n'>%02zu</span>%s</summary>\n"),
                             (hasSections ? pageBreak : wxString{}), sectionCounter,
                             htmlEncode({ GetSentencesBreakdownLabel().wc_str() }, true).c_str());
        hasSections = true;
        for (size_t i = 0; i < GetSentencesBreakdownView().GetWindowCount(); ++i)
            {
            wxWindow* activeWindow = GetSentencesBreakdownView().GetWindow(i);
            if (activeWindow != nullptr)
                {
                if (activeWindow->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx)) && includeLists)
                    {
                    formatList(dynamic_cast<Wisteria::UI::ListCtrlEx*>(activeWindow),
                               includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(wxCLASSINFO(Wisteria::Canvas)))
                    {
                    formatImageOutput(dynamic_cast<Wisteria::Canvas*>(activeWindow),
                                      includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                }
            }
        outputText += L"\n</details>\n";
        }
    // grammar section
    if (includeGrammar && (includeLists || includeTextReports) &&
        (GetGrammarView().GetWindowCount() != 0U))
        {
        bool includeLeadingPageBreak{ false };
        ++sectionCounter;
        figureCounter = tableCounter = 1;
        outputText += wxString::Format(_DT(L"\n\n%s<details class='report-section' open>"
                                           "<summary><a name='grammar'></a>"
                                           "<span class='n'>%02zu</span>%s</summary>\n"),
                                       (hasSections ? pageBreak : wxString{}), sectionCounter,
                                       htmlEncode({ GetGrammarLabel().wc_str() }, true).c_str());
        hasSections = true;
        for (size_t i = 0; i < GetGrammarView().GetWindowCount(); ++i)
            {
            wxWindow* activeWindow = GetGrammarView().GetWindow(i);
            if (activeWindow != nullptr)
                {
                if (activeWindow->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx)) && includeLists)
                    {
                    formatList(dynamic_cast<Wisteria::UI::ListCtrlEx*>(activeWindow),
                               includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(wxCLASSINFO(wxWebView)) && includeTextReports)
                    {
                    formatWebViewReport(dynamic_cast<wxWebView*>(activeWindow),
                                        includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                }
            }
        outputText += L"\n</details>\n";
        }
    // Plain Language Guide section
    if (includePlainLanguageGuide && includeTextReports &&
        (GetPlainLanguageGuideView().GetWindowCount() != 0U))
        {
        bool includeLeadingPageBreak{ false };
        ++sectionCounter;
        figureCounter = tableCounter = 1;
        outputText +=
            wxString::Format(_DT(L"\n\n%s<details class='report-section' open>"
                                 "<summary><a name='plainlanguageguide'></a>"
                                 "<span class='n'>%02zu</span>%s</summary>\n"),
                             (hasSections ? pageBreak : wxString{}), sectionCounter,
                             htmlEncode({ GetPlainLanguageGuideLabel().wc_str() }, true).c_str());
        hasSections = true;
        for (size_t i = 0; i < GetPlainLanguageGuideView().GetWindowCount(); ++i)
            {
            wxWindow* activeWindow = GetPlainLanguageGuideView().GetWindow(i);
            if (activeWindow != nullptr && activeWindow->IsKindOf(wxCLASSINFO(wxWebView)))
                {
                formatWebViewReport(dynamic_cast<wxWebView*>(activeWindow),
                                    includeLeadingPageBreak);
                includeLeadingPageBreak = true;
                }
            }
        outputText += L"\n</details>\n";
        }
    // Sight Words
    if (includeSightWords && (GetDolchSightWordsView().GetWindowCount() != 0U))
        {
        bool includeLeadingPageBreak{ false };
        ++sectionCounter;
        figureCounter = tableCounter = 1;
        outputText += wxString::Format(_DT(L"\n\n%s<details class='report-section' open>"
                                           "<summary><a name='dolch'></a>"
                                           "<span class='n'>%02zu</span>%s</summary>\n"),
                                       (hasSections ? pageBreak : wxString{}), sectionCounter,
                                       htmlEncode({ GetDolchLabel().wc_str() }, true).c_str());
        for (size_t i = 0; i < GetDolchSightWordsView().GetWindowCount(); ++i)
            {
            wxWindow* activeWindow = GetDolchSightWordsView().GetWindow(i);
            if (activeWindow != nullptr)
                {
                if (activeWindow->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx)) && includeLists)
                    {
                    formatList(dynamic_cast<Wisteria::UI::ListCtrlEx*>(activeWindow),
                               includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(wxCLASSINFO(wxWebView)))
                    {
                    formatWebViewReport(dynamic_cast<wxWebView*>(activeWindow),
                                        includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(wxCLASSINFO(Wisteria::Canvas)))
                    {
                    formatImageOutput(dynamic_cast<Wisteria::Canvas*>(activeWindow),
                                      includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                }
            }
        outputText += L"\n</details>\n";
        }

    wxString toc, infoTable;
    size_t tocIndex{ 0 };
    if (includeTestScores && (GetReadabilityResultsView().GetWindowCount() != 0U))
        {
        toc += wxString::Format(
            L"<li><a href='#scores'><span class='n'>%02zu</span><span>%s</span></a></li>\n",
            ++tocIndex, htmlEncode({ GetReadabilityScoresLabel().wc_str() }, true).c_str());
        }
    if (includeStatistics && (GetSummaryView().GetWindowCount() != 0U))
        {
        toc += wxString::Format(
            L"<li><a href='#stats'><span class='n'>%02zu</span><span>%s</span></a></li>\n",
            ++tocIndex, htmlEncode({ GetSummaryStatisticsLabel().wc_str() }, true).c_str());
        }
    if (includeWordsBreakdown && (GetWordsBreakdownView().GetWindowCount() != 0U))
        {
        toc += wxString::Format(
            L"<li><a href='#wordsbreakdown'><span class='n'>%02zu</span><span>%s</span></a></li>\n",
            ++tocIndex, htmlEncode({ GetWordsBreakdownLabel().wc_str() }, true).c_str());
        }
    if (includeSentencesBreakdown && (GetSentencesBreakdownView().GetWindowCount() != 0U))
        {
        toc += wxString::Format(
            L"<li><a href='#sentencesbreakdown'><span "
            L"class='n'>%02zu</span><span>%s</span></a></li>\n",
            ++tocIndex, htmlEncode({ GetSentencesBreakdownLabel().wc_str() }, true).c_str());
        }
    // grammar section only has text and list windows, so don't include that if
    // not including those types of windows
    if (includeGrammar && (includeLists || includeTextReports) &&
        (GetGrammarView().GetWindowCount() != 0U))
        {
        toc += wxString::Format(
            L"<li><a href='#grammar'><span class='n'>%02zu</span><span>%s</span></a></li>\n",
            ++tocIndex, htmlEncode({ GetGrammarLabel().wc_str() }, true).c_str());
        }
    // Plain Language Guide section only has text windows, so don't include that if
    // not including those types of windows
    if (includePlainLanguageGuide && includeTextReports &&
        (GetPlainLanguageGuideView().GetWindowCount() != 0U))
        {
        toc += wxString::Format(
            L"<li><a href='#plainlanguageguide'><span class='n'>%02zu</span><span>%s</span></a>"
            L"</li>\n",
            ++tocIndex, htmlEncode({ GetPlainLanguageGuideLabel().wc_str() }, true).c_str());
        }
    if (includeSightWords && (GetDolchSightWordsView().GetWindowCount() != 0U))
        {
        toc += wxString::Format(
            L"<li><a href='#dolch'><span class='n'>%02zu</span><span>%s</span></a></li>\n",
            ++tocIndex, htmlEncode({ GetDolchLabel().wc_str() }, true).c_str());
        }

    // embed the app icon as a small self-contained image for the masthead
    wxString logoDataUri;
        {
        const wxBitmap logoBmp = wxGetApp()
                                     .GetResourceManager()
                                     .GetSVG(L"ribbon/app-logo.svg")
                                     .GetBitmap(wxSize{ 64, 64 });
        wxMemoryOutputStream logoStream;
        if (logoBmp.IsOk() && logoBmp.ConvertToImage().SaveFile(logoStream, wxBITMAP_TYPE_PNG))
            {
            logoDataUri = wxString::Format(
                _DT(L"data:image/png;base64,%s"),
                wxBase64Encode(logoStream.GetOutputStreamBuffer()->GetBufferStart(),
                               logoStream.GetOutputStreamBuffer()->GetBufferSize()));
            }
        }

    // skip meta fields the project hasn't set, so the masthead doesn't show orphaned labels with no
    // value next to them
    wxString metaFields;
    const auto addMetaField =
        [&metaFields, &htmlEncode](const wxString& label, const wxString& value)
    {
        if (!value.empty())
            {
            metaFields += wxString::Format(L"<div><dt>%s</dt><dd>%s</dd></div>\n", label,
                                           htmlEncode({ value.wc_str() }, true).c_str());
            }
    };
    addMetaField(_(L"Status"), doc->GetStatus());
    addMetaField(_(L"Reviewer"), doc->GetReviewer());
    addMetaField(_(L"Date"), wxDateTime::Now().FormatDate());

    infoTable = wxString::Format(
        L"<header class='export-masthead'>\n"
        "<div class='export-masthead-inner'>\n"
        "%s"
        "<div class='export-masthead-text'>\n"
        "<p class='export-eyebrow'>%s</p>\n"
        "<h1>%s</h1>\n"
        "<dl class='export-meta'>\n"
        "%s"
        "</dl>\n"
        "</div>\n"
        "</div>\n"
        "</header>",
        (logoDataUri.empty() ?
             wxString{} :
             wxString::Format(L"<img class='export-mark' alt='' src='%s' />\n", logoDataUri)),
        htmlEncode({ wxGetApp().GetAppDisplayName().wc_str() }, true).c_str(),
        htmlEncode({ doc->GetTitle().wc_str() }, true).c_str(), metaFields);

    // embed the stylesheet: the report theme CSS (default.css), the export-all shell theme
    // (masthead and sidebar table of contents), the user's selected report theme override, and
    // finally the captured highlight rules, so the combined report is self-contained and
    // matches what the windows render on screen
    wxString reportCss = ProjectReportFormat::GetThemeCss(_DT(L"default.css")) + L"\n" +
                         ProjectReportFormat::GetThemeCss(_DT(L"export-themes/default.css"));
    if (const wxString userTheme = wxGetApp().GetAppOptions()->GetReportTheme(); !userTheme.empty())
        {
        reportCss += L"\n" + ProjectReportFormat::GetThemeCss(userTheme);
        }
    const wxString styleSection =
        L"\n    <style>\n" + reportCss + textWindowStyleSection + L"\n    </style>\n</head>";
    outputText.insert(0, L"<!DOCTYPE html>\n<html>\n" + headSection + styleSection +
                             L"\n<body id='top'>\n" + infoTable +
                             _DT(L"\n<div class='export-wrap'>"
                                 "\n<nav class='export-index no-print' aria-label='") +
                             _(L"Sections") + L"'>\n<h2>" + _(L"Sections") + L"</h2>\n<ol>\n" +
                             toc +
                             L"</ol>\n</nav>"
                             "\n<main class='export-plan'>");
    outputText +=
        _DT(L"\n</main>\n</div>\n<a href='#top' class='back-to-top no-print' aria-label='") +
        _(L"Back to top") + _DT(L"'>&#8593;</a>\n</body>\n</html>");

    wxFileName(filePath.GetFullPath()).SetPermissions(wxS_DEFAULT);
    wxFile file(filePath.GetFullPath(), wxFile::write);
    return file.Write(outputText);
    }
