#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Text/Encoding.h"
#include "Text/Locale.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Text/String.h"
#include "UI/Clipboard.h"
#include <windows.h>

UI::Clipboard::Clipboard(Optional<ControlHandle> hwnd)
{
	this->clsData = 0;
	if (OpenClipboard((HWND)hwnd.OrNull()))
	{
		this->succ = true;
	}
	else
	{
		this->succ = false;
	}
}

UI::Clipboard::~Clipboard()
{
	if (this->succ)
	{
		CloseClipboard();
		this->succ = false;
	}
}

UOSInt UI::Clipboard::GetDataFormats(Data::ArrayList<UInt32> *dataTypes)
{
	if (!this->succ)
		return 0;
	UOSInt i = 0;
	UInt32 val;
	val = 0;
	while (true)
	{
		val = EnumClipboardFormats(val);
		if (val == 0)
			break;
		dataTypes->Add(val);
		i++;
	}
	return i;
}

Bool UI::Clipboard::GetDataText(UInt32 fmtId, NN<Text::StringBuilderUTF8> sb)
{
	HANDLE hand = GetClipboardData(fmtId);
	return GetDataTextH(hand, fmtId, sb, 1);
}

UI::Clipboard::FilePasteType UI::Clipboard::GetDataFiles(Data::ArrayListStringNN *fileNames)
{
	if (!this->succ)
		return UI::Clipboard::FPT_NONE;
	UInt32 fmt = 0;
	UInt32 deff = 0;
	UInt32 fnameW = 0;
	WChar wbuff[512];
	while (true)
	{
		fmt = EnumClipboardFormats(fmt);
		if (fmt == 0)
			break;
		if (fmt == 15)
		{
			fnameW = fmt;
		}
		else
		{
			GetClipboardFormatNameW(fmt, wbuff, 512);
			if (Text::StrEquals(wbuff, L"Preferred DropEffect"))
			{
				deff = fmt;
			}
		}
	}
	UI::Clipboard::FilePasteType ret = UI::Clipboard::FPT_NONE;
	if (deff && fnameW)
	{
#if !defined(_WIN32_WCE)
		Int32 eff = *(Int32*)GetClipboardData(deff);
		Bool valid = true;
		HDROP hDrop = (HDROP)GetClipboardData(fnameW);
		UInt32 fileCnt = DragQueryFileW(hDrop, (UINT)-1, wbuff, 512);
		UInt32 i;
		if (eff == 5) //Copy
		{
			ret = UI::Clipboard::FPT_COPY;
		}
		else if (eff == 2) //Move
		{
			ret = UI::Clipboard::FPT_MOVE;
		}
		else
		{
			valid = false;
		}

		if (valid)
		{
			i = 0;
			while (i < fileCnt)
			{
				DragQueryFileW(hDrop, i, wbuff, 512);
				fileNames->Add(Text::String::NewNotNull(wbuff));
				i++;
			}
		}
#endif
	}
	return ret;
}

void UI::Clipboard::FreeDataFiles(Data::ArrayListStringNN *fileNames)
{
	fileNames->FreeAll();
}

