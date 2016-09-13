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

#ifndef BASETOOLS_OUTPUT_H
#define BASETOOLS_OUTPUT_H 1

// C++
#include<iostream>
#include <string>
#include <sstream>
#include <vector>

// ROOT
#include <TROOT.h>
#include <TProcessID.h>
#include <TTree.h>
#include <TFile.h>
#include <TServerSocket.h>

// Mic11
#include "Config.h"
#include "Processor.h"

template <class dataType>
class NRootOutput : public OutProcessor<dataType> {
 public:
  NRootOutput();
  NRootOutput(std::string n);
  NRootOutput(std::string n, dataType **r);

  virtual ~NRootOutput() {}

  virtual void init(std::string) {}
  bool process();
  void close() final;
  void setInputObj(dataType **r);

  TTree* getOutDataTreePtr() const {return outTree_;}
  TFile* getOutDataFilePtr() const {return outFile_;}

  void resetOutBranche();
  void newRun(int runNum);

 protected:
  void makeOutTree();
  void makeOutBranch();
  void makeOutFile();
  void makeOutFile(int runNum);
  void setOutTreeBasketSize(int basketSize);

 private:
  TFile      *outFile_;
  TTree      *outTree_;
  int         outTreeBasketSize_;
};

#include "NRootOutput-inl.h"

#endif


