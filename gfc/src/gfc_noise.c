#include "gfc_noise.h"

double interpolate(double a0, double a1, double w)
{
    return (a1 - a0) * w + a0;
}

/* Create pseudorandom direction gfc_vector
 */
GFC_Vector2D randomGradient(int ix, int iy) {
    // No precomputed gradients mean this works for any number of grid coordinates
    double random;
    GFC_Vector2D v;
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2; // rotation width
    unsigned a = ix, b = iy;
    a *= 3284157443;
    b ^= (a << s) | (a >> (w-s));
    b *= 1911520717;
    a ^= b << s | b >> (w-s);
    a *= 2048419325;
    random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]
    v.x = cos(random); v.y = sin(random);
    return v;
}

// Computes the dot product of the distance and gradient gfc_vectors.
double dotGridGradient(int ix, int iy, double x, double y)
{
    // Get gradient from integer coordinates
    GFC_Vector2D gradient = randomGradient(ix, iy);

    // Compute the distance gfc_vector
    double dx = x - (double)ix;
    double dy = y - (double)iy;

    // Compute the dot-product
    return (dx*gradient.x + dy*gradient.y);
}

// Compute Perlin noise at coordinates x, y
double gfc_perlin(GFC_Vector2D in)
{
    // Determine grid cell coordinates
    int x0 = (int)floor(in.x);
    int x1 = x0 + 1;
    int y0 = (int)floor(in.y);
    int y1 = y0 + 1;

    // Determine interpolation weights
    // Could also use higher order polynomial/s-curve here
    double sx = in.x - (double)x0;
    double sy = in.y - (double)y0;

    // Interpolate between grid point gradients
    double n0, n1, ix0, ix1, value;

    n0 = dotGridGradient(x0, y0, in.x, in.y);
    n1 = dotGridGradient(x1, y0, in.x, in.y);
    ix0 = interpolate(n0, n1, sx);

    n0 = dotGridGradient(x0, y1, in.x, in.y);
    n1 = dotGridGradient(x1, y1, in.x, in.y);
    ix1 = interpolate(n0, n1, sx);

    value = interpolate(ix0, ix1, sy);
    return value; // Will return in range -1 to 1. To make it in range 0 to 1, multiply by 0.5 and add 0.5
}
