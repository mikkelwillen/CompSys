<template>
  <div class="os">
    <h1>Operating Systems</h1>
    <h3>Virtual Memory Address Translator</h3>
    <div>
      <h2>VI setup</h2>
      <p>Virtual addresse Size (in bits)</p><input v-model="n" type="number" ><br/>
      <p>Physical addresse Size (in bits)</p><input v-model="m" type="number" ><br/>
      <p>Page size (in bytes)</p><input v-model="P" type="number" ><br/>
      <p>x-way set associative </p><input v-model="TLB" type="number" ><br/>
    </div>
    <div>
      <h2>Virtual address</h2>
      <p>Virtual addresse to translate (in hex)</p><input v-model="addr" type="text" ><br/>
      <p>PPN to translate if any (in hex)</p><input v-model="paddr" type="text" ><br/>
      <div v-html="res"></div>
    </div>
  </div>
</template>

<script>

function translateVIM(n, m, P, TLB, hexAddr, pv) {

    var p = Math.log2(P),
        TLBINumber = Math.ceil(Math.log2(TLB))

    var TLBTNumber = n-p-TLBINumber

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
    var res = 'Virtual address in bits: ' + bi + '<br/>'
        res += 'VPN: ' + toHex(VPN) + '<br/>'
        res += 'VPO: ' + toHex(VPO)+ '<br/>'

        res += 'TLB Index: ' + toHex(TLBI) + '<br/>'
        res += 'TLB Tag: ' + toHex(TLBT) + '<br/>'

    var ppn = toBinary(pv)

    var pa = ppn+VPO
    if (pv != '') {
        res += 'Physical address in bits: ' + pa.padStart(m, '0') + '<br/>'
    }

    return res
}

export default {
  name: 'OS',
  data () {
    return {
        n: 14,
        m: 12,
        P: 64,
        TLB: 4,
        addr: '',
        paddr: '',
        res: ''
    }
  },
  methods: {
    update() {
        this.res = translateVIM(this.n, this.m, this.P, this.TLB, this.addr, this.paddr)
    }
  },
  watch: {
      addr() {
          this.update()
      },
      paddr() {
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
