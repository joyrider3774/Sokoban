#!/bin/sh
# ============================================================================
# build-appimage.sh - Build a sharun-based AppImage for an SDL game
# ============================================================================
#
# Place and run the script in the source root directory.
#
# Self-contained: downloads quick-sharun and appimagetool if not found,
# builds the game from source with vendored (static) SDL3, bundles
# everything into a portable AppImage using sharun.
#
# The resulting AppImage works on any Linux distro (including old and
# musl-based ones) and does not require FUSE.
#
# Prerequisites: cmake, make, a C++ compiler, git, wget, patchelf, cc
#   On Debian/Ubuntu:  sudo apt install build-essential cmake git wget patchelf
#   On Arch:           sudo pacman -S base-devel cmake git wget patchelf
#   On Fedora:         sudo dnf install gcc-c++ cmake git wget patchelf
#
# Usage:
#   chmod +x build-appimage.sh
#   ./build-appimage.sh
#
# The finished AppImage lands in dist/
#
# To adapt this for another SDL game, change the variables below.
# ============================================================================

set -eu

# ---------------------------------------------------------------------------
# Game-specific settings - change these for each game
# ---------------------------------------------------------------------------

# Name of the binary that cmake produces (must match add_executable() name)
APP_NAME="Sokoban"

# Reverse-DNS application ID for the .desktop file ( GNOME demands that )
APP_ID="be.willemssoft.sokobangp2x"

# One-line description shown in desktop menus
APP_COMMENT="Sokoban engine initially made for the gp2x"

# Desktop entry categories (semicolon-separated, must end with semicolon)
# Full list: https://specifications.freedesktop.org/menu-spec/latest/category-registry.html
APP_CATEGORIES="Game;PuzzleGame;"

# Path to the icon file inside the source tree (PNG, ideally 256x256)
APP_ICON="graphics/sokoban.png"

# Asset directories that ship with the binary.
# The game finds them via SDL_GetBasePath(), so they sit next to the
# executable inside the AppImage.
APP_ASSET_DIRS="fonts graphics levelpacks music sound"

# CMake flag to build SDL3 from the vendored submodules (static linking).
# Removes the need for SDL3 system packages.
CMAKE_EXTRA_FLAGS="-DUSE_VENDORED_SDL=ON"

# ---------------------------------------------------------------------------
# Build settings
# ---------------------------------------------------------------------------

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
SOURCE_DIR="$SCRIPT_DIR"
BUILD_DIR="$SCRIPT_DIR/_build"
DIST_DIR="$SCRIPT_DIR/dist"
TOOLS_DIR="$BUILD_DIR/tools"
ARCH="$(uname -m)"

# Compression level for the AppImage filesystem.
# Lower = faster startup, larger file. Higher = smaller file, slower startup.
# Default in quick-sharun is 22 (maximum). 9 is a good balance.
export DWARFS_COMP="zstd:level=9 -S26 -B6"

# ---------------------------------------------------------------------------
# Cleanup previous build
# ---------------------------------------------------------------------------

cleanup() {
    echo "Cleaning previous build..."
    rm -rf "$BUILD_DIR"
    mkdir -p "$BUILD_DIR" "$DIST_DIR" "$TOOLS_DIR"
}

# ---------------------------------------------------------------------------
# Version detection - reads the latest vX.Y.Z git tag from git, otherwise 
#                     set the version statically
# ---------------------------------------------------------------------------

detect_version() {
    cd "$SOURCE_DIR"
    APP_VERSION=$(git tag -l 'v*' --sort=-v:refname 2>/dev/null \
        | head -1 | sed 's/^v//')
    if [ -z "$APP_VERSION" ]; then
        APP_VERSION="git-$(git rev-parse --short HEAD 2>/dev/null || echo 'unknown')"
    fi
    export VERSION="$APP_VERSION"
    echo "Version: $APP_VERSION"
}

