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

#include "script_workbench_panel.h"
#include "../../Wisteria-Dataviz/src/base/colorbrewer.h"
#include "../../app/readability_app.h"
#include "../../lua-scripting/lua_interface.h"
#include "../../lua/lua.h"
#include <algorithm>
#include <array>
#include <utility>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/numdlg.h>
#include <wx/srchctrl.h>
#include <wx/stc/minimap.h>

wxDECLARE_APP(ReadabilityApp);

//-------------------------------------------------------
ScriptWorkbenchPanel::ScriptWorkbenchPanel(wxWindow* parent, wxWindowID id /*= wxID_ANY*/)
    : wxPanel(parent, id)
    {
    ImportAPI();
    CreateControls();
    BindEvents();
    }

//------------------------------------------------------
void ScriptWorkbenchPanel::ImportAPI()
    {
    std::vector<std::vector<std::wstring>> apiStrings;

    lily_of_the_valley::standard_delimited_character_column tabbedColumn(
        lily_of_the_valley::text_column_delimited_character_parser{ L'\t' }, 1);
    lily_of_the_valley::standard_delimited_character_column semiColonColumn(
        lily_of_the_valley::text_column_delimited_character_parser{ L';' }, std::nullopt);
    lily_of_the_valley::text_row<std::wstring> row(std::nullopt);
    row.treat_consecutive_delimiters_as_one(true);
    row.add_column(tabbedColumn);
    row.add_column(semiColonColumn);
    row.allow_column_resizing(true);

    lily_of_the_valley::text_matrix<std::wstring> importer(&apiStrings);
    importer.add_row_definition(lily_of_the_valley::text_row<std::wstring>(1));
    importer.add_row_definition(row);

    lily_of_the_valley::text_preview preview;

    const auto loadApiFile =
        [&](const wxString& resourceName, std::vector<LibnameAndEntries>& target)
    {
        wxString libraryText;
        wxString libFilePath = wxGetApp().FindResourceFile(resourceName);
        if (!Wisteria::TextStream::ReadFile(libFilePath, libraryText))
            {
            return;
            }
        apiStrings.clear();
        const size_t rowCount = preview(libraryText, L'\t', true, false);
        if (rowCount == 0)
            {
            return;
            }
        apiStrings.resize(rowCount);
        importer.read(libraryText, rowCount, 2, true);

        for (auto& lib : apiStrings)
            {
            if (lib.empty())
                {
                continue;
                }
            const std::wstring libName = lib.front();
            lib.erase(lib.begin(), lib.begin() + 1);
            Wisteria::UI::CodeEditor::NameList nl;
            for (const auto& entry : lib)
                {
                nl.insert(entry);
                }
            target.emplace_back(libName, nl);
            }
    };

    loadApiFile(L"rs-classes.api", m_classes);
    loadApiFile(L"rs-libraries.api", m_libraries);
    loadApiFile(L"rs-enums.api", m_enums);
    }

