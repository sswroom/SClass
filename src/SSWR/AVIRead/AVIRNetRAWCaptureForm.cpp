#include "Stdafx.h"
#include "IO/Path.h"
#include "Net/ConnectionInfo.h"
#include "SSWR/AVIRead/AVIRNetRAWCaptureForm.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRNetRAWCaptureForm::OnAutoGenClicked(void *userObj)
{
	SSWR::AVIRead::AVIRNetRAWCaptureForm *me = (SSWR::AVIRead::AVIRNetRAWCaptureForm*)userObj;
	Net::RAWCapture::FileFormat format = (Net::RAWCapture::FileFormat)(OSInt)me->cboFormat->GetSelectedItem();
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	Data::DateTime dt;
	sptr = IO::Path::GetProcessFileName(sbuff);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
	sptr = &sbuff[i + 1];
	dt.SetCurrTimeUTC();
	sptr = Text::StrInt64(sptr, dt.ToTicks());
	*sptr++ = '.';
	sptr = Net::RAWCapture::FileFormatGetExt(format).ConcatTo(sptr);
	me->txtFileName->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::AVIRead::AVIRNetRAWCaptureForm::OnBrowseClicked(void *userObj)
{
	SSWR::AVIRead::AVIRNetRAWCaptureForm *me = (SSWR::AVIRead::AVIRNetRAWCaptureForm*)userObj;
	UI::FileDialog *dlg;
	Text::StringBuilderUTF8 sb;
	me->txtFileName->GetText(&sb);
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"NetRAWCapture", true));
	Net::RAWCapture::AddFilters(dlg);
	if (sb.GetLength() > 0)
	{
		dlg->SetFileName(sb.ToCString());
	}
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->txtFileName->SetText(dlg->GetFileName()->ToCString());
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRNetRAWCaptureForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRNetRAWCaptureForm *me = (SSWR::AVIRead::AVIRNetRAWCaptureForm*)userObj;
	if (me->capture)
	{
		DEL_CLASS(me->capture);
		me->capture = 0;
		me->cboIP->SetEnabled(true);
		me->cboFormat->SetEnabled(true);
		me->cboType->SetEnabled(true);
		return;
	}

	Text::StringBuilderUTF8 sb;
	UInt32 ip = (UInt32)(OSInt)me->cboIP->GetSelectedItem();
	Net::RAWCapture::CaptureType type = (Net::RAWCapture::CaptureType)(OSInt)me->cboType->GetSelectedItem();
	Net::RAWCapture::FileFormat format = (Net::RAWCapture::FileFormat)(OSInt)me->cboFormat->GetSelectedItem();
	me->txtFileName->GetText(&sb);
	if (ip && sb.GetLength() > 0)
	{
		NEW_CLASS(me->capture, Net::RAWCapture(me->sockf, ip, type, format, sb.ToCString(), CSTR("AVIRead")));
		if (me->capture->IsError())
		{
			DEL_CLASS(me->capture);
			me->capture = 0;
			UI::MessageDialog::ShowDialog(CSTR("Error in creating socket"), CSTR("RAW Capture"), me);
		}
		else
		{
			me->cboIP->SetEnabled(false);
			me->cboFormat->SetEnabled(false);
			me->cboType->SetEnabled(false);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRNetRAWCaptureForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRNetRAWCaptureForm *me = (SSWR::AVIRead::AVIRNetRAWCaptureForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	UInt64 val;
	if (me->capture)
	{
		val = me->capture->GetPacketCnt();
		if (val != me->currCnt)
		{
			me->currCnt = val;
			sptr = Text::StrUInt64(sbuff, val);
			me->txtPacketCnt->SetText(CSTRP(sbuff, sptr));
		}

		val = me->capture->GetDataSize();
		if (val != me->currDataSize)
		{
			me->currDataSize = val;
			sptr = Text::StrUInt64(sbuff, val);
			me->txtDataSize->SetText(CSTRP(sbuff, sptr));
		}
	}
}

SSWR::AVIRead::AVIRNetRAWCaptureForm::AVIRNetRAWCaptureForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 300, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("RAW Capture"));

	this->core = core;
	this->sockf = core->GetSocketFactory();
	this->capture = 0;
	this->currCnt = (UInt64)-1;
	this->currDataSize = (UInt64)-1;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblIP, UI::GUILabel(ui, this, CSTR("IP")));
	this->lblIP->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboIP, UI::GUIComboBox(ui, this, false));
	this->cboIP->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->lblType, UI::GUILabel(ui, this, CSTR("Type")));
	this->lblType->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->cboType, UI::GUIComboBox(ui, this, false));
	this->cboType->SetRect(104, 28, 150, 23, false);
	NEW_CLASS(this->lblFormat, UI::GUILabel(ui, this, CSTR("Format")));
	this->lblFormat->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->cboFormat, UI::GUIComboBox(ui, this, false));
	this->cboFormat->SetRect(104, 52, 150, 23, false);
	NEW_CLASS(this->lblFileName, UI::GUILabel(ui, this, CSTR("File Name")));
	this->lblFileName->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtFileName, UI::GUITextBox(ui, this, CSTR("")));
	this->txtFileName->SetRect(104, 76, 500, 23, false);
	NEW_CLASS(this->btnAutoGen, UI::GUIButton(ui, this, CSTR("Auto Gen")));
	this->btnAutoGen->SetRect(604, 76, 75, 23, false);
	this->btnAutoGen->HandleButtonClick(OnAutoGenClicked, this);
	NEW_CLASS(this->btnBrowse, UI::GUIButton(ui, this, CSTR("&Browse")));
	this->btnBrowse->SetRect(684, 76, 75, 23, false);
	this->btnBrowse->HandleButtonClick(OnBrowseClicked, this);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this, CSTR("Start")));
	this->btnStart->SetRect(104, 100, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(this->lblPacketCnt, UI::GUILabel(ui, this, CSTR("Packet Count")));
	this->lblPacketCnt->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtPacketCnt, UI::GUITextBox(ui, this, CSTR("")));
	this->txtPacketCnt->SetReadOnly(true);
	this->txtPacketCnt->SetRect(104, 124, 150, 23, false);
	NEW_CLASS(this->lblDataSize, UI::GUILabel(ui, this, CSTR("Data Size")));
	this->lblDataSize->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->txtDataSize, UI::GUITextBox(ui, this, CSTR("")));
	this->txtDataSize->SetReadOnly(true);
	this->txtDataSize->SetRect(104, 148, 150, 23, false);

	Data::ArrayList<Net::ConnectionInfo*> connInfoList;
	Net::ConnectionInfo *connInfo;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UInt32 ip;
	this->sockf->GetConnInfoList(&connInfoList);
	i = 0;
	j = connInfoList.GetCount();
	while (i < j)
	{
		connInfo = connInfoList.GetItem(i);
		k = 0;
		while (true)
		{
			ip = connInfo->GetIPAddress(k);
			if (ip == 0)
				break;
			sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip);
			this->cboIP->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)ip);
			k++;
		}
		DEL_CLASS(connInfo);
		i++;
	}
	if (this->cboIP->GetCount() > 0)
	{
		this->cboIP->SetSelectedIndex(0);
	}

	i = Net::RAWCapture::CT_FIRST;
	j = Net::RAWCapture::CT_LAST;
	while (i <= j)
	{
		this->cboType->AddItem(Net::RAWCapture::CaptureTypeGetName((Net::RAWCapture::CaptureType)i), (void*)i);
		i++;
	}
	this->cboType->SetSelectedIndex(0);

	i = Net::RAWCapture::FF_FIRST;
	j = Net::RAWCapture::FF_LAST;
	while (i <= j)
	{
		this->cboFormat->AddItem(Net::RAWCapture::FileFormatGetName((Net::RAWCapture::FileFormat)i), (void*)i);
		i++;
	}
	this->cboFormat->SetSelectedIndex(0);

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRNetRAWCaptureForm::~AVIRNetRAWCaptureForm()
{
	SDEL_CLASS(this->capture);
}

void SSWR::AVIRead::AVIRNetRAWCaptureForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
