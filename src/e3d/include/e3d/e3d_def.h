#pragma once

#if defined(_WIN32) && defined(e3d_EXPORTS)
#define E3D_EXPORT __declspec(dllexport)
#elif defined(__GNUC__) && defined(e3d_EXPORTS)
#define E3D_EXPORT __attribute__(visibility "default")
#else
#define E3D_EXPORT
#endif