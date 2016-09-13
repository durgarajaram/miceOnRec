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

// C++
#include <iostream>

// Mic11api
#include "GetBackTrace.h"

#include "MAUSConveyers.h"
#include "getArgs.h"

using namespace std;

int main(int argc, char ** argv) {
//   SetErrorHdlr();

  try {

    OnlineDAQInputWorker  input(0);
    EMRWorker             emrRec(1);
    DetectorsWorker       detRec(2);
    MAUSReduceWorker      reduce(3);
    MAUSRootOutputWorker  output(4);
//    MemFreeWorker         output(4);

    MausMTConveyer        myConveyer;
    myConveyer.enableTimeStats();

    std::string json_conf = getArgs();

    myConveyer.setInput(&input);
    myConveyer.addWorker(&emrRec);
    myConveyer.addWorker(&detRec);
    myConveyer.addWorker(&reduce);
    myConveyer.setOutput(&output);

    myConveyer.init(json_conf);

    myConveyer.start();

  } catch(MAUS::Exceptions::Exception & lExc) {
    std::cerr << "*** MAUS exception ***" << std::endl;
    std::cerr << lExc.GetLocation() << std::endl;
    std::cerr << lExc.GetMessage() << std::endl;
    return 1;
  } catch ( MDexception & lExc) {
    std::cerr << "*** Unpacking exception ***" << std::endl;
    std::cerr <<  lExc.GetDescription() << endl;
    return 1;
  } catch (...) {
    std::cerr << "*** Unknown exception ***" << std::endl;
    return 1;
  }

  return 0;
}

