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

#ifndef WEB_HARVESTER_DIALOG_H
#define WEB_HARVESTER_DIALOG_H

#include "../../Wisteria-Dataviz/src/ui/controls/codeeditor.h"
#include "../../Wisteria-Dataviz/src/ui/controls/listctrlex.h"
#include "../../Wisteria-Dataviz/src/ui/controls/sidebarbook.h"
#include "../../Wisteria-Dataviz/src/ui/dialogs/dialogwithhelp.h"
#include "../../webharvester/webharvester.h"
#include <wx/arrstr.h>
#include <wx/bmpbuttn.h>
#include <wx/collpane.h>
#include <wx/combobox.h>
#include <wx/dialog.h>
#include <wx/spinctrl.h>
#include <wx/wx.h>

/// @private
/// @internal Dialog for prompting a user for a block of HTML code to extract links from.
class LinkDialog final : public wxDialog
    {
  public:
    LinkDialog(wxWindow* parent, const wxString& message, const wxString& caption);
    LinkDialog(LinkDialog&) = delete;
    LinkDialog& operator=(const LinkDialog&) = delete;

    [[nodiscard]]
    wxString GetValue() const
        {
        return (m_codeWindow != nullptr) ? m_codeWindow->GetValue() : wxString{};
        }

    void OnOK([[maybe_unused]] wxCommandEvent& event);

  protected:
    Wisteria::UI::CodeEditor* m_codeWindow{ nullptr };
    };

