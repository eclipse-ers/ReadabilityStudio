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

#ifndef SCRIPT_WORKBENCH_PANEL_H
#define SCRIPT_WORKBENCH_PANEL_H

#include "../../Wisteria-Dataviz/src/ui/controls/codeeditor.h"
#include "../../Wisteria-Dataviz/src/ui/controls/sidebar.h"
#include "../../Wisteria-Dataviz/src/ui/dialogs/functionbrowserdlg.h"
#include <vector>
#include <wx/fdrepdlg.h>
#include <wx/panel.h>
#include <wx/simplebook.h>
#include <wx/splitter.h>
#include <wx/webview.h>

/// @brief Script workbench: script sidebar, single-editor area, debug output,
///     and an optional function-browser column.
class ScriptWorkbenchPanel final : public wxPanel
    {
  public:
    explicit ScriptWorkbenchPanel(wxWindow* parent, wxWindowID id = wxID_ANY);
    ScriptWorkbenchPanel(const ScriptWorkbenchPanel&) = delete;
    ScriptWorkbenchPanel& operator=(const ScriptWorkbenchPanel&) = delete;

    /// @name Operations (called by the ribbon / accelerators)
    /// @{
    void NewScript();
    void OpenScriptFromFile(const wxString& path);
    void SaveCurrentScript();
    void RunCurrentScript();
    void StopScript();
    void ToggleFunctionBrowser();
    void ShowFindDialog();
    void ShowReplaceDialog();
    /// @}

    /// @brief Apply the current theme background color to the workbench surfaces.
    void SetThemeColor(const wxColour& color);

    /// @brief Append a message to the debug output window.
    void DebugOutput(const wxString& str);
    /// @brief Clear the debug output window.
    void DebugClear();

    /// @brief Prompts the user to save any dirty script buffers (Yes/No/Cancel
    ///     per script).
    /// @returns @c true if the caller may proceed (no dirty buffers, or every
    ///     prompt was answered Yes or No); @c false if the user cancelled,
    ///     in which case the calling close/exit flow should be aborted.
    bool PromptToSaveUnsavedScripts();

    /// @returns @c true if any open script buffer has unsaved changes,
    ///     or if any script other than the default untitled buffer is loaded.
    [[nodiscard]]
    bool HasUnsavedOrLoadedScripts() const;

    /// @returns @c true while a script is executing.
    [[nodiscard]]
    bool IsScriptRunning() const noexcept
        {
        return m_isScriptRunning;
        }

    /// @returns @c true if the function browser is currently shown.
    [[nodiscard]]
    bool IsFunctionBrowserVisible() const noexcept;

    /// @returns @c true if the debug window is currently shown.
    [[nodiscard]]
    bool IsDebugWindowVisible() const noexcept;

    /// @brief Toggles the debug window visibility.
    void ToggleDebugWindow();

  private:
    struct ScriptEntry
        {
        wxWindowID m_sidebarId{ wxID_ANY };
        wxWindow* m_page{ nullptr };
        Wisteria::UI::CodeEditor* m_editor{ nullptr };
        };

    void ImportAPI();
    void CreateControls();
    void BindEvents();

    void OnFindDialog(wxFindDialogEvent& event);
    void OnSidebarClick(wxCommandEvent& event);

    /// @brief Updates the sidebar label of the entry that owns @p editor to
    ///     reflect its current dirty state (appends " *" when modified).
    void UpdateDirtyMark(Wisteria::UI::CodeEditor* editor);

    /// @returns The editor for the currently-selected script, or nullptr.
    [[nodiscard]]
    Wisteria::UI::CodeEditor* GetCurrentEditor() const;

    [[nodiscard]]
    Wisteria::UI::CodeEditor* CreateLuaScript(wxWindow* parent);

    /// @brief Adds a new script entry (sidebar item + simplebook page),
    ///     selects it, and returns the created editor.
    Wisteria::UI::CodeEditor* AddScriptEntry(const wxString& label);

    // Layout / controls
    wxSplitterWindow* m_outerSplitter{ nullptr };       // top: editor area, bottom: debug
    wxSplitterWindow* m_sidebarSplitter{ nullptr };     // left: sidebar, right: editor+funcbrowser
    wxSplitterWindow* m_funcBrowserSplitter{ nullptr }; // left: editor book, right: func browser

    Wisteria::UI::SideBar* m_scriptSidebar{ nullptr };
    wxSimplebook* m_editorBook{ nullptr };
    wxWebView* m_debugMessageWindow{ nullptr };
    wxString m_debugContent;
    Wisteria::UI::FunctionBrowserCtrl* m_functionBrowser{ nullptr };

    wxWindowID m_scriptsFolderId{ wxNewId() };
    int m_nextSidebarSubId{ 10000 };
    bool m_isScriptRunning{ false };

    std::vector<ScriptEntry> m_scripts;

    // find/replace
    wxFindReplaceData m_findData{ wxFR_DOWN };
    wxFindReplaceDialog* m_dlgFind{ nullptr };
    wxFindReplaceDialog* m_dlgReplace{ nullptr };

    // API tables imported from .api files
    using LibnameAndEntries = std::pair<std::wstring, Wisteria::UI::CodeEditor::NameList>;
    std::vector<LibnameAndEntries> m_classes;
    std::vector<LibnameAndEntries> m_libraries;
    std::vector<LibnameAndEntries> m_enums;
    };

#endif // SCRIPT_WORKBENCH_PANEL_H