Bool UI::Clipboard::GetDataTextH(void *hand, UInt32 fmtId, NN<Text::StringBuilderUTF8> sb, UInt32 tymed)
{
	UInt8 *memptr;
	WChar wbuff[512];
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> tmpBuff;
	UOSInt leng;
	UOSInt leng2;
	if (hand == 0)
		return false;
	switch (fmtId)
	{
	case CF_OEMTEXT:
	case CF_TEXT:
		{
			Text::Encoding enc;
			memptr = (UInt8*)GlobalLock(hand);
			leng = Text::StrCharCnt(memptr);
			leng2 = enc.CountUTF8Chars((UInt8 *)memptr, leng);
			tmpBuff = MemAllocArr(UTF8Char, leng2 + 1);
			enc.UTF8FromBytes(tmpBuff, (UInt8*)memptr, leng, 0);
			sb->AppendC(tmpBuff, leng2);
			MemFreeArr(tmpBuff);
			GlobalUnlock(hand);
		}
		return true;
	case CF_BITMAP: //2
#if !defined(_WIN32_WCE)
	case CF_METAFILEPICT: //3
#endif
	case CF_SYLK: //4
	case CF_DIF: //5
	case CF_TIFF: //6
	case CF_PALETTE: //9
	case CF_PENDATA: //10
	case CF_RIFF: //11
	case CF_WAVE: //12
		memptr = (UInt8*)GlobalLock(hand);
		GlobalUnlock(hand);
		return false;
	case CF_UNICODETEXT:
		memptr = (UInt8*)GlobalLock(hand);
		sb->AppendW((WChar*)memptr);
		GlobalUnlock(hand);
		return true;
#if !defined(_WIN32_WCE)
	case CF_HDROP: //15
		leng = 0;
		while (DragQueryFileW((HDROP)hand, (UInt32)leng, wbuff, 512))
		{
			if (leng > 0)
			{
				sb->AppendC(UTF8STRC("\r\n"));
			}
			sb->AppendW(wbuff);
			leng++;
		}
		return true;
	case CF_LOCALE: //16
		{
			memptr = (UInt8*)GlobalLock(hand);
			NN<Text::Locale::LocaleEntry> locale;
			if (Text::Locale::GetLocaleEntry(*(UInt32*)memptr).SetTo(locale))
			{
				GlobalUnlock(hand);
				sb->AppendU32(locale->lcid);
				sb->AppendC(UTF8STRC(", "));
				sb->AppendSlow(locale->shortName);
				sb->AppendC(UTF8STRC(", "));
				sb->AppendSlow(locale->desc);
				sb->AppendC(UTF8STRC(", "));
				sb->AppendU32(locale->defCodePage);
				return true;
			}
			GlobalUnlock(hand);
		}
		break;
	case CF_ENHMETAFILE: //14
		memptr = (UInt8*)GlobalLock(hand);
		GlobalUnlock(hand);
		return false;
#endif
	case CF_DIB: //8
		memptr = (UInt8*)GlobalLock(hand);
		sb->AppendC(UTF8STRC("Header Size = "));
		sb->AppendI32(ReadInt32(&memptr[0]));
		sb->AppendC(UTF8STRC("\r\nWidth = "));
		sb->AppendI32(ReadInt32(&memptr[4]));
		sb->AppendC(UTF8STRC("\r\nHeight = "));
		sb->AppendI32(ReadInt32(&memptr[8]));
		sb->AppendC(UTF8STRC("\r\nPlanes = "));
		sb->AppendI16(ReadInt16(&memptr[12]));
		sb->AppendC(UTF8STRC("\r\nBitCount = "));
		sb->AppendI16(ReadInt16(&memptr[14]));
		sb->AppendC(UTF8STRC("\r\nCompression = 0x"));
		sb->AppendHex32V(ReadUInt32(&memptr[16]));
		sb->AppendC(UTF8STRC("\r\nSizeImage = "));
		sb->AppendI32(ReadInt32(&memptr[20]));
		sb->AppendC(UTF8STRC("\r\nXPelsPerMeter = "));
		sb->AppendI32(ReadInt32(&memptr[24]));
		sb->AppendC(UTF8STRC("\r\nYPelsPerMeter = "));
		sb->AppendI32(ReadInt32(&memptr[28]));
		sb->AppendC(UTF8STRC("\r\nClrUsed = "));
		sb->AppendI32(ReadInt32(&memptr[32]));
		sb->AppendC(UTF8STRC("\r\nClrImportant = "));
		sb->AppendI32(ReadInt32(&memptr[36]));
		GlobalUnlock(hand);
		return true;
#if !defined(_WIN32_WCE)
	case CF_DIBV5: //17
		memptr = (UInt8*)GlobalLock(hand);
		sb->AppendC(UTF8STRC("Header Size = "));
		sb->AppendI32(ReadInt32(&memptr[0]));
		sb->AppendC(UTF8STRC("\r\nWidth = "));
		sb->AppendI32(ReadInt32(&memptr[4]));
		sb->AppendC(UTF8STRC("\r\nHeight = "));
		sb->AppendI32(ReadInt32(&memptr[8]));
		sb->AppendC(UTF8STRC("\r\nPlanes = "));
		sb->AppendI16(ReadInt16(&memptr[12]));
		sb->AppendC(UTF8STRC("\r\nBitCount = "));
		sb->AppendI16(ReadInt16(&memptr[14]));
		sb->AppendC(UTF8STRC("\r\nCompression = 0x"));
		sb->AppendHex32V(ReadUInt32(&memptr[16]));
		sb->AppendC(UTF8STRC("\r\nSizeImage = "));
		sb->AppendI32(ReadInt32(&memptr[20]));
		sb->AppendC(UTF8STRC("\r\nXPelsPerMeter = "));
		sb->AppendI32(ReadInt32(&memptr[24]));
		sb->AppendC(UTF8STRC("\r\nYPelsPerMeter = "));
		sb->AppendI32(ReadInt32(&memptr[28]));
		sb->AppendC(UTF8STRC("\r\nClrUsed = "));
		sb->AppendI32(ReadInt32(&memptr[32]));
		sb->AppendC(UTF8STRC("\r\nClrImportant = "));
		sb->AppendI32(ReadInt32(&memptr[36]));
		GlobalUnlock(hand);
		return true;
#endif
	}
	sbuff[0] = 0;
	sptr = GetFormatName(fmtId, sbuff, 256);
	if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("DataObject")))
	{
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Rich Text Format")))
	{
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Hidden Text Banner Format")))
	{
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Ole Private Data")))
	{
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Shell IDList Array")))
	{
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("DataObjectAttributes")))
	{
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("DataObjectAttributesRequiringElevation")))
	{
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Shell Object Offsets")))
	{
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Preferred DropEffect")))
	{
		Bool found;
		UInt32 eff;
		memptr = (UInt8*)GlobalLock(hand);
		eff = *(UInt32*)memptr;
		GlobalUnlock(hand);
	
		found = false;
		if (eff & 1)
		{
			if (found) sb->AppendC(UTF8STRC(" | "));
			sb->AppendC(UTF8STRC("Copy"));
			found = true;
		}
		if (eff & 2)
		{
			if (found) sb->AppendC(UTF8STRC(" | "));
			sb->AppendC(UTF8STRC("Move"));
			found = true;
		}
		if (eff & 4)
		{
			if (found) sb->AppendC(UTF8STRC(" | "));
			sb->AppendC(UTF8STRC("Link"));
			found = true;
		}
		if (eff & 0x80000000)
		{
			if (found) sb->AppendC(UTF8STRC(" | "));
			sb->AppendC(UTF8STRC("Scroll"));
			found = true;
		}
		if (!found)
		{
			sb->AppendC(UTF8STRC("None"));
		}
		return true;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("AsyncFlag")))
	{
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("DragImageBits")))
	{
		memptr = 0;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("DragContext")))
	{
		memptr = 0;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("FileName")))
	{
		Text::Encoding enc;
		memptr = (UInt8*)GlobalLock(hand);
		leng = Text::StrCharCnt(memptr);
		leng2 = enc.CountUTF8Chars((UInt8 *)memptr, leng);
		tmpBuff = MemAllocArr(UTF8Char, leng2 + 1);
		enc.UTF8FromBytes(tmpBuff, (UInt8*)memptr, leng, 0);
		sb->AppendC(tmpBuff, leng2);
		MemFreeArr(tmpBuff);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("FileNameW")))
	{
		memptr = (UInt8*)GlobalLock(hand);
		sb->AppendW((WChar*)memptr);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("FileContents")))
	{
		if (tymed == 4)
		{
			ULARGE_INTEGER li;
			LARGE_INTEGER ili;
			IStream *stm = (IStream*)hand;
			ili.QuadPart = 0;
			if (stm->Seek(ili, STREAM_SEEK_END, &li) == S_OK)
			{
				sb->AppendC(UTF8STRC("File Size = "));
				sb->AppendU64(li.QuadPart);
				stm->Seek(ili, STREAM_SEEK_SET, &li);
				return true;
			}
			else
			{
				UInt8 *tmpPtr = MemAlloc(UInt8, 1048576);
				UInt32 readSize;
				Int64 fileSize = 0;
				while (stm->Read(tmpPtr, 1048576, (ULONG*)&readSize) == S_OK)
				{
					if (readSize <= 0)
						break;
					fileSize += readSize;
				}
				sb->AppendC(UTF8STRC("File Size = "));
				sb->AppendI64(fileSize);
				MemFree(tmpPtr);
				return true;
			}
		}
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("UniformResourceLocator")))
	{
		memptr = (UInt8*)GlobalLock(hand);
		sb->AppendSlow((const UTF8Char*)memptr);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("UniformResourceLocatorW")))
	{
		memptr = (UInt8*)GlobalLock(hand);
		sb->AppendW((WChar*)memptr);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("application/x-moz-file-promise-url")))
	{
		memptr = (UInt8*)GlobalLock(hand);
		sb->AppendW((WChar*)memptr);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("application/x-moz-file-promise-dest-filename")))
	{
		memptr = (UInt8*)GlobalLock(hand);
		sb->AppendW((WChar*)memptr);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("text/html")))
	{
		memptr = (UInt8*)GlobalLock(hand);
		sb->AppendW((WChar*)memptr);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("text/_moz_htmlcontext")))
	{
		memptr = (UInt8*)GlobalLock(hand);
		sb->AppendW((WChar*)memptr);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("text/_moz_htmlinfo")))
	{
		memptr = (UInt8*)GlobalLock(hand);
		sb->AppendW((WChar*)memptr);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("text/x-moz-url")))
	{
		memptr = (UInt8*)GlobalLock(hand);
		sb->AppendW((WChar*)memptr);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("text/x-moz-url-priv")))
	{
		memptr = (UInt8*)GlobalLock(hand);
		sb->AppendW((WChar*)memptr);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("text/x-moz-url-data")))
	{
		memptr = (UInt8*)GlobalLock(hand);
		sb->AppendW((WChar*)memptr);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("text/x-moz-url-desc")))
	{
		memptr = (UInt8*)GlobalLock(hand);
		sb->AppendW((WChar*)memptr);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("HTML Format")))
	{
		Text::Encoding enc;
		memptr = (UInt8*)GlobalLock(hand);
		leng = Text::StrCharCnt(memptr);
		leng2 = enc.CountUTF8Chars((UInt8 *)memptr, leng);
		tmpBuff = MemAllocArr(UTF8Char, leng2 + 1);
		enc.UTF8FromBytes(tmpBuff, (UInt8*)memptr, leng, 0);
		sb->AppendC(tmpBuff, leng2);
		MemFreeArr(tmpBuff);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("_NETSCAPE_URL")))
	{
		leng = GlobalSize(hand);
		memptr = (UInt8*)GlobalLock(hand);
		sb->AppendC(memptr, leng);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("text/plain;charset=utf-8")))
	{
		leng = GlobalSize(hand);
		memptr = (UInt8*)GlobalLock(hand);
		sb->AppendC(memptr, leng);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("text/unicode")))
	{
		leng = GlobalSize(hand);
		memptr = (UInt8*)GlobalLock(hand);
		sb->AppendW((const WChar*)memptr, leng >> 1);
		GlobalUnlock(hand);
		return true;
	}
	
	leng = GlobalSize(hand);
	memptr = (UInt8*)GlobalLock(hand);
	sb->AppendHexBuff((UInt8*)memptr, leng, ' ', Text::LineBreakType::CRLF);
	GlobalUnlock(hand);
	return false;
}

