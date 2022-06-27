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

			OrganImageItem *GetImgItem() const;
			const UTF8Char *GetSrcImgDir() const;

			virtual ItemType GetItemType() const;
			virtual UTF8Char *GetItemName(UTF8Char *buff) const;
			virtual UTF8Char *GetEngName(UTF8Char *buff) const;
			virtual OrganGroupItem *Clone() const;
		};
	}
}
#endif
