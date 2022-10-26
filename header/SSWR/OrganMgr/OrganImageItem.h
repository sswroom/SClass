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
			enum class RotateType
			{
				None
			};

			enum class FileType
			{
				Unknown,
				JPEG,
				TIFF,
				AVI,
				WAV,
				Webimage,
				UserFile,
				WebFile
			};
		private:
			Text::String *dispName;
			Bool isCoverPhoto;
			Data::Timestamp photoDate;
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
			void SetPhotoDate(Data::Timestamp photoDate);
			Data::Timestamp GetPhotoDate() const;
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
