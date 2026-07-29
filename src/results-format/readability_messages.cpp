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

#include "readability_messages.h"
#include "wx/numformatter.h"
#include "wx/tokenzr.h"
#include <algorithm>

//-----------------------------------------------
wxString ReadabilityMessages::GetGradeScaleLongLabel(size_t grade) const
    {
    if (GetGradeScale() == readability::grade_scale::k12_plus_united_states)
        {
        return GetUSGradeScaleLabel(grade);
        }
    if (GetGradeScale() == readability::grade_scale::k12_plus_newfoundland_and_labrador)
        {
        return GetNewfoundlandAndLabradorScaleLabel(grade);
        }
    if (GetGradeScale() == readability::grade_scale::k12_plus_british_columbia)
        {
        return GetBritishColumbiaScaleLabel(grade);
        }
    if (GetGradeScale() == readability::grade_scale::k12_plus_newbrunswick)
        {
        return GetNewBrunswickScaleLabel(grade);
        }
    if (GetGradeScale() == readability::grade_scale::k12_plus_nova_scotia)
        {
        return GetNovaScotiaScaleLabel(grade);
        }
    if (GetGradeScale() == readability::grade_scale::k12_plus_ontario)
        {
        return GetOntarioScaleLabel(grade);
        }
    if (GetGradeScale() == readability::grade_scale::k12_plus_saskatchewan)
        {
        return GetSaskatchewanScaleLabel(grade);
        }
    if (GetGradeScale() == readability::grade_scale::k12_plus_prince_edward_island)
        {
        return GetPrinceEdwardIslandScaleLabel(grade);
        }
    if (GetGradeScale() == readability::grade_scale::k12_plus_manitoba)
        {
        return GetManitobaScaleLabel(grade);
        }
    if (GetGradeScale() == readability::grade_scale::k12_plus_northwest_territories)
        {
        return GetNorthwestTerritoriesScaleLabel(grade);
        }
    if (GetGradeScale() == readability::grade_scale::k12_plus_alberta)
        {
        return GetAlbertaScaleLabel(grade);
        }
    if (GetGradeScale() == readability::grade_scale::k12_plus_nunavut)
        {
        return GetNunavutScaleLabel(grade);
        }
    if (GetGradeScale() == readability::grade_scale::quebec)
        {
        return GetQuebecScaleLabel(grade);
        }
    if (GetGradeScale() == readability::grade_scale::key_stages_england_wales)
        {
        return GetEnglandWalesScaleLabel(grade);
        }
    return GetUSGradeScaleLabel(grade);
    }

//-----------------------------------------------
wxString ReadabilityMessages::GetFormattedValue(const double value,
                                                const Wisteria::NumberFormatInfo& format) const
    {
    if (IsUsingLongGradeScaleFormat())
        {
        size_t grade{ 0 }, month{ 0 };
        readability::split_k12_plus_grade_score(value, grade, month);
        return GetGradeScaleLongLabel(grade) + L", " + GetMonthLabel(month);
        }
    return wxNumberFormatter::ToString(value, format.m_precision,
                                       wxNumberFormatter::Style::Style_NoTrailingZeroes);
    }

//-----------------------------------------------
wxString ReadabilityMessages::GetFormattedValue(const wxString& value,
                                                const Wisteria::NumberFormatInfo& format) const
    {
    // just return value if not using long format
    if (!IsUsingLongGradeScaleFormat())
        {
        return value;
        }

    // split up a list of multiple scores
    wxStringTokenizer tkzLis(value, L";", wxTOKEN_STRTOK);
    if (tkzLis.CountTokens() > 1)
        {
        wxString fullString;
        while (tkzLis.HasMoreTokens())
            {
            fullString += GetFormattedValue(tkzLis.GetNextToken(), format) + L"; ";
            }
        // chop off the last "; "
        if (fullString.length() > 2)
            {
            fullString.RemoveLast(2);
            }
        return fullString;
        }

    double numericValue{ 0 };
    wxStringTokenizer tkz(value, L"-", wxTOKEN_STRTOK);
    // grade ranges (e.g., 5-6) need to be split and analyzed atomically
    if (tkz.CountTokens() > 1)
        {
        wxString fullValue;
        while (tkz.HasMoreTokens())
            {
            if (GetScoreValue(tkz.GetNextToken(), numericValue))
                {
                size_t grade{ 0 }, month{ 0 };
                // month is not used, grade ranges shouldn't have this
                readability::split_k12_plus_grade_score(numericValue, grade, month);
                fullValue += GetGradeScaleLongLabel(grade) + L"-";
                }
            }
        if (!fullValue.empty())
            {
            fullValue.RemoveLast();
            }
        return fullValue;
        }
    if (GetScoreValue(value, numericValue))
        {
        size_t grade{ 0 }, month{ 0 };
        readability::split_k12_plus_grade_score(numericValue, grade, month);
        return GetGradeScaleLongLabel(grade) + L", " + GetMonthLabel(month);
        }
    return value;
    }

