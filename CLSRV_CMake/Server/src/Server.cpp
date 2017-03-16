// Server.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <iostream>
#include <string>
#include <boost/asio.hpp>//libws2_32.a, libboost_system.a
#include <conio.h>

using boost::asio::ip::tcp;

int main (){
	std::cout<<"Server is running\n";
	try{
		boost::asio::io_service io;
		tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 9090));		//создаем acceptor для прослушивания новых подключений на порту 9090, IP вериии 4.
		std::string message = "Hello from server!";						//строка для отправки на клиент
		while(true){
			tcp::socket sock(io);										//создаем сокет
			acceptor.accept(sock);										//дожидаемся соединения
			std::cout<<"Client has been accepted\n";
			boost::asio::write(sock, boost::asio::buffer(message));		//запись в поток, которым является сокет
		}	
		acceptor.close();												//обрываем соединение
	}
	catch(std::exception& e){								
		std::cout << e.what() << std::endl;
	}
	return 0;
}

