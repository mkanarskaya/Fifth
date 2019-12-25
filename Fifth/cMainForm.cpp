#include "stdafx.h"

#define ID_CLOSE	 WM_USER + 1
#define ID_SHIFT	 WM_USER + 2
#define ID_NOACT	 WM_USER + 3

#include "cMainForm.h"
#include <string>

cMainForm::cMainForm(HINSTANCE hInstance) {
	HDC hDCScreen = GetDC(NULL);//Значения на всем экране
	int Horres = GetDeviceCaps(hDCScreen, HORZRES), Vertres = GetDeviceCaps(hDCScreen, VERTRES);//Размеры экрана
	ReleaseDC(NULL, hDCScreen);


	AddMessage(WM_CREATE, this, &cMainForm::OnCreate);
	AddMessage(WM_DESTROY, this, &cMainForm::OnDestroy);
	AddMessage(WM_PAINT, this, &cMainForm::OnPaint);
	AddMessage(WM_COMMAND, this, &cMainForm::OnCommand);
	AddMessage(WM_LBUTTONDBLCLK, this, &cMainForm::OnLDBLClick);
	AddMessage(WM_LBUTTONDOWN, this, &cMainForm::OnLClick);


	int _with = this->fieldSize * 42 + 165;
	int _height = this->fieldSize * 42 + 50;

	this->SetInstance(hInstance);
	wchar_t *szWindowClass = new wchar_t[MAX_LOADSTRING];										// имя класса главного окна
	this->CreateW(0, L"Пятнашки", 0, WS_POPUP | WS_OVERLAPPED | WS_VISIBLE, (Horres - _with) / 2, (Vertres - _height)/ 2, _with, _height, 0, szWindowClass);
	delete[] szWindowClass;
}

cMainForm::~cMainForm() {
	while (!this->SolverSteps.empty()) {
		this->SolverSteps.pop();
	}
}

