#!/bin/sh

set -e

if [ ! -f ./bin/featrino ] || [ ! -x ./bin/featrino ]; then
    echo "ERROR: No featrino binary file found. This script must be run in the build folder, and featrino must be built first."
    exit 1
fi

export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/opt/qt512/lib/"
export PATH="/opt/qt512/bin:$PATH"

script_path=$(readlink -f "$0")
script_dir=$(dirname "$script_path")
chatterino_dir=$(dirname "$script_dir")


echo "Running LDD on featrino binary:"
ldd ./bin/featrino
echo ""

echo "Running make install in the appdir"
make INSTALL_ROOT=appdir -j"$(nproc)" install ; find appdir/
echo ""

cp "$chatterino_dir"/resources/icon.png ./appdir/featrino.png

linuxdeployqt_path="linuxdeployqt-6-x86_64.AppImage"
linuxdeployqt_url="https://github.com/probonopd/linuxdeployqt/releases/download/6/linuxdeployqt-6-x86_64.AppImage"

if [ ! -f "$linuxdeployqt_path" ]; then
    wget -nv "$linuxdeployqt_url"
    chmod a+x "$linuxdeployqt_path"
fi
if [ ! -f appimagetool-x86_64.AppImage ]; then
    wget -nv "https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage"
    chmod a+x appimagetool-x86_64.AppImage
fi
echo "Run LinuxDeployQT"
./"$linuxdeployqt_path" \
    appdir/usr/share/applications/*.desktop \
    -no-translations \
    -bundle-non-qt-libs \
    -unsupported-allow-new-glibc

rm -rf appdir/home
rm -f appdir/AppRun

echo "Run AppImageTool"

# shellcheck disable=SC2016
echo '#!/bin/sh
here="$(dirname "$(readlink -f "${0}")")"
export QT_QPA_PLATFORM_PLUGIN_PATH="$here/usr/plugins"
cd "$here/usr"
exec "$here/usr/bin/featrino" "$@"' > appdir/AppRun
chmod a+x appdir/AppRun

./appimagetool-x86_64.AppImage appdir
