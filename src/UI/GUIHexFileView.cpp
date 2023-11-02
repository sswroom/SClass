#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "Math/Math.h"
#include "Text/CharUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "UI/GUIHexFileView.h"

UI::GUIHexFileView::GUIHexFileView(NotNullPtr<UI::GUICore> ui, UI::GUIClientControl *parent, NotNullPtr<Media::DrawEngine> deng) : UI::GUITextView(ui, parent, deng)
{
	this->fs = 0;
	this->fd = 0;
	this->analyse = 0;
	this->frame = 0;
	this->fileSize = 0;
	this->currOfst = 0;
	if ((this->txtColor & 0x808080) == 0x808080)
	{
		this->frameColor = 0xff808060;
		this->fieldColor = 0xff652b2b;
	}
	else
	{
		this->frameColor = 0xffdedf9a;
		this->fieldColor = 0xffdf9abd;
	}
	this->SetScrollHRange(0, 0);
}

UI::GUIHexFileView::~GUIHexFileView()
{
	SDEL_CLASS(this->fs);
	SDEL_CLASS(this->fd);
	SDEL_CLASS(this->analyse);
	SDEL_CLASS(this->frame);
}

void UI::GUIHexFileView::EventLineUp()
{
	if (this->currOfst >= 16)
	{
		this->GoToOffset(this->currOfst - 16);
	}
}

void UI::GUIHexFileView::EventLineDown()
{
	if (this->currOfst + 16 < this->fileSize)
	{
		this->GoToOffset(this->currOfst + 16);
	}
	else
	{
		this->EventEnd();
	}
}

void UI::GUIHexFileView::EventPageUp()
{
	if (this->currOfst >= this->pageLineCnt * 16)
	{
		this->GoToOffset(this->currOfst - (this->pageLineCnt * 16));
	}
	else
	{
		this->GoToOffset(this->currOfst & 15);
	}
}

void UI::GUIHexFileView::EventPageDown()
{
	if (this->currOfst + this->pageLineCnt * 16 < this->fileSize)
	{
		this->GoToOffset(this->currOfst + (this->pageLineCnt * 16));
	}
	else
	{
		this->EventEnd();
	}
}

void UI::GUIHexFileView::EventLeft()
{
	if (this->currOfst > 0)
	{
		this->GoToOffset(this->currOfst - 1);
	}
}

void UI::GUIHexFileView::EventRight()
{
	if (this->currOfst < this->fileSize - 1)
	{
		this->GoToOffset(this->currOfst + 1);
	}
}

void UI::GUIHexFileView::EventHome()
{
	this->GoToOffset(0);
}

void UI::GUIHexFileView::EventEnd()
{
	if (this->fileSize > 0)
	{
		this->GoToOffset(this->fileSize - 1);
	}
}

void UI::GUIHexFileView::EventLineBegin()
{
	UInt64 currOfst = this->currOfst >> 4;
	currOfst = (currOfst << 4);
	this->GoToOffset(currOfst);
}

void UI::GUIHexFileView::EventLineEnd()
{
	if (this->fileSize > 0)
	{
		UInt64 currOfst = this->currOfst >> 4;
		currOfst = (currOfst << 4) + 15;
		if (currOfst >= this->fileSize)
		{
			currOfst = this->fileSize - 1;
		}
		this->GoToOffset(currOfst);
	}
}

void UI::GUIHexFileView::EventCopy()
{

}

void UI::GUIHexFileView::EventMouseDown(OSInt scnX, OSInt scnY, MouseButton btn)
{
	if (btn == MBTN_LEFT)
	{
		UInt64 ofst;
		this->GetTextPos(OSInt2Double(scnX), OSInt2Double(scnY), &ofst);
		this->GoToOffset(ofst);
	}
}

void UI::GUIHexFileView::EventMouseUp(OSInt scnX, OSInt scnY, MouseButton btn)
{

}

void UI::GUIHexFileView::EventMouseMove(OSInt scnX, OSInt scnY)
{

}

void UI::GUIHexFileView::EventTimerTick()
{
	if (this->fs)
	{
		UInt64 fileLen = this->fs->GetLength();
		if (fileLen != this->fileSize)
		{
			this->fileSize = fileLen;
			this->SetScrollVRange(0, (UOSInt)(this->fileSize >> 4));
			this->Redraw();
		}
	}
}

