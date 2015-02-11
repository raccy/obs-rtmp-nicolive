#pragma once

#include <QtCore>
#include <QtNetwork>

class NicoLive;

class NicoLiveCmdServer : public QObject {
	Q_OBJECT
public:
	enum struct COMMAND {
		UNKNOWN = 0, // default
		GET,
		SET,
		STAT,
		STRT,
		STOP,
		HELO,
		KEEP,
		QUIT
	};
	enum struct TARGET {
		UNKNOWN = 0, // default
		SESSION,
		STREAMING
	};
	static const QMap<QString, NicoLiveCmdServer::COMMAND> COMMAND_NAME;
	static const QMap<QString, NicoLiveCmdServer::TARGET> TARGET_NAME;
	static const QMap<int, QByteArray> CODE_NAME;
	static const int DEFAULT_PORT = 25083;
private:
	NicoLive *nicolive;
	QTcpServer *tcp4_server;
	QTcpServer *tcp6_server;
	int port = NicoLiveCmdServer::DEFAULT_PORT;
	int timeout = 60 * 1000;
public:
	NicoLiveCmdServer(NicoLive *nicolive);
	int getPort();
	void setPort(int port);
	bool start();
	bool stop();
private slots:
	void tcp4Run();
	void tcp6Run();
private:
	void runLoop(QTcpSocket *socket);
	QByteArray command(const QByteArray &cmd, bool &close_flag);
};
