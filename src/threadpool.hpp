/* 
 * File:   threadpool.hpp
 * Author: gilles
 *
 * Created on 16 mars 2014, 21:58
 */

#pragma once

#include <vector>
#include <boost/thread/thread.hpp>
#include <boost/asio/io_service.hpp>

namespace fastrest
{
  class threadpool
  {
  public:
    threadpool(size_t nb_threads);
    ~threadpool();
    
    template <typename FUN>
    void run_task(FUN task)
    {
      _task_runner.post(task);
    }
    
  private:
    boost::thread_group _threads;
    boost::asio::io_service _task_runner;
    boost::asio::io_service::work _task_runner_work;
  };
}
