#ifndef _SM_SSWR_ORGANMGR_ORGANSPIMGLAYER
#define _SM_SSWR_ORGANMGR_ORGANSPIMGLAYER
#include "Map/MapDrawLayer.h"
#include "SSWR/OrganMgr/OrganImageItem.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganSpImgLayer : public Map::MapDrawLayer
		{
		private:
			Math::Coord2DDbl min;
			Math::Coord2DDbl max;
			Data::ArrayListNN<UserFileInfo> objList;

		public:
			OrganSpImgLayer();
			virtual ~OrganSpImgLayer();

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
			virtual UIntOS GetGeomCol() const;

			virtual ObjectClass GetObjectClass() const;

			void ClearItems();
			void AddItems(NN<Data::ArrayListNN<OrganImageItem>> objList);
			void AddItems(NN<Data::ArrayListNN<UserFileInfo>> objList);
			void AddItem(NN<UserFileInfo> obj);
		};
	}
}
#endif