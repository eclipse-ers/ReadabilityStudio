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

#ifndef ABOUT_DIALOG_EX_H
#define ABOUT_DIALOG_EX_H

#include "../../Wisteria-Dataviz/src/ui/controls/listctrlex.h"
#include "../../Wisteria-Dataviz/src/ui/controls/sidebarbook.h"
#include "../../Wisteria-Dataviz/src/ui/dialogs/dialogwithhelp.h"
#include <wx/bitmap.h>
#include <wx/dialog.h>
#include <wx/hyperlink.h>
#include <wx/image.h>
#include <wx/textctrl.h>
#include <wx/utils.h>
#include <wx/wx.h>
#include <wx/wxhtml.h>

/// @brief Enhanced About dialog for an application.
class AboutDialogEx final : public Wisteria::UI::DialogWithHelp
    {
  public:
    /** @brief Constructor.
        @param parent The dialog's parent.
        @param appVersion The application's version.
        @param copyright The copyright string.
        @param eula The end-user license agreement content.
        @param mlaCitation The program's citation in MLA format.
        @param apaCitation The program's citation in APA format.
        @param bibtexCitation The program's citation in BibTeX format.
        @param id The dialog's ID.
        @param pos The dialog's position.
        @param size The dialog's size.
        @param style The dialog's style.*/
    AboutDialogEx(wxWindow* parent, wxString appVersion, wxString copyright, wxString eula,
                  wxString mlaCitation, wxString apaCitation, wxString bibtexCitation,
                  wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                  const wxSize& size = wxDefaultSize,
                  long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    /// @private
    AboutDialogEx(const AboutDialogEx& that) = delete;
    /// @private
    AboutDialogEx& operator=(const AboutDialogEx& that) = delete;

    /// @returns The sidebar.
    [[nodiscard]]
    Wisteria::UI::SideBar* GetSideBar() noexcept
        {
        return m_sideBarBook->GetSideBar();
        }

  private:
    // page and button IDs
    constexpr static int ID_VERSION_PAGE = wxID_HIGHEST;
    constexpr static int ID_LICENSING_PAGE = wxID_HIGHEST + 1;
    constexpr static int ID_CITATION = wxID_HIGHEST + 2;
    constexpr static int ID_COPYMLA = wxID_HIGHEST + 3;
    constexpr static int ID_COPYAPA = wxID_HIGHEST + 4;
    constexpr static int ID_COPYBIBTEX = wxID_HIGHEST + 5;
    constexpr static int ID_COMPONENTS = wxID_HIGHEST + 6;
    constexpr static int ID_COPY_COMPONENTS = wxID_HIGHEST + 7;
    constexpr static int ID_COPY_LICENSE = wxID_HIGHEST + 8;
    constexpr static int ID_COPY_PRODUCT_INFO = wxID_HIGHEST + 9;

    /// Creation.
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

    /// Creates the controls and sizers.
    void CreateControls();

    Wisteria::UI::SideBarBook* m_sideBarBook{ nullptr };

    wxString m_appVersion;
    wxString m_copyright;
    wxString m_eula;
    wxString m_components;
    wxString m_mlaCitation;
    wxString m_apaCitation;
    wxString m_bibtexCitation;
    wxString m_productInfo;
    };

#endif // ABOUT_DIALOG_EX_H
