#!/bin/sh
# Run from the exported probes directory on the target Mac. Each probe's
# compiler CPU type determines execution; an Intel/Rosetta run is not arm64.
set -u
cd "$(dirname "$0")" || exit 1
uname -a > runtime-system.txt
sw_vers >> runtime-system.txt
status=0
for probe in address-probe layout-probe context-probe jump-probe guest-memory-probe static-image-probe; do
    [ -f "$probe" ] || continue
    if command -v xattr >/dev/null 2>&1 && xattr -p com.apple.quarantine "$probe" >/dev/null 2>&1; then
        echo "$probe: download quarantine prevents local testing."
        echo "After verifying this is your AMOS probe, authorize it in Privacy & Security or run:"
        echo "  xattr -d com.apple.quarantine ./$probe"
        echo "Then rerun this script. No quarantine attributes have been changed."
        status=1
        continue
    fi
    if ! codesign --force --sign - --timestamp=none "$probe" > "$probe-sign.log" 2>&1; then
        echo "$probe: signing failed; see $probe-sign.log"
        status=1
        continue
    fi
    "./$probe" > "$probe-runtime.json" 2> "$probe-runtime.stderr"
    result=$?
    echo "$probe: exit $result"
    [ "$result" -eq 0 ] || status=1
    cat "$probe-runtime.json"
done
printf '%s\n' 'Address-probe exit 3 means the direct-pointer address-model gate failed; the measurements are in its JSON.'
exit "$status"