void UI::GUIHexFileView::DrawImage(NotNullPtr<Media::DrawImage> dimg)
{
	Media::DrawBrush *b = dimg->NewBrushARGB(this->bgColor);
	dimg->DrawRect(Math::Coord2DDbl(0, 0), dimg->GetSize().ToDouble(), 0, b);
	dimg->DelBrush(b);
	OSInt vPos = this->GetScrollVPos();
	if (this->fileSize > 0)
	{
		UTF8Char sbuff[17];
		UTF8Char sbuff2[2];
		UTF8Char *sptr;
		UInt64 currOfst = ((UInt64)(UOSInt)vPos) * 16;
		Media::DrawFont *f = this->CreateDrawFont(dimg);
		Media::DrawBrush *lineNumBrush = dimg->NewBrushARGB(this->lineNumColor);
		Media::DrawBrush *textBrush = dimg->NewBrushARGB(this->txtColor);
		Media::DrawBrush *selBrush = dimg->NewBrushARGB(this->selColor);
		Media::DrawBrush *selTextBrush = dimg->NewBrushARGB(this->selTextColor);
		Media::DrawBrush *frameBrush = dimg->NewBrushARGB(this->frameColor);
		Media::DrawBrush *fieldBrush = dimg->NewBrushARGB(this->fieldColor);
		Math::Coord2DDbl currPos = Math::Coord2DDbl(0, 0);
		Double hHeight = this->pageLineHeight * 0.5;
		Double dHeight = UOSInt2Double(dimg->GetHeight());
		UOSInt i;
		UOSInt j;
		UTF8Char c;
		UTF32Char wc;
		Data::ByteArray currPtr;
		UOSInt k;
		UOSInt readBuffSize = (this->pageLineCnt + 1) * 16;
		const UTF8Char *textPtr;
		const UTF8Char *textPtr2;
		UOSInt textSkip;
		UInt64 drawOfst;
		const IO::FileAnalyse::FrameDetail::FieldInfo *fieldInfo = 0;
		if (this->frame)
		{
			Data::ArrayList<const IO::FileAnalyse::FrameDetail::FieldInfo*> fieldList;
			this->frame->GetFieldInfos(this->currOfst, fieldList);
			if (fieldList.GetCount() > 0)
			{
				fieldInfo = fieldList.GetItem(0);
			}
		}
		Data::ByteBuffer readBuff(readBuffSize + 1);
		readBuffSize = this->GetFileData(currOfst, readBuffSize, readBuff);
		currPtr = readBuff;
		readBuff[readBuffSize] = 0;
		k = 0;
		textSkip = 0;

		while (currOfst < this->fileSize && currPos.y < dHeight)
		{
			currPos.x = 0;
			i = 0;
			if (this->fileSize < 0x100000000)
			{
				Text::StrHexVal32(sbuff, (UInt32)currOfst);
			}
			else
			{
				Text::StrHexVal64(sbuff, currOfst);
			}
			while (true)
			{
				c = sbuff[i];
				if (c == 0)
				{
					break;
				}
				sbuff2[0] = c;
				sbuff2[1] = 0;
				dimg->DrawString(currPos, {sbuff2, 1}, f, lineNumBrush);
				currPos.x += hHeight;
				i++;
			}
			currPos.x += hHeight;

			if (readBuffSize >= 16)
			{
				k = 16;
			}
			else
			{
				k = readBuffSize;
			}
			readBuffSize -= k;
			textPtr = currPtr.Ptr();
			j = 0;
			while (j < k)
			{
				drawOfst = currOfst + j;
				if (this->frame && (drawOfst >= this->frame->GetOffset()) && (drawOfst < this->frame->GetOffset() + this->frame->GetSize()))
				{
					if (j + 1 == k || (drawOfst + 1 == this->frame->GetOffset() + this->frame->GetSize()))
					{
						dimg->DrawRect(currPos, Math::Size2DDbl(this->pageLineHeight, this->pageLineHeight), 0, frameBrush);
					}
					else
					{
						dimg->DrawRect(currPos, Math::Size2DDbl(this->pageLineHeight * 1.5, this->pageLineHeight), 0, frameBrush);
					}
					if (fieldInfo && drawOfst >= this->frame->GetOffset() + fieldInfo->ofst && drawOfst < this->frame->GetOffset() + fieldInfo->ofst + fieldInfo->size)
					{
						if (j + 1 == k || drawOfst + 1 == this->frame->GetOffset() + fieldInfo->ofst + fieldInfo->size)
						{
							dimg->DrawRect(currPos, Math::Size2DDbl(this->pageLineHeight, this->pageLineHeight), 0, fieldBrush);
						}
						else
						{
							dimg->DrawRect(currPos, Math::Size2DDbl(this->pageLineHeight * 1.5, this->pageLineHeight), 0, fieldBrush);
						}
					}
				}
				Media::DrawBrush *tBrush = textBrush;
				if (this->currOfst == drawOfst)
				{
					dimg->DrawRect(currPos, Math::Size2DDbl(this->pageLineHeight, this->pageLineHeight), 0, selBrush);
					tBrush = selTextBrush;
				}
				Text::StrHexByte(sbuff, *currPtr++);
				sbuff2[0] = sbuff[0];
				sbuff2[1] = 0;
				dimg->DrawString(currPos, {sbuff2, 1}, f, tBrush);
				currPos.x += hHeight;
				sbuff2[0] = sbuff[1];
				sbuff2[1] = 0;
				dimg->DrawString(currPos, {sbuff2, 1}, f, tBrush);
				currPos.x += hHeight * 2;
				j++;
			}

			if (k < 16)
			{
				currPos.x += hHeight * 3 * UOSInt2Double(16 - k);
			}
			j = textSkip;
			if (textSkip > 0)
			{
				currPos.x += hHeight * UOSInt2Double(textSkip);
				textPtr += textSkip;
				textSkip = 0;
			}
			while (j < k)
			{
				if (Text::CharUtil::UTF8CharValid(textPtr))
				{
					textPtr2 = Text::StrReadChar(textPtr, &wc);
					if (wc < 32)
					{
						dimg->DrawString(currPos, CSTR("."), f, textBrush);
					}
					else
					{
						sptr = Text::StrWriteChar(sbuff, wc);
						*sptr = 0;
						dimg->DrawString(currPos, CSTRP(sbuff, sptr), f, textBrush);
					}
					currPos.x += hHeight * OSInt2Double(textPtr2 - textPtr);
					j += (UOSInt)(textPtr2 - textPtr);
					textPtr = textPtr2;
				}
				else
				{
					dimg->DrawString(currPos, CSTR("."), f, textBrush);
					textPtr++;
					currPos.x += hHeight;
					j++;
				}
			}
			if (j > k)
			{
				textSkip = j - k;
			}

			currOfst += 16;
			currPos.y += this->pageLineHeight;
		}
		dimg->DelBrush(fieldBrush);
		dimg->DelBrush(frameBrush);
		dimg->DelBrush(selBrush);
		dimg->DelBrush(selTextBrush);
		dimg->DelBrush(textBrush);
		dimg->DelBrush(lineNumBrush);
		dimg->DelFont(f);
	}
}

