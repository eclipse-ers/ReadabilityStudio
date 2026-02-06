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

#include "new_custom_test_simple_dlg.h"
#include "../../app/readability_app.h"
#include <wx/valgen.h>

wxDECLARE_APP(ReadabilityApp);

//-------------------------------------------------------------
void NewCustomWordTestSimpleDlg::OnOK([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();

    m_testName.Trim(true);
    m_testName.Trim(false);
    if (m_testName.empty())
        {
        wxMessageBox(_(L"Please select a valid test name."), _(L"Test Name"),
                     wxICON_EXCLAMATION | wxOK, this);
        return;
        }
    if (m_wordListFilePath.empty() || !wxFileName::FileExists(m_wordListFilePath))
        {
        wxMessageBox(_(L"Please select a valid file path."), _(L"Invalid File"),
                     wxICON_EXCLAMATION | wxOK, this);
        return;
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
void NewCustomWordTestSimpleDlg::OnBrowseForFileClick([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    wxFileDialog dialog(this, _(L"Select Word List File"),
                        !m_wordListFilePath.empty() ? wxString{} :
                                                      wxGetApp().GetAppOptions()->GetWordListPath(),
                        m_wordListFilePath, _(L"Text files (*.txt)|*.txt"),
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
    if (dialog.ShowModal() != wxID_OK)
        {
        return;
        }

    m_wordListFilePath = dialog.GetPath();
    wxGetApp().GetAppOptions()->SetWordListPath(wxFileName(m_wordListFilePath).GetPath());
    TransferDataToWindow();
    }

//-------------------------------------------------------------
void NewCustomWordTestSimpleDlg::CreateControls()
    {
    auto* mainSizer = new wxBoxSizer(wxVERTICAL);

    auto* testNameBoxSizer =
        new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _(L"Test name:")), wxVERTICAL);
    auto* fileBrowseBoxSizer = new wxStaticBoxSizer(
        new wxStaticBox(this, wxID_ANY, _(L"Custom familiar word list:")), wxHORIZONTAL);
    mainSizer->Add(testNameBoxSizer, wxSizerFlags{}.Expand().Border());
    mainSizer->Add(fileBrowseBoxSizer, wxSizerFlags{}.Expand().Border());

    auto* testNameEdit =
        new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, FromDIP(wxSize(400, 25)),
                       wxBORDER_THEME, wxGenericValidator(&m_testName));
    testNameBoxSizer->Add(testNameEdit, wxSizerFlags{ 1 }.Expand());

    auto* filePathEdit = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                                        wxSize(FromDIP(wxSize(400, 400)).GetWidth(), -1),
                                        wxBORDER_THEME, wxGenericValidator(&m_wordListFilePath));
    filePathEdit->AutoCompleteFileNames();
    fileBrowseBoxSizer->Add(filePathEdit, wxSizerFlags{ 1 }.Expand());

    auto* fileBrowseButton =
        new wxBitmapButton(this, ID_FOLDER_BROWSE_BUTTON,
                           wxArtProvider::GetBitmapBundle(wxART_FILE_OPEN, wxART_BUTTON));
    fileBrowseBoxSizer->Add(fileBrowseButton, 0, wxALIGN_CENTER_VERTICAL);

    mainSizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL | wxHELP),
                   wxSizerFlags{}.Expand().Border());

    SetSizerAndFit(mainSizer);

    testNameEdit->SetFocus();
    }
