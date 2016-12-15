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
map <int, int> mapCount; //кол-во встречаемости символа
map <int, double> mapFreq; //словарь из символов первичного алфавита и частот встречаемости
vector<pair<int, double>> vecFreq; // массив пар символ - встречаемость

//компоратор для сортировки пар символ-частота
bool cmp(pair <int, double> a, pair <int, double> b) {
	return a.second == b.second ? a.first < b.first : a.second > b.second;
}
//функция для отладки массива c пикселями 
void debugv(vector<int> v) {
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 8; j++)
			printf("%3d:%3d | ", j * 16 + i + 1, v[j * 16 + i]);
		printf("\n");
	}
	printf("\n");
}
//функция для перевода вектора в строку
char* str(vector<int> v) {
	char* res = (char*)malloc(v.size());
	for (int i = 0; i < v.size(); i++)
		res[i] = char(v[i] + '0');
	res[v.size()] = '\0';
	return res;
}


//словарь для односимвольного кода;
map <int, vector<int>> mapUni;


//Шеннон-Фано
vector<pair<int, vector<int>>> vecShen; //вектор с кодами
map<int, vector<int>> mapShen; //словарь с кодами
void Shen(int up, int down) {
	double sumDown = 0, sumUp = 0;
	for (int i = up; i < down; i++)
		sumDown += vecFreq[i].second;
	int mid = up;
	double dif = sumDown;
	//находим линию, по которой поделим частоты
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
	//добавим к кодам первой части единицы, второй - нули 
	for (int i = up; i < mid; i++)
		vecShen[i].second.push_back(1);
	for (int i = mid; i < down; i++)
		vecShen[i].second.push_back(0);
	//рекурсивно запустимся от обеих частей, если их размер > 1
	if (mid - up > 1)
		Shen(up, mid);
	if (down - mid > 1)
		Shen(mid, down);
}
void resShen() {
	for (auto it : vecShen)
		mapShen[it.first] = it.second;
}


//Хаффман
struct point {
	int symbol; // -1, если не лист
	double val;
	bool used;
	int l, r;  // -1, если лист
};
vector<point> hf;
//заполнение таблицы
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
		//ищем первый минимум
		int min1 = -1;
		for (int i = 0; i < hf.size(); i++)
			if (!hf[i].used && (min1 == -1 || hf[i].val < hf[min1].val))
				min1 = i;
		hf[min1].used = 1;
		//ищем второй минимум
		int min2 = -1;
		for (int i = 0; i < hf.size(); i++)
			if (!hf[i].used && (min2 == -1 || hf[i].val < hf[min2].val))
				min2 = i;
		if (min2 == -1)
			return min1;
		hf[min2].used = 1;
		//добавляем новый узел
		point newPoint;
		newPoint.symbol = -1;
		newPoint.l = min1;
		newPoint.r = min2;
		newPoint.val = hf[min1].val + hf[min2].val;
		newPoint.used = 0;
		hf.push_back(newPoint);
	}
}
vector<pair<vector<int>, int>> vecHuf; //вектор с кодами
map<int, vector<int>> mapHuf; //словарь с кодами
vector<int> now;
//подсчёт вектора и словаря с кодами
void resHuf(int num) {
	if (hf[num].symbol != -1) {
		vecHuf.push_back(make_pair(now, hf[num].symbol));
		mapHuf[hf[num].symbol] = now;
		return;
	}
	//запускаемся от левого листа
	now.push_back(0);
	resHuf(hf[num].l);
	now.pop_back();

	//запускаемся от правого листа
	now.push_back(1);
	resHuf(hf[num].r);
	now.pop_back();
}


