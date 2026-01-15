#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Exporter/ZIPExporter.h"
#include "IO/VirtualPackageFile.h"

#define VERBOSE 1
#if defined(VERBOSE)
#include <stdio.h>
#endif

Exporter::ZIPExporter::ZIPExporter()
{
}

Exporter::ZIPExporter::~ZIPExporter()
{
}

Int32 Exporter::ZIPExporter::GetName()
{
	return *(Int32*)"ZIPE";
}

IO::FileExporter::SupportType Exporter::ZIPExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
{
	if (pobj->GetParserType() == IO::ParserType::PackageFile)
	{
		return IO::FileExporter::SupportType::NormalStream;
	}
	return IO::FileExporter::SupportType::NotSupported;
}

Bool Exporter::ZIPExporter::GetOutputName(UIntOS index, UnsafeArray<UTF8Char> nameBuff, UnsafeArray<UTF8Char> fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("ZIP File"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.zip"));
		return true;
	}
	return false;
}

Bool Exporter::ZIPExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	if (pobj->GetParserType() != IO::ParserType::PackageFile)
	{
		return false;
	}
	UTF8Char sbuff[512];
	IO::ZIPMTBuilder zip(stm, IO::ZIPOS::UNIX);
	return this->ExportPackage(zip, sbuff, sbuff, NN<IO::PackageFile>::ConvertFrom(pobj));
}

Bool Exporter::ZIPExporter::ExportPackage(NN<IO::ZIPMTBuilder> zip, UnsafeArray<UTF8Char> buffStart, UnsafeArray<UTF8Char> buffEnd, NN<IO::PackageFile> pkg)
{
	UIntOS i;
	UIntOS j;
	UnsafeArray<UTF8Char> sptr;
	IO::PackageFile::PackObjectType itemType;
	NN<IO::StreamData> fd;
	i = 0;
	j = pkg->GetCount();
	while (i < j)
	{
		itemType = pkg->GetItemType(i);
		if (!pkg->GetItemName(buffEnd, i).SetTo(sptr))
		{

		}
		else if (itemType == IO::PackageFile::PackObjectType::StreamData)
		{
			if (pkg->GetFileType() == IO::PackageFileType::Virtual)
			{
				NN<const IO::PackFileItem> pitem;
				if (NN<IO::VirtualPackageFile>::ConvertFrom(pkg)->GetPackFileItem(i).SetTo(pitem) && pitem->itemType == IO::PackFileItem::PackItemType::Compressed && pitem->compInfo->compMethod == Data::Compress::Decompressor::CM_DEFLATE)
				{
					UInt64 dataSize = pitem->dataLength;
					UIntOS readSize;
					Data::ByteBuffer buff((UIntOS)dataSize);
					if ((readSize = pitem->fullFd->GetRealData(NN<IO::VirtualPackageFile>::ConvertFrom(pkg)->GetPItemDataOfst(pitem), (UIntOS)dataSize, buff)) != dataSize)
					{
#if defined(VERBOSE)
						printf("ZIPExp: Error in reading compressed data: dataSize = %lld, readSize = %lld, fileName = %s\r\n", dataSize, (UInt64)readSize, pitem->name->v.Ptr());
#endif
						return false;
					}
					if (!zip->AddDeflate(CSTRP(buffStart, sptr), buff, pitem->compInfo->decSize, ReadMUInt32(pitem->compInfo->checkBytes), pitem->modTime, pitem->accTime, pitem->createTime, pitem->unixAttr))
					{
						return false;
					}
				}
				else if (pkg->GetItemStmDataNew(i).SetTo(fd))
				{
					if (!zip->AddFile(CSTRP(buffStart, sptr), fd, pkg->GetItemModTime(i), pkg->GetItemAccTime(i), pkg->GetItemCreateTime(i), Data::Compress::Inflate::CompressionLevel::BestCompression, pkg->GetItemUnixAttr(i)))
					{
						fd.Delete();
						return false;
					}
					fd.Delete();
				}
				else
				{
#if defined(VERBOSE)
					printf("ZIPExp: Error in reading file %s\r\n", buffStart.Ptr());
#endif
					return false;
				}
			}
			else if (pkg->GetItemStmDataNew(i).SetTo(fd))
			{
				if (!zip->AddFile(CSTRP(buffStart, sptr), fd, pkg->GetItemModTime(i), pkg->GetItemAccTime(i), pkg->GetItemCreateTime(i), Data::Compress::Inflate::CompressionLevel::BestCompression, pkg->GetItemUnixAttr(i)))
				{
					fd.Delete();
					return false;
				}
				fd.Delete();
			}				
			else
			{
#if defined(VERBOSE)
				printf("ZIPExp: Error in reading file %s\r\n", buffStart.Ptr());
#endif
				return false;
			}
		}
		else if (itemType == IO::PackageFile::PackObjectType::PackageFileType)
		{
			*sptr++ = '/';
			*sptr = 0;
			zip->AddDir(CSTRP(buffStart, sptr), pkg->GetItemModTime(i), pkg->GetItemAccTime(i), pkg->GetItemCreateTime(i), pkg->GetItemUnixAttr(i));
			NN<IO::PackageFile> innerPkg;
			Bool innerNeedDelete;
			if (pkg->GetItemPack(i, innerNeedDelete).SetTo(innerPkg))
			{
				if (!this->ExportPackage(zip, buffStart, sptr, innerPkg))
				{
					if (innerNeedDelete)
						innerPkg.Delete();
					return false;
				}
				if (innerNeedDelete)
					innerPkg.Delete();
			}
		}
		i++;
	}
	return true;
}
