#include "socketclient.h"
#include <QDebug>
#include <QMessageBox>
#include <QtEndian>

SocketClient::SocketClient(QObject *parent)
    : QObject(parent)
{
    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::connected, this, &SocketClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &SocketClient::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &SocketClient::onReadyRead);
    connect(m_socket, &QTcpSocket::errorOccurred, this, &SocketClient::onErrorOccurred);
}

void SocketClient::connectToBackend(const QString &host, quint16 port)
{
    qDebug() << "Connecting to" << host << ":" << port;
    m_socket->connectToHost(host, port);
}

void SocketClient::sendMessage(const QString &message)
{
    if (m_socket->state() != QAbstractSocket::ConnectedState)
        return;

    QByteArray payload = message.toUtf8();
    quint32 length = qToBigEndian<quint32>(static_cast<quint32>(payload.size()));

    QByteArray packet;
    packet.append(reinterpret_cast<const char *>(&length), sizeof(length));
    packet.append(payload);

    m_socket->write(packet);
    m_socket->flush();
}

void SocketClient::onReadyRead()
{
    m_buffer.append(m_socket->readAll());

    while (true) {
        if (m_buffer.size() < 4)
            break;

        quint32 msgLength;
        memcpy(&msgLength, m_buffer.constData(), sizeof(msgLength));
        msgLength = qFromBigEndian<quint32>(msgLength);

        if (msgLength > 64 * 1024 * 1024) {
            qWarning() << "Message too large (" << msgLength << "bytes), disconnecting.";
            m_socket->disconnectFromHost();
            m_buffer.clear();
            return;
        }

        if (m_buffer.size() < static_cast<int>(4 + msgLength))
            break;

        QByteArray payload = m_buffer.mid(4, static_cast<int>(msgLength));
        m_buffer.remove(0, static_cast<int>(4 + msgLength));

        emit dataReceived(QString::fromUtf8(payload));
    }
}

void SocketClient::onConnected()
{
    qDebug() << "Connected!";
    m_buffer.clear();
    emit connectionStatusChanged(true);
}

void SocketClient::onDisconnected()
{
    qDebug() << "Disconnected!";
    m_buffer.clear();
    emit connectionStatusChanged(false);
}

void SocketClient::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
    qWarning() << "Socket error:" << m_socket->errorString();
    QMessageBox::warning(nullptr, "Server Error","Conexiunea la server nu a reusit");
}