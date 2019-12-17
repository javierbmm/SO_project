//
// Created by javif on 12/12/2019.
//

#include <protocol.h>

Protocol *newProtocol(){
    Protocol *_protocol;
    printf("sizeofprotocol: %d\n", sizeof(_protocol));
    _protocol = (Protocol*)malloc(sizeof(Protocol));
    printf("made a new protocol\n");
    _protocol->header = calloc(0,0);
    _protocol->data = (char*)malloc(1);
    _protocol->id = 0;
    _protocol->length = malloc(2);
    return _protocol;
}

void fillProtocol(Protocol *_p, char _id, char * _header, char * _length, char *_data){
    // empty-ing
    memset(_p->header,0,strlen(_p->header)); // empty-ing header
    memset(_p->header,0,strlen(_p->length)); // empty-ing header
    memset(_p->header,0,strlen(_p->data)); // empty-ing header

    _p->header = realloc(_p->header, strlen(_header));
    _p->data   = realloc(_p->data, strlen(_data));
    _p->length = realloc(_p->length, strlen(_length));

    // filling
    _p->id = _id;
    strcpy(_p->header, _header);
    strcpy(_p->data, _data);
    strcpy(_p->length, _length);/*
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