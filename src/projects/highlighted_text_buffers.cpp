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

#include "highlighted_text_buffers.h"

//-------------------------------------------------------
void HighlightedTextPageHandler::SetPage(const wxString& key, std::string html)
    {
    m_pages[key] = std::make_shared<const std::string>(std::move(html));
    }

//-------------------------------------------------------
void HighlightedTextPageHandler::RemovePage(const wxString& key) { m_pages.erase(key); }

//-------------------------------------------------------
void HighlightedTextPageHandler::RemoveAllPages() { m_pages.clear(); }

//-------------------------------------------------------
wxFSFile* HighlightedTextPageHandler::GetFile(const wxString& uri)
    {
    // the backends hand this back as "<scheme>:<key>", even the ones that rewrite
    // the url into an internal one to fetch it
    wxString key;
    if (!uri.StartsWith(GetScheme() + L":", &key))
        {
        return nullptr;
        }

    const auto pos = m_pages.find(key);
    if (pos == m_pages.cend())
        {
        return nullptr;
        }

    // the stream takes its own reference to the page, so removing it here can't
    // pull the bytes out from under a backend that is still reading them
    return new wxFSFile(new HighlightedTextPageStream(pos->second), uri,
                        _DT(L"text/html; charset=UTF-8"), wxString{}, wxDateTime::Now());
    }

//-------------------------------------------------------
HighlightedTextPageHandler& HighlightedTextBufferMap::GetPageHandler()
    {
    return *static_cast<HighlightedTextPageHandler*>(m_handler.get());
    }

//-------------------------------------------------------
void HighlightedTextBufferMap::RegisterHandler(wxWebView* window)
    {
    if (window != nullptr)
        {
        window->RegisterHandler(m_handler);
        }
    }

//-------------------------------------------------------
void HighlightedTextBufferMap::SetContent(wxWebView* window, const wxString& html, wxString rtf)
    {
    if (window == nullptr)
        {
        return;
        }
    const wxWindowID windowId = window->GetId();

    if (const auto pos = m_buffers.find(windowId); pos != m_buffers.cend())
        {
        GetPageHandler().RemovePage(pos->second.m_pageKey);
        }

    HighlightedTextBuffers buffers;
    buffers.m_rtf = std::move(rtf);
    // a fresh key every time, so that the window refetches instead of being served
    // its previous page from the backend's cache
    buffers.m_pageKey = wxString::Format(L"highlighted-%d-%zu.html", windowId, ++m_refreshCounter);
    // the html declares itself as UTF-8, so serve it as that
    GetPageHandler().SetPage(buffers.m_pageKey, html.utf8_string());

    window->LoadURL(HighlightedTextPageHandler::GetScheme() + L":" + buffers.m_pageKey);

    m_buffers[windowId] = std::move(buffers);
    }

//-------------------------------------------------------
const HighlightedTextBuffers* HighlightedTextBufferMap::Find(const wxWindowID windowId) const
    {
    const auto pos = m_buffers.find(windowId);
    return (pos != m_buffers.cend()) ? &pos->second : nullptr;
    }

//-------------------------------------------------------
std::vector<wxWindowID> HighlightedTextBufferMap::GetWindowIds() const
    {
    std::vector<wxWindowID> ids;
    ids.reserve(m_buffers.size());
    for (const auto& [id, buffer] : m_buffers)
        {
        ids.push_back(id);
        }
    return ids;
    }

//-------------------------------------------------------
void HighlightedTextBufferMap::Clear()
    {
    GetPageHandler().RemoveAllPages();
    m_buffers.clear();
    }
