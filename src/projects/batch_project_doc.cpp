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

#include "batch_project_doc.h"
#include "../Wisteria-Dataviz/src/base/reportenumconvert.h"
#include "../Wisteria-Dataviz/src/graphs/danielsonbryan2plot.h"
#include "../Wisteria-Dataviz/src/graphs/inflesz.h"
#include "../Wisteria-Dataviz/src/graphs/lixgauge.h"
#include "../Wisteria-Dataviz/src/graphs/lixgaugegerman.h"
#include "../Wisteria-Dataviz/src/graphs/wordcloud.h"
#include "../app/readability_app.h"
#include "../graphs/schwartzgraph.h"
#include "../indexing/character_traits.h"
#include "../results-format/project_report_format.h"
#include "../ui/dialogs/project_wizard_dlg.h"
#include "batch_project_view.h"
#include <algorithm>
#include <limits>
#include <wx/dir.h>
#include <wx/wfstream.h>

wxDECLARE_APP(ReadabilityApp);

wxIMPLEMENT_DYNAMIC_CLASS(BatchProjectDoc, wxDocument)

    //-------------------------------------------------------
    void BatchProjectDoc::ShowQueuedMessages()
    {
    auto* view = dynamic_cast<BaseProjectView*>(GetFirstView());
    for (const auto& queuedMsg : GetQueuedMessages())
        {
        view->ShowInfoMessage(queuedMsg);
        }
    }

//-------------------------------------------------------
void BatchProjectDoc::RemoveMisspellings(const wxArrayString& misspellingsToRemove)
    {
    traits::case_insensitive_wstring_ex reportStr;
    wxString searchStr;
    wxString multiFactorValue;
    for (size_t i = 0; i < GetMisspelledWordData()->GetItemCount(); ++i)
        {
        double totalCount = GetMisspelledWordData()->GetItemValue(i, 2);
        double uniqueCount = GetMisspelledWordData()->GetItemValue(i, 3);
        reportStr = GetMisspelledWordData()->GetItemText(i, 4);
        for (size_t mspCounter = 0; mspCounter < misspellingsToRemove.GetCount(); ++mspCounter)
            {
            searchStr = L"\"" + misspellingsToRemove[mspCounter] + L"\"";
            size_t index = reportStr.find(searchStr);
            if (index != wxString::npos)
                {
                size_t endIndex = reportStr.find(L',', index + searchStr.length());
                if (endIndex != wxString::npos)
                    {
                    size_t multiFactorIndex = reportStr.find(L'*', index + searchStr.length());
                    if (multiFactorIndex != wxString::npos && multiFactorIndex < endIndex)
                        {
                        // skip "* "
                        multiFactorIndex += 2;
                        multiFactorValue =
                            reportStr.substr(multiFactorIndex, endIndex - multiFactorIndex).c_str();
                        double val{ 0 };
                        if (multiFactorValue.ToDouble(&val))
                            {
                            assert(val > 0);
                            totalCount -= (val - 1 /* we will subtract 1 later*/);
                            }
                        }
                    // skip trailing ", "
                    endIndex += 2;
                    reportStr.erase(index, endIndex - index);
                    }
                else
                    {
                    // we will need to strip the trailing ", " after removing this word at the end
                    if (index >= 2)
                        {
                        index -= 2;
                        }
                    size_t multiFactorIndex = reportStr.find(L'*', index + searchStr.length());
                    if (multiFactorIndex != wxString::npos)
                        {
                        // skip "* "
                        multiFactorIndex += 2;
                        multiFactorValue = reportStr.substr(multiFactorIndex).c_str();
                        double val{ 0 };
                        if (multiFactorValue.ToDouble(&val))
                            {
                            assert(val > 0);
                            totalCount -= (val - 1 /* we will subtract 1 later*/);
                            }
                        }
                    reportStr.erase(index);
                    }
                --uniqueCount;
                --totalCount;
                }
            }
        GetMisspelledWordData()->SetItemValue(i, 2, totalCount);
        GetMisspelledWordData()->SetItemValue(i, 3, uniqueCount);
        GetMisspelledWordData()->SetItemText(
            i, 4, reportStr.c_str(),
            Wisteria::NumberFormatInfo{
                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
            std::numeric_limits<double>::quiet_NaN());
        }
    // remove any blank rows
    for (size_t i = 0; i < GetMisspelledWordData()->GetItemCount(); /* handled in loop*/)
        {
        if (GetMisspelledWordData()->GetItemValue(i, 2) == 0)
            {
            // cppcheck-suppress assertWithSideEffect
            assert(GetMisspelledWordData()->GetItemValue(i, 3) == 0);
            // cppcheck-suppress assertWithSideEffect
            wxASSERT_LEVEL_2_MSG(GetMisspelledWordData()->GetItemText(i, 4).empty(),
                                 GetMisspelledWordData()->GetItemText(i, 4));
            GetMisspelledWordData()->DeleteItem(i);
            }
        else
            {
            ++i;
            }
        }
    auto* view = dynamic_cast<BatchProjectView*>(GetFirstView());
    auto* listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        view->GetGrammarView().FindWindowById(BaseProjectView::MISSPELLED_WORD_LIST_PAGE_ID));
    if (listView != nullptr)
        {
        if (GetMisspelledWordData()->GetItemCount() == 0)
            {
            view->GetGrammarView().RemoveWindowById(BaseProjectView::MISSPELLED_WORD_LIST_PAGE_ID);
            view->UpdateSideBarIcons();
            view->GetSideBar()->SelectFolder(0);
            }
        else
            {
            listView->SetVirtualDataSize(GetMisspelledWordData()->GetItemCount());
            if (listView->GetSortedColumn() == -1)
                {
                listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
                }
            else
                {
                listView->Resort();
                }
            }
        }
    }

//-------------------------------------------------------
bool BatchProjectDoc::OnCreate(const wxString& path, long flags)
    {
    if ((flags & wxDOC_NEW) != 0)
        {
        // if a folder, load the all supported document types recursively
        if (!path.empty() && wxFileName{ path }.IsDir() && wxFileName::DirExists(path))
            {
            wxArrayString files;
                {
                const wxWindowDisabler disableAll;
                const wxBusyInfo wait(wxBusyInfoFlags()
                                          .Text(_(L"Retrieving files..."))
                                          .Parent(wxGetApp().GetParentingWindow()));
#ifdef __WXGTK__
                wxMilliSleep(100);
                wxGetApp().Yield();
#endif
                wxDir::GetAllFiles(path, &files, wxString{}, wxDIR_FILES | wxDIR_DIRS);
                files = FilterFiles(files, ExtractExtensionsFromFileFilter(
                                               ReadabilityAppOptions::GetDocumentFilter()));
                }
            GetSourceFilesInfo().clear();
            GetSourceFilesInfo().reserve(files.size());
            for (const auto& fl : files)
                {
                wxLogMessage(fl);
                GetSourceFilesInfo().emplace_back(fl, wxString{});
                }
            ProjectWizardDlg::SetLastSelectedFolder(path);
            return wxDocument::OnCreate(wxString{}, flags);
            }
        // if passed a single, "regular" file (i.e., not an archive or spreadsheet), then just load
        // it with the defaults and bypass the wizard.
        if (!path.empty() && FilePathResolver(path, false).IsLocalOrNetworkFile() &&
            !FilePathResolver::IsSpreadsheet(path) && !FilePathResolver::IsArchive(path))
            {
            GetSourceFilesInfo().clear();
            GetSourceFilesInfo().emplace_back(path, wxString{});

            const wxArrayString folders = wxFileName(wxFileName(path).GetPathWithSep()).GetDirs();
            ProjectWizardDlg::SetLastSelectedFolder(!folders.empty() ? folders.back() : wxString{});
            return wxDocument::OnCreate(wxString{}, flags);
            }
        // scripting framework passes this in to create an empty project
        // that can have files added later
        if (path == L"EMPTY_PROJECT")
            {
            return wxDocument::OnCreate(wxString{}, flags);
            }
        if (!RunProjectWizard(path))
            {
            return false;
            }
        }
    return wxDocument::OnCreate(path, flags);
    }

//-------------------------------------------------------
bool BatchProjectDoc::OnNewDocument()
    {
    if (!wxDocument::OnNewDocument())
        {
        return false;
        }

    const BaseProjectProcessingLock processingLock(this);

    // load the images now
    SetPlotBackGroundImagePath(GetPlotBackGroundImagePath());
    SetStippleImagePath(GetStippleImagePath());
    SetWatermarkLogoPath(GetWatermarkLogoPath());
    SetGraphCommonImagePath(GetGraphCommonImagePath());

    LoadExcludePhrases();

    // load appended template file (if there is one)
    LoadAppendedDocument();

    wxProgressDialog progressDlg(
        _(L"Creating Project"),
        wxString::Format(
            _(L"Analyzing %s documents..."),
            wxNumberFormatter::ToString(GetSourceFilesInfo().size(), 0,
                                        wxNumberFormatter::Style::Style_WithThousandsSep)),
        static_cast<int>(GetSourceFilesInfo().size() + 13), nullptr,
        wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_ELAPSED_TIME | wxPD_CAN_ABORT | wxPD_APP_MODAL);
    progressDlg.Centre();
    int counter{ 1 };

    InitializeDocuments();

    if (!progressDlg.Update(counter++))
        {
        return false;
        }
    if (!LoadDocuments(progressDlg))
        {
        return false;
        }
    counter = progressDlg.GetValue();

    LoadGroupingLabelsFromDocumentsInfo();

    // prompt user about removing any failed documents.
    // If they request to leave them in, then load any warnings for all documents.
    if (!CheckForFailedDocuments())
        {
        LoadWarningsSection();
        }

    if (!progressDlg.Update(counter++, _(L"Loading Dolch statistics...")))
        {
        return false;
        }
    LoadDolchSection();

    if (!progressDlg.Update(counter++, _(L"Loading difficult words...")))
        {
        return false;
        }
    LoadHardWordsSection();

    if (!progressDlg.Update(counter++, _(L"Loading graphs...")))
        {
        return false;
        }
    // needs to be called before LoadScores to calculate Fry and Raygor
    DisplayReadabilityGraphs();

    if (!progressDlg.Update(counter++, _(L"Loading scores...")))
        {
        return false;
        }
    LoadScoresSection();

    if (!progressDlg.Update(counter++, _(L"Loading summary statistics...")))
        {
        return false;
        }
    LoadSummaryStatsSection();

    if (!progressDlg.Update(counter++))
        {
        return false;
        }
    DisplayScores();

    if (!progressDlg.Update(counter++))
        {
        return false;
        }
    DisplayBoxPlots();

    if (!progressDlg.Update(counter++))
        {
        return false;
        }
    DisplayHistograms();

    if (!progressDlg.Update(counter++, _(L"Loading grammar information...")))
        {
        return false;
        }
    DisplayGrammar();

    if (!progressDlg.Update(counter++))
        {
        return false;
        }
    DisplayHardWords();
    DisplaySentencesBreakdown();
    DisplaySummaryStats();

    if (!progressDlg.Update(counter++))
        {
        return false;
        }
    DisplaySightWords();

    if (!progressDlg.Update(counter++))
        {
        return false;
        }
    DisplayWarnings();

    auto* view = dynamic_cast<BatchProjectView*>(GetFirstView());
    view->UpdateSideBarIcons();
    view->UpdateRibbonState();
    view->Present();
    UpdateAllViews();

    view->GetSideBar()->SelectSubItem(
        view->GetSideBar()->FindSubItem(BaseProjectView::ID_SCORE_LIST_PAGE_ID));

    auto* scoresWindow = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        view->GetScoresView().FindWindowById(BaseProjectView::ID_SCORE_LIST_PAGE_ID));
    if (scoresWindow != nullptr && scoresWindow->GetItemCount() > 0)
        {
        scoresWindow->Select(0);
        }

    // try to base the default name of this project from the folder/web domain of the first file
    if (!GetSourceFilesInfo().empty())
        {
        const FilePathResolver resolvePath(GetOriginalDocumentFilePath(0), false);
        // if a local file, use the name of the last folder in the path as the project name
        if (resolvePath.IsLocalOrNetworkFile())
            {
            const wxArrayString dirs =
                wxFileName::DirName(ProjectWizardDlg::GetLastSelectedFolder()).GetDirs();
            if (!dirs.empty())
                {
                SetTitle(dirs.back());
                SetFilename(dirs.back(), true);
                }
            }
        else if (resolvePath.IsExcelCell() || resolvePath.IsArchivedFile())
            {
            const size_t subDocStart = resolvePath.GetResolvedPath().rfind(L"#");
            if (subDocStart != wxString::npos)
                {
                const wxFileName fn(resolvePath.GetResolvedPath().substr(0, subDocStart));
                SetTitle(fn.GetName());
                SetFilename(fn.GetName(), true);
                }
            }
        else if (resolvePath.IsWebFile())
            {
            const html_utilities::html_url_format hformat(resolvePath.GetResolvedPath().wc_str());
            wxString domain = hformat.get_root_domain().c_str();
            const auto dotPos = domain.rfind(L'.');
            if (dotPos != wxString::npos)
                {
                domain.Truncate(dotPos);
                }
            SetTitle(domain);
            SetFilename(domain, true);
            }
        }

    Modify(true);

    return true;
    }

//------------------------------------------------
void BatchProjectDoc::InitializeDocuments()
    {
    PROFILE();
    for (auto& doc : m_docs)
        {
        wxDELETE(doc);
        }
    // should certainly be the case
    if (!GetSourceFilesInfo().empty())
        {
        m_docs.resize(GetSourceFilesInfo().size(), nullptr);
        }
    for (size_t i = 0; i < GetSourceFilesInfo().size(); ++i)
        {
        m_docs[i] = new BaseProject();
        m_docs[i]->CopySettings(*this);
        m_docs[i]->SetAppendedDocumentText(GetAppendedDocumentText());
        m_docs[i]->ShareExcludePhrases(*this);
        m_docs[i]->SetUIMode(false);
        m_docs[i]->GetSourceFilesInfo().clear();
        m_docs[i]->GetSourceFilesInfo().push_back(GetSourceFilesInfo().at(i));
        }
    }

//------------------------------------------------
void BatchProjectDoc::LoadGroupingLabelsFromDocumentsInfo()
    {
    m_docLabels.clear();
    m_groupStringTable.clear();
    for (auto* const doc : m_docs)
        {
        if (doc->LoadingOriginalTextSucceeded())
            {
            auto [item, inserted] =
                m_docLabels.try_emplace(doc->GetOriginalDocumentDescription().wc_str(), 0);
            // add a unique group ID to the label
            if (inserted)
                {
                item->second = m_docLabels.size() - 1;
                }
            // if too many labels, then this probably isn't grouped data, so don't use grouping
            if (GetDocumentLabels().size() > GetMaxGroupCount())
                {
                m_docLabels.clear();
                break;
                }
            }
        }
    // build a string table for the graphs' datasets
    for (auto& [key, value] : m_docLabels)
        {
        m_groupStringTable.insert(std::make_pair(value, key.c_str()));
        }
    }

//------------------------------------------------
bool BatchProjectDoc::CheckForFailedDocuments()
    {
    wxArrayString failedDocs;
    for (auto& doc : m_docs)
        {
        if (!doc->LoadingOriginalTextSucceeded())
            {
            failedDocs.Add(doc->GetOriginalDocumentFilePath());
            }
        }

    const BatchProjectView* view = dynamic_cast<BatchProjectView*>(GetFirstView());
    wxASSERT_MSG(view->GetFrame(), L"Invalid frame for newly created document!");
    // show the names of the failed documents somehow so the user can review it before removing them
    Wisteria::UI::ListDlg listDlg(
        view->GetFrame(), failedDocs, false, wxGetApp().GetAppOptions()->GetRibbonActiveTabColor(),
        wxGetApp().GetAppOptions()->GetRibbonHoverColor(),
        wxGetApp().GetAppOptions()->GetRibbonActiveFontColor(), Wisteria::UI::LD_YES_NO_BUTTONS,
        wxID_ANY, _(L"Warning"),
        _(L"The following documents could not be loaded because they either do not contain "
          "enough valid text or could not be found. Do you wish to remove these documents "
          "from this project?"));
    if ((failedDocs.GetCount() != 0U) && listDlg.ShowModal() == wxID_YES)
        {
        RemoveFailedDocuments();
        return true;
        }
    // user choose to leave failed documents in the project
    return false;
    }

//------------------------------------------------
void BatchProjectDoc::RemoveFailedDocuments()
    {
    GetSourceFilesInfo().clear();
    GetSourceFilesInfo().reserve(m_docs.size());
    for (auto pos = m_docs.begin(); pos != m_docs.end();
         /* handled in loop*/)
        {
        if ((*pos)->LoadingOriginalTextSucceeded())
            {
            GetSourceFilesInfo().push_back((*pos)->GetSourceFilesInfo().at(0));
            ++pos;
            }
        else
            {
            wxDELETE(*pos);
            pos = m_docs.erase(pos);
            }
        }
    // reload the warnings here because we have thrown out the failed docs and
    // no point in showing their warnings anymore.
    LoadWarningsSection();

    Modify(true);
    }

//------------------------------------------------
void BatchProjectDoc::RefreshStatisticsReports()
    {
    if (!IsSafeToUpdate())
        {
        return;
        }

    // if refresh is not necessary then return
    if (!IsRefreshRequired())
        {
        return;
        }

    auto* view = dynamic_cast<BatchProjectView*>(GetFirstView());
    const wxString currentlySelectedFile = view->GetCurrentlySelectedFileName();
    const auto selectedItem = view->GetSideBar()->GetSelectedFolderId();

    const BaseProjectProcessingLock processingLock(this);
    const wxWindowUpdateLocker noUpdates(GetDocumentWindow());

    LoadSummaryStatsSection();
    DisplaySummaryStats();

    view->UpdateSideBarIcons();
    view->UpdateRibbonState();
    view->Present();
    UpdateAllViews();

    auto selectedIndex = view->GetSideBar()->FindFolder(selectedItem);
    if (!selectedIndex.has_value())
        {
        selectedIndex =
            view->GetSideBar()->FindFolder(BaseProjectView::SIDEBAR_READABILITY_SCORES_SECTION_ID);
        }
    view->GetSideBar()->SelectFolder(selectedIndex, false);

    view->UpdateStatAndTestPanes(currentlySelectedFile);

    Modify(true);
    GetDocumentWindow()->Refresh();
    ResetRefreshRequired();
    }

//------------------------------------------------
void BatchProjectDoc::RefreshGraphs()
    {
    if (!IsSafeToUpdate())
        {
        return;
        }

    // if refresh is not necessary then return
    if (!IsRefreshRequired())
        {
        return;
        }
    const BaseProjectProcessingLock processingLock(this);
    const wxWindowUpdateLocker noUpdates(GetDocumentWindow());

    DisplayReadabilityGraphs();
    DisplayBoxPlots();
    DisplayHistograms();

    Modify(true);
    GetDocumentWindow()->Refresh();
    ResetRefreshRequired();
    }

//------------------------------------------------
void BatchProjectDoc::RefreshProject()
    {
    if (!IsSafeToUpdate())
        {
        return;
        }
    const wxBusyCursor wait;

    // if refresh is not necessary then return
    if (!IsRefreshRequired())
        {
        return;
        }
    const BaseProjectProcessingLock processingLock(this);
    const wxWindowUpdateLocker noUpdates(GetDocumentWindow());

    // reload the excluded phrases
    LoadExcludePhrases();

    // load appended template file (if there is one)
    LoadAppendedDocument();

    auto* view = dynamic_cast<BatchProjectView*>(GetFirstView());
    wxProgressDialog progressDlg(
        wxString::Format(_(L"Reloading \"%s\""), GetTitle()), _(L"Analyzing documents..."),
        IsDocumentReindexingRequired() ? static_cast<int>(GetSourceFilesInfo().size() + 13) : 13,
        view->GetFrame(), wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_ELAPSED_TIME | wxPD_APP_MODAL);
    progressDlg.Centre();
    int counter{ 1 };

    progressDlg.Update(counter++);

    if (IsDocumentReindexingRequired() && GetDocumentStorageMethod() == TextStorage::NoEmbedText)
        {
        InitializeDocuments();
        }

    // reset all the sub documents so that they have the proper settings and tests included
    for (auto& doc : m_docs)
        {
        // CopySettings will clear the embedded text, so back it up and then swap it back in
        std::wstring embeddedText = std::move(doc->GetDocumentText());
        doc->CopySettings(*this);
        doc->SetDocumentText(std::move(embeddedText));
        doc->SetAppendedDocumentText(GetAppendedDocumentText());
        doc->ShareExcludePhrases(*this);
        doc->SetUIMode(false);
        }
    if (IsDocumentReindexingRequired())
        {
        LoadDocuments(progressDlg);
        }
    counter = progressDlg.GetValue();

    LoadGroupingLabelsFromDocumentsInfo();

    // prompt user about removing any failed documents.
    // If they request to leave them in, then load any warnings for all documents.
    if (!CheckForFailedDocuments())
        {
        LoadWarningsSection();
        }

    // get this before list controls are recreated
    const wxString currentlySelectedFile = view->GetCurrentlySelectedFileName();

    progressDlg.Update(counter++, _(L"Loading Dolch statistics..."));
    LoadDolchSection();

    progressDlg.Update(counter++, _(L"Loading difficult words..."));
    LoadHardWordsSection();

    progressDlg.Update(counter++, _(L"Loading graphs..."));
    // needs to be called before LoadScores to calculate Fry and Raygor
    DisplayReadabilityGraphs();

    progressDlg.Update(counter++, _(L"Loading scores..."));
    LoadScoresSection();

    progressDlg.Update(counter++, _(L"Loading summary statistics..."));
    LoadSummaryStatsSection();

    progressDlg.Update(counter++);
    DisplayScores();

    progressDlg.Update(counter++);
    DisplayBoxPlots();

    progressDlg.Update(counter++);
    DisplayHistograms();

    progressDlg.Update(counter++, _(L"Loading grammar information..."));
    DisplayGrammar();

    progressDlg.Update(counter++);
    DisplayHardWords();
    DisplaySentencesBreakdown();
    DisplaySummaryStats();

    progressDlg.Update(counter++);
    DisplaySightWords();

    progressDlg.Update(counter++);
    DisplayWarnings();

    const auto selectedItem = view->GetSideBar()->GetSelectedSubItemId();
    const auto selectedFolder = view->GetSideBar()->GetSelectedFolderId();
    view->UpdateSideBarIcons();
    view->UpdateRibbonState();
    view->Present();
    UpdateAllViews();

    if (!view->GetSideBar()->SelectSubItemById(selectedItem, true, true))
        {
        // fall back to folder that may not have subitems (e.g., the Warning section),
        // and then the score section if the folder isn't there anymore.
        if (!view->GetSideBar()->SelectFolder(selectedFolder.value_or(0), true, true))
            {
            view->GetSideBar()->SelectFolder(0, true, true);
            }
        }
    view->ShowSideBar(view->IsSideBarShown());
    auto* scoresWindow = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        view->GetScoresView().FindWindowById(BaseProjectView::ID_SCORE_LIST_PAGE_ID));
    if (scoresWindow != nullptr && scoresWindow->GetItemCount() > 0)
        {
        scoresWindow->Select(0);
        }

    view->UpdateStatAndTestPanes(currentlySelectedFile);

    Modify(true);

    GetDocumentWindow()->Refresh();

    ResetRefreshRequired();
    }

