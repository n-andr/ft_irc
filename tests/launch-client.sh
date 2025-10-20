set -euo pipefail
exec 3>/dev/tty
echo "[${CLIENT_NAME}] connecting to ${HOST}:${PORT}" >&3
{
	# auto register
	printf "pass %s\r\n" "${PASS}"
	sleep 0.1
	printf "nick %s\r\n" "${NICK}"
	sleep 0.1
	printf "user %s 0 0 * 0:%s\r\n" "${NICK}" "${REAL}"
	echo "[${CLIENT_NAME}] registered — type IRC commands" >&3

	# forward keyboard input with CRLF (Linux nc doesn’t add CR)
	if command -v osascript ; then
		:
		# OSX	
	else
		#LINUX
		stdbuf -o0 -e0 sed -u "s/$/\r/" < /dev/tty 
	fi
} | nc "${HOST}" "${PORT}" &