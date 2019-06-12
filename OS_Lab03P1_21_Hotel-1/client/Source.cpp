// макрос, подавл€ющий ошибки устаревшей WinSock
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <winsock2.h> // winsock2.h должен быть подключен раньше windows.h
#include <windows.h>

// размер буфера символов
#define BUFFER_SIZE 1024

// порт дл€ подключени€
#define PORT 666
// адрес дл€ подключени€
#define SERVERADDR "127.0.0.1"

int main()
{
	// Ўаг 1 - инициализаци€ библиотеки Winsock
	WSADATA ws;
	if (WSAStartup(0x202, &ws))
	{
		printf("WSAStartup error: %d\n", WSAGetLastError());
		system("pause");
		return -1;
	}

	// Ўаг 2 - создание сокета
	SOCKET my_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (my_sock < 0)
	{
		printf("Error creating socket: %d\n", WSAGetLastError());
		WSACleanup();
		system("pause");
		return -1;
	}

	// Ўаг 3 - установка соединени€

	// заполнение структуры sockaddr_in
	// указание адреса и порта сервера
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(PORT);

	HOSTENT *hst;

	// преобразование IP адреса из символьного в сетевой формат
	if (inet_addr(SERVERADDR) != INADDR_NONE)
		dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
	else
		// попытка получить IP адрес по доменному имени сервера
		if (hst = gethostbyname(SERVERADDR))
			// hst->h_addr_list содержит не массив адресов, а массив указателей на адреса
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

	// адрес сервера получен Ц пытаемс€ установить соединение 
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

	// Ўаг 4 - чтение и передача сообщений

	// id_char будет содержать id как массив из 4 char
	char id_char[4];
	// получаем id
	if (recv(my_sock, id_char, sizeof(id_char), 0) == SOCKET_ERROR) {
		// recv вернула ошибку
		printf("Recv error: %d\n", WSAGetLastError());
		closesocket(my_sock);
		WSACleanup();
		system("pause");
		return -1;
	}
	// извлекаем id
	int id = *((int *)id_char);

	printf("Person's ID: %d\n\n", id);

	// символьный буфер
	char buff[BUFFER_SIZE];

	// читаем пользовательский ввод с клавиатуры
	printf("Name: ");
	fgets(buff, sizeof(buff), stdin);
	// delete trailing '\n'
	int sl = strlen(buff) - 1;
	if (buff[sl] == '\n') buff[sl] = 0;
	// передаем им€ клиента серверу
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
		// recv вернула ошибку
		printf("Recv error: %d\n", WSAGetLastError());
		closesocket(my_sock);
		WSACleanup();
		system("pause");
		return -1;
	}
	// извлекаем i
	int i = *((int *)i_char);

	printf("Checked in into room %d for %d day(s)\n", i, dur);

	// waiting for leave message, when it comes, we leave
	if (recv(my_sock, buff, sizeof(buff), 0) == SOCKET_ERROR) {
		// recv вернула ошибку
		printf("Recv error: %d\n", WSAGetLastError());
		closesocket(my_sock);
		WSACleanup();
		system("pause");
		return -1;
	}
	printf("Checked out from room %d\n", i);
	printf("Bye!\n");

	//  орректный выход
	printf("Exiting...\n");
	closesocket(my_sock);
	WSACleanup();

	//system("pause");
	return 0;
	
}