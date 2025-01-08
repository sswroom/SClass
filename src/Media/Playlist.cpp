#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/RandomOS.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Media/Playlist.h"
#include "Text/MyString.h"

void __stdcall Media::Playlist::OnPBEnd(AnyType userObj)
{
	NN<Media::Playlist> me = userObj.GetNN<Media::Playlist>();
	Data::RandomOS random;
	UOSInt i = (UInt32)random.NextInt15() % me->entries.GetCount();
	me->OpenItem(i);
}

void __stdcall Media::Playlist::FreeEntry(NN<PlaylistEntry> ent)
{
	ent->title->Release();
	OPTSTR_DEL(ent->artist);
	ent->fileName->Release();
	MemFreeNN(ent);
}

Media::Playlist::Playlist(Text::CStringNN sourceName, NN<Parser::ParserList> parsers) : IO::ParsedObject(sourceName)
{
	this->parsers = parsers;
	this->player = 0;
	this->currFile = 0;
	this->playing = false;
}

Media::Playlist::~Playlist()
{
	if (this->player)
	{
		this->player->LoadMedia(0);
		this->player->SetEndHandler(0, 0);
		this->player = 0;
	}
	this->currFile.Delete();
	this->entries.FreeAll(FreeEntry);
}

IO::ParserType Media::Playlist::GetParserType() const
{
	return IO::ParserType::Playlist;
}

Bool Media::Playlist::AddFile(Text::CStringNN fileName)
{
	NN<Media::MediaFile> file;
	{
		IO::StmData::FileData fd(fileName, false);
		if (!Optional<Media::MediaFile>::ConvertFrom(this->parsers->ParseFileType(fd, IO::ParserType::MediaFile)).SetTo(file))
		{
			return false;
		}
	}


	Media::ChapterInfo *chap = file->GetChapterInfo();
	NN<Text::String> artist;
	UOSInt i;
	UOSInt j;
	Data::Duration nextTime;
	NN<PlaylistEntry> ent;
	if (chap)
	{
		i = 0;
		j = chap->GetChapterCnt();
		while (i < j)
		{
			if (i >= j - 1)
			{
				nextTime = Data::Duration::Infinity();
			}
			else
			{
				nextTime = (Int32)chap->GetChapterTime(i + 1);
			}

			ent = MemAllocNN(PlaylistEntry);
			ent->fileName = Text::String::New(fileName.v, fileName.leng);
			ent->title = Text::String::OrEmpty(chap->GetChapterName(i))->Clone();
			if (chap->GetChapterArtist(i).SetTo(artist))
			{
				ent->artist = artist->Clone();
			}
			else
			{
				ent->artist = 0;
			}
			ent->timeStart = chap->GetChapterTime(i);
			ent->timeEnd = nextTime;
			this->entries.Add(ent);

			i++;
		}
	}
	else
	{
		ent = MemAllocNN(PlaylistEntry);
		ent->fileName = Text::String::New(fileName.v, fileName.leng);
		i = Text::StrLastIndexOfCharC(fileName.v, fileName.leng, IO::Path::PATH_SEPERATOR);
		ent->title = Text::String::New(&fileName.v[i + 1], fileName.leng - i - 1);
		ent->artist = 0;
		ent->timeStart = 0;
		ent->timeEnd = -1;
		this->entries.Add(ent);
	}
	file.Delete();
	return true;
}

Bool Media::Playlist::RemoveEntry(UOSInt index)
{
	NN<PlaylistEntry> ent;
	if (!this->entries.RemoveAt(index).SetTo(ent))
		return false;
	FreeEntry(ent);
	return true;
}

