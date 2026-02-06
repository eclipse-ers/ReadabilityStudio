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

#ifndef PROJECTWIZARD_H
#define PROJECTWIZARD_H

#include "../../Wisteria-Dataviz/src/ui/controls/listctrlex.h"
#include "../../Wisteria-Dataviz/src/ui/controls/sidebarbook.h"
#include "../../Wisteria-Dataviz/src/ui/dialogs/archivedlg.h"
#include "../../Wisteria-Dataviz/src/ui/dialogs/excelpreviewdlg.h"
#include "../../app/readability_app.h"
#include "../../projects/standard_project_doc.h"
#include "../../webharvester/webharvester.h"
#include "doc_group_select_dlg.h"
#include "web_harvester_dlg.h"

class ReadabilityApp;

enum class ProjectType
    {
    StandardProject,
    BatchProject
    };

/// @brief Wizard for creating standard and batch projects.
class ProjectWizardDlg final : public wxDialog
    {
    friend ReadabilityApp;

  public:
    /** @brief Constructor.
        @param parent The parent dialog.
        @param projectType The project type.
        @param path An optional default path for a document for folder to analyze.
        @param id The window ID.
        @param caption The dialog caption.
        @param pos The dialog position.
        @param size The dialog size.
        @param style The dialog style.
        @param fileTruncMode For a batch project wizard, how to display the file paths.*/
    ProjectWizardDlg(
        wxWindow* parent, const ProjectType projectType, const wxString& path = wxString{},
        wxWindowID id = wxID_ANY, const wxString& caption = _(L"New Project Wizard"),
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
        Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode fileTruncMode =
            Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode::NoTruncation);
    /// @private
    ProjectWizardDlg(const ProjectWizardDlg&) = delete;
    /// @private
    ProjectWizardDlg& operator=(const ProjectWizardDlg&) = delete;

    [[nodiscard]]
    readability::test_language GetLanguage() const;

    /// @returns The list control data containing files for a batch project.
    [[nodiscard]]
    const std::shared_ptr<Wisteria::UI::ListCtrlExDataProvider>& GetFileData() const
        {
        return m_fileData;
        }

    /// @returns @c true if random sampling is being used for the list
    ///     of files in a batch project.
    [[nodiscard]]
    bool IsRandomSampling() const
        {
        return m_isRandomSampling->IsChecked();
        }

    /// @returns The minimum number of words a document must have to be included in the batch.
    [[nodiscard]]
    int GetMinDocWordCountForBatch() const noexcept
        {
        return m_minDocWordCountForBatch;
        }

    [[nodiscard]]
    int GetRandomSamplePercentage() const
        {
        return m_randPercentageCtrl->GetValue();
        }

    [[nodiscard]]
    static wxString GetLastSelectedFolder()
        {
        return m_lastSelectedFolder;
        }

    static void SetLastSelectedFolder(const wxString& folder) { m_lastSelectedFolder = folder; }

    [[nodiscard]]
    Wisteria::UI::ListCtrlEx* GetFileList() noexcept
        {
        return m_fileList;
        }

    // document entry
    [[nodiscard]]
    bool IsTextFromFileSelected() const noexcept
        {
        return m_fromFileSelected;
        }

    [[nodiscard]]
    bool IsManualTextEntrySelected() const noexcept
        {
        return m_manualSelected;
        }

    void SetTextFromFileSelected()
        {
        TransferDataFromWindow();
        m_fromFileSelected = true;
        m_textEntryEdit->Disable();
        m_filePathEdit->Enable();
        m_fileBrowseButton->Enable();
        TransferDataToWindow();
        }

    void SetManualTextEntrySelected()
        {
        TransferDataFromWindow();
        m_manualSelected = true;
        m_textEntryEdit->Enable();
        m_filePathEdit->Disable();
        m_fileBrowseButton->Disable();
        TransferDataToWindow();
        }

    [[nodiscard]]
    wxString GetFilePath() const
        {
        return m_filePath;
        }

    void SetFilePath(const wxString& path)
        {
        m_filePath = path;
        TransferDataToWindow();
        }

    [[nodiscard]]
    wxString GetEnteredText() const
        {
        return m_enteredText;
        }

    void SetEnteredText(const wxString& text)
        {
        m_enteredText = text;
        TransferDataToWindow();
        }

    // document structure
    [[nodiscard]]
    bool IsNarrativeSelected() const noexcept
        {
        return m_narrativeSelected;
        }

    [[nodiscard]]
    bool IsFragmentedTextSelected() const noexcept
        {
        return m_fragmentedTextSelected;
        }

    [[nodiscard]]
    bool IsSplitLinesSelected() const noexcept
        {
        return m_splitLinesSelected;
        }

    [[nodiscard]]
    bool IsCenteredTextSelected() const noexcept
        {
        return m_centeredText;
        }

    [[nodiscard]]
    bool IsNewLinesAlwaysNewParagraphsSelected() const noexcept
        {
        return m_newLinesAlwaysNewParagraphs;
        }

    void SetNarrativeSelected() noexcept
        {
        m_narrativeSelected = true;
        m_fragmentedTextSelected = false;
        }

    void SetFragmentedTextSelected() noexcept
        {
        m_narrativeSelected = false;
        m_fragmentedTextSelected = true;
        }

    void SetSplitLinesSelected(const bool setVal) noexcept { m_splitLinesSelected = setVal; }

    void SetCenteredTextSelected(const bool setVal) noexcept { m_centeredText = setVal; }

    void SetNewLinesAlwaysNewParagraphsSelected(const bool setVal) noexcept
        {
        m_newLinesAlwaysNewParagraphs = setVal;
        }

    // test selection method
    void SetTestSelectionMethod(const int method)
        {
        m_testSelectionMethod = method;
        wxASSERT(is_within(
            std::make_pair(0, static_cast<int>(TestRecommendation::TEST_RECOMMENDATION_COUNT) - 1),
            m_testSelectionMethod));
        TransferDataToWindow();
        UpdateTestSelectionMethodUI();
        }

    [[nodiscard]]
    bool IsDocumentTypeSelected() const noexcept
        {
        return m_testSelectionMethod == 0;
        }

    [[nodiscard]]
    bool IsIndustrySelected() const noexcept
        {
        return m_testSelectionMethod == 1;
        }

    [[nodiscard]]
    bool IsManualTestSelected() const noexcept
        {
        return m_testSelectionMethod == 2;
        }

    [[nodiscard]]
    bool IsTestBundleSelected() const noexcept
        {
        return m_testSelectionMethod == 3;
        }

    // document type
    [[nodiscard]]
    readability::document_classification GetSelectedDocumentType() const noexcept
        {
        return static_cast<readability::document_classification>(m_selectedDocType);
        }

    void SelectDocumentType(const readability::document_classification docType)
        {
        m_selectedDocType = static_cast<decltype(m_selectedDocType)>(docType);
        TransferDataToWindow();
        }

    // industry type
    [[nodiscard]]
    readability::industry_classification GetSelectedIndustryType() const noexcept
        {
        return static_cast<readability::industry_classification>(m_selectedIndustryType);
        }

    void SelectIndustryType(const readability::industry_classification industryType)
        {
        m_selectedIndustryType = static_cast<decltype(m_selectedIndustryType)>(industryType);
        TransferDataToWindow();
        }

    // tests
    [[nodiscard]]
    BaseProject::TestCollectionType& GetReadabilityTestsInfo()
        {
        for (size_t i = 0; i < m_testsCheckListBox->GetCount(); ++i)
            {
            m_readabilityTests.include_test(m_testsCheckListBox->GetString(i),
                                            m_testsCheckListBox->IsChecked(i));
            }
        return m_readabilityTests;
        }

    [[nodiscard]]
    bool IsDolchSelected() const
        {
        return (GetLanguage() == readability::test_language::english_test) &&
               m_includeDolchSightWords;
        }

    [[nodiscard]]
    wxArrayInt GetSelectedCustomTests() const noexcept
        {
        return m_selectedCustomTests;
        }

    // test bundles
    [[nodiscard]]
    wxString GetSelectedTestBundle() const
        {
        return m_testsBundlesRadioBox->GetString(m_selectedBundle);
        }

    void SetSelectedTestBundle(const wxString& bundleName)
        {
        const auto foundPos = m_testsBundlesRadioBox->FindString(bundleName);
        if (foundPos != wxNOT_FOUND)
            {
            m_selectedBundle = foundPos;
            }
        TransferDataToWindow();
        }

    void SelectPage(const size_t page) { m_sideBarBook->SetSelection(page); }

    [[nodiscard]]
    size_t GetSelectedPage() const noexcept
        {
        return m_sideBarBook->GetSelection();
        }

    // just used for screenshots
    void SelectStandardTestManually(const wxString& test)
        {
        auto found = m_testsCheckListBox->FindString(test);
        if (found != wxNOT_FOUND)
            {
            m_selectedTests.push_back(found);
            }
        TransferDataToWindow();
        }

    void SetFileListTruncationMode(
        const Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode mode) noexcept
        {
        m_fileListTruncationMode = mode;
        }

  private:
    /// @brief Creates the controls and sizers.
    void CreateControls();

    [[nodiscard]]
    ProjectType GetProjectType() const noexcept
        {
        return m_projectType;
        }

    void LoadSpreadsheet(wxString excelPath = wxString{});
    void LoadArchive(const wxString& archivePath = wxString{});
    void UpdateTestSelectionMethodUI();
    void UpdateTestsUI();
    /** @brief Fills the file list from @c currentFileCount with group labels
            based on the paths in @c files.
        @warning @c files should be what is already in the list control and should
            already be sorted.*/
    void LoadGroupFromLastCommonFolder(const size_t currentFileCount, const wxArrayString& files);

    constexpr static int ID_FILE_BROWSE_BUTTON = wxID_HIGHEST;
    constexpr static int ID_FROM_FILE_BUTTON = wxID_HIGHEST + 1;
    constexpr static int ID_MANUALLY_ENTERED_TEXT_BUTTON = wxID_HIGHEST + 2;
    constexpr static int NARRATIVE_WITH_ILLUSTRATIONS_LINK_ID = wxID_HIGHEST + 3;
    constexpr static int FRAGMENTED_LINK_ID = wxID_HIGHEST + 4;
    constexpr static int CENTERED_TEXT_LINK_ID = wxID_HIGHEST + 5;
    constexpr static int TEST_SELECT_METHOD_BUTTON = wxID_HIGHEST + 6;
    constexpr static int LANGUAGE_BUTTON = wxID_HIGHEST + 7;
    constexpr static int ID_BATCH_FOLDER_BROWSE_BUTTON = wxID_HIGHEST + 8;
    constexpr static int ID_BATCH_FILE_BROWSE_BUTTON = wxID_HIGHEST + 9;
    constexpr static int ID_RANDOM_SAMPLE_CHECK = wxID_HIGHEST + 10;
    constexpr static int ID_WEB_PAGES_BROWSE_BUTTON = wxID_HIGHEST + 12;
    constexpr static int ID_ARCHIVE_FILE_BROWSE_BUTTON = wxID_HIGHEST + 13;
    constexpr static int ID_SPREADSHEET_FILE_BROWSE_BUTTON = wxID_HIGHEST + 14;
    constexpr static int NEWLINES_ALWAYS_NEW_PARAGRAPH_LINK_ID = wxID_HIGHEST + 15;
    constexpr static int ID_WEB_PAGE_BROWSE_BUTTON = wxID_HIGHEST + 16;
    constexpr static int ID_ADD_FILE_BUTTON = wxID_HIGHEST + 17;
    constexpr static int ID_DELETE_FILE_BUTTON = wxID_HIGHEST + 18;
    constexpr static int ID_HARD_RETURN_CHECKBOX = wxID_HIGHEST + 19;
    constexpr static int ID_HARD_RETURN_LABEL = wxID_HIGHEST + 20;
    constexpr static int ID_NONNARRATIVE_RADIO_BUTTON = wxID_HIGHEST + 21;
    constexpr static int ID_NONNARRATIVE_LABEL = wxID_HIGHEST + 22;
    constexpr static int ID_NARRATIVE_RADIO_BUTTON = wxID_HIGHEST + 23;
    constexpr static int ID_NARRATIVE_LABEL = wxID_HIGHEST + 24;
    constexpr static int ID_SENTENCES_SPLIT_RADIO_BUTTON = wxID_HIGHEST + 25;
    constexpr static int ID_SENTENCES_SPLIT_LABEL = wxID_HIGHEST + 26;
    constexpr static int ID_GROUP_BUTTON = wxID_HIGHEST + 27;
    constexpr static int ID_RANDOM_SAMPLE_SPIN = wxID_HIGHEST + 28;
    constexpr static int ID_DOC_TYPE_RADIO_BOX = wxID_HIGHEST + 29;
    constexpr static int ID_MIN_WORDS_LABEL = wxID_HIGHEST + 30;
    constexpr static int ID_TEST_BUNDLE_RADIO_BOX = wxID_HIGHEST + 31;
    constexpr static int ID_INDUSTRY_RADIO_BOX = wxID_HIGHEST + 32;
    constexpr static int ID_COMPOSITION_BOX = wxID_HIGHEST + 33;
    constexpr static int ID_LAYOUT_BOX = wxID_HIGHEST + 34;

    void OnHelp([[maybe_unused]] wxCommandEvent& event);
    void OnContextHelp([[maybe_unused]] wxHelpEvent& event);
    void OnFileBrowseButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnSourceRadioChange(wxCommandEvent& event);
    void OnTestSelectionMethodChanged([[maybe_unused]] wxCommandEvent& event);
    void OnLanguageChanged([[maybe_unused]] wxCommandEvent& event);
    void OnButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnOK([[maybe_unused]] wxCommandEvent& event);
    void OnNavigate(wxCommandEvent& event);
    void OnPageChange(wxBookCtrlEvent& event);
    void OnAddFolderButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnAddFileButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnAddWebPagesButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnAddWebPageButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnAddArchiveFileButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnAddSpreadsheetFileButtonClick([[maybe_unused]] wxCommandEvent& event);
    void OnRandomSampleCheck([[maybe_unused]] wxCommandEvent& event);
    void OnAddToListClick([[maybe_unused]] wxCommandEvent& event);
    void OnGroupClick([[maybe_unused]] wxCommandEvent& event);
    void OnDeleteFromListClick([[maybe_unused]] wxCommandEvent& event);

    // batch document entry
    Wisteria::UI::ListCtrlEx* m_fileList{ nullptr };
    std::shared_ptr<Wisteria::UI::ListCtrlExDataProvider> m_fileData{
        std::make_shared<Wisteria::UI::ListCtrlExDataProvider>()
    };
    wxCheckBox* m_isRandomSampling{ nullptr };
    wxSpinCtrl* m_randPercentageCtrl{ nullptr };
    static wxString m_lastSelectedFolder;

    int m_minDocWordCountForBatch;

    // text/document entry
    wxTextCtrl* m_filePathEdit{ nullptr };
    wxBitmapButton* m_fileBrowseButton{ nullptr };
    wxString m_filePath;
    bool m_fromFileSelected{ true };
    bool m_manualSelected{ false };

    wxTextCtrl* m_textEntryEdit{ nullptr };
    wxString m_enteredText;

    // document structure
    bool m_narrativeSelected{ false };
    bool m_splitLinesSelected{ false };
    bool m_fragmentedTextSelected{ false };
    bool m_centeredText{ false };
    bool m_newLinesAlwaysNewParagraphs{ false };

    // test selection method
    int m_testSelectionMethod{ 0 };

    wxBoxSizer* m_testTypesSizer{ nullptr };

    // document types
    wxRadioBox* m_docTypeRadioBox{ nullptr };
    int m_selectedDocType{ 0 };

    // industry types
    wxRadioBox* m_industryTypeRadioBox{ nullptr };
    int m_selectedIndustryType{ 0 };

    // tests
    wxFlexGridSizer* m_testsSizer{ nullptr };

    wxRadioBox* m_testsBundlesRadioBox{ nullptr };
    int m_selectedBundle{ 0 };

    bool m_includeDolchSightWords{ false };
    BaseProject::TestCollectionType m_readabilityTests;
    wxArrayInt m_selectedTests;
    wxCheckListBox* m_testsCheckListBox{ nullptr };
    wxArrayInt m_selectedCustomTests;
    wxCheckListBox* m_customTestsCheckListBox{ nullptr };
    wxCheckBox* m_DolchCheckBox{ nullptr };

    ProjectType m_projectType{ ProjectType::StandardProject };
    Wisteria::UI::SideBarBook* m_sideBarBook{ nullptr };
    int m_selectedLang{ 0 };

    Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode m_fileListTruncationMode{
        Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode::NoTruncation
    };
    };

#endif // PROJECTWIZARD_H
