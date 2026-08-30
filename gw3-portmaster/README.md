# Geometry Wars 3: Dimensions (PortMaster)

A native **32-bit ARM (armhf)** port of the Android release of *Geometry Wars
3: Dimensions*. It runs the original `libgwnext.so` game engine directly on Linux
through a custom Android-on-Linux loader (KMS/DRM video, GLES2, FMOD audio) —
no emulation, no Box86/Box64.

You must own and supply the game's original **OBB** and the engine/FMOD **.so**
files extracted from the APK. None of Activision's copyrighted content is
included in this port.

## Installation

1. Install this port through PortMaster (or copy this folder into your
   `ports/` directory).
2. From your own legally-owned copy, place these files into:
   ```
   <ports>/Geometry Wars 3/gw3/
   ```
   - `main.35.com.activision.gw3.dimensions.obb`  (game data)
   - `libgwnext.so`     (engine, from APK lib/armeabi-v7a/)
   - `libfmodex.so`     (FMOD Ex, from APK lib/armeabi-v7a/)
   - `libfmodevent.so`  (FMOD Event, from APK lib/armeabi-v7a/)
3. Launch the port. The launcher verifies the files are present, then runs
   `gw3_r36` (the engine reads the OBB directly — no unpacking step).

## Status

This is a scaffold port. The loader, AAssetManager shim, FMOD loading, and
GLES2/soft-float thunks are in place, but the engine's JNI method names and
input contract are not yet reverse-engineered, and FMOD's OpenSL output backend
must be shimmed. See the top-level `../README.md` for the open-task list.

## Credits

Forked from the GTA: Chinatown Wars R36S shim (MIT). Geometry Wars 3:
Dimensions © Activision / Lucid Games.
