#ifndef _SM_MAP_DRAWMAPSERVICELAYER
#define _SM_MAP_DRAWMAPSERVICELAYER
#include "Data/SyncLinkedList.h"
#include "Map/DrawMapService.h"
#include "Map/IMapDrawLayer.h"
#include "Map/MapRenderer.h"
#include "Media/SharedImage.h"

namespace Map
{
	class DrawMapServiceLayer : public Map::IMapDrawLayer
	{
	private:
		Map::DrawMapService *mapService;

		Bool threadToStop;
		Bool threadRunning;
		Sync::Event *threadEvt;

		Sync::Mutex dispMut;
		Math::RectAreaDbl dispBounds;
		Math::Size2D<Double> dispSize;
		Double dispDPI;
		Int64 dispId;
		Media::SharedImage *dispImage;
		Text::String *dispImageURL;

		Sync::Mutex updMut;
		Data::ArrayList<UpdatedHandler> updHdlrs;
		Data::ArrayList<void *> updObjs;

		static UInt32 __stdcall TaskThread(void *userObj);
		void ClearDisp();
	public:
		DrawMapServiceLayer(Map::DrawMapService *mapService);
		virtual ~DrawMapServiceLayer();

		virtual void SetCurrScale(Double scale);
		virtual Map::MapView *CreateMapView(Math::Size2D<Double> scnSize);

		virtual DrawLayerType GetLayerType();
		virtual UOSInt GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr);
		virtual UOSInt GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax();
		virtual void ReleaseNameArr(void *nameArr);
		virtual UTF8Char *GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt();
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, DB::ColDef *colDef);
		virtual UInt32 GetCodePage();
		virtual Bool GetBounds(Math::RectAreaDbl *bounds);
		virtual void SetDispSize(Math::Size2D<Double> size, Double dpi);

		virtual void *BeginGetObject();
		virtual void EndGetObject(void *session);
		virtual Math::Geometry::Vector2D *GetNewVectorById(void *session, Int64 id);

		virtual ObjectClass GetObjectClass();

		virtual Bool CanQuery();
		virtual Math::Geometry::Vector2D *QueryInfo(Math::Coord2DDbl coord, Data::ArrayList<Text::String*> *nameList, Data::ArrayList<Text::String*> *valueList);

		virtual void AddUpdatedHandler(UpdatedHandler hdlr, void *obj);
		virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj);
		Map::DrawMapService *GetDrawMapService();
	};
}
#endif
