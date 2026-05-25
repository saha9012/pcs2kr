#!/usr/bin/env python3
"""Generate SRS PDF for elevator simulator project."""

from pathlib import Path

from fpdf import FPDF

ROOT = Path(__file__).resolve().parent
OUTPUT = ROOT / "SRS.pdf"

# Replace before submission
STUDENT_NAME = "ФИО студента"
STUDENT_GROUP = "___"


class SRSReport(FPDF):
    def footer(self):
        self.set_y(-15)
        self.set_font("Arial", size=9)
        self.cell(0, 10, f"Страница {self.page_no()}", align="C")


def centered_line(pdf: SRSReport, text: str, h: float = 8) -> None:
    pdf.cell(0, h, text, align="C", new_x="LMARGIN", new_y="NEXT")


def section_title(pdf: SRSReport, text: str) -> None:
    pdf.set_font("Arial", "B", 14)
    w = pdf.w - pdf.l_margin - pdf.r_margin
    pdf.multi_cell(w, 8, text)
    pdf.ln(2)


def body_text(pdf: SRSReport, text: str) -> None:
    pdf.set_font("Arial", size=11)
    w = pdf.w - pdf.l_margin - pdf.r_margin
    pdf.multi_cell(w, 6, text)
    pdf.ln(1)


def bullet_list(pdf: SRSReport, items: list[str]) -> None:
    pdf.set_font("Arial", size=11)
    w = pdf.w - pdf.l_margin - pdf.r_margin
    for item in items:
        pdf.multi_cell(w, 6, f"- {item}")
    pdf.ln(2)


