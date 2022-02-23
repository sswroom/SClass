#ifndef _SM_SSWR_ORGANMGR_ORGANSPECIES
#define _SM_SSWR_ORGANMGR_ORGANSPECIES

#include "SSWR/OrganMgr/OrganGroupItem.h"
#include "Text/String.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganSpecies : public OrganGroupItem
		{
		private:
			Int32 speciesId;
			Text::String *cName;
			Text::String *eName;
			Text::String *sName;
			Int32 groupId;
			Text::String *desc;
			Text::String *dirName;
			Text::String *photo;
			Text::String *idKey;
			Bool isDefault;
			Int32 flags;
			Int32 photoId;
			Int32 photoWId;
			UInt32 mapColor;

		public:
			OrganSpecies();
			virtual ~OrganSpecies();

			void SetSpeciesId(Int32 speciesId);
			Int32 GetSpeciesId();
			void SetCName(Text::CString cName);
			Text::String *GetCName();
			void SetEName(Text::CString eName);
			Text::String *GetEName();
			void SetSName(Text::CString sName);
			Text::String *GetSName();
			void SetGroupId(Int32 groupId);
			Int32 GetGroupId();
			void SetDesc(Text::CString desc);
			Text::String *GetDesc();
			void SetDirName(Text::CString dirName);
			Text::String *GetDirName();
			void SetPhoto(Text::CString photo);
			Text::String *GetPhoto();
			void SetIDKey(Text::CString idKey);
			Text::String *GetIDKey();
			void SetFlags(Int32 flags);
			Int32 GetFlags();
			void SetIsDefault(Bool isDefault);
			Bool GetIsDefault();
			void SetPhotoId(Int32 photoId);
			Int32 GetPhotoId();
			void SetPhotoWId(Int32 photoWId);
			Int32 GetPhotoWId();
			void SetMapColor(UInt32 mapColor);
			UInt32 GetMapColor();


			virtual ItemType GetItemType();
			virtual UTF8Char *GetItemName(UTF8Char *buff);
			virtual UTF8Char *GetEngName(UTF8Char *buff);
			virtual OrganGroupItem *Clone();
		};
	}
}
#endif