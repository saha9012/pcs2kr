# Генерация PDF из SRS.md
# Требуется pandoc и LaTeX (MiKTeX) или wkhtmltopdf

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
$srs = Join-Path $root "docs\SRS.md"
$output = Join-Path $root "docs\SRS.pdf"

Write-Host "Генерация PDF: $output"

$pandoc = Get-Command pandoc -ErrorAction SilentlyContinue
if (-not $pandoc) {
    Write-Warning "pandoc не найден. Установите pandoc и MiKTeX, затем повторите."
    Write-Host "Альтернатива: откройте docs/SRS.md в Word или VS Code и экспортируйте в PDF."
    exit 1
}

$titleContent = @"
---
title: "Спецификация требований к программному обеспечению (SRS)"
subtitle: "Симулятор работы лифтов в здании"
author: "ФИО студента, группа ___"
date: "2026"
lang: ru
documentclass: article
geometry: margin=2.5cm
---

"@

$body = Get-Content $srs -Raw -Encoding UTF8
# Remove duplicate YAML from SRS if present
$body = $body -replace '(?s)^---.*?---\s*', ''
$body = $body -replace '\\newpage\s*', ''

$temp = Join-Path $env:TEMP "srs_build.md"
Set-Content -Path $temp -Value ($titleContent + $body) -Encoding UTF8

try {
    & pandoc $temp -o $output --pdf-engine=xelatex -V mainfont="Times New Roman" -V geometry:margin=2.5cm
    if ($LASTEXITCODE -eq 0) {
        Write-Host "PDF создан: $output"
    } else {
        & pandoc $temp -o $output
        Write-Host "PDF создан (без xelatex): $output"
    }
} catch {
    Write-Warning $_
    exit 1
} finally {
    Remove-Item $temp -ErrorAction SilentlyContinue
}
