
#include "servidor_ftp.h"
#include <SPIFFS.h>

FtpServer ftpSrv;

bool init_ftp_server(const char* user, const char* password)
{
    if (!SPIFFS.begin(true)) {
        return false;
    }

    ftpSrv.begin(user, password);
    return true;
}