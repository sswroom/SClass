#ifndef _SM_DB_DBDATAFILE
#define _SM_DB_DBDATAFILE
#include "MyMemory.h"
#include "Data/ArrayListNN.h"
#include "Data/ByteTool.h"
#include "Data/NamedClass.h"
#include "IO/BufferedOutputStream.h"
#include "IO/FileStream.h"
#include "Text/CString.h"

namespace DB
{
	template <class T> class DBDataFile
	{
	private:
		NN<IO::FileStream> fs;
		IO::BufferedOutputStream *cstm;
		NN<Data::NamedClass<T>> cls;
		UInt8 *recordBuff;
		Data::VariItem::ItemType *colTypes;

		static UOSInt ReadInt(const UInt8 *buff, UOSInt ofst, UOSInt *outVal);
		static UOSInt WriteInt(UInt8 *buff, UOSInt ofst, UOSInt val);

		DBDataFile(Text::CStringNN fileName, NN<Data::NamedClass<T>> cls, Bool append);
		~DBDataFile();

		Bool IsError();
		void AddRecord(NN<T> obj);

	public:
		static Bool LoadFile(Text::CStringNN fileName, NN<Data::NamedClass<T>> cls, NN<Data::ArrayListNN<T>> dataListOut);
		static Bool SaveFile(Text::CStringNN fileName, NN<Data::ArrayListNN<T>> dataList, NN<Data::NamedClass<T>> cls);
	};
}

template <class T> UOSInt DB::DBDataFile<T>::ReadInt(const UInt8 *buff, UOSInt ofst, UOSInt *outVal)
{
	UOSInt val = 0;
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
			*outVal = (val << 7) | v;
			return ofst;
		}
	}
}

