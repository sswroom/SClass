#ifndef _SM_SSWR_ORGANMGR_ORGANTIMEADJLAYER
#define _SM_SSWR_ORGANMGR_ORGANTIMEADJLAYER

#include "Data/StringMapNative.hpp"
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
			NN<Map::GPSTrack> gpsTrk;
			NN<Data::ArrayListNN<UserFileInfo>> userFileList;
			Data::StringMapNative<Int32> cameraMap;
		public:
			OrganTimeAdjLayer(NN<Map::GPSTrack> gpsTrk, NN<Data::ArrayListNN<UserFileInfo>> userFileList);
			virtual ~OrganTimeAdjLayer();

			virtual Map::DrawLayerType GetLayerType() const;
			virtual UOSInt GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<Map::NameArray>> nameArr);
			virtual UOSInt GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<Map::NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
			virtual UOSInt GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<Map::NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
			virtual Int64 GetObjectIdMax() const;
			virtual UOSInt GetRecordCnt() const;
			virtual void ReleaseNameArr(Optional<Map::NameArray> nameArr);
			virtual Bool GetString(NN<Text::StringBuilderUTF8> sb, Optional<Map::NameArray> nameArr, Int64 id, UOSInt strIndex);
			virtual UOSInt GetColumnCnt() const;
			virtual UnsafeArrayOpt<UTF8Char> GetColumnName(UnsafeArray<UTF8Char> buff, UOSInt colIndex) const;
			virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize) const;
			virtual Bool GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef) const;
			virtual UInt32 GetCodePage() const;
			virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;

			virtual NN<Map::GetObjectSess> BeginGetObject();
			virtual void EndGetObject(NN<Map::GetObjectSess> session);
			virtual Optional<Math::Geometry::Vector2D> GetNewVectorById(NN<Map::GetObjectSess> session, Int64 id);
			virtual UOSInt GetGeomCol() const;

			virtual ObjectClass GetObjectClass() const;

			void SetTimeAdj(Optional<Text::String> camera, Int32 timeAdj);
			void SetTimeAdj(Text::CString camera, Int32 timeAdj);
		};
	}
}
#endif