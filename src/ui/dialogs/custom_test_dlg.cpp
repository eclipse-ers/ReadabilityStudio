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

#include "custom_test_dlg.h"
#include "../../app/readability_app_options.h"
#include "../../projects/base_project.h"
#include "../../projects/base_project_doc.h"
#include "../../projects/base_project_view.h"
#include "../../results-format/project_report_format.h"
#include "../../test-helpers/readability_formula_parser.h"
#include "../controls/word_list_property.h"
#include "edit_word_list_dlg.h"
#include <wx/richmsgdlg.h>

wxDECLARE_APP(ReadabilityApp);

//-------------------------------------------------------------
bool CustomTestDlg::ValidateFormula(const bool promptOnSuccess /*= false*/)
    {
    TransferDataFromWindow();
    // make any corrections to the formula
    SetFormula(BaseProjectDoc::UpdateCustomReadabilityTest(GetFormula().wc_str()).c_str());
    TransferDataToWindow();

    try
        {
        // if no formula specified, but they do have some sort of familiar word definition,
        // then suggest CustomNewDaleChall()
        if (GetFormula().empty() &&
            (IsIncludingCustomWordList() || IsIncludingSpacheList() || IsIncludingHJList() ||
             IsIncludingDaleChallList() || IsIncludingStockerList()))
            {
            // TRANSLATORS: "New Dale-Chall" should not be translated; it is a test name.
            if (wxMessageBox(_(L"A familiar word list has been defined, "
                               "but no formula has been specified. "
                               "Do you wish to use the New Dale-Chall formula for this test?"),
                             _(L"No Formula Defined"), wxYES_NO | wxICON_QUESTION) == wxNO)
                {
                return false;
                }

            SetFormula(ReadabilityFormulaParser::GetCustomNewDaleChallSignature());
            SetIncludingDaleChallList(true);
            SetProperNounMethod(static_cast<int>(
                wxGetApp().GetAppOptions()->GetDaleChallProperNounCountingMethod()));
            TransferDataToWindow();
            return true;
            }
        if (GetFormula().empty())
            {
            wxMessageBox(_(L"Please enter a formula."), _(L"Missing Formula"),
                         wxOK | wxICON_EXCLAMATION);
            return false;
            }

        auto blankProject = std::make_unique<BaseProjectDoc>();
        auto* activeProject = dynamic_cast<BaseProjectDoc*>(
            wxGetApp().GetMainFrame()->GetDocumentManager()->GetCurrentDocument());
        const bool isUsingActiveProject =
            ((activeProject != nullptr) && activeProject->IsKindOf(CLASSINFO(ProjectDoc)));
        BaseProjectDoc* project = isUsingActiveProject ? activeProject : blankProject.get();
        wxASSERT_MSG(project, L"Invalid project!");

        if (!project->GetFormulaParser().compile(GetFormula().ToStdString()))
            {
            wxMessageBox(
                wxString::Format(
                    _(L"Syntax error in formula at position %s:\n %s\n^"),
                    std::to_wstring(project->GetFormulaParser().get_last_error_position()),
                    GetFormula().substr(project->GetFormulaParser().get_last_error_position())),
                _(L"Error in Formula"), wxOK | wxICON_EXCLAMATION);
            return false;
            }
        // if using Custom DC test but DC word list is not included then ask them about it
        if (project->GetFormulaParser().is_function_used(
                ReadabilityFormulaParser::SignatureToFunctionName(
                    ReadabilityFormulaParser::GetCustomNewDaleChallSignature())
                    .ToStdString()) &&
            !IsIncludingDaleChallList())
            {
            // TRANSLATORS: "New Dale-Chall" should not be translated; it is a test name.
            if (wxMessageBox(_(L"The New Dale-Chall formula is being used for this test, "
                               "but the standard Dale-Chall word list is not being included. "
                               "Do you wish to include this word list?"),
                             _(L"Settings Conflict"), wxYES_NO | wxICON_QUESTION) == wxYES)
                {
                SetIncludingDaleChallList(true);
                TransferDataToWindow();
                }
            }
        // if using Custom Spache test but Spache word list is not included then ask them about it
        if (project->GetFormulaParser().is_function_used(
                ReadabilityFormulaParser::SignatureToFunctionName(
                    ReadabilityFormulaParser::GetCustomSpacheSignature())
                    .ToStdString()) &&
            !IsIncludingSpacheList())
            {
            if (wxMessageBox(_(L"The Spache formula is being used for this test, "
                               "but the standard Spache word list is not being included. "
                               "Do you wish to include this word list?"),
                             _(L"Settings Conflict"), wxYES_NO | wxICON_QUESTION) == wxYES)
                {
                SetIncludingSpacheList(true);
                TransferDataToWindow();
                }
            }
        // if using Custom HJ test but HJ word list is not included then ask them about it
        if (project->GetFormulaParser().is_function_used(
                ReadabilityFormulaParser::SignatureToFunctionName(
                    ReadabilityFormulaParser::GetCustomHarrisJacobsonSignature())
                    .ToStdString()) &&
            !IsIncludingHJList())
            {
            if (wxMessageBox(_(L"The Harris-Jacobson formula is being used for this test, "
                               "but the standard Harris-Jacobson word list is not being included. "
                               "Do you wish to include this word list?"),
                             _(L"Settings Conflict"), wxYES_NO | wxICON_QUESTION) == wxYES)
                {
                SetIncludingHJList(true);
                TransferDataToWindow();
                }
            }
        if (project->GetFormulaParser().is_function_used(
                ReadabilityFormulaParser::SignatureToFunctionName(
                    ReadabilityFormulaParser::GetCustomNewDaleChallSignature())
                    .ToStdString()) &&
            GetProperNounMethod() !=
                static_cast<int>(
                    wxGetApp().GetAppOptions()->GetDaleChallProperNounCountingMethod()))
            {
            auto warningIter = WarningManager::GetWarning(_DT(L"ndc-proper-noun-conflict"));
            if (warningIter != WarningManager::GetWarnings().end())
                {
                if (!warningIter->ShouldBeShown())
                    {
                    if (warningIter->GetPreviousResponse() == wxID_YES)
                        {
                        SetProperNounMethod(static_cast<int>(
                            wxGetApp().GetAppOptions()->GetDaleChallProperNounCountingMethod()));
                        TransferDataToWindow();
                        }
                    }
                else
                    {
                    wxRichMessageDialog msg(this, warningIter->GetMessage(),
                                            warningIter->GetTitle(), warningIter->GetFlags());
                    msg.SetEscapeId(wxID_NO);
                    msg.ShowCheckBox(_(L"Remember my answer"));
                    msg.SetYesNoLabels(_(L"Adjust to match New Dale-Chall"), _(L"Do not adjust"));
                    const int dlgResponse = msg.ShowModal();
                    if (dlgResponse == wxID_YES)
                        {
                        SetProperNounMethod(static_cast<int>(
                            wxGetApp().GetAppOptions()->GetDaleChallProperNounCountingMethod()));
                        TransferDataToWindow();
                        }
                    if (warningIter != WarningManager::GetWarnings().end() &&
                        msg.IsCheckBoxChecked())
                        {
                        warningIter->Show(false);
                        warningIter->SetPreviousResponse(dlgResponse);
                        }
                    }
                }
            }
        // HJ has really specific rules, so if using Custom HJ then
        // nag the user about using similar options to HJ
        if (project->GetFormulaParser().is_function_used(
                ReadabilityFormulaParser::SignatureToFunctionName(
                    ReadabilityFormulaParser::GetCustomHarrisJacobsonSignature())
                    .ToStdString()) &&
            !IsIncludingNumeric())
            {
            auto warningIter = WarningManager::GetWarning(
                _DT(L"custom-test-numeral-settings-adjustment-required"));
            if (warningIter != WarningManager::GetWarnings().end() && warningIter->ShouldBeShown())
                {
                wxMessageBox(warningIter->GetMessage(), wxGetApp().GetAppName(),
                             warningIter->GetFlags(), this);
                }
            SetIncludingNumeric(true);
            TransferDataToWindow();
            }
        // If formula is using any custom familiar word logic,
        // then make sure at least one word list is defined
        if ((project->GetFormulaParser().is_function_used(
                 ReadabilityFormulaParser::SignatureToFunctionName(
                     ReadabilityFormulaParser::GetCustomHarrisJacobsonSignature())
                     .ToStdString()) ||
             project->GetFormulaParser().is_function_used("UnfamiliarWordCount") ||
             project->GetFormulaParser().is_function_used("FamiliarWordCount") ||
             project->GetFormulaParser().is_function_used("UniqueUnfamiliarWordCount") ||
             project->GetFormulaParser().is_function_used(
                 ReadabilityFormulaParser::SignatureToFunctionName(
                     ReadabilityFormulaParser::GetCustomSpacheSignature())
                     .ToStdString()) ||
             project->GetFormulaParser().is_function_used(
                 ReadabilityFormulaParser::SignatureToFunctionName(
                     ReadabilityFormulaParser::GetCustomNewDaleChallSignature())
                     .ToStdString())) &&
            (!IsIncludingCustomWordList() && !IsIncludingSpacheList() && !IsIncludingHJList() &&
             !IsIncludingDaleChallList() && !IsIncludingStockerList()))
            {
            wxMessageBox(_(L"Familiar words not defined.\n\n"
                           "This formula requires a familiar word list to be selected."),
                         _(L"Error in Formula"), wxOK | wxICON_EXCLAMATION);
            return false;
            }
        // ...and vice versa. If they defined custom familiar word logic but the formula is
        // not using it then at least point that out to make sure that was the intention
        if ((!project->GetFormulaParser().is_function_used(
                 ReadabilityFormulaParser::SignatureToFunctionName(
                     ReadabilityFormulaParser::GetCustomHarrisJacobsonSignature())
                     .ToStdString()) &&
             !project->GetFormulaParser().is_function_used("UnfamiliarWordCount") &&
             !project->GetFormulaParser().is_function_used("FamiliarWordCount") &&
             !project->GetFormulaParser().is_function_used("UniqueUnfamiliarWordCount") &&
             !project->GetFormulaParser().is_function_used(
                 ReadabilityFormulaParser::SignatureToFunctionName(
                     ReadabilityFormulaParser::GetCustomSpacheSignature())
                     .ToStdString()) &&
             !project->GetFormulaParser().is_function_used(
                 ReadabilityFormulaParser::SignatureToFunctionName(
                     ReadabilityFormulaParser::GetCustomNewDaleChallSignature())
                     .ToStdString())) &&
            (IsIncludingCustomWordList() || IsIncludingSpacheList() || IsIncludingHJList() ||
             IsIncludingDaleChallList() || IsIncludingStockerList()))
            {
            wxMessageBox(
                _(L"A familiar word list has been defined, but the formula is not using it. "
                  "Familiar word list definitions will be ignored."),
                _(L"Warning"), wxOK | wxICON_EXCLAMATION);
            }
        if ((project->GetFormulaParser().is_function_used(
                 ReadabilityFormulaParser::SignatureToFunctionName(
                     ReadabilityFormulaParser::GetCustomHarrisJacobsonSignature())
                     .ToStdString()) ||
             project->GetFormulaParser().is_function_used(
                 ReadabilityFormulaParser::SignatureToFunctionName(
                     ReadabilityFormulaParser::GetCustomSpacheSignature())
                     .ToStdString()) ||
             project->GetFormulaParser().is_function_used(
                 ReadabilityFormulaParser::SignatureToFunctionName(
                     ReadabilityFormulaParser::GetCustomNewDaleChallSignature())
                     .ToStdString())) &&
            m_testTypeCombo->GetSelection() !=
                static_cast<int>(readability::readability_test_type::grade_level))
            {
            wxMessageBox(_(L"Custom familiar word tests must return a grade level result. "
                           "Test result type has been reset to grade level."),
                         _(L"Warning"), wxOK | wxICON_EXCLAMATION);
            SetTestType(static_cast<int>(readability::readability_test_type::grade_level));
            TransferDataToWindow();
            }
        if (promptOnSuccess)
            {
            const wxString calculatedValueMsg =
                !isUsingActiveProject ?
                    wxString{} :
                    wxString::FromDouble(project->GetFormulaParser().evaluate(), 1)
                        .Prepend(_(L"Calculated value: "))
                        .Prepend(L"\n\n");
            wxMessageBox(_(L"Formula is valid; no syntax errors were detected.") +
                             calculatedValueMsg,
                         _(L"Formula Validated"), wxOK | wxICON_INFORMATION);
            }
        }
    catch (const std::exception& exp)
        {
        wxMessageBox(
            wxString::Format(_(L"%s\nPlease verify the syntax of the formula."), exp.what()),
            _(L"Error in Formula"), wxOK | wxICON_EXCLAMATION);
        return false;
        }
    catch (...)
        {
        wxMessageBox(_(L"An unknown error occurred while validating the formula. "
                       "Please verify the syntax of the formula."),
                     _(L"Error in Formula"), wxOK | wxICON_EXCLAMATION);
        return false;
        }

    return true;
    }

