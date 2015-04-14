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

int net_httpDownload(struct net_httpRequest *req, char **buf);

#endif /* defined(__networking__html_download__) */
