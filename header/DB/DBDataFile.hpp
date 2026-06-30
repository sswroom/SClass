#ifndef _SM_DB_DBDATAFILE
#define _SM_DB_DBDATAFILE
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Data/ArrayListNN.hpp"
#include "Data/NamedClass.hpp"
#include "IO/BufferedOutputStream.h"
#include "IO/FileStream.h"
#include "Text/CString.h"

namespace DB
{
	template <class T> class DBDataFile
	{
	private:
		NN<IO::FileStream> fs;
		Optional<IO::BufferedOutputStream> cstm;
		NN<Data::NamedClass<T>> cls;
		UnsafeArrayOpt<UInt8> recordBuff;
		UnsafeArrayOpt<Data::VariItem::ItemType> colTypes;

		static UIntOS ReadInt(UnsafeArray<const UInt8> buff, UIntOS ofst, OutParam<UIntOS> outVal);
		static UIntOS WriteInt(UnsafeArray<UInt8> buff, UIntOS ofst, UIntOS val);

		DBDataFile(Text::CStringNN fileName, NN<Data::NamedClass<T>> cls, Bool append);
		~DBDataFile();

		Bool IsError();
		void AddRecord(NN<T> obj);

	public:
		static Bool LoadFile(Text::CStringNN fileName, NN<Data::NamedClass<T>> cls, NN<Data::ArrayListNN<T>> dataListOut);
		static Bool SaveFile(Text::CStringNN fileName, NN<Data::ArrayListNN<T>> dataList, NN<Data::NamedClass<T>> cls);
	};
}

template <class T> UIntOS DB::DBDataFile<T>::ReadInt(UnsafeArray<const UInt8> buff, UIntOS ofst, OutParam<UIntOS> outVal)
{
	UIntOS val = 0;
	UInt8 v;
	while (true)
	{
		v = buff[ofst];
		ofst++;
		if (v & 0x80)
		{
			val = (val << 7) | (v & 0x7F);
		}
		else
		{
			outVal.Set((val << 7) | v);
			return ofst;
		}
	}
}

template <class T> UIntOS DB::DBDataFile<T>::WriteInt(UnsafeArray<UInt8> buff, UIntOS ofst, UIntOS val)
{
	if (val < 0x80) // 00 - 7f
	{
		buff[ofst] = (UInt8)val;
		return ofst + 1;
	}
	else if (val <= 0x3F7F)
	{
		buff[ofst] = (UInt8)(0x80 | (val >> 7));
		buff[ofst + 1] = (UInt8)(val & 0x7F);
		return ofst + 2;
	}
	else if (val <= 0x1FBFFF)
	{
		buff[ofst] = (UInt8)(0x80 | (val >> 14));
		buff[ofst + 1] = (UInt8)(0x80 | ((val >> 7) & 0x7F));
		buff[ofst + 2] = (UInt8)(val & 0x7F);
		return ofst + 3;
	}
	else if (val <= 0xFDFFFFF)
	{
		buff[ofst] = (UInt8)(0x80 | (val >> 21));
		buff[ofst + 1] = (UInt8)(0x80 | ((val >> 14) & 0x7F));
		buff[ofst + 2] = (UInt8)(0x80 | ((val >> 7) & 0x7F));
		buff[ofst + 3] = (UInt8)(val & 0x7F);
		return ofst + 4;
	}
#if _OSINT_SIZE == 32
	else
	{
		buff[ofst] = (UInt8)(0x80 | (val >> 28));
		buff[ofst + 1] = (UInt8)(0x80 | ((val >> 21) & 0x7F));
		buff[ofst + 2] = (UInt8)(0x80 | ((val >> 14) & 0x7F));
		buff[ofst + 3] = (UInt8)(0x80 | ((val >> 7) & 0x7F));
		buff[ofst + 4] = (UInt8)(val & 0x7F);
		return ofst + 5;
	}
#else
	else if (val <= 0x7EFFFFFFFLL)
	{
		buff[ofst] = (UInt8)(0x80 | (val >> 28));
		buff[ofst + 1] = (UInt8)(0x80 | ((val >> 21) & 0x7F));
		buff[ofst + 2] = (UInt8)(0x80 | ((val >> 14) & 0x7F));
		buff[ofst + 3] = (UInt8)(0x80 | ((val >> 7) & 0x7F));
		buff[ofst + 4] = (UInt8)(val & 0x7F);
		return ofst + 5;
	}
	else
	{
		buff[ofst] = (UInt8)(0x80 | (val >> 35));
		buff[ofst + 1] = (UInt8)(0x80 | ((val >> 28) & 0x7F));
		buff[ofst + 2] = (UInt8)(0x80 | ((val >> 21) & 0x7F));
		buff[ofst + 3] = (UInt8)(0x80 | ((val >> 14) & 0x7F));
		buff[ofst + 4] = (UInt8)(0x80 | ((val >> 7) & 0x7F));
		buff[ofst + 5] = (UInt8)(val & 0x7F);
		return ofst + 6;
	}
#endif
}

