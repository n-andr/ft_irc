#!/usr/bin/env bash
# irc-macos-fifo-fixed.sh — minimal, robust, macOS-only
# Usage: ./irc-macos-fifo-fixed.sh [HOST] [PORT] [PASS]
set -eu

HOST="${1:-127.0.0.1}"
PORT="${2:-6667}"
PASS="${3:-pass}"

# three accounts to open (edit if needed)
NICKS=( "nick1" "nick2" "nick3" )
USERS=( "user1" "user2" "user3" )
REALNAMES=( "Real Name 1" "Real Name 2" "Real Name 3" )

make_helper() {
  local i="$1" nick="$2" user="$3" real="$4"
  local tmp="/tmp/irc-win-$$-$i.sh"

  cat >"$tmp" <<'EOF'
#!/usr/bin/env bash
set -eu

HOST='__HOST__'
PORT='__PORT__'
PASS='__PASS__'
NICK='__NICK__'
USER='__USER__'
REAL='__REAL__'

PATH="/opt/homebrew/bin:/usr/local/bin:/usr/bin:/bin:/usr/sbin:/sbin:$PATH"
export PATH

FIFO="/tmp/irc-fifo-$$-__IDX__"
rm -f "$FIFO"
mkfifo "$FIFO"

# writer: single atomic write of the 3 registration lines (CRLF), then close FIFO
(
  sleep 1
  printf $'PASS %s\r\nNICK %s\r\nUSER %s 0 * :%s\r\n' "$PASS" "$NICK" "$USER" "$REAL" > "$FIFO"
  # writer exits -> closes FIFO; cat will then continue with terminal stdin
) &

echo "Connecting to $HOST:$PORT (nick: $NICK) — registration will be sent shortly..."
cat "$FIFO" - | nc "$HOST" "$PORT"

echo
echo "nc exited (status $?); dropping to a shell for debugging. Press Ctrl-D to close."
exec "$SHELL"
EOF

  sed -e "s|__HOST__|${HOST}|g" \
      -e "s|__PORT__|${PORT}|g" \
      -e "s|__PASS__|${PASS}|g" \
      -e "s|__NICK__|${nick}|g" \
      -e "s|__USER__|${user}|g" \
      -e "s|__REAL__|${real}|g" \
      -e "s|__IDX__|${i}|g" \
      <"$tmp" >"${tmp}.out"
  mv "${tmp}.out" "$tmp"
  chmod +x "$tmp"
  printf "%s" "$tmp"
}

# create helpers
helpers=()
for i in 0 1 2; do
  helpers[i]="$( make_helper "$i" "${NICKS[$i]}" "${USERS[$i]}" "${REALNAMES[$i]}" )"
done

# open each helper in a new Terminal.app window
for i in 0 1 2; do
  helper="${helpers[$i]}"
  /usr/bin/osascript <<APPLESCRIPT >/dev/null
tell application "Terminal"
  do script "bash '$helper'"
  activate
end tell
APPLESCRIPT
  # cleanup helper after a short delay
  ( sleep 8; rm -f "$helper" ) &
  sleep 0.12
done

echo "Launched 3 Terminal windows to ${HOST}:${PORT}. PASS/NICK/USER will be sent after ~1s."
