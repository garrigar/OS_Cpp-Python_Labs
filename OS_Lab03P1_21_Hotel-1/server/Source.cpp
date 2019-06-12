// ������, ����������� ������ ���������� WinSock
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h>	// winsock2.h ������ ���� ��������� ������ windows.h
#include <windows.h>
#include <process.h>	// ��� _beginthreadex 

#include <vector>
#include <string>
#include <conio.h>		// for _getch()

// ������ ������ ��������
#define BUFFER_SIZE 1024

// ����, ������� ������� ������
#define MY_PORT 666

// ������ ��� ������ ���������� �������� ������������� 
#define PRINTNUSERS if (nclients)\
  printf("%d user(s) online\n", nclients);\
  else printf("No users online\n");

// number of rooms in the hotel
#define ROOM_COUNT 3

// ���������, ����� ������� ����� ������������ ��������� � ServeClient
typedef struct
{
	SOCKET * client_socket; // ����� �������
	int client_id;			// ID �������
} serve_client_params_t;

// �������� �������, ������������� �������������� �������������
unsigned __stdcall ServeClient(void* params);

// timeline function prototype
unsigned __stdcall Timeline(void* params);

// ���������� �������� ������������� 
int nclients = 0;

int current_time = 0;

HANDLE semaphore; // semaphore to access the building
HANDLE mutex; // mutex to access the reception

std::vector<bool> occupied; // whether a room is occupied or not
std::vector<std::string> names; // names of every person in room

