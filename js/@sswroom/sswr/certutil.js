import * as data from "./data.js";
import * as text from "./text.js";

export const ASN1ItemType = {
	UNKNOWN: 0,
	BOOLEAN: 0x01,
	INTEGER: 0x02,
	BIT_STRING: 0x03,
	OCTET_STRING: 0x04,
	NULL: 0x05,
	OID: 0x06,
	ENUMERATED: 0x0a,
	UTF8STRING: 0x0c,
	NUMERICSTRING: 0x12,
	PRINTABLESTRING: 0x13,
	T61STRING: 0x14,
	VIDEOTEXSTRING: 0x15,
	IA5STRING: 0x16,
	UTCTIME: 0x17,
	GENERALIZEDTIME: 0x18,
	UNIVERSALSTRING: 0x1c,
	BMPSTRING: 0x1e,
	SEQUENCE: 0x30,
	SET: 0x31,
	CHOICE_0: 0x80,
	CHOICE_1: 0x81,
	CHOICE_2: 0x82,
	CHOICE_3: 0x83,
	CHOICE_4: 0x84,
	CHOICE_5: 0x85,
	CHOICE_6: 0x86,
	CHOICE_7: 0x87,
	CHOICE_8: 0x88,
	CONTEXT_SPECIFIC_0: 0xa0,
	CONTEXT_SPECIFIC_1: 0xa1,
	CONTEXT_SPECIFIC_2: 0xa2,
	CONTEXT_SPECIFIC_3: 0xa3,
	CONTEXT_SPECIFIC_4: 0xa4
}

export const RuleCond = {
	Any: 0,
	TypeIsItemType: 1,
	TypeIsTime: 2,
	TypeIsString: 3,
	TypeIsOpt: 4,
	RepeatIfTypeIs: 5,
	LastOIDAndTypeIs: 6,
	AllNotMatch: 7
}

export class PDUInfo
{
	/**
	 * @param {number} rawOfst
	 * @param {number} hdrLen
	 * @param {number} contLen
	 * @param {number} itemType
	 */
	constructor(rawOfst, hdrLen, contLen, itemType)
	{
		this.rawOfst = rawOfst;
		this.hdrLen = hdrLen;
		this.contLen = contLen;
		this.itemType = itemType;
	}

	get dataOfst()
	{
		return this.rawOfst + this.hdrLen;
	}

	get endOfst()
	{
		return this.rawOfst + this.hdrLen + this.contLen;
	}
}