void cMainForm::ProcessMessage() {
	MSG msg;
	if (PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE)) {
		if (GetMessage(&msg, NULL, NULL, NULL)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

void cMainForm::Resize(int NewSize) {
	if (NewSize == this->fieldSize) return;
	for (int i = 0; i < this->fieldSize; i++) {
		for (int j = 0; j < this->fieldSize; j++)
			DestroyWindow(this->Buttons[i][j]);
		this->Buttons[i].clear();
	}
	this->Buttons.clear();

	this->fieldSize = NewSize;

	int _with = this->fieldSize * 42 + 165;
	int _height = this->fieldSize * 42 + 50;

	RECT rect;
	GetWindowRect(this->whWnd, &rect);
	MoveWindow(this->whWnd, rect.left, rect.top, _with, _height, false);//Изменяет размер и положение окна

	_with = this->fieldSize * 42 + 35;
	MoveWindow(this->wFiledSize, _with + 82, 35, 20, 20, true);
	MoveWindow(this->wShift, _with, 42 * 1 + 25, 100, 40, true);
	MoveWindow(this->wClose, _with, 42 * 2 + 25, 100, 40, true);

	this->Buttons.resize(this->fieldSize, {});
	for (BYTE i = 0; i < this->fieldSize; i++) {
		for (BYTE j = 0; j < this->fieldSize; j++) {
			POINT Coordinates = { 42 * i, 42 * j };
			HWND newButton = CreateWindow(L"BUTTON", L"", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, Coordinates.y + 25, Coordinates.x + 25, 40, 40, this->whWnd, reinterpret_cast<HMENU>(ID_NOACT), this->Instance, NULL);
			this->Buttons[i].push_back(newButton);
			if ((i == this->fieldSize - 1) && (j == this->fieldSize - 1)) this->SetTextPosition(i, j, 0);
			else this->SetTextPosition(i, j, i * this->fieldSize + j + 1);
		}
	}

	InvalidateRect(this->whWnd, NULL, TRUE);//перерисовывает окно
	UpdateWindow(this->whWnd);
}

void cMainForm::DrawCurrentState(TState Current) {
	for (int i = 0; i < this->fieldSize; i++)
		for (int j = 0; j < this->fieldSize; j++)
			SetTextPosition(i, j, Current[i][j]);
}

void cMainForm::NexStep() {//кнопка новая, ее вывод
	if (this->SolverSteps.empty()) return;
	TState Current = this->SolverSteps.top();
	this->SolverSteps.pop();
	this->DrawCurrentState(Current);
	if (this->SolverSteps.empty()) {
		this->condition = NEW;
		SetWindowText(this->wShift, L"Новая");
	}
}

void cMainForm::Solver() {
	while (!this->SolverSteps.empty()) {
		this->SolverSteps.pop();
	}

	const auto comparef = [](TState *lhs, TState *rhs) { return (*lhs) < (*rhs); };
	std::multiset<TState *, decltype(comparef)> open(comparef);
	std::list<TState *> close;

	TState *Current = new TState(this->fieldSize); // Получаем первую позицию
	if (Current->empty()) {
		delete Current;
		Current = NULL;
		return;
	}

	this->condition = PROCESS;
	SetWindowText(this->wShift, L"Рассчет");
	open.insert(Current);
	for (;;) {
		this->ProcessMessage();//можно двигать окно, пока считает 
		Current = (*open.begin());
		open.erase(open.begin());
		if (Current->GetDistanse() == 0) { //  Проверяем не является ли позиция собранной 
			this->condition = OUTSOLVER;
			SetWindowText(this->wShift, L"Следующий");
			while (Current) {
				this->SolverSteps.push(*Current);
				Current = Current->GetPrev();
			}
			this->NexStep();
			break;
		}

		close.push_back(Current);

		// Находим все позиции, в которые можно перейти за один ход
		for (char i = 0; i < 4; i++) {
			this->ProcessMessage();
			TState *Neighbor = new TState;
			if (!Neighbor->Shift(Current, static_cast<SHIFT>(i))) {
				delete Neighbor; continue;
			}
			if (*Neighbor == *Current) {
				delete Neighbor;  continue;
			}//, кроме позиции, из которой была получена данная

			this->ProcessMessage();
			for (auto CurPos = close.begin(); CurPos != close.end(); CurPos++) {
				if (**CurPos == *Neighbor) {
					delete Neighbor;
					Neighbor = NULL;
					break;
				}
			}
			if (!Neighbor) continue;

			this->ProcessMessage();
			auto CurPos = open.begin();
			for (; CurPos != open.end(); CurPos++) {
				if (**CurPos == *Neighbor) {
					Neighbor->ChangeFromState(Current, static_cast<SHIFT>(i));
					break;
				}
			}
			if (CurPos == open.end()) {
				Neighbor->SetPrev(Current);
				open.insert(Neighbor);
			}
		}

		if (open.empty()) {
			//Если ходить некуда значит собрать нельзя
			this->condition = NEW;
			break;
		}
	}

	for (auto CurPos = open.begin(); CurPos != open.end(); CurPos++) {
		TState *CurDel = *CurPos;
		delete CurDel;
	}

	for (auto CurPos = close.begin(); CurPos != close.end(); CurPos++) {
		TState *CurDel = *CurPos;
		delete CurDel;
	}

}

LRESULT cMainForm::OnCommand(LPARAM lparam, WPARAM wparam) {
	switch (wparam) {
	case ID_CLOSE: {
		if(this->condition != PROCESS) SendMessage(this->whWnd, WM_CLOSE, 0, 0);
		break;
	}
	case ID_SHIFT: {
		if (this->condition == NEW) {
			wchar_t text[2];
			GetWindowText(this->wFiledSize, text, 2);
			int NewSize = 0;
			switch (text[0])
			{
			case '3': NewSize = 3; break;
			case '4': NewSize = 4; break;
			case '5': NewSize = 5; break;
			default:
				break;
			}
			if (NewSize) {
				this->Resize(NewSize);
				this->Solver();
			}
		}
		else if (this->condition == OUTSOLVER) this->NexStep();
		break;
	}
	}
	return 0;
}

LRESULT cMainForm::OnLClick(LPARAM lparam, WPARAM wparam) {
	SendMessage(this->whWnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
	return 0;
}

LRESULT cMainForm::OnCreate(LPARAM lparam, WPARAM wparam) {
	this->Buttons.resize(3, {});
	for(BYTE i = 0; i < 3; i++) {
		for (BYTE j = 0; j < 3; j++) {
			POINT Coordinates = { 42 * i, 42 * j };
			HWND newButton = CreateWindow(L"button", L"", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, Coordinates.y + 25, Coordinates.x + 25, 40, 40, this->whWnd, reinterpret_cast<HMENU>(ID_NOACT), this->Instance, NULL);
			this->Buttons[i].push_back(newButton);
			if((i == 2) && (j == 2)) this->SetTextPosition(i, j, 0);
			else this->SetTextPosition(i, j, i * 3 + j + 1);
		}
	}

	int _with = this->fieldSize * 42 + 35;
	this->wFiledSize = CreateWindow(L"EDIT", L"3", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, _with + 82, 35, 20, 20, this->whWnd, 0, this->Instance, NULL);
	this->wShift = CreateWindow(L"BUTTON", L"Новая", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, _with, 42 * 1 + 25, 100, 40, this->whWnd, reinterpret_cast<HMENU>(ID_SHIFT), this->Instance, NULL);
	this->wClose = CreateWindow(L"BUTTON", L"Закрыть", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, _with, 42 * 2 + 25, 100, 40, this->whWnd, reinterpret_cast<HMENU>(ID_CLOSE), this->Instance, NULL);
	return 0;
}

LRESULT cMainForm::OnDestroy(LPARAM lparam, WPARAM wparam) {
	PostQuitMessage(0);
	return 0;
}

LRESULT cMainForm::OnLDBLClick(LPARAM lparam, WPARAM wparam) {
	return 0;
}

LRESULT cMainForm::OnPaint(LPARAM lparam, WPARAM wparam) {
		#define elroundAbout 100
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(this->whWnd, &ps);


		HDC hDC = GetDC(this->whWnd);

		RECT wRect;
		GetClientRect(this->whWnd, &wRect);

		HRGN rgn = CreateRoundRectRgn(0, 0, wRect.right, wRect.bottom, elroundAbout, elroundAbout);
		SetWindowRgn(whWnd, rgn, TRUE);

		HBRUSH Brush = CreateSolidBrush(RGB(255, 255, 204));
		FillRect(hdc, &wRect, Brush);
		DeleteObject(Brush);

		HRGN rgn1 = CreateRoundRectRgn(0, 0, wRect.right, wRect.bottom, elroundAbout, elroundAbout);
		FrameRgn(hDC, rgn1, CreateSolidBrush(RGB(245,245,0)), 1, 1);//рамка

		int _with = this->fieldSize * 42 + 25 + 10;
		SetBkColor(hDC, RGB(255, 255, 204));
		TextOut(hDC, _with / 2 + 20, 5, L"Пятнашки", 8);
		TextOut(hDC, _with, 35, L"Количество ", 11);
		EndPaint(this->whWnd, &ps);
		return 0;
}

void cMainForm::SetTextPosition(int i, int j, BYTE value) {
	std::wstring strValue = value?std::to_wstring(value):L"";
	SetWindowText(this->Buttons[i][j], strValue.c_str());
}

