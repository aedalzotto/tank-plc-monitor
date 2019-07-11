#pragma once

#ifdef _WIN32
#define DOS
#else
#define LINUX
#endif

#include <nodave.h>
#include <string>

class PLC {
public:
	enum class DaveProtocol {
		MPI,				/* MPI for S7 300/400 */
		MPI2,				/* MPI for S7 300/400, "Andrew's version" without STX */
		MPI3,				/* MPI for S7 300/400, Step 7 Version, not yet implemented */
		MPI4,				/* MPI for S7 300/400, "Andrew's version" with STX */
		PPI = 10,			/* PPI for S7 200 */
		AS511 =	20,			/* S5 programming port protocol */
		S7online = 50,		/* use s7onlinx.dll for transport */
		ISOTCP = 122,		/* ISO over TCP */
		ISOTCP243 = 123,	/* ISO over TCP with CP243 */
		MPI_IBH = 223,		/* MPI with IBH NetLink MPI to ethernet gateway */
		PPI_IBH = 224,		/* PPI with IBH NetLink PPI to ethernet gateway */
		NLpro = 230,		/* MPI with NetLink Pro MPI to ethernet gateway */
		UserTransport = 255 /* Libnodave will pass the PDUs of S7 Communication to user defined call back functions. */
	};

	enum class DaveSpeed {
		SPEED_9K,
		SPEED_19K,
		SPEED_187K,
		SPEED_500K,
		SPEED_1500K,
		SPEED_45K,
		SPEED_93K
	};

	enum class DaveMemArea {
		PERIPHERALS = 0x80,    		/* direct peripheral access */
		INPUTS = 0x81,
		OUTPUTS = 0x82,
		FLAGS = 0x83,
		DATA_BLOCKS = 0x84,		/* data blocks */
		DATA_INSTANCE = 0x85,	/* instance data blocks */
		COUNTERS = 28,			/* S7 counters */
		TIMERS = 29,			/* S7 timers */
		COUNTER_200 = 30,		/* IEC counters (200 family) */
		TIMER_200 = 31,			/* IEC timers (200 family) */
		SYS_INFO = 0x3,			/* System info of 200 family */
		SYS_FLAGS = 0x5,		/* System flags of 200 family */
		ANALOG_IN = 0x6,		/* analog inputs of 200 family */
		ANALOG_OUT = 0x7		/* analog outputs of 200 family */
	};

	enum class ConnType {
		TCP,
		NETLINK,
		SERIAL
	};

	PLC(std::string name, DaveProtocol dave_proto = DaveProtocol::ISOTCP,
		int rack = 0, int slot = 0, DaveSpeed dave_speed = DaveSpeed::SPEED_187K);
	PLC(std::string name, DaveProtocol dave_proto, int local_mpi_addr = 0, int plc_mpi_addr = 2,
		int rack = 0, int slot = 0, DaveSpeed dave_speed = DaveSpeed::SPEED_187K);
	~PLC();

	bool connect_tcp(std::string ipaddr, int port = 102);
	bool connect_netlink(std::string ipaddr, int port = 1099);
	bool connect_serial(char *port, char *baud = "38400", char parity = '0');

	void disconnect();

	bool read_bytes(DaveMemArea area, int addr_start, int length, uint8_t *buffer);
	bool write_bytes(DaveMemArea area, int addr_start, int length, uint8_t *buffer); // TODO

private:
	std::string _name;
	DaveProtocol _dave_proto;
	int _local_mpi_addr;
	int _plc_mpi_addr;
	int _rack;
	int _slot;
	DaveSpeed _dave_speed;
	
	daveInterface *di;
	daveConnection *dc;
	_daveOSserialType fds;

	ConnType conn_type;

	bool connect();
};