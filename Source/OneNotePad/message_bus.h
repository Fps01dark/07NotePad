#pragma once

#include <QList>
#include <QMap>
#include <QString>
#include <functional>
#include <memory>

class CustomTextEdit;

class MessageBus
{
public:
	// 注册消息类型的订阅者
	void Subscribe(const QString& messageType, std::function<void()> handler);
	void Publish(const QString& messageType);

	// 注册消息类型的订阅者
	void Subscribe(const QString& messageType, std::function<void(int)> handler);
	void Publish(const QString& messageType, const int& data);

	// 注册消息类型的订阅者
	void Subscribe(const QString& messageType, std::function<void(int, int)> handler);
	void Publish(const QString& messageType, const int& data1, const int& data2);

	// 注册消息类型的订阅者
	void Subscribe(const QString& messageType, std::function<void(const QString&)> handler);
	void Publish(const QString& messageType, const QString& data);

	// 注册消息类型的订阅者
	void Subscribe(const QString& messageType, std::function<void(const QStringList&)> handler);
	void Publish(const QString& messageType, const QStringList& data);

	// 注册消息类型的订阅者
	void Subscribe(const QString& messageType, std::function<void(CustomTextEdit*)> handler);
	void Publish(const QString& messageType, CustomTextEdit* data);

private:
	// 存储每种消息类型对应的所有订阅者回调
	QMap<QString, QList<std::function<void()>>> m_voidVoidsubscribers;
	QMap<QString, QList<std::function<void(bool)>>> m_voidBoolsubscribers;
	QMap<QString, QList<std::function<void(int)>>> m_voidIntsubscribers;
	QMap<QString, QList<std::function<void(int, int)>>> m_voidIntIntsubscribers;
	QMap<QString, QList<std::function<void(const QString&)>>> m_voidQStringSubscribers;
	QMap<QString, QList<std::function<void(const QStringList&)>>> m_voidQStringListSubscribers;
	QMap<QString, QList<std::function<void(CustomTextEdit*)>>> m_voidCustomTextEditSubscribers;
};