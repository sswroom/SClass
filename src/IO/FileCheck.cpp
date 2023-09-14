#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/HashCreator.h"
#include "IO/ActiveStreamReader.h"
#include "IO/FileCheck.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Text/MyString.h"

typedef struct
{
	Crypto::Hash::IHash *hash;
	UInt64 readSize;
	UInt64 fileSize;
	IO::ProgressHandler *progress;
} ReadSess;

IO::FileCheck *IO::FileCheck::CreateCheck(Text::CStringNN path, Crypto::Hash::HashType chkType, IO::ProgressHandler *progress, Bool skipError)
{
	UTF8Char sbuff[1024];
	UInt8 hashBuff[32];
	Crypto::Hash::IHash *hash;
	IO::FileCheck *fchk;
	IO::Path::PathType pt;
	UInt64 fileSize;
	ReadSess readSess;
	NotNullPtr<IO::FileStream> fs;
	IO::ActiveStreamReader *reader;
	IO::ActiveStreamReader::BottleNeckType bnt;

	hash = Crypto::Hash::HashCreator::CreateHash(chkType);
	if (hash == 0)
	{
		return 0;
	}

	pt = IO::Path::GetPathType(path);
	if (pt == IO::Path::PathType::File)
	{
		NEW_CLASS(fchk, IO::FileCheck(path, chkType));

		NEW_CLASSNN(fs, IO::FileStream(path, IO::FileMode::ReadOnly, IO::FileShare::DenyWrite, IO::FileStream::BufferType::NoBuffer));
		if (fs->IsError())
		{
			DEL_CLASS(fchk);
			fs.Delete();
			DEL_CLASS(hash);
			return 0;
		}
		else
		{
			readSess.hash = hash;
			readSess.readSize = 0;
			readSess.progress = progress;
			fileSize = fs->GetLength();
			readSess.fileSize = fileSize;
			if (progress)
			{
				progress->ProgressStart(path, fileSize);
			}
			hash->Clear();
			NEW_CLASS(reader, IO::ActiveStreamReader(CheckData, &readSess, fs, 1048576));
			reader->ReadStream(bnt);
			if (fileSize == readSess.readSize)
			{
				UOSInt i = path.LastIndexOf(IO::Path::PATH_SEPERATOR);
				hash->GetValue(hashBuff);
				fchk->AddEntry(path.Substring(i + 1), hashBuff);
			}
			else if (!skipError)
			{
				DEL_CLASS(reader);
				fs.Delete();
				if (progress)
				{
					progress->ProgressEnd();
				}
				DEL_CLASS(fchk);
				DEL_CLASS(hash);
				return 0;
			}
			DEL_CLASS(reader);
			fs.Delete();
			if (progress)
			{
				progress->ProgressEnd();
			}
		}
	}
	else if (pt == IO::Path::PathType::Directory)
	{
		NEW_CLASS(fchk, IO::FileCheck(path, chkType));
		UOSInt i = (UOSInt)(path.ConcatTo(&sbuff[2]) - sbuff);
		sbuff[0] = '.';
		sbuff[1] = IO::Path::PATH_SEPERATOR;
		if (sbuff[i - 1] == IO::Path::PATH_SEPERATOR)
		{
			sbuff[i - 1] = 0;
			sbuff[i] = 0;
			i -= 1;
		}
		i = Text::StrLastIndexOfCharC(sbuff, i, IO::Path::PATH_SEPERATOR);
		if (i < 2)
		{
			if (CheckDir(sbuff, &sbuff[i], hash, fchk, progress, skipError))
			{
				DEL_CLASS(fchk);
				fchk = 0;
			}
		}
		else
		{
			if (CheckDir(&sbuff[2], &sbuff[i], hash, fchk, progress, skipError))
			{
				DEL_CLASS(fchk);
				fchk = 0;
			}
		}
		if (progress)
		{
			progress->ProgressEnd();
		}
	}
	else
	{
		fchk = 0;
	}
	DEL_CLASS(hash);
	return fchk;
}

