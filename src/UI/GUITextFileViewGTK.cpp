#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/Encoding.h"
#include "Sync/Interlocked.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Win32/Clipboard.h"
#include "UI/GUITextFileView.h"
#include "UI/MessageDialog.h"

#define READBUFFSIZE 1048576

OSInt UI::GUITextFileView::useCnt = 0;

UInt32 __stdcall UI::GUITextFileView::ProcThread(void *userObj)
{
	OSInt lineCurr;
	WChar lastC;
	WChar c;
	UI::GUITextFileView *me = (UI::GUITextFileView*)userObj;
	me->threadRunning = true;
	while (!me->threadToStop)
	{
		if (me->loadNewFile)
		{
			if (me->isSearching)
			{
				SDEL_TEXT(me->srchText);
				me->isSearching = false;
			}
			me->mut->Lock();
			me->loadNewFile = false;
			if (me->fs)
			{
				DEL_CLASS(me->fs);
			}
			NEW_CLASS(me->fs, IO::FileStream(me->fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_SEQUENTIAL));
			me->lineOfsts->Clear();
			me->readingFile = true;
			me->readBuffOfst = 0;
			me->readBuffSize = me->fs->Read(me->readBuff, READBUFFSIZE);
			me->fileCodePage = me->codePage;
			me->mut->Unlock();

			if (me->readBuffSize >= 3)
			{
				if (me->readBuff[0] == 0xff && me->readBuff[1] == 0xfe)
				{
					me->fileCodePage = 1200;
					me->readBuffSize -= 2;
					me->readBuffOfst = 2;
					MemCopyO(me->readBuff, &me->readBuff[2], me->readBuffSize);
				}
				else if (me->readBuff[0] == 0xfe && me->readBuff[1] == 0xff)
				{
					me->fileCodePage = 1201;
					me->readBuffSize -= 2;
					me->readBuffOfst = 2;
					MemCopyO(me->readBuff, &me->readBuff[2], me->readBuffSize);
				}
				else if (me->readBuff[0] == 0xef && me->readBuff[1] == 0xbb && me->readBuff[2] == 0xbf)
				{
					me->fileCodePage = 65001;
					me->readBuffSize -= 3;
					me->readBuffOfst = 3;
					MemCopyO(me->readBuff, &me->readBuff[3], me->readBuffSize);
				}
			}

			me->mut->Lock();
			me->lineOfsts->Add(me->readBuffOfst);
			me->mut->Unlock();
			lastC = 0;
			if (me->fileCodePage == 1200)
			{
				while (me->readBuffSize > 0)
				{
					if (me->loadNewFile || me->threadToStop)
						break;

					me->mut->Lock();
					lineCurr = 0;
					while (lineCurr < me->readBuffSize)
					{
						c = *(WChar*)&me->readBuff[lineCurr];
						if (lastC == 0xd)
						{
							if (c == 0xa)
							{
								c = 0;
								me->lineOfsts->Add(me->readBuffOfst + lineCurr + 1);
							}
							else
							{
								me->lineOfsts->Add(me->readBuffOfst + lineCurr);
							}
						}
						else if (lastC == 0xa)
						{
							if (c == 0xd)
							{
								c = 0;
								me->lineOfsts->Add(me->readBuffOfst + lineCurr + 1);
							}
							else
							{
								me->lineOfsts->Add(me->readBuffOfst + lineCurr);
							}
						}
						lastC = c;
						lineCurr += 2;
					}
					me->readBuffOfst += me->readBuffSize;

					me->fs->Seek(IO::SeekableStream::ST_BEGIN, me->readBuffOfst);
					me->readBuffSize = me->fs->Read(me->readBuff, READBUFFSIZE);
					me->mut->Unlock();
				}
			}
			else if (me->fileCodePage == 1201)
			{
				while (me->readBuffSize > 0)
				{
					if (me->loadNewFile || me->threadToStop)
						break;

					lineCurr = 0;
					me->mut->Lock();
					while (lineCurr < me->readBuffSize)
					{
						c = (me->readBuff[lineCurr] << 8) | me->readBuff[lineCurr + 1];
						if (lastC == 0xd)
						{
							if (c == 0xa)
							{
								c = 0;
								me->lineOfsts->Add(me->readBuffOfst + lineCurr + 1);
							}
							else
							{
								me->lineOfsts->Add(me->readBuffOfst + lineCurr);
							}
						}
						else if (lastC == 0xa)
						{
							if (c == 0xd)
							{
								c = 0;
								me->lineOfsts->Add(me->readBuffOfst + lineCurr + 1);
							}
							else
							{
								me->lineOfsts->Add(me->readBuffOfst + lineCurr);
							}
						}
						lastC = c;
						lineCurr += 2;
					}
					me->readBuffOfst += me->readBuffSize;

					me->fs->Seek(IO::SeekableStream::ST_BEGIN, me->readBuffOfst);
					me->readBuffSize = me->fs->Read(me->readBuff, READBUFFSIZE);
					me->mut->Unlock();
				}
			}
			else
			{
				while (me->readBuffSize > 0)
				{
					if (me->loadNewFile || me->threadToStop)
						break;

					lineCurr = 0;
					me->mut->Lock();
					while (lineCurr < me->readBuffSize)
					{
						c = me->readBuff[lineCurr];
						if (lastC == 0xd)
						{
							if (c == 0xa)
							{
								c = 0;
								me->lineOfsts->Add(me->readBuffOfst + lineCurr + 1);
							}
							else
							{
								me->lineOfsts->Add(me->readBuffOfst + lineCurr);
							}
						}
						else if (lastC == 0xa)
						{
							if (c == 0xd)
							{
								c = 0;
								me->lineOfsts->Add(me->readBuffOfst + lineCurr + 1);
							}
							else
							{
								me->lineOfsts->Add(me->readBuffOfst + lineCurr);
							}
						}
						lastC = c;
						lineCurr += 1;
					}
					me->readBuffOfst += me->readBuffSize;

					me->fs->Seek(IO::SeekableStream::ST_BEGIN, me->readBuffOfst);
					me->readBuffSize = me->fs->Read(me->readBuff, READBUFFSIZE);
					me->mut->Unlock();
				}
			}
			if (me->loadNewFile || me->threadToStop)
			{
			}
			else
			{
				me->lineOfsts->Add(me->readBuffOfst);
			}
			me->fileSize = me->readBuffOfst;
			me->readingFile = false;
		}
		else if (me->fs)
		{
			Int64 size;
			me->mut->Lock();
			size = me->fs->GetLength();
			me->mut->Unlock();
			if (size > me->fileSize)
			{
				OSInt i;
				me->mut->Lock();
				i = me->lineOfsts->GetCount();
				me->lineOfsts->RemoveAt(i - 1);
				me->readBuffOfst = me->lineOfsts->GetItem(i - 2);
				me->readingFile = true;
				me->fs->Seek(IO::SeekableStream::ST_BEGIN, me->readBuffOfst);
				me->readBuffSize = me->fs->Read(me->readBuff, READBUFFSIZE);
				me->mut->Unlock();
			
				lastC = 0;
				if (me->fileCodePage == 1200)
				{
					while (me->readBuffSize > 0)
					{
						if (me->loadNewFile || me->threadToStop)
							break;

						me->mut->Lock();
						lineCurr = 0;
						while (lineCurr < me->readBuffSize)
						{
							c = *(WChar*)&me->readBuff[lineCurr];
							if (lastC == 0xd)
							{
								if (c == 0xa)
								{
									c = 0;
									me->lineOfsts->Add(me->readBuffOfst + lineCurr + 1);
								}
								else
								{
									me->lineOfsts->Add(me->readBuffOfst + lineCurr);
								}
							}
							else if (lastC == 0xa)
							{
								if (c == 0xd)
								{
									c = 0;
									me->lineOfsts->Add(me->readBuffOfst + lineCurr + 1);
								}
								else
								{
									me->lineOfsts->Add(me->readBuffOfst + lineCurr);
								}
							}
							lastC = c;
							lineCurr += 2;
						}
						me->readBuffOfst += me->readBuffSize;

						me->fs->Seek(IO::SeekableStream::ST_BEGIN, me->readBuffOfst);
						me->readBuffSize = me->fs->Read(me->readBuff, READBUFFSIZE);
						me->mut->Unlock();
					}
				}
				else if (me->fileCodePage == 1201)
				{
					while (me->readBuffSize > 0)
					{
						if (me->loadNewFile || me->threadToStop)
							break;

						lineCurr = 0;
						me->mut->Lock();
						while (lineCurr < me->readBuffSize)
						{
							c = (me->readBuff[lineCurr] << 8) | me->readBuff[lineCurr + 1];
							if (lastC == 0xd)
							{
								if (c == 0xa)
								{
									c = 0;
									me->lineOfsts->Add(me->readBuffOfst + lineCurr + 1);
								}
								else
								{
									me->lineOfsts->Add(me->readBuffOfst + lineCurr);
								}
							}
							else if (lastC == 0xa)
							{
								if (c == 0xd)
								{
									c = 0;
									me->lineOfsts->Add(me->readBuffOfst + lineCurr + 1);
								}
								else
								{
									me->lineOfsts->Add(me->readBuffOfst + lineCurr);
								}
							}
							lastC = c;
							lineCurr += 2;
						}
						me->readBuffOfst += me->readBuffSize;

						me->fs->Seek(IO::SeekableStream::ST_BEGIN, me->readBuffOfst);
						me->readBuffSize = me->fs->Read(me->readBuff, READBUFFSIZE);
						me->mut->Unlock();
					}
				}
				else
				{
					while (me->readBuffSize > 0)
					{
						if (me->loadNewFile || me->threadToStop)
							break;

						lineCurr = 0;
						me->mut->Lock();
						while (lineCurr < me->readBuffSize)
						{
							c = me->readBuff[lineCurr];
							if (lastC == 0xd)
							{
								if (c == 0xa)
								{
									c = 0;
									me->lineOfsts->Add(me->readBuffOfst + lineCurr + 1);
								}
								else
								{
									me->lineOfsts->Add(me->readBuffOfst + lineCurr);
								}
							}
							else if (lastC == 0xa)
							{
								if (c == 0xd)
								{
									c = 0;
									me->lineOfsts->Add(me->readBuffOfst + lineCurr + 1);
								}
								else
								{
									me->lineOfsts->Add(me->readBuffOfst + lineCurr);
								}
							}
							lastC = c;
							lineCurr += 1;
						}
						me->readBuffOfst += me->readBuffSize;

						me->fs->Seek(IO::SeekableStream::ST_BEGIN, me->readBuffOfst);
						me->readBuffSize = me->fs->Read(me->readBuff, READBUFFSIZE);
						me->mut->Unlock();
					}
				}
				if (me->loadNewFile || me->threadToStop)
				{
				}
				else
				{
					me->lineOfsts->Add(me->readBuffOfst);
				}
				me->fileSize = me->readBuffOfst;
				me->readingFile = false;
			}
		}

		if (me->isSearching)
		{
			UInt8 *srchTxt;
			OSInt srchTxtLen;
			OSInt strLen;
			UInt32 startCaretX = me->caretX;
			UOSInt startCaretY = me->caretY;
			UInt32 srchCaretX;
			UOSInt srchCaretY;
			Int64 startLineOfst = me->lineOfsts->GetItem(startCaretY);
			Int64 nextLineOfst = me->lineOfsts->GetItem(startCaretY + 1);
			UInt8 *srchBuff;
			Int64 currOfst;
			OSInt currSize;
			OSInt srchIndex;
			Bool found = false;

			if (me->fs)
			{
				Text::Encoding enc(me->fileCodePage);
				strLen = Text::StrCharCnt(me->srchText);
				srchTxtLen = enc.UTF8CountBytes(me->srchText, strLen);
				srchTxt = MemAlloc(UInt8, srchTxtLen + 1);
				enc.UTF8ToBytes(srchTxt, me->srchText, strLen);
				srchTxt[srchTxtLen] = 0;

				srchBuff = MemAlloc(UInt8, READBUFFSIZE + 1);
				currOfst = startLineOfst;
				while (true)
				{
					me->mut->Lock();
					if (me->fs)
					{
						me->fs->Seek(IO::SeekableStream::ST_BEGIN, currOfst);
						currSize = me->fs->Read(srchBuff, READBUFFSIZE);
						srchBuff[currSize] = 0;
					}
					else
					{
						currSize = 0;
					}
					me->mut->Unlock();
					if (currSize <= srchTxtLen)
					{
						break;
					}

					srchIndex = Text::StrIndexOf((Char*)srchBuff, (Char*)srchTxt);
					if (srchIndex >= 0)
					{
						me->GetPosFromByteOfst(currOfst + srchIndex, &srchCaretX, &srchCaretY);
						if (srchCaretY == startCaretY && srchCaretX <= startCaretX)
						{
							OSInt tmpIndex;
							while (true)
							{
								tmpIndex = Text::StrIndexOf((Char*)srchBuff + srchIndex + 1, (Char*)srchTxt);
								if (tmpIndex < 0)
									break;
								
								me->GetPosFromByteOfst(currOfst + srchIndex + 1 + tmpIndex, &srchCaretX, &srchCaretY);
								if (srchCaretY == startCaretY && srchCaretX <= startCaretX)
								{
									srchIndex += 1 + tmpIndex;
								}
								else
								{
									me->selStartX = srchCaretX;
									me->selStartY = srchCaretY;
									me->selEndX = srchCaretX + (Int32)strLen;
									me->selEndY = srchCaretY;
									me->GoToText(srchCaretY, srchCaretX);
									me->Redraw();
									found = true;
									break;
								}
							}
							if (found)
							{
								break;
							}
						}
						else
						{
							me->selStartX = srchCaretX;
							me->selStartY = srchCaretY;
							me->selEndX = srchCaretX + (Int32)strLen;
							me->selEndY = srchCaretY;
							me->GoToText(srchCaretY, srchCaretX);
							me->Redraw();
							found = true;
							break;
						}
					}
					currOfst += currSize - srchTxtLen + 1;
				}

				if (!found)
				{
					currOfst = 0;
					while (currOfst < nextLineOfst)
					{
						me->mut->Lock();
						if (me->fs)
						{
							currSize = (OSInt)(nextLineOfst - currOfst);
							if (currSize > READBUFFSIZE)
							{
								currSize = READBUFFSIZE;
							}
							me->fs->Seek(IO::SeekableStream::ST_BEGIN, currOfst);
							currSize = me->fs->Read(srchBuff, currSize);
							srchBuff[currSize] = 0;
						}
						else
						{
							currSize = 0;
						}
						me->mut->Unlock();
						if (currSize <= srchTxtLen)
						{
							break;
						}

						srchIndex = Text::StrIndexOf((Char*)srchBuff, (Char*)srchTxt);
						if (srchIndex >= 0)
						{
							me->GetPosFromByteOfst(currOfst + srchIndex, &srchCaretX, &srchCaretY);
							if (srchCaretY < startCaretY || srchCaretX < startCaretX)
							{
								me->selStartX = srchCaretX;
								me->selStartY = srchCaretY;
								me->selEndX = srchCaretX + (Int32)strLen;
								me->selEndY = srchCaretY;
								me->GoToText(srchCaretY, srchCaretX);
								me->Redraw();
								found = true;
								break;
							}
						}
						currOfst += currSize - srchTxtLen + 1;
					}
				}

				MemFree(srchBuff);
				MemFree(srchTxt);
			}
			SDEL_TEXT(me->srchText);
			me->isSearching = false;
		}
		me->evtThread->Wait(1000);
	}
	me->threadRunning = false;
	return 0;
}

