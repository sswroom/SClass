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
		PackageFile(NN<Text::String> fileName);
		PackageFile(Text::CStringNN fileName);
		virtual ~PackageFile();

		virtual IO::ParserType GetParserType() const;

		virtual UIntOS GetCount() const = 0;
		virtual PackObjectType GetItemType(UIntOS index) const = 0;
		virtual UnsafeArrayOpt<UTF8Char> GetItemName(UnsafeArray<UTF8Char> sbuff, UIntOS index) const = 0;
		virtual Optional<IO::StreamData> GetItemStmDataNew(UIntOS index) const = 0;
		Optional<IO::StreamData> GetItemStmDataNew(Text::CStringNN name) const;
		virtual Optional<IO::PackageFile> GetItemPack(UIntOS index, OutParam<Bool> needRelease) const = 0;
		virtual Optional<IO::ParsedObject> GetItemPObj(UIntOS index, OutParam<Bool> needRelease) const = 0;
		Optional<IO::PackageFile> GetItemPack(Text::CStringNN path, OutParam<Bool> needRelease) const;
		virtual Data::Timestamp GetItemModTime(UIntOS index) const = 0;
		virtual Data::Timestamp GetItemAccTime(UIntOS index) const = 0;
		virtual Data::Timestamp GetItemCreateTime(UIntOS index) const = 0;
		virtual UInt32 GetItemUnixAttr(UIntOS index) const = 0;
		virtual UInt64 GetItemStoreSize(UIntOS index) const = 0;
		virtual UInt64 GetItemSize(UIntOS index) const = 0;
		virtual UIntOS GetItemIndex(Text::CStringNN name) const = 0;
		virtual Bool IsCompressed(UIntOS index) const = 0;
		virtual Data::Compress::Decompressor::CompressMethod GetItemComp(UIntOS index) const = 0;
		virtual NN<PackageFile> Clone() const = 0;
		virtual PackageFileType GetFileType() const = 0;
		virtual Bool CopyFrom(Text::CStringNN fileName, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt) = 0;
		virtual Bool MoveFrom(Text::CStringNN fileName, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt) = 0;
		virtual Bool RetryCopyFrom(Text::CStringNN fileName, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt) = 0;
		virtual Bool RetryMoveFrom(Text::CStringNN fileName, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt) = 0;
		virtual Bool CopyTo(UIntOS index, Text::CStringNN destPath, Bool fullFileName) = 0;
		virtual Optional<IO::StreamData> OpenStreamData(Text::CStringNN fileName) const = 0;
		virtual Bool HasParent() const = 0;
		virtual Optional<IO::PackageFile> GetParent(OutParam<Bool> needRelease) const = 0;
		virtual Bool DeleteItem(UIntOS index) = 0;
		virtual void SetParent(Optional<IO::PackageFile> pkg) = 0;
	};
}
#endif
