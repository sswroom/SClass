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

void Parser::FileParser::AUIParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_VIDEO_PARSER)
	{
		this->auiMgr->PrepareSelector(selector);
	}
}

IO::ParsedObject::ParserType Parser::FileParser::AUIParser::GetParserType()
{
	return IO::ParsedObject::PT_VIDEO_PARSER;
}

IO::ParsedObject *Parser::FileParser::AUIParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	if (!fd->IsFullFile())
		return 0;
	Data::ArrayList<Media::IMediaSource *> mediaArr;
	if (auiMgr->LoadFile(fd->GetFullFileName(), &mediaArr) > 0)
	{
		Media::MediaFile *file;
		NEW_CLASS(file, Media::MediaFile(fd->GetFullName()));
		OSInt i = 0;
		OSInt j = mediaArr.GetCount();
		while (i < j)
		{
			file->AddSource(mediaArr.GetItem(i++), 0);
		}
		return file;
	}
	return 0;
}