void UI::GUITextFileView::Init(void *hInst)
{
}

void UI::GUITextFileView::Deinit(void *hInst)
{
}

void UI::GUITextFileView::OnPaint()
{
}

void UI::GUITextFileView::UpdateScrollBar()
{
}

void UI::GUITextFileView::CopySelected()
{
	if (this->selStartX == this->selEndX && this->selStartY == this->selEndY)
	{
		return;
	}

	OSInt selTopX;
	OSInt selTopY;
	OSInt selBottomX;
	OSInt selBottomY;
	selTopX = this->selStartX;
	selTopY = this->selStartY;
	selBottomX = this->selEndX;
	selBottomY = this->selEndY;
	if (selTopY > selBottomY)
	{
		selTopX = this->selEndX;
		selTopY = this->selEndY;
		selBottomX = this->selStartX;
		selBottomY = this->selStartY;
	}
	else if (selTopY < selBottomY)
	{
	}
	else if (selTopX > selBottomX)
	{
		selTopX = this->selEndX;
		selTopY = this->selEndY;
		selBottomX = this->selStartX;
		selBottomY = this->selStartY;
	}
	else if (selTopX < selBottomX)
	{
	}
	this->mut->Lock();
	if (this->fs == 0)
	{
		this->mut->Unlock();
		return;
	}

	Text::Encoding *enc;
	UInt8 *rbuff;
	UTF8Char *line;
	Int64 startOfst;
	Int64 endOfst;
	OSInt j;

	NEW_CLASS(enc, Text::Encoding(this->fileCodePage));
	if (selTopY == selBottomY)
	{
		startOfst = this->lineOfsts->GetItem(selTopY);
		endOfst = this->lineOfsts->GetItem(selTopY + 1);
		if (endOfst == 0)
		{
			this->mut->Unlock();
			DEL_CLASS(enc);
			return;
//			endOfst = this->fs->GetLength();
		}
		rbuff = MemAlloc(UInt8, (OSInt)(endOfst - startOfst));
		this->fs->Seek(IO::SeekableStream::ST_BEGIN, startOfst);
		this->fs->Read(rbuff, (OSInt)(endOfst - startOfst));
		this->mut->Unlock();

		j = enc->CountUTF8Chars(rbuff, (OSInt)(endOfst - startOfst));
		line = MemAlloc(UTF8Char, j + 1);
		enc->UTF8FromBytes(line, rbuff, (OSInt)(endOfst - startOfst), 0);
		line[selBottomX] = 0;
		Win32::Clipboard::SetString(this->hwnd, &line[selTopX]);
		MemFree(line);
		MemFree(rbuff);
	}
	else
	{
		Text::StringBuilderUTF8 sb;

		startOfst = this->lineOfsts->GetItem(selTopY);
		endOfst = this->lineOfsts->GetItem(selBottomY);
		if (endOfst == 0)
		{
			this->mut->Unlock();
			DEL_CLASS(enc);
			return;
//			endOfst = this->fs->GetLength();
		}
		else if (endOfst - startOfst > 1048576)
		{
			this->mut->Unlock();
			DEL_CLASS(enc);
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Failed to copy because selected area is too long", (const UTF8Char*)"TextViewer", this);
			return;
		}
		rbuff = MemAlloc(UInt8, (OSInt)(endOfst - startOfst));
		this->fs->Seek(IO::SeekableStream::ST_BEGIN, startOfst);
		this->fs->Read(rbuff, (OSInt)(endOfst - startOfst));

		j = enc->CountUTF8Chars(rbuff, (OSInt)(endOfst - startOfst));
		line = MemAlloc(UTF8Char, j + 1);
		enc->UTF8FromBytes(line, rbuff, (OSInt)(endOfst - startOfst), 0);
		sb.Append(&line[selTopX]);
		MemFree(line);
		MemFree(rbuff);

		startOfst = this->lineOfsts->GetItem(selBottomY);
		endOfst = this->lineOfsts->GetItem(selBottomY + 1);
		if (endOfst == 0)
		{
			this->mut->Unlock();
			return;
//			endOfst = this->fs->GetLength();
		}
		rbuff = MemAlloc(UInt8, (OSInt)(endOfst - startOfst));
		this->fs->Seek(IO::SeekableStream::ST_BEGIN, startOfst);
		this->fs->Read(rbuff, (OSInt)(endOfst - startOfst));
		this->mut->Unlock();
		j = enc->CountUTF8Chars(rbuff, (OSInt)(endOfst - startOfst));
		line = MemAlloc(UTF8Char, j + 1);
		enc->UTF8FromBytes(line, rbuff, (OSInt)(endOfst - startOfst), 0);
		line[selBottomX] = 0;
		sb.Append(line);
		MemFree(line);
		MemFree(rbuff);

		Win32::Clipboard::SetString(this->hwnd, sb.ToString());
	}
	DEL_CLASS(enc);
}

