#include <string>
#include <fstream>

#include "log.h"

using namespace std;

void log(string text) {
    ofstream logFile;
    //homeDir + "/Documents/nsx/backups/backup-" + getTime() + "-" + fileName
    //logFile.open("log.txt", ios_base::app);
    logFile.open(HOME_DIR + "/Documents/nsx/log.txt", ios_base::app);
    logFile << text << '\n';
    logFile.close();
}

void clearLog() {
    ofstream logFile;
    logFile.open(HOME_DIR + "/Documents/nsx/log.txt", ios_base::out);
    logFile.close();
}
