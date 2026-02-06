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

#ifndef TEST_BUNDLE_DLG_H
#define TEST_BUNDLE_DLG_H

#include "../../Wisteria-Dataviz/src/ui/controls/listctrlex.h"
#include "../../Wisteria-Dataviz/src/ui/controls/sidebarbook.h"
#include "../../Wisteria-Dataviz/src/ui/dialogs/dialogwithhelp.h"
#include "../../projects/base_project.h"
#include "../../readability/readability_test.h"
#include <wx/wx.h>

class TestBundle;
class ReadabilityApp;

/// @brief Test bundle editing dialog.
class TestBundleDlg final : public Wisteria::UI::DialogWithHelp
    {
    friend ReadabilityApp;

  public:
    /// @brief Constructor.
    /// @param parent The dialog's parent.
    /// @param testBundle The test bundle to edit.
    TestBundleDlg(wxWindow* parent, TestBundle& testBundle);
    /// @private
    TestBundleDlg() = delete;
    /// @private
    TestBundleDlg(const TestBundleDlg&) = delete;
    /// @private
    TestBundleDlg& operator=(const TestBundleDlg&) = delete;

    /// @brief Sets the bundle's name.
    /// @param name The bundle name to use.
    void SetTestBundleName(const wxString& name)
        {
        m_bundleName = name;
        TransferDataToWindow();
        }

    /// @brief Selects the page with the given page ID.
    /// @param pageId The window ID of the page to select.
    void SelectPage(const wxWindowID pageId);

  private:
    void CreateControls();
    void OnOK([[maybe_unused]] wxCommandEvent& event);

    // page IDs
    constexpr static int ID_GENERAL_PAGE = wxID_HIGHEST;
    constexpr static int ID_STANDARD_TEST_PAGE = wxID_HIGHEST + 1;
    constexpr static int ID_VOCAB_PAGE = wxID_HIGHEST + 2;
    constexpr static int ID_CUSTOM_TEST_PAGE = wxID_HIGHEST + 3;
    constexpr static int ID_GOALS_PAGE = wxID_HIGHEST + 4;
    constexpr static int ID_ADD_TEST_GOALS_BUTTON = wxID_HIGHEST + 5;
    constexpr static int ID_DELETE_TEST_GOALS_BUTTON = wxID_HIGHEST + 6;
    constexpr static int ID_ADD_STAT_GOALS_BUTTON = wxID_HIGHEST + 7;
    constexpr static int ID_DELETE_STAT_GOALS_BUTTON = wxID_HIGHEST + 8;

    wxString m_bundleName;
    wxString m_descriptionName;
    bool m_includeDolchSightWords{ false };
    TestBundle& m_testBundle;
    BaseProject::TestCollectionType m_standardTests;
    CustomReadabilityTestCollection m_custom_tests;
    wxTextCtrl* m_bundleNameCtrl{ nullptr };
    wxTextCtrl* m_bundleDescriptionCtrl{ nullptr };
    Wisteria::UI::ListCtrlEx* m_testGoalsListCtrl{ nullptr };
    Wisteria::UI::ListCtrlEx* m_statGoalsListCtrl{ nullptr };
    Wisteria::UI::SideBarBook* m_sideBarBook{ nullptr };
    };

#endif // TEST_BUNDLE_DLG_H
