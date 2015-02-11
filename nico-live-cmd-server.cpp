#include <QtCore>
#include <QtNetwork>
#include "nicolive.h"
#include "nico-live.hpp"
#include "nico-live-cmd-server.hpp"

// deafult NicoLiveCmdServer::COMMAND::DEFAULT == 0;
const QMap<QString, NicoLiveCmdServer::COMMAND>
		NicoLiveCmdServer::COMMAND_NAME = {
	{"GET", NicoLiveCmdServer::COMMAND::GET},
	{"SET", NicoLiveCmdServer::COMMAND::SET},
	{"STAT", NicoLiveCmdServer::COMMAND::STAT},
	{"STRT", NicoLiveCmdServer::COMMAND::STRT},
	{"STOP", NicoLiveCmdServer::COMMAND::STOP},
	{"HELO", NicoLiveCmdServer::COMMAND::HELO},
	{"KEEP", NicoLiveCmdServer::COMMAND::KEEP},
	{"QUIT", NicoLiveCmdServer::COMMAND::QUIT}
};

// deafult NicoLiveCmdServer::TARGET::DEFAULT == 0;
const QMap<QString, NicoLiveCmdServer::TARGET>
		NicoLiveCmdServer::TARGET_NAME = {
	{"SESSION", NicoLiveCmdServer::TARGET::SESSION},
	{"STREAMING", NicoLiveCmdServer::TARGET::STREAMING}
};

const QMap<int, QByteArray> NicoLiveCmdServer::CODE_NAME = {
	{100, "HELO"},
	{200, "OK"},
	{210, "TRUE"},
	{220, "FALSE"},
	{400, "SYNTAX_ERROR"},
	{401, "SIZE_OVER"},
	{402, "INVALID_ENCODE"},
	{410, "UNKNOWN_COMMAND"},
	{411, "UNKNOWN_TARGET"},
	{412, "UNKNOWN_OPTION"},
	{420, "NOT_IMPLEMENT"},
	{500, "SERVER_ERR"},
	{510, "BUSY"},
	{520, "ALREADY_DONE"},
	{590, "SERVER_STOP"},
	{591, "MAX_CONNECT"},
};

NicoLiveCmdServer::NicoLiveCmdServer(NicoLive *nicolive) :
	QObject(nicolive),
	nicolive(nicolive)
{
	this->tcp4_server = new QTcpServer(this);
	this->tcp6_server = new QTcpServer(this);
	connect(this->tcp4_server, &QTcpServer::newConnection, [=](){
		this->tcpConnection(this->tcp4_server);
	});
	connect(this->tcp6_server, &QTcpServer::newConnection, [=](){
		this->tcpConnection(this->tcp6_server);
	});
}

int NicoLiveCmdServer::getPort()
{
	return this->port;
}

void NicoLiveCmdServer::setPort(int port)
{
	if (port > 0 && port < 65536)
		this->port = port;
}

bool NicoLiveCmdServer::start()
{
	nicolive_log_debug("cmd server: start");
	if (!this->tcp4_server->isListening())
		this->tcp4_server->listen(QHostAddress::LocalHost, port);
	if (!this->tcp6_server->isListening())
		this->tcp6_server->listen(QHostAddress::LocalHostIPv6, port);
	return this->tcp4_server->isListening() ||
			this->tcp6_server->isListening();
}

bool NicoLiveCmdServer::stop()
{
	nicolive_log_debug("cmd server: stop");
	if (this->tcp4_server->isListening())
		this->tcp4_server->close();
	if (this->tcp6_server->isListening())
		this->tcp6_server->close();
	return (!this->tcp4_server->isListening()) &&
			(!this->tcp6_server->isListening());
}

void NicoLiveCmdServer::tcpConnection(QTcpServer *server)
{
	while (server->hasPendingConnections()) {
		nicolive_log_debug("cmd server connect tcp");
		QTcpSocket *socket = server->nextPendingConnection();
		this->socket_buff[socket] = QByteArray();
		connect(socket, &QTcpSocket::readyRead, [=](){
			this->readSocket(socket);
		});
		connect(socket, &QTcpSocket::disconnected, [=](){
			nicolive_log_debug("socket disconnected");
			this->socket_buff.remove(socket);
			socket->deleteLater();
		});
	}
}

