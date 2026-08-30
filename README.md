# Geometry Wars 3: Dimensions — Linux handheld port

A native Linux port of the Android version of **Geometry Wars 3: Dimensions**,
targeting ARMv7 (32-bit) Linux handhelds such as the R36S / ArkOS / NextOS
family (Rockchip RK3326, Mali GPU) running a Debian-based or buildroot CFW.

The port loads the original Android engine `libgwnext.so` directly inside a
small Linux host binary that emulates just enough of Android's runtime
(Java/JNI vtable, bionic `pthread`/`__cxa_guard` ABI, Android logging, the NDK
`AAssetManager`) to make the engine think it is still on Android. Graphics
(GLES 2.0) are bridged to the system `libGLESv2`/`libEGL` with soft-float →
hard-float ABI thunks. Audio is the game's own **FMOD** Ex/Event libraries
(also extracted from the APK), loaded by the host and resolved into the engine
at runtime.

---

## Status (scaffold)

This is a **compilable starting point**, not a finished port. What is wired up:

- Android-on-Linux host loader (`so_util`) with `RTLD_DEFAULT` fallback so the
  engine's libc/libm/libstdc++ imports resolve automatically.
- The engine's `AAssetManager` imports are served by a filesystem-backed shim
  (`aasset_patch.c`) rooted at the data directory.
- The engine's FMOD audio imports are satisfied by loading the game's own
  `libfmodex.so` / `libfmodevent.so` (`fmod_patch.c`) — they share the engine's
  `DT_NEEDED` sonames, so `so_resolve_link` binds them.
- A GLES2 soft-float↔hard-float thunk layer (`opengl_patch.c`) plus extra GLES2
  entry points the engine uses.
- A fake JNI/JavaVM environment (`jni_patch.c`) so `JNI_OnLoad` succeeds.
- The soft-float math entry points the engine calls (`fmod`, `tanf`, …) routed
  through softfp thunks.

### Known open work (runtime)

1. **JNI method names are unknown.** The engine registers native methods via
   `GetMethodID`/`RegisterNatives` with names we have not enumerated. Run the
   port with the crash/resolve logging and capture the `jni_unimpl` /
   `GetMethodID` failures, then implement those handlers in `jni_patch.c`.
2. **FMOD audio output.** The Android FMOD build `dlopen`s `libOpenSLES.so` at
   runtime, which does not exist on Linux. Audio init will fail at the output
   stage. Options: ship a desktop FMOD armhf build, or shim `libOpenSLES`.
3. **Asset container.** `AAssetManager_open` currently serves plain files from
   the data dir. If the engine expects files *inside* the packed OBB container,
   either unpack the OBB into the data dir or teach `AAssetManager_open` to read
   the container format.
4. **Input.** Twin-stick controls (move + aim/fire) need to be mapped to the
   engine's input contract, which is discovered at runtime. The `ProcessEvents`/
   controller hooks are GTA-CTW leftovers and must be adapted (`patch_gw3`).

---

## What you need to supply

These come from the legitimate Android version of the game (Google Play or your
own sideload). They are **not** in this repo for copyright reasons.

- `main.35.com.activision.gw3.dimensions.obb` — game data
- From the APK, `lib/armeabi-v7a/`:
  - `libgwnext.so`      (the engine)
  - `libfmodex.so`      (FMOD Ex)
  - `libfmodevent.so`   (FMOD Event)

The engine reads the OBB directly; there is no separate APK-asset extraction
step.

---

## Building from source

### 1. Cross-compile toolchain (one-time, on a Debian/Ubuntu Linux host)

```bash
sudo dpkg --add-architecture armhf
sudo apt-get update
sudo apt-get install -y \
    gcc-arm-linux-gnueabihf \
    libsdl2-dev:armhf \
    libgles2-mesa-dev:armhf \
    libegl1-mesa-dev:armhf \
    zlib1g-dev:armhf
```

The Makefile expects headers/libs under `armhf-sysroot/root/usr/...`. Symlink
that to your install root, or unpack the relevant `.deb` files there.

### 2. Build

```bash
make           # produces ./gw3_r36 and ./libclock_fix.so
```

For the old-glibc (PortMaster) binary:

```bash
make portmaster   # produces ./gw3_r36.pm + ./libclock_fix.so
```

Both are ARMv7-A hard-float ELFs.

### 3. Extract the engine + FMOD libs from the APK

```bash
unzip -j Geometry-Wars-3-*.apk 'lib/armeabi-v7a/libgwnext.so'  -d .
unzip -j Geometry-Wars-3-*.apk 'lib/armeabi-v7a/libfmodex.so'  -d .
unzip -j Geometry-Wars-3-*.apk 'lib/armeabi-v7a/libfmodevent.so' -d .
```

---

## Installing on the device

1. Copy the host binaries into `/roms/ports/gw3/`:
   - `gw3_r36`
   - `libclock_fix.so`
   - `gw3.sh` (to `/roms/ports/`, or bundle it in the port dir)
2. Copy the user-supplied files into the same directory:
   - `main.35.com.activision.gw3.dimensions.obb`
   - `libgwnext.so`, `libfmodex.so`, `libfmodevent.so`
3. Launch. `gw3.sh` checks for the required files, then runs `gw3_r36`.

The log lives at `/roms/ports/gw3/gw3.log` for postmortem debugging.

---

## Hardware / OS target

- **CPU**: ARMv7-A 32-bit (Cortex-A7/A35/A53), NEON, hard-float
- **GPU**: Mali / Panfrost GLES2 — bridged via KMS/DRM + `libGLESv2`
- **Display**: 1280×720 (default; override via `patch_gw3`/config)
- **OS**: ArkOS / NextOS / dArkOS (glibc 2.31+ for the PortMaster build)
- **Audio**: ALSA only (FMOD Android build; OpenSL shim TODO)

---

## Acknowledgements

This port is a fork of the NextOS-style **GTA: Chinatown Wars** R36S shim
(MIT, @mafradon / GTACTW-Port-R36s), which itself builds on the JNI/NV-thread
understanding from the GTA:CTW Vita port (@TheOfficialFloW/gtactw_vita).

Geometry Wars 3: Dimensions is © Activision / Lucid Games. This repository
contains only the port glue and Linux host code — no game assets are bundled.
