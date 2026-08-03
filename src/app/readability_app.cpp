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

#include "readability_app.h"
#include "../Wisteria-Dataviz/src/CRCpp/inc/CRC.h"
#include "../Wisteria-Dataviz/src/base/colorbrewer.h"
#include "../Wisteria-Dataviz/src/data/pdfreader.h"
#include "../Wisteria-Dataviz/src/graphs/danielsonbryan2plot.h"
#include "../Wisteria-Dataviz/src/graphs/inflesz.h"
#include "../Wisteria-Dataviz/src/graphs/lixgauge.h"
#include "../Wisteria-Dataviz/src/graphs/lixgaugegerman.h"
#include "../Wisteria-Dataviz/src/import/text_matrix.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/filelistdlg.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/getdirdlg.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/graphdlg.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/listdlg.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/radioboxdlg.h"
#include "../document-helpers/chapter_split.h"
#include "../graphs/schwartzgraph.h"
#include "../projects/batch_project_doc.h"
#include "../projects/batch_project_view.h"
#include "../projects/standard_project_doc.h"
#include "../projects/standard_project_view.h"
#include "../results-format/project_report_format.h"
#include "../ui/controls/script_workbench_panel.h"
#include "../ui/dialogs/custom_test_dlg.h"
#include "../ui/dialogs/edit_word_list_dlg.h"
#include "../ui/dialogs/new_custom_test_simple_dlg.h"
#include "../ui/dialogs/project_wizard_dlg.h"
#include "../ui/dialogs/test_bundle_dlg.h"
#include "../ui/dialogs/tools_options_dlg.h"
#include <algorithm>
#include <utility>
#include <wx/dir.h>
#include <wx/richmsgdlg.h>
#include <wx/webview.h>

// ===========================================================================
// implementation
// ===========================================================================
// NOLINTNEXTLINE(cppcoreguidelines-pro-type-static-cast-downcast,cppcoreguidelines-avoid-non-const-global-variables)
wxIMPLEMENT_APP(ReadabilityApp);

std::map<wxWindowID, wxWindowID> ReadabilityApp::m_dynamicIdMap;

RSArtProvider::RSArtProvider()
    {
    // cppcheck-suppress useInitializationList
    m_idFileMap = { { wxART_FILE_OPEN, L"ribbon/file-open.svg" },
                    { wxART_FILE_SAVE, L"ribbon/file-save.svg" },
                    { wxART_NEW, L"ribbon/document.svg" },
                    { wxART_PRINT, L"ribbon/print.svg" },
                    { wxART_CLOSE, L"ribbon/delete.svg" },
                    { wxART_DELETE, L"ribbon/delete.svg" },
                    { wxART_COPY, L"ribbon/copy.svg" },
                    { wxART_CUT, L"ribbon/cut.svg" },
                    { wxART_PASTE, L"ribbon/paste.svg" },
                    { wxART_UNDO, L"ribbon/undo.svg" },
                    { wxART_REDO, L"ribbon/redo.svg" },
                    { wxART_HELP_BOOK, L"ribbon/help.svg" },
                    { wxART_GO_DOWN, L"ribbon/down-arrow.svg" },
                    { wxART_GO_UP, L"ribbon/up-arrow.svg" },
                    { wxART_FIND, L"ribbon/find.svg" },
                    { wxART_FIND_AND_REPLACE, L"ribbon/find-replace.svg" },
                    { wxART_QUIT, L"ribbon/stop.svg" },
                    { L"ID_SYSADMIN", L"ribbon/system-admin.svg" },
                    { L"ID_NOTES", L"ribbon/notepad.svg" },
                    { L"ID_E_HELP", L"ribbon/electronic-help.svg" },
                    { L"ID_ALIGN_LEFT", L"ribbon/align-left.svg" },
                    { L"ID_ALIGN_CENTER", L"ribbon/align-center.svg" },
                    { L"ID_ALIGN_RIGHT", L"ribbon/align-right.svg" },
                    { L"ID_ALIGN_JUSTIFIED", L"ribbon/justified.svg" },
                    { L"ID_PARAGRAPH_INDENT", L"ribbon/paragraph-indent.svg" },
                    { L"ID_LINE_SPACING", L"ribbon/line-spacing.svg" },
                    { L"ID_LIST_SORT", L"ribbon/sort.svg" },
                    { L"ID_ADD", L"ribbon/add.svg" },
                    { L"ID_GROUP", L"ribbon/group.svg" },
                    { L"ID_RUN", L"ribbon/run.svg" },
                    { L"ID_CLEAR", L"ribbon/clear.svg" },
                    { L"ID_DOCUMENT", L"ribbon/document.svg" },
                    { L"ID_DOCUMENTS", L"ribbon/documents.svg" },
                    { L"ID_EDIT_DOCUMENT", L"ribbon/edit-document.svg" },
                    { L"ID_FUNCTION", wxSystemSettings::GetAppearance().IsDark() ?
                                          L"ribbon/function-dark.svg" :
                                          L"ribbon/function.svg" },
                    { L"ID_SPREADSHEET", L"ribbon/spreadsheet.svg" },
                    { L"ID_ARCHIVE", L"ribbon/archive.svg" },
                    { L"ID_GEARS", L"ribbon/gears.svg" },
                    { L"ID_WEB_EXPORT", L"ribbon/web-export.svg" },
                    { L"ID_EDIT", L"ribbon/edit.svg" },
                    { L"ID_FONT", L"ribbon/font.svg" },
                    { L"ID_SELECT_ALL", L"ribbon/select-all.svg" },
                    { L"ID_REFRESH", L"ribbon/reload.svg" },
                    { L"ID_REALTIME_UPDATE", L"ribbon/realtime.svg" },
                    { L"ID_WHATS_NEW", L"ribbon/outreach.svg" },
                    { L"ID_LINK", L"ribbon/link.svg" } };
    }

//-------------------------------------------
wxBitmapBundle RSArtProvider::CreateBitmapBundle(const wxArtID& id, const wxArtClient& client,
                                                 const wxSize& size)
    {
    const auto filePath = m_idFileMap.find(id);

    return (filePath != m_idFileMap.cend()) ?
               wxGetApp().GetResourceManager().GetSVG(filePath->second) :
               wxArtProvider::CreateBitmapBundle(id, client, size);
    }

//-------------------------------------------
void ReadabilityApp::EditDictionary(const readability::test_language lang)
    {
    EditWordListDlg editDlg(
        GetParentingWindow(), wxID_ANY,
        (lang == readability::test_language::spanish_test) ? _(L"Custom Spanish Dictionary") :
        (lang == readability::test_language::german_test)  ? _(L"Custom German Dictionary") :
                                                             _(L"Custom English Dictionary"));
    if (lang == readability::test_language::spanish_test)
        {
        editDlg.SetFilePath(m_CustomSpanishDictionaryPath);
        }
    else if (lang == readability::test_language::german_test)
        {
        editDlg.SetFilePath(m_CustomGermanDictionaryPath);
        }
    else
        {
        editDlg.SetFilePath(m_CustomEnglishDictionaryPath);
        }
    editDlg.SetHelpTopic(GetMainFrame()->GetHelpDirectory(), L"online/program-options.html");
    if (editDlg.ShowModal() == wxID_OK)
        {
        // custom dictionary will be written back to with edit words at this point,
        // so just reload it.
        const wxBusyCursor wait;
        // reload the custom dictionary
        if (lang == readability::test_language::spanish_test)
            {
            wxString extraDictionaryText;
            if (wxFile::Exists(m_CustomSpanishDictionaryPath) &&
                Wisteria::TextStream::ReadFile(m_CustomSpanishDictionaryPath, extraDictionaryText))
                {
                BaseProject::known_custom_spanish_spellings.load_words(extraDictionaryText, true,
                                                                       false);
                }
            }
        else if (lang == readability::test_language::german_test)
            {
            wxString extraDictionaryText;
            if (wxFile::Exists(m_CustomGermanDictionaryPath) &&
                Wisteria::TextStream::ReadFile(m_CustomGermanDictionaryPath, extraDictionaryText))
                {
                BaseProject::known_custom_german_spellings.load_words(extraDictionaryText, true,
                                                                      false);
                }
            }
        else
            {
            wxString extraDictionaryText;
            if (wxFile::Exists(m_CustomEnglishDictionaryPath) &&
                Wisteria::TextStream::ReadFile(m_CustomEnglishDictionaryPath, extraDictionaryText))
                {
                BaseProject::known_custom_english_spellings.load_words(extraDictionaryText, true,
                                                                       false);
                }
            }

        // reload the projects
        auto& docs = wxGetApp().GetDocManager()->GetDocuments();
        for (size_t i = 0; i < docs.GetCount(); ++i)
            {
            auto* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
            if (doc != nullptr)
                {
                doc->RefreshRequired(ProjectRefresh::FullReindexing);
                doc->RefreshProject();
                }
            }
        }
    }

//-------------------------------------------
void ReadabilityApp::ShowSplashscreen()
    {
    if (m_splashscreenImagePaths.GetCount() > 0 &&
        // holding down shift will prevent the splashscreen from showing
        !wxGetMouseState().ShiftDown() &&
        // if there are command lines then don't show the splashscreen,
        // an error message may appear under it and lock the program
        argc < 2)
        {
        std::uniform_int_distribution<size_t> randNum(0, GetSplashscreenPaths().GetCount() - 1);
        const size_t imageIndex = randNum(GetRandomNumberEngine());
        if (imageIndex < GetSplashscreenPaths().GetCount())
            {
            wxString ext{ GetSplashscreenPaths()[imageIndex] };
            wxBitmap bitmap =
                GetScaledImage(GetSplashscreenPaths()[imageIndex],
                               Wisteria::GraphItems::Image::GetImageFileTypeFromExtension(ext),
                               wxSize{ 800, 600 });
            if (bitmap.IsOk())
                {
                // quneiform-suppress-begin
                // (the concatenation here is OK, so ignore that)
                bitmap =
                    CreateSplashscreen(bitmap, GetAppName(), GetAppSubName(), GetVendorName(), true,
                                       // TRANSLATORS: "©2006-2025" should not be edited.
                                       _(L"Copyright ©2006-2025 Oleander Software, Ltd., "));
                // quneiform-suppress-end

                [[maybe_unused]]
                auto* splash =
                    new wxSplashScreen(bitmap, wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT, 2000,
                                       GetMainFrame(), -1, wxDefaultPosition, wxDefaultSize,
                                       wxFRAME_NO_TASKBAR | wxSTAY_ON_TOP);
                }
            }
        }
    Yield();
    }

//-------------------------------------------
void ReadabilityApp::AddWordsToDictionaries(const wxArrayString& theWords,
                                            const readability::test_language lang) const
    {
    wxString customDictionaryPath = m_CustomEnglishDictionaryPath;
    if (lang == readability::test_language::english_test)
        {
        // add words to the currently loaded (custom) dictionary
        for (size_t i = 0; i < theWords.GetCount(); ++i)
            {
            BaseProject::known_custom_english_spellings.add_word(theWords[i].wc_str());
            }
        }
    else if (lang == readability::test_language::spanish_test)
        {
        customDictionaryPath = m_CustomSpanishDictionaryPath;
        // add words to the currently loaded (custom) dictionary
        for (size_t i = 0; i < theWords.GetCount(); ++i)
            {
            BaseProject::known_custom_spanish_spellings.add_word(theWords[i].wc_str());
            }
        }
    else if (lang == readability::test_language::german_test)
        {
        customDictionaryPath = m_CustomGermanDictionaryPath;
        // add words to the currently loaded (custom) dictionary
        for (size_t i = 0; i < theWords.GetCount(); ++i)
            {
            BaseProject::known_custom_german_spellings.add_word(theWords[i].wc_str());
            }
        }

    // add to custom dictionary
    wxString text;
    if (!Wisteria::TextStream::ReadFile(customDictionaryPath, text))
        {
        wxMessageBox(_(L"Error loading custom dictionary file."), _(L"Error"),
                     wxOK | wxICON_EXCLAMATION);
        return;
        }

    word_list customWords;
    customWords.load_words(text, true, false);
    for (size_t i = 0; i < theWords.GetCount(); ++i)
        {
        customWords.add_word(theWords[i].wc_str());
        }
    // save the new list back to the original file
    wxString outputStr;
    outputStr.reserve(customWords.get_list_size() * 5);
    for (size_t i = 0; i < customWords.get_list_size(); ++i)
        {
        outputStr += customWords.get_words().at(i).c_str() + wxString(L"\n");
        }
    outputStr.Trim();
    wxFileName(customDictionaryPath).SetPermissions(wxS_DEFAULT);
    wxFile outputFile(customDictionaryPath, wxFile::write);
    outputFile.Write(outputStr, wxConvUTF8);
    }

