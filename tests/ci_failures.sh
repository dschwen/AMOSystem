#!/bin/sh
# For CI: what each differing text test of a host_run.py log did, as annotations (the log of a run
# needs a login, annotations do not). Usage: ci_failures.sh tests.log [build/host-test]
log=$1
runs=${2:-build/host-test}
say() { printf '%s' "$1" | tr '\n' '~' | head -c 1200 | sed 's/^/::error::/'; echo; }
grep -E "^[a-z0-9_]+: [A-Za-z0-9_.]+ differs" "$log" | head -4 | while read -r test file rest; do
    name=${test%:}
    got=$(find "$runs/$name" -name "$file" | head -1)
    want=$(ls tests/golden/"$file" tests/host_only/"$file" 2> /dev/null | head -1)
    say "$name diff (got < > want): $(diff "$got" "$want" 2>&1 | head -8)"
    say "$name log: $(grep -v '^amos: file' "$runs/$name/stdout.log" 2> /dev/null | tail -8)"
    say "$name files: $(grep '^amos: file' "$runs/$name/stdout.log" 2> /dev/null | sed "s/amos: file '\(.*\)' -> '.*' mode [0-9]* error \([0-9]*\)/\1=\2/" | tail -10 | tr '\n' ' ')"
done
