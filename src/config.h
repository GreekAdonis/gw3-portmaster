#ifndef CONFIG_H
#define CONFIG_H

/* Geometry Wars 3: Dimensions — native ARMv7 loader (NextOS-style shim) */

/* Render resolution. GW3 is a 1280x720 game; handhelds typically run 640x480
 * or native panel res. Start at 1280x720 (the game's native) and tune per-device.
 * Touch/analog mapping scales automatically. */
#define SCREEN_W    1024
#define SCREEN_H    768

/* Path to extracted game data on the device (APK lib + OBB). */
#define DATA_PATH   "/roms/ports/gw3"
#define SO_PATH     DATA_PATH "/libgwnext.so"

/* The OBB is exposed to the engine at this path. It is a custom packed
 * container (WAD/FSB/BIG magic), not a zip — the engine reads it directly. */
#define OBB_RELPATH "/main.35.com.activision.gw3.dimensions.obb"

/* Device memory in MB (used for the JNI GetDeviceType/GetTotalMemory calls). */
#define DEVICE_MEMORY_MB 512

/* Deadzone for analog sticks (0.0–1.0) */
#define STICK_DEADZONE 0.25f

/* #define DEBUG */

#endif /* CONFIG_H */
