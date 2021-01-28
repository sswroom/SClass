#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Text/Encoding.h"
#include "Text/Locale.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "Win32/Clipboard.h"
#include <windows.h>

Win32::Clipboard::Clipboard(void *hwnd)
{
	if (OpenClipboard((HWND)hwnd))
	{
		this->succ = true;
	}
	else
	{
		this->succ = false;
	}
}

Win32::Clipboard::~Clipboard()
{
	if (this->succ)
	{
		CloseClipboard();
		this->succ = false;
	}
}

OSInt Win32::Clipboard::GetDataFormats(Data::ArrayList<Int32> *dataTypes)
{
	if (!this->succ)
		return 0;
	OSInt i = 0;
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

Bool Win32::Clipboard::GetDataText(UInt32 fmtId, Text::StringBuilderUTF *sb)
{
	HANDLE hand = GetClipboardData(fmtId);
	return GetDataTextH(hand, fmtId, sb, 1);
}

Win32::Clipboard::FilePasteType Win32::Clipboard::GetDataFiles(Data::ArrayList<const UTF8Char *> *fileNames)
{
	if (!this->succ)
		return Win32::Clipboard::FPT_NONE;
	UInt32 fmt = 0;
	UInt32 deff = 0;
	UInt32 fnameW = 0;
	WChar sbuff[512];
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
			GetClipboardFormatNameW(fmt, sbuff, 512);
			if (Text::StrEquals(sbuff, L"Preferred DropEffect"))
			{
				deff = fmt;
			}
		}
	}
	Win32::Clipboard::FilePasteType ret = Win32::Clipboard::FPT_NONE;
	if (deff && fnameW)
	{
#if !defined(_WIN32_WCE)
		Int32 eff = *(Int32*)GetClipboardData(deff);
		Bool valid = true;
		HDROP hDrop = (HDROP)GetClipboardData(fnameW);
		UInt32 fileCnt = DragQueryFileW(hDrop, -1, sbuff, 512);
		UInt32 i;
		if (eff == 5) //Copy
		{
			ret = Win32::Clipboard::FPT_COPY;
		}
		else if (eff == 2) //Move
		{
			ret = Win32::Clipboard::FPT_MOVE;
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
				DragQueryFileW(hDrop, i, sbuff, 512);
				fileNames->Add(Text::StrToUTF8New(sbuff));
				i++;
			}
		}
#endif
	}
	return ret;
}

void Win32::Clipboard::FreeDataFiles(Data::ArrayList<const UTF8Char *> *fileNames)
{
	OSInt i = fileNames->GetCount();;
	while (i-- > 0)
	{
		Text::StrDelNew(fileNames->GetItem(i));
	}
}

