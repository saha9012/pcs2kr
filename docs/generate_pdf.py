#!/usr/bin/env python3
"""Генерация docs/SRS.pdf из docs/SRS.md (содержимое совпадает с Markdown)."""

from __future__ import annotations

import re
from pathlib import Path

from fpdf import FPDF

ROOT = Path(__file__).resolve().parent
SRS_MD = ROOT / "SRS.md"
OUTPUT = ROOT / "SRS.pdf"

STUDENT_NAME = "Недосекин Александр Сергеевич"
STUDENT_GROUP = "ЭФБО 15-24"

FONT_REGULAR = Path(r"C:\Windows\Fonts\arial.ttf")
FONT_BOLD = Path(r"C:\Windows\Fonts\arialbd.ttf")
FONT_MONO = Path(r"C:\Windows\Fonts\consola.ttf")


def strip_md_inline(text: str) -> str:
    text = re.sub(r"\*\*(.+?)\*\*", r"\1", text)
    text = re.sub(r"`(.+?)`", r"\1", text)
    text = text.replace("≥", ">=").replace("–", "-").replace("«", '"').replace("»", '"')
    return text.strip()


def parse_frontmatter(lines: list[str]) -> tuple[dict[str, str], list[str]]:
    if not lines or lines[0].strip() != "---":
        return {}, lines
    meta: dict[str, str] = {}
    body_start = 0
    for i in range(1, len(lines)):
        if lines[i].strip() == "---":
            body_start = i + 1
            break
        if ":" in lines[i]:
            key, val = lines[i].split(":", 1)
            meta[key.strip()] = val.strip().strip('"')
    return meta, lines[body_start:]


def parse_table_row(line: str) -> list[str]:
    parts = [p.strip() for p in line.strip().strip("|").split("|")]
    return [strip_md_inline(p) for p in parts]


def is_table_separator(line: str) -> bool:
    return bool(re.match(r"^\|[\s\-:|]+\|$", line.strip()))


class SRSPdf(FPDF):
    def __init__(self) -> None:
        super().__init__()
        self.set_auto_page_break(auto=True, margin=18)
        self.set_margins(20, 18, 20)
        if not FONT_REGULAR.exists():
            raise FileNotFoundError("Не найден Arial (C:\\Windows\\Fonts\\arial.ttf)")
        self.add_font("Arial", "", str(FONT_REGULAR))
        self.add_font("Arial", "B", str(FONT_BOLD))
        if FONT_MONO.exists():
            self.add_font("Mono", "", str(FONT_MONO))

    @property
    def content_width(self) -> float:
        return self.w - self.l_margin - self.r_margin

    def footer(self) -> None:
        self.set_y(-14)
        self.set_font("Arial", size=9)
        self.cell(0, 8, f"Страница {self.page_no()}", align="C")

    def title_page(self, title: str, subtitle: str) -> None:
        self.add_page()
        self.ln(45)
        self.set_font("Arial", "B", 17)
        self.multi_cell(self.content_width, 9, title, align="C")
        self.ln(10)
        self.set_font("Arial", "B", 15)
        self.multi_cell(self.content_width, 9, subtitle, align="C")
        self.ln(25)
        self.set_font("Arial", size=12)
        for line in (
            f"Студент: {STUDENT_NAME}",
            f"Группа: {STUDENT_GROUP}",
            "Дисциплина: Программирование корпоративных систем",
            "2025–2026 уч. г.",
        ):
            self.cell(0, 8, line, align="C", new_x="LMARGIN", new_y="NEXT")

    def heading(self, text: str, level: int) -> None:
        sizes = {1: 15, 2: 13, 3: 11}
        self.ln(4 if level > 1 else 6)
        self.set_font("Arial", "B", sizes.get(level, 11))
        self.multi_cell(self.content_width, 7, strip_md_inline(text))
        self.ln(2)

    def paragraph(self, text: str) -> None:
        if not text.strip():
            return
        self.set_font("Arial", size=11)
        self.multi_cell(self.content_width, 6, strip_md_inline(text))
        self.ln(2)

    def bullet(self, text: str) -> None:
        self.set_font("Arial", size=11)
        self.multi_cell(self.content_width, 6, f"  •  {strip_md_inline(text)}")
        self.ln(1)

    def code_block(self, lines: list[str], caption: str | None = None) -> None:
        if caption:
            self.set_font("Arial", "B", 10)
            self.multi_cell(self.content_width, 5, caption)
            self.ln(1)
        font = "Mono" if "Mono" in self.fonts else "Arial"
        self.set_font(font, size=8)
        self.set_fill_color(245, 245, 245)
        for line in lines:
            safe = line.replace("\t", "    ")
            self.multi_cell(self.content_width, 4.5, safe, fill=True)
        self.ln(3)

    def table(self, headers: list[str], rows: list[list[str]]) -> None:
        if not headers:
            return
        col_count = len(headers)
        widths = [self.content_width / col_count] * col_count
        if col_count == 2:
            widths = [self.content_width * 0.22, self.content_width * 0.78]
        elif col_count == 3:
            widths = [
                self.content_width * 0.12,
                self.content_width * 0.44,
                self.content_width * 0.44,
            ]

        line_h = 6

        def draw_row(cells: list[str], bold: bool = False) -> None:
            style = "B" if bold else ""
            self.set_font("Arial", style, 9)
            if self.get_y() + line_h * 3 > self.h - 20:
                self.add_page()
            x0 = self.l_margin
            y0 = self.get_y()
            x = x0
            max_h = line_h
            for i, cell in enumerate(cells):
                w = widths[i] if i < len(widths) else widths[-1]
                self.set_xy(x, y0)
                self.multi_cell(w, line_h, cell, border=1, align="L")
                cell_h = self.get_y() - y0
                max_h = max(max_h, cell_h)
                x += w
                self.set_xy(x, y0)
            self.set_xy(x0, y0 + max_h)

        draw_row(headers, bold=True)
        for row in rows:
            padded = row + [""] * (col_count - len(row))
            draw_row(padded[:col_count])
        self.ln(4)


