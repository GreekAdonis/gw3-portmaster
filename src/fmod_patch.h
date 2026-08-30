#ifndef FMOD_PATCH_H
#define FMOD_PATCH_H

#include "so_util.h"

/* Audio is stubbed: the game runs silently. Real FMOD (Android libfmodex.so /
 * libfmodevent.so) can't initialise here — it binds an OpenSL ES output via
 * dlopen("libOpenSLES.so"), absent on this device. Instead every FMOD symbol
 * libgwnext.so imports is bound (through main.c's dynlib table) to a stub below
 * that returns FMOD_OK and hands back g_fmod_dummy for object out-parameters.
 *
 * fmod_preload() is now a no-op kept for call-site compatibility. */
int fmod_preload(const char *data_path, so_module *fm, so_module *fme,
                 so_default_dynlib *dynlib, int dynlib_size);

/* Shared non-NULL dummy object returned for every FMOD out-pointer getter. */
extern void *const g_fmod_dummy;

int fmod_stub_ok(void);
int fmod_stub_create(void **out);
int fmod_stub_out1(void *thiz, void **out);
int fmod_stub_out2(void *thiz, void *a1, void **out);
int fmod_stub_out3(void *thiz, void *a1, void *a2, void **out);
int fmod_stub_geti(void *thiz, int *out);
int fmod_stub_getf(void *thiz, float *out);
int fmod_stub_event_getinfo(void *thiz, int *index, char **name, void *info);
int fmod_stub_param_getinfo(void *thiz, int *index, char **name);
int fmod_stub_getspectrum(void *thiz, float *arr, int numvalues, int ch, int win);
int fmod_stub_reverb(void *thiz, void *props);

#endif /* FMOD_PATCH_H */
