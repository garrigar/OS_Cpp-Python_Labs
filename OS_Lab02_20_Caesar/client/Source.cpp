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

// �������� �������, �������������� ����� �� ������
void CaesarShiftEN(char* text, int offs);

int main()
{

	printf("CAESAR CIPHER SOCKET APPLICATION\n");
	printf("CLIENT\n");
	printf("Client recieves messages from standard input, encrypts and sends them to the server.\n");
	printf("One message can contain up to %d characters. ", BUFFER_SIZE - 2);
	printf("Otherwise it will be splitted to blocks with %d characters in each.\n", BUFFER_SIZE - 1);
	printf("WARNING: Only latin letters will be encrypted.\n\n");

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

	printf("Successfully connected to %s\n", SERVERADDR);
	printf("Type \"quit\" for quit\n\n");

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

	printf("Client's ID on server: %d\n\n", id);

	// ���������� �����
	char buff[BUFFER_SIZE];

	while (true)
	{
		// ������ ���������������� ���� � ����������
		printf(">> ");
		fgets(buff, sizeof(buff), stdin);

		// �������� �� "quit"
		if (!strcmp(buff, "quit\n"))
		{
			// ���������� �����
			printf("Exiting...");
			closesocket(my_sock);
			WSACleanup();
			return 0;
		}

		// ����� ������ �� id ��� ����������
		int shift = id;
		// ���� ����� �� "����������" �����������, ������, ������� ������ �� 3 �� ���������
		if (shift % 26 == 0) shift = 3;
		// ������������ ����������
		CaesarShiftEN(buff, shift);
		// ����� �������������� ��������� ��� ������
		//printf("DEBUG - ENCRYPTED: %s\n", buff);

		// �������� ������ ������� �������
		send(my_sock, buff, sizeof(buff), 0);
	}

	// ��� ���-�������
	/*
	int nsize;
	while ((nsize = recv(my_sock, buff, sizeof(buff), 0)) != SOCKET_ERROR)
	{
		// ������ ����������� ���� � ����� ������
		buff[nsize] = 0;

		// ������� �� �����
		printf("S => C: %s", buff);

		// ������ ���������������� ���� � ����������
		printf("S <= C: ");
		fgets(buff, sizeof(buff), stdin);

		// �������� �� "quit"
		if (!strcmp(buff, "quit\n"))
		{
			// ���������� �����
			printf("Exit...");
			closesocket(my_sock);
			WSACleanup();
			return 0;
		}

		// �������� ������ ������� �������
		send(my_sock, buff, sizeof(buff) - 1, 0);
	}

	printf("Recv error: %d\n", WSAGetLastError());
	closesocket(my_sock);
	WSACleanup();
	system("pause");
	return -1;
	*/
}

// �������, �������������� ����� �� ������ ��������� ���� �� text �� offs ������ (����� �� -offs ��� offs < 0)
void CaesarShiftEN(char* text, int offs)
{
	// ����������� �� ������ ������������� ���������� ������� �� 26
	// ������ �� � ������ �� -25 �� +25
	offs %= 26;
	// �������� ����� ����� (-25..-1) ������� ������
	if (offs < 0)
		offs += 26;
	// ����� �������, �� ������� � ������ ������ �� �������� �� 0 �� 25

	int i = 0;
	// ���� �� �������� ���������� ������� � ������
	while (text[i] != 0)
	{
		// ������������ ������ ��������� �����
		if ((text[i] > 0) && isalpha(text[i]))
		{
			// ���� ��� ������ ������ �� ������ �� ��������� �����
			if ((((int)text[i]) + offs) > ((int)(isupper(text[i]) ? 'Z' : 'z')))
				// �� ������� ������������� ����� �����
				text[i] += (offs - 26);
			else
				// ����� ������ ���� ����� ������
				text[i] += offs;
		}
		i++;
	}
}
