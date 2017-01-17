#include "B4RDefines.h"
namespace B4R {
	B4RCuriePeripheral::B4RCuriePeripheral() : per(),connected(false),
		mainService	("0001"),
		readChar("1001", BLERead | BLENotify, BLE_MAX_ATTR_DATA_LEN),
		writeChar("1002", BLEWrite, BLE_MAX_ATTR_DATA_LEN) {
	}
	void B4RCuriePeripheral::Initialize(SubVoidStringBool ConnectStateSub, SubVoidArray NewDataSub) {
		per.setAdvertisedServiceUuid(mainService.uuid());
		per.addAttribute(mainService);
		per.addAttribute(readChar);
		per.addAttribute(writeChar);
		this->ConnectState = ConnectStateSub;
		this->NewDataSub = NewDataSub;
	}
	void B4RCuriePeripheral::Start(B4RString* Name) {
		per.setLocalName(Name->data);
		per.begin();
		FunctionUnion fu;
		fu.PollerFunction = bleLoop;
		pnode.functionUnion = fu;
		pnode.tag = this;
		if (pnode.next == NULL) {
			pollers.add(&pnode);
		}
	}
	bool B4RCuriePeripheral::Write(ArrayByte* Data) {
		if (per.connected()) {
			return readChar.setValue((const unsigned char*)Data->data, Data->length);
		}
		return false;
	}
	//static
	void B4RCuriePeripheral::bleLoop(void* b) {
		B4RCuriePeripheral* INSTANCE = (B4RCuriePeripheral*)b;
		BLECentral central = INSTANCE->per.central();
		const UInt cp = B4R::StackMemory::cp;
		if (INSTANCE->connected != (bool)central) {
			INSTANCE->connected = (bool)central;
			B4RString* address = B4RString::EMPTY;
			if (INSTANCE->connected) {
				address = CreateStackMemoryObject(B4RString);
				address->data = central.address();
			}
			INSTANCE->ConnectState (address, INSTANCE->connected);
		}
		if (INSTANCE->connected) {
			if (INSTANCE->writeChar.written()) {
				int length = INSTANCE->writeChar.valueSize();
				
				ArrayByte* arr = CreateStackMemoryObject(ArrayByte);
				arr->data = StackMemory::buffer + StackMemory::cp;
				for (short i = 0;i < length;i++)
					((Byte*)arr->data)[i] = INSTANCE->writeChar[i];
				B4R::StackMemory::cp += length;
				arr->length = length;
				INSTANCE->NewDataSub(arr);
				
			}
		}
		B4R::StackMemory::cp = cp;
	
		
	}
	void B4RCuriePeripheral::Stop() {
		per.end();
		if (pnode.next != NULL)
			pollers.remove(&pnode);
	}
	
}