//-------------------------------------------------------
void ScriptWorkbenchPanel::CreateControls()
    {
    // outer horizontal splitter: top = editor area, bottom = debug
    m_outerSplitter = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                           wxSP_LIVE_UPDATE | wxSP_3DSASH);
    m_outerSplitter->SetMinimumPaneSize(FromDIP(80));
    m_outerSplitter->SetSashGravity(1.0); // top (editor area) absorbs resize

    // sidebar splitter (top half): left = sidebar, right = editor + func browser
    m_sidebarSplitter = new wxSplitterWindow(m_outerSplitter, wxID_ANY, wxDefaultPosition,
                                             wxDefaultSize, wxSP_LIVE_UPDATE | wxSP_3DSASH);
    m_sidebarSplitter->SetMinimumPaneSize(FromDIP(120));
    m_sidebarSplitter->SetSashGravity(0.0); // right (editor area) absorbs resize

    m_scriptSidebar = new Wisteria::UI::SideBar(m_sidebarSplitter);
    m_scriptSidebar->SetImageList({ wxGetApp().GetResourceManager().GetSVG(
        wxSystemSettings::GetAppearance().IsDark() ? L"ribbon/lua-dark-mode.svg" :
                                                     L"ribbon/lua.svg") });
    m_scriptSidebar->InsertItem(0, _(L"Scripts"), m_scriptsFolderId, std::nullopt);

    // func browser splitter (right of sidebar): left = editor book, right = func browser
    m_funcBrowserSplitter = new wxSplitterWindow(m_sidebarSplitter, wxID_ANY, wxDefaultPosition,
                                                 wxDefaultSize, wxSP_LIVE_UPDATE | wxSP_3DSASH);
    m_funcBrowserSplitter->SetMinimumPaneSize(FromDIP(200));
    m_funcBrowserSplitter->SetSashGravity(1.0); // left (editor) absorbs resize

    m_editorBook = new wxSimplebook(m_funcBrowserSplitter, wxID_ANY);

    m_functionBrowser = new Wisteria::UI::FunctionBrowserCtrl(m_funcBrowserSplitter, this);
    m_functionBrowser->SetParameterSeparator(FormulaFormat::GetListSeparator());
    m_functionBrowser->AddCategory(_(L"Libraries"), 1000);
    m_functionBrowser->AddCategory(_(L"Classes"), 1001);
    m_functionBrowser->AddCategory(_(L"Enumerations"), 1002);
    for (const auto& theClass : m_classes)
        {
        m_functionBrowser->AddCategory(theClass.first, theClass.second, 1001);
        }
    for (const auto& theLib : m_libraries)
        {
        m_functionBrowser->AddCategory(theLib.first, theLib.second, 1000);
        }
    for (const auto& theLib : m_enums)
        {
        m_functionBrowser->AddCategory(theLib.first, theLib.second, 1002);
        }
    m_functionBrowser->FinalizeCategories();
    m_functionBrowser->Hide();

    // func browser starts hidden; the editor book occupies the entire right side
    m_funcBrowserSplitter->Initialize(m_editorBook);

    // sidebar | (editor + func browser)
    m_sidebarSplitter->SplitVertically(m_scriptSidebar, m_funcBrowserSplitter, FromDIP(200));

    // debug output (bottom half)
    m_debugMessageWindow = wxWebView::New(m_outerSplitter, wxID_ANY);

    m_outerSplitter->SplitHorizontally(m_sidebarSplitter, m_debugMessageWindow, -FromDIP(150));

    auto* outerSizer = new wxBoxSizer(wxVERTICAL);
    outerSizer->Add(m_outerSplitter, wxSizerFlags{ 1 }.Expand());
    SetSizer(outerSizer);

    // seed the default untitled script (with the import header, marked clean)
    AddScriptEntry(_(L"(unnamed)"));

    DebugClear();
    }

//-------------------------------------------------------
void ScriptWorkbenchPanel::BindEvents()
    {
    // search ctrl events (e.g., the function browser's search box)
    Bind(wxEVT_SEARCH,
         [this](wxCommandEvent& evt)
         {
             auto* codeEditor = GetCurrentEditor();
             if (codeEditor != nullptr)
                 {
                 codeEditor->FindNext(evt.GetString());
                 }
         });

    // find / replace dialog events
    Bind(wxEVT_FIND, &ScriptWorkbenchPanel::OnFindDialog, this);
    Bind(wxEVT_FIND_NEXT, &ScriptWorkbenchPanel::OnFindDialog, this);
    Bind(wxEVT_FIND_REPLACE, &ScriptWorkbenchPanel::OnFindDialog, this);
    Bind(wxEVT_FIND_REPLACE_ALL, &ScriptWorkbenchPanel::OnFindDialog, this);
    Bind(wxEVT_FIND_CLOSE, &ScriptWorkbenchPanel::OnFindDialog, this);

    Bind(
        wxEVT_CHAR_HOOK,
        [this](wxKeyEvent& event)
        {
            if (event.GetKeyCode() == WXK_F3)
                {
                wxFindDialogEvent evt{ wxEVT_FIND_NEXT };
                evt.SetFindString(m_findData.GetFindString());
                evt.SetFlags(m_findData.GetFlags());
                OnFindDialog(evt);
                }
            else if (event.GetKeyCode() == L'G' && event.ControlDown())
                {
                GotoLineDialog();
                return;
                }
            else if (event.GetKeyCode() == L'/' && event.ControlDown())
                {
                auto* editor = GetCurrentEditor();
                if (editor != nullptr)
                    {
                    const int lineStart = editor->LineFromPosition(editor->GetSelectionStart());
                    const wxString lineText = editor->GetLine(lineStart).Trim(false);
                    if (lineText.StartsWith(L"--"))
                        {
                        Uncomment();
                        }
                    else
                        {
                        Comment();
                        }
                    }
                return;
                }
            event.Skip(true);
        },
        wxID_ANY);

    // sidebar selection swaps the visible editor
    Bind(Wisteria::UI::wxEVT_SIDEBAR_CLICK, &ScriptWorkbenchPanel::OnSidebarClick, this);
    }

