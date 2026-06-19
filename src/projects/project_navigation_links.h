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

#ifndef PROJECT_NAVIGATION_LINKS_H
#define PROJECT_NAVIGATION_LINKS_H

#include <array>
#include <string_view>
#include <wx/string.h>

/// @brief Centralized identifiers for the in-app navigation hyperlinks embedded
///     in the readability-score explanations and statistics reports.
/// @details A link such as `<a href="#fry-graph">` selects the matching graph or
///     list window in the sidebar (see @c ProjectView::NavigateToHref).
namespace NavLink
    {
    // graph and chart links
    inline constexpr std::wstring_view FryGraph{ L"fry-graph" };
    inline constexpr std::wstring_view RaygorGraph{ L"raygor-graph" };
    inline constexpr std::wstring_view GilliamPenaMountainGraph{ L"gilliam-pena-mountain-graph" };
    inline constexpr std::wstring_view FraseGraph{ L"frase-graph" };
    inline constexpr std::wstring_view DanielsonBryan2{ L"danielson-bryan-2" };
    inline constexpr std::wstring_view Schwartz{ L"schwartz" };
    inline constexpr std::wstring_view Inflesz{ L"inflesz" };
    inline constexpr std::wstring_view CrawfordGraph{ L"crawford-graph" };
    inline constexpr std::wstring_view LixGauge{ L"lix-gauge" };
    inline constexpr std::wstring_view GermanLixGauge{ L"german-lix-gauge" };
    inline constexpr std::wstring_view FleschChart{ L"flesch-chart" };

    // word/sentence list links
    inline constexpr std::wstring_view Misspellings{ L"misspellings" };
    inline constexpr std::wstring_view RepeatedWords{ L"repeated-words" };
    inline constexpr std::wstring_view MismatchedArticles{ L"mismatched-articles" };
    inline constexpr std::wstring_view WordingErrors{ L"wording-errors" };
    inline constexpr std::wstring_view RedundantPhrases{ L"redundant-phrases" };
    inline constexpr std::wstring_view OverusedWordsBySentence{ L"overused-words-by-sentence" };
    inline constexpr std::wstring_view WordyPhrases{ L"wordy-phrases" };
    inline constexpr std::wstring_view Cliches{ L"cliches" };
    inline constexpr std::wstring_view PassiveVoice{ L"passive-voice" };
    inline constexpr std::wstring_view SentenceStartingWithConjunctions{
        L"sentence-starting-with-conjunctions"
    };
    inline constexpr std::wstring_view SentenceStartingWithLowercase{
        L"sentence-starting-with-lowercase"
    };
    inline constexpr std::wstring_view DifficultSentences{ L"difficult-sentences" };
    inline constexpr std::wstring_view HardWords{ L"hard-words" };
    inline constexpr std::wstring_view LongWords{ L"long-words" };
    inline constexpr std::wstring_view DaleChallWords{ L"dale-chall-words" };
    inline constexpr std::wstring_view HarrisJacobsonWords{ L"harris-jacobson-words" };
    inline constexpr std::wstring_view SpacheWords{ L"spache-words" };
    inline constexpr std::wstring_view UnusedDolchWords{ L"unused-dolch-words" };
    inline constexpr std::wstring_view Dolch{ L"dolch" };

    // action links
    inline constexpr std::wstring_view SelectStatistics{ L"select-statistics" };
    inline constexpr std::wstring_view FogHelp{ L"fog-help" };

    /// @brief Custom URL scheme used so the explanation @c wxWebView reports link
    ///     clicks as (cross-document) navigations that can be vetoed on every backend.
    /// @details A @c wxWebView (notably WebView2 on Windows) does not fire
    ///     @c wxEVT_WEBVIEW_NAVIGATING for same-document `#anchor` clicks, so the
    ///     anchors are rewritten to this scheme just before display.
    inline constexpr std::wstring_view ExplanationScheme{ L"rsexplain://" };

    /// @brief Rewrites known in-app anchor links (`href="#id"`) to the custom scheme
    ///     (`href="rsexplain://id"`) so a @c wxWebView fires a vetoable
    ///     navigation for them.
    /// @param html The explanation HTML to display.
    /// @returns The HTML with its in-app anchors rewritten to the custom scheme.
    /// @note Only the known nav-anchor IDs are rewritten; unrecognized anchors are left as-is.
    [[nodiscard]]
    inline wxString AnchorsToExplanationScheme(wxString html)
        {
        constexpr static auto knownAnchors =
            std::to_array<std::wstring_view>({ FryGraph,
                                               RaygorGraph,
                                               GilliamPenaMountainGraph,
                                               FraseGraph,
                                               DanielsonBryan2,
                                               Schwartz,
                                               Inflesz,
                                               CrawfordGraph,
                                               LixGauge,
                                               GermanLixGauge,
                                               FleschChart,
                                               Misspellings,
                                               RepeatedWords,
                                               MismatchedArticles,
                                               WordingErrors,
                                               RedundantPhrases,
                                               OverusedWordsBySentence,
                                               WordyPhrases,
                                               Cliches,
                                               PassiveVoice,
                                               SentenceStartingWithConjunctions,
                                               SentenceStartingWithLowercase,
                                               DifficultSentences,
                                               HardWords,
                                               LongWords,
                                               DaleChallWords,
                                               HarrisJacobsonWords,
                                               SpacheWords,
                                               UnusedDolchWords,
                                               Dolch,
                                               SelectStatistics,
                                               FogHelp });
        const wxString scheme(ExplanationScheme.data(), ExplanationScheme.length());
        for (const auto anchor : knownAnchors)
            {
            const wxString id(anchor.data(), anchor.length());
            html.Replace(L"href=\"#" + id + L"\"", L"href=\"" + scheme + id + L"\"");
            }
        return html;
        }
    } // namespace NavLink

#endif // PROJECT_NAVIGATION_LINKS_H
