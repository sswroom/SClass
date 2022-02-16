#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Net/ASN1MIB.h"
#include "Net/ASN1Util.h"
#include "Net/MIBReader.h"
#include "Text/CharUtil.h"
#include "Text/StringBuilderUTF8.h"
#define DEBUGOBJ "id-dsa"

UOSInt Net::ASN1MIB::CalcLineSpace(const UTF8Char *txt)
{
	UOSInt ret = 0;
	UTF8Char c;
	while ((c = *txt++))
	{
		if (c == ' ')
		{
			ret++;
		}
		else if (c == '\t')
		{
			ret += 4;
		}
		else
		{
			return ret;
		}
	}
	return ret;
}

void Net::ASN1MIB::ModuleAppendOID(Net::ASN1MIB::ModuleInfo *module, ObjectInfo *obj)
{
	OSInt i;
	OSInt j;
	OSInt k;
	OSInt l;
	ObjectInfo *obj2;
	if (obj->oidLen <= 0)
		return;
	i = 0;
	j = (OSInt)module->oidList->GetCount() - 1;
	while (i <= j)
	{
		k = (i + j) >> 1;
		obj2 = module->oidList->GetItem((UOSInt)k);
		l = Net::ASN1Util::OIDCompare(obj2->oid, obj2->oidLen, obj->oid, obj->oidLen);
		if (l > 0)
		{
			j = k - 1;
		}
		else if (l < 0)
		{
			i = k + 1;
		}
		else
		{
			return;
		}
	}
	module->oidList->Insert((UOSInt)i, obj);
}

Bool Net::ASN1MIB::ParseObjectOID(ModuleInfo *module, ObjectInfo *obj, Text::String *oriS, Text::StringBuilderUTF8 *errMessage)
{
	const UTF8Char *csptr = oriS->v;
	const UTF8Char *csptrEnd = oriS->GetEndPtr();
	UTF8Char c;
	const UTF8Char *oidName;
	UOSInt oidNameLen;
	const UTF8Char *oidNextLev;
	UOSInt oidNextLen;
	Bool isFirst = false;
	OSInt i;
	Text::StringBuilderUTF8 sb;
	while (true)
	{
		c = *csptr++;
		if (c == ' ')
		{

		}
		else if (c == '{')
		{
			break;
		}
		else if ((i = module->objKeys->SortedIndexOfPtr(csptr - 1, (UOSInt)(csptrEnd - csptr - 1))) >= 0)
		{
			ObjectInfo *refObj = module->objValues->GetItem((UOSInt)i);
			if (!refObj->parsed)
			{
				if (!ApplyModuleOID(module, refObj, errMessage))
				{
					return false;
				}
			}
			csptr = refObj->typeVal->v;
		}
		else
		{
			errMessage->Append(obj->objectName);
			errMessage->AppendC(UTF8STRC(": OID Format error 1: \""));
			errMessage->Append(oriS);
			errMessage->AppendC(UTF8STRC("\""));
			return false;
		}
	}
	while (true)
	{
		c = *csptr++;
		if (c == ' ')
		{

		}
		else if (c == '}' || c == 0)
		{
			errMessage->Append(obj->objectName);
			errMessage->AppendC(UTF8STRC(": OID Format error 2: \""));
			errMessage->Append(oriS);
			errMessage->AppendC(UTF8STRC("\""));
			return false;
		}
		else
		{
			oidName = csptr - 1;
			break;
		}
	}
	while (true)
	{
		c = *csptr++;
		if (c == ' ')
		{
			oidNameLen = (UOSInt)(csptr - oidName - 1);
			break;
		}
		else if (c == '}' || c == 0)
		{
			errMessage->Append(obj->objectName);
			errMessage->AppendC(UTF8STRC(": OID Format error 3: \""));
			errMessage->Append(oriS);
			errMessage->AppendC(UTF8STRC("\""));
			return false;
		}
	}
	sb.ClearStr();
	sb.AppendC(oidName, oidNameLen);
	if (sb.Equals(UTF8STRC("iso")))
	{
		obj->oid[0] = 40;
		obj->oidLen = 1;
		isFirst = true;
	}
	else if (sb.Equals(UTF8STRC("iso(1)")))
	{
		obj->oid[0] = 40;
		obj->oidLen = 1;
		isFirst = true;
	}
	else if (sb.Equals(UTF8STRC("joint-iso-ccitt(2)")) || sb.Equals(UTF8STRC("joint-iso-itu-t(2)")) || sb.Equals(UTF8STRC("joint-iso-itu-t")) || sb.Equals(UTF8STRC("2")))
	{
		obj->oid[0] = 80;
		obj->oidLen = 1;
		isFirst = true;
	}
	else if (sb.Equals(UTF8STRC("0")))
	{
		obj->oid[0] = 0;
		obj->oidLen = 1;
		isFirst = true;
	}
	else
	{
		OSInt i = module->objKeys->SortedIndexOfPtr(sb.ToString(), sb.GetLength());
		ObjectInfo *obj2;
		if (i < 0)
		{
//			i = this->globalModule.objKeys->SortedIndexOf(sb.ToString());
//			if (i < 0)
//			{
				errMessage->Append(obj->objectName);
				errMessage->AppendC(UTF8STRC(": OID Name \""));
				errMessage->AppendC(sb.ToString(), sb.GetLength());
				errMessage->AppendC(UTF8STRC("\" not found"));
				return false;
//			}
//			else
//			{
//				obj2 = this->globalModule.objValues->GetItem(i);
//			}
		}
		else
		{
			obj2 = module->objValues->GetItem((UOSInt)i);
		}
		if (!obj2->parsed)
		{
			if (!ApplyModuleOID(module, obj2, errMessage))
			{
				return false;
			}
		}
		if (obj2->oidLen == 0)
		{
			if (obj2->typeVal != 0)
			{
				if (!ParseObjectOID(module, obj2, obj2->typeVal, errMessage))
				{
					return false;
				}
			}
		}
		if (obj2->oidLen == 0)
		{
			errMessage->Append(obj->objectName);
			errMessage->AppendC(UTF8STRC(": OID Name \""));
			errMessage->AppendC(sb.ToString(), sb.GetLength());
			errMessage->AppendC(UTF8STRC("\" is not OID"));
			return false;
		}
		MemCopyNO(obj->oid, obj2->oid, obj2->oidLen);
		obj->oidLen = obj2->oidLen;
	}
	
	while (true)
	{
		while (true)
		{
			c = *csptr++;
			if (c == ' ')
			{

			}
			else if (c == '}')
			{
				while (true)
				{
					c = *csptr++;
					if (c == ' ')
					{
					}
					else if (c == 0)
					{
						return true;
					}
					else
					{
						errMessage->Append(obj->objectName);
						errMessage->AppendC(UTF8STRC(": OID Format error 4: \""));
						errMessage->Append(oriS);
						errMessage->AppendC(UTF8STRC("\""));
						return false;
					}
				}
			}
			else
			{
				oidNextLev = csptr - 1;
				break;
			}
		}
		while (true)
		{
			c = *csptr++;
			if (c == ' ' || c == '}')
			{
				i = 0;
				while (csptr[i] == ' ')
				{
					i++;
				}
				if (csptr[i] != '(')
				{
					oidNextLen = (UOSInt)(csptr - oidNextLev - 1);
					if (c == '}')
					{
						csptr--;
					}
					break;
				}
			}
		}

		UInt32 v;
		UOSInt i;
		UOSInt j;
		sb.ClearStr();
		sb.AppendC(oidNextLev, oidNextLen);
		i = sb.IndexOf('(');
		j = sb.IndexOf(')');
		if (i != INVALID_INDEX && j != INVALID_INDEX && j > i)
		{
			sb.SetSubstr(i + 1);
			sb.TrimToLength(j - i - 1);
			if (!sb.ToUInt32(&v))
			{
				errMessage->Append(obj->objectName);
				errMessage->AppendC(UTF8STRC(": OID Format error 5: \""));
				errMessage->Append(oriS);
				errMessage->AppendC(UTF8STRC("\""));
				return false;
			}
		}
		else if (sb.Equals(UTF8STRC("standard")))
		{
			v = 0;
		}
		else
		{
			if (!sb.ToUInt32(&v))
			{
				errMessage->Append(obj->objectName);
				errMessage->AppendC(UTF8STRC(": OID Format error 6: \""));
				errMessage->Append(oriS);
				errMessage->AppendC(UTF8STRC("\""));
				return false;
			}
		}
		
		if (v < 128)
		{
			if (isFirst)
			{
				obj->oid[0] = (UInt8)(obj->oid[0] + v);
				isFirst = false;
			}
			else
			{
				obj->oid[obj->oidLen] = (UInt8)v;
				obj->oidLen = obj->oidLen + 1;
			}
			
		}
		else if (v < 0x4000)
		{
			obj->oid[obj->oidLen] = (UInt8)(0x80 | (v >> 7));
			obj->oid[obj->oidLen + 1] = (UInt8)(v & 0x7f);
			obj->oidLen = obj->oidLen + 2;
		}
		else if (v < 0x200000)
		{
			obj->oid[obj->oidLen] = (UInt8)(0x80 | (v >> 14));
			obj->oid[obj->oidLen + 1] = (UInt8)(0x80 | ((v >> 7) & 0x7f));
			obj->oid[obj->oidLen + 2] = (UInt8)(v & 0x7f);
			obj->oidLen = obj->oidLen + 3;
		}
		else if (v < 0x10000000)
		{
			obj->oid[obj->oidLen] = (UInt8)(0x80 | (v >> 21));
			obj->oid[obj->oidLen + 1] = (UInt8)(0x80 | ((v >> 14) & 0x7f));
			obj->oid[obj->oidLen + 2] = (UInt8)(0x80 | ((v >> 7) & 0x7f));
			obj->oid[obj->oidLen + 3] = (UInt8)(v & 0x7f);
			obj->oidLen = obj->oidLen + 4;
		}
		else
		{
			obj->oid[obj->oidLen] = (UInt8)(0x80 | (v >> 28));
			obj->oid[obj->oidLen + 1] = (UInt8)(0x80 | ((v >> 21) & 0x7f));
			obj->oid[obj->oidLen + 2] = (UInt8)(0x80 | ((v >> 14) & 0x7f));
			obj->oid[obj->oidLen + 3] = (UInt8)(0x80 | ((v >> 7) & 0x7f));
			obj->oid[obj->oidLen + 4] = (UInt8)(v & 0x7f);
			obj->oidLen = obj->oidLen + 5;
		}
	}

	return true;
}

