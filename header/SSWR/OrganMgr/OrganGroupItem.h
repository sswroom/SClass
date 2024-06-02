#ifndef _SM_SSWR_ORGANMGR_ORGANGROUPITEM
#define _SM_SSWR_ORGANMGR_ORGANGROUPITEM

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganGroupItem
		{
		public:
			typedef enum
			{
				IT_GROUP,
				IT_SPECIES,
				IT_PARENT,
				IT_IMAGE
			} ItemType;

			virtual ~OrganGroupItem(){};

			virtual ItemType GetItemType() const = 0;
			virtual UnsafeArray<UTF8Char> GetItemName(UnsafeArray<UTF8Char> buff) const = 0;
			virtual UnsafeArray<UTF8Char> GetEngName(UnsafeArray<UTF8Char> buff) const = 0;
			virtual NN<OrganGroupItem> Clone() const = 0;
		};
	}
}
#endif
