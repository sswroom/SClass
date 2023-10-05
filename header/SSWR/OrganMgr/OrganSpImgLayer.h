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
			Data::ArrayList<UserFileInfo *> objList;

		public:
			OrganSpImgLayer();
			virtual ~OrganSpImgLayer();

			virtual Map::DrawLayerType GetLayerType() const;
			virtual UOSInt GetAllObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, Map::NameArray **nameArr);
			virtual UOSInt GetObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, Map::NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
			virtual UOSInt GetObjectIdsMapXY(NotNullPtr<Data::ArrayListInt64> outArr, Map::NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
			virtual Int64 GetObjectIdMax() const;
			virtual void ReleaseNameArr(Map::NameArray *nameArr);
			virtual UTF8Char *GetString(UTF8Char *buff, UOSInt buffSize, Map::NameArray *nameArr, Int64 id, UOSInt strIndex);
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

			void ClearItems();
			void AddItems(Data::ArrayList<OrganImageItem*> *objList);
			void AddItems(Data::ArrayList<UserFileInfo*> *objList);
			void AddItem(UserFileInfo *obj);
		};
	}
}
#endif