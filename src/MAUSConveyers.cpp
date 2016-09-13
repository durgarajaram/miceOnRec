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

// Root
#include "TThread.h"

// Maus
#include "src/common_cpp/Utils/Squeak.hh"

// Mic11
#include "MAUSConveyers.h"

using namespace MAUS::Exceptions;

MausSTConveyer::MausSTConveyer()
: input_(nullptr), dynamic_input_(false), output_(nullptr), dynamic_output_(false), data_(0) {
  MAUS::Squeak::setStandardOutputs(0);
}

MausSTConveyer::~MausSTConveyer() {
  this->dismissWorkers();
  if (data_)
    delete data_;
}

void MausSTConveyer::init(std::string s) {
  // Check if the MAUS environment is loaded.
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  if (!pMAUS_ROOT_DIR) {
    throw Exception(exceptionLevel::nonRecoverable,
                          "Could not find the $MAUS_ROOT_DIR environmental variable. \nDid you try running: source maus_env.sh?",
                          "MausSTConveyer::init");
  }

  if (!input_) {
    throw Exception(exceptionLevel::nonRecoverable,
                          "Unable to initialize the conveyer. The input is undefined.",
                          "MausSTConveyer::init");
  }

  if (!output_) {
    throw Exception(exceptionLevel::nonRecoverable,
                          "Unable to initialize the conveyer. The output is undefined.",
                          "MausSTConveyer::init");
  }

  input_->init(s);

  for (auto &w:workers_)
    w->init(s);

  output_->init(s);

  this->connectProcessors();

  MAUS::Squeak::setStandardOutputs(0);
  this->dumpWorkers();
}

void MausSTConveyer::start(int n) {

  MAUS::Squeak::setStandardOutputs(0);
  std::cout << "\n Start of the work ...\n" << std::endl;

  int xEvt=0;
  MAUS::Squeak::setStandardOutputs(0);
  while (xEvt<n || n==0) {
    if (!input_->process_with_stats()) break;

    for (auto &w:workers_)
    w->process_with_stats();

    output_->process_with_stats();

    TProcessID::GetPID()->Clear();
    TProcessID::SetObjectCount(0);
    ++xEvt;
  }

  this->close();
}

void MausSTConveyer::setInput(MAUSDataProcessor* i) {
  if (input_) {
    if (input_ != i) {
      delete input_;
    }
  }

  input_ = i;
  dynamic_input_ = false;
}

int MausSTConveyer::addWorker(MAUSDataProcessor* p) {
  workers_.push_back(p);
  dynamic_workers_.push_back(false);

  return workers_.size();
}

void MausSTConveyer::setOutput(MAUSDataOutput* o) {
  if (output_) {
    if (output_ != o) {
      delete output_;
    }
  }

  output_ = o;
  dynamic_output_ = false;
}

void MausSTConveyer::dismissWorkers() {
  if (dynamic_input_)
    delete input_;

  int i(0);
  for (auto &w:workers_)
    if (dynamic_workers_[i++]) delete w;

  workers_.resize(0);

  if (dynamic_output_)
    delete output_;
}

void MausSTConveyer::dumpWorkers() {
  cout << "*i* " << input_->getName() << " ***  < " << input_->getOutputObj() << " >\n";

  for (auto &w:workers_)
    cout << "*p* " << w->getName() << " ***  < " << w->getOutputObj() << " >\n";

  cout << "*0* " << output_->getName() << " ***  < " << output_->getInputObj() << " >\n";
}

void MausSTConveyer::autoinit() {
  stringstream buffer;
  ifstream config_doc("config.json");
  buffer << config_doc.rdbuf();
  std::string json_conf = buffer.str();
  this->init(json_conf);
}

void MausSTConveyer::close() {
  input_->close();
  input_->printTimeStats();

  for (auto &w:workers_) {
    w->close();
    w->printTimeStats();
  }

  output_->close();
  output_->printTimeStats();
}

void MausSTConveyer::connectProcessors() {
  data_ = new MAUS::Data*;
  *data_ = new MAUS::Data();

  input_->setOutputObj(data_);

  for (auto &w:workers_)
    w->setOutputObj(data_);

  output_->setInputObj(data_);
}


///////////////////////////////////////////////////////////////////////////////////
//   MausMTConveyer
//////////////////////////////////////////////////////////////////////////////////

