#include "Stdafx.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "IO/StmData/MemoryDataCopy.h"
#include "Media/BitmapUtil.h"
#include "Media/ImageList.h"
#include "Parser/FileParser/FNTParser.h"
#include "SSWR/AVIRead/AVIRExeForm.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/Clipboard.h"

void SSWR::AVIRead::AVIRExeForm::ParseSess16(NN<Manage::DasmX86_16::DasmX86_16_Sess> sess, NN<Data::ArrayListStringNN> codes, NN<Data::ArrayListNN<ExeB16Addr>> parts, NN<Data::ArrayListInt32> partInd, NN<ExeB16Addr> startAddr, NN<Manage::DasmX86_16> dasm, UOSInt codeSize)
{
	NN<Data::ArrayListNN<Data::ArrayListStringNN>> codesList;
	if (!this->codesList.SetTo(codesList))
		return;

	UTF8Char buff[512];
	UnsafeArray<UTF8Char> sptr;
	UOSInt buffSize;
	NN<ExeB16Addr> eaddr;
	UOSInt i;
	UInt16 oriIP;
	while (true)
	{
		while (true)
		{
//			sptr = Text::StrHexVal32(buff, (sess->regs.CS << 4) + sess->regs.IP);
			sptr = Text::StrHexVal16(buff, sess->regs.CS);
			sptr = Text::StrConcatC(sptr, UTF8STRC(":"));
			sptr = Text::StrHexVal16(sptr, sess->regs.IP);
			sptr = Text::StrConcatC(sptr, UTF8STRC(" "));
			oriIP = sess->regs.IP;
			if (oriIP == 0x229)
			{
				oriIP = 0x229;
			}
			Bool isSucc = dasm->DasmNext(sess, sptr, &buffSize);
			if (!isSucc)
			{
				UOSInt sizeLeft = codeSize - sess->regs.IP;
				if (sizeLeft > 16)
					sizeLeft = 16;
				sptr = Text::StrHexBytes(Text::StrConcatC(&buff[::Text::StrCharCnt(buff)], UTF8STRC("Unknown opcodes: ")), &sess->code[oriIP], sizeLeft, ' ');
				codes->Add(Text::String::New(buff, (UOSInt)(sptr - buff)));
				break;
			}
			codes->Add(Text::String::NewNotNullSlow(buff));
			if (sess->endStatus != 0)
			{
				UInt32 nextAddr = sess->regs.CS;
				nextAddr = (nextAddr << 16) + sess->endIP;
				OSInt i = sess->jmpAddrs.SortedIndexOf(nextAddr);
				if (i < 0)
				{
					break;
				}
				else
				{
					sess->regs.IP = sess->endIP;
				}
			}
		}
		startAddr->endAddr = sess->endIP;

		if (sess->endStatus == 3)
			break;
		if (sess->endStatus == 2)
			break;
		if (sess->endStatus == 1)
		{
			if (sess->codeSegm != sess->regs.CS)
			{
				break;
			}
			Bool found = false;
			i = parts->GetCount();
			while (i-- > 0)
			{
				eaddr = parts->GetItemNoCheck(i);
				if (sess->regs.IP >= eaddr->addr && sess->regs.IP < eaddr->endAddr)
				{
					found = true;
					break;
				}
			}
			if (found)
				break;
			NEW_CLASSNN(codes, Data::ArrayListStringNN());
			codesList->Add(codes);
			startAddr = MemAllocNN(ExeB16Addr);
			startAddr->segm = sess->regs.CS;
			startAddr->addr = sess->regs.IP;
			startAddr->endAddr = startAddr->addr;
			startAddr->codeList = codes;
//			this->lbParts->Items->Add(startAddr);
			parts->Insert(partInd->SortedInsert((startAddr->segm << 16) | startAddr->addr), startAddr);
		}
		else
		{
			break;
		}
	}
}

