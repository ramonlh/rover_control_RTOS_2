
#include "servidor_ftp.h"
#include <SPIFFS.h>

FtpServer ftpSrv;

bool init_ftp_server(const char* user, const char* password)
{
    Serial.println("Iniciando FTP...");
    ftpSrv.begin(user, password);
    Serial.println("FTP arrancado");
    return true;
}