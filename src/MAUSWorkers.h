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

#ifndef MAUSTOOLS_WORKER_H
#define MAUSTOOLS_WORKER_H 1

// MAUS
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/Utils/Squeak.hh"
// #include "src/legacy/Interface/Squeak.hh"

// Mic11
#include "Macros.h"
#include "Worker.h"
#include "MAUSProcessors.h"

typedef UpdateWorker<MAUS::Data>  MAUSWorker;
typedef InputWorker<MAUS::Data>   MAUSInput;
typedef OutputWorker<MAUS::Data>  MAUSOutput;

template<class DaqProcessorType>
class DAQWorker : public MAUSInput {
public:
  DAQWorker(int id) : MAUSInput("DAQWorker", new DaqProcessorType, id), _max_object_number(0xE00000) {}

  void start(int n=0);
  void resetROOTProcesID();
  unsigned int getROOTprocessID();

  STREAM_DOWN(MAUS::Data)

private:
  unsigned int             _max_object_number;
};

typedef DAQWorker<DaqInput>        DAQInputWorker;

#ifdef MONITORING_FOUND
typedef DAQWorker<OnlineDaqInput>  OnlineDAQInputWorker;
#endif

/////////////////////////// DetectorsWorker //////////////////////////////////
IMPLEMENT_UWORKER(DetectorsWorker, MAUS::Data, DetectorsProcessor)   // worker name, data type, processor name

/////////////////////////// EMRWorker ////////////////////////////////////////
IMPLEMENT_UWORKER(EMRWorker, MAUS::Data, EMRProcessor)   // worker name, data type, processor name

/////////////////////////// MAUSReduceWorker /////////////////////////////////
IMPLEMENT_UWORKER(MAUSReduceWorker, MAUS::Data, MAUSReduceProcessor)   // worker name, data type, processor name

///////////////////////// OutputWorker ///////////////////////////////////////
IMPLEMENT_USER_OUTPUT(MAUSRootOutputWorker, MAUS::Data, MAUSDataRootOutput) //  worker name, data type, processor name

//////////////////////// MemFreeWorker ///////////////////////////////////////
class MemFreeWorker : public MAUSOutput{
public:
  MemFreeWorker(int id) : MAUSOutput("MemFreeWorker", nullptr, id) {}
  void start(int n=0);
};

#include "MAUSWorkers-inl.h"
#endif
