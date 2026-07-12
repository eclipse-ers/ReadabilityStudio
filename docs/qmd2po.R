#########################################
# Quarto → PO Extraction & TM Toolkit   #
#########################################

if (nchar(system.file(package = "pacman")) == 0) {
  install.packages("pacman")
}
library(pacman)
pacman::p_load(tidyverse, this.path, glue, stringr, readr, purrr, tibble, dplyr, fs)

##########################
# 1. Normalization Core  #
##########################

# Centralized msgid normalization. This MUST be used consistently:
# - extract_translatable_lines()
# - qmd2po()
# - po2qmd()
normalize_msgid <- function(text) {
  text %>%
    stringr::str_replace_all("\\p{Z}", " ") %>%
    stringr::str_trim() %>%
    # Drop trailing {#id} anchors, but keep {.class} (e.g. {.unnumbered})
    stringr::str_remove("\\s*\\{#[-A-Za-z0-9_]+\\}\\s*$") %>%
    # Strip heading markers, bullets, and leading blockquote
    stringr::str_remove("^#+\\s*") %>%
    stringr::str_remove("^\\*\\s+(?=\\S)") %>%
    stringr::str_remove("^>\\s*")
}

########################################
# 2. Fence Scanner (YAML, code, math…) #
########################################

scan_fences <- function(lines) {
  n <- length(lines)
  in_code <- in_yaml <- in_math <- in_callout <- in_html_comment <- in_suppress <- logical(n)
  code_state <- yaml_state <- math_state <- callout_state <- html_comment_state <- suppress_state <- FALSE
  
  for (i in seq_len(n)) {
    line <- lines[i]
    
    # YAML fences
    if (stringr::str_detect(line, "^---$")) {
      yaml_state <- !yaml_state
      in_yaml[i] <- yaml_state
      next
    }
    
    # ``` fences (code)
    if (stringr::str_starts(line, "```")) {
      code_state <- !code_state
      in_code[i] <- code_state
      next
    }
    
    # ::: fences – treat ONLY callouts as skip regions
    if (stringr::str_starts(line, ":::")) {
      # true callouts: note, warning, tip, important, caution, .callout-*
      is_callout <- stringr::str_detect(
        line,
        "^:::\\s*(warning|note|tip|important|caution|\\{\\.callout-[^}]+\\})"
      )
      
      if (is_callout) {
        callout_state <- !callout_state
        in_callout[i] <- callout_state
      }
      next
    }
    
    # $$ math
    if (stringr::str_detect(line, "^\\$\\$$")) {
      math_state <- !math_state
      in_math[i] <- math_state
      next
    }
    
    # Suppression blocks (quneiform / md2po style)
    if (stringr::str_detect(
      line,
      "<!--\\s*(quneiform-suppress-begin|translate:off)\\s*-->"
    )) {
      suppress_state <- TRUE
      in_suppress[i] <- TRUE
      next
    }
    
    if (suppress_state) in_suppress[i] <- TRUE
    
    if (stringr::str_detect(
      line,
      "<!--\\s*(quneiform-suppress-end|translate:on)\\s*-->"
    )) {
      suppress_state <- FALSE
      in_suppress[i] <- TRUE
      next
    }
    
    # HTML comments (multi-line)
    if (stringr::str_detect(line, "<!--")) {
      html_comment_state <- TRUE
    }
    if (html_comment_state) in_html_comment[i] <- TRUE
    if (stringr::str_detect(line, "-->")) {
      html_comment_state <- FALSE
      in_html_comment[i] <- TRUE
      next
    }
    
    in_yaml[i]          <- yaml_state
    in_code[i]          <- code_state
    in_math[i]          <- math_state
    in_callout[i]       <- callout_state
    in_html_comment[i]  <- html_comment_state
    in_suppress[i]      <- suppress_state
  }
  
  tibble::tibble(
    line_num = seq_len(n),
    text = lines,
    in_yaml,
    in_code,
    in_math,
    in_callout,
    in_html_comment,
    in_suppress
  )
}

##############################################
# 3. Extract translatable lines from a QMD   #
##############################################

