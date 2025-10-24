#include "Stdafx.h"
#include "IO/Path.h"
#include "Math/Math_C.h"
#include "Media/ICCProfile.h"
#include "SSWR/AVIRead/AVIRColorCustomForm.h"
#include "SSWR/AVIRead/AVIRColorSettingForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnMonProfileChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	if (me->inited)
	{
		UTF8Char sbuff[32];
		UnsafeArray<UTF8Char> sptr;
		Media::ColorProfile::CommonProfileType cpt = (Media::ColorProfile::CommonProfileType)me->cboMonProfile->GetSelectedItem().GetOSInt();
		if (cpt == Media::ColorProfile::CPT_FILE)
		{
			NN<const Media::ColorHandler::RGBPARAM2> rgbParam = me->monColor->GetRGBParam();
			me->txtMonTran->SetText(Media::CS::TransferTypeGetName(rgbParam->monProfile.GetRTranParamRead()->GetTranType()));
			me->txtMonPrimaries->SetText(Media::ColorProfile::ColorTypeGetName(rgbParam->monProfile.GetPrimariesRead()->colorType));
			sptr = Text::StrDouble(sbuff, rgbParam->monProfile.GetRTranParamRead()->GetGamma());
			me->txtMonRGBGamma->SetText(CSTRP(sbuff, sptr));
		}
		else
		{
			me->monColor->SetMonProfileType(cpt);
			NN<const Media::ColorHandler::RGBPARAM2> rgbParam = me->monColor->GetRGBParam();
			me->txtMonTran->SetText(Media::CS::TransferTypeGetName(rgbParam->monProfile.GetRTranParamRead()->GetTranType()));
			me->txtMonPrimaries->SetText(Media::ColorProfile::ColorTypeGetName(rgbParam->monProfile.GetPrimariesRead()->colorType));
			sptr = Text::StrDouble(sbuff, rgbParam->monProfile.GetRTranParamRead()->GetGamma());
			me->txtMonRGBGamma->SetText(CSTRP(sbuff, sptr));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnMonProfileClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	if (me->inited)
	{
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"ColorMonProfile", false);
		dlg->AddFilter(CSTR("*.icc"), CSTR("ICC File"));
		dlg->AddFilter(CSTR("*.icm"), CSTR("ICM File"));
		dlg->SetAllowMultiSel(false);
		sptr = Media::ICCProfile::GetProfilePath(sbuff);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		*sptr = 0;
		dlg->SetFileName(CSTRP(sbuff, sptr));
		if (dlg->ShowDialog(me->GetHandle()))
		{
			NN<Text::String> s = dlg->GetFileName();
			if (me->monColor->SetMonProfileFile(s))
			{
				OPTSTR_DEL(me->monFileName);
				me->monFileName = s->Clone();

				NN<const Media::ColorHandler::RGBPARAM2> rgbParam = me->monColor->GetRGBParam();
				me->txtMonTran->SetText(Media::CS::TransferTypeGetName(rgbParam->monProfile.GetRTranParamRead()->GetTranType()));
				me->txtMonPrimaries->SetText(Media::ColorProfile::ColorTypeGetName(rgbParam->monProfile.GetPrimariesRead()->colorType));
				sptr = Text::StrDouble(sbuff, rgbParam->monProfile.GetRTranParamRead()->GetGamma());
				me->txtMonRGBGamma->SetText(CSTRP(sbuff, sptr));
				me->cboMonProfile->SetSelectedIndex(me->cboMonProfile->GetCount() - 1);
			}
		}
		dlg.Delete();
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnMonCustomClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	if (me->inited)
	{
		UTF8Char sbuff[64];
		UnsafeArray<UTF8Char> sptr;
		{
			SSWR::AVIRead::AVIRColorCustomForm frm(0, me->ui, me->core, me->monColor);
			frm.ShowDialog(me);
		}

		NN<const Media::ColorHandler::RGBPARAM2> rgbParam = me->monColor->GetRGBParam();
		me->txtMonTran->SetText(Media::CS::TransferTypeGetName(rgbParam->monProfile.GetRTranParamRead()->GetTranType()));
		me->txtMonPrimaries->SetText(Media::ColorProfile::ColorTypeGetName(rgbParam->monProfile.GetPrimariesRead()->colorType));
		sptr = Text::StrDouble(sbuff, rgbParam->monProfile.GetRTranParamRead()->GetGamma());
		me->txtMonRGBGamma->SetText(CSTRP(sbuff, sptr));
		me->cboMonProfile->SetSelectedIndex(me->cboMonProfile->GetCount() - 2);
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnDefVProfileChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	if (me->inited)
	{
		me->colorMgr->SetDefVProfile((Media::ColorProfile::CommonProfileType)me->cboDefVProfile->GetSelectedItem().GetOSInt());
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnDefPProfileChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	if (me->inited)
	{
		me->colorMgr->SetDefPProfile((Media::ColorProfile::CommonProfileType)me->cboDefPProfile->GetSelectedItem().GetOSInt());
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::On10BitChanged(AnyType userObj, Bool newVal)
{
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	if (me->inited)
	{
		me->monColor->Set10BitColor(newVal);
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnOKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	Text::StringBuilderUTF8 sb;
	me->txtLuminance->GetText(sb);
	Double val;
	if (Text::StrToDouble(sb.ToString(), val))
	{
		me->monColor->SetMonLuminance(val);
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Luminance is not valid"), CSTR("Error"), me);
		return;
	}
	me->monColor->Save();
	me->colorMgr->SaveDef();
	me->normalClose = true;
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnCancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	me->monColor->Load();
	me->colorMgr->LoadDef();
	me->normalClose = true;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnCalRedBChg(AnyType userObj, UOSInt newPos)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	sptr = Text::StrConcatC(Text::StrDouble(sbuff, UOSInt2Double(newPos) * 0.1), UTF8STRC("%"));
	me->lblCalRedBV->SetText(CSTRP(sbuff, sptr));
	if (me->inited)
	{
		me->monColor->SetRMonBright(UOSInt2Double(newPos) * 0.001);
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnCalRedCChg(AnyType userObj, UOSInt newPos)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	sptr = Text::StrConcatC(Text::StrDouble(sbuff, UOSInt2Double(newPos) * 0.1), UTF8STRC("%"));
	me->lblCalRedCV->SetText(CSTRP(sbuff, sptr));
	if (me->inited)
	{
		me->monColor->SetRMonContr(UOSInt2Double(newPos) * 0.001);
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnCalRedGChg(AnyType userObj, UOSInt newPos)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	Double v = UOSInt2Double(newPos) * 0.001;
	sptr = Text::StrDouble(sbuff, v);
	me->lblCalRedGV->SetText(CSTRP(sbuff, sptr));
	if (me->inited)
	{
		me->monColor->SetRMonGamma(v);
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnCalGreenBChg(AnyType userObj, UOSInt newPos)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	sptr = Text::StrConcatC(Text::StrDouble(sbuff, UOSInt2Double(newPos) * 0.1), UTF8STRC("%"));
	me->lblCalGreenBV->SetText(CSTRP(sbuff, sptr));
	if (me->inited)
	{
		me->monColor->SetGMonBright(UOSInt2Double(newPos) * 0.001);
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnCalGreenCChg(AnyType userObj, UOSInt newPos)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	sptr = Text::StrConcatC(Text::StrDouble(sbuff, UOSInt2Double(newPos) * 0.1), UTF8STRC("%"));
	me->lblCalGreenCV->SetText(CSTRP(sbuff, sptr));
	if (me->inited)
	{
		me->monColor->SetGMonContr(UOSInt2Double(newPos) * 0.001);
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnCalGreenGChg(AnyType userObj, UOSInt newPos)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	Double v = UOSInt2Double(newPos) * 0.001;
	sptr = Text::StrDouble(sbuff, v);
	me->lblCalGreenGV->SetText(CSTRP(sbuff, sptr));
	if (me->inited)
	{
		me->monColor->SetGMonGamma(v);
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnCalBlueBChg(AnyType userObj, UOSInt newPos)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	sptr = Text::StrConcatC(Text::StrDouble(sbuff, UOSInt2Double(newPos) * 0.1), UTF8STRC("%"));
	me->lblCalBlueBV->SetText(CSTRP(sbuff, sptr));
	if (me->inited)
	{
		me->monColor->SetBMonBright(UOSInt2Double(newPos) * 0.001);
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnCalBlueCChg(AnyType userObj, UOSInt newPos)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	sptr = Text::StrConcatC(Text::StrDouble(sbuff, UOSInt2Double(newPos) * 0.1), UTF8STRC("%"));
	me->lblCalBlueCV->SetText(CSTRP(sbuff, sptr));
	if (me->inited)
	{
		me->monColor->SetBMonContr(UOSInt2Double(newPos) * 0.001);
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnCalBlueGChg(AnyType userObj, UOSInt newPos)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	Double v = UOSInt2Double(newPos) * 0.001;
	sptr = Text::StrDouble(sbuff, v);
	me->lblCalBlueGV->SetText(CSTRP(sbuff, sptr));
	if (me->inited)
	{
		me->monColor->SetBMonGamma(v);
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnMonVBrigChg(AnyType userObj, UOSInt newPos)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	sptr = Text::StrConcatC(Text::StrDouble(sbuff, UOSInt2Double(newPos) * 0.1), UTF8STRC("%"));
	me->lblMonVBrigV->SetText(CSTRP(sbuff, sptr));
	if (me->inited)
	{
		me->monColor->SetMonVBright(UOSInt2Double(newPos) * 0.001);
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnMonPBrigChg(AnyType userObj, UOSInt newPos)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	sptr = Text::StrConcatC(Text::StrDouble(sbuff, UOSInt2Double(newPos) * 0.1), UTF8STRC("%"));
	me->lblMonPBrigV->SetText(CSTRP(sbuff, sptr));
	if (me->inited)
	{
		me->monColor->SetMonPBright(UOSInt2Double(newPos) * 0.001);
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnMonRBrigChg(AnyType userObj, UOSInt newPos)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	sptr = Text::StrConcatC(Text::StrDouble(sbuff, UOSInt2Double(newPos) * 0.1), UTF8STRC("%"));
	me->lblMonRBrigV->SetText(CSTRP(sbuff, sptr));
	if (me->inited)
	{
		me->monColor->SetMonRBright(UOSInt2Double(newPos) * 0.001);
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnMonGBrigChg(AnyType userObj, UOSInt newPos)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	sptr = Text::StrConcatC(Text::StrDouble(sbuff, UOSInt2Double(newPos) * 0.1), UTF8STRC("%"));
	me->lblMonGBrigV->SetText(CSTRP(sbuff, sptr));
	if (me->inited)
	{
		me->monColor->SetMonGBright(UOSInt2Double(newPos) * 0.001);
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnMonBBrigChg(AnyType userObj, UOSInt newPos)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	sptr = Text::StrConcatC(Text::StrDouble(sbuff, UOSInt2Double(newPos) * 0.1), UTF8STRC("%"));
	me->lblMonBBrigV->SetText(CSTRP(sbuff, sptr));
	if (me->inited)
	{
		me->monColor->SetMonBBright(UOSInt2Double(newPos) * 0.001);
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnYUVBrigChg(AnyType userObj, UOSInt newPos)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	Double v = UOSInt2Double(newPos) * 0.001;
	sptr = Text::StrDouble(sbuff, v);
	me->lblYUVBrigV->SetText(CSTRP(sbuff, sptr));
	if (me->inited)
	{
		me->monColor->SetYUVBright(v);
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnYUVContChg(AnyType userObj, UOSInt newPos)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	Double v = UOSInt2Double(newPos) * 0.001;
	sptr = Text::StrDouble(sbuff, v);
	me->lblYUVContV->SetText(CSTRP(sbuff, sptr));
	if (me->inited)
	{
		me->monColor->SetYUVContr(v);
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnYUVSatChg(AnyType userObj, UOSInt newPos)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	Double v = UOSInt2Double(newPos) * 0.001;
	sptr = Text::StrDouble(sbuff, v);
	me->lblYUVSatV->SetText(CSTRP(sbuff, sptr));
	if (me->inited)
	{
		me->monColor->SetYUVSat(v);
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnYUVYGammaChg(AnyType userObj, UOSInt newPos)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	Double v = UOSInt2Double(newPos) * 0.001;
	sptr = Text::StrDouble(sbuff, v);
	me->lblYUVYGammaV->SetText(CSTRP(sbuff, sptr));
	if (me->inited)
	{
		me->monColor->SetYGamma(v);
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnYUVCGammaChg(AnyType userObj, UOSInt newPos)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	Double v = UOSInt2Double(newPos) * 0.001;
	sptr = Text::StrDouble(sbuff, v);
	me->lblYUVCGammaV->SetText(CSTRP(sbuff, sptr));
	if (me->inited)
	{
		me->monColor->SetCGamma(v);
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnYUVDefBT601Chg(AnyType userObj, Bool checked)
{
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	if (checked && me->inited)
	{
		me->colorMgr->SetYUVType(Media::ColorProfile::YUVT_BT601);
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnYUVDefBT709Chg(AnyType userObj, Bool checked)
{
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	if (checked && me->inited)
	{
		me->colorMgr->SetYUVType(Media::ColorProfile::YUVT_BT601);
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnYUVDefFCCChg(AnyType userObj, Bool checked)
{
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	if (checked && me->inited)
	{
		me->colorMgr->SetYUVType(Media::ColorProfile::YUVT_BT709);
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnYUVDefBT470BGChg(AnyType userObj, Bool checked)
{
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	if (checked && me->inited)
	{
		me->colorMgr->SetYUVType(Media::ColorProfile::YUVT_BT470BG);
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnYUVDefSMPTE170MChg(AnyType userObj, Bool checked)
{
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	if (checked && me->inited)
	{
		me->colorMgr->SetYUVType(Media::ColorProfile::YUVT_SMPTE170M);
	}
}

void __stdcall SSWR::AVIRead::AVIRColorSettingForm::OnYUVDefSMPTE240MChg(AnyType userObj, Bool checked)
{
	NN<SSWR::AVIRead::AVIRColorSettingForm> me = userObj.GetNN<SSWR::AVIRead::AVIRColorSettingForm>();
	if (checked && me->inited)
	{
		me->colorMgr->SetYUVType(Media::ColorProfile::YUVT_SMPTE240M);
	}
}

SSWR::AVIRead::AVIRColorSettingForm::AVIRColorSettingForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Optional<MonitorHandle> hMon) : UI::GUIForm(parent, 640, 480, ui)
{
	this->SetText(CSTR("Monitor Color Setting"));
	this->SetFont(0, 0, 8.25, false);
	this->SetNoResize(true);
	this->core = core;
	this->colorMgr = this->core->GetColorMgr();
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->hMon = hMon;
	this->monFileName = 0;
	Media::MonitorInfo monInfo(this->hMon);
	Optional<Text::String> monName = monInfo.GetDesc();
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;

	this->normalClose = false;
	this->inited = false;
	this->monColor = this->colorMgr->GetMonColorManagerByName(monInfo.GetMonitorID());
	NN<Text::String> s;
	if (this->monColor->GetMonProfileFile().SetTo(s))
	{
		this->monFileName = s->Clone().Ptr();
	}

	UOSInt i;
	UOSInt j;
	UOSInt k;
	NN<const Media::ColorHandler::RGBPARAM2> rgbParam;
	NN<const Media::ColorHandler::YUVPARAM> yuvParam;
	rgbParam = this->monColor->GetRGBParam();
	yuvParam = this->monColor->GetYUVParam();

	this->pnlMonitor = ui->NewPanel(*this);
	this->pnlMonitor->SetRect(0, 0, 100, 23, false);
	this->pnlMonitor->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblMonitor = ui->NewLabel(this->pnlMonitor, CSTR("Monitor"));
	this->lblMonitor->SetRect(0, 0, 100, 23, false);
	this->txtMonitor = ui->NewTextBox(this->pnlMonitor, CSTR(""), false);
	this->txtMonitor->SetRect(100, 0, 300, 23, false);
	this->txtMonitor->SetReadOnly(true);
	NN<Text::String> nnmonName;
	if (monName.SetTo(nnmonName))
	{
		this->txtMonitor->SetText(nnmonName->ToCString());
	}
	this->pnlButtons = ui->NewPanel(*this);
	this->pnlButtons->SetRect(0, 0, 100, 40, false);
	this->pnlButtons->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnOK = ui->NewButton(this->pnlButtons, CSTR("OK"));
	this->btnOK->SetRect(216, 8, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->btnCancel = ui->NewButton(this->pnlButtons, CSTR("&Cancel"));
	this->btnCancel->SetRect(304, 8, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpSetting = this->tcMain->AddTabPage(CSTR("Setting"));
	this->lblMonProfile = ui->NewLabel(this->tpSetting, CSTR("Monitor Profile"));
	this->lblMonProfile->SetRect(0, 0, 150, 23, false);
	this->cboMonProfile = ui->NewComboBox(this->tpSetting, false);
	this->cboMonProfile->SetRect(150, 0, 150, 23, false);
	this->cboMonProfile->HandleSelectionChange(OnMonProfileChg, this);
	i = 0;
	j = Media::ColorProfile::CPT_LAST;
	while (i < j)
	{
		k = this->cboMonProfile->AddItem(Media::ColorProfile::CommonProfileTypeGetName((Media::ColorProfile::CommonProfileType)i), (void*)i);
		if (i == (UOSInt)rgbParam->monProfileType)
		{
			this->cboMonProfile->SetSelectedIndex(k);
		}
		i++;
	}
	k = this->cboMonProfile->AddItem(CSTR("As OS"), (void*)Media::ColorProfile::CPT_OS);
	if (rgbParam->monProfileType == Media::ColorProfile::CPT_OS)
	{
		this->cboMonProfile->SetSelectedIndex(k);
	}
	k = this->cboMonProfile->AddItem(CSTR("EDID"), (void*)Media::ColorProfile::CPT_EDID);
	if (rgbParam->monProfileType == Media::ColorProfile::CPT_EDID)
	{
		this->cboMonProfile->SetSelectedIndex(k);
	}
	k = this->cboMonProfile->AddItem(CSTR("CUSTOM"), (void*)Media::ColorProfile::CPT_CUSTOM);
	if (rgbParam->monProfileType == Media::ColorProfile::CPT_CUSTOM)
	{
		this->cboMonProfile->SetSelectedIndex(k);
	}
	k = this->cboMonProfile->AddItem(CSTR("File"), (void*)Media::ColorProfile::CPT_FILE);
	if (rgbParam->monProfileType == Media::ColorProfile::CPT_FILE)
	{
		this->cboMonProfile->SetSelectedIndex(k);
	}
	this->btnMonProfile = ui->NewButton(this->tpSetting, CSTR("B&rowse"));
	this->btnMonProfile->SetRect(300, 0, 75, 23, false);
	this->btnMonProfile->HandleButtonClick(OnMonProfileClicked, this);
	this->grpMonProfile = ui->NewGroupBox(this->tpSetting, CSTR("Monitor Profile"));
	this->grpMonProfile->SetRect(0, 24, 500, 72, false);
	this->lblMonTran = ui->NewLabel(this->grpMonProfile, CSTR("Transfer Characteristics"));
	this->lblMonTran->SetRect(0, 0, 150, 23, false);
	this->txtMonTran = ui->NewTextBox(this->grpMonProfile, CSTR(""));
	this->txtMonTran->SetRect(150, 0, 150, 23, false);
	this->txtMonTran->SetReadOnly(true);
	this->lblMonRGBGamma = ui->NewLabel(this->grpMonProfile, CSTR("Gamma"));
	this->lblMonRGBGamma->SetRect(300, 0, 80, 23, false);
	this->txtMonRGBGamma = ui->NewTextBox(this->grpMonProfile, CSTR(""));
	this->txtMonRGBGamma->SetRect(380, 0, 100, 23, false);
	this->txtMonRGBGamma->SetReadOnly(true);
	this->lblMonPrimaries = ui->NewLabel(this->grpMonProfile, CSTR("Color Primaries"));
	this->lblMonPrimaries->SetRect(0, 24, 150, 23, false);
	this->txtMonPrimaries = ui->NewTextBox(this->grpMonProfile, CSTR(""));
	this->txtMonPrimaries->SetRect(150, 24, 150, 23, false);
	this->txtMonPrimaries->SetReadOnly(true);
	this->btnMonCustom = ui->NewButton(this->grpMonProfile, CSTR("Custom"));
	this->btnMonCustom->SetRect(300, 24, 75, 23, false);
	this->btnMonCustom->HandleButtonClick(OnMonCustomClicked, this);
	this->lblDefVProfile = ui->NewLabel(this->tpSetting, CSTR("Def Video Profile"));
	this->lblDefVProfile->SetRect(0, 104, 150, 23, false);
	this->cboDefVProfile = ui->NewComboBox(this->tpSetting, false);
	this->cboDefVProfile->SetRect(150, 104, 150, 23, false);
	this->cboDefVProfile->HandleSelectionChange(OnDefVProfileChg, this);
	this->lblDefPProfile = ui->NewLabel(this->tpSetting, CSTR("Def Photo Profile"));
	this->lblDefPProfile->SetRect(0, 128, 150, 23, false);
	this->cboDefPProfile = ui->NewComboBox(this->tpSetting, false);
	this->cboDefPProfile->SetRect(150, 128, 150, 23, false);
	this->cboDefPProfile->HandleSelectionChange(OnDefPProfileChg, this);
	i = 0;
	j = Media::ColorProfile::CPT_LAST;
	while (i < j)
	{
		k = this->cboDefVProfile->AddItem(Media::ColorProfile::CommonProfileTypeGetName((Media::ColorProfile::CommonProfileType)i), (void*)i);
		if (i == (UOSInt)this->colorMgr->GetDefVProfileType())
		{
			this->cboDefVProfile->SetSelectedIndex(k);
		}
		k = this->cboDefPProfile->AddItem(Media::ColorProfile::CommonProfileTypeGetName((Media::ColorProfile::CommonProfileType)i), (void*)i);
		if (i == (UOSInt)this->colorMgr->GetDefPProfileType())
		{
			this->cboDefPProfile->SetSelectedIndex(k);
		}
		i++;
	}
	this->chk10Bit = ui->NewCheckBox(this->tpSetting, CSTR("10-Bit Color"), false);
	this->chk10Bit->SetRect(0, 152, 150, 23, false);
	if (this->monColor->Get10BitColor())
	{
		this->chk10Bit->SetChecked(true);
	}
	this->chk10Bit->HandleCheckedChange(On10BitChanged, this);
	this->lblLuminance = ui->NewLabel(this->tpSetting, CSTR("Luminance"));
	this->lblLuminance->SetRect(0, 176, 100, 23, false);
	this->txtLuminance = ui->NewTextBox(this->tpSetting, CSTR("250"));
	this->txtLuminance->SetRect(100, 176, 50, 23, false);
	this->lblLuminanceUnit = ui->NewLabel(this->tpSetting, CSTR("cd/m2"));
	this->lblLuminanceUnit->SetRect(150, 176, 100, 23, false);
	this->grpYUVDef = ui->NewGroupBox(this->tpSetting, CSTR("Default YUV Type"));
	this->grpYUVDef->SetRect(0, 208, 280, 96, false);
	this->radYUVDefBT601 = ui->NewRadioButton(this->grpYUVDef, CSTR("BT601"), false);
	this->radYUVDefBT601->SetRect(8, 0, 100, 23, false);
	this->radYUVDefBT601->HandleSelectedChange(OnYUVDefBT601Chg, this);
	this->radYUVDefBT709 = ui->NewRadioButton(this->grpYUVDef, CSTR("BT709"), false);
	this->radYUVDefBT709->SetRect(8, 24, 100, 23, false);
	this->radYUVDefBT709->HandleSelectedChange(OnYUVDefBT709Chg, this);
	this->radYUVDefFCC = ui->NewRadioButton(this->grpYUVDef, CSTR("FCC"), false);
	this->radYUVDefFCC->SetRect(8, 48, 100, 23, false);
	this->radYUVDefFCC->HandleSelectedChange(OnYUVDefFCCChg, this);
	this->radYUVDefBT470BG = ui->NewRadioButton(this->grpYUVDef, CSTR("BT470BG"), false);
	this->radYUVDefBT470BG->SetRect(144, 0, 100, 23, false);
	this->radYUVDefBT470BG->HandleSelectedChange(OnYUVDefBT470BGChg, this);
	this->radYUVDefSMPTE170M = ui->NewRadioButton(this->grpYUVDef, CSTR("SMPTE 170M"), false);
	this->radYUVDefSMPTE170M->SetRect(144, 24, 100, 23, false);
	this->radYUVDefSMPTE170M->HandleSelectedChange(OnYUVDefSMPTE170MChg, this);
	this->radYUVDefSMPTE240M = ui->NewRadioButton(this->grpYUVDef, CSTR("SMPTE 240M"), false);
	this->radYUVDefSMPTE240M->SetRect(144, 48, 100, 23, false);
	this->radYUVDefSMPTE240M->HandleSelectedChange(OnYUVDefSMPTE240MChg, this);

	this->tpCal = this->tcMain->AddTabPage(CSTR("Calibrate"));
	this->pnlCalRed = ui->NewPanel(this->tpCal);
	this->pnlCalRed->SetBGColor(0xffffc0c0);
	this->pnlCalRed->SetRect(0, 0, 608, 104, false);
	this->pnlCalRed->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblCalRedB = ui->NewLabel(this->pnlCalRed, CSTR("Brightness"));
	this->lblCalRedB->SetRect(8, 8, 120, 16, false);
	this->lblCalRedB->SetBGColor(0xffffc0c0);
	this->hsbCalRedB = ui->NewHScrollBar(this->pnlCalRed, 16);
	this->hsbCalRedB->SetRect(128, 8, 392, 17, false);
	this->hsbCalRedB->InitScrollBar(0, 4099, 1000, 100);
	this->hsbCalRedB->HandlePosChanged(OnCalRedBChg, this);
	this->lblCalRedBV = ui->NewLabel(this->pnlCalRed, CSTR(""));
	this->lblCalRedBV->SetRect(520, 8, 64, 16, false);
	this->lblCalRedC = ui->NewLabel(this->pnlCalRed, CSTR("Contrast"));
	this->lblCalRedC->SetRect(8, 32, 120, 16, false);
	this->hsbCalRedC = ui->NewHScrollBar(this->pnlCalRed, 16);
	this->hsbCalRedC->SetRect(128, 32, 392, 17, false);
	this->hsbCalRedC->InitScrollBar(0, 4099, 1000, 100);
	this->hsbCalRedC->HandlePosChanged(OnCalRedCChg, this);
	this->lblCalRedCV = ui->NewLabel(this->pnlCalRed, CSTR(""));
	this->lblCalRedCV->SetRect(520, 32, 64, 16, false);
	this->lblCalRedG = ui->NewLabel(this->pnlCalRed, CSTR("Gamma"));
	this->lblCalRedG->SetRect(8, 56, 120, 16, false);
	this->hsbCalRedG = ui->NewHScrollBar(this->pnlCalRed, 16);
	this->hsbCalRedG->SetRect(128, 56, 392, 17, false);
	this->hsbCalRedG->InitScrollBar(0, 4099, 1000, 100);
	this->hsbCalRedG->HandlePosChanged(OnCalRedGChg, this);
	this->lblCalRedGV = ui->NewLabel(this->pnlCalRed, CSTR(""));
	this->lblCalRedGV->SetRect(520, 56, 64, 16, false);
	this->pnlCalGreen = ui->NewPanel(this->tpCal);
	this->pnlCalGreen->SetBGColor(0xffc0ffc0);
	this->pnlCalGreen->SetRect(0, 0, 608, 104, false);
	this->pnlCalGreen->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblCalGreenB = ui->NewLabel(this->pnlCalGreen, CSTR("Brightness"));
	this->lblCalGreenB->SetRect(8, 8, 120, 16, false);
	this->lblCalGreenB->SetBGColor(0xffc0ffc0);
	this->hsbCalGreenB = ui->NewHScrollBar(this->pnlCalGreen, 16);
	this->hsbCalGreenB->SetRect(128, 8, 392, 17, false);
	this->hsbCalGreenB->InitScrollBar(0, 4099, 1000, 100);
	this->hsbCalGreenB->HandlePosChanged(OnCalGreenBChg, this);
	this->lblCalGreenBV = ui->NewLabel(this->pnlCalGreen, CSTR(""));
	this->lblCalGreenBV->SetRect(520, 8, 64, 16, false);
	this->lblCalGreenC = ui->NewLabel(this->pnlCalGreen, CSTR("Contrast"));
	this->lblCalGreenC->SetRect(8, 32, 120, 16, false);
	this->hsbCalGreenC = ui->NewHScrollBar(this->pnlCalGreen, 16);
	this->hsbCalGreenC->SetRect(128, 32, 392, 17, false);
	this->hsbCalGreenC->InitScrollBar(0, 4099, 1000, 100);
	this->hsbCalGreenC->HandlePosChanged(OnCalGreenCChg, this);
	this->lblCalGreenCV = ui->NewLabel(this->pnlCalGreen, CSTR(""));
	this->lblCalGreenCV->SetRect(520, 32, 64, 16, false);
	this->lblCalGreenG = ui->NewLabel(this->pnlCalGreen, CSTR("Gamma"));
	this->lblCalGreenG->SetRect(8, 56, 120, 16, false);
	this->hsbCalGreenG = ui->NewHScrollBar(this->pnlCalGreen, 16);
	this->hsbCalGreenG->SetRect(128, 56, 392, 17, false);
	this->hsbCalGreenG->InitScrollBar(0, 4099, 1000, 100);
	this->hsbCalGreenG->HandlePosChanged(OnCalGreenGChg, this);
	this->lblCalGreenGV = ui->NewLabel(this->pnlCalGreen, CSTR(""));
	this->lblCalGreenGV->SetRect(520, 56, 64, 16, false);
	this->pnlCalBlue = ui->NewPanel(this->tpCal);
	this->pnlCalBlue->SetBGColor(0xffc0c0ff);
	this->pnlCalBlue->SetRect(0, 0, 608, 104, false);
	this->pnlCalBlue->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblCalBlueB = ui->NewLabel(this->pnlCalBlue, CSTR("Brightness"));
	this->lblCalBlueB->SetRect(8, 8, 120, 16, false);
	this->lblCalBlueB->SetBGColor(0xffc0c0ff);
	this->hsbCalBlueB = ui->NewHScrollBar(this->pnlCalBlue, 16);
	this->hsbCalBlueB->SetRect(128, 8, 392, 17, false);
	this->hsbCalBlueB->InitScrollBar(0, 4099, 1000, 100);
	this->hsbCalBlueB->HandlePosChanged(OnCalBlueBChg, this);
	this->lblCalBlueBV = ui->NewLabel(this->pnlCalBlue, CSTR(""));
	this->lblCalBlueBV->SetRect(520, 8, 64, 16, false);
	this->lblCalBlueC = ui->NewLabel(this->pnlCalBlue, CSTR("Contrast"));
	this->lblCalBlueC->SetRect(8, 32, 120, 16, false);
	this->hsbCalBlueC = ui->NewHScrollBar(this->pnlCalBlue, 16);
	this->hsbCalBlueC->SetRect(128, 32, 392, 17, false);
	this->hsbCalBlueC->InitScrollBar(0, 4099, 1000, 100);
	this->hsbCalBlueC->HandlePosChanged(OnCalBlueCChg, this);
	this->lblCalBlueCV = ui->NewLabel(this->pnlCalBlue, CSTR(""));
	this->lblCalBlueCV->SetRect(520, 32, 64, 16, false);
	this->lblCalBlueG = ui->NewLabel(this->pnlCalBlue, CSTR("Gamma"));
	this->lblCalBlueG->SetRect(8, 56, 120, 16, false);
	this->hsbCalBlueG = ui->NewHScrollBar(this->pnlCalBlue, 16);
	this->hsbCalBlueG->SetRect(128, 56, 392, 17, false);
	this->hsbCalBlueG->InitScrollBar(0, 4099, 1000, 100);
	this->hsbCalBlueG->HandlePosChanged(OnCalBlueGChg, this);
	this->lblCalBlueGV = ui->NewLabel(this->pnlCalBlue, CSTR(""));
	this->lblCalBlueGV->SetRect(520, 56, 64, 16, false);

	this->tpRGB = this->tcMain->AddTabPage(CSTR("RGB"));
	this->lblMonVBrig = ui->NewLabel(this->tpRGB, CSTR("Monitor Video Brightness"));
	this->lblMonVBrig->SetRect(8, 8, 128, 16, false);
	this->hsbMonVBrig = ui->NewHScrollBar(this->tpRGB, 16);
	this->hsbMonVBrig->SetRect(136, 8, 392, 17, false);
	this->hsbMonVBrig->InitScrollBar(0, 4099, 1000, 100);
	this->hsbMonVBrig->HandlePosChanged(OnMonVBrigChg, this);
	this->lblMonVBrigV = ui->NewLabel(this->tpRGB, CSTR(""));
	this->lblMonVBrigV->SetRect(528, 8, 64, 16, false);
	this->lblMonPBrig = ui->NewLabel(this->tpRGB, CSTR("Monitor Photo Brightness"));
	this->lblMonPBrig->SetRect(8, 32, 128, 16, false);
	this->hsbMonPBrig = ui->NewHScrollBar(this->tpRGB, 16);
	this->hsbMonPBrig->SetRect(136, 32, 392, 17, false);
	this->hsbMonPBrig->InitScrollBar(0, 4099, 1000, 100);
	this->hsbMonPBrig->HandlePosChanged(OnMonPBrigChg, this);
	this->lblMonPBrigV = ui->NewLabel(this->tpRGB, CSTR(""));
	this->lblMonPBrigV->SetRect(528, 32, 64, 16, false);
	this->lblMonRBrig = ui->NewLabel(this->tpRGB, CSTR("Monitor R Brightness"));
	this->lblMonRBrig->SetRect(8, 56, 128, 16, false);
	this->lblMonRBrig->SetBGColor(0xffffc0c0);
	this->hsbMonRBrig = ui->NewHScrollBar(this->tpRGB, 16);
	this->hsbMonRBrig->SetRect(136, 56, 392, 17, false);
	this->hsbMonRBrig->InitScrollBar(0, 4099, 1000, 100);
	this->hsbMonRBrig->HandlePosChanged(OnMonRBrigChg, this);
	this->lblMonRBrigV = ui->NewLabel(this->tpRGB, CSTR(""));
	this->lblMonRBrigV->SetRect(528, 56, 64, 16, false);
	this->lblMonGBrig = ui->NewLabel(this->tpRGB, CSTR("Monitor G Brightness"));
	this->lblMonGBrig->SetRect(8, 80, 128, 16, false);
	this->lblMonGBrig->SetBGColor(0xffc0ffc0);
	this->hsbMonGBrig = ui->NewHScrollBar(this->tpRGB, 16);
	this->hsbMonGBrig->SetRect(136, 80, 392, 17, false);
	this->hsbMonGBrig->InitScrollBar(0, 4099, 1000, 100);
	this->hsbMonGBrig->HandlePosChanged(OnMonGBrigChg, this);
	this->lblMonGBrigV = ui->NewLabel(this->tpRGB, CSTR(""));
	this->lblMonGBrigV->SetRect(528, 80, 64, 16, false);
	this->lblMonBBrig = ui->NewLabel(this->tpRGB, CSTR("Monitor B Brightness"));
	this->lblMonBBrig->SetRect(8, 104, 128, 16, false);
	this->lblMonBBrig->SetBGColor(0xffc0c0ff);
	this->hsbMonBBrig = ui->NewHScrollBar(this->tpRGB, 16);
	this->hsbMonBBrig->SetRect(136, 104, 392, 17, false);
	this->hsbMonBBrig->InitScrollBar(0, 4099, 1000, 100);
	this->hsbMonBBrig->HandlePosChanged(OnMonBBrigChg, this);
	this->lblMonBBrigV = ui->NewLabel(this->tpRGB, CSTR(""));
	this->lblMonBBrigV->SetRect(528, 104, 64, 16, false);

	this->tpYUV = this->tcMain->AddTabPage(CSTR("YUV"));
	this->lblYUVBrig = ui->NewLabel(this->tpYUV, CSTR("Brightness"));
	this->lblYUVBrig->SetRect(8, 8, 120, 16, false);
	this->hsbYUVBrig = ui->NewHScrollBar(this->tpYUV, 16);
	this->hsbYUVBrig->SetRect(128, 8, 392, 17, false);
	this->hsbYUVBrig->InitScrollBar(0, 4099, 1000, 100);
	this->hsbYUVBrig->HandlePosChanged(OnYUVBrigChg, this);
	this->lblYUVBrigV = ui->NewLabel(this->tpYUV, CSTR(""));
	this->lblYUVBrigV->SetRect(520, 8, 64, 16, false);
	this->lblYUVCont = ui->NewLabel(this->tpYUV, CSTR("Contrast"));
	this->lblYUVCont->SetRect(8, 32, 120, 16, false);
	this->hsbYUVCont = ui->NewHScrollBar(this->tpYUV, 16);
	this->hsbYUVCont->SetRect(128, 32, 392, 17, false);
	this->hsbYUVCont->InitScrollBar(0, 4099, 1000, 100);
	this->hsbYUVCont->HandlePosChanged(OnYUVContChg, this);
	this->lblYUVContV = ui->NewLabel(this->tpYUV, CSTR(""));
	this->lblYUVContV->SetRect(520, 32, 64, 16, false);
	this->lblYUVSat = ui->NewLabel(this->tpYUV, CSTR("Saturation"));
	this->lblYUVSat->SetRect(8, 56, 120, 16, false);
	this->hsbYUVSat = ui->NewHScrollBar(this->tpYUV, 16);
	this->hsbYUVSat->SetRect(128, 56, 392, 17, false);
	this->hsbYUVSat->InitScrollBar(0, 4099, 1000, 100);
	this->hsbYUVSat->HandlePosChanged(OnYUVSatChg, this);
	this->lblYUVSatV = ui->NewLabel(this->tpYUV, CSTR(""));
	this->lblYUVSatV->SetRect(520, 56, 64, 16, false);
	this->lblYUVYGamma = ui->NewLabel(this->tpYUV, CSTR("Y-Gamma"));
	this->lblYUVYGamma->SetRect(8, 80, 120, 16, false);
	this->hsbYUVYGamma = ui->NewHScrollBar(this->tpYUV, 16);
	this->hsbYUVYGamma->SetRect(128, 80, 392, 17, false);
	this->hsbYUVYGamma->InitScrollBar(0, 4099, 1000, 100);
	this->hsbYUVYGamma->HandlePosChanged(OnYUVYGammaChg, this);
	this->lblYUVYGammaV = ui->NewLabel(this->tpYUV, CSTR(""));
	this->lblYUVYGammaV->SetRect(520, 80, 64, 16, false);
	this->lblYUVCGamma = ui->NewLabel(this->tpYUV, CSTR("C-Gamma"));
	this->lblYUVCGamma->SetRect(8, 104, 120, 16, false);
	this->hsbYUVCGamma = ui->NewHScrollBar(this->tpYUV, 16);
	this->hsbYUVCGamma->SetRect(128, 104, 392, 17, false);
	this->hsbYUVCGamma->InitScrollBar(0, 4099, 1000, 100);
	this->hsbYUVCGamma->HandlePosChanged(OnYUVCGammaChg, this);
	this->lblYUVCGammaV = ui->NewLabel(this->tpYUV, CSTR(""));
	this->lblYUVCGammaV->SetRect(520, 104, 64, 16, false);

	this->SetDefaultButton(this->btnOK);
	this->SetCancelButton(this->btnCancel);


	this->hsbCalRedB->SetPos((UOSInt)Double2OSInt(rgbParam->MonRBright  * 1000));
	this->hsbCalRedC->SetPos((UOSInt)Double2OSInt(rgbParam->MonRContr * 1000));
	this->hsbCalRedG->SetPos((UOSInt)Double2OSInt(rgbParam->MonRGamma * 1000));
	this->hsbCalGreenB->SetPos((UOSInt)Double2OSInt(rgbParam->MonGBright  * 1000));
	this->hsbCalGreenC->SetPos((UOSInt)Double2OSInt(rgbParam->MonGContr * 1000));
	this->hsbCalGreenG->SetPos((UOSInt)Double2OSInt(rgbParam->MonGGamma * 1000));
	this->hsbCalBlueB->SetPos((UOSInt)Double2OSInt(rgbParam->MonBBright  * 1000));
	this->hsbCalBlueC->SetPos((UOSInt)Double2OSInt(rgbParam->MonBContr * 1000));
	this->hsbCalBlueG->SetPos((UOSInt)Double2OSInt(rgbParam->MonBGamma * 1000));

	this->hsbMonVBrig->SetPos((UOSInt)Double2OSInt(rgbParam->MonVBrightness * 1000));
	this->hsbMonPBrig->SetPos((UOSInt)Double2OSInt(rgbParam->MonPBrightness * 1000));
	this->hsbMonRBrig->SetPos((UOSInt)Double2OSInt(rgbParam->MonRBrightness * 1000));
	this->hsbMonGBrig->SetPos((UOSInt)Double2OSInt(rgbParam->MonGBrightness * 1000));
	this->hsbMonBBrig->SetPos((UOSInt)Double2OSInt(rgbParam->MonBBrightness * 1000));

	this->hsbYUVBrig->SetPos((UOSInt)Double2OSInt(yuvParam->Brightness * 1000));
	this->hsbYUVCont->SetPos((UOSInt)Double2OSInt(yuvParam->Contrast * 1000));
	this->hsbYUVSat->SetPos((UOSInt)Double2OSInt(yuvParam->Saturation * 1000));
	this->hsbYUVYGamma->SetPos((UOSInt)Double2OSInt(yuvParam->YGamma * 1000));
	this->hsbYUVCGamma->SetPos((UOSInt)Double2OSInt(yuvParam->CGamma * 1000));
	switch (this->colorMgr->GetDefYUVType())
	{
	default:
	case Media::ColorProfile::YUVT_BT601:
		this->radYUVDefBT601->Select();
		break;
	case Media::ColorProfile::YUVT_BT709:
		this->radYUVDefBT709->Select();
		break;
	case Media::ColorProfile::YUVT_FCC:
		this->radYUVDefFCC->Select();
		break;
	case Media::ColorProfile::YUVT_BT470BG:
		this->radYUVDefBT470BG->Select();
		break;
	case Media::ColorProfile::YUVT_SMPTE170M:
		this->radYUVDefSMPTE170M->Select();
		break;
	case Media::ColorProfile::YUVT_SMPTE240M:
		this->radYUVDefSMPTE240M->Select();
		break;
	case Media::ColorProfile::YUVT_BT2020:
	case Media::ColorProfile::YUVT_FLAG_YUV_0_255:
	case Media::ColorProfile::YUVT_UNKNOWN:
		break;
	}
	sptr = Text::StrDouble(sbuff, rgbParam->monLuminance);
	this->txtLuminance->SetText(CSTRP(sbuff, sptr));

	this->inited = true;
	OnMonProfileChg(this);
}

SSWR::AVIRead::AVIRColorSettingForm::~AVIRColorSettingForm()
{
	if (!this->normalClose)
	{
		this->monColor->Load();
	}
	OPTSTR_DEL(this->monFileName);
}

void SSWR::AVIRead::AVIRColorSettingForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