//-------------------------------------------
bool ReadabilityApp::OnInit()
    {
    SetAppName(_READSTUDIO_APP_NAME);
    SetAppSubName(GetAppVersion());
    SetVendorName(_READSTUDIO_PUBLISHER);

    wxString appSettingFolderPath;
    // if app-specific data folder can't be determined
    // (really just relic behavior from Win9.x) then use documents dir
    if (wxStandardPaths::Get().GetUserDataDir().empty())
        {
        appSettingFolderPath =
            wxStandardPaths::Get().GetAppDocumentsDir() + wxFileName::GetPathSeparator();
        }
    // write to app folder in User's data folder (this should be the norm)
    else
        {
        appSettingFolderPath =
            wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator();
        }

    if (!wxFileName::DirExists(appSettingFolderPath))
        {
        if (!wxFileName::Mkdir(appSettingFolderPath, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            /* desperation move that should never happen--
               just save the settings to the user's config folder */
            appSettingFolderPath =
                wxStandardPaths::Get().GetUserConfigDir() + wxFileName::GetPathSeparator();
            }
        }
#ifdef __WXMSW__
    MSWEnableDarkMode();
#endif
    m_preInitOptions.LoadOptionsFile(appSettingFolderPath + L"Settings.xml");
    AppendDailyLog(m_preInitOptions.m_logAppendDailyLog);

    if (!BaseApp::OnInit())
        {
        return false;
        }

    // wait to call this after BaseApp switches to a file logger above
    wxLogMessage(L"Settings file loaded from: " + appSettingFolderPath + L"Settings.xml");

#ifndef NDEBUG
    wxFileTranslationsLoader::AddCatalogLookupPathPrefix(L".");
#endif
    wxLogVerbose(L"UI Language: %d", static_cast<int>(m_preInitOptions.m_uiLanguage));
    // if English, then just don't bother loading any translations
    if (m_preInitOptions.m_uiLanguage != UiLanguage::English)
        {
        auto* const translations{ new wxTranslations{} };
        if (m_preInitOptions.m_uiLanguage == UiLanguage::Spanish)
            {
            translations->SetLanguage(wxLANGUAGE_SPANISH);
            }
        if (!translations->AddCatalog(_READSTUDIO_BINARY_NAME))
            {
            wxLogDebug(L"Could not find application's translation catalog.");
            }
        if (!translations->AddStdCatalog())
            {
            wxLogDebug(L"Could not find standard translation catalog.");
            }
        wxTranslations::Set(translations);
        }

    GetResourceManager().LoadArchive(FindResourceFile(L"res.wad"));

    const auto splashScreens = GetResourceManager().GetFilesInFolder(_DT(L"splash"));
    for (const auto& splash : splashScreens)
        {
        AddSplashscreenImagePath(splash);
        }

    if (!LoadWordLists(appSettingFolderPath))
        {
        return false;
        }

    const wxString pdfTablesDir = FindResourceDirectory(_DT(L"pdf-tables"));
    if (!Wisteria::Data::PdfReader::LoadGlyphNameTableFromFile(
            pdfTablesDir + wxFileName::GetPathSeparator() + L"glyphlist.txt"))
        {
        wxLogDebug(L"Unable to load PDF glyph name table.");
        }
    const std::vector<std::pair<wxString, wxString>> pdfCidTables = {
        { _DT(L"Adobe-Japan1"), _DT(L"UniJIS-UTF16-H") },
        { _DT(L"Adobe-GB1"), _DT(L"UniGB-UTF16-H") },
        { _DT(L"Adobe-CNS1"), _DT(L"UniCNS-UTF16-H") },
        { _DT(L"Adobe-Korea1"), _DT(L"UniKS-UTF16-H") },
        { _DT(L"Adobe-KR"), _DT(L"UniAKR-UTF16-H") }
    };
    for (const auto& [registryOrdering, fileName] : pdfCidTables)
        {
        if (!Wisteria::Data::PdfReader::LoadCidToUnicodeTableFromFile(
                registryOrdering, pdfTablesDir + wxFileName::GetPathSeparator() + fileName))
            {
            wxLogDebug(L"Unable to load PDF CID-to-Unicode table for %s.", registryOrdering);
            }
        }

    // init random number generators
    m_mersenneTwister = std::mt19937_64(std::random_device{}());

    m_webHarvester.SetEventHandler(this);

    if (wxLog::GetVerbose())
        {
        wxLog::SetLogLevel(wxLOG_Max);
        }

    LoadInterface();
    ShowSplashscreen();
    LoadMenus();

    // load map of graph icons to human-readable strings
    m_shapeMap = { std::make_pair(_(L"Sun"), DONTTRANSLATE(L"sun")),
                   std::make_pair(_(L"Book"), DONTTRANSLATE(L"book")),
                   std::make_pair(_(L"Fall leaf"), DONTTRANSLATE(L"fall-leaf")),
                   std::make_pair(_(L"Man"), DONTTRANSLATE(L"man")),
                   std::make_pair(_(L"Woman"), DONTTRANSLATE(L"woman")),
                   std::make_pair(_(L"Business woman"), DONTTRANSLATE(L"business-woman")),
                   std::make_pair(_(L"Tire"), DONTTRANSLATE(L"tire")),
                   std::make_pair(_(L"Flower"), DONTTRANSLATE(L"flower")),
                   std::make_pair(_(L"Sunflower"), DONTTRANSLATE(L"sunflower")),
                   std::make_pair(_(L"Warning road sign"), DONTTRANSLATE(L"warning-road-sign")),
                   std::make_pair(_(L"Location marker"), DONTTRANSLATE(L"location-marker")),
                   std::make_pair(_(L"Graduation cap"), DONTTRANSLATE(L"graduation-cap")),
                   std::make_pair(_(L"Car"), DONTTRANSLATE(L"car")),
                   std::make_pair(_(L"Newspaper"), DONTTRANSLATE(L"newspaper")),
                   std::make_pair(_(L"Snowflake"), DONTTRANSLATE(L"snowflake")),
                   std::make_pair(_(L"Blackboard"), DONTTRANSLATE(L"blackboard")),
                   std::make_pair(_(L"Clock"), DONTTRANSLATE(L"clock")),
                   std::make_pair(_(L"Ruler"), DONTTRANSLATE(L"ruler")),
                   std::make_pair(_(L"Curving road"), DONTTRANSLATE(L"curving-road")),
                   // TRANSLATORS: "IV" is intravenous.
                   std::make_pair(_(L"IV bag"), DONTTRANSLATE(L"ivbag")),
                   std::make_pair(_(L"Thermometer (cold)"), DONTTRANSLATE(L"cold-thermometer")),
                   std::make_pair(_(L"Thermometer (hot)"), DONTTRANSLATE(L"hot-thermometer")),
                   std::make_pair(_(L"Apple (red)"), DONTTRANSLATE(L"apple")),
                   std::make_pair(_(L"Apple (Granny Smith)"), DONTTRANSLATE(L"granny-smith-apple")),
                   std::make_pair(_(L"Heart"), DONTTRANSLATE(L"heart")),
                   std::make_pair(_(L"Immaculate Heart"), DONTTRANSLATE(L"immaculate-heart")),
                   std::make_pair(_(L"Immaculate Heart with sword"),
                                  DONTTRANSLATE(L"immaculate-heart-with-sword")),
                   std::make_pair(_(L"Flame"), DONTTRANSLATE(L"flame")),
                   std::make_pair(_(L"Office"), DONTTRANSLATE(L"office")),
                   std::make_pair(_(L"Factory"), DONTTRANSLATE(L"factory")),
                   std::make_pair(_(L"House"), DONTTRANSLATE(L"house")),
                   std::make_pair(_(L"Barn"), DONTTRANSLATE(L"barn")),
                   std::make_pair(_(L"Farm"), DONTTRANSLATE(L"farm")),
                   std::make_pair(_(L"Hundred dollar bill"), DONTTRANSLATE(L"hundred-dollar-bill")),
                   std::make_pair(_(L"Pumpkin"), DONTTRANSLATE(L"pumpkin")),
                   std::make_pair(_(L"Jack-o'-lantern"), DONTTRANSLATE(L"jack-o-lantern")),
                   std::make_pair(_(L"Monitor"), DONTTRANSLATE(L"monitor")),
                   std::make_pair(_(L"Cheese pizza"), DONTTRANSLATE(L"cheese-pizza")),
                   std::make_pair(_(L"Pepperoni pizza"), DONTTRANSLATE(L"pepperoni-pizza")),
                   std::make_pair(_(L"Hawaiian pizza"), DONTTRANSLATE(L"hawaiian-pizza")),
                   std::make_pair(_(L"Chocolate chip cookie"),
                                  DONTTRANSLATE(L"chocolate-chip-cookie")),
                   std::make_pair(_(L"Coffee shop cup"), DONTTRANSLATE(L"coffee-shop-cup")),
                   std::make_pair(_(L"Pill"), DONTTRANSLATE(L"pill")),
                   std::make_pair(_(L"Tractor"), DONTTRANSLATE(L"tractor")),
                   std::make_pair(_(L"Butterfly"), DONTTRANSLATE(L"butterfly")),
                   std::make_pair(_(L"Star"), DONTTRANSLATE(L"star")) };

    // do the same for the color schemes (for the graphs)
    m_colorSchemeMap = { std::make_pair(_(L"Dusk"), DONTTRANSLATE(L"dusk")),
                         std::make_pair(_(L"Earth Tones"), DONTTRANSLATE(L"earthtones")),
                         std::make_pair(_DT(L"1920s"), DONTTRANSLATE(L"decade1920s")),
                         std::make_pair(_DT(L"1940s"), DONTTRANSLATE(L"decade1940s")),
                         std::make_pair(_DT(L"1950s"), DONTTRANSLATE(L"decade1950s")),
                         std::make_pair(_DT(L"1960s"), DONTTRANSLATE(L"decade1960s")),
                         std::make_pair(_DT(L"1970s"), DONTTRANSLATE(L"decade1970s")),
                         std::make_pair(_DT(L"1980s"), DONTTRANSLATE(L"decade1980s")),
                         std::make_pair(_DT(L"1990s"), DONTTRANSLATE(L"decade1990s")),
                         std::make_pair(_DT(L"2000s"), DONTTRANSLATE(L"decade2000s")),
                         std::make_pair(_(L"October"), DONTTRANSLATE(L"october")),
                         std::make_pair(DONTTRANSLATE(L"Slytherin"), DONTTRANSLATE(L"slytherin")),
                         std::make_pair(_(L"Campfire"), DONTTRANSLATE(L"campfire")),
                         std::make_pair(_(L"Coffee Shop"), DONTTRANSLATE(L"coffeeshop")),
                         std::make_pair(_(L"Arctic Chill"), DONTTRANSLATE(L"arcticchill")),
                         std::make_pair(_(L"Back to School"), DONTTRANSLATE(L"backtoschool")),
                         std::make_pair(_(L"Box of Chocolates"), DONTTRANSLATE(L"boxofchocolates")),
                         std::make_pair(_(L"Cosmopolitan"), DONTTRANSLATE(L"cosmopolitan")),
                         std::make_pair(_(L"Day and Night"), DONTTRANSLATE(L"dayandnight")),
                         std::make_pair(_(L"Fresh Flowers"), DONTTRANSLATE(L"freshflowers")),
                         std::make_pair(_(L"Ice Cream"), DONTTRANSLATE(L"icecream")),
                         std::make_pair(_(L"Urban Oasis"), DONTTRANSLATE(L"urbanoasis")),
                         std::make_pair(_(L"Typewriter"), DONTTRANSLATE(L"typewriter")),
                         // TRANSLATORS: surfing waves. This is a 1980s movie reference that gets
                         // lost in translation.
                         std::make_pair(_(L"Tasty Waves"), DONTTRANSLATE(L"tastywaves")),
                         std::make_pair(_(L"Spring"), DONTTRANSLATE(L"spring")),
                         std::make_pair(_(L"Shabby Chic"), DONTTRANSLATE(L"shabbychic")),
                         std::make_pair(_(L"Rolling Thunder"), DONTTRANSLATE(L"rollingthunder")),
                         std::make_pair(_(L"Produce Section"), DONTTRANSLATE(L"producesection")),
                         std::make_pair(_(L"Nautical"), DONTTRANSLATE(L"nautical")),
                         std::make_pair(_(L"Semesters"), DONTTRANSLATE(L"semesters")),
                         std::make_pair(_(L"Seasons"), DONTTRANSLATE(L"seasons")),
                         std::make_pair(_(L"Meadow Sunset"), DONTTRANSLATE(L"meadowsunset")) };

    m_dynamicIdMap = {
        /* This maps internal (dynamic) IDs to constants in "resources/scripting/rs-constants.lua".
           When adding a new constant to "rs-constants.lua", map that numeric ID
           from there to the respective window ID here.

           This is mostly used for screenshot generation via scripting.

           Note that the key number isn't special, it just needs to be unique from the rest of the
           keys in this map. */
        { 30001, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR },
        { 30002, MainFrame::ID_PROOFING_RIBBON_BUTTON_BAR },
        { 30003, MainFrame::ID_PARAGRAPH_DEDUCTION_RIBBON_BUTTON_BAR },
        { 30004, MainFrame::ID_TEXT_EXCLUSION_RIBBON_BUTTON_BAR },
        { 30005, MainFrame::ID_NUMERALS_RIBBON_BUTTON_BAR },
        { 30006, MainFrame::ID_VIEW_RIBBON_BUTTON_BAR },
        { 30007, MainFrame::ID_CLIPBOARD_RIBBON_BUTTON_BAR },
        { 30008, MainFrame::ID_DOCUMENT_RIBBON_BUTTON_BAR },
        { 30009, MainFrame::ID_PROJECT_RIBBON_BUTTON_BAR },
        { 1000, ToolsOptionsDlg::GENERAL_SETTINGS_PAGE },
        { 1001, ToolsOptionsDlg::THEMES_PAGE },
        { 2000, ToolsOptionsDlg::PROJECT_SETTINGS_PAGE },
        { 3000, ToolsOptionsDlg::DOCUMENT_DISPLAY_GENERAL_PAGE },
        { 3002, ToolsOptionsDlg::DOCUMENT_DISPLAY_DOLCH_PAGE },
        { 4000, ToolsOptionsDlg::SCORES_TEST_OPTIONS_PAGE },
        { 4001, ToolsOptionsDlg::SCORES_DISPLAY_PAGE },
        { 5000, ToolsOptionsDlg::ANALYSIS_INDEXING_PAGE },
        { 5001, ToolsOptionsDlg::GRAMMAR_PAGE },
        { 5002, ToolsOptionsDlg::ANALYSIS_STATISTICS_PAGE },
        { 6000, ToolsOptionsDlg::GRAPH_GENERAL_PAGE },
        { 6001, ToolsOptionsDlg::GRAPH_AXIS_PAGE },
        { 6002, ToolsOptionsDlg::GRAPH_TITLES_PAGE },
        { 6003, ToolsOptionsDlg::GRAPH_READABILITY_GRAPHS_PAGE },
        { 6004, ToolsOptionsDlg::GRAPH_BAR_CHART_PAGE },
        { 6005, ToolsOptionsDlg::GRAPH_HISTOGRAM_PAGE },
        { 6006, ToolsOptionsDlg::GRAPH_BOX_PLOT_PAGE },
        { 1036, ToolsOptionsDlg::ID_TEXT_EXCLUDE_METHOD },
        { 1040, ToolsOptionsDlg::ID_SENTENCE_LONGER_THAN_BUTTON },
        { 1041, ToolsOptionsDlg::ID_SENTENCE_OUTLIER_LENGTH_BUTTON },
        { 1042, ToolsOptionsDlg::ID_IGNORE_BLANK_LINES_BUTTON },
        { 1043, ToolsOptionsDlg::ID_IGNORE_INDENTING_BUTTON },
        { 1051, ToolsOptionsDlg::ID_AGGRESSIVE_LIST_DEDUCTION_CHECKBOX },
        { 1052, ToolsOptionsDlg::ID_EXCLUDED_PHRASES_FILE_EDIT_BUTTON },
        { 1054, ToolsOptionsDlg::ID_EXCLUDE_COPYRIGHT_CHECKBOX },
        { 1056, ToolsOptionsDlg::ID_EXCLUDE_FILE_ADDRESS_CHECKBOX },
        { 1058, ToolsOptionsDlg::ID_EXCLUDE_PROPER_NOUNS_CHECKBOX },
        { 1076, ToolsOptionsDlg::ID_INCOMPLETE_SENTENCE_VALID_LABEL_START },
        { 1077, ToolsOptionsDlg::ID_INCOMPLETE_SENTENCE_VALID_LABEL_END },
        { 1078, ToolsOptionsDlg::ID_EXCLUSION_TAG_BLOCK_LABEL },
        { 1079, ToolsOptionsDlg::ID_EXCLUSION_TAG_BLOCK_SELECTION },
        { 1080, ToolsOptionsDlg::ID_ADDITIONAL_FILE_BROWSE_BUTTON },
        { 1083, ToolsOptionsDlg::ID_ADDITIONAL_FILE_FIELD },
        { 1084, ToolsOptionsDlg::ID_INCOMPLETE_SENTENCE_VALID_VALUE_BOX },
        { 1085, ToolsOptionsDlg::ID_DOCUMENT_STORAGE_RADIO_BOX },
        { 1086, ToolsOptionsDlg::ID_SENTENCES_MUST_BE_CAP_BUTTON },
        { 1087, ToolsOptionsDlg::ID_FILE_BROWSE_BUTTON },
        { 1088, ToolsOptionsDlg::ID_DOCUMENT_DESCRIPTION_FIELD },
        { 1089, ToolsOptionsDlg::ID_NUMBER_SYLLABIZE_METHOD },
        { 20002, TestBundleDlg::ID_STANDARD_TEST_PAGE },
        { 7000, CustomTestDlg::ID_TEST_NAME_FIELD },
        { 7001, CustomTestDlg::ID_TEST_TYPE_COMBO },
        { 7002, CustomTestDlg::ID_FORMULA_FIELD },
        { 7003, CustomTestDlg::ID_WORD_LIST_PROPERTY_GRID },
        { 7004, CustomTestDlg::ID_PROPER_NOUN_PROPERTY_GRID },
        { 7005, CustomTestDlg::ID_CLASSIFICATION_PROPERTY_GRID },
        { 7006, CustomTestDlg::ID_FUNCTION_BROWSER },
        { 8001, BaseProjectView::SENTENCES_CONJUNCTION_START_LIST_PAGE_ID },
        { 8002, BaseProjectView::SENTENCES_LOWERCASE_START_LIST_PAGE_ID },
        { 8003, BaseProjectView::WORDY_PHRASES_LIST_PAGE_ID },
        { 8004, BaseProjectView::REDUNDANT_PHRASE_LIST_PAGE_ID },
        { 8005, BaseProjectView::CLICHES_LIST_PAGE_ID },
        { 8006, BaseProjectView::DUPLICATES_LIST_PAGE_ID },
        { 8007, BaseProjectView::INCORRECT_ARTICLE_PAGE_ID },
        { 8008, BaseProjectView::PASSIVE_VOICE_PAGE_ID },
        { 8009, BaseProjectView::LONG_SENTENCES_LIST_PAGE_ID },
        { 8010, BaseProjectView::HARD_WORDS_LIST_PAGE_ID },
        { 8011, BaseProjectView::LONG_WORDS_LIST_PAGE_ID },
        { 8012, BaseProjectView::DC_WORDS_LIST_PAGE_ID },
        { 8013, BaseProjectView::SPACHE_WORDS_LIST_PAGE_ID },
        { 8014, BaseProjectView::ALL_WORDS_LIST_PAGE_ID },
        { 8015, BaseProjectView::ALL_WORDS_CONDENSED_LIST_PAGE_ID },
        { 8016, BaseProjectView::PROPER_NOUNS_LIST_PAGE_ID },
        { 8017, BaseProjectView::CONTRACTIONS_LIST_PAGE_ID },
        { 8018, BaseProjectView::OVERUSED_WORDS_BY_SENTENCE_LIST_PAGE_ID },
        { 8019, BaseProjectView::HARRIS_JACOBSON_WORDS_LIST_PAGE_ID },
        { 8020, BaseProjectView::MISSPELLED_WORD_LIST_PAGE_ID },
        { 8021, BaseProjectView::WORDING_ERRORS_LIST_PAGE_ID },
        { 8022, BaseProjectView::WORD_BREAKDOWN_PAGE_ID },
        { 8023, BaseProjectView::FRY_PAGE_ID },
        { 8024, BaseProjectView::RAYGOR_PAGE_ID },
        { 8025, BaseProjectView::FRASE_PAGE_ID },
        { 8026, BaseProjectView::SCHWARTZ_PAGE_ID },
        { 8027, BaseProjectView::LIX_GAUGE_PAGE_ID },
        { 8028, BaseProjectView::LIX_GAUGE_GERMAN_PAGE_ID },
        { 8029, BaseProjectView::DB2_PAGE_ID },
        { 8030, BaseProjectView::FLESCH_CHART_PAGE_ID },
        { 8031, BaseProjectView::CRAWFORD_GRAPH_PAGE_ID },
        { 8032, BaseProjectView::DOLCH_COVERAGE_CHART_PAGE_ID },
        { 8033, BaseProjectView::DOLCH_BREAKDOWN_PAGE_ID },
        { 8034, BaseProjectView::GPM_FRY_PAGE_ID },
        { 8035, BaseProjectView::SENTENCE_BOX_PLOT_PAGE_ID },
        { 8036, BaseProjectView::SENTENCE_HISTOGRAM_PAGE_ID },
        { 8037, BaseProjectView::SYLLABLE_HISTOGRAM_PAGE_ID },
        { 8038, BaseProjectView::SENTENCE_HEATMAP_PAGE_ID },
        { 8039, BaseProjectView::WORD_CLOUD_PAGE_ID },
        { 8040, BaseProjectView::ID_SCORE_LIST_PAGE_ID },
        { 8041, BaseProjectView::ID_DIFFICULT_WORDS_LIST_PAGE_ID },
        { 8042, BaseProjectView::ID_DOLCH_COVERAGE_LIST_PAGE_ID },
        { 8043, BaseProjectView::ID_DOLCH_WORDS_LIST_PAGE_ID },
        { 8044, BaseProjectView::ID_NON_DOLCH_WORDS_LIST_PAGE_ID },
        { 8045, BaseProjectView::ID_WARNING_LIST_PAGE_ID },
        { 8046, BaseProjectView::ID_SCORE_STATS_LIST_PAGE_ID },
        { 8047, BaseProjectView::ID_AGGREGATED_DOC_SCORES_LIST_PAGE_ID },
        { 8048, BaseProjectView::ID_AGGREGATED_CLOZE_SCORES_LIST_PAGE_ID },
        { 8049, BaseProjectView::DOLCH_WORDS_LIST_PAGE_ID },
        { 8050, BaseProjectView::NON_DOLCH_WORDS_LIST_PAGE_ID },
        { 8051, BaseProjectView::UNUSED_DOLCH_WORDS_LIST_PAGE_ID },
        { 8052, BaseProjectView::DOLCH_WORDS_TEXT_PAGE_ID },
        { 8053, BaseProjectView::NON_DOLCH_WORDS_TEXT_PAGE_ID },
        { 8054, BaseProjectView::DOLCH_STATS_PAGE_ID },
        { 8055, BaseProjectView::STATS_REPORT_PAGE_ID },
        { 8056, BaseProjectView::STATS_LIST_PAGE_ID },
        { 8057, BaseProjectView::READABILITY_SCORES_PAGE_ID },
        { 8058, BaseProjectView::READABILITY_SCORES_SUMMARY_REPORT_PAGE_ID },
        { 8060, BaseProjectView::READABILITY_GOALS_PAGE_ID },
        { 8061, BaseProjectView::LONG_WORDS_TEXT_PAGE_ID },
        { 8062, BaseProjectView::DC_WORDS_TEXT_PAGE_ID },
        { 8063, BaseProjectView::SPACHE_WORDS_TEXT_PAGE_ID },
        { 8064, BaseProjectView::HARRIS_JACOBSON_WORDS_TEXT_PAGE_ID },
        { 8065, BaseProjectView::HARD_WORDS_TEXT_PAGE_ID },
        { 8066, BaseProjectView::LONG_SENTENCES_AND_WORDINESS_TEXT_PAGE_ID },
        { 8067, BaseProjectView::SIDEBAR_WORDS_BREAKDOWN_SECTION_ID },
        { 8068, BaseProjectView::SIDEBAR_READABILITY_SCORES_SECTION_ID },
        { 8069, BaseProjectView::SIDEBAR_GRAMMAR_SECTION_ID },
        { 8070, BaseProjectView::SIDEBAR_DOLCH_SECTION_ID },
        { 8071, BaseProjectView::SIDEBAR_BOXPLOTS_SECTION_ID },
        { 8072, BaseProjectView::SIDEBAR_HISTOGRAMS_SECTION_ID },
        { 8073, BaseProjectView::SIDEBAR_STATS_SUMMARY_SECTION_ID },
        { 8074, BaseProjectView::SIDEBAR_SENTENCES_BREAKDOWN_SECTION_ID },
        { 8075, BaseProjectView::SYLLABLE_PIECHART_PAGE_ID },
        { 8076, BaseProjectView::INFLESZ_GRAPH_PAGE_ID },
        { 9000, EditWordListDlg::ID_FILE_PATH_FIELD },
        { 10001, ProjectWizardDlg::ID_FILE_BROWSE_BUTTON },
        { 10002, ProjectWizardDlg::ID_FROM_FILE_BUTTON },
        { 10003, ProjectWizardDlg::ID_MANUALLY_ENTERED_TEXT_BUTTON },
        { 10004, ProjectWizardDlg::NARRATIVE_WITH_ILLUSTRATIONS_LINK_ID },
        { 10005, ProjectWizardDlg::FRAGMENTED_LINK_ID },
        { 10006, ProjectWizardDlg::CENTERED_TEXT_LINK_ID },
        { 10007, ProjectWizardDlg::TEST_SELECT_METHOD_BUTTON },
        { 10008, ProjectWizardDlg::LANGUAGE_BUTTON },
        { 10009, ProjectWizardDlg::ID_BATCH_FOLDER_BROWSE_BUTTON },
        { 10010, ProjectWizardDlg::ID_BATCH_FILE_BROWSE_BUTTON },
        { 10011, ProjectWizardDlg::ID_RANDOM_SAMPLE_CHECK },
        { 10012, ProjectWizardDlg::ID_RANDOM_SAMPLE_SPIN },
        { 10013, ProjectWizardDlg::ID_WEB_PAGES_BROWSE_BUTTON },
        { 10014, ProjectWizardDlg::ID_ARCHIVE_FILE_BROWSE_BUTTON },
        { 10015, ProjectWizardDlg::ID_SPREADSHEET_FILE_BROWSE_BUTTON },
        { 10016, ProjectWizardDlg::NEWLINES_ALWAYS_NEW_PARAGRAPH_LINK_ID },
        { 10017, ProjectWizardDlg::ID_WEB_PAGE_BROWSE_BUTTON },
        { 10018, ProjectWizardDlg::ID_ADD_FILE_BUTTON },
        { 10019, ProjectWizardDlg::ID_DELETE_FILE_BUTTON },
        { 10020, ProjectWizardDlg::ID_HARD_RETURN_CHECKBOX },
        { 10021, ProjectWizardDlg::ID_HARD_RETURN_LABEL },
        { 10022, ProjectWizardDlg::ID_NONNARRATIVE_RADIO_BUTTON },
        { 10023, ProjectWizardDlg::ID_NONNARRATIVE_LABEL },
        { 10024, ProjectWizardDlg::ID_NARRATIVE_RADIO_BUTTON },
        { 10025, ProjectWizardDlg::ID_NARRATIVE_LABEL },
        { 10026, ProjectWizardDlg::ID_SENTENCES_SPLIT_RADIO_BUTTON },
        { 10027, ProjectWizardDlg::ID_SENTENCES_SPLIT_LABEL },
        { 10028, ProjectWizardDlg::ID_DOC_TYPE_RADIO_BOX },
        { 10029, ProjectWizardDlg::ID_MIN_WORDS_LABEL },
        { 10030, ProjectWizardDlg::ID_TEST_BUNDLE_RADIO_BOX },
        { 10031, ProjectWizardDlg::ID_INDUSTRY_RADIO_BOX },
        { 10032, ProjectWizardDlg::ID_COMPOSITION_BOX },
        { 10033, ProjectWizardDlg::ID_LAYOUT_BOX },
    };

    SetAppFileExtension(_DT(L"rsp"));
    SetDocumentTypeName(_DT(L"ReadabilityStudio.Document"));
    SetDocumentVersionNumber(_READSTUDIO_PROGRAM_NUMBERED_VERSION);

    // create the document template
    [[maybe_unused]]
    auto* docTemplate =
        new wxDocTemplate(GetDocManager(), _(L"Standard project"), _DT(L"*.rsp"), wxString{},
                          GetAppFileExtension(), GetAppFileExtension() + _DT(L" Doc"), _DT(L"View"),
                          wxCLASSINFO(ProjectDoc), wxCLASSINFO(ProjectView));

    [[maybe_unused]]
    auto* batchDocTemplate =
        new wxDocTemplate(GetDocManager(), _(L"Batch project"), _DT(L"*.rsbp"), wxString{},
                          _DT(L"rsbp"), _DT(L"rsbp Doc"), _DT(L"View"),
                          wxCLASSINFO(BatchProjectDoc), wxCLASSINFO(BatchProjectView));

    const wxArrayString extensions{ GetAppFileExtension(), wxString{ _DT(L"rsbp") } };
    GetMainFrame()->SetDefaultFileExtensions(extensions);

    // printer options
    GetMainFrame()->GetDocumentManager()->GetPageSetupDialogData().GetPrintData().SetPaperId(
        GetAppOptions()->GetPaperId());
    GetMainFrame()->GetDocumentManager()->GetPageSetupDialogData().GetPrintData().SetOrientation(
        GetAppOptions()->GetPaperOrientation());
    GetMainFrame()->GetDocumentManager()->GetPageSetupDialogData().EnableMargins(false);

    // set the help
    GetMainFrame()->SetHelpDirectory(FindResourceDirectory(L"readability-studio-manual"));
    wxLogMessage(L"Documentation Location: %s", GetMainFrame()->GetHelpDirectory());

    // load the full set of user settings
    GetAppOptions()->LoadOptionsFile(appSettingFolderPath + L"Settings.xml", false);

    // theme the exported header row of list controls to match the report theme's
    // banner color (the secondary accent, as used for the summary report's banner heading)
    Wisteria::UI::ListCtrlEx::SetExportHeaderBackgroundColour(
        ProjectReportFormat::GetThemeBannerColour(GetAppOptions()->GetReportTheme()));

    // this must be done before any report web views are created
    if (GetAppOptions()->IsGpuAccelerationDisabled())
        {
        wxWebViewConfiguration::DisableGPUAcceleration();
        }

    // sync log ribbon button states now that options are fully loaded
    GetMainFrameEx()->SetLogAutoRefresh(GetAppOptions()->IsLogAutoRefresh());
    // sync tab visibility
    GetMainFrameEx()->GetRibbon()->ShowPage(
        GetMainFrameEx()->GetRibbon()->GetPageNumber(GetMainFrameEx()->GetLogRibbonPage()),
        GetAppOptions()->IsShowingLogTab());
    GetMainFrameEx()->GetRibbon()->ShowPage(
        GetMainFrameEx()->GetRibbon()->GetPageNumber(GetMainFrameEx()->GetDeveloperRibbonPage()),
        GetAppOptions()->IsShowingDeveloperTab());
    GetMainFrameEx()->GetRibbon()->Realize();

    // initialize Lua interpreter (after options are loaded so it can check unsafe mode setting)
    m_LuaRunner.Initialize();

    // clang-format off
    // add some standard test bundles
    // PSK
    TestBundle pskBundle(ReadabilityMessages::GetPskBundleName().wc_str());
    pskBundle.GetTestGoals().insert(TestGoal{ ReadabilityMessages::PSK_DALE_CHALL().wc_str() });
    pskBundle.GetTestGoals().insert(TestGoal{ ReadabilityMessages::PSK_FLESCH().wc_str() });
    pskBundle.GetTestGoals().insert(TestGoal{ ReadabilityMessages::PSK_GUNNING_FOG().wc_str() });
    pskBundle.GetTestGoals().insert(TestGoal{ ReadabilityMessages::PSK_FARR_JENKINS_PATERSON().wc_str() });
    pskBundle.SetDescription(
        _(L"Powers, Sumner, and Kearl's four adjusted formulas, which were recalculated "
           "using the McCall-Crabbs 1950 tests. These formulas were also adjusted to predict "
           "closer scores to each other.").wc_str());
    pskBundle.SetLanguage(readability::test_language::english_test);
    pskBundle.Lock();
    BaseProject::m_testBundles.insert(pskBundle);
    if (auto* mainFrame = dynamic_cast<MainFrame*>(GetMainFrame()))
        {
        mainFrame->AddTestBundleToMenus(pskBundle.GetName().c_str());
        }

    // Kincaid's Navy Personnel tests
    TestBundle navyBundle(ReadabilityMessages::GetKincaidNavyBundleName().wc_str());
    navyBundle.GetTestGoals().insert(TestGoal{ ReadabilityMessages::FLESCH_KINCAID().wc_str() });
    navyBundle.GetTestGoals().insert(TestGoal{ ReadabilityMessages::NEW_ARI().wc_str() });
    navyBundle.GetTestGoals().insert(TestGoal{ ReadabilityMessages::NEW_FOG().wc_str() });
    navyBundle.SetDescription(
        _(L"Kincaid's collection of recalculated tests, "
           "designed for enlisted U.S. Navy Personnel.").wc_str());
    navyBundle.SetLanguage(readability::test_language::english_test);
    navyBundle.Lock();
    BaseProject::m_testBundles.insert(navyBundle);
    if (auto* mainFrame = dynamic_cast<MainFrame*>(GetMainFrame()))
        {
        mainFrame->AddTestBundleToMenus(navyBundle.GetName().c_str());
        }

    // Grundner's Consent Forms
    TestBundle consentFormsBundle(ReadabilityMessages::GetConsentFormsBundleName().wc_str());
    consentFormsBundle.GetTestGoals() =
        {
        TestGoal{ ReadabilityMessages::ELF().wc_str(), std::numeric_limits<double>::quiet_NaN(), 12 },
        // intersection of 4.5 (y-axis) and 150 (x- axis)
        TestGoal{ ReadabilityMessages::FRY().wc_str(), std::numeric_limits<double>::quiet_NaN(), 10 },
        // Academic, Scientific, or Quality are too difficult (i.e., below 60)
        TestGoal{ ReadabilityMessages::FLESCH().wc_str(), 60, std::numeric_limits<double>::quiet_NaN() },
        // no constraints recommended
        TestGoal{ ReadabilityMessages::SMOG().wc_str() }
        };
    consentFormsBundle.GetStatGoals() =
        {
        // specific Fry statistic constraints
        { _DT(L"sentences-per-100-words"), 4.5, std::numeric_limits<double>::quiet_NaN() },
        { _DT(L"syllables-per-100-words"), std::numeric_limits<double>::quiet_NaN(), 150 }
        };
    consentFormsBundle.SetDescription(
        // TRANSLATORS: "Flesch Reading Ease," "Easy Listening Formula," and
        // "Consent Forms" should not be translated.
        // These are test names and the name of a published article, respectively.
        _(L"Grundner's recommendations for patient consent forms.\n\n"
           "This bundle includes the tests Fry, Flesch Reading Ease, SMOG, and "
           "the Easy Listening Formula, as recommended by T. M. Grundner (\"Consent Forms\" 9-10). "
           "Also included are the recommended test scores (i.e., goals) that consent forms "
           "should fall within. (Note that SMOG did not have a recommended test score.)").wc_str());
    consentFormsBundle.SetLanguage(readability::test_language::english_test);
    consentFormsBundle.Lock();
    BaseProject::m_testBundles.insert(consentFormsBundle);
    if (auto* mainFrame = dynamic_cast<MainFrame*>(GetMainFrame()))
        {
        mainFrame->AddTestBundleToMenus(consentFormsBundle.GetName().c_str());
        }
    // clang-format on

    // See if ClearType is turned on. If not, then graphs will look awful,
    // so ask user about turning it on.
#ifdef __WXMSW__
    int fontSmoothing{ 0 }, smoothingType{ 0 };
    ::SystemParametersInfo(SPI_GETFONTSMOOTHING, 0, &fontSmoothing, 0);
    ::SystemParametersInfo(SPI_GETFONTSMOOTHINGTYPE, 0, &smoothingType, 0);
    if (fontSmoothing == 0 || smoothingType != FE_FONTSMOOTHINGCLEARTYPE)
        {
        std::vector<WarningMessage>::iterator warningIter =
            WarningManager::GetWarning(_DT(L"clear-type-turned-off"));
        if (warningIter != WarningManager::GetWarnings().end() && warningIter->ShouldBeShown())
            {
            wxRichMessageDialog msg(wxGetApp().GetParentingWindow(), warningIter->GetMessage(),
                                    warningIter->GetTitle(), warningIter->GetFlags());
            msg.ShowCheckBox(_(L"Remember my answer"));
            const int dlgResponse = msg.ShowModal();
            if (warningIter != WarningManager::GetWarnings().end() && msg.IsCheckBoxChecked())
                {
                warningIter->Show(false);
                warningIter->SetPreviousResponse(dlgResponse);
                }
            if (dlgResponse == wxID_YES)
                {
                SystemParametersInfo(SPI_SETFONTSMOOTHING, TRUE, 0,
                                     SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
                SystemParametersInfo(SPI_SETFONTSMOOTHINGTYPE, 0, (PVOID)FE_FONTSMOOTHINGCLEARTYPE,
                                     SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
                }
            }
        else if (warningIter != WarningManager::GetWarnings().end() &&
                 warningIter->GetPreviousResponse() == wxID_YES)
            {
            SystemParametersInfo(SPI_SETFONTSMOOTHING, TRUE, 0,
                                 SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
            SystemParametersInfo(SPI_SETFONTSMOOTHINGTYPE, 0, (PVOID)FE_FONTSMOOTHINGCLEARTYPE,
                                 SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
            }
        }
#endif

    GetAppOptions()->SaveOptionsFile();

    BaseApp::LogSystemInfo();

    if (!m_commandLineFilesToOpen.empty())
        {
        CallAfter(
            [this]()
            {
                for (const auto& file : m_commandLineFilesToOpen)
                    {
                    GetMainFrame()->OpenFile(file);
                    }
            });
        }

    return true;
    }

#ifdef __WXOSX__
//-----------------------------------
void ReadabilityApp::MacOpenFiles(const wxArrayString& files)
    {
    for (const wxString& file : files)
        {
        CallAfter([this, file]() { GetMainFrame()->OpenFile(file); });
        }
    }
#endif

//-----------------------------------
void ReadabilityApp::OnInitCmdLine(wxCmdLineParser& cmdParser)
    {
    // -loglevel / --loglevel
    cmdParser.AddOption(_DT("logging"), _DT("logging"),
                        wxTRANSLATE("Log report (0 = disable, 1 = enable)."), wxCMD_LINE_VAL_NUMBER,
                        wxCMD_LINE_PARAM_OPTIONAL);
    // positional: input file
    cmdParser.AddParam(wxTRANSLATE("Input file"), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
    }

//-----------------------------------
bool ReadabilityApp::OnCmdLineParsed(wxCmdLineParser& cmdParser)
    {
    // enable or disable logging
    long loglevel = 1;
    if (cmdParser.Found(_DT(L"logging"), &loglevel))
        {
        wxLog::EnableLogging(static_cast<bool>(loglevel));
        }

    if (cmdParser.GetParamCount() > 0)
        {
        for (size_t i = 0; i < cmdParser.GetParamCount(); ++i)
            {
            wxFileName fn(cmdParser.GetParam(i));
            fn.Normalize(wxPATH_NORM_LONG | wxPATH_NORM_DOTS | wxPATH_NORM_TILDE |
                         wxPATH_NORM_ABSOLUTE);
            m_commandLineFilesToOpen.push_back(fn.GetFullPath());
            }
        }

    return true;
    }

//-----------------------------------
bool ReadabilityApp::LoadWordLists(const wxString& AppSettingFolderPath)
    {
    // load the Word & phrase files--note that these should be pre-sorted
    wxFile theFile;
    if (!theFile.Open(FindResourceFile(L"words.wad"), wxFile::read))
        {
        wxMessageBox(_(L"Word & phrase file missing or corrupt. Please reinstall."), _(L"Error"),
                     wxOK | wxICON_EXCLAMATION);
        return false;
        }

    std::vector<char> wordyZipFileText(theFile.Length());
    const size_t readSize = theFile.Read(wordyZipFileText.data(), wordyZipFileText.size());
    const Wisteria::ZipCatalog cat(wordyZipFileText.data(), readSize);
    // read in the wordy items
    const std::wstring englishWordyPhraseFileText = cat.ReadTextFile(L"wordy-phrases/english.txt");
    const std::wstring spanishWordyPhraseFileText = cat.ReadTextFile(L"wordy-phrases/spanish.txt");
    const std::wstring germanWordyPhraseFileText = cat.ReadTextFile(L"wordy-phrases/german.txt");
    const std::wstring dcReplacementFileText =
        cat.ReadTextFile(L"wordy-phrases/dale-chall-replacements.txt");
    const std::wstring spacheReplacementFileText =
        cat.ReadTextFile(L"wordy-phrases/spache-replacements.txt");
    const std::wstring harrisJacobsonReplacementFileText =
        cat.ReadTextFile(L"wordy-phrases/harris-jacobson-replacements.txt");
    const std::wstring difficultWordReplacementFileText =
        cat.ReadTextFile(L"wordy-phrases/single-word-replacements-english.txt");
    // proper noun stop word list
    const std::wstring properNounStopList =
        cat.ReadTextFile(L"stop-words/proper-nouns-stoplist.txt");
    // copyright notices
    const std::wstring copyRightNoticePhraseFileText =
        cat.ReadTextFile(L"copyright-notices/notices.txt");
    // citation headers
    const std::wstring citationPhraseFileText = cat.ReadTextFile(L"citation-headers/citations.txt");
    // known proper nouns
    const std::wstring properNounsFileText = cat.ReadTextFile(L"proper-nouns/all.txt");
    const std::wstring personalNounsFileText = cat.ReadTextFile(L"proper-nouns/personal.txt");
    // stop lists
    const std::wstring englishStopList = cat.ReadTextFile(L"stop-words/english.txt");
    const std::wstring spanishStopList = cat.ReadTextFile(L"stop-words/spanish.txt");
    const std::wstring germanStopList = cat.ReadTextFile(L"stop-words/german.txt");
    // known spellings
    const std::wstring englishSpellingsFileText = cat.ReadTextFile(L"dictionaries/english.txt");
    const std::wstring spanishSpellingsFileText = cat.ReadTextFile(L"dictionaries/spanish.txt");
    const std::wstring germanSpellingsFileText = cat.ReadTextFile(L"dictionaries/german.txt");
    const std::wstring programmingSpellingsFileText =
        cat.ReadTextFile(L"programming/all-languages.txt");
    // articles
    const std::wstring aExceptionsFileText = cat.ReadTextFile(_DT(L"articles/a-exceptions.txt"));
    const std::wstring anExceptionsFileText = cat.ReadTextFile(_DT(L"articles/an-exceptions.txt"));
    // read in the abbreviations
    const std::wstring abbreviationsFileText =
        cat.ReadTextFile(_DT(L"abbreviations/abbreviations.txt"));
    const std::wstring nonAbbreviationsFileText =
        cat.ReadTextFile(_DT(L"abbreviations/non-abbreviations.txt"));
    // read in the past-participle exceptions
    const std::wstring pastParticipleExceptionsFileText =
        cat.ReadTextFile(_DT(L"past-participles/exceptions.txt"));
    // read in the DC words
    const std::wstring dcFileText = cat.ReadTextFile(_DT(L"word-lists/new-dale-chall.txt"));
    // read in the DC Catholic supplementary words
    const std::wstring stockerCatholicFileText =
        cat.ReadTextFile(_DT(L"word-lists/stocker-catholic-supplement.txt"));
    // read in the Spache words
    const std::wstring spacheFileText = cat.ReadTextFile(_DT(L"word-lists/revised-spache.txt"));
    // Harris-Jacobson words
    const std::wstring harrisJacobsonFileText =
        cat.ReadTextFile(_DT(L"word-lists/harris-jacobson.txt"));
    // read in the Dolch words
    const std::wstring dolchFileText = cat.ReadTextFile(_DT(L"word-lists/dolch.txt"));

    BaseProject::m_englishWordyPhrases.load_phrases(englishWordyPhraseFileText.c_str(), false,
                                                    false);
    BaseProject::m_spanishWordyPhrases.load_phrases(spanishWordyPhraseFileText.c_str(), false,
                                                    false);
    BaseProject::m_germanWordyPhrases.load_phrases(germanWordyPhraseFileText.c_str(), false, false);

    BaseProject::dale_chall_replacement_list.load_words(dcReplacementFileText.c_str(), false);
    BaseProject::spache_replacement_list.load_words(spacheReplacementFileText.c_str(), false);
    BaseProject::harris_jacobson_replacement_list.load_words(
        harrisJacobsonReplacementFileText.c_str(), false);
    BaseProject::difficult_word_replacement_list.load_words(
        difficultWordReplacementFileText.c_str(), false);

    BaseProject::m_knownProperNouns.load_words(properNounsFileText.c_str(), false, false);
    BaseProject::m_knownPersonalNouns.load_words(personalNounsFileText.c_str(), false, false);
    BaseProject::english_stoplist.load_words(englishStopList.c_str(), false, false);
    BaseProject::spanish_stoplist.load_words(spanishStopList.c_str(), false, false);
    BaseProject::german_stoplist.load_words(germanStopList.c_str(), false, false);

    BaseProject::m_dale_chall_word_list.load_words(dcFileText.c_str(), false, false);
    BaseProject::m_stocker_catholic_word_list.load_words(stockerCatholicFileText.c_str(), false,
                                                         false);
    BaseProject::m_dale_chall_plus_stocker_catholic_word_list.load_words(dcFileText.c_str(), false,
                                                                         false);
    BaseProject::m_dale_chall_plus_stocker_catholic_word_list.load_words(
        stockerCatholicFileText.c_str(), true, true);
    BaseProject::m_spache_word_list.load_words(spacheFileText.c_str(), false, false);
    BaseProject::m_harris_jacobson_word_list.load_words(harrisJacobsonFileText.c_str(), false,
                                                        false);
    BaseProject::m_dolch_word_list.load_words(dolchFileText.c_str());

    BaseProject::known_english_spellings.load_words(englishSpellingsFileText.c_str(), false, false);
    BaseProject::known_programming_spellings.load_words(programmingSpellingsFileText.c_str(), false,
                                                        false);
    m_CustomEnglishDictionaryPath = AppSettingFolderPath + L"DictionaryEN.txt";
    wxString extraDictionaryText;
    if (wxFile::Exists(m_CustomEnglishDictionaryPath) &&
        Wisteria::TextStream::ReadFile(m_CustomEnglishDictionaryPath, extraDictionaryText))
        {
        BaseProject::known_custom_english_spellings.load_words(extraDictionaryText, true, false);
        }
    // if the custom dictionary doesn't exist, then create an empty file
    else if (!wxFile::Exists(m_CustomEnglishDictionaryPath))
        {
        wxFile outputFile(m_CustomEnglishDictionaryPath, wxFile::write);
        outputFile.Write(wxString{}, wxConvUTF8);
        }

    BaseProject::known_spanish_spellings.load_words(spanishSpellingsFileText.c_str(), false, false);
    m_CustomSpanishDictionaryPath = AppSettingFolderPath + L"DictionaryES.txt";
    if (wxFile::Exists(m_CustomSpanishDictionaryPath) &&
        Wisteria::TextStream::ReadFile(m_CustomSpanishDictionaryPath, extraDictionaryText))
        {
        BaseProject::known_custom_spanish_spellings.load_words(extraDictionaryText, true, false);
        }
    // if the custom dictionary doesn't exist, then create an empty file
    else if (!wxFile::Exists(m_CustomSpanishDictionaryPath))
        {
        wxFile outputFile(m_CustomSpanishDictionaryPath, wxFile::write);
        outputFile.Write(wxString{}, wxConvUTF8);
        }

    BaseProject::known_german_spellings.load_words(germanSpellingsFileText.c_str(), false, false);
    m_CustomGermanDictionaryPath = AppSettingFolderPath + L"DictionaryDE.txt";
    if (wxFile::Exists(m_CustomGermanDictionaryPath) &&
        Wisteria::TextStream::ReadFile(m_CustomGermanDictionaryPath, extraDictionaryText))
        {
        BaseProject::known_custom_german_spellings.load_words(extraDictionaryText, true, false);
        }
    // if the custom dictionary doesn't exist, then create an empty file
    else if (!wxFile::Exists(m_CustomGermanDictionaryPath))
        {
        wxFile outputFile(m_CustomGermanDictionaryPath, wxFile::write);
        outputFile.Write(wxString{}, wxConvUTF8);
        }

    // clang-format off
    BaseProject::m_copyrightNoticePhrases.load_phrases(
        copyRightNoticePhraseFileText.c_str(), false, false);
    BaseProject::m_citationPhrases.load_phrases(
        citationPhraseFileText.c_str(), false, false);
    grammar::is_non_proper_word::get_word_list().load_words(
        properNounStopList.c_str(), true, false);
    grammar::is_abbreviation::get_abbreviations().load_words(
        abbreviationsFileText.c_str(), true, false);
    grammar::is_abbreviation::get_non_abbreviations().load_words(
        nonAbbreviationsFileText.c_str(), true, false);
    grammar::is_english_passive_voice::get_past_participle_exceptions().load_words(
        pastParticipleExceptionsFileText.c_str(), true, false);
    grammar::is_incorrect_english_article::get_a_exceptions().load_words(
        aExceptionsFileText.c_str(), true, false);
    grammar::is_incorrect_english_article::get_an_exceptions().load_words(
        anExceptionsFileText.c_str(), true, false);
    // clang-format on

    return true;
    }

//-----------------------------------
bool ReadabilityApp::VerifyWordLists()
    {
    bool retVal = true;
    // verify the dictionaries are sorted
    // English
    if (!BaseProject::known_english_spellings.is_sorted())
        {
        wxLogError(L"English dictionary is not sorted.");
        retVal = false;
        }
    else
        {
        wxLogMessage(L"English dictionary is sorted properly.");
        }
    if (!BaseProject::known_programming_spellings.is_sorted())
        {
        wxLogError(L"Programming dictionary is not sorted.");
        retVal = false;
        }
    else
        {
        wxLogMessage(L"Programming dictionary is sorted properly.");
        }
    if (!BaseProject::known_custom_english_spellings.is_sorted())
        {
        wxLogError(L"Custom English dictionary is not sorted.");
        retVal = false;
        }
    else
        {
        wxLogMessage(L"Custom English dictionary is sorted properly.");
        }

    // Spanish
    if (!BaseProject::known_spanish_spellings.is_sorted())
        {
        wxLogError(L"Spanish dictionary is not sorted.");
        retVal = false;
        }
    else
        {
        wxLogMessage(L"Spanish dictionary is sorted properly.");
        }
    if (!BaseProject::known_custom_spanish_spellings.is_sorted())
        {
        wxLogError(L"Custom Spanish dictionary is not sorted.");
        retVal = false;
        }
    else
        {
        wxLogMessage(L"Custom Spanish dictionary is sorted properly.");
        }

    // German
    if (!BaseProject::known_german_spellings.is_sorted())
        {
        wxLogError(_DT(L"German dictionary is not sorted."));
        retVal = false;
        }
    else
        {
        wxLogMessage(_DT(L"German dictionary is sorted properly."));
        }
    if (!BaseProject::known_custom_german_spellings.is_sorted())
        {
        wxLogError(_DT(L"Custom German dictionary is not sorted."));
        retVal = false;
        }
    else
        {
        wxLogMessage(_DT(L"Custom German dictionary is sorted properly."));
        }

    // Proper nouns list
    if (!BaseProject::m_knownProperNouns.is_sorted())
        {
        wxLogError(_DT(L"Proper nouns are not sorted."));
        retVal = false;
        }
    else
        {
        wxLogMessage(_DT(L"Proper nouns are sorted properly."));
        }
    if (!BaseProject::m_knownPersonalNouns.is_sorted())
        {
        wxLogError(_DT(L"Personal nouns are not sorted."));
        retVal = false;
        }
    else
        {
        wxLogMessage(_DT(L"Personal nouns are sorted properly."));
        }

    // word lists
    if (!BaseProject::m_dale_chall_word_list.is_sorted())
        {
        wxLogError(_DT(L"DC words are not sorted."));
        retVal = false;
        }
    else
        {
        wxLogMessage(_DT(L"DC words are sorted properly."));
        }

    if (!BaseProject::m_dale_chall_plus_stocker_catholic_word_list.is_sorted() ||
        !BaseProject::m_stocker_catholic_word_list.is_sorted())
        {
        wxLogError(_DT(L"Stocker words are not sorted."));
        retVal = false;
        }
    else
        {
        wxLogMessage(_DT(L"Stocker words are sorted properly."));
        }

    if (!BaseProject::m_spache_word_list.is_sorted())
        {
        wxLogError(_DT(L"Spache words are not sorted."));
        retVal = false;
        }
    else
        {
        wxLogMessage(_DT(L"Spache words are sorted properly."));
        }

    if (!BaseProject::m_harris_jacobson_word_list.is_sorted())
        {
        wxLogError(_DT(L"HJ words are not sorted."));
        retVal = false;
        }
    else
        {
        wxLogMessage(_DT(L"HJ words are sorted properly."));
        }

    // the phrases
    if (!BaseProject::m_englishWordyPhrases.is_sorted())
        {
        wxLogError(_DT(L"English phrases are not sorted."));
        retVal = false;
        }
    else
        {
        wxLogMessage(_DT(L"English phrases are sorted properly."));
        }

    if (!BaseProject::m_spanishWordyPhrases.is_sorted())
        {
        wxLogError(_DT(L"Spanish phrases are not sorted."));
        retVal = false;
        }
    else
        {
        wxLogMessage(_DT(L"Spanish phrases are sorted properly."));
        }

    if (!BaseProject::m_germanWordyPhrases.is_sorted())
        {
        wxLogError(_DT(L"German phrases are not sorted."));
        retVal = false;
        }
    else
        {
        wxLogMessage(_DT(L"German phrases are sorted properly."));
        }

    if (!BaseProject::m_copyrightNoticePhrases.is_sorted())
        {
        wxLogError(_DT(L"Copyright notices are not sorted."));
        retVal = false;
        }
    else
        {
        wxLogMessage(_DT(L"Copyright notices are sorted properly."));
        }

    if (!BaseProject::m_citationPhrases.is_sorted())
        {
        wxLogError(_DT(L"Citations are not sorted."));
        retVal = false;
        }
    else
        {
        wxLogMessage(_DT(L"Citations are sorted properly."));
        }

    return retVal;
    }

//-----------------------------------
void ReadabilityApp::LoadMenus()
    {
    FillWordListsMenu(GetMainFrameEx()->m_wordListMenu);
    FillBlankGraphsMenu(GetMainFrameEx()->m_blankGraphMenu);
    }

//-----------------------------------
void ReadabilityApp::LoadInterface()
    {
    wxArtProvider::Push(new RSArtProvider);
    // load the menu resources
#ifdef __WXOSX__
    if (!wxXmlResource::Get()->Load(
            GetResourceManager().GetResourceFilePath(_DT(L"menus/menu.xrc"))))
        {
        wxLogError(L"Menu file is missing or corrupt. Please reinstall.");
        }
#endif
    if (!wxXmlResource::Get()->Load(
            GetResourceManager().GetResourceFilePath(_DT(L"menus/contextmenu.xrc"))))
        {
        wxLogError(L"Context menu file is missing or corrupt. Please reinstall.");
        }

    wxArrayString extensions;
    extensions.Add(GetAppFileExtension());
    // create the main frame window
    SetMainFrame(
        new MainFrame(GetDocManager(), nullptr, extensions, GetAppName(), wxPoint{ 0, 0 },
                      wxSize(m_preInitOptions.m_appWindowWidth, m_preInitOptions.m_appWindowHeight),
                      wxDEFAULT_FRAME_STYLE));

    wxLogMessage(L"Display Scaling Factor: %.2f", GetMainFrame()->GetDPIScaleFactor());
    m_dpiScaleFactor = GetMainFrame()->GetDPIScaleFactor();

    if (m_preInitOptions.m_appWindowMaximized)
        {
        GetMainFrame()->Maximize();
        GetMainFrame()->SetSize(GetMainFrame()->GetSize());
        }
    wxIcon appIcon;
    appIcon.CopyFromBitmap(wxGetApp()
                               .GetResourceManager()
                               .GetSVG(L"ribbon/app-logo.svg")
                               .GetBitmap(GetMainFrame()->FromDIP(wxSize{ 32, 32 })));
    GetMainFrame()->SetIcon(appIcon);
    GetMainFrame()->SetLogo(wxGetApp().GetResourceManager().GetSVG(L"ribbon/app-logo.svg"));

    // now safe to load the full options, which gets colors and font info from the systems
    CreateAppOptions();

    // set up the ribbon (and its submenus)
    GetMainFrame()->InitControls(CreateRibbon(GetMainFrame(), nullptr));

    auto* menuItem = new wxMenuItem(&GetMainFrameEx()->m_fileOpenMenu, wxID_OPEN,
                                    _(L"Open Project...") + _DT(L"\tCtrl+O"));
    menuItem->SetBitmap(GetResourceManager().GetSVG(L"ribbon/file-open.svg"));
    GetMainFrameEx()->m_fileOpenMenu.Append(menuItem);
    GetDocManager()->FileHistoryUseMenu(&GetMainFrameEx()->m_fileOpenMenu);
    if (GetMainFrameEx()->m_fileOpenMenu.FindItem(wxID_FILE1) == nullptr)
        {
        GetDocManager()->FileHistoryAddFilesToMenu(&GetMainFrameEx()->m_fileOpenMenu);
        }
    FillPrintMenu(GetMainFrameEx()->m_printMenu, RibbonType::MainFrameRibbon);
    GetMainFrameEx()->AddExamplesToMenu(&GetMainFrameEx()->m_exampleMenu);
    MainFrame::FillMenuWithCustomTests(&GetMainFrameEx()->m_customTestsMenu, nullptr, false);
    MainFrame::FillMenuWithTestBundles(&GetMainFrameEx()->m_testsBundleMenu, nullptr, false);
    // create the menubar (macOS only)
#ifdef __WXOSX__
    wxMenuBar* menuBar = wxXmlResource::Get()->LoadMenuBar(_DT(L"ID_MENUBAR"));
    wxASSERT_MSG(menuBar, L"Menubar failed to load in OnInit()!");
    if (menuBar != nullptr)
        {
    #ifdef APP_STORE_BUILD
        if (wxMenu * checkForUpdatesMenu{ nullptr };
            menuBar->FindItem(XRCID("ID_CHECK_FOR_UPDATES"), &checkForUpdatesMenu) != nullptr)
            {
            checkForUpdatesMenu->Destroy(XRCID("ID_CHECK_FOR_UPDATES"));
            }
    #endif
        GetMainFrame()->SetMenuBar(menuBar);
        }
    else
        {
        wxLogError(L"Menubar loading failed.");
        }
#endif

    LoadFileHistoryMenu();

    if (GetMainFrame()->GetMenuBar() != nullptr)
        {
        // add examples to the menus
        const wxMenuItem* exampleMenuItem =
            GetMainFrame()->GetMenuBar()->FindItem(XRCID("ID_EXAMPLES"));
        if (exampleMenuItem != nullptr)
            {
            GetMainFrameEx()->AddExamplesToMenu(exampleMenuItem->GetSubMenu());
            }
        // add the custom tests and bundles
        const int readMenuIndex = GetMainFrame()->GetMenuBar()->FindMenu(_(L"Readability"));
        if (readMenuIndex != wxNOT_FOUND)
            {
            auto* readMenu = GetMainFrame()->GetMenuBar()->GetMenu(readMenuIndex);
            if (readMenu != nullptr)
                {
                // custom tests
                GetMainFrameEx()->m_customTestsRegularMenu = new wxMenu;
                readMenu->AppendSubMenu(GetMainFrameEx()->m_customTestsRegularMenu,
                                        _(L"Custom Tests"));
                MainFrame::FillMenuWithCustomTests(GetMainFrameEx()->m_customTestsRegularMenu,
                                                   nullptr, false);
                // add test bundles
                GetMainFrameEx()->m_testsBundleRegularMenu = new wxMenu;
                readMenu->AppendSubMenu(GetMainFrameEx()->m_testsBundleRegularMenu,
                                        _(L"Test Bundles"));
                MainFrame::FillMenuWithTestBundles(GetMainFrameEx()->m_testsBundleRegularMenu,
                                                   nullptr, false);
                }
            }
        }

    InitProjectSidebar();
    InitStartPage();

    // show the interface
    GetMainFrame()->Centre();
    GetMainFrame()->Show();
    GetMainFrame()->Update();
    SetTopWindow(GetMainFrame());
    }

//-----------------------------------
int ReadabilityApp::OnExit()
    {
    wxLogDebug(__func__);
    m_webHarvester.CancelPending();
    GetAppOptions()->SaveOptionsFile();

    return BaseApp::OnExit();
    }

//-----------------------------------
void ReadabilityApp::EditCustomTest(CustomReadabilityTest& selectedTest)
    {
    // make sure there aren't any projects getting updated before we start changing these tests
    const auto& docs = GetDocManager()->GetDocuments();
    for (size_t i = 0; i < docs.GetCount(); ++i)
        {
        const auto* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
        if (doc != nullptr && !doc->IsSafeToUpdate())
            {
            return;
            }
        }

    CustomTestDlg dlg(GetParentingWindow(), wxID_ANY, selectedTest.get_name().c_str());
    dlg.SetStemmingType(selectedTest.get_stemming_type());
    dlg.SetFormula(selectedTest.get_formula().c_str());
    dlg.SetTestType(static_cast<int>(selectedTest.get_test_type()));
    dlg.SetWordListFilePath(selectedTest.get_familiar_word_list_file_path().c_str());
    dlg.SetIncludingCustomWordList(selectedTest.is_including_custom_familiar_word_list());
    dlg.SetIncludingDaleChallList(selectedTest.is_including_dale_chall_list());
    dlg.SetIncludingSpacheList(selectedTest.is_including_spache_list());
    dlg.SetIncludingHJList(selectedTest.is_including_harris_jacobson_list());
    dlg.SetIncludingStockerList(selectedTest.is_including_stocker_list());
    dlg.SetFamiliarWordsMustBeOnEachIncludedList(
        selectedTest.is_familiar_words_must_be_on_each_included_list());
    dlg.SetProperNounMethod(static_cast<int>(selectedTest.get_proper_noun_method()));
    dlg.SetIncludingNumeric(selectedTest.is_including_numeric_as_familiar());
    dlg.SetChildrensPublishingSelected(selectedTest.has_industry_classification(
        readability::industry_classification::childrens_publishing_industry));
    dlg.SetAdultPublishingSelected(selectedTest.has_industry_classification(
        readability::industry_classification::adult_publishing_industry));
    dlg.SetSecondaryLanguageSelected(selectedTest.has_industry_classification(
        readability::industry_classification::secondary_language_industry));
    dlg.SetBroadcastingSelected(selectedTest.has_industry_classification(
        readability::industry_classification::broadcasting_industry));
    dlg.SetChildrensHealthCareTestSelected(selectedTest.has_industry_classification(
        readability::industry_classification::childrens_healthcare_industry));
    dlg.SetAdultHealthCareTestSelected(selectedTest.has_industry_classification(
        readability::industry_classification::adult_healthcare_industry));
    dlg.SetMilitaryTestSelected(selectedTest.has_industry_classification(
        readability::industry_classification::military_government_industry));
    dlg.SetGeneralDocumentSelected(selectedTest.has_document_classification(
        readability::document_classification::general_document));
    dlg.SetTechnicalDocumentSelected(selectedTest.has_document_classification(
        readability::document_classification::technical_document));
    dlg.SetNonNarrativeFormSelected(selectedTest.has_document_classification(
        readability::document_classification::nonnarrative_document));
    dlg.SetYoungAdultAndAdultLiteratureSelected(selectedTest.has_document_classification(
        readability::document_classification::adult_literature_document));
    dlg.SetChildrensLiteratureSelected(selectedTest.has_document_classification(
        readability::document_classification::childrens_literature_document));
    if (dlg.IsIncludingCustomWordList())
        {
        dlg.SelectPage(CustomTestDlg::ID_WORD_LIST_PAGE);
        }
    if (dlg.ShowModal() == wxID_OK)
        {
        const wxBusyCursor wait;

        // get the word-list file text here, in case the path is wrong and
        // the user needs to correct it
        wxString filePath = dlg.GetWordListFilePath();
        wxString fileText;
        // load custom word file if they are using one. If not then just load an
        // empty string into this list
        if (dlg.IsIncludingCustomWordList())
            {
            if (!Wisteria::TextStream::ReadFile(filePath, fileText))
                {
                return;
                }
            }

        // update the test's info now
        selectedTest.set_stemming_type(dlg.GetStemmingType());
        selectedTest.set_formula(dlg.GetFormula().wc_str());
        selectedTest.set_test_type(
            static_cast<readability::readability_test_type>(dlg.GetTestType()));
        selectedTest.set_familiar_word_list_file_path(filePath.wc_str());
        selectedTest.include_custom_familiar_word_list(dlg.IsIncludingCustomWordList());
        selectedTest.include_dale_chall_list(dlg.IsIncludingDaleChallList());
        selectedTest.include_spache_list(dlg.IsIncludingSpacheList());
        selectedTest.include_harris_jacobson_list(dlg.IsIncludingHJList());
        selectedTest.include_stocker_list(dlg.IsIncludingStockerList());
        selectedTest.set_familiar_words_must_be_on_each_included_list(
            dlg.IsFamiliarWordsMustBeOnEachIncludedList());
        selectedTest.set_proper_noun_method(
            static_cast<readability::proper_noun_counting_method>(dlg.GetProperNounMethod()));
        selectedTest.include_numeric_as_familiar(dlg.IsIncludingNumeric());
        selectedTest.add_industry_classification(
            readability::industry_classification::childrens_publishing_industry,
            dlg.IsChildrensPublishingSelected());
        selectedTest.add_industry_classification(
            readability::industry_classification::adult_publishing_industry,
            dlg.IsAdultPublishingSelected());
        selectedTest.add_industry_classification(
            readability::industry_classification::secondary_language_industry,
            dlg.IsSecondaryLanguageSelected());
        selectedTest.add_industry_classification(
            readability::industry_classification::broadcasting_industry,
            dlg.IsBroadcastingSelected());
        selectedTest.add_industry_classification(
            readability::industry_classification::childrens_healthcare_industry,
            dlg.IsChildrensHealthCareTestSelected());
        selectedTest.add_industry_classification(
            readability::industry_classification::adult_healthcare_industry,
            dlg.IsAdultHealthCareTestSelected());
        selectedTest.add_industry_classification(
            readability::industry_classification::military_government_industry,
            dlg.IsMilitaryTestSelected());
        selectedTest.add_document_classification(
            readability::document_classification::general_document,
            dlg.IsGeneralDocumentSelected());
        selectedTest.add_document_classification(
            readability::document_classification::technical_document,
            dlg.IsTechnicalDocumentSelected());
        selectedTest.add_document_classification(
            readability::document_classification::nonnarrative_document,
            dlg.IsNonNarrativeFormSelected());
        selectedTest.add_document_classification(
            readability::document_classification::adult_literature_document,
            dlg.IsYoungAdultAndAdultLiteratureSelected());
        selectedTest.add_document_classification(
            readability::document_classification::childrens_literature_document,
            dlg.IsChildrensLiteratureSelected());

        // reload the word-list file
        selectedTest.load_custom_familiar_words(fileText);

        // reload any projects that have this test in it
        if ((docs.GetCount() > 0) &&
            wxMessageBox(_(L"Do you wish to recalculate any open projects that include this test?"),
                         _(L"Project Update"), wxYES_NO | wxICON_QUESTION) == wxYES)
            {
            for (size_t i = 0; i < docs.GetCount(); ++i)
                {
                auto* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
                if ((doc != nullptr) && doc->HasCustomTest(selectedTest.get_name().c_str()))
                    {
                    doc->Modify(true);
                    // projects will need to do a full re-indexing
                    doc->RefreshRequired(ProjectRefresh::FullReindexing);
                    doc->RefreshProject();
                    }
                }
            }
        }
    }

//-------------------------------------------------------
void ReadabilityApp::FillBlankGraphsMenu(wxMenu& blankGraphsMenu)
    {
    while (blankGraphsMenu.GetMenuItemCount() != 0U)
        {
        blankGraphsMenu.Destroy(blankGraphsMenu.FindItemByPosition(0));
        }

    auto* menuItem = new wxMenuItem(&blankGraphsMenu, XRCID("ID_BLANK_DB2_GRAPH"),
                                    BaseProjectView::GetDB2Label());
    menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"tests/danielson-bryan-2.svg"));
    blankGraphsMenu.Append(menuItem);

    menuItem = new wxMenuItem(&blankGraphsMenu, XRCID("ID_BLANK_FLESCH_GRAPH"),
                              _DT(L"Flesch Reading Ease"));
    menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"tests/flesch-test.svg"));
    blankGraphsMenu.Append(menuItem);

    menuItem = new wxMenuItem(&blankGraphsMenu, XRCID("ID_BLANK_FRY_GRAPH"), _DT(L"Fry"));
    menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"tests/fry-test.svg"));
    blankGraphsMenu.Append(menuItem);

    menuItem = new wxMenuItem(&blankGraphsMenu, XRCID("ID_BLANK_LIX_GRAPH"), _DT(L"Lix Gauge"));
    menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"tests/lix-test.svg"));
    blankGraphsMenu.Append(menuItem);

    menuItem = new wxMenuItem(&blankGraphsMenu, XRCID("ID_BLANK_RAYGOR_GRAPH"), _DT(L"Raygor"));
    menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"tests/raygor-test.svg"));
    blankGraphsMenu.Append(menuItem);

    // Spanish
    blankGraphsMenu.AppendSeparator();

    menuItem = new wxMenuItem(&blankGraphsMenu, XRCID("ID_BLANK_CRAWFORD_GRAPH"), _DT(L"Crawford"));
    menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"tests/crawford.svg"));
    blankGraphsMenu.Append(menuItem);

    menuItem = new wxMenuItem(&blankGraphsMenu, XRCID("ID_BLANK_FRASE_GRAPH"), _DT(L"FRASE"));
    menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"tests/frase.svg"));
    blankGraphsMenu.Append(menuItem);

    menuItem = new wxMenuItem(&blankGraphsMenu, XRCID("ID_BLANK_GPM_GRAPH"),
                              _DT(L"Gilliam-Peña-Mountain"));
    menuItem->SetBitmap(
        wxGetApp().GetResourceManager().GetSVG(L"tests/gilliam-pena-mountain-fry-graph.svg"));
    blankGraphsMenu.Append(menuItem);

    menuItem = new wxMenuItem(&blankGraphsMenu, XRCID("ID_BLANK_INFLESZ_GRAPH"), _DT(L"INFLESZ"));
    menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"tests/inflesz-test.svg"));
    blankGraphsMenu.Append(menuItem);

    // German
    blankGraphsMenu.AppendSeparator();

    menuItem = new wxMenuItem(&blankGraphsMenu, XRCID("ID_BLANK_GERMAN_LIX_GRAPH"),
                              _DT(L"German Lix Gauge"));
    menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"tests/lix-test.svg"));
    blankGraphsMenu.Append(menuItem);

    menuItem = new wxMenuItem(&blankGraphsMenu, XRCID("ID_BLANK_SCHWARTZ_GRAPH"), _DT(L"Schwartz"));
    menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"tests/schwartz.svg"));
    blankGraphsMenu.Append(menuItem);
    }

