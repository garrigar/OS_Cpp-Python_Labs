#pragma once

//***********************************************************
// макрос, подавляющий ошибки устаревшей WinSock
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h>	// winsock2.h должен быть подключен раньше windows.h
#include <windows.h>
#include <process.h>	// для _beginthreadex 

#include <vector>
#include <string>

// размер буфера символов
#define BUFFER_SIZE 1024

// порт, который слушает сервер
#define MY_PORT 666

// макрос для печати количества активных пользователей 
#define PRINTNUSERS if (nclients)\
  printf("%d user(s) online\n", nclients);\
  else printf("No users online\n");
#define PRINTNUSERS_LOG if (nclients)\
  toLog("NETWORK: " + nclients + " user(s) online\n");\
  else toLog("NETWORK: No users online\n");

// number of rooms in the hotel
#define ROOM_COUNT 10

// структура, через которую будут передаваться аргументы в ServeClient
typedef struct
{
	SOCKET * client_socket; // сокет клиента
	int client_id;			// ID клиента
} serve_client_params_t;

// прототип функции, обслуживающей подключившихся пользователей
unsigned __stdcall ServeClient(void* params);

// accepting connections function prototype
unsigned __stdcall AcceptConnections(void* params);

// this function displays an error message
void showErrorDialog(System::String^ msg);
// this function displays an error message and safely exits the app
void showErrorDialogAndSafelyExit(System::String^ msg);
// function writes to the log
void toLog(System::String^ msg);

// количество активных пользователей 
int nclients = 0;

int current_time = 0;

SOCKET mysocket;
bool closed = false;

HANDLE semaphore; // semaphore to access the building
HANDLE mutex; // mutex to access the reception

std::vector<bool> occupied; // whether a room is occupied or not
//std::vector<std::string> names; // names of every person in room
//***************************************************************

namespace server {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	//************************

	//************************

