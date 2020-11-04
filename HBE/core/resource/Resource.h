#pragma once
#include "core/utility/Job.h"
#include "list"

class Resource {
protected:
    static std::list<Job<std::string>*> running_jobs;
public:
    static void updateJobsStatus()
    {
        for (auto i = running_jobs.begin(); i != running_jobs.end();) {
            if ((*i)->isFinish())
            {
                (*i)->onFinish();
                delete (*i);
                running_jobs.erase(i);
            }
            else
            {
                ++i;
            }
        }
    }
};
