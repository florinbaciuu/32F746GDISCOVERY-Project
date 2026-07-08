#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="${SCRIPT_DIR}"

clean_dir() {
  local dir="$1"

  if [[ -d "${dir}" ]]; then
    printf 'Removing %s\n' "${dir}"
    rm -rf "${dir}"
  else
    printf 'Skipping %s (not found)\n' "${dir}"
  fi
}

case "${1:-all}" in
  all)
    clean_dir "${PROJECT_DIR}/build"
    clean_dir "${PROJECT_DIR}/build-release-size"
    clean_dir "${PROJECT_DIR}/build-release-size-lto"
    ;;
  debug)
    clean_dir "${PROJECT_DIR}/build"
    ;;
  release-size)
    clean_dir "${PROJECT_DIR}/build-release-size"
    ;;
  release-size-lto)
    clean_dir "${PROJECT_DIR}/build-release-size-lto"
    ;;
  *)
    printf 'Usage: %s [all|debug|release-size|release-size-lto]\n' "$0" >&2
    exit 1
    ;;
esac
