#pragma once

#include <ldap.h>
#include <napi.h>

enum class SEARCH_SCOPES { 
    Base = LDAP_SCOPE_BASE,
    One_Level = LDAP_SCOPE_ONELEVEL,
    Subtree = LDAP_SCOPE_SUBTREE,
    Children = LDAP_SCOPE_CHILDREN
};

//static Napi::PropertyDescriptor create_read_only_int(Napi::Env env, std::string key, SEARCH_SCOPES value);

Napi::Object CREATE_SEARCH_SCOPE_ENUM(Napi::Env env);

bool is_valid_search_scope(SEARCH_SCOPES s);