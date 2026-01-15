#include "Stdafx.h"
#include "MyMemory.h"
#include "Parser/FileParser/AUIParser.h"
#include "Media/MediaFile.h"

Parser::FileParser::AUIParser::AUIParser()
{
	NEW_CLASSNN(auiMgr, Media::AVIUtl::AUIManager());
}

Parser::FileParser::AUIParser::~AUIParser()
{
	auiMgr.Delete();
}

Int32 Parser::FileParser::AUIParser::GetName()
{
	return *(Int32*)"AUIP";
}

void Parser::FileParser::AUIParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
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

Optional<IO::ParsedObject> Parser::FileParser::AUIParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	if (!fd->IsFullFile())
		return nullptr;
	Data::ArrayListNN<Media::MediaSource> mediaArr;
	if (auiMgr->LoadFile(fd->GetFullFileName()->v, mediaArr) > 0)
	{
		Media::MediaFile *file;
		NEW_CLASS(file, Media::MediaFile(fd->GetFullName()));
		UIntOS i = 0;
		UIntOS j = mediaArr.GetCount();
		while (i < j)
		{
			file->AddSource(mediaArr.GetItemNoCheck(i++), 0);
		}
		return file;
	}
	return nullptr;
}
