
#include <iostream>
#include <map>
#include <string>

#include "vito_p300.h"
#include "unit.h"

char *progname;

void show_usage()
{
	std::cerr << "usage " << progname << " [options] hostname port [command ...]" << std::endl;
	std::cerr << "   or " << progname << " /dev/name [command ...]" << std::endl;
	std::cerr << "   or " << progname << " --version" << std::endl;
	std::cerr << "   or " << progname << " --commands" << std::endl;
	std::cerr << "option" << progname << " --format munin|blank|php" << std::endl;
	// std::cerr << "   or " << progname << " --all" << std::endl;
}

void show_version()
{
	std::cout << progname << " version 0.1" << std::endl;
}

void show_commands(std::map<std::string, command *> *comm_map)
{
	std::cout << progname << "Commands configured:" << std::endl;
	std::map<std::string, command *>::const_iterator it; // declare an iterator
	it = comm_map->begin(); // assign it to the start of the vector
	while (it != comm_map->end()) // while it hasn't reach the end
	{
		std::cout << it->second->get_myName()
			<< std::endl;
		//std::cout << *it->second << std::endl;
		it++; // and iterate to the next element
	}
	
}

std::map<std::string, command *> * initCommands()
{
	vito_unit *unitDecoderCycleTime   = new vito_unit(CycleTime);
	vito_unit *unitDecoderTemperature = new vito_unit(Temperature, 0.1);
	vito_unit *unitDecoderTemperature2= new vito_unit(Temperature, 0.01);
	vito_unit *unitDecoderRaw         = new vito_unit(Raw);
	vito_unit *unitDecoderInterger1   = new vito_unit(Integer1);
	vito_unit *unitDecoderInterger2   = new vito_unit(Integer2);
	vito_unit *unitDecoderInterger4   = new vito_unit(Integer4);
	
	//command *getTempA            = new command("getTempA",            0x5525, 2, 0.1,    1.0);
	command *getTempA            = new command("getTempA",            0x5525, 2, 0.1,    1.0, unitDecoderTemperature);
	command *getTempWWist        = new command("getTempWWist",        0x0804, 2, 0.1,   10.0, unitDecoderTemperature);
	command *getKesselTemp       = new command("getTempKist",         0xA393, 2, 0.01, 100.0, unitDecoderTemperature2);
	command *getTempVListM2      = new command("getTempVListM2",      0x3900, 2, 0.1,    1.0, unitDecoderTemperature);
	command *getTempVLsollM2     = new command("getTempVLsollM2",     0x3544, 2, 0.1,    1.0, unitDecoderTemperature);
	command *getTempKol          = new command("getTempKol",          0x6564, 2, 0.1,    1.0, unitDecoderTemperature);
	command *getTempSpu          = new command("getTempSpu",          0x6566, 2, 0.1,    1.0, unitDecoderTemperature);
	command *getTempRaumNorSollM1= new command("getTempRaumNorSollM1",0x2306, 1, 1.0,    1.0, unitDecoderInterger1);
	command *getTempRaumNorSollM2= new command("getTempRaumNorSollM2",0x3306, 1, 1.0,    1.0, unitDecoderInterger1);
	command *getTempRaumRedSollM1= new command("getTempRaumRedSollM1",0x2307, 1, 1.0,    1.0, unitDecoderInterger1);
	command *getTempRaumRedSollM2= new command("getTempRaumRedSollM2",0x3307, 1, 1.0,    1.0, unitDecoderInterger1);
	command *getBrennerStarts    = new command("getBrennerStarts",    0x088A, 4, 1.0,    1.0, unitDecoderInterger4);
	command *getPumpeStatusSp    = new command("getPumpeStatusSp",    0x6513, 1, 1.0,    1.0, unitDecoderInterger1);
	command *getPumpeStatusZirku = new command("getPumpeStatusZirku", 0x6515, 1, 1.0,    1.0, unitDecoderInterger1);
	command *getPumpeStatusSolar = new command("getPumpeStatusSolar", 0x6552, 1, 1.0,    1.0, unitDecoderInterger1);
	command *getPumpeStatusM1    = new command("getPumpeStatusM1",    0x2906, 1, 1.0,    1.0, unitDecoderInterger1);
	command *getPumpeStatusM2    = new command("getPumpeStatusM2",    0x3906, 1, 1.0,    1.0, unitDecoderInterger1);
	command *getSolarStatusWW    = new command("getSolarStatusWW",    0x6551, 1, 1.0,    1.0, unitDecoderInterger1);
	command *getSolarStunden     = new command("getSolarStunden",     0x6568, 2, 1.0,    1.0, unitDecoderInterger2);
	command *getSolarLeistung    = new command("getSolarLeistung",    0x6560, 4, 1.0,    1.0, unitDecoderInterger4);
	// command *getSolarNachladeUnt = new command("getSolarNachladeUnt", 0x6551, 1, 1.0,    1.0); // Siehe getSolarStatusWW
	command *getDevType			 = new command("getDevType",          0x00F8, 2, 1.0,    1.0, unitDecoderRaw);
	command *getUmschaltventil   = new command("getUmschaltventil",   0x0A10, 1, 1.0,    1.0, unitDecoderInterger1);
	command *getSystemTime       = new command("getSystemTime",       0x088E, 8, 1.0,    1.0);
	command	*getTimerZirkuMo	 = new command("getTimerZirkuMo",	  0x2200, 8, 1.0,    1.0, unitDecoderCycleTime);
	command	*getTimerZirkuDi	 = new command("getTimerZirkuDi",	  0x2208, 8, 1.0,    1.0, unitDecoderCycleTime);
	command	*getTimerZirkuMi	 = new command("getTimerZirkuMi",	  0x2210, 8, 1.0,    1.0, unitDecoderCycleTime);
	command	*getTimerZirkuDo	 = new command("getTimerZirkuDo",	  0x2218, 8, 1.0,    1.0, unitDecoderCycleTime);
	command	*getTimerZirkuFr	 = new command("getTimerZirkuFr",	  0x2220, 8, 1.0,    1.0, unitDecoderCycleTime);
	command	*getTimerZirkuSa	 = new command("getTimerZirkuSa",	  0x2228, 8, 1.0,    1.0, unitDecoderCycleTime);
	command	*getTimerZirkuSo	 = new command("getTimerZirkuSo",	  0x2230, 8, 1.0,    1.0, unitDecoderCycleTime);
	command *getTimerM2Mo        = new command("getTimerM2Mo",        0x3000, 8, 1.0,    1.0, unitDecoderCycleTime);
	command *getx				 = new command("getx",                0x0A3B, 2, 1.0,   1.0);
	command *getx2				 = new command("getx2",               0x0A3C, 2, 1.0,   1.0);
	command *getTempRaum		 = new command("getTempRaum",         0x0898, 2, 0.1,   1.0, unitDecoderTemperature);
		
	std::map<std::string, command *> *comm_map = new std::map<std::string, command *>;
	comm_map->insert(std::make_pair("getTempA",				getTempA));
	comm_map->insert(std::make_pair("getTempWWist",			getTempWWist));
	comm_map->insert(std::make_pair("getTempKist",			getKesselTemp));
	comm_map->insert(std::make_pair("getTempVListM2",		getTempVListM2));
	comm_map->insert(std::make_pair("getTempVLsollM2",		getTempVLsollM2));
	comm_map->insert(std::make_pair("getTempKol",			getTempKol));
	comm_map->insert(std::make_pair("getTempSpu",			getTempSpu));
	comm_map->insert(std::make_pair("getTempRaumNorSollM1",	getTempRaumNorSollM1));
	comm_map->insert(std::make_pair("getTempRaumNorSollM2",	getTempRaumNorSollM2));
	comm_map->insert(std::make_pair("getTempRaumRedSollM1",	getTempRaumRedSollM1));
	comm_map->insert(std::make_pair("getTempRaumRedSollM2",	getTempRaumRedSollM2));
	comm_map->insert(std::make_pair("getBrennerStarts",		getBrennerStarts));
	comm_map->insert(std::make_pair("getPumpeStatusSp",		getPumpeStatusSp));
	comm_map->insert(std::make_pair("getPumpeStatusZirku",	getPumpeStatusZirku));
	comm_map->insert(std::make_pair("getPumpeStatusSolar",	getPumpeStatusSolar));
	comm_map->insert(std::make_pair("getPumpeStatusM1",		getPumpeStatusM1));
	comm_map->insert(std::make_pair("getPumpeStatusM2",		getPumpeStatusM2));
	comm_map->insert(std::make_pair("getSolarStatusWW",		getSolarStatusWW));
	comm_map->insert(std::make_pair("getSolarStunden",		getSolarStunden));
	comm_map->insert(std::make_pair("getSolarLeistung",		getSolarLeistung));
	comm_map->insert(std::make_pair("getDevType",			getDevType));
	comm_map->insert(std::make_pair("getUmschaltventil",	getUmschaltventil));
	comm_map->insert(std::make_pair("getSystemTime",		getSystemTime));
	comm_map->insert(std::make_pair("getTimerZirkuMo",		getTimerZirkuMo));
	comm_map->insert(std::make_pair("getTimerZirkuDi",		getTimerZirkuDi));
	comm_map->insert(std::make_pair("getTimerZirkuMi",		getTimerZirkuMi));
	comm_map->insert(std::make_pair("getTimerZirkuDo",		getTimerZirkuDo));
	comm_map->insert(std::make_pair("getTimerZirkuFr",		getTimerZirkuFr));
	comm_map->insert(std::make_pair("getTimerZirkuSa",		getTimerZirkuSa));
	comm_map->insert(std::make_pair("getTimerZirkuSo",		getTimerZirkuSo));
	comm_map->insert(std::make_pair("getTimerM2Mo",			getTimerM2Mo));
	comm_map->insert(std::make_pair("getx",        getx));
	comm_map->insert(std::make_pair("getx2",        getx2));
	comm_map->insert(std::make_pair("getTempRaum",        getTempRaum));
	
	return (comm_map);
}

