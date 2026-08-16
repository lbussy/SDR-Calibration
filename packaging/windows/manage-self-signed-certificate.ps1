param(
    [Parameter(Mandatory = $true)]
    [ValidateSet('Create', 'Status', 'Remove')]
    [string]$Action,
    [string]$Thumbprint = ''
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

$subject = 'CN=SDR Calibration Development'
$codeSigningOid = '1.3.6.1.5.5.7.3.3'

function Normalize-Thumbprint([string]$Value) {
    return ($Value -replace '[^0-9A-Fa-f]', '').ToUpperInvariant()
}

if ($Action -eq 'Create') {
    if (-not [string]::IsNullOrWhiteSpace($Thumbprint)) {
        throw 'Create does not accept -Thumbprint'
    }
    $existing = Get-ChildItem 'Cert:\CurrentUser\My' | Where-Object {
        $_.Subject -eq $subject
    }
    if ($null -ne $existing) {
        throw "an SDR Calibration development certificate already exists: $($existing.Thumbprint -join ', ')"
    }
    $certificate = New-SelfSignedCertificate -Type CodeSigningCert `
        -Subject $subject -CertStoreLocation 'Cert:\CurrentUser\My' `
        -KeyAlgorithm RSA -KeyLength 3072 -HashAlgorithm SHA256 `
        -KeyExportPolicy NonExportable -NotAfter (Get-Date).AddYears(2)
    $temporaryCer = Join-Path ([IO.Path]::GetTempPath()) `
        "sdrcal-$($certificate.Thumbprint).cer"
    try {
        Export-Certificate -Cert $certificate -FilePath $temporaryCer -Force | Out-Null
        Import-Certificate -FilePath $temporaryCer `
            -CertStoreLocation 'Cert:\CurrentUser\TrustedPeople' | Out-Null
    } catch {
        Remove-Item -LiteralPath $certificate.PSPath -Force -ErrorAction SilentlyContinue
        throw
    } finally {
        Remove-Item -LiteralPath $temporaryCer -Force -ErrorAction SilentlyContinue
    }
    Write-Output "Created a non-exportable development signing key in CurrentUser\My."
    Write-Output "Trusted its public certificate in CurrentUser\TrustedPeople."
    Write-Output "Thumbprint: $($certificate.Thumbprint)"
    Write-Output 'This certificate is locally trusted only and must not be represented as public trust.'
    exit 0
}

$normalized = Normalize-Thumbprint $Thumbprint
if ($normalized -notmatch '^[0-9A-F]{40}$') {
    throw 'Status and Remove require a 40-hexadecimal-character -Thumbprint'
}
$personal = Get-Item "Cert:\CurrentUser\My\$normalized" -ErrorAction SilentlyContinue
$trusted = Get-Item "Cert:\CurrentUser\TrustedPeople\$normalized" -ErrorAction SilentlyContinue

if ($Action -eq 'Status') {
    [ordered]@{
        thumbprint = $normalized
        personal_store = $null -ne $personal
        has_private_key = $null -ne $personal -and $personal.HasPrivateKey
        trusted_people_store = $null -ne $trusted
        code_signing = ($null -ne $personal) -and
            ($personal.EnhancedKeyUsageList.ObjectId -contains $codeSigningOid)
        subject = if ($null -eq $personal) { $null } else { $personal.Subject }
        expires_utc = if ($null -eq $personal) { $null } else { $personal.NotAfter.ToUniversalTime() }
    } | ConvertTo-Json
    exit 0
}

foreach ($certificate in @($personal, $trusted)) {
    if ($null -ne $certificate) {
        if ($certificate.Subject -ne $subject) {
            throw "refusing to remove certificate with unexpected subject: $($certificate.Subject)"
        }
        Remove-Item -LiteralPath $certificate.PSPath -Force
    }
}
if ($null -eq $personal -and $null -eq $trusted) {
    throw "certificate was not found in the managed CurrentUser stores: $normalized"
}
Write-Output "Removed SDR Calibration development certificate $normalized from CurrentUser stores."
