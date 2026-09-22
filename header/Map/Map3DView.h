#ifndef _SM_MAP_MAP3DVIEW
#define _SM_MAP_MAP3DVIEW
#include "Map/MapEnv.h"
#include "Math/Coord2D.h"
#include "Math/Size2DDbl.h"
#include "Media/Engine3D.h"

namespace Map
{
	class Map3DView
	{
	private:
		NN<Map::MapEnv> env;
		NN<Media::Engine3D> eng3d;
		Math::Size2DDbl scnSize;
		Math::Coord2DDbl mapCenter;
		Double camDistance;
		Double vAngle;
		Double hAngle;
		Double fovDeg;

	public:
		Map3DView(NN<Map::MapEnv> env, NN<Media::Engine3D> eng3d, Math::Size2DDbl scnSize);
		~Map3DView();

		void SetScreenSize(Math::Size2DDbl scnSize);
		void SetCenter(Math::Coord2DDbl mapCenter);
		Math::Coord2DDbl GetCenter() const;
		void Orbit(Double hDiff, Double vDiff);
		void PanScreen(Math::Coord2D<IntOS> diff);
		void Zoom(Double ratio);
		void SetVAngle(Double vAngle);
		void SetHAngle(Double hAngle);
		Double GetVAngle() const;
		Double GetHAngle() const;
		Bool Render(NN<Media::Engine3DScene> scene3d, Math::Size2D<UInt32> frameSize);
	};
}
#endif