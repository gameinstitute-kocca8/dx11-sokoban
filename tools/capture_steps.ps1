# capture_steps.ps1 - 빌드된 DX11 단계 실행 파일을 WARP로 한 프레임씩 그려 PNG로 저장한다.
#
#   pwsh tools/capture_steps.ps1 -BuildDir build -OutDir screens
#
# 각 실행 파일을 "--capture <png> --time <초>"로 실행한다. 하나라도 실패하면 종료 코드 1.
param(
    [string]$BuildDir = "build",
    [string]$OutDir = "screens",
    [string]$Time = "1.0"
)

$ErrorActionPreference = "Stop"
New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
$OutDir = (Resolve-Path $OutDir).Path

$exes = Get-ChildItem -Path $BuildDir -Recurse -Filter "*.exe" |
    Where-Object { $_.Name -match '^\d\d_.+\.exe$' -and $_.FullName -notmatch '[\\/]_deps[\\/]' } |
    Sort-Object Name

if ($exes.Count -eq 0) {
    Write-Host "캡처할 실행 파일이 없습니다 ($BuildDir)."
    exit 0
}

$failed = 0
foreach ($exe in $exes) {
    $png = Join-Path $OutDir ($exe.BaseName + ".png")
    Write-Host "== $($exe.BaseName)"
    $proc = Start-Process -FilePath $exe.FullName -ArgumentList @("--capture", "`"$png`"", "--time", $Time) `
        -WorkingDirectory $exe.DirectoryName -NoNewWindow -Wait -PassThru
    if ($proc.ExitCode -ne 0 -or -not (Test-Path $png)) {
        Write-Host "   실패 (종료 코드 $($proc.ExitCode))"
        $failed++
    } else {
        Write-Host "   저장: $png"
    }
}

if ($failed -gt 0) {
    Write-Host "$failed 개 단계 캡처 실패"
    exit 1
}
