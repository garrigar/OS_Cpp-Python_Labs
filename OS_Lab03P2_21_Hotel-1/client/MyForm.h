#pragma once

//****************************************************************
// макрос, подавл§ющий ошибки устаревшей WinSock
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <winsock2.h> // winsock2.h должен быть подключен раньше windows.h
#include <windows.h>
#include <process.h>

// размер буфера символов
#define BUFFER_SIZE 1024

// порт дл§ подключени§
#define PORT 666
// адрес дл§ подключени§
#define SERVERADDR "127.0.0.1"

#define SECS_TO_CLOSE 10

// this function displays an error message
void showErrorDialog(System::String^ msg);
// this function displays an error message and safely exits the app
void showErrorDialogAndSafelyExit(System::String^ msg);

void toLog(System::String^ msg);

unsigned __stdcall Timeline(void* params);

SOCKET my_sock;

namespace client {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// —водка дл€ MyForm
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

			// °аг 1 - инициализаци§ библиотеки Winsock
			WSADATA ws;
			if (WSAStartup(0x202, &ws))
			{
				/*printf("WSAStartup error: %d\n", WSAGetLastError());
				system("pause");
				return -1;*/
				showErrorDialog("WSAStartup error: " + WSAGetLastError());

				// unsafe exit
				if (components) delete components;
				System::Environment::Exit(-1);
			}

			// °аг 2 - создание сокета
			/*SOCKET*/ my_sock = socket(AF_INET, SOCK_STREAM, 0);
			if (my_sock < 0)
			{
				//printf("Error creating socket: %d\n", WSAGetLastError());
				showErrorDialog("Error creating socket: " + WSAGetLastError());
				WSACleanup();
				/*system("pause");
				return -1;*/
				
				// unsafe exit
				if (components) delete components;
				System::Environment::Exit(-1);
			}

			// °аг 3 - установка соединени§

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
					/*printf("Invalid address: %s\n", SERVERADDR);
					closesocket(my_sock);
					WSACleanup();
					system("pause");
					return -1;*/
					showErrorDialogAndSafelyExit("Invalid address: " + SERVERADDR);
				}

			// адрес сервера получен ÷ пытаемс§ установить соединение 
			if (connect(my_sock, (sockaddr *)&dest_addr, sizeof(dest_addr)))
			{
				/*printf("Error connecting: %d\n", WSAGetLastError());
				closesocket(my_sock);
				WSACleanup();
				system("pause");
				return -1;*/
				showErrorDialogAndSafelyExit("Error connecting: " + WSAGetLastError());
			}

		}

	protected:
		/// <summary>
		/// ќсвободить все используемые ресурсы.
		/// </summary>
		~MyForm()
		{
			closesocket(my_sock);
			WSACleanup();

			if (components)
			{
				delete components;
			}
		}
	public: static System::Windows::Forms::TextBox^  textBox1;
	public: static System::Windows::Forms::RichTextBox^  textBox_log;
	public: static System::Windows::Forms::NumericUpDown^  numericUpDown1;

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Button^  button1;

