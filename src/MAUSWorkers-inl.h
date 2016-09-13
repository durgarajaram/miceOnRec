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

template<class DaqProcessorType>
void DAQWorker<DaqProcessorType>::start(int n) {
  MAUS::Squeak::setStandardOutputs(0);
  int i=0;
  while(i<n || n==0) {
//     cout << i << endl;
    *WOutput<MAUS::Data>::output_ = new MAUS::Data();
    if ( !this->process() ) {
      this->stopWork();
      return;
    }

    std::string xEvType = (*WOutput<MAUS::Data>::output_)->GetSpill()->GetDaqEventType();
//     cout << "Online Input ("<< xEvType << ")" << endl;
    this->push();
    ++i;

    if(this->getROOTprocessID() > _max_object_number ||
      xEvType == "start_of_run")
      this->resetROOTProcesID();
  }

  this->stopWork();
}

template<class DaqProcessorType>
void DAQWorker<DaqProcessorType>::resetROOTProcesID() {
  MAUS::Data *md = new MAUS::Data();
  MAUS::Spill* spill = new MAUS::Spill();
  spill->SetDaqEventType("root_reset");
  md->SetSpill(spill);
  fifo_out_->push(md);

  bool done(false);
  while (!done) {
    usleep(10000);
    if (this->getROOTprocessID() == 0) {
      std::cout << "@@@ ROOT reset done. @@@\n";
      done = true;
    }
  }
}

template<class DaqProcessorType>
unsigned int DAQWorker<DaqProcessorType>::getROOTprocessID() {
//   mutex_.lock();
  unsigned int object_number = TProcessID::GetObjectCount();
//   mutex_.unlock();
  return object_number;
}