Bool Net::ASN1MIB::ParseObjectBegin(Net::MIBReader *reader, ObjectInfo *obj, Text::StringBuilderUTF8 *errMessage)
{
	Text::StringBuilderUTF8 sb;
	while (true)
	{
		sb.ClearStr();
		if (!reader->ReadLine(&sb))
		{
			errMessage->AppendC(UTF8STRC("Object end not found"));
			return false;
		}
		if (sb.GetLength() > 0)
		{
			if (sb.EndsWith(UTF8STRC("BEGIN")))
			{
				errMessage->AppendC(UTF8STRC("Nested begin found"));
				return false;
			}
			else if (sb.EndsWith(UTF8STRC("END")))
			{
				return true;
			}
		}
	}
}

Bool Net::ASN1MIB::ParseModule(Net::MIBReader *reader, ModuleInfo *module, Text::StringBuilderUTF8 *errMessage)
{
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	UOSInt j;
	UOSInt lineSpace;
	ObjectInfo *obj;
	ObjectInfo *currObj = 0;
	Text::StringBuilderUTF8 sbObjValName;
	Text::StringBuilderUTF8 sbObjValCont;
	UOSInt objLineSpace = 0;
	Bool objIsEqual = false;
	UTF8Char objBrkType = 0;
	Bool succ;
	Bool isQuotedText = false;
	
	while (true)
	{
		sb.ClearStr();
		if (!reader->ReadLine(&sb))
		{
			errMessage->AppendC(UTF8STRC("Module end not found"));
			return false;
		}

		if (currObj && currObj->objectName && currObj->objectName->Equals(UTF8STRC("id-sha1")))
		{
			i = 0;
		}

		if (sb.GetLength() > 0)
		{
			if (sb.EndsWith(UTF8STRC("BEGIN")))
			{
				succ = ParseObjectBegin(reader, 0, errMessage);
				if (!succ)
				{
					return succ;
				}
				currObj = 0;
			}
			else if (sb.EndsWith(UTF8STRC("END")))
			{
				return true;
			}
			else if (isQuotedText)
			{
				if (currObj)
				{
					sbObjValCont.AppendC(sb.ToString(), sb.GetLength());
				}
				if (sb.EndsWith('"'))
				{
					isQuotedText = false;
					if (currObj)
					{
						if (sbObjValName.GetLength() > 0 && sbObjValCont.GetLength() > 0)
						{
							currObj->valName->Add(Text::String::New(sbObjValName.ToCString()));
							currObj->valCont->Add(Text::String::New(sbObjValCont.ToCString()));
						}
						sbObjValName.ClearStr();
						sbObjValCont.ClearStr();
					}
				}
				else
				{
					reader->GetLastLineBreak(&sbObjValCont);
				}
				
			}
			else
			{
				lineSpace = CalcLineSpace(sb.ToString());
				sb.Trim();
				if (currObj && (objBrkType != 0 || objIsEqual || (lineSpace > objLineSpace && sb.ToString()[0] >= 'A' && sb.ToString()[0] <= 'Z') || sb.StartsWith(UTF8STRC("::=")) || sb.StartsWith(UTF8STRC("{")) || sb.StartsWith(UTF8STRC("\""))))
				{
					if (objBrkType)
					{
						Char brkEndChar;
						if (objBrkType == '{')
						{
							brkEndChar = '}';
						}
						else if (objBrkType == '(')
						{
							brkEndChar = ')';
						}
						else
						{
							brkEndChar = '}';
						}

						if (objIsEqual)
						{
							Text::StringBuilderUTF8 sbTmp;
							sbTmp.Append(currObj->typeVal);
							sbTmp.AppendUTF8Char(' ');
							sbTmp.AppendC(sb.ToString(), sb.GetLength());
							currObj->typeVal->Release();
							currObj->typeVal = Text::String::New(sbTmp.ToString(), sbTmp.GetLength());

							OSInt brkEndIndex = BranketEnd(currObj->typeVal->v, (UTF8Char*)&brkEndChar);
							if (brkEndIndex >= 0)
							{
								objBrkType = 0;
								objIsEqual = false;
								if (sbObjValName.GetLength() > 0 && sbObjValCont.GetLength() > 0)
								{
									currObj->valName->Add(Text::String::New(sbObjValName.ToCString()));
									currObj->valCont->Add(Text::String::New(sbObjValCont.ToCString()));
								}
								sbObjValName.ClearStr();
								sbObjValCont.ClearStr();
								if (currObj->objectName->Equals(UTF8STRC(DEBUGOBJ)))
								{
									sbObjValCont.ClearStr();
								}

								if (currObj->typeName == 0 || currObj->typeName->v[0] == '{')
								{
									const UTF8Char *sptr = SkipWS(&currObj->typeVal->v[brkEndIndex]);
									if (sptr[0] == '(')
									{
										OSInt nextEndIndex = BranketEnd(&currObj->typeVal->v[brkEndIndex], 0);
										while (nextEndIndex < 0)
										{
											sb.ClearStr();
											sb.Append(currObj->typeVal);
											sb.AppendUTF8Char(' ');
											if (!reader->ReadLine(&sb))
											{
												errMessage->AppendC(UTF8STRC("Unexpected end of file: "));
												errMessage->AppendC(sb.ToString(), sb.GetLength());
												return false;
											}
											currObj->typeVal->Release();
											currObj->typeVal = Text::String::New(sb.ToString(), sb.GetLength());
											nextEndIndex = BranketEnd(&currObj->typeVal->v[brkEndIndex], 0);
										}
										RemoveSpace((UTF8Char*)currObj->typeVal);
									}
									else if (Text::StrStartsWith(sptr, (const UTF8Char*)"WITH SYNTAX") || Text::StrStartsWith(sptr, (const UTF8Char*)"WITH COMPONENTS"))
									{
										if (Text::StrIndexOfChar(sptr, '{') != INVALID_INDEX)
										{
											OSInt nextEndIndex = BranketEnd(&currObj->typeVal->v[brkEndIndex], 0);
											while (nextEndIndex < 0)
											{
												sb.ClearStr();
												sb.Append(currObj->typeVal);
												sb.AppendUTF8Char(' ');
												if (!reader->ReadLine(&sb))
												{
													errMessage->AppendC(UTF8STRC("Unexpected end of file: "));
													errMessage->AppendC(sb.ToString(), sb.GetLength());
													return false;
												}
												currObj->typeVal->Release();
												currObj->typeVal = Text::String::New(sb.ToString(), sb.GetLength());
												nextEndIndex = BranketEnd(&currObj->typeVal->v[brkEndIndex], 0);
											}
											RemoveSpace((UTF8Char*)currObj->typeVal);
										}
										else
										{
											sb.ClearStr();
											sb.Append(currObj->typeVal);
											sb.AppendUTF8Char(' ');
											if (!reader->ReadLine(&sb))
											{
												errMessage->AppendC(UTF8STRC("Unexpected end of file: "));
												errMessage->AppendC(sb.ToString(), sb.GetLength());
												return false;
											}
											currObj->typeVal->Release();
											currObj->typeVal = Text::String::New(sb.ToString(), sb.GetLength());
										}
									}
									else
									{
										
										RemoveSpace((UTF8Char*)currObj->typeVal);
										sb.ClearStr();
										if (reader->PeekWord(&sb))
										{
											RemoveSpace(sb.ToString());
											if (sb.Equals(UTF8STRC("WITH")))
											{
												sb.ClearStr();
												reader->NextWord(&sb);
												sb.AppendUTF8Char(' ');
												reader->NextWord(&sb);
												if (sb.Equals(UTF8STRC("WITH SYNTAX")) || sb.Equals(UTF8STRC("WITH COMPONENTS")))
												{
													sb.AppendUTF8Char(' ');
													if (!reader->NextWord(&sb))
													{
														errMessage->AppendC(UTF8STRC("WITH SYNTAX error: "));
														errMessage->AppendC(sb.ToString(), sb.GetLength());
														return false;
													}
													Text::StringBuilderUTF8 sbTmp;
													sbTmp.Append(currObj->typeVal);
													sbTmp.AppendUTF8Char(' ');
													sbTmp.AppendC(sb.ToString(), sb.GetLength());
													currObj->typeVal->Release();
													currObj->typeVal = Text::String::New(sbTmp.ToString(), sbTmp.GetLength());
													RemoveSpace((UTF8Char*)currObj->typeVal);
												}
												else
												{
													errMessage->AppendC(UTF8STRC("Unexpected word after WITH: "));
													errMessage->AppendC(sb.ToString(), sb.GetLength());
													return false;
												}
											}
											else
											{
												while (sb.StartsWith(UTF8STRC("(WITH")) ||
													sb.StartsWith(UTF8STRC("( WITH")) ||
													sb.StartsWith(UTF8STRC("(CONSTRAINED")) ||
													sb.StartsWith(UTF8STRC("(ALL")) ||
													sb.StartsWith(UTF8STRC("( ALL")))
												{
													sb.ClearStr();
													sb.Append(currObj->typeVal);
													sb.AppendUTF8Char(' ');
													reader->NextWord(&sb);
													currObj->typeVal->Release();
													currObj->typeVal = Text::String::New(sb.ToString(), sb.GetLength());
													RemoveSpace((UTF8Char*)currObj->typeVal);
													sb.ClearStr();
													reader->PeekWord(&sb);
													RemoveSpace(sb.ToString());
												}
											}
										}
									}
									currObj = 0;
								}
								else if (currObj->typeName->Equals(UTF8STRC("OBJECT IDENTIFIER")))
								{
									currObj = 0;
								}
							}
						}
						else
						{
							sbObjValCont.AppendC(sb.ToString(), sb.GetLength());
						}
					}
					else if (objIsEqual)
					{
						if (sb.StartsWith(UTF8STRC("[")) && sb.EndsWith(UTF8STRC("]")))
						{

						}
						else if (sb.Equals(UTF8STRC("OCTET")) || sb.Equals(UTF8STRC("BIT")))
						{
							sb.AppendUTF8Char(' ');
							reader->NextWord(&sb);
							if (sb.Equals(UTF8STRC("OCTET STRING")) || sb.Equals(UTF8STRC("BIT STRING")))
							{
								Text::StringBuilderUTF8 sbTmp;
								reader->PeekWord(&sbTmp);
								if (sbTmp.StartsWith(UTF8STRC("{")) || sbTmp.StartsWith(UTF8STRC("(")))
								{
									sb.AppendUTF8Char(' ');
									reader->NextWord(&sb);
									sbTmp.ClearStr();
									reader->PeekWord(&sbTmp);
								}
								currObj->typeVal = Text::String::New(sb.ToString(), sb.GetLength());
								currObj = 0;
								objIsEqual = false;
							}
							else
							{
								errMessage->AppendC(UTF8STRC("Unexpected words: "));
								errMessage->AppendC(sb.ToString(), sb.GetLength());
								return false;
							}
						}
						else
						{
							currObj->typeVal = Text::String::New(sb.ToString(), sb.GetLength());
							i = currObj->typeVal->IndexOf('{');
							if (i != INVALID_INDEX)
							{
								if (currObj->typeVal->CountChar('{') <= currObj->typeVal->CountChar('}'))
								{
									currObj = 0;
									objIsEqual = false;
								}
								else
								{
									objBrkType = '{';
								}
							}
							else if (currObj->typeVal->EndsWith(UTF8STRC(" OF")))
							{
								sb.ClearStr();
								sb.Append(currObj->typeVal);
								sb.AppendUTF8Char(' ');
								if (!reader->NextWord(&sb))
								{
									errMessage->AppendC(UTF8STRC("Unexpected end of file after OF"));
									return false;
								}

								Text::StringBuilderUTF8 sbTmp;
								reader->PeekWord(&sbTmp);
								if (sbTmp.StartsWith(UTF8STRC("{")))
								{
									reader->NextWord(&sb);
								}
								currObj->typeVal->Release();
								currObj->typeVal = Text::String::New(sb.ToString(), sb.GetLength());
								sb.ClearStr();
								currObj = 0;
							}
							else
							{
								sb.ClearStr();
								if (reader->PeekWord(&sb) && sb.StartsWith(UTF8STRC("{")))
								{
									Text::StringBuilderUTF8 sbTmp;
									sbTmp.Append(currObj->typeVal);
									sbTmp.AppendUTF8Char(' ');
									reader->NextWord(&sbTmp);
									currObj->typeVal->Release();
									currObj->typeVal = Text::String::New(sbTmp.ToString(), sbTmp.GetLength());
								}
								currObj = 0;
								objIsEqual = false;
							}
						}
					}
					else if (sb.StartsWith(UTF8STRC("::=")))
					{
						if (sbObjValName.GetLength() > 0 && sbObjValCont.GetLength() > 0)
						{
							currObj->valName->Add(Text::String::New(sbObjValName.ToCString()));
							currObj->valCont->Add(Text::String::New(sbObjValCont.ToCString()));
						}
						sbObjValName.ClearStr();
						sbObjValCont.ClearStr();

						i = 3;
						while (sb.ToString()[i] == ' ' || sb.ToString()[i] == '\t')
						{
							i++;
						}
						currObj->typeVal = Text::String::New(sb.ToString() + i, sb.GetLength() - i);
						UOSInt startCnt = currObj->typeVal->CountChar('{');
						UOSInt endCnt = currObj->typeVal->CountChar('}');
						if (endCnt >= startCnt)
						{
							currObj = 0;
							objBrkType = 0;
							objIsEqual = false;
						}							
						else
						{
							objBrkType = '{';
							objIsEqual = true;
						}
					}
					else if (sb.StartsWith(UTF8STRC("{")))
					{
						sbObjValCont.AppendC(sb.ToString(), sb.GetLength());
						if (sb.EndsWith(UTF8STRC("}")))
						{
							objBrkType = 0;
							objIsEqual = false;
							if (sbObjValName.GetLength() > 0 && sbObjValCont.GetLength() > 0)
							{
								currObj->valName->Add(Text::String::New(sbObjValName.ToCString()));
								currObj->valCont->Add(Text::String::New(sbObjValCont.ToCString()));
							}
							sbObjValName.ClearStr();
							sbObjValCont.ClearStr();
						}
						else
						{
							objBrkType = '{';
							objIsEqual = false;
						}
					}
					else if (sb.StartsWith(UTF8STRC("\"")))
					{
						sbObjValCont.AppendC(sb.ToString(), sb.GetLength());
						if (sb.GetLength() > 1 && sb.EndsWith(UTF8STRC("\"")))
						{
							if (sbObjValName.GetLength() > 0 && sbObjValCont.GetLength() > 0)
							{
								currObj->valName->Add(Text::String::New(sbObjValName.ToCString()));
								currObj->valCont->Add(Text::String::New(sbObjValCont.ToCString()));
							}
							sbObjValName.ClearStr();
							sbObjValCont.ClearStr();
						}
						else
						{
							isQuotedText = true;
						}
					}
					else
					{
						Bool proc = false;
						if (sbObjValName.GetLength() > 0 && sbObjValCont.GetLength() > 0)
						{
							if (sb.StartsWith(UTF8STRC("{")) || sb.StartsWith(UTF8STRC("\"")))
							{
								sbObjValCont.AppendUTF8Char(' ');
								sbObjValCont.AppendC(sb.ToString(), sb.GetLength());
								proc = true;
							}
							else
							{
								currObj->valName->Add(Text::String::New(sbObjValName.ToCString()));
								currObj->valCont->Add(Text::String::New(sbObjValCont.ToCString()));
								sbObjValName.ClearStr();
								sbObjValCont.ClearStr();
							}
						}
						else if (sbObjValName.GetLength() > 0)
						{
							sbObjValCont.AppendC(sb.ToString(), sb.GetLength());
							proc = true;
						}

						if (!proc)
						{
							i = sb.IndexOf(' ');
							j = sb.IndexOf('\t');
							if (i == INVALID_INDEX)
							{
								i = j;
							}
							else if (j != INVALID_INDEX && j < i)
							{
								i = j;
							}
							
							if (i == INVALID_INDEX)
							{
								sbObjValName.AppendC(sb.ToString(), sb.GetLength());
							}
							else
							{
								sbObjValName.AppendC(sb.ToString(), i);
								sbObjValCont.AppendC(sb.ToString() + i + 1, sb.GetLength() - i - 1);
								sbObjValCont.Trim();
							}
							
						}

						if ((i = sb.IndexOf('\"')) != INVALID_INDEX)
						{
							j = sb.IndexOf(UTF8STRC("\""), i + 1);
							if (j == INVALID_INDEX)
							{
								reader->GetLastLineBreak(&sbObjValCont);
								isQuotedText = true;
							}
						}
						else if ((i = sb.IndexOf(UTF8STRC("{"))) != INVALID_INDEX)
						{
							j = sb.IndexOf(UTF8STRC("}"));
							if (j != INVALID_INDEX && j > i)
							{
								if (sbObjValName.GetLength() > 0 && sbObjValCont.GetLength() > 0)
								{
									currObj->valName->Add(Text::String::New(sbObjValName.ToCString()));
									currObj->valCont->Add(Text::String::New(sbObjValCont.ToCString()));
								}
								sbObjValName.ClearStr();
								sbObjValCont.ClearStr();
							}
							else
							{
								objBrkType = '{';
								objIsEqual = false;
							}
						}
						else if ((i = sb.IndexOf(UTF8STRC("("))) != INVALID_INDEX)
						{
							j = sb.IndexOf(UTF8STRC(")"), (UOSInt)i);
							if (j != INVALID_INDEX && j > i)
							{
								if (sbObjValName.GetLength() > 0 && sbObjValCont.GetLength() > 0)
								{
									currObj->valName->Add(Text::String::New(sbObjValName.ToCString()));
									currObj->valCont->Add(Text::String::New(sbObjValCont.ToCString()));
								}
								sbObjValName.ClearStr();
								sbObjValCont.ClearStr();
							}
							else
							{
								objBrkType = '(';
								objIsEqual = false;
							}
						}
					}
				}
				else if (sb.StartsWith(UTF8STRC("IMPORTS")))
				{
					Bool isEnd = false;
					Text::StringBuilderUTF8 impObjNames;
					Net::ASN1MIB::ModuleInfo *impModule;
					Net::ASN1MIB::ObjectInfo *impObj;
					UTF8Char *impSarr[2];
					UOSInt impCnt;
					sb.SetSubstr(7);
					sb.Trim();

					while (!isEnd)
					{
						if (sb.EndsWith(';'))
						{
							isEnd = true;
							sb.RemoveChars(1);
						}
						i = sb.IndexOf(UTF8STRC("FROM "));
						if (i != INVALID_INDEX)
						{
							impObjNames.AppendC(sb.ToString(), (UOSInt)i);
							impObjNames.RTrim();

							sb.SetSubstr((UOSInt)i + 5);
							sb.Trim();
							if ((i = sb.IndexOf('{')) != INVALID_INDEX)
							{
								while (true)
								{
									j = sb.IndexOf('}');
									if (j != INVALID_INDEX)
									{
										break;
									}
									if (!reader->ReadLine(&sb))
									{
										errMessage->AppendC(UTF8STRC("Import module error: "));
										errMessage->AppendC(sb.ToString(), sb.GetLength());
										return false;
									}
								}
								if (sb.EndsWith(';'))
								{
									isEnd = true;
									sb.RemoveChars(1);
								}
								sb.TrimToLength((UOSInt)i);
								sb.Trim();
							}
							else if ((i = sb.IndexOf(' ')) != INVALID_INDEX)
							{
								if (sb.EndsWith(';'))
								{
									isEnd = true;
									sb.RemoveChars(1);
								}
								sb.TrimToLength((UOSInt)i);
							}

							if ((impModule = this->moduleMap->GetC(sb.ToCString())) != 0)
							{
								
							}
							else
							{
								UTF8Char sbuff[512];
								UTF8Char *sptr;
								sptr = module->moduleFileName->ConcatTo(sbuff);
								j = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), IO::Path::PATH_SEPERATOR);
								sptr = Text::StrConcatC(&sbuff[j + 1], sb.ToString(), sb.GetLength());
								succ = LoadFileInner(CSTRP(sbuff, sptr), errMessage, false);
								if (!succ)
								{
									return false;
								}
								impModule = this->moduleMap->GetC(sb.ToCString());
							}
							
							if (impModule == 0)
							{
								errMessage->AppendC(UTF8STRC("IMPORTS module "));
								errMessage->AppendC(sb.ToString(), sb.GetLength());
								errMessage->AppendC(UTF8STRC(" not found"));
								return false;
							}
							impSarr[1] = impObjNames.ToString();
							while (true)
							{
								UOSInt ui;
								impCnt = Text::StrSplitTrim(impSarr, 2, impSarr[1], ',');

								i = Text::StrIndexOfChar(impSarr[0], '{');
								if (i != INVALID_INDEX && Text::StrEndsWith(impSarr[0], (const UTF8Char*)"}"))
								{
									impSarr[0][i] = 0;
									while (i > 0 && Text::CharUtil::IsWS(&impSarr[0][i - 1]))
									{
										impSarr[0][--i] = 0;
									}
								}
								impObj = MemAlloc(ObjectInfo, 1);
								impObj->objectName = Text::String::NewNotNull(impSarr[0]);
								impObj->typeName = Text::String::New(sb.ToString(), sb.GetLength());
								impObj->typeVal = Text::String::New(UTF8STRC("Imported Value"));
								impObj->oidLen = 0;
								NEW_CLASS(impObj->valName, Data::ArrayList<Text::String*>());
								NEW_CLASS(impObj->valCont, Data::ArrayList<Text::String*>());
								impObj->impModule = impModule;
								impObj->parsed = false;
								ui = module->objKeys->SortedInsert(impObj->objectName);
								module->objValues->Insert(ui, impObj);

//								OSInt impInd;
//								impInd = impModule->objKeys->SortedIndexOf(impSarr[0]);
/*								if (impInd >= 0)
								{
									impObj = impModule->objValues->GetItem((UOSInt)impInd);
									impObj2 = MemAlloc(ObjectInfo, 1);
									impObj2->objectName = Text::StrCopyNew(impSarr[0]);
									if (impObj->typeName)
									{
										impObj2->typeName = Text::StrCopyNew(impObj->typeName);
									}
									else
									{
										impObj2->typeName = 0;
									}
									impObj2->typeVal = Text::StrCopyNew((const UTF8Char*)"Imported Value");
									impObj2->oidLen = impObj->oidLen;
									if (impObj->oidLen > 0)
									{
										MemCopyNO(impObj2->oid, impObj->oid, impObj->oidLen);
									}
									NEW_CLASS(impObj2->valName, Data::ArrayList<const UTF8Char*>());
									NEW_CLASS(impObj2->valCont, Data::ArrayList<const UTF8Char*>());
									impObj2->impModule = impModule;
									impObj2->parsed = false;
									UOSInt ui = 0;
									UOSInt uj = impObj->valName->GetCount();
									while (ui < uj)
									{
										impObj2->valName->Add(Text::StrCopyNew(impObj->valName->GetItem(ui)));
										impObj2->valCont->Add(Text::StrCopyNew(impObj->valCont->GetItem(ui)));
										ui++;
									}
									
									ui = module->objKeys->SortedInsert(impObj2->objectName);
									module->objValues->Insert(ui, impObj2);
								}
								else
								{
									errMessage->AppendC(UTF8STRC("IMPORTS object ");
									errMessage->Append(impSarr[0]);
									errMessage->AppendC(UTF8STRC(" in module ");
									errMessage->AppendC(sb.ToString(), sb.GetLength());
									errMessage->AppendC(UTF8STRC(" not found");
									return false;
								}*/
								if (impCnt != 2)
								{
									break;
								}
							}
							impObjNames.ClearStr();
						}
						else
						{
							impObjNames.AppendC(sb.ToString(), sb.GetLength());
						}
						if (isEnd)
						{
							break;
						}
						sb.ClearStr();
						if (!reader->ReadLine(&sb))
						{
							errMessage->AppendC(UTF8STRC("IMPORTS end not found"));
							return false;
						}
						sb.Trim();
					}
				}
				else if (sb.StartsWith(UTF8STRC("EXPORTS")))
				{
					while (true)
					{
						if (sb.EndsWith(';'))
						{
							break;
						}

						sb.ClearStr();
						if (!reader->ReadLine(&sb))
						{
							errMessage->AppendC(UTF8STRC("EXPORTS end not found"));
							return false;
						}
						sb.Trim();
					}
				}
				else
				{
					if (currObj)
					{
						if (sbObjValName.GetLength() > 0 && sbObjValCont.GetLength() > 0)
						{
							currObj->valName->Add(Text::String::New(sbObjValName.ToCString()));
							currObj->valCont->Add(Text::String::New(sbObjValCont.ToCString()));
						}
						sbObjValName.ClearStr();
						sbObjValCont.ClearStr();
					}
					currObj = 0;
					i = sb.IndexOf(UTF8STRC("::="));
					if (i == 0)
					{
						errMessage->AppendC(UTF8STRC("::= found at non object location"));
						return false;
					}
					if (i == INVALID_INDEX)
					{
						i = sb.IndexOf(' ');
						j = sb.IndexOf('\t');
						if (i == INVALID_INDEX)
						{
							i = j;
						}
						else if (j != INVALID_INDEX && j < i)
						{
							i = j;
						}
						if (i != INVALID_INDEX)
						{
							i = INVALID_INDEX;
						}
						else
						{
							i = sb.GetLength();
							if (!reader->ReadLine(&sb))
							{
								errMessage->AppendC(UTF8STRC("Unknown format 1: "));
								errMessage->AppendC(sb.ToString(), sb.GetLength());
								return false;
							}
							if (sb.ToString()[i] == ' ' || sb.ToString()[i] == '\t')
							{
								i = sb.IndexOf(UTF8STRC("::="));
							}
							else
							{
								errMessage->AppendC(UTF8STRC("Unknown format 2: "));
								errMessage->AppendC(sb.ToString(), sb.GetLength());
								return false;
							}
						}
					}
					if (i != INVALID_INDEX)
					{
						j = i;
						UOSInt k;
						while (sb.ToString()[j - 1] == ' ' || sb.ToString()[j - 1] == '\t')
						{
							j--;
						}
						k = sb.IndexOf(' ');
						UOSInt l = sb.IndexOf('\t');
						if (k == INVALID_INDEX)
						{
							k = l;
						}
						else if (l != INVALID_INDEX && l < k)
						{
							k = l;
						}
						l = sb.IndexOf('{');
						if (k == INVALID_INDEX || (l != INVALID_INDEX && l < k))
						{
							k = l;
						}
						obj = MemAlloc(ObjectInfo, 1);
						obj->objectName = Text::String::New(sb.ToString(), k);
						if (j > k)
						{
							while (sb.ToString()[k] == ' ' || sb.ToString()[k] == '\t')
							{
								k++;
							}
							obj->typeName = Text::String::New(sb.ToString() + k, (UOSInt)(j - k));
						}
						else
						{
							obj->typeName = 0;
						}
						obj->typeVal = 0;
						obj->oidLen = 0;
						NEW_CLASS(obj->valName, Data::ArrayList<Text::String*>());
						NEW_CLASS(obj->valCont, Data::ArrayList<Text::String*>());
						obj->impModule = 0;
						obj->parsed = false;
						UOSInt ui = module->objKeys->SortedInsert(obj->objectName);
						module->objValues->Insert(ui, obj);
						ui = this->globalModule.objKeys->SortedInsert(obj->objectName);
						this->globalModule.objValues->Insert(ui, obj);
						if (obj->objectName->Equals(UTF8STRC(DEBUGOBJ)))
						{
							currObj = 0;
						}

						if (sb.EndsWith(UTF8STRC("::=")))
						{
							currObj = obj;
							objLineSpace = lineSpace;
							objIsEqual = true;
							sbObjValName.ClearStr();
							sbObjValCont.ClearStr();
						}
						else
						{
							i += 3;
							while (sb.ToString()[i] == ' ' || sb.ToString()[i] == '\t')
							{
								i++;
							}
							obj->typeVal = Text::String::New(sb.ToString() + i, sb.GetLength() - i);
							currObj = obj;
							sbObjValName.ClearStr();
							sbObjValCont.ClearStr();
							objLineSpace = lineSpace;
							objIsEqual = false;
							objBrkType = 0;

							if (obj->typeVal->IndexOf('{') != INVALID_INDEX)
							{
								UOSInt startCnt = obj->typeVal->CountChar('{');
								UOSInt endCnt = obj->typeVal->CountChar('}');
								if (endCnt >= startCnt)
								{
								}
								else
								{
									objBrkType = '{';
									objIsEqual = true;
								}
							}
							else if ((i = obj->typeVal->CountChar('(')) != INVALID_INDEX)
							{
								UOSInt startCnt = obj->typeVal->CountChar('(');
								UOSInt endCnt = obj->typeVal->CountChar(')');
								if (endCnt >= startCnt)
								{
								}
								else
								{
									objBrkType = '(';
									objIsEqual = true;
								}
							}

							if (!objIsEqual)
							{
								if (obj->typeVal->EndsWith(UTF8STRC("SIZE")))
								{
									sb.ClearStr();
									sb.Append(obj->typeVal);
									sbObjValCont.ClearStr();
									reader->NextWord(&sbObjValCont);
									if (sbObjValCont.ToString()[0] != '(')
									{
										errMessage->AppendC(UTF8STRC("Unexpected SIZE format: "));
										errMessage->AppendC(sbObjValCont.ToString(), sbObjValCont.GetLength());
										return false;
									}
									sb.AppendC(sbObjValCont.ToString(), sbObjValCont.GetLength());

									sbObjValCont.ClearStr();
									reader->PeekWord(&sbObjValCont);
									if (sbObjValCont.Equals(UTF8STRC("OF")))
									{
										sb.AppendUTF8Char(' ');
										reader->NextWord(&sb);
										sb.AppendUTF8Char(' ');
										reader->NextWord(&sb);
									}
									sbObjValCont.ClearStr();
									obj->typeVal->Release();
									obj->typeVal = Text::String::New(sb.ToString(), sb.GetLength());
								}
								else if (obj->typeVal->EndsWith(UTF8STRC("TEXTUAL-CONVENTION")))
								{
									while (true)
									{
										sbObjValName.ClearStr();
										reader->PeekWord(&sbObjValName);
										if (IsUnknownType(sbObjValName.ToString()))
										{
											sbObjValName.ClearStr();
											reader->NextWord(&sbObjValName);
											sbObjValCont.ClearStr();
											reader->NextWord(&sbObjValCont);
											sb.ClearStr();
											reader->PeekWord(&sb);
											if (sb.StartsWith(UTF8STRC("{")) || sb.StartsWith(UTF8STRC("(")))
											{
												reader->NextWord(&sbObjValCont);
											}
											obj->valName->Add(Text::String::New(sbObjValName.ToCString()));
											obj->valCont->Add(Text::String::New(sbObjValCont.ToCString()));
											sbObjValCont.ClearStr();
										}
										else
										{
											sbObjValName.ClearStr();
											break;
										}
									}
								}
								else if (obj->typeVal->EndsWith(UTF8STRC(" OF")))
								{
									sb.ClearStr();
									sb.Append(obj->typeVal);
									sb.AppendUTF8Char(' ');
									reader->NextWord(&sb);

									Text::StringBuilderUTF8 sbTmp;
									reader->PeekWord(&sbTmp);
									if (sbTmp.StartsWith(UTF8STRC("{")))
									{
										reader->NextWord(&sb);
									}
									obj->typeVal->Release();
									obj->typeVal = Text::String::New(sb.ToString(), sb.GetLength());
								}
								else
								{
									sbObjValCont.ClearStr();
									reader->PeekWord(&sbObjValCont);
									if (sbObjValCont.Equals(UTF8STRC("OF")))
									{
										sb.ClearStr();
										sb.Append(obj->typeVal);
										sb.AppendUTF8Char(' ');
										reader->NextWord(&sb);
										sb.AppendUTF8Char(' ');
										reader->NextWord(&sb);
										obj->typeVal->Release();
										obj->typeVal = Text::String::New(sb.ToString(), sb.GetLength());
									}
									else if (sbObjValCont.StartsWith(UTF8STRC("(")))
									{
										sb.ClearStr();
										sb.Append(obj->typeVal);
										sb.AppendUTF8Char(' ');
										reader->NextWord(&sb);

										sbObjValCont.ClearStr();
										reader->PeekWord(&sbObjValCont);
										if (sbObjValCont.Equals(UTF8STRC("OF")))
										{
											sb.AppendUTF8Char(' ');
											reader->NextWord(&sb);
											sb.AppendUTF8Char(' ');
											reader->NextWord(&sb);
										}
										obj->typeVal->Release();
										obj->typeVal = Text::String::New(sb.ToString(), sb.GetLength());
									}
									sbObjValCont.ClearStr();
								}
							}
						}
					}
					else
					{
						i = sb.IndexOf(' ');
						j = sb.IndexOf('\t');
						if (i == INVALID_INDEX)
						{
							i = j;
						}
						else if (j != INVALID_INDEX && j < i)
						{
							i = j;
						}
						if (i == INVALID_INDEX)
						{
							i = sb.GetLength();
							if (!reader->ReadLine(&sb))
							{
								errMessage->AppendC(UTF8STRC("Unknown format 3: "));
								errMessage->AppendC(sb.ToString(), sb.GetLength());
								return false;
							}
							if (sb.ToString()[i] == ' ' || sb.ToString()[i] == '\t')
							{
							}
							else
							{
								errMessage->AppendC(UTF8STRC("Unknown format 4: "));
								errMessage->AppendC(sb.ToString(), sb.GetLength());
								return false;
							}
						}

						obj = MemAlloc(ObjectInfo, 1);
						obj->objectName = Text::String::New(sb.ToString(), (UOSInt)i);
						while (sb.ToString()[i] == ' ' || sb.ToString()[i] == '\t')
						{
							i++;
						}
						obj->typeName = Text::String::New(sb.ToString() + i, sb.GetLength() - i);
						obj->typeVal = 0;
						obj->oidLen = 0;
						NEW_CLASS(obj->valName, Data::ArrayList<Text::String*>());
						NEW_CLASS(obj->valCont, Data::ArrayList<Text::String*>());
						obj->impModule = 0;
						obj->parsed = false;
						UOSInt ui = module->objKeys->SortedInsert(obj->objectName);
						module->objValues->Insert(ui, obj);
						ui = this->globalModule.objKeys->SortedInsert(obj->objectName);
						this->globalModule.objValues->Insert(ui, obj);
						if (obj->objectName->Equals(UTF8STRC(DEBUGOBJ)))
						{
							currObj = obj;
						}

						while (true)
						{
							sbObjValName.ClearStr();
							reader->PeekWord(&sbObjValName);
							if (IsUnknownType(sbObjValName.ToString()))
							{
								sbObjValName.ClearStr();
								reader->NextWord(&sbObjValName);
								sbObjValCont.ClearStr();
								reader->NextWord(&sbObjValCont);
								sb.ClearStr();
								reader->PeekWord(&sb);
								if (sb.StartsWith(UTF8STRC("{")) || sb.StartsWith(UTF8STRC("(")))
								{
									reader->NextWord(&sbObjValCont);
								}
								obj->valName->Add(Text::String::New(sbObjValName.ToCString()));
								obj->valCont->Add(Text::String::New(sbObjValCont.ToCString()));
								sbObjValCont.ClearStr();
							}
							else if (sbObjValCont.Equals(UTF8STRC("::=")))
							{
								reader->NextWord(&sbObjValCont);
								currObj = obj;
								objLineSpace = lineSpace;
								objIsEqual = true;
								sbObjValName.ClearStr();
								sbObjValCont.ClearStr();
								break;
							}
							else
							{
								currObj = obj;
								objLineSpace = lineSpace;
								objIsEqual = false;
								sbObjValName.ClearStr();
								sbObjValCont.ClearStr();
								break;
							}
						}						
					}
				}
			}
		}
	}
}

