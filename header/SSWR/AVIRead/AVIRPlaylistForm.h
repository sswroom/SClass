#ifndef _SM_SSWR_AVIREAD_AVIRPLAYLISTFORM
#define _SM_SSWR_AVIREAD_AVIRPLAYLISTFORM
#include "Media/IMediaPlayer.h"
#include "Media/MediaPlayer.h"
#include "Media/Playlist.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIHSplitter.h"
#include "UI/GUIListView.h"
#include "UI/GUIPanel.h"
#include "UI/GUIPopupMenu.h"
#include "UI/GUIVideoBoxDD.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRPlaylistForm : public UI::GUIForm 
		{
		private:
			NN<UI::GUIListView> lvPlaylist;
			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUIHSplitter> hsplit;
			UI::GUIVideoBoxDD *vbdMain;
			UI::GUIMainMenu *mnu;
			NN<UI::GUIButton> btnStart;
			NN<UI::GUIButton> btnEnd;
			NN<UI::GUIButton> btnFS;

			NN<SSWR::AVIRead::AVIRCore> core;
			NN<Media::ColorManagerSess> colorSess;
			Media::Playlist *playlist;
			Media::MediaPlayer *player;
			const WChar *currFileName;
		private:
			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnPlaylistDblClk(AnyType userObj, UOSInt itemIndex);
			static void __stdcall OnStartClicked(AnyType userObj);
			static void __stdcall OnEndClicked(AnyType userObj);
			static void __stdcall OnFSClicked(AnyType userObj);
			void UpdatePlaylist();
		public:
			AVIRPlaylistForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Media::Playlist *playlist);
			virtual ~AVIRPlaylistForm();
		
			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	}
}
#endif
