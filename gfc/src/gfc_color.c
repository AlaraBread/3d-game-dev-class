#include "simple_logger.h"
#include "gfc_color.h"


int gfc_color_cmp(GFC_Color a, GFC_Color b)
{
    a = gfc_color_to_float(a);
    b = gfc_color_to_float(b);

    if ((a.r != b.r)||
        (a.g != b.g)||
        (a.b != b.b)||
        (a.a != b.a))return 0;
    return 1;
}

GFC_Color gfc_color(float r,float g,float b,float a)
{
    GFC_Color color;
    color.r = MIN(MAX(r,-1),1);
    color.g = MIN(MAX(g,-1),1);
    color.b = MIN(MAX(b,-1),1);
    color.a = MIN(MAX(a,-1),1);
    color.ct = CT_RGBAf;
    return color;
}

GFC_Color gfc_color8(Uint8 r,Uint8 g,Uint8 b,Uint8 a)
{
    GFC_Color color;
    color.r = (float)r;
    color.g = (float)g;
    color.b = (float)b;
    color.a = (float)a;
    color.ct = CT_RGBA8;
    return color;
}

GFC_Color gfc_color_hsl(float h,float s,float l,float a)
{
    GFC_Color color;
    color.r = h;
    while (color.r < 0)color.r+=360;
    while (color.r >= 360)color.r-=360;
    color.g = MIN(MAX(s,-1),1);
    color.b = MIN(MAX(l,-1),1);
    color.a = MIN(MAX(a,-1),1);
    color.ct = CT_HSL;
    return color;
}

GFC_Color gfc_color_hex(Uint32 hex)
{
    GFC_Color color;
    color.r = (float)hex;
    color.ct = CT_HEX;
    return color;
}

GFC_Color gfc_color_to_float(GFC_Color color)
{
    GFC_Color nc;
    float C,X,m;
    float factor = 1.0/255.0;
    switch(color.ct)
    {
        default:
        case CT_RGBAf:
            return color;
        case CT_RGBA8:
            nc.r = color.r *factor;
            nc.g = color.g *factor;
            nc.b = color.b *factor;
            nc.a = color.a *factor;
            break;
        case CT_HSL:
            nc.a = color.a;
            C = (1 - fabs(2*color.b -1)) * color.g;
            X = C * (1 - fabs(fmod(color.r/60,2) - 1));
            m = color.b - (C * 0.5);
            if (color.r < 60)
            {
                nc.r = C+m;
                nc.g = X+m;
                nc.b = 0+m;
            }
            else if (color.r < 120)
            {
                nc.r = X+m;
                nc.g = C+m;
                nc.b = 0+m;
            }
            else if (color.r < 180)
            {
                nc.r = 0+m;
                nc.g = C+m;
                nc.b = X+m;
            }
            else if (color.r < 240)
            {
                nc.r = 0+m;
                nc.g = X+m;
                nc.b = C+m;
            }
            else if (color.r < 300)
            {
                nc.r = X+m;
                nc.g = 0+m;
                nc.b = C+m;
            }
            else
            {
                nc.r = C+m;
                nc.g = 0+m;
                nc.b = X+m;
            }
            break;
        case CT_HEX:
            nc.r = (((Uint32)color.r & 0xff000000)>>24)/0xff;
            nc.g = (((Uint32)color.r & 0x00ff0000)>>16)/0xff;
            nc.b = (((Uint32)color.r & 0x0000ff00)>>8)/0xff;
            nc.a = ((Uint32)color.r & 0x000000ff)/0xff;
            break;
    }
    nc.ct = CT_RGBAf;
    return nc;
}

GFC_Color gfc_color_to_int8(GFC_Color color)
{
    GFC_Color nc;
    switch (color.ct)
    {
        case CT_HEX:
            nc.r = (((Uint32)color.r & 0xff000000)>>24);
            nc.g = (((Uint32)color.r & 0x00ff0000)>>16);
            nc.b = (((Uint32)color.r & 0x0000ff00)>>8);
            nc.a = ((Uint32)color.r & 0x000000ff);
        case CT_RGBA8:
            return color;
        case CT_HSL:
            color = gfc_color_to_float(color);
        case CT_RGBAf:
        default:
            nc.r = color.r *255;
            nc.g = color.g *255;
            nc.b = color.b *255;
            nc.a = color.a *255;
            break;
    }
    nc.ct = CT_RGBA8;
    return nc;
}

