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

#ifndef SRC_REDUCECPSCALER_HH
#define SRC_REDUCECPSCALER_HH 1

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


class ReduceCppScalerPlot : public ReduceBase<Data, ImageData> {
 public:
  ReduceCppScalerPlot();
  ~ReduceCppScalerPlot();

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

  std::deque<double> _tof0_hits;
  std::deque<double> _tof1_hits;
  std::deque<double> _tof2_hits;
  std::deque<double> _triggers;
  std::deque<double> _trigger_requests;

  TH1I *_h_tof0_hits, *_h_tof1_hits, *_h_tof2_hits, *_h_triggers, *_h_trigger_requests;
  TLegend *_legend;
  TCanvas *_canv_scalers;
  CanvasWrapper *_cwrap_scalers;
};

TH1I&  operator<< (TH1I& h, std::deque<double> s);
}

#endif // SRC_REDUCECPSCALER_HH
