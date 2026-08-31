# Geometry Wars 3: Dimensions - Linux handheld port

A native Linux port of the Android version of **Geometry Wars 3: Dimensions**,
targeting ARMv7 (32-bit) Linux handhelds such as the R36S / ArkOS family (Rockchip 
RK3326, Mali GPU) running a Debian-based or buildroot CFW.

The port loads the original Android engine `libgwnext.so` directly inside a
small Linux host binary that emulates just enough of Android's runtime
(Java/JNI vtable, bionic `pthread`/`__cxa_guard` ABI, Android logging, the NDK
`AAssetManager`) to make the engine think it is still on Android. Graphics
(GLES 2.0) are bridged to the system `libGLESv2`/`libEGL` with soft-float →
hard-float ABI thunks. Audio is the game's own **FMOD** Ex/Event libraries
(also extracted from the APK), loaded by the host and resolved into the engine
at runtime.

---

## Additional requirements

These come from v1.0.0 Android version of the game (Google Play or your
own sideload). They are **not** in this repo for copyright reasons.
evolved
- `main.35.com.activision.gw3.dimensions.obb` - game data
- The game APK - you can drop either:
  - a `.apk` (the engine + FMOD `.so` files are extracted from it at run time),
    or
  - a `.xapk` / `.apkm` bundle, which already contains **both** the APK and the
    OBB.

The launcher unpacks `libgwnext.so`, `libfmodex.so` and `libfmodevent.so` from
the APK automatically; the engine reads the OBB directly.

---

## Building the PortMaster zip

A GitHub Actions workflow (`.github/workflows/build.yml`) does the full build:

1. Builds the cross-compile Docker image (`Dockerfile`).
2. Generates the Debian 11 "bullseye" armhf sysroot
   (`scripts/build-bullseye-sysroot.sh`) so the binary keeps a low glibc floor
   (2.31) for older PortMaster CFWs.
3. Cross-compiles (`make SYSROOT=/ portmaster`) producing `gw3.pm` +
   `libclock_fix.so`.
4. Assembles the PortMaster zip and uploads it as an artifact named `gw3`.

The zip has the standard PortMaster layout - the launcher script at the root,
everything else in the `gw3/` subdirectory:

```
gw3.zip
├── Geometry Wars 3.sh
└── gw3/
    ├── port.json
    ├── README.md
    ├── screenshot.png
    ├── gameinfo.xml
    ├── gw3                        (built from gw3.pm)
    ├── libclock_fix.so
    ├── gw3.gptk
    └── licenses/
```

### Building locally

```bash
# Cross-compile toolchain (one-time, on a Debian/Ubuntu Linux host)
sudo dpkg --add-architecture armhf
sudo apt-get update
sudo apt-get install -y \
    gcc-arm-linux-gnueabihf \
    libsdl2-dev:armhf \
    libgles2-mesa-dev:armhf \
    libegl1-mesa-dev:armhf \
    zlib1g-dev:armhf

# Build the old-glibc (PortMaster) binary: ./gw3.pm + ./libclock_fix.so
make portmaster
```

Or build in Docker:

```bash
./docker-make.sh portmaster
```

To reproduce CI fully, generate the bullseye sysroot first with
`scripts/build-bullseye-sysroot.sh`, then copy `gw3.pm` → `gw3` and
`libclock_fix.so` into `gw3-portmaster/gw3/` and zip the `gw3-portmaster/`
directory.

---

## Installing on the device

Install through PortMaster, or unpack the zip so the launcher sits next to
the `gw3/` folder. Then drop your user-supplied files into the port's `gw3/`
directory:

- `main.35.com.activision.gw3.dimensions.obb`
- the game APK - either a `.apk`, or a `.xapk` / `.apkm` bundle (which already
  contains the OBB too)

Launch the port. The launcher auto-extracts the engine + FMOD libraries
(`libgwnext.so`, `libfmodex.so`, `libfmodevent.so`) from the APK on first run,
verifies everything is present, then runs `gw3`. The log lives at
`<port>/gw3/gw3.log` for postmortem debugging.

---

## Hardware / OS target

- **CPU**: ARMv7-A 32-bit (Cortex-A7/A35/A53), NEON, hard-float
- **GPU**: Mali / Panfrost GLES2 - bridged via KMS/DRM + `libGLESv2`
- **OS**: ArkOS / dArkOS / ROCKNIX / MuOS (glibc 2.31+ for the PortMaster build)
- **Audio**: the game's FMOD Ex/Event libraries via ALSA.

---
