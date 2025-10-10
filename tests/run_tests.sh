#!/usr/bin/env bash
set -euo pipefail

# Config from env (with sane defaults)
SERVER_CMD="${SERVER_CMD:-./ircserv 6667 pass}"
HOST="${HOST:-127.0.0.1}"
PORT="${PORT:-6667}"
PASS="${PASS:-pass}"
VERBOSE="${VERBOSE:-0}"
TESTS_FILE="${TESTS_FILE:-tests/TESTS.txt}"

# Load tests from manifest if present; otherwise auto-discover (no shell globs in Makefile)
declare -a TESTS=()
if [[ -f "$TESTS_FILE" ]]; then
  # strip comments and blank lines
  while IFS= read -r line; do
    [[ -z "$line" || "$line" =~ ^[[:space:]]*# ]] && continue
    TESTS+=("$line")
  done < "$TESTS_FILE"
else
  # fallback: discover .expect in tests/ via find (still not using Makefile wildcards)
  while IFS= read -r f; do TESTS+=("$(basename "$f")"); done < <(find tests -maxdepth 1 -type f -name '*.expect' | sort)
fi

if [[ ${#TESTS[@]} -eq 0 ]]; then
  echo "No tests found."
  exit 0
fi

mkdir -p tests/logs
ok=0; fail=0

for t in "${TESTS[@]}"; do
  path="tests/$t"
  [[ -x "$path" ]] || chmod +x "$path"
  echo ">>> $path"
  if SERVER_CMD="$SERVER_CMD" HOST="$HOST" PORT="$PORT" PASS="$PASS" VERBOSE="$VERBOSE" "$path"; then
    ok=$((ok+1))
  else
    fail=$((fail+1))
  fi
done

echo "=== TEST SUMMARY === OK=$ok FAIL=$fail"
exit $fail
