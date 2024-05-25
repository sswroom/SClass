#ifndef _SM_MAP_WEBIMAGELAYER
#define _SM_MAP_WEBIMAGELAYER
#include "AnyType.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListNN.h"
#include "Data/CallbackStorage.h"
#include "Data/IComparable.h"
#include "Map/MapDrawLayer.h"
#include "Media/SharedImage.h"
#include "Net/WebBrowser.h"
#include "Parser/ParserList.h"
#include "Sync/Mutex.h"
#include "Sync/RWMutex.h"

namespace Map
{
	class WebImageLayer : public Map::MapDrawLayer
	{
	private:
		class ImageStat : public Data::IComparable
		{
		public:
			Int32 id;
			NN<Text::String> url;
			Media::SharedImage *simg;
			IO::StreamData *data;
			Text::String *name;
			Int64 timeStart;
			Int64 timeEnd;
			Int32 zIndex;
			Double x1;
			Double y1;
			Double x2;
			Double y2;
			Double sizeX;
			Double sizeY;
			Bool isScreen;
			Double alpha;
			Bool hasAltitude;
			Double altitude;

		public:
			virtual ~ImageStat();
			virtual OSInt CompareTo(Data::IComparable *obj) const;
		};
	private:
		NN<Net::WebBrowser> browser;
		NN<Parser::ParserList> parsers;
		Data::ArrayListNN<ImageStat> pendingList;
		Data::ArrayListNN<ImageStat> loadingList;
		Data::ArrayListNN<ImageStat> loadedList;
		Sync::Mutex loadingMut;
		Sync::RWMutex loadedMut;
		Sync::Event loadEvt;
		Int32 nextId;
		Bool boundsExists;
		Math::Coord2DDbl min;
		Math::Coord2DDbl max;
		Int64 minTime;
		Int64 maxTime;
		Int64 currTime;
		Sync::Mutex updMut;
		Data::ArrayList<Data::CallbackStorage<UpdatedHandler>> updHdlrs;

		Bool threadRunning;
		Bool threadToStop;

		OSInt GetImageStatIndex(Int32 id);
		Optional<ImageStat> GetImageStat(Int32 id);

		void LoadImage(NN<ImageStat> stat);
		static UInt32 __stdcall LoadThread(AnyType userObj);
	public:
		WebImageLayer(NN<Net::WebBrowser> browser, NN<Parser::ParserList> parsers, Text::CStringNN sourceName, NN<Math::CoordinateSystem> csys, Text::CString layerName);
		virtual ~WebImageLayer();

		virtual void SetCurrTimeTS(Int64 timeStamp);
		virtual Int64 GetTimeStartTS() const;
		virtual Int64 GetTimeEndTS() const;

		virtual DrawLayerType GetLayerType() const;
		virtual UOSInt GetAllObjectIds(NN<Data::ArrayListInt64> outArr, NameArray **nameArr);
		virtual UOSInt GetObjectIds(NN<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax() const;
		virtual void ReleaseNameArr(NameArray *nameArr);
		virtual Bool GetString(NN<Text::StringBuilderUTF8> sb, NameArray *nameArr, Int64 id, UOSInt colIndex);
		virtual UOSInt GetColumnCnt() const;
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef);
		virtual UInt32 GetCodePage() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;

		virtual GetObjectSess *BeginGetObject();
		virtual void EndGetObject(GetObjectSess *session);
		virtual Math::Geometry::Vector2D *GetNewVectorById(GetObjectSess *session, Int64 id);

		virtual ObjectClass GetObjectClass() const;

		virtual void AddUpdatedHandler(UpdatedHandler hdlr, AnyType obj);
		virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, AnyType obj);

		void AddImage(Text::CString name, Text::CString url, Int32 zIndex, Double x1, Double y1, Double x2, Double y2, Double sizeX, Double sizeY, Bool isScreen, Int64 timeStart, Int64 timeEnd, Double alpha, Bool hasAltitude, Double altitude);
	};
}
#endif
