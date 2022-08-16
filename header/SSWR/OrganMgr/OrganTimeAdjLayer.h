#ifndef _SM_SSWR_ORGANMGR_ORGANTIMEADJLAYER
#define _SM_SSWR_ORGANMGR_ORGANTIMEADJLAYER

#include "Data/StringMap.h"
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
			Data::StringMap<Int32> *cameraMap;
		public:
			OrganTimeAdjLayer(Map::GPSTrack *gpsTrk, Data::ArrayList<UserFileInfo *> *userFileList);
			virtual ~OrganTimeAdjLayer();

			virtual Map::DrawLayerType GetLayerType();
			virtual UOSInt GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr);
			virtual UOSInt GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
			virtual UOSInt GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
			virtual Int64 GetObjectIdMax();
			virtual void ReleaseNameArr(void *nameArr);
			virtual UTF8Char *GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex);
			virtual UOSInt GetColumnCnt();
			virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
			virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, UOSInt *colSize);
			virtual Bool GetColumnDef(UOSInt colIndex, DB::ColDef *colDef);
			virtual UInt32 GetCodePage();
			virtual Bool GetBounds(Math::RectAreaDbl *bounds);

			virtual void *BeginGetObject();
			virtual void EndGetObject(void *session);
			virtual Math::Geometry::Vector2D *GetNewVectorById(void *session, Int64 id);

			virtual ObjectClass GetObjectClass();

			void SetTimeAdj(Text::String *camera, Int32 timeAdj);
			void SetTimeAdj(Text::CString camera, Int32 timeAdj);
		};
	}
}
#endif