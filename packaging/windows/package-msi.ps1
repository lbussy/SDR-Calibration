param(
    [Parameter(Mandatory = $true)][string]$BuildDir,
    [Parameter(Mandatory = $true)][string]$OutputDir,
    [Parameter(Mandatory = $true)][string]$CertificateThumbprint,
    [ValidateSet('SELF_SIGNED', 'PUBLIC_TRUST')][string]$SigningMode = 'SELF_SIGNED',
    [string]$TimestampUrl = '',
    [Parameter(Mandatory = $true)][string]$Version,
    [Parameter(Mandatory = $true)][string]$SourceDir,
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

$cmake = Require-Command 'cmake.exe'
$git = Require-Command 'git.exe'
$msiexec = Require-Command 'msiexec.exe'
$signtool = Require-Command 'signtool.exe'
$wix = Require-Command 'wix.exe'

$BuildDir = [IO.Path]::GetFullPath($BuildDir).TrimEnd([IO.Path]::DirectorySeparatorChar)
$OutputDir = [IO.Path]::GetFullPath($OutputDir).TrimEnd([IO.Path]::DirectorySeparatorChar)
$SourceDir = [IO.Path]::GetFullPath($SourceDir).TrimEnd([IO.Path]::DirectorySeparatorChar)
$outputParent = [IO.Directory]::GetParent($OutputDir)
if ($null -eq $outputParent -or $outputParent.FullName -ne $BuildDir) {
    throw 'output directory must be a direct child of the build directory'
}
if ($CertificateThumbprint -notmatch '^[0-9A-Fa-f]{40}$') {
    throw 'certificate thumbprint must contain exactly 40 hexadecimal characters'
}
if ($SigningMode -eq 'PUBLIC_TRUST' -and $TimestampUrl -notmatch '^https://') {
    throw 'PUBLIC_TRUST signing requires an HTTPS timestamp URL'
}
if ($SigningMode -eq 'SELF_SIGNED' -and -not [string]::IsNullOrWhiteSpace($TimestampUrl)) {
    throw 'SELF_SIGNED signing must not specify a public timestamp URL'
}
$certificateStore = 'CurrentUser'
$certificate = Get-Item "Cert:\CurrentUser\My\$CertificateThumbprint" -ErrorAction SilentlyContinue
if ($null -eq $certificate -and $SigningMode -eq 'PUBLIC_TRUST') {
    $certificate = Get-Item "Cert:\LocalMachine\My\$CertificateThumbprint" `
        -ErrorAction SilentlyContinue
    $certificateStore = 'LocalMachine'
}
if ($null -eq $certificate) {
    throw 'signing certificate was not found in an allowed personal certificate store'
}
if (-not $certificate.HasPrivateKey) { throw 'signing certificate has no private key' }
if ($certificate.NotBefore -gt [DateTime]::Now) { throw 'signing certificate is not valid yet' }
if ($certificate.NotAfter -le [DateTime]::Now) { throw 'signing certificate is expired' }
$codeSigningOid = '1.3.6.1.5.5.7.3.3'
if ($certificate.EnhancedKeyUsageList.ObjectId -notcontains $codeSigningOid) {
    throw 'signing certificate does not permit code signing'
}
if ($SigningMode -eq 'SELF_SIGNED' -and
    -not (Test-Path "Cert:\CurrentUser\TrustedPeople\$CertificateThumbprint")) {
    throw 'self-signed certificate is not trusted in Cert:\CurrentUser\TrustedPeople'
}
if ($SigningMode -eq 'SELF_SIGNED' -and
    $certificate.Subject -ne 'CN=SDR Calibration Development') {
    throw 'self-signed certificate has an unexpected subject'
}

function Sign-File([string]$Path) {
    $arguments = @('sign', '/sha1', $CertificateThumbprint, '/fd', 'SHA256')
    if ($certificateStore -eq 'LocalMachine') { $arguments += '/sm' }
    if ($SigningMode -eq 'PUBLIC_TRUST') {
        $arguments += @('/tr', $TimestampUrl, '/td', 'SHA256')
    }
    & $signtool @arguments $Path
    if ($LASTEXITCODE -ne 0) { throw "signing failed: $Path" }
}

function Verify-File([string]$Path, [switch]$Verbose) {
    $arguments = @('verify', '/pa', '/all')
    if ($SigningMode -eq 'PUBLIC_TRUST') { $arguments += '/tw' }
    if ($Verbose) { $arguments += '/v' }
    & $signtool @arguments $Path
}

$sourceRevision = (& $git -C $SourceDir rev-parse HEAD).Trim()
if ($LASTEXITCODE -ne 0) { throw 'could not resolve the source revision' }
$sourceStatus = @(& $git -C $SourceDir status --porcelain)
if ($sourceStatus.Count -ne 0) {
    throw 'refusing to package a dirty source tree'
}

$cache = & $cmake -LA -N $BuildDir
$qtMatch = $cache | Select-String '^Qt6_DIR:PATH=(.*)[\\/]lib[\\/]cmake[\\/]Qt6$' | Select-Object -First 1
$qtDir = if ($null -eq $qtMatch) { '' } else { $qtMatch.Matches.Groups[1].Value }
if ([string]::IsNullOrWhiteSpace($qtDir)) { throw 'configured Qt prefix was not found' }
$windeployqt = Join-Path $qtDir 'bin\windeployqt.exe'
if (-not (Test-Path -LiteralPath $windeployqt -PathType Leaf)) {
    throw 'windeployqt for the configured Qt was not found'
}

if (Test-Path -LiteralPath $OutputDir) { Remove-Item -LiteralPath $OutputDir -Recurse -Force }
$stage = Join-Path $OutputDir 'stage'
$evidence = Join-Path $OutputDir 'evidence'
$extract = Join-Path $OutputDir 'extracted'
New-Item -ItemType Directory -Path $stage, $evidence | Out-Null

& $cmake --install $BuildDir --prefix $stage --config Release
if ($LASTEXITCODE -ne 0) { throw 'isolated package install failed' }
$sourceIconManifest = Join-Path $SourceDir 'assets\icons\icon-manifest.json'
$stagedIconManifest = Join-Path $stage 'share\sdrcal\icons\icon-manifest.json'
if (-not (Test-Path -LiteralPath $stagedIconManifest -PathType Leaf) -or
    (Get-FileHash -LiteralPath $sourceIconManifest -Algorithm SHA256).Hash -ne
    (Get-FileHash -LiteralPath $stagedIconManifest -Algorithm SHA256).Hash) {
    throw 'staged icon provenance manifest differs from the source'
}
$gui = Join-Path $stage 'bin\sdrcal-gui.exe'
if (-not (Test-Path -LiteralPath $gui -PathType Leaf)) { throw 'installed GUI is missing' }
& $windeployqt --release --no-translations --no-system-d3d-compiler `
    --no-system-dxc-compiler --no-compiler-runtime --no-opengl-sw $gui
if ($LASTEXITCODE -ne 0) { throw 'windeployqt failed' }

$binaries = @(Get-ChildItem -LiteralPath $stage -Recurse -File |
    Where-Object { $_.Extension -in '.exe', '.dll' })
if ($binaries.Count -eq 0) { throw 'deployed runtime contains no signable binaries' }
$unclassifiedDlls = @($binaries | Where-Object {
    $_.Extension -eq '.dll' -and $_.BaseName -notmatch '^(Qt6|q)'
})
if ($unclassifiedDlls.Count -ne 0) {
    throw "deployed DLL lacks an exact Qt disposition: $($unclassifiedDlls.Name -join ', ')"
}
$payloadSignatureEvidence = Join-Path $evidence 'payload-signatures.txt'
foreach ($binary in $binaries) {
    $relativeBinary = $binary.FullName.Substring($stage.Length + 1)
    $initialVerification = Verify-File $binary.FullName 2>&1
    if ($LASTEXITCODE -ne 0) {
        Sign-File $binary.FullName
    }
    "FILE $relativeBinary" | Add-Content -LiteralPath $payloadSignatureEvidence -Encoding utf8
    Verify-File $binary.FullName -Verbose *>> $payloadSignatureEvidence
    if ($LASTEXITCODE -ne 0) { throw "signature verification failed: $($binary.FullName)" }
}
$runtimeInventory = Join-Path $evidence 'runtime-closure.txt'
$binaries | ForEach-Object {
    "FILE $($_.FullName.Substring($stage.Length + 1))"
    (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
} | Set-Content -LiteralPath $runtimeInventory -Encoding utf8
$qtVersion = (& (Join-Path $qtDir 'bin\qmake.exe') -query QT_VERSION).Trim()
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

$wxs = Join-Path $OutputDir 'product.wxs'
$wixStage = [Security.SecurityElement]::Escape($stage)
$wixIcon = [Security.SecurityElement]::Escape(
    (Join-Path $SourceDir 'assets\icons\windows\SDRCalibration.ico'))
$upgradeCode = '9B68E922-9277-4C40-BB3C-527C2AE236AC'
@"
<Wix xmlns="http://wixtoolset.org/schemas/v4/wxs">
  <Package Name="SDR Calibration" Manufacturer="SDR Calibration contributors"
           Version="$Version" UpgradeCode="$upgradeCode" Scope="perMachine">
    <MajorUpgrade DowngradeErrorMessage="A newer SDR Calibration version is already installed." />
    <MediaTemplate EmbedCab="yes" />
    <StandardDirectory Id="ProgramFiles64Folder">
      <Directory Id="INSTALLFOLDER" Name="SDR Calibration" />
    </StandardDirectory>
    <StandardDirectory Id="ProgramMenuFolder">
      <Directory Id="SDRCalibrationProgramMenuFolder" Name="SDR Calibration">
        <Component Id="SDRCalibrationStartMenuShortcut"
                   Guid="DFA5C9F8-7A1B-4D0B-89D8-A806BD8FD2E4">
          <Shortcut Id="SDRCalibrationStartMenuShortcutLink"
                    Name="SDR Calibration"
                    Description="Create traceable SDR frequency-calibration profiles"
                    Target="[INSTALLFOLDER]bin\sdrcal-gui.exe"
                    WorkingDirectory="INSTALLFOLDER"
                    Icon="SDRCalibration.ico" />
          <RemoveFolder Id="RemoveSDRCalibrationProgramMenuFolder" On="uninstall" />
          <RegistryValue Root="HKLM" Key="Software\SDR Calibration"
                         Name="StartMenuShortcut" Type="integer" Value="1"
                         KeyPath="yes" />
        </Component>
      </Directory>
    </StandardDirectory>
    <Icon Id="SDRCalibration.ico" SourceFile="$wixIcon" />
    <Property Id="ARPPRODUCTICON" Value="SDRCalibration.ico" />
    <Feature Id="Main" Title="SDR Calibration" Level="1">
      <Files Directory="INSTALLFOLDER" Include="$wixStage\**" />
      <ComponentRef Id="SDRCalibrationStartMenuShortcut" />
    </Feature>
  </Package>
</Wix>
"@ | Set-Content -LiteralPath $wxs -Encoding utf8

$msi = Join-Path $OutputDir "SDRCalibration-$Version-Windows-x64.msi"
& $wix build -arch x64 -o $msi $wxs
if ($LASTEXITCODE -ne 0 -or -not (Test-Path -LiteralPath $msi)) { throw 'MSI creation failed' }
Sign-File $msi
Verify-File $msi -Verbose *> (Join-Path $evidence 'msi-signature.txt')
if ($LASTEXITCODE -ne 0) { throw 'MSI signature verification failed' }

New-Item -ItemType Directory -Path $extract | Out-Null
$process = Start-Process -FilePath $msiexec -ArgumentList @('/a', "`"$msi`"", '/qn', "TARGETDIR=`"$extract`"") -Wait -PassThru
if ($process.ExitCode -ne 0) { throw "MSI administrative extraction failed: $($process.ExitCode)" }
foreach ($required in 'sdrcal.exe', 'sdrcal-gui.exe', 'LICENSE', 'THIRD_PARTY_NOTICES.md', 'sdrcal.spdx.json', 'README.md', 'icon-manifest.json') {
    if ($null -eq (Get-ChildItem -LiteralPath $extract -Recurse -File -Filter $required | Select-Object -First 1)) {
        throw "required MSI payload is missing: $required"
    }
}
$extractedCli = Get-ChildItem -LiteralPath $extract -Recurse -File -Filter 'sdrcal.exe' | Select-Object -First 1
& $extractedCli.FullName --help | Out-Null
if ($LASTEXITCODE -ne 0) { throw 'extracted sdrcal CLI startup check failed' }
foreach ($binary in Get-ChildItem -LiteralPath $extract -Recurse -File | Where-Object { $_.Extension -in '.exe', '.dll' }) {
    Verify-File $binary.FullName | Out-Null
    if ($LASTEXITCODE -ne 0) { throw "extracted payload signature is invalid: $($binary.FullName)" }
}

$hash = (Get-FileHash -LiteralPath $msi -Algorithm SHA256).Hash.ToLowerInvariant()
$os = Get-CimInstance Win32_OperatingSystem
$manifest = [ordered]@{
    schema_version = 1; source_revision = $sourceRevision; project_version = $Version
    platform = $os.Caption; platform_version = $os.Version; platform_build = $os.BuildNumber
    architecture = $env:PROCESSOR_ARCHITECTURE; artifact = [IO.Path]::GetFileName($msi)
    sha256 = $hash; qt_version = $qtVersion
    cmake_version = (& $cmake --version | Select-Object -First 1); payload_binary_count = $binaries.Count
    signing_mode = $SigningMode
    signing_certificate_thumbprint = $CertificateThumbprint.ToUpperInvariant()
    signing = if ($SigningMode -eq 'PUBLIC_TRUST') {
        'Authenticode SHA-256 with RFC 3161 timestamp; publicly trusted verification passed'
    } else {
        'Authenticode SHA-256; locally trusted self-signed verification passed; no public trust or timestamp'
    }
    clean_install_qualified = $false
    distribution_license_gate = 'passed; see license-manifest.json'; device_qualified = $false
}
$manifest | ConvertTo-Json -Depth 3 | Set-Content -LiteralPath (Join-Path $evidence 'manifest.json') -Encoding utf8
Get-ChildItem -LiteralPath $stage -Recurse -File | ForEach-Object {
    $relative = $_.FullName.Substring($stage.Length + 1)
    "$( (Get-FileHash $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant() )  $relative"
} | Sort-Object | Set-Content -LiteralPath (Join-Path $evidence 'payload-sha256.txt') -Encoding utf8
