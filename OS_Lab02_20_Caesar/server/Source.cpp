// макрос, подавляющий ошибки устаревшей WinSock
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h>	// winsock2.h должен быть подключен раньше windows.h
#include <windows.h>
#include <process.h>	// для _beginthreadex 

// размер буфера символов
#define BUFFER_SIZE 1024

// порт, который слушает сервер
#define MY_PORT 666

// макрос для печати количества активных пользователей 
#define PRINTNUSERS if (nclients)\
  printf("%d user(s) online\n", nclients);\
  else printf("No users online\n");

// структура, через которую будут передаваться аргументы в ServeClient
typedef struct
{
	SOCKET * client_socket; // сокет клиента
	int client_id;			// ID клиента
} serve_client_params_t;

// прототип функции, обслуживающей подключившихся пользователей
unsigned __stdcall ServeClient(void* params);

// прототип функции, осушествляющей сдвиг по Цезарю
void CaesarShiftEN(char* text, int offs);

// количество активных пользователей 
int nclients = 0;

int main()
{

	printf("CAESAR CIPHER SOCKET APPLICATION\n");
	printf("SERVER\n");
	printf("This server recieves encrypted messages from its clients, decrypts and shows them.\n");
	printf("WARNING: Only latin letters will be decrypted.\n\n");

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

	printf("...Awaiting connections...\n\n");

	// Шаг 5 - извлекаем сообщение из очереди
	SOCKET client_socket;		// сокет для клиента
	sockaddr_in client_addr;    // адрес клиента (заполняется системой)

	// функции accept необходимо передать размер структуры
	int client_addr_size = sizeof(client_addr);

	// ID последнего клиента
	int idclients = 0;

	// цикл извлечения запросов на подключение из очереди
	while ((client_socket = accept(mysocket, (sockaddr *)&client_addr, &client_addr_size)))
	{
		nclients++;      // увеличиваем счетчик подключившихся клиентов

		// пытаемся получить имя хоста
		HOSTENT* hst = gethostbyaddr((char *)&client_addr.sin_addr.s_addr, 4, AF_INET);

		// вывод сведений о клиенте
		printf("+%s [%s] - CONNECTED - ID #%d\n",
			(hst) ? hst->h_name : "",
			inet_ntoa(client_addr.sin_addr),
			idclients);

		PRINTNUSERS;

		// собираем параметры для функции ServeClient
		serve_client_params_t params;
		params.client_socket = &client_socket;
		params.client_id = idclients++;

		// Вызов нового потока для обслужвания клиента
		_beginthreadex(NULL, 0, ServeClient, &params, 0, NULL);

	}
	// Скорее всего, недостижимый участок
	// Если мы тут, то ошибка?
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

	// цикл сервера: прием строки от клиента, дешифрование и вывод на экран
	int bytes_recv;
	while ((bytes_recv = recv(my_sock, buff, sizeof(buff), 0))
		&& bytes_recv != SOCKET_ERROR)
	{
		// сдвиг влево на id для дешифровки
		int shift = -id;
		// если сдвиг не "собирается" происходить, значит, двигаем влево на 3 по умолчанию
		if (shift % 26 == 0) shift = -3;
		// вывод зашифрованного сообщения для дебага
		//printf("\nDEBUG - ENCRYPTED: %s", buff);
		// осуществляем дешифровку
		CaesarShiftEN(buff, shift);

		// вывод на экран
		printf(" <client #%d:> %s", id, buff);

		// для эхо-сервера
		//send(my_sock, buff, bytes_recv, 0);
	}

	// если мы здесь, то произошел выход из цикла по причине возращения функцией recv ошибки –
	// соединение с клиентом разорвано
	nclients--; // уменьшаем счетчик активных клиентов
	printf("CLIENT #%d - DISCONNECTED\n", id);
	PRINTNUSERS;

	// закрываем сокет
	closesocket(my_sock);

	return 0;
}

// функция, осуществляющая сдвиг по Цезарю латинских букв из text на offs вправо (влево на -offs при offs < 0)
void CaesarShiftEN(char* text, int offs)
{
	// избавляемся от лишних тождественных комбинаций сдвигов по 26
	// сводим всё к сдвигу от -25 до +25
	offs %= 26;
	// заменяем сдвиг влево (-25..-1) сдвигом вправо
	if (offs < 0)
		offs += 26;
	// таким образом, всё сведено к сдвигу вправо на величину от 0 до 25

	int i = 0;
	// пока не достигли последнего символа в строке
	while (text[i] != 0)
	{
		// обрабатываем только латинские буквы
		if ((text[i] > 0) && isalpha(text[i]))
		{
			// если при сдвиге вправо мы выйдем за последнюю букву
			if ((((int)text[i]) + offs) > ((int)(isupper(text[i]) ? 'Z' : 'z')))
				// то сделаем эквивалентный сдвиг влево
				text[i] += (offs - 26);
			else
				// иначе делаем этот сдвиг вправо
				text[i] += offs;
		}
		i++;
	}
}
