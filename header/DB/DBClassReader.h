#ifndef _SM_DB_DBCLASSREADER
#define _SM_DB_DBCLASSREADER
#include "Data/NamedClass.h"
#include "DB/DBReader.h"

namespace DB
{
	template <class T> class DBClassReader
	{
	private:
		NotNullPtr<DB::DBReader> reader;
		Data::NamedClass<T> *cls;
		UOSInt *colIndex;

	public:
		DBClassReader(NotNullPtr<DB::DBReader> reader, Data::NamedClass<T> *cls);
		~DBClassReader();

		Bool IsError();
		T *ReadNext();
		Bool ReadNext(T *obj);
		Bool ReadAll(Data::ArrayList<T*> *outList);
	};


	template <class T> DBClassReader<T>::DBClassReader(NotNullPtr<DB::DBReader> reader, Data::NamedClass<T> *cls)
	{
		this->reader = reader;
		this->cls = cls;
		this->colIndex = 0;

		UTF8Char sbuff[256];
		UTF8Char sbuff2[256];
		UTF8Char *sptr;
		Data::StringMap<UOSInt> colMap2;
		Bool clsValid = true;
		UOSInt i = 0;
		UOSInt j = this->reader->ColCount();
		while (i < j)
		{
			this->reader->GetName(i, sbuff);
			sptr = DB::DBUtil::DB2FieldName(sbuff2, sbuff);
			colMap2.Put(CSTRP(sbuff2, sptr), i);

			i++;
		}

		i = 0;
		j = cls->GetFieldCount();
		while (i < j)
		{
			if (!colMap2.ContainsKey(cls->GetFieldName(i)))
			{
				clsValid = false;
			}
			i++;
		}

		if (!clsValid)
		{
			return;
		}

		this->colIndex = MemAlloc(UOSInt, j);
		i = 0;
		while (i < j)
		{
			this->colIndex[i] = colMap2.Get(cls->GetFieldName(i));
			i++;
		}
	}

	template <class T> DBClassReader<T>::~DBClassReader()
	{
		if (this->colIndex)
		{
			MemFree(this->colIndex);
			this->colIndex = 0;
		}
	}

	template <class T> Bool DBClassReader<T>::IsError()
	{
		return this->colIndex == 0;
	}

	template <class T> T *DBClassReader<T>::ReadNext()
	{
		if (this->reader->ReadNext())
		{
			T *o = this->cls->CreateObject();
			Data::VariItem item;
			UOSInt i = 0;
			UOSInt j = this->cls->GetFieldCount();
			while (i < j)
			{
				this->reader->GetVariItem(this->colIndex[i], item);
				this->cls->SetFieldClearItem(o, i, item);
				i++;
			}
			return o;
		}
		return 0;
	}

	template <class T> Bool DBClassReader<T>::ReadNext(T *obj)
	{
		if (this->reader->ReadNext())
		{
			Data::VariItem item;
			UOSInt i = 0;
			UOSInt j = this->cls->GetFieldCount();
			while (i < j)
			{
				this->reader->GetVariItem(this->colIndex[i], item);
				this->cls->SetFieldClearItem(obj, i, item);
				i++;
			}
			return true;
		}
		return false;
	}

	template <class T> Bool DBClassReader<T>::ReadAll(Data::ArrayList<T*> *outList)
	{
		if (this->colIndex)
		{
			T *o;
			while ((o = this->ReadNext()) != 0)
			{
				outList->Add(o);
			}
			return true;
		}
		return false;
	}
}
#endif
