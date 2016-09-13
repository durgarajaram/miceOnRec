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

/** This is an example application that uses Single Threading
*/

// C++
#include <iostream>

// MAUS
#include "Utils/Exception.hh"

// mic11api
#include "GetBackTrace.h"

#include "MAUSConveyers.h"
#include "getArgs.h"

using namespace std;

int main(int argc, char ** argv) {
//   SetErrorHdlr();

  try {

    DaqInput            input;
    DetectorsProcessor  proc_det;
    EMRProcessor        proc_emr;
    MAUSReduceProcessor reduce;
//     MAUSDataRootOutput  output;
    MemFreeOutput       output;

    MausSTConveyer      myConveyer;

    std::string json_conf = getArgs();

    myConveyer.setInput(&input);
    myConveyer.addWorker(&proc_det);
    myConveyer.addWorker(&proc_emr);
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

