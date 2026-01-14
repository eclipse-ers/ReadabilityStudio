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

createGridCover <- function(
    image1,
    image2,
    image3,
    image4,
    outImage,
    width  = 1900,
    height = 2000,
    frame  = 20,
    gutter = 20,
    wide_frac = 0.6,
    row_frac  = 0.5,
    pad_top   = 0,
    row_shift_top    = 40,
    row_shift_bottom = 60,
    vertical_nudge   = 5,
    row_gap_extra = 40)
  {
  extra_h <- 60
  pad_bottom <- extra_h

  # horizontal breathing room for row shifts
  side_pad <- max(row_shift_top, row_shift_bottom)

  # expanded canvas
  canvas <- magick::image_blank(
    width  = width + 2 * side_pad,
    height = height + pad_top + pad_bottom)

  # inner drawable area
  inner_w <- width  - 2 * frame
  inner_h <- height - 2 * frame
  v_gutter <- gutter + row_gap_extra

  frame_x <- frame + side_pad

  top_h    <- round((inner_h - v_gutter) * row_frac)
  bottom_h <- inner_h - v_gutter - top_h

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

  # tiles (mirrored tall ones)
  i1 <- fit(image1, wide_w,   top_h)
  i2 <- fit(image2, narrow_w, top_h    + extra_h)  # top-right taller
  i3 <- fit(image3, narrow_w, bottom_h + extra_h)  # bottom-left taller
  i4 <- fit(image4, wide_w,   bottom_h)

  # vertical offsets
  y_top        <- frame + pad_top + vertical_nudge
  y_top_i2     <- y_top - extra_h   # grow upward
  y_bottom     <- frame + pad_top + top_h + v_gutter - vertical_nudge
  # i3 grows downward, so no adjustment

  # horizontal offsets
  x1_left  <- frame_x - row_shift_top
  x1_right <- frame_x + wide_w + gutter - row_shift_top
  
  x2_left  <- frame_x + row_shift_bottom
  x2_right <- frame_x + narrow_w + gutter + row_shift_bottom

  # composite
  cover <- canvas |>
    # row 1
    magick::image_composite(i1,
                            offset = glue::glue("+{x1_left}+{y_top}"),
                            operator = "DstOver") |>
    magick::image_composite(i2,
                            offset = glue::glue("+{x1_right}+{y_top_i2}"),
                            operator = "DstOver") |>

    # row 2
    magick::image_composite(i3,
                            offset = glue::glue("+{x2_left}+{y_bottom}"),
                            operator = "DstOver") |>
    magick::image_composite(i4,
                            offset = glue::glue("+{x2_right}+{y_bottom}"),
                            operator = "DstOver") |>

    magick::image_convert(format = "png", depth = 8)

  magick::image_write(cover, outImage)
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
