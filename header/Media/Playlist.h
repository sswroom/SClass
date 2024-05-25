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
			NN<Text::String> title;
			Text::String *artist;
			NN<Text::String> fileName;
			Data::Duration timeStart;
			Data::Duration timeEnd;
		} PlaylistEntry;

		Data::ArrayListNN<PlaylistEntry> entries;
		NN<Parser::ParserList> parsers;
		Media::IMediaPlayer *player;
		Optional<Media::MediaFile> currFile;
		Bool playing;

		static void __stdcall OnPBEnd(AnyType userObj);
		static void FreeEntry(NN<PlaylistEntry> ent);
	public:
		Playlist(Text::CStringNN sourceName, NN<Parser::ParserList> parsers);
		virtual ~Playlist();

		virtual IO::ParserType GetParserType() const;
		Bool AddFile(Text::CStringNN fileName);
		Bool RemoveEntry(UOSInt index);
		Bool AppendPlaylist(NN<Media::Playlist> playlist);
		void ClearFiles();

		UOSInt GetCount() const;
		Optional<Text::String> GetTitle(UOSInt index) const;
		Text::String *GetArtist(UOSInt index) const;
		Optional<Text::String> GetFileName(UOSInt index) const;
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
