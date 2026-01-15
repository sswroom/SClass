#ifndef _SM_MAP_GPSTRACK
#define _SM_MAP_GPSTRACK
#include "Data/ArrayListInt32.h"
#include "Data/ArrayListString.h"
#include "Data/CallbackStorage.h"
#include "Data/TimeInstant.h"
#include "IO/ParsedObject.h"
#include "Map/MapDrawLayer.h"
#include "Sync/Mutex.h"
#include "Text/CString.h"

namespace Map
{
	class GPSTrack : public MapDrawLayer
	{
	public:
		ASTRUCT GPSRecord3
		{
			Math::Coord2DDbl pos;
			Data::TimeInstant recTime;
			Double altitude;
			Double speed;
			Double heading;
			Bool valid;
			UInt8 nSateUsed;
			UInt8 nSateUsedGPS;
			UInt8 nSateUsedSBAS;
			UInt8 nSateUsedGLO; //GLONASS
			UInt8 nSateViewGPS; //GPS
			UInt8 nSateViewGLO; //GLONASS
			UInt8 nSateViewGA; //Galileo
			UInt8 nSateViewQZSS; //QZSS
			UInt8 nSateViewBD; //BeiDou
		};

		ASTRUCT GPSRecordFull : public GPSRecord3
		{
			UnsafeArrayOpt<const UInt8> extraData;
			UIntOS extraDataSize;
		};

		typedef struct
		{
			Double maxLat;
			Double maxLon;
			Double minLat;
			Double minLon;
			Optional<Text::String> name;
			UIntOS nRecords;
			UnsafeArray<GPSRecordFull> records;
			Bool trackUnsorted;
		} TrackRecord;

		class GPSExtraParser
		{
		public:
			virtual ~GPSExtraParser() {};
			
			virtual UIntOS GetExtraCount(UnsafeArray<const UInt8> buff, UIntOS buffSize) = 0;
			virtual Bool GetExtraName(UnsafeArray<const UInt8> buff, UIntOS buffSize, UIntOS extIndex, NN<Text::StringBuilderUTF8> sb) = 0;
			virtual Bool GetExtraValueStr(UnsafeArray<const UInt8> buff, UIntOS buffSize, UIntOS extIndex, NN<Text::StringBuilderUTF8> sb) = 0;
		};
	private:
		UInt32 codePage;
		Bool hasAltitude;
		Double currMaxLat;
		Double currMaxLon;
		Double currMinLat;
		Double currMinLon;
		Double maxLat;
		Double maxLon;
		Double minLat;
		Double minLon;
		Optional<Text::String> currTrackName;
		Sync::Mutex recMut;
		Data::ArrayListInt64 currTimes;
		Data::ArrayListNN<GPSRecordFull> currRecs;
		Data::ArrayListNN<TrackRecord> currTracks;
		Bool currUnsorted;
		UnsafeArrayOpt<Map::GPSTrack::GPSRecordFull> tmpRecord;
		Optional<GPSExtraParser> extraParser;

		Sync::Mutex updMut;
		Data::ArrayListObj<Data::CallbackStorage<Map::MapDrawLayer::UpdatedHandler>> updHdlrs;

	public:
		GPSTrack(NN<Text::String> sourceName, Bool hasAltitude, UInt32 codePage, Optional<Text::String> layerName);
		GPSTrack(Text::CStringNN sourceName, Bool hasAltitude, UInt32 codePage, Text::CString layerName);
		virtual ~GPSTrack();