//-------------------------------------------------------
Wisteria::UI::CodeEditor* ScriptWorkbenchPanel::GetCurrentEditor() const
    {
    const int sel = m_editorBook->GetSelection();
    if (sel == wxNOT_FOUND)
        {
        return nullptr;
        }
    for (const auto& entry : m_scripts)
        {
        if (m_editorBook->FindPage(entry.m_page) == sel)
            {
            return entry.m_editor;
            }
        }
    return nullptr;
    }

//-------------------------------------------------------
Wisteria::UI::CodeEditor* ScriptWorkbenchPanel::CreateLuaScript(wxWindow* parent)
    {
    auto* codeEditor = new Wisteria::UI::CodeEditor(parent, wxSTC_LEX_LUA);
    codeEditor->Show(false);
    codeEditor->IncludeNumberMargin(true);
    codeEditor->IncludeFoldingMargin(true);
    codeEditor->SetDefaultHeader(
        L"-- " +
        wxString::Format(_(L"Imports %s specific enumerations"), wxGetApp().GetAppDisplayName()) +
        L"\ndofile(Application.GetLuaConstantsPath())\n\n");
    codeEditor->SetText(codeEditor->GetDefaultHeader());

    for (const auto& theClass : m_classes)
        {
        codeEditor->AddClass(theClass.first, theClass.second);
        }
    for (const auto& theLib : m_libraries)
        {
        codeEditor->AddLibrary(theLib.first, theLib.second);
        }
    for (const auto& theLib : m_enums)
        {
        codeEditor->AddLibrary(theLib.first, theLib.second);
        }
    codeEditor->Finalize();

    codeEditor->SetSelection(codeEditor->GetTextLength(), codeEditor->GetTextLength());
    // mark the seeded header as the save point so undo can't roll past it and
    // the buffer isn't considered dirty on creation
    codeEditor->EmptyUndoBuffer();
    codeEditor->SetSavePoint();
    codeEditor->SetModified(false);
    codeEditor->Show();

    return codeEditor;
    }

//-------------------------------------------------------
Wisteria::UI::CodeEditor* ScriptWorkbenchPanel::AddScriptEntry(const wxString& label)
    {
    auto* page = new wxPanel(m_editorBook);
    auto* codeEditor = CreateLuaScript(page);
    auto* miniMap = new wxStyledTextCtrlMiniMap(page, codeEditor);
    auto* sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(codeEditor, wxSizerFlags{ 1 }.Expand());
    sizer->Add(miniMap, wxSizerFlags{}.Expand());
    page->SetSizer(sizer);
    page->SetClientData(codeEditor);

    m_editorBook->AddPage(page, label, true);

    const wxWindowID subId = m_nextSidebarSubId++;
    m_scriptSidebar->InsertSubItemById(m_scriptsFolderId, label, subId, 0);
    m_scriptSidebar->GetFolder(0).Expand();
    m_scriptSidebar->Realize();
    m_scriptSidebar->SelectSubItemById(m_scriptsFolderId, subId, false, false);

    m_scripts.push_back(ScriptEntry{ subId, page, codeEditor });

    codeEditor->Bind(wxEVT_STC_SAVEPOINTLEFT,
                     [this, codeEditor](wxStyledTextEvent&) { UpdateDirtyMark(codeEditor); });
    codeEditor->Bind(wxEVT_STC_SAVEPOINTREACHED,
                     [this, codeEditor](wxStyledTextEvent&) { UpdateDirtyMark(codeEditor); });

    codeEditor->SetFocus();
    return codeEditor;
    }

