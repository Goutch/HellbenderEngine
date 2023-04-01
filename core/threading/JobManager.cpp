#include "JobManager.h"
namespace HBE {
    std::list<IJob *> JobManager::running_jobs;

    void JobManager::updateJobsStatus() {
        auto i = running_jobs.begin();
        while (i != running_jobs.end()) {
            if ((*i)->isFinish()) {
                (*i)->onFinish();
                delete *i;
                i = running_jobs.erase(i);
            } else {
                ++i;
            }
        }
    }
}