#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "IO/FileStream.h"
#include "IO/StmData/MemoryDataCopy.h"
#include "Math/Math.h"
#include "Media/DDCReader.h"
#include "Media/EDID.h"
#include "SSWR/AVIRead/AVIREDIDViewerForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/GUIFileDialog.h"

void SSWR::AVIRead::AVIREDIDViewerForm::UpdateEDIDDisp()
{
	if (this->edid)
	{
		Media::EDID::EDIDInfo info;
		if (Media::EDID::Parse(this->edid, info))
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Vendor Name: "));
			sb.AppendSlow(info.vendorName);
			sb.AppendC(UTF8STRC("\r\nProduct Code: "));
			sb.AppendHex16(info.productCode);
			sb.AppendC(UTF8STRC("\r\nSN: "));
			sb.AppendHex32(info.sn);

			sb.AppendC(UTF8STRC("\r\nDate of Manufacture: Year "));
			sb.AppendI32(info.yearOfManu);
			sb.AppendC(UTF8STRC(", week "));
			sb.AppendU32(info.weekOfManu);
			sb.AppendC(UTF8STRC("\r\nEDID Ver: "));
			sb.AppendU32(info.edidVer);
			sb.AppendC(UTF8STRC(", Rev "));
			sb.AppendU32(info.edidRev);
			sb.AppendC(UTF8STRC("\r\nPixel Size: "));
			sb.AppendU32(info.pixelW);
			sb.AppendC(UTF8STRC(" x "));
			sb.AppendU32(info.pixelH);
			sb.AppendC(UTF8STRC("\r\nBit per color: "));
			sb.AppendU32(info.bitPerColor);
			sb.AppendC(UTF8STRC("\r\nPhysical Size (mm): "));
			sb.AppendU32(info.dispPhysicalW_mm);
			sb.AppendC(UTF8STRC(" x "));
			sb.AppendU32(info.dispPhysicalH_mm);
			sb.AppendC(UTF8STRC(" ("));
			sb.AppendDouble(Math::Unit::Distance::Convert(Math::Unit::Distance::DU_MILLIMETER, Math::Unit::Distance::DU_INCH, Math_Sqrt(info.dispPhysicalW_mm * info.dispPhysicalW_mm + info.dispPhysicalH_mm * info.dispPhysicalH_mm)));
			sb.AppendC(UTF8STRC("\")"));
			if (info.monitorName[0])
			{
				sb.AppendC(UTF8STRC("\r\nMonitor Name: "));
				sb.AppendSlow(info.monitorName);
			}
			if (info.monitorSN[0])
			{
				sb.AppendC(UTF8STRC("\r\nMonitor SN: "));
				sb.AppendSlow(info.monitorSN);
			}
			if (info.monitorOther[0])
			{
				sb.AppendC(UTF8STRC("\r\nMonitor Other: "));
				sb.AppendSlow(info.monitorOther);
			}
			sb.AppendC(UTF8STRC("\r\n\r\nColor Primaries:"));
			sb.AppendC(UTF8STRC("\r\nGamma: "));
			sb.AppendDouble(info.gamma);
			sb.AppendC(UTF8STRC("\r\nRed: "));
			sb.AppendDouble(info.r.x);
			sb.AppendC(UTF8STRC(", "));
			sb.AppendDouble(info.r.y);
			sb.AppendC(UTF8STRC("\r\nGreen: "));
			sb.AppendDouble(info.g.x);
			sb.AppendC(UTF8STRC(", "));
			sb.AppendDouble(info.g.y);
			sb.AppendC(UTF8STRC("\r\nBlue: "));
			sb.AppendDouble(info.b.x);
			sb.AppendC(UTF8STRC(", "));
			sb.AppendDouble(info.b.y);
			sb.AppendC(UTF8STRC("\r\nWhite: "));
			sb.AppendDouble(info.w.x);
			sb.AppendC(UTF8STRC(", "));
			sb.AppendDouble(info.w.y);
			this->txtEDID->SetText(sb.ToCString());
		}
		else
		{
			this->txtEDID->SetText(CSTR(""));
		}
	}
	else
	{
		this->txtEDID->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIREDIDViewerForm::OnSaveClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIREDIDViewerForm> me = userObj.GetNN<SSWR::AVIRead::AVIREDIDViewerForm>();
	if (me->edid == 0)
	{
		return;
	}
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	Media::EDID::EDIDInfo info;

	if (Media::EDID::Parse(me->edid, info))
	{
		sptr = Text::StrConcat(sbuff, info.vendorName);
		*sptr++ = '_';
		sptr = Text::StrHexVal16(sptr, info.productCode);
		if (info.monitorName[0])
		{
			*sptr++ = '_';
			sptr = Text::StrConcat(sptr, info.monitorName);
		}
		sptr = Text::StrConcatC(sptr, UTF8STRC(".dat"));

		NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"EDIDSave", true);
		dlg->SetFileName(CSTRP(sbuff, sptr));
		if (dlg->ShowDialog(me->GetHandle()))
		{
			IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
			fs.Write(me->edid, me->edidSize);
		}
		dlg.Delete();
	}
}

