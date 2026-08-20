param(
    [Parameter(Mandatory = $true)][string]$SourceDir,
    [Parameter(Mandatory = $true)][string]$QtSourceArchive,
    [Parameter(Mandatory = $true)][string]$QtSourceSha256,
    [string]$QtAdditionalSourceArchives = '',
    [string]$QtAdditionalSourceSha256 = ''
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

function Invoke-Checked([string]$Command, [string[]]$Arguments, [string]$Label) {
    & $Command @Arguments
    if ($LASTEXITCODE -ne 0) { throw "$Label failed with exit code $LASTEXITCODE" }
}

function Resolve-VsWhere {
    $command = Get-Command 'vswhere.exe' -ErrorAction SilentlyContinue
    if ($null -ne $command) { return $command.Source }
    $candidate = Join-Path ${env:ProgramFiles(x86)} `
        'Microsoft Visual Studio\Installer\vswhere.exe'
    if (Test-Path -LiteralPath $candidate -PathType Leaf) { return $candidate }
    throw 'required Visual Studio discovery tool is unavailable: vswhere.exe'
}

function Enter-MsvcEnvironment {
    $vswhere = Resolve-VsWhere
    $installation = (& $vswhere -latest -products '*' -requires `
        Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath).Trim()
    if ($LASTEXITCODE -ne 0 -or [string]::IsNullOrWhiteSpace($installation)) {
        throw 'a Visual Studio installation with the x64 C++ toolchain was not found'
    }
    $devCmd = Join-Path $installation 'Common7\Tools\VsDevCmd.bat'
    if (-not (Test-Path -LiteralPath $devCmd -PathType Leaf)) {
        throw 'Visual Studio developer environment script was not found'
    }
    $environmentCommand = '"' + $devCmd +
        '" -no_logo -arch=x64 -host_arch=x64 && set'
    $environment = & $env:ComSpec /d /s /c $environmentCommand
    if ($LASTEXITCODE -ne 0) { throw 'Visual Studio developer environment setup failed' }
    foreach ($line in $environment) {
        $separator = $line.IndexOf('=')
        if ($separator -gt 0) {
            [Environment]::SetEnvironmentVariable($line.Substring(0, $separator),
                $line.Substring($separator + 1), 'Process')
        }
    }
    $compiler = Get-Command 'cl.exe' -ErrorAction SilentlyContinue
    if ($null -eq $compiler) {
        throw 'Visual Studio environment setup did not expose cl.exe'
    }
    Write-Host "MSVC environment: $($compiler.Source)"
}

function Resolve-QtEnvironment {
    $qmakeCommand = Get-Command 'qmake.exe' -ErrorAction SilentlyContinue
    $qmake = if ($null -ne $qmakeCommand) { $qmakeCommand.Source } else {
        $candidate = 'C:\Qt\bin\qmake.exe'
        if (Test-Path -LiteralPath $candidate -PathType Leaf) { $candidate } else { '' }
    }
    if ([string]::IsNullOrWhiteSpace($qmake)) {
        throw 'Qt discovery failed: qmake.exe was not on PATH or under C:\Qt\bin'
    }
    $qtVersionText = (& $qmake -query QT_VERSION).Trim()
    $qtVersion = $qtVersionText -as [version]
    if ($LASTEXITCODE -ne 0 -or $null -eq $qtVersion -or $qtVersion -lt [version]'6.2') {
        throw "Qt 6.2+ is required; discovered version was $qtVersionText"
    }
    $qtBin = [IO.Directory]::GetParent($qmake).FullName
    $qtPrefix = [IO.Directory]::GetParent($qtBin).FullName
    $qtCMake = Join-Path $qtPrefix 'lib\cmake\Qt6'
    if (-not (Test-Path -LiteralPath (Join-Path $qtCMake 'Qt6Config.cmake') -PathType Leaf)) {
        throw 'discovered Qt installation does not contain Qt6Config.cmake'
    }
    $env:PATH = "$qtBin;$env:PATH"
    Write-Host "Qt environment: $qtPrefix ($qtVersionText)"
    return $qtCMake
}

$SourceDir = [IO.Path]::GetFullPath($SourceDir).TrimEnd(
    [IO.Path]::DirectorySeparatorChar)
if (-not (Test-Path -LiteralPath (Join-Path $SourceDir 'CMakeLists.txt') -PathType Leaf)) {
    throw 'source directory does not contain CMakeLists.txt'
}
foreach ($binding in @(
    @{ Value = $QtSourceArchive; Hash = $QtSourceSha256; Label = 'Qt base source' },
    @{ Value = $QtAdditionalSourceArchives; Hash = $QtAdditionalSourceSha256
        Label = 'Qt additional source' }
)) {
    if ([string]::IsNullOrWhiteSpace($binding.Value) -or
        -not (Test-Path -LiteralPath $binding.Value -PathType Leaf)) {
        throw "$($binding.Label) archive is unavailable"
    }
    if ($binding.Hash -notmatch '^[0-9A-Fa-f]{64}$') {
        throw "$($binding.Label) SHA-256 is invalid"
    }
    $actual = (Get-FileHash -LiteralPath $binding.Value -Algorithm SHA256).Hash
    if ($actual -ne $binding.Hash) { throw "$($binding.Label) SHA-256 does not match" }
}

Enter-MsvcEnvironment
$qtCMake = Resolve-QtEnvironment
Set-Location $SourceDir
$configure = @(
    '--preset', 'windows-store-release',
    "-DQt6_DIR:PATH=$qtCMake",
    "-DSDRCAL_QT_SOURCE_ARCHIVE=$QtSourceArchive",
    "-DSDRCAL_QT_SOURCE_SHA256=$QtSourceSha256",
    "-DSDRCAL_QT_ADDITIONAL_SOURCE_ARCHIVES=$QtAdditionalSourceArchives",
    "-DSDRCAL_QT_ADDITIONAL_SOURCE_SHA256=$QtAdditionalSourceSha256"
)
Invoke-Checked 'cmake.exe' $configure 'Store configure'
Invoke-Checked 'cmake.exe' @('--build', '--preset', 'windows-store-release') `
    'Store build'
Invoke-Checked 'ctest.exe' @('--preset', 'windows-store-release', '--output-on-failure') `
    'Store hardware-free test suite'
Invoke-Checked 'cmake.exe' @('--build', 'build/windows-store-release', '--target',
    'package-audit') 'Store package audit'
Invoke-Checked 'cmake.exe' @('--build', 'build/windows-store-release', '--target',
    'windows-store-msix') 'Store MSIX construction'
