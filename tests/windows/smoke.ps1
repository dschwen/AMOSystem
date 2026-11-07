# Headless interpreter/file/bank regression tests for the portable bundle.
$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot
$failed = 0
$report = [System.Collections.Generic.List[string]]::new()
$keys = @('AMOS_HOME', 'AMOS_DISPLAY', 'AMOS_ASSIGNS', 'AMOS_DEVICES', 'AMOS_CONFIG')
$saved = @{}
foreach ($key in $keys) { $saved[$key] = [Environment]::GetEnvironmentVariable($key, 'Process') }
try {
    $env:AMOS_HOME = (Join-Path $root 'AMOS').Replace('\', '/')
    $env:AMOS_DISPLAY = 'none'
    $tests = Get-Content (Join-Path $root 'smoke.json') -Raw | ConvertFrom-Json
    foreach ($test in $tests) {
        $hd = Join-Path (Join-Path $root 'smoke') $test.name
        $ram = Join-Path $hd 'ram'
        if (Test-Path $ram) { Remove-Item -Recurse -Force $ram }
        foreach ($dir in @('out', 'ram', 'devices/Workbench/C', 'devices/Work', 'devices/RAM')) {
            New-Item -ItemType Directory -Force (Join-Path $hd $dir) | Out-Null
        }
        Get-ChildItem (Join-Path $hd 'out') -File | Remove-Item
        $path = $hd.Replace('\', '/')
        $env:AMOS_ASSIGNS = "AMOSTEST=$path;AMOSPRO=$env:AMOS_HOME;RAM=$path/ram"
        $env:AMOS_DEVICES = "$path/devices"
        $env:AMOS_CONFIG = "$path/empty.conf"
        Set-Content (Join-Path $hd 'empty.conf') '' -Encoding ASCII
        $p = Start-Process -FilePath (Join-Path $root 'amos.exe') -ArgumentList "AMOSTEST:$($test.name).AMOS" `
             -WorkingDirectory $root -PassThru -RedirectStandardOutput (Join-Path $hd 'stdout.txt') `
             -RedirectStandardError (Join-Path $hd 'stderr.txt')
        $problems = [System.Collections.Generic.List[string]]::new()
        $null = $p.Handle      # (keeps the handle: without it ExitCode may be empty in Windows PowerShell)
        if (-not $p.WaitForExit(30000)) {
            $p.Kill()
            $p.WaitForExit()
            $problems.Add('timeout')
        } elseif ($p.ExitCode -ne 0) {
            $problems.Add("exit $($p.ExitCode)")
        }
        foreach ($entry in $test.expected.PSObject.Properties) {
            $file = Join-Path (Join-Path $hd 'out') $entry.Name
            if (-not (Test-Path $file)) { $problems.Add("missing $($entry.Name)") }
            elseif ($entry.Name.EndsWith('.iff') -and (Get-Item $file).Length -gt 0) {
                # (a picture with text: its pixels are the font's, and the bundle has AROS's topaz, not the ROM's)
            }
            elseif ((Get-FileHash $file -Algorithm SHA256).Hash -ne $entry.Value) {
                $problems.Add("differs $($entry.Name)")
            }
        }
        foreach ($file in Get-ChildItem (Join-Path $hd 'out') -File) {
            # Match the existing regression runner's per-test output scope.
            if ($file.Name -ne "$($test.name).txt" -and -not $file.Name.StartsWith("$($test.name)_")) { continue }
            if ($null -eq $test.expected.PSObject.Properties[$file.Name]) {
                $problems.Add("unexpected $($file.Name)")
            }
        }
        if ($problems.Count) {
            $failed++
            $line = "FAIL $($test.name): $($problems -join '; ')"
        } else { $line = "PASS $($test.name)" }
        Write-Output $line
        $report.Add($line)
    }
    $report.Add("RESULT $failed failure(s)")
    $report | Set-Content (Join-Path $root 'smoke-results.txt') -Encoding UTF8
} finally {
    foreach ($key in $keys) { [Environment]::SetEnvironmentVariable($key, $saved[$key], 'Process') }
}
if ($failed) { exit 1 }
exit 0
