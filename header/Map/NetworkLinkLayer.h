#ifndef _SM_MAP_NETWORKLINKLAYER
#define _SM_MAP_NETWORKLINKLAYER
#include "Map/MapDrawLayer.h"
#include "Net/WebBrowser.h"
#include "Parser/ParserList.h"
#include "Sync/RWMutex.h"
#include "Text/String.h"

namespace Map
{
	class NetworkLinkLayer : public Map::MapDrawLayer
	{
	public:
		enum class RefreshMode
		{
			OnInterval,
			OnStop,
			OnRequest
		};
	private:
		struct LinkInfo
		{
			Text::String *layerName;
			NotNullPtr<Text::String> url;
			Text::String *viewFormat;
			RefreshMode mode;
			Int32 reloadInterval;
			Map::MapDrawLayer *innerLayer;
			Map::DrawLayerType innerLayerType;
			Data::Timestamp lastUpdated;
			GetObjectSess *sess;
		};
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
		Math::Size2DDbl dispSize;
		Double dispDPI;
		Math::RectAreaDbl dispRect;
		Int64 dispTime;

		Bool hasBounds;
		Math::RectAreaDbl bounds;

		Sync::Event ctrlEvt;
		Bool ctrlRunning;
		Bool ctrlToStop;

		static void __stdcall InnerUpdated(void *userObj);
		static UInt32 __stdcall ControlThread(void *userObj);
		void CheckLinks(Bool manualRequest);
		void LoadLink(LinkInfo *link);
	public:
		NetworkLinkLayer(Text::CString fileName, Parser::ParserList *parsers, Net::WebBrowser *browser, Text::CString layerName);
		virtual ~NetworkLinkLayer();

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
		virtual Bool GetBounds(Math::RectAreaDbl *bounds);
		virtual void SetDispSize(Math::Size2DDbl size, Double dpi);

		virtual GetObjectSess *BeginGetObject();
		virtual void EndGetObject(GetObjectSess *session);
		virtual Math::Geometry::Vector2D *GetNewVectorById(GetObjectSess *session, Int64 id);
		virtual UOSInt GetNameCol();
		virtual void SetNameCol(UOSInt nameCol);

		virtual ObjectClass GetObjectClass();
		virtual Math::CoordinateSystem *GetCoordinateSystem();
		virtual void SetCoordinateSystem(Math::CoordinateSystem *csys);

		virtual void AddUpdatedHandler(UpdatedHandler hdlr, void *obj);
		virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj);

		UOSInt AddLink(Text::CString name, Text::CString url, Text::CString viewFormat, RefreshMode mode, Int32 seconds);
		void SetBounds(Math::RectAreaDbl bounds);
		void Reload();
	};
}
#endif
