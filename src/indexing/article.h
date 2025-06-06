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

#ifndef __ARTICLE_H__
#define __ARTICLE_H__

#include "../Wisteria-Dataviz/src/util/string_util.h"
#include "abbreviation.h"
#include "character_traits.h"
#include "word_list.h"
#include <algorithm>
#include <functional>
#include <set>
#include <string_view>

namespace grammar
    {
    /** @brief Base (abstract) class for determining an invalid article/proceeding word pair.*/
    class is_incorrect_article
        {
      public:
        /// @private
        virtual ~is_incorrect_article() {}

        /** @brief Determines if an article/proceeding word pair is a mismatch.
            @param article The article.
            @param word The word following the article.
            @returns Whether an article/following word pair is a mismatch.*/
        [[nodiscard]]
        virtual bool
        operator()(std::wstring_view article, std::wstring_view word) const = 0;
        };

    /** @brief Predicate for determining a mismatching article/proceeding word pair.*/
    class is_incorrect_english_article final : public is_incorrect_article
        {
      public:
        /** @brief Determines if an article/proceeding word pair is a mismatch.
            @param article The article.
            @param word The word following the article.
            @returns Whether an article/following word pair is a mismatch.*/
        [[nodiscard]]
        bool
        operator()(std::wstring_view article, std::wstring_view word) const noexcept final;

        /** @returns The list of words following an 'a' that would be correct
                (that the analyzer might get wrong).*/
        [[nodiscard]]
        static word_list& get_a_exceptions() noexcept
            {
            return m_a_exceptions;
            }

        /** @returns The list of words following an 'an' that would be correct
                (that the analyzer might get wrong).*/
        [[nodiscard]]
        static word_list& get_an_exceptions() noexcept
            {
            return m_an_exceptions;
            }

      private:
        /** @returns @c true if a consonant-starting @c word is actually correct to
                appear after an "an".
            @param word The word to review.\n
                For example, "honor" would be an exception ("an honor").**/
        [[nodiscard]]
        static bool is_an_exception(std::wstring_view word);

        /** @returns @c true if a vowel-starting word is actually correct to
                appear after an 'a'.
            @param word The word to review.*/
        [[nodiscard]]
        static bool is_a_exception(std::wstring_view word);

        static word_list m_a_exceptions;
        static word_list m_an_exceptions;
        };
    } // namespace grammar

#endif //__ARTICLE_H__
