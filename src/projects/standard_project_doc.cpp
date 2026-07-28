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

#include "standard_project_doc.h"
#include "../Wisteria-Dataviz/src/base/reportenumconvert.h"
#include "../Wisteria-Dataviz/src/graphs/crawfordgraph.h"
#include "../Wisteria-Dataviz/src/graphs/danielsonbryan2plot.h"
#include "../Wisteria-Dataviz/src/graphs/fleschchart.h"
#include "../Wisteria-Dataviz/src/graphs/heatmap.h"
#include "../Wisteria-Dataviz/src/graphs/lixgauge.h"
#include "../Wisteria-Dataviz/src/graphs/lixgaugegerman.h"
#include "../Wisteria-Dataviz/src/graphs/piechart.h"
#include "../Wisteria-Dataviz/src/graphs/wordcloud.h"
#include "../Wisteria-Dataviz/src/import/html_encode.h"
#include "../Wisteria-Dataviz/src/import/rtf_encode.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/listdlg.h"
#include "../graphs/schwartzgraph.h"
#include "../indexing/diacritics.h"
#include "../readability/readability.h"
#include "../results-format/project_report_format.h"
#include "../results-format/word_collection_text_formatting.h"
#include "../ui/dialogs/project_wizard_dlg.h"
#include "project_navigation_links.h"
#include "standard_project_view.h"
#include <wx/richmsgdlg.h>
#include <wx/webview.h>
#include <wx/webviewfshandler.h>
#include <wx/wfstream.h>

wxDECLARE_APP(ReadabilityApp);

wxIMPLEMENT_DYNAMIC_CLASS(ProjectDoc, wxDocument)

    //-------------------------------------------------------
    void ProjectDoc::ShowQueuedMessages()
    {
    auto* view = dynamic_cast<BaseProjectView*>(GetFirstView());
    for (const auto& queuedMsg : GetQueuedMessages())
        {
        view->ShowInfoMessage(queuedMsg);
        }
    }

//-------------------------------------------------------
void ProjectDoc::RemoveMisspellings([[maybe_unused]] const wxArrayString& misspellingsToRemove)
    {
    RefreshRequired(ProjectRefresh::FullReindexing);
    RefreshProject();
    }

//-------------------------------------------------------
void ProjectDoc::ExcludeAllCustomTestsTests()
    {
    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    for (const auto& customTest : GetCustTestsInUse())
        {
        while (view->GetWordsBreakdownView().RemoveWindowById(
            customTest.GetIterator()->get_interface_id()))
            {
            }
        }
    BaseProject::ExcludeAllCustomTestsTests();
    }

//-------------------------------------------------------
std::vector<CustomReadabilityTestInterface>::iterator
ProjectDoc::RemoveCustomReadabilityTest(const wxString& testName, const int Id)
    {
    auto* view = dynamic_cast<ProjectView*>(GetFirstView());

    // remove any views that are related to this test
    // (text window and word list window)
    while (view->GetWordsBreakdownView().RemoveWindowById(Id))
        {
        }
    // remove the test
    const auto iter = BaseProject::RemoveCustomReadabilityTest(testName, Id);
    // remove the test score
    const auto location = view->GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(testName);
    if (location != wxNOT_FOUND)
        {
        view->GetReadabilityScoresList()->GetResultsListCtrl()->DeleteItem(location);
        }

    DisplayStatistics();
    view->UpdateSideBarIcons();

    return iter;
    }

//------------------------------------------------
void ProjectDoc::LoadManuallyEnteredText()
    {
    SetOriginalDocumentFilePath(wxString{});
    try
        {
        LoadDocument();
        }
    catch (...)
        {
        LogMessage(
            _(L"An unknown error occurred while analyzing the text. Unable to create project."),
            _(L"Error"), wxOK | wxICON_EXCLAMATION);
        }
    }

//------------------------------------------------
void ProjectDoc::RefreshStatisticsReports()
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

    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    const auto selectedItem = view->GetSideBar()->GetSelectedFolderId();

    const wxWindowUpdateLocker noUpdates(GetDocumentWindow());
    const BaseProjectProcessingLock processingLock(this);
    DisplayStatistics();

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

    GetDocumentWindow()->Refresh();
    ResetRefreshRequired();
    Modify(true);
    }

//------------------------------------------------
void ProjectDoc::RefreshGraphs()
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

    const wxWindowUpdateLocker noUpdates(GetDocumentWindow());
    const BaseProjectProcessingLock processingLock(this);
    DisplayReadabilityScores(false);
    DisplayReadabilityGraphs();
    DisplayWordCharts();
    DisplaySentenceCharts();

    GetDocumentWindow()->Refresh();
    ResetRefreshRequired();
    Modify(true);
    }

//------------------------------------------------
void ProjectDoc::RefreshProject()
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

    StopRealtimeUpdate();

    const wxBusyCursor bc;

    // reload the excluded phrases
    LoadExcludePhrases();

    // load appended template file (if there is one)
    LoadAppendedDocument();

    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    const auto selectedItem = view->GetSideBar()->GetSelectedSubItemId();

    // If the original text is gone (there won't be anything to analyze),
    // or if just cosmetic changes (e.g., graph options), then don't re-index,
    // just do a simple refresh.
    if (!LoadingOriginalTextSucceeded() || !IsDocumentReindexingRequired())
        {
        const wxWindowUpdateLocker noUpdates(GetDocumentWindow());
        const BaseProjectProcessingLock processingLock(this);
        DisplayReadabilityScores(false);
        DisplayStatistics();
        DisplayReadabilityGraphs();
        DisplayWordsBreakdown();
        DisplaySentencesBreakdown();
        if (IsTextSectionRefreshRequired())
            {
            DisplayHighlightedText(GetTextHighlightColor(), GetTextViewFont());
            }
        else
            {
            UpdateHighlightedTextWindows();
            }

        view->UpdateSideBarIcons();
        view->UpdateRibbonState();
        view->Present();
        UpdateAllViews();

        if (!view->GetSideBar()->SelectSubItemById(selectedItem, true, true))
            {
            view->GetSideBar()->SelectFolder(0, true, true);
            }

        GetDocumentWindow()->Refresh();
        ResetRefreshRequired();
        Modify(true);
        return;
        }
    const BaseProjectProcessingLock processingLock(this);
    const wxWindowUpdateLocker noUpdates(GetDocumentWindow());

    // reload the document
    if (GetDocumentStorageMethod() == TextStorage::LoadFromExternalDocument)
        {
        if (!LoadExternalDocument())
            {
            // if the loading failed then reset everything and bail
            ResetRefreshRequired();
            return;
            }
        UpdateSourceFileModifiedTime();
        RestartRealtimeUpdate();
        }
    // if embedded, then reload our embedded content
    else
        {
        try
            {
            LoadDocument();
            }
        catch (...)
            {
            LogMessage(_(L"An unknown error occurred while analyzing the document. "
                         "Unable to create project."),
                       _(L"Error"), wxOK | wxICON_EXCLAMATION);
            return;
            }
        }

    if ((GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis ||
         GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings) &&
        GetWords()->get_complete_sentence_count() == 0)
        {
        LogMessage(
            _(L"You have requested to ignore incomplete sentences, but there are no other valid "
              "sentences in the text. No text will be included in the analysis."),
            _(L"Error"), wxOK | wxICON_EXCLAMATION);
        }

        {
        /* NOTE: do not use a progress bar with APP_MODAL because that calls
           disable on the text view windows. On macOS, disabling/re-enabling
           text controls appears to reset their font color (which we are customizing
           in DisplayHighlightedText()).*/
        const wxBusyInfo bi(wxBusyInfoFlags()
                                .Text(_(L"Reloading project..."))
                                .Parent(wxGetApp().GetParentingWindow()));
#ifdef __WXGTK__
        wxMilliSleep(100);
        wxGetApp().Yield();
#endif

        if (GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis ||
            GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings)
            {
            CalculateStatisticsIgnoringInvalidSentences();
            }
        else if (GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences)
            {
            CalculateStatistics();
            }
        CalculateGraphData();

        if (GetTotalWords() == 0)
            {
            LogMessage((GetTextSource() == TextSource::FromFile) ?
                           _(L"No words were found in the original document. "
                             "Project cannot be recalculated.") :
                           _(L"No valid words were entered. Project cannot be recalculated."),
                       _(L"Import Error"), wxOK | wxICON_INFORMATION);
            GetDocumentWindow()->Refresh();

            ResetRefreshRequired();
            return;
            }

        LoadHardWords();

        DisplayStatistics();
        DisplayReadabilityScores(false);
        DisplayReadabilityGraphs();
        DisplayWordsBreakdown();
        DisplayHighlightedText(GetTextHighlightColor(), GetTextViewFont());
        DisplaySentencesBreakdown();
        DisplayGrammar();
        DisplaySightWords();

        DeleteUniqueWordMap();

        Modify(true);

        view->UpdateSideBarIcons();
        view->UpdateRibbonState();
        view->Present();
        UpdateAllViews();
        }

    // See if the view that was originally selected is gone.
    // If so then select the scores section.
    if (!view->GetSideBar()->SelectSubItemById(selectedItem, true, true))
        {
        view->GetSideBar()->SelectFolder(0, true, true);
        }
    view->ShowSideBar(view->IsSideBarShown());

    GetDocumentWindow()->Refresh();

    ResetRefreshRequired();

    ShowQueuedMessages();
    }

//------------------------------------------------
bool ProjectDoc::LoadProjectFile(const char* projectFileText, const size_t textLength)
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

    // if the document's content is just embedded and shouldn't be reloaded
    if (GetDocumentStorageMethod() == TextStorage::EmbedText)
        {
        // load the embedded text
        std::wstring contentFile = cat.ReadTextFile(ProjectContentFileLabel());
        if (!contentFile.empty())
            {
            SetDocumentText(std::move(contentFile));
            try
                {
                LoadDocument();
                }
            catch (...)
                {
                wxMessageBox(_(L"An unknown error occurred while analyzing the document. "
                               "Unable to create project."),
                             _(L"Error"), wxOK | wxICON_EXCLAMATION);
                return false;
                }
            }
        else
            {
            // If text was manually entered before but empty, then it's either lost or was left
            // empty by the user.
            if (GetTextSource() == TextSource::EnteredText)
                {
                if (WarningManager::HasWarning(_DT(L"no-embedded-text")))
                    {
                    WarningMessage warningMsg =
                        *WarningManager::GetWarning(_DT(L"no-embedded-text"));
                    warningMsg.SetMessage(
                        _(L"Manually entered text could not be found in the project file.\n"
                          "Only empty statistics will be displayed."));
                    LogMessage(warningMsg);
                    }
                // force loading empty text just so that we have an empty word collection to build
                // statistics from
                SetDocumentText(std::move(contentFile));
                try
                    {
                    LoadDocument();
                    }
                catch (...)
                    {
                    wxMessageBox(_(L"An unknown error occurred while analyzing the document. "
                                   "Unable to create project."),
                                 _(L"Error"), wxOK | wxICON_EXCLAMATION);
                    return false;
                    }
                return true;
                }
            // ...otherwise, external file was supposed to be embedded,
            // but internal copy of the text couldn't be found.
            // Try to reload it.

            if (LoadExternalDocument())
                {
                UpdateSourceFileModifiedTime();
                LogMessage(_(L"The document's content could not be found in the project file. "
                             "Original document will be reloaded."),
                           _(L"Warning"), wxOK | wxICON_INFORMATION);
                Modify(true);
                return true;
                }
            // Should not normally happen. File was supposed to be embedded, but wasn't in the
            // project file and external file can't be found either.
            wxMessageBox(_(L"Document content could not be found in the project file and "
                           "external document could not be located.\nUnable to create project."),
                         _(L"Error"), wxOK | wxICON_EXCLAMATION);
            return false;
            }
        }
    // project is set to always reload the file
    else
        {
        if (GetTextSource() == TextSource::FromFile)
            {
            if (LoadExternalDocument())
                {
                UpdateSourceFileModifiedTime();
                return true;
                }

            wxMessageBox(wxString::Format(_(L"External document could not be located:\n\n"
                                            "\"%s\"\n\nUnable to create project."),
                                          GetOriginalDocumentFilePath()),
                         _(L"Error"), wxOK | wxICON_EXCLAMATION);
            return false;
            }
        /* This should not happen because the entered text flag overrides the storage flag to force
           it to embed, but just for the sake of being verbose... */
        // TextSource::EnteredText
        wxMessageBox(_(L"Manually entered text was not embedded previously.\n"
                       "Unable to create project."),
                     _(L"Error"), wxOK | wxICON_EXCLAMATION);
        return false;
        }

    return true;
    }

//------------------------------------------------
bool ProjectDoc::OnOpenDocument(const wxString& filename)
    {
    const wxBusyCursor bc;

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

    // make sure the file exists first
    if (!wxFile::Exists(filename))
        {
        LogMessage(
            wxString::Format(_(L"'%s': unable to open project file. File not found."), filename),
            _(L"Error"), wxOK | wxICON_EXCLAMATION);
        return false;
        }
    if (!OnSaveModified())
        {
        return false;
        }

    SetTitle(ParseTitleFromFileName(filename));
    SetFilename(filename, true);

    const BaseProjectProcessingLock processingLock(this);

    MemoryMappedFile sourceFile;
    try
        {
        sourceFile.MapFile(GetFilename());
        const char* projectFileText = static_cast<char*>(sourceFile.GetStream());
        SetLoadingOriginalTextSucceeded(LoadProjectFile(projectFileText, sourceFile.GetMapSize()));
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
            if (WarningManager::HasWarning(_DT(L"project-open-as-read-only")))
                {
                LogMessage(*WarningManager::GetWarning(_DT(L"project-open-as-read-only")));
                }
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
        SetLoadingOriginalTextSucceeded(LoadProjectFile(projectFileText.data(), readSize));
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
        /* Couldn't get a map of the file for unknown reason, so buffer it.*/
        if (!LockProjectFile())
            {
            return false;
            }
        std::string projectFileText;
        projectFileText.resize(m_File.Length());

        m_File.Seek(0);
        const size_t readSize = m_File.Read(projectFileText.data(), projectFileText.size());
        SetLoadingOriginalTextSucceeded(LoadProjectFile(projectFileText.data(), readSize));
        }
    catch (...)
        {
        LogMessage(wxString::Format(_(L"'%s': unable to open project file."), GetFilename()),
                   _(L"Project Open"), wxOK | wxICON_EXCLAMATION);
        return false;
        }

    /* If the indexed text was found in the project or external file
       then reanalyze everything. This should be the normal scenario and
       would only fail if the text was not embedded and the
       external file could not be found (if applicable).*/
    if (LoadingOriginalTextSucceeded())
        {
        const wxBusyInfo bi(wxBusyInfoFlags()
                                .Text(_(L"Loading project..."))
                                .Parent(wxGetApp().GetParentingWindow()));
#ifdef __WXGTK__
        wxMilliSleep(100);
        wxGetApp().Yield();
#endif

        /* if they set this to exclude headers and such, make sure we actually have some
           valid sentences to work with*/
        if ((GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis ||
             GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings) &&
            GetWords()->get_complete_sentence_count() == 0)
            {
            LogMessage(
                _(L"You have requested to ignore incomplete sentences, but there are no other "
                  "valid sentences in the text. Incomplete sentences will need to be included "
                  "in the analysis."),
                _(L"Warning"), wxOK | wxICON_EXCLAMATION);
            SetInvalidSentenceMethod(InvalidSentence::IncludeAsFullSentences);
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
        CalculateGraphData();

        LoadHardWords();

        DisplayStatistics();
        DisplayReadabilityScores(false);
        DisplayReadabilityGraphs();
        DisplayWordsBreakdown();
        DisplayHighlightedText(GetTextHighlightColor(), GetTextViewFont());
        DisplaySentencesBreakdown();
        DisplayGrammar();
        DisplaySightWords();

        DeleteUniqueWordMap();
        }
    else
        {
        return false;
        }

    /* Don't call "Modify(false)" because if the external document could not be found when
       loading the project and user searched for the document then the modified flag is set*/
    SetDocumentSaved(true);

    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    view->UpdateSideBarIcons();
    view->UpdateRibbonState();
    view->Present();
    UpdateAllViews();

    const auto selectedIndex =
        view->GetSideBar()->FindFolder(BaseProjectView::SIDEBAR_READABILITY_SCORES_SECTION_ID);
    view->GetSideBar()->SelectFolder(selectedIndex.value_or(0), true);

    ShowQueuedMessages();

    if (WarningManager::HasWarning(_DT(L"note-project-properties")))
        {
        view->ShowInfoMessage(*WarningManager::GetWarning(_DT(L"note-project-properties")));
        }

    if (GetDocumentStorageMethod() == TextStorage::LoadFromExternalDocument)
        {
        RestartRealtimeUpdate();
        }

    return true;
    }

//------------------------------------------------
bool ProjectDoc::RunProjectWizard(const wxString& path)
    {
    // make sure document content is cleared
    FreeDocumentText();

    // Run through the project wizard
    auto* wizard =
        new ProjectWizardDlg(wxGetApp().GetParentingWindow(), ProjectType::StandardProject, path);
    if (wizard->ShowModal() != wxID_OK)
        {
        wizard->Destroy();
        return false;
        }

    SetProjectLanguage(wizard->GetLanguage());
    wxGetApp().GetAppOptions()->SetProjectLanguage(wizard->GetLanguage());

    if (wizard->IsTextFromFileSelected())
        {
        SetTitle(ParseTitleFromFileName(wizard->GetFilePath()));

        SetOriginalDocumentFilePath(wizard->GetFilePath());
        SetTextSource(TextSource::FromFile);
        }
    else if (wizard->IsManualTextEntrySelected())
        {
        SetTitle(_(L"Untitled"));

        grammar::convert_ligatures_and_diacritics convertDiacritics;
        std::wstring enteredText = wizard->GetEnteredText().wc_string();
        if (convertDiacritics(enteredText))
            {
            enteredText = convertDiacritics.get_conversion();
            }
        SetDocumentText(enteredText);

        SetTextSource(TextSource::EnteredText);
        }

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
            // override how headers and lists are counted so that they are always included if this
            // is a form
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
            IncludeDolchSightWords(GetProjectLanguage() ==
                                   readability::test_language::english_test);
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
            IncludeDolchSightWords(GetProjectLanguage() ==
                                   readability::test_language::english_test);
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
    // pre-2007 Microsoft Word files (*.doc) are difficult to detect lists in, so if we are
    // not explicitly specifying "fitted to the page" analysis for this project (above), then
    // override the global option and set it to treat all newlines as the end of a paragraph.
    else if (wizard->IsTextFromFileSelected() &&
             wxFileName(wizard->GetFilePath()).GetExt().CmpNoCase(_DT(L"doc")) == 0)
        {
        SetParagraphsParsingMethod(ParagraphParse::EachNewLineIsAParagraph);
        }

    if (wizard->IsNarrativeSelected())
        {
        SetInvalidSentenceMethod(InvalidSentence::ExcludeFromAnalysis);
        }
    else if (wizard->IsFragmentedTextSelected())
        {
        // override how headers and lists are counted so that they are always
        // included if this is a form
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

    wxGetApp().GetAppOptions()->SetTextSource(
        wizard->IsTextFromFileSelected() ? TextSource::FromFile : TextSource::EnteredText);
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
void ProjectDoc::DisplayReadabilityScores(const bool setFocus)
    {
    // this area can be included for an empty project, just won't show anything
    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    if (view != nullptr)
        {
        view->GetReadabilityScoresList()->Clear();
        }

    for (auto& rTest : GetReadabilityTests().get_tests())
        {
        if (rTest.is_included())
            {
            AddStandardReadabilityTest(rTest.get_test().get_id().c_str(), false);
            }
        }

    if (IsIncludingDolchSightWords())
        {
        /* NOTE, do not call AddDolchSightWords here because that will require a project refresh,
           just add a message to the score window.
           We don't actually show anything in the score window (Dolch isn't really a test),
           but point user in the right direction.*/
        SetReadabilityTestResult(
            ReadabilityMessages::GetDolchLabel(), ReadabilityMessages::GetDolchLabel(),
            wxString(_(L"Refer to the <a href=\"#dolch\">Dolch Sight Words</a> section.")),
            std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxString{}), wxString{},
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            false);
        }
    AddCustomReadabilityTests();
    if (view != nullptr)
        {
        UpdateExplanationListOptions(view->GetReadabilityScoresList());
        view->UpdateStatistics();
        view->GetReadabilityScoresList()->UpdateExplanationDisplay();
        view->GetReadabilityScoresList()->GetResultsListCtrl()->SortColumn(
            0, Wisteria::SortDirection::SortAscending);
        if (setFocus)
            {
            view->GetSideBar()->SelectFolder(view->GetSideBar()->FindFolder(
                BaseProjectView::SIDEBAR_READABILITY_SCORES_SECTION_ID));
            }
        // update, add, or remove the scores summary report
        if (IsIncludingScoreSummaryReport())
            {
            wxString text = view->GetReadabilityScoresList()->GetExplanationsText();
            if (text.empty())
                {
                text = L"<div style='margin:20px 10px;'><span class='pill'>" +
                       _(L"No tests are currently in the project.") + L"</span></div>";
                }
            auto* scoresReport =
                dynamic_cast<wxWebView*>(view->GetReadabilityResultsView().FindWindowById(
                    BaseProjectView::READABILITY_SCORES_SUMMARY_REPORT_PAGE_ID));
            if (scoresReport == nullptr)
                {
                scoresReport =
                    wxWebView::New(view->GetSplitter(),
                                   BaseProjectView::READABILITY_SCORES_SUMMARY_REPORT_PAGE_ID);
                if (scoresReport != nullptr)
                    {
                    scoresReport->Hide();
                    scoresReport->SetName(_(L"Summary Report"));
                    scoresReport->SetLabel(_(L"Summary Report"));
                    scoresReport->EnableContextMenu(false);
                    scoresReport->Bind(wxEVT_WEBVIEW_NAVIGATING,
                                       &ProjectView::OnExplanationNavigating, view);
                    view->GetReadabilityResultsView().InsertWindow(1, scoresReport);
                    }
                }
            if (scoresReport != nullptr)
                {
                scoresReport->SetPage(
                    NavLink::AnchorsToExplanationScheme(
                        ProjectReportFormat::FormatHtmlReportStart(
                            wxString::Format( // TRANSLATORS: %s is the project name
                                _(L"Summary Report [%s]"), GetTitle()),
                            wxGetApp().GetAppOptions()->GetReportTheme()) +
                        ProjectReportFormat::FormatReportBanner(_(L"Readability Score Summary"),
                                                                GetTitle()) +
                        text + ProjectReportFormat::FormatHtmlReportEnd()),
                    wxString{});
                }
            }
        else
            {
            // we are getting rid of this window (if nothing in it)
            view->GetReadabilityResultsView().RemoveWindowById(
                BaseProjectView::READABILITY_SCORES_SUMMARY_REPORT_PAGE_ID);
            }

        // add/remove the goals
        if (!GetTestGoals().empty() || !GetStatGoals().empty())
            {
            auto* goalsList = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
                view->GetReadabilityResultsView().FindWindowById(
                    BaseProjectView::READABILITY_GOALS_PAGE_ID));
            if (goalsList == nullptr)
                {
                goalsList = new Wisteria::UI::ListCtrlEx(
                    view->GetSplitter(), BaseProjectView::READABILITY_GOALS_PAGE_ID,
                    wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxBORDER_SUNKEN);
                goalsList->Hide();
                goalsList->SetLabel(_(L"Goals"));
                goalsList->SetName(_(L"Goals"));
                goalsList->InsertColumn(0, _(L"Test/Statistic"));
                goalsList->InsertColumn(1, _(L"Minimum Recommended Value"));
                goalsList->InsertColumn(2, _(L"Maximum Recommended Value"));
                goalsList->SetSortable(true);
                goalsList->EnableItemViewOnDblClick();
                goalsList->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
                UpdateListOptions(goalsList);
                view->GetReadabilityResultsView().InsertWindow(1, goalsList);
                }
            goalsList->DeleteAllItems();

            // subroutine to add a goal into our results list
            const auto insertGoalIntoList =
                [goalsList](const TestGoal& goal, const wxString& goalName)
            {
                const auto insertedItem = goalsList->InsertItem(
                    0, wxString::Format(L"%s %s",
                                        (goal.GetPassFailFlags().all() ?
                                             BaseProjectView::GetCheckmarkEmoji() :
                                             BaseProjectView::GetWarningEmoji()),
                                        goalName));
                if (!std::isnan(goal.GetMinGoal()))
                    {
                    goalsList->SetItemText(
                        insertedItem, 1,
                        wxString::Format(L"%s %s",
                                         (goal.GetPassFailFlags()[0] ?
                                              BaseProjectView::GetCheckmarkEmoji() :
                                              BaseProjectView::GetWarningEmoji()),
                                         wxNumberFormatter::ToString(
                                             goal.GetMinGoal(), 1,
                                             wxNumberFormatter::Style::Style_NoTrailingZeroes)));
                    }
                // no goal means that it "passed"
                else
                    {
                    goalsList->SetItemText(insertedItem, 1, BaseProjectView::GetCheckmarkEmoji());
                    }
                if (!std::isnan(goal.GetMaxGoal()))
                    {
                    goalsList->SetItemText(
                        insertedItem, 2,
                        wxString::Format(L"%s %s",
                                         (goal.GetPassFailFlags()[1] ?
                                              BaseProjectView::GetCheckmarkEmoji() :
                                              BaseProjectView::GetWarningEmoji()),
                                         wxNumberFormatter::ToString(
                                             goal.GetMaxGoal(), 1,
                                             wxNumberFormatter::Style::Style_NoTrailingZeroes)));
                    }
                else
                    {
                    goalsList->SetItemText(insertedItem, 2, BaseProjectView::GetCheckmarkEmoji());
                    }
            };

            // test goals
            for (const auto& goal : GetTestGoals())
                {
                if (!goal.HasGoals())
                    {
                    continue;
                    }
                const auto [sTest, found] = GetReadabilityTests().find_test(goal.GetName().c_str());
                const wxString testName =
                    found ? sTest->get_test().get_long_name().c_str() : goal.GetName().c_str();

                insertGoalIntoList(goal, testName);
                }
            // stats goals
            for (const auto& goal : GetStatGoals())
                {
                if (!goal.HasGoals())
                    {
                    continue;
                    }
                const auto statGoalLabel =
                    GetStatGoalLabels().find({ goal.GetName(), goal.GetName() });
                const wxString goalName = (statGoalLabel != GetStatGoalLabels().cend()) ?
                                              statGoalLabel->first.second.c_str() :
                                              goal.GetName().c_str();

                insertGoalIntoList(goal, goalName);
                }
            goalsList->DistributeColumns();
            goalsList->SetColumnWidth(0, goalsList->FromDIP(wxSize(200, 200).GetWidth()));
            }
        else
            {
            // we are getting rid of this window (if nothing in it)
            view->GetReadabilityResultsView().RemoveWindowById(
                BaseProjectView::READABILITY_GOALS_PAGE_ID);
            }
        }
    }

//-------------------------------------------------------
bool ProjectDoc::OnCreate(const wxString& path, long flags)
    {
    if ((flags & wxDOC_NEW) != 0)
        {
        const wxString exampleFolder =
            wxGetApp().FindResourceDirectory(_DT(L"examples", DTExplanation::FilePath));
        if (exampleFolder.empty() || !wxFileName::DirExists(exampleFolder))
            {
            wxLogWarning(L"Unable to find examples folder: %s", exampleFolder);
            }
        const FilePathResolver resolvePath(path, false);
        // If a file path to a document (e.g., an RTF file) that is NOT from the examples folder,
        // then bypass the wizard and just use the system defaults.
        // Also bypass the wizard if we are running a Lua script.
        // This is useful for scripting, where you need to create a new project from a filepath
        // and add tests and whatnot afterward. In this case, you don't want an interactive
        // wizard appearing. Same for where you drag-n-drop a file into the interface.
        if (!resolvePath.IsInvalidFile() &&
            (wxFileName(path).GetPath().CmpNoCase(exampleFolder) != 0 ||
             LuaInterpreter::IsRunning()))
            {
            SetOriginalDocumentFilePath(path);
            SetTextSource(TextSource::FromFile);
            return wxDocument::OnCreate(wxString{}, flags);
            }
        // scripting framework passes this in to create an empty project
        // that can have files added later
        if (path == L"EMPTY_PROJECT")
            {
            return wxDocument::OnCreate(wxString{}, flags);
            }
        // otherwise, use the wizard if raw text (or no text, or examples file path) was passed in
        if (!RunProjectWizard(path))
            {
            return false;
            }
        }
    return wxDocument::OnCreate(path, flags);
    }

//-------------------------------------------------------
bool ProjectDoc::OnNewDocument()
    {
    if (!wxDocument::OnNewDocument())
        {
        return false;
        }

    const wxBusyCursor bc;

    const BaseProjectProcessingLock processingLock(this);

    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxASSERT_MSG(view->GetFrame(), L"Invalid frame for newly created document!");

    LoadExcludePhrases();

    // load appended template file (if there is one)
    LoadAppendedDocument();

    // load the document
    if (GetTextSource() == TextSource::FromFile)
        {
        // there is embedded text (that may have been passed from a batch project),
        // so load that here.
        if (GetDocumentStorageMethod() == TextStorage::EmbedText && !GetDocumentText().empty())
            {
            LoadDocument();
            }
        else if (!LoadExternalDocument())
            {
            return false;
            }

        if (GetDocumentStorageMethod() == TextStorage::LoadFromExternalDocument)
            {
            UpdateSourceFileModifiedTime();
            }
        SetTitle(ParseTitleFromFileName(GetOriginalDocumentFilePath()));
        SetFilename(ParseTitleFromFileName(GetOriginalDocumentFilePath()), true);
        }
    else if (GetTextSource() == TextSource::EnteredText)
        {
        LoadManuallyEnteredText();
        /* need to set storage method to embed if using manually entered text;
           otherwise, the text will get lost*/
        SetDocumentStorageMethod(TextStorage::EmbedText);
        }

    if (GetWords() == nullptr)
        {
        LogMessage(_(L"An unknown error occurred while loading the document."), _(L"Error"),
                   wxOK | wxICON_ERROR);
        return false;
        }

    /* See if there is an inordinate amount of titles/headers/bullets, and if they
       are asking to ignore these then make sure they understand that a large part of the
       document will be ignored.
       Note that we don't bother with this check with webpages because they normally
       contain lists for things like menus that we would indeed want to ignore.*/
    const FilePathResolver resolvePath(GetOriginalDocumentFilePath(), true);
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
                const auto warningIter =
                    WarningManager::GetWarning(_DT(L"high-count-sentences-being-ignored"));
                if (warningIter != WarningManager::GetWarnings().end() &&
                    warningIter->ShouldBeShown())
                    {
                    wxRichMessageDialog msg(view->GetFrame(), warningIter->GetMessage(),
                                            warningIter->GetTitle(), warningIter->GetFlags());
                    msg.SetEscapeId(wxID_NO);
                    msg.ShowCheckBox(_(L"Remember my answer"));
                    msg.SetYesNoLabels(_(L"Include incomplete sentences"),
                                       _(L"Continue excluding incomplete sentences"));
                    const int dlgResponse = msg.ShowModal();
                    if (warningIter != WarningManager::GetWarnings().end() &&
                        msg.IsCheckBoxChecked())
                        {
                        warningIter->Show(false);
                        warningIter->SetPreviousResponse(dlgResponse);
                        }
                    if (dlgResponse == wxID_YES)
                        {
                        SetInvalidSentenceMethod(InvalidSentence::IncludeAsFullSentences);
                        }
                    }
                else if (warningIter->GetPreviousResponse() == wxID_YES)
                    {
                    SetInvalidSentenceMethod(InvalidSentence::IncludeAsFullSentences);
                    }
                }
            }
        }

        // make the busy message go out of scope before queued messages appear
        {
        const wxBusyInfo bi(wxBusyInfoFlags()
                                .Text(_(L"Loading project..."))
                                .Parent(wxGetApp().GetParentingWindow()));
#ifdef __WXGTK__
        wxMilliSleep(100);
        wxGetApp().Yield();
#endif

        if (GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis ||
            GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings)
            {
            CalculateStatisticsIgnoringInvalidSentences();
            }
        else if (GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences)
            {
            CalculateStatistics();
            }
        CalculateGraphData();

        // load the images now
        SetPlotBackGroundImagePath(GetPlotBackGroundImagePath());
        SetStippleImagePath(GetStippleImagePath());
        SetWatermarkLogoPath(GetWatermarkLogoPath());
        SetGraphCommonImagePath(GetGraphCommonImagePath());

        LoadHardWords();

        DisplayStatistics();
        DisplayReadabilityScores(false);
        DisplayReadabilityGraphs();
        DisplayWordsBreakdown();
        DisplayHighlightedText(GetTextHighlightColor(), GetTextViewFont());
        DisplaySentencesBreakdown();
        DisplayGrammar();
        DisplaySightWords();

        DeleteUniqueWordMap();

        Modify(true);

        view->UpdateSideBarIcons();
        view->UpdateRibbonState();
        view->Present();
        UpdateAllViews();
        }

    const auto selectedIndex =
        view->GetSideBar()->FindFolder(BaseProjectView::SIDEBAR_READABILITY_SCORES_SECTION_ID);
    view->GetSideBar()->SelectFolder(selectedIndex.value_or(0), true);

    if (GetTotalWords() == 0)
        {
        if (WarningManager::HasWarning(_DT(L"document-no-words")))
            {
            LogMessage(*WarningManager::GetWarning(_DT(L"document-no-words")), true);
            }
        }
    else if (GetTotalWords() < 20)
        {
        if (WarningManager::HasWarning(_DT(L"document-less-than-20-words")))
            {
            LogMessage(*WarningManager::GetWarning(_DT(L"document-less-than-20-words")), true);
            }
        }
    else if (GetTotalWords() < 100)
        {
        if (WarningManager::HasWarning(_DT(L"document-less-than-100-words")))
            {
            LogMessage(*WarningManager::GetWarning(_DT(L"document-less-than-100-words")), true);
            }
        }

    // Check for sentences that got broken up by paragraph breaks and warn if there are a lot of
    // them, this indicates a messed up file. We do this here so not to interrupt the creation of
    // the project too much.
    size_t paragraphBrokenSentences{ 0 };
    for (auto pos = GetWords()->get_lowercase_beginning_sentences().begin();
         pos != GetWords()->get_lowercase_beginning_sentences().end(); ++pos)
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
                _(L"This document contains at least %zu sentences that appear "
                  "to be split by paragraph breaks. "
                  "This may lead to incorrect results.\n"
                  "Please review your document to verify that this is intentional."),
                paragraphBrokenSentences));
            LogMessage(warningMsg, true);
            }
        }

    // Go through the sentences and see if any are not complete but considered valid because of
    // their length. If any are found, then mention it to the user.
    size_t sentencesMissingEndingPunctuationsConsideredCompleteBecauseOfLength{ 0 };
    auto punctPos = GetWords()->get_punctuation().cbegin();
    wxArrayString longIncompleteSentences;
    for (const auto& sent : GetWords()->get_sentences())
        {
        // note our special case for list item lines ending with semicolons will be ignored here
        if (sent.is_valid() && !sent.ends_with_valid_punctuation() &&
            !characters::is_character::is_semicolon(sent.get_ending_punctuation()) &&
            sent.get_word_count() > GetIncludeIncompleteSentencesIfLongerThanValue())
            {
            ++sentencesMissingEndingPunctuationsConsideredCompleteBecauseOfLength;
            longIncompleteSentences.Add(ProjectReportFormat::FormatSentence(
                this, sent, punctPos, GetWords()->get_punctuation().cend()));
            }
        }
    if (sentencesMissingEndingPunctuationsConsideredCompleteBecauseOfLength > 0)
        {
        auto warningIter =
            WarningManager::GetWarning(_DT(L"incomplete-sentences-valid-from-length"));
        if (warningIter != WarningManager::GetWarnings().end() && warningIter->ShouldBeShown())
            {
            Wisteria::UI::ListDlg listDlg(
                view->GetFrame(), longIncompleteSentences, false,
                Wisteria::UI::LD_CLOSE_BUTTON | Wisteria::UI::LD_DONT_SHOW_AGAIN, wxID_ANY,
                warningIter->GetTitle(),
                wxString::Format(
                    wxPLURAL(
                        L"This document contains %zu incomplete sentence longer than %zu words "
                        "which will be included in the analysis.\n\nTo change this, increase the "
                        "\"Include incomplete sentences containing more than...\" option under "
                        "Project Properties->Document Indexing.",
                        L"This document contains %zu incomplete sentences longer than %zu words "
                        "which will be included in the analysis.\n\nTo change this, increase the "
                        "\"Include incomplete sentences containing more than...\" option under "
                        "Project Properties->Document Indexing.",
                        sentencesMissingEndingPunctuationsConsideredCompleteBecauseOfLength),
                    sentencesMissingEndingPunctuationsConsideredCompleteBecauseOfLength,
                    GetIncludeIncompleteSentencesIfLongerThanValue()));
            const int dlgResponse = listDlg.ShowModal();
            // save the checkbox status
            if (listDlg.IsCheckBoxChecked())
                {
                warningIter->Show(false);
                warningIter->SetPreviousResponse(dlgResponse);
                }
            }
        }

    ShowQueuedMessages();

    if (GetDocumentStorageMethod() == TextStorage::LoadFromExternalDocument)
        {
        RestartRealtimeUpdate();
        }

    return true;
    }

//-------------------------------------------------------
void ProjectDoc::UpdateSourceFileModifiedTime()
    {
    FilePathResolver resolvePath;
    resolvePath.ResolvePath(GetOriginalDocumentFilePath(), false);
    if ((resolvePath.IsLocalOrNetworkFile() || resolvePath.IsArchivedFile() ||
         resolvePath.IsExcelCell()) &&
        wxFileName::FileExists(resolvePath.GetResolvedPath()))
        {
        m_sourceFileLastModified = wxFileName(resolvePath.GetResolvedPath()).GetModificationTime();
        }
    }

//-------------------------------------------------------
void ProjectDoc::OnRealTimeTimer([[maybe_unused]] wxTimerEvent& event)
    {
    if (GetDocumentStorageMethod() == TextStorage::LoadFromExternalDocument)
        {
        StopRealtimeUpdate();
        const auto previousModTime{ m_sourceFileLastModified };
        UpdateSourceFileModifiedTime();
        if (m_sourceFileLastModified.IsValid() && previousModTime.IsValid() &&
            previousModTime < m_sourceFileLastModified)
            {
            RefreshRequired(RefreshRequirement::FullReindexing);
            RefreshProject();
            }
        RestartRealtimeUpdate();
        }
    }

//-------------------------------------------------------
void ProjectDoc::DisplayWordsBreakdown()
    {
    // if working with an empty project
    if (GetWords() == nullptr)
        {
        return;
        }

    DisplayWordCharts();

    auto* view = dynamic_cast<ProjectView*>(GetFirstView());

    // place the word lists beneath the last graph in the Words Breakdown section
    int lastGraphPosition{ wxNOT_FOUND };
    const auto* lastGraphWindow =
        view->GetWordsBreakdownView().RFindWindowByType(CLASSINFO(Wisteria::Canvas));
    if (lastGraphWindow != nullptr)
        {
        lastGraphPosition =
            view->GetWordsBreakdownView().FindWindowPositionById(lastGraphWindow->GetId());
        }

    const auto resetListView = [](Wisteria::UI::ListCtrlEx* listView)
    {
        if (listView != nullptr && listView->GetVirtualDataProvider() != nullptr &&
            listView->GetVirtualDataProvider()->GetItemCount() == 0)
            {
            listView->SetItemCount(0);
            }
    };

        // complex words (3+ syllable)
        {
        auto* listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
            view->GetWordsBreakdownView().FindWindowById(BaseProjectView::HARD_WORDS_LIST_PAGE_ID));
        // data will be null if call to LoadHardWords() failed
        // (will happen if document was missing or other project failure).
        if (GetWordsBreakdownInfo().Is3PlusSyllablesEnabled() &&
            GetTotalUnique3PlusSyllableWords() > 0 && Get3SyllablePlusData())
            {
            if (listView != nullptr)
                {
                listView->SetVirtualDataSize(GetTotalUnique3PlusSyllableWords());
                listView->Resort();
                listView->DistributeColumns();
                }
            else
                {
                listView = new Wisteria::UI::ListCtrlEx(
                    view->GetSplitter(), BaseProjectView::HARD_WORDS_LIST_PAGE_ID,
                    wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
                listView->Hide();
                listView->SetLabel(BaseProjectView::GetThreeSyllableListWordsLabel());
                listView->SetName(BaseProjectView::GetThreeSyllableListWordsLabel());
                listView->EnableGridLines();
                listView->InsertColumn(0, _(L"Word"));
                listView->InsertColumn(1, _(L"Syllable Count"));
                listView->InsertColumn(2, _(L"Frequency"));
                listView->InsertColumn(3, _(L"Suggestion"));
                listView->SetVirtualDataProvider(Get3SyllablePlusData());
                listView->SetVirtualDataSize(GetTotalUnique3PlusSyllableWords());
                listView->DistributeColumns();
                listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
                UpdateListOptions(listView);
                // sort by syllable count, then frequency, then words (highest to lowest)
                std::vector<std::pair<size_t, Wisteria::SortDirection>> columnsToSort;
                columnsToSort.emplace_back(1, Wisteria::SortDirection::SortDescending);
                columnsToSort.emplace_back(2, Wisteria::SortDirection::SortDescending);
                columnsToSort.emplace_back(0, Wisteria::SortDirection::SortAscending);
                listView->SortColumns(columnsToSort);

                if (view->GetWordsBreakdownView().GetWindowCount() == 0)
                    {
                    view->GetWordsBreakdownView().AddWindow(listView);
                    }
                else
                    {
                    view->GetWordsBreakdownView().InsertWindow(++lastGraphPosition, listView);
                    }
                }
            }
        else
            {
            resetListView(listView);
            // we are getting rid of this window (if it was included before)
            view->GetWordsBreakdownView().RemoveWindowById(
                BaseProjectView::HARD_WORDS_LIST_PAGE_ID);
            }
        }

        // long words (6+ characters)
        {
        auto* listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
            view->GetWordsBreakdownView().FindWindowById(BaseProjectView::LONG_WORDS_LIST_PAGE_ID));
        if (GetWordsBreakdownInfo().Is6PlusCharacterEnabled() &&
            GetTotalUnique6CharsPlusWords() > 0 && Get6CharacterPlusData())
            {
            if (listView != nullptr)
                {
                listView->SetVirtualDataSize(GetTotalUnique6CharsPlusWords());
                listView->Resort();
                listView->DistributeColumns();
                }
            else
                {
                listView = new Wisteria::UI::ListCtrlEx(
                    view->GetSplitter(), BaseProjectView::LONG_WORDS_LIST_PAGE_ID,
                    wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
                listView->Hide();
                listView->SetLabel(BaseProjectView::GetSixCharWordsListLabel());
                listView->SetName(BaseProjectView::GetSixCharWordsListLabel());
                listView->EnableGridLines();
                listView->InsertColumn(0, _(L"Word"));
                listView->InsertColumn(1, _(L"Character Count"));
                listView->InsertColumn(2, _(L"Frequency"));
                listView->InsertColumn(3, _(L"Suggestion"));
                listView->SetVirtualDataProvider(Get6CharacterPlusData());
                listView->SetVirtualDataSize(GetTotalUnique6CharsPlusWords());
                listView->DistributeColumns();
                listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
                UpdateListOptions(listView);
                // sort by character count, then frequency, then word (highest to lowest)
                std::vector<std::pair<size_t, Wisteria::SortDirection>> columnsToSort;
                columnsToSort.emplace_back(1, Wisteria::SortDirection::SortDescending);
                columnsToSort.emplace_back(2, Wisteria::SortDirection::SortDescending);
                columnsToSort.emplace_back(0, Wisteria::SortDirection::SortAscending);
                listView->SortColumns(columnsToSort);

                if (view->GetWordsBreakdownView().GetWindowCount() == 0)
                    {
                    view->GetWordsBreakdownView().AddWindow(listView);
                    }
                else
                    {
                    view->GetWordsBreakdownView().InsertWindow(++lastGraphPosition, listView);
                    }
                }
            }
        else
            {
            resetListView(listView);
            // we are getting rid of this window (if it was included before)
            view->GetWordsBreakdownView().RemoveWindowById(
                BaseProjectView::LONG_WORDS_LIST_PAGE_ID);
            }
        }

        // hard words (DC)
        {
        auto* listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
            view->GetWordsBreakdownView().FindWindowById(BaseProjectView::DC_WORDS_LIST_PAGE_ID));
        if (GetWordsBreakdownInfo().IsDCUnfamiliarEnabled() && IsDaleChallLikeTestIncluded() &&
            GetTotalUniqueDCHardWords() > 0 && GetDaleChallHardWordData())
            {
            if (listView != nullptr)
                {
                listView->SetVirtualDataSize(GetTotalUniqueDCHardWords());
                listView->Resort();
                listView->DistributeColumns();
                }
            else
                {
                listView = new Wisteria::UI::ListCtrlEx(
                    view->GetSplitter(), BaseProjectView::DC_WORDS_LIST_PAGE_ID, wxDefaultPosition,
                    wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
                listView->Hide();
                listView->SetLabel(BaseProjectView::GetDaleChallLabel());
                listView->SetName(wxString::Format(_(L"%s (Unfamiliar) List"),
                                                   BaseProjectView::GetDaleChallLabel()));
                listView->EnableGridLines();
                listView->InsertColumn(0, _(L"Unfamiliar Word"));
                listView->InsertColumn(1, _(L"Frequency"));
                listView->InsertColumn(2, _(L"Suggestion"));
                listView->SetVirtualDataProvider(GetDaleChallHardWordData());
                listView->SetVirtualDataSize(GetTotalUniqueDCHardWords());
                listView->DistributeColumns();
                listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
                UpdateListOptions(listView);

                if (view->GetWordsBreakdownView().GetWindowCount() == 0)
                    {
                    view->GetWordsBreakdownView().AddWindow(listView);
                    }
                else
                    {
                    view->GetWordsBreakdownView().InsertWindow(++lastGraphPosition, listView);
                    }
                }
            }
        else
            {
            resetListView(listView);
            // we are getting rid of this window (if nothing in it)
            view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::DC_WORDS_LIST_PAGE_ID);
            }
        }

        // hard words (Spache)
        {
        auto* listView =
            dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetWordsBreakdownView().FindWindowById(
                BaseProjectView::SPACHE_WORDS_LIST_PAGE_ID));
        if (GetWordsBreakdownInfo().IsSpacheUnfamiliarEnabled() &&
            GetReadabilityTests().is_test_included(ReadabilityMessages::SPACHE()) &&
            GetTotalUniqueHardWordsSpache() > 0 && GetSpacheHardWordData())
            {
            if (listView != nullptr)
                {
                listView->SetVirtualDataSize(GetTotalUniqueHardWordsSpache());
                listView->Resort();
                listView->DistributeColumns();
                }
            else
                {
                listView = new Wisteria::UI::ListCtrlEx(
                    view->GetSplitter(), BaseProjectView::SPACHE_WORDS_LIST_PAGE_ID,
                    wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
                listView->Hide();
                listView->SetLabel(BaseProjectView::GetSpacheLabel());
                listView->SetName(wxString::Format(_(L"%s (Unfamiliar) List"),
                                                   BaseProjectView::GetSpacheLabel()));
                listView->EnableGridLines();
                listView->InsertColumn(0, _(L"Unfamiliar Word"));
                listView->InsertColumn(1, _(L"Frequency"));
                listView->InsertColumn(2, _(L"Suggestion"));
                listView->SetVirtualDataProvider(GetSpacheHardWordData());
                listView->SetVirtualDataSize(GetTotalUniqueHardWordsSpache());
                listView->DistributeColumns();
                listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
                UpdateListOptions(listView);

                if (view->GetWordsBreakdownView().GetWindowCount() == 0)
                    {
                    view->GetWordsBreakdownView().AddWindow(listView);
                    }
                else
                    {
                    view->GetWordsBreakdownView().InsertWindow(++lastGraphPosition, listView);
                    }
                }
            }
        else
            {
            resetListView(listView);
            // we are getting rid of this window (if nothing in it)
            view->GetWordsBreakdownView().RemoveWindowById(
                BaseProjectView::SPACHE_WORDS_LIST_PAGE_ID);
            }
        }

        // hard words (Harris-Jacobson)
        {
        auto* listView =
            dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetWordsBreakdownView().FindWindowById(
                BaseProjectView::HARRIS_JACOBSON_WORDS_LIST_PAGE_ID));
        if (GetWordsBreakdownInfo().IsHarrisJacobsonUnfamiliarEnabled() &&
            GetReadabilityTests().is_test_included(ReadabilityMessages::HARRIS_JACOBSON()) &&
            GetTotalUniqueHarrisJacobsonHardWords() > 0 && GetHarrisJacobsonHardWordDataData())
            {
            if (listView != nullptr)
                {
                listView->SetVirtualDataSize(GetTotalUniqueHarrisJacobsonHardWords());
                listView->Resort();
                listView->DistributeColumns();
                }
            else
                {
                listView = new Wisteria::UI::ListCtrlEx(
                    view->GetSplitter(), BaseProjectView::HARRIS_JACOBSON_WORDS_LIST_PAGE_ID,
                    wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
                listView->Hide();
                listView->SetLabel(BaseProjectView::GetHarrisJacobsonLabel());
                listView->SetName(wxString::Format(_(L"%s (Unfamiliar) List"),
                                                   BaseProjectView::GetHarrisJacobsonLabel()));
                listView->EnableGridLines();
                listView->InsertColumn(0, _(L"Unfamiliar Word"));
                listView->InsertColumn(1, _(L"Frequency"));
                listView->InsertColumn(2, _(L"Suggestion"));
                listView->SetVirtualDataProvider(GetHarrisJacobsonHardWordDataData());
                listView->SetVirtualDataSize(GetTotalUniqueHarrisJacobsonHardWords());
                listView->DistributeColumns();
                listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
                UpdateListOptions(listView);

                if (view->GetWordsBreakdownView().GetWindowCount() == 0)
                    {
                    view->GetWordsBreakdownView().AddWindow(listView);
                    }
                else
                    {
                    view->GetWordsBreakdownView().InsertWindow(++lastGraphPosition, listView);
                    }
                }
            }
        else
            {
            resetListView(listView);
            // we are getting rid of this window (if nothing in it)
            view->GetWordsBreakdownView().RemoveWindowById(
                BaseProjectView::HARRIS_JACOBSON_WORDS_LIST_PAGE_ID);
            }
        }

    // custom hard words
    for (auto& customWordTest : GetCustTestsInUse())
        {
        auto* listView =
            dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetWordsBreakdownView().FindWindowById(
                customWordTest.GetIterator()->get_interface_id(),
                CLASSINFO(Wisteria::UI::ListCtrlEx)));
        if (GetWordsBreakdownInfo().IsCustomTestsUnfamiliarEnabled() &&
            customWordTest.GetIterator()->is_using_familiar_words() &&
            customWordTest.GetUniqueUnfamiliarWordCount() > 0 && customWordTest.GetListViewData())
            {
            if (listView != nullptr)
                {
                listView->SetVirtualDataProvider(customWordTest.GetListViewData());
                listView->SetVirtualDataSize(customWordTest.GetUniqueUnfamiliarWordCount());
                listView->Resort();
                listView->DistributeColumns();
                }
            else
                {
                listView = new Wisteria::UI::ListCtrlEx(
                    view->GetSplitter(), customWordTest.GetIterator()->get_interface_id(),
                    wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
                listView->Hide();
                listView->SetLabel(customWordTest.GetIterator()->get_name().c_str());
                listView->SetName(wxString::Format(
                    _(L"%s (Unfamiliar) List"), customWordTest.GetIterator()->get_name().c_str()));
                listView->EnableGridLines();
                listView->InsertColumn(0, _(L"Unfamiliar Word"));
                listView->InsertColumn(1, _(L"Frequency"));
                listView->SetVirtualDataProvider(customWordTest.GetListViewData());
                listView->SetVirtualDataSize(customWordTest.GetUniqueUnfamiliarWordCount());
                listView->DistributeColumns();
                listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
                UpdateListOptions(listView);

                if (view->GetWordsBreakdownView().GetWindowCount() == 0)
                    {
                    view->GetWordsBreakdownView().AddWindow(listView);
                    }
                else
                    {
                    view->GetWordsBreakdownView().InsertWindow(++lastGraphPosition, listView);
                    }
                }
            }
        else
            {
            resetListView(listView);
            // we are getting rid of this window (if it was included before)
            view->GetWordsBreakdownView().RemoveWindowById(
                customWordTest.GetIterator()->get_interface_id());
            }
        }

        // all words
        {
        auto* listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
            view->GetWordsBreakdownView().FindWindowById(BaseProjectView::ALL_WORDS_LIST_PAGE_ID));
        if (GetWordsBreakdownInfo().IsAllWordsEnabled() && GetTotalWords() > 0 &&
            GetAllWordsBaseData())
            {
            if (listView != nullptr)
                {
                listView->SetVirtualDataSize(GetAllWordsBaseData()->GetItemCount());
                listView->Resort();
                listView->DistributeColumns();
                }
            else
                {
                listView = new Wisteria::UI::ListCtrlEx(
                    view->GetSplitter(), BaseProjectView::ALL_WORDS_LIST_PAGE_ID, wxDefaultPosition,
                    wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
                listView->Hide();
                listView->SetLabel(BaseProjectView::GetAllWordsLabel());
                listView->SetName(BaseProjectView::GetAllWordsLabel());
                listView->EnableGridLines();
                listView->InsertColumn(0, _(L"Word"));
                listView->InsertColumn(1, _(L"Frequency"));
                listView->InsertColumn(2, _(L"Syllable Count"));
                listView->InsertColumn(3, _(L"Character Count"));
                listView->SetVirtualDataProvider(GetAllWordsBaseData());
                listView->SetVirtualDataSize(GetAllWordsBaseData()->GetItemCount());
                listView->DistributeColumns();
                listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
                UpdateListOptions(listView);

                if (view->GetWordsBreakdownView().GetWindowCount() == 0)
                    {
                    view->GetWordsBreakdownView().AddWindow(listView);
                    }
                else
                    {
                    view->GetWordsBreakdownView().InsertWindow(++lastGraphPosition, listView);
                    }
                }
            }
        else
            {
            resetListView(listView);
            // we are getting rid of this window (if nothing in it)
            view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::ALL_WORDS_LIST_PAGE_ID);
            }
        }

        // keywords list
        {
        auto* listView =
            dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetWordsBreakdownView().FindWindowById(
                BaseProjectView::ALL_WORDS_CONDENSED_LIST_PAGE_ID));
        if (GetWordsBreakdownInfo().IsKeyWordsEnabled() && GetTotalWords() > 0 &&
            GetKeyWordsBaseData() &&
            // don't bother with condensed list if it has the same item count as the all words list
            // (that would mean that there was no condensing [stemming]
            // that took place and that these lists are the same).
            (GetKeyWordsBaseData()->GetItemCount() != GetAllWordsBaseData()->GetItemCount()))
            {
            if (listView != nullptr)
                {
                listView->SetVirtualDataSize(GetKeyWordsBaseData()->GetItemCount());
                listView->Resort();
                listView->DistributeColumns();
                }
            else
                {
                listView = new Wisteria::UI::ListCtrlEx(
                    view->GetSplitter(), BaseProjectView::ALL_WORDS_CONDENSED_LIST_PAGE_ID,
                    wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
                listView->Hide();
                listView->SetLabel(BaseProjectView::GetKeyWordsLabel());
                listView->SetName(BaseProjectView::GetKeyWordsLabel());
                listView->EnableGridLines();
                listView->InsertColumn(0, _(L"Word"));
                listView->InsertColumn(1, _(L"Frequency"));
                listView->SetVirtualDataProvider(GetKeyWordsBaseData());
                listView->SetVirtualDataSize(GetKeyWordsBaseData()->GetItemCount());
                listView->DistributeColumns();
                listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
                UpdateListOptions(listView);
                if (view->GetWordsBreakdownView().GetWindowCount() == 0)
                    {
                    view->GetWordsBreakdownView().AddWindow(listView);
                    }
                else
                    {
                    view->GetWordsBreakdownView().InsertWindow(++lastGraphPosition, listView);
                    }
                }
            }
        else
            {
            resetListView(listView);
            // we are getting rid of this window (if nothing in it)
            view->GetWordsBreakdownView().RemoveWindowById(
                BaseProjectView::ALL_WORDS_CONDENSED_LIST_PAGE_ID);
            }
        }

#ifndef NDEBUG
        // proper nouns
        {
        auto* listView =
            dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetWordsBreakdownView().FindWindowById(
                BaseProjectView::PROPER_NOUNS_LIST_PAGE_ID));
        if (GetWordsBreakdownInfo().IsProperNounsEnabled() && GetProperNounsData() &&
            (GetProperNounsData()->GetItemCount() != 0U))
            {
            if (listView != nullptr)
                {
                listView->SetVirtualDataSize(GetProperNounsData()->GetItemCount());
                listView->Resort();
                listView->DistributeColumns();
                }
            else
                {
                listView = new Wisteria::UI::ListCtrlEx(
                    view->GetSplitter(), BaseProjectView::PROPER_NOUNS_LIST_PAGE_ID,
                    wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
                listView->Hide();
                listView->SetLabel(BaseProjectView::GetProperNounsLabel());
                listView->SetName(BaseProjectView::GetProperNounsLabel());
                listView->EnableGridLines();
                listView->InsertColumn(0, _(L"Proper Noun"));
                listView->InsertColumn(1, _(L"Frequency"));
                listView->InsertColumn(2, _(L"Personal"));
                listView->SetVirtualDataProvider(GetProperNounsData());
                listView->SetVirtualDataSize(GetProperNounsData()->GetItemCount());
                listView->DistributeColumns();
                listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
                UpdateListOptions(listView);

                if (view->GetWordsBreakdownView().GetWindowCount() == 0)
                    {
                    view->GetWordsBreakdownView().AddWindow(listView);
                    }
                else
                    {
                    view->GetWordsBreakdownView().InsertWindow(++lastGraphPosition, listView);
                    }
                }
            }
        else
            {
            resetListView(listView);
            // we are getting rid of this window (if nothing in it)
            view->GetWordsBreakdownView().RemoveWindowById(
                BaseProjectView::PROPER_NOUNS_LIST_PAGE_ID);
            }
        }

        // contractions
        {
        auto* listView =
            dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetWordsBreakdownView().FindWindowById(
                BaseProjectView::CONTRACTIONS_LIST_PAGE_ID));
        if (GetWordsBreakdownInfo().IsContractionsEnabled() && GetContractionsData() &&
            (GetContractionsData()->GetItemCount() != 0U))
            {
            if (listView != nullptr)
                {
                listView->SetVirtualDataSize(GetContractionsData()->GetItemCount());
                listView->Resort();
                listView->DistributeColumns();
                }
            else
                {
                listView = new Wisteria::UI::ListCtrlEx(
                    view->GetSplitter(), BaseProjectView::CONTRACTIONS_LIST_PAGE_ID,
                    wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
                listView->Hide();
                listView->SetLabel(BaseProjectView::GetContractionsLabel());
                listView->SetName(BaseProjectView::GetContractionsLabel());
                listView->EnableGridLines();
                listView->InsertColumn(0, _(L"Contraction"));
                listView->InsertColumn(1, _(L"Frequency"));
                listView->SetVirtualDataProvider(GetContractionsData());
                listView->SetVirtualDataSize(GetContractionsData()->GetItemCount());
                listView->DistributeColumns();
                listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
                UpdateListOptions(listView);

                if (view->GetWordsBreakdownView().GetWindowCount() == 0)
                    {
                    view->GetWordsBreakdownView().AddWindow(listView);
                    }
                else
                    {
                    view->GetWordsBreakdownView().InsertWindow(++lastGraphPosition, listView);
                    }
                }
            }
        else
            {
            resetListView(listView);
            // we are getting rid of this window (if nothing in it)
            view->GetWordsBreakdownView().RemoveWindowById(
                BaseProjectView::CONTRACTIONS_LIST_PAGE_ID);
            }
        }
#endif
    }

//-------------------------------------------------------
void ProjectDoc::DisplaySentenceCharts()
    {
    // if working with an empty project
    if (GetWords() == nullptr)
        {
        return;
        }

    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxGCDC gdc(view->GetDocFrame());

    // box plot of sentence lengths
    if (GetSentencesBreakdownInfo().IsLengthsBoxPlotEnabled() &&
        (m_sentenceWordLengths->GetRowCount() != 0U))
        {
        auto* sentenceBoxPlotCanvas =
            dynamic_cast<Wisteria::Canvas*>(view->GetSentencesBreakdownView().FindWindowById(
                BaseProjectView::SENTENCE_BOX_PLOT_PAGE_ID));
        if (sentenceBoxPlotCanvas == nullptr)
            {
            sentenceBoxPlotCanvas = new Wisteria::Canvas(
                view->GetSplitter(), BaseProjectView::SENTENCE_BOX_PLOT_PAGE_ID);
            sentenceBoxPlotCanvas->SetFixedObjectsGridSize(1, 1);
            sentenceBoxPlotCanvas->SetFixedObject(
                0, 0, std::make_shared<Wisteria::Graphs::BoxPlot>(sentenceBoxPlotCanvas));
            sentenceBoxPlotCanvas->Hide();
            sentenceBoxPlotCanvas->SetLabel(BaseProjectView::GetSentenceLengthBoxPlotLabel());
            sentenceBoxPlotCanvas->SetName(BaseProjectView::GetSentenceLengthBoxPlotLabel());
            sentenceBoxPlotCanvas->SetPrinterSettings(*wxGetApp().GetPrintData());
            view->GetSentencesBreakdownView().AddWindow(sentenceBoxPlotCanvas);
            }
        UpdateGraphOptions(sentenceBoxPlotCanvas);

        auto sentenceBoxPlot = std::dynamic_pointer_cast<Wisteria::Graphs::BoxPlot>(
            sentenceBoxPlotCanvas->GetFixedObject(0, 0));
        sentenceBoxPlot->SetBrushScheme(std::make_shared<Wisteria::Brushes::Schemes::BrushScheme>(
            Wisteria::Colors::Schemes::ColorScheme({ GetGraphBoxColor() })));
        sentenceBoxPlot->SetData(m_sentenceWordLengths, GetSentenceWordCountsColumnName(),
                                 std::nullopt);

        // TRANSLATORS: "Spread" means a range of numbers.
        sentenceBoxPlot->GetTitle().SetText(_(L"Sentence-lengths Spread"));
        sentenceBoxPlot->GetTitle().SetRelativeAlignment(Wisteria::RelativeAlignment::Centered);
        sentenceBoxPlot->SetShadowType(IsDisplayingDropShadows() ?
                                           Wisteria::ShadowType::RightSideAndBottomShadow :
                                           Wisteria::ShadowType::NoShadow);
        sentenceBoxPlot->ShowLabels(IsDisplayingBoxPlotLabels());
        sentenceBoxPlot->ShowAllPoints(IsShowingAllBoxPlotPoints());
        sentenceBoxPlot->SetOpacity(GetGraphBoxOpacity());
        sentenceBoxPlot->SetBoxEffect(GetGraphBoxEffect());
        if (const auto convertedIcon = Wisteria::ReportEnumConvert::ConvertIcon(GetStippleShape());
            convertedIcon)
            {
            sentenceBoxPlot->SetStippleShape(convertedIcon.value());
            }
        sentenceBoxPlot->SetStippleShapeColor(GetStippleShapeColor());

        sentenceBoxPlotCanvas->CalcAllSizes(gdc);
        }
    else
        {
        view->GetSentencesBreakdownView().RemoveWindowById(
            BaseProjectView::SENTENCE_BOX_PLOT_PAGE_ID);
        }

    // histogram of sentence lengths
    if (GetSentencesBreakdownInfo().IsLengthsHistogramEnabled() &&
        (m_sentenceWordLengths->GetRowCount() != 0U))
        {
        auto* sentenceHistogramCanvas =
            dynamic_cast<Wisteria::Canvas*>(view->GetSentencesBreakdownView().FindWindowById(
                BaseProjectView::SENTENCE_HISTOGRAM_PAGE_ID));
        if (sentenceHistogramCanvas == nullptr)
            {
            sentenceHistogramCanvas = new Wisteria::Canvas(
                view->GetSplitter(), BaseProjectView::SENTENCE_HISTOGRAM_PAGE_ID);
            sentenceHistogramCanvas->SetFixedObjectsGridSize(1, 1);
            sentenceHistogramCanvas->SetFixedObject(
                0, 0,
                std::make_shared<Wisteria::Graphs::Histogram>(
                    sentenceHistogramCanvas,
                    std::make_shared<Wisteria::Brushes::Schemes::BrushScheme>(
                        *std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                            Wisteria::Colors::Schemes::ColorScheme{ GetHistogramBarColor() }))));
            sentenceHistogramCanvas->Hide();
            sentenceHistogramCanvas->SetLabel(BaseProjectView::GetSentenceLengthHistogramLabel());
            sentenceHistogramCanvas->SetName(BaseProjectView::GetSentenceLengthHistogramLabel());
            sentenceHistogramCanvas->SetPrinterSettings(*wxGetApp().GetPrintData());
            view->GetSentencesBreakdownView().AddWindow(sentenceHistogramCanvas);
            }
        UpdateGraphOptions(sentenceHistogramCanvas);

        auto sentenceHistogram = std::dynamic_pointer_cast<Wisteria::Graphs::Histogram>(
            sentenceHistogramCanvas->GetFixedObject(0, 0));
        wxASSERT_MSG(sentenceHistogram,
                     L"Sentence histogram dynamic cast failed in DisplaySentenceCharts()!");

        sentenceHistogram->GetTitle().SetText(_(L"Sentence-lengths Distribution"));
        sentenceHistogram->GetTitle().SetRelativeAlignment(Wisteria::RelativeAlignment::Centered);
        sentenceHistogram->SetSortable(false);
        sentenceHistogram->SetShadowType(IsDisplayingDropShadows() ?
                                             Wisteria::ShadowType::RightSideShadow :
                                             Wisteria::ShadowType::NoShadow);
        sentenceHistogram->SetBarEffect(GetHistogramBarEffect());
        if (const auto convertedIcon = Wisteria::ReportEnumConvert::ConvertIcon(GetStippleShape());
            convertedIcon)
            {
            sentenceHistogram->SetStippleShape(convertedIcon.value());
            }
        sentenceHistogram->SetStippleShapeColor(GetStippleShapeColor());
        sentenceHistogram->SetBarOpacity(GetHistogramBarOpacity());
        sentenceHistogram->SetBrushScheme(std::make_shared<Wisteria::Brushes::Schemes::BrushScheme>(
            *std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                Wisteria::Colors::Schemes::ColorScheme({ GetHistogramBarColor() }))));
        sentenceHistogram->SetData(
            m_sentenceWordLengths, GetSentenceWordCountsColumnName(), std::nullopt,
            Wisteria::Graphs::Histogram::BinningMethod::BinByIntegerRange,
            Wisteria::RoundingMethod::NoRounding,
            Wisteria::Graphs::Histogram::IntervalDisplay::Midpoints,
            IsDisplayingBarChartLabels() ? GetHistogramBinLabelDisplay() :
                                           Wisteria::BinLabelDisplay::NoDisplay,
            true, 0, std::make_pair(std::nullopt, 5), true);
        sentenceHistogram->GetLeftYAxis().GetTitle().SetText(_(L"Number of Sentences"));
        sentenceHistogram->GetBottomXAxis().GetTitle().SetText(_(L"Number of Words per Sentence"));
        sentenceHistogramCanvas->CalcAllSizes(gdc);
        }
    else
        {
        // we are getting rid of this window (if nothing in it)
        view->GetSentencesBreakdownView().RemoveWindowById(
            BaseProjectView::SENTENCE_HISTOGRAM_PAGE_ID);
        }

    // heatmap of sentence lengths
    const auto paragraphCount =
        (GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences) ?
            GetWords()->get_paragraph_count() :
            GetWords()->get_valid_paragraph_count();
    if (GetSentencesBreakdownInfo().IsLengthsHeatmapEnabled() &&
        (m_sentenceWordLengths->GetRowCount() != 0U))
        {
        auto* sentenceHeatmapCanvas =
            dynamic_cast<Wisteria::Canvas*>(view->GetSentencesBreakdownView().FindWindowById(
                BaseProjectView::SENTENCE_HEATMAP_PAGE_ID));
        if (sentenceHeatmapCanvas == nullptr)
            {
            sentenceHeatmapCanvas = new Wisteria::Canvas(view->GetSplitter(),
                                                         BaseProjectView::SENTENCE_HEATMAP_PAGE_ID);
            sentenceHeatmapCanvas->SetFixedObjectsGridSize(1, 2);
            sentenceHeatmapCanvas->SetFixedObject(
                0, 0, std::make_shared<Wisteria::Graphs::HeatMap>(sentenceHeatmapCanvas));
            sentenceHeatmapCanvas->Hide();
            sentenceHeatmapCanvas->SetLabel(BaseProjectView::GetSentenceLengthHeatmapLabel());
            sentenceHeatmapCanvas->SetName(BaseProjectView::GetSentenceLengthHeatmapLabel());

            sentenceHeatmapCanvas->SetPrinterSettings(*wxGetApp().GetPrintData());
            view->GetSentencesBreakdownView().AddWindow(sentenceHeatmapCanvas);
            }
        UpdateGraphOptions(sentenceHeatmapCanvas);

        auto heatMap = std::dynamic_pointer_cast<Wisteria::Graphs::HeatMap>(
            sentenceHeatmapCanvas->GetFixedObject(0, 0));
        heatMap->SetCanvasMargins(5, 5, 5, 5);
        heatMap->SetData(m_sentenceWordLengths, GetSentenceWordCountsColumnName(),
                         ((paragraphCount <= 500) ? std::optional<wxString>(GetGroupColumnName()) :
                                                    std::nullopt),
                         (paragraphCount <= 20  ? 1 :
                          paragraphCount <= 50  ? 2 :
                          paragraphCount <= 75  ? 3 :
                          paragraphCount <= 100 ? 4 :
                                                  5));

        heatMap->GetCaption().SetText(
            (paragraphCount <= 500) ?
                _(L"Paragraph order is displayed top-to-bottom.\n"
                  "Sentence order is displayed left-to-right "
                  "within each paragraph.\nLonger sentences (relative to the document) are darker; "
                  "shorter ones are lighter.") :
                _(L"Sentence order is displayed left-to-right, top-to-bottom.\nLonger sentences "
                  "(relative to the document) are darker; shorter ones are lighter."));
        heatMap->GetTitle().SetRelativeAlignment(Wisteria::RelativeAlignment::Centered);
        heatMap->GetTitle().SetText(_(L"Sentence-lengths Heatmap"));
        heatMap->SetLeftPadding(5);
        heatMap->SetGroupHeaderPrefix(_(L"Paragraphs"));

        auto legend = heatMap->CreateLegend(
            Wisteria::Graphs::LegendOptions{}
                .PlacementHint(Wisteria::LegendCanvasPlacementHint::RightOfGraph)
                .IncludeHeader(true));
        legend->SetLine(0, _(L"Sentence Word Counts"));
        legend->SetCanvasWidthProportion(sentenceHeatmapCanvas->CalcMinWidthProportion(*legend));
        sentenceHeatmapCanvas->SetFixedObject(0, 1, std::move(legend));

        sentenceHeatmapCanvas->CalcAllSizes(gdc);
        }
    else
        {
        view->GetSentencesBreakdownView().RemoveWindowById(
            BaseProjectView::SENTENCE_HEATMAP_PAGE_ID);
        }
    }

//-------------------------------------------------------
void ProjectDoc::DisplayWordCharts()
    {
    // if working with an empty project
    if (GetWords() == nullptr)
        {
        return;
        }

    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxGCDC gdc(view->GetDocFrame());

    // word bar chart
    auto* wordBarChartCanvas = dynamic_cast<Wisteria::Canvas*>(
        view->GetWordsBreakdownView().FindWindowById(BaseProjectView::WORD_BREAKDOWN_PAGE_ID));
    if (GetWordsBreakdownInfo().IsWordBarchartEnabled() && GetTotalWords() > 0)
        {
        if (wordBarChartCanvas == nullptr)
            {
            wordBarChartCanvas =
                new Wisteria::Canvas(view->GetSplitter(), BaseProjectView::WORD_BREAKDOWN_PAGE_ID);
            wordBarChartCanvas->SetFixedObjectsGridSize(1, 1);
            wordBarChartCanvas->SetFixedObject(
                0, 0, std::make_shared<Wisteria::Graphs::BarChart>(wordBarChartCanvas));
            wordBarChartCanvas->Hide();
            wordBarChartCanvas->SetLabel(BaseProjectView::GetWordCountsLabel());
            wordBarChartCanvas->SetName(BaseProjectView::GetWordCountsLabel());
            wordBarChartCanvas->SetPrinterSettings(*wxGetApp().GetPrintData());
            std::dynamic_pointer_cast<Wisteria::Graphs::BarChart>(
                wordBarChartCanvas->GetFixedObject(0, 0))
                ->SetSortable(true);
            std::dynamic_pointer_cast<Wisteria::Graphs::BarChart>(
                wordBarChartCanvas->GetFixedObject(0, 0))
                ->SetSortDirection(Wisteria::SortDirection::NoSort);
            view->GetWordsBreakdownView().InsertWindow(0, wordBarChartCanvas);
            }
        UpdateGraphOptions(wordBarChartCanvas);

        auto wordBarChart = std::dynamic_pointer_cast<Wisteria::Graphs::BarChart>(
            wordBarChartCanvas->GetFixedObject(0, 0));
        wxASSERT_MSG(wordBarChart, L"Word bar chart dynamic cast failed in DisplayWordCharts()!");

        wordBarChart->ClearBars();
        wordBarChart->GetBarAxis().ClearBrackets();
        wordBarChart->SetBarOrientation(
            static_cast<Wisteria::Orientation>(GetBarChartOrientation()));
        wordBarChart->GetBarAxis().SetLabelDisplay(
            Wisteria::AxisLabelDisplay::DisplayOnlyCustomLabels);
        wordBarChart->GetScalingAxis().SetLabelDisplay(
            Wisteria::AxisLabelDisplay::DisplayCustomLabelsOrValues);
        wordBarChart->IncludeSpacesBetweenBars(true);
        wordBarChart->GetTitle().SetText(_(L"Word Totals (by Category)"));
        wordBarChart->GetTitle().SetRelativeAlignment(Wisteria::RelativeAlignment::Centered);
        wordBarChart->SetShadowType(IsDisplayingDropShadows() ?
                                        Wisteria::ShadowType::RightSideShadow :
                                        Wisteria::ShadowType::NoShadow);
        wordBarChart->SetBinLabelDisplay(IsDisplayingBarChartLabels() ?
                                             Wisteria::BinLabelDisplay::BinValue :
                                             Wisteria::BinLabelDisplay::NoDisplay);
        wordBarChart->GetRightYAxis().Show(false);
        wordBarChart->GetTopXAxis().Show(false);
        wordBarChart->GetScalingAxis().GetGridlinePen() = wxNullPen;
        if (const auto convertedIcon = Wisteria::ReportEnumConvert::ConvertIcon(GetStippleShape());
            convertedIcon)
            {
            wordBarChart->SetStippleShape(convertedIcon.value());
            }
        wordBarChart->SetStippleShapeColor(GetStippleShapeColor());
        // Do not change the order of these bars, brackets are built based on this order (see below)
        size_t currentBar{ 0 };
        // go through the custom (familiar word) tests
        for (const auto& cText : GetCustTestsInUse())
            {
            if (!cText.GetIterator()->is_using_familiar_words())
                {
                continue;
                }
            wordBarChart->AddBar(Wisteria::Graphs::BarChart::Bar(
                ++currentBar,
                { { Wisteria::Graphs::BarChart::BarBlock(
                    Wisteria::Graphs::BarChart::BarBlockInfo(
                        static_cast<double>(cText.GetUnfamiliarWordCount()))
                        .Brush(GetBarChartBarColor())) } },
                wxNumberFormatter::ToString(cText.GetUnfamiliarWordCount(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::GraphItems::Label(
                    wxString::Format( // TRANSLATORS: %s is a custom test name
                        _(L"%s (unfamiliar)"), cText.GetIterator()->get_name().c_str())),
                GetGraphBarEffect(), GetGraphBarOpacity()));
            }

        if (IsDaleChallLikeTestIncluded())
            {
            wordBarChart->AddBar(Wisteria::Graphs::BarChart::Bar(
                ++currentBar,
                { { Wisteria::Graphs::BarChart::BarBlock(
                    Wisteria::Graphs::BarChart::BarBlockInfo(
                        static_cast<double>(GetTotalHardWordsDaleChall()))
                        .Brush(GetBarChartBarColor())) } },
                wxNumberFormatter::ToString(GetTotalHardWordsDaleChall(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::GraphItems::Label(_(L"DC (unfamiliar)")), GetGraphBarEffect(),
                GetGraphBarOpacity()));
            }
        if (GetReadabilityTests().is_test_included(ReadabilityMessages::SPACHE()))
            {
            wordBarChart->AddBar(Wisteria::Graphs::BarChart::Bar(
                ++currentBar,
                { { Wisteria::Graphs::BarChart::BarBlock(
                    Wisteria::Graphs::BarChart::BarBlockInfo(
                        static_cast<double>(GetTotalHardWordsSpache()))
                        .Brush(GetBarChartBarColor())) } },
                wxNumberFormatter::ToString(GetTotalHardWordsSpache(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::GraphItems::Label(_(L"Spache (unfamiliar)")), GetGraphBarEffect(),
                GetGraphBarOpacity()));
            }
        if (GetReadabilityTests().is_test_included(ReadabilityMessages::HARRIS_JACOBSON()))
            {
            wordBarChart->AddBar(Wisteria::Graphs::BarChart::Bar(
                ++currentBar,
                { { Wisteria::Graphs::BarChart::BarBlock(
                    Wisteria::Graphs::BarChart::BarBlockInfo(
                        static_cast<double>(GetTotalHardWordsHarrisJacobson()))
                        .Brush(GetBarChartBarColor())) } },
                wxNumberFormatter::ToString(GetTotalHardWordsHarrisJacobson(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::GraphItems::Label(_(L"HJ (unfamiliar)")), GetGraphBarEffect(),
                GetGraphBarOpacity()));
            }
        wordBarChart->AddBar(Wisteria::Graphs::BarChart::Bar(
            ++currentBar,
            { { Wisteria::Graphs::BarChart::BarBlock(
                Wisteria::Graphs::BarChart::BarBlockInfo(
                    static_cast<double>(GetTotal3PlusSyllabicWords()))
                    .Brush(GetBarChartBarColor())) } },
            wxNumberFormatter::ToString(GetTotal3PlusSyllabicWords(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep),
            Wisteria::GraphItems::Label(_(L"3+ syllables")), GetGraphBarEffect(),
            GetGraphBarOpacity()));
        wordBarChart->AddBar(Wisteria::Graphs::BarChart::Bar(
            ++currentBar,
            { { Wisteria::Graphs::BarChart::BarBlock(
                Wisteria::Graphs::BarChart::BarBlockInfo(static_cast<double>(GetTotalLongWords()))
                    .Brush(GetBarChartBarColor())) } },
            wxNumberFormatter::ToString(GetTotalLongWords(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep),
            Wisteria::GraphItems::Label(_(L"6+ characters")), GetGraphBarEffect(),
            GetGraphBarOpacity()));
        wordBarChart->AddBar(Wisteria::Graphs::BarChart::Bar(
            ++currentBar,
            { { Wisteria::Graphs::BarChart::BarBlock(
                Wisteria::Graphs::BarChart::BarBlockInfo(
                    static_cast<double>(GetTotalMonoSyllabicWords()))
                    .Brush(GetBarChartBarColor())) } },
            wxNumberFormatter::ToString(GetTotalMonoSyllabicWords(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep),
            Wisteria::GraphItems::Label(_(L"Monosyllabic")), GetGraphBarEffect(),
            GetGraphBarOpacity()));
        if (GetReadabilityTests().is_test_included(ReadabilityMessages::EFLAW()))
            {
            wordBarChart->AddBar(Wisteria::Graphs::BarChart::Bar(
                ++currentBar,
                { { Wisteria::Graphs::BarChart::BarBlock(
                    Wisteria::Graphs::BarChart::BarBlockInfo(
                        static_cast<double>(GetTotalMiniWords()))
                        .Brush(GetBarChartBarColor())) } },
                wxNumberFormatter::ToString(GetTotalMiniWords(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::GraphItems::Label(_(L"EFLAW miniwords")), GetGraphBarEffect(),
                GetGraphBarOpacity()));
            }
        // all the words
        wordBarChart->AddBar(Wisteria::Graphs::BarChart::Bar(
            ++currentBar,
            { { Wisteria::Graphs::BarChart::BarBlock(
                Wisteria::Graphs::BarChart::BarBlockInfo(static_cast<double>(GetTotalWords()))
                    .Brush(GetBarChartBarColor())) } },
            wxNumberFormatter::ToString(GetTotalWords(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep),
            Wisteria::GraphItems::Label(_(L"Total Words")), GetGraphBarEffect(),
            GetGraphBarOpacity()));

        // add brackets around the factors of the categories
        auto threeSyllableBarPos =
            wordBarChart->GetBarAxis().FindCustomLabelPosition(_(L"3+ syllables"));
        auto monoSyllableBarPos =
            wordBarChart->GetBarAxis().FindCustomLabelPosition(_(L"Monosyllabic"));
        auto eflawBarPos =
            wordBarChart->GetBarAxis().FindCustomLabelPosition(_(L"EFLAW miniwords"));

        if (threeSyllableBarPos && threeSyllableBarPos.value() > 1)
            {
            wordBarChart->GetBarAxis().AddBracket(Wisteria::GraphItems::Axis::AxisBracket(
                1, threeSyllableBarPos.value() - 1,
                (threeSyllableBarPos.value() * math_constants::half), _("Familiarity\nFactors")));
            }

        wxASSERT_MSG(threeSyllableBarPos && monoSyllableBarPos,
                     L"Can't find bars in word bar chart in DisplayWordCharts()!");
        if (threeSyllableBarPos && monoSyllableBarPos)
            {
            wordBarChart->GetBarAxis().AddBracket(Wisteria::GraphItems::Axis::AxisBracket(
                threeSyllableBarPos.value(), eflawBarPos.value_or(monoSyllableBarPos.value()),
                ((eflawBarPos.value_or(monoSyllableBarPos.value()) - threeSyllableBarPos.value()) *
                 math_constants::half) +
                    threeSyllableBarPos.value(),
                // TRANSLATORS: "Length Factors" is a phrase that is split across lines in English.
                // Translations should treat this as a phrase and split accordingly.
                _("Complexity or\nLength\nFactors")));
            }

        for (auto& bracket : wordBarChart->GetBarAxis().GetBrackets())
            {
            bracket.SetPerpendicularLabelConnectionLinesAlignment(
                Wisteria::AxisLabelAlignment::AlignWithBoundary);
            }

        // update the bar labels
        wordBarChart->SetBinLabelDisplay(IsDisplayingBarChartLabels() ?
                                             Wisteria::BinLabelDisplay::BinValue :
                                             Wisteria::BinLabelDisplay::NoDisplay);

        // won't sort unless this is a refresh and user sorted it previously
        wordBarChart->SortBars(Wisteria::Graphs::BarChart::BarSortComparison::SortByBarLength,
                               wordBarChart->GetSortDirection());

        wordBarChartCanvas->CalcAllSizes(gdc);
        }
    else
        {
        // we are getting rid of this window (if nothing in it)
        view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::WORD_BREAKDOWN_PAGE_ID);
        }

    // syllable histogram
    auto* histoCanvas = dynamic_cast<Wisteria::Canvas*>(
        view->GetWordsBreakdownView().FindWindowById(BaseProjectView::SYLLABLE_HISTOGRAM_PAGE_ID));
    if (GetWordsBreakdownInfo().IsSyllableGraphsEnabled() && GetTotalWords() > 0)
        {
        if (histoCanvas == nullptr)
            {
            histoCanvas = new Wisteria::Canvas(view->GetSplitter(),
                                               BaseProjectView::SYLLABLE_HISTOGRAM_PAGE_ID);
            histoCanvas->SetFixedObjectsGridSize(1, 1);
            histoCanvas->SetFixedObject(
                0, 0,
                std::make_shared<Wisteria::Graphs::Histogram>(
                    histoCanvas,
                    std::make_shared<Wisteria::Brushes::Schemes::BrushScheme>(
                        *std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                            Wisteria::Colors::Schemes::ColorScheme({ GetHistogramBarColor() })))));
            histoCanvas->Hide();
            histoCanvas->SetLabel(BaseProjectView::GetSyllableCountsLabel());
            histoCanvas->SetName(BaseProjectView::GetSyllableCountsLabel());
            histoCanvas->SetPrinterSettings(*wxGetApp().GetPrintData());
            const auto wordChartPosition = view->GetWordsBreakdownView().FindWindowPositionById(
                BaseProjectView::WORD_BREAKDOWN_PAGE_ID);
            view->GetWordsBreakdownView().InsertWindow(
                (wordChartPosition != wxNOT_FOUND) ? wordChartPosition + 1 : 0, histoCanvas);
            }
        UpdateGraphOptions(histoCanvas);

        auto syllableHistogram = std::dynamic_pointer_cast<Wisteria::Graphs::Histogram>(
            histoCanvas->GetFixedObject(0, 0));
        wxASSERT_MSG(syllableHistogram,
                     L"Syllable histogram dynamic cast failed in DisplayWordCharts()!");

        syllableHistogram->GetTitle().SetText(_(L"Words (by Syllable Count)"));
        syllableHistogram->GetTitle().SetRelativeAlignment(Wisteria::RelativeAlignment::Centered);
        syllableHistogram->SetSortable(false);
        syllableHistogram->SetShadowType(IsDisplayingDropShadows() ?
                                             Wisteria::ShadowType::RightSideShadow :
                                             Wisteria::ShadowType::NoShadow);
        syllableHistogram->SetBarEffect(GetHistogramBarEffect());
        if (const auto convertedIcon = Wisteria::ReportEnumConvert::ConvertIcon(GetStippleShape());
            convertedIcon)
            {
            syllableHistogram->SetStippleShape(convertedIcon.value());
            }
        syllableHistogram->SetStippleShapeColor(GetStippleShapeColor());
        syllableHistogram->SetBarOpacity(GetHistogramBarOpacity());
        syllableHistogram->SetBrushScheme(std::make_shared<Wisteria::Brushes::Schemes::BrushScheme>(
            *std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                Wisteria::Colors::Schemes::ColorScheme({ GetHistogramBarColor() }))));
        syllableHistogram->SetData(m_syllableCounts, GetSyllableCountsColumnName(), std::nullopt,
                                   Wisteria::Graphs::Histogram::BinningMethod::BinUniqueValues,
                                   Wisteria::RoundingMethod::NoRounding,
                                   Wisteria::Graphs::Histogram::IntervalDisplay::Midpoints,
                                   IsDisplayingBarChartLabels() ?
                                       GetHistogramBinLabelDisplay() :
                                       Wisteria::BinLabelDisplay::NoDisplay,
                                   // show a bar for all syllable counts, starting from 1
                                   true, 1);
        if (syllableHistogram->GetBars().size() > 2)
            {
            const auto bar1Pos = syllableHistogram->GetBars()[0].GetAxisPosition();
            const auto bar2Pos = syllableHistogram->GetBars()[1].GetAxisPosition();
            const auto firstComplexBar = std::find_if(
                syllableHistogram->GetBars().cbegin(), syllableHistogram->GetBars().cend(),
                [](const auto& bar) { return bar.GetAxisPosition() >= 3; });
            if (!IsShowcasingKeyItems() && bar1Pos == 1 && bar2Pos == 2)
                {
                syllableHistogram->AddBarGroup(1, 2, _(L"Simple Words"), GetHistogramBarColor());
                }
            if (firstComplexBar != syllableHistogram->GetBars().cend())
                {
                syllableHistogram->AddBarGroup(
                    firstComplexBar->GetAxisPosition(),
                    syllableHistogram->GetBars().back().GetAxisPosition(), _(L"Complex Words"),
                    GetHistogramBarColor());
                if (IsShowcasingKeyItems())
                    {
                    std::vector<double> complexBarPositions;
                    complexBarPositions.reserve(syllableHistogram->GetBars().size());
                    for (auto complexBar{ firstComplexBar };
                         complexBar < syllableHistogram->GetBars().cend(); ++complexBar)
                        {
                        complexBarPositions.push_back(complexBar->GetAxisPosition());
                        }
                    syllableHistogram->ShowcaseBars(complexBarPositions);
                    syllableHistogram->GetLeftYAxis().Ghost(true);
                    syllableHistogram->GetBottomXAxis().Ghost(true);
                    }
                else
                    {
                    syllableHistogram->GetLeftYAxis().Ghost(false);
                    syllableHistogram->GetBottomXAxis().Ghost(false);
                    }
                }
            }

        syllableHistogram->GetLeftYAxis().GetTitle().SetText(_(L"Number of Words"));
        syllableHistogram->GetBottomXAxis().GetTitle().SetText(_(L"Number of Syllables per Word"));
        syllableHistogram->GetRightYAxis().Show(false);
        syllableHistogram->GetTopXAxis().Show(false);
        syllableHistogram->GetScalingAxis().GetGridlinePen() = wxNullPen;
        histoCanvas->CalcAllSizes(gdc);
        }
    else
        {
        // we are getting rid of this window (if nothing in it)
        view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::SYLLABLE_HISTOGRAM_PAGE_ID);
        }

    // syllable donut chart
    auto* syllablePieCanvas = dynamic_cast<Wisteria::Canvas*>(
        view->GetWordsBreakdownView().FindWindowById(BaseProjectView::SYLLABLE_PIECHART_PAGE_ID));
    if (GetWordsBreakdownInfo().IsSyllableGraphsEnabled() && GetTotalWords() > 0)
        {
        if (syllablePieCanvas == nullptr)
            {
            syllablePieCanvas = new Wisteria::Canvas(view->GetSplitter(),
                                                     BaseProjectView::SYLLABLE_PIECHART_PAGE_ID);
            syllablePieCanvas->SetFixedObjectsGridSize(1, 1);
            syllablePieCanvas->SetFixedObject(
                0, 0, std::make_shared<Wisteria::Graphs::PieChart>(syllablePieCanvas));
            syllablePieCanvas->Hide();
            syllablePieCanvas->SetLabel(BaseProjectView::GetSyllableCountsLabel());
            syllablePieCanvas->SetName(BaseProjectView::GetSyllableCountsLabel());
            syllablePieCanvas->SetPrinterSettings(*wxGetApp().GetPrintData());
            const auto wordChartPosition = view->GetWordsBreakdownView().FindWindowPositionById(
                BaseProjectView::WORD_BREAKDOWN_PAGE_ID);
            view->GetWordsBreakdownView().InsertWindow(
                (wordChartPosition != wxNOT_FOUND) ? wordChartPosition + 1 : 0, syllablePieCanvas);
            }
        UpdateGraphOptions(syllablePieCanvas);

        auto syllablePieChart = std::dynamic_pointer_cast<Wisteria::Graphs::PieChart>(
            syllablePieCanvas->GetFixedObject(0, 0));
        wxASSERT_MSG(syllablePieChart,
                     L"Syllable pie chart dynamic cast failed in DisplayWordCharts()!");

        // add a donut hole
        syllablePieChart->IncludeDonutHole(true);
        syllablePieChart->GetDonutHoleLabel().SetText(_(L"Number of\nSyllables\nper Word"));
        syllablePieChart->GetDonutHoleLabel().SetTextAlignment(
            Wisteria::TextAlignment::JustifiedAtCharacter);
        syllablePieChart->SetInnerPieMidPointLabelDisplay(
            Wisteria::BinLabelDisplay::BinNameAndPercentage);
        syllablePieChart->SetOuterPieMidPointLabelDisplay(Wisteria::BinLabelDisplay::BinPercentage);
        syllablePieChart->SetSelectable(false);

        syllablePieChart->SetData(m_syllableCounts, std::nullopt, GetWordTypeGroupColumnName(),
                                  GetSyllableCountsColumnName());

        if (IsShowcasingKeyItems())
            {
            auto groupCol = m_syllableCounts->GetCategoricalColumn(GetWordTypeGroupColumnName());
            wxASSERT_MSG(
                groupCol != m_syllableCounts->GetCategoricalColumns().cend(),
                L"Unable to get group column for syllable dataset in DisplayWordCharts()!");
            // get the complex words label from the string table
            if (groupCol != m_syllableCounts->GetCategoricalColumns().cend())
                {
                syllablePieChart->ShowcaseOuterPieSlices({ groupCol->GetStringTable()[1] });
                }
            }

        syllablePieCanvas->CalcAllSizes(gdc);
        }
    else
        {
        // we are getting rid of this window (if nothing in it)
        view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::SYLLABLE_PIECHART_PAGE_ID);
        }

    // word cloud
    auto* wordCloudCanvas = dynamic_cast<Wisteria::Canvas*>(
        view->GetWordsBreakdownView().FindWindowById(BaseProjectView::WORD_CLOUD_PAGE_ID));
    if (GetWordsBreakdownInfo().IsWordCloudEnabled() && GetTotalWords() > 0 &&
        m_keyWordsDataset != nullptr && (m_keyWordsDataset->GetRowCount() != 0U))
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

            // place beneath bar charts (if included)
            int otherGraphPosition{ wxNOT_FOUND };
            auto* otherGraphWindow =
                view->GetWordsBreakdownView().RFindWindowByType(CLASSINFO(Wisteria::Canvas));
            if (otherGraphWindow != nullptr)
                {
                otherGraphPosition =
                    view->GetWordsBreakdownView().FindWindowPositionById(otherGraphWindow->GetId());
                }
            view->GetWordsBreakdownView().InsertWindow(
                (otherGraphPosition != wxNOT_FOUND) ? otherGraphPosition + 1 : 0, wordCloudCanvas);
            }
        UpdateGraphOptions(wordCloudCanvas);

        auto wordCloud = std::dynamic_pointer_cast<Wisteria::Graphs::WordCloud>(
            wordCloudCanvas->GetFixedObject(0, 0));
        wxASSERT_MSG(wordCloud, L"Word cloud dynamic cast failed in DisplayWordCharts()!");
        // top 100 words, with a min frequency of 2
        // (unless less than 100 words, then include everything)
        wordCloud->SetData(m_keyWordsDataset, GetWordsColumnName(), GetWordsCountsColumnName(),
                           (m_keyWordsDataset->GetRowCount() < 100 ? 1 : 2), std::nullopt, 100);

        wordCloudCanvas->CalcAllSizes(gdc);
        }
    else
        {
        // we are getting rid of this window (if nothing in it)
        view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::WORD_CLOUD_PAGE_ID);
        }
    }

//-------------------------------------------------------
void ProjectDoc::AddInfleszGraph(const bool setFocus)
    {
    const size_t val = readability::szigriszt_pazos_perspicuity(
        GetTotalWords(), GetTotalSyllables(), GetTotalSentences());

    const wxString scoresColumnName{ _DT(L"SCORES") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(scoresColumnName);

    scoreDataset->AddRow(Wisteria::Data::RowInfo().Continuous({ static_cast<double>(val) }));

    // INFLESZ Scale
    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxGCDC gdc(view->GetDocFrame());

    auto* infleszGraphView = dynamic_cast<Wisteria::Canvas*>(
        view->GetReadabilityResultsView().FindWindowById(BaseProjectView::INFLESZ_GRAPH_PAGE_ID));
    if (infleszGraphView != nullptr)
        {
        auto infleszGraph = std::dynamic_pointer_cast<Wisteria::Graphs::InfleszScale>(
            infleszGraphView->GetFixedObject(0, 0));
        infleszGraph->SetData(scoreDataset, scoresColumnName);
        }
    else
        {
        infleszGraphView =
            new Wisteria::Canvas(view->GetSplitter(), BaseProjectView::INFLESZ_GRAPH_PAGE_ID);
        infleszGraphView->SetFixedObjectsGridSize(1, 1);

        infleszGraphView->Hide();
        infleszGraphView->SetLabel(BaseProjectView::GetInfleszScaleLabel());
        infleszGraphView->SetName(BaseProjectView::GetInfleszScaleLabel());
        infleszGraphView->SetPrinterSettings(*wxGetApp().GetPrintData());

        auto infleszGraph = std::make_shared<Wisteria::Graphs::InfleszScale>(
            infleszGraphView,
            std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                Wisteria::Colors::Schemes::ColorScheme{ Wisteria::Colors::ColorBrewer::GetColor(
                    Wisteria::Colors::Color::CelestialBlue) }));

        infleszGraph->SetData(scoreDataset, scoresColumnName);
        infleszGraphView->SetFixedObject(0, 0, infleszGraph);

        view->GetReadabilityResultsView().AddWindow(infleszGraphView);
        }
    UpdateGraphOptions(infleszGraphView);
    infleszGraphView->CalcAllSizes(gdc);

    // if they asked to set focus to the score, then select the graph
    if (setFocus)
        {
        view->UpdateSideBarIcons();
        view->GetSideBar()->SelectSubItem(
            view->GetSideBar()->FindSubItem(BaseProjectView::INFLESZ_GRAPH_PAGE_ID));
        }
    }

//-------------------------------------------------------
void ProjectDoc::AddCrawfordGraph(const bool setFocus)
    {
    const double gradeValue =
        readability::crawford(GetTotalWords(), GetTotalSyllables(), GetTotalSentences());
    const double syllablesPer100Words =
        GetTotalSyllables() * (safe_divide<double>(100, GetTotalWords()));

    const wxString scoresColumnName{ _DT(L"SCORES") };
    const wxString syllablesColumnName{ _DT(L"SYLLABLES") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(scoresColumnName);
    scoreDataset->AddContinuousColumn(syllablesColumnName);

    scoreDataset->AddRow(
        Wisteria::Data::RowInfo().Continuous({ gradeValue, syllablesPer100Words }));

    // Crawford graph
    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxGCDC gdc(view->GetDocFrame());

    auto* crawfordGraphView = dynamic_cast<Wisteria::Canvas*>(
        view->GetReadabilityResultsView().FindWindowById(BaseProjectView::CRAWFORD_GRAPH_PAGE_ID));
    if (crawfordGraphView != nullptr)
        {
        auto crawfordGraph = std::dynamic_pointer_cast<Wisteria::Graphs::CrawfordGraph>(
            crawfordGraphView->GetFixedObject(0, 0));
        crawfordGraph->SetData(scoreDataset, scoresColumnName, syllablesColumnName);
        }
    else
        {
        crawfordGraphView =
            new Wisteria::Canvas(view->GetSplitter(), BaseProjectView::CRAWFORD_GRAPH_PAGE_ID);
        crawfordGraphView->SetFixedObjectsGridSize(1, 1);

        crawfordGraphView->Hide();
        crawfordGraphView->SetLabel(BaseProjectView::GetCrawfordGraphLabel());
        crawfordGraphView->SetName(BaseProjectView::GetCrawfordGraphLabel());
        crawfordGraphView->SetPrinterSettings(*wxGetApp().GetPrintData());

        auto crawfordGraph = std::make_shared<Wisteria::Graphs::CrawfordGraph>(
            crawfordGraphView,
            std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                Wisteria::Colors::Schemes::ColorScheme{ Wisteria::Colors::ColorBrewer::GetColor(
                    Wisteria::Colors::Color::CelestialBlue) }));

        crawfordGraph->SetData(scoreDataset, scoresColumnName, syllablesColumnName);
        crawfordGraphView->SetFixedObject(0, 0, crawfordGraph);

        view->GetReadabilityResultsView().AddWindow(crawfordGraphView);
        }
    UpdateGraphOptions(crawfordGraphView);
    crawfordGraphView->CalcAllSizes(gdc);

    // if they asked to set focus to the score, then select the graph
    if (setFocus)
        {
        view->UpdateSideBarIcons();
        view->GetSideBar()->SelectSubItem(
            view->GetSideBar()->FindSubItem(BaseProjectView::CRAWFORD_GRAPH_PAGE_ID));
        }
    }

//-------------------------------------------------------
void ProjectDoc::AddDB2Plot(const bool setFocus)
    {
    const size_t score = readability::danielson_bryan_2(
        GetTotalWords(), GetTotalCharactersPlusPunctuation(), GetTotalSentences());

    const wxString scoresColumnName{ _DT(L"SCORES") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(scoresColumnName);

    scoreDataset->AddRow(Wisteria::Data::RowInfo().Continuous({ static_cast<double>(score) }));

    // DB2
    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxGCDC gdc(view->GetDocFrame());

    auto* db2PlotView = dynamic_cast<Wisteria::Canvas*>(
        view->GetReadabilityResultsView().FindWindowById(BaseProjectView::DB2_PAGE_ID));
    if (db2PlotView != nullptr)
        {
        auto db2Plot = std::dynamic_pointer_cast<Wisteria::Graphs::DanielsonBryan2Plot>(
            db2PlotView->GetFixedObject(0, 0));
        db2Plot->SetData(scoreDataset, scoresColumnName);
        }
    else
        {
        db2PlotView = new Wisteria::Canvas(view->GetSplitter(), BaseProjectView::DB2_PAGE_ID);
        db2PlotView->SetFixedObjectsGridSize(1, 1);

        db2PlotView->Hide();
        db2PlotView->SetLabel(BaseProjectView::GetDB2Label());
        db2PlotView->SetName(BaseProjectView::GetDB2Label());
        db2PlotView->SetPrinterSettings(*wxGetApp().GetPrintData());

        auto db2Plot = std::make_shared<Wisteria::Graphs::DanielsonBryan2Plot>(
            db2PlotView,
            std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                Wisteria::Colors::Schemes::ColorScheme{ Wisteria::Colors::ColorBrewer::GetColor(
                    Wisteria::Colors::Color::CelestialBlue) }));

        db2Plot->SetData(scoreDataset, scoresColumnName);
        db2PlotView->SetFixedObject(0, 0, db2Plot);
        view->GetReadabilityResultsView().AddWindow(db2PlotView);
        }
    UpdateGraphOptions(db2PlotView);

    auto db2Plot = std::dynamic_pointer_cast<Wisteria::Graphs::DanielsonBryan2Plot>(
        db2PlotView->GetFixedObject(0, 0));
    db2Plot->ShowcaseScore(IsShowcasingKeyItems());

    db2PlotView->CalcAllSizes(gdc);

    // if they asked to set focus to the score then select the graph
    if (setFocus)
        {
        view->UpdateSideBarIcons();
        view->GetSideBar()->SelectSubItem(
            view->GetSideBar()->FindSubItem(BaseProjectView::DB2_PAGE_ID));
        }
    }

//-------------------------------------------------------
void ProjectDoc::AddLixGermanGauge(const bool setFocus)
    {
    readability::german_lix_difficulty diffLevel{};
    const size_t score = readability::german_lix(diffLevel, GetTotalWords(),
                                                 GetTotalHardLixRixWords(), GetTotalSentences());

    const wxString scoresColumnName{ _DT(L"SCORES") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(scoresColumnName);

    scoreDataset->AddRow(Wisteria::Data::RowInfo().Continuous({ static_cast<double>(score) }));

    // Lix Gauge (German)
    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxGCDC gdc(view->GetDocFrame());

    auto* lixGaugeView =
        dynamic_cast<Wisteria::Canvas*>(view->GetReadabilityResultsView().FindWindowById(
            BaseProjectView::LIX_GAUGE_GERMAN_PAGE_ID));
    if (lixGaugeView != nullptr)
        {
        auto lixGauge = std::dynamic_pointer_cast<Wisteria::Graphs::LixGaugeGerman>(
            lixGaugeView->GetFixedObject(0, 0));
        lixGauge->SetData(scoreDataset, scoresColumnName);
        }
    else
        {
        lixGaugeView =
            new Wisteria::Canvas(view->GetSplitter(), BaseProjectView::LIX_GAUGE_GERMAN_PAGE_ID);
        lixGaugeView->SetFixedObjectsGridSize(1, 1);

        lixGaugeView->Hide();
        lixGaugeView->SetLabel(BaseProjectView::GetGermanLixGaugeLabel());
        lixGaugeView->SetName(BaseProjectView::GetGermanLixGaugeLabel());
        lixGaugeView->SetPrinterSettings(*wxGetApp().GetPrintData());

        auto lixGauge = std::make_shared<Wisteria::Graphs::LixGaugeGerman>(
            lixGaugeView,
            std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                Wisteria::Colors::Schemes::ColorScheme{ Wisteria::Colors::ColorBrewer::GetColor(
                    Wisteria::Colors::Color::CelestialBlue) }));

        lixGauge->SetData(scoreDataset, scoresColumnName);
        lixGaugeView->SetFixedObject(0, 0, lixGauge);
        view->GetReadabilityResultsView().AddWindow(lixGaugeView);
        }
    UpdateGraphOptions(lixGaugeView);

    // in case this option changed
    auto lixGermanPlot = std::dynamic_pointer_cast<Wisteria::Graphs::LixGaugeGerman>(
        lixGaugeView->GetFixedObject(0, 0));
    lixGermanPlot->ShowcaseScore(IsShowcasingKeyItems());
    lixGermanPlot->UseEnglishLabels(IsUsingEnglishLabelsForGermanLix());

    lixGaugeView->CalcAllSizes(gdc);

    // if they asked to set focus to the score then select the graph
    if (setFocus)
        {
        view->UpdateSideBarIcons();
        view->GetSideBar()->SelectSubItem(
            view->GetSideBar()->FindSubItem(BaseProjectView::LIX_GAUGE_GERMAN_PAGE_ID));
        }
    }

//-------------------------------------------------------
void ProjectDoc::AddLixGauge(const bool setFocus)
    {
    readability::lix_difficulty diffLevel{};
    size_t gradeLevel{ 1 };
    const size_t score = readability::lix(diffLevel, gradeLevel, GetTotalWords(),
                                          GetTotalHardLixRixWords(), GetTotalSentences());

    const wxString scoresColumnName{ _DT(L"SCORES") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(scoresColumnName);

    scoreDataset->AddRow(Wisteria::Data::RowInfo().Continuous({ static_cast<double>(score) }));

    // Lix Gauge
    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxGCDC gdc(view->GetDocFrame());

    auto* lixGaugeView = dynamic_cast<Wisteria::Canvas*>(
        view->GetReadabilityResultsView().FindWindowById(BaseProjectView::LIX_GAUGE_PAGE_ID));
    if (lixGaugeView != nullptr)
        {
        auto lixGauge = std::dynamic_pointer_cast<Wisteria::Graphs::LixGauge>(
            lixGaugeView->GetFixedObject(0, 0));
        lixGauge->SetData(scoreDataset, scoresColumnName);
        }
    else
        {
        lixGaugeView =
            new Wisteria::Canvas(view->GetSplitter(), BaseProjectView::LIX_GAUGE_PAGE_ID);
        lixGaugeView->SetFixedObjectsGridSize(1, 1);

        lixGaugeView->Hide();
        lixGaugeView->SetLabel(BaseProjectView::GetLixGaugeLabel());
        lixGaugeView->SetName(BaseProjectView::GetLixGaugeLabel());
        lixGaugeView->SetPrinterSettings(*wxGetApp().GetPrintData());

        auto lixGauge = std::make_shared<Wisteria::Graphs::LixGauge>(
            lixGaugeView,
            std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                Wisteria::Colors::Schemes::ColorScheme{ Wisteria::Colors::ColorBrewer::GetColor(
                    Wisteria::Colors::Color::CelestialBlue) }));

        lixGauge->SetData(scoreDataset, scoresColumnName);
        lixGaugeView->SetFixedObject(0, 0, lixGauge);
        view->GetReadabilityResultsView().AddWindow(lixGaugeView);
        }
    UpdateGraphOptions(lixGaugeView);
    auto lixPlot =
        std::dynamic_pointer_cast<Wisteria::Graphs::LixGauge>(lixGaugeView->GetFixedObject(0, 0));
    lixPlot->ShowcaseScore(IsShowcasingKeyItems());

    lixGaugeView->CalcAllSizes(gdc);

    // if they asked to set focus to the score then select the graph
    if (setFocus)
        {
        view->UpdateSideBarIcons();
        view->GetSideBar()->SelectSubItem(
            view->GetSideBar()->FindSubItem(BaseProjectView::LIX_GAUGE_PAGE_ID));
        }
    }

//-------------------------------------------------------
void ProjectDoc::AddFleschChart(const bool setFocus)
    {
    const auto asl = safe_divide<double>(GetTotalWords(), GetTotalSentences());
    const auto asw = safe_divide<double>(
        (GetFleschNumeralSyllabizeMethod() == FleschNumeralSyllabize::NumeralIsOneSyllable) ?
            GetTotalSyllablesNumeralsOneSyllable() :
            GetTotalSyllables(),
        GetTotalWords());
    readability::flesch_difficulty diffLevel{};
    const size_t score = readability::flesch_reading_ease(
        GetTotalWords(),
        (GetFleschNumeralSyllabizeMethod() == FleschNumeralSyllabize::NumeralIsOneSyllable) ?
            GetTotalSyllablesNumeralsOneSyllable() :
            GetTotalSyllables(),
        GetTotalSentences(), diffLevel);

    const wxString wordsColumnName{ _DT(L"WORDS") };
    const wxString scoresColumnName{ _DT(L"SCORES") };
    const wxString syllablesColumnName{ _DT(L"SYLLABLES") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(wordsColumnName);
    scoreDataset->AddContinuousColumn(scoresColumnName);
    scoreDataset->AddContinuousColumn(syllablesColumnName);

    scoreDataset->AddRow(
        Wisteria::Data::RowInfo().Continuous({ asl, static_cast<double>(score), asw }));

    // Flesch chart
    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxGCDC gdc(view->GetDocFrame());

    auto* fleschChartCanvas = dynamic_cast<Wisteria::Canvas*>(
        view->GetReadabilityResultsView().FindWindowById(BaseProjectView::FLESCH_CHART_PAGE_ID));
    if (fleschChartCanvas != nullptr)
        {
        auto fleschChart = std::dynamic_pointer_cast<Wisteria::Graphs::FleschChart>(
            fleschChartCanvas->GetFixedObject(0, 0));
        fleschChart->SetData(scoreDataset, wordsColumnName, scoresColumnName, syllablesColumnName);
        }
    else
        {
        fleschChartCanvas =
            new Wisteria::Canvas(view->GetSplitter(), BaseProjectView::FLESCH_CHART_PAGE_ID);
        fleschChartCanvas->SetFixedObjectsGridSize(1, 1);

        fleschChartCanvas->Hide();
        fleschChartCanvas->SetLabel(BaseProjectView::GetFleschChartLabel());
        fleschChartCanvas->SetName(BaseProjectView::GetFleschChartLabel());
        fleschChartCanvas->SetPrinterSettings(*wxGetApp().GetPrintData());

        auto fleschChart = std::make_shared<Wisteria::Graphs::FleschChart>(
            fleschChartCanvas,
            std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                Wisteria::Colors::Schemes::ColorScheme{ Wisteria::Colors::ColorBrewer::GetColor(
                    Wisteria::Colors::Color::CelestialBlue) }));
        wxASSERT_MSG(fleschChart,
                     L"Flesch chart creation failed in DisplayReadabilityLinePlots()!");
        fleschChart->SetData(scoreDataset, wordsColumnName, scoresColumnName, syllablesColumnName);
        fleschChartCanvas->SetFixedObject(0, 0, fleschChart);

        view->GetReadabilityResultsView().AddWindow(fleschChartCanvas);
        }
    UpdateGraphOptions(fleschChartCanvas);

    std::dynamic_pointer_cast<Wisteria::Graphs::FleschChart>(
        fleschChartCanvas->GetFixedObject(0, 0))
        ->ShowConnectionLine(IsConnectingFleschPoints());
    fleschChartCanvas->CalcAllSizes(gdc);

    // if they asked to set focus to the score then select the graph
    if (setFocus)
        {
        view->UpdateSideBarIcons();
        view->GetSideBar()->SelectSubItem(
            view->GetSideBar()->FindSubItem(BaseProjectView::FLESCH_CHART_PAGE_ID));
        }
    }

//-------------------------------------------------------
bool ProjectDoc::AddSchwartzTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::SCHWARTZ();

    const wxString wordsColumnName{ _DT(L"WORDS") };
    const wxString syllablesColumnName{ _DT(L"SYLLABLES") };
    const wxString sentencesColumnName{ _DT(L"SENTENCES") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(wordsColumnName);
    scoreDataset->AddContinuousColumn(syllablesColumnName);
    scoreDataset->AddContinuousColumn(sentencesColumnName);

    scoreDataset->AddRow(Wisteria::Data::RowInfo().Continuous(
        { GetTotalWords(), GetTotalSyllablesNumeralsOneSyllable(), GetTotalSentenceUnits() }));

    if (GetTotalWords() == 0)
        {
        LogMessage(
            wxString::Format(_(L"Unable to calculate %s: at least one word "
                               "must be present in document."),
                             GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
            _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }
    if (GetTotalSentenceUnits() == 0)
        {
        LogMessage(
            wxString::Format(_(L"Unable to calculate %s: at least one sentence "
                               "must be present in document."),
                             GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
            _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }
    // Schwartz graph
    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxGCDC gdc(view->GetDocFrame());

    auto* schwartzGraphView = dynamic_cast<Wisteria::Canvas*>(
        view->GetReadabilityResultsView().FindWindowById(BaseProjectView::SCHWARTZ_PAGE_ID));
    if (schwartzGraphView != nullptr)
        {
        auto schwartzGraph = std::dynamic_pointer_cast<Wisteria::Graphs::SchwartzGraph>(
            schwartzGraphView->GetFixedObject(0, 0));
        wxASSERT_MSG(schwartzGraph,
                     L"Schwartz graph dynamic cast failed in DisplayReadabilityLinePlots()!");
        schwartzGraph->SetData(scoreDataset, wordsColumnName, syllablesColumnName,
                               sentencesColumnName);
        }
    else
        {
        schwartzGraphView =
            new Wisteria::Canvas(view->GetSplitter(), BaseProjectView::SCHWARTZ_PAGE_ID);
        schwartzGraphView->SetFixedObjectsGridSize(1, 1);

        schwartzGraphView->Hide();
        schwartzGraphView->SetLabel(BaseProjectView::GetSchwartzLabel());
        schwartzGraphView->SetName(BaseProjectView::GetSchwartzLabel());
        schwartzGraphView->SetPrinterSettings(*wxGetApp().GetPrintData());

        auto schwartzGraph = std::make_shared<Wisteria::Graphs::SchwartzGraph>(
            schwartzGraphView,
            std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                Wisteria::Colors::Schemes::ColorScheme{ Wisteria::Colors::ColorBrewer::GetColor(
                    Wisteria::Colors::Color::CelestialBlue) }));
        schwartzGraph->SetMessageCatalog(GetReadabilityMessageCatalogPtr());
        schwartzGraph->SetData(scoreDataset, wordsColumnName, syllablesColumnName,
                               sentencesColumnName);

        schwartzGraphView->SetFixedObject(0, 0, schwartzGraph);
        view->GetReadabilityResultsView().AddWindow(schwartzGraphView);
        }
    UpdateGraphOptions(schwartzGraphView);

    auto schwartzGraph = std::dynamic_pointer_cast<Wisteria::Graphs::SchwartzGraph>(
        schwartzGraphView->GetFixedObject(0, 0));
    wxASSERT_MSG(schwartzGraph, L"Schwartz graph is null in DisplayReadabilityLinePlots()!");

    schwartzGraph->SetInvalidAreaColor(GetInvalidAreaColor());
    schwartzGraph->ShowcaseScore(IsShowcasingKeyItems());
    schwartzGraphView->CalcAllSizes(gdc);

    const auto theTest = GetReadabilityTests().find_test(currentTestKey);
    if (!VerifyTestBeforeAdding(theTest))
        {
        return false;
        }

    if (schwartzGraph->GetScores().front().IsScoreInvalid())
        {
        const wxString description = wxString(_(L"Text is too difficult to be plotted."));

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxString{}), wxString{},
            std::numeric_limits<double>::quiet_NaN(), // will be shown as an empty string
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    else
        {
        const wxString influenceDescription =
            schwartzGraph->GetScores().front().IsWordsHard() ?
                _(L"3+ syllable words in the text primarily influenced this score.") :
                _(L"The sentence lengths in the text primarily influenced this score.");
        const auto gradeValues = schwartzGraph->GetScores().front().GetScoreRange();

        // if a range between different grades, then it needs to be displayed differently
        wxString displayableGradeLevel, description;
        if (gradeValues.first != gradeValues.second)
            {
            displayableGradeLevel =
                wxString::Format(L"%zu-%zu", gradeValues.first, gradeValues.second);
            description = ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValues.first,
                                                                        gradeValues.second) +
                    L"<br /><br />" + influenceDescription,
                theTest.first->get_test());
            }
        else
            {
            displayableGradeLevel = std::to_wstring(gradeValues.first);
            if (gradeValues.first == 8)
                {
                displayableGradeLevel =
                    wxString::Format(_(L"(Advanced) %s+"), displayableGradeLevel);
                }
            description = ProjectReportFormat::FormatTestResult(
                GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValues.first) +
                    L"<br /><br />" + influenceDescription,
                theTest.first->get_test());
            }

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(safe_divide<double>(gradeValues.first + gradeValues.second, 2),
                           displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValues.first, gradeValues.second,
                GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }

    GetReadabilityTests().include_test(currentTestKey, true);

    // if they asked to set focus to the score then select the graph
    if (setFocus)
        {
        view->UpdateSideBarIcons();
        view->GetSideBar()->SelectSubItem(
            view->GetSideBar()->FindSubItem(BaseProjectView::SCHWARTZ_PAGE_ID));
        }

    return true;
    }

//-------------------------------------------------------
bool ProjectDoc::AddFraseTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::FRASE();

    const wxString wordsColumnName{ _DT(L"WORDS") };
    const wxString syllablesColumnName{ _DT(L"SYLLABLES") };
    const wxString sentencesColumnName{ _DT(L"SENTENCES") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(wordsColumnName);
    scoreDataset->AddContinuousColumn(syllablesColumnName);
    scoreDataset->AddContinuousColumn(sentencesColumnName);

    scoreDataset->AddRow(Wisteria::Data::RowInfo().Continuous(
        { GetTotalWords(), GetTotalSyllables(), GetTotalSentences() }));

    if (GetTotalWords() == 0)
        {
        LogMessage(
            wxString::Format(_(L"Unable to calculate %s: at least one word must "
                               "be present in document."),
                             GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
            _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }
    if (GetTotalSentences() == 0)
        {
        LogMessage(
            wxString::Format(_(L"Unable to calculate %s: at least one sentence must "
                               "be present in document."),
                             GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
            _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }
    // FRASE graph
    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxGCDC gdc(view->GetDocFrame());

    auto* fraseGraphView = dynamic_cast<Wisteria::Canvas*>(
        view->GetReadabilityResultsView().FindWindowById(BaseProjectView::FRASE_PAGE_ID));
    if (fraseGraphView != nullptr)
        {
        auto fraseGraph = std::dynamic_pointer_cast<Wisteria::Graphs::FraseGraph>(
            fraseGraphView->GetFixedObject(0, 0));
        wxASSERT_MSG(fraseGraph,
                     L"Frase graph dynamic cast failed in DisplayReadabilityLinePlots()!");
        fraseGraph->SetData(scoreDataset, wordsColumnName, syllablesColumnName,
                            sentencesColumnName);
        }
    else
        {
        fraseGraphView = new Wisteria::Canvas(view->GetSplitter(), BaseProjectView::FRASE_PAGE_ID);

        fraseGraphView->SetFixedObjectsGridSize(1, 1);

        fraseGraphView->Hide();
        fraseGraphView->SetLabel(BaseProjectView::GetFraseLabel());
        fraseGraphView->SetName(BaseProjectView::GetFraseLabel());
        fraseGraphView->SetPrinterSettings(*wxGetApp().GetPrintData());

        auto fraseGraph = std::make_shared<Wisteria::Graphs::FraseGraph>(
            fraseGraphView,
            std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                Wisteria::Colors::Schemes::ColorScheme{ Wisteria::Colors::ColorBrewer::GetColor(
                    Wisteria::Colors::Color::CelestialBlue) }));
        fraseGraph->SetMessageCatalog(GetReadabilityMessageCatalogPtr());
        fraseGraph->SetData(scoreDataset, wordsColumnName, syllablesColumnName,
                            sentencesColumnName);

        fraseGraphView->SetFixedObject(0, 0, fraseGraph);
        view->GetReadabilityResultsView().AddWindow(fraseGraphView);
        }
    UpdateGraphOptions(fraseGraphView);

    fraseGraphView->CalcAllSizes(gdc);

    const auto theTest = GetReadabilityTests().find_test(currentTestKey);
    if (!VerifyTestBeforeAdding(theTest))
        {
        return false;
        }

    auto fraseGraph = std::dynamic_pointer_cast<Wisteria::Graphs::FraseGraph>(
        fraseGraphView->GetFixedObject(0, 0));
    wxASSERT_MSG(fraseGraph, L"Frase graph is null in DisplayReadabilityLinePlots()!");
    if (fraseGraph->GetScores().front().IsScoreInvalid())
        {
        const wxString description = wxString(_(L"Text is too difficult to be plotted."));

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxString{}), wxString{},
            std::numeric_limits<double>::quiet_NaN() /*Will be shown as an empty string*/,
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    else
        {
        const wxString influenceDescription =
            fraseGraph->GetScores().front().IsWordsHard() ?
                _(L"3+ syllable words in the text primarily influenced this score.") :
                _(L"The sentence lengths in the text primarily influenced this score.");
        const size_t val = fraseGraph->GetScores().front().GetScore();

        const wxString description = ProjectReportFormat::FormatTestResult(
            ReadabilityMessages::GetFraseDescription(val) + L"<br /><br />" + influenceDescription,
            theTest.first->get_test());

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(std::numeric_limits<double>::quiet_NaN(), wxString{}), wxString{}, val,
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }

    GetReadabilityTests().include_test(currentTestKey, true);

    // if they asked to set focus to the score then select the graph
    if (setFocus)
        {
        view->UpdateSideBarIcons();
        view->GetSideBar()->SelectSubItem(
            view->GetSideBar()->FindSubItem(BaseProjectView::FRASE_PAGE_ID));
        }

    return true;
    }

//-------------------------------------------------------
void ProjectDoc::DisplayReadabilityGraphs()
    {
    // if working with an empty project
    if (GetWords() == nullptr)
        {
        return;
        }

    try
        {
        auto* view = dynamic_cast<ProjectView*>(GetFirstView());
        wxGCDC gdc(view->GetDocFrame());

        // remove Fry graph if test is not included (Note that this chart is added by AddFryTest,
        // not here).
        if (!GetReadabilityTests().is_test_included(ReadabilityMessages::FRY()))
            {
            view->GetReadabilityResultsView().RemoveWindowById(BaseProjectView::FRY_PAGE_ID);
            }
        // remove GPM (Spanish) Fry graph if test is not included
        // (Note that this chart is added by AddGilliamPenaMountainFryTest, not here).
        if (!GetReadabilityTests().is_test_included(ReadabilityMessages::GPM_FRY()))
            {
            view->GetReadabilityResultsView().RemoveWindowById(BaseProjectView::GPM_FRY_PAGE_ID);
            }
        // remove FRASE graph if test is not included (Note that this chart is added by
        // AddFraseTest, not here).
        if (!GetReadabilityTests().is_test_included(ReadabilityMessages::FRASE()))
            {
            view->GetReadabilityResultsView().RemoveWindowById(BaseProjectView::FRASE_PAGE_ID);
            }
        // remove Schwartz graph if test is not included (Note that this chart is added by
        // AddSchwartzTest, not here).
        if (!GetReadabilityTests().is_test_included(ReadabilityMessages::SCHWARTZ()))
            {
            view->GetReadabilityResultsView().RemoveWindowById(BaseProjectView::SCHWARTZ_PAGE_ID);
            }
        // remove Flesch Chart graph if test is not included (Note that this chart is added by
        // AddFleschTest, not here).
        if (!GetReadabilityTests().is_test_included(ReadabilityMessages::FLESCH()))
            {
            view->GetReadabilityResultsView().RemoveWindowById(
                BaseProjectView::FLESCH_CHART_PAGE_ID);
            }
        // remove DB2 graph if test is not included (Note that this chart is added by AddDB2,
        // not here).
        if (!GetReadabilityTests().is_test_included(ReadabilityMessages::DANIELSON_BRYAN_2()))
            {
            view->GetReadabilityResultsView().RemoveWindowById(BaseProjectView::DB2_PAGE_ID);
            }
        // remove Lix Gauge if test is not included (Note that this chart is added by AddLixTest,
        // not here).
        if (!GetReadabilityTests().is_test_included(ReadabilityMessages::LIX()))
            {
            view->GetReadabilityResultsView().RemoveWindowById(BaseProjectView::LIX_GAUGE_PAGE_ID);
            }
        // remove German Lix Gauge if neither German Lix test is included
        // (Note that this chart is added by AddLixGermanXXX, not here).
        if (!GetReadabilityTests().is_test_included(
                ReadabilityMessages::LIX_GERMAN_CHILDRENS_LITERATURE()) &&
            !GetReadabilityTests().is_test_included(ReadabilityMessages::LIX_GERMAN_TECHNICAL()))
            {
            view->GetReadabilityResultsView().RemoveWindowById(
                BaseProjectView::LIX_GAUGE_GERMAN_PAGE_ID);
            }
        // remove Crawford graph if test is not included (Note that this graph is added by
        // AddCrawfordTest, not here).
        if (!GetReadabilityTests().is_test_included(ReadabilityMessages::CRAWFORD()))
            {
            view->GetReadabilityResultsView().RemoveWindowById(
                BaseProjectView::CRAWFORD_GRAPH_PAGE_ID);
            }
        // remove INFLESZ Scale if test is not included (Note that this graph is added by
        // AddInfleszGraph, not here).
        if (!GetReadabilityTests().is_test_included(ReadabilityMessages::INFLESZ()))
            {
            view->GetReadabilityResultsView().RemoveWindowById(
                BaseProjectView::INFLESZ_GRAPH_PAGE_ID);
            }
        // remove Raygor graph if test is not included (Note that this graph is added by
        // AddRaygorTest, not here).
        if (!GetReadabilityTests().is_test_included(ReadabilityMessages::RAYGOR()))
            {
            view->GetReadabilityResultsView().RemoveWindowById(BaseProjectView::RAYGOR_PAGE_ID);
            }
        // need to sort the list of scores after adding the graph tests to it
        view->GetReadabilityScoresList()->GetResultsListCtrl()->SortColumn(
            0, Wisteria::SortDirection::SortAscending);

        // Dolch graphs
        if (IsIncludingDolchSightWords())
            {
            auto* coverageBarChartCanvas =
                dynamic_cast<Wisteria::Canvas*>(view->GetDolchSightWordsView().FindWindowById(
                    BaseProjectView::DOLCH_COVERAGE_CHART_PAGE_ID));
            if (coverageBarChartCanvas == nullptr)
                {
                coverageBarChartCanvas = new Wisteria::Canvas(
                    view->GetSplitter(), BaseProjectView::DOLCH_COVERAGE_CHART_PAGE_ID);
                coverageBarChartCanvas->SetFixedObjectsGridSize(1, 1);
                coverageBarChartCanvas->SetFixedObject(
                    0, 0, std::make_shared<Wisteria::Graphs::BarChart>(coverageBarChartCanvas));
                coverageBarChartCanvas->Hide();
                coverageBarChartCanvas->SetLabel(BaseProjectView::GetCoverageChartTabLabel());
                coverageBarChartCanvas->SetName(BaseProjectView::GetCoverageChartTabLabel());
                coverageBarChartCanvas->SetPrinterSettings(*wxGetApp().GetPrintData());
                std::dynamic_pointer_cast<Wisteria::Graphs::BarChart>(
                    coverageBarChartCanvas->GetFixedObject(0, 0))
                    ->SetSortable(true);
                view->GetDolchSightWordsView().AddWindow(coverageBarChartCanvas);
                }
            UpdateGraphOptions(coverageBarChartCanvas);

            auto coverageBarChart = std::dynamic_pointer_cast<Wisteria::Graphs::BarChart>(
                coverageBarChartCanvas->GetFixedObject(0, 0));
            wxASSERT_MSG(coverageBarChart,
                         L"Coverage bar chart dynamic cast failed in DisplayDolch()!");

            coverageBarChart->ClearBars();
            coverageBarChart->SetBarOrientation(
                static_cast<Wisteria::Orientation>(GetBarChartOrientation()));
            coverageBarChart->GetBarAxis().SetLabelDisplay(
                Wisteria::AxisLabelDisplay::DisplayOnlyCustomLabels);
            coverageBarChart->GetScalingAxis().SetLabelDisplay(
                Wisteria::AxisLabelDisplay::DisplayCustomLabelsOrValues);
            coverageBarChart->GetScalingAxis().SetRange(0, 100, 0, 10, 2);
            coverageBarChart->GetTitle().SetText(_(L"Dolch Word Coverage (%)"));
            coverageBarChart->GetTitle().SetRelativeAlignment(
                Wisteria::RelativeAlignment::Centered);
            coverageBarChart->SetShadowType(IsDisplayingDropShadows() ?
                                                Wisteria::ShadowType::RightSideShadow :
                                                Wisteria::ShadowType::NoShadow);
            coverageBarChart->IncludeSpacesBetweenBars(true);
            coverageBarChart->GetRightYAxis().Show(false);
            coverageBarChart->GetTopXAxis().Show(false);
            coverageBarChart->GetScalingAxis().GetGridlinePen() = wxNullPen;

            size_t currentBar = 0;
            const double dolchNounPercentage =
                safe_divide<double>((ProjectReportFormat::MAX_DOLCH_NOUNS - GetUnusedDolchNouns()),
                                    ProjectReportFormat::MAX_DOLCH_NOUNS) *
                100;
            coverageBarChart->AddBar(
                Wisteria::Graphs::BarChart::Bar(
                    ++currentBar,
                    { { Wisteria::Graphs::BarChart::BarBlock(
                        Wisteria::Graphs::BarChart::BarBlockInfo(dolchNounPercentage)
                            .Brush(GetDolchNounColor())) } },
                    wxString::Format(/* TRANSLATORS: Percentage value (%s) and % symbol (%%).
                                        '%%' can be changed and/or moved elsewhere in the string. */
                                     _("%s%%"),
                                     wxNumberFormatter::ToString(
                                         dolchNounPercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep)),
                    Wisteria::GraphItems::Label(_(L"Nouns")), GetGraphBarEffect(),
                    GetGraphBarOpacity()),
                false);

            const double dolchVerbsPercentage =
                safe_divide<double>((ProjectReportFormat::MAX_DOLCH_VERBS - GetUnusedDolchVerbs()),
                                    ProjectReportFormat::MAX_DOLCH_VERBS) *
                100;
            coverageBarChart->AddBar(
                Wisteria::Graphs::BarChart::Bar(
                    ++currentBar,
                    { { Wisteria::Graphs::BarChart::BarBlock(
                        Wisteria::Graphs::BarChart::BarBlockInfo(dolchVerbsPercentage)
                            .Brush(GetDolchVerbsColor())) } },
                    wxString::Format(/* TRANSLATORS: Percentage value (%s) and % symbol (%%).
                                        '%%' can be changed and/or moved elsewhere in the string. */
                                     _("%s%%"),
                                     wxNumberFormatter::ToString(
                                         dolchVerbsPercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep)),
                    Wisteria::GraphItems::Label(_(L"Verbs")), GetGraphBarEffect(),
                    GetGraphBarOpacity()),
                false);

            const double dolchAdjectivesPercentage =
                safe_divide<double>(
                    (ProjectReportFormat::MAX_DOLCH_ADJECTIVE_WORDS - GetUnusedDolchAdjectives()),
                    ProjectReportFormat::MAX_DOLCH_ADJECTIVE_WORDS) *
                100;
            coverageBarChart->AddBar(
                Wisteria::Graphs::BarChart::Bar(
                    ++currentBar,
                    { { Wisteria::Graphs::BarChart::BarBlock(
                        Wisteria::Graphs::BarChart::BarBlockInfo(dolchAdjectivesPercentage)
                            .Brush(GetDolchAdjectivesColor())) } },
                    wxString::Format(/* TRANSLATORS: Percentage value (%s) and % symbol (%%).
                                        '%%' can be changed and/or moved elsewhere in the string. */
                                     _("%s%%"),
                                     wxNumberFormatter::ToString(
                                         dolchAdjectivesPercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep)),
                    Wisteria::GraphItems::Label(_(L"Adjectives")), GetGraphBarEffect(),
                    GetGraphBarOpacity()),
                false);

            const double dolchAdverbsPercentage =
                safe_divide<double>(
                    (ProjectReportFormat::MAX_DOLCH_ADVERB_WORDS - GetUnusedDolchAdverbs()),
                    ProjectReportFormat::MAX_DOLCH_ADVERB_WORDS) *
                100;
            coverageBarChart->AddBar(
                Wisteria::Graphs::BarChart::Bar(
                    ++currentBar,
                    { { Wisteria::Graphs::BarChart::BarBlock(
                        Wisteria::Graphs::BarChart::BarBlockInfo(dolchAdverbsPercentage)
                            .Brush(GetDolchAdverbsColor())) } },
                    wxString::Format(/* TRANSLATORS: Percentage value (%s) and % symbol (%%).
                                        '%%' can be changed and/or moved elsewhere in the string. */
                                     _("%s%%"),
                                     wxNumberFormatter::ToString(
                                         dolchAdverbsPercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep)),
                    Wisteria::GraphItems::Label(_(L"Adverbs")), GetGraphBarEffect(),
                    GetGraphBarOpacity()),
                false);

            const double dolchPronounsPercentage =
                safe_divide<double>(
                    (ProjectReportFormat::MAX_DOLCH_PRONOUN_WORDS - GetUnusedDolchPronouns()),
                    ProjectReportFormat::MAX_DOLCH_PRONOUN_WORDS) *
                100;
            coverageBarChart->AddBar(
                Wisteria::Graphs::BarChart::Bar(
                    ++currentBar,
                    { { Wisteria::Graphs::BarChart::BarBlock(
                        Wisteria::Graphs::BarChart::BarBlockInfo(dolchPronounsPercentage)
                            .Brush(GetDolchPronounsColor())) } },
                    wxString::Format(/* TRANSLATORS: Percentage value (%s) and % symbol (%%).
                                        '%%' can be changed and/or moved elsewhere in the string. */
                                     _("%s%%"),
                                     wxNumberFormatter::ToString(
                                         dolchPronounsPercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep)),
                    Wisteria::GraphItems::Label(_(L"Pronouns")), GetGraphBarEffect(),
                    GetGraphBarOpacity()),
                false);

            const double dolchPrepositionsPercentage =
                safe_divide<double>((ProjectReportFormat::MAX_DOLCH_PREPOSITION_WORDS -
                                     GetUnusedDolchPrepositions()),
                                    ProjectReportFormat::MAX_DOLCH_PREPOSITION_WORDS) *
                100;
            coverageBarChart->AddBar(
                Wisteria::Graphs::BarChart::Bar(
                    ++currentBar,
                    { { Wisteria::Graphs::BarChart::BarBlock(
                        Wisteria::Graphs::BarChart::BarBlockInfo(dolchPrepositionsPercentage)
                            .Brush(GetDolchPrepositionsColor())) } },
                    wxString::Format(/* TRANSLATORS: Percentage value (%s) and % symbol (%%).
                                        '%%' can be changed and/or moved elsewhere in the string. */
                                     _("%s%%"),
                                     wxNumberFormatter::ToString(
                                         dolchPrepositionsPercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep)),
                    Wisteria::GraphItems::Label(_(L"Prepositions")), GetGraphBarEffect(),
                    GetGraphBarOpacity()),
                false);

            const double dolchConjunctionsPercentage =
                safe_divide<double>((ProjectReportFormat::MAX_DOLCH_CONJUNCTION_WORDS -
                                     GetUnusedDolchConjunctions()),
                                    ProjectReportFormat::MAX_DOLCH_CONJUNCTION_WORDS) *
                100;
            coverageBarChart->AddBar(
                Wisteria::Graphs::BarChart::Bar(
                    ++currentBar,
                    { { Wisteria::Graphs::BarChart::BarBlock(
                        Wisteria::Graphs::BarChart::BarBlockInfo(dolchConjunctionsPercentage)
                            .Brush(GetDolchConjunctionsColor())) } },
                    wxString::Format(/* TRANSLATORS: Percentage value (%s) and % symbol (%%).
                                        '%%' can be changed and/or moved elsewhere in the string. */
                                     _("%s%%"),
                                     wxNumberFormatter::ToString(
                                         dolchConjunctionsPercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep)),
                    Wisteria::GraphItems::Label(_(L"Conjunctions")), GetGraphBarEffect(),
                    GetGraphBarOpacity()),
                false);

            // update the bar labels
            coverageBarChart->SetBinLabelDisplay(IsDisplayingBarChartLabels() ?
                                                     Wisteria::BinLabelDisplay::BinValue :
                                                     Wisteria::BinLabelDisplay::NoDisplay);

            coverageBarChart->SortBars(
                Wisteria::Graphs::BarChart::BarSortComparison::SortByBarLength,
                coverageBarChart->GetSortDirection());

            coverageBarChartCanvas->CalcAllSizes(gdc);

            // Dolch words breakdown chart
            const size_t totalDolchWords =
                m_dolchConjunctionCounts.second + m_dolchPrepositionCounts.second +
                m_dolchPronounCounts.second + m_dolchAdverbCounts.second +
                m_dolchAdjectiveCounts.second + m_dolchVerbCounts.second + m_dolchNounCounts.second;

            auto* wordBarChartCanvas =
                dynamic_cast<Wisteria::Canvas*>(view->GetDolchSightWordsView().FindWindowById(
                    BaseProjectView::DOLCH_BREAKDOWN_PAGE_ID));
            if (wordBarChartCanvas == nullptr)
                {
                wordBarChartCanvas = new Wisteria::Canvas(view->GetSplitter(),
                                                          BaseProjectView::DOLCH_BREAKDOWN_PAGE_ID);
                wordBarChartCanvas->SetFixedObjectsGridSize(1, 1);
                wordBarChartCanvas->SetFixedObject(
                    0, 0, std::make_shared<Wisteria::Graphs::BarChart>(wordBarChartCanvas));
                wordBarChartCanvas->Hide();
                wordBarChartCanvas->SetLabel(BaseProjectView::GetWordCountsLabel());
                wordBarChartCanvas->SetName(BaseProjectView::GetWordCountsLabel());
                wordBarChartCanvas->SetPrinterSettings(*wxGetApp().GetPrintData());
                std::dynamic_pointer_cast<Wisteria::Graphs::BarChart>(
                    wordBarChartCanvas->GetFixedObject(0, 0))
                    ->SetSortable(true);
                view->GetDolchSightWordsView().AddWindow(wordBarChartCanvas);
                }
            UpdateGraphOptions(wordBarChartCanvas);
            auto dolchBarChart = std::dynamic_pointer_cast<Wisteria::Graphs::BarChart>(
                wordBarChartCanvas->GetFixedObject(0, 0));
            wxASSERT_MSG(dolchBarChart, L"Dolch bar chart dynamic cast failed in DisplayDolch()!");

            dolchBarChart->ClearBars();
            dolchBarChart->SetBarOrientation(
                static_cast<Wisteria::Orientation>(GetBarChartOrientation()));
            dolchBarChart->GetBarAxis().SetLabelDisplay(
                Wisteria::AxisLabelDisplay::DisplayOnlyCustomLabels);
            dolchBarChart->GetScalingAxis().SetLabelDisplay(
                Wisteria::AxisLabelDisplay::DisplayCustomLabelsOrValues);
            // TRANSLATORS: A breakdown is a summary.
            dolchBarChart->GetTitle().SetText(_(L"Dolch Word Breakdown"));
            dolchBarChart->GetTitle().SetRelativeAlignment(Wisteria::RelativeAlignment::Centered);
            dolchBarChart->SetShadowType(IsDisplayingDropShadows() ?
                                             Wisteria::ShadowType::RightSideShadow :
                                             Wisteria::ShadowType::NoShadow);
            dolchBarChart->IncludeSpacesBetweenBars(true);
            dolchBarChart->SetBinLabelDisplay(IsDisplayingBarChartLabels() ?
                                                  Wisteria::BinLabelDisplay::BinValue :
                                                  Wisteria::BinLabelDisplay::NoDisplay);
            dolchBarChart->GetRightYAxis().Show(false);
            dolchBarChart->GetTopXAxis().Show(false);
            dolchBarChart->GetScalingAxis().GetGridlinePen() = wxNullPen;

            currentBar = 0;

            dolchBarChart->AddBar(Wisteria::Graphs::BarChart::Bar(
                ++currentBar,
                { { Wisteria::Graphs::BarChart::BarBlock(
                    Wisteria::Graphs::BarChart::BarBlockInfo(
                        static_cast<double>(GetDolchNounCounts().second))
                        .Brush(GetDolchNounColor())) } },
                wxNumberFormatter::ToString(GetDolchNounCounts().second, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::GraphItems::Label(_(L"Nouns")), GetGraphBarEffect(),
                GetGraphBarOpacity()));

            dolchBarChart->AddBar(Wisteria::Graphs::BarChart::Bar(
                ++currentBar,
                { { Wisteria::Graphs::BarChart::BarBlock(
                    Wisteria::Graphs::BarChart::BarBlockInfo(
                        static_cast<double>(GetDolchVerbsCounts().second))
                        .Brush(GetDolchVerbsColor())) } },
                wxNumberFormatter::ToString(GetDolchVerbsCounts().second, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::GraphItems::Label(_(L"Verbs")), GetGraphBarEffect(),
                GetGraphBarOpacity()));

            dolchBarChart->AddBar(Wisteria::Graphs::BarChart::Bar(
                ++currentBar,
                { { Wisteria::Graphs::BarChart::BarBlock(
                    Wisteria::Graphs::BarChart::BarBlockInfo(
                        static_cast<double>(GetDolchAdjectiveCounts().second))
                        .Brush(GetDolchAdjectivesColor())) } },
                wxNumberFormatter::ToString(GetDolchAdjectiveCounts().second, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::GraphItems::Label(_(L"Adjectives")), GetGraphBarEffect(),
                GetGraphBarOpacity()));

            dolchBarChart->AddBar(Wisteria::Graphs::BarChart::Bar(
                ++currentBar,
                { { Wisteria::Graphs::BarChart::BarBlock(
                    Wisteria::Graphs::BarChart::BarBlockInfo(
                        static_cast<double>(GetDolchAdverbCounts().second))
                        .Brush(GetDolchAdverbsColor())) } },
                wxNumberFormatter::ToString(GetDolchAdverbCounts().second, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::GraphItems::Label(_(L"Adverbs")), GetGraphBarEffect(),
                GetGraphBarOpacity()));

            dolchBarChart->AddBar(Wisteria::Graphs::BarChart::Bar(
                ++currentBar,
                { { Wisteria::Graphs::BarChart::BarBlock(
                    Wisteria::Graphs::BarChart::BarBlockInfo(
                        static_cast<double>(GetDolchPronounCounts().second))
                        .Brush(GetDolchPronounsColor())) } },
                wxNumberFormatter::ToString(GetDolchPronounCounts().second, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::GraphItems::Label(_(L"Pronouns")), GetGraphBarEffect(),
                GetGraphBarOpacity()));

            dolchBarChart->AddBar(Wisteria::Graphs::BarChart::Bar(
                ++currentBar,
                { { Wisteria::Graphs::BarChart::BarBlock(
                    Wisteria::Graphs::BarChart::BarBlockInfo(
                        static_cast<double>(GetDolchPrepositionWordCounts().second))
                        .Brush(GetDolchPrepositionsColor())) } },
                wxNumberFormatter::ToString(GetDolchPrepositionWordCounts().second, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::GraphItems::Label(_(L"Prepositions")), GetGraphBarEffect(),
                GetGraphBarOpacity()));

            dolchBarChart->AddBar(Wisteria::Graphs::BarChart::Bar(
                ++currentBar,
                { { Wisteria::Graphs::BarChart::BarBlock(
                    Wisteria::Graphs::BarChart::BarBlockInfo(
                        static_cast<double>(GetDolchConjunctionCounts().second))
                        .Brush(GetDolchConjunctionsColor())) } },
                wxNumberFormatter::ToString(GetDolchConjunctionCounts().second, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::GraphItems::Label(_(L"Conjunctions")), GetGraphBarEffect(),
                GetGraphBarOpacity()));

            dolchBarChart->AddBar(Wisteria::Graphs::BarChart::Bar(
                ++currentBar,
                { { Wisteria::Graphs::BarChart::BarBlock(
                    Wisteria::Graphs::BarChart::BarBlockInfo(
                        static_cast<double>(GetTotalWords() - totalDolchWords))
                        .Brush(GetBarChartBarColor())) } },
                wxNumberFormatter::ToString((GetTotalWords() - totalDolchWords), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::GraphItems::Label(_(L"Non-Dolch Words")), GetGraphBarEffect(),
                GetGraphBarOpacity()));

            dolchBarChart->AddBar(Wisteria::Graphs::BarChart::Bar(
                ++currentBar,
                { { Wisteria::Graphs::BarChart::BarBlock(
                    Wisteria::Graphs::BarChart::BarBlockInfo(static_cast<double>(totalDolchWords))
                        .Brush(GetBarChartBarColor())) } },
                wxNumberFormatter::ToString(totalDolchWords, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::GraphItems::Label(_(L"Total Dolch Words")), GetGraphBarEffect(),
                GetGraphBarOpacity()));

            // all the words
            dolchBarChart->AddBar(Wisteria::Graphs::BarChart::Bar(
                ++currentBar,
                { { Wisteria::Graphs::BarChart::BarBlock(
                    Wisteria::Graphs::BarChart::BarBlockInfo(static_cast<double>(GetTotalWords()))
                        .Brush(GetBarChartBarColor())) } },
                wxNumberFormatter::ToString(GetTotalWords(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::GraphItems::Label(_(L"Total Words")), GetGraphBarEffect(),
                GetGraphBarOpacity()));

            // update the bar labels
            dolchBarChart->SetBinLabelDisplay(IsDisplayingBarChartLabels() ?
                                                  Wisteria::BinLabelDisplay::BinValue :
                                                  Wisteria::BinLabelDisplay::NoDisplay);

            dolchBarChart->SortBars(Wisteria::Graphs::BarChart::BarSortComparison::SortByBarLength,
                                    dolchBarChart->GetSortDirection());

            wordBarChartCanvas->CalcAllSizes(gdc);
            }
        else
            {
            view->GetDolchSightWordsView().RemoveWindowById(
                BaseProjectView::DOLCH_COVERAGE_CHART_PAGE_ID);
            view->GetDolchSightWordsView().RemoveWindowById(
                BaseProjectView::DOLCH_BREAKDOWN_PAGE_ID);
            }
        }
    catch (...)
        {
        LogMessage(_(L"An unknown error occurred. Unable to create graphs."), _(L"Error"),
                   wxOK | wxICON_EXCLAMATION);
        }
    }

//-------------------------------------------------------
void ProjectDoc::DisplaySentencesBreakdown()
    {
    DisplayOverlyLongSentences();
    DisplaySentenceCharts();
    }

/// load the general stats view
//-------------------------------------------------------
void ProjectDoc::DisplayStatistics()
    {
    // this area can be included for an empty project, just won't show anything
    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxASSERT_MSG(view, L"Project view is null in DisplayStatistics()!");

    if (GetStatisticsInfo().IsReportEnabled())
        {
        auto* summaryReportWindow = dynamic_cast<wxWebView*>(
            view->GetSummaryView().FindWindowById(BaseProjectView::STATS_REPORT_PAGE_ID));
        if (summaryReportWindow == nullptr)
            {
            summaryReportWindow =
                wxWebView::New(view->GetSplitter(), BaseProjectView::STATS_REPORT_PAGE_ID);
            if (summaryReportWindow != nullptr)
                {
                summaryReportWindow->Hide();
                summaryReportWindow->SetLabel(BaseProjectView::GetFormattedReportLabel());
                summaryReportWindow->SetName(BaseProjectView::GetFormattedReportLabel());
                summaryReportWindow->EnableContextMenu(false);
                summaryReportWindow->Bind(wxEVT_WEBVIEW_NAVIGATING,
                                          &ProjectView::OnExplanationNavigating, view);
                }
            }

        if (summaryReportWindow != nullptr)
            {
            wxString formattedStats =
                ProjectReportFormat::FormatHtmlReportStart(
                    wxString::Format( // TRANSLATORS: %s is the project name
                        _(L"Statistics Report [%s]"), GetTitle()),
                    wxGetApp().GetAppOptions()->GetReportTheme()) +
                ProjectReportFormat::FormatReportBanner(_(L"Statistics Summary"), GetTitle()) +
                ProjectReportFormat::FormatStatisticsInfo(this, GetStatisticsReportInfo(),
                                                          view->GetSummaryStatisticsListData()) +
                ProjectReportFormat::FormatHtmlReportEnd();
            // if document failed to be loaded, and we are just showing the basic stats,
            // then remove the links to the various windows that won't be shown
            if (!LoadingOriginalTextSucceeded())
                {
                std::wstring strippedStatsText{ formattedStats };
                lily_of_the_valley::html_format::strip_hyperlinks(strippedStatsText);
                formattedStats = strippedStatsText;
                }
            summaryReportWindow->SetPage(NavLink::AnchorsToExplanationScheme(formattedStats),
                                         wxString{});
            view->GetSummaryView().InsertWindow(0, summaryReportWindow);
            }
        }
    else
        {
        view->GetSummaryView().RemoveWindowById(BaseProjectView::STATS_REPORT_PAGE_ID);
        }

    if (GetStatisticsInfo().IsTableEnabled())
        {
        auto* tabularStatsList = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
            view->GetSummaryView().FindWindowById(BaseProjectView::STATS_LIST_PAGE_ID));
        if (tabularStatsList == nullptr)
            {
            tabularStatsList = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::STATS_LIST_PAGE_ID, wxDefaultPosition,
                wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
            tabularStatsList->Hide();
            tabularStatsList->SetLabel(BaseProjectView::GetTabularReportLabel());
            tabularStatsList->SetName(BaseProjectView::GetTabularReportLabel());
            tabularStatsList->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
            UpdateListOptions(tabularStatsList);
            tabularStatsList->EnableGridLines();
            tabularStatsList->EnableItemViewOnDblClick();
            tabularStatsList->InsertColumn(0, _(L"Statistic"));
            tabularStatsList->InsertColumn(1, _(L"Value"));
            tabularStatsList->InsertColumn(2, _(L"Percentage"));
            tabularStatsList->SetVirtualDataProvider(view->GetSummaryStatisticsListData());
            }
        tabularStatsList->SetVirtualDataSize(
            view->GetSummaryStatisticsListData()->GetItemCount(),
            view->GetSummaryStatisticsListData()->GetColumnCount());
        tabularStatsList->SetColumnWidth(0, tabularStatsList->EstimateColumnWidth(0));
        tabularStatsList->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);
        tabularStatsList->SetColumnWidth(2, tabularStatsList->EstimateColumnWidth(2));
        tabularStatsList->Resort();
        tabularStatsList->DistributeColumns();
        const auto buddyWindowPosition =
            view->GetSummaryView().FindWindowPositionById(BaseProjectView::STATS_REPORT_PAGE_ID);
        view->GetSummaryView().InsertWindow(
            (buddyWindowPosition != wxNOT_FOUND) ? buddyWindowPosition + 1 : 0, tabularStatsList);
        }
    else
        {
        // we are getting rid of this window (if it was included before)
        view->GetSummaryView().RemoveWindowById(BaseProjectView::STATS_LIST_PAGE_ID);
        }

    if (IsIncludingDolchSightWords())
        {
        auto* sumWindow = dynamic_cast<wxWebView*>(
            view->GetDolchSightWordsView().FindWindowById(BaseProjectView::DOLCH_STATS_PAGE_ID));
        if (sumWindow == nullptr)
            {
            sumWindow = wxWebView::New(view->GetSplitter(), BaseProjectView::DOLCH_STATS_PAGE_ID);
            if (sumWindow != nullptr)
                {
                sumWindow->Hide();
                sumWindow->SetLabel(_(L"Summary"));
                sumWindow->SetName(_(L"Dolch Summary"));
                sumWindow->EnableContextMenu(false);
                sumWindow->Bind(wxEVT_WEBVIEW_NAVIGATING, &ProjectView::OnExplanationNavigating,
                                view);
                view->GetDolchSightWordsView().AddWindow(sumWindow);
                }
            }
        if (sumWindow != nullptr)
            {
            sumWindow->SetPage(NavLink::AnchorsToExplanationScheme(
                                   ProjectReportFormat::FormatHtmlReportStart(
                                       wxString::Format( // TRANSLATORS: %s is the project name
                                           _(L"Dolch Summary [%s]"), GetTitle()),
                                       wxGetApp().GetAppOptions()->GetReportTheme()) +
                                   ProjectReportFormat::FormatReportBanner(
                                       _(L"Dolch Sight Words Summary"), GetTitle()) +
                                   ProjectReportFormat::FormatDolchStatisticsInfo(
                                       this, GetStatisticsReportInfo(), true, nullptr) +
                                   ProjectReportFormat::FormatHtmlReportEnd()),
                               wxString{});
            }
        }
    else
        {
        view->GetDolchSightWordsView().RemoveWindowById(BaseProjectView::DOLCH_STATS_PAGE_ID);
        }
    }

//-------------------------------------------------------
bool ProjectDoc::AddGilliamPenaMountainFryTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::GPM_FRY();

    const wxString wordsColumnName{ _DT(L"WORDS") };
    const wxString syllablesColumnName{ _DT(L"SYLLABLES") };
    const wxString sentencesColumnName{ _DT(L"SENTENCES") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(wordsColumnName);
    scoreDataset->AddContinuousColumn(syllablesColumnName);
    scoreDataset->AddContinuousColumn(sentencesColumnName);

    scoreDataset->AddRow(Wisteria::Data::RowInfo().Continuous(
        { GetTotalWords(), GetTotalSyllablesNumeralsFullySyllabized(), GetTotalSentences() }));

    if (GetTotalWords() == 0)
        {
        LogMessage(wxString::Format(
                       _(L"Unable to calculate %s: at least one word must be present in document."),
                       GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
                   _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }
    if (GetTotalSentences() == 0)
        {
        LogMessage(
            wxString::Format(
                _(L"Unable to calculate %s: at least one sentence must be present in document."),
                GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
            _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }
    // GPM (Fry) graph
    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxGCDC gdc(view->GetDocFrame());

    auto* fryGraphView = dynamic_cast<Wisteria::Canvas*>(
        view->GetReadabilityResultsView().FindWindowById(BaseProjectView::GPM_FRY_PAGE_ID));
    if (fryGraphView != nullptr)
        {
        auto gFryGraph = std::dynamic_pointer_cast<Wisteria::Graphs::FryGraph>(
            fryGraphView->GetFixedObject(0, 0));
        wxASSERT_MSG(gFryGraph, L"GPM Fry graph dynamic cast failed in DisplayFryGPM()!");
        gFryGraph->SetData(scoreDataset, wordsColumnName, syllablesColumnName, sentencesColumnName);
        }
    else
        {
        fryGraphView = new Wisteria::Canvas(view->GetSplitter(), BaseProjectView::GPM_FRY_PAGE_ID);

        fryGraphView->SetFixedObjectsGridSize(1, 1);
        fryGraphView->Hide();
        fryGraphView->SetLabel(BaseProjectView::GetGilliamPenaMountainFryLabel());
        fryGraphView->SetName(BaseProjectView::GetGilliamPenaMountainFryLabel());
        fryGraphView->SetPrinterSettings(*wxGetApp().GetPrintData());

        auto gFryGraph = std::make_shared<Wisteria::Graphs::FryGraph>(
            fryGraphView, Wisteria::Graphs::FryGraph::FryGraphType::GPM,
            std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                Wisteria::Colors::Schemes::ColorScheme{ Wisteria::Colors::ColorBrewer::GetColor(
                    Wisteria::Colors::Color::CelestialBlue) }));
        gFryGraph->SetMessageCatalog(GetReadabilityMessageCatalogPtr());
        gFryGraph->SetData(scoreDataset, wordsColumnName, syllablesColumnName, sentencesColumnName);

        fryGraphView->SetFixedObject(0, 0, gFryGraph);

        view->GetReadabilityResultsView().AddWindow(fryGraphView);
        }
    UpdateGraphOptions(fryGraphView);

    auto gFryGraph =
        std::dynamic_pointer_cast<Wisteria::Graphs::FryGraph>(fryGraphView->GetFixedObject(0, 0));
    gFryGraph->SetInvalidAreaColor(GetInvalidAreaColor());
    gFryGraph->ShowcaseScore(IsShowcasingKeyItems());
    fryGraphView->CalcAllSizes(gdc);

    const auto theTest = GetReadabilityTests().find_test(currentTestKey);
    if (!VerifyTestBeforeAdding(theTest))
        {
        return false;
        }

    if (gFryGraph->GetScores().front().IsScoreInvalid())
        {
        const wxString description = _(L"Text is too difficult to be plotted.");

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(std::numeric_limits<double>::quiet_NaN(),
                           BaseProjectView::GetFailedLabel()),
            BaseProjectView::GetFailedLabel(), std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    else if (gFryGraph->GetScores().front().IsScoreOutOfGradeRange())
        {
        const wxString tooDifficultDescription =
            gFryGraph->GetScores().front().IsWordsHard() ?
                _(L"Text is too difficult to be classified to a specific "
                  "grade level because it contains too many high syllable words.") :
                _(L"Text is too difficult to be classified to a specific "
                  "grade level because it contains too many long sentences.");

        const wxString description = tooDifficultDescription;

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(std::numeric_limits<double>::quiet_NaN(),
                           BaseProjectView::GetFailedLabel()),
            BaseProjectView::GetFailedLabel(), std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    else
        {
        const wxString influenceDescription =
            gFryGraph->GetScores().front().IsWordsHard() ?
                _(L"3+ syllable words in the text primarily influenced this grade level score.") :
                _(L"The sentence lengths in the text primarily influenced this grade level score.");
        const size_t gradeValue = gFryGraph->GetScores().front().GetScore();

        const wxString description = ProjectReportFormat::FormatTestResult(
            GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue) + L"<br /><br />" +
                influenceDescription,
            theTest.first->get_test(),
            ((GetTotalNumerals() > 0) ? ReadabilityMessages::GetNumeralAreFullySyllabizedNote() :
                                        wxString{}));

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 17)
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

    GetReadabilityTests().include_test(currentTestKey, true);

    // if they asked to set focus to the score then select the graph
    if (setFocus)
        {
        view->UpdateSideBarIcons();
        view->GetSideBar()->SelectSubItem(
            view->GetSideBar()->FindSubItem(BaseProjectView::GPM_FRY_PAGE_ID));
        }

    return true;
    }

//-------------------------------------------------------
bool ProjectDoc::AddFryTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::FRY();

    const wxString wordsColumnName{ _DT(L"WORDS") };
    const wxString syllablesColumnName{ _DT(L"SYLLABLES") };
    const wxString sentencesColumnName{ _DT(L"SENTENCES") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(wordsColumnName);
    scoreDataset->AddContinuousColumn(syllablesColumnName);
    scoreDataset->AddContinuousColumn(sentencesColumnName);

    scoreDataset->AddRow(Wisteria::Data::RowInfo().Continuous(
        { GetTotalWords(), GetTotalSyllablesNumeralsFullySyllabized(), GetTotalSentences() }));

    if (GetTotalWords() == 0)
        {
        LogMessage(wxString::Format(
                       _(L"Unable to calculate %s: at least one word must be present in document."),
                       GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
                   _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }
    if (GetTotalSentences() == 0)
        {
        LogMessage(
            wxString::Format(
                _(L"Unable to calculate %s: at least one sentence must be present in document."),
                GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
            _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }
    // Fry graph
    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxGCDC gdc(view->GetDocFrame());

    auto* fryGraphView = dynamic_cast<Wisteria::Canvas*>(
        view->GetReadabilityResultsView().FindWindowById(BaseProjectView::FRY_PAGE_ID));
    if (fryGraphView != nullptr)
        {
        auto fryGraph = std::dynamic_pointer_cast<Wisteria::Graphs::FryGraph>(
            fryGraphView->GetFixedObject(0, 0));
        wxASSERT_MSG(fryGraph, L"Fry graph dynamic cast failed in DisplayFry()!");
        fryGraph->SetData(scoreDataset, wordsColumnName, syllablesColumnName, sentencesColumnName);
        }
    else
        {
        fryGraphView = new Wisteria::Canvas(view->GetSplitter(), BaseProjectView::FRY_PAGE_ID);

        fryGraphView->SetFixedObjectsGridSize(1, 1);

        fryGraphView->Hide();
        fryGraphView->SetLabel(BaseProjectView::GetFryLabel());
        fryGraphView->SetName(BaseProjectView::GetFryLabel());
        fryGraphView->SetPrinterSettings(*wxGetApp().GetPrintData());

        auto fryGraph = std::make_shared<Wisteria::Graphs::FryGraph>(
            fryGraphView, Wisteria::Graphs::FryGraph::FryGraphType::Traditional,
            std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                Wisteria::Colors::Schemes::ColorScheme{ Wisteria::Colors::ColorBrewer::GetColor(
                    Wisteria::Colors::Color::CelestialBlue) }));
        fryGraph->SetMessageCatalog(GetReadabilityMessageCatalogPtr());
        fryGraph->SetData(scoreDataset, wordsColumnName, syllablesColumnName, sentencesColumnName);

        fryGraphView->SetFixedObject(0, 0, fryGraph);
        view->GetReadabilityResultsView().AddWindow(fryGraphView);
        }
    UpdateGraphOptions(fryGraphView);

    auto fryGraph =
        std::dynamic_pointer_cast<Wisteria::Graphs::FryGraph>(fryGraphView->GetFixedObject(0, 0));

    fryGraph->SetInvalidAreaColor(GetInvalidAreaColor());
    fryGraph->ShowcaseScore(IsShowcasingKeyItems());
    fryGraphView->CalcAllSizes(gdc);

    const auto theTest = GetReadabilityTests().find_test(currentTestKey);
    if (!VerifyTestBeforeAdding(theTest))
        {
        return false;
        }

    if (fryGraph->GetScores().front().IsScoreInvalid())
        {
        const wxString description = _(L"Text is too difficult to be plotted.");

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(std::numeric_limits<double>::quiet_NaN(),
                           BaseProjectView::GetFailedLabel()),
            BaseProjectView::GetFailedLabel(), std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    else if (fryGraph->GetScores().front().IsScoreOutOfGradeRange())
        {
        const wxString tooDifficultDescription =
            fryGraph->GetScores().front().IsWordsHard() ?
                _(L"Text is too difficult to be classified to a specific "
                  "grade level because it contains too many high syllable words.") :
                _(L"Text is too difficult to be classified to a specific "
                  "grade level because it contains too many long sentences.");

        const wxString description = tooDifficultDescription;

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(std::numeric_limits<double>::quiet_NaN(),
                           BaseProjectView::GetFailedLabel()),
            BaseProjectView::GetFailedLabel(), std::numeric_limits<double>::quiet_NaN(),
            std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    else
        {
        const wxString influenceDescription =
            fryGraph->GetScores().front().IsWordsHard() ?
                _(L"3+ syllable words in the text primarily influenced this grade level score.") :
                _(L"The sentence lengths in the text primarily influenced this grade level score.");
        const size_t gradeValue = fryGraph->GetScores().front().GetScore();

        const wxString description = ProjectReportFormat::FormatTestResult(
            GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue) + L"<br />" +
                influenceDescription,
            theTest.first->get_test(),
            ((GetTotalNumerals() > 0) ? ReadabilityMessages::GetNumeralAreFullySyllabizedNote() :
                                        wxString{}));

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 17)
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

    GetReadabilityTests().include_test(currentTestKey, true);

    // if they asked to set focus to the score then select the graph
    if (setFocus)
        {
        view->UpdateSideBarIcons();
        view->GetSideBar()->SelectSubItem(
            view->GetSideBar()->FindSubItem(BaseProjectView::FRY_PAGE_ID));
        }

    return true;
    }

//-------------------------------------------------------
bool ProjectDoc::AddRaygorTest(const bool setFocus)
    {
    ClearReadabilityTestResult();

    const wxString currentTestKey = ReadabilityMessages::RAYGOR();

    const wxString wordsColumnName{ _DT(L"WORDS") };
    const wxString sixCharWordsColumnName{ _DT(L"6CHARWORDS") };
    const wxString sentencesColumnName{ _DT(L"SENTENCES") };

    auto scoreDataset = std::make_shared<Wisteria::Data::Dataset>();
    scoreDataset->AddContinuousColumn(wordsColumnName);
    scoreDataset->AddContinuousColumn(sixCharWordsColumnName);
    scoreDataset->AddContinuousColumn(sentencesColumnName);

    scoreDataset->AddRow(Wisteria::Data::RowInfo().Continuous(
        { GetTotalWords() - GetTotalNumerals(), GetTotalSixPlusCharacterWordsIgnoringNumerals(),
          GetTotalSentences() }));

    if (GetTotalWords() == 0.0)
        {
        LogMessage(wxString::Format(
                       _(L"Unable to calculate %s: at least one word must be present in document."),
                       GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
                   _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }
    if (GetTotalSentences() == 0.0)
        {
        LogMessage(
            wxString::Format(
                _(L"Unable to calculate %s: at least one sentence must be present in document."),
                GetReadabilityTests().get_test_short_name(currentTestKey).c_str()),
            _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        GetReadabilityTests().include_test(currentTestKey, false);
        return false;
        }
    // Raygor graph
    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxGCDC gdc(view->GetDocFrame());

    auto* raygorGraphView = dynamic_cast<Wisteria::Canvas*>(
        view->GetReadabilityResultsView().FindWindowById(BaseProjectView::RAYGOR_PAGE_ID));
    if (raygorGraphView != nullptr)
        {
        auto raygorGraph = std::dynamic_pointer_cast<Wisteria::Graphs::RaygorGraph>(
            raygorGraphView->GetFixedObject(0, 0));
        wxASSERT_MSG(raygorGraph, L"Raygor graph dynamic cast failed in DisplayRaygor()!");
        raygorGraph->SetData(scoreDataset, wordsColumnName, sixCharWordsColumnName,
                             sentencesColumnName);
        }
    else
        {
        raygorGraphView =
            new Wisteria::Canvas(view->GetSplitter(), BaseProjectView::RAYGOR_PAGE_ID);

        raygorGraphView->SetFixedObjectsGridSize(1, 1);

        raygorGraphView->Hide();
        raygorGraphView->SetLabel(BaseProjectView::GetRaygorLabel());
        raygorGraphView->SetName(BaseProjectView::GetRaygorLabel());
        raygorGraphView->SetPrinterSettings(*wxGetApp().GetPrintData());

        auto raygorGraph = std::make_shared<Wisteria::Graphs::RaygorGraph>(
            raygorGraphView,
            std::make_shared<Wisteria::Colors::Schemes::ColorScheme>(
                Wisteria::Colors::Schemes::ColorScheme{ Wisteria::Colors::ColorBrewer::GetColor(
                    Wisteria::Colors::Color::CelestialBlue) }));
        raygorGraph->SetMessageCatalog(GetReadabilityMessageCatalogPtr());
        raygorGraph->SetData(scoreDataset, wordsColumnName, sixCharWordsColumnName,
                             sentencesColumnName);

        raygorGraphView->SetFixedObject(0, 0, raygorGraph);

        view->GetReadabilityResultsView().AddWindow(raygorGraphView);
        }
    UpdateGraphOptions(raygorGraphView);

    auto raygorGraph = std::dynamic_pointer_cast<Wisteria::Graphs::RaygorGraph>(
        raygorGraphView->GetFixedObject(0, 0));
    raygorGraph->SetInvalidAreaColor(GetInvalidAreaColor());
    raygorGraph->SetRaygorStyle(GetRaygorStyle());
    raygorGraphView->CalcAllSizes(gdc);

    const auto theTest = GetReadabilityTests().find_test(currentTestKey);
    if (!VerifyTestBeforeAdding(theTest))
        {
        return false;
        }

    if (raygorGraph->GetScores().at(0).IsScoreInvalid())
        {
        SetReadabilityTestResult(currentTestKey, theTest.first->get_test().get_long_name().c_str(),
                                 _(L"Text is too difficult to be plotted."),
                                 std::make_pair(std::numeric_limits<double>::quiet_NaN(),
                                                BaseProjectView::GetFailedLabel()),
                                 BaseProjectView::GetFailedLabel(),
                                 std::numeric_limits<double>::quiet_NaN(),
                                 std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    else if (raygorGraph->GetScores().at(0).IsScoreOutOfGradeRange())
        {
        const wxString tooDifficultDescription =
            raygorGraph->GetScores().at(0).IsWordsHard() ?
                _(L"Text is too difficult to be classified to a specific "
                  "grade level because it contains too many 6+ character words.") :
                _(L"Text is too difficult to be classified to a specific "
                  "grade level because it contains too many long sentences.");

        SetReadabilityTestResult(currentTestKey, theTest.first->get_test().get_long_name().c_str(),
                                 tooDifficultDescription,
                                 std::make_pair(std::numeric_limits<double>::quiet_NaN(),
                                                BaseProjectView::GetFailedLabel()),
                                 BaseProjectView::GetFailedLabel(),
                                 std::numeric_limits<double>::quiet_NaN(),
                                 std::numeric_limits<double>::quiet_NaN(), setFocus);
        }
    else
        {
        const wxString influenceDescription =
            raygorGraph->GetScores().at(0).IsWordsHard() ?
                _(L"6+ character words in the text primarily influenced this grade level score.") :
                _(L"The sentence lengths in the text primarily influenced this grade level score.");
        const size_t gradeValue = raygorGraph->GetScores().at(0).GetScore();

        wxString displayableGradeLevel = wxNumberFormatter::ToString(
            gradeValue, 0, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        if (gradeValue == 17)
            {
            displayableGradeLevel += L"+";
            }
        const wxString description = ProjectReportFormat::FormatTestResult(
            GetReadabilityMessageCatalog().GetGradeScaleDescription(gradeValue) + L"<br /><br />" +
                influenceDescription,
            theTest.first->get_test(), ReadabilityMessages::GetPunctuationIgnoredNote());

        SetReadabilityTestResult(
            currentTestKey, theTest.first->get_test().get_long_name().c_str(), description,
            std::make_pair(gradeValue, displayableGradeLevel),
            ReadabilityMessages::GetAgeFromUSGrade(
                gradeValue, GetReadabilityMessageCatalog().GetReadingAgeDisplay()),
            std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(),
            setFocus);
        }

    GetReadabilityTests().include_test(currentTestKey, true);

    // if they asked to set focus to the score then select the graph
    if (setFocus)
        {
        view->UpdateSideBarIcons();
        view->GetSideBar()->SelectSubItem(
            view->GetSideBar()->FindSubItem(BaseProjectView::RAYGOR_PAGE_ID));
        }

    return true;
    }

bool ProjectDoc::AddDolchSightWords()
    {
    if (GetTotalWords() == 0.0)
        {
        LogMessage(
            _(L"Unable to calculate Dolch words: at least one word must be present in document."),
            _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        IncludeDolchSightWords(false);
        return false;
        }
    if (GetTotalSentences() == 0.0)
        {
        LogMessage(_(L"Unable to calculate Dolch words: "
                     "at least one valid sentence must be present in document."),
                   _(L"Error"), wxOK | wxICON_ERROR, wxString{}, true);
        IncludeDolchSightWords(false);
        return false;
        }

    IncludeDolchSightWords(true);
    RefreshRequired(ProjectRefresh::FullReindexing);
    RefreshProject();

    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    view->GetSideBar()->SelectFolder(
        view->GetSideBar()->FindFolder(BaseProjectView::SIDEBAR_DOLCH_SECTION_ID));

    return true;
    }

//-------------------------------------------------------
void ProjectDoc::SetReadabilityTestResult(const wxString& testId, const wxString& testName,
                                          const wxString& description,
                                          const std::pair<double, wxString>& USGradeLevel,
                                          const wxString& readerAge, const double indexScore,
                                          const double clozeScore, const bool setFocus)
    {
    BaseProject::SetReadabilityTestResult(testId, testName, description, USGradeLevel, readerAge,
                                          indexScore, clozeScore, setFocus);

    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxASSERT_MSG(view, L"Invalid view when adding test!");
    if (view == nullptr)
        {
        return;
        }
    // format the explanation window
    const wxString explanationString =
        wxString::Format(L"<div class='explanation-card'>"
                         "<div class='explanation-card-header'>%s</div>"
                         "<div class='explanation-card-body'>%s</div>"
                         "</div>",
                         testName, description);

    const wxWindowUpdateLocker noUpdates(view->GetReadabilityScoresList());
    long location = view->GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(testName);
    if (location == wxNOT_FOUND)
        {
        location = view->GetReadabilityScoresList()->GetResultsListCtrl()->AddRow(testName);
        }
    view->GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(
        location, 1, USGradeLevel.second,
        Wisteria::NumberFormatInfo(Wisteria::NumberFormatInfo::NumberFormatType::CustomFormatting,
                                   1));
    view->GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(location, 2, readerAge);
    if (std::isnan(indexScore))
        {
        view->GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(location, 3,
                                                                            wxString{});
        }
    else
        {
        view->GetReadabilityScoresList()->GetDataProvider()->SetItemValue(
            location, 3, round_decimal_place(indexScore, 10),
            Wisteria::NumberFormatInfo(
                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 1));
        }
    if (std::isnan(clozeScore))
        {
        view->GetReadabilityScoresList()->GetResultsListCtrl()->SetItemText(location, 4,
                                                                            wxString{});
        }
    else
        {
        view->GetReadabilityScoresList()->GetDataProvider()->SetItemValue(
            location, 4, round_to_integer(clozeScore));
        }
    view->GetReadabilityScoresList()->GetExplanations()[testName] = explanationString;

    // select item and select scores window in the main project view
    location = view->GetReadabilityScoresList()->GetResultsListCtrl()->FindEx(testName);
    if (location != wxNOT_FOUND)
        {
        view->GetReadabilityScoresList()->GetResultsListCtrl()->Select(location);
        // don't call Focus() because that will call EnsureVisible and cause interface problems,
        // just set the state
        view->GetReadabilityScoresList()->GetResultsListCtrl()->SetItemState(
            location, wxLIST_STATE_FOCUSED, wxLIST_STATE_FOCUSED);
        }
    view->GetReadabilityScoresList()->GetResultsListCtrl()->SetColumnWidth(
        0, view->GetReadabilityScoresList()->GetResultsListCtrl()->EstimateColumnWidth(0));
    view->GetReadabilityScoresList()->GetResultsListCtrl()->SetColumnWidth(
        1, wxLIST_AUTOSIZE_USEHEADER);
    view->GetReadabilityScoresList()->GetResultsListCtrl()->SetColumnWidth(
        2, wxLIST_AUTOSIZE_USEHEADER);
    view->GetReadabilityScoresList()->GetResultsListCtrl()->SetColumnWidth(
        3, wxLIST_AUTOSIZE_USEHEADER);
    view->GetReadabilityScoresList()->GetResultsListCtrl()->SetSize(
        view->GetReadabilityScoresList()->GetResultsListCtrl()->GetSize());

    if (setFocus)
        {
        // focus the project section
        view->GetSideBar()->SelectFolder(
            view->GetSideBar()->FindFolder(BaseProjectView::SIDEBAR_READABILITY_SCORES_SECTION_ID));
        }
    }

//-------------------------------------------------------
void ProjectDoc::UpdateHighlightedTextWindows()
    {
    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    // DC
    if (GetWordsBreakdownInfo().IsDCUnfamiliarEnabled() && IsDaleChallLikeTestIncluded())
        {
        const auto buddyWindowPosition = view->GetWordsBreakdownView().FindWindowPositionById(
            BaseProjectView::DC_WORDS_LIST_PAGE_ID);
        view->GetWordsBreakdownView().InsertWindow(
            (buddyWindowPosition != wxNOT_FOUND) ? buddyWindowPosition + 1 : 0, m_dcTextWindow);
        }
    else
        {
        view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::DC_WORDS_TEXT_PAGE_ID);
        }
    // Spache
    if (GetWordsBreakdownInfo().IsSpacheUnfamiliarEnabled() &&
        GetReadabilityTests().is_test_included(ReadabilityMessages::SPACHE()))
        {
        const auto buddyWindowPosition = view->GetWordsBreakdownView().FindWindowPositionById(
            BaseProjectView::SPACHE_WORDS_LIST_PAGE_ID);
        view->GetWordsBreakdownView().InsertWindow(
            (buddyWindowPosition != wxNOT_FOUND) ? buddyWindowPosition + 1 : 0, m_spacheTextWindow);
        }
    else
        {
        view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::SPACHE_WORDS_TEXT_PAGE_ID);
        }
    // HJ
    if (GetWordsBreakdownInfo().IsHarrisJacobsonUnfamiliarEnabled() &&
        GetReadabilityTests().is_test_included(ReadabilityMessages::HARRIS_JACOBSON()))
        {
        const auto buddyWindowPosition = view->GetWordsBreakdownView().FindWindowPositionById(
            BaseProjectView::HARRIS_JACOBSON_WORDS_LIST_PAGE_ID);
        view->GetWordsBreakdownView().InsertWindow(
            (buddyWindowPosition != wxNOT_FOUND) ? buddyWindowPosition + 1 : 0, m_hjTextWindow);
        }
    else
        {
        view->GetWordsBreakdownView().RemoveWindowById(
            BaseProjectView::HARRIS_JACOBSON_WORDS_TEXT_PAGE_ID);
        }
    }

//-------------------------------------------------------
std::pair<wxString, wxString> ProjectDoc::FormatRtfHeaderFont(const wxFont& textViewFont,
                                                              const size_t mainFontColorIndex)
    {
    wxString mainFontHeader =
        wxString::Format(L"\\viewkind4\\uc1\\pard\\cf%zu", mainFontColorIndex);
    wxString endSection = L"\\par\\cf0";
    endSection += L"\\cf0";
    if (textViewFont.GetWeight() == wxFONTWEIGHT_BOLD)
        {
        endSection += L"\\b0";
        mainFontHeader += L"\\b";
        }
    if (textViewFont.GetStyle() == wxFONTSTYLE_ITALIC)
        {
        endSection += L"\\i0";
        mainFontHeader += L"\\i";
        }
    if (textViewFont.GetUnderlined())
        {
        endSection += L"\\ulnone";
        mainFontHeader += _DT(L"\\ul");
        }
    // "fs" command is in half points
    mainFontHeader += wxString::Format(L"\\f0\\fs%d ", textViewFont.GetPointSize() * 2);

    endSection += L"\\par}";

    return std::make_pair(mainFontHeader, endSection);
    }

//-------------------------------------------------------
ProjectDoc::HighlighterColors ProjectDoc::BuildReportColors(const wxColour& highlightColor,
                                                            const wxColour& backgroundColor) const
    {
    HighlighterColors highlighterColors;

    const Wisteria::Colors::ColorContrast colorContrast(backgroundColor);
    highlighterColors.highlightColor =
        (GetTextHighlightMethod() == TextHighlight::HighlightForeground) ?
            colorContrast.Contrast(highlightColor) :
            highlightColor;

    highlighterColors.errorHighlightColor =
        (GetTextHighlightMethod() == TextHighlight::HighlightForeground) ?
            colorContrast.Contrast(GetDuplicateWordHighlightColor()) :
            GetDuplicateWordHighlightColor();
    highlighterColors.styleHighlightColor =
        (GetTextHighlightMethod() == TextHighlight::HighlightForeground) ?
            colorContrast.Contrast(GetWordyPhraseHighlightColor()) :
            GetWordyPhraseHighlightColor();
    highlighterColors.excludedTextHighlightColor =
        (GetTextHighlightMethod() == TextHighlight::HighlightForeground) ?
            colorContrast.Contrast(GetExcludedTextHighlightColor()) :
            GetExcludedTextHighlightColor();
    highlighterColors.dolchConjunctionsTextHighlightColor =
        (GetTextHighlightMethod() == TextHighlight::HighlightForeground) ?
            colorContrast.Contrast(GetDolchConjunctionsColor()) :
            GetDolchConjunctionsColor();
    highlighterColors.dolchPrepositionsTextHighlightColor =
        (GetTextHighlightMethod() == TextHighlight::HighlightForeground) ?
            colorContrast.Contrast(GetDolchPrepositionsColor()) :
            GetDolchPrepositionsColor();
    highlighterColors.dolchPronounsTextHighlightColor =
        (GetTextHighlightMethod() == TextHighlight::HighlightForeground) ?
            colorContrast.Contrast(GetDolchPronounsColor()) :
            GetDolchPronounsColor();
    highlighterColors.dolchAdverbsTextHighlightColor =
        (GetTextHighlightMethod() == TextHighlight::HighlightForeground) ?
            colorContrast.Contrast(GetDolchAdverbsColor()) :
            GetDolchAdverbsColor();
    highlighterColors.dolchAdjectivesTextHighlightColor =
        (GetTextHighlightMethod() == TextHighlight::HighlightForeground) ?
            colorContrast.Contrast(GetDolchAdjectivesColor()) :
            GetDolchAdjectivesColor();
    highlighterColors.dolchVerbsTextHighlightColor =
        (GetTextHighlightMethod() == TextHighlight::HighlightForeground) ?
            colorContrast.Contrast(GetDolchVerbsColor()) :
            GetDolchVerbsColor();
    highlighterColors.dolchNounTextHighlightColor =
        (GetTextHighlightMethod() == TextHighlight::HighlightForeground) ?
            colorContrast.Contrast(GetDolchNounColor()) :
            GetDolchNounColor();

    return highlighterColors;
    }

//-------------------------------------------------------
std::tuple<wxString, wxString, wxString>
ProjectDoc::BuildColorTable(const wxFont& textViewFont, const HighlighterColors& highlighterColors,
                            const wxColour& backgroundColor)
    {
    // clang-format off
    // black is at position 1,
    // white is added to the end (position 13)
    wxString colorTable = wxString::Format(
        L"{\\colortbl; "
        "\\red0\\green0\\blue0;"
        "\\red%u\\green%u\\blue%u;\\red%u\\green%u\\blue%u;\\red%u\\green%u\\blue%u;\\red%u\\green%u\\blue%u;"
        "\\red%u\\green%u\\blue%u;\\red%u\\green%u\\blue%u;\\red%u\\green%u\\blue%u;\\red%u\\green%u\\blue%u;",
        highlighterColors.highlightColor.Red(),
            highlighterColors.highlightColor.Green(),
            highlighterColors.highlightColor.Blue(),
        highlighterColors.errorHighlightColor.Red(),
            highlighterColors.errorHighlightColor.Green(),
            highlighterColors.errorHighlightColor.Blue(),
        highlighterColors.styleHighlightColor.Red(),
            highlighterColors.styleHighlightColor.Green(),
        highlighterColors.styleHighlightColor.Blue(),
        highlighterColors.excludedTextHighlightColor.Red(),
        highlighterColors.excludedTextHighlightColor.Green(),
        highlighterColors.excludedTextHighlightColor.Blue(),
        highlighterColors.dolchConjunctionsTextHighlightColor.Red(),
        highlighterColors.dolchConjunctionsTextHighlightColor.Green(),
        highlighterColors.dolchConjunctionsTextHighlightColor.Blue(),
        highlighterColors.dolchPrepositionsTextHighlightColor.Red(),
        highlighterColors.dolchPrepositionsTextHighlightColor.Green(),
        highlighterColors.dolchPrepositionsTextHighlightColor.Blue(),
        highlighterColors.dolchPronounsTextHighlightColor.Red(),
        highlighterColors.dolchPronounsTextHighlightColor.Green(),
        highlighterColors.dolchPronounsTextHighlightColor.Blue(),
        highlighterColors.dolchAdverbsTextHighlightColor.Red(),
        highlighterColors.dolchAdverbsTextHighlightColor.Green(),
        highlighterColors.dolchAdverbsTextHighlightColor.Blue());
    colorTable += wxString::Format(
        L"\\red%u\\green%u\\blue%u;\\red%u"
                                   "\\green%u\\blue%u;\\red%u\\green%u\\blue%u;"
                                   "\\red255\\green255\\blue255;}",
                                   highlighterColors.dolchAdjectivesTextHighlightColor.Red(),
                                   highlighterColors.dolchAdjectivesTextHighlightColor.Green(),
                                   highlighterColors.dolchAdjectivesTextHighlightColor.Blue(),
                                   highlighterColors.dolchVerbsTextHighlightColor.Red(),
                                   highlighterColors.dolchVerbsTextHighlightColor.Green(),
                                   highlighterColors.dolchVerbsTextHighlightColor.Blue(),
                                   highlighterColors.dolchNounTextHighlightColor.Red(),
                                   highlighterColors.dolchNounTextHighlightColor.Green(),
                                   highlighterColors.dolchNounTextHighlightColor.Blue());
    // clang-format on
    const auto [mainFontHeader, ending] =
        FormatRtfHeaderFont(textViewFont, ((backgroundColor.GetLuminance() < .5f) ? 13 : 1));

    return std::make_tuple(colorTable, mainFontHeader, ending);
    }

//-------------------------------------------------------
wxString ProjectDoc::BuildStyleSheet() const
    {
    const bool isBackgroundMode = (GetTextHighlightMethod() == TextHighlight::HighlightBackground);

    // emits the swatch and word-highlight rules for one category: background mode uses the
    // fixed user color, foreground mode a light-dark() shade/tint so text stays legible
    const auto buildClass = [isBackgroundMode](const wxString& suffix, const wxColour& color,
                                               const bool boldForeground,
                                               const bool strikethrough) -> wxString
    {
        if (isBackgroundMode)
            {
            const wxString hex = color.GetAsString(wxC2S_HTML_SYNTAX);
            return wxString::Format(
                L"\n.hl-swatch-%s { background-color: %s; }"
                L"\n.hl-%s { background-color: %s; color: %s;%s }",
                suffix, hex, suffix, hex,
                Wisteria::Colors::ColorContrast::BlackOrWhiteContrast(color).GetAsString(
                    wxC2S_HTML_SYNTAX),
                strikethrough ? L" text-decoration: line-through;" : L"");
            }
        const wxString themedColor = wxString::Format(
            L"light-dark(%s, %s)",
            Wisteria::Colors::ColorContrast::Shade(color, 0.4).GetAsString(wxC2S_HTML_SYNTAX),
            Wisteria::Colors::ColorContrast::Tint(color, 0.6).GetAsString(wxC2S_HTML_SYNTAX));
        return wxString::Format(L"\n.hl-swatch-%s { background-color: %s; }"
                                L"\n.hl-%s { color: %s;%s%s }",
                                suffix, themedColor, suffix, themedColor,
                                boldForeground ? L" font-weight: bold;" : L"",
                                strikethrough ? L" text-decoration: line-through;" : L"");
    };

    wxString css = LR"(<style>
.hl-swatch { display: inline-block; width: 1.4em; height: 0.9em; margin-right: 0.3em;
    border: 1px solid var(--border-color); border-radius: 3px; vertical-align: middle; }
.legend-card { font-size: 0.9em; width: fit-content; max-width: 100%;
    border: 1px solid var(--border-color);
    transition: border-color 0.4s ease, box-shadow 0.4s ease; }
.legend-card:hover { border-color: color-mix(in srgb, var(--banner-color) 50%, CanvasText);
    box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1); }
.legend-card .report-banner-content { line-height: 1.5; }
)";

    css += buildClass(_DT(L"default"), GetTextHighlightColor(), true, false);
    css += buildClass(_DT(L"error"), GetDuplicateWordHighlightColor(), false, false);
    css += buildClass(_DT(L"phrase"), GetWordyPhraseHighlightColor(), false, false);
    css += buildClass(_DT(L"excluded"), GetExcludedTextHighlightColor(), false, true);
    css += buildClass(_DT(L"dolch-conjunction"), GetDolchConjunctionsColor(), true, false);
    css += buildClass(_DT(L"dolch-preposition"), GetDolchPrepositionsColor(), true, false);
    css += buildClass(_DT(L"dolch-pronoun"), GetDolchPronounsColor(), true, false);
    css += buildClass(_DT(L"dolch-adverb"), GetDolchAdverbsColor(), true, false);
    css += buildClass(_DT(L"dolch-adjective"), GetDolchAdjectivesColor(), true, false);
    css += buildClass(_DT(L"dolch-verb"), GetDolchVerbsColor(), true, false);
    css += buildClass(_DT(L"dolch-noun"), GetDolchNounColor(), true, false);

    css += L"\n</style>\n";
    return css;
    }

//-------------------------------------------------------
ProjectDoc::HighlighterTags ProjectDoc::BuildHighlighterTags(const MarkupFormat format,
                                                             const wxColour& highlightColor) const
    {
    HighlighterTags highlighterTags;
    highlighterTags.format = format;

    if (IsRtf(format))
        {
        // When highlighting backgrounds, set the text colors to contrast against it.
        // Note that even though the text window might be dark (with white font), the highlighting
        // might be light and in that case we will want a dark font for that word.
        // (13 and white, 1 is black [refer to color table below].)
        const int highlightedTextColorIndexBGMode = (highlightColor.GetLuminance() < .5f) ? 13 : 1;
        const int errorTextColorIndexBGMode =
            (GetDuplicateWordHighlightColor().GetLuminance() < .5f) ? 13 : 1;
        const int styleTextColorIndexBGMode =
            (GetWordyPhraseHighlightColor().GetLuminance() < .5f) ? 13 : 1;
        const int excludedTextColorIndexBGMode =
            (GetExcludedTextHighlightColor().GetLuminance() < .5f) ? 13 : 1;
        const int dolchConjunctionsTextColorIndexBGMode =
            (GetDolchConjunctionsColor().GetLuminance() < .5f) ? 13 : 1;
        const int dolchPrepositionsTextColorIndexBGMode =
            (GetDolchPrepositionsColor().GetLuminance() < .5f) ? 13 : 1;
        const int dolchPronounsTextColorIndexBGMode =
            (GetDolchPronounsColor().GetLuminance() < .5f) ? 13 : 1;
        const int dolchAdverbsTextColorIndexBGMode =
            (GetDolchAdverbsColor().GetLuminance() < .5f) ? 13 : 1;
        const int dolchAdjectivesTextColorIndexBGMode =
            (GetDolchAdjectivesColor().GetLuminance() < .5f) ? 13 : 1;
        const int dolchVerbsTextColorIndexBGMode =
            (GetDolchVerbsColor().GetLuminance() < .5f) ? 13 : 1;
        const int dolchNounTextColorIndexBGMode =
            (GetDolchNounColor().GetLuminance() < .5f) ? 13 : 1;

        // The only RTF-dialect difference between Windows and macOS is the
        // background-highlight control word. Windows uses \highlightN, macOS uses
        // \cbN. The foreground color (\cfN), terminators, and legend structure are
        // identical, so parameterize just that word
        const wxString bgHl = (format == MarkupFormat::RtfMacOS) ? L"cb" : L"highlight";
        const bool isBackgroundMode =
            (GetTextHighlightMethod() == TextHighlight::HighlightBackground);

        highlighterTags.HIGHLIGHT_BEGIN =
            isBackgroundMode ?
                wxString::Format(L"{\\%s2\\cf%d ", bgHl, highlightedTextColorIndexBGMode) :
                wxString{ L"{\\cf2 " };
        highlighterTags.ERROR_HIGHLIGHT_BEGIN =
            isBackgroundMode ? wxString::Format(L"{\\%s3\\cf%d ", bgHl, errorTextColorIndexBGMode) :
                               wxString{ L"{\\cf3 " };
        highlighterTags.PHRASE_HIGHLIGHT_BEGIN =
            isBackgroundMode ? wxString::Format(L"{\\%s4\\cf%d ", bgHl, styleTextColorIndexBGMode) :
                               wxString{ L"{\\cf4 " };
        highlighterTags.IGNORE_HIGHLIGHT_BEGIN =
            isBackgroundMode ?
                wxString::Format(L"{\\%s5\\cf%d ", bgHl, excludedTextColorIndexBGMode) :
                wxString{ L"{\\cf5 " };
        // Dolch highlighting
        highlighterTags.DOLCH_CONJUNCTION_BEGIN =
            IsHighlightingDolchConjunctions() ?
                isBackgroundMode ?
                wxString::Format(L"{\\%s6\\cf%d ", bgHl, dolchConjunctionsTextColorIndexBGMode) :
                wxString{ L"{\\cf6 " } :
                wxString{};
        highlighterTags.DOLCH_PREPOSITIONS_BEGIN =
            IsHighlightingDolchPrepositions() ?
                isBackgroundMode ?
                wxString::Format(L"{\\%s7\\cf%d ", bgHl, dolchPrepositionsTextColorIndexBGMode) :
                wxString{ L"{\\cf7 " } :
                wxString{};
        highlighterTags.DOLCH_PRONOUN_BEGIN =
            IsHighlightingDolchPronouns() ?
                isBackgroundMode ?
                wxString::Format(L"{\\%s8\\cf%d ", bgHl, dolchPronounsTextColorIndexBGMode) :
                wxString{ L"{\\cf8 " } :
                wxString{};
        highlighterTags.DOLCH_ADVERB_BEGIN =
            IsHighlightingDolchAdverbs() ?
                isBackgroundMode ?
                wxString::Format(L"{\\%s9\\cf%d ", bgHl, dolchAdverbsTextColorIndexBGMode) :
                wxString{ L"{\\cf9 " } :
                wxString{};
        highlighterTags.DOLCH_ADJECTIVE_BEGIN =
            IsHighlightingDolchAdjectives() ?
                isBackgroundMode ?
                wxString::Format(L"{\\%s10\\cf%d ", bgHl, dolchAdjectivesTextColorIndexBGMode) :
                wxString{ L"{\\cf10 " } :
                wxString{};
        highlighterTags.DOLCH_VERB_BEGIN =
            IsHighlightingDolchVerbs() ?
                isBackgroundMode ?
                wxString::Format(L"{\\%s11\\cf%d ", bgHl, dolchVerbsTextColorIndexBGMode) :
                wxString{ L"{\\cf11 " } :
                wxString{};
        highlighterTags.DOLCH_NOUN_BEGIN =
            IsHighlightingDolchNouns() ?
                isBackgroundMode ?
                wxString::Format(L"{\\%s12\\cf%d ", bgHl, dolchNounTextColorIndexBGMode) :
                wxString{ L"{\\cf12 " } :
                wxString{};
        // terminator
        highlighterTags.HIGHLIGHT_END = L"}";

        // these are used for the legend lines because they are always set
        // to use background highlighting
        highlighterTags.HIGHLIGHT_BEGIN_LEGEND = wxString::Format(L"{\\%s2 ", bgHl);
        highlighterTags.HIGHLIGHT_END_LEGEND = L"}";
        highlighterTags.DUPLICATE_HIGHLIGHT_BEGIN_LEGEND = wxString::Format(L"{\\%s3 ", bgHl);
        highlighterTags.PHRASE_HIGHLIGHT_BEGIN_LEGEND = wxString::Format(L"{\\%s4 ", bgHl);
        highlighterTags.IGNORE_HIGHLIGHT_BEGIN_LEGEND = wxString::Format(L"{\\%s5 ", bgHl);
        // dolch highlighting
        highlighterTags.DOLCH_CONJUNCTION_BEGIN_LEGEND = IsHighlightingDolchConjunctions() ?
                                                             wxString::Format(L"{\\%s6 ", bgHl) :
                                                             wxString{ L"{" };
        highlighterTags.DOLCH_PREPOSITIONS_BEGIN_LEGEND = IsHighlightingDolchPrepositions() ?
                                                              wxString::Format(L"{\\%s7 ", bgHl) :
                                                              wxString{ L"{" };
        highlighterTags.DOLCH_PRONOUN_BEGIN_LEGEND =
            IsHighlightingDolchPronouns() ? wxString::Format(L"{\\%s8 ", bgHl) : wxString{ L"{" };
        highlighterTags.DOLCH_ADVERB_BEGIN_LEGEND =
            IsHighlightingDolchAdverbs() ? wxString::Format(L"{\\%s9 ", bgHl) : wxString{ L"{" };
        highlighterTags.DOLCH_ADJECTIVE_BEGIN_LEGEND = IsHighlightingDolchAdjectives() ?
                                                           wxString::Format(L"{\\%s10 ", bgHl) :
                                                           wxString{ L"{" };
        highlighterTags.DOLCH_VERB_BEGIN_LEGEND =
            IsHighlightingDolchVerbs() ? wxString::Format(L"{\\%s11 ", bgHl) : wxString{ L"{" };
        highlighterTags.DOLCH_NOUN_BEGIN_LEGEND =
            IsHighlightingDolchNouns() ? wxString::Format(L"{\\%s12 ", bgHl) : wxString{ L"{" };

        highlighterTags.BOLD_BEGIN = L"{\\b";
        highlighterTags.BOLD_END = L"}";
        highlighterTags.TAB_SYMBOL = L"    ";
        highlighterTags.CRLF = L"\\par\n";
        }
    else // Html
        {
        // The .hl-* classes carry the color, the foreground/background choice, and
        // the light/dark-mode handling (see BuildStyleSheet). The tags themselves are
        // just stable class hooks. Unused Dolch categories emit no span, matching the
        // "highlighting disabled" case.
        highlighterTags.HIGHLIGHT_END = L"</span>";
        highlighterTags.HIGHLIGHT_END_LEGEND = L"</span>";
        highlighterTags.BOLD_BEGIN = L"<b>";
        highlighterTags.BOLD_END = L"</b>";
        // HTML collapses runs of whitespace, so emit an explicit break for each
        // newline and non-breaking spaces for the paragraph indent
        highlighterTags.TAB_SYMBOL = L"&nbsp;&nbsp;&nbsp;&nbsp;";
        highlighterTags.CRLF = L"<br />\n";

        // word highlighters
        highlighterTags.HIGHLIGHT_BEGIN = wxString{ LR"(<span class="hl-default">)" };
        highlighterTags.ERROR_HIGHLIGHT_BEGIN = wxString{ LR"(<span class="hl-error">)" };
        highlighterTags.PHRASE_HIGHLIGHT_BEGIN = wxString{ LR"(<span class="hl-phrase">)" };
        highlighterTags.IGNORE_HIGHLIGHT_BEGIN = wxString{ LR"(<span class="hl-excluded">)" };
        highlighterTags.DOLCH_CONJUNCTION_BEGIN =
            IsHighlightingDolchConjunctions() ?
                wxString{ LR"(<span class="hl-dolch-conjunction">)" } :
                wxString{};
        highlighterTags.DOLCH_PREPOSITIONS_BEGIN =
            IsHighlightingDolchPrepositions() ?
                wxString{ LR"(<span class="hl-dolch-preposition">)" } :
                wxString{};
        highlighterTags.DOLCH_PRONOUN_BEGIN =
            IsHighlightingDolchPronouns() ? wxString{ LR"(<span class="hl-dolch-pronoun">)" } :
                                            wxString{};
        highlighterTags.DOLCH_ADVERB_BEGIN = IsHighlightingDolchAdverbs() ?
                                                 wxString{ LR"(<span class="hl-dolch-adverb">)" } :
                                                 wxString{};
        highlighterTags.DOLCH_ADJECTIVE_BEGIN =
            IsHighlightingDolchAdjectives() ? wxString{ LR"(<span class="hl-dolch-adjective">)" } :
                                              wxString{};
        highlighterTags.DOLCH_VERB_BEGIN = IsHighlightingDolchVerbs() ?
                                               wxString{ LR"(<span class="hl-dolch-verb">)" } :
                                               wxString{};
        highlighterTags.DOLCH_NOUN_BEGIN = IsHighlightingDolchNouns() ?
                                               wxString{ LR"(<span class="hl-dolch-noun">)" } :
                                               wxString{};

        // Legend swatches show the color as a background chip.
        // The actual color (and its light/dark handling) comes from the .hl-swatch-* rules in
        // BuildStyleSheet
        highlighterTags.HIGHLIGHT_BEGIN_LEGEND =
            wxString{ LR"(<span class="hl-swatch hl-swatch-default">)" };
        highlighterTags.DUPLICATE_HIGHLIGHT_BEGIN_LEGEND =
            wxString{ LR"(<span class="hl-swatch hl-swatch-error">)" };
        highlighterTags.PHRASE_HIGHLIGHT_BEGIN_LEGEND =
            wxString{ LR"(<span class="hl-swatch hl-swatch-phrase">)" };
        highlighterTags.IGNORE_HIGHLIGHT_BEGIN_LEGEND =
            wxString{ LR"(<span class="hl-swatch hl-swatch-excluded">)" };
        highlighterTags.DOLCH_CONJUNCTION_BEGIN_LEGEND =
            IsHighlightingDolchConjunctions() ?
                wxString{ LR"(<span class="hl-swatch hl-swatch-dolch-conjunction">)" } :
                wxString{};
        highlighterTags.DOLCH_PREPOSITIONS_BEGIN_LEGEND =
            IsHighlightingDolchPrepositions() ?
                wxString{ LR"(<span class="hl-swatch hl-swatch-dolch-preposition">)" } :
                wxString{};
        highlighterTags.DOLCH_PRONOUN_BEGIN_LEGEND =
            IsHighlightingDolchPronouns() ?
                wxString{ LR"(<span class="hl-swatch hl-swatch-dolch-pronoun">)" } :
                wxString{};
        highlighterTags.DOLCH_ADVERB_BEGIN_LEGEND =
            IsHighlightingDolchAdverbs() ?
                wxString{ LR"(<span class="hl-swatch hl-swatch-dolch-adverb">)" } :
                wxString{};
        highlighterTags.DOLCH_ADJECTIVE_BEGIN_LEGEND =
            IsHighlightingDolchAdjectives() ?
                wxString{ LR"(<span class="hl-swatch hl-swatch-dolch-adjective">)" } :
                wxString{};
        highlighterTags.DOLCH_VERB_BEGIN_LEGEND =
            IsHighlightingDolchVerbs() ?
                wxString{ LR"(<span class="hl-swatch hl-swatch-dolch-verb">)" } :
                wxString{};
        highlighterTags.DOLCH_NOUN_BEGIN_LEGEND =
            IsHighlightingDolchNouns() ?
                wxString{ LR"(<span class="hl-swatch hl-swatch-dolch-noun">)" } :
                wxString{};
        }

    return highlighterTags;
    }

//-------------------------------------------------------
wxString ProjectDoc::EncodeLegendLabel(const wxString& label, const MarkupFormat format)
    {
    if (IsRtf(format))
        {
        constexpr lily_of_the_valley::rtf_encode_text RTF_ENCODE;
        return RTF_ENCODE({ label.wc_str(), label.length() });
        }
    else
        {
        return lily_of_the_valley::html_encode_text::simple_encode(
            { label.wc_str(), label.length() });
        }
    }

//-------------------------------------------------------
ProjectDoc::TextLegendLines
ProjectDoc::BuildLegendLines(const HighlighterTags& highlighterTags) const
    {
    TextLegendLines legendLines;

    // clang-format off
    wxString currentLegendLabel{ _(L"Excluded text") };
    legendLines.ignoredSentencesLegendLine =
        wxString::Format(L"%s    %s   %s  %s%s", highlighterTags.CRLF.c_str(),
            highlighterTags.IGNORE_HIGHLIGHT_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
            EncodeLegendLabel(currentLegendLabel, highlighterTags.format),
        highlighterTags.CRLF.c_str());
    currentLegendLabel = _(L"3+ syllable words");
    legendLines.hardWordsLegendLine = wxString::Format(L"%s    %s   %s  %s%s",
            highlighterTags.CRLF,
            highlighterTags.HIGHLIGHT_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
            EncodeLegendLabel(currentLegendLabel, highlighterTags.format),
        highlighterTags.CRLF.c_str());
    currentLegendLabel = _(L"6+ character words");
    legendLines.longWordsLegendLine =
        wxString::Format(L"%s    %s   %s  %s%s", highlighterTags.CRLF.c_str(),
            highlighterTags.HIGHLIGHT_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
            EncodeLegendLabel(currentLegendLabel, highlighterTags.format),
        highlighterTags.CRLF.c_str());
    currentLegendLabel = _(L"Unfamiliar New Dale-Chall words");
    legendLines.unfamiliarDCWordsLegendLine =
        wxString::Format(L"%s    %s   %s  %s%s", highlighterTags.CRLF.c_str(),
            highlighterTags.HIGHLIGHT_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
            EncodeLegendLabel(currentLegendLabel, highlighterTags.format),
        highlighterTags.CRLF.c_str());
    currentLegendLabel = _(L"Unfamiliar Spache Revised words");
    legendLines.unfamiliarSpacheWordsLegendLine =
        wxString::Format(L"%s    %s   %s  %s%s", highlighterTags.CRLF.c_str(),
            highlighterTags.HIGHLIGHT_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
            EncodeLegendLabel(currentLegendLabel, highlighterTags.format),
        highlighterTags.CRLF.c_str());
    currentLegendLabel = _(L"Unfamiliar Harris-Jacobson words");
    legendLines.unfamiliarHarrisJacobsonWordsLegendLine =
        wxString::Format(L"%s    %s   %s  %s%s", highlighterTags.CRLF.c_str(),
            highlighterTags.HIGHLIGHT_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
            EncodeLegendLabel(currentLegendLabel, highlighterTags.format),
        highlighterTags.CRLF.c_str());
    currentLegendLabel = _(L"Overly-long sentences");
    legendLines.longSentencesLegendLine =
        wxString::Format(L"%s    %s   %s  %s%s", highlighterTags.CRLF.c_str(),
            highlighterTags.HIGHLIGHT_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
            EncodeLegendLabel(currentLegendLabel, highlighterTags.format),
        highlighterTags.CRLF.c_str());
    currentLegendLabel =
        (GetProjectLanguage() == readability::test_language::english_test) ?
        _(L"Errors (Repeated words, wording errors, mismatched articles, and misspellings)") :
        _(L"Errors (Repeated words)");
    legendLines.grammarIssuesLegendLine =
        wxString::Format(L"%s    %s   %s  %s%s", highlighterTags.CRLF.c_str(),
            highlighterTags.DUPLICATE_HIGHLIGHT_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
            EncodeLegendLabel(currentLegendLabel, highlighterTags.format),
        highlighterTags.CRLF.c_str());
    currentLegendLabel =
        _(L"Style (Wordy items, redundant phrases, passive voice, and clich\u00E9s)");
    legendLines.writingStyleLegendLine =
        (GetProjectLanguage() == readability::test_language::english_test) ?
        wxString::Format(L"%s    %s   %s  %s%s", highlighterTags.CRLF.c_str(),
            highlighterTags.PHRASE_HIGHLIGHT_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
            EncodeLegendLabel(currentLegendLabel, highlighterTags.format),
                             highlighterTags.CRLF.c_str()) :
        wxString{};
    currentLegendLabel = _(L"Dolch conjunctions");
    legendLines.dolch1WordsLegendLine = IsHighlightingDolchConjunctions() ?
        wxString::Format(L"%s    %s   %s  %s%s", highlighterTags.CRLF.c_str(),
                highlighterTags.DOLCH_CONJUNCTION_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
                EncodeLegendLabel(currentLegendLabel, highlighterTags.format),
                             highlighterTags.CRLF.c_str()) :
        wxString{};
    currentLegendLabel = _(L"Dolch prepositions");
    legendLines.dolch2WordsLegendLine = IsHighlightingDolchPrepositions() ?
        wxString::Format(L"%s    %s   %s  %s%s", highlighterTags.CRLF.c_str(),
                highlighterTags.DOLCH_PREPOSITIONS_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
                EncodeLegendLabel(currentLegendLabel, highlighterTags.format),
                             highlighterTags.CRLF.c_str()) :
        wxString{};
    currentLegendLabel = _(L"Dolch pronouns");
    legendLines.dolch3WordsLegendLine = IsHighlightingDolchPronouns() ?
        wxString::Format(L"%s    %s   %s  %s%s", highlighterTags.CRLF.c_str(),
                highlighterTags.DOLCH_PRONOUN_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
                EncodeLegendLabel(currentLegendLabel, highlighterTags.format),
                             highlighterTags.CRLF.c_str()) :
        wxString{};
    currentLegendLabel = _(L"Dolch adverbs");
    legendLines.dolch4WordsLegendLine = IsHighlightingDolchAdverbs() ?
        wxString::Format(L"%s    %s   %s  %s%s", highlighterTags.CRLF.c_str(),
                highlighterTags.DOLCH_ADVERB_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
                EncodeLegendLabel(currentLegendLabel, highlighterTags.format),
                             highlighterTags.CRLF.c_str()) :
        wxString{};
    currentLegendLabel = _(L"Dolch adjectives");
    legendLines.dolch5WordsLegendLine = IsHighlightingDolchAdjectives() ?
        wxString::Format(L"%s    %s   %s  %s%s", highlighterTags.CRLF.c_str(),
                highlighterTags.DOLCH_ADJECTIVE_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
                EncodeLegendLabel(currentLegendLabel, highlighterTags.format),
                             highlighterTags.CRLF.c_str()) :
        wxString{};
    currentLegendLabel = _(L"Dolch verbs");
    legendLines.dolchVerbsLegendLine = IsHighlightingDolchVerbs() ?
        wxString::Format(L"%s    %s   %s  %s%s", highlighterTags.CRLF.c_str(),
                highlighterTags.DOLCH_VERB_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
                EncodeLegendLabel(currentLegendLabel, highlighterTags.format),
                             highlighterTags.CRLF.c_str()) :
        wxString{};
    currentLegendLabel = _(L"Dolch nouns");
    legendLines.dolchNounsLegendLine = IsHighlightingDolchNouns() ?
        wxString::Format(L"%s    %s   %s  %s%s", highlighterTags.CRLF.c_str(),
                highlighterTags.DOLCH_NOUN_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
                EncodeLegendLabel(currentLegendLabel, highlighterTags.format),
                             highlighterTags.CRLF.c_str()) :
        wxString{};
    currentLegendLabel = _(L"Non-Dolch words");
    legendLines.nonDolchWordsLegendLine =
        wxString::Format(L"%s    %s   %s  %s%s", highlighterTags.CRLF.c_str(),
            highlighterTags.HIGHLIGHT_BEGIN_LEGEND, highlighterTags.HIGHLIGHT_END_LEGEND,
            EncodeLegendLabel(currentLegendLabel, highlighterTags.format),
        highlighterTags.CRLF.c_str());
    // clang-format on

    return legendLines;
    }

//-------------------------------------------------------
wxString ProjectDoc::BuildLegendLine(const HighlighterTags& highlighterTags,
                                     const wxString& legendStr)
    {
    return wxString::Format(
        L"%s    %s   %s  %s%s", highlighterTags.CRLF, highlighterTags.HIGHLIGHT_BEGIN_LEGEND,
        highlighterTags.HIGHLIGHT_END_LEGEND, EncodeLegendLabel(legendStr, highlighterTags.format),
        highlighterTags.CRLF);
    }

//-------------------------------------------------------
wxString ProjectDoc::TidyHtmlLegendBreaks(wxString content)
    {
    const wxString singleBreak{ L"<br />\n" };
    while (content.Replace(singleBreak + singleBreak, singleBreak) > 0)
        {
        }
    if (content.StartsWith(singleBreak))
        {
        content.erase(0, singleBreak.length());
        }
    if (content.EndsWith(singleBreak))
        {
        content.erase(content.length() - singleBreak.length());
        }
    return content;
    }

//-------------------------------------------------------
wxString ProjectDoc::BuildLegend(const MarkupFormat format, const wxString& legendLine,
                                 const ProjectDoc::TextLegendLines& legendLines,
                                 const wxFont& textViewFont)
    {
    if (IsRtf(format))
        {
        return wxString::Format(
            L" \\pard\\fs%u%s%s \\fs%u\\par\n", (textViewFont.GetPointSize() - 2) * 2,
            legendLines.ignoredSentencesLegendLine, legendLine, textViewFont.GetPointSize() * 2);
        }
    else // Html
        {
        return wxString::Format(
            L"\n<div class=\"report-banner legend-card\">"
            "\n<div class=\"report-banner-accent\"></div>"
            "\n<div class=\"report-banner-content\">%s</div>\n</div>\n",
            TidyHtmlLegendBreaks(legendLines.ignoredSentencesLegendLine + legendLine));
        }
    }

//-------------------------------------------------------
ProjectDoc::TextLegends ProjectDoc::BuildLegends(const MarkupFormat format,
                                                 const ProjectDoc::TextLegendLines& legendLines,
                                                 const wxFont& textViewFont)
    {
    // clang-format off
    TextLegends textLegends;
    if (IsRtf(format))
        {
    textLegends.plaintTextWindowLegend = wxString::Format(L" \\pard\\fs%u%s \\fs%u\\par\n",
        (textViewFont.GetPointSize() - 2) * 2,
        legendLines.ignoredSentencesLegendLine,
        textViewFont.GetPointSize() * 2);
    textLegends.hardWordsLegend = wxString::Format(L" \\pard\\fs%u%s%s \\fs%u\\par\n",
        (textViewFont.GetPointSize() - 2) * 2,
        legendLines.ignoredSentencesLegendLine,
        legendLines.hardWordsLegendLine,
        textViewFont.GetPointSize() * 2);
    textLegends.longWordsLegend = wxString::Format(L" \\pard\\fs%u%s%s \\fs%u\\par\n",
        (textViewFont.GetPointSize() - 2) * 2,
        legendLines.ignoredSentencesLegendLine,
        legendLines.longWordsLegendLine,
        textViewFont.GetPointSize() * 2);
    textLegends.unfamiliarDCWordsLegend = wxString::Format(L" \\pard\\fs%u%s%s \\fs%u\\par\n",
        (textViewFont.GetPointSize() - 2) * 2,
        legendLines.ignoredSentencesLegendLine,
        legendLines.unfamiliarDCWordsLegendLine,
        textViewFont.GetPointSize() * 2);
    textLegends.unfamiliarSpacheWordsLegend = wxString::Format(L" \\pard\\fs%u%s%s \\fs%u\\par\n",
        (textViewFont.GetPointSize() - 2) * 2,
        legendLines.ignoredSentencesLegendLine,
        legendLines.unfamiliarSpacheWordsLegendLine,
        textViewFont.GetPointSize() * 2);
    textLegends.unfamiliarHarrisJacobsonWordsLegend = wxString::Format(L" \\pard\\fs%u%s%s \\fs%u\\par\n",
        (textViewFont.GetPointSize() - 2) * 2,
        legendLines.ignoredSentencesLegendLine,
        legendLines.unfamiliarHarrisJacobsonWordsLegendLine,
        textViewFont.GetPointSize() * 2);
    textLegends.dolchWindowLegend = wxString::Format(L" \\pard\\fs%u%s%s%s%s%s%s%s%s \\fs%u\\par\n",
        (textViewFont.GetPointSize() - 2) * 2,
        legendLines.ignoredSentencesLegendLine,
        legendLines.dolch1WordsLegendLine,
        legendLines.dolch2WordsLegendLine,
        legendLines.dolch3WordsLegendLine,
        legendLines.dolch4WordsLegendLine,
        legendLines.dolch5WordsLegendLine,
        legendLines.dolchVerbsLegendLine,
        legendLines.dolchNounsLegendLine,
        textViewFont.GetPointSize() * 2);
    textLegends.nonDolchWordsLegend = wxString::Format(L" \\pard\\fs%u%s%s \\fs%u\\par\n",
        (textViewFont.GetPointSize() - 2) * 2,
        legendLines.ignoredSentencesLegendLine,
        legendLines.nonDolchWordsLegendLine,
        textViewFont.GetPointSize() * 2);
    textLegends.wordinessWindowLegend = wxString::Format(L" \\pard\\fs%u%s%s%s%s \\fs%u\\par\n",
        (textViewFont.GetPointSize() - 2) * 2,
        legendLines.ignoredSentencesLegendLine,
        legendLines.longSentencesLegendLine,
        legendLines.grammarIssuesLegendLine,
        legendLines.writingStyleLegendLine,
        textViewFont.GetPointSize() * 2);
        }
    else // Html
        {
        // Wrap the legend lines in the report theme's banner card (accent gutter +
        // translucent content panel). the .hl-swatch chips carry the colors.
        const auto card = [](const wxString& lines)
        {
            return wxString::Format(
                L"\n<div class=\"report-banner legend-card\">"
                "\n<div class=\"report-banner-accent\"></div>"
                "\n<div class=\"report-banner-content\">%s</div>\n</div>\n",
                TidyHtmlLegendBreaks(lines));
        };
    textLegends.plaintTextWindowLegend = card(
        legendLines.ignoredSentencesLegendLine);
    textLegends.hardWordsLegend = card(
        legendLines.ignoredSentencesLegendLine +
        legendLines.hardWordsLegendLine);
    textLegends.longWordsLegend = card(
        legendLines.ignoredSentencesLegendLine +
        legendLines.longWordsLegendLine);
    textLegends.unfamiliarDCWordsLegend = card(
        legendLines.ignoredSentencesLegendLine +
        legendLines.unfamiliarDCWordsLegendLine);
    textLegends.unfamiliarSpacheWordsLegend = card(
        legendLines.ignoredSentencesLegendLine +
        legendLines.unfamiliarSpacheWordsLegendLine);
    textLegends.unfamiliarHarrisJacobsonWordsLegend = card(
        legendLines.ignoredSentencesLegendLine +
        legendLines.unfamiliarHarrisJacobsonWordsLegendLine);
    textLegends.dolchWindowLegend = card(
        legendLines.ignoredSentencesLegendLine +
        legendLines.dolch1WordsLegendLine +
        legendLines.dolch2WordsLegendLine +
        legendLines.dolch3WordsLegendLine +
        legendLines.dolch4WordsLegendLine +
        legendLines.dolch5WordsLegendLine +
        legendLines.dolchVerbsLegendLine +
        legendLines.dolchNounsLegendLine);
    textLegends.nonDolchWordsLegend = card(
        legendLines.ignoredSentencesLegendLine +
        legendLines.nonDolchWordsLegendLine);
    textLegends.wordinessWindowLegend = card(
        legendLines.ignoredSentencesLegendLine +
        legendLines.longSentencesLegendLine +
        legendLines.grammarIssuesLegendLine +
        legendLines.writingStyleLegendLine);
        }
    return textLegends;
    // clang-format on
    }

//-------------------------------------------------------
ProjectDoc::TextHeader ProjectDoc::BuildHeader(const MarkupFormat format,
                                               const wxColour& backgroundColor,
                                               const HighlighterColors& highlighterColors,
                                               const wxFont& textViewFont)
    {
    TextHeader textHeaders;
    if (IsRtf(format))
        {
        // other formatting
        wxString fontFamily;
        switch (textViewFont.GetFamily())
            {
        case wxFONTFAMILY_DEFAULT:
            fontFamily = L"fnil";
            break;
        case wxFONTFAMILY_DECORATIVE:
            fontFamily = L"fdecor";
            break;
        case wxFONTFAMILY_ROMAN:
            fontFamily = L"froman";
            break;
        case wxFONTFAMILY_SCRIPT:
            fontFamily = L"fscript";
            break;
        case wxFONTFAMILY_SWISS:
            fontFamily = L"fswiss";
            break;
        case wxFONTFAMILY_MODERN:
            fontFamily = L"fmodern";
            break;
        case wxFONTFAMILY_TELETYPE:
            fontFamily = L"ftech";
            break;
        default:
            fontFamily = L"fnil";
            };

        const wxString headerSection = wxString::Format(
            L"{\\rtf1\\ansi\\ansicpg1252\\deff0\\deflang1033{\\fonttbl{\\f0\\%s\\fcharset0 %s;}}",
            fontFamily, textViewFont.GetFaceName());

        const auto [colorTableThemed, mainFontHeaderThemed, ending] =
            BuildColorTable(textViewFont, highlighterColors, backgroundColor);
        textHeaders.endSection = ending;
        textHeaders.colorTable = colorTableThemed;
        textHeaders.mainFontHeader = mainFontHeaderThemed;
        textHeaders.header = headerSection + colorTableThemed + mainFontHeaderThemed;
        }
    else // Html
        {
        // Full self-contained document, in two stylesheets:
        //   1. the highlight classes, emitted first and self-contained so the combined-report
        //      export can lift them straight out with get_style_section() (that export embeds
        //      the theme CSS itself, so only these rules need to travel with it);
        //   2. the report theme CSS (for the legend card banner) and the body font.
        // Page background/text follow the browser via Canvas/CanvasText; the highlight rules
        // only reference theme custom properties, which resolve regardless of block order.
        // Printing is forced back to a light color scheme by the theme CSS, so the output is
        // always on white paper.
        const wxString bodyFontRule = wxString::Format(
            L"\nbody { font-family: \"%s\"; font-size: %dpt; font-style: %s; font-weight: %s;%s }",
            textViewFont.GetFaceName(), textViewFont.GetPointSize(),
            (textViewFont.GetStyle() == wxFONTSTYLE_ITALIC) ? L"italic" : L"normal",
            (textViewFont.GetWeight() == wxFONTWEIGHT_BOLD) ? L"bold" : L"normal",
            textViewFont.GetUnderlined() ? L" text-decoration: underline;" : L"");
        textHeaders.header =
            wxString::Format(L"<!DOCTYPE html>"
                             "\n<html>"
                             "\n<head>"
                             "\n<meta http-equiv=\"content-type\" content=\"text/html; "
                             "charset=UTF-8\" />"
                             "\n<meta name=\"color-scheme\" content=\"light dark\" />"
                             "\n<meta name=\"generator\" content=\"%s\" />"
                             "\n%s"
                             "\n<style>\n%s%s"
                             "\n</style>\n</head>\n<body>\n",
                             wxGetApp().GetAppName(), BuildStyleSheet(),
                             ProjectReportFormat::GetThemeCss(
                                 _DT(L"default.css"), wxGetApp().GetAppOptions()->GetReportTheme()),
                             bodyFontRule);
        textHeaders.endSection = L"\n</body>\n</html>";
        }
    return textHeaders;
    }

//-------------------------------------------------------
void ProjectDoc::DisplayHighlightedText(const wxColour& highlightColor, const wxFont& textViewFont)
    {
    if (GetWords() == nullptr)
        {
        return;
        }

    try
        {
        auto* view = dynamic_cast<ProjectView*>(GetFirstView());

        // this rebuilds every highlighted window, so drop the previous buffers and
        // their cached 'memory:' pages; each window is repopulated via SetContent below
        GetHighlightedTextBuffers().Clear();

        // the display windows render HTML (theme-adaptive); the paper-white
        // export buffer is RTF. macOS and Windows read background-highlight
        // control words differently, so pick the RTF flavor for the host platform.
        constexpr MarkupFormat displayFormat = MarkupFormat::Html;
#ifdef __WXOSX__
        constexpr MarkupFormat exportFormat = MarkupFormat::RtfMacOS;
#else
        constexpr MarkupFormat exportFormat = MarkupFormat::RtfWindows;
#endif

        // build the general highlighters
        const HighlighterColors highlighterColorsThemed =
            BuildReportColors(highlightColor, GetTextReportBackgroundColor());
        const HighlighterTags highlighterTagsThemed =
            BuildHighlighterTags(displayFormat, highlightColor);

        const HighlighterColors highlighterColorsPaperWhite =
            BuildReportColors(highlightColor, wxColour{ 255, 255, 255 });
        const HighlighterTags highlighterTagsPaperWhite =
            BuildHighlighterTags(exportFormat, highlightColor);

        // build the legends
        const TextLegendLines legendLinesThemed = BuildLegendLines(highlighterTagsThemed);
        const TextLegends textLegendsThemed =
            BuildLegends(displayFormat, legendLinesThemed, textViewFont);

        const TextLegendLines legendLinesPaperWhite = BuildLegendLines(highlighterTagsPaperWhite);
        const TextLegends textLegendsPaperWhite =
            BuildLegends(exportFormat, legendLinesPaperWhite, textViewFont);

        // build the headers
        const TextHeader textHeaderThemed = BuildHeader(
            displayFormat, GetTextReportBackgroundColor(), highlighterColorsThemed, textViewFont);
        const TextHeader textHeaderPaperWhite = BuildHeader(
            exportFormat, wxColour{ 255, 255, 255 }, highlighterColorsPaperWhite, textViewFont);

        // specialized highlighters
        SyllableCountGreaterEqualWithHighlighting<word_case_insensitive_no_stem>
            is3PlusSyllablesThemed(
                3, (GetNumeralSyllabicationMethod() == NumeralSyllabize::WholeWordIsOneSyllable),
                highlighterTagsThemed.HIGHLIGHT_BEGIN, highlighterTagsThemed.HIGHLIGHT_END);
        WordLengthGreaterEqualsWithHighlighting<word_case_insensitive_no_stem> is6PlusCharsThemed(
            6, highlighterTagsThemed.HIGHLIGHT_BEGIN, highlighterTagsThemed.HIGHLIGHT_END);
        const IsNotFamiliarWordWithHighlighting<word_case_insensitive_no_stem, const word_list,
                                                stemming::no_op_stem<word_case_insensitive_no_stem>>
            isNotDCWordThemed(IsIncludingStockerCatholicSupplement() ?
                                  &BaseProject::m_dale_chall_plus_stocker_catholic_word_list :
                                  &BaseProject::m_dale_chall_word_list,
                              highlighterTagsThemed.HIGHLIGHT_BEGIN,
                              highlighterTagsThemed.HIGHLIGHT_END,
                              GetDaleChallProperNounCountingMethod());
        IsNotFamiliarWordWithHighlighting<word_case_insensitive_no_stem, const word_list,
                                          stemming::no_op_stem<word_case_insensitive_no_stem>>
            isNotSpacheWordThemed(
                &m_spache_word_list, highlighterTagsThemed.HIGHLIGHT_BEGIN,
                highlighterTagsThemed.HIGHLIGHT_END,
                readability::proper_noun_counting_method::all_proper_nouns_are_familiar);
        const IsNotFamiliarWordExcludeNumeralsWithHighlighting<
            word_case_insensitive_no_stem, const word_list,
            stemming::no_op_stem<word_case_insensitive_no_stem>>
            isNotHJWordThemed(
                &m_harris_jacobson_word_list, highlighterTagsThemed.HIGHLIGHT_BEGIN,
                highlighterTagsThemed.HIGHLIGHT_END, highlighterTagsThemed.IGNORE_HIGHLIGHT_BEGIN,
                highlighterTagsThemed.HIGHLIGHT_END,
                readability::proper_noun_counting_method::all_proper_nouns_are_familiar);
        IsDolchWordWithLevelHighlighting<word_case_insensitive_no_stem> isDolchWordThemed(
            &m_dolch_word_list, highlighterTagsThemed.DOLCH_CONJUNCTION_BEGIN,
            highlighterTagsThemed.DOLCH_PREPOSITIONS_BEGIN,
            highlighterTagsThemed.DOLCH_PRONOUN_BEGIN, highlighterTagsThemed.DOLCH_ADVERB_BEGIN,
            highlighterTagsThemed.DOLCH_ADJECTIVE_BEGIN, highlighterTagsThemed.DOLCH_VERB_BEGIN,
            highlighterTagsThemed.DOLCH_NOUN_BEGIN, highlighterTagsThemed.HIGHLIGHT_END);
        IsNotDolchWordWithLevelHighlighting<word_case_insensitive_no_stem> isNotDolchWordThemed(
            &m_dolch_word_list, highlighterTagsThemed.HIGHLIGHT_BEGIN,
            highlighterTagsThemed.HIGHLIGHT_END);

        SyllableCountGreaterEqualWithHighlighting<word_case_insensitive_no_stem>
            is3PlusSyllablesPaperWhite(
                3, (GetNumeralSyllabicationMethod() == NumeralSyllabize::WholeWordIsOneSyllable),
                highlighterTagsPaperWhite.HIGHLIGHT_BEGIN, highlighterTagsPaperWhite.HIGHLIGHT_END);
        WordLengthGreaterEqualsWithHighlighting<word_case_insensitive_no_stem>
            is6PlusCharsPaperWhite(6, highlighterTagsPaperWhite.HIGHLIGHT_BEGIN,
                                   highlighterTagsPaperWhite.HIGHLIGHT_END);
        const IsNotFamiliarWordWithHighlighting<word_case_insensitive_no_stem, const word_list,
                                                stemming::no_op_stem<word_case_insensitive_no_stem>>
            isNotDCWordPaperWhite(IsIncludingStockerCatholicSupplement() ?
                                      &BaseProject::m_dale_chall_plus_stocker_catholic_word_list :
                                      &BaseProject::m_dale_chall_word_list,
                                  highlighterTagsPaperWhite.HIGHLIGHT_BEGIN,
                                  highlighterTagsPaperWhite.HIGHLIGHT_END,
                                  GetDaleChallProperNounCountingMethod());
        IsNotFamiliarWordWithHighlighting<word_case_insensitive_no_stem, const word_list,
                                          stemming::no_op_stem<word_case_insensitive_no_stem>>
            isNotSpacheWordPaperWhite(
                &m_spache_word_list, highlighterTagsPaperWhite.HIGHLIGHT_BEGIN,
                highlighterTagsPaperWhite.HIGHLIGHT_END,
                readability::proper_noun_counting_method::all_proper_nouns_are_familiar);
        const IsNotFamiliarWordExcludeNumeralsWithHighlighting<
            word_case_insensitive_no_stem, const word_list,
            stemming::no_op_stem<word_case_insensitive_no_stem>>
            isNotHJWordPaperWhite(
                &m_harris_jacobson_word_list, highlighterTagsPaperWhite.HIGHLIGHT_BEGIN,
                highlighterTagsPaperWhite.HIGHLIGHT_END,
                highlighterTagsPaperWhite.IGNORE_HIGHLIGHT_BEGIN,
                highlighterTagsPaperWhite.HIGHLIGHT_END,
                readability::proper_noun_counting_method::all_proper_nouns_are_familiar);
        IsDolchWordWithLevelHighlighting<word_case_insensitive_no_stem> isDolchWordPaperWhite(
            &m_dolch_word_list, highlighterTagsPaperWhite.DOLCH_CONJUNCTION_BEGIN,
            highlighterTagsPaperWhite.DOLCH_PREPOSITIONS_BEGIN,
            highlighterTagsPaperWhite.DOLCH_PRONOUN_BEGIN,
            highlighterTagsPaperWhite.DOLCH_ADVERB_BEGIN,
            highlighterTagsPaperWhite.DOLCH_ADJECTIVE_BEGIN,
            highlighterTagsPaperWhite.DOLCH_VERB_BEGIN, highlighterTagsPaperWhite.DOLCH_NOUN_BEGIN,
            highlighterTagsPaperWhite.HIGHLIGHT_END);
        IsNotDolchWordWithLevelHighlighting<word_case_insensitive_no_stem> isNotDolchWordPaperWhite(
            &m_dolch_word_list, highlighterTagsPaperWhite.HIGHLIGHT_BEGIN,
            highlighterTagsPaperWhite.HIGHLIGHT_END);

        // the display buffer is HTML-encoded, the paper-white export buffer is RTF-encoded
        constexpr bool DISPLAY_USE_RTF_ENCODING = IsRtf(displayFormat);
        constexpr bool EXPORT_USE_RTF_ENCODING = IsRtf(exportFormat);

        // initial buffers should be a bit bigger than the
        // original text since we are adding encoding to it
        const size_t textBufferLength{ GetTextSize() * 3 };

        const bool textBeingExcluded =
            (GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis ||
             GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings);

        // loads the HTML display buffer for a text window
        const auto loadTextBuffer =
            [this, textBeingExcluded, &textHeaderThemed, &highlighterTagsThemed](
                auto& highlighter, const wxString& legend, std::wstring& mainBuffer)
        {
            highlighter.Reset();

            FormatWordCollectionHighlightedWords(
                GetWords(), highlighter, mainBuffer, textHeaderThemed.header.wc_string(),
                textHeaderThemed.endSection.wc_string(), legend.wc_string(),
                highlighterTagsThemed.IGNORE_HIGHLIGHT_BEGIN.wc_string(),
                highlighterTagsThemed.HIGHLIGHT_END.wc_string(), highlighterTagsThemed.TAB_SYMBOL,
                highlighterTagsThemed.CRLF, textBeingExcluded,
                GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings,
                textBeingExcluded, DISPLAY_USE_RTF_ENCODING);
        };

        // loads the paper-white RTF export buffer for a text window
        const auto loadPaperTextBuffer =
            [this, textBeingExcluded, &textHeaderPaperWhite, &highlighterTagsPaperWhite](
                auto& highlighter, const wxString& legend, std::wstring& paperBuffer)
        {
            highlighter.Reset();

            FormatWordCollectionHighlightedWords(
                GetWords(), highlighter, paperBuffer, textHeaderPaperWhite.header.wc_string(),
                textHeaderPaperWhite.endSection.wc_string(), legend.wc_string(),
                highlighterTagsPaperWhite.IGNORE_HIGHLIGHT_BEGIN.wc_string(),
                highlighterTagsPaperWhite.HIGHLIGHT_END.wc_string(),
                highlighterTagsPaperWhite.TAB_SYMBOL, highlighterTagsPaperWhite.CRLF,
                textBeingExcluded,
                GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings,
                textBeingExcluded, EXPORT_USE_RTF_ENCODING);
        };

        std::wstring formattedBuffer;
        formattedBuffer.reserve(textBufferLength);
        std::wstring formattedPaperBuffer;
        formattedPaperBuffer.reserve(textBufferLength);

        std::wstring dcBuffer;
        std::wstring dcPaperBuffer;

        std::wstring hjBuffer;
        std::wstring hjPaperBuffer;

        std::wstring spacheBuffer;
        std::wstring spachePaperBuffer;

        std::wstring threeSyllBuffer;
        std::wstring threeSyllPaperBuffer;

        std::wstring sixCharsBuffer;
        std::wstring sixCharsPaperBuffer;

        std::wstring dolchBuffer;
        std::wstring dolchPaperBuffer;

        std::wstring nonDolchBuffer;
        std::wstring nonDolchPaperBuffer;

        // frees a buffer pair once its content has been handed to a window
        const auto releaseBuffers = [](std::wstring& mainBuffer, std::wstring& paperBuffer)
        {
            mainBuffer.clear();
            mainBuffer.shrink_to_fit();
            paperBuffer.clear();
            paperBuffer.shrink_to_fit();
        };

        // load the formatted text into buffers
        // ------------------------------------

        // DC buffers
        if (GetProjectLanguage() == readability::test_language::english_test &&
            GetWordsBreakdownInfo().IsDCUnfamiliarEnabled() && IsDaleChallLikeTestIncluded())
            {
            dcBuffer.reserve(textBufferLength);
            dcPaperBuffer.reserve(textBufferLength);
            // main buffer
            if (GetDaleChallTextExclusionMode() ==
                SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings)
                {
                FormatWordCollectionHighlightedWords(
                    GetWords(), isNotDCWordThemed, dcBuffer, textHeaderThemed.header.wc_string(),
                    textHeaderThemed.endSection.wc_string(),
                    textLegendsThemed.unfamiliarDCWordsLegend.wc_string(),
                    highlighterTagsThemed.IGNORE_HIGHLIGHT_BEGIN.wc_string(),
                    highlighterTagsThemed.HIGHLIGHT_END.wc_string(),
                    highlighterTagsThemed.TAB_SYMBOL, highlighterTagsThemed.CRLF,
                    // forcibly exclude lists but include headers,
                    // invalid words will also be valid
                    true, true, false, DISPLAY_USE_RTF_ENCODING);
                }
            else
                {
                FormatWordCollectionHighlightedWords(
                    GetWords(), isNotDCWordThemed, dcBuffer, textHeaderThemed.header.wc_string(),
                    textHeaderThemed.endSection.wc_string(),
                    textLegendsThemed.unfamiliarDCWordsLegend.wc_string(),
                    highlighterTagsThemed.IGNORE_HIGHLIGHT_BEGIN.wc_string(),
                    highlighterTagsThemed.HIGHLIGHT_END.wc_string(),
                    highlighterTagsThemed.TAB_SYMBOL, highlighterTagsThemed.CRLF, textBeingExcluded,
                    GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings,
                    textBeingExcluded, DISPLAY_USE_RTF_ENCODING);
                }

            // paper (printable) buffer
            if (GetDaleChallTextExclusionMode() ==
                SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings)
                {
                FormatWordCollectionHighlightedWords(
                    GetWords(), isNotDCWordPaperWhite, dcPaperBuffer,
                    textHeaderPaperWhite.header.wc_string(),
                    textHeaderPaperWhite.endSection.wc_string(),
                    textLegendsPaperWhite.unfamiliarDCWordsLegend.wc_string(),
                    highlighterTagsPaperWhite.IGNORE_HIGHLIGHT_BEGIN.wc_string(),
                    highlighterTagsPaperWhite.HIGHLIGHT_END.wc_string(),
                    highlighterTagsPaperWhite.TAB_SYMBOL, highlighterTagsPaperWhite.CRLF,
                    // forcibly exclude lists but include headers,
                    // invalid words will also be valid
                    true, true, false, EXPORT_USE_RTF_ENCODING);
                }
            else
                {
                FormatWordCollectionHighlightedWords(
                    GetWords(), isNotDCWordPaperWhite, dcPaperBuffer,
                    textHeaderPaperWhite.header.wc_string(),
                    textHeaderPaperWhite.endSection.wc_string(),
                    textLegendsPaperWhite.unfamiliarDCWordsLegend.wc_string(),
                    highlighterTagsPaperWhite.IGNORE_HIGHLIGHT_BEGIN.wc_string(),
                    highlighterTagsPaperWhite.HIGHLIGHT_END.wc_string(),
                    highlighterTagsPaperWhite.TAB_SYMBOL, highlighterTagsPaperWhite.CRLF,
                    textBeingExcluded,
                    GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings,
                    textBeingExcluded, EXPORT_USE_RTF_ENCODING);
                }
            }

        // HJ buffers
        if (GetProjectLanguage() == readability::test_language::english_test &&
            GetWordsBreakdownInfo().IsHarrisJacobsonUnfamiliarEnabled() &&
            GetReadabilityTests().is_test_included(ReadabilityMessages::HARRIS_JACOBSON()))
            {
            hjBuffer.reserve(textBufferLength);
            hjPaperBuffer.reserve(textBufferLength);
            if (GetHarrisJacobsonTextExclusionMode() ==
                SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings)
                {
                FormatWordCollectionHighlightedWords(
                    GetWords(), isNotHJWordThemed, hjBuffer, textHeaderThemed.header.wc_string(),
                    textHeaderThemed.endSection.wc_string(),
                    textLegendsThemed.unfamiliarHarrisJacobsonWordsLegend.wc_string(),
                    highlighterTagsThemed.IGNORE_HIGHLIGHT_BEGIN.wc_string(),
                    highlighterTagsThemed.HIGHLIGHT_END.wc_string(),
                    highlighterTagsThemed.TAB_SYMBOL, highlighterTagsThemed.CRLF,
                    // HJ explicitly states what to exclude, so always show what it is
                    // excluding in this window
                    true, true, false, DISPLAY_USE_RTF_ENCODING);
                }
            else
                {
                FormatWordCollectionHighlightedWords(
                    GetWords(), isNotHJWordThemed, hjBuffer, textHeaderThemed.header.wc_string(),
                    textHeaderThemed.endSection.wc_string(),
                    textLegendsThemed.unfamiliarHarrisJacobsonWordsLegend.wc_string(),
                    highlighterTagsThemed.IGNORE_HIGHLIGHT_BEGIN.wc_string(),
                    highlighterTagsThemed.HIGHLIGHT_END.wc_string(),
                    highlighterTagsThemed.TAB_SYMBOL, highlighterTagsThemed.CRLF, textBeingExcluded,
                    GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings,
                    textBeingExcluded, DISPLAY_USE_RTF_ENCODING);
                }

            // paper
            if (GetHarrisJacobsonTextExclusionMode() ==
                SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings)
                {
                FormatWordCollectionHighlightedWords(
                    GetWords(), isNotHJWordPaperWhite, hjPaperBuffer,
                    textHeaderPaperWhite.header.wc_string(),
                    textHeaderPaperWhite.endSection.wc_string(),
                    textLegendsPaperWhite.unfamiliarHarrisJacobsonWordsLegend.wc_string(),
                    highlighterTagsPaperWhite.IGNORE_HIGHLIGHT_BEGIN.wc_string(),
                    highlighterTagsPaperWhite.HIGHLIGHT_END.wc_string(),
                    highlighterTagsPaperWhite.TAB_SYMBOL, highlighterTagsPaperWhite.CRLF,
                    // HJ explicitly states what to exclude, so always show what it
                    // is excluding in this window
                    true, true, false, EXPORT_USE_RTF_ENCODING);
                }
            else
                {
                FormatWordCollectionHighlightedWords(
                    GetWords(), isNotHJWordPaperWhite, hjPaperBuffer,
                    textHeaderPaperWhite.header.wc_string(),
                    textHeaderPaperWhite.endSection.wc_string(),
                    textLegendsPaperWhite.unfamiliarHarrisJacobsonWordsLegend.wc_string(),
                    highlighterTagsPaperWhite.IGNORE_HIGHLIGHT_BEGIN.wc_string(),
                    highlighterTagsPaperWhite.HIGHLIGHT_END.wc_string(),
                    highlighterTagsPaperWhite.TAB_SYMBOL, highlighterTagsPaperWhite.CRLF,
                    textBeingExcluded,
                    GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings,
                    textBeingExcluded, EXPORT_USE_RTF_ENCODING);
                }
            }

        // Spache
        if (GetProjectLanguage() == readability::test_language::english_test &&
            GetWordsBreakdownInfo().IsSpacheUnfamiliarEnabled() &&
            GetReadabilityTests().is_test_included(ReadabilityMessages::SPACHE()))
            {
            spacheBuffer.reserve(textBufferLength);
            spachePaperBuffer.reserve(textBufferLength);
            loadTextBuffer(isNotSpacheWordThemed, textLegendsThemed.unfamiliarSpacheWordsLegend,
                           spacheBuffer);
            loadPaperTextBuffer(isNotSpacheWordPaperWhite,
                                textLegendsPaperWhite.unfamiliarSpacheWordsLegend,
                                spachePaperBuffer);
            }

        // 3+ syllable words
        if (GetWordsBreakdownInfo().Is3PlusSyllablesEnabled() &&
            GetTotalUnique3PlusSyllableWords() > 0)
            {
            threeSyllBuffer.reserve(textBufferLength);
            threeSyllPaperBuffer.reserve(textBufferLength);
            loadTextBuffer(is3PlusSyllablesThemed, textLegendsThemed.hardWordsLegend,
                           threeSyllBuffer);
            loadPaperTextBuffer(is3PlusSyllablesPaperWhite, textLegendsPaperWhite.hardWordsLegend,
                                threeSyllPaperBuffer);
            }

        // 6+ char words
        if (GetWordsBreakdownInfo().Is6PlusCharacterEnabled() &&
            GetTotalUnique6CharsPlusWords() > 0)
            {
            sixCharsBuffer.reserve(textBufferLength);
            sixCharsPaperBuffer.reserve(textBufferLength);
            loadTextBuffer(is6PlusCharsThemed, textLegendsThemed.longWordsLegend, sixCharsBuffer);
            loadPaperTextBuffer(is6PlusCharsPaperWhite, textLegendsPaperWhite.longWordsLegend,
                                sixCharsPaperBuffer);
            }

        // Dolch
        if (IsIncludingDolchSightWords())
            {
            dolchBuffer.reserve(textBufferLength);
            dolchPaperBuffer.reserve(textBufferLength);
            loadTextBuffer(isDolchWordThemed, textLegendsThemed.dolchWindowLegend, dolchBuffer);
            loadPaperTextBuffer(isDolchWordPaperWhite, textLegendsPaperWhite.dolchWindowLegend,
                                dolchPaperBuffer);

            // Non-Dolch
            nonDolchBuffer.reserve(textBufferLength);
            nonDolchPaperBuffer.reserve(textBufferLength);
            loadTextBuffer(isNotDolchWordThemed, textLegendsThemed.nonDolchWordsLegend,
                           nonDolchBuffer);
            loadPaperTextBuffer(isNotDolchWordPaperWhite, textLegendsPaperWhite.nonDolchWordsLegend,
                                nonDolchPaperBuffer);
            }

        // Load the buffers into the windows (or hide windows is not relevant anymore)
        // ---------------------------------------------------------------------------
        LoadDCTextWindow(dcBuffer, dcPaperBuffer);
        releaseBuffers(dcBuffer, dcPaperBuffer);
        LoadHJTextWindow(hjBuffer, hjPaperBuffer);
        releaseBuffers(hjBuffer, hjPaperBuffer);
        LoadSpacheTextWindow(spacheBuffer, spachePaperBuffer);
        releaseBuffers(spacheBuffer, spachePaperBuffer);
        LoadThreeSyllTextWindow(threeSyllBuffer, threeSyllPaperBuffer);
        releaseBuffers(threeSyllBuffer, threeSyllPaperBuffer);
        LoadSixCharsTextWindow(sixCharsBuffer, sixCharsPaperBuffer);
        releaseBuffers(sixCharsBuffer, sixCharsPaperBuffer);

        // go through the custom readability tests
        for (auto pos = GetCustTestsInUse().begin(); pos != GetCustTestsInUse().end(); ++pos)
            {
            const wxString windowLabel(wxString::Format(_(L"%s (Unfamiliar) Report"),
                                                        pos->GetIterator()->get_name().c_str()));
            auto* textWindow =
                dynamic_cast<wxWebView*>(view->GetWordsBreakdownView().FindWindowById(
                    pos->GetIterator()->get_interface_id(), CLASSINFO(wxWebView)));

            if (pos->GetIterator()->is_using_familiar_words())
                {
                if (textWindow == nullptr)
                    {
                    textWindow = CreateHighlightedTextWindow(
                        view->GetSplitter(), pos->GetIterator()->get_interface_id(), windowLabel);
                    // find respective list and add it beneath that
                    auto buddyWindowPosition = view->GetWordsBreakdownView().FindWindowPositionById(
                        pos->GetIterator()->get_interface_id(),
                        CLASSINFO(Wisteria::UI::ListCtrlEx));
                    view->GetWordsBreakdownView().InsertWindow(
                        (buddyWindowPosition != wxNOT_FOUND) ? buddyWindowPosition + 1 : 0,
                        textWindow);
                    }

                const wxString unfamiliarWordsLegendLineThemed =
                    BuildLegendLine(highlighterTagsThemed,
                                    wxString::Format(_(L"Unfamiliar %s words"),
                                                     pos->GetIterator()->get_name().c_str()));
                const wxString unfamiliarWordsLegendThemed =
                    BuildLegend(displayFormat, unfamiliarWordsLegendLineThemed, legendLinesThemed,
                                textViewFont);

                IsNotCustomFamiliarWordWithHighlighting<
                    std::vector<CustomReadabilityTestInterface>::iterator>
                    notCustomWordThemed(pos, highlighterTagsThemed.HIGHLIGHT_BEGIN,
                                        highlighterTagsThemed.HIGHLIGHT_END);
                IsNotCustomFamiliarWordExcludeNumeralsWithHighlighting<
                    std::vector<CustomReadabilityTestInterface>::iterator>
                    notCustomWordExcludeNumeralsThemed(pos, highlighterTagsThemed.HIGHLIGHT_BEGIN,
                                                       highlighterTagsThemed.HIGHLIGHT_END,
                                                       highlighterTagsThemed.IGNORE_HIGHLIGHT_BEGIN,
                                                       highlighterTagsThemed.HIGHLIGHT_END);

                IsNotCustomFamiliarWordWithHighlighting<
                    std::vector<CustomReadabilityTestInterface>::iterator>
                    notCustomWordPaperWhite(pos, highlighterTagsPaperWhite.HIGHLIGHT_BEGIN,
                                            highlighterTagsPaperWhite.HIGHLIGHT_END);
                IsNotCustomFamiliarWordExcludeNumeralsWithHighlighting<
                    std::vector<CustomReadabilityTestInterface>::iterator>
                    notCustomWordExcludeNumeralsPaperWhite(
                        pos, highlighterTagsPaperWhite.HIGHLIGHT_BEGIN,
                        highlighterTagsPaperWhite.HIGHLIGHT_END,
                        highlighterTagsPaperWhite.IGNORE_HIGHLIGHT_BEGIN,
                        highlighterTagsPaperWhite.HIGHLIGHT_END);

                // clear cached first instances of proper nouns
                notCustomWordThemed.Reset();
                notCustomWordExcludeNumeralsThemed.Reset();

                // special text exclusion logic is used for Custom HJ and DC tests
                if ((pos->IsHarrisJacobsonFormula() &&
                     GetHarrisJacobsonTextExclusionMode() ==
                         SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ||
                    (pos->IsDaleChallFormula() &&
                     GetDaleChallTextExclusionMode() ==
                         SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings))
                    {
                    if (pos->IsHarrisJacobsonFormula())
                        {
                        FormatWordCollectionHighlightedWords(
                            GetWords(), notCustomWordExcludeNumeralsThemed, formattedBuffer,
                            textHeaderThemed.header.wc_string(),
                            textHeaderThemed.endSection.wc_string(),
                            unfamiliarWordsLegendThemed.wc_string(),
                            highlighterTagsThemed.IGNORE_HIGHLIGHT_BEGIN.wc_string(),
                            highlighterTagsThemed.HIGHLIGHT_END.wc_string(),
                            highlighterTagsThemed.TAB_SYMBOL, highlighterTagsThemed.CRLF,
                            // forcibly exclude lists but include headers,
                            // invalid words will also be valid
                            true, true, false, DISPLAY_USE_RTF_ENCODING);
                        }
                    else
                        {
                        FormatWordCollectionHighlightedWords(
                            GetWords(), notCustomWordThemed, formattedBuffer,
                            textHeaderThemed.header.wc_string(),
                            textHeaderThemed.endSection.wc_string(),
                            unfamiliarWordsLegendThemed.wc_string(),
                            highlighterTagsThemed.IGNORE_HIGHLIGHT_BEGIN.wc_string(),
                            highlighterTagsThemed.HIGHLIGHT_END.wc_string(),
                            highlighterTagsThemed.TAB_SYMBOL, highlighterTagsThemed.CRLF,
                            // forcibly exclude lists but include headers,
                            // invalid words will also be valid
                            true, true, false, DISPLAY_USE_RTF_ENCODING);
                        }
                    }
                else
                    {
                    if (pos->IsHarrisJacobsonFormula())
                        {
                        FormatWordCollectionHighlightedWords(
                            GetWords(), notCustomWordExcludeNumeralsThemed, formattedBuffer,
                            textHeaderThemed.header.wc_string(),
                            textHeaderThemed.endSection.wc_string(),
                            unfamiliarWordsLegendThemed.wc_string(),
                            highlighterTagsThemed.IGNORE_HIGHLIGHT_BEGIN.wc_string(),
                            highlighterTagsThemed.HIGHLIGHT_END.wc_string(),
                            highlighterTagsThemed.TAB_SYMBOL, highlighterTagsThemed.CRLF,
                            textBeingExcluded,
                            GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings,
                            textBeingExcluded, DISPLAY_USE_RTF_ENCODING);
                        }
                    else
                        {
                        FormatWordCollectionHighlightedWords(
                            GetWords(), notCustomWordThemed, formattedBuffer,
                            textHeaderThemed.header.wc_string(),
                            textHeaderThemed.endSection.wc_string(),
                            unfamiliarWordsLegendThemed.wc_string(),
                            highlighterTagsThemed.IGNORE_HIGHLIGHT_BEGIN.wc_string(),
                            highlighterTagsThemed.HIGHLIGHT_END.wc_string(),
                            highlighterTagsThemed.TAB_SYMBOL, highlighterTagsThemed.CRLF,
                            textBeingExcluded,
                            GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings,
                            textBeingExcluded, DISPLAY_USE_RTF_ENCODING);
                        }
                    }

                notCustomWordThemed.Reset();
                notCustomWordExcludeNumeralsThemed.Reset();

                notCustomWordPaperWhite.Reset();
                notCustomWordExcludeNumeralsPaperWhite.Reset();

                const wxString unfamiliarWordsLegendLinePaperWhite =
                    BuildLegendLine(highlighterTagsPaperWhite,
                                    wxString::Format(_(L"Unfamiliar %s words"),
                                                     pos->GetIterator()->get_name().c_str()));
                const wxString unfamiliarWordsLegendPaperWhite =
                    BuildLegend(exportFormat, unfamiliarWordsLegendLinePaperWhite,
                                legendLinesPaperWhite, textViewFont);

                if ((pos->IsHarrisJacobsonFormula() &&
                     GetHarrisJacobsonTextExclusionMode() ==
                         SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ||
                    (pos->IsDaleChallFormula() &&
                     GetDaleChallTextExclusionMode() ==
                         SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings))
                    {
                    if (pos->IsHarrisJacobsonFormula())
                        {
                        FormatWordCollectionHighlightedWords(
                            GetWords(), notCustomWordExcludeNumeralsPaperWhite,
                            formattedPaperBuffer, textHeaderPaperWhite.header.wc_string(),
                            textHeaderPaperWhite.endSection.wc_string(),
                            unfamiliarWordsLegendPaperWhite.wc_string(),
                            highlighterTagsPaperWhite.IGNORE_HIGHLIGHT_BEGIN.wc_string(),
                            highlighterTagsPaperWhite.HIGHLIGHT_END.wc_string(),
                            highlighterTagsPaperWhite.TAB_SYMBOL, highlighterTagsPaperWhite.CRLF,
                            // forcibly exclude lists but include headers,
                            // invalid words will also be valid
                            true, true, false, EXPORT_USE_RTF_ENCODING);
                        }
                    else
                        {
                        FormatWordCollectionHighlightedWords(
                            GetWords(), notCustomWordPaperWhite, formattedPaperBuffer,
                            textHeaderPaperWhite.header.wc_string(),
                            textHeaderPaperWhite.endSection.wc_string(),
                            unfamiliarWordsLegendPaperWhite.wc_string(),
                            highlighterTagsPaperWhite.IGNORE_HIGHLIGHT_BEGIN.wc_string(),
                            highlighterTagsPaperWhite.HIGHLIGHT_END.wc_string(),
                            highlighterTagsPaperWhite.TAB_SYMBOL, highlighterTagsPaperWhite.CRLF,
                            // forcibly exclude lists but include headers,
                            // invalid words will also be valid
                            true, true, false, EXPORT_USE_RTF_ENCODING);
                        }
                    }
                else
                    {
                    if (pos->IsHarrisJacobsonFormula())
                        {
                        FormatWordCollectionHighlightedWords(
                            GetWords(), notCustomWordExcludeNumeralsPaperWhite,
                            formattedPaperBuffer, textHeaderPaperWhite.header.wc_string(),
                            textHeaderPaperWhite.endSection.wc_string(),
                            unfamiliarWordsLegendPaperWhite.wc_string(),
                            highlighterTagsPaperWhite.IGNORE_HIGHLIGHT_BEGIN.wc_string(),
                            highlighterTagsPaperWhite.HIGHLIGHT_END.wc_string(),
                            highlighterTagsPaperWhite.TAB_SYMBOL, highlighterTagsPaperWhite.CRLF,
                            textBeingExcluded,
                            GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings,
                            textBeingExcluded, EXPORT_USE_RTF_ENCODING);
                        }
                    else
                        {
                        FormatWordCollectionHighlightedWords(
                            GetWords(), notCustomWordPaperWhite, formattedPaperBuffer,
                            textHeaderPaperWhite.header.wc_string(),
                            textHeaderPaperWhite.endSection.wc_string(),
                            unfamiliarWordsLegendPaperWhite.wc_string(),
                            highlighterTagsPaperWhite.IGNORE_HIGHLIGHT_BEGIN.wc_string(),
                            highlighterTagsPaperWhite.HIGHLIGHT_END.wc_string(),
                            highlighterTagsPaperWhite.TAB_SYMBOL, highlighterTagsPaperWhite.CRLF,
                            textBeingExcluded,
                            GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings,
                            textBeingExcluded, EXPORT_USE_RTF_ENCODING);
                        }
                    }
                GetHighlightedTextBuffers().SetContent(textWindow, formattedBuffer,
                                                       formattedPaperBuffer);
                }
            else
                {
                view->GetWordsBreakdownView().RemoveWindowById(
                    pos->GetIterator()->get_interface_id());
                }
            }

        // grammar issues highlighted
        if (GetGrammarInfo().IsHighlightedReportEnabled() && GetTotalWords() > 0)
            {
            // display this in the Grammar section
            auto* textWindow = dynamic_cast<wxWebView*>(view->GetGrammarView().FindWindowById(
                BaseProjectView::LONG_SENTENCES_AND_WORDINESS_TEXT_PAGE_ID));
            if (textWindow == nullptr)
                {
                textWindow = CreateHighlightedTextWindow(
                    view->GetSplitter(), BaseProjectView::LONG_SENTENCES_AND_WORDINESS_TEXT_PAGE_ID,
                    _(L"Highlighted Report"));
                view->GetGrammarView().InsertWindow(0, textWindow);
                }
            FormatWordCollectionHighlightedGrammarIssues(
                GetWords(), GetDifficultSentenceLength(), formattedBuffer,
                textHeaderThemed.header.wc_string(), textHeaderThemed.endSection.wc_string(),
                textLegendsThemed.wordinessWindowLegend.wc_string(),
                highlighterTagsThemed.HIGHLIGHT_BEGIN.wc_string(),
                highlighterTagsThemed.HIGHLIGHT_END.wc_string(),
                highlighterTagsThemed.ERROR_HIGHLIGHT_BEGIN.wc_string(),
                highlighterTagsThemed.PHRASE_HIGHLIGHT_BEGIN.wc_string(),
                highlighterTagsThemed.IGNORE_HIGHLIGHT_BEGIN.wc_string(),
                // if default style is bold, then don't use bold tags internally
                // because that will mess up the RTF
                (textViewFont.GetWeight() == wxFONTWEIGHT_BOLD) ? std::wstring{} :
                                                                  highlighterTagsThemed.BOLD_BEGIN,
                (textViewFont.GetWeight() == wxFONTWEIGHT_BOLD) ? std::wstring{} :
                                                                  highlighterTagsThemed.BOLD_END,
                highlighterTagsThemed.TAB_SYMBOL, highlighterTagsThemed.CRLF, textBeingExcluded,
                textBeingExcluded, DISPLAY_USE_RTF_ENCODING);

            FormatWordCollectionHighlightedGrammarIssues(
                GetWords(), GetDifficultSentenceLength(), formattedPaperBuffer,
                textHeaderPaperWhite.header.wc_string(),
                textHeaderPaperWhite.endSection.wc_string(),
                textLegendsPaperWhite.wordinessWindowLegend.wc_string(),
                highlighterTagsPaperWhite.HIGHLIGHT_BEGIN.wc_string(),
                highlighterTagsPaperWhite.HIGHLIGHT_END.wc_string(),
                highlighterTagsPaperWhite.ERROR_HIGHLIGHT_BEGIN.wc_string(),
                highlighterTagsPaperWhite.PHRASE_HIGHLIGHT_BEGIN.wc_string(),
                highlighterTagsPaperWhite.IGNORE_HIGHLIGHT_BEGIN.wc_string(),
                // if default style is bold, then don't use bold tags internally because
                // that will mess up the RTF
                (textViewFont.GetWeight() == wxFONTWEIGHT_BOLD) ?
                    std::wstring{} :
                    highlighterTagsPaperWhite.BOLD_BEGIN,
                (textViewFont.GetWeight() == wxFONTWEIGHT_BOLD) ?
                    std::wstring{} :
                    highlighterTagsPaperWhite.BOLD_END,
                highlighterTagsPaperWhite.TAB_SYMBOL, highlighterTagsPaperWhite.CRLF,
                textBeingExcluded, textBeingExcluded, EXPORT_USE_RTF_ENCODING);
            GetHighlightedTextBuffers().SetContent(textWindow, formattedBuffer,
                                                   formattedPaperBuffer);
            releaseBuffers(formattedBuffer, formattedPaperBuffer);
            }
        else
            {
            view->GetGrammarView().RemoveWindowById(
                BaseProjectView::LONG_SENTENCES_AND_WORDINESS_TEXT_PAGE_ID);
            }

        // Dolch sight words
        if (IsIncludingDolchSightWords())
            {
                {
                auto* textWindow =
                    dynamic_cast<wxWebView*>(view->GetDolchSightWordsView().FindWindowById(
                        BaseProjectView::DOLCH_WORDS_TEXT_PAGE_ID));
                textWindow =
                    LoadTextWindow(textWindow, BaseProjectView::DOLCH_WORDS_TEXT_PAGE_ID,
                                   _(L"Highlighted Dolch Words"), dolchBuffer, dolchPaperBuffer);
                releaseBuffers(dolchBuffer, dolchPaperBuffer);
                view->GetDolchSightWordsView().AddWindow(textWindow);
                }
                {
                auto* textWindow =
                    dynamic_cast<wxWebView*>(view->GetDolchSightWordsView().FindWindowById(
                        BaseProjectView::NON_DOLCH_WORDS_TEXT_PAGE_ID));
                textWindow = LoadTextWindow(
                    textWindow, BaseProjectView::NON_DOLCH_WORDS_TEXT_PAGE_ID,
                    _(L"Highlighted Non-Dolch Words"), nonDolchBuffer, nonDolchPaperBuffer);
                releaseBuffers(nonDolchBuffer, nonDolchPaperBuffer);
                view->GetDolchSightWordsView().AddWindow(textWindow);
                }
            }
        else
            {
            view->GetDolchSightWordsView().RemoveWindowById(
                BaseProjectView::DOLCH_WORDS_TEXT_PAGE_ID);
            view->GetDolchSightWordsView().RemoveWindowById(
                BaseProjectView::NON_DOLCH_WORDS_TEXT_PAGE_ID);
            }
        }
    catch (...)
        {
        wxMessageBox(_(L"An internal error occurred while formatting the highlighted text. "
                       "Please contact the software vendor."),
                     _(L"Error"), wxICON_EXCLAMATION | wxOK);
        }
    }

//-------------------------------------------------------
wxWebView* ProjectDoc::CreateHighlightedTextWindow(wxWindow* parent, const int ID,
                                                   const wxString& label)
    {
    // two-step creation: register the memory: handler before Create()
    auto* textWindow = wxWebView::New();
    if (textWindow == nullptr)
        {
        wxLogError(_(L"Failed to create wxWebView. No backend available."));
        return nullptr;
        }
    textWindow->RegisterHandler(
        wxSharedPtr<wxWebViewHandler>(new wxWebViewFSHandler(_DT(L"memory"))));
    textWindow->Create(parent, ID);
    textWindow->SetName(label);
    textWindow->Hide();
    // Read-only report, so hide the browser's context menu. Don't block navigation;
    // there are no links to follow, and some backends rewrite the 'memory:' url
    // internally, which a scheme-based veto would wrongly cancel.
    textWindow->EnableContextMenu(false);
    return textWindow;
    }

//-------------------------------------------------------
wxWebView* ProjectDoc::LoadTextWindow(wxWebView* textWindow, const int ID, const wxString& label,
                                      const std::wstring& htmlBuffer, const std::wstring& rtfBuffer)
    {
    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxASSERT_MSG(view, L"Error getting view when loading highlighted text window!");
    if (view == nullptr)
        {
        return nullptr;
        }

    if (textWindow == nullptr)
        {
        textWindow = CreateHighlightedTextWindow(view->GetSplitter(), ID, label);
        }

    GetHighlightedTextBuffers().SetContent(textWindow, htmlBuffer, rtfBuffer);

    return textWindow;
    }

//-------------------------------------------------------
void ProjectDoc::LoadThreeSyllTextWindow(const std::wstring& mainBuffer,
                                         const std::wstring& paperBuffer)
    {
    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxASSERT_MSG(view, L"Error getting view when loading 3+ syllable text window!");
    if (view == nullptr)
        {
        return;
        }

    if (GetWordsBreakdownInfo().Is3PlusSyllablesEnabled() && GetTotalUnique3PlusSyllableWords() > 0)
        {
        auto* textWindow = dynamic_cast<wxWebView*>(
            view->GetWordsBreakdownView().FindWindowById(BaseProjectView::HARD_WORDS_TEXT_PAGE_ID));
        // always included for any language
        textWindow = LoadTextWindow(textWindow, BaseProjectView::HARD_WORDS_TEXT_PAGE_ID,
                                    BaseProjectView::GetThreeSyllableReportWordsLabel(), mainBuffer,
                                    paperBuffer);
        const auto buddyWindowPosition = view->GetWordsBreakdownView().FindWindowPositionById(
            BaseProjectView::HARD_WORDS_LIST_PAGE_ID);
        view->GetWordsBreakdownView().InsertWindow(
            (buddyWindowPosition != wxNOT_FOUND) ? buddyWindowPosition + 1 : 0, textWindow);
        }
    else
        {
        view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::HARD_WORDS_TEXT_PAGE_ID);
        }
    }

//-------------------------------------------------------
void ProjectDoc::LoadSixCharsTextWindow(const std::wstring& mainBuffer,
                                        const std::wstring& paperBuffer)
    {
    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxASSERT_MSG(view, L"Error getting view when loading 6+ character text window!");
    if (view == nullptr)
        {
        return;
        }

    if (GetWordsBreakdownInfo().Is6PlusCharacterEnabled() && GetTotalUnique6CharsPlusWords() > 0)
        {
        auto* textWindow = dynamic_cast<wxWebView*>(
            view->GetWordsBreakdownView().FindWindowById(BaseProjectView::LONG_WORDS_TEXT_PAGE_ID));
        // always included for any language
        textWindow =
            LoadTextWindow(textWindow, BaseProjectView::LONG_WORDS_TEXT_PAGE_ID,
                           BaseProjectView::GetSixCharWordsReportLabel(), mainBuffer, paperBuffer);
        const auto buddyWindowPosition = view->GetWordsBreakdownView().FindWindowPositionById(
            BaseProjectView::LONG_WORDS_LIST_PAGE_ID);
        view->GetWordsBreakdownView().InsertWindow(
            (buddyWindowPosition != wxNOT_FOUND) ? buddyWindowPosition + 1 : 0, textWindow);
        }
    else
        {
        view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::LONG_WORDS_TEXT_PAGE_ID);
        }
    }

//-------------------------------------------------------
void ProjectDoc::LoadSpacheTextWindow(const std::wstring& mainBuffer,
                                      const std::wstring& paperBuffer)
    {
    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxASSERT_MSG(view, L"Error getting view when loading Spache text window!");
    if (view == nullptr)
        {
        return;
        }

    if (GetProjectLanguage() == readability::test_language::english_test &&
        GetWordsBreakdownInfo().IsSpacheUnfamiliarEnabled() &&
        GetReadabilityTests().is_test_included(ReadabilityMessages::SPACHE()))
        {
        m_spacheTextWindow = dynamic_cast<wxWebView*>(view->GetWordsBreakdownView().FindWindowById(
            BaseProjectView::SPACHE_WORDS_TEXT_PAGE_ID));
        m_spacheTextWindow =
            LoadTextWindow(m_spacheTextWindow, BaseProjectView::SPACHE_WORDS_TEXT_PAGE_ID,
                           _(L"Spache (Unfamiliar) Report"), mainBuffer, paperBuffer);
        const auto buddyWindowPosition = view->GetWordsBreakdownView().FindWindowPositionById(
            BaseProjectView::SPACHE_WORDS_LIST_PAGE_ID);
        view->GetWordsBreakdownView().InsertWindow(
            (buddyWindowPosition != wxNOT_FOUND) ? buddyWindowPosition + 1 : 0, m_spacheTextWindow);
        }
    else
        {
        view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::SPACHE_WORDS_TEXT_PAGE_ID);
        }
    }

//-------------------------------------------------------
void ProjectDoc::LoadHJTextWindow(const std::wstring& mainBuffer, const std::wstring& paperBuffer)
    {
    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxASSERT_MSG(view, L"Error getting view when loading Harris-Jacobson text window!");
    if (view == nullptr)
        {
        return;
        }

    if (GetProjectLanguage() == readability::test_language::english_test &&
        GetWordsBreakdownInfo().IsHarrisJacobsonUnfamiliarEnabled() &&
        GetReadabilityTests().is_test_included(ReadabilityMessages::HARRIS_JACOBSON()))
        {
        m_hjTextWindow = dynamic_cast<wxWebView*>(view->GetWordsBreakdownView().FindWindowById(
            BaseProjectView::HARRIS_JACOBSON_WORDS_TEXT_PAGE_ID));
        if (m_hjTextWindow == nullptr)
            {
            m_hjTextWindow = CreateHighlightedTextWindow(
                view->GetSplitter(), BaseProjectView::HARRIS_JACOBSON_WORDS_TEXT_PAGE_ID,
                _(L"Harris-Jacobson (Unfamiliar) Report"));
            }

        GetHighlightedTextBuffers().SetContent(m_hjTextWindow, mainBuffer, paperBuffer);
        const auto buddyWindowPosition = view->GetWordsBreakdownView().FindWindowPositionById(
            BaseProjectView::HARRIS_JACOBSON_WORDS_LIST_PAGE_ID);
        view->GetWordsBreakdownView().InsertWindow(
            (buddyWindowPosition != wxNOT_FOUND) ? buddyWindowPosition + 1 : 0, m_hjTextWindow);
        }
    else
        {
        view->GetWordsBreakdownView().RemoveWindowById(
            BaseProjectView::HARRIS_JACOBSON_WORDS_TEXT_PAGE_ID);
        }
    }

//-------------------------------------------------------
void ProjectDoc::LoadDCTextWindow(const std::wstring& mainBuffer, const std::wstring& paperBuffer)
    {
    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxASSERT_MSG(view, L"Error getting view when loading Dale-Chall text window!");
    if (view == nullptr)
        {
        return;
        }

    if (GetProjectLanguage() == readability::test_language::english_test &&
        GetWordsBreakdownInfo().IsDCUnfamiliarEnabled() && IsDaleChallLikeTestIncluded())
        {
        m_dcTextWindow = dynamic_cast<wxWebView*>(
            view->GetWordsBreakdownView().FindWindowById(BaseProjectView::DC_WORDS_TEXT_PAGE_ID));
        // construct (if needed)
        if (m_dcTextWindow == nullptr)
            {
            m_dcTextWindow = CreateHighlightedTextWindow(view->GetSplitter(),
                                                         BaseProjectView::DC_WORDS_TEXT_PAGE_ID,
                                                         _(L"Dale-Chall (Unfamiliar) Report"));
            }

        GetHighlightedTextBuffers().SetContent(m_dcTextWindow, mainBuffer, paperBuffer);

        const auto buddyWindowPosition = view->GetWordsBreakdownView().FindWindowPositionById(
            BaseProjectView::DC_WORDS_LIST_PAGE_ID);
        view->GetWordsBreakdownView().InsertWindow(
            (buddyWindowPosition != wxNOT_FOUND) ? buddyWindowPosition + 1 : 0, m_dcTextWindow);
        }
    else
        {
        view->GetWordsBreakdownView().RemoveWindowById(BaseProjectView::DC_WORDS_TEXT_PAGE_ID);
        }
    }

//-------------------------------------------------------
bool ProjectDoc::OnSaveDocument(const wxString& filename)
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
            LogMessage(
                _(L"Project appears to be open by another application. Cannot save project."),
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
        // If the file is already there, and it is in use, then fail.
        // otherwise, may be a new project needing to be created. Either way, we need to
        // truncate the file (and maybe create it), so open it for writing.
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

    /* If storing indexed text then include it.
       Note that if text was manually entered then it is always embedded;
       otherwise, it would be lost.*/
    if (GetDocumentStorageMethod() == TextStorage::EmbedText)
        {
        Wisteria::ZipCatalog::WriteText(zip, ProjectContentFileLabel(), GetDocumentText());
        /// @todo This seems to be relic code, these images are never used from the zip file.
        ///     This should probably be removed.
        if (m_plotBackgroundImage.IsOk())
            {
            const wxFileName fn(GetPlotBackGroundImagePath());
            wxString ext{ fn.GetExt() };
            zip.PutNextEntry(fn.GetFullName());
            m_plotBackgroundImage.GetBitmap(m_plotBackgroundImage.GetDefaultSize())
                .ConvertToImage()
                .SaveFile(zip, Wisteria::GraphItems::Image::GetImageFileTypeFromExtension(ext));
            }
        if (m_graphStippleImage.IsOk())
            {
            const wxFileName fn(GetStippleImagePath());
            wxString ext{ fn.GetExt() };
            zip.PutNextEntry(fn.GetFullName());
            m_graphStippleImage.GetBitmap(m_graphStippleImage.GetDefaultSize())
                .ConvertToImage()
                .SaveFile(zip, Wisteria::GraphItems::Image::GetImageFileTypeFromExtension(ext));
            }
        if (m_waterMarkImage.IsOk())
            {
            const wxFileName fn(GetWatermarkLogoPath());
            wxString ext{ fn.GetExt() };
            zip.PutNextEntry(fn.GetFullName());
            m_waterMarkImage.GetBitmap(m_waterMarkImage.GetDefaultSize())
                .ConvertToImage()
                .SaveFile(zip, Wisteria::GraphItems::Image::GetImageFileTypeFromExtension(ext));
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
void ProjectDoc::DisplayOverlyLongSentences()
    {
    // if working with an empty project
    if (GetWords() == nullptr)
        {
        return;
        }

    auto* view = dynamic_cast<ProjectView*>(GetFirstView());

    if (!GetWords())
        {
        view->GetSentencesBreakdownView().RemoveWindowById(
            BaseProjectView::LONG_SENTENCES_LIST_PAGE_ID);
        return;
        }

    // list the overly long sentences
    m_overlyLongSentenceData->DeleteAllItems();
    m_overlyLongSentenceData->SetSize(GetWords()->get_sentence_count(), 3);
    size_t longSentenceCount = 0;
    const grammar::sentence_length_greater_than sentenceGreater(GetDifficultSentenceLength());
    const grammar::complete_sentence_length_greater_than completeSentenceGreater(
        GetDifficultSentenceLength());
    // punctuation markers
    auto punctPos = GetWords()->get_punctuation().cbegin();
    for (auto pos = GetWords()->get_sentences().begin(); pos != GetWords()->get_sentences().end();
         ++pos)
        {
        if ((GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis &&
             completeSentenceGreater(*pos)) ||
            (GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences &&
             sentenceGreater(*pos)) ||
            (GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings &&
             completeSentenceGreater(*pos)))
            {
            const wxString currentSentence = ProjectReportFormat::FormatSentence(
                this, *pos, punctPos, GetWords()->get_punctuation().cend());

            m_overlyLongSentenceData->SetItemText(
                longSentenceCount, 0, currentSentence,
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            if (GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences)
                {
                m_overlyLongSentenceData->SetItemValue(longSentenceCount, 1, pos->get_word_count());
                }
            else
                {
                m_overlyLongSentenceData->SetItemValue(longSentenceCount, 1,
                                                       pos->get_valid_word_count());
                }
            m_overlyLongSentenceData->SetItemValue(
                longSentenceCount++, 2,
                // add 1 to make it one-indexed
                (pos - GetWords()->get_sentences().begin()) + 1,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
            }
        }
    m_overlyLongSentenceData->SetSize(longSentenceCount);

    // long sentences
    auto* listView =
        dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetSentencesBreakdownView().FindWindowById(
            BaseProjectView::LONG_SENTENCES_LIST_PAGE_ID));
    if (GetSentencesBreakdownInfo().IsLongSentencesEnabled() &&
        (m_overlyLongSentenceData->GetItemCount() != 0U))
        {
        if (listView != nullptr)
            {
            listView->SetVirtualDataProvider(m_overlyLongSentenceData);
            listView->SetVirtualDataSize(m_overlyLongSentenceData->GetItemCount());
            listView->DistributeColumns();
            listView->Resort();
            }
        else
            {
            listView = new Wisteria::UI::ListCtrlEx(
                view->GetSplitter(), BaseProjectView::LONG_SENTENCES_LIST_PAGE_ID,
                wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
            listView->Hide();
            listView->SetLabel(BaseProjectView::GetLongSentencesLabel());
            listView->SetName(BaseProjectView::GetLongSentencesLabel());
            listView->EnableGridLines();
            listView->InsertColumn(0, _(L"Sentence"));
            listView->InsertColumn(1, _(L"Word Count"));
            listView->InsertColumn(2, _(L"Sentence #"));
            listView->SetVirtualDataProvider(m_overlyLongSentenceData);
            listView->SetVirtualDataSize(m_overlyLongSentenceData->GetItemCount());
            listView->DistributeColumns();
            listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
            UpdateListOptions(listView);
            // sort by length (high to low), then by index
            // (low to high, order of appearance in the document)
            std::vector<std::pair<size_t, Wisteria::SortDirection>> columnsToSort;
            columnsToSort.emplace_back(1, Wisteria::SortDirection::SortDescending);
            columnsToSort.emplace_back(2, Wisteria::SortDirection::SortAscending);
            listView->SortColumns(columnsToSort);
            view->GetSentencesBreakdownView().AddWindow(listView);
            }
        }
    else
        {
        if (m_overlyLongSentenceData != nullptr && m_overlyLongSentenceData->GetItemCount() == 0 &&
            listView != nullptr)
            {
            listView->SetItemCount(0);
            }
        // we are getting rid of this window (if it was included before)
        view->GetSentencesBreakdownView().RemoveWindowById(
            BaseProjectView::LONG_SENTENCES_LIST_PAGE_ID);
        }
    }

//-------------------------------------------------------
void ProjectDoc::DisplayGrammar()
    {
    // if working with an empty project
    if (GetWords() == nullptr)
        {
        return;
        }

    auto* view = dynamic_cast<ProjectView*>(GetFirstView());

    const auto resetListView = [](Wisteria::UI::ListCtrlEx* listView)
    {
        if (listView != nullptr && listView->GetVirtualDataProvider() != nullptr &&
            listView->GetVirtualDataProvider()->GetItemCount() == 0)
            {
            listView->SetItemCount(0);
            }
    };

    // load issues from phrase based checks
    const auto& wordyIndices = GetWords()->get_known_phrase_indices();
    const auto& wordyPhrases = GetWords()->get_known_phrases().get_phrases();
    m_wordyPhraseData->DeleteAllItems();
    m_wordyPhraseData->SetSize(GetWords()->get_known_phrase_indices().size(), 3);
    m_redundantPhraseData->DeleteAllItems();
    m_redundantPhraseData->SetSize(GetWords()->get_known_phrase_indices().size(), 3);
    m_wordingErrorData->DeleteAllItems();
    m_wordingErrorData->SetSize(GetWords()->get_known_phrase_indices().size(), 3);
    m_clichePhraseData->DeleteAllItems();
    m_clichePhraseData->SetSize(GetWords()->get_known_phrase_indices().size(), 3);
    // NOLINTBEGIN(misc-const-correctness)
    size_t wordyPhraseCount(0), redundantPhraseCount(0), wordingErrorCount(0), clicheCount(0);
    // NOLINTEND(misc-const-correctness)
    for (const auto& wordyIndex : wordyIndices)
        {
        if (wordyPhrases[wordyIndex.second].first.get_type() == grammar::phrase_type::phrase_cliche)
            {
            m_clichePhraseData->SetItemText(
                clicheCount, 0, wordyPhrases[wordyIndex.second].first.to_string().c_str(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_clichePhraseData->SetItemText(
                clicheCount, 1, wordyPhrases[wordyIndex.second].second.c_str(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_clichePhraseData->SetItemValue(
                clicheCount++, 2,
                // make 1-based index
                GetWords()->get_words()[wordyIndex.first].get_sentence_index() + 1,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
            }
        else if (wordyPhrases[wordyIndex.second].first.get_type() ==
                 grammar::phrase_type::phrase_redundant)
            {
            m_redundantPhraseData->SetItemText(
                redundantPhraseCount, 0, wordyPhrases[wordyIndex.second].first.to_string().c_str(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_redundantPhraseData->SetItemText(
                redundantPhraseCount, 1, wordyPhrases[wordyIndex.second].second.c_str(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_redundantPhraseData->SetItemValue(
                redundantPhraseCount++, 2,
                // make 1-based index
                GetWords()->get_words()[wordyIndex.first].get_sentence_index() + 1,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
            }
        else if (wordyPhrases[wordyIndex.second].first.get_type() ==
                 grammar::phrase_type::phrase_error)
            {
            m_wordingErrorData->SetItemText(
                wordingErrorCount, 0, wordyPhrases[wordyIndex.second].first.to_string().c_str(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_wordingErrorData->SetItemText(
                wordingErrorCount, 1, wordyPhrases[wordyIndex.second].second.c_str(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_wordingErrorData->SetItemValue(
                wordingErrorCount++, 2,
                // make 1-based index
                GetWords()->get_words()[wordyIndex.first].get_sentence_index() + 1,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
            }
        else
            {
            m_wordyPhraseData->SetItemText(
                wordyPhraseCount, 0, wordyPhrases[wordyIndex.second].first.to_string().c_str(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_wordyPhraseData->SetItemText(
                wordyPhraseCount, 1, wordyPhrases[wordyIndex.second].second.c_str(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_wordyPhraseData->SetItemValue(
                wordyPhraseCount++, 2,
                // make 1-based index
                GetWords()->get_words()[wordyIndex.first].get_sentence_index() + 1,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
            }
        }
    m_wordyPhraseData->SetSize(wordyPhraseCount);
    m_redundantPhraseData->SetSize(redundantPhraseCount);
    m_wordingErrorData->SetSize(wordingErrorCount);
    m_clichePhraseData->SetSize(clicheCount);

        // Wording errors and known misspellings
        {
        auto* listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
            view->GetGrammarView().FindWindowById(BaseProjectView::WORDING_ERRORS_LIST_PAGE_ID));
        if (GetGrammarInfo().IsWordingErrorsEnabled() && (m_wordingErrorData->GetItemCount() != 0U))
            {
            if (listView != nullptr)
                {
                listView->SetVirtualDataProvider(m_wordingErrorData);
                listView->SetVirtualDataSize(m_wordingErrorData->GetItemCount());
                listView->DistributeColumns();
                listView->Resort();
                }
            else
                {
                listView = new Wisteria::UI::ListCtrlEx(
                    view->GetSplitter(), BaseProjectView::WORDING_ERRORS_LIST_PAGE_ID,
                    wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
                listView->Hide();
                listView->SetLabel(BaseProjectView::GetPhrasingErrorsTabLabel());
                listView->SetName(BaseProjectView::GetPhrasingErrorsTabLabel());
                listView->EnableGridLines();
                listView->InsertColumn(0, BaseProjectView::GetPhrasingErrorsTabLabel());
                listView->InsertColumn(1, _(L"Suggestion"));
                listView->InsertColumn(2, _(L"Sentence #"));
                listView->SetVirtualDataProvider(m_wordingErrorData);
                listView->SetVirtualDataSize(m_wordingErrorData->GetItemCount());
                listView->DistributeColumns();
                listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
                UpdateListOptions(listView);
                listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
                view->GetGrammarView().AddWindow(listView);
                }
            }
        else
            {
            resetListView(listView);
            // we are getting rid of this window (if it was included before)
            view->GetGrammarView().RemoveWindowById(BaseProjectView::WORDING_ERRORS_LIST_PAGE_ID);
            }
        }

        // possibly misspelled words
        {
        frequency_set<traits::case_insensitive_wstring_ex> misspelledWords;
        const auto& misspelledWordIndices = GetWords()->get_misspelled_words();
        for (const auto misspelledWordIndex : misspelledWordIndices)
            {
            misspelledWords.insert(GetWords()->get_word(misspelledWordIndex).c_str());
            }
        m_misspelledWordData->DeleteAllItems();
        m_misspelledWordData->SetSize(misspelledWords.get_data().size(), 2);
        size_t uniqueMisspellingCount = 0; // NOLINT(misc-const-correctness)
        for (const auto& mIter : misspelledWords.get_data())
            {
            m_misspelledWordData->SetItemText(
                uniqueMisspellingCount, 0, mIter.first.c_str(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_misspelledWordData->SetItemValue(uniqueMisspellingCount++, 1, mIter.second);
            }

        m_misspelledWordData->SetSize(misspelledWords.get_data().size());
        auto* listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
            view->GetGrammarView().FindWindowById(BaseProjectView::MISSPELLED_WORD_LIST_PAGE_ID));
        if (GetGrammarInfo().IsMisspellingsEnabled() &&
            (m_misspelledWordData->GetItemCount() != 0U))
            {
            if (listView != nullptr)
                {
                listView->SetVirtualDataProvider(m_misspelledWordData);
                listView->SetVirtualDataSize(m_misspelledWordData->GetItemCount());
                listView->DistributeColumns();
                listView->Resort();
                }
            else
                {
                listView = new Wisteria::UI::ListCtrlEx(
                    view->GetSplitter(), BaseProjectView::MISSPELLED_WORD_LIST_PAGE_ID,
                    wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
                listView->Hide();
                listView->SetLabel(BaseProjectView::GetMisspellingsLabel());
                listView->SetName(BaseProjectView::GetMisspellingsLabel());
                listView->EnableGridLines();
                listView->InsertColumn(0, _(L"Misspelling"));
                listView->InsertColumn(1, _(L"Frequency"));
                listView->SetVirtualDataProvider(m_misspelledWordData);
                listView->SetVirtualDataSize(m_misspelledWordData->GetItemCount());
                listView->DistributeColumns();
                listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
                UpdateListOptions(listView);
                listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
                view->GetGrammarView().AddWindow(listView);
                }
            }
        else
            {
            resetListView(listView);
            // we are getting rid of this window (if it was included before)
            view->GetGrammarView().RemoveWindowById(BaseProjectView::MISSPELLED_WORD_LIST_PAGE_ID);
            }
        }

        // repeated words
        {
        const auto& dupWordIndices = GetWords()->get_duplicate_word_indices();
        m_dupWordData->DeleteAllItems();
        m_dupWordData->SetSize(dupWordIndices.size(), 2);
        for (size_t i = 0; i < dupWordIndices.size(); ++i)
            {
            const word_case_insensitive_no_stem& dupWord = GetWords()->get_word(dupWordIndices[i]);
            m_dupWordData->SetItemText(
                i, 0, wxString::Format(L"%s %s", dupWord.c_str(), dupWord.c_str()),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_dupWordData->SetItemValue(
                i, 1, dupWord.get_sentence_index() + 1,
                // make it one-indexed
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
            }
        auto* listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
            view->GetGrammarView().FindWindowById(BaseProjectView::DUPLICATES_LIST_PAGE_ID));
        if (GetGrammarInfo().IsRepeatedWordsEnabled() && (m_dupWordData->GetItemCount() != 0U))
            {
            if (listView != nullptr)
                {
                listView->SetVirtualDataProvider(m_dupWordData);
                listView->SetVirtualDataSize(m_dupWordData->GetItemCount());
                listView->DistributeColumns();
                listView->Resort();
                }
            else
                {
                listView = new Wisteria::UI::ListCtrlEx(
                    view->GetSplitter(), BaseProjectView::DUPLICATES_LIST_PAGE_ID,
                    wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
                listView->Hide();
                listView->SetLabel(BaseProjectView::GetRepeatedWordsLabel());
                listView->SetName(BaseProjectView::GetRepeatedWordsLabel());
                listView->EnableGridLines();
                listView->InsertColumn(0, _(L"Words that Appear Twice Adjacently"));
                listView->InsertColumn(1, _(L"Sentence #"));
                listView->SetVirtualDataProvider(m_dupWordData);
                listView->SetVirtualDataSize(m_dupWordData->GetItemCount());
                listView->DistributeColumns();
                listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
                UpdateListOptions(listView);
                listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
                view->GetGrammarView().AddWindow(listView);
                }
            }
        else
            {
            resetListView(listView);
            // we are getting rid of this window (if it was included before)
            view->GetGrammarView().RemoveWindowById(BaseProjectView::DUPLICATES_LIST_PAGE_ID);
            }
        }

        // Mismatched articles
        {
        frequency_set<traits::case_insensitive_wstring_ex> articleMismatchesWords;
        const auto& incorrectArticleIndices = GetWords()->get_incorrect_article_indices();
        for (const auto incorrectArticleIndex : incorrectArticleIndices)
            {
            articleMismatchesWords.insert(GetWords()->get_word(incorrectArticleIndex).c_str() +
                                          traits::case_insensitive_wstring_ex(L" ") +
                                          GetWords()->get_word(incorrectArticleIndex + 1).c_str());
            }
        m_incorrectArticleData->DeleteAllItems();
        m_incorrectArticleData->SetSize(articleMismatchesWords.get_data().size(), 2);
        size_t uniqueIncorrectArticleCount = 0; // NOLINT(misc-const-correctness)
        for (const auto& mIter : articleMismatchesWords.get_data())
            {
            m_incorrectArticleData->SetItemText(
                uniqueIncorrectArticleCount, 0, mIter.first.c_str(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_incorrectArticleData->SetItemValue(uniqueIncorrectArticleCount++, 1, mIter.second);
            }
        auto* listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
            view->GetGrammarView().FindWindowById(BaseProjectView::INCORRECT_ARTICLE_PAGE_ID));
        if (GetGrammarInfo().IsArticleMismatchesEnabled() &&
            (m_incorrectArticleData->GetItemCount() != 0U))
            {
            if (listView != nullptr)
                {
                listView->SetVirtualDataProvider(m_incorrectArticleData);
                listView->SetVirtualDataSize(m_incorrectArticleData->GetItemCount());
                listView->DistributeColumns();
                listView->Resort();
                }
            else
                {
                listView = new Wisteria::UI::ListCtrlEx(
                    view->GetSplitter(), BaseProjectView::INCORRECT_ARTICLE_PAGE_ID,
                    wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
                listView->Hide();
                listView->SetLabel(BaseProjectView::GetArticleMismatchesLabel());
                listView->SetName(BaseProjectView::GetArticleMismatchesLabel());
                listView->EnableGridLines();
                listView->InsertColumn(0, _(L"Article Mismatch"));
                listView->InsertColumn(1, _(L"Frequency"));
                listView->SetVirtualDataProvider(m_incorrectArticleData);
                listView->SetVirtualDataSize(m_incorrectArticleData->GetItemCount());
                listView->DistributeColumns();
                listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
                UpdateListOptions(listView);
                listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
                view->GetGrammarView().AddWindow(listView);
                }
            }
        else
            {
            resetListView(listView);
            // we are getting rid of this window (if it was included before)
            view->GetGrammarView().RemoveWindowById(BaseProjectView::INCORRECT_ARTICLE_PAGE_ID);
            }
        }

        // redundant phrases
        {
        auto* listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
            view->GetGrammarView().FindWindowById(BaseProjectView::REDUNDANT_PHRASE_LIST_PAGE_ID));
        if (GetGrammarInfo().IsRedundantPhrasesEnabled() &&
            (m_redundantPhraseData->GetItemCount() != 0U))
            {
            if (listView != nullptr)
                {
                listView->SetVirtualDataProvider(m_redundantPhraseData);
                listView->SetVirtualDataSize(m_redundantPhraseData->GetItemCount());
                listView->DistributeColumns();
                listView->Resort();
                }
            else
                {
                listView = new Wisteria::UI::ListCtrlEx(
                    view->GetSplitter(), BaseProjectView::REDUNDANT_PHRASE_LIST_PAGE_ID,
                    wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
                listView->Hide();
                listView->SetLabel(BaseProjectView::GetRedundantPhrasesTabLabel());
                listView->SetName(BaseProjectView::GetRedundantPhrasesTabLabel());
                listView->EnableGridLines();
                listView->InsertColumn(0, BaseProjectView::GetRedundantPhrasesTabLabel());
                listView->InsertColumn(1, _(L"Suggestion"));
                listView->InsertColumn(2, _(L"Sentence #"));
                listView->SetVirtualDataProvider(m_redundantPhraseData);
                listView->SetVirtualDataSize(m_redundantPhraseData->GetItemCount());
                listView->DistributeColumns();
                listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
                UpdateListOptions(listView);
                listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
                view->GetGrammarView().AddWindow(listView);
                }
            }
        else
            {
            resetListView(listView);
            // we are getting rid of this window (if it was included before)
            view->GetGrammarView().RemoveWindowById(BaseProjectView::REDUNDANT_PHRASE_LIST_PAGE_ID);
            }
        }

        // overused words (by sentence)
        {
        GetOverusedWordsBySentenceData()->SetSize(
            GetWords()->get_overused_words_by_sentence().size(), 4);
        // reset punctuation marker
        auto punctPos = GetWords()->get_punctuation().cbegin();
        size_t previousSentencePos = 0;
        auto previousPunctPos = GetWords()->get_punctuation().cbegin();
        for (auto overUsedWordsListsIter = GetWords()->get_overused_words_by_sentence().begin();
             overUsedWordsListsIter != GetWords()->get_overused_words_by_sentence().end();
             ++overUsedWordsListsIter)
            {
            // Format the full sentence (to show context)
            // Note that some sentences may have multiple issues and are listed more than once,
            // so we need to reset the punctuation make to the start of the sentence if it's the
            // same sentence again.
            if (previousSentencePos == overUsedWordsListsIter->first)
                {
                punctPos = previousPunctPos;
                }
            previousPunctPos = punctPos;
            previousSentencePos = overUsedWordsListsIter->first;
            const wxString currentSentence = ProjectReportFormat::FormatSentence(
                this, GetWords()->get_sentences()[overUsedWordsListsIter->first], punctPos,
                GetWords()->get_punctuation().cend());
            // format the list of like words
            wxString theWords;
            for (const auto overusedWords : overUsedWordsListsIter->second)
                {
                theWords.append(GetWords()->get_word(overusedWords).c_str()).append(L"; ");
                }
            theWords.Trim();
            theWords.RemoveLast();
            GetOverusedWordsBySentenceData()->SetItemText(
                overUsedWordsListsIter - GetWords()->get_overused_words_by_sentence().begin(), 0,
                theWords,
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            GetOverusedWordsBySentenceData()->SetItemText(
                overUsedWordsListsIter - GetWords()->get_overused_words_by_sentence().begin(), 1,
                currentSentence,
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            GetOverusedWordsBySentenceData()->SetItemValue(
                overUsedWordsListsIter - GetWords()->get_overused_words_by_sentence().begin(), 2,
                // Use word count (not valid word count) since this is for grammar analysis.
                // This relates more to what a reader is seeing, not a readability analysis
                GetWords()->get_sentences()[overUsedWordsListsIter->first].get_word_count());
            GetOverusedWordsBySentenceData()->SetItemValue(
                overUsedWordsListsIter - GetWords()->get_overused_words_by_sentence().begin(), 3,
                (overUsedWordsListsIter->first) + 1,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
            }

        auto* listView =
            dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetGrammarView().FindWindowById(
                BaseProjectView::OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID));
        if (GetGrammarInfo().IsOverUsedWordsBySentenceEnabled() &&
            GetOverusedWordsBySentenceData() &&
            (GetOverusedWordsBySentenceData()->GetItemCount() != 0U))
            {
            if (listView != nullptr)
                {
                listView->SetVirtualDataSize(GetOverusedWordsBySentenceData()->GetItemCount());
                listView->Resort();
                listView->DistributeColumns();
                }
            else
                {
                listView = new Wisteria::UI::ListCtrlEx(
                    view->GetSplitter(), BaseProjectView::OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID,
                    wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
                listView->Hide();
                listView->SetLabel(BaseProjectView::GetOverusedWordsBySentenceLabel());
                listView->SetName(BaseProjectView::GetOverusedWordsBySentenceLabel());
                listView->EnableGridLines();
                listView->InsertColumn(0, _(L"Repeated Word"));
                listView->InsertColumn(1, _(L"Sentence"));
                listView->InsertColumn(2, _(L"Sentence Length"));
                listView->InsertColumn(3, _(L"Sentence #"));
                listView->SetVirtualDataProvider(GetOverusedWordsBySentenceData());
                listView->SetVirtualDataSize(GetOverusedWordsBySentenceData()->GetItemCount());
                listView->DistributeColumns();
                listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
                UpdateListOptions(listView);
                // sort by sentence length, then words (lowest to highest)
                std::vector<std::pair<size_t, Wisteria::SortDirection>> columnsToSort;
                columnsToSort.emplace_back(2, Wisteria::SortDirection::SortAscending);
                columnsToSort.emplace_back(0, Wisteria::SortDirection::SortAscending);
                listView->SortColumns(columnsToSort);

                view->GetGrammarView().AddWindow(listView);
                }
            }
        else
            {
            resetListView(listView);
            // we are getting rid of this window (if nothing in it)
            view->GetGrammarView().RemoveWindowById(
                BaseProjectView::OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID);
            }
        }

        // Wordy items
        {
        auto* listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
            view->GetGrammarView().FindWindowById(BaseProjectView::WORDY_PHRASES_LIST_PAGE_ID));
        if (GetGrammarInfo().IsWordyPhrasesEnabled() && (m_wordyPhraseData->GetItemCount() != 0U))
            {
            if (listView != nullptr)
                {
                listView->SetVirtualDataProvider(m_wordyPhraseData);
                listView->SetVirtualDataSize(m_wordyPhraseData->GetItemCount());
                listView->DistributeColumns();
                listView->Resort();
                }
            else
                {
                listView = new Wisteria::UI::ListCtrlEx(
                    view->GetSplitter(), BaseProjectView::WORDY_PHRASES_LIST_PAGE_ID,
                    wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
                listView->Hide();
                listView->SetLabel(BaseProjectView::GetWordyPhrasesTabLabel());
                listView->SetName(BaseProjectView::GetWordyPhrasesTabLabel());
                listView->EnableGridLines();
                listView->InsertColumn(0, BaseProjectView::GetWordyPhrasesTabLabel());
                listView->InsertColumn(1, _(L"Suggestion"));
                listView->InsertColumn(2, _(L"Sentence #"));
                listView->SetVirtualDataProvider(m_wordyPhraseData);
                listView->SetVirtualDataSize(m_wordyPhraseData->GetItemCount());
                listView->DistributeColumns();
                listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
                UpdateListOptions(listView);
                listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
                view->GetGrammarView().AddWindow(listView);
                }
            }
        else
            {
            resetListView(listView);
            // we are getting rid of this window (if it was included before)
            view->GetGrammarView().RemoveWindowById(BaseProjectView::WORDY_PHRASES_LIST_PAGE_ID);
            }
        }

        // Cliches
        {
        auto* listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
            view->GetGrammarView().FindWindowById(BaseProjectView::CLICHES_LIST_PAGE_ID));
        if (GetGrammarInfo().IsClichesEnabled() && (m_clichePhraseData->GetItemCount() != 0U))
            {
            if (listView != nullptr)
                {
                listView->SetVirtualDataProvider(m_clichePhraseData);
                listView->SetVirtualDataSize(m_clichePhraseData->GetItemCount());
                listView->DistributeColumns();
                listView->Resort();
                }
            else
                {
                listView = new Wisteria::UI::ListCtrlEx(
                    view->GetSplitter(), BaseProjectView::CLICHES_LIST_PAGE_ID, wxDefaultPosition,
                    wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
                listView->Hide();
                listView->SetLabel(BaseProjectView::GetClichesTabLabel());
                listView->SetName(BaseProjectView::GetClichesTabLabel());
                listView->EnableGridLines();
                listView->InsertColumn(0, BaseProjectView::GetClichesTabLabel());
                listView->InsertColumn(1, _(L"Explanation/Suggestion"));
                listView->InsertColumn(2, _(L"Sentence #"));
                listView->SetVirtualDataProvider(m_clichePhraseData);
                listView->SetVirtualDataSize(m_clichePhraseData->GetItemCount());
                listView->DistributeColumns();
                listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
                UpdateListOptions(listView);
                listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
                view->GetGrammarView().AddWindow(listView);
                }
            }
        else
            {
            resetListView(listView);
            // we are getting rid of this window (if it was included before)
            view->GetGrammarView().RemoveWindowById(BaseProjectView::CLICHES_LIST_PAGE_ID);
            }
        }

        // Passive voice
        {
        frequency_set<traits::case_insensitive_wstring_ex> passiveVoicePhrases;
        const auto& passiveVoiceIndices = GetWords()->get_passive_voice_indices();
        for (const auto& passiveVoiceIndex : passiveVoiceIndices)
            {
            traits::case_insensitive_wstring_ex currentPassivePhrase;
            for (size_t wordCounter = 0; wordCounter < passiveVoiceIndex.second; ++wordCounter)
                {
                currentPassivePhrase +=
                    (wordCounter == passiveVoiceIndex.second - 1) ?
                        traits::case_insensitive_wstring_ex(
                            GetWords()->get_word(passiveVoiceIndex.first + wordCounter)) :
                        traits::case_insensitive_wstring_ex(
                            GetWords()->get_word(passiveVoiceIndex.first + wordCounter) + L' ');
                }
            passiveVoicePhrases.insert(currentPassivePhrase);
            }
        m_passiveVoiceData->DeleteAllItems();
        m_passiveVoiceData->SetSize(passiveVoicePhrases.get_data().size(), 2);
        size_t uniquePassiveVoiceCount = 0; // NOLINT(misc-const-correctness)
        for (const auto& mIter : passiveVoicePhrases.get_data())
            {
            m_passiveVoiceData->SetItemText(
                uniquePassiveVoiceCount, 0, mIter.first.c_str(),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_passiveVoiceData->SetItemValue(uniquePassiveVoiceCount++, 1, mIter.second);
            }
        auto* listView = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
            view->GetGrammarView().FindWindowById(BaseProjectView::PASSIVE_VOICE_PAGE_ID));
        if (GetGrammarInfo().IsPassiveVoiceEnabled() && (m_passiveVoiceData->GetItemCount() != 0U))
            {
            if (listView != nullptr)
                {
                listView->SetVirtualDataProvider(m_passiveVoiceData);
                listView->SetVirtualDataSize(m_passiveVoiceData->GetItemCount());
                listView->DistributeColumns();
                listView->Resort();
                }
            else
                {
                listView = new Wisteria::UI::ListCtrlEx(
                    view->GetSplitter(), BaseProjectView::PASSIVE_VOICE_PAGE_ID, wxDefaultPosition,
                    wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
                listView->Hide();
                listView->SetLabel(BaseProjectView::GetPassiveLabel());
                listView->SetName(BaseProjectView::GetPassiveLabel());
                listView->EnableGridLines();
                listView->InsertColumn(0, BaseProjectView::GetPassiveLabel());
                listView->InsertColumn(1, _(L"Frequency"));
                listView->SetVirtualDataProvider(m_passiveVoiceData);
                listView->SetVirtualDataSize(m_passiveVoiceData->GetItemCount());
                listView->DistributeColumns();
                listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
                UpdateListOptions(listView);
                listView->SortColumn(0, Wisteria::SortDirection::SortAscending);
                view->GetGrammarView().AddWindow(listView);
                }
            }
        else
            {
            resetListView(listView);
            // we are getting rid of this window (if it was included before)
            view->GetGrammarView().RemoveWindowById(BaseProjectView::PASSIVE_VOICE_PAGE_ID);
            }
        }

        // sentences that begin with conjunctions
        {
        m_sentenceStartingWithConjunctionsData->DeleteAllItems();
        m_sentenceStartingWithConjunctionsData->SetSize(GetWords()->get_sentence_count(), 2);
        size_t sentenceStartingWithConjunctionsCount = 0;
        // reset punctuation marker
        auto punctPos = GetWords()->get_punctuation().cbegin();
        wxString currentSentence;
        for (auto pos = GetWords()->get_conjunction_beginning_sentences().begin();
             pos != GetWords()->get_conjunction_beginning_sentences().end(); ++pos)
            {
            currentSentence =
                ProjectReportFormat::FormatSentence(this, GetWords()->get_sentences()[*pos],
                                                    punctPos, GetWords()->get_punctuation().cend());

            m_sentenceStartingWithConjunctionsData->SetItemText(
                sentenceStartingWithConjunctionsCount, 0, currentSentence,
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_sentenceStartingWithConjunctionsData->SetItemValue(
                sentenceStartingWithConjunctionsCount++, 1,
                // add 1 to make it one-indexed
                (*pos) + 1,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
            }
        m_sentenceStartingWithConjunctionsData->SetSize(sentenceStartingWithConjunctionsCount);
        // display it
        auto* listView =
            dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetGrammarView().FindWindowById(
                BaseProjectView::SENTENCES_CONJUNCTION_START_LIST_PAGE_ID));
        if (GetGrammarInfo().IsConjunctionStartingSentencesEnabled() &&
            (m_sentenceStartingWithConjunctionsData->GetItemCount() != 0U))
            {
            if (listView != nullptr)
                {
                listView->SetVirtualDataProvider(m_sentenceStartingWithConjunctionsData);
                listView->SetVirtualDataSize(
                    m_sentenceStartingWithConjunctionsData->GetItemCount());
                listView->DistributeColumns();
                listView->Resort();
                }
            else
                {
                listView = new Wisteria::UI::ListCtrlEx(
                    view->GetSplitter(), BaseProjectView::SENTENCES_CONJUNCTION_START_LIST_PAGE_ID,
                    wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
                listView->Hide();
                listView->SetLabel(BaseProjectView::GetSentenceStartingWithConjunctionsTabLabel());
                listView->SetName(BaseProjectView::GetSentenceStartingWithConjunctionsTabLabel());
                listView->EnableGridLines();
                listView->InsertColumn(0, _(L"Sentence"));
                listView->InsertColumn(1, _(L"Sentence #"));
                listView->SetVirtualDataProvider(m_sentenceStartingWithConjunctionsData);
                listView->SetVirtualDataSize(
                    m_sentenceStartingWithConjunctionsData->GetItemCount());
                listView->DistributeColumns();
                listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
                UpdateListOptions(listView);
                listView->SortColumn(1, Wisteria::SortDirection::SortAscending);
                view->GetGrammarView().AddWindow(listView);
                }
            }
        else
            {
            resetListView(listView);
            // we are getting rid of this window (if it was included before)
            view->GetGrammarView().RemoveWindowById(
                BaseProjectView::SENTENCES_CONJUNCTION_START_LIST_PAGE_ID);
            }
        }

        // sentences that begin with lowercased words
        {
        m_sentenceStartingWithLowercaseData->DeleteAllItems();
        m_sentenceStartingWithLowercaseData->SetSize(GetWords()->get_sentence_count(), 2);
        size_t sentenceStartingWithLowercaseCount = 0;
        // reset punctuation marker
        auto punctPos = GetWords()->get_punctuation().cbegin();
        wxString currentSentence;
        for (auto pos = GetWords()->get_lowercase_beginning_sentences().begin();
             pos != GetWords()->get_lowercase_beginning_sentences().end(); ++pos)
            {
            currentSentence =
                ProjectReportFormat::FormatSentence(this, GetWords()->get_sentences()[*pos],
                                                    punctPos, GetWords()->get_punctuation().cend());

            m_sentenceStartingWithLowercaseData->SetItemText(
                sentenceStartingWithLowercaseCount, 0, currentSentence,
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_sentenceStartingWithLowercaseData->SetItemValue(
                sentenceStartingWithLowercaseCount++, 1,
                // add 1 to make it one-indexed
                (*pos) + 1,
                Wisteria::NumberFormatInfo(
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting, 0, true));
            }
        m_sentenceStartingWithLowercaseData->SetSize(sentenceStartingWithLowercaseCount);
        // display it
        auto* listView =
            dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetGrammarView().FindWindowById(
                BaseProjectView::SENTENCES_LOWERCASE_START_LIST_PAGE_ID));
        if (GetGrammarInfo().IsLowercaseSentencesEnabled() &&
            (m_sentenceStartingWithLowercaseData->GetItemCount() != 0U))
            {
            if (listView != nullptr)
                {
                listView->SetVirtualDataProvider(m_sentenceStartingWithLowercaseData);
                listView->SetVirtualDataSize(m_sentenceStartingWithLowercaseData->GetItemCount());
                listView->DistributeColumns();
                listView->Resort();
                }
            else
                {
                listView = new Wisteria::UI::ListCtrlEx(
                    view->GetSplitter(), BaseProjectView::SENTENCES_LOWERCASE_START_LIST_PAGE_ID,
                    wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
                listView->Hide();
                listView->SetLabel(BaseProjectView::GetSentenceStartingWithLowercaseTabLabel());
                listView->SetName(BaseProjectView::GetSentenceStartingWithLowercaseTabLabel());
                listView->EnableGridLines();
                listView->InsertColumn(0, _(L"Sentence"));
                listView->InsertColumn(1, _(L"Sentence #"));
                listView->SetVirtualDataProvider(m_sentenceStartingWithLowercaseData);
                listView->SetVirtualDataSize(m_sentenceStartingWithLowercaseData->GetItemCount());
                listView->DistributeColumns();
                listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
                UpdateListOptions(listView);
                listView->SortColumn(1, Wisteria::SortDirection::SortAscending);
                view->GetGrammarView().AddWindow(listView);
                }
            }
        else
            {
            resetListView(listView);
            // we are getting rid of this window (if it was included before)
            view->GetGrammarView().RemoveWindowById(
                BaseProjectView::SENTENCES_LOWERCASE_START_LIST_PAGE_ID);
            }
        }
    }

//-------------------------------------------------------
void ProjectDoc::DisplaySightWords()
    {
    auto* view = dynamic_cast<ProjectView*>(GetFirstView());
    wxASSERT_MSG(view, L"Error getting view when displaying sight words!");
    if (view == nullptr)
        {
        return;
        }

    const auto resetListView = [](Wisteria::UI::ListCtrlEx* listView)
    {
        if (listView != nullptr && listView->GetVirtualDataProvider() != nullptr &&
            listView->GetVirtualDataProvider()->GetItemCount() == 0)
            {
            listView->SetItemCount(0);
            }
    };

        // Dolch words
        {
        auto* listView =
            dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetDolchSightWordsView().FindWindowById(
                BaseProjectView::DOLCH_WORDS_LIST_PAGE_ID));
        if (IsIncludingDolchSightWords() && (GetDolchWordData()->GetItemCount() != 0U))
            {
            if (listView != nullptr)
                {
                listView->SetVirtualDataSize(GetDolchWordData()->GetItemCount());
                listView->DistributeColumns();
                listView->Resort();
                }
            else
                {
                listView = new Wisteria::UI::ListCtrlEx(
                    view->GetSplitter(), BaseProjectView::DOLCH_WORDS_LIST_PAGE_ID,
                    wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
                listView->Hide();
                listView->SetLabel(BaseProjectView::GetDolchWordTabLabel());
                listView->SetName(BaseProjectView::GetDolchWordTabLabel());
                listView->EnableGridLines();
                listView->InsertColumn(0, BaseProjectView::GetDolchWordTabLabel());
                listView->InsertColumn(1, _(L"Frequency"));
                listView->InsertColumn(2, _(L"Category"));
                listView->SetVirtualDataProvider(GetDolchWordData());
                listView->SetVirtualDataSize(GetDolchWordData()->GetItemCount());
                listView->DistributeColumns();
                listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
                UpdateListOptions(listView);
                // sort by category, frequency, then words
                listView->SortColumns(
                    { std::make_pair(2, Wisteria::SortDirection::SortAscending),
                      std::make_pair(1, Wisteria::SortDirection::SortDescending),
                      std::make_pair(0, Wisteria::SortDirection::SortAscending) });

                view->GetDolchSightWordsView().AddWindow(listView);
                }
            }
        else
            {
            resetListView(listView);
            view->GetDolchSightWordsView().RemoveWindowById(
                BaseProjectView::DOLCH_WORDS_LIST_PAGE_ID);
            }
        }

        // non-Dolch words
        {
        auto* listView =
            dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetDolchSightWordsView().FindWindowById(
                BaseProjectView::NON_DOLCH_WORDS_LIST_PAGE_ID));
        if (IsIncludingDolchSightWords() && (GetNonDolchWordData()->GetItemCount() != 0U))
            {
            if (listView != nullptr)
                {
                listView->SetVirtualDataSize(GetNonDolchWordData()->GetItemCount());
                listView->DistributeColumns();
                listView->Resort();
                }
            else
                {
                listView = new Wisteria::UI::ListCtrlEx(
                    view->GetSplitter(), BaseProjectView::NON_DOLCH_WORDS_LIST_PAGE_ID,
                    wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
                listView->Hide();
                listView->SetLabel(BaseProjectView::GetNonDolchWordTabLabel());
                listView->SetName(BaseProjectView::GetNonDolchWordTabLabel());
                listView->EnableGridLines();
                listView->InsertColumn(0, BaseProjectView::GetNonDolchWordTabLabel());
                listView->InsertColumn(1, _(L"Frequency"));
                listView->SetVirtualDataProvider(GetNonDolchWordData());
                listView->SetVirtualDataSize(GetNonDolchWordData()->GetItemCount());
                listView->DistributeColumns();
                listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
                UpdateListOptions(listView);
                // sort by frequency (low to high), then words (highest to lowest)
                listView->SortColumns(
                    { std::make_pair(1, Wisteria::SortDirection::SortDescending),
                      std::make_pair(0, Wisteria::SortDirection::SortAscending) });

                view->GetDolchSightWordsView().AddWindow(listView);
                }
            }
        else
            {
            resetListView(listView);
            view->GetDolchSightWordsView().RemoveWindowById(
                BaseProjectView::NON_DOLCH_WORDS_LIST_PAGE_ID);
            }
        }

        // unused Dolch words
        {
        auto* listView =
            dynamic_cast<Wisteria::UI::ListCtrlEx*>(view->GetDolchSightWordsView().FindWindowById(
                BaseProjectView::UNUSED_DOLCH_WORDS_LIST_PAGE_ID));
        if (IsIncludingDolchSightWords() && (GetUnusedDolchWordData()->GetItemCount() != 0U))
            {
            if (listView != nullptr)
                {
                listView->SetVirtualDataSize(GetUnusedDolchWordData()->GetItemCount());
                listView->DistributeColumns();
                listView->Resort();
                }
            else
                {
                listView = new Wisteria::UI::ListCtrlEx(
                    view->GetSplitter(), BaseProjectView::UNUSED_DOLCH_WORDS_LIST_PAGE_ID,
                    wxDefaultPosition, wxDefaultSize, wxLC_VIRTUAL | wxLC_REPORT | wxBORDER_SUNKEN);
                listView->Hide();
                listView->SetLabel(BaseProjectView::GetUnusedDolchWordTabLabel());
                listView->SetName(BaseProjectView::GetUnusedDolchWordTabLabel());
                listView->EnableGridLines();
                listView->InsertColumn(0, BaseProjectView::GetUnusedDolchWordTabLabel());
                listView->InsertColumn(1, _(L"Category"));
                listView->SetVirtualDataProvider(GetUnusedDolchWordData());
                listView->SetVirtualDataSize(GetUnusedDolchWordData()->GetItemCount());
                listView->DistributeColumns();
                listView->AssignContextMenu(wxXmlResource::Get()->LoadMenu(L"IDM_LIST_MENU"));
                UpdateListOptions(listView);
                // sort by category, then words (highest to lowest)
                listView->SortColumns(
                    { std::make_pair(1, Wisteria::SortDirection::SortAscending),
                      std::make_pair(0, Wisteria::SortDirection::SortAscending) });

                view->GetDolchSightWordsView().AddWindow(listView);
                }
            }
        else
            {
            resetListView(listView);
            view->GetDolchSightWordsView().RemoveWindowById(
                BaseProjectView::UNUSED_DOLCH_WORDS_LIST_PAGE_ID);
            }
        }
    }

//-------------------------------------------------------
void ProjectDoc::CalculateGraphData()
    {
    // syllable histogram
    m_syllableCounts->Clear();
    m_syllableCounts->AddContinuousColumn(GetSyllableCountsColumnName());
    // whether word is simple (1-2 syllables) or complex (3+)
    m_syllableCounts->AddCategoricalColumn(
        GetWordTypeGroupColumnName(),
        { { 0, _(L"Simple Words\n(1-2 syllables)") }, { 1, _(L"Complex Words\n(3+ syllables)") } });
    m_syllableCounts->Reserve(GetTotalWords());
    for (const auto& wordPos : GetWords()->get_words())
        {
        if (GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis ||
            GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings)
            {
            if (wordPos.is_valid())
                {
                if (wordPos.is_numeric() &&
                    GetNumeralSyllabicationMethod() == NumeralSyllabize::WholeWordIsOneSyllable)
                    {
                    m_syllableCounts->AddRow(
                        Wisteria::Data::RowInfo().Continuous({ 1 }).Categoricals({ 0 }));
                    }
                else
                    {
                    m_syllableCounts->AddRow(
                        Wisteria::Data::RowInfo()
                            .Continuous({ static_cast<double>(wordPos.get_syllable_count()) })
                            .
                        // simple or complex?
                        Categoricals({ static_cast<Wisteria::Data::GroupIdType>(
                            (wordPos.get_syllable_count()) < 3 ? 0 : 1) })
                            .
                        // add the word as a row ID so that it appears as a tooltip on the bin
                        Id(wordPos.c_str()));
                    }
                }
            }
        else
            {
            if (wordPos.is_numeric() &&
                GetNumeralSyllabicationMethod() == NumeralSyllabize::WholeWordIsOneSyllable)
                {
                m_syllableCounts->AddRow(
                    Wisteria::Data::RowInfo().Continuous({ 1 }).Categoricals({ 0 }));
                }
            else
                {
                m_syllableCounts->AddRow(
                    Wisteria::Data::RowInfo()
                        .Continuous({ static_cast<double>(wordPos.get_syllable_count()) })
                        .Categoricals({ static_cast<Wisteria::Data::GroupIdType>(
                            (wordPos.get_syllable_count()) < 3 ? 0 : 1) })
                        .Id(wordPos.c_str()));
                }
            }
        }
    // load the sentence word counts for the box plot/line chart
    m_sentenceWordLengths->Clear();
    m_sentenceWordLengths->AddContinuousColumn(GetSentenceWordCountsColumnName());
    m_sentenceWordLengths->AddContinuousColumn(GetSentenceIndicesColumnName());
    m_sentenceWordLengths->AddCategoricalColumn(GetGroupColumnName());
    m_sentenceWordLengths->Reserve(GetTotalSentences());
    if (GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis ||
        GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings)
        {
        for (auto paragraphIter = GetWords()->get_paragraphs().cbegin();
             paragraphIter != GetWords()->get_paragraphs().cend(); ++paragraphIter)
            {
            if (paragraphIter->is_valid())
                {
                for (size_t sentenceIndex = paragraphIter->get_first_sentence_index();
                     sentenceIndex <= paragraphIter->get_last_sentence_index(); ++sentenceIndex)
                    {
                    if (GetWords()->get_sentences()[sentenceIndex].is_valid())
                        {
                        m_sentenceWordLengths->AddRow(
                            Wisteria::Data::RowInfo()
                                .Continuous(
                                    { static_cast<double>(GetWords()
                                                              ->get_sentences()[sentenceIndex]
                                                              .get_valid_word_count()),
                                      static_cast<double>(sentenceIndex) })
                                .Categoricals(
                                    { static_cast<Wisteria::Data::GroupIdType>(std::distance(
                                        GetWords()->get_paragraphs().cbegin(), paragraphIter)) }));
                        }
                    }
                }
            }
        }
    else
        {
        for (auto paragraphIter = GetWords()->get_paragraphs().cbegin();
             paragraphIter != GetWords()->get_paragraphs().cend(); ++paragraphIter)
            {
            for (size_t sentenceIndex = paragraphIter->get_first_sentence_index();
                 sentenceIndex <= paragraphIter->get_last_sentence_index(); ++sentenceIndex)
                {
                m_sentenceWordLengths->AddRow(
                    Wisteria::Data::RowInfo()
                        .Continuous(
                            { static_cast<double>(
                                  GetWords()->get_sentences()[sentenceIndex].get_word_count()),
                              static_cast<double>(sentenceIndex) })
                        .Categoricals({ static_cast<Wisteria::Data::GroupIdType>(std::distance(
                            GetWords()->get_paragraphs().cbegin(), paragraphIter)) }));
                }
            }
        }

    // calculate the outliers (using the box plot method)
    auto wordCountColumn =
        m_sentenceWordLengths->GetContinuousColumn(GetSentenceWordCountsColumnName());
    auto sentenceIndicesColumn =
        m_sentenceWordLengths->GetContinuousColumn(GetSentenceIndicesColumnName());
    const statistics::find_outliers outlierInfo(wordCountColumn->GetValues());
    for (auto labelsPos = wordCountColumn->GetValues().cbegin();
         labelsPos != wordCountColumn->GetValues().cend(); ++labelsPos)
        {
        const size_t sentenceIndex =
            sentenceIndicesColumn->GetValue(labelsPos - wordCountColumn->GetValues().cbegin());
        if (sentenceIndex < GetWords()->get_sentences().size())
            {
#ifndef NDEBUG
            [[maybe_unused]]
            const auto debugWordCount =
                (GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences) ?
                    GetWords()->get_sentences()[sentenceIndex].get_word_count() :
                    GetWords()->get_sentences()[sentenceIndex].get_valid_word_count();
            wxASSERT_MSG(
                debugWordCount == *labelsPos,
                L"Sentence length does not match label position in DisplayOverlyLongSentences()!");
#endif
            if (*labelsPos > outlierInfo.get_upper_outlier_boundary() ||
                *labelsPos < outlierInfo.get_lower_outlier_boundary())
                {
                auto punctPos = GetWords()->get_punctuation().cbegin();
                wxString currentSentence =
                    wxString::Format(_(L"Sentence #%s (%s)"),
                                     wxNumberFormatter::ToString(
                                         sentenceIndex + 1 /*add 1 to make it one-indexed*/, 0,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep),
                                     ProjectReportFormat::FormatSentence(
                                         this, GetWords()->get_sentences()[sentenceIndex], punctPos,
                                         GetWords()->get_punctuation().cend()));
                if (currentSentence.length() >= 100)
                    {
                    currentSentence.Truncate(99).Append(wchar_t{ 8230 });
                    }
                m_sentenceWordLengths->GetIdColumn().SetValue(
                    labelsPos - wordCountColumn->GetValues().cbegin(), currentSentence.wc_str());
                }
            else
                {
                m_sentenceWordLengths->GetIdColumn().SetValue(
                    labelsPos - wordCountColumn->GetValues().cbegin(),
                    wxString::Format(_(L"Sentence #%s"),
                                     wxNumberFormatter::ToString(
                                         sentenceIndex + 1 /*add 1 to make it one-indexed*/, 0,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep))
                        .wc_str());
                }
            }
        else
            {
            wxFAIL_MSG(wxString::Format(
                L"Invalid sentence index (%i out of %i) when updating box plot labels!",
                sentenceIndex, GetWords()->get_sentences().size()));
            }
        }
    }