def render_markdown(pdf: SRSPdf, lines: list[str]) -> None:
    i = 0
    paragraph_buf: list[str] = []

    def flush_paragraph() -> None:
        nonlocal paragraph_buf
        if paragraph_buf:
            pdf.paragraph(" ".join(paragraph_buf))
            paragraph_buf = []

    while i < len(lines):
        line = lines[i]
        stripped = line.strip()

        if stripped in ("", "\\newpage"):
            flush_paragraph()
            i += 1
            continue

        if stripped == "---":
            flush_paragraph()
            i += 1
            continue

        if stripped.startswith("```"):
            flush_paragraph()
            lang = stripped[3:].strip() or "code"
            block: list[str] = []
            i += 1
            while i < len(lines) and not lines[i].strip().startswith("```"):
                block.append(lines[i])
                i += 1
            caption = None
            if lang == "mermaid":
                caption = "Диаграмма (Mermaid, см. также docs/uml/*.puml):"
            pdf.code_block(block, caption=caption)
            i += 1
            continue

        if stripped.startswith("#"):
            flush_paragraph()
            level = len(stripped) - len(stripped.lstrip("#"))
            text = stripped[level:].strip()
            if level <= 3:
                pdf.heading(text, level)
            else:
                pdf.paragraph(text)
            i += 1
            continue

        if stripped.startswith("|") and "|" in stripped[1:]:
            flush_paragraph()
            headers = parse_table_row(stripped)
            i += 1
            if i < len(lines) and is_table_separator(lines[i]):
                i += 1
            rows: list[list[str]] = []
            while i < len(lines) and lines[i].strip().startswith("|"):
                rows.append(parse_table_row(lines[i]))
                i += 1
            pdf.table(headers, rows)
            continue

        if stripped.startswith("- "):
            flush_paragraph()
            pdf.bullet(stripped[2:])
            i += 1
            continue

        if re.match(r"^\d+\.\s", stripped):
            flush_paragraph()
            pdf.bullet(stripped)
            i += 1
            continue

        paragraph_buf.append(stripped)
        i += 1

    flush_paragraph()


def main() -> None:
    raw = SRS_MD.read_text(encoding="utf-8").splitlines()
    meta, body = parse_frontmatter(raw)

    title = meta.get("title", "Спецификация требований к программному обеспечению (SRS)")
    subtitle = meta.get("subtitle", "Симулятор работы лифтов в здании")

    pdf = SRSPdf()
    pdf.title_page(title, subtitle)
    pdf.add_page()
    render_markdown(pdf, body)

    pdf.output(str(OUTPUT))
    print(f"Создан: {OUTPUT}")


if __name__ == "__main__":
    main()
