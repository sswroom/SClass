#include "Stdafx.h"
#include "Exporter/GUIJPGExporter.h"
#include "Exporter/WebPExporter.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "SSWR/AVIRead/AVIRImageBatchConvForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/FolderDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRImageBatchConvForm::OnBrowseClicked(void *userObj)
{
	SSWR::AVIRead::AVIRImageBatchConvForm *me = (SSWR::AVIRead::AVIRImageBatchConvForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtDir->GetText(sb);
	UI::FolderDialog dlg(L"SSWR", L"AVIRead", L"ImageBatchConv");
	if (sb.GetLength() > 0)
	{
		dlg.SetFolder(sb.ToCString());
	}
	if (dlg.ShowDialog(me->GetHandle()))
	{
		me->txtDir->SetText(dlg.GetFolder()->ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRImageBatchConvForm::OnConvertClicked(void *userObj)
{
	SSWR::AVIRead::AVIRImageBatchConvForm *me = (SSWR::AVIRead::AVIRImageBatchConvForm*)userObj;
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char *sptr;
	UTF8Char *sptrEnd;
	UTF8Char *sptr2;
	UTF8Char *sptr2End;
	Text::StringBuilderUTF8 sb;
	ConvertSess csess;
	csess.quality = 0;
	me->txtQuality->GetText(sbuff);
	csess.quality = Text::StrToInt32(sbuff);
	if (csess.quality <= 0 || csess.quality > 100)
	{
		UI::MessageDialog::ShowDialog(CSTR("Invalid Quality"), CSTR("Error"), me);
		return;
	}
	sptr = me->txtDir->GetText(sbuff);
	if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) != IO::Path::PathType::Directory)
	{
		UI::MessageDialog::ShowDialog(CSTR("Not a directory"), CSTR("Error"), me);
		return;
	}
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}

	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	IO::Path::FindFileSession *sess;
	csess.succ = true;
	csess.errMsg = 0;
	Text::CString ext;
	if (me->radFormatWebP->IsSelected())
	{
		NEW_CLASS(csess.exporter, Exporter::WebPExporter());
		ext = CSTR("webp");
	}
	else
	{
		NEW_CLASS(csess.exporter, Exporter::GUIJPGExporter());
		ext = CSTR("jpg");
	}
	IO::Path::PathType pt;
	sess = IO::Path::FindFile(CSTRP(sbuff, sptr2));
	if (sess)
	{
		*sptr = 0;
		sptr2 = Text::StrConcat(sbuff2, sbuff);
		if (me->chkSubdir->IsChecked())
		{
			sb.ClearStr();
			me->txtSubdir->GetText(sb);
			if (sb.GetLength() > 0)
			{
				sptr2 = Text::StrConcatC(sptr2, sb.ToString(), sb.GetLength());
				IO::Path::CreateDirectory(CSTRP(sbuff2, sptr2));
				*sptr2++ = IO::Path::PATH_SEPERATOR;
			}
		}
		while ((sptrEnd = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
		{
			if (pt == IO::Path::PathType::File && !Text::StrEndsWithICaseC(sptr, (UOSInt)(sptrEnd - sptr), ext.v, ext.leng))
			{
				Text::StrConcatC(sptr2, sptr, (UOSInt)(sptrEnd - sptr));
				sptr2End = IO::Path::ReplaceExt(sptr2, ext.v, ext.leng);
				me->MTConvertFile(&csess, CSTRP(sbuff, sptrEnd), CSTRP(sbuff2, sptr2End));
			}
			if (!csess.succ)
			{
				break;
			}
		}
		IO::Path::FindFileClose(sess);
		me->StopThreads();
	}
	DEL_CLASS(csess.exporter);
	if (csess.errMsg)
	{
		UI::MessageDialog::ShowDialog(csess.errMsg->ToCString(), CSTR("Image Batch Convert"), me);
		csess.errMsg->Release();
		csess.errMsg = 0;
	}
}

UInt32 __stdcall SSWR::AVIRead::AVIRImageBatchConvForm::ThreadFunc(void *userObj)
{
	ThreadState *state = (ThreadState*)userObj;
	{
		Sync::Event evt;
		state->evt = &evt;
		state->status = ThreadStatus::Idle;
		state->me->threadEvt.Set();
		while (!state->me->threadToStop || state->hasData)
		{
			if (state->hasData)
			{
				Text::String *srcFile;
				Text::String *destFile;
				state->status = ThreadStatus::Processing;
				state->hasData = false;
				srcFile = state->srcFile;
				destFile = state->destFile;
				state->me->ConvertFile(state->sess, srcFile->ToCString(), destFile->ToCString());
				srcFile->Release();
				destFile->Release();

				state->status = ThreadStatus::Idle;
				state->me->threadEvt.Set();
			}
			state->evt->Wait(1000);
		}
	}
	state->status = ThreadStatus::Stopped;
	state->me->threadEvt.Set();
	return 0;
}

void SSWR::AVIRead::AVIRImageBatchConvForm::StartThreads()
{
	if (this->threadStates)
	{
		return;
	}
	this->threadToStop = false;
	this->nThreads = Sync::ThreadUtil::GetThreadCnt();
	this->threadStates = MemAlloc(ThreadState, this->nThreads);
	UOSInt i = this->nThreads;
	while (i-- > 0)
	{
		this->threadStates[i].status = ThreadStatus::NotStarted;
		this->threadStates[i].hasData = false;
		this->threadStates[i].me = this;
		Sync::ThreadUtil::Create(ThreadFunc, &this->threadStates[i]);
	}
}

void SSWR::AVIRead::AVIRImageBatchConvForm::StopThreads()
{
	if (this->threadStates == 0)
	{
		return;
	}
	this->threadToStop = true;
	UOSInt i = this->nThreads;
	while (i-- > 0)
	{
		this->threadStates[i].evt->Set();
	}
	Bool found;
	while (true)
	{
		found = false;
		i = this->nThreads;
		while (i-- > 0)
		{
			if (this->threadStates[i].status != ThreadStatus::Stopped)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			break;
		}
		this->threadEvt.Wait(1000);
	}
	MemFree(this->threadStates);
	this->threadStates = 0;
}

void SSWR::AVIRead::AVIRImageBatchConvForm::MTConvertFile(ConvertSess *sess, Text::CStringNN srcFile, Text::CStringNN destFile)
{
	if (this->threadStates == 0)
	{
		this->StartThreads();
	}
	Bool found = false;
	while (true)
	{
		UOSInt i = this->nThreads;
		while (i-- > 0)
		{
			if (this->threadStates[i].status == ThreadStatus::Idle && !this->threadStates[i].hasData)
			{
				this->threadStates[i].sess = sess;
				this->threadStates[i].srcFile = Text::String::New(srcFile).Ptr();
				this->threadStates[i].destFile = Text::String::New(destFile).Ptr();
				this->threadStates[i].hasData = true;
				this->threadStates[i].evt->Set();
				found = true;
				break;
			}
		}
		if (found)
			break;
		this->threadEvt.Wait(1000);
	}
}

void SSWR::AVIRead::AVIRImageBatchConvForm::ConvertFile(ConvertSess *sess, Text::CStringNN srcFile, Text::CStringNN destFile)
{
	Media::ImageList *imgList;
	void *param;
	{
		IO::StmData::FileData fd(srcFile, false);
		imgList = (Media::ImageList*)this->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList);
	}
	if (imgList)
	{
		imgList->ToStaticImage(0);
		param = sess->exporter->CreateParam(imgList);
		if (param)
		{
			sess->exporter->SetParamInt32(param, 0, sess->quality);
		}
		{
			IO::FileStream fs(destFile, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			if (!sess->exporter->ExportFile(fs, destFile, imgList, param))
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("Error in converting to "));
				sb.Append(destFile);
				Sync::MutexUsage mutUsage(sess->mut);
				sess->succ = false;
				SDEL_STRING(sess->errMsg);
				sess->errMsg = Text::String::New(sb.ToCString()).Ptr();
			}
		}

		if (param)
		{
			sess->exporter->DeleteParam(param);
		}
		DEL_CLASS(imgList);
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Error in loading "));
		sb.Append(srcFile);
		Sync::MutexUsage mutUsage(sess->mut);
		sess->succ = false;
		SDEL_STRING(sess->errMsg);
		sess->errMsg = Text::String::New(sb.ToCString()).Ptr();
	}
}

SSWR::AVIRead::AVIRImageBatchConvForm::AVIRImageBatchConvForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 184, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Image Batch Convert"));
	this->SetNoResize(true);
	
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->nThreads = 0;
	this->threadStates = 0;

	NEW_CLASS(this->lblDir, UI::GUILabel(ui, this, CSTR("Folder")));
	this->lblDir->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->txtDir, UI::GUITextBox(ui, this, CSTR("")));
	this->txtDir->SetRect(100, 0, 450, 23, false);
	NEW_CLASS(this->btnBrowse, UI::GUIButton(ui, this, CSTR("&Browse")));
	this->btnBrowse->SetRect(550, 0, 75, 23, false);
	this->btnBrowse->HandleButtonClick(OnBrowseClicked, this);
	NEW_CLASS(this->lblOutFormat, UI::GUILabel(ui, this, CSTR("Output Format")));
	this->lblOutFormat->SetRect(0, 24, 100, 23, false);
	NEW_CLASS(this->radFormatJPG, UI::GUIRadioButton(ui, this, CSTR("JPEG"), true));
	this->radFormatJPG->SetRect(100, 24, 100, 23, false);
	NEW_CLASS(this->radFormatWebP, UI::GUIRadioButton(ui, this, CSTR("WebP"), false));
	this->radFormatWebP->SetRect(100, 48, 100, 23, false);
	NEW_CLASS(this->lblQuality, UI::GUILabel(ui, this, CSTR("Quality")));
	this->lblQuality->SetRect(0, 72, 100, 23, false);
	NEW_CLASS(this->txtQuality, UI::GUITextBox(ui, this, CSTR("100")));
	this->txtQuality->SetRect(100, 72, 100, 23, false);
	NEW_CLASS(this->chkSubdir, UI::GUICheckBox(ui, this, CSTR("OutSubdir"), true));
	this->chkSubdir->SetRect(0, 96, 100, 23, false);
	NEW_CLASS(this->txtSubdir, UI::GUITextBox(ui, this, CSTR("JPEG")));
	this->txtSubdir->SetRect(100, 96, 100, 23, false);
	NEW_CLASS(this->btnConvert, UI::GUIButton(ui, this, CSTR("&Convert")));
	this->btnConvert->SetRect(100, 120, 75, 23, false);
	this->btnConvert->HandleButtonClick(OnConvertClicked, this);
}

SSWR::AVIRead::AVIRImageBatchConvForm::~AVIRImageBatchConvForm()
{
	this->StopThreads();
}

void SSWR::AVIRead::AVIRImageBatchConvForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
