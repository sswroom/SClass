#include "Stdafx.h"
#include "Data/ByteBuffer.h"
#include "IO/FileAnalyse/TXTFileAnalyse.h"
#include "Sync/MutexUsage.h"

void __stdcall IO::FileAnalyse::TXTFileAnalyse::ParseThread(NotNullPtr<Sync::Thread> thread)
{
	IO::FileAnalyse::TXTFileAnalyse *me = (IO::FileAnalyse::TXTFileAnalyse *)thread->GetUserObj();
	UInt64 buffOfst = 0;
	UOSInt buffSize = 0;
	UOSInt readSize;
	UOSInt i;
	UOSInt lineStart;
	Data::ByteBuffer buff(65536);
	{
		Sync::MutexUsage mutUsage(me->mut);
		me->lineOfsts.Add(0);
	}
	buffSize = me->fd->GetRealData(0, buff.GetSize(), buff);
	i = 0;
	lineStart = 0;
	if (buffSize >= 3)
	{
		if (buff[0] == 0xef && buff[1] == 0xbb && buff[2] == 0xbf)
		{
			i = 3;
			lineStart = 3;
			Sync::MutexUsage mutUsage(me->mut);
			me->lineOfsts.Add(3);
		}
	}
	while (true)
	{
		if (i >= buffSize)
		{
			if (lineStart == 0 && buffSize > 0)
			{
				lineStart = buffSize - 1;
			}
			buffOfst += lineStart;
			buff.CopyInner(0, lineStart, buffSize - lineStart);
			buffSize -= lineStart;
			lineStart = 0;
			readSize = me->fd->GetRealData(buffOfst + buffSize, buff.GetSize() - buffSize, buff.SubArray(buffSize));
			if (readSize == 0)
				break;
			if (buffSize > 0)
				i = buffSize - 1;
			else
				i = 0;
			buffSize += readSize;
		}
		if (buff[i] == 13)
		{
			i++;
			if (i < buffSize)
			{
				if (buff[i] == 10)
				{
					i++;
					lineStart = i;
				}
				else
				{
					lineStart = i;
				}
				Sync::MutexUsage mutUsage(me->mut);
				me->lineOfsts.Add(lineStart + buffOfst);
			}
		}
		else if (buff[i] == 10)
		{
			i++;
			lineStart = i;
			Sync::MutexUsage mutUsage(me->mut);
			me->lineOfsts.Add(lineStart + buffOfst);
		}
		else
		{
			i++;
		}
	}
	if (me->lineOfsts.GetItem(me->lineOfsts.GetCount() - 1) == me->fileSize)
	{
		me->lineOfsts.RemoveAt(me->lineOfsts.GetCount() - 1);
	}
}

IO::FileAnalyse::TXTFileAnalyse::TXTFileAnalyse(NotNullPtr<IO::StreamData> fd) : thread(ParseThread, this, CSTR("TXTFileAnalyse"))
{
	this->fd = 0;
	this->pauseParsing = false;
	this->fileSize = fd->GetDataSize();
	this->fd = fd->GetPartialData(0, this->fileSize).Ptr();
	if (this->fileSize > 0)
		this->thread.Start();
}

IO::FileAnalyse::TXTFileAnalyse::~TXTFileAnalyse()
{
	this->thread.Stop();
	SDEL_CLASS(this->fd);
}

Text::CStringNN IO::FileAnalyse::TXTFileAnalyse::GetFormatName()
{
	return CSTR("Text");
}

UOSInt IO::FileAnalyse::TXTFileAnalyse::GetFrameCount()
{
	return this->lineOfsts.GetCount();
}

Bool IO::FileAnalyse::TXTFileAnalyse::GetFrameName(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	Sync::MutexUsage mutUsage(this->mut);
	UOSInt cnt = this->lineOfsts.GetCount();
	if (index >= cnt)
		return false;
	UInt64 lineOfst = this->lineOfsts.GetItem(index);
	UOSInt size;
	if (index == cnt - 1)
	{
		size = (UOSInt)(this->fileSize - lineOfst);
	}
	else
	{
		size = (UOSInt)(this->lineOfsts.GetItem(index + 1) - lineOfst);
	}
	sb->AppendU64(lineOfst);
	sb->AppendC(UTF8STRC(": length="));
	sb->AppendUOSInt(size);
	return true;
}

