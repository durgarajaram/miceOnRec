#ifndef GETARG_H
#define GETARG_H 1

#include <fstream>
#include <sstream>

// Maus
#include "json/json.h"
#include "unpacking/MDargumentHandler.h"

// Mic11
#include "Config.h"

std::string getArgs(int argc, char ** argv) {
  string stringBuf;
//   int parArg;
  //char filepath[128];
  char runpath[128];
  char filename[128];

  // The following shows how to use the MDargumentHandler class
  // to deal with the main arguments
  // Define the arguments
  MDargumentHandler argh("Example of unpacking application.");
  argh.AddArgument("help","print this message","h");
  argh.AddArgument("directory","path for the data file","d","<string>","/home/daq/Downloads" );
  argh.AddArgument("file","List of data file names or run number (space separated, within double quote)","f","<string>","mandatory");

  // Check the user arguments consistancy
  // All mandatory arguments should be provided and
  // There should be no extra arguments
  if ( argh.ProcessArguments(argc, argv) ) {argh.Usage(); return "@";}

  // Treat arguments, obtain values to be used later
  if ( argh.GetValue("help") ) {argh.Usage(); return "@";}
  if ( argh.GetValue("directory", stringBuf) != MDARGUMENT_STATUS_OK ) return "@";
  strcpy(runpath,stringBuf.c_str());

  if ( argh.GetValue("file", stringBuf) != MDARGUMENT_STATUS_OK ) return "@";
  strcpy(filename, stringBuf.c_str());

  Json::Value jsonConf;
  Json::Reader reader;

  stringstream buffer_in, buffer_out;
  string fName(ONREC_DIR);
  fName += "bin/config.json";
  ifstream config_doc(fName);
  buffer_in << config_doc.rdbuf();

  reader.parse( buffer_in.str(), jsonConf);

//   cout << jsonConf << endl;

  jsonConf["daq_data_path"] = std::string(runpath);
  jsonConf["daq_data_file"] = std::string(filename);

  buffer_out << jsonConf;
  return buffer_out.str();
}

std::string getArgs() {

  stringstream buffer;
  string fName(ONREC_DIR);
  fName += "bin/config.json";
  ifstream config_doc(fName);
  buffer << config_doc.rdbuf();

  return buffer.str();
}
#endif

