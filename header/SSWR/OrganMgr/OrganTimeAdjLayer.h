#ifndef _SM_SSWR_ORGANMGR_ORGANTIMEADJLAYER
#define _SM_SSWR_ORGANMGR_ORGANTIMEADJLAYER

#include "Data/StringUTF8Map.h"
#include "Map/GPSTrack.h"
#include "Map/IMapDrawLayer.h"
#include "SSWR/OrganMgr/OrganEnv.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganTimeAdjLayer : public Map::IMapDrawLayer
		{
		private:
			Map::GPSTrack *gpsTrk;
			Data::ArrayList<UserFileInfo *> *userFileList;
			Data::StringUTF8Map<Int32> *cameraMap;
		public:
			OrganTimeAdjLayer(Map::GPSTrack *gpsTrk, Data::ArrayList<UserFileInfo *> *userFileList);
			virtual ~OrganTimeAdjLayer();

			virtual Map::DrawLayerType GetLayerType();
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
			virtual Int32 GetCodePage();
			virtual Bool GetBoundsDbl(Double *minX, Double *minY, Double *maxX, Double *maxY);

			virtual void *BeginGetObject();
			virtual void EndGetObject(void *session);
			virtual Map::DrawObjectL *GetObjectByIdD(void *session, Int64 id);
			virtual Math::Vector2D *GetVectorById(void *session, Int64 id);
			virtual void ReleaseObject(void *session, Map::DrawObjectL *obj);

			virtual ObjectClass GetObjectClass();

			void SetTimeAdj(const UTF8Char *camera, Int32 timeAdj);
		};
	}
}
#endif