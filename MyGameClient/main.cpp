#include "Game.h"


#define BUF_SIZE 512
#define NAME_SIZE  100

GAME* g_game;


int main()
{
	char name[100];
	std::cout << "�̸��� �Է��Ͻÿ�: ";
	std::cin >> name;
	CS_LOGIN_PACKET lp;
	lp.type = CS_LOGIN;
	lp.size = sizeof(CS_LOGIN_PACKET);
	strcpy_s(lp.name, name);

	std::wcout.imbue(std::locale("korean"));
	g_game = new GAME(0,720, 720, "MYGAMECLIENT");
	sf::Socket::Status status = g_game->socket.connect("127.0.0.1", 4000);
	g_game->socket.setBlocking(false);
	if (status != sf::Socket::Done) {
		std::wcout << L"������ ������ �� �����ϴ�.\n";
		while (true);
	}
	g_game->send_packet(&lp);
	g_game->Run();
	delete g_game;
}