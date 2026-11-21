#include "Stdafx.h"
#include "Crypto/Encrypt/RC4Cipher.h"
#include "Crypto/Hash/MD5.h"
#include "Data/ByteBuffer.h"
#include "Media/CCITTFaxDecoder.h"
#include "Media/ICCProfile.h"
#include "Media/JBIG2Decoder.h"
#include "Media/PDFDocument.h"
#include "Media/StaticImage.h"

void Media::PDFDocument::ClearCachedStreams()
{
	UIntOS i = this->objMap.GetCount();
	while (i-- > 0)
		this->objMap.GetItemNoCheck(i)->ClearDecodedData();
}

Media::PDFDocument::PDFDocument(NN<Text::String> sourceName, Text::CStringNN version) : IO::ParsedObject(sourceName)
{
	this->version = Text::String::New(version);
	this->encryptionRequired = false;
	this->encryptionKeyLen = 0;
	MemClear(this->encryptionKey, sizeof(this->encryptionKey));
}

Media::PDFDocument::~PDFDocument()
{
	this->version->Release();
	MemClear(this->encryptionKey, sizeof(this->encryptionKey));
	UIntOS i = this->objMap.GetCount();
	NN<Media::PDFObject> obj;
	while (i-- > 0)
	{
		obj = this->objMap.GetItemNoCheck(i);
		obj.Delete();
	}
}

IO::ParserType Media::PDFDocument::GetParserType() const
{
	return IO::ParserType::PDFDocument;
}

UIntOS Media::PDFDocument::GetCount() const
{
	return this->objMap.GetCount();
}

NN<Media::PDFObject> Media::PDFDocument::GetItemNoCheck(UIntOS index) const
{
	return this->objMap.GetItemNoCheck(index);
}

Optional<Media::PDFObject> Media::PDFDocument::GetItem(UIntOS index) const
{
	return this->objMap.GetItem(index);
}

NN<Media::PDFObject> Media::PDFDocument::AddObject(UInt32 id)
{
	NN<Media::PDFObject> obj;
	if (this->objMap.Get(id).SetTo(obj))
		return obj;
	NEW_CLASSNN(obj, Media::PDFObject(id, *this));
	this->objMap.Put(id, obj);
	return obj;
}

Optional<Media::PDFObject> Media::PDFDocument::GetObjectById(UInt32 id) const
{
	return this->objMap.Get(id);
}

void Media::PDFDocument::SetEncryptionRequired()
{
	this->ClearCachedStreams();
	this->encryptionRequired = true;
}

Bool Media::PDFDocument::IsEncryptionRequired() const
{
	return this->encryptionRequired;
}

Bool Media::PDFDocument::SetEncryptionKey(UnsafeArray<const UInt8> key, UIntOS keyLen)
{
	if (keyLen == 0 || keyLen > sizeof(this->encryptionKey))
		return false;
	this->ClearCachedStreams();
	MemCopyNO(this->encryptionKey, key.Ptr(), keyLen);
	this->encryptionKeyLen = keyLen;
	return true;
}

Bool Media::PDFDocument::DecryptStreamData(UInt32 objId, UnsafeArray<UInt8> data, UIntOS dataSize) const
{
	if (!this->encryptionRequired)
		return true;
	if (this->encryptionKeyLen == 0)
		return false;

	UInt8 keyData[21];
	MemCopyNO(keyData, this->encryptionKey, this->encryptionKeyLen);
	keyData[this->encryptionKeyLen] = (UInt8)objId;
	keyData[this->encryptionKeyLen + 1] = (UInt8)(objId >> 8);
	keyData[this->encryptionKeyLen + 2] = (UInt8)(objId >> 16);
	keyData[this->encryptionKeyLen + 3] = 0;
	keyData[this->encryptionKeyLen + 4] = 0;
	UInt8 digest[16];
	Crypto::Hash::MD5 md5;
	md5.Calc(UnsafeArray<const UInt8>(keyData), this->encryptionKeyLen + 5);
	md5.GetValue(digest);
	UIntOS objKeyLen = this->encryptionKeyLen + 5;
	if (objKeyLen > 16)
		objKeyLen = 16;
	Crypto::Encrypt::RC4Cipher cipher(UnsafeArray<const UInt8>(digest), objKeyLen);
	cipher.Decrypt(UnsafeArray<const UInt8>(data), dataSize, data);
	return true;
}

Optional<IO::ParsedObject> Media::PDFDocument::SetPObjName(Optional<IO::ParsedObject> pobj, UInt32 objId, Text::CStringNN ext)
{
	NN<IO::ParsedObject> nnpobj;
	if (pobj.SetTo(nnpobj))
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(this->sourceName);
		sb.AppendUTF8Char('.');
		sb.AppendU32(objId);
		if (ext.leng > 0)
		{
			sb.Append(ext);
		}
		nnpobj->SetSourceName(sb.ToCString());
	}
	return pobj;
}
