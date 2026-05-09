# Yet Another Magic Lamp

![Screenshot](doc/Screenshot.png)

[Demo](https://www.youtube.com/watch?v=BR4bUwFZDS8)

Yet Another Magic Lamp is a window minimization effect for KWin. Whenever a window
is minimized, it'll get sucked down into the dock/panel. Compared to the Magic Lamp
effect shipped with KWin, this effect features a smoother, more "curvy" animation,
improved multi-monitor support (including correct behavior when the panel is on a
secondary screen or positioned between displays), and more configuration options.

This project was originally created by [zzag](https://github.com/zzag/kwin-effects-yet-another-magic-lamp),
who maintained it through Plasma 5.27. Since the original repository has not been updated in
several years, this fork continues development independently, focusing exclusively on Wayland
under Plasma 6. X11 is no longer supported.

### Installation

#### Build from source

You will need the following dependencies to build this effect:
* CMake
* any C++20 enabled compiler
* Qt 6: Core, DBus, Gui, Widgets
* KWin (Plasma 6.6 or later)
* KDE Frameworks 6:
    - Config
    - ConfigWidgets
    - CoreAddons
    - Extra CMake Modules
    - KCMUtils
    - WindowSystem
* libdrm

On Arch Linux

```sh
sudo pacman -S cmake extra-cmake-modules kwin qt6-base \
    kcmutils kconfig kconfigwidgets kcoreaddons kwindowsystem libdrm
```

On Fedora

```sh
sudo dnf install cmake extra-cmake-modules kf6-kconfig-devel \
    kf6-kconfigwidgets-devel kf6-kcoreaddons-devel kf6-kcmutils-devel \
    kf6-kwindowsystem-devel kwin-devel qt6-qtbase-devel libdrm-devel
```

On Debian

```sh
sudo apt install cmake extra-cmake-modules kwin-dev \
    libkf6config-dev libkf6configwidgets-dev libkf6coreaddons-dev \
    libkf6kcmutils-dev libkf6windowsystem-dev qt6-base-dev libdrm-dev
```

After you installed all the required dependencies, you can build
the effect:

```sh
git clone https://github.com/Si13n7/kwin-effects-yet-another-magic-lamp.git
cd kwin-effects-yet-another-magic-lamp
mkdir build && cd build
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr
make
sudo make install
```


#### Building the effect against older Plasma versions

If you want to build this effect against an older Plasma (5.x) release, checkout
the corresponding `Plasma/x.yz` branch, for example

```sh
git clone https://github.com/zzag/kwin-effects-yet-another-magic-lamp.git
cd kwin-effects-yet-another-magic-lamp
git checkout Plasma/5.23
```


### Using the effect

Go to System Settings > Desktop Behavior > Desktop Effects, and select
"Yet Another Magic Lamp", then click Apply.

### Contributing

Any help is welcome. If you have suggestions how to improve this effect(e.g.
different duration for each stage of the animation, etc), please create a new
issue. If you'd like to contribute by implementing some feature, make sure
you've run clang-format before creating a PR.