extract_translatable_lines <- function(lines) {
  
  fence_df <- scan_fences(lines)
  
  fence_df %>%
    dplyr::filter(
      !in_code, !in_yaml, !in_math, !in_callout,
      !in_html_comment, !in_suppress
    ) %>%
    dplyr::mutate(
      text_trim = stringr::str_replace_all(text, "\\p{Z}", " ") |> stringr::str_trim()
    ) %>%
    dplyr::mutate(
      translator_comment = dplyr::if_else(
        stringr::str_detect(
          text_trim,
          "^<!--\\s*TRANSLATORS:\\s*(.*?)-->$"
        ),
        stringr::str_match(
          text_trim,
          "^<!--\\s*TRANSLATORS:\\s*(.*?)-->$"
        )[,2],
        NA_character_
      )
    ) %>%
    dplyr::mutate(
      # skip patterns – be conservative
      skip =
        text_trim == "" |
        # pure backtick line (not inline R)
        (stringr::str_detect(text_trim, "^`[^`]+`$") &
           !stringr::str_detect(text_trim, "^`r\\s+")) |
        # code / callout fences
        stringr::str_detect(text_trim, "^```") |
        stringr::str_detect(text_trim, "^:::") |
        # pure blockquote marker
        stringr::str_detect(text_trim, "^>\\s*$") |
        # pure math fence
        stringr::str_detect(text_trim, "^\\$\\$\\s*$") |
        # images
        stringr::str_detect(text_trim, "^!\\[.*\\]\\(.*\\)") |
        # single-line HTML comment
        stringr::str_detect(text_trim, "^<!--.*-->$") |
        # pandoc/shortcode stuff
        stringr::str_detect(text_trim, "^\\{%.*%\\}$") |
        (stringr::str_detect(text_trim, "^\\{\\{<[^>]+>\\}\\}$") &
         !stringr::str_detect(text_trim, "\\*") ) |
        # pure link-only lines like [https://git-scm.com/](https://git-scm.com/)
        stringr::str_detect(text_trim, "^\\s*\\[[^\\]]+\\]\\([^)]*\\)\\s*$") |
        # hrules
        stringr::str_detect(text_trim, "^\\s{0,3}(\\*{3,}|-{3,}|_{3,})\\s*$") |
        # ASCII table borders (grid/pipe)
        stringr::str_detect(text_trim, "^[-:|+\\s=]+$") |
        # LaTeX-only commands
        stringr::str_detect(text_trim, "^\\\\[A-Za-z]+(\\{.*\\})?$") |
        # pure inline R chunk (whole line)
        stringr::str_detect(text_trim, "^`r\\s+.*`$") |
        # [toc]
        stringr::str_detect(text_trim, "^\\[toc\\]$") |
        # {tbl:...} / {fig:...}
        stringr::str_detect(text_trim, "^\\{\\s*(tbl|fig)[:].*\\}$") |
        # bare HTML tags
        stringr::str_detect(text_trim, "^<\\/?[A-Za-z0-9]+.*>$")
    ) %>%
    dplyr::filter(!skip) %>%
    dplyr::mutate(
      text_clean = normalize_msgid(text_trim)
    ) %>%
    dplyr::filter(nzchar(text_clean))
}

#########################################
# 4. Integrity Checking for a QMD file  #
#########################################