//-------------------------------------------------------------
void CustomTestDlg::UpdateOptions()
    {
    wxASSERT_MSG(m_wordListsPropertyGrid, L"Invalid word list property grid!");
    wxASSERT_MSG(m_properNounsNumbersPropertyGrid, L"Invalid noun list property grid!");
    if ((m_wordListsPropertyGrid == nullptr) || (m_properNounsNumbersPropertyGrid == nullptr))
        {
        return;
        }
    m_wordListsPropertyGrid->EnableProperty(GetFileContainingFamiliarWordsLabel(),
                                            IsIncludingCustomWordList());
    m_wordListsPropertyGrid->EnableProperty(GetStemmingLanguageLabel(),
                                            IsIncludingCustomWordList());

    size_t selectedListCount = 0;
    selectedListCount += IsIncludingCustomWordList() ? 1 : 0;
    selectedListCount += IsIncludingDaleChallList() ? 1 : 0;
    selectedListCount += IsIncludingSpacheList() ? 1 : 0;
    selectedListCount += IsIncludingHJList() ? 1 : 0;
    selectedListCount += IsIncludingStockerList() ? 1 : 0;

    m_wordListsPropertyGrid->EnableProperty(GetFamiliarWordsOnAllLabel(), selectedListCount > 1);
    m_properNounsNumbersPropertyGrid->EnableProperty(GetFamiliarityLabel(), selectedListCount > 0);
    m_properNounsNumbersPropertyGrid->EnableProperty(GetNumeralsAsFamiliarLabel(),
                                                     selectedListCount > 0);
    }

