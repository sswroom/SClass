#ifndef _SM_MAP_MAPLAYERCOLLECTION
#define _SM_MAP_MAPLAYERCOLLECTION
#include "Data/ArrayList.h"
#include "Map/IMapDrawLayer.h"
#include "Sync/RWMutex.h"

namespace Map
{
	class MapLayerCollection : public Map::IMapDrawLayer
	{
	private:
		Sync::RWMutex *mut;
		Data::ArrayList<Map::IMapDrawLayer *> *layerList;
		Data::ArrayList<Map::IMapDrawLayer::UpdatedHandler> *updHdlrs;
		Data::ArrayList<void *> *updObjs;

		static void __stdcall InnerUpdated(void *userObj);
	public:
		MapLayerCollection(const UTF8Char *sourceName, const UTF8Char *layerName);
		virtual ~MapLayerCollection();

		virtual UOSInt Add(Map::IMapDrawLayer * val);
		virtual Map::IMapDrawLayer *RemoveAt(UOSInt index);
		virtual void Clear();
		virtual UOSInt GetCount();
		virtual Map::IMapDrawLayer *GetItem(UOSInt Index);
		virtual void SetItem(UOSInt Index, Map::IMapDrawLayer *Val);

		virtual void SetCurrScale(Double scale);
		virtual void SetCurrTimeTS(Int64 timeStamp);
		virtual Int64 GetTimeStartTS();
		virtual Int64 GetTimeEndTS();

		virtual DrawLayerType GetLayerType();
		virtual UOSInt GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr);
		virtual UOSInt GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Double x1, Double y1, Double x2, Double y2, Bool keepEmpty);
		virtual Int64 GetObjectIdMax();
		virtual void ReleaseNameArr(void *nameArr);
		virtual UTF8Char *GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt();
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, DB::ColDef *colDef);
		virtual UInt32 GetCodePage();
		virtual Bool GetBoundsDbl(Double *minX, Double *minY, Double *maxX, Double *maxY);

		virtual void *BeginGetObject();
		virtual void EndGetObject(void *session);
		virtual DrawObjectL *GetObjectByIdD(void *session, Int64 id);
		virtual Math::Vector2D *GetVectorById(void *session, Int64 id);
		virtual void ReleaseObject(void *session, DrawObjectL *obj);
		virtual void AddUpdatedHandler(UpdatedHandler hdlr, void *obj);
		virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj);

		virtual ObjectClass GetObjectClass();
		virtual Math::CoordinateSystem *GetCoordinateSystem();
		virtual void SetCoordinateSystem(Math::CoordinateSystem *csys);

		void ReleaseAll();
	};
}
#endif
