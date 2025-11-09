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

#include "export_all_dlg.h"
#include "../../app/readability_app.h"
#include "../../projects/batch_project_doc.h"
#include "../../projects/batch_project_view.h"
#include "../../projects/standard_project_doc.h"
#include "../../projects/standard_project_view.h"

wxDECLARE_APP(ReadabilityApp);

ExportAllDlg::ExportAllDlg(wxWindow* parent, BaseProjectDoc* doc, const bool fileMode,
                           wxWindowID id /*= wxID_ANY*/,
                           const wxString& caption /*= _(L"Export Options")*/,
                           const wxPoint& pos /*= wxDefaultPosition*/,
                           const wxSize& size /*= wxDefaultSize*/,
                           long style /*= wxDEFAULT_DIALOG_STYLE|wxCLIP_CHILDREN*/)
    : m_readabilityProjectDoc(doc), m_fileMode(fileMode)
    {
    wxASSERT_MSG(doc, L"NULL document passed to export all dialog!");
    wxString exportDir = (doc != nullptr) ? doc->GetExportFolder() : wxString{};
    if (exportDir.empty())
        {
        // if export path not specified yet then try the path of the project (if saved already)
        const wxFileName fn((doc != nullptr) ? doc->GetFilename() : wxString{});
        if (!fn.GetPath().empty())
            {
            exportDir = fn.GetPath();
            }
        // then try the documents folder
        else
            {
            exportDir = wxStandardPaths::Get().GetDocumentsDir();
            }
        }
    m_folderPath = exportDir;

    wxString exportFile = (doc != nullptr) ? doc->GetExportFile() : wxString{};
    if (exportFile.empty())
        {
        // if export path not specified yet, then try the path of the project (if saved already)
        const wxFileName fn(doc->GetFilename());
        if (!fn.GetPath().empty())
            {
            exportFile = fn.GetPathWithSep() + doc->GetTitle() + L".htm";
            }
        }
    m_filePath = exportFile;

    m_listExt = BaseProjectDoc::GetExportListExt();
    m_textViewExt = BaseProjectDoc::GetExportTextViewExt();
    m_graphExt = BaseProjectDoc::GetExportGraphExt();
    m_exportHardWordLists = BaseProjectDoc::IsExportingHardWordLists();
    m_exportSentencesBreakdown = BaseProjectDoc::IsExportingSentencesBreakdown();
    m_exportGraphs = BaseProjectDoc::IsExportingGraphs();
    m_exportTestResults = BaseProjectDoc::IsExportingTestResults();
    m_exportStatistics = BaseProjectDoc::IsExportingStatistics();
    m_exportWordiness = BaseProjectDoc::IsExportingWordiness();
    m_exportSightWords = BaseProjectDoc::IsExportingSightWords();
    m_exportWarnings = BaseProjectDoc::IsExportingWarnings();
    m_exportingLists = BaseProjectDoc::IsExportingLists();
    m_exportingTextReports = BaseProjectDoc::IsExportingTextReports();
    m_imageExportOptions.m_imageSize = BaseProjectDoc::GetImageExportOptions().m_imageSize;
    Create(parent, id, caption, pos, size, style);

    Bind(wxEVT_BUTTON, &ExportAllDlg::OnFolderBrowseButtonClick, this, ID_FOLDER_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &ExportAllDlg::OnImageOptionsButtonClick, this, ID_IMAGE_OPTIONS_BUTTON);
    Bind(wxEVT_BUTTON, &ExportAllDlg::OnOK, this, wxID_OK);
    Bind(wxEVT_CHECKBOX, &ExportAllDlg::OnIncludeListsTextWindowsCheck, this,
         ID_INCLUDE_LIST_CHECKBOX);
    Bind(wxEVT_CHECKBOX, &ExportAllDlg::OnIncludeListsTextWindowsCheck, this,
         ID_INCLUDE_TEXT_CHECKBOX);
    }

