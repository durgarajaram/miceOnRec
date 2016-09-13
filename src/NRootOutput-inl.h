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

template <class dataType>
NRootOutput<dataType>::NRootOutput()
: OutProcessor<dataType>("NRootOutput"), outFile_(0), outTree_(0) {
  OutProcessor<dataType>::input_ = new dataType*;
  *OutProcessor<dataType>::input_ = new dataType();
}

template <class dataType>
NRootOutput<dataType>::NRootOutput(std::string n)
: OutProcessor<dataType>(n), outFile_(0), outTree_(0) {
  OutProcessor<dataType>::input_ = new dataType*;
  *OutProcessor<dataType>::input_ = new dataType;
}

template <class dataType>
NRootOutput<dataType>::NRootOutput(std::string n, dataType **r)
: OutProcessor<dataType>(n), outFile_(0), outTree_(0) {
  OutProcessor<dataType>::input_ = r;
}

template <class dataType>
bool NRootOutput<dataType>::process() {
  outTree_->Fill();
  return true;
}

template <class dataType>
void NRootOutput<dataType>::close() {
  if (outFile_) {
    outFile_->cd();
    std::cout << "Saving file " << outFile_->GetName() << std::endl;
    outTree_->Write();
//     outFile_->Close("R");
//     TProcessID::SetObjectCount(0);

    delete outTree_;
    outTree_ = 0;

    delete outFile_;
    outFile_ = 0;
  }
}

template <class dataType>
void NRootOutput<dataType>::setInputObj(dataType **r) {
  OutProcessor<dataType>::setInputObj(r);
  if (outTree_)
    this->resetOutBranche();
}

template <class dataType>
void NRootOutput<dataType>::resetOutBranche() {
  outTree_->GetBranch( (**OutProcessor<dataType>::input_).getRecordName().c_str() )
          ->SetAddress(OutProcessor<dataType>::input_);
//   std::cout << "Reset Output Tree Branche: "
//             << (**OutProcessor<dataType, dataType>::input_).getRecordName().c_str() << std::endl;
}

template <class dataType>
void NRootOutput<dataType>::newRun(int runNum) {
  this->close();
//   gROOT->Reset();

  this->makeOutFile(runNum);
  this->makeOutTree();
  this->makeOutBranch();
}

template <class dataType>
void NRootOutput<dataType>::makeOutTree() {
  if (outTree_) delete outTree_;

  outTree_ = new TTree( (BaseProcessor::name_+"Tree").c_str(),
                        (BaseProcessor::name_+" data output").c_str() );

//   outTree_->SetDirectory(0);
//   cout << name_ << "  tree addr: " << outTree_ << endl;
}

template <class dataType>
void NRootOutput<dataType>::makeOutBranch() {
  outTree_->Branch((**OutProcessor<dataType>::input_).getRecordName().c_str(),
                         OutProcessor<dataType>::input_);
//   std::cout << "Make Output Tree Branche: "
//             << (**OutProcessor<dataType, dataType>::input_).getRecordName().c_str() << std::endl;
}

template <class dataType>
void NRootOutput<dataType>::makeOutFile() {
  if (outFile_) delete outFile_;

  std::string fname(ONREC_DIR);
  fname += "/output/" + BaseProcessor::name_ + ".root";
  outFile_ = new TFile(fname.c_str(), "recreate");
}

template <class dataType>
void NRootOutput<dataType>::makeOutFile(int runNum) {
  if (outFile_) delete outFile_;

  std::stringstream ss;
  ss << ONREC_DIR << "output/Run" << runNum <<  ".root";
//   std::cout << "Creating  file " << ss.str() << std::endl;
  outFile_ = new TFile(ss.str().c_str(), "recreate");
}

template <class dataType>
void NRootOutput<dataType>::setOutTreeBasketSize(int basketSize) {
    this->outTree_->SetBasketSize((**OutProcessor<dataType>::input_).getRecordName().c_str(), basketSize);
}


