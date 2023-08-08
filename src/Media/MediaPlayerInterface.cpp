#include "Stdafx.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Media/MediaPlayerInterface.h"

void Media::MediaPlayerInterface::OnMediaOpened()
{
}

void Media::MediaPlayerInterface::OnMediaClosed()
{
}

void Media::MediaPlayerInterface::SetPlayer(Media::MediaPlayer *player)
{
	SDEL_CLASS(this->player);
	this->player = player;
	this->currPBC = player;
}

Media::MediaPlayerInterface::MediaPlayerInterface(Parser::ParserList *parsers)
{
	this->parsers = parsers;
	this->storeTime = (UInt32)-1;
	this->currFile = 0;
	this->player = 0;
	this->currPBC = 0;
}

Media::MediaPlayerInterface::~MediaPlayerInterface()
{
	this->CloseFile();
	SDEL_CLASS(this->player);
}

Bool Media::MediaPlayerInterface::OpenFile(Text::CStringNN fileName, IO::ParserType targetType)
{
	IO::ParsedObject *pobj;

	IO::StmData::FileData fd(fileName, false);
	pobj = this->parsers->ParseFileType(fd, targetType);
	if (pobj)
	{
		return OpenVideo((Media::MediaFile*)pobj);
	}
	else
	{
		return false;
	}
}

Bool Media::MediaPlayerInterface::OpenVideo(Media::MediaFile *mf)
{
	UTF8Char sbuff[1024];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k;

	this->CloseFile();
	if (mf == 0)
	{
		return true;
	}

	Bool hasAudio = false;
	Bool hasVideo = false;
	Media::IMediaSource *msrc;
	Media::MediaType mt;
	IO::Path::PathType pt;
	UInt64 fileSize;
	IO::Path::FindFileSession *sess;
	i = 0;
	while ((msrc = mf->GetStream(i++, 0)) != 0)
	{
		mt = msrc->GetMediaType();
		if (mt == Media::MEDIA_TYPE_VIDEO)
		{
			hasVideo = true;
		}
		else if (mt == Media::MEDIA_TYPE_AUDIO)
		{
			hasAudio = true;
		}
	}
	if (hasVideo && !hasAudio)
	{
		sptr = mf->GetSourceNameObj()->ConcatTo(sbuff);
		i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
		if (i != INVALID_INDEX)
		{
			j = Text::StrLastIndexOfCharC(&sbuff[i + 1], (UOSInt)(sptr - &sbuff[i + 1]), '.');
			if (j != INVALID_INDEX)
			{
				sptr = Text::StrConcatC(&sbuff[i + j + 1], IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
				sess = IO::Path::FindFile(CSTRP(sbuff, sptr));
				if (sess)
				{
					while ((sptr = IO::Path::FindNextFile(&sbuff[i + 1], sess, 0, &pt, &fileSize)) != 0)
					{
						j = Text::StrLastIndexOfCharC(&sbuff[i + 1], (UOSInt)(sptr - &sbuff[i + 1]), '.');
						if (j != INVALID_INDEX)
						{
							Bool audFile = false;
							if (Text::StrEqualsICaseC(&sbuff[i + j + 2], (UOSInt)(sptr - &sbuff[i + j + 2]), UTF8STRC("m4a")))
							{
								audFile = true;
							}
							else if (Text::StrEqualsICaseC(&sbuff[i + j + 2], (UOSInt)(sptr - &sbuff[i + j + 2]), UTF8STRC("aac")))
							{
								audFile = true;
							}
							else if (Text::StrEqualsICaseC(&sbuff[i + j + 2], (UOSInt)(sptr - &sbuff[i + j + 2]), UTF8STRC("ac3")))
							{
								audFile = true;
							}
							else if (Text::StrEqualsICaseC(&sbuff[i + j + 2], (UOSInt)(sptr - &sbuff[i + j + 2]), UTF8STRC("wav")))
							{
								audFile = true;
							}

							if (audFile)
							{
								Media::MediaFile *audFile;
								{
									IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
									audFile = (Media::MediaFile*)this->parsers->ParseFileType(fd, IO::ParserType::MediaFile);
								}
								if (audFile)
								{
									Int32 syncTime;
									k = 0;
									while ((msrc = audFile->GetStream(k, &syncTime)) != 0)
									{
										audFile->KeepStream(k, true);
										mf->AddSource(msrc, syncTime);
										k++;
									}
									DEL_CLASS(audFile);
								}
							}
						}
					}
					IO::Path::FindFileClose(sess);
				}
			}
		}
	}
	
	this->currFile = mf;
	this->player->LoadMedia(mf);
	this->currPBC = this->player;
	this->storeTime = (UInt32)-1;
	this->OnMediaOpened();
	return true;
}

void Media::MediaPlayerInterface::CloseFile()
{
	if (this->player && this->currFile)
	{
		this->player->StopPlayback();
		this->player->LoadMedia(0);
	}
	this->storeTime = (UInt32)-1;
	SDEL_CLASS(this->currFile);
	this->currPBC = this->player;
	this->OnMediaClosed();
}

Media::MediaFile *Media::MediaPlayerInterface::GetOpenedFile()
{
	return this->currFile;
}

Media::VideoRenderer *Media::MediaPlayerInterface::GetVideoRenderer()
{
	if (this->player == 0)
		return 0;
	return this->player->GetVideoRenderer();
}

void Media::MediaPlayerInterface::PBStart()
{
	if (!this->player->IsPlaying())
	{
		this->currPBC->StartPlayback();
		if (this->storeTime != (UInt32)-1)
		{
			this->player->SeekTo(this->storeTime);
			this->storeTime = (UInt32)-1;
		}
	}
}

void Media::MediaPlayerInterface::PBStop()
{
	this->currPBC->StopPlayback();
	this->storeTime = (UInt32)-1;
}

void Media::MediaPlayerInterface::PBPause()
{
	if (this->player->IsPlaying())
	{
		this->storeTime = this->player->GetCurrTime();
		this->currPBC->StopPlayback();
	}
	else if (this->storeTime != (UInt32)-1)
	{
		this->currPBC->StartPlayback();
		this->player->SeekTo(this->storeTime);
		this->storeTime = (UInt32)-1;
	}
}

void Media::MediaPlayerInterface::PBPrevChapter()
{
	this->currPBC->PrevChapter();
}

void Media::MediaPlayerInterface::PBNextChapter()
{
	this->currPBC->NextChapter();
}

void Media::MediaPlayerInterface::PBDecAVOfst()
{
	VideoRenderer *vrenderer = this->player->GetVideoRenderer();
	vrenderer->SetAVOfst(vrenderer->GetAVOfst() - 10);
}

void Media::MediaPlayerInterface::PBIncAVOfst()
{
	VideoRenderer *vrenderer = this->player->GetVideoRenderer();
	vrenderer->SetAVOfst(vrenderer->GetAVOfst() + 10);
}

void Media::MediaPlayerInterface::PBJumpOfst(Int32 ofst)
{
	Int32 targetTime;
	if (this->player->IsPlaying())
	{
		targetTime = (Int32)this->player->GetCurrTime() + ofst;
		if (targetTime < 0)
		{
			targetTime = 0;
		}
		this->player->SeekTo((UInt32)targetTime);
	}
}

void Media::MediaPlayerInterface::PBJumpToTime(UInt32 time)
{
	if (this->player->IsPlaying())
	{
		this->player->SeekTo(time);
	}
}
