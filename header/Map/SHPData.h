#ifndef _SM_MAP_SHPDATA
#define _SM_MAP_SHPDATA
#include "Data/ArrayList.h"
#include "Data/ArrayListDbl.h"
#include "DB/DBFFile.h"
#include "IO/StmData/FileData.h"
#include "Map/MapDrawLayer.h"
#include "Math/ArcGISPRJParser.h"

namespace Map
{
	class SHPData : public MapDrawLayer
	{
	private:
		typedef struct
		{
			Double x1;
			Double y1;
			Double x2;
			Double y2;
			Math::Geometry::Vector2D *vec;
			UInt32 nPoint;
			UInt32 nPtOfst;
			UInt32 ofst;
		} RecHdr;

		DB::DBFFile *dbf;
		IO::StreamData *shpData;

		Map::DrawLayerType layerType;
		Data::ArrayListDbl *ptX;
		Data::ArrayListDbl *ptY;
		Data::ArrayListDbl *ptZ;
		Sync::Mutex *recsMut;
		Data::ArrayList<Optional<RecHdr>> *recs;

		Bool isPoint;
		Math::Coord2DDbl max;
		Math::Coord2DDbl min;
		Double mapRate;

	public:
		SHPData(const UInt8 *hdr, NN<IO::StreamData> data, UInt32 codePage, NN<Math::ArcGISPRJParser> prjParser);
		virtual ~SHPData();

		virtual Bool IsError() const;
		static void LatLon2XY(Double lat, Double lon, OutParam<Int32> x, OutParam<Int32> y);

		virtual DrawLayerType GetLayerType() const;
		virtual UOSInt GetAllObjectIds(NN<Data::ArrayListInt64> outArr, NameArray **nameArr);
		virtual UOSInt GetObjectIds(NN<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax() const;
		virtual void ReleaseNameArr(NameArray *nameArr);
		virtual Bool GetString(NN<Text::StringBuilderUTF8> sb, NameArray *nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt() const;
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef);
		virtual UInt32 GetCodePage() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> rect) const;

		virtual GetObjectSess *BeginGetObject();
		virtual void EndGetObject(GetObjectSess *session);
		virtual Math::Geometry::Vector2D *GetNewVectorById(GetObjectSess *session, Int64 id);

		virtual UOSInt QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names); // no need to release
		virtual Optional<DB::DBReader> QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual Optional<DB::TableDef> GetTableDef(Text::CString schemaName, Text::CString tableName);
		virtual void CloseReader(NN<DB::DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual void Reconnect();

		virtual ObjectClass GetObjectClass() const;
	};
}
#endif
