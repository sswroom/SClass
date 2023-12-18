#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/GPSNMEA.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/StmData/MemoryDataRef.h"
#include "SSWR/AVIRead/AVIRGISForm.h"
#include "SSWR/AVIRead/AVIRCameraControlForm.h"
#include "Text/MyString.h"
#include "UI/FileDialog.h"
#include "UI/FolderDialog.h"

void __stdcall SSWR::AVIRead::AVIRCameraControlForm::OnDownloadClicked(void *userObj)
{
	SSWR::AVIRead::AVIRCameraControlForm *me = (SSWR::AVIRead::AVIRCameraControlForm*)userObj;
	Data::ArrayList<UOSInt> selIndices;
	me->lvFiles->GetSelectedIndices(&selIndices);
	if (selIndices.GetCount() <= 0)
	{
		return;
	}
	else if (selIndices.GetCount() == 1)
	{
		IO::CameraControl::FileInfo *file = (IO::CameraControl::FileInfo*)me->lvFiles->GetItem(selIndices.GetItem(0));
		UI::FileDialog dlg(L"SSWR", L"AVIRead", L"CameraControlFile", true);
		dlg.SetFileName(Text::CString::FromPtr(file->fileName));
		if (dlg.ShowDialog(me->GetHandle()))
		{
			Data::DateTime dt;
			Bool succ;
			{
				IO::FileStream fs(dlg.GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				succ = me->camera->GetFile(file, &fs);
				if (file->fileTimeTicks)
				{
					dt.SetTicks(file->fileTimeTicks);
					fs.SetFileTimes(0, 0, &dt);
				}
			}
			if (!succ)
			{
				IO::Path::DeleteFile(dlg.GetFileName()->v);
				me->ui->ShowMsgOK(CSTR("Error in downloading the file"), CSTR("Camera Control"), me);
			}
		}
	}
	else
	{
		Data::DateTime dt;
		Text::StringBuilderUTF8 sb;
		UI::FolderDialog dlg(L"SSWR", L"AVIRead", L"CameraControlFolder");
		if (dlg.ShowDialog(me->GetHandle()))
		{
			Bool succ = true;
			UOSInt i = 0;
			UOSInt j = selIndices.GetCount();
			while (i < j)
			{
				IO::CameraControl::FileInfo *file = (IO::CameraControl::FileInfo*)me->lvFiles->GetItem(selIndices.GetItem(i));
				sb.ClearStr();
				sb.Append(dlg.GetFolder());
				if (!sb.EndsWith(IO::Path::PATH_SEPERATOR))
				{
					sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
				}
				sb.AppendSlow(file->fileName);

				{
					IO::FileStream fs(sb.ToCString(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
					succ = me->camera->GetFile(file, &fs);
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
					sb.AppendSlow(file->fileName);
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
	}
}

void __stdcall SSWR::AVIRead::AVIRCameraControlForm::OnFilesDblClick(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIRCameraControlForm *me = (SSWR::AVIRead::AVIRCameraControlForm*)userObj;
	IO::CameraControl::FileInfo *file = (IO::CameraControl::FileInfo*)me->lvFiles->GetItem(index);
	if (file == 0)
		return;
	if (file->fileType == IO::CameraControl::FT_IMAGE)
	{
		if (Text::StrEndsWithICase(file->fileName, (const UTF8Char*)".JPG"))
		{
			IO::MemoryStream mstm;
			if (me->camera->GetFile(file, &mstm))
			{
				NotNullPtr<IO::ParsedObject> pobj;
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
		if (Text::StrEndsWithICase(file->fileName, (const UTF8Char*)".LOG"))
		{
			IO::MemoryStream mstm;
			if (me->camera->GetFile(file, &mstm))
			{
				mstm.SeekFromBeginning(0);
				NotNullPtr<Map::GPSTrack> trk = IO::GPSNMEA::NMEA2Track(mstm, {file->fileName, Text::StrCharCnt(file->fileName)});
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

void __stdcall SSWR::AVIRead::AVIRCameraControlForm::OnFilesSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRCameraControlForm *me = (SSWR::AVIRead::AVIRCameraControlForm*)userObj;
	IO::CameraControl::FileInfo *file = (IO::CameraControl::FileInfo*)me->lvFiles->GetSelectedItem();
	if (file == 0)
		return;
	Media::ImageList *previewImg = me->previewMap.Get(file->fileName);
	if (previewImg)
	{
		me->pbPreview->SetImage((Media::StaticImage*)previewImg->GetImage(0, 0));
		return;
	}
	IO::MemoryStream mstm;
	if (me->camera->GetThumbnailFile(file, &mstm))
	{
		UOSInt size;
		UInt8 *buff = mstm.GetBuff(size);
		{
			IO::StmData::MemoryDataRef fd(buff, size);
			previewImg = (Media::ImageList*)me->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList);
		}
		if (previewImg)
		{
			previewImg->ToStaticImage(0);
			me->previewMap.Put(file->fileName, previewImg);
			me->pbPreview->SetImage((Media::StaticImage*)previewImg->GetImage(0, 0));
		}
	}
}

SSWR::AVIRead::AVIRCameraControlForm::AVIRCameraControlForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, IO::CameraControl *camera) : UI::GUIForm(parent, 640, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Camera Control"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->camera = camera;

	NEW_CLASS(this->lvInfo, UI::GUIListView(ui, *this, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvInfo->SetRect(0, 0, 100, 96, false);
	this->lvInfo->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lvInfo->SetFullRowSelect(true);
	this->lvInfo->SetShowGrid(true);
	this->lvInfo->AddColumn(CSTR("Name"), 200);
	this->lvInfo->AddColumn(CSTR("Value"), 400);
	NEW_CLASS(this->vspInfo, UI::GUIVSplitter(ui, *this, 3, false));
	NEW_CLASSNN(this->pnlControl, UI::GUIPanel(ui, *this));
	this->pnlControl->SetRect(0, 0, 100, 160, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->pbPreview, UI::GUIPictureBoxSimple(ui, this->pnlControl, this->core->GetDrawEngine(), false));
	this->pbPreview->SetRect(0, 0, 160, 160, false);
	this->btnDownload = ui->NewButton(this->pnlControl, CSTR("Download"));
	this->btnDownload->SetRect(164, 4, 75, 23, false);
	this->btnDownload->HandleButtonClick(OnDownloadClicked, this);
	NEW_CLASS(this->lvFiles, UI::GUIListView(ui, *this, UI::GUIListView::LVSTYLE_TABLE, 4));
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
	this->camera->GetInfoList(&nameList, &valueList);
	i = 0;
	j = nameList.GetCount();
	while (i < j)
	{
		this->lvInfo->AddItem(Text::String::OrEmpty(nameList.GetItem(i)), 0);
		this->lvInfo->SetSubItem(i, 1, Text::String::OrEmpty(valueList.GetItem(i)));
		i++;
	}
	this->camera->FreeInfoList(&nameList, &valueList);

	Data::ArrayList<IO::CameraControl::FileInfo *> fileList;
	IO::CameraControl::FileInfo *file;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	Data::DateTime dt;
	i = 0;
	j = this->camera->GetFileList(&fileList);
	while (i < j)
	{
		file = fileList.GetItem(i);
		this->lvFiles->AddItem({file->fileName, Text::StrCharCnt(file->fileName)}, file);
		this->lvFiles->SetSubItem(i, 1, Text::CStringNN::FromPtr(file->filePath));
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
	NotNullPtr<const Data::ArrayList<Media::ImageList*>> previewList = this->previewMap.GetValues();
	UOSInt i = previewList->GetCount();
	Media::ImageList *previewImg;
	while (i-- > 0)
	{
		previewImg = previewList->GetItem(i);
		DEL_CLASS(previewImg);
	}
}

void SSWR::AVIRead::AVIRCameraControlForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
