#include "HttpsUtil.h"
#ifdef WIN32
#include <winsock.h>
#else
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#endif

#include <openssl/rand.h>
#include <openssl/crypto.h>



#ifdef WIN32
#pragma comment(lib, "libeay32.lib")
#endif


HttpsUtil::HttpsUtil(){}

HttpsUtil::~HttpsUtil(){}

HttpsUtil* HttpsUtil::getInstance() {
    static HttpsUtil httpsClient;
    return &httpsClient;
}

CODE_STATUS HttpsUtil::getRequest(const std::string& strSvrIp, int iSvrPort,
                            const std::string& strUri, std::string& strResponse) {
    return sendDataToSvr(GET_STRING, strSvrIp, iSvrPort, strUri, "", "", strResponse);
}


CODE_STATUS HttpsUtil::sendDataToSvr(REQ_TYPE reqType, const std::string& strSvrIp, int iSvrPort, const std::string& strUri, const std::string& strBody, const std::string& strFilePath, std::string& strResponse) {
    CODE_STATUS status = CODE_FAILED;
    int socketFd = 0;
    SSL_CTX *ctx = nullptr;
    SSL *ssl = nullptr;
    do {
        char *pSvrIp = nullptr;
        struct hostent *pHostent = nullptr;
        pHostent = gethostbyname(strSvrIp.c_str());
        if (pHostent == nullptr) {
            break;
        }
        pSvrIp = inet_ntoa(*(struct in_addr *) pHostent->h_addr_list[0]);

        // 1.Establish TCP connection
        if (tcpConn(pSvrIp, iSvrPort, socketFd) != CODE_SUCCESS) {
            break;
        }

        // 2.Associating sockets to SSL, and establishing connections
        SSL_library_init();
        OpenSSL_add_all_algorithms();
        SSL_load_error_strings();
        ctx = SSL_CTX_new(SSLv23_client_method());
        if (ctx == nullptr) {
            break;
        }
        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, socketFd);
        int retCode = SSL_connect(ssl);
        if (retCode != 1) {
            int sslErrCode = SSL_get_error(ssl, retCode);
            strResponse = "SSL_connect error,openssl errCode = ";
            char errCode[11] = {0};
            sprintf(errCode, "%d", sslErrCode);
            strResponse.append(errCode);
            break;
        }

        std::string strReqData;
        if (GET_FILE == reqType || GET_STRING == reqType) {
            getGetReqData(strSvrIp, iSvrPort, strUri, strReqData);
        } else {
            // TODO : post
        }

        // 4.Send data by SSL
        int writeLen = SSL_write(ssl, strReqData.c_str(), strReqData.length());
        if (writeLen <= 0) {
            int sslErrCode = SSL_get_error(ssl, writeLen);
            strResponse = "SSL_write error,openssl errCode = ";
            char errCode[11] = {0};
            sprintf(errCode, "%d", sslErrCode);
            strResponse.append(errCode);
            break;
        }

        // 5.Read response data
        int readLen = 0;
        char pHeader[1] = {0};
        int i = 0;
        while ((readLen = SSL_read(ssl, pHeader, 1)) == 1) {
            if (i < 4) {
                if (pHeader[0] == '\r' || pHeader[0] == '\n') {
                    i++;
                    if (i >= 4) {
                        break;
                    }
                } else {
                    i = 0;
                }
            }
        }

        if (readLen < 0) {
            int sslErrCode = SSL_get_error(ssl, readLen);
            strResponse = "SSL_read error,openssl errCode = ";
            char errCode[11] = {0};
            sprintf(errCode, "%d", sslErrCode);
            strResponse.append(errCode);
            break;
        }

        if (reqType == GET_FILE ) {
            status = readResponseToFile(ssl, strFilePath, strResponse);
        } else {
            status = readResponseToString(ssl, strResponse);
        }

    } while (false);


    // 6.Close the socket and disconnect
    if (&socket) {
#ifdef WIN32
        closesocket(socketFd);
#else
        close(socketFd);
#endif
    }
    if (ctx) {
        SSL_CTX_free(ctx);
    }
    if (ssl) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }

    return status;
}

CODE_STATUS HttpsUtil::tcpConn(const char* pSvrIp, int iSvrPort, int& socket)
{
    socket = ::socket(AF_INET,SOCK_STREAM,0);
    if( socket == -1 )
    {
        return CODE_FAILED;
    }

    sockaddr_in sa{};
    sa.sin_addr.s_addr = inet_addr(pSvrIp);
    sa.sin_port = htons(iSvrPort);
    sa.sin_family = AF_INET;

    int retCode = ::connect(socket,(struct sockaddr*)&sa,sizeof(sa));
    if(retCode == -1)
    {
        return CODE_FAILED;
    }

    return CODE_SUCCESS;
}

CODE_STATUS HttpsUtil::getGetReqData(const std::string& strSvrIp, int iSvrPort,
                               const std::string& strUri, std::string& strReqData)
{
    char pLine[256] = {0};
    sprintf(pLine, "GET %s HTTP/1.1\r\n", strUri.c_str());
    strReqData.append(pLine);
    memset(pLine, 0, sizeof(pLine));
    sprintf(pLine, "Host: %s:%d\r\n",strSvrIp.c_str(), iSvrPort);
    strReqData.append(pLine);
    memset(pLine, 0, sizeof(pLine));
    strReqData.append("Accept: */*\r\n");
    strReqData.append("Connection: close\r\n\r\n");
    return CODE_SUCCESS;
}

CODE_STATUS HttpsUtil::readResponseToString(SSL* ssl, std::string& strRespData) {
    char pBody[1024 + 1] = {0};
    int readSize = sizeof(pBody) - 1;
    int readLen = 0;
    while ((readLen = SSL_read(ssl, pBody, readSize)) > 0) {
        strRespData.append(pBody);
        memset(pBody, 0, sizeof(pBody));
    }
    if (readLen < 0) {
        int sslErrCode = SSL_get_error(ssl, readLen);
        strRespData = "SSL_read error,openssl errCode = ";
        char errCode[11] = {0};
        sprintf(errCode, "%d", sslErrCode);
        strRespData.append(errCode);
        return CODE_FAILED;
    }
    strRespData.append(pBody);

    return CODE_SUCCESS;
}

CODE_STATUS HttpsUtil::readResponseToFile(SSL* ssl, const std::string& strFilePath, std::string& strErrMsg) {

   // TODO : read to file
   return CODE_SUCCESS;
}
