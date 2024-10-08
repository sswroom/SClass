#ifndef _SM_SSWR_ORGANMGR_ORGANPARENTITEM
#define _SM_SSWR_ORGANMGR_ORGANPARENTITEM

#include "SSWR/OrganMgr/OrganGroupItem.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganParentItem : public OrganGroupItem
		{
		public:
			OrganParentItem();
			virtual ~OrganParentItem();

			virtual ItemType GetItemType() const;
			virtual UnsafeArray<UTF8Char> GetItemName(UnsafeArray<UTF8Char> buff) const;
			virtual UnsafeArray<UTF8Char> GetEngName(UnsafeArray<UTF8Char> buff) const;
			virtual NN<OrganGroupItem> Clone() const;
		};
	}
}
#endif
