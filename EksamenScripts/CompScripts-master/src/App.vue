<template>
  <div id="app">
    <h1>Overview</h1>
    <router-link to="arch">Arch</router-link>
    <router-link to="os">OS</router-link>
    <router-link to="net">Network</router-link>

    <div>
      HEX <input v-model="hex" v-on:keyup="keyHappend('hex')" type="text"> <br/>
      BIN <input v-model="bi" v-on:keyup="keyHappend('bi')" type="text"><br/>
      DEC <input v-model="dec" v-on:keyup="keyHappend('dec')" type="text">

    </div>

    <router-view/>
  </div>
</template>

<script>


export default {
  name: 'App',
  data () {
    return {
        hex: '',
        bi: '',
        dec: '',
        editing: 'dec'
    }
  },
  methods: {
    hexToBi(str) {
      return parseInt(str.replace('0x', ''), 16).toString(2)
    },
    hexToDec(str) {
      return parseInt(str.replace('0x', ''), 16).toString()
    },
    biToHex(str) {
      return '0x' + parseInt(str, 2).toString(16)
    },
    biToDec(str) {
      return parseInt(str, 2).toString()
    },
    decimalToHex(str) {
      return '0x' + parseInt(str).toString(16)
    },
    decimalToBi(str) {
      return parseInt(str).toString(2)
    },
    keyHappend(at) {
      this.editing = at
      this.hex = this.hexDisplay
      this.bi = this.biDisplay
      this.dec = this.decDisplay

    }
  },
  computed: {
    hexDisplay() {
      if (this.editing == 'hex') {
        return this.hex
      } 
      return this.editing == 'dec' ? this.decimalToHex(this.dec) : this.biToHex(this.bi)
    },
    biDisplay() {
      if (this.editing == 'bi') {
        return this.bi
      } 
      return this.editing == 'hex' ? this.hexToBi(this.hex) : this.decimalToBi(this.dec)
    },
    decDisplay() {
      if (this.editing == 'dec') {
        return this.dec
      } 
      return this.editing == 'hex' ? this.hexToDec(this.hex) : this.biToDec(this.bi)
    },
  }
}
</script>

<style>
#app {
  font-family: 'Avenir', Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
  margin-top: 60px;
}
</style>
