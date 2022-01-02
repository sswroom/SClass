#ifndef _SM_IO_PACKAGEFILE
#define _SM_IO_PACKAGEFILE
#include "Crypto/Hash/IHash.h"
#include "Data/ArrayList.h"
#include "Data/Int32Map.h"
#include "Data/FastStringMap.h"
#include "Data/StringMap.h"
#include "Data/Compress/Decompressor.h"
#include "IO/ActiveStreamReader.h"
#include "IO/IProgressHandler.h"
#include "IO/IStreamData.h"
#include "IO/ParsedObject.h"
#include "Text/StringBuilderUTF.h"

namespace IO
{
	struct PackFileItem
	{
		typedef enum
		{
			PIT_UNCOMPRESSED,
			PIT_PARSEDOBJECT,
			PIT_COMPRESSED
		} PackItemType;

		struct CompressInfo
		{
			UInt64 decSize;
			Data::Compress::Decompressor::CompressMethod compMethod;
			Crypto::Hash::HashType checkMethod;
			Int32 compFlags;
			UInt32 compExtraSize;
			UInt8 *compExtras;
			UInt8 checkBytes[32];
		};

		Text::String *name;
		IO::IStreamData *fd;
		IO::ParsedObject *pobj;
		PackItemType itemType;
		CompressInfo *compInfo;
		Int64 modTimeTick;
		Int32 useCnt;
	};

	class PackageFile : public IO::ParsedObject
	{
	public:
		typedef enum
		{
			POT_UNKNOWN,
			POT_STREAMDATA,
			POT_PARSEDOBJECT,
			POT_PACKAGEFILE
		} PackObjectType;

		typedef enum
		{
			IT_SYSTEM_ID,
			IT_VOLUME_ID,
			IT_VOLUME_SET_ID,
			IT_PUBLISHER_ID,
			IT_DATA_PREPARER_ID,
			IT_APPLICATION_ID,
			IT_COPYRIGHT_FILE_ID,
			IT_ABSTRACT_FILE_ID,
			IT_BIBLIOGRAHPICAL_FILE_ID
		} InfoType;

	private:
		Data::ArrayList<PackFileItem *> *items;
		Data::StringMap<PackFileItem *> *namedItems;
		Data::FastStringMap<PackFileItem *> *pkgFiles;
		Data::Int32Map<const UTF8Char *> *infoMap;

		PackageFile(PackageFile *pkg);
	public:
		PackageFile(Text::String *fileName);
		PackageFile(const UTF8Char *fileName);
		virtual ~PackageFile();

		virtual IO::ParserType GetParserType();

		void AddData(IStreamData *fd, UInt64 ofst, UInt64 length, const UTF8Char *name, Int64 modTimeTick);
		void AddObject(IO::ParsedObject *pobj, const UTF8Char *name, Int64 modTimeTick);
		void AddCompData(IStreamData *fd, UInt64 ofst, UInt64 length, PackFileItem::CompressInfo *compInfo, const UTF8Char *name, Int64 modTimeTick);
		void AddPack(IO::PackageFile *pkg, const UTF8Char *name, Int64 modTimeTick);
		IO::PackageFile *GetPackFile(const UTF8Char *name);
		Bool UpdateCompInfo(const UTF8Char *name, IO::IStreamData *fd, UInt64 ofst, Int32 crc, UOSInt compSize, UInt32 decSize);

		virtual const PackFileItem *GetPackFileItem(const UTF8Char *name);
		virtual PackObjectType GetPItemType(const PackFileItem *itemObj);
		virtual IO::IStreamData *GetPItemStmData(const PackFileItem *itemObj); // need release
		virtual IO::PackageFile *GetPItemPack(const PackFileItem *itemObj); // need release

		virtual UOSInt GetCount();
		virtual PackObjectType GetItemType(UOSInt index);
		virtual UTF8Char *GetItemName(UTF8Char *sbuff, UOSInt index);
		virtual IO::IStreamData *GetItemStmData(UOSInt index); // need release
		IO::IStreamData *GetItemStmData(const UTF8Char* name);
		virtual IO::PackageFile *GetItemPack(UOSInt index); // need release
		virtual IO::ParsedObject *GetItemPObj(UOSInt index); // no need release
		virtual Int64 GetItemModTimeTick(UOSInt index);
		virtual UInt64 GetItemSize(UOSInt index);
		virtual UOSInt GetItemIndex(const UTF8Char *name);
		virtual Bool IsCompressed(UOSInt index);
		virtual Data::Compress::Decompressor::CompressMethod GetItemComp(UOSInt index);
		virtual PackageFile *Clone();
		virtual Bool AllowWrite();
		virtual Bool CopyFrom(const UTF8Char *fileName, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt);
		virtual Bool MoveFrom(const UTF8Char *fileName, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt);
		virtual Bool RetryCopyFrom(const UTF8Char *fileName, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt);
		virtual Bool RetryMoveFrom(const UTF8Char *fileName, IO::IProgressHandler *progHdlr, IO::ActiveStreamReader::BottleNeckType *bnt);
		virtual Bool CopyTo(UOSInt index, const UTF8Char *destPath, Bool fullFileName);
		virtual IO::IStreamData *OpenStreamData(const UTF8Char *fileName);

		void SetInfo(InfoType infoType, const UTF8Char *val);
		void GetInfoText(Text::StringBuilderUTF *sb);
		static const UTF8Char *GetInfoTypeName(InfoType infoType);
	};
};
#endif
