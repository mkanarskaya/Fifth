#ifndef __MAINWINDOWCLASS_H
#define __MAINWINDOWCLASS_H

#include "stdafx.h"
#include <map>

#pragma once
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

//����� ������ ����������


class cMainWindowCalss 
{
	typedef LRESULT(cMainWindowCalss::*FuncPointer)(LPARAM, WPARAM);

	struct POINTER
	{
		cMainWindowCalss* Window;//��������� �� �����, �������� ����������� ����������
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
		SelectObject(hdcTemp, hBitmap);   // �������� ������

		GetObject(hBitmap, sizeof(BITMAP), (LPSTR)&bm);
		ptSize.x = bm.bmWidth;            // �������� ������ �������
		ptSize.y = bm.bmHeight;           // �������� ������ �������
		DPtoLP(hdcTemp, &ptSize, 1);      // ������������ �� ���������
										  // ���������� � ����������
										  // �����

										  // ������ ��������� DC ��� �������� ��������� ������.
		hdcBack = CreateCompatibleDC(hdc);
		hdcObject = CreateCompatibleDC(hdc);
		hdcMem = CreateCompatibleDC(hdc);
		hdcSave = CreateCompatibleDC(hdc);

		// ������ ������ ��� ������� DC.

		// ����������� DC
		bmAndBack = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);

		// ����������� DC
		bmAndObject = CreateBitmap(ptSize.x, ptSize.y, 1, 1, NULL);

		bmAndMem = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);
		bmSave = CreateCompatibleBitmap(hdc, ptSize.x, ptSize.y);

		// � ������ DC ������ ���� ������ ������ ������� ��� ��������
		// ��������.
		bmBackOld = SelectObject(hdcBack, bmAndBack);
		bmObjectOld = SelectObject(hdcObject, bmAndObject);
		bmMemOld = SelectObject(hdcMem, bmAndMem);
		bmSaveOld = SelectObject(hdcSave, bmSave);

		// ������������� ����� ��������.
		SetMapMode(hdcTemp, GetMapMode(hdc));

		// ��������� ������, ���������� � ��������� �������, ��� ���
		// �� ����� ������.
		BitBlt(hdcSave, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCCOPY);

		// ������������� ������� ���� (� �������� DC) ��� ������,
		// ������� ����� �����������.
		cColor = SetBkColor(hdcTemp, cTransparentColor);

		// ������ ����� ��� ������� ���� ������ BitBlt �� ���������
		// ������� �� ����������� ������.
		BitBlt(hdcObject, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0,
			SRCCOPY);

		// ������������� ������� ���� ��������� DC ������� �
		// ������������ ����.
		SetBkColor(hdcTemp, cColor);

		// ������ �������� �����.
		BitBlt(hdcBack, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0,
			NOTSRCCOPY);

		// �������� ��� �������� DC � ��������.
		BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdc, xStart, yStart,
			SRCCOPY);

		// ����������� ����� �� �� �����, ��� ����� ������� ������.
		BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcObject, 0, 0, SRCAND);

		// ����������� ����� �� ���������� ������� �������.
		BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcBack, 0, 0, SRCAND);

		// XOR-�� ������ � ����� �� �������� DC.
		BitBlt(hdcMem, 0, 0, ptSize.x, ptSize.y, hdcTemp, 0, 0, SRCPAINT);

		// �������� �� �����.
		BitBlt(hdc, xStart, yStart, ptSize.x, ptSize.y, hdcMem, 0, 0,
			SRCCOPY);

		// �������� ������������ ������ ������� � ������, ���������� �
		// ��������� �������.
		BitBlt(hdcTemp, 0, 0, ptSize.x, ptSize.y, hdcSave, 0, 0, SRCCOPY);

		// ������� ������� �� ������.
		DeleteObject(SelectObject(hdcBack, bmBackOld));
		DeleteObject(SelectObject(hdcObject, bmObjectOld));
		DeleteObject(SelectObject(hdcMem, bmMemOld));
		DeleteObject(SelectObject(hdcSave, bmSaveOld));

		// ������� DC �� ������.
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

	HWND whWnd;//����� ������ ����
	std::map<UINT, POINTER> _msgmap;//����� ���������
public:

	//CreateW - �������� ����
	//	HWND ParentWND					- ������������ ����, ���� 0 - �� ������� ����
	//	LPCWSTR Title					- ��������� ����
	//	DWORD Exstyle, Style			- ����� ����
	//	int Left, Top, Width, Height	- ������� � ���������
	//	UINT id							- ������������� ����
	//	LPCWSTR szWindowClass			- ����� ����

	bool CreateW(HWND ParentWND,LPCWSTR Title,DWORD Exstyle, DWORD Style, int Left, int Top, int Width, int Height,UINT id,LPCWSTR szWindowClass)
	{
		//������������ ����� ����
		WNDCLASSEX wndc;
		wndc.lpszClassName		= szWindowClass;
		wndc.cbSize				= sizeof(WNDCLASSEX);
		wndc.lpfnWndProc		= WNDPROC(_WndProc);	//������� ���������
		wndc.cbClsExtra			= 0;
		wndc.cbWndExtra			= 0;
		wndc.hbrBackground		= HBRUSH(COLOR_WINDOW+1);//���� ���� ����
		wndc.hInstance			= Instance;
		wndc.hCursor			= Cursor;
		wndc.style				= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
		wndc.hIcon				= Icon;
		wndc.hIconSm			= IconSm;
		wndc.lpszMenuName		= 0;
		
		RegisterClassEx(&wndc);

		//������� ���� ����
		this->whWnd = CreateWindowExW(Exstyle, szWindowClass, Title,
			Style | WS_CLIPCHILDREN,//����� WS_CLIPCHILDREN ����� ��� ����, ����� �������� �������� �� ������ ��� �����������
			Left, Top, Width, Height, ParentWND, ((Menu == NULL) ? HMENU(id): Menu),
			this->Instance,
			this//�������� � ������� ������� ��������� �� ����� ������ ����
		);


		if (!this->whWnd) return false;
		return true;
	}

	// ������� �������, � ������� ��������� ��������� ��� ���������
	static LRESULT CALLBACK _WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
	{
		cMainWindowCalss *CurrentWindow = 0;
		//��������� WM_NCCREATE �������� �� WM_CREATE, �.� ��� �������� ����
		if (message == WM_NCCREATE)
		{
			//�������� ��������� �� ��������� ������ ����, ������� �� �������� � ������� CreateWindowEx
			CurrentWindow = (cMainWindowCalss*)LPCREATESTRUCT(lparam)->lpCreateParams;
			//� ��������� � ���� GWL_USERDATA
			SetWindowLong(hwnd, GWL_USERDATA, LONG(LPCREATESTRUCT(lparam)->lpCreateParams));
			CurrentWindow->whWnd = hwnd;
		}
		
		//������ �������� ��������� �� ��� ��������� ����, �� ��� �� ���� GWL_USERDATA
		CurrentWindow = (cMainWindowCalss*)GetWindowLong(hwnd, GWL_USERDATA);
		if (CurrentWindow)
		{
			//���� ��������� � �����
			std::map<UINT, POINTER>::iterator it;
			it = CurrentWindow->_msgmap.find(message);

			//���� ��������� �� �������, �� ������������ ��� �� ���������
			if (it == CurrentWindow->_msgmap.end()) return DefWindowProc(hwnd, message, wparam, lparam);
			else
			{
				POINTER msg = it->second;
				//�������� ������� ����������        
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