	/// <summary>
	/// Сводка для MyForm
	/// </summary>
	public ref class MyForm : public System::Windows::Forms::Form
	{
	public:
		MyForm(void)
		{
			InitializeComponent();
			//
			//TODO: добавьте код конструктора
			//

			// Шаг 1 - инициализация библиотеки сокетов
			WSADATA ws;
			if (WSAStartup(0x0202, &ws))
			{
				// Ошибка!
				//printf("Error WSAStartup: %d\n", WSAGetLastError());
				showErrorDialog("Error WSAStartup: " + WSAGetLastError());
				//system("pause");
				//return; //-1;

				// unsafe exit
				closed = true;
				if (components) delete components;
				System::Environment::Exit(-1);
			}

			// Шаг 2 - создание сокета
			// >>>>См. глобальную перем.>>>> SOCKET mysocket;
			// AF_INET - сокет Интернета
			// SOCK_STREAM - потоковый сокет (с установкой соединения)
			// 0 - по умолчанию выбирается TCP протокол
			if ((mysocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
			{
				// Ошибка!
				//printf("Error creating socket: %d\n", WSAGetLastError());
				showErrorDialog("Error creating socket: " + WSAGetLastError());
				// Деиницилизация библиотеки Winsock
				WSACleanup();
				//system("pause");
				//return; //-1;
				
				// unsafe exit
				closed = true;
				if (components) delete components;
				System::Environment::Exit(-1);
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
				//printf("Error binding: %d\n", WSAGetLastError());
				//closesocket(mysocket);  // закрываем сокет
				//WSACleanup();
				//system("pause");
				//return; //-1;
				showErrorDialogAndSafelyExit("Error binding: " + WSAGetLastError());
			}

			// Шаг 4 - ожидание подключений
			// размер очереди – 0x100
			if (listen(mysocket, 0x100))
			{
				// Ошибка
				//printf("Error listening: %d\n", WSAGetLastError());
				//closesocket(mysocket);
				//WSACleanup();
				//system("pause");
				//return; //-1;
				showErrorDialogAndSafelyExit("Error listening: " + WSAGetLastError());
			}

		}

	protected:
		/// <summary>
		/// Освободить все используемые ресурсы.
		/// </summary>
		~MyForm()
		{
			closed = true;

			//CloseHandle(semaphore); // exception on debug
			//CloseHandle(mutex); // exception on debug
			
			closesocket(mysocket);
			WSACleanup();

			if (components)
			{
				delete components;
			}
		}

	public: static System::Windows::Forms::DataGridView^  dataGridView1;
	public: static System::Windows::Forms::RichTextBox^  textBox_log;

	protected:

	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;

	
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  RoomColumn;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  IdColumn;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  NameColumn;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  BeginDayColumn;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  DurationColumn;



	protected:

	private:
		/// <summary>
		/// Обязательная переменная конструктора.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Требуемый метод для поддержки конструктора — не изменяйте 
		/// содержимое этого метода с помощью редактора кода.
		/// </summary>
		void InitializeComponent(void)
		{
			this->dataGridView1 = (gcnew System::Windows::Forms::DataGridView());
			this->RoomColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->IdColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->NameColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->BeginDayColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->DurationColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->textBox_log = (gcnew System::Windows::Forms::RichTextBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView1))->BeginInit();
			this->SuspendLayout();
			// 
			// dataGridView1
			// 
			this->dataGridView1->AllowUserToAddRows = false;
			this->dataGridView1->AllowUserToDeleteRows = false;
			this->dataGridView1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->dataGridView1->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(5) {
				this->RoomColumn,
					this->IdColumn, this->NameColumn, this->BeginDayColumn, this->DurationColumn
			});
			this->dataGridView1->Location = System::Drawing::Point(32, 81);
			this->dataGridView1->Name = L"dataGridView1";
			this->dataGridView1->ReadOnly = true;
			this->dataGridView1->RowHeadersVisible = false;
			this->dataGridView1->RowTemplate->Height = 24;
			this->dataGridView1->Size = System::Drawing::Size(615, 347);
			this->dataGridView1->TabIndex = 0;
			// 
			// RoomColumn
			// 
			this->RoomColumn->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::AllCells;
			this->RoomColumn->HeaderText = L"Room";
			this->RoomColumn->Name = L"RoomColumn";
			this->RoomColumn->ReadOnly = true;
			this->RoomColumn->Resizable = System::Windows::Forms::DataGridViewTriState::False;
			this->RoomColumn->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
			this->RoomColumn->Width = 51;
			// 
			// IdColumn
			// 
			this->IdColumn->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::AllCells;
			this->IdColumn->HeaderText = L"ID";
			this->IdColumn->Name = L"IdColumn";
			this->IdColumn->ReadOnly = true;
			this->IdColumn->Resizable = System::Windows::Forms::DataGridViewTriState::False;
			this->IdColumn->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
			this->IdColumn->Width = 27;
			// 
			// NameColumn
			// 
			this->NameColumn->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
			this->NameColumn->HeaderText = L"Name";
			this->NameColumn->Name = L"NameColumn";
			this->NameColumn->ReadOnly = true;
			this->NameColumn->Resizable = System::Windows::Forms::DataGridViewTriState::False;
			this->NameColumn->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
			// 
			// BeginDayColumn
			// 
			this->BeginDayColumn->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::AllCells;
			this->BeginDayColumn->HeaderText = L"Check-in day";
			this->BeginDayColumn->Name = L"BeginDayColumn";
			this->BeginDayColumn->ReadOnly = true;
			this->BeginDayColumn->Resizable = System::Windows::Forms::DataGridViewTriState::False;
			this->BeginDayColumn->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
			this->BeginDayColumn->Width = 96;
			// 
			// DurationColumn
			// 
			this->DurationColumn->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::AllCells;
			this->DurationColumn->HeaderText = L"Duration";
			this->DurationColumn->Name = L"DurationColumn";
			this->DurationColumn->ReadOnly = true;
			this->DurationColumn->Resizable = System::Windows::Forms::DataGridViewTriState::False;
			this->DurationColumn->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
			this->DurationColumn->Width = 68;
			// 
			// button1
			// 
			this->button1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->button1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->button1->Location = System::Drawing::Point(465, 19);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(182, 36);
			this->button1->TabIndex = 1;
			this->button1->Text = L"Next day";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &MyForm::button1_Click);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label1->Location = System::Drawing::Point(27, 25);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(63, 25);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Day 0";
			this->label1->Click += gcnew System::EventHandler(this, &MyForm::label1_Click);
			// 
			// label2
			// 
			this->label2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label2->Location = System::Drawing::Point(27, 450);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(93, 25);
			this->label2->TabIndex = 4;
			this->label2->Text = L"Event log";
			this->label2->Click += gcnew System::EventHandler(this, &MyForm::label2_Click);
			// 
			// textBox_log
			// 
			this->textBox_log->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_log->Location = System::Drawing::Point(32, 478);
			this->textBox_log->Name = L"textBox_log";
			this->textBox_log->ReadOnly = true;
			this->textBox_log->Size = System::Drawing::Size(615, 63);
			this->textBox_log->TabIndex = 5;
			this->textBox_log->Text = L"";
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(682, 553);
			this->Controls->Add(this->textBox_log);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->dataGridView1);
			this->MinimumSize = System::Drawing::Size(500, 400);
			this->Name = L"MyForm";
			this->Text = L"HILBERT\'S HOTEL JR.";
			this->Load += gcnew System::EventHandler(this, &MyForm::MyForm_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->dataGridView1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void MyForm_Load(System::Object^  sender, System::EventArgs^  e) {
		
		dataGridView1->Rows->Add(ROOM_COUNT);
		for (size_t i = 0; i < ROOM_COUNT; i++)
		{
			dataGridView1["RoomColumn", i]->Value = i;
			dataGridView1["RoomColumn", i]->Style->BackColor = Color::Green;
		}
		dataGridView1->ClearSelection();
		
		// vectors init
		occupied = std::vector<bool>(ROOM_COUNT);
		//names = std::vector<std::string>(ROOM_COUNT);

		//printf("...Awaiting visitors...\n\n");
		textBox_log->Text += "...Awaiting visitors...\n";
		
		semaphore = CreateSemaphore(NULL, ROOM_COUNT, ROOM_COUNT, NULL);
		mutex = CreateMutex(NULL, FALSE, NULL);
		
		_beginthreadex(NULL, 0, AcceptConnections, NULL, 0, NULL);

	}
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
		 current_time++;
		 label1->Text = "Day " + current_time;
	}
	private: System::Void label1_Click(System::Object^  sender, System::EventArgs^  e) {
	}
	private: System::Void label2_Click(System::Object^  sender, System::EventArgs^  e) {
	}
};

}

