#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/CRC32.h"
#include "Crypto/Hash/CRC32R.h"
#include "Crypto/Hash/MD5.h"
#include "Crypto/Hash/SHA1.h"
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
	IO::IProgressHandler *progress;
} ReadSess;

Crypto::Hash::IHash *IO::FileCheck::CreateHash(CheckType chkType)
{
	Crypto::Hash::IHash *hash;
	if (chkType == IO::FileCheck::CheckType::CRC32)
	{
		NEW_CLASS(hash, Crypto::Hash::CRC32R(Crypto::Hash::CRC32::GetPolynormialIEEE()));
	}
	else if (chkType == IO::FileCheck::CheckType::MD4)
	{
		return 0;
	}
	else if (chkType == IO::FileCheck::CheckType::MD5)
	{
		NEW_CLASS(hash, Crypto::Hash::MD5());
	}
	else if (chkType == IO::FileCheck::CheckType::SHA1)
	{
		NEW_CLASS(hash, Crypto::Hash::SHA1());
	}
	else
	{
		return 0;
	}
	return hash;
}

IO::FileCheck *IO::FileCheck::CreateCheck(const UTF8Char *path, IO::FileCheck::CheckType chkType, IO::IProgressHandler *progress, Bool skipError)
{
	UTF8Char sbuff[1024];
	UInt8 hashBuff[32];
	Crypto::Hash::IHash *hash;
	IO::FileCheck *fchk;
	IO::Path::PathType pt;
	UInt64 fileSize;
	ReadSess readSess;
	IO::FileStream *fs;
	IO::ActiveStreamReader *reader;
	IO::ActiveStreamReader::BottleNeckType bnt;

	hash = CreateHash(chkType);
	if (hash == 0)
	{
		return 0;
	}

	pt = IO::Path::GetPathType(path);
	if (pt == IO::Path::PT_FILE)
	{
		NEW_CLASS(fchk, IO::FileCheck(path, chkType));

		NEW_CLASS(fs, IO::FileStream(path, IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyWrite, IO::FileStream::BufferType::NoBuffer));
		if (fs->IsError())
		{
			DEL_CLASS(fchk);
			DEL_CLASS(fs);
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
			reader->ReadStream(&bnt);
			if (fileSize == readSess.readSize)
			{
				UOSInt i = Text::StrLastIndexOf(path, IO::Path::PATH_SEPERATOR);
				hash->GetValue(hashBuff);
				fchk->AddEntry(&path[i + 1], hashBuff);
			}
			else if (!skipError)
			{
				DEL_CLASS(reader);
				DEL_CLASS(fs);
				if (progress)
				{
					progress->ProgressEnd();
				}
				DEL_CLASS(fchk);
				DEL_CLASS(hash);
				return 0;
			}
			DEL_CLASS(reader);
			DEL_CLASS(fs);
			if (progress)
			{
				progress->ProgressEnd();
			}
		}
	}
	else if (pt == IO::Path::PT_DIRECTORY)
	{
		NEW_CLASS(fchk, IO::FileCheck(path, chkType));
		UOSInt i = (UOSInt)(Text::StrConcat(&sbuff[2], path) - sbuff);
		sbuff[0] = '.';
		sbuff[1] = IO::Path::PATH_SEPERATOR;
		if (sbuff[i - 1] == IO::Path::PATH_SEPERATOR)
		{
			sbuff[i - 1] = 0;
			sbuff[i] = 0;
		}
		i = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR);
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

Bool IO::FileCheck::CheckDir(UTF8Char *fullPath, UTF8Char *hashPath, Crypto::Hash::IHash *hash, IO::FileCheck *fchk, IO::IProgressHandler *progress, Bool skipError)
{
	UTF8Char *sptr = &hashPath[Text::StrCharCnt(hashPath)];
	IO::Path::FindFileSession *sess;
	IO::Path::PathType pt;
	IO::FileStream *fs;
	IO::ActiveStreamReader *reader;
	ReadSess readSess;
	UInt64 fileSize;
	UInt8 hashBuff[32];
	IO::ActiveStreamReader::BottleNeckType bnt;

	*sptr++ = IO::Path::PATH_SEPERATOR;
	Text::StrConcat(sptr, IO::Path::ALL_FILES);
	sess = IO::Path::FindFile(fullPath);
	if (sess)
	{
		while (IO::Path::FindNextFile(sptr, sess, 0, &pt, 0))
		{
			if (pt == IO::Path::PT_DIRECTORY)
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
			else if (pt == IO::Path::PT_FILE)
			{
				NEW_CLASS(fs, IO::FileStream(fullPath, IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyWrite, IO::FileStream::BufferType::NoBuffer));
				if (fs->IsError())
				{
					DEL_CLASS(fs);
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
						progress->ProgressStart(hashPath, fileSize);
					}
					hash->Clear();
					NEW_CLASS(reader, IO::ActiveStreamReader(CheckData, &readSess, fs, 1048576));
					reader->ReadStream(&bnt);
					if (fileSize == readSess.readSize)
					{
						hash->GetValue(hashBuff);
						fchk->AddEntry(hashPath, hashBuff);
					}
					else if (!skipError)
					{
						DEL_CLASS(reader);
						DEL_CLASS(fs);
						IO::Path::FindFileClose(sess);
						return true;
					}
					DEL_CLASS(reader);
					DEL_CLASS(fs);
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

IO::FileCheck::FileCheck(const UTF8Char *name, CheckType chkType) : IO::ParsedObject(name)
{
	this->chkType = chkType;

	if (chkType == IO::FileCheck::CheckType::MD5)
	{
		this->hashSize = 16;
	}
	else if (chkType == IO::FileCheck::CheckType::SHA1)
	{
		this->hashSize = 20;
	}
	else if (chkType == IO::FileCheck::CheckType::MD4)
	{
		this->hashSize = 16;
	}
	else if (chkType == IO::FileCheck::CheckType::CRC32)
	{
		this->hashSize = 4;
	}
	else
	{
		this->chkType = IO::FileCheck::CheckType::CRC32;
		this->hashSize = 4;
	}

	NEW_CLASS(this->fileNames, Data::ArrayListStrUTF8());
	this->chkCapacity = 40;
	this->chkValues = MemAlloc(UInt8, this->hashSize * this->chkCapacity);
}

IO::FileCheck::~FileCheck()
{
	UOSInt i = this->fileNames->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->fileNames->GetItem(i));
	}
	DEL_CLASS(this->fileNames);
	MemFree(this->chkValues);
}

IO::FileCheck::HashType IO::FileCheck::GetHashType()
{
	if (this->chkType == IO::FileCheck::CheckType::CRC32)
	{
		return IO::FileCheck::HashType::INT32;
	}
	else
	{
		return IO::FileCheck::HashType::ByteArray;
	}
}

UOSInt IO::FileCheck::GetHashSize()
{
	return this->hashSize;
}

IO::FileCheck::CheckType IO::FileCheck::GetCheckType()
{
	return this->chkType;
}

UOSInt IO::FileCheck::GetCount()
{
	return this->fileNames->GetCount();
}

const UTF8Char *IO::FileCheck::GetEntryName(UOSInt index)
{
	return this->fileNames->GetItem(index);
}

Bool IO::FileCheck::GetEntryHash(UOSInt index, UInt8 *hashVal)
{
	if (index >= this->fileNames->GetCount())
		return false;
	MemCopyNO(hashVal, &this->chkValues[index * this->hashSize], this->hashSize);
	return true;
}

void IO::FileCheck::AddEntry(const UTF8Char *fileName, UInt8 *hashVal)
{
	UOSInt index = this->fileNames->Add(Text::StrCopyNew(fileName));
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

Bool IO::FileCheck::CheckEntryHash(UOSInt index, UInt8 *hashVal)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	Crypto::Hash::IHash *hash;
	IO::FileStream *fs;

	const UTF8Char *fileName = this->fileNames->GetItem(index);
	if (fileName == 0)
		return false;
	sptr = Text::StrConcat(sbuff, this->sourceName);
	i = Text::StrLastIndexOf(sbuff, IO::Path::PATH_SEPERATOR);
	if (i == INVALID_INDEX)
		return false;
	sptr = &sbuff[i];
	if (fileName[0] == IO::Path::PATH_SEPERATOR)
	{
		Text::StrConcat(sptr, fileName);
	}
	else
	{
		Text::StrConcat(sptr + 1, fileName);
	}
	hash = CreateHash(this->chkType);
	if (hash == 0)
		return false;

	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyWrite, IO::FileStream::BufferType::NoBuffer));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		DEL_CLASS(hash);
		return false;
	}
	else
	{
		Bool ret = false;
		ReadSess readSess;
		IO::ActiveStreamReader *reader;
		UInt64 fileSize;
		IO::ActiveStreamReader::BottleNeckType bnt;
		readSess.hash = hash;
		readSess.readSize = 0;
		readSess.progress = 0;
		fileSize = fs->GetLength();
		readSess.fileSize = fileSize;
		hash->Clear();
		NEW_CLASS(reader, IO::ActiveStreamReader(CheckData, &readSess, fs, 1048576));
		reader->ReadStream(&bnt);
		if (fileSize == readSess.readSize)
		{
			hash->GetValue(hashVal);
			ret = true;
		}
		DEL_CLASS(reader);
		DEL_CLASS(fs);
		DEL_CLASS(hash);
		return ret;
	}
}

IO::ParsedObject::ParserType IO::FileCheck::GetParserType()
{
	return IO::ParsedObject::PT_FILE_CHECK;
}
