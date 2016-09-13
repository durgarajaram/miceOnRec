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

#ifndef SRC_REDUCECPMEMUSAGE_HH
#define SRC_REDUCECPMEMUSAGE_HH 1

// C++
#include <iostream>
#include <deque>

// Root
#include "TH1.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TSystem.h"

// Maus
#include "src/common_cpp/API/ReduceBase.hh"
#include "src/common_cpp/API/PyWrapReduceBase.hh"
#include "src/common_cpp/DataStructure/ImageData/ImageData.hh"
#include "src/common_cpp/DataStructure/ImageData/Image.hh"
#include "src/common_cpp/DataStructure/ImageData/CanvasWrapper.hh"
#include "src/common_cpp/DataStructure/Data.hh"

namespace MAUS {


class ReduceCppOnrecMonitor : public ReduceBase<Data, ImageData> {
 public:
  ReduceCppOnrecMonitor();
  ~ReduceCppOnrecMonitor();

  int  getRefreshRate() {return _refresh_rate;}
  void reset();
  void update();

 private:

  void _birth(const std::string& config);

  void _death();

  void _process(Data* data);

  void update_mon_plots(MAUS::Spill *s);

  int _refresh_rate;
  int _process_count;

  int _previous_ev_id;
  int _previous_ev_time;

  std::deque<double> _mem_sample_v;
  std::deque<double> _mem_sample_r;
  std::deque<double> _rate_sample;
  std::deque<double> _ev_missed_sample;
  std::deque<double> _ev_size_sample;
  std::deque<double> _part_ev_sample;

  ProcInfo_t _info;
  TH1F *_h_memRes, *_h_memVirt, *_h_EvId_delta, *_h_Ev_rate, *_h_Ev_size, *_h_Part_Ev;
  TLegend *_legend;
  TCanvas *_canv_mem, *_canv_mon;
  CanvasWrapper *_cwrap_mem, *_cwrap_mon;
};

TH1F&  operator<< (TH1F& h, std::deque<double> s);
}

#endif // SRC_REDUCECPMEMUSAGE_HH
