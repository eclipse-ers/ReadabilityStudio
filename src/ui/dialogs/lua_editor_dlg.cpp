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

#include "lua_editor_dlg.h"
#include "../../Wisteria-Dataviz/src/base/colorbrewer.h"
#include "../../Wisteria-Dataviz/src/import/html_extract_text.h"
#include "../../app/readability_app.h"
#include <utility>

wxDECLARE_APP(ReadabilityApp);

//-------------------------------------------------------
LuaEditorDlg::LuaEditorDlg(
    wxWindow* parent, wxWindowID id /*= wxID_ANY*/, const wxString& caption /*= _(L"Lua Script")*/,
    const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/,
    long style /*= wxCAPTION | wxCLOSE_BOX | wxMINIMIZE_BOX | wxMAXIMIZE_BOX | wxRESIZE_BORDER*/)
    : wxFrame(parent, id, caption, pos, size,
              ((parent == nullptr) ? style | wxDIALOG_NO_PARENT : style))

    {
    ImportAPI();

    m_mgr.SetManagedWindow(this);

    wxIcon ico;
    ico.CopyFromBitmap(wxGetApp().ReadSvgIcon(wxSystemSettings::GetAppearance().IsDark() ?
                                                  L"ribbon/lua-dark-mode.svg" :
                                                  L"ribbon/lua.svg",
                                              wxSize{ 32, 32 }));
    SetIcon(ico);

    CreateControls();
    Center();

    Bind(wxEVT_CLOSE_WINDOW, &LuaEditorDlg::OnClose, this);

    Bind(
        wxEVT_TOOL,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            if (m_notebook->GetPageCount() == 0)
                {
                return;
                }

            auto* editor = dynamic_cast<Wisteria::UI::CodeEditor*>(m_notebook->GetCurrentPage());
            editor->AnnotationClearAll();
            wxString errorMessage;

            // disable (and later re-enable) the Run button while the script runs
            m_toolbar->EnableTool(XRCID("ID_RUN"), false);
            m_toolbar->EnableTool(XRCID("ID_STOP"), true);

            // run the script
            wxGetApp().GetLuaRunner().RunLuaCode(
                // run either selected text or the whole script
                (editor->GetSelectionStart() == editor->GetSelectionEnd()) ?
                    editor->GetValue() :
                    editor->GetSelectedText(),
                editor->GetScriptFilePath(), errorMessage);

            m_toolbar->EnableTool(XRCID("ID_RUN"), true);
            m_toolbar->EnableTool(XRCID("ID_STOP"), false);
            m_toolbar->Refresh();

            if (!errorMessage.empty())
                {
                const int lineOffset = (editor->GetSelectionStart() == editor->GetSelectionEnd()) ?
                                           0 :
                                           editor->LineFromPosition(editor->GetSelectionStart());

                long lineNumber{ 0 };
                errorMessage.ToLong(&lineNumber);
                // make it zero indexed
                --lineNumber;
                // and offset if we are running a selection instead of the whole script
                lineNumber += lineOffset;

                // strip the number from the message
                if (const auto foundPos = errorMessage.find(L':'); foundPos != wxString::npos)
                    {
                    errorMessage.erase(0, foundPos + 1);
                    }

                wxMessageBox(wxString::Format(
                                 // TRANSLATORS: placeholders are a line number and error message
                                 // from a script
                                 _(L"Line #%s: %s"), std::to_wstring(lineNumber + 1), errorMessage),
                             _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
                if (lineNumber >= 0)
                    {
                    errorMessage.insert(0, _(L"Error:"));
                    // if scrolling up, then step back up an extra line so that we can see it
                    // after adding the annotation beneath it
                    editor->GotoLine((editor->GetFirstVisibleLine() < lineNumber) ? lineNumber :
                                                                                    lineNumber - 1);
                    editor->AnnotationSetText(lineNumber, errorMessage);
                    editor->AnnotationSetStyle(lineNumber,
                                               Wisteria::UI::CodeEditor::ERROR_ANNOTATION_STYLE);

                    // Scintilla doesn't update the scroll width for annotations, even with
                    // scroll width tracking on, so do it manually.
                    const int width = editor->GetScrollWidth();

                    // Take into account the fact that the annotation is shown indented, with
                    // the same indent as the line it's attached to.
                    // Also, add 3; this is just a hack to account for the width of the box, there
                    // doesn't seem to be any way to get it directly from Scintilla.
                    const unsigned long indent =
                        editor->GetLineIndentation(lineNumber) + FromDIP(3);

                    const int widthAnn =
                        editor->TextWidth(Wisteria::UI::CodeEditor::ERROR_ANNOTATION_STYLE,
                                          errorMessage + wxString{ indent, L' ' });

                    if (widthAnn > width)
                        {
                        editor->SetScrollWidth(widthAnn);
                        }
                    }
                }
        },
        XRCID("ID_RUN"));

    Bind(
        wxEVT_TOOL,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            m_toolbar->EnableTool(XRCID("ID_RUN"), true);
            m_toolbar->EnableTool(XRCID("ID_STOP"), false);
            m_toolbar->Refresh();
            LuaInterpreter::Quit();
        },
        XRCID("ID_STOP"));

    Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSE,
         [this]([[maybe_unused]] wxCommandEvent&)
         {
             if (m_notebook->GetPageCount() == 0)
                 {
                 return;
                 }

             auto* codeEditor =
                 dynamic_cast<Wisteria::UI::CodeEditor*>(m_notebook->GetCurrentPage());
             if (codeEditor && codeEditor->GetModify())
                 {
                 if (wxMessageBox(_(L"Do you wish to save your unsaved changes?"),
                                  _(L"Save Script"), wxYES_NO | wxICON_QUESTION) == wxYES)
                     {
                     codeEditor->Save();
                     }
                 }
         });

    Bind(
        wxEVT_TOOL,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            m_notebook->Freeze();
            m_notebook->AddPage(
                CreateLuaScript(m_notebook), _(L"(unnamed)"), true,
                wxGetApp().GetResourceManager().GetSVG(wxSystemSettings::GetAppearance().IsDark() ?
                                                           L"ribbon/lua-dark-mode.svg" :
                                                           L"ribbon/lua.svg"));
            m_notebook->Thaw();
        },
        XRCID("ID_NEW"));

    Bind(
        wxEVT_TOOL,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxFileDialog dialogOpen(this, _(L"Select Script to Open"), wxEmptyString, wxEmptyString,
                                    _(L"Lua Scripts (*.lua)|*.lua"),
                                    wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
            if (dialogOpen.ShowModal() != wxID_OK)
                {
                return;
                }
            const wxString filePath = dialogOpen.GetPath();

            auto* scriptCtrl = CreateLuaScript(m_notebook);
            if (scriptCtrl != nullptr)
                {
                // Just the generic default page open and nothing in it? Then get rid of it
                // now that we are opening an existing script.
                if (m_notebook->GetPageCount() == 1)
                    {
                    auto* codeEditor =
                        dynamic_cast<Wisteria::UI::CodeEditor*>(m_notebook->GetCurrentPage());
                    if (codeEditor && !codeEditor->GetModify() &&
                        codeEditor->GetScriptFilePath().empty())
                        {
                        m_notebook->DeletePage(0);
                        }
                    }
                scriptCtrl->LoadFile(filePath);
                scriptCtrl->SetSelection(scriptCtrl->GetTextLength(), scriptCtrl->GetTextLength());
                scriptCtrl->SetScriptFilePath(filePath);

                m_notebook->AddPage(scriptCtrl, wxFileName(filePath).GetName(), true,
                                    wxGetApp().GetResourceManager().GetSVG(
                                        wxSystemSettings::GetAppearance().IsDark() ?
                                            L"ribbon/lua-dark-mode.svg" :
                                            L"ribbon/lua.svg"));
                scriptCtrl->SetFocus();
                }
        },
        XRCID("ID_OPEN"));

    Bind(wxEVT_TOOL, &LuaEditorDlg::OnSave, this, XRCID("ID_SAVE"));

    Bind(
        wxEVT_TOOL,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            m_mgr.GetPane(L"funcbrowser").Show(!m_mgr.GetPane(L"funcbrowser").IsShown());
            m_mgr.Update();
        },
        XRCID("ID_FUNCTION_BROWSER"));

    Bind(wxEVT_TOOL, [this]([[maybe_unused]] wxCommandEvent&) { DebugClear(); }, XRCID("ID_CLEAR"));

    Bind(
        wxEVT_TOOL,
        []([[maybe_unused]] wxCommandEvent&)
        {
            const wxString manualPath = wxGetApp().GetMainFrameEx()->GetHelpDirectory() +
                                        wxFileName::GetPathSeparator() +
                                        _DT(L"readability-studio-api.pdf");
            if (wxFile::Exists(manualPath))
                {
                wxLaunchDefaultApplication(manualPath);
                }
        },
        wxID_HELP);

    Bind(
        wxEVT_TOOL,
        []([[maybe_unused]] wxCommandEvent&)
        {
            const wxString helpPath = wxGetApp().GetMainFrameEx()->GetHelpDirectory() +
                                      _DT(L"/lua-5.4/doc/contents.html");
            if (wxFile::Exists(helpPath))
                {
                wxLaunchDefaultBrowser(wxFileName::FileNameToURL(helpPath));
                }
        },
        XRCID("LUA_REFERENCE"));

    Bind(wxEVT_SEARCH,
         [this](wxCommandEvent& evt)
         {
             auto* codeEditor =
                 dynamic_cast<Wisteria::UI::CodeEditor*>(m_notebook->GetCurrentPage());
             if (codeEditor)
                 {
                 codeEditor->FindNext(evt.GetString());
                 }
         });

    Bind(wxEVT_TOOL, &LuaEditorDlg::OnShowFindDialog, this, wxID_FIND);
    Bind(wxEVT_TOOL, &LuaEditorDlg::OnShowReplaceDialog, this, wxID_REPLACE);
    Bind(wxEVT_FIND, &LuaEditorDlg::OnFindDialog, this);
    Bind(wxEVT_FIND_NEXT, &LuaEditorDlg::OnFindDialog, this);
    Bind(wxEVT_FIND_REPLACE, &LuaEditorDlg::OnFindDialog, this);
    Bind(wxEVT_FIND_REPLACE_ALL, &LuaEditorDlg::OnFindDialog, this);
    Bind(wxEVT_FIND_CLOSE, &LuaEditorDlg::OnFindDialog, this);

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
            event.Skip(true);
        },
        wxID_ANY);

    wxAcceleratorEntry accelEntries[6];
    accelEntries[0].Set(wxACCEL_CMD, static_cast<int>(L'N'), XRCID("ID_NEW"));
    accelEntries[1].Set(wxACCEL_CMD, static_cast<int>(L'O'), XRCID("ID_OPEN"));
    accelEntries[2].Set(wxACCEL_CMD, static_cast<int>(L'S'), XRCID("ID_SAVE"));
    accelEntries[3].Set(wxACCEL_CMD, static_cast<int>(L'F'), wxID_FIND);
    accelEntries[4].Set(wxACCEL_CMD, static_cast<int>(L'H'), wxID_REPLACE);
    accelEntries[5].Set(wxACCEL_NORMAL, WXK_F5, XRCID("ID_RUN"));
    const wxAcceleratorTable accelTable(std::size(accelEntries), accelEntries);
    wxWindowBase::SetAcceleratorTable(accelTable);
    }

