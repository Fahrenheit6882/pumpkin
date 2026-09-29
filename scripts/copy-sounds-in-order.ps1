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
    Folder containing the source sound files. Defaults to .\sounds\pumpkin_files

.PARAMETER Destination
    Folder/drive to copy files to, e.g. the SD card mounted as E:\

.PARAMETER NoClear
    Skip clearing the destination folder before copying.

.PARAMETER WhatIf
    Show what would happen without actually copying/deleting anything.

.EXAMPLE
    .\copy-sounds-in-order.ps1 -Destination E:\

.EXAMPLE
    .\copy-sounds-in-order.ps1 -Source .\sounds\pumpkin_min_files -Destination E:\ -WhatIf
#>

[CmdletBinding(SupportsShouldProcess = $true)]
param(
    [string]$Source = (Join-Path $PSScriptRoot "..\sounds\pumpkin_files"),

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
