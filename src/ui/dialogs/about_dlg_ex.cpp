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

#include "about_dlg_ex.h"
#include "../../OleanderStemmingLibrary/src/stemming.h"
#include "../../Wisteria-Dataviz/src/CRCpp/inc/CRC.h"
#include "../../Wisteria-Dataviz/src/easyexif/exif.h"
#include "../../Wisteria-Dataviz/src/import/html_extract_text.h"
#include "../../Wisteria-Dataviz/src/ui/dialogs/dialogwithhelp.h"
#include "../../Wisteria-Dataviz/src/util/hardwareinfo.h"
#include "../../app/readability_app.h"
#include "../../app/sbom.h"
#include "../../lua/lua.h"
#include "../../tinyexpr-plusplus/tinyexpr.h"
#include "../../tinyxml2/tinyxml2.h"
#include <algorithm>
#include <utility>
#include <wx/generic/statbmpg.h>
#include <wx/stc/stc.h>
#include <wx/webview.h>
#include <wx/zstream.h>

wxDECLARE_APP(ReadabilityApp);

//------------------------------------------------------
AboutDialogEx::AboutDialogEx(wxWindow* parent, wxBitmap banner, wxString appVersion,
                             wxString copyright, wxString eula, wxString mlaCitation,
                             wxString apaCitation, wxString bibtexCitation, wxWindowID id,
                             const wxPoint& pos, const wxSize& size, long style)
    : m_banner(std::move(banner)), m_appVersion(std::move(appVersion)),
      m_copyright(std::move(copyright)), m_eula(std::move(eula)),
      m_mlaCitation(std::move(mlaCitation)), m_apaCitation(std::move(apaCitation)),
      m_bibtexCitation(std::move(bibtexCitation))
    {
    Create(parent, id, pos, size, style);
    }

//------------------------------------------------------
bool AboutDialogEx::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                           const long style)
    {
    wxDialog::Create(parent, id,
                     wxString::Format(/* TRANSLATORS: %s is the application name */ _(L"About %s"),
                                      wxGetApp().GetAppName()),
                     pos, size, style);

    CreateControls();
    Centre();

    Bind(
        wxEVT_BUTTON,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            if (wxTheClipboard->Open())
                {
                if (!m_mlaCitation.empty())
                    {
                    // has italics in it
                    lily_of_the_valley::html_extract_text htmlExtract;
                    const wxString strippedText =
                        htmlExtract(m_mlaCitation.wc_str(), m_mlaCitation.length(), true, true);
                    wxTheClipboard->Clear();
                    auto* obj = new wxDataObjectComposite();
                    obj->Add(new wxTextDataObject(strippedText));
                    wxTheClipboard->SetData(obj);
                    }
                wxTheClipboard->Close();
                }
        },
        AboutDialogEx::ID_COPYMLA);

    Bind(
        wxEVT_BUTTON,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            if (wxTheClipboard->Open())
                {
                if (!m_apaCitation.empty())
                    {
                    wxTheClipboard->Clear();
                    auto* obj = new wxDataObjectComposite();
                    obj->Add(new wxTextDataObject(m_apaCitation));
                    wxTheClipboard->SetData(obj);
                    }
                wxTheClipboard->Close();
                }
        },
        AboutDialogEx::ID_COPYAPA);

    Bind(
        wxEVT_BUTTON,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            if (wxTheClipboard->Open())
                {
                if (!m_bibtexCitation.empty())
                    {
                    wxTheClipboard->Clear();
                    auto* obj = new wxDataObjectComposite();
                    obj->Add(new wxTextDataObject(m_bibtexCitation));
                    wxTheClipboard->SetData(obj);
                    }
                wxTheClipboard->Close();
                }
        },
        AboutDialogEx::ID_COPYBIBTEX);

    Bind(
        wxEVT_BUTTON,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            if (wxTheClipboard->Open())
                {
                if (!m_components.empty())
                    {
                    lily_of_the_valley::html_extract_text htmlExtract;
                    const wxString strippedText =
                        htmlExtract(m_components.wc_str(), m_components.length(), true, true);
                    wxTheClipboard->Clear();
                    auto* obj = new wxDataObjectComposite();
                    obj->Add(new wxTextDataObject(strippedText));
                    wxTheClipboard->SetData(obj);
                    }
                wxTheClipboard->Close();
                }
        },
        AboutDialogEx::ID_COPY_COMPONENTS);

    Bind(
        wxEVT_BUTTON,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            if (wxTheClipboard->Open())
                {
                if (!m_eula.empty())
                    {
                    wxTheClipboard->Clear();
                    auto* obj = new wxDataObjectComposite();
                    obj->Add(new wxTextDataObject(m_eula));
                    wxTheClipboard->SetData(obj);
                    }
                wxTheClipboard->Close();
                }
        },
        AboutDialogEx::ID_COPY_LICENSE);

    Bind(
        wxEVT_BUTTON,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            if (wxTheClipboard->Open())
                {
                if (!m_productInfo.empty())
                    {
                    wxTheClipboard->Clear();
                    auto* obj = new wxDataObjectComposite();
                    obj->Add(new wxTextDataObject(m_productInfo));
                    wxTheClipboard->SetData(obj);
                    }
                wxTheClipboard->Close();
                }
        },
        AboutDialogEx::ID_COPY_PRODUCT_INFO);

    return true;
    }