//---------------------------------------------
void ExportAllDlg::OnOK([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    if (m_folderPath.empty())
        {
        wxMessageBox(_(L"Export folder not specified."), _(L"Error"), wxOK | wxICON_EXCLAMATION);
        return;
        }
    if (IsStandardProject() && !m_exportHardWordLists && !m_exportSentencesBreakdown &&
        !m_exportGraphs && !m_exportTestResults && !m_exportStatistics && !m_exportWordiness &&
        !m_exportSightWords)
        {
        wxMessageBox(_(L"You must select at least one item to be saved."), _(L"Error"),
                     wxOK | wxICON_EXCLAMATION);
        return;
        }
    if (IsBatchProject() && !m_exportHardWordLists && !m_exportSentencesBreakdown &&
        !m_exportGraphs && !m_exportTestResults && !m_exportStatistics && !m_exportWordiness &&
        !m_exportSightWords && !m_exportWarnings)
        {
        wxMessageBox(_(L"You must select at least one item to be saved."), _(L"Error"),
                     wxOK | wxICON_EXCLAMATION);
        return;
        }
    if (m_textViewCombo != nullptr)
        {
        m_textViewExt = m_textViewCombo->GetValue();
        }
    if (m_listCombo != nullptr)
        {
        m_listExt = m_listCombo->GetValue();
        }
    if (m_graphCombo != nullptr)
        {
        m_graphExt = m_graphCombo->GetValue();
        }

    if (IsModal())
        {
        EndModal(wxID_OK);
        }
    else
        {
        Show(false);
        }
    }

//-------------------------------------------------------------
void ExportAllDlg::OnIncludeListsTextWindowsCheck([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();

    auto* exportControl = FindWindow(ID_LIST_TYPE_LABEL);
    if (exportControl != nullptr)
        {
        exportControl->Enable(m_exportingLists);
        }
    exportControl = FindWindow(ID_LIST_TYPE_COMBO);
    if (exportControl != nullptr)
        {
        exportControl->Enable(m_exportingLists);
        }
    exportControl = FindWindow(ID_TEXT_TYPE_LABEL);
    if (exportControl != nullptr)
        {
        exportControl->Enable(m_exportingTextReports);
        }
    exportControl = FindWindow(ID_TEXT_TYPE_COMBO);
    if (exportControl != nullptr)
        {
        exportControl->Enable(m_exportingTextReports);
        }

    TransferDataToWindow();
    }

//-------------------------------------------------------------
void ExportAllDlg::OnImageOptionsButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    wxASSERT_MSG(m_graphCombo, L"Graph file combobox not initialized!");
    wxString ext{ ((m_graphCombo != nullptr) ? m_graphCombo->GetValue() : wxString{ L"png" }) };
    Wisteria::UI::ImageExportDlg optDlg(
        this, Wisteria::GraphItems::Image::GetImageFileTypeFromExtension(ext), wxNullBitmap,
        m_imageExportOptions);
    optDlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(), L"online/publishing.html");
    if (optDlg.ShowModal() == wxID_OK)
        {
        m_imageExportOptions = optDlg.GetOptions();
        }
    }