int main() {
	setlocale(LC_ALL, "Russian");
	SetConsoleOutputCP(1252); // установка кодовой страницы win-cp 1251 в поток вывода (кириллица)
	freopen("output.txt", "w", stdout);
	vector<vector<int>> arr = bmpToArr(name); // двумерный массив значений цветов
	vector<int> v(128); // массив из значений на диагонали (большее кол-во разных значений цветов)

						//присваиваем значения массива v
	for (int i = 0; i < 128; i++)
		v[i] = arr[i][0] * 0.299 + arr[i][1] * 0.587 + arr[i][2] * 0.114;
	debugv(v);

	for (int i = 0; i < 128; i++) {
		//квантуем по определённому правилу
		v[i] = (v[i] / 20) * 20;
		//заполняем словарь 
		mapFreq[v[i]] += 1.0 / 128;
	}
	debugv(v);

	//вывод значний словаря
	printf("словарь:\n");
	for (auto it : mapFreq)
		printf("%3d:%1.3lf|", it.first, it.second);
	printf("\n");

	//преобразование словаря в вектор
	printf("сортируем по невозрастаемости частоты встречаемости:\n");
	for (auto it : mapFreq)
		vecFreq.push_back(make_pair(it.first, it.second));
	sort(vecFreq.begin(), vecFreq.end(), cmp); //сортируем по невозрастаемости частоты встречаемости
	for (auto it : vecFreq)
		printf("%3d: %lf\n", it.first, it.second);

	//средняя длина двоичного кода
	printf("средняя длина двоичного кода:");
	double averageLen = 0;
	for (auto it : vecFreq)
		averageLen += -it.second * log2(it.second);
	printf("%lf\n", averageLen);

	//двоичный равномерный односимвольный код
	printf("двоичный равномерный односимвольный код\n");
	int k = 1;
	int st = 0; //2^st - первое число >= frequency.size() 
	while (k < vecFreq.size())
		k *= 2, st++;
	int kod = 0;
	for (auto it : vecFreq) {
		printf("%3d: ", it.first);
		//вывод равномерного кода
		for (int i = st - 1; i >= 0; i--) {
			int s = bool((1 << i) & kod); //очередной символ в коде
			printf("%d", s);
			mapUni[it.first].push_back(s); //заполнение словаря
		}
		printf("\n");
		kod++;
	}

	//Шеннон-Фано
	//перенос данных в специальную структуру
	vecShen.resize(vecFreq.size());
	for (int i = 0; i < vecFreq.size(); i++)
		vecShen[i].first = vecFreq[i].first;
	Shen(0, vecFreq.size()); //подсчёт вектора с кодами
	resShen(); //подсчёт словаря с кодами
			   //вывод результата
	printf("Шеннон-Фано\n");
	for (int i = 0; i < vecShen.size(); i++) {
		printf("%3d (%1.5lf): ", vecShen[i].first, vecFreq[i].second);
		for (int j = 0; j < vecShen[i].second.size(); j++)
			printf("%d", vecShen[i].second[j]);
		printf("\n");
	}

	//Хаффман
	int num = Huf(); //вычисление таблицы
					 //вывод таблицы
	printf("num sym   val  l  r\n");
	for (int i = 0; i < hf.size(); i++) {
		printf("%3d %3d %1.3lf %2d %2d\n", i, hf[i].symbol, hf[i].val, hf[i].l, hf[i].r);
	}
	resHuf(num); //заполнение словаря и вектора 
				 //вывод кодов
	sort(vecHuf.begin(), vecHuf.end());
	for (auto it : vecHuf) {
		printf("%3d (%1.5lf): ", it.second, mapFreq[it.second]);
		for (auto it2 : it.first)
			printf("%d", it2);
		printf("\n");
	}

	//сообщения закодированные разными видами кодировки
	printf("Минимальная длина двоичного кода для одного символа = %lf\n", averageLen);
	printf("Длина сообщения для минимального двоичного кода = %lf\n", averageLen * 128);

	printf("Равномерный код\n");
	int countUni = 0;
	for (int i = 0; i < v.size(); i++) {
		printf("%s ", str(mapUni[v[i]]));
		countUni += mapUni[v[i]].size();
	}
	printf("\nКол-во символов = %d\nСредняя длина кода = %lf\nИзбыточность кода = %lf\n", countUni, countUni / 128.0, countUni / 128.0 / averageLen);

	printf("код Хаффмана\n");
	int countHuf = 0;
	for (int i = 0; i < v.size(); i++) {
		printf("%s ", str(mapHuf[v[i]]));
		countHuf += mapHuf[v[i]].size();
	}
	printf("\nКол-во символов = %d\nСредняя длина кода = %lf\nИзбыточность кода = %lf\n", countHuf, countHuf / 128.0, countHuf / 128.0 / averageLen);


	printf("код Шеннона-Фано\n");
	int countShen = 0;
	for (int i = 0; i < v.size(); i++) {
		printf("%s ", str(mapShen[v[i]]));
		countShen += mapShen[v[i]].size();
	}
	printf("\nКол-во символов = %d\nСредняя длина кода = %lf\nИзбыточность кода = %lf\n", countShen, countShen / 128.0, countShen / 128.0 / averageLen);

	system("pause");
}