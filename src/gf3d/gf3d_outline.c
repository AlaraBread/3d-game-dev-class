#include "gf3d_buffers.h"
#include "gf3d_pipeline.h"
#include "gf3d_vgraphics.h"
#include "simple_logger.h"
#include "gf3d_outline.h"
#include "gf2d_sprite.h"
#include "gf3d_swapchain.h"

typedef struct {
	Uint16 verts[3];
} Face;

typedef struct {
	GFC_Vector2D vertex;
	GFC_Vector2D texel;
} Vertex;

typedef struct {
	GFC_Vector3D color;
	GFC_Vector2D extents;
	float zNear;
	float zFar;
} OutlineUBO;

static struct {
	Pipeline *pipe;
	VkDevice device;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer faceBuffer;
	VkDeviceMemory faceBufferMemory;
	VkVertexInputAttributeDescription attributeDescriptions[2];
	VkVertexInputBindingDescription bindingDescription;
	OutlineUBO ubo;
} gf3d_outline = {0};

static VkVertexInputAttributeDescription *get_attribute_descriptions();
static VkVertexInputBindingDescription *get_bind_description();

void outline_prerender() {
	Command *commandPool = gf3d_vgraphics_get_graphics_command_pool();
	VkCommandBuffer commandBuffer = gf3d_command_begin_single_time(commandPool);

	vkCmdPipelineBarrier(
		commandBuffer,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		0, 0, NULL, 0, NULL, 0, NULL
	);
	gf3d_command_end_single_time(commandPool, commandBuffer);
}

void outline_init() {
	void *data;
	Face faces[2];
	size_t bufferSize;
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	gf3d_outline.device = gf3d_vgraphics_get_default_logical_device();

	faces[0].verts[0] = 2;
	faces[0].verts[1] = 1;
	faces[0].verts[2] = 0;
	faces[1].verts[0] = 1;
	faces[1].verts[1] = 3;
	faces[1].verts[2] = 2;

	bufferSize = sizeof(Face) * 2;

	gf3d_buffer_create(
		bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		&stagingBuffer, &stagingBufferMemory
	);

	vkMapMemory(
		gf3d_outline.device, stagingBufferMemory, 0, bufferSize, 0, &data
	);
	memcpy(data, faces, (size_t)bufferSize);
	vkUnmapMemory(gf3d_outline.device, stagingBufferMemory);

	gf3d_buffer_create(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &gf3d_outline.faceBuffer,
		&gf3d_outline.faceBufferMemory
	);

	gf3d_buffer_copy(stagingBuffer, gf3d_outline.faceBuffer, bufferSize);

	gf3d_buffer_create(sizeof(Vertex)*6, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &gf3d_outline.vertexBuffer,
		&gf3d_outline.vertexBufferMemory
	);

	vkDestroyBuffer(gf3d_outline.device, stagingBuffer, NULL);
	vkFreeMemory(gf3d_outline.device, stagingBufferMemory, NULL);

	get_attribute_descriptions();
	get_bind_description();

	gf3d_outline.pipe = gf3d_pipeline_create_from_config(
		gf3d_vgraphics_get_default_logical_device(),
		"assets/config/outline_pipeline.cfg", gf3d_vgraphics_get_view_extent(),
		1, &gf3d_outline.bindingDescription,
		gf3d_outline.attributeDescriptions, 2, sizeof(OutlineUBO), VK_INDEX_TYPE_UINT16
	);

	gf3d_outline.pipe->preRender = outline_prerender;

	atexit(outline_free);
}

static VkVertexInputAttributeDescription *get_attribute_descriptions() {
	gf3d_outline.attributeDescriptions[0].binding = 0;
	gf3d_outline.attributeDescriptions[0].location = 0;
	gf3d_outline.attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	gf3d_outline.attributeDescriptions[0].offset = offsetof(Vertex, vertex);

	gf3d_outline.attributeDescriptions[1].binding = 0;
	gf3d_outline.attributeDescriptions[1].location = 1;
	gf3d_outline.attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
	gf3d_outline.attributeDescriptions[1].offset = offsetof(Vertex, texel);
	return gf3d_outline.attributeDescriptions;
}

static VkVertexInputBindingDescription *get_bind_description() {
	gf3d_outline.bindingDescription.binding = 0;
	gf3d_outline.bindingDescription.stride = sizeof(Vertex);
	gf3d_outline.bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return &gf3d_outline.bindingDescription;
}

void render_outlines() {
	VkExtent2D extent = gf3d_swapchain_get_extent();
	gf3d_outline.ubo.extents.x = (float) extent.width;
	gf3d_outline.ubo.extents.y = (float) extent.height;
	gf3d_outline.ubo.color.x = 0.0;
	gf3d_outline.ubo.color.y = 0.0;
	gf3d_outline.ubo.color.z = 0.0;
	gf3d_outline.ubo.zNear = 0.1;
	gf3d_outline.ubo.zFar = 100000.0;

	gf3d_pipeline_queue_render(
		gf3d_outline.pipe, gf3d_outline.vertexBuffer,
		6, // its a single quad
		gf3d_outline.faceBuffer, &gf3d_outline.ubo,
		gf3d_swapchain_get_normal_image_view(), gf3d_swapchain_get_normal_image_sampler(),
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		gf3d_swapchain_get_depth_image_view(), gf3d_swapchain_get_depth_image_sampler(),
		VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL
	);
}

void outline_free() {
	if(gf3d_outline.faceBuffer != VK_NULL_HANDLE) {
		vkDestroyBuffer(gf3d_outline.device, gf3d_outline.faceBuffer, NULL);
	}
	if(gf3d_outline.faceBufferMemory != VK_NULL_HANDLE) {
		vkFreeMemory(gf3d_outline.device, gf3d_outline.faceBufferMemory, NULL);
	}
}
