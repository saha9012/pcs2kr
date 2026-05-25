# Симулятор работы лифтов в здании

Учебный проект по дисциплине «Программирование корпоративных систем».  
Многопоточное моделирование нескольких лифтов на **C++17** (Windows).

Репозиторий: [github.com/saha9012/pcs2kr](https://github.com/saha9012/pcs2kr)

## Требования (Windows)

- [Git for Windows](https://git-scm.com/download/win)
- [CMake](https://cmake.org/download/) (3.16+)
- Компилятор **MinGW-w64** (g++) — например [MSYS2](https://www.msys2.org/) или готовый MinGW в `PATH`
- Рекомендуется **Windows Terminal** или PowerShell (для корректной кириллицы)

Проверка:

```powershell
g++ --version
cmake --version
```

## Сборка

В корне проекта:

```powershell
cd c:\prodject\pcs\2
mkdir build -Force
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
```

Исполняемый файл: `build\elevator_simulator.exe`

## Запуск

```powershell
cd c:\prodject\pcs\2\build
.\elevator_simulator.exe
```

Параметры: `этажи` `лифты` `режим`

```powershell
# 10 этажей, 3 лифта, интерактивный режим (по умолчанию)
.\elevator_simulator.exe

# 15 этажей, 4 лифта
.\elevator_simulator.exe 15 4 interactive

# демо 30 секунд со случайными вызовами
.\elevator_simulator.exe 10 3 demo
```

Если кириллица в консоли отображается некорректно:

```powershell
chcp 65001
.\elevator_simulator.exe
```

### Команды (интерактивный режим)

| Команда | Описание |
|---------|----------|
| `u <этаж>` | Вызов лифта вверх с этажа |
| `d <этаж>` | Вызов лифта вниз с этажа |
| `c <лифт> <этаж>` | Кнопка этажа в кабине (номер лифта 1..M) |
| `r` | Случайный вызов |
| `q` | Выход |

## Структура проекта

```
include\          заголовки
src\              исходники C++
docs\             SRS, требования, PDF
docs\uml\         PlantUML-диаграммы
build\            каталог сборки (не в git)
```

## Документация

| Файл | Описание |
|------|----------|
| [docs/requirements.md](docs/requirements.md) | Сбор требований (Вигерс, User Stories) |
| [docs/SRS.md](docs/SRS.md) | Спецификация SRS (исходник) |
| [docs/SRS.pdf](docs/SRS.pdf) | SRS для сдачи (PDF) |

Пересборка PDF после правок `SRS.md`:

```powershell
py -3 docs\generate_pdf.py
```

## Технологии

- C++17, `std::thread`, `std::mutex`, `std::condition_variable`
- CMake + MinGW (Windows)
