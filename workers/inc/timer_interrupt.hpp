#pragma once
#include <asio.hpp>
#include <boost/fiber/all.hpp>
#include <chrono>

namespace timer_interrupt {

using namespace std::chrono_literals;
using boost::this_fiber::yield;
using std::chrono::steady_clock;

/** Poll the fiber manager from asio::io_service at a regular interval.
 * Not necessary if we don't call io.run() .
 */
class TimerInterrupt {
   public:
    static constexpr auto interval = 100ms;
    TimerInterrupt(asio::io_service& io_);
    ~TimerInterrupt();

   private:
    asio::basic_waitable_timer<steady_clock> timer;
    void yieldToFiberManager(const asio::error_code&);
};

void
TimerInterrupt::yieldToFiberManager(const asio::error_code& ec) {
    if (ec) {
        return;
    }

    yield();
    timer.expires_after(interval);
    timer.async_wait([&](const asio::error_code& ec) { TimerInterrupt::yieldToFiberManager(ec); });
}

TimerInterrupt::TimerInterrupt(asio::io_service& io_) : timer{io_} {
    timer.expires_after(interval);
    timer.async_wait([&](const asio::error_code& ec) { TimerInterrupt::yieldToFiberManager(ec); });
}

TimerInterrupt::~TimerInterrupt() { timer.cancel(); }

}  // namespace timer_interrupt