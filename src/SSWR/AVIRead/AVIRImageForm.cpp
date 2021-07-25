#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Math/Math.h"
#include "Media/ICCProfile.h"
#include "Media/CS/TransferFunc.h"
#include "SSWR/AVIRead/AVIRICCInfoForm.h"
#include "SSWR/AVIRead/AVIRImageColorForm.h"
#include "SSWR/AVIRead/AVIRImageForm.h"
#include "SSWR/AVIRead/AVIRImageGRForm.h"
#include "SSWR/AVIRead/AVIRImageResizeForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

typedef enum
{
	MNU_IMAGE_SAVE = 101,
	MNU_IMAGE_ENLARGE,
	MNU_FILTER_COLOR,
	MNU_FILTER_32BIT,
	MNU_FILTER_64BIT,
	MNU_FILTER_PAL8,
	MNU_FILTER_GR,
	MNU_FILTER_RESIZE
} MenuEvent;

void __stdcall SSWR::AVIRead::AVIRImageForm::ImagesSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRImageForm *me = (SSWR::AVIRead::AVIRImageForm *)userObj;
	UOSInt selInd = me->lbImages->GetSelectedIndex();
	Media::Image *img = me->imgList->GetImage(selInd, &me->currImgDelay);
	me->pbImage->SetImage(img, false);
	me->currImg = img;
	me->UpdateInfo();
}

