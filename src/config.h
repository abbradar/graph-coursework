#ifndef GRAPH_CONFIG_H_
#define GRAPH_CONFIG_H_

//#define USE_EIGEN
#define NO_GRAB_INPUT
//#define NO_NORMAL_FACE_CLIPPING
#define NO_DRAW_SURFACE
#define AFFINE_TEXTURES
#define COMPILE_TIME_BPP
//#define MULTITHREADED_Z_BUFFER

#define FLAT_SHADING
#define GOURAUD_SHADING
#define PHONG_SHADING

constexpr unsigned kDefaultBpp = 32;

//

#if !(defined(FLAT_SHADING) || defined(GOURAUD_SHADING) || defined(PHONG_SHADING))
#define NO_SHADING
#endif

#endif // GRAPH_CONFIG_H_
