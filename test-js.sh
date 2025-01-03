#npm --prefix "./ldap-async-addon" install
rm /code/ldap-async-addon/addon.node

(cd "/code/ldap-async-addon" && ./build.sh ) || exit 1

#valgrind --leak-check=full node /code/index.js
node /code/index.js