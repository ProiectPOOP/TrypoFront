#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <QAbstractSocket>
#include <QByteArray>
#include <QObject>
#include <QTcpSocket>

class SocketClient : public QObject
{
    Q_OBJECT

public:
    explicit SocketClient(QObject *parent = nullptr);
    void connectToBackend(const QString &host, quint16 port);
    void sendMessage(const QString &message);

signals:
    void dataReceived(const QString &data);
    void connectionStatusChanged(bool connected);

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *m_socket;
    // FIX: accumulation buffer for length-prefixed framing.
    // TCP may deliver data in fragments, so incoming bytes are buffered
    // here until a complete message (header + body) has arrived.
    QByteArray m_buffer;
};

#endif // SOCKETCLIENT_H