unsigned __stdcall AcceptConnections(void* params) {

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
		if (closed) return -1;

		nclients++;      // увеличиваем счетчик подключившихся клиентов

		// пытаемся получить имя хоста
		HOSTENT* hst = gethostbyaddr((char *)&client_addr.sin_addr.s_addr, 4, AF_INET);

		// вывод сведений о клиенте
		/*printf("+%s [%s] - CONNECTED - ID #%d\n",
		(hst) ? hst->h_name : "",
		inet_ntoa(client_addr.sin_addr),
		idclients);*/
		//PRINTNUSERS;
		
		/*toLog("NETWORK: +" + gcnew System::String((hst) ? hst->h_name : "") +
			" [" + gcnew System::String(inet_ntoa(client_addr.sin_addr)) +
			"] - CONNECTED - ID #" + idclients + "\n");
		PRINTNUSERS_LOG;*/

		// собираем параметры для функции ServeClient
		serve_client_params_t params;
		params.client_socket = &client_socket;
		params.client_id = idclients++;

		// Вызов нового потока для обслужвания клиента
		_beginthreadex(NULL, 0, ServeClient, &params, 0, NULL);

	}

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
		/*printf("Recv error: %d\n", WSAGetLastError());
		closesocket(my_sock);
		WSACleanup();
		system("pause");
		return -1;*/
		showErrorDialogAndSafelyExit("Recv error: " + WSAGetLastError());
	}

	// how many days will a person be visitor
	int duration;
	// -------------------------------------------------------------------------------------------------
	char dur_char[4];
	if (recv(my_sock, dur_char, sizeof(dur_char), 0) == SOCKET_ERROR) {
		// recv вернула ошибку
		/*printf("Recv error: %d\n", WSAGetLastError());
		closesocket(my_sock);
		WSACleanup();
		system("pause");
		return -1;*/
		showErrorDialogAndSafelyExit("Recv error: " + WSAGetLastError());
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
			break;
		}
	}
	occupied[i] = true;
	server::MyForm::dataGridView1["RoomColumn", i]->Style->BackColor = 
		System::Drawing::Color::Red;

	//names[i] = std::string(buff);
	server::MyForm::dataGridView1["NameColumn", i]->Value = gcnew System::String(buff);

	send(my_sock, (char *)&i, 4, 0);

	server::MyForm::dataGridView1["IdColumn", i]->Value = id;
	server::MyForm::dataGridView1["BeginDayColumn", i]->Value = startTime;
	server::MyForm::dataGridView1["DurationColumn", i]->Value = duration;
	//printf("Person %s (id = %d) checked in into room %d for %d day(s)\n", buff, id, i, duration);
	toLog("DAY " + current_time + ": Person " +
		(gcnew System::String(buff)) + " (id = " + id + ") checked in into room " + i +
		" for " + duration + " day(s)\n");
	ReleaseMutex(mutex);
	// check in ends

	// sleep on soft bed
	while (current_time < (startTime + duration)) {
		Sleep(5);
	}

	// check out procedure
	WaitForSingleObject(mutex, INFINITE);
	occupied[i] = false;
	server::MyForm::dataGridView1["RoomColumn", i]->Style->BackColor = 
		System::Drawing::Color::Green;
	//names[i] = "";
	server::MyForm::dataGridView1["NameColumn", i]->Value = "";

	server::MyForm::dataGridView1["IdColumn", i]->Value = "";
	server::MyForm::dataGridView1["BeginDayColumn", i]->Value = "";
	server::MyForm::dataGridView1["DurationColumn", i]->Value = "";

	const char * goaway = "leave";
	send(my_sock, goaway, sizeof(goaway), 0);
	
	//printf("Person %s (id = %d) checked out from room %d\n", buff, id, i);
	toLog("DAY " + current_time + ": Person " + 
		(gcnew System::String(buff)) + " (id = " + id + ") checked out from room " + i + "\n");
	ReleaseMutex(mutex);
	// check out ends

	ReleaseSemaphore(semaphore, 1, NULL);
	// leave the building

	nclients--; // уменьшаем счетчик активных клиентов
	//printf("CLIENT #%d - DISCONNECTED\n", id);
	//PRINTNUSERS;

	/*toLog("NETWORK: CLIENT #" + id + " - DISCONNECTED\n");
	PRINTNUSERS_LOG;*/

	// закрываем сокет
	closesocket(my_sock);

	return 0;
}

void showErrorDialog(System::String^ msg) {
	System::Windows::Forms::MessageBox::Show(msg + "\nPress OK to exit", "Error");
}

void showErrorDialogAndSafelyExit(System::String^ msg) {
	showErrorDialog(msg);
	System::Windows::Forms::Application::Exit();
}

delegate void AddMessageDelegate(System::String^ msg);

void LogAdd(System::String^ msg) {
	server::MyForm::textBox_log->Text = msg + server::MyForm::textBox_log->Text;
}

void toLog(System::String^ msg) {
	server::MyForm::textBox_log->Invoke(gcnew AddMessageDelegate(LogAdd), { msg });
}