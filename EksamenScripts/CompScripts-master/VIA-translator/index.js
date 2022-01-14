
var n = 14,
    m = 12,
    P = 64,
    TLB = 4

var p = Math.log2(P),
    TLBINumber = Math.ceil(Math.log2(TLB))

var TLBTNumber = n-p-TLBINumber

var hexAddr = "0x0040"


function getPartOfBinary(v, start, end) {
    var newPart = "";
    for (var i = start; i < end; i++) {
        newPart += v[i];
    }
    return newPart;
}

function toBinary(hex, length) {
    return parseInt(hex.replace('0x', ''), 16).toString(2).padStart(length, '0')
}   

function toHex(binary) {
    return '0x' + parseInt(binary, 2).toString(16)
}


var bi = toBinary(hexAddr, n)

var VPN = getPartOfBinary(bi, 0, n-p)
var TLBI = getPartOfBinary(VPN, TLBTNumber, n-p)
var TLBT = getPartOfBinary(VPN, 0, TLBTNumber)



var VPO = getPartOfBinary(bi, n-p, n)
console.log('Virtual address in bits: ' + bi)
console.log('VPN: ' + toHex(VPN))
console.log('VPO: ' + toHex(VPO))

console.log('TLB Index: ' + toHex(TLBI))
console.log('TLB Tag: ' + toHex(TLBT))

var ppn = toBinary('0x11')

var pa = ppn+VPO

console.log('PA in bits: ' + pa.padStart(m, '0'))
