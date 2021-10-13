#include "Stdafx.h"
#include "IO/FileStream.h"
#include "Math/Math.h"
#include "Media/DDCReader.h"
#include "Media/EDID.h"
#include "SSWR/AVIRead/AVIREDIDViewerForm.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void SSWR::AVIRead::AVIREDIDViewerForm::UpdateEDIDDisp()
{
	if (this->edid)
	{
		Media::EDID::EDIDInfo info;
		if (Media::EDID::Parse(this->edid, &info))
		{
			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"Vendor Name: ");
			sb.Append(info.vendorName);
			sb.Append((const UTF8Char*)"\r\nProduct Code: ");
			sb.AppendHex16(info.productCode);
			sb.Append((const UTF8Char*)"\r\nSN: ");
			sb.AppendHex32(info.sn);

			sb.Append((const UTF8Char*)"\r\nDate of Manufacture: Year ");
			sb.AppendI32(info.yearOfManu);
			sb.Append((const UTF8Char*)", week ");
			sb.AppendU32(info.weekOfManu);
			sb.Append((const UTF8Char*)"\r\nEDID Ver: ");
			sb.AppendU32(info.edidVer);
			sb.Append((const UTF8Char*)", Rev ");
			sb.AppendU32(info.edidRev);
			sb.Append((const UTF8Char*)"\r\nPhysical Size (cm): ");
			sb.AppendU32(info.dispPhysicalW);
			sb.Append((const UTF8Char*)" x ");
			sb.AppendU32(info.dispPhysicalH);
			sb.Append((const UTF8Char*)" (");
			Text::SBAppendF64(&sb, Math::Unit::Distance::Convert(Math::Unit::Distance::DU_CENTIMETER, Math::Unit::Distance::DU_INCH, Math::Sqrt(info.dispPhysicalW * info.dispPhysicalW + info.dispPhysicalH * info.dispPhysicalH)));
			sb.Append((const UTF8Char*)"\")");
			if (info.monitorName[0])
			{
				sb.Append((const UTF8Char*)"\r\nMonitor Name: ");
				sb.Append(info.monitorName);
			}
			if (info.monitorSN[0])
			{
				sb.Append((const UTF8Char*)"\r\nMonitor SN: ");
				sb.Append(info.monitorSN);
			}
			sb.Append((const UTF8Char*)"\r\n\r\nColor Primaries:");
			sb.Append((const UTF8Char*)"\r\nGamma: ");
			Text::SBAppendF64(&sb, info.gamma);
			sb.Append((const UTF8Char*)"\r\nRed: ");
			Text::SBAppendF64(&sb, info.rx);
			sb.Append((const UTF8Char*)", ");
			Text::SBAppendF64(&sb, info.ry);
			sb.Append((const UTF8Char*)"\r\nGreen: ");
			Text::SBAppendF64(&sb, info.gx);
			sb.Append((const UTF8Char*)", ");
			Text::SBAppendF64(&sb, info.gy);
			sb.Append((const UTF8Char*)"\r\nBlue: ");
			Text::SBAppendF64(&sb, info.bx);
			sb.Append((const UTF8Char*)", ");
			Text::SBAppendF64(&sb, info.by);
			sb.Append((const UTF8Char*)"\r\nWhite: ");
			Text::SBAppendF64(&sb, info.wx);
			sb.Append((const UTF8Char*)", ");
			Text::SBAppendF64(&sb, info.wy);
			this->txtEDID->SetText(sb.ToString());
		}
		else
		{
			this->txtEDID->SetText((const UTF8Char*)"");
		}
	}
	else
	{
		this->txtEDID->SetText((const UTF8Char*)"");
	}
}

