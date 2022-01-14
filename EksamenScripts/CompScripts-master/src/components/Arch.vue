<template>
  <div class="arch">
    <h1>Machine Architecture</h1>
    <div>
      <h2>Cache Address Calculator</h2>
      <p>Address Size (in bits)</p><input v-model="addrSize" type="number" ><br/>
      <p>X-Way Set-Associative cache?</p><input v-model="waySet" type="number" ><br/>
      <p>Data Cache Size (kilobytes)</p><input v-model="dataCache" type="number" ><br/>
      <p>Block size (in bytes)</p><input v-model="blockSize" type="number" ><br/>
    </div>
    <div>
      <p v-html="result"></p>
    </div>
    <div>
      <h2>Cache Simulator</h2>
      <textarea id="" cols="30" rows="10" v-model="operations"></textarea>
    </div>
    <div v-if="cacheSimRes != ''">
        <table v-html="cacheSimRes">
        </table>
    </div>
  </div>
</template>

<script>

import CacheSAssociative from './modules/SaModule'

function simCache(addrSize, waySet, cacheSize, blockSize, operations) {

  var mappingMethod = 'Set',
      algorithm = 'LRU'

  cacheSize = cacheSize*1024

  var numSets = cacheSize / (blockSize * waySet),
      setIndexBits = Math.log2(numSets),
      blockOffsetBits = Math.log2(blockSize),
      maxNumUniqueAddrs = Math.pow(2, addrSize),
      tagBits = addrSize - setIndexBits - blockOffsetBits


  var c = new CacheSAssociative(cacheSize, waySet)

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
      var decimalNumber = parseInt(op, 16),
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
      return blockPosition(nBlock, nBlock, nTag, op);
  }

  function blockPosition(set, block, tag, addressAccess) {
      var result = c.blockPosition(block, tag, addressAccess, algorithm);
      var stateTags = []
      for (let i = 0; i < result.set.frames.length; i++) {
          if (result.set.frames[i].lastAccess != null) {
              stateTags.push(result.set.frames[i].lastAccess)
          }
      }

      return '<td>0x' + addressAccess + '</td><td>' + (result.hm == 0 ? 'HIT ' : 'MISS') + '</td><td>' + set + '</td><td>0x' + stateTags.join(', 0x')+ '</td>'
  }

  var retVal = ''
  for (var i = 0; i < operations.length; i++) {
      retVal += '<tr>' + interpetOperation(operations[i]) + '<tr/>'
  }
  return retVal
}

/**
 * Given a byte - addressed machine with 32 - bit addresses.
 * The machine is equipped with a 2 - way set - associative data cache of 32 kilobytes.
 * Cache have a block size of 16 bytes.
 * Give the separation of an address when performing actions on the cache.
 * That is, which bits are used to index within a block, which are used to define the index and which are the tag bits?
 * Explain your answer.
 */
function cacheAddressCalculator(addrSize = 32, waySet = 2, dataCache = 32, blockSize = 16) {
    if (addrSize < 1 || waySet < 1 || dataCache < 1 || blockSize < 2) {
      return "ERROR"
    }
    dataCache = dataCache * 1024
    var numSets = dataCache/(blockSize * waySet),
        setIndexBits = Math.log2(numSets),
        blockOffsetBits = Math.log2(blockSize),
        maxNumUniqueAddrs = Math.pow(2, addrSize),
        tagBits = addrSize-setIndexBits-blockOffsetBits



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

    var printer = 'Number of sets: ' + numSets + '<br/>' +
                  'Number of lines per set: ' + waySet + '<br/>' +
                  'Block size (bytes): ' + blockSize + '<br/>' +
                  'Number of physical (main memory) address bits: ' + addrSize + '<br/>' +
                  'Maximum number of unique memory addresses: ' + maxNumUniqueAddrs + '<br/>' +
                  'Number of set index bits: ' + setIndexBits + '<br/>' +
                  'Number of block offset bits: ' + blockOffsetBits + '<br/>' +
                  'Number of tags bits: ' + tagBits + '<br/>' +
                  'Cache size (bytes), not including overhead such as the valid and tag bits: ' + dataCache + '<br/><br/>' +
                  't is the tag bits, i is the index bits, and b is the block offset bits <br/>' +
                  'Address setup: ' + (addrSize - 1) + ' |' + tags + index + block + '| 0<br/>'

    return printer    
}



export default {
  name: 'Arch',
  data () {
    return {
      addrSize: 32,
      waySet: 2,
      dataCache: 32,
      blockSize: 16,
      result: '',
      cacheSimRes: '',
      operations: ''
    }
  },
  methods: {
    update() {
      this.result = cacheAddressCalculator(this.addrSize, this.waySet, this.dataCache, this.blockSize)
      if (this.operations != '') {
        this.cacheSimRes = '<tbody><tr><th style="padding-right: 20px;padding-left: 20px;">ref</th><th style="padding-right: 20px;padding-left: 20px;">hit / miss</th><th style="padding-right: 20px;padding-left: 20px;">set number</th><th style="padding-right: 20px;padding-left: 20px;">state for the specific set number</th></tr>' + simCache(this.addrSize, this.waySet, this.dataCache, this.blockSize, this.operations.split(/\r?\n/)) + '</tbody>'
      }
    }
  },
  watch: {
    addrSize() {
      this.update()
    },
    waySet() {
      this.update()
    },
    dataCache() {
      this.update()
    },
    blockSize() {
      this.update()
    },
    operations() {
      this.update()
    }
  }
  
}
</script>

<!-- Add "scoped" attribute to limit CSS to this component only -->
<style scoped>
h1, h2 {
  font-weight: normal;
}

table {
  margin-left: auto;
  margin-right: auto;
}


ul {
  list-style-type: none;
  padding: 0;
}
li {
  display: inline-block;
  margin: 0 10px;
}
a {
  color: #42b983;
}
</style>
