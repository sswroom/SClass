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

			Int32 GetUserId();
			void SetDispName(Text::String *dispName);
			void SetDispName(const UTF8Char *dispName);
			Text::String *GetDispName();
			void SetIsCoverPhoto(Bool isCoverPhoto);
			Bool GetIsCoverPhoto();
			void SetPhotoDate(Int64 photoDate);
			Int64 GetPhotoDate();
		//	void SetPhotoLoc(const WChar *photoLoc);
		//	const WChar *GetPhotoLoc();
			void SetRotateType(RotateType rotateType);
			RotateType GetRotateType();
			void SetFullName(Text::String *fullName);
			void SetFullName(const UTF8Char *fullName);
			Text::String *GetFullName();
			void SetFileType(FileType fileType);
			FileType GetFileType();
			void SetSrcURL(Text::String *srcURL);
			void SetSrcURL(const UTF8Char *srcURL);
			Text::String *GetSrcURL();
			void SetImgURL(Text::String *imgURL);
			void SetImgURL(const UTF8Char *imgURL);
			Text::String *GetImgURL();
			void SetUserFile(UserFileInfo *userFile);
			UserFileInfo *GetUserFile();
			void SetWebFile(WebFileInfo *webFile);
			WebFileInfo *GetWebFile();

			OrganImageItem *Clone();

			WChar *ToString(WChar *sbuff);
		};
	}
}
#endif
