// макрос, подавляющий ошибки устаревшей WinSock
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h>	// winsock2.h должен быть подключен раньше windows.h
#include <windows.h>
#include <process.h>	// для _beginthreadex 

#include <vector>
#include <string>
#include <conio.h>		// for _getch()

// размер буфера символов
#define BUFFER_SIZE 1024

// порт, который слушает сервер
#define MY_PORT 666

// макрос для печати количества активных пользователей 
#define PRINTNUSERS if (nclients)\
  printf("%d user(s) online\n", nclients);\
  else printf("No users online\n");

// number of rooms in the hotel
#define ROOM_COUNT 3

// структура, через которую будут передаваться аргументы в ServeClient
typedef struct
{
	SOCKET * client_socket; // сокет клиента
	int client_id;			// ID клиента
} serve_client_params_t;

// прототип функции, обслуживающей подключившихся пользователей
unsigned __stdcall ServeClient(void* params);

// timeline function prototype
unsigned __stdcall Timeline(void* params);

// количество активных пользователей 
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
	
	// Шаг 1 - инициализация библиотеки сокетов
	WSADATA ws;
	if (WSAStartup(0x0202, &ws))
	{
		// Ошибка!
		printf("Error WSAStartup: %d\n", WSAGetLastError());
		system("pause");
		return -1;
	}

	// Шаг 2 - создание сокета
	SOCKET mysocket;
	// AF_INET - сокет Интернета
	// SOCK_STREAM - потоковый сокет (с установкой соединения)
	// 0 - по умолчанию выбирается TCP протокол
	if ((mysocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		// Ошибка!
		printf("Error creating socket: %d\n", WSAGetLastError());
		// Деиницилизация библиотеки Winsock
		WSACleanup();
		system("pause");
		return -1;
	}

	// Шаг 3 - связывание сокета с локальным адресом
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(MY_PORT);	// не забываем о сетевом порядке
	local_addr.sin_addr.s_addr = 0;			// сервер принимает подключения на все IP-адреса

	// вызываем bind для связывания
	if (bind(mysocket, (sockaddr *)&local_addr, sizeof(local_addr)))
	{
		// Ошибка
		printf("Error binding: %d\n", WSAGetLastError());
		closesocket(mysocket);  // закрываем сокет
		WSACleanup();
		system("pause");
		return -1;
	}

	// Шаг 4 - ожидание подключений
	// размер очереди – 0x100
	if (listen(mysocket, 0x100))
	{
		// Ошибка
		printf("Error listening: %d\n", WSAGetLastError());
		closesocket(mysocket);
		WSACleanup();
		system("pause");
		return -1;
	}

	printf("...Awaiting visitors...\n\n");

	// Шаг 5 - извлекаем сообщение из очереди
	SOCKET client_socket;		// сокет для клиента
	sockaddr_in client_addr;    // адрес клиента (заполняется системой)

	// функции accept необходимо передать размер структуры
	int client_addr_size = sizeof(client_addr);

	// ID последнего клиента
	int idclients = 0;
	semaphore = CreateSemaphore(NULL, ROOM_COUNT, ROOM_COUNT, NULL);
	mutex = CreateMutex(NULL, FALSE, NULL);
	// start the time counter
	_beginthreadex(NULL, 0, Timeline, NULL, 0, NULL);

	// цикл извлечения запросов на подключение из очереди
	while ((client_socket = accept(mysocket, (sockaddr *)&client_addr, &client_addr_size)))
	{
		nclients++;      // увеличиваем счетчик подключившихся клиентов

		// пытаемся получить имя хоста
		HOSTENT* hst = gethostbyaddr((char *)&client_addr.sin_addr.s_addr, 4, AF_INET);

		// вывод сведений о клиенте
		/*printf("+%s [%s] - CONNECTED - ID #%d\n",
			(hst) ? hst->h_name : "",
			inet_ntoa(client_addr.sin_addr),
			idclients);*/
		//PRINTNUSERS;

		// собираем параметры для функции ServeClient
		serve_client_params_t params;
		params.client_socket = &client_socket;
		params.client_id = idclients++;

		// Вызов нового потока для обслужвания клиента
		_beginthreadex(NULL, 0, ServeClient, &params, 0, NULL);

	}
	// Скорее всего, недостижимый участок
	// Если мы тут, то ошибка?
	CloseHandle(semaphore);
	CloseHandle(mutex);
	printf("Unknown error: %d\n", WSAGetLastError());
	closesocket(mysocket);
	WSACleanup();
	system("pause");
	return 0;
}

// Эта функция создается в отдельном потоке и обсуживает очередного подключившегося клиента независимо от остальных
unsigned __stdcall ServeClient(void* params)
{
	// достаем параметры
	serve_client_params_t * ptr = (serve_client_params_t *)params;
	SOCKET my_sock = *(ptr->client_socket); // сокет клиента
	int id = ptr->client_id;				// ID клиента

	// id_char указывает на id как на массив из 4 char
	char* id_char = (char *)&id;
	// передаем его клиенту
	send(my_sock, id_char, 4, 0);

	// символьный буфер
	char buff[BUFFER_SIZE];

	// получаем имя клиента
	if (recv(my_sock, buff, sizeof(buff), 0) == SOCKET_ERROR) {
		// recv вернула ошибку
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
		// recv вернула ошибку
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

	nclients--; // уменьшаем счетчик активных клиентов
	//printf("CLIENT #%d - DISCONNECTED\n", id);
	//PRINTNUSERS;

	// закрываем сокет
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