//-------------------------------------------------------
void ReadabilityApp::FillWordListsMenu(wxMenu& wordListMenu)
    {
    while (wordListMenu.GetMenuItemCount() != 0U)
        {
        wordListMenu.Destroy(wordListMenu.FindItemByPosition(0));
        }
    auto* menuItem =
        new wxMenuItem(&wordListMenu, XRCID("ID_DOLCH_WORD_LIST_WINDOW"), _(L"Dolch Sight Words"));
    menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"tests/dolch.svg"));
    wordListMenu.Append(menuItem);

    menuItem = new wxMenuItem(&wordListMenu, XRCID("ID_HARRIS_JACOBSON_WORD_LIST_WINDOW"),
                              _DT(L"Harris-Jacobson"));
    menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"tests/spache-test.svg"));
    wordListMenu.Append(menuItem);

    menuItem =
        new wxMenuItem(&wordListMenu, XRCID("ID_DC_WORD_LIST_WINDOW"), _DT(L"New Dale-Chall"));
    menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"tests/dale-chall-test.svg"));
    wordListMenu.Append(menuItem);

    menuItem =
        new wxMenuItem(&wordListMenu, XRCID("ID_SPACHE_WORD_LIST_WINDOW"), _DT("Spache Revised"));
    menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"tests/spache-test.svg"));
    wordListMenu.Append(menuItem);

    menuItem = new wxMenuItem(&wordListMenu, XRCID("ID_STOCKER_CATHOLIC_WORD_LIST_WINDOW"),
                              _(L"Stocker's Catholic Supplement"));
    menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"tests/stocker.svg"));
    wordListMenu.Append(menuItem);

    wordListMenu.AppendSeparator();
    menuItem = new wxMenuItem(&wordListMenu, XRCID("ID_EDIT_WORD_LIST"), _(L"Edit Word List..."));
    menuItem->SetBitmap(wxArtProvider::GetBitmapBundle(L"ID_EDIT"));
    wordListMenu.Append(menuItem);
    }

//-------------------------------------------------------
void ReadabilityApp::FillGradeScalesMenu(wxMenu& menu)
    {
    while (menu.GetMenuItemCount() != 0U)
        {
        menu.Destroy(menu.FindItemByPosition(0));
        }

    // these options get checked, so can't use icons on them
    menu.Append(new wxMenuItem(&menu, XRCID("ID_K12_US"), _(L"K-12+ (United States of America)"),
                               wxString{}, wxITEM_CHECK));
    menu.Append(new wxMenuItem(&menu, XRCID("ID_K12_NEWFOUNDLAND"),
                               _(L"K-12+ (Newfoundland and Labrador)"), wxString{}, wxITEM_CHECK));
    menu.Append(new wxMenuItem(&menu, XRCID("ID_K12_BC"), _(L"K-12+ (British Columbia/Yukon)"),
                               wxString{}, wxITEM_CHECK));
    menu.Append(new wxMenuItem(&menu, XRCID("ID_K12_NEW_BRUNSWICK"), _(L"K-12+ (New Brunswick)"),
                               wxString{}, wxITEM_CHECK));
    menu.Append(new wxMenuItem(&menu, XRCID("ID_K12_NOVA_SCOTIA"), _(L"K-12+ (Nova Scotia)"),
                               wxString{}, wxITEM_CHECK));
    menu.Append(new wxMenuItem(&menu, XRCID("ID_K12_ONTARIO"), _(L"K-12+ (Ontario)"), wxString{},
                               wxITEM_CHECK));
    menu.Append(new wxMenuItem(&menu, XRCID("ID_K12_SASKATCHEWAN"), _(L"K-12+ (Saskatchewan)"),
                               wxString{}, wxITEM_CHECK));
    menu.Append(new wxMenuItem(&menu, XRCID("ID_K12_PE"), _(L"K-12+ (Prince Edward Island)"),
                               wxString{}, wxITEM_CHECK));
    menu.Append(new wxMenuItem(&menu, XRCID("ID_K12_MANITOBA"), _(L"K-12+ (Manitoba)"), wxString{},
                               wxITEM_CHECK));
    menu.Append(new wxMenuItem(&menu, XRCID("ID_K12_NT"), _(L"K-12+ (Northwest Territories)"),
                               wxString{}, wxITEM_CHECK));
    menu.Append(new wxMenuItem(&menu, XRCID("ID_K12_ALBERTA"), _(L"K-12+ (Alberta)"), wxString{},
                               wxITEM_CHECK));
    menu.Append(new wxMenuItem(&menu, XRCID("ID_K12_NUNAVUT"), _(L"K-12+ (Nunavut)"), wxString{},
                               wxITEM_CHECK));
    menu.Append(
        new wxMenuItem(&menu, XRCID("ID_QUEBEC"), _DT(L"Quebec"), wxString{}, wxITEM_CHECK));
    menu.Append(new wxMenuItem(&menu, XRCID("ID_ENGLAND"),
                               _DT(L"Key stages (England && Wales)", DTExplanation::ProperNoun),
                               wxString{}, wxITEM_CHECK));
    }

//-----------------------------------
void ReadabilityApp::FillSaveMenu(wxMenu& saveMenu, const RibbonType rtype)
    {
    wxASSERT_MSG(rtype != RibbonType::MainFrameRibbon, L"Mainframe should not have a save menu!");
    if (rtype == RibbonType::MainFrameRibbon)
        {
        return;
        }

    const auto saveIcon = GetResourceManager().GetSVG(L"ribbon/file-save.svg");

    // Don't use stock wxID_SAVE and wxID_SAVEAS because of event handling issues
    // under GTK+ and also because we use can't use wxID_SAVE on a hybrid ribbon
    // button; otherwise, it becomes disabled when the document isn't dirty, and
    // then you can't access the other save options from the menu.
    auto* item = new wxMenuItem(&saveMenu, XRCID("ID_SAVE_PROJECT"), _(L"Save") + L"\tCtrl+S");
    item->SetBitmap(saveIcon);
    saveMenu.Append(item);

    item = new wxMenuItem(&saveMenu, XRCID("ID_SAVE_PROJECT_AS"), _(L"Save As..."));
    item->SetBitmap(saveIcon);
    saveMenu.Append(item);
    }

//-----------------------------------
void ReadabilityApp::FillExportMenu(wxMenu& exportMenu, const RibbonType rtype)
    {
    wxASSERT_MSG(rtype != RibbonType::MainFrameRibbon,
                 L"Mainframe should not have an export menu!");

    const auto filterIcon = GetResourceManager().GetSVG(L"ribbon/filter.svg");
    const auto exportAllIcon = GetResourceManager().GetSVG(L"ribbon/export-all.svg");
    const auto reportIcon = GetResourceManager().GetSVG(L"ribbon/report.svg");

    if (rtype == RibbonType::StandardProjectRibbon)
        {
        auto* item = new wxMenuItem(&exportMenu, XRCID("ID_SAVE_ITEM"), _(L"Export..."));
        exportMenu.Append(item);

        item = new wxMenuItem(&exportMenu, XRCID("ID_EXPORT_ALL"), _(L"Export All..."));
        item->SetBitmap(exportAllIcon);
        exportMenu.Append(item);
        exportMenu.AppendSeparator();

        item = new wxMenuItem(&exportMenu, XRCID("ID_EXPORT_FILTERED_DOCUMENT"),
                              _(L"Export Filtered Document..."));
        item->SetBitmap(filterIcon);
        exportMenu.Append(item);
        }
    else if (rtype == RibbonType::BatchProjectRibbon)
        {
        auto* item = new wxMenuItem(&exportMenu, XRCID("ID_SAVE_ITEM"), _(L"Export..."));
        exportMenu.Append(item);

        item = new wxMenuItem(&exportMenu, XRCID("ID_EXPORT_ALL"), _(L"Export All..."));
        item->SetBitmap(exportAllIcon);
        exportMenu.Append(item);
        exportMenu.AppendSeparator();

        item = new wxMenuItem(&exportMenu, XRCID("ID_EXPORT_SCORES_AND_STATISTICS"),
                              _(L"Export Scores && Statistics..."));
        item->SetBitmap(reportIcon);
        exportMenu.Append(item);

        item = new wxMenuItem(&exportMenu, XRCID("ID_EXPORT_STATISTICS"),
                              _(L"Export Statistics Report..."));
        item->SetBitmap(reportIcon);
        exportMenu.Append(item);
        exportMenu.AppendSeparator();

        item = new wxMenuItem(&exportMenu, XRCID("ID_EXPORT_FILTERED_DOCUMENT"),
                              _(L"Export Filtered Document..."));
        item->SetBitmap(filterIcon);
        exportMenu.Append(item);

        item = new wxMenuItem(&exportMenu, XRCID("ID_BATCH_EXPORT_FILTERED_DOCUMENTS"),
                              _(L"Batch Export Filtered Document..."));
        item->SetBitmap(filterIcon);
        exportMenu.Append(item);
        }
    }

//-----------------------------------
void ReadabilityApp::FillPrintMenu(wxMenu& printMenu, const RibbonType rtype)
    {
    wxMenuItem* item(nullptr);
    if (rtype != RibbonType::MainFrameRibbon)
        {
        // Don't use stock wxID_PRINT because of event handling issues under GTK+
        item = new wxMenuItem(&printMenu, XRCID("ID_PRINT"), _(L"Print...") + _DT(L"\tCtrl+P"));
        item->SetBitmap(GetResourceManager().GetSVG(L"ribbon/print.svg"));
        printMenu.Append(item);
// macOS's and GTK+'s print dialogs have their own built-in preview option
#ifdef __WXMSW__
        item = new wxMenuItem(&printMenu, wxID_PREVIEW, _(L"Print Preview..."));
        item->SetBitmap(GetResourceManager().GetSVG(L"ribbon/preview.svg"));
        printMenu.Append(item);
#endif
        }
    item = new wxMenuItem(&printMenu, wxID_PRINT_SETUP, _(L"Page Setup..."));
    item->SetBitmap(GetResourceManager().GetSVG(L"ribbon/print-setup.svg"));
    printMenu.Append(item);
    item =
        new wxMenuItem(&printMenu, XRCID("ID_PRINTER_HEADER_FOOTER"), _(L"Headers && Footers..."));
    item->SetBitmap(GetResourceManager().GetSVG(L"ribbon/print-header-footer.svg"));
    printMenu.Append(item);
    item = new wxMenuItem(&printMenu, XRCID("ID_EDIT_WATERMARK"), _(L"Watermark..."));
    item->SetBitmap(GetResourceManager().GetSVG(L"ribbon/watermark.svg"));
    printMenu.Append(item);
    }

//-----------------------------------
void ReadabilityApp::ApplyThemeToSideBar(Wisteria::UI::SideBar* sideBar)
    {
    if (sideBar == nullptr)
        {
        return;
        }

    const wxColour accentColour =
        ProjectReportFormat::GetThemeAccentColour(GetAppOptions()->GetReportTheme());
    const wxColour highlightColour = accentColour.ChangeLightness(135);

    sideBar->SetSelectedColour(accentColour);
    sideBar->SetSelectedFontColour(
        Wisteria::Colors::ColorContrast::BlackOrWhiteContrast(accentColour));
    sideBar->SetHighlightColour(highlightColour);
    sideBar->SetHighlightFontColour(
        Wisteria::Colors::ColorContrast::BlackOrWhiteContrast(highlightColour));

    sideBar->Refresh();
    }

//-----------------------------------
Wisteria::UI::SideBar* ReadabilityApp::CreateSideBar(wxWindow* frame, const wxWindowID id)
    {
    auto* sideBar = new Wisteria::UI::SideBar(frame, id);
    sideBar->SetImageList(dynamic_cast<MainFrame*>(GetMainFrame())->GetProjectSideBarImageList());
    ApplyThemeToSideBar(sideBar);

    return sideBar;
    }

//-----------------------------------
void ReadabilityApp::LoadRibbonHomePageListSection(wxRibbonPage* homePage)
    {
        // list button edit panel (Copy, Select, View, Sort)
        {
        auto* editPanel = new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_LIST_PANEL,
                                            _(L"Edit"), wxNullBitmap, wxDefaultPosition,
                                            wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);

        auto* editButtonBar =
            new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

        editButtonBar->AddHybridButton(wxID_COPY, _(L"Copy"), ReadSvgIcon(L"ribbon/copy.svg"),
                                       _(L"Copy the selected rows."));
        editButtonBar->AddButton(wxID_SELECTALL, _(L"Select All"),
                                 ReadSvgIcon(L"ribbon/select-all.svg"), _(L"Select All"));
        editButtonBar->AddButton(XRCID("ID_VIEW_ITEM"), _(L"View Item"),
                                 ReadSvgIcon(L"ribbon/view-spreadsheet-item.svg"),
                                 _(L"View the selected row in tabular format."));
        editButtonBar->AddButton(XRCID("ID_LIST_SORT"), _(L"Sort"), ReadSvgIcon(L"ribbon/sort.svg"),
                                 _(L"Sort the list."));
        }
        // list button edit panel (Long Format, Grade Scales, Copy, Select, View, Sort)
        {
        auto* editPanel = new wxRibbonPanel(
            homePage, MainFrame::ID_EDIT_RIBBON_LIST_TEST_SCORES_PANEL, _(L"Edit"), wxNullBitmap,
            wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);

        auto* editButtonBar =
            new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

        editButtonBar->AddToggleButton(XRCID("ID_LONG_FORMAT"), _(L"Long Format"),
                                       ReadSvgIcon(L"ribbon/grade-display.svg"),
                                       _(L"Display scores in long format."));
        editButtonBar->AddDropdownButton(XRCID("ID_GRADE_SCALES"), _(L"Grade Scale"),
                                         ReadSvgIcon(L"ribbon/education.svg"),
                                         _(L"Change the grade scale display of the scores."));
        editButtonBar->AddHybridButton(wxID_COPY, _(L"Copy"), ReadSvgIcon(L"ribbon/copy.svg"),
                                       _(L"Copy the selected rows."));
        editButtonBar->AddButton(wxID_SELECTALL, _(L"Select All"),
                                 ReadSvgIcon(L"ribbon/select-all.svg"), _(L"Select All"));
        editButtonBar->AddButton(XRCID("ID_VIEW_ITEM"), _(L"View Item"),
                                 ReadSvgIcon(L"ribbon/view-spreadsheet-item.svg"),
                                 _(L"View the selected row in tabular format."));
        editButtonBar->AddButton(XRCID("ID_LIST_SORT"), _(L"Sort"), ReadSvgIcon(L"ribbon/sort.svg"),
                                 _(L"Sort the list."));
        }
        // list button edit panel (Copy, Select, View, Sort, Sum)
        {
        auto* editPanel = new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_LIST_CSVSS_PANEL,
                                            _(L"Edit"), wxNullBitmap, wxDefaultPosition,
                                            wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);

        auto* editButtonBar =
            new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

        editButtonBar->AddHybridButton(wxID_COPY, _(L"Copy"), ReadSvgIcon(L"ribbon/copy.svg"),
                                       _(L"Copy the selected rows."));
        editButtonBar->AddButton(wxID_SELECTALL, _(L"Select All"),
                                 ReadSvgIcon(L"ribbon/select-all.svg"), _(L"Select All"));
        editButtonBar->AddButton(XRCID("ID_VIEW_ITEM"), _(L"View Item"),
                                 ReadSvgIcon(L"ribbon/view-spreadsheet-item.svg"),
                                 _(L"View the selected row in tabular format."));
        editButtonBar->AddButton(XRCID("ID_LIST_SORT"), _(L"Sort"), ReadSvgIcon(L"ribbon/sort.svg"),
                                 _(L"Sort the list."));
        editButtonBar->AddButton(XRCID("ID_SUMMATION"), _(L"Sum"), ReadSvgIcon(L"ribbon/sum.svg"),
                                 _(L"Total the values from the selected column."));
        }
        // list button edit panel (Copy, Select, Sort)
        {
        auto* editPanel = new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_LIST_SIMPLE_PANEL,
                                            _(L"Edit"), wxNullBitmap, wxDefaultPosition,
                                            wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);

        auto* editButtonBar =
            new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

        editButtonBar->AddHybridButton(wxID_COPY, _(L"Copy"), ReadSvgIcon(L"ribbon/copy.svg"),
                                       _(L"Copy"));
        editButtonBar->AddButton(wxID_SELECTALL, _(L"Select All"),
                                 ReadSvgIcon(L"ribbon/select-all.svg"), _(L"Select All"));
        editButtonBar->AddButton(XRCID("ID_LIST_SORT"), _(L"Sort"), ReadSvgIcon(L"ribbon/sort.svg"),
                                 _(L"Sort the list."));
        }
        // list button edit panel (Copy, Select, Sort, Sum)
        {
        auto* editPanel = new wxRibbonPanel(
            homePage, MainFrame::ID_EDIT_RIBBON_LIST_SIMPLE_WITH_SUM_PANEL, _(L"Edit"),
            wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);

        auto* editButtonBar =
            new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

        editButtonBar->AddHybridButton(wxID_COPY, _(L"Copy"), ReadSvgIcon(L"ribbon/copy.svg"),
                                       _(L"Copy"));
        editButtonBar->AddButton(wxID_SELECTALL, _(L"Select All"),
                                 ReadSvgIcon(L"ribbon/select-all.svg"), _(L"Select All"));
        editButtonBar->AddButton(XRCID("ID_LIST_SORT"), _(L"Sort"), ReadSvgIcon(L"ribbon/sort.svg"),
                                 _(L"Sort the list."));
        editButtonBar->AddButton(XRCID("ID_SUMMATION"), _(L"Sum"), ReadSvgIcon(L"ribbon/sum.svg"),
                                 _(L"Total the values from the selected column."));
        }
        // list button edit panel (Copy, Select, Exclude, Sum, Sort)
        {
        auto* editPanel = new wxRibbonPanel(
            homePage, MainFrame::ID_EDIT_RIBBON_LIST_SIMPLE_WITH_SUM_AND_EXCLUDE_PANEL, _(L"Edit"),
            wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);

        auto* editButtonBar =
            new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

        editButtonBar->AddHybridButton(wxID_COPY, _(L"Copy"), ReadSvgIcon(L"ribbon/copy.svg"),
                                       _(L"Copy"));
        editButtonBar->AddButton(wxID_SELECTALL, _(L"Select All"),
                                 ReadSvgIcon(L"ribbon/select-all.svg"), _(L"Select All"));
        editButtonBar->AddButton(XRCID("ID_LIST_SORT"), _(L"Sort"), ReadSvgIcon(L"ribbon/sort.svg"),
                                 _(L"Sort the list."));
        editButtonBar->AddButton(XRCID("ID_EXCLUDE_SELECTED"), _(L"Exclude Selected"),
                                 ReadSvgIcon(L"ribbon/exclude-selected.svg"),
                                 _(L"Exclude selected words."));
        editButtonBar->AddButton(XRCID("ID_SUMMATION"), _(L"Sum"), ReadSvgIcon(L"ribbon/sum.svg"),
                                 _(L"Total the values from the selected column."));
        }
    }

//-----------------------------------
void ReadabilityApp::LoadRibbonHomePageHtmlReportSection(wxRibbonPage* homePage)
    {
    auto* editPanel = new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_SUMMARY_REPORT_PANEL,
                                        _(L"Edit"), wxNullBitmap, wxDefaultPosition, wxDefaultSize,
                                        wxRIBBON_PANEL_NO_AUTO_MINIMISE);

    auto* editButtonBar = new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

    editButtonBar->AddButton(wxID_COPY, _(L"Copy"), ReadSvgIcon(L"ribbon/copy.svg"),
                             _(L"Copy the report."));
    }