qmd_check_integrity <- function(lines, file = NULL) {
  add_issue <- function(type, idx, msg)
    tibble::tibble(type = type, line = idx, snippet = msg)

  issues <- tibble::tibble(type = character(), line = integer(), snippet = character())

  fence_df <- scan_fences(lines)

  df <- fence_df %>%
    dplyr::filter(!in_code, !in_yaml, !in_math, !in_callout, !in_html_comment, !in_suppress) %>%
    dplyr::mutate(text_trim = stringr::str_trim(text))

  latex_cmd_rx <- "^\\\\[A-Za-z]+(\\{.*\\})?"
  id_rx        <- "\\{#([-A-Za-z0-9_]+)\\}"

  count <- function(rx) sum(stringr::str_detect(lines, rx))

  if (count("^---$") %% 2 != 0)
    issues <- dplyr::bind_rows(issues, add_issue("Unbalanced YAML fence", NA, "---"))
  if (count("^```") %% 2 != 0)
    issues <- dplyr::bind_rows(issues, add_issue("Unbalanced code fence", NA, "```"))
  if (count("^:::") %% 2 != 0)
    issues <- dplyr::bind_rows(issues, add_issue("Unbalanced ::: fence", NA, ":::"))
  if (count("^\\$\\$$") %% 2 != 0)
    issues <- dplyr::bind_rows(issues, add_issue("Unbalanced math fence", NA, "$$"))
  if (count("<!--") != count("-->"))
    issues <- dplyr::bind_rows(issues, add_issue("Unbalanced HTML comment", NA, "Mismatched <!-- / -->"))

  is_latex_cmd <- stringr::str_detect(df$text_trim, latex_cmd_rx)

  bad_img <- which(
    !is_latex_cmd &
      (
        stringr::str_detect(df$text_trim, "^!\\[.*\\]\\(\\s*\\\".*\\\"\\s*\\)") |
          stringr::str_detect(df$text_trim, "^!\\[[^\\]]*$") |
          stringr::str_detect(df$text_trim, "^!\\[.*\\]\\([^\\)]*$")
      )
  )
  if (length(bad_img)) {
    issues <- dplyr::bind_rows(
      issues,
      add_issue("Malformed image", df$line_num[bad_img], df$text[bad_img]))
  }

  # malformed links – leaving as in your original, trimmed for brevity
  is_citation_or_footnote <- 
    stringr::str_detect(df$text_trim, "\\[[-@][^\\]]*\\]") |
    stringr::str_detect(df$text_trim, "\\[\\^[^\\]]*\\]")

  is_quarto_attr_span <- 
    stringr::str_detect(df$text_trim, "\\]\\s*\\{\\.[^}]+\\}")

  is_bold_bracket <- 
    stringr::str_detect(df$text_trim, "\\*\\*\\s*\\[[^\\]]+\\]\\s*\\*\\*")

  is_numeric_bracket <- 
    stringr::str_detect(df$text_trim, "\\[[0-9]+\\](?!\\()")

  # Skip Pandoc-ish @[…] or *@[…] patterns (not actual links)
  is_at_block <- stringr::str_detect(df$text_trim, "@\\[[^\\]]*\\]")

  bad_link <- which(
    !is_latex_cmd &
      !is_citation_or_footnote &
      !is_quarto_attr_span &
      !is_numeric_bracket &
      !is_bold_bracket &
      !is_at_block &
      stringr::str_detect(df$text_trim, "\\[") &
      (
        (stringr::str_detect(df$text_trim, "\\[[^\\]]*\\]") &
           stringr::str_detect(df$text_trim, "\\[[^\\]]*\\]\\([^\\)]*$")) |
          (stringr::str_detect(df$text_trim, "\\[.*\\]") &
             !stringr::str_detect(df$text_trim, "\\(.*\\)"))
      )
  )

  if (length(bad_link)) {
    issues <- dplyr::bind_rows(
      issues,
      add_issue("Malformed link", df$line_num[bad_link], df$text[bad_link]))
  }

  id_matches <- stringr::str_match_all(df$text, id_rx)
  ids <- unlist(lapply(id_matches, function(m) {
    if (!is.null(m) && nrow(m) > 0) m[, 2, drop = TRUE] else character(0)
  }))
  if (length(ids)) {
    dup_ids <- ids[duplicated(ids)]
    if (length(dup_ids))
      issues <- dplyr::bind_rows(
        issues,
        add_issue("Duplicate ID", NA, paste(unique(dup_ids), collapse = ", ")))
  }

  empty_head <- which(stringr::str_detect(df$text_trim, "^#+\\s*\\{#"))
  if (length(empty_head))
    issues <- dplyr::bind_rows(
      issues,
      add_issue("Empty heading", df$line_num[empty_head], df$text[empty_head]))

  bad_path <- which(stringr::str_detect(df$text_trim, "\\]\\(/"))
  if (length(bad_path))
    issues <- dplyr::bind_rows(
      issues,
      add_issue("Suspicious absolute path", df$line_num[bad_path], df$text[bad_path]))

  todo <- which(stringr::str_detect(df$text_trim, "(?i)\\bTODO\\b"))
  if (length(todo))
    issues <- dplyr::bind_rows(
      issues,
      add_issue("TODO marker", df$line_num[todo], df$text[todo]))

  # Long lines – uses your existing logic, trimmed for brevity
  long_line <- purrr::keep(seq_len(nrow(df)), function(i) {
    txt <- df$text_trim[i]

    # Ignore blockquote lines
    if (stringr::str_starts(txt, ">")) {
      return(FALSE)
    }

    if (stringr::str_detect(txt, "<br>|\\\\linebreak")) {
      return(FALSE)
    }

    tmp <- txt
    tmp <- stringr::str_remove_all(tmp, "\\[[^\\]]*\\]\\([^\\)]*\\)")
    tmp <- stringr::str_remove_all(tmp, "\\\\[A-Za-z]+\\{[^}]*\\}")
    tmp <- stringr::str_remove_all(tmp, "`r [^`]+`")
    tmp <- stringr::str_remove_all(tmp, "\\[[-@][^\\]]*\\]")
    tmp <- stringr::str_remove_all(tmp, "(?<=\\s)@[-A-Za-z0-9_:.]+")
    tmp <- stringr::str_remove_all(tmp, "\\[\\^[^\\]]*\\]")

    nchar(tmp) > 250
  })
  
  if (length(long_line)) {
    line_lengths <- nchar(df$text_trim[long_line])
    snippet_with_len <- paste0("[LINE LENGTH: ", line_lengths, "] ", df$text[long_line])
    
    issues <- dplyr::bind_rows(
      issues,
      add_issue(
        "Possible long line",
        df$line_num[long_line],
        snippet_with_len))
  }
  
  if (nrow(issues) > 0) {
    cat("\n⚠️  Detected", nrow(issues), "potential issue(s) in", file %||% "<buffer>", "\n\n")
    print(dplyr::arrange(issues, line))
  }
  
  invisible(issues)
}

