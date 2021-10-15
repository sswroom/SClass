#ifndef _SM_MEDIA_PLAYLIST
#define _SM_MEDIA_PLAYLIST
#include "IO/ParsedObject.h"
#include "Media/IMediaPlayer.h"
#include "Media/MediaFile.h"
#include "Parser/ParserList.h"

namespace Media
{
	class Playlist : public IO::ParsedObject, public Media::IPBControl
	{
	private:
		typedef struct
		{
			const UTF8Char *title;
			const UTF8Char *artist;
			const UTF8Char *fileName;
			UInt32 timeStart;
			Int32 timeEnd;
		} PlaylistEntry;

		Data::ArrayList<PlaylistEntry*> *entries;
		Parser::ParserList *parsers;
		Media::IMediaPlayer *player;
		Media::MediaFile *currFile;
		Bool playing;

		static void __stdcall OnPBEnd(void *userObj);
		void FreeEntry(PlaylistEntry* ent);
	public:
		Playlist(const UTF8Char *sourceName, Parser::ParserList *parsers);
		virtual ~Playlist();

		virtual IO::ParserType GetParserType();
		Bool AddFile(const UTF8Char *fileName);
		Bool RemoveEntry(UOSInt index);
		Bool AppendPlaylist(Media::Playlist *playlist);
		void ClearFiles();

		UOSInt GetCount();
		const UTF8Char *GetTitle(UOSInt index);
		const UTF8Char *GetArtist(UOSInt index);
		const UTF8Char *GetFileName(UOSInt index);
		UInt32 GetTimeStart(UOSInt index);
		Int32 GetTimeEnd(UOSInt index);

		void SetPlayer(Media::IMediaPlayer *player);
		Bool OpenItem(UOSInt index);
		
		virtual Bool IsPlaying();
		virtual Bool StartPlayback();
		virtual Bool StopPlayback();
		virtual Bool PrevChapter();
		virtual Bool NextChapter();
		virtual UInt32 GetCurrTime();
	};
}
#endif
