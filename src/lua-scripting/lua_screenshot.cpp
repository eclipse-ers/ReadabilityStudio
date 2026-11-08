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

#include "lua_screenshot.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/archivedlg.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/getdirdlg.h"
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
#include "../Wisteria-Dataviz/src/lua-scripting/lua_debug.h"
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
    static Wisteria::UI::GetDirFilterDialog* LuaGetDirDlg{ nullptr };
    static Wisteria::UI::ArchiveDlg* LuaGetArchiveDlg{ nullptr };
    static Wisteria::UI::RadioBoxDlg* LuaSelectProjectType{ nullptr };
    static WebHarvesterDlg* LuaWebHarvesterDlg{ nullptr };
    static FilteredTextPreviewDlg* LuaFilteredTextPreviewDlg{ nullptr };
    static EditWordListDlg* LuaEditWordListDlg{ nullptr };
    static DocGroupSelectDlg* LuaDocGroupSelectDlg{ nullptr };
    static EditTextDlg* LuaEditTextDlg{ nullptr };

    // NOLINTEND

    //-------------------------------------------------------------
    int ShowScriptEditor(lua_State* L)
        {
        const bool show = lua_gettop(L) > 0 ? int_to_bool(lua_toboolean(L, 1)) : true;
        if (show)
            {
            wxGetApp().GetMainFrameEx()->ActivateScriptWorkbench();
            }
        else
            {
            wxGetApp().GetMainFrameEx()->ActivateHomePage();
            }
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
            LuaBatchProjectWizard->SelectStandardTestManually(
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
        if (LuaWebHarvesterDlg == nullptr)
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
