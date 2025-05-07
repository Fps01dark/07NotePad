#pragma once

#include "singleapplication.h"

class OneNotepadApplication : public SingleApplication
{
	Q_OBJECT
public:
	explicit OneNotepadApplication(int& argc, char** argv);
	~OneNotepadApplication();

private:
	void InitUi();
	void InitValue();
	void InitConnect();
};
