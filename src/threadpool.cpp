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
