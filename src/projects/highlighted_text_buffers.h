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

#include <map>
#include <wx/string.h>
#include <wx/webview.h>

/// @brief The export buffer for a highlighted-text window.
/// @details The HTML the window renders is held by the @c memory: file system, not
///     here; the RTF is a paper-white representation kept only for saving.
struct HighlightedTextBuffers
    {
    wxString m_rtf;
    /// @brief The @c memory: file-system key currently holding the window's HTML.
    wxString m_memoryKey;
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

    /// @brief Removes all cached pages before the map is destroyed.
    ~HighlightedTextBufferMap() { Clear(); }

    /// @brief Stores the buffers for @p window and loads the HTML into it via a
    ///     @c memory: URL, sidestepping the size limitations some backends impose
    ///     on @c wxWebView::SetPage().
    /// @param window The window to load the HTML into. Only used to push the
    ///     content in and to key the buffers by its ID; it is not retained.
    /// @param html The HTML to render.
    /// @param rtf The paper-white RTF used for saving and copying.
    void SetContent(wxWebView* window, wxString html, wxString rtf);
    /// @returns The buffers for @p windowId, or @c nullptr if none are stored.
    [[nodiscard]]
    const HighlightedTextBuffers* Find(const wxWindowID windowId) const;
    /// @brief Removes the buffers (and cached page) for @p windowId.
    void Remove(const wxWindowID windowId);
    /// @brief Removes every window's buffers and cached pages.
    void Clear();

  private:
    // removes the memory: file-system entry currently cached for a window (if any)
    void RemoveMemoryFile(const wxWindowID windowId);

    std::map<wxWindowID, HighlightedTextBuffers> m_buffers;
    // Keys unique across every window and every reload. Must be
    // process-wide because the memory file system is global and every open project
    // uses the same window IDs for its highlighted-text windows.
    inline static size_t m_refreshCounter{ 0 };
    };

#endif // HIGHLIGHTED_TEXT_BUFFERS_H
