# @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
# Build library
add_library(TINYXML_LIBRARY 
	tinystr.cpp
	tinyxml.cpp
	tinyxmlerror.cpp
	tinyxmlparser.cpp)
set(KERBEROS_DEPENDENCIES ${KERBEROS_DEPENDENCIES} TINYXML_LIBRARY PARENT_SCOPE)
set(KERBEROS_LIBRARIES ${KERBEROS_LIBRARIES} TINYXML_LIBRARY PARENT_SCOPE)