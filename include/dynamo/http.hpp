#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace dynamo {

struct HttpResponse {
  long status{0};          // HTTP status code (0 if request failed before HTTP)
  std::string body;        // response body
  std::string error;       // libcurl error (empty if success)
};

class HttpClient {
public:
  explicit HttpClient(uint32_t timeout_ms = 2000);
  HttpResponse get(const std::string& url) const;
  HttpResponse put(const std::string& url, const std::string& body) const;

private:
  uint32_t timeout_ms_{2000};
};

} // namespace dynamo
