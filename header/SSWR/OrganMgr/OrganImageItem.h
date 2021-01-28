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
			const UTF8Char *dispName;
			Bool isCoverPhoto;
			Int64 photoDate;
		//	const WChar *photoLoc;
			RotateType rotateType;
			const UTF8Char *fullName;
			FileType fileType;
			const UTF8Char *srcURL;
			const UTF8Char *imgURL;
			UserFileInfo *userFile;
			WebFileInfo *webFile;
			Int32 userId;

		public:
			OrganImageItem(Int32 userId);
			~OrganImageItem();

			Int32 GetUserId();
			void SetDispName(const UTF8Char *dispName);
			const UTF8Char *GetDispName();
			void SetIsCoverPhoto(Bool isCoverPhoto);
			Bool GetIsCoverPhoto();
			void SetPhotoDate(Int64 photoDate);
			Int64 GetPhotoDate();
		//	void SetPhotoLoc(const WChar *photoLoc);
		//	const WChar *GetPhotoLoc();
			void SetRotateType(RotateType rotateType);
			RotateType GetRotateType();
			void SetFullName(const UTF8Char *fullName);
			const UTF8Char *GetFullName();
			void SetFileType(FileType fileType);
			FileType GetFileType();
			void SetSrcURL(const UTF8Char *srcURL);
			const UTF8Char *GetSrcURL();
			void SetImgURL(const UTF8Char *imgURL);
			const UTF8Char *GetImgURL();
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