//-------------------------------------------------------
void ScriptWorkbenchPanel::UpdateDirtyMark(Wisteria::UI::CodeEditor* editor)
    {
    for (const auto& entry : m_scripts)
        {
        if (entry.m_editor != editor)
            {
            continue;
            }
        const wxString& path = entry.m_editor->GetScriptFilePath();
        const wxString base = path.empty() ? _(L"(unnamed)") : wxFileName(path).GetName();
        const wxString label = entry.m_editor->GetModify() ? (base + L" *") : base;
        m_scriptSidebar->SetSubItemLabel(m_scriptsFolderId, entry.m_sidebarId, label);
        return;
        }
    }

//-------------------------------------------------------
void ScriptWorkbenchPanel::NewScript() { AddScriptEntry(_(L"(unnamed)")); }

//-------------------------------------------------------
void ScriptWorkbenchPanel::OpenScriptFromFile(const wxString& path)
    {
    // if only the default empty/untitled script is open, replace it with the loaded file
    if (m_scripts.size() == 1)
        {
        auto* current = m_scripts.front().m_editor;
        if (current != nullptr && !current->GetModify() && current->GetScriptFilePath().empty())
            {
            // set the path before LoadFile so the SAVEPOINTREACHED handler can
            // derive the sidebar label from it
            current->SetScriptFilePath(path);
            current->LoadFile(path);
            current->SetSelection(current->GetTextLength(), current->GetTextLength());
            m_editorBook->SetPageText(0, wxFileName(path).GetName());
            current->SetFocus();
            return;
            }
        }

    // ...otherwise open as a new entry
    auto* editor = AddScriptEntry(wxFileName(path).GetName());
    if (editor != nullptr)
        {
        // set the path before LoadFile so the SAVEPOINTREACHED handler can
        // derive the sidebar label from it
        editor->SetScriptFilePath(path);
        editor->LoadFile(path);
        editor->SetSelection(editor->GetTextLength(), editor->GetTextLength());
        editor->SetFocus();
        }
    }

//-------------------------------------------------------
void ScriptWorkbenchPanel::SaveCurrentScript()
    {
    auto* codeEditor = GetCurrentEditor();
    if (codeEditor == nullptr || !codeEditor->Save())
        {
        return;
        }
    const int sel = m_editorBook->GetSelection();
    if (sel == wxNOT_FOUND)
        {
        return;
        }
    const wxString newLabel = wxFileName(codeEditor->GetScriptFilePath()).GetName();
    m_editorBook->SetPageText(sel, newLabel);

    for (const auto& entry : m_scripts)
        {
        if (entry.m_editor == codeEditor)
            {
            m_scriptSidebar->SetSubItemLabel(m_scriptsFolderId, entry.m_sidebarId, newLabel);
            break;
            }
        }
    }

//-------------------------------------------------------
void ScriptWorkbenchPanel::RunCurrentScript()
    {
    auto* editor = GetCurrentEditor();
    if (editor == nullptr)
        {
        return;
        }
    editor->AnnotationClearAll();

    wxString errorMessage;
    m_isScriptRunning = true;

    wxGetApp().GetLuaRunner().RunLuaCode(
        (editor->GetSelectionStart() == editor->GetSelectionEnd()) ? editor->GetValue() :
                                                                     editor->GetSelectedText(),
        editor->GetScriptFilePath(), errorMessage);

    m_isScriptRunning = false;

    if (errorMessage.empty())
        {
        return;
        }

    const int lineOffset = (editor->GetSelectionStart() == editor->GetSelectionEnd()) ?
                               0 :
                               editor->LineFromPosition(editor->GetSelectionStart());

    long lineNumber{ 0 };
    errorMessage.ToLong(&lineNumber);
    --lineNumber; // zero-indexed
    lineNumber += lineOffset;

    if (const auto foundPos = errorMessage.find(L':'); foundPos != wxString::npos)
        {
        errorMessage.erase(0, foundPos + 1);
        }

    wxMessageBox(
        wxString::Format(_(L"Line #%s: %s"), std::to_wstring(lineNumber + 1), errorMessage),
        _(L"Script Error"), wxOK | wxICON_EXCLAMATION);

    if (lineNumber < 0)
        {
        return;
        }
    errorMessage.insert(0, _(L"Error:"));
    editor->GotoLine((editor->GetFirstVisibleLine() < lineNumber) ? lineNumber : lineNumber - 1);
    editor->AnnotationSetText(lineNumber, errorMessage);
    editor->AnnotationSetStyle(lineNumber, Wisteria::UI::CodeEditor::ERROR_ANNOTATION_STYLE);

    // Scintilla doesn't update scroll width for annotations, do it manually
    const int width = editor->GetScrollWidth();
    const unsigned long indent = editor->GetLineIndentation(lineNumber) + FromDIP(3);
    const int widthAnn = editor->TextWidth(Wisteria::UI::CodeEditor::ERROR_ANNOTATION_STYLE,
                                           errorMessage + wxString{ indent, L' ' });
    if (widthAnn > width)
        {
        editor->SetScrollWidth(widthAnn);
        }
    }