#############################
# 5. Robust PO file parser  #
#############################

parse_po <- function(po_path) {
  if (!file.exists(po_path)) return(NULL)
  
  lines <- readr::read_lines(po_path)
  entries <- list()
  msgid <- NULL
  msgstr <- NULL
  state <- NULL
  fuzzy <- FALSE
  
  commit <- function() {
    if (!fuzzy && !is.null(msgid) && !is.null(msgstr) && nzchar(msgid)) {
      entries[[msgid]] <<- msgstr
    }
  }
  
  for (ln in lines) {
    # track fuzzy flag — fuzzy entries are treated as untranslated
    if (stringr::str_starts(ln, "#")) {
      if (stringr::str_detect(ln, "#,\\s*fuzzy")) fuzzy <- TRUE
      next
    }
    
    # blank line = boundary
    if (stringr::str_trim(ln) == "") {
      commit()
      msgid  <- NULL
      msgstr <- NULL
      state  <- NULL
      fuzzy  <- FALSE
      next
    }
    
    # new msgid – commit previous
    if (stringr::str_starts(ln, "msgid ")) {
      commit()
      msgid  <- stringr::str_match(ln, '^msgid "(.*)"$')[, 2]
      msgstr <- ""
      state  <- "id"
      next
    }
    
    # msgstr
    if (stringr::str_starts(ln, "msgstr ")) {
      msgstr <- stringr::str_match(ln, '^msgstr "(.*)"$')[, 2]
      state  <- "str"
      next
    }
    
    # msgid continuation
    if (!is.null(state) && state == "id" && stringr::str_starts(ln, '"')) {
      msgid <- paste0(msgid, stringr::str_match(ln, '^"(.*)"$')[, 2])
      next
    }
    
    # msgstr continuation
    if (!is.null(state) && state == "str" && stringr::str_starts(ln, '"')) {
      msgstr <- paste0(msgstr, stringr::str_match(ln, '^"(.*)"$')[, 2])
      next
    }
  }
  
  commit()
  
  if (length(entries) == 0) return(NULL)
  
  tibble::tibble(
    msgid  = names(entries),
    msgstr = unname(unlist(entries))
  )
}

