gcc -o ad_ldaptest ldap-test.c -lldap -DLDAP_DEPRECATED=1 || exit 1

#leaks --atExit -- ./ad_ldaptest
./ad_ldaptest