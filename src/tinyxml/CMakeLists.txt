# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# Build library
add_library(TINYXML_LIBRARY STATIC 
	tinystr.cpp
	tinyxml.cpp
	tinyxmlerror.cpp
	tinyxmlparser.cpp)
set(KERBEROS_DEPENDENCIES ${KERBEROS_DEPENDENCIES} TINYXML_LIBRARY PARENT_SCOPE)
set(KERBEROS_LIBRARIES ${KERBEROS_LIBRARIES} TINYXML_LIBRARY PARENT_SCOPE)