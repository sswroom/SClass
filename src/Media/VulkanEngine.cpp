#include "Stdafx.h"
#include <string.h>
#include "Math/Math_C.h"
#include "Media/VulkanEngine.h"

#if defined(__linux__)
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#ifdef GDK_WINDOWING_X11
#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#define VK_USE_PLATFORM_XLIB_KHR
#ifdef Bool
#undef Bool
#endif
#endif
#endif

#if defined(__has_include)
#if __has_include(<vulkan/vulkan.h>)
#include <vulkan/vulkan.h>
#define HAS_VULKAN_HEADER 1
#else
#define HAS_VULKAN_HEADER 0
#endif
#else
#define HAS_VULKAN_HEADER 0
#endif

#if defined(__has_include)
#if __has_include(<shaderc/shaderc.h>)
#include <shaderc/shaderc.h>
#define HAS_SHADERC_HEADER 1
#else
#define HAS_SHADERC_HEADER 0
#endif
#else
#define HAS_SHADERC_HEADER 0
#endif

#if HAS_VULKAN_HEADER && defined(__linux__) && defined(GDK_WINDOWING_X11)
struct VulkanDrawVertex
{
	float x;
	float y;
	float z;
	float r;
	float g;
	float b;
};

class VulkanEngineScene : public Media::Engine3DScene
{
public:
	Optional<ControlHandle> ctrlHdl;
	Bool initialized;

	VkInstance instance;
	VkSurfaceKHR surface;
	VkPhysicalDevice phyDev;
	VkDevice device;
	UInt32 gfxQueueFamily;
	UInt32 presentQueueFamily;
	VkQueue gfxQueue;
	VkQueue presentQueue;

	VkSwapchainKHR swapchain;
	VkFormat swapFmt;
	VkExtent2D extent;
	UInt32 imageCnt;
	VkImage *images;
	VkImageView *imageViews;
	VkFramebuffer *framebuffers;

	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;
	VkCommandPool cmdPool;
	VkCommandBuffer *cmdBuffers;
	VkSemaphore imageAvailable;
	VkSemaphore renderFinished;
	VkFence inFlightFence;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	UInt32 vertexBufferVertCap;

	VulkanDrawVertex *drawVertices;
	UInt32 drawVertCnt;
	UInt32 drawVertCap;

	VulkanEngineScene(Optional<ControlHandle> ctrlHdl)
	{
		this->ctrlHdl = ctrlHdl;
		this->initialized = false;
		this->instance = VK_NULL_HANDLE;
		this->surface = VK_NULL_HANDLE;
		this->phyDev = VK_NULL_HANDLE;
		this->device = VK_NULL_HANDLE;
		this->gfxQueueFamily = 0;
		this->presentQueueFamily = 0;
		this->gfxQueue = VK_NULL_HANDLE;
		this->presentQueue = VK_NULL_HANDLE;
		this->swapchain = VK_NULL_HANDLE;
		this->swapFmt = VK_FORMAT_UNDEFINED;
		this->extent.width = 0;
		this->extent.height = 0;
		this->imageCnt = 0;
		this->images = nullptr;
		this->imageViews = nullptr;
		this->framebuffers = nullptr;
		this->renderPass = VK_NULL_HANDLE;
		this->pipelineLayout = VK_NULL_HANDLE;
		this->pipeline = VK_NULL_HANDLE;
		this->cmdPool = VK_NULL_HANDLE;
		this->cmdBuffers = nullptr;
		this->imageAvailable = VK_NULL_HANDLE;
		this->renderFinished = VK_NULL_HANDLE;
		this->inFlightFence = VK_NULL_HANDLE;
		this->vertexBuffer = VK_NULL_HANDLE;
		this->vertexBufferMemory = VK_NULL_HANDLE;
		this->vertexBufferVertCap = 0;
		this->drawVertices = nullptr;
		this->drawVertCnt = 0;
		this->drawVertCap = 0;
	}

	void ClearSceneMesh()
	{
		this->drawVertCnt = 0;
	}

	Bool EnsureSceneMeshCapacity(UInt32 addCount)
	{
		UInt32 reqCnt = this->drawVertCnt + addCount;
		if (reqCnt <= this->drawVertCap)
			return true;
		UInt32 newCap = this->drawVertCap;
		if (newCap < 256)
			newCap = 256;
		while (newCap < reqCnt)
		{
			UInt32 nextCap = newCap << 1;
			if (nextCap <= newCap)
				return false;
			newCap = nextCap;
		}
		VulkanDrawVertex *newVerts = new VulkanDrawVertex[newCap];
		if (this->drawVertices && this->drawVertCnt > 0)
		{
			memcpy(newVerts, this->drawVertices, this->drawVertCnt * sizeof(VulkanDrawVertex));
		}
		if (this->drawVertices)
		{
			delete [] this->drawVertices;
		}
		this->drawVertices = newVerts;
		this->drawVertCap = newCap;
		return true;
	}

	Bool AddTriangle(const VulkanDrawVertex &v0, const VulkanDrawVertex &v1, const VulkanDrawVertex &v2)
	{
		if (!EnsureSceneMeshCapacity(3))
			return false;
		this->drawVertices[this->drawVertCnt++] = v0;
		this->drawVertices[this->drawVertCnt++] = v1;
		this->drawVertices[this->drawVertCnt++] = v2;
		return true;
	}

	void ReleaseSwapchainResources()
	{
		UInt32 i;
		if (this->device != VK_NULL_HANDLE)
		{
			i = this->imageCnt;
			while (i-- > 0)
			{
				if (this->framebuffers && this->framebuffers[i] != VK_NULL_HANDLE)
				{
					vkDestroyFramebuffer(this->device, this->framebuffers[i], nullptr);
				}
			}
			i = this->imageCnt;
			while (i-- > 0)
			{
				if (this->imageViews && this->imageViews[i] != VK_NULL_HANDLE)
				{
					vkDestroyImageView(this->device, this->imageViews[i], nullptr);
				}
			}
			if (this->swapchain != VK_NULL_HANDLE)
			{
				vkDestroySwapchainKHR(this->device, this->swapchain, nullptr);
			}
		}
		this->swapchain = VK_NULL_HANDLE;
		this->imageCnt = 0;
		if (this->images)
		{
			delete [] this->images;
			this->images = nullptr;
		}
		if (this->imageViews)
		{
			delete [] this->imageViews;
			this->imageViews = nullptr;
		}
		if (this->framebuffers)
		{
			delete [] this->framebuffers;
			this->framebuffers = nullptr;
		}
		if (this->cmdBuffers)
		{
			delete [] this->cmdBuffers;
			this->cmdBuffers = nullptr;
		}
	}

