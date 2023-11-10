#ifndef _SM_IO_VIRTUALPACKAGEFILE
#define _SM_IO_VIRTUALPACKAGEFILE
#include "Crypto/Hash/IHash.h"
#include "Data/ArrayList.h"
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

		NotNullPtr<Text::String> name;
		IO::StreamData *fd;
		IO::ParsedObject *pobj;
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

	private:
		NotNullPtr<Data::ArrayList<PackFileItem *>> items;
		NotNullPtr<Data::StringMap<PackFileItem *>> namedItems;
		Data::FastStringMap<PackFileItem *> pkgFiles;
		Data::FastMap<Int32, const UTF8Char *> infoMap;
		VirtualPackageFile *parent;

		VirtualPackageFile(const VirtualPackageFile *pkg);
	public:
		VirtualPackageFile(NotNullPtr<Text::String> fileName);
		VirtualPackageFile(Text::CStringNN fileName);
		virtual ~VirtualPackageFile();

		void AddData(NotNullPtr<StreamData> fd, UInt64 ofst, UInt64 length, Text::CString name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr);
		void AddObject(IO::ParsedObject *pobj, Text::CString name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr);
		void AddCompData(NotNullPtr<StreamData> fd, UInt64 ofst, UInt64 length, PackFileItem::CompressInfo *compInfo, Text::CString name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr);
		void AddPack(NotNullPtr<IO::PackageFile> pkg, Text::CString name, const Data::Timestamp &modTime, const Data::Timestamp &accTime, const Data::Timestamp &createTime, UInt32 unixAttr);
		IO::PackageFile *GetPackFile(Text::CStringNN name) const;
		Bool UpdateCompInfo(const UTF8Char *name, NotNullPtr<IO::StreamData> fd, UInt64 ofst, Int32 crc, UOSInt compSize, UInt32 decSize);

		const PackFileItem *GetPackFileItem(const UTF8Char *name) const;
		const PackFileItem *GetPackFileItem(UOSInt index) const;
		PackObjectType GetPItemType(const PackFileItem *itemObj) const;
		IO::StreamData *GetPItemStmDataNew(const PackFileItem *itemObj) const;
		IO::PackageFile *GetPItemPack(const PackFileItem *itemObj, OutParam<Bool> needRelease) const;

		virtual UOSInt GetCount() const;
		virtual PackObjectType GetItemType(UOSInt index) const;
		virtual UTF8Char *GetItemName(UTF8Char *sbuff, UOSInt index) const;
		virtual IO::StreamData *GetItemStmDataNew(UOSInt index) const;
		IO::StreamData *GetItemStmDataNew(const UTF8Char* name, UOSInt nameLen) const;
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
		virtual PackageFile *Clone() const;
		virtual PackageFileType GetFileType() const;
		virtual Bool CopyFrom(Text::CStringNN fileName, IO::ProgressHandler *progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt);
		virtual Bool MoveFrom(Text::CStringNN fileName, IO::ProgressHandler *progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt);
		virtual Bool RetryCopyFrom(Text::CStringNN fileName, IO::ProgressHandler *progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt);
		virtual Bool RetryMoveFrom(Text::CStringNN fileName, IO::ProgressHandler *progHdlr, OptOut<IO::ActiveStreamReader::BottleNeckType> bnt);
		virtual Bool CopyTo(UOSInt index, Text::CString destPath, Bool fullFileName);
		virtual IO::StreamData *OpenStreamData(Text::CString fileName) const;
		virtual Bool HasParent() const;
		virtual IO::PackageFile *GetParent(OutParam<Bool> needRelease) const;

		void SetInfo(InfoType infoType, const UTF8Char *val);
		void GetInfoText(NotNullPtr<Text::StringBuilderUTF8> sb) const;
		static Text::CString GetInfoTypeName(InfoType infoType);
	};
}
#endif
