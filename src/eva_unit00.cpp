#include <chrono>
#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <thread>
#include <vector>

namespace evangelion
{
    enum class SpearPhase
    {
        Idle,
        Charged,
        Locked,
        Released,
        Impact
    };

    struct Telemetry
    {
        double syncRate{0.0};
        bool atFieldStable{false};
        int targetRangeMeters{0};
    };

    template <typename TLog>
    void logWithTimestamp(const std::string &label, TLog &&message)
    {
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::cout << "[" << label << "] (" << now << ") " << std::forward<TLog>(message) << std::endl;
    }

    class EvaUnit00
    {
    public:
        explicit EvaUnit00(std::string pilot) : pilot_(std::move(pilot)) {}

        void initialize()
        {
            logWithTimestamp("EVA-00", "Pilot recognized: " + pilot_);
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
            telemetry_.syncRate = 62.5;
            telemetry_.atFieldStable = true;
            telemetry_.targetRangeMeters = 1200;
            logWithTimestamp("EVA-00", "Synchronization nominal. A.T. Field stable.");
        }

        void chargeSpear()
        {
            phase_ = SpearPhase::Charged;
            logWithTimestamp("SPEAR", "Charging Lance of Longinus...");
            std::this_thread::sleep_for(std::chrono::milliseconds(400));
            logWithTimestamp("SPEAR", "Energy spiral at 100%, red glow confirmed.");
        }

        void lockOn(const std::string &target)
        {
            phase_ = SpearPhase::Locked;
            currentTarget_ = target;
            logWithTimestamp("TARGET", "Locked on " + target + " at range " + std::to_string(telemetry_.targetRangeMeters) + "m");
        }

        void throwSpear()
        {
            phase_ = SpearPhase::Released;
            logWithTimestamp("SPEAR", "Release vector calculated. Launch!");
            std::this_thread::sleep_for(std::chrono::milliseconds(350));
            // In-universe moment: the spear arcs toward the target with impossible precision.
            logWithTimestamp("SPEAR", "Trajectory bending... piercing A.T. Field.");
            phase_ = SpearPhase::Impact;
            logWithTimestamp("SPEAR", "Impact registered on " + currentTarget_.value_or("unknown target") + ".");
        }

        void recordDiagnostics(const std::function<void(const Telemetry &)> &callback) const
        {
            callback(telemetry_);
        }

    private:
        std::string pilot_;
        Telemetry telemetry_{};
        SpearPhase phase_{SpearPhase::Idle};
        std::optional<std::string> currentTarget_{std::nullopt};
    };
}

int main()
{
    evangelion::EvaUnit00 unit00("Rei Ayanami");

    unit00.initialize();
    unit00.chargeSpear();
    unit00.lockOn("Arael");
    unit00.throwSpear();

    unit00.recordDiagnostics([](const evangelion::Telemetry &telemetry) {
        std::clog << "[DIAG] Sync: " << telemetry.syncRate << "%, "
                  << "A.T. stable: " << std::boolalpha << telemetry.atFieldStable << ", "
                  << "Range: " << telemetry.targetRangeMeters << "m" << std::endl;
    });

    return 0;
}
