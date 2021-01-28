#ifndef _SM_SSWR_ORGANMGR_ORGANGROUP
#define _SM_SSWR_ORGANMGR_ORGANGROUP

#include "SSWR/OrganMgr/OrganGroupItem.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganGroup : public OrganGroupItem
		{
		private:
			Int32 groupId;
			const UTF8Char *cName;
			const UTF8Char *eName;
			Int32 groupType;
			const UTF8Char *desc;
			Int32 photoGroup;
			Int32 photoSpecies;
			const UTF8Char *idKey;
			Bool isDefault;
			Bool adminOnly;

		public:
			OrganGroup();
			virtual ~OrganGroup();

			void SetGroupId(Int32 groupId);
			Int32 GetGroupId();
			void SetCName(const UTF8Char *cName);
			const UTF8Char *GetCName();
			void SetEName(const UTF8Char *eName);
			const UTF8Char *GetEName();
			void SetGroupType(Int32 groupType);
			Int32 GetGroupType();
			void SetDesc(const UTF8Char *desc);
			const UTF8Char *GetDesc();
			void SetPhotoGroup(Int32 photoGroup);
			Int32 GetPhotoGroup();
			void SetPhotoSpecies(Int32 photoSpecies);
			Int32 GetPhotoSpecies();
			void SetIDKey(const UTF8Char *idKey);
			const UTF8Char *GetIDKey();
			void SetIsDefault(Bool isDefault);
			Bool GetIsDefault();
			void SetAdminOnly(Bool isAdminOnly);
			Bool GetAdminOnly();

			virtual ItemType GetItemType();
			virtual UTF8Char *GetItemName(UTF8Char *buff);
			virtual UTF8Char *GetEngName(UTF8Char *buff);
			virtual OrganGroupItem *Clone();
		};
	}
}
#endif