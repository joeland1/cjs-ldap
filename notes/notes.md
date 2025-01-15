# Compiling openldap

MUST DO IN debian-slim

apt-get update -y && apt-get install -y build-essential groff-base

## openldap
CFLAGS='-fPIC -pie -DPIC' ./configure --prefix=/code/ldap-lib --enable-static --with-gnu-ld


## openssl
CFLAGS='-fPIC -pie -DPIC ' ./Configure --prefix=/code/openssl-lib


## ldap w/ ssl

CFLAGS='-fPIC -pie -DPIC -I/code/openssl-lib/include -L/code/openssl-lib/lib' CPPFLAGS='-I/code/openssl-lib/include -L/code/openssl-lib/lib' LDFLAGS='-I/code/openssl-lib/include -L/code/openssl-lib/lib' ./configure --prefix=/code/ldap-lib --enable-static --with-gnu-ld

## Useful shit

Getting a type of something

from https://stackoverflow.com/questions/24441505/retrieving-the-type-of-auto-in-c11-without-executing-the-program

```
template<class Type> struct S;
S<decltype(x)>();
```

run a object's function given reference to static version...


```
template <typename T, typename R>
static R run_decleration(T& v, R (T::*member)() const ) {
    return (v.*member)();}

run_decleration<obj_type,return_type>(obj,&obj_type::func);
```