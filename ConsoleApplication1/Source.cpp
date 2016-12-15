#include <iostream>
#include <cstdio>
#include "bmpToArr.h"
#include <vector>
#include <map>
#include <Windows.h>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <string>

using namespace std;

char name[1000] = "i.bmp";
map <int, int> mapCount; //���-�� ������������� �������
map <int, double> mapFreq; //������� �� �������� ���������� �������� � ������ �������������
vector<pair<int, double>> vecFreq; // ������ ��� ������ - �������������

//���������� ��� ���������� ��� ������-�������
bool cmp(pair <int, double> a, pair <int, double> b) {
	return a.second == b.second ? a.first < b.first : a.second > b.second;
}
//������� ��� ������� ������� c ��������� 
void debugv(vector<int> v) {
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 8; j++)
			printf("%3d:%3d | ", j * 16 + i + 1, v[j * 16 + i]);
		printf("\n");
	}
	printf("\n");
}
//������� ��� �������� ������� � ������
char* str(vector<int> v) {
	char* res = (char*)malloc(v.size());
	for (int i = 0; i < v.size(); i++)
		res[i] = char(v[i] + '0');
	res[v.size()] = '\0';
	return res;
}


//������� ��� ��������������� ����;
map <int, vector<int>> mapUni;


//������-����
vector<pair<int, vector<int>>> vecShen; //������ � ������
map<int, vector<int>> mapShen; //������� � ������
void Shen(int up, int down) {
	double sumDown = 0, sumUp = 0;
	for (int i = up; i < down; i++)
		sumDown += vecFreq[i].second;
	int mid = up;
	double dif = sumDown;
	//������� �����, �� ������� ������� �������
	for (int i = up; i < down; i++) {
		sumUp += vecFreq[i].second;
		sumDown -= vecFreq[i].second;
		if (sumUp >= sumDown) {
			if (dif > abs(sumUp - sumDown))
				mid = i + 1;
			else
				mid = i;
			break;
		}
		dif = abs(sumUp - sumDown);
	}
	//������� � ����� ������ ����� �������, ������ - ���� 
	for (int i = up; i < mid; i++)
		vecShen[i].second.push_back(1);
	for (int i = mid; i < down; i++)
		vecShen[i].second.push_back(0);
	//���������� ���������� �� ����� ������, ���� �� ������ > 1
	if (mid - up > 1)
		Shen(up, mid);
	if (down - mid > 1)
		Shen(mid, down);
}
void resShen() {
	for (auto it : vecShen)
		mapShen[it.first] = it.second;
}


//�������
struct point {
	int symbol; // -1, ���� �� ����
	double val;
	bool used;
	int l, r;  // -1, ���� ����
};
vector<point> hf;
//���������� �������
int Huf() {
	hf.resize(vecFreq.size());
	for (int i = 0; i < vecFreq.size(); i++) {
		hf[i].symbol = vecFreq[i].first;
		hf[i].used = 0;
		hf[i].val = vecFreq[i].second;
		hf[i].l = hf[i].r = -1;
	}
	int countNotUsed = vecFreq.size();
	while (true) {
		//���� ������ �������
		int min1 = -1;
		for (int i = 0; i < hf.size(); i++)
			if (!hf[i].used && (min1 == -1 || hf[i].val < hf[min1].val))
				min1 = i;
		hf[min1].used = 1;
		//���� ������ �������
		int min2 = -1;
		for (int i = 0; i < hf.size(); i++)
			if (!hf[i].used && (min2 == -1 || hf[i].val < hf[min2].val))
				min2 = i;
		if (min2 == -1)
			return min1;
		hf[min2].used = 1;
		//��������� ����� ����
		point newPoint;
		newPoint.symbol = -1;
		newPoint.l = min1;
		newPoint.r = min2;
		newPoint.val = hf[min1].val + hf[min2].val;
		newPoint.used = 0;
		hf.push_back(newPoint);
	}
}
vector<pair<vector<int>, int>> vecHuf; //������ � ������
map<int, vector<int>> mapHuf; //������� � ������
vector<int> now;
//������� ������� � ������� � ������
void resHuf(int num) {
	if (hf[num].symbol != -1) {
		vecHuf.push_back(make_pair(now, hf[num].symbol));
		mapHuf[hf[num].symbol] = now;
		return;
	}
	//����������� �� ������ �����
	now.push_back(0);
	resHuf(hf[num].l);
	now.pop_back();

	//����������� �� ������� �����
	now.push_back(1);
	resHuf(hf[num].r);
	now.pop_back();
}


