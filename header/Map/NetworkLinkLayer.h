#ifndef _SM_MAP_NETWORKLINKLAYER
#define _SM_MAP_NETWORKLINKLAYER
#include "Data/CallbackStorage.h"
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
			Optional<Text::String> layerName;
			NN<Text::String> url;
			Optional<Text::String> viewFormat;
			RefreshMode mode;
			Int32 reloadInterval;
			Optional<Map::MapDrawLayer> innerLayer;
			Map::DrawLayerType innerLayerType;
			Data::Timestamp lastUpdated;
			NN<GetObjectSess> sess;
		};
	private:
		NN<Net::WebBrowser> browser;
		NN<Parser::ParserList> parsers;
		Sync::RWMutex linkMut;
		Data::ArrayListNN<LinkInfo> links;
		Map::DrawLayerType innerLayerType;

		Double currScale;
		Int64 currTime;
		Data::ArrayList<Data::CallbackStorage<UpdatedHandler>> updHdlrs;

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

		static void __stdcall InnerUpdated(AnyType userObj);
		static UInt32 __stdcall ControlThread(AnyType userObj);
		void CheckLinks(Bool manualRequest);
		void LoadLink(NN<LinkInfo> link);
	public:
		NetworkLinkLayer(Text::CStringNN fileName, NN<Parser::ParserList> parsers, NN<Net::WebBrowser> browser, Text::CString layerName);
		virtual ~NetworkLinkLayer();

		virtual void SetCurrScale(Double scale);
		virtual void SetCurrTimeTS(Int64 timeStamp);
		virtual Int64 GetTimeStartTS() const;
		virtual Int64 GetTimeEndTS() const;

		virtual DrawLayerType GetLayerType() const;
		virtual UOSInt GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr);
		virtual UOSInt GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax() const;
		virtual void ReleaseNameArr(Optional<NameArray> nameArr);
		virtual Bool GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt() const;
		virtual UnsafeArrayOpt<UTF8Char> GetColumnName(UnsafeArray<UTF8Char> buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef);
		virtual UInt32 GetCodePage() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;
		virtual void SetDispSize(Math::Size2DDbl size, Double dpi);

		virtual NN<GetObjectSess> BeginGetObject();
		virtual void EndGetObject(NN<GetObjectSess> session);
		virtual Optional<Math::Geometry::Vector2D> GetNewVectorById(NN<GetObjectSess> session, Int64 id);
		virtual UOSInt GetNameCol() const;
		virtual void SetNameCol(UOSInt nameCol);
		virtual UOSInt GetGeomCol() const;

		virtual ObjectClass GetObjectClass() const;
		virtual NN<Math::CoordinateSystem> GetCoordinateSystem();
		virtual void SetCoordinateSystem(NN<Math::CoordinateSystem> csys);

		virtual void AddUpdatedHandler(UpdatedHandler hdlr, AnyType obj);
		virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, AnyType obj);

		UOSInt AddLink(Text::CString name, Text::CStringNN url, Text::CString viewFormat, RefreshMode mode, Int32 seconds);
		void SetBounds(Math::RectAreaDbl bounds);
		void Reload();
	};
}
#endif
