#include "OBJECTMGR.h"
#include "../protocol/protocol.h"
class GAME 
{
protected:
	sf::RenderWindow* m_window;
	
	sf::Texture m_map_texture;
	sf::Sprite m_map_sprite[4][4];
	
public:
	int m_id;
	sf::TcpSocket socket;
	OBJECTMGR* m_objectmgr;
	sf::RectangleShape m_max_hp_bar;
	sf::RectangleShape m_hp_bar;
	sf::RectangleShape m_exp_bar;
	sf::View m_view;
public:
	void send_packet(void* packet)
	{
		unsigned char* p = reinterpret_cast<unsigned char*>(packet);
		size_t sent = 0;
		socket.send(packet, p[0], sent);
	}
	GAME() { std::cout << "Wrong way to Create SFML Window" << std::endl; }
	GAME(int client_id, int width, int height, std::string title) {
		m_id = client_id;
		m_objectmgr = new OBJECTMGR();
		m_window = new sf::RenderWindow(sf::VideoMode(width, height), title);
		m_map_texture.loadFromFile("2drpgtile.png", sf::IntRect(0, 0, 8000, 8000));
		
		// hp ¹Ù
		m_max_hp_bar.setFillColor(sf::Color(255, 0, 0));
		m_max_hp_bar.setSize(sf::Vector2f(100, 10));
		m_max_hp_bar.setPosition(sf::Vector2f(80, 300));

		m_hp_bar.setFillColor(sf::Color(255, 127, 0));
		m_hp_bar.setSize(sf::Vector2f(100, 10));
		
		m_exp_bar.setFillColor(sf::Color::Yellow);
		m_exp_bar.setSize(sf::Vector2f(0, 10));
	

		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				m_map_sprite[i][j].setTexture(m_map_texture);
				m_map_sprite[i][j].setPosition(i * 8000, j * 8000);
			}
			
		}
	
		m_view.setCenter(sf::Vector2f(0, 0));
		m_view.setSize(sf::Vector2f(320, 320));
		m_window->setView(m_view);
		
	}
	void Run() {

		while (m_window->isOpen()) {
			recv_packet();
			sf::Event event;
			m_window->setView(m_view);
			while (m_window->pollEvent(event)) {
				if (event.type == sf::Event::Closed) {
					m_window->close();
					break;
				}
				if (event.type == sf::Event::KeyPressed) {
					switch (event.key.code) {
					case sf::Keyboard::Left: {
						CS_MOVE_PACKET mp;
						mp.type = CS_MOVE;
						mp.size = sizeof(CS_MOVE_PACKET);
						mp.direction = 2;
						send_packet(&mp);
						break;
					}
					case sf::Keyboard::Right: {
						CS_MOVE_PACKET mp;
						mp.type = CS_MOVE;
						mp.size = sizeof(CS_MOVE_PACKET);
						mp.direction = 3;
						send_packet(&mp);
						break;
					}
					case sf::Keyboard::Up: {
						CS_MOVE_PACKET mp;
						mp.type = CS_MOVE;
						mp.size = sizeof(CS_MOVE_PACKET);
						mp.direction = 0;
						send_packet(&mp);
						break;
					}
					case sf::Keyboard::Down: {
						CS_MOVE_PACKET mp;
						mp.type = CS_MOVE;
						mp.size = sizeof(CS_MOVE_PACKET);
						mp.direction = 1;
						send_packet(&mp);
						break;
					}
					case sf::Keyboard::X: {
						CS_ATTACK_PACKET p;
						p.type = CS_ATTACK;
						p.size = sizeof(CS_ATTACK_PACKET);
						send_packet(&p);
						break;
					}
					case sf::Keyboard::Escape:
						m_window->close();
						break;
					}
				}
			}
			m_window->clear();
			Render();
			m_window->display();
		
		}
	}
	~GAME() { }

protected:
	
	void recv_packet() {
		char net_buf[BUF_SIZE];
		size_t	received;

		auto recv_result = socket.receive(net_buf, BUF_SIZE, received);
		if (recv_result == sf::Socket::Error)
		{
			std::wcout << L"Recv ¿¡·¯!";
			while (true);
		}
		if (recv_result != sf::Socket::NotReady)
			if (received > 0) process_data(net_buf, received);
	}
	void ProcessPacket(char* ptr)
	{
		static bool first_time = true;
		switch (ptr[1])
		{
		case SC_LOGIN_INFO:
		{
			SC_LOGIN_INFO_PACKET* packet = reinterpret_cast<SC_LOGIN_INFO_PACKET*>(ptr);
			m_id = packet->id;
			m_objectmgr->AddObject(m_id,packet->x,packet->y,packet->max_hp, packet->hp, packet->level
				 ,packet->exp, packet->name);
			m_view.setCenter(packet->x * 16, packet->y * 16);
		}
		break;
		case SC_ADD_OBJECT:
		{
			SC_ADD_OBJECT_PACKET* packet = reinterpret_cast<SC_ADD_OBJECT_PACKET*>(ptr);
			m_objectmgr->AddObject(packet->id,packet->x, packet->y, 0
				, 0, 0, 0,packet->name);
		}
		break;
		case SC_MOVE_OBJECT:
		{
			
			SC_MOVE_OBJECT_PACKET* packet = reinterpret_cast<SC_MOVE_OBJECT_PACKET*>(ptr);
			m_objectmgr->Move(packet->id, packet->x, packet->y);
			if (packet->id == m_id) {
				m_view.setCenter(packet->x * 16, packet->y * 16);
			}
		}
		break;
		case SC_REMOVE_OBJECT:
		{
			SC_REMOVE_OBJECT_PACKET* my_packet = reinterpret_cast<SC_REMOVE_OBJECT_PACKET*>(ptr);
			m_objectmgr->RemoveObject(my_packet->id);
		}
		break;
		case SC_EXP:
		{
			SC_EXP_PACKET* my_packet = reinterpret_cast<SC_EXP_PACKET*>(ptr);
			m_objectmgr->AddEXP(m_id, my_packet->exp);
			m_exp_bar.setSize(sf::Vector2f(m_objectmgr->GetObject(m_id).m_exp, 10));
		}
		case SC_LOGIN_FAIL:
		{
			m_window->close();
		}
		default:
			printf("Unknown PACKET type [%d]\n", ptr[1]);
		}
	}
	void process_data(char* net_buf, size_t io_byte)
	{
		char* ptr = net_buf;
		static size_t in_packet_size = 0;
		static size_t saved_packet_size = 0;
		static char packet_buffer[BUF_SIZE];

		while (0 != io_byte) {
			if (0 == in_packet_size) in_packet_size = ptr[0];
			if (io_byte + saved_packet_size >= in_packet_size) {
				memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
				ProcessPacket(packet_buffer);
				ptr += in_packet_size - saved_packet_size;
				io_byte -= in_packet_size - saved_packet_size;
				in_packet_size = 0;
				saved_packet_size = 0;
			}
			else {
				memcpy(packet_buffer + saved_packet_size, ptr, io_byte);
				saved_packet_size += io_byte;
				io_byte = 0;
			}
		}
	}
	 void Tick();
	 void Render();
	 void LoadContent();
};



