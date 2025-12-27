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

#ifndef RS_MAIN_APP_H
#define RS_MAIN_APP_H

#include "../Wisteria-Dataviz/src/math/mathematics.h"
#include "../Wisteria-Dataviz/src/ui/app.h"
#include "../Wisteria-Dataviz/src/ui/controls/codeeditor.h"
#include "../Wisteria-Dataviz/src/ui/controls/listctrlex.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/functionbrowserdlg.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/listdlg.h"
#include "../Wisteria-Dataviz/src/ui/dialogs/printerheaderfooterdlg.h"
#include "../Wisteria-Dataviz/src/ui/mainframe.h"
#include "../Wisteria-Dataviz/src/util/donttranslate.h"
#include "../Wisteria-Dataviz/src/util/formulaformat.h"
#include "../Wisteria-Dataviz/src/util/idhelpers.h"
#include "../Wisteria-Dataviz/src/util/logfile.h"
#include "../Wisteria-Dataviz/src/util/screenshot.h"
#include "../Wisteria-Dataviz/src/util/xml_format.h"
#include "../Wisteria-Dataviz/src/wxStartPage/startpage.h"
#include "../app/readability_app_options.h"
#include "../lua-scripting/lua_interface.h"
#include "../readability/custom_readability_test.h"
#include "../readability/readability_project_test.h"
#include "../test-helpers/tests_functional.h"
#include "../ui/dialogs/about_dlg_ex.h"
#include "../ui/dialogs/lua_editor_dlg.h"
#include "../ui/dialogs/web_harvester_dlg.h"
#include "../ui/dialogs/word_list_dlg.h"
#include "../webharvester/webharvester.h"
#include "version.h"
#include <algorithm>
#include <map>
#include <wx/evtloop.h>
#include <wx/file.h>
#include <wx/imaglist.h>
#include <wx/ribbon/art.h>
#include <wx/ribbon/bar.h>
#include <wx/ribbon/buttonbar.h>
#include <wx/ribbon/gallery.h>
#include <wx/ribbon/toolbar.h>
#include <wx/splitter.h>
#include <wx/srchctrl.h>
#include <wx/textdlg.h>
#include <wx/wx.h>

class BaseProject;

/// @brief Extended icon provider, which is connected to application's
///     custom icons.
class RSArtProvider final : public wxArtProvider
    {
  public:
    RSArtProvider();

  protected:
    [[nodiscard]]
    wxBitmapBundle CreateBitmapBundle(const wxArtID& id, const wxArtClient& client,
                                      const wxSize& size) final;

  private:
    std::map<wxArtID, wxString> m_idFileMap;
    };

