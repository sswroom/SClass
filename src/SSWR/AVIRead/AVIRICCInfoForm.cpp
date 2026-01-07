#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "IO/FileStream.h"
#include "Media/ICCProfile.h"
#include "Media/LUT.h"
#include "SSWR/AVIRead/AVIRICCInfoForm.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRICCInfoForm::OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRICCInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRICCInfoForm>();
	UOSInt i;
	Data::ByteBuffer buff(1048576);
	UOSInt fileSize;
	UOSInt fileCnt = files.GetCount();
	i = 0;
	while (i < fileCnt)
	{
		{
			IO::FileStream fs(files[i], IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoBuffer);
			fileSize = fs.Read(buff);
		}
		if (fileSize == 0 || fileSize >= 1048576)
		{
		}
		else
		{
			NN<Media::ICCProfile> icc;
			if (Media::ICCProfile::Parse(buff.WithSize(fileSize)).SetTo(icc))
			{
				me->SetICCProfile(icc, files[i]->ToCString());
				break;
			}
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRICCInfoForm::OnRLUTClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRICCInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRICCInfoForm>();
	NN<Media::ICCProfile> icc;
	if (me->icc.SetTo(icc))
	{
		NN<Media::LUT> lut;
		if (icc->CreateRLUT().SetTo(lut))
		{
			me->core->OpenObject(lut);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRICCInfoForm::OnGLUTClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRICCInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRICCInfoForm>();
	NN<Media::ICCProfile> icc;
	if (me->icc.SetTo(icc))
	{
		NN<Media::LUT> lut;
		if (icc->CreateGLUT().SetTo(lut))
		{
			me->core->OpenObject(lut);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRICCInfoForm::OnBLUTClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRICCInfoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRICCInfoForm>();
	NN<Media::ICCProfile> icc;
	if (me->icc.SetTo(icc))
	{
		NN<Media::LUT> lut;
		if (icc->CreateBLUT().SetTo(lut))
		{
			me->core->OpenObject(lut);
		}
	}
}

SSWR::AVIRead::AVIRICCInfoForm::AVIRICCInfoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("ICC Info"));
	this->SetFont(nullptr, 8.25, false);
	this->SetNoResize(true);

	this->icc = 0;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblFileName = ui->NewLabel(*this, CSTR("File Name"));
	this->lblFileName->SetRect(4, 4, 100, 23, false);
	this->txtFileName = ui->NewTextBox(*this, CSTR(""));
	this->txtFileName->SetRect(104, 4, 800, 23, false);
	this->txtFileName->SetReadOnly(true);
	this->lblInfo = ui->NewLabel(*this, CSTR("Info"));
	this->lblInfo->SetRect(4, 28, 100, 23, false);
	this->txtInfo = ui->NewTextBox(*this, CSTR(""), true);
	this->txtInfo->SetRect(104, 28, 800, 676, false);
	this->txtInfo->SetReadOnly(true);
	this->btnRLUT = ui->NewButton(*this, CSTR("View R LUT"));
	this->btnRLUT->SetRect(104, 708, 75, 23, false);
	this->btnRLUT->HandleButtonClick(OnRLUTClicked, this);
	this->btnGLUT = ui->NewButton(*this, CSTR("View G LUT"));
	this->btnGLUT->SetRect(184, 708, 75, 23, false);
	this->btnGLUT->HandleButtonClick(OnGLUTClicked, this);
	this->btnBLUT = ui->NewButton(*this, CSTR("View B LUT"));
	this->btnBLUT->SetRect(264, 708, 75, 23, false);
	this->btnBLUT->HandleButtonClick(OnBLUTClicked, this);

	this->HandleDropFiles(OnFileDrop, this);
}

SSWR::AVIRead::AVIRICCInfoForm::~AVIRICCInfoForm()
{
	this->icc.Delete();
}

void SSWR::AVIRead::AVIRICCInfoForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRICCInfoForm::SetICCProfile(NN<Media::ICCProfile> icc, Text::CStringNN fileName)
{
	this->icc.Delete();

	Media::ColorProfile color;
	Text::StringBuilderUTF8 sb;
	icc->ToString(sb);
	if (icc->GetColorPrimaries(color.primaries))
	{
		sb.AppendC(UTF8STRC("\r\n"));
		sb.AppendC(UTF8STRC("Primaries:\r\n"));
		sb.AppendC(UTF8STRC("Red: x = "));
		sb.AppendDouble(color.primaries.r.x);
		sb.AppendC(UTF8STRC(", y = "));
		sb.AppendDouble(color.primaries.r.y);
		sb.AppendC(UTF8STRC("\r\nGreen: x = "));
		sb.AppendDouble(color.primaries.g.x);
		sb.AppendC(UTF8STRC(", y = "));
		sb.AppendDouble(color.primaries.g.y);
		sb.AppendC(UTF8STRC("\r\nBlue: x = "));
		sb.AppendDouble(color.primaries.b.x);
		sb.AppendC(UTF8STRC(", y = "));
		sb.AppendDouble(color.primaries.b.y);
		sb.AppendC(UTF8STRC("\r\nWhite: x = "));
		sb.AppendDouble(color.primaries.w.x);
		sb.AppendC(UTF8STRC(", y = "));
		sb.AppendDouble(color.primaries.w.y);
	}
	this->icc = icc;
	this->txtInfo->SetText(sb.ToCString());
	this->txtFileName->SetText(fileName);
}
