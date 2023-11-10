#ifndef _SM_IO_DIRECTORYPACKAGE
#define _SM_IO_DIRECTORYPACKAGE
#include "Data/ArrayListInt64.h"
#include "Data/ArrayListNN.h"
#include "Data/ArrayListUInt64.h"
#include "IO/PackageFile.h"
#include "Text/CString.h"
#include "Text/String.h"

namespace IO
{
	class DirectoryPackage : public IO::PackageFile
	{
	public:
		struct FileItem
		{
			NotNullPtr<Text::String> fileName;
			UInt64 fileSize;
			Data::Timestamp modTime;
			Data::Timestamp accTime;
			Data::Timestamp createTime;
			UInt32 unixAttr;

			FileItem() = default;

			FileItem(std::nullptr_t)
			{
				fileName = Text::String::NewEmpty();
				fileSize = 0;
				modTime = 0;
				accTime = 0;
				createTime = 0;
				unixAttr = 0;
			}

			Bool operator==(const FileItem &item)
			{
				return fileSize == item.fileSize &&
					modTime == item.modTime &&
					accTime == item.accTime &&
					createTime == item.createTime &&
					unixAttr == item.unixAttr &&
					fileName->Equals(item.fileName);
			}
		};
	private:
		Data::ArrayList<FileItem> files;
		NotNullPtr<Text::String> dirName;

		void AddFile(Text::CStringNN fileName);
		void Init();
	public:
		DirectoryPackage(NotNullPtr<Text::String> dirName);
		DirectoryPackage(Text::CStringNN dirName);
		virtual ~DirectoryPackage();

		virtual UOSInt GetCount() const;
		virtual PackObjectType GetItemType(UOSInt index) const;
		virtual UTF8Char *GetItemName(UTF8Char *sbuff, UOSInt index) const;
		virtual IO::StreamData *GetItemStmDataNew(UOSInt index) const;
		virtual IO::PackageFile *GetItemPack(UOSInt index, OutParam<Bool> needRelease) const;
		virtual IO::ParsedObject *GetItemPObj(UOSInt index, OutParam<Bool> needRelease) const;
		virtual Data::Timestamp GetItemModTime(UOSInt index) const;
		virtual Data::Timestamp GetItemAccTime(UOSInt index) const;
		virtual Data::Timestamp GetItemCreateTime(UOSInt index) const;
		virtual UInt32 GetItemUnixAttr(UOSInt index) const;
		virtual UInt64 GetItemStoreSize(UOSInt index) const;
		virtual UInt64 GetItemSize(UOSInt index) const;
		virtual UOSInt GetItemIndex(Text::CStringNN name) const;
		virtual Bool IsCompressed(UOSInt index) const;
		virtual Data::Compress::Decompressor::CompressMethod GetItemComp(UOSInt index) const;
		virtual NotNullPtr<PackageFile> Clone() const;
		virtual PackageFileType GetFileType() const;
		virtual Bool CopyFrom(Text::CStringNN fileName, IO::ProgressHandler *progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt);
		virtual Bool MoveFrom(Text::CStringNN fileName, IO::ProgressHandler *progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt);
		virtual Bool RetryCopyFrom(Text::CStringNN fileName, IO::ProgressHandler *progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt);
		virtual Bool RetryMoveFrom(Text::CStringNN fileName, IO::ProgressHandler *progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt);
		virtual Bool CopyTo(UOSInt index, Text::CString destPath, Bool fullFileName);
		virtual IO::StreamData *OpenStreamData(Text::CString fileName) const;
		virtual Bool HasParent() const;
		virtual IO::PackageFile *GetParent(OutParam<Bool> needRelease) const;
		virtual Bool DeleteItem(UOSInt index);

		Bool Sort();
	};
}
#endif
