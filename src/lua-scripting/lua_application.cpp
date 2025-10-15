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

#include "lua_application.h"
#include "../Wisteria-Dataviz/src/CRCpp/inc/CRC.h"
#include "../Wisteria-Dataviz/src/import/html_extract_text.h"
#include "../Wisteria-Dataviz/src/util/frequencymap.h"
#include "../Wisteria-Dataviz/src/util/memorymappedfile.h"
#include "../app/readability_app.h"
#include "../indexing/characters.h"
#include "../indexing/phrase.h"
#include "../indexing/word_list.h"
#include "../projects/base_project.h"
#include "../projects/batch_project_doc.h"
#include "../projects/standard_project_doc.h"
#include <wx/dir.h>

wxDECLARE_APP(ReadabilityApp);

// NOLINTBEGIN(readability-identifier-length)
// NOLINTBEGIN(readability-implicit-bool-conversion)

namespace LuaScripting
    {
    //-------------------------------------------------------------
    void VerifyLink(const wchar_t* link, const size_t length, const bool isImage,
                    const wxString& currentFile, std::multimap<wxString, wxString>& badLinks,
                    std::multimap<wxString, wxString>& badImageSizes,
                    const bool IncludeExternalLinks)
        {
        lily_of_the_valley::html_extract_text extract;
        // see if it's a weblink before anything else
        const wxString webPath(link, length);
        FilePathResolver resolve(webPath, false);
        if (resolve.IsHTTPFile() || resolve.IsHTTPSFile())
            {
            if (IncludeExternalLinks)
                {
                // now try to connect to it
                resolve.ResolvePath(webPath, true);
                // it's a web link that we couldn't connect to
                if (resolve.IsInvalidFile())
                    {
                    badLinks.insert(std::make_pair(currentFile, webPath));
                    return;
                    }
                }
            else
                {
                return;
                }
            }
        // encoded links will hex encode non-alphanumeric values in them,
        // so convert that to match the real name of the local file
        const wchar_t* filteredFileName = extract(link, length, true, false);
        if (!filteredFileName)
            {
            return;
            }
        wxString path(filteredFileName);
        if (path == L"./" || path == L"." || path == L"../")
            {
            return;
            }
        for (;;)
            {
            const size_t percentIndex = path.find(L'%');
            if (percentIndex == wxString::npos ||
                static_cast<size_t>(percentIndex) > path.length() - 3)
                {
                break;
                }
            wchar_t* dummy{ nullptr };
            const wxString extractedHexCode{ path.substr(percentIndex + 1, 2) };
            const auto value =
                static_cast<wchar_t>(std::wcstol(extractedHexCode.wc_str(), &dummy, 16));
            path.replace(percentIndex, 3, wxString(1, value));
            }
        // strip off bookmark (if there is one)
        const size_t bookMarkIndex = path.find(L'#', true);
        if (bookMarkIndex != wxString::npos)
            {
            path.erase(bookMarkIndex);
            if (path.empty())
                {
                return;
                }
            }
        // if link to a local CHM file then try to see if the
        // decompiled file is in the local file system
        if (path.find(_DT(L".chm::")) != wxString::npos ||
            path.find(_DT(L".CHM::")) != wxString::npos)
            {
            path.Replace(_DT(L".chm::"), wxString{});
            path.Replace(_DT(L".CHM::"), wxString{});
            path.Prepend(wxFileName::GetCwd() + L'\\');
            if (!wxFileName::FileExists(path))
                {
                badLinks.insert(std::make_pair(currentFile, path));
                return;
                }
            }
        // ignore mail links
        if (path.StartsWith(L"mailto"))
            {
            return;
            }
        // otherwise, it is a regular link
        wxFileName fn(path);
        if (fn.IsRelative())
            {
            [[maybe_unused]]
            auto absRes = fn.MakeAbsolute(wxFileName(currentFile).GetPath());
            }
        // if an image, then check its dimensions
        if (isImage)
            {
            const std::wstring widthStr =
                lily_of_the_valley::html_extract_text::read_attribute_as_string(link, _DT(L"width"),
                                                                                true, false);
            const std::wstring heightStr =
                lily_of_the_valley::html_extract_text::read_attribute_as_string(
                    link, _DT(L"height"), true, false);
            if (!widthStr.empty() && !heightStr.empty())
                {
                const long width = std::wcstol(widthStr.c_str(), nullptr, 10);
                const long height = std::wcstol(heightStr.c_str(), nullptr, 10);
                if (fn.FileExists())
                    {
                    const wxLogNull logNo;
                    wxImage img;
                    img.LoadFile(fn.GetFullPath());
                    if (!img.IsOk())
                        {
                        badImageSizes.insert(std::make_pair(
                            currentFile, wxString::Format(_(L"%s (image is possibly corrupt)"),
                                                          fn.GetFullPath())));
                        }
                    else if (!is_within<long>(img.GetHeight(), height - 5, height + 5) ||
                             !is_within<long>(img.GetWidth(), width - 5, width + 5))
                        {
                        badImageSizes.insert(
                            std::make_pair(currentFile,
                                           wxString::Format( // TRANSLATORS: Placeholders are a file
                                                             // name and an image's dimensions
                                               _(L"%s (%ld x %ld vs. %d x %d)"), fn.GetFullPath(),
                                               width, height, img.GetWidth(), img.GetHeight())));
                        }
                    }
                }
            }
        if (!fn.FileExists())
            {
            badLinks.insert(std::make_pair(currentFile, fn.GetFullPath()));
            }
        }

    //-------------------------------------------------------------
    int GetProgramPath(lua_State* L)
        {
        lua_pushstring(L, wxFileName(wxStandardPaths::Get().GetExecutablePath())
                              .GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME)
                              .utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int GetExamplesFolder(lua_State* L)
        {
        lua_pushstring(L, wxString{ wxGetApp().FindResourceDirectory(_DT(L"examples")) +
                                    wxFileName::GetPathSeparator() }
                              .utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int GetDownloadFolder(lua_State* L)
        {
        lua_pushstring(L, wxString{ wxGetApp().GetWebHarvester().GetDownloadDirectory() +
                                    wxFileName::GetPathSeparator() }
                              .utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int SetDownloadFolder(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetWebHarvester().SetDownloadDirectory(
            wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        return 0;
        }

    //-------------------------------------------------------------
    int UseWebsitesFolderStructure(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetWebHarvester().KeepWebPathWhenDownloading(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsUsingWebsitesFolderStructure(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetWebHarvester().IsKeepingWebPathWhenDownloading());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int SearchForBrokenLinks(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetWebHarvester().SearchForBrokenLinks(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsSearchingForBrokenLinks(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetWebHarvester().IsSearchingForBrokenLinks());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int ReplaceExistingFiles(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetWebHarvester().ReplaceExistingFiles(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsReplacingExistingFiles(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetWebHarvester().IsReplacingExistingFiles());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int SetMinimumDownloadFileSizeInKilobytes(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetWebHarvester().SetMinimumDownloadFileSizeInKilobytes(luaL_checkinteger(L, 1));
        return 0;
        }

    //-------------------------------------------------------------
    int GetMinimumDownloadFileSizeInKilobytes(lua_State* L)
        {
        lua_pushinteger(
            L, wxGetApp().GetWebHarvester().GetMinimumDownloadFileSizeInKilobytes().value_or(5));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int SetDepthLevel(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetWebHarvester().SetDepthLevel(luaL_checkinteger(L, 1));
        return 0;
        }

    //-------------------------------------------------------------
    int GetDepthLevel(lua_State* L)
        {
        lua_pushinteger(L, wxGetApp().GetWebHarvester().GetDepthLevel());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int AddAllowableDomain(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetWebHarvester().AddAllowableDomain(
            wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        return 0;
        }

    //-------------------------------------------------------------
    int SetDomainRestriction(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetWebHarvester().SetDomainRestriction(
            static_cast<WebHarvester::DomainRestriction>(luaL_checkinteger(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int GetDomainRestriction(lua_State* L)
        {
        lua_pushinteger(L, static_cast<int>(wxGetApp().GetWebHarvester().GetDomainRestriction()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int SetWebHarvesterFileFilter(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetWebHarvester().ClearAllowableFileTypes();

        wxStringTokenizer tkz(
            ExtractExtensionsFromFileFilter(wxString{ luaL_checkstring(L, 1), wxConvUTF8 }),
            L"*.;");
        while (tkz.HasMoreTokens())
            {
            const wxString nextFileExt = tkz.GetNextToken();
            if (!nextFileExt.empty())
                {
                wxGetApp().GetWebHarvester().AddAllowableFileType(nextFileExt);
                if (nextFileExt.CmpNoCase(L"html") == 0 || nextFileExt.CmpNoCase(L"htm") == 0)
                    {
                    wxGetApp().GetWebHarvester().HarvestAllHtmlFiles(true);
                    }
                }
            }
        return 0;
        }

    //-------------------------------------------------------------
    int SetWebHarvesterWebsite(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        const FilePathResolver resolver(wxString{ luaL_checkstring(L, 1), wxConvUTF8 }, true);
        wxGetApp().GetWebHarvester().SetUrl(resolver.GetResolvedPath());
        return 0;
        }

    //-------------------------------------------------------------
    int WebHarvest(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetWebHarvester().CrawlLinks());
        return 1;
        }

    //-------------------------------------------------------------
    int GetLuaConstantsPath(lua_State* L)
        {
        lua_pushstring(L, wxGetApp().FindResourceFile(L"rs-constants.lua").utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int GetActiveBatchProject(lua_State* L)
        {
        wxDocument* currentDoc =
            wxGetApp().GetMainFrame()->GetDocumentManager()->GetCurrentDocument();
        if (currentDoc && currentDoc->IsKindOf(wxCLASSINFO(BatchProjectDoc)))
            {
            auto* batchProject = new BatchProject(L);
            batchProject->SetProject(dynamic_cast<BatchProjectDoc*>(currentDoc));
            Luna<LuaScripting::BatchProject>::push(L, batchProject);
            return 1;
            }

        const wxList docs = wxGetApp().GetDocManager()->GetDocuments();
        for (size_t i = 0; i < docs.GetCount(); ++i)
            {
            if (docs.Item(i)->GetData()->IsKindOf(wxCLASSINFO(BatchProjectDoc)))
                {
                auto* batchProject = new BatchProject(L);
                batchProject->SetProject(dynamic_cast<BatchProjectDoc*>(docs.Item(i)->GetData()));
                Luna<LuaScripting::BatchProject>::push(L, batchProject);
                return 1;
                }
            }
        wxLogError(L"No active batch project found.");
        return 0;
        }

    int GetActiveStandardProject(lua_State* L)
        {
        wxDocument* currentDoc =
            wxGetApp().GetMainFrame()->GetDocumentManager()->GetCurrentDocument();
        if (currentDoc && currentDoc->IsKindOf(wxCLASSINFO(ProjectDoc)))
            {
            auto* standardProject = new StandardProject(L);
            standardProject->SetProject(dynamic_cast<ProjectDoc*>(currentDoc));
            Luna<LuaScripting::StandardProject>::push(L, standardProject);
            return 1;
            }

        const wxList docs = wxGetApp().GetDocManager()->GetDocuments();
        for (size_t i = 0; i < docs.GetCount(); ++i)
            {
            if (docs.Item(i)->GetData()->IsKindOf(wxCLASSINFO(ProjectDoc)))
                {
                auto* standardProject = new StandardProject(L);
                standardProject->SetProject(dynamic_cast<ProjectDoc*>(docs.Item(i)->GetData()));
                Luna<LuaScripting::StandardProject>::push(L, standardProject);
                return 1;
                }
            }
        wxLogError(L"No active standard project found.");
        return 0;
        }

    //-------------------------------------------------------------
    int GetTestId(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        const wxString testName(luaL_checkstring(L, 1), wxConvUTF8);
        const auto testPos = BaseProject::GetDefaultReadabilityTestsTemplate().find_test(testName);

        if (testPos.second)
            {
            lua_pushnumber(L, testPos.first->get_interface_id());
            return 1;
            }

        auto testIter = std::find(BaseProject::m_custom_word_tests.begin(),
                                  BaseProject::m_custom_word_tests.end(), testName);
        if (testIter != BaseProject::m_custom_word_tests.end())
            {
            lua_pushnumber(L, testIter->get_interface_id());
            }
        else
            {
            lua_pushnumber(L, wxNOT_FOUND);
            }
        return 1;
        }

    //-------------------------------------------------------------
    int MsgBox(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        if (lua_isboolean(L, 1))
            {
            wxMessageBox(lua_toboolean(L, 1) ? _(L"true") : _(L"false"),
                         (lua_gettop(L) > 1) ? wxString{ luaL_checkstring(L, 2), wxConvUTF8 } :
                                               wxGetApp().GetAppName(),
                         wxOK | wxICON_INFORMATION);
            }
        else if (lua_isstring(L, 1))
            {
            wxMessageBox(wxString{ luaL_checkstring(L, 1), wxConvUTF8 },
                         (lua_gettop(L) > 1) ? wxString{ luaL_checkstring(L, 2), wxConvUTF8 } :
                                               wxGetApp().GetAppName(),
                         wxOK | wxICON_INFORMATION);
            }
        return 0;
        }

    //-------------------------------------------------------------
    int DownloadFile(lua_State* L)
        {
        if (!VerifyParameterCount(L, 2, __func__))
            {
            return 0;
            }
        wxString urlPath(luaL_checkstring(L, 1), wxConvUTF8);
        const wxString downloadFolder(luaL_checklstring(L, 2, nullptr), wxConvUTF8);

        wxGetApp().GetWebHarvester().SetUrl(urlPath);
        wxGetApp().GetWebHarvester().SetDownloadDirectory(downloadFolder);
        wxGetApp().GetWebHarvester().KeepWebPathWhenDownloading(false);

        lua_pushstring(L, wxGetApp().GetWebHarvester().DownloadFile(urlPath).utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int FindFiles(lua_State* L)
        {
        // should be passed a folder to search and the file filter to use
        // (recursive flag is optional)
        if (!VerifyParameterCount(L, 2, __func__))
            {
            return 0;
            }
        lua_newtable(L);

        const int flags = ((lua_gettop(L) >= 3) ? int_to_bool(lua_toboolean(L, 3)) : true) ?
                              (wxDIR_FILES | wxDIR_DIRS) :
                              wxDIR_FILES;
        wxArrayString files;
        const size_t fileCount =
            wxDir::GetAllFiles(wxString{ luaL_checkstring(L, 1), wxConvUTF8 }, &files,
                               wxString{ luaL_checkstring(L, 2), wxConvUTF8 }, flags);

        for (size_t i = 1; i <= fileCount; ++i)
            {
            lua_pushnumber(L, i);
            lua_pushstring(L, files[i - 1].utf8_str());
            lua_settable(L, -3);
            }

        return 1;
        }

    //-------------------------------------------------------------
    int MergeImages(lua_State* L)
        {
        if (!VerifyParameterCount(L, 4, __func__))
            {
            return 0;
            }

        std::vector<wxImage> images;
        for (int i = 1; i <= lua_gettop(L) - 2; ++i)
            {
            const wxString inputFile(luaL_checkstring(L, i), wxConvUTF8);
            if (wxFile::Exists(inputFile))
                {
                images.push_back(Wisteria::GraphItems::Image::LoadFile(inputFile));
                }
            else
                {
                wxMessageBox(wxString::Format(_(L"%s: file not found."), inputFile),
                             _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
                lua_pushboolean(L, false);
                return 1;
                }
            }

        // create the folder to the filepath, if necessary
        const wxString outPath(luaL_checkstring(L, lua_gettop(L) - 1), wxConvUTF8);
        wxFileName::Mkdir(wxFileName(outPath).GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

        const wxImage img =
            (static_cast<Wisteria::Orientation>(luaL_checkinteger(L, lua_gettop(L))) ==
             Wisteria::Orientation::Vertical) ?
                Wisteria::GraphItems::Image::StitchVertically(images) :
                Wisteria::GraphItems::Image::StitchHorizontally(images);

        lua_pushboolean(L, img.SaveFile(outPath));

        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int ApplyImageEffect(lua_State* L)
        {
        if (!VerifyParameterCount(L, 3, __func__))
            {
            return 0;
            }
        const wxString inPath(luaL_checkstring(L, 1), wxConvUTF8);
        if (!wxFile::Exists(inPath))
            {
            wxMessageBox(wxString::Format(_(L"%s: invalid image file path."), inPath),
                         _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
            lua_pushboolean(L, false);
            return 1;
            }

        // create the folder to the filepath, if necessary
        const wxString path(luaL_checkstring(L, 2), wxConvUTF8);
        wxFileName::Mkdir(wxFileName(path).GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

        const wxImage img = Wisteria::GraphItems::Image::ApplyEffect(
            static_cast<Wisteria::ImageEffect>(luaL_checkinteger(L, 3)),
            Wisteria::GraphItems::Image::LoadFile(inPath));

        lua_pushboolean(L, img.SaveFile(wxString{ luaL_checkstring(L, 2), wxConvUTF8 }));
        return 1;
        }

    //-------------------------------------------------------------
    int GetImageInfo(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        const wxString path(luaL_checkstring(L, 1), wxConvUTF8);
        if (!wxFile::Exists(path))
            {
            wxMessageBox(wxString::Format(_(L"%s: invalid image file path."), path),
                         _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
            lua_pushboolean(L, false);
            return 1;
            }
        const wxImage img = Wisteria::GraphItems::Image::LoadFile(path);
        if (!img.IsOk())
            {
            wxMessageBox(wxString::Format(_(L"%s: unable to load image."), path),
                         _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
            lua_pushboolean(L, false);
            return 1;
            }
        lua_createtable(L, 0, 3);

        lua_pushinteger(L, img.GetSize().GetWidth());
        lua_setfield(L, -2, _DT("Width"));

        lua_pushinteger(L, img.GetSize().GetHeight());
        lua_setfield(L, -2, _DT("Height"));

        lua_pushstring(L, wxFileName{ path }.GetName().utf8_str());
        lua_setfield(L, -2, _DT("Name"));

        return 1;
        }

    //-------------------------------------------------------------
    int GetFileCheckSum(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        const wxString filePath(luaL_checkstring(L, 1), wxConvUTF8);

        std::uint32_t crc{ 0 };
        try
            {
            MemoryMappedFile file(filePath, true, true);
            crc = CRC::Calculate(file.GetStream(), file.GetMapSize(), CRC::CRC_32());
            }
        catch (...)
            {
            lua_pushnumber(L, 0);
            return 1;
            }

        lua_pushnumber(L, crc);
        return 1;
        }

    //-------------------------------------------------------------
    int ExportLogReport(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__) || wxGetApp().GetLogFile() == nullptr)
            {
            return 0;
            }
        const wxString path(luaL_checkstring(L, 1), wxConvUTF8);

        // create the folder to the filepath, if necessary
        wxFileName::Mkdir(wxFileName(path).GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

        wxFileName(path).SetPermissions(wxS_DEFAULT);
        wxFile outputFile(path, wxFile::write);
        lua_pushboolean(L, outputFile.Write(wxGetApp().GetLogFile()->Read(), wxConvUTF8));

        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int LogMessage(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        // avoid printf injection
        wxLogMessage(L"%s", wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        return 0;
        }

    //-------------------------------------------------------------
    int SplashScreen(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        assert(wxGetApp().GetSplashscreenPaths().GetCount());
        const auto index = std::clamp<size_t>(luaL_checkinteger(L, 1) - 1 /*make it zero-indexed*/,
                                              0, wxGetApp().GetSplashscreenPaths().GetCount() - 1);
        wxBitmap bitmap = wxGetApp().GetScaledImage(
            wxGetApp().GetSplashscreenPaths()[index], wxBITMAP_TYPE_PNG,
            wxSize(wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_X) * .5,
                   wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_Y) * .5));
        if (bitmap.IsOk())
            {
            bitmap = ReadabilityApp::CreateSplashscreen(bitmap, wxGetApp().GetAppName(),
                                                        wxGetApp().GetAppSubName(),
                                                        wxGetApp().GetVendorName(), true);

            [[maybe_unused]]
            auto* splash =
                new wxSplashScreen(bitmap, wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT, 60000,
                                   nullptr, -1, wxDefaultPosition, wxDefaultSize,
                                   wxFRAME_NO_TASKBAR | wxSTAY_ON_TOP | wxSPLASH_NO_TIMEOUT);
            }
        lua_pushboolean(L, true);
        return 1;
        }

    //-------------------------------------------------------------
    int QAVerify(lua_State* L)
        {
        lua_pushboolean(L, ReadabilityApp::VerifyWordLists());
        return 1;
        }

    //-------------------------------------------------------------
    int CheckHtmlLinks(lua_State* L)
        {
        if (!VerifyParameterCount(L, 2, __func__))
            {
            return 0;
            }
        const wxString path(luaL_checkstring(L, 1), wxConvUTF8);

        if (!wxFileName::DirExists(path))
            {
            wxMessageBox(wxString::Format(_(L"%s: invalid folder path."), path), _(L"Script Error"),
                         wxOK | wxICON_EXCLAMATION);
            lua_pushboolean(L, false);
            return 1;
            }

        std::multimap<wxString, wxString> badLinks;
        std::multimap<wxString, wxString> badImageSizes;

        // get the physical files in the system
        wxArrayString files;
        wxDir::GetAllFiles(path, &files, _DT(L"*.htm*"));

        wxString fileContent;
        const bool includeExternalLinks = int_to_bool(lua_toboolean(L, 2));
        for (size_t i = 0; i < files.Count(); ++i)
            {
            Wisteria::TextStream::ReadFile(files[i], fileContent);

            std::set<wxString> bookmarksInCurrentPage;
            std::pair<const wchar_t*, std::wstring> foundBookMark;
            const wchar_t* const htmlEnd = foundBookMark.first + fileContent.length();
            while (foundBookMark.first)
                {
                foundBookMark = lily_of_the_valley::html_extract_text::find_bookmark(
                    foundBookMark.first, htmlEnd);
                if (foundBookMark.first)
                    {
                    bookmarksInCurrentPage.insert(foundBookMark.second);
                    foundBookMark.first += foundBookMark.second.length();
                    }
                else
                    {
                    break;
                    }
                }
            // read any elements' IDs that may be used as bookmarks
            std::wstring_view htmlContent{ fileContent.wc_str(), fileContent.length() };
            while (!htmlContent.empty())
                {
                auto nextBookmark = lily_of_the_valley::html_extract_text::find_id(htmlContent);
                if (!nextBookmark.empty())
                    {
                    bookmarksInCurrentPage.insert(nextBookmark);
                    }
                }

            html_utilities::html_hyperlink_parse hparse(fileContent.c_str(), fileContent.length());
            const wchar_t* currentLink{ nullptr };
            while ((currentLink = hparse()) != nullptr)
                {
                // review bookmarks in the current page
                if (hparse.get_current_hyperlink_length() > 2 && *currentLink == L'#')
                    {
                    const wxString currentBookmark(currentLink + 1,
                                                   hparse.get_current_hyperlink_length() - 1);
                    if (!bookmarksInCurrentPage.contains(currentBookmark))
                        {
                        badLinks.emplace(
                            wxFileName(files[i]).GetFullName(),
                            wxString(currentLink, hparse.get_current_hyperlink_length()));
                        }
                    }
                else
                    {
                    VerifyLink(currentLink, hparse.get_current_hyperlink_length(),
                               hparse.is_current_link_an_image(), files[i], badLinks, badImageSizes,
                               includeExternalLinks);
                    }
                }
            }

        // write out the warnings of bad links
        for (const auto& badLink : badLinks)
            {
            wxGetApp().GetMainFrameEx()->GetLuaEditor()->DebugOutput(
                wxString::Format(_DT(L"Broken link in '<span style='font-weight:bold;'>%s</span>': "
                                     "<span style='color:#FF7386; font-weight:bold;'>%s</span>",
                                     DTExplanation::DebugMessage),
                                 badLink.first, badLink.second));
            wxLogError(L"Broken link in '%s': %s", badLink.first, badLink.second);
            }
        if (badLinks.empty())
            {
            wxLogMessage(L"No bad links found in help folder.");
            }
        // write out the warnings of images whose size don't
        // match their size specified in a topic
        for (const auto& badImage : badImageSizes)
            {
            wxGetApp().GetMainFrameEx()->GetLuaEditor()->DebugOutput(wxString::Format(
                _DT(L"Bad image size in '<span style='font-weight:bold;'>%s</span>%s</span>': "
                    "<span style='color:#FF7386; font-weight:bold;'>%s</span>"),
                badImage.first, badImage.second));
            wxLogError(L"Bad image size in '%s': %s", badImage.first, badImage.second);
            }
        if (badImageSizes.empty())
            {
            wxLogMessage(L"No image issues found in help folder.");
            }

        lua_pushboolean(L, true);
        return 1;
        }

    // Closes the application
    //-------------------------------------------------------------
    int Close([[maybe_unused]] lua_State* L)
        {
        if (wxGetApp().GetMainFrame()->GetDocumentManager()->CloseDocuments())
            {
            wxGetApp().GetTopWindow()->Close();
            }
        return 0;
        }

    //-------------------------------------------------------------
    int GetUserFolder(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        lua_pushstring(L,
                       wxString{ wxStandardPaths::Get().GetUserDir(
                                     static_cast<wxStandardPaths::Dir>(luaL_checkinteger(L, 1))) +
                                 wxFileName::GetPathSeparator() }
                           .utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int GetAbsoluteFilePath(lua_State* L)
        {
        if (!VerifyParameterCount(L, 2, __func__))
            {
            return 0;
            }

        // set to relative path
        wxFileName fn(wxString{ luaL_checkstring(L, 2), wxConvUTF8 });
        if (fn.MakeAbsolute(wxString{ luaL_checkstring(L, 1), wxConvUTF8 }))
            {
            lua_pushstring(L, fn.GetFullPath().utf8_str());
            }
        else
            {
            DebugPrint(wxString::Format( // TRANSLATORS: %s are formatting tags and
                                         // should stay wrapped around "Warning"
                _(L"%sWarning%s: unable to make %s path absolute."),
                L"<span style='color:#00A2E8; font-weight:bold;'>", L"</span>",
                wxString{ luaL_checkstring(L, 2), wxConvUTF8 }));
            lua_pushstring(L, wxString{ luaL_checkstring(L, 2), wxConvUTF8 });
            }

        return 1;
        }

    //-------------------------------------------------------------
    int WriteToFile(lua_State* L)
        {
        if (!VerifyParameterCount(L, 2, __func__))
            {
            return 0;
            }
        const wxString path(luaL_checkstring(L, 1), wxConvUTF8);
        const wxString outputStr(luaL_checkstring(L, 2), wxConvUTF8);

        // create the folder to the filepath, if necessary
        wxFileName::Mkdir(wxFileName(path).GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

        wxFileName(path).SetPermissions(wxS_DEFAULT);
        wxFile outputFile(path, wxFile::write);
        lua_pushboolean(L, outputFile.Write(outputStr, wxConvUTF8));
        return 1;
        }

    //-------------------------------------------------------------
    int LogError(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxLogError(L"%s", wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        return 0;
        }

    //-------------------------------------------------------------
    int RemoveAllCustomTests([[maybe_unused]] lua_State* L)
        {
        ProjectDoc::RemoveAllGlobalCustomReadabilityTests();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int RemoveAllCustomTestBundles([[maybe_unused]] lua_State* L)
        {
        wxGetApp().RemoveAllCustomTestBundles();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int CrossReferencePhraseLists(lua_State* L)
        {
        if (!VerifyParameterCount(L, 3, __func__))
            {
            return 0;
            }
        wxString path(luaL_checkstring(L, 1), wxConvUTF8);
        if (!wxFile::Exists(path))
            {
            wxMessageBox(wxString::Format(_(L"%s: invalid file path."), wxString(__func__)),
                         _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
            lua_pushboolean(L, false);
            return 1;
            }
        grammar::phrase_collection phraseList;
        wxString inputFileBuffer;
        if (Wisteria::TextStream::ReadFile(path, inputFileBuffer))
            {
            phraseList.load_phrases(inputFileBuffer, true, false);
            }
        phraseList.remove_duplicates();

        // load the word list being cross-referenced against
        path = wxString{ luaL_checkstring(L, 2), wxConvUTF8 };
        if (!wxFile::Exists(path))
            {
            wxMessageBox(wxString::Format(_(L"%s: invalid file path."), wxString(__func__)),
                         _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
            lua_pushboolean(L, false);
            return 1;
            }
        word_list wordList;
        if (Wisteria::TextStream::ReadFile(path, inputFileBuffer))
            {
            wordList.load_words(inputFileBuffer, true, false);
            }

        wxString outputStr;
        wxStringTokenizer tokenizer;
        for (const auto& phrase : phraseList.get_phrases())
            {
            tokenizer.SetString(phrase.second.c_str(), L",", wxTOKEN_STRTOK);
            while (tokenizer.HasMoreTokens())
                {
                wxString replacementStr = tokenizer.GetNextToken();
                replacementStr.Trim(true);
                replacementStr.Trim(false);
                if (replacementStr.find(L' ') == wxString::npos)
                    {
                    if (wordList.contains({ replacementStr.wc_str(), replacementStr.length() }))
                        {
                        outputStr += wxString::Format(
                            L"%s\t%s\r\n", phrase.first.to_string().c_str(), replacementStr);
                        break;
                        }
                    }
                }
            }

        // create the folder to the filepath, if necessary
        const wxString outputPath(luaL_checkstring(L, 3), wxConvUTF8);
        wxFileName::Mkdir(wxFileName(outputPath).GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

        wxFileName(outputPath).SetPermissions(wxS_DEFAULT);
        wxFile outputFile(outputPath, wxFile::write);
        lua_pushboolean(L, outputFile.Write(outputStr, wxConvUTF8));

        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int PhraseListToWordList(lua_State* L)
        {
        if (!VerifyParameterCount(L, 2, __func__))
            {
            return 0;
            }
        wxString path(luaL_checkstring(L, 1), wxConvUTF8);
        if (!wxFile::Exists(path))
            {
            wxMessageBox(wxString::Format(_(L"%s: invalid file path."), wxString(__func__)),
                         _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
            lua_pushboolean(L, false);
            return 1;
            }
        grammar::phrase_collection phraseList;
        wxString inputFileBuffer;
        if (Wisteria::TextStream::ReadFile(path, inputFileBuffer))
            {
            phraseList.load_phrases(inputFileBuffer, true, false);
            }
        phraseList.remove_duplicates();

        wxString outputStr;
        for (auto pos = phraseList.get_phrases().cbegin(); pos != phraseList.get_phrases().cend();
             ++pos)
            {
            if (pos->first.get_word_count() == 1 &&
                pos->first.get_type() == grammar::phrase_type::phrase_wordy && !pos->second.empty())
                {
                outputStr += wxString::Format(L"%s\t%s\r\n", pos->first.to_string().c_str(),
                                              pos->second.c_str());
                }
            }

        // create the folder to the filepath, if necessary
        const wxString outputPath(luaL_checkstring(L, 2), wxConvUTF8);
        wxFileName::Mkdir(wxFileName(outputPath).GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

        wxFileName(outputPath).SetPermissions(wxS_DEFAULT);
        wxFile outputFile(outputPath, wxFile::write);
        lua_pushboolean(L, outputFile.Write(outputStr, wxConvUTF8));

        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int ExpandWordList(lua_State* L)
        {
        if (!VerifyParameterCount(L, 3, __func__))
            {
            return 0;
            }
        wxString path(luaL_checkstring(L, 1), wxConvUTF8);
        if (!wxFile::Exists(path))
            {
            wxMessageBox(wxString::Format(_(L"%s: invalid file path."), wxString(__func__)),
                         _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
            lua_pushboolean(L, false);
            return 1;
            }
        word_list wordList;
        wxString inputFileBuffer;
        std::vector<word_list::word_type> newStrings;
        if (Wisteria::TextStream::ReadFile(path, inputFileBuffer))
            {
            wordList.load_words(inputFileBuffer, false, false);
            }
        for (int i = 3; i <= lua_gettop(L); ++i)
            {
            const wxString itemToAppend(luaL_checkstring(L, i), wxConvUTF8);
            for (size_t j = 0; j < wordList.get_list_size(); ++j)
                {
                // English rule for adding an 's' to the end of a word
                if (itemToAppend.CmpNoCase(L"s") == 0 && wordList.get_words().at(j).length() > 1)
                    {
                    const wchar_t secondToLastLetter =
                        wordList.get_words().at(j).at(wordList.get_words().at(j).length() - 2);
                    const wchar_t lastLetter =
                        wordList.get_words().at(j).at(wordList.get_words().at(j).length() - 1);
                    if ((secondToLastLetter == L's' || secondToLastLetter == L'S') &&
                        (lastLetter == L'h' || lastLetter == L'H'))
                        {
                        newStrings.push_back(wordList.get_words().at(j) + _DT(L"es"));
                        continue;
                        }
                    if ((secondToLastLetter == L'c' || secondToLastLetter == L'C') &&
                        (lastLetter == L'h' || lastLetter == L'H'))
                        {
                        newStrings.push_back(wordList.get_words().at(j) + _DT(L"es"));
                        continue;
                        }
                    if (lastLetter == L's' || lastLetter == L'S' || lastLetter == L'x' ||
                        lastLetter == L'X' || lastLetter == L'z' || lastLetter == L'Z')
                        {
                        newStrings.push_back(wordList.get_words().at(j) + _DT(L"es"));
                        continue;
                        }
                    if (characters::is_character::is_consonant(secondToLastLetter) &&
                        (lastLetter == L'y' || lastLetter == L'Y'))
                        {
                        newStrings.push_back(wordList.get_words().at(j).substr(
                                                 0, wordList.get_words().at(j).length() - 1) +
                                             _DT(L"ies"));
                        continue;
                        }
                    }
                newStrings.push_back(wordList.get_words().at(j) + itemToAppend.wc_str());
                }
            }
        wordList.add_words(newStrings);
        wordList.remove_duplicates();

        wxString outputStr;
        // save the new list
        outputStr.reserve(wordList.get_list_size() * 5);
        for (size_t i = 0; i < wordList.get_list_size(); ++i)
            {
            outputStr += wordList.get_words().at(i).c_str() + wxString(L"\n");
            }
        outputStr.Trim(true);
        outputStr.Trim(false);
        // create the folder to the filepath, if necessary
        const wxString outputPath(luaL_checkstring(L, 2), wxConvUTF8);
        wxFileName::Mkdir(wxFileName(outputPath).GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

        wxFileName(outputPath).SetPermissions(wxS_DEFAULT);
        wxFile outputFile(outputPath, wxFile::write);
        lua_pushboolean(L, outputFile.Write(outputStr, wxConvUTF8));

        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int MergePhraseLists(lua_State* L)
        {
        if (!VerifyParameterCount(L, 2, __func__))
            {
            return 0;
            }

        grammar::phrase_collection phrases;
        wxString inputFile;
        for (int i = 1; i <= lua_gettop(L) - 1; ++i)
            {
            inputFile = wxString{ luaL_checkstring(L, i), wxConvUTF8 };
            if (wxFile::Exists(inputFile))
                {
                wxString inputFileBuffer;
                if (Wisteria::TextStream::ReadFile(inputFile, inputFileBuffer))
                    {
                    phrases.load_phrases(inputFileBuffer, false, true);
                    }
                }
            else
                {
                wxMessageBox(wxString::Format(_(L"%s: file not found."), inputFile),
                             _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
                lua_pushboolean(L, false);
                return 1;
                }
            }

        phrases.remove_duplicates();

        for (const auto& phrase : phrases.get_phrases())
            {
            if (phrase.first.to_string() == phrase.second)
                {
                wxMessageBox(wxString::Format(_(L"%s: phrase '%s' and suggested replacement are "
                                                "the same.\nPlease review your phrase file."),
                                              inputFile, phrase.first.to_string().c_str()),
                             _(L"Warning"), wxOK | wxICON_EXCLAMATION);
                }
            }

        // create the folder to the filepath, if necessary
        const wxString path{ luaL_checkstring(L, lua_gettop(L)), wxConvUTF8 };
        wxFileName::Mkdir(wxFileName{ path }.GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

        wxFileName{ path }.SetPermissions(wxS_DEFAULT);
        wxFile outputFile{ path, wxFile::write };
        lua_pushboolean(L, outputFile.Write(phrases.to_string(), wxConvUTF8));

        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int MergeWordLists(lua_State* L)
        {
        if (!VerifyParameterCount(L, 2, __func__))
            {
            return 0;
            }
        word_list wordList;
        wxString inputFileBuffer;
        for (int i = 1; i <= lua_gettop(L) - 1; ++i)
            {
            wxString inputFile{ luaL_checkstring(L, i), wxConvUTF8 };
            if (wxFile::Exists(inputFile))
                {
                if (Wisteria::TextStream::ReadFile(inputFile, inputFileBuffer))
                    {
                    wordList.load_words(inputFileBuffer, false, true);
                    }
                }
            else
                {
                wxMessageBox(wxString::Format(_(L"%s: file not found."), inputFile),
                             _(L"Script Error"), wxOK | wxICON_EXCLAMATION);
                lua_pushboolean(L, false);
                return 1;
                }
            }
        wordList.remove_duplicates();

        wxString outputStr;
        // save the new list
        outputStr.reserve(wordList.get_list_size() * 5);
        for (size_t i = 0; i < wordList.get_list_size(); ++i)
            {
            outputStr += wordList.get_words().at(i).c_str() + wxString(L"\n");
            }
        outputStr.Trim(true);
        outputStr.Trim(false);

        // create the folder to the filepath, if necessary
        const wxString path{ luaL_checkstring(L, lua_gettop(L)), wxConvUTF8 };
        wxFileName::Mkdir(wxFileName{ path }.GetPath(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);

        wxFileName{ path }.SetPermissions(wxS_DEFAULT);
        wxFile outputFile{ path, wxFile::write };
        lua_pushboolean(L, outputFile.Write(outputStr, wxConvUTF8));

        wxGetApp().Yield();
        return 1;
        }

    // PROJECT SETTINGS
    //-------------------------------------------------------------
    int SetReviewer(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->SetReviewer(wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int GetReviewer(lua_State* L)
        {
        lua_pushstring(L, wxGetApp().GetAppOptions()->GetReviewer().utf8_str());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int SetProjectLanguage(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->SetProjectLanguage(
            static_cast<readability::test_language>(static_cast<int>(luaL_checkinteger(L, 1))));
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int GetProjectLanguage(lua_State* L)
        {
        lua_pushnumber(L, static_cast<int>(wxGetApp().GetAppOptions()->GetProjectLanguage()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int SetTextStorageMethod(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->SetDocumentStorageMethod(
            static_cast<TextStorage>(static_cast<int>(luaL_checkinteger(L, 1))));
        return 0;
        }

    //-------------------------------------------------------------
    int GetTextStorageMethod(lua_State* L)
        {
        lua_pushnumber(L, static_cast<int>(wxGetApp().GetAppOptions()->GetDocumentStorageMethod()));
        return 1;
        }

    //-------------------------------------------------------------
    int SetParagraphsParsingMethod(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->SetParagraphsParsingMethod(
            static_cast<ParagraphParse>(luaL_checkinteger(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int SetLongSentenceMethod(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetLongSentenceMethod(
            static_cast<LongSentence>(luaL_checkinteger(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int GetLongSentenceMethod(lua_State* L)
        {
        lua_pushnumber(L, static_cast<int>(wxGetApp().GetAppOptions()->GetLongSentenceMethod()));
        return 1;
        }

    //-------------------------------------------------------------
    int SetDifficultSentenceLength(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetDifficultSentenceLength(luaL_checkinteger(L, 1));
        return 0;
        }

    //-------------------------------------------------------------
    int GetDifficultSentenceLength(lua_State* L)
        {
        lua_pushnumber(L, wxGetApp().GetAppOptions()->GetDifficultSentenceLength());
        return 1;
        }

    //-------------------------------------------------------------
    int GetParagraphsParsingMethod(lua_State* L)
        {
        lua_pushnumber(L,
                       static_cast<int>(wxGetApp().GetAppOptions()->GetParagraphsParsingMethod()));
        return 1;
        }

    //-------------------------------------------------------------
    int IgnoreBlankLines(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->IgnoreBlankLinesForParagraphsParser(
            int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsIgnoringBlankLines(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetAppOptions()->IsIgnoringBlankLinesForParagraphsParser());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int IgnoreIndenting(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->IgnoreIndentingForParagraphsParser(
            int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsIgnoringIndenting(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetAppOptions()->IsIgnoringIndentingForParagraphsParser());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int SetSentenceStartMustBeUppercased(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetSentenceStartMustBeUppercased(
            int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int SentenceStartMustBeUppercased(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetAppOptions()->GetSentenceStartMustBeUppercased());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int SetMinDocWordCountForBatch(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->SetMinDocWordCountForBatch(luaL_checkinteger(L, 1));
        return 0;
        }

    //-------------------------------------------------------------
    int GetMinDocWordCountForBatch(lua_State* L)
        {
        lua_pushnumber(L,
                       static_cast<int>(wxGetApp().GetAppOptions()->GetMinDocWordCountForBatch()));
        return 1;
        }

    //-------------------------------------------------------------
    int SetFilePathDisplayMode(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->SetFilePathTruncationMode(
            static_cast<Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode>(
                luaL_checkinteger(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int GetFilePathDisplayMode(lua_State* L)
        {
        lua_pushnumber(L,
                       static_cast<int>(wxGetApp().GetAppOptions()->GetFilePathTruncationMode()));
        return 1;
        }

    //-------------------------------------------------------------
    int ImportSettings(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        const wxString filePath(luaL_checkstring(L, 1), wxConvUTF8);
        const auto reviewer = wxGetApp().GetAppOptions()->GetReviewer();
        lua_pushboolean(L, wxGetApp().GetAppOptions()->LoadOptionsFile(filePath, false, false));
        wxGetApp().GetAppOptions()->SetReviewer(reviewer);
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int ExportSettings(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        const wxString filePath(luaL_checkstring(L, 1), wxConvUTF8);
        lua_pushboolean(L, wxGetApp().GetAppOptions()->SaveOptionsFile(filePath));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int ResetSettings([[maybe_unused]] lua_State* L)
        {
        wxGetApp().GetAppOptions()->ResetSettings();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int DisableAllWarnings([[maybe_unused]] lua_State* L)
        {
        WarningManager::DisableWarnings();
        return 0;
        }

    //-------------------------------------------------------------
    int EnableAllWarnings([[maybe_unused]] lua_State* L)
        {
        WarningManager::EnableWarnings();
        return 0;
        }

    //-------------------------------------------------------------
    int EnableWarning(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        WarningManager::EnableWarning(wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        return 0;
        }

    //-------------------------------------------------------------
    int DisableWarning(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        WarningManager::DisableWarning(wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        return 0;
        }

    //-------------------------------------------------------------
    int EnableVerboseLogging(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxLog::SetVerbose(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsLoggingVerbose(lua_State* L)
        {
        lua_pushboolean(L, wxLog::GetVerbose());
        return 1;
        }

    //-------------------------------------------------------------
    int AppendDailyLog(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        if (wxGetApp().GetLogFile() != nullptr)
            {
            wxGetApp().AppendDailyLog(int_to_bool(lua_toboolean(L, 1)));
            }
        return 0;
        }

    //-------------------------------------------------------------
    int IsAppendingDailyLog(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().IsAppendingDailyLog());
        return 1;
        }

    //-------------------------------------------------------------
    int SetUserAgent(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetWebHarvester().SetUserAgent(wxString{ luaL_checkstring(L, 1), wxConvUTF8 });

        return 0;
        }

    //-------------------------------------------------------------
    int GetUserAgent(lua_State* L)
        {
        lua_pushstring(L, wxGetApp().GetWebHarvester().GetUserAgent().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int DisableSSLVerification(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetWebHarvester().DisablePeerVerify(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsSSLVerificationDisabled(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetWebHarvester().IsPeerVerifyDisabled());
        return 1;
        }

    //-------------------------------------------------------------
    int UseJavaScriptCookies(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetWebHarvester().UseJavaScriptCookies(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsUsingJavaScriptCookies(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetWebHarvester().IsUsingJavaScriptCookies());
        return 1;
        }

    //-------------------------------------------------------------
    int PersistCookies(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetWebHarvester().PersistJavaScriptCookies(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsPersistingCookies(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetWebHarvester().IsPersistingJavaScriptCookies());
        return 1;
        }

    //-------------------------------------------------------------
    int SetTextExclusion(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->SetInvalidSentenceMethod(
            static_cast<InvalidSentence>(luaL_checkinteger(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int GetTextExclusion(lua_State* L)
        {
        lua_pushinteger(L,
                        static_cast<int>(wxGetApp().GetAppOptions()->GetInvalidSentenceMethod()));
        return 1;
        }

    //-------------------------------------------------------------
    int SetIncludeIncompleteTolerance(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->SetIncludeIncompleteSentencesIfLongerThanValue(
            luaL_checkinteger(L, 1));
        return 0;
        }

    //-------------------------------------------------------------
    int GetIncludeIncompleteTolerance(lua_State* L)
        {
        lua_pushnumber(
            L, wxGetApp().GetAppOptions()->GetIncludeIncompleteSentencesIfLongerThanValue());
        return 1;
        }

    //-------------------------------------------------------------
    int AggressivelyExclude(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->AggressiveExclusion(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int ExcludeCopyrightNotices(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->ExcludeTrailingCopyrightNoticeParagraphs(
            int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int ExcludeTrailingCitations(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->ExcludeTrailingCitations(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int ExcludeFileAddress(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->ExcludeFileAddresses(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int ExcludeNumerals(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->ExcludeNumerals(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int ExcludeProperNouns(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->ExcludeProperNouns(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsExcludingAggressively(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetAppOptions()->IsExcludingAggressively());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int IsExcludingCopyrightNotices(lua_State* L)
        {
        lua_pushboolean(L,
                        wxGetApp().GetAppOptions()->IsExcludingTrailingCopyrightNoticeParagraphs());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int IsExcludingTrailingCitations(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetAppOptions()->IsExcludingTrailingCitations());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int IsExcludingFileAddresses(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetAppOptions()->IsExcludingFileAddresses());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int IsExcludingNumerals(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetAppOptions()->IsExcludingNumerals());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int IsExcludingProperNouns(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetAppOptions()->IsExcludingProperNouns());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int SetPhraseExclusionList(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->SetExcludedPhrasesPath(
            wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        return 0;
        }

    //-------------------------------------------------------------
    int GetPhraseExclusionList(lua_State* L)
        {
        lua_pushstring(L, wxGetApp().GetAppOptions()->GetExcludedPhrasesPath().utf8_str());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int IncludeExcludedPhraseFirstOccurrence(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->IncludeExcludedPhraseFirstOccurrence(
            int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsIncludingExcludedPhraseFirstOccurrence(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetAppOptions()->IsIncludingExcludedPhraseFirstOccurrence());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int SetBlockExclusionTags(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        const wxString exclusionTags(luaL_checkstring(L, 1), wxConvUTF8);
        if (exclusionTags.length() >= 2)
            {
            wxGetApp().GetAppOptions()->GetExclusionBlockTags().clear();
            wxGetApp().GetAppOptions()->GetExclusionBlockTags().emplace_back(exclusionTags[0],
                                                                             exclusionTags[1]);
            }
        return 0;
        }

    //-------------------------------------------------------------
    int GetBlockExclusionTags(lua_State* L)
        {
        lua_pushstring(
            L,
            wxGetApp().GetAppOptions()->GetExclusionBlockTags().empty() ?
                "" :
                wxString{ std::to_wstring(
                              wxGetApp().GetAppOptions()->GetExclusionBlockTags().front().first) +
                          std::to_wstring(
                              wxGetApp().GetAppOptions()->GetExclusionBlockTags().front().second) }
                    .utf8_str());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int SetNumeralSyllabication(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetNumeralSyllabicationMethod(
            static_cast<NumeralSyllabize>(luaL_checkinteger(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int GetNumeralSyllabication(lua_State* L)
        {
        lua_pushnumber(
            L, static_cast<int>(wxGetApp().GetAppOptions()->GetNumeralSyllabicationMethod()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int IsFogUsingSentenceUnits(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetAppOptions()->IsFogUsingSentenceUnits());
        return 1;
        }

    //-------------------------------------------------------------
    int FogUseSentenceUnits(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->FogUseSentenceUnits(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IncludeStockerCatholicSupplement(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->IncludeStockerCatholicSupplement(
            int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsIncludingStockerCatholicSupplement(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetAppOptions()->IsIncludingStockerCatholicSupplement());
        return 1;
        }

    //-------------------------------------------------------------
    int IncludeScoreSummaryReport(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->IncludeScoreSummaryReport(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsIncludingScoreSummaryReport(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetAppOptions()->IsIncludingScoreSummaryReport());
        return 1;
        }

    //-------------------------------------------------------------
    int SetLongGradeScaleFormat(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->GetReadabilityMessageCatalog().SetLongGradeScaleFormat(
            int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsUsingLongGradeScaleFormat(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp()
                               .GetAppOptions()
                               ->GetReadabilityMessageCatalog()
                               .IsUsingLongGradeScaleFormat());
        return 1;
        }

    //-------------------------------------------------------------
    int SetReadingAgeDisplay(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->GetReadabilityMessageCatalog().SetReadingAgeDisplay(
            static_cast<ReadabilityMessages::ReadingAgeDisplay>(luaL_checkinteger(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int GetReadingAgeDisplay(lua_State* L)
        {
        lua_pushnumber(
            L,
            static_cast<int>(
                wxGetApp().GetAppOptions()->GetReadabilityMessageCatalog().GetReadingAgeDisplay()));
        return 1;
        }

    //-------------------------------------------------------------
    int SetGradeScale(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->GetReadabilityMessageCatalog().SetGradeScale(
            static_cast<readability::grade_scale>(luaL_checkinteger(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int GetGradeScale(lua_State* L)
        {
        lua_pushnumber(
            L, static_cast<int>(
                   wxGetApp().GetAppOptions()->GetReadabilityMessageCatalog().GetGradeScale()));
        return 1;
        }

    //-------------------------------------------------------------
    int SetFleschNumeralSyllabizeMethod(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetFleschNumeralSyllabizeMethod(
            static_cast<FleschNumeralSyllabize>(luaL_checkinteger(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int GetFleschNumeralSyllabizeMethod(lua_State* L)
        {
        lua_pushnumber(
            L, static_cast<int>(wxGetApp().GetAppOptions()->GetFleschNumeralSyllabizeMethod()));
        return 1;
        }

    //-------------------------------------------------------------
    int SetFleschKincaidNumeralSyllabizeMethod(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetFleschKincaidNumeralSyllabizeMethod(
            static_cast<FleschKincaidNumeralSyllabize>(luaL_checkinteger(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int GetFleschKincaidNumeralSyllabizeMethod(lua_State* L)
        {
        lua_pushnumber(
            L,
            static_cast<int>(wxGetApp().GetAppOptions()->GetFleschKincaidNumeralSyllabizeMethod()));
        return 1;
        }

    //-------------------------------------------------------------
    int SetHarrisJacobsonTextExclusionMode(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetHarrisJacobsonTextExclusionMode(
            static_cast<SpecializedTestTextExclusion>(luaL_checkinteger(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int GetHarrisJacobsonTextExclusionMode(lua_State* L)
        {
        lua_pushnumber(
            L, static_cast<int>(wxGetApp().GetAppOptions()->GetHarrisJacobsonTextExclusionMode()));
        return 1;
        }

    //-------------------------------------------------------------
    int SetDaleChallTextExclusionMode(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetDaleChallTextExclusionMode(
            static_cast<SpecializedTestTextExclusion>(luaL_checkinteger(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int GetDaleChallTextExclusionMode(lua_State* L)
        {
        lua_pushnumber(
            L, static_cast<int>(wxGetApp().GetAppOptions()->GetDaleChallTextExclusionMode()));
        return 1;
        }

    //-------------------------------------------------------------
    int SetDaleChallProperNounCountingMethod(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetDaleChallProperNounCountingMethod(
            static_cast<readability::proper_noun_counting_method>(luaL_checkinteger(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int GetDaleChallProperNounCountingMethod(lua_State* L)
        {
        lua_pushnumber(L, static_cast<int>(
                              wxGetApp().GetAppOptions()->GetDaleChallProperNounCountingMethod()));
        return 1;
        }

    //-------------------------------------------------
    int GetAppendedDocumentFilePath(lua_State* L)
        {
        lua_pushstring(L, wxGetApp().GetAppOptions()->GetAppendedDocumentFilePath().utf8_str());
        return 1;
        }

    //-------------------------------------------------
    int SetAppendedDocumentFilePath(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetAppendedDocumentFilePath(
            wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        return 0;
        }

    //-------------------------------------------------------------
    int IsRealTimeUpdating(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetAppOptions()->IsRealTimeUpdating());
        return 1;
        }

    //-------------------------------------------------------------
    int UseRealTimeUpdate(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->UseRealTimeUpdate(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    // GRAPH OPTIONS
    //-------------------------------------------------------------
    int SetGraphBackgroundColor(lua_State* L)
        {
        if (!VerifyParameterCount(L, 3, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->SetBackGroundColor(
            LoadColor(wxString{ luaL_checkstring(L, 1), wxConvUTF8 }));
        return 0;
        }

    //-------------------------------------------------------------
    int ApplyGraphBackgroundFade(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->SetGraphBackGroundLinearGradient(
            int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsApplyingGraphBackgroundFade(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetAppOptions()->GetGraphBackGroundLinearGradient());
        return 1;
        }

    //-------------------------------------------------------------
    int SetGraphCommonImage(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->SetGraphCommonImagePath(
            wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        return 0;
        }

    //-------------------------------------------------------------
    int GetGraphCommonImage(lua_State* L)
        {
        lua_pushstring(L, wxGetApp().GetAppOptions()->GetGraphCommonImagePath().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int SetPlotBackgroundImage(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->SetPlotBackGroundImagePath(
            wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        return 0;
        }

    //-------------------------------------------------------------
    int GetPlotBackgroundImage(lua_State* L)
        {
        lua_pushstring(L, wxGetApp().GetAppOptions()->GetPlotBackGroundImagePath().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int SetPlotBackgroundImageEffect(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetPlotBackGroundImageEffect(
            static_cast<Wisteria::ImageEffect>(luaL_checkinteger(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int GetPlotBackgroundImageEffect(lua_State* L)
        {
        lua_pushnumber(
            L, static_cast<int>(wxGetApp().GetAppOptions()->GetPlotBackGroundImageEffect()));
        return 1;
        }

    //-------------------------------------------------------------
    int SetPlotBackgroundImageFit(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetPlotBackGroundImageFit(
            static_cast<Wisteria::ImageFit>(luaL_checkinteger(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int GetPlotBackgroundImageFit(lua_State* L)
        {
        lua_pushnumber(L,
                       static_cast<int>(wxGetApp().GetAppOptions()->GetPlotBackGroundImageFit()));
        return 1;
        }

    //-------------------------------------------------------------
    int SetXAxisFont(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        auto fontInfo = wxGetApp().GetAppOptions()->GetXAxisFont();
        auto fontColor = wxGetApp().GetAppOptions()->GetXAxisFontColor();
        LoadFontAttributes(L, fontInfo, fontColor, false);

        wxGetApp().GetAppOptions()->SetXAxisFont(fontInfo);
        wxGetApp().GetAppOptions()->SetXAxisFontColor(fontColor);
        return 0;
        }

    //-------------------------------------------------------------
    int SetYAxisFont(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        auto fontInfo = wxGetApp().GetAppOptions()->GetYAxisFont();
        auto fontColor = wxGetApp().GetAppOptions()->GetYAxisFontColor();
        LoadFontAttributes(L, fontInfo, fontColor, false);

        wxGetApp().GetAppOptions()->SetYAxisFont(fontInfo);
        wxGetApp().GetAppOptions()->SetYAxisFontColor(fontColor);
        return 0;
        }

    //-------------------------------------------------------------
    int SetGraphTopTitleFont(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        auto fontInfo = wxGetApp().GetAppOptions()->GetGraphTopTitleFont();
        auto fontColor = wxGetApp().GetAppOptions()->GetGraphTopTitleFontColor();
        LoadFontAttributes(L, fontInfo, fontColor, false);

        wxGetApp().GetAppOptions()->SetGraphTopTitleFont(fontInfo);
        wxGetApp().GetAppOptions()->SetGraphTopTitleFontColor(fontColor);
        return 0;
        }

    //-------------------------------------------------------------
    int SetGraphBottomTitleFont(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        auto fontInfo = wxGetApp().GetAppOptions()->GetGraphBottomTitleFont();
        auto fontColor = wxGetApp().GetAppOptions()->GetGraphBottomTitleFontColor();
        LoadFontAttributes(L, fontInfo, fontColor, false);

        wxGetApp().GetAppOptions()->SetGraphBottomTitleFont(fontInfo);
        wxGetApp().GetAppOptions()->SetGraphBottomTitleFontColor(fontColor);
        return 0;
        }

    //-------------------------------------------------------------
    int SetGraphRightTitleFont(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        auto fontInfo = wxGetApp().GetAppOptions()->GetGraphRightTitleFont();
        auto fontColor = wxGetApp().GetAppOptions()->GetGraphRightTitleFontColor();
        LoadFontAttributes(L, fontInfo, fontColor, false);

        wxGetApp().GetAppOptions()->SetGraphRightTitleFont(fontInfo);
        wxGetApp().GetAppOptions()->SetGraphRightTitleFontColor(fontColor);
        return 0;
        }

    //-------------------------------------------------------------
    int SetGraphLeftTitleFont(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        auto fontInfo = wxGetApp().GetAppOptions()->GetGraphLeftTitleFont();
        auto fontColor = wxGetApp().GetAppOptions()->GetGraphLeftTitleFontColor();
        LoadFontAttributes(L, fontInfo, fontColor, false);

        wxGetApp().GetAppOptions()->SetGraphLeftTitleFont(fontInfo);
        wxGetApp().GetAppOptions()->SetGraphLeftTitleFontColor(fontColor);
        return 0;
        }

    //-------------------------------------------------------------
    int DisplayBarChartLabels(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->DisplayBarChartLabels(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int SetGraphInvalidRegionColor(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetInvalidAreaColor(
            LoadColor(wxString{ luaL_checkstring(L, 1), wxConvUTF8 }));
        return 0;
        }

    //-------------------------------------------------------------
    int SetRaygorStyle(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetRaygorStyle(
            static_cast<Wisteria::Graphs::RaygorStyle>(luaL_checkinteger(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int GetRaygorStyle(lua_State* L)
        {
        lua_pushnumber(L, static_cast<int>(wxGetApp().GetAppOptions()->GetRaygorStyle()));
        return 1;
        }

    //-------------------------------------------------------------
    int ConnectFleschPoints(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->ConnectFleschPoints(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsConnectingFleschPoints(lua_State* L)
        {
        lua_pushnumber(L, static_cast<int>(wxGetApp().GetAppOptions()->IsConnectingFleschPoints()));
        return 1;
        }

    //-------------------------------------------------------------
    int IncludeFleschRulerDocGroups(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->IncludeFleschRulerDocGroups(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsIncludingFleschRulerDocGroups(lua_State* L)
        {
        lua_pushnumber(
            L, static_cast<int>(wxGetApp().GetAppOptions()->IsIncludingFleschRulerDocGroups()));
        return 1;
        }

    //-------------------------------------------------------------
    int UseEnglishLabelsForGermanLix(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->UseEnglishLabelsForGermanLix(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsUsingEnglishLabelsForGermanLix(lua_State* L)
        {
        lua_pushnumber(
            L, static_cast<int>(wxGetApp().GetAppOptions()->IsUsingEnglishLabelsForGermanLix()));
        return 1;
        }

    //-------------------------------------------------------------
    int SetStippleShapeColor(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetStippleShapeColor(
            LoadColor(wxString{ luaL_checkstring(L, 1), wxConvUTF8 }));
        return 0;
        }

    //-------------------------------------------------------------
    int ShowcaseKeyItems(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->ShowcaseKeyItems(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsShowcasingKeyItems(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetAppOptions()->IsShowcasingKeyItems());
        return 1;
        }

    //-------------------------------------------------------------
    int SetPlotBackgroundImageOpacity(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetPlotBackGroundImageOpacity(luaL_checkinteger(L, 1));
        return 0;
        }

    //-------------------------------------------------------------
    int GetPlotBackgroundImageOpacity(lua_State* L)
        {
        lua_pushnumber(L, wxGetApp().GetAppOptions()->GetPlotBackGroundImageOpacity());
        return 1;
        }

    //-------------------------------------------------------------
    int SetPlotBackgroundColor(lua_State* L)
        {
        if (!VerifyParameterCount(L, 3, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetPlotBackGroundColor(
            LoadColor(wxString{ luaL_checkstring(L, 1), wxConvUTF8 }));
        return 0;
        }

    //-------------------------------------------------------------
    int SetPlotBackgroundColorOpacity(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetPlotBackGroundColorOpacity(luaL_checkinteger(L, 1));
        return 0;
        }

    //-------------------------------------------------------------
    int GetPlotBackgroundColorOpacity(lua_State* L)
        {
        lua_pushnumber(L, wxGetApp().GetAppOptions()->GetPlotBackGroundColorOpacity());
        return 1;
        }

    //-------------------------------------------------------------
    int SetGraphColorScheme(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetGraphColorScheme(
            wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        return 0;
        }

    //-------------------------------------------------------------
    int GetGraphColorScheme(lua_State* L)
        {
        lua_pushstring(L, wxGetApp().GetAppOptions()->GetGraphColorScheme().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int SetWatermark(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetWatermark(wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        return 0;
        }

    //-------------------------------------------------------------
    int GetWatermark(lua_State* L)
        {
        lua_pushstring(L, wxGetApp().GetAppOptions()->GetWatermark().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int SetGraphLogoImage(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetWatermarkLogo(
            wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        return 0;
        }

    //-------------------------------------------------------------
    int GetGraphLogoImage(lua_State* L)
        {
        lua_pushstring(L, wxGetApp().GetAppOptions()->GetWatermarkLogo().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int SetStippleImage(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetStippleImagePath(
            wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        return 0;
        }

    //-------------------------------------------------------------
    int GetStippleImage(lua_State* L)
        {
        lua_pushstring(L, wxGetApp().GetAppOptions()->GetStippleImagePath().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int SetStippleShape(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetStippleShape(wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        return 0;
        }

    //-------------------------------------------------------------
    int GetStippleShape(lua_State* L)
        {
        lua_pushstring(L, wxGetApp().GetAppOptions()->GetStippleShape().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int DisplayGraphDropShadows(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->DisplayDropShadows(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsDisplayingGraphDropShadows(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetAppOptions()->IsDisplayingDropShadows());
        return 1;
        }

    //-------------------------------------------------------------
    int SetBarChartBarColor(lua_State* L)
        {
        if (!VerifyParameterCount(L, 3, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetBarChartBarColor(
            LoadColor(wxString{ luaL_checkstring(L, 1), wxConvUTF8 }));
        return 0;
        }

    //-------------------------------------------------------------
    int SetBarChartBarOpacity(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetGraphBarOpacity(luaL_checkinteger(L, 1));
        return 0;
        }

    //-------------------------------------------------------------
    int SetBarChartBarEffect(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetGraphBarEffect(
            static_cast<Wisteria::BoxEffect>(luaL_checkinteger(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int GetBarChartBarEffect(lua_State* L)
        {
        lua_pushnumber(L, static_cast<int>(wxGetApp().GetAppOptions()->GetGraphBarEffect()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int GetBarChartOrientation(lua_State* L)
        {
        lua_pushnumber(L, static_cast<int>(wxGetApp().GetAppOptions()->GetBarChartOrientation()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int GetBarChartBarOpacity(lua_State* L)
        {
        lua_pushnumber(L, static_cast<int>(wxGetApp().GetAppOptions()->GetGraphBarOpacity()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int IsDisplayingBarChartLabels(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetAppOptions()->IsDisplayingBarChartLabels());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int SetBarChartOrientation(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetBarChartOrientation(
            static_cast<Wisteria::Orientation>(luaL_checkinteger(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int SetHistogramBarColor(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetHistogramBarColor(
            LoadColor(wxString{ luaL_checkstring(L, 1), wxConvUTF8 }));
        return 0;
        }

    //-------------------------------------------------------------
    int SetHistogramBarOpacity(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetHistogramBarOpacity(luaL_checkinteger(L, 1));
        return 0;
        }

    //-------------------------------------------------------------
    int SetHistogramBarEffect(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetHistogramBarEffect(
            static_cast<Wisteria::BoxEffect>(luaL_checkinteger(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int GetHistogramBarEffect(lua_State* L)
        {
        lua_pushnumber(L, static_cast<int>(wxGetApp().GetAppOptions()->GetHistogramBarEffect()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int GetHistogramBarOpacity(lua_State* L)
        {
        lua_pushnumber(L, static_cast<int>(wxGetApp().GetAppOptions()->GetHistogramBarOpacity()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int SetHistogramBinning(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetHistogramBinningMethod(
            static_cast<Wisteria::Graphs::Histogram::BinningMethod>(luaL_checkinteger(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int GetHistogramBinning(lua_State* L)
        {
        lua_pushnumber(L,
                       static_cast<int>(wxGetApp().GetAppOptions()->GetHistogramBinningMethod()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int SetHistogramIntervalDisplay(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetHistogramIntervalDisplay(
            static_cast<Wisteria::Graphs::Histogram::IntervalDisplay>(luaL_checkinteger(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int GetHistogramIntervalDisplay(lua_State* L)
        {
        lua_pushnumber(L,
                       static_cast<int>(wxGetApp().GetAppOptions()->GetHistogramIntervalDisplay()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int SetHistogramRounding(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetHistogramRoundingMethod(
            static_cast<Wisteria::RoundingMethod>(luaL_checkinteger(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int GetHistogramRounding(lua_State* L)
        {
        lua_pushnumber(L,
                       static_cast<int>(wxGetApp().GetAppOptions()->GetHistogramRoundingMethod()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int SetHistogramBinLabelDisplay(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetHistogramBinLabelDisplay(
            static_cast<Wisteria::BinLabelDisplay>(luaL_checkinteger(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int GetHistogramBinLabelDisplay(lua_State* L)
        {
        lua_pushnumber(L,
                       static_cast<int>(wxGetApp().GetAppOptions()->GetHistogramBinLabelDisplay()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int SetBoxPlotColor(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetGraphBoxColor(
            LoadColor(wxString{ luaL_checkstring(L, 1), wxConvUTF8 }));
        return 0;
        }

    //-------------------------------------------------------------
    int SetBoxPlotOpacity(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetGraphBoxOpacity(luaL_checkinteger(L, 1));
        return 0;
        }

    //-------------------------------------------------------------
    int SetBoxPlotEffect(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetGraphBoxEffect(
            static_cast<Wisteria::BoxEffect>(luaL_checkinteger(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int GetBoxPlotEffect(lua_State* L)
        {
        lua_pushnumber(L, static_cast<int>(wxGetApp().GetAppOptions()->GetGraphBoxEffect()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int GetBoxPlotOpacity(lua_State* L)
        {
        lua_pushnumber(L, static_cast<int>(wxGetApp().GetAppOptions()->GetGraphBoxOpacity()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int DisplayBoxPlotLabels(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->DisplayBoxPlotLabels(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsDisplayingBoxPlotLabels(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetAppOptions()->IsDisplayingBoxPlotLabels());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int ConnectBoxPlotMiddlePoints(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->ConnectBoxPlotMiddlePoints(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsConnectingBoxPlotMiddlePoints(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetAppOptions()->IsConnectingBoxPlotMiddlePoints());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int DisplayAllBoxPlotPoints(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->ShowAllBoxPlotPoints(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsDisplayingAllBoxPlotPoints(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetAppOptions()->IsShowingAllBoxPlotPoints());
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int SetSpellCheckerOptions(lua_State* L)
        {
        if (lua_gettop(L) >= 1)
            {
            wxGetApp().GetAppOptions()->SpellCheckIgnoreProperNouns(
                int_to_bool(lua_toboolean(L, 1)));
            }
        if (lua_gettop(L) >= 2)
            {
            wxGetApp().GetAppOptions()->SpellCheckIgnoreUppercased(
                int_to_bool(lua_toboolean(L, 2)));
            }
        if (lua_gettop(L) >= 3)
            {
            wxGetApp().GetAppOptions()->SpellCheckIgnoreNumerals(int_to_bool(lua_toboolean(L, 3)));
            }
        if (lua_gettop(L) >= 4)
            {
            wxGetApp().GetAppOptions()->SpellCheckIgnoreFileAddresses(
                int_to_bool(lua_toboolean(L, 4)));
            }
        if (lua_gettop(L) >= 5)
            {
            wxGetApp().GetAppOptions()->SpellCheckIgnoreProgrammerCode(
                int_to_bool(lua_toboolean(L, 5)));
            }
        if (lua_gettop(L) >= 6)
            {
            wxGetApp().GetAppOptions()->SpellCheckIgnoreSocialMediaTags(
                int_to_bool(lua_toboolean(L, 6)));
            }
        if (lua_gettop(L) >= 7)
            {
            wxGetApp().GetAppOptions()->SpellCheckAllowColloquialisms(
                int_to_bool(lua_toboolean(L, 7)));
            }
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int SetWordsBreakdownResultsOptions(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        if (lua_gettop(L) >= 1)
            {
            wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().EnableWordBarchart(
                int_to_bool(lua_toboolean(L, 1)));
            }
        if (lua_gettop(L) >= 2)
            {
            wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().EnableSyllableHistogram(
                int_to_bool(lua_toboolean(L, 2)));
            }
        if (lua_gettop(L) >= 3)
            {
            wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().Enable3PlusSyllables(
                int_to_bool(lua_toboolean(L, 3)));
            }
        if (lua_gettop(L) >= 4)
            {
            wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().Enable6PlusCharacter(
                int_to_bool(lua_toboolean(L, 4)));
            }
        if (lua_gettop(L) >= 5)
            {
            wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().EnableWordCloud(
                int_to_bool(lua_toboolean(L, 5)));
            }
        if (lua_gettop(L) >= 6)
            {
            wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().EnableDCUnfamiliar(
                int_to_bool(lua_toboolean(L, 6)));
            }
        if (lua_gettop(L) >= 7)
            {
            wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().EnableSpacheUnfamiliar(
                int_to_bool(lua_toboolean(L, 7)));
            }
        if (lua_gettop(L) >= 8)
            {
            wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().EnableHarrisJacobsonUnfamiliar(
                int_to_bool(lua_toboolean(L, 8)));
            }
        if (lua_gettop(L) >= 9)
            {
            wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().EnableCustomTestsUnfamiliar(
                int_to_bool(lua_toboolean(L, 9)));
            }
        if (lua_gettop(L) >= 10)
            {
            wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().EnableAllWords(
                int_to_bool(lua_toboolean(L, 10)));
            }
        if (lua_gettop(L) >= 11)
            {
            wxGetApp().GetAppOptions()->GetWordsBreakdownInfo().EnableKeyWords(
                int_to_bool(lua_toboolean(L, 11)));
            }

        return 0;
        }

    //-------------------------------------------------------------
    int SetSummaryStatsResultsOptions(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        if (lua_gettop(L) >= 1)
            {
            wxGetApp().GetAppOptions()->GetStatisticsInfo().EnableReport(
                int_to_bool(lua_toboolean(L, 1)));
            }
        if (lua_gettop(L) >= 2)
            {
            wxGetApp().GetAppOptions()->GetStatisticsInfo().EnableTable(
                int_to_bool(lua_toboolean(L, 2)));
            }

        return 0;
        }

    //-------------------------------------------------------------
    int SetSummaryStatsReportOptions(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        if (lua_gettop(L) >= 1)
            {
            wxGetApp().GetAppOptions()->GetStatisticsReportInfo().EnableParagraph(
                int_to_bool(lua_toboolean(L, 1)));
            }
        if (lua_gettop(L) >= 2)
            {
            wxGetApp().GetAppOptions()->GetStatisticsReportInfo().EnableWords(
                int_to_bool(lua_toboolean(L, 2)));
            }
        if (lua_gettop(L) >= 3)
            {
            wxGetApp().GetAppOptions()->GetStatisticsReportInfo().EnableSentences(
                int_to_bool(lua_toboolean(L, 3)));
            }
        if (lua_gettop(L) >= 4)
            {
            wxGetApp().GetAppOptions()->GetStatisticsReportInfo().EnableExtendedWords(
                int_to_bool(lua_toboolean(L, 4)));
            }
        if (lua_gettop(L) >= 5)
            {
            wxGetApp().GetAppOptions()->GetStatisticsReportInfo().EnableGrammar(
                int_to_bool(lua_toboolean(L, 5)));
            }
        if (lua_gettop(L) >= 6)
            {
            wxGetApp().GetAppOptions()->GetStatisticsReportInfo().EnableNotes(
                int_to_bool(lua_toboolean(L, 67)));
            }
        if (lua_gettop(L) >= 7)
            {
            wxGetApp().GetAppOptions()->GetStatisticsReportInfo().EnableExtendedInformation(
                int_to_bool(lua_toboolean(L, 7)));
            }

        return 0;
        }

    //-------------------------------------------------------------
    int SetSummaryStatsDolchReportOptions(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        if (lua_gettop(L) >= 1)
            {
            wxGetApp().GetAppOptions()->GetStatisticsReportInfo().EnableDolchCoverage(
                int_to_bool(lua_toboolean(L, 1)));
            }
        if (lua_gettop(L) >= 2)
            {
            wxGetApp().GetAppOptions()->GetStatisticsReportInfo().EnableDolchWords(
                int_to_bool(lua_toboolean(L, 2)));
            }
        if (lua_gettop(L) >= 3)
            {
            wxGetApp().GetAppOptions()->GetStatisticsReportInfo().EnableDolchExplanation(
                int_to_bool(lua_toboolean(L, 3)));
            }

        return 0;
        }

    //-------------------------------------------------------------
    int SetSentenceBreakdownResultsOptions(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        if (lua_gettop(L) >= 1)
            {
            wxGetApp().GetAppOptions()->GetSentencesBreakdownInfo().EnableLongSentences(
                int_to_bool(lua_toboolean(L, 1)));
            }
        if (lua_gettop(L) >= 2)
            {
            wxGetApp().GetAppOptions()->GetSentencesBreakdownInfo().EnableLengthsBoxPlot(
                int_to_bool(lua_toboolean(L, 2)));
            }
        if (lua_gettop(L) >= 3)
            {
            wxGetApp().GetAppOptions()->GetSentencesBreakdownInfo().EnableLengthsHistogram(
                int_to_bool(lua_toboolean(L, 3)));
            }
        if (lua_gettop(L) >= 4)
            {
            wxGetApp().GetAppOptions()->GetSentencesBreakdownInfo().EnableLengthsHeatmap(
                int_to_bool(lua_toboolean(L, 4)));
            }

        return 0;
        }

    //-------------------------------------------------------------
    int SetGrammarResultsOptions(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        if (lua_gettop(L) >= 1)
            {
            wxGetApp().GetAppOptions()->GetGrammarInfo().EnableHighlightedReport(
                int_to_bool(lua_toboolean(L, 1)));
            }
        if (lua_gettop(L) >= 2)
            {
            wxGetApp().GetAppOptions()->GetGrammarInfo().EnableWordingErrors(
                int_to_bool(lua_toboolean(L, 2)));
            }
        if (lua_gettop(L) >= 3)
            {
            wxGetApp().GetAppOptions()->GetGrammarInfo().EnableMisspellings(
                int_to_bool(lua_toboolean(L, 3)));
            }
        if (lua_gettop(L) >= 4)
            {
            wxGetApp().GetAppOptions()->GetGrammarInfo().EnableRepeatedWords(
                int_to_bool(lua_toboolean(L, 4)));
            }
        if (lua_gettop(L) >= 5)
            {
            wxGetApp().GetAppOptions()->GetGrammarInfo().EnableArticleMismatches(
                int_to_bool(lua_toboolean(L, 5)));
            }
        if (lua_gettop(L) >= 6)
            {
            wxGetApp().GetAppOptions()->GetGrammarInfo().EnableRedundantPhrases(
                int_to_bool(lua_toboolean(L, 6)));
            }
        if (lua_gettop(L) >= 7)
            {
            wxGetApp().GetAppOptions()->GetGrammarInfo().EnableOverUsedWordsBySentence(
                int_to_bool(lua_toboolean(L, 7)));
            }
        if (lua_gettop(L) >= 8)
            {
            wxGetApp().GetAppOptions()->GetGrammarInfo().EnableWordyPhrases(
                int_to_bool(lua_toboolean(L, 8)));
            }
        if (lua_gettop(L) >= 9)
            {
            wxGetApp().GetAppOptions()->GetGrammarInfo().EnableCliches(
                int_to_bool(lua_toboolean(L, 9)));
            }
        if (lua_gettop(L) >= 10)
            {
            wxGetApp().GetAppOptions()->GetGrammarInfo().EnablePassiveVoice(
                int_to_bool(lua_toboolean(L, 10)));
            }
        if (lua_gettop(L) >= 11)
            {
            wxGetApp().GetAppOptions()->GetGrammarInfo().EnableConjunctionStartingSentences(
                int_to_bool(lua_toboolean(L, 11)));
            }
        if (lua_gettop(L) >= 12)
            {
            wxGetApp().GetAppOptions()->GetGrammarInfo().EnableLowercaseSentences(
                int_to_bool(lua_toboolean(L, 12)));
            }

        return 0;
        }

    //-------------------------------------------------------------
    int SetReportFont(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        auto fontInfo = wxGetApp().GetAppOptions()->GetXAxisFont();
        auto fontColor = wxGetApp().GetAppOptions()->GetXAxisFontColor();
        LoadFontAttributes(L, fontInfo, fontColor, false);

        wxGetApp().GetAppOptions()->SetTextViewFont(fontInfo);
        wxGetApp().GetAppOptions()->SetTextFontColor(fontColor);
        return 0;
        }

    //-------------------------------------------------------------
    int SetExcludedTextHighlightColor(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetExcludedTextHighlightColor(
            LoadColor(wxString{ luaL_checkstring(L, 1), wxConvUTF8 }));
        return 0;
        }

    //-------------------------------------------------------------
    int SetDifficultTextHighlightColor(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetTextHighlightColor(
            LoadColor(wxString{ luaL_checkstring(L, 1), wxConvUTF8 }));
        return 0;
        }

    //-------------------------------------------------------------
    int SetGrammarIssuesHighlightColor(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetDuplicateWordHighlightColor(
            LoadColor(wxString{ luaL_checkstring(L, 1), wxConvUTF8 }));
        return 0;
        }

    //-------------------------------------------------------------
    int SetWordyTextHighlightColor(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetWordyPhraseHighlightColor(
            LoadColor(wxString{ luaL_checkstring(L, 1), wxConvUTF8 }));
        return 0;
        }

    //-------------------------------------------------------------
    int SetTextHighlighting(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetTextHighlightMethod(
            static_cast<TextHighlight>(luaL_checkinteger(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int GetTextHighlighting(lua_State* L)
        {
        lua_pushnumber(L, static_cast<int>(wxGetApp().GetAppOptions()->GetTextHighlightMethod()));
        wxGetApp().Yield();
        return 1;
        }

    //-------------------------------------------------------------
    int SetDolchConjunctionsColor(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetDolchConjunctionsColor(
            LoadColor(wxString{ luaL_checkstring(L, 1), wxConvUTF8 }));
        return 0;
        }

    //-------------------------------------------------------------
    int SetDolchPrepositionsColor(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetDolchPrepositionsColor(
            LoadColor(wxString{ luaL_checkstring(L, 1), wxConvUTF8 }));
        return 0;
        }

    //-------------------------------------------------------------
    int SetDolchPronounsColor(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetDolchPronounsColor(
            LoadColor(wxString{ luaL_checkstring(L, 1), wxConvUTF8 }));
        return 0;
        }

    //-------------------------------------------------------------
    int SetDolchAdverbsColor(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetDolchAdverbsColor(
            LoadColor(wxString{ luaL_checkstring(L, 1), wxConvUTF8 }));
        return 0;
        }

    //-------------------------------------------------------------
    int SetDolchAdjectivesColor(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetDolchAdjectivesColor(
            LoadColor(wxString{ luaL_checkstring(L, 1), wxConvUTF8 }));
        return 0;
        }

    //-------------------------------------------------------------
    int SetDolchVerbsColor(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetDolchVerbsColor(
            LoadColor(wxString{ luaL_checkstring(L, 1), wxConvUTF8 }));
        return 0;
        }

    //-------------------------------------------------------------
    int SetDolchNounsColor(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        wxGetApp().GetAppOptions()->SetDolchNounsColor(
            LoadColor(wxString{ luaL_checkstring(L, 1), wxConvUTF8 }));
        return 0;
        }

    //-------------------------------------------------------------
    int HighlightDolchConjunctions(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->HighlightDolchConjunctions(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsHighlightingDolchConjunctions(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetAppOptions()->IsHighlightingDolchConjunctions());
        return 1;
        }

    //-------------------------------------------------------------
    int HighlightDolchPrepositions(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->HighlightDolchPrepositions(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsHighlightingDolchPrepositions(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetAppOptions()->IsHighlightingDolchPrepositions());
        return 1;
        }

    //-------------------------------------------------------------
    int HighlightDolchPronouns(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->HighlightDolchPronouns(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsHighlightingDolchPronouns(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetAppOptions()->IsHighlightingDolchPronouns());
        return 1;
        }

    //-------------------------------------------------------------
    int HighlightDolchAdverbs(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->HighlightDolchAdverbs(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsHighlightingDolchAdverbs(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetAppOptions()->IsHighlightingDolchAdverbs());
        return 1;
        }

    //-------------------------------------------------------------
    int HighlightDolchAdjectives(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->HighlightDolchAdjectives(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsHighlightingDolchAdjectives(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetAppOptions()->IsHighlightingDolchAdjectives());
        return 1;
        }

    //-------------------------------------------------------------
    int HighlightDolchVerbs(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->HighlightDolchVerbs(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsHighlightingDolchVerbs(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetAppOptions()->IsHighlightingDolchVerbs());
        return 1;
        }

    //-------------------------------------------------------------
    int HighlightDolchNouns(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->HighlightDolchNouns(int_to_bool(lua_toboolean(L, 1)));
        return 0;
        }

    //-------------------------------------------------------------
    int IsHighlightingDolchNouns(lua_State* L)
        {
        lua_pushboolean(L, wxGetApp().GetAppOptions()->IsHighlightingDolchNouns());
        return 1;
        }

    //-------------------------------------------------------------
    int SetWindowSize(lua_State* L)
        {
        if (!VerifyParameterCount(L, 2, __func__))
            {
            return 0;
            }
        wxGetApp().GetMainFrame()->Maximize(false);
        wxGetApp().GetMainFrame()->SetSize(
            wxGetApp().GetMainFrame()->FromDIP(luaL_checkinteger(L, 1)),
            wxGetApp().GetMainFrame()->FromDIP(luaL_checkinteger(L, 2)));
        wxGetApp().GetMainFrame()->Center();
        wxGetApp().Yield();
        return 0;
        }

    //-------------------------------------------------------------
    int SetPaperOrientation(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }

        const auto orient = static_cast<Wisteria::Orientation>(luaL_checkinteger(L, 1));
        wxGetApp().GetAppOptions()->SetPaperOrientation(
            (orient == Wisteria::Orientation::Horizontal) ? wxPrintOrientation::wxLANDSCAPE :
                                                            wxPrintOrientation::wxPORTRAIT);
        return 0;
        }

    //-------------------------------------------------------------
    int GetPaperOrientation(lua_State* L)
        {
        const auto orient = wxGetApp().GetAppOptions()->GetPaperOrientation();
        lua_pushnumber(L, static_cast<int>((orient == wxPrintOrientation::wxLANDSCAPE) ?
                                               Wisteria::Orientation::Horizontal :
                                               Wisteria::Orientation::Vertical));
        return 1;
        }

    //-------------------------------------------------------------
    int SetLeftPrintHeader(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->SetLeftPrinterHeader(
            wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        return 0;
        }

    //-------------------------------------------------------------
    int SetCenterPrintHeader(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->SetCenterPrinterHeader(
            wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        return 0;
        }

    //-------------------------------------------------------------
    int SetRightPrintHeader(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->SetRightPrinterHeader(
            wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        return 0;
        }

    //-------------------------------------------------------------
    int SetLeftPrintFooter(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->SetLeftPrinterFooter(
            wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        return 0;
        }

    //-------------------------------------------------------------
    int SetCenterPrintFooter(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->SetCenterPrinterFooter(
            wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        return 0;
        }

    //-------------------------------------------------------------
    int SetRightPrintFooter(lua_State* L)
        {
        if (!VerifyParameterCount(L, 1, __func__))
            {
            return 0;
            }
        wxGetApp().GetAppOptions()->SetRightPrinterFooter(
            wxString{ luaL_checkstring(L, 1), wxConvUTF8 });
        return 0;
        }

    //-------------------------------------------------------------
    int GetLeftPrintHeader(lua_State* L)
        {
        lua_pushstring(L, wxGetApp().GetAppOptions()->GetLeftPrinterHeader().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int GetCenterPrintHeader(lua_State* L)
        {
        lua_pushstring(L, wxGetApp().GetAppOptions()->GetCenterPrinterHeader().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int GetRightPrintHeader(lua_State* L)
        {
        lua_pushstring(L, wxGetApp().GetAppOptions()->GetRightPrinterHeader().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int GetLeftPrintFooter(lua_State* L)
        {
        lua_pushstring(L, wxGetApp().GetAppOptions()->GetLeftPrinterFooter().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int GetCenterPrintFooter(lua_State* L)
        {
        lua_pushstring(L, wxGetApp().GetAppOptions()->GetCenterPrinterFooter().utf8_str());
        return 1;
        }

    //-------------------------------------------------------------
    int GetRightPrintFooter(lua_State* L)
        {
        lua_pushstring(L, wxGetApp().GetAppOptions()->GetRightPrinterFooter().utf8_str());
        return 1;
        }
    } // namespace LuaScripting

// NOLINTEND(readability-implicit-bool-conversion)
// NOLINTEND(readability-identifier-length)
