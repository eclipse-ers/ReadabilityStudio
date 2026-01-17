# 📚 Documentation & Knowledge Base Dashboard

> **Project:** Readability Studio · **Last updated:** 2026-01-17

---

## Legend

- 🟢 **Complete** (≥ 95%)
- 🟡 **In Progress** (10–94%)
- 🔴 **Not Started** (< 10%)
- ⏳ **Needs Review** (pending proofread/QA)

`██████████` = 100%

`█████░░░░░` ≈ 50%

---

## Summary by Language

Overall documentation coverage for translatable docs (User Manual, Admin Guide, Programming Reference).

| Language | Progress | Status |
|:--:|:--:|:--:|
| German (`de`) | `░░░░░░░░░░` **0%** | 🔴 |
| English (`en`) | `████████░░` **83%** | 🟡 |
| Spanish (`es`) | `░░░░░░░░░░` **0%** | 🔴 |

---

### 1) 📖 User Manual

**Scope**: End-user tasks, workflows, troubleshooting, and feature overviews.  
**Sources**: Quarto/Markdown in `docs/` (e.g., `docs/sysadmin/linux-build.qmd`).  
**Localization**: Strings extracted into `locale/docs/*.po` and maintained with *POEdit*.

**Status**

| Language | Progress | Status |
|:--:|:--:|:--:|
| German | `░░░░░░░░░░` **0%** | 🔴 |
| English | `██████████` **100%** | 🟢 |
| Spanish | `░░░░░░░░░░` **0%** | 🔴 |

#### User Manual
- [x] All major features documented
- [x] Main screenshots updated to latest UI
- [x] Release notes updated


---

### 2) 🛠️ System Administrator Guide

**Scope**: Install/upgrade procedures, backup/restore, deployment topologies, and integration notes.  
**Sources**: Quarto/Markdown in `docs/` (e.g., `docs/admin-guide.qmd`).  
**Localization**: Tracked via `locale/docs/*.po` alongside the User Manual.

**Status**

| Language | Progress | Status |
|:--:|:--:|:--:|
| German | `░░░░░░░░░░` **0%** | 🔴 |
| English | `██████████` **100%** | 🟢 |
| Spanish | `░░░░░░░░░░` **0%** | 🔴 |

#### System Admin Guide
- [x] Windows install/upgrade workflows documented
- [ ] macOS install/upgrade workflows documented
- [ ] Linux install/upgrade workflows documented
- [x] Windows build system documented
- [x] macOS build system documented
- [x] Linux build system documented
- [x] Coding-style guide documented
- [x] Contributor process documented
- [ ] Code structure mapped


---

### 3) 🧪 Programming Reference (Lua / Automation API)

**Scope**: Public scripting entry points, configuration structures, return values, and code examples.  
**Sources**: Quarto/Markdown in `docs/` (e.g., `docs/readability-studio-api/libraries/Debug/Print.qmd`).  
**Localization**: Translations optional but recommended for major locales; examples may remain in English.

**Status**

| Language | Progress | Status |
|:--:|:--:|:--:|
| German | `░░░░░░░░░░` **0%** | 🔴 |
| English | `█████░░░░░` **50%** | 🟡 |
| Spanish | `░░░░░░░░░░` **0%** | 🔴 |

#### Programming Reference
- [x] Public Lua entry points documented
- [ ] All program functionality exposed to API
- [x] Code examples reviewed and ran


---

### 4) 📦 Doxygen API Reference (English Only)

This is a **developer-facing** reference generated from source comments. It is not localized, but we still track coverage and quality of documentation.

| Metric | Progress | Status |
|---|:--:|:--:|
| Overall documentation coverage | `███████░░░` **70%** | 🟡 |

#### Doxygen Coverage & Quality

#### Baseline Requirements
- [x] All public APIs have non-empty brief descriptions
- [x] Parameters and return values documented for stable APIs
- [x] Deprecated APIs marked and explained
- [ ] Warnings from Doxygen build kept under agreed threshold

#### Recommended Enhancements
- [ ] More code examples for complex workflows
- [ ] Consistent terminology with user and admin docs