#############################################
# 6. qmd2po: single-file QMD → PO extractor #
#############################################

qmd2po <- function(input,
                   output = NULL,
                   dry_run = FALSE,
                   lang = "en",
                   use_context = TRUE) {
  
  lines <- readr::read_lines(input)
  fence_df <- scan_fences(lines)
  
  # ---- Warn about malformed images ------------------------------------------
  df_for_warn <- fence_df %>%
    dplyr::filter(!in_code, !in_yaml, !in_math, !in_callout,
                  !in_html_comment, !in_suppress) %>%
    dplyr::mutate(text_trim = stringr::str_trim(text))
  
  invalid_candidates <- df_for_warn %>%
    dplyr::filter(
      (stringr::str_detect(text_trim, "^!\\[") &
         !stringr::str_detect(text_trim, "^!\\[.*\\]\\(.*\\)")) |
        stringr::str_detect(text_trim, "^!\\[.*\\]\\(\\\".*\\\"\\)")
    )
  
  if (nrow(invalid_candidates) > 0) {
    cat("\n⚠️  Warning: possible malformed Markdown image lines detected:\n")
    purrr::walk(invalid_candidates$text_trim, ~cat("  •", .x, "\n"))
    cat("  (Common causes: missing '(', misplaced quotes, or mismatched brackets.)\n\n")
  }
  
  # ---- Extract translatable lines -------------------------------------------
  df <- extract_translatable_lines(lines)

  if (nrow(df) == 0) {
    message("No translatable lines found.")
    return(invisible(NULL))
  }
  
  # ---- Group or dedupe, while preserving translator comments ----------------
  if (use_context) {
    df <- df %>%
      dplyr::group_by(text_clean) %>%
      dplyr::summarise(
        lines = paste(sort(unique(line_num)), collapse = ", "),
        translator_comment =
          paste(na.omit(unique(translator_comment)), collapse = " | "),
        .groups = "drop"
      )
  } else {
    df <- df %>%
      dplyr::distinct(text_clean, .keep_all = TRUE) %>%
      dplyr::mutate(
        translator_comment =
          paste(na.omit(unique(translator_comment)), collapse = " | ")
      )
  }
  
  # ---- Dry run --------------------------------------------------------------
  if (dry_run) {
    cat("\n🧩 Dry run — lines that would be extracted:\n\n")
    cat(paste0(df$text_clean, collapse = "\n"))
    cat("\n\n")
    return(invisible(df))
  }
  
  # ---- Build PO header ------------------------------------------------------
  header_block <- paste0(
    'msgid ""\n',
    'msgstr ""\n',
    '"Project-Id-Version: PACKAGE VERSION\\n"\n',
    '"POT-Creation-Date: ', format(Sys.time(), "%Y-%m-%d %H:%M%z"), '\\n"\n',
    '"PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\\n"\n',
    '"Last-Translator: FULL NAME <EMAIL@ADDRESS>\\n"\n',
    '"Language-Team: LANGUAGE <LL@li.org>\\n"\n',
    '"Language: ', lang, '\\n"\n',
    '"MIME-Version: 1.0\\n"\n',
    '"Content-Type: text/plain; charset=UTF-8\\n"\n',
    '"Content-Transfer-Encoding: 8bit\\n"\n\n'
  )
  
  # ---- Load existing translations -------------------------------------------
  existing <- if (!is.null(output) && file.exists(output)) parse_po(output) else NULL
  has_existing <- !is.null(existing) && nrow(existing) > 0
  
  if (has_existing) {
    existing_norm <- existing %>%
      dplyr::mutate(norm_key = normalize_msgid(msgid)) %>%
      dplyr::select(norm_key, msgstr)
  } else {
    existing_norm <- tibble::tibble(norm_key = character(), msgstr = character())
  }
  
  # ---- Build PO entries ------------------------------------------------------
  po_lines <- df %>%
    dplyr::mutate(
      # Usage context (location in file)
      header = if (use_context) {
        sprintf("#: %s:%s", basename(input), lines)
      } else {
        sprintf("#: %s", basename(input))
      },
      
      # Translator comment (PO format — no TRANSLATORS: prefix)
      translator_comment_line = dplyr::if_else(
        !is.na(translator_comment) & translator_comment != "",
        paste0("#. ", translator_comment),
        ""
      ),
      
      # Escape msgid
      msgid = sprintf(
        'msgid "%s"',
        text_clean %>%
          stringr::str_replace_all('\\\\', '\\\\\\\\') %>%  # escape backslashes
          stringr::str_replace_all('"', '\\\\\"')           # escape quotes
      ),
      
      # Apply existing translation if present
      msgstr = purrr::map_chr(
        text_clean,
        ~ {
          norm_key <- normalize_msgid(.x)
          idx <- match(norm_key, existing_norm$norm_key)
          if (has_existing && !is.na(idx)) {
            old <- existing_norm$msgstr[idx]
            if (!is.na(old) && nzchar(old)) sprintf('msgstr "%s"', old)
            else 'msgstr ""'
          } else {
            'msgstr ""'
          }
        }
      )
    ) %>%
    dplyr::mutate(
      # Combine final PO block
      po_block = paste(
        translator_comment_line[translator_comment_line != ""],
        header,
        msgid,
        msgstr,
        "",
        sep = "\n"
      )
    ) %>%
    dplyr::pull(po_block)
  
  # ---- Write PO file --------------------------------------------------------
  if (!is.null(output)) {
    readr::write_lines(c(header_block, po_lines), output)
    message("Wrote ", nrow(df), " entries to ", output, " [Language: ", lang, "]")
  }
  
  invisible(df)
}

