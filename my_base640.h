//
// Created by 杨起 on 2018/11/18.
//
#include <string>
#include <vector>

#ifndef MY_BASE64_MY_BASE64_H
#define MY_BASE64_MY_BASE64_H


class my_base64 {
private:
    std::string to_decode;
    std::vector<unsigned int> decoded_num;
    std::string decoded;
    int char2int(char&);
public:
    my_base64(char *);
    const std::vector<unsigned int>& decode2num();
    const std::string& decode2string();
    ~my_base64(){};
};


#endif //MY_BASE64_MY_BASE64_H
