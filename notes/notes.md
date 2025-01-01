# Compiling openldap

MUST DO IN debian-slim

apt-get update -y && apt-get install -y build-essential groff-base

CFLAGS='-fPIC -pie -DPIC' ./configure --prefix=/code/ldap-lib --enable-static --with-gnu-ld

