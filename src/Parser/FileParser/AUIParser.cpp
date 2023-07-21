#include "Stdafx.h"
#include "MyMemory.h"
#include "Parser/FileParser/AUIParser.h"
#include "Media/MediaFile.h"

Parser::FileParser::AUIParser::AUIParser()
{
	NEW_CLASS(auiMgr, Media::AVIUtl::AUIManager());
}

Parser::FileParser::AUIParser::~AUIParser()
{
	DEL_CLASS(auiMgr);
}

Int32 Parser::FileParser::AUIParser::GetName()
{
	return *(Int32*)"AUIP";
}

void Parser::FileParser::AUIParser::PrepareSelector(IO::FileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MediaFile)
	{
		this->auiMgr->PrepareSelector(selector);
	}
}

IO::ParserType Parser::FileParser::AUIParser::GetParserType()
{
	return IO::ParserType::MediaFile;
}

IO::ParsedObject *Parser::FileParser::AUIParser::ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	if (!fd->IsFullFile())
		return 0;
	Data::ArrayList<Media::IMediaSource *> mediaArr;
	if (auiMgr->LoadFile(fd->GetFullFileName()->v, &mediaArr) > 0)
	{
		Media::MediaFile *file;
		NEW_CLASS(file, Media::MediaFile(fd->GetFullName()));
		UOSInt i = 0;
		UOSInt j = mediaArr.GetCount();
		while (i < j)
		{
			file->AddSource(mediaArr.GetItem(i++), 0);
		}
		return file;
	}
	return 0;
}
