#include "Stdafx.h"
#include "IO/SerialPort.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRElectronicScaleForm.h"
#include "Sync/Thread.h"
#include "Text/Encoding.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRElectronicScaleForm::OnStreamClicked(void *userObj)
{
	SSWR::AVIRead::AVIRElectronicScaleForm *me = (SSWR::AVIRead::AVIRElectronicScaleForm *)userObj;
	if (me->stm)
	{
		me->StopStream();
	}
	else
	{
		SSWR::AVIRead::AVIRCore::StreamType st;
		me->stm = me->core->OpenStream(&st, me, 9600, false);
		if (me->stm)
		{
			me->txtStream->SetText(SSWR::AVIRead::AVIRCore::GetStreamTypeName(st));
			me->btnStream->SetText((const UTF8Char*)"&Close");
			me->remoteClosed = false;
			me->threadRunning = false;
			me->threadToStop = false;

			Sync::Thread::Create(RecvThread, me);
			while (!me->threadRunning && !me->remoteClosed)
			{
				Sync::Thread::Sleep(10);
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRElectronicScaleForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRElectronicScaleForm *me = (SSWR::AVIRead::AVIRElectronicScaleForm *)userObj;
	if (me->remoteClosed)
	{
		me->remoteClosed = false;
		me->StopStream();
	}

	if (me->currWeightUpd)
	{
		me->currWeightUpd = false;
		UTF8Char sbuff[64];
		OSInt strSize;
		strSize = Text::StrDouble(sbuff, Math::Unit::Mass::Convert(me->currWeightUnit, Math::Unit::Mass::MU_GRAM, me->currWeight)) - sbuff;
		me->txtWeight->SetText(sbuff);

		UOSInt w;
		UOSInt h;
		me->pbsDisplay->GetSizeP(&w, &h);
		if (w > 0 && h > 0)
		{
			if (me->dimg == 0)
			{
				me->dimg = me->eng->CreateImage32(w, h, Media::AT_NO_ALPHA);
			}
			else if (me->dimg->GetWidth() != w || me->dimg->GetHeight() != h)
			{
				me->eng->DeleteImage(me->dimg);
				me->dimg = me->eng->CreateImage32(w, h, Media::AT_NO_ALPHA);
			}

			if (me->dimg)
			{
				Media::DrawBrush *b;
				Media::DrawFont *f;
				b = me->dimg->NewBrushARGB(0xffffffff);
				me->dimg->DrawRect(0, 0, Math::UOSInt2Double(w), Math::UOSInt2Double(h), 0, b);
				me->dimg->DelBrush(b);

				Double fontHeight;
				Double sz[2];
				fontHeight = Math::UOSInt2Double(w) / Math::OSInt2Double(strSize);
				f = me->dimg->NewFontPx((const UTF8Char*)"Arial", fontHeight, Media::DrawEngine::DFS_NORMAL, 0);
				me->dimg->GetTextSize(f, sbuff, strSize, sz);
				me->dimg->DelFont(f);
				if (Math::UOSInt2Double(w) * sz[1] > Math::UOSInt2Double(h) * sz[0]) //w / sz[0] > h / sz[1]
				{
					fontHeight = fontHeight * Math::UOSInt2Double(h) / sz[1];
				}
				else
				{
					fontHeight = fontHeight * Math::UOSInt2Double(w) / sz[0];
				}
				f = me->dimg->NewFontPx((const UTF8Char*)"Arial", fontHeight, Media::DrawEngine::DFS_NORMAL, 0);
				b = me->dimg->NewBrushARGB(0xff000000);
				me->dimg->GetTextSize(f, sbuff, strSize, sz);
				me->dimg->DrawString((Math::UOSInt2Double(w) - sz[0]) * 0.5, (Math::UOSInt2Double(h) - sz[1]) * 0.5, sbuff, f, b);
				me->dimg->DelFont(f);
				me->dimg->DelBrush(b);

				me->pbsDisplay->SetImageDImg(me->dimg);
			}
		}
	}
}

UInt32 __stdcall SSWR::AVIRead::AVIRElectronicScaleForm::RecvThread(void *userObj)
{
	SSWR::AVIRead::AVIRElectronicScaleForm *me = (SSWR::AVIRead::AVIRElectronicScaleForm *)userObj;
	UInt8 buff[512];
	UOSInt recvSize;
	UOSInt buffSize = 0;
	me->threadRunning = true;
	while (!me->threadToStop)
	{
		recvSize = me->stm->Read(&buff[buffSize], 128);
		if (recvSize <= 0)
		{
			me->remoteClosed = true;
		}
		else
		{
			buffSize += recvSize;
			recvSize = 0;
			while (recvSize < buffSize - 17)
			{
				if (buff[recvSize + 16] == 13 && buff[recvSize + 17] == 10 && buff[recvSize + 2] == ',' && buff[recvSize + 5] == ',')
				{
					Bool isNeg = buff[recvSize + 6] == '-';
					Math::Unit::Mass::MassUnit munit = Math::Unit::Mass::MU_KILOGRAM;
					if (buff[recvSize + 14] == ' ' && buff[recvSize + 15] == 'g')
					{
						munit = Math::Unit::Mass::MU_GRAM;
					}
					else if (buff[recvSize + 14] == 'k' && buff[recvSize + 15] == 'g')
					{
						munit = Math::Unit::Mass::MU_KILOGRAM;
					}
					else if (buff[recvSize + 14] == 'l' && buff[recvSize + 15] == 'b')
					{
						munit = Math::Unit::Mass::MU_POUND;
					}
					else if (buff[recvSize + 14] == 'o' && buff[recvSize + 15] == 'z')
					{
						munit = Math::Unit::Mass::MU_OZ;
					}
					else
					{
						munit = Math::Unit::Mass::MU_KILOGRAM;
					}
					buff[recvSize + 14] = 0;
					Text::StrTrim((Char*)&buff[recvSize + 7]);
					Double weight = Text::StrToDouble((Char*)&buff[recvSize + 7]);
					if (isNeg)
					{
						weight = -weight;
					}

					me->currWeight = weight;
					me->currWeightUnit = munit;
					me->currWeightUpd = true;
					
					weight = Math::Unit::Mass::Convert(munit, Math::Unit::Mass::MU_GRAM, weight);
					me->rlcHistory->AddSample(&weight);

					recvSize += 18;
				}
				else
				{
					recvSize++;
				}
			}
			if (recvSize >= buffSize)
			{
				buffSize = 0;
			}
			else if (recvSize > 0)
			{
				MemCopyO(buff, &buff[recvSize], buffSize - recvSize);
				buffSize -= recvSize;
			}
		}
	}
	me->threadRunning = false;
	return 0;
}

void SSWR::AVIRead::AVIRElectronicScaleForm::StopStream()
{
	if (this->stm)
	{
		this->stm->Close();
		this->threadToStop = true;
		while (this->threadRunning)
		{
			Sync::Thread::Sleep(10);
		}
		this->threadToStop = false;
		DEL_CLASS(this->stm);
		this->stm = 0;
		this->txtStream->SetText((const UTF8Char*)"-");
		this->btnStream->SetText((const UTF8Char*)"&Open");
		this->remoteClosed = false;
	}
}

SSWR::AVIRead::AVIRElectronicScaleForm::AVIRElectronicScaleForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 456, 200, ui)
{
	this->SetText((const UTF8Char*)"Electronic Scale");
	this->SetFont(0, 8.25, false);
	
	this->core = core;
	this->eng = this->core->GetDrawEngine();
	this->dimg = 0;
	this->stm = 0;
	this->threadToStop = false;
	this->threadRunning = false;
	this->currWeight = 0;
	this->currWeightUnit = Math::Unit::Mass::MU_GRAM;
	this->currWeightUpd = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlCtrl, UI::GUIPanel(ui, this));
	this->pnlCtrl->SetRect(0, 0, 100, 80, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->grpStream, UI::GUIGroupBox(ui, this->pnlCtrl, (const UTF8Char*)"Stream"));
	this->grpStream->SetRect(0, 0, 100, 48, false);
	this->grpStream->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblStream, UI::GUILabel(ui, this->grpStream, (const UTF8Char*)"Stream Type"));
	this->lblStream->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtStream, UI::GUITextBox(ui, this->grpStream, (const UTF8Char*)"-"));
	this->txtStream->SetRect(104, 4, 200, 23, false);
	this->txtStream->SetReadOnly(true);
	NEW_CLASS(this->btnStream, UI::GUIButton(ui, this->grpStream, (const UTF8Char*)"&Open"));
	this->btnStream->SetRect(304, 4, 75, 23, false);
	this->btnStream->HandleButtonClick(OnStreamClicked, this);
	NEW_CLASS(this->lblWeight, UI::GUILabel(ui, this->pnlCtrl, (const UTF8Char*)"Weight(g)"));
	this->lblWeight->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtWeight, UI::GUITextBox(ui, this->pnlCtrl, (const UTF8Char*)""));
	this->txtWeight->SetRect(104, 52, 150, 23, false);
	this->txtWeight->SetReadOnly(true);
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpDisplay = this->tcMain->AddTabPage((const UTF8Char*)"Display");
	NEW_CLASS(this->pbsDisplay, UI::GUIPictureBoxSimple(ui, this->tpDisplay, this->eng, false));
	this->pbsDisplay->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpHistory = this->tcMain->AddTabPage((const UTF8Char*)"History");
	NEW_CLASS(this->rlcHistory, UI::GUIRealtimeLineChart(ui, this->tpHistory, this->core->GetDrawEngine(), 1, 500, 100));
	this->rlcHistory->SetDockType(UI::GUIControl::DOCK_FILL);

	this->AddTimer(100, OnTimerTick, this);
}

SSWR::AVIRead::AVIRElectronicScaleForm::~AVIRElectronicScaleForm()
{
	StopStream();
	if (this->dimg)
	{
		this->eng->DeleteImage(this->dimg);
		this->dimg = 0;
	}
}

void SSWR::AVIRead::AVIRElectronicScaleForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
