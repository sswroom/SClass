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
			NN<OrganImageItem> imgItem;
			const UTF8Char *srcImgDir;

		public:
			OrganImages(NN<OrganImageItem> imgItem, const UTF8Char *srcImgDir);
			virtual ~OrganImages();

			NN<OrganImageItem> GetImgItem() const;
			const UTF8Char *GetSrcImgDir() const;

			virtual ItemType GetItemType() const;
			virtual UnsafeArray<UTF8Char> GetItemName(UnsafeArray<UTF8Char> buff) const;
			virtual UnsafeArray<UTF8Char> GetEngName(UnsafeArray<UTF8Char> buff) const;
			virtual NN<OrganGroupItem> Clone() const;
		};
	}
}
#endif