//-----------------------------------
void ReadabilityApp::LoadRibbonHomePageGraphSection(wxRibbonPage* homePage, const wxDocument* doc)
    {
        // bar chart panel
        {
        auto* editPanel = new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_BAR_CHART_PANEL,
                                            _(L"Edit"), wxNullBitmap, wxDefaultPosition,
                                            wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);

        auto* editButtonBar =
            new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"), _(L"Background"),
                                         ReadSvgIcon(L"ribbon/photos.svg"),
                                         _(L"Set the graph's background."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"), _(L"Font"),
                                         ReadSvgIcon(L"ribbon/font.svg"),
                                         _(L"Change the graph's fonts."));
        editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"), _(L"Logo"), ReadSvgIcon(L"ribbon/logo.svg"),
                                 _(L"Add a logo to the graph."));
        editButtonBar->AddToggleButton(XRCID("ID_DROP_SHADOW"), _(L"Shadows"),
                                       ReadSvgIcon(L"ribbon/shadow.svg"),
                                       _(L"Display drop shadows on the graphs."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_BAR_STYLE"), _(L"Bar Style"),
                                         ReadSvgIcon(L"ribbon/bar-top-to-bottom.svg"),
                                         _(L"Changes the bar appearance."));
        editButtonBar->AddToggleButton(XRCID("ID_EDIT_BAR_LABELS"), _(L"Labels"),
                                       ReadSvgIcon(L"ribbon/bar-labels.svg"),
                                       _(L"Shows or hides the bars' labels."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_BAR_ORIENTATION"), _(L"Orientation"),
                                         ReadSvgIcon(L"ribbon/axis-orientation.svg"),
                                         _(L"Changes the axis orientation."));
        editButtonBar->AddDropdownButton(XRCID("ID_GRAPH_SORT"), _(L"Sort"),
                                         ReadSvgIcon(L"ribbon/bar-sort.svg"),
                                         _(L"Sort the bars in the graph."));
        editButtonBar->AddButton(wxID_COPY, _(L"Copy"), ReadSvgIcon(L"ribbon/copy.svg"),
                                 _(L"Copy"));
        editButtonBar->AddHybridButton(wxID_ZOOM_IN, _(L"Zoom"), ReadSvgIcon(L"ribbon/zoom-in.svg"),
                                       _(L"Zoom"));
        }
        // box plot panel
        {
        auto* editPanel = new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_BOX_PLOT_PANEL,
                                            _(L"Edit"), wxNullBitmap, wxDefaultPosition,
                                            wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);

        auto* editButtonBar =
            new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"), _(L"Background"),
                                         ReadSvgIcon(L"ribbon/photos.svg"),
                                         _(L"Set the graph's background."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"), _(L"Font"),
                                         ReadSvgIcon(L"ribbon/font.svg"),
                                         _(L"Change the graph's fonts."));
        editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"), _(L"Logo"), ReadSvgIcon(L"ribbon/logo.svg"),
                                 _(L"Add a logo to the graph."));
        editButtonBar->AddToggleButton(XRCID("ID_DROP_SHADOW"), _(L"Shadows"),
                                       ReadSvgIcon(L"ribbon/shadow.svg"),
                                       _(L"Display drop shadows on the graphs."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_BOX_STYLE"), _(L"Box Style"),
                                         ReadSvgIcon(L"ribbon/bar-top-to-bottom.svg"),
                                         _(L"Changes the box appearance."));
        editButtonBar->AddToggleButton(XRCID("ID_BOX_PLOT_DISPLAY_LABELS"), _(L"Display Labels"),
                                       ReadSvgIcon(L"ribbon/show-all-labels.svg"),
                                       _(L"Displays labels on the box and whiskers."));
        editButtonBar->AddToggleButton(XRCID("ID_BOX_PLOT_DISPLAY_ALL_POINTS"),
                                       _(L"Display Points"),
                                       ReadSvgIcon(L"ribbon/show-all-points.svg"),
                                       _(L"Displays all data points onto the plot."));
        editButtonBar->AddButton(wxID_COPY, _(L"Copy"), ReadSvgIcon(L"ribbon/copy.svg"),
                                 _(L"Copy the graph."));
        editButtonBar->AddHybridButton(wxID_ZOOM_IN, _(L"Zoom"), ReadSvgIcon(L"ribbon/zoom-in.svg"),
                                       _(L"Zoom"));
        }
        // syllable histogram panel
        {
        auto* editPanel = new wxRibbonPanel(
            homePage, MainFrame::ID_EDIT_RIBBON_SYLLABLE_HISTOGRAM_PANEL, _(L"Edit"), wxNullBitmap,
            wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);

        auto* editButtonBar =
            new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

        editButtonBar->AddToggleButton(XRCID("ID_EDIT_GRAPH_SHOWCASE_KEY_ITEMS"),
                                       // TRANSLATORS: Draw attention to.
                                       _(L"Showcase"), ReadSvgIcon(L"ribbon/showcase.svg"),
                                       _(L"Toggle whether complex word bars are being showcased."));

        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"), _(L"Background"),
                                         ReadSvgIcon(L"ribbon/photos.svg"),
                                         _(L"Set the graph's background."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"), _(L"Font"),
                                         ReadSvgIcon(L"ribbon/font.svg"),
                                         _(L"Change the graph's fonts."));
        editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"), _(L"Logo"), ReadSvgIcon(L"ribbon/logo.svg"),
                                 _(L"Add a logo to the graph."));
        editButtonBar->AddToggleButton(XRCID("ID_DROP_SHADOW"), _(L"Shadows"),
                                       ReadSvgIcon(L"ribbon/shadow.svg"),
                                       _(L"Display drop shadows on the graphs."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_HISTOGRAM_BAR_STYLE"), _(L"Bar Style"),
                                         ReadSvgIcon(L"ribbon/bar-top-to-bottom.svg"),
                                         _(L"Changes the bar appearance."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_HISTOBAR_LABELS"), _(L"Labels"),
                                         ReadSvgIcon(L"ribbon/bar-labels.svg"),
                                         _(L"Changes what is displayed on the bars' labels."));
        editButtonBar->AddButton(wxID_COPY, _(L"Copy"), ReadSvgIcon(L"ribbon/copy.svg"),
                                 _(L"Copy the graph."));
        editButtonBar->AddHybridButton(wxID_ZOOM_IN, _(L"Zoom"), ReadSvgIcon(L"ribbon/zoom-in.svg"),
                                       _(L"Zoom"));
        }
        // histogram panel
        {
        auto* editPanel = new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_HISTOGRAM_PANEL,
                                            _(L"Edit"), wxNullBitmap, wxDefaultPosition,
                                            wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);

        auto* editButtonBar =
            new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"), _(L"Background"),
                                         ReadSvgIcon(L"ribbon/photos.svg"),
                                         _(L"Set the graph's background."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"), _(L"Font"),
                                         ReadSvgIcon(L"ribbon/font.svg"),
                                         _(L"Change the graph's fonts."));
        editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"), _(L"Logo"), ReadSvgIcon(L"ribbon/logo.svg"),
                                 _(L"Add a logo to the graph."));
        editButtonBar->AddToggleButton(XRCID("ID_DROP_SHADOW"), _(L"Shadows"),
                                       ReadSvgIcon(L"ribbon/shadow.svg"),
                                       _(L"Display drop shadows on the graphs."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_HISTOGRAM_BAR_STYLE"), _(L"Bar Style"),
                                         ReadSvgIcon(L"ribbon/bar-top-to-bottom.svg"),
                                         _(L"Changes the bar appearance."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_HISTOBAR_LABELS"), _(L"Labels"),
                                         ReadSvgIcon(L"ribbon/bar-labels.svg"),
                                         _(L"Changes what is displayed on the bars' labels."));
        editButtonBar->AddButton(wxID_COPY, _(L"Copy"), ReadSvgIcon(L"ribbon/copy.svg"),
                                 _(L"Copy the graph."));
        editButtonBar->AddHybridButton(wxID_ZOOM_IN, _(L"Zoom"), ReadSvgIcon(L"ribbon/zoom-in.svg"),
                                       _(L"Zoom"));
        }
        // batch project histogram panel
        {
        auto* editPanel = new wxRibbonPanel(
            homePage, MainFrame::ID_EDIT_RIBBON_HISTOGRAM_BATCH_PANEL, _(L"Edit"), wxNullBitmap,
            wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);

        auto* editButtonBar =
            new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

        editButtonBar->AddButton(XRCID("ID_EDIT_GRAPH_COLOR_SCHEME"), _(L"Colors"),
                                 ReadSvgIcon(L"ribbon/color-wheel.svg"),
                                 _(L"Select the color scheme for the grouped histogram."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"), _(L"Background"),
                                         ReadSvgIcon(L"ribbon/photos.svg"),
                                         _(L"Set the graph's background."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"), _(L"Font"),
                                         ReadSvgIcon(L"ribbon/font.svg"),
                                         _(L"Change the graph's fonts."));
        editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"), _(L"Logo"), ReadSvgIcon(L"ribbon/logo.svg"),
                                 _(L"Add a logo to the graph."));
        editButtonBar->AddToggleButton(XRCID("ID_DROP_SHADOW"), _(L"Shadows"),
                                       ReadSvgIcon(L"ribbon/shadow.svg"),
                                       _(L"Display drop shadows on the graphs."));
        editButtonBar->AddDropdownButton(XRCID("ID_GRADE_SCALES"), _(L"Grade Scale"),
                                         ReadSvgIcon(L"ribbon/education.svg"),
                                         _(L"Change the grade scale display of the scores."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_HISTOGRAM_BAR_STYLE"), _(L"Bar Style"),
                                         ReadSvgIcon(L"ribbon/bar-top-to-bottom.svg"),
                                         _(L"Changes the bar appearance."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_HISTOBAR_LABELS"), _(L"Labels"),
                                         ReadSvgIcon(L"ribbon/bar-labels.svg"),
                                         _(L"Changes what is displayed on the bars' labels."));
        editButtonBar->AddButton(wxID_COPY, _(L"Copy"), ReadSvgIcon(L"ribbon/copy.svg"),
                                 _(L"Copy the graph."));
        editButtonBar->AddHybridButton(wxID_ZOOM_IN, _(L"Zoom"), ReadSvgIcon(L"ribbon/zoom-in.svg"),
                                       _(L"Zoom"));
        }
        // wordcloud panel
        {
        auto* editPanel = new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_WORDCLOUD_PANEL,
                                            _(L"Edit"), wxNullBitmap, wxDefaultPosition,
                                            wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);

        auto* editButtonBar =
            new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

        editButtonBar->AddButton(XRCID("ID_EDIT_GRAPH_COLOR_SCHEME"), _(L"Colors"),
                                 ReadSvgIcon(L"ribbon/color-wheel.svg"),
                                 _(L"Select the color scheme for the pie chart."));

        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"), _(L"Background"),
                                         ReadSvgIcon(L"ribbon/photos.svg"),
                                         _(L"Set the graph's background."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"), _(L"Font"),
                                         ReadSvgIcon(L"ribbon/font.svg"),
                                         _(L"Change the graph's fonts."));
        editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"), _(L"Logo"), ReadSvgIcon(L"ribbon/logo.svg"),
                                 _(L"Add a logo to the graph."));

        editButtonBar->AddButton(wxID_COPY, _(L"Copy"), ReadSvgIcon(L"ribbon/copy.svg"),
                                 _(L"Copy the graph."));
        editButtonBar->AddHybridButton(wxID_ZOOM_IN, _(L"Zoom"), ReadSvgIcon(L"ribbon/zoom-in.svg"),
                                       _(L"Zoom"));
        }
        // pie chart panel
        {
        auto* editPanel = new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_PIE_CHART_PANEL,
                                            _(L"Edit"), wxNullBitmap, wxDefaultPosition,
                                            wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);

        auto* editButtonBar =
            new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

        editButtonBar->AddButton(XRCID("ID_EDIT_GRAPH_COLOR_SCHEME"), _(L"Colors"),
                                 ReadSvgIcon(L"ribbon/color-wheel.svg"),
                                 _(L"Select the color scheme for the pie chart."));
        editButtonBar->AddToggleButton(
            XRCID("ID_EDIT_GRAPH_SHOWCASE_KEY_ITEMS"), _(L"Showcase"),
            ReadSvgIcon(L"ribbon/showcase.svg"),
            // TRANSLATORS: Pie chart slices.
            _(L"Toggle whether complex word slices are being showcased."));

        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"), _(L"Background"),
                                         ReadSvgIcon(L"ribbon/photos.svg"),
                                         _(L"Set the graph's background."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"), _(L"Font"),
                                         ReadSvgIcon(L"ribbon/font.svg"),
                                         _(L"Change the graph's fonts."));
        editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"), _(L"Logo"), ReadSvgIcon(L"ribbon/logo.svg"),
                                 _(L"Add a logo to the graph."));

        editButtonBar->AddButton(wxID_COPY, _(L"Copy"), ReadSvgIcon(L"ribbon/copy.svg"),
                                 _(L"Copy the graph."));
        editButtonBar->AddHybridButton(wxID_ZOOM_IN, _(L"Zoom"), ReadSvgIcon(L"ribbon/zoom-in.svg"),
                                       _(L"Zoom"));
        }
        // graph panel
        {
        auto* editPanel = new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_GRAPH_PANEL,
                                            _(L"Edit"), wxNullBitmap, wxDefaultPosition,
                                            wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);

        auto* editButtonBar =
            new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"), _(L"Background"),
                                         ReadSvgIcon(L"ribbon/photos.svg"),
                                         _(L"Set the graph's background."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"), _(L"Font"),
                                         ReadSvgIcon(L"ribbon/font.svg"),
                                         _(L"Change the graph's fonts."));
        editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"), _(L"Logo"), ReadSvgIcon(L"ribbon/logo.svg"),
                                 _(L"Add a logo to the graph."));
        editButtonBar->AddToggleButton(XRCID("ID_DROP_SHADOW"), _(L"Shadows"),
                                       ReadSvgIcon(L"ribbon/shadow.svg"),
                                       _(L"Display drop shadows on the graphs."));

        editButtonBar->AddButton(wxID_COPY, _(L"Copy"), ReadSvgIcon(L"ribbon/copy.svg"),
                                 _(L"Copy the graph."));
        editButtonBar->AddHybridButton(wxID_ZOOM_IN, _(L"Zoom"), ReadSvgIcon(L"ribbon/zoom-in.svg"),
                                       _(L"Zoom"));
        }
        // Lix (German) panel
        {
        auto* editPanel = new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_LIX_GERMAN_PANEL,
                                            _(L"Edit"), wxNullBitmap, wxDefaultPosition,
                                            wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);

        auto* editButtonBar =
            new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

        editButtonBar->AddToggleButton(XRCID("ID_EDIT_GRAPH_SHOWCASE_KEY_ITEMS"), _(L"Showcase"),
                                       ReadSvgIcon(L"ribbon/showcase.svg"),
                                       _(L"Toggle whether the score is being showcased."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"), _(L"Background"),
                                         ReadSvgIcon(L"ribbon/photos.svg"),
                                         _(L"Set the graph's background."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"), _(L"Font"),
                                         ReadSvgIcon(L"ribbon/font.svg"),
                                         _(L"Change the graph's fonts."));
        editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"), _(L"Logo"), ReadSvgIcon(L"ribbon/logo.svg"),
                                 _(L"Add a logo to the graph."));
        editButtonBar->AddToggleButton(XRCID("ID_USE_ENGLISH_LABELS"), _(L"English Labels"),
                                       ReadSvgIcon(L"ribbon/german2english.svg"),
                                       _(L"Use translated (English) labels for the brackets."));
        editButtonBar->AddButton(wxID_COPY, _(L"Copy"), ReadSvgIcon(L"ribbon/copy.svg"),
                                 _(L"Copy the graph."));
        editButtonBar->AddHybridButton(wxID_ZOOM_IN, _(L"Zoom"), ReadSvgIcon(L"ribbon/zoom-in.svg"),
                                       _(L"Zoom"));
        }
        // Raygor panel
        {
        auto* editPanel = new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_RAYGOR_PANEL,
                                            _(L"Edit"), wxNullBitmap, wxDefaultPosition,
                                            wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);

        auto* editButtonBar =
            new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"), _(L"Background"),
                                         ReadSvgIcon(L"ribbon/photos.svg"),
                                         _(L"Set the graph's background."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"), _(L"Font"),
                                         ReadSvgIcon(L"ribbon/font.svg"),
                                         _(L"Change the graph's fonts."));
        editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"), _(L"Logo"), ReadSvgIcon(L"ribbon/logo.svg"),
                                 _(L"Add a logo to the graph."));
        editButtonBar->AddButton(XRCID("ID_INVALID_REGION_COLOR"), _(L"Invalid Region"),
                                 ReadSvgIcon(L"ribbon/invalid-region.svg"),
                                 _(L"Change the color of the invalid regions."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_RAYGOR_STYLE"), _(L"Raygor Style"),
                                         ReadSvgIcon(L"ribbon/raygor-style.svg"),
                                         _(L"Change the layout style of the Raygor graph."));
        editButtonBar->AddButton(wxID_COPY, _(L"Copy"), ReadSvgIcon(L"ribbon/copy.svg"),
                                 _(L"Copy the graph."));
        editButtonBar->AddHybridButton(wxID_ZOOM_IN, _(L"Zoom"), ReadSvgIcon(L"ribbon/zoom-in.svg"),
                                       _(L"Zoom"));
        }
        // Fry panel
        {
        auto* editPanel = new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_FRY_PANEL,
                                            _(L"Edit"), wxNullBitmap, wxDefaultPosition,
                                            wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);

        auto* editButtonBar =
            new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

        if (doc != nullptr && doc->IsKindOf(wxCLASSINFO(ProjectDoc)))
            {
            editButtonBar->AddToggleButton(XRCID("ID_EDIT_GRAPH_SHOWCASE_KEY_ITEMS"),
                                           _(L"Showcase"), ReadSvgIcon(L"ribbon/showcase.svg"),
                                           _(L"Toggle whether the score is being showcased."));
            }
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"), _(L"Background"),
                                         ReadSvgIcon(L"ribbon/photos.svg"),
                                         _(L"Set the graph's background."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"), _(L"Font"),
                                         ReadSvgIcon(L"ribbon/font.svg"),
                                         _(L"Change the graph's fonts."));
        editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"), _(L"Logo"), ReadSvgIcon(L"ribbon/logo.svg"),
                                 _(L"Add a logo to the graph."));
        editButtonBar->AddButton(XRCID("ID_INVALID_REGION_COLOR"), _(L"Invalid Region"),
                                 ReadSvgIcon(L"ribbon/invalid-region.svg"),
                                 _(L"Change the color of the invalid regions."));
        editButtonBar->AddButton(wxID_COPY, _(L"Copy"), ReadSvgIcon(L"ribbon/copy.svg"),
                                 _(L"Copy the graph."));
        editButtonBar->AddHybridButton(wxID_ZOOM_IN, _(L"Zoom"), ReadSvgIcon(L"ribbon/zoom-in.svg"),
                                       _(L"Zoom"));
        }
        // Flesch panel
        {
        auto* editPanel = new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_FLESCH_PANEL,
                                            _(L"Edit"), wxNullBitmap, wxDefaultPosition,
                                            wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);

        auto* editButtonBar =
            new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"), _(L"Background"),
                                         ReadSvgIcon(L"ribbon/photos.svg"),
                                         _(L"Set the graph's background."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"), _(L"Font"),
                                         ReadSvgIcon(L"ribbon/font.svg"),
                                         _(L"Change the graph's fonts."));
        editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"), _(L"Logo"), ReadSvgIcon(L"ribbon/logo.svg"),
                                 _(L"Add a logo to the graph."));
        editButtonBar->AddToggleButton(XRCID("ID_FLESCH_DISPLAY_LINES"), _(L"Connect Points"),
                                       ReadSvgIcon(L"ribbon/flesch-line.svg"),
                                       _(L"Display the line through the factors and score."));
        editButtonBar->AddButton(wxID_COPY, _(L"Copy"), ReadSvgIcon(L"ribbon/copy.svg"),
                                 _(L"Copy the graph."));
        editButtonBar->AddHybridButton(wxID_ZOOM_IN, _(L"Zoom"), ReadSvgIcon(L"ribbon/zoom-in.svg"),
                                       _(L"Zoom"));
        }
        // DB2
        {
        auto* editPanel = new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_DB2_PANEL,
                                            _(L"Edit"), wxNullBitmap, wxDefaultPosition,
                                            wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);

        auto* editButtonBar =
            new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

        editButtonBar->AddToggleButton(XRCID("ID_EDIT_GRAPH_SHOWCASE_KEY_ITEMS"), _(L"Showcase"),
                                       ReadSvgIcon(L"ribbon/showcase.svg"),
                                       _(L"Toggle whether areas with scores are being showcased."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"), _(L"Background"),
                                         ReadSvgIcon(L"ribbon/photos.svg"),
                                         _(L"Set the graph's background."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"), _(L"Font"),
                                         ReadSvgIcon(L"ribbon/font.svg"),
                                         _(L"Change the graph's fonts."));
        editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"), _(L"Logo"), ReadSvgIcon(L"ribbon/logo.svg"),
                                 _(L"Add a logo to the graph."));

        editButtonBar->AddButton(wxID_COPY, _(L"Copy"), ReadSvgIcon(L"ribbon/copy.svg"),
                                 _(L"Copy the graph."));
        editButtonBar->AddHybridButton(wxID_ZOOM_IN, _(L"Zoom"), ReadSvgIcon(L"ribbon/zoom-in.svg"),
                                       _(L"Zoom"));
        }
        // LIX
        {
        auto* editPanel = new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_LIX_PANEL,
                                            _(L"Edit"), wxNullBitmap, wxDefaultPosition,
                                            wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);

        auto* editButtonBar =
            new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

        editButtonBar->AddToggleButton(XRCID("ID_EDIT_GRAPH_SHOWCASE_KEY_ITEMS"), _(L"Showcase"),
                                       ReadSvgIcon(L"ribbon/showcase.svg"),
                                       _(L"Toggle whether areas with scores are being showcased."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"), _(L"Background"),
                                         ReadSvgIcon(L"ribbon/photos.svg"),
                                         _(L"Set the graph's background."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"), _(L"Font"),
                                         ReadSvgIcon(L"ribbon/font.svg"),
                                         _(L"Change the graph's fonts."));
        editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"), _(L"Logo"), ReadSvgIcon(L"ribbon/logo.svg"),
                                 _(L"Add a logo to the graph."));

        editButtonBar->AddButton(wxID_COPY, _(L"Copy"), ReadSvgIcon(L"ribbon/copy.svg"),
                                 _(L"Copy the graph."));
        editButtonBar->AddHybridButton(wxID_ZOOM_IN, _(L"Zoom"), ReadSvgIcon(L"ribbon/zoom-in.svg"),
                                       _(L"Zoom"));
        }
        // panel for other readability graphs
        {
        auto* editPanel = new wxRibbonPanel(
            homePage, MainFrame::ID_EDIT_RIBBON_GENERAL_READABILITY_GRAPH_PANEL, _(L"Edit"),
            wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);

        auto* editButtonBar =
            new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_BACKGROUND"), _(L"Background"),
                                         ReadSvgIcon(L"ribbon/photos.svg"),
                                         _(L"Set the graph's background."));
        editButtonBar->AddDropdownButton(XRCID("ID_EDIT_GRAPH_FONTS"), _(L"Font"),
                                         ReadSvgIcon(L"ribbon/font.svg"),
                                         _(L"Change the graph's fonts."));
        editButtonBar->AddButton(XRCID("ID_EDIT_LOGO"), _(L"Logo"), ReadSvgIcon(L"ribbon/logo.svg"),
                                 _(L"Add a logo to the graph."));

        editButtonBar->AddButton(wxID_COPY, _(L"Copy"), ReadSvgIcon(L"ribbon/copy.svg"),
                                 _(L"Copy the graph."));
        editButtonBar->AddHybridButton(wxID_ZOOM_IN, _(L"Zoom"), ReadSvgIcon(L"ribbon/zoom-in.svg"),
                                       _(L"Zoom"));
        }
    }

//-----------------------------------
void ReadabilityApp::LoadRibbonHomePageStatisticsReportSection(wxRibbonPage* homePage)
    {
        // statistics list report in a standard project
        {
        auto* editPanel = new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_STATS_LIST_PANEL,
                                            _(L"Edit"), wxNullBitmap, wxDefaultPosition,
                                            wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);

        auto* editButtonBar =
            new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

        editButtonBar->AddButton(XRCID("ID_EDIT_STATS_REPORT"), _(L"Edit Report"),
                                 ReadSvgIcon(L"ribbon/edit-report.svg"),
                                 _(L"Select which statistics to include in the report."));
        editButtonBar->AddHybridButton(wxID_COPY, _(L"Copy"), ReadSvgIcon(L"ribbon/copy.svg"),
                                       _(L"Copy the selected rows."));
        editButtonBar->AddButton(wxID_SELECTALL, _(L"Select All"),
                                 ReadSvgIcon(L"ribbon/select-all.svg"), _(L"Select All"));
        editButtonBar->AddButton(XRCID("ID_VIEW_ITEM"), _(L"View Item"),
                                 ReadSvgIcon(L"ribbon/view-spreadsheet-item.svg"),
                                 _(L"View the selected row in tabular format."));
        editButtonBar->AddButton(XRCID("ID_LIST_SORT"), _(L"Sort"), ReadSvgIcon(L"ribbon/sort.svg"),
                                 _(L"Sort the list."));
        }
        // statistics HTML report in a standard project
        {
        auto* editPanel = new wxRibbonPanel(
            homePage, MainFrame::ID_EDIT_RIBBON_STATS_SUMMARY_REPORT_PANEL, _(L"Edit"),
            wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);

        auto* editButtonBar =
            new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

        editButtonBar->AddButton(XRCID("ID_EDIT_STATS_REPORT"), _(L"Edit Report"),
                                 ReadSvgIcon(L"ribbon/edit-report.svg"),
                                 _(L"Select which statistics to include in the report."));
        editButtonBar->AddButton(wxID_COPY, _(L"Copy"), ReadSvgIcon(L"ribbon/copy.svg"),
                                 _(L"Copy"));
        }
    }

//-----------------------------------
void ReadabilityApp::LoadRibbonHomePageExplanationListSection(wxRibbonPage* homePage)
    {
    auto* editPanel = new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_EXPLANATION_LIST_PANEL,
                                        _(L"Edit"), wxNullBitmap, wxDefaultPosition, wxDefaultSize,
                                        wxRIBBON_PANEL_NO_AUTO_MINIMISE);

    auto* editButtonBar = new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

    editButtonBar->AddToggleButton(XRCID("ID_LONG_FORMAT"), _(L"Long Format"),
                                   ReadSvgIcon(L"ribbon/grade-display.svg"),
                                   _(L"Display scores in long format."));
    editButtonBar->AddDropdownButton(XRCID("ID_GRADE_SCALES"), _(L"Grade Scale"),
                                     ReadSvgIcon(L"ribbon/education.svg"),
                                     _(L"Change the grade scale display of the scores."));
    editButtonBar->AddButton(wxID_COPY, _(L"Copy"), ReadSvgIcon(L"ribbon/copy.svg"),
                             _(L"Copy the selected rows."));
    editButtonBar->AddButton(XRCID("ID_LIST_SORT"), _(L"Sort"), ReadSvgIcon(L"ribbon/sort.svg"),
                             _(L"Sort the list."));
    }

//-----------------------------------
void ReadabilityApp::LoadRibbonHomePageTextWindowSection(wxRibbonPage* homePage)
    {
    auto* editPanel = new wxRibbonPanel(homePage, MainFrame::ID_EDIT_RIBBON_REPORT_PANEL,
                                        _(L"Edit"), wxNullBitmap, wxDefaultPosition, wxDefaultSize,
                                        wxRIBBON_PANEL_NO_AUTO_MINIMISE);

    auto* editButtonBar = new wxRibbonButtonBar(editPanel, MainFrame::ID_EDIT_RIBBON_BUTTON_BAR);

    editButtonBar->AddButton(XRCID("ID_TEXT_WINDOW_FONT"), _(L"Font"),
                             ReadSvgIcon(L"ribbon/font.svg"), _(L"Change the font."));
    editButtonBar->AddButton(XRCID("ID_TEXT_WINDOW_COLORS"), _(L"Highlight"),
                             ReadSvgIcon(L"ribbon/highlighting.svg"),
                             _(L"Change the highlight colors."));
    editButtonBar->AddButton(wxID_COPY, _(L"Copy"), ReadSvgIcon(L"ribbon/copy.svg"), _(L"Copy"));
    editButtonBar->AddButton(wxID_SELECTALL, _(L"Select All"),
                             ReadSvgIcon(L"ribbon/select-all.svg"), _(L"Select All"));
    }

