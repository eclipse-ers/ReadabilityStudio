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

#include "doc_group_select_dlg.h"
#include <wx/valgen.h>

//----------------------------------------------------------
void DocGroupSelectDlg::CreateControls()
    {
    auto* mainSizer = new wxBoxSizer(wxVERTICAL);

    const wxArrayString choices = {
        _(L"Use documents' descriptions (will be loaded during import)"),
        _(L"Use a grouping label"), _(L"Use the last common folder between files")
    };

    auto* radioBox = new wxRadioBox(this, wxID_ANY, _(L"Select how to label the documents:"),
                                    wxDefaultPosition, wxDefaultSize, choices, 0, wxRA_SPECIFY_ROWS,
                                    wxGenericValidator(&m_selected));

    mainSizer->Add(radioBox, wxSizerFlags{}.Border());

    // label box
    auto* labelSizer = new wxBoxSizer(wxHORIZONTAL);
    m_groupingLabelText = new wxStaticText(this, wxID_STATIC, _(L"Grouping label:"));
    m_groupingLabelEntry =
        new wxTextCtrl(this, wxID_ANY, wxString{}, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME,
                       wxGenericValidator(&m_groupingLabel));
    labelSizer->Add(m_groupingLabelText, wxSizerFlags{}.CenterVertical());
    labelSizer->Add(m_groupingLabelEntry, wxSizerFlags{ 1 }.Expand().Border());

    m_groupingLabelText->Enable(m_selected == 1);
    m_groupingLabelEntry->Enable(m_selected == 1);

    mainSizer->Add(labelSizer, wxSizerFlags{ 1 }.Expand().Border());

    mainSizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL | wxHELP),
                   wxSizerFlags{}.Expand().Border());

    SetSizerAndFit(mainSizer);

    Bind(wxEVT_RADIOBOX, &DocGroupSelectDlg::OnRadioBoxChange, this);
    Bind(wxEVT_BUTTON, &DocGroupSelectDlg::OnOK, this, wxID_OK);
    }

//----------------------------------------------------------
void DocGroupSelectDlg::OnRadioBoxChange([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    m_groupingLabelText->Enable(m_selected == 1);
    m_groupingLabelEntry->Enable(m_selected == 1);
    }

//-------------------------------------------------------------
void DocGroupSelectDlg::OnOK([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();

    // trim off whitespace off of label
    m_groupingLabel.Trim(true).Trim(false);

    // validate the label
    if (m_groupingLabel.empty() && GetSelection() == 1)
        {
        wxMessageBox(_(L"Please enter a grouping label."), _(L"Error"), wxOK | wxICON_EXCLAMATION);
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
