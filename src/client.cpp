#include "dynamo/client.hpp"

#include <chrono>
#include <thread>

namespace dynamo {

static bool is_success(long status) {
  return status >= 200 && status < 300;
}

DynamoClient::DynamoClient(ClusterConfig cfg, HttpClient http, RetryPolicy retry)
  : cfg_(std::move(cfg)), http_(std::move(http)), retry_(retry) {
  ring_.build(cfg_.nodes, cfg_.vnodes);
}

HttpResponse DynamoClient::request_with_retry(const std::string& url,
                                             const std::string& method,
                                             const std::string& body) const {
  HttpResponse last;

  const int attempts = 1 + std::max(0, retry_.max_retries);
  for (int i = 0; i < attempts; ++i) {
    if (method == "GET") last = http_.get(url);
    else if (method == "PUT") last = http_.put(url, body);
    else {
      last.error = "unsupported method: " + method;
      return last;
    }

    if (last.error.empty() && is_success(last.status)) return last;

    if (i + 1 < attempts) {
      const uint32_t delay = retry_.base_delay_ms * (1u << i);
      std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    }
  }
  return last;
}

bool DynamoClient::put(const std::string& key, const std::string& value) {
  const auto order = ring_.ordered_node_indices(key);

  for (const auto idx : order) {
    const auto& n = cfg_.nodes[idx];
    const std::string url = n.base_url() + "/kv/" + key;
    const auto resp = request_with_retry(url, "PUT", value);
    if (resp.error.empty() && is_success(resp.status)) return true;
  }
  return false;
}

std::optional<std::string> DynamoClient::get(const std::string& key) {
  const auto order = ring_.ordered_node_indices(key);

  for (const auto idx : order) {
    const auto& n = cfg_.nodes[idx];
    const std::string url = n.base_url() + "/kv/" + key;
    const auto resp = request_with_retry(url, "GET");
    if (resp.error.empty() && resp.status == 200) return resp.body;
  }
  return std::nullopt;
}

std::vector<std::string> DynamoClient::health_all() const {
  std::vector<std::string> out;
  out.reserve(cfg_.nodes.size());

  for (const auto& n : cfg_.nodes) {
    const auto resp = http_.get(n.base_url() + "/health");
    if (!resp.error.empty()) {
      out.push_back(n.id + " ERROR " + resp.error);
    } else {
      out.push_back(n.id + " " + std::to_string(resp.status));
    }
  }
  return out;
}

} // namespace dynamo
