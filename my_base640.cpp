//
// Created by 杨起 on 2018/11/18.
//
#include "my_base64.h"
#include <iostream>
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
my_base64::my_base64(char *input_s) {
    this->to_decode = input_s;
//    std::cout<<this->to_decode<<std::endl;
    std::size_t r = to_decode.rfind("\r\n");
    if(r != std::string::npos){
        to_decode.erase(r);
    }
}
const std::vector<unsigned int>& my_base64::decode2num() {
    // 考虑溢出情况
    for(int i=0;i<to_decode.length(); i+=4){
        unsigned long sum=0;
        for(int j =0;j<4;j++){
            //if(j>1&& to_decode[i+j] == '='){ continue;}
            sum +=  char2int(to_decode[i+j]) << (18-6*j);  // 考虑返回-1
        }
            unsigned int put_in_3 = sum&0xff;
            sum = sum >> 8;
            unsigned int put_in_2 = sum&0xff;
            sum = sum >> 8;
            unsigned int put_in_1 = sum&0xff;
            std::cout<<put_in_1<<" "<<put_in_2<<" "<<put_in_3<<std::endl;
            decoded_num.push_back(put_in_1);
            decoded_num.push_back(put_in_2);
            decoded_num.push_back(put_in_3);
    }
    if(*decoded_num.end()==0) {decoded_num.erase(decoded_num.end());}
    if(*decoded_num.end()==0) {decoded_num.erase(decoded_num.end());}
    return decoded_num;
}

const std::string& my_base64::decode2string() {
    if (decoded_num.empty()){decode2num();}
    for(int i=0;i<this->decoded_num.size();i++){
        std::cout<<this->decoded_num[i];
    }
    this->decoded = std::string(decoded_num.begin(), decoded_num.end());
//    std::cout<<decoded<<std::endl;
    return this->decoded;
}
