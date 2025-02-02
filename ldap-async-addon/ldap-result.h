#pragma once

#include <vector>
#include <variant>
#include <unordered_map>
#include <string>
#include <ldap.h>
#include <napi.h>

typedef std::variant<std::string,std::vector<std::string>> ldap_attribute_entry;
typedef std::unordered_map<std::string,ldap_attribute_entry> ldap_dn_entry;
typedef std::vector<ldap_dn_entry> ldap_response_list;
typedef std::unordered_map<std::string,ldap_dn_entry> ldap_response_map;

class LDAP_Response{
    public:
        LDAP_Response(LDAP* ld, LDAPMessage* answer);
        ~LDAP_Response();
        ldap_response_map response;

    private:
};