void __stdcall SSWR::AVIRead::AVIREDIDViewerForm::OnHexClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIREDIDViewerForm> me = userObj.GetNN<SSWR::AVIRead::AVIREDIDViewerForm>();
	if (me->edid == 0)
	{
		return;
	}
	IO::StmData::MemoryDataCopy fd(me->edid, me->edidSize);
	me->core->OpenHex(fd, 0);
}

void __stdcall SSWR::AVIRead::AVIREDIDViewerForm::OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> fileNames)
{
	NN<SSWR::AVIRead::AVIREDIDViewerForm> me = userObj.GetNN<SSWR::AVIRead::AVIREDIDViewerForm>();
	UOSInt fileSize;
	UOSInt i;
	UOSInt fileCnt = fileNames.GetCount();
	Bool found = false;
	i = 0;
	while (i < fileCnt)
	{
		{
			IO::FileStream fs(fileNames[i], IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			fileSize = (UOSInt)fs.GetLength();
			if (fileSize >= 128 && fileSize <= 1024 && (fileSize & 127) == 0)
			{
				Data::ByteBuffer fileCont(fileSize);
				fs.Read(fileCont);
				if (fileCont[0] == 0 && fileCont[1] == 0xff && fileCont[2] == 0xff && fileCont[3] == 0xff && fileCont[4] == 0xff && fileCont[5] == 0xff && fileCont[6] == 0xff && fileCont[7] == 0)
				{
					if (me->edid)
					{
						MemFree(me->edid);
						me->edid = 0;
					}
					me->edid = MemAlloc(UInt8, fileSize);
					me->edidSize = fileSize;
					MemCopyNO(me->edid, fileCont.Ptr(), fileSize);
					found = true;
				}
			}
		}

		if (found)
		{
			me->UpdateEDIDDisp();
			break;
		}
		i++;
	}
}

SSWR::AVIRead::AVIREDIDViewerForm::AVIREDIDViewerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("EDID Viewer"));
	this->SetFont(0, 0, 8.25, false);

	this->edid = 0;
	this->edidSize = 0;

	this->pnlCtrl = ui->NewPanel(*this);
	this->pnlCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnSave = ui->NewButton(this->pnlCtrl, CSTR("Save"));
	this->btnSave->SetRect(4, 4, 75, 23, false);
	this->btnSave->HandleButtonClick(OnSaveClicked, this);
	this->btnHex = ui->NewButton(this->pnlCtrl, CSTR("Hex"));
	this->btnHex->SetRect(84, 4, 75, 23, false);
	this->btnHex->HandleButtonClick(OnHexClicked, this);
	this->txtEDID = ui->NewTextBox(*this, CSTR(""), true);
	this->txtEDID->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtEDID->SetReadOnly(true);

	this->HandleDropFiles(OnFileDrop, this);
	this->OnMonitorChanged();
}

SSWR::AVIRead::AVIREDIDViewerForm::~AVIREDIDViewerForm()
{
	if (this->edid)
	{
		MemFree(this->edid);
		this->edid = 0;
	}
}

void SSWR::AVIRead::AVIREDIDViewerForm::OnMonitorChanged()
{
	UOSInt edidSize;
	UInt8 *edid;
	MonitorHandle *hMon = this->GetHMonitor();
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	if (this->edid)
	{
		MemFree(this->edid);
		this->edid = 0;
	}

	{
		Media::DDCReader reader(hMon);
		edid = reader.GetEDID(&edidSize);

		if (edid)
		{
			this->edidSize = edidSize;
			this->edid = MemAlloc(UInt8, edidSize);
			MemCopyNO(this->edid, edid, edidSize);
		}
	}
	this->UpdateEDIDDisp();
}
