#include "simple_logger.h"

#include "gfc_matrix.h"

#include "gf3d_vgraphics.h"
#include "gf3d_buffers.h"

#include "gf3d_particle.h"

#define PARTICLE_ATTRIBUTE_COUNT 1
extern int __DEBUG;
typedef struct
{
    Matrix4     model;
    Matrix4     view;
    Matrix4     proj;
    Vector4D    color;
    Vector4D    color2;
    Vector2D    viewportSize;
    Vector2D    texture_offset;// as a percent
    Vector2D    texture_size;// as a percent
    Uint32      textured;
    float       size;
}ParticleUBO;

typedef struct
{
    VkVertexInputAttributeDescription   attributeDescriptions[PARTICLE_ATTRIBUTE_COUNT];
    VkVertexInputBindingDescription     bindingDescription;
    VkBuffer                    buffer;                 /**<vertex buffer for particles (just one vertex)*/
    VkDeviceMemory              bufferMemory;           /**<memory handle for the vertex buffer*/
    Texture                    *defaultTexture;
    Pipeline *pipe;
}ParticleManager;


static ParticleManager gf3d_particle_manager = {0};

void gf3d_particle_create_vertex_buffer();


Particle gf3d_particle(Vector3D position, Color color, float size)
{
    Particle p = {position, color, color, size};
    return p;
}


void gf3d_particles_manager_close()
{
    if (gf3d_particle_manager.buffer != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(gf3d_vgraphics_get_default_logical_device(), gf3d_particle_manager.buffer, NULL);
    }
    if (gf3d_particle_manager.bufferMemory != VK_NULL_HANDLE)
    {
        vkFreeMemory(gf3d_vgraphics_get_default_logical_device(), gf3d_particle_manager.bufferMemory, NULL);
    }
    memset(&gf3d_particle_manager,0,sizeof(ParticleManager));
}

void gf3d_particle_manager_init(Uint32 max_particles)
{
    gf3d_particle_manager.bindingDescription.binding = 0;
    gf3d_particle_manager.bindingDescription.stride = sizeof(Vector3D);
    gf3d_particle_manager.bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    gf3d_particle_manager.attributeDescriptions[0].binding = 0;
    gf3d_particle_manager.attributeDescriptions[0].location = 0;
    gf3d_particle_manager.attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    gf3d_particle_manager.attributeDescriptions[0].offset = 0;

    gf3d_particle_create_vertex_buffer();
    gf3d_particle_manager.pipe = gf3d_pipeline_create_from_config(
        gf3d_vgraphics_get_default_logical_device(),
        "config/particle_pipeline.cfg",
        gf3d_vgraphics_get_view_extent(),
        max_particles,
        &gf3d_particle_manager.bindingDescription,
        gf3d_particle_manager.attributeDescriptions,
        PARTICLE_ATTRIBUTE_COUNT,
        sizeof(ParticleUBO),
        VK_INDEX_TYPE_UINT16
    );
    if(__DEBUG)slog("particle manager initiliazed");
    gf3d_particle_manager.defaultTexture = gf3d_texture_load("images/effects/flare.png");
    atexit(gf3d_particles_manager_close);
}

void gf3d_particle_reset_pipes()
{
    Uint32 bufferFrame = gf3d_vgraphics_get_current_buffer_frame();
    
    gf3d_pipeline_reset_frame(gf3d_particle_manager.pipe,bufferFrame);
}

void gf3d_particle_submit_pipe_commands()
{
    gf3d_pipeline_submit_commands(gf3d_particle_manager.pipe);
}


Pipeline *gf3d_particle_get_pipeline()
{
    return gf3d_particle_manager.pipe;
}

void gf3d_particle_create_vertex_buffer()
{
    void *data = NULL;
    size_t bufferSize;
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VkDevice device = gf3d_vgraphics_get_default_logical_device();
    Vector3D particle = {0};

    bufferSize = sizeof(Vector3D);
    
    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);
    
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
            memcpy(data, &particle, (size_t) bufferSize);
        vkUnmapMemory(device, stagingBufferMemory);

    gf3d_buffer_create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT|VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &gf3d_particle_manager.buffer, &gf3d_particle_manager.bufferMemory);

    gf3d_buffer_copy(stagingBuffer, gf3d_particle_manager.buffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);    
}


VkVertexInputBindingDescription * gf3d_particle_get_bind_description()
{
    return &gf3d_particle_manager.bindingDescription;
}

VkVertexInputAttributeDescription * gf3d_particle_get_attribute_descriptions(Uint32 *count)
{    
    if (count)*count = 1;
    return gf3d_particle_manager.attributeDescriptions;
}

ParticleUBO gf3d_particle_get_uniform_buffer(Particle *particle)
{
    ParticleUBO particleUBO = {0};
    UniformBufferObject graphics_ubo;
    
    graphics_ubo = gf3d_vgraphics_get_uniform_buffer_object();
    
    gfc_matrix_identity(particleUBO.model);
    gfc_matrix_translate(particleUBO.model,particle->position);    
    
    gfc_matrix_copy(particleUBO.view,graphics_ubo.view);
    gfc_matrix_copy(particleUBO.proj,graphics_ubo.proj);
    
    particleUBO.texture_size.x = 1.00;
    particleUBO.texture_size.y = 1.00;
    
    vector4d_copy(particleUBO.color,gfc_color_to_vector4f(particle->color));
    vector4d_copy(particleUBO.color2,gfc_color_to_vector4f(particle->color2));
    particleUBO.size = particle->size;
    particleUBO.viewportSize = gf3d_vgraphics_get_view_extent_as_vector2d();
    return particleUBO;
}

