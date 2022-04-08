#ifndef _SM_MAP_MAPSCHEDULER
#define _SM_MAP_MAPSCHEDULER
#include "Sync/Mutex.h"
#include "Sync/Event.h"
#include "Data/ArrayList.h"
#include "Media/DrawEngine.h"
#include "Map/MapView.h"
#include "Map/IMapDrawLayer.h"

namespace Map
{
	class MapScheduler
	{
	public:
		enum DrawType
		{
			MSDT_POINTS,
			MSDT_POLYLINE,
			MSDT_POLYGON,
			MSDT_CLEAR
		};

	protected:
		Map::MapView *map;
		Map::IMapDrawLayer *lyr;
		Media::DrawImage *img;
		Media::DrawPen *p;
		Media::DrawBrush *b;
		Media::DrawImage *ico;
		Double icoSpotX;
		Double icoSpotY;
		DrawType dt;
		Double *objBounds;
		UOSInt *objCnt;
		UOSInt maxCnt;

		Sync::Mutex taskMut;
		Data::ArrayList<Map::DrawObjectL *> tasks;
		Bool toStop;
		Sync::Event taskEvt;
		Sync::Event finishEvt;
		Bool threadRunning;
		Bool taskFinish;
		Bool isFirst;
		Bool *isLayerEmpty;

	private:
		static UInt32 __stdcall MapThread(void *obj);

	protected:
		virtual void DrawPoints(Map::DrawObjectL *dobj);
	public:
		MapScheduler();
		virtual ~MapScheduler();

		void SetMapView(Map::MapView *map, Media::DrawImage *img);
		void SetDrawType(Map::IMapDrawLayer *lyr, DrawType dt, Media::DrawPen *p, Media::DrawBrush *b, Media::DrawImage *ico, Double icoSpotX, Double icoSpotY, Bool *isLayerEmpty);
		void SetDrawObjs(Double *objBounds, UOSInt *objCnt, UOSInt maxCnt);
		void Draw(Map::DrawObjectL *obj);
		void DrawNextType(Media::DrawPen *p, Media::DrawBrush *b);
		void WaitForFinish();
	};
}
#endif