//-----------------------------------
void ReadabilityApp::LoadRibbonHomePage(wxRibbonBar* ribbon, const RibbonType rtype,
                                        const wxDocument* doc)
    {
    GetMainFrameEx()->m_homeRibbonPage = new wxRibbonPage(
        ribbon, wxID_ANY, _(L"Home"),
        ReadSvgIcon(wxSystemSettings::GetAppearance().IsDark() ? L"ribbon/home-dark-mode.svg" :
                                                                 L"ribbon/home.svg",
                    wxSize{ 16, 16 }));
    auto* projectPanel =
        new wxRibbonPanel(GetMainFrameEx()->m_homeRibbonPage, wxID_ANY, _(L"Project"), wxNullBitmap,
                          wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
    auto* projectButtonBar =
        new wxRibbonButtonBar(projectPanel, MainFrame::ID_PROJECT_RIBBON_BUTTON_BAR);
    projectButtonBar->AddHybridButton(wxID_NEW, _(L"New"), ReadSvgIcon(L"ribbon/document.svg"),
                                      _(L"Create a new project."));
    projectButtonBar->AddHybridButton(wxID_OPEN, _(L"Open"), ReadSvgIcon(L"ribbon/file-open.svg"),
                                      _(L"Open an existing project."));
    if (rtype == RibbonType::BatchProjectRibbon)
        {
        auto* documentsPanel = new wxRibbonPanel(GetMainFrameEx()->m_homeRibbonPage, wxID_ANY,
                                                 _(L"Documents"), wxNullBitmap, wxDefaultPosition,
                                                 wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        auto* documentsButtonBar = new wxRibbonButtonBar(documentsPanel);
        documentsButtonBar->AddButton(XRCID("ID_SEND_TO_STANDARD_PROJECT"), _(L"Subproject"),
                                      ReadSvgIcon(L"ribbon/send-to-subproject.svg"),
                                      _(L"Create a standard project from the selected document."));
        documentsButtonBar->AddToggleButton(XRCID("ID_STATISTICS_WINDOW"), _(L"Statistics"),
                                            ReadSvgIcon(L"ribbon/stats.svg"),
                                            _(L"Display statistics for the selected document."));
        documentsButtonBar->AddButton(XRCID("ID_REMOVE_DOCUMENT"), _(L"Remove Document"),
                                      ReadSvgIcon(L"ribbon/delete-document.svg"),
                                      _(L"Remove the selected document from the project."));
        }
    if (rtype != RibbonType::MainFrameRibbon)
        {
        projectButtonBar->AddButton(XRCID("ID_DOCUMENT_REFRESH"), _(L"Reload"),
                                    ReadSvgIcon(L"ribbon/reload.svg"),
                                    _(L"Reanalyze the document."));
        if (rtype == RibbonType::StandardProjectRibbon)
            {
            projectButtonBar->AddToggleButton(XRCID("ID_REALTIME_UPDATE"), _(L"Real-time Update"),
                                              ReadSvgIcon(L"ribbon/realtime.svg"),
                                              _(L"Automatically reload the project as the "
                                                "source document is edited externally."));
            }
        projectButtonBar->AddHybridButton(XRCID("ID_SAVE_PROJECT"), _(L"Save"),
                                          ReadSvgIcon(L"ribbon/file-save.svg"),
                                          _(L"Save the project."));
        projectButtonBar->AddHybridButton(XRCID("ID_SAVE_ITEM"), _(L"Export"),
                                          ReadSvgIcon(L"ribbon/export.svg"),
                                          _(L"Export the selected window."));
        projectButtonBar->AddHybridButton(wxID_PRINT, _(L"Print"), ReadSvgIcon(L"ribbon/print.svg"),
                                          _(L"Print the selected window."));
        projectButtonBar->AddButton(wxID_PROPERTIES, _(L"Properties"),
                                    ReadSvgIcon(L"ribbon/project-settings.svg"),
                                    _(L"Change the settings for this project."));
        projectButtonBar->AddToggleButton(XRCID("ID_SHOW_SIDEBAR"), _(L"Sidebar"),
                                          ReadSvgIcon(L"ribbon/toggle-sidebar.svg"),
                                          _(L"Shows or hides the sidebar."));
        projectButtonBar->ToggleButton(XRCID("ID_SHOW_SIDEBAR"), true);

        // edit sections
        //--------------
        LoadRibbonHomePageListSection(GetMainFrameEx()->m_homeRibbonPage);
        LoadRibbonHomePageHtmlReportSection(GetMainFrameEx()->m_homeRibbonPage);
        LoadRibbonHomePageExplanationListSection(GetMainFrameEx()->m_homeRibbonPage);
        LoadRibbonHomePageTextWindowSection(GetMainFrameEx()->m_homeRibbonPage);
        LoadRibbonHomePageStatisticsReportSection(GetMainFrameEx()->m_homeRibbonPage);
        LoadRibbonHomePageGraphSection(GetMainFrameEx()->m_homeRibbonPage, doc);
        }
    else // rtype == RibbonType::MainFrameRibbon
        {
        // settings section
        auto* settingsPanel = new wxRibbonPanel(GetMainFrameEx()->m_homeRibbonPage, wxID_ANY,
                                                _(L"Settings"), wxNullBitmap, wxDefaultPosition,
                                                wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        auto* settingsButtonBar = new wxRibbonButtonBar(settingsPanel);
        settingsButtonBar->AddDropdownButton(XRCID("ID_PRINT_OPTIONS"), _(L"Printing"),
                                             ReadSvgIcon(L"ribbon/print.svg"),
                                             _(L"Change print settings."));
        settingsButtonBar->AddDropdownButton(XRCID("ID_EDIT_DICTIONARY"), _(L"Spell Checker"),
                                             ReadSvgIcon(L"ribbon/misspellings.svg"),
                                             _(L"Edit the spell checker's dictionary."));
        settingsButtonBar->AddButton(wxID_PREFERENCES, _(L"Options"),
                                     ReadSvgIcon(L"ribbon/configure.svg"),
                                     _(L"Change the program's general options."));
        // test section
        auto* readabilityTestsPanel = new wxRibbonPanel(
            GetMainFrameEx()->m_homeRibbonPage, wxID_ANY, _(L"Readability Tests"), wxNullBitmap,
            wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        auto* readabilityTestsBar = new wxRibbonButtonBar(readabilityTestsPanel);
        readabilityTestsBar->AddDropdownButton(XRCID("ID_CUSTOM_TESTS"), _(L"Custom"),
                                               ReadSvgIcon(L"ribbon/formula.svg"),
                                               _(L"Create or edit custom tests."));
        readabilityTestsBar->AddDropdownButton(XRCID("ID_TEST_BUNDLES"), _(L"Bundles"),
                                               ReadSvgIcon(L"ribbon/bundles.svg"),
                                               _(L"Add multiple tests to a project at once."));
        readabilityTestsBar->AddButton(XRCID("ID_TESTS_OVERVIEW"), _(L"Tests Overview"),
                                       ReadSvgIcon(L"ribbon/tests-overview.svg"),
                                       _(L"View information about each readability test."));
        readabilityTestsBar->AddHybridButton(XRCID("ID_WORD_LISTS"), _(L"Word Lists"),
                                             ReadSvgIcon(L"tests/dale-chall-test.svg"),
                                             _(L"View the word lists used by readability tests."));
        readabilityTestsBar->AddDropdownButton(
            XRCID("ID_BLANK_GRAPHS"), _(L"Blank Graphs"), ReadSvgIcon(L"ribbon/blank-graphs.svg"),
            _(L"Print or save blank readability graph templates."));
        // tools section
        auto* toolsPanel = new wxRibbonPanel(GetMainFrameEx()->m_homeRibbonPage, wxID_ANY,
                                             _(L"Tools"), wxNullBitmap, wxDefaultPosition,
                                             wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        auto* toolButtonBar = new wxRibbonButtonBar(toolsPanel);
        toolButtonBar->AddButton(XRCID("ID_WEB_HARVEST"), _(L"Web Harvester"),
                                 ReadSvgIcon(L"ribbon/web-export.svg"),
                                 _(L"Download and analyze multiple webpages."));
#ifndef NDEBUG
        toolButtonBar->AddButton(XRCID("ID_CHAPTER_SPLIT"), _(L"Chapter Split"),
                                 ReadSvgIcon(L"ribbon/chapter-split.svg"),
                                 _(L"Split a document into smaller documents."));
        toolButtonBar->AddButton(XRCID("ID_FIND_DUPLICATE_FILES"), _(L"Find Duplicates"),
                                 ReadSvgIcon(L"ribbon/duplicate-files.svg"),
                                 _(L"Search for (and remove) duplicate files."));
#endif
        }
    }

//-----------------------------------
void ReadabilityApp::LoadRibbonDocumentPage(wxRibbonBar* ribbon, const RibbonType rtype)
    {
    if (rtype != RibbonType::MainFrameRibbon)
        {
        // Document tab
        auto* documentPage = new wxRibbonPage(ribbon, wxID_ANY, _(L"Document"));
        // proofing section
        auto* proofingPanel =
            new wxRibbonPanel(documentPage, wxID_ANY, _(L"Proofing"), wxNullBitmap,
                              wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        auto* proofingButtonBar =
            new wxRibbonButtonBar(proofingPanel, MainFrame::ID_PROOFING_RIBBON_BUTTON_BAR);
        proofingButtonBar->AddButton(XRCID("ID_LAUNCH_SOURCE_FILE"), _(L"Edit Document"),
                                     ReadSvgIcon(L"ribbon/edit-document.svg"),
                                     _(L"Edit the document that is being analyzed."));
        proofingButtonBar->AddHybridButton(XRCID("ID_EDIT_DICTIONARY"), _(L"Spell Checker"),
                                           ReadSvgIcon(L"ribbon/misspellings.svg"),
                                           _(L"Edit the spell checker's dictionary."));
        // sentence section
        auto* sentencePanel =
            new wxRibbonPanel(documentPage, wxID_ANY, _(L"Sentences"), wxNullBitmap,
                              wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        auto* sentenceButtonBar = new wxRibbonButtonBar(sentencePanel);
        sentenceButtonBar->AddDropdownButton(
            XRCID("ID_SENTENCE_LENGTHS"), _(L"Long Sentences"),
            ReadSvgIcon(L"ribbon/long-sentence.svg"),
            _(L"Control how overly long sentences are determined."));
        // sentence/paragraph deduction
        auto* deductionPanel = new wxRibbonPanel(
            documentPage, wxID_ANY, _(L"Sentence & Paragraph Deduction"), wxNullBitmap,
            wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        auto* deductionButtonBar = new wxRibbonButtonBar(
            deductionPanel, MainFrame::ID_PARAGRAPH_DEDUCTION_RIBBON_BUTTON_BAR);
        deductionButtonBar->AddDropdownButton(
            XRCID("ID_LINE_ENDS"), _(L"Line Ends"), ReadSvgIcon(L"ribbon/paragraph.svg"),
            _(L"Control how line ends effect paragraph detection."));
        deductionButtonBar->AddToggleButton(
            XRCID("ID_IGNORE_BLANK_LINES"), _(L"Ignore Blank Lines"),
            ReadSvgIcon(L"ribbon/blank-lines.svg"),
            _(L"Control whether blank lines should affect how paragraph breaks are detected."));
        deductionButtonBar->AddToggleButton(
            XRCID("ID_IGNORE_INDENTING"), _(L"Ignore Indenting"),
            ReadSvgIcon(L"ribbon/indenting.svg"),
            _(L"Control whether indenting should affect how paragraph breaks are detected."));
        deductionButtonBar->AddToggleButton(
            XRCID("ID_SENTENCES_CAPITALIZED"), _(L"Strict Capitalization"),
            ReadSvgIcon(L"ribbon/capital-letter.svg"),
            _(L"Change whether sentences must begin with capital letters "
              "when determining sentence breaks."));
        // text exclusion
        auto* exclusionPanel =
            new wxRibbonPanel(documentPage, wxID_ANY, _(L"Text Exclusion"), wxNullBitmap,
                              wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        auto* exclusionButtonBar =
            new wxRibbonButtonBar(exclusionPanel, MainFrame::ID_TEXT_EXCLUSION_RIBBON_BUTTON_BAR);
        exclusionButtonBar->AddDropdownButton(
            XRCID("ID_TEXT_EXCLUSION"), _(L"Exclusion"),
            ReadSvgIcon(L"ribbon/proofreading-delete.svg"),
            _(L"Select how text should be excluded from the analysis."));
        exclusionButtonBar->AddButton(XRCID("ID_INCOMPLETE_THRESHOLD"), _(L"Incomplete Threshold"),
                                      ReadSvgIcon(L"ribbon/period-needed.svg"),
                                      _(L"Specify the minimum length of an incomplete sentence "
                                        "that should be considered valid."));
        exclusionButtonBar->AddToggleButton(XRCID("ID_EXCLUDE_AGGRESSIVELY"), _(L"Aggressive"),
                                            ReadSvgIcon(L"ribbon/aggressive-list.svg"),
                                            _(L"Aggressively excludes list items, citations, "
                                              "and copyright notices (if applicable)."));
        exclusionButtonBar->AddToggleButton(
            XRCID("ID_EXCLUDE_COPYRIGHT_NOTICES"), _(L"Copyrights"),
            ReadSvgIcon(L"ribbon/ignore-copyright.svg"),
            _(L"Exclude trailing copyright statements from the analysis."));
        exclusionButtonBar->AddToggleButton(XRCID("ID_EXCLUDE_TRAILING_CITATIONS"), _(L"Citations"),
                                            ReadSvgIcon(L"ribbon/citation.svg")),
            _(L"Exclude trailing citations from the analysis.");
        exclusionButtonBar->AddToggleButton(
            XRCID("ID_EXCLUDE_FILE_ADDRESSES"), _(L"File Names"),
            ReadSvgIcon(L"ribbon/internet.svg"),
            _(L"Exclude file names and website addresses from the analysis."));
        exclusionButtonBar->AddToggleButton(XRCID("ID_EXCLUDE_NUMERALS"), _(L"Numerals"),
                                            ReadSvgIcon(L"ribbon/ignore-numerals.svg"),
                                            _(L"Exclude numbers from the analysis."));
        exclusionButtonBar->AddToggleButton(XRCID("ID_EXCLUDE_PROPER_NOUNS"), _(L"Proper Nouns"),
                                            ReadSvgIcon(L"ribbon/person.svg"),
                                            _(L"Exclude proper names from the analysis."));
        exclusionButtonBar->AddButton(
            XRCID("ID_EXCLUDE_WORD_LIST"), _(L"Exclude Words"),
            ReadSvgIcon(L"ribbon/exclusion-list.svg"),
            _(L"Add a custom list of words and phrases to exclude from the analysis."));
        exclusionButtonBar->AddDropdownButton(
            XRCID("ID_EXCLUSION_TAGS"), _(L"Exclusion Tags"),
            ReadSvgIcon(L"ribbon/exclusion-tags.svg"),
            _(L"Specify tags that will exclude all text between them."));
        // numeral syllabizing
        auto* numeralsPanel =
            new wxRibbonPanel(documentPage, wxID_ANY, _(L"Numerals"), wxNullBitmap,
                              wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        auto* numeralsButtonBar =
            new wxRibbonButtonBar(numeralsPanel, MainFrame::ID_NUMERALS_RIBBON_BUTTON_BAR);
        numeralsButtonBar->AddDropdownButton(
            XRCID("ID_NUMERAL_SYLLABICATION"), _(L"Syllabication"),
            ReadSvgIcon(L"ribbon/number-syllabize.svg"),
            _(L"Specify how syllables should be counted for numbers."));
        }
    }

//-----------------------------------
void ReadabilityApp::LoadRibbonReadabilityPage(wxRibbonBar* ribbon, RibbonType rtype)
    {
    if (rtype != RibbonType::MainFrameRibbon)
        {
        auto* testsPage = new wxRibbonPage(ribbon, wxID_ANY, _(L"Readability"), wxNullBitmap);
        auto* standardTestsPanel =
            new wxRibbonPanel(testsPage, wxID_ANY, _(L"Tests"), wxNullBitmap, wxDefaultPosition,
                              wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        auto* standardTestsBar = new wxRibbonButtonBar(standardTestsPanel);

        standardTestsBar->AddDropdownButton(XRCID("ID_PRIMARY_AGE_TESTS_BUTTON"), _(L"Primary"),
                                            ReadSvgIcon(L"tests/dolch.svg"),
                                            _(L"Tests meant for primary-age materials."));
        standardTestsBar->AddDropdownButton(XRCID("ID_SECONDARY_AGE_TESTS_BUTTON"), _(L"Secondary"),
                                            ReadSvgIcon(L"ribbon/secondary.svg"),
                                            _(L"Tests meant for secondary-age materials."));
        standardTestsBar->AddDropdownButton(XRCID("ID_ADULT_TESTS_BUTTON"), _(L"Adult"),
                                            ReadSvgIcon(L"ribbon/adult.svg"),
                                            _(L"Tests meant for adult-level materials."));
        standardTestsBar->AddDropdownButton(
            XRCID("ID_SECOND_LANGUAGE_TESTS_BUTTON"), _(L"Second Language"),
            ReadSvgIcon(L"ribbon/esl.svg"),
            _(L"Tests for materials designed for second-language readers."));
        standardTestsBar->AddDropdownButton(XRCID("ID_CUSTOM_TESTS"), _(L"Custom"),
                                            ReadSvgIcon(L"ribbon/formula.svg"),
                                            _(L"Create or edit custom tests."));
        standardTestsBar->AddDropdownButton(XRCID("ID_TEST_BUNDLES"), _(L"Bundles"),
                                            ReadSvgIcon(L"ribbon/bundles.svg"),
                                            _(L"Add multiple tests to a project at once."));
        standardTestsBar->AddButton(XRCID("ID_REMOVE_TEST"), _(L"Remove"),
                                    ReadSvgIcon(L"ribbon/delete.svg"),
                                    _(L"Remove the selected test from the project."));
        // readability tools section
        auto* readabilityToolsPanel =
            new wxRibbonPanel(testsPage, wxID_ANY, _(L"Tools"), wxNullBitmap, wxDefaultPosition,
                              wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        auto* readabilityToolBar = new wxRibbonButtonBar(readabilityToolsPanel);
        readabilityToolBar->AddButton(XRCID("ID_TESTS_OVERVIEW"), _(L"Tests Overview"),
                                      ReadSvgIcon(L"ribbon/tests-overview.svg"),
                                      _(L"View information about each readability test."));
        if (rtype == RibbonType::BatchProjectRibbon)
            {
            readabilityToolBar->AddToggleButton(
                XRCID("ID_TEST_EXPLANATIONS_WINDOW"), _(L"Test Explanations"),
                ReadSvgIcon(L"ribbon/formula.svg"),
                _(L"Read an explanation of the selected test score."));
            }
        readabilityToolBar->AddHybridButton(XRCID("ID_WORD_LISTS"), _(L"Word Lists"),
                                            ReadSvgIcon(L"tests/dale-chall-test.svg"),
                                            _(L"View the word lists used by readability tests."));
        readabilityToolBar->AddDropdownButton(
            XRCID("ID_BLANK_GRAPHS"), _(L"Blank Graphs"), ReadSvgIcon(L"ribbon/blank-graphs.svg"),
            _(L"Print or save blank readability graph templates."));
        }
    }

//-----------------------------------
void ReadabilityApp::LoadRibbonToolsPage(wxRibbonBar* ribbon, RibbonType rtype)
    {
    if (rtype != RibbonType::MainFrameRibbon)
        {
        auto* toolsPage = new wxRibbonPage(ribbon, wxID_ANY, _(L"Tools"));
        auto* toolsPanel =
            new wxRibbonPanel(toolsPage, wxID_ANY, _(L"Tools & Settings"), wxNullBitmap,
                              wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
        auto* toolButtonBar =
            new wxRibbonButtonBar(toolsPanel, BaseProjectView::RIBBON_TOOLS_BUTTON_BAR_ID);
        toolButtonBar->AddButton(XRCID("ID_WEB_HARVEST"), _(L"Web Harvester"),
                                 ReadSvgIcon(L"ribbon/web-export.svg"),
                                 _(L"Download and analyze multiple webpages."));
#ifndef NDEBUG
        toolButtonBar->AddButton(XRCID("ID_CHAPTER_SPLIT"), _(L"Chapter Split"),
                                 ReadSvgIcon(L"ribbon/chapter-split.svg"),
                                 _(L"Split a document into smaller documents."));
        toolButtonBar->AddButton(XRCID("ID_FIND_DUPLICATE_FILES"), _(L"Find Duplicates"),
                                 ReadSvgIcon(L"ribbon/duplicate-files.svg"),
                                 _(L"Search for and remove duplicate files."));
#endif
        toolButtonBar->AddButton(wxID_PREFERENCES, _(L"Options"),
                                 ReadSvgIcon(L"ribbon/configure.svg"),
                                 _(L"Change the program's general options."));
        toolButtonBar->AddButton(XRCID("ID_VIEW_LOG_REPORT"), _(L"Log Report"),
                                 ReadSvgIcon(L"ribbon/log-book.svg"));
        if (GetAppOptions()->IsShowingDeveloperTab())
            {
            toolButtonBar->AddButton(XRCID("ID_SCRIPT_WINDOW"), _(L"Developer Tools"),
                                     ReadSvgIcon(L"ribbon/dev-tools.svg"));
            }
        }
    }

//-----------------------------------
void ReadabilityApp::LoadRibbonHelpPage(wxRibbonBar* ribbon)
    {
    auto* helpPage = new wxRibbonPage(ribbon, wxID_ANY, _(L"Help"));
    auto* helpPanel =
        new wxRibbonPanel(helpPage, wxID_ANY, _(L"Documentation"), wxNullBitmap, wxDefaultPosition,
                          wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE);
    auto* helpButtonBar = new wxRibbonButtonBar(helpPanel);
    helpButtonBar->AddButton(wxID_HELP, _(L"Electronic Help"),
                             ReadSvgIcon(L"ribbon/electronic-help.svg"),
                             _(L"Read the documentation in a browser."));
    helpButtonBar->AddButton(XRCID("ID_HELP_MANUAL"), _(L"Manual"), ReadSvgIcon(L"ribbon/help.svg"),
                             _(L"Read the manual."));
    helpButtonBar->AddButton(XRCID("ID_TESTS_REFERENCE"), _(L"Tests Reference"),
                             ReadSvgIcon(L"ribbon/tests-overview.svg"),
                             _(L"Read the readability tests reference."));
    helpButtonBar->AddButton(XRCID("ID_SHORTCUTS_CHEATSHEET"), _(L"Shortcuts"),
                             ReadSvgIcon(L"ribbon/keyboard-shortcuts.svg"),
                             _(L"Read the keyboard shortcuts cheatsheet."));
    helpButtonBar->AddButton(XRCID("ID_RELEASE_NOTES"), _(L"Release Notes"),
                             ReadSvgIcon(L"ribbon/paper-notes.svg"), _(L"Read the release notes."));
    helpButtonBar->AddButton(XRCID("ID_PROGRAMMING_MANUAL"), _(L"Programming Manual"),
                             ReadSvgIcon(wxSystemSettings::GetAppearance().IsDark() ?
                                             L"ribbon/lua-dark-mode.svg" :
                                             L"ribbon/lua.svg"),
                             _(L"Read the Lua programming manual."));
    helpButtonBar->AddButton(XRCID("ID_SYSADMIN"), _(L"System Admin Manual"),
                             ReadSvgIcon(L"ribbon/system-admin.svg"),
                             _(L"Read the system administrator manual."));
    helpButtonBar->AddDropdownButton(XRCID("ID_EXAMPLES"), _(L"Example Documents"),
                                     ReadSvgIcon(L"ribbon/examples.svg"),
                                     _(L"Analyze example documents from the help."));

    auto* supportPanel = new wxRibbonPanel(helpPage, wxID_ANY, _(L"Support"), wxNullBitmap);
    auto* supportButtonBar = new wxRibbonButtonBar(supportPanel);
#ifndef APP_STORE_BUILD
    supportButtonBar->AddButton(XRCID("ID_CHECK_FOR_UPDATES"), _(L"Updates"),
                                ReadSvgIcon(L"ribbon/updates.svg"), _(L"Check for updates."));
#endif
    supportButtonBar->AddButton(XRCID("ID_SUPPORT"), _(L"Support"),
                                ReadSvgIcon(L"ribbon/support.svg"), _(L"Contact support."));
    supportButtonBar->AddButton(wxID_ABOUT, _(L"About"), ReadSvgIcon(L"ribbon/app-logo.svg"),
                                _(L"Learn more about the program."));
    }

//-----------------------------------
void ReadabilityApp::LoadRibbonDeveloperPage(wxRibbonBar* ribbon)
    {
    const bool darkMode = wxSystemSettings::GetAppearance().IsDark();
    GetMainFrameEx()->m_developerRibbonPage = new wxRibbonPage(ribbon, wxID_ANY, _(L"Developer"));

    auto* scriptBar = new wxRibbonButtonBar(new wxRibbonPanel(
        GetMainFrameEx()->GetDeveloperRibbonPage(), wxID_ANY, _(L"Script"), wxNullBitmap,
        wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE));
    scriptBar->AddButton(XRCID("ID_SCRIPT_NEW"), _(L"New"), ReadSvgIcon(L"ribbon/document.svg"),
                         _(L"Create a new script."));
    scriptBar->AddButton(XRCID("ID_SCRIPT_OPEN"), _(L"Open"), ReadSvgIcon(L"ribbon/file-open.svg"),
                         _(L"Open a script."));
    scriptBar->AddButton(XRCID("ID_SCRIPT_SAVE"), _(L"Save"), ReadSvgIcon(L"ribbon/file-save.svg"),
                         _(L"Save the script."));

    auto* runBar = new wxRibbonButtonBar(new wxRibbonPanel(
        GetMainFrameEx()->GetDeveloperRibbonPage(), wxID_ANY, _(L"Run"), wxNullBitmap,
        wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE));
    runBar->AddButton(XRCID("ID_SCRIPT_RUN"), _(L"Run"), ReadSvgIcon(L"ribbon/run.svg"),
                      _(L"Execute the script (or selection)."));
    runBar->AddButton(XRCID("ID_SCRIPT_CONTINUE"), _(L"Continue"),
                      ReadSvgIcon(L"ribbon/continue.svg"),
                      _(L"Continue running the script from the current breakpoint."));
    runBar->AddButton(XRCID("ID_SCRIPT_STOP"), _(L"Stop"), ReadSvgIcon(L"ribbon/stop.svg"),
                      _(L"Stop the currently running script."));
    runBar->AddButton(
        XRCID("ID_SCRIPT_CLEAR_GLOBALS"), _(L"Clear Globals"), ReadSvgIcon(L"ribbon/reload.svg"),
        _(L"Restart the Lua interpreter, clearing all global variables from previous runs."));

    auto* clipboardBar = new wxRibbonButtonBar(new wxRibbonPanel(
        GetMainFrameEx()->GetDeveloperRibbonPage(), wxID_ANY, _(L"Clipboard"), wxNullBitmap,
        wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE));
    clipboardBar->AddButton(XRCID("ID_SCRIPT_PASTE"), _(L"Paste"), ReadSvgIcon(L"ribbon/paste.svg"),
                            _(L"Paste."));
    clipboardBar->AddButton(XRCID("ID_SCRIPT_CUT"), _(L"Cut"), ReadSvgIcon(L"ribbon/cut.svg"),
                            _(L"Cut."));
    clipboardBar->AddButton(XRCID("ID_SCRIPT_COPY"), _(L"Copy"), ReadSvgIcon(L"ribbon/copy.svg"),
                            _(L"Copy."));

    auto* editBar = new wxRibbonButtonBar(new wxRibbonPanel(
        GetMainFrameEx()->GetDeveloperRibbonPage(), wxID_ANY, _(L"Edit"), wxNullBitmap,
        wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE));
    editBar->AddButton(XRCID("ID_SCRIPT_UNDO"), _(L"Undo"), ReadSvgIcon(L"ribbon/undo.svg"),
                       _(L"Undo."));
    editBar->AddButton(XRCID("ID_SCRIPT_REDO"), _(L"Redo"), ReadSvgIcon(L"ribbon/redo.svg"),
                       _(L"Redo."));
    editBar->AddButton(XRCID("ID_SCRIPT_DUPLICATE_LINE"), _(L"Duplicate Line"),
                       ReadSvgIcon(L"ribbon/duplicate-files.svg"),
                       _(L"Duplicate the current line."));
    editBar->AddButton(XRCID("ID_SCRIPT_SELECT_ALL"), _(L"Select All"),
                       ReadSvgIcon(L"ribbon/select-all.svg"), _(L"Select all text."));
    editBar->AddButton(XRCID("ID_SCRIPT_COMMENT"), _(L"Comment"),
                       ReadSvgIcon(L"ribbon/comment.svg"), _(L"Comment the selected lines."));
    editBar->AddButton(XRCID("ID_SCRIPT_UNCOMMENT"), _(L"Uncomment"),
                       ReadSvgIcon(L"ribbon/uncomment.svg"), _(L"Uncomment the selected lines."));

    auto* findBar = new wxRibbonButtonBar(new wxRibbonPanel(
        GetMainFrameEx()->GetDeveloperRibbonPage(), wxID_ANY, _(L"Find"), wxNullBitmap,
        wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE));
    findBar->AddButton(XRCID("ID_SCRIPT_FIND"), _(L"Find"), ReadSvgIcon(L"ribbon/find.svg"),
                       _(L"Find text."));
    findBar->AddButton(XRCID("ID_SCRIPT_REPLACE"), _(L"Replace"),
                       ReadSvgIcon(L"ribbon/find-replace.svg"), _(L"Replace text."));
    findBar->AddButton(XRCID("ID_SCRIPT_GOTO_LINE"), _(L"Go To Line"),
                       ReadSvgIcon(L"ribbon/go-to-line.svg"), _(L"Go to a specific line."));

    auto* debugBar = new wxRibbonButtonBar(new wxRibbonPanel(
        GetMainFrameEx()->GetDeveloperRibbonPage(), wxID_ANY, _(L"Debug"), wxNullBitmap,
        wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE));
    debugBar->AddButton(XRCID("ID_SCRIPT_TOGGLE_DEBUG"), _(L"Debug Pane"),
                        ReadSvgIcon(L"ribbon/ladybug.svg"), _(L"Toggle the debug windows."),
                        wxRIBBON_BUTTON_TOGGLE);
    debugBar->AddButton(XRCID("ID_SCRIPT_CLEAR_DEBUG"), _(L"Clear"),
                        ReadSvgIcon(L"ribbon/clear.svg"), _(L"Clear the log window."));

    auto* refBar = new wxRibbonButtonBar(new wxRibbonPanel(
        GetMainFrameEx()->GetDeveloperRibbonPage(), wxID_ANY, _(L"Reference"), wxNullBitmap,
        wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE));
    refBar->AddButton(XRCID("ID_SCRIPT_FUNCTION_BROWSER"), _(L"Function Browser"),
                      ReadSvgIcon(darkMode ? L"ribbon/function-dark.svg" : L"ribbon/function.svg"),
                      _(L"View the function browser."), wxRIBBON_BUTTON_TOGGLE);
    refBar->AddButton(XRCID("ID_SCRIPT_API_PDF"),
                      /* TRANSLATORS: Application Programming Interface */ _(L"API"),
                      ReadSvgIcon(L"ribbon/electronic-help.svg"), _(L"View the documentation."));
    refBar->AddButton(XRCID("ID_SCRIPT_LUA_REFERENCE"), _(L"Lua Reference"),
                      ReadSvgIcon(darkMode ? L"ribbon/lua-dark-mode.svg" : L"ribbon/lua.svg"),
                      _(L"View the Lua Reference Manual."));
    }

//-----------------------------------
void ReadabilityApp::LoadRibbonLogPage(wxRibbonBar* ribbon)
    {
    GetMainFrameEx()->m_logRibbonPage = new wxRibbonPage(ribbon, wxID_ANY, _(L"Log"));

    auto* exportBar = new wxRibbonButtonBar(new wxRibbonPanel(
        GetMainFrameEx()->GetLogRibbonPage(), wxID_ANY, _(L"Export"), wxNullBitmap,
        wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE));
    exportBar->AddButton(XRCID("ID_LOG_TAB_SAVE"), _(L"Save"), ReadSvgIcon(L"ribbon/file-save.svg"),
                         _(L"Save the log report."));
    exportBar->AddButton(XRCID("ID_LOG_TAB_PRINT"), _(L"Print"), ReadSvgIcon(L"ribbon/print.svg"),
                         _(L"Print the log report."));

    GetMainFrameEx()->m_logEditButtonBar = new wxRibbonButtonBar(
        new wxRibbonPanel(GetMainFrameEx()->GetLogRibbonPage(), wxID_ANY, _(L"Edit"), wxNullBitmap,
                          wxDefaultPosition, wxDefaultSize, wxRIBBON_PANEL_NO_AUTO_MINIMISE));
    GetMainFrameEx()->m_logEditButtonBar->AddButton(XRCID("ID_LOG_TAB_COPY"), _(L"Copy Selection"),
                                                    ReadSvgIcon(L"ribbon/copy.svg"),
                                                    _(L"Copy the selected items."));
    GetMainFrameEx()->m_logEditButtonBar->AddButton(
        XRCID("ID_LOG_TAB_SELECT_ALL"), _(L"Select All"), ReadSvgIcon(L"ribbon/select-all.svg"),
        _(L"Select the entire list."));
    GetMainFrameEx()->m_logEditButtonBar->AddButton(XRCID("ID_LOG_TAB_SORT"), _(L"Sort"),
                                                    ReadSvgIcon(L"ribbon/sort.svg"),
                                                    _(L"Sort the list."));
    GetMainFrameEx()->m_logEditButtonBar->AddButton(XRCID("ID_LOG_TAB_CLEAR"), _(L"Clear"),
                                                    ReadSvgIcon(L"ribbon/clear.svg"),
                                                    _(L"Clear the log report."));
    GetMainFrameEx()->m_logEditButtonBar->AddButton(XRCID("ID_LOG_TAB_REFRESH"), _(L"Refresh"),
                                                    ReadSvgIcon(L"ribbon/reload.svg"),
                                                    _(L"Refresh the log report."));
    GetMainFrameEx()->m_logEditButtonBar->AddToggleButton(
        XRCID("ID_LOG_TAB_REALTIME_UPDATE"), _(L"Auto Refresh"),
        ReadSvgIcon(L"ribbon/realtime.svg"), _(L"Refresh the log report automatically."));
    GetMainFrameEx()->m_logEditButtonBar->AddToggleButton(
        XRCID("ID_LOG_TAB_VERBOSE"), _(L"Verbose"), ReadSvgIcon(L"ribbon/info.svg"),
        _(L"Toggles whether the logging system includes more detailed information."));
    }

//-----------------------------------
wxRibbonBar* ReadabilityApp::CreateRibbon(wxWindow* frame, const wxDocument* doc)
    {
    const RibbonType rtype = (doc == nullptr) ? RibbonType::MainFrameRibbon :
                             doc->IsKindOf(wxCLASSINFO(ProjectDoc)) ?
                                                RibbonType::StandardProjectRibbon :
                                                RibbonType::BatchProjectRibbon;

    auto* ribbon = new wxRibbonBar(frame, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                                   wxRIBBON_BAR_SHOW_PAGE_ICONS | wxRIBBON_BAR_DEFAULT_STYLE);
    LoadRibbonHomePage(ribbon, rtype, doc);
    LoadRibbonDocumentPage(ribbon, rtype);
    LoadRibbonReadabilityPage(ribbon, rtype);
    LoadRibbonToolsPage(ribbon, rtype);
    if (rtype == RibbonType::MainFrameRibbon)
        {
        LoadRibbonDeveloperPage(ribbon);
        if (!GetAppOptions()->IsShowingDeveloperTab())
            {
            ribbon->HidePage(ribbon->GetPageNumber(GetMainFrameEx()->GetDeveloperRibbonPage()));
            }
        LoadRibbonLogPage(ribbon);
        if (!GetAppOptions()->IsShowingLogTab())
            {
            ribbon->HidePage(ribbon->GetPageNumber(GetMainFrameEx()->GetLogRibbonPage()));
            }
        }
    LoadRibbonHelpPage(ribbon);

    ribbon->SetArtProvider(new wxRibbonMSWFlatArtProvider);

    ribbon->Realize();

    return ribbon;
    }

//---------------------------------------------------
void ReadabilityApp::RemoveAllCustomTestBundles()
    {
    std::ranges::for_each(BaseProject::m_testBundles,
                          [this](const TestBundle& bundle)
                          {
                              if (!bundle.IsLocked())
                                  {
                                  GetMainFrameEx()->RemoveTestBundleFromMenus(
                                      bundle.GetName().c_str());
                                  }
                          });

    for (auto bundle = BaseProject::m_testBundles.begin();
         bundle != BaseProject::m_testBundles.end();
         /*in loop*/)
        {
        if (bundle->IsLocked())
            {
            ++bundle;
            }
        else
            {
            bundle = BaseProject::m_testBundles.erase(bundle);
            }
        }
    }

wxIMPLEMENT_CLASS(MainFrame, Wisteria::UI::BaseMainFrame);

std::map<int, wxString> MainFrame::m_testBundleMenuIds;
std::map<int, wxString> MainFrame::m_customTestMenuIds;
std::map<int, wxString> MainFrame::m_examplesMenuIds;

//-------------------------------------------------------
void MainFrame::OnAbout([[maybe_unused]] wxCommandEvent& event)
    {
    wxDateTime buildDate;
    buildDate.ParseDate(__DATE__);

    wxString filePath = wxGetApp().FindResourceFile(_DT(L"LICENSE"));
    wxString eula;
    if (!(wxFile::Exists(filePath) && Wisteria::TextStream::ReadFile(filePath, eula)))
        {
        wxLogMessage(L"Unable to read EULA from '%s'", filePath);
        }

    filePath = wxGetApp().FindResourceFile(_DT(L"citations.mla"));
    wxString mlaCitation;
    if (!(wxFile::Exists(filePath) && Wisteria::TextStream::ReadFile(filePath, mlaCitation)))
        {
        wxLogMessage(L"Unable to read MLA citation from '%s'", filePath);
        }

    filePath = wxGetApp().FindResourceFile(_DT(L"citations.apa"));
    wxString apaCitation;
    if (!(wxFile::Exists(filePath) && Wisteria::TextStream::ReadFile(filePath, apaCitation)))
        {
        wxLogMessage(L"Unable to read APA citation from '%s'", filePath);
        }

    filePath = wxGetApp().FindResourceFile(_DT(L"citations.bib"));
    wxString bibTexCitation;
    if (!(wxFile::Exists(filePath) && Wisteria::TextStream::ReadFile(filePath, bibTexCitation)))
        {
        wxLogMessage(L"Unable to read BibTeX citation from '%s'", filePath);
        }

    AboutDialogEx aboutDlg(
        wxGetApp().GetParentingWindow(), wxGetApp().GetAppVersion(),
        // TRANSLATORS: "%s" is the software publisher.
        wxString::Format(_(L"Copyright ©2006–2025 Oleander Software, Ltd.\n"
                           "Copyright ©2025–%d %s.\nAll rights reserved.\n\n"
                           "%s® is a Trademark of %s.\n"
                           "Jakarta and Jakarta EE are Trademarks of %s.\n"
                           "Eclipse® is a Trademark of %s.\n"
                           "Eclipse Foundation is a Trademark of %s."),
                         buildDate.GetYear(), wxGetApp().GetVendorDisplayName(),
                         _READSTUDIO_APP_LONG_NAME, wxGetApp().GetVendorDisplayName(),
                         wxGetApp().GetVendorDisplayName(), wxGetApp().GetVendorDisplayName(),
                         wxGetApp().GetVendorDisplayName()),
        eula, mlaCitation, apaCitation, bibTexCitation);

    aboutDlg.ShowModal();
    }

//-------------------------------------------------------
void MainFrame::OnRibbonBarHelpClicked([[maybe_unused]] wxRibbonBarEvent& event)
    {
    wxCommandEvent cmd;
    OnHelpContents(cmd);
    }

//-------------------------------------------------------
void MainFrame::ActivateLogTab()
    {
    if (GetLogRibbonPage() == nullptr)
        {
        return;
        }
    if (!IsShown())
        {
        Show();
        }
    Raise();
    GetRibbon()->SetActivePage(GetLogRibbonPage());
    if (GetStartPage() != nullptr)
        {
        GetStartPage()->Hide();
        }
    if (GetScriptWorkbench() != nullptr)
        {
        GetScriptWorkbench()->Hide();
        }
    m_logPanel->Show();
    if (m_logEditButtonBar != nullptr)
        {
        m_logEditButtonBar->ToggleButton(XRCID("ID_LOG_TAB_REALTIME_UPDATE"), m_logAutoRefresh);
        m_logEditButtonBar->ToggleButton(XRCID("ID_LOG_TAB_VERBOSE"), wxLog::GetVerbose());
        }
    Layout();
    wxGetApp().ReadLogIntoListCtrl(m_logListCtrl);
    m_logListCtrl->SetFocus();
    }

//-------------------------------------------------------
void MainFrame::SetLogAutoRefresh(const bool enable)
    {
    m_logAutoRefresh = enable;
    if (m_logEditButtonBar != nullptr)
        {
        m_logEditButtonBar->ToggleButton(XRCID("ID_LOG_TAB_REALTIME_UPDATE"), enable);
        }
    if (enable && IsLogTabActive())
        {
        m_logAutoRefreshTimer.Start(3000);
        }
    else
        {
        m_logAutoRefreshTimer.Stop();
        }
    }

//-------------------------------------------------------
void MainFrame::OnTestsOverview([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    const wxSize screenSize{ wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_X),
                             wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_Y) };
    // test overview dialog
    Wisteria::UI::ListDlg testsOverviewDlg(
        wxGetApp().GetParentingWindow(),
        Wisteria::UI::LD_SAVE_BUTTON | Wisteria::UI::LD_COPY_BUTTON |
            Wisteria::UI::LD_PRINT_BUTTON | Wisteria::UI::LD_SELECT_ALL_BUTTON |
            Wisteria::UI::LD_COLUMN_HEADERS | Wisteria::UI::LD_FIND_BUTTON |
            Wisteria::UI::LD_SORT_BUTTON,
        wxID_ANY, _(L"Readability Tests Overview"), wxString{}, wxDefaultPosition,
        wxSize{ static_cast<int>(screenSize.GetWidth() * math_constants::three_quarters),
                static_cast<int>(screenSize.GetHeight() * math_constants::half) });

    testsOverviewDlg.GetListCtrl()->ClearAll();
    testsOverviewDlg.GetListCtrl()->InsertColumn(0, _(L"Name"));
    testsOverviewDlg.GetListCtrl()->InsertColumn(1, _(L"Score Type"));
    testsOverviewDlg.GetListCtrl()->InsertColumn(2, _(L"Languages"));
    testsOverviewDlg.GetListCtrl()->InsertColumn(3, _(L"Word Complexity"));
    testsOverviewDlg.GetListCtrl()->InsertColumn(4, _(L"Word Length"));
    testsOverviewDlg.GetListCtrl()->InsertColumn(5, _(L"Word Familiarity"));
    testsOverviewDlg.GetListCtrl()->InsertColumn(6, _(L"Sentence Length"));
    testsOverviewDlg.GetListCtrl()->InsertColumn(7, _(L"Description"));
    testsOverviewDlg.GetListCtrl()->SetVirtualDataSize(
        BaseProject::GetDefaultReadabilityTestsTemplate().get_tests().size(), 8);
    size_t i = 0;
    for (auto testPos = BaseProject::GetDefaultReadabilityTestsTemplate().get_tests().begin();
         testPos != BaseProject::GetDefaultReadabilityTestsTemplate().get_tests().end();
         ++testPos, ++i)
        {
        // test type
        testsOverviewDlg.GetListCtrl()->SetItemText(i, 0,
                                                    wxString(testPos->get_long_name().c_str()));
        switch (testPos->get_test_type())
            {
        case readability::readability_test_type::grade_level:
            testsOverviewDlg.GetListCtrl()->SetItemText(i, 1, _(L"Grade level"));
            break;
        case readability::readability_test_type::index_value:
            testsOverviewDlg.GetListCtrl()->SetItemText(i, 1, _(L"Index score"));
            break;
        case readability::readability_test_type::index_value_and_grade_level:
            testsOverviewDlg.GetListCtrl()->SetItemText(i, 1, _(L"Grade level and index score"));
            break;
        case readability::readability_test_type::predicted_cloze_score:
            testsOverviewDlg.GetListCtrl()->SetItemText(i, 1, _(L"Cloze score"));
            break;
        case readability::readability_test_type::grade_level_and_predicted_cloze_score:
            testsOverviewDlg.GetListCtrl()->SetItemText(i, 1, _(L"Grade level and cloze score"));
            break;
        default:
            testsOverviewDlg.GetListCtrl()->SetItemText(i, 1, _(L"Grade level"));
            };
        // language
        wxString languages;
        if (testPos->has_language(readability::test_language::english_test))
            {
            languages += _(L"English") + _DT(L"/");
            }
        if (testPos->has_language(readability::test_language::spanish_test))
            {
            languages += _(L"Spanish") + _DT(L"/");
            }
        if (testPos->has_language(readability::test_language::german_test))
            {
            languages += _(L"German") + _DT(L"/");
            }
        if (!languages.empty())
            {
            languages.RemoveLast();
            }
        testsOverviewDlg.GetListCtrl()->SetItemText(i, 2, languages);
        const auto padCell = [](const auto& cellValue) { return L" " + cellValue; };
        // word complexity
        const wxString wordComplexity =
            testPos->has_factor(readability::test_factor::word_complexity_2_plus_syllables) ?
                padCell(_(L"X (2 or more syllables)")) :
            testPos->has_factor(readability::test_factor::word_complexity_3_plus_syllables) ?
                padCell(_(L"X (3 or more syllables)")) :
            testPos->has_factor(readability::test_factor::word_complexity_density) ?
                padCell(_(L"X (syllable density)")) :
            testPos->has_factor(readability::test_factor::word_complexity) ? wxString(_DT(L" X ")) :
                                                                             wxString{};
        testsOverviewDlg.GetListCtrl()->SetItemText(i, 3, wordComplexity);
        // word length
        const wxString wordLength =
            testPos->has_factor(readability::test_factor::word_length_3_less) ?
                padCell(_(L"X (3 or less characters)")) :
            testPos->has_factor(readability::test_factor::word_length_6_plus) ?
                padCell(_(L"X (6 or more characters)")) :
            testPos->has_factor(readability::test_factor::word_length_7_plus) ?
                padCell(_(L"X (7 or more characters)")) :
            testPos->has_factor(readability::test_factor::word_length) ? wxString(_DT(L" X ")) :
                                                                         wxString{};
        testsOverviewDlg.GetListCtrl()->SetItemText(i, 4, wordLength);
        // word familiarity
        const wxString wordFamiliarity =
            testPos->has_factor(readability::test_factor::word_familiarity_spache) ?
                padCell(_(L"X (Spache rules)")) :
            testPos->has_factor(readability::test_factor::word_familiarity_dale_chall) ?
                padCell(_(L"X (Dale-Chall rules)")) :
            testPos->has_factor(readability::test_factor::word_familiarity_harris_jacobson) ?
                padCell(_(L"X (Harris-Jacobson rules)")) :
                wxString{};
        testsOverviewDlg.GetListCtrl()->SetItemText(i, 5, wordFamiliarity);
        // sentence length
        testsOverviewDlg.GetListCtrl()->SetItemText(
            i, 6,
            testPos->has_factor(readability::test_factor::sentence_length) ?
                wxString{ _DT(L" X ") } :
                wxString{});
        // description
        lily_of_the_valley::html_extract_text filterHtml; // NOLINT(misc-const-correctness)
        testsOverviewDlg.GetListCtrl()->SetItemText(
            i, 7,
            wxString(filterHtml(testPos->get_description().c_str(),
                                testPos->get_description().length(), true, false)));
        }
    // fit the columns
    testsOverviewDlg.GetListCtrl()->DistributeColumns(-1);

    testsOverviewDlg.ShowModal();
    }

//-------------------------------------------------------
void MainFrame::OnBlankGraph(const wxCommandEvent& event)
    {
    BaseProject project;
    if (event.GetId() == XRCID("ID_BLANK_FRASE_GRAPH"))
        {
        Wisteria::UI::GraphDlg graphDlg(wxGetApp().GetParentingWindow(), wxID_ANY,
                                        wxString::Format(_(L"Blank \"%s\" Graph"), _DT(L"FRASE")));
        graphDlg.GetCanvas()->SetFixedObject(
            0, 0, std::make_shared<Wisteria::Graphs::FraseGraph>(graphDlg.GetCanvas()));
        wxGetApp().GetAppOptions()->UpdateGraphOptions(graphDlg.GetCanvas());
        graphDlg.GetCanvas()->ResetResizeDelay();
        graphDlg.ShowModal();
        }
    else if (event.GetId() == XRCID("ID_BLANK_CRAWFORD_GRAPH"))
        {
        Wisteria::UI::GraphDlg graphDlg(
            wxGetApp().GetParentingWindow(), wxID_ANY,
            wxString::Format(_(L"Blank \"%s\" Graph"), _DT(L"Crawford")));
        graphDlg.GetCanvas()->SetFixedObject(
            0, 0, std::make_shared<Wisteria::Graphs::CrawfordGraph>(graphDlg.GetCanvas()));
        wxGetApp().GetAppOptions()->UpdateGraphOptions(graphDlg.GetCanvas());
        graphDlg.GetCanvas()->ResetResizeDelay();
        graphDlg.ShowModal();
        }
    else if (event.GetId() == XRCID("ID_BLANK_INFLESZ_GRAPH"))
        {
        Wisteria::UI::GraphDlg graphDlg(
            wxGetApp().GetParentingWindow(), wxID_ANY,
            wxString::Format(_(L"Blank \"%s\" Graph"), _DT(L"INFLESZ")));
        graphDlg.GetCanvas()->SetFixedObject(
            0, 0, std::make_shared<Wisteria::Graphs::InfleszScale>(graphDlg.GetCanvas()));
        wxGetApp().GetAppOptions()->UpdateGraphOptions(graphDlg.GetCanvas());
        graphDlg.GetCanvas()->ResetResizeDelay();
        graphDlg.ShowModal();
        }
    else if (event.GetId() == XRCID("ID_BLANK_FRY_GRAPH"))
        {
        Wisteria::UI::GraphDlg graphDlg(wxGetApp().GetParentingWindow(), wxID_ANY,
                                        wxString::Format(_(L"Blank \"%s\" Graph"), _DT(L"Fry")));
        auto fryGraph = std::make_shared<Wisteria::Graphs::FryGraph>(
            graphDlg.GetCanvas(), Wisteria::Graphs::FryGraph::FryGraphType::Traditional);
        // update custom settings on graph
        fryGraph->SetMessageCatalog(project.GetReadabilityMessageCatalogPtr());
        fryGraph->SetInvalidAreaColor(wxGetApp().GetAppOptions()->GetInvalidAreaColor());

        graphDlg.GetCanvas()->SetFixedObject(0, 0, fryGraph);
        wxGetApp().GetAppOptions()->UpdateGraphOptions(graphDlg.GetCanvas());
        graphDlg.GetCanvas()->ResetResizeDelay();
        graphDlg.ShowModal();
        }
    else if (event.GetId() == XRCID("ID_BLANK_GPM_GRAPH"))
        {
        Wisteria::UI::GraphDlg graphDlg(
            wxGetApp().GetParentingWindow(), wxID_ANY,
            wxString::Format(_(L"Blank \"%s\" Graph"), _DT(L"Gilliam-Peña-Mountain")));
        auto gFryGraph = std::make_shared<Wisteria::Graphs::FryGraph>(
            graphDlg.GetCanvas(), Wisteria::Graphs::FryGraph::FryGraphType::GPM);
        // update custom settings on graph
        gFryGraph->SetMessageCatalog(project.GetReadabilityMessageCatalogPtr());
        gFryGraph->SetInvalidAreaColor(wxGetApp().GetAppOptions()->GetInvalidAreaColor());

        graphDlg.GetCanvas()->SetFixedObject(0, 0, gFryGraph);
        wxGetApp().GetAppOptions()->UpdateGraphOptions(graphDlg.GetCanvas());
        graphDlg.GetCanvas()->ResetResizeDelay();
        graphDlg.ShowModal();
        }
    else if (event.GetId() == XRCID("ID_BLANK_RAYGOR_GRAPH"))
        {
        Wisteria::UI::GraphDlg graphDlg(wxGetApp().GetParentingWindow(), wxID_ANY,
                                        wxString::Format(_(L"Blank \"%s\" Graph"), _DT(L"Raygor")));
        auto raygorGraph = std::make_shared<Wisteria::Graphs::RaygorGraph>(graphDlg.GetCanvas());
        // update custom settings on graph
        raygorGraph->SetMessageCatalog(project.GetReadabilityMessageCatalogPtr());
        raygorGraph->SetInvalidAreaColor(wxGetApp().GetAppOptions()->GetInvalidAreaColor());

        graphDlg.GetCanvas()->SetFixedObject(0, 0, raygorGraph);
        wxGetApp().GetAppOptions()->UpdateGraphOptions(graphDlg.GetCanvas());
        graphDlg.GetCanvas()->ResetResizeDelay();
        graphDlg.ShowModal();
        }
    else if (event.GetId() == XRCID("ID_BLANK_FLESCH_GRAPH"))
        {
        Wisteria::UI::GraphDlg graphDlg(
            wxGetApp().GetParentingWindow(), wxID_ANY,
            wxString::Format(_(L"Blank \"%s\" Graph"), _DT(L"Flesch Reading Ease")));
        auto fleschChart = std::make_shared<Wisteria::Graphs::FleschChart>(graphDlg.GetCanvas());
        fleschChart->ShowConnectionLine(wxGetApp().GetAppOptions()->IsConnectingFleschPoints());

        graphDlg.GetCanvas()->SetFixedObject(0, 0, fleschChart);
        wxGetApp().GetAppOptions()->UpdateGraphOptions(graphDlg.GetCanvas());
        graphDlg.GetCanvas()->ResetResizeDelay();
        graphDlg.ShowModal();
        }
    else if (event.GetId() == XRCID("ID_BLANK_DB2_GRAPH"))
        {
        Wisteria::UI::GraphDlg graphDlg(
            wxGetApp().GetParentingWindow(), wxID_ANY,
            wxString::Format(_(L"Blank \"%s\" Graph"), BaseProjectView::GetDB2Label()));
        graphDlg.GetCanvas()->SetFixedObject(
            0, 0, std::make_shared<Wisteria::Graphs::DanielsonBryan2Plot>(graphDlg.GetCanvas()));
        wxGetApp().GetAppOptions()->UpdateGraphOptions(graphDlg.GetCanvas());
        graphDlg.GetCanvas()->ResetResizeDelay();
        graphDlg.ShowModal();
        }
    else if (event.GetId() == XRCID("ID_BLANK_SCHWARTZ_GRAPH"))
        {
        Wisteria::UI::GraphDlg graphDlg(
            wxGetApp().GetParentingWindow(), wxID_ANY,
            wxString::Format(_(L"Blank \"%s\" Graph"), _DT(L"Schwartz")));
        auto schwartzGraph =
            std::make_shared<Wisteria::Graphs::SchwartzGraph>(graphDlg.GetCanvas());
        schwartzGraph->SetMessageCatalog(project.GetReadabilityMessageCatalogPtr());
        schwartzGraph->SetInvalidAreaColor(wxGetApp().GetAppOptions()->GetInvalidAreaColor());

        graphDlg.GetCanvas()->SetFixedObject(0, 0, schwartzGraph);
        wxGetApp().GetAppOptions()->UpdateGraphOptions(graphDlg.GetCanvas());
        graphDlg.GetCanvas()->ResetResizeDelay();
        graphDlg.ShowModal();
        }
    else if (event.GetId() == XRCID("ID_BLANK_LIX_GRAPH"))
        {
        Wisteria::UI::GraphDlg graphDlg(wxGetApp().GetParentingWindow(), wxID_ANY,
                                        _(L"Blank Lix Gauge"));
        graphDlg.GetCanvas()->SetFixedObject(
            0, 0, std::make_shared<Wisteria::Graphs::LixGauge>(graphDlg.GetCanvas()));
        wxGetApp().GetAppOptions()->UpdateGraphOptions(graphDlg.GetCanvas());
        graphDlg.GetCanvas()->ResetResizeDelay();
        graphDlg.ShowModal();
        }
    else if (event.GetId() == XRCID("ID_BLANK_GERMAN_LIX_GRAPH"))
        {
        Wisteria::UI::GraphDlg graphDlg(wxGetApp().GetParentingWindow(), wxID_ANY,
                                        _(L"Blank German Lix Gauge"));
        auto lixGauge = std::make_shared<Wisteria::Graphs::LixGaugeGerman>(graphDlg.GetCanvas());
        lixGauge->UseEnglishLabels(wxGetApp().GetAppOptions()->IsUsingEnglishLabelsForGermanLix());

        graphDlg.GetCanvas()->SetFixedObject(0, 0, lixGauge);
        wxGetApp().GetAppOptions()->UpdateGraphOptions(graphDlg.GetCanvas());
        graphDlg.GetCanvas()->ResetResizeDelay();
        graphDlg.ShowModal();
        }
    }

//-------------------------------------------------------
MainFrame::MainFrame(wxDocManager* manager, wxFrame* frame,
                     const wxArrayString& defaultFileExtensions, const wxString& title,
                     const wxPoint& pos, const wxSize& size, long type)
    : Wisteria::UI::BaseMainFrame(manager, frame, defaultFileExtensions, title, pos, size, type),
      CUSTOM_TEST_RANGE(1000), EXAMPLE_RANGE(300), TEST_BUNDLE_RANGE(300)
    {
    Bind(wxEVT_MENU, &MainFrame::OnOpenExample, this, EXAMPLE_RANGE.GetFirstId(),
         EXAMPLE_RANGE.GetLastId());
    const auto accelEntries = std::to_array<wxAcceleratorEntry>(
        { { wxACCEL_NORMAL, WXK_F1, wxID_HELP },
          { wxACCEL_CMD, static_cast<int>(L'N'), wxID_NEW },
          { wxACCEL_CMD, static_cast<int>(L'O'), wxID_OPEN },
          { wxACCEL_CMD, static_cast<int>(L'V'), wxID_PASTE },
          { wxACCEL_CMD, static_cast<int>(L'F'), XRCID("ID_SCRIPT_FIND") },
          { wxACCEL_CMD, static_cast<int>(L'H'), XRCID("ID_SCRIPT_REPLACE") },
          { wxACCEL_CMD | wxACCEL_SHIFT, static_cast<int>(L'N'), XRCID("ID_SCRIPT_NEW") },
          { wxACCEL_CMD | wxACCEL_SHIFT, static_cast<int>(L'O'), XRCID("ID_SCRIPT_OPEN") },
          { wxACCEL_CMD | wxACCEL_SHIFT, static_cast<int>(L'S'), XRCID("ID_SCRIPT_SAVE") },
          { wxACCEL_NORMAL, WXK_F5, XRCID("ID_SCRIPT_RUN") },
          { wxACCEL_SHIFT, WXK_F5, XRCID("ID_SCRIPT_STOP") } });

    wxWindowBase::SetAcceleratorTable(
        wxAcceleratorTable{ accelEntries.size(), accelEntries.data() });

    // bind menu events to their respective ribbon button events
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MainFrame::OnToolsWebHarvest, this,
         XRCID("ID_WEB_HARVEST"));
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxRibbonButtonBarEvent event;
            OnToolsWebHarvest(event);
        },
        XRCID("ID_WEB_HARVEST"));

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MainFrame::OnFindDuplicateFiles, this,
         XRCID("ID_FIND_DUPLICATE_FILES"));
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxRibbonButtonBarEvent event;
            OnFindDuplicateFiles(event);
        },
        XRCID("ID_FIND_DUPLICATE_FILES"));

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MainFrame::OnToolsChapterSplit, this,
         XRCID("ID_CHAPTER_SPLIT"));
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxRibbonButtonBarEvent event;
            OnToolsChapterSplit(event);
        },
        XRCID("ID_CHAPTER_SPLIT"));

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MainFrame::OnToolsOptions, this, wxID_PREFERENCES);
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxRibbonButtonBarEvent event;
            OnToolsOptions(event);
        },
        wxID_PREFERENCES);

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MainFrame::OnWordList, this, XRCID("ID_WORD_LISTS"));
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxRibbonButtonBarEvent event;
            OnWordList(event);
        },
        XRCID("ID_WORD_LISTS"));

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MainFrame::OnTestsOverview, this,
         XRCID("ID_TESTS_OVERVIEW"));
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxRibbonButtonBarEvent event;
            OnTestsOverview(event);
        },
        XRCID("ID_TESTS_OVERVIEW"));

    Bind(
        wxEVT_RIBBONBUTTONBAR_CLICKED, [this]([[maybe_unused]] wxRibbonButtonBarEvent&)
        { ActivateLogTab(); }, XRCID("ID_VIEW_LOG_REPORT"));
    Bind(
        wxEVT_MENU, [this]([[maybe_unused]] wxCommandEvent&) { ActivateLogTab(); },
        XRCID("ID_VIEW_LOG_REPORT"));

    // Log tab ribbon button handlers
    const auto withLogList = [this](auto fn)
    {
        return [this, fn](wxRibbonButtonBarEvent& event)
        {
            if (m_logListCtrl != nullptr && IsLogTabActive())
                {
                fn(m_logListCtrl, event);
                }
        };
    };
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED,
         withLogList([](Wisteria::UI::ListCtrlEx* list, wxRibbonButtonBarEvent& evt)
                     { list->OnSave(evt); }),
         XRCID("ID_LOG_TAB_SAVE"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED,
         withLogList([](Wisteria::UI::ListCtrlEx* list, wxRibbonButtonBarEvent& evt)
                     { list->OnPrint(evt); }),
         XRCID("ID_LOG_TAB_PRINT"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED,
         withLogList([](Wisteria::UI::ListCtrlEx* list, wxRibbonButtonBarEvent& evt)
                     { list->OnCopy(evt); }),
         XRCID("ID_LOG_TAB_COPY"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED,
         withLogList([](Wisteria::UI::ListCtrlEx* list, wxRibbonButtonBarEvent& evt)
                     { list->OnSelectAll(evt); }),
         XRCID("ID_LOG_TAB_SELECT_ALL"));
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED,
         withLogList([](Wisteria::UI::ListCtrlEx* list, wxRibbonButtonBarEvent& evt)
                     { list->OnMultiColumnSort(evt); }),
         XRCID("ID_LOG_TAB_SORT"));
    Bind(
        wxEVT_RIBBONBUTTONBAR_CLICKED,
        [this]([[maybe_unused]] wxRibbonButtonBarEvent&)
        {
            if (m_logListCtrl != nullptr && IsLogTabActive())
                {
                if (wxGetApp().GetLogFile() != nullptr)
                    {
                    wxGetApp().GetLogFile()->Clear();
                    }
                m_logListCtrl->DeleteAllItems();
                }
        },
        XRCID("ID_LOG_TAB_CLEAR"));
    Bind(
        wxEVT_RIBBONBUTTONBAR_CLICKED,
        [this]([[maybe_unused]] wxRibbonButtonBarEvent&)
        {
            if (m_logListCtrl != nullptr && IsLogTabActive())
                {
                wxGetApp().ReadLogIntoListCtrl(m_logListCtrl);
                }
        },
        XRCID("ID_LOG_TAB_REFRESH"));
    Bind(
        wxEVT_RIBBONBUTTONBAR_CLICKED,
        [this]([[maybe_unused]] wxRibbonButtonBarEvent&)
        {
            SetLogAutoRefresh(!m_logAutoRefresh);
            wxGetApp().GetAppOptions()->SetLogAutoRefresh(m_logAutoRefresh);
        },
        XRCID("ID_LOG_TAB_REALTIME_UPDATE"));
    Bind(
        wxEVT_RIBBONBUTTONBAR_CLICKED, []([[maybe_unused]] wxRibbonButtonBarEvent&)
        { wxLog::SetVerbose(!wxLog::GetVerbose()); }, XRCID("ID_LOG_TAB_VERBOSE"));
    Bind(
        wxEVT_TIMER,
        [this]([[maybe_unused]] wxTimerEvent&)
        {
            if (m_logListCtrl != nullptr && IsLogTabActive())
                {
                wxGetApp().ReadLogIntoListCtrl(m_logListCtrl);
                }
        },
        m_logAutoRefreshTimer.GetId());

#ifndef APP_STORE_BUILD
    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MainFrame::OnHelpCheckForUpdates, this,
         XRCID("ID_CHECK_FOR_UPDATES"));
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxRibbonButtonBarEvent event;
            OnHelpCheckForUpdates(event);
        },
        XRCID("ID_CHECK_FOR_UPDATES"));
