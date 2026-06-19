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

#include "explanation_listctrl.h"
#include "../../projects/project_navigation_links.h"
#include "../../Wisteria-Dataviz/src/import/html_encode.h"

wxIMPLEMENT_DYNAMIC_CLASS(ExplanationListCtrl, wxPanel)

    ExplanationListExportOptions ExplanationListCtrl::m_lastCopyOption =
        ExplanationListExportOptions::ExportGrid;
ExplanationListExportOptions ExplanationListCtrl::m_lastSaveOption =
    ExplanationListExportOptions::ExportBoth;

//------------------------------------------------------
ExplanationListCtrl::ExplanationListCtrl(wxWindow* parent, wxWindowID id,
                                         const wxPoint& point /*= wxDefaultPosition*/,
                                         const wxSize& size /*= wxDefaultSize*/,
                                         const wxString& name /*= wxString{}*/)
    : wxPanel(parent, id, point, size, wxTAB_TRAVERSAL | wxBORDER_NONE | wxCLIP_CHILDREN, name)
    {
    // NOLINTBEGIN(cppcoreguidelines-prefer-member-initializer)
    auto* splitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                          wxSP_LIVE_UPDATE | wxSP_3DSASH | wxBORDER_NONE);
    m_results_view = new Wisteria::UI::ListCtrlEx(
        splitter, id, wxDefaultPosition, wxDefaultSize,
        wxLC_SINGLE_SEL | wxLC_REPORT | wxLC_VIRTUAL | wxBORDER_SUNKEN, wxDefaultValidator);
    GetResultsListCtrl()->SetVirtualDataProvider(m_data);
    GetResultsListCtrl()->SetVirtualDataSize(0);
    GetResultsListCtrl()->EnableGridLines();
    GetResultsListCtrl()->EnableAlternateRowColours(false);
    m_explanation_view = wxWebView::New(splitter, wxID_ANY);
    // NOLINTEND(cppcoreguidelines-prefer-member-initializer)
    if (GetExplanationView() == nullptr)
        {
        wxLogError(_(L"Failed to create wxWebView. No backend available."));
        }
    else
        {
        // suppress the browser's reload/view-source context menu
        GetExplanationView()->EnableContextMenu(false);
        GetExplanationView()->Bind(wxEVT_WEBVIEW_LOADED, &ExplanationListCtrl::OnExplanationLoaded,
                                   this);
        }

    wxWindow* explanationWindow = (GetExplanationView() != nullptr) ?
                                      static_cast<wxWindow*>(GetExplanationView()) :
                                      new wxPanel(splitter);
    splitter->SplitHorizontally(m_results_view, explanationWindow);
    splitter->SetSashGravity(0.75);

    auto* sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(splitter, wxSizerFlags{ 1 }.Expand());
    SetSizer(sizer);

    Bind(wxEVT_MENU, &ExplanationListCtrl::OnPreview, this, wxID_PREVIEW);
    Bind(wxEVT_MENU, &ExplanationListCtrl::OnCopy, this, wxID_COPY);
    Bind(wxEVT_MENU, &ExplanationListCtrl::OnSave, this, wxID_SAVE);
    Bind(wxEVT_MENU, &ExplanationListCtrl::OnPrint, this, wxID_PRINT);
    Bind(wxEVT_MENU, &ExplanationListCtrl::OnMenuCommand, this, XRCID("ID_VIEW_ITEM"));
    Bind(wxEVT_MENU, &ExplanationListCtrl::OnMenuCommand, this, XRCID("ID_LIST_SORT"));
    Bind(wxEVT_MENU, &ExplanationListCtrl::OnMenuCommand, this, XRCID("ID_SAVE_LIST"));

    Bind(wxEVT_FIND, &ExplanationListCtrl::OnFind, this);
    Bind(wxEVT_FIND_NEXT, &ExplanationListCtrl::OnFind, this);
    Bind(wxEVT_FIND_CLOSE, &ExplanationListCtrl::OnFind, this);

    Bind(wxEVT_LIST_ITEM_SELECTED, &ExplanationListCtrl::OnItemSelected, this, id);
    Bind(wxEVT_SHOW, &ExplanationListCtrl::OnShow, this);
    }

//------------------------------------------------------
ExplanationListCtrl::~ExplanationListCtrl()
    {
    Unbind(wxEVT_SHOW, &ExplanationListCtrl::OnShow, this);
    }