void UI::GUITextFileView::UpdateCaretPos()
{
}

void UI::GUITextFileView::EnsureCaretVisible()
{
}

UOSInt UI::GUITextFileView::GetLineCharCnt(UOSInt lineNum)
{
	Int64 lineOfst;
	Int64 nextOfst;
	UInt8 *rbuff;

	lineOfst = this->lineOfsts->GetItem(lineNum);
	nextOfst = this->lineOfsts->GetItem(lineNum + 1);
	if (nextOfst == 0 && this->lineOfsts->GetCount() - 1 <= lineNum + 1)
	{
		nextOfst = this->lineOfsts->GetItem(this->lineOfsts->GetCount() - 1);
	}
	if (nextOfst > lineOfst)
	{
		Text::Encoding enc(this->fileCodePage);
		rbuff = MemAlloc(UInt8, (OSInt)(nextOfst - lineOfst));
		this->fs->Seek(IO::SeekableStream::ST_BEGIN, lineOfst);
		this->fs->Read(rbuff, (OSInt)(nextOfst - lineOfst));
		UOSInt charCnt = enc.CountWChars(rbuff, (OSInt)(nextOfst - lineOfst));
		WChar *line = MemAlloc(WChar, charCnt + 1);
		enc.WFromBytes(line, rbuff, (OSInt)(nextOfst - lineOfst), 0);
		charCnt -= 1;
		if (charCnt > 0 && (line[charCnt - 1] == 0x0d || line[charCnt - 1] == 0x0a))
		{
			charCnt--;
		}
		MemFree(line);
		MemFree(rbuff);
		return charCnt;
	}
	else
	{
		return 0;
	}
}

