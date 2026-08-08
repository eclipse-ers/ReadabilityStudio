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

#include "project_wizard_dlg.h"
#include "../../Wisteria-Dataviz/src/base/colorbrewer.h"
#include "../../Wisteria-Dataviz/src/ui/dialogs/getdirdlg.h"
#include "../../indexing/diacritics.h"
#include "../../results-format/project_report_format.h"
#include "../../results-format/word_collection_text_formatting.h"
#include <utility>
#include <wx/dir.h>
#include <wx/valgen.h>

wxDECLARE_APP(ReadabilityApp);

wxString ProjectWizardDlg::m_lastSelectedFolder;

class Banner final : public wxWindow
    {
  public:
    Banner(wxWindow* parent, wxWindowID id, const wxBitmapBundle& logo, wxString label)
        : wxWindow(parent, id, wxDefaultPosition, wxSize{ 300, 50 }, wxFULL_REPAINT_ON_RESIZE),
          m_logo(logo), m_label(std::move(label))
        {
        wxWindow::SetMinSize(FromDIP(wxSize{ 300, 50 }));
        wxWindow::SetBackgroundStyle(wxBG_STYLE_CUSTOM);
        Bind(wxEVT_PAINT, &Banner::OnPaint, this);
        }

    void OnPaint([[maybe_unused]] wxPaintEvent& event)
        {
        wxAutoBufferedPaintDC adc(this);
        adc.Clear();
        wxGCDC dc(adc);

        wxCoord textWidth{ 0 }, textHeight{ 0 };
        dc.GetTextExtent(m_label, &textWidth, &textHeight);

        wxBitmap logo = m_logo.GetBitmap(ScaleToContentSize(FromDIP(wxSize{ 32, 32 })));
        logo.SetScaleFactor(GetContentScaleFactor());

        const wxCoord leftBorder =
            safe_divide<int>(GetClientSize().GetWidth(), 2) -
            (safe_divide<int>(logo.GetLogicalWidth(), 2) + safe_divide<int>(textWidth, 2) +
             wxSizerFlags::GetDefaultBorder());

        dc.SetPen(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
        dc.DrawBitmap(logo, leftBorder,
                      safe_divide<int>(GetClientSize().GetHeight(), 2) -
                          safe_divide<int>(logo.GetLogicalHeight(), 2));
        dc.DrawText(m_label, leftBorder + logo.GetLogicalWidth() + 6,
                    safe_divide<int>(GetClientSize().GetHeight(), 2) -
                        safe_divide<int>(textHeight, 2));
        dc.DrawLine(safe_divide<int>(GetClientSize().GetWidth(), 10),
                    GetClientSize().GetHeight() - 1,
                    GetClientSize().GetWidth() - safe_divide<int>(GetClientSize().GetWidth(), 10),
                    GetClientSize().GetHeight() - 1);
        }

    void SetLogo(const wxBitmapBundle& logo) { m_logo = logo; }

    void SetLabel(const wxString& label) final { m_label = label; }

  private:
    [[nodiscard]]
    wxSize ScaleToContentSize(const wxSize sz) const
        {
        auto scaledSize{ sz };
        // for Retina display
        const double scaling = GetContentScaleFactor();

        scaledSize = wxSize{ static_cast<int>(std::lround(scaledSize.GetWidth() * scaling)),
                             static_cast<int>(std::lround(scaledSize.GetHeight() * scaling)) };
        return scaledSize;
        }

    wxBitmapBundle m_logo;
    wxString m_label;
    };

//-------------------------------------------------------------
ProjectWizardDlg::ProjectWizardDlg(wxWindow* parent, const ProjectType projectType,
    const wxString& path /*= wxString{}*/, wxWindowID id /*= wxID_ANY*/,
    const wxString& caption /*= _(L"New Project Wizard")*/,
    const wxPoint& pos /*= wxDefaultPosition*/, const wxSize& size /*= wxDefaultSize*/,
    long style /*= wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER*/,
    Wisteria::UI::ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode fileTruncMode /*=
        ListCtrlEx::ColumnInfo::ColumnFilePathTruncationMode::NoTruncation*/) :
    m_minDocWordCountForBatch(wxGetApp().GetAppOptions()->GetMinDocWordCountForBatch()),
    m_fromFileSelected(wxGetApp().GetAppOptions()->GetTextSource() == TextSource::FromFile),
    m_manualSelected(wxGetApp().GetAppOptions()->GetTextSource() == TextSource::EnteredText),
    m_testSelectionMethod(static_cast<int>(wxGetApp().GetAppOptions()->GetTestRecommendation())),
    m_selectedDocType(static_cast<int>(wxGetApp().GetAppOptions()->GetTestByDocumentType())),
    m_selectedIndustryType(static_cast<int>(wxGetApp().GetAppOptions()->GetTestByIndustry())),
    m_includeDolchSightWords(wxGetApp().GetAppOptions()->IsDolchSelected()),
    m_readabilityTests(wxGetApp().GetAppOptions()->GetReadabilityTests()),
    m_projectType(projectType),
    m_selectedLang(static_cast<int>(wxGetApp().GetAppOptions()->GetProjectLanguage())),
    m_fileListTruncationMode(fileTruncMode)
    {
    wxNonOwnedWindow::SetExtraStyle(GetExtraStyle() | wxWS_EX_VALIDATE_RECURSIVELY |
                                    wxWS_EX_CONTEXTHELP);
    wxDialog::Create(parent, id, caption, pos, size, style);
    // determine whether a file path or raw text was passed in
    if (!path.empty())
        {
        const FilePathResolver resolvePath(path, false);
        if (resolvePath.IsInvalidFile())
            {
            m_fromFileSelected = false;
            m_manualSelected = true;
            m_enteredText = path;
            }
        else
            {
            m_fromFileSelected = true;
            m_manualSelected = false;
            m_filePath = path;
            }
        }

    CreateControls();
    Centre();

    if (wxGetApp().GetAppOptions()->GetInvalidSentenceMethod() ==
        InvalidSentence::IncludeAsFullSentences)
        {
        SetFragmentedTextSelected();
        }
    else
        {
        SetNarrativeSelected();
        }
    SetSplitLinesSelected(wxGetApp().GetAppOptions()->IsIgnoringBlankLinesForParagraphsParser());
    // ignoring indenting doesn't make sense if each line should start a new paragraph
    SetCenteredTextSelected(wxGetApp().GetAppOptions()->GetParagraphsParsingMethod() !=
                                ParagraphParse::EachNewLineIsAParagraph &&
                            wxGetApp().GetAppOptions()->IsIgnoringIndentingForParagraphsParser());
    SetNewLinesAlwaysNewParagraphsSelected(
        wxGetApp().GetAppOptions()->GetParagraphsParsingMethod() ==
        ParagraphParse::EachNewLineIsAParagraph);

    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnGroupClick, this, ProjectWizardDlg::ID_GROUP_BUTTON);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnHelp, this, wxID_HELP);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnNavigate, this, wxID_FORWARD);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnNavigate, this, wxID_BACKWARD);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnOK, this, wxID_OK);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnFileBrowseButtonClick, this, ID_FILE_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnButtonClick, this,
         NARRATIVE_WITH_ILLUSTRATIONS_LINK_ID);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnButtonClick, this, FRAGMENTED_LINK_ID);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnButtonClick, this, CENTERED_TEXT_LINK_ID);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnAddFolderButtonClick, this,
         ID_BATCH_FOLDER_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnAddFileButtonClick, this, ID_BATCH_FILE_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnAddWebPagesButtonClick, this,
         ID_WEB_PAGES_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnAddWebPageButtonClick, this, ID_WEB_PAGE_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnAddArchiveFileButtonClick, this,
         ID_ARCHIVE_FILE_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnAddSpreadsheetFileButtonClick, this,
         ID_SPREADSHEET_FILE_BROWSE_BUTTON);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnAddToListClick, this, ID_ADD_FILE_BUTTON);
    Bind(wxEVT_BUTTON, &ProjectWizardDlg::OnDeleteFromListClick, this, ID_DELETE_FILE_BUTTON);

    Bind(wxEVT_CHECKBOX, &ProjectWizardDlg::OnRandomSampleCheck, this, ID_RANDOM_SAMPLE_CHECK);
    Bind(wxEVT_CHOICE, &ProjectWizardDlg::OnLanguageChanged, this, LANGUAGE_BUTTON);
    Bind(wxEVT_RADIOBUTTON, &ProjectWizardDlg::OnSourceRadioChange, this, ID_FROM_FILE_BUTTON);
    Bind(wxEVT_RADIOBUTTON, &ProjectWizardDlg::OnSourceRadioChange, this,
         ID_MANUALLY_ENTERED_TEXT_BUTTON);
    Bind(wxEVT_RADIOBOX, &ProjectWizardDlg::OnTestSelectionMethodChanged, this,
         TEST_SELECT_METHOD_BUTTON);

    // Preview page: these just flag that a refresh is needed.
    // The actual work only happens once the user navigates to the Preview page.
    if (GetProjectType() == ProjectType::StandardProject)
        {
        m_filePathEdit->Bind(wxEVT_TEXT, &ProjectWizardDlg::OnPreviewSourceMayHaveChanged, this);
        m_textEntryEdit->Bind(wxEVT_TEXT, &ProjectWizardDlg::OnPreviewSourceMayHaveChanged, this);
        }
    Bind(wxEVT_RADIOBUTTON, &ProjectWizardDlg::OnPreviewStructureMayHaveChanged, this,
         ID_NARRATIVE_RADIO_BUTTON);
    Bind(wxEVT_RADIOBUTTON, &ProjectWizardDlg::OnPreviewStructureMayHaveChanged, this,
         ID_NONNARRATIVE_RADIO_BUTTON);
    Bind(wxEVT_CHECKBOX, &ProjectWizardDlg::OnPreviewStructureMayHaveChanged, this,
         ID_SENTENCES_SPLIT_RADIO_BUTTON);
    Bind(wxEVT_CHECKBOX, &ProjectWizardDlg::OnPreviewStructureMayHaveChanged, this,
         ID_CENTERED_TEXT_CHECKBOX);
    Bind(wxEVT_CHECKBOX, &ProjectWizardDlg::OnPreviewStructureMayHaveChanged, this,
         ID_HARD_RETURN_CHECKBOX);

    Bind(Wisteria::UI::wxEVT_SIDEBARBOOK_PAGE_CHANGED, &ProjectWizardDlg::OnPageChange, this);

    Bind(wxEVT_HELP, &ProjectWizardDlg::OnContextHelp, this);

    Bind(
        wxEVT_CHAR_HOOK,
        [this](wxKeyEvent& event)
        {
            if (event.ControlDown() && event.GetKeyCode() == L'G')
                {
                wxRibbonButtonBarEvent dummyEvt;
                OnGroupClick(dummyEvt);
                }
            else
                {
                event.Skip(true);
                }
        },
        wxID_ANY);
    }

