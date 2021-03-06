#ifndef _SM_CRYPTO_HASH_BRUTEFORCEATTACK
#define _SM_CRYPTO_HASH_BRUTEFORCEATTACK
#include "Crypto/Hash/IHash.h"
#include "Sync/Mutex.h"

namespace Crypto
{
	namespace Hash
	{
		class BruteForceAttack
		{
		public:
			typedef enum
			{
				CL_ASCII,
				CL_LETTER_NUM,
				CL_WEBPASSWORD,

				CL_FIRST = CL_ASCII,
				CL_LAST = CL_WEBPASSWORD
			} CharLimit;

			typedef enum
			{
				CE_UTF8,
				CE_UTF16LE,
				CE_UTF32LE
			} CharEncoding;
		private:
			Crypto::Hash::IHash *hash;
			Bool toRelease;
			CharEncoding ce;
			CharLimit charLimit;
			UTF8Char resultBuff[64];
			UOSInt threadCnt;
			Bool threadToStop;
			const UInt8 *keyLimit;
			UInt8 keyFirst;
			Sync::Mutex *keyMut;
			UTF8Char keyBuff[64];
			UInt8 hashBuff[64];
			UOSInt maxLeng;
			UInt64 testCnt;

			static UInt32 __stdcall ProcessThread(void *userObj);
			UOSInt GetNextKey(UInt8 *keyBuff, UTF8Char *resultBuff);
		public:
			BruteForceAttack(Crypto::Hash::IHash *hash, Bool toRelease, CharEncoding ce);
			~BruteForceAttack();

			void SetCharLimit(CharLimit charLimit);
			Bool IsProcessing();
			UOSInt GetKeyLeng();
			UTF8Char *GetCurrKey(UTF8Char *key);
			UInt64 GetTestCnt();
			UTF8Char *GetResult(UTF8Char *resultBuff);
			Bool Start(const UInt8 *hashValue, UOSInt minLeng, UOSInt maxLeng);

			static const UTF8Char *CharLimitGetName(CharLimit charLimit);
		};
	}
}
#endif
