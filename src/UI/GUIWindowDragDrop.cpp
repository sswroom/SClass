#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "UI/GUIWindowDragDrop.h"
#include "Win32/Clipboard.h"
#include "Win32/StreamCOM.h"

void UI::GUIWindowDragData::LoadData()
{
	UTF8Char u8buff[512];
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
			if (Win32::Clipboard::GetFormatName(fmt.cfFormat, u8buff, 512) == 0)
			{
				Text::StrInt32(Text::StrConcatC(u8buff, UTF8STRC("Format ")), fmt.cfFormat);
			}
			this->dataMap->Put(u8buff, newFmt);
		}
		enumFmt->Release();
	}
}

UI::GUIWindowDragData::GUIWindowDragData(IDataObject *pDataObj)
{
	this->pDataObj = pDataObj;
	this->dataMap = 0;
}

UI::GUIWindowDragData::~GUIWindowDragData()
{
	if (this->dataMap)
	{
		FORMATETC *fmt;
		Data::ArrayList<FORMATETC *> *fmtList = this->dataMap->GetValues();
		UOSInt i = fmtList->GetCount();
		while (i-- > 0)
		{
			fmt = fmtList->GetItem(i);
			MemFree(fmt);
		}
		DEL_CLASS(this->dataMap);
	}
}

UOSInt UI::GUIWindowDragData::GetCount()
{
	if (this->dataMap == 0)
	{
		this->LoadData();
	}
	return this->dataMap->GetCount();
}

const UTF8Char *UI::GUIWindowDragData::GetName(UOSInt index)
{
	if (this->dataMap == 0)
	{
		this->LoadData();
	}
	return this->dataMap->GetKey(index);
}

Bool UI::GUIWindowDragData::GetDataText(const UTF8Char *name, Text::StringBuilderUTF8 *sb)
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
		if (Win32::Clipboard::GetDataTextH(med.hGlobal, fmt->cfFormat, sb, med.tymed))
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

IO::Stream *UI::GUIWindowDragData::GetDataStream(const UTF8Char *name)
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

UI::GUIWindowDragDrop::GUIWindowDragDrop(void *hWnd, UI::GUIDropHandler *hdlr)
{
	this->hWnd = hWnd;
	this->hdlr = hdlr;
	this->unkCnt = 0;
	this->currEff = UI::GUIDropHandler::DE_NONE;
}

UI::GUIWindowDragDrop::~GUIWindowDragDrop()
{
}

HRESULT STDMETHODCALLTYPE UI::GUIWindowDragDrop::QueryInterface(REFIID riid, void **ppvObject)
{
	return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE UI::GUIWindowDragDrop::AddRef()
{
	return ++this->unkCnt;
}

ULONG STDMETHODCALLTYPE UI::GUIWindowDragDrop::Release()
{
	return --this->unkCnt;
}

HRESULT STDMETHODCALLTYPE UI::GUIWindowDragDrop::DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	UI::GUIWindowDragData *data;
	UI::GUIDropHandler::DragEffect eff;
	NEW_CLASS(data, GUIWindowDragData(pDataObj));
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

HRESULT STDMETHODCALLTYPE UI::GUIWindowDragDrop::DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
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

HRESULT STDMETHODCALLTYPE UI::GUIWindowDragDrop::DragLeave()
{
	return S_OK;
}

HRESULT STDMETHODCALLTYPE UI::GUIWindowDragDrop::Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
{
	GUIWindowDragData *data;
	NEW_CLASS(data, GUIWindowDragData(pDataObj));
	this->hdlr->DropData(data, pt.x, pt.y);
	DEL_CLASS(data);
	return S_OK;
}

void UI::GUIWindowDragDrop::SetHandler(UI::GUIDropHandler *hdlr)
{
	this->hdlr = hdlr;
}