//------------------------------------------------------
void LuaEditorDlg::ImportAPI()
    {
    // import API info
    std::vector<std::vector<std::wstring>> apiStrings;

    lily_of_the_valley::standard_delimited_character_column tabbedColumn(
        lily_of_the_valley::text_column_delimited_character_parser{ L'\t' }, 1);
    lily_of_the_valley::standard_delimited_character_column semiColonColumn(
        lily_of_the_valley::text_column_delimited_character_parser{ L';' }, std::nullopt);
    lily_of_the_valley::text_row<std::wstring> row(std::nullopt);
    row.treat_consecutive_delimiters_as_one(true); // skip consecutive semicolons
    row.add_column(tabbedColumn);
    row.add_column(semiColonColumn);
    row.allow_column_resizing(true);

    lily_of_the_valley::text_matrix<std::wstring> importer(&apiStrings);
    importer.add_row_definition(
        lily_of_the_valley::text_row<std::wstring>(1)); // skip warning in first line
    importer.add_row_definition(row);

    lily_of_the_valley::text_preview preview;

    // library/enum file
    wxString libraryText;
    wxString libFilePath = wxGetApp().FindResourceFile(L"rs-classes.api");
    if (Wisteria::TextStream::ReadFile(libFilePath, libraryText))
        {
        apiStrings.clear();

        // see how many lines are in the file and resize the container
        const size_t rowCount = preview(libraryText, L'\t', true, false);
        if (rowCount > 0)
            {
            apiStrings.resize(rowCount);
            importer.read(libraryText, rowCount, 2, true);

            for (auto& lib : apiStrings)
                {
                if (!lib.empty())
                    {
                    const std::wstring libName = lib.front();
                    lib.erase(lib.begin(), lib.begin() + 1);
                    Wisteria::UI::CodeEditor::NameList nl;
                    for (const auto& className : lib)
                        {
                        nl.insert(className);
                        }
                    m_classes.emplace_back(libName, nl);
                    }
                }
            }
        }

    apiStrings.clear();
    libFilePath = wxGetApp().FindResourceFile(L"rs-libraries.api");
    if (Wisteria::TextStream::ReadFile(libFilePath, libraryText))
        {
        // see how many lines are in the file and resize the container
        const size_t rowCount = preview(libraryText, L'\t', true, false);
        if (rowCount > 0)
            {
            apiStrings.resize(rowCount);
            importer.read(libraryText, rowCount, 2, true);

            for (auto& lib : apiStrings)
                {
                if (!lib.empty())
                    {
                    const std::wstring libName = lib.front();
                    lib.erase(lib.begin(), lib.begin() + 1);
                    Wisteria::UI::CodeEditor::NameList nl;
                    for (const auto& lName : lib)
                        {
                        nl.insert(lName);
                        }
                    m_libraries.emplace_back(libName, nl);
                    }
                }
            }
        }

    apiStrings.clear();
    libFilePath = wxGetApp().FindResourceFile(L"rs-enums.api");
    if (Wisteria::TextStream::ReadFile(libFilePath, libraryText))
        {
        // see how many lines are in the file and resize the container
        const size_t rowCount = preview(libraryText, L'\t', true, false);
        if (rowCount > 0)
            {
            apiStrings.resize(rowCount);
            importer.read(libraryText, rowCount, 2, true);

            for (auto& lib : apiStrings)
                {
                if (!lib.empty())
                    {
                    const std::wstring libName = lib.front();
                    lib.erase(lib.begin(), lib.begin() + 1);
                    Wisteria::UI::CodeEditor::NameList nl;
                    for (const auto& lName : lib)
                        {
                        nl.insert(lName);
                        }
                    m_enums.emplace_back(libName, nl);
                    }
                }
            }
        }
    }

