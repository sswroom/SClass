#include "Stdafx.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Math/Math.h"
#include "Media/IMediaPlayer.h"
#include "SSWR/AVIRead/AVIRHQMPPlaylistForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/FileDialog.h"
#include "UI/FolderDialog.h"

void __stdcall SSWR::AVIRead::AVIRHQMPPlaylistForm::OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRHQMPPlaylistForm *me = (SSWR::AVIRead::AVIRHQMPPlaylistForm*)userObj;
	Bool changed = false;
	UOSInt i;
	i = 0;
	while (i < nFiles)
	{
		if (me->playlist->AddFile(files[i]->ToCString()))
		{
			changed = true;
		}

		i++;
	}
	if (changed)
	{
		me->UpdatePlaylist();
	}
}

void __stdcall SSWR::AVIRead::AVIRHQMPPlaylistForm::OnAddClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHQMPPlaylistForm *me = (SSWR::AVIRead::AVIRHQMPPlaylistForm*)userObj;
	Bool changed = false;
	UOSInt i;
	UOSInt j;
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"HQMPPlaylist", false);
	dlg.SetAllowMultiSel(true);
	me->core->GetParserList()->PrepareSelector(dlg, IO::ParserType::MediaFile);
	if (dlg.ShowDialog(me->GetHandle()) == UI::GUIForm::DR_OK)
	{
		i = 0;
		j = dlg.GetFileNameCount();
		while (i < j)
		{
			const UTF8Char *fileName = dlg.GetFileNames(i);
			if (me->playlist->AddFile({fileName, Text::StrCharCnt(fileName)}))
			{
				changed = true;
			}

			i++;
		}
		if (changed)
		{
			me->UpdatePlaylist();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRHQMPPlaylistForm::OnAddDirClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHQMPPlaylistForm *me = (SSWR::AVIRead::AVIRHQMPPlaylistForm*)userObj;
	UTF8Char sbuff[512];
	UI::FolderDialog dlg(L"SSWR", L"AVIRead", L"HQMPPlaylistDir");
	if (dlg.ShowDialog(me->GetHandle()) == UI::GUIForm::DR_OK)
	{
		NotNullPtr<Text::String> folder = dlg.GetFolder();
		if (me->AddFolder(sbuff, folder->ConcatTo(sbuff)))
		{
			me->UpdatePlaylist();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRHQMPPlaylistForm::OnClearClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHQMPPlaylistForm *me = (SSWR::AVIRead::AVIRHQMPPlaylistForm*)userObj;
	me->playlist->ClearFiles();
	me->UpdatePlaylist();
}

void __stdcall SSWR::AVIRead::AVIRHQMPPlaylistForm::OnOKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHQMPPlaylistForm *me = (SSWR::AVIRead::AVIRHQMPPlaylistForm*)userObj;
	if (me->playlist->GetCount() > 0)
	{
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
}

void __stdcall SSWR::AVIRead::AVIRHQMPPlaylistForm::OnCancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRHQMPPlaylistForm *me = (SSWR::AVIRead::AVIRHQMPPlaylistForm*)userObj;
	SDEL_CLASS(me->playlist);
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

Bool SSWR::AVIRead::AVIRHQMPPlaylistForm::AddFolder(UTF8Char *folderBuff, UTF8Char *folderBuffEnd)
{
	if (folderBuffEnd[-1] != IO::Path::PATH_SEPERATOR)
	{
		*folderBuffEnd++ = IO::Path::PATH_SEPERATOR;
	}
	Bool changed = false;
	UTF8Char *sptr;
	IO::Path::PathType pt;
	sptr = Text::StrConcatC(folderBuffEnd, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	IO::Path::FindFileSession *sess = IO::Path::FindFile(CSTRP(folderBuff, sptr));
	if (sess)
	{
		while ((sptr = IO::Path::FindNextFile(folderBuffEnd, sess, 0, &pt, 0)) != 0)
		{
			if (pt == IO::Path::PathType::File)
			{
				Bool toSkip = false;
				if (Text::StrEndsWithICaseC(folderBuffEnd, (UOSInt)(sptr - folderBuffEnd), UTF8STRC(".vob")))
				{
					if (sptr[-6] == '_' && sptr[-5] >= '2' && sptr[-5] <= '9')
					{
						toSkip = true;
					}
				}
				
				if (!toSkip)
				{
					if (this->playlist->AddFile({folderBuff, (UOSInt)(sptr - folderBuff)}))
					{
						changed = true;
					}
				}
			}
			else if (pt == IO::Path::PathType::Directory)
			{
				if (folderBuffEnd[0] == '.' && folderBuffEnd[1] == 0)
				{
				}
				else if (folderBuffEnd[0] == '.' && folderBuffEnd[1] == '.' && folderBuffEnd[2] == 0)
				{
				}
				else
				{
					AddFolder(folderBuff, sptr);
				}
			}
		}
		IO::Path::FindFileClose(sess);
	}
	return changed;
}

void SSWR::AVIRead::AVIRHQMPPlaylistForm::UpdatePlaylist()
{
	UOSInt i;
	UOSInt j;
	this->lbPlaylist->ClearItems();
	i = 0;
	j = this->playlist->GetCount();
	while (i < j)
	{
		Text::String *title = this->playlist->GetTitle(i);
		this->lbPlaylist->AddItem(Text::String::OrEmpty(title), (void*)i);
		i++;
	}
}

SSWR::AVIRead::AVIRHQMPPlaylistForm::AVIRHQMPPlaylistForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Media::Playlist *playlist) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Create Playlist"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASSNN(this->pnlCtrl, UI::GUIPanel(ui, *this));
	this->pnlCtrl->SetRect(0, 0, 100, 56, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbPlaylist, UI::GUIListBox(ui, *this, false));
	this->lbPlaylist->SetRect(0, 0, 160, 100, false);
	this->lbPlaylist->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->btnAdd, UI::GUIButton(ui, this->pnlCtrl, CSTR("&Add")));
	this->btnAdd->SetRect(16, 16, 75, 23, false);
	this->btnAdd->HandleButtonClick(OnAddClicked, this);
	NEW_CLASS(this->btnAddDir, UI::GUIButton(ui, this->pnlCtrl, CSTR("&Add Folder")));
	this->btnAddDir->SetRect(96, 16, 75, 23, false);
	this->btnAddDir->HandleButtonClick(OnAddDirClicked, this);
	NEW_CLASS(this->btnClear, UI::GUIButton(ui, this->pnlCtrl, CSTR("Clea&r")));
	this->btnClear->SetRect(176, 16, 75, 23, false);
	this->btnClear->HandleButtonClick(OnClearClicked, this);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this->pnlCtrl, CSTR("&OK")));
	this->btnOK->SetRect(256, 16, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this->pnlCtrl, CSTR("&Cancel")));
	this->btnCancel->SetRect(336, 16, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);

	this->HandleDropFiles(OnFileDrop, this);

	NEW_CLASS(this->playlist, Media::Playlist(CSTR("HQMP"), this->core->GetParserList()));
	if (playlist)
	{
		this->playlist->AppendPlaylist(playlist);
	}
	this->UpdatePlaylist();

}

SSWR::AVIRead::AVIRHQMPPlaylistForm::~AVIRHQMPPlaylistForm()
{
}


void SSWR::AVIRead::AVIRHQMPPlaylistForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Media::Playlist *SSWR::AVIRead::AVIRHQMPPlaylistForm::GetPlaylist()
{
	return this->playlist;
}
