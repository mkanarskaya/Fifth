#pragma once
#include "cMainWindowCalss.h"
#include <Windows.h>
#include <shellapi.h>
#include <queue>
#include<list>
#include <set>
#include "Fifth.h"
#include <stack>

#define	  MAX_LOADSTRING 100

enum econdition {NEW, PROCESS, OUTSOLVER};

class cMainForm :
	public cMainWindowCalss
{
private:
	BYTE fieldSize = 3;
	econdition condition = NEW;
	std::stack<TState>	SolverSteps;

	void NexStep();
	void Solver();

	void SetTextPosition(int i, int j, BYTE value);
	void DrawCurrentState(TState Current);
	std::vector<std::vector<HWND>>	Buttons = {};
	HWND	wFiledSize, wShift, wClose;

	void ProcessMessage();
	void Resize(int NewSize);

protected:
	NOTIFYICONDATA	icData;
public:
	cMainForm(HINSTANCE hInstance);
	~cMainForm();

	LRESULT OnCreate(LPARAM lparam, WPARAM wparam);
	LRESULT OnDestroy(LPARAM lparam, WPARAM wparam);
	LRESULT OnPaint(LPARAM lparam, WPARAM wparam);
	LRESULT OnCommand(LPARAM lparam, WPARAM wparam);
	LRESULT OnLDBLClick(LPARAM lparam, WPARAM wparam);
	LRESULT OnLClick(LPARAM lparam, WPARAM wparam);

};


