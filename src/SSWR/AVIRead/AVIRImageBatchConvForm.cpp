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
#include "UI/GUIFolderDialog.h"

void __stdcall SSWR::AVIRead::AVIRImageBatchConvForm::OnBrowseClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRImageBatchConvForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageBatchConvForm>();
	Text::StringBuilderUTF8 sb;
	me->txtDir->GetText(sb);
	NN<UI::GUIFolderDialog> dlg = me->ui->NewFolderDialog();
	if (sb.GetLength() > 0)
	{
		dlg->SetFolder(sb.ToCString());
	}
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtDir->SetText(dlg->GetFolder()->ToCString());
	}
	dlg.Delete();
}

void __stdcall SSWR::AVIRead::AVIRImageBatchConvForm::OnConvertClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRImageBatchConvForm> me = userObj.GetNN<SSWR::AVIRead::AVIRImageBatchConvForm>();
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptrEnd;
	UnsafeArray<UTF8Char> sptr2;
	UnsafeArray<UTF8Char> sptr2End;
	Text::StringBuilderUTF8 sb;
	ConvertSess csess;
	csess.quality = 0;
	me->txtQuality->GetText(sbuff);
	csess.quality = Text::StrToInt32(sbuff);
	if (csess.quality <= 0 || csess.quality > 100)
	{
		me->ui->ShowMsgOK(CSTR("Invalid Quality"), CSTR("Error"), me);
		return;
	}
	if (!me->txtDir->GetText(sbuff).SetTo(sptr) || IO::Path::GetPathType(CSTRP(sbuff, sptr)) != IO::Path::PathType::Directory)
	{
		me->ui->ShowMsgOK(CSTR("Not a directory"), CSTR("Error"), me);
		return;
	}
	if (sptr[-1] != IO::Path::PATH_SEPERATOR)
	{
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}

	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	NN<IO::Path::FindFileSession> sess;
	csess.succ = true;
	csess.errMsg = nullptr;
	Text::CStringNN ext;
	if (me->radFormatWebP->IsSelected())
	{
		NEW_CLASSNN(csess.exporter, Exporter::WebPExporter());
		ext = CSTR("webp");
	}
	else
	{
		NEW_CLASSNN(csess.exporter, Exporter::GUIJPGExporter());
		ext = CSTR("jpg");
	}
	IO::Path::PathType pt;
	if (IO::Path::FindFile(CSTRP(sbuff, sptr2)).SetTo(sess))
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
		while (IO::Path::FindNextFile(sptr, sess, 0, pt, 0).SetTo(sptrEnd))
		{
			if (pt == IO::Path::PathType::File && !Text::StrEndsWithICaseC(sptr, (UIntOS)(sptrEnd - sptr), ext.v, ext.leng))
			{
				Text::StrConcatC(sptr2, sptr, (UIntOS)(sptrEnd - sptr));
				sptr2End = IO::Path::ReplaceExt(sptr2, ext.v, ext.leng);
				me->MTConvertFile(csess, CSTRP(sbuff, sptrEnd), CSTRP(sbuff2, sptr2End));
			}
			if (!csess.succ)
			{
				break;
			}
		}
		IO::Path::FindFileClose(sess);
		me->StopThreads();
	}
	csess.exporter.Delete();
	NN<Text::String> errMsg;
	if (csess.errMsg.SetTo(errMsg))
	{
		me->ui->ShowMsgOK(errMsg->ToCString(), CSTR("Image Batch Convert"), me);
		errMsg->Release();
		csess.errMsg = nullptr;
	}
}

