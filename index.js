'use strict'

console.log('running')

const { LDAP, SEARCH_SCOPES } = require("./ldap-async-addon");
console.log("search scopes",SEARCH_SCOPES)

async function doSomething() {
    const ldap_config = {
        uri: "ldaps://localhost:1636",
        dn: "cn=admin,dc=example,dc=org",
    }

    const search_params = {
        filter: "(|(uid=user01)(uid=user02))",
        scope: SEARCH_SCOPES.subtree,
        base: "dc=example,dc=org",
    }

    const c = new LDAP(ldap_config)
    c.search(search_params)

    return true
}

(async () => {
  await doSomething();

  const x = await fetch("http://localhost:8080"); //just to force garbage collector to cleanup the ldap obj 
  const j = await x.json();
  console.log(j);
})();


console.log('should go up top');
