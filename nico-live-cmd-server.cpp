#include <QtCore>
#include <QtNetwork>
#include "nicolive.h"
#include "nico-live.hpp"
#include "nico-live-cmd-server.hpp"

NicoLiveCmdServer::NicoLiveCmdServer(NicoLive *nicolive) :
	QObject(nicolive),
	nicolive(nicolive)
{
	this->tcp4_server = new QTcpServer(this);
	this->tcp6_server = new QTcpServer(this);
	connect(this->tcp4_server, SIGNAL(newConnection()),
			this, SLOT(tcp4Run()));
	connect(this->tcp6_server, SIGNAL(newConnection()),
			this, SLOT(tcp6Run()));
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
	if (!this->tcp4_server->isListening())
		this->tcp4_server->listen(QHostAddress::LocalHost, port);
	if (!this->tcp6_server->isListening())
		this->tcp6_server->listen(QHostAddress::LocalHostIPv6, port);
	return this->tcp4_server->isListening() ||
			this->tcp6_server->isListening();
}

bool NicoLiveCmdServer::stop()
{
	if (this->tcp4_server->isListening())
		this->tcp4_server->close();
	if (this->tcp6_server->isListening())
		this->tcp6_server->close();
	return (!this->tcp4_server->isListening()) &&
			(!this->tcp6_server->isListening());
}

void NicoLiveCmdServer::tcp4Run()
{
	while (this->tcp4_server->hasPendingConnections()) {
		QTcpSocket *socket = this->tcp4_server->nextPendingConnection();
		runLoop(socket);
		socket->deleteLater();
	}
}

void NicoLiveCmdServer::tcp6Run()
{
	while (this->tcp6_server->hasPendingConnections()) {
		QTcpSocket *socket = this->tcp6_server->nextPendingConnection();
		runLoop(socket);
		socket->deleteLater();
	}
}

void NicoLiveCmdServer::runLoop(QTcpSocket *socket)
{
	QByteArray buff;
	int lf_index;
	bool close_flag = false;
	while (socket->bytesAvailable() > 0 &&
			socket->waitForReadyRead(timeout)) {
		buff += socket->readAll();
		while ((lf_index = buff.indexOf('\n')) >= 0) {
			socket->write(command(buff.left(lf_index + 1),
					close_flag));
			if (close_flag) {
				socket->disconnectFromHost();
				break;
			}
			buff.remove(0, lf_index + 1);
		}
		if (close_flag) {
			break;
		}
	}
	if (socket->state() == QAbstractSocket::UnconnectedState ||
			socket->waitForDisconnected(timeout)) {
		nicolive_log_debug("socket disconnected");
	} else {
		nicolive_log_error("socket abort");
		socket->abort();
	}
}

QByteArray NicoLiveCmdServer::command(const QByteArray &cmd, bool &close_flag)
{
	(void)cmd;
	(void)close_flag;
	QByteArray result;
	return result;
}
