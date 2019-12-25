#ifndef __MAINWINDOWCLASS_H
#define __MAINWINDOWCLASS_H

#include "stdafx.h"
#include <map>

#pragma once
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

//Класс нашего приложения


class cMainWindowCalss 
{
	typedef LRESULT(cMainWindowCalss::*FuncPointer)(LPARAM, WPARAM);

	struct POINTER
	{
		cMainWindowCalss* Window;//Указатель на класс, которому принадлежит обработчик
		FuncPointer Function;
	};

protected:
	void DrawTransparentBitmap(HDC hdc, HBITMAP hBitmap, short xStart, short yStart, COLORREF cTransparentColor)
	{
		BITMAP     bm;
		COLORREF   cColor;
		HBITMAP    bmAndBack, bmAndObject, bmAndMem, bmSave;
		HGDIOBJ    bmBackOld, bmObjectOld, bmMemOld, bmSaveOld;
		HDC        hdcMem, hdcBack, hdcObject, hdcTemp, hdcSave;
		POINT      ptSize;

		hdcTemp = CreateCompatibleDC(hdc);
		SelectObject(hdcTemp, hBitmap);   // Выбираем битмап

		GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);
		ptSize.x = bm.bmWidth;            // Получаем ширину битмапа
		ptSize.y = bm.bmHeight;           // Получаем высоту битмапа
		DPtoLP(hdcTemp, &ptSize, 1);      // Конвертируем из координат
										  // устройства в логические
										  // точки

										  // Создаём несколько DC для хранения временных данных.
		hdcBack = CreateCompatibleDC(hdc);
		hdcObject = CreateCompatibleDC(hdc);
		hdcMem = CreateCompatibleDC(hdc);
		hdcSave = CreateCompatibleDC(hdc);

		// Создаём битмап для каждого DC.

		// Монохромный DC
		bmAndBack = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);

		// Монохромный DC
		bmAndObject = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);

		bmAndMem = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);
		bmSave = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);

		// В каждом DC должен быть выбран объект битмапа для хранения
		// пикселей.
		bmBackOld = SelectObject(hdcBack, bmAndBack);
		bmObjectOld = SelectObject(hdcObject, bmAndObject);
		bmMemOld = SelectObject(hdcMem, bmAndMem);
		bmSaveOld = SelectObject(hdcSave, bmSave);

		// Устанавливаем режим маппинга.
		SetMapMode(hdcTemp, GetMapMode(hdc));

		// Сохраняем битмап, переданный в параметре функции, так как
		// он будет изменён.
		BitBlt(hdcSave, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);

		// Устанавливаем фоновый цвет (в исходном DC) тех частей,
		// которые будут прозрачными.
		cColor = SetBkColor(hdcTemp, cTransparentColor);

		// Создаём маску для битмапа путём вызова BitBlt из исходного
		// битмапа на монохромный битмап.
		BitBlt(hdcObject, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0,
			SRCCOPY);

		// Устанавливаем фоновый цвет исходного DC обратно в
		// оригинальный цвет.
		SetBkColor(hdcTemp, cColor);

		// Создаём инверсию маски.
		BitBlt(hdcBack, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0,
			NOTSRCCOPY);

		// Копируем фон главного DC в конечный.
		BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdc, xStart, yStart,
			SRCCOPY);

		// Накладываем маску на те места, где будет помещён битмап.
		BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, SRCAND);

		// Накладываем маску на прозрачные пиксели битмапа.
		BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcBack, 0, 0, SRCAND);

		// XOR-им битмап с фоном на конечном DC.
		BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCPAINT);

		// Копируем на экран.
		BitBlt(hdc, xStart, yStart, ptSize.x, ptSize.y, hdcMem, 0, 0,
			SRCCOPY);

		// Помещаем оригинальный битмап обратно в битмап, переданный в
		// параметре функции.
		BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcSave, 0, 0, SRCCOPY);

		// Удаляем битмапы из памяти.
		DeleteObject(SelectObject(hdcBack, bmBackOld));
		DeleteObject(SelectObject(hdcObject, bmObjectOld));
		DeleteObject(SelectObject(hdcMem, bmMemOld));
		DeleteObject(SelectObject(hdcSave, bmSaveOld));

		// Удаляем DC из памяти.
		DeleteDC(hdcMem);
		DeleteDC(hdcBack);
		DeleteDC(hdcObject);
		DeleteDC(hdcSave);
		DeleteDC(hdcTemp);
	}


	HINSTANCE	Instance;
	HICON		Icon;
	HICON		IconSm;
	HMENU		Menu;
	HCURSOR		Cursor;

	HWND whWnd;//Хендл нашего окна
	std::map<UINT, POINTER> _msgmap;//Карта сообщений
