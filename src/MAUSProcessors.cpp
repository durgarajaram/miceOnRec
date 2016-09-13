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

// MAUS
#include "src/common_cpp/Utils/Exception.hh"

// Mic11
#include "MAUSProcessors.h"

// C++11
#include <mutex>
extern std::mutex mtx;

///////////////////// DaqInput ///////////////////////////////////////////

inline bool stopSignal() {
  struct stat buffer;
  return (stat (STOP_SIGNAL, &buffer) == 0);
}

DaqInput::DaqInput()
: MAUSDataProcessor("DaqInput") {}

void DaqInput::init(std::string s) {
//   cout << s << endl;
  daqInput_.birth(s);
}

bool DaqInput::process() {

  if (stopSignal())
    return false;

  MAUS::Spill *spill(nullptr);
  std::string event_type("");
  int nPartEvts(0);
  while ( event_type != "physics_event" &&
          event_type != "start_of_run"  &&
          event_type != "end_of_run" ) {

    if( !daqInput_.readNextEvent() )
      return false;

    spill = new MAUS::Spill();
    (**output_).SetSpill(spill);                              // This deletes the old spill.
    nPartEvts = daqInput_.getCurEvent(*output_);
    event_type = spill->GetDaqEventType();
  }

  if(event_type != "physics_event") {
    --processCount_;
    return true;
  }

  cout << "Input ("<< event_type << ") nPartEvts: " << nPartEvts << endl;

  MAUS::ReconEventPArray *recEvts = new MAUS::ReconEventPArray();
  for (int iPe = 0; iPe < nPartEvts; ++iPe)
     recEvts->push_back(new MAUS::ReconEvent);

  spill->SetReconEvents(recEvts);

//   int evSize = spill->GetDAQData()->GetEventSize();
//   int evId = spill->GetEventId();
//   cout << "Online Input ("<< event_type << ") size: " << evSize/(1024.*1024.) << " MB  evId: " << evId
//        << "  nPartEvts: " << nPartEvts <<"  deltaT: " << GET_DURATION(t0)*1e3 << " ms" << endl;

  return true;
}

void DaqInput::close() {
  daqInput_.death();
}

#ifdef MONITORING_FOUND
///////////////////// OnlineDaqInput ///////////////////////////////////////////

OnlineDaqInput::OnlineDaqInput()
: MAUSDataProcessor("OnlineDaqInput") {}

OnlineDaqInput::~OnlineDaqInput() {}

void OnlineDaqInput::init(std::string s) {
  daqInput_.birth(s);
}

bool OnlineDaqInput::process() {

  if (stopSignal())
    return false;

  MAUS::Spill *spill(nullptr);
  std::string event_type("");
  int nPartEvts(0);
  while ( event_type != "physics_event" &&
          event_type != "start_of_run"  &&
          event_type != "end_of_run" ) {

    if( !daqInput_.readNextEvent() )
      return false;

    spill = new MAUS::Spill();
    (**output_).SetSpill(spill);                              // This deletes the old spill.
    nPartEvts = daqInput_.getCurEvent(*output_);
    event_type = spill->GetDaqEventType();
  }

//  cout << "Online Input ("<< event_type << ") nPartEvts: " << nPartEvts << endl;

  if(event_type != "physics_event")
    return true;

  MAUS::ReconEventPArray *recEvts = new MAUS::ReconEventPArray();
  for (int iPe = 0; iPe < nPartEvts; ++iPe)
     recEvts->push_back(new MAUS::ReconEvent);

  spill->SetReconEvents(recEvts);

//   int evSize = spill->GetDAQData()->GetEventSize();
//   int evId = spill->GetEventId();
//   cout << "Online Input ("<< event_type << ") size: " << evSize/(1024.*1024.) << " MB  evId: " << evId
//        << "  nPartEvts: " << nPartEvts <<"  deltaT: " << GET_DURATION(t0)*1e3 << " ms" << endl;

  return true;
}

void OnlineDaqInput::close() {
  daqInput_.death();
}

#endif

/////////////////////// DetectorsProcessor /////////////////////////////////////

DetectorsProcessor::DetectorsProcessor()
:MAUSDataProcessor("DetectorsProcessor") {}

DetectorsProcessor::~DetectorsProcessor() {}

void DetectorsProcessor::init(std::string s) {
//   cout << s << endl;
  tofDigits_.birth(s);
  tofSlHits_.birth(s);
  tofSpPoints_.birth(s);
  klDigits_.birth(s);
  klCellHits_.birth(s);
  ckovDigits_.birth(s);
  tracker_digits_.birth(s);
  tracker_rec_.birth(s);
}

