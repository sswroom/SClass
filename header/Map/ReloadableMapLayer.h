#ifndef _SM_MAP_RELOADABLEMAPLAYER
#define _SM_MAP_RELOADABLEMAPLAYER
#include "Sync/RWMutex.h"
#include "Map/IMapDrawLayer.h"
#include "Net/WebBrowser.h"
#include "Parser/ParserList.h"

namespace Map
{
	class ReloadableMapLayer : public Map::IMapDrawLayer
	{
	private:
		typedef struct
		{
			const UTF8Char *layerName;
			const UTF8Char *url;
			Map::IMapDrawLayer *innerLayer;
			Map::DrawLayerType innerLayerType;
			Int32 reloadInterval;
			void *sess;
		} InnerLayerInfo;
	private:
		Net::WebBrowser *browser;
		Parser::ParserList *parsers;
		Sync::RWMutex *innerLayerMut;
		Data::ArrayList<InnerLayerInfo*> *innerLayers;
		Map::DrawLayerType innerLayerType;

		Double currScale;
		Int64 currTime;
		Data::ArrayList<UpdatedHandler> *updHdlrs;
		Data::ArrayList<void *> *updObjs;

		static void __stdcall InnerUpdated(void *userObj);
	public:
		ReloadableMapLayer(const UTF8Char *fileName, Parser::ParserList *parsers, Net::WebBrowser *browser, const UTF8Char *layerName);
		virtual ~ReloadableMapLayer();

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
		virtual UOSInt GetNameCol();
		virtual void SetNameCol(UOSInt nameCol);

		virtual ObjectClass GetObjectClass();
		virtual Math::CoordinateSystem *GetCoordinateSystem();
		virtual void SetCoordinateSystem(Math::CoordinateSystem *csys);

		virtual void AddUpdatedHandler(UpdatedHandler hdlr, void *obj);
		virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj);

		void AddInnerLayer(const UTF8Char *name, const UTF8Char *url, Int32 seconds);
//		void SetLayerName(const WChar *name);
//		void SetReloadURL(const WChar *url);
//		void SetReloadInterval(Int32 seconds);
		void Reload();
	};
}
#endif