//-------------------------------------------------------
void ScriptWorkbenchPanel::StopScript()
    {
    LuaInterpreter::Quit();
    m_isScriptRunning = false;
    }

//-------------------------------------------------------
bool ScriptWorkbenchPanel::IsFunctionBrowserVisible() const noexcept
    {
    return (m_funcBrowserSplitter != nullptr && m_funcBrowserSplitter->IsSplit());
    }

//-------------------------------------------------------
bool ScriptWorkbenchPanel::IsDebugWindowVisible() const noexcept
    {
    return (m_outerSplitter != nullptr && m_outerSplitter->IsSplit());
    }

//-------------------------------------------------------
void ScriptWorkbenchPanel::ToggleDebugWindow()
    {
    if (m_outerSplitter->IsSplit())
        {
        m_outerSplitter->Unsplit(m_debugMessageWindow);
        }
    else
        {
        m_debugMessageWindow->Show();
        m_outerSplitter->SplitHorizontally(m_sidebarSplitter, m_debugMessageWindow, -FromDIP(150));
        }
    }

//-------------------------------------------------------
void ScriptWorkbenchPanel::ToggleFunctionBrowser()
    {
    if (m_funcBrowserSplitter->IsSplit())
        {
        m_funcBrowserSplitter->Unsplit(m_functionBrowser);
        }
    else
        {
        m_functionBrowser->Show();
        m_funcBrowserSplitter->SplitVertically(m_editorBook, m_functionBrowser, -FromDIP(500));
        }
    }

//-------------------------------------------------------
void ScriptWorkbenchPanel::OnSidebarClick(wxCommandEvent& event)
    {
    const wxWindowID clickedId = event.GetInt();
    for (const auto& entry : m_scripts)
        {
        if (entry.m_sidebarId == clickedId)
            {
            const int idx = m_editorBook->FindPage(entry.m_page);
            if (idx != wxNOT_FOUND)
                {
                m_editorBook->SetSelection(idx);
                if (entry.m_editor != nullptr)
                    {
                    entry.m_editor->SetFocus();
                    }
                }
            return;
            }
        }
    }

//-------------------------------------------------------
void ScriptWorkbenchPanel::ShowReplaceDialog()
    {
    auto* currentScript = GetCurrentEditor();
    if (currentScript != nullptr && m_findData.GetFindString().empty())
        {
        m_findData.SetFindString(currentScript->GetSelectedText());
        }

    if (m_dlgFind != nullptr)
        {
        m_dlgFind->Destroy();
        m_dlgFind = nullptr;
        }
    if (m_dlgReplace == nullptr)
        {
        m_dlgReplace =
            new wxFindReplaceDialog(this, &m_findData, _(L"Replace"), wxFR_REPLACEDIALOG);
        }
    m_dlgReplace->Show(true);
    m_dlgReplace->SetFocus();
    }

//-------------------------------------------------------
void ScriptWorkbenchPanel::ShowFindDialog()
    {
    auto* currentScript = GetCurrentEditor();
    if (currentScript != nullptr && m_findData.GetFindString().empty())
        {
        m_findData.SetFindString(currentScript->GetSelectedText());
        }

    if (m_dlgReplace != nullptr)
        {
        m_dlgReplace->Destroy();
        m_dlgReplace = nullptr;
        }
    if (m_dlgFind == nullptr)
        {
        m_dlgFind = new wxFindReplaceDialog(this, &m_findData, _(L"Find"));
        }
    m_dlgFind->Show(true);
    m_dlgFind->SetFocus();
    }

