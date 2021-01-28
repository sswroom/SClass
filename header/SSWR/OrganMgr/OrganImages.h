#ifndef _SM_SSWR_ORGANMGR_ORGANIMAGES
#define _SM_SSWR_ORGANMGR_ORGANIMAGES

#include "SSWR/OrganMgr/OrganGroupItem.h"
#include "SSWR/OrganMgr/OrganImageItem.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganImages : public OrganGroupItem
		{
		private:
			OrganImageItem *imgItem;
			const UTF8Char *srcImgDir;

		public:
			OrganImages(OrganImageItem *imgItem, const UTF8Char *srcImgDir);
			virtual ~OrganImages();

			OrganImageItem *GetImgItem();
			const UTF8Char *GetSrcImgDir();

			virtual ItemType GetItemType();
			virtual UTF8Char *GetItemName(UTF8Char *buff);
			virtual UTF8Char *GetEngName(UTF8Char *buff);
			virtual OrganGroupItem *Clone();
		};
	}
}
#endif
