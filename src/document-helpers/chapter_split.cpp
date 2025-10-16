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

#include "chapter_split.h"
#include "../Wisteria-Dataviz/src/import/html_encode.h"
#include "../indexing/sentence.h"

//------------------------------------------------
bool ChapterSplit::SplitHtmlByBookmarks(wxString& sourceFile, const bool convertToXhtml)
    {
    const wxString outputFolder =
        wxFileName(sourceFile).GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) +
        wxFileName(sourceFile).GetName();
    if (!wxFileName::DirExists(outputFolder))
        {
        if (outputFolder.empty() ||
            !wxFileName::Mkdir(outputFolder, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
            {
            return false;
            }
        }

    wxString fileText;
    Wisteria::TextStream::ReadFile(sourceFile, fileText);

    const wchar_t* htmlText = fileText.wc_str();
    const wchar_t* const htmlTextEnd = htmlText + fileText.length();

    // read the Head section
    wxString headSection{
        L"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
        "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" "
        "\"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n"
        "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\">\n<head>\n"
        "<meta content=\"text/html; charset=UTF-8\"/>\n<title></title>\n"
        "<link href=\"stylesheet.css\" type=\"text/css\" rel=\"stylesheet\" />\n"
        "<link rel=\"stylesheet\" type=\"application/vnd.adobe-page-template+xml\" "
        "href=\"page-template.xpgt\"/>\n</head>"
    };
    const wchar_t* headStart = lily_of_the_valley::html_extract_text::find_element(
        htmlText, htmlTextEnd, _DT(L"head"), true);
    if (!convertToXhtml && (headStart != nullptr))
        {
        const wchar_t* headEnd = lily_of_the_valley::html_extract_text::find_closing_element(
            headStart, htmlTextEnd, _DT(L"head"));
        if (headEnd != nullptr)
            {
            headEnd = lily_of_the_valley::html_extract_text::find_close_tag(headEnd);
            if (headEnd != nullptr)
                {
                headSection = wxString(headStart, headEnd + 1);
                }
            }
        headSection.insert(0, L"<html>\n");
        }
    // read the Body declaration
    wxString bodySection{ L"<body>" };
    const wchar_t* bodyStart = lily_of_the_valley::html_extract_text::find_element(
        htmlText, htmlTextEnd, _DT(L"body"), true);
    if (!convertToXhtml && (bodyStart != nullptr))
        {
        const wchar_t* bodyEnd = lily_of_the_valley::html_extract_text::find_close_tag(bodyStart);
        if (bodyEnd != nullptr)
            {
            bodyEnd = lily_of_the_valley::html_extract_text::find_close_tag(bodyEnd);
            if (bodyEnd != nullptr)
                {
                bodySection = wxString(bodyStart, bodyEnd + 1);
                }
            }
        }
    // begin splitting up the file
    auto bookmark = lily_of_the_valley::html_extract_text::find_bookmark(htmlText, htmlTextEnd);
    while (bookmark.first != nullptr)
        {
        auto nextBookMark =
            lily_of_the_valley::html_extract_text::find_bookmark(bookmark.first + 1, htmlTextEnd);
        const wchar_t* endOfFirstBookmark =
            lily_of_the_valley::html_extract_text::find_close_tag(bookmark.first);
        // shouldn't happen
        if (endOfFirstBookmark == nullptr)
            {
            break;
            }
        if (nextBookMark.first != nullptr)
            {
            std::wstring outputText(++endOfFirstBookmark, nextBookMark.first);
            if (!outputText.empty())
                {
                outputText.insert(0, wxString::Format(L"%s\n%s\n<div id=\"%s\">", headSection,
                                                      bodySection, bookmark.second.c_str()));
                outputText.append(L"\n</div>\n</body>\n</html>");
                string_util::replace_all(outputText, std::wstring(L"<mbp:pagebreak />"),
                                         std::wstring{});
                lily_of_the_valley::html_format::set_encoding(outputText, L"UTF-8");
                wxFile fout(
                    wxString::Format(L"%s\\%s.xhtml", outputFolder, bookmark.second.c_str()),
                    wxFile::write);
                fout.Write(outputText);
                }
            }
        else
            {
            std::wstring outputText(++endOfFirstBookmark);
            if (!outputText.empty())
                {
                outputText.insert(0, wxString::Format(L"%s\n%s\n<div id=\"%s\">", headSection,
                                                      bodySection, bookmark.second.c_str()));
                string_util::replace_all(outputText, std::wstring(L"<mbp:pagebreak />"),
                                         std::wstring{});
                const size_t endBodyTag = outputText.find(L"</body>");
                if (endBodyTag != std::wstring::npos)
                    {
                    outputText.insert(endBodyTag, L"\n</div>");
                    }
                lily_of_the_valley::html_format::set_encoding(outputText, L"UTF-8");
                wxFile fout(
                    wxString::Format(L"%s\\%s.xhtml", outputFolder, bookmark.second.c_str()),
                    wxFile::write);
                fout.Write(outputText);
                }
            }
        bookmark = nextBookMark;
        }
    return true;
    }

//------------------------------------------------
void ChapterSplit::WriteChapterFile(const wchar_t* textStart, const wchar_t* textEnd) const
    {
    if ((textStart == nullptr) || (textEnd == nullptr) || (textEnd - textStart) == 0)
        {
        return;
        }
    const wxString chapterContent(textStart, (textEnd - textStart));
    const wchar_t *firstLine(textStart), *endOfFirstLine(nullptr);
    while ((*firstLine != 0) && (firstLine < textEnd) && (std::iswspace(firstLine[0]) != 0))
        {
        ++firstLine;
        }
    endOfFirstLine = firstLine;
    constexpr grammar::is_end_of_line IS_EOL;
    while ((*endOfFirstLine != 0) && (endOfFirstLine < textEnd) && !IS_EOL(endOfFirstLine[0]))
        {
        ++endOfFirstLine;
        }
    const wxString chapterName =
        StripIllegalFileCharacters(wxString(firstLine, (endOfFirstLine - firstLine)));
    wxFile ff(m_outputFolder + wxFileName::GetPathSeparator() + chapterName + L".txt",
              wxFile::write);
    if (ff.IsOpened())
        {
        ff.Write(chapterContent, wxConvUTF8);
        }
    }

//------------------------------------------------
void ChapterSplit::SplitIntoChapters(const wchar_t* fileText) const
    {
    if (fileText == nullptr)
        {
        return;
        }
    const wchar_t* nextChapterStart = std::wcschr(fileText, 0x0C);
    while (nextChapterStart != nullptr)
        {
        WriteChapterFile(fileText, nextChapterStart);
        fileText = nextChapterStart + 1;
        nextChapterStart = std::wcschr(fileText, 0x0C);
        }
    WriteChapterFile(fileText, fileText + std::wcslen(fileText));
    }
