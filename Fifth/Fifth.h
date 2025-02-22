#pragma once

#ifndef __FIFTH__H
#define __FIFTH__H

#include<vector>
#include <time.h>

enum SHIFT { LEFT, UP, RIGHT, DOWN };

//Состояние поля
class TState {
private:
	std::vector <std::vector<BYTE>> State;

	int			Distance	= -1;		// Расстояние
	int			NumStep		= -1;		// НомерХода
	TState*		PrevSate	= NULL;		// Предыдущее состояние

	void Shuffle(int SIZE); //Берем собранную позицию и случайным образом передвигаем "косточки"
	void SetDistance(); //Рассчитываем расстояние
	POINT FindZero(); //Поиск координат пустой клетки

public:
	TState() {};
	TState(int FiledSize); 	//Конструктор для создания первоначальной позиции для решения. FiledSize - размер поля
	~TState();

	const bool operator == (TState& second); //Переопределения оператора ==. Если все позиции двух полей совпадают, то истина
	const bool operator < (TState& second); 	//Переопределения оператора <. Сравниваются "веса" положений
	const std::vector<BYTE> operator [](int index);

	bool Shift(TState* fromState, SHIFT shift); //Сдвиг "косточки". fromState - Предыдущая позиция. shift - куда двигаем

	bool empty() { return (this->Distance == -1); } //Проверка на пустое поле

	void SetPrev(TState* fromState) { this->PrevSate = fromState; } //Проверка на пустое поле
	TState* GetPrev() { return this->PrevSate; } //Получить предыдущее состояние

	void ChangeFromState(TState* fromState, SHIFT shift) { //Получить предыдущее состояние
		if (this->NumStep > fromState->NumStep + 10) { 
			this->NumStep = fromState->NumStep + 10;
			this->PrevSate = fromState;
		}
	}

	int GetDistanse() { return this->Distance; } //Получить расстояние
};

#endif