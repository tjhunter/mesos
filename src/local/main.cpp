#include <iostream>
#include <string>

#include "local.hpp"

#include "configurator/configurator.hpp"

#include "common/logging.hpp"

#include "master/master.hpp"

#include "slave/slave.hpp"

using std::cerr;
using std::endl;
using std::string;

using namespace mesos::internal;
using mesos::internal::master::Master;
using mesos::internal::slave::Slave;


void usage(const char* programName, const Configurator& conf)
{
  cerr << "Usage: " << programName
       << " [--port=PORT] [--slaves=N] [--cpus=CPUS] [--mem=MEM] [...]" << endl
       << endl
       << "Launches a single-process cluster containing N slaves, each of "
       << "which report" << endl << "CPUS cores and MEM bytes of memory."
       << endl
       << endl
       << "Supported options:" << endl
       << conf.getUsage();
}


int main (int argc, char **argv)
{
  Configurator conf;
  conf.addOption<int>("port", 'p', "Port to listen on", 5050);
  conf.addOption<string>("ip", "IP address to listen on");
  local::registerOptions(&conf);

  if (argc == 2 && string("--help") == argv[1]) {
    usage(argv[0], conf);
    exit(1);
  }

  Params params;
  try {
    params = conf.load(argc, argv, true);
  } catch (ConfigurationException& e) {
    cerr << "Configuration error: " << e.what() << endl;
    exit(1);
  }

  Logging::init(argv[0], params);

  if (params.contains("port"))
    setenv("LIBPROCESS_PORT", params["port"].c_str(), 1);

  if (params.contains("ip"))
    setenv("LIBPROCESS_IP", params["ip"].c_str(), 1);

  const PID &master = local::launch(params, false);

  Process::wait(master);

  return 0;
}
