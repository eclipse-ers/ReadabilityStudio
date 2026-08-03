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
        return _(L"Do not use stemming");
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
    for (size_t i = 0; i < formula.length(); /*handled in loop*/)
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
wxString ProjectReportFormat::FormatDolchCoverageRow(
    const wxString& rowLabel, size_t count, double percentage, const wxString& percentText,
    const wxString& listPercentText, const wxString& listDataLabel, size_t& listDataItemCount,
    const std::shared_ptr<Wisteria::UI::ListCtrlExDataProviderBase>& listData)
    {
    const wxString formattedCount =
        wxNumberFormatter::ToString(count, 0,
                                    wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                        wxNumberFormatter::Style::Style_WithThousandsSep);

    const bool needsAttention = (percentage >= 75);
    const wxString spanStart =
        needsAttention ? wxString{ L"<span class=\"attention\">" } : wxString{};
    const wxString spanEnd = needsAttention ? wxString{ L"</span>" } : wxString{};

    const wxString html = FormatDolchRow(rowLabel, spanStart + formattedCount + spanEnd,
                                         spanStart + percentText + spanEnd);

    if (listData != nullptr)
        {
        PopulateListRow(listDataLabel, formattedCount, listPercentText, listDataItemCount,
                        listData);
        }

    return html;
    }