#endif

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MainFrame::OnHelpSupport, this, XRCID("ID_SUPPORT"));
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxRibbonButtonBarEvent event;
            OnHelpSupport(event);
        },
        XRCID("ID_SUPPORT"));

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MainFrame::OnHelpManual, this, XRCID("ID_HELP_MANUAL"));
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxRibbonButtonBarEvent event;
            OnHelpManual(event);
        },
        XRCID("ID_HELP_MANUAL"));

    Bind(
        wxEVT_RIBBONBUTTONBAR_CLICKED,
        [this]([[maybe_unused]] wxRibbonButtonBarEvent&)
        {
            const wxString manualPath = GetHelpDirectory() + wxFileName::GetPathSeparator() +
                                        _DT(L"readability-test-reference.pdf");
            wxLaunchDefaultApplication(manualPath);
        },
        XRCID("ID_TESTS_REFERENCE"));

    Bind(
        wxEVT_RIBBONBUTTONBAR_CLICKED,
        [this]([[maybe_unused]] wxRibbonButtonBarEvent&)
        {
            const wxString manualPath = GetHelpDirectory() + wxFileName::GetPathSeparator() +
                                        _DT(L"shortcuts-cheatsheet.pdf");
            wxLaunchDefaultApplication(manualPath);
        },
        XRCID("ID_SHORTCUTS_CHEATSHEET"));

    Bind(
        wxEVT_RIBBONBUTTONBAR_CLICKED,
        [this]([[maybe_unused]] wxRibbonButtonBarEvent&)
        {
            const wxString manualPath =
                GetHelpDirectory() + wxFileName::GetPathSeparator() + _DT(L"release-notes.pdf");
            wxLaunchDefaultApplication(manualPath);
        },
        XRCID("ID_RELEASE_NOTES"));

    Bind(
        wxEVT_RIBBONBUTTONBAR_CLICKED,
        [this]([[maybe_unused]] wxRibbonButtonBarEvent&)
        {
            const wxString manualPath = GetHelpDirectory() + wxFileName::GetPathSeparator() +
                                        _DT(L"readability-studio-api.pdf");
            wxLaunchDefaultApplication(manualPath);
        },
        XRCID("ID_PROGRAMMING_MANUAL"));

    Bind(
        wxEVT_RIBBONBUTTONBAR_CLICKED,
        [this]([[maybe_unused]] wxRibbonButtonBarEvent&)
        {
            const wxString manualPath =
                GetHelpDirectory() + wxFileName::GetPathSeparator() + _DT(L"sysadmin-manual.pdf");
            wxLaunchDefaultApplication(manualPath);
        },
        XRCID("ID_SYSADMIN"));

    Bind(wxEVT_RIBBONBUTTONBAR_CLICKED, &MainFrame::OnAbout, this, wxID_ABOUT);
    Bind(
        wxEVT_MENU,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            wxRibbonButtonBarEvent event;
            OnAbout(event);
        },
        wxID_ABOUT);

    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &MainFrame::OnPrintDropdown, this,
         XRCID("ID_PRINT_OPTIONS"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &MainFrame::OnWordListDropdown, this,
         XRCID("ID_WORD_LISTS"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &MainFrame::OnBlankGraphDropdown, this,
         XRCID("ID_BLANK_GRAPHS"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &MainFrame::OnDictionaryDropdown, this,
         XRCID("ID_EDIT_DICTIONARY"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &MainFrame::OnExampleDropdown, this,
         XRCID("ID_EXAMPLES"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &MainFrame::OnCustomTestsDropdown, this,
         XRCID("ID_CUSTOM_TESTS"));
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &MainFrame::OnTestBundlesDropdown, this,
         XRCID("ID_TEST_BUNDLES"));

    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &MainFrame::OnNewDropdown, this, wxID_NEW);
    Bind(wxEVT_RIBBONBUTTONBAR_DROPDOWN_CLICKED, &MainFrame::OnOpenDropdown, this, wxID_OPEN);
    Bind(wxEVT_CLOSE_WINDOW, &MainFrame::OnClose, this);

    Bind(wxEVT_RIBBONBAR_HELP_CLICK, &MainFrame::OnRibbonBarHelpClicked, this);
    Bind(wxEVT_STARTPAGE_CLICKED, &MainFrame::OnStartPageClick, this);

    Bind(wxEVT_MENU, &MainFrame::OnOpenDocument, this, XRCID("ID_OPEN_DOCUMENT"));

    // custom test menu
    Bind(wxEVT_MENU, &MainFrame::OnAddCustomTest, this, XRCID("ID_ADD_CUSTOM_TEST"));
    Bind(wxEVT_MENU, &MainFrame::OnAddCustomTest, this, XRCID("ID_ADD_CUSTOM_NEW_DALE_CHALL_TEST"));
    Bind(wxEVT_MENU, &MainFrame::OnAddCustomTest, this, XRCID("ID_ADD_CUSTOM_SPACHE_TEST"));
    Bind(wxEVT_MENU, &MainFrame::OnAddCustomTest, this,
         XRCID("ID_ADD_CUSTOM_HARRIS_JACOBSON_TEST"));
    Bind(wxEVT_MENU, &MainFrame::OnAddCustomTest, this, XRCID("ID_ADD_CUSTOM_TEST_BASED_ON"));
    Bind(wxEVT_MENU, &MainFrame::OnEditCustomTest, this, XRCID("ID_EDIT_CUSTOM_TEST"));
    Bind(wxEVT_MENU, &MainFrame::OnRemoveCustomTest, this, XRCID("ID_REMOVE_CUSTOM_TEST"));
    Bind(wxEVT_MENU, &MainFrame::OnAddCustomTestBundle, this, XRCID("ID_ADD_CUSTOM_TEST_BUNDLE"));
    Bind(wxEVT_MENU, &MainFrame::OnEditCustomTestBundle, this, XRCID("ID_EDIT_CUSTOM_TEST_BUNDLE"));
    Bind(wxEVT_MENU, &MainFrame::OnRemoveCustomTestBundle, this,
         XRCID("ID_REMOVE_CUSTOM_TEST_BUNDLE"));

    // list editing
    Bind(wxEVT_MENU, &MainFrame::OnEditWordList, this, XRCID("ID_EDIT_WORD_LIST"));
    Bind(wxEVT_MENU, &MainFrame::OnEditPhraseList, this, XRCID("ID_EDIT_PHRASE_LIST"));

    // "Lua Script" buttons on MainFrame Home tab and project frames' Tools tab
    Bind(
        wxEVT_MENU, [this](wxCommandEvent&) { ActivateScriptWorkbench(); },
        XRCID("ID_SCRIPT_WINDOW"));

    // Ribbon tab changes — swap start page / script workbench / log panel
    Bind(wxEVT_RIBBONBAR_PAGE_CHANGED,
         [this](wxRibbonBarEvent& evt)
         {
             // this event bubbles up from child document frames (whose parent window is
             // the main frame), so ignore anything not coming from the main frame's own
             // ribbon; otherwise, switching tabs on a project window's ribbon could show/focus
             // the main frame's log or script workbench controls and steal focus to the main
             // frame
             if (evt.GetEventObject() != GetRibbon())
                 {
                 evt.Skip();
                 return;
                 }
             wxWindowUpdateLocker locker{ this };
             const bool showWorkbench = (evt.GetPage() == GetDeveloperRibbonPage());
             const bool showLog = (evt.GetPage() == GetLogRibbonPage());
             if (GetScriptWorkbench() != nullptr)
                 {
                 GetScriptWorkbench()->Show(showWorkbench);
                 }
             if (GetStartPage() != nullptr)
                 {
                 GetStartPage()->Show(!showWorkbench && !showLog);
                 }
             if (m_logPanel != nullptr)
                 {
                 m_logPanel->Show(showLog);
                 }
             if (showLog)
                 {
                 if (m_logEditButtonBar != nullptr)
                     {
                     m_logEditButtonBar->ToggleButton(XRCID("ID_LOG_TAB_REALTIME_UPDATE"),
                                                      m_logAutoRefresh);
                     m_logEditButtonBar->ToggleButton(XRCID("ID_LOG_TAB_VERBOSE"),
                                                      wxLog::GetVerbose());
                     }
                 if (m_logAutoRefresh)
                     {
                     m_logAutoRefreshTimer.Start(3000);
                     }
                 }
             else
                 {
                 m_logAutoRefreshTimer.Stop();
                 }
             Layout();
             if (showWorkbench && GetScriptWorkbench() != nullptr)
                 {
                 GetScriptWorkbench()->SetFocus();
                 }
             else if (showLog && m_logListCtrl != nullptr)
                 {
                 wxGetApp().ReadLogIntoListCtrl(m_logListCtrl);
                 m_logListCtrl->SetFocus();
                 }
             evt.Skip();
         });

    // Developer ribbon button handlers — forward to the workbench
    const auto withWorkbench = [this](auto fn)
    {
        return [this, fn](wxCommandEvent&)
        {
            if (GetScriptWorkbench() != nullptr && IsDeveloperTabActive())
                {
                fn(GetScriptWorkbench());
                }
        };
    };
    Bind(wxEVT_MENU, withWorkbench([](ScriptWorkbenchPanel* panel) { panel->NewScript(); }),
         XRCID("ID_SCRIPT_NEW"));
    Bind(wxEVT_MENU,
         withWorkbench(
             [this](ScriptWorkbenchPanel* panel)
             {
                 wxFileDialog dlg(this, _(L"Select Script to Open"), wxEmptyString, wxEmptyString,
                                  _(L"Lua Scripts (*.lua)|*.lua"),
                                  wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
                 if (dlg.ShowModal() == wxID_OK)
                     {
                     panel->OpenScriptFromFile(dlg.GetPath());
                     }
             }),
         XRCID("ID_SCRIPT_OPEN"));
    Bind(wxEVT_MENU, withWorkbench([](ScriptWorkbenchPanel* panel) { panel->SaveCurrentScript(); }),
         XRCID("ID_SCRIPT_SAVE"));
    Bind(wxEVT_MENU, withWorkbench([](ScriptWorkbenchPanel* panel) { panel->RunCurrentScript(); }),
         XRCID("ID_SCRIPT_RUN"));
    Bind(wxEVT_MENU, withWorkbench([](ScriptWorkbenchPanel* panel) { panel->ContinueScript(); }),
         XRCID("ID_SCRIPT_CONTINUE"));
    Bind(wxEVT_MENU, withWorkbench([](ScriptWorkbenchPanel* panel) { panel->StopScript(); }),
         XRCID("ID_SCRIPT_STOP"));
    Bind(wxEVT_MENU,
         withWorkbench([](ScriptWorkbenchPanel* panel) { panel->RestartInterpreter(); }),
         XRCID("ID_SCRIPT_CLEAR_GLOBALS"));
    auto updateUIHandler = [this](wxUpdateUIEvent& evt)
    {
        const bool isRunning = LuaInterpreter::IsRunning();
        if (evt.GetId() == XRCID("ID_SCRIPT_RUN"))
            {
            // disabled for the whole duration of a run, including while paused
            evt.Enable(GetScriptWorkbench() != nullptr && !isRunning);
            }
        else if (evt.GetId() == XRCID("ID_SCRIPT_CONTINUE"))
            {
            evt.Enable(GetScriptWorkbench() != nullptr && LuaInterpreter::IsPaused());
            }
        else if (evt.GetId() == XRCID("ID_SCRIPT_STOP"))
            {
            evt.Enable(GetScriptWorkbench() != nullptr && isRunning);
            }
        else if (evt.GetId() == XRCID("ID_SCRIPT_CLEAR_GLOBALS"))
            {
            evt.Enable(GetScriptWorkbench() != nullptr && !isRunning);
            }
        else if (evt.GetId() == XRCID("ID_SCRIPT_FUNCTION_BROWSER"))
            {
            evt.Enable(GetScriptWorkbench() != nullptr);
            evt.Check(GetScriptWorkbench() != nullptr &&
                      GetScriptWorkbench()->IsFunctionBrowserVisible());
            }
        else if (evt.GetId() == XRCID("ID_SCRIPT_TOGGLE_DEBUG"))
            {
            evt.Enable(GetScriptWorkbench() != nullptr);
            evt.Check(GetScriptWorkbench() != nullptr &&
                      GetScriptWorkbench()->IsDebugWindowVisible());
            }
    };
    Bind(wxEVT_UPDATE_UI, updateUIHandler, XRCID("ID_SCRIPT_RUN"));
    Bind(wxEVT_UPDATE_UI, updateUIHandler, XRCID("ID_SCRIPT_CONTINUE"));
    Bind(wxEVT_UPDATE_UI, updateUIHandler, XRCID("ID_SCRIPT_STOP"));
    Bind(wxEVT_UPDATE_UI, updateUIHandler, XRCID("ID_SCRIPT_CLEAR_GLOBALS"));
    Bind(wxEVT_UPDATE_UI, updateUIHandler, XRCID("ID_SCRIPT_FUNCTION_BROWSER"));
    Bind(wxEVT_UPDATE_UI, updateUIHandler, XRCID("ID_SCRIPT_TOGGLE_DEBUG"));
    Bind(wxEVT_MENU, withWorkbench([](ScriptWorkbenchPanel* panel) { panel->Undo(); }),
         XRCID("ID_SCRIPT_UNDO"));
    Bind(wxEVT_MENU, withWorkbench([](ScriptWorkbenchPanel* panel) { panel->Redo(); }),
         XRCID("ID_SCRIPT_REDO"));
    Bind(wxEVT_MENU, withWorkbench([](ScriptWorkbenchPanel* panel) { panel->DuplicateLine(); }),
         XRCID("ID_SCRIPT_DUPLICATE_LINE"));
    Bind(
        wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& evt)
        { evt.Enable(GetScriptWorkbench() != nullptr && GetScriptWorkbench()->CanUndo()); },
        XRCID("ID_SCRIPT_UNDO"));
    Bind(
        wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& evt)
        { evt.Enable(GetScriptWorkbench() != nullptr && GetScriptWorkbench()->CanRedo()); },
        XRCID("ID_SCRIPT_REDO"));
    Bind(
        wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& evt)
        { evt.Enable(GetScriptWorkbench() != nullptr); }, XRCID("ID_SCRIPT_DUPLICATE_LINE"));
    Bind(wxEVT_MENU, withWorkbench([](ScriptWorkbenchPanel* panel) { panel->Cut(); }),
         XRCID("ID_SCRIPT_CUT"));
    Bind(wxEVT_MENU, withWorkbench([](ScriptWorkbenchPanel* panel) { panel->Copy(); }),
         XRCID("ID_SCRIPT_COPY"));
    Bind(wxEVT_MENU, withWorkbench([](ScriptWorkbenchPanel* panel) { panel->Paste(); }),
         XRCID("ID_SCRIPT_PASTE"));
    Bind(wxEVT_MENU, withWorkbench([](ScriptWorkbenchPanel* panel) { panel->SelectAll(); }),
         XRCID("ID_SCRIPT_SELECT_ALL"));
    Bind(wxEVT_MENU, withWorkbench([](ScriptWorkbenchPanel* panel) { panel->Comment(); }),
         XRCID("ID_SCRIPT_COMMENT"));
    Bind(wxEVT_MENU, withWorkbench([](ScriptWorkbenchPanel* panel) { panel->Uncomment(); }),
         XRCID("ID_SCRIPT_UNCOMMENT"));
    Bind(
        wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& evt)
        { evt.Enable(GetScriptWorkbench() != nullptr && GetScriptWorkbench()->CanCut()); },
        XRCID("ID_SCRIPT_CUT"));
    Bind(
        wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& evt)
        { evt.Enable(GetScriptWorkbench() != nullptr && GetScriptWorkbench()->CanCopy()); },
        XRCID("ID_SCRIPT_COPY"));
    Bind(
        wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& evt)
        { evt.Enable(GetScriptWorkbench() != nullptr && GetScriptWorkbench()->CanPaste()); },
        XRCID("ID_SCRIPT_PASTE"));
    Bind(
        wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& evt)
        { evt.Enable(GetScriptWorkbench() != nullptr); }, XRCID("ID_SCRIPT_SELECT_ALL"));
    Bind(
        wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& evt)
        { evt.Enable(GetScriptWorkbench() != nullptr); }, XRCID("ID_SCRIPT_COMMENT"));
    Bind(
        wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& evt)
        { evt.Enable(GetScriptWorkbench() != nullptr); }, XRCID("ID_SCRIPT_UNCOMMENT"));
    Bind(wxEVT_MENU, withWorkbench([](ScriptWorkbenchPanel* panel) { panel->ShowFindDialog(); }),
         XRCID("ID_SCRIPT_FIND"));
    Bind(wxEVT_MENU, withWorkbench([](ScriptWorkbenchPanel* panel) { panel->ShowReplaceDialog(); }),
         XRCID("ID_SCRIPT_REPLACE"));
    Bind(wxEVT_MENU, withWorkbench([](ScriptWorkbenchPanel* panel) { panel->GotoLineDialog(); }),
         XRCID("ID_SCRIPT_GOTO_LINE"));
    Bind(wxEVT_MENU, withWorkbench([](ScriptWorkbenchPanel* panel) { panel->DebugClear(); }),
         XRCID("ID_SCRIPT_CLEAR_DEBUG"));
    Bind(wxEVT_MENU, withWorkbench([](ScriptWorkbenchPanel* panel) { panel->ToggleDebugWindow(); }),
         XRCID("ID_SCRIPT_TOGGLE_DEBUG"));
    Bind(wxEVT_MENU,
         withWorkbench([](ScriptWorkbenchPanel* panel) { panel->ToggleFunctionBrowser(); }),
         XRCID("ID_SCRIPT_FUNCTION_BROWSER"));

    Bind(
        wxEVT_UPDATE_UI, [this](wxUpdateUIEvent& evt)
        { evt.Enable(GetScriptWorkbench() != nullptr); }, XRCID("ID_SCRIPT_GOTO_LINE"));
    Bind(
        wxEVT_MENU,
        [](wxCommandEvent&)
        {
            const wxString manualPath = wxGetApp().GetMainFrameEx()->GetHelpDirectory() +
                                        wxFileName::GetPathSeparator() +
                                        _DT(L"readability-studio-api.pdf");
            if (wxFile::Exists(manualPath))
                {
                wxLaunchDefaultApplication(manualPath);
                }
        },
        XRCID("ID_SCRIPT_API_PDF"));
    Bind(
        wxEVT_MENU,
        [](wxCommandEvent&)
        {
            const wxString helpPath = wxGetApp().GetMainFrameEx()->GetHelpDirectory() +
                                      _DT(L"/lua-5.4/doc/contents.html");
            if (wxFile::Exists(helpPath))
                {
                wxLaunchDefaultBrowser(wxFileName::FileNameToURL(helpPath));
                }
        },
        XRCID("ID_SCRIPT_LUA_REFERENCE"));

    // dictionary menu
    Bind(wxEVT_MENU, &MainFrame::OnEditEnglishDictionary, this,
         XRCID("ID_EDIT_ENGLISH_DICTIONARY"));
    Bind(wxEVT_MENU, &MainFrame::OnEditDictionarySettings, this,
         XRCID("ID_EDIT_DICTIONARY_SETTINGS"));

    // print and paste
    Bind(wxEVT_MENU, &MainFrame::OnPrinterHeaderFooter, this, XRCID("ID_PRINTER_HEADER_FOOTER"));
    Bind(wxEVT_MENU, &MainFrame::OnPrintWatermark, this, XRCID("ID_EDIT_WATERMARK"));
    Bind(wxEVT_MENU, &MainFrame::OnPaste, this, wxID_PASTE);

    // blank graph menu
    Bind(wxEVT_MENU, &MainFrame::OnBlankGraph, this, XRCID("ID_BLANK_FRASE_GRAPH"));
    Bind(wxEVT_MENU, &MainFrame::OnBlankGraph, this, XRCID("ID_BLANK_FRY_GRAPH"));
    Bind(wxEVT_MENU, &MainFrame::OnBlankGraph, this, XRCID("ID_BLANK_GPM_GRAPH"));
    Bind(wxEVT_MENU, &MainFrame::OnBlankGraph, this, XRCID("ID_BLANK_RAYGOR_GRAPH"));
    Bind(wxEVT_MENU, &MainFrame::OnBlankGraph, this, XRCID("ID_BLANK_CRAWFORD_GRAPH"));
    Bind(wxEVT_MENU, &MainFrame::OnBlankGraph, this, XRCID("ID_BLANK_INFLESZ_GRAPH"));
    Bind(wxEVT_MENU, &MainFrame::OnBlankGraph, this, XRCID("ID_BLANK_FLESCH_GRAPH"));
    Bind(wxEVT_MENU, &MainFrame::OnBlankGraph, this, XRCID("ID_BLANK_DB2_GRAPH"));
    Bind(wxEVT_MENU, &MainFrame::OnBlankGraph, this, XRCID("ID_BLANK_SCHWARTZ_GRAPH"));
    Bind(wxEVT_MENU, &MainFrame::OnBlankGraph, this, XRCID("ID_BLANK_LIX_GRAPH"));
    Bind(wxEVT_MENU, &MainFrame::OnBlankGraph, this, XRCID("ID_BLANK_GERMAN_LIX_GRAPH"));

    // word list menu
    Bind(wxEVT_MENU, &MainFrame::OnWordListByPage, this, XRCID("ID_DC_WORD_LIST_WINDOW"));
    Bind(wxEVT_MENU, &MainFrame::OnWordListByPage, this,
         XRCID("ID_STOCKER_CATHOLIC_WORD_LIST_WINDOW"));
    Bind(wxEVT_MENU, &MainFrame::OnWordListByPage, this, XRCID("ID_SPACHE_WORD_LIST_WINDOW"));
    Bind(wxEVT_MENU, &MainFrame::OnWordListByPage, this,
         XRCID("ID_HARRIS_JACOBSON_WORD_LIST_WINDOW"));
    Bind(wxEVT_MENU, &MainFrame::OnWordListByPage, this, XRCID("ID_DOLCH_WORD_LIST_WINDOW"));
    }

//---------------------------------------------------
void ReadabilityApp::ReadLogIntoListCtrl(Wisteria::UI::ListCtrlEx* listCtrl)
    {
    if (listCtrl == nullptr || GetLogFile() == nullptr)
        {
        return;
        }
    wxTheApp->Yield();
    // TRANSLATORS: Default filename when saving the log report. %1$s is the application name and
    // %2$s is the current date.
    listCtrl->SetLabel(wxString::Format(_(L"%1$s Log %2$s"), GetAppDisplayName(),
                                        wxDateTime::Now().FormatISODate()));
    const long style = listCtrl->GetExtraStyle();
    listCtrl->SetExtraStyle(style | wxWS_EX_BLOCK_EVENTS);
    const wxWindowUpdateLocker wl{ listCtrl };

    if (listCtrl->GetColumnCount() < 4)
        {
        listCtrl->DeleteAllColumns();
        listCtrl->InsertColumn(0, _(L"Message"));
        listCtrl->InsertColumn(1, _(L"Timestamp"));
        listCtrl->InsertColumn(2, _(L"Function"));
        listCtrl->InsertColumn(3, _(L"Source"));
        }
    listCtrl->EnableAlternateRowColours(false);
    listCtrl->DeleteAllItems();

    const lily_of_the_valley::text_column_delimited_character_parser parser(L'\t');
    lily_of_the_valley::text_column<lily_of_the_valley::text_column_delimited_character_parser>
        myColumn(parser, std::nullopt);
    lily_of_the_valley::text_row<Wisteria::UI::ListCtrlExDataProvider::ListCellString> myRow(
        std::nullopt);
    myRow.treat_consecutive_delimiters_as_one(false);
    myRow.add_column(myColumn);

    auto* dataProvider = dynamic_cast<Wisteria::UI::ListCtrlExDataProvider*>(
        listCtrl->GetVirtualDataProvider().get());
    if (dataProvider == nullptr)
        {
        return;
        }
    lily_of_the_valley::text_matrix<Wisteria::UI::ListCtrlExDataProvider::ListCellString> importer(
        &dataProvider->GetMatrix());
    importer.add_row_definition(myRow);

    const wxString logBuffer{ GetLogFile()->Read() };
    lily_of_the_valley::text_preview preview;
    size_t rowCount = preview(logBuffer, L'\t', true, false);
    rowCount = importer.read(logBuffer, rowCount, 4, true);

    listCtrl->SetVirtualDataSize(rowCount, 4);
    listCtrl->SetItemCount(static_cast<long>(rowCount));

    for (long i = 0; i < listCtrl->GetItemCount(); ++i)
        {
        const auto currentRow = listCtrl->GetItemText(i, 0);
        const wxColour rowColor =
            (currentRow.find(_DT(L"Error: ", DTExplanation::LogMessage)) != wxString::npos) ?
                wxColour(242, 94, 101) :
            (currentRow.find(_DT(L"Warning: ")) != wxString::npos) ?
                Wisteria::Colors::ColorBrewer::GetColor(Wisteria::Colors::Color::Yellow) :
            (currentRow.find(_DT(L"Debug: ")) != wxString::npos) ? wxColour(143, 214, 159) :
                                                                   wxNullColour;
        if (rowColor.IsOk())
            {
            listCtrl->SetRowAttributes(
                i, wxListItemAttr(wxColour{ 0, 0, 0 }, rowColor, listCtrl->GetFont()));
            }
        }

    if (listCtrl->GetItemCount() > 0)
        {
        listCtrl->EnsureVisible(listCtrl->GetItemCount() - 1);
        }
    listCtrl->SetSortedColumn(0, Wisteria::SortDirection::SortAscending);
    listCtrl->SetExtraStyle(style);
    listCtrl->DistributeColumns(-1);
    }

