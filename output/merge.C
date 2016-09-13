{
  TChain chain("dataTree");
  chain.Add("TofCalibRun6202.root");
  chain.Add("TofCalibRun6203.root");
  chain.Add("TofCalibRun6204.root");
  chain.Add("TofCalibRun6205.root");
  chain.Add("TofCalibRun6206.root");
  chain.Add("TofCalibRun6208.root");
  chain.Add("TofCalibRun6209.root");
  chain.Add("TofCalibRun6210.root");
  chain.Add("TofCalibRun6212.root");
  chain.Add("TofCalibRun6213.root");
  chain.Add("TofCalibRun6214.root");
  chain.Add("TofCalibRun6218.root");
  chain.Add("TofCalibRun6219.root");
  chain.Add("TofCalibRun6222.root");
  //chain.Draw("slabB");
  
  TFile file("tofcalibdata_merged.root", "recreate");
  TTree* mergeTree = chain.CloneTree();
  mergeTree->Write();
  file.Close();
}
