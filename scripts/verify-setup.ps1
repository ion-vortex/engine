# Verify development environment setup for Oxide - Windows
param(
    [switch]$Detailed
)

# Enable strict mode
Set-StrictMode -Version Latest
$ErrorActionPreference = "Continue"

# Counters
$script:Errors = 0
$script:Warnings = 0

# Helper functions
function Check-Pass {
    param([string]$Message)
    Write-Host "✓ " -ForegroundColor Green -NoNewline
    Write-Host $Message
}

function Check-Fail {
    param([string]$Message)
    Write-Host "✗ " -ForegroundColor Red -NoNewline
    Write-Host $Message -ForegroundColor Red
    $script:Errors++
}

function Check-Warn {
    param([string]$Message)
    Write-Host "⚠ " -ForegroundColor Yellow -NoNewline
    Write-Host $Message -ForegroundColor Yellow
    $script:Warnings++
}

function Write-Header {
    param([string]$Title)
    Write-Host "`n=== $Title ===" -ForegroundColor Blue
}

# Main verification
Write-Host "Oxide Development Environment Verification" -ForegroundColor Cyan
Write-Host "=========================================" -ForegroundColor Cyan
Write-Host "OS: Windows $([System.Environment]::OSVersion.Version)"
Write-Host ""

# Check Git
Write-Header "Version Control"
try {
    $gitVersion = git --version
    if ($gitVersion) {
        Check-Pass $gitVersion
        
        # Check if in repo
        $gitDir = git rev-parse --git-dir 2>$null
        if ($LASTEXITCODE -eq 0) {
            $branch = git rev-parse --abbrev-ref HEAD 2>$null
            Check-Pass "In git repository (branch: $branch)"
            
            $remote = git config --get remote.origin.url 2>$null
            if ($remote -like "*git.oat.im/oat/oxide*") {
                Check-Pass "Correct remote: $remote"
            } elseif ($remote) {
                Check-Warn "Unexpected remote: $remote"
            }
        } else {
            Check-Warn "Not in a git repository"
        }
    }
} catch {
    Check-Fail "Git not found"
}

# Check build tools
Write-Header "Build Tools"

# CMake
try {
    $cmakeOutput = cmake --version 2>$null
    if ($cmakeOutput) {
        $cmakeVersion = $cmakeOutput[0]
        if ($cmakeVersion -match '(\d+)\.(\d+)\.(\d+)') {
            $major = [int]$Matches[1]
            $minor = [int]$Matches[2]
            
            if ($major -gt 3 -or ($major -eq 3 -and $minor -ge 28)) {
                Check-Pass $cmakeVersion
            } else {
                Check-Fail "$cmakeVersion (need 3.28+)"
            }
        } else {
            Check-Warn "CMake found but version unclear"
        }
    }
} catch {
    Check-Fail "CMake not found"
}

# Check for Visual Studio
Write-Header "C++ Compiler"

$vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vsFound = $false

if (Test-Path $vsWhere) {
    $vsPath = & $vsWhere -latest -property installationPath 2>$null
    if ($vsPath) {
        $vsVersion = & $vsWhere -latest -property catalog_productDisplayVersion 2>$null
        $vsName = & $vsWhere -latest -property displayName 2>$null
        Check-Pass "$vsName ($vsVersion)"
        $vsFound = $true
        
        # Check for C++ workload
        $vsComponents = & $vsWhere -latest -property packages 2>$null
        if ($vsComponents -like "*Microsoft.VisualStudio.Workload.VCTools*") {
            Check-Pass "C++ development tools installed"
        } else {
            Check-Fail "C++ development tools not found in Visual Studio"
        }
    }
}

if (-not $vsFound) {
    # Try alternative detection
    $vs2022Path = "${env:ProgramFiles}\Microsoft Visual Studio\2022"
    if (Test-Path $vs2022Path) {
        $editions = Get-ChildItem $vs2022Path -Directory
        if ($editions.Count -gt 0) {
            Check-Pass "Visual Studio 2022 found at: $vs2022Path\$($editions[0].Name)"
            $vsFound = $true
        }
    }
}

if (-not $vsFound) {
    Check-Fail "Visual Studio 2022 not found"
}

# Test C++23 support
if ($vsFound -and $Detailed) {
    Write-Header "C++ Standard Support"
    
    $testCode = @"
#include <expected>
#include <print>
int main() {
    std::expected<int, int> e{42};
    return e.value();
}
"@
    
    $testFile = "$env:TEMP\oxide_cpp23_test.cpp"
    Set-Content -Path $testFile -Value $testCode
    
    # This is complex on Windows without being in VS environment
    Check-Warn "C++23 support check requires Visual Studio Developer Command Prompt"
    Remove-Item $testFile -ErrorAction SilentlyContinue
}

