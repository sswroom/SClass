#include "Stdafx.h"
#include "MemTool.h"
#include "MyMemory.h"
#include "Crypto/Hash/BruteForceAttack.h"
#include "Data/ByteTool.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
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
	Crypto::Hash::HashValidatorSess *sess;
	Sync::Interlocked::IncrementUOS(me->threadCnt);
	sess = me->validator->CreateSess();;
	while (!me->threadToStop)
	{
		keySize = me->GetNextKey(keyBuff, result);
		if (keySize <= 0)
			break;
	
		if (me->validator->IsMatch(sess, keyBuff, keySize))
		{
			Text::StrConcat(me->resultBuff, result);
			break;
		}
	}
	me->validator->DeleteSess(sess);
	Sync::Interlocked::DecrementUOS(me->threadCnt);
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
	case CharEncoding::UTF32LE:
		i = 0;
		while (i < len)
		{
			WriteInt32(&keyBuff[i * 2], this->keyBuff[i]);
			i++;
		}
		ret = len * 4;
		break;
	case CharEncoding::UTF16LE:
		i = 0;
		while (i < len)
		{
			WriteInt16(&keyBuff[i * 2], this->keyBuff[i]);
			i++;
		}
		ret = len * 2;
		break;
	case CharEncoding::UTF8:
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

Crypto::Hash::BruteForceAttack::BruteForceAttack(Crypto::Hash::HashValidator *validator, CharEncoding ce)
{
	this->validator = validator;
	this->ce = ce;
	this->charLimit = CharLimit::ASCII;
	this->resultBuff[0] = 0;
	this->threadCnt = 0;
	this->threadToStop = false;
	this->maxLeng = 0;
}

Crypto::Hash::BruteForceAttack::~BruteForceAttack()
{
	this->threadToStop = true;
	while (this->threadCnt > 0)
	{
		Sync::SimpleThread::Sleep(1);
	}
	DEL_CLASS(this->validator);
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

Bool Crypto::Hash::BruteForceAttack::Start(const UTF8Char *hashStr, UOSInt hashLen, UOSInt minLeng, UOSInt maxLeng)
{
	if (this->threadCnt > 0)
	{
		return false;
	}
	if (!this->validator->SetHash(hashStr, hashLen))
		return false;
	this->maxLeng = maxLeng;
	switch (this->charLimit)
	{
	case CharLimit::ASCII:
	default:
		this->keyLimit = BruteForceAttack_LimitASCII;
		break;
	case CharLimit::LetterNum:
		this->keyLimit = BruteForceAttack_LimitLetterNum;
		break;
	case CharLimit::WebPassword:
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
	i = Sync::ThreadUtil::GetThreadCnt();
	while (i-- > 0)
	{
		Sync::ThreadUtil::Create(ProcessThread, this);
	}
	return true;
}

Text::CStringNN Crypto::Hash::BruteForceAttack::CharLimitGetName(CharLimit charLimit)
{
	switch (charLimit)
	{
	case CharLimit::ASCII:
		return CSTR("ASCII");
	case CharLimit::LetterNum:
		return CSTR("Letters and Numbers");
	case CharLimit::WebPassword:
		return CSTR("Web Password");
	default:
		return CSTR("Unknown");
	}
}
