#include "Stdafx.h"
#include "MyMemory.h"
#include "Parser/FileParser/VFPParser.h"
#include "Media/MediaFile.h"

Parser::FileParser::VFPParser::VFPParser()
{
	NEW_CLASS(vfpMgr, Media::VFPManager());
}

Parser::FileParser::VFPParser::~VFPParser()
{
	vfpMgr->Release();
}

Int32 Parser::FileParser::VFPParser::GetName()
{
	return *(Int32*)"VFPP";
}

void Parser::FileParser::VFPParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MediaFile)
	{
		this->vfpMgr->PrepareSelector(selector);
	}
}

IO::ParserType Parser::FileParser::VFPParser::GetParserType()
{
	return IO::ParserType::MediaFile;
}

Optional<IO::ParsedObject> Parser::FileParser::VFPParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	if (!fd->IsFullFile())
		return 0;
	Data::ArrayListNN<Media::MediaSource> mediaArr;
	if (vfpMgr->LoadFile(fd->GetFullFileName()->v, mediaArr) > 0)
	{
		Media::MediaFile *file;
		NEW_CLASS(file, Media::MediaFile(fd->GetFullName()));
		UOSInt i = 0;
		UOSInt j = mediaArr.GetCount();
		while (i < j)
		{
			file->AddSource(mediaArr.GetItemNoCheck(i++), 0);
		}
		return file;
	}
	return 0;
}
