#include "console.hpp"
#include "simulator.hpp"

#include <iostream>
#include <string>

namespace {

void print_usage() {
  std::cout << "Использование:\n"
            << "  elevator_simulator [этажи] [лифты] [режим]\n\n"
            << "  этажи  - количество этажей (по умолчанию 10)\n"
            << "  лифты  - количество лифтов (по умолчанию 3)\n"
            << "  режим  - interactive (по умолчанию) или demo\n";
}

int parse_int(const char* text, int fallback) {
  try {
    return std::stoi(text);
  } catch (...) {
    return fallback;
  }
}

}  // namespace

int main(int argc, char* argv[]) {
  elevator::setup_console_encoding();

  int floors = 10;
  int elevators = 3;
  std::string mode = "interactive";

  if (argc > 1) {
    if (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h") {
      print_usage();
      return 0;
    }
    floors = parse_int(argv[1], floors);
  }
  if (argc > 2) {
    elevators = parse_int(argv[2], elevators);
  }
  if (argc > 3) {
    mode = argv[3];
  }

  try {
    elevator::Simulator simulator(floors, elevators);

    if (mode == "demo") {
      simulator.run_demo(30);
    } else {
      simulator.run_interactive();
    }
  } catch (const std::exception& ex) {
    std::cerr << "Ошибка: " << ex.what() << "\n";
    return 1;
  }

  return 0;
}
