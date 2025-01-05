rm /code/addon.node

g++ -static -fPIC \
    -I/code/ldap-async-addon/node_modules/node-api-headers/include \
    -I/code/ldap-async-addon/node_modules/node-addon-api \
    -I/code/ldap-lib/include \
    -I/usr/include/node \
    -std=c++23 \
    -c -o addon.o addon.cpp || exit 1

g++ -static -fPIC \
    -I/code/ldap-async-addon/node_modules/node-api-headers/include \
    -I/code/ldap-async-addon/node_modules/node-addon-api \
    -I/code/ldap-lib/include \
    -I/usr/include/node \
    -std=c++23 \
    -c -o client.o client.cpp || exit 1

g++ -static -fPIC \
    -I/code/ldap-async-addon/node_modules/node-api-headers/include \
    -I/code/ldap-async-addon/node_modules/node-addon-api \
    -I/code/ldap-lib/include \
    -I/usr/include/node \
    -std=c++23 \
    -c -o async-bind-ldap.o async-bind-ldap.cpp -lldap -llber || exit 1

g++ -static -fPIC \
    -I/code/ldap-async-addon/node_modules/node-api-headers/include \
    -I/code/ldap-async-addon/node_modules/node-addon-api \
    -I/code/ldap-lib/include \
    -I/usr/include/node \
    -std=c++23 \
    -c -o async-ldap-search.o async-ldap-search.cpp -lldap -llber || exit 1

#g++ -shared -o /code/addon.node addon.o client.o

#g++ -o /code/addon.node addon.o client.o -L/usr/lib/aarch64-linux-gnu

#ar rcs /code/addon.node.a addon.o client.o /usr/lib/aarch64-linux-gnu/libldap.a
#g++ --static --shared -o /code/addon.node /code/addon.node.a
#ld -shared -static -o /code/addon.node addon.o client.o
ld -fPIC -shared -static \
    -L/code/ldap-lib/lib \
    -o addon.node addon.o client.o async-ldap-search.o async-bind-ldap.o -lldap -llber