/*
 * main routine to parse the arguments and delegate the work
 */
int main (int argc, char *argv[]) {

	// bool all_flag;
	bool hostname = false;
	bool devname  = false;
	p300 *c_stream = NULL;
	std::map<std::string, command *> *comm_map;
	
	progname = argv[0];
	
	DPRINT(std::cerr << "DPRINT " << progname << " debugging is active." << std::endl;)
	DPRINT(std::cerr << "main: calling init" <<std::endl;)

	if (argc < 2) {
		show_usage();
		exit(2);
    }
	
	comm_map = initCommands();

	argc--;
	argv++;		// skip the 0th argument, which is the programm name
	
	// loop over the remaining args if there are any
	while (argc) {
		if (strcmp("--version", argv[0]) == 0) {
			show_version();
			exit(0);
		}
		if (strcmp("--commands", argv[0]) == 0) {
			show_commands(comm_map);
			exit(0);
		}
		if (strcmp("--format", argv[0]) == 0) {
			if (argc < 2) {
				std::cerr << "missing argument to --format" << std::endl;
				exit(-1);
			}
			argv++;
			argc--;
			if (strcmp("munin", argv[0]) == 0) {
				// set format munin, this is the default
				DPRINT(std::cerr << "set format munin, this is the default\n";)
				argv++;
				argc--;
				continue;
			}
			if (strcmp("blank", argv[0]) == 0) {
				// set format blank, no label, just the value
				DPRINT(std::cerr << "set format blank, no label, just the value\n";)
				argv++;
				argc--;
				continue;
			}
			if (strcmp("php", argv[0]) == 0) {
				// set format php, parse with php, sets var in php
				DPRINT(std::cerr << "set format php, parse with php, sets var in php\n";)
				argv++;
				argc--;
				continue;
			}
			std::cerr << "unknown format: default set to munin\n";
			argv++;
			argc--;
			continue;
		}
#if 0
		// TODO: der Teil ist noch unsauber. kompiliert zwar aber argv bearbeiten muss ich noch richtig machen
		if (strcmp("--all", argv[1]) == 0) {
			all_flag = true;
			argv++;
			goto doTheWork;
		}
#endif
		// check for either a devicename or a hostname port pair
		if (hostname == false and devname == false) {
			if (*argv[0] == '/') {
				// init mit serieller Schnittstelle
				DPRINT(std::cerr << "init seriell\n";)
				c_stream = new p300(argv[0],0,0,0);
				devname = true;
				argv++;
				argc--;
				continue;
			} else if (argc >1) {
				// init mit hostname port
				DPRINT(std::cerr << "init host port\n";)
				c_stream = new p300(argv[0], atoi(argv[1]));
				hostname = true;
				argv += 2;
				argc -= 2;
				continue;
			} else {
				std::cerr << progname << ": ERROR: argument for port missing\n";
				show_usage();
				exit(0);
			}
			if (c_stream == NULL) {
				std::cerr << "unable to create command stream" << std::endl;
				exit (1);
			}
		}
		// execute the rest of the args as commands
		DPRINT(std::cerr << "about to run command: " << argv[0] << std::endl;)
		if ((*comm_map)[argv[0]]) {
			(*comm_map)[argv[0]]->setFile(c_stream->getFD(), c_stream->getFile());
			(*comm_map)[argv[0]]->sendCommand();
			std::cout << *(*comm_map)[argv[0]] << std::endl;
		} else {
			std::cerr << "unknown command " << argv[0] << std::endl;
		}
		argv++;
		argc--;
	} // while

	// cleanup
	delete c_stream;
	
    return 0;
}
