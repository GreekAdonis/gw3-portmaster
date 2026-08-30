/* fmod_patch.c -- FMOD handling for the Geometry Wars 3 port.
 *
 * The game links libfmodex.so / libfmodevent.so (Android build). Those bind an
 * OpenSL ES output at init via dlopen("libOpenSLES.so"), which does not exist on
 * this device, so real FMOD cannot initialise. Rather than shim an audio
 * backend, we run the game SILENTLY: every FMOD symbol the engine imports is
 * bound (via the loader's dynlib table) to a stub here that returns FMOD_OK and
 * hands back a shared dummy object for the out-parameter getters. The engine's
 * own Audio/C_AudioSystem bookkeeping stays consistent; nothing ever calls into
 * real FMOD code.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "so_util.h"
#include "fmod_patch.h"

/* One dummy object handed back for every EventSystem/System/Event/EventProject/
 * EventGroup/EventCategory/ChannelGroup/... out-pointer. FMOD's C++ classes are
 * polymorphic and the engine makes *virtual* calls on the objects it gets back
 * (obj->vtable[n](obj, ...)), so the dummy needs a real vtable pointer at
 * offset 0 pointing at a table of no-op functions. The engine pre-zeroes the
 * out-locals it passes to those virtuals, so "return 0 and touch nothing" reads
 * back as "empty project / 0 groups / 0 events" — i.e. silent. */
static int   fmod_vt_noop(void) { return 0; }
static void *fmod_vtable[64];
static void *fmod_dummy[8];                 /* [0] = &fmod_vtable (set at init) */
static char  fmod_empty_str[1] = "";
void *const  g_fmod_dummy = fmod_dummy;

static void fmod_dummy_init(void) {
    for (int i = 0; i < 64; i++) fmod_vtable[i] = (void *)(uintptr_t)fmod_vt_noop;
    fmod_dummy[0] = fmod_vtable;
}

/* FMOD_OK == 0. Generic "did nothing, fine". */
int fmod_stub_ok(void) { return 0; }

/* Object getters: last argument is `Foo **out`. */
int fmod_stub_create(void **out) {                       /* FMOD_EventSystem_Create */
    if (out) *out = fmod_dummy;
    return 0;
}
int fmod_stub_out1(void *thiz, void **out) {
    (void)thiz;
    if (out) *out = fmod_dummy;
    return 0;
}
int fmod_stub_out2(void *thiz, void *a1, void **out) {
    (void)thiz; (void)a1;
    if (out) *out = fmod_dummy;
    return 0;
}
int fmod_stub_out3(void *thiz, void *a1, void *a2, void **out) {
    (void)thiz; (void)a1; (void)a2;
    if (out) *out = fmod_dummy;
    return 0;
}

/* Scalar getters. */
int fmod_stub_geti(void *thiz, int *out)   { (void)thiz; if (out) *out = 0;    return 0; }
int fmod_stub_getf(void *thiz, float *out) { (void)thiz; if (out) *out = 0.0f; return 0; }

/* Event::getInfo(int *index, char **name, FMOD_EVENT_INFO *info) */
int fmod_stub_event_getinfo(void *thiz, int *index, char **name, void *info) {
    (void)thiz;
    if (index) *index = 0;
    if (name)  *name  = fmod_empty_str;
    if (info)  memset(info, 0, 256);
    return 0;
}
/* EventParameter::getInfo(int *index, char **name) */
int fmod_stub_param_getinfo(void *thiz, int *index, char **name) {
    (void)thiz;
    if (index) *index = 0;
    if (name)  *name  = fmod_empty_str;
    return 0;
}
/* ChannelGroup::getSpectrum(float *arr, int numvalues, int ch, window) */
int fmod_stub_getspectrum(void *thiz, float *arr, int numvalues, int ch, int win) {
    (void)thiz; (void)ch; (void)win;
    if (arr && numvalues > 0) memset(arr, 0, (size_t)numvalues * sizeof(float));
    return 0;
}
/* EventSystem::getReverbAmbientProperties(FMOD_REVERB_PROPERTIES *out) */
int fmod_stub_reverb(void *thiz, void *props) {
    (void)thiz;
    if (props) memset(props, 0, 256);
    return 0;
}

int fmod_preload(const char *data_path, so_module *fm, so_module *fme,
                 so_default_dynlib *dynlib, int dynlib_size) {
    (void)data_path; (void)fm; (void)fme; (void)dynlib; (void)dynlib_size;
    /* Real FMOD is never loaded — see file header. All FMOD imports are bound
     * to the stubs above through main.c's dynlib table. */
    fmod_dummy_init();
    fprintf(stderr, "fmod_preload: FMOD stubbed (silent audio)\n");
    return 0;
}
