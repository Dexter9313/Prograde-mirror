[![Build Status](https://travis-ci.org/Dexter9313/Prograde-mirror.svg?branch=master)](https://travis-ci.org/Dexter9313/Prograde-mirror)
[![Build status](https://ci.appveyor.com/api/projects/status/j4j7vqkxwuibkde7/branch/master?svg=true)](https://ci.appveyor.com/project/Dexter9313/prograde-mirror/branch/master)

# Prograde

An ambitious space flight simulator.
/!\ The project is currently in its very early development stage, don't expect much as of now. Please come back soon for a more usable simulator.

## Releases

You can download releases for GNU/Linux and Windows produced by the engine build system through Travis CI and Appveyor on Github : [Prograde Releases](https://github.com/Dexter9313/Prograde-mirror/releases).

## Installation and requirements

### Microsoft Windows

Simply run the Prograde-VERSION-windows-ARCH_setup.exe setup wizard, where VERSION is the desired version and ARCH your system architecture.

If you want to run Prograde from a portable zip archive instead, you will need to manually install this (if you don't have it on your system already) :
* [Microsoft Visual C++ 2015](https://www.microsoft.com/en-US/download/details.aspx?id=48145) (zip installation only; it is shipped with the setup wizard)

### GNU/Linux binaries

You will need the following requirements :
* OpenGL
* Boost
* Qt5 Core and Gui (libqt5core5a and libqt5gui5 packages on Ubuntu)
* Assimp

You can then install the DEB package you want from the Releases page or use the portable zip version.

### GNU/Linux building from source

You will need the following requirements :
* A C++ compiler (g++ for example)
* CMake
* OpenGL dev
* Boost dev
* Qt Core and Gui (qtbase5-dev packages on Ubuntu)
* Assimp dev
* [OpenVR](https://github.com/ValveSoftware/openvr)
* (Optional) [Leap Motion SDK 2.3.1](https://developer.leapmotion.com/sdk/v2)


Then clone this repository. We now suppose the root directory of the repository is stored in the $Prograde_ROOT_DIR variable.

        cd $Prograde_ROOT_DIR
        ./build-linux.sh
        cd build
        sudo make install

Optionally, you can generate a deb package to make installation managing easier if you are on a debian-based system. The package name will be "prograde".

        cd $Prograde_ROOT_DIR
        ./build-linux.sh package
        sudo dpkg -i ./build/*.deb

## Uninstall

If the deb method for installation was used :

        sudo apt-get autoremove prograde

If the make install method for installation was used, uninstallation can only be done if the build directory has been left untouched since installation (at least the install_manifest.txt file within it) :

        cd $Prograde_ROOT_DIR
        cd build
        sudo make uninstall

As of now, this method can leave some empty directories in your file system though... Check install_manifest.txt content yourself if you want to clean everything properly.
