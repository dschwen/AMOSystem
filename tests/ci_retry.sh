#!/bin/sh
# For CI on shared runners: the tests that failed in a host_run.py log are run once more (several
# of them depend on the wall clock: disk swaps, the tracker's counters). 0 if they all pass then.
# Usage: ci_retry.sh tests.log [host_run.py options...]
log=$1
shift
names=$(grep -E "^[a-z0-9_]+: " "$log" | grep -vE "^[a-z0-9_]+: ok" | cut -d: -f1 | sort -u | tr '\n' ' ')
[ -n "$names" ] || exit 1
echo "::warning::retrying the tests that failed: $names"
# shellcheck disable=SC2086
python3 tests/host_run.py "$@" $names > retry.log 2>&1
status=$?
tail -1 retry.log
exit $status