void gf3d_particle_update_uniform_buffer(Particle *particle,UniformBuffer *ubo)
{
    void* data;
    ParticleUBO particleUBO = {0};
    UniformBufferObject graphics_ubo;
    
    graphics_ubo = gf3d_vgraphics_get_uniform_buffer_object();
    
    gfc_matrix_identity(particleUBO.model);
    gfc_matrix_translate(particleUBO.model,particle->position);    
    
    gfc_matrix_copy(particleUBO.view,graphics_ubo.view);
    gfc_matrix_copy(particleUBO.proj,graphics_ubo.proj);
    
    vector4d_copy(particleUBO.color,gfc_color_to_vector4f(particle->color));
    particleUBO.size = particle->size;
    particleUBO.viewportSize = gf3d_vgraphics_get_view_extent_as_vector2d();
    
    vkMapMemory(gf3d_vgraphics_get_default_logical_device(), ubo->uniformBufferMemory, 0, sizeof(ParticleUBO), 0, &data);
    
        memcpy(data, &particleUBO, sizeof(ParticleUBO));

    vkUnmapMemory(gf3d_vgraphics_get_default_logical_device(), ubo->uniformBufferMemory);
}

void gf3d_particle_draw(Particle particle)
{
    ParticleUBO ubo = {0};
    ubo = gf3d_particle_get_uniform_buffer(&particle);
//     slog("particle ubo position: %f,%f,%f",ubo.model[3][0],ubo.model[3][1],ubo.model[3][2]);
//     slog("particle ubo color: %f,%f,%f,%f",ubo.color.x,ubo.color.y,ubo.color.z,ubo.color.w);
//     slog("particle ubo size: %f",ubo.size);
    gf3d_pipeline_queue_render(
        gf3d_particle_manager.pipe,
        gf3d_particle_manager.buffer,
        3,//its a single vertex
        VK_NULL_HANDLE,
        &ubo,
        gf3d_particle_manager.defaultTexture);
}

void gf3d_particle_draw_textured(Particle particle,Texture *texture)
{
    ParticleUBO ubo = {0};
    ubo = gf3d_particle_get_uniform_buffer(&particle);
    if (texture)ubo.textured = 1;
    gf3d_pipeline_queue_render(
        gf3d_particle_manager.pipe,
        gf3d_particle_manager.buffer,
        1,//its a single vertex
        VK_NULL_HANDLE,
        &ubo,
        texture);
}

void gf3d_particle_draw_sprite(Particle particle,Sprite *sprite,int frame)
{
    Texture *texture = NULL;
    ParticleUBO ubo = {0};
    ubo = gf3d_particle_get_uniform_buffer(&particle);
    if (sprite)
    {
        ubo.textured = 1;
        ubo.texture_size.x = sprite->widthPercent;
        ubo.texture_size.y = sprite->heightPercent;
        ubo.texture_offset.x = (frame%sprite->framesPerLine)* sprite->widthPercent;
        ubo.texture_offset.y = (frame/sprite->framesPerLine)* sprite->heightPercent;
        texture = sprite->texture;
    }
    gf3d_pipeline_queue_render(
        gf3d_particle_manager.pipe,
        gf3d_particle_manager.buffer,
        1,//its a single quad
        VK_NULL_HANDLE,
        &ubo,
        texture);
}



void gf3d_particle_trail_draw(Color color, float size, Uint8 count, Edge3D trail)
{
    int i;
    Particle particle;
    Vector3D position;
    Vector3D step;
    vector3d_copy(position,trail.a);
    vector3d_sub(step,trail.b,trail.a);//vector to b
    gfc_color_copy(particle.color,color);
    particle.color2 = gfc_color8(255,255,255,255);
    particle.size = size;
    if (count > 1)
    {
        vector3d_scale(step,step,1/(float)count);
    }
    for (i = 0; i < count; i++)
    {
        vector3d_copy(particle.position,position);
        vector3d_add(position,position,step);
        gf3d_particle_draw(particle);
    }
}

void draw_guiding_lights(Vector3D position,Vector3D rotation,float width, float length)
{
    Vector3D start = {0};
    Vector3D forward = {0};
    Vector3D right = {0};
    Vector3D offset= {0};
        //draw guiding lights
    vector3d_angle_vectors(rotation, &right, &forward,NULL);
    vector3d_scale(right,right,width);
    vector3d_copy(offset,forward);
    vector3d_scale(offset,offset,10 * fabs(cos(SDL_GetTicks()*0.001)));
    vector3d_add(start,position,offset);

    vector3d_scale(forward,forward,length);
    vector3d_add(forward,forward,offset);
    vector3d_add(forward,forward,position);
    vector3d_add(forward,forward,right);
    vector3d_add(start,start,right);
    
    gf3d_particle_trail_draw(GFC_COLOR_RED, 10, length *0.05, gfc_edge3d_from_vectors(start,forward));
    
    vector3d_scale(right,right,-2);
    vector3d_add(forward,forward,right);
    vector3d_add(start,start,right);
    gf3d_particle_trail_draw(GFC_COLOR_GREEN, 10, length *0.05, gfc_edge3d_from_vectors(start,forward));

}

/*eol@eof*/