Bool Media::Playlist::AppendPlaylist(NN<Media::Playlist> playlist)
{
	UOSInt i;
	UOSInt j;
	NN<PlaylistEntry> ent;
	NN<PlaylistEntry> plent;
	NN<Text::String> s;
	i = 0;
	j = playlist->entries.GetCount();
	while (i < j)
	{
		plent = playlist->entries.GetItemNoCheck(i);
		ent = MemAllocNN(PlaylistEntry);
		ent->fileName = plent->fileName->Clone();
		ent->title = plent->title->Clone();
		if (plent->artist.SetTo(s))
		{
			ent->artist = s->Clone().Ptr();
		}
		else
		{
			ent->artist = 0;
		}
		ent->timeStart = plent->timeStart;
		ent->timeEnd = plent->timeEnd;
		this->entries.Add(ent);
		i++;
	}
	return true;
}

void Media::Playlist::ClearFiles()
{
	this->entries.FreeAll(FreeEntry);
}

UOSInt Media::Playlist::GetCount() const
{
	return this->entries.GetCount();
}

Optional<Text::String> Media::Playlist::GetTitle(UOSInt index) const
{
	NN<PlaylistEntry> ent;
	if (!this->entries.GetItem(index).SetTo(ent))
		return 0;
	return ent->title;
}

Optional<Text::String> Media::Playlist::GetArtist(UOSInt index) const
{
	NN<PlaylistEntry> ent;
	if (!this->entries.GetItem(index).SetTo(ent))
		return 0;
	return ent->artist;
}

Optional<Text::String> Media::Playlist::GetFileName(UOSInt index) const
{
	NN<PlaylistEntry> ent;
	if (!this->entries.GetItem(index).SetTo(ent))
		return 0;
	return ent->fileName;
}

Data::Duration Media::Playlist::GetTimeStart(UOSInt index) const
{
	NN<PlaylistEntry> ent;
	if (!this->entries.GetItem(index).SetTo(ent))
		return 0;
	return ent->timeStart;
}

Data::Duration Media::Playlist::GetTimeEnd(UOSInt index) const
{
	NN<PlaylistEntry> ent;
	if (!this->entries.GetItem(index).SetTo(ent))
		return 0;
	return ent->timeEnd;
}

void Media::Playlist::SetPlayer(Media::IMediaPlayer *player)
{
	if (this->player)
	{
		this->player->SetEndHandler(0, 0);
		this->player = 0;
	}
	this->player = player;
	if (this->player)
	{
		this->player->SetEndHandler(OnPBEnd, this);
	}
}

Bool Media::Playlist::OpenItem(UOSInt index)
{
	NN<PlaylistEntry> ent;
	if (!this->entries.GetItem(index).SetTo(ent) || this->player == 0)
		return false;

	this->player->LoadMedia(0);
	this->currFile.Delete();
	{
		IO::StmData::FileData fd(ent->fileName, false);
		this->currFile = Optional<Media::MediaFile>::ConvertFrom(this->parsers->ParseFileType(fd, IO::ParserType::MediaFile));
	}

	NN<Media::MediaFile> currFile;
	if (!this->currFile.SetTo(currFile))
		return false;
	
	currFile->TrimFile((UInt32)ent->timeStart.GetTotalMS(), (Int32)ent->timeEnd.GetTotalMS());

	if (!this->player->LoadMedia(this->currFile))
		return false;
	if (this->playing)
	{
		return this->player->StartPlayback();
	}
	else
	{
		return true;
	}
}

Bool Media::Playlist::IsPlaying()
{
	return this->playing;
}

Bool Media::Playlist::StartPlayback()
{
	if (this->currFile.IsNull())
	{
		OnPBEnd(this);
	}

	if (this->player)
	{
		this->playing = this->player->StartPlayback();
		return this->playing;
	}
	return false;
}

Bool Media::Playlist::StopPlayback()
{
	this->playing = false;
	if (this->player)
	{
		this->player->StopPlayback();
		return true;
	}
	return false;
}

Bool Media::Playlist::PrevChapter()
{
	if (this->player == 0)
	{
		return false;
	}
	OnPBEnd(this);
	return true;
}

Bool Media::Playlist::NextChapter()
{
	if (this->player == 0)
	{
		return false;
	}
	OnPBEnd(this);
	return true;
}

Data::Duration Media::Playlist::GetCurrTime()
{
	if (this->player == 0)
	{
		return 0;
	}
	return this->player->GetCurrTime();
}
