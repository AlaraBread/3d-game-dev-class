#ifndef __GFC_COLOR_H__
#define __GFC_COLOR_H__

#include <SDL.h>
#include "gfc_vector.h"

typedef enum
{
    CT_RGBAf,   /**<RGBA in floating point 0-1*/
    CT_RGBA8,   /**<RGBA in 8 bit int 0-255*/
    CT_HSL,     /**<Hue,Saturation and Lightness*/
    CT_HEX      /**<Hex code 0xFFFFFF*/
}GFC_ColorType;

typedef struct
{
    float r;    /**<red value from 0-1, or the HUE*/
    float g;    /**<green value or Saturation*/
    float b;    /**<blue value or Lightness*/
    float a;    /**<alpha, or transparency from 0 (clear) to opaque 1*/
    GFC_ColorType ct;   /**<how this structure is used*/
}GFC_Color;

#define GFC_COLOR_RED gfc_color8(255,0,0,255)
#define GFC_COLOR_LIGHTRED gfc_color(1,.2,.2,1)
#define GFC_COLOR_DARKRED gfc_color(128,0,0,1)
#define GFC_COLOR_GREEN gfc_color8(0,255,0,255)
#define GFC_COLOR_LIGHTGREEN gfc_color(.2,1,.2,1)
#define GFC_COLOR_DARKGREEN gfc_color(0,128,0,1)
#define GFC_COLOR_BLUE gfc_color8(0,0,255,255)
#define GFC_COLOR_LIGHTBLUE gfc_color(.5,.6,1,1)
#define GFC_COLOR_DARKBLUE gfc_color(0,0,128,1)
#define GFC_COLOR_BLACK gfc_color8(0,0,0,255)
#define GFC_COLOR_DARKGREY gfc_color8(32,32,32,255)
#define GFC_COLOR_GREY gfc_color8(128,128,128,255)
#define GFC_COLOR_LIGHTGREY gfc_color8(192,192,192,255)
#define GFC_COLOR_WHITE gfc_color8(255,255,255,255)
#define GFC_COLOR_YELLOW gfc_color8(255,255,0,255)
#define GFC_COLOR_LIGHTYELLOW gfc_color(0.9,0.9,0.7,1)
#define GFC_COLOR_DARKYELLOW gfc_color8(128,128,0,255)
#define GFC_COLOR_CYAN gfc_color8(0,255,255,255)
#define GFC_COLOR_LIGHTCYAN gfc_color(0.7,0.9,0.9,1)
#define GFC_COLOR_DARKCYAN gfc_color8(0,128,128,255)
#define GFC_COLOR_MAGENTA gfc_color8(255,0,255,255)
#define GFC_COLOR_LIGHTMAGENTA gfc_color8(1,0.2,1,1)
#define GFC_COLOR_DARKMAGENTA gfc_color8(128,0,128,255)
#define GFC_COLOR_TRANSPARENT gfc_color8(0,0,0,0)
#define GFC_COLOR_BROWN gfc_color(.7,.4,0,1)
#define GFC_COLOR_ORANGE gfc_color(.9,.6,0,1)
#define GFC_COLOR_LIGHTORANGE gfc_color(1,.7,0.1,1)
#define GFC_COLOR_DARKORANGE gfc_color(.45,.3,0,1)


#define gfc_color_copy(dst,src) (dst.r = src.r,dst.g = src.g,dst.b = src.b,dst.a = src.a,dst.ct = src.ct)

/**
 * @brief create and return a color from floating point RGBA values
 * values over 1 are clamped to 1, values under 0 are clamped to zero
 * @param r red value 0 - 1
 * @param g green value 0 - 1
 * @param b blue value 0 - 1
 * @param a alpha value 0 - 1
 * @returns a newly set color
 */
GFC_Color gfc_color(float r,float g,float b,float a);

/**
 * @brief check if two colors are the same color
 * @param a color to check
 * @param b color to check
 * @return 1 if the same color, 0 if not
 */
int gfc_color_cmp(GFC_Color a, GFC_Color b);

/**
 * @brief create and return a color from 8 bit integer RGBA values
 * @param r red value 0 - 255
 * @param g green value 0 - 255
 * @param b blue value 0 - 255
 * @param a alpha value 0 - 255
 * @returns a newly set color
 */
GFC_Color gfc_color8(Uint8 r,Uint8 g,Uint8 b,Uint8 a);
/**
 * @brief create and return a color from floating point RGBA values
 * @param h hue value 0 - 360
 * @param s saturation value 0 - 1
 * @param l lightness value 0 - 1, 0 is black, 1 is white, 0.5 is pure color
 * @param a alpha value 0 - 1
 * @returns a newly set color
 */
