#ifndef __GF3D_MESH_H__
#define __GF3D_MESH_H__

#include <stdalign.h>
#include <vulkan/vulkan.h>

#include "gfc_vector.h"
#include "gfc_list.h"
#include "gfc_text.h"
#include "gfc_matrix.h"
#include "gfc_primitives.h"

#include "gf3d_pipeline.h"


//absolute basics of the mesh information sent to the graphics card
typedef struct
{
    GFC_Matrix4     model;
    GFC_Matrix4     view;
    GFC_Matrix4     proj;
    GFC_Vector4D    color;
    GFC_Vector4D    camera;
}MeshUBO;

typedef struct
{
    GFC_Vector3D vertex;
    GFC_Vector3D normal;
    GFC_Vector2D texel;
    //armature support:
    GFC_Vector4D bones;   //bone indices
    GFC_Vector4D weights; //bone weights
}Vertex;

typedef struct
{
    Uint16  verts[3];
}Face;

typedef struct
{
    Uint32          vertexCount;
    VkBuffer        vertexBuffer;
    VkDeviceMemory  vertexBufferMemory;
    Uint32          faceCount;
    VkBuffer        faceBuffer;
    VkDeviceMemory  faceBufferMemory;
}MeshPrimitive;

typedef struct
{
    GFC_TextLine        filename;
    Uint32          _refCount;
    Uint8           _inuse;
    GFC_List           *primitives;
    GFC_Box             bounds;    
}Mesh;

/**
 * @brief initializes the mesh system / configures internal data about mesh based rendering
 * @param mesh_max the maximum allowed simultaneous meshes supported at once.  Must be > 0
 * @note keep in mind that many models will be comprised of multiple sub meshes.  So this number may need to be very large
 */
void gf3d_mesh_init(Uint32 mesh_max);


/**
 * @brief get a new empty model
 * @return NULL on error, or an empty model
 */
Mesh *gf3d_mesh_new();

/**
 * @brief load mesh data from the filename.
 * @note: currently only supporting obj files
 * @param filename the name of the file to load
 * @return NULL on error or Mesh data
 */
Mesh *gf3d_mesh_load(const char *filename);

/**
 * @brief get the scaling factor necessary to make the mesh fit within the bounds
 * @param mesh the mesh to validate (if this is NULL, returns (1,1,1)
 * @param size the dimensions to scale to
 * @return the factor to scale a mesh so that it fits exactly within the size provided.
 * @note: likely you want to uniformly scale based on the SMALLEST of the dimensions
 */
GFC_Vector3D gf3d_mesh_get_scaled_to(Mesh *mesh,GFC_Vector3D size);

/**
 * @brief get the input attribute descriptions for mesh based rendering
 * @param count (optional, output) the number of attributes
 * @return a pointer to a vertex input attribute description array
 */
VkVertexInputAttributeDescription * gf3d_mesh_get_attribute_descriptions(Uint32 *count);

/**
 * @brief get the binding description for mesh based rendering
 * @return vertex input binding descriptions compatible with mesh data
 */
VkVertexInputBindingDescription * gf3d_mesh_get_bind_description();

/**
 * @brief free a mesh that has been loaded from memory
 */
void gf3d_mesh_free(Mesh *mesh);

/**
 * @brief needs to be called once at the beginning of each render frame
 */
void gf3d_mesh_reset_pipes();

/**
 * @brief called to submit all draw commands to the mesh pipelines
 */
void gf3d_mesh_submit_pipe_commands();

/**
 * @brief get the current command buffer for the mesh system
 */
VkCommandBuffer gf3d_mesh_get_model_command_buffer();
VkCommandBuffer gf3d_mesh_get_alph_model_command_buffer();
VkCommandBuffer gf3d_mesh_get_highlight_command_buffer();
VkCommandBuffer gf3d_mesh_get_sky_command_buffer();

/**
 * @brief queue up a render for the current draw frame
 * @param mesh the mesh to render
 * @param pipe the pipeline to use
 * @param uboData the data to use to draw the mesh
 * @param texture texture data to use
 */
void gf3d_mesh_queue_render(Mesh *mesh,Pipeline *pipe,void *uboData,Texture *texture);


/**
 * @brief adds a mesh to the render pass rendered as an outline highlight
 * @note: must be called within the render pass
 * @param mesh the mesh to render
 * @param com the command pool to use to handle the request we are rendering with
 */
void gf3d_mesh_render(Mesh *mesh,VkCommandBuffer commandBuffer, VkDescriptorSet * descriptorSet);
void gf3d_mesh_alpha_render(Mesh *mesh,VkCommandBuffer commandBuffer, VkDescriptorSet * descriptorSet);

void gf3d_mesh_render_highlight(Mesh *mesh,VkCommandBuffer commandBuffer, VkDescriptorSet * descriptorSet);
void gf3d_mesh_render_sky(Mesh *mesh,VkCommandBuffer commandBuffer, VkDescriptorSet * descriptorSet);

/**
 * @brief render a mesh through a given pipeline
 */
void gf3d_mesh_render_generic(Mesh *mesh,Pipeline *pipe,VkDescriptorSet * descriptorSet);

/**
 * @brief create a mesh's internal buffers based on vertices
 * @param mesh the mesh handle to populate
 * @param vertices an array of vertices to make the mesh with
 * @param vcount how many vertices are in the array
 * @param faces an array of faces to make the mesh with
 * @param fcount how many faces are in the array
 */
void gf3d_mesh_create_vertex_buffer_from_vertices(MeshPrimitive *mesh,Vertex *vertices,Uint32 vcount,Face *faces,Uint32 fcount);

/**
 * @brief get the pipeline that is used to render basic 3d meshes
 * @return NULL on error or the pipeline in question
 */
Pipeline *gf3d_mesh_get_pipeline();
Pipeline *gf3d_mesh_get_alpha_pipeline();
Pipeline *gf3d_mesh_get_highlight_pipeline();
Pipeline *gf3d_mesh_get_sky_pipeline();

/**
 * @brief given a model matrix and basic color, build the meshUBO needed to render a model
 * @param modelMat the model Matrix
 * @param colorMod the color for the UBO
 */
MeshUBO gf3d_mesh_get_ubo(
    GFC_Matrix4 modelMat,
    GFC_Color colorMod);


#endif
