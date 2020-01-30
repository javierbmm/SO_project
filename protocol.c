//
// Created by javif on 12/12/2019.
//

#include <protocol.h>

Protocol *newProtocol(){
    Protocol *_protocol;
    _protocol = (Protocol*)malloc(sizeof(Protocol));
    _protocol->header = malloc(1);
    _protocol->data = malloc(1);
    _protocol->id = '0';
    _protocol->length = malloc(2);

    return _protocol;
}

void fillProtocol(Protocol *_p, char _id, char * _header, char *_data){
    // empty-ing
    // memset(_p->header,0,strlen(_p->header)); // empty-ing header
    // memset(_p->header,0,strlen(_p->length)); // empty-ing header
    // memset(_p->header,0,strlen(_p->data)); // empty-ing header

    _p->header = realloc(_p->header, strlen(_header));
    _p->data   = realloc(_p->data, strlen(_data));
    int _len = strlen(_data);
    char _length[3];
    if(_len >= 10)
        sprintf(_length, "%d", _len);
    else if(_len < 10)
        sprintf(_length, "0%d", _len);
    _p->length = realloc(_p->length, 2);

    // filling
    _p->id = _id;
    strcpy(_p->header, _header);
    strcpy(_p->data, _data);

    strncpy(_p->length, _length, 2);/*
    *(_p->header) = _header;
    *(_p->data) = _data;
    *(_p->length) = _length;*/
}

void freeProtocol(Protocol *_p){
    free(_p->header);
    free(_p->length);
    free(_p->data);
    free(_p);
}

void resetProtocol(Protocol *_p){
    memset(_p->header,0,strlen(_p->header)); // empty-ing header
    memset(_p->length,0,strlen(_p->length)); // empty-ing header
    memset(_p->data,0,strlen(_p->data)); // empty-ing header

    _p->header = realloc(_p->header, 1);
    _p->header[0] = 0;
    _p->data   = realloc(_p->data, 1);
    _p->data[0]   = 0;
    //_p->length = realloc(_p->length, 2);
    _p->id = '0';
}
void printProtocol(Protocol p){
    write (1, &p.id, 1);
    write (1, p.header, strlen(p.header));
    write (1, p.length, strlen(p.length));
    write (1, p.data, strlen(p.data));
}