Bool Net::ASN1MIB::ApplyModuleOID(ModuleInfo *module, ObjectInfo *obj, Text::StringBuilderUTF8 *errMessage)
{
	Bool valid = false;
	if (obj->parsed)
	{
		return true;
	}
	if (obj->impModule != 0)
	{
		OSInt i = obj->impModule->objKeys->SortedIndexOf(obj->objectName);
		if (i < 0)
		{
			errMessage->AppendC(UTF8STRC("IMPORTS object "));
			errMessage->Append(obj->objectName);
			errMessage->AppendC(UTF8STRC(" in module "));
			errMessage->Append(obj->typeName);
			errMessage->AppendC(UTF8STRC(" not found"));
			return false;
		}
		ObjectInfo *impObj = obj->impModule->objValues->GetItem((UOSInt)i);
		if (!impObj->parsed)
		{
			if (!ApplyModuleOID(obj->impModule, impObj, errMessage))
			{
				return false;
			}
		}

		SDEL_STRING(obj->typeName);
		if (impObj->typeName)
		{
			obj->typeName = impObj->typeName->Clone();
		}
		SDEL_STRING(obj->typeVal);
		obj->typeVal = SCOPY_STRING(impObj->typeVal);
		obj->oidLen = impObj->oidLen;
		if (impObj->oidLen > 0)
		{
			MemCopyNO(obj->oid, impObj->oid, impObj->oidLen);
		}

		UOSInt uk = 0;
		UOSInt ul = impObj->valName->GetCount();
		while (uk < ul)
		{
			obj->valName->Add(impObj->valName->GetItem(uk)->Clone());
			obj->valCont->Add(impObj->valCont->GetItem(uk)->Clone());
			uk++;
		}
	}

	if (obj->typeName && obj->typeVal && obj->oidLen == 0)
	{
		valid = true;
		if (obj->typeName->Equals(UTF8STRC("TRAP-TYPE"))) valid = false;
		if (obj->typeName->Equals(UTF8STRC("ATTRIBUTE"))) valid = false;
		if (obj->typeName->Equals(UTF8STRC("INTEGER"))) valid = false;
		if (obj->typeName->Equals(UTF8STRC("NULL"))) valid = false;
		if (obj->typeName->StartsWith(UTF8STRC("OCTET STRING"))) valid = false;

		if (obj->typeVal->Equals(UTF8STRC("Imported Value"))) valid = false;
		if (obj->typeVal->IndexOf(',') != INVALID_INDEX) valid = false;
		if (obj->typeVal->IndexOf(UTF8STRC("...")) != INVALID_INDEX) valid = false;
	}
	obj->parsed = true;
	if (valid)
	{
		if (obj->typeName->Equals(UTF8STRC("OBJECT IDENTIFIER")))
		{
			if (!this->ParseObjectOID(module, obj, obj->typeVal, errMessage))
			{
				return false;
			}
		}
		else
		{
			Text::StringBuilderUTF8 sb;
			if (!this->ParseObjectOID(module, obj, obj->typeVal, &sb))
			{
				obj->oidLen = 0;
			}
		}
		ModuleAppendOID(module, obj);
		ModuleAppendOID(&this->globalModule, obj);
	}
	else if (obj->oidLen > 0)
	{
		ModuleAppendOID(module, obj);
		ModuleAppendOID(&this->globalModule, obj);
	}
	return true;
}

