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

void Media::MediaPlayerInterface::SetPlayer(Optional<Media::MediaPlayer> player)
{
	this->player.Delete();
	this->player = player;
	this->currPBC = player;
}

Media::MediaPlayerInterface::MediaPlayerInterface(NN<Parser::ParserList> parsers)
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
	this->player.Delete();
}

Bool Media::MediaPlayerInterface::OpenFile(Text::CStringNN fileName, IO::ParserType targetType)
{
	NN<IO::ParsedObject> pobj;
	IO::StmData::FileData fd(fileName, false);
	if (this->parsers->ParseFileType(fd, targetType).SetTo(pobj))
	{
		return OpenVideo(NN<Media::MediaFile>::ConvertFrom(pobj));
	}
	else
	{
		return false;
	}
}

Bool Media::MediaPlayerInterface::OpenVideo(NN<Media::MediaFile> mf)
{
	NN<Media::MediaPlayer> player;
	if (!this->player.SetTo(player))
	{
		return false;
	}
	UTF8Char sbuff[1024];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k;

	this->CloseFile();
	Bool hasAudio = false;
	Bool hasVideo = false;
	NN<Media::IMediaSource> msrc;
	Media::MediaType mt;
	IO::Path::PathType pt;
	UInt64 fileSize;
	NN<IO::Path::FindFileSession> sess;
	i = 0;
	while (mf->GetStream(i++, 0).SetTo(msrc))
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
				if (IO::Path::FindFile(CSTRP(sbuff, sptr)).SetTo(sess))
				{
					while (IO::Path::FindNextFile(&sbuff[i + 1], sess, 0, pt, fileSize).SetTo(sptr))
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
								NN<Media::MediaFile> audFile;
								IO::StmData::FileData fd(CSTRP(sbuff, sptr), false);
								if (Optional<Media::MediaFile>::ConvertFrom(this->parsers->ParseFileType(fd, IO::ParserType::MediaFile)).SetTo(audFile))
								{
									Int32 syncTime;
									k = 0;
									while (audFile->GetStream(k, syncTime).SetTo(msrc))
									{
										audFile->KeepStream(k, true);
										mf->AddSource(msrc, syncTime);
										k++;
									}
									audFile.Delete();
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
	player->LoadMedia(mf);
	this->currPBC = this->player;
	this->storeTime = (UInt32)-1;
	this->OnMediaOpened();
	return true;
}

void Media::MediaPlayerInterface::CloseFile()
{
	NN<Media::MediaPlayer> player;
	if (this->player.SetTo(player) && this->currFile.NotNull())
	{
		player->StopPlayback();
		player->LoadMedia(0);
	}
	this->storeTime = (UInt32)-1;
	this->currFile.Delete();
	this->currPBC = this->player;
	this->OnMediaClosed();
}

Optional<Media::MediaFile> Media::MediaPlayerInterface::GetOpenedFile()
{
	return this->currFile;
}

Optional<Media::VideoRenderer> Media::MediaPlayerInterface::GetVideoRenderer()
{
	NN<Media::MediaPlayer> player;
	if (!this->player.SetTo(player))
		return 0;
	return player->GetVideoRenderer();
}

void Media::MediaPlayerInterface::PBStart()
{
	NN<Media::MediaPlayer> player;
	NN<Media::IPBControl> currPBC;
	if (this->player.SetTo(player) && this->currPBC.SetTo(currPBC) && !player->IsPlaying())
	{
		currPBC->StartPlayback();
		if (!this->storeTime.IsInfinity())
		{
			player->SeekTo(this->storeTime);
			this->storeTime = Data::Duration::Infinity();
		}
	}
}

void Media::MediaPlayerInterface::PBStop()
{
	NN<Media::IPBControl> currPBC;
	if (this->currPBC.SetTo(currPBC))
	{
		currPBC->StopPlayback();
		this->storeTime = Data::Duration::Infinity();
	}
}

void Media::MediaPlayerInterface::PBPause()
{
	NN<Media::MediaPlayer> player;
	NN<Media::IPBControl> currPBC;
	if (this->player.SetTo(player) && this->currPBC.SetTo(currPBC))
	{
		if (player->IsPlaying())
		{
			this->storeTime = player->GetCurrTime();
			currPBC->StopPlayback();
		}
		else if (!this->storeTime.IsInfinity())
		{
			currPBC->StartPlayback();
			player->SeekTo(this->storeTime);
			this->storeTime = (UInt32)-1;
		}
	}
}

void Media::MediaPlayerInterface::PBPrevChapter()
{
	NN<Media::IPBControl> currPBC;
	if (this->currPBC.SetTo(currPBC)) currPBC->PrevChapter();
}

void Media::MediaPlayerInterface::PBNextChapter()
{
	NN<Media::IPBControl> currPBC;
	if (this->currPBC.SetTo(currPBC)) currPBC->NextChapter();
}

void Media::MediaPlayerInterface::PBDecAVOfst()
{
	NN<Media::MediaPlayer> player;
	NN<VideoRenderer> vrenderer;
	if (this->player.SetTo(player) && player->GetVideoRenderer().SetTo(vrenderer))
	{
		vrenderer->SetAVOfst(vrenderer->GetAVOfst() - 10);
	}
}

void Media::MediaPlayerInterface::PBIncAVOfst()
{
	NN<Media::MediaPlayer> player;
	NN<VideoRenderer> vrenderer;
	if (this->player.SetTo(player) && player->GetVideoRenderer().SetTo(vrenderer))
	{
		vrenderer->SetAVOfst(vrenderer->GetAVOfst() + 10);
	}
}

void Media::MediaPlayerInterface::PBJumpOfst(Int32 ofst)
{
	NN<Media::MediaPlayer> player;
	Data::Duration targetTime;
	if (this->player.SetTo(player) && player->IsPlaying())
	{
		targetTime = player->GetCurrTime() + ofst;
		if (targetTime.IsNegative())
		{
			targetTime = 0;
		}
		player->SeekTo(targetTime);
	}
}

void Media::MediaPlayerInterface::PBJumpToTime(UInt32 time)
{
	NN<Media::MediaPlayer> player;
	if (this->player.SetTo(player) && player->IsPlaying())
	{
		player->SeekTo(time);
	}
}
