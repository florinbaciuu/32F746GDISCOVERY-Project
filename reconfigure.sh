#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="${SCRIPT_DIR}"

reconfigure_preset() {
  local preset="$1"
  local build_dir="$2"

  printf 'Reconfiguring %s\n' "${preset}"
  rm -f "${build_dir}/CMakeCache.txt"
  rm -rf "${build_dir}/CMakeFiles"

  cmake --preset "${preset}"
}

cd "${PROJECT_DIR}"

case "${1:-all}" in
  all)
    reconfigure_preset "debug" "${PROJECT_DIR}/build"
    reconfigure_preset "release-size" "${PROJECT_DIR}/build-release-size"
    reconfigure_preset "release-size-lto" "${PROJECT_DIR}/build-release-size-lto"
    ;;
  debug)
    reconfigure_preset "debug" "${PROJECT_DIR}/build"
    ;;
  release-size)
    reconfigure_preset "release-size" "${PROJECT_DIR}/build-release-size"
    ;;
  release-size-lto)
    reconfigure_preset "release-size-lto" "${PROJECT_DIR}/build-release-size-lto"
    ;;
  *)
    printf 'Usage: %s [all|debug|release-size|release-size-lto]\n' "$0" >&2
    exit 1
    ;;
esac
