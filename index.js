'use strict'

console.log('running')

const { LDAP } = require("./ldap-async-addon");

async function doSomething() {
    const ldap_config = {
        uri: "ldaps://localhost:1636",
        dn: "cn=admin,dc=example,dc=org",
    }
    const c = new LDAP(ldap_config)

    const bind_config = {
        dn: "cn=admin,dc=example,dc=org",
        password: "adminpassword"
    }
    await c.bind(bind_config);

    const search_params = {
        filter: "(|(uid=user01)(uid=user02))",
        scope: 2,
        base: "dc=example,dc=org"
    }

    const search_result = await c.search(search_params);
    console.log('search result = ',search_result);

    return true
}

(async () => {
  const foo = await doSomething();
  const x = await fetch("http://localhost:8080"); //just to force garbage collector to cleanup the ldap obj 
  const j = await x.json();
  console.log(j);
})();


console.log('should go up top');
