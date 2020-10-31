
#include "Job.h"

bool Job::isFinish() {
    return result.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready;
}