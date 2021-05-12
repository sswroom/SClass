#ifndef _SM_SSWR_ORGANMGR_ORGANSPECIES
#define _SM_SSWR_ORGANMGR_ORGANSPECIES

#include "SSWR/OrganMgr/OrganGroupItem.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganSpecies : public OrganGroupItem
		{
		private:
			Int32 speciesId;
			const UTF8Char *cName;
			const UTF8Char *eName;
			const UTF8Char *sName;
			Int32 groupId;
			const UTF8Char *desc;
			const UTF8Char *dirName;
			const UTF8Char *photo;
			const UTF8Char *idKey;
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
			void SetCName(const UTF8Char *cName);
			const UTF8Char *GetCName();
			void SetEName(const UTF8Char *eName);
			const UTF8Char *GetEName();
			void SetSName(const UTF8Char *sName);
			const UTF8Char *GetSName();
			void SetGroupId(Int32 groupId);
			Int32 GetGroupId();
			void SetDesc(const UTF8Char *desc);
			const UTF8Char *GetDesc();
			void SetDirName(const UTF8Char *dirName);
			const UTF8Char *GetDirName();
			void SetPhoto(const UTF8Char *photo);
			const UTF8Char *GetPhoto();
			void SetIDKey(const UTF8Char *idKey);
			const UTF8Char *GetIDKey();
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