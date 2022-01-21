#include "Stdafx.h"
#include "MyMemory.h"
#include "Math/Math.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Win32/Clipboard.h"
#include "UI/GUITextFileView.h"
#include "UI/MessageDialog.h"

#define READBUFFSIZE 1048576

UInt32 __stdcall UI::GUITextFileView::ProcThread(void *userObj)
{
	UOSInt lineCurr;
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
			Sync::MutexUsage mutUsage(me->mut);
			me->loadNewFile = false;
			if (me->fs)
			{
				DEL_CLASS(me->fs);
			}
			NEW_CLASS(me->fs, IO::FileStream(me->fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
			me->lineOfsts->Clear();
			me->readingFile = true;
			me->readBuffOfst = 0;
			me->readBuffSize = me->fs->Read(me->readBuff, READBUFFSIZE);
			me->fileCodePage = me->codePage;
			mutUsage.EndUse();

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

			mutUsage.ReplaceMutex(me->mut);
			me->lineOfsts->Add(me->readBuffOfst);
			mutUsage.EndUse();
			lastC = 0;
			if (me->fileCodePage == 1200)
			{
				while (me->readBuffSize > 0)
				{
					if (me->loadNewFile || me->threadToStop)
						break;

					Sync::MutexUsage mutUsage(me->mut);
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

					me->fs->SeekFromBeginning(me->readBuffOfst);
					me->readBuffSize = me->fs->Read(me->readBuff, READBUFFSIZE);
					mutUsage.EndUse();
				}
			}
			else if (me->fileCodePage == 1201)
			{
				while (me->readBuffSize > 0)
				{
					if (me->loadNewFile || me->threadToStop)
						break;

					lineCurr = 0;
					Sync::MutexUsage mutUsage(me->mut);
					while (lineCurr < me->readBuffSize)
					{
						c = (WChar)((me->readBuff[lineCurr] << 8) | me->readBuff[lineCurr + 1]);
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

					me->fs->SeekFromBeginning(me->readBuffOfst);
					me->readBuffSize = me->fs->Read(me->readBuff, READBUFFSIZE);
					mutUsage.EndUse();
				}
			}
			else
			{
				while (me->readBuffSize > 0)
				{
					if (me->loadNewFile || me->threadToStop)
						break;

					lineCurr = 0;
					Sync::MutexUsage mutUsage(me->mut);
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

					me->fs->SeekFromBeginning(me->readBuffOfst);
					me->readBuffSize = me->fs->Read(me->readBuff, READBUFFSIZE);
					mutUsage.EndUse();
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
			UInt64 size;
			Sync::MutexUsage mutUsage(me->mut);
			size = me->fs->GetLength();
			mutUsage.EndUse();
			if (size > me->fileSize)
			{
				UOSInt i;
				Sync::MutexUsage mutUsage(me->mut);
				i = me->lineOfsts->GetCount();
				me->lineOfsts->RemoveAt(i - 1);
				me->readBuffOfst = me->lineOfsts->GetItem(i - 2);
				me->readingFile = true;
				me->fs->SeekFromBeginning(me->readBuffOfst);
				me->readBuffSize = me->fs->Read(me->readBuff, READBUFFSIZE);
				mutUsage.EndUse();
			
				lastC = 0;
				if (me->fileCodePage == 1200)
				{
					while (me->readBuffSize > 0)
					{
						if (me->loadNewFile || me->threadToStop)
							break;

						Sync::MutexUsage mutUsage(me->mut);
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

						me->fs->SeekFromBeginning(me->readBuffOfst);
						me->readBuffSize = me->fs->Read(me->readBuff, READBUFFSIZE);
						mutUsage.EndUse();
					}
				}
				else if (me->fileCodePage == 1201)
				{
					while (me->readBuffSize > 0)
					{
						if (me->loadNewFile || me->threadToStop)
							break;

						lineCurr = 0;
						Sync::MutexUsage mutUsage(me->mut);
						while (lineCurr < me->readBuffSize)
						{
							c = (WChar)((me->readBuff[lineCurr] << 8) | me->readBuff[lineCurr + 1]);
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

						me->fs->SeekFromBeginning(me->readBuffOfst);
						me->readBuffSize = me->fs->Read(me->readBuff, READBUFFSIZE);
						mutUsage.EndUse();
					}
				}
				else
				{
					while (me->readBuffSize > 0)
					{
						if (me->loadNewFile || me->threadToStop)
							break;

						lineCurr = 0;
						Sync::MutexUsage mutUsage(me->mut);
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

						me->fs->SeekFromBeginning(me->readBuffOfst);
						me->readBuffSize = me->fs->Read(me->readBuff, READBUFFSIZE);
						mutUsage.EndUse();
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
			UOSInt srchTxtLen;
			UOSInt strLen;
			UInt32 startCaretX = me->caretX;
			UOSInt startCaretY = me->caretY;
			UInt32 srchCaretX;
			UOSInt srchCaretY;
			UInt64 startLineOfst = me->lineOfsts->GetItem(startCaretY);
			UInt64 nextLineOfst = me->lineOfsts->GetItem(startCaretY + 1);
			UInt8 *srchBuff;
			UInt64 currOfst;
			UOSInt currSize;
			UOSInt srchIndex;
			Bool found = false;

			if (me->fs)
			{
				Text::Encoding enc(me->fileCodePage);
				strLen = Text::StrCharCnt(me->srchText);
				srchTxtLen = enc.UTF8CountBytesC(me->srchText, strLen);
				srchTxt = MemAlloc(UInt8, srchTxtLen + 1);
				enc.UTF8ToBytesC(srchTxt, me->srchText, strLen);
				srchTxt[srchTxtLen] = 0;

				srchBuff = MemAlloc(UInt8, READBUFFSIZE + 1);
				currOfst = startLineOfst;
				while (true)
				{
					Sync::MutexUsage mutUsage(me->mut);
					if (me->fs)
					{
						me->fs->SeekFromBeginning(currOfst);
						currSize = me->fs->Read(srchBuff, READBUFFSIZE);
						srchBuff[currSize] = 0;
					}
					else
					{
						currSize = 0;
					}
					mutUsage.EndUse();
					if (currSize <= srchTxtLen)
					{
						break;
					}

					srchIndex = Text::StrIndexOfC(srchBuff, currSize, srchTxt, srchTxtLen);
					if (srchIndex != INVALID_INDEX)
					{
						me->GetPosFromByteOfst(currOfst + srchIndex, &srchCaretX, &srchCaretY);
						if (srchCaretY == startCaretY && srchCaretX <= startCaretX)
						{
							UOSInt tmpIndex;
							while (true)
							{
								tmpIndex = Text::StrIndexOfC(srchBuff + srchIndex + 1, currSize - srchIndex - 1, srchTxt, srchTxtLen);
								if (tmpIndex == INVALID_INDEX)
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
									me->selEndX = srchCaretX + (UInt32)strLen;
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
							me->selEndX = srchCaretX + (UInt32)strLen;
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
						Sync::MutexUsage mutUsage(me->mut);
						if (me->fs)
						{
							currSize = (UOSInt)(nextLineOfst - currOfst);
							if (currSize > READBUFFSIZE)
							{
								currSize = READBUFFSIZE;
							}
							me->fs->SeekFromBeginning(currOfst);
							currSize = me->fs->Read(srchBuff, currSize);
							srchBuff[currSize] = 0;
						}
						else
						{
							currSize = 0;
						}
						mutUsage.EndUse();
						if (currSize <= srchTxtLen)
						{
							break;
						}

						srchIndex = Text::StrIndexOfC(srchBuff, currSize, srchTxt, srchTxtLen);
						if (srchIndex != INVALID_INDEX)
						{
							me->GetPosFromByteOfst(currOfst + srchIndex, &srchCaretX, &srchCaretY);
							if (srchCaretY < startCaretY || srchCaretX < startCaretX)
							{
								me->selStartX = srchCaretX;
								me->selStartY = srchCaretY;
								me->selEndX = srchCaretX + (UInt32)strLen;
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

void UI::GUITextFileView::EnsureCaretVisible()
{
	Bool needRedraw = false;
	UOSInt yPos;
	UInt32 xPos;
	UOSInt xScr;
	yPos = (UOSInt)this->GetScrollVPos();
	xPos = this->dispLineNumW;
	xScr = (UOSInt)this->GetScrollHPos();

	if (caretY >= yPos + this->pageLineCnt)
	{
		yPos = caretY - this->pageLineCnt + 1;
		this->SetScrollVPos(yPos, true);
		needRedraw = true;
	}
	else if (caretY < yPos)
	{
		yPos = caretY;
		this->SetScrollVPos(yPos, true);
		needRedraw = true;
	}

	if (caretY >= this->lineOfsts->GetCount() - 1)
	{
	}
	else if (this->fs)
	{
		UInt8 *rbuff;
		UInt64 lineOfst;
		UInt64 nextOfst;
		Sync::MutexUsage mutUsage(this->mut);
		lineOfst = this->lineOfsts->GetItem(this->caretY);
		nextOfst = this->lineOfsts->GetItem(this->caretY + 1);
		if (nextOfst == 0 && this->lineOfsts->GetCount() - 1 <= this->caretY + 1)
		{
			nextOfst = this->lineOfsts->GetItem(this->lineOfsts->GetCount() - 1);
		}
		if (nextOfst > lineOfst)
		{
			WChar *line;
			UOSInt drawW;
			UOSInt drawH;

			Text::Encoding enc(this->fileCodePage);
			rbuff = MemAlloc(UInt8, (UOSInt)(nextOfst - lineOfst));
			this->fs->SeekFromBeginning(lineOfst);
			this->fs->Read(rbuff, (UOSInt)(nextOfst - lineOfst));
			UOSInt charCnt = enc.CountWChars(rbuff, (UOSInt)(nextOfst - lineOfst));
			if (charCnt < this->caretX)
			{
				this->caretX = (UInt32)charCnt;
			}
			line = MemAlloc(WChar, charCnt + 1);
			enc.WFromBytes(line, rbuff, (UOSInt)(nextOfst - lineOfst), 0);
			Text::StrReplace(line, '\t', ' ');
			this->GetDrawSize(line, this->caretX, &drawW, &drawH);
			xPos = this->dispLineNumW + (UInt32)drawW;
			MemFree(line);
			MemFree(rbuff);
		}
		mutUsage.EndUse();
	}
	if (xScr > xPos)
	{
		this->SetScrollHPos(xPos, true);
		needRedraw = true;
	}
	else
	{
		UOSInt scnW;
		UOSInt scnH;
		this->GetSizeP(&scnW, &scnH);
		if (xScr + scnW - 1 < xPos)
		{
			this->SetScrollHPos(xPos - (scnW - 1), true);
			needRedraw = true;
		}
	}

	if (needRedraw)
	{
		this->Redraw();
	}
}

void UI::GUITextFileView::UpdateCaretSel(Bool noRedraw)
{
	if (this->IsShiftPressed())
	{
		this->selEndX = this->caretX;
		this->selEndY = this->caretY;
		if (!noRedraw)
		{
			this->Redraw();
		}
	}
	else if (this->selStartX == this->selEndX && this->selStartY == this->selEndY)
	{
		this->selStartX = this->caretX;
		this->selStartY = this->caretY;
		this->selEndX = this->caretX;
		this->selEndY = this->caretY;
	}
	else
	{
		this->selStartX = this->caretX;
		this->selStartY = this->caretY;
		this->selEndX = this->caretX;
		this->selEndY = this->caretY;
		if (!noRedraw)
		{
			this->Redraw();
		}
	}
}

void UI::GUITextFileView::CopySelected()
{
	if (this->selStartX == this->selEndX && this->selStartY == this->selEndY)
	{
		return;
	}

	UOSInt selTopX;
	UOSInt selTopY;
	UOSInt selBottomX;
	UOSInt selBottomY;
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
	Sync::MutexUsage mutUsage(this->mut);
	if (this->fs == 0)
	{
		mutUsage.EndUse();
		return;
	}

	Text::Encoding *enc;
	UInt8 *rbuff;
	UTF8Char *line;
	UInt64 startOfst;
	UInt64 endOfst;
	UOSInt j;

	NEW_CLASS(enc, Text::Encoding(this->fileCodePage));
	if (selTopY == selBottomY)
	{
		startOfst = this->lineOfsts->GetItem(selTopY);
		endOfst = this->lineOfsts->GetItem(selTopY + 1);
		if (endOfst == 0)
		{
			mutUsage.EndUse();
			DEL_CLASS(enc);
			return;
//			endOfst = this->fs->GetLength();
		}
		rbuff = MemAlloc(UInt8, (UOSInt)(endOfst - startOfst));
		this->fs->SeekFromBeginning(startOfst);
		this->fs->Read(rbuff, (UOSInt)(endOfst - startOfst));
		mutUsage.EndUse();

		j = enc->CountUTF8Chars(rbuff, (UOSInt)(endOfst - startOfst));
		line = MemAlloc(UTF8Char, j + 1);
		enc->UTF8FromBytes(line, rbuff, (UOSInt)(endOfst - startOfst), 0);
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
			mutUsage.EndUse();
			DEL_CLASS(enc);
			return;
//			endOfst = this->fs->GetLength();
		}
		else if (endOfst - startOfst > 1048576)
		{
			mutUsage.EndUse();
			DEL_CLASS(enc);
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Failed to copy because selected area is too long", (const UTF8Char*)"TextViewer", this);
			return;
		}
		rbuff = MemAlloc(UInt8, (UOSInt)(endOfst - startOfst));
		this->fs->SeekFromBeginning(startOfst);
		this->fs->Read(rbuff, (UOSInt)(endOfst - startOfst));

		j = enc->CountUTF8Chars(rbuff, (UOSInt)(endOfst - startOfst));
		line = MemAlloc(UTF8Char, j + 1);
		enc->UTF8FromBytes(line, rbuff, (UOSInt)(endOfst - startOfst), 0);
		sb.Append(&line[selTopX]);
		MemFree(line);
		MemFree(rbuff);

		startOfst = this->lineOfsts->GetItem(selBottomY);
		endOfst = this->lineOfsts->GetItem(selBottomY + 1);
		if (endOfst == 0)
		{
			mutUsage.EndUse();
			return;
//			endOfst = this->fs->GetLength();
		}
		rbuff = MemAlloc(UInt8, (UOSInt)(endOfst - startOfst));
		this->fs->SeekFromBeginning(startOfst);
		this->fs->Read(rbuff, (UOSInt)(endOfst - startOfst));
		mutUsage.EndUse();
		j = enc->CountUTF8Chars(rbuff, (UOSInt)(endOfst - startOfst));
		line = MemAlloc(UTF8Char, j + 1);
		enc->UTF8FromBytes(line, rbuff, (UOSInt)(endOfst - startOfst), 0);
		line[selBottomX] = 0;
		sb.Append(line);
		MemFree(line);
		MemFree(rbuff);

		Win32::Clipboard::SetString(this->hwnd, sb.ToString());
	}
	DEL_CLASS(enc);
}

UOSInt UI::GUITextFileView::GetLineCharCnt(UOSInt lineNum)
{
	UInt64 lineOfst;
	UInt64 nextOfst;
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
		rbuff = MemAlloc(UInt8, (UOSInt)(nextOfst - lineOfst));
		this->fs->SeekFromBeginning(lineOfst);
		this->fs->Read(rbuff, (UOSInt)(nextOfst - lineOfst));
		UOSInt charCnt = enc.CountWChars(rbuff, (UOSInt)(nextOfst - lineOfst));
		WChar *line = MemAlloc(WChar, charCnt + 1);
		enc.WFromBytes(line, rbuff, (UOSInt)(nextOfst - lineOfst), 0);
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

void UI::GUITextFileView::GetPosFromByteOfst(UInt64 byteOfst, UInt32 *txtPosX, UOSInt *txtPosY)
{
	OSInt lineNum = this->lineOfsts->SortedIndexOf(byteOfst);
	if (lineNum >= 0)
	{
		*txtPosX = 0;
		*txtPosY = (UOSInt)lineNum;
		return;
	}
	lineNum = ~lineNum - 1;
	UInt64 thisOfst = this->lineOfsts->GetItem((UOSInt)lineNum);
	UOSInt buffSize = (UOSInt)(byteOfst - thisOfst);
	UInt8 *rbuff = MemAlloc(UInt8, buffSize);
	Text::Encoding enc(this->fileCodePage);
	Sync::MutexUsage mutUsage(this->mut);
	this->fs->SeekFromBeginning(thisOfst);
	this->fs->Read(rbuff, buffSize);
	mutUsage.EndUse();
	*txtPosX = (UInt32)enc.CountWChars(rbuff, buffSize);
	*txtPosY = (UOSInt)lineNum;
	MemFree(rbuff);
}

void UI::GUITextFileView::EventTextPosUpdated()
{
	UOSInt i = this->textPosUpdHdlr->GetCount();
	while (i-- > 0)
	{
		this->textPosUpdHdlr->GetItem(i)(this->textPosUpdObj->GetItem(i), this->caretX, this->caretY);
	}
}

UI::GUITextFileView::GUITextFileView(UI::GUICore *ui, UI::GUIClientControl *parent, Media::DrawEngine *deng) : UI::GUITextView(ui, parent, deng)
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
	this->lastLineCnt = INVALID_INDEX;
	this->dispLineNumW = 0;
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
	NEW_CLASS(lineOfsts, Data::ArrayListUInt64());
	NEW_CLASS(this->textPosUpdHdlr, Data::ArrayList<TextPosEvent>());
	NEW_CLASS(this->textPosUpdObj, Data::ArrayList<void *>());

	NEW_CLASS(this->evtThread, Sync::Event(true, (const UTF8Char*)"UI.GUITextFileView.evtThread"));
	NEW_CLASS(this->mut, Sync::Mutex());
	Sync::Thread::Create(ProcThread, this);
}

UI::GUITextFileView::~GUITextFileView()
{
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
	DEL_CLASS(this->evtThread);
	DEL_CLASS(this->mut);
	DEL_CLASS(this->lineOfsts);
	DEL_CLASS(this->textPosUpdHdlr);
	DEL_CLASS(this->textPosUpdObj);
	SDEL_TEXT(this->srchText);
}

void UI::GUITextFileView::EventLineUp()
{
	UInt32 textXPos;
	UOSInt textYPos;
	if (this->caretY > 0)
	{
		this->GetTextPos((Int32)this->caretDispX, (Int32)this->caretDispY - Double2Int32(this->pageLineHeight - 2), &textXPos, &textYPos);
		this->caretX = textXPos;
		this->caretY = textYPos;
		this->UpdateCaretSel(false);
		this->EnsureCaretVisible();
		this->UpdateCaretPos();
		this->EventTextPosUpdated();
	}
}

void UI::GUITextFileView::EventLineDown()
{
	UInt32 textXPos;
	UOSInt textYPos;
	this->GetTextPos(this->caretDispX, this->caretDispY + Double2Int32(this->pageLineHeight + 3), &textXPos, &textYPos);
	this->caretX = textXPos;
	this->caretY = textYPos;
	this->UpdateCaretSel(false);
	this->EnsureCaretVisible();
	this->UpdateCaretPos();
	this->EventTextPosUpdated();
}

void UI::GUITextFileView::EventPageUp()
{
	if (this->caretY < this->pageLineCnt - 1)
	{
		this->caretY = 0;
		this->caretX = 0;
	}
	else
	{
		this->caretY -= this->pageLineCnt - 1;
	}
	this->UpdateCaretSel(false);
	this->EnsureCaretVisible();
	this->UpdateCaretPos();
	this->EventTextPosUpdated();
}

void UI::GUITextFileView::EventPageDown()
{
	this->caretY += this->pageLineCnt - 1;
	if (this->caretY >= this->lineOfsts->GetCount() - 1)
	{
		this->caretY = this->lineOfsts->GetCount() - 1;
		this->caretX = 0;
	}
	this->UpdateCaretSel(false);
	this->EnsureCaretVisible();
	this->UpdateCaretPos();
	this->EventTextPosUpdated();
}

void UI::GUITextFileView::EventLeft()
{
	if (this->caretX > 0)
	{
		this->caretX -= 1;
		this->UpdateCaretSel(false);
		this->EnsureCaretVisible();
		this->UpdateCaretPos();
		this->EventTextPosUpdated();
	}
	else if (this->caretY > 0)
	{
		this->caretY -= 1;
		this->caretX = (UInt32)this->GetLineCharCnt(this->caretY);
		this->UpdateCaretSel(false);
		this->EnsureCaretVisible();
		this->UpdateCaretPos();
		this->EventTextPosUpdated();
	}
}

void UI::GUITextFileView::EventRight()
{
	if (this->caretX >= this->GetLineCharCnt(this->caretY))
	{
		this->caretX = 0;
		this->caretY++;
		if (this->caretY >= this->lineOfsts->GetCount() - 1)
			this->caretY = this->lineOfsts->GetCount() - 1;
		this->UpdateCaretSel(false);
		this->EnsureCaretVisible();
		this->UpdateCaretPos();
		this->EventTextPosUpdated();
	}
	else
	{
		this->caretX += 1;
		this->UpdateCaretSel(false);
		this->EnsureCaretVisible();
		this->UpdateCaretPos();
		this->EventTextPosUpdated();
	}
}

void UI::GUITextFileView::EventHome()
{
	this->caretX = 0;
	this->caretY = 0;
	this->UpdateCaretSel(true);
	this->SetScrollVPos(0, false);
	this->Redraw();
	this->EventTextPosUpdated();
}

void UI::GUITextFileView::EventEnd()
{
	this->caretX = 0;
	this->caretY = this->lineOfsts->GetCount() - 1;
	this->UpdateCaretSel(true);
	this->SetScrollVPos(this->lineOfsts->GetCount() - 1, false);
	this->Redraw();
	this->EventTextPosUpdated();
}

void UI::GUITextFileView::EventLineBegin()
{
	this->caretX = 0;
	this->UpdateCaretSel(false);
	this->EnsureCaretVisible();
	this->UpdateCaretPos();
	this->EventTextPosUpdated();
}

void UI::GUITextFileView::EventLineEnd()
{
	this->caretX = (UInt32)this->GetLineCharCnt(this->caretY);
	this->UpdateCaretSel(false);
	this->EnsureCaretVisible();
	this->UpdateCaretPos();
	this->EventTextPosUpdated();
}

void UI::GUITextFileView::EventCopy()
{
	this->CopySelected();
}

void UI::GUITextFileView::EventMouseDown(OSInt scnX, OSInt scnY, MouseButton btn)
{
	UInt32 textXPos;
	UOSInt textYPos;
	if (btn == MBTN_LEFT)
	{
		this->GetTextPos(scnX, scnY, &textXPos, &textYPos);
		this->caretX = textXPos;
		this->caretY = textYPos;
		if (!this->IsShiftPressed())
		{
			this->selStartX = textXPos;
			this->selStartY = textYPos;
		}
		this->selEndX = textXPos;
		this->selEndY = textYPos;
		this->selLastX = textXPos;
		this->selLastY = textYPos;
		this->mouseDown = true;
		this->Redraw();
		this->EventTextPosUpdated();
		this->UpdateCaretPos();
	}
}

void UI::GUITextFileView::EventMouseUp(OSInt scnX, OSInt scnY, MouseButton btn)
{
	if (btn == MBTN_LEFT)
	{
		this->mouseDown = false;
	}
}

void UI::GUITextFileView::EventMouseMove(OSInt scnX, OSInt scnY)
{
	UInt32 textXPos;
	UOSInt textYPos;
	if (this->mouseDown)
	{
		OSInt lineOfst;
		Bool needRedraw = false;
		lineOfst = Double2OSInt(OSInt2Double(scnX) / this->pageLineHeight);
		if (lineOfst < 0)
		{
			this->SetScrollVPos((UOSInt)(this->GetScrollVPos() + lineOfst), false);
			needRedraw = true;
		}
		else if (lineOfst > (OSInt)this->pageLineCnt)
		{
			this->SetScrollVPos((UOSInt)(this->GetScrollVPos() + lineOfst) - this->pageLineCnt, false);
			needRedraw = true;
		}
		this->GetTextPos(scnX, scnY, &textXPos, &textYPos);
		if (this->selLastX != textXPos || this->selLastY != textYPos)
		{
			this->selLastX = textXPos;
			this->selLastY = textYPos;
			this->selEndX = textXPos;
			this->selEndY = textYPos;
			this->caretX = textXPos;
			this->caretY = textYPos;
			needRedraw = true;
			this->EventTextPosUpdated();
			this->UpdateCaretPos();
		}
		if (needRedraw)
		{
			this->Redraw();
		}
	}
}

void UI::GUITextFileView::EventTimerTick()
{
	UOSInt currLineCnt = this->lineOfsts->GetCount();
	if (this->lastLineCnt != currLineCnt)
	{
		Bool needRedraw = false;
		if (this->lastLineCnt < this->pageLineCnt && currLineCnt > this->pageLineCnt)
		{
			needRedraw = true;
		}
		this->lastLineCnt = currLineCnt;
		if (currLineCnt == 0)
		{
			this->SetScrollVRange(0, 0);
		}
		else
		{
			this->SetScrollVRange(0, currLineCnt - 1);
		}
		needRedraw = true;
		if (needRedraw)
		{
			this->Redraw();
		}
	}
}

void UI::GUITextFileView::DrawImage(Media::DrawImage *dimg)
{
//	WChar sbuff[21];
	UTF8Char u8buff[21];
	const UTF8Char *csptr;
	const UTF8Char *csptr2;
	UOSInt xPos;
	UOSInt yPos;
	UInt64 startOfst;
	UInt64 endOfst;
	UInt64 currOfst;
	UInt64 nextOfst;
	UInt8 *rbuff;
	UOSInt i;
	UOSInt j;
	Text::Encoding *enc;
	WChar *line;
	WChar *sptr;
	WChar c;
	Double sz[2];

	UOSInt maxScnWidth;

	xPos = (UOSInt)this->GetScrollHPos();
	yPos = (UOSInt)this->GetScrollVPos();

	Media::DrawBrush *bgBrush = dimg->NewBrushARGB(this->bgColor);
	dimg->DrawRect(0, 0, UOSInt2Double(dimg->GetWidth()), UOSInt2Double(dimg->GetHeight()), 0, bgBrush);
	dimg->DelBrush(bgBrush);

	Sync::MutexUsage mutUsage(this->mut);
	if (this->fs == 0)
	{
		mutUsage.EndUse();
		return;
	}
	NEW_CLASS(enc, Text::Encoding(this->fileCodePage));
	startOfst = this->lineOfsts->GetItem(yPos);
	endOfst = this->lineOfsts->GetItem(yPos + this->pageLineCnt);
	if (endOfst == 0)
	{
		endOfst = this->lineOfsts->GetItem(this->lineOfsts->GetCount() - 1);
	}
	rbuff = MemAlloc(UInt8, (UOSInt)(endOfst - startOfst));
	this->fs->SeekFromBeginning(startOfst);
	this->fs->Read(rbuff, (UOSInt)(endOfst - startOfst));

	maxScnWidth = dimg->GetWidth() + xPos;
	Media::DrawFont *fnt = this->CreateDrawFont(dimg);
	Text::StrUOSInt(u8buff, this->pageLineCnt + yPos);
	dimg->GetTextSize(fnt, u8buff, sz);
	this->dispLineNumW = (UInt32)Double2Int32(sz[0]) + 8;

	UInt32 selTopX;
	UOSInt selTopY;
	UInt32 selBottomX;
	UOSInt selBottomY;
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

	Media::DrawBrush *textBrush = dimg->NewBrushARGB(this->txtColor);
	Media::DrawBrush *lineNumBrush = dimg->NewBrushARGB(this->lineNumColor);
	Media::DrawBrush *selBrush = dimg->NewBrushARGB(this->selColor);
	Media::DrawBrush *selTextBrush = dimg->NewBrushARGB(this->selTextColor);
	currOfst = startOfst;
	i = 0;
	while (i < this->pageLineCnt)
	{
		nextOfst = this->lineOfsts->GetItem(i + yPos + 1);
		if (nextOfst == 0 && i + yPos + 1 >= this->lineOfsts->GetCount() - 1)
		{
			nextOfst = endOfst;
		}
		if (nextOfst > currOfst)
		{
			j = enc->CountWChars(&rbuff[currOfst - startOfst], (UOSInt)(nextOfst - currOfst));
			line = MemAlloc(WChar, j + 1);
			sptr = enc->WFromBytes(line, &rbuff[currOfst - startOfst], (UOSInt)(nextOfst - currOfst), 0);
			Text::StrReplace(line, '\t', ' ');
			if (sptr)
			{
				if (sptr[-1] == 13)
				{
					if (sptr[-2] == 10)
					{
						sptr[-2] = 0;
						sptr -= 2;
					}
					else
					{
						sptr[-1] = 0;
						sptr -= 1;
					}
				}
				else if (sptr[-1] == 10)
				{
					if (sptr[-2] == 13)
					{
						sptr[-2] = 0;
						sptr -= 2;
					}
					else
					{
						sptr[-1] = 0;
						sptr -= 1;
					}
				}

				Double szWhole[2];
				Double szThis[2];
				csptr = Text::StrToUTF8New(line);
				dimg->GetTextSize(fnt, csptr, szWhole);
				if (maxScnWidth < (UOSInt)Double2OSInt(szWhole[0] + sz[0] + 8))
				{
					maxScnWidth = (UOSInt)Double2OSInt(szWhole[0] + sz[0] + 8);
				}

				Double drawTop = UOSInt2Double(i) * sz[1];
				Double drawLeft = sz[0] + 8 - UOSInt2Double(xPos);
				Text::StrUOSInt(u8buff, i + yPos + 1);
				dimg->DrawString(-UOSInt2Double(xPos), drawTop, u8buff, fnt, lineNumBrush);

				if (i + yPos > selTopY && i + yPos < selBottomY)
				{
					dimg->GetTextSize(fnt, csptr, szThis);
					dimg->DrawRect(drawLeft, drawTop, szThis[0], szThis[1], 0, selBrush);
					dimg->DrawString(drawLeft, drawTop, csptr, fnt, selTextBrush);
				}
				else if (i + yPos == selTopY && selTopY == selBottomY)
				{
					if ((UOSInt)(sptr - line) <= selTopX || selTopX == selBottomX)
					{
						dimg->DrawString(drawLeft, drawTop, csptr, fnt, textBrush);
					}
					else if ((UOSInt)(sptr - line) <= selBottomX)
					{
						if (selTopX > 0)
						{
							c = line[selTopX];
							line[selTopX] = 0;
							csptr2 = Text::StrToUTF8New(line);
							line[selTopX] = c;
							dimg->GetTextSize(fnt, csptr2, szThis);
							dimg->DrawString(drawLeft, drawTop, csptr2, fnt, textBrush);
							drawLeft += szThis[0];
							Text::StrDelNew(csptr2);
						}
						csptr2 = Text::StrToUTF8New(&line[selTopX]);
						dimg->GetTextSize(fnt, csptr2, szThis);
						dimg->DrawRect(drawLeft, drawTop, szThis[0], szThis[1], 0, selBrush);
						dimg->DrawString(drawLeft, drawTop, csptr2, fnt, selTextBrush);
						Text::StrDelNew(csptr2);
					}
					else
					{
						if (selTopX > 0)
						{
							c = line[selTopX];
							line[selTopX] = 0;
							csptr2 = Text::StrToUTF8New(line);
							line[selTopX] = c;
							dimg->GetTextSize(fnt, csptr2, szThis);
							dimg->DrawString(drawLeft, drawTop, csptr2, fnt, textBrush);
							drawLeft += szThis[0];
							Text::StrDelNew(csptr2);
						}
						c = line[selBottomX];
						line[selBottomX] = 0;
						csptr2 = Text::StrToUTF8New(&line[selTopX]);
						line[selBottomX] = c;
						dimg->GetTextSize(fnt, csptr2, szThis);
						dimg->DrawRect(drawLeft, drawTop, szThis[0], szThis[1], 0, selBrush);
						dimg->DrawString(drawLeft, drawTop, csptr2, fnt, selTextBrush);
						Text::StrDelNew(csptr2);
						drawLeft += szThis[0];

						csptr2 = Text::StrToUTF8New(&line[selBottomX]);
						dimg->DrawString(drawLeft, drawTop, csptr2, fnt, textBrush);
						Text::StrDelNew(csptr2);
					}
				}
				else if (i + yPos == selTopY)
				{
					if (selTopX == 0)
					{
						dimg->GetTextSize(fnt, csptr, szThis);
						dimg->DrawRect(drawLeft, drawTop, szThis[0], szThis[1], 0, selBrush);
						dimg->DrawString(drawLeft, drawTop, csptr, fnt, selTextBrush);
					}
					else if ((UOSInt)(sptr - line) <= selTopX)
					{
						dimg->DrawString(drawLeft, drawTop, csptr, fnt, textBrush);
					}
					else
					{
						c = line[selTopX];
						line[selTopX] = 0;
						csptr2 = Text::StrToUTF8New(line);
						line[selTopX] = c;
						dimg->GetTextSize(fnt, csptr2, szThis);
						dimg->DrawString(drawLeft, drawTop, csptr2, fnt, textBrush);
						drawLeft += szThis[0];
						Text::StrDelNew(csptr2);
						csptr2 = Text::StrToUTF8New(&line[selTopX]);
						dimg->GetTextSize(fnt, csptr2, szThis);
						dimg->DrawRect(drawLeft, drawTop, szThis[0], szThis[1], 0, selBrush);
						dimg->DrawString(drawLeft, drawTop, csptr2, fnt, selTextBrush);
						Text::StrDelNew(csptr2);
					}
				}
				else if (i + yPos == selBottomY)
				{
					if (selBottomX == 0)
					{
						dimg->DrawString(drawLeft, drawTop, csptr, fnt, textBrush);
					}
					else if ((UOSInt)(sptr - line) <= selBottomX)
					{
						dimg->GetTextSize(fnt, csptr, szThis);
						dimg->DrawRect(drawLeft, drawTop, szThis[0], szThis[1], 0, selBrush);
						dimg->DrawString(drawLeft, drawTop, csptr, fnt, selTextBrush);
					}
					else
					{
						c = line[selBottomX];
						line[selBottomX] = 0;
						csptr2 = Text::StrToUTF8New(line);
						line[selBottomX] = c;
						dimg->GetTextSize(fnt, csptr2, szThis);
						dimg->DrawRect(drawLeft, drawTop, szThis[0], szThis[1], 0, selBrush);
						dimg->DrawString(drawLeft, drawTop, csptr2, fnt, selTextBrush);
						Text::StrDelNew(csptr2);
						drawLeft += szThis[0];

						csptr2 = Text::StrToUTF8New(&line[selBottomX]);
						dimg->DrawString(drawLeft, drawTop, csptr2, fnt, textBrush);
						Text::StrDelNew(csptr2);
					}
				}
				else
				{
					dimg->DrawString(drawLeft, drawTop, csptr, fnt, textBrush);
				}
				Text::StrDelNew(csptr);
			}
			MemFree(line);
		}

		currOfst = nextOfst;
		i++;
	}
	dimg->DelBrush(textBrush);
	dimg->DelBrush(lineNumBrush);
	dimg->DelBrush(selBrush);
	dimg->DelBrush(selTextBrush);
	dimg->DelFont(fnt);

	DEL_CLASS(enc);
	MemFree(rbuff);
	mutUsage.EndUse();
	this->SetScrollHRange(0, maxScnWidth);
}

void UI::GUITextFileView::UpdateCaretPos()
{
	UOSInt yPos;
	UInt32 xPos;
	UOSInt xScr;
	yPos = (UOSInt)this->GetScrollVPos();
	xPos = this->dispLineNumW;
	xScr = (UOSInt)this->GetScrollHPos();

	if (caretY < 0 || caretY >= this->lineOfsts->GetCount() - 1)
	{
	}
	else if (this->fs)
	{
		UInt8 *rbuff;
		UInt64 lineOfst;
		UInt64 nextOfst;
		Sync::MutexUsage mutUsage(this->mut);
		lineOfst = this->lineOfsts->GetItem(this->caretY);
		nextOfst = this->lineOfsts->GetItem(this->caretY + 1);
		if (nextOfst == 0 && this->lineOfsts->GetCount() - 1 <= this->caretY + 1)
		{
			nextOfst = this->lineOfsts->GetItem(this->lineOfsts->GetCount() - 1);
		}
		if (nextOfst > lineOfst)
		{
			WChar *line;
			UOSInt drawW;
			UOSInt drawH;

			Text::Encoding enc(this->fileCodePage);
			rbuff = MemAlloc(UInt8, (UOSInt)(nextOfst - lineOfst));
			this->fs->SeekFromBeginning(lineOfst);
			this->fs->Read(rbuff, (UOSInt)(nextOfst - lineOfst));
			UOSInt charCnt = enc.CountWChars(rbuff, (UOSInt)(nextOfst - lineOfst));
			if (charCnt < this->caretX)
			{
				this->caretX = (UInt32)charCnt;
			}
			line = MemAlloc(WChar, charCnt + 1);
			enc.WFromBytes(line, rbuff, (UOSInt)(nextOfst - lineOfst), 0);
			Text::StrReplace(line, '\t', ' ');
			this->GetDrawSize(line, this->caretX, &drawW, &drawH);
			xPos = this->dispLineNumW + (UInt32)drawW;
			MemFree(line);
			MemFree(rbuff);
		}
		mutUsage.EndUse();
	}

	this->caretDispX = (Int32)(xPos - xScr);
	this->caretDispY = (Int32)(UOSInt2Double(this->caretY - yPos) * this->pageLineHeight);
	this->SetCaretPos(this->caretDispX, this->caretDispY);
}

Bool UI::GUITextFileView::IsLoading()
{
	return this->readingFile;
}

UOSInt UI::GUITextFileView::GetLineCount()
{
	return this->lineOfsts->GetCount() - 1;
}

void UI::GUITextFileView::SetCodePage(UInt32 codePage)
{
	Sync::MutexUsage mutUsage(this->mut);
	this->codePage = codePage;
}

Bool UI::GUITextFileView::LoadFile(const UTF8Char *fileName)
{
	while (this->isSearching)
	{
		Sync::Thread::Sleep(10);
	}

	Sync::MutexUsage mutUsage(this->mut);
	if (this->fileName)
	{
		Text::StrDelNew(this->fileName);
	}
	this->fileName = Text::StrCopyNew(fileName);
	this->loadNewFile = true;
	this->fileSize = 0;

	this->lastLineCnt = 0;
	this->SetScrollVRange(0, 0);

	this->caretX = 0;
	this->caretY = 0;
	this->selStartX = 0;
	this->selStartY = 0;
	this->selEndX = 0;
	this->selEndY = 0;
	this->selLastX = 0;
	this->selLastY = 0;
	this->evtThread->Set();
	mutUsage.EndUse();
	this->EventTextPosUpdated();
	return true;
}

const UTF8Char *UI::GUITextFileView::GetFileName()
{
	return this->fileName;
}

void UI::GUITextFileView::GetTextPos(OSInt scnPosX, OSInt scnPosY, UInt32 *textPosX, UOSInt *textPosY)
{
	OSInt textY = (OSInt)(OSInt2Double(this->GetScrollVPos()) + OSInt2Double(scnPosY) / this->pageLineHeight);
	Int32 drawX;
	UInt32 textX = 0;
	if (textY >= (OSInt)this->lineOfsts->GetCount() - 1)
	{
		textY = (OSInt)this->lineOfsts->GetCount() - 1;
		textX = 0;
		*textPosX = textX;
		*textPosY = (UOSInt)textY;
		return;
	}
	drawX = (Int32)(scnPosX + this->GetScrollHPos() - this->dispLineNumW);
	if (drawX < 0)
	{
		textX = 0;
		*textPosX = textX;
		*textPosY = (UOSInt)textY;
		return;
	}
	if (this->fs)
	{
		UInt8 *rbuff;
		UInt64 lineOfst;
		UInt64 nextOfst;
		Sync::MutexUsage mutUsage(this->mut);
		lineOfst = this->lineOfsts->GetItem((UOSInt)textY);
		nextOfst = this->lineOfsts->GetItem((UOSInt)textY + 1);
		if (nextOfst == 0 && (OSInt)this->lineOfsts->GetCount() - 1 <= textY + 1)
		{
			nextOfst = this->lineOfsts->GetItem(this->lineOfsts->GetCount() - 1);
		}
		if (nextOfst > lineOfst)
		{
			WChar *line;

			Text::Encoding enc(this->fileCodePage);
			rbuff = MemAlloc(UInt8, (UOSInt)(nextOfst - lineOfst));
			this->fs->SeekFromBeginning(lineOfst);
			this->fs->Read(rbuff, (UOSInt)(nextOfst - lineOfst));
			UOSInt charCnt = enc.CountWChars(rbuff, (UOSInt)(nextOfst - lineOfst));
			line = MemAlloc(WChar, charCnt + 1);
			enc.WFromBytes(line, rbuff, (UOSInt)(nextOfst - lineOfst), 0);
			Text::StrReplace(line, '\t', ' ');
			charCnt -= 1;
			if (charCnt > 0 && (line[charCnt] == 0xd || line[charCnt] == 0xa))
			{
				charCnt--;
			}
			textX = this->GetCharCntAtWidth(line, charCnt, (UOSInt)drawX);
			MemFree(line);
			MemFree(rbuff);
		}
		mutUsage.EndUse();
	}
	*textPosX = textX;
	*textPosY = (UOSInt)textY;
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
	if (newPosY < 0)
	{
		newPosY = 0;
	}
	if (newPosX < 0)
	{
		newPosX = 0;
	}
	else if (newPosX > (lineCharCnt = this->GetLineCharCnt(newPosY)))
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
