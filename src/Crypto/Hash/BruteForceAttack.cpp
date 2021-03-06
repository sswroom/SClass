#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Hash/BruteForceAttack.h"
#include "Data/ByteTool.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"

const UInt8 BruteForceAttack_LimitASCII[] = {
	0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0
};

const UInt8 BruteForceAttack_LimitLetterNum[] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0
};

const UInt8 BruteForceAttack_LimitWebPassword[] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0
};

UInt32 __stdcall Crypto::Hash::BruteForceAttack::ProcessThread(void *userObj)
{
	Crypto::Hash::BruteForceAttack *me = (Crypto::Hash::BruteForceAttack*)userObj;
	UInt8 keyBuff[256];
	UTF8Char result[64];
	UOSInt keySize;
	UOSInt hashSize;
	Crypto::Hash::IHash *hash;
	Bool eq;
	UOSInt i;
	Sync::Interlocked::Increment(&me->threadCnt);
	hash = me->hash->Clone();
	hashSize = hash->GetResultSize();
	while (!me->threadToStop)
	{
		keySize = me->GetNextKey(keyBuff, result);
		if (keySize <= 0)
			break;
	
		hash->Clear();
		hash->Calc(keyBuff, keySize);
		hash->GetValue(keyBuff);
		eq = true;
		i = hashSize;
		while (i-- > 0)
		{
			if (keyBuff[i] != me->hashBuff[i])
			{
				eq = false;
				break;
			}
		}
		if (eq)
		{
			Text::StrConcat(me->resultBuff, result);
			break;
		}
	}
	DEL_CLASS(hash);
	Sync::Interlocked::Decrement((OSInt*)&me->threadCnt);
	return 0;
}

UOSInt Crypto::Hash::BruteForceAttack::GetNextKey(UInt8 *keyBuff, UTF8Char *resultBuff)
{
	UOSInt ret;
	UOSInt len;
	UOSInt i;
	UInt8 c;
	Sync::MutexUsage mutUsage(this->keyMut);
	len = (UOSInt)(Text::StrConcat(resultBuff, this->keyBuff) - resultBuff);
	if (len > this->maxLeng)
	{
		mutUsage.EndUse();
		return 0;
	}
	switch (this->ce)
	{
	case CE_UTF32LE:
		i = 0;
		while (i < len)
		{
			WriteInt32(&keyBuff[i * 2], this->keyBuff[i]);
			i++;
		}
		ret = len * 4;
		break;
	case CE_UTF16LE:
		i = 0;
		while (i < len)
		{
			WriteInt16(&keyBuff[i * 2], this->keyBuff[i]);
			i++;
		}
		ret = len * 2;
		break;
	case CE_UTF8:
		MemCopyNO(keyBuff, this->keyBuff, len);
		ret = len;
		break;
	default:
		ret = 0;
		break;
	}
	i = len;
	while (i-- > 0)
	{
		c = (UInt8)(this->keyBuff[i] + 1);
		while (c < 0x80)
		{
			if (this->keyLimit[c])
			{
				this->keyBuff[i] = c;
				break;
			}
			c++;
		}
		if (c < 0x80)
		{
			break;
		}
	}
	if (i < 0)
	{
		c = this->keyFirst;
		i = 0;
		while (i <= len)
		{
			this->keyBuff[i] = c;
			i++;
		}
		this->keyBuff[i] = 0;
	}
	else
	{
		i++;
		c = this->keyFirst;
		while (i < len)
		{
			this->keyBuff[i] = c;
			i++;
		}
	}
	this->testCnt++;
	mutUsage.EndUse();
	return ret;
}

Crypto::Hash::BruteForceAttack::BruteForceAttack(Crypto::Hash::IHash *hash, Bool toRelease, CharEncoding ce)
{
	this->hash = hash;
	this->toRelease = toRelease;
	this->ce = ce;
	this->charLimit = CL_ASCII;
	this->resultBuff[0] = 0;
	this->threadCnt = 0;
	this->threadToStop = false;
	this->maxLeng = 0;
	NEW_CLASS(this->keyMut, Sync::Mutex());
}

Crypto::Hash::BruteForceAttack::~BruteForceAttack()
{
	this->threadToStop = true;
	while (this->threadCnt > 0)
	{
		Sync::Thread::Sleep(1);
	}
	if (this->toRelease)
	{
		DEL_CLASS(this->hash);
		this->hash = 0;
	}
	DEL_CLASS(this->keyMut);
}

void Crypto::Hash::BruteForceAttack::SetCharLimit(CharLimit charLimit)
{
	this->charLimit = charLimit;
}

Bool Crypto::Hash::BruteForceAttack::IsProcessing()
{
	return this->threadCnt > 0;
}

UTF8Char *Crypto::Hash::BruteForceAttack::GetCurrKey(UTF8Char *key)
{
	if (this->threadCnt > 0)
	{
		Sync::MutexUsage mutUsage(this->keyMut);
		key = Text::StrConcat(key, this->keyBuff);
		mutUsage.EndUse();
		return key;
	}
	return 0;
}

UOSInt Crypto::Hash::BruteForceAttack::GetKeyLeng()
{
	UOSInt len;
	if (this->threadCnt > 0)
	{
		Sync::MutexUsage mutUsage(this->keyMut);
		len = Text::StrCharCnt(this->keyBuff);
		mutUsage.EndUse();
		return len;
	}
	return 0;
}

UInt64 Crypto::Hash::BruteForceAttack::GetTestCnt()
{
	return this->testCnt;
}

UTF8Char *Crypto::Hash::BruteForceAttack::GetResult(UTF8Char *resultBuff)
{
	if (this->resultBuff[0])
		return Text::StrConcat(resultBuff, this->resultBuff);
	return 0;
}

Bool Crypto::Hash::BruteForceAttack::Start(const UInt8 *hashValue, UOSInt minLeng, UOSInt maxLeng)
{
	if (this->threadCnt > 0)
	{
		return false;
	}
	MemCopyNO(this->hashBuff, hashValue, this->hash->GetResultSize());
	this->maxLeng = maxLeng;
	switch (this->charLimit)
	{
	case CL_ASCII:
	default:
		this->keyLimit = BruteForceAttack_LimitASCII;
		break;
	case CL_LETTER_NUM:
		this->keyLimit = BruteForceAttack_LimitLetterNum;
		break;
	case CL_WEBPASSWORD:
		this->keyLimit = BruteForceAttack_LimitWebPassword;
		break;
	}
	
	UInt8 c = 0;
	while (this->keyLimit[c] == 0)
		c++;
	this->keyFirst = c;
	UOSInt i;
	i = 0;
	while (i < minLeng)
	{
		this->keyBuff[i] = c;
		i++;
	}
	this->keyBuff[minLeng] = 0;
	this->resultBuff[0] = 0;
	this->testCnt = 0;
	i = Sync::Thread::GetThreadCnt();
	while (i-- > 0)
	{
		Sync::Thread::Create(ProcessThread, this);
	}
	return true;
}

const UTF8Char *Crypto::Hash::BruteForceAttack::CharLimitGetName(CharLimit charLimit)
{
	switch (charLimit)
	{
	case CL_ASCII:
		return (const UTF8Char*)"ASCII";
	case CL_LETTER_NUM:
		return (const UTF8Char*)"Letters and Numbers";
	case CL_WEBPASSWORD:
		return (const UTF8Char*)"Web Password";
	default:
		return (const UTF8Char*)"Unknown";
	}
}
