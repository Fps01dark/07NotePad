#include "custom_settings.h"

#include "framework.h"

CustomSettings::CustomSettings(QObject* parent)
{
}

CustomSettings::~CustomSettings()
{
}

void CustomSettings::SetBoolList(const QString& key, const QList<bool>& boolList)
{
	// ʹ�� QBitArray ����ʾ�����б�
	QBitArray bitArray(boolList.size());
	for (int i = 0; i < boolList.size(); ++i)
	{
		bitArray.setBit(i, boolList[i]);
	}

	setValue(key, bitArray);
}

QList<bool> CustomSettings::BoolList(const QString& key)
{
	// �� QSettings �м��� QBitArray
	QBitArray bitArray = value(key).value<QBitArray>();
	QList<bool> boolList;
	for (int i = 0; i < bitArray.size(); ++i)
	{
		boolList.append(bitArray.testBit(i));
	}
	return boolList;
}