void __stdcall SSWR::AVIRead::AVIREDIDViewerForm::OnSaveClicked(void *userObj)
{
	SSWR::AVIRead::AVIREDIDViewerForm *me = (SSWR::AVIRead::AVIREDIDViewerForm*)userObj;
	if (me->edid == 0)
	{
		return;
	}
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	Media::EDID::EDIDInfo info;
	UI::FileDialog *dlg;

	if (Media::EDID::Parse(me->edid, &info))
	{
		sptr = Text::StrConcat(sbuff, info.vendorName);
		*sptr++ = '_';
		sptr = Text::StrHexVal16(sptr, info.productCode);
		if (info.monitorName[0])
		{
			*sptr++ = '_';
			sptr = Text::StrConcat(sptr, info.monitorName);
		}
		sptr = Text::StrConcat(sptr, (const UTF8Char*)".dat");

		NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"EDIDSave", true));
		dlg->SetFileName(sbuff);
		if (dlg->ShowDialog(me->GetHandle()))
		{
			IO::FileStream *fs;
			NEW_CLASS(fs, IO::FileStream(dlg->GetFileName(), IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
			fs->Write(me->edid, me->edidSize);
			DEL_CLASS(fs);
		}
		DEL_CLASS(dlg);
	}
}

void __stdcall SSWR::AVIRead::AVIREDIDViewerForm::OnFileDrop(void *userObj, const UTF8Char **fileNames, UOSInt fileCnt)
{
	SSWR::AVIRead::AVIREDIDViewerForm *me = (SSWR::AVIRead::AVIREDIDViewerForm*)userObj;
	IO::FileStream *fs;
	UInt8 *fileCont;
	UOSInt fileSize;
	UOSInt i;
	Bool found = false;
	i = 0;
	while (i < fileCnt)
	{
		NEW_CLASS(fs, IO::FileStream(fileNames[i], IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		fileSize = (UOSInt)fs->GetLength();
		if (fileSize >= 128 && fileSize <= 1024 && (fileSize & 127) == 0)
		{
			fileCont = MemAlloc(UInt8, fileSize);
			fs->Read(fileCont, fileSize);
			if (fileCont[0] == 0 && fileCont[1] == 0xff && fileCont[2] == 0xff && fileCont[3] == 0xff && fileCont[4] == 0xff && fileCont[5] == 0xff && fileCont[6] == 0xff && fileCont[7] == 0)
			{
				if (me->edid)
				{
					MemFree(me->edid);
					me->edid = 0;
				}
				me->edid = MemAlloc(UInt8, fileSize);
				me->edidSize = fileSize;
				MemCopyNO(me->edid, fileCont, fileSize);
				found = true;
			}

			MemFree(fileCont);
		}
		DEL_CLASS(fs);

		if (found)
		{
			me->UpdateEDIDDisp();
			break;
		}
		i++;
	}
}

SSWR::AVIRead::AVIREDIDViewerForm::AVIREDIDViewerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText((const UTF8Char*)"EDID Viewer");
	this->SetFont(0, 8.25, false);

	this->edid = 0;
	this->edidSize = 0;

	NEW_CLASS(this->pnlCtrl, UI::GUIPanel(ui, this));
	this->pnlCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnSave, UI::GUIButton(ui, this->pnlCtrl, (const UTF8Char*)"Save"));
	this->btnSave->SetRect(4, 4, 75, 23, false);
	this->btnSave->HandleButtonClick(OnSaveClicked, this);
	NEW_CLASS(this->txtEDID, UI::GUITextBox(ui, this, (const UTF8Char*)"", true));
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
	Media::DDCReader *reader;
	UOSInt edidSize;
	UInt8 *edid;
	MonitorHandle *hMon = this->GetHMonitor();
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	if (this->edid)
	{
		MemFree(this->edid);
		this->edid = 0;
	}
	NEW_CLASS(reader, Media::DDCReader(hMon));
	edid = reader->GetEDID(&edidSize);

	if (edid)
	{
		this->edidSize = edidSize;
		this->edid = MemAlloc(UInt8, edidSize);
		MemCopyNO(this->edid, edid, edidSize);
	}
	DEL_CLASS(reader);
	this->UpdateEDIDDisp();
}
