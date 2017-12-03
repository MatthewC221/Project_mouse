#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <string.h>
#include <windows.h>
#include <iostream>
#include <chrono>
#include <thread>
#include "wtypes.h"

#define BUF_SIZE 256

int SCREEN_WIDTH = 0;				// First function will get these values
int SCREEN_HEIGHT = 0;

INPUT buffer;						// Mouse input object
HANDLE hComm;						// For serial comms
DCB dcbSerialParams;
COMMTIMEOUTS timeouts;
unsigned int current_x;				// Current positions that will change
unsigned int current_y;

// Averages are used in the following way: for the first 10 readings, we create the average origin. This acceleration
// will be subtracted from future ones. REQUIRES calibration
int average_x;
int average_y;

using namespace std;

void parseData(char accel[], int size, int total);
bool isNumber(char tmp);
void MouseMoveAbsolute(INPUT *buffer, int x, int y);

void MouseSetup(INPUT *buffer)
{
	buffer->type = INPUT_MOUSE;
	buffer->mi.dx = (0 * (0xFFFF / SCREEN_WIDTH));	// Absolute pos of mouse x
	buffer->mi.dy = (0 * (0xFFFF / SCREEN_HEIGHT));	// Absolute pos of mouse y
	buffer->mi.mouseData = 0;						// Wheel movement (up, down)
	buffer->mi.dwFlags = MOUSEEVENTF_ABSOLUTE;
	buffer->mi.time = 0;							// Time stamp for event
	buffer->mi.dwExtraInfo = 0;
}

// Create the connection to COM and initialise values
bool createConnection()
{
	hComm = CreateFile("COM3",					// Use this \\\\.\\COM10
		GENERIC_READ | GENERIC_WRITE,			//Read/Write
		0,										// No Sharing
		NULL,									// No Security
		OPEN_EXISTING,							// Open existing port only
		0,										// Non Overlapped I/O
		NULL);

	if (hComm == INVALID_HANDLE_VALUE) {
		printf("Invalid handle\n");
		const DWORD error = GetLastError();
		printf("Error = %d\n", error);
		CloseHandle(hComm);
		return false;
	} else {

		// Use the bool status to test all of the setup functions
		bool status;

		status = GetCommState(hComm, &dcbSerialParams);
		if (!status) {
			printf("Failed to get CommState\n");
			return false;
		}

		dcbSerialParams = { 0 };
		dcbSerialParams.BaudRate = 9600;		// Setting BaudRate = 9600
		dcbSerialParams.ByteSize = 8;			// Setting ByteSize = 8
		dcbSerialParams.StopBits = ONESTOPBIT;	// Setting StopBits = 1
		dcbSerialParams.Parity = NOPARITY;		// Setting Parity = None

		timeouts = { 0 };
		timeouts.ReadIntervalTimeout = 50;			// Changes interval of reads
		timeouts.ReadTotalTimeoutConstant = 50;
		timeouts.ReadTotalTimeoutMultiplier = 50;
		timeouts.WriteTotalTimeoutConstant = 50;
		timeouts.WriteTotalTimeoutMultiplier = 50;

		status = SetCommState(hComm, &dcbSerialParams);

		if (!status) {
			printf("Failed to set DCB structure\n");
			return false;
		} else {
			printf("Setting DCB Structure Successfull\n");
			printf("\n       Baudrate = %d", dcbSerialParams.BaudRate);
			printf("\n       ByteSize = %d", dcbSerialParams.ByteSize);
			printf("\n       StopBits = %d", dcbSerialParams.StopBits);
			printf("\n       Parity   = %d", dcbSerialParams.Parity);
		}

		status = SetCommTimeouts(hComm, &timeouts);
		if (!status) {
			printf("Failed to set timeouts\n");
			return false;
		}

		status = SetCommMask(hComm, EV_RXCHAR);
		if (!status) {
			printf("Failed to set CommMask\n");
			return false;
		}

		DWORD dwEventMask;
		status = WaitCommEvent(hComm, &dwEventMask, NULL);

		// End of random setup...

		if (!status) {
			printf("Not receiving!\n");
			return false;
		} else {
			printf("\nReady to receive\n");
			int i = 0;
			int total = 0;						// Total reads
			char temp_char;
			DWORD none_read;					// DWORD is equivalent to unsigned int
			char data[BUF_SIZE];				// The data of the accelerations
			bool current_read = false;

			do {
				status = ReadFile(hComm, &temp_char, sizeof(temp_char), &none_read, NULL);

				// Code for reading data values, current_read is used so we don't read half a string
				// e.g. the full string is (102, 105). If we receive 2, 105), we will disregard it
				if (current_read) data[i++] = temp_char;

				if (temp_char == '(') {
					current_read = true;
				}
				else if (temp_char == ')') {
					if (current_read) {
						parseData(data, i - 1, total);
					}
					i = 0;
					current_read = false;
					total++;
				}

				if (total == 10000) break;			// After 10000 reads of the string, break

			} while (none_read > 0);

		}
		CloseHandle(hComm);
		return true;
	}
}

