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
			Optional<Text::String> dispName;
			Bool isCoverPhoto;
			Data::Timestamp photoDate;
		//	const WChar *photoLoc;
			RotateType rotateType;
			Optional<Text::String> fullName;
			FileType fileType;
			Optional<Text::String> srcURL;
			Optional<Text::String> imgURL;
			Optional<UserFileInfo> userFile;
			Optional<WebFileInfo> webFile;
			Int32 userId;

		public:
			OrganImageItem(Int32 userId);
			~OrganImageItem();

			Int32 GetUserId() const;
			void SetDispName(Optional<Text::String> dispName);
			void SetDispName(Text::CString dispName);
			Optional<Text::String> GetDispName() const;
			void SetIsCoverPhoto(Bool isCoverPhoto);
			Bool GetIsCoverPhoto() const;
			void SetPhotoDate(const Data::Timestamp &photoDate);
			Data::Timestamp GetPhotoDate() const;
		//	void SetPhotoLoc(const WChar *photoLoc);
		//	const WChar *GetPhotoLoc();
			void SetRotateType(RotateType rotateType);
			RotateType GetRotateType() const;
			void SetFullName(Optional<Text::String> fullName);
			void SetFullName(Text::CString fullName);
			Optional<Text::String> GetFullName() const;
			void SetFileType(FileType fileType);
			FileType GetFileType() const;
			void SetSrcURL(Optional<Text::String> srcURL);
			void SetSrcURL(Text::CString srcURL);
			Optional<Text::String> GetSrcURL() const;
			void SetImgURL(Optional<Text::String> imgURL);
			void SetImgURL(Text::CString imgURL);
			Optional<Text::String> GetImgURL() const;
			void SetUserFile(Optional<UserFileInfo> userFile);
			Optional<UserFileInfo> GetUserFile() const;
			void SetWebFile(Optional<WebFileInfo> webFile);
			Optional<WebFileInfo> GetWebFile() const;

			NN<OrganImageItem> Clone() const;
		};
	}
}
#endif