Bool Win32::Clipboard::GetDataTextH(void *hand, UInt32 fmtId, Text::StringBuilderUTF *sb, UInt32 tymed)
{
	UInt8 *memptr;
	WChar sbuff[512];
	UTF8Char u8buff[512];
	UTF8Char *tmpBuff;
	const UTF8Char *csptr;
	OSInt leng;
	OSInt leng2;
	if (hand == 0)
		return false;
	switch (fmtId)
	{
	case CF_OEMTEXT:
	case CF_TEXT:
		{
			Text::Encoding enc;
			memptr = (UInt8*)GlobalLock(hand);
			leng = Text::StrCharCnt((Char*)memptr);
			leng2 = enc.CountUTF8Chars((UInt8 *)memptr, leng);
			tmpBuff = MemAlloc(UTF8Char, leng2 + 1);
			enc.UTF8FromBytes(tmpBuff, (UInt8*)memptr, leng, 0);
			sb->AppendC(tmpBuff, leng2);
			MemFree(tmpBuff);
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
		csptr = Text::StrToUTF8New((WChar*)memptr);
		sb->Append(csptr);
		Text::StrDelNew(csptr);
		GlobalUnlock(hand);
		return true;
#if !defined(_WIN32_WCE)
	case CF_HDROP: //15
		leng = 0;
		while (DragQueryFileW((HDROP)hand, (UInt32)leng, sbuff, 512))
		{
			if (leng > 0)
			{
				sb->Append((const UTF8Char*)"\r\n");
			}
			csptr = Text::StrToUTF8New(sbuff);
			sb->Append(csptr);
			Text::StrDelNew(csptr);
			leng++;
		}
		return true;
	case CF_LOCALE: //16
		{
			memptr = (UInt8*)GlobalLock(hand);
			Text::Locale::LocaleEntry *locale = Text::Locale::GetLocaleEntry(*(Int32*)memptr);
			GlobalUnlock(hand);
			if (locale)
			{
				sb->AppendI32(locale->lcid);
				sb->Append((const UTF8Char*)", ");
				sb->Append(locale->shortName);
				sb->Append((const UTF8Char*)", ");
				sb->Append(locale->desc);
				sb->Append((const UTF8Char*)", ");
				sb->AppendI32(locale->defCodePage);
				return true;
			}
		}
		break;
	case CF_ENHMETAFILE: //14
		memptr = (UInt8*)GlobalLock(hand);
		GlobalUnlock(hand);
		return false;
#endif
	case CF_DIB: //8
		memptr = (UInt8*)GlobalLock(hand);
		sb->Append((const UTF8Char*)"Header Size = ");
		sb->AppendI32(ReadInt32(&memptr[0]));
		sb->Append((const UTF8Char*)"\r\nWidth = ");
		sb->AppendI32(ReadInt32(&memptr[4]));
		sb->Append((const UTF8Char*)"\r\nHeight = ");
		sb->AppendI32(ReadInt32(&memptr[8]));
		sb->Append((const UTF8Char*)"\r\nPlanes = ");
		sb->AppendI16(ReadInt16(&memptr[12]));
		sb->Append((const UTF8Char*)"\r\nBitCount = ");
		sb->AppendI16(ReadInt16(&memptr[14]));
		sb->Append((const UTF8Char*)"\r\nCompression = 0x");
		sb->AppendHex32V(ReadInt32(&memptr[16]));
		sb->Append((const UTF8Char*)"\r\nSizeImage = ");
		sb->AppendI32(ReadInt32(&memptr[20]));
		sb->Append((const UTF8Char*)"\r\nXPelsPerMeter = ");
		sb->AppendI32(ReadInt32(&memptr[24]));
		sb->Append((const UTF8Char*)"\r\nYPelsPerMeter = ");
		sb->AppendI32(ReadInt32(&memptr[28]));
		sb->Append((const UTF8Char*)"\r\nClrUsed = ");
		sb->AppendI32(ReadInt32(&memptr[32]));
		sb->Append((const UTF8Char*)"\r\nClrImportant = ");
		sb->AppendI32(ReadInt32(&memptr[36]));
		GlobalUnlock(hand);
		return true;
#if !defined(_WIN32_WCE)
	case CF_DIBV5: //17
		memptr = (UInt8*)GlobalLock(hand);
		sb->Append((const UTF8Char*)"Header Size = ");
		sb->AppendI32(ReadInt32(&memptr[0]));
		sb->Append((const UTF8Char*)"\r\nWidth = ");
		sb->AppendI32(ReadInt32(&memptr[4]));
		sb->Append((const UTF8Char*)"\r\nHeight = ");
		sb->AppendI32(ReadInt32(&memptr[8]));
		sb->Append((const UTF8Char*)"\r\nPlanes = ");
		sb->AppendI16(ReadInt16(&memptr[12]));
		sb->Append((const UTF8Char*)"\r\nBitCount = ");
		sb->AppendI16(ReadInt16(&memptr[14]));
		sb->Append((const UTF8Char*)"\r\nCompression = 0x");
		sb->AppendHex32V(ReadInt32(&memptr[16]));
		sb->Append((const UTF8Char*)"\r\nSizeImage = ");
		sb->AppendI32(ReadInt32(&memptr[20]));
		sb->Append((const UTF8Char*)"\r\nXPelsPerMeter = ");
		sb->AppendI32(ReadInt32(&memptr[24]));
		sb->Append((const UTF8Char*)"\r\nYPelsPerMeter = ");
		sb->AppendI32(ReadInt32(&memptr[28]));
		sb->Append((const UTF8Char*)"\r\nClrUsed = ");
		sb->AppendI32(ReadInt32(&memptr[32]));
		sb->Append((const UTF8Char*)"\r\nClrImportant = ");
		sb->AppendI32(ReadInt32(&memptr[36]));
		GlobalUnlock(hand);
		return true;
#endif
	}
	u8buff[0] = 0;
	GetFormatName(fmtId, u8buff, 256);
	if (Text::StrEquals(u8buff, (const UTF8Char*)"DataObject"))
	{
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"Rich Text Format"))
	{
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"Hidden Text Banner Format"))
	{
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"Ole Private Data"))
	{
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"Shell IDList Array"))
	{
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"DataObjectAttributes"))
	{
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"DataObjectAttributesRequiringElevation"))
	{
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"Shell Object Offsets"))
	{
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"Preferred DropEffect"))
	{
		Bool found;
		Int32 eff;
		memptr = (UInt8*)GlobalLock(hand);
		eff = *(Int32*)memptr;
		GlobalUnlock(hand);
	
		found = false;
		if (eff & 1)
		{
			if (found) sb->Append((const UTF8Char*)" | ");
			sb->Append((const UTF8Char*)"Copy");
			found = true;
		}
		if (eff & 2)
		{
			if (found) sb->Append((const UTF8Char*)" | ");
			sb->Append((const UTF8Char*)"Move");
			found = true;
		}
		if (eff & 4)
		{
			if (found) sb->Append((const UTF8Char*)" | ");
			sb->Append((const UTF8Char*)"Link");
			found = true;
		}
		if (eff & 0x80000000)
		{
			if (found) sb->Append((const UTF8Char*)" | ");
			sb->Append((const UTF8Char*)"Scroll");
			found = true;
		}
		if (!found)
		{
			sb->Append((const UTF8Char*)"None");
		}
		return true;
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"AsyncFlag"))
	{
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"DragImageBits"))
	{
		memptr = 0;
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"DragContext"))
	{
		memptr = 0;
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"FileName"))
	{
		Text::Encoding enc;
		memptr = (UInt8*)GlobalLock(hand);
		leng = Text::StrCharCnt((Char*)memptr);
		leng2 = enc.CountUTF8Chars((UInt8 *)memptr, leng);
		tmpBuff = MemAlloc(UTF8Char, leng2 + 1);
		enc.UTF8FromBytes(tmpBuff, (UInt8*)memptr, leng, 0);
		sb->Append(tmpBuff);
		MemFree(tmpBuff);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"FileNameW"))
	{
		memptr = (UInt8*)GlobalLock(hand);
		csptr = Text::StrToUTF8New((WChar*)memptr);
		sb->Append(csptr);
		Text::StrDelNew(csptr);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"FileContents"))
	{
		if (tymed == 4)
		{
			ULARGE_INTEGER li;
			LARGE_INTEGER ili;
			IStream *stm = (IStream*)hand;
			ili.QuadPart = 0;
			if (stm->Seek(ili, STREAM_SEEK_END, &li) == S_OK)
			{
				sb->Append((const UTF8Char*)"File Size = ");
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
				sb->Append((const UTF8Char*)"File Size = ");
				sb->AppendI64(fileSize);
				MemFree(tmpPtr);
				return true;
			}
		}
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"UniformResourceLocator"))
	{
		memptr = (UInt8*)GlobalLock(hand);
		sb->Append((const UTF8Char*)memptr);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"UniformResourceLocatorW"))
	{
		memptr = (UInt8*)GlobalLock(hand);
		csptr = Text::StrToUTF8New((WChar*)memptr);
		sb->Append(csptr);
		Text::StrDelNew(csptr);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"application/x-moz-file-promise-url"))
	{
		memptr = (UInt8*)GlobalLock(hand);
		csptr = Text::StrToUTF8New((WChar*)memptr);
		sb->Append(csptr);
		Text::StrDelNew(csptr);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"application/x-moz-file-promise-dest-filename"))
	{
		memptr = (UInt8*)GlobalLock(hand);
		csptr = Text::StrToUTF8New((WChar*)memptr);
		sb->Append(csptr);
		Text::StrDelNew(csptr);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"text/html"))
	{
		memptr = (UInt8*)GlobalLock(hand);
		csptr = Text::StrToUTF8New((WChar*)memptr);
		sb->Append(csptr);
		Text::StrDelNew(csptr);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"text/_moz_htmlcontext"))
	{
		memptr = (UInt8*)GlobalLock(hand);
		csptr = Text::StrToUTF8New((WChar*)memptr);
		sb->Append(csptr);
		Text::StrDelNew(csptr);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"text/_moz_htmlinfo"))
	{
		memptr = (UInt8*)GlobalLock(hand);
		csptr = Text::StrToUTF8New((WChar*)memptr);
		sb->Append(csptr);
		Text::StrDelNew(csptr);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"text/x-moz-url"))
	{
		memptr = (UInt8*)GlobalLock(hand);
		csptr = Text::StrToUTF8New((WChar*)memptr);
		sb->Append(csptr);
		Text::StrDelNew(csptr);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"text/x-moz-url-priv"))
	{
		memptr = (UInt8*)GlobalLock(hand);
		csptr = Text::StrToUTF8New((WChar*)memptr);
		sb->Append(csptr);
		Text::StrDelNew(csptr);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"text/x-moz-url-data"))
	{
		memptr = (UInt8*)GlobalLock(hand);
		csptr = Text::StrToUTF8New((WChar*)memptr);
		sb->Append(csptr);
		Text::StrDelNew(csptr);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"text/x-moz-url-desc"))
	{
		memptr = (UInt8*)GlobalLock(hand);
		csptr = Text::StrToUTF8New((WChar*)memptr);
		sb->Append(csptr);
		Text::StrDelNew(csptr);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"HTML Format"))
	{
		Text::Encoding enc;
		memptr = (UInt8*)GlobalLock(hand);
		leng = Text::StrCharCnt((Char*)memptr);
		leng2 = enc.CountUTF8Chars((UInt8 *)memptr, leng);
		tmpBuff = MemAlloc(UTF8Char, leng2 + 1);
		enc.UTF8FromBytes(tmpBuff, (UInt8*)memptr, leng, 0);
		sb->Append(tmpBuff);
		MemFree(tmpBuff);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"_NETSCAPE_URL"))
	{
		leng = GlobalSize(hand);
		memptr = (UInt8*)GlobalLock(hand);
		sb->AppendC(memptr, leng);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"text/plain;charset=utf-8"))
	{
		leng = GlobalSize(hand);
		memptr = (UInt8*)GlobalLock(hand);
		sb->AppendC(memptr, leng);
		GlobalUnlock(hand);
		return true;
	}
	else if (Text::StrEquals(u8buff, (const UTF8Char*)"text/unicode"))
	{
		leng = GlobalSize(hand);
		memptr = (UInt8*)GlobalLock(hand);
		const WChar *wptr = Text::StrCopyNewC((const WChar*)memptr, leng >> 1);
		csptr = Text::StrToUTF8New(wptr);
		sb->Append(csptr);
		Text::StrDelNew(csptr);
		Text::StrDelNew(wptr);
		GlobalUnlock(hand);
		return true;
	}
	
	leng = GlobalSize(hand);
	memptr = (UInt8*)GlobalLock(hand);
	sb->AppendHexBuff((UInt8*)memptr, leng, ' ', Text::LBT_CRLF);
	GlobalUnlock(hand);
	return false;
}

