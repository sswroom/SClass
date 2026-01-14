#ifndef _SM_MAP_SHPDATA
#define _SM_MAP_SHPDATA
#include "Data/ArrayListDbl.h"
#include "Data/ArrayListObj.hpp"
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
			Optional<Math::Geometry::Vector2D> vec;
			UInt32 nPoint;
			UInt32 nPtOfst;
			UInt32 ofst;
			UInt32 endOfst;
		} RecHdr;

		DB::DBFFile *dbf;
		Optional<IO::StreamData> shpData;

		Map::DrawLayerType layerType;
		Data::ArrayListDbl *ptX;
		Data::ArrayListDbl *ptY;
		Data::ArrayListDbl *ptZ;
		Sync::Mutex *recsMut;
		Data::ArrayListObj<Optional<RecHdr>> *recs;

		Bool isPoint;
		Math::Coord2DDbl max;
		Math::Coord2DDbl min;
		Double mapRate;

	public:
		SHPData(UnsafeArray<const UInt8> hdr, NN<IO::StreamData> data, UInt32 codePage, NN<Math::ArcGISPRJParser> prjParser);
		virtual ~SHPData();

		virtual Bool IsError() const;
		static void LatLon2XY(Double lat, Double lon, OutParam<Int32> x, OutParam<Int32> y);

		virtual DrawLayerType GetLayerType() const;
		virtual UOSInt GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr);
		virtual UOSInt GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax() const;
		virtual UOSInt GetRecordCnt() const;
		virtual void ReleaseNameArr(Optional<NameArray> nameArr);
		virtual Bool GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UOSInt colIndex);
		virtual UOSInt GetColumnCnt() const;
		virtual UnsafeArrayOpt<UTF8Char> GetColumnName(UnsafeArray<UTF8Char> buff, UOSInt colIndex) const;
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize) const;
		virtual Bool GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef) const;
		virtual UInt32 GetCodePage() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> rect) const;

		virtual NN<GetObjectSess> BeginGetObject();
		virtual void EndGetObject(NN<GetObjectSess> session);
		virtual Optional<Math::Geometry::Vector2D> GetNewVectorById(NN<GetObjectSess> session, Int64 id);

		virtual UOSInt QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names); // no need to release
		//virtual Optional<DB::DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
		virtual Optional<DB::TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);
		virtual void CloseReader(NN<DB::DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual void Reconnect();
		virtual UOSInt GetGeomCol() const;

		virtual ObjectClass GetObjectClass() const;
	};
}
#endif