# ---------------------------------------------------------------------------
# Download quick-sharun if it is not available
# ---------------------------------------------------------------------------
# quick-sharun is a shell script that handles:
#   - bundling the binary and its shared libraries with sharun
#   - downloading sharun, appimagetool, and uruntime automatically
#   - creating the final AppImage
# It downloads its own tooling on first run, so we only need the script.

get_quick_sharun() {
    QUICK_SHARUN="$TOOLS_DIR/quick-sharun"

    if [ -x "$QUICK_SHARUN" ]; then
        echo "quick-sharun already present."
        return
    fi

    echo "Downloading quick-sharun..."
    wget -q -O "$QUICK_SHARUN" \
        "https://raw.githubusercontent.com/pkgforge-dev/Anylinux-AppImages/main/useful-tools/quick-sharun.sh"
    chmod +x "$QUICK_SHARUN"
    echo "quick-sharun ready."
}

# ---------------------------------------------------------------------------
# Build the game
# ---------------------------------------------------------------------------

build_game() {
    echo ""
    echo "Building $APP_NAME..."
    echo "---------------------------------------------------------------"

    mkdir -p "$BUILD_DIR/cmake"
    cd "$BUILD_DIR/cmake"

    cmake "$SOURCE_DIR" \
        -DCMAKE_BUILD_TYPE=Release \
        $CMAKE_EXTRA_FLAGS

    make -j"$(nproc)"

    echo "$APP_NAME built."
}

# ---------------------------------------------------------------------------
# Create the .desktop file and prepare the icon
# ---------------------------------------------------------------------------

create_desktop_and_icon() {
    DESKTOP_FILE="$BUILD_DIR/${APP_ID}.desktop"
    cat > "$DESKTOP_FILE" <<EOF
[Desktop Entry]
Type=Application
Name=${APP_NAME}
Comment=${APP_COMMENT}
Exec=${APP_NAME}
Icon=${APP_ID}
Categories=${APP_CATEGORIES}
Terminal=false
StartupNotify=true
EOF
    echo "Desktop file: $DESKTOP_FILE"

    # quick-sharun wants ICON and DESKTOP as env vars pointing to files
    SRC_ICON="$SOURCE_DIR/$APP_ICON"
    if [ ! -f "$SRC_ICON" ]; then
        echo "WARNING: icon not found at $SRC_ICON"
        ICON_FILE="DUMMY"
    else
        # Rename to match Icon=${APP_ID} regardless of the source filename
        ICON_FILE="$BUILD_DIR/${APP_ID}.png"
        cp "$SRC_ICON" "$ICON_FILE"
    fi
}

# ---------------------------------------------------------------------------
# Debloat the AppDir
# ---------------------------------------------------------------------------
# quick-sharun pulls in everything ldd finds. For a 2D SDL game most of
# that is waste. We remove it AFTER quick-sharun runs but BEFORE packing.
#
# if it is still too big, run the appimage with the --appimage-help to
# get a full list of command line switches, as well as extracting the data.

