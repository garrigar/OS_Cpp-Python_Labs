// макрос, подавляющий ошибки устаревшей WinSock
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <winsock2.h> // winsock2.h должен быть подключен раньше windows.h
#include <windows.h>

// размер буфера символов
#define BUFFER_SIZE 1024

// порт для подключения
#define PORT 666
// адрес для подключения
#define SERVERADDR "127.0.0.1"

// прототип функции, осушествляющей сдвиг по Цезарю
void CaesarShiftEN(char* text, int offs);

int main()
{

	printf("CAESAR CIPHER SOCKET APPLICATION\n");
	printf("CLIENT\n");
	printf("Client recieves messages from standard input, encrypts and sends them to the server.\n");
	printf("One message can contain up to %d characters. ", BUFFER_SIZE - 2);
	printf("Otherwise it will be splitted to blocks with %d characters in each.\n", BUFFER_SIZE - 1);
	printf("WARNING: Only latin letters will be encrypted.\n\n");

	// Шаг 1 - инициализация библиотеки Winsock
	WSADATA ws;
	if (WSAStartup(0x202, &ws))
	{
		printf("WSAStartup error: %d\n", WSAGetLastError());
		system("pause");
		return -1;
	}

	// Шаг 2 - создание сокета
	SOCKET my_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (my_sock < 0)
	{
		printf("Error creating socket: %d\n", WSAGetLastError());
		WSACleanup();
		system("pause");
		return -1;
	}

	// Шаг 3 - установка соединения

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

	// адрес сервера получен – пытаемся установить соединение 
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

	// Шаг 4 - чтение и передача сообщений

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

	printf("Client's ID on server: %d\n\n", id);

	// символьный буфер
	char buff[BUFFER_SIZE];

	while (true)
	{
		// читаем пользовательский ввод с клавиатуры
		printf(">> ");
		fgets(buff, sizeof(buff), stdin);

		// проверка на "quit"
		if (!strcmp(buff, "quit\n"))
		{
			// Корректный выход
			printf("Exiting...");
			closesocket(my_sock);
			WSACleanup();
			return 0;
		}

		// сдвиг вправо на id для зашифровки
		int shift = id;
		// если сдвиг не "собирается" происходить, значит, двигаем вправо на 3 по умолчанию
		if (shift % 26 == 0) shift = 3;
		// осуществляем зашифровку
		CaesarShiftEN(buff, shift);
		// вывод зашифрованного сообщения для дебага
		//printf("DEBUG - ENCRYPTED: %s\n", buff);

		// передаем строку клиента серверу
		send(my_sock, buff, sizeof(buff), 0);
	}

	// для эхо-сервера
	/*
	int nsize;
	while ((nsize = recv(my_sock, buff, sizeof(buff), 0)) != SOCKET_ERROR)
	{
		// ставим завершающий ноль в конце строки
		buff[nsize] = 0;

		// выводим на экран
		printf("S => C: %s", buff);

		// читаем пользовательский ввод с клавиатуры
		printf("S <= C: ");
		fgets(buff, sizeof(buff), stdin);

		// проверка на "quit"
		if (!strcmp(buff, "quit\n"))
		{
			// Корректный выход
			printf("Exit...");
			closesocket(my_sock);
			WSACleanup();
			return 0;
		}

		// передаем строку клиента серверу
		send(my_sock, buff, sizeof(buff) - 1, 0);
	}

	printf("Recv error: %d\n", WSAGetLastError());
	closesocket(my_sock);
	WSACleanup();
	system("pause");
	return -1;
	*/
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
