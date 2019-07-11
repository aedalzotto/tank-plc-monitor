#include <PLC.h>
#include <openSocket.h>
#include <setport.h>
#include <cstring>

PLC::PLC(std::string name, DaveProtocol dave_proto, int rack, int slot, DaveSpeed dave_speed) :
			_name(name), _dave_proto(dave_proto), _local_mpi_addr(0), _plc_mpi_addr(2),
			_rack(rack), _slot(slot), _dave_speed(dave_speed), di(nullptr), dc(nullptr)
{
	//daveSetDebug(daveDebugAll);
}

PLC::PLC(std::string name, DaveProtocol dave_proto, int local_mpi_addr, int plc_mpi_addr,
		int rack, int slot, DaveSpeed dave_speed) :
			_name(name), _dave_proto(dave_proto), _local_mpi_addr(local_mpi_addr),
			_plc_mpi_addr(plc_mpi_addr), _rack(rack), _slot(slot), _dave_speed(dave_speed),
			di(nullptr), dc(nullptr)
{
	//daveSetDebug(daveDebugAll);
}

PLC::~PLC()
{
	if(dc)
		daveDisconnectPLC(dc);
	
	if(di)
		daveDisconnectAdapter(di);
}

bool PLC::connect_tcp(std::string ipaddr, int port)
{
	conn_type = ConnType::TCP;
	fds.rfd = openSocket(port, ipaddr.c_str());
	fds.wfd = fds.rfd;

	if(!fds.rfd)
		return false;

	return connect();
}

bool PLC::connect_netlink(std::string ipaddr, int port)
{
	conn_type = ConnType::NETLINK;
	fds.rfd = openSocket(port, ipaddr.c_str());

	if(!fds.rfd)
		return false;

	return connect();
}

bool PLC::connect_serial(char *port, char *baud, char parity)
{
	conn_type = ConnType::SERIAL;
	fds.rfd = setPort(port, baud, parity);

	if(!fds.rfd)
		return false;

	return connect();
}

bool PLC::connect()
{
	char name[256];
	strcpy(name, _name.c_str());
	// std::cout << "new if" << std::endl;
	di = daveNewInterface(fds, name, _local_mpi_addr, (int)_dave_proto, (int)_dave_speed);

	if(!di)
		return false;

	// std::cout << "set to" << std::endl;
	daveSetTimeout(di,5000000);

	// std::cout << "init adapter" << std::endl;
	daveInitAdapter(di);

	// std::cout << "new conn" << std::endl;
	dc = daveNewConnection(di, _plc_mpi_addr, _rack, _slot);

	if(!dc)
		return false;

	// std::cout << "connect plc" << std::endl;
	if(!daveConnectPLC(dc))
		return true;
	else
		return false;
}

void PLC::disconnect()
{
	daveDisconnectPLC(dc);
	daveDisconnectAdapter(di);
	if(conn_type == ConnType::SERIAL){
		closePort(fds.rfd);
	} else {
		closeSocket(fds.rfd);
	}
}

bool PLC::read_bytes(DaveMemArea area, int addr_start, int length, uint8_t *buffer)
{
	return daveReadBytes(dc, (int)area, 0, addr_start, length, (void*)buffer);
}