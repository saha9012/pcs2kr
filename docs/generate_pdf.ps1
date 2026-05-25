# Генерация PDF из docs/SRS.md (содержимое совпадает с Markdown)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot

Write-Host "Генерация docs\SRS.pdf ..."
py -3 (Join-Path $root "docs\generate_pdf.py")
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
Write-Host "Готово."
