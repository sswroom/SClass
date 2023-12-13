#ifndef _SM_IO_PACKAGEFILE
#define _SM_IO_PACKAGEFILE
#include "Data/Timestamp.h"
#include "Data/Compress/Decompressor.h"
#include "IO/ActiveStreamReader.h"
#include "IO/ParsedObject.h"
#include "IO/ProgressHandler.h"
#include "IO/StreamData.h"
#include "Text/CString.h"

namespace IO
{
	enum class PackageFileType
	{
		Directory,
		Virtual
	};

	class PackageFile : public IO::ParsedObject
	{
	public:
		enum class PackObjectType
		{
			Unknown,
			StreamData,
			ParsedObject,
			PackageFileType
		};

	public:
		PackageFile(NotNullPtr<Text::String> fileName);
		PackageFile(Text::CStringNN fileName);
		virtual ~PackageFile();

		virtual IO::ParserType GetParserType() const;

		virtual UOSInt GetCount() const = 0;
		virtual PackObjectType GetItemType(UOSInt index) const = 0;
		virtual UTF8Char *GetItemName(UTF8Char *sbuff, UOSInt index) const = 0;
		virtual Optional<IO::StreamData> GetItemStmDataNew(UOSInt index) const = 0;
		Optional<IO::StreamData> GetItemStmDataNew(Text::CStringNN name) const;
		virtual Optional<IO::PackageFile> GetItemPack(UOSInt index, OutParam<Bool> needRelease) const = 0;
		virtual Optional<IO::ParsedObject> GetItemPObj(UOSInt index, OutParam<Bool> needRelease) const = 0;
		Optional<IO::PackageFile> GetItemPack(Text::CStringNN path, OutParam<Bool> needRelease) const;
		virtual Data::Timestamp GetItemModTime(UOSInt index) const = 0;
		virtual Data::Timestamp GetItemAccTime(UOSInt index) const = 0;
		virtual Data::Timestamp GetItemCreateTime(UOSInt index) const = 0;
		virtual UInt32 GetItemUnixAttr(UOSInt index) const = 0;
		virtual UInt64 GetItemStoreSize(UOSInt index) const = 0;
		virtual UInt64 GetItemSize(UOSInt index) const = 0;
		virtual UOSInt GetItemIndex(Text::CStringNN name) const = 0;
		virtual Bool IsCompressed(UOSInt index) const = 0;
		virtual Data::Compress::Decompressor::CompressMethod GetItemComp(UOSInt index) const = 0;
		virtual NotNullPtr<PackageFile> Clone() const = 0;
		virtual PackageFileType GetFileType() const = 0;
		virtual Bool CopyFrom(Text::CStringNN fileName, IO::ProgressHandler *progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt) = 0;
		virtual Bool MoveFrom(Text::CStringNN fileName, IO::ProgressHandler *progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt) = 0;
		virtual Bool RetryCopyFrom(Text::CStringNN fileName, IO::ProgressHandler *progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt) = 0;
		virtual Bool RetryMoveFrom(Text::CStringNN fileName, IO::ProgressHandler *progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt) = 0;
		virtual Bool CopyTo(UOSInt index, Text::CString destPath, Bool fullFileName) = 0;
		virtual Optional<IO::StreamData> OpenStreamData(Text::CString fileName) const = 0;
		virtual Bool HasParent() const = 0;
		virtual IO::PackageFile *GetParent(OutParam<Bool> needRelease) const = 0;
		virtual Bool DeleteItem(UOSInt index) = 0;
		virtual void SetParent(IO::PackageFile *pkg) = 0;
	};
}
#endif
