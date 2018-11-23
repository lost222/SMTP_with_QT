//
// Created by 杨起 on 2018/10/28.
//

#ifndef SMTP_SERVER_MY_BASE64_H
#define SMTP_SERVER_MY_BASE64_H

#include <string>
#include <vector>
#include <map>
#include <WinSock2.h>
#include <windows.h>
#include<ostream>
#include<fstream>
#include<QObject>
//#include "listenthread.h"

const char reply_code[][100]={
        {" \r\n"},  //0
        {"200 Mail OK.\r\n"},   //1
        {"211 System status, or system help reply.\r\n"},  //2
        {"214 Help message.\r\n"},   //3
        {"220 Ready\r\n"},  //4
        {"221 Bye\r\n"},  //5
        {"250 OK\r\n"},  //6
        {"251 User not local; will forward to %s<forward-path>.\r\n"},  //7

        {"354 End data with <CR><LF>.<CR><LF>\r\n"},  //8
        {"421 service not available, closing transmission channel\r\n"},  //9
        {"450 Requested mail action not taken: mailbox unavailable\r\n"},  //10
        {"451 Requested action aborted: local error in processing\r\n"},   //11
        {"452 Requested action not taken: insufficient system storage\r\n"}, //12
        {"500 Syntax error, command unrecognised\r\n"},  //13
        {"501 Syntax error in parameters or arguments\r\n"},  //14
        {"502 Error: command not implemented\r\n"},  //15
        {"503 Error: bad sequence of commands\r\n"}, //16
        {"504 Error: command parameter not implemented\r\n"},  //17
        {"521 <domain>%s does not accept mail (see rfc1846)\r\n"},  //18
        {"530 Access denied (???a Sendmailism)\r\n"},  //19
        {"550 Requested action not taken: mailbox unavailable\r\n"},  //20
        {"551 User not local; please try another User\r\n"},  //21
        {"552 Requested mail action aborted: exceeded storage allocation\r\n"},  //22
        {"553 authentication is required\r\n"},  //23

        {"250-mail \n250-PIPELINING \n250-AUTH LOGIN PLAIN\n250-AUTH=LOGIN PLAIN\n250 8BITMIME\r\n"},  //24
        {"334 dXNlcm5hbWU6\r\n"},  //25, "dXNlcm5hbWU6" is "username:"'s base64 code
        {"334 UGFzc3dvcmQ6\r\n"},  //26, "UGFzc3dvcmQ6" is "Password:"'s base64 code
        {"235 Authentication successful\r\n"} //27
};

const std::string PROJECT_PATH = "C:\\code\\GUI_smtp\\";


class my_base64 {
private:
    std::string to_decode;
    int char2int(char&);
public:
    my_base64(std::string &input_s);
    std::vector<unsigned char> decode2num();
    std::string decode2string();
};

class SMTPServer: public QObject {

    Q_OBJECT

private:
    std::vector<std::string> users;
    std::vector<std::string> passwd;
    std::vector<std::string> to_mails;
    int usernum; 
    bool isdata; 
    int islogin;
    std::string data;
    std::string log_user;
public:
    explicit SMTPServer(QObject *parent = 0);
//    SMTPServer();
    bool deal_with_users(std::string user);
    const char* respond(const char *);
    void save_mail();
    bool is_log_ok(std::string log_username, std::string log_passwd);

signals:
    void sig_mail_saved(const QString&);

public slots:
};




#endif //SMTP_SERVER_MY_BASE64_H