int main()
{
	// vectors init
	occupied = std::vector<bool>(ROOM_COUNT);
	names = std::vector<std::string>(ROOM_COUNT);
	// ---------------------------------------------------------------

	printf("HILBERT'S HOTEL JR.\n");
	
	// ��� 1 - ������������� ���������� �������
	WSADATA ws;
	if (WSAStartup(0x0202, &ws))
	{
		// ������!
		printf("Error WSAStartup: %d\n", WSAGetLastError());
		system("pause");
		return -1;
	}

	// ��� 2 - �������� ������
	SOCKET mysocket;
	// AF_INET - ����� ���������
	// SOCK_STREAM - ��������� ����� (� ���������� ����������)
	// 0 - �� ��������� ���������� TCP ��������
	if ((mysocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		// ������!
		printf("Error creating socket: %d\n", WSAGetLastError());
		// �������������� ���������� Winsock
		WSACleanup();
		system("pause");
		return -1;
	}

	// ��� 3 - ���������� ������ � ��������� �������
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(MY_PORT);	// �� �������� � ������� �������
	local_addr.sin_addr.s_addr = 0;			// ������ ��������� ����������� �� ��� IP-������

	// �������� bind ��� ����������
	if (bind(mysocket, (sockaddr *)&local_addr, sizeof(local_addr)))
	{
		// ������
		printf("Error binding: %d\n", WSAGetLastError());
		closesocket(mysocket);  // ��������� �����
		WSACleanup();
		system("pause");
		return -1;
	}

	// ��� 4 - �������� �����������
	// ������ ������� � 0x100
	if (listen(mysocket, 0x100))
	{
		// ������
		printf("Error listening: %d\n", WSAGetLastError());
		closesocket(mysocket);
		WSACleanup();
		system("pause");
		return -1;
	}

	printf("...Awaiting visitors...\n\n");

	// ��� 5 - ��������� ��������� �� �������
	SOCKET client_socket;		// ����� ��� �������
	sockaddr_in client_addr;    // ����� ������� (����������� ��������)

	// ������� accept ���������� �������� ������ ���������
	int client_addr_size = sizeof(client_addr);

	// ID ���������� �������
	int idclients = 0;
	semaphore = CreateSemaphore(NULL, ROOM_COUNT, ROOM_COUNT, NULL);
	mutex = CreateMutex(NULL, FALSE, NULL);
	// start the time counter
	_beginthreadex(NULL, 0, Timeline, NULL, 0, NULL);

	// ���� ���������� �������� �� ����������� �� �������
	while ((client_socket = accept(mysocket, (sockaddr *)&client_addr, &client_addr_size)))
	{
		nclients++;      // ����������� ������� �������������� ��������

		// �������� �������� ��� �����
		HOSTENT* hst = gethostbyaddr((char *)&client_addr.sin_addr.s_addr, 4, AF_INET);

		// ����� �������� � �������
		/*printf("+%s [%s] - CONNECTED - ID #%d\n",
			(hst) ? hst->h_name : "",
			inet_ntoa(client_addr.sin_addr),
			idclients);*/
		//PRINTNUSERS;

		// �������� ��������� ��� ������� ServeClient
		serve_client_params_t params;
		params.client_socket = &client_socket;
		params.client_id = idclients++;

		// ����� ������ ������ ��� ����������� �������
		_beginthreadex(NULL, 0, ServeClient, &params, 0, NULL);

	}
	// ������ �����, ������������ �������
	// ���� �� ���, �� ������?
	CloseHandle(semaphore);
	CloseHandle(mutex);
	printf("Unknown error: %d\n", WSAGetLastError());
	closesocket(mysocket);
	WSACleanup();
	system("pause");
	return 0;
}

// ��� ������� ��������� � ��������� ������ � ���������� ���������� ��������������� ������� ���������� �� ���������
unsigned __stdcall ServeClient(void* params)
{
	// ������� ���������
	serve_client_params_t * ptr = (serve_client_params_t *)params;
	SOCKET my_sock = *(ptr->client_socket); // ����� �������
	int id = ptr->client_id;				// ID �������

	// id_char ��������� �� id ��� �� ������ �� 4 char
	char* id_char = (char *)&id;
	// �������� ��� �������
	send(my_sock, id_char, 4, 0);

	// ���������� �����
	char buff[BUFFER_SIZE];

	// �������� ��� �������
	if (recv(my_sock, buff, sizeof(buff), 0) == SOCKET_ERROR) {
		// recv ������� ������
		printf("Recv error: %d\n", WSAGetLastError());
		closesocket(my_sock);
		WSACleanup();
		system("pause");
		return -1;
	}

	// how many days will a person be visitor
	int duration;
	// -------------------------------------------------------------------------------------------------
	char dur_char[4];
	if (recv(my_sock, dur_char, sizeof(dur_char), 0) == SOCKET_ERROR) {
		// recv ������� ������
		printf("Recv error: %d\n", WSAGetLastError());
		closesocket(my_sock);
		WSACleanup();
		system("pause");
		return -1;
	}
	duration = *((int *)dur_char);
	// -------------------------------------------------------------------------------------------------

	// wait for free rooms
	WaitForSingleObject(semaphore, INFINITE);
	// access granted, entered into building

	int startTime = current_time;

	// check in procedure
	WaitForSingleObject(mutex, INFINITE);
	int i; // given room
	for (i = 0; i < ROOM_COUNT; i++)
	{
		if (!occupied[i]) {
			occupied[i] = true;
			names[i] = std::string(buff);

			send(my_sock, (char *)&i, 4, 0);

			break;
		}
	}
	printf("Person %s (id = %d) checked in into room %d for %d day(s)\n", buff, id, i, duration);
	ReleaseMutex(mutex);
	// check in ends

	// sleep on soft bed
	while (current_time < (startTime + duration)) {
		Sleep(5);
	}

	// check out procedure
	WaitForSingleObject(mutex, INFINITE);
	occupied[i] = false;
	names[i] = "";
	const char * goaway = "leave";
	send(my_sock, goaway, sizeof(goaway), 0);
	printf("Person %s (id = %d) checked out from room %d\n", buff, id, i);
	ReleaseMutex(mutex);
	// check out ends

	ReleaseSemaphore(semaphore, 1, NULL);
	// leave the building

	nclients--; // ��������� ������� �������� ��������
	//printf("CLIENT #%d - DISCONNECTED\n", id);
	//PRINTNUSERS;

	// ��������� �����
	closesocket(my_sock);

	return 0;
}

// ...the clock ticks life away...
unsigned __stdcall Timeline(void* params) {
	printf("(Press any key at any moment you want to go to the next day)\n");
	while (true) {
		// printing rooms state
		printf("\nAt beginning of day %d:\n", current_time);
		WaitForSingleObject(mutex, INFINITE);
		for (int i = 0; i < ROOM_COUNT; i++)
		{
			printf("Room %d: %s\n", i, (occupied[i] ? names[i].c_str() : "FREE"));
		}
		printf("\n");
		ReleaseMutex(mutex);
		
		printf("That's what's happening after:\n");
		
		// when any key is pressed, we go to the next day
		_getch();
		current_time++;
	}
}