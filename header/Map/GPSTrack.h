#ifndef _SM_MAP_GPSTRACK
#define _SM_MAP_GPSTRACK
#include "Data/ArrayListInt32.h"
#include "Data/ArrayListString.h"
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

		typedef struct
		{
			Double maxLat;
			Double maxLon;
			Double minLat;
			Double minLon;
			Optional<Text::String> name;
			UOSInt nRecords;
			GPSRecord3 *records;
			const UInt8 **extraData;
			UOSInt *extraDataSize;
		} TrackRecord;

		class GPSExtraParser
		{
		public:
			virtual ~GPSExtraParser() {};
			
			virtual UOSInt GetExtraCount(const UInt8 *buff, UOSInt buffSize) = 0;
			virtual Bool GetExtraName(const UInt8 *buff, UOSInt buffSize, UOSInt extIndex, NotNullPtr<Text::StringBuilderUTF8> sb) = 0;
			virtual Bool GetExtraValueStr(const UInt8 *buff, UOSInt buffSize, UOSInt extIndex, NotNullPtr<Text::StringBuilderUTF8> sb) = 0;
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
		Data::ArrayList<GPSRecord3*> currRecs;
		Data::ArrayList<const UInt8 *> currExtraData;
		Data::ArrayList<UOSInt> currExtraSize;
		Data::ArrayList<TrackRecord*> currTracks;
		Map::GPSTrack::GPSRecord3 *tmpRecord;
		GPSExtraParser *extraParser;

		Sync::Mutex updMut;
		Data::ArrayList<Map::MapDrawLayer::UpdatedHandler> updHdlrs;
		Data::ArrayList<void *> updObjs;

	public:
		GPSTrack(NotNullPtr<Text::String> sourceName, Bool hasAltitude, UInt32 codePage, Text::String *layerName);
		GPSTrack(Text::CStringNN sourceName, Bool hasAltitude, UInt32 codePage, Text::CString layerName);
		virtual ~GPSTrack();

		virtual DrawLayerType GetLayerType() const;
		virtual UOSInt GetAllObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr);
		virtual UOSInt GetObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax() const;
		virtual void ReleaseNameArr(NameArray *nameArr);
		virtual Bool GetString(NotNullPtr<Text::StringBuilderUTF8> sb, NameArray *nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt() const;
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef);
		virtual UInt32 GetCodePage() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> rect) const;

		virtual GetObjectSess *BeginGetObject();
		virtual void EndGetObject(GetObjectSess *session);
		virtual Math::Geometry::Vector2D *GetNewVectorById(GetObjectSess *session, Int64 id);
		virtual void AddUpdatedHandler(UpdatedHandler hdlr, void *obj);
		virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj);

		virtual UOSInt QueryTableNames(Text::CString schemaName, NotNullPtr<Data::ArrayListStringNN> names); // no need to release
		virtual Optional<DB::DBReader> QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual DB::TableDef *GetTableDef(Text::CString schemaName, Text::CString tableName);
		virtual ObjectClass GetObjectClass() const;

		void NewTrack();
		UOSInt AddRecord(NotNullPtr<GPSRecord3> rec);
		Bool RemoveRecordRange(UOSInt index, UOSInt recStart, UOSInt recEnd);
		Bool GetHasAltitude();
		void SetTrackName(Text::CString name);
		void GetTrackNames(Data::ArrayListString *nameArr);
		Optional<Text::String> GetTrackName(UOSInt index);
		Bool GetTrackStartTime(UOSInt index, Data::DateTime *dt);
		Data::Timestamp GetTrackStartTime(UOSInt index);
		Bool GetTrackEndTime(UOSInt index, Data::DateTime *dt);
		Data::Timestamp GetTrackEndTime(UOSInt index);

		UOSInt GetTrackCnt();
		GPSRecord3 *GetTrack(UOSInt index, OutParam<UOSInt> recordCnt);
		Math::Coord2DDbl GetPosByTime(const Data::Timestamp &ts);
		Math::Coord2DDbl GetPosByTime(Data::DateTime *dt);
		Math::Coord2DDbl GetPosByTicks(Int64 tiemTicks);

		void SetExtraParser(GPSExtraParser *parser);
		void SetExtraDataIndex(UOSInt recIndex, const UInt8 *data, UOSInt dataSize);
		const UInt8 *GetExtraData(UOSInt trackIndex, UOSInt recIndex, UOSInt *dataSize);
		UOSInt GetExtraCount(UOSInt trackIndex, UOSInt recIndex);
		Bool GetExtraName(UOSInt trackIndex, UOSInt recIndex, UOSInt extIndex, NotNullPtr<Text::StringBuilderUTF8> sb);
		Bool GetExtraValueStr(UOSInt trackIndex, UOSInt recIndex, UOSInt extIndex, NotNullPtr<Text::StringBuilderUTF8> sb);
	};

	class GPSTrackReader : public Map::MapLayerReader
	{
	private:
		NotNullPtr<Map::GPSTrack> gps;
	public:
		GPSTrackReader(NotNullPtr<Map::GPSTrack> gps);
		virtual ~GPSTrackReader();
	};

	class GPSDataReader : public DB::DBReader
	{
	private:
		NotNullPtr<Map::GPSTrack> gps;
		OSInt currRow;
		GPSTrack::GPSRecord3 *currRec;
	public:
		GPSDataReader(NotNullPtr<Map::GPSTrack> gps);
		virtual ~GPSDataReader();

		virtual Bool ReadNext();
		virtual UOSInt ColCount();
		virtual OSInt GetRowChanged();

		virtual Int32 GetInt32(UOSInt colIndex);
		virtual Int64 GetInt64(UOSInt colIndex);
		virtual WChar *GetStr(UOSInt colIndex, WChar *buff);
		virtual Bool GetStr(UOSInt colIndex, NotNullPtr<Text::StringBuilderUTF8> sb);
		virtual Optional<Text::String> GetNewStr(UOSInt colIndex);
		virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize);
		virtual Data::Timestamp GetTimestamp(UOSInt colIndex);
		virtual Double GetDbl(UOSInt colIndex);
		virtual Bool GetBool(UOSInt colIndex);
		virtual UOSInt GetBinarySize(UOSInt colIndex);
		virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
		virtual Optional<Math::Geometry::Vector2D> GetVector(UOSInt colIndex);
		virtual Bool GetUUID(UOSInt colIndex, NotNullPtr<Data::UUID> uuid);

		virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
		virtual Bool IsNull(UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef);

		static Text::CString GetName(UOSInt colIndex, Bool hasAltitude);
		static Bool GetColDefV(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef, Bool hasAltitude);
	};
}
#endif
