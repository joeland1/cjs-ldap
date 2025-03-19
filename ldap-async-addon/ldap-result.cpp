#include <napi.h>
#include <ldap.h>
#include "ldap-result.h"

LDAP_Response::LDAP_Response(LDAP* ld, LDAPMessage* ans){
    
    ldap_response_map final_map;

    LDAPMessage* entry;
    for ( entry = ldap_first_entry(ld, ans); entry != NULL; entry = ldap_next_entry(ld, entry)) {
        char* dn = ldap_get_dn(ld, entry);

        ldap_dn_entry dn_entry;
        
        //dn_entry["dn"] = std::string(dn);

        //printf("Found Object: %s\n", dn);
        BerElement *ber;

        for ( char* attribute = ldap_first_attribute(ld, entry, &ber); attribute != NULL; attribute = ldap_next_attribute(ld, entry, ber)){
            struct berval** ber_arr = ldap_get_values_len(ld,entry, attribute);
            std::vector<std::string> attribute_values;

            for(int i=0;;i++){
                struct berval* ber_item = ber_arr[i];
                if(ber_item == NULL){
                    break;
                }
                attribute_values.push_back(std::string(ber_item->bv_val));

                //printf("%s: %s\n",attribute,ber_item->bv_val);
            }
            
            dn_entry[attribute] = attribute_values;
            ldap_value_free_len(ber_arr);
            ldap_memfree(attribute);
        }
        final_map[dn] = dn_entry;
        ber_free(ber,0);
        ldap_memfree(dn);
    }

    this->response = final_map;
}

LDAP_Response::~LDAP_Response(){
    //printf("ldap response destructor\n");
}
