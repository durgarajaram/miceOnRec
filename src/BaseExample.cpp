#include "MAUSWorkers.h"
#include "getArgs.h"

int main(int argc, char* argv[]) {

  std::string json_conf = getArgs(argc, argv);
  if (json_conf == "@") return 1;

  DAQInputWorker       w0(0);
  EMRWorker            w1(1);
  DetectorsWorker      w2(2);
  MAUSReduceWorker     w3(3);
  MAUSRootOutputWorker w4(4);
//   MemFreeWorker        w5(5);

  std::vector<WInterface*> workers;
  workers.push_back(&w0);

  w0 >> w1.getInput();
  w1 >> w2.getInput();
  w2 >> w3.getInput();
  w3 >> w4.getInput();

  w0.init(json_conf);
  w1.init(json_conf);
  w2.init(json_conf);
  w3.init(json_conf);
  w4.init(json_conf);

  vector<thread> threads;
  threads.push_back( thread(std::ref(w0), 100) ); // Start the work. 100 iterations.
//   threads.push_back( thread(std::ref(w0)) ); // Start the work. Run forever.
  threads.push_back( thread(std::ref(w1)) );
  threads.push_back( thread(std::ref(w2)) );
  threads.push_back( thread(std::ref(w3)) );
  threads.push_back( thread(std::ref(w4)) );

  for (auto &t:threads)
    t.join();

  return 0;
}