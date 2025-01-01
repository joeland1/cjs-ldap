docker run --detach --rm --name openldap \
  --env LDAP_ADMIN_USERNAME=admin \
  --env LDAP_ADMIN_PASSWORD=adminpassword \
  --env LDAP_USERS=customuser \
  --env LDAP_PASSWORDS=custompassword \
  --env LDAP_ROOT=dc=example,dc=org \
  --env LDAP_USERS=user01,user02 \
  --env LDAP_PASSWORDS=password1,password2 \
  --env LDAP_ADMIN_DN=cn=admin,dc=example,dc=org \
  -p 127.0.0.1:1389:1389 \
  --network host \
  bitnami/openldap:latest