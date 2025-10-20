#!/usr/bin/env bash
#The #! (called a “shebang”) says “run this script using this program.”
#/usr/bin/env bash means: “Find the bash program in your system’s PATH and use it.”
#This is a flexible way to make sure the script uses bash instead of another shell like sh or zsh.

#treat as error if: -e (any command fails), -u (undefined variable), -o pipefail (any pied command fails)
set -euo pipefail

HOST="${HOST:-127.0.0.1}"
PORT="${PORT:-6667}"
PASS="${PASS:-pass}"

# Auto-detect DISPLAY for WSL2
if grep -qi microsoft /proc/version && [ -z "${DISPLAY:-}" ]; then
  export DISPLAY="$(grep -oP '(?<=nameserver ).*' /etc/resolv.conf 2>/dev/null):0"
fi

#command -v something -> checks if something exists / is installed
# Pick a terminal
if command -v gnome-terminal >/dev/null 2>&1; then
  LAUNCH='gnome-terminal -- bash -lc'
elif command -v xterm >/dev/null 2>&1; then
  LAUNCH='xterm -hold -e bash -lc'
else
  echo "[err] need gnome-terminal or xterm installed" >&2
  exit 1
fi

launch_client() {
  local name="$1" nick="$2" real="$3"

  CLIENT_NAME="$name" \
  NICK="$nick" \
  REAL="$real" \
  HOST="$HOST" \
  PORT="$PORT" \
  PASS="$PASS" \
  $LAUNCH "
    set -euo pipefail
    exec 3>/dev/tty
    echo \"[\$CLIENT_NAME] connecting to \$HOST:\$PORT\" >&3
    {
      printf \"pass %s\\r\\n\" \"\$PASS\"
      sleep 0.1
      printf \"nick %s\\r\\n\" \"\$NICK\"
      sleep 0.1
      printf \"user %s 0 * :%s\\r\\n\" \"\$NICK\" \"\$REAL\"
      echo \"[\$CLIENT_NAME] registered — type IRC commands\" >&3
      stdbuf -o0 -e0 sed -u \"s/\$/\\r/\" < /dev/tty
    } | nc \"\$HOST\" \"\$PORT\"
  " &
}

launch_client "client1" "alice" "Alice"
launch_client "client2" "bob"   "Bob"
launch_client "client3" "carol" "Carol"

echo "[info] 3 terminals launched. They auto-send PASS/NICK/USER, then accept your commands."
echo "[info] You can override: PASS=secret PORT=7000 HOST=127.0.0.1 ./start-3-nc-linux.sh"
