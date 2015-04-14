/*
//  http_download.c
//  networking
//
//  Created by Chris on 4/13/15.
//  Copyright (c) 2015 A and C Studios. All rights reserved.
*/

#include "http_download.h"
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>

#define BUF_SIZE 4096
#define true 1
#define false 0

struct net_header *header_create()
{
    struct net_header *h;
    h = calloc(1, sizeof(struct net_header));
    h->next = NULL;
    h->name = NULL;
    h->content = NULL;
    return h;
}

struct net_header *header_copy(struct net_header *h)
{
    struct net_header *header;
    header = header_create();
    if (h->content)
        header->content = strdup(h->content);
    if (header->name)
        header->name = strdup(h->name);
    header->next = h->next;
    return header;
}

int parse_headers(char **str, struct net_header *header)
{
    char *pch/*, *head, *body*/;
    struct net_header *h, *temp;
    
    pch = strtok(*str, "\n");

    header->name = strdup(pch);
    
    h = header_create();
    header->next = h;
    
    while ((pch = strtok(NULL, "\n")) != NULL)
    {
        if (strcmp(pch, "\r") == 0)
            break;
        
        temp = header_create();
        
        h->name = strdup(pch);
        
        h->next = temp;
        
        h = temp;
    }
    *str = strdup(strtok(NULL, ""));

    h = header;
    pch = strtok(h->name, ":");
    if (!pch)
        h->content = "";
    else
        h->content = pch;
    while ((h = h->next) != NULL)
    {
        pch = strtok(h->name, ":");
        if (!pch)
            h->content = "";
        else
            h->content = strtok(NULL, "\r");
    }
    
    return N_SUCCESS;
}

int net_httpDownload(struct net_httpRequest *req, char **res)
{
    struct addrinfo *inf;
    struct addrinfo hints;
    char *buf, *temp, *url, *host, *path, *split, *request, *retval;
    int sock, status, path_malloc;
    size_t bytes, total_bytes, reqlen;
    struct net_header *header;
    long int len;
        
    total_bytes = 0;

    path_malloc = false; /* whether or not path should be freed */
    
    url = strdup(req->addr); /* to prevent memory issues */
    
    split = strtok(url, "/"); /* separates URL into hostname and path */
    if (split != NULL) /* there is a / in the URL */
    {
        host = strdup(split);
        path = strtok(NULL, "/");
        
        if (path)
        {
            reqlen = strlen(path) + 2;
            temp = strdup(path);
            path = malloc(reqlen);
            strcpy(path, "/");
            strcat(path, temp);
            
            path_malloc = true;
            
            free(temp);
        }
    }
    else /* there is no / in the URL */
    {
        host = strdup(url);
        path = "/", path_malloc = false;
    }
    
    if (!path)
        path = "/", path_malloc = false;
    
    if (!req->addr || strlen(req->addr) == 0)
        return N_INVALID_REQUEST;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_protocol = 0;
    
    if ((status = getaddrinfo(url, "80", &hints, &inf)) != 0) /* get info for host */
    {
        fprintf(stderr, "Error getaddrinfo: %s\n", gai_strerror(errno));
        return N_GETADDRINFO_ERROR;
    }
    
    sock = socket(inf->ai_family, inf->ai_socktype, inf->ai_protocol); /* open socket */
    if (sock == -1)
    {
        fprintf(stderr, "Socket error: %s\n", strerror(errno));
        return N_SOCKET_ERROR;
    }
    
    status = connect(sock, inf->ai_addr, inf->ai_addrlen); /* connect to host */
    if (status < 0)
    {
        fprintf(stderr, "Error connecting to host: %s\n", strerror(errno));
        return N_CONNECTION_ERROR;
    }
    
    reqlen = strlen("GET ") + strlen(path) + strlen(" HTTP/1.1\r\n") + strlen("Host: ") + strlen(host) + strlen("\r\n\r\n"); /* set up HTTP request */
    request = malloc(reqlen + 1);
    snprintf(request, reqlen + 1, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", path, host);
    
    status = (int)send(sock, request, reqlen + 1, 0); /* send HTTP request */
    if (status < 0)
    {
        fprintf(stderr, "Error sending request: %s\n", gai_strerror(errno));
        return N_REQUESTSEND_ERROR;
    }
    
    buf = malloc(BUF_SIZE * sizeof(char)); /* buffer for receiving data */
    retval = malloc(BUF_SIZE * sizeof(char)); /* buffer for appending data */
    
    while ((bytes = recv(sock, buf, BUF_SIZE, 0)) > 0) /* receive data from server */
    {
        printf("%lu \n", bytes);
        
        total_bytes += bytes;
        temp = strdup(retval);
        
        /* free(retval); */
        retval = realloc(retval, total_bytes + 1);
        strcpy(retval, temp);
        strcat(retval, buf);
        
        free(temp);
    }
    
    header = header_create();
    parse_headers(&retval, header);
    
    do
    {
        if (strcmp(header->name, "Content-Length") == 0)
        {
            sscanf(header->content, "%lu", &len);
            retval[len + 1] = '\0';
            break;
        }
    } while ((header = header->next) != NULL);
    
    *res = strdup(retval); /* set passed argument to proper value */

    if (path_malloc == true)
        free(path);
    free(retval);
    free(buf);
    freeaddrinfo(inf);
    free(header);
    
    return N_SUCCESS;
}