//------------------------------------------------------
void AboutDialogEx::CreateControls()
    {
    auto* mainSizer = new wxBoxSizer(wxVERTICAL);

    if (m_banner.IsOk())
        {
        auto* bannerBox = new wxStaticBitmap(this, wxID_ANY, m_banner, wxDefaultPosition,
                                             wxDefaultSize, wxSIMPLE_BORDER);
        mainSizer->Add(bannerBox, 0, wxALIGN_CENTER_HORIZONTAL | wxALL,
                       wxSizerFlags::GetDefaultBorder());
        }

    m_sideBarBook = new Wisteria::UI::SideBarBook(this, wxID_ANY);
    mainSizer->Add(m_sideBarBook, wxSizerFlags{ 1 }.Expand().Border());

        // version info page
        {
        auto* mainPage = new wxPanel(m_sideBarBook);
        auto* mainPanelSizer = new wxBoxSizer(wxVERTICAL);
        mainPage->SetSizer(mainPanelSizer);
        m_sideBarBook->AddPage(mainPage, _(L"Product Info"), ID_VERSION_PAGE, true);

        auto* productInfoGrid = new wxFlexGridSizer(2, wxSize(wxSizerFlags::GetDefaultBorder(), 0));
        productInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, _(L"Version:")));
        productInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, m_appVersion));
        constexpr std::wstring_view BUILD_VERSION{ _READSTUDIO_BUILD_VERSION };
        if constexpr (!BUILD_VERSION.empty())
            {
            productInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, _(L"Build:")));
            productInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, wxString{ BUILD_VERSION }));
            }
#ifndef NDEBUG
        productInfoGrid->Add(new wxStaticText(
            mainPage, wxID_ANY,
            // TRANSLATORS: Compiled version of the program (e.g., DEBUG or RELEASE)
            _(L"Build Type:")));
    #ifdef ENABLED_SANITIZERS_STR
        wxString sanitizerInfo = wxString::FromUTF8(ENABLED_SANITIZERS_STR) + L"\n" +
                                 wxString::FromUTF8(SANITIZER_ENV_HINTS_STR);
        sanitizerInfo.Replace(L"\\n", L"\n");
        sanitizerInfo.Trim();
        productInfoGrid->Add(
            new wxStaticText(mainPage, wxID_ANY,
                             _DT(L"DEBUG with:\n", DTExplanation::DebugMessage) + sanitizerInfo));
    #else
        productInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, _DT(L"DEBUG")));
    #endif
