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

#include "batch_project_view.h"
#include "../Wisteria-Dataviz/src/graphs/danielsonbryan2plot.h"
#include "../Wisteria-Dataviz/src/graphs/lixgauge.h"
#include "../Wisteria-Dataviz/src/graphs/lixgaugegerman.h"
#include "../Wisteria-Dataviz/src/graphs/wordcloud.h"
#include "../Wisteria-Dataviz/src/import/html_encode.h"
#include "../Wisteria-Dataviz/src/import/html_extract_text.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/radioboxdlg.h"
#include "../app/readability_app.h"
#include "../graphs/schwartzgraph.h"
#include "../results-format/project_report_format.h"
#include "../ui/dialogs/edit_text_dlg.h"
#include "../ui/dialogs/export_all_dlg.h"
#include "../ui/dialogs/filtered_text_export_options_dlg.h"
#include "../ui/dialogs/filtered_text_preview_dlg.h"
#include "../ui/dialogs/tools_options_dlg.h"
#include "batch_project_doc.h"
#include "standard_project_doc.h"
#include <wx/dir.h>
#include <wx/richmsgdlg.h>

wxDECLARE_APP(ReadabilityApp);

wxIMPLEMENT_DYNAMIC_CLASS(BatchProjectView, BaseProjectView)

    //-------------------------------------------------------
    BatchProjectView::BatchProjectView()
    {
    Bind(wxEVT_LIST_ITEM_SELECTED, &BatchProjectView::OnNonScoreItemSelected, this,
         STATS_LIST_PAGE_ID);
    Bind(Wisteria::UI::wxEVT_SIDEBAR_CLICK, &BatchProjectView::OnItemSelected, this,
         BaseProjectView::LEFT_PANE);

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &BatchProjectView::OnRibbonButtonCommand, this,
         wxID_SELECTALL);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &BatchProjectView::OnRibbonButtonCommand, this, wxID_COPY);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &BatchProjectView::OnRibbonButtonCommand, this, wxID_PRINT);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &BatchProjectView::OnRibbonButtonCommand, this,
         wxID_ZOOM_IN);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &BatchProjectView::OnRibbonButtonCommand, this,
         wxID_ZOOM_OUT);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &BatchProjectView::OnRibbonButtonCommand, this,
         wxID_ZOOM_FIT);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &BatchProjectView::OnRibbonButtonCommand, this,
         XRCID("ID_SAVE_ITEM"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &BatchProjectView::OnRibbonButtonCommand, this,
         XRCID("ID_VIEW_ITEM"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &BatchProjectView::OnRibbonButtonCommand, this,
         XRCID("ID_LIST_SORT"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &BatchProjectView::OnRibbonButtonCommand, this,
         XRCID("ID_SEND_TO_STANDARD_PROJECT"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &BatchProjectView::OnRibbonButtonCommand, this,
         XRCID("ID_LAUNCH_SOURCE_FILE"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &BatchProjectView::OnRibbonButtonCommand, this, wxID_NEW);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &BatchProjectView::OnRibbonButtonCommand, this, wxID_OPEN);
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &BatchProjectView::OnRibbonButtonCommand, this, wxID_SAVE);

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &BatchProjectView::OnLongFormat, this,
         XRCID("ID_LONG_FORMAT"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &BatchProjectView::OnSummation, this,
         XRCID("ID_SUMMATION"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &BatchProjectView::OnPaneShowOrHide, this,
         XRCID("ID_TEST_EXPLANATIONS_WINDOW"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &BatchProjectView::OnPaneShowOrHide, this,
         XRCID("ID_STATISTICS_WINDOW"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &BatchProjectView::OnDocumentDelete, this,
         XRCID("ID_REMOVE_DOCUMENT"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &BatchProjectView::OnTestDelete, this,
         XRCID("ID_REMOVE_TEST"));

    Bind(wxEVT_FIND, &BatchProjectView::OnFind, this);
    Bind(wxEVT_FIND_NEXT, &BatchProjectView::OnFind, this);
    Bind(wxEVT_FIND_CLOSE, &BatchProjectView::OnFind, this);

    Bind(wxEVT_WISTERIA_CANVAS_DCLICK, &BatchProjectView::OnEditGraphOptions, this);

    Bind(wxEVT_MENU, &BatchProjectView::OnGradeScale, this, XRCID("ID_K12_US"));
    Bind(wxEVT_MENU, &BatchProjectView::OnGradeScale, this, XRCID("ID_K12_NEWFOUNDLAND"));
    Bind(wxEVT_MENU, &BatchProjectView::OnGradeScale, this, XRCID("ID_K12_BC"));
    Bind(wxEVT_MENU, &BatchProjectView::OnGradeScale, this, XRCID("ID_K12_NEW_BRUNSWICK"));
    Bind(wxEVT_MENU, &BatchProjectView::OnGradeScale, this, XRCID("ID_K12_NOVA_SCOTIA"));
    Bind(wxEVT_MENU, &BatchProjectView::OnGradeScale, this, XRCID("ID_K12_ONTARIO"));
    Bind(wxEVT_MENU, &BatchProjectView::OnGradeScale, this, XRCID("ID_K12_SASKATCHEWAN"));
    Bind(wxEVT_MENU, &BatchProjectView::OnGradeScale, this, XRCID("ID_K12_PE"));
    Bind(wxEVT_MENU, &BatchProjectView::OnGradeScale, this, XRCID("ID_K12_MANITOBA"));
    Bind(wxEVT_MENU, &BatchProjectView::OnGradeScale, this, XRCID("ID_K12_NT"));
    Bind(wxEVT_MENU, &BatchProjectView::OnGradeScale, this, XRCID("ID_K12_ALBERTA"));
    Bind(wxEVT_MENU, &BatchProjectView::OnGradeScale, this, XRCID("ID_K12_NUNAVUT"));
    Bind(wxEVT_MENU, &BatchProjectView::OnGradeScale, this, XRCID("ID_QUEBEC"));
    Bind(wxEVT_MENU, &BatchProjectView::OnGradeScale, this, XRCID("ID_ENGLAND"));

    Bind(wxEVT_LIST_ITEM_ACTIVATED, &BatchProjectView::OnDblClick, this,
         BaseProjectView::MISSPELLED_WORD_LIST_PAGE_ID);

    Bind(wxEVT_LIST_ITEM_SELECTED, &BatchProjectView::OnScoreItemSelected, this,
         BaseProjectView::ID_SCORE_LIST_PAGE_ID);

    Bind(wxEVT_LIST_ITEM_SELECTED, &BatchProjectView::OnNonScoreItemSelected, this,
         BaseProjectView::ID_DIFFICULT_WORDS_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_SELECTED, &BatchProjectView::OnNonScoreItemSelected, this,
         BaseProjectView::DUPLICATES_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_SELECTED, &BatchProjectView::OnNonScoreItemSelected, this,
         BaseProjectView::INCORRECT_ARTICLE_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_SELECTED, &BatchProjectView::OnNonScoreItemSelected, this,
         BaseProjectView::PASSIVE_VOICE_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_SELECTED, &BatchProjectView::OnNonScoreItemSelected, this,
         BaseProjectView::MISSPELLED_WORD_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_SELECTED, &BatchProjectView::OnNonScoreItemSelected, this,
         BaseProjectView::WORDY_PHRASES_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_SELECTED, &BatchProjectView::OnNonScoreItemSelected, this,
         BaseProjectView::CLICHES_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_SELECTED, &BatchProjectView::OnNonScoreItemSelected, this,
         BaseProjectView::SENTENCES_CONJUNCTION_START_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_SELECTED, &BatchProjectView::OnNonScoreItemSelected, this,
         BaseProjectView::SENTENCES_LOWERCASE_START_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_SELECTED, &BatchProjectView::OnNonScoreItemSelected, this,
         BaseProjectView::ALL_WORDS_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_SELECTED, &BatchProjectView::OnNonScoreItemSelected, this,
         BaseProjectView::ALL_WORDS_CONDENSED_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_SELECTED, &BatchProjectView::OnNonScoreItemSelected, this,
         BaseProjectView::REDUNDANT_PHRASE_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_SELECTED, &BatchProjectView::OnNonScoreItemSelected, this,
         BaseProjectView::WORDING_ERRORS_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_SELECTED, &BatchProjectView::OnNonScoreItemSelected, this,
         BaseProjectView::LONG_SENTENCES_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_SELECTED, &BatchProjectView::OnNonScoreItemSelected, this,
         BaseProjectView::WORDY_PHRASES_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_SELECTED, &BatchProjectView::OnNonScoreItemSelected, this,
         BaseProjectView::ID_DOLCH_COVERAGE_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_SELECTED, &BatchProjectView::OnNonScoreItemSelected, this,
         BaseProjectView::ID_DOLCH_WORDS_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_SELECTED, &BatchProjectView::OnNonScoreItemSelected, this,
         BaseProjectView::ID_NON_DOLCH_WORDS_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_SELECTED, &BatchProjectView::OnNonScoreItemSelected, this,
         BaseProjectView::ID_WARNING_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_SELECTED, &BatchProjectView::OnNonScoreItemSelected, this,
         BaseProjectView::ID_AGGREGATED_DOC_SCORES_LIST_PAGE_ID);
    Bind(wxEVT_LIST_ITEM_SELECTED, &BatchProjectView::OnNonScoreItemSelected, this,
         BaseProjectView::ID_AGGREGATED_CLOZE_SCORES_LIST_PAGE_ID);

    Bind(wxEVT_MENU, &BatchProjectView::OnBatchExportFilteredDocuments, this,
         XRCID("ID_BATCH_EXPORT_FILTERED_DOCUMENTS"));
    Bind(wxEVT_MENU, &BatchProjectView::OnMenuCommand, this, XRCID("ID_SAVE_ITEM"));
    Bind(wxEVT_MENU, &BatchProjectView::OnMenuCommand, this, XRCID("ID_COPY_ALL"));
    Bind(wxEVT_MENU, &BatchProjectView::OnMenuCommand, this, XRCID("ID_COPY_WITH_COLUMN_HEADERS"));
    Bind(wxEVT_MENU, &BatchProjectView::OnMenuCommand, this, XRCID("ID_COPY_FIRST_COLUMN"));
    Bind(wxEVT_MENU, &BatchProjectView::OnMenuCommand, this, XRCID("ID_VIEW_ITEM"));
    Bind(wxEVT_MENU, &BatchProjectView::OnMenuCommand, this, XRCID("ID_LIST_SORT"));
    Bind(wxEVT_MENU, &BatchProjectView::OnMenuCommand, this, XRCID("ID_EXPORT_ALL"));
    Bind(wxEVT_MENU, &BatchProjectView::OnMenuCommand, this, XRCID("ID_SEND_TO_STANDARD_PROJECT"));
    Bind(wxEVT_MENU, &BatchProjectView::OnMenuCommand, this, XRCID("ID_LAUNCH_SOURCE_FILE"));
    Bind(wxEVT_MENU, &BatchProjectView::OnMenuCommand, this, XRCID("ID_EXPORT_FILTERED_DOCUMENT"));

    // not actually a test (it doesn't have an ID), but we'll add it in this function
    Bind(wxEVT_MENU, &BatchProjectView::OnAddTest, this, XRCID("ID_DOLCH"));

    Bind(wxEVT_MENU, &BatchProjectView::OnTestDeleteMenu, this, XRCID("ID_REMOVE_TEST"));

    Bind(wxEVT_MENU, &BatchProjectView::OnExportScoresAndStatistics, this,
         XRCID("ID_EXPORT_SCORES_AND_STATISTICS"));
    Bind(wxEVT_MENU, &BatchProjectView::OnExportStatisticsReport, this,
         XRCID("ID_EXPORT_STATISTICS"));
    Bind(wxEVT_MENU, &BatchProjectView::OnAddToDictionary, this,
         XRCID("ID_ADD_ITEM_TO_DICTIONARY"));

    Bind(wxEVT_MENU, &BatchProjectView::OnMenuCommand, this, wxID_SELECTALL);
    Bind(wxEVT_MENU, &BatchProjectView::OnMenuCommand, this, wxID_COPY);
    Bind(wxEVT_MENU, &BatchProjectView::OnMenuCommand, this, wxID_PREVIEW);
    Bind(wxEVT_MENU, &BatchProjectView::OnMenuCommand, this, XRCID("ID_PRINT"));
    Bind(wxEVT_MENU, &BatchProjectView::OnMenuCommand, this, wxID_PRINT);
    Bind(wxEVT_MENU, &BatchProjectView::OnMenuCommand, this, wxID_ZOOM_IN);
    Bind(wxEVT_MENU, &BatchProjectView::OnMenuCommand, this, wxID_ZOOM_OUT);
    Bind(wxEVT_MENU, &BatchProjectView::OnMenuCommand, this, wxID_ZOOM_FIT);

    Bind(
        wxEVT_RIBBONBUTTONBAR_CLICKED,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            auto* projDoc = dynamic_cast<BatchProjectDoc*>(GetDocument());
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
            auto* projDoc = dynamic_cast<BatchProjectDoc*>(GetDocument());
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
            auto* projDoc = dynamic_cast<BatchProjectDoc*>(GetDocument());
            if (projDoc != nullptr)
                {
                projDoc->SaveAs();
                }
        },
        XRCID("ID_SAVE_PROJECT_AS"));
    }

//-------------------------------------------------------
void BatchProjectView::OnLongFormat([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto* doc = dynamic_cast<BatchProjectDoc*>(GetDocument());

    const bool useLongFormat = !doc->GetReadabilityMessageCatalog().IsUsingLongGradeScaleFormat();
    doc->GetReadabilityMessageCatalog().SetLongGradeScaleFormat(useLongFormat);
    // update this setting in the subprojects also
    for (auto* subDoc : doc->GetDocuments())
        {
        subDoc->GetReadabilityMessageCatalog().SetLongGradeScaleFormat(useLongFormat);
        }

    if (GetSplitter()->GetWindow2()->IsKindOf(CLASSINFO(Wisteria::UI::ListCtrlEx)))
        {
        dynamic_cast<Wisteria::UI::ListCtrlEx*>(GetSplitter()->GetWindow2())->Refresh();
        }
    doc->SetModifiedFlag();
    }

//-------------------------------------------------------
void BatchProjectView::OnSummation([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    const auto* doc = dynamic_cast<const BatchProjectDoc*>(GetDocument());
    Wisteria::UI::ListCtrlItemViewDlg viewDlg;
    if (GetActiveProjectWindow()->GetId() == MISSPELLED_WORD_LIST_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total Possible Misspellings"),
            std::to_wstring(static_cast<size_t>(doc->GetMisspelledWordData()->GetColumnSum(2))));
        }
    else if (GetActiveProjectWindow()->GetId() == DUPLICATES_LIST_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total Repeated Words"),
            std::to_wstring(static_cast<size_t>(doc->GetRepeatedWordData()->GetColumnSum(2))));
        }
    else if (GetActiveProjectWindow()->GetId() == INCORRECT_ARTICLE_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total Article Mismatches"),
            std::to_wstring(static_cast<size_t>(doc->GetIncorrectArticleData()->GetColumnSum(2))));
        }
    else if (GetActiveProjectWindow()->GetId() == PASSIVE_VOICE_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total Passive Phrases"),
            std::to_wstring(static_cast<size_t>(doc->GetPassiveVoiceData()->GetColumnSum(2))));
        }
    else if (GetActiveProjectWindow()->GetId() == WORDY_PHRASES_LIST_PAGE_ID)
        {
        viewDlg.AddValue(_(L"Total Wordy Items"), std::to_wstring(static_cast<size_t>(
                                                      doc->GetWordyItemsData()->GetColumnSum(2))));
        }
    else if (GetActiveProjectWindow()->GetId() == REDUNDANT_PHRASE_LIST_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total Redundant Phrases"),
            std::to_wstring(static_cast<size_t>(doc->GetRedundantPhrasesData()->GetColumnSum(2))));
        }
    else if (GetActiveProjectWindow()->GetId() == OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID)
        {
        viewDlg.AddValue(_(L"Total Overused Words (x Sentence)"),
                         std::to_wstring(static_cast<size_t>(
                             doc->GetOverusedWordBySentenceData()->GetColumnSum(2))));
        }
    else if (GetActiveProjectWindow()->GetId() == WORDING_ERRORS_LIST_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total Errors & Misspellings"),
            std::to_wstring(static_cast<size_t>(doc->GetWordingErrorsData()->GetColumnSum(2))));
        }
    else if (GetActiveProjectWindow()->GetId() == CLICHES_LIST_PAGE_ID)
        {
        viewDlg.AddValue(_(L"Total Clich\u00E9s"), std::to_wstring(static_cast<size_t>(
                                                       doc->GetClicheData()->GetColumnSum(2))));
        }
    else if (GetActiveProjectWindow()->GetId() == LONG_SENTENCES_LIST_PAGE_ID)
        {
        viewDlg.AddValue(
            _(L"Total Overly-long Sentences"),
            std::to_wstring(static_cast<size_t>(doc->GetLongSentencesData()->GetColumnSum(2))));
        }
    else if (GetActiveProjectWindow()->GetId() == SENTENCES_CONJUNCTION_START_LIST_PAGE_ID)
        {
        viewDlg.AddValue(_(L"Total Conjunction-starting Sentences"),
                         std::to_wstring(static_cast<size_t>(
                             doc->GetConjunctionStartingSentencesData()->GetColumnSum(2))));
        }
    else if (GetActiveProjectWindow()->GetId() == SENTENCES_LOWERCASE_START_LIST_PAGE_ID)
        {
        viewDlg.AddValue(_(L"Total Lowercases Sentences"),
                         std::to_wstring(static_cast<size_t>(
                             doc->GetLowerCasedSentencesData()->GetColumnSum(2))));
        }
    viewDlg.Create(GetDocFrame(), wxID_ANY, _(L"Column Summations"));
    viewDlg.ShowModal();
    }