debloat_appdir() {
    echo ""
    echo "Debloating AppDir..."
    echo "---------------------------------------------------------------"

    BEFORE=$(du -sm "$APPDIR" | cut -f1)

    # Mesa/OpenGL compiler stack - the game should use the HOST system's
    # GPU driver, not a bundled software renderer. These three alone are
    # ~192 MB.
    #
    # if there are issues like you described with WSL, try to comment that out
    # 
    rm -f "$APPDIR"/lib/*/libLLVM*.so*
    rm -f "$APPDIR"/lib/*/libgallium*.so*
    rm -f "$APPDIR"/lib/*/libz3*.so*

    # ibus (input method framework) - 131 MB of data a game never touches
    rm -rf "$APPDIR"/share/ibus/

    # Unicode character database - 39 MB, not needed for game rendering
    rm -rf "$APPDIR"/share/unicode/

    # System icon themes (GIMP, LibreOffice, etc.) - the game's own icon
    # is at the AppDir root, these are just noise from the build host
    rm -rf "$APPDIR"/share/icons/

    # Color profiles - not relevant for a game
    rm -rf "$APPDIR"/share/color/

    # Locale data - the game doesn't use gettext/i18n.
    #               if you have that, you can delete everything except your own locale
    #               files
    rm -rf "$APPDIR"/lib/locale/

    # GNOME/desktop data that slipped in
    # should be inspected if something is needed there (but probably mostly not)
    rm -rf "$APPDIR"/share/gnome/
    rm -rf "$APPDIR"/share/images/
    rm -rf "$APPDIR"/share/menu/
    rm -rf "$APPDIR"/share/pam/

    AFTER=$(du -sm "$APPDIR" | cut -f1)
    echo "Debloated: ${BEFORE} MB -> ${AFTER} MB (removed $((BEFORE - AFTER)) MB)"
}

# ---------------------------------------------------------------------------
# Assemble and package with quick-sharun
# ---------------------------------------------------------------------------

make_appimage() {
    echo ""
    echo "Creating AppImage with sharun..."
    echo "---------------------------------------------------------------"

    cd "$BUILD_DIR"

    # The binary built by cmake (check CMakeLists.txt for the output directory)
    BINARY="$BUILD_DIR/cmake/Release/$APP_NAME"
    if [ ! -f "$BINARY" ]; then
        # Some cmake configs put it directly in the build dir
        BINARY="$BUILD_DIR/cmake/$APP_NAME"
    fi

    if [ ! -f "$BINARY" ]; then
        echo "ERROR: cannot find built binary" >&2
        exit 1
    fi

    # Tell quick-sharun where to put the result and what to call it
    export ARCH
    export OUTPATH="$DIST_DIR"
    export ICON="$ICON_FILE"
    export DESKTOP="$DESKTOP_FILE"
    export MAIN_BIN="$APP_NAME"

    # Step 1: Deploy the binary with quick-sharun.
    # This bundles the binary, its shared libraries, and the sharun loader
    # into an AppDir structure. For a statically-linked SDL3 game, the only
    # shared deps are glibc and libstdc++.
    "$QUICK_SHARUN" "$BINARY"

    # The AppDir is now at ./AppDir (quick-sharun's default)
    APPDIR="$BUILD_DIR/AppDir"

    # Step 2: Copy game assets next to the binary.
    # SDL_GetBasePath() returns the directory of the running executable,
    # and the game loads assets relative to that. In the sharun AppDir
    # layout the binary ends up in AppDir/bin/.
    for dir in $APP_ASSET_DIRS; do
        if [ -d "$SOURCE_DIR/$dir" ]; then
            cp -a "$SOURCE_DIR/$dir" "$APPDIR/bin/$dir"
        else
            echo "WARNING: asset directory '$dir' not found, skipping"
        fi
    done

    # Step 3: Debloat.
    # quick-sharun traces all shared library dependencies and pulls them in.
    # For an SDL game that statically links SDL3, it still picks up the
    # build system's Mesa/OpenGL stack (LLVM, gallium, z3) and various
    # desktop data (ibus, unicode tables, icon themes). These are either
    # unnecessary or better served by the host system's own drivers.
    debloat_appdir

    # Step 4: Turn the AppDir into a compressed AppImage.
    "$QUICK_SHARUN" --make-appimage

    echo ""
    echo "---------------------------------------------------------------"
    echo "Done!"
    ls -lh "$DIST_DIR"/*.AppImage 2>/dev/null
    echo ""
    echo "To run it:"
    echo "  chmod +x dist/*.AppImage"
    echo "  ./dist/*.AppImage"
    echo "---------------------------------------------------------------"
}

# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

echo "============================================================"
echo "  Building AppImage for $APP_NAME"
echo "============================================================"
echo ""

cleanup
detect_version
get_quick_sharun
build_game
create_desktop_and_icon
make_appimage
