#pragma once

#include <functional>
#include <memory>

#include <QString>
#include <QMap>
#include <QList>

class MessageBus
{
public:
	// 注册消息类型的订阅者
	void Subscribe(const QString& messageType, std::function<void()> handler)
	{
		m_voidVoidsubscribers[messageType].push_back(handler);
	}
	// 注册消息类型的订阅者
	void Subscribe(const QString& messageType, std::function<void(int)> handler)
	{
		m_voidIntsubscribers[messageType].push_back(handler);
	}
	// 注册消息类型的订阅者
	void Subscribe(const QString& messageType, std::function<void(const QString&)> handler)
	{
		m_voidQStringSubscribers[messageType].push_back(handler);
	}
	// 注册消息类型的订阅者
	void Subscribe(const QString& messageType, std::function<void(const QStringList&)> handler)
	{
		m_voidQStringListSubscribers[messageType].push_back(handler);
	}

	// 发布消息到消息总线
	void Publish(const QString& messageType)
	{
		// 查找该类型的订阅者，并触发他们的回调
		if (m_voidVoidsubscribers.find(messageType) != m_voidVoidsubscribers.end())
		{
			for (auto& handler : m_voidVoidsubscribers[messageType])
			{
				handler(); // 执行订阅者的回调
			}
		}
	}
	// 发布消息到消息总线
	void Publish(const QString& messageType, const int& data)
	{
		if (m_voidIntsubscribers.find(messageType) != m_voidIntsubscribers.end())
		{
			for (auto& handler : m_voidIntsubscribers[messageType])
			{
				handler(data); // 执行订阅者的回调
			}
		}
	}
	// 发布消息到消息总线
	void Publish(const QString& messageType, const QString& data)
	{
		if (m_voidQStringSubscribers.find(messageType) != m_voidQStringSubscribers.end())
		{
			for (auto& handler : m_voidQStringSubscribers[messageType])
			{
				handler(data); // 执行订阅者的回调
			}
		}
	}
	// 发布消息到消息总线
	void Publish(const QString& messageType, const QStringList& data)
	{
		if (m_voidQStringListSubscribers.find(messageType) != m_voidQStringListSubscribers.end())
		{
			for (auto& handler : m_voidQStringListSubscribers[messageType])
			{
				handler(data); // 执行订阅者的回调
			}
		}
	}

private:
	// 存储每种消息类型对应的所有订阅者回调
	QMap<QString, QList<std::function<void()>>> m_voidVoidsubscribers;
	QMap<QString, QList<std::function<void(int)>>> m_voidIntsubscribers;
	QMap<QString, QList<std::function<void(const QString&)>>> m_voidQStringSubscribers;
	QMap<QString, QList<std::function<void(const QStringList&)>>> m_voidQStringListSubscribers;
};