Bool Win32::Clipboard::SetString(void *hWndOwner, const UTF8Char *s)
{
	if (OpenClipboard((HWND)hWndOwner) == 0)
		return false;
	const WChar *wptr = Text::StrToWCharNew(s);
	OSInt len = Text::StrCharCnt(wptr);
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
	MemCopyNO(lptstrCopy, wptr, (len + 1) * sizeof(WChar));
	GlobalUnlock(hglbCopy);
	SetClipboardData(CF_UNICODETEXT, hglbCopy); 
	CloseClipboard();
	Text::StrDelNew(wptr);
	return true;
}

Bool Win32::Clipboard::GetString(void *hWndOwner, Text::StringBuilderUTF *sb)
{
	if (OpenClipboard((HWND)hWndOwner) == 0)
		return false;
	Bool succ = false;
	HANDLE hand = GetClipboardData(CF_UNICODETEXT);
	if (hand)
	{
		void *sptr = GlobalLock(hand);
		const UTF8Char *csptr = Text::StrToUTF8New((WChar*)sptr);
		sb->Append(csptr);
		Text::StrDelNew(csptr);
		GlobalUnlock(hand);
		succ = true;
	}
	CloseClipboard();
	return succ;
}

UTF8Char *Win32::Clipboard::GetFormatName(UInt32 fmtId, UTF8Char *sbuff, OSInt buffSize)
{
	switch (fmtId)
	{
	case CF_TEXT: //1
		return Text::StrConcat(sbuff, (const UTF8Char*)"TEXT");
	case CF_BITMAP: //2
		return Text::StrConcat(sbuff, (const UTF8Char*)"BITMAP");
#if !defined(_WIN32_WCE)
	case CF_METAFILEPICT: //3
		return Text::StrConcat(sbuff, (const UTF8Char*)"METAFILEPICT");
#endif
	case CF_SYLK: //4
		return Text::StrConcat(sbuff, (const UTF8Char*)"SYLK");
	case CF_DIF: //5
		return Text::StrConcat(sbuff, (const UTF8Char*)"DIF");
	case CF_TIFF: //6
		return Text::StrConcat(sbuff, (const UTF8Char*)"TIFF");
	case CF_OEMTEXT: //7
		return Text::StrConcat(sbuff, (const UTF8Char*)"OEMTEXT");
	case CF_DIB: //8
		return Text::StrConcat(sbuff, (const UTF8Char*)"DIB");
	case CF_PALETTE: //9
		return Text::StrConcat(sbuff, (const UTF8Char*)"PALETTE");
	case CF_PENDATA: //10
		return Text::StrConcat(sbuff, (const UTF8Char*)"PENDATA");
	case CF_RIFF: //11
		return Text::StrConcat(sbuff, (const UTF8Char*)"RIFF");
	case CF_WAVE: //12
		return Text::StrConcat(sbuff, (const UTF8Char*)"WAVE");
	case CF_UNICODETEXT: //13
		return Text::StrConcat(sbuff, (const UTF8Char*)"UNICODETEXT");
#if !defined(_WIN32_WCE)
	case CF_ENHMETAFILE: //14
		return Text::StrConcat(sbuff, (const UTF8Char*)"ENHMETAFILE");
	case CF_HDROP: //15
		return Text::StrConcat(sbuff, (const UTF8Char*)"HDROP");
	case CF_LOCALE: //16
		return Text::StrConcat(sbuff, (const UTF8Char*)"LOCALE");
	case CF_DIBV5: //17
		return Text::StrConcat(sbuff, (const UTF8Char*)"DIBV5");
	case CF_OWNERDISPLAY: //0x0080
		return Text::StrConcat(sbuff, (const UTF8Char*)"OWNERDISPLAY");
	case CF_DSPTEXT: //0x0081
		return Text::StrConcat(sbuff, (const UTF8Char*)"DSPTEXT");
	case CF_DSPBITMAP: //0x0082
		return Text::StrConcat(sbuff, (const UTF8Char*)"DSPBITMAP");
	case CF_DSPMETAFILEPICT: //0x0083
		return Text::StrConcat(sbuff, (const UTF8Char*)"DSPMETAFILEPICT");
	case CF_DSPENHMETAFILE: //0x008E
		return Text::StrConcat(sbuff, (const UTF8Char*)"DSPENHMETAFILE");
	case CF_PRIVATEFIRST: //0x0200
		return Text::StrConcat(sbuff, (const UTF8Char*)"PRIVATEFIRST");
	case CF_PRIVATELAST: //0x02FF
		return Text::StrConcat(sbuff, (const UTF8Char*)"PRIVATELAST");
	case CF_GDIOBJFIRST: //0x0300
		return Text::StrConcat(sbuff, (const UTF8Char*)"GDIOBJFIRST");
	case CF_GDIOBJLAST: //0x03FF
		return Text::StrConcat(sbuff, (const UTF8Char*)"GDIOBJLAST");
#endif
	default:
		{
			WChar wbuff[256];
			Int32 ret = GetClipboardFormatNameW(fmtId, wbuff, (int)255);
			if (ret == 0)
				return 0;
			return Text::StrWChar_UTF8(sbuff, wbuff, -1);
		}
	}
}