GFC_Color gfc_color_to_hsla(GFC_Color color)
{
    GFC_Color nc;
    float D,Cmin,Cmax,H,S,L;
    if (color.ct == CT_HSL)
    {
        return color;
    }
    nc = gfc_color_to_float(color);
    Cmax = MAX(MAX(nc.r,nc.g),nc.b);
    Cmin = MIN(MIN(nc.r,nc.g),nc.b);
    D = Cmax - Cmin;
    L = (Cmax + Cmin) * 0.5;
    if (D == 0.0)
    {
        H = 0;
        S = 0;
    }
    else
    {
        S = D / (1 - fabs(Cmax+Cmin-1));
        if (Cmax == nc.r)
        {
            H = 60 * fmod(((nc.g - nc.b)/D),6);
        }
        else if (Cmax == nc.g)
        {
            H = 60 * (((nc.b - nc.r)/D)+2);
        }
        else
        {
            H = 60 * (((nc.r - nc.g)/D)+4);
        }
    }
    nc.r = H;
    nc.g = S;
    nc.b = L;
    nc.ct = CT_HSL;
    return nc;
}

Uint32 gfc_color_to_hex(GFC_Color color)
{
    Uint32 hex;
    Uint32 r,g,b,a;
    GFC_Color nc;
    if (color.ct == CT_HEX)
    {
        return color.r;
    }
    nc = gfc_color_to_int8(color);
    r = (Uint32)(nc.r) << 24;
    g = (Uint32)(nc.g) << 16;
    b = (Uint32)(nc.b) << 8;
    a = (Uint32)(nc.a);
    hex = r | g | b | a;
    return hex;
}

GFC_Color gfc_color_from_vector4(GFC_Vector4DF gfc_vector)
{
    GFC_Color color;
    color.ct = CT_RGBA8;
    color.r = gfc_vector.x;
    color.g = gfc_vector.y;
    color.b = gfc_vector.z;
    color.a = gfc_vector.w;
    return color;
}

GFC_Color gfc_color_from_vector4f(GFC_Vector4DF gfc_vector)
{
    GFC_Color color;
    color.ct = CT_RGBAf;
    color.r = gfc_vector.x;
    color.g = gfc_vector.y;
    color.b = gfc_vector.z;
    color.a = gfc_vector.w;
    return color;
}


GFC_Vector4DF gfc_color_to_vector4(GFC_Color color)
{
    GFC_Vector4DF gfc_vector;
    color = gfc_color_to_int8(color);
    gfc_vector.x = color.r;
    gfc_vector.y = color.g;
    gfc_vector.z = color.b;
    gfc_vector.w = color.a;
    return gfc_vector;
}

GFC_Vector4DF gfc_color_to_vector4f(GFC_Color color)
{
    GFC_Vector4DF gfc_vector;
    color = gfc_color_to_float(color);
    gfc_vector.x = color.r;
    gfc_vector.y = color.g;
    gfc_vector.z = color.b;
    gfc_vector.w = color.a;
    return gfc_vector;
}


float gfc_color_get_hue(GFC_Color color)
{
    color = gfc_color_to_hsla(color);
    return color.r;
}

GFC_Color gfc_color_from_sdl(SDL_Color color)
{
    GFC_Color nc;
    float factor = 1.0/255.0;
    nc.ct = CT_RGBAf;
    nc.r = color.r *factor;
    nc.g = color.r *factor;
    nc.b = color.r *factor;
    nc.a = color.r *factor;
    return nc;
}

SDL_Color gfc_color_to_sdl(GFC_Color color)
{
    SDL_Color nc;
    color = gfc_color_to_int8(color);
    nc.r = (Uint8)color.r;
    nc.g = (Uint8)color.g;
    nc.b = (Uint8)color.b;
    nc.a = (Uint8)color.a;
    return nc;
}

void gfc_color_set_hue(float hue,GFC_Color *color)
{
    GFC_Color temp;
    if (color->ct == CT_HSL)
    {
        color->r = hue;
        return;
    }
    temp = gfc_color_to_hsla(*color);
    temp.r = hue;
    switch(color->ct)
    {
        case CT_HSL:
            // case already handled
            break;
        case CT_HEX:
            color->r = gfc_color_to_hex(temp);
            return;
        case CT_RGBA8:
            *color = gfc_color_to_int8(temp);
            return;
        case CT_RGBAf:
            *color = gfc_color_to_float(temp);
    }
}

