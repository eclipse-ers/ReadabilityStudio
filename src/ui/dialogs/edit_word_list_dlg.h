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

#ifndef EDIT_WORD_LIST_DLG_H
#define EDIT_WORD_LIST_DLG_H

#include "../../Wisteria-Dataviz/src/import/text_matrix.h"
#include "../../Wisteria-Dataviz/src/import/text_preview.h"
#include "../../Wisteria-Dataviz/src/ui/controls/listctrlex.h"
#include "../../Wisteria-Dataviz/src/ui/dialogs/dialogwithhelp.h"
#include "../../Wisteria-Dataviz/src/util/memorymappedfile.h"
#include "../../indexing/character_traits.h"
#include <algorithm>
#include <string>
#include <vector>
#include <wx/combobox.h>
#include <wx/intl.h>
#include <wx/wx.h>

/// @brief A dialog to edit a word (or phrase) list.
class EditWordListDlg final : public Wisteria::UI::DialogWithHelp
    {
    EditWordListDlg() = default;

  public:
    /// @brief The string type that they dialog storing data as.
    using OutputStringType = traits::case_insensitive_wstring_ex;

    /// @brief Constructor.
    /// @param parent The parent window.
    /// @param id The dialog's ID.
    /// @param caption The dialog's caption.
    /// @param pos The dialog's position.
    /// @param size The dialog's size.
    /// @param style The dialog's style.
    explicit EditWordListDlg(wxWindow* parent, wxWindowID id = wxID_ANY,
                             const wxString& caption = _(L"Edit List"),
                             const wxPoint& pos = wxDefaultPosition,
                             const wxSize& size = wxSize(1000, 1000),
                             long style = wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN |
                                          wxRESIZE_BORDER);

    /// @brief Sets the path to the file that is being edited.
    /// @param path The path of the file.
    void SetFilePath(const wxString& path)
        {
        m_wordListFilePath = path;
        TransferDataToWindow();
        }

    /// @brief Sets The default directory to search in.
    /// @param path The default directory.
    void SetDefaultDir(const wxString& path) { m_defaultDir = path; }

    /// @returns The path to the file that is being edited.
    [[nodiscard]]
    const wxString& GetFilePath() const noexcept
        {
        return m_wordListFilePath;
        }

    /// @brief Sets whether to parse and edit the lines of text as
    ///     full-lines (phrases) or tokenize everything into words.
    /// @param isPhraseFileMode @c true to import as lines of text.
    void SetPhraseFileMode(const bool isPhraseFileMode) noexcept
        {
        m_phraseFileMode = isPhraseFileMode;
        }

    /// @brief Saves the list.
    /// @param filePath The filepath to save to.
    /// @returns @c true if successful.
    bool Save(const wxString& filePath);
    /// @private
    constexpr static int ID_FILE_PATH_FIELD = wxID_HIGHEST;

  private:
    constexpr static int ID_BROWSE_FOR_FILE = wxID_HIGHEST + 1;
    constexpr static int ID_ADD_ITEM = wxID_HIGHEST + 2;
    constexpr static int ID_EDIT_ITEM = wxID_HIGHEST + 3;
    constexpr static int ID_DELETE_ITEM = wxID_HIGHEST + 4;
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY,
                const wxString& caption = _(L"Edit List"), const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxSize(600, 1000),
                long style = wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN | wxRESIZE_BORDER);
    void CreateControls();

    // user is selecting a different word list file
    void OnBrowseForFileClick([[maybe_unused]] wxCommandEvent& event);
    void OnFilePathChanged(wxCommandEvent& event);
    void OnOK([[maybe_unused]] wxCommandEvent& event);
    void OnClose([[maybe_unused]] wxCommandEvent& event);
    void OnAddItem([[maybe_unused]] wxCommandEvent& event);
    void OnEditItem([[maybe_unused]] wxCommandEvent& event);
    void OnDeleteItem([[maybe_unused]] wxCommandEvent& event);

    std::shared_ptr<Wisteria::UI::ListCtrlExDataProvider> m_wordData{
        std::make_shared<Wisteria::UI::ListCtrlExDataProvider>()
    };
    Wisteria::UI::ListCtrlEx* m_wordsList{ nullptr };
    wxTextCtrl* m_wordListFilePathCtrl{ nullptr };
    wxString m_wordListFilePath;
    wxString m_previousListFilePath;
    wxString m_defaultDir;
    bool m_phraseFileMode{ false };
    };

#endif // EDIT_WORD_LIST_DLG_H
