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

#ifndef GRADE_SCALES_H
#define GRADE_SCALES_H

#include <algorithm>

namespace readability
    {
    /// @brief Grade scales from across the globe.
    enum class grade_scale
        {
        /// @brief US K-12 scale.
        k12_plus_united_states,
        // Canadian grade scales
        //----------------------
        /** https://www.education.alberta.ca/media/832568/guidetoed.pdf
            - Elementary(Kindergarten-Grade 6)
            - Junior High (Grade 7-9)
            - Senior High (Grade 10-12)*/
        k12_plus_alberta,
        /** https://www.bced.gov.bc.ca/reporting/glossary.php#G.\n
            Yukon has the same curriculum as BC
           (https://www.education.gov.yk.ca/psb/curriculum.html)
            - Elementary (Kindergarten-Grade 7)
            - Junior Secondary (Grade 8-10)
            - Senior Secondary (Grade 11-12)*/
        k12_plus_british_columbia,
        /** https://www.edu.gov.mb.ca/ks4/schools/gts.html
            - Early (Kindergarten-Grade 4)
            - Middle (Grade 5-8)
            - Senior (Grade 9-12)*/
        k12_plus_manitoba,
        /** https://www.gnb.ca/0000/anglophone-e.asp
            - Elementary (Kindergarten-Grade 5)
            - Middle School (Grade 6-8)
            - High School (Grade 9-12)*/
        k12_plus_newbrunswick,
        /** https://www.ed.gov.nl.ca/edu/sp/pcdbgl.htm
            - Kindergarten
            - Primary (Grade 1-3)
            - Elementary (Grade 4-6)
            - Intermediate (Grade 7-9)
            - Senior High (Level I-III)*/
        k12_plus_newfoundland_and_labrador,
        /** https://www.ece.gov.nt.ca/Divisions/kindergarten_g12/curriculum/Elementary_Junior_Secondary_School_Handbook/Elementary_Junior_Secondary_School_Handbook.htm
            - Primary (Kindergarten-Grade 3)
            - Intermediate (Grade 4-6)
            - Junior Secondary (Grade 7-9)
            - Senior Secondary (Grade 10-12)*/
        k12_plus_northwest_territories,
        /** https://www.ednet.ns.ca/pdfdocs/psp/psp_03_04_full.pdf
            - Elementary (Primary-Grade 6)
            - Junior High (Grade 7-9)
            - Senior High (Grade 10-12)*/
        k12_plus_nova_scotia,
        /** https://www.edu.gov.on.ca/eng/educationFacts.html
            - Elementary (Junior Kindergarten-Grade 8)
            - Secondary (Grade 9-12)

            NOTE: Junior Kindergarten is really pre-school and does not apply to reading levels.*/
        k12_plus_ontario,
        /** https://www.edu.pe.ca/curriculum/default.asp
            - Kindergarten
            - Elementary (Grade 1-6)
            - Intermediate School (Grade 7-9)
            - Senior High (Grade 10-12)*/
        k12_plus_prince_edward_island,
        /** https://www.sasked.gov.sk.ca/branches/curr/index.shtml
            - Kindergarten
            - Elementary Level (Grade 1-5)
            - Middle Level (Grade 6-9)
            - Secondary Level (Grade 10-12)*/
        k12_plus_saskatchewan,
        /** https://www.ntanu.ca/assets/docs/Handout-Nunavut%20Approved%20Teaching%20Resources%20Version%20(5).pdf
           - Kindergarten, Grades 1-12.

           Unfortunately, this is as detailed information as I could find.*/
        k12_plus_nunavut,
        /** https://www.ibe.unesco.org/en/access-by-country/europe-and-north-america/canada/curricular-resources.html
            - Maternelle (Kindergarten)
            - Ecole Primaire (Grades 1-6)
            - Ecole Secondaire (grades 7-11)
            - CEGEP (two years of University prep)
            - University (three years for Bachelors Degree)
            - Graduate degree (two years)
            - Doctoral degree after that.*/
        quebec,
        // England & Wales
        //----------------
        /** https://curriculum.qca.org.uk/index.aspx\n
            https://old.accac.org.uk/index.php
            - Key stage 1: Ages 5-7 (Years 1-2)
            - Key stage 2: Ages 7-11 (Years 3-6)
            - Key stage 3: Ages 11-14 (Years 7-9)
            - Key stage 4: Ages 14-16 (Years 10-11)
            - Sixth Form (2 years)
            - Standard college after that.*/
        key_stages_england_wales
        };

    /// @brief Bounds checks a K-12 grade within 0-19.
    /// @param value The value to clamp to a K-12 grade level.
    /// @returns @c value, clamped to a K-12 grade level.
    [[nodiscard]]
    inline constexpr double truncate_k12_plus_grade(const double value) noexcept
        {
        return std::clamp<double>(value, 0, 19);
        }

    /** @brief Splits a K-12 grade score into grade and month.
        @param USGradeScore The grade score to split.
        @param[out] grade The grade from \c USGradeScore.
        @param[out] month The month from \c USGradeScore.*/
    inline void split_k12_plus_grade_score(double USGradeScore, size_t& grade,
                                           size_t& month) noexcept
        {
        // round the value to the nearest single decimal place
        USGradeScore = safe_divide<double>(round_to_integer(USGradeScore * 10), 10);
        // make sure that reasonable values were entered
        USGradeScore = truncate_k12_plus_grade(USGradeScore);
        // split up the number
        double intpart{ 0.0 };
        const double fractpart = std::modf(USGradeScore, &intpart);
        grade = static_cast<size_t>(intpart);
        month = static_cast<size_t>(round_to_integer(fractpart * 10));
        month = std::min<size_t>(month, 9);
        }
    } // namespace readability

#endif // GRADE_SCALES_H
