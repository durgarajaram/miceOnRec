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

#include <stdlib.h>

#include <vector>
#include <iostream>
#include <sstream>

#include "TPaveStats.h"
#include "TList.h"

#include "src/common_cpp/DataStructure/Data.hh"
#include "src/common_cpp/DataStructure/Spill.hh"
#include "src/common_cpp/DataStructure/ReconEvent.hh"

#include "src/common_cpp/DataStructure/ImageData/ImageData.hh"
#include "src/common_cpp/DataStructure/ImageData/Image.hh"
#include "src/common_cpp/DataStructure/ImageData/CanvasWrapper.hh"

#include "src/common_cpp/Utils/Squeak.hh"

// #include "src/reduce/ReduceCppOnrecMonitor/ReduceCppOnrecMonitor.hh"
#include "ReduceCppOnrecMonitor.hh"

#define N_MEM_SAMPLES 1400

namespace MAUS {
using namespace MAUS::Exceptions;

PyMODINIT_FUNC init_ReduceCppOnrecMonitor(void) {
    PyWrapReduceBase<ReduceCppOnrecMonitor>::PyWrapReduceBaseModInit(
                                  "ReduceCppOnrecMonitor", "", "", "", "");
}

ReduceCppOnrecMonitor::ReduceCppOnrecMonitor()
: ReduceBase<Data, ImageData>("ReduceCppOnrecMonitor"),
  _refresh_rate(50), _process_count(0), _previous_ev_id(-99), _previous_ev_time(-99),
  _mem_sample_v(N_MEM_SAMPLES, 0.), _mem_sample_r(N_MEM_SAMPLES, 0.), _rate_sample(N_MEM_SAMPLES, 0.),
  _ev_missed_sample(N_MEM_SAMPLES, 0.), _ev_size_sample(N_MEM_SAMPLES, 0.), _part_ev_sample(N_MEM_SAMPLES, 0.),
  _h_memRes(NULL), _h_memVirt(NULL), _h_EvId_delta(NULL), _h_Ev_rate(NULL), _h_Ev_size(NULL), _h_Part_Ev(NULL),
  _legend(NULL), _canv_mon(NULL) {}

ReduceCppOnrecMonitor::~ReduceCppOnrecMonitor() {
//   if (_h_memRes)
//     delete _h_memRes;

//   if (_h_memVirt)
//     delete _h_memVirt;

//   if (_canv_mon)
//     delete _canv_mon;

//   if (_legend)
//     delete _legend;

  // CanvasWrapper objects will be deleted by the ImageData destructor.
}

void ReduceCppOnrecMonitor::_birth(const std::string& str_config) {

  if (!_output) {
    throw MAUS::Exception(exceptionLevel::nonRecoverable,
                          "The output is disconnected.",
                          "ReduceCppOnrecMonitor::_birth");
  }

  if (!_output->GetImage())
    _output->SetImage(new MAUS::Image());

  _process_count = 0;

  // define histograms
  _h_Part_Ev= new TH1F("hPartEv", "Part. events per spill; Spills; Part. events",
                       N_MEM_SAMPLES, -N_MEM_SAMPLES*_refresh_rate, 0);
  _h_EvId_delta = new TH1F("hEvId", "Events missed; Spills; Missed events", N_MEM_SAMPLES, -N_MEM_SAMPLES*_refresh_rate, 0);
  _h_Ev_rate = new TH1F("hEvdT", "Event rate; Spills; Hz", N_MEM_SAMPLES, -N_MEM_SAMPLES*_refresh_rate, 0);
  _h_Ev_size = new TH1F("hEvSize", "Event Size; Spills; KB", N_MEM_SAMPLES, -N_MEM_SAMPLES*_refresh_rate, 0);
  _h_memRes = new TH1F("hMemR", "Memory used; Spills; MB;", N_MEM_SAMPLES, -N_MEM_SAMPLES*_refresh_rate, 0);
  _h_memRes->SetLineColor(kGreen);
  _h_memRes->SetLineWidth(2);

  _h_memVirt = new TH1F("hMemV", "Memory used; Spills;MB;", N_MEM_SAMPLES, -N_MEM_SAMPLES*_refresh_rate, 0);
  _h_memVirt->SetLineColor(kBlue);
  _h_memVirt->SetLineWidth(2);
  _h_memVirt->SetStats(0);
  _h_memVirt->SetMinimum(.0);

  _legend = new TLegend(0.1,0.15,0.3,0.25);
  _legend->SetTextSize(0.025);
  _legend->AddEntry(_h_memRes,"Res. mem. used","l");
  _legend->AddEntry(_h_memVirt,"Virt. mem. used","l");

  // define canvases
  // define canvases 
  _canv_mem = new TCanvas("OnrecMonitorReduce_MenUse", "Memory Usage", 1600, 1200);
  _canv_mem->cd();
  _h_memVirt->Draw();
  _h_memRes->Draw("same");
  _legend->Draw();

  _canv_mon = new TCanvas("OnrecMonitorReduce_Eff", "Onrec Efficiency Monitor", 1600, 1200);
  _canv_mon->cd();
  _canv_mon->Divide(2, 2);

  _canv_mon->cd(1);
  _h_Part_Ev->Draw();

  _canv_mon->cd(2);
  _h_Ev_rate->Draw();

  _canv_mon->cd(3);
  _h_EvId_delta->Draw();

  _canv_mon->cd(4);
  _h_Ev_size->Draw();

  _cwrap_mem = new CanvasWrapper();
  _cwrap_mem->SetDescription("");
  _cwrap_mem->SetFileTag("");
  _cwrap_mem->SetCanvas(_canv_mem);

  _cwrap_mon = new CanvasWrapper();
  _cwrap_mon->SetDescription("");
  _cwrap_mon->SetFileTag("");
  _cwrap_mon->SetCanvas(_canv_mon);

  _output->GetImage()->CanvasWrappersPushBack(_cwrap_mem);
  _output->GetImage()->CanvasWrappersPushBack(_cwrap_mon);
}

void ReduceCppOnrecMonitor::_death() {}

void ReduceCppOnrecMonitor::_process(MAUS::Data* data) {
  if (data == NULL)
    throw Exception(exceptionLevel::recoverable, "Data was NULL",
                    "ReduceCppOnrecMonitor::_process");

  if (data->GetSpill() == NULL)
    throw Exception(exceptionLevel::recoverable, "Spill was NULL",
                    "ReduceCppOnrecMonitor::_process");

  std::string ev_type = data->GetSpill()->GetDaqEventType();
  if (ev_type != "physics_event")
     return;

  if (data->GetSpill()->GetReconEvents() == NULL)
     throw Exception(exceptionLevel::recoverable, "ReconEvents were NULL",
                        "ReduceCppOnrecMonitor::_process");

  MAUS::Spill *spill = data->GetSpill();
  int xRun = spill->GetRunNumber();
  int xSpill = spill->GetSpillNumber();

  _output->SetEventType(ev_type);
  _output->GetImage()->SetRunNumber(xRun);
  _output->GetImage()->SetSpillNumber(xSpill);

  if (!(_process_count%_refresh_rate))
    this->update_mon_plots(spill);

  ++_process_count;
}

void ReduceCppOnrecMonitor::reset() {}

void ReduceCppOnrecMonitor::update_mon_plots(MAUS::Spill *s) {
  gSystem->GetProcInfo(&_info);
  double xMemR, xMemV;
  xMemR = _info.fMemResident/1024.;
  xMemV = _info.fMemVirtual/1024.;

//   Squeak::setStandardOutputs(0);
//   std::cout << "MemTest Evt: " << _process_count
//             << ", MemVirt: " << xMemV << " MB"
//             << ", MemRes: "  << xMemR << " MB\n";

  _mem_sample_r.push_back(xMemR);
  _mem_sample_r.pop_front();

  _mem_sample_v.push_back(xMemV);
  _mem_sample_v.pop_front();

  *_h_memRes  << _mem_sample_r;
  *_h_memVirt << _mem_sample_v;

  if (_previous_ev_time > 0)  {
    int nPartEvts = s->GetReconEvents()->size();
    _part_ev_sample.push_back(nPartEvts);
    _part_ev_sample.pop_front();
    *_h_Part_Ev << _part_ev_sample;

    int eventSize = s->GetDAQData()->GetEventSize();
    _ev_size_sample.push_back(eventSize/(1024));
    _ev_size_sample.pop_front();
    *_h_Ev_size << _ev_size_sample;

    int thisEventId = s->GetEventId();
    int thisEventTime = s->GetTimeStamp();

    double rate = (double)_refresh_rate/(double)(thisEventTime - _previous_ev_time);
    _rate_sample.push_back(rate);
    _rate_sample.pop_front();
    *_h_Ev_rate << _rate_sample;

    int missed = thisEventId - _previous_ev_id - _refresh_rate;
    if (missed < 0) missed = 0;
    _ev_missed_sample.push_back(missed);
    _ev_missed_sample.pop_front();
    *_h_EvId_delta << _ev_missed_sample;
  }

  _previous_ev_id = s->GetEventId();
  _previous_ev_time = s->GetTimeStamp();

  _canv_mon->Update();
}

TH1F& operator<< (TH1F &h, std::deque<double> s) {
  if((int)s.size() == h.GetNbinsX()) {
    int NBINS = s.size();
    for (int xBin=0;  xBin<NBINS; ++xBin) {
      h[xBin+1] = s[xBin];
    }
  } else {
    std::stringstream ss;
    ss << "Number of bins is different!!  ";
    ss << "h(" << h.GetNbinsX() << ")  s(" << s.size() << ")\n";
    throw Exception(exceptionLevel::recoverable, ss.str(),
                        "TH1F& operator<< (TH1D &h, std::deque<double> s)");
  }
  return h;
}

} // MAUS
