#include "Stdafx.h"
#include "Math/Math.h"
#include "Text/CharUtil.h"
#include "UI/GUIHexFileView.h"

UI::GUIHexFileView::GUIHexFileView(UI::GUICore *ui, UI::GUIClientControl *parent, Media::DrawEngine *deng) : UI::GUITextView(ui, parent, deng)
{
	this->fs = 0;
	this->fileSize = 0;
	this->currOfst = 0;
	NEW_CLASS(this->hdlrList, Data::ArrayList<OffsetChgHandler>());
	NEW_CLASS(this->hdlrObjList, Data::ArrayList<void *>());
}

UI::GUIHexFileView::~GUIHexFileView()
{
	SDEL_CLASS(this->fs);
	DEL_CLASS(this->hdlrList);
	DEL_CLASS(this->hdlrObjList);
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
		this->GetTextPos(Math::OSInt2Double(scnX), Math::OSInt2Double(scnY), &ofst);
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

void UI::GUIHexFileView::DrawImage(Media::DrawImage *dimg)
{
	Media::DrawBrush *b = dimg->NewBrushARGB(this->bgColor);
	dimg->DrawRect(0, 0, Math::UOSInt2Double(dimg->GetWidth()), Math::UOSInt2Double(dimg->GetHeight()), 0, b);
	dimg->DelBrush(b);
	OSInt vPos = this->GetScrollVPos();
	if (this->fileSize > 0)
	{
		UTF8Char sbuff[17];
		UTF8Char sbuff2[2];
		UInt64 currOfst = ((UInt64)(UOSInt)vPos) * 16;
		Media::DrawFont *f = this->CreateDrawFont(dimg);
		Media::DrawBrush *lineNumBrush = dimg->NewBrushARGB(this->lineNumColor);
		Media::DrawBrush *textBrush = dimg->NewBrushARGB(this->txtColor);
		Media::DrawBrush *selBrush = dimg->NewBrushARGB(this->selColor);
		Double currY = 0;
		Double currX;
		Double hHeight = this->pageLineHeight * 0.5;
		Double dHeight = Math::UOSInt2Double(dimg->GetHeight());
		UOSInt i;
		UOSInt j;
		UTF8Char c;
		UTF32Char wc;
		UInt8 *readBuff;
		UInt8 *currPtr;
		UOSInt k;
		UOSInt readBuffSize = (this->pageLineCnt + 1) * 16;
		const UTF8Char *textPtr;
		const UTF8Char *textPtr2;
		UOSInt textSkip;
		readBuff = MemAlloc(UInt8, readBuffSize + 1);
		this->fs->SeekFromBeginning(currOfst);
		readBuffSize = this->fs->Read(readBuff, readBuffSize);
		currPtr = readBuff;
		readBuff[readBuffSize] = 0;
		k = 0;
		textSkip = 0;

		while (currOfst < this->fileSize && currY < dHeight)
		{
			currX = 0;
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
				dimg->DrawString(currX, currY, sbuff2, f, lineNumBrush);
				currX += hHeight;
				i++;
			}
			currX += hHeight;

			if (readBuffSize >= 16)
			{
				k = 16;
			}
			else
			{
				k = readBuffSize;
			}
			readBuffSize -= k;
			textPtr = currPtr;
			j = 0;
			while (j < k)
			{
				if (this->currOfst == currOfst + j)
				{
					dimg->DrawRect(currX, currY, this->pageLineHeight, this->pageLineHeight, 0, selBrush);
				}
				Text::StrHexByte(sbuff, *currPtr++);
				sbuff2[0] = sbuff[0];
				sbuff2[1] = 0;
				dimg->DrawString(currX, currY, sbuff2, f, textBrush);
				currX += hHeight;
				sbuff2[0] = sbuff[1];
				sbuff2[1] = 0;
				dimg->DrawString(currX, currY, sbuff2, f, textBrush);
				currX += hHeight * 2;
				j++;
			}

			if (k < 16)
			{
				currX += hHeight * 3 * Math::UOSInt2Double(16 - k);
			}
			j = textSkip;
			if (textSkip > 0)
			{
				currX += hHeight * Math::UOSInt2Double(textSkip);
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
						dimg->DrawString(currX, currY, (const UTF8Char*)".", f, textBrush);
					}
					else
					{
						Text::StrWriteChar(sbuff, wc)[0] = 0;
						dimg->DrawString(currX, currY, sbuff, f, textBrush);
					}
					currX += hHeight * Math::OSInt2Double(textPtr2 - textPtr);
					j += (UOSInt)(textPtr2 - textPtr);
					textPtr = textPtr2;
				}
				else
				{
					dimg->DrawString(currX, currY, (const UTF8Char*)".", f, textBrush);
					textPtr++;
					currX += hHeight;
					j++;
				}
			}
			if (j > k)
			{
				textSkip = j - k;
			}

			currOfst += 16;
			currY += this->pageLineHeight;
		}
		dimg->DelBrush(selBrush);
		dimg->DelBrush(textBrush);
		dimg->DelBrush(lineNumBrush);
		dimg->DelFont(f);
		MemFree(readBuff);
	}
}

void UI::GUIHexFileView::UpdateCaretPos()
{
	OSInt posV = this->GetScrollVPos();
	Double currY = this->pageLineHeight * Math::OSInt2Double((OSInt)(this->currOfst >> 4) - posV);
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
	currX = charCnt * this->pageLineHeight * 0.5;
	this->SetCaretPos(Math::Double2OSInt(currX), Math::Double2OSInt(currY));
}

Bool UI::GUIHexFileView::LoadFile(const UTF8Char *fileName)
{
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}
	SDEL_CLASS(this->fs);
	this->fs = fs;
	this->fileSize = 0;
	this->currOfst = 0;
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
	vPos = (OSInt)(Math::OSInt2Double(vPos) + scnPosY / this->pageLineHeight);
	if (vPos < 0)
	{
		vPos = 0;
	}
	if (scnPosX < Math::UOSInt2Double(addrLen) * this->pageLineHeight * 0.5)
	{
		ofst = (UOSInt)vPos * 16;
	}
	else if (scnPosX >= Math::UOSInt2Double(addrLen + 48) * this->pageLineHeight * 0.5)
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
	OSInt vPos = this->GetScrollVPos();
	this->currOfst = ofst;
	if (vPos > (OSInt)(ofst >> 4))
	{
		this->SetScrollVPos(ofst >> 4, true);
	}
	else if (vPos + (Int32)this->pageLineCnt <= (Int64)(ofst >> 4))
	{
		this->SetScrollVPos((ofst >> 4) - this->pageLineCnt + 1, true);
	}
	this->Redraw();
	this->UpdateCaretPos();
	UOSInt i = this->hdlrList->GetCount();
	while (i-- > 0)
	{
		this->hdlrList->GetItem(i)(this->hdlrObjList->GetItem(i), ofst);
	}
}

UInt64 UI::GUIHexFileView::GetCurrOfst()
{
	return this->currOfst;
}

UOSInt UI::GUIHexFileView::GetFileData(UInt64 ofst, UOSInt size, UInt8 *outBuff)
{
	if (this->fs == 0)
	{
		return 0;
	}
	this->fs->SeekFromBeginning(ofst);
	return this->fs->Read(outBuff, size);
}

void UI::GUIHexFileView::HandleOffsetChg(OffsetChgHandler hdlr, void *hdlrObj)
{
	this->hdlrObjList->Add(hdlrObj);
	this->hdlrList->Add(hdlr);
}
