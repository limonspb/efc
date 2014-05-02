#ifndef BASECOMMAND_H
#define BASECOMMAND_H

#include "defines.h"
#include "commandtypes.h"
#include "commanddata.h"
#include "reactions.h"

class Base_Command
{
public:
    Base_Command(CommandType type);
    virtual ~Base_Command(){}
    virtual bool deserialize(char * data, quint32 dataSize) = 0;
    virtual void serialize(char * data, quint32 & dataSize) = 0;    
    virtual quint32 serializedSize() = 0;    
    void exec();
protected:


    virtual Base_CommandData * getData() = 0;

    CommandType _type;

    void (* _reaction)(Base_CommandData *);
};

template<class T>
class Command : public Base_Command{
public:
    Command(CommandType type)
        : Base_Command(type)
    {
        ((Base_CommandData*)(&this->_msg._data))->type = type;
        this->_msg.start = '\t';
        this->_msg.end = '\n';
        this->_msg.size = sizeof(T);
    }


    virtual bool deserialize(char * data, quint32 dataSize){
        bool result = false;
        if (dataSize == this->serializedSize()){
            quint8 internalDataSize = data[1];
            if (internalDataSize == sizeof(T)){
                memcpy(&_msg._data, data+2, sizeof(T));
                if (quint8(_type) == _msg._data.type){
                    result = true;
                }
            }
        }
        return result;
    }

    virtual void serialize(char * data, quint32 & dataSize){
        memcpy(data+2, &_msg._data, sizeof(T));
        data[sizeof(T)+2] = '\n';
        data[0] = '\t';
        data[1] = sizeof(T);
        dataSize = sizeof(T) + 3;
    }

    virtual quint32 serializedSize(){
        return sizeof(T)  + 3;
    }

    T & data(){
        return _msg._data;
    }

    void * msg(){
        return &this->_msg;
    }

protected:

    virtual Base_CommandData * getData(){
        //T * result = new T();
        //memcpy(result, &_data, sizeof(T));
        //return result;
        return &_msg._data;
    }
    #pragma pack(push, 1)
    struct Message{
        quint8 start;
        quint8 size;
        T _data;
        quint8 end;
    } _msg;
    #pragma pack(pop)
};

/*class Reset_Command : public Base_Command{
public:
    Reset_Command(CommandType type);
    virtual bool deserialize(char * data, quint32 dataSize);
    virtual void serialize(char * data, quint32 & dataSize);
    virtual quint32 serializedSize();
protected:

    virtual Base_CommandData * getData();

    Reset_CommandData _data;
};


class TestNumber_Command : public Base_Command{
public:
    TestNumber_Command(CommandType type);    

    virtual bool deserialize(char * data, quint32 dataSize);
    virtual void serialize(char * data, quint32 & dataSize);
    virtual quint32 serializedSize();
protected:
    virtual Base_CommandData * getData();

    TestNumber_CommandData _data;
};
*/


#endif // BASECOMMAND_H