template <class T> UOSInt DB::DBDataFile<T>::WriteInt(UInt8 *buff, UOSInt ofst, UOSInt val)
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
	this->recordBuff = 0;
	this->colTypes = 0;
	this->cstm = 0;
	IO::FileMode fileMode = append?IO::FileMode::Append:IO::FileMode::Create;
	NEW_CLASSNN(this->fs, IO::FileStream(fileName, fileMode, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (this->fs->IsError())
	{
		return;
	}
	UOSInt k = 0;
	UOSInt l = cls->GetFieldCount();
	this->recordBuff = MemAlloc(UInt8, 65536);
	this->colTypes = MemAlloc(Data::VariItem::ItemType, l);
	if (this->fs->GetPosition() == 0)
	{
		this->recordBuff[0] = 'S';
		this->recordBuff[1] = 'M';
		this->recordBuff[2] = 'D';
		this->recordBuff[3] = 'f';
		while (k < l)
		{
			this->recordBuff[k + 4] = (UInt8)(this->colTypes[k] = cls->GetFieldType(k));
			k++;
		}
		this->recordBuff[l + 4] = 0xff;
		this->fs->Write(this->recordBuff, l + 5);
	}
	else
	{
		while (k < l)
		{
			this->colTypes[k] = cls->GetFieldType(k);
			k++;
		}
	}
	NEW_CLASS(this->cstm, IO::BufferedOutputStream(this->fs, 32768));
}

template <class T> DB::DBDataFile<T>::~DBDataFile()
{
	SDEL_CLASS(this->cstm);
	this->fs.Delete();
	if (this->recordBuff)
		MemFree(this->recordBuff);
	if (this->colTypes)
		MemFree(this->colTypes);
}

template <class T> Bool DB::DBDataFile<T>::IsError()
{
	return this->fs->IsError();
}

template <class T> void DB::DBDataFile<T>::AddRecord(NN<T> obj)
{
	if (this->cstm == 0)
	{
		return;
	}
	Data::VariItem item;
	UOSInt k;
	UOSInt l;
	UOSInt m;
	m = 6;
	k = 0;
	l = this->cls->GetFieldCount();
	while (k < l)
	{
		cls->GetValue(item, k, obj);
		switch (colTypes[k])
		{
		case Data::VariItem::ItemType::F32:
			WriteFloat(&this->recordBuff[m], item.GetItemValue().f32);
			m += 4;
			break;
		case Data::VariItem::ItemType::F64:
			WriteDouble(&this->recordBuff[m], item.GetItemValue().f64);
			m += 8;
			break;
		case Data::VariItem::ItemType::I8:
			this->recordBuff[m] = (UInt8)item.GetItemValue().i8;
			m += 1;
			break;
		case Data::VariItem::ItemType::U8:
			this->recordBuff[m] = item.GetItemValue().u8;
			m += 1;
			break;
		case Data::VariItem::ItemType::I16:
			WriteInt16(&this->recordBuff[m], item.GetItemValue().i16);
			m += 2;
			break;
		case Data::VariItem::ItemType::U16:
			WriteUInt16(&this->recordBuff[m], item.GetItemValue().u16);
			m += 2;
			break;
		case Data::VariItem::ItemType::I32:
			WriteInt32(&this->recordBuff[m], item.GetItemValue().i32);
			m += 4;
			break;
		case Data::VariItem::ItemType::U32:
			WriteUInt32(&this->recordBuff[m], item.GetItemValue().u32);
			m += 4;
			break;
		case Data::VariItem::ItemType::I64:
			WriteInt64(&this->recordBuff[m], item.GetItemValue().i64);
			m += 8;
			break;
		case Data::VariItem::ItemType::U64:
			WriteUInt64(&this->recordBuff[m], item.GetItemValue().u64);
			m += 8;
			break;
		case Data::VariItem::ItemType::BOOL:
			this->recordBuff[m] = item.GetItemValue().boolean?1:0;
			m += 1;
			break;
		case Data::VariItem::ItemType::Str:
			if (item.GetItemType() == Data::VariItem::ItemType::Null)
			{
				this->recordBuff[m] = 0xff;
				m += 1;
			}
			else
			{
				Text::String *s = item.GetItemValue().str;
				m = WriteInt(this->recordBuff, m, s->leng);
				MemCopyNO(&this->recordBuff[m], s->v, s->leng);
				m += s->leng;
			}
			break;
		case Data::VariItem::ItemType::CStr:
			if (item.GetItemType() == Data::VariItem::ItemType::Null)
			{
				this->recordBuff[m] = 0xff;
				m += 1;
			}
			else
			{
				Data::VariItem::ItemValue ival = item.GetItemValue();
				m = WriteInt(this->recordBuff, m, ival.cstr.leng);
				MemCopyNO(&this->recordBuff[m], ival.cstr.v, ival.cstr.leng);
				m += ival.cstr.leng;
			}
			break;
		case Data::VariItem::ItemType::Timestamp:
			if (item.GetItemType() == Data::VariItem::ItemType::Null)
			{
				WriteInt64(&this->recordBuff[m], -1);
				m += 8;
			}
			else
			{
				WriteInt64(&this->recordBuff[m], item.GetItemValue().ts.ToTicks());
				m += 8;
			}
			break;
		case Data::VariItem::ItemType::Date:
			if (item.GetItemType() == Data::VariItem::ItemType::Null)
			{
				WriteInt64(&this->recordBuff[m], -1);
				m += 8;
			}
			else
			{
				WriteInt64(&this->recordBuff[m], item.GetItemValue().date.ToTicks());
				m += 8;
			}
			break;
		case Data::VariItem::ItemType::ByteArr:
			if (item.GetItemType() == Data::VariItem::ItemType::Null)
			{
				this->recordBuff[m] = 0xff;
				m += 1;
			}
			else
			{
				Data::ReadonlyArray<UInt8> *arr = item.GetItemValue().byteArr;
				m = WriteInt(this->recordBuff, m, arr->GetCount());
				MemCopyNO(&this->recordBuff[m], arr->GetArray(), arr->GetCount());
				m += arr->GetCount();
			}
			break;
		case Data::VariItem::ItemType::Vector:
			//////////////////////////////////
			break;
		case Data::VariItem::ItemType::UUID:
			if (item.GetItemType() == Data::VariItem::ItemType::Null)
			{
				WriteInt64(&this->recordBuff[m], -1);
				WriteInt64(&this->recordBuff[m + 8], -1);
				m += 16;
			}
			else
			{
				NotNullPtr<Data::UUID> uuid = item.GetItemValue().uuid;
				uuid->GetValue(&this->recordBuff[m]);
				m += 16;
			}
			break;
		case Data::VariItem::ItemType::Null:
		case Data::VariItem::ItemType::Unknown:
		default:
			this->recordBuff[m] = 0xff;
			m += 1;
			break;
		}
		k++;
	}
	m -= 6;
	if (m < 0x80) // 00 - 7f
	{
		this->recordBuff[5] = (UInt8)m;
		this->cstm->Write(&this->recordBuff[5], m + 1);
	}
	else if (m <= 0x3F7F)
	{
		this->recordBuff[4] = (UInt8)(0x80 | (m >> 7));
		this->recordBuff[5] = (UInt8)(m & 0x7F);
		this->cstm->Write(&this->recordBuff[4], m + 2);
	}
	else if (m <= 0x1FBFFF)
	{
		this->recordBuff[3] = (UInt8)(0x80 | (m >> 14));
		this->recordBuff[4] = (UInt8)(0x80 | ((m >> 7) & 0x7F));
		this->recordBuff[5] = (UInt8)(m & 0x7F);
		this->cstm->Write(&this->recordBuff[3], m + 3);
	}
	else
	{
		this->recordBuff[2] = (UInt8)(0x80 | (m >> 21));
		this->recordBuff[3] = (UInt8)(0x80 | ((m >> 14) & 0x7F));
		this->recordBuff[4] = (UInt8)(0x80 | ((m >> 7) & 0x7F));
		this->recordBuff[5] = (UInt8)(m & 0x7F);
		this->cstm->Write(&this->recordBuff[2], m + 4);
	}
}

template <class T> Bool DB::DBDataFile<T>::LoadFile(Text::CStringNN fileName, NN<Data::NamedClass<T>> cls, NN<Data::ArrayListNN<T>> dataListOut)
{
	UOSInt maxBuffSize = 65536;
	IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		return false;
	}
	Data::VariItem item;
	Bool succ = false;
	UInt8 *buff = MemAlloc(UInt8, maxBuffSize);
	UOSInt buffSize;
	UOSInt k;
	UOSInt l = cls->GetFieldCount();
	UOSInt m;
	UOSInt m2;
	UOSInt m3;
	UOSInt rowSize;
	Data::VariItem::ItemType *colTypes = MemAlloc(Data::VariItem::ItemType, l);
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
						MemCopyNO(buff, &buff[m], buffSize - m);
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
				m2 = ReadInt(buff, m, &rowSize);
				while (m2 + rowSize > buffSize)
				{
					if (m == 0)
					{
					}
					else if (m != buffSize)
					{
						MemCopyNO(buff, &buff[m], buffSize - m);
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
								m2 = ReadInt(buff, m2, &m3);
								NotNullPtr<Text::String> s = Text::String::New(&buff[m2], m3);
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
								m2 = ReadInt(buff, m2, &m3);
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
								NotNullPtr<Data::UUID> uuid;
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
	MemFree(buff);
	MemFree(colTypes);
	return succ;
}

template <class T> Bool DB::DBDataFile<T>::SaveFile(Text::CStringNN fileName, NN<Data::ArrayListNN<T>> dataList, NN<Data::NamedClass<T>> cls)
{
	DB::DBDataFile<T> file(fileName, cls, false);
	if (file.IsError())
	{
		return false;
	}

	UOSInt i = 0;
	UOSInt j = dataList->GetCount();
	while (i < j)
	{
		file.AddRecord(dataList->GetItemNoCheck(i));
		i++;
	}
	return true;
}
#endif
