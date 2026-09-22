#ifndef _SM_MEDIA_VULKANENGINE
#define _SM_MEDIA_VULKANENGINE
#include "Media/Engine3D.h"

namespace Media
{
	class VulkanVertexBuffer
	{
	private:
		UInt32 vertexCnt;
	public:
		VulkanVertexBuffer(UInt32 vertexCnt);
		UInt32 GetVertexCount() const;
	};

	class VulkanIndexBuffer
	{
	private:
		UInt32 indexCnt;
	public:
		VulkanIndexBuffer(UInt32 indexCnt);
		UInt32 GetIndexCount() const;
	};

	class VulkanPipelineState
	{
	private:
		Bool depthEnabled;
	public:
		VulkanPipelineState(Bool depthEnabled);
		Bool IsDepthEnabled() const;
	};

	class VulkanEngine : public Engine3D
	{
	private:
		Bool err;
		Bool vulkanHdrPresent;
		Optional<Media::Engine3DScene> currScene;
		Math::Size2D<UInt32> frameSize;

	public:
		VulkanEngine();
		virtual ~VulkanEngine();

		virtual Text::CStringNN GetEngineName() const;
		virtual Bool IsError() const;
		virtual Optional<Engine3DScene> CreateScene(Optional<ControlHandle> ctrlHdl);
		virtual void DeleteScene(NN<Engine3DScene> scene);
		virtual Bool BeginFrame(NN<Engine3DScene> scene, Math::Size2D<UInt32> frameSize);
		virtual Bool RenderScene(const CameraState &camera, NN<const Data::ArrayListNN<SceneObject>> sceneObjs);
		virtual void EndFrame();
	};
}
#endif