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
			UI::GUIListView *lvPlaylist;
			UI::GUIPanel *pnlCtrl;
			UI::GUIHSplitter *hsplit;
			UI::GUIVideoBoxDD *vbdMain;
			UI::GUIMainMenu *mnu;
			UI::GUIButton *btnStart;
			UI::GUIButton *btnEnd;
			UI::GUIButton *btnFS;

			SSWR::AVIRead::AVIRCore *core;
			Media::ColorManagerSess *colorSess;
			Media::Playlist *playlist;
			Media::MediaPlayer *player;
			const WChar *currFileName;
		private:
			static void __stdcall OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);
			static void __stdcall OnPlaylistDblClk(void *userObj, UOSInt itemIndex);
			static void __stdcall OnStartClicked(void *userObj);
			static void __stdcall OnEndClicked(void *userObj);
			static void __stdcall OnFSClicked(void *userObj);
			void UpdatePlaylist();
		public:
			AVIRPlaylistForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Media::Playlist *playlist);
			virtual ~AVIRPlaylistForm();
		
			virtual void EventMenuClicked(UInt16 cmdId);
			virtual void OnMonitorChanged();
		};
	};
};
#endif
