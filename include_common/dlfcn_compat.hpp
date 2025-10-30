/*
** EPITECH PROJECT, 2025
** G-CPP-500-BDX-5-1-rtype-1
** File description:
** dlfcn_compat.hpp
*/

#ifndef DLFCN_COMPAT_HPP
#define DLFCN_COMPAT_HPP

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
inline char* dlerror() { return (char*)NULL; }
#else
#include <dlfcn.h>
#endif

#endif // DLFCN_COMPAT_HPP


