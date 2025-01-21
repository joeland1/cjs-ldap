'use strict'

console.log('running')

const { LDAP, SEARCH_SCOPES } = require("./ldap-async-addon");

async function doSomething() {
    const ldap_config_tls = {
        uri: "ldaps://localhost:1636",
    }

    const ldap_config_plain = {
        uri: "ldap://localhost:1389"
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

    const clients = []
    const CLIENT_COUNT = 1;

    /*
    for(let i=0;i<CLIENT_COUNT;i++){
        const client = new LDAP(ldap_config_tls);
        await client.exec();
    }
    */
    
    for(let i=0;i<CLIENT_COUNT;i++){
        const client = new LDAP(ldap_config_tls)
        clients.push(client)
    }

    const exec_arr = []
    clients.forEach((client)=>{
        exec_arr.push(client.exec());
    })
    await Promise.allSettled(exec_arr)

    const bind_requests = []
    clients.forEach((client)=>{
        //bind_requests.push(client.bind(bind_params));
    })
    await Promise.allSettled(bind_requests);


    const close_requests = []
    clients.forEach((client) => {
        //close_requests.push(client.close())
    });

    await Promise.allSettled(close_requests);
    
    
    return true
}

(async () => {
  await doSomething();
  //await doSomething();
  //await doSomething();
  //await doSomething();
  //await doSomething();
})();

(async () => {
  const x = await fetch("http://localhost:8080"); //just to force garbage collector to cleanup the ldap obj 
  const j = await x.json();
  console.log(j);
})();


console.log('should go up top');
