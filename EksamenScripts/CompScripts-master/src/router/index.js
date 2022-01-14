import Vue from 'vue'
import Router from 'vue-router'
import Arch from '@/components/Arch'
import OS from '@/components/os'
import net from '@/components/network'

Vue.use(Router)

export default new Router({
  routes: [
    {
      path: '/arch',
      name: 'Arch',
      component: Arch
    },
    {
      path: '/os',
      name: 'OS',
      component: OS
    },
    {
      path: '/net',
      name: 'network',
      component: net
    }
  ]
})