//-------------------------------------------------------
void ScriptWorkbenchPanel::Undo()
    {
    if (GetCurrentEditor() != nullptr)
        {
        GetCurrentEditor()->Undo();
        }
    }

//-------------------------------------------------------
void ScriptWorkbenchPanel::Redo()
    {
    if (GetCurrentEditor() != nullptr)
        {
        GetCurrentEditor()->Redo();
        }
    }

//-------------------------------------------------------
void ScriptWorkbenchPanel::DuplicateLine()
    {
    if (GetCurrentEditor() != nullptr)
        {
        GetCurrentEditor()->LineDuplicate();
        }
    }

//-------------------------------------------------------
void ScriptWorkbenchPanel::GotoLineDialog()
    {
    auto* editor = GetCurrentEditor();
    if (editor == nullptr)
        {
        return;
        }

    const long lineCount = editor->GetLineCount();
    const long currentLine = editor->GetCurrentLine() + 1;
    const long newLine = wxGetNumberFromUser(_(L"Enter line number:"), wxEmptyString,
                                             _(L"Go To Line"), currentLine, 1, lineCount, this);
    if (newLine != -1)
        {
        editor->GotoLine(newLine - 1);
        }
    }

//-------------------------------------------------------
void ScriptWorkbenchPanel::Comment()
    {
    auto* editor = GetCurrentEditor();
    if (editor == nullptr)
        {
        return;
        }

    const int lineStart = editor->LineFromPosition(editor->GetSelectionStart());
    int lineEnd = editor->LineFromPosition(editor->GetSelectionEnd());

    // If the selection ends at the start of a line, don't include that line
    if (lineEnd > lineStart && editor->GetColumn(editor->GetSelectionEnd()) == 0)
        {
        lineEnd--;
        }

    editor->BeginUndoAction();
    for (int i = lineStart; i <= lineEnd; ++i)
        {
        editor->InsertText(editor->PositionFromLine(i), L"--");
        }
    editor->EndUndoAction();
    }

//-------------------------------------------------------
void ScriptWorkbenchPanel::Uncomment()
    {
    auto* editor = GetCurrentEditor();
    if (editor == nullptr)
        {
        return;
        }

    const int lineStart = editor->LineFromPosition(editor->GetSelectionStart());
    int lineEnd = editor->LineFromPosition(editor->GetSelectionEnd());

    // If the selection ends at the start of a line, don't include that line
    if (lineEnd > lineStart && editor->GetColumn(editor->GetSelectionEnd()) == 0)
        {
        lineEnd--;
        }

    editor->BeginUndoAction();
    for (int i = lineStart; i <= lineEnd; ++i)
        {
        const wxString lineText = editor->GetLine(i);
        int firstCharPos = 0;
        while (firstCharPos < static_cast<int>(lineText.length()) &&
               (lineText[firstCharPos] == L' ' || lineText[firstCharPos] == L'\t'))
            {
            firstCharPos++;
            }
        if (lineText.Mid(firstCharPos).StartsWith(L"--"))
            {
            editor->DeleteRange(editor->PositionFromLine(i) + firstCharPos, 2);
            }
        }
    editor->EndUndoAction();
    }

//-------------------------------------------------------
void ScriptWorkbenchPanel::Cut()
    {
    if (GetCurrentEditor() != nullptr)
        {
        GetCurrentEditor()->Cut();
        }
    }

//-------------------------------------------------------
void ScriptWorkbenchPanel::Copy()
    {
    if (GetCurrentEditor() != nullptr)
        {
        GetCurrentEditor()->Copy();
        }
    }

//-------------------------------------------------------
void ScriptWorkbenchPanel::Paste()
    {
    if (GetCurrentEditor() != nullptr)
        {
        GetCurrentEditor()->Paste();
        }
    }

//-------------------------------------------------------
void ScriptWorkbenchPanel::SelectAll()
    {
    if (GetCurrentEditor() != nullptr)
        {
        GetCurrentEditor()->SelectAll();
        }
    }

//-------------------------------------------------------
bool ScriptWorkbenchPanel::CanCut() const
    {
    return GetCurrentEditor() != nullptr &&
           GetCurrentEditor()->GetSelectionStart() != GetCurrentEditor()->GetSelectionEnd();
    }