//-------------------------------------------------------------
void CustomTestDlg::OnContextHelp([[maybe_unused]] wxHelpEvent& event)
    {
    wxCommandEvent cmd;
    OnHelp(cmd);
    }

//-------------------------------------------------------------
void CustomTestDlg::OnHelp([[maybe_unused]] wxCommandEvent& event)
    {
    wxLaunchDefaultBrowser(wxFileName::FileNameToURL(wxGetApp().GetMainFrame()->GetHelpDirectory() +
                                                     wxFileName::GetPathSeparator() +
                                                     _DT(L"online/custom-tests-and-bundles.html")));
    }

//------------------------------------------------------
bool CustomTestDlg::Create(wxWindow* parent, wxWindowID id, const wxString& caption,
                           const wxPoint& pos, const wxSize& size, long style)
    {
    wxDialog::Create(parent, id, caption, pos, size, style);
    SetExtraStyle(GetExtraStyle() | wxWS_EX_VALIDATE_RECURSIVELY | wxWS_EX_CONTEXTHELP);

    wxIcon ico;
    ico.CopyFromBitmap(wxGetApp()
                           .GetResourceManager()
                           .GetSVG(L"ribbon/formula.svg")
                           .GetBitmap(FromDIP(wxSize{ 16, 16 })));
    SetIcon(ico);

    const auto formatCodeExample = [](const wxString& example)
    {
        return FormulaFormat::FormatMathExpressionFromUS(L"<br /><br /><tt><b>" + example +
                                                         L"</b></tt>");
    };

    m_operators = { wxString(_DT(L"*\t") + _(L"Multiplication.")).wc_string(),
                    wxString(_DT(L"/\t") + _(L"Division.")).wc_string(),
                    wxString(_DT(L"%\t") +
                             _(L"Modulus: Divides two values and returns the remainder."))
                        .wc_string(),
                    wxString(_DT(L"+\t") + _(L"Addition.")).wc_string(),
                    wxString(_DT(L"-\t") + _(L"Subtraction.")).wc_string(),
                    wxString(_DT(L"^\t") + _(L"Exponentiation.")).wc_string(),
                    wxString(_DT(L"=\t") + _(L"Equals.")).wc_string(),
                    wxString(_DT(L"<\t") + _(L"Less than.")).wc_string(),
                    wxString(_DT(L">\t") + _(L"Greater than.")).wc_string(),
                    wxString(_DT(L"<>\t") + _(L"Not equal to.")).wc_string(),
                    wxString(_DT(L">=\t") + _(L"Greater than or equal to.")).wc_string(),
                    wxString(_DT(L"<=\t") + _(L"Less than or equal to.")).wc_string(),
                    wxString(_DT(L"&\t") + _(L"Logical conjunction.")).wc_string(),
                    wxString(_DT(L"|\t") + _(L"Logical alternative.")).wc_string() };

    m_logic = { wxString(
                    FormulaFormat::FormatMathExpressionFromUS(
                        _DT(L"IF(condition,value_if_true,value_if_false)\t")) +
                    _(L"If \"condition\" is true (non-zero), then \"value_if_true\" is returned; "
                      "otherwise, \"value_if_false\" is returned."))
                    .wc_string(),
                wxString(FormulaFormat::FormatMathExpressionFromUS(_DT(L"NOT(value)\t")) +
                         _(L"Returns the logical negation of \"value.\""))
                    .wc_string() };

    m_math = {
        wxString(_DT(L"MOD(Number,Divisor)\t") +
                 _(L"Returns the remainder after Number is divided by Divisor. "
                   "The result has the same sign as divisor."))
            .wc_string(),
        wxString(_DT(L"CLAMP(x,start,end)\t") +
                 _(L"Constrains x within the range of start and end."))
            .wc_string(),
        wxString(_DT(L"EXP(x)\t") + _(L"Exponential function (Euler to the power of x)."))
            .wc_string(),
        wxString(_DT(L"LN(x)\t") + _(L"Natural logarithm of x (base Euler).")).wc_string(),
        wxString(_DT(L"LOG10(x)\t") + _(L"Common logarithm of x (base 10).")).wc_string(),
        wxString(_DT(L"SQRT(x)\t") + _(L"Square root of x.")).wc_string(),
        wxString(_DT(L"ABS(x)\t") + _(L"Absolute value of x.")).wc_string(),
        wxString(_DT(L"SIGN(x)\t") + _(L"Returns the sign of x. For example:") +
                 formatCodeExample(_DT(L"\'x&lt;0\' = -1<br />\'x=0\' = 0<br />\'x&gt;0\' = 1")))
            .wc_string(),
        wxString(_DT(L"TRUNC(x)\t") + _(L"Discards the fractional part of a number. For example:") +
                 formatCodeExample(_DT(L" TRUNC(-3.2) = -3<br />TRUNC(3.2) = 3")))
            .wc_string(),
        wxString(_DT(L"CEIL(x)\t") +
                 _(L"Returns the smallest integer not less than x. For example:") +
                 formatCodeExample(_DT(L"CEIL(-3.2) = -3<br />CEIL(3.2) = 4")))
            .wc_string(),
        wxString(_DT(L"FLOOR(x)\t") +
                 _(L"Returns the largest integer not greater than x. For example:") +
                 formatCodeExample(_DT(L"FLOOR(-3.2) = -4<br />FLOOR(3.2) = 3")))
            .wc_string(),
        wxString(FormulaFormat::FormatMathExpressionFromUS(_DT(L"ROUND(x,n)\t")) +
                 _(L"Returns the number x rounded to n decimal places. "
                   "(n is optional and defaults to zero.) For example:") +
                 formatCodeExample(_DT(L"ROUND(-11.6) = 12<br />ROUND(1.5, 0) = 2<br />"
                                       "ROUND(1.55, 1) = 1.6<br />ROUND(3.1415, 3) = 3.142")))
            .wc_string(),
        wxString(_DT(L"RAND()\t") +
                 _(L"Generates a random floating point number within the range of 0 and 1."))
            .wc_string(),
        wxString(FormulaFormat::FormatMathExpressionFromUS(_DT(L"POWER(Base,Exponent)\t")) +
                 _(L"The Power function raises Base to any power. "
                   "For fractional exponents, Base must be greater than 0."))
            .wc_string(),
        wxString(FormulaFormat::FormatMathExpressionFromUS(_DT(L"MIN(value,value2,...)\t")) +
                 _(L"Returns the lowest value from a specified range of values."))
            .wc_string(),
        wxString(FormulaFormat::FormatMathExpressionFromUS(_DT(L"MAX(value,value2,...)\t")) +
                 _(L"Returns the highest value from a specified range of values."))
            .wc_string()
    };

    m_statistics = {
        wxString(FormulaFormat::FormatMathExpressionFromUS(_DT(L"SUM(value,value2,...)\t")) +
                 _(L"Returns the sum of a specified range of values."))
            .wc_string(),
        wxString(FormulaFormat::FormatMathExpressionFromUS(_DT(L"AVERAGE(value,value2,...)\t")) +
                 _(L"Returns the average of a specified range of values."))
            .wc_string()
    };

    m_customFamiliarWords = {
        wxString(ReadabilityFormulaParser::GetCustomNewDaleChallSignature() + L"\t" +
                 _(L"Performs a New Dale-Chall test with a custom familiar word list. "
                   "Note that this test will use the same text exclusion rules as the Dale-Chall "
                   "test (overriding your system defaults)."))
            .wc_string(),
        wxString(ReadabilityFormulaParser::GetCustomSpacheSignature() + L"\t" +
                 _(L"Performs a Spache Revised test with a custom familiar word list."))
            .wc_string(),
        wxString(ReadabilityFormulaParser::GetCustomHarrisJacobsonSignature() + L"\t" +
                 _(L"Performs a Harris-Jacobson test with a custom familiar word list. "
                   "Note that this test will use the same text exclusion rules as the "
                   "Harris-Jacobson test (overriding your system defaults)."))
            .wc_string()
    };

    m_generalDocumentStatistics = {
        wxString(
            _DT(L"SyllableCount()\t") +
            // TRANSLATORS: "Default" should not be translated in here, it is a program keyword.
            _(L"Returns the number of syllables from the document.\n\n"
              "This function takes an argument specifying which numeral syllabizing method to use. "
              "These values are: Default, NumeralsFullySyllabized, and NumeralsAreOneSyllable"))
            .wc_string(),
        wxString(
            _DT(L"CharacterCount()\t") +
            // TRANSLATORS: "Default" should not be translated in here, it is a program keyword.
            _(L"Returns the number of characters (i.e., letters and numbers) from the document.\n\n"
              "This function takes an argument specifying which text exclusion method to use. "
              "These values are: Default, DaleChall, and HarrisJacobson"))
            .wc_string(),
        wxString(_DT(L"CharacterPlusPunctuationCount()\t") +
                 _(L"Returns the number of characters (i.e., letters and numbers) "
                   "and punctuation from the document.\n\n"
                   "Note that sentence-ending punctuation is not included in this count."))
            .wc_string()
    };

    m_wordFunctions = {
        wxString(_DT(L"FamiliarWordCount()\t") +
                 _(L"Returns the number of familiar words (from a custom list) the document."))
            .wc_string(),
        wxString(_DT(L"UnfamiliarWordCount()\t") +
                 _(L"Returns the number of unfamiliar words (from a custom list) the document."))
            .wc_string(),
        wxString(_DT(L"UniqueUnfamiliarWordCount()\t") +
                 _(L"Returns the number of unique unfamiliar words (from a custom list) "
                   "from the document."))
            .wc_string(),
        wxString(_DT(L"UnfamiliarHarrisJacobsonWordCount()\t") +
                 _(L"Returns the number of unfamiliar Harris-Jacobson words from the document."))
            .wc_string(),
        wxString(
            _DT(L"UniqueUnfamiliarHarrisJacobsonWordCount()\t") +
            _(L"Returns the number of unique unfamiliar Harris-Jacobson words from the document."))
            .wc_string(),
        wxString(_DT(L"FamiliarHarrisJacobsonWordCount()\t") +
                 _(L"Returns the number of familiar Harris-Jacobson words from the document."))
            .wc_string(),
        wxString(_DT(L"UnfamiliarDaleChallWordCount()\t") +
                 _(L"Returns the number of unfamiliar New Dale-Chall words from the document."))
            .wc_string(),
        wxString(
            _DT(L"UniqueUnfamiliarDaleChallWordCount()\t") +
            _(L"Returns the number of unique unfamiliar New Dale-Chall words from the document."))
            .wc_string(),
        wxString(_DT(L"FamiliarDaleChallWordCount()\t") +
                 _(L"Returns the number of familiar Dale-Chall words from the document."))
            .wc_string(),
        wxString(_DT(L"UnfamiliarSpacheWordCount()\t") +
                 _(L"Returns the number of unfamiliar Spache words from the document."))
            .wc_string(),
        wxString(_DT(L"UniqueUnfamiliarSpacheWordCount()\t") +
                 _(L"Returns the number of unique unfamiliar Spache words from the document."))
            .wc_string(),
        wxString(_DT(L"FamiliarSpacheWordCount()\t") +
                 _(L"Returns the number of familiar Spache words from the document."))
            .wc_string(),
        wxString(_DT(L"SixCharacterPlusWordCount()\t") +
                 _(L"Returns the number of words consisting of six or more "
                   "characters from the document."))
            .wc_string(),
        wxString(_DT(L"SevenCharacterPlusWordCount()\t") +
                 _(L"Returns the number of words consisting of seven or more "
                   "characters from the document."))
            .wc_string(),
        wxString(_DT(L"OneSyllableWordCount()\t") +
                 _(L"Returns the number of monosyllabic words from the document."))
            .wc_string(),
        wxString(
            _DT(L"ThreeSyllablePlusWordCount()\t") +
            // TRANSLATORS: "Default" should not be translated in here, it is a program keyword.
            _(L"Returns the number of words consisting of three or more syllables from the "
              "document.\n\nThis function takes an argument specifying which numeral "
              "syllabizing method to use. These values are: Default or NumeralsFullySyllabized"))
            .wc_string(),
        wxString(
            _DT(L"WordCount()\t") +
            // TRANSLATORS: "Default" should not be translated in here, it is a program keyword.
            _(L"Returns the number of words from the document.\n\n"
              "This function takes an argument specifying which text exclusion method to use. "
              "These values are: Default, DaleChall, and HarrisJacobson"))
            .wc_string(),
        wxString(_DT(L"NumeralCount()\t") + _(L"Returns the number of numerals from the document."))
            .wc_string(),
        wxString(_DT(L"ProperNounCount()\t") +
                 _(L"Returns the number of proper nouns from the document."))
            .wc_string(),
        wxString(_DT(L"UniqueWordCount()\t") +
                 _(L"Returns the number of unique words from the document."))
            .wc_string(),
        wxString(_DT(L"UniqueSixCharacterPlusWordCount()\t") +
                 _(L"Returns the number of unique words consisting of six or more characters "
                   "from the document."))
            .wc_string(),
        wxString(
            _DT(L"UniqueThreeSyllablePlusWordCount()\t") +
            // TRANSLATORS: "Default" should not be translated in here, it is a program keyword.
            _(L"Returns the number of unique words consisting of three or more syllables from the "
              "document.\n\nThis function takes an argument specifying which numeral syllabizing "
              "method to use. These values are: Default or NumeralsFullySyllabized"))
            .wc_string(),
        wxString(_DT(L"UniqueOneSyllableWordCount()\t") +
                 _(L"Returns the number of unique monosyllabic words from the document."))
            .wc_string(),
        wxString(_DT(L"HardFogWordCount()\t") +
                 // TRANSLATORS: "Gunning Fog" should not be translated.
                 _(L"Returns the number of difficult Gunning Fog words."))
            .wc_string(),
        wxString(_DT(L"MiniWordCount()\t") +
                 _(L"Returns the number of miniwords from the document."))
            .wc_string()
    };

    m_sentenceFunctions = {
        wxString(
            _DT(L"SentenceCount()\t") +
            // TRANSLATORS: "Default" should not be translated in here, it is a program keyword.
            _(L"Returns the number of sentences from the document.\n\n"
              "This function takes an argument specifying which text exclusion method to use. "
              "These values are: Default, DaleChall, HarrisJacobson, and GunningFog"))
            .wc_string(),
        wxString(_DT(L"IndependentClauseCount()\t") +
                 _(L"Returns the number of units/independent clauses from the document.") +
                 _DT(L"\n\n") + ReadabilityMessages::GetUnitNote())
            .wc_string()
    };

    m_shortcuts = {
        wxString(_DT(L"B\t") +
                 wxString::Format(_(L"Shortcut for %s."), _DT(L"SyllableCount(Default)")))
            .wc_string(),
        wxString(_DT(L"C\t") + wxString::Format(_(L"Shortcut for %s."),
                                                _DT(L"ThreeSyllablePlusWordCount(Default)")))
            .wc_string(),
        wxString(_DT(L"D\t") +
                 wxString::Format(_(L"Shortcut for %s."), _DT(L"FamiliarDaleChallWordCount()")))
            .wc_string(),
        wxString(_DT(L"F\t") + wxString::Format(_(L"Shortcut for %s."), _DT(L"HardFogWordCount()")))
            .wc_string(),
        wxString(_DT(L"L\t") +
                 wxString::Format(_(L"Shortcut for %s."), _DT(L"SixCharacterPlusWordCount()")))
            .wc_string(),
        wxString(_DT(L"M\t") +
                 wxString::Format(_(L"Shortcut for %s."), _DT(L"OneSyllableWordCount()")))
            .wc_string(),
        wxString(_DT(L"R\t") +
                 wxString::Format(_(L"Shortcut for %s."), _DT(L"CharacterCount(Default)")))
            .wc_string(),
        wxString(_DT(L"RP\t") +
                 wxString::Format(_(L"Shortcut for %s."), _DT(L"CharacterPlusPunctuationCount()")))
            .wc_string(),
        wxString(_DT(L"S\t") +
                 wxString::Format(_(L"Shortcut for %s."), _DT(L"SentenceCount(Default)")))
            .wc_string(),
        wxString(_DT(L"U\t") +
                 wxString::Format(_(L"Shortcut for %s."), _DT(L"IndependentClauseCount()")))
            .wc_string(),
        wxString(_DT(L"UDC\t") +
                 wxString::Format(_(L"Shortcut for %s."), _DT(L"UnfamiliarDaleChallWordCount()")))
            .wc_string(),
        wxString(_DT(L"UUS\t") + wxString::Format(_(L"Shortcut for %s."),
                                                  _DT(L"UniqueUnfamiliarSpacheWordCount()")))
            .wc_string(),
        wxString(_DT(L"W\t") + wxString::Format(_(L"Shortcut for %s."), _DT(L"WordCount(Default)")))
            .wc_string(),
        wxString(_DT(L"X\t") +
                 wxString::Format(_(L"Shortcut for %s."), _DT(L"SevenCharacterPlusWordCount()")))
            .wc_string(),
        wxString(_DT(L"T\t") + wxString::Format(_(L"Shortcut for %s."), _DT(L"MiniWordCount()")))
            .wc_string()
    };

    CreateControls();
    Centre();

    TransferDataToWindow();

    m_sideBarBook->SetSelection(0);

    Bind(wxEVT_HELP, &CustomTestDlg::OnContextHelp, this);
    Bind(wxEVT_BUTTON, &CustomTestDlg::OnHelp, this, wxID_HELP);
    Bind(wxEVT_BUTTON, &CustomTestDlg::OnOK, this, wxID_OK);
    Bind(
        wxEVT_BUTTON,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            TransferDataFromWindow();
            ValidateFormula(true);
        },
        CustomTestDlg::ID_VALIDATE_FORMULA_BUTTON);
    Bind(
        wxEVT_BUTTON,
        [this]([[maybe_unused]] wxCommandEvent&)
        {
            if (m_funcBrowserSizer != nullptr)
                {
                ShowFunctionBrowser(!m_funcBrowserSizer->IsShown(size_t{ 0 }));
                }
        },
        CustomTestDlg::ID_FUNCTION_BROWSER_BUTTON);

    return true;
    }

