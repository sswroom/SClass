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
			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NotNullPtr<Text::String>> files);
			static void __stdcall OnAddClicked(AnyType userObj);
			static void __stdcall OnAddDirClicked(AnyType userObj);
			static void __stdcall OnClearClicked(AnyType userObj);
			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
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
