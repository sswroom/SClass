#ifndef _SM_SSWR_ORGANMGR_ORGANSPIMGLAYER
#define _SM_SSWR_ORGANMGR_ORGANSPIMGLAYER
#include "Map/IMapDrawLayer.h"
#include "SSWR/OrganMgr/OrganImageItem.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganSpImgLayer : public Map::IMapDrawLayer
		{
		private:
			Double minX;
			Double minY;
			Double maxX;
			Double maxY;
			Data::ArrayList<UserFileInfo *> *objList;

		public:
			OrganSpImgLayer();
			virtual ~OrganSpImgLayer();

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
			virtual UInt32 GetCodePage();
			virtual Bool GetBoundsDbl(Double *minX, Double *minY, Double *maxX, Double *maxY);

			virtual void *BeginGetObject();
			virtual void EndGetObject(void *session);
			virtual Map::DrawObjectL *GetObjectByIdD(void *session, Int64 id);
			virtual Math::Vector2D *GetVectorById(void *session, Int64 id);
			virtual void ReleaseObject(void *session, Map::DrawObjectL *obj);

			virtual ObjectClass GetObjectClass();

			void ClearItems();
			void AddItems(Data::ArrayList<OrganImageItem*> *objList);
			void AddItems(Data::ArrayList<UserFileInfo*> *objList);
			void AddItem(UserFileInfo *obj);
		};
	}
}
#endif