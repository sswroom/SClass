#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/Engine3DFactory.h"
#include "Media/NullEngine3D.h"
#include "Media/VulkanEngine.h"

NN<Media::Engine3D> Media::Engine3DFactory::CreateEngine3D()
{
	NN<Media::VulkanEngine> vkEng;
	NEW_CLASSNN(vkEng, Media::VulkanEngine());
	if (!vkEng->IsError())
	{
		return vkEng;
	}
	vkEng.Delete();

	NN<Media::NullEngine3D> nullEng;
	NEW_CLASSNN(nullEng, Media::NullEngine3D());
	return nullEng;
}
