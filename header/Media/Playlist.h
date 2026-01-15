#ifndef _SM_MEDIA_PLAYLIST
#define _SM_MEDIA_PLAYLIST
#include "IO/ParsedObject.h"
#include "Media/MediaFile.h"
#include "Media/MediaPlayer.h"
#include "Parser/ParserList.h"

namespace Media
{
	class Playlist : public IO::ParsedObject, public Media::PBControl
	{
	private:
		typedef struct
		{
			NN<Text::String> title;
			Optional<Text::String> artist;
			NN<Text::String> fileName;
			Data::Duration timeStart;
			Data::Duration timeEnd;
		} PlaylistEntry;

		Data::ArrayListNN<PlaylistEntry> entries;
		NN<Parser::ParserList> parsers;
		Optional<Media::MediaPlayer> player;
		Optional<Media::MediaFile> currFile;
		Bool playing;

		static void __stdcall OnPBEnd(AnyType userObj);
		static void __stdcall FreeEntry(NN<PlaylistEntry> ent);
	public:
		Playlist(Text::CStringNN sourceName, NN<Parser::ParserList> parsers);
		virtual ~Playlist();

		virtual IO::ParserType GetParserType() const;
		Bool AddFile(Text::CStringNN fileName);
		Bool RemoveEntry(UIntOS index);
		Bool AppendPlaylist(NN<Media::Playlist> playlist);
		void ClearFiles();

		UIntOS GetCount() const;
		Optional<Text::String> GetTitle(UIntOS index) const;
		Optional<Text::String> GetArtist(UIntOS index) const;
		Optional<Text::String> GetFileName(UIntOS index) const;
		Data::Duration GetTimeStart(UIntOS index) const;
		Data::Duration GetTimeEnd(UIntOS index) const;

		void SetPlayer(Optional<Media::MediaPlayer> player);
		Bool OpenItem(UIntOS index);
		
		virtual Bool IsPlaying();
		virtual Bool StartPlayback();
		virtual Bool StopPlayback();
		virtual Bool PrevChapter();
		virtual Bool NextChapter();
		virtual Data::Duration GetCurrTime();
	};
}
#endif
