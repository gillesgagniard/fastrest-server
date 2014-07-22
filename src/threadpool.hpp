/*
 * Copyright (c) 2014 Gilles Gagniard <gilles@gagniard.org>
 *
 * This file is part of fastrest-server.
 *
 * fastrest-server is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * fastrest-server is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with fastrest-server.  If not, see <http://www.gnu.org/licenses/>.
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