void __stdcall IO::FileCheck::CheckData(const UInt8 *buff, UOSInt buffSize, void *userData)
{
	ReadSess *sess = (ReadSess*)userData;
	sess->hash->Calc(buff, buffSize);
	sess->readSize += buffSize;
	if (sess->progress)
	{
		sess->progress->ProgressUpdate(sess->readSize, sess->fileSize);
	}
}

Bool IO::FileCheck::CheckDir(UTF8Char *fullPath, UTF8Char *hashPath, Crypto::Hash::IHash *hash, IO::FileCheck *fchk, IO::ProgressHandler *progress, Bool skipError)
{
	UTF8Char *sptr = &hashPath[Text::StrCharCnt(hashPath)];
	UTF8Char *sptr2;
	IO::Path::FindFileSession *sess;
	IO::Path::PathType pt;
	NotNullPtr<IO::FileStream> fs;
	IO::ActiveStreamReader *reader;
	ReadSess readSess;
	UInt64 fileSize;
	UInt8 hashBuff[32];
	IO::ActiveStreamReader::BottleNeckType bnt;

	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	sess = IO::Path::FindFile(CSTRP(fullPath, sptr2));
	if (sess)
	{
		while ((sptr2 = IO::Path::FindNextFile(sptr, sess, 0, &pt, 0)) != 0)
		{
			if (pt == IO::Path::PathType::Directory)
			{
				if (sptr[0] == '.')
				{
				}
				else
				{
					if (CheckDir(fullPath, hashPath, hash, fchk, progress, skipError))
					{
						IO::Path::FindFileClose(sess);
						return true;
					}
				}
			}
			else if (pt == IO::Path::PathType::File)
			{
				NEW_CLASSNN(fs, IO::FileStream({fullPath, (UOSInt)(sptr2 - fullPath)}, IO::FileMode::ReadOnly, IO::FileShare::DenyWrite, IO::FileStream::BufferType::NoBuffer));
				if (fs->IsError())
				{
					fs.Delete();
					if (!skipError)
					{
						IO::Path::FindFileClose(sess);
						return true;
					}
				}
				else
				{
					readSess.hash = hash;
					readSess.readSize = 0;
					readSess.progress = progress;
					fileSize = fs->GetLength();
					readSess.fileSize = fileSize;
					if (progress)
					{
						progress->ProgressStart(CSTRP(hashPath, sptr2), fileSize);
					}
					hash->Clear();
					NEW_CLASS(reader, IO::ActiveStreamReader(CheckData, &readSess, fs, 1048576));
					reader->ReadStream(bnt);
					if (fileSize == readSess.readSize)
					{
						hash->GetValue(hashBuff);
						fchk->AddEntry(CSTRP(hashPath, sptr2), hashBuff);
					}
					else if (!skipError)
					{
						DEL_CLASS(reader);
						fs.Delete();
						IO::Path::FindFileClose(sess);
						return true;
					}
					DEL_CLASS(reader);
					fs.Delete();
				}
			}
			else
			{
				IO::Path::FindFileClose(sess);
				return true;
			}
		}

		IO::Path::FindFileClose(sess);
	}
	return false;
}

IO::FileCheck::FileCheck(NotNullPtr<Text::String> name, Crypto::Hash::HashType chkType) : IO::ParsedObject(name)
{
	this->chkType = chkType;
	Crypto::Hash::IHash *hash = Crypto::Hash::HashCreator::CreateHash(chkType);
	if (hash == 0)
	{
		this->hashSize = 4;
		this->chkType = Crypto::Hash::HashType::CRC32;
		this->hashSize = 0;
	}
	else
	{
		this->hashSize = hash->GetResultSize();
		DEL_CLASS(hash);
	}
	this->chkCapacity = 40;
	this->chkValues = MemAlloc(UInt8, this->hashSize * this->chkCapacity);
}

