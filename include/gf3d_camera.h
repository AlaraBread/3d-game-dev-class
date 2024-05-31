#ifndef __GF3D_CAMERA_H__
#define __GF3D_CAMERA_H__

#include "gfc_matrix.h"

typedef struct
{
    GFC_Matrix4         cameraMat;      //final matrix to become the view matrix
    GFC_Matrix4         cameraMatInv;   //final matrix to become the inverse view matrix
    GFC_Vector3D        scale;          //for building camera matrix
    GFC_Vector3D        position;       //for building camera matrix
    GFC_Vector3D        rotation;       //pitch, roll, yaw for building camera matrix
    int                 autoPan;
    int                 freeLook;
    Uint8               cameraTargetLock;
    GFC_Vector3D        lookTargetPosition;
}Camera;

/**
 * @brief take the position,scale, and rotation to calculate the view matrix
 * @note: Do not use if you are tailoring the camera matrix by hand
 */
void gf3d_camera_update_view();

/**
 * @brief get the current camera view
 * @param view output, the matrix provided will be populated with the current camera information
 */
void gf3d_camera_get_view_mat4(GFC_Matrix4 *view);

/**
 * @brief set the current camera based on the matrix provided
 */
void gf3d_camera_set_view_mat4(GFC_Matrix4 *view);

/**
 * @brief have the camera point towards a point in space
 * @param target the point to look at
 * @param position [optional] if provided this will move the camera here first
 */
void gf3d_camera_look_at(GFC_Vector3D target,const GFC_Vector3D *position);

/**
 * @brief move the camera "FORWARD" relative to the view angles of the camera
 * @param magnitude how much to move
 */
void gf3d_camera_fly_forward(float magnitude);

/**
 * @brief move the camera "RIGHT" relative to the view angles of the camera
 * @param magnitude how much to move
 */
void gf3d_camera_fly_right(float magnitude);

/**
 * @brief move the camera "UP" relative to the view angles of the camera
 * @param magnitude how much to move
 */
void gf3d_camera_fly_up(float magnitude);

/**
 * @brief move the camera forward relative to the camera view angle
 * @note does not move along the z axis
 * @param magnitude how far to move
 */
void gf3d_camera_walk_forward(float magnitude);

/**
 * @brief move the camera right relative to the camera view angle
 * @note does not move along the z axis
 * @param magnitude how far to move
 */
void gf3d_camera_walk_right(float magnitude);

/**
 * @brief move the camera up along the z axis.
 * @note does not consider view angles.
 * @param magnitude how far to move
 */
void gf3d_camera_move_up(float magnitude);

/**
 * @brief rotate the camera's yaw
 * @param magnitude how far, in radians, to rotate
 */
void gf3d_camera_yaw(float magnitude);

/**
 * @brief rotate the camera's pitch
 * @param magnitude how far, in radians, to rotate
 */
void gf3d_camera_pitch(float magnitude);

/**
 * @brief rotate the camera's roll
 * @param magnitude how far, in radians, to rotate
 */
void gf3d_camera_roll(float magnitude);

/**
 * @brief move the camera "FORWARD" relative to the view angles of the camera
 * @param magnitude how much to move
 */
void gf3d_camera_fly_forward(float magnitude);

/**
 * @brief move the camera "RIGHT" relative to the view angles of the camera
 * @param magnitude how much to move
 */
void gf3d_camera_fly_right(float magnitude);

/**
 * @brief move the camera "UP" relative to the view angles of the camera
 * @param magnitude how much to move
 */
void gf3d_camera_fly_up(float magnitude);

/**
 * @brief move the camera forward relative to the camera view angle
 * @note does not move along the z axis
 * @param magnitude how far to move
 */
void gf3d_camera_walk_forward(float magnitude);

/**
 * @brief move the camera right relative to the camera view angle
 * @note does not move along the z axis
 * @param magnitude how far to move
 */
void gf3d_camera_walk_right(float magnitude);

/**
 * @brief move the camera up along the z axis.
 * @note does not consider view angles.
 * @param magnitude how far to move
 */
void gf3d_camera_move_up(float magnitude);

/**
 * @brief rotate the camera's yaw
 * @param magnitude how far, in radians, to rotate
 */
void gf3d_camera_yaw(float magnitude);

/**
 * @brief rotate the camera's pitch
 * @param magnitude how far, in radians, to rotate
 */
void gf3d_camera_pitch(float magnitude);

/**
 * @brief rotate the camera's roll
 * @param magnitude how far, in radians, to rotate
 */
void gf3d_camera_roll(float magnitude);

/**
 * @brief get the current camera position in world space;
 * @return the camera position.
 */
GFC_Vector3D gf3d_camera_get_position();


/**
 * @brief explicitely set the camera positon, holding all other parameters the same
 * @param position the new position for the camera
 */
void gf3d_camera_set_position(GFC_Vector3D position);

/**
 * @brief explicitely set the camera scale (to be applied to the entire scene)
 * @param scale the new scale for the camera
 */
void gf3d_camera_set_scale(GFC_Vector3D scale);

/**
 * @brief explicitely set the camera positon, holding all other parameters the same
 * @param rotation the new rotation for the camera (pitch[x], roll[y], yaw[z])
 */
void gf3d_camera_set_rotation(GFC_Vector3D rotation);

/**
 * @brief get the rotation angles of the camera that can be used with gfc_vector3d_angle_vectors()
 * @return the angle, in radians of the camera
 */
GFC_Vector3D gf3d_camera_get_angles();

/**
 * @brief get the gfc_vectors for forward, right, and up based on the camera view angles
 * @param forward   [output] if provided, it will be set with the normal gfc_vector in the direction the camera is looking
 * @param right     [output] if provided, it will be set with the normal gfc_vector to the right of where the camera is looking
 * @param up        [output] if provided, it will be set with the normal gfc_vector directly up from where the camera is looking
 */
void gf3d_camera_get_view_vectors(GFC_Vector3D *forward, GFC_Vector3D *right, GFC_Vector3D *up);

/**
 * @brief update the camera based on keyboard controls, autopan, and look target
 */
void gf3d_camera_controls_update();

/**
 * @brief set a look target for the camera to point at
 * @param target the position to target
 * @note it may still need to be enabled and you will need to call gf3d_camera_controls_update
 */
void gf3d_camera_set_look_target(GFC_Vector3D target);

/**
 * @brief get the look target for the camera
 * @return position of target
 */
GFC_Vector3D gf3d_camera_get_look_target();

/**
 * @brief toggle the current free look status
 */
void gf3d_camera_toggle_free_look();

/**
 * @brief turn on or off free look
 * @param enable if true then turn on, else turn off
 */
void gf3d_camera_enable_free_look(Uint8 enable);

/**
 * @brief turn on or off auto pan
 * @param enable if true then turn on, else turn off
 */
void gf3d_camera_set_auto_pan(Bool enable);

/**
 * @brief check if the camera currently has free look enabled
 * @return if it is
 */
Bool gf3d_camera_free_look_enabled();

/**
 * @brief set a look target for the camera
 */
void gf3d_camera_set_look_target(GFC_Vector3D target);

#endif
