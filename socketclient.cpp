#include "socketclient.h"
#include <QDebug>

SocketClient::SocketClient(QObject *parent)
    : QObject(parent)
{
    m_socket = new QTcpSocket(this);

    // Conectăm semnalele interne ale socket-ului la sloturile noastre
    connect(m_socket, &QTcpSocket::connected, this, &SocketClient::onConnected);
    connect(m_socket, &QTcpSocket::disconnected, this, &SocketClient::onDisconnected);
    connect(m_socket, &QTcpSocket::readyRead, this, &SocketClient::onReadyRead);

    // Sintaxă modernă pentru erori
    connect(m_socket, &QTcpSocket::errorOccurred, this, &SocketClient::onErrorOccurred);
}

void SocketClient::connectToBackend(const QString &host, quint16 port)
{
    qDebug() << "Se încearcă conectarea la" << host << ":" << port;
    m_socket->connectToHost(host, port);
}

void SocketClient::sendMessage(const QString &message)
{
    if (m_socket->state() == QAbstractSocket::ConnectedState) {
        m_socket->write(message.toUtf8());
        m_socket->flush(); // Forțează trimiterea datelor din buffer
    }
}

void SocketClient::onReadyRead()
{
    // Citim tot ce a venit de la server
    QByteArray data = m_socket->readAll();
    QString message = QString::fromUtf8(data);

    //qDebug() << "Date primite:" << message;
    emit dataReceived(message); // Trimitem datele către restul aplicației
}

void SocketClient::onConnected()
{
    qDebug() << "Conectat!";
    emit connectionStatusChanged(true);
}

void SocketClient::onDisconnected()
{
    qDebug() << "Deconectat!";
    emit connectionStatusChanged(false);
}

void SocketClient::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
    qWarning() << "Eroare Socket:" << m_socket->errorString();
}