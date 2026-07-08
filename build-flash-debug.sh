#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="${SCRIPT_DIR}"
PRESET="debug"
ELF_FILE="${PROJECT_DIR}/build/stm32f746g_lvgl_framework.elf"

find_cube_programmer() {
  if command -v STM32_Programmer_CLI >/dev/null 2>&1; then
    command -v STM32_Programmer_CLI
    return
  fi

  local cubeide_tools="/Applications/STM32CubeIDE.app/Contents/Eclipse/plugins"
  local candidate
  candidate="$(find "${cubeide_tools}" -path "*/tools/bin/STM32_Programmer_CLI" -type f 2>/dev/null | sort -r | head -n 1 || true)"

  if [[ -n "${candidate}" ]]; then
    printf '%s\n' "${candidate}"
    return
  fi

  printf 'ERROR: STM32_Programmer_CLI was not found in PATH or STM32CubeIDE.\n' >&2
  exit 1
}

cd "${PROJECT_DIR}"

cmake --preset "${PRESET}"
cmake --build --preset "${PRESET}"

if [[ ! -f "${ELF_FILE}" ]]; then
  printf 'ERROR: expected firmware image not found: %s\n' "${ELF_FILE}" >&2
  exit 1
fi

CUBE_PROGRAMMER="$(find_cube_programmer)"
"${CUBE_PROGRAMMER}" -c port=SWD -w "${ELF_FILE}" -v -rst
