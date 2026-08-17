param(
    [Parameter(Mandatory = $true)][string]$SourceDir,
    [Parameter(Mandatory = $true)][string]$PackagePath,
    [Parameter(Mandatory = $true)][string]$ArtifactSourceRevision,
    [Parameter(Mandatory = $true)][string]$ExpectedSha256,
    [Parameter(Mandatory = $true)][string]$EvidenceDir
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

function Resolve-SdkTool([string]$Name) {
    $command = Get-Command $Name -ErrorAction SilentlyContinue
    if ($null -ne $command) { return $command.Source }
    $roots = Get-ItemProperty `
        -LiteralPath 'HKLM:\SOFTWARE\Microsoft\Windows Kits\Installed Roots' `
        -ErrorAction SilentlyContinue
    if ($null -ne $roots -and -not [string]::IsNullOrWhiteSpace($roots.KitsRoot10)) {
        $versions = @(Get-ChildItem -LiteralPath (Join-Path $roots.KitsRoot10 'bin') `
            -Directory -ErrorAction SilentlyContinue |
            Where-Object { $null -ne ($_.Name -as [version]) } |
            Sort-Object { [version]$_.Name } -Descending)
        foreach ($version in $versions) {
            $candidate = Join-Path $version.FullName "x64\$Name"
            if (Test-Path -LiteralPath $candidate -PathType Leaf) { return $candidate }
        }
    }
    throw "required Windows SDK tool is unavailable: $Name"
}

$SourceDir = [IO.Path]::GetFullPath($SourceDir)
$PackagePath = [IO.Path]::GetFullPath($PackagePath)
$EvidenceDir = [IO.Path]::GetFullPath($EvidenceDir)
if (-not (Test-Path -LiteralPath $PackagePath -PathType Leaf)) {
    throw 'MSIX package does not exist'
}
if (Test-Path -LiteralPath $EvidenceDir) {
    throw 'refusing to reuse an existing lifecycle evidence directory'
}
if ($ExpectedSha256 -notmatch '^[0-9A-Fa-f]{64}$') {
    throw 'expected MSIX SHA-256 must contain exactly 64 hexadecimal characters'
}
if ($ArtifactSourceRevision -notmatch '^[0-9A-Fa-f]{40}$') {
    throw 'artifact source revision must contain exactly 40 hexadecimal characters'
}

$git = (Get-Command git.exe -ErrorAction Stop).Source
$sourceRevision = (& $git -C $SourceDir rev-parse HEAD).Trim()
if ($LASTEXITCODE -ne 0) { throw 'could not resolve source revision' }
if (@(& $git -C $SourceDir status --porcelain).Count -ne 0) {
    throw 'refusing lifecycle qualification from a dirty source tree'
}
$upstreamRevision = (& $git -C $SourceDir rev-parse '@{upstream}').Trim()
if ($LASTEXITCODE -ne 0 -or $sourceRevision -ne $upstreamRevision) {
    throw 'source revision is not synchronized with its upstream'
}
$actualHash = (Get-FileHash -LiteralPath $PackagePath -Algorithm SHA256).Hash
if ($actualHash -ne $ExpectedSha256) { throw 'MSIX SHA-256 does not match the expected artifact' }
$packageEvidence = Join-Path ([IO.Directory]::GetParent($PackagePath).FullName) `
    'evidence\manifest.json'
if (-not (Test-Path -LiteralPath $packageEvidence -PathType Leaf)) {
    throw 'package construction evidence manifest is missing'
}
$construction = Get-Content -LiteralPath $packageEvidence | ConvertFrom-Json
if ($construction.source_revision -ne $ArtifactSourceRevision -or
    $construction.sha256 -ne $ExpectedSha256.ToLowerInvariant()) {
    throw 'package construction evidence does not match the expected revision and hash'
}
& $git -C $SourceDir cat-file -e "$ArtifactSourceRevision^{commit}"
if ($LASTEXITCODE -ne 0) { throw 'artifact source revision is unavailable in the source repository' }

$makeappx = Resolve-SdkTool 'makeappx.exe'
$signtool = Resolve-SdkTool 'signtool.exe'
New-Item -ItemType Directory -Path $EvidenceDir | Out-Null
$inspection = Join-Path $EvidenceDir 'unpacked'
New-Item -ItemType Directory -Path $inspection | Out-Null
& $makeappx unpack /o /p $PackagePath /d $inspection |
    Set-Content -LiteralPath (Join-Path $EvidenceDir 'unpack.txt') -Encoding utf8
if ($LASTEXITCODE -ne 0) { throw 'MSIX unpack inspection failed' }
$manifestPath = Join-Path $inspection 'AppxManifest.xml'
[xml]$manifest = Get-Content -LiteralPath $manifestPath
$identity = $manifest.Package.Identity
$publisher = [string]$identity.Publisher
$packageName = [string]$identity.Name
$packageVersion = [string]$identity.Version
if ([string]::IsNullOrWhiteSpace($publisher) -or $publisher -notmatch '^CN=') {
    throw 'manifest publisher is missing or invalid'
}
if ([string]::IsNullOrWhiteSpace($packageName)) { throw 'manifest package name is missing' }

$signedPackage = Join-Path $EvidenceDir ([IO.Path]::GetFileNameWithoutExtension($PackagePath) +
    '-development-signed.msix')
Copy-Item -LiteralPath $PackagePath -Destination $signedPackage
$certificate = $null
$installed = $null
$thumbprint = ''
$checkpoints = New-Object System.Collections.Generic.List[string]
$checkpoints.Add("artifact_source_revision=$ArtifactSourceRevision")
$checkpoints.Add("harness_source_revision=$sourceRevision")
$checkpoints.Add("unsigned_sha256=$($actualHash.ToLowerInvariant())")
$checkpoints.Add("identity=$packageName version=$packageVersion publisher=$publisher")
try {
    $certificate = New-SelfSignedCertificate -Type Custom -Subject $publisher `
        -FriendlyName 'SDR Calibration temporary MSIX qualification' `
        -CertStoreLocation 'Cert:\CurrentUser\My' -KeyUsage DigitalSignature `
        -KeyExportPolicy NonExportable -NotAfter ([DateTime]::Now.AddDays(2)) `
        -TextExtension @('2.5.29.37={text}1.3.6.1.5.5.7.3.3')
    $thumbprint = $certificate.Thumbprint
    $publicCertificate = Join-Path $EvidenceDir 'temporary-development-certificate.cer'
    Export-Certificate -Cert $certificate -FilePath $publicCertificate | Out-Null
    Import-Certificate -FilePath $publicCertificate `
        -CertStoreLocation 'Cert:\CurrentUser\TrustedPeople' | Out-Null
    $checkpoints.Add("temporary_certificate=$thumbprint created_and_trusted_for_current_user")

    & $signtool sign /sha1 $thumbprint /fd SHA256 $signedPackage |
        Set-Content -LiteralPath (Join-Path $EvidenceDir 'sign.txt') -Encoding utf8
    if ($LASTEXITCODE -ne 0) { throw 'development signing failed' }
    $savedErrorActionPreference = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'
    try {
        $verification = @(& $signtool verify /pa /v $signedPackage 2>&1)
        $verificationExit = $LASTEXITCODE
    } finally {
        $ErrorActionPreference = $savedErrorActionPreference
    }
    $verification | Set-Content `
        -LiteralPath (Join-Path $EvidenceDir 'signature-verification.txt') -Encoding utf8
    $signature = Get-AuthenticodeSignature -LiteralPath $signedPackage
    if ($null -eq $signature.SignerCertificate -or
        $signature.SignerCertificate.Thumbprint -ne $thumbprint) {
        throw 'signed package does not identify the temporary signing certificate'
    }
    if ($verificationExit -ne 0 -and
        ($verification -join "`n") -notmatch 'root certificate which is not trusted') {
        throw 'development signature inspection failed for an unexpected reason'
    }
    $checkpoints.Add('development_signature=present; install is the local-trust validation')

    Add-AppxPackage -Path $signedPackage
    $installed = Get-AppxPackage -Name $packageName -ErrorAction Stop
    $installed | Format-List Name, PackageFullName, PackageFamilyName, Version,
        Architecture, SignatureKind, InstallLocation |
        Out-String | Set-Content -LiteralPath (Join-Path $EvidenceDir 'installed-package.txt')
    $checkpoints.Add('install=passed')

    $alias = Join-Path $env:LOCALAPPDATA 'Microsoft\WindowsApps\sdrcal.exe'
    $cliOutput = & $alias --version 2>&1
    if ($LASTEXITCODE -ne 0 -or $cliOutput -notmatch [regex]::Escape($packageVersion.Substring(0,
            $packageVersion.LastIndexOf('.')))) {
        throw 'installed CLI execution alias did not report the package version'
    }
    $cliOutput | Set-Content -LiteralPath (Join-Path $EvidenceDir 'cli-version.txt')
    $checkpoints.Add('cli_alias=passed')

    foreach ($label in 'first_launch', 'relaunch') {
        Start-Process explorer.exe "shell:AppsFolder\$($installed.PackageFamilyName)!SDRCalibration"
        Start-Sleep -Seconds 3
        $gui = Get-Process -Name 'sdrcal-gui' -ErrorAction SilentlyContinue |
            Select-Object -First 1
        if ($null -eq $gui) { throw "GUI $label did not produce a running process" }
        $checkpoints.Add("gui_$label=passed pid=$($gui.Id)")
        Stop-Process -Id $gui.Id -Force
        $gui.WaitForExit()
    }

    Remove-AppxPackage -Package $installed.PackageFullName
    $installed = $null
    if ($null -ne (Get-AppxPackage -Name $packageName -ErrorAction SilentlyContinue)) {
        throw 'package remains registered after uninstall'
    }
    $checkpoints.Add('uninstall=passed')
} finally {
    Get-Process -Name 'sdrcal-gui' -ErrorAction SilentlyContinue | Stop-Process -Force
    $remaining = Get-AppxPackage -Name $packageName -ErrorAction SilentlyContinue
    if ($null -ne $remaining) { Remove-AppxPackage -Package $remaining.PackageFullName }
    if (-not [string]::IsNullOrWhiteSpace($thumbprint)) {
        Remove-Item -LiteralPath "Cert:\CurrentUser\TrustedPeople\$thumbprint" `
            -Force -ErrorAction SilentlyContinue
        Remove-Item -LiteralPath "Cert:\CurrentUser\My\$thumbprint" `
            -Force -ErrorAction SilentlyContinue
    }
}

$cleanupPassed = $null -eq (Get-AppxPackage -Name $packageName -ErrorAction SilentlyContinue) -and
    $null -eq (Get-Process -Name 'sdrcal-gui' -ErrorAction SilentlyContinue) -and
    (-not (Test-Path -LiteralPath "Cert:\CurrentUser\TrustedPeople\$thumbprint")) -and
    (-not (Test-Path -LiteralPath "Cert:\CurrentUser\My\$thumbprint"))
if (-not $cleanupPassed) { throw 'post-lifecycle cleanup audit failed' }
$checkpoints.Add('cleanup=passed package=false gui_process=false temporary_certificate=false')
$checkpoints | Set-Content -LiteralPath (Join-Path $EvidenceDir 'lifecycle-checkpoints.txt')
[ordered]@{
    schema_version = 1
    artifact_source_revision = $ArtifactSourceRevision.ToLowerInvariant()
    harness_source_revision = $sourceRevision
    unsigned_artifact = [IO.Path]::GetFileName($PackagePath)
    unsigned_sha256 = $actualHash.ToLowerInvariant()
    package_identity_name = $packageName
    package_identity_publisher = $publisher
    package_version = $packageVersion
    development_signature = 'passed; temporary locally trusted certificate only'
    install = 'passed'
    cli_alias = 'passed'
    gui_first_launch = 'passed'
    gui_relaunch = 'passed'
    uninstall = 'passed'
    cleanup = 'passed'
    microsoft_store_signing_observed = $false
    store_delivery_qualified = $false
    device_qualified = $false
} | ConvertTo-Json -Depth 3 |
    Set-Content -LiteralPath (Join-Path $EvidenceDir 'result.json') -Encoding utf8
Write-Host "MSIX development lifecycle passed for $packageName $packageVersion ($actualHash)"