//-------------------------------------------------------
bool ScriptWorkbenchPanel::CanCopy() const
    {
    return GetCurrentEditor() != nullptr &&
           GetCurrentEditor()->GetSelectionStart() != GetCurrentEditor()->GetSelectionEnd();
    }

//-------------------------------------------------------
bool ScriptWorkbenchPanel::CanPaste() const
    {
    return GetCurrentEditor() != nullptr && GetCurrentEditor()->CanPaste();
    }

//-------------------------------------------------------
bool ScriptWorkbenchPanel::CanUndo() const
    {
    return GetCurrentEditor() != nullptr && GetCurrentEditor()->CanUndo();
    }

//-------------------------------------------------------
bool ScriptWorkbenchPanel::CanRedo() const
    {
    return GetCurrentEditor() != nullptr && GetCurrentEditor()->CanRedo();
    }

//-------------------------------------------------------
void ScriptWorkbenchPanel::OnFindDialog(wxFindDialogEvent& event)
    {
    auto* currentScript = GetCurrentEditor();
    if (currentScript == nullptr)
        {
        return;
        }

    if (event.GetEventType() == wxEVT_FIND || event.GetEventType() == wxEVT_FIND_NEXT)
        {
        currentScript->OnFind(event);
        }
    else if (event.GetEventType() == wxEVT_FIND_REPLACE)
        {
        long from{ 0 }, to{ 0 };
        currentScript->GetSelection(&from, &to);
        currentScript->SetSelection(from, from);
        currentScript->SearchAnchor();

        auto foundPos = currentScript->FindNext(event.GetFindString(), event.GetFlags());
        if (foundPos != wxSTC_INVALID_POSITION)
            {
            if (from == foundPos && std::cmp_equal(to, foundPos + event.GetFindString().length()))
                {
                currentScript->Replace(foundPos, foundPos + event.GetFindString().length(),
                                       event.GetReplaceString());
                currentScript->SetSelection(foundPos, foundPos + event.GetReplaceString().length());
                currentScript->SearchAnchor();
                foundPos = currentScript->FindNext(event.GetFindString(), event.GetFlags());
                if (foundPos != wxSTC_INVALID_POSITION)
                    {
                    currentScript->SetSelection(foundPos,
                                                foundPos + event.GetFindString().length());
                    }
                }
            else
                {
                currentScript->SetSelection(foundPos, foundPos + event.GetFindString().length());
                currentScript->SearchAnchor();
                }
            }
        else
            {
            currentScript->SetSelection(from, to);
            currentScript->SearchAnchor();
            wxMessageBox(_(L"No further occurrences found."), _(L"Item Not Found"),
                         wxOK | wxICON_INFORMATION, this);
            }
        }
    else if (event.GetEventType() == wxEVT_FIND_REPLACE_ALL)
        {
        currentScript->SetSelection(0, 0);
        currentScript->SearchAnchor();
        auto foundPos = currentScript->FindNext(event.GetFindString(), event.GetFlags(), false);
        while (foundPos != wxSTC_INVALID_POSITION)
            {
            currentScript->Replace(foundPos, foundPos + event.GetFindString().length(),
                                   event.GetReplaceString());
            currentScript->SetSelection(foundPos + event.GetReplaceString().length(),
                                        foundPos + event.GetReplaceString().length());
            currentScript->SearchAnchor();
            foundPos = currentScript->FindNext(event.GetFindString(), event.GetFlags(), false);
            }
        }
    else if (event.GetEventType() == wxEVT_FIND_CLOSE)
        {
        if (m_dlgReplace != nullptr)
            {
            m_dlgReplace->Destroy();
            m_dlgReplace = nullptr;
            }
        if (m_dlgFind != nullptr)
            {
            m_dlgFind->Destroy();
            m_dlgFind = nullptr;
            }
        }
    }

//-------------------------------------------------------
void ScriptWorkbenchPanel::SetThemeColor(const wxColour& color)
    {
    if (!color.IsOk())
        {
        return;
        }

    SetBackgroundColour(color);

    // re-render the debug window with a body bg matching the theme
    if (m_debugMessageWindow != nullptr)
        {
        const auto debugReportBody =
            wxString::Format(
                L"<!DOCTYPE html>\n<html>\n<body "
                L"style=\"background-color:%s;color:%s;font-family:sans-serif;font-size:11pt;"
                L"margin:10px;\">",
                color.GetAsString(wxC2S_HTML_SYNTAX),
                Wisteria::Colors::ColorContrast::BlackOrWhiteContrast(color).GetAsString(
                    wxC2S_HTML_SYNTAX)) +
            m_debugContent + L"\n</body>\n</html>";
        m_debugMessageWindow->SetPage(debugReportBody, wxString{});
        }

    Refresh();
    }

