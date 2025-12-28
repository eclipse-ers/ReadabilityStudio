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

#include "project_report_format.h"
#include "../app/readability_app.h"
#include "../projects/base_project_doc.h"
#include "../projects/base_project_view.h"

DECLARE_APP(ReadabilityApp)

//------------------------------------------------
wxString ProjectReportFormat::GetStemmingDisplayName(const stemming::stemming_type stemType)
    {
    switch (stemType)
        {
    case stemming::stemming_type::danish:
        return _(L"Danish");
        break;
    case stemming::stemming_type::dutch:
        return _(L"Dutch");
        break;
    case stemming::stemming_type::english:
        return _(L"English");
        break;
    case stemming::stemming_type::finnish:
        return _(L"Finnish");
        break;
    case stemming::stemming_type::french:
        return _(L"French");
        break;
    case stemming::stemming_type::german:
        return _(L"German");
        break;
    case stemming::stemming_type::italian:
        return _(L"Italian");
        break;
    case stemming::stemming_type::norwegian:
        return _(L"Norwegian");
        break;
    case stemming::stemming_type::portuguese:
        return _(L"Portuguese");
        break;
    case stemming::stemming_type::spanish:
        return _(L"Spanish");
        break;
    case stemming::stemming_type::swedish:
        return _(L"Swedish");
        break;
    case stemming::stemming_type::no_stemming:
    default:
        return _(L"Do not using stemming");
        break;
        }
    }

