# AGENTS.md

## Purpose

This file provides essential build, run, style, and workflow guidelines for agentic coding agents working in this repository. Follow these instructions and standards to maximize code quality, maintainability, and predictability in collaborative or autonomous development environments.

---

## 1. Build, Run, and Install

### 1.1. Build with CMake (Recommended)

- **Configure (with preset):**
  ```sh
  cmake --preset=linuxGCC
  ```

- **Or manual setup:**
  ```sh
  cmake -S . -B out/build/linuxGCC -DCMAKE_BUILD_TYPE=Debug
  ```

- **Build executable:**
  ```sh
  cmake --build out/build/linuxGCC
  ```

- **Executable output:**
  * Result: `open-shot` in the project root

### 1.2. Run the Game

- After building, from the project root:
  ```sh
  ./open-shot
  ```

### 1.3. Build Options
- C++ Standard: 17
- Required libraries: OpenCV, SFML (Graphics, Window, System)
- No install targets or packaging configured

---

## 2. Lint and Format

> **Note:** No formal linting or formatting tools are included. Manual consistency is expected. Use the following conventions, and run clang-format manually when possible.

### 2.1. Formatting Recommendations
- Indentation: 4 spaces
- Braces: K&R (opening brace on same line)
- Max line length: 100 (prefer breaking up long statements)
- File endings: Always end with newline
- Use spaces, not tabs

### 2.2. clang-format Usage (Suggested)
- To lint/format all files, from root (if installed):
  ```sh
  clang-format -i src/**/*.cpp src/**/*.hpp
  ```

- Provide a .clang-format file if changes are frequent or project size grows.

---

## 3. Testing

### 3.1. Status
- **No automated or unit test infrastructure exists.**
- There is NO test folder or testing framework presently configured.

### 3.2. Adding Tests (Recommend: Catch2 or GoogleTest)
- Agents are encouraged to introduce tests when extending the project.
- Example for CMake + Catch2:
  ```cmake
  # in CMakeLists.txt
  find_package(Catch2 3 REQUIRED)
  add_executable(run-tests tests/test_basic.cpp)
  target_link_libraries(run-tests PRIVATE Catch2::Catch2WithMain)
  add_test(NAME run-tests COMMAND run-tests)
  ```
- Place test files in a new `tests/` directory (not present yet).

### 3.3. Run All Tests (if present)
  ```sh
  ./run-tests
  ```

### 3.4. Run Single Test (if using Catch2 or GoogleTest)
- For Catch2:
  ```sh
  ./run-tests "TestName"
  ```
- For GoogleTest:
  ```sh
  ./run-tests --gtest_filter=SuiteName.TestName
  ```
> If no tests or runner are present, these commands will not work unless such infra is added.

---

## 4. Code Style and Conventions

### 4.1. Imports and Includes
- Standard/system libraries: <brackets> (e.g. `#include <SFML/Graphics.hpp>`)
- Project headers: "quotes" (e.g. `#include "Game/game.hpp"`)
- Group system includes before project includes

### 4.2. Formatting
- Indentation: 4 spaces
- No trailing whitespace
- Prefer self-explanatory, concise code, small functions

### 4.3. Types and Language Features
- Use C++17 and newer features when suitable (auto, smart pointers, structured bindings)
- Prefer STL containers (std::vector, std::string, etc.)
- Resource management: use RAII and smart pointers (std::unique_ptr, std::shared_ptr)

### 4.4. Naming Conventions
- **Classes/types:** PascalCase (`Game`, `Register`)
- **Member variables:** begin with `m_`, camelCase (e.g., `m_frameCount`)
- **Functions/methods:** camelCase (`run()`, `updateTexture()`)
- **Macros/constants:** UPPER_SNAKE_CASE
- **Namespaces:** lowercase, avoid global namespace where possible

### 4.5. Error Handling
- Check all resource allocations (files, cameras, etc.).
- Return error codes for recoverable errors (see `main.cpp`: `return -1`)
- Prefer exceptions for cases where execution cannot continue
- Avoid NULL; use nullptr in modern C++

### 4.6. File Structure
- Use header guards (`#pragma once` is acceptable)
- Each class gets its own header and source file
- Organize by feature/module when scaling

### 4.7. Comments and Documentation
- Public APIs: use brief comments above declarations
- Complex logic/algorithms: inline comments to clarify intent
- TODO/FIXME tags are allowed but should be actionable

---

## 5. Cursor/Copilot Rules

- **Cursor:** No `.cursor/rules/` or `.cursorrules` files present.
- **Copilot:** No `.github/copilot-instructions.md` file present.
- Agents should operate by best-practice and the above guidelines until further rules are created.

---

## 6. Useful References

- **CMake:** https://cmake.org/documentation/
- **SFML Docs:** https://www.sfml-dev.org/documentation/3.0.0/
- **OpenCV Docs:** https://docs.opencv.org/
- **Catch2:** https://github.com/catchorg/Catch2
- **GoogleTest:** https://github.com/google/googletest

---

## 7. Contributor Notes
- If you add tests or change code style, please update this file and create a minimal config if needed (e.g., `.clang-format`).
- If adding automation (CI, etc.), describe all agent-relevant tasks and update instructions above.

---

*Last updated: 2026-03-16*
