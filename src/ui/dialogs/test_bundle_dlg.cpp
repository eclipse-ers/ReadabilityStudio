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

#include "test_bundle_dlg.h"
#include "../../Wisteria-Dataviz/src/import/html_extract_text.h"
#include "../../app/readability_app.h"
#include <wx/gbsizer.h>
#include <wx/valgen.h>

wxDECLARE_APP(ReadabilityApp);

//-------------------------------------------------------------
TestBundleDlg::TestBundleDlg(wxWindow* parent, TestBundle& testBundle)
    : m_bundleName(testBundle.GetName().c_str()),
      m_descriptionName(testBundle.GetDescription().c_str()), m_testBundle(testBundle)
    {
    DialogWithHelp::Create(
        parent, wxID_ANY,
        m_bundleName.empty()    ? _(L"Add Test Bundle") :
        m_testBundle.IsLocked() ? wxString::Format(_(L"\"%s\" Test Bundle [Read-Only]"),
                                                   m_bundleName) :
                                  wxString::Format(_(L"Edit \"%s\" Test Bundle"), m_bundleName),
        wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    wxIcon ico;
    ico.CopyFromBitmap(wxGetApp()
                           .GetResourceManager()
                           .GetSVG(L"ribbon/bundles.svg")
                           .GetBitmap(FromDIP(wxSize{ 16, 16 })));
    SetIcon(ico);

    Bind(
        wxEVT_BUTTON,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            SetFocusIgnoringChildren();
            if (IsModal())
                {
                EndModal(wxID_CANCEL);
                }
            else
                {
                Show(false);
                }
        },
        wxID_CANCEL);

    Bind(wxEVT_BUTTON, &TestBundleDlg::OnOK, this, wxID_OK);

    CreateControls();
    Centre();
    }

