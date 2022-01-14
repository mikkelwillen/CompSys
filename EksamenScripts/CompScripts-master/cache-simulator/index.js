var CacheSAssociative = require('./modules/SaModule').CacheSAssociative

var addrSize = 16, 
    waySet = 2, 
    cacheSize = 2,
    blockSize = 64,
    kb = 10,
    mappingMethod = 'Set',
    algorithm = 'LRU'

cacheSize = cacheSize*1024

numSets = cacheSize / (blockSize * waySet)
setIndexBits = Math.log2(numSets)
blockOffsetBits = Math.log2(blockSize)
maxNumUniqueAddrs = Math.pow(2, addrSize)
tagBits = addrSize - setIndexBits - blockOffsetBits



var operations = [
    '0xA000', '0xF020', '0xFF00', '0xFF0C', '0x0018', '0xF0A4', '0xF004', '0x0004'
]


c = new CacheSAssociative(cacheSize, waySet)



function decToBinaryString(number) {
    var temp = number.toString(2);
    if (temp.length < addrSize) {
        var daAggiungere = "";
        for (var i = 0; i < addrSize - temp.length; i++) {
            daAggiungere += "0";
        }
        temp = daAggiungere + temp;
    }
    return temp;
}

function getPartOfBinary(v, start, end) {
    var newPart = "";
    for (var i = start; i < end; i++) {
        newPart += v[i];
    }
    return newPart;
}

function interpetOperation(op) {
    op = op.replace('0x', '')
    decimalNumber = parseInt(op, 16);
    binaryNumber = decToBinaryString(decimalNumber)
    
    var offSetBinaryBit, indexBinaryBit, tagBinaryBit
    if (mappingMethod == "Direct") {
        offSetBinaryBit = getPartOfBinary(binaryNumber, (addrSize - blockOffsetBits), addrSize);
        indexBinaryBit = getPartOfBinary(binaryNumber, tagBits, blocksInCache + tagBits);
        tagBinaryBit = getPartOfBinary(binaryNumber, 0, tagBits);
    }

    if (mappingMethod == "Fully") {
        tagBinaryBit = getPartOfBinary(binaryNumber, 0, tagBits);
        offSetBinaryBit = getPartOfBinary(binaryNumber, (addrSize - blockOffsetBits), addrSize);
    }

    if (mappingMethod == "Set") {
        offSetBinaryBit = getPartOfBinary(binaryNumber, (addrSize - blockOffsetBits), addrSize);
        indexBinaryBit = getPartOfBinary(binaryNumber, tagBits, setIndexBits + tagBits);
        tagBinaryBit = getPartOfBinary(binaryNumber, 0, tagBits);
    }
    
    var nSet = parseInt(offSetBinaryBit, 2)
    var nBlock = parseInt(indexBinaryBit, 2);
    var nTag = parseInt(tagBinaryBit, 2);
    blockPosition(nBlock, nBlock, nTag, op);
}

function blockPosition(set, block, tag, addressAccess) {
    var result = c.blockPosition(block, tag, addressAccess, algorithm);
    var stateTags = []
    for (let i = 0; i < result.set.frames.length; i++) {
        if (result.set.frames[i].lastAccess != null) {
            stateTags.push(result.set.frames[i].lastAccess)
        }
    }

    console.log('0x' + addressAccess + ' ' + (result.hm == 0 ? 'HIT ' : 'MISS') + ' ' + set + ' 0x' + stateTags.join(', 0x'))
}

console.log('ref | hit / miss | set number | state for the specific set number ')
for (var i = 0; i < operations.length; i++) {
    interpetOperation(operations[i])
}