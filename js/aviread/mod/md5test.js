import * as data from "/js/@sswroom/sswr/data.js";
import * as hash from "/js/@sswroom/sswr/hash.js";
import * as text from "/js/@sswroom/sswr/text.js";

let ret = [];
let testMsg = "The quick brown fox jumps over the lazy dog";
let testBlock = new text.UTF8TextBinEnc().decodeBin(testMsg).buffer;
let md5 = new hash.MD5();
ret.push(text.u8Arr2Hex(new Uint8Array(md5.getValue())));
md5.calc(testBlock);
ret.push(text.u8Arr2Hex(new Uint8Array(md5.getValue())));
md5.calc(testBlock);
ret.push(text.u8Arr2Hex(new Uint8Array(md5.getValue())));
md5.calc(testBlock);
ret.push(text.u8Arr2Hex(new Uint8Array(md5.getValue())));
ret.push("valid = "+(ret[0] == "d41d8cd98f00b204e9800998ecf8427e") && (ret[1] == "9e107d9d372bb6826bd81d3542a419d6") && (ret[2] == "d27c6d8bcaa695e377d32387e115763c") && (ret[3] == "4e67db4a7a406b0cfdadd887cde7888e"))
document.getElementById("result").innerHTML = ret.join("<br/>");