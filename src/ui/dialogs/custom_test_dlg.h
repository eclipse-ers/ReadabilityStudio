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

#ifndef CUSTOM_TEST_DLG_H
#define CUSTOM_TEST_DLG_H

#include "../../OleanderStemmingLibrary/src/stemming.h"
#include "../../Wisteria-Dataviz/src/import/text_matrix.h"
#include "../../Wisteria-Dataviz/src/ui/controls/codeeditor.h"
#include "../../Wisteria-Dataviz/src/ui/controls/listctrlex.h"
#include "../../Wisteria-Dataviz/src/ui/controls/sidebarbook.h"
#include "../../Wisteria-Dataviz/src/ui/dialogs/functionbrowserdlg.h"
#include "../../Wisteria-Dataviz/src/util/memorymappedfile.h"
#include "../../indexing/character_traits.h"
#include "../../projects/standard_project_doc.h"
#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <wx/combobox.h>
#include <wx/intl.h>
#include <wx/propgrid/advprops.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/propgrid.h>
#include <wx/wx.h>

class ReadabilityApp;

/** @brief Dialog for creating and editing custom readability tests.*/
class CustomTestDlg final : public wxDialog
    {
    friend ReadabilityApp;

  public:
    /// @private
    using OutputStringType = traits::case_insensitive_wstring_ex;

    /// @brief Constructor.
    CustomTestDlg() { LoadStringConstants(); }

    /** @brief Constructor.
        @param parent The parent window.
        @param id The dialog's ID.
        @param testName The name for the test that is being created or edited.
        @param pos The dialog's position.
        @param size The dialog's size.
        @param style The dialog's style.*/
    explicit CustomTestDlg(wxWindow* parent, wxWindowID id = wxID_ANY,
                           const wxString& testName = wxEmptyString,
                           const wxPoint& pos = wxDefaultPosition,
                           const wxSize& size = wxSize(600, 600),
                           long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
        : m_testName(testName)
        {
        LoadStringConstants();
        Create(parent, id,
               testName.empty() ? wxString(_(L"Add Custom Test")) :
                                  wxString::Format(_(L"Edit \"%s\" Test"), testName),
               pos, size, style);
        }

    /** @brief Creates the dialog (used when using empty constructor).
        @param parent The parent window.
        @param id The dialog's ID.
        @param caption The dialog's caption.
        @param pos The dialog's position.
        @param size The dialog's size.
        @param style The dialog's style.*/
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY,
                const wxString& caption = _(L"Add Custom Test"),
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(600, 600),
                long style = wxDEFAULT_DIALOG_STYLE | wxCLIP_CHILDREN | wxRESIZE_BORDER);

    /// @brief Selects the page with the given page ID.
    /// @param pageId The window ID of the page to select.
    void SelectPage(const wxWindowID pageId);

    /// Get/Set stemming functions
    [[nodiscard]]
    stemming::stemming_type GetStemmingType() const
        {
        return static_cast<stemming::stemming_type>(
            m_wordListsPropertyGrid->GetPropertyValueAsInt(GetStemmingLanguageLabel()));
        }

    void SetStemmingType(stemming::stemming_type stemType);

    /// @brief Sets the test's formula.
    /// @param form The formula.
    void SetFormula(const wxString& form)
        {
        if (m_formulaCtrl)
            {
            m_formulaCtrl->ClearAll();
            m_formulaCtrl->SetText(form);
            }
        }

    /// @returns The test's formula.
    [[nodiscard]]
    wxString GetFormula() const
        {
        return m_formulaCtrl ? m_formulaCtrl->GetText() : wxString{};
        }

    /// Get/Set name functions
    void SetTestName(const wxString& name, const bool changeDlgTitle = true)
        {
        m_testName = name;
        if (changeDlgTitle)
            {
            SetTitle(wxString::Format(_(L"Edit \"%s\" Test"), m_testName));
            }
        TransferDataToWindow();
        }

    [[nodiscard]]
    wxString GetTestName() const
        {
        return m_testName;
        }

    /// Get/Set word list file path functions
    void SetWordListFilePath(const wxString& path)
        {
        wxASSERT(m_wordListsPropertyGrid);
        if (m_wordListsPropertyGrid)
            {
            m_wordListsPropertyGrid->SetPropertyValue(GetFileContainingFamiliarWordsLabel(), path);
            }
        }

    [[nodiscard]]
    wxString GetWordListFilePath() const
        {
        wxASSERT(m_wordListsPropertyGrid);
        return m_wordListsPropertyGrid ? m_wordListsPropertyGrid->GetPropertyValueAsString(
                                             GetFileContainingFamiliarWordsLabel()) :
                                         wxString{};
        }

    void SetIncludingDaleChallList(const bool include)
        {
        wxASSERT(m_wordListsPropertyGrid);
        if (m_wordListsPropertyGrid)
            {
            m_wordListsPropertyGrid->SetPropertyValue(GetIncludeDCWordListLabel(), include);
            }
        UpdateOptions();
        }

    [[nodiscard]]
    bool IsIncludingDaleChallList() const
        {
        wxASSERT(m_wordListsPropertyGrid);
        return m_wordListsPropertyGrid ?
                   m_wordListsPropertyGrid->GetPropertyValueAsBool(GetIncludeDCWordListLabel()) :
                   false;
        }

    void SetIncludingCustomWordList(const bool include)
        {
        wxASSERT(m_wordListsPropertyGrid);
        if (m_wordListsPropertyGrid)
            {
            m_wordListsPropertyGrid->SetPropertyValue(GetIncludeCustomListLabel(), include);
            }
        UpdateOptions();
        }

    [[nodiscard]]
    bool IsIncludingCustomWordList() const
        {
        wxASSERT(m_wordListsPropertyGrid);
        return m_wordListsPropertyGrid ?
                   m_wordListsPropertyGrid->GetPropertyValueAsBool(GetIncludeCustomListLabel()) :
                   false;
        }

    void SetIncludingSpacheList(const bool include)
        {
        wxASSERT(m_wordListsPropertyGrid);
        if (m_wordListsPropertyGrid)
            {
            m_wordListsPropertyGrid->SetPropertyValue(GetIncludeSpacheWordListLabel(), include);
            }
        UpdateOptions();
        }

    [[nodiscard]]
    bool IsIncludingSpacheList() const
        {
        wxASSERT(m_wordListsPropertyGrid);
        return m_wordListsPropertyGrid ? m_wordListsPropertyGrid->GetPropertyValueAsBool(
                                             GetIncludeSpacheWordListLabel()) :
                                         false;
        }

    void SetIncludingHJList(const bool include)
        {
        wxASSERT(m_wordListsPropertyGrid);
        if (m_wordListsPropertyGrid)
            {
            m_wordListsPropertyGrid->SetPropertyValue(GetIncludeHJWordListLabel(), include);
            }
        UpdateOptions();
        }

    [[nodiscard]]
    bool IsIncludingHJList() const
        {
        wxASSERT(m_wordListsPropertyGrid);
        return m_wordListsPropertyGrid ?
                   m_wordListsPropertyGrid->GetPropertyValueAsBool(GetIncludeHJWordListLabel()) :
                   false;
        }

    void SetIncludingStockerList(const bool include)
        {
        wxASSERT(m_wordListsPropertyGrid);
        if (m_wordListsPropertyGrid)
            {
            m_wordListsPropertyGrid->SetPropertyValue(GetIncludeStockerWordListLabel(), include);
            }
        UpdateOptions();
        }

    [[nodiscard]]
    bool IsIncludingStockerList() const
        {
        wxASSERT(m_wordListsPropertyGrid);
        return m_wordListsPropertyGrid ? m_wordListsPropertyGrid->GetPropertyValueAsBool(
                                             GetIncludeStockerWordListLabel()) :
                                         false;
        }

    void SetFamiliarWordsMustBeOnEachIncludedList(const bool include)
        {
        wxASSERT(m_wordListsPropertyGrid);
        if (m_wordListsPropertyGrid)
            {
            m_wordListsPropertyGrid->SetPropertyValue(GetFamiliarWordsOnAllLabel(), include);
            }
        }

    [[nodiscard]]
    bool IsFamiliarWordsMustBeOnEachIncludedList() const
        {
        wxASSERT(m_wordListsPropertyGrid);
        return m_wordListsPropertyGrid ?
                   m_wordListsPropertyGrid->IsPropertyEnabled(GetFamiliarWordsOnAllLabel()) &&
                       m_wordListsPropertyGrid->GetPropertyValueAsBool(
                           GetFamiliarWordsOnAllLabel()) :
                   false;
        }

    void SetProperNounMethod(const int method)
        {
        wxASSERT(m_properNounsNumbersPropertyGrid);
        if (m_properNounsNumbersPropertyGrid)
            {
            m_properNounsNumbersPropertyGrid->SetPropertyValue(GetFamiliarityLabel(), method);
            }
        }

    [[nodiscard]]
    int GetProperNounMethod() const
        {
        wxASSERT(m_properNounsNumbersPropertyGrid);
        return m_properNounsNumbersPropertyGrid ?
                   m_properNounsNumbersPropertyGrid->GetPropertyValueAsInt(GetFamiliarityLabel()) :
                   0;
        }

    void SetIncludingNumeric(const bool include)
        {
        wxASSERT(m_properNounsNumbersPropertyGrid);
        if (m_properNounsNumbersPropertyGrid)
            {
            m_properNounsNumbersPropertyGrid->SetPropertyValue(GetNumeralsAsFamiliarLabel(),
                                                               include);
            }
        }

    [[nodiscard]]
    bool IsIncludingNumeric() const
        {
        wxASSERT(m_properNounsNumbersPropertyGrid);
        return m_properNounsNumbersPropertyGrid ?
                   m_properNounsNumbersPropertyGrid->GetPropertyValueAsBool(
                       GetNumeralsAsFamiliarLabel()) :
                   false;
        }

    void SetTestType(const int type)
        {
        m_testType = type;
        if (m_testType == static_cast<int>(readability::readability_test_type::grade_level))
            {
            m_testTypeCombo->SetSelection(0);
            }
        else if (m_testType == static_cast<int>(readability::readability_test_type::index_value))
            {
            m_testTypeCombo->SetSelection(1);
            }
        else if (m_testType ==
                 static_cast<int>(readability::readability_test_type::predicted_cloze_score))
            {
            m_testTypeCombo->SetSelection(2);
            }
        else
            {
            m_testTypeCombo->SetSelection(0);
            }
        }

    /// @returns The test result type.
    [[nodiscard]]
    int GetTestType() const
        {
        return m_testType;
        }

    // project (industry) settings
    [[nodiscard]]
    bool IsChildrensPublishingSelected() const
        {
        wxASSERT(m_associationPropertyGrid);
        return m_associationPropertyGrid ?
                   m_associationPropertyGrid->GetPropertyValueAsBool(m_professionNames[0]) :
                   false;
        }

    void SetChildrensPublishingSelected(const bool select)
        {
        wxASSERT(m_associationPropertyGrid);
        if (m_associationPropertyGrid)
            {
            m_associationPropertyGrid->SetPropertyValue(m_professionNames[0], select);
            }
        }

    [[nodiscard]]
    bool IsAdultPublishingSelected() const
        {
        wxASSERT(m_associationPropertyGrid);
        return m_associationPropertyGrid ?
                   m_associationPropertyGrid->GetPropertyValueAsBool(m_professionNames[1]) :
                   false;
        }

    void SetAdultPublishingSelected(const bool select)
        {
        wxASSERT(m_associationPropertyGrid);
        if (m_associationPropertyGrid)
            {
            m_associationPropertyGrid->SetPropertyValue(m_professionNames[1], select);
            }
        }

    [[nodiscard]]
    bool IsChildrensHealthCareTestSelected() const
        {
        wxASSERT(m_associationPropertyGrid);
        return m_associationPropertyGrid ?
                   m_associationPropertyGrid->GetPropertyValueAsBool(m_professionNames[2]) :
                   false;
        }

    void SetChildrensHealthCareTestSelected(const bool select)
        {
        wxASSERT(m_associationPropertyGrid);
        if (m_associationPropertyGrid)
            {
            m_associationPropertyGrid->SetPropertyValue(m_professionNames[2], select);
            }
        }

    [[nodiscard]]
    bool IsAdultHealthCareTestSelected() const
        {
        wxASSERT(m_associationPropertyGrid);
        return m_associationPropertyGrid ?
                   m_associationPropertyGrid->GetPropertyValueAsBool(m_professionNames[3]) :
                   false;
        }

    void SetAdultHealthCareTestSelected(const bool select)
        {
        wxASSERT(m_associationPropertyGrid);
        if (m_associationPropertyGrid)
            {
            m_associationPropertyGrid->SetPropertyValue(m_professionNames[3], select);
            }
        }

    [[nodiscard]]
    bool IsMilitaryTestSelected() const
        {
        wxASSERT(m_associationPropertyGrid);
        return m_associationPropertyGrid ?
                   m_associationPropertyGrid->GetPropertyValueAsBool(m_professionNames[4]) :
                   false;
        }

    void SetMilitaryTestSelected(const bool select)
        {
        wxASSERT(m_associationPropertyGrid);
        if (m_associationPropertyGrid)
            {
            m_associationPropertyGrid->SetPropertyValue(m_professionNames[4], select);
            }
        }

    [[nodiscard]]
    bool IsSecondaryLanguageSelected() const
        {
        wxASSERT(m_associationPropertyGrid);
        return m_associationPropertyGrid ?
                   m_associationPropertyGrid->GetPropertyValueAsBool(m_professionNames[5]) :
                   false;
        }

    void SetSecondaryLanguageSelected(const bool select)
        {
        wxASSERT(m_associationPropertyGrid);
        if (m_associationPropertyGrid)
            {
            m_associationPropertyGrid->SetPropertyValue(m_professionNames[5], select);
            }
        }

    [[nodiscard]]
    bool IsBroadcastingSelected() const
        {
        wxASSERT(m_associationPropertyGrid);
        return m_associationPropertyGrid ?
                   m_associationPropertyGrid->GetPropertyValueAsBool(m_professionNames[6]) :
                   false;
        }

    void SetBroadcastingSelected(const bool select)
        {
        wxASSERT(m_associationPropertyGrid);
        if (m_associationPropertyGrid)
            {
            m_associationPropertyGrid->SetPropertyValue(m_professionNames[6], select);
            }
        }

    // document
    [[nodiscard]]
    bool IsGeneralDocumentSelected() const
        {
        wxASSERT(m_associationPropertyGrid);
        return m_associationPropertyGrid ?
                   m_associationPropertyGrid->GetPropertyValueAsBool(m_documentNames[0]) :
                   false;
        }

    void SetGeneralDocumentSelected(const bool select)
        {
        wxASSERT(m_associationPropertyGrid);
        if (m_associationPropertyGrid)
            {
            m_associationPropertyGrid->SetPropertyValue(m_documentNames[0], select);
            }
        }

    [[nodiscard]]
    bool IsTechnicalDocumentSelected() const
        {
        wxASSERT(m_associationPropertyGrid);
        return m_associationPropertyGrid ?
                   m_associationPropertyGrid->GetPropertyValueAsBool(m_documentNames[1]) :
                   false;
        }

    void SetTechnicalDocumentSelected(const bool select)
        {
        wxASSERT(m_associationPropertyGrid);
        if (m_associationPropertyGrid)
            {
            m_associationPropertyGrid->SetPropertyValue(m_documentNames[1], select);
            }
        }

    [[nodiscard]]
    bool IsNonNarrativeFormSelected() const
        {
        wxASSERT(m_associationPropertyGrid);
        return m_associationPropertyGrid ?
                   m_associationPropertyGrid->GetPropertyValueAsBool(m_documentNames[2]) :
                   false;
        }

    void SetNonNarrativeFormSelected(const bool select)
        {
        wxASSERT(m_associationPropertyGrid);
        if (m_associationPropertyGrid)
            {
            m_associationPropertyGrid->SetPropertyValue(m_documentNames[2], select);
            }
        }

    [[nodiscard]]
    bool IsYoungAdultAndAdultLiteratureSelected() const
        {
        wxASSERT(m_associationPropertyGrid);
        return m_associationPropertyGrid ?
                   m_associationPropertyGrid->GetPropertyValueAsBool(m_documentNames[3]) :
                   false;
        }

    void SetYoungAdultAndAdultLiteratureSelected(const bool select)
        {
        wxASSERT(m_associationPropertyGrid);
        if (m_associationPropertyGrid)
            {
            m_associationPropertyGrid->SetPropertyValue(m_documentNames[3], select);
            }
        }

    [[nodiscard]]
    bool IsChildrensLiteratureSelected() const
        {
        wxASSERT(m_associationPropertyGrid);
        return m_associationPropertyGrid ?
                   m_associationPropertyGrid->GetPropertyValueAsBool(m_documentNames[4]) :
                   false;
        }

    void SetChildrensLiteratureSelected(const bool select)
        {
        wxASSERT(m_associationPropertyGrid);
        if (m_associationPropertyGrid)
            {
            m_associationPropertyGrid->SetPropertyValue(m_documentNames[4], select);
            }
        }

    void ShowFunctionBrowser(const bool show);

    /// @brief General page.
    constexpr static int ID_GENERAL_PAGE = wxID_HIGHEST;
    /// @brief Word list page.
    constexpr static int ID_WORD_LIST_PAGE = wxID_HIGHEST + 1;
    /// @brief Numerals page.
    constexpr static int ID_PROPER_NUMERALS_PAGE = wxID_HIGHEST + 2;
    /// @brief Classification page.
    constexpr static int ID_CLASSIFICATION_PAGE = wxID_HIGHEST + 3;

  private:
    // control IDs
    constexpr static int ID_WORD_LIST_PROPERTY_GRID = wxID_HIGHEST + 4;
    constexpr static int ID_VALIDATE_FORMULA_BUTTON = wxID_HIGHEST + 5;
    constexpr static int ID_INSERT_FUNCTION_BUTTON = wxID_HIGHEST + 6;
    constexpr static int ID_TEST_NAME_FIELD = wxID_HIGHEST + 7;
    constexpr static int ID_TEST_TYPE_COMBO = wxID_HIGHEST + 8;
    constexpr static int ID_FORMULA_FIELD = wxID_HIGHEST + 9;
    constexpr static int ID_PROPER_NOUN_PROPERTY_GRID = wxID_HIGHEST + 10;
    constexpr static int ID_CLASSIFICATION_PROPERTY_GRID = wxID_HIGHEST + 11;
    constexpr static int ID_FUNCTION_BROWSER = wxID_HIGHEST + 12;
    constexpr static int ID_FUNCTION_BROWSER_BUTTON = wxID_HIGHEST + 13;

    void OnOK([[maybe_unused]] wxCommandEvent& event);
    void OnHelp([[maybe_unused]] wxCommandEvent& event);
    void OnContextHelp([[maybe_unused]] wxHelpEvent& event);
    void OnPropertyGridChange(wxPropertyGridEvent& event);

    bool ValidateFormula(const bool promptOnSuccess = false);
    void UpdateOptions();
    void CreateControls();

    void LoadStringConstants()
        {
        m_testTypes.Add(_(L"Grade level"));
        m_testTypes.Add(_(L"Index value"));
        m_testTypes.Add(_(L"Predicted cloze score"));

        m_documentNames.Add(_(L"General document"));
        m_documentNames.Add(_(L"Technical document"));
        m_documentNames.Add(_(L"Structured form"));
        m_documentNames.Add(_(L"Literature"));
        m_documentNames.Add(_(L"Children's literature"));

        m_professionNames.Add(_(L"Children's publishing"));
        m_professionNames.Add(_(L"Young adult and adult publishing"));
        m_professionNames.Add(_(L"Children's healthcare"));
        m_professionNames.Add(_(L"Adult healthcare"));
        m_professionNames.Add(_(L"Military and government"));
        m_professionNames.Add(_(L"Second language education"));
        m_professionNames.Add(_(L"Broadcasting"));
        }

    [[nodiscard]]
    static wxString GetNumeralsLabel()
        {
        return _(L"Numerals");
        }

    [[nodiscard]]
    static wxString GetProperNounsLabel()
        {
        return _(L"Proper Nouns");
        }

    [[nodiscard]]
    static wxString GetFamiliarityLabel()
        {
        return _(L"Familiarity");
        }

    [[nodiscard]]
    static wxString GetNumeralsAsFamiliarLabel()
        {
        return _(L"Treat numerals as familiar");
        }

    [[nodiscard]]
    static wxString GetIncludeCustomListLabel()
        {
        return _(L"Include custom familiar word list");
        }

    [[nodiscard]]
    static wxString GetCustomFamiliarWordListLabel()
        {
        return _(L"Custom familiar word list");
        }

    [[nodiscard]]
    static wxString GetFileContainingFamiliarWordsLabel()
        {
        return _(L"File containing familiar words");
        }

    [[nodiscard]]
    static wxString GetStemmingLanguageLabel()
        {
        return _(L"Use stemming to search for similar words");
        }

    [[nodiscard]]
    static wxString GetStandardWordListsLabel()
        {
        return _(L"Standard familiar word lists");
        }

    [[nodiscard]]
    static wxString GetIncludeDCWordListLabel()
        {
        // TRANSLATORS: %s is a test (person) name, like Spache.
        return wxString::Format(_(L"Include %s word list"), _DT(L"New Dale-Chall"));
        }

    [[nodiscard]]
    static wxString GetIncludeSpacheWordListLabel()
        {
        return wxString::Format(_(L"Include %s word list"), _DT(L"Spache"));
        }

    [[nodiscard]]
    static wxString GetIncludeHJWordListLabel()
        {
        return wxString::Format(_(L"Include %s word list"), _DT(L"Harris-Jacobson"));
        }

    [[nodiscard]]
    static wxString GetIncludeStockerWordListLabel()
        {
        return _(L"Include Stocker's Catholic supplement");
        }

    [[nodiscard]]
    static wxString GetOtherLabel()
        {
        return _(L"Other");
        }

    [[nodiscard]]
    static wxString GetFamiliarWordsOnAllLabel()
        {
        return _(L"Only use words common to all lists");
        }

    Wisteria::UI::SideBarBook* m_sideBarBook{ nullptr };
    wxTextCtrl* m_testNameCtrl{ nullptr };
    Wisteria::UI::CodeEditor* m_formulaCtrl{ nullptr };
    wxComboBox* m_testTypeCombo{ nullptr };
    wxPropertyGridPage* m_wordListsPropertyGrid{ nullptr };
    wxPropertyGridPage* m_properNounsNumbersPropertyGrid{ nullptr };
    wxPropertyGrid* m_associationPropertyGrid{ nullptr };
    wxString m_testName;
    wxString m_currentWordListFilePath;
    int m_testType{ 0 };
    wxArrayString m_testTypes;
    wxArrayString m_professionNames;
    wxArrayString m_documentNames;

    wxPanel* m_generalPage{ nullptr };
    wxBoxSizer* m_funcBrowserSizer{ nullptr };

    // data used for browser and function guide
    Wisteria::UI::FunctionBrowserCtrl::NameList m_operators;
    Wisteria::UI::FunctionBrowserCtrl::NameList m_logic;
    Wisteria::UI::FunctionBrowserCtrl::NameList m_math;
    Wisteria::UI::FunctionBrowserCtrl::NameList m_statistics;
    Wisteria::UI::FunctionBrowserCtrl::NameList m_generalDocumentStatistics;
    Wisteria::UI::FunctionBrowserCtrl::NameList m_customFamiliarWords;
    Wisteria::UI::FunctionBrowserCtrl::NameList m_wordFunctions;
    Wisteria::UI::FunctionBrowserCtrl::NameList m_sentenceFunctions;
    Wisteria::UI::FunctionBrowserCtrl::NameList m_shortcuts;
    };

#endif // CUSTOM_TEST_DLG_H