//-----------------------------------------------
wxString ReadabilityMessages::GetMonthLabel(size_t month)
    {
    month = std::min<size_t>(month, 9);

    switch (month)
        {
    case 0:
        return _(L"first month of class");
        break;
    case 1:
        return _(L"first month of class completed");
        break;
    case 2:
        return _(L"second month of class completed");
        break;
    case 3:
        return _(L"third month of class completed");
        break;
    case 4:
        return _(L"fourth month of class completed");
        break;
    case 5:
        return _(L"fifth month of class completed");
        break;
    case 6:
        return _(L"sixth month of class completed");
        break;
    case 7:
        return _(L"seventh month of class completed");
        break;
    case 8:
        return _(L"eighth month of class completed");
        break;
    case 9:
        return _(L"ninth month of class completed");
        break;
    default:
        wxLogDebug(L"Invalid month in GetMonthLabel()!");
        return {};
        }

    return {};
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetAgeFromUSGrade(double value, const ReadingAgeDisplay displayFormat)
    {
    // round the grade score to the month before converting to an age
    value = round_decimal_place(value, 10);
    if (displayFormat == ReadingAgeDisplay::ReadingAgeRoundToSemester)
        {
        const size_t age = static_cast<size_t>(round_to_integer(value)) + 5;
        return std::to_wstring(age);
        }
    return wxString::Format(L"%zu-%zu", static_cast<size_t>(std::floor(value)) + 5,
                            static_cast<size_t>(std::floor(value)) + 6);
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetAgeFromUSGrade(const size_t firstGrade, const size_t secondGrade,
                                                const ReadingAgeDisplay displayFormat)
    {
    if (firstGrade == secondGrade)
        {
        return GetAgeFromUSGrade(firstGrade, displayFormat);
        }
    if (displayFormat == ReadingAgeDisplay::ReadingAgeRoundToSemester)
        {
        const double age = safe_divide<double>((firstGrade + secondGrade), 2) + 5;
        return std::to_wstring(static_cast<size_t>(round_to_integer(age)));
        }
    return wxString::Format(L"%zu-%zu", firstGrade + 5, secondGrade + 6);
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetUSGradeScaleLabel(size_t value)
    {
    value = readability::truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        return _(L"Kindergarten");
        break;
    case 1:
        return _(L"1st grade");
        break;
    case 2:
        return _(L"2nd grade");
        break;
    case 3:
        return _(L"3rd grade");
        break;
    case 4:
        return _(L"4th grade");
        break;
    case 5:
        return _(L"5th grade");
        break;
    case 6:
        return _(L"6th grade");
        break;
    case 7:
        return _(L"7th grade");
        break;
    case 8:
        return _(L"8th grade");
        break;
    case 9:
        return _(L"High school freshman");
        break;
    case 10:
        return _(L"High school sophomore");
        break;
    case 11:
        return _(L"High school junior");
        break;
    case 12:
        return _(L"High school senior");
        break;
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetNewfoundlandAndLabradorScaleLabel(size_t value)
    {
    value = readability::truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        return _(L"Kindergarten");
        break;
    case 1:
        return _(L"Grade 1 (Primary)");
        break;
    case 2:
        return _(L"Grade 2 (Primary)");
        break;
    case 3:
        return _(L"Grade 3 (Primary)");
        break;
    case 4:
        return _(L"Grade 4 (Elementary)");
        break;
    case 5:
        return _(L"Grade 5 (Elementary)");
        break;
    case 6:
        return _(L"Grade 6 (Elementary)");
        break;
    case 7:
        return _(L"Grade 7 (Intermediate)");
        break;
    case 8:
        return _(L"Grade 8 (Intermediate)");
        break;
    case 9:
        return _(L"Grade 9 (Intermediate)");
        break;
    case 10:
        return _(L"Senior High (Level I)");
        break;
    case 11:
        return _(L"Senior High (Level II)");
        break;
    case 12:
        return _(L"Senior High (Level III)");
        break;
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetBritishColumbiaScaleLabel(size_t value)
    {
    value = readability::truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        return _(L"Kindergarten (Elementary)");
        break;
    case 1:
        return _(L"Grade 1 (Elementary)");
        break;
    case 2:
        return _(L"Grade 2 (Elementary)");
        break;
    case 3:
        return _(L"Grade 3 (Elementary)");
        break;
    case 4:
        return _(L"Grade 4 (Elementary)");
        break;
    case 5:
        return _(L"Grade 5 (Elementary)");
        break;
    case 6:
        return _(L"Grade 6 (Elementary)");
        break;
    case 7:
        return _(L"Grade 7 (Elementary)");
        break;
    case 8:
        return _(L"Grade 8 (Junior Secondary)");
        break;
    case 9:
        return _(L"Grade 9 (Junior Secondary)");
        break;
    case 10:
        return _(L"Grade 10 (Junior Secondary)");
        break;
    case 11:
        return _(L"Grade 11 (Senior Secondary)");
        break;
    case 12:
        return _(L"Grade 12 (Senior Secondary)");
        break;
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetNewBrunswickScaleLabel(size_t value)
    {
    value = readability::truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        return _(L"Kindergarten (Elementary)");
        break;
    case 1:
        return _(L"Grade 1 (Elementary)");
        break;
    case 2:
        return _(L"Grade 2 (Elementary)");
        break;
    case 3:
        return _(L"Grade 3 (Elementary)");
        break;
    case 4:
        return _(L"Grade 4 (Elementary)");
        break;
    case 5:
        return _(L"Grade 5 (Elementary)");
        break;
    case 6:
        return _(L"Grade 6 (Middle School)");
        break;
    case 7:
        return _(L"Grade 7 (Middle School)");
        break;
    case 8:
        return _(L"Grade 8 (Middle School)");
        break;
    case 9:
        return _(L"Grade 9 (High School)");
        break;
    case 10:
        return _(L"Grade 10 (High School)");
        break;
    case 11:
        return _(L"Grade 11 (High School)");
        break;
    case 12:
        return _(L"Grade 12 (High School)");
        break;
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetNovaScotiaScaleLabel(size_t value)
    {
    value = readability::truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        return _(L"Primary (Elementary)");
        break;
    case 1:
        return _(L"Grade 1 (Elementary)");
        break;
    case 2:
        return _(L"Grade 2 (Elementary)");
        break;
    case 3:
        return _(L"Grade 3 (Elementary)");
        break;
    case 4:
        return _(L"Grade 4 (Elementary)");
        break;
    case 5:
        return _(L"Grade 5 (Elementary)");
        break;
    case 6:
        return _(L"Grade 6 (Elementary)");
        break;
    case 7:
        return _(L"Grade 7 (Junior High)");
        break;
    case 8:
        return _(L"Grade 8 (Junior High)");
        break;
    case 9:
        return _(L"Grade 9 (Junior High)");
        break;
    case 10:
        return _(L"Grade 10 (Senior High)");
        break;
    case 11:
        return _(L"Grade 11 (Senior High)");
        break;
    case 12:
        return _(L"Grade 12 (Senior High)");
        break;
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetAlbertaScaleLabel(size_t value)
    {
    value = readability::truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        return _(L"Kindergarten (Elementary)");
        break;
    case 1:
        return _(L"Grade 1 (Elementary)");
        break;
    case 2:
        return _(L"Grade 2 (Elementary)");
        break;
    case 3:
        return _(L"Grade 3 (Elementary)");
        break;
    case 4:
        return _(L"Grade 4 (Elementary)");
        break;
    case 5:
        return _(L"Grade 5 (Elementary)");
        break;
    case 6:
        return _(L"Grade 6 (Elementary)");
        break;
    case 7:
        return _(L"Grade 7 (Junior High)");
        break;
    case 8:
        return _(L"Grade 8 (Junior High)");
        break;
    case 9:
        return _(L"Grade 9 (Junior High)");
        break;
    case 10:
        return _(L"Grade 10 (Senior High)");
        break;
    case 11:
        return _(L"Grade 11 (Senior High)");
        break;
    case 12:
        return _(L"Grade 12 (Senior High)");
        break;
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetOntarioScaleLabel(size_t value)
    {
    value = readability::truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        return _(L"Senior Kindergarten (Elementary)");
        break;
    case 1:
        return _(L"Grade 1 (Elementary)");
        break;
    case 2:
        return _(L"Grade 2 (Elementary)");
        break;
    case 3:
        return _(L"Grade 3 (Elementary)");
        break;
    case 4:
        return _(L"Grade 4 (Elementary)");
        break;
    case 5:
        return _(L"Grade 5 (Elementary)");
        break;
    case 6:
        return _(L"Grade 6 (Elementary)");
        break;
    case 7:
        return _(L"Grade 7 (Elementary)");
        break;
    case 8:
        return _(L"Grade 8 (Elementary)");
        break;
    case 9:
        return _(L"Grade 9 (Secondary)");
        break;
    case 10:
        return _(L"Grade 10 (Secondary)");
        break;
    case 11:
        return _(L"Grade 11 (Secondary)");
        break;
    case 12:
        return _(L"Grade 12 (Secondary)");
        break;
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetSaskatchewanScaleLabel(size_t value)
    {
    value = readability::truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        return _(L"Kindergarten");
        break;
    case 1:
        return _(L"Grade 1 (Elementary Level)");
        break;
    case 2:
        return _(L"Grade 2 (Elementary Level)");
        break;
    case 3:
        return _(L"Grade 3 (Elementary Level)");
        break;
    case 4:
        return _(L"Grade 4 (Elementary Level)");
        break;
    case 5:
        return _(L"Grade 5 (Elementary Level)");
        break;
    case 6:
        return _(L"Grade 6 (Middle Level)");
        break;
    case 7:
        return _(L"Grade 7 (Middle Level)");
        break;
    case 8:
        return _(L"Grade 8 (Middle Level)");
        break;
    case 9:
        return _(L"Grade 9 (Middle Level)");
        break;
    case 10:
        return _(L"Grade 10 (Secondary Level)");
        break;
    case 11:
        return _(L"Grade 11 (Secondary Level)");
        break;
    case 12:
        return _(L"Grade 12 (Secondary Level)");
        break;
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetPrinceEdwardIslandScaleLabel(size_t value)
    {
    value = readability::truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        return _(L"Kindergarten");
        break;
    case 1:
        return _(L"Grade 1 (Elementary)");
        break;
    case 2:
        return _(L"Grade 2 (Elementary)");
        break;
    case 3:
        return _(L"Grade 3 (Elementary)");
        break;
    case 4:
        return _(L"Grade 4 (Elementary)");
        break;
    case 5:
        return _(L"Grade 5 (Elementary)");
        break;
    case 6:
        return _(L"Grade 6 (Elementary)");
        break;
    case 7:
        return _(L"Grade 7 (Intermediate School)");
        break;
    case 8:
        return _(L"Grade 8 (Intermediate School)");
        break;
    case 9:
        return _(L"Grade 9 (Intermediate School)");
        break;
    case 10:
        return _(L"Grade 10 (Senior High)");
        break;
    case 11:
        return _(L"Grade 11 (Senior High)");
        break;
    case 12:
        return _(L"Grade 12 (Senior High)");
        break;
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetManitobaScaleLabel(size_t value)
    {
    value = readability::truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        return _(L"Kindergarten (Early)");
        break;
    case 1:
        return _(L"Grade 1 (Early)");
        break;
    case 2:
        return _(L"Grade 2 (Early)");
        break;
    case 3:
        return _(L"Grade 3 (Early)");
        break;
    case 4:
        return _(L"Grade 4 (Early)");
        break;
    case 5:
        return _(L"Grade 5 (Middle)");
        break;
    case 6:
        return _(L"Grade 6 (Middle)");
        break;
    case 7:
        return _(L"Grade 7 (Middle)");
        break;
    case 8:
        return _(L"Grade 8 (Middle)");
        break;
    case 9:
        return _(L"Grade 9 (Senior 1)");
        break;
    case 10:
        return _(L"Grade 10 (Senior 2)");
        break;
    case 11:
        return _(L"Grade 11 (Senior 3)");
        break;
    case 12:
        return _(L"Grade 12 (Senior 4)");
        break;
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetNorthwestTerritoriesScaleLabel(size_t value)
    {
    value = readability::truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        return _(L"Kindergarten (Primary)");
        break;
    case 1:
        return _(L"Grade 1 (Primary)");
        break;
    case 2:
        return _(L"Grade 2 (Primary)");
        break;
    case 3:
        return _(L"Grade 3 (Primary)");
        break;
    case 4:
        return _(L"Grade 4 (Intermediate)");
        break;
    case 5:
        return _(L"Grade 5 (Intermediate)");
        break;
    case 6:
        return _(L"Grade 6 (Intermediate)");
        break;
    case 7:
        return _(L"Grade 7 (Junior Secondary)");
        break;
    case 8:
        return _(L"Grade 8 (Junior Secondary)");
        break;
    case 9:
        return _(L"Grade 9 (Junior Secondary)");
        break;
    case 10:
        return _(L"Grade 10 (Senior Secondary)");
        break;
    case 11:
        return _(L"Grade 11 (Senior Secondary)");
        break;
    case 12:
        return _(L"Grade 12 (Senior Secondary)");
        break;
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetNunavutScaleLabel(size_t value)
    {
    value = readability::truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        return _(L"Kindergarten");
        break;
    case 1:
        return _(L"Grade 1");
        break;
    case 2:
        return _(L"Grade 2");
        break;
    case 3:
        return _(L"Grade 3");
        break;
    case 4:
        return _(L"Grade 4");
        break;
    case 5:
        return _(L"Grade 5");
        break;
    case 6:
        return _(L"Grade 6");
        break;
    case 7:
        return _(L"Grade 7");
        break;
    case 8:
        return _(L"Grade 8");
        break;
    case 9:
        return _(L"Grade 9");
        break;
    case 10:
        return _(L"Grade 10");
        break;
    case 11:
        return _(L"Grade 11");
        break;
    case 12:
        return _(L"Grade 12");
        break;
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------
wxString ReadabilityMessages::GetQuebecScaleLabel(size_t value)
    {
    // there are 20 grades in Quebec, just broken up differently
    value = readability::truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        return _DT(L"Maternelle");
        break;
    case 1:
        return _(L"Grade 1 (École Primaire)");
        break;
    case 2:
        return _(L"Grade 2 (École Primaire)");
        break;
    case 3:
        return _(L"Grade 3 (École Primaire)");
        break;
    case 4:
        return _(L"Grade 4 (École Primaire)");
        break;
    case 5:
        return _(L"Grade 5 (École Primaire)");
        break;
    case 6:
        return _(L"Grade 6 (École Primaire)");
        break;
    case 7:
        return _DT(L"1 Secondaire (École Secondaire)");
        break;
    case 8:
        return _DT(L"2 Secondaire (École Secondaire)");
        break;
    case 9:
        return _DT(L"3 Secondaire (École Secondaire)");
        break;
    case 10:
        return _DT(L"4 Secondaire (École Secondaire)");
        break;
    case 11:
        return _DT(L"5 Secondaire (École Secondaire)");
        break;
    case 12:
        return _(L"CEGEP, year 1");
        break;
    case 13:
        return _(L"CEGEP, year 2");
        break;
    case 14:
        return _(L"University, year 1");
        break;
    case 15:
        return _(L"University, year 2");
        break;
    case 16:
        return _(L"University, year 3");
        break;
    // university graduate  system is the same in Quebec as K-12
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------
wxString ReadabilityMessages::GetEnglandWalesScaleLabel(size_t value)
    {
    // there are 20 grades in the U.K., just broken up differently
    value = readability::truncate_k12_plus_grade(value);

    switch (value)
        {
    case 0:
        // TRANSLATORS: "Keys stage" should probably not be translated, but "year" can.
        return _(L"Key stage 1 (year 1)");
        break;
    case 1:
        return _(L"Key stage 1 (year 2)");
        break;
    case 2:
        return _(L"Key stage 2 (year 3)");
        break;
    case 3:
        return _(L"Key stage 2 (year 4)");
        break;
    case 4:
        return _(L"Key stage 2 (year 5)");
        break;
    case 5:
        return _(L"Key stage 2 (year 6)");
        break;
    case 6:
        return _(L"Key stage 3 (year 7)");
        break;
    case 7:
        return _(L"Key stage 3 (year 8)");
        break;
    case 8:
        return _(L"Key stage 3 (year 9)");
        break;
    case 9:
        return _(L"Key stage 4 (year 10)");
        break;
    case 10:
        return _(L"Key stage 4 (year 11)");
        break;
    case 11:
        return _(L"Sixth Form, year 1");
        break;
    case 12:
        return _(L"Sixth Form, year 2");
        break;
    // university graduate  system is the same in Quebec as K-12
    default:
        return GetPostSecondaryScaleLabel(value);
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetPostSecondaryScaleLabel(size_t value)
    {
    switch (value)
        {
    case 13:
        return _(L"University freshman");
        break;
    case 14:
        return _(L"University sophomore");
        break;
    case 15:
        return _(L"University junior");
        break;
    case 16:
        return _(L"University senior");
        break;
    case 17:
        return _(L"Post-graduate freshman");
        break;
    case 18:
        return _(L"Post-graduate sophomore");
        break;
    case 19:
        return _(L"Doctorate");
        break;
    default:
        return wxEmptyString;
        };
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetGradeScaleDescription(double value) const
    {
    size_t grade{ 0 }, month{ 0 };
    readability::split_k12_plus_grade_score(value, grade, month);

    return wxString::Format(
        // TRANSLATORS: "%s%s, %s%s" is opening highlight tag, grade, month,
        // and closing highlight tag
        _(L"This document is at least suitable for a reader at the level: %s%s, %s%s."),
        GetHighlightBegin(), GetGradeScaleLongLabel(grade), GetMonthLabel(month),
        GetHighlightEnd());
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetGradeScaleDescription(size_t value) const
    {
    // make sure that reasonable values were entered
    value = readability::truncate_k12_plus_grade(value);

    return wxString::Format(
        // TRANSLATORS: "%s%s%s" is opening highlight tag, grade level description,
        // and closing highlight tag
        _(L"This document is at least suitable for a reader at the level: %s%s%s."),
        GetHighlightBegin(), GetGradeScaleLongLabel(value), GetHighlightEnd());
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetGradeScaleDescription(size_t firstGrade, size_t secondGrade) const
    {
    // make sure that reasonable values were entered
    firstGrade = readability::truncate_k12_plus_grade(firstGrade);
    secondGrade = readability::truncate_k12_plus_grade(secondGrade);

    return wxString::Format(
        // TRANSLATORS: "%s%s%s" is opening highlight tag, grade level description,
        // and closing highlight tag
        _(L"This document is at least suitable for a reader between the levels: %s%s%s to %s%s%s."),
        GetHighlightBegin(), GetGradeScaleLongLabel(firstGrade), GetHighlightEnd(),
        GetHighlightBegin(), GetGradeScaleLongLabel(secondGrade), GetHighlightEnd());
    }

//-----------------------------------------------------
wxString
ReadabilityMessages::GetDanielsonBryan2Description(const readability::flesch_difficulty diffLevel)
    {
    switch (diffLevel)
        {
    case readability::flesch_difficulty::flesch_very_difficult:
        return _(L"Text is very difficult to read, college level.");
    case readability::flesch_difficulty::flesch_difficult:
        return _(L"Text is difficult to read, high school level.");
    case readability::flesch_difficulty::flesch_fairly_difficult:
        return _(L"Text is fairly difficult to read, junior high school level.");
    case readability::flesch_difficulty::flesch_standard:
        return _(L"Text is average in terms of reading difficulty, sixth-grade level.");
    case readability::flesch_difficulty::flesch_fairly_easy:
        return _(L"Text is fairly easy to read, fifth-grade level.");
    case readability::flesch_difficulty::flesch_easy:
        return _(L"Text is easy to read, fourth-grade level.");
    case readability::flesch_difficulty::flesch_very_easy:
        return _(L"Text is very easy to read, third-grade level.");
    default:
        return _(L"Unable to determine difficulty.");
        };
    return {};
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetInfleszDescription(const readability::inflesz_difficulty diffLevel)
    {
    switch (diffLevel)
        {
    case readability::inflesz_difficulty::very_difficult:
        return _DT(L"MUY DIFÍCIL");
    case readability::inflesz_difficulty::fairly_difficult:
        return _DT(L"ALGO DIFÍCIL");
    case readability::inflesz_difficulty::normal:
        return _DT(L"NORMAL");
    case readability::inflesz_difficulty::fairly_easy:
        return _DT(L"BASTANTE FÁCIL");
    case readability::inflesz_difficulty::very_easy:
        return _DT(L"MUY FÁCIL");
        };
    return {};
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetFleschDescription(const readability::flesch_difficulty diffLevel)
    {
    switch (diffLevel)
        {
    case readability::flesch_difficulty::flesch_very_difficult:
        return _(L"Text is very difficult to read.");
    case readability::flesch_difficulty::flesch_difficult:
        return _(L"Text is difficult to read.");
    case readability::flesch_difficulty::flesch_fairly_difficult:
        return _(L"Text is fairly difficult to read.");
    case readability::flesch_difficulty::flesch_standard:
        return _(L"Text is average (i.e., \"Plain English\") in terms of reading difficulty.");
    case readability::flesch_difficulty::flesch_fairly_easy:
        return _(L"Text is fairly easy to read.");
    case readability::flesch_difficulty::flesch_easy:
        return _(L"Text is easy to read.");
    case readability::flesch_difficulty::flesch_very_easy:
        return _(L"Text is very easy to read.");
        };
    return {};
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetEflawDescription(const readability::eflaw_difficulty diffLevel)
    {
    switch (diffLevel)
        {
    case readability::eflaw_difficulty::eflaw_very_confusing:
        return _(L"Text is very confusing to read.");
    case readability::eflaw_difficulty::eflaw_confusing:
        return _(L"Text is confusing to read.");
    case readability::eflaw_difficulty::eflaw_easy:
        return _(L"Text is easy to read.");
    case readability::eflaw_difficulty::eflaw_very_easy:
        return _(L"Text is very easy to read.");
        };
    return {};
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetLixDescription(const readability::lix_difficulty diffLevel)
    {
    switch (diffLevel)
        {
    case readability::lix_difficulty::lix_very_difficult:
        return _(L"Text is very difficult to read.");
    case readability::lix_difficulty::lix_difficult:
        return _(L"Text is difficult to read.");
    case readability::lix_difficulty::lix_average:
        return _(L"Text is average in terms of reading difficulty.");
    case readability::lix_difficulty::lix_easy:
        return _(L"Text is easy to read.");
    case readability::lix_difficulty::lix_very_easy:
        return _(L"Text is very easy to read.");
        };
    return {};
    }

//-----------------------------------------------------
wxString ReadabilityMessages::GetLixDescription(const readability::german_lix_difficulty diffLevel)
    {
    switch (diffLevel)
        {
    case readability::german_lix_difficulty::german_lix_very_easy:
        return _(L"Text is very easy to read.");
    case readability::german_lix_difficulty::german_lix_children_and_youth:
        return _(L"Text is at a children and young adult level of difficulty.");
    case readability::german_lix_difficulty::german_lix_easy:
        return _(L"Text is easy to read.");
    case readability::german_lix_difficulty::german_lix_adult_fiction:
        return _(L"Text is at an adult fiction level of difficulty.");
    case readability::german_lix_difficulty::german_lix_average:
        return _(L"Text is average in terms of reading difficulty.");
    case readability::german_lix_difficulty::german_lix_nonfiction:
        return _(L"Text is at a non-fiction level of difficulty.");
    case readability::german_lix_difficulty::german_lix_difficult:
        return _(L"Text is difficult to read.");
    case readability::german_lix_difficulty::german_lix_technical:
        return _(L"Text is at a technical document level of difficulty.");
    case readability::german_lix_difficulty::german_lix_very_difficult:
        return _(L"Text is very difficult to read.");
        };
    return {};
    }

//-------------------------------------------------------
wxString ReadabilityMessages::GetFraseDescription(const size_t fraseLevel)
    {
    switch (fraseLevel)
        {
    case 1:
        return _(L"Beginning (Level I)");
    case 2:
        return _(L"Intermediate (Level II)");
    case 3:
        return _(L"Advanced Intermediate (Level III)");
    case 4:
        return _(L"Advanced (Level IV)");
    default:
        return _(L"Unknown Level");
        };
    return {};
    }

//-------------------------------------------------------
wxString ReadabilityMessages::GetDrpUnitDescription(const size_t drpScore)
    {
    return wxString::Format(_(L"DRP (difficulty) Units: %zu"), drpScore);
    }

//-------------------------------------------------------
wxString ReadabilityMessages::GetPredictedClozeDescription(const int clozeScore)
    {
    return wxString::Format(_(L"Predicted mean cloze score: %i%%"), clozeScore);
    }

//-------------------------------------------------------
bool ReadabilityMessages::GetScoreValue(const wxString& valueStr, double& value)
    {
    const wchar_t* const valueStrPtr = valueStr.wc_str();
    wchar_t* end{ nullptr };

    value = string_util::strtod_ex(valueStrPtr, &end);
    return (valueStrPtr != end);
    }
