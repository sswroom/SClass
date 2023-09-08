#ifndef _SM_MAP_DRAWMAPSERVICELAYER
#define _SM_MAP_DRAWMAPSERVICELAYER
#include "Data/SyncLinkedList.h"
#include "Map/DrawMapService.h"
#include "Map/MapDrawLayer.h"
#include "Map/MapRenderer.h"
#include "Media/SharedImage.h"

namespace Map
{
	class DrawMapServiceLayer : public Map::MapDrawLayer
	{
	private:
		Map::DrawMapService *mapService;

		Bool threadToStop;
		Bool threadRunning;
		Sync::Event *threadEvt;

		Sync::Mutex dispMut;
		Math::RectAreaDbl dispBounds;
		Math::Size2DDbl dispSize;
		Double dispDPI;
		Int64 dispId;
		Media::SharedImage *dispImage;
		Text::String *dispImageURL;
		Math::RectAreaDbl lastBounds;
		Math::Size2DDbl lastSize;
		Double lastDPI;
		Int64 lastId;
		Media::SharedImage *lastImage;
		Text::String *lastImageURL;

		Sync::Mutex updMut;
		Data::ArrayList<UpdatedHandler> updHdlrs;
		Data::ArrayList<void *> updObjs;

		static UInt32 __stdcall TaskThread(void *userObj);
		void ClearDisp();
	public:
		DrawMapServiceLayer(Map::DrawMapService *mapService);
		virtual ~DrawMapServiceLayer();

		virtual void SetCurrScale(Double scale);
		virtual Map::MapView *CreateMapView(Math::Size2DDbl scnSize);

		virtual DrawLayerType GetLayerType() const;
		virtual UOSInt GetAllObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr);
		virtual UOSInt GetObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax() const;
		virtual void ReleaseNameArr(NameArray *nameArr);
		virtual UTF8Char *GetString(UTF8Char *buff, UOSInt buffSize, NameArray *nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt() const;
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef);
		virtual UInt32 GetCodePage() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;
		virtual void SetDispSize(Math::Size2DDbl size, Double dpi);

		virtual GetObjectSess *BeginGetObject();
		virtual void EndGetObject(GetObjectSess *session);
		virtual Math::Geometry::Vector2D *GetNewVectorById(GetObjectSess *session, Int64 id);

		virtual ObjectClass GetObjectClass() const;

		virtual Bool CanQuery();
		virtual Bool QueryInfos(Math::Coord2DDbl coord, Data::ArrayList<Math::Geometry::Vector2D*> *vecList, Data::ArrayList<UOSInt> *valueOfstList, Data::ArrayListNN<Text::String> *nameList, Data::ArrayList<Text::String*> *valueList);

		virtual void AddUpdatedHandler(UpdatedHandler hdlr, void *obj);
		virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj);
		Map::DrawMapService *GetDrawMapService();
	};
}
#endif
