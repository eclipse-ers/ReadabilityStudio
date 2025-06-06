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

#ifndef __PROJECT_REFRESH_H__
#define __PROJECT_REFRESH_H__

/// @brief Class for managing which sections of a project need to be updated.
class ProjectRefresh
    {
  public:
    /// @brief The type of refresh to perform.
    /// @internal Used as a bitmask.
    enum RefreshRequirement
        {
        /// @brief Don't refresh anything.
        NoRefresh = 0,
        /// @brief Refresh only the reports and graphs.
        Minimal = (1 << 0),
        /// @brief Refresh the highlighted text (standard projects).
        TextSection = (1 << 1),
        /// @brief Reload the document(s) and refresh all results.
        FullReindexing = (1 << 2)
        };

    /// @brief Adds a flag indicating what needs to be updated on the next
    ///     call to RefreshProject().
    /// @note This is cumulative, so the flags passed in here will add up,
    ///     unless NoRefresh is passed, which will reset everything.
    /// @param refreshNeeded The type of refresh to perform.
    void RefreshRequired(const RefreshRequirement refreshNeeded) noexcept
        {
        if (refreshNeeded == NoRefresh)
            {
            ResetRefreshRequired();
            }
        else
            {
            m_refreshNeeded = (m_refreshNeeded | refreshNeeded);
            }
        }

    /// @brief Resets the state.
    void ResetRefreshRequired() noexcept { m_refreshNeeded = NoRefresh; }

    /// @returns Whether anything is needed to be refreshed.
    [[nodiscard]]
    bool IsRefreshRequired() const noexcept
        {
        return m_refreshNeeded != NoRefresh;
        }

    /// @returns Whether the highlighted text needs to be reformatted.
    [[nodiscard]]
    bool IsTextSectionRefreshRequired() const noexcept
        {
        return (m_refreshNeeded & TextSection) != 0;
        }

    /// @returns Whether the document(s) need to be reloaded.
    [[nodiscard]]
    bool IsDocumentReindexingRequired() const noexcept
        {
        return (m_refreshNeeded & FullReindexing) != 0;
        }

  private:
    int m_refreshNeeded{ NoRefresh };
    };

#endif //__PROJECT_REFRESH_H__