//------------------------------------------------------
void CustomTestDlg::ShowFunctionBrowser(const bool show)
    {
    if (m_generalPage != nullptr && m_generalPage->GetSizer() != nullptr)
        {
        Freeze();

        m_generalPage->GetSizer()->Show(m_funcBrowserSizer, show, true);
        m_generalPage->GetSizer()->Layout();
        m_generalPage->GetSizer()->Fit(m_generalPage);
        m_sideBarBook->InvalidateBestSize();
        GetSizer()->SetMinSize(GetSizer()->CalcMin());
        Layout();
        Fit();

        Thaw();
        Refresh();
        }
    }

//------------------------------------------------------
void CustomTestDlg::CreateControls()
    {
    auto* mainSizer = new wxBoxSizer(wxVERTICAL);

    m_sideBarBook = new Wisteria::UI::SideBarBook(this, wxID_ANY);

    wxGetApp().UpdateSideBarTheme(m_sideBarBook->GetSideBar());

    mainSizer->Add(m_sideBarBook, wxSizerFlags{ 1 }.Expand().Border());

    m_sideBarBook->GetImageList().push_back(wxGetApp().GetResourceManager().GetSVG(
        wxSystemSettings::GetAppearance().IsDark() ? L"ribbon/bullet-dark.svg" :
                                                     L"ribbon/bullet.svg"));

        // general page
        {
        m_generalPage = new wxPanel(m_sideBarBook, ID_GENERAL_PAGE, wxDefaultPosition,
                                    wxDefaultSize, wxTAB_TRAVERSAL);
        auto* editorSectionSizer = new wxBoxSizer(wxVERTICAL);
        auto* mainPageSizer = new wxBoxSizer(wxHORIZONTAL);
        mainPageSizer->Add(editorSectionSizer, wxSizerFlags{ 1 }.Expand());
        m_generalPage->SetSizer(mainPageSizer);
        m_sideBarBook->AddPage(m_generalPage, _(L"General Settings"), ID_GENERAL_PAGE, true);

        // if no test name then we are in "add new test" mode
        if (m_testName.empty())
            {
            auto* nameBoxSizer = new wxStaticBoxSizer(
                new wxStaticBox(m_generalPage, wxID_ANY, _(L"Test name:")), wxVERTICAL);
            editorSectionSizer->Add(nameBoxSizer, wxSizerFlags{}.Expand().Border());

            m_testNameCtrl = new wxTextCtrl(nameBoxSizer->GetStaticBox(), ID_TEST_NAME_FIELD,
                                            wxString{}, wxDefaultPosition, wxDefaultSize, 0,
                                            wxTextValidator(wxFILTER_NONE, &m_testName));
            nameBoxSizer->Add(m_testNameCtrl, wxSizerFlags{ 1 }.Expand().Border());
            editorSectionSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            }
            // test result type
            {
            auto* testTypeSizer = new wxBoxSizer(wxHORIZONTAL);
            editorSectionSizer->Add(testTypeSizer, wxSizerFlags{}.Expand().Border());
            testTypeSizer->Add(
                new wxStaticText(m_generalPage, wxID_STATIC, _(L"Test result type:")), 0,
                wxALIGN_CENTER_VERTICAL);
            testTypeSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

            m_testTypeCombo =
                new wxComboBox(m_generalPage, ID_TEST_TYPE_COMBO, wxString{}, wxDefaultPosition,
                               wxDefaultSize, m_testTypes, wxCB_DROPDOWN | wxCB_READONLY);
            m_testTypeCombo->SetSelection(0);
            testTypeSizer->Add(m_testTypeCombo);
            }
            // formula editor
            {
            auto* functionControlsSizer = new wxBoxSizer(wxHORIZONTAL);
            editorSectionSizer->Add(functionControlsSizer, wxSizerFlags{ 1 }.Expand().Border());
            auto* formulaBoxSizer = new wxStaticBoxSizer(
                new wxStaticBox(m_generalPage, wxID_ANY, _(L"Formula:")), wxVERTICAL);
            functionControlsSizer->Add(formulaBoxSizer, wxSizerFlags{ 1 }.Expand());

            auto* formulaButtonsSizer = new wxBoxSizer(wxHORIZONTAL);

            formulaButtonsSizer->Add(
                new wxBitmapButton(formulaBoxSizer->GetStaticBox(), ID_VALIDATE_FORMULA_BUTTON,
                                   wxGetApp().GetResourceManager().GetSVG(L"ribbon/check.svg")));

            formulaButtonsSizer->Add(new wxBitmapButton(
                formulaBoxSizer->GetStaticBox(), ID_FUNCTION_BROWSER_BUTTON,
                wxGetApp().GetResourceManager().GetSVG(wxSystemSettings::GetAppearance().IsDark() ?
                                                           L"ribbon/function-dark.svg" :
                                                           L"ribbon/function.svg")));

            formulaBoxSizer->Add(formulaButtonsSizer,
                                 wxSizerFlags{}.Border(wxLEFT | wxTOP | wxRIGHT));
            formulaBoxSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

            m_formulaCtrl = new Wisteria::UI::CodeEditor(
                formulaBoxSizer->GetStaticBox(), wxSTC_LEX_CPPNOCASE, ID_FORMULA_FIELD,
                wxDefaultPosition, FromDIP(wxSize{ 600, 300 }));

            m_formulaCtrl->AddFunctionsOrClasses(m_math);
            m_formulaCtrl->AddFunctionsOrClasses(m_logic);
            m_formulaCtrl->AddFunctionsOrClasses(m_statistics);
            m_formulaCtrl->AddFunctionsOrClasses(m_customFamiliarWords);
            m_formulaCtrl->AddFunctionsOrClasses(m_generalDocumentStatistics);
            m_formulaCtrl->AddFunctionsOrClasses(m_wordFunctions);
            m_formulaCtrl->AddFunctionsOrClasses(m_sentenceFunctions);
            m_formulaCtrl->AddFunctionsOrClasses(m_shortcuts);
            m_formulaCtrl->Finalize();
            formulaBoxSizer->Add(m_formulaCtrl, wxSizerFlags{ 1 }.Expand().Border());

            // examples labels
            formulaBoxSizer->Add(new wxStaticText(formulaBoxSizer->GetStaticBox(), wxID_STATIC,
                                                  _(L"Enter a formula, such as:")));
            auto* formulaExample = new wxStaticText(
                formulaBoxSizer->GetStaticBox(), wxID_STATIC,
                FormulaFormat::FormatMathExpressionFromUS(
                    _DT(L"ROUND(206.835 - (84.6*(SyllableCount(Default)/WordCount(Default))) -\n"
                        "(1.015*(WordCount(Default)/SentenceCount(Default))))")));
            formulaExample->SetFont(
                wxFont(wxFontInfo().Family(wxFontFamily::wxFONTFAMILY_TELETYPE).Bold(true)));
            formulaBoxSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            formulaBoxSizer->Add(formulaExample, wxSizerFlags{}.DoubleBorder(wxLEFT));
            formulaBoxSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

            formulaBoxSizer->Add(
                new wxStaticText(formulaBoxSizer->GetStaticBox(), wxID_STATIC,
                                 _(L"or a function representing a familiar-word test:")));
            formulaExample =
                new wxStaticText(formulaBoxSizer->GetStaticBox(), wxID_STATIC,
                                 ReadabilityFormulaParser::GetCustomNewDaleChallSignature());
            formulaExample->SetFont(
                wxFont(wxFontInfo().Family(wxFontFamily::wxFONTFAMILY_TELETYPE).Bold(true)));
            formulaBoxSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            formulaBoxSizer->Add(formulaExample, wxSizerFlags{}.DoubleBorder(wxLEFT));
            }
            // function browser
            {
            auto* functionBrowser =
                new Wisteria::UI::FunctionBrowserCtrl(m_generalPage, m_formulaCtrl);
            wxGetApp().UpdateSideBarTheme(functionBrowser->GetSidebar());
            functionBrowser->Hide();
            functionBrowser->SetParameterSeparator(FormulaFormat::GetListSeparator());

            functionBrowser->AddCategory(_(L"Operators").wc_string(), m_operators);
            functionBrowser->AddCategory(_(L"Logic").wc_string(), m_logic);
            functionBrowser->AddCategory(_(L"Math").wc_string(), m_math);
            functionBrowser->AddCategory(_(L"Statistics").wc_string(), m_statistics);
            functionBrowser->AddCategory(_(L"Custom Familiar Word Tests").wc_string(),
                                         m_customFamiliarWords);
            functionBrowser->AddCategory(_(L"Syllable/Character Counts").wc_string(),
                                         m_generalDocumentStatistics);
            functionBrowser->AddCategory(_(L"Word Counts").wc_string(), m_wordFunctions);
            functionBrowser->AddCategory(_(L"Sentence Counts").wc_string(), m_sentenceFunctions);
            functionBrowser->AddCategory(_(L"Shortcuts").wc_string(), m_shortcuts);

            functionBrowser->FinalizeCategories();

            m_funcBrowserSizer = new wxBoxSizer(wxHORIZONTAL);
            m_funcBrowserSizer->Add(functionBrowser, wxSizerFlags{}.Expand().Border());

            mainPageSizer->Add(m_funcBrowserSizer, wxSizerFlags{}.Expand());
            }
        }

        // Familiar word options
        {
        auto* wordListPanel = new wxPanel(m_sideBarBook, ID_WORD_LIST_PAGE, wxDefaultPosition,
                                          wxDefaultSize, wxTAB_TRAVERSAL);
        m_sideBarBook->AddPage(wordListPanel, _(L"Familiar Words"), ID_WORD_LIST_PAGE, false);

            // word lists
            {
            auto* panelSizer = new wxBoxSizer(wxVERTICAL);
            wordListPanel->SetSizer(panelSizer);
            m_sideBarBook->AddSubPage(wordListPanel, _(L"Word Lists"), ID_WORD_LIST_PAGE, false, 0);

            auto* pgMan = new wxPropertyGridManager(
                wordListPanel, ID_WORD_LIST_PROPERTY_GRID, wxDefaultPosition, wxDefaultSize,
                wxPG_BOLD_MODIFIED | wxPG_DESCRIPTION | wxPGMAN_DEFAULT_STYLE);
            m_wordListsPropertyGrid = pgMan->AddPage();
            // custom word list
            m_wordListsPropertyGrid->Append(
                new wxPropertyCategory(GetCustomFamiliarWordListLabel()));
            m_wordListsPropertyGrid->Append(
                new wxBoolProperty(GetIncludeCustomListLabel(), wxPG_LABEL, false));
            m_wordListsPropertyGrid->SetPropertyAttribute(GetIncludeCustomListLabel(),
                                                          wxPG_BOOL_USE_CHECKBOX, true);
            m_wordListsPropertyGrid->SetPropertyHelpString(
                GetIncludeCustomListLabel(), _(L"Check this option to use your own word list to "
                                               "determine if a word is familiar."));

            m_wordListsPropertyGrid->Append(
                new WordListProperty(GetFileContainingFamiliarWordsLabel(), wxPG_LABEL));
            m_wordListsPropertyGrid->SetPropertyAttribute(GetFileContainingFamiliarWordsLabel(),
                                                          wxPG_DIALOG_TITLE, _(L"Edit Word List"));
            m_wordListsPropertyGrid->SetPropertyHelpString(
                GetFileContainingFamiliarWordsLabel(),
                _(L"Enter the path to the familiar-word list into this field. "
                  "This list must be a text file where each word is "
                  "separated by a space, tab, comma, semicolon, or new line."));

            wxPGChoices stemLanguages;
            for (size_t i = 0;
                 i < static_cast<size_t>(stemming::stemming_type::STEMMING_TYPE_COUNT); ++i)
                {
                stemLanguages.Add(ProjectReportFormat::GetStemmingDisplayName(
                    static_cast<stemming::stemming_type>(i)));
                }
            m_wordListsPropertyGrid->Append(
                new wxEnumProperty(GetStemmingLanguageLabel(), wxPG_LABEL, stemLanguages, 0));
            m_wordListsPropertyGrid->SetPropertyHelpString(
                GetStemmingLanguageLabel(), _(L"Select from this list the stemming method (if any) "
                                              "to use when comparing your familiar "
                                              "words with the words in a document."));

            // standard word lists
            m_wordListsPropertyGrid->Append(new wxPropertyCategory(GetStandardWordListsLabel()));
            m_wordListsPropertyGrid->Append(
                new wxBoolProperty(GetIncludeDCWordListLabel(), wxPG_LABEL, false));
            m_wordListsPropertyGrid->SetPropertyAttribute(GetIncludeDCWordListLabel(),
                                                          wxPG_BOOL_USE_CHECKBOX, true);
            m_wordListsPropertyGrid->SetPropertyHelpString(
                GetIncludeDCWordListLabel(),
                // TRANSLATORS: "New Dale-Chall" should not be translated.
                _(L"Check this option to use the New Dale-Chall word list "
                  "(along with any other selected word lists) "
                  "to determine if a word is familiar."));

            m_wordListsPropertyGrid->Append(
                new wxBoolProperty(GetIncludeSpacheWordListLabel(), wxPG_LABEL, false));
            m_wordListsPropertyGrid->SetPropertyAttribute(GetIncludeSpacheWordListLabel(),
                                                          wxPG_BOOL_USE_CHECKBOX, true);
            m_wordListsPropertyGrid->SetPropertyHelpString(
                GetIncludeSpacheWordListLabel(),
                // TRANSLATORS: "Spache Revised" should not be translated.
                _(L"Check this option to use the Spache Revised word list "
                  "(along with any other selected word lists) "
                  "to determine if a word is familiar."));

            m_wordListsPropertyGrid->Append(
                new wxBoolProperty(GetIncludeHJWordListLabel(), wxPG_LABEL, false));
            m_wordListsPropertyGrid->SetPropertyAttribute(GetIncludeHJWordListLabel(),
                                                          wxPG_BOOL_USE_CHECKBOX, true);
            m_wordListsPropertyGrid->SetPropertyHelpString(
                GetIncludeHJWordListLabel(),
                _(L"Check this option to use the Harris-Jacobson word list "
                  "(along with any other selected word lists) "
                  "to determine if a word is familiar."));

            m_wordListsPropertyGrid->Append(
                new wxBoolProperty(GetIncludeStockerWordListLabel(), wxPG_LABEL, false));
            m_wordListsPropertyGrid->SetPropertyAttribute(GetIncludeStockerWordListLabel(),
                                                          wxPG_BOOL_USE_CHECKBOX, true);
            m_wordListsPropertyGrid->SetPropertyHelpString(
                GetIncludeStockerWordListLabel(),
                _(L"Check this option to include Stocker's supplementary word list "
                  "for Catholic students (along with any other selected word lists) "
                  "to determine if a word is familiar."));

            // other options
            m_wordListsPropertyGrid->Append(new wxPropertyCategory(GetOtherLabel()));
            m_wordListsPropertyGrid->Append(
                new wxBoolProperty(GetFamiliarWordsOnAllLabel(), wxPG_LABEL, false));
            m_wordListsPropertyGrid->SetPropertyAttribute(GetFamiliarWordsOnAllLabel(),
                                                          wxPG_BOOL_USE_CHECKBOX, true);
            m_wordListsPropertyGrid->SetPropertyHelpString(
                GetFamiliarWordsOnAllLabel(),
                _(L"Check this option to consider words familiar only if they appear on your "
                  "custom list and other selected lists.\n\n"
                  "This option is only recommended for special situations where you only want "
                  "to find words that appear within a union "
                  "of your word list and another list (e.g., Spache)."));

            pgMan->SetDescBoxHeight(FromDIP(wxSize(125, 125)).GetHeight());

            pgMan->SelectProperty(GetCustomFamiliarWordListLabel());

            Connect(pgMan->GetId(), wxEVT_PG_CHANGED,
                    wxPropertyGridEventHandler(CustomTestDlg::OnPropertyGridChange));

            panelSizer->Add(pgMan, wxSizerFlags{ 1 }.Expand());
            }
            // proper nouns and numbers
            {
            auto* panel = new wxPanel(m_sideBarBook, ID_PROPER_NUMERALS_PAGE, wxDefaultPosition,
                                      wxDefaultSize, wxTAB_TRAVERSAL);
            auto* panelSizer = new wxBoxSizer(wxVERTICAL);
            panel->SetSizer(panelSizer);
            m_sideBarBook->AddSubPage(panel, _(L"Proper Nouns & Numerals"), ID_PROPER_NUMERALS_PAGE,
                                      false, 0);

            auto* pgMan = new wxPropertyGridManager(
                panel, ID_PROPER_NOUN_PROPERTY_GRID, wxDefaultPosition, wxDefaultSize,
                wxPG_BOLD_MODIFIED | wxPG_DESCRIPTION | wxPGMAN_DEFAULT_STYLE);
            m_properNounsNumbersPropertyGrid = pgMan->AddPage();
            // proper nouns
            m_properNounsNumbersPropertyGrid->Append(new wxPropertyCategory(GetProperNounsLabel()));
            wxPGChoices properNounMethods;
            properNounMethods.Add(_(L"Count as unfamiliar"));
            properNounMethods.Add(_(L"Count as familiar"));
            properNounMethods.Add(_(L"Count only first occurrence of each as unfamiliar"));
            m_properNounsNumbersPropertyGrid->Append(
                new wxEnumProperty(GetFamiliarityLabel(), wxPG_LABEL, properNounMethods, 1));
            m_properNounsNumbersPropertyGrid->SetPropertyHelpString(
                GetFamiliarityLabel(),
                _(L"Controls how proper nouns are handled in terms of being familiar words."));
            // nouns
            m_properNounsNumbersPropertyGrid->Append(new wxPropertyCategory(GetNumeralsLabel()));
            m_properNounsNumbersPropertyGrid->Append(
                new wxBoolProperty(GetNumeralsAsFamiliarLabel(), wxPG_LABEL, true));
            m_properNounsNumbersPropertyGrid->SetPropertyAttribute(GetNumeralsAsFamiliarLabel(),
                                                                   wxPG_BOOL_USE_CHECKBOX, true);
            m_properNounsNumbersPropertyGrid->SetPropertyHelpString(
                GetNumeralsAsFamiliarLabel(),
                _(L"Check this to also consider numeric words as familiar."));

            pgMan->SelectProperty(GetProperNounsLabel());

            panelSizer->Add(pgMan, wxSizerFlags{ 1 }.Expand());
            }
        }

        // new project wizard options
        {
        auto* wizardPage = new wxPanel(m_sideBarBook, ID_CLASSIFICATION_PAGE, wxDefaultPosition,
                                       wxDefaultSize, wxTAB_TRAVERSAL);
        auto* wizardPageSizer = new wxBoxSizer(wxVERTICAL);
        wizardPage->SetSizer(wizardPageSizer);
        m_sideBarBook->AddPage(wizardPage, _(L"Classification"), ID_CLASSIFICATION_PAGE, false);

        m_associationPropertyGrid = new wxPropertyGrid(wizardPage, ID_CLASSIFICATION_PROPERTY_GRID);
        m_associationPropertyGrid->Append(new wxPropertyCategory(_(L"Associate with industry")));
        for (size_t i = 0; i < m_professionNames.GetCount(); ++i)
            {
            m_associationPropertyGrid->Append(
                new wxBoolProperty(m_professionNames[i], wxPG_LABEL, false));
            m_associationPropertyGrid->SetPropertyAttribute(m_professionNames[i],
                                                            wxPG_BOOL_USE_CHECKBOX, true);
            }
        m_associationPropertyGrid->Append(
            new wxPropertyCategory(_(L"Associate with document type")));
        for (size_t i = 0; i < m_documentNames.GetCount(); ++i)
            {
            m_associationPropertyGrid->Append(
                new wxBoolProperty(m_documentNames[i], wxPG_LABEL, false));
            m_associationPropertyGrid->SetPropertyAttribute(m_documentNames[i],
                                                            wxPG_BOOL_USE_CHECKBOX, true);
            }

        wizardPageSizer->Add(m_associationPropertyGrid, wxSizerFlags{ 1 }.Expand());
        }

    mainSizer->Add(CreateSeparatedButtonSizer(wxOK | wxCANCEL | wxHELP),
                   wxSizerFlags{}.Expand().Border());

    SetSizerAndFit(mainSizer);

    if (m_testNameCtrl != nullptr)
        {
        m_testNameCtrl->SetFocus();
        }

    UpdateOptions();

    if (m_wordListsPropertyGrid != nullptr)
        {
        m_wordListsPropertyGrid->FitColumns();
        }
    if (m_properNounsNumbersPropertyGrid != nullptr)
        {
        m_properNounsNumbersPropertyGrid->FitColumns();
        }
    if (m_associationPropertyGrid != nullptr)
        {
        m_associationPropertyGrid->FitColumns();
        }
    }