Bool Net::ASN1MIB::ApplyModuleOIDs(ModuleInfo *module, Text::StringBuilderUTF8 *errMessage)
{
	Data::ArrayList<ObjectInfo*> *objList = module->objValues;
	ObjectInfo *obj;
	UOSInt ui = 0;
	UOSInt uj = objList->GetCount();
	while (ui < uj)
	{
		obj = objList->GetItem(ui);
		if (!ApplyModuleOID(module, obj, errMessage))
		{
			return false;
		}
		ui++;
	}
	return true;
}

Bool Net::ASN1MIB::ApplyOIDs(Text::StringBuilderUTF8 *errMessage)
{
	UOSInt i = this->moduleMap->GetCount();
	while (i-- > 0)
	{
		if (!ApplyModuleOIDs(this->moduleMap->GetItem(i), errMessage))
		{
			return false;
		}
	}
	return true;
}

/*Bool Net::ASN1MIB::ApplyModuleImports(ModuleInfo *module, Text::StringBuilderUTF *errMessage)
{
	Data::ArrayList<ObjectInfo*> *objList = module->objValues;
	ObjectInfo *obj;
	UOSInt ui = 0;
	UOSInt uj = objList->GetCount();
	while (ui < uj)
	{
		obj = objList->GetItem(ui);
		if (obj->typeName && obj->typeVal && Text::StrEquals(obj->typeVal, (const UTF8Char*)"Imported Value"))
		{
			ModuleInfo *impModule = this->moduleMap->Get(obj->typeName);
			ObjectInfo *impObj;
			OSInt impInd;
			impInd = impModule->objKeys->SortedIndexOf(obj->objectName);
			if (impInd >= 0)
			{
				impObj = impModule->objValues->GetItem((UOSInt)impInd);
				if (!impObj->parsed)
				{
					if (!ApplyModuleOID(impModule, impObj, errMessage))
					{
						return false;
					}
				}
				SDEL_TEXT(obj->typeName);
				if (impObj->typeName)
				{
					obj->typeName = Text::StrCopyNew(impObj->typeName);
				}
				SDEL_TEXT(obj->typeVal);
				obj->typeVal = SCOPY_TEXT(impObj->typeVal);
				obj->oidLen = impObj->oidLen;
				if (impObj->oidLen > 0)
				{
					MemCopyNO(obj->oid, impObj->oid, impObj->oidLen);
				}

				UOSInt uk = 0;
				UOSInt ul = impObj->valName->GetCount();
				while (uk < ul)
				{
					obj->valName->Add(Text::StrCopyNew(impObj->valName->GetItem(uk)));
					obj->valCont->Add(Text::StrCopyNew(impObj->valCont->GetItem(uk)));
					uk++;
				}
			}
			else
			{
				errMessage->AppendC(UTF8STRC("IMPORTS object ");
				errMessage->Append(obj->objectName);
				errMessage->AppendC(UTF8STRC(" in module ");
				errMessage->Append(obj->typeName);
				errMessage->AppendC(UTF8STRC(" not found");
				return false;
			}
		}
		ui++;
	}
	return true;
}*/

