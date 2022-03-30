#include "http.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <pthread.h>

#define OK "HTTP/1.1 200 OK\r\n"
#define NOT_FOUND "HTTP/1.1 404 Not Found\r\n"
#define SPACE " "
#define CONT_KEY "Content-Type: "
#define PNG_CONT_TYPE "image/png\r\n\r\n"
#define HTML_CONT_TYPE "text/html\r\n\r\n"

// void print_req(HTTPRequest* req) {
//   printf("%s\n", "~~~New Request~~~");
//   printf("\t%s %s %s\n", req->action, req->path, req->version);
  
//   Header* iter = req->headers;
//   while (iter!=NULL) {
//     printf("\t%s%s%s\n", iter->key, ": ", iter->value);
//     iter = iter->next;
//   }

//   printf("\t%s\n", (char*) req->payload);


// }

void *client_thread(void *vptr) {
  int fd = *((int *)vptr);
  // reads data into the req obj
  HTTPRequest* req = malloc(sizeof(HTTPRequest));
  char* response = calloc(1, 10001);
  int bytes_written = 0;
  httprequest_read(req, fd);

  if (req == NULL) {
    printf("null request");
    return NULL;
  }

  // print_req(req);

  char file_path[500] = "static";

  char* first_char_of_path = calloc(1,2);
  strncpy(first_char_of_path, req->path, 1);
  FILE* fp;

  // char* content_type_val; // ?

  // incremental write
    // write all headers to socket at once
    // get payload size, get payload, and write payload all at once
  // close socket after writing payload (so data gets sent)


  // response line
  // headers: CONT_TYPE
  // payload

  // if path is '/', treat as index
  if (strcmp(req->path, "/") == 0) {
    strcat(file_path, "/index.html");

    strcat(response, OK);
    bytes_written += strlen(OK);
    // at this point: response line completed

    strcat(response, CONT_KEY);
    bytes_written += strlen(CONT_KEY);
    strcat(response, HTML_CONT_TYPE);
    bytes_written += strlen(HTML_CONT_TYPE);

    // write the response line + header contents
    write(fd, response, bytes_written);
    //printf("%s\n", response);


    fp = fopen(file_path, "r");
    fseek(fp, 0L, SEEK_END);
    int len = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    char* response_payload = malloc(len);
    fread(response_payload, sizeof(char), len, fp);

    // write the payload to the socket
    write(fd, response_payload, len);
    //printf("correct payload:\n%s\n", response_payload);

  } else if (strcmp(first_char_of_path, "/") == 0) {
    // req->path without the beginning '/'
    char* tmp = req->path + 1;

    strcat(response, OK);
    bytes_written += strlen(OK);
    strcat(response, CONT_KEY);
    bytes_written += strlen(CONT_KEY);

    if (strcmp(tmp, "getaway.html") == 0) {
      strcat(file_path, "/getaway.html");

      strcat(response, HTML_CONT_TYPE);
      bytes_written += strlen(HTML_CONT_TYPE);

      // write response line and header contents
      write(fd, response, bytes_written);
      //printf("@1 %s\n", response);

      fp = fopen(file_path, "r");
      fseek(fp, 0L, SEEK_END);
      int len = ftell(fp);
      fseek(fp, 0L, SEEK_SET);

      char* response_payload = malloc(len);
      fread(response_payload, sizeof(char), len, fp);

      // write the payload to the socket
      write(fd, response_payload, len);
      //printf("@2 %s\n", response_payload);

      // add stuff to response string

    } else if (strcmp(tmp, "index.html") == 0) {
      strcat(file_path, "/index.html");

      strcat(response, HTML_CONT_TYPE);
      bytes_written += strlen(HTML_CONT_TYPE);

      // write response line and header contents
      write(fd, response, bytes_written);
      //printf("@3 %s\n", response);

      fp = fopen(file_path, "r");
      fseek(fp, 0L, SEEK_END);
      int len = ftell(fp);
      fseek(fp, 0L, SEEK_SET);

      char* response_payload = malloc(len);
      fread(response_payload, sizeof(char), len, fp);

      // write the payload to the socket
      write(fd, response_payload, len);
      //printf("@4 %s\n", response_payload);

      // add stuff to response string

    } else if (strcmp(tmp, "240.png") == 0) {
      strcat(file_path, "/240.png");

      strcat(response, PNG_CONT_TYPE);
      bytes_written += strlen(PNG_CONT_TYPE);

      // write response line and header contents
      write(fd, response, bytes_written);
      //printf("@5 %s\n", response);

      fp = fopen(file_path, "r");
      fseek(fp, 0L, SEEK_END);
      int len = ftell(fp);
      fseek(fp, 0L, SEEK_SET);

      char* response_payload = malloc(len);
      fread(response_payload, sizeof(char), len, fp);

      // write the payload to the socket
      write(fd, response_payload, len);
      //printf("@6 %s\n", response_payload);

    } else {
      strcpy(response, NOT_FOUND);
      bytes_written += strlen(NOT_FOUND);

      write(fd, response, bytes_written);
      //printf("@7 %s\n", response);
    }

  } else {
    strcpy(response, NOT_FOUND);
    bytes_written += strlen(NOT_FOUND);

    write(fd, response, bytes_written);
    //printf("@8 %s\n", response);
  }

  free(first_char_of_path);
  fclose(fp);
  close(fd);

  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: %s <port>\n", argv[0]);
    return 1;
  }
  int port = atoi(argv[1]);
  printf("Binding to port %d. Visit http://localhost:%d/ to interact with your server!\n", port, port);

  // socket:
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // bind:
  struct sockaddr_in server_addr, client_address;
  memset(&server_addr, 0x00, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);  
  bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr));

  // listen:
  listen(sockfd, 10);

  // accept:
  socklen_t client_addr_len;
  while (1) {
    int *fd = malloc(sizeof(int));
    *fd = accept(sockfd, (struct sockaddr *)&client_address, &client_addr_len);
    printf("Client connected (fd=%d)\n", *fd);

    pthread_t tid;
    pthread_create(&tid, NULL, client_thread, fd);
    pthread_detach(tid);
  }

  return 0;
}