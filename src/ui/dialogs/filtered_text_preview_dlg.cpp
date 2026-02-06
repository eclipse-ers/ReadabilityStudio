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

#include "filtered_text_preview_dlg.h"
#include <wx/valgen.h>

//-------------------------------------------------------------
void FilteredTextPreviewDlg::CreateControls()
    {
    auto* mainSizer = new wxBoxSizer(wxVERTICAL);

    auto* previewWindowSizer = new wxBoxSizer(wxVERTICAL);
    mainSizer->Add(previewWindowSizer, wxSizerFlags{ 1 }.Expand().Border());

    previewWindowSizer->Add(new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition,
                                           FromDIP(wxSize(500, 300)),
                                           wxTE_MULTILINE | wxTE_RICH2 | wxTE_READONLY,
                                           wxGenericValidator(&m_filteredValue)),
                            wxSizerFlags{ 1 }.Expand());

    previewWindowSizer->AddSpacer(wxSizerFlags::GetDefaultBorder() * 2);

    m_collPane = new wxCollapsiblePane(this, wxID_ANY, _(L"Details:"));
    previewWindowSizer->Add(m_collPane, wxSizerFlags{}.Expand());

    auto* filteredOutSizer = new wxBoxSizer(wxVERTICAL);
    filteredOutSizer->Add(new wxStaticText(m_collPane->GetPane(), wxID_STATIC,
                                           _(L"The following are being filtered:")));
    auto* infoLabel = new wxStaticText(m_collPane->GetPane(), wxID_STATIC, m_infoLabel);
    filteredOutSizer->Add(infoLabel, wxSizerFlags{}.Expand());
    m_collPane->GetPane()->SetSizer(filteredOutSizer);
    m_collPane->Expand();
    filteredOutSizer->SetSizeHints(m_collPane->GetPane());

    mainSizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL | wxHELP),
                   wxSizerFlags{}.Expand().Border());

    SetSizerAndFit(mainSizer);
    }

//-------------------------------------------------------------
void FilteredTextPreviewDlg::SetExclusionInfo(
    const InvalidSentence textExclusionMethod, const bool excludeCopyrightNotices,
    const bool excludeCitations, const bool replaceCharacters, const bool removeEllipses,
    const bool removeBullets, const bool removeFilePaths, const bool stripAbbreviationPeriods)
    {
    m_infoLabel.clear();
    if (textExclusionMethod != InvalidSentence::IncludeAsFullSentences)
        {
        if (textExclusionMethod == InvalidSentence::ExcludeFromAnalysis)
            {
            m_infoLabel += L"\n\t" + _(L"All incomplete sentences");
            }
        else if (textExclusionMethod == InvalidSentence::ExcludeExceptForHeadings)
            {
            m_infoLabel += L"\n\t" + _(L"All incomplete sentences, except headings");
            }
        if (excludeCopyrightNotices)
            {
            m_infoLabel += L"\n\t" + _(L"Trailing copyright notices");
            }
        if (excludeCitations)
            {
            m_infoLabel += L"\n\t" + _(L"Trailing citations");
            }
        }
    if (replaceCharacters)
        {
        // TRANSLATORS: "Special" as in extended ASCII (i.e., non-English).
        m_infoLabel += L"\n\t" + _(L"Special characters");
        }
    if (removeEllipses)
        {
        m_infoLabel += L"\n\t" + _(L"Ellipses");
        }
    if (removeBullets)
        {
        m_infoLabel += L"\n\t" + _(L"Bullets and list-item numbering");
        }
    if (removeFilePaths)
        {
        m_infoLabel += L"\n\t" + _(L"Internet and file addresses");
        }
    if (stripAbbreviationPeriods)
        {
        m_infoLabel += L"\n\t" + _(L"Trailing periods from abbreviations and acronyms");
        }
    }