//---------------------------------------------
void CustomTestDlg::OnOK([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    // trim off whitespace off of test name and then validate it (if applicable)
    m_testName.Trim(true).Trim(false);

    // validate the formula
    if (!ValidateFormula())
        {
        return;
        }

    // validate the test name
    if (m_testNameCtrl != nullptr)
        {
        // if a new test, then make sure it isn't empty
        if (m_testName.empty())
            {
            wxMessageBox(_(L"Please enter a test name."), _(L"Error"), wxOK | wxICON_EXCLAMATION);
            return;
            }
        // or that the name is already taken
        if (std::find(BaseProject::m_custom_word_tests.cbegin(),
                      BaseProject::m_custom_word_tests.cend(),
                      m_testName) != BaseProject::m_custom_word_tests.cend())
            {
            wxMessageBox(wxString::Format(_(L"There is a test named \"%s\" already. "
                                            "Please enter a different name."),
                                          m_testName),
                         _(L"Error"), wxOK | wxICON_ERROR);
            return;
            }
        // check for the same name in the standard tests too
        BaseProject::TestCollectionType standardTests;
        BaseProject::ResetStandardReadabilityTests(standardTests);
        if (standardTests.has_test(m_testName))
            {
            wxMessageBox(
                wxString::Format(_(L"There is a standard test with the name \"%s\" already. "
                                   "Please enter a different name."),
                                 m_testName),
                _(L"Error"), wxOK | wxICON_ERROR);
            return;
            }
        // check against names of statistics
        if (m_testName.CmpNoCase(BaseProjectView::GetAverageLabel()) == 0 ||
            m_testName.CmpNoCase(BaseProjectView::GetMedianLabel()) == 0 ||
            m_testName.CmpNoCase(BaseProjectView::GetModeLabel()) == 0 ||
            m_testName.CmpNoCase(BaseProjectView::GetStdDevLabel()) == 0)
            {
            wxMessageBox(wxString::Format(_(L"The name \"%s\" is already in use as a statistic. "
                                            "Please enter a different name."),
                                          m_testName),
                         _(L"Error"), wxOK | wxICON_ERROR);
            return;
            }
        }
    if (IsIncludingCustomWordList() && !wxFile::Exists(GetWordListFilePath()))
        {
        wxMessageBox(_(L"Familiar word list file not found."), _(L"Error"), wxOK | wxICON_ERROR);
        return;
        }

    // ordering of this combobox doesn't match up exactly with
    // enum that it reflects, so handle that here
    switch (m_testTypeCombo->GetSelection())
        {
    case 0:
        m_testType = 0;
        break;
    case 1:
        m_testType = 1;
        break;
    // we don't use all the values in the readability_test_type enum
    case 2:
        m_testType = 3;
        break;
    default:
        m_testType = 0;
        };

    if (GetStemmingType() == stemming::stemming_type::german &&
        static_cast<readability::proper_noun_counting_method>(GetProperNounMethod()) !=
            readability::proper_noun_counting_method::all_proper_nouns_are_unfamiliar)
        {
        auto warningIter = WarningManager::GetWarning(_DT(L"german-no-proper-noun-support"));
        if (warningIter != WarningManager::GetWarnings().end() && warningIter->ShouldBeShown())
            {
            wxMessageBox(warningIter->GetMessage(), wxGetApp().GetAppName(),
                         warningIter->GetFlags(), this);
            }
        SetProperNounMethod(static_cast<int>(
            readability::proper_noun_counting_method::all_proper_nouns_are_unfamiliar));
        TransferDataToWindow();
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

//-------------------------------------------------------------
void CustomTestDlg::SetStemmingType(stemming::stemming_type stemType)
    {
    m_wordListsPropertyGrid->SetPropertyValue(GetStemmingLanguageLabel(),
                                              static_cast<int>(stemType));
    }

//-------------------------------------------------------------
void CustomTestDlg::SelectPage(const wxWindowID pageId)
    {
    for (size_t i = 0; i < m_sideBarBook->GetPageCount(); ++i)
        {
        const wxWindow* page = m_sideBarBook->GetPage(i);
        if ((page != nullptr) && page->GetId() == pageId)
            {
            m_sideBarBook->SetSelection(i);
            }
        }
    }

//-------------------------------------------------------------
void CustomTestDlg::OnPropertyGridChange(wxPropertyGridEvent& event)
    {
    if (event.GetProperty()->GetName() == GetIncludeCustomListLabel() ||
        event.GetProperty()->GetName() == GetIncludeDCWordListLabel() ||
        event.GetProperty()->GetName() == GetIncludeSpacheWordListLabel() ||
        event.GetProperty()->GetName() == GetIncludeHJWordListLabel() ||
        event.GetProperty()->GetName() == GetIncludeStockerWordListLabel())
        {
        UpdateOptions();
        }
    event.Skip();
    }
