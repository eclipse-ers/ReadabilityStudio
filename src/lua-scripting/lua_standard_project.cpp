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

#include "lua_standard_project.h"
#include "../Wisteria-Dataviz/src/base/label.h"
#include "../Wisteria-Dataviz/src/base/reportbuilder.h"
#include "../Wisteria-Dataviz/src/import/html_encode.h"
#include "../app/readability_app.h"
#include "../projects/batch_project_doc.h"
#include "../projects/batch_project_view.h"
#include "../projects/standard_project_doc.h"
#include "../ui/dialogs/tools_options_dlg.h"
#include <wx/webview.h>

// NOLINTBEGIN(readability-identifier-length)
// NOLINTBEGIN(readability-implicit-bool-conversion)

wxDECLARE_APP(ReadabilityApp);

/* Note: with Luna, an extra boolean argument is passed into class functions at the front,
   this should be skipped over. It seems to be an indicator of the function being successfully
   called, but not sure about that.*/

namespace LuaScripting
    {
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
    const char StandardProject::className[] = "StandardProject";

    //-------------------------------------------------------------
    StandardProject::StandardProject(lua_State* L)
        {
        const auto createProject = [this](const wxString& folderPath)
        {
            // create a standard project and dump the text into it
            const wxList& templateList =
                wxGetApp().GetMainFrame()->GetDocumentManager()->GetTemplates();
            for (size_t i = 0; i < templateList.GetCount(); ++i)
                {
                wxDocTemplate* docTemplate =
                    dynamic_cast<wxDocTemplate*>(templateList.Item(i)->GetData());
                if (docTemplate &&
                    docTemplate->GetDocClassInfo()->IsKindOf(wxCLASSINFO(ProjectDoc)))
                    {
                    m_project = dynamic_cast<ProjectDoc*>(
                        docTemplate->CreateDocument(folderPath, wxDOC_NEW));
                    if (m_project != nullptr)
                        {
                        // if a blank project (that will presumably have a document connected later)
                        // then we need to temporarily set the project to use (empty) manually
                        // entered text to suppress errors from analyzing an invalid file.
                        if (folderPath == L"EMPTY_PROJECT")
                            {
                            m_project->SetTextSource(TextSource::EnteredText);
                            }
                        if (!m_project->OnNewDocument())
                            {
                            // Document is implicitly deleted by DeleteAllViews
                            m_project->DeleteAllViews();
                            m_project = nullptr;
                            }
                        else if (folderPath == L"EMPTY_PROJECT")
                            {
                            m_project->SetTextSource(wxGetApp().GetAppOptions()->GetTextSource());
                            m_project->SetDocumentStorageMethod(
                                wxGetApp().GetAppOptions()->GetDocumentStorageMethod());
                            }
                        }
                    break;
                    }
                }
        };

        if (lua_gettop(L) > 1) // see if a path was passed in
            {
            const wxString path(luaL_checkstring(L, 2), wxConvUTF8);
            wxFileName fn(path);
            fn.Normalize(wxPATH_NORM_LONG | wxPATH_NORM_DOTS | wxPATH_NORM_TILDE |
                         wxPATH_NORM_ABSOLUTE);
            if (fn.GetExt().CmpNoCase(_DT(L"rsp")) == 0)
                {
                m_project = dynamic_cast<ProjectDoc*>(wxGetApp().GetMainFrame()->OpenFile(path));
                }
            else if (fn.GetExt().CmpNoCase(_DT(L"rsbp")) == 0)
                {
                m_project = nullptr;
                wxMessageBox(_(L"A standard project cannot open a batch project file."),
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
    wxString StandardProject::GetDebugSummary() const
        {
        if (m_project == nullptr)
            {
            return {};
            }

        wxString summary;
        if (m_project->GetTextSource() == TextSource::EnteredText)
            {
            summary = _(L"embedded text");
            }
        else if (!m_project->GetOriginalDocumentFilePath().empty())
            {
            summary =
                L"'" + wxFileName(m_project->GetOriginalDocumentFilePath()).GetFullName() + L"'";
            }

        if (!m_project->GetAppendedDocumentFilePath().empty())
            {
            if (!summary.empty())
                {
                summary += L" + ";
                }
            summary +=
                L"'" + wxFileName(m_project->GetAppendedDocumentFilePath()).GetFullName() + L"'";
            }

        return summary;
        }

    //-------------------------------------------------------------
    bool StandardProject::ReloadIfNotDelayed()
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
        return false;
        }

    //-------------------------------------------------------------
    bool StandardProject::ReloadIfNotDelayedSimple()
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
        return false;
        }

    //-------------------------------------------------------------
    int StandardProject::DelayReloading(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        m_delayReloading = (lua_gettop(L) > 1) ? int_to_bool(lua_toboolean(L, 2)) : true;
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::GetTitle(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushstring(L, m_project->GetTitle().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetWindowSize(lua_State* L)
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
    int StandardProject::GetWindowSize(lua_State* L)
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
    int StandardProject::GetSentenceCount(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushinteger(L, m_project->GetTotalSentences());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::GetIndependentClauseCount(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushinteger(L, m_project->GetTotalSentenceUnits());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::GetNumeralCount(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushinteger(L, m_project->GetTotalNumerals());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::GetProperNounCount(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (m_project->GetProjectLanguage() == readability::test_language::german_test)
            {
            wxMessageBox(_(L"ProperNounCount() not supported for German projects."),
                         _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
            lua_pushinteger(L, 0);
            return 1;
            }

        lua_pushinteger(L, m_project->GetTotalProperNouns());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::GetUniqueWordCount(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushinteger(L, m_project->GetTotalUniqueWords());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::GetWordCount(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushinteger(L, m_project->GetTotalWords());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::GetCharacterAndPunctuationCount(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushinteger(L, m_project->GetTotalCharactersPlusPunctuation());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::GetCharacterCount(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushinteger(L, m_project->GetTotalCharacters());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::GetSyllableCount(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushinteger(L, m_project->GetTotalSyllables());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::GetUnique3SyllablePlusWordCount(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushinteger(L, m_project->GetTotalUnique3PlusSyllableWords());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::Get3SyllablePlusWordCount(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushinteger(L, m_project->GetTotal3PlusSyllabicWords());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::GetUnique1SyllableWordCount(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushinteger(L, m_project->GetTotalUniqueMonoSyllabicWords());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::Get1SyllableWordCount(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushinteger(L, m_project->GetTotalMonoSyllabicWords());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::Get7CharacterPlusWordCount(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushinteger(L, m_project->GetTotalHardLixRixWords());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::GetUnique6CharPlusWordCount(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushinteger(L, m_project->GetTotalUnique6CharsPlusWords());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::Get6CharacterPlusWordCount(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushinteger(L, m_project->GetTotalLongWords());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::GetUnfamiliarSpacheWordCount(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushinteger(L, m_project->GetTotalHardWordsSpache());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::GetUnfamiliarDCWordCount(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushinteger(L, m_project->GetTotalHardWordsDaleChall());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::GetUnfamiliarHJWordCount(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushinteger(L, m_project->GetTotalHardWordsHarrisJacobson());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetTextExclusion(lua_State* L)
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
    int StandardProject::GetTextExclusion(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushinteger(L, static_cast<int>(m_project->GetInvalidSentenceMethod()));
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetIncludeIncompleteTolerance(lua_State* L)
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
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::GetIncludeIncompleteTolerance(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, m_project->GetIncludeIncompleteSentencesIfLongerThanValue());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::AggressivelyExclude(lua_State* L)
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
    int StandardProject::ExcludeCopyrightNotices(lua_State* L)
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
    int StandardProject::ExcludeTrailingCitations(lua_State* L)
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
    int StandardProject::ExcludeFileAddress(lua_State* L)
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
    int StandardProject::ExcludeNumerals(lua_State* L)
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
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::ExcludeProperNouns(lua_State* L)
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
    int StandardProject::IsExcludingAggressively(lua_State* L)
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
    int StandardProject::IsExcludingCopyrightNotices(lua_State* L)
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
    int StandardProject::IsExcludingTrailingCitations(lua_State* L)
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
    int StandardProject::IsExcludingFileAddresses(lua_State* L)
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
    int StandardProject::IsExcludingNumerals(lua_State* L)
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
    int StandardProject::IsExcludingProperNouns(lua_State* L)
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
    int StandardProject::SetPhraseExclusionList(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetExcludedPhrasesPath(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        m_project->LoadExcludePhrases();
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::GetPhraseExclusionList(lua_State* L)
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
    int StandardProject::IncludeExcludedPhraseFirstOccurrence(lua_State* L)
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
    int StandardProject::IsIncludingExcludedPhraseFirstOccurrence(lua_State* L)
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
    int StandardProject::SetBlockExclusionTags(lua_State* L)
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
    int StandardProject::GetBlockExclusionTags(lua_State* L)
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
    int StandardProject::SetNumeralSyllabication(lua_State* L)
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
    int StandardProject::GetNumeralSyllabication(lua_State* L)
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
    int StandardProject::IsFogUsingSentenceUnits(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsFogUsingSentenceUnits());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::FogUseSentenceUnits(lua_State* L)
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
    int StandardProject::IncludeStockerCatholicSupplement(lua_State* L)
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
    int StandardProject::IsIncludingStockerCatholicSupplement(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsIncludingStockerCatholicSupplement());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::IncludeScoreSummaryReport(lua_State* L)
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
    int StandardProject::IsIncludingScoreSummaryReport(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsIncludingScoreSummaryReport());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetLongGradeScaleFormat(lua_State* L)
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
    int StandardProject::IsUsingLongGradeScaleFormat(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->GetReadabilityMessageCatalog().IsUsingLongGradeScaleFormat());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetReadingAgeDisplay(lua_State* L)
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
    int StandardProject::GetReadingAgeDisplay(lua_State* L)
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
    int StandardProject::SetGradeScale(lua_State* L)
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
    int StandardProject::GetGradeScale(lua_State* L)
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
    int StandardProject::SetFleschNumeralSyllabizeMethod(lua_State* L)
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
    int StandardProject::GetFleschNumeralSyllabizeMethod(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetFleschNumeralSyllabizeMethod()));
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetFleschKincaidNumeralSyllabizeMethod(lua_State* L)
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
    int StandardProject::SetHarrisJacobsonTextExclusionMode(lua_State* L)
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
    int StandardProject::GetHarrisJacobsonTextExclusionMode(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetHarrisJacobsonTextExclusionMode()));
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetDaleChallTextExclusionMode(lua_State* L)
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
    int StandardProject::GetDaleChallTextExclusionMode(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetDaleChallTextExclusionMode()));
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetDaleChallProperNounCountingMethod(lua_State* L)
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
    int StandardProject::GetDaleChallProperNounCountingMethod(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetDaleChallProperNounCountingMethod()));
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::GetFleschKincaidNumeralSyllabizeMethod(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetFleschKincaidNumeralSyllabizeMethod()));
        return 1;
        }

    //-------------------------------------------------
    int StandardProject::SetAppendedDocumentFilePath(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetAppendedDocumentFilePath(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------
    int StandardProject::GetAppendedDocumentFilePath(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushstring(L, m_project->GetAppendedDocumentFilePath().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::IsRealTimeUpdating(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsRealTimeUpdating());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::UseRealTimeUpdate(lua_State* L)
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

    //-------------------------------------------------
    int StandardProject::SetDocumentFilePath(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetOriginalDocumentFilePath(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------
    int StandardProject::GetDocumentFilePath(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushstring(L, m_project->GetOriginalDocumentFilePath().utf8_str());
        return 1;
        }

    // GRAPH OPTIONS
    //-------------------------------------------------
    int StandardProject::SetGraphBackgroundColor(lua_State* L)
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
    int StandardProject::ApplyGraphBackgroundFade(lua_State* L)
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
    int StandardProject::IsApplyingGraphBackgroundFade(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->GetGraphBackGroundLinearGradient());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetPlotBackgroundImage(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetPlotBackGroundImagePath(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::GetPlotBackgroundImage(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushstring(L, m_project->GetPlotBackGroundImagePath().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetGraphCommonImage(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetGraphCommonImagePath(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::GetGraphCommonImage(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushstring(L, m_project->GetGraphCommonImagePath().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetPlotBackgroundImageEffect(lua_State* L)
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
    int StandardProject::GetPlotBackgroundImageEffect(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetPlotBackGroundImageEffect()));
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetPlotBackgroundImageFit(lua_State* L)
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
    int StandardProject::GetPlotBackgroundImageFit(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetPlotBackGroundImageFit()));
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetPlotBackgroundImageOpacity(lua_State* L)
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
    int StandardProject::GetPlotBackgroundImageOpacity(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, m_project->GetPlotBackGroundImageOpacity());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetPlotBackgroundColor(lua_State* L)
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
    int StandardProject::SetGraphInvalidRegionColor(lua_State* L)
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
    int StandardProject::SetRaygorStyle(lua_State* L)
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
    int StandardProject::GetRaygorStyle(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetRaygorStyle()));
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::ConnectFleschPoints(lua_State* L)
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
    int StandardProject::IsConnectingFleschPoints(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->IsConnectingFleschPoints()));
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::IncludeFleschRulerDocGroups(lua_State* L)
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
    int StandardProject::IsIncludingFleschRulerDocGroups(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->IsIncludingFleschRulerDocGroups()));
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::UseEnglishLabelsForGermanLix(lua_State* L)
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
    int StandardProject::IsUsingEnglishLabelsForGermanLix(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->IsUsingEnglishLabelsForGermanLix()));
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetStippleShapeColor(lua_State* L)
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
    int StandardProject::ShowcaseKeyItems(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->ShowcaseKeyItems(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::IsShowcasingKeyItems(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsShowcasingKeyItems());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetPlotBackgroundColorOpacity(lua_State* L)
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
    int StandardProject::GetPlotBackgroundColorOpacity(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, m_project->GetPlotBackGroundColorOpacity());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetGraphColorScheme(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetGraphColorScheme(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::GetGraphColorScheme(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushstring(L, m_project->GetGraphColorScheme().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetWatermark(lua_State* L)
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
    int StandardProject::GetWatermark(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushstring(L, m_project->GetWatermark().m_label.utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetGraphLogoImage(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetWatermarkLogoPath(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::GetGraphLogoImage(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushstring(L, m_project->GetWatermarkLogoPath().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetStippleImage(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetStippleImagePath(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::GetStippleImage(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushstring(L, m_project->GetStippleImagePath().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetStippleShape(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetStippleShape(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayedSimple();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::GetStippleShape(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushstring(L, m_project->GetStippleShape().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetXAxisFont(lua_State* L)
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
    int StandardProject::SetYAxisFont(lua_State* L)
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
    int StandardProject::SetGraphTopTitleFont(lua_State* L)
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
    int StandardProject::SetGraphBottomTitleFont(lua_State* L)
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
    int StandardProject::SetGraphLeftTitleFont(lua_State* L)
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
    int StandardProject::SetGraphRightTitleFont(lua_State* L)
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
    int StandardProject::DisplayBarChartLabels(lua_State* L)
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
    int StandardProject::DisplayGraphDropShadows(lua_State* L)
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
    int StandardProject::IsDisplayingGraphDropShadows(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsDisplayingDropShadows());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetBarChartBarColor(lua_State* L)
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
    int StandardProject::SetBarChartBarOpacity(lua_State* L)
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
    int StandardProject::SetBarChartBarEffect(lua_State* L)
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
    int StandardProject::GetBarChartBarEffect(lua_State* L)
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
    int StandardProject::GetBarChartOrientation(lua_State* L)
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
    int StandardProject::GetBarChartBarOpacity(lua_State* L)
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
    int StandardProject::IsDisplayingBarChartLabels(lua_State* L)
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
    int StandardProject::SetBarChartOrientation(lua_State* L)
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
    int StandardProject::SetHistogramBarColor(lua_State* L)
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
    int StandardProject::SetHistogramBarOpacity(lua_State* L)
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
    int StandardProject::SetHistogramBarEffect(lua_State* L)
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
    int StandardProject::GetHistogramBarEffect(lua_State* L)
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
    int StandardProject::GetHistogramBarOpacity(lua_State* L)
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
    int StandardProject::SetHistogramBinning(lua_State* L)
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
    int StandardProject::GetHistogramBinning(lua_State* L)
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
    int StandardProject::SetHistogramIntervalDisplay(lua_State* L)
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
    int StandardProject::GetHistogramIntervalDisplay(lua_State* L)
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
    int StandardProject::SetHistogramRounding(lua_State* L)
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
    int StandardProject::GetHistogramRounding(lua_State* L)
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
    int StandardProject::SetHistogramBinLabelDisplay(lua_State* L)
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
    int StandardProject::GetHistogramBinLabelDisplay(lua_State* L)
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
    int StandardProject::SetBoxPlotColor(lua_State* L)
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
    int StandardProject::SetBoxPlotOpacity(lua_State* L)
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
    int StandardProject::SetBoxPlotEffect(lua_State* L)
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
    int StandardProject::GetBoxPlotEffect(lua_State* L)
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
    int StandardProject::GetBoxPlotOpacity(lua_State* L)
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
    int StandardProject::DisplayBoxPlotLabels(lua_State* L)
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
    int StandardProject::IsDisplayingBoxPlotLabels(lua_State* L)
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
    int StandardProject::ConnectBoxPlotMiddlePoints(lua_State* L)
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
    int StandardProject::IsConnectingBoxPlotMiddlePoints(lua_State* L)
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
    int StandardProject::DisplayAllBoxPlotPoints(lua_State* L)
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
    int StandardProject::IsDisplayingAllBoxPlotPoints(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsShowingAllBoxPlotPoints());
        wxGetApp().Yield();
        return 1;
        }

    // PROJECT SETTINGS
    //-------------------------------------------------------------
    int StandardProject::SetLanguage(lua_State* L)
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
    int StandardProject::GetLanguage(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetProjectLanguage()));
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetReviewer(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetReviewer(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::GetReviewer(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushstring(L, m_project->GetReviewer().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetStatus(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetStatus(wxString(luaL_checkstring(L, 2), wxConvUTF8));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::GetStatus(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushstring(L, m_project->GetStatus().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetTextStorageMethod(lua_State* L)
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
    int StandardProject::GetTextStorageMethod(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetDocumentStorageMethod()));
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetTextSource(lua_State* L)
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
    int StandardProject::GetTextSource(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetTextSource()));
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetLongSentenceMethod(lua_State* L)
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
    int StandardProject::GetLongSentenceMethod(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetLongSentenceMethod()));
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetDifficultSentenceLength(lua_State* L)
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
    int StandardProject::GetDifficultSentenceLength(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, m_project->GetDifficultSentenceLength());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetParagraphsParsingMethod(lua_State* L)
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
    int StandardProject::GetParagraphsParsingMethod(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetParagraphsParsingMethod()));
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::IgnoreBlankLines(lua_State* L)
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
    int StandardProject::IsIgnoringBlankLines(lua_State* L)
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
    int StandardProject::IgnoreIndenting(lua_State* L)
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
    int StandardProject::IsIgnoringIndenting(lua_State* L)
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
    int StandardProject::SetSentenceStartMustBeUppercased(lua_State* L)
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
    int StandardProject::SentenceStartMustBeUppercased(lua_State* L)
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
    int StandardProject::SetSpellCheckerOptions(lua_State* L)
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
    int StandardProject::SetSummaryStatsResultsOptions(lua_State* L)
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
    int StandardProject::SetSummaryStatsReportOptions(lua_State* L)
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
    int StandardProject::SetSummaryStatsDolchReportOptions(lua_State* L)
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
    int StandardProject::SetWordsBreakdownResultsOptions(lua_State* L)
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
    int StandardProject::SetSentenceBreakdownResultsOptions(lua_State* L)
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
    int StandardProject::SetGrammarResultsOptions(lua_State* L)
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
    int StandardProject::SetReportFont(lua_State* L)
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

        m_project->SetTextViewFont(fontInfo);
        m_project->SetTextFontColor(fontColor);
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::SetExcludedTextHighlightColor(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetExcludedTextHighlightColor(
            LoadColor(wxString{ luaL_checkstring(L, 2), wxConvUTF8 }));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::SetDifficultTextHighlightColor(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetTextHighlightColor(LoadColor(wxString{ luaL_checkstring(L, 2), wxConvUTF8 }));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::SetGrammarIssuesHighlightColor(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetDuplicateWordHighlightColor(
            LoadColor(wxString{ luaL_checkstring(L, 2), wxConvUTF8 }));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::SetWordyTextHighlightColor(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetWordyPhraseHighlightColor(
            LoadColor(wxString{ luaL_checkstring(L, 2), wxConvUTF8 }));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::SetTextHighlighting(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetTextHighlightMethod(static_cast<TextHighlight>(luaL_checkinteger(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::GetTextHighlighting(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushnumber(L, static_cast<int>(m_project->GetTextHighlightMethod()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::SetDolchConjunctionsColor(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetDolchConjunctionsColor(
            LoadColor(wxString{ luaL_checkstring(L, 2), wxConvUTF8 }));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::SetDolchPrepositionsColor(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetDolchPrepositionsColor(
            LoadColor(wxString{ luaL_checkstring(L, 2), wxConvUTF8 }));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::SetDolchPronounsColor(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetDolchPronounsColor(LoadColor(wxString{ luaL_checkstring(L, 2), wxConvUTF8 }));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::SetDolchAdverbsColor(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetDolchAdverbsColor(LoadColor(wxString{ luaL_checkstring(L, 2), wxConvUTF8 }));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::SetDolchAdjectivesColor(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetDolchAdjectivesColor(
            LoadColor(wxString{ luaL_checkstring(L, 2), wxConvUTF8 }));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::SetDolchVerbsColor(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetDolchVerbsColor(LoadColor(wxString{ luaL_checkstring(L, 2), wxConvUTF8 }));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::SetDolchNounsColor(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->SetDolchNounColor(LoadColor(wxString{ luaL_checkstring(L, 2), wxConvUTF8 }));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::HighlightDolchConjunctions(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->HighlightDolchConjunctions(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::IsHighlightingDolchConjunctions(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsHighlightingDolchConjunctions());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::HighlightDolchPrepositions(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->HighlightDolchPrepositions(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::IsHighlightingDolchPrepositions(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsHighlightingDolchPrepositions());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::HighlightDolchPronouns(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->HighlightDolchPronouns(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::IsHighlightingDolchPronouns(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsHighlightingDolchPronouns());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::HighlightDolchAdverbs(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->HighlightDolchAdverbs(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::IsHighlightingDolchAdverbs(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsHighlightingDolchAdverbs());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::HighlightDolchAdjectives(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->HighlightDolchAdjectives(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::IsHighlightingDolchAdjectives(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsHighlightingDolchAdjectives());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::HighlightDolchVerbs(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->HighlightDolchVerbs(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::IsHighlightingDolchVerbs(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsHighlightingDolchVerbs());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::HighlightDolchNouns(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        m_project->HighlightDolchNouns(int_to_bool(lua_toboolean(L, 2)));
        ReloadIfNotDelayed();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::IsHighlightingDolchNouns(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        lua_pushboolean(L, m_project->IsHighlightingDolchNouns());
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::AddTest(lua_State* L)
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

        const wxString testName(luaL_checkstring(L, 2), wxConvUTF8);

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
            auto* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
            if (view != nullptr)
                {
                if (m_project->GetReadabilityTests().has_test(testName))
                    {
                    const auto result = m_project->GetReadabilityTests().find_test(testName);
                    wxCommandEvent cmd(wxEVT_NULL, result.first->get_test().get_interface_id());
                    view->OnAddTest(cmd);
                    }
                else if (testName.CmpNoCase(_DT(L"dolch")) == 0)
                    {
                    wxCommandEvent cmd(wxEVT_NULL, XRCID("ID_DOLCH"));
                    view->OnAddTest(cmd);
                    }
                else
                    {
                    std::map<int, wxString>::const_iterator pos;
                    for (pos = MainFrame::GetCustomTestMenuIds().cbegin();
                         pos != MainFrame::GetCustomTestMenuIds().cend(); ++pos)
                        {
                        if (pos->second.CmpNoCase(testName) == 0)
                            {
                            break;
                            }
                        }
                    if (pos != MainFrame::GetCustomTestMenuIds().end())
                        {
                        m_project->AddCustomReadabilityTest(pos->second, true);
                        auto testIter = std::find(BaseProject::m_custom_word_tests.begin(),
                                                  BaseProject::m_custom_word_tests.end(), testName);
                        // find the test
                        if (testIter != BaseProject::m_custom_word_tests.end())
                            {
                            // projects will need to do a full re-indexing
                            m_project->RefreshRequired(ProjectRefresh::FullReindexing);
                            m_project->RefreshProject();
                            }
                        else
                            {
                            lua_pushboolean(L, false);
                            return 1;
                            }
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
                wxGetApp().Yield();
                lua_pushboolean(L, true);
                return 1;
                }
            }

        lua_pushboolean(L, false);
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::Reload([[maybe_unused]] lua_State* L)
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
    int StandardProject::Close(lua_State* L)
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
    int StandardProject::ExportAll(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        const wxString outputPath(luaL_checkstring(L, 2), wxConvUTF8);
        auto* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view != nullptr)
            {
            view->ExportAll(outputPath, BaseProjectDoc::GetExportListExt(),
                            BaseProjectDoc::GetExportTextViewExt(),
                            BaseProjectDoc::GetExportSummaryReportExt(),
                            BaseProjectDoc::GetExportGraphExt(), true, true, true, true, true, true,
                            true, true, Wisteria::UI::ImageExportOptions());
            }
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::ExportScores(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        auto* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view != nullptr)
            {
            auto* scoresWindow =
                dynamic_cast<ExplanationListCtrl*>(view->GetReadabilityResultsView().FindWindowById(
                    BaseProjectView::READABILITY_SCORES_PAGE_ID));
            if (scoresWindow)
                {
                const ProjectDoc* doc = dynamic_cast<ProjectDoc*>(view->GetDocument());
                const wxString originalLabel = scoresWindow->GetName();
                scoresWindow->SetLabel(
                    originalLabel +
                    wxString::Format(L" [%s]", wxFileName::StripExtension(doc->GetTitle())));
                lua_pushboolean(
                    L, scoresWindow->Save(wxString(luaL_checklstring(L, 2, nullptr), wxConvUTF8)));
                scoresWindow->SetLabel(originalLabel);
                wxGetApp().Yield();
                return 1;
                }

            wxMessageBox(_(L"Unable to find the scores in the project."), _(L"Script Error"),
                         wxOK | wxICON_EXCLAMATION);
            lua_pushboolean(L, false);
            return 1;
            }
        wxGetApp().Yield();
        lua_pushboolean(L, false);
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::ExportGraph(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 2, __func__))
            {
            return 0;
            }

        auto* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view != nullptr)
            {
            const auto idPos = ReadabilityApp::GetDynamicIdMap().find(luaL_checkinteger(L, 2));
            if (idPos == ReadabilityApp::GetDynamicIdMap().cend())
                {
                wxMessageBox(
                    wxString::Format(_(L"Unable to find the specified graph (%d) in the project."),
                                     static_cast<int>(luaL_checkinteger(L, 2))),
                    _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
                return 0;
                }
            auto* graphWindow = dynamic_cast<Wisteria::Canvas*>(
                view->GetReadabilityResultsView().FindWindowById(idPos->second));
            // look in stats summary section if not in the readability section
            if (graphWindow == nullptr)
                {
                graphWindow = dynamic_cast<Wisteria::Canvas*>(
                    view->GetSummaryView().FindWindowById(idPos->second));
                }
            // look in word section if not in the stats section
            if (graphWindow == nullptr)
                {
                graphWindow = dynamic_cast<Wisteria::Canvas*>(
                    view->GetWordsBreakdownView().FindWindowById(idPos->second));
                }
            // look in sentence section if not in the words section
            if (graphWindow == nullptr)
                {
                graphWindow = dynamic_cast<Wisteria::Canvas*>(
                    view->GetSentencesBreakdownView().FindWindowById(idPos->second));
                }
            // look in Dolch section if not in the stats summary section
            if (graphWindow == nullptr)
                {
                graphWindow = dynamic_cast<Wisteria::Canvas*>(
                    view->GetDolchSightWordsView().FindWindowById(idPos->second));
                }
            if (graphWindow != nullptr)
                {
                const ProjectDoc* doc = dynamic_cast<ProjectDoc*>(view->GetDocument());
                const wxString originalLabel = graphWindow->GetName();
                graphWindow->SetLabel(
                    originalLabel +
                    wxString::Format(L" [%s]", wxFileName::StripExtension(doc->GetTitle())));
                Wisteria::UI::ImageExportOptions opt;
                if (lua_gettop(L) >= 4 && lua_toboolean(L, 4))
                    {
                    opt.m_mode = static_cast<decltype(opt.m_mode)>(
                        Wisteria::UI::ImageExportOptions::ColorMode::Greyscale);
                    }
                if (lua_gettop(L) >= 5)
                    {
                    opt.m_imageSize.SetWidth(luaL_checkinteger(L, 5));
                    }
                if (lua_gettop(L) >= 6)
                    {
                    opt.m_imageSize.SetHeight(luaL_checkinteger(L, 6));
                    }
                lua_pushboolean(
                    L, graphWindow->Save(wxString{ luaL_checklstring(L, 3, nullptr), wxConvUTF8 },
                                         opt));
                graphWindow->SetLabel(originalLabel);
                wxGetApp().Yield();
                return 1;
                }

            wxMessageBox(
                wxString::Format(_(L"Unable to find the specified graph (%d) in the project."),
                                 static_cast<int>(luaL_checkinteger(L, 2))),
                _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
            lua_pushboolean(L, false);
            return 1;
            }
        wxGetApp().Yield();
        lua_pushboolean(L, false);
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::ExportHighlightedWords(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 2, __func__))
            {
            return 0;
            }

        auto* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view != nullptr)
            {
            wxWindowID windowId = luaL_checkinteger(L, 2);
            if (const auto windowMappedId =
                    ReadabilityApp::GetDynamicIdMap().find(luaL_checkinteger(L, 2));
                windowMappedId != ReadabilityApp::GetDynamicIdMap().cend())
                {
                windowId = windowMappedId->second;
                }
            auto* textWindow = dynamic_cast<wxWebView*>(
                view->GetWordsBreakdownView().FindWindowById(windowId, CLASSINFO(wxWebView)));
            // look in grammar section if not in the highlighted words section
            if (textWindow == nullptr)
                {
                textWindow =
                    dynamic_cast<wxWebView*>(view->GetGrammarView().FindWindowById(windowId));
                }
            // look in Dolch section if not in the grammar section
            if (textWindow == nullptr)
                {
                textWindow = dynamic_cast<wxWebView*>(
                    view->GetDolchSightWordsView().FindWindowById(windowId));
                }
            if (textWindow != nullptr)
                {
                const auto* doc = dynamic_cast<const ProjectDoc*>(view->GetDocument());
                const auto* buffers =
                    (doc != nullptr) ? doc->GetHighlightedTextBuffers().Find(textWindow->GetId()) :
                                       nullptr;
                const wxFileName filePath{ wxString{ luaL_checklstring(L, 3, nullptr),
                                                     wxConvUTF8 } };
                // create the folder for the filepath, if necessary
                wxFileName::Mkdir(filePath.GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
                // The paper-white RTF is served from the registry; HTML and PDF come
                // from the live webview. The file extension picks the format.
                bool saved = false;
                if (buffers != nullptr && filePath.GetExt().CmpNoCase(L"rtf") == 0)
                    {
                    wxFileName(filePath).SetPermissions(wxS_DEFAULT);
                    wxFile file(filePath.GetFullPath(), wxFile::write);
                    saved = file.IsOpened() && file.Write(buffers->m_rtf);
                    }
                else if (filePath.GetExt().CmpNoCase(L"pdf") == 0)
                    {
                    textWindow->PrintToPDF(filePath.GetFullPath());
                    saved = true;
                    }
                else
                    {
                    std::wstring htmlText = textWindow->GetPageSource().ToStdWstring();
                    lily_of_the_valley::html_format::strip_hyperlinks(htmlText);
                    if (!htmlText.starts_with(L"<!DOCTYPE"))
                        {
                        htmlText.insert(0, L"<!DOCTYPE html>\n");
                        }
                    wxFileName(filePath).SetPermissions(wxS_DEFAULT);
                    wxFile file(filePath.GetFullPath(), wxFile::write);
                    saved = file.IsOpened() && file.Write(htmlText);
                    }
                lua_pushboolean(L, saved);
                wxGetApp().Yield();
                return 1;
                }

            wxMessageBox(
                wxString::Format(
                    _(L"Unable to find the specified highlighted words (%d) in the project."),
                    static_cast<int>(luaL_checkinteger(L, 2))),
                _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
            lua_pushboolean(L, false);
            return 1;
            }
        wxGetApp().Yield();
        lua_pushboolean(L, false);
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::ExportReport(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 2, __func__))
            {
            return 0;
            }

        auto* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view != nullptr)
            {
            const ProjectDoc* doc = dynamic_cast<ProjectDoc*>(view->GetDocument());
            wxWindowID windowId = luaL_checkinteger(L, 2);
            if (const auto windowMappedId =
                    ReadabilityApp::GetDynamicIdMap().find(luaL_checkinteger(L, 2));
                windowMappedId != ReadabilityApp::GetDynamicIdMap().cend())
                {
                windowId = windowMappedId->second;
                }
            if (windowId == BaseProjectView::STATS_REPORT_PAGE_ID)
                {
                auto* window =
                    dynamic_cast<wxWebView*>(view->GetSummaryView().FindWindowById(windowId));
                if (window != nullptr)
                    {
                    const wxString filePath{ luaL_checklstring(L, 3, nullptr), wxConvUTF8 };
                    std::wstring htmlText = window->GetPageSource().ToStdWstring();
                    lily_of_the_valley::html_format::strip_hyperlinks(htmlText);
                    // create the folder for the filepath, if necessary
                    wxFileName::Mkdir(wxFileName{ filePath }.GetPath(), wxS_DIR_DEFAULT,
                                      wxPATH_MKDIR_FULL);
                    wxFileName(filePath).SetPermissions(wxS_DEFAULT);
                    wxFile file(filePath, wxFile::write);
                    lua_pushboolean(L, file.IsOpened() && file.Write(htmlText));
                    wxGetApp().Yield();
                    return 1;
                    }
                }
            else if (windowId == BaseProjectView::READABILITY_SCORES_SUMMARY_REPORT_PAGE_ID)
                {
                auto* window = dynamic_cast<wxWebView*>(
                    view->GetReadabilityResultsView().FindWindowById(windowId));
                if (window != nullptr)
                    {
                    const wxString filePath{ luaL_checklstring(L, 3, nullptr), wxConvUTF8 };
                    std::wstring htmlText = window->GetPageSource().ToStdWstring();
                    lily_of_the_valley::html_format::strip_hyperlinks(htmlText);
                    // create the folder for the filepath, if necessary
                    wxFileName::Mkdir(wxFileName{ filePath }.GetPath(), wxS_DIR_DEFAULT,
                                      wxPATH_MKDIR_FULL);
                    wxFileName(filePath).SetPermissions(wxS_DEFAULT);
                    wxFile file(filePath, wxFile::write);
                    lua_pushboolean(L, file.IsOpened() && file.Write(htmlText));
                    wxGetApp().Yield();
                    return 1;
                    }
                }
            else if (windowId == BaseProjectView::READABILITY_SCORES_PAGE_ID)
                {
                auto* window = dynamic_cast<ExplanationListCtrl*>(
                    view->GetReadabilityResultsView().FindWindowById(windowId));
                if (window != nullptr)
                    {
                    const wxString originalLabel = window->GetName();
                    window->SetLabel(
                        originalLabel +
                        wxString::Format(L" [%s]", wxFileName::StripExtension(doc->GetTitle())));
                    lua_pushboolean(
                        L, window->Save(wxString{ luaL_checklstring(L, 3, nullptr), wxConvUTF8 }));
                    window->SetLabel(originalLabel);
                    wxGetApp().Yield();
                    return 1;
                    }
                }
            else
                {
                wxMessageBox(
                    wxString::Format(_(L"Unable to find the specified report (%d) in the project."),
                                     static_cast<int>(luaL_checkinteger(L, 2))),
                    _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
                wxGetApp().Yield();
                lua_pushboolean(L, false);
                return 1;
                }
            }
        wxGetApp().Yield();
        lua_pushboolean(L, false);
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::ExportList(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 2, __func__))
            {
            return 0;
            }

        auto* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view != nullptr)
            {
            wxWindowID windowId = luaL_checkinteger(L, 2);
            if (const auto windowMappedId =
                    ReadabilityApp::GetDynamicIdMap().find(luaL_checkinteger(L, 2));
                windowMappedId != ReadabilityApp::GetDynamicIdMap().cend())
                {
                windowId = windowMappedId->second;
                }
            auto* listWindow = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
                view->GetWordsBreakdownView().FindWindowById(windowId));
            if (listWindow ==
                nullptr) // look in grammar section if not in the highlighted words section
                {
                listWindow = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
                    view->GetGrammarView().FindWindowById(windowId));
                }
            if (listWindow == nullptr) // look in sentences section if not in the grammar section
                {
                listWindow = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
                    view->GetSentencesBreakdownView().FindWindowById(windowId));
                }
            if (listWindow == nullptr) // look in Dolch section if not in the sentences section
                {
                listWindow = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
                    view->GetDolchSightWordsView().FindWindowById(windowId));
                }
            if (listWindow == nullptr) // look in stats section if not in the Dolch section
                {
                listWindow = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
                    view->GetSummaryView().FindWindowById(windowId));
                }
            if (listWindow != nullptr)
                {
                const ProjectDoc* doc = dynamic_cast<ProjectDoc*>(view->GetDocument());
                const wxString originalLabel = listWindow->GetName();
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
                wxGetApp().Yield();
                return 1;
                }

            wxMessageBox(
                wxString::Format(_(L"Unable to find the specified list (%d) in the project."),
                                 static_cast<int>(luaL_checkinteger(L, 2))),
                _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
            wxGetApp().Yield();
            lua_pushboolean(L, false);
            return 1;
            }
        wxGetApp().Yield();
        lua_pushboolean(L, false);
        return 1;
        }

    //-------------------------------------------------------------
    int StandardProject::ExportFilteredText(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 6, __func__))
            {
            return 0;
            }

        std::wstring filteredText;
        m_project->FormatFilteredText(
            filteredText, int_to_bool(lua_toboolean(L, 3)), int_to_bool(lua_toboolean(L, 4)),
            int_to_bool(lua_toboolean(L, 5)), int_to_bool(lua_toboolean(L, 6)),
            int_to_bool(lua_toboolean(L, 7)), int_to_bool(lua_toboolean(L, 8)));

        const wxString exportFilePath = wxString(luaL_checklstring(L, 2, nullptr), wxConvUTF8);
        wxFileName::Mkdir(wxFileName(exportFilePath).GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        wxFileName(exportFilePath).SetPermissions(wxS_DEFAULT);
        wxFile filteredFile(exportFilePath, wxFile::write);
        if (!filteredFile.Write(filteredText, wxConvUTF8))
            {
            wxLogError(L"Unable to write to output file.");
            lua_pushboolean(L, false);
            return 1;
            }
        lua_pushboolean(L, true);
        return 1;
        }

    //-------------------------------------------------------------
    bool StandardProject::FindAndSelectTextInWebView(wxWebView* webView, const wxString& searchText)
        {
        if (webView == nullptr || searchText.empty())
            {
            return false;
            }

        // escape for embedding in a single-quoted JS string literal
        wxString escapedSearchText{ searchText };
        escapedSearchText.Replace(L"\\", L"\\\\", true);
        escapedSearchText.Replace(L"'", L"\\'", true);
        escapedSearchText.Replace(L"\n", L"\\n", true);
        escapedSearchText.Replace(L"\r", L"\\r", true);

        // window.find() is a non-standard (but Chromium-supported) API that searches,
        // selects, and scrolls the match into view all in one call
        const wxString script = wxString::Format(LR"JS(
            (function() {
                window.getSelection().removeAllRanges();
                window.scrollTo(0, 0);
                return window.find('%s', false, false, true, false, true, false) ?
                    'true' : 'false';
                })();
            )JS",
                                                 escapedSearchText);

        wxString scriptOutput;
        return webView->RunScript(script, &scriptOutput) && scriptOutput == L"true";
        }

    //-------------------------------------------------------------
    int StandardProject::SelectHighlightedWordReport(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        auto* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view != nullptr)
            {
            view->Activate(true);
            wxWindowID windowId = luaL_checkinteger(L, 2);
            if (const auto windowMappedId =
                    ReadabilityApp::GetDynamicIdMap().find(luaL_checkinteger(L, 2));
                windowMappedId != ReadabilityApp::GetDynamicIdMap().cend())
                {
                windowId = windowMappedId->second;
                }

            view->GetSideBar()->CollapseAll();

            wxWindow* selWindow =
                view->GetWordsBreakdownView().FindWindowById(windowId, wxCLASSINFO(wxWebView));
            if (selWindow != nullptr && selWindow->IsKindOf(wxCLASSINFO(wxWebView)))
                {
                // Custom word-list tests have the same integral IDs for their highlighted-text
                // reports and list controls, so search by label instead.
                view->GetSideBar()->SelectSubItem(
                    view->GetSideBar()->FindSubItem(selWindow->GetName()));
                // selecting the report hides its webview and asynchronously reloads
                // its content, only re-showing it once the backend fires its loaded
                // event; wait for that before searching it
                for (int waitAttempts = 0; !selWindow->IsShown() && waitAttempts < 8;
                     ++waitAttempts)
                    {
                    wxMilliSleep(500);
                    wxGetApp().Yield();
                    }
                if (lua_gettop(L) >= 3)
                    {
                    const wxString contentToFind{ luaL_checkstring(L, 3), wxConvUTF8 };
                    if (!FindAndSelectTextInWebView(dynamic_cast<wxWebView*>(selWindow),
                                                    contentToFind))
                        {
                        lua_Debug ar{};
                        wxString lineInfo;
                        if (lua_getstack(L, 1, &ar) != 0 && lua_getinfo(L, "l", &ar) != 0 &&
                            ar.currentline > 0)
                            {
                            // quneiform-suppress-begin
                            lineInfo = wxString::Format(_(L" (chunk line #%d)"), ar.currentline);
                            // quneiform-suppress-end
                            }
                        DebugPrint(wxString::Format(
                            // TRANSLATORS: %s are formatting tags and
                            // should stay wrapped around "Warning"
                            _(L"⚠️%sWarning%s: unable to find \"%s\" in text window.") + lineInfo,
                            L"<span class='warning' style='font-weight:bold;'>", L"</span>",
                            wxString{ lily_of_the_valley::html_encode_text::simple_encode(
                                wxString{ contentToFind }
                                    .Truncate(10)
                                    .append(contentToFind.length() > 10 ? wxString{ _DT(L"...") } :
                                                                          wxString{})
                                    .ToStdWstring()) }));
                        DebugPrint(wxString{});
                        }
                    }
                selWindow->SetFocus();
                }
            }
        // yield so that the view can be fully refreshed before proceeding
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::ShowSidebar(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        auto* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view != nullptr)
            {
            view->ShowSideBar(int_to_bool(lua_toboolean(L, 2)));
            }

        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::SelectWindow(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        auto* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
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
                // If mapped ID not found, then use the numeric value from the script
                // as the literal ID. We need to do this for custom test report and list
                // windows because the script will convert their string to a dynamic ID
                // generated by the framework.
                wxWindowID userWindowId = luaL_checkinteger(L, 3);
                if (const auto windowMappedId =
                        ReadabilityApp::GetDynamicIdMap().find(userWindowId);
                    windowMappedId != ReadabilityApp::GetDynamicIdMap().cend())
                    {
                    userWindowId = windowMappedId->second;
                    }
                windowId = userWindowId;
                }
            const auto [parentPos, childPos] =
                windowId ? view->GetSideBar()->FindSubItem(sectionId->second, windowId.value()) :
                           view->GetSideBar()->FindSubItem(sectionId->second);
            if (parentPos.has_value() && childPos.has_value())
                {
                view->GetSideBar()->CollapseAll();
                view->GetSideBar()->SelectSubItem(parentPos.value(), childPos.value());
                }
            else
                {
                const auto index = view->GetSideBar()->FindFolder(sectionId->second);
                if (index.has_value())
                    {
                    view->GetSideBar()->CollapseAll();
                    view->GetSideBar()->SelectFolder(index.value());
                    }
                }
            view->GetActiveProjectWindow()->SetFocus();
            view->GetActiveProjectWindow()->Refresh();
            view->GetDocFrame()->Refresh();
            if (auto* activeWebView = dynamic_cast<wxWebView*>(view->GetActiveProjectWindow());
                activeWebView != nullptr)
                {
                for (int waitAttempts = 0; !activeWebView->IsShown() && waitAttempts < 8;
                     ++waitAttempts)
                    {
                    wxMilliSleep(500);
                    wxGetApp().Yield();
                    }
                }
            }
        // yield so that the view can be fully refreshed before proceeding
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::SelectReadabilityTest(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        auto* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view != nullptr)
            {
            view->Activate(true);
            const auto index = view->GetSideBar()->FindFolder(
                BaseProjectView::SIDEBAR_READABILITY_SCORES_SECTION_ID);
            if (index.has_value())
                {
                view->GetSideBar()->CollapseAll();
                view->GetSideBar()->SelectFolder(index.value());
                view->GetReadabilityScoresList()->GetResultsListCtrl()->Select(
                    luaL_checkinteger(L, 2) - 1 /* make it zero-indexed*/);
                }
            // selecting a row hides the explanation webview and asynchronously reloads
            // its content, only re-showing it once the backend fires its loaded event
            const auto* explanationView = view->GetReadabilityScoresList()->GetExplanationView();
            for (int waitAttempts = 0;
                 explanationView != nullptr && !explanationView->IsShown() && waitAttempts < 8;
                 ++waitAttempts)
                {
                wxMilliSleep(500);
                wxGetApp().Yield();
                }
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::SortList(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 2, __func__))
            {
            return 0;
            }

        auto* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view != nullptr)
            {
            wxWindowID windowId = luaL_checkinteger(L, 2);
            if (const auto windowMappedId =
                    ReadabilityApp::GetDynamicIdMap().find(luaL_checkinteger(L, 2));
                windowMappedId != ReadabilityApp::GetDynamicIdMap().cend())
                {
                windowId = windowMappedId->second;
                }
            // look in the words section
            auto* listWindow = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
                view->GetWordsBreakdownView().FindWindowById(windowId));
            // look in grammar section if not in the highlighted words section
            if (listWindow == nullptr)
                {
                listWindow = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
                    view->GetGrammarView().FindWindowById(windowId));
                }
            // look in Dolch section if not in the grammar section
            if (listWindow == nullptr)
                {
                listWindow = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
                    view->GetDolchSightWordsView().FindWindowById(windowId));
                }
            // look in stats section if not in the Dolch section
            if (listWindow == nullptr)
                {
                listWindow = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
                    view->GetSummaryView().FindWindowById(windowId));
                }
            // look in stats section if not in the sentences section
            if (listWindow == nullptr)
                {
                listWindow = dynamic_cast<Wisteria::UI::ListCtrlEx*>(
                    view->GetSentencesBreakdownView().FindWindowById(windowId));
                }
            if (listWindow != nullptr)
                {
                std::vector<std::pair<size_t, Wisteria::SortDirection>> columns;
                for (int i = 3; i <= lua_gettop(L); i += 2)
                    {
                    columns.emplace_back(
                        luaL_checkinteger(L, i) - 1 /* make it zero-indexed*/,
                        static_cast<Wisteria::SortDirection>(luaL_checkinteger(L, i + 1)));
                    }
                listWindow->SortColumns(columns);
                }
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::SortGraph(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 2, __func__))
            {
            return 0;
            }

        auto* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view != nullptr)
            {
            const auto graphID = ReadabilityApp::GetDynamicIdMap().find(luaL_checkinteger(L, 2));
            if (graphID == ReadabilityApp::GetDynamicIdMap().cend())
                {
                wxMessageBox(
                    wxString::Format(_(L"Unable to find the specified graph (%d) in the project."),
                                     static_cast<int>(luaL_checkinteger(L, 2))),
                    _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
                return 0;
                }
            auto* graphWindow = dynamic_cast<Wisteria::Canvas*>(
                view->GetSummaryView().FindWindowById(graphID->second));
            // look in Dolch section if not in the summary section
            if (graphWindow == nullptr)
                {
                graphWindow = dynamic_cast<Wisteria::Canvas*>(
                    view->GetDolchSightWordsView().FindWindowById(graphID->second));
                }
            // look in words section
            if (graphWindow == nullptr)
                {
                graphWindow = dynamic_cast<Wisteria::Canvas*>(
                    view->GetWordsBreakdownView().FindWindowById(graphID->second));
                }
            // look in sentences section
            if (graphWindow == nullptr)
                {
                graphWindow = dynamic_cast<Wisteria::Canvas*>(
                    view->GetSentencesBreakdownView().FindWindowById(graphID->second));
                }
            // look in grammar section
            if (graphWindow == nullptr)
                {
                graphWindow = dynamic_cast<Wisteria::Canvas*>(
                    view->GetGrammarView().FindWindowById(graphID->second));
                }
            if (graphWindow != nullptr)
                {
                std::dynamic_pointer_cast<Wisteria::Graphs::BarChart>(
                    graphWindow->GetFixedObject(0, 0))
                    ->SortBars(Wisteria::Graphs::BarChart::BarSortComparison::SortByBarLength,
                               static_cast<Wisteria::SortDirection>(luaL_checkinteger(L, 3)));
                }
            }
        // yield so that the view can be fully refreshed before proceeding
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::SelectRowsInWordsBreakdownList(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        auto* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view != nullptr)
            {
            view->Activate(true);
            wxWindowID windowId = luaL_checkinteger(L, 2);
            if (const auto windowMappedId =
                    ReadabilityApp::GetDynamicIdMap().find(luaL_checkinteger(L, 2));
                windowMappedId != ReadabilityApp::GetDynamicIdMap().cend())
                {
                windowId = windowMappedId->second;
                }

            view->GetSideBar()->CollapseAll();

            wxWindow* selWindow = view->GetWordsBreakdownView().FindWindowById(
                windowId, CLASSINFO(Wisteria::UI::ListCtrlEx));
            if (selWindow != nullptr && selWindow->IsKindOf(wxCLASSINFO(Wisteria::UI::ListCtrlEx)))
                {
                // Custom word-list tests have the same integral IDs for their highlighted-text
                // reports and list controls, so search by label instead.
                view->GetSideBar()->SelectSubItem(
                    view->GetSideBar()->FindSubItem(selWindow->GetLabel()));
                for (int i = 3; i <= lua_gettop(L); ++i)
                    {
                    dynamic_cast<Wisteria::UI::ListCtrlEx*>(selWindow)->Select(
                        luaL_checkinteger(L, i) - 1 /*make it zero-indexed*/);
                    }
                selWindow->SetFocus();
                }
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::ScrollTextWindow(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }
        if (!VerifyParameterCount(L, 2, __func__))
            {
            return 0;
            }

        auto* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view != nullptr)
            {
            view->Activate(true);
            wxWindowID windowId = luaL_checkinteger(L, 2);
            if (const auto windowMappedId =
                    ReadabilityApp::GetDynamicIdMap().find(luaL_checkinteger(L, 2));
                windowMappedId != ReadabilityApp::GetDynamicIdMap().cend())
                {
                windowId = windowMappedId->second;
                }
            auto item = view->GetSideBar()->FindSubItem(
                BaseProjectView::SIDEBAR_WORDS_BREAKDOWN_SECTION_ID, windowId);
            if (!item.second.has_value())
                {
                item = view->GetSideBar()->FindSubItem(BaseProjectView::SIDEBAR_GRAMMAR_SECTION_ID,
                                                       windowId);
                }
            if (item.second.has_value())
                {
                view->GetSideBar()->CollapseAll();

                wxWindow* selWindow = view->GetWordsBreakdownView().FindWindowById(windowId);
                if (selWindow == nullptr)
                    {
                    selWindow = view->GetGrammarView().FindWindowById(windowId);
                    }
                if (selWindow != nullptr && selWindow->IsKindOf(wxCLASSINFO(wxWebView)))
                    {
                    // Custom word-list tests have the same integral IDs for their highlighted-text
                    // reports and list controls, so search by label instead.
                    view->GetSideBar()->SelectSubItem(
                        view->GetSideBar()->FindSubItem(selWindow->GetName()));

                    const wxString contentToFind{ luaL_checkstring(L, 3), wxConvUTF8 };
                    // let the page finish rendering before searching it
                    ::wxSleep(2);
                    if (!FindAndSelectTextInWebView(dynamic_cast<wxWebView*>(selWindow),
                                                    contentToFind))
                        {
                        lua_Debug ar{};
                        wxString lineInfo;
                        if (lua_getstack(L, 1, &ar) != 0 && lua_getinfo(L, "l", &ar) != 0 &&
                            ar.currentline > 0)
                            {
                            // quneiform-suppress-begin
                            lineInfo = wxString::Format(_(L" (chunk line #%d)"), ar.currentline);
                            // quneiform-suppress-end
                            }
                        DebugPrint(wxString::Format( // TRANSLATORS: %s are formatting tags and
                                                     // should stay wrapped around "Warning"
                            _(L"⚠️%sWarning%s: unable to find \"%s\" in text window.") + lineInfo,
                            L"<span class='warning' style='font-weight:bold;'>", L"</span>",
                            wxString{ lily_of_the_valley::html_encode_text::simple_encode(
                                wxString{ contentToFind }
                                    .Truncate(10)
                                    .append(contentToFind.length() > 10 ? wxString{ _DT(L"...") } :
                                                                          wxString{})
                                    .ToStdWstring()) }));
                        DebugPrint(wxString{});
                        }
                    }
                }
            }
        // yield so that the view can be fully refreshed before proceeding
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int StandardProject::SelectTextGrammarWindow(lua_State* L)
        {
        if (!VerifyProjectIsOpen(__func__))
            {
            return 0;
            }

        auto* view = dynamic_cast<ProjectView*>(m_project->GetFirstView());
        if (view != nullptr)
            {
            view->Activate(true);
            const auto [parentPos, childPos] = view->GetSideBar()->FindSubItem(
                BaseProjectView::SIDEBAR_GRAMMAR_SECTION_ID,
                BaseProjectView::LONG_SENTENCES_AND_WORDINESS_TEXT_PAGE_ID);
            if (parentPos.has_value() && childPos.has_value())
                {
                if (lua_gettop(L) >= 1)
                    {
                    wxWindow* selWindow = view->GetGrammarView().FindWindowById(
                        BaseProjectView::LONG_SENTENCES_AND_WORDINESS_TEXT_PAGE_ID);
                    if (selWindow != nullptr && selWindow->IsKindOf(wxCLASSINFO(wxWebView)))
                        {
                        const wxString contentToFind{ luaL_checkstring(L, 2), wxConvUTF8 };
                        // let the page finish rendering before searching it
                        ::wxSleep(2);
                        if (!FindAndSelectTextInWebView(dynamic_cast<wxWebView*>(selWindow),
                                                        contentToFind))
                            {
                            lua_Debug ar{};
                            wxString lineInfo;
                            if (lua_getstack(L, 1, &ar) != 0 && lua_getinfo(L, "l", &ar) != 0 &&
                                ar.currentline > 0)
                                {
                                // quneiform-suppress-begin
                                lineInfo = wxString::Format(L" (chunk line #%d)", ar.currentline);
                                // quneiform-suppress-end
                                }
                            DebugPrint(wxString::Format(
                                // TRANSLATORS: %s are formatting tags and
                                // should stay wrapped around "Warning"
                                _(L"⚠️%sWarning%s: unable to find \"%s\" in text window.") +
                                    lineInfo,
                                L"<span class='warning' style='font-weight:bold;'>", L"</span>",
                                wxString{ lily_of_the_valley::html_encode_text::simple_encode(
                                    wxString{ contentToFind }
                                        .Truncate(10)
                                        .append(contentToFind.length() > 10 ?
                                                    wxString{ _DT(L"...") } :
                                                    wxString{})
                                        .ToStdWstring()) }));
                            DebugPrint(wxString{});
                            }
                        selWindow->SetFocus();
                        }
                    }
                view->GetSideBar()->CollapseAll();
                view->GetSideBar()->SelectSubItem(parentPos.value(), childPos.value());
                }
            }
        wxGetApp().Yield();
        return 0;
        }

    // HIDDEN interfaces for testing and screenshots
    //-------------------------------------------------------------
    int StandardProject::ShowProperties(lua_State* L)
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
    int StandardProject::CloseProperties([[maybe_unused]] lua_State* L)
        {
        if (m_settingsDlg)
            {
            m_settingsDlg->Destroy();
            m_settingsDlg = nullptr;
            }
        return 0;
        }

    // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
    Luna<StandardProject>::PropertyType StandardProject::properties[] = { { nullptr, nullptr,
                                                                            nullptr } };
    // NOLINTEND(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)

    // NOLINTBEGIN(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
    Luna<StandardProject>::FunctionType StandardProject::methods[] = {
        LUNA_DECLARE_METHOD(StandardProject, Close),
        LUNA_DECLARE_METHOD(StandardProject, DelayReloading),
        LUNA_DECLARE_METHOD(StandardProject, GetTitle),
        LUNA_DECLARE_METHOD(StandardProject, SetWindowSize),
        LUNA_DECLARE_METHOD(StandardProject, GetWindowSize),
        LUNA_DECLARE_METHOD(StandardProject, GetSentenceCount),
        LUNA_DECLARE_METHOD(StandardProject, GetIndependentClauseCount),
        LUNA_DECLARE_METHOD(StandardProject, GetNumeralCount),
        LUNA_DECLARE_METHOD(StandardProject, GetProperNounCount),
        LUNA_DECLARE_METHOD(StandardProject, GetUniqueWordCount),
        LUNA_DECLARE_METHOD(StandardProject, GetWordCount),
        LUNA_DECLARE_METHOD(StandardProject, GetCharacterAndPunctuationCount),
        LUNA_DECLARE_METHOD(StandardProject, GetCharacterCount),
        LUNA_DECLARE_METHOD(StandardProject, GetSyllableCount),
        LUNA_DECLARE_METHOD(StandardProject, GetUnique3SyllablePlusWordCount),
        LUNA_DECLARE_METHOD(StandardProject, Get3SyllablePlusWordCount),
        LUNA_DECLARE_METHOD(StandardProject, GetUnique1SyllableWordCount),
        LUNA_DECLARE_METHOD(StandardProject, Get1SyllableWordCount),
        LUNA_DECLARE_METHOD(StandardProject, Get7CharacterPlusWordCount),
        LUNA_DECLARE_METHOD(StandardProject, GetUnique6CharPlusWordCount),
        LUNA_DECLARE_METHOD(StandardProject, Get6CharacterPlusWordCount),
        LUNA_DECLARE_METHOD(StandardProject, GetUnfamiliarSpacheWordCount),
        LUNA_DECLARE_METHOD(StandardProject, GetUnfamiliarDCWordCount),
        LUNA_DECLARE_METHOD(StandardProject, GetUnfamiliarHJWordCount),
        LUNA_DECLARE_METHOD(StandardProject, SetPhraseExclusionList),
        LUNA_DECLARE_METHOD(StandardProject, GetPhraseExclusionList),
        LUNA_DECLARE_METHOD(StandardProject, SetBlockExclusionTags),
        LUNA_DECLARE_METHOD(StandardProject, GetBlockExclusionTags),
        LUNA_DECLARE_METHOD(StandardProject, GetNumeralSyllabication),
        LUNA_DECLARE_METHOD(StandardProject, SetNumeralSyllabication),
        LUNA_DECLARE_METHOD(StandardProject, IsFogUsingSentenceUnits),
        LUNA_DECLARE_METHOD(StandardProject, FogUseSentenceUnits),
        LUNA_DECLARE_METHOD(StandardProject, IncludeStockerCatholicSupplement),
        LUNA_DECLARE_METHOD(StandardProject, IsIncludingStockerCatholicSupplement),
        LUNA_DECLARE_METHOD(StandardProject, IncludeScoreSummaryReport),
        LUNA_DECLARE_METHOD(StandardProject, IsIncludingScoreSummaryReport),
        LUNA_DECLARE_METHOD(StandardProject, SetLongGradeScaleFormat),
        LUNA_DECLARE_METHOD(StandardProject, IsUsingLongGradeScaleFormat),
        LUNA_DECLARE_METHOD(StandardProject, GetReadingAgeDisplay),
        LUNA_DECLARE_METHOD(StandardProject, SetReadingAgeDisplay),
        LUNA_DECLARE_METHOD(StandardProject, GetGradeScale),
        LUNA_DECLARE_METHOD(StandardProject, SetGradeScale),
        LUNA_DECLARE_METHOD(StandardProject, GetFleschNumeralSyllabizeMethod),
        LUNA_DECLARE_METHOD(StandardProject, SetFleschNumeralSyllabizeMethod),
        LUNA_DECLARE_METHOD(StandardProject, GetFleschKincaidNumeralSyllabizeMethod),
        LUNA_DECLARE_METHOD(StandardProject, SetFleschKincaidNumeralSyllabizeMethod),
        LUNA_DECLARE_METHOD(StandardProject, GetHarrisJacobsonTextExclusionMode),
        LUNA_DECLARE_METHOD(StandardProject, SetHarrisJacobsonTextExclusionMode),
        LUNA_DECLARE_METHOD(StandardProject, GetDaleChallTextExclusionMode),
        LUNA_DECLARE_METHOD(StandardProject, SetDaleChallTextExclusionMode),
        LUNA_DECLARE_METHOD(StandardProject, GetDaleChallProperNounCountingMethod),
        LUNA_DECLARE_METHOD(StandardProject, SetDaleChallProperNounCountingMethod),
        LUNA_DECLARE_METHOD(StandardProject, IncludeExcludedPhraseFirstOccurrence),
        LUNA_DECLARE_METHOD(StandardProject, IsIncludingExcludedPhraseFirstOccurrence),
        LUNA_DECLARE_METHOD(StandardProject, SetAppendedDocumentFilePath),
        LUNA_DECLARE_METHOD(StandardProject, GetAppendedDocumentFilePath),
        LUNA_DECLARE_METHOD(StandardProject, UseRealTimeUpdate),
        LUNA_DECLARE_METHOD(StandardProject, IsRealTimeUpdating),
        LUNA_DECLARE_METHOD(StandardProject, SetDocumentFilePath),
        LUNA_DECLARE_METHOD(StandardProject, GetDocumentFilePath),
        LUNA_DECLARE_METHOD(StandardProject, AggressivelyExclude),
        LUNA_DECLARE_METHOD(StandardProject, SetTextExclusion),
        LUNA_DECLARE_METHOD(StandardProject, GetTextExclusion),
        LUNA_DECLARE_METHOD(StandardProject, SetIncludeIncompleteTolerance),
        LUNA_DECLARE_METHOD(StandardProject, GetIncludeIncompleteTolerance),
        LUNA_DECLARE_METHOD(StandardProject, ExcludeCopyrightNotices),
        LUNA_DECLARE_METHOD(StandardProject, ExcludeTrailingCitations),
        LUNA_DECLARE_METHOD(StandardProject, ExcludeFileAddress),
        LUNA_DECLARE_METHOD(StandardProject, ExcludeNumerals),
        LUNA_DECLARE_METHOD(StandardProject, ExcludeProperNouns),
        LUNA_DECLARE_METHOD(StandardProject, IsExcludingAggressively),
        LUNA_DECLARE_METHOD(StandardProject, IsExcludingCopyrightNotices),
        LUNA_DECLARE_METHOD(StandardProject, IsExcludingTrailingCitations),
        LUNA_DECLARE_METHOD(StandardProject, IsExcludingFileAddresses),
        LUNA_DECLARE_METHOD(StandardProject, IsExcludingNumerals),
        LUNA_DECLARE_METHOD(StandardProject, IsExcludingProperNouns),
        LUNA_DECLARE_METHOD(StandardProject, SetLanguage),
        LUNA_DECLARE_METHOD(StandardProject, GetLanguage),
        LUNA_DECLARE_METHOD(StandardProject, SetReviewer),
        LUNA_DECLARE_METHOD(StandardProject, GetReviewer),
        LUNA_DECLARE_METHOD(StandardProject, SetStatus),
        LUNA_DECLARE_METHOD(StandardProject, GetStatus),
        LUNA_DECLARE_METHOD(StandardProject, SetTextStorageMethod),
        LUNA_DECLARE_METHOD(StandardProject, SetParagraphsParsingMethod),
        LUNA_DECLARE_METHOD(StandardProject, GetParagraphsParsingMethod),
        LUNA_DECLARE_METHOD(StandardProject, GetLongSentenceMethod),
        LUNA_DECLARE_METHOD(StandardProject, SetLongSentenceMethod),
        LUNA_DECLARE_METHOD(StandardProject, GetDifficultSentenceLength),
        LUNA_DECLARE_METHOD(StandardProject, SetDifficultSentenceLength),
        LUNA_DECLARE_METHOD(StandardProject, IgnoreBlankLines),
        LUNA_DECLARE_METHOD(StandardProject, IsIgnoringBlankLines),
        LUNA_DECLARE_METHOD(StandardProject, IgnoreIndenting),
        LUNA_DECLARE_METHOD(StandardProject, IsIgnoringIndenting),
        LUNA_DECLARE_METHOD(StandardProject, SetSentenceStartMustBeUppercased),
        LUNA_DECLARE_METHOD(StandardProject, SentenceStartMustBeUppercased),
        LUNA_DECLARE_METHOD(StandardProject, GetTextStorageMethod),
        LUNA_DECLARE_METHOD(StandardProject, GetTextSource),
        LUNA_DECLARE_METHOD(StandardProject, SetTextSource),
        LUNA_DECLARE_METHOD(StandardProject, SetSpellCheckerOptions),
        LUNA_DECLARE_METHOD(StandardProject, SetSummaryStatsResultsOptions),
        LUNA_DECLARE_METHOD(StandardProject, SetSummaryStatsReportOptions),
        LUNA_DECLARE_METHOD(StandardProject, SetSummaryStatsDolchReportOptions),
        LUNA_DECLARE_METHOD(StandardProject, SetWordsBreakdownResultsOptions),
        LUNA_DECLARE_METHOD(StandardProject, SetSentenceBreakdownResultsOptions),
        LUNA_DECLARE_METHOD(StandardProject, SetGrammarResultsOptions),
        LUNA_DECLARE_METHOD(StandardProject, SetReportFont),
        LUNA_DECLARE_METHOD(StandardProject, SetExcludedTextHighlightColor),
        LUNA_DECLARE_METHOD(StandardProject, SetDifficultTextHighlightColor),
        LUNA_DECLARE_METHOD(StandardProject, SetGrammarIssuesHighlightColor),
        LUNA_DECLARE_METHOD(StandardProject, SetWordyTextHighlightColor),
        LUNA_DECLARE_METHOD(StandardProject, SetTextHighlighting),
        LUNA_DECLARE_METHOD(StandardProject, GetTextHighlighting),
        LUNA_DECLARE_METHOD(StandardProject, SetDolchConjunctionsColor),
        LUNA_DECLARE_METHOD(StandardProject, SetDolchPrepositionsColor),
        LUNA_DECLARE_METHOD(StandardProject, SetDolchPronounsColor),
        LUNA_DECLARE_METHOD(StandardProject, SetDolchAdverbsColor),
        LUNA_DECLARE_METHOD(StandardProject, SetDolchAdjectivesColor),
        LUNA_DECLARE_METHOD(StandardProject, SetDolchVerbsColor),
        LUNA_DECLARE_METHOD(StandardProject, SetDolchNounsColor),
        LUNA_DECLARE_METHOD(StandardProject, HighlightDolchConjunctions),
        LUNA_DECLARE_METHOD(StandardProject, IsHighlightingDolchConjunctions),
        LUNA_DECLARE_METHOD(StandardProject, HighlightDolchPrepositions),
        LUNA_DECLARE_METHOD(StandardProject, IsHighlightingDolchPrepositions),
        LUNA_DECLARE_METHOD(StandardProject, HighlightDolchPronouns),
        LUNA_DECLARE_METHOD(StandardProject, IsHighlightingDolchPronouns),
        LUNA_DECLARE_METHOD(StandardProject, HighlightDolchAdverbs),
        LUNA_DECLARE_METHOD(StandardProject, IsHighlightingDolchAdverbs),
        LUNA_DECLARE_METHOD(StandardProject, HighlightDolchAdjectives),
        LUNA_DECLARE_METHOD(StandardProject, IsHighlightingDolchAdjectives),
        LUNA_DECLARE_METHOD(StandardProject, HighlightDolchVerbs),
        LUNA_DECLARE_METHOD(StandardProject, IsHighlightingDolchVerbs),
        LUNA_DECLARE_METHOD(StandardProject, HighlightDolchNouns),
        LUNA_DECLARE_METHOD(StandardProject, IsHighlightingDolchNouns),
        LUNA_DECLARE_METHOD(StandardProject, SetGraphColorScheme),
        LUNA_DECLARE_METHOD(StandardProject, GetGraphColorScheme),
        LUNA_DECLARE_METHOD(StandardProject, SetGraphBackgroundColor),
        LUNA_DECLARE_METHOD(StandardProject, ApplyGraphBackgroundFade),
        LUNA_DECLARE_METHOD(StandardProject, IsApplyingGraphBackgroundFade),
        LUNA_DECLARE_METHOD(StandardProject, SetGraphCommonImage),
        LUNA_DECLARE_METHOD(StandardProject, GetGraphCommonImage),
        LUNA_DECLARE_METHOD(StandardProject, SetPlotBackgroundImage),
        LUNA_DECLARE_METHOD(StandardProject, GetPlotBackgroundImage),
        LUNA_DECLARE_METHOD(StandardProject, SetPlotBackgroundImageEffect),
        LUNA_DECLARE_METHOD(StandardProject, GetPlotBackgroundImageEffect),
        LUNA_DECLARE_METHOD(StandardProject, SetPlotBackgroundImageFit),
        LUNA_DECLARE_METHOD(StandardProject, GetPlotBackgroundImageFit),
        LUNA_DECLARE_METHOD(StandardProject, SetPlotBackgroundImageOpacity),
        LUNA_DECLARE_METHOD(StandardProject, GetPlotBackgroundImageOpacity),
        LUNA_DECLARE_METHOD(StandardProject, SetWatermark),
        LUNA_DECLARE_METHOD(StandardProject, GetWatermark),
        LUNA_DECLARE_METHOD(StandardProject, SetGraphLogoImage),
        LUNA_DECLARE_METHOD(StandardProject, GetGraphLogoImage),
        LUNA_DECLARE_METHOD(StandardProject, SetPlotBackgroundColor),
        LUNA_DECLARE_METHOD(StandardProject, SetGraphInvalidRegionColor),
        LUNA_DECLARE_METHOD(StandardProject, SetRaygorStyle),
        LUNA_DECLARE_METHOD(StandardProject, GetRaygorStyle),
        LUNA_DECLARE_METHOD(StandardProject, ConnectFleschPoints),
        LUNA_DECLARE_METHOD(StandardProject, IsConnectingFleschPoints),
        LUNA_DECLARE_METHOD(StandardProject, IncludeFleschRulerDocGroups),
        LUNA_DECLARE_METHOD(StandardProject, IsIncludingFleschRulerDocGroups),
        LUNA_DECLARE_METHOD(StandardProject, UseEnglishLabelsForGermanLix),
        LUNA_DECLARE_METHOD(StandardProject, IsUsingEnglishLabelsForGermanLix),
        LUNA_DECLARE_METHOD(StandardProject, SetStippleShapeColor),
        LUNA_DECLARE_METHOD(StandardProject, ShowcaseKeyItems),
        LUNA_DECLARE_METHOD(StandardProject, IsShowcasingKeyItems),
        LUNA_DECLARE_METHOD(StandardProject, SetPlotBackgroundColorOpacity),
        LUNA_DECLARE_METHOD(StandardProject, GetPlotBackgroundColorOpacity),
        LUNA_DECLARE_METHOD(StandardProject, SetStippleImage),
        LUNA_DECLARE_METHOD(StandardProject, GetStippleImage),
        LUNA_DECLARE_METHOD(StandardProject, SetStippleShape),
        LUNA_DECLARE_METHOD(StandardProject, GetStippleShape),
        LUNA_DECLARE_METHOD(StandardProject, SetXAxisFont),
        LUNA_DECLARE_METHOD(StandardProject, SetYAxisFont),
        LUNA_DECLARE_METHOD(StandardProject, SetGraphTopTitleFont),
        LUNA_DECLARE_METHOD(StandardProject, SetGraphBottomTitleFont),
        LUNA_DECLARE_METHOD(StandardProject, SetGraphLeftTitleFont),
        LUNA_DECLARE_METHOD(StandardProject, SetGraphRightTitleFont),
        LUNA_DECLARE_METHOD(StandardProject, DisplayBarChartLabels),
        LUNA_DECLARE_METHOD(StandardProject, DisplayGraphDropShadows),
        LUNA_DECLARE_METHOD(StandardProject, IsDisplayingGraphDropShadows),
        LUNA_DECLARE_METHOD(StandardProject, SetBarChartBarColor),
        LUNA_DECLARE_METHOD(StandardProject, SetBarChartBarOpacity),
        LUNA_DECLARE_METHOD(StandardProject, SetBarChartBarEffect),
        LUNA_DECLARE_METHOD(StandardProject, GetBarChartBarEffect),
        LUNA_DECLARE_METHOD(StandardProject, GetBarChartBarOpacity),
        LUNA_DECLARE_METHOD(StandardProject, GetBarChartOrientation),
        LUNA_DECLARE_METHOD(StandardProject, IsDisplayingBarChartLabels),
        LUNA_DECLARE_METHOD(StandardProject, SetHistogramBarColor),
        LUNA_DECLARE_METHOD(StandardProject, SetHistogramBarEffect),
        LUNA_DECLARE_METHOD(StandardProject, GetHistogramBarEffect),
        LUNA_DECLARE_METHOD(StandardProject, SetHistogramBarOpacity),
        LUNA_DECLARE_METHOD(StandardProject, GetHistogramBarOpacity),
        LUNA_DECLARE_METHOD(StandardProject, SetHistogramBinning),
        LUNA_DECLARE_METHOD(StandardProject, GetHistogramBinning),
        LUNA_DECLARE_METHOD(StandardProject, SetHistogramRounding),
        LUNA_DECLARE_METHOD(StandardProject, GetHistogramRounding),
        LUNA_DECLARE_METHOD(StandardProject, SetHistogramIntervalDisplay),
        LUNA_DECLARE_METHOD(StandardProject, GetHistogramIntervalDisplay),
        LUNA_DECLARE_METHOD(StandardProject, SetHistogramBinLabelDisplay),
        LUNA_DECLARE_METHOD(StandardProject, GetHistogramBinLabelDisplay),
        LUNA_DECLARE_METHOD(StandardProject, SetBarChartOrientation),
        LUNA_DECLARE_METHOD(StandardProject, SetBoxPlotColor),
        LUNA_DECLARE_METHOD(StandardProject, SetBoxPlotEffect),
        LUNA_DECLARE_METHOD(StandardProject, GetBoxPlotEffect),
        LUNA_DECLARE_METHOD(StandardProject, SetBoxPlotOpacity),
        LUNA_DECLARE_METHOD(StandardProject, GetBoxPlotOpacity),
        LUNA_DECLARE_METHOD(StandardProject, DisplayBoxPlotLabels),
        LUNA_DECLARE_METHOD(StandardProject, IsDisplayingBoxPlotLabels),
        LUNA_DECLARE_METHOD(StandardProject, DisplayAllBoxPlotPoints),
        LUNA_DECLARE_METHOD(StandardProject, IsDisplayingAllBoxPlotPoints),
        LUNA_DECLARE_METHOD(StandardProject, ConnectBoxPlotMiddlePoints),
        LUNA_DECLARE_METHOD(StandardProject, IsConnectingBoxPlotMiddlePoints),
        LUNA_DECLARE_METHOD(StandardProject, ExcludeFileAddress),
        LUNA_DECLARE_METHOD(StandardProject, AddTest),
        LUNA_DECLARE_METHOD(StandardProject, Reload),
        LUNA_DECLARE_METHOD(StandardProject, ExportAll),
        LUNA_DECLARE_METHOD(StandardProject, ExportScores),
        LUNA_DECLARE_METHOD(StandardProject, ExportGraph),
        LUNA_DECLARE_METHOD(StandardProject, ExportHighlightedWords),
        LUNA_DECLARE_METHOD(StandardProject, ExportReport),
        LUNA_DECLARE_METHOD(StandardProject, ExportList),
        LUNA_DECLARE_METHOD(StandardProject, ExportFilteredText),
        LUNA_DECLARE_METHOD(StandardProject, ShowProperties),
        LUNA_DECLARE_METHOD(StandardProject, CloseProperties),
        LUNA_DECLARE_METHOD(StandardProject, SelectHighlightedWordReport),
        LUNA_DECLARE_METHOD(StandardProject, SelectWindow),
        LUNA_DECLARE_METHOD(StandardProject, ShowSidebar),
        LUNA_DECLARE_METHOD(StandardProject, SelectReadabilityTest),
        LUNA_DECLARE_METHOD(StandardProject, SortList),
        LUNA_DECLARE_METHOD(StandardProject, SortGraph),
        LUNA_DECLARE_METHOD(StandardProject, SelectRowsInWordsBreakdownList),
        LUNA_DECLARE_METHOD(StandardProject, ScrollTextWindow),
        LUNA_DECLARE_METHOD(StandardProject, SelectTextGrammarWindow),
        { nullptr, nullptr }
    };
    // NOLINTEND(cppcoreguidelines-avoid-c-arrays,hicpp-avoid-c-arrays,modernize-avoid-c-arrays)
    } // namespace LuaScripting

// NOLINTEND(readability-implicit-bool-conversion)
// NOLINTEND(readability-identifier-length)
