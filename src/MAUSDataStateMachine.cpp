// MAUS
// #include "src/legacy/Interface/Squeak.hh"
#include "src/common_cpp/Utils/Squeak.hh"

// Mic11
#include "MAUSDataStateMachine.h"

using namespace std;
using namespace MAUS;

MAUSDataStateMachine::MAUSDataStateMachine()
: state_(IDLE), runNum_(-99), spillNum_(-99), verbose_(false) {}

MAUSDataStateMachine::~MAUSDataStateMachine() {}

int MAUSDataStateMachine::processState(Data *data) {
std::string event_type = data->GetSpill()->GetDaqEventType();
  Squeak::setStandardOutputs(0);

//   if (verbose_) {
//     cout << "\nprocessState " << event_type << endl;
//     cout << "Old state: " << state_ << std::endl;
//   }

  switch (state_) {
    case IDLE:
      if (event_type == "root_reset") {
        state_ = IDLE;
      } else {
        runNum_ = data->GetSpill()->GetRunNumber();
        this->newRun();
      }

      if (event_type == "start_of_run") {
        state_ = NEW_RUN;
        if (verbose_) cout << " ++++ Start of Run " << runNum_ << " ++++\n";
      } else if (event_type == "physics_event") {
        state_ = TAKING_DATA;
        if (verbose_) cout << " ++++ This is Run " << runNum_ << " ++++\n";
      }
      break;

    case NEW_RUN:
      if (event_type == "root_reset") {
       state_ = NEW_RUN;
      } else if (event_type == "end_of_run") {
        runNum_ = data->GetSpill()->GetRunNumber();
        state_ = END_RUN;
        if (verbose_) cout << " ++++ End of Run " << runNum_ << " ++++\n";
      } else if (event_type != "start_of_run") {
        state_ = TAKING_DATA;
      }
      break;

    case TAKING_DATA:
      if (event_type == "root_reset")
       state_ = TAKING_DATA;
      else if (data->GetSpill()->GetErrors().count("bad_data_input") == 0) {
        if (event_type == "end_of_run") {
          runNum_ = data->GetSpill()->GetRunNumber();
          this->endOfRun();
          state_ = END_RUN;
          if (verbose_) cout << " ++++ End of Run " << runNum_ << " ++++\n";
        } else if (event_type == "start_of_run") {
          runNum_ = data->GetSpill()->GetRunNumber();
          this->newRun();
          state_ = NEW_RUN;
          if (verbose_) cout << " ++++ Start of Run " << runNum_ << " ++++\n";
        } else if (runNum_ != data->GetSpill()->GetRunNumber()) {
          runNum_ = data->GetSpill()->GetRunNumber();
          this->newRun();
          if (verbose_) cout << " ++++ This is Run " << runNum_ << " ++++\n";
        } else
          runNum_ = data->GetSpill()->GetRunNumber();
      }
      break;

    case END_RUN:
      if (event_type == "root_reset")
       state_ = END_RUN;
      else if (event_type == "start_of_run") {
        runNum_ = data->GetSpill()->GetRunNumber();
        this->newRun();
        state_ = NEW_RUN;
        if (verbose_) cout << " ++++ Start of Run " << runNum_ << " ++++\n";
      }
      break;

  }

//   if (verbose_) std::cout << "New state: " << state_ << std::endl;

  return state_;
}