Bool UI::Clipboard::SetString(Optional<ControlHandle> hWndOwner, Text::CStringNN s)
{
	if (OpenClipboard((HWND)hWndOwner.OrNull()) == 0)
		return false;
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(s.v);
	UOSInt len = Text::StrCharCnt(wptr);
	HGLOBAL hglbCopy;
	hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (len + 1) * sizeof(WChar));
	if (hglbCopy == 0)
	{
		CloseClipboard();
		Text::StrDelNew(wptr);
		return false;
	}
	if (EmptyClipboard() == 0)
	{
		CloseClipboard();
		GlobalFree(hglbCopy);
		Text::StrDelNew(wptr);
		return false;
	}
	WChar *lptstrCopy = (WChar*)GlobalLock(hglbCopy); 
	MemCopyNO(lptstrCopy, wptr.Ptr(), (len + 1) * sizeof(WChar));
	GlobalUnlock(hglbCopy);
	SetClipboardData(CF_UNICODETEXT, hglbCopy); 
	CloseClipboard();
	Text::StrDelNew(wptr);
	return true;
}

Bool UI::Clipboard::GetString(Optional<ControlHandle> hWndOwner, NN<Text::StringBuilderUTF8> sb)
{
	if (OpenClipboard((HWND)hWndOwner.OrNull()) == 0)
		return false;
	Bool succ = false;
	HANDLE hand = GetClipboardData(CF_UNICODETEXT);
	if (hand)
	{
		void *sptr = GlobalLock(hand);
		sb->AppendW((WChar*)sptr);
		GlobalUnlock(hand);
		succ = true;
	}
	CloseClipboard();
	return succ;
}

