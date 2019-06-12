// ������, ����������� ������ ���������� WinSock
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h>	// winsock2.h ������ ���� ��������� ������ windows.h
#include <windows.h>
#include <process.h>	// ��� _beginthreadex 

// ������ ������ ��������
#define BUFFER_SIZE 1024

// ����, ������� ������� ������
#define MY_PORT 666

// ������ ��� ������ ���������� �������� ������������� 
#define PRINTNUSERS if (nclients)\
  printf("%d user(s) online\n", nclients);\
  else printf("No users online\n");

// ���������, ����� ������� ����� ������������ ��������� � ServeClient
typedef struct
{
	SOCKET * client_socket; // ����� �������
	int client_id;			// ID �������
} serve_client_params_t;

// �������� �������, ������������� �������������� �������������
unsigned __stdcall ServeClient(void* params);

// �������� �������, �������������� ����� �� ������
void CaesarShiftEN(char* text, int offs);

// ���������� �������� ������������� 
int nclients = 0;

int main()
{

	printf("CAESAR CIPHER SOCKET APPLICATION\n");
	printf("SERVER\n");
	printf("This server recieves encrypted messages from its clients, decrypts and shows them.\n");
	printf("WARNING: Only latin letters will be decrypted.\n\n");

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

	printf("...Awaiting connections...\n\n");

	// ��� 5 - ��������� ��������� �� �������
	SOCKET client_socket;		// ����� ��� �������
	sockaddr_in client_addr;    // ����� ������� (����������� ��������)

	// ������� accept ���������� �������� ������ ���������
	int client_addr_size = sizeof(client_addr);

	// ID ���������� �������
	int idclients = 0;

	// ���� ���������� �������� �� ����������� �� �������
	while ((client_socket = accept(mysocket, (sockaddr *)&client_addr, &client_addr_size)))
	{
		nclients++;      // ����������� ������� �������������� ��������

		// �������� �������� ��� �����
		HOSTENT* hst = gethostbyaddr((char *)&client_addr.sin_addr.s_addr, 4, AF_INET);

		// ����� �������� � �������
		printf("+%s [%s] - CONNECTED - ID #%d\n",
			(hst) ? hst->h_name : "",
			inet_ntoa(client_addr.sin_addr),
			idclients);

		PRINTNUSERS;

		// �������� ��������� ��� ������� ServeClient
		serve_client_params_t params;
		params.client_socket = &client_socket;
		params.client_id = idclients++;

		// ����� ������ ������ ��� ����������� �������
		_beginthreadex(NULL, 0, ServeClient, &params, 0, NULL);

	}
	// ������ �����, ������������ �������
	// ���� �� ���, �� ������?
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

	// ���� �������: ����� ������ �� �������, ������������ � ����� �� �����
	int bytes_recv;
	while ((bytes_recv = recv(my_sock, buff, sizeof(buff), 0))
		&& bytes_recv != SOCKET_ERROR)
	{
		// ����� ����� �� id ��� ����������
		int shift = -id;
		// ���� ����� �� "����������" �����������, ������, ������� ����� �� 3 �� ���������
		if (shift % 26 == 0) shift = -3;
		// ����� �������������� ��������� ��� ������
		//printf("\nDEBUG - ENCRYPTED: %s", buff);
		// ������������ ����������
		CaesarShiftEN(buff, shift);

		// ����� �� �����
		printf(" <client #%d:> %s", id, buff);

		// ��� ���-�������
		//send(my_sock, buff, bytes_recv, 0);
	}

	// ���� �� �����, �� ��������� ����� �� ����� �� ������� ���������� �������� recv ������ �
	// ���������� � �������� ���������
	nclients--; // ��������� ������� �������� ��������
	printf("CLIENT #%d - DISCONNECTED\n", id);
	PRINTNUSERS;

	// ��������� �����
	closesocket(my_sock);

	return 0;
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
