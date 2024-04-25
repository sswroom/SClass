#ifndef _SM_IO_FILECHECK
#define _SM_IO_FILECHECK
#include "AnyType.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListStringNN.h"
#include "IO/ActiveStreamReader.h"
#include "IO/ParsedObject.h"
#include "IO/ProgressHandler.h"
#include "Crypto/Hash/IHash.h"

namespace IO
{
	class FileCheck : public IO::ParsedObject
	{
	private:
		Data::ArrayListStringNN fileNames;
		UInt8 *chkValues;
		UOSInt chkCapacity;
		Crypto::Hash::HashType chkType;
		UOSInt hashSize;

	public:
		static FileCheck *CreateCheck(Text::CStringNN path, Crypto::Hash::HashType chkType, Optional<IO::ProgressHandler> progress, Bool skipError);
	private:
		static void __stdcall CheckData(const UInt8 *buff, UOSInt buffSize, AnyType userData);
		static Bool CheckDir(NN<IO::ActiveStreamReader> reader, UTF8Char *fullPath, UTF8Char *hashPath, Crypto::Hash::IHash *hash, IO::FileCheck *fchk, Optional<IO::ProgressHandler> progress, Bool skipError); //true = error

	public:
		FileCheck(NN<Text::String> name, Crypto::Hash::HashType chkType);
		FileCheck(Text::CStringNN name, Crypto::Hash::HashType chkType);
		virtual ~FileCheck();

		UOSInt GetHashSize() const;
		Crypto::Hash::HashType GetCheckType() const;
		UOSInt GetCount() const;
		Optional<Text::String> GetEntryName(UOSInt index) const;
		Bool GetEntryHash(UOSInt index, UInt8 *hashVal) const;
		void AddEntry(Text::CStringNN fileName, UInt8 *hashVal);
		Bool CheckEntryHash(UOSInt index, UInt8 *hashVal) const;
		Bool MergeFrom(NN<FileCheck> chk);

		virtual IO::ParserType GetParserType() const;
	};
}
#endif
