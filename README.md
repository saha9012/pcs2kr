# Симулятор работы лифтов в здании

Учебный проект по дисциплине «Программирование корпоративных систем».  
Многопоточное моделирование нескольких лифтов, обрабатывающих вызовы с этажей.

## Сборка

```bash
mkdir build && cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
```

На Windows с MinGW:

```powershell
mkdir build; cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
```

## Запуск

```bash
# Интерактивный режим (10 этажей, 3 лифта)
./elevator_simulator

# Свои параметры: этажи, лифты, режим
./elevator_simulator 15 4 interactive
./elevator_simulator 10 3 demo
```

### Команды интерактивного режима

| Команда | Описание |
|---------|----------|
| `u <этаж>` | Вызов лифта вверх с этажа |
| `d <этаж>` | Вызов лифта вниз с этажа |
| `c <лифт> <этаж>` | Кнопка этажа в кабине |
| `r` | Случайный вызов |
| `q` | Выход |

## Структура проекта

```
include/          — заголовочные файлы
src/              — исходный код
docs/             — требования и SRS
docs/uml/         — PlantUML-диаграммы
build/            — каталог сборки
```

## Документация

- [Сбор требований (Вigers)](docs/requirements.md)
- [SRS — спецификация требований](docs/SRS.md)
- PDF: `docs/SRS.pdf` (сгенерировать скриптом `docs/generate_pdf.ps1`)

Перед сдачей замените в SRS placeholders **ФИО** и **номер группы**.

## Технологии

- C++17
- std::thread, std::mutex, std::condition_variable
- CMake 3.16+