void UI::GUIHexFileView::UpdateCaretPos()
{
	OSInt posV = this->GetScrollVPos();
	Double currY = this->pageLineHeight * OSInt2Double((OSInt)(this->currOfst >> 4) - posV);
	Double currX;
	UOSInt charCnt;
	if (this->fileSize < 0x100000000)
	{
		charCnt = 9;
	}
	else
	{
		charCnt = 17;
	}
	charCnt += (UOSInt)(this->currOfst & 15) * 3;
	currX = UOSInt2Double(charCnt) * this->pageLineHeight * 0.5;
	this->SetCaretPos(Double2OSInt(currX), Double2OSInt(currY));
}

Bool UI::GUIHexFileView::LoadFile(Text::CStringNN fileName, Bool dynamicSize)
{
	if (dynamicSize)
	{
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		if (fs->IsError())
		{
			DEL_CLASS(fs);
			return false;
		}
		SDEL_CLASS(this->analyse);
		SDEL_CLASS(this->fs);
		SDEL_CLASS(this->fd);
		SDEL_CLASS(this->frame);
		this->fs = fs;
		this->fileSize = 0;
		this->currOfst = 0;
	}
	else
	{
		NotNullPtr<IO::StmData::FileData> fd;
		NEW_CLASSNN(fd, IO::StmData::FileData(fileName, false));
		if (fd->IsError())
		{
			fd.Delete();
			return false;
		}
		SDEL_CLASS(this->analyse);
		SDEL_CLASS(this->fs);
		SDEL_CLASS(this->fd);
		SDEL_CLASS(this->frame);
		this->fd = fd.Ptr();
		this->analyse = IO::FileAnalyse::IFileAnalyse::AnalyseFile(fd);
		this->fileSize = this->fd->GetDataSize();
		this->currOfst = 0;
		this->SetScrollVRange(0, (UOSInt)(this->fileSize >> 4));
		this->GoToOffset(0);
	}
	this->Redraw();
	return true;
}

