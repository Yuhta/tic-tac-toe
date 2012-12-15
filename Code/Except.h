#pragma once

#include <windows.h>

class EXCEPTION {
	public:
		EXCEPTION( LPCWSTR msg = L""): message( msg ) {}
		LPCWSTR GetMsg() const { return message; }
	private:
		LPCWSTR message;
};