void UI::GUITextFileView::GetPosFromByteOfst(Int64 byteOfst, UInt32 *txtPosX, UOSInt *txtPosY)
{
	OSInt lineNum = this->lineOfsts->SortedIndexOf(byteOfst);
	if (lineNum >= 0)
	{
		*txtPosX = 0;
		*txtPosY = lineNum;
		return;
	}
	lineNum = ~lineNum - 1;
	Int64 thisOfst = this->lineOfsts->GetItem(lineNum);
	OSInt buffSize = (OSInt)(byteOfst - thisOfst);
	UInt8 *rbuff = MemAlloc(UInt8, buffSize);
	Text::Encoding enc(this->fileCodePage);
	this->mut->Lock();
	this->fs->Seek(IO::SeekableStream::ST_BEGIN, thisOfst);
	this->fs->Read(rbuff, buffSize);
	this->mut->Unlock();
	*txtPosX = (Int32)enc.CountWChars(rbuff, buffSize);
	*txtPosY = lineNum;
	MemFree(rbuff);
}

void UI::GUITextFileView::UpdateCaretSel(Bool noRedraw)
{
}

void UI::GUITextFileView::EventTextPosUpdated()
{
	UOSInt i = this->textPosUpdHdlr->GetCount();
	while (i-- > 0)
	{
		this->textPosUpdHdlr->GetItem(i)(this->textPosUpdObj->GetItem(i), this->caretX, this->caretY);
	}
}

