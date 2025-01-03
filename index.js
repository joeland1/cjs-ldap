'use strict'

console.log('running')

const { LDAP } = require("./ldap-async-addon");

async function doSomething() {
    const ldap_config = {
        uri: "ldap://127.0.0.1:1389",
        dn: "cn=admin,dc=example,dc=org",
    }
    const c = new LDAP(ldap_config)

    const bind_config = {
        dn: "cn=admin,dc=example,dc=org",
        password: "adminpassword"
    }
    let x = c.bind(bind_config);
    console.log(x)
    x = await x;
    console.log(x === 0)
    //const x = c.exec()
    //console.log(x)
    return true
}

(async () => {
  const foo = await doSomething();
  const x = await fetch("http://localhost:8080"); //just to force garbage collector to cleanup the ldap obj 
  const j = await x.json();
  console.log(j);
})();


console.log('should go up top');
