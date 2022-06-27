#ifndef _SM_SSWR_ORGANMGR_ORGANGROUP
#define _SM_SSWR_ORGANMGR_ORGANGROUP

#include "SSWR/OrganMgr/OrganGroupItem.h"
#include "Text/String.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganGroup : public OrganGroupItem
		{
		private:
			Int32 groupId;
			Text::String *cName;
			Text::String *eName;
			Int32 groupType;
			Text::String *desc;
			Int32 photoGroup;
			Int32 photoSpecies;
			Text::String *idKey;
			Bool isDefault;
			Bool adminOnly;

		public:
			OrganGroup();
			virtual ~OrganGroup();

			void SetGroupId(Int32 groupId);
			Int32 GetGroupId() const;
			void SetCName(Text::CString cName);
			Text::String *GetCName() const;
			void SetEName(Text::CString eName);
			Text::String *GetEName() const;
			void SetGroupType(Int32 groupType);
			Int32 GetGroupType() const;
			void SetDesc(Text::CString desc);
			Text::String *GetDesc() const;
			void SetPhotoGroup(Int32 photoGroup);
			Int32 GetPhotoGroup() const;
			void SetPhotoSpecies(Int32 photoSpecies);
			Int32 GetPhotoSpecies() const;
			void SetIDKey(Text::CString idKey);
			Text::String *GetIDKey() const;
			void SetIsDefault(Bool isDefault);
			Bool GetIsDefault() const;
			void SetAdminOnly(Bool isAdminOnly);
			Bool GetAdminOnly() const;

			virtual ItemType GetItemType() const;
			virtual UTF8Char *GetItemName(UTF8Char *buff) const;
			virtual UTF8Char *GetEngName(UTF8Char *buff) const;
			virtual OrganGroupItem *Clone() const;
		};
	}
}
#endif