//------------------------------------------------------------
void BatchProjectDoc::LoadDolchSection()
    {
    PROFILE();
    m_dolchCompletionData->DeleteAllItems();
    m_dolchCompletionData->SetSize(m_docs.size(), 9);
    m_dolchWordsBatchData->DeleteAllItems();
    m_dolchWordsBatchData->SetSize(
        m_docs.size(), (GetStatisticsReportInfo().IsExtendedInformationEnabled()) ? 16 : 9);
    m_NonDolchWordsData->DeleteAllItems();
    m_NonDolchWordsData->SetSize(
        m_docs.size(), (GetStatisticsReportInfo().IsExtendedInformationEnabled()) ? 4 : 3);

    size_t dolchDocumentCount = 0;

    for (auto& doc : m_docs)
        {
        // dolch words
        if (doc->LoadingOriginalTextSucceeded())
            {
            // completion stats
            m_dolchCompletionData->SetItemText(
                dolchDocumentCount, 0, doc->GetOriginalDocumentFilePath(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_dolchCompletionData->SetItemText(
                dolchDocumentCount, 1, doc->GetOriginalDocumentDescription(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            const double dolchConjunctionPercentage =
                safe_divide<double>((ProjectReportFormat::MAX_DOLCH_CONJUNCTION_WORDS -
                                     doc->GetUnusedDolchConjunctions()),
                                    ProjectReportFormat::MAX_DOLCH_CONJUNCTION_WORDS) *
                100;
            const double dolchPrepositionsPercentage =
                safe_divide<double>((ProjectReportFormat::MAX_DOLCH_PREPOSITION_WORDS -
                                     doc->GetUnusedDolchPrepositions()),
                                    ProjectReportFormat::MAX_DOLCH_PREPOSITION_WORDS) *
                100;
            const double dolchPronounsPercentage =
                safe_divide<double>(
                    (ProjectReportFormat::MAX_DOLCH_PRONOUN_WORDS - doc->GetUnusedDolchPronouns()),
                    ProjectReportFormat::MAX_DOLCH_PRONOUN_WORDS) *
                100;
            const double dolchAdverbsPercentage =
                safe_divide<double>(
                    (ProjectReportFormat::MAX_DOLCH_ADVERB_WORDS - doc->GetUnusedDolchAdverbs()),
                    ProjectReportFormat::MAX_DOLCH_ADVERB_WORDS) *
                100;
            const double dolchAdjectivesPercentage =
                safe_divide<double>((ProjectReportFormat::MAX_DOLCH_ADJECTIVE_WORDS -
                                     doc->GetUnusedDolchAdjectives()),
                                    ProjectReportFormat::MAX_DOLCH_ADJECTIVE_WORDS) *
                100;
            const double dolchVerbsPercentage =
                safe_divide<double>(
                    (ProjectReportFormat::MAX_DOLCH_VERBS - doc->GetUnusedDolchVerbs()),
                    ProjectReportFormat::MAX_DOLCH_VERBS) *
                100;
            const double dolchNounPercentage =
                safe_divide<double>(
                    (ProjectReportFormat::MAX_DOLCH_NOUNS - doc->GetUnusedDolchNouns()),
                    ProjectReportFormat::MAX_DOLCH_NOUNS) *
                100;
            m_dolchCompletionData->SetItemValue(
                dolchDocumentCount, 2, dolchConjunctionPercentage,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
            m_dolchCompletionData->SetItemValue(
                dolchDocumentCount, 3, dolchPrepositionsPercentage,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
            m_dolchCompletionData->SetItemValue(
                dolchDocumentCount, 4, dolchPronounsPercentage,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
            m_dolchCompletionData->SetItemValue(
                dolchDocumentCount, 5, dolchAdverbsPercentage,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
            m_dolchCompletionData->SetItemValue(
                dolchDocumentCount, 6, dolchAdjectivesPercentage,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
            m_dolchCompletionData->SetItemValue(
                dolchDocumentCount, 7, dolchVerbsPercentage,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
            m_dolchCompletionData->SetItemValue(
                dolchDocumentCount, 8, dolchNounPercentage,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
            // word stats
            size_t columnCount = 0;
            m_dolchWordsBatchData->SetItemText(
                dolchDocumentCount, columnCount++, doc->GetOriginalDocumentFilePath(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_dolchWordsBatchData->SetItemText(
                dolchDocumentCount, columnCount++, doc->GetOriginalDocumentDescription(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_dolchWordsBatchData->SetItemValue(
                dolchDocumentCount, columnCount++,
                safe_divide<double>(doc->GetDolchConjunctionCounts().second, doc->GetTotalWords()) *
                    100,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
            if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
                {
                m_dolchWordsBatchData->SetItemValue(
                    dolchDocumentCount, columnCount++, doc->GetDolchConjunctionCounts().second,
                    Wisteria::NumberFormatInfo(
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
                }

            m_dolchWordsBatchData->SetItemValue(
                dolchDocumentCount, columnCount++,
                safe_divide<double>(doc->GetDolchPrepositionWordCounts().second,
                                    doc->GetTotalWords()) *
                    100,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
            if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
                {
                m_dolchWordsBatchData->SetItemValue(
                    dolchDocumentCount, columnCount++, doc->GetDolchPrepositionWordCounts().second,
                    Wisteria::NumberFormatInfo(
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
                }

            m_dolchWordsBatchData->SetItemValue(
                dolchDocumentCount, columnCount++,
                safe_divide<double>(doc->GetDolchPronounCounts().second, doc->GetTotalWords()) *
                    100,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
            if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
                {
                m_dolchWordsBatchData->SetItemValue(
                    dolchDocumentCount, columnCount++, doc->GetDolchPronounCounts().second,
                    Wisteria::NumberFormatInfo(
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
                }

            m_dolchWordsBatchData->SetItemValue(
                dolchDocumentCount, columnCount++,
                safe_divide<double>(doc->GetDolchAdverbCounts().second, doc->GetTotalWords()) * 100,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
            if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
                {
                m_dolchWordsBatchData->SetItemValue(
                    dolchDocumentCount, columnCount++, doc->GetDolchAdverbCounts().second,
                    Wisteria::NumberFormatInfo(
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
                }

            m_dolchWordsBatchData->SetItemValue(
                dolchDocumentCount, columnCount++,
                safe_divide<double>(doc->GetDolchAdjectiveCounts().second, doc->GetTotalWords()) *
                    100,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
            if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
                {
                m_dolchWordsBatchData->SetItemValue(
                    dolchDocumentCount, columnCount++, doc->GetDolchAdjectiveCounts().second,
                    Wisteria::NumberFormatInfo(
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
                }

            m_dolchWordsBatchData->SetItemValue(
                dolchDocumentCount, columnCount++,
                safe_divide<double>(doc->GetDolchVerbsCounts().second, doc->GetTotalWords()) * 100,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
            if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
                {
                m_dolchWordsBatchData->SetItemValue(
                    dolchDocumentCount, columnCount++, doc->GetDolchVerbsCounts().second,
                    Wisteria::NumberFormatInfo(
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
                }

            m_dolchWordsBatchData->SetItemValue(
                dolchDocumentCount, columnCount++,
                safe_divide<double>(doc->GetDolchNounCounts().second, doc->GetTotalWords()) * 100,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
            if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
                {
                m_dolchWordsBatchData->SetItemValue(
                    dolchDocumentCount, columnCount++, doc->GetDolchNounCounts().second,
                    Wisteria::NumberFormatInfo(
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
                }

            // non-word stats
            m_NonDolchWordsData->SetItemText(
                dolchDocumentCount, 0, doc->GetOriginalDocumentFilePath(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_NonDolchWordsData->SetItemText(
                dolchDocumentCount, 1, doc->GetOriginalDocumentDescription(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            const size_t totalDolchWords =
                doc->GetDolchConjunctionCounts().second +
                doc->GetDolchPrepositionWordCounts().second + doc->GetDolchPronounCounts().second +
                doc->GetDolchAdverbCounts().second + doc->GetDolchAdjectiveCounts().second +
                doc->GetDolchVerbsCounts().second + doc->GetDolchNounCounts().second;
            const double totalDolchPercentage =
                safe_divide<double>(totalDolchWords, doc->GetTotalWords()) * 100;
            m_NonDolchWordsData->SetItemValue(
                dolchDocumentCount, 2, 100 - totalDolchPercentage,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
            if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
                {
                m_NonDolchWordsData->SetItemValue(
                    dolchDocumentCount, 3, doc->GetTotalWords() - totalDolchWords,
                    Wisteria::NumberFormatInfo(
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
                }

            ++dolchDocumentCount;
            }
        }

    m_dolchCompletionData->SetSize(dolchDocumentCount);
    m_dolchWordsBatchData->SetSize(dolchDocumentCount);
    m_NonDolchWordsData->SetSize(dolchDocumentCount);
    }

//------------------------------------------------------------
void BatchProjectDoc::LoadHardWordsSection()
    {
    PROFILE();
    m_hardWordsData->DeleteAllItems();
    size_t extraColumnCount = 0;
    if (GetStatisticsReportInfo().IsExtendedWordsEnabled())
        {
        extraColumnCount += IsSmogLikeTestIncluded() ? 2 : 0;
        extraColumnCount +=
            GetReadabilityTests().is_test_included(ReadabilityMessages::GUNNING_FOG()) ? 2 : 0;
        extraColumnCount += IsDaleChallLikeTestIncluded() ? 2 : 0;
        extraColumnCount +=
            GetReadabilityTests().is_test_included(ReadabilityMessages::SPACHE()) ? 2 : 0;
        extraColumnCount +=
            GetReadabilityTests().is_test_included(ReadabilityMessages::HARRIS_JACOBSON()) ? 2 : 0;
        extraColumnCount += GetCustTestsInUse().size() * 2;
        }
    // doc name, description, overall words, complex words and %, long words and % = 7
    m_hardWordsData->SetSize(m_docs.size(), 7 + extraColumnCount);

    size_t hardWordRowCount = 0;

    for (const auto& doc : m_docs)
        {
        // hard word statistics (note the ordering here must match the column ordering in
        // DisplayHardWords())
        if (doc->LoadingOriginalTextSucceeded())
            {
            size_t columnIndex = 0;
            m_hardWordsData->SetItemText(
                hardWordRowCount, columnIndex++, doc->GetOriginalDocumentFilePath(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_hardWordsData->SetItemText(
                hardWordRowCount, columnIndex++, doc->GetOriginalDocumentDescription(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            // total overall words
            m_hardWordsData->SetItemValue(
                hardWordRowCount, columnIndex++, doc->GetTotalWords(),
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
            // complex words
            m_hardWordsData->SetItemValue(
                hardWordRowCount, columnIndex++,
                safe_divide<double>(doc->GetTotal3PlusSyllabicWords(), doc->GetTotalWords()) * 100,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
            m_hardWordsData->SetItemValue(
                hardWordRowCount, columnIndex++, doc->GetTotal3PlusSyllabicWords(),
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
            // long words
            m_hardWordsData->SetItemValue(
                hardWordRowCount, columnIndex++,
                safe_divide<double>(doc->GetTotalLongWords(), doc->GetTotalWords()) * 100,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::PercentageFormatting, 1, true));
            m_hardWordsData->SetItemValue(
                hardWordRowCount, columnIndex++, doc->GetTotalLongWords(),
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
            if (GetStatisticsReportInfo().IsExtendedWordsEnabled())
                {
                // hard SMOG words (numerals fully syllabized)
                if (IsSmogLikeTestIncluded())
                    {
                    m_hardWordsData->SetItemValue(
                        hardWordRowCount, columnIndex++,
                        safe_divide<double>(
                            doc->GetTotal3PlusSyllabicWordsNumeralsFullySyllabized(),
                            doc->GetTotalWords()) *
                            100,
                        Wisteria::NumberFormatInfo(
                            Wisteria::NumberFormatInfo::NumberFormatType::PercentageFormatting, 1,
                            true));
                    m_hardWordsData->SetItemValue(
                        hardWordRowCount, columnIndex++,
                        doc->GetTotal3PlusSyllabicWordsNumeralsFullySyllabized(),
                        Wisteria::NumberFormatInfo(
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 0,
                            true));
                    }
                // hard FOG words
                if (GetReadabilityTests().is_test_included(ReadabilityMessages::GUNNING_FOG()))
                    {
                    m_hardWordsData->SetItemValue(
                        hardWordRowCount, columnIndex++,
                        safe_divide<double>(doc->GetTotalHardWordsFog(), doc->GetTotalWords()) *
                            100,
                        Wisteria::NumberFormatInfo(
                            Wisteria::NumberFormatInfo::NumberFormatType::PercentageFormatting, 1,
                            true));
                    m_hardWordsData->SetItemValue(
                        hardWordRowCount, columnIndex++, doc->GetTotalHardWordsFog(),
                        Wisteria::NumberFormatInfo(
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 0,
                            true));
                    }
                // hard DC words
                const size_t totalWordCountForDC =
                    (GetDaleChallTextExclusionMode() ==
                     SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                        doc->GetTotalWordsFromCompleteSentencesAndHeaders() :
                        doc->GetTotalWords();
                const size_t totalWordCountForHJ =
                    (GetHarrisJacobsonTextExclusionMode() ==
                     SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                        doc->GetTotalWordsFromCompleteSentencesAndHeaders() :
                        doc->GetTotalWords();
                const size_t totalNumeralCountForHJ =
                    (GetHarrisJacobsonTextExclusionMode() ==
                     SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                        doc->GetTotalNumeralsFromCompleteSentencesAndHeaders() :
                        doc->GetTotalNumerals();
                // hard DC words
                if (IsDaleChallLikeTestIncluded())
                    {
                    m_hardWordsData->SetItemValue(
                        hardWordRowCount, columnIndex++,
                        safe_divide<double>(doc->GetTotalHardWordsDaleChall(),
                                            totalWordCountForDC) *
                            100,
                        Wisteria::NumberFormatInfo(
                            Wisteria::NumberFormatInfo::NumberFormatType::PercentageFormatting, 1,
                            true));
                    m_hardWordsData->SetItemValue(
                        hardWordRowCount, columnIndex++, doc->GetTotalHardWordsDaleChall(),
                        Wisteria::NumberFormatInfo(
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 0,
                            true));
                    }
                // hard spache words
                if (GetReadabilityTests().is_test_included(ReadabilityMessages::SPACHE()))
                    {
                    m_hardWordsData->SetItemValue(
                        hardWordRowCount, columnIndex++,
                        safe_divide<double>(doc->GetTotalHardWordsSpache(), doc->GetTotalWords()) *
                            100,
                        Wisteria::NumberFormatInfo(
                            Wisteria::NumberFormatInfo::NumberFormatType::PercentageFormatting, 1,
                            true));
                    m_hardWordsData->SetItemValue(
                        hardWordRowCount, columnIndex++, doc->GetTotalHardWordsSpache(),
                        Wisteria::NumberFormatInfo(
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 0,
                            true));
                    }
                // hard HJ words
                if (GetReadabilityTests().is_test_included(ReadabilityMessages::HARRIS_JACOBSON()))
                    {
                    m_hardWordsData->SetItemValue(
                        hardWordRowCount, columnIndex++,
                        safe_divide<double>(doc->GetTotalHardWordsHarrisJacobson(),
                                            totalWordCountForHJ - totalNumeralCountForHJ) *
                            100,
                        Wisteria::NumberFormatInfo(
                            Wisteria::NumberFormatInfo::NumberFormatType::PercentageFormatting, 1,
                            true));
                    m_hardWordsData->SetItemValue(
                        hardWordRowCount, columnIndex++, doc->GetTotalHardWordsHarrisJacobson(),
                        Wisteria::NumberFormatInfo(
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 0,
                            true));
                    }
                for (auto custTestPos = doc->GetCustTestsInUse().begin();
                     custTestPos != doc->GetCustTestsInUse().end(); ++custTestPos)
                    {
                    if (!custTestPos->GetIterator()->is_using_familiar_words())
                        {
                        continue;
                        }

                    // special logic for calculating word percentage if test is based on DC or HJ
                    const size_t totalWordCountForCustomTest =
                        custTestPos->IsDaleChallFormula() ?
                            totalWordCountForDC :
                        custTestPos->IsHarrisJacobsonFormula() ?
                            totalWordCountForHJ - totalNumeralCountForHJ :
                            doc->GetTotalWords();
                    m_hardWordsData->SetItemValue(
                        hardWordRowCount, columnIndex++,
                        safe_divide<double>(custTestPos->GetUnfamiliarWordCount(),
                                            totalWordCountForCustomTest) *
                            100,
                        Wisteria::NumberFormatInfo(
                            Wisteria::NumberFormatInfo::NumberFormatType::PercentageFormatting, 1,
                            true));
                    m_hardWordsData->SetItemValue(
                        hardWordRowCount, columnIndex++, custTestPos->GetUnfamiliarWordCount(),
                        Wisteria::NumberFormatInfo(
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 0,
                            true));
                    }
                }
            ++hardWordRowCount;
            }
        }

    m_hardWordsData->SetSize(hardWordRowCount);
    }

//------------------------------------------------------------
void BatchProjectDoc::LoadSummaryStatsSection()
    {
    m_summaryStatsData->DeleteAllItems();

    m_summaryStatsColumnNames.clear();
    m_summaryStatsColumnNames = { _(L"Document"), _(L"Label") };
    if (GetStatisticsReportInfo().IsParagraphEnabled())
        {
        m_summaryStatsColumnNames.push_back(_(L"Number of paragraphs"));
        }
    if (GetStatisticsReportInfo().IsSentencesEnabled())
        {
        m_summaryStatsColumnNames.push_back(_(L"Number of sentences"));
        m_summaryStatsColumnNames.push_back(_(L"Number of units/independent clauses"));
        m_summaryStatsColumnNames.push_back(_(L"Number of difficult sentences"));
        m_summaryStatsColumnNames.push_back(_(L"Longest sentence"));
        m_summaryStatsColumnNames.push_back(_(L"Average sentence length"));
        m_summaryStatsColumnNames.push_back(_(L"Number of interrogative sentences (questions)"));
        m_summaryStatsColumnNames.push_back(_(L"Number of exclamatory sentences"));
        }
    if (GetStatisticsReportInfo().IsWordsEnabled())
        {
        m_summaryStatsColumnNames.push_back(_(L"Number of words"));
        m_summaryStatsColumnNames.push_back(_(L"Number of unique words"));
        m_summaryStatsColumnNames.push_back(_(L"Number of syllables"));
        m_summaryStatsColumnNames.push_back(_(L"Number of characters (punctuation excluded)"));
        m_summaryStatsColumnNames.push_back(_(L"Number of characters + punctuation"));
        m_summaryStatsColumnNames.push_back(_(L"Average number of characters"));
        m_summaryStatsColumnNames.push_back(_(L"Average number of syllables"));
        }
    if (GetStatisticsReportInfo().IsExtendedWordsEnabled())
        {
        m_summaryStatsColumnNames.push_back(_(L"Number of numerals"));
        m_summaryStatsColumnNames.push_back(_(L"Number of proper nouns"));
        m_summaryStatsColumnNames.push_back(_(L"Number of monosyllabic words"));
        m_summaryStatsColumnNames.push_back(_(L"Number of unique monosyllabic words"));
        m_summaryStatsColumnNames.push_back(_(L"Number of complex (3+ syllable) words"));
        m_summaryStatsColumnNames.push_back(_(L"Number of unique 3+ syllable words"));
        m_summaryStatsColumnNames.push_back(_(L"Number of long (6+ characters) words"));
        m_summaryStatsColumnNames.push_back(_(L"Number of unique long words"));
        // TRANSLATORS: "hard words" as in difficult, not as in "harsh."
        m_summaryStatsColumnNames.push_back(_(L"Number of SMOG hard words"));
        m_summaryStatsColumnNames.push_back(_(L"Number of unique SMOG hard words"));
        // TRANSLATORS: "Fog" is a test name; don't translate it.
        m_summaryStatsColumnNames.push_back(_(L"Number of Fog hard words"));
        m_summaryStatsColumnNames.push_back(_(L"Number of unique Fog hard words"));
        if (IsIncludingDolchSightWords())
            {
            if (GetStatisticsReportInfo().IsDolchCoverageEnabled())
                {
                m_summaryStatsColumnNames.push_back(_(L"Number of conjunctions used"));
                m_summaryStatsColumnNames.push_back(_(L"Number of prepositions used"));
                m_summaryStatsColumnNames.push_back(_(L"Number of pronouns used"));
                m_summaryStatsColumnNames.push_back(_(L"Number of adverbs used"));
                m_summaryStatsColumnNames.push_back(_(L"Number of adjectives used"));
                m_summaryStatsColumnNames.push_back(_(L"Number of verbs used"));
                m_summaryStatsColumnNames.push_back(_(L"Number of nouns used"));
                }
            if (GetStatisticsReportInfo().IsDolchWordsEnabled())
                {
                m_summaryStatsColumnNames.push_back(_(L"Number of Dolch words"));
                m_summaryStatsColumnNames.push_back(_(L"Number of Dolch words (excluding nouns)"));
                m_summaryStatsColumnNames.push_back(_(L"Number of non-Dolch words"));
                m_summaryStatsColumnNames.push_back(_(L"Number of Dolch conjunctions"));
                m_summaryStatsColumnNames.push_back(_(L"Number of unique Dolch conjunctions"));
                m_summaryStatsColumnNames.push_back(_(L"Number of Dolch prepositions"));
                m_summaryStatsColumnNames.push_back(_(L"Number of unique Dolch prepositions"));
                m_summaryStatsColumnNames.push_back(_(L"Number of Dolch pronouns"));
                m_summaryStatsColumnNames.push_back(_(L"Number of unique Dolch pronouns"));
                m_summaryStatsColumnNames.push_back(_(L"Number of Dolch adverbs"));
                m_summaryStatsColumnNames.push_back(_(L"Number of unique Dolch adverbs"));
                m_summaryStatsColumnNames.push_back(_(L"Number of Dolch adjectives"));
                m_summaryStatsColumnNames.push_back(_(L"Number of unique Dolch adjectives"));
                m_summaryStatsColumnNames.push_back(_(L"Number of Dolch verbs"));
                m_summaryStatsColumnNames.push_back(_(L"Number of unique Dolch verbs"));
                m_summaryStatsColumnNames.push_back(_(L"Number of Dolch nouns"));
                m_summaryStatsColumnNames.push_back(_(L"Number of unique Dolch nouns"));
                }
            }
        if (IsDaleChallLikeTestIncluded())
            {
            m_summaryStatsColumnNames.push_back(_(L"Number of Dale-Chall unfamiliar words"));
            m_summaryStatsColumnNames.push_back(_(L"Number of unique Dale-Chall unfamiliar words"));
            }
        if (GetReadabilityTests().is_test_included(ReadabilityMessages::HARRIS_JACOBSON()))
            {
            m_summaryStatsColumnNames.push_back(_(L"Number of Harris-Jacobson unfamiliar words"));
            m_summaryStatsColumnNames.push_back(
                _(L"Number of unique Harris-Jacobson unfamiliar words"));
            }
        if (GetReadabilityTests().is_test_included(ReadabilityMessages::SPACHE()))
            {
            m_summaryStatsColumnNames.push_back(_(L"Number of Spache unfamiliar words"));
            m_summaryStatsColumnNames.push_back(_(L"Number of unique Spache unfamiliar words"));
            }
        if (GetReadabilityTests().is_test_included(ReadabilityMessages::EFLAW()))
            {
            m_summaryStatsColumnNames.push_back(_(L"Number of McAlpine EFLAW miniwords"));
            m_summaryStatsColumnNames.push_back(
                _(L"Number of unique McAlpine EFLAW miniwords words"));
            }
        for (const auto& cTests : GetCustTestsInUse())
            {
            m_summaryStatsColumnNames.push_back(wxString::Format(
                _(L"Number of %s unfamiliar words"), cTests.GetIterator()->get_name().c_str()));
            m_summaryStatsColumnNames.push_back(
                wxString::Format(_(L"Number of unique %s unfamiliar words"),
                                 cTests.GetIterator()->get_name().c_str()));
            }
        }
    if (GetStatisticsReportInfo().IsGrammarEnabled() && GetGrammarInfo().IsAnyFeatureEnabled())
        {
        m_summaryStatsColumnNames.push_back(_(L"Number of possible misspellings"));
        m_summaryStatsColumnNames.push_back(_(L"Number of repeated words"));
        m_summaryStatsColumnNames.push_back(_(L"Number of article mismatches"));
        m_summaryStatsColumnNames.push_back(_(L"Number of errors & misspellings"));
        m_summaryStatsColumnNames.push_back(_(L"Number of redundant phrases"));
        m_summaryStatsColumnNames.push_back(_(L"Number of overused words (x sentence)"));
        m_summaryStatsColumnNames.push_back(_(L"Number of wordy items"));
        m_summaryStatsColumnNames.push_back(_(L"Number of clich\u00E9s"));
        m_summaryStatsColumnNames.push_back(_(L"Number of passive voices"));
        m_summaryStatsColumnNames.push_back(_(L"Number of sentences that begin with conjunctions"));
        m_summaryStatsColumnNames.push_back(
            _(L"Number of Sentences that begin with lowercased words"));
        }
    if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
        {
        m_summaryStatsColumnNames.push_back(_(L"Text size (Kbs.)"));
        }
    m_summaryStatsData->SetSize(m_docs.size(), m_summaryStatsColumnNames.size());

    // quneiform-suppress-begin
    size_t rowCount{ 0 };
    for (const auto& doc : m_docs)
        {
        size_t columnCount{ 0 };
        m_summaryStatsData->SetItemText(
            rowCount, columnCount++, doc->GetOriginalDocumentFilePath(),
            Wisteria::NumberFormatInfo{
                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
            std::numeric_limits<double>::quiet_NaN());
        m_summaryStatsData->SetItemText(
            rowCount, columnCount++, doc->GetOriginalDocumentDescription(),
            Wisteria::NumberFormatInfo{
                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
            std::numeric_limits<double>::quiet_NaN());
        if (GetStatisticsReportInfo().IsParagraphEnabled())
            {
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of paragraphs"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalParagraphs());
            }
        if (GetStatisticsReportInfo().IsSentencesEnabled())
            {
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of sentences"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalSentences());
            assert(m_summaryStatsColumnNames[columnCount] ==
                   _(L"Number of units/independent clauses"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalSentenceUnits());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of difficult sentences"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                             doc->GetTotalOverlyLongSentences());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Longest sentence"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetLongestSentence());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Average sentence length"));
            m_summaryStatsData->SetItemValue(
                rowCount, columnCount++,
                safe_divide<double>(doc->GetTotalWords(), doc->GetTotalSentences()),
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 1, false));
            assert(m_summaryStatsColumnNames[columnCount] ==
                   _(L"Number of interrogative sentences (questions)"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                             doc->GetTotalInterrogativeSentences());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of exclamatory sentences"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                             doc->GetTotalExclamatorySentences());
            }
        if (GetStatisticsReportInfo().IsWordsEnabled())
            {
            const auto averageCharacterCount =
                safe_divide<double>(doc->GetTotalCharacters(), doc->GetTotalWords());
            const auto averageSyllableCount =
                safe_divide<double>(doc->GetTotalSyllables(), doc->GetTotalWords());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of words"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalWords());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of unique words"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalUniqueWords());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of syllables"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalSyllables());
            assert(m_summaryStatsColumnNames[columnCount] ==
                   _(L"Number of characters (punctuation excluded)"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalCharacters());
            assert(m_summaryStatsColumnNames[columnCount] ==
                   _(L"Number of characters + punctuation"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                             doc->GetTotalCharactersPlusPunctuation());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Average number of characters"));
            m_summaryStatsData->SetItemValue(
                rowCount, columnCount++, averageCharacterCount,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 1, false));
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Average number of syllables"));
            m_summaryStatsData->SetItemValue(
                rowCount, columnCount++, averageSyllableCount,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 1, false));
            }
        if (GetStatisticsReportInfo().IsExtendedWordsEnabled())
            {
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of numerals"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalNumerals());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of proper nouns"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalProperNouns());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of monosyllabic words"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                             doc->GetTotalMonoSyllabicWords());
            assert(m_summaryStatsColumnNames[columnCount] ==
                   _(L"Number of unique monosyllabic words"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                             doc->GetTotalUniqueMonoSyllabicWords());
            assert(m_summaryStatsColumnNames[columnCount] ==
                   _(L"Number of complex (3+ syllable) words"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                             doc->GetTotal3PlusSyllabicWords());
            assert(m_summaryStatsColumnNames[columnCount] ==
                   _(L"Number of unique 3+ syllable words"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                             doc->GetTotalUnique3PlusSyllableWords());
            assert(m_summaryStatsColumnNames[columnCount] ==
                   _(L"Number of long (6+ characters) words"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalLongWords());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of unique long words"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                             doc->GetTotalUnique6CharsPlusWords());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of SMOG hard words"));
            m_summaryStatsData->SetItemValue(
                rowCount, columnCount++, doc->GetTotal3PlusSyllabicWordsNumeralsFullySyllabized());
            assert(m_summaryStatsColumnNames[columnCount] ==
                   _(L"Number of unique SMOG hard words"));
            m_summaryStatsData->SetItemValue(
                rowCount, columnCount++, doc->GetUnique3PlusSyllabicWordsNumeralsFullySyllabized());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of Fog hard words"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalHardWordsFog());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of unique Fog hard words"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                             doc->GetTotalUniqueHardFogWords());
            if (IsIncludingDolchSightWords())
                {
                if (GetStatisticsReportInfo().IsDolchCoverageEnabled())
                    {
                    assert(m_summaryStatsColumnNames[columnCount] ==
                           _(L"Number of conjunctions used"));
                    m_summaryStatsData->SetItemValue(
                        rowCount, columnCount++,
                        ProjectReportFormat::MAX_DOLCH_CONJUNCTION_WORDS -
                            doc->GetUnusedDolchConjunctions());
                    assert(m_summaryStatsColumnNames[columnCount] ==
                           _(L"Number of prepositions used"));
                    m_summaryStatsData->SetItemValue(
                        rowCount, columnCount++,
                        ProjectReportFormat::MAX_DOLCH_PREPOSITION_WORDS -
                            doc->GetUnusedDolchPrepositions());
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of pronouns used"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                     ProjectReportFormat::MAX_DOLCH_PRONOUN_WORDS -
                                                         doc->GetUnusedDolchPronouns());
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of adverbs used"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                     ProjectReportFormat::MAX_DOLCH_ADVERB_WORDS -
                                                         doc->GetUnusedDolchAdverbs());
                    assert(m_summaryStatsColumnNames[columnCount] ==
                           _(L"Number of adjectives used"));
                    m_summaryStatsData->SetItemValue(
                        rowCount, columnCount++,
                        ProjectReportFormat::MAX_DOLCH_ADJECTIVE_WORDS -
                            doc->GetUnusedDolchAdjectives());
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of verbs used"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                     ProjectReportFormat::MAX_DOLCH_VERBS -
                                                         doc->GetUnusedDolchVerbs());
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of nouns used"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                     ProjectReportFormat::MAX_DOLCH_NOUNS -
                                                         doc->GetUnusedDolchNouns());
                    }
                if (GetStatisticsReportInfo().IsDolchWordsEnabled())
                    {
                    const size_t totalDolchWords =
                        doc->GetDolchConjunctionCounts().second +
                        doc->GetDolchPrepositionWordCounts().second +
                        doc->GetDolchPronounCounts().second + doc->GetDolchAdverbCounts().second +
                        doc->GetDolchAdjectiveCounts().second + doc->GetDolchVerbsCounts().second +
                        doc->GetDolchNounCounts().second;
                    const size_t totalDolchWordsExcludingNouns =
                        doc->GetDolchConjunctionCounts().second +
                        doc->GetDolchPrepositionWordCounts().second +
                        doc->GetDolchPronounCounts().second + doc->GetDolchAdverbCounts().second +
                        doc->GetDolchAdjectiveCounts().second + doc->GetDolchVerbsCounts().second;
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of Dolch words"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++, totalDolchWords);
                    assert(m_summaryStatsColumnNames[columnCount] ==
                           _(L"Number of Dolch words (excluding nouns)"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                     totalDolchWordsExcludingNouns);
                    assert(m_summaryStatsColumnNames[columnCount] ==
                           _(L"Number of non-Dolch words"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                     doc->GetTotalWords() - totalDolchWords);
                    assert(m_summaryStatsColumnNames[columnCount] ==
                           _(L"Number of Dolch conjunctions"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                     doc->GetDolchConjunctionCounts().second);
                    assert(m_summaryStatsColumnNames[columnCount] ==
                           _(L"Number of unique Dolch conjunctions"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                     doc->GetDolchConjunctionCounts().first);
                    assert(m_summaryStatsColumnNames[columnCount] ==
                           _(L"Number of Dolch prepositions"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                     doc->GetDolchPrepositionWordCounts().second);
                    assert(m_summaryStatsColumnNames[columnCount] ==
                           _(L"Number of unique Dolch prepositions"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                     doc->GetDolchPrepositionWordCounts().first);
                    assert(m_summaryStatsColumnNames[columnCount] ==
                           _(L"Number of Dolch pronouns"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                     doc->GetDolchPronounCounts().second);
                    assert(m_summaryStatsColumnNames[columnCount] ==
                           _(L"Number of unique Dolch pronouns"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                     doc->GetDolchPronounCounts().first);
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of Dolch adverbs"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                     doc->GetDolchAdverbCounts().second);
                    assert(m_summaryStatsColumnNames[columnCount] ==
                           _(L"Number of unique Dolch adverbs"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                     doc->GetDolchAdverbCounts().first);
                    assert(m_summaryStatsColumnNames[columnCount] ==
                           _(L"Number of Dolch adjectives"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                     doc->GetDolchAdjectiveCounts().second);
                    assert(m_summaryStatsColumnNames[columnCount] ==
                           _(L"Number of unique Dolch adjectives"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                     doc->GetDolchAdjectiveCounts().first);
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of Dolch verbs"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                     doc->GetDolchVerbsCounts().second);
                    assert(m_summaryStatsColumnNames[columnCount] ==
                           _(L"Number of unique Dolch verbs"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                     doc->GetDolchVerbsCounts().first);
                    assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of Dolch nouns"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                     doc->GetDolchNounCounts().second);
                    assert(m_summaryStatsColumnNames[columnCount] ==
                           _(L"Number of unique Dolch nouns"));
                    m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                     doc->GetDolchNounCounts().first);
                    }
                }
            if (IsDaleChallLikeTestIncluded())
                {
                assert(m_summaryStatsColumnNames[columnCount] ==
                       _(L"Number of Dale-Chall unfamiliar words"));
                m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                 doc->GetTotalHardWordsDaleChall());
                assert(m_summaryStatsColumnNames[columnCount] ==
                       _(L"Number of unique Dale-Chall unfamiliar words"));
                m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                 doc->GetTotalUniqueDCHardWords());
                }
            if (GetReadabilityTests().is_test_included(ReadabilityMessages::HARRIS_JACOBSON()))
                {
                assert(m_summaryStatsColumnNames[columnCount] ==
                       _(L"Number of Harris-Jacobson unfamiliar words"));
                m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                 doc->GetTotalHardWordsHarrisJacobson());
                assert(m_summaryStatsColumnNames[columnCount] ==
                       _(L"Number of unique Harris-Jacobson unfamiliar words"));
                m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                 doc->GetTotalUniqueHarrisJacobsonHardWords());
                }
            if (GetReadabilityTests().is_test_included(ReadabilityMessages::SPACHE()))
                {
                assert(m_summaryStatsColumnNames[columnCount] ==
                       _(L"Number of Spache unfamiliar words"));
                m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                 doc->GetTotalHardWordsSpache());
                assert(m_summaryStatsColumnNames[columnCount] ==
                       _(L"Number of unique Spache unfamiliar words"));
                m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                 doc->GetTotalUniqueHardWordsSpache());
                }
            if (GetReadabilityTests().is_test_included(ReadabilityMessages::EFLAW()))
                {
                assert(m_summaryStatsColumnNames[columnCount] ==
                       _(L"Number of McAlpine EFLAW miniwords"));
                m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetTotalMiniWords());
                assert(m_summaryStatsColumnNames[columnCount] ==
                       _(L"Number of unique McAlpine EFLAW miniwords words"));
                m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                 doc->GetTotalUniqueMiniWords());
                }
            for (const auto& cTest : doc->GetCustTestsInUse())
                {
                assert(m_summaryStatsColumnNames[columnCount] ==
                       wxString::Format(_(L"Number of %s unfamiliar words"),
                                        cTest.GetIterator()->get_name().c_str()));
                m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                 cTest.GetUnfamiliarWordCount());
                assert(m_summaryStatsColumnNames[columnCount] ==
                       wxString::Format(_(L"Number of unique %s unfamiliar words"),
                                        cTest.GetIterator()->get_name().c_str()));
                m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                                 cTest.GetUniqueUnfamiliarWordCount());
                }
            }
        if (GetStatisticsReportInfo().IsGrammarEnabled() && GetGrammarInfo().IsAnyFeatureEnabled())
            {
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of possible misspellings"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                             doc->GetMisspelledWordCount());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of repeated words"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetDuplicateWordCount());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of article mismatches"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                             doc->GetMismatchedArticleCount());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of errors & misspellings"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetWordingErrorCount());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of redundant phrases"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                             doc->GetRedundantPhraseCount());
            assert(m_summaryStatsColumnNames[columnCount] ==
                   _(L"Number of overused words (x sentence)"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                             doc->GetOverusedWordsBySentenceCount());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of wordy items"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetWordyPhraseCount());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of clich\u00E9s"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetClicheCount());
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Number of passive voices"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++, doc->GetPassiveVoicesCount());
            assert(m_summaryStatsColumnNames[columnCount] ==
                   _(L"Number of sentences that begin with conjunctions"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                             doc->GetSentenceStartingWithConjunctionsCount());
            assert(m_summaryStatsColumnNames[columnCount] ==
                   _(L"Number of Sentences that begin with lowercased words"));
            m_summaryStatsData->SetItemValue(rowCount, columnCount++,
                                             doc->GetSentenceStartingWithLowercaseCount());
            }
        if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
            {
            assert(m_summaryStatsColumnNames[columnCount] == _(L"Text size (Kbs.)"));
            m_summaryStatsData->SetItemValue(
                rowCount, columnCount++, safe_divide<double>(doc->GetTextSize(), 1024),
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 2, true));
            }

        ++rowCount;
        }
    // quneiform-suppress-end

    m_summaryStatsData->SetSize(rowCount);
    }

//------------------------------------------------------------
void BatchProjectDoc::LoadWarningsSection()
    {
    m_warnings->DeleteAllItems();
    m_warnings->SetSize((m_docs.size() * 2) + GetSubProjectMessages().size(), 3);

    size_t warningCount = 0;

    // warnings from batch project itself (shouldn't really happen)
    for (const auto& message : GetSubProjectMessages())
        {
        // in case there are more warnings than expected, then resize it
        if (warningCount >= m_warnings->GetItemCount())
            {
            m_warnings->SetSize(m_warnings->GetItemCount() * 1.5);
            }
        m_warnings->SetItemText(
            warningCount, 0, message.GetMessage(),
            Wisteria::NumberFormatInfo{
                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
            std::numeric_limits<double>::quiet_NaN());
        ++warningCount;
        }
    for (const auto& doc : m_docs)
        {
        if (!doc->GetSubProjectMessages().empty())
            {
            for (const auto& message : doc->GetSubProjectMessages())
                {
                // in case there are more warnings than expected, then resize it
                if (warningCount >= m_warnings->GetItemCount())
                    {
                    m_warnings->SetSize(m_warnings->GetItemCount() * 1.5);
                    }
                m_warnings->SetItemText(
                    warningCount, 0, doc->GetOriginalDocumentFilePath(),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                m_warnings->SetItemText(
                    warningCount, 1, doc->GetOriginalDocumentDescription(),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                m_warnings->SetItemText(
                    warningCount, 2, message.GetMessage(),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                ++warningCount;
                }
            }
        }

    m_warnings->SetSize(warningCount);
    }

//------------------------------------------------------------
bool BatchProjectDoc::LoadDocuments(wxProgressDialog& progressDlg)
    {
    GetRepeatedWordData()->DeleteAllItems();
    GetRepeatedWordData()->SetSize(m_docs.size(), 4);
    m_incorrectArticleData->DeleteAllItems();
    m_incorrectArticleData->SetSize(m_docs.size(), 4);
    m_overusedWordBySentenceData->DeleteAllItems();
    m_overusedWordBySentenceData->SetSize(m_docs.size(), 4);
    m_passiveVoiceData->DeleteAllItems();
    m_passiveVoiceData->SetSize(m_docs.size(), 4);
    GetMisspelledWordData()->DeleteAllItems();
    GetMisspelledWordData()->SetSize(m_docs.size(), 5);
    m_overlyLongSentenceData->DeleteAllItems();
    m_overlyLongSentenceData->SetSize(m_docs.size(), 5);
    m_sentenceStartingWithConjunctionsData->DeleteAllItems();
    m_sentenceStartingWithConjunctionsData->SetSize(m_docs.size(), 4);
    m_sentenceStartingWithLowercaseData->DeleteAllItems();
    m_sentenceStartingWithLowercaseData->SetSize(m_docs.size(), 4);
    m_wordyPhraseData->DeleteAllItems();
    m_wordyPhraseData->SetSize(m_docs.size(), 5);
    m_redundantPhraseData->DeleteAllItems();
    m_redundantPhraseData->SetSize(m_docs.size(), 5);
    m_wordingErrorData->DeleteAllItems();
    m_wordingErrorData->SetSize(m_docs.size(), 5);
    m_clichePhraseData->DeleteAllItems();
    m_clichePhraseData->SetSize(m_docs.size(), 5);

    size_t dupWordCount = 0;
    size_t incorrectArticleCount = 0;
    size_t overusedWordBySentenceCount = 0;
    size_t passiveVoiceCount = 0;
    size_t misspelledWordCount = 0;
    size_t longSenteceCount = 0;
    size_t conjunctionSentencesCount = 0;
    size_t lowercaseSentencesCount = 0;
    size_t wordyPhraseCount = 0;
    size_t redundantPhraseCount = 0;
    size_t wordingErrorCount = 0;
    size_t clicheCount = 0;

    int counter{ progressDlg.GetValue() };

    double_frequency_set<word_case_insensitive_no_stem> wordsFromAllDocs;

    std::map<wxString, Wisteria::ZipCatalog*> archiveFiles;
    std::map<wxString, ExcelFile*> excelFiles;
    for (auto& doc : m_docs)
        {
        // clear the document's text just in case the user switched from embedding to linking.
        // If the user switched from linking to embedded then note that the documents will need
        // to be externally loaded here to reacquire the text.
        if (GetDocumentStorageMethod() == TextStorage::NoEmbedText)
            {
            doc->FreeDocumentText();
            }
        // pre-2007 Microsoft Word files (*.doc) are difficult to detect lists in, so if we are
        // not explicitly specifying "fitted to the page" analysis for this project (above),
        // then override the global option and set it to treat all newlines as the
        // end of a paragraph.
        if (m_adjustParagraphParserForDocFiles &&
            wxFileName(doc->GetOriginalDocumentFilePath()).GetExt().CmpNoCase(_DT(L"doc")) == 0)
            {
            doc->SetParagraphsParsingMethod(ParagraphParse::EachNewLineIsAParagraph);
            }

        const FilePathResolver fileResolve(doc->GetOriginalDocumentFilePath(), false);
        if (fileResolve.IsExcelCell())
            {
            FilePathResolver fileResolver;
            size_t excelTag = doc->GetOriginalDocumentFilePath().Lower().find(_DT(L".xlsx#"));
            assert(excelTag != std::wstring::npos);
            if (excelTag != std::wstring::npos)
                {
                wxFileName fn(doc->GetOriginalDocumentFilePath().substr(0, excelTag + 5));
                if (!wxFile::Exists(fn.GetFullPath()))
                    {
                    wxString fileBySameNameInProjectDirectory;
                    if (FindMissingFile(fn.GetFullPath(), fileBySameNameInProjectDirectory))
                        {
                        doc->SetOriginalDocumentFilePath(
                            fileBySameNameInProjectDirectory +
                            doc->GetOriginalDocumentFilePath().substr(excelTag + 5));
                        excelTag = doc->GetOriginalDocumentFilePath().Lower().find(_DT(L".xlsx#"));
                        fn.Assign(fileBySameNameInProjectDirectory);
                        SetModifiedFlag();
                        }
                    }
                wxString worksheetName = doc->GetOriginalDocumentFilePath().substr(excelTag + 6);
                const size_t slash = worksheetName.find_last_of(L'#');
                if (slash != wxString::npos)
                    {
                    const wxString cellName = worksheetName.substr(slash + 1);
                    worksheetName.Truncate(slash);
                    const wxString workSheetPath = fn.GetFullPath() + L"#" + worksheetName;
                    auto excelFilePos = excelFiles.find(workSheetPath);
                    if (excelFilePos == excelFiles.end())
                        {
                        excelFilePos = excelFiles
                                           .insert(std::pair<wxString, ExcelFile*>(
                                               workSheetPath, new ExcelFile(fn.GetFullPath())))
                                           .first;
                        // read in the worksheets
                        const std::wstring workBookFileText =
                            excelFilePos->second->m_zip.ReadTextFile(L"xl/workbook.xml");
                        excelFilePos->second->m_xlsx_extract.read_worksheet_names(
                            workBookFileText.c_str(), workBookFileText.length());
                        // read workbook relationships
                        const std::wstring workbookRels =
                            excelFilePos->second->m_zip.ReadTextFile(L"xl/_rels/workbook.xml.rels");
                        excelFilePos->second->m_xlsx_extract.read_relative_paths(
                            workbookRels.c_str(), workbookRels.length());
                        // resolve worksheet names to XML paths
                        excelFilePos->second->m_xlsx_extract.map_workbook_paths();
                        // read in the string table
                        const std::wstring sharedStrings =
                            excelFilePos->second->m_zip.ReadTextFile(L"xl/sharedStrings.xml");
                        if (!sharedStrings.empty())
                            {
                            excelFilePos->second->m_xlsx_extract.read_shared_strings(
                                sharedStrings.c_str(), sharedStrings.length());
                            }
                        }

                    // find the sheet to get the cells from
                    const auto& worksheetPaths =
                        excelFilePos->second->m_xlsx_extract.get_worksheet_paths();

                    auto sheetPos =
                        std::ranges::find_if(worksheetPaths, [&](const auto& wsPath)
                                             { return wsPath.first == worksheetName.wc_str(); });

                    if (sheetPos != worksheetPaths.end())
                        {
                        const wxString internalSheetName = sheetPos->second;

                        // see if this worksheet is already loaded
                        auto internalSheetPos =
                            excelFilePos->second->m_worksheets.find(internalSheetName);

                        // wasn't loaded before, so load it now
                        if (internalSheetPos == excelFilePos->second->m_worksheets.end())
                            {
                            const std::pair<ExcelFile::Workbook::iterator, bool> insertPos =
                                excelFilePos->second->m_worksheets.insert(
                                    std::pair<wxString,
                                              lily_of_the_valley::xlsx_extract_text::worksheet>(
                                        internalSheetName,
                                        lily_of_the_valley::xlsx_extract_text::worksheet()));
                            internalSheetPos = insertPos.first;

                            const std::wstring sheetFile =
                                excelFilePos->second->m_zip.ReadTextFile(internalSheetName);

                            if (!sheetFile.empty())
                                {
                                excelFilePos->second->m_xlsx_extract(sheetFile.c_str(),
                                                                     sheetFile.length(),
                                                                     internalSheetPos->second);
                                }
                            }

                        const wxString cellText =
                            lily_of_the_valley::xlsx_extract_text::get_cell_text(
                                cellName.wc_str(), internalSheetPos->second);

                        fileResolver.ResolvePath(cellText, false);
                        if (!fileResolver.IsInvalidFile())
                            {
                            // this will change the spreadsheet cell path to the real file path
                            doc->LoadDocumentAsSubProject(fileResolver.GetResolvedPath(),
                                                          std::wstring{},
                                                          GetMinDocWordCountForBatch());
                            }
                        else
                            {
                            doc->SetDocumentText(cellText.wc_string());
                            doc->LoadDocumentAsSubProject(doc->GetOriginalDocumentFilePath(),
                                                          doc->GetDocumentText(),
                                                          GetMinDocWordCountForBatch());
                            }
                        }
                    else
                        {
                        doc->SetLoadingOriginalTextSucceeded(false);
                        }
                    }
                else
                    {
                    doc->SetLoadingOriginalTextSucceeded(false);
                    }
                }
            else
                {
                doc->SetLoadingOriginalTextSucceeded(false);
                }
            }
        else if (fileResolve.IsArchivedFile())
            {
            size_t archiveTag = doc->GetOriginalDocumentFilePath().Lower().find(_DT(L".zip#"));
            assert(archiveTag != std::wstring::npos);
            if (archiveTag != std::wstring::npos)
                {
                wxFileName fn(doc->GetOriginalDocumentFilePath().substr(0, archiveTag + 4));
                if (!wxFile::Exists(fn.GetFullPath()))
                    {
                    wxString fileBySameNameInProjectDirectory;
                    if (FindMissingFile(fn.GetFullPath(), fileBySameNameInProjectDirectory))
                        {
                        doc->SetOriginalDocumentFilePath(
                            fileBySameNameInProjectDirectory +
                            doc->GetOriginalDocumentFilePath().substr(archiveTag + 4));
                        archiveTag = doc->GetOriginalDocumentFilePath().Lower().find(_DT(L".zip#"));
                        fn.Assign(fileBySameNameInProjectDirectory);
                        SetModifiedFlag();
                        }
                    }
                auto archiveFilePos = archiveFiles.find(fn.GetFullPath());
                if (archiveFilePos == archiveFiles.end())
                    {
                    archiveFilePos =
                        archiveFiles
                            .insert(std::pair<wxString, Wisteria::ZipCatalog*>(
                                fn.GetFullPath(), new Wisteria::ZipCatalog(fn.GetFullPath())))
                            .first;
                    }
                wxMemoryOutputStream memstream;
                if (!archiveFilePos->second->ReadFile(
                        doc->GetOriginalDocumentFilePath().substr(archiveTag + 5), memstream) &&
                    !archiveFilePos->second->GetMessages().empty())
                    {
                    AddQuietSubProjectMessage(
                        archiveFilePos->second->GetMessages().back().m_message,
                        archiveFilePos->second->GetMessages().back().m_icon);
                    archiveFilePos->second->ClearMessages();
                    }
                // Only load the document if the archive read didn't fail.
                // Otherwise, LoadDocumentNoUI() will try to load the ZIP file and
                // get the same error.
                if (memstream.GetLength() != 0)
                    {
                    const std::pair<bool, std::wstring> extractResult = doc->ExtractRawText(
                        { static_cast<const char*>(
                              memstream.GetOutputStreamBuffer()->GetBufferStart()),
                          static_cast<size_t>(memstream.GetLength()) },
                        wxFileName(doc->GetOriginalDocumentFilePath()).GetExt());
                    doc->LoadDocumentAsSubProject(doc->GetOriginalDocumentFilePath(),
                                                  extractResult.second,
                                                  GetMinDocWordCountForBatch());
                    }
                else
                    {
                    doc->SetLoadingOriginalTextSucceeded(false);
                    }
                }
            else
                {
                doc->SetLoadingOriginalTextSucceeded(false);
                }
            }
        else
            {
            if (fileResolve.IsLocalOrNetworkFile() &&
                !wxFile::Exists(doc->GetOriginalDocumentFilePath()))
                {
                wxString fileBySameNameInProjectDirectory;
                if (FindMissingFile(doc->GetOriginalDocumentFilePath(),
                                    fileBySameNameInProjectDirectory))
                    {
                    doc->SetOriginalDocumentFilePath(fileBySameNameInProjectDirectory);
                    SetModifiedFlag();
                    }
                }
            doc->LoadDocumentAsSubProject(doc->GetOriginalDocumentFilePath(),
                                          doc->GetDocumentText(), GetMinDocWordCountForBatch());
            }
        // passing in an archived file that we extracted here will cause the
        // subproject to use embedded text, see reset it after loading the document
        doc->SetDocumentStorageMethod(GetDocumentStorageMethod());
        // free the text from the document to conserve memory
        // (unless we are embedding it in the project)
        if (GetDocumentStorageMethod() == TextStorage::NoEmbedText)
            {
            doc->FreeDocumentText();
            }

        // NOTE: Grammar info needs to be loaded here before the documents'
        // word collections are deleted

        // misspellings
        if (doc->LoadingOriginalTextSucceeded() && !doc->GetWords()->get_misspelled_words().empty())
            {
            GetMisspelledWordData()->SetItemText(
                misspelledWordCount, 0, doc->GetOriginalDocumentFilePath(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            GetMisspelledWordData()->SetItemText(
                misspelledWordCount, 1, doc->GetOriginalDocumentDescription(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            GetMisspelledWordData()->SetItemValue(misspelledWordCount, 2,
                                                  doc->GetWords()->get_misspelled_words().size());
            wxString misspelledWordsStr;
            frequency_set<traits::case_insensitive_wstring_ex> misspelledWords;
            const auto& misspelledWordIndices = doc->GetWords()->get_misspelled_words();
            for (const auto misspelledWordIndex : misspelledWordIndices)
                {
                misspelledWords.insert(doc->GetWords()->get_word(misspelledWordIndex).c_str());
                }
            GetMisspelledWordData()->SetItemValue(misspelledWordCount, 3,
                                                  misspelledWords.get_data().size());
            // these must all be quoted for the Add to Dictionary dialog to pick them up correctly
            for (const auto& misspelled : misspelledWords.get_data())
                {
                if (misspelled.second > 1)
                    {
                    misspelledWordsStr.Append(L'\"')
                        .Append(misspelled.first.c_str())
                        .Append(wxString::Format(L"\" * %zu, ", misspelled.second));
                    }
                else
                    {
                    misspelledWordsStr.Append(L'\"')
                        .Append(misspelled.first.c_str())
                        .Append(L"\", ");
                    }
                }
            // chop off the last ", "
            if (misspelledWordsStr.length() > 2)
                {
                misspelledWordsStr.RemoveLast(2);
                }
            GetMisspelledWordData()->SetItemText(
                misspelledWordCount++, 4, misspelledWordsStr,
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        // repeated (duplicate) words
        if (doc->LoadingOriginalTextSucceeded() &&
            !doc->GetWords()->get_duplicate_word_indices().empty())
            {
            GetRepeatedWordData()->SetItemText(
                dupWordCount, 0, doc->GetOriginalDocumentFilePath(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            GetRepeatedWordData()->SetItemText(
                dupWordCount, 1, doc->GetOriginalDocumentDescription(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            GetRepeatedWordData()->SetItemValue(
                dupWordCount, 2, doc->GetWords()->get_duplicate_word_indices().size());
            wxString doubleWordsStr;
            frequency_set<traits::case_insensitive_wstring_ex> doubleWords;
            const auto& dupWordIndices = doc->GetWords()->get_duplicate_word_indices();
            for (const auto dupWordIndex : dupWordIndices)
                {
                doubleWords.insert(doc->GetWords()->get_word(dupWordIndex).c_str());
                }
            const bool useQuotes{ doubleWords.get_data().size() > 1 };
            for (const auto& doubleWord : doubleWords.get_data())
                {
                if (doubleWord.second > 1)
                    {
                    doubleWordsStr.Append(L'\"')
                        .Append(doubleWord.first.c_str())
                        .Append(L' ')
                        .Append(doubleWord.first.c_str())
                        .Append(wxString::Format(L"\" * %zu, ", doubleWord.second));
                    }
                else
                    {
                    if (useQuotes)
                        {
                        doubleWordsStr.Append(L'\"')
                            .Append(doubleWord.first.c_str())
                            .Append(L' ')
                            .Append(doubleWord.first.c_str())
                            .Append(L"\", ");
                        }
                    else
                        {
                        doubleWordsStr.Append(doubleWord.first.c_str())
                            .Append(L' ')
                            .Append(doubleWord.first.c_str())
                            .Append(L", ");
                        }
                    }
                }
            // chop off the last ", "
            if (doubleWordsStr.length() > 2)
                {
                doubleWordsStr.RemoveLast(2);
                }
            GetRepeatedWordData()->SetItemText(
                dupWordCount++, 3, doubleWordsStr,
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        // incorrect articles
        if (doc->LoadingOriginalTextSucceeded() &&
            !doc->GetWords()->get_incorrect_article_indices().empty())
            {
            m_incorrectArticleData->SetItemText(
                incorrectArticleCount, 0, doc->GetOriginalDocumentFilePath(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_incorrectArticleData->SetItemText(
                incorrectArticleCount, 1, doc->GetOriginalDocumentDescription(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_incorrectArticleData->SetItemValue(
                incorrectArticleCount, 2, doc->GetWords()->get_incorrect_article_indices().size());
            wxString incorrectArticleStr;
            frequency_set<traits::case_insensitive_wstring_ex> incorrectArticles;
            const auto& incorrectArticleIndices = doc->GetWords()->get_incorrect_article_indices();

            for (const auto incorrectArticleIndex : incorrectArticleIndices)
                {
                incorrectArticles.insert(doc->GetWords()->get_word(incorrectArticleIndex) + L' ' +
                                         doc->GetWords()->get_word(incorrectArticleIndex + 1));
                }
            const bool useQuotes{ incorrectArticles.get_data().size() > 1 };
            for (const auto& incorrectArticle : incorrectArticles.get_data())
                {
                if (incorrectArticle.second > 1)
                    {
                    incorrectArticleStr.Append(L'\"')
                        .Append(incorrectArticle.first.c_str())
                        .Append(wxString::Format(L"\" * %zu, ", incorrectArticle.second));
                    }
                else
                    {
                    if (useQuotes)
                        {
                        incorrectArticleStr.Append(L'\"')
                            .Append(incorrectArticle.first.c_str())
                            .Append(L"\", ");
                        }
                    else
                        {
                        incorrectArticleStr.Append(incorrectArticle.first.c_str()).Append(L", ");
                        }
                    }
                }
            // chop off the last ", "
            if (incorrectArticleStr.length() > 2)
                {
                incorrectArticleStr.RemoveLast(2);
                }
            m_incorrectArticleData->SetItemText(
                incorrectArticleCount++, 3, incorrectArticleStr,
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        // overused words (by sentence)
        if (doc->LoadingOriginalTextSucceeded() &&
            !doc->GetWords()->get_overused_words_by_sentence().empty())
            {
            m_overusedWordBySentenceData->SetItemText(
                overusedWordBySentenceCount, 0, doc->GetOriginalDocumentFilePath(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_overusedWordBySentenceData->SetItemText(
                overusedWordBySentenceCount, 1, doc->GetOriginalDocumentDescription(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_overusedWordBySentenceData->SetItemValue(
                overusedWordBySentenceCount, 2,
                doc->GetWords()->get_overused_words_by_sentence().size());

            wxString theWords;
            for (auto overUsedWordsListsIter =
                     doc->GetWords()->get_overused_words_by_sentence().cbegin();
                 overUsedWordsListsIter != doc->GetWords()->get_overused_words_by_sentence().cend();
                 ++overUsedWordsListsIter)
                {
                theWords += L'\"';
                for (const auto overusedWords : overUsedWordsListsIter->second)
                    {
                    theWords.append(doc->GetWords()->get_word(overusedWords).c_str()).append(L" ");
                    }
                theWords.Trim();
                theWords += L"\", ";
                }
            // chop off the last ", "
            if (theWords.length() > 2)
                {
                theWords.RemoveLast(2);
                }
            m_overusedWordBySentenceData->SetItemText(
                overusedWordBySentenceCount++, 3, theWords,
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        // passive Voice
        if (doc->LoadingOriginalTextSucceeded() &&
            !doc->GetWords()->get_passive_voice_indices().empty())
            {
            m_passiveVoiceData->SetItemText(
                passiveVoiceCount, 0, doc->GetOriginalDocumentFilePath(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_passiveVoiceData->SetItemText(
                passiveVoiceCount, 1, doc->GetOriginalDocumentDescription(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_passiveVoiceData->SetItemValue(passiveVoiceCount, 2,
                                             doc->GetWords()->get_passive_voice_indices().size());
            wxString passiveVoiceStr;
            frequency_set<traits::case_insensitive_wstring_ex> passiveVoices;
            const auto& passiveVoiceIndices = doc->GetWords()->get_passive_voice_indices();
            for (const auto& passiveVoiceIndex : passiveVoiceIndices)
                {
                traits::case_insensitive_wstring_ex currentPassivePhrase;
                for (size_t wordCounter = 0; wordCounter < passiveVoiceIndex.second; ++wordCounter)
                    {
                    // NOLINTBEGIN(readability-redundant-string-cstr)
                    currentPassivePhrase +=
                        (wordCounter == passiveVoiceIndex.second - 1) ?
                            traits::case_insensitive_wstring_ex{
                                doc->GetWords()
                                    ->get_word(passiveVoiceIndex.first + wordCounter)
                                    .c_str()
                            } :
                            traits::case_insensitive_wstring_ex{
                                doc->GetWords()
                                    ->get_word(passiveVoiceIndex.first + wordCounter)
                                    .c_str()
                            } + L' ';
                    // NOLINTEND(readability-redundant-string-cstr)
                    }
                passiveVoices.insert(currentPassivePhrase);
                }
            const bool useQuotes{ passiveVoices.get_data().size() > 1 };
            for (const auto& passiveVoice : passiveVoices.get_data())
                {
                if (passiveVoice.second > 1)
                    {
                    passiveVoiceStr.Append(L'\"')
                        .Append(passiveVoice.first.c_str())
                        .Append(wxString::Format(L"\" * %zu, ", passiveVoice.second));
                    }
                else
                    {
                    if (useQuotes)
                        {
                        passiveVoiceStr.Append(L'\"')
                            .Append(passiveVoice.first.c_str())
                            .Append(L"\", ");
                        }
                    else
                        {
                        passiveVoiceStr.Append(passiveVoice.first.c_str()).Append(L", ");
                        }
                    }
                }
            // chop off the last ", "
            if (passiveVoiceStr.length() > 2)
                {
                passiveVoiceStr.RemoveLast(2);
                }
            m_passiveVoiceData->SetItemText(
                passiveVoiceCount++, 3, passiveVoiceStr,
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        // overly long sentences
        if (doc->LoadingOriginalTextSucceeded() && doc->GetTotalOverlyLongSentences() > 0)
            {
            m_overlyLongSentenceData->SetItemText(
                longSenteceCount, 0, doc->GetOriginalDocumentFilePath(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_overlyLongSentenceData->SetItemText(
                longSenteceCount, 1, doc->GetOriginalDocumentDescription(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_overlyLongSentenceData->SetItemValue(longSenteceCount, 2,
                                                   doc->GetTotalOverlyLongSentences());
            m_overlyLongSentenceData->SetItemValue(longSenteceCount, 3, doc->GetLongestSentence());
            // piece the sentence together
            const grammar::sentence_info& sentence =
                doc->GetWords()->get_sentences()[doc->GetLongestSentenceIndex()];
            auto punctPos = doc->GetWords()->get_punctuation().begin();
            auto punctEnd = doc->GetWords()->get_punctuation().end();
            const wxString currentSentence =
                ProjectReportFormat::FormatSentence(doc, sentence, punctPos, punctEnd);

            m_overlyLongSentenceData->SetItemText(
                longSenteceCount++, 4, currentSentence,
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        // sentences that start with conjunctions
        if (doc->LoadingOriginalTextSucceeded() &&
            doc->GetSentenceStartingWithConjunctionsCount() > 0)
            {
            m_sentenceStartingWithConjunctionsData->SetItemText(
                conjunctionSentencesCount, 0, doc->GetOriginalDocumentFilePath(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_sentenceStartingWithConjunctionsData->SetItemText(
                conjunctionSentencesCount, 1, doc->GetOriginalDocumentDescription(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_sentenceStartingWithConjunctionsData->SetItemValue(
                conjunctionSentencesCount, 2, doc->GetSentenceStartingWithConjunctionsCount());
            wxString conjunctionsStr;
            frequency_set<traits::case_insensitive_wstring_ex> conjunctions;
            for (auto sentIter = doc->GetWords()->get_conjunction_beginning_sentences().cbegin();
                 sentIter != doc->GetWords()->get_conjunction_beginning_sentences().cend();
                 ++sentIter)
                {
                const size_t wordPos =
                    doc->GetWords()->get_sentences()[*sentIter].get_first_word_index();
                conjunctions.insert(doc->GetWords()->get_words()[wordPos].c_str());
                }
            for (const auto& conIter : conjunctions.get_data())
                {
                if (conIter.second > 1)
                    {
                    conjunctionsStr.Append(L'\"')
                        .Append(conIter.first.c_str())
                        .Append(wxString::Format(L"\" * %zu, ", conIter.second));
                    }
                else
                    {
                    conjunctionsStr.Append(L'\"').Append(conIter.first.c_str()).Append(L"\", ");
                    }
                }
            // chop off the last ", "
            if (conjunctionsStr.length() > 2)
                {
                conjunctionsStr.RemoveLast(2);
                }
            m_sentenceStartingWithConjunctionsData->SetItemText(
                conjunctionSentencesCount++, 3, conjunctionsStr,
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        // sentences that start with lowercase words
        if (doc->LoadingOriginalTextSucceeded() && doc->GetSentenceStartingWithLowercaseCount() > 0)
            {
            m_sentenceStartingWithLowercaseData->SetItemText(
                lowercaseSentencesCount, 0, doc->GetOriginalDocumentFilePath(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_sentenceStartingWithLowercaseData->SetItemText(
                lowercaseSentencesCount, 1, doc->GetOriginalDocumentDescription(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_sentenceStartingWithLowercaseData->SetItemValue(
                lowercaseSentencesCount, 2, doc->GetSentenceStartingWithLowercaseCount());
            wxString lowercasesStr;
            frequency_set<traits::case_insensitive_wstring_ex> lowercases;
            for (auto sentIter = doc->GetWords()->get_lowercase_beginning_sentences().cbegin();
                 sentIter != doc->GetWords()->get_lowercase_beginning_sentences().cend();
                 ++sentIter)
                {
                const size_t wordPos =
                    doc->GetWords()->get_sentences()[*sentIter].get_first_word_index();
                lowercases.insert(doc->GetWords()->get_words()[wordPos].c_str());
                }
            for (const auto& lowerCaseSent : lowercases.get_data())
                {
                if (lowerCaseSent.second > 1)
                    {
                    lowercasesStr.Append(L'\"')
                        .Append(lowerCaseSent.first.c_str())
                        .Append(wxString::Format(L"\" * %zu, ", lowerCaseSent.second));
                    }
                else
                    {
                    lowercasesStr.Append(L'\"').Append(lowerCaseSent.first.c_str()).Append(L"\", ");
                    }
                }
            // chop off the last ", "
            if (lowercasesStr.length() > 2)
                {
                lowercasesStr.RemoveLast(2);
                }
            m_sentenceStartingWithLowercaseData->SetItemText(
                lowercaseSentencesCount++, 3, lowercasesStr,
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        // wordy items & clichés
        if (doc->LoadingOriginalTextSucceeded() &&
            !doc->GetWords()->get_known_phrase_indices().empty())
            {
            const auto& wordyIndices = doc->GetWords()->get_known_phrase_indices();
            const auto& wordyPhrases = doc->GetWords()->get_known_phrases().get_phrases();
            frequency_map<traits::case_insensitive_wstring_ex, wxString> wordyPhrasesAndSuggestions;
            frequency_map<traits::case_insensitive_wstring_ex, wxString>
                redundantPhrasesAndSuggestions;
            frequency_map<traits::case_insensitive_wstring_ex, wxString> clichesAndSuggestions;
            frequency_map<traits::case_insensitive_wstring_ex, wxString> errorsAndSuggestions;
            // put together the phrases and their respective suggestions
            for (const auto& wordyIndex : wordyIndices)
                {
                switch (wordyPhrases[wordyIndex.second].first.get_type())
                    {
                case grammar::phrase_type::phrase_wordy:
                    wordyPhrasesAndSuggestions.insert(
                        wordyPhrases[wordyIndex.second].first.to_string().c_str(),
                        wordyPhrases[wordyIndex.second].second.c_str());
                    break;
                case grammar::phrase_type::phrase_redundant:
                    redundantPhrasesAndSuggestions.insert(
                        wordyPhrases[wordyIndex.second].first.to_string().c_str(),
                        wordyPhrases[wordyIndex.second].second.c_str());
                    break;
                case grammar::phrase_type::phrase_cliche:
                    clichesAndSuggestions.insert(
                        wordyPhrases[wordyIndex.second].first.to_string().c_str(),
                        wordyPhrases[wordyIndex.second].second.c_str());
                    break;
                case grammar::phrase_type::phrase_error:
                    errorsAndSuggestions.insert(
                        wordyPhrases[wordyIndex.second].first.to_string().c_str(),
                        wordyPhrases[wordyIndex.second].second.c_str());
                    break;
                    };
                }

            // if anything was found in this document then add it to the lists
            if (!errorsAndSuggestions.get_data().empty())
                {
                wxString values;
                wxString suggestions;
                size_t totalCount{ 0 };
                const bool useQuotes{ errorsAndSuggestions.get_data().size() > 1 };
                for (const auto& errorAndSuggestion : errorsAndSuggestions.get_data())
                    {
                    if (errorAndSuggestion.second.second > 1)
                        {
                        // quotes will be needed if a multiplier is being added
                        values.Append(L'\"')
                            .Append(errorAndSuggestion.first.c_str())
                            .Append(
                                wxString::Format(L"\" * %zu, ", errorAndSuggestion.second.second));
                        }
                    else
                        {
                        // if there are multiple issues and suggestions, then wrap each
                        // one in quotes
                        if (useQuotes)
                            {
                            values.Append(L'\"')
                                .Append(errorAndSuggestion.first.c_str())
                                .Append(L"\", ");
                            }
                        else
                            {
                            values.Append(errorAndSuggestion.first.c_str()).Append(L", ");
                            }
                        }
                    if (useQuotes)
                        {
                        suggestions.Append(L'\"')
                            .Append(errorAndSuggestion.second.first)
                            .Append(L"\", ");
                        }
                    else
                        {
                        suggestions.Append(errorAndSuggestion.second.first).Append(L", ");
                        }
                    totalCount += errorAndSuggestion.second.second;
                    }
                // trim off trailing comma and space
                if (values.length() > 2)
                    {
                    values.RemoveLast(2);
                    }
                if (suggestions.length() > 2)
                    {
                    suggestions.RemoveLast(2);
                    }
                m_wordingErrorData->SetItemText(
                    wordingErrorCount, 0, doc->GetOriginalDocumentFilePath(),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                m_wordingErrorData->SetItemText(
                    wordingErrorCount, 1, doc->GetOriginalDocumentDescription(),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                m_wordingErrorData->SetItemValue(wordingErrorCount, 2, totalCount);
                m_wordingErrorData->SetItemText(
                    wordingErrorCount, 3, values,
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                m_wordingErrorData->SetItemText(
                    wordingErrorCount++, 4, suggestions,
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            if (!wordyPhrasesAndSuggestions.get_data().empty())
                {
                wxString values;
                wxString suggestions;
                size_t totalCount{ 0 };
                const bool useQuotes{ wordyPhrasesAndSuggestions.get_data().size() > 1 };
                for (const auto& wordyPhrase : wordyPhrasesAndSuggestions.get_data())
                    {
                    if (wordyPhrase.second.second > 1)
                        {
                        values.Append(L'\"')
                            .Append(wordyPhrase.first.c_str())
                            .Append(wxString::Format(L"\" * %zu, ", wordyPhrase.second.second));
                        }
                    else
                        {
                        if (useQuotes)
                            {
                            values.Append(L'\"').Append(wordyPhrase.first.c_str()).Append(L"\", ");
                            }
                        else
                            {
                            values.Append(wordyPhrase.first.c_str()).Append(L", ");
                            }
                        }
                    if (useQuotes)
                        {
                        suggestions.Append(L'\"').Append(wordyPhrase.second.first).Append(L"\", ");
                        }
                    else
                        {
                        suggestions.Append(wordyPhrase.second.first).Append(L", ");
                        }
                    totalCount += wordyPhrase.second.second;
                    }
                if (values.length() > 2)
                    {
                    values.RemoveLast(2);
                    }
                if (suggestions.length() > 2)
                    {
                    suggestions.RemoveLast(2);
                    }
                m_wordyPhraseData->SetItemText(
                    wordyPhraseCount, 0, doc->GetOriginalDocumentFilePath(),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                m_wordyPhraseData->SetItemText(
                    wordyPhraseCount, 1, doc->GetOriginalDocumentDescription(),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                m_wordyPhraseData->SetItemValue(wordyPhraseCount, 2, totalCount);
                m_wordyPhraseData->SetItemText(
                    wordyPhraseCount, 3, values,
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                m_wordyPhraseData->SetItemText(
                    wordyPhraseCount++, 4, suggestions,
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            if (!redundantPhrasesAndSuggestions.get_data().empty())
                {
                wxString values;
                wxString suggestions;
                size_t totalCount{ 0 };
                const bool useQuotes{ redundantPhrasesAndSuggestions.get_data().size() > 1 };
                for (const auto& redundant : redundantPhrasesAndSuggestions.get_data())
                    {
                    if (redundant.second.second > 1)
                        {
                        values.Append(L'\"')
                            .Append(redundant.first.c_str())
                            .Append(wxString::Format(L"\" * %zu, ", redundant.second.second));
                        }
                    else
                        {
                        if (useQuotes)
                            {
                            values.Append(L'\"').Append(redundant.first.c_str()).Append(L"\", ");
                            }
                        else
                            {
                            values.Append(redundant.first.c_str()).Append(L", ");
                            }
                        }
                    if (useQuotes)
                        {
                        suggestions.Append(L'\"').Append(redundant.second.first).Append(L"\", ");
                        }
                    else
                        {
                        suggestions.Append(redundant.second.first).Append(L", ");
                        }
                    totalCount += redundant.second.second;
                    }
                if (values.length() > 2)
                    {
                    values.RemoveLast(2);
                    }
                if (suggestions.length() > 2)
                    {
                    suggestions.RemoveLast(2);
                    }
                m_redundantPhraseData->SetItemText(
                    redundantPhraseCount, 0, doc->GetOriginalDocumentFilePath(),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                m_redundantPhraseData->SetItemText(
                    redundantPhraseCount, 1, doc->GetOriginalDocumentDescription(),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                m_redundantPhraseData->SetItemValue(redundantPhraseCount, 2, totalCount);
                m_redundantPhraseData->SetItemText(
                    redundantPhraseCount, 3, values,
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                m_redundantPhraseData->SetItemText(
                    redundantPhraseCount++, 4, suggestions,
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            if (!clichesAndSuggestions.get_data().empty())
                {
                wxString values;
                wxString suggestions;
                size_t totalCount{ 0 };
                const bool useQuotes{ clichesAndSuggestions.get_data().size() > 1 };
                for (const auto& cliche : clichesAndSuggestions.get_data())
                    {
                    if (cliche.second.second > 1)
                        {
                        values.Append(L'\"')
                            .Append(cliche.first.c_str())
                            .Append(wxString::Format(L"\" * %zu, ", cliche.second.second));
                        }
                    else
                        {
                        if (useQuotes)
                            {
                            values.Append(L'\"').Append(cliche.first.c_str()).Append(L"\", ");
                            }
                        else
                            {
                            values.Append(cliche.first.c_str()).Append(L", ");
                            }
                        }
                    if (useQuotes)
                        {
                        suggestions.Append(L'\"').Append(cliche.second.first).Append(L"\", ");
                        }
                    else
                        {
                        suggestions.Append(cliche.second.first).Append(L", ");
                        }
                    totalCount += cliche.second.second;
                    }
                if (values.length() > 2)
                    {
                    values.RemoveLast(2);
                    }
                if (suggestions.length() > 2)
                    {
                    suggestions.RemoveLast(2);
                    }
                m_clichePhraseData->SetItemText(
                    clicheCount, 0, doc->GetOriginalDocumentFilePath(),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                m_clichePhraseData->SetItemText(
                    clicheCount, 1, doc->GetOriginalDocumentDescription(),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                m_clichePhraseData->SetItemValue(clicheCount, 2, totalCount);
                m_clichePhraseData->SetItemText(
                    clicheCount, 3, values,
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                m_clichePhraseData->SetItemText(
                    clicheCount++, 4, suggestions,
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }

        if (doc->LoadingOriginalTextSucceeded() && doc->GetWordsWithFrequencies())
            {
            wordsFromAllDocs.insert_with_custom_increment(*doc->GetWordsWithFrequencies(), 1);
            }

        // free up some memory by destroying the indexed data in the document
        doc->DeleteUniqueWordMap();
        doc->DeleteWords();

        if (!progressDlg.Update(counter++))
            {
            return false;
            }
        }

    // move all the words (from all documents) into lists
    multi_value_frequency_aggregate_map<traits::case_insensitive_wstring_ex,
                                        traits::case_insensitive_wstring_ex>
        keyWordsStemmedWithCounts;

    GetAllWordsBatchData()->DeleteAllItems();
    GetAllWordsBatchData()->SetSize(wordsFromAllDocs.get_data().size(), 3);

    auto stemmer = CreateStemmer();
    const auto& commonWords = GetStopList();
    size_t i = 0;
    for (auto wordPos = wordsFromAllDocs.get_data().cbegin();
         wordPos != wordsFromAllDocs.get_data().cend(); ++wordPos, ++i)
        {
        GetAllWordsBatchData()->SetItemText(
            i, 0, wordPos->first.c_str(),
            Wisteria::NumberFormatInfo{
                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
            std::numeric_limits<double>::quiet_NaN());
        GetAllWordsBatchData()->SetItemValue(i, 1, wordPos->second.first);
        GetAllWordsBatchData()->SetItemValue(i, 2, wordPos->second.second);

        if (!wordPos->first.is_file_address() && !wordPos->first.is_numeric() &&
            !commonWords.contains(wordPos->first.c_str()))
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
                    traits::case_insensitive_wstring_ex stemmedWord{ wordPos->first.c_str(),
                                                                     wordPos->first.length() };
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
                traits::case_insensitive_wstring_ex stemmedWord{ wordPos->first.c_str(),
                                                                 wordPos->first.length() };
                (*stemmer)(stemmedWord);
                keyWordsStemmedWithCounts.insert(
                    // the stem and original word
                    std::move(stemmedWord), wordPos->first,
                    // overall frequency of current word
                    wordPos->second.first);
                }
            }
        }

    // prepare word cloud dataset
    if (m_keyWordsDataset == nullptr)
        {
        m_keyWordsDataset = std::make_shared<Wisteria::Data::Dataset>();
        }
    m_keyWordsDataset->Clear();
    m_keyWordsDataset->AddCategoricalColumn(GetWordsColumnName());
    m_keyWordsDataset->AddContinuousColumn(GetWordsCountsColumnName());
    wxASSERT_MSG(m_keyWordsDataset->GetCategoricalColumns().size() == 1,
                 L"Hard word dataset invalid!");
    wxASSERT_MSG(m_keyWordsDataset->GetRowCount() == 0, L"Hard word dataset should be empty!");
    m_keyWordsDataset->Resize(keyWordsStemmedWithCounts.get_data().size());
    auto keyWordsColumn = m_keyWordsDataset->GetCategoricalColumn(GetWordsColumnName());
    auto keydWordsFreqColumn = m_keyWordsDataset->GetContinuousColumn(GetWordsCountsColumnName());

        // condensed keywords & word cloud
        {
        GetKeyWordsBatchData()->DeleteAllItems();
        GetKeyWordsBatchData()->SetSize(keyWordsStemmedWithCounts.get_data().size(), 2);

        size_t uniqueImportWordsCount{ 0 };
        size_t wordCloudWordsCount{ 0 };
        wxString allValuesStr;
        for (const auto& [keyWordStem, keyWordFreqInfo] : keyWordsStemmedWithCounts.get_data())
            {
            // aggregate all the variations of the current word that share a common stem
            allValuesStr.clear();
            for (const auto& subWord : keyWordFreqInfo.first.get_data())
                {
                allValuesStr.append(subWord.first.c_str()).append(L"; ");
                }
            allValuesStr.Trim().RemoveLast();

            GetKeyWordsBatchData()->SetItemText(
                uniqueImportWordsCount, 0, allValuesStr,
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            GetKeyWordsBatchData()->SetItemValue(uniqueImportWordsCount++, 1,
                                                 keyWordFreqInfo.second);

            // word cloud
            // which variation of the current stem occurs the most often
            auto mostFrequentWordVariation = std::max_element(
                keyWordFreqInfo.first.get_data().cbegin(), keyWordFreqInfo.first.get_data().cend(),
                [](const auto& lhv, const auto& rhv) noexcept { return lhv.second < rhv.second; });
            wxASSERT_MSG(mostFrequentWordVariation != keyWordFreqInfo.first.get_data().cend(),
                         L"Empty word list for stemmed word?!");
            // add the next word to the dataset's string table
            const auto nextKey = keyWordsColumn->GetNextKey();
            if (mostFrequentWordVariation != keyWordFreqInfo.first.get_data().cend())
                {
                keyWordsColumn->GetStringTable().insert(
                    std::make_pair(nextKey, mostFrequentWordVariation->first.c_str()));
                }
            // could never happen, but for robustness’s sake use the stem word
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
        GetKeyWordsBatchData()->SetSize(uniqueImportWordsCount);
        }

    GetRepeatedWordData()->SetSize(dupWordCount);
    m_incorrectArticleData->SetSize(incorrectArticleCount);
    m_overusedWordBySentenceData->SetSize(overusedWordBySentenceCount);
    m_passiveVoiceData->SetSize(passiveVoiceCount);
    GetMisspelledWordData()->SetSize(misspelledWordCount);
    m_overlyLongSentenceData->SetSize(longSenteceCount);
    m_sentenceStartingWithConjunctionsData->SetSize(conjunctionSentencesCount);
    m_sentenceStartingWithLowercaseData->SetSize(lowercaseSentencesCount);
    m_wordyPhraseData->SetSize(wordyPhraseCount);
    m_redundantPhraseData->SetSize(redundantPhraseCount);
    m_wordingErrorData->SetSize(wordingErrorCount);
    m_clichePhraseData->SetSize(clicheCount);

    // in case any webpaths were redirected, we will need to recreate the list of document paths
    SyncFilePathsWithDocuments();

    for (auto& archiveFile : archiveFiles)
        {
        wxDELETE(archiveFile.second);
        }
    for (auto& excelFile : excelFiles)
        {
        wxDELETE(excelFile.second);
        }

    return true;
    }

//------------------------------------------------------------
void BatchProjectDoc::LoadScoresSection()
    {
    PROFILE();
    auto* view = dynamic_cast<BatchProjectView*>(GetFirstView());

    // update any stats goals (test goals are reviewed as the tests are added below).
    for (auto* doc : m_docs)
        {
        doc->ReviewStatGoals();
        }

    m_customTestScores.clear();

    // clear out the aggregated (standard) test scores,
    // resize them (if needed), and reset their string tables
    for (auto& sTest : GetReadabilityTests().get_tests())
        {
        sTest.get_grade_point_collection()->Clear();
        sTest.get_index_point_collection()->Clear();
        sTest.get_cloze_point_collection()->Clear();
        // add columns for the scores and groups
        sTest.get_grade_point_collection()->AddContinuousColumn(GetScoreColumnName());
        sTest.get_index_point_collection()->AddContinuousColumn(GetScoreColumnName());
        sTest.get_cloze_point_collection()->AddContinuousColumn(GetScoreColumnName());

        sTest.get_grade_point_collection()->AddCategoricalColumn(GetGroupColumnName());
        sTest.get_index_point_collection()->AddCategoricalColumn(GetGroupColumnName());
        sTest.get_cloze_point_collection()->AddCategoricalColumn(GetGroupColumnName());
        if (sTest.is_included())
            {
            if (sTest.get_test().get_test_type() == readability::readability_test_type::grade_level)
                {
                sTest.get_grade_point_collection()->Reserve(m_docs.size());
                }
            else if (sTest.get_test().get_test_type() ==
                     readability::readability_test_type::index_value)
                {
                sTest.get_index_point_collection()->Reserve(m_docs.size());
                }
            else if (sTest.get_test().get_test_type() ==
                     readability::readability_test_type::predicted_cloze_score)
                {
                sTest.get_cloze_point_collection()->Reserve(m_docs.size());
                }
            else if (sTest.get_test().get_test_type() ==
                     readability::readability_test_type::index_value_and_grade_level)
                {
                sTest.get_grade_point_collection()->Reserve(m_docs.size());
                sTest.get_index_point_collection()->Reserve(m_docs.size());
                }
            else if (sTest.get_test().get_test_type() ==
                     readability::readability_test_type::grade_level_and_predicted_cloze_score)
                {
                sTest.get_grade_point_collection()->Reserve(m_docs.size());
                sTest.get_cloze_point_collection()->Reserve(m_docs.size());
                }
            for (const auto& docLabel : GetDocumentLabels())
                {
                sTest.get_grade_point_collection()
                    ->GetCategoricalColumn(GetGroupColumnName())
                    ->GetStringTable()[docLabel.second] = docLabel.first.c_str();
                sTest.get_index_point_collection()
                    ->GetCategoricalColumn(GetGroupColumnName())
                    ->GetStringTable()[docLabel.second] = docLabel.first.c_str();
                sTest.get_cloze_point_collection()
                    ->GetCategoricalColumn(GetGroupColumnName())
                    ->GetStringTable()[docLabel.second] = docLabel.first.c_str();
                }
            }
        }

    m_customTestScores.resize(GetCustTestsInUse().size());
    for (auto& customTestScores : m_customTestScores)
        {
        if (customTestScores == nullptr)
            {
            customTestScores = std::make_shared<Wisteria::Data::Dataset>();
            }
        customTestScores->Clear();
        customTestScores->AddContinuousColumn(GetScoreColumnName());
        customTestScores->AddCategoricalColumn(GetGroupColumnName());
        customTestScores->Reserve(m_docs.size());
        for (const auto& docLabel : GetDocumentLabels())
            {
            customTestScores->GetCategoricalColumn(GetGroupColumnName())
                ->GetStringTable()[docLabel.second] = docLabel.first.c_str();
            }
        }

    m_scoreRawData->SetNumberFormatter(GetReadabilityMessageCatalogPtr());
    m_scoreRawData->DeleteAllItems();
    // maximum test count + document path + document description (and any extra columns for tests
    // with 2 results)
    //(we use their grade AND index values in the output)
    m_scoreRawData->SetSize(m_docs.size(), GetStandardTestCount() + GetCustomTestCount() +
                                               GetMultiResultTestCount() + 2);

    size_t i = 0;
    for (auto& doc : m_docs)
        {
        doc->GetAggregatedGradeScores().clear();
        doc->GetAggregatedClozeScores().clear();
        if (!doc->LoadingOriginalTextSucceeded())
            {
            continue;
            }
        long currentColumn = 0;
        m_scoreRawData->SetItemText(
            i, currentColumn++, doc->GetOriginalDocumentFilePath(),
            Wisteria::NumberFormatInfo{
                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
            std::numeric_limits<double>::quiet_NaN());
        m_scoreRawData->SetItemText(
            i, currentColumn++, doc->GetOriginalDocumentDescription(),
            Wisteria::NumberFormatInfo{
                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
            std::numeric_limits<double>::quiet_NaN());
        double value = 0;

        // if using groups for the documents
        auto docLabel = GetDocumentLabels().find(doc->GetOriginalDocumentDescription().wc_str());
        const Wisteria::Data::GroupIdType groupId =
            (docLabel != GetDocumentLabels().end()) ? docLabel->second : 0;

        // go through the standard tests
        for (auto& rTests : GetReadabilityTests().get_tests())
            {
            // grade level tests
            if (rTests.is_included() && rTests.get_test().get_test_type() ==
                                            readability::readability_test_type::grade_level)
                {
                // have special logic for graphical tests
                if (rTests.get_test().get_id() == ReadabilityMessages::FRY().wc_str())
                    {
                    const auto fryGraph = std::dynamic_pointer_cast<Wisteria::Graphs::FryGraph>(
                        view->GetFryGraph()->GetFixedObject(0, 0));
                    if (!fryGraph->GetScores().at(i).IsScoreInvalid() &&
                        !fryGraph->GetScores().at(i).IsScoreOutOfGradeRange())
                        {
                        rTests.get_grade_point_collection()->AddRow(
                            Wisteria::Data::RowInfo()
                                .Continuous(
                                    { static_cast<double>(fryGraph->GetScores().at(i).GetScore()) })
                                .Categoricals({ groupId })
                                .Id(wxFileName(doc->GetOriginalDocumentFilePath()).GetFullName()));
                        m_scoreRawData->SetItemText(
                            i, currentColumn++,
                            wxNumberFormatter::ToString(
                                fryGraph->GetScores().at(i).GetScore(), 0,
                                wxNumberFormatter::Style::Style_NoTrailingZeroes),
                            Wisteria::NumberFormatInfo(
                                Wisteria::NumberFormatInfo::NumberFormatType::CustomFormatting, 1),
                            std::numeric_limits<double>::quiet_NaN());
                        doc->GetAggregatedGradeScores().push_back(
                            fryGraph->GetScores().at(i).GetScore());
                        doc->ReviewTestGoal(ReadabilityMessages::FRY(),
                                            fryGraph->GetScores().at(i).GetScore());
                        }
                    else if (fryGraph->GetScores().at(i).IsScoreInvalid())
                        {
                        m_scoreRawData->SetItemText(
                            i, currentColumn++, _(L"Text is too difficult to be plotted."),
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        doc->ReviewTestGoal(ReadabilityMessages::FRY(),
                                            std::numeric_limits<double>::quiet_NaN());
                        }
                    else if (fryGraph->GetScores().at(i).IsScoreOutOfGradeRange())
                        {
                        const wxString tooDifficultDescription =
                            fryGraph->GetScores().at(i).IsWordsHard() ?
                                _(L"Text is too difficult to be classified to a specific grade "
                                  "level because it contains too many high syllable words.") :
                                _(L"Text is too difficult to be classified to a specific grade "
                                  "level because it contains too many long sentences.");
                        m_scoreRawData->SetItemText(
                            i, currentColumn++, tooDifficultDescription,
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        doc->ReviewTestGoal(ReadabilityMessages::FRY(),
                                            std::numeric_limits<double>::quiet_NaN());
                        }
                    else
                        {
                        m_scoreRawData->SetItemText(
                            i, currentColumn++, wxString{},
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        doc->ReviewTestGoal(ReadabilityMessages::FRY(),
                                            std::numeric_limits<double>::quiet_NaN());
                        }
                    }
                else if (rTests.get_test().get_id() == ReadabilityMessages::GPM_FRY().wc_str())
                    {
                    const auto fryGraph = std::dynamic_pointer_cast<Wisteria::Graphs::FryGraph>(
                        view->GetGpmFryGraph()->GetFixedObject(0, 0));
                    if (!fryGraph->GetScores().at(i).IsScoreInvalid() &&
                        !fryGraph->GetScores().at(i).IsScoreOutOfGradeRange())
                        {
                        rTests.get_grade_point_collection()->AddRow(
                            Wisteria::Data::RowInfo()
                                .Continuous(
                                    { static_cast<double>(fryGraph->GetScores().at(i).GetScore()) })
                                .Categoricals({ groupId })
                                .Id(wxFileName(doc->GetOriginalDocumentFilePath()).GetFullName()));
                        m_scoreRawData->SetItemText(
                            i, currentColumn++,
                            wxNumberFormatter::ToString(
                                fryGraph->GetScores().at(i).GetScore(), 0,
                                wxNumberFormatter::Style::Style_NoTrailingZeroes),
                            Wisteria::NumberFormatInfo(
                                Wisteria::NumberFormatInfo::NumberFormatType::CustomFormatting, 1),
                            std::numeric_limits<double>::quiet_NaN());
                        doc->GetAggregatedGradeScores().push_back(
                            fryGraph->GetScores().at(i).GetScore());
                        doc->ReviewTestGoal(ReadabilityMessages::GPM_FRY(),
                                            fryGraph->GetScores().at(i).GetScore());
                        }
                    else if (fryGraph->GetScores().at(i).IsScoreInvalid())
                        {
                        m_scoreRawData->SetItemText(
                            i, currentColumn++, _(L"Text is too difficult to be plotted."),
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        doc->ReviewTestGoal(ReadabilityMessages::GPM_FRY(),
                                            std::numeric_limits<double>::quiet_NaN());
                        }
                    else if (fryGraph->GetScores().at(i).IsScoreOutOfGradeRange())
                        {
                        const wxString tooDifficultDescription =
                            fryGraph->GetScores().at(i).IsWordsHard() ?
                                _(L"Text is too difficult to be classified to a specific "
                                  "grade level because it contains too many high syllable words.") :
                                _(L"Text is too difficult to be classified to a specific "
                                  "grade level because it contains too many long sentences.");

                        m_scoreRawData->SetItemText(
                            i, currentColumn++, tooDifficultDescription,
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        doc->ReviewTestGoal(ReadabilityMessages::GPM_FRY(),
                                            std::numeric_limits<double>::quiet_NaN());
                        }
                    else
                        {
                        m_scoreRawData->SetItemText(
                            i, currentColumn++, wxString{},
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        doc->ReviewTestGoal(ReadabilityMessages::GPM_FRY(),
                                            std::numeric_limits<double>::quiet_NaN());
                        }
                    }
                else if (rTests.get_test().get_id() == ReadabilityMessages::SCHWARTZ().wc_str())
                    {
                    const auto schwartzGraph =
                        std::dynamic_pointer_cast<Wisteria::Graphs::SchwartzGraph>(
                            view->GetSchwartzGraph()->GetFixedObject(0, 0));
                    if (!schwartzGraph->GetScores().at(i).IsScoreInvalid() &&
                        !schwartzGraph->GetScores().at(i).IsScoreOutOfGradeRange())
                        {
                        rTests.get_grade_point_collection()->AddRow(
                            Wisteria::Data::RowInfo()
                                .Continuous({ schwartzGraph->GetScores().at(i).GetScoreAverage() })
                                .Categoricals({ groupId })
                                .Id(wxFileName(doc->GetOriginalDocumentFilePath()).GetFullName()));
                        m_scoreRawData->SetItemText(
                            i, currentColumn++,
                            wxNumberFormatter::ToString(
                                schwartzGraph->GetScores().at(i).GetScoreAverage(), 1,
                                wxNumberFormatter::Style::Style_NoTrailingZeroes),
                            Wisteria::NumberFormatInfo(
                                Wisteria::NumberFormatInfo::NumberFormatType::CustomFormatting, 1),
                            std::numeric_limits<double>::quiet_NaN());
                        doc->GetAggregatedGradeScores().push_back(
                            schwartzGraph->GetScores().at(i).GetScoreAverage());
                        doc->ReviewTestGoal(ReadabilityMessages::SCHWARTZ(),
                                            schwartzGraph->GetScores().at(i).GetScoreAverage());
                        }
                    else if (schwartzGraph->GetScores().at(i).IsScoreInvalid())
                        {
                        m_scoreRawData->SetItemText(
                            i, currentColumn++, _(L"Text is too difficult to be plotted."),
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        doc->ReviewTestGoal(ReadabilityMessages::SCHWARTZ(),
                                            std::numeric_limits<double>::quiet_NaN());
                        }
                    else if (schwartzGraph->GetScores().at(i).IsScoreOutOfGradeRange())
                        {
                        const wxString tooDifficultDescription =
                            schwartzGraph->GetScores().at(i).IsWordsHard() ?
                                _(L"Text is too difficult to be classified to a specific grade "
                                  "level because it contains too many high syllable words.") :
                                _(L"Text is too difficult to be classified to a specific grade "
                                  "level because it contains too many long sentences.");
                        m_scoreRawData->SetItemText(
                            i, currentColumn++, tooDifficultDescription,
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        doc->ReviewTestGoal(ReadabilityMessages::SCHWARTZ(),
                                            std::numeric_limits<double>::quiet_NaN());
                        }
                    else
                        {
                        m_scoreRawData->SetItemText(
                            i, currentColumn++, wxString{},
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        doc->ReviewTestGoal(ReadabilityMessages::SCHWARTZ(),
                                            std::numeric_limits<double>::quiet_NaN());
                        }
                    }
                else if (rTests.get_test().get_id() == ReadabilityMessages::RAYGOR().wc_str())
                    {
                    const auto raygorGraph =
                        std::dynamic_pointer_cast<Wisteria::Graphs::RaygorGraph>(
                            view->GetRaygorGraph()->GetFixedObject(0, 0));
                    if (!raygorGraph->GetScores().at(i).IsScoreInvalid() &&
                        !raygorGraph->GetScores().at(i).IsScoreOutOfGradeRange())
                        {
                        rTests.get_grade_point_collection()->AddRow(
                            Wisteria::Data::RowInfo()
                                .Continuous({ static_cast<double>(
                                    raygorGraph->GetScores().at(i).GetScore()) })
                                .Categoricals({ groupId })
                                .Id(wxFileName(doc->GetOriginalDocumentFilePath()).GetFullName()));
                        m_scoreRawData->SetItemText(
                            i, currentColumn++,
                            wxNumberFormatter::ToString(
                                raygorGraph->GetScores().at(i).GetScore(), 0,
                                wxNumberFormatter::Style::Style_NoTrailingZeroes),
                            Wisteria::NumberFormatInfo(
                                Wisteria::NumberFormatInfo::NumberFormatType::CustomFormatting, 1),
                            std::numeric_limits<double>::quiet_NaN());
                        doc->GetAggregatedGradeScores().push_back(
                            raygorGraph->GetScores().at(i).GetScore());
                        doc->ReviewTestGoal(ReadabilityMessages::RAYGOR(),
                                            raygorGraph->GetScores().at(i).GetScore());
                        }
                    else if (raygorGraph->GetScores().at(i).IsScoreInvalid())
                        {
                        m_scoreRawData->SetItemText(
                            i, currentColumn++, _(L"Text is too difficult to be plotted."),
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        doc->ReviewTestGoal(ReadabilityMessages::RAYGOR(),
                                            std::numeric_limits<double>::quiet_NaN());
                        }
                    else if (raygorGraph->GetScores().at(i).IsScoreOutOfGradeRange())
                        {
                        const wxString tooDifficultDescription =
                            raygorGraph->GetScores().at(i).IsWordsHard() ?
                                _(L"Text is too difficult to be classified to a specific "
                                  "grade level because it contains too many 6+ character words.") :
                                _(L"Text is too difficult to be classified to a specific "
                                  "grade level because it contains too many long sentences.");

                        m_scoreRawData->SetItemText(
                            i, currentColumn++, tooDifficultDescription,
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        doc->ReviewTestGoal(ReadabilityMessages::RAYGOR(),
                                            std::numeric_limits<double>::quiet_NaN());
                        }
                    else
                        {
                        m_scoreRawData->SetItemText(
                            i, currentColumn++, wxString{},
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        doc->ReviewTestGoal(ReadabilityMessages::RAYGOR(),
                                            std::numeric_limits<double>::quiet_NaN());
                        }
                    }
                else if (doc->AddStandardReadabilityTest(rTests.get_test().get_id().c_str()) &&
                         ReadabilityMessages::GetScoreValue(doc->GetLastGradeLevel(), value))
                    {
                    rTests.get_grade_point_collection()->AddRow(
                        Wisteria::Data::RowInfo()
                            .Continuous({ value })
                            .Categoricals({ groupId })
                            .Id(wxFileName(doc->GetOriginalDocumentFilePath()).GetFullName()));
                    m_scoreRawData->SetItemText(
                        i, currentColumn++, doc->GetLastGradeLevel(),
                        Wisteria::NumberFormatInfo(
                            Wisteria::NumberFormatInfo::NumberFormatType::CustomFormatting, 1),
                        std::numeric_limits<double>::quiet_NaN());
                    doc->GetAggregatedGradeScores().push_back(value);
                    }
                else
                    {
                    m_scoreRawData->SetItemText(
                        i, currentColumn++, wxString{},
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    }
                }
            // index tests
            else if (rTests.is_included() && rTests.get_test().get_test_type() ==
                                                 readability::readability_test_type::index_value)
                {
                if (rTests.get_test().get_id() == ReadabilityMessages::FRASE().wc_str())
                    {
                    auto fraseGraph = std::dynamic_pointer_cast<Wisteria::Graphs::FraseGraph>(
                        view->GetFraseGraph()->GetFixedObject(0, 0));
                    if (!fraseGraph->GetScores().at(i).IsScoreInvalid())
                        {
                        rTests.get_index_point_collection()->AddRow(
                            Wisteria::Data::RowInfo()
                                .Continuous({ static_cast<double>(
                                    fraseGraph->GetScores().at(i).GetScore()) })
                                .Categoricals({ groupId })
                                .Id(wxFileName(doc->GetOriginalDocumentFilePath()).GetFullName()));
                        m_scoreRawData->SetItemText(
                            i, currentColumn++,
                            wxNumberFormatter::ToString(
                                fraseGraph->GetScores().at(i).GetScore(), 0,
                                wxNumberFormatter::Style::Style_NoTrailingZeroes),
                            Wisteria::NumberFormatInfo(
                                Wisteria::NumberFormatInfo::NumberFormatType::CustomFormatting, 1),
                            std::numeric_limits<double>::quiet_NaN());
                        doc->ReviewTestGoal(ReadabilityMessages::FRASE(),
                                            fraseGraph->GetScores().at(i).GetScore());
                        }
                    else if (fraseGraph->GetScores().at(i).IsScoreInvalid())
                        {
                        m_scoreRawData->SetItemText(
                            i, currentColumn++, _(L"Text is too difficult to be plotted."),
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        doc->ReviewTestGoal(ReadabilityMessages::FRASE(),
                                            std::numeric_limits<double>::quiet_NaN());
                        }
                    else
                        {
                        m_scoreRawData->SetItemText(
                            i, currentColumn++, wxString{},
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        doc->ReviewTestGoal(ReadabilityMessages::FRASE(),
                                            std::numeric_limits<double>::quiet_NaN());
                        }
                    }
                else if (doc->AddStandardReadabilityTest(rTests.get_test().get_id().c_str()) &&
                         !std::isnan(doc->GetLastIndexScore()))
                    {
                    rTests.get_index_point_collection()->AddRow(
                        Wisteria::Data::RowInfo()
                            .Continuous({ doc->GetLastIndexScore() })
                            .Categoricals({ groupId })
                            .Id(wxFileName(doc->GetOriginalDocumentFilePath()).GetFullName()));
                    m_scoreRawData->SetItemValue(
                        i, currentColumn++, doc->GetLastIndexScore(),
                        Wisteria::NumberFormatInfo(
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 1));
                    }
                else
                    {
                    m_scoreRawData->SetItemText(
                        i, currentColumn++, wxString{},
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    }
                }
            // cloze tests
            else if (rTests.is_included() &&
                     rTests.get_test().get_test_type() ==
                         readability::readability_test_type::predicted_cloze_score)
                {
                if (doc->AddStandardReadabilityTest(rTests.get_test().get_id().c_str()) &&
                    !std::isnan(doc->GetLastClozeScore()))
                    {
                    rTests.get_cloze_point_collection()->AddRow(
                        Wisteria::Data::RowInfo()
                            .Continuous({ doc->GetLastClozeScore() })
                            .Categoricals({ groupId })
                            .Id(wxFileName(doc->GetOriginalDocumentFilePath()).GetFullName()));
                    m_scoreRawData->SetItemValue(i, currentColumn++, doc->GetLastClozeScore());
                    doc->GetAggregatedClozeScores().push_back(doc->GetLastClozeScore());
                    }
                else
                    {
                    m_scoreRawData->SetItemText(
                        i, currentColumn++, wxString{},
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    }
                }
            // grade and index test
            else if (rTests.is_included() &&
                     rTests.get_test().get_test_type() ==
                         readability::readability_test_type::index_value_and_grade_level)
                {
                if (doc->AddStandardReadabilityTest(rTests.get_test().get_id().c_str()))
                    {
                    if (!std::isnan(doc->GetLastIndexScore()))
                        {
                        rTests.get_index_point_collection()->AddRow(
                            Wisteria::Data::RowInfo()
                                .Continuous({ doc->GetLastIndexScore() })
                                .Categoricals({ groupId })
                                .Id(wxFileName(doc->GetOriginalDocumentFilePath()).GetFullName()));
                        m_scoreRawData->SetItemValue(
                            i, currentColumn++, doc->GetLastIndexScore(),
                            Wisteria::NumberFormatInfo(
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting,
                                1));
                        }
                    else
                        {
                        m_scoreRawData->SetItemText(
                            i, currentColumn++, wxString{},
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        }
                    if (ReadabilityMessages::GetScoreValue(doc->GetLastGradeLevel(), value))
                        {
                        rTests.get_grade_point_collection()->AddRow(
                            Wisteria::Data::RowInfo()
                                .Continuous({ value })
                                .Categoricals({ groupId })
                                .Id(wxFileName(doc->GetOriginalDocumentFilePath()).GetFullName()));
                        m_scoreRawData->SetItemText(
                            i, currentColumn++, doc->GetLastGradeLevel(),
                            Wisteria::NumberFormatInfo(
                                Wisteria::NumberFormatInfo::NumberFormatType::CustomFormatting, 1),
                            std::numeric_limits<double>::quiet_NaN());
                        doc->GetAggregatedGradeScores().push_back(value);
                        }
                    else
                        {
                        m_scoreRawData->SetItemText(
                            i, currentColumn++, wxString{},
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        }
                    }
                else
                    {
                    m_scoreRawData->SetItemText(
                        i, currentColumn++, wxString{},
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    m_scoreRawData->SetItemText(
                        i, currentColumn++, wxString{},
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    }
                }
            // grade and cloze score
            else if (rTests.is_included() &&
                     rTests.get_test().get_test_type() ==
                         readability::readability_test_type::grade_level_and_predicted_cloze_score)
                {
                if (doc->AddStandardReadabilityTest(rTests.get_test().get_id().c_str()))
                    {
                    if (ReadabilityMessages::GetScoreValue(doc->GetLastGradeLevel(), value))
                        {
                        rTests.get_grade_point_collection()->AddRow(
                            Wisteria::Data::RowInfo()
                                .Continuous({ value })
                                .Categoricals({ groupId })
                                .Id(wxFileName(doc->GetOriginalDocumentFilePath()).GetFullName()));
                        m_scoreRawData->SetItemText(
                            i, currentColumn++, doc->GetLastGradeLevel(),
                            Wisteria::NumberFormatInfo(
                                Wisteria::NumberFormatInfo::NumberFormatType::CustomFormatting, 1),
                            std::numeric_limits<double>::quiet_NaN());
                        doc->GetAggregatedGradeScores().push_back(value);
                        }
                    else
                        {
                        m_scoreRawData->SetItemText(
                            i, currentColumn++, wxString{},
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        }
                    if (!std::isnan(doc->GetLastClozeScore()))
                        {
                        rTests.get_cloze_point_collection()->AddRow(
                            Wisteria::Data::RowInfo()
                                .Continuous({ doc->GetLastClozeScore() })
                                .Categoricals({ groupId })
                                .Id(wxFileName(doc->GetOriginalDocumentFilePath()).GetFullName()));
                        m_scoreRawData->SetItemValue(i, currentColumn++, doc->GetLastClozeScore());
                        doc->GetAggregatedClozeScores().push_back(doc->GetLastClozeScore());
                        }
                    else
                        {
                        m_scoreRawData->SetItemText(
                            i, currentColumn++, wxString{},
                            Wisteria::NumberFormatInfo{
                                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                            std::numeric_limits<double>::quiet_NaN());
                        }
                    }
                else
                    {
                    m_scoreRawData->SetItemText(
                        i, currentColumn++, wxString{},
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    m_scoreRawData->SetItemText(
                        i, currentColumn++, wxString{},
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    }
                }
            }
        // go through the custom tests now
        for (auto testPos = GetCustTestsInUse().cbegin(); testPos != GetCustTestsInUse().cend();
             ++testPos)
            {
            if (doc->AddCustomReadabilityTest(testPos->GetTestName(), true))
                {
                if (testPos->GetIterator()->get_test_type() ==
                        readability::readability_test_type::grade_level &&
                    ReadabilityMessages::GetScoreValue(doc->GetLastGradeLevel(), value))
                    {
                    m_customTestScores[(testPos - GetCustTestsInUse().begin())]->AddRow(
                        Wisteria::Data::RowInfo()
                            .Continuous({ value })
                            .Categoricals({ groupId })
                            .Id(wxFileName(doc->GetOriginalDocumentFilePath()).GetFullName()));
                    m_scoreRawData->SetItemText(
                        i, currentColumn++, doc->GetLastGradeLevel(),
                        Wisteria::NumberFormatInfo(
                            Wisteria::NumberFormatInfo::NumberFormatType::CustomFormatting, 1),
                        std::numeric_limits<double>::quiet_NaN());
                    doc->GetAggregatedGradeScores().push_back(value);
                    }
                else if (testPos->GetIterator()->get_test_type() ==
                             readability::readability_test_type::index_value &&
                         !std::isnan(doc->GetLastIndexScore()))
                    {
                    m_customTestScores[(testPos - GetCustTestsInUse().begin())]->AddRow(
                        Wisteria::Data::RowInfo()
                            .Continuous({ doc->GetLastIndexScore() })
                            .Categoricals({ groupId })
                            .Id(wxFileName(doc->GetOriginalDocumentFilePath()).GetFullName()));
                    m_scoreRawData->SetItemValue(
                        i, currentColumn++, doc->GetLastIndexScore(),
                        Wisteria::NumberFormatInfo(
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 1));
                    }
                else if (testPos->GetIterator()->get_test_type() ==
                             readability::readability_test_type::predicted_cloze_score &&
                         !std::isnan(doc->GetLastClozeScore()))
                    {
                    m_customTestScores[(testPos - GetCustTestsInUse().begin())]->AddRow(
                        Wisteria::Data::RowInfo()
                            .Continuous({ doc->GetLastClozeScore() })
                            .Categoricals({ groupId })
                            .Id(wxFileName(doc->GetOriginalDocumentFilePath()).GetFullName()));
                    m_scoreRawData->SetItemValue(i, currentColumn++, doc->GetLastClozeScore());
                    doc->GetAggregatedClozeScores().push_back(doc->GetLastClozeScore());
                    }
                else
                    {
                    m_scoreRawData->SetItemText(
                        i, currentColumn++, wxString{},
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    }
                }
            else
                {
                m_scoreRawData->SetItemText(
                    i, currentColumn++, wxString{},
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }
        // increment this here because our current index into the score data will not be the same
        // as the index into the current document if any of the documents failed
        ++i;
        }
    m_scoreRawData->SetSize(i);

    m_scoreStatsData->SetNumberFormatter(GetReadabilityMessageCatalogPtr());
    m_scoreStatsData->DeleteAllItems();
    m_scoreStatsData->SetSize(
        // maximum test count
        GetStandardTestCount() + GetCustomTestCount() + GetMultiResultTestCount(),
        // maximum stats count, statistics + test name
        CUMULATIVE_STATS_COUNT + 1);
    long currentRow = 0;
    // Summarize the standard tests' scores
    for (auto& rTests : GetReadabilityTests().get_tests())
        {
        if (rTests.is_included() &&
            rTests.get_test().get_test_type() == readability::readability_test_type::grade_level)
            {
            SetScoreStatsRow(m_scoreStatsData, rTests.get_test().get_long_name().c_str(),
                             wxString{}, currentRow++,
                             rTests.get_grade_point_collection()
                                 ->GetContinuousColumn(GetScoreColumnName())
                                 ->GetValues(),
                             rTests.get_test().is_integral() ? 0 : 1, GetVarianceMethod(), true);
            }
        else if (rTests.is_included() && rTests.get_test().get_test_type() ==
                                             readability::readability_test_type::index_value)
            {
            SetScoreStatsRow(m_scoreStatsData, rTests.get_test().get_long_name().c_str(),
                             wxString{}, currentRow++,
                             rTests.get_index_point_collection()
                                 ->GetContinuousColumn(GetScoreColumnName())
                                 ->GetValues(),
                             rTests.get_test().is_integral() ? 0 : 1, GetVarianceMethod(), false);
            }
        else if (rTests.is_included() &&
                 rTests.get_test().get_test_type() ==
                     readability::readability_test_type::predicted_cloze_score)
            {
            SetScoreStatsRow(m_scoreStatsData, rTests.get_test().get_long_name().c_str(),
                             wxString{}, currentRow++,
                             rTests.get_cloze_point_collection()
                                 ->GetContinuousColumn(GetScoreColumnName())
                                 ->GetValues(),
                             0, GetVarianceMethod(), false);
            }
        else if (rTests.is_included() &&
                 rTests.get_test().get_test_type() ==
                     readability::readability_test_type::index_value_and_grade_level)
            {
            SetScoreStatsRow(
                m_scoreStatsData,
                BatchProjectView::FormatIndexValuesLabel(rTests.get_test().get_long_name().c_str()),
                wxString{}, currentRow++,
                rTests.get_index_point_collection()
                    ->GetContinuousColumn(GetScoreColumnName())
                    ->GetValues(),
                rTests.get_test().is_integral() ? 0 : 1, GetVarianceMethod(), false);
            SetScoreStatsRow(
                m_scoreStatsData,
                BatchProjectView::FormatGradeLevelsLabel(rTests.get_test().get_long_name().c_str()),
                wxString{}, currentRow++,
                rTests.get_grade_point_collection()
                    ->GetContinuousColumn(GetScoreColumnName())
                    ->GetValues(),
                rTests.get_test().is_integral() ? 0 : 1, GetVarianceMethod(), true);
            }
        else if (rTests.is_included() &&
                 rTests.get_test().get_test_type() ==
                     readability::readability_test_type::grade_level_and_predicted_cloze_score)
            {
            SetScoreStatsRow(
                m_scoreStatsData,
                BatchProjectView::FormatGradeLevelsLabel(rTests.get_test().get_long_name().c_str()),
                wxString{}, currentRow++,
                rTests.get_grade_point_collection()
                    ->GetContinuousColumn(GetScoreColumnName())
                    ->GetValues(),
                rTests.get_test().is_integral() ? 0 : 1, GetVarianceMethod(), true);
            SetScoreStatsRow(
                m_scoreStatsData,
                BatchProjectView::FormatClozeValuesLabel(rTests.get_test().get_long_name().c_str()),
                wxString{}, currentRow++,
                rTests.get_cloze_point_collection()
                    ->GetContinuousColumn(GetScoreColumnName())
                    ->GetValues(),
                0, GetVarianceMethod(), false);
            }
        }
    if (IsIncludingDolchSightWords())
        {
        m_scoreStatsData->SetItemText(
            currentRow, 0, ReadabilityMessages::GetDolchLabel(),
            Wisteria::NumberFormatInfo{
                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
            std::numeric_limits<double>::quiet_NaN());
        for (size_t j = 1; j < m_scoreStatsData->GetColumnCount(); ++j)
            {
            m_scoreStatsData->SetItemText(
                currentRow, j, _(L"N/A"),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        ++currentRow;
        }
    // Summarize the custom tests' scores
    for (auto testVectorsPos = m_customTestScores.cbegin();
         testVectorsPos != m_customTestScores.cend(); ++testVectorsPos)
        {
        SetScoreStatsRow(
            m_scoreStatsData,
            GetCustTestsInUse()[(testVectorsPos - m_customTestScores.begin())].GetTestName(),
            wxString{}, currentRow++,
            (*testVectorsPos)->GetContinuousColumn(GetScoreColumnName())->GetValues(), 1,
            GetVarianceMethod(),

            (GetCustTestsInUse()[(testVectorsPos - m_customTestScores.begin())]
                 .GetIterator()
                 ->get_test_type() == readability::readability_test_type::grade_level));
        }
    m_scoreStatsData->SetSize(currentRow);

    // summarize the documents' aggregated grade scores
    m_aggregatedGradeScoresData->SetNumberFormatter(GetReadabilityMessageCatalogPtr());
    m_aggregatedGradeScoresData->DeleteAllItems();
    m_aggregatedGradeScoresData->SetSize(m_docs.size(),
                                         CUMULATIVE_STATS_COUNT + 2 /*doc and description*/);
    currentRow = 0;
    for (auto& doc : m_docs)
        {
        if (doc->LoadingOriginalTextSucceeded())
            {
            SetScoreStatsRow(m_aggregatedGradeScoresData, doc->GetOriginalDocumentFilePath(),
                             // a bit of a hack--need to pass in something to force the use of
                             // description column.
                             !doc->GetOriginalDocumentDescription().empty() ?
                                 doc->GetOriginalDocumentDescription() :
                                 wxString{ L"  " },
                             currentRow++, doc->GetAggregatedGradeScores(), 1, GetVarianceMethod(),
                             true);
            }
        }
    m_aggregatedGradeScoresData->SetSize(currentRow);

    // summarize the documents' aggregated cloze scores
    m_aggregatedClozeScoresData->SetNumberFormatter(GetReadabilityMessageCatalogPtr());
    m_aggregatedClozeScoresData->DeleteAllItems();
    m_aggregatedClozeScoresData->SetSize(m_docs.size(),
                                         CUMULATIVE_STATS_COUNT + 2 /*doc and description*/);
    currentRow = 0;
    for (auto& doc : m_docs)
        {
        if (doc->LoadingOriginalTextSucceeded())
            {
            SetScoreStatsRow(m_aggregatedClozeScoresData, doc->GetOriginalDocumentFilePath(),
                             (!doc->GetOriginalDocumentDescription().empty()) ?
                                 doc->GetOriginalDocumentDescription() :
                                 wxString{ L"  " },
                             currentRow++, doc->GetAggregatedClozeScores(), 1, GetVarianceMethod(),
                             false);
            }
        }
    m_aggregatedClozeScoresData->SetSize(currentRow);
    }

//------------------------------------------------------------
void BatchProjectDoc::DisplayWarnings()
    {
    auto* view = dynamic_cast<BatchProjectView*>(GetFirstView());

    // initialize the warnings listctrl if it doesn't have any columns in it yet
    if (view->GetWarningsView()->GetColumnCount() == 0)
        {
        view->GetWarningsView()->InsertColumn(0, _(L"Document"));
        view->GetWarningsView()->InsertColumn(1, _(L"Label"));
        view->GetWarningsView()->InsertColumn(2, _(L"Warning"));
        view->GetWarningsView()->AssignContextMenu(
            wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
        }

    view->GetWarningsView()->SetName(BaseProjectView::GetWarningLabel());
    view->GetWarningsView()->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
    view->GetWarningsView()->EnableGridLines();
    view->GetWarningsView()->EnableItemViewOnDblClick();
    view->GetWarningsView()->SetVirtualDataProvider(m_warnings);
    view->GetWarningsView()->SetVirtualDataSize(m_warnings->GetItemCount());
    if (view->GetWarningsView()->GetSortedColumn() == -1)
        {
        view->GetWarningsView()->SortColumn(0, Wisteria::SortDirection::SortAscending);
        }
    else
        {
        view->GetWarningsView()->Resort();
        }
    UpdateListOptions(view->GetWarningsView());
    if (m_warnings->GetItemCount() > 0)
        {
        view->GetWarningsView()->SetColumnWidth(0, view->GetWarningsView()->EstimateColumnWidth(0));
        view->GetWarningsView()->SetColumnWidth(
            1,
            std::min(view->GetWarningsView()->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        view->GetWarningsView()->SetColumnWidth(2, view->GetWarningsView()->EstimateColumnWidth(2));
        }
    view->GetWarningsView()->Refresh();
    }

//------------------------------------------------------------
void BatchProjectDoc::DisplayScores()
    {
    PROFILE();
    auto* view = dynamic_cast<BatchProjectView*>(GetFirstView());

        // main scores grid
        {
        auto* listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
            view->GetScoresView().FindWindowById(BaseProjectView::ID_SCORE_LIST_PAGE_ID));
        if (listView == nullptr)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::ID_SCORE_LIST_PAGE_ID, wxDefaultPosition,
                wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetRawScoresTabLabel());
            listView->SetName(BaseProjectView::GetRawScoresTabLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
            view->GetScoresView().AddWindow(listView);
            }
        listView->DeleteAllColumns();
        listView->InsertColumn(0, _(L"Document"));
        listView->InsertColumn(1, _(L"Label"));
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        // Note, the ordering of these columns must match the ordering in LoadScores()
        // add columns for the included standard tests
        for (auto& rTests : GetReadabilityTests().get_tests())
            {
            if (rTests.is_included())
                {
                if (rTests.get_test().get_test_type() ==
                        readability::readability_test_type::grade_level ||
                    rTests.get_test().get_test_type() ==
                        readability::readability_test_type::index_value ||
                    rTests.get_test().get_test_type() ==
                        readability::readability_test_type::predicted_cloze_score)
                    {
                    listView->InsertColumn(listView->GetColumnCount(),
                                           rTests.get_test().get_short_name().c_str());
                    }
                else if (rTests.get_test().get_test_type() ==
                         readability::readability_test_type::index_value_and_grade_level)
                    {
                    listView->InsertColumn(listView->GetColumnCount(),
                                           BatchProjectView::FormatIndexValuesLabel(
                                               rTests.get_test().get_short_name().c_str()));
                    listView->InsertColumn(listView->GetColumnCount(),
                                           BatchProjectView::FormatGradeLevelsLabel(
                                               rTests.get_test().get_short_name().c_str()));
                    }
                else if (rTests.get_test().get_test_type() ==
                         readability::readability_test_type::grade_level_and_predicted_cloze_score)
                    {
                    listView->InsertColumn(listView->GetColumnCount(),
                                           BatchProjectView::FormatGradeLevelsLabel(
                                               rTests.get_test().get_short_name().c_str()));
                    listView->InsertColumn(listView->GetColumnCount(),
                                           BatchProjectView::FormatClozeValuesLabel(
                                               rTests.get_test().get_short_name().c_str()));
                    }
                }
            }

        for (const auto& testPos : GetCustTestsInUse())
            {
            listView->InsertColumn(listView->GetColumnCount(), testPos.GetTestName());
            }

        listView->SetVirtualDataProvider(m_scoreRawData);
        listView->SetVirtualDataSize(m_scoreRawData->GetItemCount());
        if (listView->GetSortedColumn() == -1)
            {
            listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
            }
        else
            {
            listView->Resort();
            }
        UpdateListOptions(listView);
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        for (long i = 1; i < listView->GetColumnCount(); ++i)
            {
            listView->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
            // sometimes an error description might make a column too wide
            if (listView->GetColumnWidth(i) > view->GetMaxColumnWidth())
                {
                listView->SetColumnWidth(i, view->GetMaxColumnWidth());
                }
            else if (listView->GetColumnWidth(i) < view->GetMaxColumnWidth() / 2)
                {
                listView->SetColumnWidth(i, view->GetMaxColumnWidth() / 2);
                }
            }
        }

    // add/remove the goals
    if (!GetTestGoals().empty() || !GetStatGoals().empty())
        {
        auto* goalsList = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
            view->GetScoresView().FindWindowById(BaseProjectView::READABILITY_GOALS_PAGE_ID));
        if (goalsList == nullptr)
            {
            goalsList = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::READABILITY_GOALS_PAGE_ID, wxDefaultPosition,
                wxDefaultSize, wxLC_REPORT | wxLC_VIRTUAL | wxBORDER_SUNKEN);
            goalsList->Hide();
            goalsList->SetLabel(_(L"Goals"));
            goalsList->SetName(_(L"Goals"));
            goalsList->SetSortable(true);
            goalsList->EnableItemViewOnDblClick();
            goalsList->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
            goalsList->SetVirtualDataProvider(m_goalsData);
            UpdateListOptions(goalsList);
            view->GetScoresView().InsertWindow(1, goalsList);
            }
        goalsList->DeleteAllColumns();
        // add columns for all the goals x document
        goalsList->InsertColumn(0, _(L"Document"));
        goalsList->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        goalsList->InsertColumn(1, _(L"Label"));
        for (const auto& goal : GetTestGoals())
            {
            if (!goal.HasGoals())
                {
                continue;
                }
            auto [sTest, found] = GetReadabilityTests().find_test(goal.GetName().c_str());
            const wxString testName =
                found ? sTest->get_test().get_long_name().c_str() : goal.GetName().c_str();

            // only include min or max goal columns if there is an actual goal for it.
            if (!std::isnan(goal.GetMinGoal()))
                {
                goalsList->InsertColumn(goalsList->GetColumnCount(),
                                        wxString::Format(
                                            // TRANSLATORS: %s is a test name
                                            _(L"%s Min"), testName));
                }
            if (!std::isnan(goal.GetMaxGoal()))
                {
                goalsList->InsertColumn(goalsList->GetColumnCount(),
                                        wxString::Format(
                                            // TRANSLATORS: %s is a test name
                                            _(L"%s Max"), testName));
                }
            }
        for (const auto& goal : GetStatGoals())
            {
            if (!goal.HasGoals())
                {
                continue;
                }
            const auto statGoalLabel = GetStatGoalLabels().find({ goal.GetName(), goal.GetName() });
            const wxString goalName = (statGoalLabel != GetStatGoalLabels().cend()) ?
                                          statGoalLabel->first.second.c_str() :
                                          goal.GetName().c_str();

            // only include min or max goal columns if there is an actual goal for it.
            if (!std::isnan(goal.GetMinGoal()))
                {
                goalsList->InsertColumn(goalsList->GetColumnCount(),
                                        wxString::Format(
                                            // TRANSLATORS: %s is goal name
                                            _(L"%s Min"), goalName));
                }
            if (!std::isnan(goal.GetMaxGoal()))
                {
                goalsList->InsertColumn(goalsList->GetColumnCount(),
                                        wxString::Format(
                                            // TRANSLATORS: %s is goal name
                                            _(L"%s Max"), goalName));
                }
            }
        m_goalsData->DeleteAllItems();
        m_goalsData->SetSize(m_docs.size(), goalsList->GetColumnCount());

        size_t i = 0;
        for (const auto& doc : m_docs)
            {
            if (!doc->LoadingOriginalTextSucceeded())
                {
                continue;
                }
            bool includeDoc{ false };
            // If at least one failing goal, then include the doc in the results;
            // otherwise, leave it out.
            // Because there can be so many documents in a batch,
            // it's better to only show ones that are failing
            // to make finding issues easier.
            for (const auto& goal : doc->GetTestGoals())
                {
                if (goal.HasGoals() && !goal.GetPassFailFlags().all())
                    {
                    includeDoc = true;
                    break;
                    }
                }
            for (const auto& goal : doc->GetStatGoals())
                {
                if (goal.HasGoals() && !goal.GetPassFailFlags().all())
                    {
                    includeDoc = true;
                    break;
                    }
                }
            if (!includeDoc)
                {
                continue;
                }
            long currentColumn = 0;
            goalsList->SetItemText(i, currentColumn++, doc->GetOriginalDocumentFilePath());
            goalsList->SetItemText(i, currentColumn++, doc->GetOriginalDocumentDescription());
            for (const auto& goal : doc->GetTestGoals())
                {
                if (!goal.HasGoals())
                    {
                    continue;
                    }
                // fill in pass/fail
                // (if min or max goal is unspecified, then skip it because the
                //  column won't be there)
                if (!std::isnan(goal.GetMinGoal()))
                    {
                    goalsList->SetItemText(
                        i, currentColumn++,
                        wxString::Format(L"%s %s",
                                         (goal.GetPassFailFlags()[0] ?
                                              BaseProjectView::GetCheckmarkEmoji() :
                                              BaseProjectView::GetWarningEmoji()),
                                         wxNumberFormatter::ToString(
                                             goal.GetMinGoal(), 1,
                                             wxNumberFormatter::Style::Style_NoTrailingZeroes)));
                    }
                if (!std::isnan(goal.GetMaxGoal()))
                    {
                    goalsList->SetItemText(
                        i, currentColumn++,
                        wxString::Format(L"%s %s",
                                         (goal.GetPassFailFlags()[1] ?
                                              BaseProjectView::GetCheckmarkEmoji() :
                                              BaseProjectView::GetWarningEmoji()),
                                         wxNumberFormatter::ToString(
                                             goal.GetMaxGoal(), 1,
                                             wxNumberFormatter::Style::Style_NoTrailingZeroes)));
                    }
                }
            for (const auto& goal : doc->GetStatGoals())
                {
                if (!goal.HasGoals())
                    {
                    continue;
                    }
                // fill in pass/fail
                // (if min or max goal is unspecified, then skip it because the
                //  column won't be there)
                if (!std::isnan(goal.GetMinGoal()))
                    {
                    goalsList->SetItemText(
                        i, currentColumn++,
                        wxString::Format(L"%s %s",
                                         (goal.GetPassFailFlags()[0] ?
                                              BaseProjectView::GetCheckmarkEmoji() :
                                              BaseProjectView::GetWarningEmoji()),
                                         wxNumberFormatter::ToString(
                                             goal.GetMinGoal(), 1,
                                             wxNumberFormatter::Style::Style_NoTrailingZeroes)));
                    }
                if (!std::isnan(goal.GetMaxGoal()))
                    {
                    goalsList->SetItemText(
                        i, currentColumn++,
                        wxString::Format(L"%s %s",
                                         (goal.GetPassFailFlags()[1] ?
                                              BaseProjectView::GetCheckmarkEmoji() :
                                              BaseProjectView::GetWarningEmoji()),
                                         wxNumberFormatter::ToString(
                                             goal.GetMaxGoal(), 1,
                                             wxNumberFormatter::Style::Style_NoTrailingZeroes)));
                    }
                }
            ++i;
            }
        goalsList->SetVirtualDataSize(i);
        goalsList->DistributeColumns();
        // everything is passing, so nothing to show
        if (i == 0)
            {
            goalsList->DeleteAllColumns();
            goalsList->InsertColumn(0, _(L"Status"));
            goalsList->SetVirtualDataSize(1);
            goalsList->SetItemText(0, 0, _(L"All documents passing."));
            goalsList->SetVirtualDataSize(1);
            goalsList->DistributeColumns();
            }
        }
    else
        {
        // we are getting rid of this window (if nothing in it)
        view->GetScoresView().RemoveWindowById(BaseProjectView::READABILITY_GOALS_PAGE_ID);
        }

    DisplayScoreStatisticsWindow(_(L"Score Summary"), BaseProjectView::ID_SCORE_STATS_LIST_PAGE_ID,
                                 m_scoreStatsData, _(L"Test"), wxString{}, false);
    // aggregated grade level scores, listed by document
    if (IsIncludingGradeTest())
        {
        // TRANSLATORS: "x" means "by".
        DisplayScoreStatisticsWindow(_(L"Grade Score Summary (x Document)"),
                                     BaseProjectView::ID_AGGREGATED_DOC_SCORES_LIST_PAGE_ID,
                                     m_aggregatedGradeScoresData, _(L"Document"), _(L"Label"),
                                     true);
        }
    else
        {
        view->GetScoresView().RemoveWindowById(
            BaseProjectView::ID_AGGREGATED_DOC_SCORES_LIST_PAGE_ID);
        }

    // aggregated predicted cloze scores, listed by document
    if (IsIncludingClozeTest())
        {
        // TRANSLATORS: "x" means "by".
        DisplayScoreStatisticsWindow(_(L"Cloze Score Summary (x Document)"),
                                     BaseProjectView::ID_AGGREGATED_CLOZE_SCORES_LIST_PAGE_ID,
                                     m_aggregatedClozeScoresData, _(L"Document"), _(L"Label"),
                                     true);
        }
    else
        {
        view->GetScoresView().RemoveWindowById(
            BaseProjectView::ID_AGGREGATED_CLOZE_SCORES_LIST_PAGE_ID);
        }

    // add these here so that they are ordered after the aggregated stats
    if ((view->GetCrawfordGraph() != nullptr) &&
        GetReadabilityTests().is_test_included(ReadabilityMessages::CRAWFORD()) &&
        !GetDocuments().empty())
        {
        view->GetScoresView().AddWindow(view->GetCrawfordGraph());
        }
    if ((view->GetInfleszGraph() != nullptr) &&
        GetReadabilityTests().is_test_included(ReadabilityMessages::INFLESZ()) &&
        !GetDocuments().empty())
        {
        view->GetScoresView().AddWindow(view->GetInfleszGraph());
        }
    if ((view->GetFleschChart() != nullptr) &&
        GetReadabilityTests().is_test_included(ReadabilityMessages::FLESCH()) &&
        !GetDocuments().empty())
        {
        view->GetScoresView().AddWindow(view->GetFleschChart());
        }
    if ((view->GetDB2Plot() != nullptr) &&
        GetReadabilityTests().is_test_included(ReadabilityMessages::DANIELSON_BRYAN_2()) &&
        !GetDocuments().empty())
        {
        view->GetScoresView().AddWindow(view->GetDB2Plot());
        }
    if ((view->GetFryGraph() != nullptr) &&
        GetReadabilityTests().is_test_included(ReadabilityMessages::FRY()) &&
        !GetDocuments().empty())
        {
        view->GetScoresView().AddWindow(view->GetFryGraph());
        }
    if ((view->GetGpmFryGraph() != nullptr) &&
        GetReadabilityTests().is_test_included(ReadabilityMessages::GPM_FRY()) &&
        !GetDocuments().empty())
        {
        view->GetScoresView().AddWindow(view->GetGpmFryGraph());
        }
    if ((view->GetFraseGraph() != nullptr) &&
        GetReadabilityTests().is_test_included(ReadabilityMessages::FRASE()) &&
        !GetDocuments().empty())
        {
        view->GetScoresView().AddWindow(view->GetFraseGraph());
        }
    if ((view->GetSchwartzGraph() != nullptr) &&
        GetReadabilityTests().is_test_included(ReadabilityMessages::SCHWARTZ()) &&
        !GetDocuments().empty())
        {
        view->GetScoresView().AddWindow(view->GetSchwartzGraph());
        }
    if ((view->GetLixGauge() != nullptr) &&
        GetReadabilityTests().is_test_included(ReadabilityMessages::LIX()) &&
        !GetDocuments().empty())
        {
        view->GetScoresView().AddWindow(view->GetLixGauge());
        }
    if ((view->GetGermanLixGauge() != nullptr) &&
        (GetReadabilityTests().is_test_included(
             ReadabilityMessages::LIX_GERMAN_CHILDRENS_LITERATURE()) ||
         GetReadabilityTests().is_test_included(ReadabilityMessages::LIX_GERMAN_TECHNICAL())) &&
        !GetDocuments().empty())
        {
        view->GetScoresView().AddWindow(view->GetGermanLixGauge());
        }
    if ((view->GetRaygorGraph() != nullptr) &&
        GetReadabilityTests().is_test_included(ReadabilityMessages::RAYGOR()) &&
        !GetDocuments().empty())
        {
        view->GetScoresView().AddWindow(view->GetRaygorGraph());
        }
    }

//------------------------------------------------------------
void BatchProjectDoc::DisplayScoreStatisticsWindow(
    const wxString& windowName, const int windowId,
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& data,
    const wxString& firstColumnName, const wxString& optionalSecondColumnName,
    const bool multiSelectable)
    {
    auto* view = dynamic_cast<BatchProjectView*>(GetFirstView());
    auto* listView =
        dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetScoresView().FindWindowById(windowId));
    if (listView == nullptr)
        {
        long style = wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN;
        if (!multiSelectable)
            {
            style |= wxLC_SINGLE_SEL;
            }
        listView = new Wisteria::UI::ListCtrlEx(view->GetSplitter(), windowId, wxDefaultPosition,
                                                wxDefaultSize, style);
        listView->Hide();
        listView->SetLabel(windowName);
        listView->SetName(windowName);
        listView->EnableGridLines();
        listView->EnableItemViewOnDblClick();
        view->GetScoresView().AddWindow(listView);
        }
    listView->DeleteAllColumns();
    long currentColumnCount = 0;
    listView->InsertColumn(currentColumnCount++, firstColumnName);
    if (!optionalSecondColumnName.empty())
        {
        listView->InsertColumn(currentColumnCount++, optionalSecondColumnName);
        }
    listView->InsertColumn(currentColumnCount++, _(L"Valid N"));
    listView->InsertColumn(currentColumnCount++, _(L"Minimum"));
    listView->InsertColumn(currentColumnCount++, _(L"Maximum"));
    listView->InsertColumn(currentColumnCount++, _(L"Range"));
    listView->InsertColumn(currentColumnCount++, _(L"Modes"));
    listView->InsertColumn(currentColumnCount++, _(L"Means"));
    // if verbose, then add the extra columns
    if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
        {
        listView->InsertColumn(currentColumnCount++, _(L"Median"));
        listView->InsertColumn(currentColumnCount++, _(L"Skewness"));
        listView->InsertColumn(currentColumnCount++, _(L"Kurtosis"));
        listView->InsertColumn(currentColumnCount++, _(L"Std. Dev."));
        listView->InsertColumn(currentColumnCount++, _(L"Variance"));
        listView->InsertColumn(currentColumnCount++, _(L"Lower Quartile"));
        listView->InsertColumn(currentColumnCount++, _(L"Upper Quartile"));
        }
    listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
    listView->SetVirtualDataProvider(data);
    listView->SetVirtualDataSize(data->GetItemCount());
    UpdateListOptions(listView);
    listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
    listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
    listView->SetColumnWidth(1,
                             std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
    for (long i = 2; i < listView->GetColumnCount(); ++i)
        {
        listView->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
        }
    if (listView->GetSortedColumn() == -1)
        {
        listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
        }
    else
        {
        listView->Resort();
        }
    }

//------------------------------------------------------------
void BatchProjectDoc::DisplayInfleszGraph()
    {
    auto* view = dynamic_cast<BatchProjectView*>(GetFirstView());

    const wxString scoresColumnName{ _DT(L"SCORES") };
    const wxString groupColumnName{ _DT(L"GROUP") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(scoresColumnName);
    scoreDataset->AddCategoricalColumn(groupColumnName, m_groupStringTable);
    scoreDataset->GetIdColumn().SetName(_DT(L"DOCS"));
    scoreDataset->Reserve(GetDocuments().size());

    for (auto* doc : GetDocuments())
        {
        if (doc->LoadingOriginalTextSucceeded())
            {
            const double indexValue = readability::szigriszt_pazos_perspicuity(
                doc->GetTotalWords(), doc->GetTotalSyllables(), doc->GetTotalSentences());

            auto foundGroupId =
                GetDocumentLabels().find(doc->GetOriginalDocumentDescription().wc_str());
            wxASSERT_MSG((!IsShowingGroupLegends() || foundGroupId != GetDocumentLabels().cend()),
                         L"Could not find group label for INFLESZ Scale!");
            scoreDataset->AddRow(
                Wisteria::Data::RowInfo()
                    .Id(wxFileName(doc->GetOriginalDocumentFilePath()).GetFullName().wc_str())
                    .Categoricals({ IsShowingGroupLegends() ? foundGroupId->second : 0 })
                    .Continuous({ indexValue }));
            }
        }

    // INFLESZ Scale
    if (GetReadabilityTests().is_test_included(ReadabilityMessages::INFLESZ()) && !m_docs.empty())
        {
        std::shared_ptr<Wisteria::Graphs::InfleszScale> infleszGraph{ nullptr };
        auto* infleszGraphCanvas = dynamic_cast<Wisteria::Canvas*>(
            view->GetScoresView().FindWindowById(BaseProjectView::INFLESZ_GRAPH_PAGE_ID));

        if (infleszGraphCanvas == nullptr)
            {
            infleszGraphCanvas =
                new Wisteria::Canvas(view->GetSplitter(), BaseProjectView::INFLESZ_GRAPH_PAGE_ID);
            infleszGraphCanvas->SetFixedObjectsGridSize(1, 1);

            infleszGraph = std::make_shared<Wisteria::Graphs::InfleszScale>(
                infleszGraphCanvas,
                std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                    *std::make_shared<Wisteria::Colors::Schemes::EarthTones>()));
            infleszGraph->SetData(scoreDataset, scoresColumnName,
                                  IsShowingGroupLegends() ?
                                      std::optional<const wxString>(groupColumnName) :
                                      std::nullopt);
            infleszGraphCanvas->SetFixedObject(0, 0, infleszGraph);
            infleszGraphCanvas->Hide();
            view->SetInfleszGraph(infleszGraphCanvas);
            view->GetInfleszGraph()->SetLabel(
                GetReadabilityTests().get_test_long_name(ReadabilityMessages::INFLESZ()).c_str());
            view->GetInfleszGraph()->SetName(
                GetReadabilityTests().get_test_long_name(ReadabilityMessages::INFLESZ()).c_str());
            }
        else
            {
            infleszGraph = std::dynamic_pointer_cast<Wisteria::Graphs::InfleszScale>(
                view->GetInfleszGraph()->GetFixedObject(0, 0));
            assert(infleszGraph);
            infleszGraph->SetData(scoreDataset, scoresColumnName,
                                  IsShowingGroupLegends() ?
                                      std::optional<const wxString>(groupColumnName) :
                                      std::nullopt);
            }
        UpdateGraphOptions(view->GetInfleszGraph());

        // add legend if grouping
        if (IsShowingGroupLegends())
            {
            view->GetInfleszGraph()->SetFixedObjectsGridSize(1, 2);
            view->GetInfleszGraph()->SetFixedObject(
                0, 1,
                infleszGraph->CreateLegend(Wisteria::Graphs::LegendOptions().PlacementHint(
                    Wisteria::LegendCanvasPlacementHint::RightOfGraph)));
            }
        // ...and if not grouping, use light blue for points
        else
            {
            infleszGraph->SetColorScheme(std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                Wisteria::Colors::Schemes::ColorScheme{ Wisteria::Colors::ColorBrewer::GetColor(
                    Wisteria::Colors::Color::CelestialBlue) }));
            }

        wxGCDC gdc(view->GetDocFrame());
        view->GetInfleszGraph()->CalcAllSizes(gdc);
        }
    else
        {
        view->GetScoresView().RemoveWindowById(BaseProjectView::INFLESZ_GRAPH_PAGE_ID);
        }
    }

//------------------------------------------------------------
void BatchProjectDoc::DisplayCrawfordGraph()
    {
    auto* view = dynamic_cast<BatchProjectView*>(GetFirstView());

    const wxString scoresColumnName{ _DT(L"SCORES") };
    const wxString syllablesColumnName{ _DT(L"SYLLABLES") };
    const wxString groupColumnName{ _DT(L"GROUP") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(scoresColumnName);
    scoreDataset->AddContinuousColumn(syllablesColumnName);
    scoreDataset->AddCategoricalColumn(groupColumnName, m_groupStringTable);
    scoreDataset->GetIdColumn().SetName(_DT(L"DOCS"));
    scoreDataset->Reserve(GetDocuments().size());

    for (auto* doc : GetDocuments())
        {
        if (doc->LoadingOriginalTextSucceeded())
            {
            const double gradeValue = readability::crawford(
                doc->GetTotalWords(), doc->GetTotalSyllables(), doc->GetTotalSentences());
            const double syllablesPer100Words =
                doc->GetTotalSyllables() * (safe_divide<double>(100, doc->GetTotalWords()));

            auto foundGroupId =
                GetDocumentLabels().find(doc->GetOriginalDocumentDescription().wc_str());
            wxASSERT_MSG((!IsShowingGroupLegends() || foundGroupId != GetDocumentLabels().cend()),
                         L"Could not find group label for Crawford graph!");
            scoreDataset->AddRow(
                Wisteria::Data::RowInfo()
                    .Id(wxFileName(doc->GetOriginalDocumentFilePath()).GetFullName().wc_str())
                    .Categoricals({ IsShowingGroupLegends() ? foundGroupId->second : 0 })
                    .Continuous({ gradeValue, syllablesPer100Words }));
            }
        }

    // Crawford Graph
    if (GetReadabilityTests().is_test_included(ReadabilityMessages::CRAWFORD()) && !m_docs.empty())
        {
        std::shared_ptr<Wisteria::Graphs::CrawfordGraph> crawfordGraph{ nullptr };
        auto* crawfordGraphCanvas = dynamic_cast<Wisteria::Canvas*>(
            view->GetScoresView().FindWindowById(BaseProjectView::CRAWFORD_GRAPH_PAGE_ID));

        if (crawfordGraphCanvas == nullptr)
            {
            crawfordGraphCanvas =
                new Wisteria::Canvas(view->GetSplitter(), BaseProjectView::CRAWFORD_GRAPH_PAGE_ID);
            crawfordGraphCanvas->SetFixedObjectsGridSize(1, 1);

            crawfordGraph = std::make_shared<Wisteria::Graphs::CrawfordGraph>(
                crawfordGraphCanvas,
                std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                    *std::make_shared<Wisteria::Colors::Schemes::EarthTones>()));
            crawfordGraph->SetData(scoreDataset, scoresColumnName, syllablesColumnName,
                                   IsShowingGroupLegends() ?
                                       std::optional<const wxString>(groupColumnName) :
                                       std::nullopt);
            crawfordGraphCanvas->SetFixedObject(0, 0, crawfordGraph);
            crawfordGraphCanvas->Hide();
            view->SetCrawfordGraph(crawfordGraphCanvas);
            view->GetCrawfordGraph()->SetLabel(
                GetReadabilityTests().get_test_long_name(ReadabilityMessages::CRAWFORD()).c_str());
            view->GetCrawfordGraph()->SetName(
                GetReadabilityTests().get_test_long_name(ReadabilityMessages::CRAWFORD()).c_str());
            }
        else
            {
            crawfordGraph = std::dynamic_pointer_cast<Wisteria::Graphs::CrawfordGraph>(
                view->GetCrawfordGraph()->GetFixedObject(0, 0));
            assert(crawfordGraph);
            crawfordGraph->SetData(scoreDataset, scoresColumnName, syllablesColumnName,
                                   IsShowingGroupLegends() ?
                                       std::optional<const wxString>(groupColumnName) :
                                       std::nullopt);
            }
        UpdateGraphOptions(view->GetCrawfordGraph());

        // add legend if grouping
        if (IsShowingGroupLegends())
            {
            view->GetCrawfordGraph()->SetFixedObjectsGridSize(1, 2);
            view->GetCrawfordGraph()->SetFixedObject(
                0, 1,
                crawfordGraph->CreateLegend(Wisteria::Graphs::LegendOptions().PlacementHint(
                    Wisteria::LegendCanvasPlacementHint::RightOfGraph)));
            }
        // ...and if not grouping, use light blue for points
        else
            {
            crawfordGraph->SetColorScheme(std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                Wisteria::Colors::Schemes::ColorScheme{ Wisteria::Colors::ColorBrewer::GetColor(
                    Wisteria::Colors::Color::CelestialBlue) }));
            }

        wxGCDC gdc(view->GetDocFrame());
        view->GetCrawfordGraph()->CalcAllSizes(gdc);
        }
    else
        {
        view->GetScoresView().RemoveWindowById(BaseProjectView::CRAWFORD_GRAPH_PAGE_ID);
        }
    }

//------------------------------------------------------------
void BatchProjectDoc::DisplayDB2Plot()
    {
    auto* view = dynamic_cast<BatchProjectView*>(GetFirstView());

    const wxString scoresColumnName{ _DT(L"SCORES") };
    const wxString groupColumnName{ _DT(L"GROUP") };

    const auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(scoresColumnName);
    scoreDataset->AddCategoricalColumn(groupColumnName, m_groupStringTable);
    scoreDataset->GetIdColumn().SetName(_DT(L"DOCS"));
    scoreDataset->Reserve(GetDocuments().size());

    for (auto* doc : GetDocuments())
        {
        if (doc->LoadingOriginalTextSucceeded())
            {
            const auto score = readability::danielson_bryan_2(
                doc->GetTotalWords(), doc->GetTotalCharactersPlusPunctuation(),
                doc->GetTotalSentences());

            auto foundGroupId =
                GetDocumentLabels().find(doc->GetOriginalDocumentDescription().wc_str());
            wxASSERT_MSG((!IsShowingGroupLegends() || foundGroupId != GetDocumentLabels().cend()),
                         L"Could not find group label for DB Plot!");
            scoreDataset->AddRow(
                Wisteria::Data::RowInfo()
                    .Id(wxFileName(doc->GetOriginalDocumentFilePath()).GetFullName().wc_str())
                    .Categoricals({ IsShowingGroupLegends() ? foundGroupId->second : 0 })
                    .Continuous({ score }));
            }
        }

    // DB2
    if (GetReadabilityTests().is_test_included(ReadabilityMessages::DANIELSON_BRYAN_2()) &&
        !m_docs.empty())
        {
        std::shared_ptr<Wisteria::Graphs::DanielsonBryan2Plot> db2Plot{ nullptr };
        auto* db2PlotCanvas = dynamic_cast<Wisteria::Canvas*>(
            view->GetScoresView().FindWindowById(BaseProjectView::DB2_PAGE_ID));

        if (db2PlotCanvas == nullptr)
            {
            db2PlotCanvas = new Wisteria::Canvas(view->GetSplitter(), BaseProjectView::DB2_PAGE_ID);
            db2PlotCanvas->SetFixedObjectsGridSize(1, 1);

            db2Plot = std::make_shared<Wisteria::Graphs::DanielsonBryan2Plot>(
                db2PlotCanvas, std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                                   *std::make_shared<Wisteria::Colors::Schemes::EarthTones>()));
            db2Plot->SetData(scoreDataset, scoresColumnName,
                             IsShowingGroupLegends() ?
                                 std::optional<const wxString>(groupColumnName) :
                                 std::nullopt);
            db2PlotCanvas->SetFixedObject(0, 0, db2Plot);
            db2PlotCanvas->Hide();
            view->SetDB2Plot(db2PlotCanvas);
            view->GetDB2Plot()->SetLabel(
                GetReadabilityTests()
                    .get_test_long_name(ReadabilityMessages::DANIELSON_BRYAN_2())
                    .c_str());
            view->GetDB2Plot()->SetName(
                GetReadabilityTests()
                    .get_test_long_name(ReadabilityMessages::DANIELSON_BRYAN_2())
                    .c_str());
            }
        else
            {
            db2Plot = std::dynamic_pointer_cast<Wisteria::Graphs::DanielsonBryan2Plot>(
                view->GetDB2Plot()->GetFixedObject(0, 0));
            assert(db2Plot);
            db2Plot->SetData(scoreDataset, scoresColumnName,
                             IsShowingGroupLegends() ?
                                 std::optional<const wxString>(groupColumnName) :
                                 std::nullopt);
            }
        UpdateGraphOptions(view->GetDB2Plot());

        db2Plot->ShowcaseScore(IsShowcasingKeyItems());

        // add legend if grouping
        if (IsShowingGroupLegends())
            {
            view->GetDB2Plot()->SetFixedObjectsGridSize(1, 2);
            view->GetDB2Plot()->SetFixedObject(
                0, 1,
                db2Plot->CreateLegend(Wisteria::Graphs::LegendOptions().PlacementHint(
                    Wisteria::LegendCanvasPlacementHint::RightOfGraph)));
            }
        // ...and if not grouping, use light blue for points
        else
            {
            db2Plot->SetColorScheme(std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                Wisteria::Colors::Schemes::ColorScheme{ Wisteria::Colors::ColorBrewer::GetColor(
                    Wisteria::Colors::Color::CelestialBlue) }));
            }

        wxGCDC gdc(view->GetDocFrame());
        view->GetDB2Plot()->CalcAllSizes(gdc);
        }
    else
        {
        view->GetScoresView().RemoveWindowById(BaseProjectView::DB2_PAGE_ID);
        }
    }

//------------------------------------------------------------
void BatchProjectDoc::DisplayFleschChart()
    {
    auto* view = dynamic_cast<BatchProjectView*>(GetFirstView());

    const wxString wordsColumnName{ _DT(L"WORDS") };
    const wxString scoresColumnName{ _DT(L"SCORES") };
    const wxString syllablesColumnName{ _DT(L"SYLLABLES") };
    const wxString groupColumnName{ _DT(L"GROUP") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(wordsColumnName);
    scoreDataset->AddContinuousColumn(scoresColumnName);
    scoreDataset->AddContinuousColumn(syllablesColumnName);
    scoreDataset->AddCategoricalColumn(groupColumnName, m_groupStringTable);
    scoreDataset->GetIdColumn().SetName(_DT(L"DOCS"));
    scoreDataset->Reserve(GetDocuments().size());

    for (auto* doc : GetDocuments())
        {
        if (doc->LoadingOriginalTextSucceeded())
            {
            const auto asl = safe_divide<double>(doc->GetTotalWords(), doc->GetTotalSentences());
            const auto asw = safe_divide<double>((doc->GetFleschNumeralSyllabizeMethod() ==
                                                  FleschNumeralSyllabize::NumeralIsOneSyllable) ?
                                                     doc->GetTotalSyllablesNumeralsOneSyllable() :
                                                     doc->GetTotalSyllables(),
                                                 doc->GetTotalWords());
            readability::flesch_difficulty diffLevel{};
            const size_t score =
                readability::flesch_reading_ease(doc->GetTotalWords(),
                                                 (doc->GetFleschNumeralSyllabizeMethod() ==
                                                  FleschNumeralSyllabize::NumeralIsOneSyllable) ?
                                                     doc->GetTotalSyllablesNumeralsOneSyllable() :
                                                     doc->GetTotalSyllables(),
                                                 doc->GetTotalSentences(), diffLevel);

            auto foundGroupId =
                GetDocumentLabels().find(doc->GetOriginalDocumentDescription().wc_str());
            wxASSERT_MSG((!IsShowingGroupLegends() || foundGroupId != GetDocumentLabels().cend()),
                         L"Could not find group label for Flesch Chart!");
            scoreDataset->AddRow(
                Wisteria::Data::RowInfo()
                    .Id(wxFileName(doc->GetOriginalDocumentFilePath()).GetFullName().wc_str())
                    .Categoricals({ IsShowingGroupLegends() ? foundGroupId->second : 0 })
                    .Continuous({ asl, static_cast<double>(score), asw }));
            }
        }

    // Flesch Chart
    if (GetReadabilityTests().is_test_included(ReadabilityMessages::FLESCH()) && !m_docs.empty())
        {
        std::shared_ptr<Wisteria::Graphs::FleschChart> fleschChart{ nullptr };
        auto* fleschChartCanvas = dynamic_cast<Wisteria::Canvas*>(
            view->GetScoresView().FindWindowById(BaseProjectView::FLESCH_CHART_PAGE_ID));

        // document name brackets next to syllable ruler will
        // override the legend. (It would be to busy showing both of these.)
        const bool showLegend = (IsShowingGroupLegends() && !IsIncludingFleschRulerDocGroups());

        if (fleschChartCanvas == nullptr)
            {
            fleschChartCanvas =
                new Wisteria::Canvas(view->GetSplitter(), BaseProjectView::FLESCH_CHART_PAGE_ID);
            fleschChartCanvas->SetFixedObjectsGridSize(1, 1);

            fleschChart = std::make_shared<Wisteria::Graphs::FleschChart>(
                fleschChartCanvas, std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                                       *std::make_shared<Wisteria::Colors::Schemes::EarthTones>()));
            fleschChart->SetData(
                scoreDataset, wordsColumnName, scoresColumnName, syllablesColumnName,
                showLegend ? std::optional<const wxString>(groupColumnName) : std::nullopt,
                IsIncludingFleschRulerDocGroups());
            fleschChartCanvas->SetFixedObject(0, 0, fleschChart);
            fleschChartCanvas->Hide();
            view->SetFleschChart(fleschChartCanvas);
            view->GetFleschChart()->SetLabel(
                GetReadabilityTests().get_test_long_name(ReadabilityMessages::FLESCH()).c_str());
            view->GetFleschChart()->SetName(
                GetReadabilityTests().get_test_long_name(ReadabilityMessages::FLESCH()).c_str());
            }
        else
            {
            fleschChart = std::dynamic_pointer_cast<Wisteria::Graphs::FleschChart>(
                view->GetFleschChart()->GetFixedObject(0, 0));
            assert(fleschChart);
            fleschChart->SetData(
                scoreDataset, wordsColumnName, scoresColumnName, syllablesColumnName,
                showLegend ? std::optional<const wxString>(groupColumnName) : std::nullopt,
                IsIncludingFleschRulerDocGroups());
            }
        assert(fleschChart);
        UpdateGraphOptions(view->GetFleschChart());

        fleschChart->ShowConnectionLine(IsConnectingFleschPoints());

        if (showLegend)
            {
            view->GetFleschChart()->SetFixedObjectsGridSize(1, 2);
            view->GetFleschChart()->SetFixedObject(
                0, 1,
                fleschChart->CreateLegend(Wisteria::Graphs::LegendOptions().PlacementHint(
                    Wisteria::LegendCanvasPlacementHint::RightOfGraph)));
            }

        wxGCDC gdc(view->GetDocFrame());
        view->GetFleschChart()->CalcAllSizes(gdc);
        }
    else
        {
        view->GetScoresView().RemoveWindowById(BaseProjectView::FLESCH_CHART_PAGE_ID);
        }
    }

//------------------------------------------------------------
void BatchProjectDoc::DisplayGermanLixGauge()
    {
    auto* view = dynamic_cast<BatchProjectView*>(GetFirstView());

    const wxString scoresColumnName{ _DT(L"SCORES") };
    const wxString groupColumnName{ _DT(L"GROUP") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(scoresColumnName);
    scoreDataset->AddCategoricalColumn(groupColumnName, m_groupStringTable);
    scoreDataset->GetIdColumn().SetName(_DT(L"DOCS"));
    scoreDataset->Reserve(GetDocuments().size());

    for (auto* doc : GetDocuments())
        {
        if (doc->LoadingOriginalTextSucceeded())
            {
            readability::german_lix_difficulty diffLevel{};
            const size_t score =
                readability::german_lix(diffLevel, doc->GetTotalWords(),
                                        doc->GetTotalHardLixRixWords(), doc->GetTotalSentences());

            auto foundGroupId =
                GetDocumentLabels().find(doc->GetOriginalDocumentDescription().wc_str());
            wxASSERT_MSG((!IsShowingGroupLegends() || foundGroupId != GetDocumentLabels().cend()),
                         L"Could not find group label for German Lix gauge!");
            scoreDataset->AddRow(
                Wisteria::Data::RowInfo()
                    .Id(wxFileName(doc->GetOriginalDocumentFilePath()).GetFullName().wc_str())
                    .Categoricals({ IsShowingGroupLegends() ? foundGroupId->second : 0 })
                    .Continuous({ static_cast<double>(score) }));
            }
        }

    // German Lix Gauge
    if ((GetReadabilityTests().is_test_included(
             ReadabilityMessages::LIX_GERMAN_CHILDRENS_LITERATURE()) ||
         GetReadabilityTests().is_test_included(ReadabilityMessages::LIX_GERMAN_TECHNICAL())) &&
        !m_docs.empty())
        {
        std::shared_ptr<Wisteria::Graphs::LixGaugeGerman> lixGauge{ nullptr };
        auto* lixGaugeCanvas = dynamic_cast<Wisteria::Canvas*>(
            view->GetScoresView().FindWindowById(BaseProjectView::LIX_GAUGE_GERMAN_PAGE_ID));

        if (lixGaugeCanvas == nullptr)
            {
            lixGaugeCanvas = new Wisteria::Canvas(view->GetSplitter(),
                                                  BaseProjectView::LIX_GAUGE_GERMAN_PAGE_ID);
            lixGaugeCanvas->SetFixedObjectsGridSize(1, 1);

            lixGauge = std::make_shared<Wisteria::Graphs::LixGaugeGerman>(
                lixGaugeCanvas, std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                                    *std::make_shared<Wisteria::Colors::Schemes::EarthTones>()));
            lixGauge->SetData(scoreDataset, scoresColumnName,
                              IsShowingGroupLegends() ?
                                  std::optional<const wxString>(groupColumnName) :
                                  std::nullopt);
            lixGaugeCanvas->SetFixedObject(0, 0, lixGauge);
            lixGaugeCanvas->Hide();
            view->SetGermanLixGauge(lixGaugeCanvas);
            view->GetGermanLixGauge()->SetLabel(BaseProjectView::GetGermanLixGaugeLabel());
            view->GetGermanLixGauge()->SetName(BaseProjectView::GetGermanLixGaugeLabel());
            }
        else
            {
            lixGauge = std::dynamic_pointer_cast<Wisteria::Graphs::LixGaugeGerman>(
                view->GetGermanLixGauge()->GetFixedObject(0, 0));
            assert(lixGauge);
            lixGauge->SetData(scoreDataset, scoresColumnName,
                              IsShowingGroupLegends() ?
                                  std::optional<const wxString>(groupColumnName) :
                                  std::nullopt);
            }
        assert(lixGauge);
        lixGauge->UseEnglishLabels(IsUsingEnglishLabelsForGermanLix());
        lixGauge->ShowcaseScore(IsShowcasingKeyItems());
        UpdateGraphOptions(view->GetGermanLixGauge());

        if (IsShowingGroupLegends())
            {
            view->GetGermanLixGauge()->SetFixedObjectsGridSize(1, 2);
            view->GetGermanLixGauge()->SetFixedObject(
                0, 1,
                lixGauge->CreateLegend(Wisteria::Graphs::LegendOptions().PlacementHint(
                    Wisteria::LegendCanvasPlacementHint::RightOfGraph)));
            }
        else
            {
            lixGauge->SetColorScheme(std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                Wisteria::Colors::Schemes::ColorScheme{ Wisteria::Colors::ColorBrewer::GetColor(
                    Wisteria::Colors::Color::CelestialBlue) }));
            }

        wxGCDC gdc(view->GetDocFrame());
        view->GetGermanLixGauge()->CalcAllSizes(gdc);
        }
    else
        {
        view->GetScoresView().RemoveWindowById(BaseProjectView::LIX_GAUGE_GERMAN_PAGE_ID);
        }
    }

//------------------------------------------------------------
void BatchProjectDoc::DisplayLixGauge()
    {
    auto* view = dynamic_cast<BatchProjectView*>(GetFirstView());

    const wxString scoresColumnName{ _DT(L"SCORES") };
    const wxString groupColumnName{ _DT(L"GROUP") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(scoresColumnName);
    scoreDataset->AddCategoricalColumn(groupColumnName, m_groupStringTable);
    scoreDataset->GetIdColumn().SetName(_DT(L"DOCS"));
    scoreDataset->Reserve(GetDocuments().size());

    for (auto* doc : GetDocuments())
        {
        if (doc->LoadingOriginalTextSucceeded())
            {
            readability::lix_difficulty diffLevel{};
            size_t gradeLevel{ 1 };
            const size_t score =
                readability::lix(diffLevel, gradeLevel, doc->GetTotalWords(),
                                 doc->GetTotalHardLixRixWords(), doc->GetTotalSentences());
            auto foundGroupId =
                GetDocumentLabels().find(doc->GetOriginalDocumentDescription().wc_str());
            wxASSERT_MSG((!IsShowingGroupLegends() || foundGroupId != GetDocumentLabels().cend()),
                         L"Could not find group label for Lix Gauge!");
            scoreDataset->AddRow(
                Wisteria::Data::RowInfo()
                    .Id(wxFileName(doc->GetOriginalDocumentFilePath()).GetFullName().wc_str())
                    .Categoricals({ IsShowingGroupLegends() ? foundGroupId->second : 0 })
                    .Continuous({ static_cast<double>(score) }));
            }
        }

    // Lix Gauge
    if (GetReadabilityTests().is_test_included(ReadabilityMessages::LIX()) && !m_docs.empty())
        {
        std::shared_ptr<Wisteria::Graphs::LixGauge> lixGauge{ nullptr };
        auto* lixGaugeCanvas = dynamic_cast<Wisteria::Canvas*>(
            view->GetScoresView().FindWindowById(BaseProjectView::LIX_GAUGE_PAGE_ID));

        if (lixGaugeCanvas == nullptr)
            {
            lixGaugeCanvas =
                new Wisteria::Canvas(view->GetSplitter(), BaseProjectView::LIX_GAUGE_PAGE_ID);
            lixGaugeCanvas->SetFixedObjectsGridSize(1, 1);

            lixGauge = std::make_shared<Wisteria::Graphs::LixGauge>(
                lixGaugeCanvas, std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                                    *std::make_shared<Wisteria::Colors::Schemes::EarthTones>()));
            lixGauge->SetData(scoreDataset, scoresColumnName,
                              IsShowingGroupLegends() ?
                                  std::optional<const wxString>(groupColumnName) :
                                  std::nullopt);

            lixGaugeCanvas->SetFixedObject(0, 0, lixGauge);
            lixGaugeCanvas->Hide();
            view->SetLixGauge(lixGaugeCanvas);
            view->GetLixGauge()->SetLabel(
                GetReadabilityTests().get_test_long_name(ReadabilityMessages::LIX()).c_str());
            view->GetLixGauge()->SetName(
                GetReadabilityTests().get_test_long_name(ReadabilityMessages::LIX()).c_str());
            }
        else
            {
            lixGauge = std::dynamic_pointer_cast<Wisteria::Graphs::LixGauge>(
                view->GetLixGauge()->GetFixedObject(0, 0));
            assert(lixGauge);
            lixGauge->SetData(scoreDataset, scoresColumnName,
                              IsShowingGroupLegends() ?
                                  std::optional<const wxString>(groupColumnName) :
                                  std::nullopt);
            }
        UpdateGraphOptions(view->GetLixGauge());
        lixGauge->ShowcaseScore(IsShowcasingKeyItems());

        if (IsShowingGroupLegends())
            {
            view->GetLixGauge()->SetFixedObjectsGridSize(1, 2);
            view->GetLixGauge()->SetFixedObject(
                0, 1,
                lixGauge->CreateLegend(Wisteria::Graphs::LegendOptions().PlacementHint(
                    Wisteria::LegendCanvasPlacementHint::RightOfGraph)));
            }
        else
            {
            lixGauge->SetColorScheme(std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                Wisteria::Colors::Schemes::ColorScheme{ Wisteria::Colors::ColorBrewer::GetColor(
                    Wisteria::Colors::Color::CelestialBlue) }));
            }

        wxGCDC gdc(view->GetDocFrame());
        view->GetLixGauge()->CalcAllSizes(gdc);
        }
    else
        {
        view->GetScoresView().RemoveWindowById(BaseProjectView::LIX_GAUGE_PAGE_ID);
        }
    }

//------------------------------------------------------------
void BatchProjectDoc::DisplayReadabilityGraphs()
    {
    PROFILE();
    DisplayFleschChart();
    DisplayDB2Plot();
    DisplayCrawfordGraph();
    DisplayInfleszGraph();
    DisplayLixGauge();
    DisplayGermanLixGauge();

    const wxString groupColumnName{ _DT(L"GROUP") };
    // columns for Schwartz calculations
    const wxString totalWordsColumnName{ _DT(L"TOTALWORDS") };
    const wxString totalSyllablesNumeralsOneSyllableColumnName{ _DT(
        L"TOTALSYLLABLESNUMERALSONESYLLABLE") };
    const wxString totalSentenceUnitsColumnName{ _DT(L"TOTALSENTENCEUNITS") };
    // columns for FRASE calculations
    const wxString totalSyllablesColumnName{ _DT(L"TOTALSYLLABLES") };
    const wxString totalSentencesColumnName{ _DT(L"TOTALSENTENCES") };
    // Fry
    const wxString totalSyllablesNumeralsFullySyllabizedColumnName{ _DT(
        L"TOTALSYLLABLESNUMERALSFULLYSYLLABIZED") };
    // Raygor
    const wxString totalWordsLessNumerals{ _DT(L"TOTALWORDSLESSNUMERALS") };
    const wxString total6PlusCharWordsLessNumerals{ _DT(L"TOTAL6PLUSCHARWORDSLESSNUMERALS") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();

    scoreDataset->GetIdColumn().SetName(_DT(L"DOCS"));
    // do NOT change the ordering here
    scoreDataset->AddContinuousColumn(totalWordsColumnName);
    scoreDataset->AddContinuousColumn(totalSyllablesNumeralsOneSyllableColumnName);
    scoreDataset->AddContinuousColumn(totalSentenceUnitsColumnName);
    scoreDataset->AddContinuousColumn(totalSyllablesColumnName);
    scoreDataset->AddContinuousColumn(totalSentencesColumnName);
    scoreDataset->AddContinuousColumn(totalSyllablesNumeralsFullySyllabizedColumnName);
    scoreDataset->AddContinuousColumn(totalWordsLessNumerals);
    scoreDataset->AddContinuousColumn(total6PlusCharWordsLessNumerals);

    scoreDataset->AddCategoricalColumn(groupColumnName, m_groupStringTable);
    scoreDataset->Reserve(GetDocuments().size());

    for (auto* const doc : GetDocuments())
        {
        if (doc->LoadingOriginalTextSucceeded())
            {
            auto foundGroupId =
                GetDocumentLabels().find(doc->GetOriginalDocumentDescription().wc_str());
            wxASSERT_MSG((!IsShowingGroupLegends() || foundGroupId != GetDocumentLabels().cend()),
                         L"Could not find group label!");
            scoreDataset->AddRow(
                Wisteria::Data::RowInfo()
                    .Id(wxFileName(doc->GetOriginalDocumentFilePath()).GetFullName())
                    .Categoricals({ IsShowingGroupLegends() ? foundGroupId->second : 0 })
                    .
                // do NOT change the ordering here
                Continuous({ doc->GetTotalWords(), doc->GetTotalSyllablesNumeralsOneSyllable(),
                             doc->GetTotalSentenceUnits(), doc->GetTotalSyllables(),
                             doc->GetTotalSentences(),
                             doc->GetTotalSyllablesNumeralsFullySyllabized(),
                             doc->GetTotalWords() - doc->GetTotalNumerals(),
                             doc->GetTotalSixPlusCharacterWordsIgnoringNumerals() }));
            }
        }

    auto* view = dynamic_cast<BatchProjectView*>(GetFirstView());

    // Fry graph
    auto* fryGraphCanvas = dynamic_cast<Wisteria::Canvas*>(
        view->GetScoresView().FindWindowById(BaseProjectView::FRY_PAGE_ID));
    if (GetReadabilityTests().is_test_included(ReadabilityMessages::FRY()) &&
        !GetDocuments().empty())
        {
        std::shared_ptr<Wisteria::Graphs::FryGraph> fryGraph{ nullptr };
        if (fryGraphCanvas == nullptr)
            {
            fryGraphCanvas =
                new Wisteria::Canvas(view->GetSplitter(), BaseProjectView::FRY_PAGE_ID);

            fryGraph = std::make_shared<Wisteria::Graphs::FryGraph>(
                fryGraphCanvas, Wisteria::Graphs::FryGraph::FryGraphType::Traditional);
            fryGraph->SetMessageCatalog(GetReadabilityMessageCatalogPtr());
            fryGraph->SetData(
                scoreDataset, totalWordsColumnName, totalSyllablesNumeralsFullySyllabizedColumnName,
                totalSentencesColumnName,
                IsShowingGroupLegends() ? std::optional<const wxString>(groupColumnName) :
                                          std::nullopt);

            fryGraphCanvas->SetFixedObjectsGridSize(1, 1);
            fryGraphCanvas->SetFixedObject(0, 0, fryGraph);
            fryGraphCanvas->Hide();
            view->SetFryGraph(fryGraphCanvas);
            view->GetFryGraph()->SetLabel(
                GetReadabilityTests().get_test_long_name(ReadabilityMessages::FRY()).c_str());
            view->GetFryGraph()->SetName(
                GetReadabilityTests().get_test_long_name(ReadabilityMessages::FRY()).c_str());
            }
        else
            {
            fryGraph = std::dynamic_pointer_cast<Wisteria::Graphs::FryGraph>(
                view->GetFryGraph()->GetFixedObject(0, 0));
            assert(fryGraph);
            fryGraph->SetData(
                scoreDataset, totalWordsColumnName, totalSyllablesNumeralsFullySyllabizedColumnName,
                totalSentencesColumnName,
                IsShowingGroupLegends() ? std::optional<const wxString>(groupColumnName) :
                                          std::nullopt);
            }
        UpdateGraphOptions(view->GetFryGraph());
        fryGraph->SetInvalidAreaColor(GetInvalidAreaColor());

        // add legend if grouping
        if (IsShowingGroupLegends())
            {
            view->GetFryGraph()->SetFixedObjectsGridSize(1, 2);
            view->GetFryGraph()->SetFixedObject(
                0, 1,
                fryGraph->CreateLegend(Wisteria::Graphs::LegendOptions().PlacementHint(
                    Wisteria::LegendCanvasPlacementHint::RightOfGraph)));
            }
        // ...and if not grouping, use light blue for points
        else
            {
            fryGraph->SetColorScheme(std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                Wisteria::Colors::Schemes::ColorScheme{ Wisteria::Colors::ColorBrewer::GetColor(
                    Wisteria::Colors::Color::CelestialBlue) }));
            }

        wxGCDC gdc(view->GetDocFrame());
        view->GetFryGraph()->CalcAllSizes(gdc);
        }
    else
        {
        view->GetScoresView().RemoveWindowById(BaseProjectView::FRY_PAGE_ID);
        view->SetFryGraph(nullptr);
        }

    // GPM Fry graph
    fryGraphCanvas = dynamic_cast<Wisteria::Canvas*>(
        view->GetScoresView().FindWindowById(BaseProjectView::GPM_FRY_PAGE_ID));
    if (GetReadabilityTests().is_test_included(ReadabilityMessages::GPM_FRY()) &&
        !GetDocuments().empty())
        {
        std::shared_ptr<Wisteria::Graphs::FryGraph> gFryGraph{ nullptr };
        if (fryGraphCanvas == nullptr)
            {
            fryGraphCanvas =
                new Wisteria::Canvas(view->GetSplitter(), BaseProjectView::GPM_FRY_PAGE_ID);

            gFryGraph = std::make_shared<Wisteria::Graphs::FryGraph>(
                fryGraphCanvas, Wisteria::Graphs::FryGraph::FryGraphType::GPM);
            gFryGraph->SetMessageCatalog(GetReadabilityMessageCatalogPtr());
            gFryGraph->SetData(
                scoreDataset, totalWordsColumnName, totalSyllablesNumeralsFullySyllabizedColumnName,
                totalSentencesColumnName,
                IsShowingGroupLegends() ? std::optional<const wxString>(groupColumnName) :
                                          std::nullopt);

            fryGraphCanvas->SetFixedObjectsGridSize(1, 1);
            fryGraphCanvas->SetFixedObject(0, 0, gFryGraph);
            fryGraphCanvas->Hide();
            view->SetGpmFryGraph(fryGraphCanvas);
            view->GetGpmFryGraph()->SetLabel(
                GetReadabilityTests().get_test_long_name(ReadabilityMessages::GPM_FRY()).c_str());
            view->GetGpmFryGraph()->SetName(
                GetReadabilityTests().get_test_long_name(ReadabilityMessages::GPM_FRY()).c_str());
            }
        else
            {
            gFryGraph = std::dynamic_pointer_cast<Wisteria::Graphs::FryGraph>(
                view->GetGpmFryGraph()->GetFixedObject(0, 0));
            assert(gFryGraph);
            gFryGraph->SetData(
                scoreDataset, totalWordsColumnName, totalSyllablesNumeralsFullySyllabizedColumnName,
                totalSentencesColumnName,
                IsShowingGroupLegends() ? std::optional<const wxString>(groupColumnName) :
                                          std::nullopt);
            }
        UpdateGraphOptions(view->GetGpmFryGraph());
        gFryGraph->SetInvalidAreaColor(GetInvalidAreaColor());

        // add legend if grouping
        if (IsShowingGroupLegends())
            {
            view->GetGpmFryGraph()->SetFixedObjectsGridSize(1, 2);
            view->GetGpmFryGraph()->SetFixedObject(
                0, 1,
                gFryGraph->CreateLegend(Wisteria::Graphs::LegendOptions().PlacementHint(
                    Wisteria::LegendCanvasPlacementHint::RightOfGraph)));
            }
        // ...and if not grouping, use light blue for points
        else
            {
            gFryGraph->SetColorScheme(std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                Wisteria::Colors::Schemes::ColorScheme{ Wisteria::Colors::ColorBrewer::GetColor(
                    Wisteria::Colors::Color::CelestialBlue) }));
            }

        wxGCDC gdc(view->GetDocFrame());
        view->GetGpmFryGraph()->CalcAllSizes(gdc);
        }
    else
        {
        view->GetScoresView().RemoveWindowById(BaseProjectView::GPM_FRY_PAGE_ID);
        view->SetGpmFryGraph(nullptr);
        }

    // Schwartz graph
    auto* schwartzGraphCanvas = dynamic_cast<Wisteria::Canvas*>(
        view->GetScoresView().FindWindowById(BaseProjectView::SCHWARTZ_PAGE_ID));
    if (GetReadabilityTests().is_test_included(ReadabilityMessages::SCHWARTZ()) &&
        !GetDocuments().empty())
        {
        std::shared_ptr<Wisteria::Graphs::SchwartzGraph> schwartzGraph{ nullptr };
        if (schwartzGraphCanvas == nullptr)
            {
            schwartzGraphCanvas =
                new Wisteria::Canvas(view->GetSplitter(), BaseProjectView::SCHWARTZ_PAGE_ID);

            schwartzGraph = std::make_shared<Wisteria::Graphs::SchwartzGraph>(schwartzGraphCanvas);
            schwartzGraph->SetMessageCatalog(GetReadabilityMessageCatalogPtr());
            schwartzGraph->SetData(
                scoreDataset, totalWordsColumnName, totalSyllablesNumeralsOneSyllableColumnName,
                totalSentenceUnitsColumnName,
                IsShowingGroupLegends() ? std::optional<const wxString>(groupColumnName) :
                                          std::nullopt);

            schwartzGraphCanvas->SetFixedObjectsGridSize(1, 1);
            schwartzGraphCanvas->SetFixedObject(0, 0, schwartzGraph);
            schwartzGraphCanvas->Hide();
            view->SetSchwartzGraph(schwartzGraphCanvas);
            view->GetSchwartzGraph()->SetLabel(
                GetReadabilityTests().get_test_long_name(ReadabilityMessages::SCHWARTZ()).c_str());
            view->GetSchwartzGraph()->SetName(
                GetReadabilityTests().get_test_long_name(ReadabilityMessages::SCHWARTZ()).c_str());
            }
        else
            {
            schwartzGraph = std::dynamic_pointer_cast<Wisteria::Graphs::SchwartzGraph>(
                view->GetSchwartzGraph()->GetFixedObject(0, 0));
            assert(schwartzGraph);
            schwartzGraph->SetData(
                scoreDataset, totalWordsColumnName, totalSyllablesNumeralsOneSyllableColumnName,
                totalSentenceUnitsColumnName,
                IsShowingGroupLegends() ? std::optional<const wxString>(groupColumnName) :
                                          std::nullopt);
            }
        UpdateGraphOptions(view->GetSchwartzGraph());

        schwartzGraph->SetInvalidAreaColor(GetInvalidAreaColor());

        // add legend if grouping
        if (IsShowingGroupLegends())
            {
            view->GetSchwartzGraph()->SetFixedObjectsGridSize(1, 2);
            view->GetSchwartzGraph()->SetFixedObject(
                0, 1,
                schwartzGraph->CreateLegend(Wisteria::Graphs::LegendOptions().PlacementHint(
                    Wisteria::LegendCanvasPlacementHint::RightOfGraph)));
            }
        // ...and if not grouping, use light blue for points
        else
            {
            schwartzGraph->SetColorScheme(std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                Wisteria::Colors::Schemes::ColorScheme{ Wisteria::Colors::ColorBrewer::GetColor(
                    Wisteria::Colors::Color::CelestialBlue) }));
            }

        wxGCDC gdc(view->GetDocFrame());
        view->GetSchwartzGraph()->CalcAllSizes(gdc);
        }
    else
        {
        view->GetScoresView().RemoveWindowById(BaseProjectView::SCHWARTZ_PAGE_ID);
        view->SetSchwartzGraph(nullptr);
        }

    // FRASE graph
    auto* fraseGraphCanvas = dynamic_cast<Wisteria::Canvas*>(
        view->GetScoresView().FindWindowById(BaseProjectView::FRASE_PAGE_ID));
    if (GetReadabilityTests().is_test_included(ReadabilityMessages::FRASE()) &&
        !GetDocuments().empty())
        {
        std::shared_ptr<Wisteria::Graphs::FraseGraph> fraseGraph{ nullptr };
        if (fraseGraphCanvas == nullptr)
            {
            fraseGraphCanvas =
                new Wisteria::Canvas(view->GetSplitter(), BaseProjectView::FRASE_PAGE_ID);

            fraseGraph = std::make_shared<Wisteria::Graphs::FraseGraph>(fraseGraphCanvas);
            fraseGraph->SetMessageCatalog(GetReadabilityMessageCatalogPtr());
            fraseGraph->SetData(scoreDataset, totalWordsColumnName, totalSyllablesColumnName,
                                totalSentencesColumnName,
                                IsShowingGroupLegends() ?
                                    std::optional<const wxString>(groupColumnName) :
                                    std::nullopt);

            fraseGraphCanvas->SetFixedObjectsGridSize(1, 1);
            fraseGraphCanvas->SetFixedObject(0, 0, fraseGraph);
            fraseGraphCanvas->Hide();
            view->SetFraseGraph(fraseGraphCanvas);
            view->GetFraseGraph()->SetLabel(
                GetReadabilityTests().get_test_long_name(ReadabilityMessages::FRASE()).c_str());
            view->GetFraseGraph()->SetName(
                GetReadabilityTests().get_test_long_name(ReadabilityMessages::FRASE()).c_str());
            }
        else
            {
            fraseGraph = std::dynamic_pointer_cast<Wisteria::Graphs::FraseGraph>(
                view->GetFraseGraph()->GetFixedObject(0, 0));
            assert(fraseGraph);
            fraseGraph->SetData(scoreDataset, totalWordsColumnName, totalSyllablesColumnName,
                                totalSentencesColumnName,
                                IsShowingGroupLegends() ?
                                    std::optional<const wxString>(groupColumnName) :
                                    std::nullopt);
            }
        UpdateGraphOptions(view->GetFraseGraph());

        // add legend if grouping
        if (IsShowingGroupLegends())
            {
            view->GetFraseGraph()->SetFixedObjectsGridSize(1, 2);
            view->GetFraseGraph()->SetFixedObject(
                0, 1,
                fraseGraph->CreateLegend(Wisteria::Graphs::LegendOptions().PlacementHint(
                    Wisteria::LegendCanvasPlacementHint::RightOfGraph)));
            }
        // ...and if not grouping, use light blue for points
        else
            {
            fraseGraph->SetColorScheme(std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                Wisteria::Colors::Schemes::ColorScheme{ Wisteria::Colors::ColorBrewer::GetColor(
                    Wisteria::Colors::Color::CelestialBlue) }));
            }

        wxGCDC gdc(view->GetDocFrame());
        view->GetFraseGraph()->CalcAllSizes(gdc);
        }
    else
        {
        view->GetScoresView().RemoveWindowById(BaseProjectView::FRASE_PAGE_ID);
        view->SetFraseGraph(nullptr);
        }

    // Raygor graph
    auto* raygorGraphCanvas = dynamic_cast<Wisteria::Canvas*>(
        view->GetScoresView().FindWindowById(BaseProjectView::RAYGOR_PAGE_ID));
    if (GetReadabilityTests().is_test_included(ReadabilityMessages::RAYGOR()) &&
        !GetDocuments().empty())
        {
        std::shared_ptr<Wisteria::Graphs::RaygorGraph> raygorGraph{ nullptr };
        if (raygorGraphCanvas == nullptr)
            {
            raygorGraphCanvas =
                new Wisteria::Canvas(view->GetSplitter(), BaseProjectView::RAYGOR_PAGE_ID);

            raygorGraphCanvas->SetFixedObjectsGridSize(1, 1);

            raygorGraph = std::make_shared<Wisteria::Graphs::RaygorGraph>(raygorGraphCanvas);
            raygorGraph->SetMessageCatalog(GetReadabilityMessageCatalogPtr());
            raygorGraph->SetData(scoreDataset, totalWordsLessNumerals,
                                 total6PlusCharWordsLessNumerals, totalSentencesColumnName,
                                 IsShowingGroupLegends() ?
                                     std::optional<const wxString>(groupColumnName) :
                                     std::nullopt);

            raygorGraphCanvas->SetFixedObject(0, 0, raygorGraph);
            raygorGraphCanvas->Hide();
            view->SetRaygorGraph(raygorGraphCanvas);
            view->GetRaygorGraph()->SetLabel(
                GetReadabilityTests().get_test_long_name(ReadabilityMessages::RAYGOR()).c_str());
            view->GetRaygorGraph()->SetName(
                GetReadabilityTests().get_test_long_name(ReadabilityMessages::RAYGOR()).c_str());
            }
        else
            {
            raygorGraph = std::dynamic_pointer_cast<Wisteria::Graphs::RaygorGraph>(
                view->GetRaygorGraph()->GetFixedObject(0, 0));
            assert(raygorGraph);
            raygorGraph->SetData(scoreDataset, totalWordsLessNumerals,
                                 total6PlusCharWordsLessNumerals, totalSentencesColumnName,
                                 IsShowingGroupLegends() ?
                                     std::optional<const wxString>(groupColumnName) :
                                     std::nullopt);
            }
        UpdateGraphOptions(view->GetRaygorGraph());

        raygorGraph->SetInvalidAreaColor(GetInvalidAreaColor());
        raygorGraph->SetRaygorStyle(GetRaygorStyle());

        // add legend if grouping
        if (IsShowingGroupLegends())
            {
            view->GetRaygorGraph()->SetFixedObjectsGridSize(1, 2);
            view->GetRaygorGraph()->SetFixedObject(
                0, 1,
                raygorGraph->CreateLegend(Wisteria::Graphs::LegendOptions().PlacementHint(
                    Wisteria::LegendCanvasPlacementHint::RightOfGraph)));
            }
        // ...and if not grouping, use light blue for points
        else
            {
            raygorGraph->SetColorScheme(std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                Wisteria::Colors::Schemes::ColorScheme{ Wisteria::Colors::ColorBrewer::GetColor(
                    Wisteria::Colors::Color::CelestialBlue) }));
            }

        wxGCDC gdc(view->GetDocFrame());
        view->GetRaygorGraph()->CalcAllSizes(gdc);
        }
    else
        {
        view->GetScoresView().RemoveWindowById(BaseProjectView::RAYGOR_PAGE_ID);
        view->SetRaygorGraph(nullptr);
        }
    }

//------------------------------------------------------------
void BatchProjectDoc::DisplayBoxPlots()
    {
    PROFILE();
    auto* view = dynamic_cast<BatchProjectView*>(GetFirstView());

    // standard tests
    for (auto& sTest : GetReadabilityTests().get_tests())
        {
        if (sTest.get_test().get_test_type() == readability::readability_test_type::grade_level ||
            sTest.get_test().get_test_type() ==
                readability::readability_test_type::index_value_and_grade_level ||
            sTest.get_test().get_test_type() ==
                readability::readability_test_type::grade_level_and_predicted_cloze_score)
            {
            const wxString pageLabel = (sTest.get_test().get_test_type() ==
                                        readability::readability_test_type::grade_level) ?
                                           wxString(sTest.get_test().get_short_name().c_str()) :
                                           BatchProjectView::FormatGradeLevelsLabel(
                                               sTest.get_test().get_short_name().c_str());
            auto* boxPlotCanvas =
                dynamic_cast<Wisteria::Canvas*>(view->GetBoxPlotView().FindWindowByIdAndLabel(
                    sTest.get_test().get_interface_id(), pageLabel));
            if (sTest.is_included() && (sTest.get_grade_point_collection()
                                            ->GetContinuousColumn(GetScoreColumnName())
                                            ->GetRowCount() != 0U))
                {
                if (boxPlotCanvas == nullptr)
                    {
                    boxPlotCanvas = new Wisteria::Canvas(view->GetSplitter(),
                                                         sTest.get_test().get_interface_id());
                    boxPlotCanvas->SetFixedObjectsGridSize(1, 1);
                    boxPlotCanvas->SetFixedObject(
                        0, 0, std::make_shared<Wisteria::Graphs::BoxPlot>(boxPlotCanvas));
                    boxPlotCanvas->Hide();
                    boxPlotCanvas->SetLabel(pageLabel);
                    boxPlotCanvas->SetName(pageLabel);
                    view->GetBoxPlotView().AddWindow(boxPlotCanvas);
                    }
                UpdateGraphOptions(boxPlotCanvas);

                auto boxPlot = std::dynamic_pointer_cast<Wisteria::Graphs::BoxPlot>(
                    boxPlotCanvas->GetFixedObject(0, 0));
                wxASSERT_MSG(boxPlot, L"Invalid dynamic cast to box plot!");
                boxPlot->SetBrushScheme(std::make_shared<Wisteria::Brushes::Schemes::BrushScheme>(
                    Wisteria::Colors::Schemes::ColorScheme({ GetGraphBoxColor() })));
                boxPlot->SetData(sTest.get_grade_point_collection(), GetScoreColumnName(),
                                 // if more documents than groups, then use grouping
                                 (GetDocumentLabels().size() > 1 &&
                                  GetDocuments().size() > GetDocumentLabels().size()) ?
                                     std::optional<wxString>(GetGroupColumnName()) :
                                     std::nullopt);

                if (boxPlot->GetBoxCount() > 1)
                    {
                    boxPlotCanvas->SetFixedObjectsGridSize(1, 2);
                    boxPlotCanvas->SetFixedObject(
                        0, 1,
                        boxPlot->CreateLegend(Wisteria::Graphs::LegendOptions().PlacementHint(
                            Wisteria::LegendCanvasPlacementHint::RightOfGraph)));
                    }

                boxPlot->GetTitle() = Wisteria::GraphItems::Label(
                    Wisteria::GraphItems::GraphItemInfo(sTest.get_test().get_long_name().c_str())
                        .DPIScaling(boxPlotCanvas->GetDPIScaleFactor())
                        .Scaling(boxPlotCanvas->GetScaling())
                        .Pen(wxNullPen));

                boxPlot->SetShadowType(IsDisplayingDropShadows() ?
                                           Wisteria::ShadowType::RightSideAndBottomShadow :
                                           Wisteria::ShadowType::NoShadow);
                boxPlot->SetOpacity(GetGraphBoxOpacity());
                boxPlot->SetBoxEffect(GetGraphBoxEffect());
                if (const auto convertedIcon =
                        Wisteria::ReportEnumConvert::ConvertIcon(GetStippleShape());
                    convertedIcon)
                    {
                    boxPlot->SetStippleShape(convertedIcon.value());
                    }
                boxPlot->SetStippleShapeColor(GetStippleShapeColor());
                boxPlot->ShowLabels(IsDisplayingBoxPlotLabels());
                boxPlot->ShowAllPoints(IsShowingAllBoxPlotPoints());

                // Adjust the axis range and load its labels,
                // and force the grade (Y) axis to show the full range of Kindergarten through
                // Doctorate.
                boxPlot->GetLeftYAxis().SetRange(0, 19, 0, 1, 1);
                for (int i = 0; i < 20; ++i)
                    {
                    boxPlot->GetLeftYAxis().SetCustomLabel(
                        i, Wisteria::GraphItems::Label(
                               GetReadabilityMessageCatalog().GetGradeScaleLongLabel(i)));
                    }
                boxPlot->GetLeftYAxis().SetLabelDisplay(
                    Wisteria::AxisLabelDisplay::DisplayOnlyCustomLabels);
                wxGCDC gdc(view->GetDocFrame());
                boxPlotCanvas->CalcAllSizes(gdc);
                }
            else
                {
                view->GetBoxPlotView().RemoveWindowByIdAndLabel(sTest.get_test().get_interface_id(),
                                                                pageLabel);
                }
            }
        // some tests can have grade levels AND cloze or index values, so don't use "else" here,
        // go through each logic gate to plot all the test's results
        if (sTest.get_test().get_test_type() == readability::readability_test_type::index_value ||
            sTest.get_test().get_test_type() ==
                readability::readability_test_type::index_value_and_grade_level)
            {
            const wxString pageLabel = (sTest.get_test().get_test_type() ==
                                        readability::readability_test_type::index_value) ?
                                           wxString(sTest.get_test().get_short_name().c_str()) :
                                           BatchProjectView::FormatIndexValuesLabel(
                                               sTest.get_test().get_short_name().c_str());
            auto* boxPlotCanvas =
                dynamic_cast<Wisteria::Canvas*>(view->GetBoxPlotView().FindWindowByIdAndLabel(
                    sTest.get_test().get_interface_id(), pageLabel));
            if (sTest.is_included() && (sTest.get_index_point_collection()
                                            ->GetContinuousColumn(GetScoreColumnName())
                                            ->GetRowCount() != 0U))
                {
                if (boxPlotCanvas == nullptr)
                    {
                    boxPlotCanvas = new Wisteria::Canvas(view->GetSplitter(),
                                                         sTest.get_test().get_interface_id());
                    boxPlotCanvas->SetFixedObjectsGridSize(1, 1);
                    boxPlotCanvas->SetFixedObject(
                        0, 0, std::make_shared<Wisteria::Graphs::BoxPlot>(boxPlotCanvas));
                    boxPlotCanvas->Hide();
                    boxPlotCanvas->SetLabel(pageLabel);
                    boxPlotCanvas->SetName(pageLabel);
                    view->GetBoxPlotView().AddWindow(boxPlotCanvas);
                    }
                UpdateGraphOptions(boxPlotCanvas);

                auto boxPlot = std::dynamic_pointer_cast<Wisteria::Graphs::BoxPlot>(
                    boxPlotCanvas->GetFixedObject(0, 0));
                wxASSERT_MSG(boxPlot, L"Invalid dynamic cast to box plot!");
                boxPlot->SetBrushScheme(std::make_shared<Wisteria::Brushes::Schemes::BrushScheme>(
                    Wisteria::Colors::Schemes::ColorScheme({ GetGraphBoxColor() })));
                boxPlot->SetData(sTest.get_index_point_collection(), GetScoreColumnName(),
                                 // if more documents than groups, then use grouping
                                 (GetDocumentLabels().size() > 1 &&
                                  GetDocuments().size() > GetDocumentLabels().size()) ?
                                     std::optional<wxString>(GetGroupColumnName()) :
                                     std::nullopt);

                if (boxPlot->GetBoxCount() > 1)
                    {
                    boxPlotCanvas->SetFixedObjectsGridSize(1, 2);
                    boxPlotCanvas->SetFixedObject(
                        0, 1,
                        boxPlot->CreateLegend(Wisteria::Graphs::LegendOptions().PlacementHint(
                            Wisteria::LegendCanvasPlacementHint::RightOfGraph)));
                    }

                boxPlot->GetTitle() = Wisteria::GraphItems::Label(
                    Wisteria::GraphItems::GraphItemInfo(sTest.get_test().get_long_name().c_str())
                        .DPIScaling(boxPlotCanvas->GetDPIScaleFactor())
                        .Scaling(boxPlotCanvas->GetScaling())
                        .Pen(wxNullPen));

                boxPlot->SetShadowType(IsDisplayingDropShadows() ?
                                           Wisteria::ShadowType::RightSideAndBottomShadow :
                                           Wisteria::ShadowType::NoShadow);
                boxPlot->ShowLabels(IsDisplayingBoxPlotLabels());
                boxPlot->ShowAllPoints(IsShowingAllBoxPlotPoints());
                boxPlot->SetOpacity(GetGraphBoxOpacity());
                boxPlot->SetBoxEffect(GetGraphBoxEffect());
                if (const auto convertedIcon =
                        Wisteria::ReportEnumConvert::ConvertIcon(GetStippleShape());
                    convertedIcon)
                    {
                    boxPlot->SetStippleShape(convertedIcon.value());
                    }
                boxPlot->SetStippleShapeColor(GetStippleShapeColor());

                const auto [rangeStart, rangeEnd] = boxPlot->GetLeftYAxis().GetRange();
                // Set the ranges to fit the index range. Note that the calculated outlier ranges
                // may go outside the standard test range, so use the calculated range if larger.
                if (sTest.get_test().get_id() == ReadabilityMessages::FLESCH().wc_str())
                    {
                    boxPlot->GetLeftYAxis().SetRange(std::min<double>(0, rangeStart),
                                                     std::max<double>(100, rangeEnd), 0, 10, 1);
                    }
                else if (sTest.get_test().get_id() == ReadabilityMessages::EFLAW().wc_str())
                    {
                    boxPlot->GetLeftYAxis().SetRange(std::min<double>(0, rangeStart),
                                                     std::max<double>(30, rangeEnd), 0, 5, 1);
                    }
                else if (sTest.get_test().get_id() == ReadabilityMessages::LIX().wc_str())
                    {
                    boxPlot->GetLeftYAxis().SetRange(std::min<double>(0, rangeStart),
                                                     std::max<double>(60, rangeEnd), 0, 10, 1);
                    }
                else if (sTest.get_test().get_id() == ReadabilityMessages::RIX().wc_str())
                    {
                    boxPlot->GetLeftYAxis().SetRange(
                        std::min<double>(0, rangeStart), std::max<double>(8, rangeEnd), 1,
                        boxPlot->GetLeftYAxis().GetInterval() /*might be 1 or 2*/, 1);
                    }
                wxGCDC gdc(view->GetDocFrame());
                boxPlotCanvas->CalcAllSizes(gdc);
                }
            else
                {
                view->GetBoxPlotView().RemoveWindowByIdAndLabel(sTest.get_test().get_interface_id(),
                                                                pageLabel);
                }
            }
        if (sTest.get_test().get_test_type() ==
                readability::readability_test_type::predicted_cloze_score ||
            sTest.get_test().get_test_type() ==
                readability::readability_test_type::grade_level_and_predicted_cloze_score)
            {
            const wxString pageLabel = (sTest.get_test().get_test_type() ==
                                        readability::readability_test_type::predicted_cloze_score) ?
                                           wxString(sTest.get_test().get_short_name().c_str()) :
                                           BatchProjectView::FormatClozeValuesLabel(
                                               sTest.get_test().get_short_name().c_str());
            auto* boxPlotCanvas =
                dynamic_cast<Wisteria::Canvas*>(view->GetBoxPlotView().FindWindowByIdAndLabel(
                    sTest.get_test().get_interface_id(), pageLabel));
            if (sTest.is_included() && (sTest.get_cloze_point_collection()
                                            ->GetContinuousColumn(GetScoreColumnName())
                                            ->GetRowCount() != 0U))
                {
                if (boxPlotCanvas == nullptr)
                    {
                    boxPlotCanvas = new Wisteria::Canvas(view->GetSplitter(),
                                                         sTest.get_test().get_interface_id());
                    boxPlotCanvas->SetFixedObjectsGridSize(1, 1);
                    boxPlotCanvas->SetFixedObject(
                        0, 0, std::make_shared<Wisteria::Graphs::BoxPlot>(boxPlotCanvas));
                    boxPlotCanvas->Hide();
                    boxPlotCanvas->SetLabel(pageLabel);
                    boxPlotCanvas->SetName(pageLabel);
                    view->GetBoxPlotView().AddWindow(boxPlotCanvas);
                    }
                UpdateGraphOptions(boxPlotCanvas);

                auto boxPlot = std::dynamic_pointer_cast<Wisteria::Graphs::BoxPlot>(
                    boxPlotCanvas->GetFixedObject(0, 0));
                wxASSERT_MSG(boxPlot, L"Invalid dynamic cast to box plot!");
                boxPlot->SetBrushScheme(std::make_shared<Wisteria::Brushes::Schemes::BrushScheme>(
                    Wisteria::Colors::Schemes::ColorScheme({ GetGraphBoxColor() })));
                boxPlot->SetData(sTest.get_cloze_point_collection(), GetScoreColumnName(),
                                 // if more documents than groups, then use grouping
                                 (GetDocumentLabels().size() > 1 &&
                                  GetDocuments().size() > GetDocumentLabels().size()) ?
                                     std::optional<wxString>(GetGroupColumnName()) :
                                     std::nullopt);

                if (boxPlot->GetBoxCount() > 1)
                    {
                    boxPlotCanvas->SetFixedObjectsGridSize(1, 2);
                    boxPlotCanvas->SetFixedObject(
                        0, 1,
                        boxPlot->CreateLegend(Wisteria::Graphs::LegendOptions().PlacementHint(
                            Wisteria::LegendCanvasPlacementHint::RightOfGraph)));
                    }

                boxPlot->GetTitle() = Wisteria::GraphItems::Label(
                    Wisteria::GraphItems::GraphItemInfo(sTest.get_test().get_long_name().c_str())
                        .DPIScaling(boxPlotCanvas->GetDPIScaleFactor())
                        .Scaling(boxPlotCanvas->GetScaling())
                        .Pen(wxNullPen));
                boxPlot->SetShadowType(IsDisplayingDropShadows() ?
                                           Wisteria::ShadowType::RightSideAndBottomShadow :
                                           Wisteria::ShadowType::NoShadow);
                boxPlot->SetOpacity(GetGraphBoxOpacity());
                boxPlot->SetBoxEffect(GetGraphBoxEffect());
                if (const auto convertedIcon =
                        Wisteria::ReportEnumConvert::ConvertIcon(GetStippleShape());
                    convertedIcon)
                    {
                    boxPlot->SetStippleShape(convertedIcon.value());
                    }
                boxPlot->SetStippleShapeColor(GetStippleShapeColor());
                boxPlot->ShowLabels(IsDisplayingBoxPlotLabels());
                boxPlot->ShowAllPoints(IsShowingAllBoxPlotPoints());
                boxPlot->SetLabelPrecision(0);

                // adjust the range
                const auto [rangeStart, rangeEnd] = boxPlot->GetLeftYAxis().GetRange();
                boxPlot->GetLeftYAxis().SetRange(std::min<double>(0, rangeStart),
                                                 std::max<double>(100, rangeEnd), 0, 10, 1);
                wxGCDC gdc(view->GetDocFrame());
                boxPlotCanvas->CalcAllSizes(gdc);
                }
            else
                {
                view->GetBoxPlotView().RemoveWindowByIdAndLabel(sTest.get_test().get_interface_id(),
                                                                pageLabel);
                }
            }
        }

    // custom tests
    for (auto testPos = GetCustTestsInUse().cbegin(); testPos != GetCustTestsInUse().cend();
         ++testPos)
        {
        if (testPos->GetIterator()->get_test_type() ==
            readability::readability_test_type::grade_level)
            {
            auto* boxPlotCanvas = dynamic_cast<Wisteria::Canvas*>(
                view->GetBoxPlotView().FindWindowById(testPos->GetIterator()->get_interface_id()));
            auto& scoreDataset = m_customTestScores[(testPos - GetCustTestsInUse().begin())];
            if (scoreDataset->GetContinuousColumn(GetScoreColumnName())->GetRowCount() != 0U)
                {
                if (boxPlotCanvas == nullptr)
                    {
                    boxPlotCanvas = new Wisteria::Canvas(
                        view->GetSplitter(), testPos->GetIterator()->get_interface_id());
                    boxPlotCanvas->SetFixedObjectsGridSize(1, 1);
                    boxPlotCanvas->SetFixedObject(
                        0, 0, std::make_shared<Wisteria::Graphs::BoxPlot>(boxPlotCanvas));
                    boxPlotCanvas->Hide();
                    boxPlotCanvas->SetLabel(testPos->GetIterator()->get_name().c_str());
                    boxPlotCanvas->SetName(testPos->GetIterator()->get_name().c_str());
                    view->GetBoxPlotView().AddWindow(boxPlotCanvas);
                    }
                UpdateGraphOptions(boxPlotCanvas);

                auto boxPlot = std::dynamic_pointer_cast<Wisteria::Graphs::BoxPlot>(
                    boxPlotCanvas->GetFixedObject(0, 0));
                wxASSERT_MSG(boxPlot, L"Invalid dynamic cast to box plot!");
                boxPlot->SetBrushScheme(std::make_shared<Wisteria::Brushes::Schemes::BrushScheme>(
                    Wisteria::Colors::Schemes::ColorScheme({ GetGraphBoxColor() })));
                boxPlot->SetData(scoreDataset, GetScoreColumnName(),
                                 // if more documents than groups, then use grouping
                                 (GetDocumentLabels().size() > 1 &&
                                  GetDocuments().size() > GetDocumentLabels().size()) ?
                                     std::optional<wxString>(GetGroupColumnName()) :
                                     std::nullopt);

                if (boxPlot->GetBoxCount() > 1)
                    {
                    boxPlotCanvas->SetFixedObjectsGridSize(1, 2);
                    boxPlotCanvas->SetFixedObject(
                        0, 1,
                        boxPlot->CreateLegend(Wisteria::Graphs::LegendOptions().PlacementHint(
                            Wisteria::LegendCanvasPlacementHint::RightOfGraph)));
                    }

                boxPlot->GetTitle() = Wisteria::GraphItems::Label(
                    Wisteria::GraphItems::GraphItemInfo(testPos->GetIterator()->get_name().c_str())
                        .DPIScaling(boxPlotCanvas->GetDPIScaleFactor())
                        .Scaling(boxPlotCanvas->GetScaling())
                        .Pen(wxNullPen));

                boxPlot->SetShadowType(IsDisplayingDropShadows() ?
                                           Wisteria::ShadowType::RightSideAndBottomShadow :
                                           Wisteria::ShadowType::NoShadow);
                boxPlot->SetOpacity(GetGraphBoxOpacity());
                boxPlot->SetBoxEffect(GetGraphBoxEffect());
                if (const auto convertedIcon =
                        Wisteria::ReportEnumConvert::ConvertIcon(GetStippleShape());
                    convertedIcon)
                    {
                    boxPlot->SetStippleShape(convertedIcon.value());
                    }
                boxPlot->SetStippleShapeColor(GetStippleShapeColor());
                boxPlot->ShowLabels(IsDisplayingBoxPlotLabels());
                boxPlot->ShowAllPoints(IsShowingAllBoxPlotPoints());

                // Adjust the axis range and load its labels,
                // and force the grade (Y) axis to show the full range of
                // Kindergarten through Doctorate.
                boxPlot->GetLeftYAxis().SetRange(0, 19, 0, 1, 1);
                for (int i = 0; i < 20; ++i)
                    {
                    boxPlot->GetLeftYAxis().SetCustomLabel(
                        i, Wisteria::GraphItems::Label(
                               GetReadabilityMessageCatalog().GetGradeScaleLongLabel(i)));
                    }
                boxPlot->GetLeftYAxis().SetLabelDisplay(
                    Wisteria::AxisLabelDisplay::DisplayOnlyCustomLabels);
                wxGCDC gdc(view->GetDocFrame());
                boxPlotCanvas->CalcAllSizes(gdc);
                }
            else
                {
                view->GetBoxPlotView().RemoveWindowById(testPos->GetIterator()->get_interface_id());
                }
            }
        else if (testPos->GetIterator()->get_test_type() ==
                     readability::readability_test_type::index_value ||
                 testPos->GetIterator()->get_test_type() ==
                     readability::readability_test_type::predicted_cloze_score)
            {
            Wisteria::Canvas* boxPlotCanvas = dynamic_cast<Wisteria::Canvas*>(
                view->GetBoxPlotView().FindWindowById(testPos->GetIterator()->get_interface_id()));
            auto& scoreDataset = m_customTestScores[(testPos - GetCustTestsInUse().begin())];
            if (scoreDataset->GetContinuousColumn(GetScoreColumnName())->GetRowCount() != 0U)
                {
                if (boxPlotCanvas == nullptr)
                    {
                    boxPlotCanvas = new Wisteria::Canvas(
                        view->GetSplitter(), testPos->GetIterator()->get_interface_id());
                    boxPlotCanvas->SetFixedObjectsGridSize(1, 1);
                    boxPlotCanvas->SetFixedObject(
                        0, 0, std::make_shared<Wisteria::Graphs::BoxPlot>(boxPlotCanvas));
                    boxPlotCanvas->Hide();
                    boxPlotCanvas->SetLabel(testPos->GetIterator()->get_name().c_str());
                    boxPlotCanvas->SetName(testPos->GetIterator()->get_name().c_str());
                    view->GetBoxPlotView().AddWindow(boxPlotCanvas);
                    }
                UpdateGraphOptions(boxPlotCanvas);

                auto boxPlot = std::dynamic_pointer_cast<Wisteria::Graphs::BoxPlot>(
                    boxPlotCanvas->GetFixedObject(0, 0));
                wxASSERT_MSG(boxPlot, L"Invalid dynamic cast to box plot!");
                boxPlot->SetData(scoreDataset, GetScoreColumnName(),
                                 // if more documents than groups, then use grouping
                                 (GetDocumentLabels().size() > 1 &&
                                  GetDocuments().size() > GetDocumentLabels().size()) ?
                                     std::optional<wxString>(GetGroupColumnName()) :
                                     std::nullopt);
                boxPlot->SetBrushScheme(std::make_shared<Wisteria::Brushes::Schemes::BrushScheme>(
                    Wisteria::Colors::Schemes::ColorScheme({ GetGraphBoxColor() })));

                if (boxPlot->GetBoxCount() > 1)
                    {
                    boxPlotCanvas->SetFixedObjectsGridSize(1, 2);
                    boxPlotCanvas->SetFixedObject(
                        0, 1,
                        boxPlot->CreateLegend(Wisteria::Graphs::LegendOptions().PlacementHint(
                            Wisteria::LegendCanvasPlacementHint::RightOfGraph)));
                    }

                boxPlot->GetTitle() = Wisteria::GraphItems::Label(
                    Wisteria::GraphItems::GraphItemInfo(testPos->GetIterator()->get_name().c_str())
                        .DPIScaling(boxPlotCanvas->GetDPIScaleFactor())
                        .Scaling(boxPlotCanvas->GetScaling())
                        .Pen(wxNullPen));

                boxPlot->SetShadowType(IsDisplayingDropShadows() ?
                                           Wisteria::ShadowType::RightSideAndBottomShadow :
                                           Wisteria::ShadowType::NoShadow);
                boxPlot->SetOpacity(GetGraphBoxOpacity());
                boxPlot->SetBoxEffect(GetGraphBoxEffect());
                if (const auto convertedIcon =
                        Wisteria::ReportEnumConvert::ConvertIcon(GetStippleShape());
                    convertedIcon)
                    {
                    boxPlot->SetStippleShape(convertedIcon.value());
                    }
                boxPlot->SetStippleShapeColor(GetStippleShapeColor());
                boxPlot->ShowLabels(IsDisplayingBoxPlotLabels());
                boxPlot->ShowAllPoints(IsShowingAllBoxPlotPoints());

                // adjust the axis range
                const auto [rangeStart, rangeEnd] = boxPlot->GetLeftYAxis().GetRange();
                if (testPos->GetIterator()->get_test_type() ==
                    readability::readability_test_type::predicted_cloze_score)
                    {
                    boxPlot->GetLeftYAxis().SetRange(std::min<double>(0, rangeStart),
                                                     std::max<double>(100, rangeEnd), 0, 10, 1);
                    }
                wxGCDC gdc(view->GetDocFrame());
                boxPlotCanvas->CalcAllSizes(gdc);
                }
            else
                {
                view->GetBoxPlotView().RemoveWindowById(testPos->GetIterator()->get_interface_id());
                }
            }
        }
    }

//------------------------------------------------------------
void BatchProjectDoc::DisplayHistograms()
    {
    PROFILE();
    // First, remove any custom-test histograms that had their test removed from the project.
    auto* view = dynamic_cast<BatchProjectView*>(GetFirstView());
    std::set<wxWindowID> validTestNames;
    for (auto& rTests : GetReadabilityTests().get_tests())
        {
        validTestNames.insert(rTests.get_test().get_interface_id());
        }
    for (const auto& testPos : GetCustTestsInUse())
        {
        validTestNames.insert(testPos.GetIterator()->get_interface_id());
        }
    const long pageCount = static_cast<long>(view->GetHistogramsView().GetWindowCount());
    for (long i = pageCount - 1; i >= 0; --i)
        {
        const wxWindowID currentId = view->GetHistogramsView().GetWindow(i)->GetId();
        if (!validTestNames.contains(currentId))
            {
            view->GetHistogramsView().RemoveWindowById(currentId);
            }
        }

    // standard tests
    for (auto& rTests : GetReadabilityTests().get_tests())
        {
        if (rTests.get_test().get_test_type() == readability::readability_test_type::grade_level)
            {
            DisplayHistogram(rTests.get_test().get_short_name().c_str(),
                             rTests.get_test().get_interface_id(),
                             rTests.get_test().get_long_name().c_str(), _(L"Grade Levels"),
                             rTests.get_grade_point_collection(), rTests.is_included(), true, true);
            }
        else if (rTests.get_test().get_test_type() ==
                 readability::readability_test_type::index_value)
            {
            DisplayHistogram(
                rTests.get_test().get_short_name().c_str(), rTests.get_test().get_interface_id(),
                rTests.get_test().get_long_name().c_str(), _(L"Index Values"),
                rTests.get_index_point_collection(), rTests.is_included(), false, false);
            }
        else if (rTests.get_test().get_test_type() ==
                 readability::readability_test_type::predicted_cloze_score)
            {
            DisplayHistogram(
                rTests.get_test().get_short_name().c_str(), rTests.get_test().get_interface_id(),
                rTests.get_test().get_long_name().c_str(), _(L"Predicted Cloze Scores"),
                rTests.get_cloze_point_collection(), rTests.is_included(), false, true);
            }
        else if (rTests.get_test().get_test_type() ==
                 readability::readability_test_type::index_value_and_grade_level)
            {
            DisplayHistogram(
                BatchProjectView::FormatIndexValuesLabel(
                    rTests.get_test().get_short_name().c_str()),
                rTests.get_test().get_interface_id(),
                BatchProjectView::FormatIndexValuesLabel(rTests.get_test().get_long_name().c_str()),
                _(L"Index Values"), rTests.get_index_point_collection(), rTests.is_included(),
                false, false);
            DisplayHistogram(
                BatchProjectView::FormatGradeLevelsLabel(
                    rTests.get_test().get_short_name().c_str()),
                rTests.get_test().get_interface_id(),
                BatchProjectView::FormatGradeLevelsLabel(rTests.get_test().get_long_name().c_str()),
                _(L"Grade Levels"), rTests.get_grade_point_collection(), rTests.is_included(), true,
                true);
            }
        else if (rTests.get_test().get_test_type() ==
                 readability::readability_test_type::grade_level_and_predicted_cloze_score)
            {
            DisplayHistogram(
                BatchProjectView::FormatGradeLevelsLabel(
                    rTests.get_test().get_short_name().c_str()),
                rTests.get_test().get_interface_id(),
                BatchProjectView::FormatGradeLevelsLabel(rTests.get_test().get_long_name().c_str()),
                _(L"Grade Levels"), rTests.get_grade_point_collection(), rTests.is_included(), true,
                true);
            DisplayHistogram(
                BatchProjectView::FormatClozeValuesLabel(
                    rTests.get_test().get_short_name().c_str()),
                rTests.get_test().get_interface_id(),
                BatchProjectView::FormatClozeValuesLabel(rTests.get_test().get_long_name().c_str()),
                _(L"Predicted Cloze Scores"), rTests.get_cloze_point_collection(),
                rTests.is_included(), false, true);
            }
        }
    // Custom word tests
    for (auto testPos = GetCustTestsInUse().cbegin(); testPos != GetCustTestsInUse().cend();
         ++testPos)
        {
        auto& scoreDataset = m_customTestScores[(testPos - GetCustTestsInUse().begin())];
        DisplayHistogram(testPos->GetTestName(), testPos->GetIterator()->get_interface_id(),
                         testPos->GetTestName(),
                         testPos->GetIterator()->get_test_type() ==
                                 readability::readability_test_type::grade_level ?
                             _(L"Grade Levels") :
                         testPos->GetIterator()->get_test_type() ==
                                 readability::readability_test_type::index_value ?
                             _(L"Index Values") :
                             _(L"Predicted Cloze Scores"),
                         scoreDataset, true,
                         testPos->GetIterator()->get_test_type() ==
                             readability::readability_test_type::grade_level,
                         (testPos->GetIterator()->get_test_type() ==
                              readability::readability_test_type::grade_level ||
                          testPos->GetIterator()->get_test_type() ==
                              readability::readability_test_type::predicted_cloze_score));
        }
    }

//------------------------------------------------
void BatchProjectDoc::DisplayHistogram(const wxString& name, const wxWindowID Id,
                                       const wxString& topLabel, const wxString& bottomLabel,
                                       const std::shared_ptr<const Wisteria::Data::Dataset>& data,
                                       const bool includeTest, const bool isTestGradeLevel,
                                       const bool startAtOne)
    {
    auto* view = dynamic_cast<BatchProjectView*>(GetFirstView());

    auto* canvas =
        dynamic_cast<Wisteria::Canvas*>(view->GetHistogramsView().FindWindowByIdAndLabel(Id, name));
    if (includeTest && (data->GetRowCount() != 0U))
        {
        if (canvas == nullptr)
            {
            canvas = new Wisteria::Canvas(view->GetSplitter(), Id);
            canvas->SetFixedObjectsGridSize(1, 1);
            canvas->SetFixedObject(
                0, 0,
                std::make_shared<Wisteria::Graphs::Histogram>(
                    canvas, IsShowingGroupLegends() ?
                                std::make_shared<Wisteria::Brushes::Schemes::BrushScheme>(
                                    *std::make_shared<Wisteria::Colors::Schemes::EarthTones>()) :
                                std::make_shared<Wisteria::Brushes::Schemes::BrushScheme>(
                                    *std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                                        Wisteria::Colors::Schemes::ColorScheme{
                                            GetHistogramBarColor() }))));
            canvas->Hide();
            canvas->SetLabel(name);
            canvas->SetName(name);
            view->GetHistogramsView().AddWindow(canvas);
            }
        UpdateGraphOptions(canvas);

        auto histogram =
            std::dynamic_pointer_cast<Wisteria::Graphs::Histogram>(canvas->GetFixedObject(0, 0));
        wxASSERT_MSG(histogram, L"Invalid histogram cast!");

        if (!IsShowingGroupLegends())
            {
            histogram->SetBrushScheme(std::make_shared<Wisteria::Brushes::Schemes::BrushScheme>(
                *std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                    Wisteria::Colors::Schemes::ColorScheme({ GetHistogramBarColor() }))));
            }

        histogram->SetData(
            data, GetScoreColumnName(),
            (IsShowingGroupLegends() ? std::optional<wxString>(GetGroupColumnName()) :
                                       std::nullopt),
            (isTestGradeLevel ? Wisteria::Graphs::Histogram::BinningMethod::BinUniqueValues :
                                GetHistogramBinningMethod()),
            GetHistogramRoundingMethod(), GetHistogramIntervalDisplay(),
            GetHistogramBinLabelDisplay(), true,
            (isTestGradeLevel ? 0 :
             startAtOne       ? 1 :
                                std::numeric_limits<double>::quiet_NaN()));

        if (IsShowingGroupLegends())
            {
            canvas->SetFixedObjectsGridSize(1, 2);
            canvas->SetFixedObject(
                0, 1,
                histogram->CreateLegend(
                    Wisteria::Graphs::LegendOptions()
                        .PlacementHint(Wisteria::LegendCanvasPlacementHint::RightOfGraph)
                        .IncludeHeader(false)));
            }

        histogram->ClearProperties();
        histogram->GetBarAxis().ClearBrackets();
        histogram->GetTitle() =
            Wisteria::GraphItems::Label(Wisteria::GraphItems::GraphItemInfo(topLabel)
                                            .DPIScaling(canvas->GetDPIScaleFactor())
                                            .Scaling(canvas->GetScaling())
                                            .Pen(wxNullPen));
        histogram->GetScalingAxis().GetTitle().SetText(_(L"Number of Documents"));
        histogram->GetBarAxis().GetTitle() =
            Wisteria::GraphItems::Label(Wisteria::GraphItems::GraphItemInfo(bottomLabel)
                                            .DPIScaling(canvas->GetDPIScaleFactor())
                                            .Scaling(canvas->GetScaling())
                                            .Pen(wxNullPen));
        histogram->SetShadowType(IsDisplayingDropShadows() ? Wisteria::ShadowType::RightSideShadow :
                                                             Wisteria::ShadowType::NoShadow);
        histogram->SetBarEffect(GetHistogramBarEffect());
        if (const auto convertedIcon = Wisteria::ReportEnumConvert::ConvertIcon(GetStippleShape());
            convertedIcon)
            {
            histogram->SetStippleShape(convertedIcon.value());
            }
        histogram->SetStippleShapeColor(GetStippleShapeColor());
        histogram->SetBarOpacity(GetHistogramBarOpacity());

        histogram->GetBarAxis().SetLabelLineLength(10);
        if (isTestGradeLevel)
            {
            histogram->AddProperty(_DT(L"ISGRADEPLOT"), true);
            if (!histogram->GetBars().empty())
                {
                for (int i = 0; i < 20; ++i)
                    {
                    const auto foundValidBar =
                        std::find_if(histogram->GetBars().cbegin(), histogram->GetBars().cend(),
                                     [i](const auto& bar) noexcept
                                     { return bar.GetAxisPosition() == i && bar.GetLength() > 0; });
                    histogram->GetBarAxis().SetCustomLabel(
                        i,
                        // include the Kindergarten bin label just to show where everything starts,
                        // then hide any labels where the bins are empty
                        (foundValidBar == histogram->GetBars().cend() && i > 0) ?
                            Wisteria::GraphItems::Label{} :
                            Wisteria::GraphItems::Label{
                                GetReadabilityMessageCatalog().GetGradeScaleLongLabel(i) });
                    }
                // if not too many bins, show the long grade labels on the x-axis
                histogram->GetBarAxis().SetLabelDisplay(
                    (histogram->GetBinsWithValuesCount() <= 5) ?
                        Wisteria::AxisLabelDisplay::DisplayOnlyCustomLabels :
                        Wisteria::AxisLabelDisplay::DisplayValues);
                if (GetReadabilityMessageCatalog().GetGradeScale() ==
                    readability::grade_scale::k12_plus_united_states)
                    {
                    const auto lastGradeBar = histogram->GetBars().back().GetAxisPosition();
                    const auto addGradesBracket =
                        [&histogram, &lastGradeBar, this](
                            const double startGrade, const double endGrade, const wxString& label)
                    {
                        if (startGrade > lastGradeBar)
                            {
                            return;
                            }
                        histogram->GetBarAxis().AddBracket(Wisteria::GraphItems::Axis::AxisBracket(
                            startGrade, std::min(endGrade, lastGradeBar),
                            startGrade + ((std::min(endGrade, lastGradeBar) - startGrade) / 2),
                            label));
                        histogram->GetBarAxis().SetCustomLabel(
                            startGrade,
                            Wisteria::GraphItems::Label(
                                GetReadabilityMessageCatalog().GetGradeScaleLongLabel(startGrade)));
                        histogram->GetBarAxis().SetCustomLabel(
                            endGrade,
                            Wisteria::GraphItems::Label(
                                GetReadabilityMessageCatalog().GetGradeScaleLongLabel(endGrade)));
                    };

                    addGradesBracket(0.0, 4.0, _("Elementary School"));
                    addGradesBracket(5.0, 8.0, _("Middle School"));
                    addGradesBracket(9.0, 12.0, _("High School"));
                    addGradesBracket(13.0, 16.0, _("College"));
                    addGradesBracket(17.0, 18.0, _("Graduate School"));
                    addGradesBracket(19.0, 20.0, _("PhD"));
                    }
                }
            }
        wxGCDC gdc(view->GetDocFrame());
        canvas->CalcAllSizes(gdc);
        }
    else
        {
        // we are getting rid of this window (if it was included before)
        view->GetHistogramsView().RemoveWindowByIdAndLabel(Id, name);
        }
    }

//------------------------------------------------
bool BatchProjectDoc::RunProjectWizard(const wxString& path)
    {
    // Run through the project wizard
    auto* wizard =
        new ProjectWizardDlg(wxGetApp().GetParentingWindow(), ProjectType::BatchProject, path);
    if (wizard->ShowModal() != wxID_OK)
        {
        wizard->Destroy();
        return false;
        }

    SetProjectLanguage(wizard->GetLanguage());
    wxGetApp().GetAppOptions()->SetProjectLanguage(wizard->GetLanguage());

    // get readability options that were selected
    if (wizard->IsDocumentTypeSelected())
        {
        // general documents
        if (wizard->GetSelectedDocumentType() ==
            readability::document_classification::general_document)
            {
            for (auto& rTest : GetReadabilityTests().get_tests())
                {
                rTest.include(rTest.get_test().has_document_classification(
                                  readability::document_classification::general_document) &&
                              rTest.get_test().has_language(GetProjectLanguage()));
                }
            for (const auto& customWordTest : m_custom_word_tests)
                {
                if (customWordTest.has_document_classification(
                        readability::document_classification::general_document))
                    {
                    AddCustomReadabilityTest(wxString(customWordTest.get_name().c_str()));
                    }
                }
            }
        // technical manuals and documents
        else if (wizard->GetSelectedDocumentType() ==
                 readability::document_classification::technical_document)
            {
            for (auto& rTest : GetReadabilityTests().get_tests())
                {
                rTest.include(rTest.get_test().has_document_classification(
                                  readability::document_classification::technical_document) &&
                              rTest.get_test().has_language(GetProjectLanguage()));
                }
            for (const auto& customWordTest : m_custom_word_tests)
                {
                if (customWordTest.has_document_classification(
                        readability::document_classification::technical_document))
                    {
                    AddCustomReadabilityTest(wxString(customWordTest.get_name().c_str()));
                    }
                }
            }
        // short, terse forms
        else if (wizard->GetSelectedDocumentType() ==
                 readability::document_classification::nonnarrative_document)
            {
            // override how headers and lists are counted so that they are
            // always included if this is a form
            SetInvalidSentenceMethod(InvalidSentence::IncludeAsFullSentences);

            for (auto& rTest : GetReadabilityTests().get_tests())
                {
                rTest.include(rTest.get_test().has_document_classification(
                                  readability::document_classification::nonnarrative_document) &&
                              rTest.get_test().has_language(GetProjectLanguage()));
                }
            for (const auto& customWordTest : m_custom_word_tests)
                {
                if (customWordTest.has_document_classification(
                        readability::document_classification::nonnarrative_document))
                    {
                    AddCustomReadabilityTest(wxString(customWordTest.get_name().c_str()));
                    }
                }
            }
        else if (wizard->GetSelectedDocumentType() ==
                 readability::document_classification::adult_literature_document)
            {
            for (auto& rTest : GetReadabilityTests().get_tests())
                {
                rTest.include(
                    rTest.get_test().has_document_classification(
                        readability::document_classification::adult_literature_document) &&
                    rTest.get_test().has_language(GetProjectLanguage()));
                }
            for (const auto& customWordTest : m_custom_word_tests)
                {
                if (customWordTest.has_document_classification(
                        readability::document_classification::adult_literature_document))
                    {
                    AddCustomReadabilityTest(wxString(customWordTest.get_name().c_str()));
                    }
                }
            }
        else if (wizard->GetSelectedDocumentType() ==
                 readability::document_classification::childrens_literature_document)
            {
            for (auto& rTest : GetReadabilityTests().get_tests())
                {
                rTest.include(
                    rTest.get_test().has_document_classification(
                        readability::document_classification::childrens_literature_document) &&
                    rTest.get_test().has_language(GetProjectLanguage()));
                }
            IncludeDolchSightWords(GetProjectLanguage() ==
                                   readability::test_language::english_test);
            for (const auto& customWordTest : m_custom_word_tests)
                {
                if (customWordTest.has_document_classification(
                        readability::document_classification::childrens_literature_document))
                    {
                    AddCustomReadabilityTest(wxString(customWordTest.get_name().c_str()));
                    }
                }
            }
        wxGetApp().GetAppOptions()->SetTestByDocumentType(wizard->GetSelectedDocumentType());
        }
    // user selected the program to use recommended tests by industry
    else if (wizard->IsIndustrySelected())
        {
        if (wizard->GetSelectedIndustryType() ==
            readability::industry_classification::childrens_publishing_industry)
            {
            for (auto& rTest : GetReadabilityTests().get_tests())
                {
                rTest.include(
                    rTest.get_test().has_industry_classification(
                        readability::industry_classification::childrens_publishing_industry) &&
                    rTest.get_test().has_language(GetProjectLanguage()));
                }
            IncludeDolchSightWords(
                (GetProjectLanguage() == readability::test_language::english_test));
            for (const auto& customWordTest : m_custom_word_tests)
                {
                if (customWordTest.has_industry_classification(
                        readability::industry_classification::childrens_publishing_industry))
                    {
                    AddCustomReadabilityTest(wxString(customWordTest.get_name().c_str()));
                    }
                }
            }
        else if (wizard->GetSelectedIndustryType() ==
                 readability::industry_classification::adult_publishing_industry)
            {
            for (auto& rTest : GetReadabilityTests().get_tests())
                {
                rTest.include(
                    rTest.get_test().has_industry_classification(
                        readability::industry_classification::adult_publishing_industry) &&
                    rTest.get_test().has_language(GetProjectLanguage()));
                }
            for (const auto& customWordTest : m_custom_word_tests)
                {
                if (customWordTest.has_industry_classification(
                        readability::industry_classification::adult_publishing_industry))
                    {
                    AddCustomReadabilityTest(wxString(customWordTest.get_name().c_str()));
                    }
                }
            }
        else if (wizard->GetSelectedIndustryType() ==
                 readability::industry_classification::secondary_language_industry)
            {
            for (auto& rTest : GetReadabilityTests().get_tests())
                {
                rTest.include(
                    rTest.get_test().has_industry_classification(
                        readability::industry_classification::secondary_language_industry) &&
                    rTest.get_test().has_language(GetProjectLanguage()));
                }
            IncludeDolchSightWords(
                (GetProjectLanguage() == readability::test_language::english_test));
            for (const auto& customWordTest : m_custom_word_tests)
                {
                if (customWordTest.has_industry_classification(
                        readability::industry_classification::secondary_language_industry))
                    {
                    AddCustomReadabilityTest(wxString(customWordTest.get_name().c_str()));
                    }
                }
            }
        else if (wizard->GetSelectedIndustryType() ==
                 readability::industry_classification::childrens_healthcare_industry)
            {
            for (auto& rTest : GetReadabilityTests().get_tests())
                {
                rTest.include(
                    rTest.get_test().has_industry_classification(
                        readability::industry_classification::childrens_healthcare_industry) &&
                    rTest.get_test().has_language(GetProjectLanguage()));
                }
            for (const auto& customWordTest : m_custom_word_tests)
                {
                if (customWordTest.has_industry_classification(
                        readability::industry_classification::childrens_healthcare_industry))
                    {
                    AddCustomReadabilityTest(wxString(customWordTest.get_name().c_str()));
                    }
                }
            }
        else if (wizard->GetSelectedIndustryType() ==
                 readability::industry_classification::adult_healthcare_industry)
            {
            for (auto& rTest : GetReadabilityTests().get_tests())
                {
                rTest.include(
                    rTest.get_test().has_industry_classification(
                        readability::industry_classification::adult_healthcare_industry) &&
                    rTest.get_test().has_language(GetProjectLanguage()));
                }
            for (const auto& customWordTest : m_custom_word_tests)
                {
                if (customWordTest.has_industry_classification(
                        readability::industry_classification::adult_healthcare_industry))
                    {
                    AddCustomReadabilityTest(wxString(customWordTest.get_name().c_str()));
                    }
                }
            }
        else if (wizard->GetSelectedIndustryType() ==
                 readability::industry_classification::military_government_industry)
            {
            for (auto& rTest : GetReadabilityTests().get_tests())
                {
                rTest.include(
                    rTest.get_test().has_industry_classification(
                        readability::industry_classification::military_government_industry) &&
                    rTest.get_test().has_language(GetProjectLanguage()));
                }
            for (const auto& customWordTest : m_custom_word_tests)
                {
                if (customWordTest.has_industry_classification(
                        readability::industry_classification::military_government_industry))
                    {
                    AddCustomReadabilityTest(wxString(customWordTest.get_name().c_str()));
                    }
                }
            }
        else if (wizard->GetSelectedIndustryType() ==
                 readability::industry_classification::broadcasting_industry)
            {
            for (auto& rTest : GetReadabilityTests().get_tests())
                {
                rTest.include(rTest.get_test().has_industry_classification(
                                  readability::industry_classification::broadcasting_industry) &&
                              rTest.get_test().has_language(GetProjectLanguage()));
                }
            for (const auto& customWordTest : m_custom_word_tests)
                {
                if (customWordTest.has_industry_classification(
                        readability::industry_classification::broadcasting_industry))
                    {
                    AddCustomReadabilityTest(wxString(customWordTest.get_name().c_str()));
                    }
                }
            }
        wxGetApp().GetAppOptions()->SetTestByIndustry(wizard->GetSelectedIndustryType());
        }
    // user manually selected the readability test to run
    else if (wizard->IsManualTestSelected())
        {
        // manually selected standard tests
        SetReadabilityTests(wizard->GetReadabilityTestsInfo());
        for (auto& rTest : GetReadabilityTests().get_tests())
            {
            // turn off any selected tests that don't belong to the project's language
            if (!rTest.get_test().has_language(GetProjectLanguage()))
                {
                rTest.include(false);
                }
            }
        wxGetApp().GetAppOptions()->SetReadabilityTests(wizard->GetReadabilityTestsInfo());

        // Dolch
        if (wizard->GetLanguage() == readability::test_language::english_test)
            {
            IncludeDolchSightWords(wizard->IsDolchSelected());
            // Ignore whether this was checked or not if not English.
            // This way, if this project is non-English, then it won't affect
            // future English projects when they are being created.
            wxGetApp().GetAppOptions()->SetDolch(IsIncludingDolchSightWords());
            }
        else
            {
            IncludeDolchSightWords(false);
            }
        // Custom tests. See what was selected, look it up in the global list of test, and add
        // its unique test ID to the options manager's list of included custom tests.
        wxGetApp().GetAppOptions()->GetIncludedCustomTests().clear();
        const wxArrayInt selectedTestIndices = wizard->GetSelectedCustomTests();
        for (size_t i = 0; i < selectedTestIndices.Count(); ++i)
            {
            const CustomReadabilityTest& selectedTest =
                m_custom_word_tests[selectedTestIndices.Item(i)];
            AddCustomReadabilityTest(selectedTest.get_name().c_str());
            wxGetApp().GetAppOptions()->GetIncludedCustomTests().emplace_back(
                selectedTest.get_name().c_str());
            }
        }
    // user selected a test bundle
    else if (wizard->IsTestBundleSelected())
        {
        ApplyTestBundle(wizard->GetSelectedTestBundle());
        wxGetApp().GetAppOptions()->SetSelectedTestBundle(wizard->GetSelectedTestBundle());
        }
    // set parsing options based on how the user defined the structure of the document
    IgnoreBlankLinesForParagraphsParser(wizard->IsSplitLinesSelected());
    IgnoreIndentingForParagraphsParser(wizard->IsCenteredTextSelected());
    if ((wizard->IsSplitLinesSelected() || wizard->IsCenteredTextSelected()) &&
        !wizard->IsNewLinesAlwaysNewParagraphsSelected())
        {
        // also override paragraph ending logic if special format parsing is requested
        SetParagraphsParsingMethod(ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs);
        }
    else if (wizard->IsNewLinesAlwaysNewParagraphsSelected())
        {
        SetParagraphsParsingMethod(ParagraphParse::EachNewLineIsAParagraph);
        }
    // DOC files are difficult to detect lists in, so if they are not explicitly specifying
    // "fitted to the page" analysis for this project (above), then override the global
    // option and set it to treat all newlines as the end of a paragraph.
    else
        {
        m_adjustParagraphParserForDocFiles = true;
        }

    if (wizard->IsNarrativeSelected())
        {
        SetInvalidSentenceMethod(InvalidSentence::ExcludeFromAnalysis);
        }
    else if (wizard->IsFragmentedTextSelected())
        {
        // override how headers and lists are counted so that they are
        // always included if this is a form
        SetInvalidSentenceMethod(InvalidSentence::IncludeAsFullSentences);
        for (const auto& customWordTest : m_custom_word_tests)
            {
            if (customWordTest.has_document_classification(
                    readability::document_classification::nonnarrative_document))
                {
                AddCustomReadabilityTest(wxString(customWordTest.get_name().c_str()));
                }
            }
        }

    // grab the list of files and their (optional) descriptions
    GetSourceFilesInfo().clear();
    GetSourceFilesInfo().reserve(wizard->GetFileData()->GetItemCount());
    for (size_t i = 0; i < wizard->GetFileData()->GetItemCount(); ++i)
        {
        GetSourceFilesInfo().emplace_back(wizard->GetFileData()->GetItemText(i, 0),
                                          wizard->GetFileData()->GetItemText(i, 1));
        }
    // remove any duplicates
    std::sort(GetSourceFilesInfo().begin(), GetSourceFilesInfo().end());
    auto endOfUniquePos = std::unique(GetSourceFilesInfo().begin(), GetSourceFilesInfo().end());
    if (endOfUniquePos != GetSourceFilesInfo().end())
        {
        GetSourceFilesInfo().erase(endOfUniquePos, GetSourceFilesInfo().end());
        }
    // if using random subsampling
    if (wizard->IsRandomSampling())
        {
        const size_t sampleSize = GetSourceFilesInfo().size() *
                                  safe_divide<double>(wizard->GetRandomSamplePercentage(), 100);
        wxASSERT_MSG(sampleSize < GetSourceFilesInfo().size(), L"Invalid random sample size!");

        std::shuffle(GetSourceFilesInfo().begin(), GetSourceFilesInfo().end(),
                     wxGetApp().GetRandomNumberEngine());
        GetSourceFilesInfo().erase(GetSourceFilesInfo().begin() + sampleSize,
                                   GetSourceFilesInfo().end());
        wxASSERT_MSG(sampleSize == GetSourceFilesInfo().size(), L"Invalid random sample size!");

        wxGetApp().GetAppOptions()->EnableRandomSampling(true);
        wxGetApp().GetAppOptions()->SetBatchRandomSamplingSize(wizard->GetRandomSamplePercentage());
        }
    else
        {
        wxGetApp().GetAppOptions()->EnableRandomSampling(false);
        }
    SetMinDocWordCountForBatch(wizard->GetMinDocWordCountForBatch());
    wxGetApp().GetAppOptions()->SetMinDocWordCountForBatch(wizard->GetMinDocWordCountForBatch());

    wxGetApp().GetAppOptions()->SetTestRecommendation(
        wizard->IsDocumentTypeSelected() ? TestRecommendation::BasedOnDocumentType :
        wizard->IsIndustrySelected()     ? TestRecommendation::BasedOnIndustry :
        wizard->IsTestBundleSelected()   ? TestRecommendation::UseBundle :
                                           TestRecommendation::ManuallySelectTests);
    wxGetApp().GetAppOptions()->SaveOptionsFile();
    wizard->Destroy();
    return true;
    }

//-------------------------------------------------------
bool BatchProjectDoc::OnSaveDocument(const wxString& filename)
    {
    if (!IsSafeToUpdate())
        {
        return false;
        }

    if (!GetFilename().empty() && GetFilename() != filename)
        {
        // must be coming from Save As, so make sure file isn't locked
        try
            {
            const MemoryMappedFile sourceFile(filename);
            }
        catch (const MemoryMappedFileShareViolationException&)
            {
            LogMessage(_(L"File appears to be open by another application. Cannot save project."),
                       _(L"Project Save"), wxOK | wxICON_EXCLAMATION);
            return false;
            }
        // don't care about the file being empty or whatever, just if it's locked
        catch (...)
            {
            }
        m_FileReadOnly = false;
        m_File.Close();
        }

    // if we opened earlier in read only mode then bail
    if (m_FileReadOnly)
        {
        LogMessage(_(L"Project file was opened as read only. Unable to save."), _(L"Project Save"),
                   wxOK | wxICON_INFORMATION);
        return false;
        }

    if (!m_File.IsOpened())
        {
        // if the file is already there, and it is in use, then fail
        if (!m_File.Open(filename, wxFile::write))
            {
            m_FileReadOnly = true;
            LogMessage(_(L"File appears to be open by another application. Cannot save project."),
                       _(L"Project Save"), wxOK | wxICON_EXCLAMATION);
            return false;
            }
        }

    SetFilename(filename, true);
    SetTitle(ParseTitleFromFileName(filename));

    /* Write the (zip file) project out to a temp file first, then swap it.
       This helps us from corrupting the original file if something goes wrong
       during the write process.*/
    wxTempFileOutputStream out(filename);
    wxZipOutputStream zip(out, 9 /*Maximum compression*/);

    // settings.xml
    Wisteria::ZipCatalog::WriteText(zip, ProjectSettingsFileLabel(), FormatProjectSettings());

    // if storing indexed text, then include it
    if (GetDocumentStorageMethod() == TextStorage::EmbedText)
        {
        int counter{ 1 };
        wxProgressDialog progressDlg(wxString::Format(_(L"Saving \"%s\""), GetTitle()), wxString{},
                                     static_cast<int>(m_docs.size()), nullptr,
                                     wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_CAN_ABORT |
                                         wxPD_APP_MODAL);
        progressDlg.Centre();
        /* Use buffered output stream, NOT text output stream. Text output buffer
           messes around with the newlines in the text, whereas buffer streams preserve the text.*/
        for (auto pos = m_docs.begin(); pos != m_docs.end(); ++pos)
            {
            Wisteria::ZipCatalog::WriteText(
                zip, wxString::Format(L"Content%zu.txt", pos - m_docs.begin()),
                (*pos)->GetDocumentText());

            if (!progressDlg.Update(counter++))
                {
                zip.Close();
                m_File.Close();
                return false;
                }
            }
        }
    zip.Close();

    // close the project, replace it with the temp file, and (re)lock it
    m_File.Close();
    if (!out.Commit())
        {
        LogMessage(_(L"Unable to save project file. File may be locked by another process."),
                   _(L"Project Error"), wxOK | wxICON_EXCLAMATION);
        return false;
        }
    if (!LockProjectFile())
        {
        return false;
        }

    Modify(false);
    SetDocumentSaved(true);
    return true;
    }

//-------------------------------------------------------
bool BatchProjectDoc::OnOpenDocument(const wxString& filename)
    {
    wxLogMessage(L"Opening project \"%s\"", filename);
    // make sure there aren't any projects getting updated before we start opening a new one.
    // opening a project may try to add new custom tests, which would cause a race condition with
    // the processing project
    const wxList docs = wxGetApp().GetDocManager()->GetDocuments();
    for (size_t i = 0; i < docs.GetCount(); ++i)
        {
        const auto* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
        if (doc != nullptr && !doc->IsSafeToUpdate())
            {
            return false;
            }
        }

    const wxBusyCursor wait;

    // make sure the file exists first
    if (!wxFile::Exists(filename))
        {
        LogMessage(
            wxString::Format(_(L"'%s': unable to open project file. File not found."), filename),
            _(L"Error"), wxOK | wxICON_ERROR);
        return false;
        }
    if (!OnSaveModified())
        {
        return false;
        }

    SetTitle(ParseTitleFromFileName(filename));
    SetFilename(filename, true);
    Modify(false);
    SetDocumentSaved(true);

    const BaseProjectProcessingLock processingLock(this);

    MemoryMappedFile sourceFile;
    try
        {
        sourceFile.MapFile(GetFilename());
        const char* projectFileText = static_cast<char*>(sourceFile.GetStream());
        LoadProjectFile(projectFileText, sourceFile.GetMapSize());
        // unmap and lock the file while project is open
        sourceFile.UnmapFile();
        if (!LockProjectFile())
            {
            return false;
            }
        }
    catch (const MemoryMappedFileShareViolationException&)
        {
        /* Couldn't get a map of it (might be open in another process),
           so try to buffer it.*/
        if (m_File.Open(GetFilename(), wxFile::read))
            {
            m_FileReadOnly = true;
            LogMessage(_(L"File appears to be open by another application. "
                         "Project file will be opened as read only."),
                       _(L"Project Open"), wxOK | wxICON_INFORMATION);
            }
        else
            {
            LogMessage(wxString::Format(_(L"'%s': unable to open project file."), GetFilename()),
                       _(L"Project Open"), wxOK | wxICON_EXCLAMATION);
            return false;
            }
        std::string projectFileText;
        projectFileText.resize(m_File.Length());

        m_File.Seek(0);
        const size_t readSize = m_File.Read(projectFileText.data(), projectFileText.size());
        LoadProjectFile(projectFileText.data(), readSize);
        }
    catch (const MemoryMappedFileEmptyException&)
        {
        LogMessage(_(L"Invalid project file. File is empty."), _(L"Project Open"),
                   wxOK | wxICON_EXCLAMATION);
        return false;
        }
    catch (const MemoryMappedInvalidFileType&)
        {
        LogMessage(_(L"Invalid file."), _(L"Project Open"), wxOK | wxICON_EXCLAMATION);
        return false;
        }
    catch (const MemoryMappedFileCloudFileError&)
        {
        LogMessage(_(L"Unable to open file from Cloud service."), _(L"Project Open"),
                   wxOK | wxICON_EXCLAMATION);
        return false;
        }
    catch (const MemoryMappedFileException&)
        {
        // Couldn't get a map of the file for unknown reason, so buffer it.
        if (!LockProjectFile())
            {
            return false;
            }
        std::string projectFileText;
        projectFileText.resize(m_File.Length());

        m_File.Seek(0);
        const size_t readSize = m_File.Read(projectFileText.data(), projectFileText.size());
        LoadProjectFile(projectFileText.data(), readSize);
        }
    catch (...)
        {
        LogMessage(wxString::Format(_(L"'%s': unable to open project file."), GetFilename()),
                   _(L"Project Open"), wxOK | wxICON_EXCLAMATION);
        return false;
        }

    wxProgressDialog progressDlg(
        wxString::Format(_(L"Opening \"%s\""), GetTitle()),
        wxString::Format(
            _(L"Analyzing %s documents..."),
            wxNumberFormatter::ToString(GetSourceFilesInfo().size(), 0,
                                        wxNumberFormatter::Style::Style_WithThousandsSep)),
        static_cast<int>(GetSourceFilesInfo().size() + 13), nullptr,
        wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_ELAPSED_TIME | wxPD_CAN_ABORT | wxPD_APP_MODAL);
    progressDlg.Centre();
    int counter{ 1 };

    // If externally linking to the documents, then reset document collection.
    // Note that if the documents are embedded, then they would already be
    // initialized and loaded from the project file.
    if (GetDocumentStorageMethod() == TextStorage::NoEmbedText)
        {
        InitializeDocuments();
        }

    if (!progressDlg.Update(counter++))
        {
        return false;
        }
    if (!LoadDocuments(progressDlg))
        {
        return false;
        }
    counter = progressDlg.GetValue();

    LoadGroupingLabelsFromDocumentsInfo();

    // prompt user about removing any failed documents.
    // If they request to leave them in, then load any warnings for all documents.
    if (!CheckForFailedDocuments())
        {
        LoadWarningsSection();
        }

    if (!progressDlg.Update(counter++, _(L"Loading Dolch statistics...")))
        {
        return false;
        }
    LoadDolchSection();

    if (!progressDlg.Update(counter++, _(L"Loading difficult words...")))
        {
        return false;
        }
    LoadHardWordsSection();

    if (!progressDlg.Update(counter++, _(L"Loading graphs...")))
        {
        return false;
        }
    DisplayReadabilityGraphs();

    if (!progressDlg.Update(counter++, _(L"Loading scores...")))
        {
        return false;
        }
    LoadScoresSection();

    if (!progressDlg.Update(counter++, _(L"Loading summary statistics...")))
        {
        return false;
        }
    LoadSummaryStatsSection();

    if (!progressDlg.Update(counter++))
        {
        return false;
        }
    DisplayScores();

    if (!progressDlg.Update(counter++))
        {
        return false;
        }
    DisplayBoxPlots();

    if (!progressDlg.Update(counter++))
        {
        return false;
        }
    DisplayHistograms();

    if (!progressDlg.Update(counter++, _(L"Loading grammar information...")))
        {
        return false;
        }
    DisplayGrammar();

    if (!progressDlg.Update(counter++))
        {
        return false;
        }
    DisplayHardWords();
    DisplaySentencesBreakdown();
    DisplaySummaryStats();

    if (!progressDlg.Update(counter++))
        {
        return false;
        }
    DisplaySightWords();

    if (!progressDlg.Update(counter++))
        {
        return false;
        }
    DisplayWarnings();

    auto* view = dynamic_cast<BatchProjectView*>(GetFirstView());
    view->UpdateSideBarIcons();
    view->UpdateRibbonState();
    view->Present();
    UpdateAllViews();

    view->GetSideBar()->SelectSubItem(
        view->GetSideBar()->FindSubItem(BatchProjectView::ID_SCORE_LIST_PAGE_ID));
    auto* scoresWindow = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        view->GetScoresView().FindWindowById(BaseProjectView::ID_SCORE_LIST_PAGE_ID));
    if (scoresWindow != nullptr && scoresWindow->GetItemCount() > 0)
        {
        scoresWindow->Select(0);
        }

    return true;
    }

//-------------------------------------------------------
void BatchProjectDoc::LoadProjectFile(const char* projectFileText, const size_t textLength)
    {
    const Wisteria::ZipCatalog cat(projectFileText, textLength);

    // open the project settings file
    const std::wstring settingsFile = cat.ReadTextFile(ProjectSettingsFileLabel());
    if (!settingsFile.empty())
        {
        LoadSettingsFile(settingsFile.c_str());
        }
    else
        {
        LogMessage(_(L"Settings file could not be found in the project file. "
                     "Default settings will be used."),
                   wxGetApp().GetAppName(), wxOK | wxICON_INFORMATION);
        }

    InitializeDocuments();

    // if storing indexed text then read that in from the project file
    // and assign it to respective documents.
    if (GetDocumentStorageMethod() == TextStorage::EmbedText)
        {
        for (auto pos = m_docs.begin(); pos != m_docs.end(); ++pos)
            {
            (*pos)->SetDocumentText(
                cat.ReadTextFile(wxString::Format(_DT(L"Content%zu.txt"), pos - m_docs.begin())));
            }
        }
    }

//-------------------------------------------------------
void BatchProjectDoc::DisplayGrammar()
    {
    PROFILE();
    auto* view = dynamic_cast<BatchProjectView*>(GetFirstView());
    assert(view);

    // Wording Errors
    auto* listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        view->GetGrammarView().FindWindowById(BaseProjectView::WORDING_ERRORS_LIST_PAGE_ID));
    if (GetGrammarInfo().IsWordingErrorsEnabled() && (m_wordingErrorData->GetItemCount() != 0U))
        {
        if (listView == nullptr)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::WORDING_ERRORS_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetPhrasingErrorsTabLabel());
            listView->SetName(BaseProjectView::GetPhrasingErrorsTabLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->InsertColumn(0, _(L"Document"));
            listView->InsertColumn(1, _(L"Label"));
            listView->InsertColumn(2, _(L"Frequency"));
            listView->InsertColumn(3, BaseProjectView::GetPhrasingErrorsTabLabel());
            listView->InsertColumn(4, _(L"Suggestions"));
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
            view->GetGrammarView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_wordingErrorData);
        listView->SetVirtualDataSize(m_wordingErrorData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(
            1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(
            3, std::min(listView->EstimateColumnWidth(3), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(
            4, std::min(listView->EstimateColumnWidth(4), view->GetMaxColumnWidth()));
        if (listView->GetSortedColumn() == -1)
            {
            listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
            }
        else
            {
            listView->Resort();
            }
        }
    else
        {
        view->GetGrammarView().RemoveWindowById(BaseProjectView::WORDING_ERRORS_LIST_PAGE_ID);
        }

    // Misspelled words
    listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        view->GetGrammarView().FindWindowById(BaseProjectView::MISSPELLED_WORD_LIST_PAGE_ID));
    if (GetGrammarInfo().IsMisspellingsEnabled() && (GetMisspelledWordData()->GetItemCount() != 0U))
        {
        if (listView == nullptr)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::MISSPELLED_WORD_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetMisspellingsLabel());
            listView->SetName(BaseProjectView::GetMisspellingsLabel());
            listView->EnableGridLines();
            listView->InsertColumn(0, _(L"Document"));
            listView->InsertColumn(1, _(L"Label"));
            listView->InsertColumn(2, _(L"Frequency"));
            listView->InsertColumn(3, _(L"Unique Count"));
            listView->InsertColumn(4, BaseProjectView::GetMisspellingsLabel());
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
            view->GetGrammarView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(GetMisspelledWordData());
        listView->SetVirtualDataSize(GetMisspelledWordData()->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(
            1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(3, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(
            4, std::min(listView->EstimateColumnWidth(4), view->GetMaxColumnWidth()));
        if (listView->GetSortedColumn() == -1)
            {
            listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
            }
        else
            {
            listView->Resort();
            }
        }
    else
        {
        view->GetGrammarView().RemoveWindowById(BaseProjectView::MISSPELLED_WORD_LIST_PAGE_ID);
        }

    // Repeated words
    listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        view->GetGrammarView().FindWindowById(BaseProjectView::DUPLICATES_LIST_PAGE_ID));
    if (GetGrammarInfo().IsRepeatedWordsEnabled() && (GetRepeatedWordData()->GetItemCount() != 0U))
        {
        if (listView == nullptr)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::DUPLICATES_LIST_PAGE_ID, wxDefaultPosition,
                wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetRepeatedWordsLabel());
            listView->SetName(BaseProjectView::GetRepeatedWordsLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->InsertColumn(0, _(L"Document"));
            listView->InsertColumn(1, _(L"Label"));
            listView->InsertColumn(2, _(L"Frequency"));
            listView->InsertColumn(3, _(L"Repeated Words"));
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
            view->GetGrammarView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(GetRepeatedWordData());
        listView->SetVirtualDataSize(GetRepeatedWordData()->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(
            1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(
            3, std::min(listView->EstimateColumnWidth(3), view->GetMaxColumnWidth()));
        if (listView->GetSortedColumn() == -1)
            {
            listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
            }
        else
            {
            listView->Resort();
            }
        }
    else
        {
        view->GetGrammarView().RemoveWindowById(BaseProjectView::DUPLICATES_LIST_PAGE_ID);
        }

    // Incorrect articles
    listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        view->GetGrammarView().FindWindowById(BaseProjectView::INCORRECT_ARTICLE_PAGE_ID));
    if (GetGrammarInfo().IsArticleMismatchesEnabled() &&
        (m_incorrectArticleData->GetItemCount() != 0U))
        {
        if (listView == nullptr)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::INCORRECT_ARTICLE_PAGE_ID, wxDefaultPosition,
                wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetArticleMismatchesLabel());
            listView->SetName(BaseProjectView::GetArticleMismatchesLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->InsertColumn(0, _(L"Document"));
            listView->InsertColumn(1, _(L"Label"));
            listView->InsertColumn(2, _(L"Frequency"));
            listView->InsertColumn(3, BaseProjectView::GetArticleMismatchesLabel());
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
            view->GetGrammarView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_incorrectArticleData);
        listView->SetVirtualDataSize(m_incorrectArticleData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(
            1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(
            3, std::min(listView->EstimateColumnWidth(3), view->GetMaxColumnWidth()));
        if (listView->GetSortedColumn() == -1)
            {
            listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
            }
        else
            {
            listView->Resort();
            }
        }
    else
        {
        view->GetGrammarView().RemoveWindowById(BaseProjectView::INCORRECT_ARTICLE_PAGE_ID);
        }

    // redundant phrases
    listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        view->GetGrammarView().FindWindowById(BaseProjectView::REDUNDANT_PHRASE_LIST_PAGE_ID));
    if (GetGrammarInfo().IsRedundantPhrasesEnabled() &&
        (m_redundantPhraseData->GetItemCount() != 0U))
        {
        if (listView == nullptr)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::REDUNDANT_PHRASE_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetRedundantPhrasesTabLabel());
            listView->SetName(BaseProjectView::GetRedundantPhrasesTabLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->InsertColumn(0, _(L"Document"));
            listView->InsertColumn(1, _(L"Label"));
            listView->InsertColumn(2, _(L"Frequency"));
            listView->InsertColumn(3, BaseProjectView::GetRedundantPhrasesTabLabel());
            listView->InsertColumn(4, _(L"Suggestions"));
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
            view->GetGrammarView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_redundantPhraseData);
        listView->SetVirtualDataSize(m_redundantPhraseData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(
            1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(
            3, std::min(listView->EstimateColumnWidth(3), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(
            4, std::min(listView->EstimateColumnWidth(4), view->GetMaxColumnWidth()));
        if (listView->GetSortedColumn() == -1)
            {
            listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
            }
        else
            {
            listView->Resort();
            }
        }
    else
        {
        view->GetGrammarView().RemoveWindowById(BaseProjectView::REDUNDANT_PHRASE_LIST_PAGE_ID);
        }

    // overused words (by sentence)
    listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetGrammarView().FindWindowById(
        BaseProjectView::OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID));
    if (GetGrammarInfo().IsOverUsedWordsBySentenceEnabled() &&
        (m_overusedWordBySentenceData->GetItemCount() != 0U))
        {
        if (listView == nullptr)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetOverusedWordsBySentenceLabel());
            listView->SetName(BaseProjectView::GetOverusedWordsBySentenceLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->InsertColumn(0, _(L"Document"));
            listView->InsertColumn(1, _(L"Label"));
            listView->InsertColumn(2, _(L"Frequency"));
            listView->InsertColumn(3, _(L"Overused Words"));
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
            view->GetGrammarView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_overusedWordBySentenceData);
        listView->SetVirtualDataSize(m_overusedWordBySentenceData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(
            1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(
            3, std::min(listView->EstimateColumnWidth(3), view->GetMaxColumnWidth()));
        if (listView->GetSortedColumn() == -1)
            {
            listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
            }
        else
            {
            listView->Resort();
            }
        }
    else
        {
        view->GetGrammarView().RemoveWindowById(
            BaseProjectView::OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID);
        }

    // wordy items
    listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        view->GetGrammarView().FindWindowById(BaseProjectView::WORDY_PHRASES_LIST_PAGE_ID));
    if (GetGrammarInfo().IsWordyPhrasesEnabled() && (m_wordyPhraseData->GetItemCount() != 0U))
        {
        if (listView == nullptr)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::WORDY_PHRASES_LIST_PAGE_ID, wxDefaultPosition,
                wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetWordyPhrasesTabLabel());
            listView->SetName(BaseProjectView::GetWordyPhrasesTabLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->InsertColumn(0, _(L"Document"));
            listView->InsertColumn(1, _(L"Label"));
            listView->InsertColumn(2, _(L"Frequency"));
            listView->InsertColumn(3, BaseProjectView::GetWordyPhrasesTabLabel());
            listView->InsertColumn(4, _(L"Suggestions"));
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
            view->GetGrammarView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_wordyPhraseData);
        listView->SetVirtualDataSize(m_wordyPhraseData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(
            1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(
            3, std::min(listView->EstimateColumnWidth(3), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(
            4, std::min(listView->EstimateColumnWidth(4), view->GetMaxColumnWidth()));
        if (listView->GetSortedColumn() == -1)
            {
            listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
            }
        else
            {
            listView->Resort();
            }
        }
    else
        {
        view->GetGrammarView().RemoveWindowById(BaseProjectView::WORDY_PHRASES_LIST_PAGE_ID);
        }

    // cliches
    listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        view->GetGrammarView().FindWindowById(BaseProjectView::CLICHES_LIST_PAGE_ID));
    if (GetGrammarInfo().IsClichesEnabled() && (m_clichePhraseData->GetItemCount() != 0U))
        {
        if (listView == nullptr)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::CLICHES_LIST_PAGE_ID, wxDefaultPosition,
                wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetClichesTabLabel());
            listView->SetName(BaseProjectView::GetClichesTabLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->InsertColumn(0, _(L"Document"));
            listView->InsertColumn(1, _(L"Label"));
            listView->InsertColumn(2, _(L"Frequency"));
            listView->InsertColumn(3, BaseProjectView::GetClichesTabLabel());
            listView->InsertColumn(4, _(L"Explanations/Suggestions"));
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
            view->GetGrammarView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_clichePhraseData);
        listView->SetVirtualDataSize(m_clichePhraseData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(
            1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(
            3, std::min(listView->EstimateColumnWidth(3), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(
            4, std::min(listView->EstimateColumnWidth(4), view->GetMaxColumnWidth()));
        if (listView->GetSortedColumn() == -1)
            {
            listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
            }
        else
            {
            listView->Resort();
            }
        }
    else
        {
        view->GetGrammarView().RemoveWindowById(BaseProjectView::CLICHES_LIST_PAGE_ID);
        }

    // Passive voice
    listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        view->GetGrammarView().FindWindowById(BaseProjectView::PASSIVE_VOICE_PAGE_ID));
    if (GetGrammarInfo().IsPassiveVoiceEnabled() && (m_passiveVoiceData->GetItemCount() != 0U))
        {
        if (listView == nullptr)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::PASSIVE_VOICE_PAGE_ID, wxDefaultPosition,
                wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetPassiveLabel());
            listView->SetName(BaseProjectView::GetPassiveLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->InsertColumn(0, _(L"Document"));
            listView->InsertColumn(1, _(L"Label"));
            listView->InsertColumn(2, _(L"Frequency"));
            listView->InsertColumn(3, BaseProjectView::GetPassiveLabel());
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
            view->GetGrammarView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_passiveVoiceData);
        listView->SetVirtualDataSize(m_passiveVoiceData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(
            1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(
            3, std::min(listView->EstimateColumnWidth(3), view->GetMaxColumnWidth()));
        if (listView->GetSortedColumn() == -1)
            {
            listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
            }
        else
            {
            listView->Resort();
            }
        }
    else
        {
        view->GetGrammarView().RemoveWindowById(BaseProjectView::PASSIVE_VOICE_PAGE_ID);
        }

    // sentences that begin with conjunctions
    listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetGrammarView().FindWindowById(
        BaseProjectView::SENTENCES_CONJUNCTION_START_LIST_PAGE_ID));
    if (GetGrammarInfo().IsConjunctionStartingSentencesEnabled() &&
        (m_sentenceStartingWithConjunctionsData->GetItemCount() != 0U))
        {
        if (listView == nullptr)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::SENTENCES_CONJUNCTION_START_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetSentenceStartingWithConjunctionsTabLabel());
            listView->SetName(BaseProjectView::GetSentenceStartingWithConjunctionsTabLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->InsertColumn(0, _(L"Document"));
            listView->InsertColumn(1, _(L"Label"));
            listView->InsertColumn(2, _(L"Frequency"));
            listView->InsertColumn(3, _(L"Conjunctions"));
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
            view->GetGrammarView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_sentenceStartingWithConjunctionsData);
        listView->SetVirtualDataSize(m_sentenceStartingWithConjunctionsData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(
            1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(
            3, std::min(listView->EstimateColumnWidth(3), view->GetMaxColumnWidth()));
        if (listView->GetSortedColumn() == -1)
            {
            listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
            }
        else
            {
            listView->Resort();
            }
        }
    else
        {
        view->GetGrammarView().RemoveWindowById(
            BaseProjectView::SENTENCES_CONJUNCTION_START_LIST_PAGE_ID);
        }

    // sentences that begin with lowercased words
    listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetGrammarView().FindWindowById(
        BaseProjectView::SENTENCES_LOWERCASE_START_LIST_PAGE_ID));
    if (GetGrammarInfo().IsLowercaseSentencesEnabled() &&
        (m_sentenceStartingWithLowercaseData->GetItemCount() != 0U))
        {
        if (listView == nullptr)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::SENTENCES_LOWERCASE_START_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetSentenceStartingWithLowercaseTabLabel());
            listView->SetName(BaseProjectView::GetSentenceStartingWithLowercaseTabLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->InsertColumn(0, _(L"Document"));
            listView->InsertColumn(1, _(L"Label"));
            listView->InsertColumn(2, _(L"Frequency"));
            // TRANSLATORS: First word in a sentence.
            listView->InsertColumn(3, _(L"Starting Word"));
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
            view->GetGrammarView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_sentenceStartingWithLowercaseData);
        listView->SetVirtualDataSize(m_sentenceStartingWithLowercaseData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(
            1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(
            3, std::min(listView->EstimateColumnWidth(3), view->GetMaxColumnWidth()));
        if (listView->GetSortedColumn() == -1)
            {
            listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
            }
        else
            {
            listView->Resort();
            }
        }
    else
        {
        view->GetGrammarView().RemoveWindowById(
            BaseProjectView::SENTENCES_LOWERCASE_START_LIST_PAGE_ID);
        }
    }

//-------------------------------------------------------
void BatchProjectDoc::DisplaySummaryStats()
    {
    auto* view = dynamic_cast<BatchProjectView*>(GetFirstView());
    // summary stats
    auto* listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
        view->GetSummaryStatsView().FindWindowById(BaseProjectView::STATS_LIST_PAGE_ID));
    if ((m_summaryStatsData->GetItemCount() != 0U) && GetStatisticsInfo().IsTableEnabled() &&
        GetStatisticsReportInfo().HasStatisticsEnabled())
        {
        if (listView == nullptr)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::STATS_LIST_PAGE_ID, wxDefaultPosition,
                wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetSummaryStatisticsLabel());
            listView->SetName(BaseProjectView::GetSummaryStatisticsLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
            view->GetSummaryStatsView().AddWindow(listView);
            }
        listView->DeleteAllColumns();
        for (size_t i = 0; i < m_summaryStatsColumnNames.size(); ++i)
            {
            listView->InsertColumn(i, m_summaryStatsColumnNames[i]);
            }

        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_summaryStatsData);
        listView->SetVirtualDataSize(m_summaryStatsData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(
            1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        for (size_t i = 2; i < m_summaryStatsColumnNames.size(); ++i)
            {
            listView->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
            }

        if (listView->GetSortedColumn() == -1)
            {
            listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
            }
        else
            {
            listView->Resort();
            }
        }
    else
        {
        view->GetSummaryStatsView().RemoveWindowById(BaseProjectView::STATS_LIST_PAGE_ID);
        }
    }

//-------------------------------------------------------
void BatchProjectDoc::DisplaySentencesBreakdown()
    {
    auto* view = dynamic_cast<BatchProjectView*>(GetFirstView());
    // long sentences
    auto* listView =
        dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetSentencesBreakdownView().FindWindowById(
            BaseProjectView::LONG_SENTENCES_LIST_PAGE_ID));
    if (m_overlyLongSentenceData->GetItemCount() != 0U)
        {
        if (listView == nullptr)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::LONG_SENTENCES_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetLongSentencesLabel());
            listView->SetName(BaseProjectView::GetLongSentencesLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->InsertColumn(0, _(L"Document"));
            listView->InsertColumn(1, _(L"Label"));
            listView->InsertColumn(2, _(L"Overly-long Sentences"));
            listView->InsertColumn(3, _(L"Longest Sentence Length"));
            listView->InsertColumn(4, _(L"Longest Sentence"));
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
            view->GetSentencesBreakdownView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_overlyLongSentenceData);
        listView->SetVirtualDataSize(m_overlyLongSentenceData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(
            1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        listView->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(3, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(4, listView->EstimateColumnWidth(3));
        if (listView->GetSortedColumn() == -1)
            {
            listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
            }
        else
            {
            listView->Resort();
            }
        }
    else
        {
        view->GetSentencesBreakdownView().RemoveWindowById(
            BaseProjectView::LONG_SENTENCES_LIST_PAGE_ID);
        }
    }

//-------------------------------------------------------
void BatchProjectDoc::DisplayHardWords()
    {
    PROFILE();
    auto* view = dynamic_cast<BatchProjectView*>(GetFirstView());

    // Difficult words
    if (m_hardWordsData->GetItemCount() != 0U)
        {
        auto* listView =
            dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetWordsBreakdownView().FindWindowById(
                BaseProjectView::ID_DIFFICULT_WORDS_LIST_PAGE_ID));
        if (listView == nullptr)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::ID_DIFFICULT_WORDS_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetDifficultWordsLabel());
            listView->SetName(BaseProjectView::GetDifficultWordsLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
            view->GetWordsBreakdownView().InsertWindow(0, listView);
            }
        listView->DeleteAllColumns();
        long columnIndex = 0;
        listView->InsertColumn(columnIndex++, _(L"Document"));
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->InsertColumn(columnIndex++, _(L"Label"));
        listView->InsertColumn(columnIndex++, _(L"Total Words"));
        listView->InsertColumn(columnIndex++,
                               /* xgettext:no-c-format */ _(L"% of complex (3+ syllable) words"));
        listView->InsertColumn(columnIndex++, _(L"Complex (3+ syllable) words"));
        listView->InsertColumn(columnIndex++,
                               /* xgettext:no-c-format */ _(L"% of long (6+ characters) words"));
        listView->InsertColumn(columnIndex++, _(L"Long (6+ characters) words"));
        if (GetStatisticsReportInfo().IsExtendedWordsEnabled())
            {
            if (IsSmogLikeTestIncluded())
                {
                listView->InsertColumn(columnIndex++,
                                       /* xgettext:no-c-format */ _(L"% of SMOG hard words"));
                listView->InsertColumn(columnIndex++, _(L"SMOG hard words"));
                }
            if (GetReadabilityTests().is_test_included(ReadabilityMessages::GUNNING_FOG()))
                {
                listView->InsertColumn(columnIndex++,
                                       /* xgettext:no-c-format */ _(L"% of Fog hard words"));
                listView->InsertColumn(columnIndex++, _(L"Fog hard words"));
                }
            if (IsDaleChallLikeTestIncluded())
                {
                listView->InsertColumn(
                    columnIndex++,
                    wxString::Format(_(L"%% of %s unfamiliar words"), _DT(L"Dale-Chall")));
                listView->InsertColumn(columnIndex++, _(L"Dale-Chall unfamiliar words"));
                }
            if (GetReadabilityTests().is_test_included(ReadabilityMessages::SPACHE()))
                {
                listView->InsertColumn(
                    columnIndex++,
                    wxString::Format(_(L"%% of %s unfamiliar words"), _DT(L"Spache")));
                listView->InsertColumn(columnIndex++, _(L"Spache unfamiliar words"));
                }
            if (GetReadabilityTests().is_test_included(ReadabilityMessages::HARRIS_JACOBSON()))
                {
                listView->InsertColumn(
                    columnIndex++, wxString::Format(_(L"%% of %s unfamiliar words"), _DT(L"HJ")));
                listView->InsertColumn(columnIndex++, _(L"HJ unfamiliar words"));
                }
            for (const auto& cTestInUse : GetCustTestsInUse())
                {
                if (!cTestInUse.GetIterator()->is_using_familiar_words())
                    {
                    continue;
                    }
                listView->InsertColumn(
                    columnIndex++, wxString::Format(_(L"%% of %s unfamiliar words"),
                                                    cTestInUse.GetIterator()->get_name().c_str()));
                listView->InsertColumn(
                    columnIndex++, wxString::Format(_(L"%s unfamiliar words"),
                                                    cTestInUse.GetIterator()->get_name().c_str()));
                }
            }
        UpdateListOptions(listView);
        listView->SetVirtualDataProvider(m_hardWordsData);
        listView->SetVirtualDataSize(m_hardWordsData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(
            1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        for (long i = 2; i < listView->GetColumnCount(); ++i)
            {
            listView->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
            }
        if (listView->GetSortedColumn() == -1)
            {
            listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
            }
        else
            {
            listView->Resort();
            }
        }
    else
        {
        view->GetWordsBreakdownView().RemoveWindowById(
            BaseProjectView::ID_DIFFICULT_WORDS_LIST_PAGE_ID);
        }

    // All words
    if (GetAllWordsBatchData()->GetItemCount() != 0U)
        {
        auto* listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
            view->GetWordsBreakdownView().FindWindowById(BaseProjectView::ALL_WORDS_LIST_PAGE_ID));
        if (listView == nullptr)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::ALL_WORDS_LIST_PAGE_ID, wxDefaultPosition,
                wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetAllWordsLabel());
            listView->SetName(BaseProjectView::GetAllWordsLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->InsertColumn(0, _(L"Word"));
            listView->InsertColumn(1, _(L"Frequency"));
            listView->InsertColumn(2, _(L"Document Count"));
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
            view->GetWordsBreakdownView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetVirtualDataProvider(GetAllWordsBatchData());
        listView->SetVirtualDataSize(GetAllWordsBatchData()->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
        listView->SetColumnWidth(2, wxLIST_AUTOSIZE_USEHEADER);
        if (listView->GetSortedColumn() == -1)
            {
            listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
            }
        else
            {
            listView->Resort();
            }
        }
    else
        {
        view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::ALL_WORDS_LIST_PAGE_ID);
        }

    // keywords (uncommon words removed, remaining stemmed and combined)
    if ((GetKeyWordsBatchData()->GetItemCount() != 0U) &&
        // don't bother with condensed list if it has the same item count as the all words list
        // (that would mean that there was no condensing [stemming] that took place and that
        // these lists are the same).
        (GetKeyWordsBatchData()->GetItemCount() != GetAllWordsBatchData()->GetItemCount()))
        {
        auto* listView =
            dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetWordsBreakdownView().FindWindowById(
                BaseProjectView::ALL_WORDS_CONDENSED_LIST_PAGE_ID));
        if (listView == nullptr)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::ALL_WORDS_CONDENSED_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetKeyWordsLabel());
            listView->SetName(BaseProjectView::GetKeyWordsLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->InsertColumn(0, _(L"Word"));
            listView->InsertColumn(1, _(L"Frequency"));
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
            view->GetWordsBreakdownView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetVirtualDataProvider(GetKeyWordsBatchData());
        listView->SetVirtualDataSize(GetKeyWordsBatchData()->GetItemCount());
        listView->DistributeColumns();
        if (listView->GetSortedColumn() == -1)
            {
            listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
            }
        else
            {
            listView->Resort();
            }
        }
    else
        {
        // we are getting rid of this window (if nothing in it)
        view->GetWordsBreakdownView().RemoveWindowById(
            BaseProjectView::ALL_WORDS_CONDENSED_LIST_PAGE_ID);
        }

    // word cloud
    wxGCDC gdc(view->GetDocFrame());
    auto* wordCloudCanvas = dynamic_cast<Wisteria::Canvas*>(
        view->GetWordsBreakdownView().FindWindowById(BaseProjectView::WORD_CLOUD_PAGE_ID));
    if (m_keyWordsDataset->GetRowCount() != 0U)
        {
        if (wordCloudCanvas == nullptr)
            {
            wordCloudCanvas =
                new Wisteria::Canvas(view->GetSplitter(), BaseProjectView::WORD_CLOUD_PAGE_ID);
            wordCloudCanvas->SetFixedObjectsGridSize(1, 1);
            wordCloudCanvas->SetFixedObject(
                0, 0, std::make_shared<Wisteria::Graphs::WordCloud>(wordCloudCanvas));
            wordCloudCanvas->Hide();
            wordCloudCanvas->SetLabel(BaseProjectView::GetWordCloudLabel());
            wordCloudCanvas->SetName(BaseProjectView::GetWordCloudLabel());
            wordCloudCanvas->SetPrinterSettings(*wxGetApp().GetPrintData());
            view->GetWordsBreakdownView().AddWindow(wordCloudCanvas);
            }
        UpdateGraphOptions(wordCloudCanvas);

        const auto wordCloud = std::dynamic_pointer_cast<Wisteria::Graphs::WordCloud>(
            wordCloudCanvas->GetFixedObject(0, 0));
        assert(wordCloud);
        // top 100 words, with a min frequency of 2
        wordCloud->SetData(m_keyWordsDataset, GetWordsColumnName(), GetWordsCountsColumnName(), 2,
                           std::nullopt, 100);

        wordCloudCanvas->CalcAllSizes(gdc);
        }
    else
        {
        // we are getting rid of this window (if nothing in it)
        view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::WORD_CLOUD_PAGE_ID);
        }
    }

//-------------------------------------------------------
void BatchProjectDoc::DisplaySightWords()
    {
    PROFILE();
    auto* view = dynamic_cast<BatchProjectView*>(GetFirstView());

    auto* listView =
        dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetDolchSightWordsView().FindWindowById(
            BaseProjectView::ID_DOLCH_COVERAGE_LIST_PAGE_ID));
    if (IsIncludingDolchSightWords() && (m_dolchCompletionData->GetItemCount() != 0U))
        {
        if (listView == nullptr)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::ID_DOLCH_COVERAGE_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetDolchCoverageTabLabel());
            listView->SetName(BaseProjectView::GetDolchCoverageTabLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
            // add the columns
            listView->InsertColumn(listView->GetColumnCount(), _(L"Document"));
            listView->InsertColumn(listView->GetColumnCount(), _(L"Label"));
            listView->InsertColumn(listView->GetColumnCount(), _(L"Conjunctions Coverage"));
            listView->InsertColumn(listView->GetColumnCount(), _(L"Prepositions Coverage"));
            listView->InsertColumn(listView->GetColumnCount(), _(L"Pronouns Coverage"));
            listView->InsertColumn(listView->GetColumnCount(), _(L"Adverbs Coverage"));
            listView->InsertColumn(listView->GetColumnCount(), _(L"Adjectives Coverage"));
            listView->InsertColumn(listView->GetColumnCount(), _(L"Verbs Coverage"));
            listView->InsertColumn(listView->GetColumnCount(), _(L"Noun Coverage"));
            view->GetDolchSightWordsView().AddWindow(listView);
            }
        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_dolchCompletionData);
        listView->SetVirtualDataSize(m_dolchCompletionData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(
            1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        for (long i = 2; i < listView->GetColumnCount(); ++i)
            {
            listView->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
            }
        if (listView->GetSortedColumn() == -1)
            {
            listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
            }
        else
            {
            listView->Resort();
            }
        }
    else
        {
        view->GetDolchSightWordsView().RemoveWindowById(
            BaseProjectView::ID_DOLCH_COVERAGE_LIST_PAGE_ID);
        }

    listView =
        dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetDolchSightWordsView().FindWindowById(
            BaseProjectView::ID_DOLCH_WORDS_LIST_PAGE_ID));
    if (IsIncludingDolchSightWords() && (m_dolchWordsBatchData->GetItemCount() != 0U))
        {
        if (listView == nullptr)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::ID_DOLCH_WORDS_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetDolchWordTabLabel());
            listView->SetName(BaseProjectView::GetDolchWordTabLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
            view->GetDolchSightWordsView().AddWindow(listView);
            }
        // insert the columns
        listView->DeleteAllColumns();
        listView->InsertColumn(listView->GetColumnCount(), _(L"Document"));
        listView->InsertColumn(listView->GetColumnCount(), _(L"Label"));
        listView->InsertColumn(listView->GetColumnCount(),
                               /* xgettext:no-c-format */ _(L"% of Conjunctions"));
        if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
            {
            listView->InsertColumn(listView->GetColumnCount(), _(L"Conjunctions"));
            }
        listView->InsertColumn(listView->GetColumnCount(),
                               /* xgettext:no-c-format */ _(L"% of Prepositions"));
        if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
            {
            listView->InsertColumn(listView->GetColumnCount(), _(L"Prepositions"));
            }
        listView->InsertColumn(listView->GetColumnCount(),
                               /* xgettext:no-c-format */ _(L"% of Pronouns"));
        if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
            {
            listView->InsertColumn(listView->GetColumnCount(), _(L"Pronouns"));
            }
        listView->InsertColumn(listView->GetColumnCount(),
                               /* xgettext:no-c-format */ _(L"% of Adverbs"));
        if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
            {
            listView->InsertColumn(listView->GetColumnCount(), _(L"Adverbs"));
            }
        listView->InsertColumn(listView->GetColumnCount(),
                               /* xgettext:no-c-format */ _(L"% of Adjectives"));
        if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
            {
            listView->InsertColumn(listView->GetColumnCount(), _(L"Adjectives"));
            }
        listView->InsertColumn(listView->GetColumnCount(),
                               /* xgettext:no-c-format */ _(L"% of Verbs"));
        if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
            {
            listView->InsertColumn(listView->GetColumnCount(), _(L"Verbs"));
            }
        listView->InsertColumn(listView->GetColumnCount(),
                               /* xgettext:no-c-format */ _(L"% of Nouns"));
        if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
            {
            listView->InsertColumn(listView->GetColumnCount(), _(L"Nouns"));
            }

        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_dolchWordsBatchData);
        listView->SetVirtualDataSize(m_dolchWordsBatchData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(
            1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        for (long i = 2; i < listView->GetColumnCount(); ++i)
            {
            listView->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
            }
        if (listView->GetSortedColumn() == -1)
            {
            listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
            }
        else
            {
            listView->Resort();
            }
        }
    else
        {
        view->GetDolchSightWordsView().RemoveWindowById(
            BaseProjectView::ID_DOLCH_WORDS_LIST_PAGE_ID);
        }

    listView =
        dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetDolchSightWordsView().FindWindowById(
            BaseProjectView::ID_NON_DOLCH_WORDS_LIST_PAGE_ID));
    if (IsIncludingDolchSightWords() && (m_NonDolchWordsData->GetItemCount() != 0U))
        {
        if (listView == nullptr)
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::ID_NON_DOLCH_WORDS_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetNonDolchWordTabLabel());
            listView->SetName(BaseProjectView::GetNonDolchWordTabLabel());
            listView->EnableGridLines();
            listView->EnableItemViewOnDblClick();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
            view->GetDolchSightWordsView().AddWindow(listView);
            }
        // insert the columns
        listView->DeleteAllColumns();
        listView->InsertColumn(listView->GetColumnCount(), _(L"Document"));
        listView->InsertColumn(listView->GetColumnCount(), _(L"Label"));
        listView->InsertColumn(listView->GetColumnCount(),
                               /* xgettext:no-c-format */ _(L"% of Non-Dolch Words"));
        if (GetStatisticsReportInfo().IsExtendedInformationEnabled())
            {
            listView->InsertColumn(listView->GetColumnCount(), _(L"Non-Dolch Words"));
            }

        UpdateListOptions(listView);
        listView->SetColumnFilePathTruncationMode(0, GetFilePathTruncationMode());
        listView->SetVirtualDataProvider(m_NonDolchWordsData);
        listView->SetVirtualDataSize(m_NonDolchWordsData->GetItemCount());
        listView->SetColumnWidth(0, listView->EstimateColumnWidth(0));
        listView->SetColumnWidth(
            1, std::min(listView->EstimateColumnWidth(1), view->GetMaxColumnWidth()));
        for (long i = 2; i < listView->GetColumnCount(); ++i)
            {
            listView->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
            }
        if (listView->GetSortedColumn() == -1)
            {
            listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
            }
        else
            {
            listView->Resort();
            }
        }
    else
        {
        view->GetDolchSightWordsView().RemoveWindowById(
            BaseProjectView::ID_NON_DOLCH_WORDS_LIST_PAGE_ID);
        }
    }

//-------------------------------------------------------
void BatchProjectDoc::SetScoreStatsRow(
    const std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& dataGrid,
    const wxString& rowName, const wxString& optionalDescription, const long rowNum,
    const std::vector<double>& data, const int decimalSize, const VarianceMethod varianceMethod,
    const bool allowCustomFormatting)
    {
    PROFILE();
    size_t currentColumn = 0;
    dataGrid->SetItemText(rowNum, currentColumn++, rowName,
                          Wisteria::NumberFormatInfo{
                              Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                          std::numeric_limits<double>::quiet_NaN());
    if (!optionalDescription.empty())
        {
        dataGrid->SetItemText(
            rowNum, currentColumn++, optionalDescription,
            Wisteria::NumberFormatInfo{
                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
            std::numeric_limits<double>::quiet_NaN());
        }
    if (!data.empty())
        {
        constexpr int HIGHER_PRECISION = 3;
        std::vector<double> sortedData(data.begin(), data.end());
        std::ranges::sort(sortedData);

        const double minVal = *std::ranges::min_element(sortedData);
        const double maxVal = *std::ranges::max_element(sortedData);
        std::set<double> modes = statistics::mode(sortedData, floor_value<double>());
        const double rangeVal = maxVal - minVal;
        const double meansVal = statistics::mean(sortedData);
        const double medianVal = statistics::median_presorted(sortedData);
        double skewness = 0;
        double kurtosis = 0;
        double stddev = 0;
        double variance = 0;
        if (sortedData.size() >= 2)
            {
            stddev = statistics::standard_deviation(sortedData, varianceMethod ==
                                                                    VarianceMethod::SampleVariance);
            variance =
                statistics::variance(sortedData, varianceMethod == VarianceMethod::SampleVariance);
            }
        if (sortedData.size() >= 3)
            {
            skewness =
                statistics::skewness(sortedData, varianceMethod == VarianceMethod::SampleVariance);
            }
        if (sortedData.size() >= 4)
            {
            kurtosis =
                statistics::kurtosis(sortedData, varianceMethod == VarianceMethod::SampleVariance);
            }
        double lowerQuartile(0), upperQuartile(0);
        statistics::quartiles_presorted(sortedData, lowerQuartile, upperQuartile);

        dataGrid->SetItemValue(rowNum, currentColumn++, sortedData.size());
        dataGrid->SetItemValue(
            rowNum, currentColumn++, minVal,
            Wisteria::NumberFormatInfo(
                allowCustomFormatting ?
                    Wisteria::NumberFormatInfo::NumberFormatType::CustomFormatting :
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting,
                decimalSize));
        dataGrid->SetItemValue(
            rowNum, currentColumn++, maxVal,
            Wisteria::NumberFormatInfo(
                allowCustomFormatting ?
                    Wisteria::NumberFormatInfo::NumberFormatType::CustomFormatting :
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting,
                decimalSize));
        dataGrid->SetItemValue(
            rowNum, currentColumn++, rangeVal,
            Wisteria::NumberFormatInfo(
                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, decimalSize));

        if (modes.size() > 1)
            {
            wxString modeString;
            for (const double mode : modes)
                {
                modeString += wxNumberFormatter::ToString(
                                  mode, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes) +
                              L"; ";
                }
            // chop off the last "; "
            if (modeString.length() > 2)
                {
                modeString.RemoveLast(2);
                }
            dataGrid->SetItemText(
                rowNum, currentColumn++, modeString,
                Wisteria::NumberFormatInfo(
                    allowCustomFormatting ?
                        Wisteria::NumberFormatInfo::NumberFormatType::CustomFormatting :
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting,
                    0),
                *modes.begin());
            }
        else if (modes.size() == 1)
            {
            dataGrid->SetItemValue(
                rowNum, currentColumn++, *modes.cbegin(),
                Wisteria::NumberFormatInfo(
                    allowCustomFormatting ?
                        Wisteria::NumberFormatInfo::NumberFormatType::CustomFormatting :
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting,
                    0));
            }
        else // shouldn't happen
            {
            dataGrid->SetItemText(
                rowNum, currentColumn++, wxString{},
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }

        dataGrid->SetItemValue(
            rowNum, currentColumn++, meansVal,
            Wisteria::NumberFormatInfo(
                allowCustomFormatting ?
                    Wisteria::NumberFormatInfo::NumberFormatType::CustomFormatting :
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting,
                decimalSize));
        dataGrid->SetItemValue(
            rowNum, currentColumn++, medianVal,
            Wisteria::NumberFormatInfo(
                allowCustomFormatting ?
                    Wisteria::NumberFormatInfo::NumberFormatType::CustomFormatting :
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting,
                decimalSize));

        if (sortedData.size() < 3)
            {
            dataGrid->SetItemText(
                rowNum, currentColumn++,
                _(L"More than two values are required to calculate skewness."),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        else
            {
            dataGrid->SetItemValue(
                rowNum, currentColumn++, skewness,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting,
                    std::max(decimalSize, HIGHER_PRECISION)));
            }

        if (sortedData.size() < 4)
            {
            dataGrid->SetItemText(
                rowNum, currentColumn++,
                _(L"More than three values are required to calculate Kurtosis."),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        else
            {
            dataGrid->SetItemValue(
                rowNum, currentColumn++, kurtosis,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting,
                    std::max(decimalSize, HIGHER_PRECISION)));
            }

        if (sortedData.size() < 2)
            {
            dataGrid->SetItemText(
                rowNum, currentColumn++,
                // TRANSLATORS: "std. dev." is standard deviation.
                _(L"More than one value is required to calculate std. dev."),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            dataGrid->SetItemText(
                rowNum, currentColumn++,
                _(L"More than one value is required to calculate variance."),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        else
            {
            dataGrid->SetItemValue(
                rowNum, currentColumn++, stddev,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting,
                    std::max(decimalSize, HIGHER_PRECISION)));
            dataGrid->SetItemValue(
                rowNum, currentColumn++, variance,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting,
                    std::max(decimalSize, HIGHER_PRECISION)));
            }
        dataGrid->SetItemValue(
            rowNum, currentColumn++, lowerQuartile,
            Wisteria::NumberFormatInfo(
                allowCustomFormatting ?
                    Wisteria::NumberFormatInfo::NumberFormatType::CustomFormatting :
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting,
                decimalSize));
        dataGrid->SetItemValue(
            rowNum, currentColumn++, upperQuartile,
            Wisteria::NumberFormatInfo(
                allowCustomFormatting ?
                    Wisteria::NumberFormatInfo::NumberFormatType::CustomFormatting :
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting,
                decimalSize));
        }
    else
        {
        // show a valid N of zero
        dataGrid->SetItemValue(rowNum, 1, 0);
        for (size_t i = 2; i <= CUMULATIVE_STATS_COUNT; ++i)
            {
            dataGrid->SetItemText(
                rowNum, i, _(L"N/A"),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        }
    }

/// Removes a document from the collection (based on filepath).
//-------------------------------------------------------
void BatchProjectDoc::RemoveDocument(const wxString& docName)
    {
    std::optional<size_t> position{ std::nullopt };
    for (auto pos = m_docs.begin(); pos != m_docs.end(); ++pos)
        {
        if (CompareFilePaths((*pos)->GetOriginalDocumentFilePath(), docName) == 0)
            {
            wxDELETE(*pos);
            position = pos - m_docs.begin();
            m_docs.erase(pos);
            break;
            }
        }
    // if not found then don't bother looking for it in the file paths list
    if (!position.has_value())
        {
        return;
        }
    // Also remove the filepath from the list of file paths. These should already be synced up, so
    // we can remove it from the same position. If they are not synced up, then something is wrong,
    // so then we would re-sync everything to fix it.
    // cppcheck-suppress assertWithSideEffect
    assert(position.value() < GetSourceFilesInfo().size());
    // cppcheck-suppress assertWithSideEffect
    assert(CompareFilePaths(GetOriginalDocumentFilePath(position.value()), docName) == 0);
    if (position.value() < GetSourceFilesInfo().size() &&
        CompareFilePaths(GetOriginalDocumentFilePath(position.value()), docName) == 0)
        {
        GetSourceFilesInfo().erase(GetSourceFilesInfo().begin() + position.value());
        }
    // should never happen, this is a fail-safe
    else
        {
        SyncFilePathsWithDocuments();
        }
    }

//-------------------------------------------------------
void BatchProjectDoc::SyncFilePathsWithDocuments()
    {
    GetSourceFilesInfo().clear();
    GetSourceFilesInfo().reserve(m_docs.size());
    for (const auto* doc : m_docs)
        {
        assert(!doc->GetSourceFilesInfo().empty());
        GetSourceFilesInfo().push_back(doc->GetSourceFilesInfo().at(0));
        }
    }