#########################################################
# 7. qmd2po_folder: all QMDs under a folder → one PO    #
#########################################################

qmd2po_folder <- function(input_dir,
                          output_po,
                          lang = "en",
                          use_context = FALSE,
                          dry_run = FALSE,
                          exclude_pattern = NULL) {
  
  qmd_files <- list.files(
    path = input_dir,
    pattern = "\\.qmd$",
    full.names = TRUE,
    recursive = TRUE
  )
  
  if (!is.null(exclude_pattern)) {
    qmd_files <- qmd_files[!stringr::str_detect(qmd_files, exclude_pattern)]
  }
  
  if (length(qmd_files) == 0) {
    stop("No .qmd files found in ", input_dir)
  }
  
  message("Found ", length(qmd_files), " QMD file(s) under ", input_dir)
  
  all_entries <- purrr::imap_dfr(qmd_files, function(file_path, file_index) {
    buffer <- readr::read_lines(file_path, skip_empty_rows = FALSE)
    
    issues <- qmd_check_integrity(buffer, file_path)
    if (dry_run && nrow(issues) > 0) {
      stop(paste0(
        "❌ Integrity issues detected in ", file_path, ".\n",
        "Dry run aborted — please fix these issues before extraction."
      ), call. = FALSE)
    }
    
    df <- extract_translatable_lines(buffer)
    if (nrow(df) == 0) return(NULL)
    
    df %>%
      dplyr::mutate(
        source_file  = file_path,
        source_base  = basename(file_path),
        file_index   = file_index
      )
  })
  
  if (nrow(all_entries) == 0) {
    message("No translatable lines found.")
    return(invisible(NULL))
  }
  
  # Global ordering: file, then line, then a stable row index
  all_entries <- all_entries %>%
    dplyr::arrange(file_index, line_num) %>%
    dplyr::mutate(order_index = dplyr::row_number())
  
  if (use_context) {
    df <- all_entries %>%
      dplyr::group_by(text_clean) %>%
      dplyr::summarise(
        first_order = min(order_index),
        lines = paste0(
          paste0(source_base, ":", line_num),
          collapse = ", "
        ),
        translator_comment =
          paste(na.omit(unique(translator_comment)), collapse = " | "),
        .groups = "drop"
      ) %>%
      dplyr::arrange(first_order)
  } else {
    df <- all_entries %>%
      dplyr::arrange(order_index) %>%
      dplyr::distinct(text_clean, .keep_all = TRUE) %>%
      dplyr::mutate(
        translator_comment =
          paste(na.omit(unique(translator_comment)), collapse = " | ")
      )
  }
  
  if (dry_run) {
    message("\n🧩 Dry run — extracted lines:\n")
    cat(paste0(df$text_clean, collapse = "\n"))
    cat("\n")
    return(invisible(df))
  }
  
  # PO header via same logic as qmd2po()
  header_block <- paste0(
    'msgid ""\n',
    'msgstr ""\n',
    '"Project-Id-Version: PACKAGE VERSION\\n"\n',
    '"POT-Creation-Date: ', format(Sys.time(), "%Y-%m-%d %H:%M%z"), '\\n"\n',
    '"PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\\n"\n',
    '"Last-Translator: FULL NAME <EMAIL@ADDRESS>\\n"\n',
    '"Language-Team: LANGUAGE <LL@li.org>\\n"\n',
    '"Language: ', lang, '\\n"\n',
    '"MIME-Version: 1.0\\n"\n',
    '"Content-Type: text/plain; charset=UTF-8\\n"\n',
    '"Content-Transfer-Encoding: 8bit\\n"\n\n'
  )
  
  existing <- if (file.exists(output_po)) parse_po(output_po) else NULL
  has_existing <- !is.null(existing) && nrow(existing) > 0
  
  if (has_existing) {
    existing_norm <- existing %>%
      dplyr::mutate(norm_key = normalize_msgid(msgid)) %>%
      dplyr::select(norm_key, msgstr)
  } else {
    existing_norm <- tibble::tibble(norm_key = character(), msgstr = character())
  }
  
  # df already has text_clean; normalize it the same way
  df_norm <- df %>%
    dplyr::mutate(norm_key = normalize_msgid(text_clean)) %>%
    dplyr::left_join(existing_norm, by = "norm_key")
  
  po_lines <- df_norm %>%
    dplyr::mutate(
      header = if (use_context) {
        sprintf("#: %s", lines)
      } else {
        "#: aggregated"
      },
      
      translator_comment_line = dplyr::if_else(
        !is.na(translator_comment) & translator_comment != "",
        paste0("#. ", translator_comment),
        ""
      ),
      
      msgid = sprintf(
        'msgid "%s"',
        text_clean %>%
          stringr::str_replace_all('\\\\', '\\\\\\\\') %>%
          stringr::str_replace_all('"', '\\\\\"')
      ),
      msgstr = dplyr::if_else(
        !is.na(msgstr) & nzchar(msgstr),
        sprintf('msgstr "%s"', msgstr),
        'msgstr ""'
      )
    ) %>%
    dplyr::mutate(
      po_block = paste(
        translator_comment_line[translator_comment_line != ""],
        header,
        msgid,
        msgstr,
        "",
        sep = "\n"
      )
    ) %>%
    dplyr::pull(po_block)
  
  readr::write_lines(c(header_block, po_lines), output_po)
  message("Wrote ", nrow(df), " entries to ", output_po, " [Language: ", lang, "]")
  invisible(df)
}