//-------------------------------------------------------
void BatchProjectView::OnPaneShowOrHide(wxRibbonButtonBarEvent& event)
    {
    if (event.GetId() == XRCID("ID_TEST_EXPLANATIONS_WINDOW"))
        {
        if (m_testExplanations != nullptr)
            {
            m_testExplanations->Show(!m_testExplanations->IsShown());
            }
        }
    else if (event.GetId() == XRCID("ID_STATISTICS_WINDOW"))
        {
        if (m_statsReport != nullptr)
            {
            m_statsReport->Show(!m_statsReport->IsShown());
            }
        }
    GetDocFrame()->GetSizer()->Layout();
    }

//------------------------------------------------------
void BatchProjectView::OnBatchExportFilteredDocuments([[maybe_unused]] wxCommandEvent& event)
    {
    auto* doc = dynamic_cast<BatchProjectDoc*>(GetDocument());

    wxDirDialog dirDlg(nullptr, _(L"Select Output Directory"), wxString{},
                       wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (dirDlg.ShowModal() != wxID_OK)
        {
        return;
        }

    FilteredTextExportOptionsDlg optDlg(GetDocFrame());
    optDlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(), L"online/publishing.html");
    if (optDlg.ShowModal() != wxID_OK)
        {
        return;
        }

    wxProgressDialog progressDlg(_(L"Exporting"), _(L"Exporting filtered documents..."),
                                 static_cast<int>(doc->GetDocuments().size()), nullptr,
                                 wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_ELAPSED_TIME | wxPD_CAN_ABORT |
                                     wxPD_APP_MODAL);
    int counter{ 1 };

    std::wstring validDocText;
    bool errorsExport{ false };
    for (const auto* subDoc : doc->GetDocuments())
        {
        wxGetApp().Yield(true);
        if (!progressDlg.Update(counter++))
            {
            return;
            }

        subDoc->FormatFilteredText(validDocText, optDlg.IsReplacingCharacters(),
                                   optDlg.IsRemovingEllipses(), optDlg.IsRemovingBullets(),
                                   optDlg.IsRemovingFilePaths(), optDlg.IsStrippingAbbreviations(),
                                   optDlg.IsNarrowingFullWidthCharacters());

        wxString folderStructure = dirDlg.GetPath() + wxFileName::GetPathSeparator();
        const wxArrayString dirs = wxFileName(subDoc->GetOriginalDocumentFilePath()).GetDirs();
        for (const auto& dir : dirs)
            {
            folderStructure += StripIllegalFileCharacters(dir) + wxFileName::GetPathSeparator();
            }
        const wxString exportFilePath =
            folderStructure + wxFileName(subDoc->GetOriginalDocumentFilePath()).GetName() + L".txt";
        if (!wxDir::Exists(folderStructure) &&
            !wxDir::Make(folderStructure, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            wxLogError(L"Unable to create folder '%s'.", folderStructure);
            errorsExport = true;
            continue;
            }
        wxFileName(exportFilePath).SetPermissions(wxS_DEFAULT);
        wxFile filteredFile(exportFilePath, wxFile::write);
        if (!filteredFile.Write(validDocText, wxConvUTF8))
            {
            wxMessageBox(_(L"Unable to write to output file."), _(L"Error"),
                         wxOK | wxICON_EXCLAMATION);
            }
        }

    if (errorsExport)
        {
        wxMessageBox(_(L"Errors encountered while exporting. Please review the Log Report."),
                     _(L"Error"), wxOK | wxICON_EXCLAMATION);
        }
    }

//------------------------------------------------------
void BatchProjectView::OnAddToDictionary([[maybe_unused]] wxCommandEvent& event)
    {
    if (GetSideBar()->GetSelectedFolderId() != SIDEBAR_GRAMMAR_SECTION_ID)
        {
        GetSideBar()->SelectFolder(GetSideBar()->FindFolder(SIDEBAR_GRAMMAR_SECTION_ID), false);
        }

    const auto* listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        GetGrammarView().FindWindowById(MISSPELLED_WORD_LIST_PAGE_ID));
    if (listView != nullptr)
        {
        GetSideBar()->SelectSubItem(
            GetSideBar()->FindSubItem(SIDEBAR_GRAMMAR_SECTION_ID, MISSPELLED_WORD_LIST_PAGE_ID));
        if (listView->GetFocusedItem() == wxNOT_FOUND)
            {
            wxMessageBox(_(L"Please select a document to add its misspellings to your dictionary."),
                         _(L"Add to Dictionary"), wxOK | wxICON_INFORMATION);
            return;
            }
        wxArrayString misspellings;
        const wxString reportString =
            listView->GetItemTextEx(listView->GetFocusedItem(), listView->GetColumnCount() - 1);
        size_t startingQuote = reportString.find(L'\"');
        while (startingQuote != wxString::npos)
            {
            const auto endingQuote = reportString.find(L'\"', ++startingQuote);
            if (endingQuote == wxString::npos)
                {
                break;
                }
            misspellings.Add(reportString.substr(startingQuote, endingQuote - startingQuote));
            // next starting quote
            startingQuote = reportString.find(L'\"', endingQuote + 1);
            }
        Wisteria::UI::ListDlg misspellingDlg(
            GetDocFrame(), misspellings, true, wxNullColour, wxNullColour, wxNullColour,
            Wisteria::UI::LD_COPY_BUTTON | Wisteria::UI::LD_SELECT_ALL_BUTTON |
                Wisteria::UI::LD_OK_CANCEL_BUTTONS,
            wxID_ANY, _(L"Add to Dictionary"),
            _(L"Check the words to add to your custom dictionary and click OK:"));
        if (misspellingDlg.ShowModal() == wxID_OK)
            {
            wxGetApp().AddWordsToDictionaries(
                misspellingDlg.GetSelectedItems(),
                dynamic_cast<BaseProjectDoc*>(GetDocument())->GetProjectLanguage());
            const wxList docs = wxGetApp().GetDocManager()->GetDocuments();
            for (size_t i = 0; i < docs.GetCount(); ++i)
                {
                auto* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
                if (doc != nullptr)
                    {
                    doc->RemoveMisspellings(misspellingDlg.GetSelectedItems());
                    }
                }
            }
        }
    else
        {
        wxMessageBox(_(L"There are no misspellings in this document."), _(L"Add to Dictionary"),
                     wxOK | wxICON_INFORMATION);
        }
    }

//------------------------------------------------------
void BatchProjectView::OnDblClick(wxListEvent& event)
    {
    if (event.GetId() == MISSPELLED_WORD_LIST_PAGE_ID)
        {
        const auto* listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
            GetGrammarView().FindWindowById(MISSPELLED_WORD_LIST_PAGE_ID));
        if (listView == nullptr)
            {
            return;
            }
        wxArrayString misspellings;
        const wxString reportString =
            listView->GetItemTextEx(listView->GetFocusedItem(), listView->GetColumnCount() - 1);
        size_t startingQuote = reportString.find(L'\"');
        while (startingQuote != wxString::npos)
            {
            const auto endingQuote = reportString.find(L'\"', ++startingQuote);
            if (endingQuote == wxString::npos)
                {
                break;
                }
            misspellings.Add(reportString.substr(startingQuote, endingQuote - startingQuote));
            // next starting quote
            startingQuote = reportString.find(L'\"', endingQuote + 1);
            }
        Wisteria::UI::ListDlg misspellingDlg(
            GetDocFrame(), misspellings, true, wxNullColour, wxNullColour, wxNullColour,
            Wisteria::UI::LD_COPY_BUTTON | Wisteria::UI::LD_SELECT_ALL_BUTTON |
                Wisteria::UI::LD_OK_CANCEL_BUTTONS,
            wxID_ANY, _(L"Add to Dictionary"),
            _(L"Check the words to add to your custom dictionary and click OK:"));
        if (misspellingDlg.ShowModal() == wxID_OK)
            {
            wxGetApp().AddWordsToDictionaries(
                misspellingDlg.GetSelectedItems(),
                dynamic_cast<BaseProjectDoc*>(GetDocument())->GetProjectLanguage());
            const wxList docs = wxGetApp().GetDocManager()->GetDocuments();
            for (size_t i = 0; i < docs.GetCount(); ++i)
                {
                auto* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
                if (doc != nullptr)
                    {
                    doc->RemoveMisspellings(misspellingDlg.GetSelectedItems());
                    }
                }
            }
        }
    }

//-------------------------------------------------------
///@todo need to set the page more intelligently, like in ProjectView::OnEditGraphOptions().
void BatchProjectView::OnEditGraphOptions([[maybe_unused]] wxCommandEvent& event)
    {
    ToolsOptionsDlg optionsDlg(GetDocFrame(), dynamic_cast<BatchProjectDoc*>(GetDocument()),
                               ToolsOptionsDlg::GraphsSection);
    optionsDlg.SelectPage(ToolsOptionsDlg::GRAPH_GENERAL_PAGE);
    if (optionsDlg.ShowModal() == wxID_OK)
        {
        dynamic_cast<BatchProjectDoc*>(GetDocument())->RefreshGraphs();
        }
    }

//-------------------------------------------------------
void BatchProjectView::OnAddTest(wxCommandEvent& event)
    {
    auto* doc = dynamic_cast<BatchProjectDoc*>(GetDocument());
    if (!doc->IsSafeToUpdate())
        {
        return;
        }
    // include test from XRC_ID passed in from the menu
    if (event.GetId() == XRCID("ID_DOLCH"))
        {
        doc->IncludeDolchSightWords();
        }
    else
        {
        doc->GetReadabilityTests().include_test(
            doc->GetReadabilityTests().get_test_id(event.GetId()).c_str(), true);
        }

    doc->RefreshRequired(ProjectRefresh::Minimal);
    doc->RefreshProject();
    }

bool BatchProjectView::OnCreate(wxDocument* doc, long flags)
    {
    if (!BaseProjectView::OnCreate(doc, flags))
        {
        return false;
        }

    m_testExplanations = wxWebView::New(GetDocFrame(), wxID_ANY);
    if (m_testExplanations != nullptr)
        {
        m_testExplanations->Hide();
        m_testExplanations->SetLabel(_(L"Test Explanations"));
        m_testExplanations->EnableContextMenu(false);
        GetWorkSpaceSizer()->Add(m_testExplanations, wxSizerFlags{ 1 }.Expand());
        }

    m_statsReport = wxWebView::New(GetDocFrame(), wxID_ANY);
    if (m_statsReport != nullptr)
        {
        m_statsReport->Hide();
        m_statsReport->SetLabel(_(L"Summary Statistics"));
        m_statsReport->EnableContextMenu(false);
        GetWorkSpaceSizer()->Add(m_statsReport, wxSizerFlags{ 1 }.Expand());
        }

    m_warningsView =
        new Wisteria::UI::ListCtrlEx(GetSplitter(), ID_WARNING_LIST_PAGE_ID, wxDefaultPosition,
                                     wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxLC_ALIGN_LEFT);
    m_warningsView->Show(false);

    GetSplitter()->SplitVertically(GetSideBar(), GetWarningsView(), GetSideBar()->GetMinWidth());

#ifdef __WXOSX__
    // just load the menubar right now, we will set it in Present
    // after the document has successfully loaded
    m_menuBar = wxXmlResource::Get()->LoadMenuBar(L"ID_BATCHDOCMENUBAR");
#endif

    // connect the test events
    for (const auto& rTest :
         dynamic_cast<const BaseProjectDoc*>(doc)->GetReadabilityTests().get_tests())
        {
        Connect(rTest.get_test().get_interface_id(), wxEVT_MENU,
                wxCommandEventHandler(BatchProjectView::OnAddTest));
        }

    // bind menu events to their respective ribbon button events
    Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& evt)
        {
            wxRibbonButtonBarEvent event;
            event.SetId(evt.GetId());
            OnPaneShowOrHide(event);
        },
        XRCID("ID_STATISTICS_WINDOW"));
    Bind(
        wxEVT_MENU,
        [this](wxCommandEvent& evt)
        {
            wxRibbonButtonBarEvent event;
            event.SetId(evt.GetId());
            OnPaneShowOrHide(event);
        },
        XRCID("ID_TEST_EXPLANATIONS_WINDOW"));

    return true;
    }