bool DetectorsProcessor::process() {

  try {
    tofDigits_.process(*output_);
    tofSlHits_.process(*output_);
    tofSpPoints_.process(*output_);
    klDigits_.process(*output_);
    klCellHits_.process(*output_);
    ckovDigits_.process(*output_);
    tracker_digits_.process(*output_);
    tracker_rec_.process(*output_);
  } catch(MAUS::Exceptions::Exception & lExc) {
    std::unique_lock<std::mutex> lck (mtx, std::defer_lock);
    lck.lock();
    std::cerr << "*** MAUS exception in DetectorsProcessor::process ***" << std::endl;
    std::cerr << lExc.what() << std::endl;
    lck.unlock();
  }

  std::string event_type = (**output_).GetSpill()->GetDaqEventType();
  if(event_type != "physics_event") {
    --processCount_;
    return true;
  }

  return true;
}

void DetectorsProcessor::close() {
  tofDigits_.death();
  tofSlHits_.death();
  tofSpPoints_.death();
  klDigits_.death();
  klCellHits_.death();
  ckovDigits_.death();
  tracker_digits_.death();
  tracker_rec_.death();
}

/////////////////////// EMRProcessor /////////////////////////////////////

EMRProcessor::EMRProcessor()
:MAUSDataProcessor("EMRProcessor") {}

void EMRProcessor::init(std::string s) {
//   cout << s << endl;
  emrPHits_.birth(s);
  emrSPnts_.birth(s);
  emrRecon_.birth(s);
}

bool EMRProcessor::process() {

  try {
    emrPHits_.process(*output_);
    emrSPnts_.process(*output_);
    emrRecon_.process(*output_);
  } catch(MAUS::Exceptions::Exception & lExc) {
    std::unique_lock<std::mutex> lck (mtx, std::defer_lock);
    lck.lock();
    std::cerr << "*** MAUS exception in EMRProcessor::process ***" << std::endl;
    std::cerr << lExc.what() << std::endl;
    lck.unlock();
  }

  std::string event_type = (**output_).GetSpill()->GetDaqEventType();
  if(event_type != "physics_event") {
    --processCount_;
    return true;
  }

  return true;
}

void EMRProcessor::close() {
  emrPHits_.death();
  emrSPnts_.death();
  emrRecon_.death();
}

////////////////////////// MAUSDataRootOutput /////////////////////////////////

MAUSDataRootOutput::MAUSDataRootOutput()
:NRootOutput("MAUSDataRootOutput") {}

MAUSDataRootOutput::MAUSDataRootOutput(MAUS::Data **r)
:NRootOutput("MAUSDataRootOutput", r) {}

MAUSDataRootOutput::~MAUSDataRootOutput() {
//   tofCalib_.Save();
}

void MAUSDataRootOutput::init(std::string s) {
//   cout << s << endl;
//  tofCalib_.birth(s);
//  std::string xPath(ONREC_DIR);
//  xPath += "/output/";
//  tofCalib_.setFilePath(xPath);

//   MAUSDataStateMachine::verbose_ = true;
}

bool MAUSDataRootOutput::process() {

  MAUSDataStateMachine::processState(*input_);

  std::string event_type = (**input_).GetSpill()->GetDaqEventType();
  if(event_type != "physics_event") {
    --processCount_;
    return true;
  }

  try {
//     tofCalib_.process(*output_);
    NRootOutput::process();
  } catch(MAUS::Exceptions::Exception & lExc) {
    std::unique_lock<std::mutex> lck (mtx, std::defer_lock);
    lck.lock();
    std::cerr << "*** MAUS exception in MAUSDataRootOutput::process ***" << std::endl;
    std::cerr << lExc.what() << std::endl;
    lck.unlock();
  }

  if( !((processCount_+1)%250) )
    cout << processCount_ << " spills completed\n";

  return true;
}

void MAUSDataRootOutput::newRun() {
//   cout << " #### MAUSDataRootOutput::newRun " << runNum_ << " ####\n";
  NRootOutput::newRun(MAUSDataStateMachine::runNum_);

//   tofCalib_.Save();
}

///////////////////// MAUSReduceProcessor ////////////////////////////////////////////

MAUSReduceProcessor::MAUSReduceProcessor()
: MAUSDataProcessor("MAUSReduceProcessor"), canvasId_(0), plotPath_(""), out_file_(0) {}

