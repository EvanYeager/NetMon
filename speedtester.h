
#include <atomic>
class SpeedTester {
public:
  static void downloadTest(std::atomic<bool>& quitFlag);
  static void uploadTest(std::atomic<bool>& quitFlag);
};
