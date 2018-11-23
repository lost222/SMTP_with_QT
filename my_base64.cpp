//
// Created by 杨起 on 2018/10/28.
//
#include <Winsock2.h>
#include <iostream>
#include<QDebug>
#include<algorithm>
#include "my_base64.h"

int my_base64::char2int(char &c) {
    if(c=='+') return 62;
    if(c=='/') return 63;
    if(isalnum(c)){
        if(isupper(c)) return c-65;
        if(isalpha(c)) return c-71;
        return c+4;
    }
    return 0;
}

bool is_not_b64_member(char &c){
    if (isalnum(c) || c=='+' || c=='/' || c=='='){return false;}
    return true;
}

my_base64::my_base64(std::string &input_s) {
    this->to_decode = input_s;
    to_decode.erase(remove_if(to_decode.begin(), to_decode.end(), is_not_b64_member), to_decode.end());
    std::cout<<"Length : "<<to_decode.length()<<std::endl;
    int i=0;
    if (to_decode.length() % 4 != 0){
        i = to_decode.length() % 4;
        for(int j=0; j<i;j++){
            to_decode.erase(to_decode.end() - 1);
        }
    }
}
std::vector<unsigned char> my_base64::decode2num() {
    std::vector<unsigned char> result;
    // 考虑溢出情况
    for(int i=0;i<to_decode.length(); i+=4){
        unsigned long sum=0;
        for(int j =0;j<4;j++){
            //if(j>1&& to_decode[i+j] == '='){ continue;}
            unsigned int ans = (unsigned int)char2int(to_decode[i+j]);
            sum +=  ans << (18-6*j);  // 考虑返回-1
        }
        unsigned int put_in_3 = sum&0xff;
        sum = sum >> 8;
        unsigned int put_in_2 = sum&0xff;
        sum = sum >> 8;
        unsigned int put_in_1 = sum&0xff;
       // std::cout<<put_in_1<<" "<<put_in_2<<" "<<put_in_3<<std::endl;
        result.push_back(put_in_1);
        result.push_back(put_in_2);
        result.push_back(put_in_3);
    }
    std::cout<<"OUT of LOOP"<<std::endl;
    if(*(result.end()-1)==0) {result.erase(result.end()-1);}
    if(*(result.end()-1)==0) {result.erase(result.end()-1);}
    std::cout<<"END remove end"<<std::endl;
    return result;
}

std::string my_base64::decode2string() {
    std::vector<unsigned char> num;
    std::cout<<"SIZE NUM : "<<num.size()<<std::endl;
    num = this->decode2num();
    std::cout<<"SIZE NUM : "<<num.size()<<std::endl;
    std::string result = std::string(num.begin(), num.end());
    return result;
}

//ListenThread::ListenThread(QObject *parent) : QThread(parent)
//{
//    stopped = false;
//}

SMTPServer::SMTPServer(QObject *parent) : QObject(parent){
    // 读入用户列表， 为每一个用户创建一个文件夹
	this->isdata = false;
	this->usernum = -1;
    std::fstream usertxt;
    std::string uPath = PROJECT_PATH+"userdata\\users.txt";
    usertxt.open(uPath);
    if (!usertxt.is_open()) { exit(-1); }
    std::string user;
    std::getline(usertxt, user);
    while (!user.empty()){
        std::size_t sp = user.find('-');
        std::string pswd = user.substr(sp+1, user.size());
        user = user.substr(0, sp);
        users.push_back(user);
        this->passwd.push_back(pswd);
        std::getline(usertxt, user);
    }
    usertxt.close();



    for(std::vector<std::string>::iterator i = users.begin();i != users.end(); i++){
       std::string path = PROJECT_PATH + "userdata\\" + *i;
       CreateDirectory((LPCTSTR)path.c_str(),NULL);
    }
}

const char* SMTPServer::respond(const char* request) {
    //user name
    if (islogin == 1) {
        // 保存用户名
        std::string re = std::string(request);
        my_base64 de_re = my_base64(re);
        std::string username = de_re.decode2string();
//        QByteArray re = QByteArray(request);
//        QByteArray barray = QByteArray::fromBase64(re);
//        QString u = QString(barray);
//        std::string username = u.toStdString();
        this->log_user = username;
        // 要密码
        islogin++;
        return reply_code[26];
    }

    // password
    if ( islogin == 2 ) {
        // save password
        std::string re = std::string(request);
        my_base64 de_re = my_base64(re);
        std::string password = de_re.decode2string();
//        QByteArray re = QByteArray(request);
//        QByteArray barray = QByteArray::fromBase64(re);
//        QString u = QString(barray);
//        std::string password = u.toStdString();
        bool ok = is_log_ok(log_user, password);
        if ( ok ){
            islogin = 0; // end login
            return reply_code[27];
        }
        return reply_code[21];

        //检查是不是匹配
    }

    //DATA
    if (this->isdata){
        this->data += request;
//		int i=0;
		std::size_t rfound = data.rfind(".\r\n");
		if(rfound == std::string::npos){return "Nsend";}
		this->save_mail();
		isdata = false;
        islogin = 0;
		return reply_code[6];
    }
    // HELO
    if (strncmp(request, "EHLO", 4) == 0) {
        return reply_code[6];
    }
    //AUTH LOGIN
    if (strncmp(request, "AUTH", 4) == 0) {
       islogin = 1;
       // 要用户名
       return reply_code[25];
    }
    //

    // MAIL FROM : 判断是不是和userlist 里的一样 不一样返回502
    if (strncmp(request, "MAIL FROM", 9) == 0) {
        std::string user= std::string(request);
        user = user.substr(user.find('<')+1, user.find('@') - user.find('<') - 1);
        bool is_user = deal_with_users(user);
        if (is_user){return reply_code[6];}
        else return reply_code[16];
    }
    // MAIL TO :  添加 to_mail数组， 已备等等写文件
    if (strncmp(request, "RCPT TO", 7) == 0) {
        std::string toMan = std::string(request);
        toMan = toMan.substr(toMan.find('<'), toMan.find('>') - toMan.find('<') + 1);
        to_mails.push_back(toMan);
        return reply_code[6];
    }
    // DATA ： 不遇到 . 不能停下
    if (strncmp(request, "DATA", 4) == 0) {
        isdata = true;
        return reply_code[8];
    }
    // QUIT ： 看看怎么关闭链接
    if (strncmp(request, "QUIT", 4) == 0) {
        return reply_code[5];
    }

    return reply_code[15];
}

bool SMTPServer::deal_with_users(std::string user) {
    for(unsigned int i=0;i<users.size();i++){
        if (user == users[i]) {
            usernum = i;
            return true;
        }
    }
    return false;
}


void SMTPServer::save_mail() {
    std::string path = PROJECT_PATH+"userdata\\" + users[usernum] + "\\";
    // 对每一个要发送的对象分别保存信息
    SYSTEMTIME date;
    GetSystemTime(&date);
    std::string to = to_mails[0].substr(1,to_mails[0].size() - 2);
    std::string oPath = path+to+".txt";
    std::ofstream mail_file(oPath);
    if (!mail_file.is_open()) {exit(-1);}
    mail_file<<data;
    mail_file.close();
    emit sig_mail_saved(QString::fromStdString(oPath));
    return ;
}

bool SMTPServer::is_log_ok(std::string log_username, std::string log_passwd)
{
    int match = 0;
    while( log_username != users[match] && match < users.size() ){match++;}
    return this->passwd[match] == log_passwd;
}
