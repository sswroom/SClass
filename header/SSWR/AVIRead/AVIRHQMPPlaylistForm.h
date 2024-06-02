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
			NN<UI::GUIPanel> pnlCtrl;
			NN<UI::GUIListBox> lbPlaylist;
			NN<UI::GUIButton> btnAdd;
			NN<UI::GUIButton> btnAddDir;
			NN<UI::GUIButton> btnClear;
			NN<UI::GUIButton> btnOK;
			NN<UI::GUIButton> btnCancel;

			NN<SSWR::AVIRead::AVIRCore> core;
			Media::Playlist *playlist;
		private:
			static void __stdcall OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files);
			static void __stdcall OnAddClicked(AnyType userObj);
			static void __stdcall OnAddDirClicked(AnyType userObj);
			static void __stdcall OnClearClicked(AnyType userObj);
			static void __stdcall OnOKClicked(AnyType userObj);
			static void __stdcall OnCancelClicked(AnyType userObj);
			Bool AddFolder(UnsafeArray<UTF8Char> folderBuff, UnsafeArray<UTF8Char> folderBuffEnd);
			void UpdatePlaylist();
		public:
			AVIRHQMPPlaylistForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Media::Playlist *playlist);
			virtual ~AVIRHQMPPlaylistForm();

			virtual void OnMonitorChanged();
			Media::Playlist *GetPlaylist();
		};
	}
}
#endif