void gfc_color_blend(GFC_Color *dst,GFC_Color a,GFC_Color b)
{
    float hex;
    if (!dst)return;
    switch (a.ct)
    {
        case CT_HSL:
            b = gfc_color_to_hsla(b);
            *dst = gfc_color_hsl(
                (a.r+b.r)*0.5,
                (a.g+b.g)*0.5,
                (a.b+b.b)*0.5,
                (a.a+b.a)*0.5);
        return;
        case CT_HEX:
            a = gfc_color_to_int8(a);
            b = gfc_color_to_int8(b);
            *dst = gfc_color8(
                (a.r+b.r)*0.5,
                (a.g+b.g)*0.5,
                (a.b+b.b)*0.5,
                (a.a+b.a)*0.5);
            hex = gfc_color_to_hex(*dst);
            *dst = gfc_color_hex(hex);
        return;
        case CT_RGBA8:
            b = gfc_color_to_int8(b);
            *dst = gfc_color8(
                (a.r+b.r)*0.5,
                (a.g+b.g)*0.5,
                (a.b+b.b)*0.5,
                (a.a+b.a)*0.5);
        return;
        case CT_RGBAf:
            b = gfc_color_to_float(b);
            *dst = gfc_color(
                (a.r+b.r)*0.5,
                (a.g+b.g)*0.5,
                (a.b+b.b)*0.5,
                (a.a+b.a)*0.5);
        return;
    }    
}

void gfc_color_add(GFC_Color *dst,GFC_Color a,GFC_Color b)
{
    float hex;
    if (!dst)return;
    switch (a.ct)
    {
        case CT_HSL:
            b = gfc_color_to_hsla(b);
            *dst = gfc_color_hsl(
                a.r+b.r,
                a.g+b.g,
                a.b+b.b,
                a.a+b.a);
        return;
        case CT_HEX:
            a = gfc_color_to_int8(a);
            b = gfc_color_to_int8(b);
            *dst = gfc_color8(
                a.r+b.r,
                a.g+b.g,
                a.b+b.b,
                a.a+b.a);
            hex = gfc_color_to_hex(*dst);
            *dst = gfc_color_hex(hex);
        return;
        case CT_RGBA8:
            b = gfc_color_to_int8(b);
            *dst = gfc_color8(
                a.r+b.r,
                a.g+b.g,
                a.b+b.b,
                a.a+b.a);
        return;
        case CT_RGBAf:
            b = gfc_color_to_float(b);
            *dst = gfc_color(
                a.r+b.r,
                a.g+b.g,
                a.b+b.b,
                a.a+b.a);
        return;
    }    
}

void gfc_color_multiply(GFC_Color *dst,GFC_Color a,GFC_Color b)
{
    GFC_ColorType ct;
    if (!dst)return;
    ct = a.ct;
    a = gfc_color_to_float(a);
    b = gfc_color_to_float(b);
    dst->ct = CT_RGBAf;
    dst->r  = a.r*b.r;
    dst->g  = a.g*b.g;
    dst->b  = a.b*b.b;
    dst->a  = a.a*b.a;
    switch(ct)
    {
        case CT_RGBAf:
            return;//done
        case CT_RGBA8:
            *dst = gfc_color_to_int8(*dst);
            return;
        case CT_HSL:
            *dst = gfc_color_to_hsla(*dst);
            return;
        case CT_HEX:
            *dst = gfc_color_hex(gfc_color_to_hex(*dst));
            return;
    }
}

GFC_Color gfc_color_clamp(GFC_Color color)
{
    switch(color.ct)
    {
        case CT_RGBAf:
            if (color.r < 0)color.r = 0;
            else if (color.r > 1)color.r = 1;
            if (color.g < 0)color.g = 0;
            else if (color.g > 1)color.g = 1;
            if (color.b < 0)color.b = 0;
            else if (color.b > 1)color.b = 1;
            if (color.a < 0)color.a = 0;
            else if (color.a > 1)color.a = 1;
        return color;
        case CT_RGBA8:
            if (color.r < 0)color.r = 0;
            else if (color.r > 255)color.r = 255;
            if (color.g < 0)color.g = 0;
            else if (color.g > 255)color.g = 255;
            if (color.b < 0)color.b = 0;
            else if (color.b > 255)color.b = 255;
            if (color.a < 0)color.a = 0;
            else if (color.a > 255)color.a = 255;
        return color;
        case CT_HEX:
        return color;
        case CT_HSL:
            while (color.r < 0)color.r += 360;
            while (color.r > 360)color.r -= 360;
            if (color.g < 0)color.g = 0;
            else if (color.g > 1)color.g = 1;
            if (color.b < 0)color.b = 0;
            else if (color.b > 1)color.b = 1;
            if (color.a < 0)color.a = 0;
            else if (color.a > 1)color.a = 1;
        return color;
    }
    return color;
}

/*eol@eof*/
