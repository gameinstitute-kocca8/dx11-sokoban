# capture_steps.ps1 - 빌드된 DX11 단계 실행 파일을 WARP로 한 프레임씩 그려 PNG로 저장한다.
#
#   pwsh tools/capture_steps.ps1 -BuildDir build -OutDir screens
#
# 각 실행 파일을 "--capture <png> --time <초>"로 실행한다. 하나라도 실패하면 종료 코드 1.
#
# 실행 파일 옆에 captures.txt가 있으면 한 줄에 하나씩 여러 장을 찍는다 (10단계~).
#   형식: <이름> [추가 인자...]     # 뒤는 주석
#   예:   push  --moves d
#   결과: <실행 파일 이름>_<이름>.png
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

function Invoke-Capture($exe, $png, $extraArgs) {
    $argList = @("--capture", "`"$png`"", "--time", $Time) + $extraArgs
    $proc = Start-Process -FilePath $exe.FullName -ArgumentList $argList `
        -WorkingDirectory $exe.DirectoryName -NoNewWindow -Wait -PassThru
    if ($proc.ExitCode -ne 0 -or -not (Test-Path $png)) {
        Write-Host "   실패 (종료 코드 $($proc.ExitCode)): $png"
        return $false
    }
    Write-Host "   저장: $png"
    return $true
}

$failed = 0
foreach ($exe in $exes) {
    Write-Host "== $($exe.BaseName)"
    $list = Join-Path $exe.DirectoryName "captures.txt"
    if (Test-Path $list) {
        foreach ($line in Get-Content $list) {
            $line = ($line -split '#', 2)[0].Trim()
            if ($line -eq "") { continue }
            $parts = $line -split '\s+'
            $png = Join-Path $OutDir ("{0}_{1}.png" -f $exe.BaseName, $parts[0])
            $extra = @()
            if ($parts.Count -gt 1) { $extra = @($parts[1..($parts.Count - 1)]) }
            if (-not (Invoke-Capture $exe $png $extra)) { $failed++ }
        }
    } else {
        $png = Join-Path $OutDir ($exe.BaseName + ".png")
        if (-not (Invoke-Capture $exe $png @())) { $failed++ }
    }
}

if ($failed -gt 0) {
    Write-Host "$failed 개 캡처 실패"
    exit 1
}
