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

#include "filtered_text_export_options_dlg.h"
#include <wx/valgen.h>

/// Creates the controls and sizers
//-------------------------------------------------------------
void FilteredTextExportOptionsDlg::CreateControls()
    {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticBoxSizer* itemBoxSizer = new wxStaticBoxSizer(
        new wxStaticBox(this, wxID_ANY, _(L"Along with text exclusion, also:")), wxVERTICAL);
    mainSizer->Add(itemBoxSizer, wxSizerFlags{}.Top().Border());

    itemBoxSizer->Add(new wxCheckBox(itemBoxSizer->GetStaticBox(), wxID_ANY,
                                     _(L"Romanize (replace special characters)"), wxDefaultPosition,
                                     wxDefaultSize, wxCHK_2STATE,
                                     wxGenericValidator(&m_replaceCharacters)),
                      wxSizerFlags{}.Expand().Border(wxBOTTOM));
    itemBoxSizer->Add(new wxCheckBox(itemBoxSizer->GetStaticBox(), wxID_ANY, _(L"Remove ellipses"),
                                     wxDefaultPosition, wxDefaultSize, wxCHK_2STATE,
                                     wxGenericValidator(&m_removeEllipses)),
                      wxSizerFlags{}.Expand().Border(wxBOTTOM));
    itemBoxSizer->Add(new wxCheckBox(itemBoxSizer->GetStaticBox(), wxID_ANY,
                                     // TRANSLATORS: bullet points in text.
                                     _(L"Remove bullets and list-item numbering"),
                                     wxDefaultPosition, wxDefaultSize, wxCHK_2STATE,
                                     wxGenericValidator(&m_removeBullets)),
                      wxSizerFlags{}.Expand().Border(wxBOTTOM));
    itemBoxSizer->Add(new wxCheckBox(itemBoxSizer->GetStaticBox(), wxID_ANY,
                                     _(L"Remove Internet and file addresses"), wxDefaultPosition,
                                     wxDefaultSize, wxCHK_2STATE,
                                     wxGenericValidator(&m_removeFilePaths)),
                      wxSizerFlags{}.Expand().Border(wxBOTTOM));
    itemBoxSizer->Add(new wxCheckBox(itemBoxSizer->GetStaticBox(), wxID_ANY,
                                     _(L"Remove trailing periods from abbreviations and acronyms"),
                                     wxDefaultPosition, wxDefaultSize, wxCHK_2STATE,
                                     wxGenericValidator(&m_stripAbbreviationPeriods)),
                      wxSizerFlags{}.Expand().Border(wxBOTTOM));
    itemBoxSizer->Add(new wxCheckBox(itemBoxSizer->GetStaticBox(), wxID_ANY,
                                     _(L"Narrow full-width characters"), wxDefaultPosition,
                                     wxDefaultSize, wxCHK_2STATE,
                                     wxGenericValidator(&m_narrowFullWidthCharacters)),
                      wxSizerFlags{}.Expand().Border(wxBOTTOM));

    // OK, Cancel, Help buttons
    mainSizer->Add(
        new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL),
        wxSizerFlags{ 1 }.Expand().Border(wxLEFT | wxRIGHT));
    wxSizer* OkCancelSizer = CreateButtonSizer(wxOK | wxCANCEL | wxHELP);
    wxStaticText* infoText = new wxStaticText(
        this, wxID_ANY,
        _(L"Note: these options will be used along with your project's text-exclusion settings."),
        wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
    infoText->Wrap(FromDIP(wxSize{ 200, 200 }.GetWidth()));
    OkCancelSizer->Insert(0, infoText, wxSizerFlags{}.Expand().Border(wxRIGHT));

    mainSizer->Add(OkCancelSizer, wxSizerFlags{}.Expand().Border());

    SetSizerAndFit(mainSizer);
    }