UInt32 __stdcall SSWR::AVIRead::AVIRImageBatchConvForm::ThreadFunc(AnyType userObj)
{
	NN<ThreadState> state = userObj.GetNN<ThreadState>();
	{
		Sync::Event evt;
		state->evt = evt;
		state->status = ThreadStatus::Idle;
		state->me->threadEvt.Set();
		while (!state->me->threadToStop || state->hasData)
		{
			if (state->hasData)
			{
				NN<Text::String> srcFile;
				NN<Text::String> destFile;
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
	UnsafeArray<ThreadState> threadStates;
	if (this->threadStates.NotNull())
	{
		return;
	}
	this->threadToStop = false;
	this->nThreads = Sync::ThreadUtil::GetThreadCnt();
	this->threadStates = threadStates = MemAllocArr(ThreadState, this->nThreads);
	UIntOS i = this->nThreads;
	while (i-- > 0)
	{
		threadStates[i].status = ThreadStatus::NotStarted;
		threadStates[i].hasData = false;
		threadStates[i].me = *this;
		Sync::ThreadUtil::Create(ThreadFunc, &threadStates[i]);
	}
}

void SSWR::AVIRead::AVIRImageBatchConvForm::StopThreads()
{
	UnsafeArray<ThreadState> threadStates;
	if (!this->threadStates.SetTo(threadStates))
	{
		return;
	}
	this->threadToStop = true;
	UIntOS i = this->nThreads;
	while (i-- > 0)
	{
		while (threadStates[i].status == ThreadStatus::NotStarted)
			Sync::ThreadUtil::SleepDur(10);
		threadStates[i].evt->Set();
	}
	Bool found;
	while (true)
	{
		found = false;
		i = this->nThreads;
		while (i-- > 0)
		{
			if (threadStates[i].status != ThreadStatus::Stopped)
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
	MemFreeArr(threadStates);
	this->threadStates = nullptr;
}

void SSWR::AVIRead::AVIRImageBatchConvForm::MTConvertFile(NN<ConvertSess> sess, Text::CStringNN srcFile, Text::CStringNN destFile)
{
	UnsafeArray<ThreadState> threadStates;
	if (!this->threadStates.SetTo(threadStates))
	{
		this->StartThreads();
		if (!this->threadStates.SetTo(threadStates))
		{
			return;
		}
	}

	Bool found = false;
	while (true)
	{
		UIntOS i = this->nThreads;
		while (i-- > 0)
		{
			if (threadStates[i].status == ThreadStatus::Idle && !threadStates[i].hasData)
			{
				threadStates[i].sess = sess;
				threadStates[i].srcFile = Text::String::New(srcFile);
				threadStates[i].destFile = Text::String::New(destFile);
				threadStates[i].hasData = true;
				threadStates[i].evt->Set();
				found = true;
				break;
			}
		}
		if (found)
			break;
		this->threadEvt.Wait(1000);
	}
}

void SSWR::AVIRead::AVIRImageBatchConvForm::ConvertFile(NN<ConvertSess> sess, Text::CStringNN srcFile, Text::CStringNN destFile)
{
	Optional<Media::ImageList> imgList;
	NN<Media::ImageList> nnimgList;
	Optional<IO::FileExporter::ParamData> param;
	{
		IO::StmData::FileData fd(srcFile, false);
		imgList = Optional<Media::ImageList>::ConvertFrom(this->core->GetParserList()->ParseFileType(fd, IO::ParserType::ImageList));
	}
	if (imgList.SetTo(nnimgList))
	{
		nnimgList->ToStaticImage(0);
		param = sess->exporter->CreateParam(nnimgList);
		if (!param.IsNull())
		{
			sess->exporter->SetParamInt32(param, 0, sess->quality);
		}
		{
			IO::FileStream fs(destFile, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			if (!sess->exporter->ExportFile(fs, destFile, nnimgList, param))
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("Error in converting to "));
				sb.Append(destFile);
				Sync::MutexUsage mutUsage(sess->mut);
				sess->succ = false;
				OPTSTR_DEL(sess->errMsg);
				sess->errMsg = Text::String::New(sb.ToCString());
			}
		}

		if (!param.IsNull())
		{
			sess->exporter->DeleteParam(param);
		}
		nnimgList.Delete();
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Error in loading "));
		sb.Append(srcFile);
		Sync::MutexUsage mutUsage(sess->mut);
		sess->succ = false;
		OPTSTR_DEL(sess->errMsg);
		sess->errMsg = Text::String::New(sb.ToCString());
	}
}

SSWR::AVIRead::AVIRImageBatchConvForm::AVIRImageBatchConvForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 184, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("Image Batch Convert"));
	this->SetNoResize(true);
	
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->nThreads = 0;
	this->threadStates = nullptr;

	this->lblDir = ui->NewLabel(*this, CSTR("Folder"));
	this->lblDir->SetRect(0, 0, 100, 23, false);
	this->txtDir = ui->NewTextBox(*this, CSTR(""));
	this->txtDir->SetRect(100, 0, 450, 23, false);
	this->btnBrowse = ui->NewButton(*this, CSTR("&Browse"));
	this->btnBrowse->SetRect(550, 0, 75, 23, false);
	this->btnBrowse->HandleButtonClick(OnBrowseClicked, this);
	this->lblOutFormat = ui->NewLabel(*this, CSTR("Output Format"));
	this->lblOutFormat->SetRect(0, 24, 100, 23, false);
	this->radFormatJPG = ui->NewRadioButton(*this, CSTR("JPEG"), true);
	this->radFormatJPG->SetRect(100, 24, 100, 23, false);
	this->radFormatWebP = ui->NewRadioButton(*this, CSTR("WebP"), false);
	this->radFormatWebP->SetRect(100, 48, 100, 23, false);
	this->lblQuality = ui->NewLabel(*this, CSTR("Quality"));
	this->lblQuality->SetRect(0, 72, 100, 23, false);
	this->txtQuality = ui->NewTextBox(*this, CSTR("100"));
	this->txtQuality->SetRect(100, 72, 100, 23, false);
	this->chkSubdir = ui->NewCheckBox(*this, CSTR("OutSubdir"), true);
	this->chkSubdir->SetRect(0, 96, 100, 23, false);
	this->txtSubdir = ui->NewTextBox(*this, CSTR("JPEG"));
	this->txtSubdir->SetRect(100, 96, 100, 23, false);
	this->btnConvert = ui->NewButton(*this, CSTR("&Convert"));
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
