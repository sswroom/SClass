#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/RandomOS.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Media/Playlist.h"
#include "Text/MyString.h"

void __stdcall Media::Playlist::OnPBEnd(void *userObj)
{
	Media::Playlist *me = (Media::Playlist*)userObj;
	Data::RandomOS random;
	UOSInt i = (UInt32)random.NextInt15() % me->entries->GetCount();
	me->OpenItem(i);
}

void Media::Playlist::FreeEntry(PlaylistEntry* ent)
{
	SDEL_TEXT(ent->title);
	SDEL_TEXT(ent->artist);
	SDEL_TEXT(ent->fileName);
	MemFree(ent);
}

Media::Playlist::Playlist(const UTF8Char *sourceName, Parser::ParserList *parsers) : IO::ParsedObject(sourceName)
{
	this->parsers = parsers;
	this->player = 0;
	this->currFile = 0;
	this->playing = false;
	NEW_CLASS(this->entries, Data::ArrayList<PlaylistEntry*>());
}

Media::Playlist::~Playlist()
{
	if (this->player)
	{
		this->player->LoadMedia(0);
		this->player->SetEndHandler(0, 0);
		this->player = 0;
	}
	SDEL_CLASS(this->currFile);
	UOSInt i;
	i = this->entries->GetCount();
	while (i-- > 0)
	{
		FreeEntry(this->entries->GetItem(i));
	}
	DEL_CLASS(this->entries);
}

IO::ParsedObject::ParserType Media::Playlist::GetParserType()
{
	return IO::ParsedObject::PT_PLAYLIST;
}

Bool Media::Playlist::AddFile(const UTF8Char *fileName)
{
	Media::MediaFile *file;
	IO::StmData::FileData *fd;
	NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
	file = (Media::MediaFile*)this->parsers->ParseFileType(fd, IO::ParsedObject::PT_VIDEO_PARSER);
	DEL_CLASS(fd);
	if (file == 0)
	{
		return false;
	}


	Media::ChapterInfo *chap = file->GetChapterInfo();
	const UTF8Char *artist;
	UOSInt i;
	UOSInt j;
	Int32 nextTime;
	PlaylistEntry *ent;
	if (chap)
	{
		i = 0;
		j = chap->GetChapterCnt();
		while (i < j)
		{
			if (i >= j - 1)
			{
				nextTime = -1;
			}
			else
			{
				nextTime = (Int32)chap->GetChapterTime(i + 1);
			}

			ent = MemAlloc(PlaylistEntry, 1);
			ent->fileName = Text::StrCopyNew(fileName);
			ent->title = Text::StrCopyNew(chap->GetChapterName(i));
			artist = chap->GetChapterArtist(i);
			if (artist)
			{
				ent->artist = Text::StrCopyNew(artist);
			}
			else
			{
				ent->artist = 0;
			}
			ent->timeStart = chap->GetChapterTime(i);
			ent->timeEnd = nextTime;
			this->entries->Add(ent);

			i++;
		}
	}
	else
	{
		OSInt si;
		ent = MemAlloc(PlaylistEntry, 1);
		ent->fileName = Text::StrCopyNew(fileName);
		si = Text::StrLastIndexOf(fileName, IO::Path::PATH_SEPERATOR);
		ent->title = Text::StrCopyNew(&fileName[si + 1]);
		ent->artist = 0;
		ent->timeStart = 0;
		ent->timeEnd = -1;
		this->entries->Add(ent);
	}
	DEL_CLASS(file);
	return true;
}

Bool Media::Playlist::RemoveEntry(UOSInt index)
{
	PlaylistEntry *ent = this->entries->RemoveAt(index);
	if (ent == 0)
		return false;
	FreeEntry(ent);
	return true;
}

Bool Media::Playlist::AppendPlaylist(Media::Playlist *playlist)
{
	UOSInt i;
	UOSInt j;
	PlaylistEntry *ent;
	PlaylistEntry *plent;
	i = 0;
	j = playlist->entries->GetCount();
	while (i < j)
	{
		plent = playlist->entries->GetItem(i);
		ent = MemAlloc(PlaylistEntry, 1);
		ent->fileName = Text::StrCopyNew(plent->fileName);
		ent->title = Text::StrCopyNew(plent->title);
		if (plent->artist)
		{
			ent->artist = Text::StrCopyNew(plent->artist);
		}
		else
		{
			ent->artist = 0;
		}
		ent->timeStart = plent->timeStart;
		ent->timeEnd = plent->timeEnd;
		this->entries->Add(ent);
		i++;
	}
	return true;
}

void Media::Playlist::ClearFiles()
{
	UOSInt i = this->entries->GetCount();
	PlaylistEntry *ent;
	while (i-- > 0)
	{
		ent = this->entries->RemoveAt(i);
		FreeEntry(ent);
	}
	this->entries->Clear();
}

UOSInt Media::Playlist::GetCount()
{
	return this->entries->GetCount();
}

const UTF8Char *Media::Playlist::GetTitle(UOSInt index)
{
	PlaylistEntry *ent = this->entries->GetItem(index);
	if (ent == 0)
		return 0;
	return ent->title;
}

const UTF8Char *Media::Playlist::GetArtist(UOSInt index)
{
	PlaylistEntry *ent = this->entries->GetItem(index);
	if (ent == 0)
		return 0;
	return ent->artist;
}

const UTF8Char *Media::Playlist::GetFileName(UOSInt index)
{
	PlaylistEntry *ent = this->entries->GetItem(index);
	if (ent == 0)
		return 0;
	return ent->fileName;
}

UInt32 Media::Playlist::GetTimeStart(UOSInt index)
{
	PlaylistEntry *ent = this->entries->GetItem(index);
	if (ent == 0)
		return 0;
	return ent->timeStart;
}

Int32 Media::Playlist::GetTimeEnd(UOSInt index)
{
	PlaylistEntry *ent = this->entries->GetItem(index);
	if (ent == 0)
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
	PlaylistEntry *ent = this->entries->GetItem(index);
	if (ent == 0 || this->player == 0)
		return false;

	this->player->LoadMedia(0);
	SDEL_CLASS(this->currFile);
	IO::StmData::FileData *fd;
	NEW_CLASS(fd, IO::StmData::FileData(ent->fileName, false));
	this->currFile = (Media::MediaFile*)this->parsers->ParseFileType(fd, IO::ParsedObject::PT_VIDEO_PARSER);
	DEL_CLASS(fd);

	if (this->currFile == 0)
		return false;
	
	this->currFile->TrimFile(ent->timeStart, ent->timeEnd);

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
	if (this->currFile == 0)
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

UInt32 Media::Playlist::GetCurrTime()
{
	if (this->player == 0)
	{
		return 0;
	}
	return this->player->GetCurrTime();
}
