/**
 * @file consol-log.hpp
 * @author Michal Ľaš (xlasmi00)
 * @brief Header file for consol-log.cpp
 * @date 2023-10-21
 * 
 */


#include <vector>
#include <ncurses.h>
#include <iomanip>
#include <sys/ioctl.h>
#include <csignal>
#include <atomic>
#include <syslog.h>

#include "errors.hpp"
#include "ip-prefix.hpp"


#ifndef CONSOL_LOG_H
#define CONSOL_LOG_H


using namespace std;


/******************** MACROS ********************/

#define APP_COLUMNS 4   // nomber of application columns
#define HEADER_ROWS 2   // number of header rows
#define SYSLOG_THRESHOLD 50.0 // threshold of ip prefix utilization to make syslog


/***************** STRUCTURE ********************/


/**
 * @brief Data needed to display prefix statistics
 * 
 */
struct prefixData{
    int row;            /// row in which is prefix data displayed
    bool logFlag;       /// flag indicating whether the prefix was entered in the log
    IPv4Prefix *prefix; /// pointer to prefix
};


/*********** FUNCTIONS AND CLASSES **************/


class ConsolLog
{
private:
    /* Private Attributes */
    map<string, prefixData> _prefixes;              /// prefixes/data that will be printed
    atomic<bool> *_stopFlag;                        /// pointer to flag if program should stop (if set to true consol got the signal to stop the program)
    int _columnWidth;                               /// width of one column
    unsigned _rows;                                 /// number of rows
    bool _consolOpen;                               /// Flag indicating if consol is open or not

    /* Private Methods */


public:
    /* Public Attributes */


    /* Public Methods */
    /**
     * @brief Construct a new Consol Log object
     * 
     * @param prefixes reference to vector with pointers to prefixes that will be displayed
     * @param stopFlag pointer to flag that indicates, that program should stops (this flag is set to true if the user enters an input to terminate the program)
     */
    ConsolLog(vector<IPv4Prefix*> &prefixes, atomic<bool> *stopFlag);
    ~ConsolLog();

    /**
     * @brief Open a dynamic consol with initial header and data (It is static method because it will allow this function to run in separate thread)
     * 
     * @param consol pointer to method's ConsolLog class (aka. this)
     */
    static void makeConsol(ConsolLog *consol);

    /**
     * @brief Update consol data for specific prefix name
     * 
     * @param prefix name of prefix that will be updated
     */
    void updateConsol(string prefix);

    /**
     * @brief Prints table with prefixes data to standard output 
     * 
     */
    void printStats();

    /**
     * @brief make syslog for given prefix
     * 
     * @param prefix name of prefix
     */
    void makeSysLog(string prefix);

};


#endif // CONSOL_LOG_H