###############################
# 8. po2qmd: apply TM to QMD  #
###############################

po2qmd <- function(input, po, output = NULL, show_missing = FALSE) {
  if (!file.exists(input)) stop("Input file not found: ", input)
  if (!file.exists(po))    stop("PO file not found: ", po)

  tm <- parse_po(po)
  if (is.null(tm) || nrow(tm) == 0) {
    stop("No valid translations found in ", po)
  }
  tm <- dplyr::filter(tm, nzchar(msgstr))

  lines <- readr::read_lines(input)

  df <- extract_translatable_lines(lines)

  df <- df %>%
    dplyr::mutate(norm_key = normalize_msgid(text_clean))

  tm <- tm %>%
    dplyr::mutate(norm_key = normalize_msgid(msgid))

  merged <- df %>%
    dplyr::left_join(tm %>% dplyr::select(norm_key, msgstr), by = "norm_key") %>%
    dplyr::mutate(
      has_translation = !is.na(msgstr) & nzchar(msgstr),
      translated_line = purrr::pmap_chr(
        list(
          original_line = text,
          clean_segment = text_clean,
          translated    = msgstr
        ),
        function(original_line, clean_segment, translated) {
          
          # no translation → return original
          if (is.na(translated) || !nzchar(translated)) {
            return(original_line)
          }
          
          # Replace all occurrences of the cleaned extracted text in the original line
          gsub(
            pattern     = stringr::fixed(clean_segment),
            replacement = translated,
            x           = original_line,
            fixed       = TRUE
          )
        }
      )
    )

  translated <- dplyr::filter(merged, has_translation)
  if (nrow(translated) > 0) {
    cat("\nℹ️  ", nrow(translated),
        " line(s) successfully translated in ", input, ":\n", sep = "")
    purrr::walk2(translated$line_num, translated$text_clean, function(line_number, txt) {
      cat("  • [L", line_number, "] ", stringr::str_trunc(txt, 120), "\n", sep = "")
    })
  }

  missing <- dplyr::filter(merged, !has_translation)
  if (nrow(missing) > 0) {
    cat("\n⚠️  ", nrow(missing),
        " untranslated line(s) in ", input, "\n", sep = "")
    if (isTRUE(show_missing)) {
      purrr::walk2(missing$line_num, missing$text_trim, function(line_number, txt) {
        cat("  • [L", line_number, "] ", stringr::str_trunc(txt, 120), "\n", sep = "")
      })
    }
    cat("  (see PO file for details)\n")
  }

  lines[merged$line_num] <- merged$translated_line

  out_path <- output %||% input
  readr::write_lines(lines, out_path)
  cat("✅ Wrote translated file:", out_path, "\n")

  invisible(merged)
}

