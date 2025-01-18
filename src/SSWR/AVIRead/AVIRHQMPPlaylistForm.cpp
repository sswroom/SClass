#include "Stdafx.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Math/Math.h"
#include "Media/IMediaPlayer.h"
#include "SSWR/AVIRead/AVIRHQMPPlaylistForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUIFileDialog.h"
#include "UI/GUIFolderDialog.h"

void __stdcall SSWR::AVIRead::AVIRHQMPPlaylistForm::OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRHQMPPlaylistForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHQMPPlaylistForm>();
	Bool changed = false;
	UOSInt i;
	UOSInt nFiles = files.GetCount();
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

void __stdcall SSWR::AVIRead::AVIRHQMPPlaylistForm::OnAddClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHQMPPlaylistForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHQMPPlaylistForm>();
	Bool changed = false;
	UOSInt i;
	UOSInt j;
	NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"HQMPPlaylist", false);
	dlg->SetAllowMultiSel(true);
	me->core->GetParserList()->PrepareSelector(dlg, IO::ParserType::MediaFile);
	if (dlg->ShowDialog(me->GetHandle()) == UI::GUIForm::DR_OK)
	{
		i = 0;
		j = dlg->GetFileNameCount();
		while (i < j)
		{
			NN<Text::String> fileName;
			if (dlg->GetFileNames(i).SetTo(fileName) && me->playlist->AddFile(fileName->ToCString()))
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
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRHQMPPlaylistForm::OnAddDirClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHQMPPlaylistForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHQMPPlaylistForm>();
	UTF8Char sbuff[512];
	NN<UI::GUIFolderDialog> dlg = me->ui->NewFolderDialog();
	if (dlg->ShowDialog(me->GetHandle()) == UI::GUIForm::DR_OK)
	{
		NN<Text::String> folder = dlg->GetFolder();
		if (me->AddFolder(sbuff, folder->ConcatTo(sbuff)))
		{
			me->UpdatePlaylist();
		}
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRHQMPPlaylistForm::OnClearClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHQMPPlaylistForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHQMPPlaylistForm>();
	me->playlist->ClearFiles();
	me->UpdatePlaylist();
}

void __stdcall SSWR::AVIRead::AVIRHQMPPlaylistForm::OnOKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHQMPPlaylistForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHQMPPlaylistForm>();
	if (me->playlist->GetCount() > 0)
	{
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
}

void __stdcall SSWR::AVIRead::AVIRHQMPPlaylistForm::OnCancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRHQMPPlaylistForm> me = userObj.GetNN<SSWR::AVIRead::AVIRHQMPPlaylistForm>();
	me->playlist.Delete();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

Bool SSWR::AVIRead::AVIRHQMPPlaylistForm::AddFolder(UnsafeArray<UTF8Char> folderBuff, UnsafeArray<UTF8Char> folderBuffEnd)
{
	if (folderBuffEnd[-1] != IO::Path::PATH_SEPERATOR)
	{
		*folderBuffEnd++ = IO::Path::PATH_SEPERATOR;
	}
	Bool changed = false;
	UnsafeArray<UTF8Char> sptr;
	IO::Path::PathType pt;
	sptr = Text::StrConcatC(folderBuffEnd, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	NN<IO::Path::FindFileSession> sess;
	if (IO::Path::FindFile(CSTRP(folderBuff, sptr)).SetTo(sess))
	{
		while (IO::Path::FindNextFile(folderBuffEnd, sess, 0, pt, 0).SetTo(sptr))
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
		Optional<Text::String> title = this->playlist->GetTitle(i);
		this->lbPlaylist->AddItem(Text::String::OrEmpty(title), (void*)i);
		i++;
	}
}

SSWR::AVIRead::AVIRHQMPPlaylistForm::AVIRHQMPPlaylistForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Optional<Media::Playlist> playlist) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Create Playlist"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlCtrl = ui->NewPanel(*this);
	this->pnlCtrl->SetRect(0, 0, 100, 56, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lbPlaylist = ui->NewListBox(*this, false);
	this->lbPlaylist->SetRect(0, 0, 160, 100, false);
	this->lbPlaylist->SetDockType(UI::GUIControl::DOCK_FILL);
	this->btnAdd = ui->NewButton(this->pnlCtrl, CSTR("&Add"));
	this->btnAdd->SetRect(16, 16, 75, 23, false);
	this->btnAdd->HandleButtonClick(OnAddClicked, this);
	this->btnAddDir = ui->NewButton(this->pnlCtrl, CSTR("&Add Folder"));
	this->btnAddDir->SetRect(96, 16, 75, 23, false);
	this->btnAddDir->HandleButtonClick(OnAddDirClicked, this);
	this->btnClear = ui->NewButton(this->pnlCtrl, CSTR("Clea&r"));
	this->btnClear->SetRect(176, 16, 75, 23, false);
	this->btnClear->HandleButtonClick(OnClearClicked, this);
	this->btnOK = ui->NewButton(this->pnlCtrl, CSTR("&OK"));
	this->btnOK->SetRect(256, 16, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->btnCancel = ui->NewButton(this->pnlCtrl, CSTR("&Cancel"));
	this->btnCancel->SetRect(336, 16, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);

	this->HandleDropFiles(OnFileDrop, this);

	NEW_CLASSNN(this->playlist, Media::Playlist(CSTR("HQMP"), this->core->GetParserList()));
	NN<Media::Playlist> nnplaylist;
	if (playlist.SetTo(nnplaylist))
	{
		this->playlist->AppendPlaylist(nnplaylist);
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

NN<Media::Playlist> SSWR::AVIRead::AVIRHQMPPlaylistForm::GetPlaylist()
{
	return this->playlist;
}