//------------------------------------------------------
void ExplanationListCtrl::OnShow(wxShowEvent& event)
    {
    if (event.IsShown())
        {
        Layout();
        const long selected = GetResultsListCtrl()->GetFirstSelected();
        if (selected != wxNOT_FOUND && GetExplanationView() != nullptr && !IsBeingDeleted() &&
            !GetExplanationView()->IsBeingDeleted())
            {
            GetExplanationView()->Hide();
            GetExplanationView()->SetPage(
                NavLink::AnchorsToExplanationScheme(wxString::Format(
                    _DT(L"<!DOCTYPE html><html><head>"
                        "<meta name='color-scheme' content='light dark' />"
                        "<style>body{background-color:Canvas;color:CanvasText;}</style>"
                        "</head><body>%s</body></html>"),
                    m_explanations[GetResultsListCtrl()->GetItemTextEx(selected, 0)])),
                wxString{});
            }
        }
    event.Skip();
    }

//------------------------------------------------------
void ExplanationListCtrl::OnExplanationLoaded(wxWebViewEvent& event)
    {
    if (GetExplanationView() != nullptr && !IsBeingDeleted() &&
        !GetExplanationView()->IsBeingDeleted() && !GetExplanationView()->IsShown())
        {
        GetExplanationView()->Show();
        Layout();
        }
    event.Skip();
    }

//------------------------------------------------------
void ExplanationListCtrl::OnMenuCommand(wxCommandEvent& event)
    {
    const ParentEventBlocker blocker(GetResultsListCtrl());
    if (event.GetId() == XRCID("ID_SAVE_LIST"))
        {
        event.SetId(wxID_SAVE);
        }
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
    // NOLINTBEGIN(bugprone-branch-clone,misc-static-cast-sign-conversion,clang-analyzer-optin.core.EnumCastOutOfRange)
    Save(filePath, static_cast<ExplanationListExportOptions>(choiceDlg.GetSelection()));
    m_lastSaveOption = static_cast<ExplanationListExportOptions>(choiceDlg.GetSelection());
    // NOLINTEND(bugprone-branch-clone,misc-static-cast-sign-conversion,clang-analyzer-optin.core.EnumCastOutOfRange)
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
        if (GetExplanationView() != nullptr)
            {
            GetExplanationView()->Copy();
            }
        break;
    default:
        // noop
        break;
        }
    // NOLINTNEXTLINE(clang-analyzer-optin.core.EnumCastOutOfRange)
    m_lastCopyOption = static_cast<ExplanationListExportOptions>(choiceDlg.GetSelection());
    }

//------------------------------------------------------
void ExplanationListCtrl::OnFind(wxFindDialogEvent& event)
    {
    const ParentEventBlocker blocker(GetResultsListCtrl());
    GetResultsListCtrl()->ProcessWindowEvent(event);
    }

//------------------------------------------------------
void ExplanationListCtrl::OnItemSelected(const wxListEvent& event)
    {
    if (GetExplanationView() == nullptr)
        {
        return;
        }
    GetExplanationView()->Hide();
    GetExplanationView()->SetPage(
        NavLink::AnchorsToExplanationScheme(wxString::Format(
            _DT(L"<!DOCTYPE html><html><head><meta name='color-scheme' content='light dark' />"
                "<style>body{background-color:Canvas;color:CanvasText;}</style>"
                "</head><body>%s</body></html>"),
            m_explanations[GetResultsListCtrl()->GetItemTextEx(event.GetIndex(), 0)])),
        wxString{});
    }

//------------------------------------------------------
void ExplanationListCtrl::UpdateExplanationDisplay()
    {
    const long selected = GetResultsListCtrl()->GetFirstSelected();
    if (selected != wxNOT_FOUND && GetExplanationView() != nullptr)
        {
        GetExplanationView()->Hide();
        GetExplanationView()->SetPage(
            NavLink::AnchorsToExplanationScheme(wxString::Format(
                _DT(L"<!DOCTYPE html><html><head>"
                    "<meta name='color-scheme' content='light dark' />"
                    "<style>body{background-color:Canvas;color:CanvasText;}</style>"
                    "</head><body>%s</body></html>"),
                m_explanations[GetResultsListCtrl()->GetItemTextEx(selected, 0)])),
            wxString{});
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
