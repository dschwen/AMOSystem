# Run installer lifecycle checks from an elevated PowerShell in a disposable VM.
# User mode must be run separately from a NON-elevated PowerShell.
param(
    [ValidateSet('Probe','Install','User','Uninstall')][string]$Mode = 'Probe',
    [string]$Packages = $PSScriptRoot,
    [string]$Version = '0.1.0',
    [string]$Artifacts = (Join-Path $PSScriptRoot 'msi-test-results')
)
$ErrorActionPreference = 'Stop'
if ([IntPtr]::Size -ne 8) { throw 'Run this script in 64-bit PowerShell.' }
$principal = [Security.Principal.WindowsPrincipal]::new([Security.Principal.WindowsIdentity]::GetCurrent())
$admin = $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
if ($Mode -eq 'User' -and $admin) { throw 'Run User mode from a non-elevated PowerShell.' }
if ($Mode -ne 'User' -and -not $admin) { throw 'Run installer lifecycle modes from an elevated PowerShell.' }
New-Item -ItemType Directory -Force $Artifacts | Out-Null
$Artifacts = (Resolve-Path $Artifacts).Path
$Packages = (Resolve-Path $Packages).Path
$work = Join-Path $env:TEMP ('amos-msi-test-' + [Guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory $work | Out-Null
$results = [System.Collections.Generic.List[object]]::new()
function Record($name, $ok) {
    $results.Add(@{ name=$name; passed=[bool]$ok })
    Write-Output "$(if ($ok) {'PASS'} else {'FAIL'}) $name"
    if (-not $ok) { throw $name }
}
function Metadata($stem) {
    $data = Get-Content (Join-Path $Packages ($stem+'.json')) -Raw | ConvertFrom-Json
    $source = Join-Path $Packages ($stem+'.msi')
    if ((Get-FileHash $source -Algorithm SHA256).Hash.ToLower() -ne $data.sha256) { throw "MSI hash mismatch: $stem" }
    Copy-Item $source $work
    return $data
}
function Invoke-Msi($name, $arguments, $allowed=@(0,3010)) {
    $log = Join-Path $Artifacts ($name+'.log')
    $full = @($arguments) + @('/qn','/norestart','REBOOT=ReallySuppress','/L*v',('"'+$log+'"'))
    $p = Start-Process msiexec.exe -ArgumentList $full -PassThru
    $null = $p.Handle
    if (-not $p.WaitForExit(180000)) { throw "MSI timed out: $name (see $log)" }
    $code = $p.ExitCode
    Record "$name exit $code" ($allowed -contains $code)
    return $code
}
function Check-Payload($root, $metadata) {
    foreach ($entry in $metadata.payload.PSObject.Properties) {
        $path = Join-Path $root $entry.Name
        Record "payload $($entry.Name)" ((Test-Path $path) -and (Get-FileHash $path -Algorithm SHA256).Hash.ToLower() -eq $entry.Value)
    }
}
try {
    if ($Mode -eq 'Probe') {
        $names = @('0.1.0','0.1.1','0.1.2')
        $meta = @{}
        foreach ($v in $names) { $meta[$v] = Metadata "amos-msi-probe-$v-windows-x64" }
        $target = Join-Path $env:ProgramFiles 'AMOS MSI Probe'
        if (Test-Path $target) { throw 'Probe folder already exists; use a clean VM snapshot or uninstall the prior probe first.' }
        $one = Join-Path $work 'amos-msi-probe-0.1.0-windows-x64.msi'
        $two = Join-Path $work 'amos-msi-probe-0.1.1-windows-x64.msi'
        $three = Join-Path $work 'amos-msi-probe-0.1.2-windows-x64.msi'
        Invoke-Msi 'probe-install' @('/i',('"'+$one+'"')) | Out-Null
        Check-Payload $target $meta['0.1.0']
        Remove-Item (Join-Path $target 'one.txt')
        Invoke-Msi 'probe-repair' @('/fa',$meta['0.1.0'].product_code) | Out-Null
        Check-Payload $target $meta['0.1.0']
        Invoke-Msi 'probe-upgrade' @('/i',('"'+$two+'"')) | Out-Null
        Check-Payload $target $meta['0.1.1']
        Invoke-Msi 'probe-downgrade-blocked' @('/i',('"'+$one+'"')) @(1603,1638) | Out-Null
        Check-Payload $target $meta['0.1.1']
        Invoke-Msi 'probe-forced-rollback' @('/i',('"'+$three+'"'),'MSI_PROBE_FAIL=1') @(1603) | Out-Null
        Check-Payload $target $meta['0.1.1']
        Record 'rollback restores registry version' ((Get-ItemProperty 'HKLM:\Software\AMOSystem\InstallerProbe').Version -eq '0.1.1')
        Invoke-Msi 'probe-uninstall' @('/x',$meta['0.1.1'].product_code) | Out-Null
        Record 'probe payload removed' (-not (Test-Path $target))
        Record 'probe shortcut removed' (-not (Test-Path (Join-Path ([Environment]::GetFolderPath('CommonPrograms')) 'AMOS MSI Probe')))
    } elseif ($Mode -eq 'Install') {
        $stem = "amos-$Version-windows-x64"
        $data = Metadata $stem
        Invoke-Msi 'amos-install' @('/i',('"'+(Join-Path $work ($stem+'.msi'))+'"')) | Out-Null
        Check-Payload (Join-Path $env:ProgramFiles 'AMOSystem') $data
        $command = (Get-Item 'Registry::HKEY_LOCAL_MACHINE\Software\Classes\AMOSystem.Program\shell\open\command').GetValue('')
        Record 'quoted editor document handler registered' ($command -match '".*amos.exe" --open-document "%1"')
    } elseif ($Mode -eq 'User') {
        $install = Join-Path $env:ProgramFiles 'AMOSystem'
        Record 'write-test path initially absent' (-not (Test-Path (Join-Path $install 'user-write-test')))
        $wrote = $false
        try { [IO.File]::WriteAllText((Join-Path $install 'user-write-test'),'test'); $wrote = $true } catch [UnauthorizedAccessException] {}
        if ($wrote) { Remove-Item (Join-Path $install 'user-write-test') }
        Record 'Program Files is read-only to standard token' (-not $wrote)
        Copy-Item (Join-Path $Packages 'MSI document with spaces.AMOS') $work
        $file = Join-Path $work 'MSI document with spaces.AMOS'
        $savedHome = $env:AMOS_HOME; $savedDisplay = $env:AMOS_DISPLAY
        $p = $null
        try {
            Remove-Item Env:AMOS_HOME -ErrorAction SilentlyContinue
            $env:AMOS_DISPLAY = 'none'
            $p = Start-Process (Join-Path $install 'amos.exe') -ArgumentList @('--open-document',('"'+$file+'"')) -PassThru `
                -WorkingDirectory $work -RedirectStandardOutput (Join-Path $Artifacts 'document.stdout.txt') `
                -RedirectStandardError (Join-Path $Artifacts 'document.stderr.txt')
            $null = $p.Handle
            Start-Sleep -Seconds 4
            Record 'editor remains running after document load' (-not $p.HasExited)
            Record 'document was not executed' (-not (Test-Path (Join-Path $work 'document-executed.txt')))
            $p.Kill(); $p.WaitForExit()
            $runtime = Join-Path ([Environment]::GetFolderPath('LocalApplicationData')) 'AMOSystem/runtime'
            Record 'per-user configuration seeded' (Test-Path (Join-Path $runtime 'APSystem/AMOSPro_Editor_Config'))
        } finally {
            if ($p -and -not $p.HasExited) { $p.Kill(); $p.WaitForExit() }
            $env:AMOS_HOME = $savedHome; $env:AMOS_DISPLAY = $savedDisplay
        }
    } else {
        $data = Metadata "amos-$Version-windows-x64"
        Invoke-Msi 'amos-uninstall' @('/x',$data.product_code) | Out-Null
        Record 'application binary removed' (-not (Test-Path (Join-Path $env:ProgramFiles 'AMOSystem/amos.exe')))
        Record 'owned ProgID removed' (-not (Test-Path 'Registry::HKEY_LOCAL_MACHINE\Software\Classes\AMOSystem.Program'))
    }
} finally {
    $results | ConvertTo-Json -Depth 5 | Set-Content (Join-Path $Artifacts ($Mode+'-results.json')) -Encoding UTF8
    Remove-Item -Recurse -Force $work -ErrorAction SilentlyContinue
}
