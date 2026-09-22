#ifndef _SM_MEDIA_NULLENGINE3D
#define _SM_MEDIA_NULLENGINE3D
#include "Media/Engine3D.h"

namespace Media
{
	class NullEngine3D : public Engine3D
	{
	public:
		NullEngine3D();
		virtual ~NullEngine3D();

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