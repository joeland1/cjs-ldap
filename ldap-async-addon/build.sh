LD_BIN_DEFAULT="ld"
LD_BIN="${LD_BIN:-${LD_BIN_DEFAULT}}"

GPP_BIN_DEFAULT="g++"
GPP_BIN="${GPP_BIN:-${GPP_BIN_DEFAULT}}"

GPP_FLAGS_DEFAULT="-fPIC -pie -DPIC
    -I/code/ldap-async-addon/node_modules/node-addon-api 
    -I/code/ldap-lib/include 
    -I/usr/local/include/node 
    -std=c++23
    -c"
GPP_FLAGS="${GPP_FLAGS:-${GPP_FLAGS_DEFAULT}}"

LDAP_LIB_DEFAULT="/code/ldap-lib/lib"
LDAP_LIB="${LDAP_LIB:-${LDAP_LIB_DEFAULT}}"

$GPP_BIN $GPP_FLAGS -o addon.o addon.cpp || exit 1
$GPP_BIN $GPP_FLAGS -o client.o client.cpp || exit 1
$GPP_BIN $GPP_FLAGS -o async-bind-ldap.o async-bind-ldap.cpp -lldap -llber || exit 1
$GPP_BIN $GPP_FLAGS -o async-ldap-search.o async-ldap-search.cpp -lldap -llber || exit 1
$GPP_BIN $GPP_FLAGS -o async-ldap-close.o async-ldap-close.cpp -lldap -llber || exit 1
$GPP_BIN $GPP_FLAGS -o search_values.o search_values.cpp || exit 1
$GPP_BIN $GPP_FLAGS -o ldap-result.o ldap-result.cpp || exit 1
$GPP_BIN $GPP_FLAGS -o ldap-result-obj.o ldap-result-obj.cpp || exit 1
$GPP_BIN $GPP_FLAGS -o tls_validation_settings.o tls_validation_settings.cpp || exit 1

#g++ $GPP_FLAGS -o generic-async.o generic-async.cpp || exit 1

#g++ -shared -o /code/addon.node addon.o client.o

#g++ -o /code/addon.node addon.o client.o -L/usr/lib/aarch64-linux-gnu

#ar rcs /code/addon.node.a addon.o client.o /usr/lib/aarch64-linux-gnu/libldap.a
#g++ --static --shared -o /code/addon.node /code/addon.node.a
#ld -shared -static -o /code/addon.node addon.o client.o
$LD_BIN -fPIC -shared -static \
    -L${LDAP_LIB} \
    -o addon.node addon.o client.o ldap-result.o async-ldap-search.o async-bind-ldap.o async-ldap-close.o ldap-result-obj.o search_values.o tls_validation_settings.o -lldap -llber || exit 1
