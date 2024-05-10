#include <stddef.h>

#include "simple_logger.h"

#include "gf3d_buffers.h"
#include "gf3d_vgraphics.h"
#include "gf3d_obj_load.h"
#include "gf3d_swapchain.h"
#include "gf3d_commands.h"
#include "gf3d_pipeline.h"
#include "gf3d_mesh.h"

#define ATTRIBUTE_COUNT 5

extern int __DEBUG;

typedef struct
{
    Mesh *mesh_list;
    Pipeline *pipe;
    Pipeline *highlight_pipe;
    Pipeline *sky_pipe;
    Pipeline *alpha_pipe;
    Uint32 mesh_max;
    VkVertexInputAttributeDescription attributeDescriptions[ATTRIBUTE_COUNT];
    VkVertexInputBindingDescription bindingDescription;
    Command *stagingCommandBuffer;
}MeshSystem;

static MeshSystem gf3d_mesh = {0};

void gf3d_mesh_close();
void gf3d_mesh_delete(Mesh *mesh);
Mesh *gf3d_mesh_get_by_filename(const char *filename);

void gf3d_mesh_init(Uint32 mesh_max)
{
    Uint32 count = 0;
    if (!mesh_max)
    {
        slog("failed to initialize mesh system: cannot allocate 0 mesh_max");
        return;
    }
    atexit(gf3d_mesh_close);
    gf3d_mesh.mesh_max = mesh_max;
    
    gf3d_mesh.bindingDescription.binding = 0;
    gf3d_mesh.bindingDescription.stride = sizeof(Vertex);
    gf3d_mesh.bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    gf3d_mesh.attributeDescriptions[0].binding = 0;
    gf3d_mesh.attributeDescriptions[0].location = 0;
    gf3d_mesh.attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    gf3d_mesh.attributeDescriptions[0].offset = offsetof(Vertex, vertex);

    gf3d_mesh.attributeDescriptions[1].binding = 0;
    gf3d_mesh.attributeDescriptions[1].location = 1;
    gf3d_mesh.attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    gf3d_mesh.attributeDescriptions[1].offset = offsetof(Vertex, normal);
    
    gf3d_mesh.attributeDescriptions[2].binding = 0;
    gf3d_mesh.attributeDescriptions[2].location = 2;
    gf3d_mesh.attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    gf3d_mesh.attributeDescriptions[2].offset = offsetof(Vertex, texel);

    gf3d_mesh.attributeDescriptions[3].binding = 0;
    gf3d_mesh.attributeDescriptions[3].location = 3;
    gf3d_mesh.attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    gf3d_mesh.attributeDescriptions[3].offset = offsetof(Vertex, bones);

    gf3d_mesh.attributeDescriptions[4].binding = 0;
    gf3d_mesh.attributeDescriptions[4].location = 4;
    gf3d_mesh.attributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    gf3d_mesh.attributeDescriptions[4].offset = offsetof(Vertex, weights);

    gf3d_mesh.mesh_list = gfc_allocate_array(sizeof(Mesh),mesh_max);
    
    gf3d_mesh_get_attribute_descriptions(&count);
    
    gf3d_mesh.sky_pipe = gf3d_pipeline_create_from_config(
        gf3d_vgraphics_get_default_logical_device(),
        "config/sky_pipeline.cfg",
        gf3d_vgraphics_get_view_extent(),
        mesh_max,
        gf3d_mesh_get_bind_description(),
        gf3d_mesh_get_attribute_descriptions(NULL),
        count,
        sizeof(SkyUBO),
        VK_INDEX_TYPE_UINT16
    );

    gf3d_mesh.pipe = gf3d_pipeline_create_from_config(
        gf3d_vgraphics_get_default_logical_device(),
        "config/model_pipeline.cfg",
        gf3d_vgraphics_get_view_extent(),
        mesh_max,
        gf3d_mesh_get_bind_description(),
        gf3d_mesh_get_attribute_descriptions(NULL),
        count,
        sizeof(MeshUBO),
        VK_INDEX_TYPE_UINT16
    );

    gf3d_mesh.alpha_pipe = gf3d_pipeline_create_from_config(
        gf3d_vgraphics_get_default_logical_device(),
        "config/model_alpha_pipeline.cfg",
        gf3d_vgraphics_get_view_extent(),
        mesh_max,
        gf3d_mesh_get_bind_description(),
        gf3d_mesh_get_attribute_descriptions(NULL),
        count,
        sizeof(MeshUBO),
        VK_INDEX_TYPE_UINT16
    );

    gf3d_mesh.highlight_pipe = gf3d_pipeline_create_from_config(
        gf3d_vgraphics_get_default_logical_device(),
        "config/highlight_pipeline.cfg",
        gf3d_vgraphics_get_view_extent(),
        mesh_max,
        gf3d_mesh_get_bind_description(),
        gf3d_mesh_get_attribute_descriptions(NULL),
        count,
        sizeof(HighlightUBO),
        VK_INDEX_TYPE_UINT16
    );

    if (__DEBUG)slog("mesh system initialized");
}

