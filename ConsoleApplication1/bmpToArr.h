#include <iostream>
#include <fstream> 
#include <string> 
#include <vector>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

vector<vector<int>> bmpToArr(const char *fname) {
	vector<vector<int>> v(128, vector<int>(128));
	FILE *f = fopen(fname, "rb");

	//пропускаем заголовочный файл
	fseek(f, 56, SEEK_SET);

	//считываем битмап
	for (int i = 0; i < 128; i++) {
		for (int j = 0; j < 3; j++) {
			fread(&v[i][j], 1, sizeof(unsigned char), f);
		}
	}
	fclose(f);
	return v;
}