//-------------------------------------------------------------
void TestBundleDlg::CreateControls()
    {
    auto* mainSizer = new wxBoxSizer(wxVERTICAL);

    m_sideBarBook = new Wisteria::UI::SideBarBook(this, wxID_ANY);

    mainSizer->Add(m_sideBarBook, wxSizerFlags{ 1 }.Expand().Border());

    wxArrayString availableTestNames;

        // General page
        {
        auto* page = new wxPanel(m_sideBarBook, ID_GENERAL_PAGE, wxDefaultPosition, wxDefaultSize,
                                 wxTAB_TRAVERSAL);
        auto* mainPanelSizer = new wxBoxSizer(wxVERTICAL);
        page->SetSizer(mainPanelSizer);
        m_sideBarBook->AddPage(page, _(L"General"), ID_GENERAL_PAGE, true);

        // if no bundle name then we are in "add new bundle" mode
        if (m_bundleName.empty())
            {
            auto* nameBoxSizer =
                new wxStaticBoxSizer(new wxStaticBox(page, wxID_ANY, _(L"Name:")), wxVERTICAL);

            m_bundleNameCtrl = new wxTextCtrl(nameBoxSizer->GetStaticBox(), wxID_ANY, wxString{},
                                              wxDefaultPosition, wxDefaultSize, 0,
                                              wxTextValidator(wxFILTER_NONE, &m_bundleName));
            nameBoxSizer->Add(m_bundleNameCtrl, wxSizerFlags{ 1 }.Expand().Border());

            mainPanelSizer->Add(nameBoxSizer, wxSizerFlags{}.Expand().Border());
            }

            // description
            {
            auto* descriptionBoxSizer = new wxStaticBoxSizer(
                new wxStaticBox(page, wxID_ANY, _(L"Description:")), wxVERTICAL);
            const long style =
                m_testBundle.IsLocked() ? wxTE_MULTILINE | wxTE_READONLY : wxTE_MULTILINE;
            m_bundleDescriptionCtrl = new wxTextCtrl(
                descriptionBoxSizer->GetStaticBox(), wxID_ANY, wxString{}, wxDefaultPosition,
                wxDefaultSize, style, wxTextValidator(wxFILTER_NONE, &m_descriptionName));
            descriptionBoxSizer->Add(m_bundleDescriptionCtrl, wxSizerFlags{ 1 }.Expand().Border());

            mainPanelSizer->Add(descriptionBoxSizer, wxSizerFlags{ 1 }.Expand().Border());
            }

        mainPanelSizer->Add(
            new wxStaticText(page, wxID_STATIC,
                             _(L"Bundles are a convenient way to apply a predefined list "
                               "of tests and goals to a project.")),
            wxSizerFlags{}.Border());
        mainPanelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
        }

    constexpr size_t MAX_COLUMNS{ 3 };

        // standard tests page
        {
        auto* page = new wxPanel(m_sideBarBook, ID_STANDARD_TEST_PAGE, wxDefaultPosition,
                                 wxDefaultSize, wxTAB_TRAVERSAL);
        auto* mainPanelSizer = new wxBoxSizer(wxVERTICAL);
        page->SetSizer(mainPanelSizer);
        m_sideBarBook->AddPage(page, _(L"Standard Tests"), ID_STANDARD_TEST_PAGE, false);

        m_standardTests = wxGetApp().GetAppOptions()->GetReadabilityTests();

        lily_of_the_valley::html_extract_text stripHtml;
        auto* readabilityStandardTestSizer = new wxGridBagSizer(
            wxSizerFlags::GetDefaultBorder(), wxSizerFlags::GetDefaultBorder() / 2);
        mainPanelSizer->Add(readabilityStandardTestSizer, wxSizerFlags{}.Expand().Border());
        // add the standard tests
        size_t currentRow{ 0 }, currentCol{ 0 };
        const size_t rowCount = (m_standardTests.get_tests().size() / MAX_COLUMNS) + 1;
        for (auto& sTest : m_standardTests.get_tests())
            {
            // if set to a particular language, then only include tests with that language
            if (m_testBundle.GetLanguage() != readability::test_language::unknown_language &&
                !sTest.get_test().has_language(m_testBundle.GetLanguage()))
                {
                continue;
                }

            // used in the Goals grid below
            availableTestNames.push_back(sTest.get_test().get_long_name().c_str());

            // set the test to included (i.e., checked) if it is already included in the bundle
            sTest.include(
                m_testBundle.GetTestGoals().contains(TestGoal{ sTest.get_test().get_id() }) ||
                m_testBundle.GetTestGoals().contains(
                    TestGoal{ sTest.get_test().get_short_name() }) ||
                m_testBundle.GetTestGoals().contains(TestGoal{ sTest.get_test().get_long_name() }));

            auto* testCheckBox = new wxCheckBox(
                page, wxID_ANY, sTest.get_test().get_long_name().c_str(), wxDefaultPosition,
                wxDefaultSize, 0, wxGenericValidator(&sTest.get_include_flag()));
            testCheckBox->SetHelpText(stripHtml(sTest.get_test().get_description().c_str(),
                                                sTest.get_test().get_description().length(), true,
                                                false));
            if (m_testBundle.IsLocked())
                {
                testCheckBox->Disable();
                }
            readabilityStandardTestSizer->Add(testCheckBox, wxGBPosition(currentRow++, currentCol));
            // if at the end of the rows, then move to the next column
            if (currentRow >= rowCount)
                {
                currentRow = 0;
                ++currentCol;
                }
            }
        }

    // custom tests
    const auto includedCustomTests =
        std::count_if(m_custom_tests.cbegin(), m_custom_tests.cend(),
                      [](const auto& test) noexcept { return test.is_included(); });
    if (!BaseProject::m_custom_word_tests.empty() &&
        // only show if editable or if there are custom tests
        (!m_testBundle.IsLocked() || includedCustomTests > 0))
        {
        auto* page = new wxPanel(m_sideBarBook, ID_CUSTOM_TEST_PAGE, wxDefaultPosition,
                                 wxDefaultSize, wxTAB_TRAVERSAL);
        auto* mainPanelSizer = new wxBoxSizer(wxVERTICAL);
        page->SetSizer(mainPanelSizer);
        m_sideBarBook->AddPage(page, _(L"Custom Tests"), ID_CUSTOM_TEST_PAGE, false);

        m_custom_tests = BaseProject::m_custom_word_tests;

        auto* readabilityCustomTestSizer = new wxGridBagSizer(wxSizerFlags::GetDefaultBorder(),
                                                              wxSizerFlags::GetDefaultBorder() / 2);
        mainPanelSizer->Add(readabilityCustomTestSizer, wxSizerFlags{}.Expand().Border());

        size_t currentRow{ 0 }, currentCol{ 0 };
        const size_t rowCount = (BaseProject::m_custom_word_tests.size() > 10) ?
                                    (BaseProject::m_custom_word_tests.size() / MAX_COLUMNS) + 1 :
                                    BaseProject::m_custom_word_tests.size();
        for (auto& cTest : m_custom_tests)
            {
            // used in the Goals grid below
            availableTestNames.push_back(cTest.get_name().c_str());

            // go through the list of custom tests and see if any of them are in the bundle
            cTest.include(m_testBundle.GetTestGoals().contains(TestGoal{ cTest.get_name() }));
            // add the test to the dialog
            auto* testCheckBox =
                new wxCheckBox(page, wxID_ANY, cTest.get_name().c_str(), wxDefaultPosition,
                               wxDefaultSize, 0, wxGenericValidator(&cTest.get_include_flag()));
            if (m_testBundle.IsLocked())
                {
                testCheckBox->Disable();
                }
            readabilityCustomTestSizer->Add(testCheckBox, wxGBPosition(currentRow++, currentCol));
            // if at the end of the rows, then move to the next column
            if (currentRow >= rowCount)
                {
                currentRow = 0;
                ++currentCol;
                }
            }
        }

    // Vocabulary suites
    // Dolch
    m_includeDolchSightWords =
        (m_testBundle.GetTestGoals().contains(TestGoal{ ReadabilityMessages::DOLCH().wc_str() }));
    if ((m_testBundle.GetLanguage() == readability::test_language::unknown_language ||
         m_testBundle.GetLanguage() == readability::test_language::english_test) &&
        // only show if editable or Dolch is included
        (!m_testBundle.IsLocked() || m_includeDolchSightWords))
        {
        auto* page = new wxPanel(m_sideBarBook, ID_VOCAB_PAGE, wxDefaultPosition, wxDefaultSize,
                                 wxTAB_TRAVERSAL);
        auto* mainPanelSizer = new wxBoxSizer(wxVERTICAL);
        page->SetSizer(mainPanelSizer);
        m_sideBarBook->AddPage(page, _(L"Vocabulary Tools"), ID_VOCAB_PAGE, false);

        auto* dolchCheckBox =
            new wxCheckBox(page, wxID_ANY, _(L"&Dolch Sight Words Suite"), wxDefaultPosition,
                           wxDefaultSize, 0, wxGenericValidator(&m_includeDolchSightWords));
        dolchCheckBox->SetHelpText(
            _(L"Select this option to include a Dolch Sight Words section to your project. "
              "This is recommended for writers and educators whose documents need to demonstrate "
              "functional words for early readers to learn."));
        if (m_testBundle.IsLocked())
            {
            dolchCheckBox->Disable();
            }
        mainPanelSizer->Add(dolchCheckBox, wxSizerFlags{}.Expand().Border());
        }

    // goals (only show if editable or there are goals)
    const auto includedTestGoals =
        std::count_if(m_testBundle.GetTestGoals().cbegin(), m_testBundle.GetTestGoals().cend(),
                      [](const auto& test) noexcept { return test.HasGoals(); });
    if (!m_testBundle.IsLocked() ||
        ((includedTestGoals != 0) || !m_testBundle.GetStatGoals().empty()))
        {
        auto* page = new wxPanel(m_sideBarBook, ID_GOALS_PAGE, wxDefaultPosition, wxDefaultSize,
                                 wxTAB_TRAVERSAL);
        auto* mainPanelSizer = new wxBoxSizer(wxVERTICAL);
        page->SetSizer(mainPanelSizer);
        m_sideBarBook->AddPage(page, _(L"Goals"), ID_GOALS_PAGE, false);

            // Test goals
            {
            if (!m_testBundle.IsLocked())
                {
                // add and remove buttons for grid
                auto* editButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
                auto* addButton =
                    new wxBitmapButton(page, ID_ADD_TEST_GOALS_BUTTON,
                                       wxArtProvider::GetBitmapBundle(L"ID_ADD", wxART_BUTTON));
                addButton->SetToolTip(_(L"Add a test goal"));
                editButtonsSizer->Add(addButton);

                Bind(
                    wxEVT_BUTTON, [this](wxCommandEvent&)
                    { m_testGoalsListCtrl->EditItem(m_testGoalsListCtrl->AddRow(), 0); },
                    ID_ADD_TEST_GOALS_BUTTON);

                auto* deleteButton =
                    new wxBitmapButton(page, ID_DELETE_TEST_GOALS_BUTTON,
                                       wxArtProvider::GetBitmapBundle(wxART_DELETE, wxART_BUTTON));
                deleteButton->SetToolTip(_(L"Remove selected test goals"));
                editButtonsSizer->Add(deleteButton);

                Bind(
                    wxEVT_BUTTON, [this](wxCommandEvent&)
                    { m_testGoalsListCtrl->DeleteSelectedItems(); }, ID_DELETE_TEST_GOALS_BUTTON);

                mainPanelSizer->Add(editButtonsSizer,
                                    wxSizerFlags{}.Right().Border(wxTOP | wxRIGHT));
                }

            // prepare the control
            availableTestNames.Sort();
            m_testGoalsListCtrl = new Wisteria::UI::ListCtrlEx(
                page, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxBORDER_SUNKEN);
            m_testGoalsListCtrl->InsertColumn(0, _(L"Test"));
            m_testGoalsListCtrl->InsertColumn(1, _(L"Minimum Recommended Score"));
            m_testGoalsListCtrl->InsertColumn(2, _(L"Maximum Recommended Score"));
            m_testGoalsListCtrl->EnableItemAdd();
            m_testGoalsListCtrl->EnableItemDeletion();
            m_testGoalsListCtrl->EnableLabelEditing();
            m_testGoalsListCtrl->SetSortable(true);
            m_testGoalsListCtrl->SetColumnTextSelectionsReadOnly(0, availableTestNames);
            m_testGoalsListCtrl->SetColumnNumericRange<double>(1, 0.0, 100.0);
            m_testGoalsListCtrl->SetColumnNumericRange<double>(2, 0.0, 100.0);
            m_testGoalsListCtrl->Enable(!m_testBundle.IsLocked());
            for (const auto& bundleTest : m_testBundle.GetTestGoals())
                {
                if (!std::isnan(bundleTest.GetMinGoal()) || !std::isnan(bundleTest.GetMaxGoal()))
                    {
                    // if a standard test
                    const auto [sTest, found] =
                        m_standardTests.find_test(bundleTest.GetName().c_str());
                    if (found)
                        {
                        const auto insertedItem = m_testGoalsListCtrl->InsertItem(
                            0, sTest->get_test().get_long_name().c_str());
                        if (!std::isnan(bundleTest.GetMinGoal()))
                            {
                            m_testGoalsListCtrl->SetItemText(
                                insertedItem, 1,
                                wxNumberFormatter::ToString(
                                    bundleTest.GetMinGoal(), 1,
                                    wxNumberFormatter::Style::Style_NoTrailingZeroes));
                            }
                        if (!std::isnan(bundleTest.GetMaxGoal()))
                            {
                            m_testGoalsListCtrl->SetItemText(
                                insertedItem, 2,
                                wxNumberFormatter::ToString(
                                    bundleTest.GetMaxGoal(), 1,
                                    wxNumberFormatter::Style::Style_NoTrailingZeroes));
                            }
                        }
                    // or a custom one
                    const auto foundCustomTest =
                        std::find(m_custom_tests.cbegin(), m_custom_tests.cend(),
                                  bundleTest.GetName().c_str());
                    if (foundCustomTest != m_custom_tests.cend())
                        {
                        const auto insertedItem =
                            m_testGoalsListCtrl->InsertItem(0, foundCustomTest->get_name().c_str());
                        if (!std::isnan(bundleTest.GetMinGoal()))
                            {
                            m_testGoalsListCtrl->SetItemText(
                                insertedItem, 1,
                                wxNumberFormatter::ToString(
                                    bundleTest.GetMinGoal(), 1,
                                    wxNumberFormatter::Style::Style_NoTrailingZeroes));
                            }
                        if (!std::isnan(bundleTest.GetMaxGoal()))
                            {
                            m_testGoalsListCtrl->SetItemText(
                                insertedItem, 2,
                                wxNumberFormatter::ToString(
                                    bundleTest.GetMaxGoal(), 1,
                                    wxNumberFormatter::Style::Style_NoTrailingZeroes));
                            }
                        }
                    }
                }
            m_testGoalsListCtrl->DistributeColumns();
            m_testGoalsListCtrl->SetColumnWidth(0, FromDIP(wxSize{ 200, 200 }.GetWidth()));
            mainPanelSizer->Add(m_testGoalsListCtrl,
                                wxSizerFlags{ 1 }.Expand().Border(wxLEFT | wxRIGHT | wxBOTTOM));
            }

            // Stat goals
            {
            if (!m_testBundle.IsLocked())
                {
                // add and remove buttons for grid
                auto* editButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
                auto* addButton =
                    new wxBitmapButton(page, ID_ADD_STAT_GOALS_BUTTON,
                                       wxArtProvider::GetBitmapBundle(L"ID_ADD", wxART_BUTTON));
                addButton->SetToolTip(_(L"Add a statistic goal"));
                editButtonsSizer->Add(addButton);

                Bind(
                    wxEVT_BUTTON, [this](wxCommandEvent&)
                    { m_statGoalsListCtrl->EditItem(m_statGoalsListCtrl->AddRow(), 0); },
                    ID_ADD_STAT_GOALS_BUTTON);

                auto* deleteButton =
                    new wxBitmapButton(page, ID_DELETE_STAT_GOALS_BUTTON,
                                       wxArtProvider::GetBitmapBundle(wxART_DELETE, wxART_BUTTON));
                deleteButton->SetToolTip(_(L"Remove selected statistic goals"));
                editButtonsSizer->Add(deleteButton);

                Bind(
                    wxEVT_BUTTON, [this](wxCommandEvent&)
                    { m_statGoalsListCtrl->DeleteSelectedItems(); }, ID_DELETE_STAT_GOALS_BUTTON);

                mainPanelSizer->Add(editButtonsSizer,
                                    wxSizerFlags{}.Right().Border(wxTOP | wxRIGHT));
                }

            // prepare the control
            wxArrayString availableStats;
            for (const auto& statName : BaseProject::GetStatGoalLabels())
                {
                availableStats.push_back(statName.first.second.c_str());
                }
            availableStats.Sort();
            m_statGoalsListCtrl = new Wisteria::UI::ListCtrlEx(
                page, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxBORDER_SUNKEN);
            m_statGoalsListCtrl->InsertColumn(0, _(L"Statistic"));
            m_statGoalsListCtrl->InsertColumn(1, _(L"Minimum Recommended Value"));
            m_statGoalsListCtrl->InsertColumn(2, _(L"Maximum Recommended Value"));
            m_statGoalsListCtrl->EnableItemAdd();
            m_statGoalsListCtrl->EnableItemDeletion();
            m_statGoalsListCtrl->EnableLabelEditing();
            m_statGoalsListCtrl->SetSortable(true);
            m_statGoalsListCtrl->SetColumnTextSelectionsReadOnly(0, availableStats);
            // statistics can be just about anything, so use a huge range
            m_statGoalsListCtrl->SetColumnNumericRange<double>(1, 0.0, 1'000'000.0);
            m_statGoalsListCtrl->SetColumnNumericRange<double>(2, 0.0, 1'000'000.0);
            m_statGoalsListCtrl->Enable(!m_testBundle.IsLocked());
            for (const auto& bundleStat : m_testBundle.GetStatGoals())
                {
                if (!std::isnan(bundleStat.GetMinGoal()) || !std::isnan(bundleStat.GetMaxGoal()))
                    {
                    // get the user-friendly label for stat goal and add it to the list
                    const auto statPos = BaseProject::GetStatGoalLabels().find(
                        { bundleStat.GetName(), bundleStat.GetName() });
                    if (statPos != BaseProject::GetStatGoalLabels().cend())
                        {
                        const auto insertedItem =
                            m_statGoalsListCtrl->InsertItem(0, statPos->first.second.c_str());
                        if (!std::isnan(bundleStat.GetMinGoal()))
                            {
                            m_statGoalsListCtrl->SetItemText(
                                insertedItem, 1,
                                wxNumberFormatter::ToString(
                                    bundleStat.GetMinGoal(), 1,
                                    wxNumberFormatter::Style::Style_NoTrailingZeroes));
                            }
                        if (!std::isnan(bundleStat.GetMaxGoal()))
                            {
                            m_statGoalsListCtrl->SetItemText(
                                insertedItem, 2,
                                wxNumberFormatter::ToString(
                                    bundleStat.GetMaxGoal(), 1,
                                    wxNumberFormatter::Style::Style_NoTrailingZeroes));
                            }
                        }
                    }
                }
            m_statGoalsListCtrl->DistributeColumns();
            m_statGoalsListCtrl->SetColumnWidth(0, FromDIP(wxSize{ 200, 200 }.GetWidth()));
            mainPanelSizer->Add(m_statGoalsListCtrl,
                                wxSizerFlags{ 1 }.Expand().Border(wxLEFT | wxRIGHT | wxBOTTOM));
            }

        mainPanelSizer->Add(
            new wxStaticText(page, wxID_STATIC,
                             _(L"Goals are a way to warn if a statistic or test's score falls "
                               "outside of a given range.\nDouble click in the grid to select a "
                               "test and edit its constraints.")),
            wxSizerFlags{}.Border());
        mainPanelSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
        }

    mainSizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL | wxHELP),
                   wxSizerFlags{}.Expand().Border());

    SetSizerAndFit(mainSizer);

    if (m_bundleNameCtrl != nullptr)
        {
        m_bundleNameCtrl->SetFocus();
        }
    }