Pipeline *gf3d_mesh_get_pipeline()
{
    return gf3d_mesh.pipe;
}

Pipeline *gf3d_mesh_get_alpha_pipeline()
{
    return gf3d_mesh.alpha_pipe;
}

Pipeline *gf3d_mesh_get_highlight_pipeline()
{
    return gf3d_mesh.highlight_pipe;
}

Pipeline *gf3d_mesh_get_sky_pipeline()
{
    return gf3d_mesh.sky_pipe;
}

void gf3d_mesh_reset_pipes()
{
    Uint32 bufferFrame = gf3d_vgraphics_get_current_buffer_frame();
    
    gf3d_pipeline_reset_frame(gf3d_mesh.pipe,bufferFrame);
    gf3d_pipeline_reset_frame(gf3d_mesh.sky_pipe,bufferFrame);
    gf3d_pipeline_reset_frame(gf3d_mesh.alpha_pipe,bufferFrame);
    gf3d_pipeline_reset_frame(gf3d_mesh.highlight_pipe,bufferFrame);
}

void gf3d_mesh_submit_pipe_commands()
{
    gf3d_pipeline_submit_commands(gf3d_mesh.sky_pipe);
    gf3d_pipeline_submit_commands(gf3d_mesh.pipe);
    gf3d_pipeline_submit_commands(gf3d_mesh.alpha_pipe);
    gf3d_pipeline_submit_commands(gf3d_mesh.highlight_pipe);
}

VkCommandBuffer gf3d_mesh_get_model_command_buffer()
{
    if (!gf3d_mesh.pipe)return VK_NULL_HANDLE;
    return gf3d_mesh.pipe->commandBuffer;
}

VkCommandBuffer gf3d_mesh_get_alph_model_command_buffer()
{
    if (!gf3d_mesh.alpha_pipe)return VK_NULL_HANDLE;
    return gf3d_mesh.alpha_pipe->commandBuffer;
}

VkCommandBuffer gf3d_mesh_get_highlight_command_buffer()
{
    if (!gf3d_mesh.highlight_pipe)return VK_NULL_HANDLE;
    return gf3d_mesh.highlight_pipe->commandBuffer;
}

VkCommandBuffer gf3d_mesh_get_sky_command_buffer()
{
    if (!gf3d_mesh.sky_pipe)return VK_NULL_HANDLE;
    return gf3d_mesh.sky_pipe->commandBuffer;
}


VkVertexInputAttributeDescription * gf3d_mesh_get_attribute_descriptions(Uint32 *count)
{
    if (count)*count = ATTRIBUTE_COUNT;
    return gf3d_mesh.attributeDescriptions;
}

VkVertexInputBindingDescription * gf3d_mesh_get_bind_description()
{
    return &gf3d_mesh.bindingDescription;
}

Mesh *gf3d_mesh_new()
{
    int i;
    for (i = 0; i < gf3d_mesh.mesh_max; i++)
    {
        if (gf3d_mesh.mesh_list[i]._inuse == 0)
        {
            gf3d_mesh.mesh_list[i]._inuse = 1;
            gf3d_mesh.mesh_list[i]._refCount = 1;
            gf3d_mesh.mesh_list[i].primitives = gfc_list_new();
            return &gf3d_mesh.mesh_list[i];
        }
    }
    for (i = 0; i < gf3d_mesh.mesh_max; i++)
    {
        if (gf3d_mesh.mesh_list[i]._refCount == 0)
        {
            gf3d_mesh_delete(&gf3d_mesh.mesh_list[i]);
            gf3d_mesh.mesh_list[i]._inuse = 1;
            gf3d_mesh.mesh_list[i]._refCount = 1;
            gf3d_mesh.mesh_list[i].primitives = gfc_list_new();
            return &gf3d_mesh.mesh_list[i];
        }
    }
    return NULL;
}

