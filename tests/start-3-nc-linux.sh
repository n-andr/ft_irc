#!/usr/bin/env bash
set -euo pipefail

HOST="${HOST:-127.0.0.1}"
PORT="${PORT:-6667}"
PASS="${PASS:-pass}"

# pick a terminal
if command -v gnome-terminal >/dev/null 2>&1; then
  LAUNCH='gnome-terminal --'
elif command -v xterm >/dev/null 2>&1; then
  LAUNCH='xterm -e'
else
  echo "[err] need gnome-terminal or xterm installed" >&2
  exit 1
fi

launch_client() {
  local name="$1" nick="$2" real="$3"

  # Open a terminal that:
  #  1) sends PASS
  #  2) waits 0.1s, then sends NICK and USER
  #  3) forwards your typing with CRLF
  $LAUNCH env CLIENT_NAME="$name" NICK="$nick" REAL="$real" HOST="$HOST" PORT="$PORT" PASS="$PASS" bash -lc '
    set -euo pipefail
    exec 3>/dev/tty
    echo "[${CLIENT_NAME}] connecting to ${HOST}:${PORT}" >&3
    {
      # auto register
      printf "pass %s\r\n" "${PASS}"
      sleep 0.1
      printf "nick %s\r\n" "${NICK}"
	  sleep 0.1
      printf "user %s 0 * 0:%s\r\n" "${NICK}" "${REAL}"
      echo "[${CLIENT_NAME}] registered — type IRC commands" >&3

      # forward keyboard input with CRLF (Linux nc doesn’t add CR)
      stdbuf -o0 -e0 sed -u "s/$/\r/" < /dev/tty
    } | nc "${HOST}" "${PORT}"
  ' &
}

launch_client "client1" "alice" "Alice"
launch_client "client2" "bob"   "Bob"
launch_client "client3" "carol" "Carol"

echo "[info] 3 terminals launched. They auto-send PASS/NICK/USER, then accept your commands."
echo "[info] You can override: PASS=secret PORT=7000 HOST=127.0.0.1 ./start-3-nc-linux.sh"
