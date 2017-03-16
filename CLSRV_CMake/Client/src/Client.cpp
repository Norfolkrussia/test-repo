// Client.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <iostream>
#include <boost/asio.hpp>

namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace expr = boost::log::expressions;
using boost::asio::ip::tcp;


void init()
{
	logging::add_common_attributes();
	logging::core::get()->set_filter
		(
		logging::trivial::severity >= logging::trivial::info												   //установка уровня важности лога
		);

	auto fmtTimeStamp = expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S.%f"); //переменная для отображения даты в заданном формате
	auto fmtSeverity = expr::attr<logging::trivial::severity_level>("Severity");							   //переменная для отображения уровня важности

	auto fsSink = logging::add_file_log																		   
		(
		keywords::file_name = "test_%Y-%m-%d_%H-%M-%S.%N.log",												   //настройка формата имени файла с логами
		keywords::rotation_size = 10 * 1024 * 1024															   //вес файла, при достижении которого будет создан новый
		//keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
		);
	logging::formatter logFmt = expr::format("[%1%] (%2%) %3%")												   //настройка формата лога в файле
		% fmtTimeStamp % fmtSeverity % expr::smessage;
	fsSink->set_formatter(logFmt);

}

int main(int argc, char **argv)
{
	init();

	//BOOST_LOG_TRIVIAL(trace) << "A trace severity message";
	//BOOST_LOG_TRIVIAL(debug) << "A debug severity message";
	//BOOST_LOG_TRIVIAL(info) << "An informational severity message";						//Макросы для печати логов
	//BOOST_LOG_TRIVIAL(warning) << "A warning severity message";
	//BOOST_LOG_TRIVIAL(error) << "An error severity message";
	//BOOST_LOG_TRIVIAL(fatal) << "A fatal severity message";

	BOOST_LOG_TRIVIAL(info) << "Client is running";
	try
	{
		boost::asio::io_service io;
		tcp::socket sock(io);																//создаем сокет
		tcp::endpoint endPoint(boost::asio::ip::address::from_string("127.0.0.1"), 9090);
		boost::system::error_code err;
		sock.connect(endPoint, err);														//производим соединение
		if (err)
		{
			throw boost::system::system_error(err);
			BOOST_LOG_TRIVIAL(error) << err;
		}
		else 
		{
			std::cout << "Connection established\n";
			BOOST_LOG_TRIVIAL(info) << "Connection established";
		}
		boost::asio::streambuf sbuf;
		boost::asio::read_until(sock, sbuf, "!");											//операция синхронного чтения, пока не встретится знак "!"
		std::cout << &sbuf << std::endl;													//вывод прочитанного
		sock.close();
	}
	catch(std::exception& e)
	{
		BOOST_LOG_TRIVIAL(error) << "Exception: " << e.what();
	}
	return 0;
}