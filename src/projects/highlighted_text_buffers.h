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

#ifndef HIGHLIGHTED_TEXT_BUFFERS_H
#define HIGHLIGHTED_TEXT_BUFFERS_H

#include "../Wisteria-Dataviz/src/util/donttranslate.h"
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <wx/filesys.h>
#include <wx/mstream.h>
#include <wx/string.h>
#include <wx/webview.h>

/// @brief A stream over a page of HTML that keeps that HTML alive itself.
/// @details Some backends read a page asynchronously and go on reading it after it
///     has been replaced, without reporting when they are done. Holding the bytes
///     by reference count instead of handing out a pointer into storage owned
///     elsewhere means there is no moment at which dropping a page is unsafe.
class HighlightedTextPageStream final : public wxMemoryInputStream
    {
  public:
    /// @brief Constructor.
    /// @param page The HTML to read, kept alive for the life of the stream.
    explicit HighlightedTextPageStream(std::shared_ptr<const std::string> page)
        : wxMemoryInputStream(page->data(), page->length()), m_page(std::move(page))
        {
        }

  private:
    std::shared_ptr<const std::string> m_page;
    };

/// @brief Serves the highlighted-text windows their HTML.
/// @details This is registered on every highlighted-text window, which then loads
///     its page by URL. That sidesteps the size limitations some backends impose
///     on @c wxWebView::SetPage(), and removing a page here only drops this
///     handler's reference to it.
class HighlightedTextPageHandler final : public wxWebViewHandler
    {
  public:
    HighlightedTextPageHandler() : wxWebViewHandler(GetScheme()) {}

    /// @returns The URL scheme that the highlighted-text windows load from.
    [[nodiscard]]
    static wxString GetScheme()
        {
        return _DT(L"highlightedtext");
        }

    /// @brief Stores @p html under @p key, replacing whatever was there.
    /// @param key The key to store the page under.
    /// @param html The UTF-8 encoded HTML to serve.
    void SetPage(const wxString& key, std::string html);
    /// @brief Drops this handler's reference to @p key's page.
    /// @param key The key to forget.
    void RemovePage(const wxString& key);
    /// @brief Drops this handler's reference to every page.
    void RemoveAllPages();

    /// @private
    [[nodiscard]]
    wxFSFile* GetFile(const wxString& uri) final;

  private:
    std::map<wxString, std::shared_ptr<const std::string>> m_pages;
    };

/// @brief The export buffer for a highlighted-text window.
/// @details The HTML the window renders is served by the page handler, not held
///     here; the RTF is a paper-white representation kept only for saving.
struct HighlightedTextBuffers
    {
    wxString m_rtf;
    /// @brief The handler key holding the window's HTML.
    wxString m_pageKey;
    };

/// @brief Maps highlighted-text window IDs to their HTML and RTF buffers.
/// @details A highlighted-text window is a plain @c wxWebView; this registry is
///     what marks such a window as "highlighted text" and holds
///     the buffers used to render and export it.
class HighlightedTextBufferMap
    {
  public:
    HighlightedTextBufferMap() = default;
    HighlightedTextBufferMap(const HighlightedTextBufferMap&) = delete;
    HighlightedTextBufferMap& operator=(const HighlightedTextBufferMap&) = delete;

    /// @brief Registers the page handler on @p window, which every
    ///     highlighted-text window needs before it can load anything.
    /// @param window The window to register on, before it is created.
    void RegisterHandler(wxWebView* window);

    /// @brief Stores the buffers for @p window and loads the HTML into it.
    /// @param window The window to load the HTML into. Only used to push the
    ///     content in and to key the buffers by its ID; it is not retained.
    /// @param html The HTML to render.
    /// @param rtf The paper-white RTF used for saving and copying.
    void SetContent(wxWebView* window, const wxString& html, wxString rtf);
    /// @returns The buffers for @p windowId, or @c nullptr if none are stored.
    [[nodiscard]]
    const HighlightedTextBuffers* Find(const wxWindowID windowId) const;
    /// @returns The window IDs of every currently cached highlighted-text window.
    [[nodiscard]]
    std::vector<wxWindowID> GetWindowIds() const;
    /// @brief Forgets every window's buffers and drops their pages.
    void Clear();

  private:
    [[nodiscard]]
    HighlightedTextPageHandler& GetPageHandler();

    // Shared by every highlighted-text window: RegisterHandler() takes a reference,
    // so the handler (and any page a window is still reading) outlives this map.
    wxSharedPtr<wxWebViewHandler> m_handler{ new HighlightedTextPageHandler };
    std::map<wxWindowID, HighlightedTextBuffers> m_buffers;
    // process-wide, so that no two documents can mint the same key; which handler
    // answers a request is up to the backend, and some of them route by scheme
    inline static size_t m_refreshCounter{ 0 };
    };

#endif // HIGHLIGHTED_TEXT_BUFFERS_H