##########################################
# 9. po2qmd_folder: TM over folder tree  #
##########################################

po2qmd_folder <- function(input_dir,
                          output_dir,
                          po,
                          show_missing = FALSE,
                          exclude_pattern = NULL) {
  if (!dir.exists(input_dir))
    stop("Input directory not found: ", input_dir)
  if (!file.exists(po))
    stop("PO file not found: ", po)

  if (dir.exists(output_dir)) unlink(output_dir, recursive = TRUE)
  dir.create(output_dir, recursive = TRUE, showWarnings = FALSE)

  roots <- list.files(input_dir, full.names = TRUE, all.files = FALSE)
  file.copy(from = roots, to = output_dir, recursive = TRUE)

  qmd_files <- list.files(
    output_dir,
    pattern = "\\.qmd$",
    recursive = TRUE,
    full.names = TRUE
  )

  if (!is.null(exclude_pattern)) {
    qmd_files <- qmd_files[!stringr::str_detect(qmd_files, exclude_pattern)]
  }

  if (length(qmd_files) == 0) {
    message("No .qmd files found under ", output_dir)
    return(invisible(NULL))
  }

  message("📂 Found ", length(qmd_files), " QMD file(s) under ", output_dir)
  if (!is.null(exclude_pattern))
    message("Excluded files matching pattern: ", exclude_pattern)
  message("Applying translations from: ", po, "\n")

  results <- purrr::map_dfr(qmd_files, function(file_path) {
    message("→ Translating ", file_path)
    merged <- tryCatch(
      po2qmd(file_path, po = po, output = file_path, show_missing = show_missing),
      error = function(e) {
        warning("Failed to process ", file_path, ": ", conditionMessage(e))
        return(NULL)
      }
    )

    if (is.null(merged)) {
      dplyr::tibble(
        file = file_path,
        translated_lines = NA_integer_,
        total_lines = NA_integer_
      )
    } else {
      dplyr::tibble(
        file = file_path,
        translated_lines = sum(merged$has_translation),
        total_lines = nrow(merged)
      )
    }
  })

  message("\n✅ Completed translation for ", nrow(results), " file(s).")
  invisible(results)
}
