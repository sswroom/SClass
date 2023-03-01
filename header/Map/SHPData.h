#ifndef _SM_MAP_SHPDATA
#define _SM_MAP_SHPDATA
#include "Data/ArrayList.h"
#include "Data/ArrayListDbl.h"
#include "DB/DBFFile.h"
#include "IO/StmData/FileData.h"
#include "Map/MapDrawLayer.h"

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
		Data::ArrayList<RecHdr*> *recs;

		Bool isPoint;
		Math::Coord2DDbl max;
		Math::Coord2DDbl min;
		Double mapRate;

	public:
		SHPData(const UInt8 *hdr, IO::StreamData *data, UInt32 codePage);
		virtual ~SHPData();

		Bool IsError();
		static void LatLon2XY(Double lat, Double lon, Int32 *x, Int32 *y);

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
		virtual Bool GetBounds(Math::RectAreaDbl *rect);

		virtual GetObjectSess *BeginGetObject();
		virtual void EndGetObject(GetObjectSess *session);
		virtual Math::Geometry::Vector2D *GetNewVectorById(GetObjectSess *session, Int64 id);

		virtual UOSInt QueryTableNames(Text::CString schemaName, Data::ArrayList<Text::String*> *names); // no need to release
		virtual DB::DBReader *QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual DB::TableDef *GetTableDef(Text::CString schemaName, Text::CString tableName);
		virtual void CloseReader(DB::DBReader *r);
		virtual void GetLastErrorMsg(Text::StringBuilderUTF8 *str);
		virtual void Reconnect();

		virtual ObjectClass GetObjectClass();
	};
}
#endif
