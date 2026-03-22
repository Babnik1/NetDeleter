#pragma once
#include <iostream> 

#ifdef DEBUG_MODE
    #define DEBUG_LOG( msg ) \
        std::clog << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ": " << msg << std::endl
#else
    #define DEBUG_LOG( msg ) ( ( void ) 0 )
#endif

#ifdef ERROR_MODE
    #define ERROR_LOG( msg ) \
        std::clog << "[ERROR] " << __FILE__ << ":" << __LINE__ << ": " << msg << std::endl
#else
    #define ERROR_LOG( msg ) ( ( void ) 0 )
#endif

#ifdef INFO_MODE
    #define INFO_LOG( msg ) \
        std::clog << "[INFO] " << __FILE__ << ":" << __LINE__ << ": " << msg << std::endl
#else
    #define INFO_LOG( msg ) ( ( void ) 0 )
#endif