def main() -> None:
    pdf = SRSReport()
    pdf.set_auto_page_break(auto=True, margin=15)
    font_regular = Path(r"C:\Windows\Fonts\arial.ttf")
    font_bold = Path(r"C:\Windows\Fonts\arialbd.ttf")
    if not font_regular.exists():
        raise FileNotFoundError("Не найден Arial.ttf для кириллицы")
    pdf.add_font("Arial", "", str(font_regular))
    pdf.add_font("Arial", "B", str(font_bold))

    # Title page
    pdf.add_page()
    pdf.ln(40)
    pdf.set_font("Arial", "B", 18)
    centered_line(pdf, "Спецификация требований к программному обеспечению (SRS)", 10)
    pdf.ln(8)
    pdf.set_font("Arial", "B", 16)
    centered_line(pdf, "Симулятор работы лифтов в здании", 10)
    pdf.ln(20)
    pdf.set_font("Arial", size=12)
    centered_line(pdf, f"Студент: {STUDENT_NAME}")
    centered_line(pdf, f"Группа: {STUDENT_GROUP}")
    pdf.ln(8)
    centered_line(pdf, "Дисциплина: Программирование корпоративных систем")
    centered_line(pdf, "2025-2026 уч. г.")

    # 1. Introduction
    pdf.add_page()
    section_title(pdf, "1. Введение")
    section_title(pdf, "1.1. Цель")
    body_text(
        pdf,
        "Документ описывает требования к программному обеспечению «Симулятор работы лифтов "
        "в здании» — многопоточной системе моделирования группы лифтов.",
    )
    section_title(pdf, "1.2. Область действия")
    body_text(pdf, "В scope: моделирование здания, многопоточные лифты, диспетчер, консольный UI.")
    body_text(pdf, "Вне scope: 3D-интерфейс, реальное оборудование, СУБД, сеть.")
    section_title(pdf, "1.3. Определения")
    bullet_list(
        pdf,
        [
            "Hall Call — вызов с этажа (вверх/вниз)",
            "Cabin Call — выбор этажа в кабине",
            "Dispatcher — диспетчер вызовов",
            "SRS — Software Requirements Specification",
        ],
    )

    # 2. General description
    section_title(pdf, "2. Общее описание")
    section_title(pdf, "2.1. Описание продукта")
    body_text(
        pdf,
        "Консольное приложение на C++17. Каждый лифт — отдельный поток. "
        "Диспетчер в фоне распределяет вызовы по эвристическому алгоритму.",
    )
    section_title(pdf, "2.2. Функции продукта")
    bullet_list(
        pdf,
        [
            "Инициализация здания и лифтов",
            "Приём вызовов с этажей и из кабины",
            "Диспетчеризация и движение лифтов",
            "Интерактивный и демо-режим",
        ],
    )
    section_title(pdf, "2.3. Характеристики пользователей")
    bullet_list(
        pdf,
        [
            "Оператор/студент — запускает симуляцию, вводит команды",
            "Разработчик — настраивает алгоритмы",
            "Преподаватель — проверяет соответствие ТЗ",
        ],
    )

    # 3. Detailed requirements
    pdf.add_page()
    section_title(pdf, "3. Детальные требования")
    section_title(pdf, "3.1. Методология сбора")
    bullet_list(
        pdf,
        [
            "Анализ предметной области лифтовых систем",
            "User Stories и Story Mapping",
            "Классификация по иерархии Вигерса",
        ],
    )

    section_title(pdf, "3.2. Бизнес-требования")
    bullet_list(
        pdf,
        [
            "BR-01: Снизить время ожидания за счёт диспетчеризации",
            "BR-02: Безопасная имитация без реального оборудования",
            "BR-03: Тестирование алгоритмов до внедрения",
            "BR-04: Поддержка разного числа этажей и лифтов",
        ],
    )

    section_title(pdf, "3.3. Пользовательские требования")
    bullet_list(
        pdf,
        [
            "UR-01: Вызов лифта вверх с этажа",
            "UR-02: Вызов лифта вниз с этажа",
            "UR-03: Выбор этажа в кабине",
            "UR-04: Просмотр статуса лифтов",
            "UR-05: Демо-режим",
        ],
    )

    section_title(pdf, "3.4. Функциональные требования")
    bullet_list(
        pdf,
        [
            "FR-01: Здание с N этажами и M лифтами",
            "FR-02–FR-03: Hall и Cabin вызовы",
            "FR-04: Диспетчер назначает лифт",
            "FR-05–FR-06: Движение и открытие дверей",
            "FR-07–FR-08: Многопоточность лифтов и диспетчера",
            "FR-09–FR-10: Интерактивный и demo режимы",
        ],
    )

    section_title(pdf, "3.5. Нефункциональные требования")
    bullet_list(
        pdf,
        [
            "NFR-01: C++17",
            "NFR-02: std::thread, mutex, condition_variable",
            "NFR-04: Корректное завершение потоков",
            "NFR-05: Сборка через CMake",
        ],
    )

    # 4. UML
    pdf.add_page()
    section_title(pdf, "4. Диаграммы UML")
    section_title(pdf, "4.1. Use Case Diagram")
    body_text(pdf, "Акторы: Пассажир, Оператор.")
    body_text(
        pdf,
        "Прецеденты: вызвать лифт вверх/вниз, выбрать этаж в кабине, "
        "просмотреть статус, запустить демо, диспетчеризовать вызовы, перемещать лифт.",
    )
    body_text(pdf, "Связи: Пассажир → вызовы; Оператор → статус и демо; вызовы include диспетчеризацию.")

    section_title(pdf, "4.2. Class Diagram")
    body_text(pdf, "Основные классы: Simulator, Building, Elevator, Dispatcher, CallRequest.")
    body_text(
        pdf,
        "Simulator агрегирует Building и Dispatcher. Building содержит коллекцию Elevator "
        "и очередь CallRequest. Dispatcher использует Building для назначения вызовов.",
    )

    section_title(pdf, "4.3. Sequence Diagram — «Вызов с этажа»")
    bullet_list(
        pdf,
        [
            "Пассажир → Simulator: команда u 5",
            "Simulator → Building: submit_call(HallUp, 5)",
            "Dispatcher → Building: pop_next_call()",
            "Dispatcher → Elevator: assign_hall_call()",
            "Elevator: move_one_floor() в цикле, open_doors()",
            "Elevator → Building: on_elevator_arrived()",
            "Simulator → Пассажир: обновление статуса",
        ],
    )
    body_text(pdf, "PlantUML-исходники: docs/uml/*.puml. Mermaid-версии: docs/SRS.md.")

    pdf.output(str(OUTPUT))
    print(f"Created: {OUTPUT}")


if __name__ == "__main__":
    main()
