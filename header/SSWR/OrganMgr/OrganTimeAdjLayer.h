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
			virtual UIntOS GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<Map::NameArray>> nameArr);
			virtual UIntOS GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<Map::NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
			virtual UIntOS GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<Map::NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
			virtual Int64 GetObjectIdMax() const;
			virtual UIntOS GetRecordCnt() const;
			virtual void ReleaseNameArr(Optional<Map::NameArray> nameArr);
			virtual Bool GetString(NN<Text::StringBuilderUTF8> sb, Optional<Map::NameArray> nameArr, Int64 id, UIntOS strIndex);
			virtual UIntOS GetColumnCnt() const;
			virtual UnsafeArrayOpt<UTF8Char> GetColumnName(UnsafeArray<UTF8Char> buff, UIntOS colIndex) const;
			virtual DB::DBUtil::ColType GetColumnType(UIntOS colIndex, OptOut<UIntOS> colSize) const;
			virtual Bool GetColumnDef(UIntOS colIndex, NN<DB::ColDef> colDef) const;
			virtual UInt32 GetCodePage() const;
			virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;

			virtual NN<Map::GetObjectSess> BeginGetObject();
			virtual void EndGetObject(NN<Map::GetObjectSess> session);
			virtual Optional<Math::Geometry::Vector2D> GetNewVectorById(NN<Map::GetObjectSess> session, Int64 id);
			virtual FailReason GetFailReason() const { return Map::MapDrawLayer::FailReason::IdNotFound; };
			virtual void WaitForLoad(Data::Duration maxWaitTime) {};
			virtual UIntOS GetGeomCol() const;

			virtual ObjectClass GetObjectClass() const;

			void SetTimeAdj(Optional<Text::String> camera, Int32 timeAdj);
			void SetTimeAdj(Text::CString camera, Int32 timeAdj);
		};
	}
}
#endif