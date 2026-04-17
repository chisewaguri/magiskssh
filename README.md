## MagiskSSH (Mountless Fork)
a personal fork of https://gitlab.com/d4rcm4rc/MagiskSSH

mounting is not needed for this module to work on APatch or KSU and mounting also creates detection points. unnecessary mounting bad.

this fork's main purpose is to make the module mountless on KSU and APatch, since they inject their bin directory under /data/adb into PATH, so there's no need to mount anything to /system/bin. 

main changes made are:
  - move lib and libexec to /data/adb/ssh
  - use ksu and ap bin directory for the ssh binaries

An SSH server for Android devices having Magisk/KSU (build system)
==============================================================

This is my WIP for a fully functional system-daemon-like SSH server for Android devices.

Its core is a version of OpenSSH modified to be usable on Android. It also includes rsync (which actually was my main motivation for this project). It will be available for devices using the architectures arm, arm64, x86, x86_64.

This repository is a collection of build scripts for simply building an installable Magisk module. It can not be installed itself. Once I have a working installable package, I will link it here.

It should run on all devices with Android API version 24 or higher (Android 7.0 Nougat and higher) that have [Magisk](https://github.com/topjohnwu/Magisk) installed. It includes binaries for arm, arm64, x86, x86_64. However I only tested it on my arm64 Samsung Galaxy S10+ running LineageOS 20.

## Download and Install

The module can be downloaded in the [Releases repository](https://gitlab.com/d4rcm4rc/MagiskSSH_releases). Once Gitlab raises their size limit on tag-attachments, the releases will migrate there. Further hints for installation, configuration and usage can be found [in the module's README.md](module_data/README.md).

You don't trust me and don't want to use binaries I compiled? No problem at all! Just head to [How To Build](#how-to-build), grab the source code, check it and compile it yourself.


## Used Packages and Included Resources

* [OpenSSL](https://www.openssl.org/) (only needed for its libcrypto)
* [OpenSSH](https://www.openssh.com/)
* [Rsync](https://rsync.samba.org/)
* [Magisk Module Template](https://github.com/topjohnwu/magisk-module-template)

Some changes to OpenSSH are used from [Arachnoid's SSHelper](https://arachnoid.com/android/SSHelper/). Also I have to partially ship a version of `resolv.h` from my system. It is, as far as I can tell, an 'internal-only' header and not included in the Android NDK. Still OpenSSH somehow needs it to compile.

## How To Build

    <clone or download>
    cd <source dir>
    mkdir build
    cd build
    make -f ../all_arches.mk -j8 zip

A zip file will be created in the build-directory. It can be copied to the Android device and installed via the Magisk Manager app.

On my i7-6700k a full build using all cores takes about 4 minutes.
The Android-NDK path is set to `/opt/android-ndk` per default. It can be changed by passing `ANDROID_ROOT=/path/to/ndk` to make or exporting it:

    ...
    export ANDROID_ROOT=/path/to/ndk
    make -f ../all_arches.mk -j8 zip

## Build Dependencies

* Recent GNU/Linux system on amd64
* Make. Only tested using GNU Make 4.4.1
* Wget. Only tested using GNU Wget 1.25.0
* Android NDK. Only tested using version r25c
* Python3. Only tested using Python 3.13.3
* 7z (or zip as fallback). Only tested 17.05

Newer versions generally should work. Older versions may work or may not.

## Version bumping OpenSSL and rsync

A version bump for these two packages is pretty straightforward:

- Enter the new version in openssl.mk or rsync.mk
- these commands will download and generate checksums for each package:
  - `make -f all_arches.mk update_openssl_with_tofu`
  - `make -f all_arches.mk update_rsync_with_tofu`
- Update the module version and go through the checklist
- Delete build and src directories and rebuild the whole module

## Version bumping OpenSSH

A version bump for OpenSSH is more difficult. Basically, the same steps as for OpenSSL and rsync are required.
OpenSSH however also needs a patch which is different for every version.
To generate one for a new version do this:

- Unpack the new version's source to a directory twice (ie. `tar xzf openssh-version.tar.gz; mv openssh-version a; cp -a a b`)
- Try to apply the patch to b, it will not patch without issues (`cd b; patch -p1 < path/to/previous.patch`)
- Fix all errors and warnings
- Remove the pre-patch backups and reject files
- Possibly add more changes. Candidates are:
  - Calls to getpwnam
  - Calls to getpwuid
  - Direct uses of /tmp as path for temporary files
- Generate a new patch (`diff -urN a b > path/to/new.patch`)
- Try to build the module. If not possible, fix errors and generate a new patch

## Checklist for a new version

- All packages have the correct version
- For all updated packages checksum files have been generated
- A new version is entered in module_data/module.prop under both `version` and `versionCode`
- The module_data/README.md is updated to include the new package versions
- An entry in the changelog in module_data/README.md is added

Then we can create a full build (delete _build_ directory first), upload it to
the releases repository and update the update.json in here.

## License

This program is under the GPLv3. It downloads and bundles software with different licenses:

* OpenSSL [OpenSSL License](https://www.openssl.org/source/license.html)
* OpenSSH [BSD License](https://www.openbsd.org/policy.html)
* Rsync [GPL v3](https://rsync.samba.org/GPL.html)