// Parse the data, format of data: X, Y
void parseData(char accel[], int size, int total)
{
	int x_move = 0;
	int y_move = 0;

	/* To print the data
	for (int i = 0; i < size; i++) {
	printf("%c", accel[i]);
	}
	printf("\n");
	*/

	// This part below parses the string and fills up x_move and y_move with the accel.
	bool x_flag = true;
	for (int i = 0; i < BUF_SIZE; i++) {
		if (x_flag && isNumber(accel[i])) {
			x_move = (x_move * 10) + int(accel[i] - '0');
		}
		else if (!x_flag && isNumber(accel[i])) {
			y_move = (y_move * 10) + int(accel[i] - '0');
		}
		else if (accel[i] == ',') {
			x_flag = false;
		}
	}
	
	// Don't worry about this stuff, the calibrations are really poor
	if (total > 10) {
		int dest_x = current_x + (x_move - average_x) / 50;
		int dest_y = current_y + (y_move - average_y) / 50;
		MouseMoveAbsolute(&buffer, dest_x, dest_y);
	}
	else {
		average_x += (x_move / 10);
		average_y += (y_move / 10);
	}

	/* More calculations
	cur_x = cur_x + x_move, cur_y = cur_y + y_move
	MouseMoveAbsolute(&buffer, cur_x, cur_y);

	if (!x_move) x_move = x_move * -1;
	if (!y_move) y_move = y_move * -1;

	while (x_move > SCREEN_WIDTH) x_move -= SCREEN_WIDTH;
	while (y_move > SCREEN_HEIGHT) y_move -= SCREEN_HEIGHT;

	if (total > 10) {
	current_x = current_x + x_move - average_y;
	current_y = current_y + y_move - average_x;
	MouseMoveAbsolute(&buffer, current_x, current_y);
	}
	else {			// Set the average, calibrating
	average_x += (x_move / 10);
	average_y += (y_move / 10);
	}
	*/
}

bool isNumber(char tmp_char)
{
	int tmp = (int)tmp_char;
	return (tmp >= 48 && tmp <= 57);
}
// Get the horizontal and vertical screen sizes in pixels
void GetDesktopResolution(int &horizontal, int &vertical)
{
	RECT desktop;
	// Get a handle to the desktop window
	const HWND hDesktop = GetDesktopWindow();
	// Get the size of screen to the variable desktop
	GetWindowRect(hDesktop, &desktop);
	// The top left corner will have coordinates (0,0)
	// and the bottom right corner will have coordinates
	// (horizontal, vertical)
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

// The function that moves the mouse
void MouseMoveAbsolute(INPUT *buffer, int x, int y)
{
	buffer->mi.dx = (x * (0xFFFF / SCREEN_WIDTH));
	buffer->mi.dy = (y * (0xFFFF / SCREEN_HEIGHT));
	buffer->mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE);

	SendInput(1, buffer, sizeof(INPUT));
}

// Mouse clicker
void MouseClick(INPUT *buffer)
{
	// Initial click
	buffer->mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN);
	SendInput(1, buffer, sizeof(INPUT));

	Sleep(10);

	// Let go of click
	buffer->mi.dwFlags = (MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP);
	SendInput(1, buffer, sizeof(INPUT));
}

int main(int argc, char *argv[])
{

	// Initially get the resolution
	GetDesktopResolution(SCREEN_WIDTH, SCREEN_HEIGHT);
	cout << SCREEN_WIDTH << '\n' << SCREEN_HEIGHT << '\n';

	// Origin is in the centre of the screen
	current_x = SCREEN_WIDTH / 2;
	current_y = SCREEN_HEIGHT / 2;

	MouseSetup(&buffer);

	// Gets min of screen width and screen height
	int min = SCREEN_WIDTH;
	if (min > SCREEN_HEIGHT) min = SCREEN_HEIGHT;

	cout << "Setting up... setting mouse to origin\n";
	MouseMoveAbsolute(&buffer, current_x, current_y);
	createConnection();

	/* Random movement, with delays
	for (int i = 0; i < min; i++) {
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	MouseMoveAbsolute(&buffer, i, i);
	}
	*/


	// MouseClick(&buffer); // Clicks the current location

	return 0;
}
