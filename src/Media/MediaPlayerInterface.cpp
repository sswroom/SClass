#include "Stdafx.h"
#include "IO/StmData/FileData.h"
#include "Media/MediaPlayerInterface.h"

void Media::MediaPlayerInterface::OnMediaOpened()
{
}

void Media::MediaPlayerInterface::OnMediaClosed()
{
}

Media::MediaPlayerInterface::MediaPlayerInterface(Parser::ParserList *parsers)
{
	this->parsers = parsers;
}

Media::MediaPlayerInterface::~MediaPlayerInterface()
{

}

Bool Media::MediaPlayerInterface::OpenFile(const UTF8Char *fileName)
{
	IO::ParsedObject *pobj;
	IO::StmData::FileData *fd;

	NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
	pobj = this->parsers->ParseFileType(fd, IO::ParsedObject::PT_VIDEO_PARSER);
	if (pobj)
	{
		DEL_CLASS(fd);
		return OpenVideo((Media::MediaFile*)pobj);
	}
	else
	{
		DEL_CLASS(fd);
		return false;
	}
}

Bool Media::MediaPlayerInterface::OpenVideo(Media::MediaFile *mf)
{
	return false;
}
