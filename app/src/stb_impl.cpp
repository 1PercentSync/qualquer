/**
 * @file stb_impl.cpp
 * @brief App layer — stb single-header library implementation sink.
 *
 * Concentrates the stb_image / stb_image_resize2 implementations in one
 * translation unit so that editing texture.cpp does not recompile them, and so
 * no other .cpp can accidentally double-define the implementations.
 */

#define STB_IMAGE_IMPLEMENTATION
// ReSharper disable once CppUnusedIncludeDirective
#include <stb_image.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
// ReSharper disable once CppUnusedIncludeDirective
#include <stb_image_resize2.h>
