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

#ifndef WORD_LIST_DIALOG_H
#define WORD_LIST_DIALOG_H

#include "../../Wisteria-Dataviz/src/ui/controls/listctrlex.h"
#include "../../Wisteria-Dataviz/src/ui/controls/sidebarbook.h"
#include "../../indexing/word_list.h"
#include <wx/ribbon/buttonbar.h>
#include <wx/ribbon/gallery.h>

/// @brief Dialog to display multiple word lists (read-only viewing).
class WordListDlg final : public wxDialog
    {
  public:
    /** @brief Constructor.
        @param parent The dialog's parent.
        @param id The dialog's ID.
        @param caption The dialog's caption.
        @param pos The dialog's position.
        @param size The dialog's size.
        @param style The dialog's style.*/
    explicit WordListDlg(wxWindow* parent, wxWindowID id = wxID_ANY,
                         const wxString& caption = _(L"Word Lists"),
                         const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                         long style = wxCAPTION | wxCLOSE_BOX | wxRESIZE_BORDER);

    /// @private
    WordListDlg(const WordListDlg&) = delete;
    /// @private
    WordListDlg& operator=(const WordListDlg&) = delete;

    /// @brief Selects a word list page on the dialog.
    /// @param pageId The page (by ID) to select.
    void SelectPage(const int pageId);
    /// @brief The Dale-Chall page.
    constexpr static int DALE_CHALL_PAGE_ID = wxID_HIGHEST;
    /// @brief Stocker Catholic page.
    constexpr static int STOCKER_PAGE_ID = wxID_HIGHEST + 1;
    /// @brief The Spache page.
    constexpr static int SPACHE_PAGE_ID = wxID_HIGHEST + 2;
    /// @brief The Harris-Jacobson page.
    constexpr static int HARRIS_JACOBSON_PAGE_ID = wxID_HIGHEST + 3;
    /// @brief The Dolch page.
    constexpr static int DOLCH_PAGE_ID = wxID_HIGHEST + 4;

  private:
    void CreateControls();
    [[nodiscard]]
    Wisteria::UI::ListCtrlEx* GetActiveList();
    void OnFind(wxFindDialogEvent& event);
    void OnRibbonButton(wxRibbonButtonBarEvent& event);
    void OnNegative(wxCommandEvent& event);
    void OnClose([[maybe_unused]] wxCloseEvent& event);
    void AddSingleColumnPage(Wisteria::UI::SideBarBook* sideBar, const int id, const int listId,
                             const wxString& label, const int imageId,
                             const std::shared_ptr<Wisteria::UI::ListCtrlExDataProvider>& data,
                             const word_list& wordList);

    constexpr static int DALE_CHALL_LIST_ID = wxID_HIGHEST + 5;
    constexpr static int STOCKER_LIST_ID = wxID_HIGHEST + 6;
    constexpr static int SPACHE_LIST_ID = wxID_HIGHEST + 7;
    constexpr static int HARRIS_JACOBSON_LIST_ID = wxID_HIGHEST + 8;
    constexpr static int DOLCH_LIST_ID = wxID_HIGHEST + 9;

    std::shared_ptr<Wisteria::UI::ListCtrlExDataProvider> m_DCData{
        std::make_shared<Wisteria::UI::ListCtrlExDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExDataProvider> m_StockerData{
        std::make_shared<Wisteria::UI::ListCtrlExDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExDataProvider> m_SpacheData{
        std::make_shared<Wisteria::UI::ListCtrlExDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExDataProvider> m_HJData{
        std::make_shared<Wisteria::UI::ListCtrlExDataProvider>()
    };
    std::shared_ptr<Wisteria::UI::ListCtrlExDataProvider> m_DolchData{
        std::make_shared<Wisteria::UI::ListCtrlExDataProvider>()
    };
    Wisteria::UI::SideBarBook* m_sideBar{ nullptr };
    };

#endif // WORD_LIST_DIALOG_H
