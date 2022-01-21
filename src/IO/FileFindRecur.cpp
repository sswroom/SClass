#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileFindRecur.h"

IO::FileFindRecur::FileFindRecur(const UTF8Char *path)
{
	UOSInt strLen = Text::StrCharCnt(path);
	this->srcBuff = MemAlloc(UTF8Char, strLen + 1);
	Text::StrConcat(this->srcBuff, path);
	this->partCnt = Text::StrCountChar(this->srcBuff, IO::Path::PATH_SEPERATOR) + 1;
	this->srcStrs = MemAlloc(UTF8Char *, this->partCnt);
	this->srchParts = MemAlloc(FindRecurPart, this->partCnt);
	UOSInt i;
	i = 0;
	while (i < this->partCnt)
	{
		this->srchParts[i].sess = 0;
		this->srchParts[i].buffPtr = 0;
		i++;
	}
	Text::StrSplit(this->srcStrs, this->partCnt, this->srcBuff, IO::Path::PATH_SEPERATOR);
	this->isFirst = true;
}

IO::FileFindRecur::~FileFindRecur()
{
	UOSInt i = this->partCnt;
	while (i-- > 0)
	{
		if (this->srchParts[i].sess)
		{
			IO::Path::FindFileClose(this->srchParts[i].sess);
			this->srchParts[i].sess = 0;
		}
	}
	MemFree(this->srcBuff);
	MemFree(this->srchParts);
	MemFree(this->srcStrs);
}

const UTF8Char *IO::FileFindRecur::NextFile(IO::Path::PathType *pt)
{
	UOSInt i;
	IO::Path::PathType thisPt;
	UTF8Char *sptr;

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
					return 0;
				}
				i--;
				if (IO::Path::IsSearchPattern(this->srcStrs[i]))
				{
					sptr = IO::Path::FindNextFile(this->srchParts[i].buffPtr, this->srchParts[i].sess, 0, &thisPt, 0);
					if (sptr)
					{
						i++;
						break;
					}
					else
					{
						IO::Path::FindFileClose(this->srchParts[i].sess);
						this->srchParts[i].sess = 0;
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
			sptr = Text::StrConcat(sptr, this->srcStrs[i]);
			if (IO::Path::IsSearchPattern(this->srcStrs[i]))
			{
				this->srchParts[i].sess = IO::Path::FindFile(this->currBuff);
				if (this->srchParts[i].sess)
				{
					sptr = IO::Path::FindNextFile(this->srchParts[i].buffPtr, this->srchParts[i].sess, 0, &thisPt, 0);
					if (sptr)
					{
					}
					else
					{
						IO::Path::FindFileClose(this->srchParts[i].sess);
						this->srchParts[i].sess = 0;
						while (true)
						{
							if (i == 0)
							{
								return 0;
							}
							i--;
							if (IO::Path::IsSearchPattern(this->srcStrs[i]))
							{
								sptr = IO::Path::FindNextFile(this->srchParts[i].buffPtr, this->srchParts[i].sess, 0, &thisPt, 0);
								if (sptr)
								{
									break;
								}
								else
								{
									IO::Path::FindFileClose(this->srchParts[i].sess);
									this->srchParts[i].sess = 0;
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
							return 0;
						}
						i--;
						if (IO::Path::IsSearchPattern(this->srcStrs[i]))
						{
							sptr = IO::Path::FindNextFile(this->srchParts[i].buffPtr, this->srchParts[i].sess, 0, &thisPt, 0);
							if (sptr)
							{
								break;
							}
							else
							{
								IO::Path::FindFileClose(this->srchParts[i].sess);
								this->srchParts[i].sess = 0;
							}
						}
					}
				}
			}
			i++;
		}
		thisPt = IO::Path::GetPathType(this->currBuff, Text::StrCharCnt(this->currBuff));
		if (thisPt != IO::Path::PathType::Unknown)
		{
			if (pt)
			{
				*pt = thisPt;
			}
			return this->currBuff;
		}
	}
}
