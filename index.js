'use strict'

console.log('running')

const { LDAP, SEARCH_SCOPES, TLS_CHECK } = require("./cjs-ldap");

console.log(TLS_CHECK)

const ldap_config_tls = {
    uri: "ldap://localhost:1389",
    //TLS_CHECK: TLS_CHECK.allow
}

const bind_params = {
    dn: "cn=admin,dc=example,dc=org",
    password: "adminpassword"
}

const search_params = {
    filter: "(|(uid=user01)(uid=user02))",
    scope: SEARCH_SCOPES.subtree,
    base: "dc=example,dc=org",
    attributes: ["dn"]
}

async function doSomething() {
    const client = new LDAP(ldap_config_tls)
    const bind = await client.bind(bind_params)
    const search = await client.search(search_params)
    console.log(search.toObject())
    const close = await client.close()
    return true
}

(async () => {
    const MAX = 5
    for(let i=0;i<MAX;i++)
        await doSomething();
    process.exit(0)
  //await doSomething();
  //await doSomething();
})();

function sleep(ms) {
  return new Promise((resolve) => {
    setTimeout(resolve, ms);
});
}

console.log('should go up top');
