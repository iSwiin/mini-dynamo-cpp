#pragma once
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "dynamo/config.hpp"
#include "dynamo/http.hpp"
#include "dynamo/ring.hpp"

namespace dynamo {

struct RetryPolicy {
  int max_retries{2};         // number of retries (not counting first attempt)
  uint32_t base_delay_ms{80}; // exponential backoff base
};

class DynamoClient {
public:
  DynamoClient(ClusterConfig cfg,
               HttpClient http = HttpClient(),
               RetryPolicy retry = RetryPolicy());

  // PUT /kv/<key> body=<value>
  // Returns true if any node returns 2xx.
  bool put(const std::string& key, const std::string& value);

  // GET /kv/<key>
  // Returns value if any node returns 200.
  std::optional<std::string> get(const std::string& key);

  // GET /health (best-effort)
  // Returns per-node status lines.
  std::vector<std::string> health_all() const;

private:
  HttpResponse request_with_retry(const std::string& url,
                                 const std::string& method,
                                 const std::string& body = "") const;

  ClusterConfig cfg_;
  HttpClient http_;
  RetryPolicy retry_;
  ConsistentHashRing ring_;
};

} // namespace dynamo
