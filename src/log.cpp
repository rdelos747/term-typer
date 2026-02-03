#include <string>
#include <fstream>

#include "log.h"

using namespace std;

void log(string text) {
    string home = getenv("HOME");
    ofstream logFile;
    logFile.open(home + "/Documents/typing/log.txt", ios_base::app);
    logFile << text << '\n';
    logFile.close();
}

void clearLog() {
    string home = getenv("HOME");
    ofstream logFile;
    logFile.open(home + "/Documents/typing/log.txt", ios_base::out);
    logFile.close();
}