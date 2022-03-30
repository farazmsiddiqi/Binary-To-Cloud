#include "lib/catch.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "../http.h"

HTTPRequest *_parse_vptr(const void *s, size_t len) {
  char *buf = malloc(len + 1);
  memcpy(buf, s, len);
  buf[len] = 0;

  HTTPRequest *req = malloc(sizeof(HTTPRequest));
  httprequest_parse_headers(req, buf, len);
  free(buf);
  return req;
}

HTTPRequest *_parse(const char *s) {
  return _parse_vptr(s, strlen(s));
}


TEST_CASE("httprequest_parse_headers - Request Line", "[weight=2][part=1][suite=start]") {
  HTTPRequest *req = _parse("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");

  REQUIRE( req->action != NULL );
  CHECK( strcmp(req->action, "GET") == 0 );

  REQUIRE( req->path != NULL );
  CHECK( strcmp(req->path, "/") == 0 );

  REQUIRE( req->version != NULL );
  CHECK( strcmp(req->version, "HTTP/1.1") == 0 );

  httprequest_destroy(req);
  free(req);
}

TEST_CASE("httprequest_parse_headers - First Header", "[weight=2][part=1]") {
  HTTPRequest *req = _parse("GET / HTTP/1.1\r\nHost: localhost\r\n\r\n");

  const char *host = httprequest_get_header(req, "Host");
  CAPTURE( host );
  REQUIRE( host != NULL );
  CHECK( strcmp(host, "localhost") == 0 );

  httprequest_destroy(req);
  free(req);
}

TEST_CASE("httprequest_parse_headers - Two Headers", "[weight=3][part=1]") {
  HTTPRequest *req = _parse("GET / HTTP/1.1\r\nHello: World\r\nHost: 127.0.0.1\r\n\r\n");

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

TEST_CASE("httprequest_parse_headers - Content-Length Header", "[weight=3][part=1]") {
  HTTPRequest *req = _parse("GET / HTTP/1.1\r\nHello: World\r\nHost: 127.0.0.1\r\nContent-Length: 10\r\n\r\n0123456789");

  const char *content_length = httprequest_get_header(req, "Content-Length");
  CAPTURE( content_length );
  REQUIRE( content_length != NULL );
  CHECK( strcmp(content_length, "10") == 0 );

  httprequest_destroy(req);
  free(req);
}
