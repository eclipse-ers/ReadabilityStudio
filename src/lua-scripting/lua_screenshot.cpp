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

#include "lua_screenshot.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/archivedlg.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/getdirdlg.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/gridexportdlg.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/listctrlitemviewdlg.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/listctrlsortdlg.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/radioboxdlg.h"
#include "../Wisteria-Dataviz/src/util/screenshot.h"
#include "../app/readability_app.h"
#include "../projects/base_project.h"
#include "../projects/batch_project_doc.h"
#include "../ui/dialogs/custom_test_dlg.h"
#include "../ui/dialogs/doc_group_select_dlg.h"
#include "../ui/dialogs/edit_text_dlg.h"
#include "../ui/dialogs/edit_word_list_dlg.h"
#include "../ui/dialogs/filtered_text_preview_dlg.h"
#include "../ui/dialogs/project_wizard_dlg.h"
#include "../ui/dialogs/test_bundle_dlg.h"
#include "../ui/dialogs/tools_options_dlg.h"
#include "../ui/dialogs/web_harvester_dlg.h"
#include "lua_debug.h"
#include <wx/msgdlg.h>

wxDECLARE_APP(ReadabilityApp);

namespace LuaScripting
    {
    // NOLINTBEGIN
    static ToolsOptionsDlg* LuaOptionsDlg{ nullptr };
    static TestBundleDlg* LuaTestBundleDlg{ nullptr };
    static CustomTestDlg* LuaCustomTestDlg{ nullptr };
    static ProjectWizardDlg* LuaStandardProjectWizard{ nullptr };
    static ProjectWizardDlg* LuaBatchProjectWizard{ nullptr };
    static Wisteria::UI::PrinterHeaderFooterDlg* LuaPrinterOptions{ nullptr };
    static Wisteria::UI::GridExportDlg* LuaListExportDlg{ nullptr };
    static Wisteria::UI::ListCtrlItemViewDlg* LuaListViewItemDlg{ nullptr };
    static Wisteria::UI::GetDirFilterDialog* LuaGetDirDlg{ nullptr };
    static Wisteria::UI::ArchiveDlg* LuaGetArchiveDlg{ nullptr };
    static Wisteria::UI::RadioBoxDlg* LuaSelectProjectType{ nullptr };
    static WebHarvesterDlg* LuaWebHarvesterDlg{ nullptr };
    static FilteredTextPreviewDlg* LuaFilteredTextPreviewDlg{ nullptr };
    static Wisteria::UI::ListCtrlSortDlg* LuaListCtrlSortDlg{ nullptr };
    static EditWordListDlg* LuaEditWordListDlg{ nullptr };
    static DocGroupSelectDlg* LuaDocGroupSelectDlg{ nullptr };
    static EditTextDlg* LuaEditTextDlg{ nullptr };

    // NOLINTEND

    //-------------------------------------------------------------
    int ShowScriptEditor(lua_State* L)
        {
        if (wxGetApp().GetMainFrameEx()->GetLuaEditor() != nullptr)
            {
            wxGetApp().GetMainFrameEx()->GetLuaEditor()->Show(
                lua_gettop(L) > 0 ? int_to_bool(lua_toboolean(L, 1)) : true);
            }
        return 1;
        }

    //-------------------------------------------------------------
    int ConvertImage(lua_State* L)
        {
        if (!VerifyParameterCount(L, 3, __func__))
            {
            return 0;
            }

        lua_pushboolean(L, static_cast<int>(Screenshot::ConvertImageToPng(
                               wxString{ luaL_checkstring(L, 1), wxConvUTF8 },
                               wxSize(lua_tonumber(L, 2), lua_tonumber(L, 3)), true)));
        return 1;
        }

    //-------------------------------------------------------------
    int SnapScreenshot(lua_State* L)
        {
        // wait for any changes in the UI to finish updating
        ::wxSleep(2);
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        int startWindowToHighlight = wxID_ANY, endWindowToHighlight = wxID_ANY,
            cutOffWindow = wxID_ANY;
        if (lua_gettop(L) > 1)
            {
            const wxWindowID wId = lua_tonumber(L, 2);
            if (wId != wxID_ANY)
                {
                auto idPos = ReadabilityApp::GetDynamicIdMap().find(wId);
                if (idPos != ReadabilityApp::GetDynamicIdMap().cend())
                    {
                    startWindowToHighlight = idPos->second;
                    }
                else
                    {
                    startWindowToHighlight = lua_tonumber(L, 2);
                    }
                }
            }
        if (lua_gettop(L) > 2)
            {
            const wxWindowID wId = lua_tonumber(L, 3);
            if (wId != wxID_ANY)
                {
                auto idPos = ReadabilityApp::GetDynamicIdMap().find(wId);
                if (idPos != ReadabilityApp::GetDynamicIdMap().cend())
                    {
                    endWindowToHighlight = idPos->second;
                    }
                else
                    {
                    endWindowToHighlight = lua_tonumber(L, 3);
                    }
                }
            }
        if (lua_gettop(L) > 3)
            {
            const wxWindowID wId = lua_tonumber(L, 4);
            if (wId != wxID_ANY)
                {
                auto idPos = ReadabilityApp::GetDynamicIdMap().find(wId);
                if (idPos != ReadabilityApp::GetDynamicIdMap().cend())
                    {
                    cutOffWindow = idPos->second;
                    }
                else
                    {
                    cutOffWindow = lua_tonumber(L, 4);
                    }
                }
            }
        lua_pushboolean(L, static_cast<int>(Screenshot::SaveScreenshot(
                               wxString{ luaL_checkstring(L, 1), wxConvUTF8 },
                               startWindowToHighlight, endWindowToHighlight, cutOffWindow)));
        return 1;
        }

    //-------------------------------------------------------------
    int SnapScreenshotWithAnnotation(lua_State* L)
        {
        ::wxSleep(2);
        if (!VerifyParameterCount(L, 3, __func__))
            {
            return 0;
            }

        int startWindowToHighlight = wxID_ANY, endWindowToHighlight = wxID_ANY;

        auto idPos = ReadabilityApp::GetDynamicIdMap().find(lua_tonumber(L, 3));
        if (idPos != ReadabilityApp::GetDynamicIdMap().cend())
            {
            startWindowToHighlight = idPos->second;
            }
        else
            {
            startWindowToHighlight = lua_tonumber(L, 3);
            }

        if (lua_gettop(L) > 3)
            {
            auto endIdPos = ReadabilityApp::GetDynamicIdMap().find(lua_tonumber(L, 4));
            if (endIdPos != ReadabilityApp::GetDynamicIdMap().cend())
                {
                endWindowToHighlight = endIdPos->second;
                }
            else
                {
                endWindowToHighlight = lua_tonumber(L, 4);
                }
            }

        lua_pushboolean(L, static_cast<int>(Screenshot::SaveScreenshot(
                               wxString{ luaL_checkstring(L, 1), wxConvUTF8 },
                               wxString{ luaL_checkstring(L, 2), wxConvUTF8 },
                               startWindowToHighlight, endWindowToHighlight)));
        return 1;
        }

    //-------------------------------------------------------------
    int SnapScreenshotOfActiveProject(lua_State* L)
        {
        ::wxSleep(2);
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        const wxString path{ luaL_checkstring(L, 1), wxConvUTF8 };

        wxDocument* currentDoc =
            wxGetApp().GetMainFrame()->GetDocumentManager()->GetCurrentDocument();

        if ((currentDoc != nullptr) && (currentDoc->IsKindOf(wxCLASSINFO(ProjectDoc)) ||
                                        currentDoc->IsKindOf(wxCLASSINFO(BatchProjectDoc))))
            {
            auto* project = dynamic_cast<BaseProjectDoc*>(currentDoc);
            auto* firstView = project->GetFirstView();
            if (firstView != nullptr && firstView->IsKindOf(wxCLASSINFO(BaseProjectView)))
                {
                auto* docView = dynamic_cast<BaseProjectView*>(firstView);
                docView->Activate(true);
                int x{ 0 }, y{ 0 }, mainX{ 0 }, mainY{ 0 };
                docView->GetQuickToolbar()->GetScreenPosition(&mainX, &mainY);
                docView->GetSideBar()->GetScreenPosition(&x, &y);
                y = y - mainY;
                if (Screenshot::SaveScreenshot(path))
                    {
                    // if requesting to crop the image vertically to the last
                    // (or selected) item in the sidebar
                    if (lua_gettop(L) >= 2)
                        {
                        auto cropMode{ static_cast<ProjectScreenshotCropMode>(
                            lua_tointeger(L, 2)) };
                        y += ((cropMode == ProjectScreenshotCropMode::CropToSidebarBottom) ?
                                  docView->GetSideBar()->CalculateItemRects().first :
                              (cropMode == ProjectScreenshotCropMode::CropToSidebarSelectedItem) ?
                                  docView->GetSideBar()->CalculateItemRects().second :
                                  0);
                        if (cropMode == ProjectScreenshotCropMode::NoCrop)
                            {
                            lua_pushboolean(L, 1);
                            return 1;
                            }
                        if (Screenshot::CropScreenshot(path, wxDefaultCoord, y))
                            {
                            lua_pushboolean(L, 1);
                            return 1;
                            }
                        lua_pushboolean(L, 0);
                        return 1;
                        }

                    lua_pushboolean(L, 1);
                    return 1;
                    }

                lua_pushboolean(L, 0);
                return 1;
                }

            lua_pushboolean(L, 0);
            return 1;
            }

        lua_pushboolean(L, 0);
        return 1;
        }

    //-------------------------------------------------------------
    int CropScreenshot(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxCoord x{ wxDefaultCoord }, y{ wxDefaultCoord };

        if (lua_gettop(L) >= 1)
            {
            x = lua_tonumber(L, 2);
            }
        if (lua_gettop(L) >= 2)
            {
            y = lua_tonumber(L, 3);
            }

        if (Screenshot::CropScreenshot(wxString{ luaL_checkstring(L, 1), wxConvUTF8 }, x, y))
            {
            lua_pushboolean(L, 1);
            return 1;
            }

        lua_pushboolean(L, 0);
        return 1;
        }

    //-------------------------------------------------------------
    int SnapScreenshotOfTextWindow(lua_State* L)
        {
        ::wxSleep(2);
        if (!VerifyParameterCount(L, 2, __func__))
            {
            return 0;
            }

        wxWindowID windowId = lua_tonumber(L, 2);
        if (const auto windowMappedId = ReadabilityApp::GetDynamicIdMap().find(lua_tonumber(L, 2));
            windowMappedId != ReadabilityApp::GetDynamicIdMap().cend())
            {
            windowId = windowMappedId->second;
            }

        wxWindow* windowToCapture = Screenshot::GetActiveDialogOrFrame();
        if (windowToCapture == nullptr && wxTopLevelWindows.GetCount() > 0)
            {
            windowToCapture = wxTopLevelWindows.GetLast()->GetData();
            }
        if (windowToCapture == nullptr)
            {
            lua_pushboolean(L, 0);
            return 1;
            }
        if (windowToCapture->GetId() != windowId ||
            !windowToCapture->IsKindOf(CLASSINFO(wxTextCtrl)))
            {
            wxWindow* foundWindow = windowToCapture->FindWindow(windowId);
            if (foundWindow != nullptr && foundWindow->IsKindOf(CLASSINFO(wxTextCtrl)))
                {
                windowToCapture = foundWindow;
                }
            else
                {
                lua_pushboolean(L, 0);
                return 1;
                }
            }

        auto* textCtrl = dynamic_cast<wxTextCtrl*>(windowToCapture);
        wxASSERT(textCtrl);
        if (textCtrl == nullptr)
            {
            lua_pushboolean(L, 0);
            return 1;
            }

        std::vector<std::pair<long, long>> highlightPoints;
        if (lua_gettop(L) > 3)
            {
            // search for the strings to highlight and store their positions in the text
            // (it is assumed that the strings are in the order that they appear in the text)
            wxTextSearchResult previousFind;
            for (long i = 4; i <= lua_gettop(L); ++i)
                {
                const wxString contentToFind{ luaL_checkstring(L, i), wxConvUTF8 };
                const auto searchResult = textCtrl->SearchText(
                    wxTextSearch{ contentToFind }.Start(previousFind ? previousFind.m_end : 0));
                if (searchResult)
                    {
                    highlightPoints.emplace_back(searchResult.m_start, searchResult.m_end);
                    previousFind = searchResult;
                    }
                else
                    {
                    DebugPrint(wxString::Format(
                        // TRANSLATORS: %s are formatting tags and
                        // should stay wrapped around "Warning"
                        _(L"%sWarning%s: unable to find \"%s\" in text window."),
                        L"<span style='color:#00A2E8; font-weight:bold;'>", L"</span>",
                        wxString{ contentToFind }.Truncate(10).append(
                            contentToFind.length() > 10 ? wxString{ _DT(L"...") } : wxString{})));
                    }
                }
            }

        lua_pushboolean(L, static_cast<int>(Screenshot::SaveScreenshotOfTextWindow(
                               wxString{ luaL_checkstring(L, 1), wxConvUTF8 }, windowId,
                               int_to_bool(lua_toboolean(L, 3)), highlightPoints)));
        return 1;
        }

    //-------------------------------------------------------------
    int SnapScreenshotOfRibbon(lua_State* L)
        {
        ::wxSleep(2);
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        int pageToSelect{ 0 }, firstButtonBarID{ -1 }, lastButtonBarID{ -1 };
        if (lua_gettop(L) >= 2)
            {
            auto idPos = ReadabilityApp::GetDynamicIdMap().find(lua_tonumber(L, 2));
            if (idPos != ReadabilityApp::GetDynamicIdMap().cend())
                {
                pageToSelect = idPos->second;
                }
            else
                {
                pageToSelect = lua_tonumber(L, 2);
                }
            }
        if (lua_gettop(L) >= 3)
            {
            auto idPos = ReadabilityApp::GetDynamicIdMap().find(lua_tonumber(L, 3));
            if (idPos != ReadabilityApp::GetDynamicIdMap().cend())
                {
                firstButtonBarID = idPos->second;
                }
            else
                {
                firstButtonBarID = lua_tonumber(L, 3);
                }
            }
        if (lua_gettop(L) >= 4)
            {
            auto idPos = ReadabilityApp::GetDynamicIdMap().find(lua_tonumber(L, 4));
            if (idPos != ReadabilityApp::GetDynamicIdMap().cend())
                {
                lastButtonBarID = idPos->second;
                }
            else
                {
                lastButtonBarID = lua_tonumber(L, 3);
                }
            }
        lua_pushboolean(L, static_cast<int>(Screenshot::SaveScreenshotOfRibbon(
                               wxString{ luaL_checkstring(L, 1), wxConvUTF8 }, pageToSelect,
                               firstButtonBarID, lastButtonBarID)));
        return 1;
        }

    //-------------------------------------------------------------
    int SnapScreenshotOfListControl(lua_State* L)
        {
        ::wxSleep(2);
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        int startRow{ -1 }, endRow{ -1 }, startColumn{ -1 }, endColumn{ -1 }, cutOffRow{ -1 };
        if (lua_gettop(L) >= 3)
            {
            startRow = lua_tonumber(L, 3);
            }
        if (lua_gettop(L) >= 4)
            {
            startColumn = lua_tonumber(L, 4);
            }
        if (lua_gettop(L) >= 5)
            {
            endRow = lua_tonumber(L, 5);
            }
        if (lua_gettop(L) >= 6)
            {
            endColumn = lua_tonumber(L, 6);
            }
        if (lua_gettop(L) >= 7)
            {
            cutOffRow = lua_tonumber(L, 7);
            }
        wxWindowID windowId = lua_tonumber(L, 2);
        if (const auto windowMappedId = ReadabilityApp::GetDynamicIdMap().find(lua_tonumber(L, 2));
            windowMappedId != ReadabilityApp::GetDynamicIdMap().cend())
            {
            windowId = windowMappedId->second;
            }
        lua_pushboolean(
            L, static_cast<int>(Screenshot::SaveScreenshotOfListControl(
                   wxString{ luaL_checkstring(L, 1), wxConvUTF8 }, windowId,
                   // make zero-indexed
                   startRow == -1 ? -1 : startRow - 1, endRow == -1 ? -1 : endRow - 1,
                   startColumn == -1 ? -1 : startColumn - 1, endColumn == -1 ? -1 : endColumn - 1,
                   cutOffRow == -1 ? -1 : cutOffRow - 1)));
        return 1;
        }

    //-------------------------------------------------------------
    int SnapScreenshotOfDialogWithPropertyGrid(lua_State* L)
        {
        ::wxSleep(2);
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        int propGridId = wxID_ANY;
        wxString propertyStart, propertyEnd;
        if (lua_gettop(L) > 1)
            {
            auto idPos = ReadabilityApp::GetDynamicIdMap().find(lua_tonumber(L, 2));
            if (idPos != ReadabilityApp::GetDynamicIdMap().cend())
                {
                propGridId = idPos->second;
                }
            else
                {
                propGridId = lua_tonumber(L, 2);
                }
            }
        if (lua_gettop(L) > 2)
            {
            propertyStart = wxString{ luaL_checkstring(L, 3), wxConvUTF8 };
            }
        if (lua_gettop(L) > 3)
            {
            propertyEnd = wxString{ luaL_checkstring(L, 4), wxConvUTF8 };
            }
        lua_pushboolean(
            L, static_cast<int>(Screenshot::SaveScreenshotOfDialogWithPropertyGrid(
                   wxString{ luaL_checkstring(L, 1), wxConvUTF8 }, propGridId, propertyStart,
                   propertyEnd,
                   std::make_pair(((lua_gettop(L) > 4) ? int_to_bool(lua_toboolean(L, 5)) : false),
                                  ((lua_gettop(L) > 5) ? lua_tonumber(L, 6) : -1)))));
        return 1;
        }

    //-------------------------------------------------------------
    int HighlightScreenshot(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        lua_pushboolean(L, static_cast<int>(Screenshot::HighlightItemInScreenshot(
                               wxString{ luaL_checkstring(L, 1), wxConvUTF8 },
                               wxPoint(lua_tonumber(L, 2), lua_tonumber(L, 3)),
                               wxPoint(lua_tonumber(L, 4), lua_tonumber(L, 5)))));
        return 1;
        }

    //-------------------------------------------------------------
    int ShowDocGroupSelectDlg(lua_State* L)
        {
        if (LuaDocGroupSelectDlg == nullptr)
            {
            LuaDocGroupSelectDlg = new DocGroupSelectDlg(wxGetApp().GetMainFrame());
            }
        if (lua_gettop(L) > 0)
            {
            LuaDocGroupSelectDlg->SetSelection(lua_tonumber(L, 1) - 1 /*make zero indexed*/);
            }
        if (lua_gettop(L) > 1)
            {
            LuaDocGroupSelectDlg->SetGroupingLabel(wxString{ luaL_checkstring(L, 2), wxConvUTF8 });
            }
        LuaDocGroupSelectDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseDocGroupSelectDlg([[maybe_unused]] lua_State* L)
        {
        if (LuaDocGroupSelectDlg != nullptr)
            {
            LuaDocGroupSelectDlg->Destroy();
            LuaDocGroupSelectDlg = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowEditorTextDlg(lua_State* L)
        {
        if (LuaEditTextDlg == nullptr)
            {
            wxDocument* currentDoc =
                wxGetApp().GetMainFrame()->GetDocumentManager()->GetCurrentDocument();
            LuaEditTextDlg = new EditTextDlg(
                wxGetApp().GetMainFrame(),
                ((currentDoc != nullptr) && currentDoc->IsKindOf(wxCLASSINFO(ProjectDoc))) ?
                    dynamic_cast<ProjectDoc*>(currentDoc) :
                    nullptr,
                ((currentDoc != nullptr) && currentDoc->IsKindOf(wxCLASSINFO(ProjectDoc))) ?
                    dynamic_cast<ProjectDoc*>(currentDoc)->GetDocumentText() :
                    wxString{});
            if (lua_gettop(L) > 1)
                {
                LuaEditTextDlg->SetSize(
                    LuaEditTextDlg->FromDIP(wxSize(lua_tonumber(L, 1), lua_tonumber(L, 2))));
                }
            }
        LuaEditTextDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseEditorTextDlg([[maybe_unused]] lua_State* L)
        {
        if (LuaEditTextDlg != nullptr)
            {
            LuaEditTextDlg->Destroy();
            LuaEditTextDlg = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowEditWordListDlg(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        if (LuaEditWordListDlg == nullptr)
            {
            LuaEditWordListDlg =
                new EditWordListDlg(wxGetApp().GetMainFrame(), wxID_ANY, _(L"Edit Phrase List"));
            }
        LuaEditWordListDlg->SetPhraseFileMode(true);
        LuaEditWordListDlg->SetFilePath(wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        LuaEditWordListDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseEditWordListDlg([[maybe_unused]] lua_State* L)
        {
        if (LuaEditWordListDlg != nullptr)
            {
            LuaEditWordListDlg->Destroy();
            LuaEditWordListDlg = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowSortListDlg(lua_State* L)
        {
        wxArrayString columns;
        for (int i = 1; i < lua_gettop(L) + 1; ++i)
            {
            columns.Add(wxGetTranslation(wxString{ luaL_checkstring(L, i), wxConvUTF8 }));
            }
        if (LuaListCtrlSortDlg == nullptr)
            {
            LuaListCtrlSortDlg =
                new Wisteria::UI::ListCtrlSortDlg(wxGetApp().GetMainFrame(), columns);
            }
        std::vector<std::pair<size_t, Wisteria::SortDirection>> sortInfo;
        sortInfo.reserve(columns.size());
        for (size_t i = 0; i < columns.size(); ++i)
            {
            sortInfo.emplace_back(i, Wisteria::SortDirection::SortAscending);
            }
        LuaListCtrlSortDlg->FillSortCriteria(sortInfo);
        LuaListCtrlSortDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseSortListDlg([[maybe_unused]] lua_State* L)
        {
        if (LuaListCtrlSortDlg != nullptr)
            {
            LuaListCtrlSortDlg->Destroy();
            LuaListCtrlSortDlg = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowFilteredTextPreviewDlg([[maybe_unused]] lua_State* L)
        {
        if (LuaFilteredTextPreviewDlg == nullptr)
            {
            LuaFilteredTextPreviewDlg = new FilteredTextPreviewDlg(
                wxGetApp().GetMainFrame(), InvalidSentence::ExcludeFromAnalysis, true, true, true,
                true, true, true, true, true);
            }
        wxDocument* currentDoc =
            wxGetApp().GetMainFrame()->GetDocumentManager()->GetCurrentDocument();
        if (currentDoc != nullptr && currentDoc->IsKindOf(wxCLASSINFO(ProjectDoc)))
            {
            std::wstring filteredText;
            dynamic_cast<ProjectDoc*>(currentDoc)
                ->FormatFilteredText(filteredText, true, true, true, true, true, true);
            LuaFilteredTextPreviewDlg->SetFilteredValue(filteredText);
            }
        LuaFilteredTextPreviewDlg->ShowDetails();
        LuaFilteredTextPreviewDlg->FindWindow(wxID_OK)->SetFocus();
        LuaFilteredTextPreviewDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseFilteredTextPreviewDlg([[maybe_unused]] lua_State* L)
        {
        if (LuaFilteredTextPreviewDlg != nullptr)
            {
            LuaFilteredTextPreviewDlg->Destroy();
            LuaFilteredTextPreviewDlg = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowStandardProjectWizardLanguagePage([[maybe_unused]] lua_State* L)
        {
        if (LuaStandardProjectWizard != nullptr)
            {
            CloseStandardProjectWizard(nullptr);
            }
        LuaStandardProjectWizard =
            new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::StandardProject);
        LuaStandardProjectWizard->SelectPage(0);
        LuaStandardProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaStandardProjectWizard->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowStandardProjectWizardTextSourcePage(lua_State* L)
        {
        if (LuaStandardProjectWizard != nullptr)
            {
            CloseStandardProjectWizard(nullptr);
            }
        LuaStandardProjectWizard =
            new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::StandardProject);
        if (lua_gettop(L) > 0 && lua_tonumber(L, 1) == 1 /*make zero-indexed*/)
            {
            LuaStandardProjectWizard->SetTextFromFileSelected();
            }
        if (lua_gettop(L) > 0 && lua_tonumber(L, 1) == 2 /*make zero-indexed*/)
            {
            LuaStandardProjectWizard->SetManualTextEntrySelected();
            }
        LuaStandardProjectWizard->SelectPage(0);
        LuaStandardProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaStandardProjectWizard->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowStandardProjectWizardTextFromFilePage(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        if (LuaStandardProjectWizard != nullptr)
            {
            CloseStandardProjectWizard(nullptr);
            }
        LuaStandardProjectWizard =
            new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::StandardProject,
                                 wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        LuaStandardProjectWizard->SetTextFromFileSelected();
        LuaStandardProjectWizard->SetFilePath(wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        LuaStandardProjectWizard->SelectPage(0);
        LuaStandardProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaStandardProjectWizard->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int SetStandardProjectWizardTextFromFilePath(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        if (LuaStandardProjectWizard != nullptr)
            {
            CloseStandardProjectWizard(nullptr);
            }
        LuaStandardProjectWizard =
            new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::StandardProject,
                                 wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        LuaStandardProjectWizard->SetTextFromFileSelected();
        LuaStandardProjectWizard->SetFilePath(wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        LuaStandardProjectWizard->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowStandardProjectWizardTextEntryPage(lua_State* L)
        {
        if (LuaStandardProjectWizard != nullptr)
            {
            CloseStandardProjectWizard(nullptr);
            }
        LuaStandardProjectWizard =
            new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::StandardProject);
        LuaStandardProjectWizard->SetManualTextEntrySelected();
        if (lua_gettop(L) > 0)
            {
            LuaStandardProjectWizard->SetEnteredText(
                wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
            }
        LuaStandardProjectWizard->SelectPage(0);
        LuaStandardProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaStandardProjectWizard->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowStandardProjectWizardTestRecommendationPage(lua_State* L)
        {
        if (LuaStandardProjectWizard != nullptr)
            {
            CloseStandardProjectWizard(nullptr);
            }
        LuaStandardProjectWizard =
            new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::StandardProject);
        if (lua_gettop(L) > 0)
            {
            LuaStandardProjectWizard->SetTestSelectionMethod(lua_tonumber(L, 1) -
                                                             1 /*make zero-indexed*/);
            }
        LuaStandardProjectWizard->SelectPage(2);
        LuaStandardProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaStandardProjectWizard->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowStandardProjectWizardTestByIndustryPage(lua_State* L)
        {
        if (LuaStandardProjectWizard != nullptr)
            {
            CloseStandardProjectWizard(nullptr);
            }
        LuaStandardProjectWizard =
            new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::StandardProject);
        LuaStandardProjectWizard->SetTestSelectionMethod(1);
        LuaStandardProjectWizard->SelectPage(2);
        if (lua_gettop(L) > 0)
            {
            const int selected = lua_tonumber(L, 1) - 1; // make zero-indexed
            LuaStandardProjectWizard->SelectIndustryType(
                static_cast<readability::industry_classification>(selected));
            }
        LuaStandardProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaStandardProjectWizard->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowStandardProjectWizardTestByDocumentTypePage(lua_State* L)
        {
        if (LuaStandardProjectWizard != nullptr)
            {
            CloseStandardProjectWizard(nullptr);
            }
        LuaStandardProjectWizard =
            new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::StandardProject);
        LuaStandardProjectWizard->SetTestSelectionMethod(0);
        LuaStandardProjectWizard->SelectPage(2);
        if (lua_gettop(L) > 0)
            {
            const int selection = lua_tonumber(L, 1) - 1; // make zero-indexed
            LuaStandardProjectWizard->SelectDocumentType(
                static_cast<readability::document_classification>(selection));
            }
        LuaStandardProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaStandardProjectWizard->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowStandardProjectWizardDocumentStructurePage(lua_State* L)
        {
        if (LuaStandardProjectWizard != nullptr)
            {
            CloseStandardProjectWizard(nullptr);
            }
        LuaStandardProjectWizard =
            new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::StandardProject);
        if (lua_gettop(L) > 0)
            {
            const int selectedDocType = lua_tonumber(L, 1) - 1 /* make zero-indexed*/;
            (selectedDocType == 0) ? LuaStandardProjectWizard->SetNarrativeSelected() :
                                     LuaStandardProjectWizard->SetFragmentedTextSelected();
            if (lua_gettop(L) > 1)
                {
                LuaStandardProjectWizard->SetSplitLinesSelected(int_to_bool(lua_toboolean(L, 2)));
                }
            if (lua_gettop(L) > 2)
                {
                LuaStandardProjectWizard->SetCenteredTextSelected(int_to_bool(lua_toboolean(L, 3)));
                }
            if (lua_gettop(L) > 3)
                {
                LuaStandardProjectWizard->SetNewLinesAlwaysNewParagraphsSelected(
                    int_to_bool(lua_toboolean(L, 4)));
                }
            }
        LuaStandardProjectWizard->SelectPage(1);
        LuaStandardProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaStandardProjectWizard->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowStandardProjectWizardManualTestSelectionPage(lua_State* L)
        {
        if (LuaStandardProjectWizard != nullptr)
            {
            CloseStandardProjectWizard(nullptr);
            }
        LuaStandardProjectWizard =
            new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::StandardProject);
        LuaStandardProjectWizard->SetTestSelectionMethod(2);
        LuaStandardProjectWizard->SelectPage(2);
        for (int i = 1; i <= lua_gettop(L); ++i)
            {
            LuaStandardProjectWizard->SelectStandardTestManually(
                wxString{ luaL_checkstring(L, i), wxConvUTF8 });
            }
        LuaStandardProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaStandardProjectWizard->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowStandardProjectWizardTestByBundlePage(lua_State* L)
        {
        if (LuaStandardProjectWizard != nullptr)
            {
            CloseStandardProjectWizard(nullptr);
            }
        LuaStandardProjectWizard =
            new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::StandardProject);
        LuaStandardProjectWizard->SelectPage(2);
        LuaStandardProjectWizard->SetTestSelectionMethod(3);
        if (lua_gettop(L) > 0)
            {
            LuaStandardProjectWizard->SetSelectedTestBundle(
                wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
            }
        LuaStandardProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaStandardProjectWizard->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseStandardProjectWizard([[maybe_unused]] lua_State* L)
        {
        if (LuaStandardProjectWizard != nullptr)
            {
            LuaStandardProjectWizard->Destroy();
            LuaStandardProjectWizard = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowBatchProjectWizardLanguagePage(lua_State* L)
        {
        if (LuaBatchProjectWizard != nullptr)
            {
            CloseBatchProjectWizard(nullptr);
            }
        if (lua_gettop(L) > 0)
            {
            LuaBatchProjectWizard =
                new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::BatchProject,
                                     wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
            }
        else
            {
            LuaBatchProjectWizard =
                new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::BatchProject);
            }
        LuaBatchProjectWizard->SetFileListTruncationMode(
            Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode::OnlyShowFileNames);
        LuaBatchProjectWizard->SelectPage(0);
        LuaBatchProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaBatchProjectWizard->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowBatchProjectWizardTextSourcePage(lua_State* L)
        {
        if (LuaBatchProjectWizard != nullptr)
            {
            CloseBatchProjectWizard(nullptr);
            }
        LuaBatchProjectWizard = new ProjectWizardDlg(
            wxGetApp().GetMainFrame(), ProjectType::BatchProject,
            wxString{ luaL_checkstring(L, 1), wxConvUTF8 }, wxID_ANY, _(L"New Project Wizard"),
            wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
            Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode::OnlyShowFileNames);
        LuaBatchProjectWizard->SetFileListTruncationMode(
            Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode::OnlyShowFileNames);
        LuaBatchProjectWizard->SelectPage(0);
        LuaBatchProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaBatchProjectWizard->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowBatchProjectWizardTestRecommendationPage(lua_State* L)
        {
        if (LuaBatchProjectWizard != nullptr)
            {
            CloseBatchProjectWizard(nullptr);
            }
        LuaBatchProjectWizard =
            new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::BatchProject);
        LuaBatchProjectWizard->SetFileListTruncationMode(
            Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode::OnlyShowFileNames);
        if (lua_gettop(L) > 0)
            {
            LuaBatchProjectWizard->SetTestSelectionMethod(lua_tonumber(L, 1) -
                                                          1 /*make zero-indexed*/);
            }
        LuaBatchProjectWizard->SelectPage(2);
        LuaBatchProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaBatchProjectWizard->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowBatchProjectWizardTestByIndustryPage(lua_State* L)
        {
        if (LuaBatchProjectWizard != nullptr)
            {
            CloseBatchProjectWizard(nullptr);
            }
        LuaBatchProjectWizard =
            new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::BatchProject);
        LuaBatchProjectWizard->SetFileListTruncationMode(
            Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode::OnlyShowFileNames);
        LuaBatchProjectWizard->SetTestSelectionMethod(1);
        LuaBatchProjectWizard->SelectPage(2);
        if (lua_gettop(L) > 0)
            {
            const int selected = lua_tonumber(L, 1) - 1; // make zero-indexed
            LuaBatchProjectWizard->SelectIndustryType(
                static_cast<readability::industry_classification>(selected));
            }
        LuaBatchProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaBatchProjectWizard->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowBatchProjectWizardTestByDocumentTypePage(lua_State* L)
        {
        if (LuaBatchProjectWizard != nullptr)
            {
            CloseBatchProjectWizard(nullptr);
            }
        LuaBatchProjectWizard =
            new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::BatchProject);
        LuaBatchProjectWizard->SetFileListTruncationMode(
            Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode::OnlyShowFileNames);
        LuaBatchProjectWizard->SetTestSelectionMethod(0);
        LuaBatchProjectWizard->SelectPage(2);
        if (lua_gettop(L) > 0)
            {
            const int selection = lua_tonumber(L, 1) - 1; // make zero-indexed
            LuaBatchProjectWizard->SelectDocumentType(
                static_cast<readability::document_classification>(selection));
            }
        LuaBatchProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaBatchProjectWizard->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowBatchProjectWizardDocumentStructurePage(lua_State* L)
        {
        if (LuaBatchProjectWizard != nullptr)
            {
            CloseBatchProjectWizard(nullptr);
            }
        LuaBatchProjectWizard =
            new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::BatchProject);
        LuaBatchProjectWizard->SetFileListTruncationMode(
            Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode::OnlyShowFileNames);
        if (lua_gettop(L) > 0)
            {
            const int selectedDocType = lua_tonumber(L, 1) - 1 /*make zero-indexed*/;
            (selectedDocType == 0) ? LuaBatchProjectWizard->SetNarrativeSelected() :
                                     LuaBatchProjectWizard->SetFragmentedTextSelected();
            if (lua_gettop(L) > 1)
                {
                LuaBatchProjectWizard->SetSplitLinesSelected(int_to_bool(lua_toboolean(L, 2)));
                }
            if (lua_gettop(L) > 2)
                {
                LuaBatchProjectWizard->SetCenteredTextSelected(int_to_bool(lua_toboolean(L, 3)));
                }
            if (lua_gettop(L) > 3)
                {
                LuaBatchProjectWizard->SetNewLinesAlwaysNewParagraphsSelected(
                    int_to_bool(lua_toboolean(L, 4)));
                }
            }
        LuaBatchProjectWizard->SelectPage(1);
        LuaBatchProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaBatchProjectWizard->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowBatchProjectWizardManualTestSelectionPage(lua_State* L)
        {
        if (LuaBatchProjectWizard != nullptr)
            {
            CloseBatchProjectWizard(nullptr);
            }
        LuaBatchProjectWizard =
            new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::BatchProject);
        LuaBatchProjectWizard->SetFileListTruncationMode(
            Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode::OnlyShowFileNames);
        LuaBatchProjectWizard->SetTestSelectionMethod(2);
        LuaBatchProjectWizard->SelectPage(2);
        for (int i = 1; i <= lua_gettop(L); ++i)
            {
            LuaStandardProjectWizard->SelectStandardTestManually(
                wxString{ luaL_checkstring(L, i), wxConvUTF8 });
            }
        LuaBatchProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaBatchProjectWizard->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProjectWizardTextSourcePageSetFiles(lua_State* L)
        {
        if (LuaBatchProjectWizard != nullptr)
            {
            CloseBatchProjectWizard(nullptr);
            }
        LuaBatchProjectWizard =
            new ProjectWizardDlg(wxGetApp().GetMainFrame(), ProjectType::BatchProject);
        LuaBatchProjectWizard->SetFileListTruncationMode(
            Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode::OnlyShowFileNames);
        LuaBatchProjectWizard->GetFileList()->DeleteAllItems();
        if (lua_gettop(L) > 0)
            {
            LuaBatchProjectWizard->GetFileList()->SetVirtualDataSize(1, 1);
            LuaBatchProjectWizard->GetFileList()->SetItemText(
                0, 0, wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
            }
        LuaBatchProjectWizard->FindWindow(wxID_FORWARD)->SetFocus();
        LuaBatchProjectWizard->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseBatchProjectWizard([[maybe_unused]] lua_State* L)
        {
        if (LuaBatchProjectWizard != nullptr)
            {
            LuaBatchProjectWizard->Destroy();
            LuaBatchProjectWizard = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowTestBundleDialog(lua_State* L)
        {
        if (LuaTestBundleDlg != nullptr)
            {
            LuaTestBundleDlg->Close();
            }

        TestBundle bundle(L"");
        for (int i = 2; i < lua_gettop(L); ++i)
            {
            bundle.GetTestGoals().insert(
                TestGoal{ wxString{ luaL_checkstring(L, i + 1), wxConvUTF8 }.wc_str() });
            }
        LuaTestBundleDlg = new TestBundleDlg(wxGetApp().GetMainFrame(), bundle);
        if (lua_gettop(L) > 0)
            {
            LuaTestBundleDlg->SetTestBundleName(
                wxString{ luaL_checkstring(L, 1), wxConvUTF8 }.wc_str());
            }
        if (lua_gettop(L) > 1)
            {
            auto idPos = ReadabilityApp::GetDynamicIdMap().find(lua_tonumber(L, 2));
            if (idPos != ReadabilityApp::GetDynamicIdMap().cend())
                {
                LuaTestBundleDlg->SelectPage(idPos->second);
                }
            else
                {
                LuaTestBundleDlg->SelectPage(lua_tonumber(L, 2));
                }
            }
        LuaTestBundleDlg->Show();
        LuaTestBundleDlg->FindWindow(wxID_OK)->SetFocus();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseTestBundleDialog([[maybe_unused]] lua_State* L)
        {
        if (LuaTestBundleDlg != nullptr)
            {
            LuaTestBundleDlg->Destroy();
            LuaTestBundleDlg = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowCustomTestDialogGeneralSettings(lua_State* L)
        {
        if (LuaCustomTestDlg == nullptr)
            {
            LuaCustomTestDlg = new CustomTestDlg(wxGetApp().GetMainFrame());
            }
        LuaCustomTestDlg->SelectPage(CustomTestDlg::ID_GENERAL_PAGE);
        // set the test name (if provided)
        if (lua_gettop(L) > 0)
            {
            LuaCustomTestDlg->SetTestName(wxString{ luaL_checkstring(L, 1), wxConvUTF8 }, false);
            }
        // set formula (if provided)
        if (lua_gettop(L) > 1)
            {
            LuaCustomTestDlg->SetFormula(wxString{ luaL_checklstring(L, 2, nullptr), wxConvUTF8 });
            }
        // set the test type (if provided)
        if (lua_gettop(L) > 2)
            {
            LuaCustomTestDlg->SetTestType(lua_tonumber(L, 3));
            }
        LuaCustomTestDlg->FindWindow(wxID_OK)->SetFocus();
        LuaCustomTestDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowCustomTestDialogFunctionBrowser(lua_State* L)
        {
        if (LuaCustomTestDlg == nullptr)
            {
            LuaCustomTestDlg = new CustomTestDlg(wxGetApp().GetMainFrame());
            }
        LuaCustomTestDlg->SelectPage(CustomTestDlg::ID_GENERAL_PAGE);
        if (lua_gettop(L) > 0)
            {
            LuaCustomTestDlg->ShowFunctionBrowser(int_to_bool(lua_toboolean(L, 1)));
            }
        else
            {
            LuaCustomTestDlg->ShowFunctionBrowser(true);
            }
        LuaCustomTestDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowCustomTestDialogProperNounsAndNumbers(lua_State* L)
        {
        if (LuaCustomTestDlg == nullptr)
            {
            LuaCustomTestDlg = new CustomTestDlg(wxGetApp().GetMainFrame());
            }
        LuaCustomTestDlg->SelectPage(CustomTestDlg::ID_PROPER_NUMERALS_PAGE);
        if (lua_gettop(L) > 0)
            {
            LuaCustomTestDlg->SetProperNounMethod(lua_tonumber(L, 1));
            }
        if (lua_gettop(L) > 1)
            {
            LuaCustomTestDlg->SetIncludingNumeric(int_to_bool(lua_toboolean(L, 2)));
            }
        LuaCustomTestDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowCustomTestDialogFamiliarWords(lua_State* L)
        {
        if (LuaCustomTestDlg == nullptr)
            {
            LuaCustomTestDlg = new CustomTestDlg(wxGetApp().GetMainFrame());
            }
        LuaCustomTestDlg->SelectPage(CustomTestDlg::ID_WORD_LIST_PAGE);
        // set the custom words
        if (lua_gettop(L) > 0)
            {
            const wxString wordFile(luaL_checklstring(L, 1, nullptr), wxConvUTF8);
            if (!wordFile.empty())
                {
                LuaCustomTestDlg->SetIncludingCustomWordList(true);
                LuaCustomTestDlg->SetWordListFilePath(wordFile);
                }
            }
        // set the stemming type (if provided)
        if (lua_gettop(L) > 1)
            {
            LuaCustomTestDlg->SetStemmingType(
                static_cast<stemming::stemming_type>(static_cast<int>(lua_tonumber(L, 2))));
            }
        // other word lists
        if (lua_gettop(L) > 2)
            {
            LuaCustomTestDlg->SetIncludingDaleChallList(int_to_bool(lua_toboolean(L, 3)));
            }
        if (lua_gettop(L) > 3)
            {
            LuaCustomTestDlg->SetIncludingSpacheList(int_to_bool(lua_toboolean(L, 4)));
            }
        if (lua_gettop(L) > 4)
            {
            LuaCustomTestDlg->SetIncludingHJList(int_to_bool(lua_toboolean(L, 5)));
            }
        if (lua_gettop(L) > 5)
            {
            LuaCustomTestDlg->SetIncludingStockerList(int_to_bool(lua_toboolean(L, 6)));
            }
        // whether a union of word lists should be used
        if (lua_gettop(L) > 6)
            {
            LuaCustomTestDlg->SetFamiliarWordsMustBeOnEachIncludedList(
                int_to_bool(lua_toboolean(L, 7)));
            }
        LuaCustomTestDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowCustomTestDialogClassification([[maybe_unused]] lua_State* L)
        {
        if (LuaCustomTestDlg == nullptr)
            {
            LuaCustomTestDlg = new CustomTestDlg(wxGetApp().GetMainFrame());
            }
        LuaCustomTestDlg->SelectPage(CustomTestDlg::ID_CLASSIFICATION_PAGE);
        LuaCustomTestDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int SetCustomTestDialogDocumentTypes(lua_State* L)
        {
        if (lua_gettop(L) < 5)
            {
            wxMessageBox(
                wxString::Format(_(L"%s: invalid number of arguments."), wxString{ __func__ }),
                _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
            lua_pushboolean(L, 0);
            return 1;
            }
        if (LuaCustomTestDlg == nullptr)
            {
            LuaCustomTestDlg = new CustomTestDlg(wxGetApp().GetMainFrame());
            }
        LuaCustomTestDlg->SetGeneralDocumentSelected(int_to_bool(lua_tonumber(L, 1)));
        LuaCustomTestDlg->SetTechnicalDocumentSelected(int_to_bool(lua_toboolean(L, 2)));
        LuaCustomTestDlg->SetNonNarrativeFormSelected(int_to_bool(lua_toboolean(L, 3)));
        LuaCustomTestDlg->SetYoungAdultAndAdultLiteratureSelected(int_to_bool(lua_toboolean(L, 4)));
        LuaCustomTestDlg->SetChildrensLiteratureSelected(int_to_bool(lua_toboolean(L, 5)));
        wxGetApp().Yield();
        lua_pushboolean(L, 1);
        return 1;
        }

    //-------------------------------------------------------------
    int SetCustomTestDialogIndustries(lua_State* L)
        {
        if (lua_gettop(L) < 7)
            {
            wxMessageBox(
                wxString::Format(_(L"%s: invalid number of arguments."), wxString{ __func__ }),
                _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
            lua_pushboolean(L, 0);
            return 1;
            }
        if (LuaCustomTestDlg == nullptr)
            {
            LuaCustomTestDlg = new CustomTestDlg(wxGetApp().GetMainFrame());
            }
        LuaCustomTestDlg->SetChildrensPublishingSelected(int_to_bool(lua_toboolean(L, 1)));
        LuaCustomTestDlg->SetAdultPublishingSelected(int_to_bool(lua_toboolean(L, 2)));
        LuaCustomTestDlg->SetChildrensHealthCareTestSelected(int_to_bool(lua_toboolean(L, 3)));
        LuaCustomTestDlg->SetAdultHealthCareTestSelected(int_to_bool(lua_toboolean(L, 4)));
        LuaCustomTestDlg->SetMilitaryTestSelected(int_to_bool(lua_toboolean(L, 5)));
        LuaCustomTestDlg->SetSecondaryLanguageSelected(int_to_bool(lua_toboolean(L, 6)));
        LuaCustomTestDlg->SetBroadcastingSelected(int_to_bool(lua_toboolean(L, 7)));
        wxGetApp().Yield();
        lua_pushboolean(L, 1);
        return 1;
        }

    //-------------------------------------------------------------
    int SetCustomTestDialogIncludedLists(lua_State* L)
        {
        if (LuaCustomTestDlg == nullptr)
            {
            LuaCustomTestDlg = new CustomTestDlg(wxGetApp().GetMainFrame());
            }
        LuaCustomTestDlg->SetIncludingCustomWordList(int_to_bool(lua_toboolean(L, 1)));
        LuaCustomTestDlg->SetIncludingDaleChallList(int_to_bool(lua_toboolean(L, 2)));
        LuaCustomTestDlg->SetIncludingSpacheList(int_to_bool(lua_toboolean(L, 3)));
        LuaCustomTestDlg->SetIncludingHJList(int_to_bool(lua_toboolean(L, 4)));
        LuaCustomTestDlg->SetIncludingStockerList(int_to_bool(lua_toboolean(L, 5)));
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseCustomTestDialog([[maybe_unused]] lua_State* L)
        {
        if (LuaCustomTestDlg != nullptr)
            {
            LuaCustomTestDlg->Destroy();
            LuaCustomTestDlg = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowOptions(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        if (LuaOptionsDlg == nullptr)
            {
            LuaOptionsDlg = new ToolsOptionsDlg(wxGetApp().GetMainFrame());
            }
        auto idPos = ReadabilityApp::GetDynamicIdMap().find(lua_tonumber(L, 1));
        if (idPos != ReadabilityApp::GetDynamicIdMap().cend())
            {
            LuaOptionsDlg->SelectPage(idPos->second);
            }
        else
            {
            LuaOptionsDlg->SelectPage(lua_tonumber(L, 1));
            }
        LuaOptionsDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseOptions([[maybe_unused]] lua_State* L)
        {
        if (LuaOptionsDlg != nullptr)
            {
            LuaOptionsDlg->Destroy();
            LuaOptionsDlg = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowPrinterHeaderFooterOptions([[maybe_unused]] lua_State* L)
        {
        if (LuaPrinterOptions == nullptr)
            {
            LuaPrinterOptions = new Wisteria::UI::PrinterHeaderFooterDlg(
                wxGetApp().GetMainFrame(), wxGetApp().GetAppOptions()->GetLeftPrinterHeader(),
                wxGetApp().GetAppOptions()->GetCenterPrinterHeader(),
                wxGetApp().GetAppOptions()->GetRightPrinterHeader(),
                wxGetApp().GetAppOptions()->GetLeftPrinterFooter(),
                wxGetApp().GetAppOptions()->GetCenterPrinterFooter(),
                wxGetApp().GetAppOptions()->GetRightPrinterFooter());
            }
        LuaPrinterOptions->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ClosePrinterHeaderFooterOptions([[maybe_unused]] lua_State* L)
        {
        if (LuaPrinterOptions != nullptr)
            {
            LuaPrinterOptions->Destroy();
            LuaPrinterOptions = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowListExportDlg(lua_State* L)
        {
        if (LuaListExportDlg == nullptr)
            {
            LuaListExportDlg = new Wisteria::UI::GridExportDlg(
                wxGetApp().GetMainFrame(), lua_tonumber(L, 1), lua_tonumber(L, 2),
                Wisteria::UI::GridExportFormat::ExportHtml);
            }
        LuaListExportDlg->IncludeColumnHeaders(int_to_bool(lua_toboolean(L, 3)));
        LuaListExportDlg->ExportSelectedRowsOnly(int_to_bool(lua_toboolean(L, 4)));
        LuaListExportDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseListExportDlg([[maybe_unused]] lua_State* L)
        {
        if (LuaListExportDlg != nullptr)
            {
            LuaListExportDlg->Destroy();
            LuaListExportDlg = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseListViewItemDlg([[maybe_unused]] lua_State* L)
        {
        if (LuaListViewItemDlg != nullptr)
            {
            LuaListViewItemDlg->Destroy();
            LuaListViewItemDlg = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowListViewItemDlg(lua_State* L)
        {
        if (LuaListViewItemDlg != nullptr)
            {
            CloseListViewItemDlg(L);
            }
        LuaListViewItemDlg = new Wisteria::UI::ListCtrlItemViewDlg;
        const FilePathResolver fileResolve;
        for (int i = 1; i < lua_gettop(L); i += 2)
            {
            LuaListViewItemDlg->AddValue(
                wxGetTranslation(wxString{ luaL_checklstring(L, i, nullptr), wxConvUTF8 }),
                wxGetTranslation(wxString{ luaL_checklstring(L, i + 1, nullptr), wxConvUTF8 }));
            }
        LuaListViewItemDlg->Create(wxGetApp().GetMainFrame());
        LuaListViewItemDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowGetArchiveDlg(lua_State* L)
        {
        if (LuaGetArchiveDlg == nullptr)
            {
            LuaGetArchiveDlg = new Wisteria::UI::ArchiveDlg(
                wxGetApp().GetMainFrame(), ReadabilityAppOptions::GetDocumentFilter());
            }
        LuaGetArchiveDlg->SetPath(wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        LuaGetArchiveDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseGetArchiveDlg([[maybe_unused]] lua_State* L)
        {
        if (LuaGetArchiveDlg != nullptr)
            {
            LuaGetArchiveDlg->Destroy();
            LuaGetArchiveDlg = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowGetDirDlg(lua_State* L)
        {
        if (LuaGetDirDlg == nullptr)
            {
            LuaGetDirDlg = new Wisteria::UI::GetDirFilterDialog(
                wxGetApp().GetMainFrame(), ReadabilityAppOptions::GetDocumentFilter());
            }
        LuaGetDirDlg->SetPath(wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        LuaGetDirDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseGetDirDlg([[maybe_unused]] lua_State* L)
        {
        if (LuaGetDirDlg != nullptr)
            {
            LuaGetDirDlg->Destroy();
            LuaGetDirDlg = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowSelectProjectTypeDlg(lua_State* L)
        {
        if (LuaSelectProjectType == nullptr)
            {
            wxArrayString docNames;
            docNames.Add(_(L"Standard Project"));
            docNames.Add(_(L"Batch Project"));
            const wxArrayString docDescriptions;
            LuaSelectProjectType = new Wisteria::UI::RadioBoxDlg(
                wxGetApp().GetMainFrame(), _(L"Select Project Type"), wxEmptyString,
                _(L"Project types:"), _(L"New Project"), docNames, docDescriptions);
            }
        LuaSelectProjectType->SetSelection(lua_tonumber(L, 1) - 1 /*make zero-indexed*/);
        LuaSelectProjectType->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseSelectProjectTypeDlg([[maybe_unused]] lua_State* L)
        {
        if (LuaSelectProjectType != nullptr)
            {
            LuaSelectProjectType->Destroy();
            LuaSelectProjectType = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int ShowWebHarvesterDlg([[maybe_unused]] lua_State* L)
        {
        if (LuaSelectProjectType == nullptr)
            {
            LuaWebHarvesterDlg =
                new WebHarvesterDlg(wxGetApp().GetMainFrame(), wxGetApp().GetLastSelectedWebPages(),
                                    ReadabilityAppOptions::GetDocumentFilter(),
                                    wxGetApp().GetLastSelectedDocFilter(), false);
            }
        LuaWebHarvesterDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CloseWebHarvesterDlg([[maybe_unused]] lua_State* L)
        {
        if (LuaWebHarvesterDlg != nullptr)
            {
            LuaWebHarvesterDlg->Destroy();
            LuaWebHarvesterDlg = nullptr;
            }
        wxGetApp().Yield();
        return 0;
        }
    } // namespace LuaScripting