UI::GUITextFileView::GUITextFileView(UI::GUICore *ui, UI::GUIClientControl *parent) : UI::GUIControl(ui, parent)
{
	this->fileName = 0;
	this->threadToStop = false;
	this->threadRunning = false;
	this->loadNewFile = false;
	this->readingFile = false;
	this->readBuff = MemAlloc(UInt8, READBUFFSIZE);
	this->readBuffOfst = 0;
	this->readBuffSize = 0;
	this->fs = 0;
	this->codePage = 0;
	this->bgBmp = 0;
	this->lastLineCnt = -1;
	this->dispLineNumW = 0;
	this->pageLineCnt = 0;
	this->pageLineHeight = 12;
	this->mouseDown = false;
	this->selStartX = 0;
	this->selStartY = 0;
	this->selEndX = 0;
	this->selEndY = 0;
	this->caretX = 0;
	this->caretY = 0;
	this->isSearching = false;
	this->srchText = 0;
	this->fileSize = 0;
	NEW_CLASS(lineOfsts, Data::ArrayListInt64());
	NEW_CLASS(this->textPosUpdHdlr, Data::ArrayList<TextPosEvent>());
	NEW_CLASS(this->textPosUpdObj, Data::ArrayList<void *>());

	if (Sync::Interlocked::Increment(&useCnt) == 1)
	{
//		Init(((UI::GUICoreWin*)this->ui)->GetHInst());
	}
	NEW_CLASS(this->evtThread, Sync::Event(true, (const UTF8Char*)"UI.MSWindowTextFileView.evtThread"));
	NEW_CLASS(this->mut, Sync::Mutex());
	Sync::Thread::Create(ProcThread, this);

/*	Int32 style = WS_CHILD | WS_HSCROLL | WS_VSCROLL | WS_TABSTOP;
	if (parent->IsChildVisible())
	{
		style = style | WS_VISIBLE;
	}
	this->InitControl(((UI::GUICoreWin*)this->ui)->GetHInst(), parent, CLASSNAME, (const UTF8Char*)"", style, WS_EX_CONTROLPARENT, 0, 0, 200, 200);
	SetTimer((HWND)this->hwnd, 1, 1000, 0);*/
}