MausMTConveyer::MausMTConveyer()
: input_(nullptr), dynamic_input_(false), output_(nullptr),
  dynamic_output_(false), time_stats_enable_(false) {}

MausMTConveyer::~MausMTConveyer() {
  this->dismissWorkers();
}

void MausMTConveyer::init(std::string s) {
  // Check if the MAUS environment is loaded.
  char* pMAUS_ROOT_DIR = getenv("MAUS_ROOT_DIR");
  if (!pMAUS_ROOT_DIR) {
    throw Exception(exceptionLevel::nonRecoverable,
                          "Could not find the $MAUS_ROOT_DIR environmental variable. \nDid you try running: source maus_env.sh?",
                          "MausSTConveyer::init");
  }

  if (!input_) {
    throw Exception(exceptionLevel::nonRecoverable,
                          "Unable to initialize the conveyer. The input is undefined.",
                          "MausMTConveyer::init");
  }

  if (!output_) {
    throw Exception(exceptionLevel::nonRecoverable,
                          "Unable to initialize the conveyer. The output is undefined.",
                          "MausMTConveyer::init");;
  }

  input_->init(s);

  for (auto &w:workers_)
    w->init(s);

  output_->init(s);

  if (time_stats_enable_) {
    input_->enableTimeStats();
    for (auto &w:workers_)
      w->enableTimeStats();

    output_->enableTimeStats();
  }

  MAUS::Squeak::setStandardOutputs(0);
  this->dumpWorkers();
}

void MausMTConveyer::start(int n) {

  MAUS::Squeak::setStandardOutputs(0);
  std::cout << "Start of the work ..." << std::endl;

  // Initialize ROOT threading support
  TThread::Initialize();

  vector<thread> threads;
  auto ijob = [&]{input_->start(n);};
  threads.push_back(thread(ijob));

  for (auto &w:workers_) {
    auto job = [&]{w->start();};
    threads.push_back( thread(job) );
  }

  auto ojob = [&]{output_->start();};
  threads.push_back(thread(ojob));

  for (auto &t:threads) {
    t.join();
  }
  std::cout << "End of the work." << std::endl;

  if (time_stats_enable_) {
    input_->printStats();

    for (auto &w:workers_)
      w->printStats();

    output_->printStats();
  }
}

void MausMTConveyer::setInput(MAUSInput* i) {
  if (input_) {
    if (input_ != i) {
      delete input_;
    }
  }

  input_ = i;
  dynamic_input_ = false;
}

int MausMTConveyer::addWorker(MAUSWorker* w) {
  unsigned int nWorkers = workers_.size();
  if(nWorkers) {
    auto fifo = workers_.back()->getOutFifoPtr();
    w->setInFifoPtr( fifo );
  } else if (input_) {
    auto fifo = input_->getOutFifoPtr();
    w->setInFifoPtr( fifo );
  }

  workers_.push_back(w);
  dynamic_workers_.push_back(false);

  return nWorkers;
}

void MausMTConveyer::setOutput(MAUSOutput* o) {
  if (output_) {
    if (output_ != o) {
      delete output_;
    }
  }

  output_ = o;
  dynamic_output_ = false;

  unsigned int nWorkers = workers_.size();
  if(nWorkers) {
    auto fifo = workers_.back()->getOutFifoPtr();
    output_->setInFifoPtr( fifo );
  } else if (input_) {
    auto fifo = input_->getOutFifoPtr();
    output_->setInFifoPtr( fifo );
  }
}

void MausMTConveyer::dismissWorkers() {
  if (dynamic_input_)
    delete input_;

  if (dynamic_output_)
    delete output_;

  for (unsigned int i=0; i< workers_.size(); ++i)
    if(dynamic_workers_[i])
      delete workers_[i];

  workers_.resize(0);
}

void MausMTConveyer::dumpWorkers() {
  cout << "*** " << input_->getName() << " ***  < " << input_->getOutFifoPtr().get() << " >\n";

  for (auto &w:workers_)
    cout << "*** " << w->getName() << " ***  < " << w->getInFifoPtr().get()
         << ", " << w->getOutFifoPtr().get() << " >\n";

  cout << "***" << output_->getName() << " ***  < " << output_->getInFifoPtr().get() << " >\n";
}

void MausMTConveyer::autoinit() {
  stringstream buffer;
  ifstream config_doc("config.json");
  buffer << config_doc.rdbuf();
  std::string json_conf = buffer.str();
  this->init(json_conf);
}