Mesh *gf3d_mesh_get_by_filename(const char *filename)
{
    int i;
    for (i = 0; i < gf3d_mesh.mesh_max; i++)
    {
        if (!gf3d_mesh.mesh_list[i]._inuse)continue;
        if (gfc_line_cmp(gf3d_mesh.mesh_list[i].filename,filename) == 0)
        {
            return &gf3d_mesh.mesh_list[i];
        }
    }
    return NULL;
}

void gf3d_mesh_free(Mesh *mesh)
{
    if (!mesh)return;
    mesh->_refCount--;
}

void gf3d_mesh_free_all()
{
    int i;
    for (i = 0; i < gf3d_mesh.mesh_max; i++)
    {
        gf3d_mesh_delete(&gf3d_mesh.mesh_list[i]);
    }
}

void gf3d_mesh_close()
{
    if (gf3d_mesh.mesh_list)
    {
        gf3d_mesh_free_all();
        // TODO: iterate through mesh data and free all data
        free(gf3d_mesh.mesh_list);
        gf3d_mesh.mesh_list = NULL;
    }
    if (__DEBUG)slog("mesh system closed");
}

void gf3d_mesh_delete(Mesh *mesh)
{
    int i,c;
    MeshPrimitive *primitive;
    if ((!mesh)||(!mesh->_inuse))return;
    c = gfc_list_get_count(mesh->primitives);
    for (i = 0; i < c; i++)
    {
        primitive = gfc_list_get_nth(mesh->primitives,i);
        if (!primitive)continue;
        if (primitive->faceBuffer != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(gf3d_vgraphics_get_default_logical_device(), primitive->faceBuffer, NULL);
        }
        if (primitive->faceBufferMemory != VK_NULL_HANDLE)
        {
            vkFreeMemory(gf3d_vgraphics_get_default_logical_device(), primitive->faceBufferMemory, NULL);
        }
        if (primitive->vertexBuffer != VK_NULL_HANDLE)
        {
            vkDestroyBuffer(gf3d_vgraphics_get_default_logical_device(), primitive->vertexBuffer, NULL);
        }
        if (primitive->vertexBufferMemory != VK_NULL_HANDLE)
        {
            vkFreeMemory(gf3d_vgraphics_get_default_logical_device(), primitive->vertexBufferMemory, NULL);
        }
        free(primitive);
    }
    gfc_list_delete(mesh->primitives);
    memset(mesh,0,sizeof(Mesh));
}

void gf3d_mesh_scene_add(Mesh *mesh)
{
    if (!mesh)return;
}

void gf3d_mesh_queue_render(Mesh *mesh,Pipeline *pipe,void *uboData,Texture *texture)
{
    int i,c;
    MeshPrimitive *primitive;
    if (!mesh)
    {
        slog("cannot render a NULL mesh");
        return;
    }
    if (!pipe)
    {
        slog("cannot render with NULL pipe");
        return;
    }
    if (!uboData)
    {
        slog("cannot render with NULL descriptor set");
        return;
    }
    c = gfc_list_get_count(mesh->primitives);
    for (i = 0; i < c; i++)
    {
        primitive = gfc_list_get_nth(mesh->primitives,i);
        if (!primitive)continue;
        gf3d_pipeline_queue_render(
            pipe,
            primitive->vertexBuffer,
            primitive->faceCount * 3,
            primitive->faceBuffer,
            uboData,
            texture);
    }
}

void gf3d_mesh_render_generic(Mesh *mesh,Pipeline *pipe, VkDescriptorSet * descriptorSet)
{
    int i,c;
    MeshPrimitive *primitive;
    if (!mesh)
    {
        slog("cannot render a NULL mesh");
        return;
    }
    if (!pipe)
    {
        slog("cannot render with NULL pipe");
        return;
    }
    if (!descriptorSet)
    {
        slog("cannot render with NULL descriptor set");
        return;
    }
    c = gfc_list_get_count(mesh->primitives);
    for (i = 0; i < c; i++)
    {
        primitive = gfc_list_get_nth(mesh->primitives,i);
        if (!primitive)continue;
        gf3d_pipeline_call_render(
            pipe,
            descriptorSet,
            primitive->vertexBuffer,
            primitive->faceCount * 3,
            primitive->faceBuffer);
    }
}


