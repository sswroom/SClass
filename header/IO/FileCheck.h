#ifndef _SM_IO_FILECHECK
#define _SM_IO_FILECHECK
#include "AnyType.h"
#include "Data/ArrayListStringNN.h"
#include "IO/ActiveStreamReader.h"
#include "IO/ParsedObject.h"
#include "IO/ProgressHandler.h"
#include "IO/Writer.h"
#include "Crypto/Hash/HashAlgorithm.h"

namespace IO
{
	class FileCheck : public IO::ParsedObject
	{
	private:
		Data::ArrayListStringNN fileNames;
		UnsafeArray<UInt8> chkValues;
		UIntOS chkCapacity;
		Crypto::Hash::HashType chkType;
		UIntOS hashSize;

	public:
		static Optional<FileCheck> CreateCheck(Text::CStringNN path, Crypto::Hash::HashType chkType, Optional<IO::ProgressHandler> progress, Bool skipError);
	private:
		static void __stdcall CheckData(Data::ByteArrayR data, AnyType userData);
		static Bool CheckDir(NN<IO::ActiveStreamReader> reader, UnsafeArray<UTF8Char> fullPath, UnsafeArray<UTF8Char> hashPath, NN<Crypto::Hash::HashAlgorithm> hash, IO::FileCheck *fchk, Optional<IO::ProgressHandler> progress, Bool skipError); //true = error

	public:
		FileCheck(NN<Text::String> name, Crypto::Hash::HashType chkType);
		FileCheck(Text::CStringNN name, Crypto::Hash::HashType chkType);
		virtual ~FileCheck();

		UIntOS GetHashSize() const;
		Crypto::Hash::HashType GetCheckType() const;
		UIntOS GetCount() const;
		Optional<Text::String> GetEntryName(UIntOS index) const;
		Bool GetEntryHash(UIntOS index, UnsafeArray<UInt8> hashVal) const;
		void AddEntry(Text::CStringNN fileName, UnsafeArray<UInt8> hashVal);
		Bool CheckEntryHash(UIntOS index, UnsafeArray<UInt8> hashVal, Optional<IO::ProgressHandler> progress, Optional<IO::Writer> verboseWriter) const;
		Bool MergeFrom(NN<FileCheck> chk);

		virtual IO::ParserType GetParserType() const;
	};
}
#endif