Bool Net::ASN1MIB::ApplyImports(Text::StringBuilderUTF8 *errMessage)
{
/*	Data::ArrayList<ModuleInfo*> *moduleList = this->moduleMap->GetValues();
	UOSInt i = moduleList->GetCount();
	while (i-- > 0)
	{
		if (!ApplyModuleImports(moduleList->GetItem(i), errMessage))
		{
			return false;
		}
	}*/
	return true;
}

Bool Net::ASN1MIB::LoadFileInner(Text::CString fileName, Text::StringBuilderUTF8 *errMessage, Bool postApply)
{
	Text::StringBuilderUTF8 sbFileName;
	IO::FileStream *fs;
	Net::MIBReader *reader;
	ModuleInfo *module;
	Bool succ;
	if (IO::Path::GetPathType(fileName.v, fileName.leng) != IO::Path::PathType::File)
	{
		sbFileName.ClearStr();
		sbFileName.Append(fileName);
		sbFileName.AppendC(UTF8STRC(".asn"));
		if (IO::Path::GetPathType(sbFileName.ToString(), sbFileName.GetLength()) == IO::Path::PathType::File)
		{
			fileName.v = sbFileName.ToString();
			fileName.leng = sbFileName.GetLength();
		}
		else
		{
			sbFileName.ClearStr();
			sbFileName.Append(fileName);
			sbFileName.AppendC(UTF8STRC(".mib"));
			fileName.v = sbFileName.ToString();
			fileName.leng = sbFileName.GetLength();
		}
	}
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		errMessage->AppendC(UTF8STRC("Error in opening file "));
		errMessage->Append(fileName);
		return false;
	}
	succ = false;
	NEW_CLASS(reader, Net::MIBReader(fs));
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbModuleName;
	Text::StringBuilderUTF8 sbOID;
	sb.ClearStr();
	if (!reader->NextWord(&sbModuleName))
	{
		errMessage->AppendC(UTF8STRC("Module definition not found"));
	}
	else if (!Text::CharUtil::IsAlphaNumeric(sbModuleName.ToString()[0]))
	{
		errMessage->AppendC(UTF8STRC("Module name not found"));
	}
	else if (!reader->NextWord(&sbOID))
	{
		errMessage->AppendC(UTF8STRC("Invalid file format: Unexpected end of file 1"));
	}
	else
	{
		succ = true;
		if (sbOID.ToString()[0] == '{')
		{
			if (!reader->NextWord(&sb))
			{
				errMessage->AppendC(UTF8STRC("Invalid file format: Unexpected end of file 2"));
				succ = false;
			}
		}
		else
		{
			sb.AppendC(sbOID.ToString(), sbOID.GetLength());
			sbOID.ClearStr();
		}
		if (succ && !sb.Equals(UTF8STRC("DEFINITIONS")))
		{
			errMessage->AppendC(UTF8STRC("Invalid file format: Expected DEFINITIONS: "));
			errMessage->AppendC(sb.ToString(), sb.GetLength());
			succ = false;
		}
		sb.ClearStr();
		if (succ && !reader->NextWord(&sb))
		{
			errMessage->AppendC(UTF8STRC("Invalid file format: Unexpected end of file 3"));
			succ = false;
		}
		if (succ && (sb.Equals(UTF8STRC("IMPLICIT")) || sb.Equals(UTF8STRC("EXPLICIT"))))
		{
			sb.ClearStr();
			if (!reader->NextWord(&sb))
			{
				errMessage->AppendC(UTF8STRC("Invalid file format: Unexpected end of file 4"));
				succ = false;
			}
			else if (!sb.Equals(UTF8STRC("TAGS")))
			{
				errMessage->AppendC(UTF8STRC("Invalid file format: Expected TAGS: "));
				errMessage->AppendC(sb.ToString(), sb.GetLength());
				succ = false;
			}
			else
			{
				sb.ClearStr();
				if (!reader->NextWord(&sb))
				{
					errMessage->AppendC(UTF8STRC("Invalid file format: Unexpected end of file 5"));
					succ = false;
				}
			}
		}
		if (succ && !sb.Equals(UTF8STRC("::=")))
		{
			errMessage->AppendC(UTF8STRC("Invalid file format: Expected ::= : "));
			errMessage->AppendC(sb.ToString(), sb.GetLength());
			succ = false;
		}
		if (succ)
		{
			sb.ClearStr();
			if (!reader->NextWord(&sb))
			{
				errMessage->AppendC(UTF8STRC("Invalid file format: Unexpected end of file 6"));
				succ = false;
			}
			else if (!sb.Equals(UTF8STRC("BEGIN")))
			{
				errMessage->AppendC(UTF8STRC("Invalid file format: Expected BEGIN: "));
				errMessage->AppendC(sb.ToString(), sb.GetLength());
				succ = false;
			}
		}
	}

	if (succ)
	{
		module = MemAlloc(ModuleInfo, 1);
		module->moduleName = Text::String::New(sbModuleName.ToString(), sbModuleName.GetLength());
		module->moduleFileName = Text::String::New(fileName.v, fileName.leng);
		NEW_CLASS(module->objKeys, Data::ArrayListString());
		NEW_CLASS(module->objValues, Data::ArrayList<ObjectInfo*>());
		NEW_CLASS(module->oidList, Data::ArrayList<ObjectInfo*>());
		this->moduleMap->Put(module->moduleName, module);
		if ((succ = ParseModule(reader, module, errMessage)) == true)
		{
			if (postApply)
			{
				succ = ApplyImports(errMessage) && ApplyModuleOIDs(module, errMessage);
			}
		}
	}

