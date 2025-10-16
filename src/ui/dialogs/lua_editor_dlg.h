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

#ifndef LUA_EDITOR_DIALOG_H
#define LUA_EDITOR_DIALOG_H

#include "../../Wisteria-Dataviz/src/ui/controls/codeeditor.h"
#include "../../Wisteria-Dataviz/src/ui/dialogs/functionbrowserdlg.h"
#include <wx/aui/aui.h>
#include <wx/fdrepdlg.h>
#include <wx/html/helpwnd.h>
#include <wx/renderer.h>
#include <wx/ribbon/buttonbar.h>
#include <wx/splitter.h>
#include <wx/srchctrl.h>

/// @brief Lua editor and runner dialog.
class LuaEditorDlg final : public wxFrame
    {
  public:
    /** @brief Constructor.
        @param parent The dialog's parent.
        @param id The dialog's ID.
        @param caption The dialog's caption.
        @param pos The dialog's position.
        @param size The dialog's size.
        @param style The dialog's style.*/
    explicit LuaEditorDlg(wxWindow* parent, wxWindowID id = wxID_ANY,
                          const wxString& caption = _(L"Lua Script"),
                          const wxPoint& pos = wxDefaultPosition,
                          const wxSize& size = wxDefaultSize,
                          long style = wxCAPTION | wxCLOSE_BOX | wxMINIMIZE_BOX | wxMAXIMIZE_BOX |
                                       wxRESIZE_BORDER);
    /// @private
    LuaEditorDlg(const LuaEditorDlg&) = delete;
    /// @private
    LuaEditorDlg& operator=(const LuaEditorDlg&) = delete;

    /// @brief Sets the background color for the interface.
    ///     (Foreground colors will be adjusted to this color.)
    /// @param color The background color to use.
    void SetThemeColor(const wxColour& color);
    /// @brief Add a message to the debug output window.
    /// @param str The message to append.
    void DebugOutput(const wxString& str);
    /// @brief Clears the debug output window.
    void DebugClear();

    /// @returns A serialization string to store the interface layout.
    [[nodiscard]]
    wxString GetLayout()
        {
        return m_mgr.SavePerspective();
        }

    /// @brief Loads the layout of the interface.
    void LoadLayout(const wxString& layout) { m_mgr.LoadPerspective(layout); }

    /// @brief Ensures that all windows are shown, even if some are floating subwindows.
    void ShowAllWindows()
        {
        Show();
        for (auto& pn : m_mgr.GetAllPanes())
            {
            if (pn.name == L"funcbrowser" && !m_isFunctionBrowserShown)
                {
                continue;
                }
            pn.Show();
            }

        m_mgr.Update();
        }

    /// @brief Ensures that all windows are hidden, even if some are floating subwindows.
    void HideAllWindows()
        {
        // close any call tip and auto-completion windows
        if (m_notebook != nullptr && m_notebook->GetPageCount() > 0)
            {
            auto codeEditor = dynamic_cast<Wisteria::UI::CodeEditor*>(m_notebook->GetCurrentPage());
            if (codeEditor != nullptr)
                {
                if (codeEditor->CallTipActive())
                    {
                    codeEditor->CallTipCancel();
                    }
                if (codeEditor->AutoCompActive())
                    {
                    codeEditor->AutoCompCancel();
                    }
                }
            }

        Hide();
        for (auto& pn : m_mgr.GetAllPanes())
            {
            if (pn.name == L"funcbrowser")
                {
                m_isFunctionBrowserShown = pn.IsShown();
                }
            pn.Hide();
            }

        m_mgr.Update();
        }

  private:
    void ImportAPI();
    void CreateControls();
    void OnClose([[maybe_unused]] wxCloseEvent& event);
    void OnSave([[maybe_unused]] wxCommandEvent& event);
    void OnShowFindDialog([[maybe_unused]] wxCommandEvent& event);
    void OnShowReplaceDialog([[maybe_unused]] wxCommandEvent& event);
    void OnFindDialog(wxFindDialogEvent& event);

    [[nodiscard]]
    Wisteria::UI::CodeEditor* CreateLuaScript(wxWindow* parent);

    wxAuiNotebook* m_notebook{ nullptr };
    wxAuiToolBar* m_toolbar{ nullptr };
    wxHtmlWindow* m_debugMessageWindow{ nullptr };
    Wisteria::UI::FunctionBrowserCtrl* m_functionBrowser{ nullptr };
    wxAuiManager m_mgr;
    bool m_isFunctionBrowserShown{ false };

    wxFindReplaceData m_findData{ wxFR_DOWN };

    wxFindReplaceDialog* m_dlgFind{ nullptr };
    wxFindReplaceDialog* m_dlgReplace{ nullptr };

    using LibnameAndEntries = std::pair<std::wstring, Wisteria::UI::CodeEditor::NameList>;
    std::vector<LibnameAndEntries> m_classes;
    std::vector<LibnameAndEntries> m_libraries;
    std::vector<LibnameAndEntries> m_enums;
    };

#endif // LUA_EDITOR_DIALOG_H
