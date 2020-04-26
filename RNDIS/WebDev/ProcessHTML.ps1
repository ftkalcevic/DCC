param (
    [parameter(mandatory=$true,ValueFromPipeline=$true,ValueFromPipelineByPropertyName=$true)][string]$FullName,
    [parameter(mandatory=$false,ValueFromPipeline=$false,ValueFromPipelineByPropertyName=$true)][string]$GZipPath
)


$_BufferSize = 1024 * 8
if (Test-Path -Path $FullName -PathType Leaf) {
    Write-Verbose "Reading from: $FullName"
    if ($GZipPath.Length -eq 0) {
        $tmpPath = ls -Path $FullName
        $GZipPath = Join-Path -Path ($tmpPath.DirectoryName) -ChildPath ($tmpPath.Name + '.gz')
    }
    Write-Verbose "Compressing to: $GZipPath"
} else {
    Write-Error -Message "$FullName is not a valid path/file"
    return
}

Write-Verbose "Creating Compress File @ $GZipPath"
Write-Verbose "Opening streams and file to save compressed version to..."
$input = New-Object System.IO.FileStream $FullName, ([IO.FileMode]::Open), ([IO.FileAccess]::Read), ([IO.FileShare]::Read);
$output = New-Object System.IO.FileStream $GZipPath, ([IO.FileMode]::Create), ([IO.FileAccess]::Write), ([IO.FileShare]::None)
$gzipStream = New-Object System.IO.Compression.GzipStream $output, ([IO.Compression.CompressionMode]::Compress)
try {
    $buffer = New-Object byte[]($_BufferSize);
    while ($true) {
        $read = $input.Read($buffer, 0, ($_BufferSize))
        if ($read -le 0) {
            break;
        }
        $gzipStream.Write($buffer, 0, $read)
    }
}
finally {
    Write-Verbose "Closing streams and newly compressed file"
    $gzipStream.Close();
    $output.Close();
    $input.Close();
}

