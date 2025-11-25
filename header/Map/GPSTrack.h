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
			UOSInt extraDataSize;
		};

		typedef struct
		{
			Double maxLat;
			Double maxLon;
			Double minLat;
			Double minLon;
			Optional<Text::String> name;
			UOSInt nRecords;
			UnsafeArray<GPSRecordFull> records;
			Bool trackUnsorted;
		} TrackRecord;

		class GPSExtraParser
		{
		public:
			virtual ~GPSExtraParser() {};
			
			virtual UOSInt GetExtraCount(UnsafeArray<const UInt8> buff, UOSInt buffSize) = 0;
			virtual Bool GetExtraName(UnsafeArray<const UInt8> buff, UOSInt buffSize, UOSInt extIndex, NN<Text::StringBuilderUTF8> sb) = 0;
			virtual Bool GetExtraValueStr(UnsafeArray<const UInt8> buff, UOSInt buffSize, UOSInt extIndex, NN<Text::StringBuilderUTF8> sb) = 0;
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
		Data::ArrayList<Data::CallbackStorage<Map::MapDrawLayer::UpdatedHandler>> updHdlrs;

	public:
		GPSTrack(NN<Text::String> sourceName, Bool hasAltitude, UInt32 codePage, Optional<Text::String> layerName);
		GPSTrack(Text::CStringNN sourceName, Bool hasAltitude, UInt32 codePage, Text::CString layerName);
		virtual ~GPSTrack();

		virtual DrawLayerType GetLayerType() const;
		virtual UOSInt GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr);
		virtual UOSInt GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax() const;
		virtual UOSInt GetRecordCnt() const;
		virtual void ReleaseNameArr(Optional<NameArray> nameArr);
		virtual Bool GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt() const;
		virtual UnsafeArrayOpt<UTF8Char> GetColumnName(UnsafeArray<UTF8Char> buff, UOSInt colIndex) const;
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize) const;
		virtual Bool GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef) const;
		virtual UInt32 GetCodePage() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> rect) const;

		virtual NN<GetObjectSess> BeginGetObject();
		virtual void EndGetObject(NN<GetObjectSess> session);
		virtual Optional<Math::Geometry::Vector2D> GetNewVectorById(NN<GetObjectSess> session, Int64 id);
		virtual void AddUpdatedHandler(UpdatedHandler hdlr, AnyType obj);
		virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, AnyType obj);

		virtual UOSInt QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names); // no need to release
		virtual Optional<DB::DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
		virtual Optional<DB::TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);
		virtual UOSInt GetGeomCol() const;
		virtual ObjectClass GetObjectClass() const;
		
		void NewTrack();
		UOSInt AddRecord(NN<GPSRecord3> rec);
		Bool RemoveRecordRange(UOSInt index, UOSInt recStart, UOSInt recEnd);
		Bool GetHasAltitude();
		void SetTrackName(Text::CString name);
		void GetTrackNames(NN<Data::ArrayListString> nameArr);
		Optional<Text::String> GetTrackName(UOSInt index);
		Bool GetTrackStartTime(UOSInt index, NN<Data::DateTime> dt);
		Data::Timestamp GetTrackStartTime(UOSInt index);
		Bool GetTrackEndTime(UOSInt index, NN<Data::DateTime> dt);
		Data::Timestamp GetTrackEndTime(UOSInt index);

		UOSInt GetTrackCnt();
		UnsafeArrayOpt<GPSRecordFull> GetTrack(UOSInt index, OutParam<UOSInt> recordCnt);
		Math::Coord2DDbl GetPosByTime(const Data::Timestamp &ts);
		Math::Coord2DDbl GetPosByTime(NN<Data::DateTime> dt);
		Math::Coord2DDbl GetPosByTicks(Int64 tiemTicks);

		void SetExtraParser(Optional<GPSExtraParser> parser);
		void SetExtraDataIndex(UOSInt recIndex, UnsafeArray<const UInt8> data, UOSInt dataSize);
		UnsafeArrayOpt<const UInt8> GetExtraData(UOSInt trackIndex, UOSInt recIndex, OutParam<UOSInt> dataSize);
		UOSInt GetExtraCount(UOSInt trackIndex, UOSInt recIndex);
		Bool GetExtraName(UOSInt trackIndex, UOSInt recIndex, UOSInt extIndex, NN<Text::StringBuilderUTF8> sb);
		Bool GetExtraValueStr(UOSInt trackIndex, UOSInt recIndex, UOSInt extIndex, NN<Text::StringBuilderUTF8> sb);
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
		OSInt currRow;
		Optional<GPSTrack::GPSRecordFull> currRec;
	public:
		GPSDataReader(NN<Map::GPSTrack> gps);
		virtual ~GPSDataReader();

		virtual Bool ReadNext();
		virtual UOSInt ColCount();
		virtual OSInt GetRowChanged();

		virtual Int32 GetInt32(UOSInt colIndex);
		virtual Int64 GetInt64(UOSInt colIndex);
		virtual UnsafeArrayOpt<WChar> GetStr(UOSInt colIndex, UnsafeArray<WChar> buff);
		virtual Bool GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb);
		virtual Optional<Text::String> GetNewStr(UOSInt colIndex);
		virtual UnsafeArrayOpt<UTF8Char> GetStr(UOSInt colIndex, UnsafeArray<UTF8Char> buff, UOSInt buffSize);
		virtual Data::Timestamp GetTimestamp(UOSInt colIndex);
		virtual Double GetDblOrNAN(UOSInt colIndex);
		virtual Bool GetBool(UOSInt colIndex);
		virtual UOSInt GetBinarySize(UOSInt colIndex);
		virtual UOSInt GetBinary(UOSInt colIndex, UnsafeArray<UInt8> buff);
		virtual Optional<Math::Geometry::Vector2D> GetVector(UOSInt colIndex);
		virtual Bool GetUUID(UOSInt colIndex, NN<Data::UUID> uuid);

		virtual UnsafeArrayOpt<UTF8Char> GetName(UOSInt colIndex, UnsafeArray<UTF8Char> buff);
		virtual Bool IsNull(UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef);

		static Text::CString GetName(UOSInt colIndex, Bool hasAltitude);
		static Bool GetColDefV(UOSInt colIndex, NN<DB::ColDef> colDef, Bool hasAltitude);
	};
}
#endif
