// Fifth.cpp: Определяет точку входа для приложения.
//

#include "stdafx.h"
#include "Fifth.h"
#include "cMainForm.h"

TState::TState(int FiledSize) {
	if ((FiledSize > 2) && (FiledSize < 6)) {
		this->Shuffle(FiledSize);
		this->SetDistance();
	}
	else this->Distance = -1;
}


TState::~TState() {
	for (auto Current = this->State.begin(); Current != this->State.end(); Current++) Current->clear();
	this->State.clear();
}

const bool TState::operator == (TState& second) {
	int SIZE = this->State.size();
	for (BYTE i = 0; i < SIZE; i++) {
		for (BYTE j = 0; j < SIZE; j++) {
			if (this->State[i][j] != second.State[i][j])
				return false;
		}
	}
	return true;
}

const bool TState::operator < (TState& second) {
	return (this->Distance + this->NumStep) < (second.Distance + second.NumStep);
}

const std::vector<BYTE> TState::operator [](int index) {
	return this->State[index];
}

void TState::Shuffle(int SIZE) {
	//Собранная позиция
	this->State.resize(SIZE, {});
	for (int i = 0; i < SIZE; i++) {
		this->State[i].resize(SIZE, 0);
		for (int j = 0; j < SIZE; j++)
			this->State[i][j] = i * SIZE + j + 1;
	}

	State[SIZE - 1][SIZE - 1] = 0;
	srand(time(0));
	int qSteps = rand() % 55 + 15; //Количество действий
	for (int i = 0; i < qSteps;) {
		SHIFT tShift = (SHIFT)(rand() % 4); //Куда двигаем
		if (this->Shift(this, tShift)) i++;
	}
	this->NumStep = 0;
}

void TState::SetDistance() {
	this->Distance = 0;
	int SIZE = this->State.size();
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			if (this->State[i][j]) {
				int x = ((this->State[i][j] - 1) / SIZE);
				int y = ((this->State[i][j] - 1) % SIZE);
				this->Distance += (abs(i - x) + abs(j - y));
			}
			else {
				this->Distance += (SIZE * 2 - 2 - i - j); // abs(SIZE - 1 -i) + abs(SIZE - 1 -j) ==> 2-i+2-j, т.к. i <=2  & j <= 2 ==> 4 - i - j
			}
		}
	}
}

POINT TState::FindZero() {
	POINT res = { -1, -1 };
	int SIZE = this->State.size();
	for (BYTE i = 0; i < SIZE; i++) {
		for (BYTE j = 0; j < SIZE; j++) if (!this->State[i][j]) {
			res.x = i;
			res.y = j;
			return res;
		}
	}
	return res;
}


bool TState::Shift(TState* fromState, SHIFT shift) {
	this->State = fromState->State;
	int SIZE = this->State.size() - 1;

	POINT ZeroPos = this->FindZero();
	if (ZeroPos.x == -1) return false;

	POINT changeShift = { 0,0 };
	switch (shift) {
		case LEFT:
			if (ZeroPos.x) changeShift.x = -1; //Если не самая левая колонка, то сдвиг по горизонтали на -1
			break;
		case RIGHT:
			if (ZeroPos.x < SIZE) changeShift.x = 1; //Если не самая правая колонка, то сдвиг по горизонтали на 1
			break;
		case UP:
			if (ZeroPos.y) changeShift.y = -1; //Если не самая верхняя строка, то сдвиг по вертикали на -1
			break;
		case DOWN:
			if (ZeroPos.y < SIZE) changeShift.y = 1; //Если не самая нижняя строка, то сдвиг по вертикали на 1
			break;
		default:
			return false;
	}

	//Если нельзя передвинуть (выходим за границы поля), то ЛОЖЬ
	if (!changeShift.x && !changeShift.y) return false; 

	this->State[ZeroPos.x][ZeroPos.y] = this->State[ZeroPos.x + changeShift.x][ZeroPos.y + changeShift.y];
	this->State[ZeroPos.x + changeShift.x][ZeroPos.y + changeShift.y] = 0;

	this->SetDistance();
	this->NumStep = fromState->NumStep + 10;
	return true;
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	cMainForm *MainForm = new cMainForm(hInstance);
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


}