template <class T> DB::DBDataFile<T>::DBDataFile(Text::CStringNN fileName, NN<Data::NamedClass<T>> cls, Bool append)
{
	this->cls = cls;
	this->recordBuff = nullptr;
	this->colTypes = nullptr;
	this->cstm = nullptr;
	IO::FileMode fileMode = append?IO::FileMode::Append:IO::FileMode::Create;
	NEW_CLASSNN(this->fs, IO::FileStream(fileName, fileMode, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (this->fs->IsError())
	{
		return;
	}
	UnsafeArray<UInt8> recordBuff;
	UnsafeArray<Data::VariItem::ItemType> colTypes;
	UIntOS k = 0;
	UIntOS l = cls->GetFieldCount();
	this->recordBuff = recordBuff = MemAllocArr(UInt8, 65536);
	this->colTypes = colTypes = MemAllocArr(Data::VariItem::ItemType, l);
	if (this->fs->GetPosition() == 0)
	{
		recordBuff[0] = 'S';
		recordBuff[1] = 'M';
		recordBuff[2] = 'D';
		recordBuff[3] = 'f';
		while (k < l)
		{
			recordBuff[k + 4] = (UInt8)(colTypes[k] = cls->GetFieldType(k));
			k++;
		}
		recordBuff[l + 4] = 0xff;
		this->fs->Write(Data::ByteArrayR(recordBuff, l + 5));
	}
	else
	{
		while (k < l)
		{
			colTypes[k] = cls->GetFieldType(k);
			k++;
		}
	}
	NEW_CLASSOPT(this->cstm, IO::BufferedOutputStream(this->fs, 32768));
}

template <class T> DB::DBDataFile<T>::~DBDataFile()
{
	this->cstm.Delete();
	this->fs.Delete();
	UnsafeArray<UInt8> recordBuff;
	if (this->recordBuff.SetTo(recordBuff))
		MemFreeArr(recordBuff);
	UnsafeArray<Data::VariItem::ItemType> colTypes;
	if (this->colTypes.SetTo(colTypes))
		MemFreeArr(colTypes);
}

template <class T> Bool DB::DBDataFile<T>::IsError()
{
	return this->fs->IsError();
}

template <class T> void DB::DBDataFile<T>::AddRecord(NN<T> obj)
{
	UnsafeArray<UInt8> recordBuff;
	UnsafeArray<Data::VariItem::ItemType> colTypes;
	NN<IO::BufferedOutputStream> cstm;
	if (!this->cstm.SetTo(cstm) || !this->recordBuff.SetTo(recordBuff) || !this->colTypes.SetTo(colTypes))
	{
		return;
	}
	Data::VariItem item;
	UIntOS k;
	UIntOS l;
	UIntOS m;
	m = 6;
	k = 0;
	l = this->cls->GetFieldCount();
	while (k < l)
	{
		cls->GetValue(item, k, obj);
		switch (colTypes[k])
		{
		case Data::VariItem::ItemType::F32:
			WriteFloat(&recordBuff[m], item.GetItemValue().f32);
			m += 4;
			break;
		case Data::VariItem::ItemType::F64:
			WriteDouble(&recordBuff[m], item.GetItemValue().f64);
			m += 8;
			break;
		case Data::VariItem::ItemType::I8:
			recordBuff[m] = (UInt8)item.GetItemValue().i8;
			m += 1;
			break;
		case Data::VariItem::ItemType::U8:
			recordBuff[m] = item.GetItemValue().u8;
			m += 1;
			break;
		case Data::VariItem::ItemType::I16:
			WriteInt16(&recordBuff[m], item.GetItemValue().i16);
			m += 2;
			break;
		case Data::VariItem::ItemType::U16:
			WriteUInt16(&recordBuff[m], item.GetItemValue().u16);
			m += 2;
			break;
		case Data::VariItem::ItemType::NI32:
		case Data::VariItem::ItemType::I32:
			WriteInt32(&recordBuff[m], item.GetItemValue().i32);
			m += 4;
			break;
		case Data::VariItem::ItemType::U32:
			WriteUInt32(&recordBuff[m], item.GetItemValue().u32);
			m += 4;
			break;
		case Data::VariItem::ItemType::I64:
			WriteInt64(&recordBuff[m], item.GetItemValue().i64);
			m += 8;
			break;
		case Data::VariItem::ItemType::U64:
			WriteUInt64(&recordBuff[m], item.GetItemValue().u64);
			m += 8;
			break;
		case Data::VariItem::ItemType::BOOL:
			recordBuff[m] = item.GetItemValue().boolean?1:0;
			m += 1;
			break;
		case Data::VariItem::ItemType::Str:
			if (item.GetItemType() == Data::VariItem::ItemType::Null)
			{
				recordBuff[m] = 0xff;
				m += 1;
			}
			else
			{
				Text::String *s = item.GetItemValue().str;
				m = WriteInt(recordBuff, m, s->leng);
				MemCopyNO(&recordBuff[m], s->v.Ptr(), s->leng);
				m += s->leng;
			}
			break;
		case Data::VariItem::ItemType::CStr:
			if (item.GetItemType() == Data::VariItem::ItemType::Null)
			{
				recordBuff[m] = 0xff;
				m += 1;
			}
			else
			{
				Data::VariItem::ItemValue ival = item.GetItemValue();
				m = WriteInt(recordBuff, m, ival.cstr.leng);
				MemCopyNO(&recordBuff[m], ival.cstr.v.Ptr(), ival.cstr.leng);
				m += ival.cstr.leng;
			}
			break;
		case Data::VariItem::ItemType::Timestamp:
			if (item.GetItemType() == Data::VariItem::ItemType::Null)
			{
				WriteInt64(&recordBuff[m], -1);
				m += 8;
			}
			else
			{
				WriteInt64(&recordBuff[m], item.GetItemValue().ts.ToTicks());
				m += 8;
			}
			break;
		case Data::VariItem::ItemType::Date:
			if (item.GetItemType() == Data::VariItem::ItemType::Null)
			{
				WriteInt64(&recordBuff[m], -1);
				m += 8;
			}
			else
			{
				WriteInt64(&recordBuff[m], item.GetItemValue().date.ToTicks());
				m += 8;
			}
			break;
		case Data::VariItem::ItemType::ByteArr:
			if (item.GetItemType() == Data::VariItem::ItemType::Null)
			{
				recordBuff[m] = 0xff;
				m += 1;
			}
			else
			{
				NN<Data::ReadonlyArray<UInt8>> arr = item.GetItemValue().byteArr;
				m = WriteInt(recordBuff, m, arr->GetCount());
				MemCopyNO(&recordBuff[m], arr->GetArray().Ptr(), arr->GetCount());
				m += arr->GetCount();
			}
			break;
		case Data::VariItem::ItemType::Vector:
			//////////////////////////////////
			break;
		case Data::VariItem::ItemType::UUID:
			if (item.GetItemType() == Data::VariItem::ItemType::Null)
			{
				WriteInt64(&recordBuff[m], -1);
				WriteInt64(&recordBuff[m + 8], -1);
				m += 16;
			}
			else
			{
				NN<Data::UUID> uuid = item.GetItemValue().uuid;
				uuid->GetValue(&recordBuff[m]);
				m += 16;
			}
			break;
		case Data::VariItem::ItemType::Null:
		case Data::VariItem::ItemType::Unknown:
		default:
			recordBuff[m] = 0xff;
			m += 1;
			break;
		}
		k++;
	}
	m -= 6;
	if (m < 0x80) // 00 - 7f
	{
		recordBuff[5] = (UInt8)m;
		cstm->Write(Data::ByteArrayR(&recordBuff[5], m + 1));
	}
	else if (m <= 0x3F7F)
	{
		recordBuff[4] = (UInt8)(0x80 | (m >> 7));
		recordBuff[5] = (UInt8)(m & 0x7F);
		cstm->Write(Data::ByteArrayR(&recordBuff[4], m + 2));
	}
	else if (m <= 0x1FBFFF)
	{
		recordBuff[3] = (UInt8)(0x80 | (m >> 14));
		recordBuff[4] = (UInt8)(0x80 | ((m >> 7) & 0x7F));
		recordBuff[5] = (UInt8)(m & 0x7F);
		cstm->Write(Data::ByteArrayR(&recordBuff[3], m + 3));
	}
	else
	{
		recordBuff[2] = (UInt8)(0x80 | (m >> 21));
		recordBuff[3] = (UInt8)(0x80 | ((m >> 14) & 0x7F));
		recordBuff[4] = (UInt8)(0x80 | ((m >> 7) & 0x7F));
		recordBuff[5] = (UInt8)(m & 0x7F);
		cstm->Write(Data::ByteArrayR(&recordBuff[2], m + 4));
	}
}

template <class T> Bool DB::DBDataFile<T>::LoadFile(Text::CStringNN fileName, NN<Data::NamedClass<T>> cls, NN<Data::ArrayListNN<T>> dataListOut)
{
	UIntOS maxBuffSize = 65536;
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		return false;
	}
	Data::VariItem item;
	Bool succ = false;
	UnsafeArray<UInt8> buff = MemAllocArr(UInt8, maxBuffSize);
	UIntOS buffSize;
	UIntOS k;
	UIntOS l = cls->GetFieldCount();
	UIntOS m;
	UIntOS m2;
	UIntOS m3;
	UIntOS rowSize;
	UnsafeArray<Data::VariItem::ItemType> colTypes = MemAllocArr(Data::VariItem::ItemType, l);
	buffSize = fs.Read(Data::ByteArray(buff, maxBuffSize));
	if (buff[0] == 'S' && buff[1] == 'M' && buff[2] == 'D' && buff[3] == 'f')
	{
		succ = true;

		k = 0;
		while (k < l)
		{
			if (buff[k + 4] != (UInt8)(colTypes[k] = cls->GetFieldType(k)))
			{
				succ = false;
				break;
			}
			k++;
		}
		if (buff[l + 4] != 0xff)
		{
			succ = false;
		}
		m = l + 5;
		if (succ)
		{
			while (succ)
			{
				if (m + 4 > buffSize)
				{
					if (m == 0)
					{
					}
					else if (m != buffSize)
					{
						MemCopyO(&buff[0], &buff[m], buffSize - m);
						buffSize -= m;
						m = 0;
					}
					else
					{
						buffSize = 0;
						m = 0;
					}
					m2 = fs.Read(Data::ByteArray(&buff[buffSize], maxBuffSize - buffSize));
					if (m2 == 0)
					{
						break;
					}
					buffSize += m2;
				}
				m2 = ReadInt(buff, m, rowSize);
				while (m2 + rowSize > buffSize)
				{
					if (m == 0)
					{
					}
					else if (m != buffSize)
					{
						MemCopyO(&buff[0], &buff[m], buffSize - m);
						buffSize -= m;
						m2 -= m;
						m = 0;
					}
					else
					{
						buffSize = 0;
						m2 -= m;
						m = 0;
					}
					k = fs.Read(Data::ByteArray(&buff[buffSize], maxBuffSize - buffSize));
					if (k == 0)
					{
						succ = false;
						break;
					}
					buffSize += k;
				}
				if (succ)
				{
					NN<T> obj = cls->CreateObject();
					m = m2 + rowSize;
					k = 0;
					while (k < l)
					{
						switch (colTypes[k])
						{
						case Data::VariItem::ItemType::F32:
							item.SetF32(ReadFloat(&buff[m2]));
							cls->SetField(obj, k, item);
							m2 += 4;
							break;
						case Data::VariItem::ItemType::F64:
							item.SetF64(ReadDouble(&buff[m2]));
							cls->SetField(obj, k, item);
							m2 += 8;
							break;
						case Data::VariItem::ItemType::I8:
							item.SetI8((Int8)buff[m2]);
							cls->SetField(obj, k, item);
							m2 += 1;
							break;
						case Data::VariItem::ItemType::U8:
							item.SetU8(buff[m2]);
							cls->SetField(obj, k, item);
							m2 += 1;
							break;
						case Data::VariItem::ItemType::I16:
							item.SetI16(ReadInt16(&buff[m2]));
							cls->SetField(obj, k, item);
							m2 += 2;
							break;
						case Data::VariItem::ItemType::U16:
							item.SetU16(ReadUInt16(&buff[m2]));
							cls->SetField(obj, k, item);
							m2 += 2;
							break;
						case Data::VariItem::ItemType::I32:
							item.SetI32(ReadInt32(&buff[m2]));
							cls->SetField(obj, k, item);
							m2 += 4;
							break;
						case Data::VariItem::ItemType::U32:
							item.SetU32(ReadUInt32(&buff[m2]));
							cls->SetField(obj, k, item);
							m2 += 4;
							break;
						case Data::VariItem::ItemType::NI32:
							item.SetI32(ReadInt32(&buff[m2]));
							cls->SetField(obj, k, item);
							m2 += 4;
							break;
						case Data::VariItem::ItemType::I64:
							item.SetI64(ReadInt64(&buff[m2]));
							cls->SetField(obj, k, item);
							m2 += 8;
							break;
						case Data::VariItem::ItemType::U64:
							item.SetU64(ReadUInt64(&buff[m2]));
							cls->SetField(obj, k, item);
							m2 += 8;
							break;
						case Data::VariItem::ItemType::BOOL:
							item.SetBool(buff[m2] != 0);
							cls->SetField(obj, k, item);
							m2 += 1;
							break;
						case Data::VariItem::ItemType::Str:
						case Data::VariItem::ItemType::CStr:
							if (buff[m2] == 0xff)
							{
								m2 += 1;
							}
							else
							{
								m2 = ReadInt(buff, m2, m3);
								NN<Text::String> s = Text::String::New(&buff[m2], m3);
								item.SetStr(s);
								s->Release();
								cls->SetField(obj, k, item);
								m2 += m3;
							}
							break;
						case Data::VariItem::ItemType::Timestamp:
							{
								Int64 ticks = ReadInt64(&buff[m2]);
								if (ticks == -1)
								{
								}
								else
								{
									item.SetDate(Data::Timestamp(ticks, 0));
									cls->SetFieldClearItem(obj, k, item);
								}
								m2 += 8;
							}
							break;
						case Data::VariItem::ItemType::Date:
							{
								Int64 ticks = ReadInt64(&buff[m2]);
								if (ticks == -1)
								{
								}
								else
								{
									item.SetDate(Data::Date(ticks / 86400000));
									cls->SetFieldClearItem(obj, k, item);
								}
								m2 += 8;
							}
							break;
						case Data::VariItem::ItemType::ByteArr:
							if (buff[m2] == 0xff)
							{
								m2 += 1;
							}
							else
							{
								m2 = ReadInt(buff, m2, m3);
								item.SetByteArr(&buff[m2], m3);
								cls->SetFieldClearItem(obj, k, item);
								m2 += m3;
							}
							break;
						case Data::VariItem::ItemType::Vector:
							//////////////////////////////////
							break;
						case Data::VariItem::ItemType::UUID:
							{
								NN<Data::UUID> uuid;
								NEW_CLASSNN(uuid, Data::UUID(&buff[m2]));
								item.SetUUIDDirect(uuid);
								cls->SetFieldClearItem(obj, k, item);
								m2 += 16;
							}
							break;
						case Data::VariItem::ItemType::Null:
						case Data::VariItem::ItemType::Unknown:
						default:
							m2 += 1;
							break;
						}
						k++;
					}
					dataListOut->Add(obj);
				}
			}
		}
	}
	MemFreeArr(buff);
	MemFreeArr(colTypes);
	return succ;
}

template <class T> Bool DB::DBDataFile<T>::SaveFile(Text::CStringNN fileName, NN<Data::ArrayListNN<T>> dataList, NN<Data::NamedClass<T>> cls)
{
	DB::DBDataFile<T> file(fileName, cls, false);
	if (file.IsError())
	{
		return false;
	}

	UIntOS i = 0;
	UIntOS j = dataList->GetCount();
	while (i < j)
	{
		file.AddRecord(dataList->GetItemNoCheck(i));
		i++;
	}
	return true;
}
#endif
