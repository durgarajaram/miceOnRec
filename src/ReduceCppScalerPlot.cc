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
#include "src/common_cpp/Utils/Squeak.hh"

// #include "src/reduce/ReduceCppScalerPlot/ReduceCppScalerPlot.hh"
#include "ReduceCppScalerPlot.hh"

#define N_MEM_SAMPLES 400

namespace MAUS {
using namespace MAUS::Exceptions;

PyMODINIT_FUNC init_ReduceCppScalerPlot(void) {
    PyWrapReduceBase<ReduceCppScalerPlot>::PyWrapReduceBaseModInit(
                                  "ReduceCppScalerPlot", "", "", "", "");
}

ReduceCppScalerPlot::ReduceCppScalerPlot()
: ReduceBase<Data, ImageData>("ReduceCppScalerPlot"),
  _refresh_rate(1), _process_count(0),   _tof0_hits(N_MEM_SAMPLES, 0.),
  _tof1_hits(N_MEM_SAMPLES, 0.), _tof2_hits(N_MEM_SAMPLES, 0.),
  _triggers(N_MEM_SAMPLES, 0.), _trigger_requests(N_MEM_SAMPLES, 0.),
  _h_tof0_hits(NULL), _h_tof1_hits(NULL), _h_tof2_hits(NULL), _h_triggers(NULL),
  _h_trigger_requests(NULL), _legend(NULL), _canv_scalers(NULL) {}

ReduceCppScalerPlot::~ReduceCppScalerPlot() {

  // CanvasWrapper objects will be deleted by the ImageData destructor.
}

void ReduceCppScalerPlot::_birth(const std::string& str_config) {

  if (!_output) {
    throw Exception(exceptionLevel::nonRecoverable,
                          "The output is disconnected.",
                          "ReduceCppScalerPlot::_birth");
  }

  if (!_output->GetImage())
    _output->SetImage(new MAUS::Image());

  _process_count = 0;

  // define histograms
  _h_tof0_hits = new TH1I("hTif0Hits", "TOF0 hits per spill; Spills; TOF0 hits",
                       N_MEM_SAMPLES, -N_MEM_SAMPLES*_refresh_rate, 0);
  _h_tof0_hits->SetLineColor(kGreen);
  _h_tof0_hits->SetLineWidth(2);
  _h_tof0_hits->SetMinimum(.0);

  _h_tof1_hits = new TH1I("hTif1Hits", "TOF1 hits per spill; Spills; TOF1 hits",
                       N_MEM_SAMPLES, -N_MEM_SAMPLES*_refresh_rate, 0);
  _h_tof1_hits->SetLineColor(kMagenta);
  _h_tof1_hits->SetLineWidth(2);

  _h_tof2_hits = new TH1I("hTif2Hits", "TOF2 hits per spill; Spills; TOF2 hits",
                       N_MEM_SAMPLES, -N_MEM_SAMPLES*_refresh_rate, 0);
  _h_tof2_hits->SetLineColor(kRed);
  _h_tof2_hits->SetLineWidth(2);

  _h_triggers = new TH1I("hTriggers", "Triggers per spill; Spills; Triggers",
                       N_MEM_SAMPLES, -N_MEM_SAMPLES*_refresh_rate, 0);
  _h_triggers->SetLineColor(kBlack);
  _h_triggers->SetLineWidth(2);

  _h_trigger_requests = new TH1I("hTrReqs", "Tr. requests per spill; Spills; Tr. req.",
                       N_MEM_SAMPLES, -N_MEM_SAMPLES*_refresh_rate, 0);
  _h_trigger_requests->SetLineColor(kBlue);
  _h_trigger_requests->SetLineWidth(2);

  _legend = new TLegend(0.1,0.35,0.3,0.5);
  _legend->SetTextSize(0.025);
  _legend->AddEntry(_h_triggers,"Trig. per spill","l");
  _legend->AddEntry(_h_trigger_requests,"Tr.req. per spill","l");
  _legend->AddEntry(_h_tof0_hits,"TOF0 hits per spill","l");
  _legend->AddEntry(_h_tof1_hits,"TOF1 hits per spill","l");
  _legend->AddEntry(_h_tof2_hits,"TOF2 hits per spill","l");

  // define canvases
  // define canvases 
  _canv_scalers = new TCanvas("ScalerReduce", "Scaler", 1600, 1200);
  _canv_scalers->cd();
  _h_tof0_hits->Draw();
  _h_tof1_hits->Draw("same");
  _h_tof2_hits->Draw("same");
  _h_trigger_requests->Draw("same");
  _h_triggers->Draw("same");
  _legend->Draw();

  _cwrap_scalers = new CanvasWrapper();
  _cwrap_scalers->SetDescription("");
  _cwrap_scalers->SetFileTag("");
  _cwrap_scalers->SetCanvas(_canv_scalers);

  _output->GetImage()->CanvasWrappersPushBack(_cwrap_scalers);
}

void ReduceCppScalerPlot::_death() {}

void ReduceCppScalerPlot::_process(MAUS::Data* data) {
  if (data == NULL)
    throw Exception(exceptionLevel::recoverable, "Data was NULL",
                    "ReduceCppScalerPlot::_process");

  if (data->GetSpill() == NULL)
    throw Exception(exceptionLevel::recoverable, "Spill was NULL",
                    "ReduceCppScalerPlot::_process");

  std::string ev_type = data->GetSpill()->GetDaqEventType();
  if (ev_type != "physics_event")
     return;

  if (data->GetSpill()->GetReconEvents() == NULL)
     throw Exception(exceptionLevel::recoverable, "ReconEvents were NULL",
                        "ReduceCppScalerPlot::_process");

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

void ReduceCppScalerPlot::reset() {}

void ReduceCppScalerPlot::update_mon_plots(MAUS::Spill *s) {

  Channels scaler = s->GetDAQData()->GetV830().GetChannels();
  int nTrigs    = scaler.GetCh0();
  _triggers.push_back(nTrigs);
  _triggers.pop_front();
  *_h_triggers << _triggers;

  int nTrReqs   = scaler.GetCh1();
  _trigger_requests.push_back(nTrReqs);
  _trigger_requests.pop_front();
  *_h_trigger_requests << _trigger_requests;

  int nTof0Hits = scaler.GetCh3();
  _tof0_hits.push_back(nTof0Hits);
  _tof0_hits.pop_front();
  *_h_tof0_hits << _tof0_hits;

  int nTof1Hits = scaler.GetCh4();
  _tof1_hits.push_back(nTof1Hits);
  _tof1_hits.pop_front();
  *_h_tof1_hits << _tof1_hits;

  int nTof2Hits = scaler.GetCh5();
  _tof2_hits.push_back(nTof2Hits);
  _tof2_hits.pop_front();
  *_h_tof2_hits << _tof2_hits;

  _canv_scalers->Update();
}

TH1I& operator<< (TH1I &h, std::deque<double> s) {
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
