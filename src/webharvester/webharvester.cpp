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

#include "webharvester.h"
#include "filepathresolver.h"
#include "wx/tokenzr.h"

//----------------------------------
bool wxStringLessWebPath::operator()(const wxString& first, const wxString& second) const
    {
    FilePathResolver resolver;
    wxString firstPath = resolver.ResolvePath(first, true);
    wxString secondPath = resolver.ResolvePath(second, true);
    if (!firstPath.empty() && firstPath[firstPath.length() - 1] == L'/')
        {
        firstPath.RemoveLast();
        }
    if (!secondPath.empty() && secondPath[secondPath.length() - 1] == L'/')
        {
        secondPath.RemoveLast();
        }
    return (firstPath.CmpNoCase(secondPath) < 0);
    }

//----------------------------------
wxString WebHarvester::DownloadFile(wxString& url, const wxString& fileExtension /*= wxString{}*/)
    {
    if (url.empty())
        {
        return {};
        }

    // if a OneDrive document, then we need to connect to the page it is display on
    // and download the file from there
    if (IsOneDriveDocument(url))
        {
        if (!m_downloader.DownloadOneDriveFile(url, GetDownloadDirectory()))
            {
            return {};
            }
        const auto downloadPath = GetDownloadDirectory() + wxFileName::GetPathSeparator() +
                                  m_downloader.GetLastOneDriveFileName();
        m_downloadedFiles.insert(downloadPath);
        return downloadPath;
        }

    // strip off bookmark (if there is one)
    const auto bookMarkIndex = url.rfind(L'#');
    if (bookMarkIndex != wxString::npos)
        {
        url.Truncate(bookMarkIndex);
        }
    url = NormalizeUrl(url);

    // remove "https" (and the like) from the file path so that when
    // we build a mirrored local folder structure, we don't have a
    // folder named "https."
    wxString urlLocalFileFriendlyName = url;
    const wxRegEx re(L"^(http|https|ftp|ftps|gopher|file)://", wxRE_ICASE | wxRE_EXTENDED);
    re.ReplaceFirst(&urlLocalFileFriendlyName, wxString{});
    // in case of an url like www.company.com/events/
    if (!urlLocalFileFriendlyName.empty() &&
        urlLocalFileFriendlyName[urlLocalFileFriendlyName.length() - 1] == L'/')
        {
        // chop off the trailing '/'
        urlLocalFileFriendlyName.RemoveLast();
        // add an extension to the url, unless it is the main page
        if (!html_utilities::html_url_format::is_url_top_level_domain(
                urlLocalFileFriendlyName.wc_str()))
            {
            urlLocalFileFriendlyName.Append(L".htm");
            }
        }

    // first create the folder to save the file
    wxString downloadPath = m_downloadDirectory;
    if (!downloadPath.empty() &&
        downloadPath[downloadPath.length() - 1] != wxFileName::GetPathSeparator())
        {
        downloadPath += wxFileName::GetPathSeparator();
        }
    // "mirror" the webpage's path on the local system
    if (m_keepWebPathWhenDownloading)
        {
        html_utilities::html_url_format formatUrl(urlLocalFileFriendlyName.wc_str());
        wxString webDirPath = formatUrl.get_directory_path().c_str();

        // convert the path to something acceptable for the local system
#ifdef __WXMSW__
        webDirPath.Replace(L"/", L"\\");
#endif
        webDirPath = StripIllegalFileCharacters(webDirPath);
        if (!webDirPath.empty() &&
            webDirPath[webDirPath.length() - 1] != wxFileName::GetPathSeparator())
            {
            webDirPath += wxFileName::GetPathSeparator();
            }
        downloadPath += webDirPath;
        }

    const wxString downloadPathFolder = downloadPath;

    // and see where it will be downloaded locally
    // (make sure file name is legal for the local file system)
    const wxString fileName = wxFileName(urlLocalFileFriendlyName).GetFullName();
    if (fileName.empty())
        {
        return {};
        }
    downloadPath = downloadPath + StripIllegalFileCharacters(fileName);

    /* Check the extension on the file we are downloading. It might not have one,
       or it might be a domain that wouldn't make sense for
       local file types. If so, append the file extension "hint" onto it
       (or determine it from the MIME type).*/
    const wxString webFileExt = GetExtensionOrDomain(downloadPath);
    if (webFileExt.empty() ||
        html_utilities::html_url_format::is_url_top_level_domain(url.wc_str()))
        {
        downloadPath += L".html";
        }
    // if a PHP query (or something like that)...
    else if (wxFileName{ downloadPath }.GetExt().length() > 4)
        {
        // ...append the file extension based on MIME type
        // if GetExtensionOrDomain() couldn't figure out an extension
        // cppcheck-suppress knownConditionTrueFalse
        if (webFileExt.empty())
            {
            wxLogVerbose(L"'%s': querying file type from MIME type", url);
            int rCode{ 200 };
            const wxString downloadExt = StripIllegalFileCharacters(
                !fileExtension.empty() ? fileExtension :
                                         GetFileTypeFromContentType(GetContentType(url, rCode)));
            // If we needed to connect to the page to get its MIME type, then check the response
            // code while we are at it. Bail early if we got a bad response (or timed out).
            if (fileExtension.empty() && QueueDownload::IsBadResponseCode(rCode))
                {
                wxLogVerbose(L"'%s': bad response from web page; unable to download", url);
                return {};
                }
            downloadPath += L'.' + downloadExt;
            }
        // ...or use what GetExtensionOrDomain() was able to figure out
        else
            {
            downloadPath += L'.' + webFileExt;
            }
        }
    // ...otherwise, the download path already has a proper extension

    if (!m_replaceExistingFiles && wxFileName::FileExists(downloadPath))
        {
        // if the file already exists, and we aren't overwriting,
        // then create a different name for it
        downloadPath = CreateNewFileName(downloadPath);
        if (downloadPath.empty())
            {
            return {};
            }
        }

    wxYield();
    if (m_progressDlg != nullptr)
        {
        wxStringTokenizer tkz(url, L"\n\r", wxTOKEN_STRTOK);
        const wxString urlLabel = tkz.GetNextToken();
        if (!m_progressDlg->Pulse(m_hideFileNamesWhileDownloading ?
                                      _(L"Downloading...") :
                                      wxString::Format(_(L"Downloading \"%s\""), urlLabel)))
            {
            m_isCancelled = true;
            return {};
            }
        }

    wxLogVerbose(L"Preparing to download '%s'", url);
    // create the target folder
    if (!wxFileName::DirExists(downloadPathFolder))
        {
        wxFileName::Mkdir(downloadPathFolder, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        }

    if (m_useJsCookies)
        {
        wxString fileText;
        wxString contentType;
        wxString statusText;
        int responseCode{ 200 };
        if (ReadWebPage(url, fileText, contentType, statusText, responseCode, true))
            {
            std::wstring cookies = html_utilities::javascript_hyperlink_parse::get_cookies(
                { fileText.wc_str(), fileText.length() });
            if (m_persistJsCookies)
                {
                m_JsCookies.insert(cookies);
                cookies.clear();
                for (const auto& cookie : m_JsCookies)
                    {
                    cookies += cookie + L';';
                    }
                if (!cookies.empty() && cookies[cookies.length() - 1] == L';')
                    {
                    cookies.erase(cookies.length() - 1);
                    }
                }
            m_downloader.SetCookies(cookies.c_str());
            }
        }

    // now download the file locally
    if (m_downloader.Download(url, downloadPath))
        {
        m_downloadedFiles.insert(downloadPath);
        }
    else
        {

        // check the response code
        // NOLINTBEGIN(cppcoreguidelines-init-variables)
        if (const int responseCode{ m_downloader.GetLastStatus() };
            QueueDownload::IsBadResponseCode(responseCode))
            {
            wxLogWarning(L"%s: unable to connect to page, error code #%i (%s).", url, responseCode,
                         QueueDownload::GetResponseMessage(responseCode));
            }
        // NOLINTEND(cppcoreguidelines-init-variables)
        wxLogWarning(L"Unable to download to '%s': %s", downloadPath,
                     m_downloader.GetLastStatusText());
        downloadPath.clear();
        }

    // reset to empty cookies
    m_downloader.SetCookies(wxString{});

    return downloadPath;
    }

//----------------------------------
wxString WebHarvester::GetFileTypeFromContentType(const wxString& contentType)
    {
    wxString fileExt;
    auto slash = contentType.find(L'/');
    if (slash != wxString::npos)
        {
        fileExt = contentType.substr(++slash);
        // in case there is charset info at the end, then chop that off
        const auto semiColon = fileExt.find(L';');
        if (semiColon != wxString::npos)
            {
            fileExt = fileExt.substr(0, semiColon);
            }
        }
    return fileExt;
    }

//----------------------------------
wxString WebHarvester::GetContentType(wxString& url, int& responseCode)
    {
    responseCode = 404;

    if (url.empty())
        {
        return wxString{};
        }
    url = NormalizeUrl(url);

    wxLogVerbose(L"Preparing to get content type from '%s'", url);
    m_downloader.RequestResponse(url);

    responseCode = m_downloader.GetLastStatus();

    return m_downloader.GetLastContentType();
    }

//----------------------------------
bool WebHarvester::ReadWebPage(wxString& url, wxString& webPageContent, wxString& contentType,
                               wxString& statusText, int& responseCode,
                               const bool acceptOnlyHtmlOrScriptFiles /*= true*/)
    {
    webPageContent.clear();
    contentType.clear();
    statusText.clear();
    responseCode = 404;

    if (url.empty())
        {
        return false;
        }

    // strip off bookmark (if there is one)
    const auto bookMarkIndex = url.rfind(L'#');
    if (bookMarkIndex != wxString::npos)
        {
        url.Truncate(bookMarkIndex);
        }
    url = NormalizeUrl(url);

    wxLogVerbose(L"Preparing to read %s", url);
    if (!m_downloader.Read(url))
        {
        responseCode = m_downloader.GetLastStatus();
        statusText = m_downloader.GetLastStatusText();
        wxLogWarning(L"%s: Unable to connect to page, error code #%i (%s).", url, responseCode,
                     QueueDownload::GetResponseMessage(responseCode));
        return false;
        }

    responseCode = m_downloader.GetLastStatus();
    statusText = m_downloader.GetLastStatusText();
    if (QueueDownload::IsBadResponseCode(responseCode))
        {
        wxLogWarning(L"%s: Unable to connect to page, error code #%i (%s).", url, responseCode,
                     QueueDownload::GetResponseMessage(responseCode));
        return false;
        }
    if (!m_downloader.GetLastRead().empty())
        {
        contentType = m_downloader.GetLastContentType();
        if (contentType.empty())
            {
            contentType = L"text/html; charset=utf-8";
            }

        // first make sure it is really a webpage
        if (acceptOnlyHtmlOrScriptFiles && !contentType.empty() &&
            string_util::strnicmp(contentType.wc_str(), HTML_CONTENT_TYPE.data(),
                                  HTML_CONTENT_TYPE.length()) != 0 &&
            string_util::strnicmp(contentType.wc_str(), JAVASCRIPT_CONTENT_TYPE.data(),
                                  JAVASCRIPT_CONTENT_TYPE.length()) != 0 &&
            string_util::strnicmp(contentType.wc_str(), VBSCRIPT_CONTENT_TYPE.data(),
                                  VBSCRIPT_CONTENT_TYPE.length()) != 0)
            {
            return false;
            }

        // get redirect URL (if we got redirected)
        url = m_downloader.GetLastUrl();
        // Block redirects to non-HTTP(S) schemes (SSRF protection)
        const FilePathResolver redirectCheck(url, false);
        if (!redirectCheck.IsHTTPFile() && !redirectCheck.IsHTTPSFile())
            {
            wxLogWarning(L"Blocked redirect to non-HTTP(S) scheme: %s", url);
            m_alreadyCrawledFiles.insert(url);
            --m_currentLevel;
            return false;
            }
        /* Convert from the file's charset to the application's charset.
           Try to get it from the response header first because that is more
           accurate when the file is really UTF-8 but the designer put something like
           8859-1 in the meta section. If that fails, then read the meta section.*/
        wxString charSet = GetCharsetFromContentType(contentType); // NOLINT(misc-const-correctness)
        if (charSet.empty())
            {
            charSet = GetCharsetFromPageContent(
                { m_downloader.GetLastRead().data(), m_downloader.GetLastRead().size() });
            }
        // Watch out for embedded NULLs in stream.
        // It may be in the middle of the text, or just at the end of the zeroed-out stream
        // (where what was read wasn't as large as the reported size).
        // In this situation, we need to split the stream into valid chunks, convert them,
        // and then piece them back together (or simply read the valid text up to the
        // zeroed-out region).
        if (string_util::strnlen(m_downloader.GetLastRead().data(),
                                 m_downloader.GetLastRead().size()) <
            m_downloader.GetLastRead().size())
            {
            webPageContent = Wisteria::TextStream::CharStreamWithEmbeddedNullsToUnicode(
                m_downloader.GetLastRead().data(), m_downloader.GetLastRead().size(), charSet);
            }
        else
            {
            webPageContent = Wisteria::TextStream::CharStreamToUnicode(
                m_downloader.GetLastRead().data(), m_downloader.GetLastRead().size(), charSet);
            }
        }
    else
        {
        responseCode = 204;
        wxLogWarning(L"'%s': connection successful, but no content was read.", url);
        return false;
        }

    return true;
    }

//----------------------------------
bool WebHarvester::ReadWebDocument(wxString& url, wxString& statusText, int& responseCode)
    {
    statusText.clear();
    responseCode = 404;

    if (url.empty())
        {
        return false;
        }

    // strip off bookmark (if there is one)
    const auto bookMarkIndex = url.rfind(L'#');
    if (bookMarkIndex != wxString::npos)
        {
        url.Truncate(bookMarkIndex);
        }
    url = NormalizeUrl(url);

    wxLogVerbose(L"Preparing to read %s", url);
    // if a OneDrive document, then we need to connect to the page it is display on
    // and download the file from there
    if (IsOneDriveDocument(url))
        {
        if (!m_downloader.ReadOneDriveFile(url))
            {
            responseCode = m_downloader.GetLastStatus();
            statusText = m_downloader.GetLastStatusText();
            if (QueueDownload::IsBadResponseCode(responseCode))
                {
                wxLogWarning(L"%s: Unable to connect to page, error code #%i (%s).", url,
                             responseCode, QueueDownload::GetResponseMessage(responseCode));
                }
            return false;
            }
        responseCode = m_downloader.GetLastStatus();
        statusText = m_downloader.GetLastStatusText();
        return true;
        }

    return false;
    }

//----------------------------------
bool WebHarvester::IsPageHtml(wxString& url, wxString& contentType, int& responseCode)
    {
    contentType.clear();
    if (url.empty())
        {
        return false;
        }

    contentType = GetContentType(url, responseCode);
    if (contentType.empty())
        {
        return false;
        }
    return string_util::strnicmp(contentType.wc_str(), HTML_CONTENT_TYPE.data(),
                                 HTML_CONTENT_TYPE.length()) == 0;
    }

//----------------------------------
void WebHarvester::DownloadFiles()
    {
    m_downloadedFiles.clear();
    // go through the harvested links and download the files
    // matching the ones that we want to download
    for (const auto& link : m_harvestedLinks)
        {
        if (m_isCancelled)
            {
            break;
            }
        wxString currentPage{ link };
        DownloadFile(currentPage);
        }
    }

//----------------------------------
bool WebHarvester::CrawlLinks()
    {
    wxStringTokenizer tkz(m_url, L"\n\r", wxTOKEN_STRTOK);
    const wxString urlLabel = m_hideFileNamesWhileDownloading ? wxString{ _(L"...") } :
                                                                L" \"" + tkz.GetNextToken() + L"\"";
    m_progressDlg = new wxProgressDialog(_(L"Web Harvester"),
                                         // TRANSLATORS: Gathering pages from a website.
                                         wxString::Format(_(L"Harvesting %s"), urlLabel), 5,
                                         nullptr, wxPD_SMOOTH | wxPD_ELAPSED_TIME | wxPD_CAN_ABORT);
    m_progressDlg->Centre();
    m_progressDlg->Raise();

    // reset state information
    m_isCancelled = false;
    m_currentLevel = 0;

    m_harvestedLinks.clear();
    m_downloadedFiles.clear();
    m_brokenLinks.clear();
    m_alreadyCrawledFiles.clear();
    // depth level of zero means that we just want to download the root URL and don't actually
    // crawl anything
    if (GetDepthLevel() > 0)
        {
        CrawlLinks(m_url, html_utilities::hyperlink_parse::hyperlink_parse_method::html);
        }

    // Now check the original URL to see if it is a file that should be downloaded
    // (falling back to HTM if no extension, as most webpages are)
    wxString fnExt = GetExtensionOrDomain(m_url);
    if (fnExt.empty())
        {
        fnExt = L"htm";
        }
    // Add the link to files to harvest/download if it matches our criteria
    if ((m_harvestAllHtml && IsWebPageExtension(fnExt)) || VerifyFileExtension(fnExt))
        {
        HarvestLink(m_url, fnExt);
        }
    m_progressDlg->Pulse();

    m_progressDlg->Destroy();
    m_progressDlg = nullptr;

    // will be empty anyway if search was turned off
    for (const auto& bLink : GetBrokenLinks())
        {
        wxLogWarning(L"Broken link '%s' (from '%s')", bLink.first, bLink.second);
        }

    wxLogVerbose(L"Crawling '%s' complete.", m_url);

    return !m_isCancelled;
    }

//----------------------------------
bool WebHarvester::CrawlLinks(wxString& url,
                              const html_utilities::hyperlink_parse::hyperlink_parse_method method)
    {
    if (m_isCancelled || url.empty() || HasUrlAlreadyBeenCrawled(url))
        {
        return false;
        }

    ++m_currentLevel;
    if (m_currentLevel > GetDepthLevel())
        {
        --m_currentLevel;
        return false;
        }

    wxString fileText;
    const FilePathResolver resolve(url, true);
    if (resolve.IsHTTPFile() || resolve.IsHTTPSFile())
        {
        // read in the page
        wxString contentType;
        wxString statusText;
        int responseCode{ 0 };
        if (!ReadWebPage(url, fileText, contentType, statusText, responseCode, true))
            {
            --m_currentLevel;
            // don't bother trying to crawl this later if it failed
            m_alreadyCrawledFiles.insert(url);
            return false;
            }
        if (m_useJsCookies)
            {
            std::wstring cookies = html_utilities::javascript_hyperlink_parse::get_cookies(
                { fileText.wc_str(), fileText.length() });
            if (!cookies.empty())
                {
                if (m_persistJsCookies)
                    {
                    m_JsCookies.insert(cookies);
                    cookies.clear();
                    for (const auto& cookie : m_JsCookies)
                        {
                        cookies += cookie + L';';
                        }
                    if (!cookies.empty() && cookies[cookies.length() - 1] == L';')
                        {
                        cookies.erase(cookies.length() - 1);
                        }
                    }
                m_downloader.SetCookies(cookies.c_str());

                if (!ReadWebPage(url, fileText, contentType, statusText, responseCode, true))
                    {
                    --m_currentLevel;
                    // don't bother trying to crawl this later if it failed
                    m_alreadyCrawledFiles.insert(url);
                    return false;
                    }

                // reset to empty cookies
                m_downloader.SetCookies(wxString{});
                }
            }
        }
    else if (resolve.IsLocalOrNetworkFile())
        {
        Wisteria::TextStream::ReadFile(url, fileText);
        }
    else
        {
        --m_currentLevel;
        return false;
        }

    // Just in case the url was redirected, make sure it isn't one that we have already crawled
    // or isn't a different domain than what we are allowing.
    if (!VerifyUrlDomainCriteria(url))
        {
        // prevent crawling it later if it doesn't meet our criteria
        m_alreadyCrawledFiles.insert(url);
        --m_currentLevel;
        return false;
        }
    if (HasUrlAlreadyBeenCrawled(url))
        {
        --m_currentLevel;
        return false;
        }

    // it's ready to be crawled now, so marked it as crawled for later
    m_alreadyCrawledFiles.insert(url);

    wxYield();
    if (m_progressDlg != nullptr)
        {
        wxStringTokenizer tkz(url, L"\n\r", wxTOKEN_STRTOK);
        const wxString urlLabel = tkz.GetNextToken();
        if (!m_progressDlg->Pulse(m_hideFileNamesWhileDownloading ?
                                      _(L"Harvesting...") :
                                      wxString::Format(_(L"Harvesting \"%s\""), urlLabel)))
            {
            m_isCancelled = true;
            --m_currentLevel;
            return false;
            }
        }

    html_utilities::hyperlink_parse getHyperLinks(fileText.wc_str(), fileText.length(), method);
    if (getHyperLinks.get_parse_method() ==
            html_utilities::hyperlink_parse::hyperlink_parse_method::html &&
        getHyperLinks.get_html_parser().get_base_url() != nullptr)
        {
        url.assign(getHyperLinks.get_html_parser().get_base_url(),
                   getHyperLinks.get_html_parser().get_base_url_length());
        }

    // NOTE: if a 302 or 300 are encountered, then the url may be different now,
    // as well as a base url in the head
    html_utilities::html_url_format formatUrl(url.wc_str());
    while (true)
        {
        // gather its hyperlinks
        const wchar_t* currentLink = getHyperLinks();
        if (currentLink != nullptr)
            {
            CrawlLink(wxString(currentLink, getHyperLinks.get_current_hyperlink_length()),
                      formatUrl, url, getHyperLinks);
            }
        else
            {
            break;
            }
        if (m_isCancelled)
            {
            --m_currentLevel;
            return false;
            }
        }
    --m_currentLevel;

    return true;
    }

//----------------------------------
void WebHarvester::CrawlLink(const wxString& currentLink,
                             // cppcheck-suppress constParameter
                             html_utilities::html_url_format& formatUrl, const wxString& mainUrl,
                             const html_utilities::hyperlink_parse& linkParser)
    {
    if (m_isCancelled)
        {
        return;
        }

    // if an image (can only determine this if using the HTML parser)
    const bool isImage = (linkParser.get_parse_method() ==
                          html_utilities::hyperlink_parse::hyperlink_parse_method::html) ?
                             linkParser.get_html_parser().is_current_link_an_image() :
                             false;

    if (m_progressDlg != nullptr)
        {
        if (!m_progressDlg->Pulse(m_hideFileNamesWhileDownloading ?
                                      // TRANSLATORS: Searching across websites.
                                      _(L"Crawling...") :
                                      wxString::Format(_(L"Crawling \"%s\""), currentLink)))
            {
            m_isCancelled = true;
            --m_currentLevel;
            return;
            }
        }

    // skip "mailto" anchors, telephone numbers, placeholders,
    // and any bookmarks on the same page
    const wxRegEx anchorsToSkip{ L"[[:space:]]*(mailto[:]|tel[:]|#|"
                                 "javascript[:][[:space:]]*void).*" };

    if (anchorsToSkip.Matches(currentLink) ||
        // weird placeholders to ignore
        currentLink == _DT(L"%link%"))
        {
        return;
        }

    wxString fullUrl;
    fullUrl.assign(formatUrl(currentLink.wc_str(), isImage));
    if (fullUrl.empty())
        {
        return;
        }
    if ((m_currentLevel > GetDepthLevel() || HasUrlAlreadyBeenCrawled(fullUrl)) &&
        HasUrlAlreadyBeenHarvested(fullUrl))
        {
        return;
        }
    // this should usually be turned off for performance,
    // but it's a nice way to get a list of broken links from a site
    if (IsSearchingForBrokenLinks())
        {
        int responseCode{ 200 };
        GetContentType(fullUrl, responseCode);
        if (responseCode == 404)
            {
            m_brokenLinks.emplace(fullUrl, mainUrl);
            }
        if (QueueDownload::IsBadResponseCode(responseCode))
            {
            wxLogVerbose(L"'%s': bad response from web page; unable to crawl page", fullUrl);
            return;
            }
        }

    // first make sure that if we are domain restricted,
    // then don't bother with it if it's from another domain
    if (!VerifyUrlDomainCriteria(fullUrl))
        {
        return;
        }

    wxString fileExt = GetExtensionOrDomain(fullUrl);
    // If no extension, fall back to it being a regular webpage (or JS file)
    // Modern webpages generally don't have HTM extensions (or any extension) like in the past.
    if (fileExt.empty())
        {
        if ((linkParser.get_parse_method() ==
             html_utilities::hyperlink_parse::hyperlink_parse_method::html) &&
            linkParser.get_html_parser().is_current_link_a_javascript())
            {
            fileExt = L"js";
            }
        else
            {
            fileExt = L"htm";
            }
        }

    /* See if the page is HTML so that we know whether to crawl it or not. Sometimes
       the file extension on a page is different from its actual content, so if we are
       verifying the content, then also connect to it and read its mime type. Otherwise,
       go off of its extension and if that doesn't work then read its mime type.*/
    wxString contentType;
    bool pageIsHtml = false;

    if (IsNonWebPageFileExtension(fileExt.wc_str()) || IsScriptFileExtension(fileExt.wc_str()))
        {
        pageIsHtml = false;
        }
    else if (IsWebPageExtension(fileExt))
        {
        pageIsHtml = true;
        }
    else if (html_utilities::html_url_format::is_url_top_level_domain(fullUrl.wc_str()))
        {
        pageIsHtml = true;
        }
    else
        {
        int responseCode{ 200 };
        pageIsHtml = IsPageHtml(fullUrl, contentType, responseCode);
        if (QueueDownload::IsBadResponseCode(responseCode))
            {
            wxLogVerbose(L"'%s': bad response from web page; unable to crawl page", fullUrl);
            return;
            }
        }

    const html_utilities::html_url_format formatCurrentUrl(fullUrl.wc_str());

    // First, crawl the page (if applicable)
    ///////////////////////////////////////
    // Javascript/VBScript files are crawled differently, so check that first
    if (IsScriptFileExtension(fileExt.wc_str()))
        {
        CrawlLinks(fullUrl, html_utilities::hyperlink_parse::hyperlink_parse_method::script);
        if (VerifyFileExtension(fileExt))
            {
            HarvestLink(fullUrl, fileExt);
            }
        return;
        }
    if (IsNonWebPageFileExtension(fileExt.wc_str()))
        {
        // add the link to files to harvest/download if it matches our criteria
        if ((m_harvestAllHtml && pageIsHtml) || VerifyFileExtension(fileExt))
            {
            HarvestLink(fullUrl, fileExt);
            }
        }
    else
        {
        /* If not a known "regular" file extension (e.g., PDF) or a webpage extension (e.g., HTML),
           then figure out its type. If a webpage, then crawl it or see if it is a type of file that
           we want to download.*/
        if (IsWebPageExtension(fileExt) ||
            html_utilities::html_url_format::is_url_top_level_domain(fullUrl.wc_str()))
            {
            CrawlLinks(fullUrl, html_utilities::hyperlink_parse::hyperlink_parse_method::html);
            if (m_harvestAllHtml || VerifyFileExtension(fileExt))
                {
                HarvestLink(fullUrl, fileExt);
                }
            return;
            }

        // if it's a PHP query, then see if it is really a page needing to be crawled
        if (formatCurrentUrl.has_query())
            {
            if (pageIsHtml)
                {
                CrawlLinks(fullUrl, html_utilities::hyperlink_parse::hyperlink_parse_method::html);
                }
            // need to get this page's type and download it if it meets the criteria
            fileExt = GetFileTypeFromContentType(contentType);
            if (VerifyFileExtension(fileExt) || (pageIsHtml && m_harvestAllHtml))
                {
                // need to override its extension too because the url has a different
                // file extension on it due to it being a PHP query
                HarvestLink(fullUrl, fileExt);
                }
            return;
            }
        // otherwise, an HTML page with an unknown extension
        if (pageIsHtml)
            {
            CrawlLinks(fullUrl, html_utilities::hyperlink_parse::hyperlink_parse_method::html);
            if (m_harvestAllHtml || VerifyFileExtension(fileExt))
                {
                HarvestLink(fullUrl, fileExt);
                }
            return;
            }
        // ...finally, not a webpage and an unknown extension.
        // Just figure out its real type and see if we should download it
        if (!contentType.empty())
            {
            fileExt = GetFileTypeFromContentType(contentType);
            if (VerifyFileExtension(fileExt))
                {
                HarvestLink(fullUrl, fileExt);
                }
            return;
            }
        }
    }

//----------------------------------
bool WebHarvester::VerifyUrlDomainCriteria(const wxString& url) const
    {
    // the root url should always pass this test, even if its own domain is not
    // in the list of allowable domains
    if (url.CmpNoCase(GetUrl()) == 0)
        {
        return true;
        }
    // this URL should already be fully expanded, so just initialize the formatter with it
    // as the root URL and then set it as the main URL.
    html_utilities::html_url_format formatUrl(url.wc_str());
    formatUrl(url.wc_str(), false);
    if (m_domainRestriction == DomainRestriction::RestrictToDomain)
        {
        if (m_domain != formatUrl.get_domain())
            {
            return false;
            }
        }
    else if (m_domainRestriction == DomainRestriction::RestrictToSubDomain)
        {
        if (m_fullDomain != formatUrl.get_full_domain())
            {
            return false;
            }
        }
    else if (m_domainRestriction == DomainRestriction::RestrictToExternalLinks)
        {
        if (m_domain == formatUrl.get_domain() || m_fullDomain == formatUrl.get_full_domain())
            {
            return false;
            }
        }
    else if (m_domainRestriction == DomainRestriction::RestrictToSpecificDomains)
        {
        // NOLINTBEGIN(readability-redundant-string-cstr)
        if (!m_allowableWebFolders.contains(formatUrl.get_directory_path().c_str()))
            {
            return false;
            }
        // NOLINTEND(readability-redundant-string-cstr)
        }
    else if (m_domainRestriction == DomainRestriction::RestrictToFolder)
        {
        // NOLINTBEGIN(readability-redundant-string-cstr)
        const string_util::case_insensitive_wstring targetUrlPath{
            formatUrl.get_directory_path().c_str()
        };
        // NOLINTEND(readability-redundant-string-cstr)
        return targetUrlPath.starts_with(m_fullDomainFolderPath);
        }
    return true;
    }

//----------------------------------
bool WebHarvester::HarvestLink(wxString& url, const wxString& fileExtension)
    {
    if (m_isCancelled || url.empty() || !VerifyUrlDomainCriteria(url))
        {
        return false;
        }
    if (HasUrlAlreadyBeenHarvested(url))
        {
        return true;
        }

    // Verify that the file is HTTP/HTTPS.
    const FilePathResolver resolve(url, false);
    if (!resolve.IsHTTPFile() && !resolve.IsHTTPSFile())
        {
        return false;
        }
    m_harvestedLinks.insert(url);
    if (IsDownloadingFilesWhileCrawling())
        {
        wxString downloadPath = DownloadFile(url, fileExtension);
        if (!downloadPath.empty())
            {
            // Some image links are actually HTML pages being used as a gallery of sorts
            // for an image, so treat it as such if the MIME type indicates that.
            if (fileExtension.CmpNoCase(L"jpg") == 0 || fileExtension.CmpNoCase(L"jpeg") == 0 ||
                fileExtension.CmpNoCase(L"png") == 0)
                {
                const wxString actualFileType =
                    GetFileTypeFromContentType(m_downloader.GetLastContentType());
                if (actualFileType.CmpNoCase(L"html") == 0)
                    {
                    wxLogVerbose(
                        L"'%s': image is really HTML; will attempt to download actual image", url);
                    wxString fileText;
                    if (Wisteria::TextStream::ReadFile(downloadPath, fileText))
                        {
                        std::set<wxString, wxStringLessWebPath> queuedDownloads;
                        const wxString jpgName{ wxFileName{ downloadPath }.GetFullName() };

                        html_utilities::hyperlink_parse getHyperLinks(
                            fileText.wc_str(), fileText.length(),
                            html_utilities::hyperlink_parse::hyperlink_parse_method::html);
                        // gather its hyperlinks
                        while (true)
                            {
                            const wchar_t* currentLink = getHyperLinks();
                            if (currentLink != nullptr)
                                {
                                const wxString cLink{
                                    currentLink, getHyperLinks.get_current_hyperlink_length()
                                };
                                const wxFileName fn(cLink);
                                if (fn.GetFullName().CmpNoCase(jpgName) == 0 &&
                                    html_utilities::html_url_format::is_absolute_url(
                                        cLink.wc_str()))
                                    {
                                    queuedDownloads.insert(cLink);
                                    }
                                }
                            else
                                {
                                break;
                                }
                            }
                        for (wxString link : queuedDownloads)
                            {
                            DownloadFile(link, fileExtension);
                            }
                        }
                    }
                }
            }
        }
    return true;
    }

//----------------------------------
wxString WebHarvester::GetCharsetFromContentType(const wxString& contentType)
    {
    const auto index = contentType.Lower().find(L"charset=");
    const auto semicolon = contentType.find(L";");
    if (index != wxString::npos)
        {
        wxString charSet = contentType.substr(index + 8);
        charSet.Replace(L"\"", L"");
        charSet.Replace(L"\'", L"");
        charSet.Trim(false);
        charSet.Trim(true);
        return charSet;
        }
    if (semicolon != wxString::npos)
        {
        wxString charSet = contentType.substr(semicolon + 1);
        charSet.Replace(L"\"", L"");
        charSet.Replace(L"\'", L"");
        charSet.Trim(false);
        charSet.Trim(true);
        return charSet;
        }

    const wxString encoding = wxLocale::GetSystemEncodingName().MakeLower();
    return (encoding.find(L"utf") != wxString::npos) ? wxString{ L"windows-1252" } : encoding;
    }

//----------------------------------
wxString WebHarvester::GetCharsetFromPageContent(std::string_view pageContent)
    {
    wxString charSet = lily_of_the_valley::html_extract_text::parse_charset(pageContent.data(),
                                                                            pageContent.length());
    if (charSet.empty())
        {
        // If system encoding is UTF (8, 16, etc.), then we don't want to convert from that.
        // We determine if content is UTF-8/16 elsewhere, so returning that here could
        // cause UTF-8 conversion on a file that isn't really UTF-8.
        // Legacy pages missing encoding info is probably ANSI, so fall back to Windows-1252
        // if our platform is using a Unicode encoding that we can't use.
        const wxString encoding = wxLocale::GetSystemEncodingName().MakeLower();
        return (encoding.find(L"utf") != wxString::npos) ? wxString{ L"windows-1252" } : encoding;
        }
    // The HTML 5 specification requires that documents advertised as
    // ISO-8859-1 actually be parsed with the Windows-1252 encoding.
    if (charSet.CmpNoCase("iso-8859-1") == 0)
        {
        charSet = "windows-1252";
        }
    return charSet;
    }
