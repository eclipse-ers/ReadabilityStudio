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

#ifndef CHAPTER_SPLIT_H
#define CHAPTER_SPLIT_H

#include "../Wisteria-Dataviz/src/util/fileutil.h"

class ChapterSplit
    {
  public:
    void SetOutputFolder(const wxString& outFolder) { m_outputFolder = outFolder; }

    void SplitIntoChapters(const wchar_t* fileText) const;
    static bool SplitHtmlByBookmarks(wxString& sourceFile, const bool convertToXhtml);

  private:
    void WriteChapterFile(const wchar_t* textStart, const wchar_t* textEnd) const;
    wxString m_outputFolder;
    };

#endif // CHAPTER_SPLIT_H