UOSInt IO::FileAnalyse::TXTFileAnalyse::GetFrameIndex(UInt64 ofst)
{
	if (ofst >= this->fileSize)
		return INVALID_INDEX;
	Sync::MutexUsage mutUsage(this->mut);
	OSInt i = this->lineOfsts.SortedIndexOf(ofst);
	if (i < 0)
		return (UOSInt)~i - 1;
	else
		return (UOSInt)i;
}

Optional<IO::FileAnalyse::FrameDetail> IO::FileAnalyse::TXTFileAnalyse::GetFrameDetail(UOSInt index)
{
	Sync::MutexUsage mutUsage(this->mut);
	UOSInt cnt = this->lineOfsts.GetCount();
	if (index >= cnt)
		return 0;
	UInt64 lineOfst = this->lineOfsts.GetItem(index);
	UOSInt size;
	if (index == cnt - 1)
	{
		size = (UOSInt)(this->fileSize - lineOfst);
	}
	else
	{
		size = (UOSInt)(this->lineOfsts.GetItem(index + 1) - lineOfst);
	}
	mutUsage.EndUse();
	NotNullPtr<IO::FileAnalyse::FrameDetail> frame;
	NEW_CLASSNN(frame, IO::FileAnalyse::FrameDetail(lineOfst, size));
	UInt8 buff[4096];
	if (size <= sizeof(buff))
	{
		if (this->fd->GetRealData(lineOfst, size, BYTEARR(buff)) == size)
		{
			UOSInt txtSize;
			if (size == 3 && buff[0] == 0xef && buff[1] == 0xbb && buff[2] == 0xbf)
			{
				frame->AddField(0, size, CSTR("UTF-8 BOM"), CSTR("EF BB BF"));
			}
			else if (size > 0)
			{
				if (buff[size - 1] == 13)
				{
					txtSize = size - 1;
					frame->AddStrC(0, txtSize, CSTR("Text Line"), buff);
					frame->AddField(txtSize, 1, CSTR("Line Break"), CSTR("CR"));
				}
				else if (buff[size - 1] == 10)
				{
					if (size > 1 && buff[size - 2] == 13)
					{
						txtSize = size - 2;
						frame->AddStrC(0, txtSize, CSTR("Text Line"), buff);
						frame->AddField(txtSize, 2, CSTR("Line Break"), CSTR("CRLF"));
					}
					else
					{
						txtSize = size - 1;
						frame->AddStrC(0, txtSize, CSTR("Text Line"), buff);
						frame->AddField(txtSize, 1, CSTR("Line Break"), CSTR("LF"));
					}
				}
				else
				{
					txtSize = size;
					frame->AddField(0, size, CSTR("Text Line"), Text::CStringNN(buff, txtSize));
				}
			}
		}
	}
	else
	{
		Data::ByteBuffer buff2(size);
		if (this->fd->GetRealData(lineOfst, size, buff2) == size)
		{
			UOSInt txtSize;
			if (size == 3 && buff2[0] == 0xef && buff2[1] == 0xbb && buff2[2] == 0xbf)
			{
				frame->AddField(0, size, CSTR("UTF-8 BOM"), CSTR("EF BB BF"));
			}
			else if (size > 0)
			{
				if (buff2[size - 1] == 13)
				{
					txtSize = size - 1;
					frame->AddStrC(0, txtSize, CSTR("Text Line"), buff2.Ptr());
					frame->AddField(txtSize, 1, CSTR("Line Break"), CSTR("CR"));
				}
				else if (buff2[size - 1] == 10)
				{
					if (size > 1 && buff2[size - 2] == 13)
					{
						txtSize = size - 2;
						frame->AddStrC(0, txtSize, CSTR("Text Line"), buff2.Ptr());
						frame->AddField(txtSize, 2, CSTR("Line Break"), CSTR("CRLF"));
					}
					else
					{
						txtSize = size - 1;
						frame->AddStrC(0, txtSize, CSTR("Text Line"), buff2.Ptr());
						frame->AddField(txtSize, 1, CSTR("Line Break"), CSTR("LF"));
					}
				}
				else
				{
					txtSize = size;
					frame->AddField(0, size, CSTR("Text Line"), Text::CStringNN(buff2.Ptr(), txtSize));
				}
			}
		}
	}
	return frame;
}

Bool IO::FileAnalyse::TXTFileAnalyse::IsError()
{
	return this->fd == 0;
}

Bool IO::FileAnalyse::TXTFileAnalyse::IsParsing()
{
	return this->thread.IsRunning();
}

Bool IO::FileAnalyse::TXTFileAnalyse::TrimPadding(Text::CStringNN outputFile)
{
	return false;
}
