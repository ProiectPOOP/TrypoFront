#ifndef SOCKETCLIENT_H
#define SOCKETCLIENT_H

#include <QAbstractSocket>
#include <QObject>
#include <QTcpSocket>

class SocketClient : public QObject
{
    Q_OBJECT
public:
    explicit SocketClient(QObject *parent = nullptr);

    // Metodă pentru a iniția conexiunea
    void connectToBackend(const QString &host, quint16 port);

    // Metodă pentru a trimite date (mesaje, comenzi)
    void sendMessage(const QString &message);

signals:
    // Semnale pe care le poți capta în interfața grafică (UI)
    void dataReceived(const QString &data);
    void connectionStatusChanged(bool connected);

private slots:
    void onReadyRead();
    void onConnected();
    void onDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *m_socket;
};

#endif // SOCKETCLIENT_H