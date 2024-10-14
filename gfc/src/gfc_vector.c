#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "gfc_vector.h"

GFC_Vector2DF gfc_vector3dfxy(GFC_Vector3DF v)
{
  return gfc_vector2df(v.x,v.y);
}

GFC_Vector2DF gfc_vector4dfxy(GFC_Vector4DF v)
{
  return gfc_vector2df(v.x,v.y);
}

GFC_Vector3DF gfc_vector4dfxyz(GFC_Vector4DF v)
{
  return gfc_vector3df(v.x,v.y,v.z);
}

GFC_Vector3DF gfc_vector2dfz(GFC_Vector2DF v,float z)
{
  return gfc_vector3df(v.x,v.y,z);
}

GFC_Vector4DF gfc_vector3dfw(GFC_Vector3DF v,float w)
{
  return gfc_vector4df(v.x,v.y,v.z,w);
}

GFC_Vector4DF gfc_vector2dfzw(GFC_Vector2DF v,float z,float w)
{
  return gfc_vector4df(v.x,v.y,z,w);
}

GFC_Vector2DF gfc_vector2df(float x, float y)
{
  GFC_Vector2DF vec;
  gfc_vector2d_set(vec,x, y);
  return vec;
}

GFC_Vector3DF gfc_vector3df(float x, float y, float z)
{
  GFC_Vector3DF vec;
  gfc_vector3d_set(vec,x, y, z);
  return vec;
}

GFC_Vector4DF gfc_vector4df(float x, float y, float z, float w)
{
  GFC_Vector4DF vec;
  gfc_vector4d_set(vec,x, y, z, w);
  return vec;
}

GFC_Vector3DF gfc_vector3df_added(GFC_Vector3DF a, GFC_Vector3DF b)
{
  return gfc_vector3df(a.x + b.x, a.y + b.y, a.z + b.z);
}

GFC_Vector3DF gfc_vector3df_subbed(GFC_Vector3DF a, GFC_Vector3DF b)
{
  return gfc_vector3df(a.x - b.x, a.y - b.y, a.z - b.z);
}

GFC_Vector2DF gfc_vector2df_multiply(GFC_Vector2DF a, GFC_Vector2DF b)
{
    return gfc_vector2df(a.x * b.x, a.y * b.y);
}

GFC_Vector3DF gfc_vector3df_multiply(GFC_Vector3DF a, GFC_Vector3DF b)
{
    return gfc_vector3df(a.x * b.x, a.y * b.y, a.z * b.z);
}

