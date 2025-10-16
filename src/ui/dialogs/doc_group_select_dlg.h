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

#ifndef DOC_GROUP_DIALOG_H
#define DOC_GROUP_DIALOG_H

#include "../../Wisteria-Dataviz/src/ui/dialogs/dialogwithhelp.h"
#include <wx/dialog.h>
#include <wx/html/htmlwin.h>
#include <wx/wx.h>

/// @brief A dialog to choose what sort of label should be connected
///     to a document (or batch of documents).
class DocGroupSelectDlg final : public Wisteria::UI::DialogWithHelp
    {
  public:
    /** @brief Constructor.
        @param parent The parent window.
        @param id The dialog's ID.
        @param caption The dialog's caption.
        @param pos The dialog's window position.
        @param size The dialog's size.
        @param style The dialog's style.*/
    explicit DocGroupSelectDlg(wxWindow* parent, wxWindowID id = wxID_ANY,
                               const wxString& caption = _(L"Select Labeling"),
                               const wxPoint& pos = wxDefaultPosition,
                               const wxSize& size = wxDefaultSize,
                               long style = wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN |
                                            wxRESIZE_BORDER)
        {
        wxNonOwnedWindow::SetExtraStyle(GetExtraStyle() | wxWS_EX_BLOCK_EVENTS);
        DialogWithHelp::Create(parent, id, caption, pos, size, style);

        CreateControls();
        GetSizer()->SetSizeHints(this);
        Centre();
        }

    /// @private
    DocGroupSelectDlg() = delete;
    /// @private
    DocGroupSelectDlg(const DocGroupSelectDlg&) = delete;
    /// @private
    DocGroupSelectDlg& operator=(const DocGroupSelectDlg&) = delete;

    /// @returns The selected item.
    [[nodiscard]]
    int GetSelection() noexcept
        {
        TransferDataFromWindow();
        return m_selected;
        }

    /// @returns The grouping label.
    [[nodiscard]]
    const wxString& GetGroupingLabel() noexcept
        {
        TransferDataFromWindow();
        return m_groupingLabel;
        }

    /// @brief Sets the grouping label.
    /// @param label The label to use.
    void SetGroupingLabel(const wxString& label)
        {
        m_groupingLabel = label;
        TransferDataToWindow();
        }

    /** @brief Sets the selected item (i.e., radio button index).
        @param selected The radio button to select.*/
    void SetSelection(const int selected)
        {
        m_selected = std::clamp(selected, 0, 2);
        m_groupingLabelText->Enable(m_selected == 1);
        m_groupingLabelEntry->Enable(m_selected == 1);
        TransferDataToWindow();
        }

  private:
    void OnRadioBoxChange([[maybe_unused]] wxCommandEvent& event);
    void OnOK([[maybe_unused]] wxCommandEvent& event);

    /// Creates the controls and sizers.
    void CreateControls();

    wxString m_groupingLabel;
    int m_selected{ 0 };
    wxStaticText* m_groupingLabelText{ nullptr };
    wxTextCtrl* m_groupingLabelEntry{ nullptr };
    };

#endif // DOC_GROUP_DIALOG_H