class MainFrame final : public Wisteria::UI::BaseMainFrame
    {
  public:
    friend class ReadabilityApp;

    MainFrame(wxDocManager* manager, wxFrame* frame, const wxArrayString& defaultFileExtensions,
              const wxString& title, const wxPoint& pos, const wxSize& size, long type);
    MainFrame(const MainFrame&) = delete;
    MainFrame& operator=(const MainFrame&) = delete;

    ~MainFrame() override
        {
        wxLogDebug(__func__);
        // modeless dialogs that do not have parents
        if (m_logWindow != nullptr)
            {
            m_logWindow->Destroy();
            }
        if (m_luaEditor != nullptr)
            {
            m_luaEditor->Destroy();
            }
        }

    void OnAbout([[maybe_unused]] wxCommandEvent& event);
    void OnStartPageClick(const wxCommandEvent& event);
    void OnOpenDocument([[maybe_unused]] wxCommandEvent& event);
    void OnPaste([[maybe_unused]] wxCommandEvent& event);
    void OnPrinterHeaderFooter([[maybe_unused]] wxCommandEvent& event);
    void OnBlankGraph(const wxCommandEvent& event);
    void OnTestsOverview([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnViewLogReport([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnViewProfileReport([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnToolsOptions([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnEditWordList([[maybe_unused]] wxCommandEvent& event);
    void OnEditPhraseList([[maybe_unused]] wxCommandEvent& event);
    void OnToolsWebHarvest([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnToolsChapterSplit([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnFindDuplicateFiles([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnHelpContents([[maybe_unused]] wxCommandEvent& event) final;
    void OnHelpManual([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnHelpCheckForUpdates([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnHelpSupport([[maybe_unused]] wxRibbonButtonBarEvent& event);
    void OnOpenExample(const wxCommandEvent& event);
    // pane events
    void OnWordListByPage(const wxCommandEvent& event);
    void OnScriptEditor([[maybe_unused]] wxCommandEvent& event);
    // custom test events
    void OnAddCustomTest(const wxCommandEvent& event);
    void OnEditCustomTest([[maybe_unused]] wxCommandEvent& event);
    void OnRemoveCustomTest([[maybe_unused]] wxCommandEvent& event);
    // custom test bundles
    void OnAddCustomTestBundle([[maybe_unused]] wxCommandEvent& event);
    void OnEditCustomTestBundle([[maybe_unused]] wxCommandEvent& event);
    void OnRemoveCustomTestBundle([[maybe_unused]] wxCommandEvent& event);
    // ribbon events
    void OnWordList(wxRibbonButtonBarEvent& event);
    void OnWordListDropdown(wxRibbonButtonBarEvent& event);
    void OnBlankGraphDropdown(wxRibbonButtonBarEvent& event);
    void OnNewDropdown(wxRibbonButtonBarEvent& event);
    void OnOpenDropdown(wxRibbonButtonBarEvent& event);
    void OnPrintDropdown(wxRibbonButtonBarEvent& event);
    void OnCustomTestsDropdown(wxRibbonButtonBarEvent& event);
    void OnTestBundlesDropdown(wxRibbonButtonBarEvent& event);
    void OnExampleDropdown(wxRibbonButtonBarEvent& event);
    void OnDictionaryDropdown(wxRibbonButtonBarEvent& event);
    void OnRibbonBarHelpClicked([[maybe_unused]] wxRibbonBarEvent& event);

    void OnEditEnglishDictionary([[maybe_unused]] wxCommandEvent& event);
    void OnEditDictionarySettings([[maybe_unused]] wxCommandEvent& event);

    /// @todo move to app class and accept reference types
    static void FillMenuWithTestBundles(wxMenu* testBundleMenu, const BaseProject* project,
                                        const bool includeDocMenuItems);
    static void FillMenuWithCustomTests(wxMenu* customTestMenu, const BaseProject* project,
                                        const bool includeDocMenuItems);
    void AddExamplesToMenu(wxMenu* exampleMenu);
    static void FillReadabilityMenu(wxMenu* primaryMenu, wxMenu* secondaryMenu, wxMenu* adultMenu,
                                    wxMenu* secondLanguageMenu, const BaseProject* project);

    [[nodiscard]]
    static const std::map<int, wxString>& GetTestBundleMenuIds() noexcept
        {
        return m_testBundleMenuIds;
        }

    [[nodiscard]]
    static const std::map<int, wxString>& GetCustomTestMenuIds() noexcept
        {
        return m_customTestMenuIds;
        }

    [[nodiscard]]
    static const std::map<int, wxString>& GetExamplesMenuIds() noexcept
        {
        return m_examplesMenuIds;
        }

    void AddTestBundleToMenus(const wxString& bundleName);
    void RemoveTestBundleFromMenus(const wxString& bundleName);
    /// This also adds the new test to the menu of each open document/view,
    /// as well as the mainframe.
    void AddCustomTestToMenus(const wxString& testName);
    void RemoveCustomTestFromMenus(const wxString& testName);

    void Paste();

    [[nodiscard]]
    std::vector<wxBitmapBundle>& GetProjectSideBarImageList() noexcept
        {
        return m_projectSideBarImageList;
        }

    [[nodiscard]]
    wxStartPage* GetStartPage() const noexcept
        {
        return m_startPage;
        }

    /// @brief Sets the About dialog image.
    /// @param bmp The image.
    void SetAboutDialogImage(const wxBitmap& bmp) { m_aboutBmp = bmp; }

    /// @returns The image used for a custom About dialog.
    [[nodiscard]]
    const wxBitmap& GetAboutDialogImage() const noexcept
        {
        return m_aboutBmp;
        }

    // Note that some of these ribbon button bar IDs are used by
    // ribbons other than the project one. This is the communal collection
    // of all bar IDs for the program's ribbons.
    constexpr static wxWindowID ID_EDIT_RIBBON_BUTTON_BAR = wxID_HIGHEST;
    constexpr static wxWindowID ID_PROOFING_RIBBON_BUTTON_BAR = wxID_HIGHEST + 1;
    constexpr static wxWindowID ID_PARAGRAPH_DEDUCTION_RIBBON_BUTTON_BAR = wxID_HIGHEST + 2;
    constexpr static wxWindowID ID_TEXT_EXCLUSION_RIBBON_BUTTON_BAR = wxID_HIGHEST + 3;
    constexpr static wxWindowID ID_NUMERALS_RIBBON_BUTTON_BAR = wxID_HIGHEST + 4;
    constexpr static wxWindowID ID_PROJECT_RIBBON_BUTTON_BAR = wxID_HIGHEST + 5;
    constexpr static wxWindowID ID_VIEW_RIBBON_BUTTON_BAR = wxID_HIGHEST + 6;
    constexpr static wxWindowID ID_DOCUMENT_RIBBON_BUTTON_BAR = wxID_HIGHEST + 7;
    constexpr static wxWindowID ID_CLIPBOARD_RIBBON_BUTTON_BAR = wxID_HIGHEST + 8;

    constexpr static wxWindowID ID_EDIT_RIBBON_LIST_PANEL = wxID_HIGHEST + 9;
    // HTML window
    constexpr static wxWindowID ID_EDIT_RIBBON_SUMMARY_REPORT_PANEL = wxID_HIGHEST + 10;
    constexpr static wxWindowID ID_EDIT_RIBBON_EXPLANATION_LIST_PANEL = wxID_HIGHEST + 11;
    // formatted text control
    constexpr static wxWindowID ID_EDIT_RIBBON_REPORT_PANEL = wxID_HIGHEST + 12;
    constexpr static wxWindowID ID_EDIT_RIBBON_STATS_LIST_PANEL = wxID_HIGHEST + 13;
    constexpr static wxWindowID ID_EDIT_RIBBON_STATS_SUMMARY_REPORT_PANEL = wxID_HIGHEST + 14;
    constexpr static wxWindowID ID_EDIT_RIBBON_LIST_SIMPLE_PANEL = wxID_HIGHEST + 15;
    constexpr static wxWindowID ID_EDIT_RIBBON_LIST_SIMPLE_WITH_SUM_PANEL = wxID_HIGHEST + 16;
    constexpr static wxWindowID ID_EDIT_RIBBON_BAR_CHART_PANEL = wxID_HIGHEST + 17;
    constexpr static wxWindowID ID_EDIT_RIBBON_HISTOGRAM_PANEL = wxID_HIGHEST + 18;
    constexpr static wxWindowID ID_EDIT_RIBBON_BOX_PLOT_PANEL = wxID_HIGHEST + 19;
    constexpr static wxWindowID ID_EDIT_RIBBON_PIE_CHART_PANEL = wxID_HIGHEST + 20;
    constexpr static wxWindowID ID_EDIT_RIBBON_LIX_GERMAN_PANEL = wxID_HIGHEST + 21;
    constexpr static wxWindowID ID_EDIT_RIBBON_FRY_PANEL = wxID_HIGHEST + 22;
    constexpr static wxWindowID ID_EDIT_RIBBON_RAYGOR_PANEL = wxID_HIGHEST + 23;
    constexpr static wxWindowID ID_EDIT_RIBBON_SCHWARTZ_PANEL = wxID_HIGHEST + 24;
    constexpr static wxWindowID ID_EDIT_RIBBON_FLESCH_PANEL = wxID_HIGHEST + 25;
    constexpr static wxWindowID ID_EDIT_RIBBON_LIST_SIMPLE_WITH_SUM_AND_EXCLUDE_PANEL =
        wxID_HIGHEST + 26;
    constexpr static wxWindowID ID_EDIT_RIBBON_GRAPH_PANEL = wxID_HIGHEST + 27;
    constexpr static wxWindowID ID_EDIT_RIBBON_LIST_CSVSS_PANEL = wxID_HIGHEST + 28;
    constexpr static wxWindowID ID_EDIT_RIBBON_LIST_TEST_SCORES_PANEL = wxID_HIGHEST + 29;
    constexpr static wxWindowID ID_EDIT_RIBBON_HISTOGRAM_BATCH_PANEL = wxID_HIGHEST + 30;
    constexpr static wxWindowID ID_EDIT_RIBBON_WORDCLOUD_PANEL = wxID_HIGHEST + 31;
    constexpr static wxWindowID ID_EDIT_RIBBON_SYLLABLE_HISTOGRAM_PANEL = wxID_HIGHEST + 32;
    constexpr static wxWindowID ID_EDIT_RIBBON_GENERAL_READABILITY_GRAPH_PANEL = wxID_HIGHEST + 33;
    constexpr static wxWindowID ID_EDIT_RIBBON_DB2_PANEL = wxID_HIGHEST + 34;
    constexpr static wxWindowID ID_EDIT_RIBBON_LIX_PANEL = wxID_HIGHEST + 35;

    IdRangeLock CUSTOM_TEST_RANGE;
    IdRangeLock EXAMPLE_RANGE;
    IdRangeLock TEST_BUNDLE_RANGE;

    wxMenu m_fileOpenMenu;
    wxMenu m_printMenu;
    wxMenu m_exampleMenu;
    wxMenu m_customTestsMenu;
    wxMenu m_wordListMenu;
    wxMenu m_blankGraphMenu;
    wxMenu* m_customTestsRegularMenu{ nullptr };
    wxMenu m_testsBundleMenu;
    wxMenu* m_testsBundleRegularMenu{ nullptr };

    /// @returns The lua editor dialog.
    LuaEditorDlg* GetLuaEditor() noexcept { return m_luaEditor; }

    /// @returns The log report dialog.
    Wisteria::UI::ListDlg* GetLogWindow() noexcept { return m_logWindow; }

  private:
    static std::map<int, wxString> m_testBundleMenuIds;
    static std::map<int, wxString> m_customTestMenuIds;
    static std::map<int, wxString> m_examplesMenuIds;

    std::vector<wxBitmapBundle> m_projectSideBarImageList;

    wxStartPage* m_startPage{ nullptr };
    LuaEditorDlg* m_luaEditor{ nullptr };
    Wisteria::UI::ListDlg* m_logWindow{ nullptr };

    wxBitmap m_aboutBmp;

    wxDECLARE_CLASS(MainFrame);
    };

// Define a new application
class ReadabilityApp final : public Wisteria::UI::BaseApp
    {
  public:
    // NOLINTNEXTLINE(cert-msc32-c,cert-msc51-cpp) RNG is initialized later correctly
    ReadabilityApp() { SetClassName(_READSTUDIO_APP_RDNS_IDENTIFIER); }

    ReadabilityApp(const ReadabilityApp&) = delete;
    ReadabilityApp& operator=(const ReadabilityApp&) = delete;

    bool OnInit() final;
    int OnExit() final;

    void OnInitCmdLine(wxCmdLineParser& cmdParser) final;
    bool OnCmdLineParsed(wxCmdLineParser& cmdParser) final;
#ifdef __WXOSX__
    void MacOpenFiles(const wxArrayString& files) final;
#endif
    void LoadInterface();
    void LoadMenus();

    /// @brief Constructs the options handler.
    /// @warning This must be called after `BaseApp::OnInit()` has be called
    ///     so the construction of system fonts and colors are safe.
    void CreateAppOptions() { m_appOptions = std::make_unique<ReadabilityAppOptions>(); }

    [[nodiscard]]
    MainFrame* GetMainFrameEx() noexcept
        {
        return dynamic_cast<MainFrame*>(GetMainFrame());
        }

    [[nodiscard]]
    wxString GetAppVersion() const
        {
        return _READSTUDIO_PROGRAM_VERSION;
        }

    void EditCustomTest(CustomReadabilityTest& selectedTest);

    /// web harvester info
    [[nodiscard]]
    wxArrayString GetLastSelectedWebPages() const
        {
        return m_lastSelectedWebPages;
        }

    void SetLastSelectedWebPages(const wxArrayString& webpages)
        {
        m_lastSelectedWebPages = webpages;
        }

    [[nodiscard]]
    wxString GetLastSelectedDocFilter() const
        {
        return m_lastSelectedDocFilter;
        }

    void SetLastSelectedDocFilter(const wxString& filter) { m_lastSelectedDocFilter = filter; }

    [[nodiscard]]
    std::unique_ptr<ReadabilityAppOptions>& GetAppOptions() noexcept
        {
        wxASSERT_MSG(m_appOptions, L"Call CreateAppOptions() to construct this interface!");
        return m_appOptions;
        }

    [[nodiscard]]
    LuaInterpreter& GetLuaRunner() noexcept
        {
        return m_LuaRunner;
        }

    [[nodiscard]]
    WebHarvester& GetWebHarvester() noexcept
        {
        return m_webHarvester;
        }

    enum class RibbonType
        {
        MainFrameRibbon,
        StandardProjectRibbon,
        BatchProjectRibbon
        };

    // UI initializers
    void InitStartPage();
    void InitProjectSidebar();

    // ribbon creation
    Wisteria::UI::SideBar* CreateSideBar(wxWindow* frame, const wxWindowID id);
    wxRibbonBar* CreateRibbon(wxWindow* frame, const wxDocument* doc);
    void LoadRibbonHomePage(wxRibbonBar* ribbon, RibbonType rtype, const wxDocument* doc);
    void LoadRibbonDocumentPage(wxRibbonBar* ribbon, RibbonType rtype);
    void LoadRibbonReadabilityPage(wxRibbonBar* ribbon, RibbonType rtype);
    void LoadRibbonToolsPage(wxRibbonBar* ribbon, RibbonType rtype);
    void LoadRibbonHelpPage(wxRibbonBar* ribbon);

    // menu creation
    void FillPrintMenu(wxMenu& printMenu, const RibbonType rtype);
    void FillSaveMenu(wxMenu& saveMenu, const RibbonType rtype);
    static void FillGradeScalesMenu(wxMenu& menu);
    static void FillWordListsMenu(wxMenu& wordListMenu);
    static void FillBlankGraphsMenu(wxMenu& blankGraphsMenu);

    // theming
    void UpdateRibbonTheme();
    void UpdateDocumentThemes();
    void UpdateStartPageTheme();
    void UpdateScriptEditorTheme();
    void UpdateLogWindowTheme();
    void UpdateSideBarTheme(Wisteria::UI::SideBar* sidebar);
    void UpdateRibbonTheme(const wxRibbonBar* ribbon);

    /// Adds a list of words to the custom dictionary
    ///     (based on language of the project that it is coming from).
    void AddWordsToDictionaries(const wxArrayString& theWords,
                                const readability::test_language lang) const;
    void EditDictionary(const readability::test_language lang);
    /// This is just used for testing purposes, to validate that the word lists
    /// are properly presorted.
    static bool VerifyWordLists();

    void RemoveAllCustomTestBundles();

    /// @returns The DPI scaling factor (e.g., can be 2 on HiDPI displays).
    ///     This is a convenient way of getting this value when a window isn't
    ///     available.
    [[nodiscard]]
    double GetDPIScaleFactor() const noexcept
        {
        return m_dpiScaleFactor;
        }

    void AddSplashscreenImagePath(const wxString& imagePath)
        {
        m_splashscreenImagePaths.Add(imagePath);
        }

    /** @returns A bitmap from the resource manager, scaling it to the system's DPI.
        @param image The path to the image from the resource manager (e.g.,
            "ribbon/logo.png").
        @param type The image type of @c image.
        @param imageSize The size of the image (in pixels).\n
            This function will scale this according to the system's DPI.*/
    [[nodiscard]]
    wxBitmap GetScaledImage(const wxString& image, const wxBitmapType type = wxBITMAP_TYPE_PNG,
                            const wxSize imageSize = wxSize{ 32, 32 })
        {
        wxASSERT(GetMainFrame());
        if (GetMainFrame() == nullptr)
            {
            return wxNullBitmap;
            }
#ifndef __WXMSW__
        // For example, 2.0 on Retina
        const double scaling = GetMainFrame()->GetContentScaleFactor();

        const wxSize scaledSize{ static_cast<int>(std::lround(imageSize.GetWidth() * scaling)),
                                 static_cast<int>(std::lround(imageSize.GetHeight() * scaling)) };
#else
        const wxSize scaledSize = GetMainFrame()->FromDIP(imageSize);
#endif
        wxBitmap bmp = GetResourceManager().GetBitmap(image, type);

        // Only resize if being downscaled. If the original image is smaller than
        // the requested size, then return the original image (i.e., don't upscale it).
        const auto [width, height] = geometry::downscaled_size(
            std::make_pair(bmp.GetLogicalWidth(), bmp.GetLogicalHeight()),
            std::make_pair(scaledSize.GetWidth(), scaledSize.GetHeight()));

        wxBitmap::Rescale(bmp, wxSize{ static_cast<int>(width), static_cast<int>(height) });
#ifndef __WXMSW__
        bmp.SetScaleFactor(scaling);
#endif
        return bmp;
        }

    [[nodiscard]]
    const wxArrayString& GetSplashscreenPaths() const noexcept
        {
        return m_splashscreenImagePaths;
        }

    /// @returns A random number generator engine for use in calls to
    /// std::uniform_xxx().
    [[nodiscard]]
    std::mt19937_64& GetRandomNumberEngine() noexcept
        {
        return m_mersenneTwister;
        }

    void ShowSplashscreen();

    [[nodiscard]]
    static const std::map<wxWindowID, wxWindowID>& GetDynamicIdMap() noexcept
        {
        return m_dynamicIdMap;
        }

    [[nodiscard]]
    const std::map<wxString, wxString>& GetShapeMap() const noexcept
        {
        return m_shapeMap;
        }

    [[nodiscard]]
    const std::map<wxString, wxString>& GetGraphColorSchemeMap() const noexcept
        {
        return m_colorSchemeMap;
        }

  private:
    /// @brief IDs exposed to scripting and their respective dynamic IDs in the
    /// framework.
    /// @details Set (or add to this) in your framework's initialization.
    static std::map<wxWindowID, wxWindowID> m_dynamicIdMap;

    PreAppInitOptions m_preInitOptions;
    std::unique_ptr<ReadabilityAppOptions> m_appOptions{ nullptr };
    bool LoadWordLists(const wxString& AppSettingFolderPath);
    wxArrayString m_lastSelectedWebPages;
    wxString m_lastSelectedDocFilter;
    LuaInterpreter m_LuaRunner;
    wxString m_CustomEnglishDictionaryPath;
    wxString m_CustomSpanishDictionaryPath;
    wxString m_CustomGermanDictionaryPath;
    double m_dpiScaleFactor{ 1.0 };
    wxArrayString m_splashscreenImagePaths;
    WebHarvester m_webHarvester;
    std::mt19937_64 m_mersenneTwister;

    std::map<wxString, wxString> m_shapeMap;
    std::map<wxString, wxString> m_colorSchemeMap;

    std::vector<wxString> m_commandLineFilesToOpen;
    };

#endif // RS_MAIN_APP_H