//------------------------------------------------------
void LuaEditorDlg::OnSave([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_notebook->GetPageCount() == 0)
        {
        return;
        }

    if (dynamic_cast<Wisteria::UI::CodeEditor*>(m_notebook->GetCurrentPage())->Save())
        {
        m_notebook->SetPageText(
            m_notebook->GetSelection(),
            wxFileName(dynamic_cast<Wisteria::UI::CodeEditor*>(m_notebook->GetCurrentPage())
                           ->GetScriptFilePath())
                .GetName());
        }
    }

//------------------------------------------------------
void LuaEditorDlg::OnShowReplaceDialog([[maybe_unused]] wxCommandEvent& event)
    {
    auto* currentScript = dynamic_cast<Wisteria::UI::CodeEditor*>(m_notebook->GetCurrentPage());
    if (currentScript != nullptr && m_findData.GetFindString().empty())
        {
        m_findData.SetFindString(currentScript->GetSelectedText());
        }

    // get rid of Find dialog (if it was opened)
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

//------------------------------------------------------
void LuaEditorDlg::OnShowFindDialog([[maybe_unused]] wxCommandEvent& event)
    {
    auto* currentScript = dynamic_cast<Wisteria::UI::CodeEditor*>(m_notebook->GetCurrentPage());
    if (currentScript != nullptr && m_findData.GetFindString().empty())
        {
        m_findData.SetFindString(currentScript->GetSelectedText());
        }

    // get rid of Replace dialog (if it was opened)
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

//------------------------------------------------------
void LuaEditorDlg::OnFindDialog(wxFindDialogEvent& event)
    {
    if (m_notebook->GetPageCount() == 0)
        {
        return;
        }

    auto* currentScript = dynamic_cast<Wisteria::UI::CodeEditor*>(m_notebook->GetCurrentPage());
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
        // force search to start from beginning of selection
        currentScript->SetSelection(from, from);
        currentScript->SearchAnchor();

        auto foundPos = currentScript->FindNext(event.GetFindString(), event.GetFlags());
        if (foundPos != wxSTC_INVALID_POSITION)
            {
            // if what is being replaced matches what was already selected, then replace it
            if (from == foundPos && std::cmp_equal(to, foundPos + event.GetFindString().length()))
                {
                currentScript->Replace(foundPos, foundPos + event.GetFindString().length(),
                                       event.GetReplaceString());
                currentScript->SetSelection(foundPos, foundPos + event.GetReplaceString().length());
                currentScript->SearchAnchor();
                // ...then, find the next occurrence of string being replaced for the next replace
                // button click
                foundPos = currentScript->FindNext(event.GetFindString(), event.GetFlags());
                if (foundPos != wxSTC_INVALID_POSITION)
                    {
                    currentScript->SetSelection(foundPos,
                                                foundPos + event.GetFindString().length());
                    }
                }
            // ...otherwise, just select the next string being replaced so that user can see it in
            // its context and then decide on the next replace button click if they want to replace
            // it
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
            // ...then, find the next occurrence of string being replaced for the next loop
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
void LuaEditorDlg::SetThemeColor(const wxColour& color)
    {
    if (!color.IsOk())
        {
        return;
        }

    m_mgr.GetArtProvider()->SetColour(wxAUI_DOCKART_BACKGROUND_COLOUR, color);

    // notebook (and its children edit windows)
    auto* notebookArt = new wxAuiDefaultTabArt;
    notebookArt->SetColour(color);
    m_notebook->SetArtProvider(notebookArt);

    const wxString htmlText = *(m_debugMessageWindow->GetParser()->GetSource());
    const auto debugReportBody =
        wxString::Format(L"<html>\n<body bgcolor=%s text=%s>", color.GetAsString(wxC2S_HTML_SYNTAX),
                         Wisteria::Colors::ColorContrast::BlackOrWhiteContrast(color).GetAsString(
                             wxC2S_HTML_SYNTAX)) +
        wxString{ lily_of_the_valley::html_extract_text::get_body(htmlText.wc_str()) } +
        L"\n</body>\n</html>";
    m_debugMessageWindow->SetPage(debugReportBody);

    m_mgr.Update();
    }

//-------------------------------------------------------
void LuaEditorDlg::DebugOutput(const wxString& str)
    {
    if (m_debugMessageWindow != nullptr)
        {
        const wxColour bkColor = m_mgr.GetArtProvider()->GetColour(wxAUI_DOCKART_BACKGROUND_COLOUR);
        const wxString htmlText = wxString{ lily_of_the_valley::html_extract_text::get_body(
                                      m_debugMessageWindow->GetParser()->GetSource()->wc_str()) } +
                                  L"\n<br />" + str;
        const auto debugReportBody =
            wxString::Format(
                L"<html>\n<body bgcolor=%s text=%s>", bkColor.GetAsString(wxC2S_HTML_SYNTAX),
                Wisteria::Colors::ColorContrast::BlackOrWhiteContrast(bkColor).GetAsString(
                    wxC2S_HTML_SYNTAX)) +
            htmlText + L"\n</body>\n</html>";
        m_debugMessageWindow->SetPage(debugReportBody);
        }
    }

//-------------------------------------------------------
void LuaEditorDlg::DebugClear()
    {
    if (m_debugMessageWindow != nullptr)
        {
        const wxColour bkColor = m_mgr.GetArtProvider()->GetColour(wxAUI_DOCKART_BACKGROUND_COLOUR);
        const auto debugReportBody = wxString::Format(
            L"<html>\n<body bgcolor=%s text=%s>\n</body>\n</html>",
            bkColor.GetAsString(wxC2S_HTML_SYNTAX),
            Wisteria::Colors::ColorContrast::BlackOrWhiteContrast(bkColor).GetAsString(
                wxC2S_HTML_SYNTAX));
        m_debugMessageWindow->SetPage(debugReportBody);
        }
    }

//-------------------------------------------------------
Wisteria::UI::CodeEditor* LuaEditorDlg::CreateLuaScript(wxWindow* parent)
    {
    auto* codeEditor = new Wisteria::UI::CodeEditor(parent, wxSTC_LEX_LUA);
    codeEditor->Show(false);
    codeEditor->IncludeNumberMargin(true);
    codeEditor->IncludeFoldingMargin(true);
    codeEditor->SetDefaultHeader(
        L"-- " +
        // TRANSLATORS: %s is the program name
        wxString::Format(_(L"Imports %s specific enumerations"), wxGetApp().GetAppDisplayName()) +
        L"\ndofile(Application.GetLuaConstantsPath())\n\n");
    codeEditor->SetText(codeEditor->GetDefaultHeader());
    codeEditor->SetModified(false);

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
    codeEditor->Show();

    return codeEditor;
    }

//-------------------------------------------------------
void LuaEditorDlg::CreateControls()
    {
    m_toolbar = new wxAuiToolBar(this, wxID_ANY);
    m_toolbar->SetToolBitmapSize(FromDIP(wxSize{ 16, 16 }));
    // wxID_NEW and such trigger parent events
    m_toolbar->AddTool(XRCID("ID_NEW"), _(L"New"),
                       wxArtProvider::GetBitmapBundle(wxART_NEW, wxART_BUTTON),
                       // TRANSLATORS: A programming script.
                       _(L"Create a new script."));
    m_toolbar->AddTool(XRCID("ID_OPEN"), _(L"Open"),
                       wxArtProvider::GetBitmapBundle(wxART_FILE_OPEN, wxART_BUTTON),
                       // TRANSLATORS: A programming script.
                       _(L"Open a script."));
    m_toolbar->AddTool(XRCID("ID_SAVE"), _(L"Save"),
                       wxArtProvider::GetBitmapBundle(wxART_FILE_SAVE, wxART_BUTTON),
                       // TRANSLATORS: A programming script.
                       _(L"Save the script."));
    m_toolbar->AddTool(XRCID("ID_RUN"), _(L"Run"),
                       wxArtProvider::GetBitmapBundle(L"ID_RUN", wxART_BUTTON),
                       _(L"Execute the script (or selection)."));
    m_toolbar->AddTool(XRCID("ID_STOP"), _(L"Stop"),
                       wxArtProvider::GetBitmapBundle(wxART_QUIT, wxART_BUTTON),
                       _(L"Stop the currently running script."));
    m_toolbar->EnableTool(XRCID("ID_STOP"), false);
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(wxID_FIND, _(L"Find"),
                       wxArtProvider::GetBitmapBundle(wxART_FIND, wxART_BUTTON), _(L"Find text."));
    m_toolbar->AddTool(wxID_REPLACE, _(L"Replace"),
                       wxArtProvider::GetBitmapBundle(wxART_FIND_AND_REPLACE, wxART_BUTTON),
                       _(L"Replace text."));
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(XRCID("ID_CLEAR"), _(L"Clear"),
                       wxArtProvider::GetBitmapBundle(L"ID_CLEAR", wxART_BUTTON),
                       _(L"Clear the log window."));
    m_toolbar->AddSeparator();
    m_toolbar->AddTool(XRCID("ID_FUNCTION_BROWSER"), _(L"Function Browser"),
                       wxArtProvider::GetBitmapBundle(L"ID_FUNCTION", wxART_BUTTON),
                       _(L"View the function browser."));
    m_toolbar->AddTool(wxID_HELP, _(L"Content"),
                       wxArtProvider::GetBitmapBundle(wxART_HELP_BOOK, wxART_BUTTON),
                       _(L"View the documentation."));
    m_toolbar->AddTool(XRCID("LUA_REFERENCE"), _(L"Lua Reference"),
                       wxArtProvider::GetBitmapBundle(L"ID_E_HELP", wxART_BUTTON),
                       _(L"View the Lua Reference Manual."));

    m_toolbar->Realize();
    m_mgr.AddPane(m_toolbar, wxAuiPaneInfo()
                                 .Name(L"auitoolbar")
                                 .Caption(_(L"Tools"))
                                 .ToolbarPane()
                                 .Top()
                                 .CloseButton(false)
                                 .Fixed());

    m_notebook = new wxAuiNotebook(this, wxID_ANY, wxPoint{ 0, 0 }, FromDIP(wxSize{ 400, 200 }),
                                   wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE |
                                       wxAUI_NB_SCROLL_BUTTONS | wxAUI_NB_CLOSE_ON_ALL_TABS |
                                       wxAUI_NB_MIDDLE_CLICK_CLOSE | wxAUI_NB_TAB_EXTERNAL_MOVE |
                                       wxNO_BORDER);

    m_notebook->AddPage(
        CreateLuaScript(m_notebook), _(L"(unnamed)"), true,
        wxGetApp().GetResourceManager().GetSVG(wxSystemSettings::GetAppearance().IsDark() ?
                                                   L"ribbon/lua-dark-mode.svg" :
                                                   L"ribbon/lua.svg"));

    m_mgr.AddPane(m_notebook, wxAuiPaneInfo().Name(L"auinotebook").CenterPane().PaneBorder(false));

    m_functionBrowser = new Wisteria::UI::FunctionBrowserCtrl(this, this);
    wxGetApp().UpdateSideBarTheme(m_functionBrowser->GetSidebar());
    m_functionBrowser->SetParameterSeparator(FormulaFormat::GetListSeparator());
    m_functionBrowser->AddCategory(L"Libraries", 1000);
    m_functionBrowser->AddCategory(L"Classes", 1001);
    m_functionBrowser->AddCategory(L"Enumerations", 1002);
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
    m_mgr.AddPane(m_functionBrowser, wxAuiPaneInfo()
                                         .Name(L"funcbrowser")
                                         .Right()
                                         .MinimizeButton(true)
                                         .MaximizeButton(true)
                                         .Caption(_(L"Function Browser"))
                                         .FloatingSize(FromDIP(wxSize{ 800, 800 }))
                                         .BestSize(FromDIP(wxSize{ 800, 800 }))
                                         .PinButton(true)
                                         .CloseButton(true)
                                         .Hide());

    m_debugMessageWindow = new wxHtmlWindow(this);
    m_mgr.AddPane(m_debugMessageWindow, wxAuiPaneInfo()
                                            .Name(L"auidebug")
                                            .Bottom()
                                            .MinimizeButton(true)
                                            .MaximizeButton(true)
                                            .Caption(_(L"Debug Output"))
                                            .FloatingSize(FromDIP(wxSize{ 800, 200 }))
                                            .BestSize(FromDIP(wxSize{ 800, 100 }))
                                            .PinButton(true)
                                            .CloseButton(false));

    SetSize(FromDIP(wxSize{ 800, 800 }));

    m_mgr.Update();
    }

//-------------------------------------------------------
void LuaEditorDlg::OnClose([[maybe_unused]] wxCloseEvent& event)
    {
    // ask about any unsaved changes
    for (size_t i = 0; i < m_notebook->GetPageCount(); ++i)
        {
        auto* codeEditor = dynamic_cast<Wisteria::UI::CodeEditor*>(m_notebook->GetPage(i));
        if ((codeEditor != nullptr) && codeEditor->GetModify() &&
            (wxMessageBox(_(L"Do you wish to save your unsaved changes?"), _(L"Save Script"),
                          wxYES_NO | wxICON_QUESTION) == wxYES))
            {
            if (codeEditor->Save())
                {
                m_notebook->SetPageText(i, wxFileName{ codeEditor->GetScriptFilePath() }.GetName());
                }
            }
        }

    // this frame is meant to be modeless, so just hide it when closing and let
    // the parent app clean it up on app exit
    HideAllWindows();
    }