//------------------------------------------------
wxString ProjectReportFormat::FormatDolchWordsRow(
    const wxString& rowLabel, size_t count, size_t totalWords, const wxString& uniqueRowLabel,
    size_t uniqueCount, const wxString& listDataCountLabel, const wxString& listDataUniqueLabel,
    size_t& listDataItemCount,
    const std::shared_ptr<Wisteria::UI::ListCtrlExDataProviderBase>& listData)
    {
    wxString html;

    const wxString formattedCount =
        wxNumberFormatter::ToString(count, 0,
                                    wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                        wxNumberFormatter::Style::Style_WithThousandsSep);
    const wxString formattedPercentage =
        wxNumberFormatter::ToString(safe_divide<double>(count, totalWords) * 100, 1,
                                    wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                        wxNumberFormatter::Style::Style_WithThousandsSep);

    html += FormatDolchRow(rowLabel, formattedCount,
                           wxString::Format(_(L"(%s%% of all words)"), formattedPercentage));

    if (listData != nullptr)
        {
        listData->SetItemText(
            listDataItemCount, 0, listDataCountLabel,
            Wisteria::NumberFormatInfo{
                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
            std::numeric_limits<double>::quiet_NaN());
        listData->SetItemText(
            listDataItemCount, 1, formattedCount,
            Wisteria::NumberFormatInfo{
                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
            std::numeric_limits<double>::quiet_NaN());
        listData->SetItemText(
            listDataItemCount++, 2, wxString::Format(_(L"%s%% of all words"), formattedPercentage),
            Wisteria::NumberFormatInfo{
                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
            std::numeric_limits<double>::quiet_NaN());
        }

    const wxString formattedUnique =
        wxNumberFormatter::ToString(uniqueCount, 0,
                                    wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                        wxNumberFormatter::Style::Style_WithThousandsSep);

    html += FormatDolchRow(uniqueRowLabel, formattedUnique, L"");

    if (listData != nullptr)
        {
        listData->SetItemText(
            listDataItemCount, 0, listDataUniqueLabel,
            Wisteria::NumberFormatInfo{
                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
            std::numeric_limits<double>::quiet_NaN());
        listData->SetItemText(
            listDataItemCount++, 1, formattedUnique,
            Wisteria::NumberFormatInfo{
                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
            std::numeric_limits<double>::quiet_NaN());
        }

    return html;
    }

//------------------------------------------------
void ProjectReportFormat::PopulateListRow(
    const wxString& label, const wxString& value, size_t& listDataItemCount,
    const std::shared_ptr<Wisteria::UI::ListCtrlExDataProviderBase>& listData)
    {
    if (listData == nullptr)
        {
        return;
        }
    listData->SetItemText(listDataItemCount, 0, label,
                          Wisteria::NumberFormatInfo{
                              Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                          std::numeric_limits<double>::quiet_NaN());
    listData->SetItemText(listDataItemCount++, 1, value,
                          Wisteria::NumberFormatInfo{
                              Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                          std::numeric_limits<double>::quiet_NaN());
    }

//------------------------------------------------
void ProjectReportFormat::PopulateListRow(
    const wxString& label, const wxString& value, const wxString& percent,
    size_t& listDataItemCount,
    const std::shared_ptr<Wisteria::UI::ListCtrlExDataProviderBase>& listData)
    {
    if (listData == nullptr)
        {
        return;
        }
    listData->SetItemText(listDataItemCount, 0, label,
                          Wisteria::NumberFormatInfo{
                              Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                          std::numeric_limits<double>::quiet_NaN());
    listData->SetItemText(listDataItemCount, 1, value,
                          Wisteria::NumberFormatInfo{
                              Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                          std::numeric_limits<double>::quiet_NaN());
    listData->SetItemText(listDataItemCount++, 2, percent,
                          Wisteria::NumberFormatInfo{
                              Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                          std::numeric_limits<double>::quiet_NaN());
    }

//------------------------------------------------
wxString ProjectReportFormat::FormatDolchStatisticsInfo(
    const BaseProject* project, const StatisticsReportInfo& statsInfo,
    const bool includeExplanation,
    const std::shared_ptr<Wisteria::UI::ListCtrlExDataProviderBase>& listData)
    {
    wxASSERT(project);
    if (project == nullptr)
        {
        return {};
        }
    if (!project->IsIncludingDolchSightWords())
        {
        return {};
        }
    size_t listDataItemCount{ 0 };
    if (listData != nullptr)
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
        htmlText +=
            L"<div class='pill-message'><span class='pill'>" +
            _(L"No Dolch statistics <a href=\"#select-statistics\">currently selected</a>.") +
            L"</span></div>";
        }

    const wxString tableStart = L"<div class='explanation-card'>";

    if (statsInfo.IsDolchCoverageEnabled())
        {
        // list completions
        htmlText.append(tableStart).append(FormatDolchHeader(_(L"Dolch Word Coverage")));

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
        const wxString dolchConjunctionPercentageStr = wxNumberFormatter::ToString(
            dolchConjunctionPercentage, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        htmlText += FormatDolchCoverageRow(
            _(L"Conjunctions used:"),
            MAX_DOLCH_CONJUNCTION_WORDS - project->GetUnusedDolchConjunctions(),
            dolchConjunctionPercentage,
            wxString::Format(_(L"(%s%% of all Dolch conjunctions)"), dolchConjunctionPercentageStr),
            wxString::Format(_(L"%s%% of all Dolch conjunctions"), dolchConjunctionPercentageStr),
            _(L"Number of conjunctions used"), listDataItemCount, listData);
        // Prepositions
        const wxString dolchPrepositionsPercentageStr = wxNumberFormatter::ToString(
            dolchPrepositionsPercentage, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        htmlText += FormatDolchCoverageRow(
            _(L"Prepositions used:"),
            MAX_DOLCH_PREPOSITION_WORDS - project->GetUnusedDolchPrepositions(),
            dolchPrepositionsPercentage,
            wxString::Format(_(L"(%s%% of all Dolch prepositions)"),
                             dolchPrepositionsPercentageStr),
            wxString::Format(_(L"%s%% of all Dolch prepositions"), dolchPrepositionsPercentageStr),
            _(L"Number of prepositions used"), listDataItemCount, listData);
        // Pronouns
        const wxString dolchPronounsPercentageStr = wxNumberFormatter::ToString(
            dolchPronounsPercentage, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        htmlText += FormatDolchCoverageRow(
            _(L"Pronouns used:"), MAX_DOLCH_PRONOUN_WORDS - project->GetUnusedDolchPronouns(),
            dolchPronounsPercentage,
            wxString::Format(_(L"(%s%% of all Dolch pronouns)"), dolchPronounsPercentageStr),
            wxString::Format(_(L"%s%% of all Dolch pronouns"), dolchPronounsPercentageStr),
            _(L"Number of pronouns used"), listDataItemCount, listData);
        // Adverbs
        const wxString dolchAdverbsPercentageStr = wxNumberFormatter::ToString(
            dolchAdverbsPercentage, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        htmlText += FormatDolchCoverageRow(
            _(L"Adverbs used:"), MAX_DOLCH_ADVERB_WORDS - project->GetUnusedDolchAdverbs(),
            dolchAdverbsPercentage,
            wxString::Format(_(L"(%s%% of all Dolch adverbs)"), dolchAdverbsPercentageStr),
            wxString::Format(_(L"%s%% of all Dolch adverbs"), dolchAdverbsPercentageStr),
            _(L"Number of adverbs used"), listDataItemCount, listData);
        // Adjectives
        const wxString dolchAdjectivesPercentageStr = wxNumberFormatter::ToString(
            dolchAdjectivesPercentage, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        htmlText += FormatDolchCoverageRow(
            _(L"Adjectives used:"), MAX_DOLCH_ADJECTIVE_WORDS - project->GetUnusedDolchAdjectives(),
            dolchAdjectivesPercentage,
            wxString::Format(_(L"(%s%% of all Dolch adjectives)"), dolchAdjectivesPercentageStr),
            wxString::Format(_(L"%s%% of all Dolch adjectives"), dolchAdjectivesPercentageStr),
            _(L"Number of adjectives used"), listDataItemCount, listData);
        // Verbs
        const wxString dolchVerbsPercentageStr = wxNumberFormatter::ToString(
            dolchVerbsPercentage, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        htmlText += FormatDolchCoverageRow(
            _(L"Verbs used:"), MAX_DOLCH_VERBS - project->GetUnusedDolchVerbs(),
            dolchVerbsPercentage,
            wxString::Format(_(L"(%s%% of all Dolch verbs)"), dolchVerbsPercentageStr),
            wxString::Format(_(L"%s%% of all Dolch verbs"), dolchVerbsPercentageStr),
            _(L"Number of verbs used"), listDataItemCount, listData);
        // Nouns
        const wxString dolchNounPercentageStr = wxNumberFormatter::ToString(
            dolchNounPercentage, 1, wxNumberFormatter::Style::Style_NoTrailingZeroes);
        htmlText += FormatDolchCoverageRow(
            _(L"Nouns used:"), MAX_DOLCH_NOUNS - project->GetUnusedDolchNouns(),
            dolchNounPercentage,
            wxString::Format(_(L"(%s%% of all Dolch nouns)"), dolchNounPercentageStr),
            wxString::Format(_(L"%s%% of all Dolch nouns"), dolchNounPercentageStr),
            _(L"Number of nouns used"), listDataItemCount, listData);
        htmlText += L"\n</div></div>";

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
              "<a href=\"#unused-dolch-words\">Unused Dolch Words</a> output."));
        }

    // total words
    if (statsInfo.IsDolchWordsEnabled())
        {
        htmlText.append(tableStart).append(FormatDolchHeader(_(L"Dolch Words")));

            // number of Dolch words
            {
            const wxString valueStr =
                (totalDolchPercentage < 70) ?
                    L"<span class=\"issue\">" +
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
                    L"<span class=\"issue\">" +
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
            htmlText += FormatDolchRow(_(L"Number of Dolch words:"), valueStr, percentStr);

            if (listData != nullptr)
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
                    L"<span class=\"issue\">" +
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
                    L"<span class=\"issue\">" +
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
            htmlText += FormatDolchRow(_(L"Number of Dolch words (excluding nouns):"), valueStr,
                                       percentStr);

            if (listData != nullptr)
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
                    L"<span class=\"issue\">" +
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
                    L"<span class=\"issue\">" +
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
            htmlText += FormatDolchRow(_(L"Number of non-Dolch words:"), valueStr, percentStr);

            if (listData != nullptr)
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
        htmlText += FormatDolchWordsRow(
            _(L"Number of Dolch conjunctions:"), project->GetDolchConjunctionCounts().second,
            project->GetTotalWords(), _(L"Number of unique Dolch conjunctions:"),
            project->GetDolchConjunctionCounts().first, _(L"Number of Dolch conjunctions"),
            _(L"Number of unique Dolch conjunctions"), listDataItemCount, listData);
        // Prepositions words
        htmlText += FormatDolchWordsRow(
            _(L"Number of Dolch prepositions:"), project->GetDolchPrepositionWordCounts().second,
            project->GetTotalWords(), _(L"Number of unique Dolch prepositions:"),
            project->GetDolchPrepositionWordCounts().first, _(L"Number of Dolch prepositions"),
            _(L"Number of unique Dolch prepositions"), listDataItemCount, listData);
        // Pronouns words
        htmlText += FormatDolchWordsRow(
            _(L"Number of Dolch pronouns:"), project->GetDolchPronounCounts().second,
            project->GetTotalWords(), _(L"Number of unique Dolch pronouns:"),
            project->GetDolchPronounCounts().first, _(L"Number of Dolch pronouns"),
            _(L"Number of unique Dolch pronouns"), listDataItemCount, listData);
        // Adverbs
        htmlText += FormatDolchWordsRow(
            _(L"Number of Dolch adverbs:"), project->GetDolchAdverbCounts().second,
            project->GetTotalWords(), _(L"Number of unique Dolch adverbs:"),
            project->GetDolchAdverbCounts().first, _(L"Number of Dolch adverbs"),
            _(L"Number of unique Dolch adverbs"), listDataItemCount, listData);
        // adjectives
        htmlText += FormatDolchWordsRow(
            _(L"Number of Dolch adjectives:"), project->GetDolchAdjectiveCounts().second,
            project->GetTotalWords(), _(L"Number of unique Dolch adjectives:"),
            project->GetDolchAdjectiveCounts().first, _(L"Number of Dolch adjectives"),
            _(L"Number of unique Dolch adjectives"), listDataItemCount, listData);
        // verbs
        htmlText +=
            FormatDolchWordsRow(_(L"Number of Dolch verbs:"), project->GetDolchVerbsCounts().second,
                                project->GetTotalWords(), _(L"Number of unique Dolch verbs:"),
                                project->GetDolchVerbsCounts().first, _(L"Number of Dolch verbs"),
                                _(L"Number of unique Dolch verbs"), listDataItemCount, listData);
        // nouns
        htmlText +=
            FormatDolchWordsRow(_(L"Number of Dolch nouns:"), project->GetDolchNounCounts().second,
                                project->GetTotalWords(), _(L"Number of unique Dolch nouns:"),
                                project->GetDolchNounCounts().first, _(L"Number of Dolch nouns"),
                                _(L"Number of unique Dolch nouns"), listDataItemCount, listData);
        htmlText += L"\n</div></div>";

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
        htmlText += L"\n<p>";
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
        htmlText += L"</p></div></div>";
        }

    if (listData != nullptr)
        {
        listData->SetSize(listDataItemCount, 3);
        }

    return htmlText;
    }

//------------------------------------------------
wxString ProjectReportFormat::GetThemeCss(const wxString& fileName /*= _DT(L"default.css")*/,
                                          const wxString& overrideFileName /*= wxEmptyString*/)
    {
    wxString resourceDir =
        wxGetApp().FindResourceDirectory(_DT(L"report-themes")) + wxFileName::GetPathSeparator();
    wxString styleInfo;
    wxString cssPath = resourceDir + fileName;
    if (wxFile::Exists(cssPath))
        {
        Wisteria::TextStream::ReadFile(cssPath, styleInfo);
        }
    // "emerald-isles.css" just restates default.css's color variables,
    // so overlaying it would be a no-op
    if (!overrideFileName.empty() && overrideFileName.CmpNoCase(_DT(L"emerald-isles.css")) != 0)
        {
        wxString overridePath = resourceDir + overrideFileName;
        if (wxFile::Exists(overridePath))
            {
            wxString overrideCss;
            Wisteria::TextStream::ReadFile(overridePath, overrideCss);
            if (!overrideCss.empty())
                {
                styleInfo += L"\n" + overrideCss;
                }
            }
        }
    return styleInfo;
    }

//------------------------------------------------
wxColour ProjectReportFormat::ParseThemeColour(const wxString& css, const wxString& propertyName,
                                               const wxColour& fallbackColour)
    {
    wxColour colour{ fallbackColour };
    const auto tagPos = css.rfind(propertyName);
    if (tagPos != wxString::npos)
        {
        const auto hashPos = css.find(L'#', tagPos);
        if (hashPos != wxString::npos && hashPos + 7 <= css.length())
            {
            const wxColour parsedColour{ css.substr(hashPos, 7) };
            if (parsedColour.IsOk())
                {
                colour = parsedColour;
                }
            }
        }
    return colour;
    }

//------------------------------------------------
wxColour
ProjectReportFormat::GetThemeAccentColour(const wxString& overrideFileName /*= wxEmptyString*/)
    {
    return ParseThemeColour(GetThemeCss(_DT(L"default.css"), overrideFileName),
                            _DT(L"--header-accent:"), wxColour{ L"#00CC66" });
    }

//------------------------------------------------
wxColour
ProjectReportFormat::GetThemeBannerColour(const wxString& overrideFileName /*= wxEmptyString*/)
    {
    return ParseThemeColour(GetThemeCss(_DT(L"default.css"), overrideFileName),
                            _DT(L"--banner-color:"), wxColour{ L"#2E86AB" });
    }

//------------------------------------------------
wxString
ProjectReportFormat::FormatHtmlReportStart(const wxString& title /*= wxString{}*/,
                                           const wxString& overrideCssFile /*= wxEmptyString*/)
    {
    return wxString::Format(
        L"<!DOCTYPE html>"
        "\n<html>"
        "\n<head>"
        "\n    <meta http-equiv='content-type' content='text/html; charset=UTF-8' />"
        "\n    <meta name='color-scheme' content='light dark' />"
        "\n    <meta name='generator' content='%s' />"
        "\n    <title>%s</title>"
        "\n    <style>"
        "\n    %s"
        "\n    </style>"
        "\n</head>"
        "\n<body id='top'>",
        wxGetApp().GetAppName(), title, GetThemeCss(_DT(L"default.css"), overrideCssFile));
    }

//------------------------------------------------
wxString ProjectReportFormat::FormatHtmlReportEnd()
    {
    return { L"\n<a href='#top' class='back-to-top no-print' aria-label='" + _(L"Back to top") +
             _DT(L"'>&#8593;</a>\n</body>\n</html>") };
    }

//------------------------------------------------
void ProjectReportFormat::StripBackToTopButton(wxString& html)
    {
    // find the elevator button at the tail end of the content
    const auto classPos = html.rfind(L"back-to-top");
    if (classPos == wxString::npos)
        {
        return;
        }
    const auto startPos = html.rfind(L"<a", classPos);
    if (startPos == wxString::npos)
        {
        return;
        }
    const auto endPos = html.find(L"</a>", classPos);
    if (endPos != wxString::npos)
        {
        html.erase(startPos, (endPos + 4) - startPos);
        }
    }

//------------------------------------------------
wxString ProjectReportFormat::FormatReportBanner(const wxString& title,
                                                 const wxString& subtitle /*= wxString{}*/)
    {
    wxString html = L"\n<div class='report-banner'>"
                    L"\n<div class='report-banner-accent'></div>"
                    L"\n<div class='report-banner-content'>"
                    L"\n<h1>" +
                    title + L"</h1>";
    if (!subtitle.empty())
        {
        html += L"\n<p>" + subtitle + L"</p>";
        }
    html += L"\n</div>\n</div>\n";
    return html;
    }

//------------------------------------------------
wxString ProjectReportFormat::FormatStatisticsInfo(
    const BaseProject* project, const StatisticsReportInfo& statsInfo,
    const std::shared_ptr<Wisteria::UI::ListCtrlExDataProviderBase>& listData)
    {
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
        htmlText += L"<div class='pill-message'><span class='pill'>" +
                    _(L"No statistics <a href=\"#select-statistics\">currently selected</a>.") +
                    L"</span></div>";
        }

    const wxString tableStart = L"<div class='explanation-card'>";

    const auto formatHeader = [](const wxString& label)
    {
        return wxString::Format(L"\n<div class='explanation-card-header'>%s</div>"
                                "<div class='explanation-card-body'>",
                                label);
    };

    const auto formatRow =
        [](const wxString& label, const wxString& value, const wxString& percent = wxString{})
    {
        return wxString::Format(L"\n<div class='data-row'>"
                                "<span>%s</span>"
                                "<span>%s %s</span></div>",
                                label, value, percent);
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

        PopulateListRow(
            _(L"Number of paragraphs"),
            wxNumberFormatter::ToString(project->GetTotalParagraphs(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep),
            listDataItemCount, listData);

        // average paragraph length
        htmlText += formatRow(
            _(L"Average number of sentences per paragraph:"),
            wxNumberFormatter::ToString(
                safe_divide<double>(project->GetTotalSentences(), project->GetTotalParagraphs()), 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes |
                    wxNumberFormatter::Style::Style_WithThousandsSep));
        htmlText += L"\n</div></div>";

        PopulateListRow(
            _(L"Average number of sentences per paragraph"),
            wxNumberFormatter::ToString(
                safe_divide<double>(project->GetTotalSentences(), project->GetTotalParagraphs()), 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes |
                    wxNumberFormatter::Style::Style_WithThousandsSep),
            listDataItemCount, listData);
        }

    if (statsInfo.IsSentencesEnabled())
        {
        // Number of sentences
        htmlText += tableStart + formatHeader(_(L"Sentences"));

        if (project->GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis)
            {
            currentLabel = _(L"Number of sentences (excluding incomplete sentences, "
                             "see notes below):");
            }
        else if (project->GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings)
            {
            currentLabel = _(L"Number of sentences (excluding lists and tables, see notes below):");
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

        if (listData != nullptr)
            {
            currentLabel = _(L"Number of sentences");
            listData->SetItemText(
                listDataItemCount, 0,
                htmlStrip(currentLabel.wc_str(), currentLabel.length(), true, false),
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
                                 "sentences, see notes below):");
                }
            else if (project->GetInvalidSentenceMethod() ==
                     InvalidSentence::ExcludeExceptForHeadings)
                {
                currentLabel = _(L"Number of units/independent clauses (excluding lists and "
                                 L"tables, see notes below):");
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

            if (listData != nullptr)
                {
                currentLabel = _(L"Number of units/independent clauses");
                listData->SetItemText(
                    listDataItemCount, 0,
                    htmlStrip(currentLabel.wc_str(), currentLabel.length(), true, false),
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
                wxString::Format(_(L"Number of <a href=\"#difficult-sentences\">difficult "
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
                L"<span class=\"issue\">" +
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
                L"<span class=\"issue\">" +
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

        if (listData != nullptr)
            {
            wxString sentLabelForTable =
                htmlStrip(currentLabel.wc_str(), currentLabel.length(), true, false);
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
                               L"<span class=\"issue\">" +
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

            PopulateListRow(
                _(L"Longest sentence"),
                wxNumberFormatter::ToString(project->GetLongestSentence(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                listDataItemCount, listData);
            }
        else
            {
            htmlText += formatRow(_(L"Longest sentence:"), _(L"N/A"));
            }
        // average sentence length
        currentValue =
            (safe_divide<double>(project->GetTotalWords(), project->GetTotalSentences()) > 20) ?
                L"<span class=\"issue\">" +
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

        PopulateListRow(
            _(L"Average sentence length"),
            wxNumberFormatter::ToString(
                safe_divide<double>(project->GetTotalWords(), project->GetTotalSentences()), 1,
                wxNumberFormatter::Style::Style_NoTrailingZeroes |
                    wxNumberFormatter::Style::Style_WithThousandsSep),
            listDataItemCount, listData);
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

        PopulateListRow(
            _(L"Number of interrogative sentences (questions)"),
            wxNumberFormatter::ToString(project->GetTotalInterrogativeSentences(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep),
            wxString::Format(
                _(L"%s%% of all sentences"),
                wxNumberFormatter::ToString(interrogativeSentencePercentage, 1,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep)),
            listDataItemCount, listData);

        // exclamatory sentences
        currentValue =
            (exclamatorySentencePercentage >= 25) ?
                L"<span class=\"issue\">" +
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
                L"<span class=\"issue\">" +
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
        htmlText += L"\n</div></div>";

        PopulateListRow(
            _(L"Number of exclamatory sentences"),
            wxNumberFormatter::ToString(project->GetTotalExclamatorySentences(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep),
            wxString::Format(
                _(L"%s%% of all sentences"),
                wxNumberFormatter::ToString(exclamatorySentencePercentage, 1,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep)),
            listDataItemCount, listData);
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
                L"<span class=\"issue\">" +
                    wxNumberFormatter::ToString(
                        project->GetTotalWords(), 0,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep) +
                    L"</span>" :
                wxNumberFormatter::ToString(project->GetTotalWords(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep);
        htmlText += formatRow(_(L"Number of words:"), currentValue);

        PopulateListRow(
            _(L"Number of words"),
            wxNumberFormatter::ToString(project->GetTotalWords(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep),
            listDataItemCount, listData);
        // Number of unique words
        htmlText += formatRow(
            _(L"Number of unique words:"),
            wxNumberFormatter::ToString(project->GetTotalUniqueWords(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));

        PopulateListRow(
            _(L"Number of unique words"),
            wxNumberFormatter::ToString(project->GetTotalUniqueWords(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep),
            listDataItemCount, listData);
        // Number of total syllables
        htmlText += formatRow(
            _(L"Number of syllables:"),
            wxNumberFormatter::ToString(project->GetTotalSyllables(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));

        PopulateListRow(
            _(L"Number of syllables"),
            wxNumberFormatter::ToString(project->GetTotalSyllables(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep),
            listDataItemCount, listData);
        // Number of total characters
        htmlText += formatRow(
            _(L"Number of characters (punctuation excluded):"),
            wxNumberFormatter::ToString(project->GetTotalCharacters(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));

        PopulateListRow(
            _(L"Number of characters (punctuation excluded)"),
            wxNumberFormatter::ToString(project->GetTotalCharacters(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep),
            listDataItemCount, listData);
        htmlText += formatRow(
            _(L"Number of characters + punctuation:"),
            wxNumberFormatter::ToString(project->GetTotalCharactersPlusPunctuation(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep));

        htmlText +=
            FormatHtmlNoteSection(_(L"Sentence-ending punctuation is excluded from this statistic. "
                                    "Tests that include punctuation counts instruct to not include "
                                    "any punctuation that ends a sentence."));

        PopulateListRow(
            _(L"Number of characters + punctuation"),
            wxNumberFormatter::ToString(project->GetTotalCharactersPlusPunctuation(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep),
            listDataItemCount, listData);
        // average number of characters
        currentValue =
            (averageCharacterCount >= 6) ?
                L"<span class=\"issue\">" +
                    wxNumberFormatter::ToString(
                        averageCharacterCount, 1,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep) +
                    L"</span>" :
                wxNumberFormatter::ToString(averageCharacterCount, 1,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep);
        htmlText += formatRow(_(L"Average number of characters:"), currentValue);

        PopulateListRow(
            _(L"Average number of characters"),
            wxNumberFormatter::ToString(averageCharacterCount, 1,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep),
            listDataItemCount, listData);
        // average number of syllables
        currentValue =
            (averageSyllableCount >= 4) ?
                L"<span class=\"issue\">" +
                    wxNumberFormatter::ToString(
                        averageSyllableCount, 1,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep) +
                    L"</span>" :
                wxNumberFormatter::ToString(averageSyllableCount, 1,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep);
        htmlText += formatRow(_(L"Average number of syllables:"), currentValue);
        htmlText += L"\n</div></div>";

        PopulateListRow(
            _(L"Average number of syllables"),
            wxNumberFormatter::ToString(averageSyllableCount, 1,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep),
            listDataItemCount, listData);
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
        htmlText += L"\n</div></div>";

        PopulateListRow(
            _(L"Number of numerals"),
            wxNumberFormatter::ToString(project->GetTotalNumerals(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep),
            wxString::Format(
                _(L"%s%% of all words"),
                wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetTotalNumerals(), project->GetTotalWords()) *
                        100,
                    1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes |
                        wxNumberFormatter::Style::Style_WithThousandsSep)),
            listDataItemCount, listData);

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
            htmlText += L"\n</div></div>";

            PopulateListRow(
                _(L"Number of proper nouns"),
                wxNumberFormatter::ToString(project->GetTotalProperNouns(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                wxString::Format(_(L"%s%% of all words"),
                                 wxNumberFormatter::ToString(
                                     safe_divide<double>(project->GetTotalProperNouns(),
                                                         project->GetTotalWords()) *
                                         100,
                                     1,
                                     wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                         wxNumberFormatter::Style::Style_WithThousandsSep)),
                listDataItemCount, listData);
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

        PopulateListRow(
            _(L"Number of monosyllabic words"),
            wxNumberFormatter::ToString(project->GetTotalMonoSyllabicWords(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep),
            wxString::Format(_(L"%s%% of all words"),
                             wxNumberFormatter::ToString(
                                 safe_divide<double>(project->GetTotalMonoSyllabicWords(),
                                                     project->GetTotalWords()) *
                                     100,
                                 1,
                                 wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                     wxNumberFormatter::Style::Style_WithThousandsSep)),
            listDataItemCount, listData);

        // unique monosyllabic words
        htmlText += formatRow(_(L"Number of unique monosyllabic words:"),
                              wxNumberFormatter::ToString(
                                  project->GetTotalUniqueMonoSyllabicWords(), 0,
                                  wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                      wxNumberFormatter::Style::Style_WithThousandsSep)) +
                    L"\n</div></div>";

        PopulateListRow(
            _(L"Number of unique monosyllabic words"),
            wxNumberFormatter::ToString(project->GetTotalUniqueMonoSyllabicWords(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep),
            listDataItemCount, listData);

        // 3+ syllable
        htmlText += tableStart + formatHeader(_(L"Complex Words"));
        if (project->GetTotal3PlusSyllabicWords() > 0)
            {
            currentLabel = _(L"Number of <a href=\"#hard-words\">complex (3+ syllable)</a> words:");
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

        PopulateListRow(
            _(L"Number of complex (3+ syllable) words"),
            wxNumberFormatter::ToString(project->GetTotal3PlusSyllabicWords(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep),
            wxString::Format(_(L"%s%% of all words"),
                             wxNumberFormatter::ToString(
                                 safe_divide<double>(project->GetTotal3PlusSyllabicWords(),
                                                     project->GetTotalWords()) *
                                     100,
                                 1,
                                 wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                     wxNumberFormatter::Style::Style_WithThousandsSep)),
            listDataItemCount, listData);
        // unique 3+ syllable words
        htmlText += formatRow(_(L"Number of unique 3+ syllable words:"),
                              wxNumberFormatter::ToString(
                                  project->GetTotalUnique3PlusSyllableWords(), 0,
                                  wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                      wxNumberFormatter::Style::Style_WithThousandsSep)) +
                    L"\n</div></div>";

        PopulateListRow(
            _(L"Number of unique 3+ syllable words"),
            wxNumberFormatter::ToString(project->GetTotalUnique3PlusSyllableWords(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep),
            listDataItemCount, listData);

        // long words (6 characters or more)
        htmlText += tableStart + formatHeader(_(L"Long Words"));
        if (project->GetTotalLongWords() > 0)
            {
            currentLabel = _(L"Number of <a href=\"#long-words\">long (6+ characters)</a> words:");
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

        PopulateListRow(
            _(L"Number of long (6+ characters) words"),
            wxNumberFormatter::ToString(project->GetTotalLongWords(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep),
            wxString::Format(
                _(L"%s%% of all words"),
                wxNumberFormatter::ToString(
                    safe_divide<double>(project->GetTotalLongWords(), project->GetTotalWords()) *
                        100,
                    1,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes |
                        wxNumberFormatter::Style::Style_WithThousandsSep)),
            listDataItemCount, listData);
        // unique long words
        htmlText += formatRow(_(L"Number of unique long words:"),
                              wxNumberFormatter::ToString(
                                  project->GetTotalUnique6CharsPlusWords(), 0,
                                  wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                      wxNumberFormatter::Style::Style_WithThousandsSep)) +
                    L"\n</div></div>";
        PopulateListRow(
            _(L"Number of unique long words"),
            wxNumberFormatter::ToString(project->GetTotalUnique6CharsPlusWords(), 0,
                                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                            wxNumberFormatter::Style::Style_WithThousandsSep),
            listDataItemCount, listData);

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

            PopulateListRow(
                _(L"Number of SMOG hard words (3+ syllables, numerals fully syllabized)"),
                wxNumberFormatter::ToString(
                    project->GetTotal3PlusSyllabicWordsNumeralsFullySyllabized(), 0,
                    wxNumberFormatter::Style::Style_NoTrailingZeroes |
                        wxNumberFormatter::Style::Style_WithThousandsSep),
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
                listDataItemCount, listData);
            // unique SMOG words
            htmlText +=
                formatRow(_(L"Number of unique SMOG hard words:"),
                          wxNumberFormatter::ToString(
                              project->GetUnique3PlusSyllabicWordsNumeralsFullySyllabized(), 0,
                              wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                  wxNumberFormatter::Style::Style_WithThousandsSep)) +
                L"\n</div></div>";
            PopulateListRow(_(L"Number of unique SMOG hard words"),
                            wxNumberFormatter::ToString(
                                project->GetUnique3PlusSyllabicWordsNumeralsFullySyllabized(), 0,
                                wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                    wxNumberFormatter::Style::Style_WithThousandsSep),
                            listDataItemCount, listData);
            }
        if (project->GetReadabilityTests().is_test_included(ReadabilityMessages::GUNNING_FOG()))
            {
            // Fog
            htmlText += tableStart +
                        // TRANSLATORS: "Fog" is the name of a test, don't translate that part.
                        formatHeader(_(L"Fog Words")) +
                        // TRANSLATORS: "Fog" is the name of a test, don't translate that part.
                        formatRow(_(L"Number of Fog hard words (3+ syllables, with <a "
                                    "href=\"#fog-help\">exceptions</a>):"),
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

            PopulateListRow(
                _(L"Number of Fog hard words (3+ syllables, with exceptions)"),
                wxNumberFormatter::ToString(project->GetTotalHardWordsFog(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                wxString::Format(_(L"%s%% of all words"),
                                 wxNumberFormatter::ToString(
                                     safe_divide<double>(project->GetTotalHardWordsFog(),
                                                         project->GetTotalWords()) *
                                         100,
                                     1,
                                     wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                         wxNumberFormatter::Style::Style_WithThousandsSep)),
                listDataItemCount, listData);
            // unique fog words
            htmlText += formatRow(_(L"Number of unique Fog hard words:"),
                                  wxNumberFormatter::ToString(
                                      project->GetTotalUniqueHardFogWords(), 0,
                                      wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                          wxNumberFormatter::Style::Style_WithThousandsSep)) +
                        L"\n</div></div>";
            PopulateListRow(
                _(L"Number of unique Fog hard words"),
                wxNumberFormatter::ToString(project->GetTotalUniqueHardFogWords(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                listDataItemCount, listData);
            }

        if (project->IsIncludingDolchSightWords())
            {
            // Resize the list grid (if using one) to fit its data, then the call to Dolch
            // stats will append to that and resize it again.
            if (listData != nullptr)
                {
                listData->SetSize(listDataItemCount, 3);
                }
            htmlText += FormatDolchStatisticsInfo(project, statsInfo, false, listData);
            // ...then we will add some more rows for the rest of the stats.
            if (listData != nullptr)
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
                    _(L"Number of <a href=\"#dale-chall-words\">Dale-Chall</a> unfamiliar words:");
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

            PopulateListRow(
                _(L"Number of Dale-Chall unfamiliar words"),
                wxNumberFormatter::ToString(project->GetTotalHardWordsDaleChall(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                wxString::Format(_(L"%s%% of all words"),
                                 wxNumberFormatter::ToString(
                                     safe_divide<double>(project->GetTotalHardWordsDaleChall(),
                                                         totalWordCountForDC) *
                                         100,
                                     1,
                                     wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                         wxNumberFormatter::Style::Style_WithThousandsSep)),
                listDataItemCount, listData);
            // unique Dale Chall hard words
            htmlText += formatRow(_(L"Number of unique Dale-Chall unfamiliar words:"),
                                  wxNumberFormatter::ToString(
                                      project->GetTotalUniqueDCHardWords(), 0,
                                      wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                          wxNumberFormatter::Style::Style_WithThousandsSep)) +
                        L"\n</div></div>";
            PopulateListRow(
                _(L"Number of unique Dale-Chall unfamiliar words"),
                wxNumberFormatter::ToString(project->GetTotalUniqueDCHardWords(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                listDataItemCount, listData);

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
                currentLabel =
                    _(L"Number of <a href=\"#harris-jacobson-words\">Harris-Jacobson</a> "
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

            PopulateListRow(
                _(L"Number of Harris-Jacobson unfamiliar words"),
                wxNumberFormatter::ToString(project->GetTotalHardWordsHarrisJacobson(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                wxString::Format(
                    _(L"%s%% of all words"),
                    wxNumberFormatter::ToString(
                        safe_divide<double>(project->GetTotalHardWordsHarrisJacobson(),
                                            totalWordCountForHJ - totalNumeralCountForHJ) *
                            100,
                        1,
                        wxNumberFormatter::Style::Style_NoTrailingZeroes |
                            wxNumberFormatter::Style::Style_WithThousandsSep)),
                listDataItemCount, listData);

            // unique Harris-Jacobson hard words
            htmlText += formatRow(_(L"Number of unique Harris-Jacobson unfamiliar words:"),
                                  wxNumberFormatter::ToString(
                                      project->GetTotalUniqueHarrisJacobsonHardWords(), 0,
                                      wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                          wxNumberFormatter::Style::Style_WithThousandsSep)) +
                        L"\n</div></div>";
            PopulateListRow(
                _(L"Number of unique Harris-Jacobson unfamiliar words"),
                wxNumberFormatter::ToString(project->GetTotalUniqueHarrisJacobsonHardWords(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                listDataItemCount, listData);

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
                    _(L"Number of <a href=\"#spache-words\">Spache</a> unfamiliar words:");
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

            PopulateListRow(
                _(L"Number of Spache unfamiliar words"),
                wxNumberFormatter::ToString(project->GetTotalHardWordsSpache(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                wxString::Format(_(L"%s%% of all words"),
                                 wxNumberFormatter::ToString(
                                     safe_divide<double>(project->GetTotalHardWordsSpache(),
                                                         project->GetTotalWords()) *
                                         100,
                                     1,
                                     wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                         wxNumberFormatter::Style::Style_WithThousandsSep)),
                listDataItemCount, listData);

            // unique Spache hard words
            htmlText += formatRow(_(L"Number of unique Spache unfamiliar words:"),
                                  wxNumberFormatter::ToString(
                                      project->GetTotalUniqueHardWordsSpache(), 0,
                                      wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                          wxNumberFormatter::Style::Style_WithThousandsSep)) +
                        L"\n</div></div>";
            PopulateListRow(
                _(L"Number of unique Spache unfamiliar words"),
                wxNumberFormatter::ToString(project->GetTotalUniqueHardWordsSpache(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                listDataItemCount, listData);
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

            PopulateListRow(
                _(L"Number of McAlpine EFLAW miniwords (1-3 characters, except for numerals)"),
                wxNumberFormatter::ToString(project->GetTotalMiniWords(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                wxString::Format(_(L"%s%% of all words"),
                                 wxNumberFormatter::ToString(
                                     safe_divide<double>(project->GetTotalMiniWords(),
                                                         project->GetTotalWords()) *
                                         100,
                                     1,
                                     wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                         wxNumberFormatter::Style::Style_WithThousandsSep)),
                listDataItemCount, listData);

            // unique EFLAW miniwords words
            htmlText += formatRow(_(L"Number of unique McAlpine EFLAW miniwords words:"),
                                  wxNumberFormatter::ToString(
                                      project->GetTotalUniqueMiniWords(), 0,
                                      wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                          wxNumberFormatter::Style::Style_WithThousandsSep)) +
                        L"\n</div></div>";
            PopulateListRow(
                _(L"Number of unique McAlpine EFLAW miniwords words"),
                wxNumberFormatter::ToString(project->GetTotalUniqueMiniWords(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                listDataItemCount, listData);
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

            PopulateListRow(
                wxString::Format(_(L"Number of %s unfamiliar words"),
                                 pos->GetIterator()->get_name().c_str()),
                wxNumberFormatter::ToString(pos->GetUnfamiliarWordCount(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                wxString::Format(_(L"%s%% of all words"),
                                 wxNumberFormatter::ToString(
                                     safe_divide<double>(pos->GetUnfamiliarWordCount(),
                                                         totalWordCountForCustomTest) *
                                         100,
                                     1,
                                     wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                         wxNumberFormatter::Style::Style_WithThousandsSep)),
                listDataItemCount, listData);

            // unique unfamiliar words
            htmlText +=
                formatRow(wxString::Format(_(L"Number of unique %s unfamiliar words:"), testName),
                          wxNumberFormatter::ToString(
                              pos->GetUniqueUnfamiliarWordCount(), 0,
                              wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                  wxNumberFormatter::Style::Style_WithThousandsSep)) +
                L"\n</div></div>";
            PopulateListRow(
                wxString::Format(_(L"Number of unique %s unfamiliar words"),
                                 pos->GetIterator()->get_name().c_str()),
                wxNumberFormatter::ToString(pos->GetUniqueUnfamiliarWordCount(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                listDataItemCount, listData);
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
                currentLabel += L"<a href=\"#misspellings\">";
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

            PopulateListRow(
                _(L"Number of possible misspellings"),
                wxNumberFormatter::ToString(project->GetMisspelledWordCount(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                listDataItemCount, listData);
            }
        // repeated words
        if (project->GetGrammarInfo().IsRepeatedWordsEnabled())
            {
            currentLabel.clear();
            if (project->GetDuplicateWordCount() != 0)
                {
                currentLabel += L"<a href=\"#repeated-words\">";
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

            PopulateListRow(
                _(L"Number of repeated words"),
                wxNumberFormatter::ToString(project->GetDuplicateWordCount(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                listDataItemCount, listData);
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
                    currentLabel += L"<a href=\"#mismatched-articles\">";
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

                PopulateListRow(_(L"Number of article mismatches"),
                                wxNumberFormatter::ToString(
                                    project->GetMismatchedArticleCount(), 0,
                                    wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                        wxNumberFormatter::Style::Style_WithThousandsSep),
                                listDataItemCount, listData);
                }
            // Wording Errors & Known Misspellings
            if (project->GetGrammarInfo().IsWordingErrorsEnabled())
                {
                currentLabel.clear();
                if (project->GetWordingErrorCount() != 0)
                    {
                    currentLabel += L"<a href=\"#wording-errors\">";
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

                PopulateListRow(_(L"Number of wording errors & misspellings"),
                                wxNumberFormatter::ToString(
                                    project->GetWordingErrorCount(), 0,
                                    wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                        wxNumberFormatter::Style::Style_WithThousandsSep),
                                listDataItemCount, listData);
                }
            // redundant phrase count
            if (project->GetGrammarInfo().IsRedundantPhrasesEnabled())
                {
                currentLabel.clear();
                if (project->GetRedundantPhraseCount() != 0)
                    {
                    currentLabel += L"<a href=\"#redundant-phrases\">";
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

                PopulateListRow(_(L"Number of redundant phrases"),
                                wxNumberFormatter::ToString(
                                    project->GetRedundantPhraseCount(), 0,
                                    wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                        wxNumberFormatter::Style::Style_WithThousandsSep),
                                listDataItemCount, listData);
                }
            // overused words (by sentence)
            if (project->GetGrammarInfo().IsOverUsedWordsBySentenceEnabled())
                {
                currentLabel.clear();
                if (project->GetOverusedWordsBySentenceCount() != 0)
                    {
                    currentLabel += L"<a href=\"#overused-words-by-sentence\">";
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

                PopulateListRow(_(L"Number of overused words (x sentence)"),
                                wxNumberFormatter::ToString(
                                    project->GetOverusedWordsBySentenceCount(), 0,
                                    wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                        wxNumberFormatter::Style::Style_WithThousandsSep),
                                listDataItemCount, listData);
                }
            // wordiness
            if (project->GetGrammarInfo().IsWordyPhrasesEnabled())
                {
                currentLabel.clear();
                if (project->GetWordyPhraseCount() != 0)
                    {
                    currentLabel += L"<a href=\"#wordy-phrases\">";
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

                PopulateListRow(_(L"Number of wordy items"),
                                wxNumberFormatter::ToString(
                                    project->GetWordyPhraseCount(), 0,
                                    wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                        wxNumberFormatter::Style::Style_WithThousandsSep),
                                listDataItemCount, listData);
                }
            // cliches
            if (project->GetGrammarInfo().IsClichesEnabled())
                {
                currentLabel.clear();
                if (project->GetClicheCount() > 0)
                    {
                    currentLabel += L"<a href=\"#cliches\">";
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

                PopulateListRow(_(L"Number of clichés"),
                                wxNumberFormatter::ToString(
                                    project->GetClicheCount(), 0,
                                    wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                        wxNumberFormatter::Style::Style_WithThousandsSep),
                                listDataItemCount, listData);
                }
            // passive voice
            if (project->GetGrammarInfo().IsPassiveVoiceEnabled())
                {
                currentLabel.clear();
                if (project->GetPassiveVoicesCount() != 0)
                    {
                    currentLabel += L"<a href=\"#passive-voice\">";
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

                PopulateListRow(_(L"Number of passive voices"),
                                wxNumberFormatter::ToString(
                                    project->GetPassiveVoicesCount(), 0,
                                    wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                        wxNumberFormatter::Style::Style_WithThousandsSep),
                                listDataItemCount, listData);
                }
            }
        // conjunction starting sentences
        if (project->GetGrammarInfo().IsConjunctionStartingSentencesEnabled())
            {
            currentLabel.clear();
            if (project->GetSentenceStartingWithConjunctionsCount() > 0)
                {
                currentLabel += L"<a href=\"#sentence-starting-with-conjunctions\">";
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

            PopulateListRow(
                _(L"Number of sentences that begin with conjunctions"),
                wxNumberFormatter::ToString(project->GetSentenceStartingWithConjunctionsCount(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                listDataItemCount, listData);
            }
        // lowercased sentences
        if (project->GetGrammarInfo().IsLowercaseSentencesEnabled())
            {
            currentLabel.clear();
            if (project->GetSentenceStartingWithLowercaseCount() > 0)
                {
                currentLabel += L"<a href=\"#sentence-starting-with-lowercase\">";
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

            PopulateListRow(
                _(L"Number of Sentences that begin with lowercased words"),
                wxNumberFormatter::ToString(project->GetSentenceStartingWithLowercaseCount(), 0,
                                            wxNumberFormatter::Style::Style_NoTrailingZeroes |
                                                wxNumberFormatter::Style::Style_WithThousandsSep),
                listDataItemCount, listData);
            }

        htmlText += FormatHtmlNoteSection(
            _(L"Grammar statistics do not directly factor into readability formulas; however, "
              "they can be useful suggestions for improving the document."));

        if (project->GetSentenceStartMustBeUppercased())
            {
            htmlText += FormatHtmlNoteSection(_(L"* This project's sentence-deduction method is "
                                                "set to only accept capitalized sentences. "
                                                "Lowercased-sentence detection will be limited to "
                                                "sentences that begin new paragraphs."));
            }

        htmlText += L"\n</div></div>";
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

                fileModDate = wxString::Format( // TRANSLATORS: placeholders are date and time
                    _(L"%s at %s"),
                    modDate.Format(wxUILocale::GetCurrent().GetInfo(wxLOCALE_LONG_DATE_FMT)),
                    timeStr);
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
        htmlText += L"\n</div></div>";

        if (listData != nullptr)
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
        htmlText.append(tableStart).append(formatHeader(_(L"Notes")));
        if (project->GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis)
            {
            htmlText.append(
                L"<p>" +
                _(L"Incomplete sentences have been excluded from the analysis. "
                  "All words from incomplete sentences were ignored and not factored into "
                  "these statistics (except for grammar information).") +
                L"</p>");
            }
        else if (project->GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings)
            {
            htmlText.append(
                L"<p>" +
                _(L"Lists and tables have been excluded from the analysis. "
                  "All words from these sentences were ignored and not factored into these "
                  "statistics (except for grammar information).") +
                L"</p>");
            }
        if (!project->GetAppendedDocumentFilePath().empty())
            {
            htmlText.append(
                L"<p>" +
                wxString::Format(_(L"An additional document (\"%s\") has been appended and "
                                   "included in the analysis."),
                                 wxFileName(project->GetAppendedDocumentFilePath()).GetFullName()) +
                L"</p>");
            }
        if (statsInfo.IsExtendedInformationEnabled())
            {
            htmlText.append(
                L"<p>" +
                _(L"Averages are calculated using arithmetic mean "
                  "(the summation of all values in a range divided by the number of items).") +
                L"</p>");
            }
        if (project->GetTotalWords() < 300)
            {
            htmlText.append(
                L"<p>" +
                wxString::Format(
                    _(L"This document only contains %s words. "
                      "Most readability tests require a minimum of 300 words to be able to "
                      "generate "
                      "meaningful results."),
                    wxNumberFormatter::ToString(project->GetTotalWords(), 0,
                                                wxNumberFormatter::Style::Style_NoTrailingZeroes)) +
                L"</p>");
            }
        if (overlyLongSentencePercentage >= 40)
            {
            htmlText.append(L"<p>" + _(L"A large percentage of sentences are overly long.") +
                            L"</p>");
            }
        if (exclamatorySentencePercentage >= 25)
            {
            htmlText.append(
                L"<p>" +
                _(L"A large percentage of sentences are exclamatory, "
                  "giving the document an overall angry/excited tone. Is this intended?") +
                L"</p>");
            }
        if (averageCharacterCount >= 6)
            {
            htmlText.append(L"<p>" + _(L"The average word length is high.") + L"</p>");
            }
        if (averageSyllableCount >= 4)
            {
            htmlText.append(L"<p>" + _(L"The average word complexity (syllable count) is high.") +
                            L"</p>");
            }
        htmlText += L"\n</div></div>";
        }

    // trim off any trailing breaks
    htmlText = TrimTrailingBreaks(htmlText);

    if (listData != nullptr)
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
    return wxString::Format(L"<div class='test-result'>"
                            "<div class='test-result-info'><p>%s</p><p>%s</p></div>"
                            "<div class='test-result-factors'>%s</div></div>\n",
                            score, theTest.get_description().c_str(),
                            ProjectReportFormat::FormatTestFactors(theTest)) +
           (!note.empty() ? wxString::Format(L"<div>%s</div>\n",
                                             ProjectReportFormat::FormatHtmlNoteSection(note)) :
                            wxString{});
    }

//------------------------------------------------
wxString ProjectReportFormat::FormatTestFactors(const readability::readability_test& test)
    {
    wxString htmlText = wxString::Format(L"<div class='test-factors'>"
                                         "<div class='test-factors-header'>%s</div>",
                                         _(L"Difficulty Prediction Factors"));

    const auto formatRow = [](const wxString& label, const bool value) -> wxString
    {
        return wxString::Format(L"<div class='data-row%s'>"
                                "<span>%s</span>"
                                "<span class='checkmark'>%s</span></div>",
                                (!value ? L" data-row-muted" : L""), label, (value ? L"✓" : L""));
    };

    // word complexity
    if (test.has_factor(readability::test_factor::word_complexity_2_plus_syllables))
        {
        htmlText += formatRow(_(L"Word complexity (2 or more syllables)"), true);
        }
    else if (test.has_factor(readability::test_factor::word_complexity_3_plus_syllables))
        {
        htmlText += formatRow(_(L"Word complexity (3 or more syllables)"), true);
        }
    else if (test.has_factor(readability::test_factor::word_complexity_density))
        {
        htmlText += formatRow(_(L"Density of complex words"), true);
        }
    else
        {
        htmlText += formatRow(_(L"Word complexity (syllable counts)"),
                              test.has_factor(readability::test_factor::word_complexity));
        }

    // word length
    if (test.has_factor(readability::test_factor::word_length_3_less))
        {
        htmlText += formatRow(_(L"Word length (3 or less characters)"), true);
        }
    else if (test.has_factor(readability::test_factor::word_length_6_plus))
        {
        htmlText += formatRow(_(L"Word length (6 or more characters)"), true);
        }
    else if (test.has_factor(readability::test_factor::word_length_7_plus))
        {
        htmlText += formatRow(_(L"Word length (7 or more characters)"), true);
        }
    else
        {
        htmlText +=
            formatRow(_(L"Word length"), test.has_factor(readability::test_factor::word_length));
        }

    // word familiarity
    if (test.has_factor(readability::test_factor::word_familiarity_spache))
        {
        htmlText +=
            formatRow(wxString::Format(_(L"Word familiarity (%s rules)"), _DT(L"Spache")), true);
        }
    else if (test.has_factor(readability::test_factor::word_familiarity_dale_chall))
        {
        htmlText += formatRow(
            wxString::Format(_(L"Word familiarity (%s rules)"), _DT(L"Dale-Chall")), true);
        }
    else if (test.has_factor(readability::test_factor::word_familiarity_harris_jacobson))
        {
        htmlText += formatRow(
            wxString::Format(_(L"Word familiarity (%s rules)"), _DT(L"Harris-Jacobson")), true);
        }
    else
        {
        htmlText += formatRow(_(L"Word familiarity"), false);
        }

    // sentence length
    htmlText += formatRow(_(L"Sentence length"),
                          test.has_factor(readability::test_factor::sentence_length));

    htmlText += L"\n</div>\n";
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
    return wxString::Format(L"\n<div class='callout callout-warning minipage'>"
                            "<div class='callout-icon'>%s %s</div>"
                            "<div class='callout-content'>%s</div></div>\n",
                            BaseProjectView::GetWarningEmoji(), _(L"Warning"), note);
    }

//------------------------------------------------
wxString ProjectReportFormat::FormatHtmlNoteSection(const wxString& note)
    {
    return wxString::Format(L"\n<div class='callout callout-note minipage'>"
                            "<div class='callout-icon'>%s %s</div>"
                            "<div class='callout-content'>%s</div></div>\n",
                            BaseProjectView::GetNoteEmoji(), _(L"Note"), note);
    }

//---------------------------------------------
wxString ProjectReportFormat::FormatSentence(
    const BaseProject* project, const grammar::sentence_info& sentence,
    std::vector<punctuation::punctuation_mark>::const_iterator& punctStart,
    const std::vector<punctuation::punctuation_mark>::const_iterator& punctEnd)
    {
    wxASSERT(project);
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
