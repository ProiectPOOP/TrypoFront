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

// FIX: Prefix every outgoing message with a 4-byte big-endian length,
// matching the server's recvAll() which reads the length header first.
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

// FIX: The old onReadyRead() called readAll() and emitted immediately.
// This breaks with length-prefixed framing because TCP can deliver data
// in fragments — readAll() may return only part of a message, or multiple
// messages merged together.
// Now we buffer incoming bytes and only emit complete messages.
void SocketClient::onReadyRead()
{
    m_buffer.append(m_socket->readAll());

    // Process as many complete messages as are available in the buffer
    while (true) {
        // Need at least 4 bytes for the length header
        if (m_buffer.size() < 4)
            break;

        // Peek at the length without consuming it yet
        quint32 msgLength;
        memcpy(&msgLength, m_buffer.constData(), sizeof(msgLength));
        msgLength = qFromBigEndian<quint32>(msgLength);

        // Sanity check: reject absurdly large messages (64 MB cap, same as server)
        if (msgLength > 64 * 1024 * 1024) {
            qWarning() << "Message too large (" << msgLength << "bytes), disconnecting.";
            m_socket->disconnectFromHost();
            m_buffer.clear();
            return;
        }

        // Wait until the full message body has arrived
        if (m_buffer.size() < static_cast<int>(4 + msgLength))
            break;

        // Extract the complete message
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