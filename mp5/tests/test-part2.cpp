#include "lib/catch.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "../http.h"

HTTPRequest *_readpipe_vptr(const void *s, size_t len) {
  char *buf = malloc(len + 1);
  memcpy(buf, s, len);
  buf[len] = 0;

  HTTPRequest *req = malloc(sizeof(HTTPRequest));

  int pipefd[2];
  pipe(pipefd);
  int readfd = pipefd[0];
  int writefd = pipefd[1];

  write(writefd, buf, len);
  close(writefd);

  httprequest_read(req, readfd);
  close(readfd);

  free(buf);

  return req;
}

HTTPRequest *_readpipe(const char *s) {
  return _readpipe_vptr(s, strlen(s));
}


TEST_CASE("httprequest_read - Request Line", "[weight=1][part=2]") {
  HTTPRequest *req = _readpipe("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");

  CAPTURE( req->action );
  REQUIRE( req->action != NULL );
  CHECK( strcmp(req->action, "GET") == 0 );

  CAPTURE( req->path );
  REQUIRE( req->path != NULL );
  CHECK( strcmp(req->path, "/") == 0 );

  CAPTURE( req->version );
  REQUIRE( req->version != NULL );
  CHECK( strcmp(req->version, "HTTP/1.1") == 0 );

  httprequest_destroy(req);
  free(req);
}

TEST_CASE("httprequest_read - First Header", "[weight=1][part=2]") {
  HTTPRequest *req = _readpipe("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");

  const char *host = httprequest_get_header(req, "Host");
  CAPTURE( host );
  REQUIRE( host != NULL );
  CHECK( strcmp(host, "localhost") == 0 );

  httprequest_destroy(req);
  free(req);
}

TEST_CASE("httprequest_read - Two Headers", "[weight=1][part=2]") {
  HTTPRequest *req = _readpipe("GET / HTTP/1.1\r\nHello: World\r\nHost: 127.0.0.1\r\n\r\n");

  const char *hello = httprequest_get_header(req, "Hello");
  CAPTURE( hello );
  REQUIRE( hello != NULL );
  CHECK( strcmp(hello, "World") == 0 );

  const char *host = httprequest_get_header(req, "Host");
  CAPTURE( host );
  REQUIRE( host != NULL );
  CHECK( strcmp(host, "127.0.0.1") == 0 );

  httprequest_destroy(req);
  free(req);
}

TEST_CASE("httprequest_read - Content-Length Header", "[weight=1][part=2]") {
  HTTPRequest *req = _readpipe("GET / HTTP/1.1\r\nHello: World\r\nHost: 127.0.0.1\r\nContent-Length: 10\r\n\r\n0123456789");

  const char *content_length = httprequest_get_header(req, "Content-Length");
  CAPTURE( content_length );
  REQUIRE( content_length != NULL );
  CHECK( strcmp(content_length, "10") == 0 );

  httprequest_destroy(req);
  free(req);
}


TEST_CASE("httprequest_read - No Content-Length results in a NULL payload", "[weight=2][part=2]") {
  HTTPRequest *req = _readpipe("GET / HTTP/1.1\r\nHello: World\r\nHost: 127.0.0.1\r\n\r\n");

  CHECK( req->payload == NULL );

  httprequest_destroy(req);
  free(req);
}

TEST_CASE("httprequest_read - ASCII Payload Data", "[weight=2][part=2]") {
  HTTPRequest *req = _readpipe("GET / HTTP/1.1\r\nHello: World\r\nHost: 127.0.0.1\r\nContent-Length: 10\r\n\r\n0123456789");

  REQUIRE( req->payload != NULL );
  CHECK( memcmp(req->payload, "0123456789", 10) == 0 );

  httprequest_destroy(req);
  free(req);
}

TEST_CASE("httprequest_read - Payload Binary Payload", "[weight=2][part=2]") {
  HTTPRequest *req = _readpipe_vptr(
    "GET / HTTP/1.1\r\nHello: World\r\nHost: 127.0.0.1\r\nContent-Length: 23\r\n\r\n0123456789-\x00-0123456789",
    92
  );

  REQUIRE( req->payload != NULL );
  CAPTURE( ((char *)req->payload)[0] );
  CAPTURE( ((char *)req->payload)[9] );
  CAPTURE( ((char *)req->payload)[10] );
  CAPTURE( ((char *)req->payload)[11] );
  CAPTURE( ((char *)req->payload)[12] );
  CAPTURE( ((char *)req->payload)[13] );
  CAPTURE( ((char *)req->payload)[21] );
  CAPTURE( ((char *)req->payload)[22] );
  CHECK( memcmp(req->payload, "0123456789-\x00-0123456789", 23) == 0 );

  httprequest_destroy(req);
  free(req);
}
