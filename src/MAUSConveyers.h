/* This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
 *
 * MAUS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MAUS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef MAUSTOOLS_CONVEYER_H
#define MAUSTOOLS_CONVEYER_H 1

// C++
#include <vector>


#include "MAUSWorkers.h"
#include "MAUSProcessors.h"

class MausSTConveyer {
public:
  MausSTConveyer();
  virtual ~MausSTConveyer();

  void init(std::string s);
  void start(int n=0);

  void setInput(MAUSDataProcessor* p);
  int  addWorker(MAUSDataProcessor* p);
  void setOutput(MAUSDataOutput* p);

  template<typename inputType>
  void setInput() {
    input_ = new inputType(0);
    dynamic_input_ = true;
  }

  template<typename processorType>
  int addWorker() {
    MAUSDataProcessor *newWorker = new processorType();
    workers_.push_back(newWorker);
    dynamic_workers_.push_back(true);

    return workers_.size();
  }

  template<typename outputType>
  void setOutput() {
    output_ = new outputType(0);
    dynamic_output_ = true;
  }

  void dismissWorkers();
  void dumpWorkers();
  void autoinit();

private:
  void close();
  void connectProcessors();

  MAUSDataProcessor*               input_;
  bool dynamic_input_;

  std::vector<MAUSDataProcessor*>  workers_;
  vector<bool> dynamic_workers_;

  MAUSDataOutput* output_;
  bool dynamic_output_;

  MAUS::Data **data_;
};

class MausMTConveyer {
public:
  MausMTConveyer();
  virtual ~MausMTConveyer();

  void init(std::string s);
  void start(int n=0);

  void setInput(MAUSInput *i);
  int  addWorker(MAUSWorker *w);
  void setOutput(MAUSOutput *o);

  template<typename inputType>
  void setInput() {
    input_ = new inputType(0);
    dynamic_input_ = true;
  }

  template<typename workerType>
  int addWorker() {
    unsigned int nWorkers = workers_.size();
    MAUSWorker *newWorker = new workerType(nWorkers+1);
    workers_.push_back(newWorker);
    dynamic_workers_.push_back(true);

    if(nWorkers > 0) {
      auto fifo = workers_[nWorkers-1]->getOutFifoPtr();
      workers_[nWorkers]->setInFifoPtr( fifo );
    }

    return nWorkers;
  }

  template<typename outputType>
  void setOutput() {
    unsigned int nWorkers = workers_.size();
    output_ = new outputType(nWorkers+1);
    dynamic_output_ = true;
  }

  void dismissWorkers();
  void dumpWorkers();
  void autoinit();

void enableTimeStats() {time_stats_enable_ = true;}

private:
  MAUSInput *input_;
  bool dynamic_input_;

  std::vector<MAUSWorker*>  workers_;
  vector<bool>       dynamic_workers_;

  MAUSOutput *output_;
  bool dynamic_output_;

  bool time_stats_enable_;
};

#endif