void NicoLiveCmdServer::readSocket(QTcpSocket *socket)
{
	int lf_index;
	bool close_flag = false;
	while ((!close_flag) && socket->bytesAvailable() > 0) {
		nicolive_log_debug("cmd server: read");
		this->socket_buff[socket] += socket->readAll();
		while ((lf_index = this->socket_buff[socket].indexOf('\n'))
		 		>= 0) {
			socket->write(command(this->socket_buff[socket]
					.left(lf_index + 1), close_flag));
			if (close_flag) {
				socket->disconnectFromHost();
				break;
			}
			this->socket_buff[socket].remove(0, lf_index + 1);
		}
	}
}

// void NicoLiveCmdServer::runLoop(QTcpSocket *socket)
// {
// 	QByteArray buff;
// 	int lf_index;
// 	bool close_flag = false;
// 	while (socket->bytesAvailable() > 0 ||
// 			socket->waitForReadyRead(timeout)) {
// 		nicolive_log_debug("cmd server: read");
// 		buff += socket->readAll();
// 		while ((lf_index = buff.indexOf('\n')) >= 0) {
// 			socket->write(command(buff.left(lf_index + 1),
// 					close_flag));
// 			if (close_flag) {
// 				socket->disconnectFromHost();
// 				break;
// 			}
// 			buff.remove(0, lf_index + 1);
// 		}
// 		if (close_flag) {
// 			break;
// 		}
// 	}
// 	if (socket->state() == QAbstractSocket::UnconnectedState ||
// 			socket->waitForDisconnected(timeout)) {
// 		nicolive_log_debug("socket disconnected");
// 	} else {
// 		nicolive_log_error("socket abort");
// 		socket->abort();
// 	}
// }

QByteArray NicoLiveCmdServer::command(const QByteArray &cmd, bool &close_flag)
{
	nicolive_log_debug("read cmd: %s",
			cmd.toPercentEncoding("%", "", '\\').constData());
	int code = 500; // bug
	QString desc;
	QByteArray result; // empty

	QString cmd_command, cmd_target, cmd_option;
	QList<QByteArray> cmd_list = cmd.simplified().split(' ');
	int count = 0;
	for (auto ba: cmd_list) {
		if (!ba.isEmpty()) {
			switch (count) {
			case 0:
				cmd_command = QString::fromLatin1(ba).toUpper();
				break;
			case 1:
				cmd_target = QString::fromLatin1(ba).toUpper();
				break;
			case 2:
				ba.replace('+', ' ');
				cmd_option = QUrl::fromPercentEncoding(ba);
				break;
			}
			++count;
		}
	}
	nicolive_log_debug("cmd command: %s",
			cmd_command.toStdString().c_str());
	nicolive_log_debug("cmd target: %s", cmd_target.toStdString().c_str());
	nicolive_log_debug("cmd option: %s", cmd_option.toStdString().c_str());

	if (cmd_command.isEmpty()) {
		// no command, so do nothing
		return result;
	}

	// TODO:
	switch (NicoLiveCmdServer::COMMAND_NAME[cmd_command]) {
	case NicoLiveCmdServer::COMMAND::GET:
		code = 420;
		break;
	case NicoLiveCmdServer::COMMAND::SET:
		code = 420;
		break;
	case NicoLiveCmdServer::COMMAND::STAT:
		code = 420;
		break;
	case NicoLiveCmdServer::COMMAND::STRT:
		code = 420;
		break;
	case NicoLiveCmdServer::COMMAND::STOP:
		code = 420;
		break;
	case NicoLiveCmdServer::COMMAND::HELO:
		code = 420;
		break;
	case NicoLiveCmdServer::COMMAND::KEEP:
		code = 420;
		break;
	case NicoLiveCmdServer::COMMAND::QUIT:
		close_flag = true;
		code = 0;
		break;
	case NicoLiveCmdServer::COMMAND::UNKNOWN:
	default:
		code = 410;
	}

	if (code != 0) {
		result = QByteArray::number(code);
		result += " ";
		result += NicoLiveCmdServer::CODE_NAME[code];
		if (!desc.isEmpty()) {
			result += " ";
			result += QUrl::toPercentEncoding(desc);
		}
		result += "\r\n";
	}
	return result;
}
