#! /bin/bash
# FIXME: base_path must not contain any regular expression characters or ';'
set -euf -o pipefail

: ${DEBUG:=false}

if [ $# -lt 2 ]; then
	>&2 echo "usage: $(basename $0) <base_path> <file.proto>..."
	exit 1
fi

deps() {
	local raw_imports="$(grep import -- "$1")"

	local imports="$(printf "%s" "$raw_imports" | sed -e 's;^import ";'$base'/;' -e 's/";$//')"
	local headers="$(printf "%s" "$imports" | sed -e 's/.proto$/.pb.h/')"
	if [ -n "$imports" ] ; then
		if $DEBUG; then
			printf "# %s\n" "$1"
		fi
		printf "%s " "$imports"
		printf "%s\n" "$headers"
	fi
}

base="$1"
shift

for i in "$@"; do
	deps "$i"
done