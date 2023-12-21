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
			Optional<Text::String> cName;
			Optional<Text::String> eName;
			Optional<Text::String> sName;
			Int32 groupId;
			Optional<Text::String> desc;
			Optional<Text::String> dirName;
			Optional<Text::String> photo;
			Optional<Text::String> idKey;
			Bool isDefault;
			Int32 flags;
			Int32 photoId;
			Int32 photoWId;
			UInt32 mapColor;

		public:
			OrganSpecies();
			virtual ~OrganSpecies();

			void SetSpeciesId(Int32 speciesId);
			Int32 GetSpeciesId() const;
			void SetCName(Text::CString cName);
			Optional<Text::String> GetCName() const;
			void SetEName(Text::CString eName);
			Optional<Text::String> GetEName() const;
			void SetSName(Text::CStringNN sName);
			Optional<Text::String> GetSName() const;
			void SetGroupId(Int32 groupId);
			Int32 GetGroupId() const;
			void SetDesc(Text::CString desc);
			Optional<Text::String> GetDesc() const;
			void SetDirName(Text::CString dirName);
			Optional<Text::String> GetDirName() const;
			void SetPhoto(Text::CString photo);
			Optional<Text::String> GetPhoto() const;
			void SetIDKey(Text::CString idKey);
			Optional<Text::String> GetIDKey() const;
			void SetFlags(Int32 flags);
			Int32 GetFlags() const;
			void SetIsDefault(Bool isDefault);
			Bool GetIsDefault() const;
			void SetPhotoId(Int32 photoId);
			Int32 GetPhotoId() const;
			void SetPhotoWId(Int32 photoWId);
			Int32 GetPhotoWId() const;
			void SetMapColor(UInt32 mapColor);
			UInt32 GetMapColor() const;


			virtual ItemType GetItemType() const;
			virtual UTF8Char *GetItemName(UTF8Char *buff) const;
			virtual UTF8Char *GetEngName(UTF8Char *buff) const;
			virtual OrganGroupItem *Clone() const;
		};
	}
}
#endif