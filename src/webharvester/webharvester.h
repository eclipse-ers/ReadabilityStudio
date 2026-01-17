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

#ifndef WEBHARVESTER_H
#define WEBHARVESTER_H

#include "../Wisteria-Dataviz/src/import/html_extract_text.h"
#include "../Wisteria-Dataviz/src/util/char_traits.h"
#include "../Wisteria-Dataviz/src/util/donttranslate.h"
#include "../Wisteria-Dataviz/src/util/downloadfile.h"
#include "../Wisteria-Dataviz/src/util/fileutil.h"
#include <algorithm>
#include <optional>
#include <set>
#include <string_view>
#include <wx/filename.h>
#include <wx/progdlg.h>
#include <wx/utils.h>
#include <wx/wx.h>

/// @private
class wxStringLessWebPath
    {
  public:
    [[nodiscard]]
    bool operator()(const wxString& first, const wxString& second) const;
    };

/// @brief List of known web file extensions.
class WebPageExtension
    {
  public:
    /** @returns @c true if @c extension is a known web file extension.
        @param extension The file extension to review.*/
    [[nodiscard]]
    bool operator()(const wxString& extension) const
        {
        return (m_knownWebPageExtensions.contains(extension.wc_str()) ||
                m_knownWebPageExtensions.contains(GetExtensionOrDomain(extension).wc_str()));
        }

    /** @returns @c true if @c extension is a dynamic webpage extension.
        @param extension The file extension to review.*/
    [[nodiscard]]
    bool IsDynamicExtension(const wxString& extension) const
        {
        return (m_knownDynamicExtensions.contains(extension.wc_str()) ||
                m_knownDynamicExtensions.contains(GetExtensionOrDomain(extension).wc_str()));
        }

  private:
    std::set<string_util::case_insensitive_wstring> m_knownWebPageExtensions{
        _DT(L"asp"),  _DT(L"aspx"), _DT(L"ca"),    _DT(L"cfm"), _DT(L"cfml"), _DT(L"biz"),
        _DT(L"com"),  _DT(L"net"),  _DT(L"org"),   _DT(L"php"), _DT(L"php3"), _DT(L"php4"),
        _DT(L"html"), _DT(L"htm"),  _DT(L"xhtml"), _DT(L"sgml")
    };
    std::set<string_util::case_insensitive_wstring> m_knownDynamicExtensions{
        _DT(L"asp"), _DT(L"aspx"), _DT(L"php"), _DT(L"php3"), _DT(L"php4")
    };
    };

/// @brief List of known document or media file extensions that can be downloaded from a website.
/// @details These would be documents such as PDF or images, rather than HTML pages.
class NonWebPageFileExtension
    {
  public:
    /** @returns @c true if @c extension is a known file extension.
        @param extension The file extension to review.*/
    [[nodiscard]]
    bool operator()(std::wstring_view extension) const
        {
        return (m_nonWebPageFileExtensions.contains(extension.data()));
        }

  private:
    std::set<string_util::case_insensitive_wstring> m_nonWebPageFileExtensions{
        // images
        _DT(L"ico"), _DT(L"jpg"), _DT(L"jpeg"), _DT(L"bmp"), _DT(L"gif"), _DT(L"png"), _DT(L"psd"),
        _DT(L"tif"), _DT(L"tiff"), _DT(L"wmf"), _DT(L"tga"), _DT(L"svg"),
        // style sheets
        _DT(L"css"),
        // documents
        _DT(L"doc"), _DT(L"docx"), _DT(L"ppt"), _DT(L"pptx"), _DT(L"xls"), _DT(L"xlsx"),
        _DT(L"csv"), _DT(L"rtf"), _DT(L"pdf"), _DT(L"ps"), _DT(L"txt"),
        // movies
        _DT(L"mov"), _DT(L"qt"), _DT(L"rv"), _DT(L"rm"), _DT(L"wmv"), _DT(L"mpg"), _DT(L"mpeg"),
        _DT(L"mpe"), _DT(L"avi"),
        // music
        _DT(L"mp3"), _DT(L"wav"), _DT(L"wma"), _DT(L"midi"), _DT(L"ra"), _DT(L"ram"),
        // programs
        _DT(L"exe"), _DT(L"jar"), _DT(L"swf"),
        // compressed files
        _DT(L"zip"), _DT(L"gzip"), _DT(L"tar"), _DT(L"bz2")
    };
    };