Bool __stdcall SSWR::AVIRead::AVIRImageForm::OnImageMouseMove(void *userObj, OSInt scnX, OSInt scnY, MouseButton btn)
{
	SSWR::AVIRead::AVIRImageForm *me = (SSWR::AVIRead::AVIRImageForm *)userObj;
	if (me->currImg)
	{
		Double imgX;
		Double imgY;
		Double dR;
		Double dG;
		Double dB;
		UInt8 pixel[16];
		Text::StringBuilderUTF8 sb;
		me->pbImage->Scn2ImagePos(scnX, scnY, &imgX, &imgY);
		OSInt xPos = Math::Double2Int32(imgX);
		OSInt yPos = Math::Double2Int32(imgY);
		if (xPos < 0)
			xPos = 0;
		else if ((UOSInt)xPos >= me->currImg->info->dispWidth)
			xPos = (OSInt)me->currImg->info->dispWidth - 1;
		if (yPos < 0)
			yPos = 0;
		else if ((UOSInt)yPos >= me->currImg->info->dispHeight)
			yPos = (OSInt)me->currImg->info->dispHeight - 1;
		me->currImg->GetImageData(pixel, xPos, yPos, 1, 1, 16, false);
		sb.Append((const UTF8Char*)"(x, y) = (");
		sb.AppendOSInt(xPos);
		sb.Append((const UTF8Char*)", ");
		sb.AppendOSInt(yPos);
		sb.Append((const UTF8Char*)")");
		if (me->currImg->info->pf == Media::PF_PAL_1 || me->currImg->info->pf == Media::PF_PAL_W1)
		{
			UInt8 i = 0;
			UInt8 *p;
			switch (xPos & 7)
			{
			case 0:
				i = (UInt8)(pixel[0] >> 7);
				break;
			case 1:
				i = (UInt8)((pixel[0] >> 6) & 1);
				break;
			case 2:
				i = (UInt8)((pixel[0] >> 5) & 1);
				break;
			case 3:
				i = (UInt8)((pixel[0] >> 4) & 1);
				break;
			case 4:
				i = (UInt8)((pixel[0] >> 3) & 1);
				break;
			case 5:
				i = (UInt8)((pixel[0] >> 2) & 1);
				break;
			case 6:
				i = (UInt8)((pixel[0] >> 1) & 1);
				break;
			case 7:
				i = (UInt8)(pixel[0] & 1);
				break;
			}
			sb.Append((const UTF8Char*)", I");
			sb.AppendU32(i);
			p = &me->currImg->pal[i * 4];
			sb.Append((const UTF8Char*)" (A");
			sb.AppendU32(p[3]);
			sb.Append((const UTF8Char*)"R");
			sb.AppendU32(p[2]);
			sb.Append((const UTF8Char*)"G");
			sb.AppendU32(p[1]);
			sb.Append((const UTF8Char*)"B");
			sb.AppendU32(p[0]);
			sb.Append((const UTF8Char*)")");
			dR = p[2] / 255.0;
			dG = p[1] / 255.0;
			dB = p[0] / 255.0;
		}
		else if (me->currImg->info->pf == Media::PF_PAL_2 || me->currImg->info->pf == Media::PF_PAL_W2)
		{
			UInt8 i = 0;
			UInt8 *p;
			switch (xPos & 3)
			{
			case 0:
				i = (UInt8)(pixel[0] >> 6);
				break;
			case 1:
				i = (UInt8)((pixel[0] >> 4) & 3);
				break;
			case 2:
				i = (UInt8)((pixel[0] >> 2) & 3);
				break;
			case 3:
				i = (UInt8)(pixel[0] & 3);
				break;
			}
			sb.Append((const UTF8Char*)", I");
			sb.AppendU32(i);
			p = &me->currImg->pal[i * 4];
			sb.Append((const UTF8Char*)" (A");
			sb.AppendU32(p[3]);
			sb.Append((const UTF8Char*)"R");
			sb.AppendU32(p[2]);
			sb.Append((const UTF8Char*)"G");
			sb.AppendU32(p[1]);
			sb.Append((const UTF8Char*)"B");
			sb.AppendU32(p[0]);
			sb.Append((const UTF8Char*)")");
			dR = p[2] / 255.0;
			dG = p[1] / 255.0;
			dB = p[0] / 255.0;
		}
		else if (me->currImg->info->pf == Media::PF_PAL_4 || me->currImg->info->pf == Media::PF_PAL_W4)
		{
			UInt8 i = 0;
			UInt8 *p;
			switch (xPos & 1)
			{
			case 0:
				i = (UInt8)(pixel[0] >> 4);
				break;
			case 1:
				i = (UInt8)(pixel[0] & 15);
				break;
			}
			sb.Append((const UTF8Char*)", I");
			sb.AppendU32(i);
			p = &me->currImg->pal[i * 4];
			sb.Append((const UTF8Char*)" (A");
			sb.AppendU32(p[3]);
			sb.Append((const UTF8Char*)"R");
			sb.AppendU32(p[2]);
			sb.Append((const UTF8Char*)"G");
			sb.AppendU32(p[1]);
			sb.Append((const UTF8Char*)"B");
			sb.AppendU32(p[0]);
			sb.Append((const UTF8Char*)")");
			dR = p[2] / 255.0;
			dG = p[1] / 255.0;
			dB = p[0] / 255.0;
		}
		else if (me->currImg->info->pf == Media::PF_PAL_8 || me->currImg->info->pf == Media::PF_PAL_W8)
		{
			UInt8 *p;
			sb.Append((const UTF8Char*)", I");
			sb.AppendU32(pixel[0]);
			p = &me->currImg->pal[pixel[0] * 4];
			sb.Append((const UTF8Char*)" (A");
			sb.AppendU32(p[3]);
			sb.Append((const UTF8Char*)"R");
			sb.AppendU32(p[2]);
			sb.Append((const UTF8Char*)"G");
			sb.AppendU32(p[1]);
			sb.Append((const UTF8Char*)"B");
			sb.AppendU32(p[0]);
			sb.Append((const UTF8Char*)")");
			dR = p[2] / 255.0;
			dG = p[1] / 255.0;
			dB = p[0] / 255.0;
		}
		else if (me->currImg->info->pf == Media::PF_PAL_1_A1)
		{
			UInt8 i = 0;
			UInt8 a = 0;
			UInt8 *p;
			switch (xPos & 7)
			{
			case 0:
				i = (UInt8)(pixel[0] >> 7);
				a = (UInt8)(pixel[1] >> 7);
				break;
			case 1:
				i = (UInt8)((pixel[0] >> 6) & 1);
				a = (UInt8)((pixel[1] >> 6) & 1);
				break;
			case 2:
				i = (UInt8)((pixel[0] >> 5) & 1);
				a = (UInt8)((pixel[1] >> 5) & 1);
				break;
			case 3:
				i = (UInt8)((pixel[0] >> 4) & 1);
				a = (UInt8)((pixel[1] >> 4) & 1);
				break;
			case 4:
				i = (UInt8)((pixel[0] >> 3) & 1);
				a = (UInt8)((pixel[1] >> 3) & 1);
				break;
			case 5:
				i = (UInt8)((pixel[0] >> 2) & 1);
				a = (UInt8)((pixel[1] >> 2) & 1);
				break;
			case 6:
				i = (UInt8)((pixel[0] >> 1) & 1);
				a = (UInt8)((pixel[1] >> 1) & 1);
				break;
			case 7:
				i = (UInt8)(pixel[0] & 1);
				a = (UInt8)(pixel[1] & 1);
				break;
			}
			sb.Append((const UTF8Char*)", I");
			sb.AppendU32(i);
			p = &me->currImg->pal[i * 4];
			sb.Append((const UTF8Char*)" (R");
			sb.AppendU32(p[2]);
			sb.Append((const UTF8Char*)"G");
			sb.AppendU32(p[1]);
			sb.Append((const UTF8Char*)"B");
			sb.AppendU32(p[0]);
			sb.Append((const UTF8Char*)"), A");
			sb.AppendU32(a);
			dR = p[2] / 255.0;
			dG = p[1] / 255.0;
			dB = p[0] / 255.0;
		}
		else if (me->currImg->info->pf == Media::PF_PAL_2_A1)
		{
			UInt8 i = 0;
			UInt8 a = 0;
			UInt8 *p;
			switch (xPos & 3)
			{
			case 0:
				i = (UInt8)(pixel[0] >> 6);
				break;
			case 1:
				i = (UInt8)((pixel[0] >> 4) & 3);
				break;
			case 2:
				i = (UInt8)((pixel[0] >> 2) & 3);
				break;
			case 3:
				i = (UInt8)(pixel[0] & 3);
				break;
			}
			switch (xPos & 7)
			{
			case 0:
				a = (UInt8)(pixel[1] >> 7);
				break;
			case 1:
				a = (UInt8)((pixel[1] >> 6) & 1);
				break;
			case 2:
				a = (UInt8)((pixel[1] >> 5) & 1);
				break;
			case 3:
				a = (UInt8)((pixel[1] >> 4) & 1);
				break;
			case 4:
				a = (UInt8)((pixel[1] >> 3) & 1);
				break;
			case 5:
				a = (UInt8)((pixel[1] >> 2) & 1);
				break;
			case 6:
				a = (UInt8)((pixel[1] >> 1) & 1);
				break;
			case 7:
				a = (UInt8)(pixel[1] & 1);
				break;
			}
			sb.Append((const UTF8Char*)", I");
			sb.AppendU32(i);
			p = &me->currImg->pal[i * 4];
			sb.Append((const UTF8Char*)" (R");
			sb.AppendU32(p[2]);
			sb.Append((const UTF8Char*)"G");
			sb.AppendU32(p[1]);
			sb.Append((const UTF8Char*)"B");
			sb.AppendU32(p[0]);
			sb.Append((const UTF8Char*)"), A");
			sb.AppendU32(a);
			dR = p[2] / 255.0;
			dG = p[1] / 255.0;
			dB = p[0] / 255.0;
		}
		else if (me->currImg->info->pf == Media::PF_PAL_4_A1)
		{
			UInt8 i = 0;
			UInt8 a = 0;
			UInt8 *p;
			switch (xPos & 1)
			{
			case 0:
				i = (UInt8)(pixel[0] >> 4);
				break;
			case 1:
				i = (UInt8)(pixel[0] & 15);
				break;
			}
			switch (xPos & 7)
			{
			case 0:
				a = (UInt8)(pixel[1] >> 7);
				break;
			case 1:
				a = (UInt8)((pixel[1] >> 6) & 1);
				break;
			case 2:
				a = (UInt8)((pixel[1] >> 5) & 1);
				break;
			case 3:
				a = (UInt8)((pixel[1] >> 4) & 1);
				break;
			case 4:
				a = (UInt8)((pixel[1] >> 3) & 1);
				break;
			case 5:
				a = (UInt8)((pixel[1] >> 2) & 1);
				break;
			case 6:
				a = (UInt8)((pixel[1] >> 1) & 1);
				break;
			case 7:
				a = (UInt8)(pixel[1] & 1);
				break;
			}
			sb.Append((const UTF8Char*)", I");
			sb.AppendU32(i);
			p = &me->currImg->pal[i * 4];
			sb.Append((const UTF8Char*)" (R");
			sb.AppendU32(p[2]);
			sb.Append((const UTF8Char*)"G");
			sb.AppendU32(p[1]);
			sb.Append((const UTF8Char*)"B");
			sb.AppendU32(p[0]);
			sb.Append((const UTF8Char*)"), A");
			sb.AppendU32(a);
			dR = p[2] / 255.0;
			dG = p[1] / 255.0;
			dB = p[0] / 255.0;
		}
		else if (me->currImg->info->pf == Media::PF_PAL_8_A1)
		{
			UInt8 *p;
			UInt8 a = 0;
			sb.Append((const UTF8Char*)", I");
			sb.AppendU32(pixel[0]);
			p = &me->currImg->pal[pixel[0] * 4];
			sb.Append((const UTF8Char*)" (R");
			sb.AppendU32(p[2]);
			sb.Append((const UTF8Char*)"G");
			sb.AppendU32(p[1]);
			sb.Append((const UTF8Char*)"B");
			sb.AppendU32(p[0]);
			sb.Append((const UTF8Char*)"), A");
			sb.AppendU32(a);
			dR = p[2] / 255.0;
			dG = p[1] / 255.0;
			dB = p[0] / 255.0;
		}
		else if (me->currImg->info->pf == Media::PF_LE_R5G5B5)
		{
			sb.Append((const UTF8Char*)", R");
			UInt16 p = ReadUInt16(pixel);
			sb.AppendU32((p >> 10) & 0x1f);
			sb.Append((const UTF8Char*)"G");
			sb.AppendU32((p >> 5) & 0x1f);
			sb.Append((const UTF8Char*)"B");
			sb.AppendU32(p & 0x1f);
			dR = ((p >> 10) & 0x1f) / 31.0;
			dG = ((p >> 5) & 0x1f) / 31.0;
			dB = (p & 0x1f) / 31.0;
		}
		else if (me->currImg->info->pf == Media::PF_LE_R5G6B5)
		{
			sb.Append((const UTF8Char*)", R");
			UInt16 p = ReadUInt16(pixel);
			sb.AppendU32((p >> 11) & 0x1f);
			sb.Append((const UTF8Char*)"G");
			sb.AppendU32((p >> 5) & 0x3f);
			sb.Append((const UTF8Char*)"B");
			sb.AppendU32(p & 0x1f);
			dR = ((p >> 11) & 0x1f) / 31.0;
			dG = ((p >> 5) & 0x3f) / 63.0;
			dB = (p & 0x1f) / 31.0;
		}
		else if (me->currImg->info->pf == Media::PF_B8G8R8)
		{
			sb.Append((const UTF8Char*)", R");
			sb.AppendU32(pixel[2]);
			sb.Append((const UTF8Char*)"G");
			sb.AppendU32(pixel[1]);
			sb.Append((const UTF8Char*)"B");
			sb.AppendU32(pixel[0]);
			dR = pixel[2] / 255.0;
			dG = pixel[1] / 255.0;
			dB = pixel[0] / 255.0;
		}
		else if (me->currImg->info->pf == Media::PF_B8G8R8A8)
		{
			sb.Append((const UTF8Char*)", A");
			sb.AppendU32(pixel[3]);
			sb.Append((const UTF8Char*)"R");
			sb.AppendU32(pixel[2]);
			sb.Append((const UTF8Char*)"B");
			sb.AppendU32(pixel[1]);
			sb.Append((const UTF8Char*)"B");
			sb.AppendU32(pixel[0]);
			dR = pixel[2] / 255.0;
			dG = pixel[1] / 255.0;
			dB = pixel[0] / 255.0;
		}
		else if (me->currImg->info->pf == Media::PF_LE_B16G16R16)
		{
			sb.Append((const UTF8Char*)", R");
			sb.AppendU32(ReadUInt16(&pixel[4]));
			sb.Append((const UTF8Char*)"G");
			sb.AppendU32(ReadUInt16(&pixel[2]));
			sb.Append((const UTF8Char*)"B");
			sb.AppendU32(ReadUInt16(&pixel[0]));
			dR = ReadUInt16(&pixel[4]) / 65535.0;
			dG = ReadUInt16(&pixel[2]) / 65535.0;
			dB = ReadUInt16(&pixel[0]) / 65535.0;
		}
		else if (me->currImg->info->pf == Media::PF_LE_B16G16R16A16)
		{
			sb.Append((const UTF8Char*)", A");
			sb.AppendU32(ReadUInt16(&pixel[6]));
			sb.Append((const UTF8Char*)"R");
			sb.AppendU32(ReadUInt16(&pixel[4]));
			sb.Append((const UTF8Char*)"G");
			sb.AppendU32(ReadUInt16(&pixel[2]));
			sb.Append((const UTF8Char*)"B");
			sb.AppendU32(ReadUInt16(&pixel[0]));
			dR = ReadUInt16(&pixel[4]) / 65535.0;
			dG = ReadUInt16(&pixel[2]) / 65535.0;
			dB = ReadUInt16(&pixel[0]) / 65535.0;
		}
		else if (me->currImg->info->pf == Media::PF_LE_A2B10G10R10)
		{
			UInt32 p = ReadUInt32(&pixel[0]);
			sb.Append((const UTF8Char*)", A");
			sb.AppendU32((p >> 30) & 3);
			sb.Append((const UTF8Char*)"R");
			sb.AppendU32(p & 0x3ff);
			sb.Append((const UTF8Char*)"G");
			sb.AppendU32((p >> 10) & 0x3ff);
			sb.Append((const UTF8Char*)"B");
			sb.AppendU32((p >> 20) & 0x3ff);
			dR = (p & 0x3ff) / 1023.0;
			dG = ((p >> 10) & 0x3ff) / 1023.0;
			dB = ((p >> 20) & 0x3ff) / 1023.0;
		}
		else if (me->currImg->info->pf == Media::PF_W8A8)
		{
			sb.Append((const UTF8Char*)", A");
			sb.AppendU32(pixel[1]);
			dR = pixel[0] / 255.0;
			dG = dR;
			dB = dR;
		}
		else if (me->currImg->info->pf == Media::PF_LE_W16)
		{
			sb.Append((const UTF8Char*)", W");
			sb.AppendU32(ReadUInt16(&pixel[0]));
			dR = ReadUInt16(&pixel[0]) / 65535.0;
			dG = dR;
			dB = dR;
		}
		else if (me->currImg->info->pf == Media::PF_LE_W16A16)
		{
			sb.Append((const UTF8Char*)", A");
			sb.AppendU32(ReadUInt16(&pixel[2]));
			sb.Append((const UTF8Char*)", W");
			sb.AppendU32(ReadUInt16(&pixel[0]));
			dR = ReadUInt16(&pixel[0]) / 65535.0;
			dG = dR;
			dB = dR;
		}
		else if (me->currImg->info->pf == Media::PF_LE_FB32G32R32A32)
		{
			sb.Append((const UTF8Char*)", A");
			Text::SBAppendF32(&sb, ReadFloat(&pixel[12]));
			sb.Append((const UTF8Char*)" R");
			Text::SBAppendF32(&sb, ReadFloat(&pixel[8]));
			sb.Append((const UTF8Char*)" G");
			Text::SBAppendF32(&sb, ReadFloat(&pixel[4]));
			sb.Append((const UTF8Char*)" B");
			Text::SBAppendF32(&sb, ReadFloat(&pixel[0]));
			dR = ReadFloat(&pixel[8]);
			dG = ReadFloat(&pixel[4]);
			dB = ReadFloat(&pixel[0]);
		}
		else
		{
			dR = 0;
			dG = 0;
			dB = 0;
		}
		if (me->imgList && me->imgList->HasThermoImage())
		{
			sb.Append((const UTF8Char*)", T = ");
			Text::SBAppendF64(&sb, me->imgList->GetThermoValue(imgX / Math::UOSInt2Double(me->currImg->info->dispWidth), imgY / Math::UOSInt2Double(me->currImg->info->dispHeight)));
		}
		sb.Append((const UTF8Char*)", RGB(");
		Text::SBAppendF64(&sb, dR);
		sb.AppendChar(',', 1);
		Text::SBAppendF64(&sb, dG);
		sb.AppendChar(',', 1);
		Text::SBAppendF64(&sb, dB);
		sb.AppendChar(')', 1);
		me->txtImageStatus->SetText(sb.ToString());
	}
	else
	{
		me->txtImageStatus->SetText((const UTF8Char*)"");
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRImageForm::OnInfoICCClicked(void *userObj)
{
	SSWR::AVIRead::AVIRImageForm *me = (SSWR::AVIRead::AVIRImageForm *)userObj;
	if (me->currImg)
	{
		const UInt8 *iccBuff = me->currImg->info->color->rawICC;
		if (iccBuff)
		{
			Media::ICCProfile *icc = Media::ICCProfile::Parse(iccBuff, ReadMUInt32(iccBuff));
			if (icc)
			{
				SSWR::AVIRead::AVIRICCInfoForm *frm;
				NEW_CLASS(frm, SSWR::AVIRead::AVIRICCInfoForm(0, me->ui, me->core));
				frm->SetICCProfile(icc, me->imgList->GetSourceNameObj());
				me->core->ShowForm(frm);
			}
		}
	}
}

void SSWR::AVIRead::AVIRImageForm::UpdateInfo()
{
	if (this->currImg)
	{
		Text::StringBuilderUTF8 sb;
		this->currImg->ToString(&sb);
		sb.Append((const UTF8Char*)"\r\nDelay: ");
		sb.AppendU32(this->currImgDelay);
		if (this->imgList)
		{
			sb.Append((const UTF8Char*)"\r\n");
			this->imgList->ToValueString(&sb);
		}
		this->txtInfo->SetText(sb.ToString());
		if (this->currImg->info->color->GetRAWICC())
		{
			this->btnInfoICC->SetEnabled(true);
		}
		else
		{
			this->btnInfoICC->SetEnabled(false);
		}
	}
	else
	{
		this->txtInfo->SetText((const UTF8Char*)"");
		this->btnInfoICC->SetEnabled(false);
	}
}

SSWR::AVIRead::AVIRImageForm::AVIRImageForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Media::ImageList *imgList) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	UTF8Char sbuff[512];
	Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"Image Form - "), imgList->GetSourceNameObj());
	this->SetText(sbuff);
	this->SetFormState(UI::GUIForm::FS_MAXIMIZED);

	this->core = core;
	this->colorSess = this->core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->imgList = imgList;
	this->allowEnlarge = false;
	this->currImg = 0;
	this->currImgDelay = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	
	NEW_CLASS(this->lbImages, UI::GUIListBox(ui, this, false));
	this->lbImages->SetRect(0, 0, 160, 10, false);
	this->lbImages->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbImages->HandleSelectionChange(ImagesSelChg, this);
	NEW_CLASS(this->hSplitter, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->tcImage, UI::GUITabControl(ui, this));
	this->tcImage->SetDockType(UI::GUIControl::DOCK_FILL);
	
	this->tpImage = this->tcImage->AddTabPage((const UTF8Char*)"Image");
	NEW_CLASS(this->txtImageStatus, UI::GUITextBox(ui, this->tpImage, (const UTF8Char*)""));
	this->txtImageStatus->SetRect(0, 0, 100, 23, false);
	this->txtImageStatus->SetReadOnly(true);
	this->txtImageStatus->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->pbImage, UI::GUIPictureBoxDD(ui, this->tpImage, this->colorSess, this->allowEnlarge, false));
	this->pbImage->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pbImage->HandleMouseMove(OnImageMouseMove, this);
	this->tpInfo = this->tcImage->AddTabPage((const UTF8Char*)"Info");
	NEW_CLASS(this->pnlInfo, UI::GUIPanel(ui, this->tpInfo));
	this->pnlInfo->SetRect(0, 0, 100, 31, false);
	this->pnlInfo->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->btnInfoICC, UI::GUIButton(ui, this->pnlInfo, (const UTF8Char*)"ICC Profile"));
	this->btnInfoICC->SetRect(4, 4, 100, 23, false);
	this->btnInfoICC->SetEnabled(false);
	this->btnInfoICC->HandleButtonClick(OnInfoICCClicked, this);
	NEW_CLASS(this->txtInfo, UI::GUITextBox(ui, this->tpInfo, (const UTF8Char*)"", true));
	this->txtInfo->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtInfo->SetReadOnly(true);
	
	UI::GUIMenu *mnu;
	NEW_CLASS(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu((const UTF8Char*)"&Image");
	mnu->AddItem((const UTF8Char*)"&Save", MNU_IMAGE_SAVE, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_S);
	mnu->AddItem((const UTF8Char*)"&Allow Enlarge", MNU_IMAGE_ENLARGE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu = this->mnuMain->AddSubMenu((const UTF8Char*)"&Filter");
	mnu->AddItem((const UTF8Char*)"&Color", MNU_FILTER_COLOR, (UI::GUIMenu::KeyModifier)(UI::GUIMenu::KM_CONTROL | UI::GUIMenu::KM_SHIFT), UI::GUIControl::GK_C);
	mnu->AddItem((const UTF8Char*)"&Ghost Reduction", MNU_FILTER_GR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"&Resize", MNU_FILTER_RESIZE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"To 32bpp", MNU_FILTER_32BIT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"To 64bpp", MNU_FILTER_64BIT, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"To 8bpp (Palette)", MNU_FILTER_PAL8, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->SetMenu(this->mnuMain);

	UOSInt i = 0;
	UOSInt j = this->imgList->GetCount();
	while (i < j)
	{
		Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"Image"), i);
		this->lbImages->AddItem(sbuff, 0);
		i++;
	}
	if (j > 0)
	{
		this->lbImages->SetSelectedIndex(0);
	}
}

