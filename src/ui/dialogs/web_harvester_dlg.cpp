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

#include "web_harvester_dlg.h"
#include "../../app/readability_app.h"

wxDECLARE_APP(ReadabilityApp);

//-------------------------------------------------------------
LinkDialog::LinkDialog(wxWindow* parent, const wxString& message, const wxString& caption)
    {
    wxNonOwnedWindow::SetExtraStyle(GetExtraStyle() | wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create(parent, wxID_ANY, caption, wxDefaultPosition, wxDefaultSize,
                     wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

    auto* mainSizer = new wxBoxSizer(wxVERTICAL);

    mainSizer->Add(new wxStaticText(this, wxID_STATIC, message), wxSizerFlags{}.Border());

    m_codeWindow = new Wisteria::UI::CodeEditor(this, wxSTC_LEX_HTML, wxID_ANY, wxDefaultPosition,
                                                FromDIP(wxSize{ 1000, 900 }));
    m_codeWindow->SetWrapMode(wxSTC_WRAP_WHITESPACE);
    m_codeWindow->SetModified(false);
    const wxFont font{ wxFontInfo().Family(wxFONTFAMILY_SWISS) };
    for (auto i = 0; i < wxSTC_STYLE_LASTPREDEFINED; ++i)
        {
        m_codeWindow->StyleSetFont(i, font);
        }
    mainSizer->Add(m_codeWindow, wxSizerFlags{ 1 }.Expand().Border());

    mainSizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL), wxSizerFlags{}.Expand().Border());

    m_codeWindow->SetFocus();

    SetSizerAndFit(mainSizer);
    Center();

    Bind(wxEVT_BUTTON, &LinkDialog::OnOK, this, wxID_OK);
    }