#endif
        productInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, _(L"Build Date:")));
        wxDateTime buildDate;
        buildDate.ParseDate(__DATE__);
        productInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, buildDate.Format(L"%B %d, %G")));
        productInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY,
                                              // TRANSLATORS: Operating System
                                              _(L"Platform:")));
        productInfoGrid->Add(
            new wxStaticText(mainPage, wxID_ANY,
#ifdef __WXGTK__
                             wxGetOsDescription() + L" (" +
                                 wxPlatformInfo::Get().GetLinuxDistributionInfo().Description + L")"
#else
                             wxGetOsDescription()
#endif
                             ));
        productInfoGrid->Add(new wxStaticText(
            mainPage, wxID_ANY, /* TRANSLATORS: Computer CPU information */ _(L"Architecture:")));
        productInfoGrid->Add(
            new wxStaticText(mainPage, wxID_ANY,
                             wxString::Format(/* TRANSLATORS: CPU type, and then threads count */
                                              _(L"%s, %d CPU threads"),
                                              wxSystemHardwareInfo::GetNativeCPUArchitectureName(),
                                              wxSystemHardwareInfo::GetCPUCount())));

        productInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, _(L"Memory:")));
        productInfoGrid->Add(new wxStaticText(
            mainPage, wxID_ANY,
            wxFileName::GetHumanReadableSize(wxSystemHardwareInfo::GetMemory().GetValue(),
                                             _("Not available"))));

        if (wxGetMouseState().ShiftDown())
            {
            productInfoGrid->Add(new wxStaticText(mainPage, wxID_ANY, _(L"Peak Memory Usage:")));
            productInfoGrid->Add(new wxStaticText(
                mainPage, wxID_ANY,
                wxFileName::GetHumanReadableSize(
                    wxSystemHardwareInfo::GetPeakUsedMemory().GetValue(), _("Not available"))));
            }

        // put it all together
        auto* appLabelSizer = new wxBoxSizer(wxVERTICAL);
        auto* appLabel = new wxStaticText(mainPage, wxID_ANY, wxGetApp().GetAppName());
        appLabel->SetFont(wxFontInfo{ appLabel->GetFont().GetFractionalPointSize() * 2 }.FaceName(
            Wisteria::GraphItems::Label::GetFirstAvailableFont({ DONTTRANSLATE(L"Roboto"),
                                                                 DONTTRANSLATE(L"Orbitron"),
                                                                 DONTTRANSLATE(L"Georgia") })));
        appLabel->SetForegroundColour(
            wxSystemSettings::SelectLightDark(wxColour{ L"#3D3C3B" }, wxColour{ L"#F89522" }));
        appLabelSizer->Add(appLabel);
        appLabel = new wxStaticText(mainPage, wxID_ANY, wxGetApp().GetAppVersion());
        appLabel->SetFont(wxFontInfo{ appLabel->GetFont().GetFractionalPointSize() * 1.5 }.FaceName(
            Wisteria::GraphItems::Label::GetFirstAvailableFont({ DONTTRANSLATE(L"Roboto"),
                                                                 DONTTRANSLATE(L"Orbitron"),
                                                                 DONTTRANSLATE(L"Georgia") })));
        appLabel->SetForegroundColour(
            wxSystemSettings::SelectLightDark(wxColour{ L"#F89522" }, wxColour{ 255, 255, 255 }));
        appLabelSizer->Add(appLabel);
        auto* appTitleSizer = new wxBoxSizer(wxHORIZONTAL);
        appTitleSizer->Add(
            new wxStaticBitmap(mainPage, wxID_ANY,
                               Wisteria::UI::DialogWithHelp::FixStaticBitmapImage(
                                   wxGetApp().GetMainFrame()->GetLogo(), wxSize{ 128, 128 })));
        appTitleSizer->Add(appLabelSizer, wxSizerFlags{}.CenterVertical());
        mainPanelSizer->Add(appTitleSizer, wxSizerFlags{}.Left().Border(wxLEFT));

        auto* productArea = new wxBoxSizer(wxHORIZONTAL);
        productArea->Add(productInfoGrid);
        productArea->AddStretchSpacer();
        productArea->Add(new wxBitmapButton(mainPage, ID_COPY_PRODUCT_INFO,
                                            wxArtProvider::GetBitmap(wxART_COPY, wxART_BUTTON)));
        mainPanelSizer->Add(productArea, wxSizerFlags{}.Expand().Border());
        mainPanelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder() * 5);
        mainPanelSizer->AddStretchSpacer();
        mainPanelSizer->Add(new wxStaticText(mainPage, wxID_ANY, m_copyright),
                            wxSizerFlags{}.Left().Border(wxLEFT));
        mainPanelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

        m_productInfo = wxGetApp().GetAppName() + L"\n";
        for (size_t i = 0; i < productInfoGrid->GetChildren().size(); ++i)
            {
            const auto* currentWindow = productInfoGrid->GetChildren()[i]->GetWindow();
            if (currentWindow != nullptr)
                {
                m_productInfo.append(currentWindow->GetLabel());
                m_productInfo.append((i % 2 == 0) ? L'\t' : L'\n');
                }
            }
        }

    // License page
    if (!m_eula.empty())
        {
        auto* eulaPage = new wxPanel(m_sideBarBook);
        auto* mainPanelSizer = new wxBoxSizer(wxVERTICAL);
        eulaPage->SetSizer(mainPanelSizer);
        m_sideBarBook->AddPage(eulaPage, _(L"License"), ID_LICENSING_PAGE, false);

        auto* textRowSizer = new wxBoxSizer(wxHORIZONTAL);
        auto* eulaWindow = new wxTextCtrl(
            eulaPage, wxID_ANY, wxString{}, wxDefaultPosition, wxDefaultSize,
            wxTE_MULTILINE | wxTE_RICH2 | wxTE_READONLY, wxGenericValidator(&m_eula));
        textRowSizer->Add(eulaWindow, wxSizerFlags{ 1 }.Expand());
        textRowSizer->Add(new wxBitmapButton(eulaPage, ID_COPY_LICENSE,
                                             wxArtProvider::GetBitmap(wxART_COPY, wxART_BUTTON)));
        mainPanelSizer->Add(textRowSizer, wxSizerFlags{ 1 }.Expand().Border());
        }

        // components
        {
        auto* componentsPage = new wxPanel(m_sideBarBook);
        auto* mainPanelSizer = new wxBoxSizer(wxVERTICAL);
        componentsPage->SetSizer(mainPanelSizer);
        m_sideBarBook->AddPage(componentsPage,
                               /* TRANSLATORS: Software bill of materials */ _(L"SBOM"),
                               ID_COMPONENTS, false);

        // Consistently format library info with the name and version number.
        const auto formatLibInfo = [](const auto& libInfo, wxString commitHash = wxString{},
                                      wxString origin = wxString{}) -> wxString
        {
            if (!libInfo.IsOk())
                {
                return {};
                }
            wxString copyright = libInfo.GetCopyright();
            copyright.Replace(L"\n", L"<br />");

            if (!commitHash.empty())
                {
                commitHash = wxString::Format(_(L"Commit hash: %s<br />"), commitHash);
                }
            if (!origin.empty())
                {
                origin = wxString::Format(_(L"Origin: %s<br />"), origin);
                }

            return wxString::Format(
                L"<span style='font-weight: bold;'>%s</span> - %s<br />%s%s%s<br />",
                libInfo.GetName(),
                (libInfo.AtLeast(0, 0, 1) ?
                     wxString::Format(_(L"version %s"), libInfo.GetNumericVersionString()) :
                     _(L"unversioned")),
                (libInfo.HasCopyright() ? copyright + L"<br />" : wxString{}), commitHash, origin);
        };

        // Get wxWidget's version, as well as its submodules, and sort them.
        std::vector<wxString> allLibInfo{
            formatLibInfo(wxGetLibraryVersionInfo(), WXWIDGETS_HASH, WXWIDGETS_ORIGIN),
            formatLibInfo(Wisteria::GetLibraryVersionInfo(), WISTERIA_DATAVIZ_HASH,
                          WISTERIA_DATAVIZ_ORIGIN),
            formatLibInfo(wxVersionInfo{ L"Lua", LUA_VERSION_MAJOR_N, LUA_VERSION_MINOR_N,
                                         LUA_VERSION_RELEASE_N, 0, wxString{}, LUA_COPYRIGHT },
                          LUA_HASH, LUA_ORIGIN),
            formatLibInfo(wxVersionInfo{ L"Oleander Stemming Library",
                                         stemming::OLEANDER_STEM_MAJOR_VERSION,
                                         stemming::OLEANDER_STEM_MINOR_VERSION,
                                         stemming::OLEANDER_STEM_PATCH_VERSION,
                                         stemming::OLEANDER_STEM_TWEAK_VERSION, wxString{},
                                         stemming::OLEANDER_STEM_COPYRIGHT },
                          OLEANDERSTEMMINGLIBRARY_HASH, OLEANDERSTEMMINGLIBRARY_ORIGIN),
            formatLibInfo(wxVersionInfo{ L"TinyExpr++", TINYEXPR_CPP_MAJOR_VERSION,
                                         TINYEXPR_CPP_MINOR_VERSION, TINYEXPR_CPP_PATCH_VERSION,
                                         TINYEXPR_CPP_TWEAK_VERSION, wxString{},
                                         TINYEXPR_CPP_COPYRIGHT },
                          TINYEXPR_PLUSPLUS_HASH, TINYEXPR_PLUSPLUS_ORIGIN),
            formatLibInfo(wxVersionInfo{ L"TinyXML2", TIXML2_MAJOR_VERSION, TIXML2_MINOR_VERSION,
                                         TIXML2_PATCH_VERSION },
                          TINYXML2_HASH, TINYXML2_ORIGIN),
            formatLibInfo(wxTIFFHandler::GetLibraryVersionInfo(), TIFF_HASH, TIFF_ORIGIN),
            formatLibInfo(wxJPEGHandler::GetLibraryVersionInfo(), JPEG_HASH, JPEG_ORIGIN),
            formatLibInfo(wxPNGHandler::GetLibraryVersionInfo(), PNG_HASH, PNG_ORIGIN),
            formatLibInfo(wxWEBPHandler::GetLibraryVersionInfo(), LIBWEBP_HASH, LIBWEBP_ORIGIN),
            formatLibInfo(wxRegEx::GetLibraryVersionInfo(), PCRE_HASH, PCRE_ORIGIN),
            formatLibInfo(wxXmlDocument::GetLibraryVersionInfo(), EXPAT_HASH, EXPAT_ORIGIN),
            formatLibInfo(wxStyledTextCtrl::GetLibraryVersionInfo(), SCINTILLA_HASH,
                          SCINTILLA_ORIGIN),
            formatLibInfo(wxGetZlibVersionInfo(), ZLIB_HASH, ZLIB_ORIGIN),
            /// @todo uncomment if WebView is ever included
            // formatLibInfo(wxWebView::GetBackendVersionInfo()),
            // submodules without version information
            formatLibInfo(wxVersionInfo{ L"NanoSVG", -1 }, NANOSVG_HASH, NANOSVG_ORIGIN),
            formatLibInfo(wxVersionInfo{ L"CRC++", CRCPP_MAJOR_VERSION, CRCPP_MINOR_VERSION,
                                         CRCPP_PATCH_VERSION, CRCPP_REVISION_VERSION, wxString{},
                                         CRCPP_COPYRIGHT },
                          CRCPP_HASH, CRCPP_ORIGIN),
            formatLibInfo(wxVersionInfo{ L"wxStartPage", -1 }, WXSTARTPAGE_HASH,
                          WXSTARTPAGE_ORIGIN),
            formatLibInfo(
                wxVersionInfo{ L"easyexif", -1, 0, 0, 0, wxString{}, easyexif::EASYEXIF_COPYRIGHT },
                EASYEXIF_HASH, EASYEXIF_ORIGIN),
            formatLibInfo(wxVersionInfo{ L"UTF8-CPP", -1 }, UTFCPP_HASH, UTFCPP_ORIGIN)
        };

        std::ranges::sort(allLibInfo,
                          [](const auto& lhv, const auto& rhv) { return wxStricmp(lhv, rhv) < 0; });

        // Format all the library strings into one.
        m_components = [&allLibInfo]()
        {
            wxString allStr;
            // most library strings will be ~30 characters long
            allStr.reserve(allLibInfo.size() * 50);
            for (const auto& lib : allLibInfo)
                {
                allStr.append(lib);
                }
            allStr.Trim();
            return allStr;
        }();

        mainPanelSizer->Add(
            new wxStaticText(
                componentsPage, wxID_ANY,
                /* TRANSLATORS: program libraries */ _(L"Software bill of materials:")),
            wxSizerFlags{}.Border(wxLEFT));
        auto* textRowSizer = new wxBoxSizer(wxHORIZONTAL);
        auto* textWindow =
            new wxHtmlWindow(componentsPage, wxID_ANY, wxDefaultPosition, wxSize{ -1, FromDIP(75) },
                             wxHW_SCROLLBAR_AUTO | wxBORDER_THEME | wxHW_NO_SELECTION);
        textWindow->SetPage(m_components);
        textRowSizer->Add(textWindow, wxSizerFlags{ 1 }.Expand());
        textRowSizer->Add(new wxBitmapButton(componentsPage, ID_COPY_COMPONENTS,
                                             wxArtProvider::GetBitmap(wxART_COPY, wxART_BUTTON)));
        mainPanelSizer->Add(textRowSizer, wxSizerFlags{ 1 }.Expand().Border());
        }

    if (!m_mlaCitation.empty() && !m_apaCitation.empty() && !m_bibtexCitation.empty())
        {
        auto* citationPage = new wxPanel(m_sideBarBook);
        auto* mainPanelSizer = new wxBoxSizer(wxVERTICAL);
        citationPage->SetSizer(mainPanelSizer);
        m_sideBarBook->AddPage(citationPage, _(L"Citation"), ID_CITATION, false);

        mainPanelSizer->Add(new wxStaticText(citationPage, wxID_ANY,
                                             wxString::Format(_(L"To cite %s in publications:"),
                                                              wxGetApp().GetAppName())),
                            wxSizerFlags{}.Border(wxLEFT));

        mainPanelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder() * 2);

            {
            mainPanelSizer->Add(new wxStaticText(citationPage, wxID_ANY, _DT(L"MLA:")),
                                wxSizerFlags{}.Border(wxLEFT));
            auto* textRowSizer = new wxBoxSizer(wxHORIZONTAL);
            auto* textWindow = new wxHtmlWindow(
                citationPage, wxID_ANY, wxDefaultPosition, wxSize{ -1, FromDIP(75) },
                wxHW_SCROLLBAR_AUTO | wxBORDER_THEME | wxHW_NO_SELECTION);
            textWindow->SetPage(m_mlaCitation);
            textRowSizer->Add(textWindow, wxSizerFlags{ 1 }.Expand());
            textRowSizer->Add(new wxBitmapButton(
                citationPage, ID_COPYMLA, wxArtProvider::GetBitmap(wxART_COPY, wxART_BUTTON)));
            mainPanelSizer->Add(textRowSizer, wxSizerFlags{}.Expand().Border());
            }

            {
            mainPanelSizer->Add(new wxStaticText(citationPage, wxID_ANY, _DT(L"APA:")),
                                wxSizerFlags{}.Border(wxLEFT));
            auto* textRowSizer = new wxBoxSizer(wxHORIZONTAL);
            auto* textWindow = new wxHtmlWindow(
                citationPage, wxID_ANY, wxDefaultPosition, wxSize{ -1, FromDIP(75) },
                wxHW_SCROLLBAR_AUTO | wxBORDER_THEME | wxHW_NO_SELECTION);
            textWindow->SetPage(m_apaCitation);
            textRowSizer->Add(textWindow, wxSizerFlags{ 1 }.Expand());
            textRowSizer->Add(new wxBitmapButton(
                citationPage, ID_COPYAPA, wxArtProvider::GetBitmap(wxART_COPY, wxART_BUTTON)));
            mainPanelSizer->Add(textRowSizer, wxSizerFlags{}.Expand().Border());
            }

            {
            mainPanelSizer->Add(new wxStaticText(citationPage, wxID_ANY, _DT(L"BibTeX:")),
                                wxSizerFlags{}.Border(wxLEFT));
            auto* textRowSizer = new wxBoxSizer(wxHORIZONTAL);
            auto* textWindow = new wxTextCtrl(
                citationPage, wxID_ANY, wxString{}, wxDefaultPosition, wxSize{ -1, FromDIP(175) },
                wxTE_MULTILINE | wxTE_RICH2 | wxTE_READONLY, wxGenericValidator(&m_bibtexCitation));
            textWindow->SetFont(wxFontInfo{}.Family(wxFontFamily::wxFONTFAMILY_TELETYPE));
            // Force the text color to reset to the system text color.
            // Setting the font changes the font color to black, and simply setting it back to
            // wxSYS_COLOUR_WINDOWTEXT will have no effect because the control still thinks the font
            // color is that, although it really isn't. So set it to the dialog color and then
            // to the text color for it to take effect.
            textWindow->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
            textWindow->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
            textRowSizer->Add(textWindow, wxSizerFlags{ 1 }.Expand());
            textRowSizer->Add(new wxBitmapButton(
                citationPage, ID_COPYBIBTEX, wxArtProvider::GetBitmap(wxART_COPY, wxART_BUTTON)));
            mainPanelSizer->Add(textRowSizer, wxSizerFlags{ 1 }.Expand().Border());
            }
        }

    mainSizer->Add(CreateSeparatedButtonSizer(wxCLOSE), wxSizerFlags{}.Expand().Border());

    SetSizerAndFit(mainSizer);
    }