//---------------------------------------------------
void ReadabilityApp::InitProjectSidebar()
    {
    auto& imgList{ GetMainFrameEx()->GetProjectSideBarImageList() };
    // Fill in the icons for the projects' sidebars.
    // Do NOT change the ordering of these (indices are used by UpdateSideBarIcons());
    // new ones always get added at the bottom.
    imgList.push_back(GetResourceManager().GetSVG(L"ribbon/highlightedwords.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"tests/flesch-test.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"ribbon/stats.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"ribbon/difficult.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"ribbon/grammar.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"tests/dolch.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"ribbon/histogram.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"ribbon/boxplot.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"ribbon/warning.svg"));
    imgList.push_back(GetResourceManager().GetSVG(wxSystemSettings::GetAppearance().IsDark() ?
                                                      L"ribbon/bullet-dark.svg" :
                                                      L"ribbon/bullet.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"ribbon/configure.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"ribbon/project-settings.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"ribbon/document-structure.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"ribbon/words.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"ribbon/sentences.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"ribbon/list.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"ribbon/bar-chart.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"ribbon/report.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"tests/flesch-test.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"tests/frase.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"tests/fry-test.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"tests/raygor-test.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"tests/crawford.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"ribbon/scores-window.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"ribbon/heatmap.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"tests/schwartz.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"tests/lix-test.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"tests/danielson-bryan-2.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"ribbon/checklist.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"ribbon/word-cloud.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"ribbon/donut-subgrouped.svg"));
    imgList.push_back(GetResourceManager().GetSVG(L"tests/inflesz-test.svg"));
    }

//---------------------------------------------------
void ReadabilityApp::InitStartPage()
    {
    // add start area inside mainframe
    wxArrayString mruFiles;
    for (size_t i = 0; i < GetDocManager()->GetFileHistory()->GetCount(); ++i)
        {
        mruFiles.Add(GetDocManager()->GetFileHistory()->GetHistoryFile(i));
        }
    GetMainFrameEx()->m_startPage = new wxStartPage(
        GetMainFrameEx(), wxID_ANY, mruFiles, GetResourceManager().GetSVG(L"ribbon/app-logo.svg"));

    GetMainFrameEx()->GetStartPage()->SetUserName(m_preInitOptions.m_userName);
    GetMainFrameEx()->GetStartPage()->AddButton(
        wxArtProvider::GetBitmapBundle(L"ID_DOCUMENT", wxART_BUTTON), _(L"Create a New Project"));
    GetMainFrameEx()->GetStartPage()->AddButton(wxArtProvider::GetBitmapBundle(wxART_FILE_OPEN),
                                                _(L"Open a Project"));
    GetMainFrameEx()->GetStartPage()->AddButton(GetResourceManager().GetSVG(L"ribbon/examples.svg"),
                                                _(L"Read the Examples"));
    GetMainFrameEx()->GetStartPage()->AddButton(
        wxArtProvider::GetBitmapBundle("ID_NOTES", wxART_BUTTON), _(L"Read the Notes"));
    GetMainFrameEx()->GetStartPage()->AddButton(
        wxArtProvider::GetBitmapBundle(wxART_HELP_BOOK, wxART_BUTTON), _(L"Read the Manual"));
    GetMainFrameEx()->GetStartPage()->AddButton(
        wxArtProvider::GetBitmapBundle("ID_WHATS_NEW", wxART_BUTTON), _(L"What's New"));
    GetMainFrameEx()->GetStartPage()->AddButton(
        GetResourceManager().GetSVG(L"ribbon/configure.svg"), _(L"Review Program Options"));

    GetMainFrameEx()->GetSizer()->Add(GetMainFrameEx()->GetStartPage(), wxSizerFlags{ 1 }.Expand());

    // The script workbench and log panel share the same sizer slot as the start page;
    // the Developer/Log ribbon tabs swap which one is visible.
    GetMainFrameEx()->m_scriptWorkbench = new ScriptWorkbenchPanel(GetMainFrameEx());
    GetMainFrameEx()->GetScriptWorkbench()->Hide();
    GetMainFrameEx()->GetSizer()->Add(GetMainFrameEx()->GetScriptWorkbench(),
                                      wxSizerFlags{ 1 }.Expand());

    GetMainFrameEx()->m_logPanel = new wxPanel(GetMainFrameEx());
    GetMainFrameEx()->m_logPanel->Hide();
    GetMainFrameEx()->m_logDataProvider = std::make_shared<Wisteria::UI::ListCtrlExDataProvider>();
    GetMainFrameEx()->m_logListCtrl =
        new Wisteria::UI::ListCtrlEx(GetMainFrameEx()->m_logPanel, wxID_ANY, wxDefaultPosition,
                                     wxDefaultSize, wxLC_REPORT | wxLC_VIRTUAL | wxBORDER_NONE);
    GetMainFrameEx()->m_logListCtrl->SetVirtualDataProvider(GetMainFrameEx()->m_logDataProvider);
    auto* logPanelSizer = new wxBoxSizer(wxVERTICAL);
    logPanelSizer->Add(GetMainFrameEx()->m_logListCtrl, wxSizerFlags{ 1 }.Expand());
    GetMainFrameEx()->m_logPanel->SetSizer(logPanelSizer);
    GetMainFrameEx()->GetSizer()->Add(GetMainFrameEx()->m_logPanel, wxSizerFlags{ 1 }.Expand());
    }

//---------------------------------------------------
void MainFrame::OnStartPageClick(const wxCommandEvent& event)
    {
    if (GetStartPage()->IsCustomButtonId(event.GetId()))
        {
        if (event.GetId() == GetStartPage()->GetButtonID(0))
            {
            OpenFileNew(wxString{});
            }
        else if (event.GetId() == GetStartPage()->GetButtonID(1))
            {
            wxFileDialog dialog(wxGetApp().GetParentingWindow(), _(L"Select Project to Open"),
                                wxGetApp().GetAppOptions()->GetProjectPath(), wxString{},
                                // TRANSLATORS: %s is program name.
                                wxString::Format(_(L"%s Project (*.rsp;*.rsbp)|*.rsp;*.rsbp"),
                                                 wxGetApp().GetAppDisplayName()),
                                wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
            if (dialog.ShowModal() == wxID_OK)
                {
                wxGetApp().GetAppOptions()->SetProjectPath(wxFileName(dialog.GetPath()).GetPath());
                OpenFile(dialog.GetPath());
                }
            }
        else if (event.GetId() == GetStartPage()->GetButtonID(2))
            {
            DisplayHelp(_DT(L"online/examples-getting-started.html"));
            }
        else if (event.GetId() == GetStartPage()->GetButtonID(3))
            {
            DisplayHelp(_DT(L"online/analysis-notes.html"));
            }
        else if (event.GetId() == GetStartPage()->GetButtonID(4))
            {
            const wxString manualPath = GetHelpDirectory() + wxFileName::GetPathSeparator() +
                                        _DT(L"readability-studio-manual.pdf");
            wxLaunchDefaultApplication(manualPath);
            }
        else if (event.GetId() == GetStartPage()->GetButtonID(5))
            {
            const wxString manualPath =
                GetHelpDirectory() + wxFileName::GetPathSeparator() + _DT(L"release-notes.pdf");
            wxLaunchDefaultApplication(manualPath);
            }
        else if (event.GetId() == GetStartPage()->GetButtonID(6))
            {
            const wxString previousReportTheme = wxGetApp().GetAppOptions()->GetReportTheme();
            ToolsOptionsDlg optionsDlg(wxGetApp().GetParentingWindow());
            optionsDlg.SelectPage(ToolsOptionsDlg::GENERAL_SETTINGS_PAGE);
            if (optionsDlg.ShowModal() == wxID_OK)
                {
                wxGetApp().GetAppOptions()->SaveOptionsFile();
                RefreshOpenProjectsIfThemeChanged(previousReportTheme);
                }
            }
        }
    else if (wxStartPage::IsFileId(event.GetId()))
        {
        OpenFile(event.GetString());
        }
    else if (wxStartPage::IsBrowseId(event.GetId()))
        {
        wxCommandEvent openEvent(wxEVT_MENU, wxID_OPEN);
        ProcessWindowEvent(openEvent);
        }
    else if (wxStartPage::IsFileListClearId(event.GetId()))
        {
        wxGetApp().ClearFileHistoryMenu();
        }
    }

//---------------------------------------------------
void MainFrame::OnNewDropdown(wxRibbonButtonBarEvent& event)
    {
    wxMenu menu;
    auto* menuItem = new wxMenuItem(&menu, wxID_NEW, _(L"New Project...") + _DT(L"\tCtrl+N"));
    menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/document.svg"));
    menu.Append(menuItem);
    menu.Append(wxID_PASTE, _(L"New Project from Clipboard...") + _DT(L"\tCtrl+V"));
    event.PopupMenu(&menu);
    }

//---------------------------------------------------
void MainFrame::OnOpenDropdown(wxRibbonButtonBarEvent& event) { event.PopupMenu(&m_fileOpenMenu); }

//---------------------------------------------------
void MainFrame::OnPrintDropdown(wxRibbonButtonBarEvent& event) { event.PopupMenu(&m_printMenu); }

//---------------------------------------------------
void MainFrame::OnCustomTestsDropdown(wxRibbonButtonBarEvent& event)
    {
    event.PopupMenu(&m_customTestsMenu);
    }

//---------------------------------------------------
void MainFrame::OnTestBundlesDropdown(wxRibbonButtonBarEvent& event)
    {
    event.PopupMenu(&m_testsBundleMenu);
    }

//---------------------------------------------------
void MainFrame::OnExampleDropdown(wxRibbonButtonBarEvent& event)
    {
    event.PopupMenu(&m_exampleMenu);
    }

//---------------------------------------------------
void MainFrame::OnDictionaryDropdown(wxRibbonButtonBarEvent& event)
    {
    wxMenu menu;
    menu.Append(XRCID("ID_EDIT_ENGLISH_DICTIONARY"), _(L"Edit Custom Dictionary..."));
    menu.Append(XRCID("ID_EDIT_DICTIONARY_SETTINGS"), _(L"Settings..."));
    event.PopupMenu(&menu);
    }

//---------------------------------------------------
void MainFrame::OnWordList([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    WordListDlg wordListsDlg(wxGetApp().GetParentingWindow());
    wordListsDlg.ShowModal();
    }

//---------------------------------------------------
void MainFrame::OnWordListDropdown(wxRibbonButtonBarEvent& event)
    {
    event.PopupMenu(&m_wordListMenu);
    }

//---------------------------------------------------
void MainFrame::OnBlankGraphDropdown(wxRibbonButtonBarEvent& event)
    {
    event.PopupMenu(&m_blankGraphMenu);
    }

//---------------------------------------------------
void MainFrame::OnEditEnglishDictionary([[maybe_unused]] wxCommandEvent& event)
    {
    wxGetApp().EditDictionary(readability::test_language::english_test);
    }

//---------------------------------------------------
void MainFrame::OnEditDictionarySettings([[maybe_unused]] wxCommandEvent& event)
    {
    const wxString previousReportTheme = wxGetApp().GetAppOptions()->GetReportTheme();
    ToolsOptionsDlg optionsDlg(wxGetApp().GetParentingWindow());
    optionsDlg.SelectPage(ToolsOptionsDlg::GRAMMAR_PAGE);
    if (optionsDlg.ShowModal() == wxID_OK)
        {
        wxGetApp().GetAppOptions()->SaveOptionsFile();
        RefreshOpenProjectsIfThemeChanged(previousReportTheme);
        }
    }

//-------------------------------------------------------
void MainFrame::OnOpenExample(const wxCommandEvent& event)
    {
    const auto pos = GetExamplesMenuIds().find(event.GetId());
    if (pos == GetExamplesMenuIds().cend())
        {
        return;
        }
    const FilePathResolver fn(pos->second, false);
    // create a batch project from the example file
    if (FilePathResolver::IsSpreadsheet(fn.GetResolvedPath()) ||
        FilePathResolver::IsArchive(fn.GetResolvedPath()))
        {
        auto* docTemplate = wxGetApp().GetDocManager()->FindTemplate(CLASSINFO(BatchProjectDoc));
        if (docTemplate != nullptr)
            {
            auto* newDoc = dynamic_cast<BatchProjectDoc*>(
                docTemplate->CreateDocument(fn.GetResolvedPath(), wxDOC_NEW));
            if ((newDoc != nullptr) && !newDoc->OnNewDocument())
                {
                // Document is implicitly deleted by DeleteAllViews
                newDoc->DeleteAllViews();
                }
            if ((newDoc != nullptr) && (newDoc->GetFirstView() != nullptr))
                {
                newDoc->GetFirstView()->Activate(true);
                wxGetApp().GetDocManager()->ActivateView(newDoc->GetFirstView());
                if (newDoc->GetDocumentWindow() != nullptr)
                    {
                    newDoc->GetDocumentWindow()->SetFocus();
                    }
                }
            }
        }
    // create a standard project from the example file
    else
        {
        wxArrayString choices;
        choices.push_back(_(L"Create a new project"));
        choices.push_back(_(L"View document"));
        wxArrayString descriptions;
        descriptions.push_back(_(L"Create a new project using the example document"));
        descriptions.push_back(_(L"View the document in your system's default editor"));
        Wisteria::UI::RadioBoxDlg choiceDlg(
            wxGetApp().GetParentingWindow(), _(L"Select how to open the example document"),
            wxString{}, wxString{}, _(L"Open Example Document"), choices, descriptions);
        if (choiceDlg.ShowModal() == wxID_CANCEL)
            {
            return;
            }
        if (choiceDlg.GetSelection() == 0)
            {
            wxDocTemplate* docTemplate =
                wxGetApp().GetDocManager()->FindTemplate(CLASSINFO(ProjectDoc));
            if (docTemplate != nullptr)
                {
                ProjectDoc* newDoc = dynamic_cast<ProjectDoc*>(
                    docTemplate->CreateDocument(fn.GetResolvedPath(), wxDOC_NEW));
                if ((newDoc != nullptr) && !newDoc->OnNewDocument())
                    {
                    // Document is implicitly deleted by DeleteAllViews
                    newDoc->DeleteAllViews();
                    newDoc = nullptr;
                    }
                if ((newDoc != nullptr) && (newDoc->GetFirstView() != nullptr))
                    {
                    newDoc->GetFirstView()->Activate(true);
                    wxGetApp().GetDocManager()->ActivateView(newDoc->GetFirstView());
                    if (newDoc->GetDocumentWindow() != nullptr)
                        {
                        newDoc->GetDocumentWindow()->SetFocus();
                        }
                    }
                }
            }
        else
            {
            wxLaunchDefaultApplication(fn.GetResolvedPath());
            }
        }
    }

//-------------------------------------------------------
void MainFrame::OnWordListByPage(const wxCommandEvent& event)
    {
    WordListDlg wordListsDlg(wxGetApp().GetParentingWindow());
    // New Dale-Chall Words
    if (event.GetId() == XRCID("ID_DC_WORD_LIST_WINDOW"))
        {
        wordListsDlg.SelectPage(WordListDlg::DALE_CHALL_PAGE_ID);
        }
    // Stocker list
    else if (event.GetId() == XRCID("ID_STOCKER_CATHOLIC_WORD_LIST_WINDOW"))
        {
        wordListsDlg.SelectPage(WordListDlg::STOCKER_PAGE_ID);
        }
    // Spache list
    else if (event.GetId() == XRCID("ID_SPACHE_WORD_LIST_WINDOW"))
        {
        wordListsDlg.SelectPage(WordListDlg::SPACHE_PAGE_ID);
        }
    // Harris-Jacobson
    else if (event.GetId() == XRCID("ID_HARRIS_JACOBSON_WORD_LIST_WINDOW"))
        {
        wordListsDlg.SelectPage(WordListDlg::HARRIS_JACOBSON_PAGE_ID);
        }
    // Dolch words
    else if (event.GetId() == XRCID("ID_DOLCH_WORD_LIST_WINDOW"))
        {
        wordListsDlg.SelectPage(WordListDlg::DOLCH_PAGE_ID);
        }
    wordListsDlg.ShowModal();
    }

//-------------------------------------------------------
void MainFrame::ActivateScriptWorkbench()
    {
    if (GetScriptWorkbench() == nullptr || GetDeveloperRibbonPage() == nullptr)
        {
        return;
        }
    if (!IsShown())
        {
        Show();
        }
    Raise();
    GetRibbon()->SetActivePage(GetDeveloperRibbonPage());
    // manually fire the layout swap; SetActivePage() doesn't always emit a change event
    GetScriptWorkbench()->Show(true);
    if (GetStartPage() != nullptr)
        {
        GetStartPage()->Hide();
        }
    if (m_logPanel != nullptr)
        {
        m_logPanel->Hide();
        }
    Layout();
    GetScriptWorkbench()->SetFocus();
    }

//-------------------------------------------------------
void MainFrame::ActivateHomePage()
    {
    if (GetHomeRibbonPage() == nullptr)
        {
        return;
        }
    if (!IsShown())
        {
        Show();
        }
    Raise();
    GetRibbon()->SetActivePage(GetHomeRibbonPage());
    // manually fire the layout swap; SetActivePage() doesn't always emit a change event
    if (GetStartPage() != nullptr)
        {
        GetStartPage()->Show();
        }
    if (GetScriptWorkbench() != nullptr)
        {
        GetScriptWorkbench()->Hide();
        }
    Layout();
    }

//-------------------------------------------------------
void MainFrame::AddCustomTestToMenus(const wxString& testName)
    {
    const int menuId = CUSTOM_TEST_RANGE.GetNextId();
    if (menuId == wxNOT_FOUND)
        {
        wxMessageBox(_(L"Unable to add custom test to menu: not enough menu IDs, "
                       "please contact software vendor to remove this limitation."),
                     _(L"Error"), wxOK | wxICON_ERROR);
        return;
        }
    m_customTestMenuIds.insert(std::make_pair(menuId, testName));
    // set a unique ID for this test for use in the sidebar (this is different from the menu ID).
    auto testIter = std::find(BaseProject::m_custom_word_tests.begin(),
                              BaseProject::m_custom_word_tests.end(), testName);
    if (testIter != BaseProject::m_custom_word_tests.end())
        {
        testIter->set_interface_id(BaseProjectView::GetCustomTestSidebarIdRange().GetNextId());
        }
    // add it to any open views' menus now
    auto& docs = wxGetApp().GetDocManager()->GetDocuments();
    for (size_t i = 0; i < docs.GetCount(); ++i)
        {
        auto* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
        if (doc != nullptr)
            {
            auto* view = dynamic_cast<BaseProjectView*>(doc->GetFirstView());
            if (view != nullptr)
                {
                FillMenuWithCustomTests(&view->GetDocFrame()->m_customTestsMenu, doc, true);
                FillMenuWithCustomTests(view->GetDocFrame()->m_customTestsRegularMenu, doc, true);
                }
            }
        }
    FillMenuWithCustomTests(&m_customTestsMenu, nullptr, false);
    FillMenuWithCustomTests(m_customTestsRegularMenu, nullptr, false);
    }

//-------------------------------------------------------
void MainFrame::AddTestBundleToMenus(const wxString& bundleName)
    {
    const int menuId = TEST_BUNDLE_RANGE.GetNextId();
    if (menuId == wxNOT_FOUND)
        {
        wxMessageBox(_(L"Unable to add test bundle to menu: not enough menu IDs, "
                       "please contact software vendor to remove this limitation."),
                     _(L"Error"), wxOK | wxICON_ERROR);
        return;
        }
    m_testBundleMenuIds.insert(std::make_pair(menuId, bundleName));
    // add it to any open views' menus now
    auto& docs = wxGetApp().GetDocManager()->GetDocuments();
    for (size_t i = 0; i < docs.GetCount(); ++i)
        {
        auto* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
        if (doc != nullptr)
            {
            auto* view = dynamic_cast<BaseProjectView*>(doc->GetFirstView());
            if (view != nullptr)
                {
                FillMenuWithTestBundles(&view->GetDocFrame()->m_testsBundleMenu, doc, true);
                FillMenuWithTestBundles(view->GetDocFrame()->m_testsBundleRegularMenu, doc, true);
                }
            }
        }
    FillMenuWithTestBundles(&m_testsBundleMenu, nullptr, false);
    FillMenuWithTestBundles(m_testsBundleRegularMenu, nullptr, false);
    }

//-------------------------------------------------------
void MainFrame::RemoveTestBundleFromMenus(const wxString& bundleName)
    {
    auto menuPos = m_testBundleMenuIds.begin();
    for (/*initialized already*/; menuPos != m_testBundleMenuIds.end(); ++menuPos)
        {
        if (menuPos->second == bundleName)
            {
            break;
            }
        }
    if (menuPos == m_testBundleMenuIds.end())
        {
        return;
        }
    m_testBundleMenuIds.erase(menuPos);

    // remove it from any open views' menus now
    auto& docs = wxGetApp().GetDocManager()->GetDocuments();
    for (size_t i = 0; i < docs.GetCount(); ++i)
        {
        auto* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
        if (doc != nullptr)
            {
            auto* view = dynamic_cast<BaseProjectView*>(doc->GetFirstView());
            if (view != nullptr)
                {
                FillMenuWithTestBundles(&view->GetDocFrame()->m_testsBundleMenu, doc, true);
                FillMenuWithTestBundles(view->GetDocFrame()->m_testsBundleRegularMenu, doc, true);
                }
            }
        }
    FillMenuWithTestBundles(&m_testsBundleMenu, nullptr, false);
    FillMenuWithTestBundles(m_testsBundleRegularMenu, nullptr, false);
    }

//-------------------------------------------------------
void MainFrame::RemoveCustomTestFromMenus(const wxString& testName)
    {
    auto menuPos = m_customTestMenuIds.begin();
    for (/*initialized already*/; menuPos != m_customTestMenuIds.end(); ++menuPos)
        {
        if (menuPos->second == testName)
            {
            break;
            }
        }
    if (menuPos == m_customTestMenuIds.end())
        {
        return;
        }
    m_customTestMenuIds.erase(menuPos);

    // remove it from any open views' menus now
    auto& docs = wxGetApp().GetDocManager()->GetDocuments();
    for (size_t i = 0; i < docs.GetCount(); ++i)
        {
        auto* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
        if (doc != nullptr)
            {
            auto* view = dynamic_cast<BaseProjectView*>(doc->GetFirstView());
            if (view != nullptr)
                {
                FillMenuWithCustomTests(&view->GetDocFrame()->m_customTestsMenu, doc, true);
                FillMenuWithCustomTests(view->GetDocFrame()->m_customTestsRegularMenu, doc, true);
                }
            }
        }
    FillMenuWithCustomTests(&m_customTestsMenu, nullptr, false);
    FillMenuWithCustomTests(m_customTestsRegularMenu, nullptr, false);
    }

//-------------------------------------------------------
void MainFrame::FillReadabilityMenu(wxMenu* primaryMenu, wxMenu* secondaryMenu, wxMenu* adultMenu,
                                    wxMenu* secondLanguageMenu, const BaseProject* project)
    {
    if ((primaryMenu != nullptr) && (secondaryMenu != nullptr) && (adultMenu != nullptr) &&
        (secondLanguageMenu != nullptr) && (project != nullptr))
        {
        // clear the menus
        while (primaryMenu->GetMenuItemCount() != 0U)
            {
            primaryMenu->Destroy(primaryMenu->FindItemByPosition(0));
            }
        while (secondaryMenu->GetMenuItemCount() != 0U)
            {
            secondaryMenu->Destroy(secondaryMenu->FindItemByPosition(0));
            }
        while (adultMenu->GetMenuItemCount() != 0U)
            {
            adultMenu->Destroy(adultMenu->FindItemByPosition(0));
            }
        while (secondLanguageMenu->GetMenuItemCount() != 0U)
            {
            secondLanguageMenu->Destroy(secondLanguageMenu->FindItemByPosition(0));
            }
        // add the tests
        for (const auto& rTest : project->GetReadabilityTests().get_tests())
            {
            if (rTest.get_test().has_teaching_level(
                    readability::test_teaching_level::primary_grade) &&
                rTest.get_test().has_language(project->GetProjectLanguage()))
                {
                auto* testItem = new wxMenuItem(primaryMenu, rTest.get_test().get_interface_id(),
                                                rTest.get_test().get_long_name().c_str(),
                                                rTest.get_test().get_long_name().c_str());
                const auto bmp = wxGetApp().GetResourceManager().GetSVG(
                    wxString::Format(L"tests/%s.svg", rTest.get_test().get_id().c_str()));
                if (bmp.IsOk())
                    {
                    testItem->SetBitmap(bmp);
                    }
                primaryMenu->Append(testItem);
                }
            if (rTest.get_test().has_teaching_level(
                    readability::test_teaching_level::secondary_grade) &&
                rTest.get_test().has_language(project->GetProjectLanguage()))
                {
                auto* testItem = new wxMenuItem(secondaryMenu, rTest.get_test().get_interface_id(),
                                                rTest.get_test().get_long_name().c_str(),
                                                rTest.get_test().get_long_name().c_str());
                const auto bmp = wxGetApp().GetResourceManager().GetSVG(
                    wxString::Format(L"tests/%s.svg", rTest.get_test().get_id().c_str()));
                if (bmp.IsOk())
                    {
                    testItem->SetBitmap(bmp);
                    }
                secondaryMenu->Append(testItem);
                }
            if (rTest.get_test().has_teaching_level(
                    readability::test_teaching_level::adult_level) &&
                rTest.get_test().has_language(project->GetProjectLanguage()))
                {
                auto* testItem = new wxMenuItem(adultMenu, rTest.get_test().get_interface_id(),
                                                rTest.get_test().get_long_name().c_str(),
                                                rTest.get_test().get_long_name().c_str());
                const auto bmp = wxGetApp().GetResourceManager().GetSVG(
                    wxString::Format(L"tests/%s.svg", rTest.get_test().get_id().c_str()));
                if (bmp.IsOk())
                    {
                    testItem->SetBitmap(bmp);
                    }
                adultMenu->Append(testItem);
                }
            if (rTest.get_test().has_teaching_level(
                    readability::test_teaching_level::second_language) &&
                rTest.get_test().has_language(project->GetProjectLanguage()))
                {
                auto* testItem =
                    new wxMenuItem(secondLanguageMenu, rTest.get_test().get_interface_id(),
                                   rTest.get_test().get_long_name().c_str(),
                                   rTest.get_test().get_long_name().c_str());
                const auto bmp = wxGetApp().GetResourceManager().GetSVG(
                    wxString::Format(L"tests/%s.svg", rTest.get_test().get_id().c_str()));
                if (bmp.IsOk())
                    {
                    testItem->SetBitmap(bmp);
                    }
                secondLanguageMenu->Append(testItem);
                }
            }
        // add Dolch
        if (project->GetProjectLanguage() == readability::test_language::english_test)
            {
            const auto bp = wxGetApp().GetResourceManager().GetSVG(L"tests/dolch.svg");

                {
                auto* dolchItem = new wxMenuItem(primaryMenu, XRCID("ID_DOLCH"),
                                                 _(L"Dolch Sight Words"), _(L"Dolch Sight Words"));
                if (bp.IsOk())
                    {
                    dolchItem->SetBitmap(bp);
                    }
                primaryMenu->AppendSeparator();
                primaryMenu->Append(dolchItem);
                }

                {
                auto* dolchItem = new wxMenuItem(secondLanguageMenu, XRCID("ID_DOLCH"),
                                                 _(L"Dolch Sight Words"), _(L"Dolch Sight Words"));
                if (bp.IsOk())
                    {
                    dolchItem->SetBitmap(bp);
                    }
                secondLanguageMenu->AppendSeparator();
                secondLanguageMenu->Append(dolchItem);
                }
            }
        }
    }

//-------------------------------------------------------
void MainFrame::AddExamplesToMenu(wxMenu* exampleMenu)
    {
    if (exampleMenu != nullptr)
        {
        wxArrayString files;
        const wxString exampleFolder =
            wxGetApp().FindResourceDirectory(_DT(L"examples", DTExplanation::FilePath));
        if (!wxFileName::DirExists(exampleFolder) ||
            wxDir::GetAllFiles(exampleFolder, &files, wxString{}, wxDIR_FILES) == 0)
            {
            wxLogWarning(L"Unable to find examples folder: %s", exampleFolder);
            return;
            }
        files.Sort();
        // go through all the example files and add them to the menu
        for (size_t i = 0; i < files.GetCount(); ++i)
            {
            const wxFileName fName(files[i]);
            // see if we already have a menu ID for this example file; otherwise, make a new ID
            auto menuPos = m_examplesMenuIds.begin();
            for (/*initialized already*/; menuPos != m_examplesMenuIds.end(); ++menuPos)
                {
                if (menuPos->second == files[i])
                    {
                    break;
                    }
                }
            const int menuId =
                (menuPos == m_examplesMenuIds.end()) ? EXAMPLE_RANGE.GetNextId() : menuPos->first;
            // bail if we run out of menu IDs
            if (menuId == wxNOT_FOUND)
                {
                break;
                }
            if (menuPos == m_examplesMenuIds.end())
                {
                m_examplesMenuIds.insert(std::make_pair(menuId, files[i]));
                }
            if (exampleMenu->FindItem(menuId) == nullptr)
                {
                exampleMenu->Append(menuId, fName.GetName(), files[i]);
                }
            }
        }
    }

//-------------------------------------------------------
void MainFrame::FillMenuWithTestBundles(wxMenu* testBundleMenu, const BaseProject* project,
                                        const bool includeDocMenuItems)
    {
    if (testBundleMenu != nullptr)
        {
        // clear the menu
        while (testBundleMenu->GetMenuItemCount() != 0U)
            {
            testBundleMenu->Destroy(testBundleMenu->FindItemByPosition(0));
            }
        if (includeDocMenuItems)
            {
            auto* industryMenu = new wxMenu();
            industryMenu->Append(XRCID("ID_ADD_CHILDRENS_PUBLISHING_TEST_BUNDLE"),
                                 _(L"Children's Publishing"));
            industryMenu->Append(XRCID("ID_ADD_ADULT_PUBLISHING_TEST_BUNDLE"),
                                 _(L"Young Adult and Adult Publishing"));
            industryMenu->Append(XRCID("ID_ADD_CHILDRENS_HEALTHCARE_TEST_BUNDLE"),
                                 _(L"Children's Healthcare"));
            industryMenu->Append(XRCID("ID_ADD_ADULT_HEALTHCARE_TEST_BUNDLE"),
                                 _(L"Adult Healthcare"));
            industryMenu->Append(XRCID("ID_ADD_MILITARY_COVERNMENT_TEST_BUNDLE"),
                                 _(L"Military & Government"));
            industryMenu->Append(XRCID("ID_ADD_SECOND_LANGUAGE_TEST_BUNDLE"),
                                 _(L"Second Language Education"));
            industryMenu->Append(XRCID("ID_ADD_BROADCASTING_TEST_BUNDLE"), _(L"Broadcasting"));
            testBundleMenu->AppendSubMenu(industryMenu, _(L"Industry"));
            auto* docTypeMenu = new wxMenu();
            docTypeMenu->Append(XRCID("ID_GENERAL_DOCUMENT_TEST_BUNDLE"), _(L"General Document"));
            docTypeMenu->Append(XRCID("ID_TECHNICAL_DOCUMENT_TEST_BUNDLE"),
                                _(L"Technical Document"));
            docTypeMenu->Append(XRCID("ID_NONNARRATIVE_DOCUMENT_TEST_BUNDLE"),
                                _(L"Structured Form"));
            docTypeMenu->Append(XRCID("ID_ADULT_LITERATURE_DOCUMENT_TEST_BUNDLE"),
                                _(L"Literature"));
            docTypeMenu->Append(XRCID("ID_CHILDRENS_LITERATURE_DOCUMENT_TEST_BUNDLE"),
                                _(L"Children's Literature"));
            testBundleMenu->AppendSubMenu(docTypeMenu, _(L"General Document Type"));
            }

        // add the bundles to the menu
        if (includeDocMenuItems && !GetTestBundleMenuIds().empty())
            {
            bool separatorNeeded = true; // NOLINT(misc-const-correctness)
            // add all the global test bundles to this menu (if they aren't already on it)
            for (const auto& bundle : MainFrame::GetTestBundleMenuIds())
                {
                if (testBundleMenu->FindItem(bundle.first) == nullptr)
                    {
                    auto testIter =
                        BaseProject::m_testBundles.find(TestBundle(bundle.second.wc_str()));
                    if (testIter == BaseProject::m_testBundles.cend())
                        {
                        wxMessageBox(_(L"Unable to add test bundle to menu: "
                                       "internal error, please contact software vendor."),
                                     _(L"Error"), wxOK | wxICON_ERROR);
                        return;
                        }
                    // make sure that if the bundle has a language (meaning it is a standard system
                    // one), then make sure it matches the project.
                    if ((project != nullptr) &&
                        (testIter->GetLanguage() == readability::test_language::unknown_language ||
                         testIter->GetLanguage() == project->GetProjectLanguage()))
                        {
                        if (separatorNeeded)
                            {
                            testBundleMenu->AppendSeparator();
                            separatorNeeded = false;
                            }
                        auto* item = new wxMenuItem(testBundleMenu, bundle.first,
                                                    testIter->GetName().c_str());
                        if (testIter->GetName() == ReadabilityMessages::GetPskBundleName().wc_str())
                            {
                            item->SetBitmap(
                                wxGetApp().GetResourceManager().GetSVG(L"tests/psk-test.svg"));
                            }
                        else if (testIter->GetName() ==
                                 ReadabilityMessages::GetKincaidNavyBundleName().wc_str())
                            {
                            item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(
                                L"tests/flesch-kincaid-test.svg"));
                            }
                        else if (testIter->GetName() ==
                                 ReadabilityMessages::GetConsentFormsBundleName().wc_str())
                            {
                            item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(
                                L"ribbon/cdc-femaledoc.svg"));
                            }
                        testBundleMenu->Append(item);
                        }
                    }
                }
            }
        // Add/Edit/Remove options
        if (includeDocMenuItems)
            {
            testBundleMenu->AppendSeparator();
            }
        auto* menuItem =
            new wxMenuItem(testBundleMenu, XRCID("ID_ADD_CUSTOM_TEST_BUNDLE"), _(L"Add..."));
        menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/add.svg"));
        testBundleMenu->Append(menuItem);
        // if there are editable bundles, then add menu items for them
        bool hasRemovableBundles{ false }; // NOLINT(misc-const-correctness)
        for (const auto& bundle : BaseProject::m_testBundles)
            {
            if (!bundle.IsLocked())
                {
                hasRemovableBundles = true;
                break;
                }
            }
        // locked ones can still be viewed from the editor
        menuItem =
            new wxMenuItem(testBundleMenu, XRCID("ID_EDIT_CUSTOM_TEST_BUNDLE"), _(L"Edit..."));
        menuItem->SetBitmap(wxArtProvider::GetBitmapBundle(L"ID_EDIT"));
        testBundleMenu->Append(menuItem);
        if (hasRemovableBundles)
            {
            menuItem = new wxMenuItem(testBundleMenu, XRCID("ID_REMOVE_CUSTOM_TEST_BUNDLE"),
                                      _(L"Remove..."));
            menuItem->SetBitmap(wxArtProvider::GetBitmapBundle(wxART_DELETE));
            testBundleMenu->Append(menuItem);
            }
        }
    }

//-------------------------------------------------------
void MainFrame::FillMenuWithCustomTests(wxMenu* customTestMenu, const BaseProject* project,
                                        const bool includeDocMenuItems)
    {
    if (customTestMenu != nullptr)
        {
        // clear the menu
        while (customTestMenu->GetMenuItemCount() != 0U)
            {
            customTestMenu->Destroy(customTestMenu->FindItemByPosition(0));
            }

        // if there are tests then add editing options and the tests themselves
        if (includeDocMenuItems && !GetCustomTestMenuIds().empty())
            {
            // add all the global custom tests to this view's menu (if they aren't already on it)
            for (const auto& customMenuId : GetCustomTestMenuIds())
                {
                if (customTestMenu->FindItem(customMenuId.first) == nullptr)
                    {
                    auto testIter =
                        std::find(BaseProject::m_custom_word_tests.begin(),
                                  BaseProject::m_custom_word_tests.end(), customMenuId.second);
                    if (testIter == BaseProject::m_custom_word_tests.end())
                        {
                        wxMessageBox(_(L"Unable to add custom test to menu: "
                                       "internal error, please contact software vendor."),
                                     _(L"Error"), wxOK | wxICON_ERROR);
                        return;
                        }
                    auto* item = new wxMenuItem(customTestMenu, customMenuId.first,
                                                testIter->get_name().c_str());
                    customTestMenu->Append(item);
                    }
                }
            }
        if (includeDocMenuItems && !GetCustomTestMenuIds().empty())
            {
            customTestMenu->AppendSeparator();
            }
        auto* menuItem = new wxMenuItem(customTestMenu, XRCID("ID_ADD_CUSTOM_TEST"), _(L"Add..."));
        menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/add.svg"));
        customTestMenu->Append(menuItem);

        menuItem = new wxMenuItem(customTestMenu, XRCID("ID_ADD_CUSTOM_TEST_BASED_ON"),
                                  _(L"Add Custom Test Based on..."));
        menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/add.svg"));
        customTestMenu->Append(menuItem);
        if ((project == nullptr) ||
            project->GetProjectLanguage() == readability::test_language::english_test)
            {
            menuItem = new wxMenuItem(
                customTestMenu, XRCID("ID_ADD_CUSTOM_NEW_DALE_CHALL_TEST"),
                wxString::Format(_(L"Add Custom \"%s\"..."), _DT(L"New Dale-Chall")));
            menuItem->SetBitmap(
                wxGetApp().GetResourceManager().GetSVG(L"tests/dale-chall-test.svg"));
            customTestMenu->Append(menuItem);

            menuItem = new wxMenuItem(customTestMenu, XRCID("ID_ADD_CUSTOM_SPACHE_TEST"),
                                      wxString::Format(_(L"Add Custom \"%s\"..."), _DT(L"Spache")));
            menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"tests/spache-test.svg"));
            customTestMenu->Append(menuItem);

            menuItem = new wxMenuItem(
                customTestMenu, XRCID("ID_ADD_CUSTOM_HARRIS_JACOBSON_TEST"),
                wxString::Format(_(L"Add Custom \"%s\"..."), _DT(L"Harris-Jacobson")));
            menuItem->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"tests/spache-test.svg"));
            customTestMenu->Append(menuItem);
            }

        // Remove or Edit options, depending on whether there are any tests
        if (!GetCustomTestMenuIds().empty())
            {
            customTestMenu->AppendSeparator();
            menuItem = new wxMenuItem(customTestMenu, XRCID("ID_EDIT_CUSTOM_TEST"), _(L"Edit..."));
            menuItem->SetBitmap(wxArtProvider::GetBitmapBundle(L"ID_EDIT"));
            customTestMenu->Append(menuItem);

            menuItem =
                new wxMenuItem(customTestMenu, XRCID("ID_REMOVE_CUSTOM_TEST"), _(L"Remove..."));
            menuItem->SetBitmap(wxArtProvider::GetBitmapBundle(wxART_DELETE));
            customTestMenu->Append(menuItem);
            }
        }
    }

//-------------------------------------------------------
void MainFrame::OnRemoveCustomTest([[maybe_unused]] wxCommandEvent& event)
    {
    // make sure there aren't any projects getting updated before we start changing these tests
    const auto& docs = wxGetApp().GetDocManager()->GetDocuments();
    for (size_t i = 0; i < docs.GetCount(); ++i)
        {
        const auto* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
        if (doc != nullptr && !doc->IsSafeToUpdate())
            {
            return;
            }
        }
    wxArrayString testNames;
    for (const auto& customWordTest : BaseProject::m_custom_word_tests)
        {
        testNames.Add(customWordTest.get_name().c_str());
        }
    wxSingleChoiceDialog dlg(wxGetApp().GetParentingWindow(), _(L"Select test to remove:"),
                             _(L"Remove Test"), testNames);
    dlg.SetSize(FromDIP(400), -1);
    dlg.Center();
    if (dlg.ShowModal() == wxID_CANCEL)
        {
        return;
        }
    const auto selectedTestIndex = dlg.GetSelection();
    if (selectedTestIndex < 0 ||
        std::cmp_greater_equal(selectedTestIndex, BaseProject::m_custom_word_tests.size()))
        {
        return;
        }

    const CustomReadabilityTest& selectedTest = BaseProject::m_custom_word_tests[selectedTestIndex];

    BaseProjectDoc::RemoveGlobalCustomReadabilityTest(selectedTest.get_name().c_str());
    }

//-------------------------------------------------------
void MainFrame::OnAddCustomTestBundle([[maybe_unused]] wxCommandEvent& event)
    {
    TestBundle bundle(L"");
    TestBundleDlg bundleDlg(wxGetApp().GetParentingWindow(), bundle);
    bundleDlg.SetHelpTopic(GetHelpDirectory(), L"online/custom-tests-and-bundles.html");
    if (bundleDlg.ShowModal() == wxID_OK)
        {
        BaseProject::m_testBundles.insert(bundle);
        dynamic_cast<MainFrame*>(wxGetApp().GetMainFrame())
            ->AddTestBundleToMenus(bundle.GetName().c_str());
        }
    }

