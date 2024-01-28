/**
 * @file dhcp-monitor.cpp
 * @author Michal Ľaš (xlasmi00)
 * @brief Class for printing out information about DHCP traffic, include consol printing and updating and logging to syslog
 * @date 2023-10-21
 * 
 */


#include "consol-log.hpp"

using namespace std;



ConsolLog::ConsolLog(vector<IPv4Prefix*> &prefixes, atomic<bool> *stopFlag) : _stopFlag(stopFlag)
{
    this->_consolOpen = false;
    this->_rows = 0;
    int row_count = HEADER_ROWS;
    for (auto &prefix : prefixes)
    {
        this->_prefixes.insert(make_pair(prefix->getPrefixName(), prefixData{row_count, false, prefix}));
        row_count++;
    }
    // Open syslog
    setlogmask(LOG_UPTO (LOG_WARNING));
    openlog("dhcp-stats", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
}


ConsolLog::~ConsolLog()
{
    closelog();
}


void ConsolLog::makeConsol(ConsolLog *consol)
{
    // Initialize ncurses
    initscr();
    consol->_consolOpen = true;
    // Hide user input and cursor
    noecho();
    curs_set(0);
    // Enable keypad input
    keypad(stdscr, TRUE);

    // Print header
    int rows, cols;
    getmaxyx(stdscr, rows, cols);
    consol->_columnWidth = cols / APP_COLUMNS;

    mvprintw(0, 0, "%-*s", consol->_columnWidth, "IP-Prefix");
    mvprintw(0, consol->_columnWidth, "%-*s", consol->_columnWidth, "Max-hosts");
    mvprintw(0, consol->_columnWidth*2, "%-*s", consol->_columnWidth, "Allocated addresses");
    mvprintw(0, consol->_columnWidth*3, "%-*s", consol->_columnWidth, "Utilization");
    for (int i = 0; i < cols; i++)
    {
        mvprintw(1, i, "=");
    }

    consol->_rows += 2;

    // print data
    for (auto const &[key, val] : consol->_prefixes)
    {
        mvprintw(val.row, 0, "%-*s", consol->_columnWidth, key.c_str()); // first column => 0
        mvprintw(val.row, consol->_columnWidth, "%u", val.prefix->getMaxHosts());   // second column => 1
        mvprintw(val.row, consol->_columnWidth*2, "%u", val.prefix->getAllocatedHosts()); // third column => 2
        mvprintw(val.row, consol->_columnWidth*3, "%.2f%%", val.prefix->calculateUtilizationPercentage()); // fourth column => 3
        consol->_rows++;
    }

    // Refresh the screen to see the output
    refresh();

    // Wait for ESC or 'q' to close the consol
    int ch;
    do
    {
        ch = getch();
    } while ((ch != 27) && (ch != 'q'));

    // rise signal to stop the program
    consol->_stopFlag->store(true);

    endwin();
}


void ConsolLog::updateConsol(string prefix)
{
    if (this->_prefixes.count(prefix))
    {
        prefixData row = this->_prefixes.at(prefix);
        // move cursor and clear the row
        move(row.row, this->_columnWidth*2); // clear just from column 3
        clrtoeol();
        // print new data
        mvprintw(row.row, this->_columnWidth*2, "%u", row.prefix->getAllocatedHosts()); // third column => 2
        mvprintw(row.row, this->_columnWidth*3, "%.2f%%", row.prefix->calculateUtilizationPercentage()); // fourth column => 3
        // Refresh the screen to see the output
        refresh();
    }
}


void ConsolLog::printStats()
{
    // calculate column width
    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    this->_columnWidth = w.ws_col / APP_COLUMNS;

    // print header
    cout<< setw(this->_columnWidth) << left << "IP-Prefix"
        << setw(this->_columnWidth) << left << "Max-hosts"
        << setw(this->_columnWidth) << left << "Allocated addresses"
        << setw(this->_columnWidth) << left << "Utilization"
        << endl;

    for (int i = 0; i < APP_COLUMNS * this->_columnWidth; i++)
    {
        cout << "=";
    }
    cout << endl;

    // sort prefixes before printing (they are sorted based on order in program arguments)
    prefixData dataLines[this->_prefixes.size()];
    for (auto const &[key,val] : this->_prefixes)
    {
        dataLines[val.row-HEADER_ROWS] = val;
    }

    // print prefixes
    for (auto const val : dataLines)
    {
        cout<< setw(this->_columnWidth) << left << val.prefix->getPrefixName().c_str()
            << setw(this->_columnWidth) << left << val.prefix->getMaxHosts()
            << setw(this->_columnWidth) << left << val.prefix->getAllocatedHosts()
            << setw(this->_columnWidth) << left << fixed << setprecision(2) << setw(0) << val.prefix->calculateUtilizationPercentage() << "%"
            << endl << endl;
    }
}


void ConsolLog::makeSysLog(string prefix)
{
    // If prefix is monitored
    if (this->_prefixes.count(prefix))
    {
        // if prefix exceeded 50% of alloctations
        if (SYSLOG_THRESHOLD <= this->_prefixes.at(prefix).prefix->calculateUtilizationPercentage())
        {
            // log was not made yet
            if (!this->_prefixes.at(prefix).logFlag)
            {
                syslog(LOG_WARNING, "prefix %s exceeded 50%% of allocations.", prefix.c_str());

                if (this->_consolOpen)
                {
                    this->_rows += 1;
                    move(this->_rows, 0);
                    clrtoeol();
                    mvprintw(this->_rows, 0, "WARNING: prefix %s exceeded 50%% of allocations.", prefix.c_str());
                    refresh();
                    
                }
                else
                {
                    cout << "WARNING: prefix " << prefix << " exceeded 50% of allocations. " << endl;
                }
                this->_prefixes.at(prefix).logFlag = true;
            }
        }
        else
        {
            this->_prefixes.at(prefix).logFlag = false;
        }
    }
}


/* END OF FILE */