UI::GUITextFileView::~GUITextFileView()
{
//	KillTimer((HWND)this->hwnd, 1);
	this->threadToStop = true;
	this->evtThread->Set();
	while (this->threadRunning)
	{
		Sync::Thread::Sleep(10);
	}
	MemFree(this->readBuff);
	if (this->fs)
	{
		DEL_CLASS(this->fs);
	}
	if (this->fileName)
	{
		Text::StrDelNew(this->fileName);
		this->fileName = 0;
	}
/*	if (this->bgBmp)
	{
		DeleteObject((HBITMAP)this->bgBmp);
		this->bgBmp = 0;
	}*/
	DEL_CLASS(this->evtThread);
	DEL_CLASS(this->mut);
	DEL_CLASS(this->lineOfsts);
	DEL_CLASS(this->textPosUpdHdlr);
	DEL_CLASS(this->textPosUpdObj);
	SDEL_TEXT(this->srchText);
	if (Sync::Interlocked::Decrement(&useCnt) == 0)
	{
//		Deinit(((UI::GUICoreWin*)this->ui)->GetHInst());
	}
}

const UTF8Char *UI::GUITextFileView::GetObjectClass()
{
	return (const UTF8Char*)"TextFileView";
}

OSInt UI::GUITextFileView::OnNotify(Int32 code, void *lParam)
{
	return 0;
}

