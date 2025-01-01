'use strict'

console.log('running')

const { LDAP } = require("./ldap-async-addon");

async function doSomething() {
    const ldap_config = {
        uri: "ldap://127.0.0.1:1389",
        host: '127.0.0.1',
        port: 1389
    }
    const c = new LDAP(ldap_config)
    const x = c.bind()
    console.log(x)
    await x
    console.log(x)

}

doSomething();