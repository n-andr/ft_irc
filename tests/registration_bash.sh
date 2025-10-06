#!/usr/bin/env bash
set -euo pipefail

SERVER_CMD="${SERVER_CMD:-./ircserv 6667 pass}"
HOST="${HOST:-127.0.0.1}"
PORT="${PORT:-6667}"
PASS="${PASS:-pass}"
LOGDIR="tests/logs"
LOGFILE="$LOGDIR/server.bash.log"

mkdir -p "$LOGDIR"
: > "$LOGFILE"

# Start server in background and log stdout+stderr
( $SERVER_CMD ) >"$LOGFILE" 2>&1 & SERVER_PID=$!

cleanup() {
  [[ -n "${C_TAIL_PID-}" ]] && kill "$C_TAIL_PID" 2>/dev/null || true
  [[ -n "${C_NC_PID-}"   ]] && kill "$C_NC_PID"   2>/dev/null || true
  [[ -n "${SERVER_PID-}" ]] && kill "$SERVER_PID" 2>/dev/null || true
}
trap cleanup EXIT

grep_wait() { # file pattern timeout_seconds
  local file="$1"
  local pat="$2"
  local to="$3"
  local end
  end=$(( SECONDS + to ))
  while (( SECONDS < end )); do
    if grep -E "$pat" "$file" >/dev/null 2>&1; then
      return 0
    fi
    sleep 0.05
  done
  return 1
}


new_client() {
  rm -f c.in c.out
  mkfifo c.in
  # feed FIFO into nc; capture output if you want in c.out
  tail -f c.in | nc "$HOST" "$PORT" > c.out & C_TAIL_PID=$!
  C_NC_PID=$!
  # wait for server to log new client
  grep_wait "$LOGFILE" "New client fd=" 3 || return 1
}
sendln() { printf "%s\r\n" "$*" > c.in; }
close_client() {
  [[ -n "${C_TAIL_PID-}" ]] && kill "$C_TAIL_PID" 2>/dev/null || true
  [[ -n "${C_NC_PID-}"   ]] && kill "$C_NC_PID"   2>/dev/null || true
  exec 3>&- || true
  rm -f c.in c.out
  unset C_TAIL_PID C_NC_PID
}

drain_log() { : > "$LOGFILE"; }   # clear log so we only match fresh lines

ok() { echo "OK"; }
ko() { echo "KO"; return 1; }

run_case() { # name commands... | then check markers from log
  local name="$1"; shift
  echo; echo "--- TEST: $name ---"
  drain_log
  new_client || { echo "KO detail: no accept log"; ko || true; close_client; return 1; }

  # run commands
  for line in "$@"; do sendln "$line"; done
  sleep 0.1

  # assertions based on your server markers
  case "$name" in
    PASS)
      grep_wait "$LOGFILE" "Correct password" 3 && ok || { echo "KO detail: PASS marker"; ko || true; }
      ;;
    NICK)
      grep_wait "$LOGFILE" "Correct password" 3 && \
      grep_wait "$LOGFILE" "NICK successful" 3 && ok || { echo "KO detail: NICK markers"; ko || true; }
      ;;
    USER)
      grep_wait "$LOGFILE" "Correct password" 3 && \
      grep_wait "$LOGFILE" "NICK successful" 3 && \
      grep_wait "$LOGFILE" "USER successful" 3 && ok || { echo "KO detail: USER markers"; ko || true; }
      ;;
  esac

  close_client
}

fail=0
run_case PASS "pass $PASS" || fail=$((fail+1))
run_case NICK "pass $PASS" "nick alice" || fail=$((fail+1))
run_case USER "pass $PASS" "nick bob" "user bob 0 * :Bob Real" || fail=$((fail+1))

echo; echo "=== SUMMARY ==="
echo "Failures: $fail"
exit "$fail"