void UI::GUITextFileView::UpdateFont()
{
	this->drawFont = this->GetFont();
	UpdateScrollBar();
}

Bool UI::GUITextFileView::IsLoading()
{
	return this->readingFile;
}

UOSInt UI::GUITextFileView::GetLineCount()
{
	return this->lineOfsts->GetCount() - 1;
}

void UI::GUITextFileView::SetCodePage(Int32 codePage)
{
	this->mut->Lock();
	this->codePage = codePage;
	this->mut->Unlock();
}

Bool UI::GUITextFileView::LoadFile(const UTF8Char *fileName)
{
/*	while (this->isSearching)
	{
		Sync::Thread::Sleep(10);
	}

	this->mut->Lock();
	if (this->fileName)
	{
		Text::StrDelNew(this->fileName);
	}
	this->fileName = Text::StrCopyNew(fileName);
	this->loadNewFile = true;
	this->fileSize = 0;

	this->lastLineCnt = 0;
	SetScrollRange((HWND)this->hwnd, SB_VERT, 0, 0, TRUE);

	this->caretX = 0;
	this->caretY = 0;
	this->selStartX = 0;
	this->selStartY = 0;
	this->selEndX = 0;
	this->selEndY = 0;
	this->selLastX = 0;
	this->selLastY = 0;
	this->evtThread->Set();
	this->mut->Unlock();
	this->EventTextPosUpdated();*/
	return true;
}