/// @brief Dialog for harvester webpages.
class WebHarvesterDlg final : public Wisteria::UI::DialogWithHelp
    {
  public:
    /// @brief Constructor.
    /// @note Call UpdateFromHarvesterSettings() to fill in the remaining settings
    ///     from an existing WebHarvester object.
    WebHarvesterDlg(wxWindow* parent, wxArrayString urls, const wxString& fullDocFilter,
                    const wxString& selectedDocFilter, const bool hideLocalDownloadOption,
                    wxWindowID id = wxID_ANY, const wxString& caption = _(L"Web Harvester"),
                    const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                    long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
        : m_fullDocFilter(fullDocFilter), m_selectedDocFilter(selectedDocFilter),
          m_hideLocalDownloadOption(hideLocalDownloadOption), m_urls(std::move(urls))
        {
        Create(parent, id, caption, pos, size, style);
        }

    /// @private
    WebHarvesterDlg(WebHarvesterDlg&) = delete;
    /// @private
    WebHarvesterDlg& operator=(const WebHarvesterDlg&) = delete;

    [[nodiscard]]
    const wxArrayString& GetUrls() const noexcept
        {
        return m_urls;
        }

    [[nodiscard]]
    const wxString& GetSelectedDocFilter() const noexcept
        {
        return m_selectedDocFilter;
        }

    [[nodiscard]]
    int GetDepthLevel() const noexcept
        {
        return m_depthLevel;
        }

    [[nodiscard]]
    wxString GetUserAgent() const
        {
        return m_userAgent;
        }

    /// @returns Returns @c true if peer verification has been disabled.
    [[nodiscard]]
    bool IsPeerVerifyDisabled() const noexcept
        {
        return m_disablePeerVerify;
        }

    /// @returns Whether cookies should be extracted from JS code and sent back to the
    ///     server when connected to them.
    [[nodiscard]]
    bool IsUsingJavaScriptCookies() const noexcept
        {
        return m_useJsCookies;
        }

    /** @brief If using JavaScript cookies, store and send all encountered cookies for
            all sites being crawled during a given session.
        @param persistCookies @c true to reuse cookies.*/
    void PersistJavaScriptCookies(const bool persistCookies)
        {
        m_persistJsCookies = persistCookies;
        }

    /// @returns If using JavaScript cookies, store and send all encountered cookies for
    ///     all sites being crawled during a given session.
    [[nodiscard]]
    bool IsPersistingJavaScriptCookies() const noexcept
        {
        return m_persistJsCookies;
        }

    [[nodiscard]]
    int GetDomainRestriction() const noexcept
        {
        return m_selectedDomainRestriction;
        }

    [[nodiscard]]
    const wxArrayString& GetRestrictedDomains() const noexcept
        {
        return m_domains;
        }

    void DownloadFilesLocally(const bool download) noexcept
        {
        m_downloadFilesLocally = download;
        TransferDataToWindow();

        UpdateDownloadOptions();
        }

    [[nodiscard]]
    bool IsDownloadFilesLocally() const noexcept
        {
        return m_downloadFilesLocally;
        }

    [[nodiscard]]
    bool IsRetainingWebsiteFolderStructure() const noexcept
        {
        return m_keepWebPathWhenDownloading;
        }

    [[nodiscard]]
    bool IsReplacingExistingFiles() const noexcept
        {
        return m_replaceExistingFiles;
        }

    [[nodiscard]]
    const wxString& GetDownloadFolder() const noexcept
        {
        return m_downloadFolder;
        }

    void SetDownloadFolder(const wxString& dir) noexcept
        {
        if (wxFileName::DirExists(dir))
            {
            m_downloadFolder = dir;
            }
        TransferDataToWindow();
        }

    void SetMinimumDownloadFileSizeInKilobytes(const int minKbs) noexcept
        {
        m_minFileSizeInKiloBytes = minKbs;
        TransferDataToWindow();
        }

    [[nodiscard]]
    int GetMinimumDownloadFileSizeInKilobytes() const noexcept
        {
        return m_minFileSizeInKiloBytes;
        }

    /// Updates the dialog from a harvester's settings.
    void UpdateFromHarvesterSettings(const WebHarvester& harvester);
    /// Sets a harvest object to use the settings from this dialog.
    void UpdateHarvesterSettings(WebHarvester& harvester);

  private:
    constexpr static int ID_DOWNLOAD_CHECKBOX = wxID_HIGHEST;
    constexpr static int ID_DOWNLOAD_FOLDER_BROWSE_BUTTON = wxID_HIGHEST + 1;
    constexpr static int ID_DOMAIN_COMBO = wxID_HIGHEST + 2;
    constexpr static int ID_ADD_URL_BUTTON = wxID_HIGHEST + 3;
    constexpr static int ID_DELETE_URL_BUTTON = wxID_HIGHEST + 4;
    constexpr static int ID_ADD_DOMAIN_BUTTON = wxID_HIGHEST + 5;
    constexpr static int ID_DELETE_DOMAIN_BUTTON = wxID_HIGHEST + 6;
    constexpr static int ID_HARVESTING_PAGE = wxID_HIGHEST + 7;
    constexpr static int ID_DOMAINS_PAGE = wxID_HIGHEST + 8;
    constexpr static int ID_DOWNLOAD_PAGE = wxID_HIGHEST + 9;
    constexpr static int ID_LOAD_URLS_BUTTON = wxID_HIGHEST + 10;
    constexpr static int ID_JS_COOKIES_CHECKBOX = wxID_HIGHEST + 11;

    void OnOK([[maybe_unused]] wxCommandEvent& event);
    void OnDownloadCheck([[maybe_unused]] wxCommandEvent& event);
    void OnFolderBrowseButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnDomainComboSelect([[maybe_unused]] wxCommandEvent& event);
    void OnAddUrlClick([[maybe_unused]] wxCommandEvent& event);
    void OnDeleteUrlClick([[maybe_unused]] wxCommandEvent& event);
    void OnAddDomainClick([[maybe_unused]] wxCommandEvent& event);
    void OnLoadUrlsClick([[maybe_unused]] wxCommandEvent& event);
    void OnDeleteDomainClick([[maybe_unused]] wxCommandEvent& event);

    void UpdateDownloadOptions()
        {
        m_localFolderLabel->Enable(m_downloadFilesLocally);
        m_localFolderEdit->Enable(m_downloadFilesLocally);
        m_retainWebsiteFolderStructureCheckBox->Enable(m_downloadFilesLocally);
        m_replaceExistingFilesCheckBox->Enable(m_downloadFilesLocally);
        m_minFileSizeLabel->Enable(m_downloadFilesLocally);
        m_minFileSizeCtrl->Enable(m_downloadFilesLocally);
        m_folderBrowseButton->Enable(m_downloadFilesLocally);
        }

    [[nodiscard]]
    wxString GetUserSpecifiedDomainsLabel() const
        {
        return _(L"Restricted to user-defined domains");
        }

    /// Creation
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& caption = wxString{},
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_DIALOG_STYLE);
    /// Creates the controls and sizers
    void CreateControls();
    int m_depthLevel{ 1 };
    wxString m_userAgent;
    bool m_disablePeerVerify{ false };
    bool m_useJsCookies{ false };
    bool m_persistJsCookies{ false };
    wxString m_fullDocFilter;
    wxString m_selectedDocFilter;
    int m_selectedDomainRestriction{ 0 };
    wxArrayString m_domains;
    bool m_hideLocalDownloadOption{ false };
    bool m_downloadFilesLocally{ true };
    bool m_keepWebPathWhenDownloading{ true };
    bool m_replaceExistingFiles{ true };
    bool m_logBrokenLinks{ false };
    int m_minFileSizeInKiloBytes{ 5 };
    Wisteria::UI::SideBarBook* m_sideBarBook{ nullptr };
    wxComboBox* m_docFilterCombo{ nullptr };
    wxSpinCtrl* m_depthLevelCtrl{ nullptr };
    wxComboBox* m_domainCombo{ nullptr };
    Wisteria::UI::ListCtrlEx* m_domainList{ nullptr };
    std::shared_ptr<Wisteria::UI::ListCtrlExDataProvider> m_domainData{
        std::make_shared<Wisteria::UI::ListCtrlExDataProvider>()
    };
    wxTextCtrl* m_localFolderEdit{ nullptr };
    wxStaticText* m_localFolderLabel{ nullptr };
    wxCheckBox* m_retainWebsiteFolderStructureCheckBox{ nullptr };
    wxCheckBox* m_replaceExistingFilesCheckBox{ nullptr };
    Wisteria::UI::ListCtrlEx* m_urlList{ nullptr };
    wxBitmapButton* m_addDomainButton{ nullptr };
    wxBitmapButton* m_deleteDomainButton{ nullptr };
    wxStaticText* m_minFileSizeLabel{ nullptr };
    wxSpinCtrl* m_minFileSizeCtrl{ nullptr };
    wxBitmapButton* m_folderBrowseButton{ nullptr };
    wxCheckBox* m_persistCookiesCheck{ nullptr };

    wxArrayString m_urls;
    std::shared_ptr<Wisteria::UI::ListCtrlExDataProvider> m_urlData{
        std::make_shared<Wisteria::UI::ListCtrlExDataProvider>()
    };

    wxString m_downloadFolder;
    };

#endif // WEB_HARVESTER_DIALOG_H