//-------------------------------------------------------------
void TestBundleDlg::SelectPage(const wxWindowID pageId)
    {
    for (size_t i = 0; i < m_sideBarBook->GetPageCount(); ++i)
        {
        const wxWindow* page = m_sideBarBook->GetPage(i);
        if ((page != nullptr) && page->GetId() == pageId)
            {
            m_sideBarBook->SetSelection(i);
            break;
            }
        }
    }

//-------------------------------------------------------------
void TestBundleDlg::OnOK([[maybe_unused]] wxCommandEvent& event)
    {
    SetFocusIgnoringChildren();

    TransferDataFromWindow();
    // trim off whitespace off of bundle name and then validate it (if applicable)
    m_bundleName.Trim(true).Trim(false);
    m_descriptionName.Trim(true).Trim(false);

    // validate the name
    if (m_bundleName.empty())
        {
        wxMessageBox(_(L"Please enter a bundle name."), _(L"Error"), wxOK | wxICON_EXCLAMATION);
        return;
        }
    // or that the name is already taken
    if (m_bundleNameCtrl != nullptr)
        {
        if (BaseProject::m_testBundles.contains(TestBundle(m_bundleName.wc_str())))
            {
            wxMessageBox(
                wxString::Format(
                    _(L"There is a bundle named \"%s\" already. Please enter a different name."),
                    m_bundleName),
                _(L"Error"), wxOK | wxICON_ERROR);
            return;
            }
        }

    // reads and verifies the goals from a test in the grid, returns false if user needs to fix them
    auto readTestGoals = [this](const wxString& testName, double& minGoal, double& maxGoal)
    {
        auto foundPos = m_testGoalsListCtrl->FindEx(testName);
        if (foundPos != wxNOT_FOUND)
            {
            auto minGoalStr = m_testGoalsListCtrl->GetItemText(foundPos, 1);
            if (!minGoalStr.empty())
                {
                minGoalStr.ToDouble(&minGoal);
                }
            auto maxGoalStr = m_testGoalsListCtrl->GetItemText(foundPos, 2);
            if (!maxGoalStr.empty())
                {
                maxGoalStr.ToDouble(&maxGoal);
                }
            }
        // make sure the goal is sensical
        if (!std::isnan(minGoal) && !std::isnan(maxGoal) && maxGoal < minGoal)
            {
            wxMessageBox(
                wxString::Format(_(L"\"%s\" has an invalid goal; "
                                   "minimum recommended score cannot be higher than the maximum."),
                                 testName),
                _(L"Goal Error"), wxOK | wxICON_ERROR);
            m_testGoalsListCtrl->DeselectAll();
            m_testGoalsListCtrl->Select(foundPos);
            SelectPage(ID_GOALS_PAGE);
            return false;
            }
        return true;
    };

    // reads and verifies the goals from a stats in the grid,
    // returns false if user needs to fix them
    auto readStatGoals = [this](const wxString& statName, double& minGoal, double& maxGoal)
    {
        auto foundPos = m_statGoalsListCtrl->FindEx(statName);
        if (foundPos != wxNOT_FOUND)
            {
            auto minGoalStr = m_statGoalsListCtrl->GetItemText(foundPos, 1);
            if (!minGoalStr.empty())
                {
                minGoalStr.ToDouble(&minGoal);
                }
            auto maxGoalStr = m_statGoalsListCtrl->GetItemText(foundPos, 2);
            if (!maxGoalStr.empty())
                {
                maxGoalStr.ToDouble(&maxGoal);
                }
            }
        // make sure the goal is sensical
        if (!std::isnan(minGoal) && !std::isnan(maxGoal) && maxGoal < minGoal)
            {
            wxMessageBox(
                wxString::Format(_(L"\"%s\" has an invalid goal; "
                                   "minimum recommended value cannot be higher than the maximum."),
                                 statName),
                _(L"Goal Error"), wxOK | wxICON_ERROR);
            m_statGoalsListCtrl->DeselectAll();
            m_statGoalsListCtrl->Select(foundPos);
            SelectPage(ID_GOALS_PAGE);
            return false;
            }
        return true;
    };

    // initial review of test goals
    for (auto i = 0; i < m_testGoalsListCtrl->GetItemCount(); ++i)
        {
        auto currentTest = m_testGoalsListCtrl->GetItemText(i, 0);
        // see if they forget to select a test name
        if (currentTest.empty())
            {
            wxMessageBox(wxString::Format(_(L"Goal does not have a test name specified.")),
                         _(L"Goal Error"), wxOK | wxICON_ERROR);
            m_testGoalsListCtrl->DeselectAll();
            m_testGoalsListCtrl->Select(i);
            SelectPage(ID_GOALS_PAGE);
            return;
            }
        // and actually has a goal specified
        if (m_testGoalsListCtrl->GetItemText(i, 1).empty() &&
            m_testGoalsListCtrl->GetItemText(i, 2).empty())
            {
            wxMessageBox(wxString::Format(_(L"\"%s\" does not have either a minimum or maximum "
                                            "recommended score specified."),
                                          currentTest.wc_str()),
                         _(L"Goal Error"), wxOK | wxICON_ERROR);
            m_testGoalsListCtrl->DeselectAll();
            m_testGoalsListCtrl->Select(i);
            SelectPage(ID_GOALS_PAGE);
            return;
            }
        // see if any goals where specified, but they forgot to include the test in the bundle
        // (might be either a standard or custom test).
        auto foundStandardTest = m_standardTests.find_test(currentTest.wc_str());
        if (foundStandardTest.second && !foundStandardTest.first->is_included())
            {
            if (wxMessageBox(
                    wxString::Format(
                        _(L"You have a goal for \"%s,\" but didn't include it in the bundle. "
                          "Do you wish to add this test to the bundle?"),
                        currentTest),
                    _(L"Test Not Included"), wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION) == wxYES)
                {
                foundStandardTest.first->include(true);
                }
            }
        auto foundCustomTest =
            std::find(m_custom_tests.begin(), m_custom_tests.end(), currentTest.wc_str());
        if (foundCustomTest != m_custom_tests.end() && !foundCustomTest->is_included())
            {
            if (wxMessageBox(
                    wxString::Format(
                        _(L"You have a goal for \"%s,\" but didn't include it in the bundle. "
                          "Do you wish to add this test to the bundle?"),
                        currentTest),
                    _(L"Test Not Included"), wxYES_NO | wxYES_DEFAULT | wxICON_QUESTION) == wxYES)
                {
                foundCustomTest->include(true);
                }
            }
        }

    // initial review of stat goals
    for (auto i = 0; i < m_statGoalsListCtrl->GetItemCount(); ++i)
        {
        auto currentStat = m_statGoalsListCtrl->GetItemText(i, 0);
        // see if they forget to select a test name
        if (currentStat.empty())
            {
            wxMessageBox(wxString::Format(_(L"Goal does not have a statistic specified.")),
                         _(L"Goal Error"), wxOK | wxICON_ERROR);
            m_statGoalsListCtrl->DeselectAll();
            m_statGoalsListCtrl->Select(i);
            SelectPage(ID_GOALS_PAGE);
            return;
            }
        // and actually has a goal specified
        if (m_statGoalsListCtrl->GetItemText(i, 1).empty() &&
            m_statGoalsListCtrl->GetItemText(i, 2).empty())
            {
            wxMessageBox(wxString::Format(_(L"\"%s\" does not have either a minimum or maximum "
                                            "recommended value specified."),
                                          currentStat.wc_str()),
                         _(L"Goal Error"), wxOK | wxICON_ERROR);
            m_statGoalsListCtrl->DeselectAll();
            m_statGoalsListCtrl->Select(i);
            SelectPage(ID_GOALS_PAGE);
            return;
            }
        }

    m_testBundle.SetName(m_bundleName.wc_str());
    m_testBundle.SetDescription(m_descriptionName.wc_str());

    // set the test goals, connecting them to the tests
    m_testBundle.GetTestGoals().clear();
    for (const auto& sTest : m_standardTests.get_tests())
        {
        if (sTest.is_included())
            {
            // connect any goals specified for the test to it
            double minGoal{ std::numeric_limits<double>::quiet_NaN() },
                maxGoal{ std::numeric_limits<double>::quiet_NaN() };
            if (!readTestGoals(sTest.get_test().get_long_name().c_str(), minGoal, maxGoal))
                {
                return;
                }
            m_testBundle.GetTestGoals().insert({ sTest.get_test().get_id(), minGoal, maxGoal });
            }
        }
    if (m_includeDolchSightWords)
        {
        m_testBundle.GetTestGoals().insert(TestGoal{ ReadabilityMessages::DOLCH().wc_str() });
        }
    for (const auto& cTest : m_custom_tests)
        {
        if (cTest.is_included())
            {
            // connect any goals specified for the test to it
            double minGoal{ std::numeric_limits<double>::quiet_NaN() },
                maxGoal{ std::numeric_limits<double>::quiet_NaN() };
            if (!readTestGoals(cTest.get_name().c_str(), minGoal, maxGoal))
                {
                return;
                }
            m_testBundle.GetTestGoals().insert({ cTest.get_name(), minGoal, maxGoal });
            }
        }

    // set the stat goals
    m_testBundle.GetStatGoals().clear();
    for (const auto& statLabel : BaseProject::GetStatGoalLabels())
        {
        double minGoal{ std::numeric_limits<double>::quiet_NaN() },
            maxGoal{ std::numeric_limits<double>::quiet_NaN() };
        if (!readStatGoals(statLabel.first.second.c_str(), minGoal, maxGoal))
            {
            return;
            }
        m_testBundle.GetStatGoals().insert({ statLabel.first.first, minGoal, maxGoal });
        }

    if (IsModal())
        {
        EndModal(wxID_OK);
        }
    else
        {
        Show(false);
        }
    }