//-------------------------------------------------------------
void ProjectWizardDlg::CreateControls()
    {
    const int scaledNoteWidth = FromDIP(wxSize(500, 500)).GetWidth();

    const wxSize maxImageSize{
        static_cast<int>(static_cast<int>(safe_divide<int>(
                             wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_X),
                             GetContentScaleFactor())) *
                         .75),
        static_cast<int>(static_cast<int>(safe_divide<int>(
                             wxSystemSettings::GetMetric(wxSystemMetric::wxSYS_SCREEN_Y),
                             GetContentScaleFactor())) *
                         .75)
    };

    auto* mainSizer = new wxBoxSizer(wxVERTICAL);
    m_sideBarBook = new Wisteria::UI::SideBarBook(this, wxID_ANY);
    mainSizer->Add(m_sideBarBook, wxSizerFlags{ 1 }.Expand().Border());

    m_sideBarBook->GetImageList().push_back(wxArtProvider::GetBitmapBundle(L"ID_DOCUMENT"));
    m_sideBarBook->GetImageList().push_back(
        wxGetApp().GetResourceManager().GetSVG(L"ribbon/document-structure.svg"));
    m_sideBarBook->GetImageList().push_back(
        wxGetApp().GetResourceManager().GetSVG(L"tests/flesch-test.svg"));
    m_sideBarBook->GetImageList().push_back(
        wxGetApp().GetResourceManager().GetSVG(L"ribbon/preview.svg"));

    // document page
    if (GetProjectType() == ProjectType::StandardProject)
        {
        auto* page =
            new wxPanel(m_sideBarBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
        auto* pageSizer = new wxBoxSizer(wxVERTICAL);
        page->SetSizer(pageSizer);
        m_sideBarBook->AddPage(page, _(L"Document"), wxID_ANY, true, 0);

        // The options
        auto* optionsSizer = new wxBoxSizer(wxVERTICAL);

        auto* banner =
            new Banner(page, wxID_ANY, wxArtProvider::GetBitmapBundle(L"ID_DOCUMENT", wxART_BUTTON),
                       _(L"Select Document"));
        optionsSizer->Add(banner, wxSizerFlags{}.Expand().Border(wxBOTTOM));

        // select the language
        // (do not sort, connected to an enumeration)
        wxArrayString langs;
        langs.Add(_(L"English"));
        langs.Add(_(L"Spanish"));
        langs.Add(_(L"German"));
        auto* langSizer = new wxBoxSizer(wxHORIZONTAL);
        langSizer->Add(new wxStaticText(page, wxID_STATIC, _(L"Document language:")), 0,
                       wxRIGHT | wxALIGN_CENTRE, wxSizerFlags::GetDefaultBorder());
        langSizer->Add(new wxChoice(page, LANGUAGE_BUTTON, wxDefaultPosition, wxDefaultSize, langs,
                                    0, wxGenericValidator(&m_selectedLang)),
                       0, wxALIGN_LEFT | wxALL, wxSizerFlags::GetDefaultBorder());
        optionsSizer->Add(langSizer);

        // file path
        optionsSizer->Add(new wxRadioButton(
            page, ID_FROM_FILE_BUTTON, _(L"&Read text from a file or webpage:"), wxDefaultPosition,
            wxDefaultSize, wxRB_GROUP, wxGenericValidator(&m_fromFileSelected)));
        optionsSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

        auto* fileBrowseBoxSizer = new wxBoxSizer(wxHORIZONTAL);
        optionsSizer->Add(fileBrowseBoxSizer, wxSizerFlags{}.Expand().Border(
                                                  wxLEFT, wxSizerFlags::GetDefaultBorder() * 3));

        m_filePathEdit =
            new wxTextCtrl(page, wxID_ANY, wxString{}, wxDefaultPosition, wxDefaultSize,
                           wxTE_RICH2 | wxBORDER_THEME, wxGenericValidator(&m_filePath));
        m_filePathEdit->AutoCompleteFileNames();
        fileBrowseBoxSizer->Add(m_filePathEdit, wxSizerFlags{ 1 }.Expand());

        m_fileBrowseButton =
            new wxBitmapButton(page, ID_FILE_BROWSE_BUTTON,
                               wxArtProvider::GetBitmapBundle(wxART_FILE_OPEN, wxART_BUTTON));
        m_fileBrowseButton->SetHelpText(
            _(L"Click this button to browse for the file that you want to analyze."));
        fileBrowseBoxSizer->Add(m_fileBrowseButton, wxSizerFlags{}.Border(wxRIGHT));
        optionsSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

        // Manual text entry
        optionsSizer->Add(new wxRadioButton(
            page, ID_MANUALLY_ENTERED_TEXT_BUTTON, _(L"Manually enter text:"), wxDefaultPosition,
            wxDefaultSize, 0, wxGenericValidator(&m_manualSelected)));

        m_textEntryEdit =
            new wxTextCtrl(page, wxID_ANY, wxString{}, wxDefaultPosition, wxDefaultSize,
                           wxTE_MULTILINE | wxTE_RICH2, wxGenericValidator(&m_enteredText));
        optionsSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
        auto* editBoxSizer = new wxBoxSizer(wxHORIZONTAL);
        editBoxSizer->Add(m_textEntryEdit, wxSizerFlags{ 1 }.Expand().Border(
                                               wxLEFT, wxSizerFlags::GetDefaultBorder() * 3));
        optionsSizer->Add(editBoxSizer, wxSizerFlags{ 1 }.Expand().Border(wxRIGHT));

        if (m_fromFileSelected)
            {
            m_textEntryEdit->Disable();
            }
        else
            {
            m_filePathEdit->Disable();
            m_fileBrowseButton->Disable();
            }

        auto* noteLabel = new wxStaticText(
            page, wxID_STATIC,
            _(L"The selected language will affect syllable counting and determine which "
              "tests and grammar features will be made available."));
        optionsSizer->Add(noteLabel, 0, wxALIGN_LEFT | wxALL, wxSizerFlags::GetDefaultBorder());

        pageSizer->Add(optionsSizer, wxSizerFlags{ 1 }.Expand().Border());
        }
    else // Batch project
        {
        auto* page =
            new wxPanel(m_sideBarBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
        auto* pageSizer = new wxBoxSizer(wxVERTICAL);
        page->SetSizer(pageSizer);
        m_sideBarBook->AddPage(page, _(L"Documents"), wxID_ANY, true, 0);

        // The options
        auto* optionsSizer = new wxBoxSizer(wxVERTICAL);

        auto* banner =
            new Banner(page, wxID_ANY, wxArtProvider::GetBitmapBundle(L"ID_DOCUMENT", wxART_BUTTON),
                       _(L"Select Documents"));
        optionsSizer->Add(banner, wxSizerFlags{}.Expand().Border(wxBOTTOM));

        // select the language
        wxArrayString langs;
        langs.Add(_(L"English"));
        langs.Add(_(L"Spanish"));
        langs.Add(_(L"German"));
        auto* langSizer = new wxBoxSizer(wxHORIZONTAL);
        langSizer->Add(new wxStaticText(page, wxID_STATIC, _(L"Documents' language:")), 0,
                       wxRIGHT | wxALIGN_CENTRE, wxSizerFlags::GetDefaultBorder());
        langSizer->Add(new wxChoice(page, LANGUAGE_BUTTON, wxDefaultPosition, wxDefaultSize, langs,
                                    0, wxGenericValidator(&m_selectedLang)),
                       0, wxALIGN_LEFT | wxALL, wxSizerFlags::GetDefaultBorder());
        optionsSizer->Add(langSizer);

        auto* buttonsSizer = new wxGridSizer(
            4, wxSize(wxSizerFlags::GetDefaultBorder(), wxSizerFlags::GetDefaultBorder()));

        auto* button = new wxButton(page, ID_BATCH_FOLDER_BROWSE_BUTTON, _(L"&Add folder..."));
        button->SetBitmap(wxArtProvider::GetBitmapBundle(wxART_FILE_OPEN, wxART_BUTTON));
        buttonsSizer->Add(button, wxSizerFlags{}.Align(wxALIGN_LEFT).Expand());

        button = new wxButton(page, ID_BATCH_FILE_BROWSE_BUTTON, _(L"&Add files..."));
        button->SetBitmap(wxArtProvider::GetBitmapBundle(L"ID_DOCUMENTS", wxART_BUTTON));
        buttonsSizer->Add(button, wxSizerFlags{}.Align(wxALIGN_LEFT).Expand());

        button = new wxButton(page, ID_ARCHIVE_FILE_BROWSE_BUTTON, _(L"&Add archive..."));
        button->SetBitmap(wxArtProvider::GetBitmapBundle(L"ID_ARCHIVE", wxART_BUTTON));
        buttonsSizer->Add(button, wxSizerFlags{}.Align(wxALIGN_LEFT).Expand());

        button = new wxButton(page, ID_SPREADSHEET_FILE_BROWSE_BUTTON, _(L"&Add spreadsheet..."));
        button->SetBitmap(wxArtProvider::GetBitmapBundle(L"ID_SPREADSHEET", wxART_BUTTON));
        buttonsSizer->Add(button, wxSizerFlags{}.Align(wxALIGN_LEFT).Expand());

        button = new wxButton(page, ID_WEB_PAGES_BROWSE_BUTTON, _(L"&Add web pages..."));
        button->SetBitmap(wxArtProvider::GetBitmapBundle(L"ID_WEB_EXPORT", wxART_BUTTON));
        buttonsSizer->Add(button, wxSizerFlags{}.Align(wxALIGN_LEFT).Expand());

        button = new wxButton(page, ID_WEB_PAGE_BROWSE_BUTTON, _(L"&Add web page..."));
        button->SetBitmap(wxArtProvider::GetBitmapBundle(L"ID_WEB_EXPORT", wxART_BUTTON));
        buttonsSizer->Add(button, wxSizerFlags{}.Align(wxALIGN_LEFT).Expand());

        optionsSizer->Add(buttonsSizer);

        // add and remove buttons for file grid
        auto* filesButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
        auto* addFileButton = new wxBitmapButton(
            page, ID_ADD_FILE_BUTTON, wxArtProvider::GetBitmapBundle(L"ID_ADD", wxART_BUTTON));
        addFileButton->SetToolTip(_(L"Add a document"));
        filesButtonsSizer->Add(addFileButton);

        auto* deleteFileButton =
            new wxBitmapButton(page, ID_DELETE_FILE_BUTTON,
                               wxArtProvider::GetBitmapBundle(wxART_DELETE, wxART_BUTTON));
        deleteFileButton->SetToolTip(_(L"Remove selected document"));
        filesButtonsSizer->Add(deleteFileButton);

        auto* groupButton = new wxBitmapButton(
            page, ID_GROUP_BUTTON, wxArtProvider::GetBitmapBundle(L"ID_GROUP", wxART_BUTTON));
        groupButton->SetToolTip(_(L"Group selected documents"));
        filesButtonsSizer->Add(groupButton);

        optionsSizer->Add(filesButtonsSizer, wxSizerFlags{}.Right());

        if (!GetFilePath().empty())
            {
            const FilePathResolver rp(GetFilePath(), false);
            // if page is created with a default folder or file then add it to the list
            if (wxFileName::DirExists(GetFilePath()))
                {
                const wxBusyCursor wait;
                wxArrayString files;
                wxDir::GetAllFiles(GetFilePath(), &files, wxString{}, wxDIR_FILES | wxDIR_DIRS);
                files = FilterFiles(files, ReadabilityAppOptions::ALL_DOCUMENTS_WILDCARD.data());

                m_fileData->SetSize(files.GetCount(), 2);
                m_fileData->SetValues(files);
                }
            else if (FilePathResolver::IsSpreadsheet(GetFilePath()) &&
                     wxFileName::FileExists(GetFilePath()))
                {
                LoadSpreadsheet(GetFilePath());
                }
            else if (FilePathResolver::IsArchive(GetFilePath()) &&
                     wxFileName::FileExists(GetFilePath()))
                {
                LoadArchive(GetFilePath());
                }
            else
                {
                m_fileData->SetSize(1, 2);
                m_fileData->SetItemText(
                    0, 0, GetFilePath(),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            }
        else
            {
            m_fileData->SetSize(0, 2);
            }
        m_fileList = new Wisteria::UI::ListCtrlEx(
            page, wxID_ANY, wxDefaultPosition, FromDIP(wxSize{ 200, 150 }),
            wxLC_VIRTUAL | wxLC_EDIT_LABELS | wxLC_REPORT | wxLC_ALIGN_LEFT);
        m_fileList->EnableGridLines();
        m_fileList->EnableItemDeletion();
        m_fileList->InsertColumn(0, _(L"Files"));
        m_fileList->InsertColumn(1, _(L"Labels"));
        m_fileList->SetColumnEditable(0);
        m_fileList->SetColumnEditable(1);
        m_fileList->SetColumnFilePathTruncationMode(0, m_fileListTruncationMode);
        m_fileList->SetVirtualDataProvider(m_fileData);
        m_fileList->SetVirtualDataSize(m_fileData->GetItemCount());
        optionsSizer->Add(m_fileList, wxSizerFlags{ 2 }.Expand().Border(wxLEFT | wxBOTTOM));

        // min word count
        auto* minDocSizeBoxSizer = new wxBoxSizer(wxHORIZONTAL);

        auto* minDocSizeLabel =
            new wxStaticText(page, ID_MIN_WORDS_LABEL, _(L"Minimum document word count:"),
                             wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
        minDocSizeBoxSizer->Add(minDocSizeLabel, wxSizerFlags{}.CenterVertical().Border(wxRIGHT));

        auto* minDocWordCountForBatchSpinCtrl = new wxSpinCtrl(
            page, wxID_ANY, std::to_wstring(m_minDocWordCountForBatch), wxDefaultPosition,
            wxDefaultSize, wxSP_ARROW_KEYS, 1, std::numeric_limits<int>::max(), 0);
        minDocWordCountForBatchSpinCtrl->SetValidator(
            wxGenericValidator(&m_minDocWordCountForBatch));
        minDocSizeBoxSizer->Add(minDocWordCountForBatchSpinCtrl,
                                wxSizerFlags{}.CenterVertical().Border());

        // random sampling
        auto* randomOptionsSizer = new wxBoxSizer(wxHORIZONTAL);
        m_isRandomSampling =
            new wxCheckBox(page, ID_RANDOM_SAMPLE_CHECK,
                           /* xgettext:no-c-format */ _(L"% of documents to randomly sample:"));
        m_isRandomSampling->SetValue(wxGetApp().GetAppOptions()->IsRandomSampling());

        m_randPercentageCtrl = new wxSpinCtrl(
            page, ID_RANDOM_SAMPLE_SPIN,
            std::to_wstring(wxGetApp().GetAppOptions()->GetBatchRandomSamplingSize()));
        m_randPercentageCtrl->SetRange(1, 100);
        m_randPercentageCtrl->Enable(wxGetApp().GetAppOptions()->IsRandomSampling());

        randomOptionsSizer->Add(m_isRandomSampling,
                                wxSizerFlags{}.Border(wxRIGHT).CenterVertical());
        randomOptionsSizer->Add(m_randPercentageCtrl);

        pageSizer->Add(optionsSizer, wxSizerFlags{ 1 }.Expand().Border());
        pageSizer->Add(minDocSizeBoxSizer, wxSizerFlags{}.Expand().Border(wxLEFT));
        pageSizer->Add(randomOptionsSizer, wxSizerFlags{}.Expand().Border());
        }
        // document structure
        {
        Wisteria::GraphItems::Label imageLabel(Wisteria::GraphItems::GraphItemInfo()
                                                   .DPIScaling(GetDPIScaleFactor())
                                                   .Pen(wxPen(wxColour(L"#BCE8F1"), 2))
                                                   .Padding(4, 4, 4, 4));
        imageLabel.SetFontBackgroundColor(wxColour(L"#D9EDF7"));
        imageLabel.SetFontColor(wxColour(L"#31708F"));
        imageLabel.SetTextAlignment(Wisteria::TextAlignment::FlushLeft);
        imageLabel.SetAnchoring(Wisteria::Anchoring::TopRightCorner);
        imageLabel.SetBoxCorners(Wisteria::BoxCorners::Rounded);

        if (wxGetApp().GetAppOptions()->GetInvalidSentenceMethod() ==
            InvalidSentence::IncludeAsFullSentences)
            {
            SetFragmentedTextSelected();
            }
        else
            {
            SetNarrativeSelected();
            }
        SetSplitLinesSelected(
            wxGetApp().GetAppOptions()->IsIgnoringBlankLinesForParagraphsParser());
        SetCenteredTextSelected(
            wxGetApp().GetAppOptions()->IsIgnoringIndentingForParagraphsParser());

        auto* page =
            new wxPanel(m_sideBarBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
        auto* pageSizer = new wxBoxSizer(wxVERTICAL);
        page->SetSizer(pageSizer);
        m_sideBarBook->AddPage(page, _(L"Document Structure"), wxID_ANY, false, 1);

        // the options
        auto* optionsSizer = new wxBoxSizer(wxVERTICAL);

        auto* banner =
            new Banner(page, wxID_ANY,
                       wxGetApp().GetResourceManager().GetSVG(L"ribbon/document-structure.svg"),
                       _(L"Specify Document Structure"));
        optionsSizer->Add(banner, wxSizerFlags{}.Expand().Border(wxBOTTOM));

        auto* docTypeSizer = new wxStaticBoxSizer(wxVERTICAL, page, _(L"Composition"));
        docTypeSizer->GetStaticBox()->SetId(ID_COMPOSITION_BOX);
        auto* docLayoutSizer = new wxStaticBoxSizer(wxVERTICAL, page, _(L"Layout"));
        docLayoutSizer->GetStaticBox()->SetId(ID_LAYOUT_BOX);
        optionsSizer->Add(docTypeSizer, wxSizerFlags{}.Expand().Border(wxLEFT | wxRIGHT));
        optionsSizer->Add(docLayoutSizer, wxSizerFlags{}.Expand().Border(wxLEFT | wxRIGHT));

            // narrative text
            {
            auto* narrativeSizer = new wxBoxSizer(wxHORIZONTAL);
            auto* narrativeLabelsSizer = new wxBoxSizer(wxVERTICAL);
            auto* narrativeRadioButton =
                new wxRadioButton(docTypeSizer->GetStaticBox(), ID_NARRATIVE_RADIO_BUTTON,
                                  _(L"&Narrative text"), wxDefaultPosition, wxDefaultSize,
                                  wxRB_GROUP, wxGenericValidator(&m_narrativeSelected));
            narrativeLabelsSizer->Add(narrativeRadioButton);
            auto* noteLabel = new wxStaticText(
                docTypeSizer->GetStaticBox(), ID_NARRATIVE_LABEL,
                _(L"Document contains flowing sentences and paragraphs. "
                  "Items such as headers and list items are not part of the narrative text and "
                  "should be ignored."),
                wxDefaultPosition, wxDefaultSize, 0);
            noteLabel->Wrap(scaledNoteWidth);
            narrativeLabelsSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            narrativeLabelsSizer->Add(noteLabel, 1, wxLEFT, wxSizerFlags::GetDefaultBorder() * 3);
            narrativeLabelsSizer->SetMinSize(scaledNoteWidth, -1);
            narrativeSizer->Add(narrativeLabelsSizer, wxSizerFlags{}.Center());

            narrativeSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            wxBitmap previewImage = wxGetApp().GetScaledImage(L"wizard/narrative-text.png",
                                                              wxBITMAP_TYPE_PNG, maxImageSize);
            wxMemoryDC memDc(previewImage);
            // draw the label
            imageLabel.SetText(
                _(L"Narrative text is analyzed, while headers such as these are ignored."));
            imageLabel.SplitTextToFitBoundingBox(
                memDc,
                wxSize(previewImage.GetLogicalWidth() * .70f, previewImage.GetLogicalHeight()));
            imageLabel.SetAnchorPoint(
                wxPoint(previewImage.GetLogicalWidth() - wxSizerFlags::GetDefaultBorder(),
                        wxSizerFlags::GetDefaultBorder()));
            imageLabel.Draw(memDc);
            memDc.SelectObject(wxNullBitmap);
            narrativeSizer->AddStretchSpacer();
            narrativeSizer->Add(
                new Wisteria::UI::Thumbnail(docTypeSizer->GetStaticBox(), previewImage));
            docTypeSizer->Add(narrativeSizer, wxSizerFlags{}.Expand().Border());
            }
            // non-narrative text
            {
            auto* sparseSizer = new wxBoxSizer(wxHORIZONTAL);
            auto* sparseLabelsSizer = new wxBoxSizer(wxVERTICAL);
            auto* sparseRadioButton =
                new wxRadioButton(docTypeSizer->GetStaticBox(), ID_NONNARRATIVE_RADIO_BUTTON,
                                  _(L"Non-narrative, &fragmented text"), wxDefaultPosition,
                                  wxDefaultSize, 0, wxGenericValidator(&m_fragmentedTextSelected));
            sparseLabelsSizer->Add(sparseRadioButton);
            auto* noteLabel = new wxStaticText(
                docTypeSizer->GetStaticBox(), ID_NONNARRATIVE_LABEL,
                _(L"Instead of the standard sentence and paragraph structure, the document mostly "
                  "consists of list items and terse sentence fragments. "
                  "NOTE: this option will disable text exclusion."),
                wxDefaultPosition, wxDefaultSize, 0);
            noteLabel->Wrap(scaledNoteWidth);
            auto* noteSizer = new wxBoxSizer(wxHORIZONTAL);
            noteSizer->Add(noteLabel, 1, wxLEFT, wxSizerFlags::GetDefaultBorder() * 3);

            auto* moreInfoButton =
                new wxButton(docTypeSizer->GetStaticBox(), FRAGMENTED_LINK_ID, wxString{},
                             wxDefaultPosition, wxDefaultSize, wxBU_NOTEXT | wxBORDER_NONE);
            moreInfoButton->SetBitmap(
                wxGetApp().GetResourceManager().GetSVG(L"ribbon/help-button.svg"));
            if (wxSystemSettings::GetAppearance().IsDark())
                {
                moreInfoButton->SetBackgroundColour(GetBackgroundColour());
                }
            noteSizer->Add(moreInfoButton, wxSizerFlags{}.Border(wxLEFT));
            noteSizer->SetMinSize(scaledNoteWidth, -1);

            sparseLabelsSizer->Add(noteSizer);

            sparseSizer->Add(sparseLabelsSizer, wxSizerFlags{}.Center());

            sparseSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            wxBitmap previewImage = wxGetApp().GetScaledImage(L"wizard/sparse-text.png",
                                                              wxBITMAP_TYPE_PNG, maxImageSize);
            wxMemoryDC memDc(previewImage);
            // draw the label
            imageLabel.SetText(
                _(L"Document contains few sentences and mostly consists of terse blocks of text."));
            imageLabel.SplitTextToFitBoundingBox(
                memDc, wxSize(memDc.GetSize().GetWidth() * .60f, memDc.GetSize().GetHeight()));
            imageLabel.SetAnchorPoint(
                wxPoint(memDc.GetSize().GetWidth() - wxSizerFlags::GetDefaultBorder(),
                        wxSizerFlags::GetDefaultBorder()));
            imageLabel.Draw(memDc);
            memDc.SelectObject(wxNullBitmap);
            sparseSizer->AddStretchSpacer();
            sparseSizer->Add(
                new Wisteria::UI::Thumbnail(docTypeSizer->GetStaticBox(), previewImage));
            docTypeSizer->Add(sparseSizer, wxSizerFlags{}.Expand().Border());
            }
            // text with "broken lines"
            {
            auto* narrativeSizer = new wxBoxSizer(wxHORIZONTAL);
            auto* narrativeLabelsSizer = new wxBoxSizer(wxVERTICAL);
            auto* narrativeButton = new wxCheckBox(
                docLayoutSizer->GetStaticBox(), ID_SENTENCES_SPLIT_RADIO_BUTTON,
                _(L"&Sentences are split by illustrations or extra spacing"), wxDefaultPosition,
                wxDefaultSize, wxCHK_2STATE, wxGenericValidator(&m_splitLinesSelected));
            narrativeLabelsSizer->Add(narrativeButton);
            narrativeLabelsSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

            auto* noteLabel = new wxStaticText(
                docLayoutSizer->GetStaticBox(), ID_SENTENCES_SPLIT_LABEL,
                _(L"The document's sentences may be wrapped around illustrations or contain "
                  "empty lines between them. This is common for children's picture books."),
                wxDefaultPosition, wxDefaultSize, 0);
            noteLabel->Wrap(scaledNoteWidth);
            auto* noteSizer = new wxBoxSizer(wxHORIZONTAL);
            noteSizer->Add(noteLabel, 1, wxLEFT, wxSizerFlags::GetDefaultBorder() * 3);
            noteSizer->SetMinSize(scaledNoteWidth, -1);

            auto* moreInfoButton = new wxButton(
                docLayoutSizer->GetStaticBox(), NARRATIVE_WITH_ILLUSTRATIONS_LINK_ID, wxString{},
                wxDefaultPosition, wxDefaultSize, wxBU_NOTEXT | wxBORDER_NONE);
            moreInfoButton->SetBitmap(
                wxGetApp().GetResourceManager().GetSVG(L"ribbon/help-button.svg"));
            if (wxSystemSettings::GetAppearance().IsDark())
                {
                moreInfoButton->SetBackgroundColour(GetBackgroundColour());
                }
            noteSizer->Add(moreInfoButton, wxSizerFlags{}.Border(wxLEFT));

            narrativeLabelsSizer->Add(noteSizer);

            narrativeSizer->Add(narrativeLabelsSizer, wxSizerFlags{}.Center());

            narrativeSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            wxBitmap previewImage = wxGetApp().GetScaledImage(L"wizard/narrative-illustrated.png",
                                                              wxBITMAP_TYPE_PNG, maxImageSize);
            wxMemoryDC memDc(previewImage);
            // draw the label
            imageLabel.SetText(
                _(L"Sentences split into fragments by illustrations will be chained together."));
            imageLabel.SplitTextToFitBoundingBox(
                memDc, wxSize(memDc.GetSize().GetWidth() * .33f, memDc.GetSize().GetHeight()));
            imageLabel.SetAnchorPoint(
                wxPoint(memDc.GetSize().GetWidth() - wxSizerFlags::GetDefaultBorder(),
                        wxSizerFlags::GetDefaultBorder()));
            imageLabel.Draw(memDc);
            memDc.SelectObject(wxNullBitmap);
            narrativeSizer->AddStretchSpacer();
            narrativeSizer->Add(
                new Wisteria::UI::Thumbnail(docLayoutSizer->GetStaticBox(), previewImage));
            narrativeSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

            previewImage = wxGetApp().GetScaledImage(L"wizard/narrative-with-lines.png",
                                                     wxBITMAP_TYPE_PNG, maxImageSize);
            wxMemoryDC memDc2(previewImage);
            // draw the label
            imageLabel.SetText(
                _(L"Split sentences with extra lines between them will be chained together."));
            imageLabel.SplitTextToFitBoundingBox(
                memDc2, wxSize(memDc2.GetSize().GetWidth() * .50f, memDc2.GetSize().GetHeight()));
            imageLabel.SetAnchorPoint(
                wxPoint(memDc2.GetSize().GetWidth() - wxSizerFlags::GetDefaultBorder(),
                        wxSizerFlags::GetDefaultBorder()));
            imageLabel.Draw(memDc2);
            memDc2.SelectObject(wxNullBitmap);
            narrativeSizer->Add(
                new Wisteria::UI::Thumbnail(docLayoutSizer->GetStaticBox(), previewImage));
            docLayoutSizer->Add(narrativeSizer, wxSizerFlags{}.Expand().Border());
            }
            // centered text
            {
            auto* centeredSizer = new wxBoxSizer(wxHORIZONTAL);
            auto* centeredLabelsSizer = new wxBoxSizer(wxVERTICAL);
            auto* centeredButton =
                new wxCheckBox(docLayoutSizer->GetStaticBox(), ID_CENTERED_TEXT_CHECKBOX,
                               _(L"Centered/left-aligned text"), wxDefaultPosition, wxDefaultSize,
                               wxCHK_2STATE, wxGenericValidator(&m_centeredText));
            centeredLabelsSizer->Add(centeredButton);
            auto* noteLabel = new wxStaticText(
                docLayoutSizer->GetStaticBox(), wxID_STATIC,
                _(L"Text is indented to be centered or left-aligned on the page. "
                  "Selecting this option will instruct the program to ignore indenting when "
                  "deducing where paragraphs begin and end."),
                wxDefaultPosition, wxDefaultSize, 0);
            noteLabel->Wrap(scaledNoteWidth);
            auto* noteSizer = new wxBoxSizer(wxHORIZONTAL);
            noteSizer->Add(noteLabel, 1, wxLEFT, wxSizerFlags::GetDefaultBorder() * 3);

            auto* moreInfoButton =
                new wxButton(docLayoutSizer->GetStaticBox(), CENTERED_TEXT_LINK_ID, wxString{},
                             wxDefaultPosition, wxDefaultSize, wxBU_NOTEXT | wxBORDER_NONE);
            moreInfoButton->SetBitmap(
                wxGetApp().GetResourceManager().GetSVG(L"ribbon/help-button.svg"));
            if (wxSystemSettings::GetAppearance().IsDark())
                {
                moreInfoButton->SetBackgroundColour(GetBackgroundColour());
                }
            noteSizer->Add(moreInfoButton, 0, wxLEFT, wxSizerFlags::GetDefaultBorder() * 3);
            noteSizer->SetMinSize(scaledNoteWidth, -1);

            centeredLabelsSizer->Add(noteSizer, wxSizerFlags{}.Center());

            centeredSizer->Add(centeredLabelsSizer, wxSizerFlags{}.Center());

            wxBitmap previewImage = wxGetApp().GetScaledImage(L"wizard/centered-text.png",
                                                              wxBITMAP_TYPE_PNG, maxImageSize);
            wxMemoryDC memDc(previewImage);
            // draw the label
            imageLabel.SetText(_(L"Text is left-aligned to be centered on the page."));
            imageLabel.SetAnchorPoint(
                wxPoint(memDc.GetSize().GetWidth() - wxSizerFlags::GetDefaultBorder(),
                        wxSizerFlags::GetDefaultBorder()));
            imageLabel.Draw(memDc);
            memDc.SelectObject(wxNullBitmap);
            centeredSizer->AddStretchSpacer();
            centeredSizer->Add(
                new Wisteria::UI::Thumbnail(docLayoutSizer->GetStaticBox(), previewImage));
            docLayoutSizer->Add(centeredSizer, wxSizerFlags{}.Expand().Border());
            }
            // new lines are always new paragraphs (overrides center text option above)
            {
            auto* wrappedSizer = new wxBoxSizer(wxHORIZONTAL);
            auto* wrappedLabelsSizer = new wxBoxSizer(wxVERTICAL);
            auto* wrappedButton = new wxCheckBox(
                docLayoutSizer->GetStaticBox(), ID_HARD_RETURN_CHECKBOX,
                _(L"Line ends (i.e., hard returns) mark the start of a new paragraph"),
                wxDefaultPosition, wxDefaultSize, wxCHK_2STATE,
                wxGenericValidator(&m_newLinesAlwaysNewParagraphs));
            wrappedLabelsSizer->Add(wrappedButton);
            auto* noteLabel = new wxStaticText(
                docLayoutSizer->GetStaticBox(), ID_HARD_RETURN_LABEL,
                _(L"Hard returns in the text always force the start of a new paragraph. "
                  "Selecting this option will instruct the program to treat all line ends as the "
                  "end of the sentence and paragraph, "
                  "regardless of whether it ends with a period."),
                wxDefaultPosition, wxDefaultSize, 0);
            noteLabel->Wrap(scaledNoteWidth);
            wrappedLabelsSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
            wrappedLabelsSizer->Add(noteLabel, 1, wxLEFT, wxSizerFlags::GetDefaultBorder() * 3);
            wrappedLabelsSizer->SetMinSize(scaledNoteWidth, -1);
            wrappedSizer->Add(wrappedLabelsSizer, wxSizerFlags{}.Center());

            wrappedSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

            wxBitmap previewImage = wxGetApp().GetScaledImage(L"wizard/hard-returns.png",
                                                              wxBITMAP_TYPE_PNG, maxImageSize);
            wxMemoryDC memDc(previewImage);
            // draw the labels
            imageLabel.SetText(_(L"Hard returns separate these lines into different paragraphs. "
                                 "This prevents them from being combined into one sentence."));
            imageLabel.SplitTextToFitBoundingBox(
                memDc, wxSize(memDc.GetSize().GetWidth() * math_constants::half,
                              memDc.GetSize().GetHeight()));
            imageLabel.SetAnchorPoint(
                wxPoint(memDc.GetSize().GetWidth() - wxSizerFlags::GetDefaultBorder(),
                        wxSizerFlags::GetDefaultBorder()));
            imageLabel.Draw(memDc);
            memDc.SelectObject(wxNullBitmap);
            wrappedSizer->AddStretchSpacer();
            wrappedSizer->Add(
                new Wisteria::UI::Thumbnail(docLayoutSizer->GetStaticBox(), previewImage));
            wrappedSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());

            previewImage = wxGetApp().GetScaledImage(L"wizard/line-ends-are-new-paragraphs.png",
                                                     wxBITMAP_TYPE_PNG, maxImageSize);
            wxMemoryDC memDc2(previewImage);
            // draw the labels
            imageLabel.SetText(
                _(L"Line ends always force the start of a new sentence and paragraph."));
            imageLabel.SetAnchorPoint(
                wxPoint(memDc2.GetSize().GetWidth() - wxSizerFlags::GetDefaultBorder(),
                        wxSizerFlags::GetDefaultBorder()));
            imageLabel.Draw(memDc2);
            memDc2.SelectObject(wxNullBitmap);
            wrappedSizer->Add(
                new Wisteria::UI::Thumbnail(docLayoutSizer->GetStaticBox(), previewImage));
            docLayoutSizer->Add(wrappedSizer, wxSizerFlags{}.Expand().Border());
            }
        pageSizer->Add(optionsSizer, wxSizerFlags{ 1 }.Expand().Border());
        }
        // test selection
        {
        auto* page =
            new wxPanel(m_sideBarBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
        auto* pageSizer = new wxBoxSizer(wxVERTICAL);
        page->SetSizer(pageSizer);
        m_sideBarBook->AddPage(page, _(L"Test Selection"), wxID_ANY, false, 2);

        // The options
        auto* optionsSizer = new wxBoxSizer(wxVERTICAL);

        auto* banner = new Banner(page, wxID_ANY,
                                  wxGetApp().GetResourceManager().GetSVG(L"tests/flesch-test.svg"),
                                  _(L"Select Readability Tests"));
        optionsSizer->Add(banner, wxSizerFlags{}.Expand().Border(wxBOTTOM));

        wxArrayString options;
        options.Add(_(L"Recommend tests based on &document type"));
        options.Add(_(L"Recommend tests based on &industry or field"));
        options.Add(_(L"&Manually select tests"));
        options.Add(_(L"Use a test &bundle"));
        optionsSizer->Add(new wxRadioBox(page, TEST_SELECT_METHOD_BUTTON,
                                         _(L"Choose how to select tests:"), wxDefaultPosition,
                                         wxDefaultSize, options, 0, wxRA_SPECIFY_ROWS,
                                         wxGenericValidator(&m_testSelectionMethod)),
                          wxSizerFlags{}.Border(wxLEFT));

        m_testTypesSizer = new wxBoxSizer(wxVERTICAL);
        // document types
        wxArrayString docTypes;
        docTypes.Add(_(L"&General document (textbook, report, correspondence)"));
        docTypes.Add(_(L"&Technical document or form (application, résumé/CV, instructions, "
                       "manual, detailed report)"));
        docTypes.Add(_(L"Non-narrative &form with fragmented text (brochure, menu, quiz). "
                       "NOTE: this option will disable text exclusion."));
        docTypes.Add(_(L"&Literature (young adult and adult)"));
        docTypes.Add(_(L"&Children's literature"));
        m_docTypeRadioBox = new wxRadioBox(
            page, ID_DOC_TYPE_RADIO_BOX, _(L"Select the type of document that you are analyzing:"),
            wxDefaultPosition, wxDefaultSize, docTypes, 0, wxRA_SPECIFY_ROWS,
            wxGenericValidator(&m_selectedDocType));
        m_testTypesSizer->Add(m_docTypeRadioBox, wxSizerFlags{}.Border(wxLEFT));

        // industry type
        wxArrayString industryTypes;
        industryTypes.Add(_(L"Children's &publishing (literature, textbooks, magazines)"));
        industryTypes.Add(_(L"&Young adult and adult publishing (literature, textbooks, "
                            "magazine/newspaper articles)"));
        industryTypes.Add(_(L"&Children's healthcare"));
        industryTypes.Add(_(L"&Adult healthcare"));
        industryTypes.Add(_(L"&Military and government"));
        industryTypes.Add(_(L"&Second Language (ESL) Education"));
        industryTypes.Add(_(L"&Broadcasting"));
        m_industryTypeRadioBox =
            new wxRadioBox(page, ID_INDUSTRY_RADIO_BOX,
                           _(L"Select the type of industry that this document belongs to:"),
                           wxDefaultPosition, wxDefaultSize, industryTypes, 0, wxRA_SPECIFY_ROWS,
                           wxGenericValidator(&m_selectedIndustryType));
        m_testTypesSizer->Add(m_industryTypeRadioBox, wxSizerFlags{}.Border(wxLEFT));

        // standard tests
        m_testsCheckListBox =
            new wxCheckListBox(page, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, nullptr,
                               wxLB_EXTENDED | wxLB_NEEDED_SB | wxLB_HSCROLL | wxBORDER_THEME,
                               wxGenericValidator(&m_selectedTests));
        m_testsSizer = new wxFlexGridSizer(3, 2, wxSizerFlags::GetDefaultBorder(),
                                           wxSizerFlags::GetDefaultBorder());
        m_testsSizer->Add(new wxStaticText(page, wxID_STATIC, _(L"Standard tests:")));
        m_testsSizer->Add(new wxStaticText(
            page, wxID_STATIC,
            !BaseProject::m_custom_word_tests.empty() ? _(L"Custom tests:") : wxString{}));
        m_testsSizer->AddGrowableRow(1, 1);
        m_testsSizer->Add(m_testsCheckListBox, wxSizerFlags{}.Expand());
        // custom test
        if (!BaseProject::m_custom_word_tests.empty())
            {
            wxArrayString customTestNames; // NOLINT(misc-const-correctness)
            for (const auto& customTest : BaseProject::m_custom_word_tests)
                {
                customTestNames.Add(customTest.get_name().c_str());
                }
            // go through the list of test IDs that were checked the last time this wizard was used
            for (const auto& includedCustomTest :
                 wxGetApp().GetAppOptions()->GetIncludedCustomTests())
                {
                // find the test in the global list of tests, searching by test id
                auto testIter =
                    std::find(BaseProject::m_custom_word_tests.begin(),
                              BaseProject::m_custom_word_tests.end(), includedCustomTest.c_str());
                // if the test was found, then check it in the list
                if (testIter != BaseProject::m_custom_word_tests.end())
                    {
                    m_selectedCustomTests.push_back(testIter -
                                                    BaseProject::m_custom_word_tests.begin());
                    }
                }
            m_customTestsCheckListBox = new wxCheckListBox(
                page, wxID_ANY, wxDefaultPosition, wxSize(FromDIP(wxSize(250, 250)).GetWidth(), -1),
                customTestNames, wxLB_EXTENDED | wxLB_NEEDED_SB | wxLB_HSCROLL | wxBORDER_THEME,
                wxGenericValidator(&m_selectedCustomTests));
            m_testsSizer->Add(m_customTestsCheckListBox, wxSizerFlags{}.Expand());
            }
        else
            {
            m_testsSizer->Add(new wxStaticText(page, wxID_STATIC, wxString{}));
            }
            // Dolch option
            {
            m_DolchCheckBox =
                new wxCheckBox(page, wxID_ANY, _(L"Dolch Sight Words Suite"), wxDefaultPosition,
                               wxDefaultSize, 0, wxGenericValidator(&m_includeDolchSightWords));
            m_testsSizer->Add(m_DolchCheckBox);
            }
        m_testTypesSizer->Add(m_testsSizer, wxSizerFlags{ 1 }.Expand().Border(wxLEFT));

        // test bundles
        wxArrayString testBundles;
        for (const auto& bundle : BaseProject::m_testBundles)
            {
            testBundles.Add(bundle.GetName().c_str());
            }
        m_testsBundlesRadioBox =
            new wxRadioBox(page, ID_TEST_BUNDLE_RADIO_BOX, _(L"Select the test bundle to apply:"),
                           wxDefaultPosition, wxDefaultSize, testBundles, 0, wxRA_SPECIFY_ROWS,
                           wxGenericValidator(&m_selectedBundle));
        SetSelectedTestBundle(wxGetApp().GetAppOptions()->GetSelectedTestBundle());
        m_testTypesSizer->Add(m_testsBundlesRadioBox, wxSizerFlags{}.Border(wxLEFT));

        TransferDataToWindow();
        UpdateTestsUI();
        UpdateTestSelectionMethodUI();
        optionsSizer->AddSpacer(wxSizerFlags::GetDefaultBorder());
        optionsSizer->Add(m_testTypesSizer, wxSizerFlags{ 1 }.Expand());

        pageSizer->Add(optionsSizer, wxSizerFlags{ 1 }.Expand().Border());
        }
        // preview (for a batch project, this just shows the first file currently in the list)
        {
        auto* page =
            new wxPanel(m_sideBarBook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
        auto* pageSizer = new wxBoxSizer(wxVERTICAL);
        page->SetSizer(pageSizer);
        m_sideBarBook->AddPage(page, _(L"Preview"), wxID_ANY, false, 3);

        auto* banner = new Banner(page, wxID_ANY,
                                  wxGetApp().GetResourceManager().GetSVG(L"ribbon/preview.svg"),
                                  _(L"Preview"));
        pageSizer->Add(banner, wxSizerFlags{}.Expand().Border(wxBOTTOM));

        m_previewWebView = wxWebView::New(page, wxID_ANY);
        pageSizer->Add(m_previewWebView, wxSizerFlags{ 1 }.Expand().Border());
        }
    auto* buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonsSizer->AddStretchSpacer();
    auto* backButton = new wxButton(this, wxID_BACKWARD, _(L"< Back"));
    buttonsSizer->Add(backButton, wxSizerFlags{}.Expand());
    buttonsSizer->Add(new wxButton(this, wxID_FORWARD, _(L"Forward >")), wxSizerFlags{}.Expand());
    buttonsSizer->Add(new wxButton(this, wxID_OK, _(L"Finish")), wxSizerFlags{}.Expand());
    buttonsSizer->AddSpacer(wxSizerFlags::GetDefaultBorder() * 2);
    auto* cancelButton = new wxButton(this, wxID_CANCEL);
    buttonsSizer->Add(cancelButton, wxSizerFlags{}.Expand());
    auto* helpButton = new wxButton(this, wxID_HELP);
    buttonsSizer->Add(helpButton, wxSizerFlags{}.Expand());

    backButton->Enable(false);

    mainSizer->Add(buttonsSizer, wxSizerFlags{}.Expand().Border());
    SetSizerAndFit(mainSizer);

    if (GetFileList() != nullptr)
        {
        m_fileList->SetColumnWidth(0, m_fileList->GetClientSize().GetWidth() * .75);
        m_fileList->SetColumnWidth(1, m_fileList->GetClientSize().GetWidth() * .25);
        }
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnPreviewSourceMayHaveChanged([[maybe_unused]] wxCommandEvent& event)
    {
    m_previewSourceDirty = true;
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnPreviewStructureMayHaveChanged([[maybe_unused]] wxCommandEvent& event)
    {
    m_previewFormattingDirty = true;
    }

//-------------------------------------------------------------
void ProjectWizardDlg::ReloadPreviewSource()
    {
    m_previewSourceDirty = false;
    m_previewHaveSample = false;

    if (m_previewProject == nullptr)
        {
        m_previewProject = std::make_unique<BaseProject>();
        }
    m_previewProject->SetProjectLanguage(GetLanguage());

    const wxBusyCursor busy;

    std::wstring fullText;
    if (!ExtractPreviewSourceText(fullText) || fullText.empty())
        {
        ShowPreviewMessage(_(L"Preview not available."));
        return;
        }

    m_previewSampleText = DerivePreviewSample(fullText);
    m_previewIsExcerpt = (m_previewSampleText.length() < fullText.length());
    m_previewHaveSample = true;

    RefreshPreviewFormatting();
    }

//-------------------------------------------------------------
wxString ProjectWizardDlg::GetPreviewSourceFilePath() const
    {
    if (GetProjectType() == ProjectType::BatchProject)
        {
        // preview whichever document is currently first in the file list,
        // re-picked fresh every time this is called since the list can be added to,
        // reordered, or emptied out
        return (m_fileData->GetItemCount() > 0) ? m_fileData->GetItemText(0, 0) : wxString{};
        }
    return IsTextFromFileSelected() ? GetFilePath() : wxString{};
    }

//-------------------------------------------------------------
bool ProjectWizardDlg::ExtractPreviewSourceText(std::wstring& fullText)
    {
    fullText.clear();

    if (GetProjectType() != ProjectType::BatchProject && IsManualTextEntrySelected())
        {
        std::wstring enteredText{ GetEnteredText().wc_string() };
        grammar::convert_ligatures_and_diacritics convertDiacritics;
        if (convertDiacritics(enteredText))
            {
            enteredText = convertDiacritics.get_conversion();
            }
        fullText = std::move(enteredText);
        return !fullText.empty();
        }

    const wxString filePath = GetPreviewSourceFilePath();
    if (filePath.empty())
        {
        return false;
        }

    const FilePathResolver resolvePath(filePath, true);

    // webpage is fetched quietly, once
    if (resolvePath.IsHTTPFile() || resolvePath.IsHTTPSFile())
        {
        wxString urlPath{ resolvePath.GetResolvedPath() };
        wxString content, contentType, statusText, title;
        int responseCode{ 404 };
        std::pair<bool, std::wstring> extractResult;

        if (WebHarvester::IsOneDriveDocument(urlPath))
            {
            if (!wxGetApp().GetWebHarvester().ReadWebDocument(urlPath, statusText, responseCode))
                {
                return false;
                }
            extractResult = m_previewProject->ExtractRawText(
                std::string_view{
                    wxGetApp().GetWebHarvester().GetDownloader().GetLastRead().data(),
                    wxGetApp().GetWebHarvester().GetDownloader().GetLastRead().size() },
                wxFileName{ wxGetApp().GetWebHarvester().GetDownloader().GetLastOneDriveFileName() }
                    .GetExt());
            }
        else
            {
            if (!wxGetApp().GetWebHarvester().ReadWebPage(urlPath, content, contentType, statusText,
                                                          responseCode, false))
                {
                return false;
                }
            extractResult = PreviewExtractionProject::ExtractRawTextWithEncoding(
                content.wc_string(), WebHarvester::GetFileTypeFromContentType(contentType), urlPath,
                title);
            }

        if (!extractResult.first)
            {
            return false;
            }
        fullText = std::move(extractResult.second);
        return true;
        }

    // archive subfile (e.g., "notes.zip#folder/readme.txt")
    if (resolvePath.IsArchivedFile())
        {
        const size_t poundInFile = filePath.Lower().find(_DT(L".zip#"));
        const wxFileName archiveFn(filePath.substr(0, poundInFile + 4));
        if (!wxFile::Exists(archiveFn.GetFullPath()))
            {
            return false;
            }
        const Wisteria::ZipCatalog zc(archiveFn.GetFullPath());
        wxMemoryOutputStream memstream;
        if (!zc.ReadFile(filePath.substr(poundInFile + 5), memstream))
            {
            return false;
            }
        std::pair<bool, std::wstring> extractResult = m_previewProject->ExtractRawText(
            { static_cast<const char*>(memstream.GetOutputStreamBuffer()->GetBufferStart()),
              static_cast<size_t>(memstream.GetLength()) },
            wxFileName(filePath).GetExt());
        if (!extractResult.first)
            {
            return false;
            }
        fullText = std::move(extractResult.second);
        return true;
        }

    // Excel cell (e.g., "data.xlsx#Sheet1#A1"); same reasoning as the archive branch above
    if (resolvePath.IsExcelCell())
        {
        const size_t excelTag = filePath.Lower().find(_DT(L".xlsx#"));
        const wxFileName workbookFn(filePath.substr(0, excelTag + 5));
        wxString worksheetName = filePath.substr(excelTag + 6);
        const size_t slash = worksheetName.find_last_of(L'#');
        if (!wxFile::Exists(workbookFn.GetFullPath()) || slash == wxString::npos)
            {
            return false;
            }
        const wxString cellName = worksheetName.substr(slash + 1);
        worksheetName.Truncate(slash);

        lily_of_the_valley::xlsx_extract_text filterXlsx{ false };
        const Wisteria::ZipCatalog zc(workbookFn.GetFullPath());
        const std::wstring workBookFileText = zc.ReadTextFile(L"xl/workbook.xml");
        filterXlsx.read_worksheet_names(workBookFileText.c_str(), workBookFileText.length());
        const std::wstring workbookRels = zc.ReadTextFile(L"xl/_rels/workbook.xml.rels");
        filterXlsx.read_relative_paths(workbookRels.c_str(), workbookRels.length());
        filterXlsx.map_workbook_paths();
        const std::wstring sharedStrings = zc.ReadTextFile(L"xl/sharedStrings.xml");
        if (sharedStrings.empty())
            {
            return false;
            }

        const auto& worksheetPaths = filterXlsx.get_worksheet_paths();
        const auto sheetPos =
            std::ranges::find_if(worksheetPaths, [&](const auto& wsPath)
                                 { return wsPath.first == worksheetName.wc_string(); });
        if (sheetPos == worksheetPaths.end())
            {
            return false;
            }
        const std::wstring sheetFile = zc.ReadTextFile(sheetPos->second);
        fullText =
            filterXlsx.get_cell_text(cellName.wc_str(), sharedStrings.c_str(),
                                     sharedStrings.length(), sheetFile.c_str(), sheetFile.length());
        return true;
        }

    // ODS cell (e.g., "data.ods#Sheet1#A1")
    if (resolvePath.IsOdsCell())
        {
        const size_t odsTag = filePath.Lower().find(_DT(L".ods#"));
        const wxFileName workbookFn(filePath.substr(0, odsTag + 4));
        wxString worksheetName = filePath.substr(odsTag + 5);
        const size_t slash = worksheetName.find_last_of(L'#');
        if (!wxFile::Exists(workbookFn.GetFullPath()) || slash == wxString::npos)
            {
            return false;
            }
        const wxString cellName = worksheetName.substr(slash + 1);
        worksheetName.Truncate(slash);

        lily_of_the_valley::ods_extract_text filterOds{ false };
        const Wisteria::ZipCatalog zc(workbookFn.GetFullPath());
        const std::wstring contentXml = zc.ReadTextFile(L"content.xml");
        filterOds.read_worksheet_names(contentXml.c_str(), contentXml.length());

        lily_of_the_valley::ods_extract_text::worksheet wkData;
        filterOds(contentXml.c_str(), contentXml.length(), wkData, worksheetName.ToStdWstring());
        if (wkData.empty())
            {
            return false;
            }
        fullText =
            lily_of_the_valley::spreadsheet_extract_text::get_cell_text(cellName.wc_str(), wkData);
        return true;
        }

    if (!resolvePath.IsLocalOrNetworkFile() || !wxFile::Exists(resolvePath.GetResolvedPath()))
        {
        return false;
        }

    try
        {
        const MemoryMappedFile sourceFile(resolvePath.GetResolvedPath(), true, true);
        std::pair<bool, std::wstring> extractResult = m_previewProject->ExtractRawText(
            { static_cast<const char*>(sourceFile.GetStream()), sourceFile.GetMapSize() },
            wxFileName(resolvePath.GetResolvedPath()).GetExt());
        if (!extractResult.first)
            {
            return false;
            }
        fullText = std::move(extractResult.second);
        return true;
        }
    catch (...)
        {
        return false;
        }
    }

//-------------------------------------------------------------
ParagraphParse ProjectWizardDlg::GetPreviewParagraphParsingMethod() const
    {
    // mirrors the mapping ProjectDoc::RunProjectWizard() applies when actually
    // creating the project (see standard_project_doc.cpp)
    if ((IsSplitLinesSelected() || IsCenteredTextSelected()) &&
        !IsNewLinesAlwaysNewParagraphsSelected())
        {
        return ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs;
        }
    if (IsNewLinesAlwaysNewParagraphsSelected())
        {
        return ParagraphParse::EachNewLineIsAParagraph;
        }
    if (wxFileName{ GetPreviewSourceFilePath() }.GetExt().CmpNoCase(_DT(L"doc")) == 0)
        {
        return ParagraphParse::EachNewLineIsAParagraph;
        }
    return (m_previewProject != nullptr) ? m_previewProject->GetParagraphsParsingMethod() :
                                           ParagraphParse::EachNewLineIsAParagraph;
    }

//-------------------------------------------------------------
std::wstring ProjectWizardDlg::DerivePreviewSample(const std::wstring& fullText) const
    {
    constexpr size_t previewWordGoal{ 300 };

    if (fullText.empty())
        {
        return fullText;
        }

    // scans the raw text with the same low-level tokenizer the indexing engine uses
    // (rather than indexing the whole document through BaseProject) so this stays
    // cheap for large documents
    tokenize::document_tokenize<> tokenizer(
        fullText.c_str(), fullText.length(),
        GetPreviewParagraphParsingMethod() == ParagraphParse::EachNewLineIsAParagraph,
        IsSplitLinesSelected(), IsCenteredTextSelected(),
        (m_previewProject != nullptr) && m_previewProject->GetSentenceStartMustBeUppercased());

    size_t wordCount{ 0 };
    size_t targetSentenceIndex{ static_cast<size_t>(-1) };
    const wchar_t* cutPosition{ nullptr };
    const wchar_t* currentWord{ nullptr };

    while ((currentWord = tokenizer()) != nullptr)
        {
        ++wordCount;
        if (wordCount == previewWordGoal)
            {
            targetSentenceIndex = tokenizer.get_current_sentence_index();
            }
        else if (wordCount > previewWordGoal &&
                 tokenizer.get_current_sentence_index() != targetSentenceIndex)
            {
            cutPosition = currentWord;
            break;
            }
        }

    // word #300 was in the document's last sentence (or the document is shorter
    // than that to begin with), use it as-is
    if (cutPosition == nullptr)
        {
        return fullText;
        }

    size_t cutLength = static_cast<size_t>(cutPosition - fullText.c_str());
    while (cutLength > 0 && characters::is_character::is_space(fullText[cutLength - 1]))
        {
        --cutLength;
        }

    return fullText.substr(0, cutLength);
    }

//-------------------------------------------------------------
void ProjectWizardDlg::RefreshPreviewFormatting()
    {
    m_previewFormattingDirty = false;

    if (!m_previewHaveSample || m_previewProject == nullptr)
        {
        ShowPreviewMessage(_(L"Preview not available for this source."));
        return;
        }

    m_previewProject->IgnoreBlankLinesForParagraphsParser(IsSplitLinesSelected());
    m_previewProject->IgnoreIndentingForParagraphsParser(IsCenteredTextSelected());
    m_previewProject->SetParagraphsParsingMethod(GetPreviewParagraphParsingMethod());
    if (IsNarrativeSelected())
        {
        m_previewProject->SetInvalidSentenceMethod(InvalidSentence::ExcludeFromAnalysis);
        }
    else if (IsFragmentedTextSelected())
        {
        m_previewProject->SetInvalidSentenceMethod(InvalidSentence::IncludeAsFullSentences);
        }

    m_previewProject->SetDocumentText(m_previewSampleText);
    try
        {
        m_previewProject->LoadDocument();
        }
    catch (...)
        {
        ShowPreviewMessage(_(L"Preview not available for this source."));
        return;
        }

    const bool textBeingExcluded =
        (m_previewProject->GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis ||
         m_previewProject->GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings);

    const NoWordHighlighting noHighlighting;
    std::wstring formattedBody;
    FormatWordCollectionHighlightedWords(
        m_previewProject->GetWords(), noHighlighting, formattedBody, std::wstring{}, std::wstring{},
        std::wstring{}, std::wstring{ L"<span class=\"hl-excluded\">" }, std::wstring{ L"</span>" },
        std::wstring{ L"&nbsp;&nbsp;&nbsp;&nbsp;" }, std::wstring{ L"<br />\n" }, textBeingExcluded,
        m_previewProject->GetInvalidSentenceMethod() == InvalidSentence::ExcludeExceptForHeadings,
        textBeingExcluded, false);

    wxString pageHtml =
        wxString::Format(L"<div class=\"report-banner legend-card\">"
                         "<div class=\"report-banner-accent\"></div>"
                         "<div class=\"report-banner-content\">"
                         "<span class=\"hl-swatch hl-swatch-excluded\"></span>&nbsp;&nbsp;%s"
                         "</div></div>\n",
                         _(L"Excluded text"));
    pageHtml += formattedBody;
    if (m_previewIsExcerpt)
        {
        pageHtml += L"<br />\n…";
        }

    m_previewWebView->SetPage(BuildPreviewHtml(pageHtml), wxString{});
    }

//-------------------------------------------------------------
wxString ProjectWizardDlg::BuildPreviewHtml(const wxString& sampleHtmlBody) const
    {
    const bool isBackgroundMode = (wxGetApp().GetAppOptions()->GetTextHighlightMethod() ==
                                   TextHighlight::HighlightBackground);
    const wxColour excludedColor = wxGetApp().GetAppOptions()->GetExcludedTextHighlightColor();

    const wxString excludedRule =
        isBackgroundMode ?
            wxString::Format(L".hl-swatch-excluded { background-color: %s; }"
                             "\n.hl-excluded { background-color: %s; color: %s; text-decoration: "
                             "line-through; }",
                             excludedColor.GetAsString(wxC2S_HTML_SYNTAX),
                             excludedColor.GetAsString(wxC2S_HTML_SYNTAX),
                             Wisteria::Colors::ColorContrast::BlackOrWhiteContrast(excludedColor)
                                 .GetAsString(wxC2S_HTML_SYNTAX)) :
            wxString::Format(
                L".hl-swatch-excluded { background-color: light-dark(%s, %s); }"
                "\n.hl-excluded { color: light-dark(%s, %s); text-decoration: line-through; }",
                Wisteria::Colors::ColorContrast::Shade(excludedColor, 0.4)
                    .GetAsString(wxC2S_HTML_SYNTAX),
                Wisteria::Colors::ColorContrast::Tint(excludedColor, 0.6)
                    .GetAsString(wxC2S_HTML_SYNTAX),
                Wisteria::Colors::ColorContrast::Shade(excludedColor, 0.4)
                    .GetAsString(wxC2S_HTML_SYNTAX),
                Wisteria::Colors::ColorContrast::Tint(excludedColor, 0.6)
                    .GetAsString(wxC2S_HTML_SYNTAX));

    // the same report theme (default.css, overlaid with the user's chosen report theme)
    // used by the app's other report webviews, so the legend card/swatch match exactly
    const wxString themeCss = ProjectReportFormat::GetThemeCss(
        _DT(L"default.css"), wxGetApp().GetAppOptions()->GetReportTheme());

    return wxString::Format(
        L"<!DOCTYPE html>\n<html>\n<head>\n"
        "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\" />\n"
        "<meta name=\"color-scheme\" content=\"light dark\" />\n"
        "<style>\n%s\n%s\n</style>\n"
        "</head>\n<body>\n%s\n</body>\n</html>",
        themeCss, excludedRule, sampleHtmlBody);
    }

//-------------------------------------------------------------
void ProjectWizardDlg::ShowPreviewMessage(const wxString& message)
    {
    if (m_previewWebView != nullptr)
        {
        m_previewWebView->SetPage(BuildPreviewHtml(message), wxString{});
        }
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnDeleteFromListClick([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_fileList != nullptr)
        {
        m_fileList->DeleteSelectedItems();
        }
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnAddToListClick([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_fileList != nullptr)
        {
        m_fileList->EditItem(m_fileList->AddRow(), 0);
        }
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnGroupClick([[maybe_unused]] wxCommandEvent& event)
    {
    if (m_fileList != nullptr)
        {
        const auto getCommonFolder = [this]()
        {
            auto selectedItem = m_fileList->GetFirstSelected();
            if (selectedItem != wxNOT_FOUND)
                {
                wxString lastPath{ m_fileList->GetItemTextFormatted(selectedItem, 0) };
                wxString lastCommonFolder;
                while (selectedItem != wxNOT_FOUND)
                    {
                    selectedItem = m_fileList->GetNextSelected(selectedItem);
                    if (selectedItem == wxNOT_FOUND)
                        {
                        break;
                        }
                    const auto [currentCommonFolder, folderPos] = GetCommonFolder(
                        lastPath, m_fileList->GetItemTextFormatted(selectedItem, 0));
                    if (currentCommonFolder.empty())
                        {
                        return wxString{};
                        }
                    if (!lastCommonFolder.empty() &&
                        currentCommonFolder.CmpNoCase(lastCommonFolder) != 0)
                        {
                        return wxString{};
                        }
                    lastPath = m_fileList->GetItemTextFormatted(selectedItem, 0);
                    lastCommonFolder = currentCommonFolder;
                    }
                return lastCommonFolder;
                }

            return wxString{};
        };

        auto firstSelected = m_fileList->GetFirstSelected();
        if (firstSelected != wxNOT_FOUND)
            {
            // use either the first group (if already specified), or find the common folder
            // from selected items and use that as the default group
            wxString currentGroup{ m_fileList->GetItemTextFormatted(firstSelected, 1) };
            // cppcheck-suppress knownConditionTrueFalse
            if (currentGroup.empty())
                {
                currentGroup = getCommonFolder();
                }
            wxTextEntryDialog dlg(this, _(L"Enter a group label for the selected documents"),
                                  _(L"Group Label"), currentGroup);
            if (dlg.ShowModal() == wxID_OK)
                {
                const wxWindowUpdateLocker noUpdates(m_fileList);
                while (firstSelected != wxNOT_FOUND)
                    {
                    m_fileList->SetItemText(firstSelected, 1, dlg.GetValue());
                    firstSelected = m_fileList->GetNextSelected(firstSelected);
                    }
                }
            }
        }
    }

//-------------------------------------------------------------
void ProjectWizardDlg::UpdateTestsUI()
    {
    TransferDataFromWindow();
    m_readabilityTests = wxGetApp().GetAppOptions()->GetReadabilityTests();
    wxArrayString testNames; // NOLINT(misc-const-correctness)
    m_selectedTests.clear();
    for (const auto& rTest : m_readabilityTests.get_tests())
        {
        if (rTest.get_test().has_language(GetLanguage()))
            {
            testNames.push_back(rTest.get_test().get_long_name().c_str());
            // if the test is included, add it to the list of checked items
            // in this list to reflect that
            if (rTest.is_included())
                {
                m_selectedTests.push_back(testNames.size() - 1);
                }
            }
        }
    m_testsCheckListBox->Clear();
    m_testsCheckListBox->InsertItems(testNames, 0);
    m_DolchCheckBox->Enable(GetLanguage() == readability::test_language::english_test);
    TransferDataToWindow();
    }

//-------------------------------------------------------------
void ProjectWizardDlg::LoadArchive(const wxString& archivePath /*= wxString{}*/)
    {
    Wisteria::UI::ArchiveDlg dlg(this, ReadabilityAppOptions::GetDocumentFilter());
    dlg.SetPath(archivePath);
    dlg.SetSelectedFileFilter(wxGetApp().GetLastSelectedDocFilter());
    dlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                     L"online/additional-features.html");
    if (dlg.ShowModal() != wxID_OK)
        {
        return;
        }

    // see what sort of labeling should be used
    wxString groupLabel;
    bool groupByLastCommonFolder{ false };
    DocGroupSelectDlg selectLabelTypeDlg(this);
    selectLabelTypeDlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                                    L"online/additional-features.html");
    selectLabelTypeDlg.SetSelection(wxGetApp().GetAppOptions()->GetBatchGroupMethod());
    if (selectLabelTypeDlg.ShowModal() != wxID_OK)
        {
        return;
        }
    if (selectLabelTypeDlg.GetSelection() == 1)
        {
        groupLabel = selectLabelTypeDlg.GetGroupingLabel();
        }
    else if (selectLabelTypeDlg.GetSelection() == 2)
        {
        groupByLastCommonFolder = true;
        }
    wxGetApp().GetAppOptions()->SetBatchGroupMethod(selectLabelTypeDlg.GetSelection());

    const wxWindowDisabler disableAll;
    const wxBusyInfo wait(_(L"Retrieving files..."), this);
#ifdef __WXGTK__
    wxMilliSleep(100);
    wxGetApp().Yield();
#endif
    wxGetApp().SetLastSelectedDocFilter(dlg.GetSelectedFileFilter());

    const Wisteria::ZipCatalog archive(dlg.GetPath());
    wxArrayString files = FilterFiles(archive.GetPaths(),
                                      ExtractExtensionsFromFileFilter(dlg.GetSelectedFileFilter()));
    files.Sort();

    const size_t currentFileCount = m_fileData->GetItemCount();
    m_fileData->SetSize(currentFileCount + files.GetCount(), 2);
    for (size_t i = 0; i < files.GetCount(); ++i)
        {
        m_fileData->SetItemText(
            currentFileCount + i, 0, dlg.GetPath() + L"#" + files[i],
            Wisteria::NumberFormatInfo{
                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
            std::numeric_limits<double>::quiet_NaN());
        if (!groupLabel.empty())
            {
            m_fileData->SetItemText(
                currentFileCount + i, 1, groupLabel,
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        }

    if (groupByLastCommonFolder && !files.empty())
        {
        LoadGroupFromLastCommonFolder(currentFileCount, files);
        }

    if (m_fileList != nullptr)
        {
        m_fileList->SetVirtualDataSize(m_fileData->GetItemCount());
        m_fileList->SetColumnWidth(0, m_fileList->GetClientSize().GetWidth() * .75);
        m_fileList->SetColumnWidth(1, m_fileList->GetClientSize().GetWidth() * .25);
        }
    }

//-------------------------------------------------------------
readability::test_language ProjectWizardDlg::GetLanguage() const
    {
    return static_cast<readability::test_language>(m_selectedLang);
    }

//-------------------------------------------------------------
void ProjectWizardDlg::LoadSpreadsheet(wxString filePath /*= wxString{}*/)
    {
    if (filePath.empty())
        {
        wxFileDialog dlg(this, _(L"Select Spreadsheet to Analyze"), wxString{}, wxString{},
                         _(L"Spreadsheet Files (*.xlsx;*.ods)|*.xlsx;*.ods|Excel Files "
                           "(*.xlsx)|*.xlsx|OpenDocument Spreadsheet Files (*.ods)|*.ods"),
                         wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);

        if (dlg.ShowModal() != wxID_OK)
            {
            return;
            }
        filePath = dlg.GetPath();
        }

    // only a provided group label makes sense here since cells don't have document
    // descriptions and there aren't any folders to pull a group from
    wxString groupLabel;
    wxTextEntryDialog dlg(this, _(L"Enter a group label for the selected documents"),
                          _(L"Group Label"));
    if (dlg.ShowModal() == wxID_OK)
        {
        groupLabel = dlg.GetValue();
        }

    // name of worksheets and list of cells with text in them
    std::vector<std::pair<std::wstring, std::vector<std::wstring>>> workSheets;

    const bool isOds{ wxFileName(filePath).GetExt().CmpNoCase(L"ods") == 0 };

    if (isOds)
        {
        const Wisteria::ZipCatalog archive(filePath);
        if (archive.Find(L"content.xml") == nullptr)
            {
            wxMessageBox(
                _(L"Unable to open ODS document, file is either password-protected or corrupt."),
                wxGetApp().GetAppDisplayName(), wxICON_EXCLAMATION | wxOK);
            return;
            }
        lily_of_the_valley::ods_extract_text odsExtract{ false };
        std::wstring contentXml;
            {
            const wxWindowDisabler disableAll;
            const wxBusyInfo wait(_(L"Loading ODS file..."), this);
#ifdef __WXGTK__
            wxMilliSleep(100);
            wxGetApp().Yield();
#endif
            contentXml = archive.ReadTextFile(L"content.xml");
            odsExtract.read_worksheet_names(contentXml.c_str(), contentXml.length());
            }

        wxArrayString worksheets;
        wxArrayInt workSheetSelections;

        const auto worksheetNames = odsExtract.get_worksheet_names();
        for (size_t i = 0; i < worksheetNames.size(); ++i)
            {
            worksheets.push_back(worksheetNames[i].c_str());
            workSheetSelections.push_back(i);
            }

        // only ask for which worksheets to select if there is more than one in the workbook
        if (worksheets.size() > 1)
            {
            wxMultiChoiceDialog chooseWorksheetsDlg(this, _(L"Select the worksheets to import:"),
                                                    _(L"ODS Import"), worksheets);
            chooseWorksheetsDlg.SetSelections(workSheetSelections);
            if (chooseWorksheetsDlg.ShowModal() != wxID_OK)
                {
                return;
                }
            workSheetSelections = chooseWorksheetsDlg.GetSelections();
            }

        for (size_t i = 0; i < workSheetSelections.size(); ++i)
            {
            lily_of_the_valley::ods_extract_text::worksheet wrk;
                {
                const wxWindowDisabler disableAll;
                const wxBusyInfo wait(_(L"Loading worksheet..."), this);
#ifdef __WXGTK__
                wxMilliSleep(100);
                wxGetApp().Yield();
#endif
                odsExtract(contentXml.c_str(), contentXml.length(), wrk,
                           worksheetNames[workSheetSelections.Item(i)]);
                }

            Wisteria::UI::OdsPreviewDlg odsPreview(
                this, &wrk, &odsExtract, wxID_ANY,
                wxString::Format(_(L"\"%s\" Preview"),
                                 worksheetNames[workSheetSelections.Item(i)].c_str()));

            odsPreview.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                                    L"online/additional-features.html");
            if (odsPreview.ShowModal() == wxID_OK)
                {
                // filter just the rows that were requested by setting unselected cells to empty
                if (odsPreview.IsImportingOnlySelectedCells())
                    {
                    for (size_t rowPos = 0; rowPos < wrk.size(); ++rowPos)
                        {
                        for (size_t colPos = 0; colPos < wrk[rowPos].size(); ++colPos)
                            {
                            if (!odsPreview.IsCellSelected(wxGridCellCoords(rowPos, colPos)))
                                {
                                wrk[rowPos].operator[](colPos).set_value(std::wstring());
                                }
                            }
                        }
                    }
#ifndef NDEBUG
                if (!lily_of_the_valley::ods_extract_text::verify_sheet(wrk).first)
                    {
                    wxFAIL_MSG(
                        wxString(L"ODS worksheet cell's out of order. First incorrect cell: ") +
                        lily_of_the_valley::ods_extract_text::verify_sheet(wrk).second.c_str());
                    }
                // verify that the filtering looks OK when debugging
                Wisteria::UI::OdsPreviewDlg odsPreviewFilterDEBUG(
                    this, &wrk, &odsExtract, wxID_ANY,
                    wxString(_DT(L"DEBUG CHECK ")) +
                        worksheetNames[workSheetSelections.Item(i)].c_str());
                odsPreviewFilterDEBUG.ShowModal();
#endif
                workSheets.emplace_back(worksheetNames[workSheetSelections.Item(i)],
                                        std::vector<std::wstring>());

                lily_of_the_valley::ods_extract_text::get_text_cell_names(wrk,
                                                                          workSheets.back().second);
                }
            }
        }
    else
        {
        const Wisteria::ZipCatalog archive(filePath);
        if (archive.Find(L"xl/workbook.xml") == nullptr)
            {
            wxMessageBox(
                _(L"Unable to open Excel document, file is either password-protected or corrupt."),
                wxGetApp().GetAppDisplayName(), wxICON_EXCLAMATION | wxOK);
            return;
            }
        lily_of_the_valley::xlsx_extract_text excelExtract{ false };
            {
            const wxWindowDisabler disableAll;
            const wxBusyInfo wait(_(L"Loading Excel file..."), this);
#ifdef __WXGTK__
            wxMilliSleep(100);
            wxGetApp().Yield();
#endif
            const std::wstring workbookFileText = archive.ReadTextFile(L"xl/workbook.xml");
            excelExtract.read_worksheet_names(workbookFileText.c_str(), workbookFileText.length());
            // read workbook relationships
            const std::wstring workbookRels = archive.ReadTextFile(L"xl/_rels/workbook.xml.rels");
            excelExtract.read_relative_paths(workbookRels.c_str(), workbookRels.length());
            // resolve worksheet names to XML paths
            excelExtract.map_workbook_paths();
            // read the string table
            const std::wstring sharedStrings = archive.ReadTextFile(L"xl/sharedStrings.xml");
            if (!sharedStrings.empty())
                {
                excelExtract.read_shared_strings(sharedStrings.c_str(), sharedStrings.length());
                }
            }
        wxArrayString worksheets;
        wxArrayInt workSheetSelections;

        const auto& worksheetPaths = excelExtract.get_worksheet_paths();
        for (size_t i = 0; i < worksheetPaths.size(); ++i)
            {
            worksheets.push_back(worksheetPaths[i].first.c_str());
            workSheetSelections.push_back(i);
            }

        // only ask for which worksheets to select if there is more than one in the workbook
        if (worksheets.size() > 1)
            {
            wxMultiChoiceDialog chooseWorksheetsDlg(this, _(L"Select the worksheets to import:"),
                                                    _(L"Excel Import"), worksheets);
            chooseWorksheetsDlg.SetSelections(workSheetSelections);
            if (chooseWorksheetsDlg.ShowModal() != wxID_OK)
                {
                return;
                }
            workSheetSelections = chooseWorksheetsDlg.GetSelections();
            }

        for (size_t i = 0; i < workSheetSelections.size(); ++i)
            {
            lily_of_the_valley::xlsx_extract_text::worksheet wrk;
                {
                const wxWindowDisabler disableAll;
                const wxBusyInfo wait(_(L"Loading worksheet..."), this);
#ifdef __WXGTK__
                wxMilliSleep(100);
                wxGetApp().Yield();
#endif
                const std::wstring sheetFile =
                    archive.ReadTextFile(worksheetPaths[workSheetSelections.Item(i)].second);

                if (!sheetFile.empty())
                    {
                    excelExtract(sheetFile.c_str(), sheetFile.length(), wrk);
                    }
                else
                    {
                    return;
                    }
                }

            Wisteria::UI::ExcelPreviewDlg excelPreview(
                this, &wrk, &excelExtract, wxID_ANY,
                wxString::Format(_(L"\"%s\" Preview"),
                                 worksheetPaths[workSheetSelections.Item(i)].first.c_str()));

            excelPreview.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                                      L"online/additional-features.html");
            if (excelPreview.ShowModal() == wxID_OK)
                {
                // filter just the rows that were requested by setting unselected cells to empty
                if (excelPreview.IsImportingOnlySelectedCells())
                    {
                    for (size_t rowPos = 0; rowPos < wrk.size(); ++rowPos)
                        {
                        for (size_t colPos = 0; colPos < wrk[rowPos].size(); ++colPos)
                            {
                            if (!excelPreview.IsCellSelected(wxGridCellCoords(rowPos, colPos)))
                                {
                                wrk[rowPos].operator[](colPos).set_value(std::wstring());
                                }
                            }
                        }
                    }
#ifndef NDEBUG
                if (!lily_of_the_valley::xlsx_extract_text::verify_sheet(wrk).first)
                    {
                    wxFAIL_MSG(
                        wxString(L"Excel worksheet cell's out of order. First incorrect cell: ") +
                        lily_of_the_valley::xlsx_extract_text::verify_sheet(wrk).second.c_str());
                    }
                // verify that the filtering looks OK when debugging
                Wisteria::UI::ExcelPreviewDlg excelPreviewFilterDEBUG(
                    this, &wrk, &excelExtract, wxID_ANY,
                    wxString(_DT(L"DEBUG CHECK ")) +
                        worksheetPaths[workSheetSelections.Item(i)].first.c_str());
                excelPreviewFilterDEBUG.ShowModal();
#endif
                workSheets.emplace_back(worksheetPaths[workSheetSelections.Item(i)].first,
                                        std::vector<std::wstring>());

                lily_of_the_valley::xlsx_extract_text::get_text_cell_names(
                    wrk, workSheets.back().second);
                }
            }
        }

    if (!workSheets.empty())
        {
        const wxWindowDisabler disableAll;
        const wxBusyInfo wait(_(L"Updating file list..."), this);
#ifdef __WXGTK__
        wxMilliSleep(100);
        wxGetApp().Yield();
#endif
        // list the cells in the grid
        const size_t currentFileCount = m_fileData->GetItemCount();
        size_t cellCount = 0; // NOLINT(misc-const-correctness)
        for (auto& workSheet : workSheets)
            {
            cellCount += workSheet.second.size();
            }
        m_fileData->SetSize(currentFileCount + cellCount, 2);

        size_t cellCounter = 0; // NOLINT(misc-const-correctness)
        for (auto& workSheet : workSheets)
            {
            const wxString fullPath = filePath + L"#" + workSheet.first.c_str() + L"#";
            for (auto cellPos = workSheet.second.begin(); cellPos != workSheet.second.end();
                 ++cellPos, ++cellCounter)
                {
                m_fileData->SetItemText(
                    currentFileCount + cellCounter, 0, fullPath + cellPos->c_str(),
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                if (!groupLabel.empty())
                    {
                    m_fileData->SetItemText(
                        currentFileCount + cellCounter, 1, groupLabel,
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    }
                }
            }
        if (m_fileList != nullptr)
            {
            m_fileList->SetVirtualDataSize(m_fileData->GetItemCount());
            m_fileList->SetColumnWidth(0, m_fileList->GetClientSize().GetWidth() * .75);
            m_fileList->SetColumnWidth(1, m_fileList->GetClientSize().GetWidth() * .25);
            }
        }
    }

//-------------------------------------------------------------
void ProjectWizardDlg::UpdateTestSelectionMethodUI()
    {
    TransferDataFromWindow();
    if (IsDocumentTypeSelected())
        {
        m_testTypesSizer->Show(m_docTypeRadioBox, true, true);
        m_testTypesSizer->Hide(m_industryTypeRadioBox, true);
        m_testTypesSizer->Hide(m_testsSizer, true);
        m_testTypesSizer->Hide(m_testsBundlesRadioBox, true);
        }
    else if (IsIndustrySelected())
        {
        m_testTypesSizer->Show(m_industryTypeRadioBox, true, true);
        m_testTypesSizer->Hide(m_docTypeRadioBox, true);
        m_testTypesSizer->Hide(m_testsSizer, true);
        m_testTypesSizer->Hide(m_testsBundlesRadioBox, true);
        }
    else if (IsManualTestSelected())
        {
        m_testTypesSizer->Hide(m_docTypeRadioBox, true);
        m_testTypesSizer->Hide(m_industryTypeRadioBox, true);
        m_testTypesSizer->Show(m_testsSizer, true, true);
        m_testTypesSizer->Hide(m_testsBundlesRadioBox, true);
        }
    else if (IsTestBundleSelected())
        {
        m_testTypesSizer->Hide(m_docTypeRadioBox, true);
        m_testTypesSizer->Hide(m_industryTypeRadioBox, true);
        m_testTypesSizer->Hide(m_testsSizer, true);
        m_testTypesSizer->Show(m_testsBundlesRadioBox, true, true);
        }
    m_testTypesSizer->Layout();
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnPageChange(wxBookCtrlEvent& event)
    {
    if (event.GetSelection() == 0)
        {
        if (wxWindow::FindWindow(wxID_BACKWARD) != nullptr)
            {
            wxWindow::FindWindow(wxID_BACKWARD)->Enable(false);
            }
        if (wxWindow::FindWindow(wxID_FORWARD) != nullptr)
            {
            wxWindow::FindWindow(wxID_FORWARD)->Enable(true);
            }
        }
    else if (static_cast<size_t>(event.GetSelection()) == m_sideBarBook->GetPageCount() - 1)
        {
        if (wxWindow::FindWindow(wxID_FORWARD) != nullptr)
            {
            wxWindow::FindWindow(wxID_FORWARD)->Enable(false);
            }
        if (wxWindow::FindWindow(wxID_BACKWARD) != nullptr)
            {
            wxWindow::FindWindow(wxID_BACKWARD)->Enable(true);
            }
        if (m_previewWebView != nullptr)
            {
            TransferDataFromWindow();
            // a batch project's source is whichever file is currently first in the list
            if (GetProjectType() == ProjectType::BatchProject)
                {
                const wxString currentFirstFile = GetPreviewSourceFilePath();
                if (currentFirstFile != m_previewLastBatchFilePath)
                    {
                    m_previewLastBatchFilePath = currentFirstFile;
                    m_previewSourceDirty = true;
                    }
                }
            if (m_previewSourceDirty || !m_previewHaveSample)
                {
                ReloadPreviewSource();
                }
            else if (m_previewFormattingDirty)
                {
                RefreshPreviewFormatting();
                }
            }
        }
    else
        {
        if (wxWindow::FindWindow(wxID_BACKWARD) != nullptr)
            {
            wxWindow::FindWindow(wxID_BACKWARD)->Enable(true);
            }
        if (wxWindow::FindWindow(wxID_FORWARD) != nullptr)
            {
            wxWindow::FindWindow(wxID_FORWARD)->Enable(true);
            }
        }
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnNavigate(wxCommandEvent& event)
    {
    if (event.GetId() == wxID_FORWARD)
        {
        if (static_cast<size_t>(m_sideBarBook->GetSelection()) < m_sideBarBook->GetPageCount() - 1)
            {
            m_sideBarBook->SetSelection(m_sideBarBook->GetSelection() + 1);
            }
        if (static_cast<size_t>(m_sideBarBook->GetSelection()) == m_sideBarBook->GetPageCount() - 1)
            {
            wxWindow::FindWindow(wxID_FORWARD)->Enable(false);
            }
        wxWindow::FindWindow(wxID_BACKWARD)->Enable(true);
        }
    else
        {
        if (m_sideBarBook->GetSelection() > 0)
            {
            m_sideBarBook->SetSelection(m_sideBarBook->GetSelection() - 1);
            }
        if (m_sideBarBook->GetSelection() == 0)
            {
            wxWindow::FindWindow(wxID_BACKWARD)->Enable(false);
            }
        wxWindow::FindWindow(wxID_FORWARD)->Enable(true);
        }
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnOK([[maybe_unused]] wxCommandEvent& event)
    {
    Validate();
    TransferDataFromWindow();

    if (GetProjectType() == ProjectType::StandardProject)
        {
        if (IsTextFromFileSelected())
            {
            const FilePathResolver resolvePath(GetFilePath(), true);
            if (resolvePath.IsInvalidFile() || (resolvePath.IsLocalOrNetworkFile() &&
                                                !wxFile::Exists(resolvePath.GetResolvedPath())))
                {
                // open the file dialog to give the user one last chance to select file
                wxCommandEvent fileButtonEvent(wxEVT_COMMAND_BUTTON_CLICKED, ID_FILE_BROWSE_BUTTON);
                GetEventHandler()->ProcessEvent(fileButtonEvent);
                // if user hit Cancel then don't proceed and tell them what to do
                if (GetFilePath().empty())
                    {
                    m_sideBarBook->SetSelection(0);
                    wxMessageBox(_(L"Please enter a file to be analyzed before continuing."),
                                 _(L"Invalid Input"), wxICON_EXCLAMATION | wxOK, this);
                    return;
                    }
                // if file doesn't exist
                if (!wxFile::Exists(GetFilePath()))
                    {
                    m_sideBarBook->SetSelection(0);
                    wxMessageBox(_(L"File not found. Please enter a valid file to be analyzed "
                                   "before continuing."),
                                 _(L"Invalid Input"), wxICON_EXCLAMATION | wxOK, this);
                    return;
                    }
                }
            }
        }
    else
        {
        if (GetFileData()->GetItemCount() == 0)
            {
            // open the file dialog to give the user one last chance to select file
            wxCommandEvent fileButtonEvent(wxEVT_COMMAND_BUTTON_CLICKED,
                                           ID_BATCH_FOLDER_BROWSE_BUTTON);
            GetEventHandler()->ProcessEvent(fileButtonEvent);
            // if user hit Cancel then don't proceed and tell them what to do
            if (GetFileData()->GetItemCount() == 0)
                {
                wxMessageBox(_(L"Please select files to be analyzed before continuing."),
                             _(L"Invalid Input"), wxICON_EXCLAMATION | wxOK, this);
                return;
                }
            }
        }

    // if user indicates that the document is non-narrative text that should be fully analyzed, then
    // non-narrative form on the document type page should be selected too
    if (IsFragmentedTextSelected() && IsDocumentTypeSelected() &&
        GetSelectedDocumentType() != readability::document_classification::nonnarrative_document)
        {
        wxMessageBox(
            _(L"Because the document composition was set to non-narrative text, the document type "
              "will also be adjusted to non-narrative."),
            wxGetApp().GetAppDisplayName(), wxICON_INFORMATION | wxOK, this);
        SelectDocumentType(readability::document_classification::nonnarrative_document);
        }
    // and the inverse as well, if the tests for non-narrative form are selected,
    // then set the composition to non-narrative.
    if (IsDocumentTypeSelected() &&
        GetSelectedDocumentType() == readability::document_classification::nonnarrative_document &&
        !IsFragmentedTextSelected())
        {
        wxMessageBox(
            _(L"Because the test selection is set to non-narrative, the document composition will "
              "also be set to non-narrative."),
            wxGetApp().GetAppDisplayName(), wxICON_INFORMATION | wxOK, this);
        SetFragmentedTextSelected();
        }
    if (IsCenteredTextSelected() && IsNewLinesAlwaysNewParagraphsSelected())
        {
        SelectPage(1);
        wxMessageBox(
            _(L"If text is centered, then new lines should not force the start of a new paragraph. "
              "Please unselect one of these options."),
            wxGetApp().GetAppDisplayName(), wxICON_INFORMATION | wxOK, this);
        return;
        }
    if (IsSplitLinesSelected() && IsNewLinesAlwaysNewParagraphsSelected())
        {
        SelectPage(1);
        wxMessageBox(_(L"If sentences are split, then new lines should not force the start of a "
                       "new paragraph. Please unselect one of these options."),
                     wxGetApp().GetAppDisplayName(), wxICON_INFORMATION | wxOK, this);
        return;
        }

    if (IsDocumentTypeSelected())
        {
        bool hasIncludedStandardTest = false; // NOLINT(misc-const-correctness)
        for (const auto& rTest : m_readabilityTests.get_tests())
            {
            if (rTest.get_test().has_document_classification(GetSelectedDocumentType()) &&
                rTest.get_test().has_language(GetLanguage()))
                {
                hasIncludedStandardTest = true;
                break;
                }
            }
        if (!hasIncludedStandardTest)
            {
            wxMessageBox(_(L"Note that there are no tests associated with this document type for "
                           "the selected language."),
                         wxGetApp().GetAppDisplayName(), wxICON_INFORMATION | wxOK);
            }
        }
    else if (IsIndustrySelected())
        {
        bool hasIncludedStandardTest = false; // NOLINT(misc-const-correctness)
        for (const auto& rTest : m_readabilityTests.get_tests())
            {
            if (rTest.get_test().has_industry_classification(GetSelectedIndustryType()) &&
                rTest.get_test().has_language(GetLanguage()))
                {
                hasIncludedStandardTest = true;
                break;
                }
            }
        if (!hasIncludedStandardTest)
            {
            wxMessageBox(_(L"Note that there are no tests associated with this "
                           "industry for the selected language."),
                         wxGetApp().GetAppDisplayName(), wxICON_INFORMATION | wxOK);
            }
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
void ProjectWizardDlg::OnTestSelectionMethodChanged([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    UpdateTestSelectionMethodUI();
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    wxLaunchDefaultBrowser(wxFileName::FileNameToURL(wxGetApp().GetMainFrame()->GetHelpDirectory() +
                                                     wxFileName::GetPathSeparator() +
                                                     L"online/analysis-notes.html"));
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnLanguageChanged([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    UpdateTestsUI();
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnSourceRadioChange(wxCommandEvent& event)
    {
    m_textEntryEdit->Enable((event.GetId() == ID_MANUALLY_ENTERED_TEXT_BUTTON));
    m_filePathEdit->Enable((event.GetId() == ID_FROM_FILE_BUTTON));
    m_fileBrowseButton->Enable((event.GetId() == ID_FROM_FILE_BUTTON));
    m_previewSourceDirty = true;
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnFileBrowseButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    wxFileDialog dialog(this, _(L"Select Document to Analyze"), wxString{}, wxString{},
                        ReadabilityAppOptions::GetDocumentFilter(),
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW);

    if (dialog.ShowModal() != wxID_OK)
        {
        return;
        }

    m_filePath = dialog.GetPath();
    TransferDataToWindow();
    m_previewSourceDirty = true;
    SetFocus();
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnRandomSampleCheck([[maybe_unused]] wxCommandEvent& event)
    {
    TransferDataFromWindow();
    if ((m_randPercentageCtrl != nullptr) && (m_isRandomSampling != nullptr))
        {
        m_randPercentageCtrl->Enable(m_isRandomSampling->IsChecked());
        }
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnAddWebPageButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    wxTextEntryDialog textDlg(this, _(L"Enter a web page to analyze:"), _(L"Enter Web Page"));
    if (textDlg.ShowModal() == wxID_OK && !textDlg.GetValue().empty())
        {
        const FilePathResolver resolver(textDlg.GetValue(), false);
        if (!resolver.IsWebFile())
            {
            wxMessageBox(
                wxString::Format(_(L"\"%s\" does not appear to be a valid web page.\n"
                                   "Be sure to include the 'www', 'http', or 'https' prefix."),
                                 resolver.GetResolvedPath()),
                wxGetApp().GetAppDisplayName(), wxICON_WARNING | wxOK);
            return;
            }
        // see what sort of labeling should be used
        wxString groupLabel;
        DocGroupSelectDlg selectLabelTypeDlg(this);
        selectLabelTypeDlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                                        L"online/additional-features.html");
        selectLabelTypeDlg.SetSelection(wxGetApp().GetAppOptions()->GetBatchGroupMethod());
        if (selectLabelTypeDlg.ShowModal() != wxID_OK)
            {
            return;
            }
        if (selectLabelTypeDlg.GetSelection() == 1)
            {
            groupLabel = selectLabelTypeDlg.GetGroupingLabel();
            }
        wxGetApp().GetAppOptions()->SetBatchGroupMethod(selectLabelTypeDlg.GetSelection());

        const size_t currentFileCount = m_fileData->GetItemCount();
        m_fileData->SetSize(currentFileCount + 1, 2);
        m_fileData->SetItemText(
            currentFileCount, 0, resolver.GetResolvedPath(),
            Wisteria::NumberFormatInfo{
                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
            std::numeric_limits<double>::quiet_NaN());
        if (!groupLabel.empty())
            {
            m_fileData->SetItemText(
                currentFileCount, 1, groupLabel,
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        m_fileList->SetVirtualDataSize(m_fileData->GetItemCount());
        m_fileList->SetColumnWidth(0, m_fileList->GetClientSize().GetWidth() * .75);
        m_fileList->SetColumnWidth(1, m_fileList->GetClientSize().GetWidth() * .25);
        }
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnAddWebPagesButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    WebHarvesterDlg webHarvestDlg(this, wxGetApp().GetLastSelectedWebPages(),
                                  ReadabilityAppOptions::GetDocumentFilter(),
                                  wxGetApp().GetLastSelectedDocFilter(), false);
    webHarvestDlg.UpdateFromHarvesterSettings(wxGetApp().GetWebHarvester());
    webHarvestDlg.SetDownloadFolder(wxGetApp().GetAppOptions()->GetDownloadsPath());
    webHarvestDlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                               L"online/additional-features.html");
    if (webHarvestDlg.ShowModal() != wxID_OK)
        {
        return;
        }

    // see what sort of labeling should be used
    wxString groupLabel;
    bool groupByLastCommonFolder{ false };
    DocGroupSelectDlg selectLabelTypeDlg(this);
    selectLabelTypeDlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                                    L"online/additional-features.html");
    selectLabelTypeDlg.SetSelection(wxGetApp().GetAppOptions()->GetBatchGroupMethod());
    if (selectLabelTypeDlg.ShowModal() != wxID_OK)
        {
        return;
        }
    if (selectLabelTypeDlg.GetSelection() == 1)
        {
        groupLabel = selectLabelTypeDlg.GetGroupingLabel();
        }
    else if (selectLabelTypeDlg.GetSelection() == 2)
        {
        groupByLastCommonFolder = true;
        }
    wxGetApp().GetAppOptions()->SetBatchGroupMethod(selectLabelTypeDlg.GetSelection());

    wxGetApp().SetLastSelectedWebPages(webHarvestDlg.GetUrls());
    wxGetApp().SetLastSelectedDocFilter(webHarvestDlg.GetSelectedDocFilter());
    webHarvestDlg.UpdateHarvesterSettings(wxGetApp().GetWebHarvester());

    size_t totalFileCount{ 0 };
    wxArrayString files;

    for (size_t urlCounter = 0; urlCounter < webHarvestDlg.GetUrls().GetCount(); ++urlCounter)
        {
        const FilePathResolver resolver(webHarvestDlg.GetUrls().Item(urlCounter), false);
        wxGetApp().GetWebHarvester().SetUrl(resolver.GetResolvedPath());

        // if cancelled, we still will want what was harvested up to that point,
        // so it's OK to ignore the user response here
        [[maybe_unused]]
        auto crawlResult = wxGetApp().GetWebHarvester().CrawlLinks();

        // add the new links to the list
        const size_t currentFileCount = m_fileData->GetItemCount();
        totalFileCount += currentFileCount;
        size_t i = 0;
        if (webHarvestDlg.IsDownloadFilesLocally())
            {
            m_fileData->SetSize(
                currentFileCount + wxGetApp().GetWebHarvester().GetDownloadedFilePaths().size(), 2);
            for (const auto& path : wxGetApp().GetWebHarvester().GetDownloadedFilePaths())
                {
                m_fileData->SetItemText(
                    currentFileCount + i, 0, path,
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                files.push_back(path);
                if (!groupLabel.empty())
                    {
                    m_fileData->SetItemText(
                        currentFileCount + i, 1, groupLabel,
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    }
                // if they chose to use the documents' descriptions as the labels,
                // then those are loaded on import
                ++i;
                }
            }
        else
            {
            m_fileData->SetSize(
                currentFileCount + wxGetApp().GetWebHarvester().GetHarvestedLinks().size(), 2);
            for (const auto& path : wxGetApp().GetWebHarvester().GetHarvestedLinks())
                {
                m_fileData->SetItemText(
                    currentFileCount + i, 0, path,
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                files.push_back(path);
                if (!groupLabel.empty())
                    {
                    m_fileData->SetItemText(
                        currentFileCount + i, 1, groupLabel,
                        Wisteria::NumberFormatInfo{
                            Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                        std::numeric_limits<double>::quiet_NaN());
                    }
                ++i;
                }
            }
        }

    // In case JS cookies are being reused, clear them from the current session
    // (we are treating crawling multiple sites from here as one session, that restarts
    //  when the dialog is opened up again).
    wxGetApp().GetWebHarvester().ClearCookies();

    // update global internet options that mirror the same options from the dialog
    wxGetApp().GetAppOptions()->DisablePeerVerify(webHarvestDlg.IsPeerVerifyDisabled());
    wxGetApp().GetAppOptions()->UseJavaScriptCookies(webHarvestDlg.IsUsingJavaScriptCookies());
    wxGetApp().GetAppOptions()->PersistJavaScriptCookies(
        webHarvestDlg.IsPersistingJavaScriptCookies());
    wxGetApp().GetAppOptions()->SetUserAgent(webHarvestDlg.GetUserAgent());
    wxGetApp().GetAppOptions()->SetDownloadsPath(webHarvestDlg.GetDownloadFolder());

    if (groupByLastCommonFolder && !files.empty())
        {
        LoadGroupFromLastCommonFolder(totalFileCount, files);
        }

    m_fileList->SetVirtualDataSize(m_fileData->GetItemCount());
    m_fileList->SetColumnWidth(0, m_fileList->GetClientSize().GetWidth() * .75);
    m_fileList->SetColumnWidth(1, m_fileList->GetClientSize().GetWidth() * .25);
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnAddFolderButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    Wisteria::UI::GetDirFilterDialog dirDlg(this, ReadabilityAppOptions::GetDocumentFilter());
    dirDlg.SetSelectedFileFilter(wxGetApp().GetLastSelectedDocFilter());
    dirDlg.SetPath(GetLastSelectedFolder());
    dirDlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                        L"online/additional-features.html");
    if (dirDlg.ShowModal() != wxID_OK || dirDlg.GetPath().empty())
        {
        return;
        }
    // get the list of files
    wxArrayString files;
        {
        const wxWindowDisabler disableAll;
        const wxBusyInfo wait(_(L"Retrieving files..."), this);
#ifdef __WXGTK__
        wxMilliSleep(100);
        wxGetApp().Yield();
#endif
        SetLastSelectedFolder(dirDlg.GetPath());
        wxGetApp().SetLastSelectedDocFilter(dirDlg.GetSelectedFileFilter());
        wxDir::GetAllFiles(dirDlg.GetPath(), &files, wxString{},
                           dirDlg.IsRecursive() ? wxDIR_FILES | wxDIR_DIRS : wxDIR_FILES);
        files = FilterFiles(files, ExtractExtensionsFromFileFilter(dirDlg.GetSelectedFileFilter()));
        }
    files.Sort();

    // see what sort of labeling should be used
    wxString groupLabel;
    bool groupByLastCommonFolder{ false };
    DocGroupSelectDlg selectLabelTypeDlg(this);
    selectLabelTypeDlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                                    L"online/additional-features.html");
    selectLabelTypeDlg.SetSelection(wxGetApp().GetAppOptions()->GetBatchGroupMethod());
    if (selectLabelTypeDlg.ShowModal() != wxID_OK)
        {
        return;
        }
    if (selectLabelTypeDlg.GetSelection() == 1)
        {
        groupLabel = selectLabelTypeDlg.GetGroupingLabel();
        }
    else if (selectLabelTypeDlg.GetSelection() == 2)
        {
        groupByLastCommonFolder = true;
        }
    wxGetApp().GetAppOptions()->SetBatchGroupMethod(selectLabelTypeDlg.GetSelection());

    const wxWindowUpdateLocker noUpdates(m_fileList);
    const size_t currentFileCount = m_fileData->GetItemCount();
    m_fileData->SetSize(currentFileCount + files.GetCount(), 2);
    for (size_t i = 0; i < files.GetCount(); ++i)
        {
        m_fileData->SetItemText(
            currentFileCount + i, 0, files.Item(i),
            Wisteria::NumberFormatInfo{
                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
            std::numeric_limits<double>::quiet_NaN());
        if (!groupLabel.empty())
            {
            m_fileData->SetItemText(
                currentFileCount + i, 1, groupLabel,
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        // if they chose to use the documents' descriptions as the labels,
        // then those are loaded on import later
        }

    if (groupByLastCommonFolder && !files.empty())
        {
        LoadGroupFromLastCommonFolder(currentFileCount, files);
        }

    m_fileList->SetVirtualDataSize(m_fileData->GetItemCount());
    m_fileList->SetColumnWidth(0, m_fileList->GetClientSize().GetWidth() * .75);
    m_fileList->SetColumnWidth(1, m_fileList->GetClientSize().GetWidth() * .25);
    }

//-------------------------------------------------------------
void ProjectWizardDlg::LoadGroupFromLastCommonFolder(const size_t currentFileCount,
                                                     const wxArrayString& files)
    {
    std::pair<wxString, size_t> commonFolder{ wxString{}, wxString::npos };
    size_t i{ 0 };
    for (; i < files.GetCount() - 1; ++i)
        {
        // if last item had a common folder group match, then...
        if (!commonFolder.first.empty())
            {
            const auto lastMatch{ commonFolder };
            commonFolder = GetCommonFolder(files[i], files[i + 1]);
            // ...update if the match between the current item and the next one
            // is a longer path
            if (!commonFolder.first.empty() && lastMatch.second <= commonFolder.second)
                {
                m_fileData->SetItemText(
                    currentFileCount + i, 1, commonFolder.first,
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                m_fileData->SetItemText(
                    currentFileCount + i + 1, 1, commonFolder.first,
                    Wisteria::NumberFormatInfo{
                        Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                    std::numeric_limits<double>::quiet_NaN());
                }
            continue;
            }
        commonFolder = GetCommonFolder(files[i], files[i + 1]);
        if (!commonFolder.first.empty())
            {
            m_fileData->SetItemText(
                currentFileCount + i, 1, commonFolder.first,
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            m_fileData->SetItemText(
                currentFileCount + i + 1, 1, commonFolder.first,
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        }
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnAddSpreadsheetFileButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    LoadSpreadsheet();
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnAddArchiveFileButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    LoadArchive();
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnAddFileButtonClick([[maybe_unused]] wxCommandEvent& event)
    {
    wxFileDialog dialog(this, _(L"Select Documents to Analyze"), wxString{}, wxString{},
                        ReadabilityAppOptions::GetDocumentFilter(),
                        wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_PREVIEW | wxFD_MULTIPLE);

    if (dialog.ShowModal() != wxID_OK)
        {
        return;
        }

    wxArrayString files;
    dialog.GetPaths(files);

    if (files.GetCount() == 0)
        {
        return;
        }

    // see what sort of labeling should be used
    wxString groupLabel;
    bool groupByLastCommonFolder{ false };
    DocGroupSelectDlg selectLabelTypeDlg(this);
    selectLabelTypeDlg.SetHelpTopic(wxGetApp().GetMainFrame()->GetHelpDirectory(),
                                    L"online/additional-features.html");
    selectLabelTypeDlg.SetSelection(wxGetApp().GetAppOptions()->GetBatchGroupMethod());
    if (selectLabelTypeDlg.ShowModal() != wxID_OK)
        {
        return;
        }
    if (selectLabelTypeDlg.GetSelection() == 1)
        {
        groupLabel = selectLabelTypeDlg.GetGroupingLabel();
        }
    else if (selectLabelTypeDlg.GetSelection() == 2)
        {
        groupByLastCommonFolder = true;
        }
    wxGetApp().GetAppOptions()->SetBatchGroupMethod(selectLabelTypeDlg.GetSelection());

    // set the default name of the project to the last folder of the file selected here.
    const wxArrayString folders = wxFileName(wxFileName(files[0]).GetPathWithSep()).GetDirs();
    SetLastSelectedFolder(!folders.empty() ? folders.back() : wxString{});

    const size_t currentFileCount = m_fileData->GetItemCount();
    m_fileData->SetSize(currentFileCount + files.GetCount(), 2);
    for (size_t i = 0; i < files.GetCount(); ++i)
        {
        m_fileData->SetItemText(
            currentFileCount + i, 0, files.Item(i),
            Wisteria::NumberFormatInfo{
                Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
            std::numeric_limits<double>::quiet_NaN());
        if (!groupLabel.empty())
            {
            m_fileData->SetItemText(
                currentFileCount + i, 1, groupLabel,
                Wisteria::NumberFormatInfo{
                    Wisteria::NumberFormatInfo::NumberFormatType::StandardFormatting },
                std::numeric_limits<double>::quiet_NaN());
            }
        // if they chose to use the documents' descriptions as the labels,
        // then those are loaded on import
        }

    if (groupByLastCommonFolder && !files.empty())
        {
        LoadGroupFromLastCommonFolder(currentFileCount, files);
        }

    m_fileList->SetVirtualDataSize(m_fileData->GetItemCount());
    m_fileList->SetColumnWidth(0, m_fileList->GetClientSize().GetWidth() * .75);
    m_fileList->SetColumnWidth(1, m_fileList->GetClientSize().GetWidth() * .25);
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnContextHelp([[maybe_unused]] wxHelpEvent& event)
    {
    wxCommandEvent cmd;
    OnHelp(cmd);
    }

//-------------------------------------------------------------
void ProjectWizardDlg::OnHelp([[maybe_unused]] wxCommandEvent& event)
    {
    wxLaunchDefaultBrowser(wxFileName::FileNameToURL(wxGetApp().GetMainFrame()->GetHelpDirectory() +
                                                     wxFileName::GetPathSeparator() +
                                                     L"online/creating-a-new-project.html"));
    }
