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

void Parser::FileParser::VFPParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_VIDEO_PARSER)
	{
		this->vfpMgr->PrepareSelector(selector);
	}
}

IO::ParsedObject::ParserType Parser::FileParser::VFPParser::GetParserType()
{
	return IO::ParsedObject::PT_VIDEO_PARSER;
}

IO::ParsedObject *Parser::FileParser::VFPParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	if (!fd->IsFullFile())
		return 0;
	Data::ArrayList<Media::IMediaSource *> mediaArr;
	if (vfpMgr->LoadFile(fd->GetFullFileName(), &mediaArr) > 0)
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
