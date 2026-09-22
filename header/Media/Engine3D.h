#ifndef _SM_MEDIA_ENGINE3D
#define _SM_MEDIA_ENGINE3D
#include "Data/ArrayListA.hpp"
#include "Data/ArrayListNN.hpp"
#include "Handles.h"
#include "Math/Coord2DDbl.h"
#include "Math/Size2D.h"
#include "Math/Vector3.h"
#include "Text/CString.h"

namespace Media
{
	class Engine3DMaterial
	{
	protected:
		Engine3DMaterial()
		{
		}
	};

	class Engine3DMesh
	{
	protected:
		Engine3DMesh()
		{
		}
	};

	class Engine3DScene
	{
	protected:
		Engine3DScene()
		{
		}

	public:
		virtual ~Engine3DScene()
		{
		}
	};

	class Engine3D
	{
	public:
		enum class SceneObjectType
		{
			Point,
			LineString,
			Polygon
		};

		struct SceneObject
		{
			SceneObjectType objType;
			UInt32 color;
			Data::ArrayListA<Math::Coord2DDbl> points;
			Double z;

			SceneObject(SceneObjectType objType, UInt32 color) : points(32)
			{
				this->objType = objType;
				this->color = color;
				this->z = 0;
			}
		};

		struct CameraState
		{
			Math::Vector3 camPos;
			Math::Vector3 targetPos;
			Double fovDeg;
			Double vAngle;
			Double hAngle;
		};

	public:
		virtual ~Engine3D(){};
		virtual Text::CStringNN GetEngineName() const = 0;
		virtual Bool IsError() const = 0;
		virtual Optional<Engine3DScene> CreateScene(Optional<ControlHandle> ctrlHdl) = 0;
		virtual void DeleteScene(NN<Engine3DScene> scene) = 0;
		virtual Bool BeginFrame(NN<Engine3DScene> scene, Math::Size2D<UInt32> frameSize) = 0;
		virtual Bool RenderScene(const CameraState &camera, NN<const Data::ArrayListNN<SceneObject>> sceneObjs) = 0;
		virtual void EndFrame() = 0;
	};
}
#endif