//-------------------------------------------------------
void MainFrame::OnEditCustomTestBundle([[maybe_unused]] wxCommandEvent& event)
    {
    wxArrayString bundleNames;
    for (const auto& testBundle : BaseProject::m_testBundles)
        {
        bundleNames.Add(testBundle.GetName().c_str());
        } // locked bundled will be viewed as read only
    wxSingleChoiceDialog dlg(wxGetApp().GetParentingWindow(), _(L"Select bundle to edit:"),
                             _(L"Edit Bundle"), bundleNames);
    dlg.SetSize(FromDIP(400), -1);
    dlg.Center();
    if (dlg.ShowModal() == wxID_CANCEL)
        {
        return;
        }

    const auto testBundleIter =
        BaseProject::m_testBundles.find(TestBundle(dlg.GetStringSelection().wc_str()));
    if (testBundleIter == BaseProject::m_testBundles.end()) // shouldn't happen
        {
        return;
        }

    // We will make a copy of the bundle, edit it, and then reinsert it.
    TestBundle bundle = *testBundleIter;
    TestBundleDlg bundleDlg(wxGetApp().GetParentingWindow(), bundle);
    bundleDlg.SetHelpTopic(GetHelpDirectory(), L"online/custom-tests-and-bundles.html");
    if (bundleDlg.ShowModal() == wxID_OK && !bundle.IsLocked())
        {
        BaseProject::m_testBundles.erase(testBundleIter);
        BaseProject::m_testBundles.insert(bundle);
        }
    }

//-------------------------------------------------------
void MainFrame::OnRemoveCustomTestBundle([[maybe_unused]] wxCommandEvent& event)
    {
    wxArrayString bundleNames;
    for (const auto& testBundle : BaseProject::m_testBundles)
        {
        if (!testBundle.IsLocked())
            {
            bundleNames.Add(testBundle.GetName().c_str());
            }
        }
    wxSingleChoiceDialog dlg(wxGetApp().GetParentingWindow(), _(L"Select bundle to remove:"),
                             _(L"Remove Bundle"), bundleNames);
    dlg.SetSize(FromDIP(400), -1);
    dlg.Center();
    if (dlg.ShowModal() == wxID_CANCEL)
        {
        return;
        }

    auto testBundleIter =
        BaseProject::m_testBundles.find(TestBundle(dlg.GetStringSelection().wc_str()));
    if (testBundleIter == BaseProject::m_testBundles.end()) // shouldn't happen
        {
        return;
        }
    BaseProject::m_testBundles.erase(testBundleIter);

    dynamic_cast<MainFrame*>(wxGetApp().GetMainFrame())
        ->RemoveTestBundleFromMenus(dlg.GetStringSelection());
    }

//-------------------------------------------------------
void MainFrame::OnAddCustomTest(const wxCommandEvent& event)
    {
    // make sure there aren't any projects getting updated before we start changing these tests
    const auto& docs = wxGetApp().GetDocManager()->GetDocuments();
    for (size_t i = 0; i < docs.GetCount(); ++i)
        {
        const auto* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
        if (doc != nullptr && !doc->IsSafeToUpdate())
            {
            return;
            }
        }
    if (event.GetId() == XRCID("ID_ADD_CUSTOM_NEW_DALE_CHALL_TEST"))
        {
        NewCustomWordTestSimpleDlg dlg(
            wxGetApp().GetParentingWindow(), wxID_ANY,
            wxString::Format(_(L"Add Custom \"%s\""), _DT(L"New Dale-Chall")));
        dlg.SetHelpTopic(GetHelpDirectory(), _DT(L"online/custom-tests-and-bundles.html"));
        if (dlg.ShowModal() == wxID_OK)
            {
            CustomReadabilityTest cTest(
                dlg.GetTestName().wc_str(),
                ReadabilityFormulaParser::GetCustomNewDaleChallSignature().wc_str(),
                readability::readability_test_type::grade_level, dlg.GetWordListFilePath().wc_str(),
                stemming::stemming_type::no_stemming, true, true,
                &BaseProject::m_dale_chall_word_list, false, &BaseProject::m_spache_word_list,
                false, &BaseProject::m_harris_jacobson_word_list,
                wxGetApp().GetAppOptions()->IsIncludingStockerCatholicSupplement(),
                &BaseProject::m_stocker_catholic_word_list, false,
                wxGetApp().GetAppOptions()->GetDaleChallProperNounCountingMethod(), true, false,
                false, false, false, false, false, false, false, false, false, false, false);
            ProjectDoc::AddGlobalCustomReadabilityTest(cTest);
            }
        }
    else if (event.GetId() == XRCID("ID_ADD_CUSTOM_SPACHE_TEST"))
        {
        NewCustomWordTestSimpleDlg dlg(wxGetApp().GetParentingWindow(), wxID_ANY,
                                       wxString::Format(_(L"Add Custom \"%s\""), _DT(L"Spache")));
        dlg.SetHelpTopic(GetHelpDirectory(), _DT(L"online/custom-tests-and-bundles.html"));
        if (dlg.ShowModal() == wxID_OK)
            {
            CustomReadabilityTest cTest(
                dlg.GetTestName().wc_str(),
                ReadabilityFormulaParser::GetCustomSpacheSignature().wc_str(),
                readability::readability_test_type::grade_level, dlg.GetWordListFilePath().wc_str(),
                stemming::stemming_type::no_stemming, true, false,
                &BaseProject::m_dale_chall_word_list, true, &BaseProject::m_spache_word_list, false,
                &BaseProject::m_harris_jacobson_word_list, false,
                &BaseProject::m_stocker_catholic_word_list, false,
                readability::proper_noun_counting_method::all_proper_nouns_are_familiar, true,
                false, false, false, false, false, false, false, false, false, false, false, false);
            ProjectDoc::AddGlobalCustomReadabilityTest(cTest);
            }
        }
    else if (event.GetId() == XRCID("ID_ADD_CUSTOM_TEST_BASED_ON"))
        {
        wxArrayString tests;
        for (const auto& rTest : BaseProject::GetDefaultReadabilityTestsTemplate().get_tests())
            {
            if (!rTest.get_formula().empty())
                {
                tests.push_back(rTest.get_long_name().c_str());
                }
            }
        wxSingleChoiceDialog lDlg(
            wxGetApp().GetParentingWindow(),
            _(L"Select a linear-regression-based test to use as the basis for a new custom test:"),
            _(L"Select a Test"), tests);
        if (lDlg.ShowModal() == wxID_OK)
            {
            auto selected = lDlg.GetStringSelection();
            if (!selected.empty())
                {
                auto [testIterator, found] =
                    BaseProject::GetDefaultReadabilityTestsTemplate().find_test(selected);
                if (found)
                    {
                    CustomTestDlg dlg(wxGetApp().GetParentingWindow());
                    dlg.SetTestName(wxString::Format(
                        // TRANSLATORS: %s is a readability test name
                        _(L"Custom %s"), testIterator->get_long_name().c_str()));
                    dlg.SetFormula(testIterator->get_formula().c_str());
                    dlg.SetTestType(static_cast<int>(testIterator->get_test_type()));
                    if (dlg.ShowModal() == wxID_OK)
                        {
                        CustomReadabilityTest cTest(
                            dlg.GetTestName().wc_str(), dlg.GetFormula().wc_str(),
                            static_cast<readability::readability_test_type>(dlg.GetTestType()),
                            dlg.GetWordListFilePath().wc_str(), dlg.GetStemmingType(),
                            dlg.IsIncludingCustomWordList(), dlg.IsIncludingDaleChallList(),
                            &BaseProject::m_dale_chall_word_list, dlg.IsIncludingSpacheList(),
                            &BaseProject::m_spache_word_list, dlg.IsIncludingHJList(),
                            &BaseProject::m_harris_jacobson_word_list, dlg.IsIncludingStockerList(),
                            &BaseProject::m_stocker_catholic_word_list,
                            dlg.IsFamiliarWordsMustBeOnEachIncludedList(),
                            static_cast<readability::proper_noun_counting_method>(
                                dlg.GetProperNounMethod()),
                            dlg.IsIncludingNumeric(), dlg.IsChildrensPublishingSelected(),
                            dlg.IsAdultPublishingSelected(), dlg.IsSecondaryLanguageSelected(),
                            dlg.IsChildrensHealthCareTestSelected(),
                            dlg.IsAdultHealthCareTestSelected(), dlg.IsMilitaryTestSelected(),
                            dlg.IsBroadcastingSelected(), dlg.IsGeneralDocumentSelected(),
                            dlg.IsTechnicalDocumentSelected(), dlg.IsNonNarrativeFormSelected(),
                            dlg.IsYoungAdultAndAdultLiteratureSelected(),
                            dlg.IsChildrensLiteratureSelected());
                        ProjectDoc::AddGlobalCustomReadabilityTest(cTest);
                        }
                    }
                else
                    {
                    wxMessageBox(
                        wxString::Format(_(L"Internal error: unable to find '%s'."), selected[0]),
                        wxGetApp().GetAppName(), wxOK | wxICON_WARNING);
                    }
                }
            }
        }
    else if (event.GetId() == XRCID("ID_ADD_CUSTOM_HARRIS_JACOBSON_TEST"))
        {
        NewCustomWordTestSimpleDlg dlg(
            wxGetApp().GetParentingWindow(), wxID_ANY,
            wxString::Format(_(L"Add Custom \"%s\""), _DT(L"Harris-Jacobson")));
        dlg.SetHelpTopic(GetHelpDirectory(), _DT(L"online/custom-tests-and-bundles.html"));
        if (dlg.ShowModal() == wxID_OK)
            {
            CustomReadabilityTest cTest(
                dlg.GetTestName().wc_str(),
                ReadabilityFormulaParser::GetCustomHarrisJacobsonSignature().wc_str(),
                readability::readability_test_type::grade_level, dlg.GetWordListFilePath().wc_str(),
                stemming::stemming_type::no_stemming, true, false,
                &BaseProject::m_dale_chall_word_list, false, &BaseProject::m_spache_word_list, true,
                &BaseProject::m_harris_jacobson_word_list, false,
                &BaseProject::m_stocker_catholic_word_list, false,
                readability::proper_noun_counting_method::all_proper_nouns_are_familiar, true,
                false, false, false, false, false, false, false, false, false, false, false, false);
            ProjectDoc::AddGlobalCustomReadabilityTest(cTest);
            }
        }
    else
        {
        CustomTestDlg dlg(wxGetApp().GetParentingWindow());
        if (dlg.ShowModal() == wxID_OK)
            {
            CustomReadabilityTest cTest(
                dlg.GetTestName().wc_str(), dlg.GetFormula().wc_str(),
                static_cast<readability::readability_test_type>(dlg.GetTestType()),
                dlg.GetWordListFilePath().wc_str(), dlg.GetStemmingType(),
                dlg.IsIncludingCustomWordList(), dlg.IsIncludingDaleChallList(),
                &BaseProject::m_dale_chall_word_list, dlg.IsIncludingSpacheList(),
                &BaseProject::m_spache_word_list, dlg.IsIncludingHJList(),
                &BaseProject::m_harris_jacobson_word_list, dlg.IsIncludingStockerList(),
                &BaseProject::m_stocker_catholic_word_list,
                dlg.IsFamiliarWordsMustBeOnEachIncludedList(),
                static_cast<readability::proper_noun_counting_method>(dlg.GetProperNounMethod()),
                dlg.IsIncludingNumeric(), dlg.IsChildrensPublishingSelected(),
                dlg.IsAdultPublishingSelected(), dlg.IsSecondaryLanguageSelected(),
                dlg.IsChildrensHealthCareTestSelected(), dlg.IsAdultHealthCareTestSelected(),
                dlg.IsMilitaryTestSelected(), dlg.IsBroadcastingSelected(),
                dlg.IsGeneralDocumentSelected(), dlg.IsTechnicalDocumentSelected(),
                dlg.IsNonNarrativeFormSelected(), dlg.IsYoungAdultAndAdultLiteratureSelected(),
                dlg.IsChildrensLiteratureSelected());
            ProjectDoc::AddGlobalCustomReadabilityTest(cTest);
            }
        }
    }

//-------------------------------------------------------
void MainFrame::OnEditCustomTest([[maybe_unused]] wxCommandEvent& event)
    {
    // make sure there aren't any projects getting updated before we start changing these tests
    const auto& docs = wxGetApp().GetDocManager()->GetDocuments();
    for (size_t i = 0; i < docs.GetCount(); ++i)
        {
        const auto* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
        if (doc != nullptr && !doc->IsSafeToUpdate())
            {
            return;
            }
        }
    wxArrayString testNames;
    for (const auto& cTest : BaseProject::m_custom_word_tests)
        {
        testNames.Add(cTest.get_name().c_str());
        }
    wxSingleChoiceDialog selDlg(wxGetApp().GetParentingWindow(), _(L"Select test to edit:"),
                                _(L"Edit Test"), testNames);
    selDlg.SetSize(FromDIP(400), -1);
    selDlg.Center();
    if (selDlg.ShowModal() == wxID_CANCEL)
        {
        return;
        }
    const auto selectedTestIndex = selDlg.GetSelection();
    if (selectedTestIndex < 0 ||
        std::cmp_greater_equal(selectedTestIndex, BaseProject::m_custom_word_tests.size()))
        {
        return;
        }

    wxGetApp().EditCustomTest(BaseProject::m_custom_word_tests[selectedTestIndex]);
    }

//-------------------------------------------------------
void MainFrame::Paste()
    {
    // If we're in the script workbench, then let the focused window handle it.
    // This allows the editor to handle paste correctly instead of it falling
    // through to here and creating a new project.
    if (GetScriptWorkbench() != nullptr && GetScriptWorkbench()->IsShown())
        {
        wxWindow* focusWin = wxWindow::FindFocus();
        if (focusWin != nullptr && focusWin->IsKindOf(wxCLASSINFO(wxStyledTextCtrl)))
            {
            if (auto* stc = dynamic_cast<wxStyledTextCtrl*>(focusWin))
                {
                stc->Paste();
                return;
                }
            }
        }

    if (wxTheClipboard->Open())
        {
        if (wxTheClipboard->IsSupported(wxDF_TEXT))
            {
            wxTextDataObject data;
            wxTheClipboard->GetData(data);
            // create a standard project and dump the text into it
            const auto& templateList = m_docManager->GetTemplates();
            for (size_t i = 0; i < templateList.GetCount(); ++i)
                {
                auto* docTemplate = dynamic_cast<wxDocTemplate*>(templateList.Item(i)->GetData());
                if ((docTemplate != nullptr) &&
                    docTemplate->GetDocClassInfo()->IsKindOf(wxCLASSINFO(ProjectDoc)))
                    {
                    auto* newDoc = dynamic_cast<ProjectDoc*>(
                        docTemplate->CreateDocument(data.GetText(), wxDOC_NEW));
                    if ((newDoc != nullptr) && !newDoc->OnNewDocument())
                        {
                        // Document is implicitly deleted by DeleteAllViews
                        newDoc->DeleteAllViews();
                        newDoc = nullptr;
                        }
                    if ((newDoc != nullptr) && (newDoc->GetFirstView() != nullptr))
                        {
                        newDoc->GetFirstView()->Activate(true);
                        wxGetApp().GetDocManager()->ActivateView(newDoc->GetFirstView());
                        if (newDoc->GetDocumentWindow() != nullptr)
                            {
                            newDoc->GetDocumentWindow()->SetFocus();
                            }
                        }
                    break;
                    }
                }
            }
        else if (wxTheClipboard->IsSupported(wxDF_FILENAME))
            {
            wxFileDataObject data;
            wxTheClipboard->GetData(data);
            wxArrayString files = data.GetFilenames();
            for (size_t i = 0; i < files.GetCount(); ++i)
                {
                m_docManager->CreateDocument(files[i], wxDOC_NEW);
                }
            }
        else
            {
            wxMessageBox(_(L"No text is currently on the clipboard."), wxGetApp().GetAppName(),
                         wxOK | wxICON_INFORMATION);
            }
        wxTheClipboard->Close();
        }
    }

//-------------------------------------------------------
void MainFrame::OnPaste([[maybe_unused]] wxCommandEvent& event) { Paste(); }

//-------------------------------------------------------
void MainFrame::OnPrinterHeaderFooter([[maybe_unused]] wxCommandEvent& event)
    {
    Wisteria::UI::PrinterHeaderFooterDlg dlg(wxGetApp().GetParentingWindow(),
                                             wxGetApp().GetAppOptions()->GetLeftPrinterHeader(),
                                             wxGetApp().GetAppOptions()->GetCenterPrinterHeader(),
                                             wxGetApp().GetAppOptions()->GetRightPrinterHeader(),
                                             wxGetApp().GetAppOptions()->GetLeftPrinterFooter(),
                                             wxGetApp().GetAppOptions()->GetCenterPrinterFooter(),
                                             wxGetApp().GetAppOptions()->GetRightPrinterFooter());
    dlg.SetHelpTopic(GetHelpDirectory(), _DT(L"online/publishing.html"));
    if (dlg.ShowModal() == wxID_OK)
        {
        wxGetApp().GetAppOptions()->SetLeftPrinterHeader(dlg.GetLeftPrinterHeader());
        wxGetApp().GetAppOptions()->SetCenterPrinterHeader(dlg.GetCenterPrinterHeader());
        wxGetApp().GetAppOptions()->SetRightPrinterHeader(dlg.GetRightPrinterHeader());
        wxGetApp().GetAppOptions()->SetLeftPrinterFooter(dlg.GetLeftPrinterFooter());
        wxGetApp().GetAppOptions()->SetCenterPrinterFooter(dlg.GetCenterPrinterFooter());
        wxGetApp().GetAppOptions()->SetRightPrinterFooter(dlg.GetRightPrinterFooter());
        }
    }

//---------------------------------------------------
void MainFrame::OnPrintWatermark([[maybe_unused]] wxCommandEvent& event)
    {
    wxTextEntryDialog textDlg(this,
                              _(L"Enter watermark:\n\n(Note that the tags @DATETIME@, @DATE@, "
                                L"and @TIME@ can be used\n"
                                "to dynamically expand into the current date and time.)"),
                              _(L"Watermark"), wxGetApp().GetAppOptions()->GetWatermark().m_label,
                              wxTextEntryDialogStyle | wxTE_MULTILINE);
    if (textDlg.ShowModal() == wxID_OK)
        {
        auto watermark = wxGetApp().GetAppOptions()->GetWatermark();
        watermark.m_label = textDlg.GetValue();
        wxGetApp().GetAppOptions()->SetWatermark(watermark);
        }
    }

//-------------------------------------------------------
void MainFrame::OnClose(wxCloseEvent& event)
    {
    // If project windows are still open, then the user is just dismissing this frame
    // (e.g., the log or script workbench embedded here), not the whole app.
    if (event.CanVeto() && wxGetApp().GetDocumentCount() > 0)
        {
        Hide();
        event.Veto();
        return;
        }

    if (LuaInterpreter::IsRunning())
        {
        ActivateScriptWorkbench();
        wxMessageBox(_(L"A script is still running. Please stop it before closing."),
                     _(L"Script Running"), wxOK | wxICON_EXCLAMATION);
        event.Veto();
        return;
        }

    const auto& docs = wxGetApp().GetDocManager()->GetDocuments();
    for (size_t i = 0; i < docs.GetCount(); ++i)
        {
        const auto* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
        if (doc != nullptr && doc->IsProcessing())
            {
            wxMessageBox(_(L"Project still processing. Please wait before closing."),
                         wxFileName::StripExtension(doc->GetTitle()), wxOK | wxICON_EXCLAMATION);
            event.Veto();
            return;
            }
        }
    if (GetScriptWorkbench() != nullptr && !GetScriptWorkbench()->PromptToSaveUnsavedScripts())
        {
        ActivateScriptWorkbench();
        event.Veto();
        return;
        }
    wxGetApp().GetAppOptions()->SetAppWindowMaximized(IsMaximized());
    wxGetApp().GetAppOptions()->SetAppWindowWidth(GetSize().GetWidth());
    wxGetApp().GetAppOptions()->SetAppWindowHeight(GetSize().GetHeight());
    wxGetApp().GetAppOptions()->SetPaperId(wxGetApp().GetPrintData()->GetPaperId());
    wxGetApp().GetAppOptions()->SetPaperOrientation(wxGetApp().GetPrintData()->GetOrientation());
    event.Skip();
    }

//-------------------------------------------------------
void MainFrame::OnOpenDocument([[maybe_unused]] wxCommandEvent& event)
    {
    wxFileDialog dialog(wxGetApp().GetParentingWindow(), _(L"Select Document to Analyze"),
                        wxString{}, wxString{}, ReadabilityAppOptions::GetDocumentFilter(),
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);
    if (dialog.ShowModal() == wxID_CANCEL)
        {
        return;
        }
    const auto& templateList = wxGetApp().GetDocManager()->GetTemplates();
    for (size_t i = 0; i < templateList.GetCount(); ++i)
        {
        auto* docTemplate = dynamic_cast<wxDocTemplate*>(templateList.Item(i)->GetData());
        if ((docTemplate != nullptr) &&
            docTemplate->GetDocClassInfo()->IsKindOf(wxCLASSINFO(ProjectDoc)))
            {
            auto* newDoc =
                dynamic_cast<ProjectDoc*>(docTemplate->CreateDocument(dialog.GetPath(), wxDOC_NEW));
            if ((newDoc != nullptr) && !newDoc->OnNewDocument())
                {
                // Document is implicitly deleted by DeleteAllViews
                newDoc->DeleteAllViews();
                newDoc = nullptr;
                }
            if ((newDoc != nullptr) && (newDoc->GetFirstView() != nullptr))
                {
                newDoc->GetFirstView()->Activate(true);
                wxGetApp().GetDocManager()->ActivateView(newDoc->GetFirstView());
                if (newDoc->GetDocumentWindow() != nullptr)
                    {
                    newDoc->GetDocumentWindow()->SetFocus();
                    }
                }
            break;
            }
        }
    }

//-------------------------------------------------------
void MainFrame::OnHelpContents([[maybe_unused]] wxCommandEvent& event)
    {
    const auto* activeProject =
        dynamic_cast<const BaseProjectDoc*>(GetDocumentManager()->GetCurrentDocument());
    if (activeProject == nullptr)
        {
        DisplayHelp(L"online/index.html");
        }
    else if (activeProject->IsKindOf(wxCLASSINFO(ProjectDoc)))
        {
        DisplayHelp(_DT(L"online/reviewing-standard-projects.html"));
        }
    else if (activeProject->IsKindOf(wxCLASSINFO(BatchProjectDoc)))
        {
        DisplayHelp(_DT(L"online/reviewing-batch-project.html"));
        }
    }

//-------------------------------------------------------
void MainFrame::OnHelpManual([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    const wxString manualPath =
        GetHelpDirectory() + wxFileName::GetPathSeparator() + _DT(L"readability-studio-manual.pdf");
    wxLaunchDefaultApplication(manualPath);
    }

//-------------------------------------------------------
void MainFrame::OnHelpSupport([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    wxLaunchDefaultBrowser(_READSTUDIO_ISSUE_PAGE);
    }

//-------------------------------------------------------
#ifndef APP_STORE_BUILD
void MainFrame::OnHelpCheckForUpdates([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    wxString updateFileContent, contentType, statusText;
    wxString updatedFilePath{ _READSTUDIO_REMOTE_VERSION_FILE };
    int responseCode{ 404 };
    if (!wxGetApp().GetWebHarvester().ReadWebPage(updatedFilePath, updateFileContent, contentType,
                                                  statusText, responseCode, false))
        {
        wxMessageBox(
            wxString::Format(_(L"An error occurred while trying to connect to the website:\t%s"),
                             QueueDownload::GetResponseMessage(responseCode)),
            _(L"Connection Error"), wxOK | wxICON_EXCLAMATION);
        return;
        }

    updateFileContent.Trim(false).Trim(true); // will just be a version string

    if (wxGetApp().GetAppVersion() < updateFileContent)
        {
        if (wxMessageBox(wxString::Format(_(L"There is a new version of %s currently available.\n"
                                            "Do you wish to go to the download page?"),
                                          wxGetApp().GetAppName()),
                         wxGetApp().GetAppName(), wxYES_NO | wxICON_QUESTION) == wxYES)
            {
            if (!::wxLaunchDefaultBrowser(_READSTUDIO_DOWNLOAD_PAGE))
                {
                wxMessageBox(_(L"Unable to open default browser. Please make sure that you have an "
                               "Internet browser installed and are connected to the Internet."),
                             _(L"Error"), wxOK | wxICON_ERROR);
                }
            }
        }
    else
        {
        wxMessageBox(_(L"There are no updates currently available."), wxGetApp().GetAppName(),
                     wxOK | wxICON_INFORMATION);
        }
    }
#endif

//-------------------------------------------------------
void MainFrame::RefreshOpenProjectsIfThemeChanged(const wxString& previousReportTheme)
    {
    if (wxGetApp().GetAppOptions()->GetReportTheme() == previousReportTheme)
        {
        return;
        }

    Wisteria::UI::ListCtrlEx::SetExportHeaderBackgroundColour(
        ProjectReportFormat::GetThemeBannerColour(wxGetApp().GetAppOptions()->GetReportTheme()));

    auto& docs = wxGetApp().GetDocManager()->GetDocuments();
    for (size_t i = 0; i < docs.GetCount(); ++i)
        {
        auto* doc = dynamic_cast<BaseProjectDoc*>(docs.Item(i)->GetData());
        if (doc != nullptr)
            {
            doc->RefreshRequired(ProjectRefresh::Minimal);
            // the theme is baked into the highlighted-text HTML, so reformat it too
            doc->RefreshRequired(ProjectRefresh::TextSection);
            doc->RefreshProject();
            }
        }

    if (wxGetApp().GetMainFrameEx()->GetScriptWorkbench() != nullptr)
        {
        wxGetApp().GetMainFrameEx()->GetScriptWorkbench()->UpdateTheme();
        }
    }

//-------------------------------------------------------
void MainFrame::OnToolsOptions([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    const wxString previousReportTheme = wxGetApp().GetAppOptions()->GetReportTheme();
    ToolsOptionsDlg optionsDlg(wxGetApp().GetParentingWindow());
    if (optionsDlg.ShowModal() == wxID_OK)
        {
        wxGetApp().GetAppOptions()->SaveOptionsFile();
        RefreshOpenProjectsIfThemeChanged(previousReportTheme);
        }
    }

//-------------------------------------------------------
void MainFrame::OnEditWordList([[maybe_unused]] wxCommandEvent& event)
    {
    EditWordListDlg editDlg(wxGetApp().GetParentingWindow(), wxID_ANY, _(L"Edit Word List"));
    editDlg.SetDefaultDir(wxGetApp().GetAppOptions()->GetWordListPath());
    editDlg.SetHelpTopic(GetHelpDirectory(), _DT(L"online/program-options.html"));
    if (editDlg.ShowModal() == wxID_OK)
        {
        wxGetApp().GetAppOptions()->SetWordListPath(wxFileName(editDlg.GetFilePath()).GetPath());
        }
    }

//-------------------------------------------------------
void MainFrame::OnEditPhraseList([[maybe_unused]] wxCommandEvent& event)
    {
    EditWordListDlg editDlg(wxGetApp().GetParentingWindow(), wxID_ANY, _(L"Edit Phrase List"));
    editDlg.SetDefaultDir(wxGetApp().GetAppOptions()->GetWordListPath());
    editDlg.SetHelpTopic(GetHelpDirectory(), _DT(L"online/program-options.html"));
    editDlg.SetPhraseFileMode(true);
    editDlg.ShowModal();
    }

//-------------------------------------------------------
void MainFrame::OnFindDuplicateFiles([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    Wisteria::UI::GetDirFilterDialog dirDlg(wxGetApp().GetParentingWindow(),
                                            ReadabilityAppOptions::GetDocumentFilter() + L"|" +
                                                Wisteria::GraphItems::Image::GetImageFileFilter() +
                                                L"|" + _(L"All Files (*.*)|*.*"));
    if (dirDlg.ShowModal() != wxID_OK || dirDlg.GetPath().empty())
        {
        return;
        }

    const wxWindowDisabler disableAll;
    // get the list of files
    wxArrayString files;
        {
        const wxBusyInfo wait(_(L"Retrieving files..."), this);
#ifdef __WXGTK__
        wxMilliSleep(100);
        wxGetApp().Yield();
#endif
        wxDir::GetAllFiles(dirDlg.GetPath(), &files, wxString{},
                           dirDlg.IsRecursive() ? wxDIR_FILES | wxDIR_DIRS : wxDIR_FILES);
        files = FilterFiles(files, ExtractExtensionsFromFileFilter(dirDlg.GetSelectedFileFilter()));
        }

    // get the checksums
    multi_value_aggregate_map<std::uint32_t, wxString> filesMap;
        {
        wxProgressDialog progressDlg(_(L"Duplicate Files"), _(L"Searching for duplicate files..."),
                                     files.size(), wxGetApp().GetParentingWindow(),
                                     wxPD_AUTO_HIDE | wxPD_SMOOTH | wxPD_ELAPSED_TIME |
                                         wxPD_ESTIMATED_TIME | wxPD_REMAINING_TIME |
                                         wxPD_CAN_ABORT | wxPD_APP_MODAL);
        progressDlg.Centre();

        int counter{ 1 }; // NOLINT(misc-const-correctness)
        for (const auto& curFile : files)
            {
            progressDlg.SetTitle(wxString::Format(
                _(L"Processing %s of %s files..."),
                wxNumberFormatter::ToString(counter, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                wxNumberFormatter::ToString(files.size(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep)));
            if (!progressDlg.Update(counter, wxFileName(curFile).GetFullName()))
                {
                return;
                }
            MemoryMappedFile memFile(curFile, true, true);
            if (memFile.IsOk())
                {
                const std::uint32_t crc =
                    CRC::Calculate(memFile.GetStream(), memFile.GetMapSize(), CRC::CRC_32());
                filesMap.insert(crc, curFile);
                }
            ++counter;
            }
        }

    Wisteria::UI::FileListDlg fileListDlg(wxGetApp().GetParentingWindow(), wxID_ANY,
                                          _(L"Duplicate Files"));
    fileListDlg.GetListCtrl()->SetVirtualDataSize(files.size());
    size_t rowCount{ 0 }; // NOLINT(misc-const-correctness)
    fileListDlg.GetListCtrl()->SetForegroundColour(wxColour{ 0, 0, 0 });
        // catalogue duplicates
        {
        const wxBusyInfo wait(_(L"Loading duplicates..."), this);
#ifdef __WXGTK__
        wxMilliSleep(100);
        wxGetApp().Yield();
#endif
        unsigned long groupId{ 1 };    // NOLINT(misc-const-correctness)
        bool alternatingColor{ true }; // NOLINT(misc-const-correctness)
        for (const auto& mapVal : filesMap.get_data())
            {
            if (mapVal.second.second > 1)
                {
                for (const auto& curFile : mapVal.second.first)
                    {
                    const wxFileName fn(curFile);
                    wxItemAttr attribs;
                    attribs.SetBackgroundColour((alternatingColor ?
                                                     wxColour{ 0, 255, 0 }.ChangeLightness(160) :
                                                     wxColour{ 255, 255, 255 }));
                    fileListDlg.GetListCtrlData()->SetRowAttributes(rowCount, attribs);
                    fileListDlg.GetListCtrlData()->SetItemText(
                        rowCount, 0, fn.GetFullName(),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    fileListDlg.GetListCtrlData()->SetItemText(
                        rowCount, 1, fn.GetPath(),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    fileListDlg.GetListCtrlData()->SetItemValue(rowCount++, 2, groupId);
                    }
                // flip it for next group of duplicates
                alternatingColor = !alternatingColor;
                ++groupId;
                }
            }
        fileListDlg.GetListCtrl()->SetVirtualDataSize(rowCount);
        fileListDlg.GetListCtrl()->DistributeColumns(-1);
        fileListDlg.GetInfoBar()->ShowMessage(
            wxString::Format(
                _(L"Found %lu duplicate files. You can select and delete duplicates from a group, "
                  "leaving one file for the group."),
                static_cast<decltype(groupId)>(fileListDlg.GetListCtrl()->GetItemCount()) -
                    (groupId - 1)),
            wxICON_INFORMATION);
        }
    if (rowCount == 0)
        {
        wxMessageBox(_(L"No duplicate files found."), wxGetApp().GetAppDisplayName(),
                     wxICON_INFORMATION | wxOK);
        }
    else
        {
        fileListDlg.ShowModal();
        }
    }

//-------------------------------------------------------
void MainFrame::OnToolsChapterSplit([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    wxFileDialog dialog(wxGetApp().GetParentingWindow(), _(L"Select File to Split"),
                        wxGetApp().GetAppOptions()->GetProjectPath(), wxString{},
                        ReadabilityAppOptions::GetDocumentFilter(),
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);

    if (dialog.ShowModal() == wxID_OK)
        {
        ChapterSplit cSplit;
        cSplit.SetOutputFolder(wxFileName(dialog.GetPath()).GetPath());
        try
            {
            MemoryMappedFile filemap(dialog.GetPath(), true, true);
            BaseProject project;
            auto extractedResult =
                project.ExtractRawText({ static_cast<const char*>(filemap.GetStream()),
                                         static_cast<size_t>(filemap.GetMapSize()) },
                                       wxFileName(dialog.GetPath()).GetExt());
            if (extractedResult.first)
                {
                cSplit.SplitIntoChapters(extractedResult.second.c_str());
                }
            else
                {
                wxMessageBox(_(L"Unable to split document."), wxGetApp().GetAppDisplayName(),
                             wxICON_EXCLAMATION | wxOK);
                }
            }
        catch (...)
            {
            wxMessageBox(_(L"Unable to split document."), wxGetApp().GetAppDisplayName(),
                         wxICON_EXCLAMATION | wxOK);
            return;
            }
        }
    }

//-------------------------------------------------------
void MainFrame::OnToolsWebHarvest([[maybe_unused]] wxRibbonButtonBarEvent& event)
    {
    WebHarvesterDlg webHarvestDlg(wxGetApp().GetParentingWindow(), wxArrayString{},
                                  wxString::Format(
                                      // TRANSLATORS: %s are file filters
                                      _(L"Documents & Images (%s;%s)|%s;%s|"),
                                      ReadabilityAppOptions::ALL_DOCUMENTS_WILDCARD.data(),
                                      ReadabilityAppOptions::ALL_IMAGES_WILDCARD.data(),
                                      ReadabilityAppOptions::ALL_DOCUMENTS_WILDCARD.data(),
                                      ReadabilityAppOptions::ALL_IMAGES_WILDCARD.data()) +
                                      ReadabilityAppOptions::GetDocumentFilter() + L"|" +
                                      Wisteria::GraphItems::Image::GetImageFileFilter(),
                                  wxGetApp().GetLastSelectedDocFilter(),
                                  // hide the option that disables local file downloading
                                  true);
    webHarvestDlg.UpdateFromHarvesterSettings(wxGetApp().GetWebHarvester());
    // force downloading locally
    webHarvestDlg.DownloadFilesLocally(true);
    webHarvestDlg.SetDownloadFolder(wxGetApp().GetAppOptions()->GetDownloadsPath());
    webHarvestDlg.SetHelpTopic(GetHelpDirectory(), _DT(L"online/additional-features.html"));

    if (webHarvestDlg.ShowModal() != wxID_OK)
        {
        return;
        }

    wxGetApp().SetLastSelectedWebPages(webHarvestDlg.GetUrls());
    wxGetApp().SetLastSelectedDocFilter(webHarvestDlg.GetSelectedDocFilter());
    webHarvestDlg.UpdateHarvesterSettings(wxGetApp().GetWebHarvester());

    // Holding down SHIFT will use a progress bar with a generic label on it that won't
    // resize itself to fit a different filename label.
    wxGetApp().GetWebHarvester().ShowFileNames(!wxGetMouseState().ShiftDown());

    wxString failedCrawls;
    for (size_t i = 0; i < webHarvestDlg.GetUrls().GetCount(); ++i)
        {
        const FilePathResolver resolver(webHarvestDlg.GetUrls().Item(i), true);
        wxGetApp().GetWebHarvester().SetUrl(resolver.GetResolvedPath());

        // if user cancelled harvesting, then stop
        if (!wxGetApp().GetWebHarvester().CrawlLinks())
            {
            break;
            }

        if (wxGetApp().GetWebHarvester().GetDownloadedFilePaths().empty())
            {
            failedCrawls += wxGetApp().GetWebHarvester().GetUrl() + L'\n';
            }
        }

    if (!failedCrawls.empty())
        {
        wxMessageBox(
            wxString::Format(_(L"No files were downloaded from the following sites:\n\n%s\n"
                               "Please review the log report for any possible connection issues."),
                             failedCrawls),
            _(L"Warning"), wxOK | wxICON_WARNING);
        }

    // In case JS cookies are being reused, clear them from the current session
    // (we are treating crawling multiple sites from here as one session, that restarts
    //  when the dialog is opened up again).
    wxGetApp().GetWebHarvester().ClearCookies();

    // update global internet options that mirror the same options from the dialog
    wxGetApp().GetAppOptions()->DisablePeerVerify(webHarvestDlg.IsPeerVerifyDisabled());
    wxGetApp().GetAppOptions()->UseJavaScriptCookies(webHarvestDlg.IsUsingJavaScriptCookies());
    wxGetApp().GetAppOptions()->PersistJavaScriptCookies(
        webHarvestDlg.IsPersistingJavaScriptCookies());
    wxGetApp().GetAppOptions()->SetUserAgent(webHarvestDlg.GetUserAgent());
    wxGetApp().GetAppOptions()->SetDownloadsPath(webHarvestDlg.GetDownloadFolder());

    wxMessageBox(_(L"Web crawl complete."), _(L"Web Harvester"), wxOK | wxICON_INFORMATION);
    }
