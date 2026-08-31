# Geometry Wars 3: Dimensions (PortMaster)

A native **32-bit ARM (armhf)** port of the Android release of *Geometry Wars
3: Dimensions*. It runs the original `libgwnext.so` game engine directly on Linux
through a custom Android-on-Linux loader (KMS/DRM video, GLES2, FMOD audio) -
no emulation, no Box86/Box64.

You must own and supply the game's original v1.0.0 **APK** and **OBB**. The launcher
extracts the engine/FMOD `.so` files from the APK automatically. None of
Activision's copyrighted content is included in this port.

## Installation

1. Install this port through PortMaster (or copy this folder into your
   `ports/` directory).
2. From your own legally-owned copy, place these into:
   ```
   <ports>/Geometry Wars 3/gw3/
   ```
   - the game APK - either a `.apk`, or a `.xapk` / `.apkm` bundle (which also
     contains the OBB)
   - `main.35.com.activision.gw3.dimensions.obb`  (game data)
3. Launch the port. The launcher auto-extracts `libgwnext.so`, `libfmodex.so`
   and `libfmodevent.so` from the APK on first run, verifies the files are
   present, then runs `gw3` (the engine reads the OBB directly - no unpacking
   step).
