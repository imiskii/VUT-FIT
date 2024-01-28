/**
 * @file errors.cpp
 * @author Michal Ľaš
 * @brief Error and warning function for dhcp-stats
 * @date 2023-19-10
 * 
 */


#include "errors.hpp"


using namespace std;


template <class... Args>
void exit_error(ErrorCodes error_code, Args... args);


template <class... Args>
void print_warning(Args... args);

/* END OF FILE */