	void ReleaseAll()
	{
		if (this->device != VK_NULL_HANDLE)
		{
			vkDeviceWaitIdle(this->device);
		}
		ReleaseSwapchainResources();
		if (this->device != VK_NULL_HANDLE)
		{
			if (this->vertexBuffer != VK_NULL_HANDLE)
			{
				vkDestroyBuffer(this->device, this->vertexBuffer, nullptr);
			}
			if (this->vertexBufferMemory != VK_NULL_HANDLE)
			{
				vkFreeMemory(this->device, this->vertexBufferMemory, nullptr);
			}
			if (this->pipeline != VK_NULL_HANDLE)
			{
				vkDestroyPipeline(this->device, this->pipeline, nullptr);
			}
			if (this->pipelineLayout != VK_NULL_HANDLE)
			{
				vkDestroyPipelineLayout(this->device, this->pipelineLayout, nullptr);
			}
			if (this->inFlightFence != VK_NULL_HANDLE)
			{
				vkDestroyFence(this->device, this->inFlightFence, nullptr);
			}
			if (this->imageAvailable != VK_NULL_HANDLE)
			{
				vkDestroySemaphore(this->device, this->imageAvailable, nullptr);
			}
			if (this->renderFinished != VK_NULL_HANDLE)
			{
				vkDestroySemaphore(this->device, this->renderFinished, nullptr);
			}
			if (this->cmdPool != VK_NULL_HANDLE)
			{
				vkDestroyCommandPool(this->device, this->cmdPool, nullptr);
			}
			if (this->renderPass != VK_NULL_HANDLE)
			{
				vkDestroyRenderPass(this->device, this->renderPass, nullptr);
			}
			vkDestroyDevice(this->device, nullptr);
		}
		if (this->surface != VK_NULL_HANDLE && this->instance != VK_NULL_HANDLE)
		{
			vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
		}
		if (this->instance != VK_NULL_HANDLE)
		{
			vkDestroyInstance(this->instance, nullptr);
		}
		this->initialized = false;
		this->instance = VK_NULL_HANDLE;
		this->surface = VK_NULL_HANDLE;
		this->phyDev = VK_NULL_HANDLE;
		this->device = VK_NULL_HANDLE;
		this->pipelineLayout = VK_NULL_HANDLE;
		this->pipeline = VK_NULL_HANDLE;
		this->vertexBuffer = VK_NULL_HANDLE;
		this->vertexBufferMemory = VK_NULL_HANDLE;
		this->vertexBufferVertCap = 0;
		if (this->drawVertices)
		{
			delete [] this->drawVertices;
			this->drawVertices = nullptr;
		}
		this->drawVertCnt = 0;
		this->drawVertCap = 0;
	}
};
#endif

static UInt32 VulkanFindMemoryType(NN<VulkanEngineScene> scene, UInt32 typeBits, VkMemoryPropertyFlags props)
{
#if HAS_VULKAN_HEADER && defined(__linux__) && defined(GDK_WINDOWING_X11)
	VkPhysicalDeviceMemoryProperties memProps;
	vkGetPhysicalDeviceMemoryProperties(scene->phyDev, &memProps);
	UInt32 i = 0;
	while (i < memProps.memoryTypeCount)
	{
		if ((typeBits & (1U << i)) != 0 && (memProps.memoryTypes[i].propertyFlags & props) == props)
		{
			return i;
		}
		i++;
	}
	return 0xffffffff;
#else
	(void)scene;
	(void)typeBits;
	(void)props;
	return 0xffffffff;
#endif
}

static Bool VulkanCompileShaderModule(NN<VulkanEngineScene> scene, Int32 shaderKind, const Char *source, const Char *fileName, VkShaderModule *shaderModule)
{
#if HAS_VULKAN_HEADER && HAS_SHADERC_HEADER && defined(__linux__) && defined(GDK_WINDOWING_X11)
	shaderc_compiler_t compiler = shaderc_compiler_initialize();
	if (compiler == nullptr)
		return false;
	shaderc_compile_options_t options = shaderc_compile_options_initialize();
	if (options == nullptr)
	{
		shaderc_compiler_release(compiler);
		return false;
	}
	shaderc_compile_options_set_target_env(options, shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_0);
	shaderc_compilation_result_t result = shaderc_compile_into_spv(compiler, source, strlen(source), (shaderc_shader_kind)shaderKind, fileName, "main", options);
	shaderc_compile_options_release(options);
	shaderc_compiler_release(compiler);
	if (result == nullptr)
		return false;
	if (shaderc_result_get_compilation_status(result) != shaderc_compilation_status_success)
	{
		shaderc_result_release(result);
		return false;
	}
	VkShaderModuleCreateInfo smCI = {};
	smCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	smCI.codeSize = shaderc_result_get_length(result);
	smCI.pCode = (const UInt32*)shaderc_result_get_bytes(result);
	Bool succ = vkCreateShaderModule(scene->device, &smCI, nullptr, shaderModule) == VK_SUCCESS;
	shaderc_result_release(result);
	return succ;
#else
	(void)scene;
	(void)shaderKind;
	(void)source;
	(void)fileName;
	(void)shaderModule;
	return false;
#endif
}

