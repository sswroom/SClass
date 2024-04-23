#ifndef _SM_SSWR_ORGANMGR_ORGANTIMEADJLAYER
#define _SM_SSWR_ORGANMGR_ORGANTIMEADJLAYER

#include "Data/StringMap.h"
#include "Map/GPSTrack.h"
#include "Map/MapDrawLayer.h"
#include "SSWR/OrganMgr/OrganEnv.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganTimeAdjLayer : public Map::MapDrawLayer
		{
		private:
			NotNullPtr<Map::GPSTrack> gpsTrk;
			NN<Data::ArrayListNN<UserFileInfo>> userFileList;
			Data::StringMap<Int32> cameraMap;
		public:
			OrganTimeAdjLayer(NotNullPtr<Map::GPSTrack> gpsTrk, NN<Data::ArrayListNN<UserFileInfo>> userFileList);
			virtual ~OrganTimeAdjLayer();

			virtual Map::DrawLayerType GetLayerType() const;
			virtual UOSInt GetAllObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, Map::NameArray **nameArr);
			virtual UOSInt GetObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, Map::NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
			virtual UOSInt GetObjectIdsMapXY(NotNullPtr<Data::ArrayListInt64> outArr, Map::NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
			virtual Int64 GetObjectIdMax() const;
			virtual void ReleaseNameArr(Map::NameArray *nameArr);
			virtual Bool GetString(NotNullPtr<Text::StringBuilderUTF8> sb, Map::NameArray *nameArr, Int64 id, UOSInt strIndex);
			virtual UOSInt GetColumnCnt() const;
			virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
			virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize);
			virtual Bool GetColumnDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef);
			virtual UInt32 GetCodePage() const;
			virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;

			virtual Map::GetObjectSess *BeginGetObject();
			virtual void EndGetObject(Map::GetObjectSess *session);
			virtual Math::Geometry::Vector2D *GetNewVectorById(Map::GetObjectSess *session, Int64 id);

			virtual ObjectClass GetObjectClass() const;

			void SetTimeAdj(Text::String *camera, Int32 timeAdj);
			void SetTimeAdj(Text::CString camera, Int32 timeAdj);
		};
	}
}
#endif