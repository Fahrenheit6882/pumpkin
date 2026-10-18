<#
.SYNOPSIS
    Copies mp3 files to an HW-896 (JQ6500-clone) SD card in filename order.

.DESCRIPTION
    The HW-896 V2.0.2 clone plays files in the order they were WRITTEN to the
    SD card, not by filename. This script copies files from a source folder
    to a destination (e.g. the SD card drive) one at a time, sorted by
    filename, so playback order matches the numeric prefix in the filenames
    (001_..., 002_..., etc).

    By default it clears existing mp3/wav files out of the destination first,
    since leftover files from a previous copy would otherwise keep their old
    write order and mess up playback order.

.PARAMETER Source
    Folder containing the source sound files. Defaults to ..\sounds relative
    to this script.

.PARAMETER Sketch
    Arduino sketch to check the sound count against. Defaults to
    ..\pumpkin\pumpkin.ino relative to this script. If the number of sound
    files doesn't match the number of TRACK_DURATIONS entries (or a literal
    TRACK_COUNT) in the sketch, a warning is shown. Copying still proceeds.

.PARAMETER Destination
    Folder/drive to copy files to, e.g. the SD card mounted as E:\

.PARAMETER NoClear
    Skip clearing the destination folder before copying.

.PARAMETER WhatIf
    Show what would happen without actually copying/deleting anything.

.EXAMPLE
    .\copy-sounds-in-order.ps1 -Destination E:\

.EXAMPLE
    .\copy-sounds-in-order.ps1 -Destination E:\ -WhatIf
#>

[CmdletBinding(SupportsShouldProcess = $true)]
param(
    [string]$Source = (Join-Path $PSScriptRoot "..\sounds"),

    [string]$Sketch = (Join-Path $PSScriptRoot "..\pumpkin\pumpkin.ino"),

    [Parameter(Mandatory = $true)]
    [string]$Destination,

    [switch]$NoClear
)

$ErrorActionPreference = "Stop"

$Source = (Resolve-Path $Source).Path

if (-not (Test-Path $Destination)) {
    throw "Destination '$Destination' does not exist. Is the SD card plugged in?"
}

# Natural sort key: extract leading number so "2_foo" < "10_foo" even without
# zero-padding. Falls back to plain filename if no leading number found.
function Get-SortKey {
    param([string]$Name)
    if ($Name -match '^(\d+)') {
        return [int]$Matches[1]
    }
    return [int]::MaxValue
}

$files = Get-ChildItem -Path $Source -File |
    Where-Object { $_.Extension -match '\.(mp3|wav)$' } |
    Sort-Object @{Expression = { Get-SortKey $_.Name }}, Name

if ($files.Count -eq 0) {
    throw "No .mp3/.wav files found in '$Source'"
}

Write-Host "Files to copy (in playback order):"
$files | ForEach-Object { Write-Host "  $($_.Name)" }

# Warn if the sketch's track list doesn't match the number of sound files.
# Returns a list of warning messages (empty if everything matches).
function Get-SketchSyncWarnings {
    param([string]$SketchPath, [int]$SoundCount)

    if (-not (Test-Path $SketchPath)) {
        return @("Sketch not found at '$SketchPath', so the sound count was not checked against it.")
    }

    # Strip comments so commented-out values or notes don't confuse the match.
    $code = (Get-Content -Raw -Path $SketchPath) -replace '(?s)/\*.*?\*/', '' -replace '//[^\r\n]*', ''
    $name = Split-Path $SketchPath -Leaf
    $warnings = @()

    $m = [regex]::Match($code, 'TRACK_DURATIONS\s*\[\s*\]\s*=\s*\{([^}]*)\}')
    if ($m.Success) {
        $entries = @($m.Groups[1].Value -split ',' | ForEach-Object { $_.Trim() } | Where-Object { $_ -ne '' })
        if ($entries.Count -ne $SoundCount) {
            $warnings += "$SoundCount sound file(s) but $name lists $($entries.Count) TRACK_DURATIONS entries. Update TRACK_DURATIONS (and the state checks in loop()) to match."
        }
    } else {
        $warnings += "Could not find a TRACK_DURATIONS array in $name, so the sound count was not checked."
    }

    # TRACK_COUNT is normally derived from the array; only check it if hardcoded.
    $c = [regex]::Match($code, 'TRACK_COUNT\s*=\s*(\d+)\s*;')
    if ($c.Success -and [int]$c.Groups[1].Value -ne $SoundCount) {
        $warnings += "$SoundCount sound file(s) but $name sets TRACK_COUNT = $($c.Groups[1].Value)."
    }

    return $warnings
}

$syncWarnings = @(Get-SketchSyncWarnings -SketchPath $Sketch -SoundCount $files.Count)
if ($syncWarnings.Count -eq 0) {
    Write-Host "Sketch check: $($files.Count) sound file(s) match $(Split-Path $Sketch -Leaf)."
} else {
    $syncWarnings | ForEach-Object { Write-Warning $_ }
}

if (-not $NoClear) {
    $existing = Get-ChildItem -Path $Destination -File |
        Where-Object { $_.Extension -match '\.(mp3|wav)$' }
    foreach ($f in $existing) {
        if ($PSCmdlet.ShouldProcess($f.FullName, "Delete existing file")) {
            Remove-Item -Path $f.FullName -Force
        }
    }
}

foreach ($f in $files) {
    $dest = Join-Path $Destination $f.Name
    if ($PSCmdlet.ShouldProcess($dest, "Copy $($f.FullName)")) {
        Copy-Item -Path $f.FullName -Destination $dest -Force
        Write-Host "Copied $($f.Name)"
    }
}

Write-Host "Done. Copied $($files.Count) file(s) to '$Destination' in order."
if ($syncWarnings.Count -gt 0) {
    Write-Warning "The sketch check reported warnings (see above). Fix them and re-upload the sketch if needed."
}
