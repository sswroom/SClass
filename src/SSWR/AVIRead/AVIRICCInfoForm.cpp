#include "Stdafx.h"
#include "IO/FileStream.h"
#include "Media/ICCProfile.h"
#include "Media/LUT.h"
#include "SSWR/AVIRead/AVIRICCInfoForm.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRICCInfoForm::OnFileDrop(void *userObj, const UTF8Char **files, OSInt fileCnt)
{
	SSWR::AVIRead::AVIRICCInfoForm *me = (SSWR::AVIRead::AVIRICCInfoForm*)userObj;
	OSInt i;
	IO::FileStream *fs;
	UInt8 *buff = MemAlloc(UInt8, 1048576);
	OSInt fileSize;
	i = 0;
	while (i < fileCnt)
	{
		NEW_CLASS(fs, IO::FileStream(files[i], IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NO_BUFFER));
		fileSize = fs->Read(buff, 1048576);
		DEL_CLASS(fs);
		if (fileSize == 0 || fileSize >= 1048576)
		{
		}
		else
		{
			Media::ICCProfile *icc = Media::ICCProfile::Parse(buff, fileSize);
			if (icc)
			{
				me->SetICCProfile(icc, files[i]);
				break;
			}
		}
		i++;
	}
	MemFree(buff);
}

void __stdcall SSWR::AVIRead::AVIRICCInfoForm::OnRLUTClicked(void *userObj)
{
	SSWR::AVIRead::AVIRICCInfoForm *me = (SSWR::AVIRead::AVIRICCInfoForm*)userObj;
	if (me->icc)
	{
		Media::LUT *lut = me->icc->CreateRLUT();
		if (lut)
		{
			me->core->OpenObject(lut);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRICCInfoForm::OnGLUTClicked(void *userObj)
{
	SSWR::AVIRead::AVIRICCInfoForm *me = (SSWR::AVIRead::AVIRICCInfoForm*)userObj;
	if (me->icc)
	{
		Media::LUT *lut = me->icc->CreateGLUT();
		if (lut)
		{
			me->core->OpenObject(lut);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRICCInfoForm::OnBLUTClicked(void *userObj)
{
	SSWR::AVIRead::AVIRICCInfoForm *me = (SSWR::AVIRead::AVIRICCInfoForm*)userObj;
	if (me->icc)
	{
		Media::LUT *lut = me->icc->CreateBLUT();
		if (lut)
		{
			me->core->OpenObject(lut);
		}
	}
}

SSWR::AVIRead::AVIRICCInfoForm::AVIRICCInfoForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"ICC Info");
	this->SetFont(0, 8.25, false);
	this->SetNoResize(true);

	this->icc = 0;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lblFileName, UI::GUILabel(ui, this, (const UTF8Char*)"File Name"));
	this->lblFileName->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtFileName, UI::GUITextBox(ui, this, (const UTF8Char*)""));
	this->txtFileName->SetRect(104, 4, 800, 23, false);
	this->txtFileName->SetReadOnly(true);
	NEW_CLASS(this->lblInfo, UI::GUILabel(ui, this, (const UTF8Char*)"Info"));
	this->lblInfo->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtInfo, UI::GUITextBox(ui, this, (const UTF8Char*)"", true));
	this->txtInfo->SetRect(104, 28, 800, 676, false);
	this->txtInfo->SetReadOnly(true);
	NEW_CLASS(this->btnRLUT, UI::GUIButton(ui, this, (const UTF8Char*)"View R LUT"));
	this->btnRLUT->SetRect(104, 708, 75, 23, false);
	this->btnRLUT->HandleButtonClick(OnRLUTClicked, this);
	NEW_CLASS(this->btnGLUT, UI::GUIButton(ui, this, (const UTF8Char*)"View G LUT"));
	this->btnGLUT->SetRect(184, 708, 75, 23, false);
	this->btnGLUT->HandleButtonClick(OnGLUTClicked, this);
	NEW_CLASS(this->btnBLUT, UI::GUIButton(ui, this, (const UTF8Char*)"View B LUT"));
	this->btnBLUT->SetRect(264, 708, 75, 23, false);
	this->btnBLUT->HandleButtonClick(OnBLUTClicked, this);

	this->HandleDropFiles(OnFileDrop, this);
}

SSWR::AVIRead::AVIRICCInfoForm::~AVIRICCInfoForm()
{
	SDEL_CLASS(this->icc);
}

void SSWR::AVIRead::AVIRICCInfoForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRICCInfoForm::SetICCProfile(Media::ICCProfile *icc, const UTF8Char *fileName)
{
	if (icc)
	{
		SDEL_CLASS(this->icc);

		Media::ColorProfile color;
		Text::StringBuilderUTF8 sb;
		icc->ToString(&sb);
		if (icc->GetColorPrimaries(&color.primaries))
		{
			sb.Append((const UTF8Char*)"\r\n");
			sb.Append((const UTF8Char*)"Primaries:\r\n");
			sb.Append((const UTF8Char*)"Red: x = ");
			Text::SBAppendF64(&sb, color.primaries.rx);
			sb.Append((const UTF8Char*)", y = ");
			Text::SBAppendF64(&sb, color.primaries.ry);
			sb.Append((const UTF8Char*)"\r\nGreen: x = ");
			Text::SBAppendF64(&sb, color.primaries.gx);
			sb.Append((const UTF8Char*)", y = ");
			Text::SBAppendF64(&sb, color.primaries.gy);
			sb.Append((const UTF8Char*)"\r\nBlue: x = ");
			Text::SBAppendF64(&sb, color.primaries.bx);
			sb.Append((const UTF8Char*)", y = ");
			Text::SBAppendF64(&sb, color.primaries.by);
			sb.Append((const UTF8Char*)"\r\nWhite: x = ");
			Text::SBAppendF64(&sb, color.primaries.wx);
			sb.Append((const UTF8Char*)", y = ");
			Text::SBAppendF64(&sb, color.primaries.wy);
		}
		this->icc = icc;
		this->txtInfo->SetText(sb.ToString());
		this->txtFileName->SetText(fileName);
	}
}
