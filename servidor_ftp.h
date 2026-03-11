
//#include <FTPServer.h>
#include <MultiFTPServer.h>

// Crear el servidor FTP
FtpServer ftpSrv;

void init_ftp_server()
{
  //bool fsok = SPIFFS.begin();
  ftpSrv.begin("admin", "12341234");
}