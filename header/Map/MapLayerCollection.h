#ifndef _SM_MAP_MAPLAYERCOLLECTION
#define _SM_MAP_MAPLAYERCOLLECTION
#include "Data/ArrayList.h"
#include "Map/MapDrawLayer.h"
#include "Sync/RWMutex.h"

namespace Map
{
	class MapLayerCollection : public Map::MapDrawLayer
	{
	private:
		Sync::RWMutex mut;
		Data::ArrayList<Map::MapDrawLayer *> layerList;
		Data::ArrayList<Map::MapDrawLayer::UpdatedHandler> updHdlrs;
		Data::ArrayList<void *> updObjs;

		static void __stdcall InnerUpdated(void *userObj);
	public:
		MapLayerCollection(Text::String *sourceName, Text::String *layerName);
		MapLayerCollection(Text::CString sourceName, Text::CString layerName);
		virtual ~MapLayerCollection();

		virtual UOSInt Add(Map::MapDrawLayer * val);
		virtual Map::MapDrawLayer *RemoveAt(UOSInt index);
		virtual void Clear();
		virtual UOSInt GetCount() const;
		virtual Map::MapDrawLayer *GetItem(UOSInt Index);
		virtual void SetItem(UOSInt Index, Map::MapDrawLayer *Val);

		virtual void SetCurrScale(Double scale);
		virtual void SetCurrTimeTS(Int64 timeStamp);
		virtual Int64 GetTimeStartTS();
		virtual Int64 GetTimeEndTS();

		virtual DrawLayerType GetLayerType();
		virtual UOSInt GetAllObjectIds(Data::ArrayListInt64 *outArr, NameArray **nameArr);
		virtual UOSInt GetObjectIds(Data::ArrayListInt64 *outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax();
		virtual void ReleaseNameArr(NameArray *nameArr);
		virtual UTF8Char *GetString(UTF8Char *buff, UOSInt buffSize, NameArray *nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt();
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, DB::ColDef *colDef);
		virtual UInt32 GetCodePage();
		virtual Bool GetBounds(Math::RectAreaDbl *rect);

		virtual GetObjectSess *BeginGetObject();
		virtual void EndGetObject(GetObjectSess *session);
		virtual Math::Geometry::Vector2D *GetNewVectorById(GetObjectSess *session, Int64 id);
		virtual void AddUpdatedHandler(UpdatedHandler hdlr, void *obj);
		virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj);

		virtual ObjectClass GetObjectClass();
		virtual Math::CoordinateSystem *GetCoordinateSystem();
		virtual void SetCoordinateSystem(Math::CoordinateSystem *csys);

		void ReleaseAll();
		UOSInt GetUpdatedHandlerCnt() const;
		UpdatedHandler GetUpdatedHandler(UOSInt index) const;
		void *GetUpdatedObject(UOSInt index) const;
	};
}
#endif