		virtual DrawLayerType GetLayerType() const;
		virtual UIntOS GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr);
		virtual UIntOS GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UIntOS GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax() const;
		virtual UIntOS GetRecordCnt() const;
		virtual void ReleaseNameArr(Optional<NameArray> nameArr);
		virtual Bool GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UIntOS strIndex);
		virtual UIntOS GetColumnCnt() const;
		virtual UnsafeArrayOpt<UTF8Char> GetColumnName(UnsafeArray<UTF8Char> buff, UIntOS colIndex) const;
		virtual DB::DBUtil::ColType GetColumnType(UIntOS colIndex, OptOut<UIntOS> colSize) const;
		virtual Bool GetColumnDef(UIntOS colIndex, NN<DB::ColDef> colDef) const;
		virtual UInt32 GetCodePage() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> rect) const;

		virtual NN<GetObjectSess> BeginGetObject();
		virtual void EndGetObject(NN<GetObjectSess> session);
		virtual Optional<Math::Geometry::Vector2D> GetNewVectorById(NN<GetObjectSess> session, Int64 id);
		virtual void AddUpdatedHandler(UpdatedHandler hdlr, AnyType obj);
		virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, AnyType obj);

		virtual UIntOS QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names); // no need to release
		virtual Optional<DB::DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS ofst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
		virtual Optional<DB::TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);
		virtual UIntOS GetGeomCol() const;
		virtual ObjectClass GetObjectClass() const;
		
		void NewTrack();
		UIntOS AddRecord(NN<GPSRecord3> rec);
		Bool RemoveRecordRange(UIntOS index, UIntOS recStart, UIntOS recEnd);
		Bool GetHasAltitude();
		void SetTrackName(Text::CString name);
		void GetTrackNames(NN<Data::ArrayListString> nameArr);
		Optional<Text::String> GetTrackName(UIntOS index);
		Bool GetTrackStartTime(UIntOS index, NN<Data::DateTime> dt);
		Data::Timestamp GetTrackStartTime(UIntOS index);
		Bool GetTrackEndTime(UIntOS index, NN<Data::DateTime> dt);
		Data::Timestamp GetTrackEndTime(UIntOS index);

		UIntOS GetTrackCnt();
		UnsafeArrayOpt<GPSRecordFull> GetTrack(UIntOS index, OutParam<UIntOS> recordCnt);
		Math::Coord2DDbl GetPosByTime(const Data::Timestamp &ts);
		Math::Coord2DDbl GetPosByTime(NN<Data::DateTime> dt);
		Math::Coord2DDbl GetPosByTicks(Int64 tiemTicks);

		void SetExtraParser(Optional<GPSExtraParser> parser);
		void SetExtraDataIndex(UIntOS recIndex, UnsafeArray<const UInt8> data, UIntOS dataSize);
		UnsafeArrayOpt<const UInt8> GetExtraData(UIntOS trackIndex, UIntOS recIndex, OutParam<UIntOS> dataSize);
		UIntOS GetExtraCount(UIntOS trackIndex, UIntOS recIndex);
		Bool GetExtraName(UIntOS trackIndex, UIntOS recIndex, UIntOS extIndex, NN<Text::StringBuilderUTF8> sb);
		Bool GetExtraValueStr(UIntOS trackIndex, UIntOS recIndex, UIntOS extIndex, NN<Text::StringBuilderUTF8> sb);
		void SortRecords();
	};

	class GPSTrackReader : public Map::MapLayerReader
	{
	private:
		NN<Map::GPSTrack> gps;
	public:
		GPSTrackReader(NN<Map::GPSTrack> gps);
		virtual ~GPSTrackReader();
	};

	class GPSDataReader : public DB::DBReader
	{
	private:
		NN<Map::GPSTrack> gps;
		IntOS currRow;
		Optional<GPSTrack::GPSRecordFull> currRec;
	public:
		GPSDataReader(NN<Map::GPSTrack> gps);
		virtual ~GPSDataReader();

		virtual Bool ReadNext();
		virtual UIntOS ColCount();
		virtual IntOS GetRowChanged();

		virtual Int32 GetInt32(UIntOS colIndex);
		virtual Int64 GetInt64(UIntOS colIndex);
		virtual UnsafeArrayOpt<WChar> GetStr(UIntOS colIndex, UnsafeArray<WChar> buff);
		virtual Bool GetStr(UIntOS colIndex, NN<Text::StringBuilderUTF8> sb);
		virtual Optional<Text::String> GetNewStr(UIntOS colIndex);
		virtual UnsafeArrayOpt<UTF8Char> GetStr(UIntOS colIndex, UnsafeArray<UTF8Char> buff, UIntOS buffSize);
		virtual Data::Timestamp GetTimestamp(UIntOS colIndex);
		virtual Double GetDblOrNAN(UIntOS colIndex);
		virtual Bool GetBool(UIntOS colIndex);
		virtual UIntOS GetBinarySize(UIntOS colIndex);
		virtual UIntOS GetBinary(UIntOS colIndex, UnsafeArray<UInt8> buff);
		virtual Optional<Math::Geometry::Vector2D> GetVector(UIntOS colIndex);
		virtual Bool GetUUID(UIntOS colIndex, NN<Data::UUID> uuid);

		virtual UnsafeArrayOpt<UTF8Char> GetName(UIntOS colIndex, UnsafeArray<UTF8Char> buff);
		virtual Bool IsNull(UIntOS colIndex);
		virtual DB::DBUtil::ColType GetColType(UIntOS colIndex, OptOut<UIntOS> colSize);
		virtual Bool GetColDef(UIntOS colIndex, NN<DB::ColDef> colDef);

		static Text::CString GetName(UIntOS colIndex, Bool hasAltitude);
		static Bool GetColDefV(UIntOS colIndex, NN<DB::ColDef> colDef, Bool hasAltitude);
	};
}
#endif
