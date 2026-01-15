#ifndef _SM_CRYPTO_HASH_BRUTEFORCEATTACK
#define _SM_CRYPTO_HASH_BRUTEFORCEATTACK
#include "AnyType.h"
#include "Crypto/Hash/HashValidator.h"
#include "Sync/Mutex.h"
#include "Text/CString.h"

namespace Crypto
{
	namespace Hash
	{
		class BruteForceAttack
		{
		public:
			enum class CharLimit
			{
				ASCII,
				LetterNum,
				WebPassword,

				First = ASCII,
				Last = WebPassword
			};

			enum class CharEncoding
			{
				UTF8,
				UTF16LE,
				UTF32LE
			};
		private:
			NN<Crypto::Hash::HashValidator> validator;
			CharEncoding ce;
			CharLimit charLimit;
			UTF8Char resultBuff[64];
			UIntOS threadCnt;
			Bool threadToStop;
			UnsafeArray<const UInt8> keyLimit;
			UInt8 keyFirst;
			Sync::Mutex keyMut;
			UTF8Char keyBuff[64];
			UIntOS maxLeng;
			UInt64 testCnt;

			static UInt32 __stdcall ProcessThread(AnyType userObj);
			UIntOS GetNextKey(UnsafeArray<UInt8> keyBuff, UnsafeArray<UTF8Char> resultBuff);
		public:
			BruteForceAttack(NN<Crypto::Hash::HashValidator> validator, CharEncoding ce);
			~BruteForceAttack();

			void SetCharLimit(CharLimit charLimit);
			Bool IsProcessing();
			UIntOS GetKeyLeng();
			UnsafeArrayOpt<UTF8Char> GetCurrKey(UnsafeArray<UTF8Char> key);
			UInt64 GetTestCnt();
			UnsafeArrayOpt<UTF8Char> GetResult(UnsafeArray<UTF8Char> resultBuff);
			Bool Start(UnsafeArray<const UTF8Char> hashStr, UIntOS hashLen, UIntOS minLeng, UIntOS maxLeng);

			static Text::CStringNN CharLimitGetName(CharLimit charLimit);
		};
	}
}
#endif