/// @brief List of known web script (e.g., JavaScript) extensions.
class ScriptFileExtension
    {
  public:
    /** @returns @c true if @c extension is a web script file extension.
        @param extension The file extension to review.*/
    [[nodiscard]]
    bool operator()(std::wstring_view extension) const
        {
        return (m_scriptFileExtensions.contains(extension.data()));
        }

  private:
    std::set<string_util::case_insensitive_wstring> m_scriptFileExtensions{ _DT(L"js"),
                                                                            _DT(L"vbs") };
    };

/// @brief Interface for harvesting and (optionally) downloading web content from a base URL.
/// @details This is recommended to be a singleton object that connects to the application
///     or main frame. Be sure to call SetEventHandler() to connect the download events
///     for your parent event handler to the harvester. Also, call CancelPending() in the
///     parent event handler's close event.
class WebHarvester
    {
  public:
    /// @brief Domain restriction methods.
    enum class DomainRestriction
        {
        NotRestricted,             /*!< No restrictions.*/
        RestrictToDomain,          /*!< Restrict harvesting to the base URL's domain.*/
        RestrictToSubDomain,       /*!< Restrict harvesting to the base URL's subdomain.*/
        RestrictToSpecificDomains, /*!< Restrict harvesting to a list of user-defined domains.*/
        RestrictToExternalLinks,   /*!< Restrict harvesting to links outside the base URL's
                                        domain.*/
        RestrictToFolder           /*!< Restrict harvesting to links within the base URL's folder.*/
        };

    /// @private
    // NOLINTNEXTLINE(hicpp-use-equals-default,modernize-use-equals-default)
    WebHarvester()
        {
        m_downloader.SetUserAgent(GetUserAgent());
        m_downloader.DisablePeerVerify(IsPeerVerifyDisabled());
        m_downloader.UseSuggestedFileNames(true);
        }

    /// @private
    WebHarvester(const WebHarvester&) = delete;
    /// @private
    WebHarvester& operator=(const WebHarvester&) = delete;

    /// @brief Crawls the loaded URL.
    /// @returns @c false if crawl was cancelled.
    [[nodiscard]]
    bool CrawlLinks();

    /// @brief Downloads a file from the Internet.
    /// @param url The link to download.
    /// @param fileExtension The (hint) file extension to download the file as. This is only
    ///     used if the webpage doesn't have a proper extension. If empty and
    ///     @c Url is empty, then the file extension will be determined by the MIME type.
    /// @returns The local file path of the file after downloading, or empty string upon failure.
    wxString DownloadFile(wxString& url, const wxString& fileExtension = wxString{});
    /// @brief Download all the harvested links.
    /// @note This should be called after CrawlLinks().
    void DownloadFiles();

    /// @brief Cancels any pending download, read, or harvesting operation.
    void CancelPending() noexcept
        {
        m_isCancelled = true;
        m_downloader.CancelPending();
        }

    /** @brief Reads the content of a webpage into a buffer.
        @param[in,out] url The webpage (might be altered if redirected).
        @param[out] webPageContent The content of the page.
        @param[out] contentType The MIME type (and possibly charset) of the page.
        @param[out] statusText Any possible information from the server
            (usually extended error information).
        @param[out] responseCode The response code when connecting to the page.
        @param acceptOnlyHtmlOrScriptFiles Whether only HTML or
            JS script files should be read.
        @returns Whether the file was successfully read.*/
    [[nodiscard]]
    bool ReadWebPage(wxString& url, wxString& webPageContent, wxString& contentType,
                     wxString& statusText, int& responseCode,
                     const bool acceptOnlyHtmlOrScriptFiles = true);

    /** @brief Reads the content of a web document
            (not a page, but something like a OneDrive document).
        @param[in,out] url The web line (might be altered if redirected).
        @param[out] statusText Any possible information from the server
            (usually extended error information).
        @param[out] responseCode The response code when connecting to the page.
        @returns Whether the file was successfully read.
        @note Call GetDownloader().GetLastRead() and GetDownloader().GetLastOneDriveFileName()
            to retrieve the document that was read.
        @warning Currently only supports OneDrive (publicly accessible) documents.*/
    [[nodiscard]]
    bool ReadWebDocument(wxString& url, wxString& statusText, int& responseCode);

    /// @returns @c true if @c url is a OneDrive link.
    /// @param url The link to reviews.
    [[nodiscard]]
    static bool IsOneDriveDocument(const wxString& url)
        {
        html_utilities::html_url_format formatUrl(url.wc_str());
        formatUrl(url.wc_str(), false);
        return (formatUrl.get_domain() == L"1drv.ms" ||
                formatUrl.get_root_subdomain() == L"onedrive.live.com");
        }

    /// @brief Attempts to connect to @c url.
    /// @param url The webpage to try to connect to.
    void RequestResponse(const wxString& url) { m_downloader.RequestResponse(url); }

    /// @returns The internal FileDownload object.
    [[nodiscard]]
    const FileDownload& GetDownloader() const noexcept
        {
        return m_downloader;
        }

    /** @brief Gets the content type of a webpage.
        @param[in,out] url The webpage (might be altered if redirected).
        @param[out] responseCode The response code when connecting to the page.
        @returns The MIME type (and possibly charset) of the page's content type.*/
    wxString GetContentType(wxString& url, int& responseCode);
    /// @returns The file type (possibly an extension) from a MIME type string.
    /// @param contentType The MIME type string.
    [[nodiscard]]
    static wxString GetFileTypeFromContentType(const wxString& contentType);

    /** @returns @c true if a link is pointing to an HTML page.
        @param[in,out] url The link to test (might be redirected).
        @param[out] contentType The MIME type read from the page.
        @param[out] responseCode The response code from the page.*/
    [[nodiscard]]
    bool IsPageHtml(wxString& url, wxString& contentType, int& responseCode);

    /// @brief Sets the depth level to crawl from the base URL.
    /// @param levels The number of levels to crawl.
    void SetDepthLevel(const size_t levels) noexcept { m_levelDepth = levels; }

    /// @returns The depth level to crawl from the base URL.
    [[nodiscard]]
    size_t GetDepthLevel() const noexcept
        {
        return m_levelDepth;
        }

    /// @brief Adds a file type to harvest and download (based on extension).
    /// @param fileExtension The file extension to download.
    /// @note You can pass in the extension,
    ///     or a full filepath, and it will get the extension from that.
    void AddAllowableFileType(const wxString& fileExtension)
        {
        if (fileExtension.find(L'.') == std::wstring::npos)
            {
            m_fileExtensions.insert(fileExtension);
            }
        else
            {
            m_fileExtensions.insert(wxFileName(fileExtension).GetExt());
            }
        }

    /// @brief Removes any explicit file extensions that are restricting
    ///     to while harvesting and downloading.
    void ClearAllowableFileTypes() { m_fileExtensions.clear(); }

    /// @brief When downloading locally, keep the folder structure from the website.
    /// @param keep @c true to use the website's folder structure,
    ///     @c false to download files in a flat folder structure.
    /// @note This is recommended to prevent overwriting files with the same name.
    void KeepWebPathWhenDownloading(const bool keep = true) noexcept
        {
        m_keepWebPathWhenDownloading = keep;
        }

    /// @returns Whether the website's folder structure is being
    ///     mirrored when downloading files.
    [[nodiscard]]
    bool IsKeepingWebPathWhenDownloading() const noexcept
        {
        return m_keepWebPathWhenDownloading;
        }

    /// @brief Specifies whether all HTML content should be downloaded,
    ///     regardless of the file's extension (or lack of one).
    /// @details This is the default behavior.
    /// @param harvestAll @c true to download all HTML content.
    /// @note This is recommended if you need to download pages
    ///     that don't have extensions (i.e., dynamic pages).
    void HarvestAllHtmlFiles(const bool harvestAll = true) noexcept
        {
        m_harvestAllHtml = harvestAll;
        }

    /// @brief Sets whether to build a list of broken links while crawling.
    /// @param search @c true to catalogue broken links.
    /// @warning Enabling this will degrade performance because it will
    ///     attempt to connect to each link.
    void SearchForBrokenLinks(const bool search = true) noexcept
        {
        m_searchForBrokenLinks = search;
        }

    /// @returns @c true if a list of broken links are being catalogued while harvesting.
    [[nodiscard]]
    bool IsSearchingForBrokenLinks() const noexcept
        {
        return m_searchForBrokenLinks;
        }

    /// @brief Sets the base URL to crawl.
    /// @param url The base URL.
    void SetUrl(const wxString& url)
        {
        m_url = url;
        html_utilities::html_url_format formatUrl(m_url.wc_str());
        m_domain = formatUrl.get_root_domain().c_str();
        m_fullDomain = formatUrl.get_root_full_domain().c_str();
        m_fullDomainFolderPath = formatUrl.get_directory_path().c_str();
        }

    /// @returns The base URL being crawled.
    [[nodiscard]]
    const wxString& GetUrl() const noexcept
        {
        return m_url;
        }

    /// @brief Sets the local folder to download files from the web.
    /// @param downloadDirectory The local download folder.
    void SetDownloadDirectory(const wxString& downloadDirectory)
        {
        m_downloadDirectory = downloadDirectory;
        }

    /// @returns The local folder where web content is being downloaded.
    [[nodiscard]]
    const wxString& GetDownloadDirectory() const noexcept
        {
        return m_downloadDirectory;
        }

    /// @returns @c true if files being downloaded will replace existing ones.
    /// @note If this returns @c false, then downloaded files will be renamed
    ///     if necessary to avoid overwriting existing files.
    [[nodiscard]]
    bool IsReplacingExistingFiles() const noexcept
        {
        return m_replaceExistingFiles;
        }

    /// @brief Specifies whether files being downloaded can overwrite
    ///     each other if they have the same path.
    /// @param replaceExistingFiles @c true to overwrite existing files.
    /// @note If this is set to @c false and a file with the same path
    ///     is about to be downloaded, the program will attempt to download
    ///     it with a different (but similar) name.
    void ReplaceExistingFiles(const bool replaceExistingFiles = true) noexcept
        {
        m_replaceExistingFiles = replaceExistingFiles;
        }

    /// @brief Sets whether to download files locally while crawling.
    /// @param downloadWhileCrawling @c true to download the web content.
    void DownloadFilesWhileCrawling(const bool downloadWhileCrawling = true) noexcept
        {
        m_downloadWhileCrawling = downloadWhileCrawling;
        }

    /// @returns Whether files are being downloaded locally while crawling.
    [[nodiscard]]
    bool IsDownloadingFilesWhileCrawling() const noexcept
        {
        return m_downloadWhileCrawling;
        }

    // Domain restriction
    //----------------------------------

    /** @brief Sets the domain restriction method.
        @param restriction The restriction method to use.*/
    void SetDomainRestriction(const DomainRestriction restriction) noexcept
        {
        m_domainRestriction = restriction;
        }

    /// @returns The domain-restriction method.
    [[nodiscard]]
    DomainRestriction GetDomainRestriction() const noexcept
        {
        return m_domainRestriction;
        }

    /// @brief Overrides the domain of the main webpage. Useful for only getting files
    ///     from an outside domain (or specific folder).
    /// @param domain The domain to restrict to.
    void SetRestrictedDomain(const wxString& domain)
        {
        const html_utilities::html_url_format formatUrl(domain.wc_str());
        m_domain = formatUrl.get_root_domain().c_str();
        m_domainRestriction = DomainRestriction::RestrictToDomain;
        }

    /// @brief Resets the list of user-defined webpath restrictions.
    void ClearAllowableWebFolders() noexcept { m_allowableWebFolders.clear(); }

    /** @brief Adds a user-defined web path (domain/folder structure)
            to restrict harvesting to.
        @param domain A webpath to restrict harvesting to.*/
    void AddAllowableDomain(wxString domain)
        {
        if (domain.empty())
            {
            return;
            }
        // if a full webpage, then it should have an extension on it and
        // html_url_format will remove the webpage. But if there is no
        // extension (or its junk), then add a trailing '/' to prevent
        // the last folder from being removed.
        const wxString webExt = wxFileName(domain).GetExt();
        if ((webExt.empty() || webExt.length() > 4) && !domain.ends_with(L"/"))
            {
            domain.append(L"/");
            }
        html_utilities::html_url_format formatUrl(domain.wc_str());
        if (!formatUrl.get_directory_path().empty())
            {
            m_allowableWebFolders.emplace(formatUrl.get_directory_path().c_str());
            }
        }

    /// @returns The user-defined web paths (domains, folder structure)
    ///     that harvesting is constrained to.
    [[nodiscard]]
    wxArrayString GetAllowableWebFolders() const
        {
        wxArrayString domains;
        for (const auto& domain : m_allowableWebFolders)
            {
            domains.Add(domain.c_str());
            }
        return domains;
        }

    /// @brief Connect the downloader to a parent dialog or @c wxApp.
    /// @param handler The @c wxEvtHandler to connect the downloader to.
    void SetEventHandler(wxEvtHandler* handler) { m_downloader.SetAndBindEventHandler(handler); }

    /// @returns The list of harvested links.
    [[nodiscard]]
    const std::set<wxString, wxStringLessWebPath>& GetHarvestedLinks() const noexcept
        {
        return m_harvestedLinks;
        }

    /// @returns The list of files downloaded.
    /// @note DownloadFilesWhileCrawling() must be enabled.
    [[nodiscard]]
    const std::set<wxString>& GetDownloadedFilePaths() const noexcept
        {
        return m_downloadedFiles;
        }

    /// @returns A map of broken links and the respective pages they were found on.
    /// @note SearchForBrokenLinks() must be enabled.
    [[nodiscard]]
    const std::map<wxString, wxString>& GetBrokenLinks() const noexcept
        {
        return m_brokenLinks;
        }

    /// @returns The user agent sent to websites when crawling.
    /// @note If the user agent is empty, then one will be built from the OS description.
    [[nodiscard]]
    wxString GetUserAgent() const
        {
        /* May need to be set if not initialized.
           Needs to be initialized here because wxGetOsDescription()
           can't be called during global startup.
           Note that we call this a "WebLion."
           Using words like "harvester," "crawler," "scraper," and
           even "browser" will result in a forbidden response from some sites,
           so avoid using those words. */
        return (m_userAgent.empty() ?
                    _DT(L"Mozilla/5.0 (") + wxGetOsDescription() + _DT(L") WebKit/12.0 WebLion") :
                    m_userAgent);
        }

    /// @brief Sets the user agent sent to websites when crawling.
    /// @param agent The user agent string.
    void SetUserAgent(wxString agent)
        {
        m_userAgent = std::move(agent);
        m_downloader.SetUserAgent(GetUserAgent());
        }

    /** @brief Disable SSL certificate verification.
        @details This can be used to connect to self-signed servers or other
            invalid SSL connections.\n
            Disabling verification makes the communication insecure.
        @param disable @c true to disable SSL certificate verification.*/
    void DisablePeerVerify(const bool disable) noexcept
        {
        m_disablePeerVerify = disable;
        m_downloader.DisablePeerVerify(m_disablePeerVerify);
        }

    /// @returns Returns @c true if peer verification has been disabled.
    [[nodiscard]]
    bool IsPeerVerifyDisabled() const noexcept
        {
        return m_disablePeerVerify;
        }

    /** @brief If @c true, will scan for any cookies meant to be sent via JavaScript when
            reading or downloading a page. If any cookies are found,
            then page will be re-read with the cookies being sent.
        @details This is useful when connecting to pages that won't load as expected
            unless certain cookies are sent back to the server.
        @warning This will result in an additional call to read each webpage and is
            only recommended if JavaScript is being used to block headless connections.
        @param useCookies @c true to reconnect with cookies in the JavaScript.
        @sa PersistJavaScriptCookies().*/
    void UseJavaScriptCookies(const bool useCookies) { m_useJsCookies = useCookies; }

    /// @returns Whether cookies should be extracted from JS code and sent back to the
    ///     server when connecting to them.
    [[nodiscard]]
    bool IsUsingJavaScriptCookies() const noexcept
        {
        return m_useJsCookies;
        }

    /** @brief If using JavaScript cookies, store and send all encountered cookies for
            all sites being crawled during a given session.
        @details Client will be responsible for clearing persisting cookies via ClearCookies().
        @note UseJavaScriptCookies() must be set to @c true; otherwise, this will be ignored.
        @param persistCookies @c true to reuse cookies. If @c false, cookies will not be reused
            and ClearCookies() will be called.
        @sa UseJavaScriptCookies(), ClearCookies().*/
    void PersistJavaScriptCookies(const bool persistCookies)
        {
        m_persistJsCookies = persistCookies;
        if (!m_persistJsCookies)
            {
            ClearCookies();
            }
        }

    /// @returns If using JavaScript cookies, store and send all encountered cookies for
    ///     all sites being crawled during a given session.
    [[nodiscard]]
    bool IsPersistingJavaScriptCookies() const noexcept
        {
        return m_persistJsCookies;
        }

    /// @brief Clears any JavaScript cookies if they are being reused.
    /// @sa UseJavaScriptCookies(), PersistJavaScriptCookies().
    void ClearCookies() { m_JsCookies.clear(); }

    /// @brief Sets the minimum size that a file has to be to download it.
    /// @param size The minimum file size, in kilobytes.
    void SetMinimumDownloadFileSizeInKilobytes(const std::optional<uint32_t> size)
        {
        m_minFileDownloadSizeKilobytes = size;
        m_downloader.SetMinimumDownloadFileSizeInKilobytes(size);
        }

    /// @returns The minimum size a file must be to download. Will be `std::nullopt`
    ///     if size constraints are not being enforced.
    [[nodiscard]]
    std::optional<uint32_t> GetMinimumDownloadFileSizeInKilobytes() const
        {
        return m_minFileDownloadSizeKilobytes;
        }

    /// @returns The character set, parsed from HTML's content type.
    /// @param contentType The content type section from a block of HTML to parse.
    [[nodiscard]]
    static wxString GetCharsetFromContentType(const wxString& contentType);

    /// @returns The character set, parsed from HTML's content.
    /// @param pageContent The full content of the HTML page.
    [[nodiscard]]
    static wxString GetCharsetFromPageContent(std::string_view pageContent);

    /// @brief Shows the file names while crawling.
    /// @param show @c true to show the names; @c false to just show dots.
    /// @details By default, the names will be shown on the progress bar, which may cause the
    ///     dialog to constantly resize itself. Setting this to @c false will show a
    ///     simpler "Downloading..." label that may be less jarring.
    void ShowFileNames(const bool show) { m_hideFileNamesWhileDownloading = !show; }

  private:
    /// @returns An URL with spaces encoded to '%20', '\/' converted to '/', and trimmed.
    /// @param url The URL to normalize.
    [[nodiscard]]
    static wxString NormalizeUrl(const wxString& url)
        {
        wxString adjUrl{ url };
        adjUrl.Trim(true).Trim(false);
        // encode any spaces
        adjUrl.Replace(L" ", L"%20");
        adjUrl.Replace(L"\\/", L"/");

        return adjUrl;
        }

    [[nodiscard]]
    bool VerifyUrlDomainCriteria(const wxString& url) const;
    /** @brief If @c url meets all the criteria, adds it to the list of links
            that we are gathering for the client.\n
            If downloading while crawling, will also download the file.
        @param url The URL to review.
        @param fileExtension File extension information about the URL.
            This will be used if filtering file types to harvest (and download).
        @returns @c true if the provided URL will be included in the harvested results
            (and downloaded, if applicable).
        @warning File extension criteria must be handled by the caller because
            if the harvester is including all HTML pages then it will need to determine
            if an URL is HTML before passing it to this function.*/
    bool HarvestLink(wxString& url, const wxString& fileExtension);
    //----------------------------------
    bool CrawlLinks(wxString& url,
                    const html_utilities::hyperlink_parse::hyperlink_parse_method method);
    // cppcheck-suppress constParameter
    void CrawlLink(const wxString& currentLink, html_utilities::html_url_format& formatUrl,
                   const wxString& mainUrl, const html_utilities::hyperlink_parse& linkParser);

    [[nodiscard]]
    bool HasUrlAlreadyBeenHarvested(const wxString& url) const
        {
        return (m_harvestedLinks.contains(url));
        }

    [[nodiscard]]
    bool HasUrlAlreadyBeenCrawled(const wxString& url) const
        {
        return (m_alreadyCrawledFiles.contains(url));
        }

    [[nodiscard]]
    bool VerifyFileExtension(const wxString& fileExt) const
        {
        return (m_fileExtensions.contains(fileExt));
        }

    class wxStringLessNoCase
        {
      public:
        [[nodiscard]]
        inline bool operator()(const wxString& first, const wxString& second) const
            {
            return (first.CmpNoCase(second) < 0);
            }
        };

    size_t m_levelDepth{ 1 };
    wxString m_url;
    wxString m_userAgent;
    bool m_disablePeerVerify{ false };
    bool m_useJsCookies{ false };
    bool m_persistJsCookies{ false };
    string_util::case_insensitive_wstring m_domain;
    string_util::case_insensitive_wstring m_fullDomain;
    string_util::case_insensitive_wstring m_fullDomainFolderPath;
    std::set<string_util::case_insensitive_wstring> m_allowableWebFolders;
    std::set<wxString, wxStringLessNoCase> m_fileExtensions;
    // cached state information
    std::set<wxString, wxStringLessNoCase> m_JsCookies;
    std::set<wxString, wxStringLessWebPath> m_harvestedLinks;
    std::set<wxString> m_downloadedFiles;
    std::map<wxString, wxString> m_brokenLinks;
    std::set<wxString, wxStringLessWebPath> m_alreadyCrawledFiles;
    int m_currentLevel{ 0 };
    bool m_isCancelled{ false };

    wxString m_downloadDirectory;
    bool m_keepWebPathWhenDownloading{ true };
    NonWebPageFileExtension IsNonWebPageFileExtension;
    ScriptFileExtension IsScriptFileExtension;
    WebPageExtension IsWebPageExtension;
    // download criteria
    DomainRestriction m_domainRestriction{ DomainRestriction::RestrictToDomain };
    bool m_downloadWhileCrawling{ false };

    std::optional<uint32_t> m_minFileDownloadSizeKilobytes{ std::nullopt };
    bool m_replaceExistingFiles{ true };
    bool m_harvestAllHtml{ true };
    bool m_searchForBrokenLinks{ false };
    bool m_hideFileNamesWhileDownloading{ false };

    FileDownload m_downloader;
    // UI functionality
    wxProgressDialog* m_progressDlg{ nullptr };

    inline constexpr static std::wstring_view HTML_CONTENT_TYPE{ _DT(L"text/html") };
    inline constexpr static std::wstring_view JAVASCRIPT_CONTENT_TYPE{ _DT(
        L"application/x-javascript") };
    inline constexpr static std::wstring_view VBSCRIPT_CONTENT_TYPE{ _DT(
        L"application/x-vbscript") };
    };

#endif // WEBHARVESTER_H
