/*
//  http_download.h
//  networking
//
//  Created by Chris on 4/13/15.
//  Copyright (c) 2015 A and C Studios. All rights reserved.
*/

#ifndef __networking__http_download__
#define __networking__http_download__

#define N_SUCCESS 0
#define N_TIMEOUT_ERROR -10001
#define N_INVALID_REQUEST -10002
#define N_SOCKET_ERROR -10003
#define N_CONNECTION_ERROR -10004
#define N_GETADDRINFO_ERROR -10005
#define N_REQUESTSEND_ERROR -10006

#define N_PARSE_ERROR -20001

#include <stdio.h>

typedef void(*net_downloadDataReceivedCallback)(char *data, long int len);
typedef void(*net_downloadCompletedCallback)(char *data, long int len);

struct net_header
{
    char *name;
    char *content;
    struct net_header *next;
};

struct net_httpRequest
{
    char *addr;
};

void net_registerDownloadCompletedCallback(net_downloadCompletedCallback func);
void net_registerDownloadDataReceivedCallback(net_downloadDataReceivedCallback func);
int net_httpDownloadSync(struct net_httpRequest *req, char **buf);
void net_httpDownloadAsync(struct net_httpRequest *req);
char *net_Error(int err);

#endif /* defined(__networking__html_download__) */