//-------------------------------------------------------
void BatchProjectView::UpdateSideBarIcons()
    {
    GetSideBar()->SaveState();
    GetSideBar()->DeleteAllFolders();

    const auto checkGraphType = [](wxWindow* window, const wxClassInfo* className)
    {
        const auto* canvas = dynamic_cast<Wisteria::Canvas*>(window);
        wxASSERT_MSG(canvas, L"Window is not a canvas!");
        return (canvas != nullptr) ? canvas->GetFixedObject(0, 0)->IsKindOf(className) : false;
    };

    // Note: refer to ReadabilityApp::InitProjectSidebar() for the icon indices.
    if (GetScoresView().GetWindowCount() > 0)
        {
        GetSideBar()->InsertItem(GetSideBar()->GetFolderCount(), GetReadabilityScoresLabel(),
                                 SIDEBAR_READABILITY_SCORES_SECTION_ID, 1);

        for (auto* window : GetScoresView().GetWindows())
            {
            const bool isGraph = window->IsKindOf(wxCLASSINFO(Wisteria::Canvas));

            GetSideBar()->InsertSubItemById(
                SIDEBAR_READABILITY_SCORES_SECTION_ID, window->GetName(), window->GetId(),
                window->GetId() == ID_SCORE_LIST_PAGE_ID                   ? 15 :
                window->GetId() == ID_SCORE_STATS_LIST_PAGE_ID             ? 15 :
                window->GetId() == ID_AGGREGATED_DOC_SCORES_LIST_PAGE_ID   ? 15 :
                window->GetId() == ID_AGGREGATED_CLOZE_SCORES_LIST_PAGE_ID ? 15 :
                window->GetId() == READABILITY_GOALS_PAGE_ID               ? 28 :
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
    if (GetHistogramsView().GetWindowCount() > 0)
        {
        GetSideBar()->InsertItem(GetSideBar()->GetFolderCount(), GetHistogramsLabel(),
                                 SIDEBAR_HISTOGRAMS_SECTION_ID, 6);
        for (const auto* window : GetHistogramsView().GetWindows())
            {
            GetSideBar()->InsertSubItemById(SIDEBAR_HISTOGRAMS_SECTION_ID, window->GetName(),
                                            window->GetId(), 6);
            }
        }
    if (GetBoxPlotView().GetWindowCount() > 0)
        {
        GetSideBar()->InsertItem(GetSideBar()->GetFolderCount(), GetBoxPlotsLabel(),
                                 SIDEBAR_BOXPLOTS_SECTION_ID, 7);
        for (const auto* window : GetBoxPlotView().GetWindows())
            {
            GetSideBar()->InsertSubItemById(SIDEBAR_BOXPLOTS_SECTION_ID, window->GetName(),
                                            window->GetId(), 7);
            }
        }
    if (GetWordsBreakdownView().GetWindowCount() > 0)
        {
        GetSideBar()->InsertItem(GetSideBar()->GetFolderCount(), GetWordsBreakdownLabel(),
                                 SIDEBAR_WORDS_BREAKDOWN_SECTION_ID, 13);
        for (auto* window : GetWordsBreakdownView().GetWindows())
            {
            const bool isGraph = window->IsKindOf(wxCLASSINFO(Wisteria::Canvas));

            GetSideBar()->InsertSubItemById(
                SIDEBAR_WORDS_BREAKDOWN_SECTION_ID, window->GetName(), window->GetId(),
                (isGraph && checkGraphType(window, wxCLASSINFO(Wisteria::Graphs::WordCloud))) ? 29 :
                                                                                                15);
            }
        }
    if (GetSentencesBreakdownView().GetWindowCount() > 0)
        {
        GetSideBar()->InsertItem(GetSideBar()->GetFolderCount(), GetSentencesBreakdownLabel(),
                                 SIDEBAR_SENTENCES_BREAKDOWN_SECTION_ID, 14);
        for (const auto* window : GetSentencesBreakdownView().GetWindows())
            {
            GetSideBar()->InsertSubItemById(SIDEBAR_SENTENCES_BREAKDOWN_SECTION_ID,
                                            window->GetName(), window->GetId(), 15);
            }
        }
    if (GetSummaryStatsView().GetWindowCount() > 0)
        {
        GetSideBar()->InsertItem(GetSideBar()->GetFolderCount(), GetSummaryStatisticsLabel(),
                                 SIDEBAR_STATS_SUMMARY_SECTION_ID, 2);
        for (const auto* window : GetSummaryStatsView().GetWindows())
            {
            GetSideBar()->InsertSubItemById(SIDEBAR_STATS_SUMMARY_SECTION_ID, window->GetName(),
                                            window->GetId(), 15);
            }
        }
    if (GetGrammarView().GetWindowCount() > 0)
        {
        GetSideBar()->InsertItem(GetSideBar()->GetFolderCount(), GetGrammarLabel(),
                                 SIDEBAR_GRAMMAR_SECTION_ID, 4);
        for (const auto* window : GetGrammarView().GetWindows())
            {
            GetSideBar()->InsertSubItemById(SIDEBAR_GRAMMAR_SECTION_ID, window->GetName(),
                                            window->GetId(), 15);
            }
        }
    if (GetDolchSightWordsView().GetWindowCount() > 0)
        {
        GetSideBar()->InsertItem(GetSideBar()->GetFolderCount(), GetDolchLabel(),
                                 SIDEBAR_DOLCH_SECTION_ID, 5);
        for (const auto* window : GetDolchSightWordsView().GetWindows())
            {
            GetSideBar()->InsertSubItemById(SIDEBAR_DOLCH_SECTION_ID, window->GetName(),
                                            window->GetId(), 15);
            }
        }
    if (GetWarningsView()->GetItemCount() > 0)
        {
        GetSideBar()->InsertItem(GetSideBar()->GetFolderCount(), GetWarningLabel(),
                                 SIDEBAR_WARNINGS_SECTION_ID, 8);
        }

    GetSideBar()->ResetState();
    }

//----------------------------------------
void BatchProjectView::RemoveFromAllListCtrls(const wxString& valueToRemove)
    {
    wxWindow* activeWindow = nullptr;
    for (size_t i = 0; i < GetScoresView().GetWindowCount(); ++i)
        {
        activeWindow = GetScoresView().GetWindow(i);
        if ((activeWindow != nullptr) &&
            activeWindow->IsKindOf(CLASSINFO(Wisteria::UI::ListCtrlEx)))
            {
            dynamic_cast<Wisteria::UI::ListCtrlEx*>(activeWindow)->RemoveAll(valueToRemove);
            }
        }
    for (size_t i = 0; i < GetGrammarView().GetWindowCount(); ++i)
        {
        activeWindow = GetGrammarView().GetWindow(i);
        if ((activeWindow != nullptr) &&
            activeWindow->IsKindOf(CLASSINFO(Wisteria::UI::ListCtrlEx)))
            {
            dynamic_cast<Wisteria::UI::ListCtrlEx*>(activeWindow)->RemoveAll(valueToRemove);
            }
        }
    for (size_t i = 0; i < GetDolchSightWordsView().GetWindowCount(); ++i)
        {
        activeWindow = GetDolchSightWordsView().GetWindow(i);
        if ((activeWindow != nullptr) &&
            activeWindow->IsKindOf(CLASSINFO(Wisteria::UI::ListCtrlEx)))
            {
            dynamic_cast<Wisteria::UI::ListCtrlEx*>(activeWindow)->RemoveAll(valueToRemove);
            }
        }
    for (size_t i = 0; i < GetWordsBreakdownView().GetWindowCount(); ++i)
        {
        activeWindow = GetWordsBreakdownView().GetWindow(i);
        if ((activeWindow != nullptr) &&
            activeWindow->IsKindOf(CLASSINFO(Wisteria::UI::ListCtrlEx)))
            {
            dynamic_cast<Wisteria::UI::ListCtrlEx*>(activeWindow)->RemoveAll(valueToRemove);
            }
        }
    for (size_t i = 0; i < GetSentencesBreakdownView().GetWindowCount(); ++i)
        {
        activeWindow = GetSentencesBreakdownView().GetWindow(i);
        if ((activeWindow != nullptr) &&
            activeWindow->IsKindOf(CLASSINFO(Wisteria::UI::ListCtrlEx)))
            {
            dynamic_cast<Wisteria::UI::ListCtrlEx*>(activeWindow)->RemoveAll(valueToRemove);
            }
        }
    for (size_t i = 0; i < GetSummaryStatsView().GetWindowCount(); ++i)
        {
        activeWindow = GetSummaryStatsView().GetWindow(i);
        if ((activeWindow != nullptr) &&
            activeWindow->IsKindOf(CLASSINFO(Wisteria::UI::ListCtrlEx)))
            {
            dynamic_cast<Wisteria::UI::ListCtrlEx*>(activeWindow)->RemoveAll(valueToRemove);
            }
        }
    GetWarningsView()->RemoveAll(valueToRemove);
    }

//----------------------------------------
void BatchProjectView::OnTestDeleteMenu([[maybe_unused]] wxCommandEvent& event)
    {
    wxRibbonButtonBarEvent cmd;
    OnTestDelete(cmd);
    }

//----------------------------------------
void BatchProjectView::OnTestDelete([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto* doc = dynamic_cast<BatchProjectDoc*>(GetDocument());
    if (!doc->IsSafeToUpdate())
        {
        return;
        }

    if ((GetActiveProjectWindow() != nullptr) &&
        GetActiveProjectWindow()->GetId() != ID_SCORE_STATS_LIST_PAGE_ID)
        {
        GetSideBar()->SelectSubItem(GetSideBar()->FindSubItem(SIDEBAR_READABILITY_SCORES_SECTION_ID,
                                                              ID_SCORE_STATS_LIST_PAGE_ID));
        }

    const Wisteria::UI::ListCtrlEx* activeListCtrl = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        GetScoresView().FindWindowById(ID_SCORE_STATS_LIST_PAGE_ID));
    if (activeListCtrl != nullptr)
        {
        const wxString testToRemove = activeListCtrl->GetSelectedText();
        if (testToRemove.empty())
            {
            wxMessageBox(_(L"Please select a test to remove."), wxGetApp().GetAppName(),
                         wxOK | wxICON_INFORMATION);
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

        if (!doc->RemoveTest(testToRemove))
            {
            // some test names from the interface might have extra labeling on them
            if (!doc->RemoveTest(BatchProjectView::StripIndexValuesLabel(testToRemove)) &&
                !doc->RemoveTest(BatchProjectView::StripClozeValuesLabel(testToRemove)))
                {
                doc->RemoveTest(BatchProjectView::StripGradeLevelsLabel(testToRemove));
                }
            }
        }

    doc->RefreshRequired(ProjectRefresh::Minimal);
    doc->RefreshProject();
    }

//----------------------------------------
void BatchProjectView::OnDocumentDelete([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    auto* doc = dynamic_cast<BatchProjectDoc*>(GetDocument());
    if (!doc->IsSafeToUpdate())
        {
        return;
        }

    if ((GetActiveProjectWindow() == nullptr) ||
        !GetActiveProjectWindow()->IsKindOf(CLASSINFO(Wisteria::UI::ListCtrlEx)))
        {
        wxMessageBox(_(L"Please select a window that contains a list of documents, "
                       "select the ones that you wish to remove, and try again."),
                     wxGetApp().GetAppName(), wxOK | wxICON_INFORMATION);
        return;
        }

    const auto* activeListCtrl = dynamic_cast<Wisteria::UI::ListCtrlEx*>(GetActiveProjectWindow());
    if (IsFilenameList(activeListCtrl->GetId()))
        {
        // get the files that are selected
        wxArrayString filesToRemove;
        long item = -1;
        while (true)
            {
            item = activeListCtrl->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            if (item == wxNOT_FOUND)
                {
                break;
                }
            filesToRemove.Add(activeListCtrl->GetItemTextEx(item, 0));
            }
        if (filesToRemove.GetCount() == 0)
            {
            return;
            }

        auto warningIter = WarningManager::GetWarning(_DT(L"delete-document-from-batch"));
        if (warningIter != WarningManager::GetWarnings().end() && warningIter->ShouldBeShown())
            {
            Wisteria::UI::ListDlg listDlg(
                GetDocFrame(), filesToRemove, false, wxNullColour, wxNullColour, wxNullColour,
                Wisteria::UI::LD_YES_NO_BUTTONS | Wisteria::UI::LD_DONT_SHOW_AGAIN, wxID_ANY,
                _(L"Remove Documents"),
                _(L"Do you wish to remove these documents from the project?"));
            listDlg.SetCheckBoxLabel(_(L"Always delete without prompting"));
            const int dlgResponse = listDlg.ShowModal();
            // save the checkbox status
            if (listDlg.IsCheckBoxChecked() && (dlgResponse == wxID_YES))
                {
                warningIter->Show(false);
                warningIter->SetPreviousResponse(dlgResponse);
                }
            if (dlgResponse == wxID_NO)
                {
                return;
                }
            }
        const wxWindowUpdateLocker noUpdates(GetDocFrame());
        const wxBusyCursor wait;

        wxProgressDialog progressDlg(wxFileName::StripExtension(doc->GetTitle()),
                                     _(L"Removing documents..."), filesToRemove.size(),
                                     GetDocFrame(), wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_APP_MODAL);

        // remove the files from the documents collection and from all the listcontrols
        // (some of these controls can't be updated without doing a full re-indexing,
        // so just manually remove the paths from them).
        for (size_t i = 0; i < filesToRemove.GetCount(); ++i)
            {
            doc->RemoveDocument(filesToRemove[i]);
            RemoveFromAllListCtrls(filesToRemove[i]);
            progressDlg.Update(i + 1);
            wxGetApp().Yield(true);
            }
        doc->RefreshRequired(ProjectRefresh::Minimal);
        doc->RefreshProject();
        if (activeListCtrl->GetItemCount() == 0)
            {
            const wxString noDocsHtml =
                L"<!DOCTYPE html><html><head>"
                L"<meta name='color-scheme' content='light dark' />"
                L"<style>body{background-color:Canvas;color:CanvasText;}</style>"
                L"</head><body>" +
                _(L"No documents available.") + L"</body></html>";
            if (m_testExplanations != nullptr)
                {
                m_testExplanations->SetPage(noDocsHtml, wxString{});
                }
            if (m_statsReport != nullptr)
                {
                m_statsReport->SetPage(noDocsHtml, wxString{});
                }
            }
        }
    else
        {
        wxMessageBox(_(L"Please select a window that contains a list of documents, "
                       "select the ones that you wish to remove, and try again."),
                     wxGetApp().GetAppName(), wxOK | wxICON_INFORMATION);
        }
    }

//-------------------------------------------------------
void BatchProjectView::OnItemSelected(wxCommandEvent& event)
    {
    wxASSERT(GetRibbon() != nullptr);
    const auto hideEditPanel = [this](const wxWindowID windowId)
    {
        wxWindow* editButtonBarWindow = GetRibbon()->FindWindow(windowId);
        wxASSERT(editButtonBarWindow != nullptr);
        wxASSERT(editButtonBarWindow->IsKindOf(CLASSINFO(wxRibbonPanel)));
        editButtonBarWindow->Show(false);
        return dynamic_cast<wxRibbonPanel*>(editButtonBarWindow);
    };

    const auto getEditButtonBar = [](const wxRibbonPanel* panel)
    {
        auto* buttonBar = panel->FindWindow(MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);
        wxASSERT(buttonBar != nullptr && buttonBar->IsKindOf(CLASSINFO(wxRibbonButtonBar)));
        return dynamic_cast<wxRibbonButtonBar*>(buttonBar);
    };

    const auto resetActiveCanvasResizeDelay = [this]()
    {
        if (GetActiveProjectWindow() != nullptr &&
            GetActiveProjectWindow()->IsKindOf(CLASSINFO(Wisteria::Canvas)))
            {
            wxASSERT(dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow()));
            dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())->ResetResizeDelay();
            }
    };

    wxRibbonPanel* editListButtonBarWindow = hideEditPanel(MainFrame::ID_EDIT_RIBBON_LIST_PANEL);
    wxRibbonPanel* editStatsListButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_STATS_LIST_PANEL);
    wxRibbonPanel* editBoxPlotButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_BOX_PLOT_PANEL);
    wxRibbonPanel* editHistogramBatchButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_HISTOGRAM_BATCH_PANEL);
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
    wxRibbonPanel* editListCsvssButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_LIST_CSVSS_PANEL);
    wxRibbonPanel* editListTestScoresButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_LIST_TEST_SCORES_PANEL);
    wxRibbonPanel* editGeneralReadabilityButtonBarWindow =
        hideEditPanel(MainFrame::ID_EDIT_RIBBON_GENERAL_READABILITY_GRAPH_PANEL);
    // hide standard project panels that we don't use here
    hideEditPanel(MainFrame::ID_EDIT_RIBBON_STATS_SUMMARY_REPORT_PANEL);
    hideEditPanel(MainFrame::ID_EDIT_RIBBON_LIST_SIMPLE_WITH_SUM_PANEL);
    hideEditPanel(MainFrame::ID_EDIT_RIBBON_LIST_SIMPLE_WITH_SUM_AND_EXCLUDE_PANEL);
    hideEditPanel(MainFrame::ID_EDIT_RIBBON_LIST_SIMPLE_PANEL);
    hideEditPanel(MainFrame::ID_EDIT_RIBBON_BAR_CHART_PANEL);
    hideEditPanel(MainFrame::ID_EDIT_RIBBON_PIE_CHART_PANEL);
    hideEditPanel(MainFrame::ID_EDIT_RIBBON_HISTOGRAM_PANEL);
    hideEditPanel(MainFrame::ID_EDIT_RIBBON_SUMMARY_REPORT_PANEL);
    hideEditPanel(MainFrame::ID_EDIT_RIBBON_EXPLANATION_LIST_PANEL);
    hideEditPanel(MainFrame::ID_EDIT_RIBBON_REPORT_PANEL);
    hideEditPanel(MainFrame::ID_EDIT_RIBBON_SYLLABLE_HISTOGRAM_PANEL);

    if (event.GetExtraLong() == SIDEBAR_READABILITY_SCORES_SECTION_ID)
        {
        m_activeWindow = GetScoresView().FindWindowById(event.GetInt());
        resetActiveCanvasResizeDelay();
        wxASSERT(m_activeWindow != nullptr);

        if (GetActiveProjectWindow() != nullptr)
            {
            GetSplitter()->GetWindow2()->Hide();
            GetSplitter()->ReplaceWindow(GetSplitter()->GetWindow2(), GetActiveProjectWindow());
            GetActiveProjectWindow()->Show();
            if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(Wisteria::Canvas)))
                {
                if (GetRibbon() != nullptr)
                    {
                    const auto graph = dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())
                                           ->GetFixedObject(0, 0);

                    if (graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::LixGaugeGerman)))
                        {
                        editLixGermanButtonBarWindow->Show();
                        getEditButtonBar(editLixGermanButtonBarWindow)
                            ->ToggleButton(XRCID("ID_USE_ENGLISH_LABELS"),
                                           dynamic_cast<BatchProjectDoc*>(GetDocument())
                                               ->IsUsingEnglishLabelsForGermanLix());
                        getEditButtonBar(editLixGermanButtonBarWindow)
                            ->ToggleButton(XRCID("ID_EDIT_GRAPH_SHOWCASE_KEY_ITEMS"),
                                           dynamic_cast<BatchProjectDoc*>(GetDocument())
                                               ->IsShowcasingKeyItems());
                        }
                    else if (graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::LixGauge)))
                        {
                        editLixButtonBarWindow->Show();
                        getEditButtonBar(editLixButtonBarWindow)
                            ->ToggleButton(XRCID("ID_EDIT_GRAPH_SHOWCASE_KEY_ITEMS"),
                                           dynamic_cast<BatchProjectDoc*>(GetDocument())
                                               ->IsShowcasingKeyItems());
                        }
                    else if (graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::RaygorGraph)))
                        {
                        editRaygorButtonBarWindow->Show();
                        }
                    else if (graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::FryGraph)) ||
                             graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::SchwartzGraph)))
                        {
                        editFrySchwartzButtonBarWindow->Show();
                        }
                    else if (graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::FleschChart)))
                        {
                        editFleschButtonBarWindow->Show();
                        getEditButtonBar(editFleschButtonBarWindow)
                            ->ToggleButton(XRCID("ID_FLESCH_DISPLAY_LINES"),
                                           dynamic_cast<BatchProjectDoc*>(GetDocument())
                                               ->IsConnectingFleschPoints());
                        }
                    else if (graph->IsKindOf(wxCLASSINFO(Wisteria::Graphs::DanielsonBryan2Plot)))
                        {
                        editDB2ButtonBarWindow->Show();
                        getEditButtonBar(editDB2ButtonBarWindow)
                            ->ToggleButton(XRCID("ID_EDIT_GRAPH_SHOWCASE_KEY_ITEMS"),
                                           dynamic_cast<BatchProjectDoc*>(GetDocument())
                                               ->IsShowcasingKeyItems());
                        }
                    else
                        {
                        editGeneralReadabilityButtonBarWindow->Show();
                        }
                    }
                }
            else if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(Wisteria::UI::ListCtrlEx)))
                {
                if (GetRibbon() != nullptr)
                    {
                    editListTestScoresButtonBarWindow->Show();
                    getEditButtonBar(editListTestScoresButtonBarWindow)
                        ->ToggleButton(XRCID("ID_LONG_FORMAT"),
                                       dynamic_cast<BatchProjectDoc*>(GetDocument())
                                           ->GetReadabilityMessageCatalog()
                                           .IsUsingLongGradeScaleFormat());
                    getEditButtonBar(editListTestScoresButtonBarWindow)
                        ->EnableButton(wxID_SELECTALL, !dynamic_cast<Wisteria::UI::ListCtrlEx*>(
                                                            GetActiveProjectWindow())
                                                            ->HasFlag(wxLC_SINGLE_SEL));
                    }
                }

            // show info messages
            if (event.GetInt() == READABILITY_GOALS_PAGE_ID)
                {
                if (WarningManager::HasWarning(_DT(L"batch-goals")))
                    {
                    ShowInfoMessage(*WarningManager::GetWarning(_DT(L"batch-goals")));
                    }
                }
            }
        }
    else if (event.GetExtraLong() == SIDEBAR_BOXPLOTS_SECTION_ID ||
             event.GetExtraLong() == SIDEBAR_HISTOGRAMS_SECTION_ID)
        {
        if (event.GetExtraLong() == SIDEBAR_BOXPLOTS_SECTION_ID)
            {
            // some tests have two box plots
            m_activeWindow =
                GetBoxPlotView().FindWindowByIdAndLabel(event.GetInt(), event.GetString());
            resetActiveCanvasResizeDelay();
            wxASSERT(m_activeWindow != nullptr);

            if (GetActiveProjectWindow() != nullptr)
                {
                GetSplitter()->GetWindow2()->Hide();
                GetSplitter()->ReplaceWindow(GetSplitter()->GetWindow2(), GetActiveProjectWindow());
                GetActiveProjectWindow()->Show();
                }
            }
        else if (event.GetExtraLong() == SIDEBAR_HISTOGRAMS_SECTION_ID)
            {
            // some tests have two histograms
            m_activeWindow =
                GetHistogramsView().FindWindowByIdAndLabel(event.GetInt(), event.GetString());
            resetActiveCanvasResizeDelay();
            wxASSERT(m_activeWindow != nullptr);

            if (GetActiveProjectWindow() != nullptr)
                {
                GetSplitter()->GetWindow2()->Hide();
                GetSplitter()->ReplaceWindow(GetSplitter()->GetWindow2(), GetActiveProjectWindow());
                GetActiveProjectWindow()->Show();
                }
            }
        if (GetRibbon() != nullptr)
            {
            auto graph =
                dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())->GetFixedObject(0, 0);

            if (graph->IsKindOf(CLASSINFO(Wisteria::Graphs::BoxPlot)))
                {
                editBoxPlotButtonBarWindow->Show();
                getEditButtonBar(editBoxPlotButtonBarWindow)
                    ->ToggleButton(
                        XRCID("ID_BOX_PLOT_DISPLAY_ALL_POINTS"),
                        dynamic_cast<BatchProjectDoc*>(GetDocument())->IsShowingAllBoxPlotPoints());
                getEditButtonBar(editBoxPlotButtonBarWindow)
                    ->ToggleButton(
                        XRCID("ID_BOX_PLOT_DISPLAY_LABELS"),
                        dynamic_cast<BatchProjectDoc*>(GetDocument())->IsDisplayingBoxPlotLabels());
                getEditButtonBar(editBoxPlotButtonBarWindow)
                    ->ToggleButton(
                        XRCID("ID_DROP_SHADOW"),
                        dynamic_cast<BatchProjectDoc*>(GetDocument())->IsDisplayingDropShadows());
                }
            else if (graph->IsKindOf(CLASSINFO(Wisteria::Graphs::Histogram)))
                {
                editHistogramBatchButtonBarWindow->Show();
                getEditButtonBar(editHistogramBatchButtonBarWindow)
                    ->ToggleButton(
                        XRCID("ID_DROP_SHADOW"),
                        dynamic_cast<BatchProjectDoc*>(GetDocument())->IsDisplayingDropShadows());
                getEditButtonBar(editHistogramBatchButtonBarWindow)
                    ->EnableButton(
                        XRCID("ID_EDIT_GRAPH_COLOR_SCHEME"),
                        dynamic_cast<Wisteria::Graphs::Histogram*>(graph.get())->IsUsingGrouping());
                getEditButtonBar(editHistogramBatchButtonBarWindow)
                    ->EnableButton(
                        XRCID("ID_GRADE_SCALES"),
                        std::dynamic_pointer_cast<Wisteria::Graphs::Graph2D>(graph)->HasProperty(
                            _DT(L"ISGRADEPLOT")));
                }
            }
        }
    else if (event.GetExtraLong() == SIDEBAR_WORDS_BREAKDOWN_SECTION_ID ||
             event.GetExtraLong() == SIDEBAR_STATS_SUMMARY_SECTION_ID)
        {
        m_activeWindow = (event.GetExtraLong() == SIDEBAR_WORDS_BREAKDOWN_SECTION_ID ?
                              GetWordsBreakdownView().FindWindowById(event.GetInt()) :
                              GetSummaryStatsView().FindWindowById(event.GetInt()));
        resetActiveCanvasResizeDelay();
        wxASSERT(m_activeWindow != nullptr);

        if (GetActiveProjectWindow() != nullptr)
            {
            GetSplitter()->GetWindow2()->Hide();
            GetSplitter()->ReplaceWindow(GetSplitter()->GetWindow2(), GetActiveProjectWindow());
            GetActiveProjectWindow()->Show();

            if (GetRibbon() != nullptr)
                {
                if (GetActiveProjectWindow()->IsKindOf(CLASSINFO(Wisteria::Canvas)))
                    {
                    if (dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow())
                            ->GetFixedObject(0, 0)
                            ->IsKindOf(wxCLASSINFO(Wisteria::Graphs::WordCloud)))
                        {
                        editWordCloudButtonBarWindow->Show();
                        }
                    else
                        {
                        editGraphButtonBarWindow->Show();
                        getEditButtonBar(editGraphButtonBarWindow)
                            ->ToggleButton(XRCID("ID_DROP_SHADOW"),
                                           dynamic_cast<BatchProjectDoc*>(GetDocument())
                                               ->IsDisplayingDropShadows());
                        }
                    }
                else
                    {
                    if (event.GetExtraLong() == SIDEBAR_STATS_SUMMARY_SECTION_ID)
                        {
                        editStatsListButtonBarWindow->Show();
                        }
                    else
                        {
                        editListButtonBarWindow->Show();
                        }
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
                editListCsvssButtonBarWindow->Show();
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
                editListCsvssButtonBarWindow->Show();
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

            if (GetRibbon() != nullptr)
                {
                editListButtonBarWindow->Show();
                }
            }
        }
    else if (event.GetInt() == SIDEBAR_WARNINGS_SECTION_ID)
        {
        m_activeWindow = GetWarningsView();
        resetActiveCanvasResizeDelay();
        wxASSERT(m_activeWindow != nullptr);

        if (GetActiveProjectWindow() != nullptr)
            {
            GetSplitter()->GetWindow2()->Hide();
            GetSplitter()->ReplaceWindow(GetSplitter()->GetWindow2(), GetActiveProjectWindow());
            GetActiveProjectWindow()->Show();

            if (GetRibbon() != nullptr)
                {
                editListButtonBarWindow->Show();
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
void BatchProjectView::UpdateStatAndTestPanes(const wxString& fileName)
    {
    if (fileName.empty())
        {
        return;
        }
    auto* list = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        GetScoresView().FindWindowById(ID_SCORE_LIST_PAGE_ID));
    // shouldn't happen
    if (list == nullptr)
        {
        return;
        }
    UpdateStatAndTestPanes(list->FindEx(fileName));
    }

//-------------------------------------------------------
wxString BatchProjectView::StripToTestName(const wxString& testName)
    {
    auto* doc = dynamic_cast<BatchProjectDoc*>(GetDocument());
    if (doc->GetReadabilityTests().has_test(testName))
        {
        return testName;
        }
    if (doc->GetReadabilityTests().has_test(StripGradeLevelsLabel(testName)))
        {
        return StripGradeLevelsLabel(testName);
        }
    if (doc->GetReadabilityTests().has_test(StripIndexValuesLabel(testName)))
        {
        return StripIndexValuesLabel(testName);
        }
    if (doc->GetReadabilityTests().has_test(StripClozeValuesLabel(testName)))
        {
        return StripClozeValuesLabel(testName);
        }
    return testName;
    }

//-------------------------------------------------------
void BatchProjectView::UpdateStatAndTestPanes(const long scoreListItem)
    {
    if (scoreListItem == wxNOT_FOUND)
        {
        return;
        }
    auto* list = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        GetScoresView().FindWindowById(ID_SCORE_LIST_PAGE_ID));
    wxASSERT(list != nullptr);
    // shouldn't happen
    if (list == nullptr)
        {
        return;
        }
    // If nothing is selected in the raw score list then just select this item and return--
    // selecting this item will simply fire the OnScoreItemSelected event and return here.
    // The point of selecting the item (if nothing is selected) is to prevent
    // OnActiveView from clearing the stats panes.
    if (list->GetSelectedItemCount() == 0)
        {
        list->Select(scoreListItem);
        return;
        }

    m_currentlySelectedFileName = list->GetItemTextEx(scoreListItem, 0);
    wxString scoreText = L"<br /><span class='emphasis'>" +
                         list->GetItemTextFormatted(scoreListItem, 0) + L"</span><hr>";

    auto* doc = dynamic_cast<BatchProjectDoc*>(GetDocument());
    double score = 0;
    const auto fleschPos = doc->GetReadabilityTests().get_test(ReadabilityMessages::FLESCH());
    const auto eflawPos = doc->GetReadabilityTests().get_test(ReadabilityMessages::EFLAW());
    const auto fjpPos =
        doc->GetReadabilityTests().get_test(ReadabilityMessages::FARR_JENKINS_PATERSON());
    const auto amstadPos = doc->GetReadabilityTests().get_test(ReadabilityMessages::AMSTAD());
    const auto db2Pos =
        doc->GetReadabilityTests().get_test(ReadabilityMessages::DANIELSON_BRYAN_2());
    const auto drpPos =
        doc->GetReadabilityTests().get_test(ReadabilityMessages::DEGREES_OF_READING_POWER());
    const auto frasePos = doc->GetReadabilityTests().get_test(ReadabilityMessages::FRASE());
    const auto infleszPos = doc->GetReadabilityTests().get_test(ReadabilityMessages::INFLESZ());
    for (long i = 2 /*skip document and description column*/; i < list->GetColumnCount(); ++i)
        {
        const wxString currentTestFullName = list->GetColumnName(i);
        // we will want grade level columns, but not cloze or index values
        const wxString currentTest = StripGradeLevelsLabel(currentTestFullName);
        auto standardTestPos = doc->GetReadabilityTests().get_test(currentTest);
        if (standardTestPos.second)
            {
            scoreText += L"<div class='explanation-card'>"
                         "<div class='explanation-card-header'>" +
                         currentTestFullName + L"</div><div class='explanation-card-body'>";

            // note that tests with their own scales have to be
            // formatted differently than the grade-level tests
            if (eflawPos.second && *eflawPos.first == readability::readability_test(currentTest))
                {
                if (ReadabilityMessages::GetScoreValue(list->GetItemTextEx(scoreListItem, i),
                                                       score))
                    {
                    scoreText += L"\n<p>" +
                                 ReadabilityMessages::GetEflawDescription(
                                     readability::eflaw_index_to_difficulty(score)) +
                                 L"</p>";
                    scoreText +=
                        L"\n<p>" +
                        wxString(
                            doc->GetReadabilityTests().get_test_description(currentTest).c_str()) +
                        L"</p></div></div>";
                    }
                else
                    {
                    scoreText +=
                        L"\n<p>" + list->GetItemTextEx(scoreListItem, i) + L"</p></div></div>";
                    }
                }
            else if ((fleschPos.second &&
                      *fleschPos.first == readability::readability_test(currentTest)) ||
                     (fjpPos.second &&
                      *fjpPos.first == readability::readability_test(currentTest)) ||
                     (amstadPos.second &&
                      *amstadPos.first == readability::readability_test(currentTest)))
                {
                if (ReadabilityMessages::GetScoreValue(list->GetItemTextEx(scoreListItem, i),
                                                       score))
                    {
                    scoreText += L"\n<p>" +
                                 ReadabilityMessages::GetFleschDescription(
                                     readability::flesch_score_to_difficulty_level(score)) +
                                 L"</p>";
                    scoreText +=
                        L"\n<p>" +
                        wxString(
                            doc->GetReadabilityTests().get_test_description(currentTest).c_str()) +
                        L"</p></div></div>";
                    }
                else
                    {
                    scoreText +=
                        L"\n<p>" + list->GetItemTextEx(scoreListItem, i) + L"</p></div></div>";
                    }
                }
            else if (db2Pos.second && *db2Pos.first == readability::readability_test(currentTest))
                {
                if (ReadabilityMessages::GetScoreValue(list->GetItemTextEx(scoreListItem, i),
                                                       score))
                    {
                    scoreText += L"\n<p>" +
                                 ReadabilityMessages::GetDanielsonBryan2Description(
                                     readability::flesch_score_to_difficulty_level(score)) +
                                 L"</p>";
                    scoreText +=
                        L"\n<p>" +
                        wxString(
                            doc->GetReadabilityTests().get_test_description(currentTest).c_str()) +
                        L"</p></div></div>";
                    }
                else
                    {
                    scoreText +=
                        L"\n<p>" + list->GetItemTextEx(scoreListItem, i) + L"</p></div></div>";
                    }
                }
            else if (drpPos.second && *drpPos.first == readability::readability_test(currentTest))
                {
                if (ReadabilityMessages::GetScoreValue(list->GetItemTextEx(scoreListItem, i),
                                                       score))
                    {
                    scoreText +=
                        L"\n<p>" + ReadabilityMessages::GetDrpUnitDescription(score) + L"</p>";
                    scoreText +=
                        L"\n<p>" +
                        wxString(
                            doc->GetReadabilityTests().get_test_description(currentTest).c_str()) +
                        L"</p></div></div>";
                    }
                else
                    {
                    scoreText +=
                        L"\n<p>" + list->GetItemTextEx(scoreListItem, i) + L"</p></div></div>";
                    }
                }
            else if (frasePos.second &&
                     *frasePos.first == readability::readability_test(currentTest))
                {
                if (ReadabilityMessages::GetScoreValue(list->GetItemTextEx(scoreListItem, i),
                                                       score))
                    {
                    scoreText +=
                        L"\n<p>" + ReadabilityMessages::GetFraseDescription(score) + L"</p>";
                    scoreText +=
                        L"\n<p>" +
                        wxString(
                            doc->GetReadabilityTests().get_test_description(currentTest).c_str()) +
                        L"</p></div></div>";
                    }
                else
                    {
                    scoreText +=
                        L"\n<p>" + list->GetItemTextEx(scoreListItem, i) + L"</p></div></div>";
                    }
                }
            else if (infleszPos.second &&
                     *infleszPos.first == readability::readability_test(currentTest))
                {
                if (ReadabilityMessages::GetScoreValue(list->GetItemTextEx(scoreListItem, i),
                                                       score))
                    {
                    const readability::inflesz_difficulty diffLevel =
                        readability::szigriszt_pazos_perspicuity_score_to_difficulty_inflesz_level(
                            static_cast<size_t>(score));

                    scoreText +=
                        L"\n<p>" + ReadabilityMessages::GetInfleszDescription(diffLevel) + L"</p>";
                    scoreText +=
                        L"\n<p>" +
                        wxString(
                            doc->GetReadabilityTests().get_test_description(currentTest).c_str()) +
                        L"</p></div></div>";
                    }
                else
                    {
                    scoreText +=
                        L"\n<p>" + list->GetItemTextEx(scoreListItem, i) + L"</p></div></div>";
                    }
                }
            else if (standardTestPos.first->get_test().get_test_type() ==
                         readability::readability_test_type::grade_level ||
                     standardTestPos.first->get_test().get_test_type() ==
                         readability::readability_test_type::index_value_and_grade_level ||
                     standardTestPos.first->get_test().get_test_type() ==
                         readability::readability_test_type::grade_level_and_predicted_cloze_score)
                {
                if (ReadabilityMessages::GetScoreValue(list->GetItemTextEx(scoreListItem, i),
                                                       score))
                    {
                    scoreText +=
                        L"\n<p>" +
                        doc->GetReadabilityMessageCatalog().GetGradeScaleDescription(score) +
                        L"</p>";
                    scoreText +=
                        L"\n<p>" +
                        wxString(
                            doc->GetReadabilityTests().get_test_description(currentTest).c_str()) +
                        L"</p></div></div>";
                    }
                // just show whatever message is in the list if we can't convert it to a value
                else
                    {
                    scoreText +=
                        L"\n<p>" + list->GetItemTextEx(scoreListItem, i) + L"</p></div></div>";
                    }
                }
            else if (standardTestPos.first->get_test().get_test_type() ==
                     readability::readability_test_type::predicted_cloze_score)
                {
                if (ReadabilityMessages::GetScoreValue(list->GetItemTextEx(scoreListItem, i),
                                                       score))
                    {
                    scoreText += L"\n<p>" +
                                 ReadabilityMessages::GetPredictedClozeDescription(score) + L"</p>";
                    scoreText +=
                        L"\n<p>" +
                        wxString(
                            doc->GetReadabilityTests().get_test_description(currentTest).c_str()) +
                        L"</p></div></div>";
                    }
                else
                    {
                    scoreText +=
                        L"\n<p>" + list->GetItemTextEx(scoreListItem, i) + L"</p></div></div>";
                    }
                }
            }
        else if (doc->HasCustomTest(currentTestFullName))
            {
            scoreText += L"<div class='explanation-card'>"
                         "<div class='explanation-card-header'>" +
                         currentTestFullName + L"</div><div class='explanation-card-body'>";

            auto customTestPos = doc->GetCustomTest(currentTestFullName)->GetIterator();
            if (customTestPos->get_test_type() == readability::readability_test_type::grade_level)
                {
                if (ReadabilityMessages::GetScoreValue(list->GetItemTextEx(scoreListItem, i),
                                                       score))
                    {
                    scoreText +=
                        L"\n<p>" +
                        doc->GetReadabilityMessageCatalog().GetGradeScaleDescription(score) +
                        L"</p></div></div>";
                    }
                // just show whatever message is in the list if we can't convert it to a value
                else
                    {
                    scoreText +=
                        L"\n<p>" + list->GetItemTextEx(scoreListItem, i) + L"</p></div></div>";
                    }
                }
            else if (customTestPos->get_test_type() ==
                     readability::readability_test_type::index_value)
                {
                // just show whatever value is in there, as we won't know what the
                // index value for a user's custom test really means to him/her
                scoreText += L"\n<p>" + _(L"Index score: ") +
                             list->GetItemTextEx(scoreListItem, i) + L"</p></div></div>";
                }
            else if (customTestPos->get_test_type() ==
                     readability::readability_test_type::predicted_cloze_score)
                {
                if (ReadabilityMessages::GetScoreValue(list->GetItemTextEx(scoreListItem, i),
                                                       score))
                    {
                    scoreText += L"\n<p>" +
                                 ReadabilityMessages::GetPredictedClozeDescription(score) +
                                 L"</p></div></div>";
                    }
                else
                    {
                    scoreText +=
                        L"\n<p>" + list->GetItemTextEx(scoreListItem, i) + L"</p></div></div>";
                    }
                }
            }
        }

    std::wstring scoreTextStrippedLinks{ scoreText };
    lily_of_the_valley::html_format::strip_hyperlinks(scoreTextStrippedLinks, false);
    if (m_testExplanations != nullptr)
        {
        m_testExplanations->SetPage(
            ProjectReportFormat::FormatHtmlReportStart(
                wxString::Format(_(L"Test Explanations [%s]"),
                                 wxFileName(m_currentlySelectedFileName).GetName()),
                wxGetApp().GetAppOptions()->GetReportTheme()) +
                ProjectReportFormat::FormatReportBanner(
                    _(L"Test Explanations"), wxFileName(m_currentlySelectedFileName).GetName()) +
                scoreTextStrippedLinks + ProjectReportFormat::FormatHtmlReportEnd(),
            wxString{});
        }

    const wxString docName = list->GetItemTextEx(scoreListItem, 0);
    for (size_t i = 0; i < doc->GetDocuments().size(); ++i)
        {
        if ((m_statsReport != nullptr) &&
            CompareFilePaths(doc->GetDocuments()[i]->GetOriginalDocumentFilePath(), docName) == 0)
            {
            const wxString docTable = L"<br /><span class='emphasis'>" +
                                      list->GetItemTextFormatted(scoreListItem, 0) + L"</span><hr>";
            const wxString text =
                docTable + ProjectReportFormat::FormatStatisticsInfo(
                               doc->GetDocuments()[i],
                               // use the batches settings, which may have just been updated,
                               // not the subproject's
                               doc->GetStatisticsReportInfo(),
                               wxSystemSettings::GetColour(wxSYS_COLOUR_HOTLIGHT), nullptr);
            std::wstring textStripped{ text };
            lily_of_the_valley::html_format::strip_hyperlinks(textStripped, false);
            m_statsReport->SetPage(
                ProjectReportFormat::FormatHtmlReportStart(
                    wxString::Format(_(L"Summary Statistics [%s]"), wxFileName(docName).GetName()),
                    wxGetApp().GetAppOptions()->GetReportTheme()) +
                    ProjectReportFormat::FormatReportBanner(_(L"Summary Statistics"),
                                                            wxFileName(docName).GetName()) +
                    textStripped + ProjectReportFormat::FormatHtmlReportEnd(),
                wxString{});
            break;
            }
        }
    }

//-------------------------------------------------------
void BatchProjectView::OnNonScoreItemSelected(wxListEvent& event)
    {
    const wxWindow* window = FindWindowById(event.GetId());
    if ((window != nullptr) && window->IsKindOf(CLASSINFO(Wisteria::UI::ListCtrlEx)))
        {
        UpdateStatAndTestPanes(
            dynamic_cast<const Wisteria::UI::ListCtrlEx*>(window)->GetSelectedText());
        }
    }

//-------------------------------------------------------
void BatchProjectView::OnScoreItemSelected(wxListEvent& event)
    {
    UpdateStatAndTestPanes(event.GetIndex());
    }

//-------------------------------------------------------
wxWindow* BatchProjectView::FindWindowById(const int Id)
    {
    if (GetScoresView().FindWindowById(Id) != nullptr)
        {
        return GetScoresView().FindWindowById(Id);
        }
    if (GetHistogramsView().FindWindowById(Id) != nullptr)
        {
        return GetHistogramsView().FindWindowById(Id);
        }
    if (GetBoxPlotView().FindWindowById(Id) != nullptr)
        {
        return GetBoxPlotView().FindWindowById(Id);
        }
    if (GetGrammarView().FindWindowById(Id) != nullptr)
        {
        return GetGrammarView().FindWindowById(Id);
        }
    if (GetWordsBreakdownView().FindWindowById(Id) != nullptr)
        {
        return GetWordsBreakdownView().FindWindowById(Id);
        }
    if (GetSentencesBreakdownView().FindWindowById(Id) != nullptr)
        {
        return GetSentencesBreakdownView().FindWindowById(Id);
        }
    if (GetSummaryStatsView().FindWindowById(Id) != nullptr)
        {
        return GetSummaryStatsView().FindWindowById(Id);
        }
    if (GetDolchSightWordsView().FindWindowById(Id) != nullptr)
        {
        return GetDolchSightWordsView().FindWindowById(Id);
        }
    if (GetWarningsView()->GetId() == Id)
        {
        return GetWarningsView();
        }
    return nullptr;
    }

//-------------------------------------------------------
bool BatchProjectView::ExportAll(const wxString& folder, wxString listExt, wxString graphExt,
                                 const bool includeHardWordLists,
                                 const bool includeSentencesBreakdown, const bool includeGraphs,
                                 const bool includeTestScores, const bool includeGrammarIssues,
                                 const bool includeSightWords, const bool includeWarnings,
                                 const bool includeSummaryStats,
                                 const Wisteria::UI::ImageExportOptions& graphOptions)
    {
    const auto* doc = dynamic_cast<const BatchProjectDoc*>(GetDocument());

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

    if (graphExt.empty())
        {
        graphExt = L".png";
        }
    else if (graphExt[0] != L'.')
        {
        graphExt.insert(0, L".");
        }

    const BaseProjectProcessingLock processingLock(dynamic_cast<BatchProjectDoc*>(GetDocument()));

    wxProgressDialog progressDlg(
        wxFileName::StripExtension(doc->GetTitle()), _(L"Exporting project..."),
        static_cast<int>(
            (includeTestScores ? GetScoresView().GetWindowCount() : 0) +
            (includeGraphs ? GetBoxPlotView().GetWindowCount() : 0) +
            (includeGraphs ? GetHistogramsView().GetWindowCount() : 0) +
            (includeGrammarIssues ? GetGrammarView().GetWindowCount() : 0) +
            (includeSightWords ? GetDolchSightWordsView().GetWindowCount() : 0) +
            (includeHardWordLists ? GetWordsBreakdownView().GetWindowCount() : 0) +
            (includeSentencesBreakdown ? GetSentencesBreakdownView().GetWindowCount() : 0) +
            (includeSummaryStats ? GetSummaryStatsView().GetWindowCount() : 0) +
            (includeWarnings ? 1 : 0)),
        GetDocFrame(), wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_CAN_ABORT | wxPD_APP_MODAL);
    progressDlg.Centre();
    int counter{ 1 };

    // the results window
    if (includeTestScores && (GetScoresView().GetWindowCount() != 0U))
        {
        if (!wxFileName::Mkdir(folder + wxFileName::GetPathSeparator() + _DT(L"Readability Scores"),
                               wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            wxMessageBox(
                wxString::Format(_(L"Unable to create \"%s\" folder."), _DT(L"Readability Scores")),
                wxGetApp().GetAppName(), wxOK | wxICON_EXCLAMATION);
            }
        else
            {
            for (size_t i = 0; i < GetScoresView().GetWindowCount(); ++i)
                {
                wxWindow* activeWindow = GetScoresView().GetWindow(i);
                if (activeWindow != nullptr)
                    {
                    if (activeWindow->IsKindOf(CLASSINFO(Wisteria::UI::ListCtrlEx)))
                        {
                        auto* listWindow = dynamic_cast<Wisteria::UI::ListCtrlEx*>(activeWindow);
                        if (listWindow != nullptr)
                            {
                            listWindow->SetLabel(
                                wxString::Format(L"%s [%s]", listWindow->GetName(),
                                                 wxFileName::StripExtension(doc->GetTitle())));
                            listWindow->Save(folder + wxFileName::GetPathSeparator() +
                                                 _DT(L"Readability Scores") +
                                                 wxFileName::GetPathSeparator() +
                                                 listWindow->GetLabel() + listExt,
                                             Wisteria::UI::GridExportOptions());
                            }
                        }
                    else if (activeWindow->IsKindOf(CLASSINFO(Wisteria::Canvas)))
                        {
                        auto* graphWindow = dynamic_cast<Wisteria::Canvas*>(activeWindow);
                        if (graphWindow != nullptr)
                            {
                            graphWindow->SetLabel(
                                wxString::Format(L"%s [%s]", graphWindow->GetName(),
                                                 wxFileName::StripExtension(doc->GetTitle())));
                            graphWindow->Save(folder + wxFileName::GetPathSeparator() +
                                                  _DT(L"Readability Scores") +
                                                  wxFileName::GetPathSeparator() +
                                                  graphWindow->GetLabel() + graphExt,
                                              graphOptions);
                            }
                        }
                    }
                wxGetApp().Yield(true);
                if (!progressDlg.Update(counter++))
                    {
                    return false;
                    }
                }
            }
        }
    // the graphs
    if (includeGraphs &&
        ((GetHistogramsView().GetWindowCount() != 0U) || (GetBoxPlotView().GetWindowCount() != 0U)))
        {
        if (!wxFileName::Mkdir(folder + wxFileName::GetPathSeparator() +
                                   _DT(L"Graphs", DTExplanation::FilePath),
                               wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            // TRANSLATORS: "Graphs" should not be translated; these folder names should stay in
            // English for consistency.
            wxMessageBox(wxString::Format(_(L"Unable to create \"%s\" folder."), _DT(L"Graphs")),
                         wxGetApp().GetAppName(), wxOK | wxICON_EXCLAMATION);
            }
        else
            {
            for (size_t i = 0; i < GetHistogramsView().GetWindowCount(); ++i)
                {
                auto* graphWindow =
                    dynamic_cast<Wisteria::Canvas*>(GetHistogramsView().GetWindow(i));
                if (graphWindow != nullptr)
                    {
                    graphWindow->SetLabel(
                        wxString::Format(_(L"%s Histogram [%s]"), graphWindow->GetName(),
                                         wxFileName::StripExtension(doc->GetTitle())));
                    graphWindow->Save(folder + wxFileName::GetPathSeparator() + _(L"Graphs") +
                                          wxFileName::GetPathSeparator() + graphWindow->GetLabel() +
                                          graphExt,
                                      graphOptions);
                    }
                wxGetApp().Yield(true);
                if (!progressDlg.Update(counter++))
                    {
                    return false;
                    }
                }
            for (size_t i = 0; i < GetBoxPlotView().GetWindowCount(); ++i)
                {
                auto* graphWindow = dynamic_cast<Wisteria::Canvas*>(GetBoxPlotView().GetWindow(i));
                if (graphWindow != nullptr)
                    {
                    graphWindow->SetLabel(
                        wxString::Format(_(L"%s Box Plot [%s]"), graphWindow->GetName(),
                                         wxFileName::StripExtension(doc->GetTitle())));
                    graphWindow->Save(folder + wxFileName::GetPathSeparator() + _(L"Graphs") +
                                          wxFileName::GetPathSeparator() + graphWindow->GetLabel() +
                                          graphExt,
                                      graphOptions);
                    }
                wxGetApp().Yield(true);
                if (!progressDlg.Update(counter++))
                    {
                    return false;
                    }
                }
            }
        }
    // the hard word list views
    if (includeHardWordLists)
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
                auto* listWindow =
                    dynamic_cast<Wisteria::UI::ListCtrlEx*>(GetWordsBreakdownView().GetWindow(i));
                if (listWindow != nullptr)
                    {
                    listWindow->SetLabel(
                        wxString::Format(L"%s [%s]", listWindow->GetName(),
                                         wxFileName::StripExtension(doc->GetTitle())));
                    listWindow->Save(folder + wxFileName::GetPathSeparator() +
                                         _DT(L"Words Breakdown") + wxFileName::GetPathSeparator() +
                                         listWindow->GetLabel() + listExt,
                                     Wisteria::UI::GridExportOptions());
                    }
                wxGetApp().Yield(true);
                if (!progressDlg.Update(counter++))
                    {
                    return false;
                    }
                }
            }
        }
    // sentences breakdown
    if (includeSentencesBreakdown)
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
                auto* listWindow = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
                    GetSentencesBreakdownView().GetWindow(i));
                if (listWindow != nullptr)
                    {
                    listWindow->SetLabel(
                        wxString::Format(L"%s [%s]", listWindow->GetName(),
                                         wxFileName::StripExtension(doc->GetTitle())));
                    listWindow->Save(
                        folder + wxFileName::GetPathSeparator() + _DT(L"Sentences Breakdown") +
                            wxFileName::GetPathSeparator() + listWindow->GetLabel() + listExt,
                        Wisteria::UI::GridExportOptions());
                    }
                wxGetApp().Yield(true);
                if (!progressDlg.Update(counter++))
                    {
                    return false;
                    }
                }
            }
        }
    // summary statistics
    if (includeSummaryStats)
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
            for (size_t i = 0; i < GetSummaryStatsView().GetWindowCount(); ++i)
                {
                auto* listWindow =
                    dynamic_cast<Wisteria::UI::ListCtrlEx*>(GetSummaryStatsView().GetWindow(i));
                if (listWindow != nullptr)
                    {
                    listWindow->SetLabel(
                        wxString::Format(L"%s [%s]", listWindow->GetName(),
                                         wxFileName::StripExtension(doc->GetTitle())));
                    listWindow->Save(
                        folder + wxFileName::GetPathSeparator() + _DT(L"Summary Statistics") +
                            wxFileName::GetPathSeparator() + listWindow->GetLabel() + listExt,
                        Wisteria::UI::GridExportOptions());
                    }
                wxGetApp().Yield(true);
                if (!progressDlg.Update(counter++))
                    {
                    return false;
                    }
                }
            }
        }
    // grammar
    if (includeGrammarIssues && (GetGrammarView().GetWindowCount() != 0U))
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
                auto* listWindow =
                    dynamic_cast<Wisteria::UI::ListCtrlEx*>(GetGrammarView().GetWindow(i));
                if (listWindow != nullptr)
                    {
                    listWindow->SetLabel(
                        wxString::Format(L"%s [%s]", listWindow->GetName(),
                                         wxFileName::StripExtension(doc->GetTitle())));
                    listWindow->Save(folder + wxFileName::GetPathSeparator() + _DT(L"Grammar") +
                                         wxFileName::GetPathSeparator() + listWindow->GetLabel() +
                                         listExt,
                                     Wisteria::UI::GridExportOptions());
                    }
                wxGetApp().Yield(true);
                if (!progressDlg.Update(counter++))
                    {
                    return false;
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
                auto* listWindow =
                    dynamic_cast<Wisteria::UI::ListCtrlEx*>(GetDolchSightWordsView().GetWindow(i));
                if (listWindow != nullptr)
                    {
                    listWindow->SetLabel(
                        wxString::Format(L"%s [%s]", listWindow->GetName(),
                                         wxFileName::StripExtension(doc->GetTitle())));
                    listWindow->Save(folder + wxFileName::GetPathSeparator() + _DT(L"Sight Words") +
                                         wxFileName::GetPathSeparator() + listWindow->GetLabel() +
                                         listExt,
                                     Wisteria::UI::GridExportOptions());
                    }
                wxGetApp().Yield(true);
                if (!progressDlg.Update(counter++))
                    {
                    return false;
                    }
                }
            }
        }
    // warnings
    if (includeWarnings && (GetWarningsView()->GetItemCount() != 0))
        {
        if (!wxFileName::Mkdir(folder + wxFileName::GetPathSeparator() + _DT(L"Warnings"),
                               wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            wxMessageBox(wxString::Format(_(L"Unable to create \"%s\" folder."), _DT(L"Warnings")),
                         wxGetApp().GetAppName(), wxOK | wxICON_EXCLAMATION);
            }
        else
            {
            if (GetWarningsView()->GetItemCount() > 0)
                {
                GetWarningsView()->SetLabel(
                    wxString::Format(L"%s [%s]", GetWarningsView()->GetName(),
                                     wxFileName::StripExtension(doc->GetTitle())));
                GetWarningsView()->Save(folder + wxFileName::GetPathSeparator() + _DT(L"Warnings") +
                                            wxFileName::GetPathSeparator() +
                                            GetWarningsView()->GetLabel() + listExt,
                                        Wisteria::UI::GridExportOptions());
                }
            wxGetApp().Yield(true);
            if (!progressDlg.Update(counter++))
                {
                return false;
                }
            }
        }
    return true;
    }

