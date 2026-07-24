#include <array>
#include <chrono>
#include <iostream>
#include <string_view>

namespace signal_lab {

enum class LinkState { dormant, syncing, nominal, saturated };

struct Channel {
  std::string_view name;
  double coherence;
  LinkState state;
};

constexpr std::array channels{
    Channel{"motor-cortex", 0.982, LinkState::nominal},
    Channel{"visual-array", 0.947, LinkState::syncing},
    Channel{"armor-bus", 0.999, LinkState::nominal},
};

constexpr std::string_view status(LinkState state) {
  switch (state) {
    case LinkState::dormant: return "DORMANT";
    case LinkState::syncing: return "SYNCING";
    case LinkState::nominal: return "NOMINAL";
    case LinkState::saturated: return "SATURATED";
  }
  return "UNKNOWN";
}

}  // namespace signal_lab

int main() {
  for (const auto& channel : signal_lab::channels) {
    std::cout << channel.name << "  "
              << channel.coherence * 100.0 << "%  "
              << signal_lab::status(channel.state) << '\n';
  }
}