SSWR::AVIRead::AVIRImageForm::~AVIRImageForm()
{
	DEL_CLASS(this->imgList);
	this->ClearChildren();
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
}

void SSWR::AVIRead::AVIRImageForm::EventMenuClicked(UInt16 cmdId)
{
	switch (cmdId)
	{
	case MNU_IMAGE_SAVE:
		this->core->SaveData(this, this->imgList, L"SaveImage");
		break;
	case MNU_IMAGE_ENLARGE:
		this->allowEnlarge = !this->allowEnlarge;
		this->pbImage->SetAllowEnlarge(this->allowEnlarge);
		break;
	case MNU_FILTER_COLOR:
		{
			UOSInt selInd = this->lbImages->GetSelectedIndex();
			Media::Image *img = this->imgList->GetImage(selInd, 0);
			if (img)
			{
				Media::StaticImage *buffImg = img->CreateStaticImage();
				Media::StaticImage *prevImg = img->CreateStaticImage();

				this->pbImage->SetImage(prevImg, true);

				SSWR::AVIRead::AVIRImageColorForm *frm;
				NEW_CLASS(frm, SSWR::AVIRead::AVIRImageColorForm(0, this->ui, this->core, buffImg, prevImg, this->pbImage));
				UI::GUIForm::DialogResult dr = frm->ShowDialog(this);
				
				if (dr == UI::GUIForm::DR_OK)
				{
					this->imgList->ReplaceImage((UOSInt)selInd, prevImg);
					DEL_CLASS(buffImg);
				}
				else
				{
					DEL_CLASS(prevImg);
					DEL_CLASS(buffImg);
					this->pbImage->SetImage(img, true);
				}
				DEL_CLASS(frm);
				this->currImg = img;
				this->UpdateInfo();
			}
		}
		break;
	case MNU_FILTER_GR:
		{
			UOSInt selInd = this->lbImages->GetSelectedIndex();
			Media::Image *img = this->imgList->GetImage(selInd, 0);
			if (img)
			{
				Bool valid = false;
				if (img->info->storeBPP == 32 && img->info->pf == Media::PF_B8G8R8A8)
				{
					valid = true;
				}
				
				if (valid)
				{
					Media::StaticImage *buffImg = img->CreateStaticImage();
					Media::StaticImage *prevImg = img->CreateStaticImage();

					this->pbImage->SetImage(prevImg, true);

					SSWR::AVIRead::AVIRImageGRForm *frm;
					NEW_CLASS(frm, SSWR::AVIRead::AVIRImageGRForm(0, this->ui, this->core, buffImg, prevImg, this->pbImage));
					UI::GUIForm::DialogResult dr = frm->ShowDialog(this);
					
					if (dr == UI::GUIForm::DR_OK)
					{
						this->imgList->ReplaceImage((UOSInt)selInd, prevImg);
						DEL_CLASS(buffImg);
						img = prevImg;
						this->pbImage->SetImage(prevImg, true);
					}
					else
					{
						DEL_CLASS(prevImg);
						DEL_CLASS(buffImg);
						this->pbImage->SetImage(img, true);
					}
					DEL_CLASS(frm);
					this->currImg = img;
					this->UpdateInfo();
				}
			}
		}
		break;
	case MNU_FILTER_RESIZE:
		{
			UOSInt selInd = this->lbImages->GetSelectedIndex();
			Media::Image *img = this->imgList->GetImage(selInd, 0);
			if (img)
			{
				Bool valid = false;
				if (img->info->pf == Media::PF_B8G8R8A8 || img->info->pf == Media::PF_LE_B16G16R16A16)
				{
					valid = true;
				}
				
				if (valid)
				{
					SSWR::AVIRead::AVIRImageResizeForm *frm;
					NEW_CLASS(frm, SSWR::AVIRead::AVIRImageResizeForm(0, this->ui, this->core, img));
					UI::GUIForm::DialogResult dr = frm->ShowDialog(this);
					
					if (dr == UI::GUIForm::DR_OK)
					{
						img = frm->GetNewImage();
						this->pbImage->SetImage(0, false);
						this->imgList->ReplaceImage((UOSInt)selInd, img);
						this->pbImage->SetImage(img, false);
						this->currImg = img;
						this->UpdateInfo();
					}
					else
					{
					}
					DEL_CLASS(frm);
				}
			}
		}
		break;
	case MNU_FILTER_32BIT:
		{
			UOSInt selInd = this->lbImages->GetSelectedIndex();
			Media::Image *img = this->imgList->GetImage(selInd, 0);
			if (img)
			{
				Media::StaticImage *simg = img->CreateStaticImage();
				simg->To32bpp();
				if (this->currImg == img)
				{
					this->pbImage->SetImage(simg, true);
				}
				this->imgList->ReplaceImage(selInd, simg);
				this->currImg = simg;
				this->UpdateInfo();
			}
		}
		break;
	case MNU_FILTER_64BIT:
		{
			UOSInt selInd = this->lbImages->GetSelectedIndex();
			Media::Image *img = this->imgList->GetImage(selInd, 0);
			if (img)
			{
				Media::StaticImage *simg = img->CreateStaticImage();
				simg->To64bpp();
				if (this->currImg == img)
				{
					this->pbImage->SetImage(simg, true);
				}
				this->imgList->ReplaceImage(selInd, simg);
				this->currImg = simg;
				this->UpdateInfo();
			}
		}
		break;
	case MNU_FILTER_PAL8:
		{
			UOSInt selInd = this->lbImages->GetSelectedIndex();
			Media::Image *img = this->imgList->GetImage(selInd, 0);
			if (img)
			{
				Media::StaticImage *simg = img->CreateStaticImage();
				if (simg->ToPal8())
				{
					if (this->currImg == img)
					{
						this->pbImage->SetImage(simg, true);
					}
					this->imgList->ReplaceImage(selInd, simg);
					this->currImg = simg;
					this->UpdateInfo();
				}
				else
				{
					DEL_CLASS(simg);
				}
			}
		}
		break;
	}
}

void SSWR::AVIRead::AVIRImageForm::OnMonitorChanged()
{
	this->colorSess->ChangeMonitor(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
