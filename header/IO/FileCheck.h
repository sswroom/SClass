#ifndef _SM_IO_FILECHECK
#define _SM_IO_FILECHECK
#include "Data/ArrayListString.h"
#include "IO/ParsedObject.h"
#include "IO/IProgressHandler.h"
#include "Crypto/Hash/IHash.h"

namespace IO
{
	class FileCheck : public IO::ParsedObject
	{
	private:
		Data::ArrayListString fileNames;
		UInt8 *chkValues;
		UOSInt chkCapacity;
		Crypto::Hash::HashType chkType;
		UOSInt hashSize;

	public:
		static FileCheck *CreateCheck(Text::CString path, Crypto::Hash::HashType chkType, IO::IProgressHandler *progress, Bool skipError);
	private:
		static void __stdcall CheckData(const UInt8 *buff, UOSInt buffSize, void *userData);
		static Bool CheckDir(UTF8Char *fullPath, UTF8Char *hashPath, Crypto::Hash::IHash *hash, IO::FileCheck *fchk, IO::IProgressHandler *progress, Bool skipError); //true = error

	public:
		FileCheck(Text::String *name, Crypto::Hash::HashType chkType);
		FileCheck(Text::CString name, Crypto::Hash::HashType chkType);
		virtual ~FileCheck();

		UOSInt GetHashSize() const;
		Crypto::Hash::HashType GetCheckType() const;
		UOSInt GetCount() const;
		Text::String *GetEntryName(UOSInt index) const;
		Bool GetEntryHash(UOSInt index, UInt8 *hashVal) const;
		void AddEntry(Text::CString fileName, UInt8 *hashVal);
		Bool CheckEntryHash(UOSInt index, UInt8 *hashVal) const;

		virtual IO::ParserType GetParserType() const;
	};
}
#endif
