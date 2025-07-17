#!/bin/bash
set -e  # Exit immediately if a command exits with a non-zero status

cd build || { echo "Failed to change to build directory"; exit 1; }

cmake .. || { echo "CMake configuration failed"; exit 1; }

cmake --build . --parallel || { echo "Build failed"; exit 1; }

# Check if running in GitHub Actions or other CI environments
if [ "$GITHUB_ACTIONS" = "true" ] || [ "$CI" = "true" ] || [ -n "$RUNNER_OS" ]; then
    echo "Running in CI environment - skipping app launch"
    echo "Build completed successfully"
    exit 0
fi

# Cross-platform executable detection and launch
if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS
    if [ -f "./GIMMEL-TEST_artefacts/Standalone/GIMMEL-TEST.app/Contents/MacOS/GIMMEL-TEST" ]; then
        echo "Launching macOS standalone app..."
        ./GIMMEL-TEST_artefacts/Standalone/GIMMEL-TEST.app/Contents/MacOS/GIMMEL-TEST
    else
        echo "macOS standalone app not found!"
        exit 1
    fi
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Linux
    if [ -f "./GIMMEL-TEST_artefacts/Standalone/GIMMEL-TEST" ]; then
        echo "Launching Linux standalone app..."
        ./GIMMEL-TEST_artefacts/Standalone/GIMMEL-TEST
    else
        echo "Linux standalone app not found!"
        exit 1
    fi
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]] || [[ "$OSTYPE" == "win32" ]]; then
    # Windows
    if [ -f "./GIMMEL-TEST_artefacts/Debug/Standalone/GIMMEL-TEST.exe" ]; then
        echo "Launching Windows standalone app..."
        ./GIMMEL-TEST_artefacts/Debug/Standalone/GIMMEL-TEST.exe
    else
        echo "Windows standalone app not found!"
        exit 1
    fi
else
    echo "Unknown operating system: $OSTYPE"
    echo "Build completed, but cannot auto-launch standalone app"
    echo "Check build/GIMMEL-TEST_artefacts/Standalone/ for built executable"
fi