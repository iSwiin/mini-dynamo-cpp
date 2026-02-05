#include "dynamo/config.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <regex>

namespace dynamo {

std::string Node::base_url() const {
  return "http://" + host + ":" + std::to_string(port);
}

static std::string slurp(const std::string& path) {
  std::ifstream in(path, std::ios::in | std::ios::binary);
  if (!in) return {};
  std::ostringstream ss;
  ss << in.rdbuf();
  return ss.str();
}

ClusterConfig ClusterConfig::local_default() {
  ClusterConfig cfg;
  cfg.vnodes = 128;
  cfg.nodes = {
    {"n1", "localhost", 9001},
    {"n2", "localhost", 9002},
    {"n3", "localhost", 9003},
  };
  return cfg;
}

static ClusterConfig parse_text_config(const std::string& text) {
  ClusterConfig cfg;
  cfg.vnodes = 128;

  std::istringstream ss(text);
  std::string line;
  while (std::getline(ss, line)) {
    if (line.empty()) continue;
    if (line[0] == '#') continue;

    // vnodes=128
    {
      std::smatch m;
      std::regex rv(R"(vnodes\s*=\s*(\d+))");
      if (std::regex_search(line, m, rv)) {
        cfg.vnodes = std::stoi(m[1].str());
        continue;
      }
    }

    // n1 localhost 9001
    std::istringstream ls(line);
    Node n;
    ls >> n.id >> n.host >> n.port;
    if (!n.id.empty() && !n.host.empty() && n.port != 0) {
      cfg.nodes.push_back(n);
    }
  }

  if (cfg.nodes.empty()) {
    throw std::runtime_error("config has no nodes");
  }
  return cfg;
}

static ClusterConfig parse_min_json_config(const std::string& text) {
  ClusterConfig cfg;
  cfg.vnodes = 128;

  // vnodes
  {
    std::smatch m;
    std::regex rv(R"("vnodes"\s*:\s*(\d+))");
    if (std::regex_search(text, m, rv)) {
      cfg.vnodes = std::stoi(m[1].str());
    }
  }

  // nodes with host/port
  // {"id":"n1","host":"localhost","port":9001}
  std::regex rnode_hp(R"REGEX(\{\s*"id"\s*:\s*"([^"]+)"\s*,\s*"host"\s*:\s*"([^"]+)"\s*,\s*"port"\s*:\s*(\d+)\s*\})REGEX");
  for (auto it = std::sregex_iterator(text.begin(), text.end(), rnode_hp);
       it != std::sregex_iterator(); ++it) {
    Node n;
    n.id = (*it)[1].str();
    n.host = (*it)[2].str();
    n.port = static_cast<uint16_t>(std::stoi((*it)[3].str()));
    cfg.nodes.push_back(n);
  }

  // nodes with address field: {"id":"n1","address":"localhost:9001"}
  std::regex rnode_addr(R"REGEX(\{\s*"id"\s*:\s*"([^"]+)"\s*,\s*"address"\s*:\s*"([^"]+)"\s*\})REGEX");
  for (auto it = std::sregex_iterator(text.begin(), text.end(), rnode_addr);
       it != std::sregex_iterator(); ++it) {
    Node n;
    n.id = (*it)[1].str();
    std::string addr = (*it)[2].str();
    auto pos = addr.rfind(':');
    if (pos == std::string::npos) continue;
    n.host = addr.substr(0, pos);
    n.port = static_cast<uint16_t>(std::stoi(addr.substr(pos + 1)));
    cfg.nodes.push_back(n);
  }

  if (cfg.nodes.empty()) {
    throw std::runtime_error("JSON config parse failed (no nodes found)");
  }
  return cfg;
}

ClusterConfig ClusterConfig::load_from_file(const std::string& path) {
  const std::string text = slurp(path);
  if (text.empty()) {
    // If file missing/empty, fallback to local default to keep UX simple.
    return local_default();
  }

  // Heuristic: if looks like JSON, parse JSON; else parse text.
  const bool looks_json = text.find('{') != std::string::npos && text.find("nodes") != std::string::npos;
  try {
    return looks_json ? parse_min_json_config(text) : parse_text_config(text);
  } catch (const std::exception& e) {
    throw std::runtime_error(std::string("failed to parse config '") + path + "': " + e.what());
  }
}

} // namespace dynamo
