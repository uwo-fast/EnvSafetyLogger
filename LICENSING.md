# Licensing

This document states which licence covers which parts of this repository, and
explains what that means. [What applies where](#what-applies-where) is the operative
section — it is how these files are made available. The commentary around it is for
convenience; the binding terms are the full texts in [`LICENSES/`](LICENSES/).

Each part of the repository is licensed separately by component — not
dual-licensed, since you do not get to choose which licence covers a given file:

- **Software** — GNU General Public License v3.0 or later (`GPL-3.0-or-later`) —
  [`LICENSES/GPL-3.0-or-later.txt`](LICENSES/GPL-3.0-or-later.txt)
- **Hardware** — CERN Open Hardware Licence Version 2, Strongly Reciprocal
  (`CERN-OHL-S-2.0`) — [`LICENSES/CERN-OHL-S-2.0.txt`](LICENSES/CERN-OHL-S-2.0.txt)
- **Research data** — Creative Commons Attribution 4.0 International (`CC-BY-4.0`) —
  <https://creativecommons.org/licenses/by/4.0/legalcode>

The root [`LICENSE`](LICENSE) is a copy of the GPL text. GitHub reports only one
licence per repository and reads that file, so this document is where the full
picture lives.

Releases up to and including `v1.0.0` were published under `GPL-3.0` alone, and
copies obtained from them keep that licence.

## What applies where

| Path | Licence |
| --- | --- |
| `firmware/`, `justfile`, `.github/`, and the plotting scripts in `data/` (`*.py`) | `GPL-3.0-or-later` |
| `hardware/` — the KiCad schematic and board, and the PDF and SVG exported from them | `CERN-OHL-S-2.0` |
| `docs/BOM_v0.ods` — the bill of materials | `CERN-OHL-S-2.0` |
| `docs/assets/pic0_schematic.png` — a render of the schematic | `CERN-OHL-S-2.0` |
| `data/` — the logged measurements (`*.CSV`, `*.csv`) and the plots made from them | `CC-BY-4.0` |
| `docs/`, `README.md` | see below |

**Documentation.** Documentation specific to software is licensed
`GPL-3.0-or-later`, and documentation specific to hardware `CERN-OHL-S-2.0`. A
document covering both — such as the build guide in `docs/README.md` and its photos
in `docs/assets/`, other than the schematic render above — and documentation specific to neither are licensed under both,
at your option.

## Third-party components

Anything vendored or included keeps its own licence, unaffected by this document.

| Component | Licence |
| --- | --- |
| `docs/mq-datasheets/*.pdf` — the Winsen MQ-8, MQ-9B, MQ136 and MQ137 manuals | © Zhengzhou Winsen Electronics Technology Co., Ltd.; included for reference, not covered by this document |

The Arduino libraries the firmware builds against are installed at build time (see
the `justfile`) and are not part of this repository.

## Contributions

"Contribution" means any work of authorship intentionally submitted for inclusion
in this project — a pull request, patch, commit, issue, or other communication —
excluding anything conspicuously marked "Not a Contribution".

Unless you state otherwise, contributions you submit are licensed under whichever
of the licences above covers the files they touch, with no additional terms. By
submitting a contribution you represent that you have the right to license it on
those terms.

## Patents

Both GPL-3.0 (section 11) and CERN-OHL-S-2.0 (section 7) contain express patent
provisions. Review them before using, modifying, or distributing this project.

## Disclaimer of warranty and liability

A summary of terms in the licences themselves — GPL-3.0 sections 15–17 and
CERN-OHL-S-2.0 section 6.

THIS PROJECT, INCLUDING ALL SOFTWARE AND HARDWARE DESIGNS, IS PROVIDED "AS IS"
WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
NON-INFRINGEMENT.

TO THE EXTENT PERMITTED BY APPLICABLE LAW, IN NO EVENT SHALL THE AUTHORS,
CONTRIBUTORS, OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER
LIABILITY ARISING FROM, OUT OF, OR IN CONNECTION WITH THIS PROJECT.

### Additional disclaimer

The following is not a summary of either licence. It is an additional disclaimer of
the kind GPL-3.0 section 7(a) permits, offered alongside them:

HARDWARE DESIGNS HERE HAVE NOT BEEN INDEPENDENTLY VERIFIED OR CERTIFIED FOR SAFETY
OR REGULATORY COMPLIANCE. ANY PHYSICAL PRODUCT MANUFACTURED FROM THEM IS PRODUCED
ENTIRELY AT YOUR OWN RISK. TO THE EXTENT PERMITTED BY APPLICABLE LAW, THE AUTHORS
AND CONTRIBUTORS ARE NOT RESPONSIBLE FOR ANY PERSONAL INJURY, PROPERTY DAMAGE, OR
OTHER HARM RESULTING FROM THE USE OR MANUFACTURE OF PRODUCTS BASED ON THEM.

## Trademarks

Neither licence grants trademark rights. This project does not grant permission to
use the names, trademarks, or logos of the FAST research group or of the project's
contributors, except as needed to describe the project's origin.

## Precedence

Where the commentary in this document and the full licence texts conflict, the full
texts prevail. The scope table above is not commentary — it is the statement of
which files are made available under which terms.
