#ifndef _SM_MAP_NETWORKLINKLAYER
#define _SM_MAP_NETWORKLINKLAYER
#include "Map/IMapDrawLayer.h"
#include "Net/WebBrowser.h"
#include "Parser/ParserList.h"
#include "Sync/RWMutex.h"
#include "Text/String.h"

namespace Map
{
	class NetworkLinkLayer : public Map::IMapDrawLayer
	{
	public:
		enum class RefreshMode
		{
			OnInterval,
			OnStop,
			OnRequest
		};
	private:
		typedef struct
		{
			Text::String *layerName;
			Text::String *url;
			Text::String *viewFormat;
			RefreshMode mode;
			Int32 reloadInterval;
			Map::IMapDrawLayer *innerLayer;
			Map::DrawLayerType innerLayerType;
			Data::Timestamp lastUpdated;
			void *sess;
		} LinkInfo;
	private:
		Net::WebBrowser *browser;
		Parser::ParserList *parsers;
		Sync::RWMutex linkMut;
		Data::ArrayList<LinkInfo*> links;
		Map::DrawLayerType innerLayerType;

		Double currScale;
		Int64 currTime;
		Data::ArrayList<UpdatedHandler> updHdlrs;
		Data::ArrayList<void *> updObjs;

		Sync::Mutex dispMut;
		Math::Size2D<Double> dispSize;
		Double dispDPI;
		Math::RectAreaDbl dispRect;

		static void __stdcall InnerUpdated(void *userObj);
		void LoadLink(LinkInfo *link);
	public:
		NetworkLinkLayer(Text::CString fileName, Parser::ParserList *parsers, Net::WebBrowser *browser, Text::CString layerName);
		virtual ~NetworkLinkLayer();

		virtual void SetCurrScale(Double scale);
		virtual void SetCurrTimeTS(Int64 timeStamp);
		virtual Int64 GetTimeStartTS();
		virtual Int64 GetTimeEndTS();

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
		virtual UOSInt GetNameCol();
		virtual void SetNameCol(UOSInt nameCol);

		virtual ObjectClass GetObjectClass();
		virtual Math::CoordinateSystem *GetCoordinateSystem();
		virtual void SetCoordinateSystem(Math::CoordinateSystem *csys);

		virtual void AddUpdatedHandler(UpdatedHandler hdlr, void *obj);
		virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj);

		void AddLink(Text::CString name, Text::CString url, Text::CString viewFormat, RefreshMode mode, Int32 seconds);
		void Reload();
	};
}
#endif