/*	
	Bool moduleFound = false;
	OSInt i;
	while (true)
	{
		sb.ClearStr();
		if (!reader->ReadLine(&sb))
		{
			if (!moduleFound)
			{
				errMessage->AppendC(UTF8STRC("Module definition not found"));
			}
			break;
		}
		sb.Trim();
		if (sb.GetLength() > 0)
		{
			if (moduleFound)
			{
				errMessage->AppendC(UTF8STRC("Object found after Module definition"));
				succ = false;
				break;
			}
			i = sb.IndexOf(UTF8STRC(" DEFINITIONS ::= BEGIN"));
			if (i == INVALID_INDEX)
			{
				succ = false;
				errMessage->AppendC(UTF8STRC("Wrong Module definition format"));
				break;
			}
			sb.ToString()[i] = 0;
			if (this->moduleMap->Get(sb.ToString()))
			{
				errMessage->AppendC(UTF8STRC("Module "));
				errMessage->AppendC(sb.ToString(), sb.GetLength());
				errMessage->AppendC(UTF8STRC(" already loaded"));
				break;
			}
			module = MemAlloc(ModuleInfo, 1);
			module->moduleName = Text::StrCopyNew(sb.ToString());
			module->moduleFileName = Text::StrCopyNew(fileName);
			NEW_CLASS(module->objKeys, Data::ArrayListStrUTF8());
			NEW_CLASS(module->objValues, Data::ArrayList<ObjectInfo*>());
			NEW_CLASS(module->oidList, Data::ArrayList<ObjectInfo*>());
			this->moduleMap->Put(module->moduleName, module);
			succ = ParseModule(reader, module, errMessage);
			moduleFound = true;
			if (!succ)
			{
				break;
			}
		}
	}*/
	DEL_CLASS(reader);
	DEL_CLASS(fs);
	return succ;
}

