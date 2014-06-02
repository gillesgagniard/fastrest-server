/* 
 * File:   threadpool.cpp
 * Author: gilles
 * 
 * Created on 16 mars 2014, 21:59
 */

#include "threadpool.hpp"
#include "logutils.hpp"

namespace fastrest
{
  threadpool::threadpool(size_t nb_threads) :
        _task_runner_work(_task_runner)
  {
    for (size_t i = 0; i < nb_threads; ++i)
    {
      boost::thread* t = _threads.create_thread([this](){_task_runner.run();});
      LOG_DEBUG << "created thread " << i << " id=" << t->get_id();
    }
  }
  
  threadpool::~threadpool()
  {
    LOG_INFO << "finalize threadpool";
    _task_runner.stop();
    _threads.join_all();
  }
}
