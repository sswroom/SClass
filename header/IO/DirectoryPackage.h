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
			NN<Text::String> fileName;
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
		NN<Text::String> dirName;
		Optional<IO::PackageFile> parent;

		void AddFile(Text::CStringNN fileName);
		void Init();
	public:
		DirectoryPackage(NN<Text::String> dirName);
		DirectoryPackage(Text::CStringNN dirName);
		virtual ~DirectoryPackage();

		virtual UOSInt GetCount() const;
		virtual PackObjectType GetItemType(UOSInt index) const;
		virtual UnsafeArrayOpt<UTF8Char> GetItemName(UnsafeArray<UTF8Char> sbuff, UOSInt index) const;
		virtual Optional<IO::StreamData> GetItemStmDataNew(UOSInt index) const;
		virtual Optional<IO::PackageFile> GetItemPack(UOSInt index, OutParam<Bool> needRelease) const;
		virtual Optional<IO::ParsedObject> GetItemPObj(UOSInt index, OutParam<Bool> needRelease) const;
		virtual Data::Timestamp GetItemModTime(UOSInt index) const;
		virtual Data::Timestamp GetItemAccTime(UOSInt index) const;
		virtual Data::Timestamp GetItemCreateTime(UOSInt index) const;
		virtual UInt32 GetItemUnixAttr(UOSInt index) const;
		virtual UInt64 GetItemStoreSize(UOSInt index) const;
		virtual UInt64 GetItemSize(UOSInt index) const;
		virtual UOSInt GetItemIndex(Text::CStringNN name) const;
		virtual Bool IsCompressed(UOSInt index) const;
		virtual Data::Compress::Decompressor::CompressMethod GetItemComp(UOSInt index) const;
		virtual NN<PackageFile> Clone() const;
		virtual PackageFileType GetFileType() const;
		virtual Bool CopyFrom(Text::CStringNN fileName, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt);
		virtual Bool MoveFrom(Text::CStringNN fileName, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt);
		virtual Bool RetryCopyFrom(Text::CStringNN fileName, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt);
		virtual Bool RetryMoveFrom(Text::CStringNN fileName, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt);
		virtual Bool CopyTo(UOSInt index, Text::CStringNN destPath, Bool fullFileName);
		virtual Optional<IO::StreamData> OpenStreamData(Text::CStringNN fileName) const;
		virtual Bool HasParent() const;
		virtual Optional<IO::PackageFile> GetParent(OutParam<Bool> needRelease) const;
		virtual Bool DeleteItem(UOSInt index);
		virtual void SetParent(Optional<IO::PackageFile> pkg);

		Bool Sort();
	};
}
#endif
