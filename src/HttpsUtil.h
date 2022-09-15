#pragma once
#include <string>
#include <openssl/ssl.h>

enum REQ_TYPE {
    GET_STRING = 0,
    GET_FILE
};

enum CODE_STATUS {
    CODE_SUCCESS = 0,
    CODE_FAILED = -1
};

class HttpsUtil
{
public:
    virtual ~HttpsUtil();
    static HttpsUtil* getInstance();

    // https get
    CODE_STATUS getRequest(const std::string& strSvrIp, int iSvrPort,
                   const std::string& strUri, std::string& strResponse);

private:
    HttpsUtil();

private:

    // Establish TCP connection
    CODE_STATUS tcpConn(const char* pSvrIp, int iSvrPort, int& socket);

    // Send https request
    CODE_STATUS sendDataToSvr(REQ_TYPE reqType, const std::string& strSvrIp, int iSvrPort,
                      const std::string& strUri, const std::string& strBody, const std::string& strFilePath, std::string& strResponse );

    // Encapsulate request data
    CODE_STATUS getGetReqData(const std::string& strSvrIp, int iSvrPort,
                      const std::string& strUri, std::string& strReqData);

    // Save for GET_STRING
    CODE_STATUS readResponseToString(SSL* ssl, std::string& strRespData);

    // Save for GET_FILE
    CODE_STATUS readResponseToFile(SSL* ssl, const std::string& strFilePath, std::string& strErrMsg);
};

