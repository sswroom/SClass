#include "Stdafx.h"
#include "Media/NullEngine3D.h"

class NullEngine3DScene : public Media::Engine3DScene
{
public:
	Optional<ControlHandle> ctrlHdl;

	NullEngine3DScene(Optional<ControlHandle> ctrlHdl)
	{
		this->ctrlHdl = ctrlHdl;
	}
};

Media::NullEngine3D::NullEngine3D()
{
}

Media::NullEngine3D::~NullEngine3D()
{
}

Text::CStringNN Media::NullEngine3D::GetEngineName() const
{
	return CSTR("NullEngine3D");
}

Bool Media::NullEngine3D::IsError() const
{
	return false;
}

Optional<Media::Engine3DScene> Media::NullEngine3D::CreateScene(Optional<ControlHandle> ctrlHdl)
{
	NN<NullEngine3DScene> scene;
	NEW_CLASSNN(scene, NullEngine3DScene(ctrlHdl));
	return scene;
}

void Media::NullEngine3D::DeleteScene(NN<Engine3DScene> scene)
{
	scene.Delete();
}

Bool Media::NullEngine3D::BeginFrame(NN<Engine3DScene> scene, Math::Size2D<UInt32> frameSize)
{
	return true;
}

Bool Media::NullEngine3D::RenderScene(const CameraState &camera, NN<const Data::ArrayListNN<SceneObject>> sceneObjs)
{
	(void)camera;
	(void)sceneObjs;
	return false;
}

void Media::NullEngine3D::EndFrame()
{
}
