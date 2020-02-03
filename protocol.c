//
// Created by javif on 12/12/2019.
//

#include <protocol.h>
#include <fileManager.h>

Protocol *newProtocol(){
    Protocol *_protocol;
    _protocol = calloc(1, sizeof(Protocol));
    _protocol->header = calloc(1, 1);
    _protocol->data = calloc(1, 1);
    _protocol->id = '0';
    _protocol->length[0] = 0;
    _protocol->length[1] = 0;

    return _protocol;
}

void fillProtocol(Protocol *_p, char _id, char * _header, char *_data){
    // empty-ing
    // memset(_p->header,0,strlen(_p->header)); // empty-ing header
    // memset(_p->header,0,strlen(_p->length)); // empty-ing header
    // memset(_p->header,0,strlen(_p->data)); // empty-ing header

    _p->header = realloc(_p->header, strlen(_header)+1);
    int _len = strlen(_data);
    if(_len > 99) // maximum size of data: 99
        _len = 99;
    char _length[3];
    if(_len >= 10)
        sprintf(_length, "%d", _len);
    else if(_len < 10)
        sprintf(_length, "0%d", _len);

   // _p->length = realloc(_p->length, 2);
    _p->data   = realloc(_p->data, _len);

    // filling
    _p->id = _id;
    strcpy(_p->header, _header);
    strncpy(_p->data, _data, _len);
    _p->length[0] = _length[0];
    _p->length[1] = _length[1];
    /*strncpy(_p->length, _length, 2);
    *(_p->header) = _header;
    *(_p->data) = _data;
    *(_p->length) = _length;*/
}

void fillProtocoln(Protocol *_p, char _id, char * _header, char *_data, int _len){
    _p->header = realloc(_p->header, strlen(_header));
    memset(_p->header, 0, strlen(_header));
    if(_len > 99) // maximum size of data: 99
        _len = 99;
    char _length[3];
    if(_len >= 10){
        sprintf(_length, "%d", _len);
    }else if(_len < 10){
        sprintf(_length, "0%d", _len);
    }
    // _p->length = realloc(_p->length, 2);
    _p->data   = realloc(_p->data, _len);
    memset(_p->data, 0, _len);
    // filling
    _p->id = _id;
    memcpy(_p->header, _header, strlen(_header));
    memcpy(_p->data, _data, _len);
    _p->length[0] = _length[0];
    _p->length[1] = _length[1];
}

void freeProtocol(Protocol *_p){
    resetProtocol(_p);

    free(_p->header);
    free(_p->data);
    free(_p);
}

void resetProtocol(Protocol *_p){
    memset(_p->header,0,strlen(_p->header)); // empty-ing header
    memset(_p->length,0, sizeof(_p->length));                // empty-ing length
    memset(_p->data,0, atoi(_p->length));    // empty-ing data
    _p->header = realloc(_p->header, 1);
    _p->header[0] = 0;
    _p->data   = realloc(_p->data, 1);
    _p->data[0]   = 0;
    //_p->length = realloc(_p->length, 2);
    _p->id = '0';
}
void printProtocol(Protocol p){
    write (1, &p.id, 1);
    myprint("|");
    write (1, p.header, strlen(p.header));
    myprint("|");
    write (1, p.length, 2);
    myprint("|");
    write (1, p.data, atoi(p.length));
    myprint("\n");
}