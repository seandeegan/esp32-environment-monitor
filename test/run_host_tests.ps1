$ErrorActionPreference = 'Stop'
$projectDirectory = Split-Path -Parent $PSScriptRoot
$buildDirectory = Join-Path $projectDirectory '.pio/host-tests'
New-Item -ItemType Directory -Force -Path $buildDirectory | Out-Null

if (-not (Get-Command cl.exe -ErrorAction SilentlyContinue)) {
    $vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
    if (-not (Test-Path -LiteralPath $vswhere)) {
        throw 'Install Visual Studio C++ build tools or run the portable C++ test with g++/clang++.'
    }
    $installation = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if (-not $installation) { throw 'Visual Studio C++ build tools were not found.' }
    $developerShell = Join-Path $installation 'Common7/Tools/VsDevCmd.bat'
    $environmentLines = & cmd.exe /d /c "call `"$developerShell`" -no_logo -arch=x64 >nul && set"
    if ($LASTEXITCODE -ne 0) { throw 'Could not initialize the C++ build environment.' }
    foreach ($line in $environmentLines) {
        if ($line -match '^([^=]+)=(.*)$') {
            [Environment]::SetEnvironmentVariable($matches[1], $matches[2], 'Process')
        }
    }
}

$testSource = Join-Path $PSScriptRoot 'test_sensor_state.cpp'
$testExecutable = Join-Path $buildDirectory 'sensor_state_tests.exe'
$testObject = Join-Path $buildDirectory 'sensor_state_tests.obj'
& cl.exe /nologo /EHsc /std:c++14 /W4 /WX $testSource "/Fe:$testExecutable" "/Fo:$testObject"
if ($LASTEXITCODE -ne 0) { throw 'Host test compilation failed.' }
& $testExecutable
if ($LASTEXITCODE -ne 0) { throw 'Host tests failed.' }
