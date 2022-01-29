#ifndef _SM_IO_DIRECTORYPACKAGE
#define _SM_IO_DIRECTORYPACKAGE
#include "Data/ArrayListInt64.h"
#include "Data/ArrayListUInt64.h"
#include "IO/PackageFile.h"
#include "Text/CString.h"
#include "Text/String.h"

namespace IO
{
	class DirectoryPackage : public IO::PackageFile
	{
	private:
		Data::ArrayList<Text::String *> *files;
		Data::ArrayListUInt64 *fileSizes;
		Data::ArrayListInt64 *fileTimes;
		Text::String *dirName;

		void AddFile(Text::CString fileName);
		void Init();
	public:
		DirectoryPackage(Text::String *dirName);
		DirectoryPackage(Text::CString dirName);
		virtual ~DirectoryPackage();

		virtual UOSInt GetCount();
		virtual PackObjectType GetItemType(UOSInt index);
		virtual UTF8Char *GetItemName(UTF8Char *sbuff, UOSInt index);
		virtual IO::IStreamData *GetItemStmData(UOSInt index); // need release
		virtual IO::PackageFile *GetItemPack(UOSInt index); // need release
		virtual IO::ParsedObject *GetItemPObj(UOSInt index); // no need release
		virtual Int64 GetItemModTimeTick(UOSInt index);
		virtual UInt64 GetItemSize(UOSInt index);
		virtual UOSInt GetItemIndex(Text::CString name);
		virtual Bool IsCompressed(UOSInt index);
		virtual Data::Compress::Decompressor::CompressMethod GetItemComp(UOSInt index);
		virtual PackageFile *Clone();
		virtual Bool AllowWrite();
		virtual Bool CopyFrom(Text::CString fileName, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt);
		virtual Bool MoveFrom(Text::CString fileName, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt);
		virtual Bool RetryCopyFrom(Text::CString fileName, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt);
		virtual Bool RetryMoveFrom(Text::CString fileName, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt);

		Bool Sort();
	};
}
#endif
