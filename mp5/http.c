#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "http.h"


/**
 * httprequest_parse_headers
 * 
 * Populate a `req` with the contents of `buffer`, returning the number of bytes used from `buf`.
 */
ssize_t httprequest_parse_headers(HTTPRequest *req, char *buffer, ssize_t buffer_len) {
  // pristine buffer, unmodified by strtok 
  char* buffer2 = malloc(buffer_len);
  memcpy(buffer2, buffer, buffer_len);

  char** line_arr = calloc(1, buffer_len);
  req->headers = NULL;
  req->payload = NULL;
  ssize_t bytes_used = buffer_len;
  int reqContainsPayload = 0;
  int payload_size = 0;

  char delim_line[] = "\r\n";
  char* ptr = strtok(buffer, delim_line);
  int line_arr_idx = 0;
  while(ptr != NULL) {
    line_arr[line_arr_idx] = calloc(1, strlen(ptr) + 1);
    // has to be memcpy??
    strcpy(line_arr[line_arr_idx], ptr);
    bytes_used -= 2;
		ptr = strtok(NULL, delim_line);
    line_arr_idx++;
	}

  char delim_space[] = " ";
  char delim_colon[] = ": ";
  // indicates line count
  int line_counter = 0;
  char* ptr1 = strtok(line_arr[line_counter], delim_space);

  // indicates space count
  int space_idx = 0;
  // populate Request Line: ACTION, PATH, VERSION
  while (space_idx < 3 && ptr1 != NULL) {

    if (space_idx == 0) {
      char* tmp = calloc(1, strlen(ptr1) + 1);
      strcpy(tmp, ptr1);
      bytes_used -= 1;
      req->action = tmp;
    } else if (space_idx == 1) {
      char* tmp = calloc(1, strlen(ptr1) + 1);
      strcpy(tmp, ptr1);
      bytes_used -= 1;
      req->path = tmp;
    } else { // space_idx == 2
      char* tmp = calloc(1, strlen(ptr1) + 1);
      strcpy(tmp, ptr1);
      bytes_used -= 1;
      req->version = tmp;
    }

    ptr1 = strtok(NULL, delim_space);
    space_idx++;
  }

  // line_count = 1, so we parse headers
  line_counter++;

  char* isHeader = strstr(line_arr[line_counter], delim_colon);

  while (isHeader) { // if true, line_arr[line_counter] is a header

    char* ptr2 = strtok(line_arr[line_counter], delim_colon);
    int colon_idx = 0;
    Header* header = malloc(sizeof(Header));
    int isContentLengthHeader = 0;

    if (strstr(ptr2, "Content-Length")) {
      isContentLengthHeader = 1;
    }

    // populate header->KEY, header->VALUE
    while(colon_idx < 2 && ptr2 != NULL) {

      if (colon_idx == 0) { // KEY
        char* tmp = calloc(1, strlen(ptr2) + 1);
        strcpy(tmp, ptr2);
        bytes_used -= 2;
        header->key = tmp;

      } else { // colon_idx == 2, VALUE
        char* tmp = calloc(1, strlen(ptr2) + 1);
        strcpy(tmp, ptr2);
        bytes_used -= 2;
        header->value = tmp;

      }
      header->next = NULL;
      ptr2 = strtok(NULL, delim_colon);
      colon_idx++;
    }

    // add header struct to the linked list of headers in the request struct
    add_header_to_request(req, header);

    if (isContentLengthHeader) {
      req->payload = calloc(1, atoi(header->value) + 1);
      payload_size = atoi(header->value);
      reqContainsPayload = 1;
    }

    line_counter++;
    if (line_arr[line_counter] == NULL) {
      break;
    }
    isHeader = strstr(line_arr[line_counter], delim_colon);
  }

  // at this point, there are no more header lines. Only the payload remains.

  if (reqContainsPayload) {
    //char delim_double[] = "\r\n\r\n";
    // buffer_len - payload_size is the index of the beginning of the payload
    const char* payload_data = buffer2;
    // get rid of /r/n/r/n and
    payload_data = payload_data + (buffer_len - payload_size);
    memcpy(req->payload, payload_data, payload_size);
    
  }

  // free the linearr
  for (ssize_t i = 0; i <= line_arr_idx; i++) {
    if (line_arr[i] != NULL) {
      free(line_arr[i]);
    }
  }
  free(line_arr);
  free(buffer2);

  return bytes_used;
}

void add_header_to_request(HTTPRequest *req, Header* header) {
  // if null, add to head
  if (req->headers == NULL) {
    req->headers = header;
    return;
  }

  // else: add to end of header linked list
  Header* iter = req->headers;
  while (iter->next != NULL) {
    iter = iter->next;
  }
  iter->next = header;
}


/**
 * httprequest_read
 * 
 * Populate a `req` from the socket `sockfd`, returning the number of bytes read to populate `req`.
 */
ssize_t httprequest_read(HTTPRequest *req, int sockfd) {
  // the large buffer size is replaced by buffer_len
  char* buffer = calloc(1, 5000);
  // sockfd is fd[0], or the READ file descriptor (the output end)
  int buffer_len = read(sockfd, buffer, 4999);

  if (buffer_len < 0) {
    perror("read");
    return -1;
  }

  //printf("%d\n", buffer_len);
  //printf("%s\n", buffer);

  ssize_t bytes_used = httprequest_parse_headers(req, buffer, buffer_len);

  free(buffer);
  
  return bytes_used;
}


/**
 * httprequest_get_action
 * 
 * Returns the HTTP action verb for a given `req`.
 */
const char *httprequest_get_action(HTTPRequest *req) {
  return req->action;
}


/**
 * httprequest_get_header
 * 
 * Returns the value of the HTTP header `key` for a given `req`.
 */
const char *httprequest_get_header(HTTPRequest *req, const char *key) {
  Header* tmp = req->headers;
  while (tmp != NULL) {
    if (strcmp(tmp->key, key) == 0) {
      return tmp->value;
    } 
    tmp = tmp->next;
  }
  return NULL;
}


/**
 * httprequest_get_path
 * 
 * Returns the requested path for a given `req`.
 */
const char *httprequest_get_path(HTTPRequest *req) {
  return req->path;
}


/**
 * httprequest_destroy
 * 
 * Destroys a `req`, freeing all associated memory.
 */
void httprequest_destroy(HTTPRequest *req) {
  free((char*)req->action);
  free((char*)req->path);
  free((char*)req->version);
  free((void*)req->payload);
  
  Header* tmp = req->headers;
  while (tmp != NULL) {
    Header* next = tmp->next;
    free(tmp->key);
    free(tmp->value);
    free(tmp);
    tmp = next;
  }
}