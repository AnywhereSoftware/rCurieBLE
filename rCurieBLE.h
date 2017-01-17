

#pragma once
#include "B4RDefines.h"
#include <CurieBLE.h>
namespace B4R {
	//~Version: 1.0
	//~shortname: CuriePeripheral
	//~event: ConnectState (Address As String, Connected As Boolean)
	//~Event: NewData (Buffer() As Byte)
	class B4RCuriePeripheral {
		private:
			BLEService mainService;
			BLEPeripheral per;
			BLECharacteristic writeChar;
			BLECharacteristic readChar;
			typedef void (*SubVoidStringBool)(B4RString* string, bool Connected) ;
			SubVoidStringBool ConnectState;
			SubVoidArray NewDataSub;
			PollerNode pnode;
			static void bleLoop(void* b);
			bool connected;
		public:
			//~hide
			B4RCuriePeripheral();
			//Initializes the object and sets the subs that will handle the events.
			void Initialize(SubVoidStringBool ConnectStateSub, SubVoidArray NewDataSub);
			//Starts advertising and listening for connections.
			//The ConnectState event will be raised when there is a new connection.
			void Start(B4RString* Name);
			//Stops the BLE module.
			void Stop();
			//Writes data to the readable characteristic.
			//The connected central will receive a notification.
			//Message size should not exceed 20 bytes.
			bool Write(ArrayByte* Data);
			
	};
}