GFC_Color gfc_color_hsl(float h,float s,float l,float a);
/**
 * @brief create and return a color from floating point RGBA values
 * @param hex the hex value of the color
 * @returns a newly set color
 */
GFC_Color gfc_color_hex(Uint32 hex);

/**
 * @brief convert a color to floating point representation
 * @param color the color to convert
 * @return a new color in the format CT_RGBAf
 */
GFC_Color gfc_color_to_float(GFC_Color color);

/**
 * @brief convert a color to 8 bit integer representation
 * @param color the color to convert
 * @return a new color
 */
GFC_Color gfc_color_to_int8(GFC_Color color);

/**
 * @brief convert a color to Hue Saturation Lightness (and Alpha) representation
 * @param color the color to convert
 * @return a new color in the format CT_HSL
 */
GFC_Color gfc_color_to_hsla(GFC_Color color);

/**
 * @brief convert a color to Hex representation
 * @param color the color to convert
 * @return a new color in the format CT_HEX
 */
Uint32 gfc_color_to_hex(GFC_Color color);

/**
 * @brief convert a SDL color to gfc floating point representation
 * @param color the color to convert
 * @return a new color in the format CT_RGBAf
 */
GFC_Color gfc_color_from_sdl(SDL_Color color);

/**
 * @brief convert a gfc color to SDL color format
 * @param color the color to convert
 * @return a new SDL_GFC_Color
 */
SDL_Color gfc_color_to_sdl(GFC_Color color);

/**
 * @brief create a color from a gfc_vector where x = r, y = g, z = b, w = a
 * @param gfc_vector the gfc_vector to convert
 * @return a new color in the format CT_RGBA8
 */
GFC_Color gfc_color_from_vector4(GFC_Vector4DF gfc_vector);

/**
 * @brief create a color from a gfc_vector where x = r, y = g, z = b, w = a
 * @param gfc_vector the gfc_vector to convert
 * @return a new color in the format CT_RGBAf
 */
GFC_Color gfc_color_from_vector4f(GFC_Vector4DF gfc_vector);

/**
 * @brief convert a color to a gfc_vector where x = r, y = g, z = b, w = a
 * @param color the color to convert
 * @return a gfc_vector set to the color provided
 * @note: will be in values from 0-255
 */
GFC_Vector4DF gfc_color_to_vector4(GFC_Color color);

/**
 * @brief convert a color to a gfc_vector where x = r, y = g, z = b, w = a
 * @param color the color to convert
 * @return a gfc_vector set to the color provided
 * @note: will be in values from 0-1.0f
 */
GFC_Vector4DF gfc_color_to_vector4f(GFC_Color color);

/**
 * @brief Get the Hue value of a color.  It may convert internally as needed.
 * @param color the color to check
 * @return the value of the Hue of the color (0-360)
 */
float gfc_color_get_hue(GFC_Color color);

/**
 * @brief Sets the hue of the color provided.  Leaves format unchanged.  Saturation and Value are also unchanged
 * if saturation is zero this may not show a change
 * @param hue the hue to change to
 * @param color the color to change
 * @return a new color in the format CT_HSL
 */
void gfc_color_set_hue(float hue,GFC_Color *color);

/**
 * @brief adds colors a and b and saves the result in dst
 * @param dst if NULL this function is a no op.  Result saved here
 * @param a the format for a is used as the resulting format.  Conversion happen before adding
 * @param b the color to add to a
 */
void gfc_color_add(GFC_Color *dst,GFC_Color a,GFC_Color b);

/**
 * @brief blend two colors together
 * @param dst if NULL this function is a no op.  Result saved here
 * @param a the format for a is used as the resulting format.  Conversion happen before blending
 * @param b the color to add to a
 */
void gfc_color_blend(GFC_Color *dst,GFC_Color a,GFC_Color b);

/**
 * @brief multiplies colors a and b and saves the result in dst
 * @param dst if NULL this function is a no op.  Result saved here
 * @param a the format for a is used as the resulting format.  Conversion happen before multiplying
 * @param b the color to add to a
 */
void gfc_color_multiply(GFC_Color *dst,GFC_Color a,GFC_Color b);

/**
 * @brief make sure the values in the color are within acceptable ranges
 * Note this will break any colors used as a gfc_vector that contain negative values
 * @param color the color to clamp
 * @return the new color within specified range
 */
GFC_Color gfc_color_clamp(GFC_Color color);


 #endif