//-------------------------------------------------------------
void LinkDialog::OnOK([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    if (IsModal())
        {
        EndModal(wxID_OK);
        }
    else
        {
        SetReturnCode(wxID_OK);
        Show(false);
        }
    }

//-------------------------------------------------------------
void WebHarvesterDlg::OnDomainComboSelect([[maybe_unused]] wxCommandEvent& event)
    {
    m_domainList->Enable(m_domainCombo->GetValue() == GetUserSpecifiedDomainsLabel());
    m_addDomainButton->Enable(m_domainCombo->GetValue() == GetUserSpecifiedDomainsLabel());
    m_deleteDomainButton->Enable(m_domainCombo->GetValue() == GetUserSpecifiedDomainsLabel());
    }

//-------------------------------------------------------------
void WebHarvesterDlg::OnDownloadCheck([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();

    UpdateDownloadOptions();
    }

//-------------------------------------------------------------
void WebHarvesterDlg::OnFolderBrowseButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    wxDirDialog dirDlg(this, _(L"Choose a directory"), m_downloadFolder);
    if (dirDlg.ShowModal() != wxID_OK)
        {
        return;
        }
    m_downloadFolder = dirDlg.GetPath();
    TransferDataToWindow();
    m_localFolderEdit->SetFocus();
    }

//-------------------------------------------------------------
void WebHarvesterDlg::OnOK([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();

    m_urls.Clear();
    for (size_t i = 0; i < m_urlData->GetItemCount(); ++i)
        {
        if (!m_urlData->GetItemText(i, 0).empty())
            {
            m_urls.Add(m_urlData->GetItemText(i, 0));
            }
        }

    if (GetUrls().GetCount() == 0)
        {
        wxMessageBox(_(L"Please enter a website to harvest."), _(L"Error"),
                     wxOK | wxICON_EXCLAMATION);
        return;
        }
    if (m_downloadFilesLocally && m_downloadFolder.empty())
        {
        wxMessageBox(_(L"Please enter a valid folder to download the files to."), _(L"Error"),
                     wxOK | wxICON_EXCLAMATION);
        m_sideBarBook->SetSelection(2);
        wxCommandEvent cmd;
        OnFolderBrowseButtonClick(cmd);
        return;
        }
    if (m_downloadFilesLocally && !wxFileName::DirExists(m_downloadFolder))
        {
        if (!wxFileName::Mkdir(m_downloadFolder, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            m_sideBarBook->SetSelection(2);
            wxMessageBox(_(L"Please enter a valid folder to download the files to."), _(L"Error"),
                         wxOK | wxICON_EXCLAMATION);
            wxCommandEvent cmd;
            OnFolderBrowseButtonClick(cmd);
            return;
            }
        }

    m_domains.clear();
    for (long i = 0; i < m_domainList->GetItemCount(); ++i)
        {
        if (!m_domainList->GetItemTextEx(i, 0).empty())
            {
            m_domains.Add(m_domainList->GetItemTextEx(i, 0));
            }
        }
    if (static_cast<WebHarvester::DomainRestriction>(GetDomainRestriction()) ==
            WebHarvester::DomainRestriction::RestrictToSpecificDomains &&
        m_domains.GetCount() == 0)
        {
        wxMessageBox(_(L"Please enter the domains that you want to be restricted to."), _(L"Error"),
                     wxOK | wxICON_ERROR);
        return;
        }

    // validators seem to be broken with combo boxes, so do this the old-fashioned way
    m_selectedDocFilter = m_docFilterCombo->GetValue();

    if (IsModal())
        {
        EndModal(wxID_OK);
        }
    else
        {
        Show(false);
        }
    }

//-------------------------------------------------------------
void WebHarvesterDlg::OnAddUrlClick([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_urlList != nullptr)
        {
        m_urlList->AddRow();
        m_urlList->EditItem(m_urlList->GetItemCount() - 1, 0);
        }
    }

//-------------------------------------------------------------
void WebHarvesterDlg::OnDeleteUrlClick([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_urlList != nullptr)
        {
        m_urlList->DeleteSelectedItems();
        }
    }

//-------------------------------------------------------------
void WebHarvesterDlg::OnLoadUrlsClick([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_urlList != nullptr)
        {
        LinkDialog linksDlg(this,
                            _(L"Enter a block of HTML content to extract website links from."),
                            _(L"Load URLs"));
        if (linksDlg.ShowModal() == wxID_OK)
            {
            // case-sensitive is fine since Linux servers use case-sensitive page links
            std::set<wxString> gatheredLinks;
            const wxString content{ linksDlg.GetValue() };
            html_utilities::hyperlink_parse getHyperLinks(
                content.wc_str(), content.length(),
                html_utilities::hyperlink_parse::hyperlink_parse_method::html);
            while (true)
                {
                // gather its hyperlinks
                const wchar_t* currentLink = getHyperLinks();
                if (currentLink != nullptr &&
                    html_utilities::html_url_format::is_absolute_url(currentLink))
                    {
                    gatheredLinks.insert(
                        wxString{ currentLink, getHyperLinks.get_current_hyperlink_length() });
                    }
                else if (currentLink == nullptr)
                    {
                    break;
                    }
                }
            const wxWindowUpdateLocker noUpdates(m_urlList);
            for (const auto& link : gatheredLinks)
                {
                m_urlList->AddRow(link);
                }
            }
        }
    }

//-------------------------------------------------------------
void WebHarvesterDlg::OnAddDomainClick([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_domainList != nullptr)
        {
        m_domainList->AddRow();
        m_domainList->EditItem(m_domainList->GetItemCount() - 1, 0);
        }
    }

//-------------------------------------------------------------
void WebHarvesterDlg::OnDeleteDomainClick([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_domainList != nullptr)
        {
        m_domainList->DeleteSelectedItems();
        }
    }

//-------------------------------------------------------------
void WebHarvesterDlg::CreateControls()
    {
    auto* mainSizer = new wxBoxSizer(wxVERTICAL);

    m_sideBarBook = new Wisteria::UI::SideBarBook(this, wxID_ANY);
    wxGetApp().UpdateSideBarTheme(m_sideBarBook->GetSideBar());

    mainSizer->Add(m_sideBarBook, wxSizerFlags{ 1 }.Expand().Border());

        // website page
        {
        auto* panel = new wxPanel(m_sideBarBook, ID_HARVESTING_PAGE, wxDefaultPosition,
                                  wxDefaultSize, wxTAB_TRAVERSAL);
        auto* panelSizer = new wxBoxSizer(wxVERTICAL);
        panel->SetSizer(panelSizer);
        m_sideBarBook->AddPage(panel,
                               // TRANSLATORS: Gathering pages from a website.
                               _(L"Harvesting"), ID_HARVESTING_PAGE, true);

        // the URLs
        auto* urlSizer = new wxStaticBoxSizer(
            new wxStaticBox(panel, wxID_ANY, _(L"Websites to Harvest")), wxVERTICAL);
        auto* urlButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
        auto* addUrlButton = new wxBitmapButton(
            urlSizer->GetStaticBox(), ID_ADD_URL_BUTTON,
            wxArtProvider::GetBitmap(L"ID_ADD", wxART_BUTTON, FromDIP(wxSize{ 16, 16 })));
        addUrlButton->SetToolTip(_(L"Add a website to the list"));
        urlButtonsSizer->Add(addUrlButton);

        auto* deleteUrlButton = new wxBitmapButton(
            urlSizer->GetStaticBox(), ID_DELETE_URL_BUTTON,
            wxArtProvider::GetBitmap(wxART_DELETE, wxART_BUTTON, FromDIP(wxSize{ 16, 16 })));
        deleteUrlButton->SetToolTip(_(L"Delete selected websites"));
        urlButtonsSizer->Add(deleteUrlButton);

        auto* loadUrlsButton = new wxBitmapButton(
            urlSizer->GetStaticBox(), ID_LOAD_URLS_BUTTON,
            wxArtProvider::GetBitmap(L"ID_LINK", wxART_BUTTON, FromDIP(wxSize{ 16, 16 })));
        loadUrlsButton->SetToolTip(_(L"Load links from HTML content"));
        urlButtonsSizer->Add(loadUrlsButton);
        urlSizer->Add(urlButtonsSizer, wxSizerFlags{}.Right());

        m_urlData->SetValues(m_urls);
        m_urlList = new Wisteria::UI::ListCtrlEx(
            urlSizer->GetStaticBox(), wxID_ANY, wxDefaultPosition, FromDIP(wxSize{ 600, 200 }),
            wxLC_VIRTUAL | wxLC_EDIT_LABELS | wxLC_REPORT | wxLC_ALIGN_LEFT);
        urlSizer->Add(m_urlList, wxSizerFlags{ 1 }.Expand());
        m_urlList->EnableGridLines();
        m_urlList->EnableItemAdd();
        m_urlList->EnableLabelEditing();
        m_urlList->EnableItemDeletion();
        m_urlList->InsertColumn(0, _(L"Websites:"));
        m_urlList->SetColumnEditable(0);
        m_urlList->SetVirtualDataProvider(m_urlData);
        m_urlList->SetVirtualDataSize(m_urlData->GetItemCount(), 1);

        panelSizer->Add(urlSizer, wxSizerFlags{ 1 }.Expand().Border());

        // depth level
        auto* depthLevelSizer = new wxBoxSizer(wxHORIZONTAL);
        depthLevelSizer->Add(new wxStaticText(panel, wxID_ANY, _(L"Depth level:")),
                             wxSizerFlags{}.CenterVertical());
        depthLevelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

        m_depthLevelCtrl = new wxSpinCtrl(panel);
        m_depthLevelCtrl->SetRange(0, 10);
        m_depthLevelCtrl->SetValidator(wxGenericValidator(&m_depthLevel));
        depthLevelSizer->Add(m_depthLevelCtrl, wxSizerFlags{}.CenterVertical());
        panelSizer->Add(depthLevelSizer, wxSizerFlags{}.Border(wxLEFT | wxBOTTOM));

        // file filters
        auto* fileTypeSizer = new wxBoxSizer(wxHORIZONTAL);
        panelSizer->Add(fileTypeSizer, wxSizerFlags{}.Expand());
        fileTypeSizer->Add(new wxStaticText(panel, wxID_STATIC, _(L"File types to include:")),
                           wxSizerFlags{}.Border(wxLEFT).CenterVertical());
        wxArrayString choiceStrings;
        wxStringTokenizer tkz(m_fullDocFilter, L"|", wxTOKEN_STRTOK);
        while (tkz.HasMoreTokens())
            {
            wxString currentFilter = tkz.GetNextToken();
            if (!currentFilter.empty() && currentFilter[0] != L'*')
                {
                choiceStrings.Add(currentFilter);
                }
            }
        if (m_selectedDocFilter.empty() && (choiceStrings.GetCount() != 0U))
            {
            m_selectedDocFilter = choiceStrings[0];
            }
        m_docFilterCombo = new wxComboBox(
            panel, wxID_ANY, wxString{}, wxDefaultPosition,
            // need to hardcode a size here because the file filter
            // string for all documents may be huge
            wxSize(FromDIP(wxSize(100, 100)).GetWidth(), -1), choiceStrings,
            wxGetMouseState().ShiftDown() ? wxCB_DROPDOWN : wxCB_DROPDOWN | wxCB_READONLY);
        m_docFilterCombo->SetValue(m_selectedDocFilter);
        fileTypeSizer->Add(m_docFilterCombo,
                           wxSizerFlags{ 1 }.Expand().Border(wxLEFT | wxRIGHT | wxBOTTOM));

        // user agent
        auto* userAgentSizer = new wxBoxSizer(wxHORIZONTAL);
        panelSizer->Add(userAgentSizer, wxSizerFlags{}.Expand().Border(wxLEFT));

        userAgentSizer->Add(new wxStaticText(panel, wxID_STATIC, _(L"User agent:")),
                            wxSizerFlags{}.CenterVertical());
        auto* userAgentEdit =
            new wxTextCtrl(panel, wxID_ANY, wxString{}, wxDefaultPosition, wxDefaultSize,
                           wxBORDER_THEME, wxGenericValidator(&m_userAgent));
        userAgentSizer->Add(userAgentEdit, wxSizerFlags{ 1 }.Expand().Border(wxLEFT | wxRIGHT));

        auto* extendedOpsSizer = new wxBoxSizer(wxVERTICAL);
        panelSizer->Add(extendedOpsSizer, wxSizerFlags{}.Expand().Border(wxLEFT));
        // disable SSL
        extendedOpsSizer->Add(new wxCheckBox(panel, wxID_ANY,
                                             _(L"Disable SSL certificate verification"),
                                             wxDefaultPosition, wxDefaultSize, 0,
                                             wxGenericValidator(&m_disablePeerVerify)),
                              wxSizerFlags{}.Expand().Border(wxTOP));

        // JS cookies
        extendedOpsSizer->Add(new wxCheckBox(panel, ID_JS_COOKIES_CHECKBOX,
                                             _(L"Use JavaScript cookies"), wxDefaultPosition,
                                             wxDefaultSize, 0, wxGenericValidator(&m_useJsCookies)),
                              wxSizerFlags{}.Expand().Border(wxTOP));

        m_persistCookiesCheck = new wxCheckBox(
            panel, wxID_ANY, _(L"Persist cookies for current sites"), wxDefaultPosition,
            wxDefaultSize, 0, wxGenericValidator(&m_persistJsCookies));
        extendedOpsSizer->Add(m_persistCookiesCheck, wxSizerFlags{}.Expand().Border(wxTOP));
        m_persistCookiesCheck->Enable(m_useJsCookies);

        // check links
        extendedOpsSizer->Add(new wxCheckBox(panel, wxID_ANY, _(L"&Log broken links"),
                                             wxDefaultPosition, wxDefaultSize, 0,
                                             wxGenericValidator(&m_logBrokenLinks)),
                              wxSizerFlags{}.Expand().Border(wxTOP | wxBOTTOM));
        }

        // domain restriction page
        {
        auto* panel = new wxPanel(m_sideBarBook, ID_DOMAINS_PAGE, wxDefaultPosition, wxDefaultSize,
                                  wxTAB_TRAVERSAL);
        auto* panelSizer = new wxBoxSizer(wxVERTICAL);
        panel->SetSizer(panelSizer);
        m_sideBarBook->AddPage(panel, _(L"Domain Restriction"), ID_DOMAINS_PAGE, false);

        auto* domainBoxSizer = new wxStaticBoxSizer(
            new wxStaticBox(panel, wxID_ANY, _(L"Domain Restriction")), wxVERTICAL);
        panelSizer->Add(domainBoxSizer, wxSizerFlags{ 1 }.Expand().Border());

        wxArrayString choiceStrings;
        choiceStrings.Add(_(L"Not restricted to any domain"));
        choiceStrings.Add(_(L"Domain restricted"));
        choiceStrings.Add(_(L"Subdomain restricted"));
        choiceStrings.Add(GetUserSpecifiedDomainsLabel());
        choiceStrings.Add(_(L"Restricted to external domains"));
        choiceStrings.Add(_(L"Restricted to same folder"));
        m_domainCombo = new wxComboBox(domainBoxSizer->GetStaticBox(), ID_DOMAIN_COMBO, wxString{},
                                       wxDefaultPosition, wxDefaultSize, choiceStrings,
                                       wxCB_DROPDOWN | wxCB_READONLY);
        m_domainCombo->SetValidator(wxGenericValidator(&m_selectedDomainRestriction));
        domainBoxSizer->Add(m_domainCombo, wxSizerFlags{}.Expand().Border());

        auto* domainButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
        m_addDomainButton = new wxBitmapButton(
            domainBoxSizer->GetStaticBox(), ID_ADD_DOMAIN_BUTTON,
            wxArtProvider::GetBitmap(L"ID_ADD", wxART_BUTTON, FromDIP(wxSize{ 16, 16 })));
        m_addDomainButton->SetToolTip(_(L"Add a domain to the list"));
        m_addDomainButton->Enable(m_domainCombo->GetValue() == GetUserSpecifiedDomainsLabel());
        domainButtonsSizer->Add(m_addDomainButton);

        m_deleteDomainButton = new wxBitmapButton(
            domainBoxSizer->GetStaticBox(), ID_DELETE_DOMAIN_BUTTON,
            wxArtProvider::GetBitmap(wxART_DELETE, wxART_BUTTON, FromDIP(wxSize{ 16, 16 })));
        m_deleteDomainButton->SetToolTip(_(L"Delete selected domain"));
        m_deleteDomainButton->Enable(m_domainCombo->GetValue() == GetUserSpecifiedDomainsLabel());
        domainButtonsSizer->Add(m_deleteDomainButton);
        domainBoxSizer->Add(domainButtonsSizer, wxSizerFlags{}.Right().Border(wxRIGHT | wxTOP));

        m_domainData->SetValues(m_domains);
        m_domainList = new Wisteria::UI::ListCtrlEx(domainBoxSizer->GetStaticBox(), wxID_ANY,
                                                    wxDefaultPosition, FromDIP(wxSize{ 600, 200 }),
                                                    wxLC_VIRTUAL | wxLC_EDIT_LABELS | wxLC_REPORT |
                                                        wxLC_ALIGN_LEFT);
        domainBoxSizer->Add(m_domainList,
                            wxSizerFlags{ 1 }.Expand().Border(wxLEFT | wxRIGHT | wxBOTTOM));
        m_domainList->EnableGridLines();
        m_domainList->EnableItemAdd();
        m_domainList->EnableLabelEditing();
        m_domainList->EnableItemDeletion();
        // TRANSLATORS: Website domains.
        m_domainList->InsertColumn(0, _(L"User-defined Domains:"));
        m_domainList->SetColumnEditable(0);
        m_domainList->SetVirtualDataProvider(m_domainData);
        m_domainList->SetVirtualDataSize(m_domainData->GetItemCount(), 1);
        m_domainList->Enable(m_domainCombo->GetValue() == GetUserSpecifiedDomainsLabel());
        }

        // local downloading
        {
        auto* panel = new wxPanel(m_sideBarBook, ID_DOWNLOAD_PAGE, wxDefaultPosition, wxDefaultSize,
                                  wxTAB_TRAVERSAL);
        auto* panelSizer = new wxBoxSizer(wxVERTICAL);
        panel->SetSizer(panelSizer);
        m_sideBarBook->AddPage(panel, _(L"Download"), ID_DOWNLOAD_PAGE, false);
        panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

        if (!m_hideLocalDownloadOption)
            {
            auto* downloadCheckBox = new wxCheckBox(
                panel, ID_DOWNLOAD_CHECKBOX, _(L"&Download files locally"), wxDefaultPosition,
                wxDefaultSize, 0, wxGenericValidator(&m_downloadFilesLocally));
            panelSizer->Add(downloadCheckBox, wxSizerFlags{}.Border());
            }

        m_localFolderLabel = new wxStaticText(panel, wxID_ANY, _(L"Folder to download files to:"),
                                              wxDefaultPosition, wxDefaultSize, 0);
        m_localFolderLabel->Enable(m_downloadFilesLocally);
        panelSizer->Add(m_localFolderLabel, wxSizerFlags{}.Border(wxLEFT | wxBOTTOM | wxRIGHT));

        auto* downloadFolderPathSizer = new wxBoxSizer(wxHORIZONTAL);
        m_localFolderEdit =
            new wxTextCtrl(panel, wxID_ANY, wxString{}, wxDefaultPosition, wxDefaultSize,
                           wxBORDER_THEME, wxGenericValidator(&m_downloadFolder));
        m_localFolderEdit->AutoCompleteFileNames();
        m_localFolderEdit->Enable(m_downloadFilesLocally);
        downloadFolderPathSizer->Add(m_localFolderEdit, wxSizerFlags{ 1 }.Expand());

        m_folderBrowseButton = new wxBitmapButton(
            panel, ID_DOWNLOAD_FOLDER_BROWSE_BUTTON,
            wxArtProvider::GetBitmap(wxART_FILE_OPEN, wxART_BUTTON, FromDIP(wxSize{ 16, 16 })));
        m_folderBrowseButton->Enable(m_downloadFilesLocally);
        downloadFolderPathSizer->Add(m_folderBrowseButton, wxSizerFlags{}.Expand().Border(wxRIGHT));
        panelSizer->Add(downloadFolderPathSizer, wxSizerFlags{}.Border(wxLEFT).Expand());
        panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

        // min file size
        auto* minFileSizeSizer = new wxBoxSizer(wxHORIZONTAL);
        panelSizer->Add(minFileSizeSizer, wxSizerFlags{}.Border(wxLEFT));

        m_minFileSizeLabel =
            new wxStaticText(panel, wxID_STATIC, _(L"Minimum file size to download (in Kbs.):"));
        m_minFileSizeLabel->Enable(m_downloadFilesLocally);
        minFileSizeSizer->Add(m_minFileSizeLabel, wxSizerFlags{}.CenterVertical().Border(wxRIGHT));

        m_minFileSizeCtrl =
            new wxSpinCtrl(panel, wxID_ANY, std::to_wstring(m_minFileSizeInKiloBytes));
        m_minFileSizeCtrl->SetRange(1, 1024 * 20);
        m_minFileSizeCtrl->SetValidator(wxGenericValidator(&m_minFileSizeInKiloBytes));
        m_minFileSizeCtrl->Enable(m_downloadFilesLocally);
        minFileSizeSizer->Add(m_minFileSizeCtrl);
        panelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

        // folder structure
        m_retainWebsiteFolderStructureCheckBox = new wxCheckBox(
            panel, wxID_ANY, _(L"&Use website's folder structure"), wxDefaultPosition,
            wxDefaultSize, 0, wxGenericValidator(&m_keepWebPathWhenDownloading));
        m_retainWebsiteFolderStructureCheckBox->Enable(m_downloadFilesLocally);
        panelSizer->Add(m_retainWebsiteFolderStructureCheckBox, wxSizerFlags{}.Border());

        // existing files
        m_replaceExistingFilesCheckBox =
            new wxCheckBox(panel, wxID_ANY, _(L"Replace existing files"), wxDefaultPosition,
                           wxDefaultSize, 0, wxGenericValidator(&m_replaceExistingFiles));
        m_replaceExistingFilesCheckBox->Enable(m_downloadFilesLocally);
        panelSizer->Add(m_replaceExistingFilesCheckBox, wxSizerFlags{}.Border(wxLEFT));
        }

    mainSizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL | wxHELP),
                   wxSizerFlags{}.Expand().Border());

    SetSizerAndFit(mainSizer);
    }

//-------------------------------------------------------------
void WebHarvesterDlg::UpdateHarvesterSettings(WebHarvester& harvester)
    {
    TransferDataFromWindow();

    // will be turned back on if looking for HTML files; otherwise, limit file extension
    // criteria from what was explicitly selected
    harvester.HarvestAllHtmlFiles(false);
    harvester.ClearAllowableFileTypes();

    wxStringTokenizer tkz(ExtractExtensionsFromFileFilter(GetSelectedDocFilter()), L"*.;");
    while (tkz.HasMoreTokens())
        {
        const wxString nextFileExt = tkz.GetNextToken();
        if (!nextFileExt.empty())
            {
            harvester.AddAllowableFileType(nextFileExt);
            if (nextFileExt.CmpNoCase(L"html") == 0 || nextFileExt.CmpNoCase(L"htm") == 0)
                {
                harvester.HarvestAllHtmlFiles(true);
                }
            }
        }
    harvester.SetDepthLevel(GetDepthLevel());
    harvester.SetDomainRestriction(
        static_cast<WebHarvester::DomainRestriction>(GetDomainRestriction()));
    harvester.ClearAllowableWebFolders();
    if (static_cast<WebHarvester::DomainRestriction>(GetDomainRestriction()) ==
        WebHarvester::DomainRestriction::RestrictToSpecificDomains)
        {
        for (const auto& dom : GetRestrictedDomains())
            {
            harvester.AddAllowableDomain(dom);
            }
        }
    harvester.SetUserAgent(GetUserAgent());
    harvester.DisablePeerVerify(IsPeerVerifyDisabled());
    harvester.UseJavaScriptCookies(IsUsingJavaScriptCookies());
    harvester.PersistJavaScriptCookies(IsPersistingJavaScriptCookies());
    harvester.DownloadFilesWhileCrawling(IsDownloadFilesLocally());
    harvester.SetDownloadDirectory(GetDownloadFolder());
    harvester.KeepWebPathWhenDownloading(IsRetainingWebsiteFolderStructure());
    harvester.ReplaceExistingFiles(IsReplacingExistingFiles());
    harvester.SearchForBrokenLinks(m_logBrokenLinks);
    harvester.SetMinimumDownloadFileSizeInKilobytes(GetMinimumDownloadFileSizeInKilobytes());
    }

//-------------------------------------------------------------
void WebHarvesterDlg::UpdateFromHarvesterSettings(const WebHarvester& harvester)
    {
    TransferDataFromWindow();

    if (static_cast<WebHarvester::DomainRestriction>(harvester.GetDomainRestriction()) ==
        WebHarvester::DomainRestriction::RestrictToSpecificDomains)
        {
        m_domains = harvester.GetAllowableWebFolders();
        m_domainData->SetValues(m_domains);
        m_domainList->SetVirtualDataSize(m_domainData->GetItemCount(), 1);
        }
    m_depthLevel = harvester.GetDepthLevel();
    m_userAgent = harvester.GetUserAgent();
    m_disablePeerVerify = harvester.IsPeerVerifyDisabled();
    m_useJsCookies = harvester.IsUsingJavaScriptCookies();
    m_persistJsCookies = harvester.IsPersistingJavaScriptCookies();
    m_logBrokenLinks = harvester.IsSearchingForBrokenLinks();
    m_selectedDomainRestriction = static_cast<int>(harvester.GetDomainRestriction());
    m_downloadFilesLocally = harvester.IsDownloadingFilesWhileCrawling();
    m_downloadFolder = harvester.GetDownloadDirectory();
    m_keepWebPathWhenDownloading = harvester.IsKeepingWebPathWhenDownloading();
    m_replaceExistingFiles = harvester.IsReplacingExistingFiles();
    m_minFileSizeInKiloBytes = harvester.GetMinimumDownloadFileSizeInKilobytes().value_or(5);

    m_persistCookiesCheck->Enable(m_useJsCookies);
    UpdateDownloadOptions();

    TransferDataToWindow();
    }

//-------------------------------------------------------------
bool WebHarvesterDlg::Create(wxWindow* parent, wxWindowID id /*= wxID_ANY*/,
                             const wxString& caption /*= wxString{}*/,
                             const wxPoint& pos /*= wxDefaultPosition*/,
                             const wxSize& size /*= wxDefaultSize*/,
                             long style /*= wxDEFAULT_DIALOG_STYLE*/)
    {
    SetExtraStyle(GetExtraStyle() | wxWS_EX_BLOCK_EVENTS | wxWS_EX_CONTEXTHELP);
    DialogWithHelp::Create(parent, id, caption, pos, size, style);

    CreateControls();

    m_domainList->SetColumnWidth(0, m_domainList->GetClientSize().GetWidth() -
                                        wxSizerFlags::GetDefaultBorder());
    Centre();

    TransferDataToWindow();

    Bind(wxEVT_BUTTON, &WebHarvesterDlg::OnFolderBrowseButtonClick, this,
         WebHarvesterDlg::ID_DOWNLOAD_FOLDER_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &WebHarvesterDlg::OnOK, this, wxID_OK);
    Bind(wxEVT_BUTTON, &WebHarvesterDlg::OnAddUrlClick, this, WebHarvesterDlg::ID_ADD_URL_BUTTON);
    Bind(wxEVT_BUTTON, &WebHarvesterDlg::OnDeleteUrlClick, this,
         WebHarvesterDlg::ID_DELETE_URL_BUTTON);
    Bind(wxEVT_BUTTON, &WebHarvesterDlg::OnAddDomainClick, this,
         WebHarvesterDlg::ID_ADD_DOMAIN_BUTTON);
    Bind(wxEVT_BUTTON, &WebHarvesterDlg::OnLoadUrlsClick, this,
         WebHarvesterDlg::ID_LOAD_URLS_BUTTON);
    Bind(wxEVT_BUTTON, &WebHarvesterDlg::OnDeleteDomainClick, this,
         WebHarvesterDlg::ID_DELETE_DOMAIN_BUTTON);

    Bind(wxEVT_CHECKBOX, &WebHarvesterDlg::OnDownloadCheck, this,
         WebHarvesterDlg::ID_DOWNLOAD_CHECKBOX);
    Bind(
        wxEVT_CHECKBOX,
        [this]([[maybe_unused]]
               wxCommandEvent& event)
        {
            TransferDataFromWindow();
            m_persistCookiesCheck->Enable(m_useJsCookies);
        },
        WebHarvesterDlg::ID_JS_COOKIES_CHECKBOX);
    Bind(wxEVT_COMBOBOX, &WebHarvesterDlg::OnDomainComboSelect, this,
         WebHarvesterDlg::ID_DOMAIN_COMBO);

    Bind(
        wxEVT_CHAR_HOOK,
        [this](wxKeyEvent& event)
        {
            if (event.ControlDown() && event.GetKeyCode() == L'K')
                {
                wxRibbonButtonBarEvent dummyEvt;
                OnLoadUrlsClick(dummyEvt);
                }
            else
                {
                event.Skip(true);
                }
        },
        wxID_ANY);

    return true;
    }