IO::FileCheck::FileCheck(Text::CStringNN name, Crypto::Hash::HashType chkType) : IO::ParsedObject(name)
{
	this->chkType = chkType;
	Crypto::Hash::IHash *hash = Crypto::Hash::HashCreator::CreateHash(chkType);
	if (hash == 0)
	{
		this->hashSize = 4;
		this->chkType = Crypto::Hash::HashType::CRC32;
		this->hashSize = 0;
	}
	else
	{
		this->hashSize = hash->GetResultSize();
		DEL_CLASS(hash);
	}
	this->chkCapacity = 40;
	this->chkValues = MemAlloc(UInt8, this->hashSize * this->chkCapacity);
}

IO::FileCheck::~FileCheck()
{
	LIST_FREE_STRING(&this->fileNames);
	MemFree(this->chkValues);
}

UOSInt IO::FileCheck::GetHashSize() const
{
	return this->hashSize;
}

Crypto::Hash::HashType IO::FileCheck::GetCheckType() const
{
	return this->chkType;
}

UOSInt IO::FileCheck::GetCount() const
{
	return this->fileNames.GetCount();
}

Text::String *IO::FileCheck::GetEntryName(UOSInt index) const
{
	return this->fileNames.GetItem(index);
}

Bool IO::FileCheck::GetEntryHash(UOSInt index, UInt8 *hashVal) const
{
	if (index >= this->fileNames.GetCount())
		return false;
	MemCopyNO(hashVal, &this->chkValues[index * this->hashSize], this->hashSize);
	return true;
}

void IO::FileCheck::AddEntry(Text::CString fileName, UInt8 *hashVal)
{
	UOSInt index = this->fileNames.Add(Text::String::New(fileName));
	if (index >= this->chkCapacity)
	{
		this->chkCapacity = this->chkCapacity << 1;
		UInt8 *newVals = MemAlloc(UInt8, this->chkCapacity * this->hashSize);
		MemCopyNO(newVals, this->chkValues, index * this->hashSize);
		MemFree(this->chkValues);
		this->chkValues = newVals;
	}
	MemCopyNO(&this->chkValues[index * this->hashSize], hashVal, this->hashSize);
}

Bool IO::FileCheck::CheckEntryHash(UOSInt index, UInt8 *hashVal) const
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptrEnd;
	UOSInt i;
	Crypto::Hash::IHash *hash;

	Text::String *fileName = this->fileNames.GetItem(index);
	if (fileName == 0)
		return false;
	sptr = this->sourceName->ConcatTo(sbuff);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
	if (i == INVALID_INDEX)
		return false;
	sptr = &sbuff[i];
	if (fileName->v[0] == '/' || fileName->v[0] == '\\')
	{
		sptrEnd = fileName->ConcatTo(sptr);
	}
	else
	{
		sptrEnd = fileName->ConcatTo(sptr + 1);
	}
	if (IO::Path::PATH_SEPERATOR == '/')
	{
		Text::StrReplace(sptr, '\\', '/');
	}
	else
	{
		Text::StrReplace(sptr, '/', '\\');
	}
	hash = Crypto::Hash::HashCreator::CreateHash(this->chkType);
	if (hash == 0)
		return false;

	IO::FileStream fs(CSTRP(sbuff, sptrEnd), IO::FileMode::ReadOnly, IO::FileShare::DenyWrite, IO::FileStream::BufferType::NoBuffer);
	if (fs.IsError())
	{
		DEL_CLASS(hash);
		return false;
	}
	else
	{
		Bool ret = false;
		ReadSess readSess;
		UInt64 fileSize;
		IO::ActiveStreamReader::BottleNeckType bnt;
		readSess.hash = hash;
		readSess.readSize = 0;
		readSess.progress = 0;
		fileSize = fs.GetLength();
		readSess.fileSize = fileSize;
		hash->Clear();
		IO::ActiveStreamReader reader(CheckData, &readSess, fs, 1048576);
		reader.ReadStream(bnt);
		if (fileSize == readSess.readSize)
		{
			hash->GetValue(hashVal);
			ret = true;
		}
		DEL_CLASS(hash);
		return ret;
	}
}

IO::ParserType IO::FileCheck::GetParserType() const
{
	return IO::ParserType::FileCheck;
}
