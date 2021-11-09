#ifndef _SM_MAP_ESRI_ESRIMDBLAYER
#define _SM_MAP_ESRI_ESRIMDBLAYER
#include "Data/ArrayListStrUTF8.h"
#include "Data/Int32Map.h"
#include "DB/DBReader.h"
#include "DB/SharedDBConn.h"
#include "Map/IMapDrawLayer.h"

namespace Map
{
	namespace ESRI
	{
		class ESRIMDBLayer : public Map::IMapDrawLayer
		{
		private:
			DB::SharedDBConn *conn;
			Data::Int32Map<Math::Vector2D*> *objects;
			DB::DBConn *currDB;
			DB::DBConn *lastDB;
			Map::DrawLayerType layerType;
			Data::ArrayListStrUTF8 *colNames;
			Double minX;
			Double minY;
			Double maxX;
			Double maxY;
			UOSInt objIdCol;
			UOSInt shapeCol;
			const UTF8Char *tableName;

		private:
			Data::Int32Map<const UTF8Char **> *ReadNameArr();
		public:
			ESRIMDBLayer(DB::SharedDBConn *conn, UInt32 srid, const UTF8Char *sourceName, const UTF8Char *tableName);
			virtual ~ESRIMDBLayer();

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
			virtual DrawObjectL *GetObjectByIdD(void *session, Int64 id);
			virtual Math::Vector2D *GetVectorById(void *session, Int64 id);
			virtual void ReleaseObject(void *session, DrawObjectL *obj);
			virtual void AddUpdatedHandler(UpdatedHandler hdlr, void *obj);
			virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj);

			virtual UOSInt GetTableNames(Data::ArrayList<const UTF8Char*> *names); // no need to release
			virtual DB::DBReader *GetTableData(const UTF8Char *name);
			virtual void CloseReader(DB::DBReader *r);
			virtual void GetErrorMsg(Text::StringBuilderUTF *str);
			virtual void Reconnect();

			virtual ObjectClass GetObjectClass();
		};

		class ESRIMDBReader : public DB::DBReader
		{
		private:
			DB::DBConn *conn;
			DB::DBReader *r;
			Sync::MutexUsage *mutUsage;
		public:
			ESRIMDBReader(DB::DBConn *conn, DB::DBReader *r, Sync::MutexUsage *mutUsage);
			virtual ~ESRIMDBReader();

			virtual Bool ReadNext();
			virtual UOSInt ColCount();
			virtual OSInt GetRowChanged();

			virtual Int32 GetInt32(UOSInt colIndex);
			virtual Int64 GetInt64(UOSInt colIndex);
			virtual WChar *GetStr(UOSInt colIndex, WChar *buff);
			virtual Bool GetStr(UOSInt colIndex, Text::StringBuilderUTF *sb);
			virtual const UTF8Char *GetNewStr(UOSInt colIndex);
			virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize);
			virtual DateErrType GetDate(UOSInt colIndex, Data::DateTime *outVal); //return 0 = ok, 2 = Error
			virtual Double GetDbl(UOSInt colIndex);
			virtual Bool GetBool(UOSInt colIndex);
			virtual UOSInt GetBinarySize(UOSInt colIndex);
			virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
			virtual Math::Vector2D *GetVector(UOSInt colIndex);
			virtual Bool GetUUID(UOSInt colIndex, Data::UUID *uuid);

			virtual Bool IsNull(UOSInt colIndex);
			virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
			virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize);
			virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef);
			virtual void DelNewStr(const UTF8Char *s);
		};
	}
}
#endif
