<template>
  <div class="net">
    <h1>Computer Networks</h1>
    <div>
      <h3>Calculate Parity Bits of text string</h3>
      <input v-model="str" type="text" ><br/>
      <div v-if="str != ''" v-html="'Even: ' + calcParityBitsE(str)"></div>
      <div v-if="str != ''" v-html="'Odd: ' + calcParityBitsO(str)"></div>
    </div>
    <div>
      <h3>Calculate Internet Checksum of text string or bytes</h3>
      <textarea id="" cols="30" rows="10" v-model="checkStr"></textarea>
      <div v-if="checkStr != ''" v-html="calcInternetCheckSum(checkStr)"></div>
    </div>
  </div>
</template>

<script>


function calcParityBits(str) {
    var table = []

    for (var i = 0; i < str.length; i++) {
        table.push(str.charCodeAt(i).toString(2).padStart(8, '0'))
    }

    var rowBits = '',
        columBits = ''

    for (var row = 0; row < table.length; row++) {
        var sum = 0
        for (var i = 0; i < 8; i++) {
            if (table[row][i] == '1') {
                sum += 1
            }
        }
        if (sum%2 != 0) {
            rowBits += "1"
        } else {
            rowBits += "0"
        }
    }


    for (var i = 0; i < 8; i++) {
        var sum = 0
        for (var row = 0; row < table.length; row++) {
            if (table[row][i] == '1') {
                sum += 1
            }
        }
        if (sum % 2 != 0) {
            columBits += "1"
        } else {
            columBits += "0"
        }
    }

    return rowBits + " " + columBits
}

function reverseString(str) {
    // Step 1. Use the split() method to return a new array
    var splitString = str.split(""); // var splitString = "hello".split("");
    // ["h", "e", "l", "l", "o"]

    // Step 2. Use the reverse() method to reverse the new created array
    var reverseArray = splitString.reverse(); // var reverseArray = ["h", "e", "l", "l", "o"].reverse();
    // ["o", "l", "l", "e", "h"]

    // Step 3. Use the join() method to join all elements of the array into a string
    var joinArray = reverseArray.join(""); // var joinArray = ["o", "l", "l", "e", "h"].join("");
    // "olleh"

    //Step 4. Return the reversed string
    return joinArray; // "olleh"
}


function addBi(bi1, bi2) {
    var carry = 0,
        res = ''
    for (var i = bi1.length-1; i >= 0; i--) {
        var sum = parseInt(bi1[i])+parseInt(bi2[i])+carry
        if (sum > 1) {
            carry = 1
            res += '0'
        } else {
            res += '' + sum
            carry = 0
        }
    }
    return {s: reverseString(res), finalCarry: carry}
}

function not (bi) {
    var ret = ''
    for (var i = 0; i < bi.length; i++) {
        ret += bi[i] == '0' ? '1' : '0'
    }
    return ret
}


function internetChecksum(str) {
    var table = []
    if (str.split(/\r?\n/).length > 1) {
        // We got binary, otherwise it is a single str
        table = str.split(/\r?\n/)
    } else {
      for (var i = 0; i < str.length; i++) {
          table.push(str.charCodeAt(i).toString(2).padStart(8, '0'))
      }
    }

    var sum = '0'.padStart(16, '0'),
        fc = 0   
    if (table.length > 1) {
      for (var row = 0; row < table.length; row+=2) {
        if (table[row+1]) {
          var res = addBi(sum, table[row].padStart(8, '0') + table[row+1].padStart(8, '0'))
          sum = res.s
          fc = res.finalCarry
        } else {
          var res = addBi(sum, ''.padStart(8, '0') + table[row].padStart(8, '0'))
          sum = res.s
          fc = res.finalCarry
        }
      }
    } else {
      return ''
    }

    if (fc > 0) {
        sum = addBi(sum, '1'.padStart(16,'0'))
    }
    var r = not(sum)
    return r.slice(0, 8) + " " + r.slice(8)
}



function ipToBinary(ip) {
    var ipSplit = ip.split('.'),
        biStr = []
    for (var i = 0; i < ipSplit.length; i++) {
        biStr.push(parseInt(ipSplit[i]).toString(2).padStart(8, '0'))
    }
    return biStr.join('.')
}

export default {
  name: 'net',
  data () {
    return {
      str: '',
      resStr: '',
      checkStr: '',
      checkRes: ''
    }
  },
  methods: {
    calcParityBitsE(str) {
      return calcParityBits(str)
    },
    calcParityBitsO(str) {
      try {
        var res = calcParityBits(str).split(' ')
        res[0] = not(res[0])
        res[1] = not(res[1])
      } catch(e) {

      }
      return res.join(' ')
    },
    calcInternetCheckSum(str) {
      return internetChecksum(str)
    }
  },
  watch: {
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