void gf3d_mesh_render(Mesh *mesh,VkCommandBuffer commandBuffer, VkDescriptorSet * descriptorSet)
{
    gf3d_mesh_render_generic(mesh,gf3d_mesh.pipe, descriptorSet);
}

void gf3d_mesh_alpha_render(Mesh *mesh,VkCommandBuffer commandBuffer, VkDescriptorSet * descriptorSet)
{
    gf3d_mesh_render_generic(mesh,gf3d_mesh.alpha_pipe, descriptorSet);
}

void gf3d_mesh_render_highlight(Mesh *mesh,VkCommandBuffer commandBuffer, VkDescriptorSet * descriptorSet)
{
    gf3d_mesh_render_generic(mesh,gf3d_mesh.highlight_pipe, descriptorSet);
}

void gf3d_mesh_render_sky(Mesh *mesh,VkCommandBuffer commandBuffer, VkDescriptorSet * descriptorSet)
{
    gf3d_mesh_render_generic(mesh,gf3d_mesh.sky_pipe,descriptorSet);
}


void gf3d_mesh_setup_face_buffers(MeshPrimitive *mesh,Face *faces,Uint32 fcount)
{
    void* data;
    VkDevice device = gf3d_vgraphics_get_default_logical_device();
    VkDeviceSize bufferSize = sizeof(Face) * fcount;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    
    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, faces, (size_t) bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mesh->faceBuffer, &mesh->faceBufferMemory);

    gf3d_buffer_copy(stagingBuffer, mesh->faceBuffer, bufferSize);

    mesh->faceCount = fcount;
    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);
}

void gf3d_mesh_create_vertex_buffer_from_vertices(MeshPrimitive *mesh,Vertex *vertices,Uint32 vcount,Face *faces,Uint32 fcount)
{
    void *data = NULL;
    VkDevice device = gf3d_vgraphics_get_default_logical_device();
    size_t bufferSize;    
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    
    if (!mesh)
    {
        slog("no mesh primitive provided");
        return;
    }

    bufferSize = sizeof(Vertex) * vcount;
    
    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);
    
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices, (size_t) bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT|VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &mesh->vertexBuffer, &mesh->vertexBufferMemory);

    gf3d_buffer_copy(stagingBuffer, mesh->vertexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);
    
    mesh->vertexCount = vcount;
    
    gf3d_mesh_setup_face_buffers(mesh,faces,fcount);
}

GFC_Vector3D gf3d_mesh_get_scaled_to(Mesh *mesh,GFC_Vector3D size)
{
    GFC_Vector3D outScale = {1,1,1};
    if (!mesh)return outScale;
    if (size.x)outScale.x = mesh->bounds.w / size.x;
    if (size.y)outScale.y = mesh->bounds.h / size.y;
    if (size.z)outScale.z = mesh->bounds.d / size.z;    
    return outScale;
}

Mesh *gf3d_mesh_load(const char *filename)
{
    Mesh *mesh;
    MeshPrimitive *primitive;

    ObjData *obj;
    mesh = gf3d_mesh_get_by_filename(filename);
    if (mesh)return mesh;
    
    obj = gf3d_obj_load_from_file(filename);
    
    if (!obj)
    {
        return NULL;
    }
    
    mesh = gf3d_mesh_new();
    if (!mesh)
    {
        return NULL;
    }
    
    primitive = gfc_allocate_array(sizeof(MeshPrimitive),1);
    gf3d_mesh_create_vertex_buffer_from_vertices(primitive,obj->faceVertices,obj->face_vert_count,obj->outFace,obj->face_count);
    mesh->primitives = gfc_list_append(mesh->primitives,primitive);
    memcpy(&mesh->bounds,&obj->bounds,sizeof(GFC_Box));
    gf3d_obj_free(obj);
    gfc_line_cpy(mesh->filename,filename);
    
    return mesh;
}

/*eol@eof*/
