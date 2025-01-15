'use strict'

console.log('running')

const { LDAP, SEARCH_SCOPES } = require("./ldap-async-addon");

async function doSomething() {
    const ldap_config = {
        uri: "ldaps://localhost:1636",
        dn: "cn=admin,dc=example,dc=org",
    }

    const bind_params = {
        dn: "cn=admin,dc=example,dc=org",
        password: "adminpassword"
    }

    const search_params = {
        filter: "(|(uid=user01)(uid=user02))",
        scope: SEARCH_SCOPES.subtree,
        base: "dc=example,dc=org",
    }

    const client = new LDAP(ldap_config)
    const bind_result = await client.bind(bind_params);
    const search_result = await client.search(search_params);
    client.search(search_params);
    client.search(search_params);
    client.search(search_params);
    client.search(search_params);
    client.search(search_params);
    client.search(search_params);
    
    client.close()

    return true
}

(async () => {
  await doSomething();

  const x = await fetch("http://localhost:8080"); //just to force garbage collector to cleanup the ldap obj 
  const j = await x.json();
  console.log(j);
})();


console.log('should go up top');
