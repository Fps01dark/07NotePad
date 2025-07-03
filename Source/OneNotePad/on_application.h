#pragma once

#include "singleapplication.h"

class OnApplication : public SingleApplication
{
	Q_OBJECT
public:
	explicit OnApplication(int& argc, char** argv);
	~OnApplication();

private:
	void InitUi();
	void InitValue();
	void InitConnect();
};
