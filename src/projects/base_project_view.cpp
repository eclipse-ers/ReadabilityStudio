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

#include "base_project_view.h"
#include "../app/readability_app.h"
#include "../ui/controls/script_workbench_panel.h"
#include "../ui/dialogs/test_bundle_dlg.h"
#include "batch_project_doc.h"
#include "standard_project_doc.h"
#include "standard_project_view.h"

wxDECLARE_APP(ReadabilityApp);

wxIMPLEMENT_DYNAMIC_CLASS(BaseProjectView, wxView)

    IdRange BaseProjectView::m_customTestSidebarIdRange(SIDEBAR_CUSTOM_TESTS_START_ID, 1000);

//-------------------------------------------------------
ProjectDocChildFrame* BaseProjectView::CreateChildFrame(wxDocument* doc, wxView* view)
    {
    const wxSize windowSize(std::max(wxGetApp().GetMainFrame()->GetClientSize().GetWidth(),
                                     wxGetApp().GetMainFrame()->FromDIP(800)),
                            std::max(wxGetApp().GetMainFrame()->GetClientSize().GetHeight(),
                                     wxGetApp().GetMainFrame()->FromDIP(600)));

    const wxFileName fn(doc->GetFilename());
    const wxString title =
        !fn.GetName().empty() ? fn.GetName() : wxFileName::StripExtension(doc->GetTitle());
    auto* subframe = new ProjectDocChildFrame(doc, view, wxGetApp().GetMainFrame(), wxID_ANY, title,
                                              wxDefaultPosition, windowSize, wxDEFAULT_FRAME_STYLE);
    subframe->Show(false);
    if (wxGetApp().GetMainFrame()->IsMaximized())
        {
        subframe->Maximize();
        subframe->SetSize(subframe->GetSize());
        }

    wxIcon appIcon;
    appIcon.CopyFromBitmap(wxGetApp()
                               .GetResourceManager()
                               .GetSVG(L"ribbon/app-logo.svg")
                               .GetBitmap(subframe->FromDIP(wxSize{ 32, 32 })));
    subframe->SetIcon(appIcon);
    subframe->CenterOnScreen();

    // list control-specific menu for copying
    subframe->m_copyMenu.Append(wxID_COPY, _(L"Copy") + L"\tCtrl+C");
    subframe->m_copyMenu.Append(XRCID("ID_COPY_WITH_COLUMN_HEADERS"),
                                _(L"Copy with Column Headers"));
    subframe->m_copyMenu.Append(XRCID("ID_COPY_FIRST_COLUMN"),
                                _(L"Copy (First Column Only)") + L"\tShift+Ctrl+C");
    subframe->m_copyMenu.Append(XRCID("ID_COPY_ALL"), _(L"Copy All"));

    // bar chart sorting menu
    auto* item = new wxMenuItem(&subframe->m_graphSortMenu, XRCID("ID_SORT_ASCENDING"),
                                _(L"Sort Ascending"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-sort-ascending.svg"));
    subframe->m_graphSortMenu.Append(item);

    item = new wxMenuItem(&subframe->m_graphSortMenu, XRCID("ID_SORT_DESCENDING"),
                          _(L"Sort Descending"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-sort-descending.svg"));
    subframe->m_graphSortMenu.Append(item);

    // graph background menu
    // color submenu
    auto* graphBackgroundColorSubMenu{ new wxMenu{} };
    item =
        new wxMenuItem(graphBackgroundColorSubMenu, XRCID("ID_EDIT_GRAPH_BKCOLOR"), _(L"Color..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/color-wheel.svg"));
    graphBackgroundColorSubMenu->Append(item);

    graphBackgroundColorSubMenu->Append(new wxMenuItem(graphBackgroundColorSubMenu,
                                                       XRCID("ID_GRAPH_BKCOLOR_FADE"), _(L"Fade"),
                                                       wxString{}, wxITEM_CHECK));

    subframe->m_graphBackgroundMenu.AppendSubMenu(graphBackgroundColorSubMenu,
                                                  _(L"Background Color"));

    subframe->m_graphBackgroundMenu.AppendSeparator();

    // plot color submenu
    auto* plotColorSubMenu{ new wxMenu{} };
    item = new wxMenuItem(plotColorSubMenu, XRCID("ID_EDIT_PLOT_BKCOLOR"), _(L"Color..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/color-wheel.svg"));
    plotColorSubMenu->Append(item);

    item =
        new wxMenuItem(plotColorSubMenu, XRCID("ID_EDIT_PLOT_BKCOLOR_OPACITY"), _(L"Opacity..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/opacity.svg"));
    plotColorSubMenu->Append(item);

    subframe->m_graphBackgroundMenu.AppendSubMenu(plotColorSubMenu, _(L"Plot Color"));

    // image submenu
    auto* graphBackgroundImageSubMenu{ new wxMenu{} };
    item =
        new wxMenuItem(graphBackgroundImageSubMenu, XRCID("ID_EDIT_PLOT_BKIMAGE"), _(L"Image..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    graphBackgroundImageSubMenu->Append(item);

    item = new wxMenuItem(graphBackgroundImageSubMenu, XRCID("ID_MERGE_PLOT_BKIMAGES"),
                          _(L"Merge Images..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/photos-merge.svg"));
    graphBackgroundImageSubMenu->Append(item);

    graphBackgroundImageSubMenu->Append(new wxMenuItem(
        graphBackgroundImageSubMenu, XRCID("ID_EDIT_PLOT_BKIMAGE_OPACITY"), _(L"Opacity...")));

    auto* graphBackgroundImageEffectSubMenu{ new wxMenu{} };

    graphBackgroundImageEffectSubMenu->Append(
        new wxMenuItem(graphBackgroundImageEffectSubMenu, XRCID("ID_PLOT_BKIMAGE_EFFECT_NO_EFFECT"),
                       _(L"No Effect"), wxString{}, wxITEM_CHECK));
    graphBackgroundImageEffectSubMenu->Append(
        new wxMenuItem(graphBackgroundImageEffectSubMenu, XRCID("ID_PLOT_BKIMAGE_EFFECT_GRAYSCALE"),
                       _(L"Grayscale"), wxString{}, wxITEM_CHECK));
    graphBackgroundImageEffectSubMenu->Append(new wxMenuItem(
        graphBackgroundImageEffectSubMenu, XRCID("ID_PLOT_BKIMAGE_EFFECT_BLUR_HORIZONTALLY"),
        _(L"Blur Horizontally"), wxString{}, wxITEM_CHECK));
    graphBackgroundImageEffectSubMenu->Append(new wxMenuItem(
        graphBackgroundImageEffectSubMenu, XRCID("ID_PLOT_BKIMAGE_EFFECT_BLUR_VERTICALLY"),
        _(L"Blur Vertically"), wxString{}, wxITEM_CHECK));
    graphBackgroundImageEffectSubMenu->Append(
        new wxMenuItem(graphBackgroundImageEffectSubMenu, XRCID("ID_PLOT_BKIMAGE_EFFECT_SEPIA"),
                       _(L"Sepia"), wxString{}, wxITEM_CHECK));
    graphBackgroundImageEffectSubMenu->Append(new wxMenuItem(
        graphBackgroundImageEffectSubMenu, XRCID("ID_PLOT_BKIMAGE_EFFECT_FROSTED_GLASS"),
        _(L"Frosted Glass"), wxString{}, wxITEM_CHECK));
    graphBackgroundImageEffectSubMenu->Append(new wxMenuItem(
        graphBackgroundImageEffectSubMenu, XRCID("ID_PLOT_BKIMAGE_EFFECT_OIL_PAINTING"),
        _(L"Oil Painting"), wxString{}, wxITEM_CHECK));
    graphBackgroundImageEffectSubMenu->Append(new wxMenuItem(
        graphBackgroundImageEffectSubMenu, XRCID("ID_PLOT_BKIMAGE_EFFECT_COLOR_BALANCE"),
        _(L"Color Balance"), wxString{}, wxITEM_CHECK));
    graphBackgroundImageEffectSubMenu->Append(
        new wxMenuItem(graphBackgroundImageEffectSubMenu, XRCID("ID_PLOT_BKIMAGE_EFFECT_DESPECKLE"),
                       _(L"Despeckle"), wxString{}, wxITEM_CHECK));
    graphBackgroundImageEffectSubMenu->Append(
        new wxMenuItem(graphBackgroundImageEffectSubMenu, XRCID("ID_PLOT_BKIMAGE_EFFECT_SHARPEN"),
                       _(L"Sharpen"), wxString{}, wxITEM_CHECK));

    graphBackgroundImageSubMenu->AppendSubMenu(graphBackgroundImageEffectSubMenu, _(L"Effects"));

    auto* graphBackgroundImageFitSubMenu{ new wxMenu{} };

    graphBackgroundImageFitSubMenu->Append(
        new wxMenuItem(graphBackgroundImageFitSubMenu, XRCID("ID_PLOT_BKIMAGE_FIT_CROP_AND_CENTER"),
                       _(L"Crop && Center"), wxString{}, wxITEM_CHECK));
    graphBackgroundImageFitSubMenu->Append(
        new wxMenuItem(graphBackgroundImageFitSubMenu, XRCID("ID_PLOT_BKIMAGE_FIT_SHRINK"),
                       _(L"Shrink to Fit"), wxString{}, wxITEM_CHECK));

    graphBackgroundImageSubMenu->AppendSubMenu(graphBackgroundImageFitSubMenu,
                                               // TRANSLATORS: How an image is adjusted to fit
                                               // inside an area
                                               _(L"Fit"));

    item = new wxMenuItem(graphBackgroundImageSubMenu, XRCID("ID_EDIT_PLOT_BKIMAGE_REMOVE"),
                          _(L"Remove Image"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/delete.svg"));
    graphBackgroundImageSubMenu->Append(item);

    subframe->m_graphBackgroundMenu.AppendSubMenu(graphBackgroundImageSubMenu, _(L"Plot Image"));

    // Raygor styles
    subframe->m_raygorStyleMenu.Append(new wxMenuItem(&subframe->m_raygorStyleMenu,
                                                      XRCID("ID_EDIT_GRAPH_RAYGOR_ORIGINAL"),
                                                      _(L"Original"), wxString{}, wxITEM_CHECK));
    subframe->m_raygorStyleMenu.Append(
        new wxMenuItem(&subframe->m_raygorStyleMenu, XRCID("ID_EDIT_GRAPH_RAYGOR_BALDWIN_KAUFMAN"),
                       _DT(L"Baldwin-Kaufman"), wxString{}, wxITEM_CHECK));
    subframe->m_raygorStyleMenu.Append(new wxMenuItem(&subframe->m_raygorStyleMenu,
                                                      XRCID("ID_EDIT_GRAPH_RAYGOR_MODERN"),
                                                      _(L"Modern"), wxString{}, wxITEM_CHECK));

    // histogram bar labels
    subframe->m_histoBarLabelsMenu.Append(new wxMenuItem(&subframe->m_histoBarLabelsMenu,
                                                         XRCID("ID_HISTOBAR_LABELS_COUNT"),
                                                         _(L"Counts"), wxString{}, wxITEM_CHECK));
    subframe->m_histoBarLabelsMenu.Append(
        new wxMenuItem(&subframe->m_histoBarLabelsMenu, XRCID("ID_HISTOBAR_LABELS_PERCENTAGE"),
                       _(L"Percentages"), wxString{}, wxITEM_CHECK));
    subframe->m_histoBarLabelsMenu.Append(new wxMenuItem(
        &subframe->m_histoBarLabelsMenu, XRCID("ID_HISTOBAR_LABELS_COUNT_AND_PERCENT"),
        _(L"Counts && Percentages"), wxString{}, wxITEM_CHECK));
    subframe->m_histoBarLabelsMenu.Append(
        new wxMenuItem(&subframe->m_histoBarLabelsMenu, XRCID("ID_HISTOBAR_NO_LABELS"),
                       _(L"No labels"), wxString{}, wxITEM_CHECK));

    // zoom
    item = new wxMenuItem(&subframe->m_zoomMenu, wxID_ZOOM_IN, _(L"Zoom In"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/zoom-in.svg"));
    subframe->m_zoomMenu.Append(item);

    item = new wxMenuItem(&subframe->m_zoomMenu, wxID_ZOOM_OUT, _(L"Zoom Out"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/zoom-out.svg"));
    subframe->m_zoomMenu.Append(item);

    item = new wxMenuItem(&subframe->m_zoomMenu, wxID_ZOOM_FIT, _(L"Reset Zoom"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/zoom-to-fit.svg"));
    subframe->m_zoomMenu.Append(item);

    // bar orientation
    item = new wxMenuItem(&subframe->m_barOrientationMenu, XRCID("ID_BAR_HORIZONTAL"),
                          _(L"Horizontal"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-chart.svg"));
    subframe->m_barOrientationMenu.Append(item);

    item =
        new wxMenuItem(&subframe->m_barOrientationMenu, XRCID("ID_BAR_VERTICAL"), _(L"Vertical"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/histogram.svg"));
    subframe->m_barOrientationMenu.Append(item);

    // bar style menu
    item = new wxMenuItem(&subframe->m_barStyleMenu, XRCID("ID_EDIT_BAR_COLOR"), _(L"Color..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/color-wheel.svg"));
    subframe->m_barStyleMenu.Append(item);

    item =
        new wxMenuItem(&subframe->m_barStyleMenu, XRCID("ID_EDIT_BAR_OPACITY"), _(L"Opacity..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-top-to-bottom.svg"));
    subframe->m_barStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_barStyleMenu, XRCID("ID_BAR_SELECT_STIPPLE_IMAGE"),
                          _(L"Select Stipple Image..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    subframe->m_barStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_barStyleMenu, XRCID("ID_BAR_SELECT_STIPPLE_SHAPE"),
                          _(L"Select Stipple Shape..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/apple.svg"));
    subframe->m_barStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_barStyleMenu, XRCID("ID_BAR_SELECT_COMMON_IMAGE"),
                          _(L"Select Common Image..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    subframe->m_barStyleMenu.Append(item);

    subframe->m_barStyleMenu.AppendSeparator();

    item = new wxMenuItem(&subframe->m_barStyleMenu, XRCID("ID_BAR_STYLE_SOLID"), _(L"Solid"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-solid.svg"));
    subframe->m_barStyleMenu.Append(item);

    item =
        new wxMenuItem(&subframe->m_barStyleMenu, XRCID("ID_BAR_STYLE_GLASS"), _(L"Glass Effect"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-glass.svg"));
    subframe->m_barStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_barStyleMenu, XRCID("ID_BAR_STYLE_BTOT"),
                          _(L"Color fade, Bottom-to-top"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-bottom-to-top.svg"));
    subframe->m_barStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_barStyleMenu, XRCID("ID_BAR_STYLE_TTOB"),
                          _(L"Color Fade, Top-to-bottom"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-top-to-bottom.svg"));
    subframe->m_barStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_barStyleMenu, XRCID("ID_BAR_STYLE_STIPPLE_IMAGE"),
                          _(L"Stipple Image"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    subframe->m_barStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_barStyleMenu, XRCID("ID_BAR_STYLE_STIPPLE_SHAPE"),
                          _(L"Stipple Shape"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/apple.svg"));
    subframe->m_barStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_barStyleMenu, XRCID("ID_BAR_STYLE_WATERCOLOR"),
                          _(L"Watercolor"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/brush.svg"));
    subframe->m_barStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_barStyleMenu, XRCID("ID_BAR_STYLE_THICK_WATERCOLOR"),
                          _(L"Thick Watercolor"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/brush.svg"));
    subframe->m_barStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_barStyleMenu, XRCID("ID_BAR_STYLE_MARKER"), _(L"Marker"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/highlighter.svg"));
    subframe->m_barStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_barStyleMenu, XRCID("ID_BAR_STYLE_COMMON_IMAGE"),
                          _(L"Common Image"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    subframe->m_barStyleMenu.Append(item);

    // histogram bar style menu
    item = new wxMenuItem(&subframe->m_histoBarStyleMenu, XRCID("ID_EDIT_HISTOGRAM_BAR_COLOR"),
                          _(L"Color..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/color-wheel.svg"));
    subframe->m_histoBarStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_histoBarStyleMenu, XRCID("ID_EDIT_HISTOBAR_OPACITY"),
                          _(L"Opacity..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-top-to-bottom.svg"));
    subframe->m_histoBarStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_histoBarStyleMenu, XRCID("ID_HISTOGRAM_BAR_SELECT_BRUSH"),
                          _(L"Select Stipple Image..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    subframe->m_histoBarStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_histoBarStyleMenu,
                          XRCID("ID_HISTOGRAM_BAR_SELECT_STIPPLE_SHAPE"),
                          _(L"Select Stipple Shape..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/apple.svg"));
    subframe->m_histoBarStyleMenu.Append(item);

    item =
        new wxMenuItem(&subframe->m_histoBarStyleMenu,
                       XRCID("ID_HISTOGRAM_BAR_SELECT_COMMON_IMAGE"), _(L"Select Common Image..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    subframe->m_histoBarStyleMenu.Append(item);

    subframe->m_histoBarStyleMenu.AppendSeparator();

    item = new wxMenuItem(&subframe->m_histoBarStyleMenu, XRCID("ID_HISTOGRAM_BAR_STYLE_SOLID"),
                          _(L"Solid"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-solid.svg"));
    subframe->m_histoBarStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_histoBarStyleMenu, XRCID("ID_HISTOGRAM_BAR_STYLE_GLASS"),
                          _(L"Glass Effect"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-glass.svg"));
    subframe->m_histoBarStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_histoBarStyleMenu, XRCID("ID_HISTOGRAM_BAR_STYLE_BTOT"),
                          _(L"Color Fade, Bottom-to-top"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-bottom-to-top.svg"));
    subframe->m_histoBarStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_histoBarStyleMenu, XRCID("ID_HISTOGRAM_BAR_STYLE_TTOB"),
                          _(L"Color Fade, Top-to-bottom"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-top-to-bottom.svg"));
    subframe->m_histoBarStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_histoBarStyleMenu,
                          XRCID("ID_HISTOGRAM_BAR_STYLE_STIPPLE_IMAGE"), _(L"Stipple Image"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    subframe->m_histoBarStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_histoBarStyleMenu,
                          XRCID("ID_HISTOGRAM_BAR_STYLE_STIPPLE_SHAPE"), _(L"Stipple Shape"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/apple.svg"));
    subframe->m_histoBarStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_histoBarStyleMenu,
                          XRCID("ID_HISTOGRAM_BAR_STYLE_WATERCOLOR"), _(L"Watercolor"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/brush.svg"));
    subframe->m_histoBarStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_histoBarStyleMenu,
                          XRCID("ID_HISTOGRAM_BAR_STYLE_THICK_WATERCOLOR"), _(L"Thick Watercolor"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/brush.svg"));
    subframe->m_histoBarStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_histoBarStyleMenu, XRCID("ID_HISTOGRAM_BAR_STYLE_MARKER"),
                          _(L"Marker"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/highlighter.svg"));
    subframe->m_histoBarStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_histoBarStyleMenu,
                          XRCID("ID_HISTOGRAM_BAR_STYLE_COMMON_IMAGE"), _(L"Common Image"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    subframe->m_histoBarStyleMenu.Append(item);

    // box style menu
    item = new wxMenuItem(&subframe->m_boxStyleMenu, XRCID("ID_EDIT_BOX_COLOR"), _(L"Color..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/color-wheel.svg"));
    subframe->m_boxStyleMenu.Append(item);

    item =
        new wxMenuItem(&subframe->m_boxStyleMenu, XRCID("ID_EDIT_BOX_OPACITY"), _(L"Opacity..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-top-to-bottom.svg"));
    subframe->m_boxStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_boxStyleMenu, XRCID("ID_BOX_SELECT_STIPPLE_IMAGE"),
                          _(L"Select Stipple Image..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    subframe->m_boxStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_boxStyleMenu, XRCID("ID_BOX_BAR_SELECT_STIPPLE_SHAPE"),
                          _(L"Select Stipple Shape..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/apple.svg"));
    subframe->m_boxStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_boxStyleMenu, XRCID("ID_BOX_SELECT_COMMON_IMAGE"),
                          _(L"Select Common Image..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    subframe->m_boxStyleMenu.Append(item);

    subframe->m_boxStyleMenu.AppendSeparator();

    item = new wxMenuItem(&subframe->m_boxStyleMenu, XRCID("ID_BOX_STYLE_SOLID"), _(L"Solid"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-solid.svg"));
    subframe->m_boxStyleMenu.Append(item);

    item =
        new wxMenuItem(&subframe->m_boxStyleMenu, XRCID("ID_BOX_STYLE_GLASS"), _(L"Glass Effect"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-glass.svg"));
    subframe->m_boxStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_boxStyleMenu, XRCID("ID_BOX_STYLE_LTOR"),
                          _(L"Color Fade, Left-to-right"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-left-to-right.svg"));
    subframe->m_boxStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_boxStyleMenu, XRCID("ID_BOX_STYLE_RTOL"),
                          _(L"Color Fade, Right-to-left"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bar-right-to-left.svg"));
    subframe->m_boxStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_boxStyleMenu, XRCID("ID_BOX_STYLE_STIPPLE_IMAGE"),
                          _(L"Stipple Image"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    subframe->m_boxStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_boxStyleMenu, XRCID("ID_BOX_STYLE_STIPPLE_SHAPE"),
                          _(L"Stipple Shape"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/apple.svg"));
    subframe->m_boxStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_boxStyleMenu, XRCID("ID_BOX_STYLE_WATERCOLOR"),
                          _(L"Watercolor"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/brush.svg"));
    subframe->m_boxStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_boxStyleMenu, XRCID("ID_BOX_STYLE_THICK_WATERCOLOR"),
                          _(L"Thick Watercolor"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/brush.svg"));
    subframe->m_boxStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_boxStyleMenu, XRCID("ID_BOX_STYLE_MARKER"), _(L"Marker"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/highlighter.svg"));
    subframe->m_boxStyleMenu.Append(item);

    item = new wxMenuItem(&subframe->m_boxStyleMenu, XRCID("ID_BOX_STYLE_COMMON_IMAGE"),
                          _(L"Common Image"));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/image.svg"));
    subframe->m_boxStyleMenu.Append(item);

    // graph fonts
    item =
        new wxMenuItem(&subframe->m_graphFontsMenu, XRCID("ID_EDIT_X_AXIS_FONT"), _(L"X Axis..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/x-axis.svg"));
    subframe->m_graphFontsMenu.Append(item);

    item =
        new wxMenuItem(&subframe->m_graphFontsMenu, XRCID("ID_EDIT_Y_AXIS_FONT"), _(L"Y Axis..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/y-axis.svg"));
    subframe->m_graphFontsMenu.Append(item);

    subframe->m_graphFontsMenu.AppendSeparator();

    item = new wxMenuItem(&subframe->m_graphFontsMenu, XRCID("ID_EDIT_TOP_TITLES_FONT"),
                          _(L"Top Titles..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/top-titles.svg"));
    subframe->m_graphFontsMenu.Append(item);

    item = new wxMenuItem(&subframe->m_graphFontsMenu, XRCID("ID_EDIT_BOTTOM_TITLES_FONT"),
                          _(L"Bottom Titles..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/bottom-titles.svg"));
    subframe->m_graphFontsMenu.Append(item);

    item = new wxMenuItem(&subframe->m_graphFontsMenu, XRCID("ID_EDIT_LEFT_TITLES_FONT"),
                          _(L"Left Titles..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/left-titles.svg"));
    subframe->m_graphFontsMenu.Append(item);

    item = new wxMenuItem(&subframe->m_graphFontsMenu, XRCID("ID_EDIT_RIGHT_TITLES_FONT"),
                          _(L"Right Titles..."));
    item->SetBitmap(wxGetApp().GetResourceManager().GetSVG(L"ribbon/right-titles.svg"));
    subframe->m_graphFontsMenu.Append(item);

    // document indexing menus
    subframe->m_lineEndsMenu.Append(new wxMenuItem(
        &subframe->m_lineEndsMenu, XRCID("ID_LE_ONLY_AFTER_VALID_SENTENCE"),
        _(L"Only begin a new paragraph if following a valid sentence"), wxString{}, wxITEM_CHECK));
    subframe->m_lineEndsMenu.Append(
        new wxMenuItem(&subframe->m_lineEndsMenu, XRCID("ID_LE_ALWAYS_NEW_PARAGRAPH"),
                       _(L"Always begin a new paragraph"), wxString{}, wxITEM_CHECK));

    subframe->m_longSentencesMenu.Append(
        new wxMenuItem(&subframe->m_longSentencesMenu, XRCID("ID_LS_LONGER_THAN"),
                       _(L"If longer than..."), wxString{}, wxITEM_CHECK));
    subframe->m_longSentencesMenu.Append(
        new wxMenuItem(&subframe->m_longSentencesMenu, XRCID("ID_LS_OUTLIER_RANGE"),
                       _(L"Outside outlier range"), wxString{}, wxITEM_CHECK));

    subframe->m_textExclusionMenu.Append(
        new wxMenuItem(&subframe->m_textExclusionMenu, XRCID("ID_TE_ALL_INCOMPLETE"),
                       _(L"Exclude all incomplete sentences"), wxString{}, wxITEM_CHECK));
    subframe->m_textExclusionMenu.Append(
        new wxMenuItem(&subframe->m_textExclusionMenu, XRCID("ID_TE_NO_EXCLUDE"),
                       _(L"Do not exclude any text"), wxString{}, wxITEM_CHECK));
    subframe->m_textExclusionMenu.Append(new wxMenuItem(
        &subframe->m_textExclusionMenu, XRCID("ID_TE_ALL_INCOMPLETE_EXCEPT_HEADERS"),
        _(L"Exclude all incomplete sentences, except headings"), wxString{}, wxITEM_CHECK));

    subframe->m_exclusionTagsMenu.Append(
        new wxMenuItem(&subframe->m_exclusionTagsMenu, XRCID("ID_EXCLUSION_TAGS_NOT_ENABLED"),
                       _(L"Not enabled"), wxString{}, wxITEM_CHECK));
    subframe->m_exclusionTagsMenu.Append(new wxMenuItem(&subframe->m_exclusionTagsMenu,
                                                        XRCID("ID_EXCLUSION_TAGS_CAROTS"),
                                                        _(L"^ and ^"), wxString{}, wxITEM_CHECK));
    subframe->m_exclusionTagsMenu.Append(new wxMenuItem(&subframe->m_exclusionTagsMenu,
                                                        XRCID("ID_EXCLUSION_TAGS_ANGLES"),
                                                        _(L"< and >"), wxString{}, wxITEM_CHECK));
    subframe->m_exclusionTagsMenu.Append(new wxMenuItem(&subframe->m_exclusionTagsMenu,
                                                        XRCID("ID_EXCLUSION_TAGS_BRACES"),
                                                        _(L"[ and ]"), wxString{}, wxITEM_CHECK));
    subframe->m_exclusionTagsMenu.Append(new wxMenuItem(&subframe->m_exclusionTagsMenu,
                                                        XRCID("ID_EXCLUSION_TAGS_CURLIES"),
                                                        _(L"{ and }"), wxString{}, wxITEM_CHECK));
    subframe->m_exclusionTagsMenu.Append(new wxMenuItem(&subframe->m_exclusionTagsMenu,
                                                        XRCID("ID_EXCLUSION_TAGS_PARANS"),
                                                        _(L"( and )"), wxString{}, wxITEM_CHECK));

    subframe->m_numeralSyllabicationMenu.Append(
        new wxMenuItem(&subframe->m_numeralSyllabicationMenu, XRCID("ID_NUMSYL_ONE"),
                       _(L"Numerals are one syllable"), wxString{}, wxITEM_CHECK));
    subframe->m_numeralSyllabicationMenu.Append(
        new wxMenuItem(&subframe->m_numeralSyllabicationMenu, XRCID("ID_NUMSYL_EACH_DIGIT"),
                       _(L"Sound out each digit"), wxString{}, wxITEM_CHECK));

    return subframe;
    }

//-------------------------------------------------------
bool BaseProjectView::OnClose(bool deleteWindow)
    {
    if (!wxView::OnClose(deleteWindow))
        {
        wxLogWarning(L"Project view not successfully closed.");
        return false;
        }

    if (deleteWindow)
        {
        m_frame->Destroy();
        }
    SetFrame(nullptr);
    Activate(false);

    if (wxGetApp().GetDocumentCount() == 1)
        {
        // show the empty mainframe when the last document is being closed
        wxArrayString mruFiles;
        for (size_t i = 0; i < wxGetApp().GetDocManager()->GetFileHistory()->GetCount(); ++i)
            {
            mruFiles.Add(wxGetApp().GetDocManager()->GetFileHistory()->GetHistoryFile(i));
            }
        wxGetApp().GetMainFrameEx()->GetStartPage()->SetMRUList(mruFiles);
        wxGetApp().GetMainFrame()->CenterOnScreen();
        wxGetApp().GetMainFrame()->Show();
        }

    return true;
    }

//-------------------------------------------------------
void BaseProjectView::Present()
    {
#ifdef __WXOSX__
    // Associate the menu with this view.
    if (m_frame->GetMenuBar() == nullptr)
        {
        m_frame->SetMenuBar(GetMenuBar());
        }
#endif
    if (m_frame->GetMenuBar() != nullptr)
        {
        wxGetApp().GetDocManager()->FileHistoryUseMenu(m_frame->GetMenuBar()->GetMenu(0));
        if (m_frame->GetMenuBar()->GetMenu(0)->FindItem(wxID_FILE1) == nullptr)
            {
            wxGetApp().GetDocManager()->FileHistoryAddFilesToMenu(
                m_frame->GetMenuBar()->GetMenu(0));
            }
        }

    auto* doc = dynamic_cast<BaseProjectDoc*>(GetDocument());

    // update menus that might change due to the project's language being changed
    if (GetMenuBar() != nullptr)
        {
        int readMenuIndex = GetMenuBar()->FindMenu(_(L"Readability"));
        // add menu if needed, although this shouldn't happen
        if (readMenuIndex == wxNOT_FOUND)
            {
            GetMenuBar()->Append(new wxMenu(), _(L"Readability"));
            readMenuIndex = static_cast<int>(GetMenuBar()->GetMenuCount() - 1);
            }
        wxMenu* readMenu = GetMenuBar()->GetMenu(readMenuIndex);
        if (readMenu != nullptr)
            {
            // remove all the submenus
            wxMenuItemList menuItems = readMenu->GetMenuItems();
            for (const auto& menuItem : menuItems)
                {
                readMenu->Destroy(menuItem);
                }
            // add all the submenus for the different groupings
            auto* primaryMenu = new wxMenu;
            auto* secondaryMenu = new wxMenu;
            auto* adultMenu = new wxMenu;
            auto* secondLanguageMenu = new wxMenu;
            GetDocFrame()->m_customTestsRegularMenu = new wxMenu;
            GetDocFrame()->m_testsBundleRegularMenu = new wxMenu;
            readMenu->AppendSubMenu(primaryMenu, _(L"Primary-age Reading (5-12 years old)"));
            readMenu->AppendSubMenu(secondaryMenu,
                                    _(L"Secondary-age Reading (13 years old and above)"));
            readMenu->AppendSubMenu(adultMenu, _(L"Adult Reading (technical forms and documents)"));
            readMenu->AppendSubMenu(secondLanguageMenu, _(L"Second Language Reading"));
            MainFrame::FillReadabilityMenu(primaryMenu, secondaryMenu, adultMenu,
                                           secondLanguageMenu, doc);
            // add the custom test menu to the bottom
            readMenu->AppendSeparator();
            readMenu->AppendSubMenu(GetDocFrame()->m_customTestsRegularMenu, _(L"Custom Tests"));
            MainFrame::FillMenuWithCustomTests(GetDocFrame()->m_customTestsRegularMenu, doc, true);
            // add test bundles
            readMenu->AppendSeparator();
            readMenu->AppendSubMenu(GetDocFrame()->m_testsBundleRegularMenu, _(L"Bundles"));
            MainFrame::FillMenuWithTestBundles(GetDocFrame()->m_testsBundleRegularMenu, doc, true);

            // remove any submenus that don't have anything in them
            menuItems = readMenu->GetMenuItems();
            for (const auto& menuItem : menuItems)
                {
                if (menuItem->IsSubMenu() && ((menuItem->GetSubMenu() == nullptr) ||
                                              menuItem->GetSubMenu()->GetMenuItemCount() == 0))
                    {
                    readMenu->Destroy(menuItem);
                    }
                }
            }
        }
    // fill the menus
    MainFrame::FillReadabilityMenu(
        &GetDocFrame()->m_primaryAgeTestsMenu, &GetDocFrame()->m_secondaryAgeTestsMenu,
        &GetDocFrame()->m_adultTestsMenu, &GetDocFrame()->m_secondLanguageTestsMenu, doc);
    MainFrame::FillMenuWithCustomTests(&GetDocFrame()->m_customTestsMenu, doc, true);
    MainFrame::FillMenuWithTestBundles(&GetDocFrame()->m_testsBundleMenu, doc, true);
    ReadabilityApp::FillWordListsMenu(GetDocFrame()->m_wordListMenu);
    ReadabilityApp::FillGradeScalesMenu(GetDocFrame()->m_gradeScaleMenu);
    const readability::grade_scale gs = doc->GetReadabilityMessageCatalog().GetGradeScale();
    if (gs == readability::grade_scale::k12_plus_united_states)
        {
        GetDocFrame()->m_gradeScaleMenu.Check(XRCID("ID_K12_US"), true);
        }
    else if (gs == readability::grade_scale::k12_plus_newfoundland_and_labrador)
        {
        GetDocFrame()->m_gradeScaleMenu.Check(XRCID("ID_K12_NEWFOUNDLAND"), true);
        }
    else if (gs == readability::grade_scale::k12_plus_british_columbia)
        {
        GetDocFrame()->m_gradeScaleMenu.Check(XRCID("ID_K12_BC"), true);
        }
    else if (gs == readability::grade_scale::k12_plus_newbrunswick)
        {
        GetDocFrame()->m_gradeScaleMenu.Check(XRCID("ID_K12_NEW_BRUNSWICK"), true);
        }
    else if (gs == readability::grade_scale::k12_plus_nova_scotia)
        {
        GetDocFrame()->m_gradeScaleMenu.Check(XRCID("ID_K12_NOVA_SCOTIA"), true);
        }
    else if (gs == readability::grade_scale::k12_plus_ontario)
        {
        GetDocFrame()->m_gradeScaleMenu.Check(XRCID("ID_K12_ONTARIO"), true);
        }
    else if (gs == readability::grade_scale::k12_plus_saskatchewan)
        {
        GetDocFrame()->m_gradeScaleMenu.Check(XRCID("ID_K12_SASKATCHEWAN"), true);
        }
    else if (gs == readability::grade_scale::k12_plus_prince_edward_island)
        {
        GetDocFrame()->m_gradeScaleMenu.Check(XRCID("ID_K12_PE"), true);
        }
    else if (gs == readability::grade_scale::k12_plus_manitoba)
        {
        GetDocFrame()->m_gradeScaleMenu.Check(XRCID("ID_K12_MANITOBA"), true);
        }
    else if (gs == readability::grade_scale::k12_plus_northwest_territories)
        {
        GetDocFrame()->m_gradeScaleMenu.Check(XRCID("ID_K12_NT"), true);
        }
    else if (gs == readability::grade_scale::k12_plus_alberta)
        {
        GetDocFrame()->m_gradeScaleMenu.Check(XRCID("ID_K12_ALBERTA"), true);
        }
    else if (gs == readability::grade_scale::k12_plus_nunavut)
        {
        GetDocFrame()->m_gradeScaleMenu.Check(XRCID("ID_K12_NUNAVUT"), true);
        }
    else if (gs == readability::grade_scale::quebec)
        {
        GetDocFrame()->m_gradeScaleMenu.Check(XRCID("ID_QUEBEC"), true);
        }
    else if (gs == readability::grade_scale::key_stages_england_wales)
        {
        GetDocFrame()->m_gradeScaleMenu.Check(XRCID("ID_ENGLAND"), true);
        }

    ReadabilityApp::FillBlankGraphsMenu(GetDocFrame()->m_blankGraphMenu);
    if (GetMenuBar() != nullptr)
        {
        const wxMenuItem* exampleMenuItem = GetMenuBar()->FindItem(XRCID("ID_EXAMPLES"));
        if (exampleMenuItem != nullptr && exampleMenuItem->GetSubMenu() != nullptr)
            {
            wxGetApp().GetMainFrameEx()->AddExamplesToMenu(exampleMenuItem->GetSubMenu());
            }
        }
    wxGetApp().GetMainFrameEx()->AddExamplesToMenu(&GetDocFrame()->m_exampleMenu);

    if (doc->GetRaygorStyle() == Wisteria::Graphs::RaygorStyle::Original)
        {
        GetDocFrame()->m_raygorStyleMenu.Check(XRCID("ID_EDIT_GRAPH_RAYGOR_ORIGINAL"), true);
        }
    else if (doc->GetRaygorStyle() == Wisteria::Graphs::RaygorStyle::BaldwinKaufman)
        {
        GetDocFrame()->m_raygorStyleMenu.Check(XRCID("ID_EDIT_GRAPH_RAYGOR_BALDWIN_KAUFMAN"), true);
        }
    else if (doc->GetRaygorStyle() == Wisteria::Graphs::RaygorStyle::Modern)
        {
        GetDocFrame()->m_raygorStyleMenu.Check(XRCID("ID_EDIT_GRAPH_RAYGOR_MODERN"), true);
        }

    // other menu items with checkmarks that need to be checked here
    wxMenuItem* fadeOption =
        GetDocFrame()->m_graphBackgroundMenu.FindItem(XRCID("ID_GRAPH_BKCOLOR_FADE"));
    if (fadeOption != nullptr)
        {
        fadeOption->Check(doc->GetGraphBackGroundLinearGradient());
        }

    if (auto* tempMenuItem = GetDocFrame()->m_graphBackgroundMenu.FindItem(
            XRCID("ID_PLOT_BKIMAGE_EFFECT_NO_EFFECT"));
        tempMenuItem != nullptr)
        {
        tempMenuItem->Check(doc->GetPlotBackGroundImageEffect() == Wisteria::ImageEffect::NoEffect);
        }
    if (auto* tempMenuItem = GetDocFrame()->m_graphBackgroundMenu.FindItem(
            XRCID("ID_PLOT_BKIMAGE_EFFECT_GRAYSCALE"));
        tempMenuItem != nullptr)
        {
        tempMenuItem->Check(doc->GetPlotBackGroundImageEffect() ==
                            Wisteria::ImageEffect::Grayscale);
        }
    if (auto* tempMenuItem = GetDocFrame()->m_graphBackgroundMenu.FindItem(
            XRCID("ID_PLOT_BKIMAGE_EFFECT_BLUR_HORIZONTALLY"));
        tempMenuItem != nullptr)
        {
        tempMenuItem->Check(doc->GetPlotBackGroundImageEffect() ==
                            Wisteria::ImageEffect::BlurHorizontal);
        }
    if (auto* tempMenuItem = GetDocFrame()->m_graphBackgroundMenu.FindItem(
            XRCID("ID_PLOT_BKIMAGE_EFFECT_BLUR_VERTICALLY"));
        tempMenuItem != nullptr)
        {
        tempMenuItem->Check(doc->GetPlotBackGroundImageEffect() ==
                            Wisteria::ImageEffect::BlurVertical);
        }
    if (auto* tempMenuItem =
            GetDocFrame()->m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_SEPIA"));
        tempMenuItem != nullptr)
        {
        tempMenuItem->Check(doc->GetPlotBackGroundImageEffect() == Wisteria::ImageEffect::Sepia);
        }
    if (auto* tempMenuItem = GetDocFrame()->m_graphBackgroundMenu.FindItem(
            XRCID("ID_PLOT_BKIMAGE_EFFECT_FROSTED_GLASS"));
        tempMenuItem != nullptr)
        {
        tempMenuItem->Check(doc->GetPlotBackGroundImageEffect() ==
                            Wisteria::ImageEffect::FrostedGlass);
        }
    if (auto* tempMenuItem = GetDocFrame()->m_graphBackgroundMenu.FindItem(
            XRCID("ID_PLOT_BKIMAGE_EFFECT_OIL_PAINTING"));
        tempMenuItem != nullptr)
        {
        tempMenuItem->Check(doc->GetPlotBackGroundImageEffect() ==
                            Wisteria::ImageEffect::OilPainting);
        }
    if (auto* tempMenuItem = GetDocFrame()->m_graphBackgroundMenu.FindItem(
            XRCID("ID_PLOT_BKIMAGE_EFFECT_COLOR_BALANCE"));
        tempMenuItem != nullptr)
        {
        tempMenuItem->Check(doc->GetPlotBackGroundImageEffect() ==
                            Wisteria::ImageEffect::ColorBalance);
        }
    if (auto* tempMenuItem = GetDocFrame()->m_graphBackgroundMenu.FindItem(
            XRCID("ID_PLOT_BKIMAGE_EFFECT_DESPECKLE"));
        tempMenuItem != nullptr)
        {
        tempMenuItem->Check(doc->GetPlotBackGroundImageEffect() ==
                            Wisteria::ImageEffect::Despeckle);
        }
    if (auto* tempMenuItem =
            GetDocFrame()->m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_EFFECT_SHARPEN"));
        tempMenuItem != nullptr)
        {
        tempMenuItem->Check(doc->GetPlotBackGroundImageEffect() == Wisteria::ImageEffect::Sharpen);
        }

    if (auto* tempMenuItem = GetDocFrame()->m_graphBackgroundMenu.FindItem(
            XRCID("ID_PLOT_BKIMAGE_FIT_CROP_AND_CENTER"));
        tempMenuItem != nullptr)
        {
        tempMenuItem->Check(doc->GetPlotBackGroundImageFit() == Wisteria::ImageFit::CropAndCenter);
        }
    if (auto* tempMenuItem =
            GetDocFrame()->m_graphBackgroundMenu.FindItem(XRCID("ID_PLOT_BKIMAGE_FIT_SHRINK"));
        tempMenuItem != nullptr)
        {
        tempMenuItem->Check(doc->GetPlotBackGroundImageFit() == Wisteria::ImageFit::Shrink);
        }

        // histogram bin labels
        {
        for (size_t i = 0; i < GetDocFrame()->m_histoBarLabelsMenu.GetMenuItemCount(); ++i)
            {
            GetDocFrame()->m_histoBarLabelsMenu.FindItemByPosition(i)->Check(false);
            }
        wxMenuItem* item = GetDocFrame()->m_histoBarLabelsMenu.FindItem(
            (doc->GetHistogramBinLabelDisplay() == Wisteria::BinLabelDisplay::BinPercentage) ?
                XRCID("ID_HISTOBAR_LABELS_PERCENTAGE") :
            (doc->GetHistogramBinLabelDisplay() == Wisteria::BinLabelDisplay::BinValue) ?
                XRCID("ID_HISTOBAR_LABELS_COUNT") :
            (doc->GetHistogramBinLabelDisplay() ==
             Wisteria::BinLabelDisplay::BinValueAndPercentage) ?
                XRCID("ID_HISTOBAR_LABELS_COUNT_AND_PERCENT") :
                XRCID("ID_HISTOBAR_NO_LABELS"));
        if (item != nullptr)
            {
            item->Check(true);
            }
        }
        // long sentences
        {
        for (size_t i = 0; i < GetDocFrame()->m_longSentencesMenu.GetMenuItemCount(); ++i)
            {
            GetDocFrame()->m_longSentencesMenu.FindItemByPosition(i)->Check(false);
            }
        wxMenuItem* item = GetDocFrame()->m_longSentencesMenu.FindItem(
            (doc->GetLongSentenceMethod() == LongSentence::LongerThanSpecifiedLength) ?
                XRCID("ID_LS_LONGER_THAN") :
                XRCID("ID_LS_OUTLIER_RANGE"));
        if (item != nullptr)
            {
            item->Check(true);
            }
        }
        // paragraph parsing
        {
        for (size_t i = 0; i < GetDocFrame()->m_lineEndsMenu.GetMenuItemCount(); ++i)
            {
            GetDocFrame()->m_lineEndsMenu.FindItemByPosition(i)->Check(false);
            }
        wxMenuItem* item = GetDocFrame()->m_lineEndsMenu.FindItem(
            (doc->GetParagraphsParsingMethod() ==
             ParagraphParse::OnlySentenceTerminatedNewLinesAreParagraphs) ?
                XRCID("ID_LE_ONLY_AFTER_VALID_SENTENCE") :
                XRCID("ID_LE_ALWAYS_NEW_PARAGRAPH"));
        if (item != nullptr)
            {
            item->Check(true);
            }
        }
        // text exclusion
        {
        for (size_t i = 0; i < GetDocFrame()->m_textExclusionMenu.GetMenuItemCount(); ++i)
            {
            GetDocFrame()->m_textExclusionMenu.FindItemByPosition(i)->Check(false);
            }
        wxMenuItem* item = GetDocFrame()->m_textExclusionMenu.FindItem(
            (doc->GetInvalidSentenceMethod() == InvalidSentence::ExcludeFromAnalysis) ?
                XRCID("ID_TE_ALL_INCOMPLETE") :
            (doc->GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences) ?
                XRCID("ID_TE_NO_EXCLUDE") :
                XRCID("ID_TE_ALL_INCOMPLETE_EXCEPT_HEADERS"));
        if (item != nullptr)
            {
            item->Check(true);
            }
        }
        // tag exclusion
        {
        for (size_t i = 0; i < GetDocFrame()->m_exclusionTagsMenu.GetMenuItemCount(); ++i)
            {
            GetDocFrame()->m_exclusionTagsMenu.FindItemByPosition(i)->Check(false);
            }
        wxMenuItem* item = GetDocFrame()->m_exclusionTagsMenu.FindItem(
            (doc->GetExclusionBlockTags().empty()) ? XRCID("ID_EXCLUSION_TAGS_NOT_ENABLED") :
            (doc->GetExclusionBlockTags().at(0) == std::make_pair(L'^', L'^')) ?
                                                     XRCID("ID_EXCLUSION_TAGS_CAROTS") :
            (doc->GetExclusionBlockTags().at(0) == std::make_pair(L'<', L'>')) ?
                                                     XRCID("ID_EXCLUSION_TAGS_ANGLES") :
            (doc->GetExclusionBlockTags().at(0) == std::make_pair(L'[', L']')) ?
                                                     XRCID("ID_EXCLUSION_TAGS_BRACES") :
            (doc->GetExclusionBlockTags().at(0) == std::make_pair(L'{', L'}')) ?
                                                     XRCID("ID_EXCLUSION_TAGS_CURLIES") :
            (doc->GetExclusionBlockTags().at(0) == std::make_pair(L'(', L')')) ?
                                                     XRCID("ID_EXCLUSION_TAGS_PARANS") :
                                                     XRCID("ID_EXCLUSION_TAGS_NOT_ENABLED"));
        if (item != nullptr)
            {
            item->Check(true);
            }
        }
        // numerals
        {
        for (size_t i = 0; i < GetDocFrame()->m_numeralSyllabicationMenu.GetMenuItemCount(); ++i)
            {
            GetDocFrame()->m_numeralSyllabicationMenu.FindItemByPosition(i)->Check(false);
            }
        wxMenuItem* item = GetDocFrame()->m_numeralSyllabicationMenu.FindItem(
            (doc->GetNumeralSyllabicationMethod() == NumeralSyllabize::WholeWordIsOneSyllable) ?
                XRCID("ID_NUMSYL_ONE") :
                XRCID("ID_NUMSYL_EACH_DIGIT"));
        if (item != nullptr)
            {
            item->Check(true);
            }
        }

    if (GetRibbon() != nullptr)
        {
        wxWindow* deductionButtonBar =
            GetRibbon()->FindWindow(MainFrame::ID_PARAGRAPH_DEDUCTION_RIBBON_BUTTON_BAR);
        if ((deductionButtonBar != nullptr) &&
            deductionButtonBar->IsKindOf(wxCLASSINFO(wxRibbonButtonBar)))
            {
            dynamic_cast<wxRibbonButtonBar*>(deductionButtonBar)
                ->ToggleButton(XRCID("ID_IGNORE_BLANK_LINES"),
                               doc->IsIgnoringBlankLinesForParagraphsParser());
            dynamic_cast<wxRibbonButtonBar*>(deductionButtonBar)
                ->ToggleButton(XRCID("ID_IGNORE_INDENTING"),
                               doc->IsIgnoringIndentingForParagraphsParser());
            dynamic_cast<wxRibbonButtonBar*>(deductionButtonBar)
                ->ToggleButton(XRCID("ID_SENTENCES_CAPITALIZED"),
                               doc->GetSentenceStartMustBeUppercased());
            if (dynamic_cast<BaseProjectDoc*>(GetDocument())->GetParagraphsParsingMethod() ==
                ParagraphParse::EachNewLineIsAParagraph)
                {
                dynamic_cast<wxRibbonButtonBar*>(deductionButtonBar)
                    ->EnableButton(XRCID("ID_IGNORE_BLANK_LINES"), false);
                dynamic_cast<wxRibbonButtonBar*>(deductionButtonBar)
                    ->EnableButton(XRCID("ID_IGNORE_INDENTING"), false);
                }
            }
        wxWindow* exclusionButtonBar =
            GetRibbon()->FindWindow(MainFrame::ID_TEXT_EXCLUSION_RIBBON_BUTTON_BAR);
        if ((exclusionButtonBar != nullptr) &&
            exclusionButtonBar->IsKindOf(wxCLASSINFO(wxRibbonButtonBar)))
            {
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)
                ->ToggleButton(XRCID("ID_EXCLUDE_AGGRESSIVELY"), doc->IsExcludingAggressively());
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)
                ->ToggleButton(XRCID("ID_EXCLUDE_COPYRIGHT_NOTICES"),
                               doc->IsExcludingTrailingCopyrightNoticeParagraphs());
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)
                ->ToggleButton(XRCID("ID_EXCLUDE_TRAILING_CITATIONS"),
                               doc->IsExcludingTrailingCitations());
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)
                ->ToggleButton(XRCID("ID_EXCLUDE_FILE_ADDRESSES"), doc->IsExcludingFileAddresses());
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)
                ->ToggleButton(XRCID("ID_EXCLUDE_NUMERALS"), doc->IsExcludingNumerals());
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)
                ->ToggleButton(XRCID("ID_EXCLUDE_PROPER_NOUNS"), doc->IsExcludingProperNouns());
            // disable exclusion buttons
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)
                ->EnableButton(
                    XRCID("ID_EXCLUDE_AGGRESSIVELY"),
                    (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)
                ->EnableButton(
                    XRCID("ID_EXCLUDE_COPYRIGHT_NOTICES"),
                    (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)
                ->EnableButton(
                    XRCID("ID_EXCLUDE_TRAILING_CITATIONS"),
                    (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)
                ->EnableButton(
                    XRCID("ID_EXCLUDE_FILE_ADDRESSES"),
                    (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)
                ->EnableButton(
                    XRCID("ID_EXCLUDE_NUMERALS"),
                    (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)
                ->EnableButton(
                    XRCID("ID_EXCLUDE_PROPER_NOUNS"),
                    (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)
                ->EnableButton(
                    XRCID("ID_EXCLUDE_WORD_LIST"),
                    (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            dynamic_cast<wxRibbonButtonBar*>(exclusionButtonBar)
                ->EnableButton(
                    XRCID("ID_EXCLUSION_TAGS"),
                    (doc->GetInvalidSentenceMethod() != InvalidSentence::IncludeAsFullSentences));
            }
        wxWindow* numeralButtonBar =
            GetRibbon()->FindWindow(MainFrame::ID_NUMERALS_RIBBON_BUTTON_BAR);
        if ((numeralButtonBar != nullptr) &&
            numeralButtonBar->IsKindOf(wxCLASSINFO(wxRibbonButtonBar)))
            {
            dynamic_cast<wxRibbonButtonBar*>(numeralButtonBar)
                ->EnableButton(
                    XRCID("ID_NUMERAL_SYLLABICATION"),
                    (doc->GetInvalidSentenceMethod() == InvalidSentence::IncludeAsFullSentences) ?
                        true :
                        !doc->IsExcludingNumerals());
            }
        }

    // fit the sidebar to its labels' sizes
    const size_t sideBarMinimumWidth = GetSideBar()->AdjustWidthToFitItems();
    GetSplitter()->SetMinimumPaneSize(sideBarMinimumWidth);
    GetSplitter()->SplitVertically(GetSideBar(), GetSplitter()->GetWindow2(), sideBarMinimumWidth);

    m_presentedSuccessfully = true;

    Activate(true);

    if (WarningManager::HasWarning(_DT(L"note-export-from-save")))
        {
        ShowInfoMessage(*WarningManager::GetWarning(_DT(L"note-export-from-save")));
        }
    }

//-------------------------------------------------------
void BaseProjectView::ShowInfoMessage(const WarningMessage& message)
    {
    if (m_lastShownMessageId == message.GetId())
        {
        return;
        }
    auto warningIter = WarningManager::GetWarning(message.GetId());
    if (warningIter != WarningManager::GetWarnings().end() && warningIter->ShouldBeShown())
        {
        if (GetInfoBar()->IsShown())
            {
            AddQueuedMessage(message);
            }
        else
            {
            GetInfoBar()->ShowCheckBox(
                message.ShouldOnlyBeShownOnce() ? wxString{} : _(L"Do not show this again."),
                false);
            GetInfoBar()->ShowMessage(message.GetMessage(), message.GetFlags());
            m_lastShownMessageId = message.GetId();
            }
        }
    // message isn't in the general warning system, so just generically add it to be shown
    else if (warningIter == WarningManager::GetWarnings().end())
        {
        AddQueuedMessage(message);
        }
    }

//-------------------------------------------------------
void BaseProjectView::OnCloseInfoBar([[maybe_unused]] wxCommandEvent& event)
    {
    GetInfoBar()->Dismiss();
    if (!m_lastShownMessageId.empty())
        {
        auto warningIter = WarningManager::GetWarning(m_lastShownMessageId);
        if (warningIter != WarningManager::GetWarnings().end())
            {
            if (warningIter->ShouldOnlyBeShownOnce())
                {
                warningIter->Show(false);
                }
            else
                {
                if (GetInfoBar()->IsCheckBoxChecked())
                    {
                    warningIter->Show(false);
                    }
                }
            }
        m_lastShownMessageId.clear();
        }
    if (!GetQueuedMessages().empty())
        {
        auto nextMessage = m_queuedMessages.begin();
        ShowInfoMessage(*nextMessage);
        m_queuedMessages.erase(nextMessage);
        }
    }

//-------------------------------------------------------
void BaseProjectView::ShowSideBar(const bool show /*= true*/)
    {
    const wxWindowUpdateLocker noUpdates(GetDocFrame());
    m_sidebarShown = show;
    if (show)
        {
        GetSideBar()->AdjustWidthToFitItems();
        GetSplitter()->SetMinimumPaneSize(GetSideBar()->GetMinSize().GetWidth());
        GetSplitter()->SetSashPosition(GetSideBar()->GetMinSize().GetWidth());
        }
    else
        {
        GetSideBar()->SetMinSize({ 1, 1 });
        GetSplitter()->SetMinimumPaneSize(1);
        GetSplitter()->SetSashPosition(1);
        }
    // update the ribbon bar
    wxWindow* projectButtonBarWindow =
        GetRibbon()->FindWindow(MainFrame::ID_PROJECT_RIBBON_BUTTON_BAR);
    if ((projectButtonBarWindow != nullptr) &&
        projectButtonBarWindow->IsKindOf(wxCLASSINFO(wxRibbonButtonBar)))
        {
        auto* const projectButtonBar = dynamic_cast<wxRibbonButtonBar*>(projectButtonBarWindow);
        wxASSERT_MSG(projectButtonBar, L"Error casting project ribbon bar!");
        if (projectButtonBar != nullptr)
            {
            projectButtonBar->ToggleButton(XRCID("ID_SHOW_SIDEBAR"), m_sidebarShown);
            }
        }
    }

//---------------------------------------------------
void BaseProjectView::OnDClickRibbonBar([[maybe_unused]] wxRibbonBarEvent& event)
    {
    GetRibbon()->ShowPanels(!GetRibbon()->ArePanelsShown());
    }

//---------------------------------------------------
void BaseProjectView::OnClickRibbonBar(wxRibbonBarEvent& event)
    {
    GetRibbon()->ShowPanels();
    event.Skip();
    }

//-------------------------------------------------------
bool BaseProjectView::OnCreate(wxDocument* doc, [[maybe_unused]] long flags)
    {
    // hide the empty mainframe when a document window is opened, unless the
    // script workbench has unsaved or loaded scripts that the user may need
    const auto* wb = wxGetApp().GetMainFrameEx()->GetScriptWorkbench();
    if (wb == nullptr || !wb->HasUnsavedOrLoadedScripts())
        {
        wxGetApp().GetMainFrame()->Hide();
        }

    m_frame = CreateChildFrame(doc, this);
    SetFrame(m_frame);

    m_maxColumnWidth = GetDocFrame()->FromDIP(wxSize{ 200, 200 }).GetWidth();

    m_splitter = new wxSplitterWindow(m_frame, SPLITTER_ID, wxDefaultPosition,
                                      m_frame->GetClientSize(), wxSP_LIVE_UPDATE | wxSP_NOBORDER);

    // main sidebar (left side of splitter)
    m_sideBar = wxGetApp().CreateSideBar(m_splitter, BaseProjectView::LEFT_PANE);

    auto* mainSizer = new wxBoxSizer(wxVERTICAL);

    auto* quickAccessToolbarSizer = new wxBoxSizer(wxHORIZONTAL);
    // quick access toolbar
    m_quickToolbar = new wxAuiToolBar(m_frame, wxID_ANY);
    quickAccessToolbarSizer->Add(GetQuickToolbar(), wxSizerFlags{ 1 }.Expand());
    // save
    GetQuickToolbar()->AddTool(wxID_SAVE, _(L"Save the project"),
                               wxArtProvider::GetBitmapBundle(wxART_FILE_SAVE, wxART_BUTTON));
    // copy
    GetQuickToolbar()->AddTool(wxID_COPY, _(L"Copy selection"),
                               wxArtProvider::GetBitmapBundle(wxART_COPY, wxART_BUTTON));
    // print
    GetQuickToolbar()->AddTool(wxID_PRINT, _(L"Print the selected window"),
                               wxArtProvider::GetBitmapBundle(wxART_PRINT, wxART_BUTTON));
    GetQuickToolbar()->Realize();

    m_searchCtrl = new Wisteria::UI::SearchPanel(m_frame, wxID_ANY);
    quickAccessToolbarSizer->Add(GetSearchPanel());
    mainSizer->Add(quickAccessToolbarSizer, wxSizerFlags{}.Expand());

    m_ribbon = wxGetApp().CreateRibbon(m_frame, doc);
    mainSizer->Add(m_ribbon, wxSizerFlags{}.Expand());

    m_infoBar = new wxInfoBar(m_frame, wxID_ANY, wxINFOBAR_CHECKBOX);
    GetInfoBar()->Connect(wxID_CLOSE, wxEVT_BUTTON,
                          wxCommandEventHandler(BaseProjectView::OnCloseInfoBar), nullptr, this);
    GetInfoBar()->SetEffectDuration(250);
    GetInfoBar()->SetShowHideEffects(wxSHOW_EFFECT_SLIDE_TO_BOTTOM, wxSHOW_EFFECT_SLIDE_TO_TOP);
    mainSizer->Add(GetInfoBar(), wxSizerFlags{}.Expand());

    // initialize ribbon menus (print menu is the same for both types of projects)
    wxGetApp().FillPrintMenu(m_frame->m_printMenu,
                             ReadabilityApp::RibbonType::StandardProjectRibbon);
    wxGetApp().FillSaveMenu(m_frame->m_saveMenu,
                            IsKindOf(wxCLASSINFO(ProjectView)) ?
                                ReadabilityApp::RibbonType::StandardProjectRibbon :
                                ReadabilityApp::RibbonType::BatchProjectRibbon);
    wxGetApp().FillExportMenu(m_frame->m_exportMenu,
                              IsKindOf(wxCLASSINFO(ProjectView)) ?
                                  ReadabilityApp::RibbonType::StandardProjectRibbon :
                                  ReadabilityApp::RibbonType::BatchProjectRibbon);
    m_frame->m_fileOpenMenu.Append(wxID_OPEN, _(L"Open Project...") + L"\tCtrl+O");
    wxGetApp().GetDocManager()->FileHistoryUseMenu(&m_frame->m_fileOpenMenu);
    if (m_frame->m_fileOpenMenu.FindItem(wxID_FILE1) == nullptr)
        {
        wxGetApp().GetDocManager()->FileHistoryAddFilesToMenu(&m_frame->m_fileOpenMenu);
        }
    m_workSpaceSizer = new wxBoxSizer(wxHORIZONTAL);
    m_workSpaceSizer->Add(m_splitter, wxSizerFlags{ 1 }.Expand());

    mainSizer->Add(m_workSpaceSizer, wxSizerFlags{ 1 }.Expand());

    m_frame->SetSizer(mainSizer);

    const std::array<wxAcceleratorEntry, 18> accelEntries = {
        wxAcceleratorEntry(wxACCEL_NORMAL, WXK_F1, wxID_HELP),
        wxAcceleratorEntry(wxACCEL_CMD, 'N', wxID_NEW),
        wxAcceleratorEntry(wxACCEL_CMD, 'O', wxID_OPEN),
        wxAcceleratorEntry(wxACCEL_CMD, 'V', wxID_PASTE),
        wxAcceleratorEntry(wxACCEL_CMD, 'P', wxID_PRINT),
        wxAcceleratorEntry(wxACCEL_CMD, 'F', wxID_FIND),
        wxAcceleratorEntry(wxACCEL_CMD, 'C', wxID_COPY),
        wxAcceleratorEntry(wxACCEL_CMD, 'S', wxID_SAVE),
        wxAcceleratorEntry(wxACCEL_CMD, 'A', wxID_SELECTALL),

        wxAcceleratorEntry(wxACCEL_NORMAL, WXK_F5, XRCID("ID_DOCUMENT_REFRESH")),
        wxAcceleratorEntry(wxACCEL_CMD, WXK_BACK, XRCID("ID_REMOVE_TEST")),
        wxAcceleratorEntry(wxACCEL_CMD, WXK_NUMPAD_DELETE, XRCID("ID_REMOVE_TEST")),
        wxAcceleratorEntry(wxACCEL_CMD, WXK_DELETE, XRCID("ID_REMOVE_TEST")),

        wxAcceleratorEntry(wxACCEL_NORMAL, WXK_F2, XRCID("ID_SHOW_SIDEBAR")),
        wxAcceleratorEntry(wxACCEL_NORMAL, WXK_F3, XRCID("ID_FIND_NEXT")),
        // the Microsoft Office shortcut for this is Ctrl+F1, but Ctrl and function key
        // combinations don't seem to work under GTK+
        wxAcceleratorEntry(wxACCEL_NORMAL, WXK_F4, XRCID("ID_TOGGLE_RIBBON")),
        wxAcceleratorEntry(wxACCEL_CMD, WXK_RIGHT, XRCID("ID_SEND_TO_STANDARD_PROJECT")),
        wxAcceleratorEntry(wxACCEL_NORMAL, WXK_F6, XRCID("ID_LAUNCH_SOURCE_FILE")),
    };

    m_frame->SetAcceleratorTable(wxAcceleratorTable{ accelEntries.size(), accelEntries.data() });

    Bind(wxEVT_RIBBONBAR_TAB_LEFT_DCLICK, &BaseProjectView::OnDClickRibbonBar, this, wxID_ANY);
    Bind(wxEVT_RIBBONBAR_PAGE_CHANGED, &BaseProjectView::OnClickRibbonBar, this, wxID_ANY);

    return true;
    }

//-------------------------------------------------------
BaseProjectView::~BaseProjectView()
    {
    if (GetMenuBar() != nullptr)
        {
        wxGetApp().GetDocManager()->FileHistoryRemoveMenu(GetMenuBar()->GetMenu(0));
        }
    wxGetApp().GetDocManager()->FileHistoryRemoveMenu(&m_frame->m_fileOpenMenu);
    /* In case the document failed to load, and we didn't set these to the parent,
       then clean them up ourselves*/
    if (!m_presentedSuccessfully)
        {
        wxLogWarning(L"Project was not fully loaded; Project UI being destroyed manually.");
        m_frame->Destroy();
        m_frame = nullptr;
        if (GetMenuBar() != nullptr)
            {
            m_menuBar->Destroy();
            m_menuBar = nullptr;
            }
        }
    // OnClose() should handle this (even on failure), but doesn't hurt to check this here too.
    if (wxGetApp().GetDocumentCount() == 1)
        {
        // show the empty mainframe when the last document is being closed
        wxArrayString mruFiles;
        for (size_t i = 0; i < wxGetApp().GetDocManager()->GetFileHistory()->GetCount(); ++i)
            {
            mruFiles.Add(wxGetApp().GetDocManager()->GetFileHistory()->GetHistoryFile(i));
            }
        wxGetApp().GetMainFrameEx()->GetStartPage()->SetMRUList(mruFiles);
        wxGetApp().GetMainFrame()->CenterOnScreen();
        wxGetApp().GetMainFrame()->Show();
        wxGetApp().SafeYield(wxGetApp().GetMainFrame(), true);
        }
    }

//-------------------------------------------------------
void BaseProjectView::OnActivateView(const bool activate, [[maybe_unused]] wxView* oldView,
                                     [[maybe_unused]] wxView* newView)
    {
    // if the frame (and its views) are ready for showing then show it
    if (activate && m_presentedSuccessfully && (GetDocFrame() != nullptr))
        {
        GetDocFrame()->Show(true);
        GetSplitter()->Show(true);
        if (GetSplitter()->GetWindow2() != nullptr)
            {
            GetSplitter()->GetWindow2()->Show(true);
            }
        }
    }
