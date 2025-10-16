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

#include "explanation_listctrl.h"
#include "../../Wisteria-Dataviz/src/ui/controls/htmltablewinprintout.h"

wxIMPLEMENT_DYNAMIC_CLASS(ExplanationListCtrl, wxSplitterWindow)

    ExplanationListExportOptions ExplanationListCtrl::m_lastCopyOption =
        ExplanationListExportOptions::ExportGrid;
ExplanationListExportOptions ExplanationListCtrl::m_lastSaveOption =
    ExplanationListExportOptions::ExportBoth;

//------------------------------------------------------
ExplanationListCtrl::ExplanationListCtrl(wxWindow* parent, wxWindowID id,
                                         const wxPoint& point /*= wxDefaultPosition*/,
                                         const wxSize& size /*= wxDefaultSize*/,
                                         const wxString& name /*= wxString{}*/)
    : wxSplitterWindow(parent, id, point, size, wxCLIP_CHILDREN | wxSP_NOBORDER, name)
    {
    m_results_view = new Wisteria::UI::ListCtrlEx(
        this, id, wxDefaultPosition, wxDefaultSize,
        wxLC_SINGLE_SEL | wxLC_REPORT | wxLC_VIRTUAL | wxBORDER_SUNKEN, wxDefaultValidator);
    GetResultsListCtrl()->SetVirtualDataProvider(m_data);
    GetResultsListCtrl()->SetVirtualDataSize(0);
    GetResultsListCtrl()->EnableGridLines();
    GetResultsListCtrl()->EnableAlternateRowColours(false);
    m_explanation_view = new Wisteria::UI::HtmlTableWindow(this);
    wxSplitterWindow::SplitHorizontally(GetResultsListCtrl(), GetExplanationView());
    SetMinimumPaneSize(100 * wxWindow::GetDPIScaleFactor());

    Bind(wxEVT_MENU, &ExplanationListCtrl::OnPreview, this, wxID_PREVIEW);
    Bind(wxEVT_MENU, &ExplanationListCtrl::OnCopy, this, wxID_COPY);
    Bind(wxEVT_MENU, &ExplanationListCtrl::OnSave, this, wxID_SAVE);
    Bind(wxEVT_MENU, &ExplanationListCtrl::OnPrint, this, wxID_PRINT);
    Bind(wxEVT_MENU, &ExplanationListCtrl::OnMenuCommand, this, XRCID("ID_VIEW_ITEM"));
    Bind(wxEVT_MENU, &ExplanationListCtrl::OnMenuCommand, this, XRCID("ID_LIST_SORT"));

    Bind(wxEVT_FIND, &ExplanationListCtrl::OnFind, this);
    Bind(wxEVT_FIND_NEXT, &ExplanationListCtrl::OnFind, this);
    Bind(wxEVT_FIND_CLOSE, &ExplanationListCtrl::OnFind, this);

    Bind(wxEVT_SIZE, &ExplanationListCtrl::OnResize, this);
    Bind(wxEVT_LIST_ITEM_SELECTED, &ExplanationListCtrl::OnItemSelected, this, id);
    }

//------------------------------------------------------
void ExplanationListCtrl::OnMenuCommand(wxCommandEvent& event)
    {
    const ParentEventBlocker blocker(GetResultsListCtrl());
    GetResultsListCtrl()->ProcessWindowEvent(event);
    }

//------------------------------------------------------
void ExplanationListCtrl::OnPreview([[maybe_unused]] wxCommandEvent& event) { PrintPreview(); }

//------------------------------------------------------
void ExplanationListCtrl::OnPrint([[maybe_unused]] wxCommandEvent& event) { Print(); }

//------------------------------------------------------
void ExplanationListCtrl::PrintPreview()
    {
#if defined(__WXMSW__)
    if (m_printData)
        {
        GetResultsListCtrl()->SetPrinterSettings(m_printData);
        }
    GetResultsListCtrl()->SetLabel(GetLabel());
    GetResultsListCtrl()->SetLeftPrinterHeader(GetLeftPrinterHeader());
    GetResultsListCtrl()->SetCenterPrinterHeader(GetCenterPrinterHeader());
    GetResultsListCtrl()->SetRightPrinterHeader(GetRightPrinterHeader());
    GetResultsListCtrl()->SetLeftPrinterFooter(GetLeftPrinterFooter());
    GetResultsListCtrl()->SetCenterPrinterFooter(GetCenterPrinterFooter());
    GetResultsListCtrl()->SetRightPrinterFooter(GetRightPrinterFooter());
    GetResultsListCtrl()->SetWatermark(GetWatermark());

    wxCommandEvent gridPreviewEvent(wxEVT_MENU, wxID_PREVIEW);
    GetResultsListCtrl()->ProcessWindowEvent(gridPreviewEvent);
#else
    wxFAIL_MSG(L"Print preview is Windows only!");
#endif
    }

