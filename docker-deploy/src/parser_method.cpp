#include "parser_method.hpp"

void parser_method::get_host_and_port(const httpparser::Request & req,
                                      std::string & hostname,
                                      std::string & port) {
  size_t i = 0;
  for (; i < req.headers.size() && req.headers[i].name != "Host"; i++)
    ;
  if (i == req.headers.size()) {
    throw my_exception("bad request");
  }
  std::string hostname_port = req.headers[i].value;
  std::size_t pos = hostname_port.find(":");
  if (pos != std::string::npos) {
    hostname = hostname_port.substr(0, pos);
    port = hostname_port.substr(pos + 1);
  }
  else {
    hostname = hostname_port;
    port = "80";
  }
}

/** 
    return "" if not find such header, the header value if it exists
*/
std::string parser_method::request_get_header_value(const httpparser::Request & req,
                                                    const std::string header_name) {
  size_t i = 0;
  for (; i < req.headers.size() && req.headers[i].name != header_name; i++)
    ;
  if (i == req.headers.size()) {
    return "";
  }
  return req.headers[i].value;
}

std::string parser_method::response_get_header_value(const httpparser::Response & resp,
                                                     const std::string header_name) {
  size_t i = 0;
  for (; i < resp.headers.size() && resp.headers[i].name != header_name; i++)
    ;
  if (i == resp.headers.size()) {
    return "";
  }
  return resp.headers[i].value;
}

std::string parser_method::get_request_line(const httpparser::Request & req) {
  std::stringstream stream;
  stream << req.method << " " << req.uri << " HTTP/" << req.versionMajor << "."
         << req.versionMinor;
  return stream.str();
}

std::string parser_method::get_response_line(const httpparser::Response & resp) {
  std::stringstream stream;
  stream << "HTTP/" << resp.versionMajor << "." << resp.versionMinor << " "
         << resp.statusCode << " " << resp.status;
  return stream.str();
}

std::unique_ptr<httpparser::Request> parser_method::http_request_parse(
    std::vector<char> & buffer) {
  httpparser::HttpRequestParser parser;
  std::unique_ptr<httpparser::Request> request_res_ptr(new httpparser::Request());
  httpparser::HttpRequestParser::ParseResult res = parser.parse(
      *request_res_ptr, &buffer.data()[0], &buffer.data()[0] + buffer.size());
  if (res == httpparser::HttpRequestParser::ParsingCompleted) {  //SUCCESS
    return request_res_ptr;  //return local value, no need to move()
  }
  else {
    //return a 400 error code
    throw my_exception("cannot parse request");
  }
}

std::unique_ptr<httpparser::Response> parser_method::http_response_parse(
    std::vector<char> & buffer) {
  httpparser::HttpResponseParser parser;
  std::unique_ptr<httpparser::Response> response_res_ptr(new httpparser::Response());
  httpparser::HttpResponseParser::ParseResult res = parser.parse(
      *response_res_ptr, &buffer.data()[0], &buffer.data()[0] + buffer.size());
  if (res == httpparser::HttpResponseParser::ParsingCompleted) {  //SUCCESS
    return response_res_ptr;  //return local value, no need to move()
  }
  else {
    //return a 400 error code.
    throw my_exception("cannot parse response");
  }
}

void parser_method::update_response(httpparser::Response & resp,
                                    std::string name,
                                    std::string value) {
  size_t i = 0;
  for (; i < resp.headers.size() && resp.headers[i].name != name; i++)
    ;
  if (i == resp.headers.size()) {
    struct httpparser::Response::HeaderItem item = {name, value};
    resp.headers.push_back(item);
  }
  resp.headers[i].value = value;
}
