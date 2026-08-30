#!/bin/bash
# Geometry Wars 3: Dimensions launcher for R36S / ArkOS / NextOS (PortMaster-compatible)

CURR_TTY="/dev/tty1"
XDG_DATA_HOME=${XDG_DATA_HOME:-$HOME/.local/share}

# PortMaster integration
for pmdir in "/opt/system/Tools/PortMaster" "/opt/tools/PortMaster" \
             "$XDG_DATA_HOME/PortMaster" "/roms/ports/PortMaster"; do
  if [ -f "$pmdir/control.txt" ]; then
    source "$pmdir/control.txt"
    source "$pmdir/device_info.txt" 2>/dev/null || true
    get_controls 2>/dev/null || true
    break
  fi
done

# Fallbacks when running outside PortMaster (direct terminal / SSH)
ESUDO="${ESUDO:-}"
sdl_controllerconfig="${sdl_controllerconfig:-}"
pm_finish() { :; }

GAMEDIR="/roms/ports/gw3"
cd "$GAMEDIR" || exit 1

# Truncate the log (we'll append to it later, via tee).
> "$GAMEDIR/gw3.log"

# ── TTY ownership & EmulationStation shutdown ──────────────────────────────
$ESUDO chmod 666 $CURR_TTY            2>/dev/null
$ESUDO chmod 666 /dev/uinput          2>/dev/null
$ESUDO chmod 666 /dev/dri/card0       2>/dev/null
$ESUDO chmod 666 /dev/dri/renderD128  2>/dev/null

$ESUDO pkill -TERM emulationstation 2>/dev/null || true
sleep 1
$ESUDO pkill -9    emulationstation 2>/dev/null || true

printf "\033c"   > $CURR_TTY    # clear
printf "\e[?25l" > $CURR_TTY    # hide cursor

# ── First-run check ───────────────────────────────────────────────────────
# Geometry Wars 3 needs: the OBB (game data), the engine .so, and the FMOD libs.
OBB_FILE=$(ls "$GAMEDIR"/*.obb 2>/dev/null | head -1)
if [ -z "$OBB_FILE" ] || [ ! -f "$GAMEDIR/libgwnext.so" ] \
   || [ ! -f "$GAMEDIR/libfmodex.so" ] || [ ! -f "$GAMEDIR/libfmodevent.so" ]; then
    exec > >(tee -a "$GAMEDIR/gw3.log" > $CURR_TTY) 2>&1
    echo ""
    echo "============================================================"
    echo "  Geometry Wars 3 — INSTALLATION INCOMPLETE"
    echo "============================================================"
    echo ""
    echo "Place these files in:  $GAMEDIR/"
    echo ""
    echo "  - main.35.com.activision.gw3.dimensions.obb  (game data)"
    echo "  - libgwnext.so     (extracted from the APK: lib/armeabi-v7a/)"
    echo "  - libfmodex.so     (extracted from the APK: lib/armeabi-v7a/)"
    echo "  - libfmodevent.so  (extracted from the APK: lib/armeabi-v7a/)"
    echo ""
    echo "Status:"
    [ -n "$OBB_FILE" ] && echo "  [OK]      OBB found: $(basename "$OBB_FILE")" \
                       || echo "  [MISSING] No *.obb file found"
    [ -f "$GAMEDIR/libgwnext.so" ]    && echo "  [OK]      libgwnext.so"    || echo "  [MISSING] libgwnext.so"
    [ -f "$GAMEDIR/libfmodex.so" ]    && echo "  [OK]      libfmodex.so"    || echo "  [MISSING] libfmodex.so"
    [ -f "$GAMEDIR/libfmodevent.so" ] && echo "  [OK]      libfmodevent.so" || echo "  [MISSING] libfmodevent.so"
    echo ""
    echo "Returning to menu in 20 seconds..."
    sleep 20
    $ESUDO systemctl start emulationstation 2>/dev/null || true
    pm_finish
    exit 1
fi

# From here on (game run): log only, no more TTY output (game owns the FB).
exec > >(tee -a "$GAMEDIR/gw3.log") 2>&1

# Release VT console framebuffer so KMS/DRM is free for the game
echo 0 | $ESUDO tee /sys/class/vtconsole/vtcon0/bind > /dev/null 2>&1 || true
echo 0 | $ESUDO tee /sys/class/vtconsole/vtcon1/bind > /dev/null 2>&1 || true

# Video
export SDL_VIDEODRIVER=kmsdrm
export SDL_VIDEO_GL_DRIVER=libGLESv2.so
export SDL_VIDEO_EGL_DRIVER=libEGL.so

# Audio — ALSA only on ArkOS (FMOD Android build will try OpenSL and fall back)
export SDL_AUDIODRIVER=alsa
export AUDIODEV=default

# Controller
export SDL_GAMECONTROLLERCONFIG="$sdl_controllerconfig"

# Libraries
export LD_LIBRARY_PATH="/usr/lib/arm-linux-gnueabihf:$LD_LIBRARY_PATH"

# Preload our __clock_gettime64 override (bionic/old-glibc time64 mismatch)
export LD_PRELOAD="$GAMEDIR/libclock_fix.so"

./gw3_r36

# Restore VT console and restart frontend
echo 1 | $ESUDO tee /sys/class/vtconsole/vtcon0/bind > /dev/null 2>&1 || true
echo 1 | $ESUDO tee /sys/class/vtconsole/vtcon1/bind > /dev/null 2>&1 || true
printf "\033c" > $CURR_TTY
printf "\e[?25h" > $CURR_TTY

$ESUDO systemctl start emulationstation 2>/dev/null || \
  $ESUDO systemctl restart oga_events 2>/dev/null || true

pm_finish
