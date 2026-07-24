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
#include <wx/fs_mem.h>

//-------------------------------------------------------
void HighlightedTextBufferMap::SetContent(wxWebView* window, wxString html, wxString rtf)
    {
    if (window == nullptr)
        {
        return;
        }
    const wxWindowID windowId = window->GetId();
    // drop any page previously cached for this window before replacing it
    RemoveMemoryFile(windowId);

    HighlightedTextBuffers buffers;
    buffers.m_rtf = std::move(rtf);
    // the key must be unique across every window and every reload; otherwise, the
    // process-global memory file system would collide with a still-cached page
    buffers.m_memoryKey =
        wxString::Format(L"highlighted-%d-%zu.html", windowId, ++m_refreshCounter);
    // the memory file system takes its own copy of the HTML, so don't keep one here
    wxMemoryFSHandler::AddFileWithMimeType(buffers.m_memoryKey, html, L"text/html");

    window->LoadURL(L"memory:" + buffers.m_memoryKey);

    m_buffers[windowId] = std::move(buffers);
    }

//-------------------------------------------------------
const HighlightedTextBuffers* HighlightedTextBufferMap::Find(const wxWindowID windowId) const
    {
    const auto pos = m_buffers.find(windowId);
    return (pos != m_buffers.cend()) ? &pos->second : nullptr;
    }

//-------------------------------------------------------
void HighlightedTextBufferMap::Remove(const wxWindowID windowId)
    {
    RemoveMemoryFile(windowId);
    m_buffers.erase(windowId);
    }

//-------------------------------------------------------
void HighlightedTextBufferMap::Clear()
    {
    for (const auto& buffer : m_buffers)
        {
        if (!buffer.second.m_memoryKey.empty())
            {
            wxMemoryFSHandler::RemoveFile(buffer.second.m_memoryKey);
            }
        }
    m_buffers.clear();
    }

//-------------------------------------------------------
void HighlightedTextBufferMap::RemoveMemoryFile(const wxWindowID windowId)
    {
    // The ID only indexes this document's own map. What gets removed from the
    // process-global memory file system is the unique key stored in that entry,
    // so this can't reach another project's page. for example, two open projects
    // can each have a window with the ID 1234, but the ID resolves through
    // separate maps to separate keys:
    //     project A: 1234 -> its map -> "highlighted-1234-7.html"
    //     project B: 1234 -> its map -> "highlighted-1234-12.html"
    // and each map only removes the string that it stored
    const auto pos = m_buffers.find(windowId);
    if (pos != m_buffers.cend() && !pos->second.m_memoryKey.empty())
        {
        wxMemoryFSHandler::RemoveFile(pos->second.m_memoryKey);
        }
    }
