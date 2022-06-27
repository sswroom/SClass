#ifndef _SM_SSWR_ORGANMGR_ORGANIMAGEITEM
#define _SM_SSWR_ORGANMGR_ORGANIMAGEITEM

#include "SSWR/OrganMgr/OrganEnv.h"

namespace SSWR
{
	namespace OrganMgr
	{
		class OrganImageItem
		{
		public:
			typedef enum
			{
				RT_NONE
			} RotateType;

			typedef enum
			{
				FT_UNKNOWN,
				FT_JPG,
				FT_TIF,
				FT_AVI,
				FT_WAV,
				FT_WEB_IMAGE,
				FT_USERFILE,
				FT_WEBFILE
			} FileType;
		private:
			Text::String *dispName;
			Bool isCoverPhoto;
			Int64 photoDate;
		//	const WChar *photoLoc;
			RotateType rotateType;
			Text::String *fullName;
			FileType fileType;
			Text::String *srcURL;
			Text::String *imgURL;
			UserFileInfo *userFile;
			WebFileInfo *webFile;
			Int32 userId;

		public:
			OrganImageItem(Int32 userId);
			~OrganImageItem();

			Int32 GetUserId() const;
			void SetDispName(Text::String *dispName);
			void SetDispName(Text::CString dispName);
			Text::String *GetDispName() const;
			void SetIsCoverPhoto(Bool isCoverPhoto);
			Bool GetIsCoverPhoto() const;
			void SetPhotoDate(Int64 photoDate);
			Int64 GetPhotoDate() const;
		//	void SetPhotoLoc(const WChar *photoLoc);
		//	const WChar *GetPhotoLoc();
			void SetRotateType(RotateType rotateType);
			RotateType GetRotateType() const;
			void SetFullName(Text::String *fullName);
			void SetFullName(Text::CString fullName);
			Text::String *GetFullName() const;
			void SetFileType(FileType fileType);
			FileType GetFileType() const;
			void SetSrcURL(Text::String *srcURL);
			void SetSrcURL(Text::CString srcURL);
			Text::String *GetSrcURL() const;
			void SetImgURL(Text::String *imgURL);
			void SetImgURL(Text::CString imgURL);
			Text::String *GetImgURL() const;
			void SetUserFile(UserFileInfo *userFile);
			UserFileInfo *GetUserFile() const;
			void SetWebFile(WebFileInfo *webFile);
			WebFileInfo *GetWebFile() const;

			OrganImageItem *Clone() const;

			WChar *ToString(WChar *sbuff) const;
		};
	}
}
#endif
