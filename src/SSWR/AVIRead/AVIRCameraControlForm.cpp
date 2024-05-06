#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/GPSNMEA.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/StmData/MemoryDataRef.h"
#include "SSWR/AVIRead/AVIRGISForm.h"
#include "SSWR/AVIRead/AVIRCameraControlForm.h"
#include "Text/MyString.h"
#include "UI/GUIFileDialog.h"
#include "UI/GUIFolderDialog.h"

void __stdcall SSWR::AVIRead::AVIRCameraControlForm::OnDownloadClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCameraControlForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCameraControlForm>();
	Data::ArrayList<UOSInt> selIndices;
	me->lvFiles->GetSelectedIndices(&selIndices);
	if (selIndices.GetCount() <= 0)
	{
		return;
	}
	else if (selIndices.GetCount() == 1)
	{
		NN<IO::CameraControl::FileInfo> file = me->lvFiles->GetItem(selIndices.GetItem(0)).GetNN<IO::CameraControl::FileInfo>();
		NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"CameraControlFile", true);
		dlg->SetFileName(Text::CString(file->fileName2, file->fileNameLen));
		if (dlg->ShowDialog(me->GetHandle()))
		{
			Data::DateTime dt;
			Bool succ;
			{
				IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				succ = me->camera->GetFile(file, fs);
				if (file->fileTimeTicks)
				{
					dt.SetTicks(file->fileTimeTicks);
					fs.SetFileTimes(0, 0, &dt);
				}
			}
			if (!succ)
			{
				IO::Path::DeleteFile(dlg->GetFileName()->v);
				me->ui->ShowMsgOK(CSTR("Error in downloading the file"), CSTR("Camera Control"), me);
			}
		}
		dlg.Delete();
	}
	else
	{
		Data::DateTime dt;
		Text::StringBuilderUTF8 sb;
		NN<UI::GUIFolderDialog> dlg = me->ui->NewFolderDialog();
		if (dlg->ShowDialog(me->GetHandle()))
		{
			Bool succ = true;
			UOSInt i = 0;
			UOSInt j = selIndices.GetCount();
			while (i < j)
			{
				NN<IO::CameraControl::FileInfo> file = me->lvFiles->GetItem(selIndices.GetItem(i)).GetNN<IO::CameraControl::FileInfo>();
				sb.ClearStr();
				sb.Append(dlg->GetFolder());
				if (!sb.EndsWith(IO::Path::PATH_SEPERATOR))
				{
					sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
				}
				sb.AppendC(file->fileName2, file->fileNameLen);

				{
					IO::FileStream fs(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
					succ = me->camera->GetFile(file, fs);
					if (file->fileTimeTicks)
					{
						dt.SetTicks(file->fileTimeTicks);
						fs.SetFileTimes(0, 0, &dt);
					}
				}
				if (!succ)
				{
					IO::Path::DeleteFile(sb.ToString());
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Error in downloading "));
					sb.AppendC(file->fileName2, file->fileNameLen);
					sb.AppendC(UTF8STRC(", continue?"));
					if (!me->ui->ShowMsgYesNo(sb.ToCString(), CSTR("Camera Control"), me))
					{
						break;
					}
				}
				i++;
			}
			if (succ)
			{
				me->ui->ShowMsgOK(CSTR("Finish downloading selected files"), CSTR("Camera Control"), me);
			}
		}
		dlg.Delete();
	}
}

