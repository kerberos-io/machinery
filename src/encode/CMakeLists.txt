# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# Build encode

add_library(ENCODE_LIBRARY STATIC 
    base64.cpp
    HMAC_SHA1.cpp
    SHA1.cpp)
    
set(KERBEROS_DEPENDENCIES ${KERBEROS_DEPENDENCIES} ENCODE_LIBRARY PARENT_SCOPE)
set(KERBEROS_LIBRARIES ${KERBEROS_LIBRARIES} ENCODE_LIBRARY PARENT_SCOPE)