//------------------------------------------------------
void ExplanationListCtrl::Print()
    {
    if (m_printData != nullptr)
        {
        GetResultsListCtrl()->SetPrinterSettings(m_printData);
        }
    GetResultsListCtrl()->SetLabel(GetLabel());
    GetResultsListCtrl()->SetLeftPrinterHeader(GetLeftPrinterHeader());
    GetResultsListCtrl()->SetCenterPrinterHeader(GetCenterPrinterHeader());
    GetResultsListCtrl()->SetRightPrinterHeader(GetRightPrinterHeader());
    GetResultsListCtrl()->SetLeftPrinterFooter(GetLeftPrinterFooter());
    GetResultsListCtrl()->SetCenterPrinterFooter(GetCenterPrinterFooter());
    GetResultsListCtrl()->SetRightPrinterFooter(GetRightPrinterFooter());
    GetResultsListCtrl()->SetWatermark(GetWatermark());

    wxCommandEvent gridPreviewEvent(wxEVT_MENU, wxID_PRINT);
    GetResultsListCtrl()->ProcessWindowEvent(gridPreviewEvent);
    }

//------------------------------------------------------
bool ExplanationListCtrl::Save(
    const wxFileName& filePath,
    const ExplanationListExportOptions exportOptions /*= SaveBoth*/) const
    {
    // create the folder to the filepath, if necessary
    wxFileName::Mkdir(filePath.GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

    std::wstring resultsHtml, descriptionHtml;
    if (exportOptions == ExplanationListExportOptions::ExportGrid ||
        exportOptions == ExplanationListExportOptions::ExportBoth)
        {
        wxString buffer;
        GetResultsListCtrl()->FormatToHtml(buffer, false);
        resultsHtml = buffer;
        }
    if (exportOptions == ExplanationListExportOptions::ExportExplanations ||
        exportOptions == ExplanationListExportOptions::ExportBoth)
        {
        for (long i = 0; i < GetResultsListCtrl()->GetItemCount(); ++i)
            {
            auto pos = m_explanations.find(GetResultsListCtrl()->GetItemTextEx(i, 0));
            if (pos != m_explanations.end())
                {
                descriptionHtml += pos->second + L"<br />\n";
                }
            }
        }

    resultsHtml.insert(
        0, wxString::Format(
               L"<!DOCTYPE html>\n<html>\n<head>"
               "\n    <meta http-equiv='content-type' content='text/html; charset=UTF-8' />"
               "\n    <title>%s</title>"
               "\n</head>\n<body>\n",
               GetLabel()));
    resultsHtml += L"\n<br />\n" + descriptionHtml + L"\n</body>\n</html>";

    lily_of_the_valley::html_format::strip_hyperlinks(resultsHtml);

    wxFileName(filePath.GetFullPath()).SetPermissions(wxS_DEFAULT);
    wxFile file(filePath.GetFullPath(), wxFile::write);
    if (!file.Write(resultsHtml))
        {
        wxMessageBox(wxString::Format(_(L"Failed to save document\n(%s)."), filePath.GetFullPath()),
                     _(L"Error"), wxOK | wxICON_EXCLAMATION);
        return false;
        }
    return true;
    }

//------------------------------------------------------
void ExplanationListCtrl::OnSave([[maybe_unused]] wxCommandEvent& event)
    {
    wxFileDialog dialog(this, _(L"Save As"), wxString{}, GetLabel(),
                        _DT(L"HTML (*.htm;*.html)|*.htm;*.html"),
                        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (dialog.ShowModal() != wxID_OK)
        {
        return;
        }

    wxFileName filePath = dialog.GetPath();
    // in case the extension is missing then use the selected filter
    if (filePath.GetExt().IsEmpty())
        {
        filePath.SetExt(L"htm");
        }

    wxArrayString choices, descriptions;
    choices.Add(_(L"Grid"));
    choices.Add(_(L"Explanations"));
    choices.Add(_(L"Both"));
    descriptions.Add(_(L"Save the grid."));
    descriptions.Add(
        _(L"Save a report of the explanations associated with the items in the grid."));
    descriptions.Add(_(L"Save the grid and explanations."));
    Wisteria::UI::RadioBoxDlg choiceDlg(this, _(L"Save List"), wxString{},
                                        _(L"Select which section to save:"), _(L"Save"), choices,
                                        descriptions);
    choiceDlg.SetSelection(static_cast<int>(m_lastSaveOption));
    if (choiceDlg.ShowModal() != wxID_OK)
        {
        return;
        }
    Save(filePath, static_cast<ExplanationListExportOptions>(choiceDlg.GetSelection()));
    m_lastSaveOption = static_cast<ExplanationListExportOptions>(choiceDlg.GetSelection());
    }

//------------------------------------------------------
void ExplanationListCtrl::OnCopy([[maybe_unused]] wxCommandEvent& event)
    {
    wxArrayString choices, descriptions;
    choices.Add(_(L"Grid (selected item)"));
    choices.Add(_(L"Grid (all items)"));
    choices.Add(_(L"Explanation"));
    descriptions.Add(_(L"Copy the selected item in the grid."));
    descriptions.Add(_(L"Copy all items in the grid."));
    descriptions.Add(_(L"Copy the explanation of the selected item in the grid."));
    Wisteria::UI::RadioBoxDlg choiceDlg(this, _(L"Copy List"), wxString{},
                                        _(L"Select which section to copy:"), _(L"Copy"), choices,
                                        descriptions);
    choiceDlg.SetSelection(static_cast<int>(m_lastCopyOption));
    if (choiceDlg.ShowModal() != wxID_OK)
        {
        return;
        }
    switch (choiceDlg.GetSelection())
        {
    case 0:
        GetResultsListCtrl()->Copy(true, false);
        break;
    case 1:
        GetResultsListCtrl()->Copy(false, true);
        break;
    case 2:
        GetExplanationView()->Copy();
        break;
        }
    m_lastCopyOption = static_cast<ExplanationListExportOptions>(choiceDlg.GetSelection());
    }

//------------------------------------------------------
void ExplanationListCtrl::OnFind(wxFindDialogEvent& event)
    {
    const ParentEventBlocker blocker(GetResultsListCtrl());
    GetResultsListCtrl()->ProcessWindowEvent(event);
    }

//------------------------------------------------------
void ExplanationListCtrl::OnResize(wxSizeEvent& event)
    {
    FitWindows();
    event.Skip();
    }

//------------------------------------------------------
void ExplanationListCtrl::OnItemSelected(const wxListEvent& event)
    {
    GetExplanationView()->SetPage(
        wxString::Format(L"<body>%s</body>",
                         m_explanations[GetResultsListCtrl()->GetItemTextEx(event.GetIndex(), 0)]));
    }

//------------------------------------------------------
void ExplanationListCtrl::FitWindows()
    {
    if (IsSplit())
        {
        if (GetResultsListCtrl()->GetItemCount() != 0)
            {
            wxRect rect;
            GetResultsListCtrl()->GetItemRect(GetResultsListCtrl()->GetItemCount() - 1, rect);
            SetSashPosition(
                std::min<double>(
                    rect.GetBottom() +
                        wxSystemSettings::GetMetric(wxSYS_VSCROLL_X, GetResultsListCtrl()) + 5,
                    GetSize().GetHeight() * .5),
                true);
            }
        else
            {
            SetSashPosition(static_cast<int>(GetSize().GetHeight() * .33), true);
            }
        }
    }

//------------------------------------------------------
void ExplanationListCtrl::UpdateExplanationDisplay()
    {
    const long selected = GetResultsListCtrl()->GetFirstSelected();
    if (selected != wxNOT_FOUND)
        {
        GetExplanationView()->SetPage(wxString::Format(
            L"<body>%s</body>", m_explanations[GetResultsListCtrl()->GetItemTextEx(selected, 0)]));
        }
    }

//------------------------------------------------------
wxString ExplanationListCtrl::GetExplanationsText() const
    {
    wxString descriptionHtml;
    if (GetResultsListCtrl() != nullptr)
        {
        for (long i = 0; i < GetResultsListCtrl()->GetItemCount(); ++i)
            {
            auto pos = m_explanations.find(GetResultsListCtrl()->GetItemTextEx(i, 0));
            if (pos != m_explanations.end())
                {
                descriptionHtml += pos->second;
                if (i < (GetResultsListCtrl()->GetItemCount() - 1))
                    {
                    descriptionHtml += L"<br />\n";
                    }
                }
            }
        }
    return descriptionHtml;
    }
