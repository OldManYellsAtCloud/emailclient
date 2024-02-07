#ifndef MAIL_H
#define MAIL_H

#include <string>
#include <stdint.h>

class mail {
private:
    uint32_t uid_;
    std::string body_;
    std::string subject_;
    int date_;
    std::string from_;
    std::string folder_;
    bool read_;

public:
    mail(){body_ = "";}
    ~mail(){}

    void setUid(uint32_t uid){uid_ = uid;}
    void setBody(std::string body){body_ = body;}
    void setSubject(std::string subject){subject_ = subject;}
    void setDate(int date){date_ = date;}
    void setFrom(std::string from){from_ = from;}
    void setFolder(std::string folder){folder_ = folder;}
    void setRead(bool read){read_ = read;}

    uint32_t getUid(){return uid_;}
    std::string getBody(){return body_;}
    std::string getSubject(){return subject_;}
    int getDate(){return date_;}
    std::string getFrom(){return from_;}
    std::string getFolder(){return folder_;}
    bool isRead(){return read_;}
};

#endif // MAIL_H