void Net::ASN1MIB::RemoveSpace(UTF8Char *s)
{
	if (s)
	{
		UOSInt strLen = Text::StrCharCnt(s);
		UOSInt wsCnt = 0;
		while (strLen-- > 0)
		{
			if (s[strLen] == '\r' || s[strLen] == '\n' || s[strLen] == '\t' || s[strLen] == ' ')
			{
				s[strLen] = ' ';
				wsCnt++;
			}
			else if (wsCnt > 0)
			{
				if (wsCnt > 1)
				{
					Text::StrLTrim(&s[strLen + 2]);
				}
				wsCnt = 0;
			}
		}
	}
}

Bool Net::ASN1MIB::IsType(const UTF8Char *s)
{
	UTF8Char c;
	if (s[0] == 0)
	{
		return false;
	}
	while ((c = *s++) != 0)
	{
		if (c != ' ' && c != '-' && (c < 'A' || c > 'Z'))
		{
			return false;
		}
	}
	return true;
}

Bool Net::ASN1MIB::IsKnownType(const UTF8Char *s)
{
	if (Text::StrEquals(s, (const UTF8Char*)"OCTET STRING") ||
		Text::StrEquals(s, (const UTF8Char*)"END") ||
		Text::StrEquals(s, (const UTF8Char*)"INTEGER") ||
		Text::StrEquals(s, (const UTF8Char*)"OBJECT IDENTIFIER"))
	{
		return true;
	}
	return false;
}

