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

#include "lua_batch_project.h"
#include "../Wisteria-Dataviz/src/base/label.h"
#include "../Wisteria-Dataviz/src/base/reportbuilder.h"
#include "../app/readability_app.h"
#include "../projects/batch_project_doc.h"
#include "../projects/batch_project_view.h"
#include "../projects/standard_project_doc.h"
#include "../ui/dialogs/tools_options_dlg.h"
#include <utility>
#include <wx/dir.h>

wxDECLARE_APP(ReadabilityApp);

// NOLINTBEGIN(readability-identifier-length)
// NOLINTBEGIN(readability-implicit-bool-conversion)
// cppcheck-suppress-file [functionConst]

/* Note: with Luna, an extra boolean argument is passed into class functions at the front,
   this should be skipped over. It seems to be an indicator of the function being successfully
   called, but not sure about that.*/

namespace LuaScripting
    {
    //-------------------------------------------------------------
    bool BatchProject::ReloadIfNotDelayed()
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return false;
            }

        if (!m_delayReloading)
            {
            m_project->RefreshRequired(ProjectRefresh::FullReindexing);
            m_project->RefreshProject();
            wxGetApp().Yield();
            }
        return true;
        }

    //-------------------------------------------------------------
    bool BatchProject::ReloadIfNotDelayedSimple()
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return false;
            }

        if (!m_delayReloading)
            {
            m_project->RefreshRequired(ProjectRefresh::Minimal);
            m_project->RefreshProject();
            wxGetApp().Yield();
            }
        return true;
        }

    //-------------------------------------------------------------
    BatchProject::BatchProject(lua_State* L)
        {
        const auto createProject = [this](const wxString& folderPath)
        {
            // create a batch project and load a folder
            const wxList& templateList =
                wxGetApp().GetMainFrame()->GetDocumentManager()->GetTemplates();
            for (size_t i = 0; i < templateList.GetCount(); ++i)
                {
                wxDocTemplate* docTemplate =
                    dynamic_cast<wxDocTemplate*>(templateList.Item(i)->GetData());
                if (docTemplate &&
                    docTemplate->GetDocClassInfo()->IsKindOf(wxCLASSINFO(BatchProjectDoc)))
                    {
                    m_project = dynamic_cast<BatchProjectDoc*>(
                        docTemplate->CreateDocument(folderPath, wxDOC_NEW));
                    if (m_project && !m_project->OnNewDocument())
                        {
                        // Document is implicitly deleted by DeleteAllViews
                        m_project->DeleteAllViews();
                        m_project = nullptr;
                        }
                    break;
                    }
                }
        };

        // see if a path was passed in
        if (lua_gettop(L) > 1)
            {
            const wxString path(luaL_checkstring(L, 2), wxConvUTF8);
            wxFileName fn(path);
            fn.Normalize(wxPATH_NORM_LONG | wxPATH_NORM_DOTS | wxPATH_NORM_TILDE |
                         wxPATH_NORM_ABSOLUTE);
            if (fn.GetExt().CmpNoCase(_DT(L"rsbp")) == 0)
                {
                m_project =
                    dynamic_cast<BatchProjectDoc*>(wxGetApp().GetMainFrame()->OpenFile(path));
                }
            else if (fn.GetExt().CmpNoCase(_DT(L"rsp")) == 0)
                {
                m_project = nullptr;
                wxMessageBox(_(L"A batch project cannot open a standard project file."),
                             _(L"Project File Mismatch"), wxOK | wxICON_EXCLAMATION);
                return;
                }
            else if (path.empty())
                {
                createProject(L"EMPTY_PROJECT");
                }
            else
                {
                createProject(path);
                }
            }
        else
            {
            createProject(L"EMPTY_PROJECT");
            }
        // yield so that the view can be fully refreshed before proceeding
        wxGetApp().Yield();
        }

    //-------------------------------------------------------------
    int BatchProject::LoadFiles(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        if (!lua_istable(L, 2))
            {
            // NOLINTBEGIN(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
            return luaL_error(L, "%s",
                              static_cast<const char*>(
                                  wxString::Format(_(L"%s requires a table argument."), __func__)));
            // NOLINTEND(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
            }

        const size_t fileCount{ lua_rawlen(L, 2) };

        // append the files to this project, leave what was there already
        m_project->GetSourceFilesInfo().reserve(m_project->GetSourceFilesInfo().size() + fileCount);
        for (int i = 1; std::cmp_less_equal(i, fileCount); ++i)
            {
            lua_rawgeti(L, 2, i);
            const wxString inputFile(luaL_checkstring(L, -1), wxConvUTF8);
            m_project->GetSourceFilesInfo().emplace_back(inputFile, wxString{});
            lua_pop(L, 1);
            }

        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::LoadFolder(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        const wxString path(luaL_checkstring(L, 2), wxConvUTF8);
        if (!path.empty() && wxFileName{ path }.IsDir() && wxFileName::DirExists(path))
            {
            const bool recursive = ((lua_gettop(L) > 2) ? int_to_bool(lua_toboolean(L, 3)) : true);
            wxArrayString files;
                {
                const wxWindowDisabler disableAll;
                const wxBusyInfo wait(_(L"Retrieving files..."), m_project->GetDocumentWindow());
#ifdef __WXGTK__
                wxMilliSleep(100);
                wxGetApp().Yield();
#endif
                wxDir::GetAllFiles(path, &files, wxString{},
                                   recursive ? (wxDIR_FILES | wxDIR_DIRS) : wxDIR_FILES);
                files = FilterFiles(files, ExtractExtensionsFromFileFilter(
                                               ReadabilityAppOptions::GetDocumentFilter()));
                }
            // append the files to this project, leave what was there already
            m_project->GetSourceFilesInfo().reserve(m_project->GetSourceFilesInfo().size() +
                                                    files.size());
            for (const auto& fl : files)
                {
                wxLogMessage(fl);
                m_project->GetSourceFilesInfo().emplace_back(fl, wxString{});
                }
            }

        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::DelayReloading(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        m_delayReloading = (lua_gettop(L) > 1) ? int_to_bool(lua_toboolean(L, 2)) : true;
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetTitle(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushstring(L, m_project->GetTitle().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetWindowSize(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 2, __func__))
            {
            return 0;
            }

        if (m_project->GetFirstView() && m_project->GetFirstView()->GetFrame())
            {
            m_project->GetFirstView()->GetFrame()->SetSize(luaL_checkinteger(L, 2),
                                                           luaL_checkinteger(L, 3));
            m_project->GetFirstView()->GetFrame()->Center();
            wxGetApp().Yield();
            }
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetWindowSize(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        const auto size = m_project->GetFirstView()->GetFrame()->GetSize();

        lua_createtable(L, 0, 2);

        lua_pushinteger(L, size.GetWidth());
        lua_setfield(L, -2, _DT("Width"));

        lua_pushinteger(L, size.GetHeight());
        lua_setfield(L, -2, _DT("Height"));

        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetIncludeIncompleteTolerance(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetIncludeIncompleteSentencesIfLongerThanValue(luaL_checkinteger(L, 2));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetIncludeIncompleteTolerance(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, m_project->GetIncludeIncompleteSentencesIfLongerThanValue());
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetTextExclusion(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetInvalidSentenceMethod(static_cast<InvalidSentence>(luaL_checkinteger(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetTextExclusion(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushinteger(L, static_cast<int>(m_project->GetInvalidSentenceMethod()));
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::AggressivelyExclude(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->AggressiveExclusion(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::ExcludeCopyrightNotices(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->ExcludeTrailingCopyrightNoticeParagraphs(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::ExcludeTrailingCitations(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->ExcludeTrailingCitations(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::ExcludeFileAddress(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->ExcludeFileAddresses(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::ExcludeNumerals(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->ExcludeNumerals(int_to_bool(lua_toboolean(L, 2)));
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::ExcludeProperNouns(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->ExcludeProperNouns(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::IsExcludingAggressively(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsExcludingAggressively());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::IsExcludingCopyrightNotices(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsExcludingTrailingCopyrightNoticeParagraphs());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::IsExcludingTrailingCitations(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsExcludingTrailingCitations());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::IsExcludingFileAddresses(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsExcludingFileAddresses());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::IsExcludingNumerals(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsExcludingNumerals());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::IsExcludingProperNouns(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsExcludingProperNouns());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetPhraseExclusionList(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetExcludedPhrasesPath(wxString{ luaL_checkstring(L, 2), wxConvUTF8 });
        m_project->LoadExcludePhrases();
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetPhraseExclusionList(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushstring(L, m_project->GetExcludedPhrasesPath().utf8_str());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::IncludeExcludedPhraseFirstOccurrence(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->IncludeExcludedPhraseFirstOccurrence(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::IsIncludingExcludedPhraseFirstOccurrence(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsIncludingExcludedPhraseFirstOccurrence());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetBlockExclusionTags(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        const wxString exclusionTags(luaL_checkstring(L, 2), wxConvUTF8);
        m_project->GetExclusionBlockTags().clear();
        if (exclusionTags.length() >= 2)
            {
            m_project->GetExclusionBlockTags().emplace_back(exclusionTags[0], exclusionTags[1]);
            }
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetBlockExclusionTags(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushstring(
            L, m_project->GetExclusionBlockTags().empty() ?
                   "" :
                   wxString{ std::to_wstring(m_project->GetExclusionBlockTags().front().first) +
                             std::to_wstring(m_project->GetExclusionBlockTags().front().second) }
                       .utf8_str());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetNumeralSyllabication(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetNumeralSyllabicationMethod(
            static_cast<NumeralSyllabize>(luaL_checkinteger(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetNumeralSyllabication(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetNumeralSyllabicationMethod()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::IsFogUsingSentenceUnits(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsFogUsingSentenceUnits());
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::FogUseSentenceUnits(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->FogUseSentenceUnits(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::IncludeStockerCatholicSupplement(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->IncludeStockerCatholicSupplement(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::IsIncludingStockerCatholicSupplement(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsIncludingStockerCatholicSupplement());
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::IncludeScoreSummaryReport(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->IncludeScoreSummaryReport(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::IsIncludingScoreSummaryReport(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsIncludingScoreSummaryReport());
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetLongGradeScaleFormat(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->GetReadabilityMessageCatalog().SetLongGradeScaleFormat(
            int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::IsUsingLongGradeScaleFormat(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->GetReadabilityMessageCatalog().IsUsingLongGradeScaleFormat());
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetReadingAgeDisplay(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->GetReadabilityMessageCatalog().SetReadingAgeDisplay(
            static_cast<ReadabilityMessages::ReadingAgeDisplay>(luaL_checkinteger(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetReadingAgeDisplay(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(
            L, static_cast<int>(m_project->GetReadabilityMessageCatalog().GetReadingAgeDisplay()));
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetGradeScale(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->GetReadabilityMessageCatalog().SetGradeScale(
            static_cast<readability::grade_scale>(luaL_checkinteger(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetGradeScale(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L,
                       static_cast<int>(m_project->GetReadabilityMessageCatalog().GetGradeScale()));
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetFleschNumeralSyllabizeMethod(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetFleschNumeralSyllabizeMethod(
            static_cast<FleschNumeralSyllabize>(luaL_checkinteger(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetFleschNumeralSyllabizeMethod(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetFleschNumeralSyllabizeMethod()));
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetFleschKincaidNumeralSyllabizeMethod(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetFleschKincaidNumeralSyllabizeMethod(
            static_cast<FleschKincaidNumeralSyllabize>(luaL_checkinteger(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetFleschKincaidNumeralSyllabizeMethod(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetFleschKincaidNumeralSyllabizeMethod()));
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetHarrisJacobsonTextExclusionMode(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetHarrisJacobsonTextExclusionMode(
            static_cast<SpecializedTestTextExclusion>(luaL_checkinteger(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetHarrisJacobsonTextExclusionMode(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetHarrisJacobsonTextExclusionMode()));
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetDaleChallTextExclusionMode(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetDaleChallTextExclusionMode(
            static_cast<SpecializedTestTextExclusion>(luaL_checkinteger(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetDaleChallTextExclusionMode(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetDaleChallTextExclusionMode()));
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetDaleChallProperNounCountingMethod(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetDaleChallProperNounCountingMethod(
            static_cast<readability::proper_noun_counting_method>(luaL_checkinteger(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetDaleChallProperNounCountingMethod(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetDaleChallProperNounCountingMethod()));
        return 1;
        }

    //-------------------------------------------------
    int BatchProject::SetAppendedDocumentFilePath(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetAppendedDocumentFilePath(wxString{ luaL_checkstring(L, 2), wxConvUTF8 });
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------
    int BatchProject::GetAppendedDocumentFilePath(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushstring(L, m_project->GetAppendedDocumentFilePath().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::IsRealTimeUpdating(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsRealTimeUpdating());
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::UseRealTimeUpdate(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->UseRealTimeUpdate(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetGraphBackgroundColor(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetBackGroundColor(LoadColor(wxString{ luaL_checkstring(L, 2), wxConvUTF8 }));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::ApplyGraphBackgroundFade(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetGraphBackGroundLinearGradient(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::IsApplyingGraphBackgroundFade(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->GetGraphBackGroundLinearGradient());
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetGraphCommonImage(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetGraphCommonImagePath(wxString{ luaL_checkstring(L, 2), wxConvUTF8 });
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetGraphCommonImage(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushstring(L, m_project->GetGraphCommonImagePath().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetPlotBackgroundImage(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetPlotBackGroundImagePath(wxString{ luaL_checkstring(L, 2), wxConvUTF8 });
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetPlotBackgroundImage(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushstring(L, m_project->GetPlotBackGroundImagePath().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetPlotBackgroundImageEffect(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetPlotBackGroundImageEffect(
            static_cast<Wisteria::ImageEffect>(luaL_checkinteger(L, 2)));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetPlotBackgroundImageEffect(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetPlotBackGroundImageEffect()));
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetPlotBackgroundImageFit(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetPlotBackGroundImageFit(
            static_cast<Wisteria::ImageFit>(luaL_checkinteger(L, 2)));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetPlotBackgroundImageFit(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetPlotBackGroundImageFit()));
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetPlotBackgroundImageOpacity(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetPlotBackGroundImageOpacity(luaL_checkinteger(L, 2));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetPlotBackgroundImageOpacity(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, m_project->GetPlotBackGroundImageOpacity());
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetPlotBackgroundColor(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetPlotBackGroundColor(
            LoadColor(wxString{ luaL_checkstring(L, 2), wxConvUTF8 }));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetGraphInvalidRegionColor(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetInvalidAreaColor(LoadColor(wxString{ luaL_checkstring(L, 2), wxConvUTF8 }));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetRaygorStyle(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetRaygorStyle(
            static_cast<Wisteria::Graphs::RaygorStyle>(luaL_checkinteger(L, 2)));
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetRaygorStyle(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetRaygorStyle()));
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::ConnectFleschPoints(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->ConnectFleschPoints(int_to_bool(lua_toboolean(L, 2)));
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::IsConnectingFleschPoints(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->IsConnectingFleschPoints()));
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::IncludeFleschRulerDocGroups(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->IncludeFleschRulerDocGroups(int_to_bool(lua_toboolean(L, 2)));
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::IsIncludingFleschRulerDocGroups(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->IsIncludingFleschRulerDocGroups()));
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::UseEnglishLabelsForGermanLix(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->UseEnglishLabelsForGermanLix(int_to_bool(lua_toboolean(L, 2)));
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::IsUsingEnglishLabelsForGermanLix(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->IsUsingEnglishLabelsForGermanLix()));
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetStippleShapeColor(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetStippleShapeColor(LoadColor(wxString{ luaL_checkstring(L, 2), wxConvUTF8 }));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetPlotBackgroundColorOpacity(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetPlotBackGroundColorOpacity(luaL_checkinteger(L, 2));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetPlotBackgroundColorOpacity(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, m_project->GetPlotBackGroundColorOpacity());
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetGraphColorScheme(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetGraphColorScheme(wxString{ luaL_checkstring(L, 2), wxConvUTF8 });
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetGraphColorScheme(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushstring(L, m_project->GetGraphColorScheme().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetWatermark(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        auto watermark = m_project->GetWatermark();
        watermark.m_label = wxString{ luaL_checkstring(L, 2), wxConvUTF8 };
        m_project->SetWatermark(watermark);
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetWatermark(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushstring(L, m_project->GetWatermark().m_label.utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetGraphLogoImage(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetWatermarkLogoPath(wxString{ luaL_checkstring(L, 2), wxConvUTF8 });
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetGraphLogoImage(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushstring(L, m_project->GetWatermarkLogoPath().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetStippleImage(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetStippleImagePath(wxString{ luaL_checkstring(L, 2), wxConvUTF8 });
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetStippleImage(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushstring(L, m_project->GetStippleImagePath().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetStippleShape(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetStippleShape(wxString{ luaL_checkstring(L, 2), wxConvUTF8 });
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetStippleShape(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushstring(L, m_project->GetStippleShape().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetXAxisFont(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        auto fontInfo = m_project->GetXAxisFont();
        auto fontColor = m_project->GetXAxisFontColor();
        LoadFontAttributes(L, fontInfo, fontColor, true);

        m_project->SetXAxisFont(fontInfo);
        m_project->SetXAxisFontColor(fontColor);
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetYAxisFont(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        auto fontInfo = m_project->GetYAxisFont();
        auto fontColor = m_project->GetYAxisFontColor();
        LoadFontAttributes(L, fontInfo, fontColor, true);

        m_project->SetYAxisFont(fontInfo);
        m_project->SetYAxisFontColor(fontColor);
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetGraphTopTitleFont(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        auto fontInfo = m_project->GetGraphTopTitleFont();
        auto fontColor = m_project->GetGraphTopTitleFontColor();
        LoadFontAttributes(L, fontInfo, fontColor, true);

        m_project->SetGraphTopTitleFont(fontInfo);
        m_project->SetGraphTopTitleFontColor(fontColor);
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetGraphBottomTitleFont(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        auto fontInfo = m_project->GetGraphBottomTitleFont();
        auto fontColor = m_project->GetGraphBottomTitleFontColor();
        LoadFontAttributes(L, fontInfo, fontColor, true);

        m_project->SetGraphBottomTitleFont(fontInfo);
        m_project->SetGraphBottomTitleFontColor(fontColor);
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetGraphLeftTitleFont(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        auto fontInfo = m_project->GetGraphLeftTitleFont();
        auto fontColor = m_project->GetGraphLeftTitleFontColor();
        LoadFontAttributes(L, fontInfo, fontColor, true);

        m_project->SetGraphLeftTitleFont(fontInfo);
        m_project->SetGraphLeftTitleFontColor(fontColor);
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetGraphRightTitleFont(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        auto fontInfo = m_project->GetGraphRightTitleFont();
        auto fontColor = m_project->GetGraphRightTitleFontColor();
        LoadFontAttributes(L, fontInfo, fontColor, true);

        m_project->SetGraphRightTitleFont(fontInfo);
        m_project->SetGraphRightTitleFontColor(fontColor);
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetBarChartBarColor(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetBarChartBarColor(LoadColor(wxString{ luaL_checkstring(L, 2), wxConvUTF8 }));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetBarChartBarOpacity(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetGraphBarOpacity(luaL_checkinteger(L, 2));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetBarChartBarEffect(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetGraphBarEffect(static_cast<Wisteria::BoxEffect>(luaL_checkinteger(L, 2)));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetBarChartBarEffect(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetGraphBarEffect()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::GetBarChartBarOpacity(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetGraphBarOpacity()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::GetBarChartOrientation(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetBarChartOrientation()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::IsDisplayingBarChartLabels(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsDisplayingBarChartLabels());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetBarChartOrientation(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetBarChartOrientation(
            static_cast<Wisteria::Orientation>(luaL_checkinteger(L, 2)));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::DisplayBarChartLabels(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->DisplayBarChartLabels(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetHistogramBarColor(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetHistogramBarColor(LoadColor(wxString{ luaL_checkstring(L, 2), wxConvUTF8 }));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetHistogramBarOpacity(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetHistogramBarOpacity(luaL_checkinteger(L, 2));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetHistogramBarEffect(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetHistogramBarEffect(static_cast<Wisteria::BoxEffect>(luaL_checkinteger(L, 2)));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetHistogramBarEffect(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetHistogramBarEffect()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::GetHistogramBarOpacity(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetHistogramBarOpacity()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetHistogramBinning(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetHistogramBinningMethod(
            static_cast<Wisteria::Graphs::Histogram::BinningMethod>(luaL_checkinteger(L, 2)));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetHistogramBinning(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetHistogramBinningMethod()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetHistogramIntervalDisplay(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetHistogramIntervalDisplay(
            static_cast<Wisteria::Graphs::Histogram::IntervalDisplay>(luaL_checkinteger(L, 2)));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetHistogramIntervalDisplay(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetHistogramIntervalDisplay()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetHistogramRounding(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetHistogramRoundingMethod(
            static_cast<Wisteria::RoundingMethod>(luaL_checkinteger(L, 2)));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetHistogramRounding(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetHistogramRoundingMethod()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetHistogramBinLabelDisplay(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetHistogramBinLabelDisplay(
            static_cast<Wisteria::BinLabelDisplay>(luaL_checkinteger(L, 2)));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetHistogramBinLabelDisplay(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetHistogramBinLabelDisplay()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetBoxPlotColor(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetGraphBoxColor(LoadColor(wxString{ luaL_checkstring(L, 2), wxConvUTF8 }));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetBoxPlotOpacity(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetGraphBoxOpacity(luaL_checkinteger(L, 2));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetBoxPlotEffect(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetGraphBoxEffect(static_cast<Wisteria::BoxEffect>(luaL_checkinteger(L, 2)));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetBoxPlotEffect(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetGraphBoxEffect()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::GetBoxPlotOpacity(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetGraphBoxOpacity()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::DisplayBoxPlotLabels(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->DisplayBoxPlotLabels(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::IsDisplayingBoxPlotLabels(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsDisplayingBoxPlotLabels());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::ConnectBoxPlotMiddlePoints(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->ConnectBoxPlotMiddlePoints(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::IsConnectingBoxPlotMiddlePoints(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsConnectingBoxPlotMiddlePoints());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::DisplayAllBoxPlotPoints(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->ShowAllBoxPlotPoints(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::IsDisplayingAllBoxPlotPoints(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsShowingAllBoxPlotPoints());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::DisplayGraphDropShadows(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->DisplayDropShadows(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::IsDisplayingGraphDropShadows(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsDisplayingDropShadows());
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetLanguage(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetProjectLanguage(
            static_cast<readability::test_language>(luaL_checkinteger(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetLanguage(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetProjectLanguage()));
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetReviewer(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetReviewer(wxString{ luaL_checkstring(L, 2), wxConvUTF8 });
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetReviewer(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushstring(L, m_project->GetReviewer().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetStatus(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetStatus(wxString{ luaL_checkstring(L, 2), wxConvUTF8 });
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------
    int BatchProject::GetStatus(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushstring(L, m_project->GetStatus().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetTextStorageMethod(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetDocumentStorageMethod(static_cast<TextStorage>(luaL_checkinteger(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetTextStorageMethod(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetDocumentStorageMethod()));
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetTextSource(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetTextSource(static_cast<TextSource>(luaL_checkinteger(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetTextSource(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetTextSource()));
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetLongSentenceMethod(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetLongSentenceMethod(static_cast<LongSentence>(luaL_checkinteger(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetLongSentenceMethod(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetLongSentenceMethod()));
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetDifficultSentenceLength(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetDifficultSentenceLength(luaL_checkinteger(L, 2));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetDifficultSentenceLength(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, m_project->GetDifficultSentenceLength());
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetParagraphsParsingMethod(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetParagraphsParsingMethod(static_cast<ParagraphParse>(luaL_checkinteger(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetParagraphsParsingMethod(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetParagraphsParsingMethod()));
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::IgnoreBlankLines(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->IgnoreBlankLinesForParagraphsParser(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::IsIgnoringBlankLines(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsIgnoringBlankLinesForParagraphsParser());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::IgnoreIndenting(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->IgnoreIndentingForParagraphsParser(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::IsIgnoringIndenting(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsIgnoringIndentingForParagraphsParser());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetSentenceStartMustBeUppercased(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetSentenceStartMustBeUppercased(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SentenceStartMustBeUppercased(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->GetSentenceStartMustBeUppercased());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetMinDocWordCountForBatch(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetMinDocWordCountForBatch(luaL_checkinteger(L, 2));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetMinDocWordCountForBatch(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetMinDocWordCountForBatch()));
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetFilePathDisplayMode(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetFilePathTruncationMode(
            static_cast<Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode>(
                luaL_checkinteger(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::GetFilePathDisplayMode(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetFilePathTruncationMode()));
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::SetSpellCheckerOptions(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        if (lua_gettop(L) >= 2)
            {
            m_project->SpellCheckIgnoreProperNouns(int_to_bool(lua_toboolean(L, 2)));
            }
        if (lua_gettop(L) >= 3)
            {
            m_project->SpellCheckIgnoreUppercased(int_to_bool(lua_toboolean(L, 3)));
            }
        if (lua_gettop(L) >= 4)
            {
            m_project->SpellCheckIgnoreNumerals(int_to_bool(lua_toboolean(L, 4)));
            }
        if (lua_gettop(L) >= 5)
            {
            m_project->SpellCheckIgnoreFileAddresses(int_to_bool(lua_toboolean(L, 5)));
            }
        if (lua_gettop(L) >= 6)
            {
            m_project->SpellCheckIgnoreProgrammerCode(int_to_bool(lua_toboolean(L, 6)));
            }
        if (lua_gettop(L) >= 7)
            {
            m_project->SpellCheckAllowColloquialisms(int_to_bool(lua_toboolean(L, 7)));
            }
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetSummaryStatsResultsOptions(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        if (lua_gettop(L) >= 2)
            {
            m_project->GetStatisticsInfo().EnableReport(int_to_bool(lua_toboolean(L, 2)));
            }
        if (lua_gettop(L) >= 3)
            {
            m_project->GetStatisticsInfo().EnableTable(int_to_bool(lua_toboolean(L, 3)));
            }
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetSummaryStatsReportOptions(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        if (lua_gettop(L) >= 2)
            {
            m_project->GetStatisticsReportInfo().EnableParagraph(int_to_bool(lua_toboolean(L, 2)));
            }
        if (lua_gettop(L) >= 3)
            {
            m_project->GetStatisticsReportInfo().EnableWords(int_to_bool(lua_toboolean(L, 3)));
            }
        if (lua_gettop(L) >= 4)
            {
            m_project->GetStatisticsReportInfo().EnableSentences(int_to_bool(lua_toboolean(L, 4)));
            }
        if (lua_gettop(L) >= 5)
            {
            m_project->GetStatisticsReportInfo().EnableExtendedWords(
                int_to_bool(lua_toboolean(L, 5)));
            }
        if (lua_gettop(L) >= 6)
            {
            m_project->GetStatisticsReportInfo().EnableGrammar(int_to_bool(lua_toboolean(L, 6)));
            }
        if (lua_gettop(L) >= 7)
            {
            m_project->GetStatisticsReportInfo().EnableNotes(int_to_bool(lua_toboolean(L, 7)));
            }
        if (lua_gettop(L) >= 8)
            {
            m_project->GetStatisticsReportInfo().EnableExtendedInformation(
                int_to_bool(lua_toboolean(L, 8)));
            }
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetSummaryStatsDolchReportOptions(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        if (lua_gettop(L) >= 2)
            {
            m_project->GetStatisticsReportInfo().EnableDolchCoverage(
                int_to_bool(lua_toboolean(L, 2)));
            }
        if (lua_gettop(L) >= 3)
            {
            m_project->GetStatisticsReportInfo().EnableDolchWords(int_to_bool(lua_toboolean(L, 3)));
            }
        if (lua_gettop(L) >= 4)
            {
            m_project->GetStatisticsReportInfo().EnableDolchExplanation(
                int_to_bool(lua_toboolean(L, 4)));
            }
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetWordsBreakdownResultsOptions(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        if (lua_gettop(L) >= 2)
            {
            m_project->GetWordsBreakdownInfo().EnableWordBarchart(int_to_bool(lua_toboolean(L, 2)));
            }
        if (lua_gettop(L) >= 3)
            {
            m_project->GetWordsBreakdownInfo().EnableSyllableHistogram(
                int_to_bool(lua_toboolean(L, 3)));
            }
        if (lua_gettop(L) >= 4)
            {
            m_project->GetWordsBreakdownInfo().Enable3PlusSyllables(
                int_to_bool(lua_toboolean(L, 4)));
            }
        if (lua_gettop(L) >= 5)
            {
            m_project->GetWordsBreakdownInfo().Enable6PlusCharacter(
                int_to_bool(lua_toboolean(L, 5)));
            }
        if (lua_gettop(L) >= 6)
            {
            m_project->GetWordsBreakdownInfo().EnableWordCloud(int_to_bool(lua_toboolean(L, 6)));
            }
        if (lua_gettop(L) >= 7)
            {
            m_project->GetWordsBreakdownInfo().EnableDCUnfamiliar(int_to_bool(lua_toboolean(L, 7)));
            }
        if (lua_gettop(L) >= 8)
            {
            m_project->GetWordsBreakdownInfo().EnableSpacheUnfamiliar(
                int_to_bool(lua_toboolean(L, 8)));
            }
        if (lua_gettop(L) >= 9)
            {
            m_project->GetWordsBreakdownInfo().EnableHarrisJacobsonUnfamiliar(
                int_to_bool(lua_toboolean(L, 9)));
            }
        if (lua_gettop(L) >= 10)
            {
            m_project->GetWordsBreakdownInfo().EnableCustomTestsUnfamiliar(
                int_to_bool(lua_toboolean(L, 10)));
            }
        if (lua_gettop(L) >= 11)
            {
            m_project->GetWordsBreakdownInfo().EnableAllWords(int_to_bool(lua_toboolean(L, 11)));
            }
        if (lua_gettop(L) >= 12)
            {
            m_project->GetWordsBreakdownInfo().EnableKeyWords(int_to_bool(lua_toboolean(L, 12)));
            }
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetSentenceBreakdownResultsOptions(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        if (lua_gettop(L) >= 2)
            {
            m_project->GetSentencesBreakdownInfo().EnableLongSentences(
                int_to_bool(lua_toboolean(L, 2)));
            }
        if (lua_gettop(L) >= 3)
            {
            m_project->GetSentencesBreakdownInfo().EnableLengthsBoxPlot(
                int_to_bool(lua_toboolean(L, 3)));
            }
        if (lua_gettop(L) >= 4)
            {
            m_project->GetSentencesBreakdownInfo().EnableLengthsHistogram(
                int_to_bool(lua_toboolean(L, 4)));
            }
        if (lua_gettop(L) >= 5)
            {
            m_project->GetSentencesBreakdownInfo().EnableLengthsHeatmap(
                int_to_bool(lua_toboolean(L, 5)));
            }
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SetGrammarResultsOptions(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        if (lua_gettop(L) >= 2)
            {
            m_project->GetGrammarInfo().EnableHighlightedReport(int_to_bool(lua_toboolean(L, 2)));
            }
        if (lua_gettop(L) >= 3)
            {
            m_project->GetGrammarInfo().EnableWordingErrors(int_to_bool(lua_toboolean(L, 3)));
            }
        if (lua_gettop(L) >= 4)
            {
            m_project->GetGrammarInfo().EnableMisspellings(int_to_bool(lua_toboolean(L, 4)));
            }
        if (lua_gettop(L) >= 5)
            {
            m_project->GetGrammarInfo().EnableRepeatedWords(int_to_bool(lua_toboolean(L, 5)));
            }
        if (lua_gettop(L) >= 6)
            {
            m_project->GetGrammarInfo().EnableArticleMismatches(int_to_bool(lua_toboolean(L, 6)));
            }
        if (lua_gettop(L) >= 7)
            {
            m_project->GetGrammarInfo().EnableRedundantPhrases(int_to_bool(lua_toboolean(L, 7)));
            }
        if (lua_gettop(L) >= 8)
            {
            m_project->GetGrammarInfo().EnableOverUsedWordsBySentence(
                int_to_bool(lua_toboolean(L, 8)));
            }
        if (lua_gettop(L) >= 9)
            {
            m_project->GetGrammarInfo().EnableWordyPhrases(int_to_bool(lua_toboolean(L, 9)));
            }
        if (lua_gettop(L) >= 10)
            {
            m_project->GetGrammarInfo().EnableCliches(int_to_bool(lua_toboolean(L, 10)));
            }
        if (lua_gettop(L) >= 11)
            {
            m_project->GetGrammarInfo().EnablePassiveVoice(int_to_bool(lua_toboolean(L, 11)));
            }
        if (lua_gettop(L) >= 12)
            {
            m_project->GetGrammarInfo().EnableConjunctionStartingSentences(
                int_to_bool(lua_toboolean(L, 12)));
            }
        if (lua_gettop(L) >= 13)
            {
            m_project->GetGrammarInfo().EnableLowercaseSentences(int_to_bool(lua_toboolean(L, 13)));
            }
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::AddTest(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            lua_pushboolean(L, false);
            return 1;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            lua_pushboolean(L, false);
            return 1;
            }

        const wxString testName{ luaL_checkstring(L, 2), wxConvUTF8 };

        if (m_delayReloading)
            {
            if (m_project->GetReadabilityTests().has_test(testName))
                {
                m_project->GetReadabilityTests().include_test(
                    m_project->GetReadabilityTests().get_test_id(testName).c_str(), true);
                }
            else if (testName.CmpNoCase(_DT(L"dolch")) == 0)
                {
                m_project->IncludeDolchSightWords();
                }
            else
                {
                const auto customTestPos = std::find_if(
                    MainFrame::GetCustomTestMenuIds().cbegin(),
                    MainFrame::GetCustomTestMenuIds().cend(),
                    [&testName](const auto& test) { return test.second.CmpNoCase(testName) == 0; });
                if (customTestPos != MainFrame::GetCustomTestMenuIds().cend())
                    {
                    m_project->AddCustomReadabilityTest(testName, false);
                    }
                else
                    {
                    wxMessageBox(
                        wxString::Format(_(L"%s: unknown test could not be added."), testName),
                        _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
                    lua_pushboolean(L, false);
                    return 1;
                    }
                }
            }
        else
            {
            auto* view = dynamic_cast<BatchProjectView*>(m_project->GetFirstView());
            if (view != nullptr)
                {
                if (m_project->GetReadabilityTests().has_test(testName))
                    {
                    auto result = m_project->GetReadabilityTests().find_test(testName);
                    wxCommandEvent cmd(0, result.first->get_test().get_interface_id());
                    view->OnAddTest(cmd);
                    }
                else if (testName.CmpNoCase(_DT(L"dolch")) == 0)
                    {
                    wxCommandEvent cmd(0, XRCID("ID_DOLCH"));
                    view->OnAddTest(cmd);
                    }
                else
                    {
                    const auto customTestPos = std::find_if(
                        MainFrame::GetCustomTestMenuIds().cbegin(),
                        MainFrame::GetCustomTestMenuIds().cend(), [&testName](const auto& test)
                        { return test.second.CmpNoCase(testName) == 0; });
                    if (customTestPos != MainFrame::GetCustomTestMenuIds().cend())
                        {
                        wxCommandEvent cmd(0, customTestPos->first);
                        view->GetDocFrame()->OnCustomTest(cmd);
                        }
                    else
                        {
                        wxMessageBox(
                            wxString::Format(_(L"%s: unknown test could not be added."), testName),
                            _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
                        lua_pushboolean(L, false);
                        return 1;
                        }
                    }
                lua_pushboolean(L, true);
                return 1;
                }
            }

        lua_pushboolean(L, false);
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::Reload([[maybe_unused]] lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        m_project->RefreshRequired(ProjectRefresh::FullReindexing);
        m_project->RefreshProject();
        m_delayReloading = false;
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::Close(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        wxGetApp().Yield();

        // save it or turn off modified flag before closing
        if (m_project->IsModified())
            {
            if (lua_gettop(L) > 1 && int_to_bool(lua_toboolean(L, 2)))
                {
                if (!m_project->Save())
                    {
                    return 0;
                    }
                }
            else
                {
                m_project->Modify(false);
                }
            }

        m_project->GetDocumentManager()->CloseDocument(m_project, true);
        // the view won't be fully deleted until idle processing takes place
        wxGetApp().ProcessIdle();
        m_project = nullptr;
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::ExportList(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 2, __func__))
            {
            return 0;
            }

        auto* view = dynamic_cast<BatchProjectView*>(m_project->GetFirstView());
        if (view != nullptr)
            {
            Wisteria::UI::ListCtrlEx* listWindow = nullptr;
            const auto listId = ReadabilityApp::GetDynamicIdMap().find(luaL_checkinteger(L, 2));
            if (listId == ReadabilityApp::GetDynamicIdMap().cend())
                {
                wxMessageBox(
                    wxString::Format(_(L"Unable to find the specified list (%d) in the project."),
                                     static_cast<int>(luaL_checkinteger(L, 2))),
                    _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
                return 0;
                }
            const auto [parentPos, childPos] = view->GetSideBar()->FindSubItem(listId->second);
            if (parentPos.has_value() && childPos.has_value())
                {
                view->GetSideBar()->SelectSubItem(parentPos.value(), childPos.value());
                wxWindow* selWindow = view->GetActiveProjectWindow();
                if (selWindow && selWindow->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx)))
                    {
                    listWindow = dynamic_cast<Wisteria::UI::ListCtrlEx*>(selWindow);
                    }
                }
            else
                {
                const auto index = view->GetSideBar()->FindFolder(listId->second);
                if (index.has_value())
                    {
                    view->GetSideBar()->SelectFolder(index.value());
                    wxWindow* selWindow = view->GetActiveProjectWindow();
                    if (selWindow && selWindow->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx)))
                        {
                        listWindow = dynamic_cast<Wisteria::UI::ListCtrlEx*>(selWindow);
                        }
                    }
                }
            if (listWindow)
                {
                const BatchProjectDoc* doc = dynamic_cast<BatchProjectDoc*>(view->GetDocument());
                const wxString originalLabel = listWindow->GetLabel();
                listWindow->SetLabel(
                    originalLabel +
                    wxString::Format(L" [%s]", wxFileName::StripExtension(doc->GetTitle())));
                Wisteria::UI::GridExportOptions exportOptions;
                exportOptions.m_fromRow = (lua_gettop(L) > 3) ? luaL_checkinteger(L, 4) : 1;
                exportOptions.m_toRow = (lua_gettop(L) > 4) ? luaL_checkinteger(L, 5) : -1;
                exportOptions.m_fromColumn = (lua_gettop(L) > 5) ? luaL_checkinteger(L, 6) : 1;
                exportOptions.m_toColumn = (lua_gettop(L) > 6) ? luaL_checkinteger(L, 7) : -1;
                exportOptions.m_includeColumnHeaders =
                    (lua_gettop(L) > 7) ? int_to_bool(lua_toboolean(L, 8)) : true;
                exportOptions.m_pageUsingPrinterSettings =
                    (lua_gettop(L) > 8) ? int_to_bool(lua_toboolean(L, 9)) : false;
                lua_pushboolean(
                    L, listWindow->Save(wxString{ luaL_checklstring(L, 3, nullptr), wxConvUTF8 },
                                        exportOptions));
                listWindow->SetLabel(originalLabel);
                listWindow->SetFocus();
                wxGetApp().Yield();
                return 1;
                }
            }
        wxGetApp().Yield();
        lua_pushboolean(L, false);
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::ExportGraph(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 3, __func__))
            {
            return 0;
            }

        auto* view = dynamic_cast<BatchProjectView*>(m_project->GetFirstView());
        if (view != nullptr)
            {
            Wisteria::Canvas* graphWindow = nullptr;
            const auto sectionId = ReadabilityApp::GetDynamicIdMap().find(luaL_checkinteger(L, 2));
            if (sectionId == ReadabilityApp::GetDynamicIdMap().cend())
                {
                wxMessageBox(wxString::Format(
                                 _(L"Unable to find the specified section (%d) in the project."),
                                 static_cast<int>(luaL_checkinteger(L, 2))),
                             _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
                return 0;
                }
            // Use mapped value or numeric value from a script, which may be a literal (unmapped)
            // ID. This can happen for test-specific graphs that are using the XRC-generated ID from
            // the test.
            wxWindowID userWindowId = luaL_checkinteger(L, 3);
            if (const auto windowMappedId = ReadabilityApp::GetDynamicIdMap().find(userWindowId);
                windowMappedId != ReadabilityApp::GetDynamicIdMap().cend())
                {
                userWindowId = windowMappedId->second;
                }

            const auto [parentPos, childPos] =
                view->GetSideBar()->FindSubItem(sectionId->second, userWindowId);
            if (parentPos.has_value() && childPos.has_value())
                {
                view->GetSideBar()->SelectSubItem(parentPos.value(), childPos.value());
                wxWindow* selWindow = view->GetActiveProjectWindow();
                if (selWindow && selWindow->IsKindOf(wxCLASSINFO(Wisteria::Canvas)))
                    {
                    graphWindow = dynamic_cast<Wisteria::Canvas*>(selWindow);
                    }
                }
            if (graphWindow != nullptr)
                {
                const BatchProjectDoc* doc = dynamic_cast<BatchProjectDoc*>(view->GetDocument());
                const wxString originalLabel = graphWindow->GetLabel();
                graphWindow->SetLabel(
                    originalLabel +
                    wxString::Format(L" [%s]", wxFileName::StripExtension(doc->GetTitle())));
                Wisteria::UI::ImageExportOptions opt;
                if (lua_gettop(L) >= 5 && lua_toboolean(L, 5))
                    {
                    opt.m_mode = static_cast<decltype(opt.m_mode)>(
                        Wisteria::UI::ImageExportOptions::ColorMode::Greyscale);
                    }
                if (lua_gettop(L) >= 6)
                    {
                    opt.m_imageSize.SetWidth(luaL_checkinteger(L, 6));
                    }
                if (lua_gettop(L) >= 7)
                    {
                    opt.m_imageSize.SetHeight(luaL_checkinteger(L, 7));
                    }
                lua_pushboolean(
                    L,
                    graphWindow->Save(wxString(luaL_checklstring(L, 4, nullptr), wxConvUTF8), opt));
                graphWindow->SetLabel(originalLabel);
                wxGetApp().Yield();
                return 1;
                }
            wxMessageBox(
                wxString::Format(_(L"Unable to find the specified graph (%d) in the project."),
                                 static_cast<int>(luaL_checkinteger(L, 3))),
                _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
            lua_pushboolean(L, false);
            return 1;
            }
        wxGetApp().Yield();
        lua_pushboolean(L, false);
        return 1;
        }

    //-------------------------------------------------------------
    int BatchProject::ExportAll(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        const wxString outputPath(luaL_checkstring(L, 2), wxConvUTF8);
        auto* view = dynamic_cast<BatchProjectView*>(m_project->GetFirstView());
        if (view != nullptr)
            {
            view->ExportAll(outputPath, BaseProjectDoc::GetExportListExt(),
                            BaseProjectDoc::GetExportGraphExt(), true, true, true, true, true, true,
                            true, true, Wisteria::UI::ImageExportOptions());
            }
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::ShowSidebar(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        auto* view = dynamic_cast<BatchProjectView*>(m_project->GetFirstView());
        if (view != nullptr)
            {
            view->ShowSideBar(int_to_bool(lua_toboolean(L, 2)));
            }

        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SelectWindow(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        auto* view = dynamic_cast<BatchProjectView*>(m_project->GetFirstView());
        if (view != nullptr)
            {
            view->Activate(true);
            const auto sectionId = ReadabilityApp::GetDynamicIdMap().find(luaL_checkinteger(L, 2));
            if (sectionId == ReadabilityApp::GetDynamicIdMap().cend())
                {
                wxMessageBox(wxString::Format(
                                 _(L"Unable to find the specified section (%d) in the project."),
                                 static_cast<int>(luaL_checkinteger(L, 2))),
                             _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
                return 0;
                }
            std::optional<wxWindowID> windowId{ std::nullopt };
            // selecting section and subwindow
            if (lua_gettop(L) > 2)
                {
                const auto userWindowId =
                    ReadabilityApp::GetDynamicIdMap().find(luaL_checkinteger(L, 3));
                if (userWindowId == ReadabilityApp::GetDynamicIdMap().cend())
                    {
                    wxMessageBox(wxString::Format(
                                     _(L"Unable to find the specified window (%d) in the project."),
                                     static_cast<int>(luaL_checkinteger(L, 3))),
                                 _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
                    return 0;
                    }
                windowId = userWindowId->second;
                }
            const auto [parentPos, childPos] =
                windowId ? view->GetSideBar()->FindSubItem(sectionId->second, windowId.value()) :
                           view->GetSideBar()->FindSubItem(sectionId->second);
            if (parentPos.has_value() && childPos.has_value())
                {
                view->GetSideBar()->CollapseAll();
                view->GetSideBar()->SelectSubItem(parentPos.value(), childPos.value());
                }
            const auto index = view->GetSideBar()->FindFolder(sectionId->second);
            if (index.has_value())
                {
                view->GetSideBar()->CollapseAll();
                view->GetSideBar()->SelectFolder(index.value());
                }
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::SortList(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        auto* view = dynamic_cast<BatchProjectView*>(m_project->GetFirstView());
        if (view != nullptr)
            {
            std::vector<std::pair<size_t, Wisteria::SortDirection>> columns;
            for (int i = 3; i <= lua_gettop(L); i += 2)
                {
                columns.emplace_back(
                    luaL_checkinteger(L, i) - 1,
                    static_cast<Wisteria::SortDirection>(luaL_checkinteger(L, i + 1)));
                }
            const auto windowOrSectionId =
                ReadabilityApp::GetDynamicIdMap().find(luaL_checkinteger(L, 2));
            if (windowOrSectionId == ReadabilityApp::GetDynamicIdMap().cend())
                {
                wxMessageBox(
                    wxString::Format(_(L"Unable to find the specified list (%d) in the project."),
                                     static_cast<int>(luaL_checkinteger(L, 2))),
                    _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
                return 0;
                }
            const auto [parentPos, childPos] =
                view->GetSideBar()->FindSubItem(windowOrSectionId->second);
            if (parentPos.has_value() && childPos.has_value())
                {
                view->GetSideBar()->SelectSubItem(parentPos.value(), childPos.value());
                wxWindow* selWindow = view->GetActiveProjectWindow();
                if (selWindow && selWindow->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx)))
                    {
                    dynamic_cast<Wisteria::UI::ListCtrlEx*>(selWindow)->SortColumns(columns);
                    }
                }
            else
                {
                const auto index = view->GetSideBar()->FindFolder(windowOrSectionId->second);
                if (index.has_value())
                    {
                    view->GetSideBar()->SelectFolder(index.value());
                    wxWindow* selWindow = view->GetActiveProjectWindow();
                    if (selWindow && selWindow->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx)))
                        {
                        dynamic_cast<Wisteria::UI::ListCtrlEx*>(selWindow)->SortColumns(columns);
                        }
                    }
                }
            }
        wxGetApp().Yield();
        return 0;
        }

    // hidden functions just used for screenshots
    //-------------------------------------------------------------
    int BatchProject::ShowProperties(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        if (m_settingsDlg == nullptr)
            {
            m_settingsDlg = new ToolsOptionsDlg(wxGetApp().GetMainFrame(), m_project);
            }

        const auto idPos = ReadabilityApp::GetDynamicIdMap().find(luaL_checkinteger(L, 2));
        if (idPos != ReadabilityApp::GetDynamicIdMap().cend())
            {
            m_settingsDlg->SelectPage(idPos->second);
            }
        else
            {
            m_settingsDlg->SelectPage(luaL_checkinteger(L, 2));
            }
        m_settingsDlg->Show();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int BatchProject::CloseProperties([[maybe_unused]] lua_State* L)
        {
        if (m_settingsDlg)
            {
            m_settingsDlg->Destroy();
            m_settingsDlg = nullptr;
            }
        return 0;
        }

    // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
    const char BatchProject::className[] = "BatchProject";

    Luna<BatchProject>::PropertyType BatchProject::properties[] = { { nullptr, nullptr, nullptr } };

    Luna<BatchProject>::FunctionType BatchProject::methods[] = {
        LUNA_DECLARE_METHOD(BatchProject, Close),
        LUNA_DECLARE_METHOD(BatchProject, LoadFolder),
        LUNA_DECLARE_METHOD(BatchProject, LoadFiles),
        LUNA_DECLARE_METHOD(BatchProject, GetTitle),
        LUNA_DECLARE_METHOD(BatchProject, SetWindowSize),
        LUNA_DECLARE_METHOD(BatchProject, GetWindowSize),
        LUNA_DECLARE_METHOD(BatchProject, DelayReloading),
        LUNA_DECLARE_METHOD(BatchProject, SetLanguage),
        LUNA_DECLARE_METHOD(BatchProject, GetLanguage),
        LUNA_DECLARE_METHOD(BatchProject, SetReviewer),
        LUNA_DECLARE_METHOD(BatchProject, GetReviewer),
        LUNA_DECLARE_METHOD(BatchProject, SetStatus),
        LUNA_DECLARE_METHOD(BatchProject, GetStatus),
        LUNA_DECLARE_METHOD(BatchProject, SetTextStorageMethod),
        LUNA_DECLARE_METHOD(BatchProject, SetParagraphsParsingMethod),
        LUNA_DECLARE_METHOD(BatchProject, GetParagraphsParsingMethod),
        LUNA_DECLARE_METHOD(BatchProject, GetLongSentenceMethod),
        LUNA_DECLARE_METHOD(BatchProject, SetLongSentenceMethod),
        LUNA_DECLARE_METHOD(BatchProject, GetDifficultSentenceLength),
        LUNA_DECLARE_METHOD(BatchProject, SetDifficultSentenceLength),
        LUNA_DECLARE_METHOD(BatchProject, IgnoreBlankLines),
        LUNA_DECLARE_METHOD(BatchProject, IsIgnoringBlankLines),
        LUNA_DECLARE_METHOD(BatchProject, IgnoreIndenting),
        LUNA_DECLARE_METHOD(BatchProject, IsIgnoringIndenting),
        LUNA_DECLARE_METHOD(BatchProject, SetSentenceStartMustBeUppercased),
        LUNA_DECLARE_METHOD(BatchProject, SentenceStartMustBeUppercased),
        LUNA_DECLARE_METHOD(BatchProject, GetTextStorageMethod),
        LUNA_DECLARE_METHOD(BatchProject, GetTextSource),
        LUNA_DECLARE_METHOD(BatchProject, SetTextSource),
        LUNA_DECLARE_METHOD(BatchProject, SetMinDocWordCountForBatch),
        LUNA_DECLARE_METHOD(BatchProject, GetMinDocWordCountForBatch),
        LUNA_DECLARE_METHOD(BatchProject, SetSpellCheckerOptions),
        LUNA_DECLARE_METHOD(BatchProject, SetSummaryStatsResultsOptions),
        LUNA_DECLARE_METHOD(BatchProject, SetSummaryStatsReportOptions),
        LUNA_DECLARE_METHOD(BatchProject, SetSummaryStatsDolchReportOptions),
        LUNA_DECLARE_METHOD(BatchProject, SetWordsBreakdownResultsOptions),
        LUNA_DECLARE_METHOD(BatchProject, SetSentenceBreakdownResultsOptions),
        LUNA_DECLARE_METHOD(BatchProject, SetGrammarResultsOptions),
        LUNA_DECLARE_METHOD(BatchProject, ExcludeFileAddress),
        LUNA_DECLARE_METHOD(BatchProject, SetPhraseExclusionList),
        LUNA_DECLARE_METHOD(BatchProject, GetPhraseExclusionList),
        LUNA_DECLARE_METHOD(BatchProject, SetBlockExclusionTags),
        LUNA_DECLARE_METHOD(BatchProject, GetBlockExclusionTags),
        LUNA_DECLARE_METHOD(BatchProject, GetNumeralSyllabication),
        LUNA_DECLARE_METHOD(BatchProject, SetNumeralSyllabication),
        LUNA_DECLARE_METHOD(BatchProject, IsFogUsingSentenceUnits),
        LUNA_DECLARE_METHOD(BatchProject, FogUseSentenceUnits),
        LUNA_DECLARE_METHOD(BatchProject, IncludeStockerCatholicSupplement),
        LUNA_DECLARE_METHOD(BatchProject, IsIncludingStockerCatholicSupplement),
        LUNA_DECLARE_METHOD(BatchProject, IncludeScoreSummaryReport),
        LUNA_DECLARE_METHOD(BatchProject, IsIncludingScoreSummaryReport),
        LUNA_DECLARE_METHOD(BatchProject, SetLongGradeScaleFormat),
        LUNA_DECLARE_METHOD(BatchProject, IsUsingLongGradeScaleFormat),
        LUNA_DECLARE_METHOD(BatchProject, GetReadingAgeDisplay),
        LUNA_DECLARE_METHOD(BatchProject, SetReadingAgeDisplay),
        LUNA_DECLARE_METHOD(BatchProject, GetGradeScale),
        LUNA_DECLARE_METHOD(BatchProject, SetGradeScale),
        LUNA_DECLARE_METHOD(BatchProject, GetFleschNumeralSyllabizeMethod),
        LUNA_DECLARE_METHOD(BatchProject, SetFleschNumeralSyllabizeMethod),
        LUNA_DECLARE_METHOD(BatchProject, GetFleschKincaidNumeralSyllabizeMethod),
        LUNA_DECLARE_METHOD(BatchProject, SetFleschKincaidNumeralSyllabizeMethod),
        LUNA_DECLARE_METHOD(BatchProject, GetHarrisJacobsonTextExclusionMode),
        LUNA_DECLARE_METHOD(BatchProject, SetHarrisJacobsonTextExclusionMode),
        LUNA_DECLARE_METHOD(BatchProject, GetDaleChallTextExclusionMode),
        LUNA_DECLARE_METHOD(BatchProject, SetDaleChallTextExclusionMode),
        LUNA_DECLARE_METHOD(BatchProject, GetDaleChallProperNounCountingMethod),
        LUNA_DECLARE_METHOD(BatchProject, SetDaleChallProperNounCountingMethod),
        LUNA_DECLARE_METHOD(BatchProject, IncludeExcludedPhraseFirstOccurrence),
        LUNA_DECLARE_METHOD(BatchProject, IsIncludingExcludedPhraseFirstOccurrence),
        LUNA_DECLARE_METHOD(BatchProject, SetAppendedDocumentFilePath),
        LUNA_DECLARE_METHOD(BatchProject, GetAppendedDocumentFilePath),
        LUNA_DECLARE_METHOD(BatchProject, UseRealTimeUpdate),
        LUNA_DECLARE_METHOD(BatchProject, IsRealTimeUpdating),
        LUNA_DECLARE_METHOD(BatchProject, AggressivelyExclude),
        LUNA_DECLARE_METHOD(BatchProject, SetTextExclusion),
        LUNA_DECLARE_METHOD(BatchProject, GetTextExclusion),
        LUNA_DECLARE_METHOD(BatchProject, SetIncludeIncompleteTolerance),
        LUNA_DECLARE_METHOD(BatchProject, GetIncludeIncompleteTolerance),
        LUNA_DECLARE_METHOD(BatchProject, ExcludeCopyrightNotices),
        LUNA_DECLARE_METHOD(BatchProject, ExcludeTrailingCitations),
        LUNA_DECLARE_METHOD(BatchProject, ExcludeFileAddress),
        LUNA_DECLARE_METHOD(BatchProject, ExcludeNumerals),
        LUNA_DECLARE_METHOD(BatchProject, ExcludeProperNouns),
        LUNA_DECLARE_METHOD(BatchProject, IsExcludingAggressively),
        LUNA_DECLARE_METHOD(BatchProject, IsExcludingCopyrightNotices),
        LUNA_DECLARE_METHOD(BatchProject, IsExcludingTrailingCitations),
        LUNA_DECLARE_METHOD(BatchProject, IsExcludingFileAddresses),
        LUNA_DECLARE_METHOD(BatchProject, IsExcludingNumerals),
        LUNA_DECLARE_METHOD(BatchProject, IsExcludingProperNouns),
        LUNA_DECLARE_METHOD(BatchProject, SetGraphColorScheme),
        LUNA_DECLARE_METHOD(BatchProject, GetGraphColorScheme),
        LUNA_DECLARE_METHOD(BatchProject, SetGraphBackgroundColor),
        LUNA_DECLARE_METHOD(BatchProject, ApplyGraphBackgroundFade),
        LUNA_DECLARE_METHOD(BatchProject, IsApplyingGraphBackgroundFade),
        LUNA_DECLARE_METHOD(BatchProject, SetGraphCommonImage),
        LUNA_DECLARE_METHOD(BatchProject, GetGraphCommonImage),
        LUNA_DECLARE_METHOD(BatchProject, SetPlotBackgroundImage),
        LUNA_DECLARE_METHOD(BatchProject, GetPlotBackgroundImage),
        LUNA_DECLARE_METHOD(BatchProject, SetPlotBackgroundImageEffect),
        LUNA_DECLARE_METHOD(BatchProject, GetPlotBackgroundImageEffect),
        LUNA_DECLARE_METHOD(BatchProject, SetPlotBackgroundImageFit),
        LUNA_DECLARE_METHOD(BatchProject, GetPlotBackgroundImageFit),
        LUNA_DECLARE_METHOD(BatchProject, SetPlotBackgroundImageOpacity),
        LUNA_DECLARE_METHOD(BatchProject, GetPlotBackgroundImageOpacity),
        LUNA_DECLARE_METHOD(BatchProject, SetWatermark),
        LUNA_DECLARE_METHOD(BatchProject, GetWatermark),
        LUNA_DECLARE_METHOD(BatchProject, SetGraphLogoImage),
        LUNA_DECLARE_METHOD(BatchProject, GetGraphLogoImage),
        LUNA_DECLARE_METHOD(BatchProject, SetPlotBackgroundColor),
        LUNA_DECLARE_METHOD(BatchProject, SetGraphInvalidRegionColor),
        LUNA_DECLARE_METHOD(BatchProject, SetRaygorStyle),
        LUNA_DECLARE_METHOD(BatchProject, GetRaygorStyle),
        LUNA_DECLARE_METHOD(BatchProject, ConnectFleschPoints),
        LUNA_DECLARE_METHOD(BatchProject, IsConnectingFleschPoints),
        LUNA_DECLARE_METHOD(BatchProject, IncludeFleschRulerDocGroups),
        LUNA_DECLARE_METHOD(BatchProject, IsIncludingFleschRulerDocGroups),
        LUNA_DECLARE_METHOD(BatchProject, UseEnglishLabelsForGermanLix),
        LUNA_DECLARE_METHOD(BatchProject, IsUsingEnglishLabelsForGermanLix),
        LUNA_DECLARE_METHOD(BatchProject, SetStippleShapeColor),
        LUNA_DECLARE_METHOD(BatchProject, SetPlotBackgroundColorOpacity),
        LUNA_DECLARE_METHOD(BatchProject, GetPlotBackgroundColorOpacity),
        LUNA_DECLARE_METHOD(BatchProject, SetStippleImage),
        LUNA_DECLARE_METHOD(BatchProject, GetStippleImage),
        LUNA_DECLARE_METHOD(BatchProject, SetStippleShape),
        LUNA_DECLARE_METHOD(BatchProject, GetStippleShape),
        LUNA_DECLARE_METHOD(BatchProject, SetXAxisFont),
        LUNA_DECLARE_METHOD(BatchProject, SetYAxisFont),
        LUNA_DECLARE_METHOD(BatchProject, SetGraphTopTitleFont),
        LUNA_DECLARE_METHOD(BatchProject, SetGraphBottomTitleFont),
        LUNA_DECLARE_METHOD(BatchProject, SetGraphLeftTitleFont),
        LUNA_DECLARE_METHOD(BatchProject, SetGraphRightTitleFont),
        LUNA_DECLARE_METHOD(BatchProject, DisplayBarChartLabels),
        LUNA_DECLARE_METHOD(BatchProject, SetBarChartBarColor),
        LUNA_DECLARE_METHOD(BatchProject, SetBarChartBarOpacity),
        LUNA_DECLARE_METHOD(BatchProject, SetBarChartBarEffect),
        LUNA_DECLARE_METHOD(BatchProject, GetBarChartBarEffect),
        LUNA_DECLARE_METHOD(BatchProject, GetBarChartBarOpacity),
        LUNA_DECLARE_METHOD(BatchProject, GetBarChartOrientation),
        LUNA_DECLARE_METHOD(BatchProject, IsDisplayingBarChartLabels),
        LUNA_DECLARE_METHOD(BatchProject, SetBarChartOrientation),
        LUNA_DECLARE_METHOD(BatchProject, SetHistogramBarColor),
        LUNA_DECLARE_METHOD(BatchProject, SetHistogramBarEffect),
        LUNA_DECLARE_METHOD(BatchProject, GetHistogramBarEffect),
        LUNA_DECLARE_METHOD(BatchProject, SetHistogramBarOpacity),
        LUNA_DECLARE_METHOD(BatchProject, GetHistogramBarOpacity),
        LUNA_DECLARE_METHOD(BatchProject, SetHistogramBinning),
        LUNA_DECLARE_METHOD(BatchProject, GetHistogramBinning),
        LUNA_DECLARE_METHOD(BatchProject, SetHistogramRounding),
        LUNA_DECLARE_METHOD(BatchProject, GetHistogramRounding),
        LUNA_DECLARE_METHOD(BatchProject, SetHistogramIntervalDisplay),
        LUNA_DECLARE_METHOD(BatchProject, GetHistogramIntervalDisplay),
        LUNA_DECLARE_METHOD(BatchProject, SetHistogramBinLabelDisplay),
        LUNA_DECLARE_METHOD(BatchProject, GetHistogramBinLabelDisplay),
        LUNA_DECLARE_METHOD(BatchProject, SetBoxPlotColor),
        LUNA_DECLARE_METHOD(BatchProject, SetBoxPlotEffect),
        LUNA_DECLARE_METHOD(BatchProject, GetBoxPlotEffect),
        LUNA_DECLARE_METHOD(BatchProject, SetBoxPlotOpacity),
        LUNA_DECLARE_METHOD(BatchProject, GetBoxPlotOpacity),
        LUNA_DECLARE_METHOD(BatchProject, DisplayBoxPlotLabels),
        LUNA_DECLARE_METHOD(BatchProject, IsDisplayingBoxPlotLabels),
        LUNA_DECLARE_METHOD(BatchProject, DisplayAllBoxPlotPoints),
        LUNA_DECLARE_METHOD(BatchProject, IsDisplayingAllBoxPlotPoints),
        LUNA_DECLARE_METHOD(BatchProject, ConnectBoxPlotMiddlePoints),
        LUNA_DECLARE_METHOD(BatchProject, IsConnectingBoxPlotMiddlePoints),
        LUNA_DECLARE_METHOD(BatchProject, DisplayGraphDropShadows),
        LUNA_DECLARE_METHOD(BatchProject, IsDisplayingGraphDropShadows),
        LUNA_DECLARE_METHOD(BatchProject, AddTest),
        LUNA_DECLARE_METHOD(BatchProject, Reload),
        LUNA_DECLARE_METHOD(BatchProject, ExportAll),
        LUNA_DECLARE_METHOD(BatchProject, ExportList),
        LUNA_DECLARE_METHOD(BatchProject, ExportGraph),
        LUNA_DECLARE_METHOD(BatchProject, SelectWindow),
        LUNA_DECLARE_METHOD(BatchProject, ShowSidebar),
        LUNA_DECLARE_METHOD(BatchProject, SortList),
        LUNA_DECLARE_METHOD(BatchProject, ShowProperties),
        LUNA_DECLARE_METHOD(BatchProject, CloseProperties),
        { nullptr, nullptr }
    };
    // NOLINTEND(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
    } // namespace LuaScripting

// NOLINTEND(readability-implicit-bool-conversion)
// NOLINTEND(readability-identifier-length)