Bool UI::GUIHexFileView::LoadData(NotNullPtr<IO::StreamData> data, IO::FileAnalyse::IFileAnalyse *fileAnalyse)
{
	SDEL_CLASS(this->analyse);
	SDEL_CLASS(this->fs);
	SDEL_CLASS(this->fd);
	SDEL_CLASS(this->frame);
	this->fd = data.Ptr();
	if (fileAnalyse)
	{
		this->analyse = fileAnalyse;
	}
	else
	{
		this->analyse = IO::FileAnalyse::IFileAnalyse::AnalyseFile(data);
	}
	this->fileSize = this->fd->GetDataSize();
	this->currOfst = 0;
	this->SetScrollVRange(0, (UOSInt)(this->fileSize >> 4));
	this->GoToOffset(0);
	this->Redraw();
	return true;

}

void UI::GUIHexFileView::GetTextPos(Double scnPosX, Double scnPosY, UInt64 *byteOfst)
{
	UOSInt addrLen;
	if (this->fileSize < 0x100000000)
	{
		addrLen = 9;
	}
	else
	{
		addrLen = 17;
	}

	OSInt vPos = this->GetScrollVPos();
	UInt64 ofst;
	vPos = (OSInt)(OSInt2Double(vPos) + scnPosY / this->pageLineHeight);
	if (vPos < 0)
	{
		vPos = 0;
	}
	if (scnPosX < UOSInt2Double(addrLen) * this->pageLineHeight * 0.5)
	{
		ofst = (UOSInt)vPos * 16;
	}
	else if (scnPosX >= UOSInt2Double(addrLen + 48) * this->pageLineHeight * 0.5)
	{
		ofst = (UOSInt)vPos * 16 + 15;
	}
	else
	{
		ofst = (UOSInt)vPos * 16 + ((UOSInt)(scnPosX / this->pageLineHeight * 2.0) - addrLen) / 3;
	}
	if (this->fileSize == 0)
	{
		ofst = 0;
	}
	else if (ofst >= this->fileSize)
	{
		ofst = this->fileSize - 1;
	}
	*byteOfst = ofst;
}

void UI::GUIHexFileView::GoToOffset(UInt64 ofst)
{
	if (ofst >= this->fileSize)
	{
		return;
	}
	if (ofst == this->currOfst)
	{
		return;
	}
	if (this->analyse)
	{
		if (this->frame == 0 || ofst < this->frame->GetOffset() || ofst >= this->frame->GetOffset() + this->frame->GetSize())
		{
			SDEL_CLASS(this->frame);
			UOSInt i = this->analyse->GetFrameIndex(ofst);
			if (i != INVALID_INDEX)
			{
				this->frame = this->analyse->GetFrameDetail(i);
			}
		}
	}

	OSInt vPos = this->GetScrollVPos();
	this->currOfst = ofst;
	if (vPos > (OSInt)(ofst >> 4))
	{
		this->SetScrollVPos((UOSInt)(ofst >> 4), true);
	}
	else if (vPos + (Int32)this->pageLineCnt <= (Int64)(ofst >> 4))
	{
		this->SetScrollVPos((UOSInt)(ofst >> 4) - this->pageLineCnt + 1, true);
	}
	this->Redraw();
	this->UpdateCaretPos();
	UOSInt i = this->hdlrList.GetCount();
	while (i-- > 0)
	{
		this->hdlrList.GetItem(i)(this->hdlrObjList.GetItem(i), ofst);
	}
}

UInt64 UI::GUIHexFileView::GetCurrOfst()
{
	return this->currOfst;
}

UInt64 UI::GUIHexFileView::GetFileSize()
{
	return this->fileSize;
}

