SRC_FILES =  src/main_ws.cpp
WS_SRC_FILES = src/ws/stdsoap2.cpp src/ws/soapServer.cpp src/ws/soapC.cpp
WS_FILES = src/ws/soapC.cpp src/ws/soapH.h src/ws/soapWS_strManagerObject.h src/ws/soapServer.cpp src/ws/soapServerLib.cpp src/ws/soapStub.h src/ws/*.xml src/ws/*.nsmap src/ws/*.wsdl src/ws/*.xsd
WS_PATH = src/ws
WS_INTERFACE = web_service.h
LIBS = -llog4cxx -lgsoap++ -lstrmanager
SOAP_BIN = soapcpp2
INCLUDES = 
BIN=bin/WS_strManager

all:
	$(CXX) -Wall -O3 -o $(BIN) $(LIBS) $(INCLUDES) $(SRC_FILES) $(WS_SRC_FILES) 
	
clean:
	rm -fv bin/*

ws:
	 $(SOAP_BIN) -S -x $(WS_PATH)/$(WS_INTERFACE) -d$(WS_PATH)
	
cleanws:
	rm -fv $(WS_FILES)
