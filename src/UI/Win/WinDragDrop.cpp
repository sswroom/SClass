#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/Clipboard.h"
#include "UI/Win/WinDragDrop.h"
#include "Win32/StreamCOM.h"

void UI::Win::WinDropData::LoadData()
{
	UTF8Char sbuff[512];
	FORMATETC fmt;
	FORMATETC *newFmt;
	IEnumFORMATETC *enumFmt;

	NEW_CLASS(this->dataMap, Data::StringUTF8Map<FORMATETC *>());

	if (pDataObj->EnumFormatEtc(DATADIR_GET, &enumFmt) == S_OK)
	{
		while (enumFmt->Next(1, &fmt, 0) == S_OK)
		{
			newFmt = MemAlloc(FORMATETC, 1);
			MemCopyNO(newFmt, &fmt, sizeof(fmt));
			if (UI::Clipboard::GetFormatName(fmt.cfFormat, sbuff, 512) == 0)
			{
				Text::StrInt32(Text::StrConcatC(sbuff, UTF8STRC("Format ")), fmt.cfFormat);
			}
			this->dataMap->Put(sbuff, newFmt);
		}
		enumFmt->Release();
	}
}

UI::Win::WinDropData::WinDropData(IDataObject *pDataObj)
{
	this->pDataObj = pDataObj;
	this->dataMap = 0;
}

UI::Win::WinDropData::~WinDropData()
{
	if (this->dataMap)
	{
		FORMATETC *fmt;
		NotNullPtr<const Data::ArrayList<FORMATETC *>> fmtList = this->dataMap->GetValues();
		UOSInt i = fmtList->GetCount();
		while (i-- > 0)
		{
			fmt = fmtList->GetItem(i);
			MemFree(fmt);
		}
		DEL_CLASS(this->dataMap);
	}
}

UOSInt UI::Win::WinDropData::GetCount()
{
	if (this->dataMap == 0)
	{
		this->LoadData();
	}
	return this->dataMap->GetCount();
}

const UTF8Char *UI::Win::WinDropData::GetName(UOSInt index)
{
	if (this->dataMap == 0)
	{
		this->LoadData();
	}
	return this->dataMap->GetKey(index);
}

Bool UI::Win::WinDropData::GetDataText(const UTF8Char *name, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	if (this->dataMap == 0)
	{
		this->LoadData();
	}
	FORMATETC *fmt = this->dataMap->Get(name);
	HRESULT hres;
	STGMEDIUM med;
	if (fmt == 0)
		return false;

	if ((hres = this->pDataObj->GetData(fmt, &med)) == S_OK)
	{
		if (UI::Clipboard::GetDataTextH(med.hGlobal, fmt->cfFormat, sb, med.tymed))
		{
		}
		else
		{
			sb->AppendC(UTF8STRC("Unknown data"));
		}
		ReleaseStgMedium(&med);
		return true;
	}
	else
	{
		return false;
	}
}

IO::Stream *UI::Win::WinDropData::GetDataStream(const UTF8Char *name)
{
	if (this->dataMap == 0)
	{
		this->LoadData();
	}
	FORMATETC *fmt = this->dataMap->Get(name);
	HRESULT hres;
	STGMEDIUM med;
	if (fmt == 0)
		return 0;

	if ((hres = this->pDataObj->GetData(fmt, &med)) == S_OK)
	{
		Win32::StreamCOM *stm = 0;
		if (med.tymed == TYMED_ISTREAM)
		{
			NEW_CLASS(stm, Win32::StreamCOM(med.pstm, true));
			med.pstm = 0;
		}
		ReleaseStgMedium(&med);
		return stm;
	}
	else
	{
		return 0;
	}
}

UI::Win::WinDragDrop::WinDragDrop(void *hWnd, UI::GUIDropHandler *hdlr)
{
	this->hWnd = hWnd;
	this->hdlr = hdlr;
	this->unkCnt = 0;
	this->currEff = UI::GUIDropHandler::DE_NONE;
}

UI::Win::WinDragDrop::~WinDragDrop()
{
}

HRESULT STDMETHODCALLTYPE UI::Win::WinDragDrop::QueryInterface(REFIID riid, void **ppvObject)
{
	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE UI::Win::WinDragDrop::AddRef()
{
	return ++this->unkCnt;
}

ULONG STDMETHODCALLTYPE UI::Win::WinDragDrop::Release()
{
	return --this->unkCnt;
}

HRESULT STDMETHODCALLTYPE UI::Win::WinDragDrop::DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	UI::Win::WinDropData *data;
	UI::GUIDropHandler::DragEffect eff;
	NEW_CLASS(data, WinDropData(pDataObj));
	eff = this->hdlr->DragEnter(data);
	DEL_CLASS(data);
	this->currEff = eff;
	if (eff == UI::GUIDropHandler::DE_COPY)
	{
		*pdwEffect = DROPEFFECT_COPY;
	}
	else if (eff == UI::GUIDropHandler::DE_MOVE)
	{
		*pdwEffect = DROPEFFECT_MOVE;
	}
	else if (eff == UI::GUIDropHandler::DE_LINK)
	{
		*pdwEffect = DROPEFFECT_LINK;
	}
	else
	{
		*pdwEffect = DROPEFFECT_NONE;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE UI::Win::WinDragDrop::DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	if (this->currEff == UI::GUIDropHandler::DE_COPY)
	{
		*pdwEffect = *pdwEffect & DROPEFFECT_COPY;
	}
	else if (this->currEff == UI::GUIDropHandler::DE_MOVE)
	{
		*pdwEffect = *pdwEffect & DROPEFFECT_MOVE;
	}
	else if (this->currEff == UI::GUIDropHandler::DE_LINK)
	{
		*pdwEffect = *pdwEffect & DROPEFFECT_LINK;
	}
	else
	{
		*pdwEffect = *pdwEffect & DROPEFFECT_NONE;
	}
	return S_OK;
}

HRESULT STDMETHODCALLTYPE UI::Win::WinDragDrop::DragLeave()
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE UI::Win::WinDragDrop::Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	WinDropData *data;
	NEW_CLASS(data, WinDropData(pDataObj));
	this->hdlr->DropData(data, pt.x, pt.y);
	DEL_CLASS(data);
	return S_OK;
}

void UI::Win::WinDragDrop::SetHandler(UI::GUIDropHandler *hdlr)
{
	this->hdlr = hdlr;
}
