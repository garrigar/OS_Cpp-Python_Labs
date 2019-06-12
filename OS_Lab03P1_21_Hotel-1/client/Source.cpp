// ������, ����������� ������ ���������� WinSock
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <winsock2.h> // winsock2.h ������ ���� ��������� ������ windows.h
#include <windows.h>

// ������ ������ ��������
#define BUFFER_SIZE 1024

// ���� ��� �����������
#define PORT 666
// ����� ��� �����������
#define SERVERADDR "127.0.0.1"

int main()
{
	// ��� 1 - ������������� ���������� Winsock
	WSADATA ws;
	if (WSAStartup(0x202, &ws))
	{
		printf("WSAStartup error: %d\n", WSAGetLastError());
		system("pause");
		return -1;
	}

	// ��� 2 - �������� ������
	SOCKET my_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (my_sock < 0)
	{
		printf("Error creating socket: %d\n", WSAGetLastError());
		WSACleanup();
		system("pause");
		return -1;
	}

	// ��� 3 - ��������� ����������

	// ���������� ��������� sockaddr_in
	// �������� ������ � ����� �������
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(PORT);

	HOSTENT *hst;

	// �������������� IP ������ �� ����������� � ������� ������
	if (inet_addr(SERVERADDR) != INADDR_NONE)
		dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
	else
		// ������� �������� IP ����� �� ��������� ����� �������
		if (hst = gethostbyname(SERVERADDR))
			// hst->h_addr_list �������� �� ������ �������, � ������ ���������� �� ������
			((unsigned long *)&dest_addr.sin_addr)[0] =
			((unsigned long **)hst->h_addr_list)[0][0];
		else
		{
			printf("Invalid address: %s\n", SERVERADDR);
			closesocket(my_sock);
			WSACleanup();
			system("pause");
			return -1;
		}

	// ����� ������� ������� � �������� ���������� ���������� 
	if (connect(my_sock, (sockaddr *)&dest_addr, sizeof(dest_addr)))
	{
		printf("Error connecting: %d\n", WSAGetLastError());
		closesocket(my_sock);
		WSACleanup();
		system("pause");
		return -1;
	}

	/*printf("Successfully connected to %s\n", SERVERADDR);
	printf("Type \"quit\" for quit\n\n");*/
	printf("Successfully reached the HILBERT'S HOTEL JR. building\n");

	// ��� 4 - ������ � �������� ���������

	// id_char ����� ��������� id ��� ������ �� 4 char
	char id_char[4];
	// �������� id
	if (recv(my_sock, id_char, sizeof(id_char), 0) == SOCKET_ERROR) {
		// recv ������� ������
		printf("Recv error: %d\n", WSAGetLastError());
		closesocket(my_sock);
		WSACleanup();
		system("pause");
		return -1;
	}
	// ��������� id
	int id = *((int *)id_char);

	printf("Person's ID: %d\n\n", id);

	// ���������� �����
	char buff[BUFFER_SIZE];

	// ������ ���������������� ���� � ����������
	printf("Name: ");
	fgets(buff, sizeof(buff), stdin);
	// delete trailing '\n'
	int sl = strlen(buff) - 1;
	if (buff[sl] == '\n') buff[sl] = 0;
	// �������� ��� ������� �������
	send(my_sock, buff, sizeof(buff), 0);

	// customize duration
	// ----------------------------------------------------------------------------------------------
	/*
	printf("Wished duration: ");
	fgets(buff, sizeof(buff), stdin);
	// delete trailing '\n'
	sl = strlen(buff) - 1;
	if (buff[sl] == '\n') buff[sl] = 0;
	//int dur = ;
	while (atoi(buff) <= 0) {
		// input again
		printf("Invalid duration, try again: ");
		fgets(buff, sizeof(buff), stdin);
		// delete trailing '\n'
		sl = strlen(buff) - 1;
		if (buff[sl] == '\n') buff[sl] = 0;
	}
	int dur = atoi(buff);
	*/
	int dur = 1;

	char* dur_char = (char *)&dur;
	send(my_sock, dur_char, 4, 0);
	// ----------------------------------------------------------------------------------------------

	printf("Waiting for room...\n");

	// i - room index
	char i_char[4];
	// try to get i, waiting for it to arrive, if so, we continue
	if (recv(my_sock, i_char, sizeof(i_char), 0) == SOCKET_ERROR) {
		// recv ������� ������
		printf("Recv error: %d\n", WSAGetLastError());
		closesocket(my_sock);
		WSACleanup();
		system("pause");
		return -1;
	}
	// ��������� i
	int i = *((int *)i_char);

	printf("Checked in into room %d for %d day(s)\n", i, dur);

	// waiting for leave message, when it comes, we leave
	if (recv(my_sock, buff, sizeof(buff), 0) == SOCKET_ERROR) {
		// recv ������� ������
		printf("Recv error: %d\n", WSAGetLastError());
		closesocket(my_sock);
		WSACleanup();
		system("pause");
		return -1;
	}
	printf("Checked out from room %d\n", i);
	printf("Bye!\n");

	// ���������� �����
	printf("Exiting...\n");
	closesocket(my_sock);
	WSACleanup();

	//system("pause");
	return 0;
	
}