#include <string>

//gsoap ns service name: WS_strManager
//gsoap ns service location: http://localhost:8888/strManager

typedef char* xsd__string;

//gsoap ns service method-action: command ""
int ns__command(xsd__string* command, bool& result);

//gsoap ns service method-action: addSource ""
int ns__addSource(int* workerType, int* src_id, xsd__string* ip, int* port,
		int& result);

//gsoap ns service method-action: removeSource ""
int ns__removeSource(int* src_id, int& result);

//gsoap ns service method-action: addDestination ""
int ns__addDestination(int* src_id, int* dest_id, xsd__string* ip, int* port,
		int& result);

//gsoap ns service method-action: removeDestination ""
int ns__removeDestination(int* src_id, int* dest_id, int& result);

//gsoap ns service method-action: printManagement ""
int ns__printManagement(xsd__string& result);

//gsoap ns service method-action: printForwarding ""
int ns__printForwarding(xsd__string& result);

//gsoap ns service method-action: printPortFd ""
int ns__printPortFd(xsd__string& result);