//-------------------------------------------------------------
void ExportAllDlg::OnFolderBrowseButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();

    if (m_fileMode)
        {
        wxFileDialog fdialog(
            this, _(L"Save As"), wxString{},
            !m_filePath.empty() ? m_filePath : m_readabilityProjectDoc->GetTitle() + L".htm",
            _(L"HTML Files (*.htm;*.html)|*.htm;*.html"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        if (fdialog.ShowModal() != wxID_OK)
            {
            return;
            }
        m_filePath = fdialog.GetPath();
        }
    else
        {
        wxDirDialog dirDlg(this, _(L"Choose a directory"), m_folderPath);
        if (dirDlg.ShowModal() != wxID_OK)
            {
            return;
            }
        m_folderPath = dirDlg.GetPath();
        }
    TransferDataToWindow();
    SetFocus();
    }

//-------------------------------------------------------------
void ExportAllDlg::CreateControls()
    {
    auto* mainSizer = new wxBoxSizer(wxVERTICAL);
    SetSizer(mainSizer);

    auto* itemsBoxSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(itemsBoxSizer, wxSizerFlags{}.Expand().Border().Top());

    // export path
    auto* pathBox = new wxStaticBox(
        this, wxID_ANY, m_fileMode ? _(L"File to export to:") : _(L"Folder to export to:"));
    auto* pathBoxBoxSizer = new wxStaticBoxSizer(pathBox, wxVERTICAL);
    itemsBoxSizer->Add(pathBoxBoxSizer, wxSizerFlags{ 1 }.Expand());

    auto* folderBrowseBoxSizer = new wxBoxSizer(wxHORIZONTAL);
    pathBoxBoxSizer->Add(folderBrowseBoxSizer, wxSizerFlags{}.Expand().Border());

    auto* folderPathEdit = new wxTextCtrl(pathBoxBoxSizer->GetStaticBox(), wxID_ANY, wxString{},
                                          wxDefaultPosition, wxDefaultSize, wxBORDER_THEME,
                                          m_fileMode ? wxGenericValidator(&m_filePath) :
                                                       wxGenericValidator(&m_folderPath));
    folderBrowseBoxSizer->Add(folderPathEdit, wxSizerFlags{ 1 }.Expand());

    m_folderBrowseButton =
        new wxBitmapButton(pathBoxBoxSizer->GetStaticBox(), ID_FOLDER_BROWSE_BUTTON,
                           wxArtProvider::GetBitmapBundle(wxART_FILE_OPEN, wxART_BUTTON));
    folderBrowseBoxSizer->Add(m_folderBrowseButton);

    // inclusion options
    auto* inclusionSectionBox = new wxStaticBox(this, wxID_ANY, _(L"Sections to export:"));
    auto* inclusionSectionBoxSizer = new wxStaticBoxSizer(inclusionSectionBox, wxVERTICAL);
    itemsBoxSizer->Add(inclusionSectionBoxSizer);
    if (IsStandardProject())
        {
        const auto* view =
            dynamic_cast<const ProjectView*>(m_readabilityProjectDoc->GetFirstView());

        auto* testResultsCheck = new wxCheckBox(inclusionSectionBoxSizer->GetStaticBox(), wxID_ANY,
                                                _(L"Test scores"), wxDefaultPosition, wxDefaultSize,
                                                0, wxGenericValidator(&m_exportTestResults));
        inclusionSectionBoxSizer->Add(testResultsCheck, wxSizerFlags{}.Expand().Border());

        auto* statisticsCheck = new wxCheckBox(
            inclusionSectionBoxSizer->GetStaticBox(), wxID_ANY, _(L"Summary statistics"),
            wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_exportStatistics));
        inclusionSectionBoxSizer->Add(statisticsCheck, wxSizerFlags{}.Expand().Border());

        if (view->GetWordsBreakdownView().GetWindowCount() > 0)
            {
            auto* listCheck = new wxCheckBox(
                inclusionSectionBoxSizer->GetStaticBox(), wxID_ANY, _(L"Words breakdown"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_exportHardWordLists));
            inclusionSectionBoxSizer->Add(listCheck, wxSizerFlags{}.Expand().Border());
            }

        if (view->GetSentencesBreakdownView().GetWindowCount() > 0)
            {
            auto* listCheck =
                new wxCheckBox(inclusionSectionBoxSizer->GetStaticBox(), wxID_ANY,
                               _(L"Sentences breakdown"), wxDefaultPosition, wxDefaultSize, 0,
                               wxGenericValidator(&m_exportSentencesBreakdown));
            inclusionSectionBoxSizer->Add(listCheck, wxSizerFlags{}.Expand().Border());
            }

        if (view->GetGrammarView().GetWindowCount() > 0)
            {
            auto* wordinessCheck = new wxCheckBox(
                inclusionSectionBoxSizer->GetStaticBox(), wxID_ANY, _(L"Grammar section"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_exportWordiness));
            inclusionSectionBoxSizer->Add(wordinessCheck, wxSizerFlags{}.Expand().Border());
            }

        if (view->GetDolchSightWordsView().GetWindowCount() > 0)
            {
            auto* sightWordsCheck = new wxCheckBox(
                inclusionSectionBoxSizer->GetStaticBox(), wxID_ANY, _(L"Dolch sight words section"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_exportSightWords));
            inclusionSectionBoxSizer->Add(sightWordsCheck, wxSizerFlags{}.Expand().Border());
            }
        }
    else
        {
        const auto* view =
            dynamic_cast<const BatchProjectView*>(m_readabilityProjectDoc->GetFirstView());

        auto* testResultsCheck = new wxCheckBox(inclusionSectionBoxSizer->GetStaticBox(), wxID_ANY,
                                                _(L"Test scores"), wxDefaultPosition, wxDefaultSize,
                                                0, wxGenericValidator(&m_exportTestResults));
        inclusionSectionBoxSizer->Add(testResultsCheck, wxSizerFlags{}.Expand().Border());

        auto* statisticsCheck = new wxCheckBox(
            inclusionSectionBoxSizer->GetStaticBox(), wxID_ANY, _(L"Summary statistics"),
            wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_exportStatistics));
        inclusionSectionBoxSizer->Add(statisticsCheck, wxSizerFlags{}.Expand().Border());

        auto* graphCheck = new wxCheckBox(inclusionSectionBoxSizer->GetStaticBox(), wxID_ANY,
                                          _(L"Histograms/box plots"), wxDefaultPosition,
                                          wxDefaultSize, 0, wxGenericValidator(&m_exportGraphs));
        inclusionSectionBoxSizer->Add(graphCheck, wxSizerFlags{}.Expand().Border());

        if (view->GetWordsBreakdownView().GetWindowCount() > 0)
            {
            auto* listCheck = new wxCheckBox(
                inclusionSectionBoxSizer->GetStaticBox(), wxID_ANY, _(L"Words breakdown"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_exportHardWordLists));
            inclusionSectionBoxSizer->Add(listCheck, wxSizerFlags{}.Expand().Border());
            }

        if (view->GetSentencesBreakdownView().GetWindowCount() > 0)
            {
            auto* listCheck =
                new wxCheckBox(inclusionSectionBoxSizer->GetStaticBox(), wxID_ANY,
                               _(L"Sentences breakdown"), wxDefaultPosition, wxDefaultSize, 0,
                               wxGenericValidator(&m_exportSentencesBreakdown));
            inclusionSectionBoxSizer->Add(listCheck, wxSizerFlags{}.Expand().Border());
            }

        if (view->GetGrammarView().GetWindowCount() > 0)
            {
            auto* grammarCheck = new wxCheckBox(
                inclusionSectionBoxSizer->GetStaticBox(), wxID_ANY, _(L"Grammar section"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_exportWordiness));
            inclusionSectionBoxSizer->Add(grammarCheck, wxSizerFlags{}.Expand().Border());
            }

        if (view->GetDolchSightWordsView().GetWindowCount() > 0)
            {
            auto* sightWordsCheck = new wxCheckBox(
                inclusionSectionBoxSizer->GetStaticBox(), wxID_ANY, _(L"Dolch sight words section"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_exportSightWords));
            inclusionSectionBoxSizer->Add(sightWordsCheck, wxSizerFlags{}.Expand().Border());
            }

        if (view->GetWarningsView()->GetItemCount() > 0)
            {
            auto* warningsCheck = new wxCheckBox(
                inclusionSectionBoxSizer->GetStaticBox(), wxID_ANY, _(L"Warnings section"),
                wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_exportWarnings));
            inclusionSectionBoxSizer->Add(warningsCheck, wxSizerFlags{}.Expand().Border());
            }
        }

    // the extension values
    auto* exportTypeBox = new wxStaticBox(this, wxID_ANY, _(L"Export file types:"));
    auto* exportTypeStaticBoxSizer = new wxStaticBoxSizer(exportTypeBox, wxVERTICAL);
    itemsBoxSizer->Add(exportTypeStaticBoxSizer);

    auto* exportTypeBoxSizer = new wxFlexGridSizer(3, 5, 5);
    exportTypeStaticBoxSizer->Add(exportTypeBoxSizer);

    if (!m_fileMode)
        {
        auto* listLabel =
            new wxStaticText(exportTypeStaticBoxSizer->GetStaticBox(), ID_LIST_TYPE_LABEL,
                             _(L"Export lists as:"), wxDefaultPosition, wxDefaultSize, 0);
        listLabel->Enable(m_exportingLists);
        exportTypeBoxSizer->Add(listLabel, wxSizerFlags{}.Border().CenterVertical());

        m_listCombo = new wxComboBox(exportTypeStaticBoxSizer->GetStaticBox(), ID_LIST_TYPE_COMBO,
                                     wxString{}, wxDefaultPosition, wxDefaultSize, 0, nullptr,
                                     wxCB_DROPDOWN | wxCB_READONLY);
        m_listCombo->Append(L"htm");
        m_listCombo->Append(L"txt");
        m_listCombo->Append(L"tex");
        m_listCombo->SetStringSelection(m_listExt);
        m_listCombo->Enable(m_exportingLists);
        exportTypeBoxSizer->Add(m_listCombo, wxSizerFlags{}.Border().Left());
        exportTypeBoxSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

        // text view section
        if (IsStandardProject())
            {
            auto* textViewLabel = new wxStaticText(
                exportTypeStaticBoxSizer->GetStaticBox(), ID_TEXT_TYPE_LABEL,
                _(L"Export text reports as:"), wxDefaultPosition, wxDefaultSize, 0);
            textViewLabel->Enable(m_exportingTextReports);
            exportTypeBoxSizer->Add(textViewLabel, wxSizerFlags{}.Border().CenterVertical());

            m_textViewCombo = new wxComboBox(
                exportTypeStaticBoxSizer->GetStaticBox(), ID_TEXT_TYPE_COMBO, wxString{},
                wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_DROPDOWN | wxCB_READONLY);
            m_textViewCombo->Append(L"htm");
            m_textViewCombo->Append(L"rtf");
            m_textViewCombo->SetStringSelection(m_textViewExt);
            m_textViewCombo->Enable(m_exportingTextReports);
            exportTypeBoxSizer->Add(m_textViewCombo, wxSizerFlags{}.Border().Left());
            exportTypeBoxSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            }
        }

    // graph view section
    auto* graphLabel =
        new wxStaticText(exportTypeStaticBoxSizer->GetStaticBox(), wxID_STATIC,
                         _(L"Export graphs as:"), wxDefaultPosition, wxDefaultSize, 0);
    exportTypeBoxSizer->Add(graphLabel, wxSizerFlags{}.Border().CenterVertical());

    m_graphCombo =
        new wxComboBox(exportTypeStaticBoxSizer->GetStaticBox(), wxID_ANY, wxString{},
                       wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_DROPDOWN | wxCB_READONLY);
    m_graphCombo->Append(L"png");
    m_graphCombo->Append(L"jpg");
    m_graphCombo->Append(L"bmp");
    m_graphCombo->Append(L"tif");
    m_graphCombo->Append(L"tga");
    m_graphCombo->Append(L"gif");
    m_graphCombo->Append(L"svg");
    m_graphCombo->SetStringSelection(m_graphExt);
    exportTypeBoxSizer->Add(m_graphCombo, wxSizerFlags{}.Border().CenterVertical());

    auto* imageButton = new wxButton(exportTypeStaticBoxSizer->GetStaticBox(),
                                     ID_IMAGE_OPTIONS_BUTTON, _(L"Image options"));
    imageButton->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/boxplot.svg"));
    exportTypeBoxSizer->Add(imageButton, wxSizerFlags{}.Border().CenterVertical());

    if (IsStandardProject())
        {
        mainSizer->Add(new wxCheckBox(this, ID_INCLUDE_LIST_CHECKBOX, _(L"Include lists"),
                                      wxDefaultPosition, wxDefaultSize, 0,
                                      wxGenericValidator(&m_exportingLists)),
                       wxSizerFlags{}.Expand().Border());

        mainSizer->Add(new wxCheckBox(this, ID_INCLUDE_TEXT_CHECKBOX, _(L"Include text reports"),
                                      wxDefaultPosition, wxDefaultSize, 0,
                                      wxGenericValidator(&m_exportingTextReports)),
                       wxSizerFlags{}.Expand().Border());
        }

    mainSizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL | wxHELP),
                   wxSizerFlags{}.Expand().Border());
    }

//---------------------------------------------
bool ExportAllDlg::IsStandardProject() const
    {
    return ((m_readabilityProjectDoc != nullptr) &&
            m_readabilityProjectDoc->IsKindOf(CLASSINFO(ProjectDoc)));
    }

//---------------------------------------------
bool ExportAllDlg::IsBatchProject() const
    {
    return ((m_readabilityProjectDoc != nullptr) &&
            m_readabilityProjectDoc->IsKindOf(CLASSINFO(BatchProjectDoc)));
    }
