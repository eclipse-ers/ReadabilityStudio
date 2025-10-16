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

#ifndef EXPLANATION_LISTCTRL_H
#define EXPLANATION_LISTCTRL_H

#include "../../Wisteria-Dataviz/src/base/canvas.h"
#include "../../Wisteria-Dataviz/src/ui/controls/htmltablewin.h"
#include "../../Wisteria-Dataviz/src/ui/controls/listctrlex.h"
#include "../../Wisteria-Dataviz/src/util/parentblocker.h"
#include <map>
#include <wx/html/htmprint.h>
#include <wx/splitter.h>
#include <wx/xrc/xmlres.h>

/// @brief What to export when saving from an ExplanationListCtrl.
enum class ExplanationListExportOptions
    {
    /// @brief Export just the grid.
    ExportGrid,
    /// @brief Export just the explanations.
    ExportExplanations,
    /// @brief Export both the grid and associated explanations.
    ExportBoth
    };

/// @brief A list control with an HTML buddy window.
/// @details Selecting an item in the list will display its associated text in the HTML window.
class ExplanationListCtrl final : public wxSplitterWindow
    {
  public:
    /** @brief Constructor.
        @param parent The parent window.
        @param id The window's ID.
        @param point The window's position.
        @param size The window's size.
        @param name The name of the control.*/
    ExplanationListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& point = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        const wxString& name = L"ExplanationListCtrl");
    /// @private
    ExplanationListCtrl(const ExplanationListCtrl&) = delete;
    /// @private
    ExplanationListCtrl& operator=(const ExplanationListCtrl&) = delete;

    /// @brief Splits the windows to show the items in the list control.
    void FitWindows();
    /// @brief Updates the HTML window to display the associated text
    ///     for the currently selected item in the list.
    void UpdateExplanationDisplay();
    /** @brief Saves the control.
        @param filePath The path to save the report to.
        @param exportOptions Options for how to format the output.*/
    bool Save(const wxFileName& filePath, const ExplanationListExportOptions exportOptions =
                                              ExplanationListExportOptions::ExportBoth) const;
    /** @brief Prints the grid area of the control.*/
    void Print();
    /** @brief Print previews the grid area of the control.*/
    void PrintPreview();

    /** @brief Sets the printer settings.
        @param printData A pointer to the printer settings to use.*/
    void SetPrinterSettings(wxPrintData* printData) noexcept { m_printData = printData; }

    /// @brief Sets the left header used for printing.
    /// @param header The string to use.
    void SetLeftPrinterHeader(const wxString& header) { m_leftPrinterHeader = header; }

    /// @returns The left header used for printing.
    [[nodiscard]]
    const wxString& GetLeftPrinterHeader() const noexcept
        {
        return m_leftPrinterHeader;
        }

    /// @brief Sets the center header used for printing.
    /// @param header The string to use.
    void SetCenterPrinterHeader(const wxString& header) { m_centerPrinterHeader = header; }

    /// @returns The center header used for printing.
    [[nodiscard]]
    const wxString& GetCenterPrinterHeader() const noexcept
        {
        return m_centerPrinterHeader;
        }

    /// @brief Sets the right header used for printing.
    /// @param header The string to use.
    void SetRightPrinterHeader(const wxString& header) { m_rightPrinterHeader = header; }

    /// @returns The right header used for printing.
    [[nodiscard]]
    const wxString& GetRightPrinterHeader() const noexcept
        {
        return m_rightPrinterHeader;
        }

    /// @brief Sets the left footer used for printing.
    /// @param footer The string to use.
    void SetLeftPrinterFooter(const wxString& footer) { m_leftPrinterFooter = footer; }

    /// @returns The left footer used for printing.
    [[nodiscard]]
    const wxString& GetLeftPrinterFooter() const noexcept
        {
        return m_leftPrinterFooter;
        }

    /// @brief Sets the center footer used for printing.
    /// @param footer The string to use.
    void SetCenterPrinterFooter(const wxString& footer) { m_centerPrinterFooter = footer; }

    /// @returns The center footer used for printing.
    [[nodiscard]]
    const wxString& GetCenterPrinterFooter() const noexcept
        {
        return m_centerPrinterFooter;
        }

    /// @brief Sets the right footer used for printing.
    /// @param footer The string to use.
    void SetRightPrinterFooter(const wxString& footer) { m_rightPrinterFooter = footer; }

    /// @returns The right footer used for printing.
    [[nodiscard]]
    const wxString& GetRightPrinterFooter() const noexcept
        {
        return m_rightPrinterFooter;
        }

    /// @brief Sets the watermark for the list when printed.
    /// @param watermark The watermark information.
    void SetWatermark(const Wisteria::Canvas::Watermark& watermark)
        {
        m_waterMark = watermark;
        }

    /// @returns The printer watermark.
    [[nodiscard]]
    const Wisteria::Canvas::Watermark& GetWatermark() const noexcept
        {
        return m_waterMark;
        }

    /// @returns The number of items in the list.
    [[nodiscard]]
    int GetItemCount() const
        {
        return m_results_view->GetItemCount();
        }

    /// @returns The text from the HTML window.
    [[nodiscard]]
    wxString GetExplanationsText() const;

    /// @returns The keys and their explanations.
    [[nodiscard]]
    std::map<wxString, wxString>& GetExplanations() noexcept
        {
        return m_explanations;
        }

    /// @returns The list control.
    [[nodiscard]]
    Wisteria::UI::ListCtrlEx* GetResultsListCtrl() noexcept
        {
        return m_results_view;
        }

    /// @private
    [[nodiscard]]
    const Wisteria::UI::ListCtrlEx* GetResultsListCtrl() const noexcept
        {
        return m_results_view;
        }

    /// @returns The HTML window.
    [[nodiscard]]
    Wisteria::UI::HtmlTableWindow* GetExplanationView() const noexcept
        {
        return m_explanation_view;
        }

    /// @brief Clears the list, HTML window, removes all data.
    void Clear()
        {
        m_explanations.clear();
        m_explanation_view->SetPage(wxString{});
        m_results_view->DeleteAllItems();
        }

    /// @returns The list's data provider.
    [[nodiscard]]
    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider>& GetDataProvider() noexcept
        {
        return m_data;
        }

    /// @brief Sets the help sort topic for the list control.
    /// @param helpProjectPath The help folder.
    /// @param sortTopicPath The subpath to the topic.
    void SetResources(const wxString& helpProjectPath, const wxString& sortTopicPath)
        {
        if (GetResultsListCtrl() != nullptr)
            {
            GetResultsListCtrl()->SetSortHelpTopic(helpProjectPath, sortTopicPath);
            }
        }

  private:
    ExplanationListCtrl() = default;

    void OnPreview([[maybe_unused]] wxCommandEvent& event);
    void OnPrint([[maybe_unused]] wxCommandEvent& event);
    void OnSave([[maybe_unused]] wxCommandEvent& event);
    void OnCopy([[maybe_unused]] wxCommandEvent& event);
    void OnFind(wxFindDialogEvent& event);
    void OnItemSelected(const wxListEvent& event);
    void OnResize(wxSizeEvent& event);
    void OnMenuCommand(wxCommandEvent& event);

    std::shared_ptr<Wisteria::UI::ListCtrlExNumericDataProvider> m_data{
        std::make_shared<Wisteria::UI::ListCtrlExNumericDataProvider>()
    };
    std::map<wxString, wxString> m_explanations;
    // view classes
    Wisteria::UI::HtmlTableWindow* m_explanation_view{ nullptr };
    Wisteria::UI::ListCtrlEx* m_results_view{ nullptr };

    wxPrintData* m_printData{ nullptr };
    // headers
    wxString m_leftPrinterHeader;
    wxString m_centerPrinterHeader;
    wxString m_rightPrinterHeader;
    // footers
    wxString m_leftPrinterFooter;
    wxString m_centerPrinterFooter;
    wxString m_rightPrinterFooter;

    Wisteria::Canvas::Watermark m_waterMark;

    // state information
    static ExplanationListExportOptions m_lastCopyOption;
    static ExplanationListExportOptions m_lastSaveOption;

    wxDECLARE_DYNAMIC_CLASS(ExplanationListCtrl);
    };

#endif // EXPLANATION_LISTCTRL_H