void SSWR::AVIRead::AVIRExeForm::InitSess16()
{
	Manage::DasmX86_16::DasmX86_16_Regs regs;
	NN<Manage::DasmX86_16> dasm;
	NN<Manage::DasmX86_16::DasmX86_16_Sess> sess;
	NN<Data::ArrayListStringNN> codes;
	NN<Data::ArrayListNN<ExeB16Addr>> parts;
	NN<Data::ArrayListInt32> partInd;
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<ExeB16Addr> eaddr;
	UOSInt codeSize;
	UOSInt i;
	UOSInt j;
	Data::ArrayListUInt32 *funcCalls;
	Data::ArrayListUInt32 *nfuncCalls;
	NN<Data::ArrayListNN<Data::ArrayListStringNN>> codesList;

	this->exeFile->GetDOSInitRegs(&regs);
	NEW_CLASSNN(parts, Data::ArrayListNN<ExeB16Addr>());
	NEW_CLASSNN(partInd, Data::ArrayListInt32());
	NEW_CLASSNN(codesList, Data::ArrayListNN<Data::ArrayListStringNN>());
	this->codesList = codesList;
	NEW_CLASSNN(codes, Data::ArrayListStringNN());
	codesList->Add(codes);
	eaddr = MemAllocNN(ExeB16Addr);
	eaddr->segm = regs.CS;
	eaddr->addr = eaddr->endAddr = regs.IP;
	eaddr->codeList = codes;
	//this->lbParts->Items->Add(eaddr);
	
	parts->Insert(partInd->SortedInsert((eaddr->segm << 16) | eaddr->addr), eaddr);
	
	NEW_CLASSNN(dasm, Manage::DasmX86_16());
	NEW_CLASS(funcCalls, Data::ArrayListUInt32());
	NEW_CLASS(nfuncCalls, Data::ArrayListUInt32());
	sess = dasm->CreateSess(regs, this->exeFile->GetDOSCodePtr(codeSize), this->exeFile->GetDOSCodeSegm());
	this->ParseSess16(sess, codes, parts, partInd, eaddr, dasm, codeSize);
	nfuncCalls->AddAll(sess->callAddrs);
	nfuncCalls->AddAll(sess->jmpAddrs);
	UOSInt arrSize;
	UInt32 *tmpArr = nfuncCalls->GetArr(arrSize).Ptr();
	ArtificialQuickSort_SortUInt32(tmpArr, 0, (OSInt)arrSize - 1);
	dasm->DeleteSess(sess);

	while (nfuncCalls->GetCount() > 0)
	{
		UInt32 faddr = nfuncCalls->GetItem(0);
		nfuncCalls->RemoveAt(0);
		OSInt si = funcCalls->SortedIndexOf(faddr);
		if (si < 0)
		{
			funcCalls->Insert((UOSInt)-si - 1, faddr);
			sess = dasm->CreateSess(regs, this->exeFile->GetDOSCodePtr(codeSize), this->exeFile->GetDOSCodeSegm());
			sess->regs.IP = (::UInt16)faddr;
			NEW_CLASSNN(codes, Data::ArrayListStringNN());
			codesList->Add(codes);
			eaddr = MemAllocNN(ExeB16Addr);
			eaddr->segm = sess->regs.CS;
			eaddr->addr = eaddr->endAddr = sess->regs.IP;
			eaddr->codeList = codes;
//				this->lbParts->Items->Add(eaddr);
			parts->Insert(partInd->SortedInsert((eaddr->segm << 16) | eaddr->addr), eaddr);
			
			this->ParseSess16(sess, codes, parts, partInd, eaddr, dasm, codeSize);
			nfuncCalls->AddAll(sess->callAddrs);
			nfuncCalls->AddAll(sess->jmpAddrs);
			tmpArr = nfuncCalls->GetArr(arrSize).Ptr();
			ArtificialQuickSort_SortUInt32(tmpArr, 0, (OSInt)arrSize - 1);
			dasm->DeleteSess(sess);
		}
	}
	DEL_CLASS(funcCalls);
	DEL_CLASS(nfuncCalls);
	dasm.Delete();
	partInd.Delete();
	this->parts = parts;

	Optional<ExeB16Addr> lastAddr = 0;
	NN<ExeB16Addr> addr;
	i = 0;
	j = parts->GetCount();
	while (i < j)
	{
		eaddr = parts->GetItemNoCheck(i);
		if (lastAddr.SetTo(addr) && addr->segm == eaddr->segm && addr->addr < eaddr->addr && addr->endAddr >= eaddr->endAddr)
		{
		}
		else
		{
			sptr = Text::StrHexVal16(Text::StrConcatC(Text::StrHexVal16(sbuff, eaddr->segm), UTF8STRC(":")), eaddr->addr);
			this->lb16BitFuncs->AddItem(CSTRP(sbuff, sptr), eaddr);
			lastAddr = eaddr;
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRExeForm::On16BitFuncsChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRExeForm> me = userObj.GetNN<SSWR::AVIRead::AVIRExeForm>();
	AnyType item = me->lb16BitFuncs->GetSelectedItem();
	if (item.NotNull())
	{
		NN<ExeB16Addr> addr = item.GetNN<ExeB16Addr>();
		UOSInt i;
		UOSInt j;
		me->lb16BitCont->ClearItems();
		i = 0;
		j = addr->codeList->GetCount();
		while (i < j)
		{
			me->lb16BitCont->AddItem(Text::String::OrEmpty(addr->codeList->GetItem(i)), 0);
			i++;
		}
	}
	else
	{
		me->lb16BitCont->ClearItems();
	}
}

void __stdcall SSWR::AVIRead::AVIRExeForm::OnImportSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRExeForm> me = userObj.GetNN<SSWR::AVIRead::AVIRExeForm>();
	UOSInt modIndex = (UOSInt)me->lbImport->GetSelectedItem().p;
	me->lvImport->ClearItems();
	UOSInt i;
	UOSInt j;
	i = 0;
	j = me->exeFile->GetImportFuncCount(modIndex);
	while (i < j)
	{
		me->lvImport->AddItem(Text::String::OrEmpty(me->exeFile->GetImportFunc(modIndex, i)), 0);
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRExeForm::OnExportDblClk(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRExeForm> me = userObj.GetNN<SSWR::AVIRead::AVIRExeForm>();
	NN<Text::String> s;
	if (me->lbExport->GetSelectedItemTextNew().SetTo(s))
	{
		UI::Clipboard::SetString(me->GetHandle(), s->ToCString());
		s->Release();
	}
}

void __stdcall SSWR::AVIRead::AVIRExeForm::OnResourceSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRExeForm> me = userObj.GetNN<SSWR::AVIRead::AVIRExeForm>();
	AnyType item = me->lbResource->GetSelectedItem();
	if (item.NotNull())
	{
		NN<const IO::EXEFile::ResourceInfo> res = item.GetNN<const IO::EXEFile::ResourceInfo>();
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("Size = "));
		sb.AppendUOSInt(res->dataSize);
		sb.AppendC(UTF8STRC("\r\nCodePage = "));
		sb.AppendU32(res->codePage);
		sb.AppendC(UTF8STRC("\r\nType = "));
		sb.Append(IO::EXEFile::GetResourceTypeName(res->rt));
		sb.AppendC(UTF8STRC("\r\n"));
		IO::EXEFile::GetResourceDesc(res, sb);
		me->txtResource->SetText(sb.ToCString());
	}
	else
	{
		me->txtResource->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRExeForm::OnResourceDblClk(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRExeForm> me = userObj.GetNN<SSWR::AVIRead::AVIRExeForm>();
	AnyType item = me->lbResource->GetSelectedItem();
	if (item.NotNull())
	{
		NN<const IO::EXEFile::ResourceInfo> res = item.GetNN<const IO::EXEFile::ResourceInfo>();
		if (res->rt == IO::EXEFile::RT_BITMAP)
		{
			NN<Media::StaticImage> simg;
			if (Media::BitmapUtil::ParseDIBBuffer(res->data, res->dataSize).SetTo(simg))
			{
				NN<Media::ImageList> imgList;
				NEW_CLASSNN(imgList, Media::ImageList(res->name));
				imgList->AddImage(simg, 0);
				me->core->OpenObject(imgList);
			}
		}
		else if (res->rt == IO::EXEFile::RT_ICON || res->rt == IO::EXEFile::RT_CURSOR)
		{
			IO::StmData::MemoryDataCopy fd(res->data, res->dataSize);
			me->core->LoadData(fd, 0);
		}
		else if (res->rt == IO::EXEFile::RT_FONT)
		{
			NN<Media::FontRenderer> font;
			if (font.Set(Parser::FileParser::FNTParser::ParseFontBuff(me->exeFile->GetSourceNameObj(), res->data, res->dataSize)))
			{
				me->core->OpenObject(font);
			}
		}
	}
}

SSWR::AVIRead::AVIRExeForm::AVIRExeForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<IO::EXEFile> exeFile) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	this->SetFont(0, 0, 8.25, false);
	this->exeFile = exeFile;
	this->parts = 0;
	this->codesList = 0;
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	sptr = exeFile->GetSourceNameObj()->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("EXE Form - ")));
	this->SetText(CSTRP(sbuff, sptr));

	UOSInt i;
	UOSInt j;
	this->tcEXE = ui->NewTabControl(*this);
	this->tcEXE->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpProp = this->tcEXE->AddTabPage(CSTR("Properties"));
	this->lvProp = ui->NewListView(this->tpProp, UI::ListViewStyle::Table, 2);
	this->lvProp->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvProp->SetShowGrid(true);
	this->lvProp->SetFullRowSelect(true);
	this->lvProp->AddColumn(CSTR("Name"), 250);
	this->lvProp->AddColumn(CSTR("Value"), 250);

	UOSInt k;
	i = 0;
	j = this->exeFile->GetPropCount();
	while (i < j)
	{
		k = this->lvProp->AddItem(Text::String::OrEmpty(this->exeFile->GetPropName(i)), 0);
		this->lvProp->SetSubItem(k, 1, Text::String::OrEmpty(this->exeFile->GetPropValue(i)));

		i++;
	}

	this->tpImport = this->tcEXE->AddTabPage(CSTR("Import"));
	this->lbImport = ui->NewListBox(this->tpImport, false);
	this->lbImport->SetRect(0, 0, 150, 23, false);
	this->lbImport->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbImport->HandleSelectionChange(OnImportSelChg, this);
	this->hspImport = ui->NewHSplitter(this->tpImport, 3, false);
	this->lvImport = ui->NewListView(this->tpImport, UI::ListViewStyle::Table, 1);
	this->lvImport->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvImport->SetShowGrid(true);
	this->lvImport->SetFullRowSelect(true);
	this->lvImport->AddColumn(CSTR("Func Name"), 200);
	i = 0;
	j = this->exeFile->GetImportCount();
	while (i < j)
	{
		this->lbImport->AddItem(Text::String::OrEmpty(this->exeFile->GetImportName(i)), (void*)i);
		i++;
	}

	this->tpExport = this->tcEXE->AddTabPage(CSTR("Export"));
	this->lbExport = ui->NewListBox(this->tpExport, false);
	this->lbExport->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbExport->HandleDoubleClicked(OnExportDblClk, this);
	i = 0;
	j = this->exeFile->GetExportCount();
	while (i < j)
	{
		this->lbExport->AddItem(Text::String::OrEmpty(this->exeFile->GetExportName(i)), (void*)i);
		i++;
	}

	this->tpResource = this->tcEXE->AddTabPage(CSTR("Resource"));
	this->lbResource = ui->NewListBox(this->tpResource, false);
	this->lbResource->SetRect(0, 0, 200, 100, false);
	this->lbResource->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbResource->HandleSelectionChange(OnResourceSelChg, this);
	this->lbResource->HandleDoubleClicked(OnResourceDblClk, this);
	this->hspResource = ui->NewHSplitter(this->tpResource, 3, false);
	this->txtResource = ui->NewTextBox(this->tpResource, CSTR(""), true);
	this->txtResource->SetReadOnly(true);
	this->txtResource->SetDockType(UI::GUIControl::DOCK_FILL);
	i = 0;
	j = this->exeFile->GetResourceCount();
	while (i < j)
	{
		NN<const IO::EXEFile::ResourceInfo> res;
		if (this->exeFile->GetResource(i).SetTo(res))
		{
			this->lbResource->AddItem(res->name, NN<IO::EXEFile::ResourceInfo>::ConvertFrom(res));
		}
		i++;
	}

	if (this->exeFile->HasDOS())
	{
		this->tp16Bit = this->tcEXE->AddTabPage(CSTR("16 Bit"));
		this->pnl16BitInfo = ui->NewPanel(this->tp16Bit);
		this->pnl16BitInfo->SetRect(0, 0, 100, 40, false);
		this->pnl16BitInfo->SetDockType(UI::GUIControl::DOCK_TOP);
		this->lb16BitFuncs = ui->NewListBox(this->tp16Bit, false);
		this->lb16BitFuncs->SetRect(0, 0, 120, 208, false);
		this->lb16BitFuncs->SetDockType(UI::GUIControl::DOCK_LEFT);
		this->lb16BitFuncs->HandleSelectionChange(On16BitFuncsChg, this);
		this->hsp16Bit = ui->NewHSplitter(this->tp16Bit, 3, false);
		this->lb16BitCont = ui->NewListBox(this->tp16Bit, false);
		this->lb16BitCont->SetDockType(UI::GUIControl::DOCK_FILL);

		this->InitSess16();
	}
}

SSWR::AVIRead::AVIRExeForm::~AVIRExeForm()
{
	UOSInt j;
	UOSInt i;
	NN<Data::ArrayListNN<ExeB16Addr>> parts;
	NN<Data::ArrayListNN<Data::ArrayListStringNN>> codesList;
	this->exeFile.Delete();
	if (this->parts.SetTo(parts))
	{
		NN<ExeB16Addr> addr;
		i = parts->GetCount();
		while (i-- > 0)
		{
			addr = parts->GetItemNoCheck(i);
			MemFreeNN(addr);
		}
		this->parts.Delete();
	}
	if (this->codesList.SetTo(codesList))
	{
		NN<Data::ArrayListStringNN> codes;
		i = codesList->GetCount();
		while (i-- > 0)
		{
			codes = codesList->GetItemNoCheck(i);
			j = codes->GetCount();
			while (j-- > 0)
			{
				OPTSTR_DEL(codes->GetItem(j));
			}
			codes.Delete();
		}
		this->codesList.Delete();
	}
}

void SSWR::AVIRead::AVIRExeForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