void __stdcall SSWR::AVIRead::AVIRCameraControlForm::OnFilesDblClick(AnyType userObj, UOSInt index)
{
	NN<SSWR::AVIRead::AVIRCameraControlForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCameraControlForm>();
	NN<IO::CameraControl::FileInfo> file;
	if (!me->lvFiles->GetItem(index).GetOpt<IO::CameraControl::FileInfo>().SetTo(file))
		return;
	if (file->fileType == IO::CameraControl::FT_IMAGE)
	{
		if (Text::StrEndsWithICaseC(file->fileName2, file->fileNameLen, UTF8STRC(".JPG")))
		{
			IO::MemoryStream mstm;
			if (me->camera->GetFile(file, mstm))
			{
				NN<IO::ParsedObject> pobj;
				UOSInt size;
				UInt8 *buff = mstm.GetBuff(size);
				IO::StmData::MemoryDataRef fd(buff, size);
				if (pobj.Set(me->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList)))
				{
					me->core->OpenObject(pobj);
				}
			}
		}
	}
	else if (file->fileType == IO::CameraControl::FT_GPSLOG)
	{
		if (Text::StrEndsWithICaseC(file->fileName2, file->fileNameLen, UTF8STRC(".LOG")))
		{
			IO::MemoryStream mstm;
			if (me->camera->GetFile(file, mstm))
			{
				mstm.SeekFromBeginning(0);
				NN<Map::GPSTrack> trk = IO::GPSNMEA::NMEA2Track(mstm, Text::CStringNN(file->fileName2, file->fileNameLen));
				SSWR::AVIRead::AVIRGISForm *frm = me->core->GetGISForm();
				if (frm)
				{
					frm->AddLayer(trk);
				}
				else
				{
					me->core->OpenObject(trk);
				}
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRCameraControlForm::OnFilesSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRCameraControlForm> me = userObj.GetNN<SSWR::AVIRead::AVIRCameraControlForm>();
	NN<IO::CameraControl::FileInfo> file;
	if (!me->lvFiles->GetSelectedItem().GetOpt<IO::CameraControl::FileInfo>().SetTo(file))
		return;
	NN<Media::ImageList> previewImg;
	if (me->previewMap.Get(Text::CStringNN(file->fileName2, file->fileNameLen)).SetTo(previewImg))
	{
		me->pbPreview->SetImage(Optional<Media::StaticImage>::ConvertFrom(previewImg->GetImage(0, 0)));
		return;
	}
	IO::MemoryStream mstm;
	if (me->camera->GetThumbnailFile(file, mstm))
	{
		UOSInt size;
		UInt8 *buff = mstm.GetBuff(size);
		IO::StmData::MemoryDataRef fd(buff, size);
		if (previewImg.Set((Media::ImageList*)me->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList)))
		{
			previewImg->ToStaticImage(0);
			me->previewMap.Put(Text::CStringNN(file->fileName2, file->fileNameLen), previewImg);
			me->pbPreview->SetImage(Optional<Media::StaticImage>::ConvertFrom(previewImg->GetImage(0, 0)));
		}
	}
}

SSWR::AVIRead::AVIRCameraControlForm::AVIRCameraControlForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, IO::CameraControl *camera) : UI::GUIForm(parent, 640, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Camera Control"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->camera = camera;

	this->lvInfo = ui->NewListView(*this, UI::ListViewStyle::Table, 2);
	this->lvInfo->SetRect(0, 0, 100, 96, false);
	this->lvInfo->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lvInfo->SetFullRowSelect(true);
	this->lvInfo->SetShowGrid(true);
	this->lvInfo->AddColumn(CSTR("Name"), 200);
	this->lvInfo->AddColumn(CSTR("Value"), 400);
	this->vspInfo = ui->NewVSplitter(*this, 3, false);
	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 160, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->pbPreview = ui->NewPictureBoxSimple(this->pnlControl, this->core->GetDrawEngine(), false);
	this->pbPreview->SetRect(0, 0, 160, 160, false);
	this->btnDownload = ui->NewButton(this->pnlControl, CSTR("Download"));
	this->btnDownload->SetRect(164, 4, 75, 23, false);
	this->btnDownload->HandleButtonClick(OnDownloadClicked, this);
	this->lvFiles = ui->NewListView(*this, UI::ListViewStyle::Table, 4);
	this->lvFiles->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvFiles->SetFullRowSelect(true);
	this->lvFiles->SetShowGrid(true);
	this->lvFiles->AddColumn(CSTR("File Name"), 200);
	this->lvFiles->AddColumn(CSTR("File Path"), 200);
	this->lvFiles->AddColumn(CSTR("File Size"), 120);
	this->lvFiles->AddColumn(CSTR("Modify Date"), 150);
	this->lvFiles->HandleDblClk(OnFilesDblClick, this);
	this->lvFiles->HandleSelChg(OnFilesSelChg, this);

	Data::ArrayListStringNN nameList;
	Data::ArrayListStringNN valueList;
	UOSInt i;
	UOSInt j;
	this->camera->GetInfoList(nameList, valueList);
	i = 0;
	j = nameList.GetCount();
	while (i < j)
	{
		this->lvInfo->AddItem(Text::String::OrEmpty(nameList.GetItem(i)), 0);
		this->lvInfo->SetSubItem(i, 1, Text::String::OrEmpty(valueList.GetItem(i)));
		i++;
	}
	this->camera->FreeInfoList(nameList, valueList);

	Data::ArrayListNN<IO::CameraControl::FileInfo> fileList;
	NN<IO::CameraControl::FileInfo> file;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	Data::DateTime dt;
	i = 0;
	j = this->camera->GetFileList(fileList);
	while (i < j)
	{
		file = fileList.GetItemNoCheck(i);
		this->lvFiles->AddItem(Text::CStringNN(file->fileName2, file->fileNameLen), file);
		this->lvFiles->SetSubItem(i, 1, Text::CStringNN(file->filePath2, file->filePathLen));
		sptr = Text::StrUInt64(sbuff, file->fileSize);
		this->lvFiles->SetSubItem(i, 2, CSTRP(sbuff, sptr));
		if (file->fileTimeTicks)
		{
			dt.SetTicks(file->fileTimeTicks);
			dt.ToLocalTime();
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
			this->lvFiles->SetSubItem(i, 3, CSTRP(sbuff, sptr));
		}
		else
		{
			this->lvFiles->SetSubItem(i, 3, CSTR("-"));
		}
		
		i++;
	}
}

SSWR::AVIRead::AVIRCameraControlForm::~AVIRCameraControlForm()
{
	this->ClearChildren();
	DEL_CLASS(this->camera);
	NN<const Data::ArrayListNN<Media::ImageList>> previewList = this->previewMap.GetValues();
	UOSInt i = previewList->GetCount();
	NN<Media::ImageList> previewImg;
	while (i-- > 0)
	{
		previewImg = previewList->GetItemNoCheck(i);
		previewImg.Delete();
	}
}

void SSWR::AVIRead::AVIRCameraControlForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
