# Bootstrap script for Ion Vortex - Windows
param(
    [switch]$Help
)

# Enable strict mode
Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

# Show help if requested
if ($Help) {
    Write-Host @"
Ion Vortex Bootstrap Script for Windows

Usage: .\bootstrap.ps1 [options]

Options:
    -Help       Show this help message

This script will:
1. Check for required tools (CMake, Git, Visual Studio)
2. Set up vcpkg if needed
3. Install all dependencies via vcpkg
4. Prepare your environment for building Ion Vortex

"@
    exit 0
}

# Helper functions
function Write-Info {
    param([string]$Message)
    Write-Host "[INFO] " -ForegroundColor Green -NoNewline
    Write-Host $Message
}

function Write-Error {
    param([string]$Message)
    Write-Host "[ERROR] " -ForegroundColor Red -NoNewline
    Write-Host $Message -ForegroundColor Red
}

function Write-Warning {
    param([string]$Message)
    Write-Host "[WARN] " -ForegroundColor Yellow -NoNewline
    Write-Host $Message
}

# Check if running as admin (not required, but note if not)
$currentPrincipal = New-Object Security.Principal.WindowsPrincipal([Security.Principal.WindowsIdentity]::GetCurrent())
$isAdmin = $currentPrincipal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)

if (-not $isAdmin) {
    Write-Warning "Not running as administrator. This is usually fine."
}

Write-Info "Starting Ion Vortex bootstrap for Windows..."

# Check for Git
Write-Info "Checking for Git..."
try {
    $gitVersion = git --version
    Write-Info "Found $gitVersion ✓"
} catch {
    Write-Error "Git not found. Please install Git for Windows."
    Write-Info "Download from: https://git-scm.com/download/win"
    exit 1
}

# Check for CMake
Write-Info "Checking for CMake..."
try {
    $cmakeVersion = cmake --version | Select-Object -First 1
    $versionMatch = [regex]::Match($cmakeVersion, '(\d+)\.(\d+)\.(\d+)')
    if ($versionMatch.Success) {
        $major = [int]$versionMatch.Groups[1].Value
        $minor = [int]$versionMatch.Groups[2].Value
        
        if ($major -lt 3 -or ($major -eq 3 -and $minor -lt 28)) {
            Write-Error "CMake version is too old. Found $cmakeVersion, need 3.28 or later."
            exit 1
        }
    }
    Write-Info "Found $cmakeVersion ✓"
} catch {
    Write-Error "CMake not found. Please install CMake 3.28 or later."
    Write-Info "Download from: https://cmake.org/download/"
    exit 1
}

# Check for Visual Studio
Write-Info "Checking for Visual Studio..."
$vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (Test-Path $vsWhere) {
    $vsPath = & $vsWhere -latest -property installationPath
    if ($vsPath) {
        $vsVersion = & $vsWhere -latest -property installationVersion
        Write-Info "Found Visual Studio at: $vsPath (version $vsVersion) ✓"
    } else {
        Write-Error "Visual Studio not found. Please install Visual Studio 2022 with C++ development tools."
        exit 1
    }
} else {
    # Try to detect VS another way
    $vs2022 = "${env:ProgramFiles}\Microsoft Visual Studio\2022"
    if (Test-Path $vs2022) {
        Write-Info "Found Visual Studio 2022 ✓"
    } else {
        Write-Error "Visual Studio not found. Please install Visual Studio 2022 with C++ development tools."
        exit 1
    }
}

# Get script directory and project root
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$projectRoot = Split-Path -Parent $scriptDir

# Check/setup vcpkg
if ($env:VCPKG_ROOT -and (Test-Path "$env:VCPKG_ROOT\vcpkg.exe")) {
    Write-Info "Using existing vcpkg at: $env:VCPKG_ROOT"
    $vcpkgRoot = $env:VCPKG_ROOT
} else {
    # Use local vcpkg in project
    $vcpkgRoot = Join-Path $projectRoot "vcpkg-instance"
    
    if (-not (Test-Path $vcpkgRoot)) {
        Write-Info "Cloning vcpkg..."
        git clone https://github.com/Microsoft/vcpkg.git $vcpkgRoot
        if ($LASTEXITCODE -ne 0) {
            Write-Error "Failed to clone vcpkg"
            exit 1
        }
    }
    
    if (-not (Test-Path "$vcpkgRoot\vcpkg.exe")) {
        Write-Info "Bootstrapping vcpkg..."
        Push-Location $vcpkgRoot
        try {
            .\bootstrap-vcpkg.bat
            if ($LASTEXITCODE -ne 0) {
                Write-Error "Failed to bootstrap vcpkg"
                exit 1
            }
        } finally {
            Pop-Location
        }
    }
    
    $env:VCPKG_ROOT = $vcpkgRoot
    Write-Info "vcpkg ready at: $vcpkgRoot"
}

# Install dependencies via vcpkg
Write-Info "Installing dependencies via vcpkg..."
Write-Info "This may take 10-30 minutes on first run..."

# Create a temporary cmake directory
$tempCmakeDir = Join-Path $projectRoot "build\bootstrap-temp"
New-Item -ItemType Directory -Force -Path $tempCmakeDir | Out-Null

# Configure with CMake to trigger vcpkg
Write-Info "Running CMake configure to install vcpkg dependencies..."
Push-Location $projectRoot
try {
    # Run cmake and capture output
    $cmakeArgs = @(
        "-S", ".",
        "-B", $tempCmakeDir,
        "--preset", "debug-windows"
    )
    
    $process = Start-Process -FilePath "cmake" -ArgumentList $cmakeArgs -NoNewWindow -PassThru -RedirectStandardOutput "$tempCmakeDir\cmake-out.log" -RedirectStandardError "$tempCmakeDir\cmake-err.log"
    
    # Show progress while cmake runs
    while (-not $process.HasExited) {
        Write-Host "." -NoNewline
        Start-Sleep -Seconds 2
    }
    Write-Host ""
    
    if ($process.ExitCode -ne 0) {
        Write-Error "CMake configuration failed. Check the logs:"
        Write-Error "  Output: $tempCmakeDir\cmake-out.log"
        Write-Error "  Errors: $tempCmakeDir\cmake-err.log"
        
        # Show last few lines of error log
        if (Test-Path "$tempCmakeDir\cmake-err.log") {
            Write-Host "`nLast few lines of error log:" -ForegroundColor Yellow
            Get-Content "$tempCmakeDir\cmake-err.log" -Tail 20
        }
        exit 1
    }
} finally {
    Pop-Location
}

# Clean up temp directory
Remove-Item -Recurse -Force $tempCmakeDir -ErrorAction SilentlyContinue

Write-Info ""
Write-Info "Bootstrap complete! ✓"
Write-Info ""
Write-Info "Next steps:"
Write-Info "  1. Configure: cmake --preset debug-windows"
Write-Info "  2. Build:     cmake --build --preset debug-windows"
Write-Info "  3. Run:       .\build\debug\Windows\apps\client\client.exe"
Write-Info ""
Write-Info "For release builds, use: release-windows"
Write-Info ""
Write-Info "Tip: If you see 'cannot find compiler' errors, use the Visual Studio"
Write-Info "     generator (default) instead of Ninja."