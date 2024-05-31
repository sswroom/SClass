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
	NN<Crypto::Hash::IHash> hash;
	UInt64 readSize;
	UInt64 fileSize;
	Optional<IO::ProgressHandler> progress;
} ReadSess;

Optional<IO::FileCheck> IO::FileCheck::CreateCheck(Text::CStringNN path, Crypto::Hash::HashType chkType, Optional<IO::ProgressHandler> progress, Bool skipError)
{
	UTF8Char sbuff[1024];
	UInt8 hashBuff[32];
	NN<Crypto::Hash::IHash> hash;
	IO::FileCheck *fchk;
	IO::Path::PathType pt;
	UInt64 fileSize;
	ReadSess readSess;
	NN<IO::FileStream> fs;
	IO::ActiveStreamReader::BottleNeckType bnt;
	NN<IO::ProgressHandler> nnprogress;

	if (!Crypto::Hash::HashCreator::CreateHash(chkType).SetTo(hash))
	{
		return 0;
	}

	IO::ActiveStreamReader reader(CheckData, &readSess, 1048576);
	pt = IO::Path::GetPathType(path);
	if (pt == IO::Path::PathType::File)
	{
		NEW_CLASS(fchk, IO::FileCheck(path, chkType));

		NEW_CLASSNN(fs, IO::FileStream(path, IO::FileMode::ReadOnly, IO::FileShare::DenyWrite, IO::FileStream::BufferType::NoBuffer));
		if (fs->IsError())
		{
			DEL_CLASS(fchk);
			fs.Delete();
			hash.Delete();
			return 0;
		}
		else
		{
			readSess.hash = hash;
			readSess.readSize = 0;
			readSess.progress = progress;
			fileSize = fs->GetLength();
			readSess.fileSize = fileSize;
			if (progress.SetTo(nnprogress))
			{
				nnprogress->ProgressStart(path, fileSize);
			}
			hash->Clear();
			reader.SetUserData(&readSess);
			reader.ReadStream(fs, bnt);
			if (fileSize == readSess.readSize)
			{
				UOSInt i = path.LastIndexOf(IO::Path::PATH_SEPERATOR);
				hash->GetValue(hashBuff);
				fchk->AddEntry(path.Substring(i + 1), hashBuff);
			}
			else if (!skipError)
			{
				fs.Delete();
				if (progress.SetTo(nnprogress))
				{
					nnprogress->ProgressEnd();
				}
				DEL_CLASS(fchk);
				hash.Delete();
				return 0;
			}
			fs.Delete();
			if (progress.SetTo(nnprogress))
			{
				nnprogress->ProgressEnd();
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
			if (CheckDir(reader, sbuff, &sbuff[i + 1], hash, fchk, progress, skipError))
			{
				DEL_CLASS(fchk);
				fchk = 0;
			}
		}
		else
		{
			if (CheckDir(reader, &sbuff[2], &sbuff[i + 1], hash, fchk, progress, skipError))
			{
				DEL_CLASS(fchk);
				fchk = 0;
			}
		}
		if (progress.SetTo(nnprogress))
		{
			nnprogress->ProgressEnd();
		}
	}
	else
	{
		fchk = 0;
	}
	hash.Delete();
	return fchk;
}

void __stdcall IO::FileCheck::CheckData(const UInt8 *buff, UOSInt buffSize, AnyType userData)
{
	NN<ReadSess> sess = userData.GetNN<ReadSess>();
	sess->hash->Calc(buff, buffSize);
	sess->readSize += buffSize;
	NN<IO::ProgressHandler> progress;
	if (sess->progress.SetTo(progress))
	{
		progress->ProgressUpdate(sess->readSize, sess->fileSize);
	}
}

Bool IO::FileCheck::CheckDir(NN<IO::ActiveStreamReader> reader, UnsafeArray<UTF8Char> fullPath, UnsafeArray<UTF8Char> hashPath, NN<Crypto::Hash::IHash> hash, IO::FileCheck *fchk, Optional<IO::ProgressHandler> progress, Bool skipError)
{
	UnsafeArray<UTF8Char> sptr = &hashPath[Text::StrCharCnt(hashPath)];
	UnsafeArray<UTF8Char> sptr2;
	IO::Path::FindFileSession *sess;
	IO::Path::PathType pt;
	NN<IO::FileStream> fs;
	ReadSess readSess;
	UInt64 fileSize;
	UInt8 hashBuff[32];
	IO::ActiveStreamReader::BottleNeckType bnt;
	NN<IO::ProgressHandler> nnprogress;

	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr2 = Text::StrConcatC(sptr, IO::Path::ALL_FILES, IO::Path::ALL_FILES_LEN);
	sess = IO::Path::FindFile(CSTRP(fullPath, sptr2));
	if (sess)
	{
		while (IO::Path::FindNextFile(sptr, sess, 0, &pt, 0).SetTo(sptr2))
		{
			if (pt == IO::Path::PathType::Directory)
			{
				if (sptr[0] == '.')
				{
				}
				else
				{
					if (CheckDir(reader, fullPath, hashPath, hash, fchk, progress, skipError))
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
					if (progress.SetTo(nnprogress))
					{
						nnprogress->ProgressStart(CSTRP(hashPath, sptr2), fileSize);
					}
					hash->Clear();
					reader->SetUserData(&readSess);
					reader->ReadStream(fs, bnt);
					if (fileSize == readSess.readSize)
					{
						hash->GetValue(hashBuff);
						fchk->AddEntry(CSTRP(hashPath, sptr2), hashBuff);
					}
					else if (!skipError)
					{
						fs.Delete();
						IO::Path::FindFileClose(sess);
						return true;
					}
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

IO::FileCheck::FileCheck(NN<Text::String> name, Crypto::Hash::HashType chkType) : IO::ParsedObject(name)
{
	this->chkType = chkType;
	NN<Crypto::Hash::IHash> hash;
	if (!Crypto::Hash::HashCreator::CreateHash(chkType).SetTo(hash))
	{
		this->hashSize = 4;
		this->chkType = Crypto::Hash::HashType::CRC32;
		this->hashSize = 0;
	}
	else
	{
		this->hashSize = hash->GetResultSize();
		hash.Delete();
	}
	this->chkCapacity = 40;
	this->chkValues = MemAlloc(UInt8, this->hashSize * this->chkCapacity);
}

IO::FileCheck::FileCheck(Text::CStringNN name, Crypto::Hash::HashType chkType) : IO::ParsedObject(name)
{
	this->chkType = chkType;
	NN<Crypto::Hash::IHash> hash;
	if (!Crypto::Hash::HashCreator::CreateHash(chkType).SetTo(hash))
	{
		this->hashSize = 4;
		this->chkType = Crypto::Hash::HashType::CRC32;
		this->hashSize = 0;
	}
	else
	{
		this->hashSize = hash->GetResultSize();
		hash.Delete();
	}
	this->chkCapacity = 40;
	this->chkValues = MemAlloc(UInt8, this->hashSize * this->chkCapacity);
}

IO::FileCheck::~FileCheck()
{
	this->fileNames.FreeAll();
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

Optional<Text::String> IO::FileCheck::GetEntryName(UOSInt index) const
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

void IO::FileCheck::AddEntry(Text::CStringNN fileName, UInt8 *hashVal)
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

Bool IO::FileCheck::CheckEntryHash(UOSInt index, UInt8 *hashVal, Optional<IO::Writer> verboseWriter) const
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptrEnd;
	UOSInt i;
	NN<Crypto::Hash::IHash> hash;
	NN<IO::Writer> writer;

	NN<Text::String> fileName;
	if (!this->fileNames.GetItem(index).SetTo(fileName))
	{
		if (verboseWriter.SetTo(writer))
			writer->WriteLine(CSTR("Unknown file name"));
		return false;
	}
	if (verboseWriter.SetTo(writer))
	{
		writer->Write(CSTR("Checking file: "));
		writer->WriteLine(fileName->ToCString());
	}
	sptr = this->sourceName->ConcatTo(sbuff);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
	if (i == INVALID_INDEX)
	{
		sptr = sbuff;
		*sptr++ = '.';
		*sptr++ = IO::Path::PATH_SEPERATOR;
	}
	else
	{
		sptr = &sbuff[i + 1];
	}
	if (fileName->v[0] == '/' || fileName->v[0] == '\\')
	{
		sptrEnd = fileName->ConcatTo(sptr + 1);
	}
	else
	{
		sptrEnd = fileName->ConcatTo(sptr);
	}
	if (IO::Path::PATH_SEPERATOR == '/')
	{
		Text::StrReplace(sptr, '\\', '/');
	}
	else
	{
		Text::StrReplace(sptr, '/', '\\');
	}
	if (!Crypto::Hash::HashCreator::CreateHash(this->chkType).SetTo(hash))
	{
		if (verboseWriter.SetTo(writer))
			writer->WriteLine(CSTR("Error in creating hash calculator"));
		return false;
	}

	IO::FileStream fs(CSTRP(sbuff, sptrEnd), IO::FileMode::ReadOnly, IO::FileShare::DenyWrite, IO::FileStream::BufferType::NoBuffer);
	if (fs.IsError())
	{
		if (verboseWriter.SetTo(writer))
		{
			writer->Write(CSTR("Error in opening file: "));
			writer->WriteLine(CSTRP(sbuff, sptrEnd));
		}
		hash.Delete();
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
		IO::ActiveStreamReader reader(CheckData, &readSess, 1048576);
		reader.ReadStream(fs, bnt);
		if (fileSize == readSess.readSize)
		{
			hash->GetValue(hashVal);
			ret = true;
		}
		else
		{
			if (verboseWriter.SetTo(writer))
			{
				writer->Write(CSTR("Error in reading file: "));
				writer->WriteLine(CSTRP(sbuff, sptrEnd));
			}
		}
		hash.Delete();
		return ret;
	}
}

Bool IO::FileCheck::MergeFrom(NN<FileCheck> chk)
{
	if (this->chkType != chk->chkType)
		return false;
	NN<Text::String> s;
	UInt8 val[64];
	UOSInt i = 0;
	UOSInt j = chk->GetCount();
	while (i < j)
	{
		if (chk->GetEntryName(i).SetTo(s) && chk->GetEntryHash(i, val))
		{
			this->AddEntry(s->ToCString(), val);
		}
		i++;
	}
	return true;
}

IO::ParserType IO::FileCheck::GetParserType() const
{
	return IO::ParserType::FileCheck;
}
