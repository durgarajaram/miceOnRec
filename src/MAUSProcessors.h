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

#ifndef MAUSTOOLS_PROCESSOR_H
#define MAUSTOOLS_PROCESSOR_H 1

// Root
#include <TSystem.h>
#include <TImage.h>

// Mic11
#include "Processor.h"
#include "NRootOutput.h"
#include "MAUSDataStateMachine.h"

// maus
#include "src/input/InputCppDAQOfflineData/InputCppDAQOfflineData.hh"
#include "src/map/MapCppEMRRecon/MapCppEMRRecon.hh"
#include "src/map/MapCppEMRSpacePoints/MapCppEMRSpacePoints.hh"
#include "src/map/MapCppEMRPlaneHits/MapCppEMRPlaneHits.hh"
#include "src/map/MapCppTOFDigits/MapCppTOFDigits.hh"
#include "src/map/MapCppTOFSlabHits/MapCppTOFSlabHits.hh"
#include "src/map/MapCppTOFSpacePoints/MapCppTOFSpacePoints.hh"
#include "src/map/MapCppKLDigits/MapCppKLDigits.hh"
#include "src/map/MapCppKLCellHits/MapCppKLCellHits.hh"
#include "src/map/MapCppCkovDigits/MapCppCkovDigits.hh"
#include "src/map/MapCppTrackerDigits/MapCppTrackerDigits.hh"
#include "src/map/MapCppTrackerRecon/MapCppTrackerRecon.hh"
#include "src/reduce/ReduceCppTOFPlot/ReduceCppTOFPlot.hh"
#include "src/reduce/ReduceCppEMRPlot/ReduceCppEMRPlot.hh"
#include "src/reduce/ReduceCppKLPlot/ReduceCppKLPlot.hh"
#include "src/reduce/ReduceCppCkovPlot/ReduceCppCkovPlot.hh"
#include "src/reduce/ReduceCppSciFiPlot/ReduceCppSciFiPlot.hh"
#include "src/reduce/ReduceCppTofCalib/ReduceCppTofCalib.hh"
#include "ReduceCppOnrecMonitor.hh"
#include "ReduceCppScalerPlot.hh"
#include "src/common_cpp/DataStructure/ImageData/ImageData.hh"

typedef InProcessor<MAUS::Data>   MAUSDataProcessor;
typedef OutProcessor<MAUS::Data>  MAUSDataOutput;

class BaseMAUSProcessor : public MAUSDataProcessor {
 public:
  BaseMAUSProcessor(std::string n)
  : MAUSDataProcessor(n) {}

  void init(std::string s) =0;
  bool process() =0;
  void close() =0;
};

class DaqInput : public MAUSDataProcessor {
 public:
  DaqInput();
  virtual ~DaqInput() {}

  void init(std::string s);
  bool process();
  void close();

 private:
  MAUS::InputCppDAQOfflineData daqInput_;
};

#ifdef MONITORING_FOUND
#include "src/input/InputCppDAQOnlineData/InputCppDAQOnlineData.hh"

class OnlineDaqInput : public MAUSDataProcessor {
 public:
  OnlineDaqInput();
  virtual ~OnlineDaqInput();

  void init(std::string s);
  bool process();
  void close();

 private:
  MAUS::InputCppDAQOnlineData daqInput_;
};

#endif

class DetectorsProcessor : public MAUSDataProcessor {
 public:
  DetectorsProcessor();
  virtual ~DetectorsProcessor();

  void init(std::string s);
  bool process();
  void close();

 private:
  MAUS::MapCppTOFDigits      tofDigits_;
  MAUS::MapCppTOFSlabHits    tofSlHits_;
  MAUS::MapCppKLDigits       klDigits_;
  MAUS::MapCppKLCellHits     klCellHits_;
  MAUS::MapCppCkovDigits     ckovDigits_;
  MAUS::MapCppTOFSpacePoints tofSpPoints_;
  MAUS::MapCppTrackerDigits  tracker_digits_;
  MAUS::MapCppTrackerRecon   tracker_rec_;
};

class EMRProcessor : public MAUSDataProcessor {
 public:
  EMRProcessor();
  virtual ~EMRProcessor() {}

  void init(std::string s);
  bool process();
  void close();

 private:
  MAUS::MapCppEMRPlaneHits   emrPHits_;
  MAUS::MapCppEMRSpacePoints emrSPnts_;
  MAUS::MapCppEMRRecon       emrRecon_;
};

class MAUSDataRootOutput : public NRootOutput<MAUS::Data>, public MAUSDataStateMachine {
 public:
  MAUSDataRootOutput();
  MAUSDataRootOutput(MAUS::Data **r);
  virtual ~MAUSDataRootOutput();

  void init(std::string s);
  bool process();

  void newRun();
  void endOfRun() final {}

 private:
//   MAUS::ReduceCppTofCalib tofCalib_;
};

class MAUSReduceProcessor : public MAUSDataProcessor, public MAUSDataStateMachine {
 public:
  MAUSReduceProcessor();
  virtual ~MAUSReduceProcessor() {}

  void init(std::string s);
  bool process();
  void close();

  void newRun();
  void endOfRun() final;

 private:
  void writeInages();
  void printCanvas(int id);
  void printCanvas();

  unsigned int canvasId_;
  std::string plotPath_;
  TImage *img_;

  MAUS::ReduceCppTOFPlot      tofReduce_;
  MAUS::ReduceCppEMRPlot      emrReduce_;
  MAUS::ReduceCppKLPlot       klReduce_;
  MAUS::ReduceCppCkovPlot     ckovReduce_;
  MAUS::ReduceCppSciFiPlot    scifiReduce_;
  MAUS::ReduceCppOnrecMonitor memReduce_;
  MAUS::ReduceCppScalerPlot   scalerReduce_;

  MAUS::ImageData         outImages_;
  TFile                  *out_file_;
};

class MemTest : public MAUSDataProcessor {
 public:
  MemTest();
  virtual ~MemTest() {}

  void init(std::string) {};
  bool process();
  void close() {}

 public:
  ProcInfo_t info_;
};

class MemFreeOutput : public MAUSDataOutput {
 public:
  MemFreeOutput();
  virtual ~MemFreeOutput();

  void init(std::string) {}
  bool process();
  void close() {}
};

#endif

