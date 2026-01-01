# Old-timey / sepia printed look on an image.
oldTimey <- function(image)
  {
  im <-
    if (inherits(image, "magick-image"))
      {
      image
      }
    else
      {
      image_read(image)
      }

  im |>
    image_modulate(saturation = 80, brightness = 100) |>
    image_colorize(opacity = 30, color = "brown") |>
    image_contrast(sharpen = 1) |>
    image_noise("Gaussian") |>
    image_blur(0, 0.4)
  }

#' @brief Create a 4-image grid cover with alternating layout.
#'
#' Creates a fixed-size cover image composed of four input images arranged
#' in a two-row grid. The top row allocates 60% of the width to the first image
#' and 40% to the second; the bottom row reverses this allocation.
#'
#' Each image is scaled to completely fill its assigned tile while preserving
#' aspect ratio, with any excess cropped from the center. The resulting grid is
#' placed on a transparent background frame and written as an image.
#'
#' @param image1 Path to the top-left (wide) image.
#' @param image2 Path to the top-right (narrow) image.
#' @param image3 Path to the bottom-left (narrow) image.
#' @param image4 Path to the bottom-right (wide) image.
#' @param outImage Output file path for the generated PNG cover image.
#' @param width Total width of the output image in pixels.
#' @param height Total height of the output image in pixels.
#' @param frame Thickness of the outer background frame in pixels.
#' @param wide_frac Fraction of row width allocated to the wide tile.
#' @param row_frac Fraction of height allocated to the top row.
createGridCover <- function(
    image1,
    image2,
    image3,
    image4,
    outImage,
    width  = 1913,
    height = 2000,
    frame  = 20,
    gutter = 20,
    wide_frac = 0.6,
    row_frac  = 0.5,
    pad_top   = 0)
  {
  # canvas with transparent padding at top
  canvas <- magick::image_blank(
    width  = width,
    height = height + pad_top  )

  # inner drawable area (unchanged)
  inner_w <- width  - 2 * frame
  inner_h <- height - 2 * frame

  top_h    <- round((inner_h - gutter) * row_frac)
  bottom_h <- inner_h - gutter - top_h

  wide_w   <- round((inner_w - gutter) * wide_frac)
  narrow_w <- inner_w - gutter - wide_w

  # read + fit helper
  fit <- function(x, w, h)
    {
    im <- if (inherits(x, "magick-image")) x else magick::image_read(x)
    im |>
      magick::image_resize(glue::glue("{w}x{h}^")) |>
      magick::image_crop(glue::glue("{w}x{h}+0+0"), gravity = "west")
    }

  # tiles
  i1 <- fit(image1, wide_w,   top_h)     # row 1 left (wide)
  i2 <- fit(image2, narrow_w, top_h)     # row 1 right (narrow)
  i3 <- fit(image3, narrow_w, bottom_h)  # row 2 left (narrow)
  i4 <- fit(image4, wide_w,   bottom_h)  # row 2 right (wide)

  # offsets (everything shifted down by pad_top)
  y_top    <- frame + pad_top
  y_bottom <- frame + pad_top + top_h + gutter

  # row 1: wide | narrow
  x1_left  <- frame
  x1_right <- frame + wide_w + gutter

  # row 2: narrow | wide
  x2_left  <- frame
  x2_right <- frame + narrow_w + gutter

  # composite
  cover <- canvas |>
    magick::image_composite(i1, offset = glue::glue("+{x1_left}+{y_top}"),    operator = "DstOver") |>
    magick::image_composite(i2, offset = glue::glue("+{x1_right}+{y_top}"),   operator = "DstOver") |>
    magick::image_composite(i3, offset = glue::glue("+{x2_left}+{y_bottom}"), operator = "DstOver") |>
    magick::image_composite(i4, offset = glue::glue("+{x2_right}+{y_bottom}"),operator = "DstOver") |>
    magick::image_convert(format = "png", depth = 8)

  magick::image_write(cover, outImage)
  }

# Make an image look like a Polaroid.
createPolaroid <- function(image,
                           caption = "Polaroid",
                           rotate = 3,
                           font_candidates = NULL)
  {
  # default handwritten candidates
  if (is.null(font_candidates))
    {
    font_candidates <- c(
      "Ink Free",
      "Gabriola",
      "Segoe Script",
      "Lucida Handwriting",
      "Bradley Hand",
      "Chalkboard",
      "Snell Roundhand")
    }

  matchedFont <- font_candidates[font_candidates %in% magick_fonts()$family]

  captionFont <- "sans"
  if (length(matchedFont) > 0)
    {
    captionFont <- matchedFont[1]
    message(str_glue("✔ Using font family {captionFont} for Polaroid effect."))
    }
  else
    {
    message("⚠ No magick-compatible handwritten font found; falling back to 'sans' for Polaroid effect.")
    }

  # if a path, load it as an image
  if (!inherits(image, "magick-image")) image <- image_read(image)
  info <- image_info(image)
  w <- info$width
  h <- info$height

  # borders (proportional)
  sideBorder  <- round(w * 0.03)
  topBorder   <- round(h * 0.02)
  bottomExtra <- round(h * 0.15)

  # caption sizing/offsets (proportional)
  fontSize <- round(w * 0.05)           # 5% of width
  xOffset  <- round(w * 0.10)           # offset from left inside white border
  yOffset  <- round(bottomExtra * 0.6)  # lifted into bottom border

  # render the Polaroid
  image %>%
    image_border("white", geometry = str_glue("{sideBorder}x{topBorder}")) %>%
    image_extent(
      geometry = str_glue("{w + 2*sideBorder}x{h + topBorder + sideBorder + bottomExtra}"),
      gravity = "north",
      color = "white"
    ) %>%
    image_border("gray40", "2x2") %>%
    image_shadow() %>%
    # add a caption
    image_annotate(
      text     = caption,
      font     = captionFont,
      size     = fontSize,
      color    = "black",
      gravity  = "southwest",
      location = str_glue("+{xOffset}+{yOffset}")
    ) %>%
    # rotate
    image_rotate(rotate)
  }

# Creates a collage from a set of images for a manual cover.
# If captions contains three values, then the images will be displays as Polaroids
# with those captions.
createPolaroidCover <- function(image1, image2, image3, outImage, captions = c())
  {
  imageLeft <- image_read(image1) %>%
    image_scale(geometry_area(1200))

  if (length(captions) >= 3)
    {
    imageLeft %<>% createPolaroid(caption = captions[1], rotate = 3)
    }

  imageRight <- image_read(image2) %>%
    image_scale(geometry_area(1000))

  if (length(captions) >= 3)
    {
    imageRight %<>% createPolaroid(caption = captions[2], rotate = 5)
    }

  imageBottom <- image_read(image3) %>%
    image_scale(geometry_area(1000))

  if (length(captions) >= 3)
    {
    imageBottom %<>% createPolaroid(caption = captions[3], rotate = -5)
    }

  manualCover <- magick::image_blank(width = 1913, height = 2200) %>%
    image_composite(imageRight, offset = "+750+500", operator = "DstOver") %>%
    image_composite(imageLeft %>% image_rotate(3), offset = "+50+325", operator = "DstOver") %>%
    image_composite(imageBottom, offset = "+250+1100", operator = "DstOver") %>%
    image_convert(depth = 8, format = "png")

  magick::image_write(manualCover, path = outImage)
  }