UnsafeArray<UTF8Char> UI::Clipboard::GetFormatName(UInt32 fmtId, UnsafeArray<UTF8Char> sbuff, UOSInt buffSize)
{
	switch (fmtId)
	{
	case CF_TEXT: //1
		return Text::StrConcatC(sbuff, UTF8STRC("TEXT"));
	case CF_BITMAP: //2
		return Text::StrConcatC(sbuff, UTF8STRC("BITMAP"));
#if !defined(_WIN32_WCE)
	case CF_METAFILEPICT: //3
		return Text::StrConcatC(sbuff, UTF8STRC("METAFILEPICT"));
#endif
	case CF_SYLK: //4
		return Text::StrConcatC(sbuff, UTF8STRC("SYLK"));
	case CF_DIF: //5
		return Text::StrConcatC(sbuff, UTF8STRC("DIF"));
	case CF_TIFF: //6
		return Text::StrConcatC(sbuff, UTF8STRC("TIFF"));
	case CF_OEMTEXT: //7
		return Text::StrConcatC(sbuff, UTF8STRC("OEMTEXT"));
	case CF_DIB: //8
		return Text::StrConcatC(sbuff, UTF8STRC("DIB"));
	case CF_PALETTE: //9
		return Text::StrConcatC(sbuff, UTF8STRC("PALETTE"));
	case CF_PENDATA: //10
		return Text::StrConcatC(sbuff, UTF8STRC("PENDATA"));
	case CF_RIFF: //11
		return Text::StrConcatC(sbuff, UTF8STRC("RIFF"));
	case CF_WAVE: //12
		return Text::StrConcatC(sbuff, UTF8STRC("WAVE"));
	case CF_UNICODETEXT: //13
		return Text::StrConcatC(sbuff, UTF8STRC("UNICODETEXT"));
#if !defined(_WIN32_WCE)
	case CF_ENHMETAFILE: //14
		return Text::StrConcatC(sbuff, UTF8STRC("ENHMETAFILE"));
	case CF_HDROP: //15
		return Text::StrConcatC(sbuff, UTF8STRC("HDROP"));
	case CF_LOCALE: //16
		return Text::StrConcatC(sbuff, UTF8STRC("LOCALE"));
	case CF_DIBV5: //17
		return Text::StrConcatC(sbuff, UTF8STRC("DIBV5"));
	case CF_OWNERDISPLAY: //0x0080
		return Text::StrConcatC(sbuff, UTF8STRC("OWNERDISPLAY"));
	case CF_DSPTEXT: //0x0081
		return Text::StrConcatC(sbuff, UTF8STRC("DSPTEXT"));
	case CF_DSPBITMAP: //0x0082
		return Text::StrConcatC(sbuff, UTF8STRC("DSPBITMAP"));
	case CF_DSPMETAFILEPICT: //0x0083
		return Text::StrConcatC(sbuff, UTF8STRC("DSPMETAFILEPICT"));
	case CF_DSPENHMETAFILE: //0x008E
		return Text::StrConcatC(sbuff, UTF8STRC("DSPENHMETAFILE"));
	case CF_PRIVATEFIRST: //0x0200
		return Text::StrConcatC(sbuff, UTF8STRC("PRIVATEFIRST"));
	case CF_PRIVATELAST: //0x02FF
		return Text::StrConcatC(sbuff, UTF8STRC("PRIVATELAST"));
	case CF_GDIOBJFIRST: //0x0300
		return Text::StrConcatC(sbuff, UTF8STRC("GDIOBJFIRST"));
	case CF_GDIOBJLAST: //0x03FF
		return Text::StrConcatC(sbuff, UTF8STRC("GDIOBJLAST"));
#endif
	default:
		{
			WChar wbuff[256];
			Int32 ret = GetClipboardFormatNameW(fmtId, wbuff, (int)255);
			if (ret == 0)
				return Text::StrUInt32(Text::StrConcatC(sbuff, UTF8STRC("Format ")), fmtId);
			return Text::StrWChar_UTF8(sbuff, wbuff);
		}
	}
}
