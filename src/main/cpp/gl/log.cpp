//
// Created by BZLZHH on 2025/1/26.
//

#include "log.h"
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>
#include <mutex>

#include "GL/gl.h"

#ifndef __ANDROID__
// Define a stub for __android_log_print if not on Android
int __android_log_print(int prio, const char *tag,  const char *fmt, ...) {
    return 0; // Do nothing
}
#endif

#define CASE(e) \
    case e: return #e;

const char* glEnumToString(GLenum e) {
    static char str[128];
    switch (e) {
/* Boolean values */

/* Data types */
        CASE(GL_BYTE)
        CASE(GL_UNSIGNED_BYTE)
        CASE(GL_SHORT)
        CASE(GL_UNSIGNED_SHORT)
        CASE(GL_INT)
        CASE(GL_UNSIGNED_INT)
        CASE(GL_FLOAT)
        CASE(GL_2_BYTES)
        CASE(GL_3_BYTES)
        CASE(GL_4_BYTES)
        CASE(GL_DOUBLE)

        CASE(GL_UNSIGNED_BYTE_3_3_2)
        CASE(GL_UNSIGNED_BYTE_2_3_3_REV)
        CASE(GL_UNSIGNED_SHORT_5_6_5)
        CASE(GL_UNSIGNED_SHORT_5_6_5_REV)
        CASE(GL_UNSIGNED_SHORT_4_4_4_4)
        CASE(GL_UNSIGNED_SHORT_4_4_4_4_REV)
        CASE(GL_UNSIGNED_SHORT_5_5_5_1)
        CASE(GL_UNSIGNED_SHORT_1_5_5_5_REV)
        CASE(GL_UNSIGNED_INT_8_8_8_8)
        CASE(GL_UNSIGNED_INT_8_8_8_8_REV)
        CASE(GL_UNSIGNED_INT_10_10_10_2)

/* Primitives */
        CASE(GL_LINE_LOOP)
        CASE(GL_LINE_STRIP)
        CASE(GL_TRIANGLES)
        CASE(GL_TRIANGLE_STRIP)
        CASE(GL_TRIANGLE_FAN)
        CASE(GL_QUADS)
        CASE(GL_QUAD_STRIP)
        CASE(GL_POLYGON)

/* Vertex Arrays */
        CASE(GL_VERTEX_ARRAY)
        CASE(GL_NORMAL_ARRAY)
        CASE(GL_COLOR_ARRAY)
        CASE(GL_INDEX_ARRAY)
        CASE(GL_TEXTURE_COORD_ARRAY)
        CASE(GL_EDGE_FLAG_ARRAY)
        CASE(GL_VERTEX_ARRAY_SIZE)
        CASE(GL_VERTEX_ARRAY_TYPE)
        CASE(GL_VERTEX_ARRAY_STRIDE)
        CASE(GL_NORMAL_ARRAY_TYPE)
        CASE(GL_NORMAL_ARRAY_STRIDE)
        CASE(GL_COLOR_ARRAY_SIZE)
        CASE(GL_COLOR_ARRAY_TYPE)
        CASE(GL_COLOR_ARRAY_STRIDE)
        CASE(GL_INDEX_ARRAY_TYPE)
        CASE(GL_INDEX_ARRAY_STRIDE)
        CASE(GL_TEXTURE_COORD_ARRAY_SIZE)
        CASE(GL_TEXTURE_COORD_ARRAY_TYPE)
        CASE(GL_TEXTURE_COORD_ARRAY_STRIDE)
        CASE(GL_EDGE_FLAG_ARRAY_STRIDE)
        CASE(GL_VERTEX_ARRAY_POINTER)
        CASE(GL_NORMAL_ARRAY_POINTER)
        CASE(GL_COLOR_ARRAY_POINTER)
        CASE(GL_INDEX_ARRAY_POINTER)
        CASE(GL_TEXTURE_COORD_ARRAY_POINTER)
        CASE(GL_EDGE_FLAG_ARRAY_POINTER)
        CASE(GL_V2F)
        CASE(GL_V3F)
        CASE(GL_C4UB_V2F)
        CASE(GL_C4UB_V3F)
        CASE(GL_C3F_V3F)
        CASE(GL_N3F_V3F)
        CASE(GL_C4F_N3F_V3F)
        CASE(GL_T2F_V3F)
        CASE(GL_T4F_V4F)
        CASE(GL_T2F_C4UB_V3F)
        CASE(GL_T2F_C3F_V3F)
        CASE(GL_T2F_N3F_V3F)
        CASE(GL_T2F_C4F_N3F_V3F)
        CASE(GL_T4F_C4F_N3F_V4F)

/* Matrix Mode */
        CASE(GL_MATRIX_MODE)
        CASE(GL_MODELVIEW)
        CASE(GL_PROJECTION)
        CASE(GL_TEXTURE)

/* Points */
        CASE(GL_POINT_SMOOTH)
        CASE(GL_POINT_SIZE)
        CASE(GL_POINT_SIZE_GRANULARITY)
        CASE(GL_POINT_SIZE_RANGE)

/* Lines */
        CASE(GL_LINE_SMOOTH)
        CASE(GL_LINE_STIPPLE)
        CASE(GL_LINE_STIPPLE_PATTERN)
        CASE(GL_LINE_STIPPLE_REPEAT)
        CASE(GL_LINE_WIDTH)
        CASE(GL_LINE_WIDTH_GRANULARITY)
        CASE(GL_LINE_WIDTH_RANGE)

/* Polygons */
        CASE(GL_POINT)
        CASE(GL_LINE)
        CASE(GL_FILL)
        CASE(GL_CW)
        CASE(GL_CCW)
        CASE(GL_FRONT)
        CASE(GL_BACK)
        CASE(GL_POLYGON_MODE)
        CASE(GL_POLYGON_SMOOTH)
        CASE(GL_POLYGON_STIPPLE)
        CASE(GL_EDGE_FLAG)
        CASE(GL_CULL_FACE)
        CASE(GL_CULL_FACE_MODE)
        CASE(GL_FRONT_FACE)
        CASE(GL_POLYGON_OFFSET_FACTOR)
        CASE(GL_POLYGON_OFFSET_UNITS)
        CASE(GL_POLYGON_OFFSET_POINT)
        CASE(GL_POLYGON_OFFSET_LINE)
        CASE(GL_POLYGON_OFFSET_FILL)

/* Display Lists */
        CASE(GL_COMPILE)
        CASE(GL_COMPILE_AND_EXECUTE)
        CASE(GL_LIST_BASE)
        CASE(GL_LIST_INDEX)
        CASE(GL_LIST_MODE)

/* Depth buffer */
        CASE(GL_NEVER)
        CASE(GL_LESS)
        CASE(GL_EQUAL)
        CASE(GL_LEQUAL)
        CASE(GL_GREATER)
        CASE(GL_NOTEQUAL)
        CASE(GL_GEQUAL)
        CASE(GL_ALWAYS)
        CASE(GL_DEPTH_TEST)
        CASE(GL_DEPTH_BITS)
        CASE(GL_DEPTH_CLEAR_VALUE)
        CASE(GL_DEPTH_FUNC)
        CASE(GL_DEPTH_RANGE)
        CASE(GL_DEPTH_WRITEMASK)
        CASE(GL_DEPTH_COMPONENT)

/* Lighting */
        CASE(GL_LIGHTING)
        CASE(GL_LIGHT0)
        CASE(GL_LIGHT1)
        CASE(GL_LIGHT2)
        CASE(GL_LIGHT3)
        CASE(GL_LIGHT4)
        CASE(GL_LIGHT5)
        CASE(GL_LIGHT6)
        CASE(GL_LIGHT7)
        CASE(GL_SPOT_EXPONENT)
        CASE(GL_SPOT_CUTOFF)
        CASE(GL_CONSTANT_ATTENUATION)
        CASE(GL_LINEAR_ATTENUATION)
        CASE(GL_QUADRATIC_ATTENUATION)
        CASE(GL_AMBIENT)
        CASE(GL_DIFFUSE)
        CASE(GL_SPECULAR)
        CASE(GL_SHININESS)
        CASE(GL_EMISSION)
        CASE(GL_POSITION)
        CASE(GL_SPOT_DIRECTION)
        CASE(GL_AMBIENT_AND_DIFFUSE)
        CASE(GL_COLOR_INDEXES)
        CASE(GL_LIGHT_MODEL_TWO_SIDE)
        CASE(GL_LIGHT_MODEL_LOCAL_VIEWER)
        CASE(GL_LIGHT_MODEL_AMBIENT)
        CASE(GL_FRONT_AND_BACK)
        CASE(GL_SHADE_MODEL)
        CASE(GL_FLAT)
        CASE(GL_SMOOTH)
        CASE(GL_COLOR_MATERIAL)
        CASE(GL_COLOR_MATERIAL_FACE)
        CASE(GL_COLOR_MATERIAL_PARAMETER)
        CASE(GL_NORMALIZE)

/* User clipping planes */
        CASE(GL_CLIP_PLANE0)
        CASE(GL_CLIP_PLANE1)
        CASE(GL_CLIP_PLANE2)
        CASE(GL_CLIP_PLANE3)
        CASE(GL_CLIP_PLANE4)
        CASE(GL_CLIP_PLANE5)

/* Accumulation buffer */
        CASE(GL_ACCUM_RED_BITS)
        CASE(GL_ACCUM_GREEN_BITS)
        CASE(GL_ACCUM_BLUE_BITS)
        CASE(GL_ACCUM_ALPHA_BITS)
        CASE(GL_ACCUM_CLEAR_VALUE)
        CASE(GL_ACCUM)
        CASE(GL_ADD)
        CASE(GL_LOAD)
        CASE(GL_MULT)
        CASE(GL_RETURN)

/* Alpha testing */
        CASE(GL_ALPHA_TEST)
        CASE(GL_ALPHA_TEST_REF)
        CASE(GL_ALPHA_TEST_FUNC)

/* Blending */
        CASE(GL_BLEND)
        CASE(GL_BLEND_SRC)
        CASE(GL_BLEND_DST)
        CASE(GL_ZERO)
        CASE(GL_ONE)
        CASE(GL_SRC_COLOR)
        CASE(GL_ONE_MINUS_SRC_COLOR)
        CASE(GL_SRC_ALPHA)
        CASE(GL_ONE_MINUS_SRC_ALPHA)
        CASE(GL_DST_ALPHA)
        CASE(GL_ONE_MINUS_DST_ALPHA)
        CASE(GL_DST_COLOR)
        CASE(GL_ONE_MINUS_DST_COLOR)
        CASE(GL_SRC_ALPHA_SATURATE)

/* Render Mode */
        CASE(GL_FEEDBACK)
        CASE(GL_RENDER)
        CASE(GL_SELECT)

/* Feedback */
        CASE(GL_2D)
        CASE(GL_3D)
        CASE(GL_3D_COLOR)
        CASE(GL_3D_COLOR_TEXTURE)
        CASE(GL_4D_COLOR_TEXTURE)
        CASE(GL_POINT_TOKEN)
        CASE(GL_LINE_TOKEN)
        CASE(GL_LINE_RESET_TOKEN)
        CASE(GL_POLYGON_TOKEN)
        CASE(GL_BITMAP_TOKEN)
        CASE(GL_DRAW_PIXEL_TOKEN)
        CASE(GL_COPY_PIXEL_TOKEN)
        CASE(GL_PASS_THROUGH_TOKEN)
        CASE(GL_FEEDBACK_BUFFER_POINTER)
        CASE(GL_FEEDBACK_BUFFER_SIZE)
        CASE(GL_FEEDBACK_BUFFER_TYPE)

/* Selection */
        CASE(GL_SELECTION_BUFFER_POINTER)
        CASE(GL_SELECTION_BUFFER_SIZE)

/* Fog */
        CASE(GL_FOG)
        CASE(GL_FOG_MODE)
        CASE(GL_FOG_DENSITY)
        CASE(GL_FOG_COLOR)
        CASE(GL_FOG_INDEX)
        CASE(GL_FOG_START)
        CASE(GL_FOG_END)
        CASE(GL_LINEAR)
        CASE(GL_EXP)
        CASE(GL_EXP2)

/* Logic Ops */
        CASE(GL_INDEX_LOGIC_OP)
        CASE(GL_COLOR_LOGIC_OP)
        CASE(GL_LOGIC_OP_MODE)
        CASE(GL_CLEAR)
        CASE(GL_SET)
        CASE(GL_COPY)
        CASE(GL_COPY_INVERTED)
        CASE(GL_NOOP)
        CASE(GL_INVERT)
        CASE(GL_AND)
        CASE(GL_NAND)
        CASE(GL_OR)
        CASE(GL_NOR)
        CASE(GL_XOR)
        CASE(GL_EQUIV)
        CASE(GL_AND_REVERSE)
        CASE(GL_AND_INVERTED)
        CASE(GL_OR_REVERSE)
        CASE(GL_OR_INVERTED)

/* Stencil */
        CASE(GL_STENCIL_BITS)
        CASE(GL_STENCIL_TEST)
        CASE(GL_STENCIL_CLEAR_VALUE)
        CASE(GL_STENCIL_FUNC)
        CASE(GL_STENCIL_VALUE_MASK)
        CASE(GL_STENCIL_FAIL)
        CASE(GL_STENCIL_PASS_DEPTH_FAIL)
        CASE(GL_STENCIL_PASS_DEPTH_PASS)
        CASE(GL_STENCIL_REF)
        CASE(GL_STENCIL_WRITEMASK)
        CASE(GL_STENCIL_INDEX)
        CASE(GL_KEEP)
        CASE(GL_REPLACE)
        CASE(GL_INCR)
        CASE(GL_DECR)

/* Buffers, Pixel Drawing/Reading */
        CASE(GL_LEFT)
        CASE(GL_RIGHT)
        CASE(GL_FRONT_LEFT)
        CASE(GL_FRONT_RIGHT)
        CASE(GL_BACK_LEFT)
        CASE(GL_BACK_RIGHT)
        CASE(GL_AUX0)
        CASE(GL_AUX1)
        CASE(GL_AUX2)
        CASE(GL_AUX3)
        CASE(GL_COLOR_INDEX)
        CASE(GL_RED)
        CASE(GL_GREEN)
        CASE(GL_BLUE)
        CASE(GL_ALPHA)
        CASE(GL_LUMINANCE)
        CASE(GL_LUMINANCE_ALPHA)
        CASE(GL_ALPHA_BITS)
        CASE(GL_RED_BITS)
        CASE(GL_GREEN_BITS)
        CASE(GL_BLUE_BITS)
        CASE(GL_INDEX_BITS)
        CASE(GL_SUBPIXEL_BITS)
        CASE(GL_AUX_BUFFERS)
        CASE(GL_READ_BUFFER)
        CASE(GL_DRAW_BUFFER)
        CASE(GL_DOUBLEBUFFER)
        CASE(GL_STEREO)
        CASE(GL_BITMAP)
        CASE(GL_COLOR)
        CASE(GL_DEPTH)
        CASE(GL_STENCIL)
        CASE(GL_DITHER)
        CASE(GL_RGB)
        CASE(GL_RGBA)

/* Implementation limits */
        CASE(GL_MAX_LIST_NESTING)
        CASE(GL_MAX_EVAL_ORDER)
        CASE(GL_MAX_LIGHTS)
        CASE(GL_MAX_CLIP_PLANES)
        CASE(GL_MAX_TEXTURE_SIZE)
        CASE(GL_MAX_PIXEL_MAP_TABLE)
        CASE(GL_MAX_ATTRIB_STACK_DEPTH)
        CASE(GL_MAX_MODELVIEW_STACK_DEPTH)
        CASE(GL_MAX_NAME_STACK_DEPTH)
        CASE(GL_MAX_PROJECTION_STACK_DEPTH)
        CASE(GL_MAX_TEXTURE_STACK_DEPTH)
        CASE(GL_MAX_VIEWPORT_DIMS)
        CASE(GL_MAX_CLIENT_ATTRIB_STACK_DEPTH)

/* Gets */
        CASE(GL_ATTRIB_STACK_DEPTH)
        CASE(GL_CLIENT_ATTRIB_STACK_DEPTH)
        CASE(GL_COLOR_CLEAR_VALUE)
        CASE(GL_COLOR_WRITEMASK)
        CASE(GL_CURRENT_INDEX)
        CASE(GL_CURRENT_COLOR)
        CASE(GL_CURRENT_NORMAL)
        CASE(GL_CURRENT_RASTER_COLOR)
        CASE(GL_CURRENT_RASTER_DISTANCE)
        CASE(GL_CURRENT_RASTER_INDEX)
        CASE(GL_CURRENT_RASTER_POSITION)
        CASE(GL_CURRENT_RASTER_TEXTURE_COORDS)
        CASE(GL_CURRENT_RASTER_POSITION_VALID)
        CASE(GL_CURRENT_TEXTURE_COORDS)
        CASE(GL_INDEX_CLEAR_VALUE)
        CASE(GL_INDEX_MODE)
        CASE(GL_INDEX_WRITEMASK)
        CASE(GL_MODELVIEW_MATRIX)
        CASE(GL_MODELVIEW_STACK_DEPTH)
        CASE(GL_NAME_STACK_DEPTH)
        CASE(GL_PROJECTION_MATRIX)
        CASE(GL_PROJECTION_STACK_DEPTH)
        CASE(GL_RENDER_MODE)
        CASE(GL_RGBA_MODE)
        CASE(GL_TEXTURE_MATRIX)
        CASE(GL_TEXTURE_STACK_DEPTH)
        CASE(GL_VIEWPORT)

/* Evaluators */
        CASE(GL_AUTO_NORMAL)
        CASE(GL_MAP1_COLOR_4)
        CASE(GL_MAP1_INDEX)
        CASE(GL_MAP1_NORMAL)
        CASE(GL_MAP1_TEXTURE_COORD_1)
        CASE(GL_MAP1_TEXTURE_COORD_2)
        CASE(GL_MAP1_TEXTURE_COORD_3)
        CASE(GL_MAP1_TEXTURE_COORD_4)
        CASE(GL_MAP1_VERTEX_3)
        CASE(GL_MAP1_VERTEX_4)
        CASE(GL_MAP2_COLOR_4)
        CASE(GL_MAP2_INDEX)
        CASE(GL_MAP2_NORMAL)
        CASE(GL_MAP2_TEXTURE_COORD_1)
        CASE(GL_MAP2_TEXTURE_COORD_2)
        CASE(GL_MAP2_TEXTURE_COORD_3)
        CASE(GL_MAP2_TEXTURE_COORD_4)
        CASE(GL_MAP2_VERTEX_3)
        CASE(GL_MAP2_VERTEX_4)
        CASE(GL_MAP1_GRID_DOMAIN)
        CASE(GL_MAP1_GRID_SEGMENTS)
        CASE(GL_MAP2_GRID_DOMAIN)
        CASE(GL_MAP2_GRID_SEGMENTS)
        CASE(GL_COEFF)
        CASE(GL_ORDER)
        CASE(GL_DOMAIN)

/* Hints */
        CASE(GL_PERSPECTIVE_CORRECTION_HINT)
        CASE(GL_POINT_SMOOTH_HINT)
        CASE(GL_LINE_SMOOTH_HINT)
        CASE(GL_POLYGON_SMOOTH_HINT)
        CASE(GL_FOG_HINT)
        CASE(GL_DONT_CARE)
        CASE(GL_FASTEST)
        CASE(GL_NICEST)

/* Scissor box */
        CASE(GL_SCISSOR_BOX)
        CASE(GL_SCISSOR_TEST)

/* Pixel Mode / Transfer */
        CASE(GL_MAP_COLOR)
        CASE(GL_MAP_STENCIL)
        CASE(GL_INDEX_SHIFT)
        CASE(GL_INDEX_OFFSET)
        CASE(GL_RED_SCALE)
        CASE(GL_RED_BIAS)
        CASE(GL_GREEN_SCALE)
        CASE(GL_GREEN_BIAS)
        CASE(GL_BLUE_SCALE)
        CASE(GL_BLUE_BIAS)
        CASE(GL_ALPHA_SCALE)
        CASE(GL_ALPHA_BIAS)
        CASE(GL_DEPTH_SCALE)
        CASE(GL_DEPTH_BIAS)
        CASE(GL_PIXEL_MAP_S_TO_S_SIZE)
        CASE(GL_PIXEL_MAP_I_TO_I_SIZE)
        CASE(GL_PIXEL_MAP_I_TO_R_SIZE)
        CASE(GL_PIXEL_MAP_I_TO_G_SIZE)
        CASE(GL_PIXEL_MAP_I_TO_B_SIZE)
        CASE(GL_PIXEL_MAP_I_TO_A_SIZE)
        CASE(GL_PIXEL_MAP_R_TO_R_SIZE)
        CASE(GL_PIXEL_MAP_G_TO_G_SIZE)
        CASE(GL_PIXEL_MAP_B_TO_B_SIZE)
        CASE(GL_PIXEL_MAP_A_TO_A_SIZE)
        CASE(GL_PIXEL_MAP_S_TO_S)
        CASE(GL_PIXEL_MAP_I_TO_I)
        CASE(GL_PIXEL_MAP_I_TO_R)
        CASE(GL_PIXEL_MAP_I_TO_G)
        CASE(GL_PIXEL_MAP_I_TO_B)
        CASE(GL_PIXEL_MAP_I_TO_A)
        CASE(GL_PIXEL_MAP_R_TO_R)
        CASE(GL_PIXEL_MAP_G_TO_G)
        CASE(GL_PIXEL_MAP_B_TO_B)
        CASE(GL_PIXEL_MAP_A_TO_A)
        CASE(GL_PACK_ALIGNMENT)
        CASE(GL_PACK_LSB_FIRST)
        CASE(GL_PACK_ROW_LENGTH)
        CASE(GL_PACK_SKIP_PIXELS)
        CASE(GL_PACK_SKIP_ROWS)
        CASE(GL_PACK_SWAP_BYTES)
        CASE(GL_UNPACK_ALIGNMENT)
        CASE(GL_UNPACK_LSB_FIRST)
        CASE(GL_UNPACK_ROW_LENGTH)
        CASE(GL_UNPACK_SKIP_PIXELS)
        CASE(GL_UNPACK_SKIP_ROWS)
        CASE(GL_UNPACK_SWAP_BYTES)
        CASE(GL_ZOOM_X)
        CASE(GL_ZOOM_Y)

/* Texture mapping */
        CASE(GL_TEXTURE_ENV)
        CASE(GL_TEXTURE_ENV_MODE)
        CASE(GL_TEXTURE_1D)
        CASE(GL_TEXTURE_2D)
        CASE(GL_TEXTURE_WRAP_S)
        CASE(GL_TEXTURE_WRAP_T)
        CASE(GL_TEXTURE_MAG_FILTER)
        CASE(GL_TEXTURE_MIN_FILTER)
        CASE(GL_TEXTURE_ENV_COLOR)
        CASE(GL_TEXTURE_GEN_S)
        CASE(GL_TEXTURE_GEN_T)
        CASE(GL_TEXTURE_GEN_R)
        CASE(GL_TEXTURE_GEN_Q)
        CASE(GL_TEXTURE_GEN_MODE)
        CASE(GL_TEXTURE_BORDER_COLOR)
        CASE(GL_TEXTURE_WIDTH)
        CASE(GL_TEXTURE_HEIGHT)
        CASE(GL_TEXTURE_BORDER)
        CASE(GL_TEXTURE_COMPONENTS)
        CASE(GL_TEXTURE_RED_SIZE)
        CASE(GL_TEXTURE_GREEN_SIZE)
        CASE(GL_TEXTURE_BLUE_SIZE)
        CASE(GL_TEXTURE_ALPHA_SIZE)
        CASE(GL_TEXTURE_LUMINANCE_SIZE)
        CASE(GL_TEXTURE_INTENSITY_SIZE)
        CASE(GL_NEAREST_MIPMAP_NEAREST)
        CASE(GL_NEAREST_MIPMAP_LINEAR)
        CASE(GL_LINEAR_MIPMAP_NEAREST)
        CASE(GL_LINEAR_MIPMAP_LINEAR)
        CASE(GL_OBJECT_LINEAR)
        CASE(GL_OBJECT_PLANE)
        CASE(GL_EYE_LINEAR)
        CASE(GL_EYE_PLANE)
        CASE(GL_SPHERE_MAP)
        CASE(GL_DECAL)
        CASE(GL_MODULATE)
        CASE(GL_NEAREST)
        CASE(GL_REPEAT)
        CASE(GL_CLAMP)
        CASE(GL_S)
        CASE(GL_T)
        CASE(GL_R)
        CASE(GL_Q)

/* Utility */
        CASE(GL_VENDOR)
        CASE(GL_RENDERER)
        CASE(GL_VERSION)
        CASE(GL_EXTENSIONS)

/* Errors */
        CASE(GL_INVALID_ENUM)
        CASE(GL_INVALID_VALUE)
        CASE(GL_INVALID_OPERATION)
        CASE(GL_STACK_OVERFLOW)
        CASE(GL_STACK_UNDERFLOW)
        CASE(GL_OUT_OF_MEMORY)


/* OpenGL 1.1 */
        CASE(GL_PROXY_TEXTURE_1D)
        CASE(GL_PROXY_TEXTURE_2D)
        CASE(GL_TEXTURE_PRIORITY)
        CASE(GL_TEXTURE_RESIDENT)
        CASE(GL_TEXTURE_BINDING_1D)
        CASE(GL_TEXTURE_BINDING_2D)
        CASE(GL_ALPHA4)
        CASE(GL_ALPHA8)
        CASE(GL_ALPHA12)
        CASE(GL_ALPHA16)
        CASE(GL_LUMINANCE4)
        CASE(GL_LUMINANCE8)
        CASE(GL_LUMINANCE12)
        CASE(GL_LUMINANCE16)
        CASE(GL_LUMINANCE4_ALPHA4)
        CASE(GL_LUMINANCE6_ALPHA2)
        CASE(GL_LUMINANCE8_ALPHA8)
        CASE(GL_LUMINANCE12_ALPHA4)
        CASE(GL_LUMINANCE12_ALPHA12)
        CASE(GL_LUMINANCE16_ALPHA16)
        CASE(GL_INTENSITY)
        CASE(GL_INTENSITY4)
        CASE(GL_INTENSITY8)
        CASE(GL_INTENSITY12)
        CASE(GL_INTENSITY16)
        CASE(GL_R3_G3_B2)
        CASE(GL_RGB4)
        CASE(GL_RGB5)
        CASE(GL_RGB8)
        CASE(GL_RGB10)
        CASE(GL_RGB12)
        CASE(GL_RGB16)
        CASE(GL_RGBA2)
        CASE(GL_RGBA4)
        CASE(GL_RGB5_A1)
        CASE(GL_RGBA8)
        CASE(GL_RGB10_A2)
        CASE(GL_RGBA12)
        CASE(GL_RGBA16)

        /* Buffers Array */
        CASE(GL_BUFFER_SIZE)
        CASE(GL_BUFFER_USAGE)
        CASE(GL_QUERY_COUNTER_BITS)
        CASE(GL_CURRENT_QUERY)
        CASE(GL_QUERY_RESULT)
        CASE(GL_QUERY_RESULT_AVAILABLE)
        CASE(GL_ARRAY_BUFFER)
        CASE(GL_ELEMENT_ARRAY_BUFFER)
        CASE(GL_ARRAY_BUFFER_BINDING)
        CASE(GL_ELEMENT_ARRAY_BUFFER_BINDING)
        CASE(GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING)
        CASE(GL_READ_ONLY)
        CASE(GL_WRITE_ONLY)
        CASE(GL_READ_WRITE)
        CASE(GL_BUFFER_ACCESS)
        CASE(GL_BUFFER_MAPPED)
        CASE(GL_BUFFER_MAP_POINTER)
        CASE(GL_STREAM_DRAW)
        CASE(GL_STREAM_READ)
        CASE(GL_STREAM_COPY)
        CASE(GL_STATIC_DRAW)
        CASE(GL_STATIC_READ)
        CASE(GL_STATIC_COPY)
        CASE(GL_DYNAMIC_DRAW)
        CASE(GL_DYNAMIC_READ)
        CASE(GL_DYNAMIC_COPY)
        CASE(GL_VERTEX_ARRAY_BUFFER_BINDING)
        CASE(GL_NORMAL_ARRAY_BUFFER_BINDING)
        CASE(GL_COLOR_ARRAY_BUFFER_BINDING)
        CASE(GL_INDEX_ARRAY_BUFFER_BINDING)
        CASE(GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING)
        CASE(GL_EDGE_FLAG_ARRAY_BUFFER_BINDING)
        CASE(GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING)
        CASE(GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING)
        CASE(GL_WEIGHT_ARRAY_BUFFER_BINDING)
//        CASE(GL_MAP_READ_BIT)
//        CASE(GL_MAP_WRITE_BIT)
        CASE(GL_BUFFER_ACCESS_FLAGS)
        CASE(GL_BUFFER_MAP_LENGTH)
        CASE(GL_BUFFER_MAP_OFFSET)
//        CASE(GL_READ_ONLY)
//        CASE(GL_WRITE_ONLY)
//        CASE(GL_READ_WRITE)
        CASE(GL_PIXEL_PACK_BUFFER)
        CASE(GL_PIXEL_UNPACK_BUFFER)
        CASE(GL_PIXEL_UNPACK_BUFFER_BINDING)
        CASE(GL_PIXEL_PACK_BUFFER_BINDING)
        CASE(GL_CURRENT_VERTEX_ATTRIB)
        CASE(GL_MAP_PERSISTENT_BIT)
        CASE(GL_QUERY_BUFFER_BINDING_AMD)
        CASE(GL_COPY_READ_BUFFER)
        CASE(GL_COPY_WRITE_BUFFER)

//        CASE(GL_READ_BUFFER)
//        CASE(GL_UNPACK_ROW_LENGTH)
//        CASE(GL_UNPACK_SKIP_ROWS)
//        CASE(GL_UNPACK_SKIP_PIXELS)
//        CASE(GL_PACK_ROW_LENGTH)
//        CASE(GL_PACK_SKIP_ROWS)
//        CASE(GL_PACK_SKIP_PIXELS)
//        CASE(GL_COLOR)
//        CASE(GL_DEPTH)
//        CASE(GL_STENCIL)
//        CASE(GL_RED)
//        CASE(GL_RGB8)
//        CASE(GL_RGBA8)
//        CASE(GL_RGB10_A2)
        CASE(GL_TEXTURE_BINDING_3D)
        CASE(GL_UNPACK_SKIP_IMAGES)
        CASE(GL_UNPACK_IMAGE_HEIGHT)
        CASE(GL_TEXTURE_3D)
        CASE(GL_TEXTURE_WRAP_R)
        CASE(GL_MAX_3D_TEXTURE_SIZE)
        CASE(GL_UNSIGNED_INT_2_10_10_10_REV)
        CASE(GL_MAX_ELEMENTS_VERTICES)
        CASE(GL_MAX_ELEMENTS_INDICES)
        CASE(GL_TEXTURE_MIN_LOD)
        CASE(GL_TEXTURE_MAX_LOD)
        CASE(GL_TEXTURE_BASE_LEVEL)
        CASE(GL_TEXTURE_MAX_LEVEL)
        CASE(GL_MIN)
        CASE(GL_MAX)
        CASE(GL_DEPTH_COMPONENT24)
        CASE(GL_MAX_TEXTURE_LOD_BIAS)
        CASE(GL_TEXTURE_COMPARE_MODE)
        CASE(GL_TEXTURE_COMPARE_FUNC)
//        CASE(GL_CURRENT_QUERY)
//        CASE(GL_QUERY_RESULT)
//        CASE(GL_QUERY_RESULT_AVAILABLE)
//        CASE(GL_BUFFER_MAPPED)
//        CASE(GL_BUFFER_MAP_POINTER)
//        CASE(GL_STREAM_READ)
//        CASE(GL_STREAM_COPY)
//        CASE(GL_STATIC_READ)
//        CASE(GL_STATIC_COPY)
//        CASE(GL_DYNAMIC_READ)
//        CASE(GL_DYNAMIC_COPY)
        CASE(GL_MAX_DRAW_BUFFERS)
        CASE(GL_DRAW_BUFFER0)
        CASE(GL_DRAW_BUFFER1)
        CASE(GL_DRAW_BUFFER2)
        CASE(GL_DRAW_BUFFER3)
        CASE(GL_DRAW_BUFFER4)
        CASE(GL_DRAW_BUFFER5)
        CASE(GL_DRAW_BUFFER6)
        CASE(GL_DRAW_BUFFER7)
        CASE(GL_DRAW_BUFFER8)
        CASE(GL_DRAW_BUFFER9)
        CASE(GL_DRAW_BUFFER10)
        CASE(GL_DRAW_BUFFER11)
        CASE(GL_DRAW_BUFFER12)
        CASE(GL_DRAW_BUFFER13)
        CASE(GL_DRAW_BUFFER14)
        CASE(GL_DRAW_BUFFER15)
        CASE(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS)
        CASE(GL_MAX_VERTEX_UNIFORM_COMPONENTS)
        CASE(GL_SAMPLER_3D)
        CASE(GL_SAMPLER_2D_SHADOW)
        CASE(GL_FRAGMENT_SHADER_DERIVATIVE_HINT)
//        CASE(GL_PIXEL_PACK_BUFFER)
//        CASE(GL_PIXEL_UNPACK_BUFFER)
//        CASE(GL_PIXEL_PACK_BUFFER_BINDING)
//        CASE(GL_PIXEL_UNPACK_BUFFER_BINDING)
        CASE(GL_FLOAT_MAT2x3)
        CASE(GL_FLOAT_MAT2x4)
        CASE(GL_FLOAT_MAT3x2)
        CASE(GL_FLOAT_MAT3x4)
        CASE(GL_FLOAT_MAT4x2)
        CASE(GL_FLOAT_MAT4x3)
        CASE(GL_SRGB)
        CASE(GL_SRGB8)
        CASE(GL_SRGB8_ALPHA8)
        CASE(GL_COMPARE_REF_TO_TEXTURE)
        CASE(GL_MAJOR_VERSION)
        CASE(GL_MINOR_VERSION)
        CASE(GL_NUM_EXTENSIONS)
        CASE(GL_RGBA32F)
        CASE(GL_RGB32F)
        CASE(GL_RGBA16F)
        CASE(GL_RGB16F)
        CASE(GL_VERTEX_ATTRIB_ARRAY_INTEGER)
        CASE(GL_MAX_ARRAY_TEXTURE_LAYERS)
        CASE(GL_MIN_PROGRAM_TEXEL_OFFSET)
        CASE(GL_MAX_PROGRAM_TEXEL_OFFSET)
        CASE(GL_MAX_VARYING_COMPONENTS)
        CASE(GL_TEXTURE_2D_ARRAY)
        CASE(GL_TEXTURE_BINDING_2D_ARRAY)
        CASE(GL_R11F_G11F_B10F)
        CASE(GL_UNSIGNED_INT_10F_11F_11F_REV)
        CASE(GL_RGB9_E5)
        CASE(GL_UNSIGNED_INT_5_9_9_9_REV)
        CASE(GL_TRANSFORM_FEEDBACK_VARYING_MAX_LENGTH)
        CASE(GL_TRANSFORM_FEEDBACK_BUFFER_MODE)
        CASE(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS)
        CASE(GL_TRANSFORM_FEEDBACK_VARYINGS)
        CASE(GL_TRANSFORM_FEEDBACK_BUFFER_START)
        CASE(GL_TRANSFORM_FEEDBACK_BUFFER_SIZE)
        CASE(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN)
        CASE(GL_RASTERIZER_DISCARD)
        CASE(GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS)
        CASE(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS)
        CASE(GL_INTERLEAVED_ATTRIBS)
        CASE(GL_SEPARATE_ATTRIBS)
        CASE(GL_TRANSFORM_FEEDBACK_BUFFER)
        CASE(GL_TRANSFORM_FEEDBACK_BUFFER_BINDING)
        CASE(GL_RGBA32UI)
        CASE(GL_RGB32UI)
        CASE(GL_RGBA16UI)
        CASE(GL_RGB16UI)
        CASE(GL_RGBA8UI)
        CASE(GL_RGB8UI)
        CASE(GL_RGBA32I)
        CASE(GL_RGB32I)
        CASE(GL_RGBA16I)
        CASE(GL_RGB16I)
        CASE(GL_RGBA8I)
        CASE(GL_RGB8I)
        CASE(GL_RED_INTEGER)
        CASE(GL_RGB_INTEGER)
        CASE(GL_RGBA_INTEGER)
        CASE(GL_SAMPLER_2D_ARRAY)
        CASE(GL_SAMPLER_2D_ARRAY_SHADOW)
        CASE(GL_SAMPLER_CUBE_SHADOW)
        CASE(GL_UNSIGNED_INT_VEC2)
        CASE(GL_UNSIGNED_INT_VEC3)
        CASE(GL_UNSIGNED_INT_VEC4)
        CASE(GL_INT_SAMPLER_2D)
        CASE(GL_INT_SAMPLER_3D)
        CASE(GL_INT_SAMPLER_CUBE)
        CASE(GL_INT_SAMPLER_2D_ARRAY)
        CASE(GL_UNSIGNED_INT_SAMPLER_2D)
        CASE(GL_UNSIGNED_INT_SAMPLER_3D)
        CASE(GL_UNSIGNED_INT_SAMPLER_CUBE)
        CASE(GL_UNSIGNED_INT_SAMPLER_2D_ARRAY)
//        CASE(GL_BUFFER_ACCESS_FLAGS)
//        CASE(GL_BUFFER_MAP_LENGTH)
//        CASE(GL_BUFFER_MAP_OFFSET)
        CASE(GL_DEPTH_COMPONENT32F)
        CASE(GL_DEPTH32F_STENCIL8)
        CASE(GL_FLOAT_32_UNSIGNED_INT_24_8_REV)
        CASE(GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING)
        CASE(GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE)
        CASE(GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE)
        CASE(GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE)
        CASE(GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE)
        CASE(GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE)
        CASE(GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE)
        CASE(GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE)
        CASE(GL_FRAMEBUFFER_DEFAULT)
        CASE(GL_FRAMEBUFFER_UNDEFINED)
        CASE(GL_DEPTH_STENCIL_ATTACHMENT)
        CASE(GL_DEPTH_STENCIL)
        CASE(GL_UNSIGNED_INT_24_8)
        CASE(GL_DEPTH24_STENCIL8)
        CASE(GL_UNSIGNED_NORMALIZED)
        CASE(GL_DRAW_FRAMEBUFFER_BINDING)
        CASE(GL_READ_FRAMEBUFFER)
        CASE(GL_DRAW_FRAMEBUFFER)
        CASE(GL_READ_FRAMEBUFFER_BINDING)
        CASE(GL_RENDERBUFFER_SAMPLES)
        CASE(GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER)
        CASE(GL_MAX_COLOR_ATTACHMENTS)
        CASE(GL_COLOR_ATTACHMENT1)
        CASE(GL_COLOR_ATTACHMENT2)
        CASE(GL_COLOR_ATTACHMENT3)
        CASE(GL_COLOR_ATTACHMENT4)
        CASE(GL_COLOR_ATTACHMENT5)
        CASE(GL_COLOR_ATTACHMENT6)
        CASE(GL_COLOR_ATTACHMENT7)
        CASE(GL_COLOR_ATTACHMENT8)
        CASE(GL_COLOR_ATTACHMENT9)
        CASE(GL_COLOR_ATTACHMENT10)
        CASE(GL_COLOR_ATTACHMENT11)
        CASE(GL_COLOR_ATTACHMENT12)
        CASE(GL_COLOR_ATTACHMENT13)
        CASE(GL_COLOR_ATTACHMENT14)
        CASE(GL_COLOR_ATTACHMENT15)
        CASE(GL_COLOR_ATTACHMENT16)
        CASE(GL_COLOR_ATTACHMENT17)
        CASE(GL_COLOR_ATTACHMENT18)
        CASE(GL_COLOR_ATTACHMENT19)
        CASE(GL_COLOR_ATTACHMENT20)
        CASE(GL_COLOR_ATTACHMENT21)
        CASE(GL_COLOR_ATTACHMENT22)
        CASE(GL_COLOR_ATTACHMENT23)
        CASE(GL_COLOR_ATTACHMENT24)
        CASE(GL_COLOR_ATTACHMENT25)
        CASE(GL_COLOR_ATTACHMENT26)
        CASE(GL_COLOR_ATTACHMENT27)
        CASE(GL_COLOR_ATTACHMENT28)
        CASE(GL_COLOR_ATTACHMENT29)
        CASE(GL_COLOR_ATTACHMENT30)
        CASE(GL_COLOR_ATTACHMENT31)
        CASE(GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE)
        CASE(GL_MAX_SAMPLES)
        CASE(GL_HALF_FLOAT)
//        CASE(GL_MAP_READ_BIT)
//        CASE(GL_MAP_WRITE_BIT)
//        CASE(GL_MAP_INVALIDATE_RANGE_BIT)
//        CASE(GL_MAP_INVALIDATE_BUFFER_BIT)
        CASE(GL_MAP_FLUSH_EXPLICIT_BIT)
        CASE(GL_MAP_UNSYNCHRONIZED_BIT)
        CASE(GL_RG)
        CASE(GL_RG_INTEGER)
        CASE(GL_R8)
        CASE(GL_RG8)
        CASE(GL_R16F)
        CASE(GL_R32F)
        CASE(GL_RG16F)
        CASE(GL_RG32F)
        CASE(GL_R8I)
        CASE(GL_R8UI)
        CASE(GL_R16I)
        CASE(GL_R16UI)
        CASE(GL_R32I)
        CASE(GL_R32UI)
        CASE(GL_RG8I)
        CASE(GL_RG8UI)
        CASE(GL_RG16I)
        CASE(GL_RG16UI)
        CASE(GL_RG32I)
        CASE(GL_RG32UI)
        CASE(GL_VERTEX_ARRAY_BINDING)
        CASE(GL_R8_SNORM)
        CASE(GL_RG8_SNORM)
        CASE(GL_RGB8_SNORM)
        CASE(GL_RGBA8_SNORM)
        CASE(GL_SIGNED_NORMALIZED)
        CASE(GL_PRIMITIVE_RESTART_FIXED_INDEX)
//        CASE(GL_COPY_READ_BUFFER)
//        CASE(GL_COPY_WRITE_BUFFER)
//        CASE(GL_COPY_READ_BUFFER_BINDING)
//        CASE(GL_COPY_WRITE_BUFFER_BINDING)
        CASE(GL_UNIFORM_BUFFER)
        CASE(GL_UNIFORM_BUFFER_BINDING)
        CASE(GL_UNIFORM_BUFFER_START)
        CASE(GL_UNIFORM_BUFFER_SIZE)
        CASE(GL_MAX_VERTEX_UNIFORM_BLOCKS)
        CASE(GL_MAX_FRAGMENT_UNIFORM_BLOCKS)
        CASE(GL_MAX_COMBINED_UNIFORM_BLOCKS)
        CASE(GL_MAX_UNIFORM_BUFFER_BINDINGS)
        CASE(GL_MAX_UNIFORM_BLOCK_SIZE)
        CASE(GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS)
        CASE(GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS)
        CASE(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT)
        CASE(GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH)
        CASE(GL_ACTIVE_UNIFORM_BLOCKS)
        CASE(GL_UNIFORM_TYPE)
        CASE(GL_UNIFORM_SIZE)
        CASE(GL_UNIFORM_NAME_LENGTH)
        CASE(GL_UNIFORM_BLOCK_INDEX)
        CASE(GL_UNIFORM_OFFSET)
        CASE(GL_UNIFORM_ARRAY_STRIDE)
        CASE(GL_UNIFORM_MATRIX_STRIDE)
        CASE(GL_UNIFORM_IS_ROW_MAJOR)
        CASE(GL_UNIFORM_BLOCK_BINDING)
        CASE(GL_UNIFORM_BLOCK_DATA_SIZE)
        CASE(GL_UNIFORM_BLOCK_NAME_LENGTH)
        CASE(GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS)
        CASE(GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES)
        CASE(GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER)
        CASE(GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER)
        CASE(GL_INVALID_INDEX)
        CASE(GL_MAX_VERTEX_OUTPUT_COMPONENTS)
        CASE(GL_MAX_FRAGMENT_INPUT_COMPONENTS)
        CASE(GL_MAX_SERVER_WAIT_TIMEOUT)
        CASE(GL_OBJECT_TYPE)
        CASE(GL_SYNC_CONDITION)
        CASE(GL_SYNC_STATUS)
        CASE(GL_SYNC_FLAGS)
        CASE(GL_SYNC_FENCE)
        CASE(GL_SYNC_GPU_COMMANDS_COMPLETE)
        CASE(GL_UNSIGNALED)
        CASE(GL_SIGNALED)
        CASE(GL_ALREADY_SIGNALED)
        CASE(GL_TIMEOUT_EXPIRED)
        CASE(GL_CONDITION_SATISFIED)
        CASE(GL_WAIT_FAILED)
//        CASE(GL_SYNC_FLUSH_COMMANDS_BIT)
//        CASE(GL_TIMEOUT_IGNORED)
        CASE(GL_VERTEX_ATTRIB_ARRAY_DIVISOR)
        CASE(GL_ANY_SAMPLES_PASSED)
        CASE(GL_ANY_SAMPLES_PASSED_CONSERVATIVE)
        CASE(GL_SAMPLER_BINDING)
        CASE(GL_RGB10_A2UI)
        CASE(GL_TEXTURE_SWIZZLE_R)
        CASE(GL_TEXTURE_SWIZZLE_G)
        CASE(GL_TEXTURE_SWIZZLE_B)
        CASE(GL_TEXTURE_SWIZZLE_A)
//        CASE(GL_GREEN)
//        CASE(GL_BLUE)
        CASE(GL_INT_2_10_10_10_REV)
        CASE(GL_TRANSFORM_FEEDBACK)
        CASE(GL_TRANSFORM_FEEDBACK_PAUSED)
        CASE(GL_TRANSFORM_FEEDBACK_ACTIVE)
        CASE(GL_TRANSFORM_FEEDBACK_BINDING)
        CASE(GL_PROGRAM_BINARY_RETRIEVABLE_HINT)
        CASE(GL_PROGRAM_BINARY_LENGTH)
        CASE(GL_NUM_PROGRAM_BINARY_FORMATS)
        CASE(GL_PROGRAM_BINARY_FORMATS)
        CASE(GL_COMPRESSED_R11_EAC)
        CASE(GL_COMPRESSED_SIGNED_R11_EAC)
        CASE(GL_COMPRESSED_RG11_EAC)
        CASE(GL_COMPRESSED_SIGNED_RG11_EAC)
        CASE(GL_COMPRESSED_RGB8_ETC2)
        CASE(GL_COMPRESSED_SRGB8_ETC2)
        CASE(GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2)
        CASE(GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2)
        CASE(GL_COMPRESSED_RGBA8_ETC2_EAC)
        CASE(GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC)
        CASE(GL_TEXTURE_IMMUTABLE_FORMAT)
        CASE(GL_MAX_ELEMENT_INDEX)
        CASE(GL_NUM_SAMPLE_COUNTS)
        CASE(GL_TEXTURE_IMMUTABLE_LEVELS)

        CASE(GL_TEXTURE_RECTANGLE)
        CASE(GL_TEXTURE_CUBE_MAP_ARRAY)

        CASE(GL_BGR)
        CASE(GL_BGRA)
        CASE(GL_GREEN_INTEGER)
        CASE(GL_BLUE_INTEGER)
        CASE(GL_BGR_INTEGER)
        CASE(GL_BGRA_INTEGER)

/*
 * OpenGL 1.3
 */

        /* multitexture */
        CASE(GL_TEXTURE0)
        CASE(GL_TEXTURE1)
        CASE(GL_TEXTURE2)
        CASE(GL_TEXTURE3)
        CASE(GL_TEXTURE4)
        CASE(GL_TEXTURE5)
        CASE(GL_TEXTURE6)
        CASE(GL_TEXTURE7)
        CASE(GL_TEXTURE8)
        CASE(GL_TEXTURE9)
        CASE(GL_TEXTURE10)
        CASE(GL_TEXTURE11)
        CASE(GL_TEXTURE12)
        CASE(GL_TEXTURE13)
        CASE(GL_TEXTURE14)
        CASE(GL_TEXTURE15)
        CASE(GL_TEXTURE16)
        CASE(GL_TEXTURE17)
        CASE(GL_TEXTURE18)
        CASE(GL_TEXTURE19)
        CASE(GL_TEXTURE20)
        CASE(GL_TEXTURE21)
        CASE(GL_TEXTURE22)
        CASE(GL_TEXTURE23)
        CASE(GL_TEXTURE24)
        CASE(GL_TEXTURE25)
        CASE(GL_TEXTURE26)
        CASE(GL_TEXTURE27)
        CASE(GL_TEXTURE28)
        CASE(GL_TEXTURE29)
        CASE(GL_TEXTURE30)
        CASE(GL_TEXTURE31)
        CASE(GL_ACTIVE_TEXTURE)
        CASE(GL_CLIENT_ACTIVE_TEXTURE)
        CASE(GL_MAX_TEXTURE_UNITS)
/* texture_cube_map */
        CASE(GL_NORMAL_MAP)
        CASE(GL_REFLECTION_MAP)
        CASE(GL_TEXTURE_CUBE_MAP)
        CASE(GL_TEXTURE_BINDING_CUBE_MAP)
        CASE(GL_TEXTURE_CUBE_MAP_POSITIVE_X)
        CASE(GL_TEXTURE_CUBE_MAP_NEGATIVE_X)
        CASE(GL_TEXTURE_CUBE_MAP_POSITIVE_Y)
        CASE(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y)
        CASE(GL_TEXTURE_CUBE_MAP_POSITIVE_Z)
        CASE(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z)
        CASE(GL_PROXY_TEXTURE_CUBE_MAP)
        CASE(GL_MAX_CUBE_MAP_TEXTURE_SIZE)
/* texture_compression */
        CASE(GL_COMPRESSED_ALPHA)
        CASE(GL_COMPRESSED_LUMINANCE)
        CASE(GL_COMPRESSED_LUMINANCE_ALPHA)
        CASE(GL_COMPRESSED_INTENSITY)
        CASE(GL_COMPRESSED_RGB)
        CASE(GL_COMPRESSED_RGBA)
        CASE(GL_TEXTURE_COMPRESSION_HINT)
        CASE(GL_TEXTURE_COMPRESSED_IMAGE_SIZE)
        CASE(GL_TEXTURE_COMPRESSED)
        CASE(GL_NUM_COMPRESSED_TEXTURE_FORMATS)
        CASE(GL_COMPRESSED_TEXTURE_FORMATS)
/* multisample */
        CASE(GL_MULTISAMPLE)
        CASE(GL_SAMPLE_ALPHA_TO_COVERAGE)
        CASE(GL_SAMPLE_ALPHA_TO_ONE)
        CASE(GL_SAMPLE_COVERAGE)
        CASE(GL_SAMPLE_BUFFERS)
        CASE(GL_SAMPLES)
        CASE(GL_SAMPLE_COVERAGE_VALUE)
        CASE(GL_SAMPLE_COVERAGE_INVERT)
        CASE(GL_MULTISAMPLE_BIT)
/* transpose_matrix */
        CASE(GL_TRANSPOSE_MODELVIEW_MATRIX)
        CASE(GL_TRANSPOSE_PROJECTION_MATRIX)
        CASE(GL_TRANSPOSE_TEXTURE_MATRIX)
        CASE(GL_TRANSPOSE_COLOR_MATRIX)
/* texture_env_combine */
        CASE(GL_COMBINE)
        CASE(GL_COMBINE_RGB)
        CASE(GL_COMBINE_ALPHA)
        CASE(GL_SOURCE0_RGB)
        CASE(GL_SOURCE1_RGB)
        CASE(GL_SOURCE2_RGB)
        CASE(GL_SOURCE0_ALPHA)
        CASE(GL_SOURCE1_ALPHA)
        CASE(GL_SOURCE2_ALPHA)
        CASE(GL_OPERAND0_RGB)
        CASE(GL_OPERAND1_RGB)
        CASE(GL_OPERAND2_RGB)
        CASE(GL_OPERAND0_ALPHA)
        CASE(GL_OPERAND1_ALPHA)
        CASE(GL_OPERAND2_ALPHA)
        CASE(GL_RGB_SCALE)
        CASE(GL_ADD_SIGNED)
        CASE(GL_INTERPOLATE)
        CASE(GL_SUBTRACT)
        CASE(GL_CONSTANT)
        CASE(GL_PRIMARY_COLOR)
        CASE(GL_PREVIOUS)
/* texture_env_dot3 */
        CASE(GL_DOT3_RGB)
        CASE(GL_DOT3_RGBA)
/* texture_border_clamp */
        CASE(GL_CLAMP_TO_BORDER)
/*
 * Miscellaneous
 */
        default:
            sprintf(str, "0x%x", e);
            return str;
    }
}

#if LOG_CALLED_FUNCS

#include "../config/config.h"

void log_unique_function(const char* func_name) {
    if (!func_name || strlen(func_name) < 2 || strncmp(func_name, "gl", 2) != 0) {
        return;
    }
    static std::unordered_set<std::string> logged_functions;
    static std::mutex log_mutex;
    std::string func_str(func_name);

    std::lock_guard<std::mutex> guard(log_mutex);

    if (logged_functions.find(func_str) != logged_functions.end()) {
        return;
    }

    static FILE* fp = fopen(concatenate(mg_directory_path, "/glcalls.txt"), "a");
    if (fp) {
        fprintf(fp, "%s\n", func_name);
        fflush(fp);
    }

    logged_functions.insert(func_str);
}
#endif