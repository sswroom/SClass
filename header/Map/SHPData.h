#ifndef _SM_MAP_SHPDATA
#define _SM_MAP_SHPDATA
#include "Data/ArrayList.h"
#include "Data/ArrayListDbl.h"
#include "DB/DBFFile.h"
#include "IO/StmData/FileData.h"
#include "Map/IMapDrawLayer.h"

namespace Map
{
	class SHPData : public IMapDrawLayer
	{
	private:
		typedef struct
		{
			Double x1;
			Double y1;
			Double x2;
			Double y2;
			UInt32 nPoint;
			UInt32 nPtOfst;
			UInt32 ofst;
		} RecHdr;

		DB::DBFFile *dbf;
		IO::IStreamData *shpData;

		Map::DrawLayerType layerType;
		Data::ArrayListDbl *ptX;
		Data::ArrayListDbl *ptY;
		Data::ArrayListDbl *ptZ;
		Data::ArrayList<RecHdr*> *recs;

		Bool isPoint;
		Double xMax;
		Double xMin;
		Double yMax;
		Double yMin;
		Double mapRate;

	public:
		SHPData(UInt8 *hdr, IO::IStreamData *data, UInt32 codePage);
		virtual ~SHPData();

		Bool IsError();
		static void LatLon2XY(Double lat, Double lon, Int32 *x, Int32 *y);

		virtual DrawLayerType GetLayerType();
		virtual UOSInt GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr);
		virtual UOSInt GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Double x1, Double y1, Double x2, Double y2, Bool keepEmpty);
		virtual Int64 GetObjectIdMax();
		virtual void ReleaseNameArr(void *nameArr);
		virtual UTF8Char *GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt();
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, DB::ColDef *colDef);
		virtual UInt32 GetCodePage();
		virtual Bool GetBoundsDbl(Double *minX, Double *minY, Double *maxX, Double *maxY);

		virtual void *BeginGetObject();
		virtual void EndGetObject(void *session);
		virtual DrawObjectL *GetNewObjectById(void *session, Int64 id);
		virtual Math::Vector2D *GetNewVectorById(void *session, Int64 id);
		virtual void ReleaseObject(void *session, DrawObjectL *obj);

		virtual UOSInt GetTableNames(Data::ArrayList<Text::CString> *names); // no need to release
		virtual DB::DBReader *QueryTableData(Text::CString tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual void CloseReader(DB::DBReader *r);
		virtual void GetErrorMsg(Text::StringBuilderUTF8 *str);
		virtual void Reconnect();

		virtual ObjectClass GetObjectClass();
	};
}
#endif