public:

	//CreateW - Создание окна
	//	HWND ParentWND					- Родительское окно, если 0 - то главное окно
	//	LPCWSTR Title					- Заголовок окна
	//	DWORD Exstyle, Style			- Стили окна
	//	int Left, Top, Width, Height	- Размеры и положение
	//	UINT id							- Идентификатор окна
	//	LPCWSTR szWindowClass			- Класс окна

	bool CreateW(HWND ParentWND,LPCWSTR Title,DWORD Exstyle, DWORD Style, int Left, int Top, int Width, int Height,UINT id,LPCWSTR szWindowClass)
	{
		//Регистрируем класс окна
		WNDCLASSEX wndc;
		wndc.lpszClassName		= szWindowClass;
		wndc.cbSize				= sizeof(WNDCLASSEX);
		wndc.lpfnWndProc		= WNDPROC(_WndProc);	//Оконная процедура
		wndc.cbClsExtra			= 0;
		wndc.cbWndExtra			= 0;
		wndc.hbrBackground		= HBRUSH(COLOR_WINDOW+1);//Цвет фона окна
		wndc.hInstance			= Instance;
		wndc.hCursor			= Cursor;
		wndc.style				= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wndc.hIcon				= Icon;
		wndc.hIconSm			= IconSm;
		wndc.lpszMenuName		= 0;
		
		RegisterClassEx(&wndc);

		//Создаем само окно
		this->whWnd = CreateWindowExW(Exstyle, szWindowClass, Title,
			Style | WS_CLIPCHILDREN,//Стиль WS_CLIPCHILDREN нужен для того, чтобы дочерние контролы не мигали при перерисовке
			Left, Top, Width, Height, ParentWND, ((Menu == NULL) ? HMENU(id): Menu),
			this->Instance,
			this//Передаем в оконную функцию указатель на класс нашего окна
		);


		if (!this->whWnd) return false;
		return true;
	}

	// Оконная функция, в которую поступают сообщения для обработки
	static LRESULT CALLBACK _WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
	{
		cMainWindowCalss *CurrentWindow = 0;
		//Сообщения WM_NCCREATE приходит до WM_CREATE, т.е при создании окна
		if (message == WM_NCCREATE)
		{
			//Получаем указатель на экземпляр нашего окна, который мы передали в функцию CreateWindowEx
			CurrentWindow = (cMainWindowCalss*)LPCREATESTRUCT(lparam)->lpCreateParams;
			//И сохраняем в поле GWL_USERDATA
			SetWindowLong(hwnd, GWL_USERDATA, LONG(LPCREATESTRUCT(lparam)->lpCreateParams));
			CurrentWindow->whWnd = hwnd;
		}
		
		//Теперь получаем указатель на наш экземлпяр окна, но уже из поля GWL_USERDATA
		CurrentWindow = (cMainWindowCalss*)GetWindowLong(hwnd, GWL_USERDATA);
		if (CurrentWindow)
		{
			//Ищем сообщение в карте
			std::map<UINT, POINTER>::iterator it;
			it = CurrentWindow->_msgmap.find(message);

			//Если сообщение не найдено, то обрабатываем его по умолчанию
			if (it == CurrentWindow->_msgmap.end()) return DefWindowProc(hwnd, message, wparam, lparam);
			else
			{
				POINTER msg = it->second;
				//Вызываем функцию обработчик        
				LRESULT result = (msg.Window->*msg.Function)(lparam, wparam);
				if (result) return result;
			}
		}
		return DefWindowProc(hwnd, message, wparam, lparam);
	}


	template<typename T>
	bool AddMessage(UINT message, cMainWindowCalss* wnd, LRESULT(T::*funcpointer)(LPARAM, WPARAM))
	{
		if (!wnd || !funcpointer) return false;

		POINTER msg;
		msg.Window = wnd;
		msg.Function = reinterpret_cast<FuncPointer>(funcpointer);

		_msgmap.insert(std::pair<UINT, POINTER>(message, msg));

		return true;
	}

	cMainWindowCalss() {
		whWnd = 0;
		Instance = GetModuleHandle(0);
		Icon = IconSm = 0;
		Menu = NULL;
		Cursor = LoadCursor(0, IDC_ARROW);
	}

	~cMainWindowCalss() {
	};

	void SetInstance(HINSTANCE _Instance) { this->Instance = _Instance; }
	void SetIcons(HICON _icon = 0, HICON _hIconSm = 0) { this->Icon = _icon; this->IconSm = _hIconSm;  }
	void SetMenu(HMENU _menu) { this->Menu = _menu; }
	void SetCursor(HCURSOR _cursor) { this->Cursor = _cursor;  }
};

#endif