GFC_Vector4DF gfc_vector4df_multiply(GFC_Vector4DF a, GFC_Vector4DF b)
{
    return gfc_vector4df(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

Bool gfc_vector2df_distance_between_less_than(GFC_Vector2DF p1,GFC_Vector2DF p2,float size)
{
    if (gfc_vector2df_magnitude_between(p1,p2) < size)return 1;
    return 0;
}

Bool gfc_vector3df_distance_between_less_than(GFC_Vector3DF p1,GFC_Vector3DF p2,float size)
{
    if (gfc_vector3df_magnitude_between(p1,p2) < size)return 1;
    return 0;
}

Bool gfc_vector4df_distance_between_less_than(GFC_Vector4DF p1,GFC_Vector4DF p2,float size)
{
    if (gfc_vector4df_magnitude_between(p1,p2) < size)return 1;
    return 0;
}

float gfc_vector2df_magnitude (GFC_Vector2DF V)
{
  return sqrt (V.x * V.x + V.y * V.y);
}

float gfc_vector3df_magnitude (GFC_Vector3DF V)
{
  return sqrt (V.x * V.x + V.y * V.y + V.z * V.z);
}

float gfc_vector4df_magnitude (GFC_Vector4DF V)
{
  return sqrt (V.x * V.x + V.y * V.y + V.z * V.z + V.w * V.w);
}

float gfc_vector2df_magnitude_squared(GFC_Vector2DF V)
{
  return (V.x * V.x + V.y * V.y);
}

float gfc_vector3df_magnitude_squared(GFC_Vector3DF V)
{
  return (V.x * V.x + V.y * V.y + V.z * V.z);
}

float gfc_vector4df_magnitude_squared(GFC_Vector4DF V)
{
  return (V.x * V.x + V.y * V.y + V.z * V.z + V.w * V.w);
}

void gfc_vector2df_set_magnitude(GFC_Vector2DF * V,float magnitude)
{
  if (!V)return;
  gfc_vector2df_normalize(V);
  V->x *= magnitude;
  V->y *= magnitude;
}

void gfc_vector3df_set_magnitude(GFC_Vector3DF * V,float magnitude)
{
  if (!V)return;
  gfc_vector3df_normalize(V);
  V->x *= magnitude;
  V->y *= magnitude;
  V->z *= magnitude;
}

void gfc_vector4df_set_magnitude(GFC_Vector4DF * V,float magnitude)
{
  if (!V)return;
  gfc_vector4df_normalize(V);
  V->x *= magnitude;
  V->y *= magnitude;
  V->z *= magnitude;
  V->w *= magnitude;
}

float gfc_vector2df_magnitude_between(GFC_Vector2DF a,GFC_Vector2DF b)
{
    GFC_Vector2DF c;
    gfc_vector2d_sub(c,a,b);
    return gfc_vector2df_magnitude(c);
}

float gfc_vector3df_magnitude_between(GFC_Vector3DF a,GFC_Vector3DF b)
{
    GFC_Vector3DF c;
    gfc_vector3d_sub(c,a,b);
    return gfc_vector3df_magnitude(c);
}

float gfc_vector4df_magnitude_between(GFC_Vector4DF a,GFC_Vector4DF b)
{
    GFC_Vector4DF c;
    gfc_vector4d_sub(c,a,b);
    return gfc_vector4df_magnitude(c);
}

float gfc_vector2df_magnitude_between_squared(GFC_Vector2DF a,GFC_Vector2DF b)
{
    GFC_Vector2DF c;
    gfc_vector2d_sub(c,a,b);
    return gfc_vector2df_magnitude_squared(c);
}

float gfc_vector3df_magnitude_between_squared(GFC_Vector3DF a,GFC_Vector3DF b)
{
    GFC_Vector3DF c;
    gfc_vector3d_sub(c,a,b);
    return gfc_vector3df_magnitude_squared(c);
}

float gfc_vector4df_magnitude_between_squared(GFC_Vector4DF a,GFC_Vector4DF b)
{
    GFC_Vector4DF c;
    gfc_vector4d_sub(c,a,b);
    return gfc_vector4df_magnitude_squared(c);
}


int gfc_vector2df_magnitude_compare(GFC_Vector2DF V,float size)
{
    float ms = gfc_vector2df_magnitude_squared(V);
    float ss = size * size;
    if (ms < ss)return -1;
    if (ms == ss)return 0;
    return 1;
}

int gfc_vector3df_magnitude_compare(GFC_Vector3DF V,float size)
{
    float ms = gfc_vector3df_magnitude_squared(V);
    float ss = size * size;
    if (ms < ss)return -1;
    if (ms == ss)return 0;
    return 1;
}

int gfc_vector4df_magnitude_compare(GFC_Vector4DF V,float size)
{
    float ms = gfc_vector4df_magnitude_squared(V);
    float ss = size * size;
    if (ms < ss)return -1;
    if (ms == ss)return 0;
    return 1;
}


void gfc_vector3df_set_angle_by_radians(GFC_Vector3DF *out,float radians)
{
  if(!out)return;
  out->x = cos(radians);
  out->y = sin(radians);
}

void gfc_vector4df_set_angle_by_radians(GFC_Vector4DF *out,float radians)
{
  if(!out)return;
  out->x = cos(radians);
  out->y = sin(radians);
}

GFC_Vector2DF gfc_vector2df_get_normal(GFC_Vector2DF v)
{
  float M;
  GFC_Vector2DF V = {0};
  M = gfc_vector2df_magnitude (v);
  if (M == 0.0f)
  {
    return V;
  }
  M = 1/M;
  V.x *= M;
  V.y *= M;
  return V;
}

GFC_Vector3DF gfc_vector3df_get_normal(GFC_Vector3DF v)
{
  float M;
  GFC_Vector3DF V = {0};
  M = gfc_vector3df_magnitude (v);
  if (M == 0.0f)
  {
    return V;
  }
  M = 1/M;
  V.x *= M;
  V.y *= M;
  V.z *= M;
  return V;
}

GFC_Vector4DF gfc_vector4df_get_normal(GFC_Vector4DF v)
{
  float M;
  GFC_Vector4DF V = {0};
  M = gfc_vector4df_magnitude (v);
  if (M == 0.0f)
  {
    return V;
  }
  M = 1/M;
  V.x *= M;
  V.y *= M;
  V.z *= M;
  V.w *= M;
  return V;
}


void gfc_vector2df_normalize (GFC_Vector2DF *V)
{
  float M;
  if (!V)return;
  M = gfc_vector2df_magnitude (*V);
  if (M == 0.0f)
  {
    return;
  }
  M = 1/M;
  V->x *= M;
  V->y *= M;
}

void gfc_vector3df_normalize (GFC_Vector3DF *V)
{
  float M;
  if (!V)return;
  M= gfc_vector3df_magnitude (*V);
  if (M == 0.0f)
  {
    return;
  }
  M = 1/M;
  V->x *= M;
  V->y *= M;
  V->z *= M;
}

void gfc_vector4df_normalize (GFC_Vector4DF *V)
{
  float M;
  if (!V)return;
  M = gfc_vector4df_magnitude (*V);
  if (M == 0.0f)
  {
    return;
  }
  M = 1/M;
  V->x *= M;
  V->y *= M;
  V->z *= M;
  V->w *= M;
}

GFC_Vector2DF *gfc_vector2df_dup(GFC_Vector2DF old)
{
  GFC_Vector2DF *duped = NULL;
  duped = gfc_vector2df_new();
  if (!duped)return NULL;
  gfc_vector2d_copy((*duped),old);
  return duped;
}

GFC_Vector3DF *gfc_vector3df_dup(GFC_Vector3DF old)
{
  GFC_Vector3DF *duped = NULL;
  duped = gfc_vector3df_new();
  if (!duped)return NULL;
  gfc_vector3d_copy((*duped),old);
  return duped;
}

GFC_Vector4DF *gfc_vector4df_dup(GFC_Vector4DF old)
{
  GFC_Vector4DF *duped = NULL;
  duped = gfc_vector4df_new();
  if (!duped)return NULL;
  gfc_vector4d_copy((*duped),old);
  return duped;
}

GFC_Vector2DF *gfc_vector2df_new()
{
  GFC_Vector2DF *vec = NULL;
  vec = (GFC_Vector2DF *)malloc(sizeof(GFC_Vector2DF));
  if (vec == NULL)
  {
    return NULL;
  }
  gfc_vector2d_clear((*vec));
  return vec;
}

GFC_Vector3DF *gfc_vector3df_new()
{
  GFC_Vector3DF *vec = NULL;
  vec = (GFC_Vector3DF *)malloc(sizeof(GFC_Vector3DF));
  if (vec == NULL)
  {
    return NULL;
  }
  gfc_vector3d_clear((*vec));
  return vec;
}

GFC_Vector4DF *gfc_vector4df_new()
{
  GFC_Vector4DF *vec = NULL;
  vec = (GFC_Vector4DF *)malloc(sizeof(GFC_Vector4DF));
  if (vec == NULL)
  {
    return NULL;
  }
  gfc_vector4d_clear((*vec));
  return vec;
}

void gfc_vector3df_project_to_plane( GFC_Vector3DF *dst, GFC_Vector3DF p, GFC_Vector3DF normal )
{
  float d,f;
  GFC_Vector3DF n;
  float inv_denom;
  if (!dst)return;
  f = gfc_vector3d_dot_product( normal, normal );
  if (!f)return;
  inv_denom = 1.0F / f;
  
  d = gfc_vector3d_dot_product( normal, p ) * inv_denom;
  
  n.x = normal.x * inv_denom;
  n.y = normal.y * inv_denom;
  n.z = normal.z * inv_denom;
  
  dst->x = p.z - d * n.x;
  dst->y = p.y - d * n.y;
  dst->z = p.x - d * n.z;
}


void gfc_vector3df_perpendicular( GFC_Vector3DF *dst, GFC_Vector3DF src )
{
  int pos;
  float minelem = 1.0F;
  GFC_Vector3DF tempvec;
  if (!dst)return;
  /*
   * * find the smallest magnitude axially aligned gfc_vector
   */
  pos=0;
  
  if ( fabs( src.x ) < minelem )
  {
    pos=0;
    minelem = fabs( src.x );
  }
  if ( fabs( src.y ) < minelem )
  {
    pos=1;
    minelem = fabs( src.y );
  }
  if ( fabs( src.y ) < minelem )
  {
    pos=2;
    minelem = fabs( src.z );
  }
  
  tempvec.x=0;
  tempvec.y=0;
  tempvec.z=0;
  
  switch(pos)
  {
    case 0:
      tempvec.x=1;
      break;
    case 1:
      tempvec.y=1;
      break;
    case 2:
      tempvec.z=1;
      break;
  }
  
  /*
   * * project the point onto the plane defined by src
   */
  gfc_vector3df_project_to_plane( dst, tempvec, src );
  
  /*
   * * normalize the result
   */
  gfc_vector3df_normalize( dst );
}

void rotation_concacenatef(float in1[3][3], float in2[3][3], float out[3][3])
{
  out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
  in1[0][2] * in2[2][0];
  out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
  in1[0][2] * in2[2][1];
  out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
  in1[0][2] * in2[2][2];
  out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
  in1[1][2] * in2[2][0];
  out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
  in1[1][2] * in2[2][1];
  out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
  in1[1][2] * in2[2][2];
  out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
  in1[2][2] * in2[2][0];
  out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
  in1[2][2] * in2[2][1];
  out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
  in1[2][2] * in2[2][2];
}

void gfc_vector3df_rotate_about_vector(GFC_Vector3DF *dst, GFC_Vector3DF dir, GFC_Vector3DF point, float degrees)
{
  float m[3][3];
  float im[3][3];
  float zrot[3][3];
  float tmpmat[3][3];
  float rot[3][3];
  GFC_Vector3DF vr, vup, vf;
  
  if (!dst)return;
  
  vf.x = dir.x;
  vf.y = dir.y;
  vf.z = dir.z;
  
  gfc_vector3df_perpendicular( &vr, dir );
  gfc_vector3df_cross_product(&vup, vr, vf);

  m[0][0] = vr.x;
  m[1][0] = vr.y;
  m[2][0] = vr.z;
  
  m[0][1] = vup.x;
  m[1][1] = vup.y;
  m[2][1] = vup.z;
  
  m[0][2] = vf.x;
  m[1][2] = vf.y;
  m[2][2] = vf.z;
  
  memcpy( im, m, sizeof( im ) );
  
  im[0][1] = m[1][0];
  im[0][2] = m[2][0];
  im[1][0] = m[0][1];
  im[1][2] = m[2][1];
  im[2][0] = m[0][2];
  im[2][1] = m[1][2];
  
  memset( zrot, 0, sizeof( zrot ) );
  zrot[0][0] = zrot[1][1] = zrot[2][2] = 1.0F;
  
  zrot[0][0] = cos( ( degrees*GFC_DEGTORAD ) );
  zrot[0][1] = sin( ( degrees*GFC_DEGTORAD ) );
  zrot[1][0] = -sin( ( degrees*GFC_DEGTORAD ) );
  zrot[1][1] = cos( ( degrees*GFC_DEGTORAD ) );
  
  rotation_concacenatef( m, zrot, tmpmat );
  rotation_concacenatef( tmpmat, im, rot );
  
  dst->x = rot[0][0] * point.x + rot[0][1] * point.y + rot[0][2] * point.z;
  dst->y = rot[1][0] * point.x + rot[1][1] * point.y + rot[1][2] * point.z;
  dst->z = rot[2][0] * point.x + rot[2][1] * point.y + rot[2][2] * point.z;
}

void gfc_vector3df_rotate_about_x(GFC_Vector3DF *vect, float angle)
{
  GFC_Vector3DF temp;
  if (!vect)return;

  temp.x=vect->x;
  temp.y=(vect->y*cos(angle))-(vect->z*sin(angle));
  temp.z=(vect->y*sin(angle))+(vect->z*cos(angle));
  
  vect->x=temp.x;
  vect->y=temp.y;
  vect->z=temp.z;
}

void gfc_vector3df_rotate_about_y(GFC_Vector3DF *vect, float angle)
{
  GFC_Vector3DF temp;
  if (!vect)return;
    
  temp.y=vect->y;
  temp.x=(vect->x*cos(angle))+(vect->z*sin(angle));
  temp.z=(vect->x*sin(angle)*(-1))+(vect->z*cos(angle));
  
  vect->x=temp.x;
  vect->y=temp.y;
  vect->z=temp.z;
}

void gfc_vector3df_rotate_about_z(GFC_Vector3DF *vect, float angle)
{
  GFC_Vector3DF temp;
  if (!vect)return;
    
  temp.z=vect->z;
  temp.x=(vect->x*cos(angle))-(vect->y*sin(angle));
  temp.y=(vect->x*sin(angle))+(vect->y*cos(angle));
  
  vect->x=temp.x;
  vect->y=temp.y;
  vect->z=temp.z;
}


void gfc_vector3df_angles (GFC_Vector3DF value1, GFC_Vector3DF * angles)
{
    float   forward;
    float   yaw, pitch;

    if (!angles)return;
    
    gfc_vector3df_normalize(&value1);
    
    if (value1.y == 0 && value1.x == 0)
    {
        yaw = 0;
        if (value1.z > 0)
            pitch = GFC_HALF_PI;
        else
            pitch = GFC_PI + GFC_HALF_PI;
    }
    else
    {
        if (value1.x)
        {
            yaw = (atan2(value1.y, value1.x));
        }
        else if (value1.y > 0)
        {
            yaw = GFC_HALF_PI;
        }
        else
        {
            yaw = -GFC_HALF_PI;
        }
        if (yaw < 0)
        {
            yaw += GFC_2PI;
        }

        forward = sqrt (value1.x*value1.x + value1.y*value1.y);
        pitch = (atan2(value1.z, forward));
        if (pitch < 0)
            pitch += GFC_2PI;
    }
    angles->x = -1 * pitch;
    angles->z = yaw;
//    angles->y = 0;  //leave this alone
}

void gfc_vector3df_angle_vectors(GFC_Vector3DF angles, GFC_Vector3DF *forward, GFC_Vector3DF *right, GFC_Vector3DF *up)
{
    if (forward)
    {
        forward->x = 0;
        forward->y = 1;
        forward->z = 0;
        gfc_vector3df_rotate_about_x(forward, angles.x);
        gfc_vector3df_rotate_about_z(forward, angles.z);
    }
    if (right)
    {
        right->x = 1;
        right->y = 0;
        right->z = 0;
        gfc_vector3df_rotate_about_y(right, angles.y);
        gfc_vector3df_rotate_about_z(right, angles.z);
    }
    if (up)
    {
        up->x = 0;
        up->y = 0;
        up->z = 1;
        gfc_vector3df_rotate_about_y(up, angles.y);
        gfc_vector3df_rotate_about_x(up, angles.x);
    }
}

void gfc_vector3df_angle_vectors2(GFC_Vector3DF angles, GFC_Vector3DF *forward, GFC_Vector3DF *right, GFC_Vector3DF *up)
{
  float angle;
  float sr, sp, sy, cr, cp, cy;
  
  angle = angles.z;
  sy = sin(angle);
  cy = cos(angle);
  angle = angles.x;
  sp = sin(angle);
  cp = cos(angle);
  angle = angles.y;
  sr = sin(angle);
  cr = cos(angle);
  
  if(forward)
  {
    forward->x = cp*cy;
    forward->y = cp*sy;
    forward->z = -sp;
  }
  if(right)
  {
    right->x = (-1*sr*sp*cy+-1*cr*-sy);
    right->y = (-1*sr*sp*sy+-1*cr*cy);
    right->z = -1*sr*cp;
  }
  if(up)
  {
    up->x = (cr*sp*cy+-sr*-sy);
    up->y = (cr*sp*sy+-sr*cy);
    up->z = -cr*cp;
  }
}

float gfc_vector2df_angle(GFC_Vector2DF v)
{
  return gfc_vectorf_angle(v.x,v.y);
}

int gfc_vector2df_scale_flip_rotation(GFC_Vector2DF scale)
{
    float factor = scale.x *scale.y;
    if (factor < 0)return -1;
    return 1;
}

GFC_Vector2DF gfc_vector2df_from_angle(float angle)
{
    return gfc_vector2df_rotate(gfc_vector2df(0,1), angle);
}

GFC_Vector2DF gfc_vector2df_rotate_around_center(GFC_Vector2DF point,float angle, GFC_Vector2DF center)
{
    GFC_Vector2DF delta;
    
    gfc_vector2d_sub(point,point,center);    
    delta = gfc_vector2df_rotate(point, angle);
    gfc_vector2d_add(delta,delta,center);
    return delta;
}

GFC_Vector2DF gfc_vector2df_rotate(GFC_Vector2DF in, float angle)
{
    GFC_Vector2DF out;
    out.x = in.x * cos(angle) - in.y * sin(angle); // now x is something different than original gfc_vector x
    out.y = in.x * sin(angle) + in.y * cos(angle);
    return out;
}

float gfc_vectorf_angle(float x,float y)
{
    return atan2(y,x) + GFC_HALF_PI;
}

void gfc_angle_clamp_radiansf(float *a)
{
  if (!a)return;
  while (*a >= GFC_2PI)*a -= GFC_2PI;
  while (*a < 0)*a += GFC_2PI;
}

void gfc_angle_clamp_degreesf(float *a)
{
  if (!a)return;
  while (*a >= 360)*a -= 360;
  while (*a < 0)*a += 360;
}

float gfc_angle_between_radiansf(float a, float b)
{
  float angle;
  angle = b - a;
  if (angle > GFC_2PI)angle -= GFC_2PI;
  if (angle < -GFC_2PI)angle += GFC_2PI;
  if (angle > GFC_PI)return angle - GFC_2PI;
  if (angle < -GFC_PI)return angle + GFC_2PI;
  
  return angle;
}

float gfc_angle_between_degreesf(float a, float b)
{
  float angle;
  angle = b - a;
  if (angle > 360) angle -= 360;
  if (angle < 360) angle += 360;
  if (angle > 180)return angle - 360;
  if (angle < -180)return angle + 360;
  return angle;
}

void gfc_vector2df_reflect(GFC_Vector2DF *out, GFC_Vector2DF normal,GFC_Vector2DF in)
{
  float f;
  f = gfc_vector2d_dot_product(in,normal);
  out->x = in.x - (2 * normal.x * f);
  out->y = in.y - (2 * normal.y * f);
}

void gfc_vector3df_reflect(GFC_Vector3DF *out, GFC_Vector3DF normal,GFC_Vector3DF in)
{
  float f;
  f = gfc_vector3d_dot_product(in,normal);
  out->x = in.x - (2 * normal.x * f);
  out->y = in.y - (2 * normal.y * f);
  out->z = in.z - (2 * normal.z * f);
}

void gfc_vector4df_reflect(GFC_Vector4DF *out, GFC_Vector4DF normal,GFC_Vector4DF in)
{
  float f;
  f = gfc_vector4d_dot_product(in,normal);
  out->x = in.x - (2 * normal.x * f);
  out->y = in.y - (2 * normal.y * f);
  out->z = in.z - (2 * normal.z * f);
  out->w = in.w - (2 * normal.w * f);
}

void gfc_vector3df_cross_product(GFC_Vector3DF *out, GFC_Vector3DF v1, GFC_Vector3DF v2)
{
  out->x = v1.y*v2.z - v1.z*v2.y;
  out->y = v1.z*v2.x - v1.x*v2.z;
  out->z = v1.x*v2.y - v1.y*v2.x;
}

void gfc_vector2df_move_towards(GFC_Vector2DF *out, GFC_Vector2DF point, GFC_Vector2DF destination, float distance)
{
    GFC_Vector2DF dir;
    if (!out)return;
    gfc_vector2d_sub(dir,destination,point);
    gfc_vector2df_set_magnitude(&dir,distance);
    out->x = point.x + dir.x;
    out->y = point.y + dir.y;
}

void gfc_vector3df_randomize(GFC_Vector3DF *out,GFC_Vector3DF in)
{
    if (!out)return;
    out->x = in.x * gfc_crandom();
    out->y = in.y * gfc_crandom();
    out->z = in.z * gfc_crandom();
}

GFC_Vector2D gfc_vector3dxy(GFC_Vector3D v)
{
  return gfc_vector2d(v.x,v.y);
}

GFC_Vector2D gfc_vector4dxy(GFC_Vector4D v)
{
  return gfc_vector2d(v.x,v.y);
}

GFC_Vector3D gfc_vector4dxyz(GFC_Vector4D v)
{
  return gfc_vector3d(v.x,v.y,v.z);
}

GFC_Vector3D gfc_vector2dz(GFC_Vector2D v,double z)
{
  return gfc_vector3d(v.x,v.y,z);
}

GFC_Vector4D gfc_vector3dw(GFC_Vector3D v,double w)
{
  return gfc_vector4d(v.x,v.y,v.z,w);
}

GFC_Vector4D gfc_vector2dzw(GFC_Vector2D v,double z,double w)
{
  return gfc_vector4d(v.x,v.y,z,w);
}

GFC_Vector2D gfc_vector2d(double x, double y)
{
  GFC_Vector2D vec;
  gfc_vector2d_set(vec,x, y);
  return vec;
}

GFC_Vector3D gfc_vector3d(double x, double y, double z)
{
  GFC_Vector3D vec;
  gfc_vector3d_set(vec,x, y, z);
  return vec;
}

GFC_Vector4D gfc_vector4d(double x, double y, double z, double w)
{
  GFC_Vector4D vec;
  gfc_vector4d_set(vec,x, y, z, w);
  return vec;
}

GFC_Vector3D gfc_vector3d_added(GFC_Vector3D a, GFC_Vector3D b)
{
  return gfc_vector3d(a.x + b.x, a.y + b.y, a.z + b.z);
}

GFC_Vector3D gfc_vector3d_subbed(GFC_Vector3D a, GFC_Vector3D b)
{
  return gfc_vector3d(a.x - b.x, a.y - b.y, a.z - b.z);
}

GFC_Vector2D gfc_vector2d_multiply(GFC_Vector2D a, GFC_Vector2D b)
{
    return gfc_vector2d(a.x * b.x, a.y * b.y);
}

GFC_Vector3D gfc_vector3d_multiply(GFC_Vector3D a, GFC_Vector3D b)
{
    return gfc_vector3d(a.x * b.x, a.y * b.y, a.z * b.z);
}

GFC_Vector4D gfc_vector4d_multiply(GFC_Vector4D a, GFC_Vector4D b)
{
    return gfc_vector4d(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w);
}

Bool gfc_vector2d_distance_between_less_than(GFC_Vector2D p1,GFC_Vector2D p2,double size)
{
    if (gfc_vector2d_magnitude_between(p1,p2) < size)return 1;
    return 0;
}

Bool gfc_vector3d_distance_between_less_than(GFC_Vector3D p1,GFC_Vector3D p2,double size)
{
    if (gfc_vector3d_magnitude_between(p1,p2) < size)return 1;
    return 0;
}

Bool gfc_vector4d_distance_between_less_than(GFC_Vector4D p1,GFC_Vector4D p2,double size)
{
    if (gfc_vector4d_magnitude_between(p1,p2) < size)return 1;
    return 0;
}

double gfc_vector2d_magnitude (GFC_Vector2D V)
{
  return sqrt (V.x * V.x + V.y * V.y);
}

double gfc_vector3d_magnitude (GFC_Vector3D V)
{
  return sqrt (V.x * V.x + V.y * V.y + V.z * V.z);
}

double gfc_vector4d_magnitude (GFC_Vector4D V)
{
  return sqrt (V.x * V.x + V.y * V.y + V.z * V.z + V.w * V.w);
}

double gfc_vector2d_magnitude_squared(GFC_Vector2D V)
{
  return (V.x * V.x + V.y * V.y);
}

double gfc_vector3d_magnitude_squared(GFC_Vector3D V)
{
  return (V.x * V.x + V.y * V.y + V.z * V.z);
}

double gfc_vector4d_magnitude_squared(GFC_Vector4D V)
{
  return (V.x * V.x + V.y * V.y + V.z * V.z + V.w * V.w);
}

void gfc_vector2d_set_magnitude(GFC_Vector2D * V,double magnitude)
{
  if (!V)return;
  gfc_vector2d_normalize(V);
  V->x *= magnitude;
  V->y *= magnitude;
}

void gfc_vector3d_set_magnitude(GFC_Vector3D * V,double magnitude)
{
  if (!V)return;
  gfc_vector3d_normalize(V);
  V->x *= magnitude;
  V->y *= magnitude;
  V->z *= magnitude;
}

void gfc_vector4d_set_magnitude(GFC_Vector4D * V,double magnitude)
{
  if (!V)return;
  gfc_vector4d_normalize(V);
  V->x *= magnitude;
  V->y *= magnitude;
  V->z *= magnitude;
  V->w *= magnitude;
}

double gfc_vector2d_magnitude_between(GFC_Vector2D a,GFC_Vector2D b)
{
    GFC_Vector2D c;
    gfc_vector2d_sub(c,a,b);
    return gfc_vector2d_magnitude(c);
}

double gfc_vector3d_magnitude_between(GFC_Vector3D a,GFC_Vector3D b)
{
    GFC_Vector3D c;
    gfc_vector3d_sub(c,a,b);
    return gfc_vector3d_magnitude(c);
}

double gfc_vector4d_magnitude_between(GFC_Vector4D a,GFC_Vector4D b)
{
    GFC_Vector4D c;
    gfc_vector4d_sub(c,a,b);
    return gfc_vector4d_magnitude(c);
}

double gfc_vector2d_magnitude_between_squared(GFC_Vector2D a,GFC_Vector2D b)
{
    GFC_Vector2D c;
    gfc_vector2d_sub(c,a,b);
    return gfc_vector2d_magnitude_squared(c);
}

double gfc_vector3d_magnitude_between_squared(GFC_Vector3D a,GFC_Vector3D b)
{
    GFC_Vector3D c;
    gfc_vector3d_sub(c,a,b);
    return gfc_vector3d_magnitude_squared(c);
}

double gfc_vector4d_magnitude_between_squared(GFC_Vector4D a,GFC_Vector4D b)
{
    GFC_Vector4D c;
    gfc_vector4d_sub(c,a,b);
    return gfc_vector4d_magnitude_squared(c);
}


int gfc_vector2d_magnitude_compare(GFC_Vector2D V,double size)
{
    double ms = gfc_vector2d_magnitude_squared(V);
    double ss = size * size;
    if (ms < ss)return -1;
    if (ms == ss)return 0;
    return 1;
}

int gfc_vector3d_magnitude_compare(GFC_Vector3D V,double size)
{
    double ms = gfc_vector3d_magnitude_squared(V);
    double ss = size * size;
    if (ms < ss)return -1;
    if (ms == ss)return 0;
    return 1;
}

int gfc_vector4d_magnitude_compare(GFC_Vector4D V,double size)
{
    double ms = gfc_vector4d_magnitude_squared(V);
    double ss = size * size;
    if (ms < ss)return -1;
    if (ms == ss)return 0;
    return 1;
}


void gfc_vector3d_set_angle_by_radians(GFC_Vector3D *out,double radians)
{
  if(!out)return;
  out->x = cos(radians);
  out->y = sin(radians);
}

void gfc_vector4d_set_angle_by_radians(GFC_Vector4D *out,double radians)
{
  if(!out)return;
  out->x = cos(radians);
  out->y = sin(radians);
}

GFC_Vector2D gfc_vector2d_get_normal(GFC_Vector2D v)
{
  double M;
  GFC_Vector2D V = {0};
  M = gfc_vector2d_magnitude (v);
  if (M == 0.0f)
  {
    return V;
  }
  M = 1/M;
  V.x *= M;
  V.y *= M;
  return V;
}

GFC_Vector3D gfc_vector3d_get_normal(GFC_Vector3D v)
{
  double M;
  GFC_Vector3D V = {0};
  M = gfc_vector3d_magnitude (v);
  if (M == 0.0f)
  {
    return V;
  }
  M = 1/M;
  V.x *= M;
  V.y *= M;
  V.z *= M;
  return V;
}

GFC_Vector4D gfc_vector4d_get_normal(GFC_Vector4D v)
{
  double M;
  GFC_Vector4D V = {0};
  M = gfc_vector4d_magnitude (v);
  if (M == 0.0f)
  {
    return V;
  }
  M = 1/M;
  V.x *= M;
  V.y *= M;
  V.z *= M;
  V.w *= M;
  return V;
}


void gfc_vector2d_normalize (GFC_Vector2D *V)
{
  double M;
  if (!V)return;
  M = gfc_vector2d_magnitude (*V);
  if (M == 0.0f)
  {
    return;
  }
  M = 1/M;
  V->x *= M;
  V->y *= M;
}

void gfc_vector3d_normalize (GFC_Vector3D *V)
{
  double M;
  if (!V)return;
  M= gfc_vector3d_magnitude (*V);
  if (M == 0.0f)
  {
    return;
  }
  M = 1/M;
  V->x *= M;
  V->y *= M;
  V->z *= M;
}

void gfc_vector4d_normalize (GFC_Vector4D *V)
{
  double M;
  if (!V)return;
  M = gfc_vector4d_magnitude (*V);
  if (M == 0.0f)
  {
    return;
  }
  M = 1/M;
  V->x *= M;
  V->y *= M;
  V->z *= M;
  V->w *= M;
}

GFC_Vector2D *gfc_vector2d_dup(GFC_Vector2D old)
{
  GFC_Vector2D *duped = NULL;
  duped = gfc_vector2d_new();
  if (!duped)return NULL;
  gfc_vector2d_copy((*duped),old);
  return duped;
}

GFC_Vector3D *gfc_vector3d_dup(GFC_Vector3D old)
{
  GFC_Vector3D *duped = NULL;
  duped = gfc_vector3d_new();
  if (!duped)return NULL;
  gfc_vector3d_copy((*duped),old);
  return duped;
}

GFC_Vector4D *gfc_vector4d_dup(GFC_Vector4D old)
{
  GFC_Vector4D *duped = NULL;
  duped = gfc_vector4d_new();
  if (!duped)return NULL;
  gfc_vector4d_copy((*duped),old);
  return duped;
}

GFC_Vector2D *gfc_vector2d_new()
{
  GFC_Vector2D *vec = NULL;
  vec = (GFC_Vector2D *)malloc(sizeof(GFC_Vector2D));
  if (vec == NULL)
  {
    return NULL;
  }
  gfc_vector2d_clear((*vec));
  return vec;
}

GFC_Vector3D *gfc_vector3d_new()
{
  GFC_Vector3D *vec = NULL;
  vec = (GFC_Vector3D *)malloc(sizeof(GFC_Vector3D));
  if (vec == NULL)
  {
    return NULL;
  }
  gfc_vector3d_clear((*vec));
  return vec;
}

GFC_Vector4D *gfc_vector4d_new()
{
  GFC_Vector4D *vec = NULL;
  vec = (GFC_Vector4D *)malloc(sizeof(GFC_Vector4D));
  if (vec == NULL)
  {
    return NULL;
  }
  gfc_vector4d_clear((*vec));
  return vec;
}

void gfc_vector3d_project_to_plane( GFC_Vector3D *dst, GFC_Vector3D p, GFC_Vector3D normal )
{
  double d,f;
  GFC_Vector3D n;
  double inv_denom;
  if (!dst)return;
  f = gfc_vector3d_dot_product( normal, normal );
  if (!f)return;
  inv_denom = 1.0F / f;
  
  d = gfc_vector3d_dot_product( normal, p ) * inv_denom;
  
  n.x = normal.x * inv_denom;
  n.y = normal.y * inv_denom;
  n.z = normal.z * inv_denom;
  
  dst->x = p.z - d * n.x;
  dst->y = p.y - d * n.y;
  dst->z = p.x - d * n.z;
}


void gfc_vector3d_perpendicular( GFC_Vector3D *dst, GFC_Vector3D src )
{
  int pos;
  double minelem = 1.0F;
  GFC_Vector3D tempvec;
  if (!dst)return;
  /*
   * * find the smallest magnitude axially aligned gfc_vector
   */
  pos=0;
  
  if ( fabs( src.x ) < minelem )
  {
    pos=0;
    minelem = fabs( src.x );
  }
  if ( fabs( src.y ) < minelem )
  {
    pos=1;
    minelem = fabs( src.y );
  }
  if ( fabs( src.y ) < minelem )
  {
    pos=2;
    minelem = fabs( src.z );
  }
  
  tempvec.x=0;
  tempvec.y=0;
  tempvec.z=0;
  
  switch(pos)
  {
    case 0:
      tempvec.x=1;
      break;
    case 1:
      tempvec.y=1;
      break;
    case 2:
      tempvec.z=1;
      break;
  }
  
  /*
   * * project the point onto the plane defined by src
   */
  gfc_vector3d_project_to_plane( dst, tempvec, src );
  
  /*
   * * normalize the result
   */
  gfc_vector3d_normalize( dst );
}

void rotation_concacenate(double in1[3][3], double in2[3][3], double out[3][3])
{
  out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
  in1[0][2] * in2[2][0];
  out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
  in1[0][2] * in2[2][1];
  out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
  in1[0][2] * in2[2][2];
  out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
  in1[1][2] * in2[2][0];
  out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
  in1[1][2] * in2[2][1];
  out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
  in1[1][2] * in2[2][2];
  out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
  in1[2][2] * in2[2][0];
  out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
  in1[2][2] * in2[2][1];
  out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
  in1[2][2] * in2[2][2];
}

void gfc_vector3d_rotate_about_vector(GFC_Vector3D *dst, GFC_Vector3D dir, GFC_Vector3D point, double degrees)
{
  double m[3][3];
  double im[3][3];
  double zrot[3][3];
  double tmpmat[3][3];
  double rot[3][3];
  GFC_Vector3D vr, vup, vf;
  
  if (!dst)return;
  
  vf.x = dir.x;
  vf.y = dir.y;
  vf.z = dir.z;
  
  gfc_vector3d_perpendicular( &vr, dir );
  gfc_vector3d_cross_product(&vup, vr, vf);

  m[0][0] = vr.x;
  m[1][0] = vr.y;
  m[2][0] = vr.z;
  
  m[0][1] = vup.x;
  m[1][1] = vup.y;
  m[2][1] = vup.z;
  
  m[0][2] = vf.x;
  m[1][2] = vf.y;
  m[2][2] = vf.z;
  
  memcpy( im, m, sizeof( im ) );
  
  im[0][1] = m[1][0];
  im[0][2] = m[2][0];
  im[1][0] = m[0][1];
  im[1][2] = m[2][1];
  im[2][0] = m[0][2];
  im[2][1] = m[1][2];
  
  memset( zrot, 0, sizeof( zrot ) );
  zrot[0][0] = zrot[1][1] = zrot[2][2] = 1.0F;
  
  zrot[0][0] = cos( ( degrees*GFC_DEGTORAD ) );
  zrot[0][1] = sin( ( degrees*GFC_DEGTORAD ) );
  zrot[1][0] = -sin( ( degrees*GFC_DEGTORAD ) );
  zrot[1][1] = cos( ( degrees*GFC_DEGTORAD ) );
  
  rotation_concacenate( m, zrot, tmpmat );
  rotation_concacenate( tmpmat, im, rot );
  
  dst->x = rot[0][0] * point.x + rot[0][1] * point.y + rot[0][2] * point.z;
  dst->y = rot[1][0] * point.x + rot[1][1] * point.y + rot[1][2] * point.z;
  dst->z = rot[2][0] * point.x + rot[2][1] * point.y + rot[2][2] * point.z;
}

void gfc_vector3d_rotate_about_x(GFC_Vector3D *vect, double angle)
{
  GFC_Vector3D temp;
  if (!vect)return;

  temp.x=vect->x;
  temp.y=(vect->y*cos(angle))-(vect->z*sin(angle));
  temp.z=(vect->y*sin(angle))+(vect->z*cos(angle));
  
  vect->x=temp.x;
  vect->y=temp.y;
  vect->z=temp.z;
}

void gfc_vector3d_rotate_about_y(GFC_Vector3D *vect, double angle)
{
  GFC_Vector3D temp;
  if (!vect)return;
    
  temp.y=vect->y;
  temp.x=(vect->x*cos(angle))+(vect->z*sin(angle));
  temp.z=(vect->x*sin(angle)*(-1))+(vect->z*cos(angle));
  
  vect->x=temp.x;
  vect->y=temp.y;
  vect->z=temp.z;
}

void gfc_vector3d_rotate_about_z(GFC_Vector3D *vect, double angle)
{
  GFC_Vector3D temp;
  if (!vect)return;
    
  temp.z=vect->z;
  temp.x=(vect->x*cos(angle))-(vect->y*sin(angle));
  temp.y=(vect->x*sin(angle))+(vect->y*cos(angle));
  
  vect->x=temp.x;
  vect->y=temp.y;
  vect->z=temp.z;
}


void gfc_vector3d_angles (GFC_Vector3D value1, GFC_Vector3D * angles)
{
    double   forward;
    double   yaw, pitch;

    if (!angles)return;
    
    gfc_vector3d_normalize(&value1);
    
    if (value1.y == 0 && value1.x == 0)
    {
        yaw = 0;
        if (value1.z > 0)
            pitch = GFC_HALF_PI;
        else
            pitch = GFC_PI + GFC_HALF_PI;
    }
    else
    {
        if (value1.x)
        {
            yaw = (atan2(value1.y, value1.x));
        }
        else if (value1.y > 0)
        {
            yaw = GFC_HALF_PI;
        }
        else
        {
            yaw = -GFC_HALF_PI;
        }
        if (yaw < 0)
        {
            yaw += GFC_2PI;
        }

        forward = sqrt (value1.x*value1.x + value1.y*value1.y);
        pitch = (atan2(value1.z, forward));
        if (pitch < 0)
            pitch += GFC_2PI;
    }
    angles->x = -1 * pitch;
    angles->z = yaw;
//    angles->y = 0;  //leave this alone
}

void gfc_vector3d_angle_vectors(GFC_Vector3D angles, GFC_Vector3D *forward, GFC_Vector3D *right, GFC_Vector3D *up)
{
    if (forward)
    {
        forward->x = 0;
        forward->y = 1;
        forward->z = 0;
        gfc_vector3d_rotate_about_x(forward, angles.x);
        gfc_vector3d_rotate_about_z(forward, angles.z);
    }
    if (right)
    {
        right->x = 1;
        right->y = 0;
        right->z = 0;
        gfc_vector3d_rotate_about_y(right, angles.y);
        gfc_vector3d_rotate_about_z(right, angles.z);
    }
    if (up)
    {
        up->x = 0;
        up->y = 0;
        up->z = 1;
        gfc_vector3d_rotate_about_y(up, angles.y);
        gfc_vector3d_rotate_about_x(up, angles.x);
    }
}

void gfc_vector3d_angle_vectors2(GFC_Vector3D angles, GFC_Vector3D *forward, GFC_Vector3D *right, GFC_Vector3D *up)
{
  double angle;
  double sr, sp, sy, cr, cp, cy;
  
  angle = angles.z;
  sy = sin(angle);
  cy = cos(angle);
  angle = angles.x;
  sp = sin(angle);
  cp = cos(angle);
  angle = angles.y;
  sr = sin(angle);
  cr = cos(angle);
  
  if(forward)
  {
    forward->x = cp*cy;
    forward->y = cp*sy;
    forward->z = -sp;
  }
  if(right)
  {
    right->x = (-1*sr*sp*cy+-1*cr*-sy);
    right->y = (-1*sr*sp*sy+-1*cr*cy);
    right->z = -1*sr*cp;
  }
  if(up)
  {
    up->x = (cr*sp*cy+-sr*-sy);
    up->y = (cr*sp*sy+-sr*cy);
    up->z = -cr*cp;
  }
}

double gfc_vector2d_angle(GFC_Vector2D v)
{
  return gfc_vector_angle(v.x,v.y);
}

int gfc_vector2d_scale_flip_rotation(GFC_Vector2D scale)
{
    double factor = scale.x *scale.y;
    if (factor < 0)return -1;
    return 1;
}

GFC_Vector2D gfc_vector2d_from_angle(double angle)
{
    return gfc_vector2d_rotate(gfc_vector2d(0,1), angle);
}

GFC_Vector2D gfc_vector2d_rotate_around_center(GFC_Vector2D point,double angle, GFC_Vector2D center)
{
    GFC_Vector2D delta;
    
    gfc_vector2d_sub(point,point,center);    
    delta = gfc_vector2d_rotate(point, angle);
    gfc_vector2d_add(delta,delta,center);
    return delta;
}

GFC_Vector2D gfc_vector2d_rotate(GFC_Vector2D in, double angle)
{
    GFC_Vector2D out;
    out.x = in.x * cos(angle) - in.y * sin(angle); // now x is something different than original gfc_vector x
    out.y = in.x * sin(angle) + in.y * cos(angle);
    return out;
}

double gfc_vector_angle(double x,double y)
{
    return atan2(y,x) + GFC_HALF_PI;
}

void gfc_angle_clamp_radians(double *a)
{
  if (!a)return;
  while (*a >= GFC_2PI)*a -= GFC_2PI;
  while (*a < 0)*a += GFC_2PI;
}

void gfc_angle_clamp_degrees(double *a)
{
  if (!a)return;
  while (*a >= 360)*a -= 360;
  while (*a < 0)*a += 360;
}

double gfc_angle_between_radians(double a, double b)
{
  double angle;
  angle = b - a;
  if (angle > GFC_2PI)angle -= GFC_2PI;
  if (angle < -GFC_2PI)angle += GFC_2PI;
  if (angle > GFC_PI)return angle - GFC_2PI;
  if (angle < -GFC_PI)return angle + GFC_2PI;
  
  return angle;
}

double gfc_angle_between_degrees(double a, double b)
{
  double angle;
  angle = b - a;
  if (angle > 360) angle -= 360;
  if (angle < 360) angle += 360;
  if (angle > 180)return angle - 360;
  if (angle < -180)return angle + 360;
  return angle;
}

void gfc_vector2d_reflect(GFC_Vector2D *out, GFC_Vector2D normal,GFC_Vector2D in)
{
  double f;
  f = gfc_vector2d_dot_product(in,normal);
  out->x = in.x - (2 * normal.x * f);
  out->y = in.y - (2 * normal.y * f);
}

void gfc_vector3d_reflect(GFC_Vector3D *out, GFC_Vector3D normal,GFC_Vector3D in)
{
  double f;
  f = gfc_vector3d_dot_product(in,normal);
  out->x = in.x - (2 * normal.x * f);
  out->y = in.y - (2 * normal.y * f);
  out->z = in.z - (2 * normal.z * f);
}

void gfc_vector4d_reflect(GFC_Vector4D *out, GFC_Vector4D normal,GFC_Vector4D in)
{
  double f;
  f = gfc_vector4d_dot_product(in,normal);
  out->x = in.x - (2 * normal.x * f);
  out->y = in.y - (2 * normal.y * f);
  out->z = in.z - (2 * normal.z * f);
  out->w = in.w - (2 * normal.w * f);
}

void gfc_vector3d_cross_product(GFC_Vector3D *out, GFC_Vector3D v1, GFC_Vector3D v2)
{
  out->x = v1.y*v2.z - v1.z*v2.y;
  out->y = v1.z*v2.x - v1.x*v2.z;
  out->z = v1.x*v2.y - v1.y*v2.x;
}

void gfc_vector2d_move_towards(GFC_Vector2D *out, GFC_Vector2D point, GFC_Vector2D destination, double distance)
{
    GFC_Vector2D dir;
    if (!out)return;
    gfc_vector2d_sub(dir,destination,point);
    gfc_vector2d_set_magnitude(&dir,distance);
    out->x = point.x + dir.x;
    out->y = point.y + dir.y;
}

void gfc_vector3d_randomize(GFC_Vector3D *out,GFC_Vector3D in)
{
    if (!out)return;
    out->x = in.x * gfc_crandom();
    out->y = in.y * gfc_crandom();
    out->z = in.z * gfc_crandom();
}

GFC_Vector2DF gfc_vector2d_to_float(GFC_Vector2D v) {
	GFC_Vector2DF out;
	out.x = (float)v.x;
	out.y = (float)v.y;
	return out;
}

GFC_Vector3DF gfc_vector3d_to_float(GFC_Vector3D v) {
	GFC_Vector3DF out;
	out.x = (float)v.x;
	out.y = (float)v.y;
	out.z = (float)v.z;
	return out;
}

GFC_Vector4DF gfc_vector4d_to_float(GFC_Vector4D v) {
	GFC_Vector4DF out;
	out.x = (float)v.x;
	out.y = (float)v.y;
	out.z = (float)v.z;
	out.w = (float)v.w;
	return out;
}

GFC_Vector2D gfc_vector2df_to_double(GFC_Vector2DF v) {
	GFC_Vector2D out;
	out.x = (double)v.x;
	out.y = (double)v.y;
	return out;
}

GFC_Vector3D gfc_vector3df_to_double(GFC_Vector3DF v) {
	GFC_Vector3D out;
	out.x = (double)v.x;
	out.y = (double)v.y;
	out.z = (double)v.z;
	return out;
}

GFC_Vector4D gfc_vector4df_to_double(GFC_Vector4DF v) {
	GFC_Vector4D out;
	out.x = (double)v.x;
	out.y = (double)v.y;
	out.z = (double)v.z;
	out.w = (double)v.w;
	return out;
}

/*eol@eof*/