UOSInt UI::GUIHexFileView::GetFileData(UInt64 ofst, UOSInt size, Data::ByteArray outBuff)
{
	if (this->fd)
	{
		return this->fd->GetRealData(ofst, size, outBuff);
	}
	else if (this->fs)
	{
		this->fs->SeekFromBeginning(ofst);
		return this->fs->Read(outBuff.WithSize(size));
	}
	else
	{
		return 0;
	}
}

void UI::GUIHexFileView::HandleOffsetChg(OffsetChgHandler hdlr, void *hdlrObj)
{
	this->hdlrObjList.Add(hdlrObj);
	this->hdlrList.Add(hdlr);
}

Text::CString UI::GUIHexFileView::GetAnalyzerName()
{
	if (this->analyse)
	{
		return this->analyse->GetFormatName();
	}
	return CSTR_NULL;
}

Bool UI::GUIHexFileView::GetFrameName(NotNullPtr<Text::StringBuilderUTF8> sb)
{
	if (this->analyse == 0)
	{
		return false;
	}
	UOSInt index = this->analyse->GetFrameIndex(this->currOfst);
	if (index == INVALID_INDEX)
	{
		return false;
	}
	return this->analyse->GetFrameName(index, sb);
}

UOSInt UI::GUIHexFileView::GetFieldInfos(NotNullPtr<Data::ArrayList<const IO::FileAnalyse::FrameDetail::FieldInfo *>> fieldList)
{
	if (this->frame)
	{
		UOSInt i = this->frame->GetFieldInfos(this->currOfst, fieldList);
		if (i > 0)
		{
			const IO::FileAnalyse::FrameDetail::FieldInfo *field;
			UOSInt k = fieldList->GetCount();
			UOSInt j = k - i;
			while (j < k)
			{
				field = fieldList->GetItem(j);
				if (field->fieldType == IO::FileAnalyse::FrameDetail::FT_SUBFRAME)
				{
					j = this->analyse->GetFrameIndex(this->currOfst);
					if (j == INVALID_INDEX)
					{
						return i;
					}
					DEL_CLASS(this->frame);
					this->frame = this->analyse->GetFrameDetail(j);
					fieldList->RemoveRange(k - i, i);
					return this->frame->GetFieldInfos(this->currOfst, fieldList);
				}
				j++;
			}
		}
		return i;
	}
	return 0;
}

UOSInt UI::GUIHexFileView::GetAreaInfos(NotNullPtr<Data::ArrayList<const IO::FileAnalyse::FrameDetail::FieldInfo *>> areaList)
{
	if (this->frame)
	{
		return this->frame->GetAreaInfos(this->currOfst, areaList);
	}
	return 0;
}

Bool UI::GUIHexFileView::GoToNextUnkField()
{
	if (this->analyse == 0)
	{
		return false;
	}
	UInt64 currOfst = this->currOfst;
	UOSInt index = this->analyse->GetFrameIndex(currOfst);
	if (index == INVALID_INDEX)
	{
		return true;
	}
	IO::FileAnalyse::FrameDetail *frame = this->analyse->GetFrameDetail(index);
	Data::ArrayList<const IO::FileAnalyse::FrameDetail::FieldInfo*> fieldList;
	const IO::FileAnalyse::FrameDetail::FieldInfo *field;
	if (frame == 0)
	{
		return true;
	}
	while (currOfst < this->fileSize)
	{
		if (currOfst >= frame->GetOffset() + frame->GetSize())
		{
			DEL_CLASS(frame);
			index = this->analyse->GetFrameIndex(currOfst);
			if (index == INVALID_INDEX)
			{
				this->GoToOffset(currOfst);
				return true;
			}
			frame = this->analyse->GetFrameDetail(index);
			if (frame == 0)
			{
				this->GoToOffset(currOfst);
				return true;
			}
			if (currOfst < frame->GetOffset() || currOfst >= frame->GetOffset() + frame->GetSize())
			{
				this->GoToOffset(currOfst);
				DEL_CLASS(frame);
				return true;
			}
		}
		fieldList.Clear();
		if (frame->GetFieldInfos(currOfst, fieldList) == 0)
		{
			this->GoToOffset(currOfst);
			DEL_CLASS(frame);
			return true;
		}
		field = fieldList.GetItem(0);
		currOfst = frame->GetOffset() + field->ofst + field->size;
	}
	DEL_CLASS(frame);
	return false;
}
