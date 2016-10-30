#include "SerialClass.h"

Serial::Serial() {};

DWORD Serial::connectToPort(char *_portName, int baudRate) {
	error = ERROR_SUCCESS;
	//We're not yet connected
	connected = false;

	//Try to connect to the given port throuh CreateFile
	hSerial = CreateFile(_portName,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	//Check if the connection was successfull
	if (hSerial == INVALID_HANDLE_VALUE) {
		error = GetLastError();
		return error;
	} else {
		//If connected we try to set the comm parameters
		DCB dcbSerialParams = { 0 };

		//Try to get the current
		if (!GetCommState(hSerial, &dcbSerialParams)) {
			//If impossible, show an error
			error = GetLastError();
			return error;
		} else {
			//Define serial connection parameters for the arduino board
			dcbSerialParams.BaudRate = baudRate;
			dcbSerialParams.ByteSize = 8;
			dcbSerialParams.StopBits = ONESTOPBIT;
			dcbSerialParams.Parity = NOPARITY;
			//Setting the DTR to Control_Enable ensures that the Arduino is properly
			//reset upon establishing a connection
			dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;

			//Set the parameters and check for their proper application
			if (!SetCommState(hSerial, &dcbSerialParams)) {
				//printf("ALERT: Could not set Serial Port parameters");
				error = GetLastError();
				return error;
			} else {
				//If everything went fine we're connected
				connected = true;
				//Flush any remaining characters in the buffers 
				PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR);
				//We wait 2s as the arduino board will be reseting
				Sleep(ARDUINO_WAIT_TIME);
				return ERROR_SUCCESS;
			}
		}
	}
	//Save the _portName
	strncpy_s(portName, 6, _portName, sizeof(_portName) / sizeof(char));
}

Serial::~Serial() {
	//Check if we are connected before trying to disconnect
	if (connected) {
		//We're no longer connected
		connected = false;
		//Close the serial handler
		CloseHandle(hSerial);
	}
}

int Serial::ReadData(char *buffer, unsigned int nbChar) {
	//Number of bytes we'll have read
	DWORD bytesRead;
	//Number of bytes we'll really ask to read
	unsigned int toRead;

	//Use the ClearCommError function to get status info on the Serial port
	ClearCommError(hSerial, &error, &status);

	//Check if there is something to read
	if (status.cbInQue>0) {
		//If there is we check if there is enough data to read the required number
		//of characters, if not we'll read only the available characters to prevent
		//locking of the application.
		if (status.cbInQue>nbChar) {
			toRead = nbChar;
		} else {
			toRead = status.cbInQue;
		}
		//Try to read the require number of chars, and return the number of read bytes on success
		if (ReadFile(hSerial, buffer, toRead, &bytesRead, NULL)) {
			return bytesRead;
		}
	}
	//If nothing has been read, or that an error was detected return 0
	return 0;
}


bool Serial::WriteData(char *buffer, unsigned int nbChar) {
	DWORD bytesSend;
	//Try to write the buffer on the Serial port
	if (!WriteFile(hSerial, (void *)buffer, nbChar, &bytesSend, 0)) {
		//In case it don't work get comm error and return false
		ClearCommError(hSerial, &error, &status);
		return false;
	} else {
		return true;
	}
}

bool Serial::IsConnected() {
	//Simply return the connection status
	return connected;
}

std::string Serial::getLine() {
	static std::string buildingString = "";
	std::string result;
	char c;
	while (true) {
		if (ReadData(&c, 1) <= 0) {
			error = GetLastError();
			break;
		};
		if (c == '\r') {
			ReadData(&c, 1);
			result = buildingString;
			buildingString = "";
			return result;
		}
		buildingString += c;
	}
	return "";
};