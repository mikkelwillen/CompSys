

/**
 * Given a byte - addressed machine with 32 - bit addresses.
 * The machine is equipped with a 2 - way set - associative data cache of 32 kilobytes.
 * Cache have a block size of 16 bytes.
 * Give the separation of an address when performing actions on the cache.
 * That is, which bits are used to index within a block, which are used to define the index and which are the tag bits?
 * Explain your answer.
 */
function cacheAddressCalculator(addrSize = 32, waySet = 2, dataCache = 32, blockSize = 16) {
    dataCache = dataCache * 1024
    var numSets = dataCache/(blockSize * waySet),
        setIndexBits = Math.log2(numSets),
        blockOffsetBits = Math.log2(blockSize),
        maxNumUniqueAddrs = Math.pow(2, addrSize),
        tagBits = addrSize-setIndexBits-blockOffsetBits,



    var tags = ''
    var index = ' '
    var block = ' '

    for (let i = 0; i < tagBits; i++) {
        tags += 't'
    }
    for (let i = 0; i < setIndexBits; i++) {
        index += 'i'
    }
    for (let i = 0; i < blockOffsetBits; i++) {
        block += 'b'
    }

    printer = 'Number of sets: ' + numSets + '\n' +
              'Number of lines per set: ' + waySet + '\n' +
              'Block size (bytes): ' + blockSize + '\n' +
              'Number of physical (main memory) address bits: ' + addrSize + '\n' +
              'Maximum number of unique memory addresses: ' + maxNumUniqueAddrs + '\n' +
              'Number of set index bits: ' + setIndexBits + '\n' +
              'Number of block offset bits: ' + blockOffsetBits + '\n' +
              'Number of tags bits: ' + tagBits + '\n' +
              'Cache size (bytes), not including overhead such as the valid and tag bits: ' + dataCache + '\n\n' +
              't is the tag bits, i is the index bits, and b is the block offset bits \n' +
              'Address setup: ' + (addrSize - 1) + ' |' + tags + index + block + '| 0\n'

    return printer    
}
cacheAddressCalculator(16, 4, 8, 16)
