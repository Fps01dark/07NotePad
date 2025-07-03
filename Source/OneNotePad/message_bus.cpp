#include "message_bus.h"

#include <QList>
#include <QMap>
#include <QString>
#include <functional>
#include <memory>

// 注册消息类型的订阅者
void MessageBus::Subscribe(const QString& messageType, std::function<void()> handler)
{
	m_voidVoidsubscribers[messageType].push_back(handler);
}
void MessageBus::Publish(const QString& messageType)
{
	// 查找该类型的订阅者，并触发他们的回调
	if (m_voidVoidsubscribers.find(messageType) != m_voidVoidsubscribers.end())
	{
		for (auto& handler : m_voidVoidsubscribers[messageType])
		{
			handler();  // 执行订阅者的回调
		}
	}
}

// 注册消息类型的订阅者
void MessageBus::Subscribe(const QString& messageType, std::function<void(int)> handler)
{
	m_voidIntsubscribers[messageType].push_back(handler);
}
void MessageBus::Publish(const QString& messageType, const int& data)
{
	if (m_voidIntsubscribers.find(messageType) != m_voidIntsubscribers.end())
	{
		for (auto& handler : m_voidIntsubscribers[messageType])
		{
			handler(data);  // 执行订阅者的回调
		}
	}
}

// 注册消息类型的订阅者
void MessageBus::Subscribe(const QString& messageType, std::function<void(int, int)> handler)
{
	m_voidIntIntsubscribers[messageType].push_back(handler);
}
void MessageBus::Publish(const QString& messageType, const int& data1, const int& data2)
{
	if (m_voidIntIntsubscribers.find(messageType) != m_voidIntIntsubscribers.end())
	{
		for (auto& handler : m_voidIntIntsubscribers[messageType])
		{
			handler(data1, data2);  // 执行订阅者的回调
		}
	}
}

// 注册消息类型的订阅者
void MessageBus::Subscribe(const QString& messageType, std::function<void(const QString&)> handler)
{
	m_voidQStringSubscribers[messageType].push_back(handler);
}
void MessageBus::Publish(const QString& messageType, const QString& data)
{
	if (m_voidQStringSubscribers.find(messageType) != m_voidQStringSubscribers.end())
	{
		for (auto& handler : m_voidQStringSubscribers[messageType])
		{
			handler(data);  // 执行订阅者的回调
		}
	}
}

// 注册消息类型的订阅者
void MessageBus::Subscribe(const QString& messageType, std::function<void(const QStringList&)> handler)
{
	m_voidQStringListSubscribers[messageType].push_back(handler);
}
void MessageBus::Publish(const QString& messageType, const QStringList& data)
{
	if (m_voidQStringListSubscribers.find(messageType) != m_voidQStringListSubscribers.end())
	{
		for (auto& handler : m_voidQStringListSubscribers[messageType])
		{
			handler(data);  // 执行订阅者的回调
		}
	}
}

// 注册消息类型的订阅者
void MessageBus::Subscribe(const QString& messageType, std::function<void(OnTextEdit*)> handler)
{
	m_voidCustomTextEditSubscribers[messageType].push_back(handler);
}
void MessageBus::Publish(const QString& messageType, OnTextEdit* data)
{
	if (m_voidCustomTextEditSubscribers.find(messageType) != m_voidCustomTextEditSubscribers.end())
	{
		for (auto& handler : m_voidCustomTextEditSubscribers[messageType])
		{
			handler(data);  // 执行订阅者的回调
		}
	}
}