private:


	protected:

	private:
		/// <summary>
		/// ќб€зательна€ переменна€ конструктора.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// “ребуемый метод дл€ поддержки конструктора Ч не измен€йте 
		/// содержимое этого метода с помощью редактора кода.
		/// </summary>
		void InitializeComponent(void)
		{
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->numericUpDown1 = (gcnew System::Windows::Forms::NumericUpDown());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->textBox_log = (gcnew System::Windows::Forms::RichTextBox());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown1))->BeginInit();
			this->SuspendLayout();
			// 
			// textBox1
			// 
			this->textBox1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->textBox1->Location = System::Drawing::Point(111, 10);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(259, 26);
			this->textBox1->TabIndex = 0;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label1->Location = System::Drawing::Point(12, 13);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(53, 20);
			this->label1->TabIndex = 1;
			this->label1->Text = L"Name";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 10, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(204)));
			this->label2->Location = System::Drawing::Point(12, 41);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(73, 20);
			this->label2->TabIndex = 2;
			this->label2->Text = L"Duration";
			// 
			// numericUpDown1
			// 
			this->numericUpDown1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->numericUpDown1->Location = System::Drawing::Point(111, 42);
			this->numericUpDown1->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 100000, 0, 0, 0 });
			this->numericUpDown1->Minimum = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			this->numericUpDown1->Name = L"numericUpDown1";
			this->numericUpDown1->Size = System::Drawing::Size(128, 22);
			this->numericUpDown1->TabIndex = 3;
			this->numericUpDown1->Value = System::Decimal(gcnew cli::array< System::Int32 >(4) { 1, 0, 0, 0 });
			// 
			// button1
			// 
			this->button1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->button1->Location = System::Drawing::Point(245, 42);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(125, 24);
			this->button1->TabIndex = 4;
			this->button1->Text = L"Go!";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &MyForm::button1_Click);
			// 
			// textBox_log
			// 
			this->textBox_log->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
				| System::Windows::Forms::AnchorStyles::Left)
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_log->Location = System::Drawing::Point(16, 80);
			this->textBox_log->Name = L"textBox_log";
			this->textBox_log->Size = System::Drawing::Size(354, 161);
			this->textBox_log->TabIndex = 5;
			this->textBox_log->Text = L"";
			// 
			// MyForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(8, 16);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(382, 253);
			this->ControlBox = false;
			this->Controls->Add(this->textBox_log);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->numericUpDown1);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->textBox1);
			this->MinimumSize = System::Drawing::Size(400, 300);
			this->Name = L"MyForm";
			this->Text = L"Client";
			this->Load += gcnew System::EventHandler(this, &MyForm::MyForm_Load);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->numericUpDown1))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void MyForm_Load(System::Object^  sender, System::EventArgs^  e) {
		//textBox_log->Text += "Successfully connected to " + SERVERADDR + "\n";
		textBox_log->Text += "Successfully reached the HILBERT'S HOTEL JR. building\n";

		// °аг 4 - чтение и передача сообщений

		// id_char будет содержать id как массив из 4 char
		char id_char[4];
		// получаем id
		if (recv(my_sock, id_char, sizeof(id_char), 0) == SOCKET_ERROR) {
			// recv вернула ошибку
			/*printf("Recv error: %d\n", WSAGetLastError());
			closesocket(my_sock);
			WSACleanup();
			system("pause");
			return -1;*/
			showErrorDialogAndSafelyExit("Recv error: " + WSAGetLastError());
		}
		// извлекаем id
		int id = *((int *)id_char);

		textBox_log->Text += ("Person's ID: " + id + "\n\n");
	}

	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
		
		textBox1->Enabled = false;
		button1->Enabled = false;
		numericUpDown1->Enabled = false;

		_beginthreadex(NULL, 0, Timeline, NULL, 0, NULL);
	}
};
}

unsigned __stdcall Timeline(void* params) {

	// символьный буфер
	char buff[BUFFER_SIZE];

	array<wchar_t>^ temp = client::MyForm::textBox1->Text->ToCharArray();
	int idx;
	for (idx = 0; idx < temp->Length; idx++)
	{
		buff[idx] = (char)temp[idx];
	}
	buff[idx] = 0;
	// передаем им§ клиента серверу
	send(my_sock, buff, sizeof(buff), 0);

	// customize duration
	// ---------------------------------------
	int dur = (int)client::MyForm::numericUpDown1->Value;

	char* dur_char = (char *)&dur;
	send(my_sock, dur_char, 4, 0);

	toLog("Waiting for room...\n");

	// i - room index
	char i_char[4];
	// try to get i, waiting for it to arrive, if so, we continue
	if (recv(my_sock, i_char, sizeof(i_char), 0) == SOCKET_ERROR) {
		// recv вернула ошибку
		/*printf("Recv error: %d\n", WSAGetLastError());
		closesocket(my_sock);
		WSACleanup();
		system("pause");
		return -1;*/
		showErrorDialogAndSafelyExit("Recv error: " + WSAGetLastError());
	}
	// извлекаем i
	int i = *((int *)i_char);

	toLog("Checked in into room " + i + " for " + dur + " day(s)\n");

	// waiting for leave message, when it comes, we leave
	if (recv(my_sock, buff, sizeof(buff), 0) == SOCKET_ERROR) {
		// recv вернула ошибку
		/*printf("Recv error: %d\n", WSAGetLastError());
		closesocket(my_sock);
		WSACleanup();
		system("pause");
		return -1;*/
		showErrorDialogAndSafelyExit("Recv error: " + WSAGetLastError());
	}

	toLog("Checked out from room " + i + "\n");
	toLog("Bye!\n");

	toLog("Service finished. This window will close in " + SECS_TO_CLOSE + " seconds\n");
	Sleep(SECS_TO_CLOSE * 1000);
	System::Windows::Forms::Application::Exit();

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
	client::MyForm::textBox_log->Text += msg;
}

void toLog(System::String^ msg) {
	client::MyForm::textBox_log->Invoke(gcnew AddMessageDelegate(LogAdd), { msg });
}