#ifndef _SM_SSWR_AVIREAD_AVIRHQMPPLAYLISTFORM
#define _SM_SSWR_AVIREAD_AVIRHQMPPLAYLISTFORM
#include "Media/Playlist.h"
#include "SSWR/AVIRead/AVIRCore.h"
#include "UI/GUIButton.h"
#include "UI/GUIForm.h"
#include "UI/GUIListBox.h"
#include "UI/GUIPanel.h"

namespace SSWR
{
	namespace AVIRead
	{
		class AVIRHQMPPlaylistForm : public UI::GUIForm 
		{
		private:
			NotNullPtr<UI::GUIPanel> pnlCtrl;
			NotNullPtr<UI::GUIListBox> lbPlaylist;
			NotNullPtr<UI::GUIButton> btnAdd;
			NotNullPtr<UI::GUIButton> btnAddDir;
			NotNullPtr<UI::GUIButton> btnClear;
			NotNullPtr<UI::GUIButton> btnOK;
			NotNullPtr<UI::GUIButton> btnCancel;

			NotNullPtr<SSWR::AVIRead::AVIRCore> core;
			Media::Playlist *playlist;
		private:
			static void __stdcall OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles);
			static void __stdcall OnAddClicked(void *userObj);
			static void __stdcall OnAddDirClicked(void *userObj);
			static void __stdcall OnClearClicked(void *userObj);
			static void __stdcall OnOKClicked(void *userObj);
			static void __stdcall OnCancelClicked(void *userObj);
			Bool AddFolder(UTF8Char *folderBuff, UTF8Char *folderBuffEnd);
			void UpdatePlaylist();
		public:
			AVIRHQMPPlaylistForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Media::Playlist *playlist);
			virtual ~AVIRHQMPPlaylistForm();

			virtual void OnMonitorChanged();
			Media::Playlist *GetPlaylist();
		};
	}
}
#endif
