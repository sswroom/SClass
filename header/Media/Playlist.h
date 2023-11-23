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
			NotNullPtr<Text::String> title;
			Text::String *artist;
			NotNullPtr<Text::String> fileName;
			Data::Duration timeStart;
			Data::Duration timeEnd;
		} PlaylistEntry;

		Data::ArrayList<PlaylistEntry*> entries;
		NotNullPtr<Parser::ParserList> parsers;
		Media::IMediaPlayer *player;
		Media::MediaFile *currFile;
		Bool playing;

		static void __stdcall OnPBEnd(void *userObj);
		void FreeEntry(PlaylistEntry* ent);
	public:
		Playlist(Text::CStringNN sourceName, NotNullPtr<Parser::ParserList> parsers);
		virtual ~Playlist();

		virtual IO::ParserType GetParserType() const;
		Bool AddFile(Text::CStringNN fileName);
		Bool RemoveEntry(UOSInt index);
		Bool AppendPlaylist(Media::Playlist *playlist);
		void ClearFiles();

		UOSInt GetCount() const;
		Text::String *GetTitle(UOSInt index) const;
		Text::String *GetArtist(UOSInt index) const;
		Text::String *GetFileName(UOSInt index) const;
		Data::Duration GetTimeStart(UOSInt index) const;
		Data::Duration GetTimeEnd(UOSInt index) const;

		void SetPlayer(Media::IMediaPlayer *player);
		Bool OpenItem(UOSInt index);
		
		virtual Bool IsPlaying();
		virtual Bool StartPlayback();
		virtual Bool StopPlayback();
		virtual Bool PrevChapter();
		virtual Bool NextChapter();
		virtual Data::Duration GetCurrTime();
	};
}
#endif