//-------------------------------------------------------
bool BatchProjectView::ExportAllToHtml(
    const wxFileName& filePath, wxString graphExt, const bool includeHardWordLists,
    const bool includeSentencesBreakdown, const bool includeGraphs, const bool includeTestScores,
    const bool includeGrammarIssues, const bool includeSightWords, const bool includeWarnings,
    const bool includeSummaryStats, const Wisteria::UI::ImageExportOptions& graphOptions)
    {
    const auto* doc = dynamic_cast<const BatchProjectDoc*>(GetDocument());

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

    const BaseProjectProcessingLock processingLock(dynamic_cast<BatchProjectDoc*>(GetDocument()));

    const wxBusyCursor bc;
    const wxBusyInfo bi(wxBusyInfoFlags().Text(_(L"Exporting project...")).Parent(GetDocFrame()));
#ifdef __WXGTK__
    wxMilliSleep(100);
    wxGetApp().Yield();
#endif

    lily_of_the_valley::html_encode_text htmlEncode;
    wxString outputText;
    const wxString headSection =
        L"<head>" +
        wxString::Format(
            L"\n    <meta name='generator' content='%s %s' />"
            "\n    <title>%s</title>"
            "\n    <meta http-equiv='content-type' content='text/html; charset=utf-8' />"
            "\n    <link rel='stylesheet' href='style.css'>"
            "\n</head>",
            wxGetApp().GetAppDisplayName(), wxGetApp().GetAppVersion(), doc->GetTitle());

    size_t sectionCounter = 0;
    size_t figureCounter = 0;
    size_t tableCounter = 0;

    const wxString pageBreak = L"<div style='page-break-before:always'></div><br />\n";

    const auto formatImageOutput =
        [&outputText, &sectionCounter, &figureCounter, pageBreak, doc, htmlEncode, graphExt,
         graphOptions, filePath](Wisteria::Canvas* canvas, const bool includeLeadingPageBreak,
                                 const wxString& subFolder = wxString{})
    {
        if (canvas == nullptr)
            {
            return;
            }
        canvas->SetLabel(wxString::Format(L"%s [%s]", canvas->GetName(),
                                          wxFileName::StripExtension(doc->GetTitle())));
        canvas->Save(filePath.GetPathWithSep() + _DT(L"images") + subFolder +
                         wxFileName::GetPathSeparator() + canvas->GetLabel() + graphExt,
                     graphOptions);

        outputText += wxString::Format(
            L"%s\n<div class='minipage figure'>\n<img src='images%s\\%s' />\n"
            "<div class='caption'>%s</div>\n</div>\n",
            (includeLeadingPageBreak ? pageBreak : wxString{}), subFolder,
            canvas->GetLabel() + graphExt,
            wxString::Format(
                _(L"Figure %zu.%zu: %s"), sectionCounter, figureCounter++,
                htmlEncode({ canvas->GetName().wc_str(), canvas->GetName().length() }, true)
                    .c_str()));
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
            wxString::Format(
                _(L"Table %zu.%zu: %s"), sectionCounter, tableCounter++,
                htmlEncode({ list->GetName().wc_str(), list->GetName().length() }, true).c_str()));
        std::wstring htmlText{ buffer.wc_string() };
        lily_of_the_valley::html_format::strip_hyperlinks(htmlText);

        outputText += (includeLeadingPageBreak ? pageBreak : wxString{}) +
                      lily_of_the_valley::html_extract_text::get_body(htmlText);
    };

    bool hasSections{ false };

    // the results window
    if (includeTestScores && (GetScoresView().GetWindowCount() != 0U))
        {
        // the first output in this section will not have a leading page break, but the rest will
        bool includeLeadingPageBreak{ false };
        // update/reset counters for sections, tables, and figures
        ++sectionCounter;
        figureCounter = tableCounter = 1;
        outputText += wxString::Format(
            L"\n\n%s<div class=\"report-section\"><a name=\"scores\"></a>%s</div>\n",
            (hasSections ? pageBreak : wxString{}),
            htmlEncode(
                { GetReadabilityScoresLabel().wc_str(), GetReadabilityScoresLabel().length() },
                true));
        // indicates that a section has already been written out after the TOC so that we
        // know if we need to insert a page break in front of the next section
        hasSections = true;
        for (size_t i = 0; i < GetScoresView().GetWindowCount(); ++i)
            {
            wxWindow* activeWindow = GetScoresView().GetWindow(i);
            if (activeWindow != nullptr)
                {
                if (activeWindow->IsKindOf(CLASSINFO(Wisteria::UI::ListCtrlEx)))
                    {
                    formatList(dynamic_cast<Wisteria::UI::ListCtrlEx*>(activeWindow),
                               includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                else if (activeWindow->IsKindOf(CLASSINFO(Wisteria::Canvas)))
                    {
                    formatImageOutput(dynamic_cast<Wisteria::Canvas*>(activeWindow),
                                      includeLeadingPageBreak);
                    includeLeadingPageBreak = true;
                    }
                }
            }
        }
    // the graphs
    if (includeGraphs &&
        ((GetHistogramsView().GetWindowCount() != 0U) || (GetBoxPlotView().GetWindowCount() != 0U)))
        {
        bool includeLeadingPageBreak{ false };
        ++sectionCounter;
        figureCounter = tableCounter = 1;
        outputText += wxString::Format(
            L"\n\n%s<div class=\"report-section\"><a name=\"histograms\"></a>%s</div>\n",
            (hasSections ? pageBreak : wxString{}),
            htmlEncode({ GetHistogramsLabel().wc_str(), GetHistogramsLabel().length() }, true));
        hasSections = true;
        for (size_t i = 0; i < GetHistogramsView().GetWindowCount(); ++i)
            {
            formatImageOutput(dynamic_cast<Wisteria::Canvas*>(GetHistogramsView().GetWindow(i)),
                              includeLeadingPageBreak,
                              wxFileName::GetPathSeparator() + wxString(_DT(L"histograms")));
            includeLeadingPageBreak = true;
            }
        outputText += wxString::Format(
            L"\n\n%s<div class=\"report-section\"><a name=\"box-plots\"></a>%s</div>\n", pageBreak,
            htmlEncode({ GetBoxPlotsLabel().wc_str(), GetBoxPlotsLabel().length() }, true));
        includeLeadingPageBreak = false; // reset for new subsection
        for (size_t i = 0; i < GetBoxPlotView().GetWindowCount(); ++i)
            {
            formatImageOutput(dynamic_cast<Wisteria::Canvas*>(GetBoxPlotView().GetWindow(i)),
                              includeLeadingPageBreak,
                              wxFileName::GetPathSeparator() + wxString(_DT(L"box-plots")));
            includeLeadingPageBreak = true;
            }
        }
    // the word breakdowns lists
    if (includeHardWordLists)
        {
        bool includeLeadingPageBreak{ false };
        ++sectionCounter;
        figureCounter = tableCounter = 1;
        outputText += wxString::Format(
            L"\n\n%s<div class=\"report-section\"><a name=\"hardwordlist\"></a>%s</div>\n",
            (hasSections ? pageBreak : wxString{}),
            htmlEncode({ GetWordsBreakdownLabel().wc_str(), GetWordsBreakdownLabel().length() },
                       true));
        hasSections = true;
        for (size_t i = 0; i < GetWordsBreakdownView().GetWindowCount(); ++i)
            {
            formatList(
                dynamic_cast<Wisteria::UI::ListCtrlEx*>(GetWordsBreakdownView().GetWindow(i)),
                includeLeadingPageBreak);
            includeLeadingPageBreak = true;
            }
        }
    // the sentences breakdowns lists
    if (includeSentencesBreakdown)
        {
        bool includeLeadingPageBreak{ false };
        ++sectionCounter;
        figureCounter = tableCounter = 1;
        outputText += wxString::Format(
            L"\n\n%s<div class=\"report-section\"><a name=\"sentencebreakdown\"></a>%s</div>\n",
            (hasSections ? pageBreak : wxString{}),
            htmlEncode(
                { GetSentencesBreakdownLabel().wc_str(), GetSentencesBreakdownLabel().length() },
                true));
        hasSections = true;
        for (size_t i = 0; i < GetSentencesBreakdownView().GetWindowCount(); ++i)
            {
            formatList(
                dynamic_cast<Wisteria::UI::ListCtrlEx*>(GetSentencesBreakdownView().GetWindow(i)),
                includeLeadingPageBreak);
            includeLeadingPageBreak = true;
            }
        }
    // summary stats list
    if (includeSummaryStats)
        {
        bool includeLeadingPageBreak{ false };
        ++sectionCounter;
        figureCounter = tableCounter = 1;
        outputText += wxString::Format(
            L"\n\n%s<div class=\"report-section\"><a name=\"summarystats\"></a>%s</div>\n",
            (hasSections ? pageBreak : wxString{}),
            htmlEncode(
                { GetSummaryStatisticsLabel().wc_str(), GetSummaryStatisticsLabel().length() },
                true));
        hasSections = true;
        for (size_t i = 0; i < GetSummaryStatsView().GetWindowCount(); ++i)
            {
            formatList(dynamic_cast<Wisteria::UI::ListCtrlEx*>(GetSummaryStatsView().GetWindow(i)),
                       includeLeadingPageBreak);
            includeLeadingPageBreak = true;
            }
        }
    // grammar
    if (includeGrammarIssues && (GetGrammarView().GetWindowCount() != 0U))
        {
        bool includeLeadingPageBreak{ false };
        ++sectionCounter;
        figureCounter = tableCounter = 1;
        outputText += wxString::Format(
            L"\n\n%s<div class=\"report-section\"><a name=\"grammar\"></a>%s</div>\n",
            (hasSections ? pageBreak : wxString{}),
            htmlEncode({ GetGrammarLabel().wc_str(), GetGrammarLabel().length() }, true));
        hasSections = true;
        for (size_t i = 0; i < GetGrammarView().GetWindowCount(); ++i)
            {
            formatList(dynamic_cast<Wisteria::UI::ListCtrlEx*>(GetGrammarView().GetWindow(i)),
                       includeLeadingPageBreak);
            includeLeadingPageBreak = true;
            }
        }
    // Sight Words
    if (includeSightWords && (GetDolchSightWordsView().GetWindowCount() != 0U))
        {
        bool includeLeadingPageBreak{ false };
        ++sectionCounter;
        figureCounter = tableCounter = 1;
        outputText += wxString::Format(
            L"\n\n%s<div class=\"report-section\"><a name=\"dolch\"></a>%s</div>\n",
            (hasSections ? pageBreak : wxString{}),
            htmlEncode({ GetDolchLabel().wc_str(), GetDolchLabel().length() }, true));
        hasSections = true;
        for (size_t i = 0; i < GetDolchSightWordsView().GetWindowCount(); ++i)
            {
            formatList(
                dynamic_cast<Wisteria::UI::ListCtrlEx*>(GetDolchSightWordsView().GetWindow(i)),
                includeLeadingPageBreak);
            includeLeadingPageBreak = true;
            }
        }
    // warnings
    if (includeWarnings && GetWarningsView()->GetItemCount() > 0)
        {
        ++sectionCounter;
        figureCounter = tableCounter = 1;
        outputText += wxString::Format(
            L"\n\n%s<div class=\"report-section\"><a name=\"warnings\"></a>%s</div>\n",
            (hasSections ? pageBreak : wxString{}),
            htmlEncode({ GetWarningLabel().wc_str(), GetWarningLabel().length() }, true));
        formatList(GetWarningsView(), false);
        }
    wxString toc, infoTable;
    infoTable = wxString::Format(
        L"<div class='report-info-table'>\n"
        "<div class='report-header'>\n"
        "<div class='report-header-inner-cell report-header-first-column'>%s</div>\n"
        "<div class='report-header-inner-cell'>%s</div>\n"
        "<div class='report-header-inner-cell report-header-first-column'>%s</div>\n"
        "<div class='report-header-inner-cell'>%s</div>\n"
        "<div class='report-header-inner-cell report-header-first-column'>%s</div>\n"
        "<div class='report-header-inner-cell'>%s</div>\n"
        "<div class='report-header-first-column'>%s</div>\n"
        "<div>%s</div>\n"
        "</div>\n"
        "</div>",
        _(L"Project Title"), doc->GetTitle(), _(L"Status"), doc->GetStatus(), _(L"Reviewer"),
        doc->GetReviewer(), _(L"Date"), wxDateTime::Now().FormatDate());

    if (includeTestScores && (GetScoresView().GetWindowCount() != 0U))
        {
        toc += L"<a href=\"#scores\">" + GetReadabilityScoresLabel() + L"</a><br />\r\n";
        }
    if (includeGraphs && (GetHistogramsView().GetWindowCount() != 0U))
        {
        toc += L"<a href=\"#histograms\">" + GetHistogramsLabel() + L"</a><br />\r\n";
        }
    if (includeGraphs && (GetBoxPlotView().GetWindowCount() != 0U))
        {
        toc += L"<a href=\"#box-plots\">" + GetBoxPlotsLabel() + L"</a><br />\r\n";
        }
    if (includeHardWordLists)
        {
        toc += L"<a href=\"#hardwordlist\">" + GetWordsBreakdownLabel() + L"</a><br />\r\n";
        }
    if (includeSentencesBreakdown)
        {
        toc +=
            L"<a href=\"#sentencebreakdown\">" + GetSentencesBreakdownLabel() + L"</a><br />\r\n";
        }
    if (includeSummaryStats)
        {
        toc += L"<a href=\"#summarystats\">" + GetSummaryStatisticsLabel() + L"</a><br />\r\n";
        }
    if (includeGrammarIssues && (GetGrammarView().GetWindowCount() != 0U))
        {
        toc += L"<a href=\"#grammar\">" + GetGrammarLabel() + L"</a><br />\r\n";
        }
    if (includeSightWords && (GetDolchSightWordsView().GetWindowCount() != 0U))
        {
        toc += L"<a href=\"#dolch\">" + GetDolchLabel() + L"</a><br />\r\n";
        }
    if (includeWarnings)
        {
        toc += L"<a href=\"#warnings\">" + GetWarningLabel() + L"</a><br />\r\n";
        }
    outputText.insert(0, L"<!DOCTYPE html>\n<html>\n" + headSection + L"\r\n    </style>" +
                             L"\r\n</head>\r\n<body>\r\n" + infoTable +
                             L"\r\n<div class=\"toc-section no-print\">" + toc + L"</div>");
    outputText += L"\r\n</body>\r\n</html>";

    // copy over the CSS file
    const wxString cssTemplatePath = wxGetApp().FindResourceDirectory(_DT(L"report-themes")) +
                                     wxFileName::GetPathSeparator() + L"default.css";
    const wxString cssPath = filePath.GetPathWithSep() + L"style.css";
    if (wxFileName::FileExists(cssTemplatePath))
        {
        if (!wxCopyFile(cssTemplatePath, cssPath, true))
            {
            wxLogWarning(L"Failed to copy CSS file '%s' to '%s'.", cssTemplatePath, cssPath);
            }
        }

    wxFileName(filePath.GetFullPath()).SetPermissions(wxS_DEFAULT);
    wxFile file(filePath.GetFullPath(), wxFile::write);
    return file.Write(outputText);
    }

//---------------------------------------------------
void BatchProjectView::OnRibbonButtonCommand(wxRibbonButtonBarEvent& event)
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

//---------------------------------------------------
void BatchProjectView::OnExportScoresAndStatistics([[maybe_unused]] wxCommandEvent& event)
    {
    const auto* doc = dynamic_cast<const BatchProjectDoc*>(GetDocument());

    wxFileDialog fdialog(GetDocFrame(), _(L"Export Scores & Statistics"), wxString{},
                         wxString::Format(
                             // TRANSLATORS: %s is document title
                             _(L"%s Scores & Statistics"), doc->GetTitle()),
                         _(L"HTML Files (*.htm;*.html)|*.htm;*.html"),
                         wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (fdialog.ShowModal() != wxID_OK)
        {
        return;
        }

    const Wisteria::UI::ListCtrlEx* list = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        GetScoresView().FindWindowById(ID_SCORE_LIST_PAGE_ID));
    wxASSERT(list);
    // shouldn't happen
    if (list == nullptr)
        {
        return;
        }

    wxString htmlText =
        wxString::Format(
            L"<!DOCTYPE html>\n<html>\n<head>\n    "
            "<title>%s</title>\n</head>\n<body>\n<table border='1' style='width:100%%; "
            "border-collapse:collapse;'>",
            _(L"Scores &amp; Statistics")) +
        wxString::Format(
            L"\n<tr style='background:%s;'>",
            ProjectReportFormat::GetReportHeaderColor().GetAsString(wxC2S_HTML_SYNTAX));
    const wxString tdStart = wxString::Format(
        L"\n<td><span style='color:%s;'>",
        ProjectReportFormat::GetReportHeaderFontColor().GetAsString(wxC2S_HTML_SYNTAX));
    for (long colCount = 0; colCount < list->GetColumnCount(); ++colCount)
        {
        htmlText += tdStart + list->GetColumnName(colCount) + L"</span></td>";
        }
    if (doc->GetStatisticsReportInfo().IsParagraphEnabled())
        {
        htmlText += tdStart + _(L"Number of Paragraphs") + L"</span></td>";
        }
    if (doc->GetStatisticsReportInfo().IsSentencesEnabled())
        {
        htmlText += tdStart + _(L"Number of Sentences") + L"</span></td>";
        }
    if (doc->GetStatisticsReportInfo().IsWordsEnabled())
        {
        htmlText += tdStart + _(L"Number of Words") + L"</span></td>";
        }
    if (doc->GetStatisticsReportInfo().IsExtendedInformationEnabled())
        {
        htmlText += tdStart + _(L"Text Size") + L"</span></td>";
        }
    htmlText += L"</tr>";
    for (long rowCount = 0; rowCount < list->GetItemCount(); ++rowCount)
        {
        htmlText += L"\n<tr>";
        for (long colCount = 0; colCount < list->GetColumnCount(); ++colCount)
            {
            htmlText += L"<td>" + list->GetItemTextEx(rowCount, colCount) + L"</td>";
            }
        const BaseProject* subDoc = doc->GetDocument(list->GetItemTextEx(rowCount, 0));
        if (subDoc != nullptr)
            {
            if (doc->GetStatisticsReportInfo().IsParagraphEnabled())
                {
                htmlText += wxString::Format(
                    L"<td>%s</td>", wxNumberFormatter::ToString(
                                        subDoc->GetTotalParagraphs(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));
                }
            if (doc->GetStatisticsReportInfo().IsSentencesEnabled())
                {
                htmlText += wxString::Format(
                    L"<td>%s</td>", wxNumberFormatter::ToString(
                                        subDoc->GetTotalSentences(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));
                }
            if (doc->GetStatisticsReportInfo().IsWordsEnabled())
                {
                htmlText += wxString::Format(
                    L"<td>%s</td>", wxNumberFormatter::ToString(
                                        subDoc->GetTotalWords(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));
                }
            if (doc->GetStatisticsReportInfo().IsExtendedInformationEnabled())
                {
                htmlText +=
                    L"<td>" +
                    wxString::Format(
                        // TRANSLATORS: %s is number of kilobytes in a file
                        _(L"%s Kbs."), wxNumberFormatter::ToString(
                                           safe_divide<double>(subDoc->GetTextSize(), 1024), 2,
                                           wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                               wxNumberFormatter::Style::Style_WithThousandsSep)) +
                    L"</td>";
                }
            }
        htmlText += L"</tr>";
        }
    htmlText += L"\n</table>\n</body>\n</html>";
    std::wstring strippedHTML{ htmlText };
    lily_of_the_valley::html_format::set_encoding(strippedHTML);
    wxFileName(fdialog.GetPath()).SetPermissions(wxS_DEFAULT);
    wxFile outFile(fdialog.GetPath(), wxFile::write);
    outFile.Write(strippedHTML);
    }

//---------------------------------------------------
void BatchProjectView::OnExportStatisticsReport([[maybe_unused]] wxCommandEvent& event)
    {
    auto* doc = dynamic_cast<BatchProjectDoc*>(GetDocument());
    wxFileDialog fdialog(GetDocFrame(), _(L"Export Statistics Report"), wxString{},
                         wxString::Format(
                             // TRANSLATORS: %s is document title
                             _(L"%s Summary Statistics Report"), doc->GetTitle()),
                         _(L"HTML Files (*.htm;*.html)|*.htm;*.html"),
                         wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (fdialog.ShowModal() != wxID_OK)
        {
        return;
        }

    wxProgressDialog progressDlg(_(L"Exporting"), _(L"Exporting statistics..."),
                                 static_cast<int>((doc->GetDocuments().size() * 2) + 1), nullptr,
                                 wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_ELAPSED_TIME | wxPD_CAN_ABORT |
                                     wxPD_APP_MODAL);
    int counter{ 1 };

    const wxString fileHeader =
        ProjectReportFormat::FormatHtmlReportStart(_(L"Summary Statistics")) +
        ProjectReportFormat::FormatReportBanner(_(L"Summary Statistics"), doc->GetTitle()) +
        L"\n<h2>" + _(L"Files:") + L"</h2>\n";
    std::wstring strippedFileHeader{ fileHeader };
    lily_of_the_valley::html_format::strip_body_attributes(strippedFileHeader);

    wxTempFile outputFile(fdialog.GetPath());
    if (!outputFile.Write(strippedFileHeader))
        {
        wxMessageBox(_(L"Unable to write to output file."), _(L"Error"), wxOK | wxICON_EXCLAMATION);
        return;
        }

    html_utilities::html_strip_hyperlinks stripLinks;
    // write the TOC section
    for (size_t i = 0; i < doc->GetDocuments().size(); ++i)
        {
        wxGetApp().Yield(true);
        if (!progressDlg.Update(counter++))
            {
            return;
            }

        wxString currentDocName;
        if (doc->GetFilePathTruncationMode() ==
            Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode::TruncatePaths)
            {
            currentDocName =
                GetShortenedFilePath(doc->GetDocuments()[i]->GetOriginalDocumentFilePath());
            }
        else if (doc->GetFilePathTruncationMode() ==
                 Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode::
                     OnlyShowFileNames)
            {
            const wxFileName fn(doc->GetDocuments()[i]->GetOriginalDocumentFilePath());
            currentDocName = fn.GetFullName().empty() ?
                                 doc->GetDocuments()[i]->GetOriginalDocumentFilePath() :
                                 fn.GetFullName();
            }
        else
            {
            currentDocName = doc->GetDocuments()[i]->GetOriginalDocumentFilePath();
            }

        if (!outputFile.Write(L"<a href=\"#" + wxString::Format(L"Doc%zu", i) + L"\">" +
                              currentDocName + L"</a><br />\n"))
            {
            wxMessageBox(_(L"Unable to write to output file."), _(L"Error"),
                         wxOK | wxICON_EXCLAMATION);
            return;
            }
        }

    if (!outputFile.Write(L"<br /><br />\n"))
        {
        wxMessageBox(_(L"Unable to write to output file."), _(L"Error"), wxOK | wxICON_EXCLAMATION);
        return;
        }

    for (size_t i = 0; i < doc->GetDocuments().size(); ++i)
        {
        wxGetApp().Yield(true);
        if (!progressDlg.Update(counter++))
            {
            return;
            }

        wxString currentDocName;
        if (doc->GetFilePathTruncationMode() ==
            Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode::TruncatePaths)
            {
            currentDocName =
                GetShortenedFilePath(doc->GetDocuments()[i]->GetOriginalDocumentFilePath());
            }
        else if (doc->GetFilePathTruncationMode() ==
                 Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode::
                     OnlyShowFileNames)
            {
            const wxFileName fn(doc->GetDocuments()[i]->GetOriginalDocumentFilePath());
            currentDocName = fn.GetFullName().empty() ?
                                 doc->GetDocuments()[i]->GetOriginalDocumentFilePath() :
                                 fn.GetFullName();
            }
        else
            {
            currentDocName = doc->GetDocuments()[i]->GetOriginalDocumentFilePath();
            }

        wxString formattedStats = ProjectReportFormat::FormatStatisticsInfo(
            doc->GetDocuments()[i], doc->GetStatisticsReportInfo(),
            wxSystemSettings::GetColour(wxSYS_COLOUR_HOTLIGHT), nullptr);
        if (stripLinks(formattedStats.wc_str(), formattedStats.length()) != nullptr)
            {
            formattedStats.assign(stripLinks.get_filtered_text(),
                                  stripLinks.get_filtered_text_length());
            }

        if (!outputFile.Write(L"<a name=\"" + wxString::Format(L"Doc%zu", i) +
                              L"\"></a><span style='font-weight:bold;'>" + currentDocName +
                              L"</span><hr>" + formattedStats + L"<br /><br />\n"))
            {
            wxMessageBox(_(L"Unable to write to output file."), _(L"Error"),
                         wxOK | wxICON_EXCLAMATION);
            return;
            }
        }

    if (!outputFile.Write(ProjectReportFormat::FormatHtmlReportEnd()))
        {
        wxMessageBox(_(L"Unable to write to output file."), _(L"Error"), wxOK | wxICON_EXCLAMATION);
        return;
        }

    if (!outputFile.Commit())
        {
        wxMessageBox(_(L"Unable to write to output file."), _(L"Error"), wxOK | wxICON_EXCLAMATION);
        return;
        }
    progressDlg.Update(counter++);
    }

//---------------------------------------------------
void BatchProjectView::OnMenuCommand(wxCommandEvent& event)
    {
    auto* doc = dynamic_cast<BatchProjectDoc*>(GetDocument());
    if (!doc->IsSafeToUpdate())
        {
        return;
        }
    // Propagate standard save command to active subwindow if "export window" option selected
    if (event.GetId() == XRCID("ID_SAVE_ITEM"))
        {
        event.SetId(wxID_SAVE);
        }
    else if (event.GetId() == XRCID("ID_PRINT"))
        {
        event.SetId(wxID_PRINT);
        }
    else if (event.GetId() == XRCID("ID_EXPORT_ALL"))
        {
        wxArrayString choices, descriptions;
        choices.Add(_(L"Single report"));
        descriptions.Add(_(L"Saves the results into a single <span style='font-weight: bold;'>"
                           "HTML</span> report."));
        choices.Add(_(L"Separate files"));
        descriptions.Add(_(L"Saves each result window to a separate file."));
        Wisteria::UI::RadioBoxDlg exportTypesDlg(GetDocFrame(), _(L"Select How to Export"),
                                                 wxString{}, _(L"Export methods:"),
                                                 _(L"Export All"), choices, descriptions);
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
            ExportAllToHtml(dlg.GetFilePath(), dlg.GetExportGraphExt(),
                            dlg.IsExportingHardWordLists(), dlg.IsExportingSentencesBreakdown(),
                            dlg.IsExportingGraphs(), dlg.IsExportingTestResults(),
                            dlg.IsExportingGrammar(), dlg.IsExportingSightWords(),
                            dlg.IsExportingWarnings(), dlg.IsExportingStatistics(),
                            dlg.GetImageExportOptions());
            }
        else
            {
            ExportAll(dlg.GetFolderPath(), dlg.GetExportListExt(), dlg.GetExportGraphExt(),
                      dlg.IsExportingHardWordLists(), dlg.IsExportingSentencesBreakdown(),
                      dlg.IsExportingGraphs(), dlg.IsExportingTestResults(),
                      dlg.IsExportingGrammar(), dlg.IsExportingSightWords(),
                      dlg.IsExportingWarnings(), dlg.IsExportingStatistics(),
                      dlg.GetImageExportOptions());
            }
        doc->SetExportFile(dlg.GetFilePath());
        doc->SetExportFolder(dlg.GetFolderPath());
        BatchProjectDoc::SetExportListExt(dlg.GetExportListExt());
        BatchProjectDoc::SetExportGraphExt(dlg.GetExportGraphExt());
        BatchProjectDoc::ExportHardWordLists(dlg.IsExportingHardWordLists());
        BatchProjectDoc::ExportSentencesBreakdown(dlg.IsExportingSentencesBreakdown());
        BatchProjectDoc::ExportGraphs(dlg.IsExportingGraphs());
        BatchProjectDoc::ExportTestResults(dlg.IsExportingTestResults());
        BatchProjectDoc::ExportWordiness(dlg.IsExportingGrammar());
        BatchProjectDoc::ExportSightWords(dlg.IsExportingSightWords());
        BatchProjectDoc::ExportWarnings(dlg.IsExportingWarnings());
        BatchProjectDoc::GetImageExportOptions() = dlg.GetImageExportOptions();
        // export folder paths may have changed
        doc->SetModifiedFlag();
        return;
        }
    else if (event.GetId() == XRCID("ID_SEND_TO_STANDARD_PROJECT") ||
             event.GetId() == XRCID("ID_LAUNCH_SOURCE_FILE") ||
             event.GetId() == XRCID("ID_EXPORT_FILTERED_DOCUMENT"))
        {
        // Get the active window, and if it is a list with a valid file path selected then create
        // a new project from that. Otherwise, create a project from the selected file in the
        // scores list.
        wxWindow* window = GetActiveProjectWindow();
        const Wisteria::UI::ListCtrlEx* activeListCtrl = nullptr;
        if ((window != nullptr) && window->IsKindOf(CLASSINFO(Wisteria::UI::ListCtrlEx)) &&
            IsFilenameList(window->GetId()))
            {
            activeListCtrl = dynamic_cast<Wisteria::UI::ListCtrlEx*>(window);
            }
        else
            {
            wxMessageBox(_(L"Select a list that contains document names."), wxGetApp().GetAppName(),
                         wxOK | wxICON_INFORMATION);
            return;
            }
        if (activeListCtrl == nullptr)
            {
            return;
            }
        std::vector<std::pair<wxString, wxString>> selectedFilePaths;
        long item = -1;
        while (true)
            {
            item = activeListCtrl->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            if (item == wxNOT_FOUND)
                {
                break;
                }
            selectedFilePaths.emplace_back(
                activeListCtrl->GetItemTextEx(item, 0),
                activeListCtrl->GetItemTextEx(item, 1).Trim(true).Trim(false));
            }
        if (selectedFilePaths.empty())
            {
            wxMessageBox(_(L"Select a document in the list."), wxGetApp().GetAppName(),
                         wxOK | wxICON_INFORMATION);
            return;
            }

        for (const auto& selectedFilePath : selectedFilePaths)
            {
            if (event.GetId() == XRCID("ID_EXPORT_FILTERED_DOCUMENT"))
                {
                const BaseProject* subProject = doc->GetDocument(selectedFilePath.first);
                if (subProject == nullptr)
                    {
                    wxMessageBox(_(L"Unable to find selected subproject."), _(L"Error"),
                                 wxOK | wxICON_ERROR);
                    return;
                    }

                wxFileDialog fdialog(
                    GetDocFrame(), _(L"Export Filtered Document"), wxString{},
                    wxFileName(subProject->GetOriginalDocumentFilePath()).GetName(),
                    _(L"Text Files (*.txt)|*.txt"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
                if (fdialog.ShowModal() != wxID_OK)
                    {
                    return;
                    }

                FilteredTextExportOptionsDlg optDlg(GetDocFrame());
                optDlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                                    L"online/publishing.html");
                if (optDlg.ShowModal() != wxID_OK)
                    {
                    return;
                    }

                std::wstring validDocText;
                subProject->FormatFilteredText(
                    validDocText, optDlg.IsReplacingCharacters(), optDlg.IsRemovingEllipses(),
                    optDlg.IsRemovingBullets(), optDlg.IsRemovingFilePaths(),
                    optDlg.IsStrippingAbbreviations(), optDlg.IsNarrowingFullWidthCharacters());

                FilteredTextPreviewDlg dlg(
                    GetDocFrame(), subProject->GetInvalidSentenceMethod(),
                    subProject->IsExcludingTrailingCopyrightNoticeParagraphs(),
                    subProject->IsExcludingTrailingCitations(), optDlg.IsReplacingCharacters(),
                    optDlg.IsRemovingEllipses(), optDlg.IsRemovingBullets(),
                    optDlg.IsRemovingFilePaths(), optDlg.IsStrippingAbbreviations());
                dlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                                 L"online/analysis-notes.html");
                dlg.SetFilteredValue(validDocText);
                if (dlg.ShowModal() == wxID_OK)
                    {
                    wxFileName(fdialog.GetPath()).SetPermissions(wxS_DEFAULT);
                    wxFile filteredFile(fdialog.GetPath(), wxFile::write);
                    if (!filteredFile.Write(validDocText, wxConvUTF8))
                        {
                        wxMessageBox(_(L"Unable to write to output file."), _(L"Error"),
                                     wxOK | wxICON_EXCLAMATION);
                        }
                    }
                }
            else if (event.GetId() == XRCID("ID_SEND_TO_STANDARD_PROJECT"))
                {
                // create a standard project and dump the text into it
                const wxList& templateList = wxGetApp().GetDocManager()->GetTemplates();
                for (size_t i = 0; i < templateList.GetCount(); ++i)
                    {
                    wxDocTemplate* docTemplate =
                        dynamic_cast<wxDocTemplate*>(templateList.Item(i)->GetData());
                    if ((docTemplate != nullptr) &&
                        docTemplate->GetDocClassInfo()->IsKindOf(CLASSINFO(ProjectDoc)))
                        {
                        // Silently create the document to suppress the project wizard,
                        // and then copy the batch project settings into this new project
                        // so that it gets analyzed the same way.
                        // Also, if there were any special settings for this subproject,
                        // then copy over its settings on top of the batch project's settings.
                        auto* const newDoc = dynamic_cast<ProjectDoc*>(
                            docTemplate->CreateDocument(selectedFilePath.first, wxDOC_SILENT));
                        if (newDoc == nullptr)
                            {
                            return;
                            }
                        newDoc->CopyDocumentLevelSettings(*doc, false);
                        const BaseProject* subDocument = doc->GetDocument(selectedFilePath.first);
                        if (subDocument != nullptr)
                            {
                            newDoc->CopySettings(*subDocument);
                            }
                        newDoc->SetUIMode(true);
                        // set the document path
                        newDoc->SetOriginalDocumentFilePath(selectedFilePath.first);
                        newDoc->SetOriginalDocumentDescription(selectedFilePath.second);
                        if (!newDoc->OnNewDocument())
                            {
                            // Document is implicitly deleted by DeleteAllViews
                            newDoc->DeleteAllViews();
                            return;
                            }
                        if (newDoc->GetFirstView() != nullptr)
                            {
                            const auto selectedItem = GetSideBar()->GetSelectedSubItemId();

                            if (newDoc->GetDocumentWindow() != nullptr)
                                {
                                newDoc->GetDocumentWindow()->SetFocus();
                                }
                            newDoc->GetFirstView()->Activate(true);
                            wxGetApp().GetDocManager()->ActivateView(newDoc->GetFirstView());

                            // try to select in the new project the same sidebar selection
                            // from the batch project
                            auto* view = dynamic_cast<ProjectView*>(newDoc->GetFirstView());
                            const auto selectedIndex = view->GetSideBar()->FindSubItem(
                                selectedItem.first, selectedItem.second);
                            if (selectedIndex.first.has_value() && selectedIndex.second.has_value())
                                {
                                view->GetSideBar()->SelectSubItem(selectedIndex.first.value(),
                                                                  selectedIndex.second.value());
                                }
                            else
                                {
                                const auto selectedId = GetSideBar()->GetSelectedFolderId();
                                if (selectedId.has_value())
                                    {
                                    view->GetSideBar()->SelectFolder(
                                        view->GetSideBar()->FindFolder(selectedId.value()));
                                    }
                                }
                            }
                        break;
                        }
                    }
                }
            else if (event.GetId() == XRCID("ID_LAUNCH_SOURCE_FILE"))
                {
                const FilePathResolver resolvePath(selectedFilePath.first, false);
                if (doc->GetDocumentStorageMethod() == TextStorage::EmbedText)
                    {
                    std::vector<BaseProject*>::iterator subDocPos;
                    for (subDocPos = doc->GetDocuments().begin();
                         subDocPos != doc->GetDocuments().end(); ++subDocPos)
                        {
                        if (CompareFilePaths((*subDocPos)->GetOriginalDocumentFilePath(),
                                             selectedFilePath.first) == 0)
                            {
                            break;
                            }
                        }
                    // if not found then bail (shouldn't happen)
                    if (subDocPos == doc->GetDocuments().end())
                        {
                        return;
                        }
                    // don't bother asking them about relinking if a file is inside an archive
                    if (!resolvePath.IsArchivedFile())
                        {
                        auto warningIter =
                            WarningManager::GetWarning(_DT(L"linked-document-is-embedded"));
                        // if they want to be prompted for this...
                        if (warningIter != WarningManager::GetWarnings().end() &&
                            warningIter->ShouldBeShown())
                            {
                            wxRichMessageDialog msg(GetDocFrame(), warningIter->GetMessage(),
                                                    warningIter->GetTitle(),
                                                    warningIter->GetFlags());
                            msg.SetEscapeId(wxID_NO);
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
                                for (auto& subDoc : doc->GetDocuments())
                                    {
                                    subDoc->SetDocumentStorageMethod(TextStorage::NoEmbedText);
                                    }
                                wxLaunchDefaultApplication(selectedFilePath.first);
                                return;
                                }
                            }
                        // or if they said "yes" before, then use the found path
                        else if (warningIter != WarningManager::GetWarnings().end() &&
                                 warningIter->GetPreviousResponse() == wxID_YES)
                            {
                            doc->SetModifiedFlag();
                            doc->SetDocumentStorageMethod(TextStorage::NoEmbedText);
                            for (auto& subDoc : doc->GetDocuments())
                                {
                                subDoc->SetDocumentStorageMethod(TextStorage::NoEmbedText);
                                }
                            wxLaunchDefaultApplication(selectedFilePath.first);
                            return;
                            }
                        }

                    EditTextDlg dlg(
                        GetDocFrame(), doc, (*subDocPos)->GetDocumentText(), wxID_ANY,
                        _(L"Edit Embedded Document"),
                        (!doc->GetAppendedDocumentText().empty()) ?
                            _(L"Note: The appended template document is not included here.\n"
                              "Only the embedded text is editable from this dialog.") :
                            wxString{});
                    if (dlg.ShowModal() == wxID_OK)
                        {
                        (*subDocPos)->SetDocumentText(dlg.GetValue().wc_string());
                        doc->Modify(true);
                        doc->RefreshRequired(ProjectRefresh::FullReindexing);
                        doc->RefreshProject();
                        }
                    }
                else
                    {
                    if (resolvePath.IsArchivedFile())
                        {
                        wxMessageBox(_(L"Files inside of archives files cannot be edited."),
                                     wxString{}, wxOK | wxICON_INFORMATION);
                        }
                    else if (resolvePath.IsExcelCell())
                        {
                        const size_t excelTag =
                            resolvePath.GetResolvedPath().MakeLower().find(_DT(L".xlsx#"));
                        const wxFileName fn(resolvePath.GetResolvedPath().substr(0, excelTag + 5));
                        wxLaunchDefaultApplication(fn.GetFullPath());
                        }
                    else
                        {
                        if (wxFileName::FileExists(resolvePath.GetResolvedPath()))
                            {
                            wxLaunchDefaultApplication(resolvePath.GetResolvedPath());
                            }
                        else
                            {
                            wxMessageBox(wxString::Format(_(L"File could not be found: %s"),
                                                          resolvePath.GetResolvedPath()),
                                         _(L"File Not Found"), wxOK | wxICON_INFORMATION);
                            }
                        }
                    }
                }
            }
        return;
        }

    // if they were just hitting Cancel then close
    if (event.GetEventType() == wxEVT_COMMAND_FIND_CLOSE)
        {
        return;
        }

    if ((GetActiveProjectWindow() != nullptr) &&
        GetActiveProjectWindow()->IsKindOf(CLASSINFO(Wisteria::UI::ListCtrlEx)))
        {
        /* just in case this is a print or preview command, update the window's headers
           and footer to whatever the global options currently are.*/
        doc->UpdateListOptions(dynamic_cast<Wisteria::UI::ListCtrlEx*>(GetActiveProjectWindow()));

        // in case we are exporting the window, set its label to include
        // the name of the document, and then reset it
        GetActiveProjectWindow()->SetLabel(
            wxString::Format(L"%s [%s]", GetActiveProjectWindow()->GetName(),
                             wxFileName::StripExtension(doc->GetTitle())));
        const ParentEventBlocker blocker(GetActiveProjectWindow());
        GetActiveProjectWindow()->ProcessWindowEvent(event);
        }
    else if ((GetActiveProjectWindow() != nullptr) &&
             GetActiveProjectWindow()->IsKindOf(CLASSINFO(Wisteria::Canvas)))
        {
        auto* activeWindow = dynamic_cast<Wisteria::Canvas*>(GetActiveProjectWindow());
        if (activeWindow != nullptr)
            {
            /* just in case this is a print or preview command update the window's headers
               and footer to whatever the global options currently are*/
            if (event.GetId() == wxID_PREVIEW || event.GetId() == wxID_PRINT)
                {
                activeWindow->SetPrinterSettings(*wxGetApp().GetPrintData());
                activeWindow->SetLeftPrinterHeader(
                    wxGetApp().GetAppOptions()->GetLeftPrinterHeader());
                activeWindow->SetCenterPrinterHeader(
                    wxGetApp().GetAppOptions()->GetCenterPrinterHeader());
                activeWindow->SetRightPrinterHeader(
                    wxGetApp().GetAppOptions()->GetRightPrinterHeader());
                activeWindow->SetLeftPrinterFooter(
                    wxGetApp().GetAppOptions()->GetLeftPrinterFooter());
                activeWindow->SetCenterPrinterFooter(
                    wxGetApp().GetAppOptions()->GetCenterPrinterFooter());
                activeWindow->SetRightPrinterFooter(
                    wxGetApp().GetAppOptions()->GetRightPrinterFooter());
                }

            activeWindow->SetLabel(wxString::Format(L"%s [%s]", activeWindow->GetName(),
                                                    wxFileName::StripExtension(doc->GetTitle())));
            const ParentEventBlocker blocker(activeWindow);
            activeWindow->ProcessWindowEvent(event);
            }
        }
    }

//---------------------------------------------------
void BatchProjectView::OnFind(wxFindDialogEvent& event)
    {
    // if they were just hitting Cancel then close
    if (event.GetEventType() == wxEVT_COMMAND_FIND_CLOSE)
        {
        return;
        }

    if (GetActiveProjectWindow() != nullptr)
        {
        const ParentEventBlocker blocker(GetActiveProjectWindow());
        GetActiveProjectWindow()->ProcessWindowEvent(event);
        }
    }

//---------------------------------------------------
void BatchProjectView::OnGradeScale(wxCommandEvent& event)
    {
    for (size_t i = 0; i < GetDocFrame()->m_gradeScaleMenu.GetMenuItemCount(); ++i)
        {
        GetDocFrame()->m_gradeScaleMenu.FindItemByPosition(i)->Check(false);
        }

    auto* doc = dynamic_cast<BatchProjectDoc*>(GetDocument());

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
    doc->RefreshRequired(ProjectRefresh::Minimal);
    doc->RefreshGraphs();
    if (GetSplitter()->GetWindow2()->IsKindOf(CLASSINFO(wxWindow)))
        {
        GetSplitter()->GetWindow2()->Refresh();
        }
    UpdateStatAndTestPanes(GetCurrentlySelectedFileName());

    // update this setting in the subprojects
    for (auto* subDoc : doc->GetDocuments())
        {
        subDoc->GetReadabilityMessageCatalog().SetGradeScale(gs);
        }

    doc->SetModifiedFlag();
    }