int main() {
	setlocale(LC_ALL, "Russian");
	SetConsoleOutputCP(1252); // ��������� ������� �������� win-cp 1251 � ����� ������ (���������)
	freopen("output.txt", "w", stdout);
	vector<vector<int>> arr = bmpToArr(name); // ��������� ������ �������� ������
	vector<int> v(128); // ������ �� �������� �� ��������� (������� ���-�� ������ �������� ������)

						//����������� �������� ������� v
	for (int i = 0; i < 128; i++)
		v[i] = arr[i][0] * 0.299 + arr[i][1] * 0.587 + arr[i][2] * 0.114;
	debugv(v);

	for (int i = 0; i < 128; i++) {
		//�������� �� ������������ �������
		v[i] = (v[i] / 20) * 20;
		//��������� ������� 
		mapFreq[v[i]] += 1.0 / 128;
	}
	debugv(v);

	//����� ������� �������
	printf("�������:\n");
	for (auto it : mapFreq)
		printf("%3d:%1.3lf|", it.first, it.second);
	printf("\n");

	//�������������� ������� � ������
	printf("��������� �� ���������������� ������� �������������:\n");
	for (auto it : mapFreq)
		vecFreq.push_back(make_pair(it.first, it.second));
	sort(vecFreq.begin(), vecFreq.end(), cmp); //��������� �� ���������������� ������� �������������
	for (auto it : vecFreq)
		printf("%3d: %lf\n", it.first, it.second);

	//������� ����� ��������� ����
	printf("������� ����� ��������� ����:");
	double averageLen = 0;
	for (auto it : vecFreq)
		averageLen += -it.second * log2(it.second);
	printf("%lf\n", averageLen);

	//�������� ����������� �������������� ���
	printf("�������� ����������� �������������� ���\n");
	int k = 1;
	int st = 0; //2^st - ������ ����� >= frequency.size() 
	while (k < vecFreq.size())
		k *= 2, st++;
	int kod = 0;
	for (auto it : vecFreq) {
		printf("%3d: ", it.first);
		//����� ������������ ����
		for (int i = st - 1; i >= 0; i--) {
			int s = bool((1 << i) & kod); //��������� ������ � ����
			printf("%d", s);
			mapUni[it.first].push_back(s); //���������� �������
		}
		printf("\n");
		kod++;
	}

	//������-����
	//������� ������ � ����������� ���������
	vecShen.resize(vecFreq.size());
	for (int i = 0; i < vecFreq.size(); i++)
		vecShen[i].first = vecFreq[i].first;
	Shen(0, vecFreq.size()); //������� ������� � ������
	resShen(); //������� ������� � ������
			   //����� ����������
	printf("������-����\n");
	for (int i = 0; i < vecShen.size(); i++) {
		printf("%3d (%1.5lf): ", vecShen[i].first, vecFreq[i].second);
		for (int j = 0; j < vecShen[i].second.size(); j++)
			printf("%d", vecShen[i].second[j]);
		printf("\n");
	}

	//�������
	int num = Huf(); //���������� �������
					 //����� �������
	printf("num sym   val  l  r\n");
	for (int i = 0; i < hf.size(); i++) {
		printf("%3d %3d %1.3lf %2d %2d\n", i, hf[i].symbol, hf[i].val, hf[i].l, hf[i].r);
	}
	resHuf(num); //���������� ������� � ������� 
				 //����� �����
	sort(vecHuf.begin(), vecHuf.end());
	for (auto it : vecHuf) {
		printf("%3d (%1.5lf): ", it.second, mapFreq[it.second]);
		for (auto it2 : it.first)
			printf("%d", it2);
		printf("\n");
	}

	//��������� �������������� ������� ������ ���������
	printf("����������� ����� ��������� ���� ��� ������ ������� = %lf\n", averageLen);
	printf("����� ��������� ��� ������������ ��������� ���� = %lf\n", averageLen * 128);

	printf("����������� ���\n");
	int countUni = 0;
	for (int i = 0; i < v.size(); i++) {
		printf("%s ", str(mapUni[v[i]]));
		countUni += mapUni[v[i]].size();
	}
	printf("\n���-�� �������� = %d\n������� ����� ���� = %lf\n������������ ���� = %lf\n", countUni, countUni / 128.0, countUni / 128.0 / averageLen);

	printf("��� ��������\n");
	int countHuf = 0;
	for (int i = 0; i < v.size(); i++) {
		printf("%s ", str(mapHuf[v[i]]));
		countHuf += mapHuf[v[i]].size();
	}
	printf("\n���-�� �������� = %d\n������� ����� ���� = %lf\n������������ ���� = %lf\n", countHuf, countHuf / 128.0, countHuf / 128.0 / averageLen);


	printf("��� �������-����\n");
	int countShen = 0;
	for (int i = 0; i < v.size(); i++) {
		printf("%s ", str(mapShen[v[i]]));
		countShen += mapShen[v[i]].size();
	}
	printf("\n���-�� �������� = %d\n������� ����� ���� = %lf\n������������ ���� = %lf\n", countShen, countShen / 128.0, countShen / 128.0 / averageLen);

	system("pause");
}