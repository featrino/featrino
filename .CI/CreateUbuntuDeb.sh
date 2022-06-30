#!/bin/sh
set -e

if [ ! -f ./bin/featrino ] || [ ! -x ./bin/featrino ]; then
    echo "ERROR: No featrino binary file found. This script must be run in the build folder, and featrino must be built first."
    exit 1
fi

# chatterino_version=$(git describe --tags | cut -c 2-)
chatterino_version="0.0.3"
echo "Found featrino version $chatterino_version via git"

rm -vrf "./package" || true  # delete any old packaging dir

# create ./package/ from scratch
mkdir package/DEBIAN -p
packaging_dir="$(realpath ./package)"

echo "Making control file"
cat >> "$packaging_dir/DEBIAN/control" << EOF
Package: featrino
Section: net
Priority: optional
Architecture: amd64
Maintainer: goldbattle <no-reply@featrino.com>
Description: featrino as a Ubuntu package
Depends: libc6, libqt5concurrent5, libqt5core5a, libqt5dbus5, libqt5gui5, libqt5multimedia5, libqt5network5, libqt5svg5, libqt5widgets5, libssl1.1, libstdc++6
EOF
echo "Version: $chatterino_version" >> "$packaging_dir/DEBIAN/control"

echo "Running make install in package dir"
DESTDIR="$packaging_dir" make INSTALL_ROOT="$packaging_dir" -j"$(nproc)" install; find "$packaging_dir/"
echo ""

echo "Building package..."
dpkg-deb --build "$packaging_dir" "featrino.deb"
