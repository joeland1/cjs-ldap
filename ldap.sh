docker run --detach --name openldap \
  --env LDAP_ADMIN_USERNAME=admin \
  --env LDAP_ADMIN_PASSWORD=adminpassword \
  --env LDAP_USERS=customuser \
  --env LDAP_PASSWORDS=custompassword \
  --env LDAP_ROOT=dc=example,dc=org \
  --env LDAP_USERS=user01,user02 \
  --env LDAP_PASSWORDS=password1,password2 \
  --env LDAP_ADMIN_DN=cn=admin,dc=example,dc=org \
  --env LDAP_ENABLE_TLS=yes \
  --env LDAP_TLS_CA_FILE=/certs/myCA.pem \
  --env LDAP_TLS_CERT_FILE=/certs/ldap.crt \
  --env LDAP_TLS_KEY_FILE=/certs/ldap.key \
  -v $(pwd)/ldap-certs:/certs \
  -p 127.0.0.1:1389:1389 \
  -p 127.0.0.1:1636:1636 \
  --network host \
  bitnami/openldap:latest