//-------------------------------------------------------
void ScriptWorkbenchPanel::DebugOutput(const wxString& str)
    {
    if (m_debugMessageWindow == nullptr)
        {
        return;
        }
    m_debugContent += L"\n<br />" + str;

    const wxColour bkColor = GetBackgroundColour();
    const auto debugReportBody =
        wxString::Format(L"<!DOCTYPE html>\n<html>\n<body "
                         L"style=\"background-color:%s;color:%s;font-family:sans-serif;font-size:"
                         L"11pt;margin:10px;\">",
                         bkColor.GetAsString(wxC2S_HTML_SYNTAX),
                         Wisteria::Colors::ColorContrast::BlackOrWhiteContrast(bkColor).GetAsString(
                             wxC2S_HTML_SYNTAX)) +
        m_debugContent + L"\n</body>\n</html>";
    m_debugMessageWindow->SetPage(debugReportBody, wxString{});
    }

//-------------------------------------------------------
void ScriptWorkbenchPanel::DebugClear()
    {
    if (m_debugMessageWindow == nullptr)
        {
        return;
        }
    // TRANSLATORS: %d placeholders are the major, minor, and release version numbers of Lua
    m_debugContent = wxString::Format(_(L"Lua version %d.%d.%d, ready..."), LUA_VERSION_MAJOR_N,
                                      LUA_VERSION_MINOR_N, LUA_VERSION_RELEASE_N);

    const wxColour bkColor = GetBackgroundColour();
    const auto debugReportBody =
        wxString::Format(L"<!DOCTYPE html>\n<html>\n<body "
                         L"style=\"background-color:%s;color:%s;font-family:sans-serif;font-size:"
                         L"11pt;margin:10px;\">",
                         bkColor.GetAsString(wxC2S_HTML_SYNTAX),
                         Wisteria::Colors::ColorContrast::BlackOrWhiteContrast(bkColor).GetAsString(
                             wxC2S_HTML_SYNTAX)) +
        m_debugContent + L"\n</body>\n</html>";
    m_debugMessageWindow->SetPage(debugReportBody, wxString{});
    }

//-------------------------------------------------------
bool ScriptWorkbenchPanel::PromptToSaveUnsavedScripts()
    {
    for (auto& entry : m_scripts)
        {
        auto* codeEditor = entry.m_editor;
        if (codeEditor == nullptr || !codeEditor->GetModify())
            {
            continue;
            }
        // surface which script the prompt is about
        m_scriptSidebar->SelectSubItemById(m_scriptsFolderId, entry.m_sidebarId, false, false);
        const wxString name = codeEditor->GetScriptFilePath().empty() ?
                                  wxString{ _(L"(unnamed)") } :
                                  wxFileName{ codeEditor->GetScriptFilePath() }.GetName();
        const int answer = wxMessageBox(wxString::Format(_(L"Save changes to '%s'?"), name),
                                        _(L"Save Script"), wxYES_NO | wxCANCEL | wxICON_QUESTION);
        if (answer == wxCANCEL)
            {
            return false;
            }
        if (answer != wxYES)
            {
            continue;
            }
        if (!codeEditor->Save())
            {
            return false;
            }
        const int idx = m_editorBook->FindPage(entry.m_page);
        if (idx != wxNOT_FOUND)
            {
            m_editorBook->SetPageText(idx, wxFileName{ codeEditor->GetScriptFilePath() }.GetName());
            }
        UpdateDirtyMark(codeEditor);
        }
    return true;
    }

//-------------------------------------------------------
bool ScriptWorkbenchPanel::HasUnsavedOrLoadedScripts() const
    {
    return std::ranges::any_of(m_scripts,
                               [](const auto& entry)
                               {
                                   return entry.m_editor != nullptr &&
                                          (entry.m_editor->GetModify() ||
                                           !entry.m_editor->GetScriptFilePath().empty());
                               });
    }
