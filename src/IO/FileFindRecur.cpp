#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileFindRecur.h"

IO::FileFindRecur::FileFindRecur(Text::CStringNN path)
{
	this->srcBuff = MemAllocArr(UTF8Char, path.leng + 1);
	path.ConcatTo(this->srcBuff);
	this->partCnt = Text::StrCountChar(this->srcBuff, IO::Path::PATH_SEPERATOR) + 1;
	this->srcStrs = MemAllocArr(Text::PString, this->partCnt);
	this->srchParts = MemAllocArr(FindRecurPart, this->partCnt);
	UIntOS i;
	i = 0;
	while (i < this->partCnt)
	{
		this->srchParts[i].sess = nullptr;
		this->srchParts[i].buffPtr = nullptr;
		i++;
	}
	Text::StrSplitP(this->srcStrs, this->partCnt, Text::PString(this->srcBuff, path.leng), IO::Path::PATH_SEPERATOR);
	this->isFirst = true;
}

IO::FileFindRecur::~FileFindRecur()
{
	NN<IO::Path::FindFileSession> sess;
	UIntOS i = this->partCnt;
	while (i-- > 0)
	{
		if (this->srchParts[i].sess.SetTo(sess))
		{
			IO::Path::FindFileClose(sess);
			this->srchParts[i].sess = nullptr;
		}
	}
	MemFreeArr(this->srcBuff);
	MemFreeArr(this->srchParts);
	MemFreeArr(this->srcStrs);
}

Text::CString IO::FileFindRecur::NextFile(IO::Path::PathType *pt)
{
	UIntOS i;
	IO::Path::PathType thisPt;
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> buffPtr;
	NN<IO::Path::FindFileSession> sess;

	while (true)
	{
		if (this->isFirst)
		{
			this->isFirst = false;
			sptr = this->currBuff;
			sptr[0] = 0;
			i = 0;
		}
		else
		{
			i = this->partCnt;
			while (true)
			{
				if (i == 0)
				{
					return nullptr;
				}
				i--;
				if (this->srchParts[i].sess.SetTo(sess) && IO::Path::IsSearchPattern(this->srcStrs[i].v) && this->srchParts[i].buffPtr.SetTo(buffPtr))
				{
					if (IO::Path::FindNextFile(buffPtr, sess, 0, thisPt, 0).SetTo(sptr))
					{
						i++;
						break;
					}
					else
					{
						IO::Path::FindFileClose(sess);
						this->srchParts[i].sess = nullptr;
					}
				}
			}
		}

		while (i < this->partCnt)
		{
			if (i > 0)
			{
				*sptr++ = IO::Path::PATH_SEPERATOR;
			}
			this->srchParts[i].buffPtr = sptr;
			sptr = this->srcStrs[i].ConcatTo(sptr);
			if (IO::Path::IsSearchPattern(this->srcStrs[i].v))
			{
				this->srchParts[i].sess = IO::Path::FindFile(CSTRP(this->currBuff, sptr));
				if (this->srchParts[i].sess.SetTo(sess) && this->srchParts[i].buffPtr.SetTo(buffPtr))
				{
					if (IO::Path::FindNextFile(buffPtr, sess, 0, thisPt, 0).SetTo(sptr))
					{
					}
					else
					{
						IO::Path::FindFileClose(sess);
						this->srchParts[i].sess = nullptr;
						while (true)
						{
							if (i == 0)
							{
								return nullptr;
							}
							i--;
							if (this->srchParts[i].sess.SetTo(sess) && IO::Path::IsSearchPattern(this->srcStrs[i].v) && this->srchParts[i].buffPtr.SetTo(buffPtr))
							{
								if (IO::Path::FindNextFile(buffPtr, sess, 0, thisPt, 0).SetTo(sptr))
								{
									break;
								}
								else
								{
									IO::Path::FindFileClose(sess);
									this->srchParts[i].sess = nullptr;
								}
							}
						}
					}
				}
				else
				{
					while (true)
					{
						if (i == 0)
						{
							return nullptr;
						}
						i--;
						if (this->srchParts[i].sess.SetTo(sess) && IO::Path::IsSearchPattern(this->srcStrs[i].v) && this->srchParts[i].buffPtr.SetTo(buffPtr))
						{
							if (IO::Path::FindNextFile(buffPtr, sess, 0, thisPt, 0).SetTo(sptr))
							{
								break;
							}
							else
							{
								IO::Path::FindFileClose(sess);
								this->srchParts[i].sess = nullptr;
							}
						}
					}
				}
			}
			i++;
		}
		UIntOS buffLen = Text::StrCharCnt(this->currBuff);
		thisPt = IO::Path::GetPathType({this->currBuff, buffLen});
		if (thisPt != IO::Path::PathType::Unknown)
		{
			if (pt)
			{
				*pt = thisPt;
			}
			return {this->currBuff, buffLen};
		}
	}
}
