/*
** EPITECH PROJECT, 2025
** R-TYPE
** File description:
** dlfcn_compat
*/

#pragma once

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#ifndef RTLD_LAZY
#define RTLD_LAZY 0
#endif
inline void* dlopen(const char* p, int) { return (void*)LoadLibraryA(p); }
inline void* dlsym(void* h, const char* s) { return (void*)GetProcAddress((HMODULE)h, s); }
inline int dlclose(void* h) { return FreeLibrary((HMODULE)h) ? 0 : -1; }
inline char* dlerror() { return (char*)"dlerror not available on Windows"; }
#else
#include <dlfcn.h>
#endif

