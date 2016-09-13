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

#ifndef MAUS_DATA_STATE_MACHINE
#define MAUS_DATA_STATE_MACHINE 1

// C++
#include <iostream>
#include <string>

// MAUS
#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/Spill.hh"

// Mic11
#include "NRootOutput.h"

typedef NRootOutput<MAUS::Data> MAUSRootOutput;

class MAUSDataStateMachine {
 public:
  MAUSDataStateMachine();
  virtual ~MAUSDataStateMachine();

  int getState()    const {return state_;}
  int getRunNum()   const {return runNum_;}
  int getSpillNum() const {return spillNum_;}

  void setVerbose(bool v) {verbose_ = v;}
  int processState(MAUS::Data *d);
  virtual void newRun() =0;
  virtual void endOfRun() =0;

 protected:
  enum State { IDLE, NEW_RUN, TAKING_DATA, END_RUN };

  int state_;
  int runNum_;
  int spillNum_;
  bool verbose_;
};

#endif