static Bool VulkanCreateGraphicsPipeline(NN<VulkanEngineScene> scene)
{
#if HAS_VULKAN_HEADER && HAS_SHADERC_HEADER && defined(__linux__) && defined(GDK_WINDOWING_X11)
	if (scene->pipeline != VK_NULL_HANDLE)
		return true;

	const Char *vtxSrc =
		"#version 450\n"
		"layout(location=0) in vec3 inPos;\n"
		"layout(location=1) in vec3 inColor;\n"
		"layout(location=0) out vec3 outColor;\n"
		"void main()\n"
		"{\n"
		"    gl_Position = vec4(inPos, 1.0);\n"
		"    outColor = inColor;\n"
		"}\n";

	const Char *fragSrc =
		"#version 450\n"
		"layout(location=0) in vec3 outColor;\n"
		"layout(location=0) out vec4 fragColor;\n"
		"void main()\n"
		"{\n"
		"    fragColor = vec4(outColor, 1.0);\n"
		"}\n";

	VkShaderModule vertMod = VK_NULL_HANDLE;
	VkShaderModule fragMod = VK_NULL_HANDLE;
	if (!VulkanCompileShaderModule(scene, shaderc_vertex_shader, vtxSrc, "scene.vert", &vertMod) ||
		!VulkanCompileShaderModule(scene, shaderc_fragment_shader, fragSrc, "scene.frag", &fragMod))
	{
		if (vertMod != VK_NULL_HANDLE)
			vkDestroyShaderModule(scene->device, vertMod, nullptr);
		if (fragMod != VK_NULL_HANDLE)
			vkDestroyShaderModule(scene->device, fragMod, nullptr);
		return false;
	}

	VkPipelineShaderStageCreateInfo stages[2];
	stages[0] = {};
	stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	stages[0].module = vertMod;
	stages[0].pName = "main";
	stages[1] = {};
	stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	stages[1].module = fragMod;
	stages[1].pName = "main";

	VkVertexInputBindingDescription vb = {};
	vb.binding = 0;
	vb.stride = sizeof(VulkanDrawVertex);
	vb.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription va[2];
	va[0] = {};
	va[0].binding = 0;
	va[0].location = 0;
	va[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	va[0].offset = 0;
	va[1] = {};
	va[1].binding = 0;
	va[1].location = 1;
	va[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	va[1].offset = sizeof(float) * 3;

	VkPipelineVertexInputStateCreateInfo vi = {};
	vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vi.vertexBindingDescriptionCount = 1;
	vi.pVertexBindingDescriptions = &vb;
	vi.vertexAttributeDescriptionCount = 2;
	vi.pVertexAttributeDescriptions = va;

	VkPipelineInputAssemblyStateCreateInfo ia = {};
	ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	ia.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	VkPipelineViewportStateCreateInfo vp = {};
	vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	vp.viewportCount = 1;
	vp.scissorCount = 1;

	VkPipelineRasterizationStateCreateInfo rs = {};
	rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rs.polygonMode = VK_POLYGON_MODE_FILL;
	rs.lineWidth = 1.0f;
	rs.cullMode = VK_CULL_MODE_NONE;
	rs.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

	VkPipelineMultisampleStateCreateInfo ms = {};
	ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState cba = {};
	cba.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo cb = {};
	cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	cb.attachmentCount = 1;
	cb.pAttachments = &cba;

	VkDynamicState dynStates[2] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
	VkPipelineDynamicStateCreateInfo dyn = {};
	dyn.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dyn.dynamicStateCount = 2;
	dyn.pDynamicStates = dynStates;

	VkPipelineLayoutCreateInfo plCI = {};
	plCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	if (vkCreatePipelineLayout(scene->device, &plCI, nullptr, &scene->pipelineLayout) != VK_SUCCESS)
	{
		vkDestroyShaderModule(scene->device, vertMod, nullptr);
		vkDestroyShaderModule(scene->device, fragMod, nullptr);
		return false;
	}

	VkGraphicsPipelineCreateInfo gpCI = {};
	gpCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	gpCI.stageCount = 2;
	gpCI.pStages = stages;
	gpCI.pVertexInputState = &vi;
	gpCI.pInputAssemblyState = &ia;
	gpCI.pViewportState = &vp;
	gpCI.pRasterizationState = &rs;
	gpCI.pMultisampleState = &ms;
	gpCI.pColorBlendState = &cb;
	gpCI.pDynamicState = &dyn;
	gpCI.layout = scene->pipelineLayout;
	gpCI.renderPass = scene->renderPass;
	gpCI.subpass = 0;

	Bool succ = vkCreateGraphicsPipelines(scene->device, VK_NULL_HANDLE, 1, &gpCI, nullptr, &scene->pipeline) == VK_SUCCESS;
	vkDestroyShaderModule(scene->device, vertMod, nullptr);
	vkDestroyShaderModule(scene->device, fragMod, nullptr);
	if (!succ)
	{
		vkDestroyPipelineLayout(scene->device, scene->pipelineLayout, nullptr);
		scene->pipelineLayout = VK_NULL_HANDLE;
	}
	return succ;
#else
	(void)scene;
	return false;
#endif
}

static Bool VulkanEnsureVertexBuffer(NN<VulkanEngineScene> scene, UInt32 minVerts)
{
#if HAS_VULKAN_HEADER && defined(__linux__) && defined(GDK_WINDOWING_X11)
	if (minVerts == 0)
		return true;
	if (scene->vertexBuffer != VK_NULL_HANDLE && scene->vertexBufferVertCap >= minVerts)
		return true;

	vkDeviceWaitIdle(scene->device);
	if (scene->vertexBuffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(scene->device, scene->vertexBuffer, nullptr);
		scene->vertexBuffer = VK_NULL_HANDLE;
	}
	if (scene->vertexBufferMemory != VK_NULL_HANDLE)
	{
		vkFreeMemory(scene->device, scene->vertexBufferMemory, nullptr);
		scene->vertexBufferMemory = VK_NULL_HANDLE;
	}

	UInt32 newCap = 256;
	while (newCap < minVerts)
	{
		UInt32 nextCap = newCap << 1;
		if (nextCap <= newCap)
			return false;
		newCap = nextCap;
	}

	VkBufferCreateInfo bCI = {};
	bCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bCI.size = sizeof(VulkanDrawVertex) * (UInt64)newCap;
	bCI.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if (vkCreateBuffer(scene->device, &bCI, nullptr, &scene->vertexBuffer) != VK_SUCCESS)
		return false;

	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(scene->device, scene->vertexBuffer, &memReq);
	UInt32 memType = VulkanFindMemoryType(scene, memReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	if (memType == 0xffffffff)
		return false;

	VkMemoryAllocateInfo maI = {};
	maI.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	maI.allocationSize = memReq.size;
	maI.memoryTypeIndex = memType;
	if (vkAllocateMemory(scene->device, &maI, nullptr, &scene->vertexBufferMemory) != VK_SUCCESS)
		return false;

	if (vkBindBufferMemory(scene->device, scene->vertexBuffer, scene->vertexBufferMemory, 0) != VK_SUCCESS)
		return false;

	scene->vertexBufferVertCap = newCap;
	return true;
#else
	(void)scene;
	(void)minVerts;
	return false;
#endif
}

static Bool VulkanUploadSceneMesh(NN<VulkanEngineScene> scene)
{
#if HAS_VULKAN_HEADER && defined(__linux__) && defined(GDK_WINDOWING_X11)
	if (scene->drawVertCnt == 0)
		return true;
	if (!VulkanEnsureVertexBuffer(scene, scene->drawVertCnt))
		return false;
	void *mapPtr = nullptr;
	if (vkMapMemory(scene->device, scene->vertexBufferMemory, 0, sizeof(VulkanDrawVertex) * (UInt64)scene->drawVertCnt, 0, &mapPtr) != VK_SUCCESS)
		return false;
	memcpy(mapPtr, scene->drawVertices, sizeof(VulkanDrawVertex) * (UInt64)scene->drawVertCnt);
	vkUnmapMemory(scene->device, scene->vertexBufferMemory);
	return true;
#else
	(void)scene;
	return false;
#endif
}

static Bool VulkanInitScene(NN<VulkanEngineScene> scene)
{
#if HAS_VULKAN_HEADER && defined(__linux__) && defined(GDK_WINDOWING_X11)
	GtkWidget *widget = (GtkWidget*)scene->ctrlHdl.OrNull();
	if (widget == nullptr)
		return false;
	gtk_widget_realize(widget);
	GdkWindow *gdkWnd = gtk_widget_get_window(widget);
	GdkDisplay *gdkDisp = gtk_widget_get_display(widget);
	if (gdkWnd == nullptr || gdkDisp == nullptr)
		return false;
	Display *xDisp = gdk_x11_display_get_xdisplay(gdkDisp);
	Window xWnd = gdk_x11_window_get_xid(gdkWnd);
	if (xDisp == nullptr || xWnd == 0)
		return false;

	const Char *instExts[] =
	{
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_XLIB_SURFACE_EXTENSION_NAME
	};
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "SClass VulkanEngine";
	appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
	appInfo.pEngineName = "SClass";
	appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo instCI = {};
	instCI.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instCI.pApplicationInfo = &appInfo;
	instCI.enabledExtensionCount = 2;
	instCI.ppEnabledExtensionNames = instExts;
	if (vkCreateInstance(&instCI, nullptr, &scene->instance) != VK_SUCCESS)
		return false;

	VkXlibSurfaceCreateInfoKHR surfCI = {};
	surfCI.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
	surfCI.dpy = xDisp;
	surfCI.window = xWnd;
	if (vkCreateXlibSurfaceKHR(scene->instance, &surfCI, nullptr, &scene->surface) != VK_SUCCESS)
	{
		scene->ReleaseAll();
		return false;
	}

	UInt32 devCnt = 0;
	vkEnumeratePhysicalDevices(scene->instance, &devCnt, nullptr);
	if (devCnt == 0)
	{
		scene->ReleaseAll();
		return false;
	}
	VkPhysicalDevice *devs = new VkPhysicalDevice[devCnt];
	vkEnumeratePhysicalDevices(scene->instance, &devCnt, devs);

	Bool found = false;
	UInt32 i = 0;
	while (i < devCnt && !found)
	{
		UInt32 qCnt = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(devs[i], &qCnt, nullptr);
		VkQueueFamilyProperties *qProps = new VkQueueFamilyProperties[qCnt];
		vkGetPhysicalDeviceQueueFamilyProperties(devs[i], &qCnt, qProps);

		UInt32 gfxQ = 0xffffffff;
		UInt32 presentQ = 0xffffffff;
		UInt32 j = 0;
		while (j < qCnt)
		{
			VkBool32 presentSup = VK_FALSE;
			vkGetPhysicalDeviceSurfaceSupportKHR(devs[i], j, scene->surface, &presentSup);
			if ((qProps[j].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0 && gfxQ == 0xffffffff)
			{
				gfxQ = j;
			}
			if (presentSup != VK_FALSE && presentQ == 0xffffffff)
			{
				presentQ = j;
			}
			j++;
		}
		delete [] qProps;

		if (gfxQ != 0xffffffff && presentQ != 0xffffffff)
		{
			scene->phyDev = devs[i];
			scene->gfxQueueFamily = gfxQ;
			scene->presentQueueFamily = presentQ;
			found = true;
		}
		i++;
	}
	delete [] devs;
	if (!found)
	{
		scene->ReleaseAll();
		return false;
	}

	float qPriority = 1.0f;
	VkDeviceQueueCreateInfo qCIs[2];
	qCIs[0] = {};
	qCIs[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	qCIs[0].queueFamilyIndex = scene->gfxQueueFamily;
	qCIs[0].queueCount = 1;
	qCIs[0].pQueuePriorities = &qPriority;
	UInt32 qCICnt = 1;
	if (scene->presentQueueFamily != scene->gfxQueueFamily)
	{
		qCIs[1] = {};
		qCIs[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		qCIs[1].queueFamilyIndex = scene->presentQueueFamily;
		qCIs[1].queueCount = 1;
		qCIs[1].pQueuePriorities = &qPriority;
		qCICnt = 2;
	}

	const Char *devExts[] =
	{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	VkDeviceCreateInfo devCI = {};
	devCI.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	devCI.queueCreateInfoCount = qCICnt;
	devCI.pQueueCreateInfos = qCIs;
	devCI.enabledExtensionCount = 1;
	devCI.ppEnabledExtensionNames = devExts;
	if (vkCreateDevice(scene->phyDev, &devCI, nullptr, &scene->device) != VK_SUCCESS)
	{
		scene->ReleaseAll();
		return false;
	}

	vkGetDeviceQueue(scene->device, scene->gfxQueueFamily, 0, &scene->gfxQueue);
	vkGetDeviceQueue(scene->device, scene->presentQueueFamily, 0, &scene->presentQueue);

	VkAttachmentDescription colorAttach = {};
	colorAttach.format = VK_FORMAT_B8G8R8A8_UNORM;
	colorAttach.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttach.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttach.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttach.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttach.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttach.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttach.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorRef = {};
	colorRef.attachment = 0;
	colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorRef;

	VkRenderPassCreateInfo rpCI = {};
	rpCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	rpCI.attachmentCount = 1;
	rpCI.pAttachments = &colorAttach;
	rpCI.subpassCount = 1;
	rpCI.pSubpasses = &subpass;
	if (vkCreateRenderPass(scene->device, &rpCI, nullptr, &scene->renderPass) != VK_SUCCESS)
	{
		scene->ReleaseAll();
		return false;
	}

	VkCommandPoolCreateInfo cpCI = {};
	cpCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cpCI.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	cpCI.queueFamilyIndex = scene->gfxQueueFamily;
	if (vkCreateCommandPool(scene->device, &cpCI, nullptr, &scene->cmdPool) != VK_SUCCESS)
	{
		scene->ReleaseAll();
		return false;
	}

	VkSemaphoreCreateInfo semCI = {};
	semCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	if (vkCreateSemaphore(scene->device, &semCI, nullptr, &scene->imageAvailable) != VK_SUCCESS ||
		vkCreateSemaphore(scene->device, &semCI, nullptr, &scene->renderFinished) != VK_SUCCESS)
	{
		scene->ReleaseAll();
		return false;
	}

	VkFenceCreateInfo fenceCI = {};
	fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCI.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	if (vkCreateFence(scene->device, &fenceCI, nullptr, &scene->inFlightFence) != VK_SUCCESS)
	{
		scene->ReleaseAll();
		return false;
	}
	if (!VulkanCreateGraphicsPipeline(scene))
	{
		scene->ReleaseAll();
		return false;
	}

	scene->initialized = true;
	return true;
#else
	(void)scene;
	return false;
#endif
}

static Bool VulkanCreateSwapchain(NN<VulkanEngineScene> scene, Math::Size2D<UInt32> frameSize)
{
#if HAS_VULKAN_HEADER && defined(__linux__) && defined(GDK_WINDOWING_X11)
	if (!scene->initialized)
		return false;
	if (frameSize.x == 0 || frameSize.y == 0)
		return false;

	vkDeviceWaitIdle(scene->device);
	scene->ReleaseSwapchainResources();

	VkSurfaceCapabilitiesKHR caps;
	if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(scene->phyDev, scene->surface, &caps) != VK_SUCCESS)
		return false;

	UInt32 fmtCnt = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(scene->phyDev, scene->surface, &fmtCnt, nullptr);
	if (fmtCnt == 0)
		return false;
	VkSurfaceFormatKHR *formats = new VkSurfaceFormatKHR[fmtCnt];
	vkGetPhysicalDeviceSurfaceFormatsKHR(scene->phyDev, scene->surface, &fmtCnt, formats);

	VkSurfaceFormatKHR chosenFmt = formats[0];
	UInt32 i = 0;
	while (i < fmtCnt)
	{
		if (formats[i].format == VK_FORMAT_B8G8R8A8_UNORM && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			chosenFmt = formats[i];
			break;
		}
		i++;
	}
	delete [] formats;

	UInt32 pmCnt = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(scene->phyDev, scene->surface, &pmCnt, nullptr);
	VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
	if (pmCnt > 0)
	{
		VkPresentModeKHR *pms = new VkPresentModeKHR[pmCnt];
		vkGetPhysicalDeviceSurfacePresentModesKHR(scene->phyDev, scene->surface, &pmCnt, pms);
		i = 0;
		while (i < pmCnt)
		{
			if (pms[i] == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
				break;
			}
			i++;
		}
		delete [] pms;
	}

	VkExtent2D extent;
	if (caps.currentExtent.width != 0xffffffff)
	{
		extent = caps.currentExtent;
	}
	else
	{
		extent.width = frameSize.x;
		extent.height = frameSize.y;
		if (extent.width < caps.minImageExtent.width)
			extent.width = caps.minImageExtent.width;
		if (extent.height < caps.minImageExtent.height)
			extent.height = caps.minImageExtent.height;
		if (extent.width > caps.maxImageExtent.width)
			extent.width = caps.maxImageExtent.width;
		if (extent.height > caps.maxImageExtent.height)
			extent.height = caps.maxImageExtent.height;
	}

	UInt32 imageCount = caps.minImageCount + 1;
	if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount)
	{
		imageCount = caps.maxImageCount;
	}

	UInt32 qIndices[2] = {scene->gfxQueueFamily, scene->presentQueueFamily};
	VkSwapchainCreateInfoKHR swCI = {};
	swCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swCI.surface = scene->surface;
	swCI.minImageCount = imageCount;
	swCI.imageFormat = chosenFmt.format;
	swCI.imageColorSpace = chosenFmt.colorSpace;
	swCI.imageExtent = extent;
	swCI.imageArrayLayers = 1;
	swCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	if (scene->gfxQueueFamily != scene->presentQueueFamily)
	{
		swCI.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swCI.queueFamilyIndexCount = 2;
		swCI.pQueueFamilyIndices = qIndices;
	}
	else
	{
		swCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}
	swCI.preTransform = caps.currentTransform;
	swCI.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swCI.presentMode = presentMode;
	swCI.clipped = VK_TRUE;
	swCI.oldSwapchain = VK_NULL_HANDLE;
	if (vkCreateSwapchainKHR(scene->device, &swCI, nullptr, &scene->swapchain) != VK_SUCCESS)
		return false;

	scene->swapFmt = chosenFmt.format;
	scene->extent = extent;

	vkGetSwapchainImagesKHR(scene->device, scene->swapchain, &scene->imageCnt, nullptr);
	if (scene->imageCnt == 0)
		return false;
	scene->images = new VkImage[scene->imageCnt];
	scene->imageViews = new VkImageView[scene->imageCnt];
	scene->framebuffers = new VkFramebuffer[scene->imageCnt];
	scene->cmdBuffers = new VkCommandBuffer[scene->imageCnt];
	vkGetSwapchainImagesKHR(scene->device, scene->swapchain, &scene->imageCnt, scene->images);

	i = 0;
	while (i < scene->imageCnt)
	{
		VkImageViewCreateInfo ivCI = {};
		ivCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ivCI.image = scene->images[i];
		ivCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ivCI.format = scene->swapFmt;
		ivCI.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		ivCI.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		ivCI.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		ivCI.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		ivCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		ivCI.subresourceRange.baseMipLevel = 0;
		ivCI.subresourceRange.levelCount = 1;
		ivCI.subresourceRange.baseArrayLayer = 0;
		ivCI.subresourceRange.layerCount = 1;
		if (vkCreateImageView(scene->device, &ivCI, nullptr, &scene->imageViews[i]) != VK_SUCCESS)
			return false;

		VkImageView fbAtt[1] = {scene->imageViews[i]};
		VkFramebufferCreateInfo fbCI = {};
		fbCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		fbCI.renderPass = scene->renderPass;
		fbCI.attachmentCount = 1;
		fbCI.pAttachments = fbAtt;
		fbCI.width = scene->extent.width;
		fbCI.height = scene->extent.height;
		fbCI.layers = 1;
		if (vkCreateFramebuffer(scene->device, &fbCI, nullptr, &scene->framebuffers[i]) != VK_SUCCESS)
			return false;
		i++;
	}

	VkCommandBufferAllocateInfo cbAI = {};
	cbAI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cbAI.commandPool = scene->cmdPool;
	cbAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cbAI.commandBufferCount = scene->imageCnt;
	if (vkAllocateCommandBuffers(scene->device, &cbAI, scene->cmdBuffers) != VK_SUCCESS)
		return false;

	return true;
#else
	(void)scene;
	(void)frameSize;
	return false;
#endif
}

static Bool VulkanProjectPoint(
	const Media::Engine3D::CameraState &camera,
	Math::Vector3 pos,
	float *outX,
	float *outY,
	float *outZ)
{
	Math::Vector3 fwd = camera.targetPos - camera.camPos;
	Double fwdLen = fwd.GetLength();
	if (fwdLen < 0.000001)
		return false;
	fwd = fwd / fwdLen;

	Math::Vector3 upRef(0, 0, 1);
	Math::Vector3 right = fwd.CrossProduct(upRef);
	Double rightLen = right.GetLength();
	if (rightLen < 0.000001)
	{
		right = Math::Vector3(1, 0, 0);
	}
	else
	{
		right = right / rightLen;
	}
	Math::Vector3 up = right.CrossProduct(fwd);
	Double upLen = up.GetLength();
	if (upLen < 0.000001)
		return false;
	up = up / upLen;

	Math::Vector3 rel = pos - camera.camPos;
	Double viewX = rel.DotProduct(right);
	Double viewY = rel.DotProduct(up);
	Double viewZ = rel.DotProduct(fwd);
	if (viewZ <= 0.1)
		return false;

	Double aspect = 1.0;
	Double tanHalfFov = Math_Tan(camera.fovDeg * Math::PI / 360.0);
	if (tanHalfFov < 0.0001)
		tanHalfFov = 0.0001;
	Double ndcX = viewX / (viewZ * tanHalfFov * aspect);
	Double ndcY = viewY / (viewZ * tanHalfFov);
	Double ndcZ = viewZ / (viewZ + 1000.0);
	if (ndcX < -2.0 || ndcX > 2.0 || ndcY < -2.0 || ndcY > 2.0)
		return false;

	*outX = (float)ndcX;
	*outY = (float)ndcY;
	*outZ = (float)ndcZ;
	return true;
}

static void VulkanColorToFloat(UInt32 color, float *r, float *g, float *b)
{
	*r = (float)(UInt32)((color >> 16) & 0xff) / 255.0f;
	*g = (float)(UInt32)((color >> 8) & 0xff) / 255.0f;
	*b = (float)(UInt32)(color & 0xff) / 255.0f;
}

static Bool VulkanBuildSceneMesh(
	NN<VulkanEngineScene> scene,
	const Media::Engine3D::CameraState &camera,
	NN<const Data::ArrayListNN<Media::Engine3D::SceneObject>> sceneObjs)
{
	scene->ClearSceneMesh();

	UIntOS i = 0;
	UIntOS objCnt = sceneObjs->GetCount();
	while (i < objCnt)
	{
		NN<const Media::Engine3D::SceneObject> obj = sceneObjs->GetItemNoCheck(i);
		UIntOS ptCnt = obj->points.GetCount();
		if (ptCnt == 0)
		{
			i++;
			continue;
		}

		float cr;
		float cg;
		float cb;
		VulkanColorToFloat(obj->color, &cr, &cg, &cb);

		if (obj->objType == Media::Engine3D::SceneObjectType::Polygon)
		{
			if (ptCnt >= 3)
			{
				Math::Coord2DDbl c0 = obj->points.GetItem(0);
				UIntOS j = 1;
				while (j + 1 < ptCnt)
				{
					Math::Coord2DDbl c1 = obj->points.GetItem(j);
					Math::Coord2DDbl c2 = obj->points.GetItem(j + 1);
					VulkanDrawVertex v0;
					VulkanDrawVertex v1;
					VulkanDrawVertex v2;
					if (VulkanProjectPoint(camera, Math::Vector3(c0, obj->z), &v0.x, &v0.y, &v0.z) &&
						VulkanProjectPoint(camera, Math::Vector3(c1, obj->z), &v1.x, &v1.y, &v1.z) &&
						VulkanProjectPoint(camera, Math::Vector3(c2, obj->z), &v2.x, &v2.y, &v2.z))
					{
						v0.r = cr; v0.g = cg; v0.b = cb;
						v1.r = cr; v1.g = cg; v1.b = cb;
						v2.r = cr; v2.g = cg; v2.b = cb;
						scene->AddTriangle(v0, v1, v2);
					}
					j++;
				}
			}
		}
		else
		{
			Double objSize = 2.0;
			if (obj->objType == Media::Engine3D::SceneObjectType::Point)
			{
				UIntOS j = 0;
				while (j < ptCnt)
				{
					Math::Coord2DDbl c = obj->points.GetItem(j);
					Math::Coord2DDbl p0(c.x - objSize, c.y - objSize);
					Math::Coord2DDbl p1(c.x + objSize, c.y - objSize);
					Math::Coord2DDbl p2(c.x + objSize, c.y + objSize);
					Math::Coord2DDbl p3(c.x - objSize, c.y + objSize);
					VulkanDrawVertex v0;
					VulkanDrawVertex v1;
					VulkanDrawVertex v2;
					VulkanDrawVertex v3;
					if (VulkanProjectPoint(camera, Math::Vector3(p0, obj->z), &v0.x, &v0.y, &v0.z) &&
						VulkanProjectPoint(camera, Math::Vector3(p1, obj->z), &v1.x, &v1.y, &v1.z) &&
						VulkanProjectPoint(camera, Math::Vector3(p2, obj->z), &v2.x, &v2.y, &v2.z) &&
						VulkanProjectPoint(camera, Math::Vector3(p3, obj->z), &v3.x, &v3.y, &v3.z))
					{
						v0.r = cr; v0.g = cg; v0.b = cb;
						v1.r = cr; v1.g = cg; v1.b = cb;
						v2.r = cr; v2.g = cg; v2.b = cb;
						v3.r = cr; v3.g = cg; v3.b = cb;
						scene->AddTriangle(v0, v1, v2);
						scene->AddTriangle(v0, v2, v3);
					}
					j++;
				}
			}
			else if (ptCnt >= 2)
			{
				UIntOS j = 1;
				while (j < ptCnt)
				{
					Math::Coord2DDbl a = obj->points.GetItem(j - 1);
					Math::Coord2DDbl bpt = obj->points.GetItem(j);
					Math::Coord2DDbl dir = bpt - a;
					Double dlen = dir.CalcLengTo(Math::Coord2DDbl(0, 0));
					if (dlen > 0.00001)
					{
						dir = dir / dlen;
						Math::Coord2DDbl perp(-dir.y * objSize, dir.x * objSize);
						Math::Coord2DDbl p0 = a - perp;
						Math::Coord2DDbl p1 = a + perp;
						Math::Coord2DDbl p2 = bpt + perp;
						Math::Coord2DDbl p3 = bpt - perp;
						VulkanDrawVertex v0;
						VulkanDrawVertex v1;
						VulkanDrawVertex v2;
						VulkanDrawVertex v3;
						if (VulkanProjectPoint(camera, Math::Vector3(p0, obj->z), &v0.x, &v0.y, &v0.z) &&
							VulkanProjectPoint(camera, Math::Vector3(p1, obj->z), &v1.x, &v1.y, &v1.z) &&
							VulkanProjectPoint(camera, Math::Vector3(p2, obj->z), &v2.x, &v2.y, &v2.z) &&
							VulkanProjectPoint(camera, Math::Vector3(p3, obj->z), &v3.x, &v3.y, &v3.z))
						{
							v0.r = cr; v0.g = cg; v0.b = cb;
							v1.r = cr; v1.g = cg; v1.b = cb;
							v2.r = cr; v2.g = cg; v2.b = cb;
							v3.r = cr; v3.g = cg; v3.b = cb;
							scene->AddTriangle(v0, v1, v2);
							scene->AddTriangle(v0, v2, v3);
						}
					}
					j++;
				}
			}
		}
		i++;
	}
	return true;
}

static Bool VulkanDrawFrame(NN<VulkanEngineScene> scene, float clearR, float clearG, float clearB)
{
#if HAS_VULKAN_HEADER && defined(__linux__) && defined(GDK_WINDOWING_X11)
	if (scene->swapchain == VK_NULL_HANDLE)
		return false;
	if (vkWaitForFences(scene->device, 1, &scene->inFlightFence, VK_TRUE, 1000000000) != VK_SUCCESS)
		return false;
	if (vkResetFences(scene->device, 1, &scene->inFlightFence) != VK_SUCCESS)
		return false;

	UInt32 imageIndex = 0;
	VkResult rr = vkAcquireNextImageKHR(scene->device, scene->swapchain, 1000000000, scene->imageAvailable, VK_NULL_HANDLE, &imageIndex);
	if (rr == VK_ERROR_OUT_OF_DATE_KHR)
		return false;
	if (rr != VK_SUCCESS && rr != VK_SUBOPTIMAL_KHR)
		return false;

	vkResetCommandBuffer(scene->cmdBuffers[imageIndex], 0);

	VkCommandBufferBeginInfo cbBI = {};
	cbBI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vkBeginCommandBuffer(scene->cmdBuffers[imageIndex], &cbBI);

	VkClearValue cv = {};
	cv.color.float32[0] = clearR;
	cv.color.float32[1] = clearG;
	cv.color.float32[2] = clearB;
	cv.color.float32[3] = 1.0f;

	VkRenderPassBeginInfo rpBI = {};
	rpBI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	rpBI.renderPass = scene->renderPass;
	rpBI.framebuffer = scene->framebuffers[imageIndex];
	rpBI.renderArea.offset.x = 0;
	rpBI.renderArea.offset.y = 0;
	rpBI.renderArea.extent = scene->extent;
	rpBI.clearValueCount = 1;
	rpBI.pClearValues = &cv;

	vkCmdBeginRenderPass(scene->cmdBuffers[imageIndex], &rpBI, VK_SUBPASS_CONTENTS_INLINE);
	VkViewport vp;
	vp.x = 0;
	vp.y = 0;
	vp.width = (float)scene->extent.width;
	vp.height = (float)scene->extent.height;
	vp.minDepth = 0.0f;
	vp.maxDepth = 1.0f;
	vkCmdSetViewport(scene->cmdBuffers[imageIndex], 0, 1, &vp);
	VkRect2D scissor;
	scissor.offset.x = 0;
	scissor.offset.y = 0;
	scissor.extent = scene->extent;
	vkCmdSetScissor(scene->cmdBuffers[imageIndex], 0, 1, &scissor);
	if (scene->pipeline != VK_NULL_HANDLE && scene->vertexBuffer != VK_NULL_HANDLE && scene->drawVertCnt > 0)
	{
		vkCmdBindPipeline(scene->cmdBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, scene->pipeline);
		VkDeviceSize ofst = 0;
		vkCmdBindVertexBuffers(scene->cmdBuffers[imageIndex], 0, 1, &scene->vertexBuffer, &ofst);
		vkCmdDraw(scene->cmdBuffers[imageIndex], scene->drawVertCnt, 1, 0, 0);
	}
	vkCmdEndRenderPass(scene->cmdBuffers[imageIndex]);
	vkEndCommandBuffer(scene->cmdBuffers[imageIndex]);

	VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo submit = {};
	submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit.waitSemaphoreCount = 1;
	submit.pWaitSemaphores = &scene->imageAvailable;
	submit.pWaitDstStageMask = &waitStage;
	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &scene->cmdBuffers[imageIndex];
	submit.signalSemaphoreCount = 1;
	submit.pSignalSemaphores = &scene->renderFinished;
	if (vkQueueSubmit(scene->gfxQueue, 1, &submit, scene->inFlightFence) != VK_SUCCESS)
		return false;

	VkPresentInfoKHR present = {};
	present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present.waitSemaphoreCount = 1;
	present.pWaitSemaphores = &scene->renderFinished;
	present.swapchainCount = 1;
	present.pSwapchains = &scene->swapchain;
	present.pImageIndices = &imageIndex;
	rr = vkQueuePresentKHR(scene->presentQueue, &present);
	if (rr == VK_ERROR_OUT_OF_DATE_KHR || rr == VK_SUBOPTIMAL_KHR)
		return false;
	return rr == VK_SUCCESS;
#else
	(void)scene;
	(void)clearR;
	(void)clearG;
	(void)clearB;
	return false;
#endif
}

Media::VulkanVertexBuffer::VulkanVertexBuffer(UInt32 vertexCnt)
{
	this->vertexCnt = vertexCnt;
}

UInt32 Media::VulkanVertexBuffer::GetVertexCount() const
{
	return this->vertexCnt;
}

Media::VulkanIndexBuffer::VulkanIndexBuffer(UInt32 indexCnt)
{
	this->indexCnt = indexCnt;
}

UInt32 Media::VulkanIndexBuffer::GetIndexCount() const
{
	return this->indexCnt;
}

Media::VulkanPipelineState::VulkanPipelineState(Bool depthEnabled)
{
	this->depthEnabled = depthEnabled;
}

Bool Media::VulkanPipelineState::IsDepthEnabled() const
{
	return this->depthEnabled;
}

Media::VulkanEngine::VulkanEngine()
{
	this->currScene = nullptr;
	this->frameSize = Math::Size2D<UInt32>(0, 0);
	this->vulkanHdrPresent = HAS_VULKAN_HEADER != 0;
	this->err = !this->vulkanHdrPresent;
}

Media::VulkanEngine::~VulkanEngine()
{
}

Text::CStringNN Media::VulkanEngine::GetEngineName() const
{
	return CSTR("VulkanEngine");
}

Bool Media::VulkanEngine::IsError() const
{
	return this->err;
}

Optional<Media::Engine3DScene> Media::VulkanEngine::CreateScene(Optional<ControlHandle> ctrlHdl)
{
#if HAS_VULKAN_HEADER && defined(__linux__) && defined(GDK_WINDOWING_X11)
	NN<VulkanEngineScene> scene;
	NEW_CLASSNN(scene, VulkanEngineScene(ctrlHdl));
	if (!VulkanInitScene(scene))
	{
		scene.Delete();
		this->err = true;
		return nullptr;
	}
	this->err = false;
	return scene;
#else
	(void)ctrlHdl;
	this->err = true;
	return nullptr;
#endif
}

void Media::VulkanEngine::DeleteScene(NN<Engine3DScene> scene)
{
	#if HAS_VULKAN_HEADER && defined(__linux__) && defined(GDK_WINDOWING_X11)
	if (this->currScene.OrNull() == scene.Ptr())
	{
		this->currScene = nullptr;
	}
	VulkanEngineScene *vkScene = (VulkanEngineScene*)scene.Ptr();
	vkScene->ReleaseAll();
	#endif
	scene.Delete();
}

Bool Media::VulkanEngine::BeginFrame(NN<Engine3DScene> scene, Math::Size2D<UInt32> frameSize)
{
	#if HAS_VULKAN_HEADER && defined(__linux__) && defined(GDK_WINDOWING_X11)
	VulkanEngineScene *vkScene = (VulkanEngineScene*)scene.Ptr();
	if (!vkScene->initialized)
		return false;
	if (frameSize.x == 0 || frameSize.y == 0)
		return false;

	if (vkScene->swapchain == VK_NULL_HANDLE || vkScene->extent.width != frameSize.x || vkScene->extent.height != frameSize.y)
	{
		if (!VulkanCreateSwapchain(*vkScene, frameSize))
		{
			this->currScene = nullptr;
			return false;
		}
	}
	this->currScene = scene;
	this->frameSize = frameSize;
	return true;
	#else
	(void)scene;
	(void)frameSize;
	return false;
	#endif
}

Bool Media::VulkanEngine::RenderScene(const CameraState &camera, NN<const Data::ArrayListNN<SceneObject>> sceneObjs)
{
	#if HAS_VULKAN_HEADER && defined(__linux__) && defined(GDK_WINDOWING_X11)
	NN<Engine3DScene> scene;
	if (!this->currScene.SetTo(scene))
		return false;
	VulkanEngineScene *vkScene = (VulkanEngineScene*)scene.Ptr();
	VulkanBuildSceneMesh(*vkScene, camera, sceneObjs);
	if (!VulkanUploadSceneMesh(*vkScene))
		return false;
	UIntOS sceneObjCnt = sceneObjs->GetCount();
	UIntOS triCnt = vkScene->drawVertCnt / 3;

	float clearR = 0.08f + (float)(Math_Cos(camera.hAngle) * 0.04);
	float clearG = 0.12f + (float)(Math_Sin(camera.hAngle) * 0.04);
	float clearB = 0.18f + (float)(Math_Sin(camera.vAngle) * 0.05);
	clearG += (float)(UIntOS2Double(sceneObjCnt & 31) * 0.002);
	clearR += (float)(UIntOS2Double(triCnt & 63) * 0.001);
	if (clearR < 0.0f) clearR = 0.0f;
	if (clearG < 0.0f) clearG = 0.0f;
	if (clearB < 0.0f) clearB = 0.0f;
	if (clearR > 1.0f) clearR = 1.0f;
	if (clearG > 1.0f) clearG = 1.0f;
	if (clearB > 1.0f) clearB = 1.0f;

	if (!VulkanDrawFrame(*vkScene, clearR, clearG, clearB))
	{
		if (vkScene->initialized)
		{
			if (VulkanCreateSwapchain(*vkScene, this->frameSize))
			{
				return VulkanDrawFrame(*vkScene, clearR, clearG, clearB);
			}
		}
		return false;
	}
	return true;
	#else
	(void)camera;
	(void)sceneObjs;
	return false;
	#endif
}

void Media::VulkanEngine::EndFrame()
{
	this->currScene = nullptr;
}