//------------------------------------------------
wxString ProjectReportFormat::FormatFormulaToHtml(const wxString& formula)
    {
    if (formula.empty())
        {
        return formula;
        }
    wxString formattedFormula = L"<tt>";
    const wxString wordFormatStart = L"<span>";
    const wxString operatorFormatStart = L"<span style='font-weight:bold;'>";
    for (size_t i = 0; i < formula.length(); /*handled in lop*/)
        {
        if (string_util::is_one_of<wchar_t>(
                formula[i], L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"))
            {
            const size_t endOfWord = formula.find_first_not_of(
                L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890", i);
            if (endOfWord == std::wstring::npos)
                {
                formattedFormula += wordFormatStart;
                formattedFormula += formula[i];
                formattedFormula += L"</span>";
                break;
                }
            formattedFormula += wordFormatStart;
            formattedFormula += formula.substr(i, endOfWord - i);
            formattedFormula += L"</span>";
            i = endOfWord;
            }
        else
            {
            if (string_util::is_one_of<wchar_t>(formula[i], L"-+*/%^,();"))
                {
                formattedFormula += operatorFormatStart;
                formattedFormula += formula[i];
                formattedFormula += L"</span>";
                }
            else if (formula[i] == L'<' && i + 1 < formula.length() && formula[i + 1] == L'=')
                {
                formattedFormula += operatorFormatStart + L"&le;</span>";
                ++i;
                }
            else if (formula[i] == L'>' && i + 1 < formula.length() && formula[i + 1] == L'=')
                {
                formattedFormula += operatorFormatStart + L"&ge;</span>";
                ++i;
                }
            else if (formula[i] == L'<')
                {
                formattedFormula += operatorFormatStart + L"&lt;</span>";
                }
            else if (formula[i] == L'>')
                {
                formattedFormula += operatorFormatStart + L"&gt;</span>";
                }
            else if (formula[i] == L'\'')
                {
                formattedFormula += L"&apos;";
                }
            else if (formula[i] == L'\"')
                {
                formattedFormula += L"&quot;";
                }
            else if (formula[i] == L' ')
                {
                formattedFormula += L"&nbsp;";
                }
            else if (formula[i] == 10 || formula[i] == 13)
                {
                // treats CRLF combo as one break, so make one extra step for CRLF combination
                // so that it counts as only one line break
                if (i < formula.length() - 1 && (formula[i + 1] == 10 || formula[i + 1] == 13))
                    {
                    ++i;
                    }
                formattedFormula += L"<br />";
                }
            else
                {
                formattedFormula += formula[i];
                }
            ++i;
            }
        }
    formattedFormula += L"</tt>";
    return formattedFormula;
    }

//------------------------------------------------
wxString ProjectReportFormat::FormatDolchStatisticsInfo(
    const BaseProject* project, const StatisticsReportInfo& statsInfo,
    const bool includeExplanation, const wxColour& attentionColor,
    const std::shared_ptr<Wisteria::UI::ListCtrlExDataProviderBase>& listData)
    {
    assert(project);
    if (project == nullptr)
        {
        return {};
        }
    if (!project->IsIncludingDolchSightWords())
        {
        return {};
        }
    size_t listDataItemCount = 0;
    if (listData)
        {
        listDataItemCount = listData->GetItemCount();
        listData->SetSize(listDataItemCount + MAX_SUMMARY_STAT_ROWS, 3);
        }

    const size_t totalDolchWords =
        project->GetDolchConjunctionCounts().second +
        project->GetDolchPrepositionWordCounts().second + project->GetDolchPronounCounts().second +
        project->GetDolchAdverbCounts().second + project->GetDolchAdjectiveCounts().second +
        project->GetDolchVerbsCounts().second + project->GetDolchNounCounts().second;
    const size_t totalDolchWordsExcludingNouns =
        project->GetDolchConjunctionCounts().second +
        project->GetDolchPrepositionWordCounts().second + project->GetDolchPronounCounts().second +
        project->GetDolchAdverbCounts().second + project->GetDolchAdjectiveCounts().second +
        project->GetDolchVerbsCounts().second;
    const double totalDolchPercentage =
        safe_divide<double>(totalDolchWords, project->GetTotalWords()) * 100;
    const double totalDolchExcludingNounsPercentage =
        safe_divide<double>(totalDolchWordsExcludingNouns, project->GetTotalWords()) * 100;

    const bool containsHighPercentageOfNonDolchWords =
        ((totalDolchPercentage < 70) || (totalDolchExcludingNounsPercentage < 60));
    wxString htmlText;

    if (!statsInfo.HasDolchStatisticsEnabled())
        {
        htmlText += _(L"No Dolch statistics <a href=\"#SelectStatistics\">currently selected</a>.");
        }

    wxBitmap bmp{ 100, 100 };
    wxMemoryDC measuringDc(bmp);
    measuringDc.SetFont(measuringDc.GetFont().Larger().Larger());
    // will likely be the longest label (even after translation)
    const auto labelColumnWidth = safe_divide<long>(
        measuringDc.GetTextExtent(_(L"Number of Dolch words (excluding nouns):")).GetWidth(),
        wxGetApp().GetDPIScaleFactor());
    // the total number of words would be the largest (i.e., widest)
    // number possible, so measure using that
    const auto numberColumnWidth = safe_divide<long>(
        measuringDc.GetTextExtent(std::to_wstring(project->GetTotalWords())).GetWidth(),
        wxGetApp().GetDPIScaleFactor());

    const wxString tableStart = L"<table class='minipage' border='0' style='width:100%'>";

    if (statsInfo.IsDolchCoverageEnabled())
        {
        // list completions
        htmlText += tableStart + FormatDolchHeader(_(L"Dolch Word Coverage"));

        const double dolchConjunctionPercentage =
            safe_divide<double>(
                (MAX_DOLCH_CONJUNCTION_WORDS - project->GetUnusedDolchConjunctions()),
                MAX_DOLCH_CONJUNCTION_WORDS) *
            100;
        const double dolchPronounsPercentage =
            safe_divide<double>((MAX_DOLCH_PRONOUN_WORDS - project->GetUnusedDolchPronouns()),
                                MAX_DOLCH_PRONOUN_WORDS) *
            100;
        const double dolchPrepositionsPercentage =
            safe_divide<double>(
                (MAX_DOLCH_PREPOSITION_WORDS - project->GetUnusedDolchPrepositions()),
                MAX_DOLCH_PREPOSITION_WORDS) *
            100;
        const double dolchAdverbsPercentage =
            safe_divide<double>((MAX_DOLCH_ADVERB_WORDS - project->GetUnusedDolchAdverbs()),
                                MAX_DOLCH_ADVERB_WORDS) *
            100;
        const double dolchAdjectivesPercentage =
            safe_divide<double>((MAX_DOLCH_ADJECTIVE_WORDS - project->GetUnusedDolchAdjectives()),
                                MAX_DOLCH_ADJECTIVE_WORDS) *
            100;
        const double dolchVerbsPercentage =
            safe_divide<double>((MAX_DOLCH_VERBS - project->GetUnusedDolchVerbs()),
                                MAX_DOLCH_VERBS) *
            100;
        const double dolchNounPercentage =
            safe_divide<double>((MAX_DOLCH_NOUNS - project->GetUnusedDolchNouns()),
                                MAX_DOLCH_NOUNS) *
            100;

            // Conjunctions
            {
            const wxString valueStr =
                (dolchConjunctionPercentage >= 75) ?
                    wxString::Format(L"<span style=\"color:%s\">",
                                     attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                        wxNumberFormatter::ToString(
                            MAX_DOLCH_CONJUNCTION_WORDS - project->GetUnusedDolchConjunctions(), 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                wxNumberFormatter::Style::Style_WithThousandsSep) +
                        L"</span>" :
                    wxNumberFormatter::ToString(
                        MAX_DOLCH_CONJUNCTION_WORDS - project->GetUnusedDolchConjunctions(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr =
                (dolchConjunctionPercentage >= 75) ?
                    wxString::Format(L"<span style=\"color:%s\">",
                                     attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                        wxString::Format(_(L"(%s%% of all Dolch conjunctions)"),
                                         wxNumberFormatter::ToString(
                                             dolchConjunctionPercentage, 1,
                                             wxNumberFormatter::Style::Style_NoTrailingZeroes)) +
                        L"</span>" :
                    wxString::Format(_(L"(%s%% of all Dolch conjunctions)"),
                                     wxNumberFormatter::ToString(
                                         dolchConjunctionPercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes));
            htmlText += FormatDolchRow(labelColumnWidth, numberColumnWidth,
                                       _(L"Conjunctions used:"), valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of conjunctions used"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount, 1,
                    wxNumberFormatter::ToString(
                        MAX_DOLCH_CONJUNCTION_WORDS - project->GetUnusedDolchConjunctions(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 2,
                    wxString::Format(_(L"%s%% of all Dolch conjunctions"),
                                     wxNumberFormatter::ToString(
                                         dolchConjunctionPercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes)),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }
            // Prepositions
            {
            const wxString valueStr =
                (dolchPrepositionsPercentage >= 75) ?
                    wxString::Format(L"<span style=\"color:%s\">",
                                     attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                        wxNumberFormatter::ToString(
                            MAX_DOLCH_PREPOSITION_WORDS - project->GetUnusedDolchPrepositions(), 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                wxNumberFormatter::Style::Style_WithThousandsSep) +
                        L"</span>" :
                    wxNumberFormatter::ToString(
                        MAX_DOLCH_PREPOSITION_WORDS - project->GetUnusedDolchPrepositions(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr =
                (dolchPrepositionsPercentage >= 75) ?
                    wxString::Format(L"<span style=\"color:%s\">",
                                     attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                        wxString::Format(_(L"(%s%% of all Dolch prepositions)"),
                                         wxNumberFormatter::ToString(
                                             dolchPrepositionsPercentage, 1,
                                             wxNumberFormatter::Style::Style_NoTrailingZeroes)) +
                        L"</span>" :
                    wxString::Format(_(L"(%s%% of all Dolch prepositions)"),
                                     wxNumberFormatter::ToString(
                                         dolchPrepositionsPercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes));
            htmlText += FormatDolchRow(labelColumnWidth, numberColumnWidth,
                                       _(L"Prepositions used:"), valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of prepositions used"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount, 1,
                    wxNumberFormatter::ToString(
                        MAX_DOLCH_PREPOSITION_WORDS - project->GetUnusedDolchPrepositions(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 2,
                    wxString::Format(_(L"%s%% of all Dolch prepositions"),
                                     wxNumberFormatter::ToString(
                                         dolchPrepositionsPercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes)),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }
            // Pronouns
            {
            const wxString valueStr =
                (dolchPronounsPercentage >= 75) ?
                    wxString::Format(L"<span style=\"color:%s\">",
                                     attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                        wxNumberFormatter::ToString(
                            MAX_DOLCH_PRONOUN_WORDS - project->GetUnusedDolchPronouns(), 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                wxNumberFormatter::Style::Style_WithThousandsSep) +
                        L"</span>" :
                    wxNumberFormatter::ToString(
                        MAX_DOLCH_PRONOUN_WORDS - project->GetUnusedDolchPronouns(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr =
                (dolchPronounsPercentage >= 75) ?
                    wxString::Format(L"<span style=\"color:%s\">",
                                     attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                        wxString::Format(_(L"(%s%% of all Dolch pronouns)"),
                                         wxNumberFormatter::ToString(
                                             dolchPronounsPercentage, 1,
                                             wxNumberFormatter::Style::Style_NoTrailingZeroes)) +
                        L"</span>" :
                    wxString::Format(_(L"(%s%% of all Dolch pronouns)"),
                                     wxNumberFormatter::ToString(
                                         dolchPronounsPercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes));
            htmlText += FormatDolchRow(labelColumnWidth, numberColumnWidth, _(L"Pronouns used:"),
                                       valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of pronouns used"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount, 1,
                    wxNumberFormatter::ToString(
                        MAX_DOLCH_PRONOUN_WORDS - project->GetUnusedDolchPronouns(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 2,
                    wxString::Format(_(L"%s%% of all Dolch pronouns"),
                                     wxNumberFormatter::ToString(
                                         dolchPronounsPercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes)),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }
            // Adverbs
            {
            const wxString valueStr =
                (dolchAdverbsPercentage >= 75) ?
                    wxString::Format(L"<span style=\"color:%s\">",
                                     attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                        wxNumberFormatter::ToString(
                            MAX_DOLCH_ADVERB_WORDS - project->GetUnusedDolchAdverbs(), 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                wxNumberFormatter::Style::Style_WithThousandsSep) +
                        L"</span>" :
                    wxNumberFormatter::ToString(
                        MAX_DOLCH_ADVERB_WORDS - project->GetUnusedDolchAdverbs(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr =
                (dolchAdverbsPercentage >= 75) ?
                    wxString::Format(L"<span style=\"color:%s\">",
                                     attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                        wxString::Format(_(L"(%s%% of all Dolch adverbs)"),
                                         wxNumberFormatter::ToString(
                                             dolchAdverbsPercentage, 1,
                                             wxNumberFormatter::Style::Style_NoTrailingZeroes)) +
                        L"</span>" :
                    wxString::Format(_(L"(%s%% of all Dolch adverbs)"),
                                     wxNumberFormatter::ToString(
                                         dolchAdverbsPercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes));
            htmlText += FormatDolchRow(labelColumnWidth, numberColumnWidth, _(L"Adverbs used:"),
                                       valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of adverbs used"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount, 1,
                    wxNumberFormatter::ToString(
                        MAX_DOLCH_ADVERB_WORDS - project->GetUnusedDolchAdverbs(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 2,
                    wxString::Format(_(L"%s%% of all Dolch adverbs"),
                                     wxNumberFormatter::ToString(
                                         dolchAdverbsPercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes)),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }
            // Adjectives
            {
            const wxString valueStr =
                (dolchAdjectivesPercentage >= 75) ?
                    wxString::Format(L"<span style=\"color:%s\">",
                                     attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                        wxNumberFormatter::ToString(
                            MAX_DOLCH_ADJECTIVE_WORDS - project->GetUnusedDolchAdjectives(), 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                wxNumberFormatter::Style::Style_WithThousandsSep) +
                        L"</span>" :
                    wxNumberFormatter::ToString(
                        MAX_DOLCH_ADJECTIVE_WORDS - project->GetUnusedDolchAdjectives(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr =
                (dolchAdjectivesPercentage >= 75) ?
                    wxString::Format(L"<span style=\"color:%s\">",
                                     attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                        wxString::Format(_(L"(%s%% of all Dolch adjectives)"),
                                         wxNumberFormatter::ToString(
                                             dolchAdjectivesPercentage, 1,
                                             wxNumberFormatter::Style::Style_NoTrailingZeroes)) +
                        L"</span>" :
                    wxString::Format(_(L"(%s%% of all Dolch adjectives)"),
                                     wxNumberFormatter::ToString(
                                         dolchAdjectivesPercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes));
            htmlText += FormatDolchRow(labelColumnWidth, numberColumnWidth, _(L"Adjectives used:"),
                                       valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of adjectives used"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount, 1,
                    wxNumberFormatter::ToString(
                        MAX_DOLCH_ADJECTIVE_WORDS - project->GetUnusedDolchAdjectives(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 2,
                    wxString::Format(_(L"%s%% of all Dolch adjectives"),
                                     wxNumberFormatter::ToString(
                                         dolchAdjectivesPercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes)),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }
            // Verbs
            {
            const wxString valueStr =
                (dolchVerbsPercentage >= 75) ?
                    wxString::Format(L"<span style=\"color:%s\">",
                                     attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                        wxNumberFormatter::ToString(
                            MAX_DOLCH_VERBS - project->GetUnusedDolchVerbs(), 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                wxNumberFormatter::Style::Style_WithThousandsSep) +
                        L"</span>" :
                    wxNumberFormatter::ToString(
                        MAX_DOLCH_VERBS - project->GetUnusedDolchVerbs(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr =
                (dolchVerbsPercentage >= 75) ?
                    wxString::Format(L"<span style=\"color:%s\">",
                                     attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                        wxString::Format(_(L"(%s%% of all Dolch verbs)"),
                                         wxNumberFormatter::ToString(
                                             dolchVerbsPercentage, 1,
                                             wxNumberFormatter::Style::Style_NoTrailingZeroes)) +
                        L"</span>" :
                    wxString::Format(_(L"(%s%% of all Dolch verbs)"),
                                     wxNumberFormatter::ToString(
                                         dolchVerbsPercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes));
            htmlText += FormatDolchRow(labelColumnWidth, numberColumnWidth, _(L"Verbs used:"),
                                       valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of verbs used"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount, 1,
                    wxNumberFormatter::ToString(
                        MAX_DOLCH_VERBS - project->GetUnusedDolchVerbs(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 2,
                    wxString::Format(_(L"%s%% of all Dolch verbs"),
                                     wxNumberFormatter::ToString(
                                         dolchVerbsPercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes)),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }
            // Nouns
            {
            const wxString valueStr =
                (dolchNounPercentage >= 75) ?
                    wxString::Format(L"<span style=\"color:%s\">",
                                     attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                        wxNumberFormatter::ToString(
                            MAX_DOLCH_NOUNS - project->GetUnusedDolchNouns(), 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                wxNumberFormatter::Style::Style_WithThousandsSep) +
                        L"</span>" :
                    wxNumberFormatter::ToString(
                        MAX_DOLCH_NOUNS - project->GetUnusedDolchNouns(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr =
                (dolchNounPercentage >= 75) ?
                    wxString::Format(L"<span style=\"color:%s\">",
                                     attentionColor.GetAsString(wxC2S_HTML_SYNTAX)) +
                        wxString::Format(_(L"(%s%% of all Dolch nouns)"),
                                         wxNumberFormatter::ToString(
                                             dolchNounPercentage, 1,
                                             wxNumberFormatter::Style::Style_NoTrailingZeroes)) +
                        L"</span>" :
                    wxString::Format(_(L"(%s%% of all Dolch nouns)"),
                                     wxNumberFormatter::ToString(
                                         dolchNounPercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes));
            htmlText += FormatDolchRow(labelColumnWidth, numberColumnWidth, _(L"Nouns used:"),
                                       valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of nouns used"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount, 1,
                    wxNumberFormatter::ToString(
                        MAX_DOLCH_NOUNS - project->GetUnusedDolchNouns(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 2,
                    wxString::Format(_(L"%s%% of all Dolch nouns"),
                                     wxNumberFormatter::ToString(
                                         dolchNounPercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes)),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }
        htmlText += L"\n</table>";

        wxString useDescription;
        if (dolchConjunctionPercentage >= 75 || dolchPrepositionsPercentage >= 75 ||
            dolchPronounsPercentage >= 75 || dolchAdverbsPercentage >= 75 ||
            dolchAdjectivesPercentage >= 75 || dolchVerbsPercentage >= 75 ||
            dolchNounPercentage >= 75)
            {
            useDescription = _(L"This document makes excellent use of:");
            if (dolchConjunctionPercentage >= 75)
                {
                useDescription += wxString::Format(L" %s, ", _(L"conjunctions"));
                }
            if (dolchPrepositionsPercentage >= 75)
                {
                useDescription += wxString::Format(L" %s, ", _(L"prepositions"));
                }
            if (dolchPronounsPercentage >= 75)
                {
                useDescription += wxString::Format(L" %s, ", _(L"pronouns"));
                }
            if (dolchAdverbsPercentage >= 75)
                {
                useDescription += wxString::Format(L" %s, ", _(L"adverbs"));
                }
            if (dolchAdjectivesPercentage >= 75)
                {
                useDescription += wxString::Format(L" %s, ", _(L"adjectives"));
                }
            if (dolchVerbsPercentage >= 75)
                {
                useDescription += wxString::Format(L" %s, ", _(L"verbs"));
                }
            if (dolchNounPercentage >= 75)
                {
                useDescription += wxString::Format(L" %s, ", _(L"nouns"));
                }
            useDescription.RemoveLast(2);
            useDescription += L".";
            if (containsHighPercentageOfNonDolchWords)
                {
                useDescription.append(L" ").append(
                    _(L"However, this document contains a high percentage of "
                      "non-Dolch words and may not be appropriate for using as a "
                      "Dolch test aid."));
                }
            useDescription += L"<br /><br />";
            }
        htmlText += FormatHtmlNoteSection(
            useDescription +
            _(L"Dolch words that are not being used in this document may be viewed in the "
              "<a href=\"#UnusedDolchWords\">Unused Dolch Words</a> output."));
        }

    // total words
    if (statsInfo.IsDolchWordsEnabled())
        {
        htmlText += tableStart + FormatDolchHeader(_(L"Dolch Words"));

            // number of Dolch words
            {
            const wxString valueStr =
                (totalDolchPercentage < 70) ?
                    GetIssueSpanStart() +
                        wxNumberFormatter::ToString(
                            totalDolchWords, 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                wxNumberFormatter::Style::Style_WithThousandsSep) +
                        L"</span>" :
                    wxNumberFormatter::ToString(
                        totalDolchWords, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr =
                (totalDolchPercentage < 70) ?
                    GetIssueSpanStart() +
                        wxString::Format(
                            _(L"(%s%% of all words)"),
                            wxNumberFormatter::ToString(
                                totalDolchPercentage, 1,
                                wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                    wxNumberFormatter::Style::Style_WithThousandsSep)) +
                        L"</span>" :
                    wxString::Format(_(L"(%s%% of all words)"),
                                     wxNumberFormatter::ToString(
                                         totalDolchPercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep));
            htmlText += FormatDolchRow(labelColumnWidth, numberColumnWidth,
                                       _(L"Number of Dolch words:"), valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of Dolch words"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount, 1,
                    wxNumberFormatter::ToString(
                        totalDolchWords, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 2,
                    wxString::Format(_(L"%s%% of all words"),
                                     wxNumberFormatter::ToString(
                                         totalDolchPercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep)),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }

            // number of Dolch words (excluding nouns)
            {
            const wxString valueStr =
                (totalDolchExcludingNounsPercentage < 60) ?
                    GetIssueSpanStart() +
                        wxNumberFormatter::ToString(
                            totalDolchWordsExcludingNouns, 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                wxNumberFormatter::Style::Style_WithThousandsSep) +
                        L"</span>" :
                    wxNumberFormatter::ToString(
                        totalDolchWordsExcludingNouns, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr =
                (totalDolchExcludingNounsPercentage < 60) ?
                    GetIssueSpanStart() +
                        wxString::Format(
                            _(L"(%s%% of all words)"),
                            wxNumberFormatter::ToString(
                                totalDolchExcludingNounsPercentage, 1,
                                wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                    wxNumberFormatter::Style::Style_WithThousandsSep)) +
                        L"</span>" :
                    wxString::Format(_(L"(%s%% of all words)"),
                                     wxNumberFormatter::ToString(
                                         totalDolchExcludingNounsPercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep));
            htmlText += FormatDolchRow(labelColumnWidth, numberColumnWidth,
                                       _(L"Number of Dolch words (excluding nouns):"), valueStr,
                                       percentStr);

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of Dolch words (excluding nouns)"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount, 1,
                    wxNumberFormatter::ToString(
                        totalDolchWordsExcludingNouns, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 2,
                    // TRANSLATORS: "%s%%" is a formatted number and % symbol.
                    // They should stay together.
                    wxString::Format(_(L"%s%% of all words"),
                                     wxNumberFormatter::ToString(
                                         totalDolchExcludingNounsPercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep)),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }

            // non-Dolch words
            {
            const wxString valueStr =
                (totalDolchPercentage < 70) ?
                    GetIssueSpanStart() +
                        wxNumberFormatter::ToString(
                            project->GetTotalWords() - totalDolchWords, 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                wxNumberFormatter::Style::Style_WithThousandsSep) +
                        L"</span>" :
                    wxNumberFormatter::ToString(
                        project->GetTotalWords() - totalDolchWords, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr =
                (totalDolchPercentage < 70) ?
                    GetIssueSpanStart() +
                        wxString::Format(
                            _(L"(%s%% of all words)"),
                            wxNumberFormatter::ToString(
                                100 - totalDolchPercentage, 1,
                                wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                    wxNumberFormatter::Style::Style_WithThousandsSep)) +
                        L"</span>" :
                    wxString::Format(_(L"(%s%% of all words)"),
                                     wxNumberFormatter::ToString(
                                         100 - totalDolchPercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep));
            htmlText += FormatDolchRow(labelColumnWidth, numberColumnWidth,
                                       _(L"Number of non-Dolch words:"), valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of non-Dolch words"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount, 1,
                    wxNumberFormatter::ToString(
                        project->GetTotalWords() - totalDolchWords, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 2,
                    wxString::Format(_(L"%s%% of all words"),
                                     wxNumberFormatter::ToString(
                                         100 - totalDolchPercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep)),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }

            // Conjunctions words
            {
            const wxString valueStr =
                wxNumberFormatter::ToString(project->GetDolchConjunctionCounts().second, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr = wxString::Format(
                _(L"(%s%% of all words)"),
                wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetDolchConjunctionCounts().second,
                                        project->GetTotalWords()) *
                        100,
                    1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes |
                        wxNumberFormatter::Style::Style_WithThousandsSep));
            htmlText += FormatDolchRow(labelColumnWidth, numberColumnWidth,
                                       _(L"Number of Dolch conjunctions:"), valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of Dolch conjunctions"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount, 1,
                    wxNumberFormatter::ToString(
                        project->GetDolchConjunctionCounts().second, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 2,
                    wxString::Format(
                        _(L"%s%% of all words"),
                        wxNumberFormatter::ToString(
                            safe_divide<double>(project->GetDolchConjunctionCounts().second,
                                                project->GetTotalWords()) *
                                100,
                            1,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                wxNumberFormatter::Style::Style_WithThousandsSep)),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }

            htmlText += FormatDolchRow(
                labelColumnWidth, numberColumnWidth, _(L"Number of unique Dolch conjunctions:"),
                wxNumberFormatter::ToString(project->GetDolchConjunctionCounts().first, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                L"");
            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of unique Dolch conjunctions"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 1,
                    wxNumberFormatter::ToString(
                        project->GetDolchConjunctionCounts().first, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }
            // Prepositions words
            {
            const wxString valueStr =
                wxNumberFormatter::ToString(project->GetDolchPrepositionWordCounts().second, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr = wxString::Format(
                _(L"(%s%% of all words)"),
                wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetDolchPrepositionWordCounts().second,
                                        project->GetTotalWords()) *
                        100,
                    1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes |
                        wxNumberFormatter::Style::Style_WithThousandsSep));
            htmlText += FormatDolchRow(labelColumnWidth, numberColumnWidth,
                                       _(L"Number of Dolch prepositions:"), valueStr, percentStr);
            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of Dolch prepositions"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount, 1,
                    wxNumberFormatter::ToString(
                        project->GetDolchPrepositionWordCounts().second, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 2,
                    wxString::Format(
                        _(L"%s%% of all words"),
                        wxNumberFormatter::ToString(
                            safe_divide<double>(project->GetDolchPrepositionWordCounts().second,
                                                project->GetTotalWords()) *
                                100,
                            1,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                wxNumberFormatter::Style::Style_WithThousandsSep)),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }

            htmlText += FormatDolchRow(
                labelColumnWidth, numberColumnWidth, _(L"Number of unique Dolch prepositions:"),
                wxNumberFormatter::ToString(project->GetDolchPrepositionWordCounts().first, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                L"");
            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of unique Dolch prepositions"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 1,
                    wxNumberFormatter::ToString(
                        project->GetDolchPrepositionWordCounts().first, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }
            // Pronouns
            {
            const wxString valueStr =
                wxNumberFormatter::ToString(project->GetDolchPronounCounts().second, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr =
                wxString::Format(_(L"(%s%% of all words)"),
                                 wxNumberFormatter::ToString(
                                     safe_divide<double>(project->GetDolchPronounCounts().second,
                                                         project->GetTotalWords()) *
                                         100,
                                     1,
                                     wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                         wxNumberFormatter::Style::Style_WithThousandsSep));
            htmlText += FormatDolchRow(labelColumnWidth, numberColumnWidth,
                                       _(L"Number of Dolch pronouns:"), valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of Dolch pronouns"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount, 1,
                    wxNumberFormatter::ToString(
                        project->GetDolchPronounCounts().second, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 2,
                    wxString::Format(
                        _(L"%s%% of all words"),
                        wxNumberFormatter::ToString(
                            safe_divide<double>(project->GetDolchPronounCounts().second,
                                                project->GetTotalWords()) *
                                100,
                            1,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                wxNumberFormatter::Style::Style_WithThousandsSep)),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }

            htmlText += FormatDolchRow(
                labelColumnWidth, numberColumnWidth, _(L"Number of unique Dolch pronouns:"),
                wxNumberFormatter::ToString(project->GetDolchPronounCounts().first, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                L"");
            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of unique Dolch pronouns"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 1,
                    wxNumberFormatter::ToString(
                        project->GetDolchPronounCounts().first, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }
            // Adverbs
            {
            const wxString valueStr =
                wxNumberFormatter::ToString(project->GetDolchAdverbCounts().second, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr =
                wxString::Format(_(L"(%s%% of all words)"),
                                 wxNumberFormatter::ToString(
                                     safe_divide<double>(project->GetDolchAdverbCounts().second,
                                                         project->GetTotalWords()) *
                                         100,
                                     1,
                                     wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                         wxNumberFormatter::Style::Style_WithThousandsSep));
            htmlText += FormatDolchRow(labelColumnWidth, numberColumnWidth,
                                       _(L"Number of Dolch adverbs:"), valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of Dolch adverbs"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount, 1,
                    wxNumberFormatter::ToString(
                        project->GetDolchAdverbCounts().second, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 2,
                    wxString::Format(_(L"%s%% of all words"),
                                     wxNumberFormatter::ToString(
                                         safe_divide<double>(project->GetDolchAdverbCounts().second,
                                                             project->GetTotalWords()) *
                                             100,
                                         1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep)),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }

            htmlText += FormatDolchRow(
                labelColumnWidth, numberColumnWidth, _(L"Number of unique Dolch adverbs:"),
                wxNumberFormatter::ToString(project->GetDolchAdverbCounts().first, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                L"");
            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of unique Dolch adverbs"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 1,
                    wxNumberFormatter::ToString(
                        project->GetDolchAdverbCounts().first, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }
            // adjectives
            {
            const wxString valueStr =
                wxNumberFormatter::ToString(project->GetDolchAdjectiveCounts().second, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr =
                wxString::Format(_(L"(%s%% of all words)"),
                                 wxNumberFormatter::ToString(
                                     safe_divide<double>(project->GetDolchAdjectiveCounts().second,
                                                         project->GetTotalWords()) *
                                         100,
                                     1,
                                     wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                         wxNumberFormatter::Style::Style_WithThousandsSep));
            htmlText += FormatDolchRow(labelColumnWidth, numberColumnWidth,
                                       _(L"Number of Dolch adjectives:"), valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of Dolch adjectives"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount, 1,
                    wxNumberFormatter::ToString(
                        project->GetDolchAdjectiveCounts().second, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 2,
                    wxString::Format(
                        _(L"%s%% of all words"),
                        wxNumberFormatter::ToString(
                            safe_divide<double>(project->GetDolchAdjectiveCounts().second,
                                                project->GetTotalWords()) *
                                100,
                            1,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                wxNumberFormatter::Style::Style_WithThousandsSep)),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }

            htmlText += FormatDolchRow(
                labelColumnWidth, numberColumnWidth, _(L"Number of unique Dolch adjectives:"),
                wxNumberFormatter::ToString(project->GetDolchAdjectiveCounts().first, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                L"");
            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of unique Dolch adjectives"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 1,
                    wxNumberFormatter::ToString(
                        project->GetDolchAdjectiveCounts().first, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }
            // verbs
            {
            const wxString valueStr =
                wxNumberFormatter::ToString(project->GetDolchVerbsCounts().second, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr =
                wxString::Format(_(L"(%s%% of all words)"),
                                 wxNumberFormatter::ToString(
                                     safe_divide<double>(project->GetDolchVerbsCounts().second,
                                                         project->GetTotalWords()) *
                                         100,
                                     1,
                                     wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                         wxNumberFormatter::Style::Style_WithThousandsSep));
            htmlText += FormatDolchRow(labelColumnWidth, numberColumnWidth,
                                       _(L"Number of Dolch verbs:"), valueStr, percentStr);

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of Dolch verbs"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount, 1,
                    wxNumberFormatter::ToString(
                        project->GetDolchVerbsCounts().second, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 2,
                    wxString::Format(_(L"%s%% of all words"),
                                     wxNumberFormatter::ToString(
                                         safe_divide<double>(project->GetDolchVerbsCounts().second,
                                                             project->GetTotalWords()) *
                                             100,
                                         1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep)),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }

            htmlText += FormatDolchRow(
                labelColumnWidth, numberColumnWidth, _(L"Number of unique Dolch verbs:"),
                wxNumberFormatter::ToString(project->GetDolchVerbsCounts().first, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                L"");
            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of unique Dolch verbs"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 1,
                    wxNumberFormatter::ToString(
                        project->GetDolchVerbsCounts().first, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }
            // nouns
            {
            const wxString valueStr =
                wxNumberFormatter::ToString(project->GetDolchNounCounts().second, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep);
            const wxString percentStr =
                wxString::Format(_(L"(%s%% of all words)"),
                                 wxNumberFormatter::ToString(
                                     safe_divide<double>(project->GetDolchNounCounts().second,
                                                         project->GetTotalWords()) *
                                         100,
                                     1,
                                     wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                         wxNumberFormatter::Style::Style_WithThousandsSep));
            htmlText += FormatDolchRow(labelColumnWidth, numberColumnWidth,
                                       _(L"Number of Dolch nouns:"), valueStr, percentStr);
            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of Dolch nouns"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount, 1,
                    wxNumberFormatter::ToString(
                        project->GetDolchNounCounts().second, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 2,
                    wxString::Format(_(L"%s%% of all words"),
                                     wxNumberFormatter::ToString(
                                         safe_divide<double>(project->GetDolchNounCounts().second,
                                                             project->GetTotalWords()) *
                                             100,
                                         1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep)),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }

            htmlText += FormatDolchRow(
                labelColumnWidth, numberColumnWidth, _(L"Number of unique Dolch nouns:"),
                wxNumberFormatter::ToString(project->GetDolchNounCounts().first, 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                L"");
            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of unique Dolch nouns"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 1,
                    wxNumberFormatter::ToString(
                        project->GetDolchNounCounts().first, 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }
        htmlText += L"\n</table>";

        if (containsHighPercentageOfNonDolchWords)
            {
            htmlText += FormatHtmlWarningSection(_(L"This document contains a high percentage of "
                                                   "non-Dolch words and may not be appropriate "
                                                   "for using as a Dolch test aid."));
            }
        }

    if (includeExplanation && statsInfo.IsDolchExplanationEnabled())
        {
        htmlText += tableStart + FormatDolchHeader(_(L"Explanation"));
        htmlText += L"\n<tr><td style='width:100%'><p>";
        htmlText +=
            _(L"The Dolch Sight Words represent the most frequently occurring service "
              "words<sup>1</sup> "
              "in most text, especially children's literature. Early readers need to learn and "
              "recognize "
              "these words to attain reading fluency. Many of these words cannot be sounded out or "
              "represented by pictures; therefore, they must be learned by sight<sup>2</sup>.");
        htmlText += L"</p>\n<p>";
        htmlText +=
            _(L"If you are writing educational materials for early readers, then it is recommended "
              "to use as many of these words as possible to help readers practice them. "
              "It is also recommended to keep the percentage of non-Dolch words low so that the "
              "reader can better focus on the sight words.");
        htmlText += L"</p>\n<p><sup>1</sup> ";
        htmlText += _(L"Pronouns, adjectives, adverbs, prepositions, conjunctions, and verbs.");
        htmlText += L"<br /><sup>2</sup> ";
        htmlText += _(L"A separate list of nouns commonly found in children's literature is also "
                      "included with the "
                      "Dolch collection. However, the sight words are generally the focus of most "
                      "Dolch activities.");
        htmlText += L"</p></td></tr>\n</table>";
        }

    if (listData)
        {
        listData->SetSize(listDataItemCount, 3);
        }

    return htmlText;
    }

//------------------------------------------------
wxString ProjectReportFormat::FormatHtmlReportStart(const wxString& title /*= wxString{}*/)
    {
    wxString cssTemplatePath = wxGetApp().FindResourceDirectory(_DT(L"report-themes")) +
                               wxFileName::GetPathSeparator() + L"default.css";
    wxString styleInfo;
    if (wxFile::Exists(cssTemplatePath))
        {
        Wisteria::TextStream::ReadFile(cssTemplatePath, styleInfo);
        }

    return wxString::Format(
        L"<!DOCTYPE html>"
        "\n<html>"
        "\n<head>"
        "\n    <meta http-equiv='content-type' content='text/html; charset=UTF-8' />"
        "\n    <title>%s</title>"
        "\n    <style>"
        "\n    %s"
        "\n    </style>"
        "\n</head>"
        "\n<body>",
        title, styleInfo);
    }

//------------------------------------------------
wxString ProjectReportFormat::FormatHtmlReportEnd() { return { L"\n</body>\n</html>" }; }

//------------------------------------------------
wxString ProjectReportFormat::FormatStatisticsInfo(
    const BaseProject* project, const StatisticsReportInfo& statsInfo,
    const wxColour& attentionColor,
    const std::shared_ptr<Wisteria::UI::ListCtrlExDataProviderBase>& listData)
    {
    PROFILE();
    if (project == nullptr)
        {
        return {};
        }
    wxString currentLabel;
    const lily_of_the_valley::html_encode_text htmlEncode;
    lily_of_the_valley::html_extract_text htmlStrip;
    size_t listDataItemCount{ 0 };
    if (listData != nullptr)
        {
        listData->DeleteAllItems();
        listData->SetSize(MAX_SUMMARY_STAT_ROWS, 3);
        }

    wxString htmlText;

    // Number of difficult sentences
    const double overlyLongSentencePercentage =
        (project->GetTotalOverlyLongSentences() == 0) ?
            0 :
            safe_divide<double>(project->GetTotalOverlyLongSentences(),
                                project->GetTotalSentences()) *
                100;
    // Number of exclamatory sentences
    const auto exclamatorySentencePercentage =
        (project->GetTotalExclamatorySentences() == 0) ?
            0 :
            safe_divide<double>(project->GetTotalExclamatorySentences(),
                                project->GetTotalSentences()) *
                100;
    const auto averageCharacterCount =
        safe_divide<double>(project->GetTotalCharacters(), project->GetTotalWords());
    const auto averageSyllableCount =
        safe_divide<double>(project->GetTotalSyllables(), project->GetTotalWords());

    if (!statsInfo.HasStatisticsEnabled())
        {
        htmlText += _(L"No statistics <a href=\"#SelectStatistics\">currently selected</a>.");
        }

    wxBitmap bmp(100, 100);
    wxMemoryDC measuringDc(bmp);
    measuringDc.SetFont(measuringDc.GetFont().Larger().Larger());
    // one of the longer labels
    const auto labelColumnWidth = safe_divide<long>(
        // TRANSLATORS: This is really a placeholder string used for measuring, but
        // should be translated to measure properly. The "20" here doesn't mean anything.
        measuringDc.GetTextExtent(_(L"Number of difficult sentences (more than 20 words):"))
            .GetWidth(),
        wxGetApp().GetDPIScaleFactor());
    // the total number of words would be the largest (i.e., widest) number possible, so measure
    // using that
    const auto numberColumnWidth = safe_divide<long>(
        measuringDc.GetTextExtent(std::to_wstring(project->GetTotalWords())).GetWidth(),
        wxGetApp().GetDPIScaleFactor());

    const wxString tableStart = L"<table class='minipage' border='0' style='width:100%'>";

    const auto formatHeader = [](const wxString& label)
    {
        return wxString::Format(L"\n<tr class='report-column-header' style='background:%s;'>"
                                "<td colspan='3'><span style='color:%s;'>%s</span></td></tr>",
                                GetReportHeaderColor().GetAsString(wxC2S_HTML_SYNTAX),
                                GetReportHeaderFontColor().GetAsString(wxC2S_HTML_SYNTAX), label);
    };

    const auto formatNoteHeader = [](const wxString& label)
    {
        return wxString::Format(L"\n<tr style='background:%s;'><td colspan='3'><span "
                                "style='color:white;'>%s</span></td></tr>",
                                GetReportNoteHeaderColor().GetAsString(wxC2S_HTML_SYNTAX), label);
    };

    const auto formatRow =
        [labelColumnWidth, numberColumnWidth](const wxString& label, const wxString& value,
                                              const wxString& percent = wxString{})
    {
        return wxString::Format(L"\n<tr><td style='min-width:%ldpx; width:40%%;'>%s</td>"
                                "<td style='text-align:right; width:%ldpx;'>%s</td>"
                                "<td style='text-align:left;'>%s</td></tr>",
                                labelColumnWidth, label, numberColumnWidth, value, percent);
    };

    if (statsInfo.IsParagraphEnabled())
        {
        // number of paragraphs
        htmlText += tableStart + formatHeader(_(L"Paragraphs")) +
                    formatRow(_(L"Number of paragraphs:"),
                              wxNumberFormatter::ToString(
                                  project->GetTotalParagraphs(), 0,
                                  wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                      wxNumberFormatter::Style::Style_WithThousandsSep));

        if (listData)
            {
            listData->SetItemText(
                listDataItemCount, 0, _(L"Number of paragraphs"),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount++, 1,
                wxNumberFormatter::ToString(project->GetTotalParagraphs(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }

        // average paragraph length
        htmlText += formatRow(
            _(L"Average number of sentences per paragraph:"),
            wxNumberFormatter::ToString(
                safe_divide<double>(project->GetTotalSentences(), project->GetTotalParagraphs()), 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes |
                    wxNumberFormatter::Style::Style_WithThousandsSep));
        htmlText += L"\n</table>";

        if (listData)
            {
            listData->SetItemText(
                listDataItemCount, 0, _(L"Average number of sentences per paragraph"),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount++, 1,
                wxNumberFormatter::ToString(safe_divide<double>(project->GetTotalSentences(),
                                                                project->GetTotalParagraphs()),
                                            1,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        }

    if (statsInfo.IsSentencesEnabled())
        {
        // Number of sentences
        htmlText += tableStart + formatHeader(_(L"Sentences"));

        if (project->GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis)
            {
            currentLabel = _(L"Number of sentences (excluding incomplete sentences, "
                             "see notes <a href=\"#incompsent\">below</a>):");
            }
        else if (project->GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings)
            {
            currentLabel = _(L"Number of sentences (excluding lists and tables, see notes <a "
                             "href=\"#incompsent\">below</a>):");
            }
        else
            {
            currentLabel = _(L"Number of sentences:");
            }
        htmlText += formatRow(
            currentLabel,
            wxNumberFormatter::ToString(project->GetTotalSentences(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));

        if (listData)
            {
            currentLabel = _(L"Number of sentences");
            listData->SetItemText(
                listDataItemCount, 0, htmlStrip(currentLabel, currentLabel.length(), true, false),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount++, 1,
                wxNumberFormatter::ToString(project->GetTotalSentences(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }

        // Number of sentence units
        if (statsInfo.IsExtendedInformationEnabled())
            {
            if (project->GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis)
                {
                currentLabel = _(L"Number of units/independent clauses (excluding incomplete "
                                 "sentences, see notes "
                                 "<a href=\"#incompsent\">below</a>):");
                }
            else if (project->GetInvalidSentenceMethod() ==
                     InvalidSentence::ExcludeExceptForHeadings)
                {
                currentLabel =
                    _(L"Number of units/independent clauses (excluding lists and tables, see notes "
                      "<a href=\"#incompsent\">below</a>):");
                }
            else
                {
                currentLabel = _(L"Number of units/independent clauses:");
                }
            htmlText += formatRow(
                currentLabel,
                wxNumberFormatter::ToString(project->GetTotalSentenceUnits(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep));

            if (listData)
                {
                currentLabel = _(L"Number of units/independent clauses");
                listData->SetItemText(
                    listDataItemCount, 0,
                    htmlStrip(currentLabel, currentLabel.length(), true, false),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 1,
                    wxNumberFormatter::ToString(
                        project->GetTotalSentenceUnits(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }

        if (project->GetTotalOverlyLongSentences() > 0)
            {
            currentLabel =
                wxString::Format(_(L"Number of <a href=\"#DifficultSentences\">difficult "
                                   "sentences</a> (more than %d words):"),
                                 project->GetDifficultSentenceLength());
            }
        else
            {
            currentLabel =
                wxString::Format(_(L"Number of difficult sentences (more than %d words):"),
                                 project->GetDifficultSentenceLength());
            }
        wxString currentValue =
            (overlyLongSentencePercentage >= 40) ?
                GetIssueSpanStart() +
                    wxNumberFormatter::ToString(
                        project->GetTotalOverlyLongSentences(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep) +
                    L"</span>" :
                wxNumberFormatter::ToString(project->GetTotalOverlyLongSentences(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep);
        wxString currentPercent =
            (overlyLongSentencePercentage >= 40) ?
                L"<span style=\"color:#FF0000\">" +
                    wxString::Format(_(L"(%s%% of all sentences)"),
                                     wxNumberFormatter::ToString(
                                         overlyLongSentencePercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep)) +
                    L"</span>" :
                wxString::Format(_(L"(%s%% of all sentences)"),
                                 wxNumberFormatter::ToString(
                                     overlyLongSentencePercentage, 1,
                                     wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                         wxNumberFormatter::Style::Style_WithThousandsSep));
        htmlText += formatRow(currentLabel, currentValue, currentPercent);

        if (listData)
            {
            wxString sentLabelForTable =
                htmlStrip(currentLabel, currentLabel.length(), true, false);
            if (sentLabelForTable[sentLabelForTable.length() - 1] == L':')
                {
                sentLabelForTable.RemoveLast(1);
                }
            listData->SetItemText(
                listDataItemCount, 0, sentLabelForTable,
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount, 1,
                wxNumberFormatter::ToString(project->GetTotalOverlyLongSentences(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount++, 2,
                wxString::Format(_(L"%s%% of all sentences"),
                                 wxNumberFormatter::ToString(
                                     overlyLongSentencePercentage, 1,
                                     wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                         wxNumberFormatter::Style::Style_WithThousandsSep)),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }

        // Longest sentence
        if (project->GetTotalSentences() > 0) // this should usually be the case
            {
            currentValue = (project->GetLongestSentence() >= 30) ?
                               GetIssueSpanStart() +
                                   wxNumberFormatter::ToString(
                                       project->GetLongestSentence(), 0,
                                       wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                           wxNumberFormatter::Style::Style_WithThousandsSep) +
                                   L"</span>" :
                               wxNumberFormatter::ToString(
                                   project->GetLongestSentence(), 0,
                                   wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                       wxNumberFormatter::Style::Style_WithThousandsSep);
            htmlText += formatRow(
                wxString::Format(_(L"Longest sentence (sentence #%s):"),
                                 // add 1 to display it as 1 based
                                 wxNumberFormatter::ToString(
                                     project->GetLongestSentenceIndex() + 1, 0,
                                     wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                         wxNumberFormatter::Style::Style_WithThousandsSep)),
                currentValue);

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Longest sentence"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 1,
                    wxNumberFormatter::ToString(
                        project->GetLongestSentence(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }
        else
            {
            htmlText += L"<tr><td style=\"width:60%\">";
            htmlText += _(L"Longest sentence:");
            htmlText += L"</td><td style=\"text-align:right; width:10%\">";
            htmlText += _(L"N/A");
            htmlText += L"</td><td></td></tr>\n";
            }
        // average sentence length
        currentValue =
            (safe_divide<double>(project->GetTotalWords(), project->GetTotalSentences()) > 20) ?
                GetIssueSpanStart() +
                    wxNumberFormatter::ToString(
                        safe_divide<double>(project->GetTotalWords(), project->GetTotalSentences()),
                        1,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep) +
                    L"</span>" :
                wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetTotalWords(), project->GetTotalSentences()), 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes |
                        wxNumberFormatter::Style::Style_WithThousandsSep);
        htmlText += formatRow(_(L"Average sentence length:"), currentValue);

        if (listData)
            {
            listData->SetItemText(
                listDataItemCount, 0, _(L"Average sentence length"),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount++, 1,
                wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetTotalWords(), project->GetTotalSentences()), 1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes |
                        wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        // Number of interrogative sentences
        const double interrogativeSentencePercentage =
            (project->GetTotalInterrogativeSentences() == 0) ?
                0 :
                safe_divide<double>(project->GetTotalInterrogativeSentences(),
                                    project->GetTotalSentences()) *
                    100;
        htmlText += formatRow(
            _(L"Number of interrogative sentences (questions):"),
            wxNumberFormatter::ToString(project->GetTotalInterrogativeSentences(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep),
            wxString::Format(
                _(L"(%s%% of all sentences)"),
                wxNumberFormatter::ToString(interrogativeSentencePercentage, 1,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep)));

        if (listData)
            {
            listData->SetItemText(
                listDataItemCount, 0, _(L"Number of interrogative sentences (questions)"),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount, 1,
                wxNumberFormatter::ToString(project->GetTotalInterrogativeSentences(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount++, 2,
                wxString::Format(_(L"%s%% of all sentences"),
                                 wxNumberFormatter::ToString(
                                     interrogativeSentencePercentage, 1,
                                     wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                         wxNumberFormatter::Style::Style_WithThousandsSep)),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }

        // exclamatory sentences
        currentValue =
            (exclamatorySentencePercentage >= 25) ?
                GetIssueSpanStart() +
                    wxNumberFormatter::ToString(
                        project->GetTotalExclamatorySentences(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep) +
                    L"</span>" :
                wxNumberFormatter::ToString(project->GetTotalExclamatorySentences(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep);
        currentPercent =
            (exclamatorySentencePercentage >= 25) ?
                L"<span style=\"color:#FF0000\">" +
                    wxString::Format(_(L"(%s%% of all sentences)"),
                                     wxNumberFormatter::ToString(
                                         exclamatorySentencePercentage, 1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep)) +
                    L"</span>" :
                wxString::Format(_(L"(%s%% of all sentences)"),
                                 wxNumberFormatter::ToString(
                                     exclamatorySentencePercentage, 1,
                                     wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                         wxNumberFormatter::Style::Style_WithThousandsSep));
        htmlText += formatRow(_(L"Number of exclamatory sentences:"), currentValue, currentPercent);
        htmlText += L"\n</table>";

        if (listData)
            {
            listData->SetItemText(
                listDataItemCount, 0, _(L"Number of exclamatory sentences"),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount, 1,
                wxNumberFormatter::ToString(project->GetTotalExclamatorySentences(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount++, 2,
                wxString::Format(_(L"%s%% of all sentences"),
                                 wxNumberFormatter::ToString(
                                     exclamatorySentencePercentage, 1,
                                     wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                         wxNumberFormatter::Style::Style_WithThousandsSep)),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        // add any necessary notes
        if (overlyLongSentencePercentage >= 40)
            {
            htmlText +=
                FormatHtmlWarningSection(_(L"A large percentage of sentences are overly long."));
            }
        if (exclamatorySentencePercentage >= 25)
            {
            htmlText += FormatHtmlNoteSection(
                _(L"A large percentage of sentences are exclamatory, giving the document an "
                  "overall angry/excited tone. Is this intended?"));
            }
        }

    if (statsInfo.IsWordsEnabled())
        {
        // Number of words
        htmlText += tableStart + formatHeader(_(L"Words"));

        wxString currentValue =
            (project->GetTotalWords() < 300) ?
                GetIssueSpanStart() +
                    wxNumberFormatter::ToString(
                        project->GetTotalWords(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep) +
                    L"</span>" :
                wxNumberFormatter::ToString(project->GetTotalWords(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep);
        htmlText += formatRow(_(L"Number of words:"), currentValue);

        if (listData)
            {
            listData->SetItemText(
                listDataItemCount, 0, _(L"Number of words"),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount++, 1,
                wxNumberFormatter::ToString(project->GetTotalWords(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        // Number of unique words
        htmlText += formatRow(
            _(L"Number of unique words:"),
            wxNumberFormatter::ToString(project->GetTotalUniqueWords(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));

        if (listData)
            {
            listData->SetItemText(
                listDataItemCount, 0, _(L"Number of unique words"),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount++, 1,
                wxNumberFormatter::ToString(project->GetTotalUniqueWords(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        // Number of total syllables
        htmlText += formatRow(
            _(L"Number of syllables:"),
            wxNumberFormatter::ToString(project->GetTotalSyllables(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));

        if (listData)
            {
            listData->SetItemText(
                listDataItemCount, 0, _(L"Number of syllables"),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount++, 1,
                wxNumberFormatter::ToString(project->GetTotalSyllables(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        // Number of total characters
        htmlText += formatRow(
            _(L"Number of characters (punctuation excluded):"),
            wxNumberFormatter::ToString(project->GetTotalCharacters(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));

        if (listData)
            {
            listData->SetItemText(
                listDataItemCount, 0, _(L"Number of characters (punctuation excluded)"),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount++, 1,
                wxNumberFormatter::ToString(project->GetTotalCharacters(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        htmlText += formatRow(
            _(L"Number of characters + punctuation:"),
            wxNumberFormatter::ToString(project->GetTotalCharactersPlusPunctuation(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));

        htmlText += L"\n<tr><td colspan=\"3\">";
        htmlText +=
            FormatHtmlNoteSection(_(L"Sentence-ending punctuation is excluded from this statistic. "
                                    "Tests that include punctuation counts instruct to not include "
                                    "any punctuation that ends a sentence."));
        htmlText += L"</td></tr>";

        if (listData)
            {
            listData->SetItemText(
                listDataItemCount, 0, _(L"Number of characters + punctuation"),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount++, 1,
                wxNumberFormatter::ToString(project->GetTotalCharactersPlusPunctuation(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        // average number of characters
        currentValue =
            (averageCharacterCount >= 6) ?
                GetIssueSpanStart() +
                    wxNumberFormatter::ToString(
                        averageCharacterCount, 1,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep) +
                    L"</span>" :
                wxNumberFormatter::ToString(averageCharacterCount, 1,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep);
        htmlText += formatRow(_(L"Average number of characters:"), currentValue);

        if (listData)
            {
            listData->SetItemText(
                listDataItemCount, 0, _(L"Average number of characters"),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount++, 1,
                wxNumberFormatter::ToString(averageCharacterCount, 1,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        // average number of syllables
        currentValue =
            (averageSyllableCount >= 4) ?
                GetIssueSpanStart() +
                    wxNumberFormatter::ToString(
                        averageSyllableCount, 1,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep) +
                    L"</span>" :
                wxNumberFormatter::ToString(averageSyllableCount, 1,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep);
        htmlText += formatRow(_(L"Average number of syllables:"), currentValue);
        htmlText += L"\n</table>";

        if (listData)
            {
            listData->SetItemText(
                listDataItemCount, 0, _(L"Average number of syllables"),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount++, 1,
                wxNumberFormatter::ToString(averageSyllableCount, 1,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        // supplementary info about these stats
        if ((project->GetTotalWords() < 300) || (averageCharacterCount >= 6) ||
            (averageSyllableCount >= 4))
            {
            if (project->GetTotalWords() < 300)
                {
                htmlText += FormatHtmlWarningSection(
                    wxString::Format(_(L"This document only contains %s words. "
                                       "Most readability tests require a minimum of 300 words to "
                                       "generate meaningful results."),
                                     wxNumberFormatter::ToString(
                                         project->GetTotalWords(), 0,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep)));
                }
            if (averageCharacterCount >= 6)
                {
                htmlText += FormatHtmlWarningSection(_(L"The average word length is high."));
                }
            if (averageSyllableCount >= 4)
                {
                htmlText += FormatHtmlWarningSection(
                    _(L"The average word complexity (syllable count) is high."));
                }
            }
        }

    if (statsInfo.IsExtendedWordsEnabled())
        {
        // Numeric words
        htmlText +=
            tableStart + formatHeader(_(L"Numerals (Numbers, Dates, Currency, etc.)")) +
            formatRow(
                _(L"Number of numerals:"),
                wxNumberFormatter::ToString(project->GetTotalNumerals(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                wxString::Format(
                    _(L"(%s%% of all words)"),
                    wxNumberFormatter::ToString(
                        safe_divide<double>(project->GetTotalNumerals(), project->GetTotalWords()) *
                            100,
                        1,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep)));
        htmlText += L"\n</table>";

        if (listData)
            {
            listData->SetItemText(
                listDataItemCount, 0, _(L"Number of numerals"),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount, 1,
                wxNumberFormatter::ToString(project->GetTotalNumerals(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount++, 2,
                wxString::Format(
                    _(L"%s%% of all words"),
                    wxNumberFormatter::ToString(
                        safe_divide<double>(project->GetTotalNumerals(), project->GetTotalWords()) *
                            100,
                        1,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep)),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }

        // Proper nouns (not supported by German)
        if (project->GetProjectLanguage() != readability::test_language::german_test)
            {
            htmlText += tableStart + formatHeader(_(L"Proper Nouns"));
            htmlText += formatRow(
                _(L"Number of proper nouns:"),
                wxNumberFormatter::ToString(project->GetTotalProperNouns(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                wxString::Format(_(L"(%s%% of all words)"),
                                 wxNumberFormatter::ToString(
                                     safe_divide<double>(project->GetTotalProperNouns(),
                                                         project->GetTotalWords()) *
                                         100,
                                     1,
                                     wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                         wxNumberFormatter::Style::Style_WithThousandsSep)));
            htmlText += L"\n</table>";

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of proper nouns"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount, 1,
                    wxNumberFormatter::ToString(
                        project->GetTotalProperNouns(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 2,
                    wxString::Format(_(L"%s%% of all words"),
                                     wxNumberFormatter::ToString(
                                         safe_divide<double>(project->GetTotalProperNouns(),
                                                             project->GetTotalWords()) *
                                             100,
                                         1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep)),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }

        // Monosyllabic words
        htmlText +=
            tableStart + formatHeader(_(L"Monosyllabic Words")) +
            formatRow(
                _(L"Number of monosyllabic words:"),
                wxNumberFormatter::ToString(project->GetTotalMonoSyllabicWords(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                wxString::Format(_(L"(%s%% of all words)"),
                                 wxNumberFormatter::ToString(
                                     safe_divide<double>(project->GetTotalMonoSyllabicWords(),
                                                         project->GetTotalWords()) *
                                         100,
                                     1,
                                     wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                         wxNumberFormatter::Style::Style_WithThousandsSep)));

        if (listData)
            {
            listData->SetItemText(
                listDataItemCount, 0, _(L"Number of monosyllabic words"),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount, 1,
                wxNumberFormatter::ToString(project->GetTotalMonoSyllabicWords(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount++, 2,
                wxString::Format(_(L"%s%% of all words"),
                                 wxNumberFormatter::ToString(
                                     safe_divide<double>(project->GetTotalMonoSyllabicWords(),
                                                         project->GetTotalWords()) *
                                         100,
                                     1,
                                     wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                         wxNumberFormatter::Style::Style_WithThousandsSep)),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }

        // unique monosyllabic words
        htmlText += formatRow(_(L"Number of unique monosyllabic words:"),
                              wxNumberFormatter::ToString(
                                  project->GetTotalUniqueMonoSyllabicWords(), 0,
                                  wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                      wxNumberFormatter::Style::Style_WithThousandsSep)) +
                    L"\n</table>";

        if (listData)
            {
            listData->SetItemText(
                listDataItemCount, 0, _(L"Number of unique monosyllabic words"),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount++, 1,
                wxNumberFormatter::ToString(project->GetTotalUniqueMonoSyllabicWords(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }

        // 3+ syllable
        htmlText += tableStart + formatHeader(_(L"Complex Words"));
        if (project->GetTotal3PlusSyllabicWords() > 0)
            {
            currentLabel = _(L"Number of <a href=\"#HardWords\">complex (3+ syllable)</a> words:");
            }
        else
            {
            currentLabel = _(L"Number of complex (3+ syllable) words:");
            }
        htmlText += formatRow(
            currentLabel,
            wxNumberFormatter::ToString(project->GetTotal3PlusSyllabicWords(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep),
            wxString::Format(_(L"(%s%% of all words)"),
                             wxNumberFormatter::ToString(
                                 safe_divide<double>(project->GetTotal3PlusSyllabicWords(),
                                                     project->GetTotalWords()) *
                                     100,
                                 1,
                                 wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                     wxNumberFormatter::Style::Style_WithThousandsSep)));

        if (listData)
            {
            listData->SetItemText(
                listDataItemCount, 0, _(L"Number of complex (3+ syllable) words"),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount, 1,
                wxNumberFormatter::ToString(project->GetTotal3PlusSyllabicWords(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount++, 2,
                wxString::Format(_(L"%s%% of all words"),
                                 wxNumberFormatter::ToString(
                                     safe_divide<double>(project->GetTotal3PlusSyllabicWords(),
                                                         project->GetTotalWords()) *
                                         100,
                                     1,
                                     wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                         wxNumberFormatter::Style::Style_WithThousandsSep)),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        // unique 3+ syllable words
        htmlText += formatRow(_(L"Number of unique 3+ syllable words:"),
                              wxNumberFormatter::ToString(
                                  project->GetTotalUnique3PlusSyllableWords(), 0,
                                  wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                      wxNumberFormatter::Style::Style_WithThousandsSep)) +
                    L"\n</table>";

        if (listData)
            {
            listData->SetItemText(
                listDataItemCount, 0, _(L"Number of unique 3+ syllable words"),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount++, 1,
                wxNumberFormatter::ToString(project->GetTotalUnique3PlusSyllableWords(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }

        // long words (6 characters or more)
        htmlText += tableStart + formatHeader(_(L"Long Words"));
        if (project->GetTotalLongWords() > 0)
            {
            currentLabel = _(L"Number of <a href=\"#LongWords\">long (6+ characters)</a> words:");
            }
        else
            {
            currentLabel = _(L"Number of long (6+ characters) words:");
            }
        htmlText += formatRow(
            currentLabel,
            wxNumberFormatter::ToString(project->GetTotalLongWords(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep),
            wxString::Format(
                _(L"(%s%% of all words)"),
                wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetTotalLongWords(), project->GetTotalWords()) *
                        100,
                    1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes |
                        wxNumberFormatter::Style::Style_WithThousandsSep)));

        if (listData)
            {
            listData->SetItemText(
                listDataItemCount, 0, _(L"Number of long (6+ characters) words"),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount, 1,
                wxNumberFormatter::ToString(project->GetTotalLongWords(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount++, 2,
                wxString::Format(_(L"%s%% of all words"),
                                 wxNumberFormatter::ToString(
                                     safe_divide<double>(project->GetTotalLongWords(),
                                                         project->GetTotalWords()) *
                                         100,
                                     1,
                                     wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                         wxNumberFormatter::Style::Style_WithThousandsSep)),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        // unique long words
        htmlText += formatRow(_(L"Number of unique long words:"),
                              wxNumberFormatter::ToString(
                                  project->GetTotalUnique6CharsPlusWords(), 0,
                                  wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                      wxNumberFormatter::Style::Style_WithThousandsSep)) +
                    L"\n</table>";
        if (listData)
            {
            listData->SetItemText(
                listDataItemCount, 0, _(L"Number of unique long words"),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount++, 1,
                wxNumberFormatter::ToString(project->GetTotalUnique6CharsPlusWords(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }

        if (project->IsSmogLikeTestIncluded())
            {
            // SMOG
            htmlText += tableStart + formatHeader(_(L"SMOG Words"));
            htmlText += formatRow(
                _(L"Number of SMOG hard words (3+ syllables, numerals fully syllabized):"),
                wxNumberFormatter::ToString(
                    project->GetTotal3PlusSyllabicWordsNumeralsFullySyllabized(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes |
                        wxNumberFormatter::Style::Style_WithThousandsSep),
                wxString::Format(
                    _(L"(%s%% of all words)"),
                    wxNumberFormatter::ToString(
                        safe_divide<double>(
                            project->GetTotal3PlusSyllabicWordsNumeralsFullySyllabized(),
                            project->GetTotalWords()) *
                            100,
                        1,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep)));

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0,
                    _(L"Number of SMOG hard words (3+ syllables, numerals fully syllabized)"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount, 1,
                    wxNumberFormatter::ToString(
                        project->GetTotal3PlusSyllabicWordsNumeralsFullySyllabized(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 2,
                    wxString::Format(
                        _(L"%s%% of all words"),
                        wxNumberFormatter::ToString(
                            safe_divide<double>(
                                project->GetTotal3PlusSyllabicWordsNumeralsFullySyllabized(),
                                project->GetTotalWords()) *
                                100,
                            1,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                wxNumberFormatter::Style::Style_WithThousandsSep)),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            // unique SMOG words
            htmlText +=
                formatRow(_(L"Number of unique SMOG hard words:"),
                          wxNumberFormatter::ToString(
                              project->GetUnique3PlusSyllabicWordsNumeralsFullySyllabized(), 0,
                              wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                  wxNumberFormatter::Style::Style_WithThousandsSep)) +
                L"\n</table>";
            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of unique SMOG hard words"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 1,
                    wxNumberFormatter::ToString(
                        project->GetUnique3PlusSyllabicWordsNumeralsFullySyllabized(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }
        if (project->GetReadabilityTests().is_test_included(ReadabilityMessages::GUNNING_FOG()))
            {
            // Fog
            htmlText += tableStart +
                        // TRANSLATORS: "Fog" is the name of a test, don't translate that part.
                        formatHeader(_(L"Fog Words")) +
                        // TRANSLATORS: "Fog" is the name of a test, don't translate that part.
                        formatRow(_(L"Number of Fog hard words (3+ syllables, with <a "
                                    "href=\"#FogHelp\">exceptions</a>):"),
                                  wxNumberFormatter::ToString(
                                      project->GetTotalHardWordsFog(), 0,
                                      wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                          wxNumberFormatter::Style::Style_WithThousandsSep),
                                  wxString::Format(
                                      _(L"(%s%% of all words)"),
                                      wxNumberFormatter::ToString(
                                          safe_divide<double>(project->GetTotalHardWordsFog(),
                                                              project->GetTotalWords()) *
                                              100,
                                          1,
                                          wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                              wxNumberFormatter::Style::Style_WithThousandsSep)));

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0,
                    _(L"Number of Fog hard words (3+ syllables, with exceptions)"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount, 1,
                    wxNumberFormatter::ToString(
                        project->GetTotalHardWordsFog(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 2,
                    wxString::Format(_(L"%s%% of all words"),
                                     wxNumberFormatter::ToString(
                                         safe_divide<double>(project->GetTotalHardWordsFog(),
                                                             project->GetTotalWords()) *
                                             100,
                                         1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep)),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            // unique fog words
            htmlText += formatRow(_(L"Number of unique Fog hard words:"),
                                  wxNumberFormatter::ToString(
                                      project->GetTotalUniqueHardFogWords(), 0,
                                      wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                          wxNumberFormatter::Style::Style_WithThousandsSep)) +
                        L"\n</table>";
            if (listData)
                {
                // TRANSLATORS: "Fog" is a test name; don't translate it.
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of unique Fog hard words"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 1,
                    wxNumberFormatter::ToString(
                        project->GetTotalUniqueHardFogWords(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }

        if (project->IsIncludingDolchSightWords())
            {
            // Resize the list grid (if using one) to fit its data, then the call to Dolch
            // stats will append to that and resize it again.
            if (listData)
                {
                listData->SetSize(listDataItemCount, 3);
                }
            htmlText +=
                FormatDolchStatisticsInfo(project, statsInfo, false, attentionColor, listData);
            // ...then we will add some more rows for the rest of the stats.
            if (listData)
                {
                listDataItemCount = listData->GetItemCount();
                listData->SetSize(listDataItemCount + MAX_SUMMARY_STAT_ROWS, 3);
                }
            }

        const size_t totalWordCountForDC =
            (project->GetDaleChallTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                project->GetTotalWordsFromCompleteSentencesAndHeaders() :
                project->GetTotalWords();
        if (project->IsDaleChallLikeTestIncluded())
            {
            // DC
            htmlText += tableStart + formatHeader(_(L"Dale-Chall Unfamiliar Words"));

            if (project->GetTotalHardWordsDaleChall() > 0)
                {
                currentLabel =
                    _(L"Number of <a href=\"#DaleChallWords\">Dale-Chall</a> unfamiliar words:");
                }
            else
                {
                currentLabel = _(L"Number of Dale-Chall unfamiliar words:");
                }
            htmlText += formatRow(
                currentLabel,
                wxNumberFormatter::ToString(project->GetTotalHardWordsDaleChall(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                wxString::Format(_(L"(%s%% of all words)"),
                                 wxNumberFormatter::ToString(
                                     safe_divide<double>(project->GetTotalHardWordsDaleChall(),
                                                         totalWordCountForDC) *
                                         100,
                                     1,
                                     wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                         wxNumberFormatter::Style::Style_WithThousandsSep)));

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of Dale-Chall unfamiliar words"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount, 1,
                    wxNumberFormatter::ToString(
                        project->GetTotalHardWordsDaleChall(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 2,
                    wxString::Format(_(L"%s%% of all words"),
                                     wxNumberFormatter::ToString(
                                         safe_divide<double>(project->GetTotalHardWordsDaleChall(),
                                                             totalWordCountForDC) *
                                             100,
                                         1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep)),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            // unique Dale Chall hard words
            htmlText += formatRow(_(L"Number of unique Dale-Chall unfamiliar words:"),
                                  wxNumberFormatter::ToString(
                                      project->GetTotalUniqueDCHardWords(), 0,
                                      wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                          wxNumberFormatter::Style::Style_WithThousandsSep)) +
                        L"\n</table>";
            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of unique Dale-Chall unfamiliar words"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 1,
                    wxNumberFormatter::ToString(
                        project->GetTotalUniqueDCHardWords(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }

            if (project->GetDaleChallTextExclusionMode() ==
                SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings)
                {
                htmlText += FormatHtmlNoteSection(
                    // TRANSLATORS: "New Dale Chall" is a test name that should not be translated.
                    _(L"New Dale-Chall treats headers and sub-headers as full sentences and "
                      "excludes all "
                      "other incomplete sentences (regardless of your current analysis settings). "
                      "This is taken into account when calculating the unfamiliar word count and "
                      "percentage."));
                }
            if (project->IsIncludingStockerCatholicSupplement())
                {
                htmlText +=
                    // TRANSLATORS: "New Dale Chall" is a test name that should not be translated.
                    FormatHtmlNoteSection(_(L"Stocker's Catholic supplement is being included with "
                                            "the standard New Dale-Chall word list."));
                }
            }

        const size_t totalWordCountForHJ =
            (project->GetHarrisJacobsonTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                project->GetTotalWordsFromCompleteSentencesAndHeaders() :
                project->GetTotalWords();
        const size_t totalNumeralCountForHJ =
            (project->GetHarrisJacobsonTextExclusionMode() ==
             SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings) ?
                project->GetTotalNumeralsFromCompleteSentencesAndHeaders() :
                project->GetTotalNumerals();
        if (project->GetReadabilityTests().is_test_included(ReadabilityMessages::HARRIS_JACOBSON()))
            {
            // Harris-Jacobson
            htmlText += tableStart + formatHeader(_(L"Harris-Jacobson Unfamiliar Words"));
            if (project->GetTotalHardWordsHarrisJacobson() > 0)
                {
                currentLabel = _(L"Number of <a href=\"#HarrisJacobsonWords\">Harris-Jacobson</a> "
                                 "unfamiliar words:");
                }
            else
                {
                currentLabel = _(L"Number of Harris-Jacobson unfamiliar words:");
                }
            htmlText += formatRow(
                currentLabel,
                wxNumberFormatter::ToString(project->GetTotalHardWordsHarrisJacobson(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                wxString::Format(
                    _(L"(%s%% of all words)"),
                    wxNumberFormatter::ToString(
                        safe_divide<double>(project->GetTotalHardWordsHarrisJacobson(),
                                            totalWordCountForHJ - totalNumeralCountForHJ) *
                            100,
                        1,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep)));

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of Harris-Jacobson unfamiliar words"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount, 1,
                    wxNumberFormatter::ToString(
                        project->GetTotalHardWordsHarrisJacobson(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 2,
                    wxString::Format(
                        _(L"%s%% of all words"),
                        wxNumberFormatter::ToString(
                            safe_divide<double>(project->GetTotalHardWordsHarrisJacobson(),
                                                totalWordCountForHJ - totalNumeralCountForHJ) *
                                100,
                            1,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                wxNumberFormatter::Style::Style_WithThousandsSep)),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }

            // unique Harris-Jacobson hard words
            htmlText += formatRow(_(L"Number of unique Harris-Jacobson unfamiliar words:"),
                                  wxNumberFormatter::ToString(
                                      project->GetTotalUniqueHarrisJacobsonHardWords(), 0,
                                      wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                          wxNumberFormatter::Style::Style_WithThousandsSep)) +
                        L"\n</table>";
            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of unique Harris-Jacobson unfamiliar words"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 1,
                    wxNumberFormatter::ToString(
                        project->GetTotalUniqueHarrisJacobsonHardWords(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }

            if (project->GetHarrisJacobsonTextExclusionMode() ==
                SpecializedTestTextExclusion::ExcludeIncompleteSentencesExceptHeadings)
                {
                htmlText += FormatHtmlNoteSection(
                    _(L"Harris-Jacobson treats headers and sub-headers as full sentences and "
                      "excludes all other "
                      "incomplete sentences (regardless of your current analysis settings). "
                      "It also excludes all numerals from the total word count&mdash;this is taken "
                      "into account "
                      "when calculating the unfamiliar word count and percentage."));
                }
            }

        if (project->GetReadabilityTests().is_test_included(ReadabilityMessages::SPACHE()))
            {
            // Spache
            htmlText += tableStart + formatHeader(_(L"Spache Unfamiliar Words"));
            if (project->GetTotalHardWordsSpache() > 0)
                {
                currentLabel =
                    _(L"Number of <a href=\"#SpacheWords\">Spache</a> unfamiliar words:");
                }
            else
                {
                currentLabel = _(L"Number of Spache unfamiliar words:");
                }
            htmlText += formatRow(
                currentLabel,
                wxNumberFormatter::ToString(project->GetTotalHardWordsSpache(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                wxString::Format(_(L"(%s%% of all words)"),
                                 wxNumberFormatter::ToString(
                                     safe_divide<double>(project->GetTotalHardWordsSpache(),
                                                         project->GetTotalWords()) *
                                         100,
                                     1,
                                     wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                         wxNumberFormatter::Style::Style_WithThousandsSep)));

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of Spache unfamiliar words"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount, 1,
                    wxNumberFormatter::ToString(
                        project->GetTotalHardWordsSpache(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 2,
                    wxString::Format(_(L"%s%% of all words"),
                                     wxNumberFormatter::ToString(
                                         safe_divide<double>(project->GetTotalHardWordsSpache(),
                                                             project->GetTotalWords()) *
                                             100,
                                         1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep)),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }

            // unique Spache hard words
            htmlText += formatRow(_(L"Number of unique Spache unfamiliar words:"),
                                  wxNumberFormatter::ToString(
                                      project->GetTotalUniqueHardWordsSpache(), 0,
                                      wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                          wxNumberFormatter::Style::Style_WithThousandsSep)) +
                        L"\n</table>";
            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of unique Spache unfamiliar words"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 1,
                    wxNumberFormatter::ToString(
                        project->GetTotalUniqueHardWordsSpache(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }
        if (project->GetReadabilityTests().is_test_included(ReadabilityMessages::EFLAW()))
            {
            // EFLAW Miniwords
            htmlText += tableStart + formatHeader(_(L"McAlpine EFLAW Miniwords"));
            htmlText += formatRow(
                _(L"Number of McAlpine EFLAW miniwords (1-3 characters, except for numerals):"),
                wxNumberFormatter::ToString(project->GetTotalMiniWords(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                wxString::Format(_(L"(%s%% of all words)"),
                                 wxNumberFormatter::ToString(
                                     safe_divide<double>(project->GetTotalMiniWords(),
                                                         project->GetTotalWords()) *
                                         100,
                                     1,
                                     wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                         wxNumberFormatter::Style::Style_WithThousandsSep)));

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0,
                    _(L"Number of McAlpine EFLAW miniwords (1-3 characters, except for numerals)"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount, 1,
                    wxNumberFormatter::ToString(
                        project->GetTotalMiniWords(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 2,
                    wxString::Format(_(L"%s%% of all words"),
                                     wxNumberFormatter::ToString(
                                         safe_divide<double>(project->GetTotalMiniWords(),
                                                             project->GetTotalWords()) *
                                             100,
                                         1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep)),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }

            // unique EFLAW miniwords words
            htmlText += formatRow(_(L"Number of unique McAlpine EFLAW miniwords words:"),
                                  wxNumberFormatter::ToString(
                                      project->GetTotalUniqueMiniWords(), 0,
                                      wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                          wxNumberFormatter::Style::Style_WithThousandsSep)) +
                        L"\n</table>";
            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of unique McAlpine EFLAW miniwords words"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 1,
                    wxNumberFormatter::ToString(
                        project->GetTotalUniqueMiniWords(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }

        // go through the custom readability tests
        for (auto pos = project->GetCustTestsInUse().cbegin();
             pos != project->GetCustTestsInUse().cend(); ++pos)
            {
            if (!pos->GetIterator()->is_using_familiar_words())
                {
                continue;
                }

            // special logic for calculating word percentage if test is based on DC or HJ
            const size_t totalWordCountForCustomTest =
                pos->IsDaleChallFormula()      ? totalWordCountForDC :
                pos->IsHarrisJacobsonFormula() ? totalWordCountForHJ - totalNumeralCountForHJ :
                                                 project->GetTotalWords();

            wxString testName(pos->GetIterator()->get_name().c_str());
            testName = htmlEncode({ testName.wc_str(), testName.length() }, true).c_str();
            // total unfamiliar words
            htmlText += tableStart + formatHeader(testName) +
                        formatRow(wxString::Format(_(L"Number of %s unfamiliar words:"), testName),
                                  wxNumberFormatter::ToString(
                                      pos->GetUnfamiliarWordCount(), 0,
                                      wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                          wxNumberFormatter::Style::Style_WithThousandsSep),
                                  wxString::Format(
                                      _(L"(%s%% of all words)"),
                                      wxNumberFormatter::ToString(
                                          safe_divide<double>(pos->GetUnfamiliarWordCount(),
                                                              totalWordCountForCustomTest) *
                                              100,
                                          1,
                                          wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                              wxNumberFormatter::Style::Style_WithThousandsSep)));

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0,
                    wxString::Format(_(L"Number of %s unfamiliar words"),
                                     pos->GetIterator()->get_name().c_str()),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount, 1,
                    wxNumberFormatter::ToString(
                        pos->GetUnfamiliarWordCount(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 2,
                    wxString::Format(_(L"%s%% of all words"),
                                     wxNumberFormatter::ToString(
                                         safe_divide<double>(pos->GetUnfamiliarWordCount(),
                                                             totalWordCountForCustomTest) *
                                             100,
                                         1,
                                         wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                             wxNumberFormatter::Style::Style_WithThousandsSep)),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }

            // unique unfamiliar words
            htmlText +=
                formatRow(wxString::Format(_(L"Number of unique %s unfamiliar words:"), testName),
                          wxNumberFormatter::ToString(
                              pos->GetUniqueUnfamiliarWordCount(), 0,
                              wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                  wxNumberFormatter::Style::Style_WithThousandsSep)) +
                L"\n</table>";
            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0,
                    wxString::Format(_(L"Number of unique %s unfamiliar words"),
                                     pos->GetIterator()->get_name().c_str()),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 1,
                    wxNumberFormatter::ToString(
                        pos->GetUniqueUnfamiliarWordCount(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }
        }

    if (statsInfo.IsGrammarEnabled() && project->GetGrammarInfo().IsAnyFeatureEnabled())
        {
        // grammar section
        htmlText += tableStart + formatHeader(_(L"Grammar"));
        // misspelled words
        if (project->GetProjectLanguage() == readability::test_language::english_test &&
            project->GetGrammarInfo().IsMisspellingsEnabled())
            {
            currentLabel.clear();
            if (project->GetMisspelledWordCount() != 0)
                {
                currentLabel += L"<a href=\"#Misspellings\">";
                }
            currentLabel += _(L"Misspellings:");
            if (project->GetMisspelledWordCount() != 0)
                {
                currentLabel += L"</a>";
                }
            htmlText += formatRow(
                currentLabel,
                wxNumberFormatter::ToString(project->GetMisspelledWordCount(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep));

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of possible misspellings"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 1,
                    wxNumberFormatter::ToString(
                        project->GetMisspelledWordCount(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }
        // repeated words
        if (project->GetGrammarInfo().IsRepeatedWordsEnabled())
            {
            currentLabel.clear();
            if (project->GetDuplicateWordCount() != 0)
                {
                currentLabel += L"<a href=\"#RepeatedWords\">";
                }
            currentLabel += _(L"Repeated words:");
            if (project->GetDuplicateWordCount() != 0)
                {
                currentLabel += L"</a>";
                }
            htmlText += formatRow(
                currentLabel,
                wxNumberFormatter::ToString(project->GetDuplicateWordCount(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep));

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of repeated words"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 1,
                    wxNumberFormatter::ToString(
                        project->GetDuplicateWordCount(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }
        /// @todo Need to add these grammar features to other languages too
        if (project->GetProjectLanguage() == readability::test_language::english_test)
            {
            // mismatched articles
            if (project->GetGrammarInfo().IsArticleMismatchesEnabled())
                {
                currentLabel.clear();
                if (project->GetMismatchedArticleCount() != 0)
                    {
                    currentLabel += L"<a href=\"#MismatchedArticles\">";
                    }
                currentLabel += _(L"Article mismatches:");
                if (project->GetMismatchedArticleCount() != 0)
                    {
                    currentLabel += L"</a>";
                    }
                htmlText += formatRow(currentLabel,
                                      wxNumberFormatter::ToString(
                                          project->GetMismatchedArticleCount(), 0,
                                          wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                              wxNumberFormatter::Style::Style_WithThousandsSep));

                if (listData)
                    {
                    listData->SetItemText(
                        listDataItemCount, 0, _(L"Number of article mismatches"),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    listData->SetItemText(
                        listDataItemCount++, 1,
                        wxNumberFormatter::ToString(
                            project->GetMismatchedArticleCount(), 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                wxNumberFormatter::Style::Style_WithThousandsSep),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    }
                }
            // Wording Errors & Known Misspellings
            if (project->GetGrammarInfo().IsWordingErrorsEnabled())
                {
                currentLabel.clear();
                if (project->GetWordingErrorCount() != 0)
                    {
                    currentLabel += L"<a href=\"#WordingErrors\">";
                    }
                currentLabel += _(L"Wording errors & misspellings:");
                if (project->GetWordingErrorCount() != 0)
                    {
                    currentLabel += L"</a>";
                    }
                htmlText += formatRow(currentLabel,
                                      wxNumberFormatter::ToString(
                                          project->GetWordingErrorCount(), 0,
                                          wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                              wxNumberFormatter::Style::Style_WithThousandsSep));

                if (listData)
                    {
                    listData->SetItemText(
                        listDataItemCount, 0, _(L"Number of wording errors & misspellings"),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    listData->SetItemText(
                        listDataItemCount++, 1,
                        wxNumberFormatter::ToString(
                            project->GetWordingErrorCount(), 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                wxNumberFormatter::Style::Style_WithThousandsSep),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    }
                }
            // redundant phrase count
            if (project->GetGrammarInfo().IsRedundantPhrasesEnabled())
                {
                currentLabel.clear();
                if (project->GetRedundantPhraseCount() != 0)
                    {
                    currentLabel += L"<a href=\"#RedundantPhrases\">";
                    }
                currentLabel += _(L"Redundant phrases:");
                if (project->GetRedundantPhraseCount() != 0)
                    {
                    currentLabel += L"</a>";
                    }
                htmlText += formatRow(currentLabel,
                                      wxNumberFormatter::ToString(
                                          project->GetRedundantPhraseCount(), 0,
                                          wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                              wxNumberFormatter::Style::Style_WithThousandsSep));

                if (listData)
                    {
                    listData->SetItemText(
                        listDataItemCount, 0, _(L"Number of redundant phrases"),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    listData->SetItemText(
                        listDataItemCount++, 1,
                        wxNumberFormatter::ToString(
                            project->GetRedundantPhraseCount(), 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                wxNumberFormatter::Style::Style_WithThousandsSep),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    }
                }
            // overused words (by sentence)
            if (project->GetGrammarInfo().IsOverUsedWordsBySentenceEnabled())
                {
                currentLabel.clear();
                if (project->GetOverusedWordsBySentenceCount() != 0)
                    {
                    currentLabel += L"<a href=\"#OverusedWordsBySentence\">";
                    }
                currentLabel += _(L"Overused words (x sentence):");
                if (project->GetOverusedWordsBySentenceCount() != 0)
                    {
                    currentLabel += L"</a>";
                    }
                htmlText += formatRow(currentLabel,
                                      wxNumberFormatter::ToString(
                                          project->GetOverusedWordsBySentenceCount(), 0,
                                          wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                              wxNumberFormatter::Style::Style_WithThousandsSep));

                if (listData)
                    {
                    listData->SetItemText(
                        listDataItemCount, 0, _(L"Number of overused words (x sentence)"),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    listData->SetItemText(
                        listDataItemCount++, 1,
                        wxNumberFormatter::ToString(
                            project->GetOverusedWordsBySentenceCount(), 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                wxNumberFormatter::Style::Style_WithThousandsSep),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    }
                }
            // wordiness
            if (project->GetGrammarInfo().IsWordyPhrasesEnabled())
                {
                currentLabel.clear();
                if (project->GetWordyPhraseCount() != 0)
                    {
                    currentLabel += L"<a href=\"#WordyPhrases\">";
                    }
                currentLabel += _(L"Wordy items:");
                if (project->GetWordyPhraseCount() != 0)
                    {
                    currentLabel += L"</a>";
                    }
                htmlText += formatRow(currentLabel,
                                      wxNumberFormatter::ToString(
                                          project->GetWordyPhraseCount(), 0,
                                          wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                              wxNumberFormatter::Style::Style_WithThousandsSep));

                if (listData)
                    {
                    listData->SetItemText(
                        listDataItemCount, 0, _(L"Number of wordy items"),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    listData->SetItemText(
                        listDataItemCount++, 1,
                        wxNumberFormatter::ToString(
                            project->GetWordyPhraseCount(), 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                wxNumberFormatter::Style::Style_WithThousandsSep),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    }
                }
            // cliches
            if (project->GetGrammarInfo().IsClichesEnabled())
                {
                currentLabel.clear();
                if (project->GetClicheCount() > 0)
                    {
                    currentLabel += L"<a href=\"#Cliches\">";
                    }
                currentLabel += BaseProjectView::GetClichesTabLabel() + L":";
                if (project->GetClicheCount() != 0)
                    {
                    currentLabel += L"</a>";
                    }
                htmlText += formatRow(currentLabel,
                                      wxNumberFormatter::ToString(
                                          project->GetClicheCount(), 0,
                                          wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                              wxNumberFormatter::Style::Style_WithThousandsSep));

                if (listData)
                    {
                    listData->SetItemText(
                        listDataItemCount, 0, _(L"Number of clich\u00E9s"),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    listData->SetItemText(
                        listDataItemCount++, 1,
                        wxNumberFormatter::ToString(
                            project->GetClicheCount(), 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                wxNumberFormatter::Style::Style_WithThousandsSep),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    }
                }
            // passive voice
            if (project->GetGrammarInfo().IsPassiveVoiceEnabled())
                {
                currentLabel.clear();
                if (project->GetPassiveVoicesCount() != 0)
                    {
                    currentLabel += L"<a href=\"#PassiveVoice\">";
                    }
                currentLabel += _(L"Passive voice:");
                if (project->GetPassiveVoicesCount() != 0)
                    {
                    currentLabel += L"</a>";
                    }
                htmlText += formatRow(currentLabel,
                                      wxNumberFormatter::ToString(
                                          project->GetPassiveVoicesCount(), 0,
                                          wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                              wxNumberFormatter::Style::Style_WithThousandsSep));

                if (listData)
                    {
                    listData->SetItemText(
                        listDataItemCount, 0, _(L"Number of passive voices"),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    listData->SetItemText(
                        listDataItemCount++, 1,
                        wxNumberFormatter::ToString(
                            project->GetPassiveVoicesCount(), 0,
                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                wxNumberFormatter::Style::Style_WithThousandsSep),
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    }
                }
            }
        // conjunction starting sentences
        if (project->GetGrammarInfo().IsConjunctionStartingSentencesEnabled())
            {
            currentLabel.clear();
            if (project->GetSentenceStartingWithConjunctionsCount() > 0)
                {
                currentLabel += L"<a href=\"#SentenceStartingWithConjunctions\">";
                }
            currentLabel += BaseProjectView::GetSentenceStartingWithConjunctionsLabel() + L":";
            if (project->GetSentenceStartingWithConjunctionsCount() != 0)
                {
                currentLabel += L"</a>";
                }
            htmlText += formatRow(
                currentLabel,
                wxNumberFormatter::ToString(project->GetSentenceStartingWithConjunctionsCount(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep));

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Number of sentences that begin with conjunctions"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 1,
                    wxNumberFormatter::ToString(
                        project->GetSentenceStartingWithConjunctionsCount(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }
        // lowercased sentences
        if (project->GetGrammarInfo().IsLowercaseSentencesEnabled())
            {
            currentLabel.clear();
            if (project->GetSentenceStartingWithLowercaseCount() > 0)
                {
                currentLabel += L"<a href=\"#SentenceStartingWithLowercase\">";
                }
            if (project->GetSentenceStartMustBeUppercased())
                {
                currentLabel += L"* ";
                }
            currentLabel += BaseProjectView::GetSentenceStartingWithLowercaseLabel() + L":";
            if (project->GetSentenceStartingWithLowercaseCount() != 0)
                {
                currentLabel += L"</a>";
                }
            htmlText += formatRow(
                currentLabel,
                wxNumberFormatter::ToString(project->GetSentenceStartingWithLowercaseCount(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep));

            if (listData)
                {
                listData->SetItemText(
                    listDataItemCount, 0,
                    _(L"Number of Sentences that begin with lowercased words"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 1,
                    wxNumberFormatter::ToString(
                        project->GetSentenceStartingWithLowercaseCount(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }

        htmlText +=
            L"\n<tr><td colspan=\"3\">" +
            FormatHtmlNoteSection(
                _(L"Grammar statistics do not directly factor into readability formulas; however, "
                  "they can be useful suggestions for improving the document.")) +
            L"</td></tr>\n";

        if (project->GetSentenceStartMustBeUppercased())
            {
            htmlText += L"\n<tr><td colspan=\"3\">" +
                        FormatHtmlNoteSection(_(L"* This project's sentence-deduction method is "
                                                "set to only accept capitalized sentences. "
                                                "Lowercased-sentence detection will be limited to "
                                                "sentences that begin new paragraphs.")) +
                        L"</td></tr>\n";
            }

        htmlText += L"\n</table>";
        }

    if (statsInfo.IsExtendedInformationEnabled())
        {
        // file/text stream info section
        htmlText += tableStart + formatHeader((!project->GetOriginalDocumentFilePath().empty() ?
                                                   _(L"File Information") :
                                                   _(L"Text Information")));

        wxString fileModDate;

        // file path (if not manually entered text)
        if (!project->GetOriginalDocumentFilePath().empty())
            {
            htmlText += formatRow(_(L"Path:"), wxString{}, project->GetOriginalDocumentFilePath());

            // get modified time also if a local file
            FilePathResolver resolvePath;
            resolvePath.ResolvePath(project->GetOriginalDocumentFilePath(), false);
            if (resolvePath.IsLocalOrNetworkFile() &&
                wxFileName::FileExists(resolvePath.GetResolvedPath()))
                {
                const auto modDate =
                    wxFileName{ resolvePath.GetResolvedPath() }.GetModificationTime();
                // wxDateTime::Format doesn't appear to handle %X, so do it manually
                wxString amStr, pmStr, timeStr;
                wxDateTime::GetAmPmStrings(&amStr, &pmStr);
                if (!amStr.empty() && !pmStr.empty())
                    {
                    timeStr = modDate.Format(L"%I:%M %p").MakeUpper();
                    }
                else
                    {
                    timeStr = modDate.Format(L"%H:%M");
                    }

                // the default locale formatting is cryptic looking, so format
                // it in a more readable way that most locales would still understand
                fileModDate = wxString::Format( // TRANSLATORS: placeholders are date and time
                    _(L"%s at %s"), modDate.Format(L"%B %d, %Y"), timeStr);
                }
            }
        if (!project->GetAppendedDocumentFilePath().empty())
            {
            htmlText += formatRow(_(L"Additional document path:"), wxString{},
                                  project->GetAppendedDocumentFilePath());
            }
        // text size
        htmlText += formatRow(
            _(L"Text size:"), wxString{},
            wxString::Format( // TRANSLATORS: %s is a file size
                _(L"%s Kbs."),
                wxNumberFormatter::ToString(safe_divide<double>(project->GetTextSize(), 1024), 2,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep)));
        if (!fileModDate.empty())
            {
            htmlText += formatRow(_(L"Last modified:"), wxString{}, fileModDate);
            }
        htmlText += L"\n</table>";

        if (listData)
            {
            listData->SetItemText(
                listDataItemCount, 0, _(L"Text size"),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            listData->SetItemText(
                listDataItemCount++, 1,
                wxString::Format( // TRANSLATORS: %s is a file size
                    _(L"%s Kbs."), wxNumberFormatter::ToString(
                                       safe_divide<double>(project->GetTextSize(), 1024), 2,
                                       wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                           wxNumberFormatter::Style::Style_WithThousandsSep)),
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());

            if (!fileModDate.empty())
                {
                listData->SetItemText(
                    listDataItemCount, 0, _(L"Last modified"),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                listData->SetItemText(
                    listDataItemCount++, 1, fileModDate,
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }
        }

    // Notes section
    if (statsInfo.IsNotesEnabled() &&
        (project->GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis ||
         project->GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings ||
         project->GetTotalWords() < 300 || overlyLongSentencePercentage >= 40 ||
         exclamatorySentencePercentage >= 25 || averageCharacterCount >= 6 ||
         averageSyllableCount >= 4 || !project->GetAppendedDocumentFilePath().empty()))
        {
        htmlText += tableStart + formatNoteHeader(_(L"Notes"));
        if (project->GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis)
            {
            htmlText += L"\n<tr><td style='width:100%;'><a name=\"incompsent\">";
            htmlText += _(L"Incomplete sentences have been excluded from the analysis. "
                          "All words from incomplete sentences were ignored and not factored into "
                          "these statistics (except for grammar information).");
            htmlText += L"</a></td></tr>";
            }
        else if (project->GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings)
            {
            htmlText += L"\n<tr><td style='width:100%;'><a name=\"incompsent\">";
            htmlText += _(L"Lists and tables have been excluded from the analysis. "
                          "All words from these sentences were ignored and not factored into these "
                          "statistics (except for grammar information).");
            htmlText += L"</a></td></tr>";
            }
        if (!project->GetAppendedDocumentFilePath().empty())
            {
            htmlText += L"\n<tr><td style='width:100%;'>";
            htmlText +=
                wxString::Format(_(L"An additional document (\"%s\") has been appended and "
                                   "included in the analysis."),
                                 wxFileName(project->GetAppendedDocumentFilePath()).GetFullName());
            htmlText += L"</td></tr>";
            }
        if (statsInfo.IsExtendedInformationEnabled())
            {
            htmlText += L"\n<tr><td style='width:100%;'>";
            htmlText +=
                _(L"Averages are calculated using arithmetic mean "
                  "(the summation of all values in a range divided by the number of items).");
            htmlText += L"</td></tr>";
            }
        if (project->GetTotalWords() < 300)
            {
            htmlText += L"\n<tr><td style='width:100%;'>";
            htmlText += wxString::Format(
                _(L"This document only contains %s words. "
                  "Most readability tests require a minimum of 300 words to be able to generate "
                  "meaningful results."),
                wxNumberFormatter::ToString(project->GetTotalWords(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes));
            htmlText += L"</td></tr>";
            }
        if (overlyLongSentencePercentage >= 40)
            {
            htmlText += L"\n<tr><td style='width:100%;'>";
            htmlText += _(L"A large percentage of sentences are overly long.");
            htmlText += L"</td></tr>";
            }
        if (exclamatorySentencePercentage >= 25)
            {
            htmlText += L"\n<tr><td style='width:100%;'>";
            htmlText += _(L"A large percentage of sentences are exclamatory, "
                          "giving the document an overall angry/excited tone. Is this intended?");
            htmlText += L"</td></tr>";
            }
        if (averageCharacterCount >= 6)
            {
            htmlText += L"\n<tr><td style='width:100%;'>";
            htmlText += _(L"The average word length is high.");
            htmlText += L"</td></tr>";
            }
        if (averageSyllableCount >= 4)
            {
            htmlText += L"\n<tr><td style='width:100%;'>";
            htmlText += _(L"The average word complexity (syllable count) is high.");
            htmlText += L"</td></tr>";
            }
        htmlText += L"\n</table>";
        }

    // trim off any trailing breaks
    htmlText = TrimTrailingBreaks(htmlText);

    if (listData)
        {
        listData->SetSize(listDataItemCount, listData->GetColumnCount());
        }

    return htmlText;
    }

//------------------------------------------------
wxString ProjectReportFormat::FormatTestResult(const wxString& score,
                                               const readability::readability_test& theTest,
                                               const wxString& note /*= wxString{}*/)
    {
    return wxString::Format(L"<tr><td style='vertical-align:top; "
                            "width:60%%;'><p>%s</p><p>%s</p></td><td>%s</td></tr>\n",
                            score, theTest.get_description().c_str(),
                            ProjectReportFormat::FormatTestFactors(theTest)) +
           (!note.empty() ? wxString::Format(L"<tr><td colspan='2'>%s</td></tr>\n",
                                             ProjectReportFormat::FormatHtmlNoteSection(note)) :
                            wxString{});
    }

//------------------------------------------------
wxString ProjectReportFormat::FormatTestFactors(const readability::readability_test& test)
    {
    wxString htmlText =
        wxString::Format(L"<table border='0' style='width:100%%'>\n"
                         "<tr><th colspan='2' style='text-align:left;'>%s</th></tr>",
                         _(L"Difficulty Prediction Factors"));

    const auto formatRow = [](const wxString& label, const bool value, const bool gray) -> wxString
    {
        return wxString::Format(
            L"\n<tr style='%s'><td><span style='%s'>%s</span></td>"
            "<td style='text-align:center;'><span style='%s'>%s</span></td></tr>",
            (gray ? L"background:#F2F2F2;" : L""), (gray ? L"color:#000000;" : L""), label,
            (gray ? L"color:#000000;" : L""), (value ? L"&nbsp;X&nbsp;" : L"&nbsp;&nbsp;&nbsp;"));
    };

    // word complexity
    if (test.has_factor(readability::test_factor::word_complexity_2_plus_syllables))
        {
        htmlText += formatRow(_(L"Word complexity (2 or more syllables)"), true, true);
        }
    else if (test.has_factor(readability::test_factor::word_complexity_3_plus_syllables))
        {
        htmlText += formatRow(_(L"Word complexity (3 or more syllables)"), true, true);
        }
    else if (test.has_factor(readability::test_factor::word_complexity_density))
        {
        htmlText += formatRow(_(L"Density of complex words"), true, true);
        }
    else
        {
        htmlText += formatRow(_(L"Word complexity (syllable counts)"),
                              test.has_factor(readability::test_factor::word_complexity), true);
        }

    // word length
    if (test.has_factor(readability::test_factor::word_length_3_less))
        {
        htmlText += formatRow(_(L"Word length (3 or less characters)"), true, false);
        }
    else if (test.has_factor(readability::test_factor::word_length_6_plus))
        {
        htmlText += formatRow(_(L"Word length (6 or more characters)"), true, false);
        }
    else if (test.has_factor(readability::test_factor::word_length_7_plus))
        {
        htmlText += formatRow(_(L"Word length (7 or more characters)"), true, false);
        }
    else
        {
        htmlText += formatRow(_(L"Word length"),
                              test.has_factor(readability::test_factor::word_length), false);
        }

    // word familiarity
    if (test.has_factor(readability::test_factor::word_familiarity_spache))
        {
        htmlText += formatRow(wxString::Format(_(L"Word familiarity (%s rules)"), _DT(L"Spache")),
                              true, true);
        }
    else if (test.has_factor(readability::test_factor::word_familiarity_dale_chall))
        {
        htmlText += formatRow(
            wxString::Format(_(L"Word familiarity (%s rules)"), _DT(L"Dale-Chall")), true, true);
        }
    else if (test.has_factor(readability::test_factor::word_familiarity_harris_jacobson))
        {
        htmlText +=
            formatRow(wxString::Format(_(L"Word familiarity (%s rules)"), _DT(L"Harris-Jacobson")),
                      true, true);
        }
    else
        {
        htmlText += formatRow(_(L"Word familiarity"), false, true);
        }

    // sentence length
    htmlText += formatRow(_(L"Sentence length"),
                          test.has_factor(readability::test_factor::sentence_length), false);

    htmlText += L"\n</table>\n";
    return htmlText;
    }

//------------------------------------------------
wxString ProjectReportFormat::TrimTrailingBreaks(const wxString& text)
    {
    wxString trimmedText = text;
    trimmedText.Trim();
    const wxString trailingBreaks = L"<br />";
    while (trimmedText.ends_with(trailingBreaks))
        {
        trimmedText.RemoveLast(trailingBreaks.length());
        }
    return trimmedText;
    }

//------------------------------------------------
wxString ProjectReportFormat::FormatHtmlWarningSection(const wxString& note)
    {
    return wxString::Format(L"\n<table class='minipage' style='width:100%%;' border='0'>\n"
                            "<tr><th rowspan='2' style='width:20%%;'></th>"
                            "<th style='text-align:left; background:yellow;'><span "
                            "style='color:black;'>%s</span></th></tr>\n"
                            "<tr><td style='text-align:left'>%s</td></tr></table>\n",
                            wxString::Format( // TRANSLATORS: %s is a warning emoji (icon)
                                _(L"%s Warning"), BaseProjectView::GetWarningEmoji()),
                            note);
    }

//------------------------------------------------
wxString ProjectReportFormat::FormatHtmlNoteSection(const wxString& note)
    {
    // note that wxHTML doesn't support 'color' style on the <tr> element,
    // so we need to use a <span> for that
    return wxString::Format(L"\n<table class='minipage' style='width:100%%;' border='0'>\n"
                            "<tr><th rowspan='2' style='width:20%%;'></th>"
                            "<th style='text-align:left; background:%s;'><span "
                            "style='color:white;'>%s</span></th></tr>\n"
                            "<tr><td style='text-align:left'>%s</td></tr></table>\n",
                            GetReportNoteHeaderColor().GetAsString(wxC2S_HTML_SYNTAX),
                            // can't use note emoji because wxHTML printing system can't handle it
                            _(L"Note"), note);
    }

//---------------------------------------------
wxString ProjectReportFormat::FormatSentence(
    const BaseProject* project, const grammar::sentence_info& sentence,
    std::vector<punctuation::punctuation_mark>::const_iterator& punctStart,
    const std::vector<punctuation::punctuation_mark>::const_iterator& punctEnd)
    {
    assert(project);
    if (project == nullptr)
        {
        return {};
        }
    wxString currentSentence;
    for (size_t i = sentence.get_first_word_index(); i <= sentence.get_last_word_index(); ++i)
        {
        // append any punctuation that should be in front of this word.
        // first, play "catch up" to get the punctuation iterator up to the current word.
        while (punctStart != punctEnd && punctStart->get_word_position() < i)
            {
            ++punctStart;
            }
        while (punctStart != punctEnd && punctStart->get_word_position() == i)
            {
            if (!punctStart->is_connected_to_previous_word())
                {
                currentSentence += punctStart->get_punctuation_mark();
                }
            ++punctStart;
            }
        currentSentence += project->GetWords()->get_words()[i].c_str();
        // append any punctuation that should be after this word
        while (punctStart != punctEnd && punctStart->get_word_position() == i + 1 &&
               punctStart->is_connected_to_previous_word())
            {
            currentSentence += punctStart->get_punctuation_mark();
            ++punctStart;
            }
        currentSentence += L' ';
        }
    currentSentence.Trim();

    bool quoteBeingMoved = false;
    wchar_t quoteChar{ L'\"' };
    if (!currentSentence.empty() &&
        characters::is_character::is_quote(currentSentence[currentSentence.length() - 1]))
        {
        quoteChar = currentSentence[currentSentence.length() - 1];
        quoteBeingMoved = true;
        currentSentence.RemoveLast();
        }
    currentSentence += sentence.get_ending_punctuation();
    if (quoteBeingMoved)
        {
        currentSentence += quoteChar;
        }

    return currentSentence;
    }
