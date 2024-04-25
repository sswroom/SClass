#ifndef _SM_IO_VIRTUALPACKAGEFILE
#define _SM_IO_VIRTUALPACKAGEFILE
#include "Crypto/Hash/IHash.h"
#include "Data/ArrayListNN.h"
#include "Data/FastMap.h"
#include "Data/FastStringMap.h"
#include "Data/StringMap.h"
#include "Data/Timestamp.h"
#include "Data/Compress/Decompressor.h"
#include "IO/ActiveStreamReader.h"
#include "IO/PackageFile.h"
#include "IO/ProgressHandler.h"
#include "IO/StreamData.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	struct PackFileItem
	{
		enum class PackItemType
		{
			Uncompressed,
			ParsedObject,
			Compressed
		};

		enum class HeaderType
		{
			No,
			Zip
		};

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

		NN<Text::String> name;
		IO::StreamData *fullFd;
		UInt64 fileOfst;
		UInt64 dataLength;
		HeaderType headerType;
		Optional<IO::ParsedObject> pobj;
		PackItemType itemType;
		CompressInfo *compInfo;
		Data::Timestamp modTime;
		Data::Timestamp accTime;
		Data::Timestamp createTime;
		UInt32 unixAttr;
		Int32 useCnt;
	};

	class VirtualPackageFile : public PackageFile
	{
	public:
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

	protected:
		Data::ArrayListNN<PackFileItem> items;
		Data::FastStringMap<PackFileItem *> pkgFiles;
		Data::FastMap<Int32, const UTF8Char *> infoMap;
		Optional<PackageFile> parent;

		void ReusePackFileItem(NN<IO::PackFileItem> item);
		VirtualPackageFile(NN<Text::String> fileName);
		VirtualPackageFile(Text::CStringNN fileName);

		virtual Optional<const PackFileItem> GetItemByName(Text::CStringNN name) const = 0;
		virtual void PutItem(NN<Text::String> name, NN<PackFileItem> item) = 0;
		virtual void RemoveItem(NN<Text::String> name) = 0;
	public:
		virtual ~VirtualPackageFile();

		Bool AddData(NN<StreamData> fd, UInt64 ofst, UInt64 dataLength, PackFileItem::HeaderType headerType, Text::CStringNN name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr);
		Bool AddObject(IO::ParsedObject *pobj, Text::CStringNN name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr);
		Bool AddCompData(NN<StreamData> fd, UInt64 ofst, UInt64 dataLength, PackFileItem::HeaderType headerType, PackFileItem::CompressInfo *compInfo, Text::CStringNN name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr);
		Bool AddPack(NN<IO::PackageFile> pkg, Text::CStringNN name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr);
		Bool AddOrReplaceData(NN<StreamData> fd, UInt64 ofst, UInt64 dataLength, PackFileItem::HeaderType headerType, Text::CStringNN name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr);
		Bool AddOrReplaceObject(IO::ParsedObject *pobj, Text::CStringNN name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr);
		Bool AddOrReplaceCompData(NN<StreamData> fd, UInt64 ofst, UInt64 dataLength, PackFileItem::HeaderType headerType, PackFileItem::CompressInfo *compInfo, Text::CStringNN name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr);
		Bool AddOrReplacePack(NN<IO::PackageFile> pkg, Text::CStringNN name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr);
		Optional<IO::PackageFile> GetPackFile(Text::CStringNN name) const;
		Bool MergePackage(NN<IO::PackageFile> pkg);

		Optional<const PackFileItem> GetPackFileItem(const UTF8Char *name) const;
		Optional<const PackFileItem> GetPackFileItem(UOSInt index) const;
		PackObjectType GetPItemType(NN<const PackFileItem> itemObj) const;
		UInt64 GetPItemDataOfst(NN<const PackFileItem> itemObj) const;
		Optional<IO::StreamData> GetPItemStmDataNew(NN<const PackFileItem> itemObj) const;
		Optional<IO::PackageFile> GetPItemPack(NN<const PackFileItem> itemObj, OutParam<Bool> needRelease) const;
		Data::ArrayIterator<NN<PackFileItem>> PackFileIterator() const;

		virtual UOSInt GetCount() const;
		virtual PackObjectType GetItemType(UOSInt index) const;
		virtual UTF8Char *GetItemName(UTF8Char *sbuff, UOSInt index) const;
		virtual Optional<IO::StreamData> GetItemStmDataNew(UOSInt index) const;
		Optional<IO::StreamData> GetItemStmDataNew(Text::CStringNN name) const;
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
		virtual PackageFileType GetFileType() const;
		virtual Bool CopyFrom(Text::CStringNN fileName, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt);
		virtual Bool MoveFrom(Text::CStringNN fileName, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt);
		virtual Bool RetryCopyFrom(Text::CStringNN fileName, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt);
		virtual Bool RetryMoveFrom(Text::CStringNN fileName, Optional<IO::ProgressHandler> progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt);
		virtual Bool CopyTo(UOSInt index, Text::CString destPath, Bool fullFileName);
		virtual Optional<IO::StreamData> OpenStreamData(Text::CString fileName) const;
		virtual Bool HasParent() const;
		virtual Optional<IO::PackageFile> GetParent(OutParam<Bool> needRelease) const;
		virtual Bool DeleteItem(UOSInt index);
		virtual void SetParent(Optional<IO::PackageFile> pkg);

		void SetInfo(InfoType infoType, const UTF8Char *val);
		void GetInfoText(NN<Text::StringBuilderUTF8> sb) const;
		static Text::CString GetInfoTypeName(InfoType infoType);
	};
}
#endif
