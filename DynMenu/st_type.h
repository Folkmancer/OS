#pragma once
#include <fstream>

struct st_type // ��������� ��� �������� ���������� � ������� ����
{
	std::wstring str; // ������
	bool used; // �������������� ��
	int level; // ������� ������ (���-�� "-" ����� ��������� ������)
	int ID;// �������������, �� �������� ���������� ��������� � ������ ����
};