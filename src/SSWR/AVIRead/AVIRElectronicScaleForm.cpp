#include "Stdafx.h"
#include "IO/SerialPort.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRElectronicScaleForm.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
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
		IO::StreamType st;
		me->stm = me->core->OpenStream(&st, me, 9600, false);
		if (me->stm)
		{
			me->txtStream->SetText(IO::StreamTypeGetName(st));
			me->btnStream->SetText(CSTR("&Close"));
			me->remoteClosed = false;
			me->threadRunning = false;
			me->threadToStop = false;

			Sync::ThreadUtil::Create(RecvThread, me);
			while (!me->threadRunning && !me->remoteClosed)
			{
				Sync::SimpleThread::Sleep(10);
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
		UTF8Char *sptr;
		UOSInt strSize;
		sptr = Text::StrDouble(sbuff, Math::Unit::Mass::Convert(me->currWeightUnit, Math::Unit::Mass::MU_GRAM, me->currWeight));
		me->txtWeight->SetText(CSTRP(sbuff, sptr));
		strSize = (UOSInt)(sptr - sbuff);

		Math::Size2D<UOSInt> usz = me->pbsDisplay->GetSizeP();
		if (usz.x > 0 && usz.y > 0)
		{
			NotNullPtr<Media::DrawImage> img;
			if (!img.Set(me->dimg))
			{
				me->dimg = me->eng->CreateImage32(usz, Media::AT_NO_ALPHA);
			}
			else if (img->GetWidth() != usz.x || img->GetHeight() != usz.y)
			{
				me->eng->DeleteImage(img);
				me->dimg = me->eng->CreateImage32(usz, Media::AT_NO_ALPHA);
			}

			if (img.Set(me->dimg))
			{
				NotNullPtr<Media::DrawBrush> b;
				NotNullPtr<Media::DrawFont> f;
				b = img->NewBrushARGB(0xffffffff);
				img->DrawRect(Math::Coord2DDbl(0, 0), usz.ToDouble(), 0, b);
				img->DelBrush(b);

				Double fontHeight;
				Math::Size2DDbl sz;
				fontHeight = UOSInt2Double(usz.x) / UOSInt2Double(strSize);
				f = img->NewFontPx(CSTR("Arial"), fontHeight, Media::DrawEngine::DFS_NORMAL, 0);
				sz = img->GetTextSize(f, {sbuff, strSize});
				img->DelFont(f);
				if (UOSInt2Double(usz.x) * sz.y > UOSInt2Double(usz.y) * sz.x) //w / sz[0] > h / sz[1]
				{
					fontHeight = fontHeight * UOSInt2Double(usz.y) / sz.y;
				}
				else
				{
					fontHeight = fontHeight * UOSInt2Double(usz.x) / sz.x;
				}
				f = img->NewFontPx(CSTR("Arial"), fontHeight, Media::DrawEngine::DFS_NORMAL, 0);
				b = img->NewBrushARGB(0xff000000);
				sz = img->GetTextSize(f, {sbuff, strSize});
				img->DrawString((usz.ToDouble() - sz) * 0.5, {sbuff, strSize}, f, b);
				img->DelFont(f);
				img->DelBrush(b);

				me->pbsDisplay->SetImageDImg(img.Ptr());
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
		recvSize = me->stm->Read(Data::ByteArray(&buff[buffSize], 128));
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
			Sync::SimpleThread::Sleep(10);
		}
		this->threadToStop = false;
		DEL_CLASS(this->stm);
		this->stm = 0;
		this->txtStream->SetText(CSTR("-"));
		this->btnStream->SetText(CSTR("&Open"));
		this->remoteClosed = false;
	}
}

SSWR::AVIRead::AVIRElectronicScaleForm::AVIRElectronicScaleForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 456, 200, ui)
{
	this->SetText(CSTR("Electronic Scale"));
	this->SetFont(0, 0, 8.25, false);
	
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

	NEW_CLASSNN(this->pnlCtrl, UI::GUIPanel(ui, *this));
	this->pnlCtrl->SetRect(0, 0, 100, 80, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASSNN(this->grpStream, UI::GUIGroupBox(ui, this->pnlCtrl, CSTR("Stream")));
	this->grpStream->SetRect(0, 0, 100, 48, false);
	this->grpStream->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblStream, UI::GUILabel(ui, this->grpStream, CSTR("Stream Type")));
	this->lblStream->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtStream, UI::GUITextBox(ui, this->grpStream, CSTR("-")));
	this->txtStream->SetRect(104, 4, 200, 23, false);
	this->txtStream->SetReadOnly(true);
	NEW_CLASS(this->btnStream, UI::GUIButton(ui, this->grpStream, CSTR("&Open")));
	this->btnStream->SetRect(304, 4, 75, 23, false);
	this->btnStream->HandleButtonClick(OnStreamClicked, this);
	NEW_CLASS(this->lblWeight, UI::GUILabel(ui, this->pnlCtrl, CSTR("Weight(g)")));
	this->lblWeight->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtWeight, UI::GUITextBox(ui, this->pnlCtrl, CSTR("")));
	this->txtWeight->SetRect(104, 52, 150, 23, false);
	this->txtWeight->SetReadOnly(true);
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, *this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpDisplay = this->tcMain->AddTabPage(CSTR("Display"));
	NEW_CLASS(this->pbsDisplay, UI::GUIPictureBoxSimple(ui, this->tpDisplay, this->eng, false));
	this->pbsDisplay->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpHistory = this->tcMain->AddTabPage(CSTR("History"));
	NEW_CLASS(this->rlcHistory, UI::GUIRealtimeLineChart(ui, this->tpHistory, this->core->GetDrawEngine(), 1, 500, 100));
	this->rlcHistory->SetDockType(UI::GUIControl::DOCK_FILL);

	this->AddTimer(100, OnTimerTick, this);
}

SSWR::AVIRead::AVIRElectronicScaleForm::~AVIRElectronicScaleForm()
{
	StopStream();
	NotNullPtr<Media::DrawImage> img;
	if (img.Set(this->dimg))
	{
		this->eng->DeleteImage(img);
		this->dimg = 0;
	}
}

void SSWR::AVIRead::AVIRElectronicScaleForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
