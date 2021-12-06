#ifndef _SM_IO_FILECHECK
#define _SM_IO_FILECHECK
#include "Data/ArrayListStrUTF8.h"
#include "IO/ParsedObject.h"
#include "IO/IProgressHandler.h"
#include "Crypto/Hash/IHash.h"

namespace IO
{
	class FileCheck : public IO::ParsedObject
	{
	public:
		enum class CheckType
		{
			CRC32,
			MD5,
			SHA1,
			MD4
		};

		enum class HashType
		{
			INT32,
			ByteArray
		};
	private:
		Data::ArrayListStrUTF8 *fileNames;
		UInt8 *chkValues;
		UOSInt chkCapacity;
		CheckType chkType;
		UOSInt hashSize;

	public:
		static Crypto::Hash::IHash *CreateHash(CheckType chkType);
		static FileCheck *CreateCheck(const UTF8Char *path, CheckType chkType, IO::IProgressHandler *progress, Bool skipError);
	private:
		static void __stdcall CheckData(const UInt8 *buff, UOSInt buffSize, void *userData);
		static Bool CheckDir(UTF8Char *fullPath, UTF8Char *hashPath, Crypto::Hash::IHash *hash, IO::FileCheck *fchk, IO::IProgressHandler *progress, Bool skipError); //true = error

	public:
		FileCheck(Text::String *name, CheckType chkType);
		FileCheck(const UTF8Char *name, CheckType chkType);
		virtual ~FileCheck();

		HashType GetHashType();
		UOSInt GetHashSize();
		CheckType GetCheckType();
		UOSInt GetCount();
		const UTF8Char *GetEntryName(UOSInt index);
		Bool GetEntryHash(UOSInt index, UInt8 *hashVal);
		void AddEntry(const UTF8Char *fileName, UInt8 *hashVal);
		Bool CheckEntryHash(UOSInt index, UInt8 *hashVal);

		virtual IO::ParserType GetParserType();
	};
}
#endif