Bool Net::ASN1MIB::IsUnknownType(const UTF8Char *s)
{
	return IsType(s) && !IsKnownType(s);
}

OSInt Net::ASN1MIB::BranketEnd(const UTF8Char *s, UTF8Char *brkType)
{
	OSInt i = 0;
	UTF8Char c;
	UTF8Char brkStart;
	UTF8Char brkEnd;
	UOSInt level;
	while (true)
	{
		c = s[i++];
		if (c == 0)
		{
			return -1;
		}
		if (c == '{')
		{
			brkStart = '{';
			brkEnd = '}';
			break;
		}
		else if (c == '(')
		{
			brkStart = '(';
			brkEnd = ')';
			break;
		}
	}
	level = 1;
	while ((c = s[i++]) != 0)
	{
		if (c == brkStart)
		{
			level++;
		}
		else if (c == brkEnd)
		{
			level--;
			if (level == 0)
			{
				if (brkType)
				{
					*brkType = brkStart;
				}
				return i;
			}
		}
	}
	return -1;
}

const UTF8Char *Net::ASN1MIB::SkipWS(const UTF8Char *s)
{
	UTF8Char c;
	while (true)
	{
		c = *s;
		if (c == 0)
		{
			return s;
		}
		else if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
		{
			return s;
		}
		s++;
	}
}

UTF8Char Net::ASN1MIB::NextChar(const UTF8Char *s)
{
	return SkipWS(s)[0];
}

Net::ASN1MIB::ASN1MIB()
{
	NEW_CLASS(this->moduleMap, Data::FastStringMap<ModuleInfo *>());
	NEW_CLASS(this->globalModule.objKeys, Data::ArrayListString());
	NEW_CLASS(this->globalModule.objValues, Data::ArrayList<ObjectInfo*>());
	NEW_CLASS(this->globalModule.oidList, Data::ArrayList<ObjectInfo*>());
}

Net::ASN1MIB::~ASN1MIB()
{
	this->UnloadAll();
	DEL_CLASS(this->moduleMap);
	DEL_CLASS(this->globalModule.objKeys);
	DEL_CLASS(this->globalModule.objValues);
	DEL_CLASS(this->globalModule.oidList);
}

Net::ASN1MIB::ModuleInfo *Net::ASN1MIB::GetGlobalModule()
{
	return &this->globalModule;	
}

Net::ASN1MIB::ModuleInfo *Net::ASN1MIB::GetModuleByFileName(Text::CString fileName)
{
	ModuleInfo *module;
	UOSInt i = this->moduleMap->GetCount();
	while (i-- > 0)
	{
		module = this->moduleMap->GetItem(i);
		if (module->moduleFileName && module->moduleFileName->Equals(fileName.v, fileName.leng))
			return module;
	}
	return 0;
}

void Net::ASN1MIB::UnloadAll()
{
	Data::ArrayList<ObjectInfo*> *objList;
	ObjectInfo *obj;
	ModuleInfo *module;
	UOSInt i = this->moduleMap->GetCount();
	UOSInt j;
	UOSInt k;
	while (i-- > 0)
	{
		module = this->moduleMap->GetItem(i);
		objList = module->objValues;
		j = objList->GetCount();
		while (j-- > 0)
		{
			obj = objList->GetItem(j);
			obj->objectName->Release();
			SDEL_STRING(obj->typeName);
			SDEL_STRING(obj->typeVal);
			k = obj->valName->GetCount();
			while (k-- > 0)
			{
				obj->valName->GetItem(k)->Release();
				obj->valCont->GetItem(k)->Release();
			}
			DEL_CLASS(obj->valName);
			DEL_CLASS(obj->valCont);
			MemFree(obj);
		}

		DEL_CLASS(module->objKeys);
		DEL_CLASS(module->objValues);
		SDEL_STRING(module->moduleName);
		SDEL_STRING(module->moduleFileName);
		DEL_CLASS(module->oidList);
		MemFree(module);
	}
	this->moduleMap->Clear();

	this->globalModule.oidList->Clear();
	this->globalModule.objKeys->Clear();
	this->globalModule.objValues->Clear();
}

Bool Net::ASN1MIB::LoadFile(Text::CString fileName, Text::StringBuilderUTF8 *errMessage)
{
	return LoadFileInner(fileName, errMessage, true);
}
