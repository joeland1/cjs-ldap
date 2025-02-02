#pragma once

#include <ldap.h>
#include <napi.h>

enum class TLS_CHECK { 
    NEVER = LDAP_OPT_X_TLS_NEVER,
    HARD = LDAP_OPT_X_TLS_HARD,
    DEMAND = LDAP_OPT_X_TLS_DEMAND,
    ALLOW = LDAP_OPT_X_TLS_ALLOW,
    TRY = LDAP_OPT_X_TLS_TRY
};


Napi::Object CREATE_TLS_CHECK_ENUM(Napi::Env env);

bool valid_tls_check(TLS_CHECK c);