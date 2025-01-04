# Compiling openldap

MUST DO IN debian-slim

apt-get update -y && apt-get install -y build-essential groff-base

## openldap
CFLAGS='-fPIC -pie -DPIC' ./configure --prefix=/code/ldap-lib --enable-static --with-gnu-ld


## openssl
CFLAGS='-fPIC -pie -DPIC ' ./Configure --prefix=/code/openssl-lib


## ldap w/ ssl

CFLAGS='-fPIC -pie -DPIC -I/code/openssl-lib/include -L/code/openssl-lib/lib' CPPFLAGS='-I/code/openssl-lib/include -L/code/openssl-lib/lib' LDFLAGS='-I/code/openssl-lib/include -L/code/openssl-lib/lib' ./configure --prefix=/code/ldap-lib --enable-static --with-gnu-ld