# Check vcpkg
Write-Header "Package Manager"

$vcpkgFound = $false
if ($env:VCPKG_ROOT -and (Test-Path "$env:VCPKG_ROOT\vcpkg.exe")) {
    Check-Pass "vcpkg found at: $env:VCPKG_ROOT"
    $vcpkgFound = $true
} else {
    # Check for local vcpkg
    $scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
    $projectRoot = Split-Path -Parent $scriptDir
    $localVcpkg = Join-Path $projectRoot "vcpkg-instance"
    
    if (Test-Path "$localVcpkg\vcpkg.exe") {
        Check-Warn "vcpkg found locally at: $localVcpkg (VCPKG_ROOT not set)"
        Write-Host "    Run: `$env:VCPKG_ROOT = '$localVcpkg'"
        $vcpkgFound = $true
    } else {
        Check-Fail "vcpkg not found (run bootstrap.ps1 first)"
    }
}

# Check disk space
Write-Header "System Resources"

$drive = (Get-Location).Drive
if ($drive) {
    $driveInfo = Get-PSDrive $drive.Name -ErrorAction SilentlyContinue
    if ($driveInfo) {
        $freeGB = [math]::Round($driveInfo.Free / 1GB, 1)
        if ($freeGB -ge 20) {
            Check-Pass "Disk space: ${freeGB}GB available"
        } elseif ($freeGB -ge 10) {
            Check-Warn "Disk space: ${freeGB}GB available (20GB recommended)"
        } else {
            Check-Fail "Disk space: ${freeGB}GB available (20GB required)"
        }
    }
}

# Check memory
$totalMemoryGB = [math]::Round((Get-CimInstance Win32_ComputerSystem).TotalPhysicalMemory / 1GB, 1)
if ($totalMemoryGB -ge 16) {
    Check-Pass "Memory: ${totalMemoryGB}GB"
} elseif ($totalMemoryGB -ge 8) {
    Check-Warn "Memory: ${totalMemoryGB}GB (16GB recommended for faster builds)"
} else {
    Check-Fail "Memory: ${totalMemoryGB}GB (8GB minimum required)"
}

# Check if we can build
Write-Header "Build Environment Test"

if (Test-Path "CMakeLists.txt") {
    Check-Pass "CMakeLists.txt found"
    
    if (Test-Path "CMakePresets.json") {
        Check-Pass "CMakePresets.json found"
        
        # Try to list presets
        try {
            $presets = cmake --list-presets 2>$null
            if ($LASTEXITCODE -eq 0) {
                Check-Pass "CMake presets accessible"
            } else {
                Check-Warn "Cannot list CMake presets"
            }
        } catch {
            Check-Warn "Cannot test CMake presets"
        }
    } else {
        Check-Fail "CMakePresets.json not found"
    }
} else {
    Check-Fail "Not in Oxide project root (CMakeLists.txt not found)"
}

# Check for common issues
Write-Header "Common Windows Issues"

# Check Windows Defender exclusions
$defenderEnabled = (Get-MpComputerStatus -ErrorAction SilentlyContinue).RealTimeProtectionEnabled
if ($defenderEnabled) {
    Check-Warn "Windows Defender real-time protection is enabled"
    Write-Host "    Consider adding build directories to exclusions for faster builds"
}

# Check if in OneDrive/Dropbox folder
$currentPath = (Get-Location).Path
if ($currentPath -like "*\OneDrive\*" -or $currentPath -like "*\Dropbox\*") {
    Check-Warn "Project is in a cloud-synced folder"
    Write-Host "    This can cause build issues. Consider moving to a local folder."
}

# Summary
Write-Host ""
Write-Host "=========================================" -ForegroundColor Cyan

if ($script:Errors -eq 0) {
    if ($script:Warnings -eq 0) {
        Write-Host "All checks passed!" -ForegroundColor Green
        Write-Host ""
        Write-Host "Your environment is ready. Next steps:"
        Write-Host "  1. Run: .\scripts\bootstrap.ps1"
        Write-Host "  2. Run: cmake --preset debug-windows"
        Write-Host "  3. Run: cmake --build --preset debug-windows"
    } else {
        Write-Host "Setup complete with $($script:Warnings) warnings" -ForegroundColor Yellow
        Write-Host ""
        Write-Host "Your environment should work, but check the warnings above."
    }
    exit 0
} else {
    Write-Host "Setup incomplete: $($script:Errors) errors, $($script:Warnings) warnings" -ForegroundColor Red
    Write-Host ""
    Write-Host "Please fix the errors above before proceeding."
    exit 1
}