void MAUSReduceProcessor::init(std::string s) {
  char* pPath;
  pPath = getenv ("MAUS_WEB_MEDIA");
  if (pPath!=NULL) {
    plotPath_.assign(pPath);
    plotPath_ += "/reco/";
  } else {
    plotPath_.assign(ONREC_DIR);
    plotPath_ += "/output/plots/";
  }

  img_ = TImage::Create();
  MAUS::Image *image = new MAUS::Image();
  outImages_.SetImage(image);
  tofReduce_.SetOutput(&outImages_);
  emrReduce_.SetOutput(&outImages_);
  klReduce_.SetOutput(&outImages_);
  ckovReduce_.SetOutput(&outImages_);
  scifiReduce_.SetOutput(&outImages_);
  memReduce_.SetOutput(&outImages_);
  scalerReduce_.SetOutput(&outImages_);
//   cout << s << endl;

  tofReduce_.birth(s);
  emrReduce_.birth(s);
  klReduce_.birth(s);
  ckovReduce_.birth(s);
  scifiReduce_.birth(s);
  memReduce_.birth(s);
  scalerReduce_.birth(s);

  MAUSDataStateMachine::verbose_ = true;
}

bool MAUSReduceProcessor::process() {

  MAUSDataStateMachine::processState(*output_);

  std::string event_type = (**output_).GetSpill()->GetDaqEventType();
  if(event_type != "physics_event") {
    --processCount_;
    return true;
  }

  try {
    tofReduce_.process(*output_);
    emrReduce_.process(*output_);
    klReduce_.process(*output_);
    ckovReduce_.process(*output_);
    scifiReduce_.process(*output_);
    memReduce_.process(*output_);
    scalerReduce_.process(*output_);
    this->printCanvas();

//     if (processCount_%20 == 0)
//       writeInages();

  } catch(MAUS::Exceptions::Exception & lExc) {
    std::unique_lock<std::mutex> lck (mtx, std::defer_lock);
    lck.lock();
    std::cerr << "*** MAUS exception in EMRProcessor::process ***" << std::endl;
    std::cerr << lExc.what() << std::endl;
    lck.unlock();
  }

  return true;
}

void MAUSReduceProcessor::close() {
  this->endOfRun();

  tofReduce_.death();
  emrReduce_.death();
  klReduce_.death();
  ckovReduce_.death();
  scifiReduce_.death();
  memReduce_.death();
  scalerReduce_.death();
}

void MAUSReduceProcessor::newRun() {
  this->endOfRun();

  tofReduce_.reset();
  emrReduce_.reset();
  klReduce_.reset();
  ckovReduce_.reset();
  scifiReduce_.reset();
  scalerReduce_.reset();

  if (MAUSDataStateMachine::runNum_) {
    stringstream file_name;
    file_name << plotPath_ << "Run" << MAUSDataStateMachine::runNum_ << "imageData.root";
    out_file_ = new TFile(file_name.str().c_str(),"recreate");
  }

  processCount_ = 0;
}

void MAUSReduceProcessor::endOfRun() {
  if (out_file_) {
    if (processCount_>10)
      this->writeInages();

    out_file_->Close();
    delete out_file_;
    out_file_ = nullptr;
  }
}

void MAUSReduceProcessor::writeInages() {
  std::vector<MAUS::CanvasWrapper *> canv_wraps = outImages_.GetImage()->GetCanvasWrappers();
  out_file_->cd();
  for (auto &cw : canv_wraps) {
//     cout << "Writing " << cw->GetCanvas()->GetName() << endl;
    cw->GetCanvas()->Write();
  }
}

void MAUSReduceProcessor::printCanvas(int id) {
  TCanvas *c = outImages_.GetImage()->GetCanvasWrappers()[id]->GetCanvas();
//   c->Print("png");
  img_->FromPad(c);

  std::string xName(plotPath_);
  xName += c->GetName();
  xName += ".png";
  img_->WriteImage(xName.c_str());
}

void MAUSReduceProcessor::printCanvas() {
  this->printCanvas(canvasId_);
  ++canvasId_;
  unsigned int nCanv = outImages_.GetImage()->GetCanvasWrappers().size();
  if (canvasId_ == nCanv)
    canvasId_ = 0;
}

///////////////////// MemTest ////////////////////////////////////////////

MemTest::MemTest()
: MAUSDataProcessor("MemTest") {}

bool MemTest::process() {
  std::string event_type = (**output_).GetSpill()->GetDaqEventType();
  if( event_type == "physics_event") {
    if (processCount_%25 == 0) {
      ++processCount_;
      gSystem->GetProcInfo(&info_);
      cout << "MemTest Evt: " << (**output_).GetEvent()->GetSpillNumber()
           << ", MemVirt: " << info_.fMemVirtual/1024.  << " MB"
           << ", MemRes: "  << info_.fMemResident/1024. << " MB\n";
    }
  }

  return true;
}

/////////////////////////// MemFreeOutput ////////////////////////////////

MemFreeOutput::MemFreeOutput()
: OutProcessor<MAUS::Data>("MemFreeOutput") {}

MemFreeOutput::~MemFreeOutput() {}

bool MemFreeOutput::process() {

 (**input_).SetSpill(0);
  return true;
}


