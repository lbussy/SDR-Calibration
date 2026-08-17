param(
    [Parameter(Mandatory = $true)][string]$BuildDir,
    [Parameter(Mandatory = $true)][string]$OutputDir,
    [Parameter(Mandatory = $true)][string]$Version,
    [Parameter(Mandatory = $true)][string]$SourceDir,
    [Parameter(Mandatory = $true)][string]$PackageName,
    [Parameter(Mandatory = $true)][string]$Publisher,
    [Parameter(Mandatory = $true)][string]$PublisherDisplayName,
    [Parameter(Mandatory = $true)][string]$ProductName,
    [Parameter(Mandatory = $true)][string]$QtSourceArchive,
    [Parameter(Mandatory = $true)][string]$QtSourceSha256,
    [string]$QtAdditionalSourceArchives = '',
    [string]$QtAdditionalSourceSha256 = ''
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

function Require-Command([string]$Name) {
    $command = Get-Command $Name -ErrorAction SilentlyContinue
    if ($null -eq $command) { throw "required tool is unavailable: $Name" }
    return $command.Source
}

function Require-Identity([string]$Value, [string]$Label) {
    if ([string]::IsNullOrWhiteSpace($Value) -or
        $Value -match '(?i)(placeholder|example|todo|tbd|change.?me)') {
        throw "$Label must be an exact non-placeholder Partner Center value"
    }
}

Require-Identity $PackageName 'package name'
Require-Identity $Publisher 'publisher'
Require-Identity $PublisherDisplayName 'publisher display name'
Require-Identity $ProductName 'reserved product name'
if ($PackageName -notmatch '^[A-Za-z0-9.-]{3,50}$') {
    throw 'package name is not a valid MSIX identity name'
}
if ($Publisher -notmatch '^CN=') { throw 'publisher must be the exact Partner Center X.500 value' }
if ($Version -notmatch '^[0-9]+\.[0-9]+\.[0-9]+$') {
    throw 'project version must contain exactly three numeric components'
}

$cmake = Require-Command 'cmake.exe'
$git = Require-Command 'git.exe'
$makeappx = Require-Command 'makeappx.exe'

$BuildDir = [IO.Path]::GetFullPath($BuildDir).TrimEnd([IO.Path]::DirectorySeparatorChar)
$OutputDir = [IO.Path]::GetFullPath($OutputDir).TrimEnd([IO.Path]::DirectorySeparatorChar)
$SourceDir = [IO.Path]::GetFullPath($SourceDir).TrimEnd([IO.Path]::DirectorySeparatorChar)
$outputParent = [IO.Directory]::GetParent($OutputDir)
if ($null -eq $outputParent -or $outputParent.FullName -ne $BuildDir) {
    throw 'output directory must be a direct child of the build directory'
}
if (Test-Path -LiteralPath $OutputDir) {
    throw 'refusing to reuse an existing Store package output directory'
}

$sourceRevision = (& $git -C $SourceDir rev-parse HEAD).Trim()
if ($LASTEXITCODE -ne 0) { throw 'could not resolve the source revision' }
if (@(& $git -C $SourceDir status --porcelain).Count -ne 0) {
    throw 'refusing to package a dirty source tree'
}
$upstream = (& $git -C $SourceDir rev-parse --abbrev-ref '@{upstream}' 2>$null).Trim()
if ($LASTEXITCODE -ne 0 -or [string]::IsNullOrWhiteSpace($upstream)) {
    throw 'source branch has no configured upstream'
}
$upstreamRevision = (& $git -C $SourceDir rev-parse '@{upstream}').Trim()
if ($sourceRevision -ne $upstreamRevision) {
    throw 'source revision is not synchronized with its upstream'
}

$cache = & $cmake -LA -N $BuildDir
$qtMatch = $cache | Select-String '^Qt6_DIR:PATH=(.*)[\\/]lib[\\/]cmake[\\/]Qt6$' |
    Select-Object -First 1
$qtDir = if ($null -eq $qtMatch) { '' } else { $qtMatch.Matches.Groups[1].Value }
if ([string]::IsNullOrWhiteSpace($qtDir)) { throw 'configured Qt prefix was not found' }
$windeployqt = Join-Path $qtDir 'bin\windeployqt.exe'
if (-not (Test-Path -LiteralPath $windeployqt -PathType Leaf)) {
    throw 'windeployqt for the configured Qt was not found'
}

$stage = Join-Path $OutputDir 'stage'
$evidence = Join-Path $OutputDir 'evidence'
$unpacked = Join-Path $OutputDir 'unpacked'
New-Item -ItemType Directory -Path $stage, $evidence | Out-Null
& $cmake --install $BuildDir --prefix $stage --config Release
if ($LASTEXITCODE -ne 0) { throw 'isolated package install failed' }

$gui = Join-Path $stage 'bin\sdrcal-gui.exe'
$cli = Join-Path $stage 'bin\sdrcal.exe'
foreach ($required in $gui, $cli) {
    if (-not (Test-Path -LiteralPath $required -PathType Leaf)) {
        throw "required Store executable is missing: $required"
    }
}
& $windeployqt --release --no-translations --no-system-d3d-compiler `
    --no-system-dxc-compiler --no-compiler-runtime --no-opengl-sw $gui
if ($LASTEXITCODE -ne 0) { throw 'windeployqt failed' }

$binaries = @(Get-ChildItem -LiteralPath $stage -Recurse -File |
    Where-Object { $_.Extension -in '.exe', '.dll' })
$unexpected = @($binaries | Where-Object {
    $relative = $_.FullName.Substring($stage.Length + 1)
    $relative -notin 'bin\sdrcal.exe', 'bin\sdrcal-gui.exe' -and
    $_.BaseName -notmatch '^(Qt6|q)'
})
if ($unexpected.Count -ne 0) {
    throw "unexpected executable or DLL in Store payload: $($unexpected.Name -join ', ')"
}
if (-not (Test-Path -LiteralPath (Join-Path $stage 'bin\platforms\qwindows.dll'))) {
    throw 'deployed Qt platform plug-in is missing'
}

$qtVersion = (& (Join-Path $qtDir 'bin\qmake.exe') -query QT_VERSION).Trim()
$runtimeInventory = Join-Path $evidence 'runtime-closure.txt'
$binaries | ForEach-Object {
    "FILE $($_.FullName.Substring($stage.Length + 1))"
    (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
} | Set-Content -LiteralPath $runtimeInventory -Encoding utf8
& $cmake "-DSDRCAL_STAGE_DIR=$stage" "-DSDRCAL_OUTPUT_DIR=$evidence" `
    '-DSDRCAL_PLATFORM=Windows' "-DSDRCAL_QT_VERSION=$qtVersion" `
    "-DSDRCAL_QT_SOURCE_ARCHIVE=$QtSourceArchive" `
    "-DSDRCAL_QT_SOURCE_SHA256=$QtSourceSha256" `
    "-DSDRCAL_QT_ADDITIONAL_SOURCE_ARCHIVES=$QtAdditionalSourceArchives" `
    "-DSDRCAL_QT_ADDITIONAL_SOURCE_SHA256=$QtAdditionalSourceSha256" `
    "-DSDRCAL_RUNTIME_INVENTORY=$runtimeInventory" `
    "-DSDRCAL_REPLACEMENT_INSTRUCTIONS=$SourceDir\packaging\licenses\qt-library-replacement.md" `
    -P "$SourceDir\packaging\licenses\assemble-qt-disposition.cmake"
if ($LASTEXITCODE -ne 0) { throw 'Qt license disposition failed' }

$assets = Join-Path $stage 'Assets'
New-Item -ItemType Directory -Path $assets | Out-Null
Add-Type -AssemblyName System.Drawing
function Resize-Png([string]$Source, [string]$Destination, [int]$Size) {
    $input = [Drawing.Image]::FromFile($Source)
    try {
        $output = New-Object Drawing.Bitmap $Size, $Size
        try {
            $graphics = [Drawing.Graphics]::FromImage($output)
            try {
                $graphics.CompositingMode = [Drawing.Drawing2D.CompositingMode]::SourceCopy
                $graphics.CompositingQuality = [Drawing.Drawing2D.CompositingQuality]::HighQuality
                $graphics.InterpolationMode = [Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
                $graphics.PixelOffsetMode = [Drawing.Drawing2D.PixelOffsetMode]::HighQuality
                $graphics.SmoothingMode = [Drawing.Drawing2D.SmoothingMode]::HighQuality
                $graphics.DrawImage($input, 0, 0, $Size, $Size)
            } finally { $graphics.Dispose() }
            $output.Save($Destination, [Drawing.Imaging.ImageFormat]::Png)
        } finally { $output.Dispose() }
    } finally { $input.Dispose() }
}
Resize-Png "$SourceDir\assets\icons\source\sdr-calibration-master-1024.png" `
    (Join-Path $assets 'Square150x150Logo.png') 150
Resize-Png "$SourceDir\assets\icons\source\sdr-calibration-small-master-1024.png" `
    (Join-Path $assets 'Square44x44Logo.png') 44

function Escape-Xml([string]$Value) { return [Security.SecurityElement]::Escape($Value) }
$manifest = Join-Path $stage 'AppxManifest.xml'
$escapedPackageName = Escape-Xml $PackageName
$escapedPublisher = Escape-Xml $Publisher
$escapedPublisherDisplayName = Escape-Xml $PublisherDisplayName
$escapedProductName = Escape-Xml $ProductName
@"
<?xml version="1.0" encoding="utf-8"?>
<Package xmlns="http://schemas.microsoft.com/appx/manifest/foundation/windows10"
         xmlns:uap="http://schemas.microsoft.com/appx/manifest/uap/windows10"
         xmlns:uap5="http://schemas.microsoft.com/appx/manifest/uap/windows10/5"
         xmlns:desktop="http://schemas.microsoft.com/appx/manifest/desktop/windows10"
         xmlns:rescap="http://schemas.microsoft.com/appx/manifest/foundation/windows10/restrictedcapabilities"
         IgnorableNamespaces="uap uap5 desktop rescap">
  <Identity Name="$escapedPackageName" Publisher="$escapedPublisher"
            Version="${Version}.0" ProcessorArchitecture="x64" />
  <Properties>
    <DisplayName>$escapedProductName</DisplayName>
    <PublisherDisplayName>$escapedPublisherDisplayName</PublisherDisplayName>
    <Logo>Assets\Square150x150Logo.png</Logo>
  </Properties>
  <Dependencies>
    <TargetDeviceFamily Name="Windows.Desktop" MinVersion="10.0.22000.0"
                        MaxVersionTested="10.0.26100.0" />
    <PackageDependency Name="Microsoft.VCLibs.140.00.UWPDesktop"
                       Publisher="CN=Microsoft Corporation, O=Microsoft Corporation, L=Redmond, S=Washington, C=US"
                       MinVersion="14.0.30704.0" />
  </Dependencies>
  <Applications>
    <Application Id="SDRCalibration" Executable="bin\sdrcal-gui.exe"
                 EntryPoint="Windows.FullTrustApplication">
      <uap:VisualElements DisplayName="$escapedProductName"
          Description="Create traceable SDR frequency-calibration profiles"
          BackgroundColor="transparent"
          Square150x150Logo="Assets\Square150x150Logo.png"
          Square44x44Logo="Assets\Square44x44Logo.png" />
      <Extensions>
        <uap5:Extension Category="windows.appExecutionAlias"
                        Executable="bin\sdrcal.exe"
                        EntryPoint="Windows.FullTrustApplication">
          <uap5:AppExecutionAlias>
            <desktop:ExecutionAlias Alias="sdrcal.exe" />
          </uap5:AppExecutionAlias>
        </uap5:Extension>
      </Extensions>
    </Application>
  </Applications>
  <Capabilities>
    <rescap:Capability Name="runFullTrust" />
  </Capabilities>
</Package>
"@ | Set-Content -LiteralPath $manifest -Encoding utf8

$msix = Join-Path $OutputDir "SDRCalibration-$Version-Windows-Store-x64.msix"
& $makeappx pack /o /d $stage /p $msix
if ($LASTEXITCODE -ne 0 -or -not (Test-Path -LiteralPath $msix)) {
    throw 'MSIX creation or manifest/schema validation failed'
}
New-Item -ItemType Directory -Path $unpacked | Out-Null
& $makeappx unpack /o /p $msix /d $unpacked
if ($LASTEXITCODE -ne 0) { throw 'MSIX unpack inspection failed' }
foreach ($required in 'AppxManifest.xml', 'sdrcal.exe', 'sdrcal-gui.exe', 'LICENSE',
    'THIRD_PARTY_NOTICES.md', 'sdrcal.spdx.json', 'icon-manifest.json', 'qwindows.dll') {
    if ($null -eq (Get-ChildItem -LiteralPath $unpacked -Recurse -File -Filter $required |
        Select-Object -First 1)) {
        throw "required MSIX payload is missing: $required"
    }
}

$payload = Get-ChildItem -LiteralPath $stage -Recurse -File | ForEach-Object {
    $relative = $_.FullName.Substring($stage.Length + 1)
    "$( (Get-FileHash $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant() )  $relative"
} | Sort-Object
$payload | Set-Content -LiteralPath (Join-Path $evidence 'payload-sha256.txt') -Encoding utf8
$hash = (Get-FileHash -LiteralPath $msix -Algorithm SHA256).Hash.ToLowerInvariant()
$os = Get-CimInstance Win32_OperatingSystem
[ordered]@{
    schema_version = 1; source_revision = $sourceRevision; project_version = $Version
    package_identity_name = $PackageName; package_identity_publisher = $Publisher
    publisher_display_name = $PublisherDisplayName; reserved_product_name = $ProductName
    package_version = "${Version}.0"; architecture = 'x64'; artifact = [IO.Path]::GetFileName($msix)
    sha256 = $hash; platform = $os.Caption; platform_version = $os.Version
    platform_build = $os.BuildNumber; qt_version = $qtVersion
    cmake_version = (& $cmake --version | Select-Object -First 1)
    makeappx = $makeappx; capabilities = @('runFullTrust')
    signing_state = 'unsigned pre-submission MSIX; Microsoft Store signing not observed'
    certification_state = 'not submitted'; store_lifecycle_qualified = $false
    distribution_license_gate = 'passed; see license-manifest.json'
    cli_disposition = 'sdrcal.exe declared as app execution alias'; device_qualified = $false
} | ConvertTo-Json -Depth 3 | Set-Content -LiteralPath (Join-Path $evidence 'manifest.json') -Encoding utf8
