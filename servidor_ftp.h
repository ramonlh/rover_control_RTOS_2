
#ifndef SERVIDOR_FTP_H
#define SERVIDOR_FTP_H

#include <MultiFTPServer.h>

extern FtpServer ftpSrv;

// Inicializa el servidor FTP, devuelve true si todo ha ido bien.
bool init_ftp_server(const char* user, const char* password);

#endif