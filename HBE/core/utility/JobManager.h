class IJob;

#include "Job.h"
#include "list"
class JobManager {
private:
    static std::list<IJob*> running_jobs;
public:
    template<typename Return, typename... Args>
    static Job<Return, Args...> *create() {
        Job<Return, Args...>* job=new Job<Return,Args...>();
        running_jobs.push_back(job);
        return job;
    }
    static void updateJobsStatus();
};