const UTF8Char *UI::GUITextFileView::GetFileName()
{
	return this->fileName;
}

void UI::GUITextFileView::GetTextPos(Int32 scnPosX, Int32 scnPosY, UInt32 *textPosX, UOSInt *textPosY)
{
/*	OSInt textY = GetScrollPos((HWND)this->hwnd, SB_VERT) + (scnPosY / this->pageLineHeight);
	Int32 drawX;
	Int32 textX = 0;
	if (textY >= this->lineOfsts->GetCount() - 1)
	{
		textY = this->lineOfsts->GetCount() - 1;
		textX = 0;
		*textPosX = textX;
		*textPosY = textY;
		return;
	}
	drawX = scnPosX + GetScrollPos((HWND)this->hwnd, SB_HORZ) - this->dispLineNumW;
	if (drawX < 0)
	{
		textX = 0;
		*textPosX = textX;
		*textPosY = textY;
		return;
	}
	if (this->fs)
	{
		UInt8 *rbuff;
		Int64 lineOfst;
		Int64 nextOfst;
		this->mut->Lock();
		lineOfst = this->lineOfsts->GetItem(textY);
		nextOfst = this->lineOfsts->GetItem(textY + 1);
		if (nextOfst == 0 && this->lineOfsts->GetCount() - 1 <= textY + 1)
		{
			nextOfst = this->lineOfsts->GetItem(this->lineOfsts->GetCount() - 1);
		}
		if (nextOfst > lineOfst)
		{
			WChar *line;
			SIZE sz;

			Text::Encoding enc(this->fileCodePage);
			rbuff = MemAlloc(UInt8, (OSInt)(nextOfst - lineOfst));
			this->fs->Seek(IO::SeekableStream::ST_BEGIN, lineOfst);
			this->fs->Read(rbuff, (OSInt)(nextOfst - lineOfst));
			OSInt charCnt = enc.CountWChars(rbuff, (OSInt)(nextOfst - lineOfst));
			line = MemAlloc(WChar, charCnt + 1);
			enc.WFromBytes(line, rbuff, (OSInt)(nextOfst - lineOfst), 0);
			Text::StrReplace(line, '\t', ' ');
			charCnt -= 1;
			if (charCnt > 0 && (line[charCnt] == 0xd || line[charCnt] == 0xa))
			{
				charCnt--;
			}
			HDC hdc = GetDC((HWND)this->hwnd);
			void *fnt = this->GetFont();
			if (fnt)
			{
				SelectObject(hdc, fnt);
			}
			GetTextExtentExPoint(hdc, line, (Int32)charCnt, drawX, &textX, 0, &sz);
			ReleaseDC((HWND)this->hwnd, hdc);
			MemFree(line);
			MemFree(rbuff);
		}
		this->mut->Unlock();
	}
	*textPosX = textX;
	*textPosY = textY;*/
}

UOSInt UI::GUITextFileView::GetTextPosY()
{
	return this->caretY;
}

UInt32 UI::GUITextFileView::GetTextPosX()
{
	return this->caretX;
}

void UI::GUITextFileView::GoToText(UOSInt newPosY, UInt32 newPosX)
{
	UOSInt lineCharCnt;
	if (newPosY >= this->lineOfsts->GetCount())
	{
		newPosY = this->lineOfsts->GetCount() - 1;
	}
	if (newPosX > (lineCharCnt = this->GetLineCharCnt(newPosY)))
	{
		newPosX = (UInt32)lineCharCnt;
	}
	this->caretX = newPosX;
	this->caretY = newPosY;
	this->EnsureCaretVisible();
	this->EventTextPosUpdated();
}

void UI::GUITextFileView::SearchText(const UTF8Char *txt)
{
	if (this->fs && !this->isSearching)
	{
		SDEL_TEXT(this->srchText);
		this->srchText = Text::StrCopyNew(txt);
		this->isSearching = true;
		this->evtThread->Set();
	}
}

void UI::GUITextFileView::HandleTextPosUpdate(TextPosEvent hdlr, void *obj)
{
	this->textPosUpdHdlr->Add(hdlr);
	this->textPosUpdObj->Add(obj);
}