export class ASN1Util
{
	/**
	 * @param {data.ByteReader} reader
	 * @param {number} ofst
	 * @param {number} endOfst
	 */
	static pduParseLen(reader, ofst, endOfst)
	{
		if (ofst >= endOfst)
			return null;
		let v = reader.readUInt8(ofst);
		if (v & 0x80)
		{
			if (v == 0x81)
			{
				if (ofst + 2 > endOfst)
					return null;
				return {nextOfst: ofst + 2, pduLen: reader.readUInt8(ofst + 1)};
			}
			else if (v == 0x82)
			{
				if (ofst + 3 > endOfst)
					return null;
				return {nextOfst: ofst + 3, pduLen: reader.readUInt16(ofst + 1, false)};
			}
			else if (v == 0x83)
			{
				if (ofst + 4 > endOfst)
					return null;
				return {nextOfst: ofst + 4, pduLen: reader.readUInt24(ofst + 1, false)};
			}
			else if (v == 0x84)
			{
				if (ofst + 5 > endOfst)
					return null;
				return {nextOfst: ofst + 5, pduLen: reader.readUInt32(ofst + 1, false)};
			}
			else if (v == 0x80)
			{
				return {nextOfst: ofst + 1, pduLen: 0};
			}
			return null;
		}
		else
		{
			return {nextOfst: ofst + 1, pduLen: v};
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} ofst
	 * @param {number} endOfst
	 */
	static pduParseUInt32(reader, ofst, endOfst)
	{
		if (endOfst - ofst < 3)
			return null;
		if (reader.readUInt8(ofst) != 2)
			return null;
		let len = reader.readUInt8(ofst + 1);
		if (len == 1)
		{
			return {nextOfst: ofst + 3,
				val: reader.readUInt8(ofst + 2)};
		}
		else if (len == 2 && endOfst - ofst >= 4)
		{
			return {nextOfst: ofst + 4,
				val: reader.readUInt16(ofst + 2, false)};
		}
		else if (len == 3 && endOfst - ofst >= 5)
		{
			return {nextOfst: ofst + 5,
				val: reader.readUInt24(ofst + 2, false)};
		}
		else if (len == 4 && endOfst - ofst >= 6)
		{
			return {nextOfst: ofst + 6,
				val: reader.readUInt32(ofst + 2, false)};
		}
		else
		{
			return null;
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} ofst
	 * @param {number} endOfst
	 */
	static pduParseString(reader, ofst, endOfst)
	{
		let len = ASN1Util.pduParseLen(reader, ofst, endOfst);
		if (len == null)
			return null;
		if (len.itemType != 4)
			return null;
		if (len.nextOfst + len.pduLen > endOfst)
			return null;
		return {nextOfst: len.nextOfst + len.pduLen,
			val: reader.readUTF8(len.nextOfst, len.pduLen)};
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} ofst
	 * @param {number} endOfst
	 */
	static pduParseChoice(reader, ofst, endOfst)
	{
		if (endOfst - ofst < 3)
			return null;
		if (reader.readUInt8(ofst) != 10)
			return null;
		let len = reader.readUInt8(ofst + 1);
		if (len == 1)
		{
			return {nextOfst: ofst + 3,
				val: reader.readUInt8(ofst + 2)};
		}
		else if (len == 2 && endOfst - ofst >= 4)
		{
			return {nextOfst: ofst + 4,
				val: reader.readUInt16(ofst + 2, false)};
		}
		else if (len == 3 && endOfst - ofst >= 5)
		{
			return {nextOfst: ofst + 5,
				val: reader.readUInt24(ofst + 2, false)};
		}
		else if (len == 4 && endOfst - ofst >= 6)
		{
			return {nextOfst: ofst + 6,
				val: reader.readUInt32(ofst + 2, false)};
		}
		else
		{
			return null;
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 */
	static pduParseUTCTimeCont(reader, startOfst, endOfst)
	{
		if ((endOfst - startOfst) == 13 && reader.readUInt8(startOfst + 12) == 'Z'.charCodeAt(0))
		{
			let tv = new data.TimeValue();
			tv.year = ASN1Util.str2Digit(reader, startOfst);
			if (tv.year < 70)
			{
				tv.year += 2000;
			}
			else
			{
				tv.year += 1900;
			}
			tv.month = ASN1Util.str2Digit(reader, startOfst + 2);
			tv.day = ASN1Util.str2Digit(reader, startOfst + 4);
			tv.hour = ASN1Util.str2Digit(reader, startOfst + 6);
			tv.minute = ASN1Util.str2Digit(reader, startOfst + 8);
			tv.second = ASN1Util.str2Digit(reader, startOfst + 10);
			tv.nanosec = 0;
			tv.tzQhr = 0;
			return data.Timestamp.fromTimeValue(tv);
		}
		else if ((endOfst - startOfst) == 15 && reader.readUInt8(startOfst + 14) == 'Z'.charCodeAt(0))
		{
			let tv = new data.TimeValue();
			tv.year = ASN1Util.str2Digit(reader, startOfst) * 100 + ASN1Util.str2Digit(reader, startOfst + 2);
			tv.month = ASN1Util.str2Digit(reader, startOfst + 4);
			tv.day = ASN1Util.str2Digit(reader, startOfst + 6);
			tv.hour = ASN1Util.str2Digit(reader, startOfst + 8);
			tv.minute = ASN1Util.str2Digit(reader, startOfst + 10);
			tv.second = ASN1Util.str2Digit(reader, startOfst + 12);
			tv.nanosec = 0;
			tv.tzQhr = 0;
			return data.Timestamp.fromTimeValue(tv);
		}
		return null;
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string[]} outLines
	 * @param {number} level
	 * @param {ASN1Names|null|undefined} names
	 */
	static pduToString(reader, startOfst, endOfst, outLines, level, names)
	{
		let startOfstZ;
		while (startOfst < endOfst)
		{
			let type = reader.readUInt8(startOfst);
			let sb;
			let len = ASN1Util.pduParseLen(reader, startOfst + 1, endOfst);
			if (len == null)
			{
				return null;
			}
			else if (len.nextOfst + len.pduLen > endOfst)
			{
				return null;
			}
	
			let name;
			if (names == null)
				name = null;
			else
				name = names.readNameNoDef(type, len.pduLen, reader, len.nextOfst);

			switch (type)
			{
			case 0x1:
				sb = ["\t".repeat(level)];
				if (name) sb.push(name+" ")
				sb.push("BOOLEAN (");
				sb.push(ASN1Util.booleanToString(reader, len.nextOfst, len.pduLen));
				sb.push(')');
				outLines.push(sb.join(""));
				startOfst = len.nextOfst + len.pduLen;
				break;
			case 0x2:
				if (len.pduLen <= 4)
				{
					let val = ASN1Util.pduParseUInt32(reader, startOfst, endOfst);
					if (val == null)
					{
						return null;
					}
					sb = ["\t".repeat(level)];
					if (name) sb.push(name+" ")
					sb.push("INTEGER ");
					sb.push(val.val.toString());
					outLines.push(sb.join(""));
					startOfst = len.nextOfst + len.pduLen;
				}
				else if (len.pduLen <= 32)
				{
					sb = ["\t".repeat(level)];
					if (name) sb.push(name+" ")
					sb.push("INTEGER ");
					sb.push(text.u8Arr2Hex(new Uint8Array(reader.getArrayBuffer(len.nextOfst, len.pduLen)), " ", null));
					outLines.push(sb.join(""));
					startOfst = len.nextOfst + len.pduLen;
				}
				else
				{
					sb = ["\t".repeat(level)];
					if (name) sb.push(name+" ")
					sb.push("INTEGER");
					outLines.push(sb.join(""));
					outLines.push(text.u8Arr2Hex(new Uint8Array(reader.getArrayBuffer(len.nextOfst, len.pduLen)), " ", "\r\n"));
					startOfst = len.nextOfst + len.pduLen;
				}
				break;
			case 0x3:
				sb = ["\t".repeat(level)];
				if (name) sb.push(name+" ")
				sb.push("BIT STRING ");
				sb.push(text.toHex8(reader.readUInt8(len.nextOfst)));
				if (ASN1Util.pduIsValid(reader, len.nextOfst + 1, len.nextOfst + len.pduLen))
				{
					sb.push(" {");
					outLines.push(sb.join(""));
					if (names) names.readContainerBegin();
					ASN1Util.pduToString(reader, len.nextOfst + 1, len.nextOfst + len.pduLen, outLines, level + 1, names);
					if (names) names.readContainerEnd();
					outLines.push("\t".repeat(level)+"}");
				}
				else
				{
					sb.push(" (");
					sb.push(text.u8Arr2Hex(new Uint8Array(reader.getArrayBuffer(len.nextOfst + 1, len.pduLen - 1)), " ", null));
					sb.push(")");
					outLines.push(sb.join(""));
				}
				startOfst = len.nextOfst + len.pduLen;
				break;
			case 0x4:
				sb = ["\t".repeat(level)];
				if (name) sb.push(name+" ")
				sb.push("OCTET STRING ");
				if (ASN1Util.pduIsValid(reader, len.nextOfst, len.nextOfst + len.pduLen))
				{
					sb.push("{");
					outLines.push(sb.join(""));
					if (names) names.readContainerBegin();
					ASN1Util.pduToString(reader, len.nextOfst, len.nextOfst + len.pduLen, outLines, level + 1, names);
					if (names) names.readContainerEnd();
					outLines.push("\t".repeat(level)+"}");
				}
				else
				{
					sb.push("(");
					sb.push(text.u8Arr2Hex(new Uint8Array(reader.getArrayBuffer(len.nextOfst + 1, len.pduLen - 1)), " ", null));
					sb.push(")");
					outLines.push(sb.join(""));
				}
				startOfst = len.nextOfst + len.pduLen;
				break;
			case 0x5:
				sb = ["\t".repeat(level)];
				if (name) sb.push(name+" ")
				sb.push("NULL");
				outLines.push(sb.join(""));
				startOfst = len.nextOfst + len.pduLen;
				break;
			case 0x6:
				sb = ["\t".repeat(level)];
				if (name) sb.push(name+" ")
				sb.push("OID ");
				let oidPDU = new Uint8Array(reader.getArrayBuffer(len.nextOfst, len.pduLen));
				sb.push(ASN1Util.oidToString(oidPDU));
				sb.push(" (");
				//sb.push(oiddb.oidToNameString(oidPDU));
				sb.push(")");
				outLines.push(sb.join(""));
				startOfst = len.nextOfst + len.pduLen;
				break;
			case 0x0a:
				if (len.pduLen == 1)
				{
					sb = ["\t".repeat(level)];
					if (name) sb.push(name+" ")
					sb.push("ENUMERATED ");
					sb.push(reader.readUInt8(len.nextOfst).toString());
					outLines.push(sb.join(""));
					startOfst = len.nextOfst + len.pduLen;
				}
				else
				{
					return null;
				}
				break;
			case 0x0C:
				sb = ["\t".repeat(level)];
				if (name) sb.push(name+" ")
				sb.push("UTF8String ");
				sb.push(reader.readUTF8(len.nextOfst, len.pduLen));
				outLines.push(sb.join(""));
				startOfst = len.nextOfst + len.pduLen;
				break;
			case 0x12:
				sb = ["\t".repeat(level)];
				if (name) sb.push(name+" ")
				sb.push("NumericString ");
				sb.push(reader.readUTF8(len.nextOfst, len.pduLen));
				outLines.push(sb.join(""));
				startOfst = len.nextOfst + len.pduLen;
				break;
			case 0x13:
				sb = ["\t".repeat(level)];
				if (name) sb.push(name+" ")
				sb.push("PrintableString ");
				sb.push(reader.readUTF8(len.nextOfst, len.pduLen));
				outLines.push(sb.join(""));
				startOfst = len.nextOfst + len.pduLen;
				break;
			case 0x14:
				sb = ["\t".repeat(level)];
				if (name) sb.push(name+" ")
				sb.push("T61String ");
				sb.push(reader.readUTF8(len.nextOfst, len.pduLen));
				outLines.push(sb.join(""));
				startOfst = len.nextOfst + len.pduLen;
				break;
			case 0x15:
				sb = ["\t".repeat(level)];
				if (name) sb.push(name+" ")
				sb.push("VideotexString ");
				sb.push(reader.readUTF8(len.nextOfst, len.pduLen));
				outLines.push(sb.join(""));
				startOfst = len.nextOfst + len.pduLen;
				break;
			case 0x16:
				sb = ["\t".repeat(level)];
				if (name) sb.push(name+" ")
				sb.push("IA5String ");
				if (reader.readUInt8(len.nextOfst + len.pduLen - 1) == 0)
					sb.push(reader.readUTF8(len.nextOfst + 1, len.pduLen - 1));
				else
					sb.push(reader.readUTF8(len.nextOfst, len.pduLen));
				outLines.push(sb.join(""));
				startOfst = len.nextOfst + len.pduLen;
				break;
			case 0x17:
				sb = ["\t".repeat(level)];
				if (name) sb.push(name+" ")
				sb.push("UTCTIME ");
				if (len.pduLen == 13 && reader.readUTF8(len.nextOfst + 12, 1) == "Z")
				{
					let tv = new data.TimeValue();
					tv.year = ASN1Util.str2Digit(reader, len.nextOfst) + 2000;
					tv.month = ASN1Util.str2Digit(reader, len.nextOfst + 2);
					tv.day = ASN1Util.str2Digit(reader, len.nextOfst + 4);
					tv.hour = ASN1Util.str2Digit(reader, len.nextOfst + 6);
					tv.minute = ASN1Util.str2Digit(reader, len.nextOfst + 8);
					tv.second = ASN1Util.str2Digit(reader, len.nextOfst + 10);
					tv.nanosec = 0;
					tv.tzQhr = 0;
					sb.push(data.Timestamp.fromTimeValue(tv).toStringNoZone());
				}
				else
				{
					sb.push(reader.readUTF8(len.nextOfst, len.pduLen));
				}
				outLines.push(sb.join(""));
				startOfst = len.nextOfst + len.pduLen;
				break;
			case 0x18:
				sb = ["\t".repeat(level)];
				if (name) sb.push(name+" ")
				sb.push("GeneralizedTime ");
				if (len.pduLen == 15 && reader.readUTF8(len.nextOfst + 14, 1) == "Z")
				{
					let tv = new data.TimeValue();
					tv.year = ASN1Util.str2Digit(reader, len.nextOfst) * 100 + ASN1Util.str2Digit(reader, len.nextOfst + 2);
					tv.month = ASN1Util.str2Digit(reader, len.nextOfst + 4);
					tv.day = ASN1Util.str2Digit(reader, len.nextOfst + 6);
					tv.hour = ASN1Util.str2Digit(reader, len.nextOfst + 8);
					tv.minute = ASN1Util.str2Digit(reader, len.nextOfst + 10);
					tv.second = ASN1Util.str2Digit(reader, len.nextOfst + 12);
					tv.nanosec = 0;
					tv.tzQhr = 0;
					sb.push(data.Timestamp.fromTimeValue(tv).toStringNoZone());
				}
				else
				{
					sb.push(reader.readUTF8(len.nextOfst, len.pduLen));
				}
				outLines.push(sb.join(""));
				startOfst = len.nextOfst + len.pduLen;
				break;
			case 0x1C:
				sb = ["\t".repeat(level)];
				if (name) sb.push(name+" ")
				sb.push("UniversalString ");
				sb.push(reader.readUTF8(len.nextOfst, len.pduLen));
				outLines.push(sb.join(""));
				startOfst = len.nextOfst + len.pduLen;
				break;
			case 0x1E:
				sb = ["\t".repeat(level)];
				if (name) sb.push(name+" ")
				if (len.pduLen & 1)
				{
					sb.push("BMPString (");
					sb.push(text.u8Arr2Hex(new Uint8Array(reader.getArrayBuffer(len.nextOfst, len.pduLen)), ' ', null));
					sb.push(")");
				}
				else
				{
					sb.push("BMPString ");
					sb.push(reader.readUTF16(len.nextOfst, len.pduLen >> 1, false));
				}
				outLines.push(sb.join(""));
				startOfst = len.nextOfst + len.pduLen;
				break;
			case 0x0:
				if (len.pduLen == 0)
				{
					return startOfst + 2;
				}
			default:
				if (type < 0x30)
				{
					sb = ["\t".repeat(level)];
					if (name) sb.push(name+" ")
					sb.push("UNKNOWN 0x");
					sb.push(text.toHex8(type));
					sb.push(" (");
					sb.push(text.u8Arr2Hex(new Uint8Array(reader.getArrayBuffer(len.nextOfst, len.pduLen)), " ", null));
					sb.push(')');
					outLines.push(sb.join(""));
					startOfst = len.nextOfst + len.pduLen;
					break;
				}
				else
				{
					sb = ["\t".repeat(level)];
					if (name) sb.push(name+" ")
					sb.push("UNKNOWN 0x");
					sb.push(text.toHex8(type));
					if (ASN1Util.pduIsValid(reader, len.nextOfst, len.nextOfst + len.pduLen))
					{
						sb.push(" {");
						outLines.push(sb.join(""));
						if (names) names.readContainerBegin();
						ASN1Util.pduToString(reader, len.nextOfst, len.nextOfst + len.pduLen, outLines, level + 1, names);
						if (names) names.readContainerEnd();
						outLines.push("\t".repeat(level)+"}");
					}
					else
					{
						sb.push(" (");
						if (reader.isASCIIText(len.nextOfst, len.pduLen))
						{
							sb.push(reader.readUTF8(len.nextOfst, len.pduLen));
						}
						else
						{
							sb.push(text.u8Arr2Hex(new Uint8Array(reader.getArrayBuffer(len.nextOfst, len.pduLen)), " ", null));
						}
						sb.push(")");
					}
					outLines.push(sb.join(""));
					startOfst = len.nextOfst + len.pduLen;
					break;
				}
			case 0x30:
				sb = ["\t".repeat(level)];
				if (name) sb.push(name+" ")
				sb.push("SEQUENCE {");
				outLines.push(sb.join(""));

				if (reader.readUInt8(startOfst + 1) == 0x80)
				{
					startOfst = len.nextOfst;
					if (names) names.readContainerBegin();
					startOfstZ = ASN1Util.pduToString(reader, startOfst, endOfst, outLines, level + 1, names);
					if (startOfstZ == null)
					{
						return null;
					}
					startOfst = startOfstZ;
					if (names) names.readContainerEnd();
				}
				else
				{
					startOfst = len.nextOfst;
					if (names) names.readContainerBegin();
					startOfstZ = ASN1Util.pduToString(reader, startOfst, startOfst + len.pduLen, outLines, level + 1, names);
					if (startOfstZ == null)
					{
						return null;
					}
					if (names) names.readContainerEnd();
					startOfst = len.nextOfst + len.pduLen;
				}
				outLines.push("\t".repeat(level)+"}");
				break;
			case 0x31:
				sb = ["\t".repeat(level)];
				if (name) sb.push(name+" ")
				sb.push("SET {");
				outLines.push(sb.join(""));

				if (reader.readUInt8(startOfst + 1) == 0x80)
				{
					startOfst = len.nextOfst;
					if (names) names.readContainerBegin();
					startOfstZ = ASN1Util.pduToString(reader, startOfst, endOfst, outLines, level + 1, names);
					if (startOfstZ == null)
					{
						return null;
					}
					startOfst = startOfstZ;
					if (names) names.readContainerEnd();
				}
				else
				{
					startOfst = len.nextOfst;
					if (names) names.readContainerBegin();
					startOfstZ = ASN1Util.pduToString(reader, startOfst, startOfst + len.pduLen, outLines, level + 1, names);
					if (startOfstZ == null)
					{
						return null;
					}
					if (names) names.readContainerEnd();
					startOfst = len.nextOfst + len.pduLen;
				}
				outLines.push("\t".repeat(level)+"}");
				break;
			case 0x80:
			case 0x81:
			case 0x82:
			case 0x83:
			case 0x84:
			case 0x85:
			case 0x86:
			case 0x87:
			case 0x88:
				sb = ["\t".repeat(level)];
				if (name) sb.push(name+" ")
				sb.push("CHOICE[");
				sb.push((type - 0x80).toString());
				sb.push("] ");
				if (reader.readUInt8(startOfst + 1) == 0x80)
				{
					sb.push("{");
					outLines.push(sb.join(""));
					if (names) names.readContainerBegin();
					startOfstZ = ASN1Util.pduToString(reader, len.nextOfst, len.nextOfst + len.pduLen, outLines, level + 1, names);
					if (startOfstZ == null)
					{
						return null;
					}
					startOfst = startOfstZ;
					if (names) names.readContainerEnd();
					outLines.push("\t".repeat(level)+"}");
				}
				else
				{
					if (ASN1Util.pduIsValid(reader, len.nextOfst, len.nextOfst + len.pduLen))
					{
						sb.push("{");
						outLines.push(sb.join(""));
						if (names) names.readContainerBegin();
						ASN1Util.pduToString(reader, len.nextOfst, len.nextOfst + len.pduLen, outLines, level + 1, names);
						if (names) names.readContainerEnd();
						outLines.push("\t".repeat(level)+"}");
					}
					else
					{
						sb.push(" (");
						if (reader.isASCIIText(len.nextOfst, len.pduLen))
						{
							sb.push(reader.readUTF8(len.nextOfst, len.pduLen));
						}
						else
						{
							sb.push(text.u8Arr2Hex(new Uint8Array(reader.getArrayBuffer(len.nextOfst, len.pduLen)), " ", null));
						}
						sb.push(")");
						outLines.push(sb.join(""));
					}
					startOfst = len.nextOfst + len.pduLen;
				}
				break;
			case 0xA0:
			case 0xA1:
			case 0xA2:
			case 0xA3:
			case 0xA4:
			case 0xA5:
			case 0xA6:
			case 0xA7:
			case 0xA8:
				sb = ["\t".repeat(level)];
				if (name) sb.push(name+" ")
				sb.push("CONTEXT SPECIFIC[");
				sb.push((type - 0xA0).toString());
				sb.push("] ");
				if (reader.readUInt8(startOfst + 1) == 0x80)
				{
					sb.push("{");
					outLines.push(sb.join(""));
					if (names) names.readContainerBegin();
					startOfstZ = ASN1Util.pduToString(reader, len.nextOfst, endOfst, outLines, level + 1, names);
					if (startOfstZ == null)
					{
						return null;
					}
					startOfst = startOfstZ;
					if (names) names.readContainerEnd();
					outLines.push("\t".repeat(level)+"}");
				}
				else
				{
					if (ASN1Util.pduIsValid(reader, len.nextOfst, len.nextOfst + len.pduLen))
					{
						sb.push("{");
						outLines.push(sb.join(""));
						if (names) names.readContainerBegin();
						ASN1Util.pduToString(reader, len.nextOfst, len.nextOfst + len.pduLen, outLines, level + 1, names);
						if (names) names.readContainerEnd();
						outLines.push("\t".repeat(level)+"}");
					}
					else
					{
						sb.push(" (");
						if (reader.isASCIIText(len.nextOfst, len.pduLen))
						{
							sb.push(reader.readUTF8(len.nextOfst, len.pduLen));
						}
						else
						{
							sb.push(text.u8Arr2Hex(new Uint8Array(reader.getArrayBuffer(len.nextOfst, len.pduLen)), " ", null));
						}
						sb.push(")");
						outLines.push(sb.join(""));
					}
					startOfst = len.nextOfst + len.pduLen;
				}
				break;
			}
		}
		return startOfst;
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 */
	static pduDSizeEnd(reader, startOfst, endOfst)
	{
		let startOfstZ;
		while (startOfst < endOfst)
		{
			let len = ASN1Util.pduParseLen(reader, startOfst + 1, endOfst);
			if (len == null)
			{
				return null;
			}
			else if (len.nextOfst + len.pduLen > endOfst)
			{
				return null;
			}
	
			if (reader.readUInt8(startOfst) == 0 && reader.readUInt8(startOfst + 1) == 0)
			{
				return startOfst + 2;
			}
			else if (reader.readUInt8(startOfst + 1) == 0x80)
			{
				startOfstZ = ASN1Util.pduDSizeEnd(reader, len.nextOfst, endOfst);
				if (startOfstZ == null)
				{
					return null;
				}
				startOfst = startOfstZ;
			}
			else
			{
				startOfst = len.nextOfst + len.pduLen;
			}
		}
		return startOfst;
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string | null} path
	 */
	static pduGetItem(reader, startOfst, endOfst, path)
	{
		let ofstZ;
		if (path == null || path == "")
			return null;
		let i = path.indexOf(".");
		let cnt;
		if (i == -1)
		{
			cnt = Number.parseInt(path);
			path = "";
		}
		else
		{
			cnt = Number.parseInt(path.substr(0, i));
			path = path.substr(i + 1);
		}
		if (Number.isNaN(cnt) || cnt < 1)
			return null;

		while (startOfst < endOfst)
		{
			let len = ASN1Util.pduParseLen(reader, startOfst + 1, endOfst);
			if (len == null)
			{
				return null;
			}
			else if (len.nextOfst + len.pduLen > endOfst)
			{
				return null;
			}
			else if (reader.readUInt8(startOfst) == 0 && reader.readUInt8(startOfst + 1) == 0)
			{
				return null;
			}
	
			cnt--;
			if (cnt == 0)
			{
				if (path == "")
				{
					if (reader.readUInt8(startOfst + 1) == 0x80)
					{
						ofstZ = ASN1Util.pduDSizeEnd(reader, len.nextOfst, endOfst);
						if (ofstZ == null)
							return null;
						let ret = new PDUInfo(startOfst, len.nextOfst - startOfst, ofstZ, reader.readUInt8(startOfst));
						if (ret.contLen == null)
							return null;
						return ret;
					}
					else
					{
						return new PDUInfo(startOfst, len.nextOfst - startOfst, len.pduLen, reader.readUInt8(startOfst));
					}
				}
				if (reader.readUInt8(startOfst + 1) == 0x80)
				{
					return ASN1Util.pduGetItem(reader, len.nextOfst, endOfst, path);
				}
				else
				{
					return ASN1Util.pduGetItem(reader, len.nextOfst, len.nextOfst + len.pduLen, path);
				}
			}
			else if (reader.readUInt8(startOfst + 1) == 0x80)
			{
				ofstZ = ASN1Util.pduDSizeEnd(reader, len.nextOfst, endOfst);
				if (ofstZ == null)
				{
					return null;
				}
				startOfst = ofstZ;
			}
			else
			{
				startOfst = len.nextOfst + len.pduLen;
			}
		}
		return null;
	}

	/**
	 * @param {any} reader
	 * @param {any} startOfst
	 * @param {any} endOfst
	 * @param {any} path
	 */
	static pduGetItemType(reader, startOfst, endOfst, path)
	{
		let item = ASN1Util.pduGetItem(reader, startOfst, endOfst, path);
		if (item == null)
			return ASN1ItemType.UNKNOWN;
		else
			return item.itemType;
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 * @param {string | null} path
	 */
	static pduCountItem(reader, startOfst, endOfst, path)
	{
		let startOfstZ;
		let cnt;
		let len;
		if (path == null || path == "")
		{
			cnt = 0;
			while (startOfst < endOfst)
			{
				len = ASN1Util.pduParseLen(reader, startOfst + 1, endOfst);
				if (len == null)
				{
					return 0;
				}
				else if (len.nextOfst + len.pduLen > endOfst)
				{
					return 0;
				}
				else if (reader.readUInt8(startOfst) == 0 && reader.readUInt8(startOfst + 1) == 0)
				{
					return cnt;
				}
				cnt++;
				if (reader.readUInt8(startOfst + 1) == 0x80)
				{
					if (ASN1Util.pduDSizeEnd(reader, len.nextOfst, len.nextOfst + len.pduLen) != null)
					{
						return cnt;
					}
				}
				else
				{
					startOfst = len.nextOfst + len.pduLen;
				}
			}
			return cnt;
		}
		len = path.indexOf(".");
		if (len == -1)
		{
			cnt = Number.parseInt(path);
			path = "";
		}
		else
		{
			cnt = Number.parseInt(path.substr(0, len));
			path = path.substr(len + 1);
		}
	
		if (Number.isNaN(cnt) || cnt < 1)
		{
			return 0;
		}
	
		while (startOfst < endOfst)
		{
			len = ASN1Util.pduParseLen(reader, startOfst + 1, endOfst);
			if (len == null)
			{
				return 0;
			}
			else if (len.nextOfst + len.pduLen > endOfst)
			{
				return 0;
			}
			else if (reader.readUInt8(startOfst) == 0 && reader.readUInt8(startOfst + 1) == 0)
			{
				return 0;
			}
	
			cnt--;
			if (cnt == 0)
			{
				if (reader.readUInt8(startOfst + 1) == 0x80)
				{
					return ASN1Util.pduCountItem(reader, len.nextOfst, endOfst, path);
				}
				else
				{
					return ASN1Util.pduCountItem(reader, len.nextOfst, len.nextOfst + len.pduLen, path);
				}
			}
			else if (reader.readUInt8(startOfst + 1) == 0x80)
			{
				startOfstZ = ASN1Util.pduDSizeEnd(reader, len.nextOfst, endOfst);
				if (startOfstZ == null)
				{
					return 0;
				}
				startOfst = startOfstZ;
			}
			else
			{
				startOfst = len.nextOfst + len.pduLen;
			}
		}
		return 0;
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} startOfst
	 * @param {number} endOfst
	 */
	static pduIsValid(reader, startOfst, endOfst)
	{
		while (startOfst < endOfst)
		{
			let len = ASN1Util.pduParseLen(reader, startOfst + 1, endOfst);
			if (len == null)
			{
				return false;
			}
			else if (len.nextOfst + len.pduLen > endOfst)
			{
				return false;
			}
			let type = reader.readUInt8(startOfst);
			if (type >= 0x30 && type < 0x40)
			{
				if (!ASN1Util.pduIsValid(reader, len.nextOfst, len.nextOfst + len.pduLen))
				{
					return false;
				}
			}
			startOfst = len.nextOfst + len.pduLen;
		}
		return true;
	}

	/**
	 * @param {Uint8Array | number[]} oid1
	 * @param {Uint8Array | number[]} oid2
	 */
	static oidCompare(oid1, oid2)
	{
		let i = 0;
		let oid1Len = oid1.length;
		let oid2Len = oid2.length;
		while (true)
		{
			if (i == oid1Len && i == oid2Len)
			{
				return 0;
			}
			else if (i >= oid1Len)
			{
				return -1;
			}
			else if (i >= oid2Len)
			{
				return 1;
			}
			else if (oid1[i] > oid2[i])
			{
				return 1;
			}
			else if (oid1[i] < oid2[i])
			{
				return -1;
			}
			i++;
		}
	}

	/**
	 * @param {Uint8Array | number[]} oidPDU
	 * @param {string} oidText
	 */
	static oidEqualsText(oidPDU, oidText)
	{
		let oid2 = ASN1Util.oidText2PDU(oidText);
		if (oid2 == null)
			return false;
		return ASN1Util.oidCompare(oidPDU, oid2) == 0;
	}

	/**
	 * @param {Uint8Array | number[]} oidPDU
	 */
	static oidToString(oidPDU)
	{
		let sb = [];
		let v = 0;
		let i = 1;
		sb.push(Math.floor(oidPDU[0] / 40));
		sb.push(oidPDU[0] % 40);
		while (i < oidPDU.length)
		{
			v = (v << 7) | (oidPDU[i] & 0x7f);
			if ((oidPDU[i] & 0x80) == 0)
			{
				sb.push(v);
				v = 0;
			}
			i++;
		}
		return sb.join(".");
	}

	/**
	 * @param {string} oidText
	 * @returns {number[] | null}
	 */
	static oidText2PDU(oidText)
	{
		let sarr = oidText.split(".");
		let i = 2;
		let j = sarr.length;
		if (j == 1)
		{
			return [Number.parseInt(sarr[0])];
		}
		let pduBuff = [];
		pduBuff.push(pduBuff[0] * 40 + pduBuff[1]);
		if (j == 2)
		{
			return pduBuff;
		}
		while (i < j)
		{
			let v = Number.parseInt(sarr[i]);
			if (Number.isNaN(v))
				return null;
			if (v < 128)
			{
				pduBuff.push(v);
			}
			else if (v < 0x4000)
			{
				pduBuff.push(0x80 | (v >> 7));
				pduBuff.push((v & 0x7f));
			}
			else if (v < 0x200000)
			{
				pduBuff.push(0x80 | (v >> 14));
				pduBuff.push(0x80 | ((v >> 7) & 0x7f));
				pduBuff.push(v & 0x7f);
			}
			else if (v < 0x10000000)
			{
				pduBuff.push(0x80 | (v >> 21));
				pduBuff.push(0x80 | ((v >> 14) & 0x7f));
				pduBuff.push(0x80 | ((v >> 7) & 0x7f));
				pduBuff.push(v & 0x7f);
			}
			else
			{
				pduBuff.push(0x80 | (v >> 28));
				pduBuff.push(0x80 | ((v >> 21) & 0x7f));
				pduBuff.push(0x80 | ((v >> 14) & 0x7f));
				pduBuff.push(0x80 | ((v >> 7) & 0x7f));
				pduBuff.push(v & 0x7f);
			}
			i++;
		}
		return pduBuff;
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} ofst
	 * @param {number} len
	 */
	static booleanToString(reader, ofst, len)
	{
		if (len == 1)
		{
			let v = reader.readUInt8(ofst);
			if (v == 0xFF)
			{
				return "0xFF TRUE";
			}
			else if (v == 0)
			{
				return "0x00 FALSE";
			}
			else
			{
				return "0x"+text.toHex8(v);
			}
		}
		else
		{
			return text.u8Arr2Hex(new Uint8Array(reader.getArrayBuffer(ofst, len)), " ", null);
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} ofst
	 * @param {number} len
	 */
	static integerToString(reader, ofst, len)
	{
		switch (len)
		{
		case 1:
			return reader.readUInt8(ofst).toString();
		case 2:
			return reader.readUInt16(ofst, false).toString();
		case 3:
			return reader.readUInt24(ofst, false).toString();
		case 4:
			return reader.readUInt32(ofst, false).toString();
		default:
			return text.u8Arr2Hex(new Uint8Array(reader.getArrayBuffer(ofst, len)), " ", null);
		}
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} ofst
	 * @param {number} len
	 */
	static utcTimeToString(reader, ofst, len)
	{
		let ts = ASN1Util.pduParseUTCTimeCont(reader, ofst, ofst + len);
		if (ts)
		{
			return ts.toString("yyyy-MM-dd HH:mm:ss");
		}
		return "";
	}

	/**
	 * @param {number} itemType
	 */
	static itemTypeGetName(itemType)
	{
		switch (itemType)
		{
		case ASN1ItemType.UNKNOWN:
			return "UNKNOWN";
		case ASN1ItemType.BOOLEAN:
			return "BOOLEAN";
		case ASN1ItemType.INTEGER:
			return "INTEGER";
		case ASN1ItemType.BIT_STRING:
			return "BIT_STRING";
		case ASN1ItemType.OCTET_STRING:
			return "OCTET_STRING";
		case ASN1ItemType.NULL:
			return "NULL";
		case ASN1ItemType.OID:
			return "OID";
		case ASN1ItemType.ENUMERATED:
			return "ENUMERATED";
		case ASN1ItemType.UTF8STRING:
			return "UTF8STRING";
		case ASN1ItemType.NUMERICSTRING:
			return "NUMERICSTRING";
		case ASN1ItemType.PRINTABLESTRING:
			return "PRINTABLESTRING";
		case ASN1ItemType.T61STRING:
			return "T61STRING";
		case ASN1ItemType.VIDEOTEXSTRING:
			return "VIDEOTEXSTRING";
		case ASN1ItemType.IA5STRING:
			return "IA5STRING";
		case ASN1ItemType.UTCTIME:
			return "UTCTIME";
		case ASN1ItemType.GENERALIZEDTIME:
			return "GENERALIZEDTIME";
		case ASN1ItemType.UNIVERSALSTRING:
			return "UNIVERSALSTRING";
		case ASN1ItemType.BMPSTRING:
			return "BMPSTRING";
		case ASN1ItemType.SEQUENCE:
			return "SEQUENCE";
		case ASN1ItemType.SET:
			return "SET";
		case ASN1ItemType.CHOICE_0:
			return "CHOICE_0";
		case ASN1ItemType.CHOICE_1:
			return "CHOICE_1";
		case ASN1ItemType.CHOICE_2:
			return "CHOICE_2";
		case ASN1ItemType.CHOICE_3:
			return "CHOICE_3";
		case ASN1ItemType.CHOICE_4:
			return "CHOICE_4";
		case ASN1ItemType.CHOICE_5:
			return "CHOICE_5";
		case ASN1ItemType.CHOICE_6:
			return "CHOICE_6";
		case ASN1ItemType.CHOICE_7:
			return "CHOICE_7";
		case ASN1ItemType.CHOICE_8:
			return "CHOICE_8";
		case ASN1ItemType.CONTEXT_SPECIFIC_0:
			return "CONTEXT_SPECIFIC_0";
		case ASN1ItemType.CONTEXT_SPECIFIC_1:
			return "CONTEXT_SPECIFIC_1";
		case ASN1ItemType.CONTEXT_SPECIFIC_2:
			return "CONTEXT_SPECIFIC_2";
		case ASN1ItemType.CONTEXT_SPECIFIC_3:
			return "CONTEXT_SPECIFIC_3";
		case ASN1ItemType.CONTEXT_SPECIFIC_4:
			return "CONTEXT_SPECIFIC_4";
		default:
			return "Unknown";
		}		
	}

	/**
	 * @param {data.ByteReader} reader
	 * @param {number} ofst
	 */
	static str2Digit(reader, ofst)
	{
		return Number.parseInt(reader.readUTF8(ofst, 2));
	}
}

class RuleContainer
{
	constructor()
	{
		this.rules = [];
		/**
		 * @type {RuleContainer | null}
		 */
		this.parent = null;
	}
}

export class ASN1Names
{
	/**
	 * @param {{ cond: number | undefined; itemType: any; condParam: any; name: any; contentFunc: any; enumVals: any; }} rule
	 */
	addRule(rule)
	{
		if (this.readContainer)
			this.readContainer.rules.push(rule);
		else
			this.rules.push(rule);
		this.anyCond();
	}
	
	constructor()
	{
		this.readContainer = null;
		this.readLev = [];
		this.readLastOID = null;
		this.readIndex = 0;
		this.rules = [];
		this.readBegin();
		this.anyCond();
	}

	readBegin()
	{
		this.readLev = [];
		this.readIndex = 0;
		this.readContainer = 0;
		this.readLastOIDLen = 0;
	}

	/**
	 * @param {any} itemType
	 * @param {any} len
	 * @param {any} reader
	 * @param {any} ofst
	 */
	readName(itemType, len, reader, ofst)
	{
		let name = this.readNameNoDef(itemType, len, reader, ofst);
		if (name)
			return name;
		return ASN1Util.itemTypeGetName(itemType);
	}

	/**
	 * @param {number} itemType
	 * @param {number} len
	 * @param {data.ByteReader} reader
	 * @param {number} ofst
	 */
	readNameNoDef(itemType, len, reader, ofst)
	{
		let anyMatch = false;
		if (itemType == ASN1ItemType.OID)
		{
			this.readLastOID = new Uint8Array(reader.getArrayBuffer(ofst, len));
		}
		if (this.readIndex == -1)
		{
			return null;
		}
		let rule;
		while (true)
		{
			if (this.readContainer)
			{
				rule = this.readContainer.rules[this.readIndex];
			}
			else
			{
				rule = this.rules[this.readIndex];
			}
			if (rule == null)
			{
				return null;
			}
			switch (rule.cond)
			{
			default:
			case RuleCond.Any:
				this.readIndex++;
				return rule.name;
			case RuleCond.TypeIsItemType:
				this.readIndex++;
				if (rule.itemType == itemType)
					return rule.name;
				break;
			case RuleCond.TypeIsTime:
				this.readIndex++;
				if (itemType == ASN1ItemType.UTCTIME || itemType == ASN1ItemType.GENERALIZEDTIME)
					return rule.name;
				break;
			case RuleCond.TypeIsString:
				this.readIndex++;
				if (itemType == ASN1ItemType.BMPSTRING ||
					itemType == ASN1ItemType.UTF8STRING ||
					itemType == ASN1ItemType.UNIVERSALSTRING ||
					itemType == ASN1ItemType.PRINTABLESTRING ||
					itemType == ASN1ItemType.T61STRING)
					return rule.name;
				break;
			case RuleCond.TypeIsOpt:
				this.readIndex++;
				if (itemType == rule.itemType + ASN1ItemType.CHOICE_0 ||
					itemType == rule.itemType + ASN1ItemType.CONTEXT_SPECIFIC_0)
					return rule.name;
				break;
			case RuleCond.LastOIDAndTypeIs:
				this.readIndex++;
				if (itemType == rule.itemType && this.readLastOID && ASN1Util.oidEqualsText(this.readLastOID, rule.condParam))
					return rule.name;
				break;
			case RuleCond.RepeatIfTypeIs:
				if (itemType == rule.itemType)
					return rule.name;
				this.readIndex++;
				break;
			case RuleCond.AllNotMatch:
				this.readIndex = 0;
				if (anyMatch)
					return rule.name;
				anyMatch = true;
				break;
			}
		}
	}

	readContainerBegin()
	{
		let rule;
		if (this.readIndex == -1)
		{
			this.readLev.push(-1);
		}
		else
		{
			if (this.readContainer)
			{
				if (this.readIndex == 0)
					rule = this.readContainer.rules[0];
				else
					rule = this.readContainer.rules[this.readIndex - 1];
			}
			else
			{
				if (this.readIndex == 0)
					rule = this.rules[0];
				else
					rule = this.rules[this.readIndex - 1];
			}
			this.readLev.push(this.readIndex);
			if (rule == null || rule.contentFunc == null)
			{
				this.readIndex = -1;
			}
			else
			{
				this.readIndex = 0;
				let container = new RuleContainer();
				container.parent = this.readContainer;
				this.readContainer = container;
				rule.contentFunc(this);
			}
		}
	}

	readContainerEnd()
	{
		if (this.readIndex == -1)
		{
			this.readIndex = this.readLev.pop();
		}
		else
		{
			this.readIndex = this.readLev.pop();
			if (this.readContainer)
			{
				this.readContainer = this.readContainer.parent;
			}
		}
	}

	anyCond()
	{
		this.currCond = RuleCond.Any;
		this.currItemType = ASN1ItemType.UNKNOWN;
		this.currCondParam = null;
		return this;
	}

	/**
	 * @param {any} itemType
	 */
	typeIs(itemType)
	{
		this.currCond = RuleCond.TypeIsItemType;
		this.currItemType = itemType;
		this.currCondParam = null;
		return this;
	}

	typeIsTime()
	{
		this.currCond = RuleCond.TypeIsTime;
		this.currItemType = ASN1ItemType.UNKNOWN;
		this.currCondParam = null;
		return this;
	}

	typeIsString()
	{
		this.currCond = RuleCond.TypeIsString;
		this.currItemType = ASN1ItemType.UNKNOWN;
		this.currCondParam = null;
		return this;
	}

	/**
	 * @param {any} index
	 */
	typeIsOpt(index)
	{
		this.currCond = RuleCond.TypeIsOpt;
		this.currItemType = index;
		this.currCondParam = null;
		return this;
	}

	/**
	 * @param {any} itemType
	 */
	repeatIfTypeIs(itemType)
	{
		this.currCond = RuleCond.RepeatIfTypeIs;
		this.currItemType = itemType;
		this.currCondParam = null;
		return this;
	}

	/**
	 * @param {any} oidText
	 * @param {any} itemType
	 */
	lastOIDAndTypeIs(oidText, itemType)
	{
		this.currCond = RuleCond.LastOIDAndTypeIs;
		this.currItemType = itemType;
		this.currCondParam = oidText;
		return this;
	}

	allNotMatch()
	{
		this.currCond = RuleCond.AllNotMatch;
		this.currItemType = ASN1ItemType.UNKNOWN;
		this.currCondParam = null;
		return this;
	}

	/**
	 * @param {any} name
	 * @param {any} contFunc
	 */
	container(name, contFunc)
	{
		let rule = {
			cond: this.currCond,
			itemType: this.currItemType,
			condParam: this.currCondParam,
			name: name,
			contentFunc: contFunc,
			enumVals: null};
		this.addRule(rule);
		return this;
	}

	/**
	 * @param {any} name
	 */
	nextValue(name)
	{
		let rule = {
			cond: this.currCond,
			itemType: this.currItemType,
			condParam: this.currCondParam,
			name: name,
			contentFunc: null,
			enumVals: null};
		this.addRule(rule);
		return this;
	}

	/**
	 * @param {any} name
	 * @param {any} enums
	 */
	enum(name, enums)
	{
		let rule = {
			cond: this.currCond,
			itemType: this.currItemType,
			condParam: this.currCondParam,
			name: name,
			contentFunc: null,
			enumVals: enums};
		this.addRule(rule);
		return this;
	}

	setCertificate()
	{
		PKIX1Explicit88.certificate(this);
		return this;
	}

	setRSAPublicKey()
	{
		PKCS1.rsaPublicKey(this);
		return this;
	}

	setRSAPrivateKey()
	{
		PKCS1.rsaPrivateKey(this);
		return this;
	}

	setPKCS7ContentInfo()
	{
		PKCS7.contentInfo(this);
		return this;
	}

	setCertificationRequest()
	{
		PKCS10.certificationRequest(this);
		return this;
	}

	setCertificateList()
	{
		PKIX1Explicit88.certificateList(this);
		return this;
	}

	setPFX()
	{
		PKCS12.pfx(this);
		return this;
	}
}

class General
{
	/**
	 * @param {ASN1Names} names
	 */
	static pbeParam(names)
	{
		names.typeIs(ASN1ItemType.OCTET_STRING).nextValue("salt");
		names.typeIs(ASN1ItemType.INTEGER).nextValue("iterations");
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static extendedValidationCertificates(names)
	{
		names.repeatIfTypeIs(ASN1ItemType.OCTET_STRING).nextValue("signedCertTimestamp");
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static attributeOutlookExpress(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("issuerAndSerialNumber", PKCS7.issuerAndSerialNumberCont);
	}
}

class InformationFramework
{
	/**
	 * @param {ASN1Names} names
	 */
	static attributeCont(names)
	{
		names.typeIs(ASN1ItemType.OID).nextValue("attrId");
	//	names.lastOIDAndTypeIs("1.2.840.113549.1.9.1", ASN1ItemType.SET).container("attrValues", AttributeEmailAddress);
	//	names.lastOIDAndTypeIs("1.2.840.113549.1.9.2", ASN1ItemType.SET).container("attrValues", AttributeUnstructuredName);
		names.lastOIDAndTypeIs("1.2.840.113549.1.9.3", ASN1ItemType.SET).container("attrValues", PKCS9.attributeContentType);
		names.lastOIDAndTypeIs("1.2.840.113549.1.9.4", ASN1ItemType.SET).container("attrValues", PKCS9.attributeMessageDigest);
		names.lastOIDAndTypeIs("1.2.840.113549.1.9.5", ASN1ItemType.SET).container("attrValues", PKCS9.attributeSigningTime);
	//	names.lastOIDAndTypeIs("1.2.840.113549.1.9.6", ASN1ItemType.SET).container("attrValues", AttributeCounterSignature);
	//	names.lastOIDAndTypeIs("1.2.840.113549.1.9.7", ASN1ItemType.SET).container("attrValues", AttributeChallengePassword);
	//	names.lastOIDAndTypeIs("1.2.840.113549.1.9.8", ASN1ItemType.SET).container("attrValues", AttributeUnstructuredAddress);
	//	names.lastOIDAndTypeIs("1.2.840.113549.1.9.9", ASN1ItemType.SET).container("attrValues", AttributeExtendedCertificateAttributes);
	
		names.lastOIDAndTypeIs("1.2.840.113549.1.9.15", ASN1ItemType.SET).container("attrValues", PKCS9.attributeSMIMECapabilities);
		names.lastOIDAndTypeIs("1.2.840.113549.1.9.16.2.11", ASN1ItemType.SET).container("attrValues", RFC8551.smimeEncryptionKeyPreference);
	
		names.lastOIDAndTypeIs("1.2.840.113549.1.9.20", ASN1ItemType.SET).container("attrValues", PKCS9.attributeFriendlyName);
		names.lastOIDAndTypeIs("1.2.840.113549.1.9.21", ASN1ItemType.SET).container("attrValues", PKCS9.attributeLocalKeyId);
		names.lastOIDAndTypeIs("1.3.6.1.4.1.311.16.4", ASN1ItemType.SET).container("attrValues", General.attributeOutlookExpress);
		
		names.typeIs(ASN1ItemType.SET).nextValue("attrValues");
	}
}

class PKCS1
{
	/**
	 * @param {ASN1Names} names
	 */
	static rsaPublicKey(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("RSAPublicKey", PKCS1.rsaPublicKeyCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static rsaPublicKeyCont(names)
	{
		names.typeIs(ASN1ItemType.INTEGER).nextValue("modulus");
		names.typeIs(ASN1ItemType.INTEGER).nextValue("publicExponent");
	}

	/**
	 * @param {ASN1Names} names
	 */
	static rsaPrivateKey(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("RSAPrivateKey", PKCS1.rsaPrivateKeyCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static rsaPrivateKeyCont(names)
	{
		let version = ["two-prime", "multi"];
		names.typeIs(ASN1ItemType.INTEGER).enum("Version", version);
		names.typeIs(ASN1ItemType.INTEGER).nextValue("modulus");
		names.typeIs(ASN1ItemType.INTEGER).nextValue("publicExponent");
		names.typeIs(ASN1ItemType.INTEGER).nextValue("privateExponent");
		names.typeIs(ASN1ItemType.INTEGER).nextValue("prime1");
		names.typeIs(ASN1ItemType.INTEGER).nextValue("prime2");
		names.typeIs(ASN1ItemType.INTEGER).nextValue("exponent1");
		names.typeIs(ASN1ItemType.INTEGER).nextValue("exponent2");
		names.typeIs(ASN1ItemType.INTEGER).nextValue("coefficient");
		names.typeIs(ASN1ItemType.SEQUENCE).container("otherPrimeInfos", PKCS1.otherPrimeInfos);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static otherPrimeInfos(names)
	{
		names.typeIs(ASN1ItemType.INTEGER).nextValue("prime");
		names.typeIs(ASN1ItemType.INTEGER).nextValue("exponent");
		names.typeIs(ASN1ItemType.INTEGER).nextValue("coefficient");
	}

	/**
	 * @param {ASN1Names} names
	 * @param {string} name
	 */
	static addDigestInfo(names, name)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container(name, PKCS1.digestInfoCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static digestInfoCont(names)
	{
		PKIX1Explicit88.addAlgorithmIdentifier(names, "digestAlgorithm");
		names.typeIs(ASN1ItemType.OCTET_STRING).nextValue("digest");
	}

}

class PKCS7
{
	/**
	 * @param {ASN1Names} names
	 * @param {string} name
	 */
	static addContentInfo(names, name)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container(name, PKCS7.contentInfoCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static contentInfo(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("ContentInfo", PKCS7.contentInfoCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static contentInfoCont(names)
	{
		names.typeIs(ASN1ItemType.OID).nextValue("content-type");
		names.lastOIDAndTypeIs("1.2.840.113549.1.7.1", ASN1ItemType.CONTEXT_SPECIFIC_0).container("pkcs7-content", PKCS7.data);
		names.lastOIDAndTypeIs("1.2.840.113549.1.7.2", ASN1ItemType.CONTEXT_SPECIFIC_0).container("pkcs7-content", PKCS7.signedData);
		names.lastOIDAndTypeIs("1.2.840.113549.1.7.3", ASN1ItemType.CONTEXT_SPECIFIC_0).container("pkcs7-content", PKCS7.envelopedData);
		names.lastOIDAndTypeIs("1.2.840.113549.1.7.4", ASN1ItemType.CONTEXT_SPECIFIC_0).container("pkcs7-content", PKCS7.signedAndEnvelopedData);
		names.lastOIDAndTypeIs("1.2.840.113549.1.7.5", ASN1ItemType.CONTEXT_SPECIFIC_0).container("pkcs7-content", PKCS7.digestedData);
		names.lastOIDAndTypeIs("1.2.840.113549.1.7.6", ASN1ItemType.CONTEXT_SPECIFIC_0).container("pkcs7-content", PKCS7.encryptedData);
		names.lastOIDAndTypeIs("1.2.840.113549.1.9.16.1.2", ASN1ItemType.CONTEXT_SPECIFIC_0).container("pkcs7-content", PKCS7.authenticatedData);
		names.nextValue("pkcs7-content"); ////////////////////////
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static data(names)
	{
		names.typeIs(ASN1ItemType.OCTET_STRING).nextValue("data");
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static signedData(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("signed-data", PKCS7.signedDataCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static signedDataCont(names)
	{
		names.typeIs(ASN1ItemType.INTEGER).nextValue("version");
		names.typeIs(ASN1ItemType.SET).container("digestAlgorithms", PKCS7.digestAlgorithmIdentifiers);
		names.typeIs(ASN1ItemType.SEQUENCE).container("contentInfo", PKCS7.contentInfoCont);
		names.typeIs(ASN1ItemType.CONTEXT_SPECIFIC_0).container("certificates", PKCS7.certificateSet);
		names.typeIs(ASN1ItemType.CONTEXT_SPECIFIC_1).container("crls", PKCS7.certificateRevocationLists);
		names.typeIs(ASN1ItemType.SET).container("signerInfos", PKCS7.signerInfos);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static digestAlgorithmIdentifiers(names)
	{
		names.repeatIfTypeIs(ASN1ItemType.SEQUENCE).container("DigestAlgorithmIdentifier", PKIX1Explicit88.algorithmIdentifierCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static certificateSet(names)
	{
		names.repeatIfTypeIs(ASN1ItemType.SEQUENCE).container("certificate", PKIX1Explicit88.certificateCont);
		names.repeatIfTypeIs(ASN1ItemType.CHOICE_0).nextValue("extendedCertificate");//, ExtendedCertificate);
		names.repeatIfTypeIs(ASN1ItemType.CHOICE_1).nextValue("attributeCertificate");//, AttributeCertificate);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static certificateRevocationLists(names)
	{
		names.repeatIfTypeIs(ASN1ItemType.SEQUENCE).nextValue("CertificateRevocationLists");//, CertificateListCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static signerInfos(names)
	{
		names.repeatIfTypeIs(ASN1ItemType.SEQUENCE).container("SignerInfo", PKCS7.signerInfoCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static signerInfoCont(names)
	{
		names.typeIs(ASN1ItemType.INTEGER).nextValue("version");
		names.typeIs(ASN1ItemType.SEQUENCE).container("signerIdentifier", PKCS7.issuerAndSerialNumberCont);
		names.typeIs(ASN1ItemType.CONTEXT_SPECIFIC_2).container("signerIdentifier", PKIX1Implicit88.subjectKeyIdentifier);
		names.typeIs(ASN1ItemType.SEQUENCE).container("digestAlgorithm", PKIX1Explicit88.algorithmIdentifierCont);
		names.typeIs(ASN1ItemType.CONTEXT_SPECIFIC_0).container("authenticatedAttributes", PKCS10.attributesCont);
		names.typeIs(ASN1ItemType.SEQUENCE).container("digestEncryptionAlgorithm", PKIX1Explicit88.algorithmIdentifierCont);
		names.typeIs(ASN1ItemType.OCTET_STRING).nextValue("encryptedDigest");
		names.typeIs(ASN1ItemType.CONTEXT_SPECIFIC_1).container("unauthenticatedAttributes", PKCS10.attributesCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static issuerAndSerialNumberCont(names)
	{
		PKIX1Explicit88.addName(names, "issuer");
		names.typeIs(ASN1ItemType.INTEGER).nextValue("serialNumber");
	}
	
	/**
	 * @param {ASN1Names} names
	 * @param {string} name
	 */
	static addDigestInfo(names, name)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container(name, PKCS7.digestInfoCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static digestInfoCont(names)
	{
		PKIX1Explicit88.addAlgorithmIdentifier(names, "digestAlgorithm");
		names.nextValue("digest"); ////////////////////////
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static envelopedData(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("enveloped-data", PKCS7.envelopedDataCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static envelopedDataCont(names)
	{
		names.typeIs(ASN1ItemType.INTEGER).nextValue("version");
		names.typeIs(ASN1ItemType.CONTEXT_SPECIFIC_0).container("originatorInfo", PKCS7.originatorInfoCont);
		names.typeIs(ASN1ItemType.SET).container("recipientInfos", PKCS7.recipientInfos);
		names.typeIs(ASN1ItemType.SEQUENCE).container("encryptedContentInfo", PKCS7.contentInfoCont);
		names.typeIs(ASN1ItemType.CONTEXT_SPECIFIC_1).container("unprotectedAttributes", PKCS10.attributesCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static originatorInfoCont(names)
	{
		names.typeIs(ASN1ItemType.CONTEXT_SPECIFIC_0).container("certificates", PKCS7.certificateSet);
		names.typeIs(ASN1ItemType.CONTEXT_SPECIFIC_1).container("crls", PKCS7.certificateRevocationLists);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static recipientInfos(names)
	{
		names.repeatIfTypeIs(ASN1ItemType.SEQUENCE).container("keyTransportRecipientInfo", PKCS7.keyTransportRecipientInfoCont);
		names.repeatIfTypeIs(ASN1ItemType.CHOICE_0).nextValue("keyAgreementRecipientInfo");//, PKCS7.keyAgreementRecipientInfo);
		names.repeatIfTypeIs(ASN1ItemType.CHOICE_1).nextValue("keyEncryptionKeyRecipientInfo");//, PKCS7.keyEncryptionKeyRecipientInfo);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static keyTransportRecipientInfoCont(names)
	{
		names.typeIs(ASN1ItemType.INTEGER).nextValue("version");
		names//.typeIs(ASN1ItemType.INTEGER)
			.nextValue("recipientIdentifier"); //PKCS7.RecipientIdentifier
		names.typeIs(ASN1ItemType.SEQUENCE).container("keyEncryptionAlgorithm", PKIX1Explicit88.algorithmIdentifierCont);
		names.typeIs(ASN1ItemType.OCTET_STRING).nextValue("encryptedKey");
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static signedAndEnvelopedData(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("signed-data", PKCS7.signedAndEnvelopedDataCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static signedAndEnvelopedDataCont(names)
	{
		names.typeIs(ASN1ItemType.INTEGER).nextValue("version");
		names.typeIs(ASN1ItemType.SET).nextValue("recipientInfos");
		names.typeIs(ASN1ItemType.SET).container("digestAlgorithms", PKCS7.digestAlgorithmIdentifiers);
		names.typeIs(ASN1ItemType.SEQUENCE).container("contentInfo", PKCS7.contentInfoCont);
		names.typeIs(ASN1ItemType.CONTEXT_SPECIFIC_0).container("certificates", PKCS7.certificateSet);
		names.typeIs(ASN1ItemType.CONTEXT_SPECIFIC_1).container("crls", PKCS7.certificateRevocationLists);
		names.typeIs(ASN1ItemType.SET).container("signerInfos", PKCS7.signerInfos);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static digestedData(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("signed-data", PKCS7.digestedDataCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static digestedDataCont(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("signed-data", PKCS7.digestedDataCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static encryptedData(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("encrypted-data", PKCS7.encryptedDataCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static encryptedDataCont(names)
	{
		names.typeIs(ASN1ItemType.INTEGER).nextValue("version");
		names.typeIs(ASN1ItemType.SEQUENCE).container("encryptedContentInfo", PKCS7.encryptedContentInfoCont);
		names.typeIs(ASN1ItemType.CONTEXT_SPECIFIC_1).container("unprotectedAttributes", PKCS10.attributesCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static encryptedContentInfoCont(names)
	{
		names.typeIs(ASN1ItemType.OID).nextValue("contentType");
		names.typeIs(ASN1ItemType.SEQUENCE).container("contentEncryptionAlgorithm", PKIX1Explicit88.algorithmIdentifierCont);
		names.typeIs(ASN1ItemType.CHOICE_0).nextValue("encryptedContent");
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static authenticatedData(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("signed-data", PKCS7.authenticatedData);
	}
}

class PKCS8
{
	/**
	 * @param {ASN1Names} names
	 */
	static privateKeyInfo(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("PrivateKeyInfo", PKCS8.privateKeyInfoCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static privateKeyInfoCont(names)
	{
		names.typeIs(ASN1ItemType.INTEGER).nextValue("version");
		names.typeIs(ASN1ItemType.SEQUENCE).container("privateKeyAlgorithm", PKIX1Explicit88.algorithmIdentifierCont);
		names.typeIs(ASN1ItemType.OCTET_STRING).nextValue("privateKey");
		names.typeIs(ASN1ItemType.CONTEXT_SPECIFIC_0).container("attributes", PKCS10.attributesCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static encryptedPrivateKeyInfo(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("EncryptedPrivateKeyInfo", PKCS8.encryptedPrivateKeyInfoCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static encryptedPrivateKeyInfoCont(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("encryptionAlgorithm", PKIX1Explicit88.algorithmIdentifierCont);
		names.typeIs(ASN1ItemType.OCTET_STRING).nextValue("encryptedData");
	}
}

class PKCS9
{
	/**
	 * @param {ASN1Names} names
	 */
	static attributeContentType(names)
	{
		names.typeIs(ASN1ItemType.OID).nextValue("contentType");
	}

	/**
	 * @param {ASN1Names} names
	 */
	static attributeMessageDigest(names)
	{
		names.typeIs(ASN1ItemType.OCTET_STRING).nextValue("messageDigest");
	}

	/**
	 * @param {ASN1Names} names
	 */
	static attributeSigningTime(names)
	{
		names.typeIsTime().nextValue("signingTime");
	}

	/**
	 * @param {ASN1Names} names
	 */
	static attributeFriendlyName(names)
	{
		names.typeIs(ASN1ItemType.BMPSTRING).nextValue("friendlyName");
	}

	/**
	 * @param {ASN1Names} names
	 */
	static attributeSMIMECapabilities(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("smimeCapabilities", PKCS9.smimeCapabilitiesCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static attributeLocalKeyId(names)
	{
		names.typeIs(ASN1ItemType.OCTET_STRING).nextValue("localKeyId");
	}

	/**
	 * @param {ASN1Names} names
	 */
	static smimeCapabilitiesCont(names)
	{
		names.repeatIfTypeIs(ASN1ItemType.SEQUENCE).container("SMIMECapability", PKCS9.smimeCapabilityCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static smimeCapabilityCont(names)
	{
		names.typeIs(ASN1ItemType.OID).nextValue("algorithm");
		names.nextValue("parameters");
	}
}

class PKCS10
{
	/**
	 * @param {ASN1Names} names
	 * @param {string} name
	 */
	static addCertificationRequestInfo(names, name)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container(name, PKCS10.certificationRequestInfoCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static certificationRequestInfoCont(names)
	{
		names.typeIs(ASN1ItemType.INTEGER).nextValue("version");
		PKIX1Explicit88.addName(names, "subject");
		PKIX1Explicit88.addSubjectPublicKeyInfo(names, "subjectPKInfo");
		names.typeIs(ASN1ItemType.CONTEXT_SPECIFIC_0).container("attributes", PKCS10.attributesCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static attributesCont(names)
	{
		names.repeatIfTypeIs(ASN1ItemType.SEQUENCE).container("Attribute", InformationFramework.attributeCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static certificationRequest(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("CertificationRequest", PKCS10.certificationRequestCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static certificationRequestCont(names)
	{
		PKCS10.addCertificationRequestInfo(names, "certificationRequestInfo");
		PKIX1Explicit88.addAlgorithmIdentifier(names, "signatureAlgorithm");
		names.typeIs(ASN1ItemType.BIT_STRING).nextValue("signature");
	}
}

class PKCS12
{
	/**
	 * @param {ASN1Names} names
	 */
	static pfx(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("PFX", PKCS12.pfxCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static pfxCont(names)
	{
		names.typeIs(ASN1ItemType.INTEGER).nextValue("Version");
		names.typeIs(ASN1ItemType.SEQUENCE).container("authSafe", PKCS12.authenticatedSafeContentInfoCont);
		PKCS12.addMacData(names, "macData");
	}
	
	/**
	 * @param {ASN1Names} names
	 * @param {string} name
	 */
	static addMacData(names, name)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container(name, PKCS12.macDataCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static macDataCont(names)
	{
		PKCS7.addDigestInfo(names, "mac");
		names.typeIs(ASN1ItemType.OCTET_STRING).nextValue("macSalt");
		names.typeIs(ASN1ItemType.INTEGER).nextValue("iterations");
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static authenticatedSafeContentInfoCont(names)
	{
		names.typeIs(ASN1ItemType.OID).nextValue("content-type");
		names.lastOIDAndTypeIs("1.2.840.113549.1.7.1", ASN1ItemType.CONTEXT_SPECIFIC_0).container("pkcs7-content", PKCS12.authenticatedSafeData);
		names.lastOIDAndTypeIs("1.2.840.113549.1.7.3", ASN1ItemType.CONTEXT_SPECIFIC_0).container("pkcs7-content", PKCS12.authenticatedSafeEnvelopedData);
		names.lastOIDAndTypeIs("1.2.840.113549.1.7.6", ASN1ItemType.CONTEXT_SPECIFIC_0).container("pkcs7-content", PKCS12.authenticatedSafeEncryptedData);
		names.nextValue("pkcs7-content"); ////////////////////////
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static authenticatedSafeData(names)
	{
		names.typeIs(ASN1ItemType.OCTET_STRING).container("data", PKCS12.authenticatedSafe);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static authenticatedSafeEnvelopedData(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("signed-data", PKCS7.envelopedDataCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static authenticatedSafeEncryptedData(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("encrypted-data", PKCS7.encryptedDataCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static authenticatedSafe(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("AuthenticatedSafe", PKCS12.authSafeContentInfo);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static authSafeContentInfo(names)
	{
		names.repeatIfTypeIs(ASN1ItemType.SEQUENCE).container("ContentInfo", PKCS12.authSafeContentInfoCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static authSafeContentInfoCont(names)
	{
		names.typeIs(ASN1ItemType.OID).nextValue("content-type");
		names.lastOIDAndTypeIs("1.2.840.113549.1.7.1", ASN1ItemType.CONTEXT_SPECIFIC_0).container("pkcs7-content", PKCS12.safeContentsData);
		names.lastOIDAndTypeIs("1.2.840.113549.1.7.2", ASN1ItemType.CONTEXT_SPECIFIC_0).container("pkcs7-content", PKCS7.signedData);
		names.lastOIDAndTypeIs("1.2.840.113549.1.7.3", ASN1ItemType.CONTEXT_SPECIFIC_0).container("pkcs7-content", PKCS7.envelopedData);
		names.lastOIDAndTypeIs("1.2.840.113549.1.7.4", ASN1ItemType.CONTEXT_SPECIFIC_0).container("pkcs7-content", PKCS7.signedAndEnvelopedData);
		names.lastOIDAndTypeIs("1.2.840.113549.1.7.5", ASN1ItemType.CONTEXT_SPECIFIC_0).container("pkcs7-content", PKCS7.digestedData);
		names.lastOIDAndTypeIs("1.2.840.113549.1.7.6", ASN1ItemType.CONTEXT_SPECIFIC_0).container("pkcs7-content", PKCS7.encryptedData);
		names.lastOIDAndTypeIs("1.2.840.113549.1.9.16.1.2", ASN1ItemType.CONTEXT_SPECIFIC_0).container("pkcs7-content", PKCS7.authenticatedData);
		names.nextValue("pkcs7-content"); ////////////////////////
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static safeContentsData(names)
	{
		names.typeIs(ASN1ItemType.OCTET_STRING).container("data", PKCS12.safeContents);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static safeContents(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("SafeContents", PKCS12.safeContentsCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static safeContentsCont(names)
	{
		names.repeatIfTypeIs(ASN1ItemType.SEQUENCE).container("SafeBag", PKCS12.safeBagCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static safeBagCont(names)
	{
		names.typeIs(ASN1ItemType.OID).nextValue("bagId");
		names.lastOIDAndTypeIs("1.2.840.113549.1.12.10.1.1", ASN1ItemType.CONTEXT_SPECIFIC_0).container("keyBag", PKCS8.privateKeyInfo);
		names.lastOIDAndTypeIs("1.2.840.113549.1.12.10.1.2", ASN1ItemType.CONTEXT_SPECIFIC_0).container("pkcs8ShroudedKeyBag", PKCS8.encryptedPrivateKeyInfo);
		names.lastOIDAndTypeIs("1.2.840.113549.1.12.10.1.3", ASN1ItemType.CONTEXT_SPECIFIC_0).container("certBag", PKCS12.certBag);
		names.lastOIDAndTypeIs("1.2.840.113549.1.12.10.1.4", ASN1ItemType.CONTEXT_SPECIFIC_0).container("crlBag", PKCS12.crlBag);
		names.lastOIDAndTypeIs("1.2.840.113549.1.12.10.1.5", ASN1ItemType.CONTEXT_SPECIFIC_0).container("secretBag", PKCS12.secretBag);
		names.lastOIDAndTypeIs("1.2.840.113549.1.12.10.1.6", ASN1ItemType.CONTEXT_SPECIFIC_0).container("safeContentsBag", PKCS12.safeContents);
		names.typeIs(ASN1ItemType.SET).container("bagAttributes", PKCS12.pkcs12Attributes);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static certBag(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("CertBag", PKCS12.certBagCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static certBagCont(names)
	{
		names.typeIs(ASN1ItemType.OID).nextValue("certId");
		names.lastOIDAndTypeIs("1.2.840.113549.1.9.22.1", ASN1ItemType.CONTEXT_SPECIFIC_0).container("certValue", PKCS12.x509Certificate);
		names.lastOIDAndTypeIs("1.2.840.113549.1.9.22.2", ASN1ItemType.CONTEXT_SPECIFIC_0).container("certValue", PKCS12.sdsiCertificate);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static x509Certificate(names)
	{
		names.typeIs(ASN1ItemType.OCTET_STRING).container("x509Certificate", PKIX1Explicit88.certificate);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static sdsiCertificate(names)
	{
		names.typeIs(ASN1ItemType.IA5STRING).nextValue("sdsiCertificate");
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static crlBag(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("CRLBag", PKCS12.crlBagCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static crlBagCont(names)
	{
		names.typeIs(ASN1ItemType.OID).nextValue("crlId");
		names.lastOIDAndTypeIs("1.2.840.113549.1.9.23.1", ASN1ItemType.CONTEXT_SPECIFIC_0).container("crlValue", PKCS12.x509CRL);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static x509CRL(names)
	{
		names.typeIs(ASN1ItemType.OCTET_STRING).container("x509CRL", PKIX1Explicit88.certificateList);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static secretBag(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("SecretBag", PKCS12.secretBagCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static secretBagCont(names)
	{
		names.typeIs(ASN1ItemType.OID).nextValue("secretTypeId");
		names.typeIs(ASN1ItemType.CONTEXT_SPECIFIC_0).nextValue("secretValue");
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static pkcs12Attributes(names)
	{
		names.repeatIfTypeIs(ASN1ItemType.SEQUENCE).container("PKCS12Attribute", InformationFramework.attributeCont);
	}
}

class PKIX1Explicit88
{
	/**
	 * @param {ASN1Names} names
	 * @param {string} name
	 */
	static addAttributeTypeAndValue(names, name)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container(name, PKIX1Explicit88.attributeTypeAndValueCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static attributeTypeAndValueCont(names)
	{
		names.typeIs(ASN1ItemType.OID).nextValue("type");
		names.nextValue("value");
	}

	/**
	 * @param {ASN1Names} names
	 * @param {string} name
	 */
	static addName(names, name)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container(name, PKIX1Explicit88.rdnSequenceCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static rdnSequenceCont(names)
	{
		names.repeatIfTypeIs(ASN1ItemType.SET).container("rdnSequence", PKIX1Explicit88.relativeDistinguishedNameCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static relativeDistinguishedName(names)
	{
		names.typeIs(ASN1ItemType.SET).container("RelativeDistinguishedName", PKIX1Explicit88.relativeDistinguishedNameCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static relativeDistinguishedNameCont(names)
	{
		PKIX1Explicit88.addAttributeTypeAndValue(names, "AttributeTypeAndValue");
	}

	/**
	 * @param {ASN1Names} names
	 */
	static certificate(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("Certificate", PKIX1Explicit88.certificateCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static certificateCont(names)
	{
		PKIX1Explicit88.addTBSCertificate(names, "tbsCertificate");
		PKIX1Explicit88.addAlgorithmIdentifier(names, "signatureAlgorithm");
		names.typeIs(ASN1ItemType.BIT_STRING).nextValue("signature");
	}

	/**
	 * @param {ASN1Names} names
	 * @param {string} name
	 */
	static addTBSCertificate(names, name)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container(name, PKIX1Explicit88.tbsCertificateCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static tbsCertificateCont(names)
	{
		names.typeIs(ASN1ItemType.CONTEXT_SPECIFIC_0).container("version", PKIX1Explicit88.version);
		names.typeIs(ASN1ItemType.INTEGER).nextValue("serialNumber");
		PKIX1Explicit88.addAlgorithmIdentifier(names, "signature");
		PKIX1Explicit88.addName(names, "issuer");
		PKIX1Explicit88.addValidity(names, "validity");
		PKIX1Explicit88.addName(names, "subject");
		PKIX1Explicit88.addSubjectPublicKeyInfo(names, "subjectPublicKeyInfo");
		names.typeIs(ASN1ItemType.CONTEXT_SPECIFIC_1).nextValue("issuerUniqueID");/////////////////////
		names.typeIs(ASN1ItemType.CONTEXT_SPECIFIC_2).nextValue("subjectUniqueID");//////////////////////
		names.typeIs(ASN1ItemType.CONTEXT_SPECIFIC_3).container("extensions", PKIX1Explicit88.extensions);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static version(names)
	{
		let versions = ["v1", "v2", "v3"];
		names.typeIs(ASN1ItemType.INTEGER).enum("Version", versions);
	}

	/**
	 * @param {ASN1Names} names
	 * @param {string} name
	 */
	static addValidity(names, name)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container(name, PKIX1Explicit88.validityCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static validityCont(names)
	{
		names.typeIsTime().nextValue("notBefore");
		names.typeIsTime().nextValue("notAfter");
	}

	/**
	 * @param {ASN1Names} names
	 * @param {string} name
	 */
	static addSubjectPublicKeyInfo(names, name)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container(name, PKIX1Explicit88.subjectPublicKeyInfoCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static subjectPublicKeyInfoCont(names)
	{
		PKIX1Explicit88.addAlgorithmIdentifier(names, "algorithm");
		names.lastOIDAndTypeIs("1.2.840.113549.1.1.1", ASN1ItemType.BIT_STRING).container("subjectPublicKey", PKCS1.rsaPublicKey);
		names.typeIs(ASN1ItemType.BIT_STRING).nextValue("subjectPublicKey");
	}

	/**
	 * @param {ASN1Names} names
	 * @param {string} name
	 */
	static addExtensions(names, name)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container(name, PKIX1Explicit88.extensionsCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static extensions(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("Extensions", PKIX1Explicit88.extensionsCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static extensionsCont(names)
	{
		names.repeatIfTypeIs(ASN1ItemType.SEQUENCE).container("Extension", PKIX1Explicit88.extensionCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static extensionCont(names)
	{
		names.typeIs(ASN1ItemType.OID).nextValue("extnID");
		names.typeIs(ASN1ItemType.BOOLEAN).nextValue("critical");
		names.lastOIDAndTypeIs("1.3.6.1.4.1.11129.2.4.2", ASN1ItemType.OCTET_STRING).container("extendedValidationCertificates", General.extendedValidationCertificates);
		names.lastOIDAndTypeIs("1.3.6.1.5.5.7.1.1", ASN1ItemType.OCTET_STRING).container("authorityInfoAccess", RFC2459.authorityInfoAccessSyntax);
		names.lastOIDAndTypeIs("2.5.29.14", ASN1ItemType.OCTET_STRING).container("subjectKeyIdentifier", PKIX1Implicit88.subjectKeyIdentifier);
		names.lastOIDAndTypeIs("2.5.29.15", ASN1ItemType.OCTET_STRING).container("keyUsage", PKIX1Implicit88.keyUsage);
		names.lastOIDAndTypeIs("2.5.29.17", ASN1ItemType.OCTET_STRING).container("subjectAltName", PKIX1Implicit88.generalNames);
		names.lastOIDAndTypeIs("2.5.29.19", ASN1ItemType.OCTET_STRING).container("basicConstraints", PKIX1Implicit88.basicConstraints);
		names.lastOIDAndTypeIs("2.5.29.31", ASN1ItemType.OCTET_STRING).container("cRLDistributionPoints", PKIX1Implicit88.crlDistributionPoints);
		names.lastOIDAndTypeIs("2.5.29.32", ASN1ItemType.OCTET_STRING).container("certificatePolicies", PKIX1Implicit88.certificatePolicies);
		names.lastOIDAndTypeIs("2.5.29.35", ASN1ItemType.OCTET_STRING).container("authorityKeyIdentifier", PKIX1Implicit88.authorityKeyIdentifier);
		names.lastOIDAndTypeIs("2.5.29.37", ASN1ItemType.OCTET_STRING).container("extKeyUsage", PKIX1Implicit88.extKeyUsageSyntax);
		names.nextValue("extnValue");//////////////////////////////
	}

	/**
	 * @param {ASN1Names} names
	 */
	static certificateList(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("CertificateList", PKIX1Explicit88.certificateListCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static certificateListCont(names)
	{
		PKIX1Explicit88.addTBSCertList(names, "tbsCertList");
		PKIX1Explicit88.addAlgorithmIdentifier(names, "signatureAlgorithm");
		names.typeIs(ASN1ItemType.BIT_STRING).nextValue("signature");
	}

	/**
	 * @param {ASN1Names} names
	 * @param {string} name
	 */
	static addTBSCertList(names, name)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container(name, PKIX1Explicit88.tbsCertListCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static tbsCertListCont(names)
	{
		names.typeIs(ASN1ItemType.INTEGER).nextValue("Version");
		PKIX1Explicit88.addAlgorithmIdentifier(names, "signature");
		PKIX1Explicit88.addName(names, "issuer");
		names.typeIsTime().nextValue("thisUpdate");
		names.typeIsTime().nextValue("nextUpdate");
		names.typeIs(ASN1ItemType.SEQUENCE).container("revokedCertificates", PKIX1Explicit88.revokedCertificates);
		names.typeIs(ASN1ItemType.CONTEXT_SPECIFIC_0).container("crlExtensions", PKIX1Explicit88.extensions);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static revokedCertificates(names)
	{
		names.repeatIfTypeIs(ASN1ItemType.SEQUENCE).container("revokedCertificate", PKIX1Explicit88.revokedCertificateCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static revokedCertificateCont(names)
	{
		names.typeIs(ASN1ItemType.INTEGER).nextValue("userCertificate");
		names.typeIsTime().nextValue("revocationDate");
		names.typeIs(ASN1ItemType.SEQUENCE).container("crlEntryExtensions", PKIX1Explicit88.extensionsCont);
	}

	/**
	 * @param {ASN1Names} names
	 * @param {string} name
	 */
	static addAlgorithmIdentifier(names, name)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container(name, PKIX1Explicit88.algorithmIdentifierCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static algorithmIdentifierCont(names)
	{
		names.typeIs(ASN1ItemType.OID).nextValue("algorithm");
		names.lastOIDAndTypeIs("1.2.840.113549.1.12.1.3", ASN1ItemType.SEQUENCE).container("parameters", General.pbeParam); //pbeWithSHAAnd3-KeyTripleDES-CBC
		names.lastOIDAndTypeIs("1.2.840.113549.1.12.1.6", ASN1ItemType.SEQUENCE).container("parameters", General.pbeParam); //pbeWithSHAAnd40BitRC2-CBC
		names.nextValue("parameters");
	}
}

class PKIX1Implicit88
{
	/**
	 * @param {ASN1Names} names
	 */
	static authorityKeyIdentifier(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("AuthorityKeyIdentifier", PKIX1Implicit88.authorityKeyIdentifierCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static authorityKeyIdentifierCont(names)
	{
		names.typeIsOpt(0).nextValue("keyIdentifier");
		names.typeIsOpt(1).container("authorityCertIssuer", PKIX1Implicit88.generalNameCont);
		names.typeIsOpt(2).nextValue("authorityCertSerialNumber");
	}

	/**
	 * @param {ASN1Names} names
	 */
	static subjectKeyIdentifier(names)
	{
		names.typeIs(ASN1ItemType.OCTET_STRING).nextValue("SubjectKeyIdentifier");
	}

	/**
	 * @param {ASN1Names} names
	 */
	static keyUsage(names)
	{
		names.typeIs(ASN1ItemType.BIT_STRING).nextValue("KeyUsage");
	}

	/**
	 * @param {ASN1Names} names
	 */
	static certificatePolicies(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("CertificatePolicies", PKIX1Implicit88.certificatePoliciesCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static certificatePoliciesCont(names)
	{
		names.repeatIfTypeIs(ASN1ItemType.SEQUENCE).container("PolicyInformation", PKIX1Implicit88.policyInformationCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static policyInformationCont(names)
	{
		names.typeIs(ASN1ItemType.OID).nextValue("policyIdentifier");
		names.typeIs(ASN1ItemType.SEQUENCE).container("policyQualifiers", PKIX1Implicit88.policyQualifiers);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static policyQualifiers(names)
	{
		names.repeatIfTypeIs(ASN1ItemType.SEQUENCE).container("PolicyQualifierInfo", PKIX1Implicit88.policyQualifierInfoCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static policyQualifierInfoCont(names)
	{
		names.typeIs(ASN1ItemType.OID).nextValue("policyQualifierId");
		names.nextValue("qualifier");
	}

	/**
	 * @param {ASN1Names} names
	 */
	static generalNames(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("GeneralName", PKIX1Implicit88.generalNameCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static generalNameCont(names)
	{
		names.typeIsOpt(0).nextValue("otherName");
		names.typeIsOpt(1).nextValue("rfc822Name");
		names.typeIsOpt(2).nextValue("dNSName");
		names.typeIsOpt(3).nextValue("x400Address");
		names.typeIsOpt(4).container("directoryName", PKIX1Explicit88.name);
		names.typeIsOpt(5).nextValue("ediPartyName");
		names.typeIsOpt(6).nextValue("uniformResourceIdentifier");
		names.typeIsOpt(7).nextValue("iPAddress");
		names.typeIsOpt(8).nextValue("registeredID");
		names.allNotMatch().nextValue("unknown");
	}

	/**
	 * @param {ASN1Names} names
	 */
	static basicConstraints(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("BasicConstraints", PKIX1Implicit88.basicConstraintsCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static basicConstraintsCont(names)
	{
		names.typeIs(ASN1ItemType.BOOLEAN).nextValue("cA");
		names.typeIs(ASN1ItemType.INTEGER).nextValue("pathLenConstraint");
	}

	/**
	 * @param {ASN1Names} names
	 */
	static crlDistributionPoints(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("CRLDistributionPoints", PKIX1Implicit88.crlDistributionPointsCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static crlDistributionPointsCont(names)
	{
		names.repeatIfTypeIs(ASN1ItemType.SEQUENCE).container("DistributionPoint", PKIX1Implicit88.distributionPointCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static distributionPointCont(names)
	{
		names.typeIsOpt(0).container("distributionPoint", PKIX1Implicit88.distributionPointName);
		names.typeIsOpt(1).container("reasons", PKIX1Implicit88.reasonFlags);
		names.typeIsOpt(2).container("cRLIssuer", PKIX1Implicit88.generalNames);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static distributionPointName(names)
	{
		names.typeIsOpt(0).container("fullName", PKIX1Implicit88.generalNameCont);
		names.typeIsOpt(1).container("nameRelativeToCRLIssuer", PKIX1Explicit88.relativeDistinguishedNameCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static reasonFlags(names)
	{
		names.typeIs(ASN1ItemType.BIT_STRING).nextValue("ReasonFlags");
	}

	/**
	 * @param {ASN1Names} names
	 */
	static extKeyUsageSyntax(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("ExtKeyUsageSyntax", PKIX1Implicit88.extKeyUsageSyntaxCont);
	}

	/**
	 * @param {ASN1Names} names
	 */
	static extKeyUsageSyntaxCont(names)
	{
		names.repeatIfTypeIs(ASN1ItemType.OID).nextValue("KeyPurposeId");
	}
}

class RFC2459
{
	/**
	 * @param {ASN1Names} names
	 */
	static authorityInfoAccessSyntax(names)
	{
		names.typeIs(ASN1ItemType.SEQUENCE).container("AuthorityInfoAccessSyntax", RFC2459.authorityInfoAccessSyntaxCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static authorityInfoAccessSyntaxCont(names)
	{
		names.repeatIfTypeIs(ASN1ItemType.SEQUENCE).container("AccessDescription", RFC2459.accessDescriptionCont);
	}
	
	/**
	 * @param {ASN1Names} names
	 */
	static accessDescriptionCont(names)
	{
		names.typeIs(ASN1ItemType.OID).nextValue("accessMethod");
		names.nextValue("accessLocation");
	}
}

class RFC8551
{
	/**
	 * @param {ASN1Names} names
	 */
	static smimeEncryptionKeyPreference(names)
	{
		names.typeIs(ASN1ItemType.CONTEXT_SPECIFIC_0).container("issuerAndSerialNumber", PKCS7.issuerAndSerialNumberCont);
		names.typeIs(ASN1ItemType.CONTEXT_SPECIFIC_1).nextValue("receipentKeyId");//, RecipientKeyIdentifierCont);
		names.typeIs(ASN1ItemType.CONTEXT_SPECIFIC_2).nextValue("subjectAltKeyIdentifier");//, SubjectKeyIdentifierCont);
	}
}

export class ASN1PDUBuilder
{
	constructor()
	{
		this.seqOffset = [];
		this.buff = [];
	}
	
	/**
	 * @param {number} type
	 */
	beginOther(type)
	{
		this.buff.push(type);
		this.buff.push(0);
		this.seqOffset.push(this.buff.length);
	}

	beginSequence()
	{
		this.beginOther(0x30);
	}

	beginSet()
	{
		this.beginOther(0x31);
	}

	/**
	 * @param {number} n
	 */
	beginContentSpecific(n)
	{
		this.beginOther(0xa0 + n);
	}

	endLevel()
	{
		if (this.seqOffset.length > 0)
		{
			let seqOffset = this.seqOffset.pop();
			let seqLen = this.buff.length - seqOffset;
			if (seqLen < 128)
			{
				this.buff[seqOffset - 1] = seqLen;
			}
			else if (seqLen < 256)
			{
				this.buff[seqOffset - 1] = 0x81;
				this.buff.splice(seqOffset, 0, seqLen);
			}
			else if (seqLen < 65536)
			{
				this.buff[seqOffset - 1] = 0x82;
				this.buff.splice(seqOffset, 0, seqLen >> 8, seqLen & 0xff);
			}
			else
			{
				this.buff[seqOffset - 1] = 0x83;
				this.buff.splice(seqOffset, 0, seqLen >> 16, (seqLen >> 8) & 0xff, seqLen & 0xff);
			}
		}	
	}

	endAll()
	{
		while (this.seqOffset.length > 0)
		{
			this.endLevel();
		}
	}

	/**
	 * @param {boolean} v
	 */
	appendBool(v)
	{
		this.buff.push(1);
		this.buff.push(1);
		this.buff.push(v?0xFF:0);
	}

	/**
	 * @param {number} v
	 */
	appendInt32(v)
	{
		this.buff.push(2);
		if (v < 128 && v >= -128)
		{
			this.buff.push(1);
			this.buff.push(v & 0xff);
		}
		else if (v < 32768 && v >= -32768)
		{
			this.buff.push(2);
			this.buff.push((v >> 8) & 0xff);
			this.buff.push(v & 0xff);
		}
		else if (v < 8388608 && v >= -8388608)
		{
			this.buff.push(3);
			this.buff.push((v >> 16) & 0xff);
			this.buff.push((v >> 8) & 0xff);
			this.buff.push(v & 0xff);
		}
		else
		{
			this.buff.push(4);
			this.buff.push((v >> 24) & 0xff);
			this.buff.push((v >> 16) & 0xff);
			this.buff.push((v >> 8) & 0xff);
			this.buff.push(v & 0xff);
		}
	}

	/**
	 * @param {number} bitLeft
	 * @param {ArrayBuffer} buff
	 */
	appendBitString(bitLeft, buff)
	{
		this.appendTypeLen(3, buff.byteLength + 1);
		this.buff.push(bitLeft);
		this.appendArrayBuffer(buff);
	}

	/**
	 * @param {string | ArrayBuffer} buff
	 */
	appendOctetString(buff)
	{
		if (buff instanceof ArrayBuffer)
		{
			this.appendTypeLen(4, buff.byteLength);
			this.appendArrayBuffer(buff);
		}
		else if (typeof buff == "string")
		{
			let b = new TextEncoder().encode(buff);
			this.appendTypeLen(4, b.byteLength);
			this.appendArrayBuffer(b);
		}
		else
		{
			throw new Error("Unknown type");
		}
	}

	appendNull()
	{
		this.appendTypeLen(5, 0);
	}

	/**
	 * @param {ArrayBufferLike} buff
	 */
	appendOID(buff)
	{
		this.appendTypeLen(6, buff.byteLength);
		this.appendArrayBuffer(buff);
	}

	/**
	 * @param {string} oidStr
	 */
	appendOIDString(oidStr)
	{
		let buff = ASN1Util.oidText2PDU(oidStr);
		if (buff == null)
			throw new Error("\""+oidStr+"\" is not valid oid string");
		this.appendOID(new Uint8Array(buff).buffer);
	}

	/**
	 * @param {number} v
	 */
	appendChoice(v)
	{
		this.buff.push(10);
		if (v < 128 && v >= -128)
		{
			this.buff.push(1);
			this.buff.push(v & 0xff);
		}
		else if (v < 32768 && v >= -32768)
		{
			this.buff.push(2);
			this.buff.push((v >> 8) & 0xff);
			this.buff.push(v & 0xff);
		}
		else if (v < 8388608 && v >= -8388608)
		{
			this.buff.push(3);
			this.buff.push((v >> 16) & 0xff);
			this.buff.push((v >> 8) & 0xff);
			this.buff.push(v & 0xff);
		}
		else
		{
			this.buff.push(4);
			this.buff.push((v >> 24) & 0xff);
			this.buff.push((v >> 16) & 0xff);
			this.buff.push((v >> 8) & 0xff);
			this.buff.push(v & 0xff);
		}
	}

	/**
	 * @param {string} s
	 */
	appendPrintableString(s)
	{
		this.appendOther(0x13, new TextEncoder().encode(s).buffer);
	}

	/**
	 * @param {string} s
	 */
	appendUTF8String(s)
	{
		this.appendOther(0xc, new TextEncoder().encode(s).buffer);
	}

	/**
	 * @param {string} s
	 */
	appendIA5String(s)
	{
		this.appendOther(0x16, new TextEncoder().encode(s).buffer);
	}

	/**
	 * @param {data.Timestamp} t
	 */
	appendUTCTime(t)
	{
		let s = t.toUTCTime().toString("yyMMddHHmmss")+"Z";
		this.appendOther(0x17, new TextEncoder().encode(s).buffer);
	}

	/**
	 * @param {number} type
	 * @param {ArrayBufferLike} buff
	 */
	appendOther(type, buff)
	{
		this.appendTypeLen(type, buff.byteLength);
		this.appendArrayBuffer(buff);
	}

	/**
	 * @param {number} n
	 * @param {ArrayBuffer} buff
	 */
	appendContentSpecific(n, buff)
	{
		this.appendOther(0xa0 + n, buff);
	}

	/**
	 * @param {ArrayBuffer} buff
	 */
	appendSequence(buff)
	{
		this.appendOther(0x30, buff);
	}

	/**
	 * @param {ArrayBuffer} buff
	 */
	appendInteger(buff)
	{
		this.appendOther(2, buff);
	}

	getArrayBuffer()
	{
		this.endAll();
		return new Uint8Array(this.buff).buffer;
	}

	/**
	 * @param {number} type
	 * @param {number} len
	 */
	appendTypeLen(type, len)
	{
		this.buff.push(type);
		if (len < 128)
		{
			this.buff.push(len & 0xff);
		}
		else if (len < 256)
		{
			this.buff.push(0x81);
			this.buff.push(len);
		}
		else if (len < 65536)
		{
			this.buff.push(0x82);
			this.buff.push(len >> 8);
			this.buff.push(len & 0xff);
		}
		else if (len < 0x1000000)
		{
			this.buff.push(0x83);
			this.buff.push(len >> 16);
			this.buff.push((len >> 8) & 0xff);
			this.buff.push(len & 0xff);
		}
		else
		{
			this.buff.push(0x84);
			this.buff.push(len >> 24);
			this.buff.push((len >> 16) & 0xff);
			this.buff.push((len >> 8) & 0xff);
			this.buff.push(len & 0xff);
		}
	}

	/**
	 * @param {ArrayBuffer} buff
	 */
	appendArrayBuffer(buff)
	{
		let arr = new Uint8Array(buff);
		let i = 0;
		while (i < arr.length)
		{
			this.buff.push(arr[i]);
			i++;
		}
	}
}
