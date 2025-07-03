#include "on_settings.h"

#include <QBitArray>

OnSettings::OnSettings(QObject* parent)
	:QSettings(parent)
{
}

OnSettings::~OnSettings()
{
}

void OnSettings::SetBoolList(const QString& key, const QList<bool>& boolList)
{
	// 使用 QBitArray 来表示布尔列表
	QBitArray bitArray(boolList.size());
	for (int i = 0; i < boolList.size(); ++i)
	{
		bitArray.setBit(i, boolList[i]);
	}

	setValue(key, bitArray);
}

QList<bool> OnSettings::BoolList(const QString& key)
{
	// 从 QSettings 中加载 QBitArray
	QBitArray   bitArray = value(key).value<QBitArray>();
	QList<bool> boolList;
	for (int i = 0; i < bitArray.size(); ++i)
	{
		boolList.append(bitArray.testBit(i));
	}
	return boolList;
}