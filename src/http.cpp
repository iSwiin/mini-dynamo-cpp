#include "dynamo/http.hpp"

#include <curl/curl.h>

namespace dynamo {

static size_t write_cb(char* ptr, size_t size, size_t nmemb, void* userdata) {
  auto* s = static_cast<std::string*>(userdata);
  s->append(ptr, size * nmemb);
  return size * nmemb;
}

class CurlGlobal {
public:
  CurlGlobal() { curl_global_init(CURL_GLOBAL_DEFAULT); }
  ~CurlGlobal() { curl_global_cleanup(); }
};

// One global initializer per process
static CurlGlobal g_curl;

HttpClient::HttpClient(uint32_t timeout_ms) : timeout_ms_(timeout_ms) {}

static HttpResponse perform_request(const std::string& url,
                                    const char* method,
                                    const std::string& body,
                                    uint32_t timeout_ms) {
  HttpResponse resp;

  CURL* curl = curl_easy_init();
  if (!curl) {
    resp.error = "curl_easy_init failed";
    return resp;
  }

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp.body);
  curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout_ms);
  curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, timeout_ms);
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

  struct curl_slist* headers = nullptr;
  if (std::string(method) == "PUT") {
    headers = curl_slist_append(headers, "Content-Type: text/plain");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(body.size()));
  }

  const CURLcode code = curl_easy_perform(curl);
  if (code != CURLE_OK) {
    resp.error = curl_easy_strerror(code);
  }

  curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &resp.status);

  if (headers) curl_slist_free_all(headers);
  curl_easy_cleanup(curl);
  return resp;
}

HttpResponse HttpClient::get(const std::string& url) const {
  return perform_request(url, "GET", "", timeout_ms_);
}

HttpResponse HttpClient::put(const std::string& url, const std::string& body) const {
  return perform_request(url, "PUT", body, timeout_ms_);
}

} // namespace dynamo
