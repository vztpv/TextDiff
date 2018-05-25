
// https://en.wikipedia.org/wiki/Longest_common_subsequence_problem
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#define ENTER "\n"

inline bool isWhiteSpace(char ch)
{
	switch(ch)
	{
	case ' ':
	case '\t':
	case '\r':
	case '\n':
	case '\v':
	case '\f':
	case '\0':
		return true;
		break;
	}
	return false;
}
class C_STR_INFO
{
public:
	char* str;
	int len;
public:
	explicit C_STR_INFO(char* str, int len) : str(str), len(len)
	{
		//
	}

	// idx  1..len
	char operator[](const int idx) const {
		return str[idx - 1];
	}
	char& operator[](const int idx) {
		return str[idx - 1];
	}

	friend std::ostream& operator<<(std::ostream& stream, const C_STR_INFO& info) {
		for (int i = 0; i < info.len; ++i) {
			stream << info.str[i];
		}
		return stream;
	}

public:
	explicit C_STR_INFO() { }
	virtual ~C_STR_INFO() { }
};

class DIFF_INFO
{
public:
	enum class TYPE { SAME, REMOVAL, ADDITION, EOL }; // EOL : End Of Line.
	TYPE type = TYPE::SAME; 
	char* str = nullptr;
	int len = 0;

public:
	DIFF_INFO(TYPE type, char* str, int len) : type(type), str(str), len(len)
	{
		//
	}
};

/*
function LCSLength(X[1..m], Y[1..n])
    C = array(0..m, 0..n)
    for i := 0..m
       C[i,0] = 0
    for j := 0..n
       C[0,j] = 0

    for i := 1..m
        for j := 1..n
            if X[i] = Y[j]
                C[i,j] := C[i-1,j-1] + 1
            else
                C[i,j] := max(C[i,j-1], C[i-1,j])
    return C[m,n]
*/
std::vector<std::vector<int>> LCSLength(const C_STR_INFO& x, const C_STR_INFO& y)
{
	const int m = x.len;
	const int n = y.len;
	std::vector<std::vector<int>> c(m + 1, std::vector<int>(n + 1, 0));

	for (int i = 1; i <= m; ++i) {
		for (int j = 1; j <= n; ++j) {
			if (x[i] == y[j]) {
				c[i][j] = c[i - 1][j - 1] + 1;
			}
			else {
				c[i][j] = std::max(c[i][j - 1], c[i - 1][j]);
			}
		}
	}
	
	return c;
}

/*
function printDiff(C[0..m, 0..n], X[1..m], Y[1..n], i, j)
if i > 0 and j > 0 and X[i] = Y[j]
	printDiff(C, X, Y, i - 1, j - 1)
	print "  " + X[i]
else if j > 0 and (i = 0 or C[i, j - 1] ≥ C[i - 1, j])
	printDiff(C, X, Y, i, j - 1)
	print "+ " + Y[j]
else if i > 0 and (j = 0 or C[i, j - 1] < C[i - 1, j])
	printDiff(C, X, Y, i - 1, j)
	print "- " + X[i]
else
	print ""
*/
void printDiff(std::vector<DIFF_INFO>& diff_info, const std::vector<std::vector<int>>& c, const C_STR_INFO& x, const C_STR_INFO& y, const int i, const int j)
{
	if (i > 0 && j > 0 && x[i] == y[j]) {
		printDiff(diff_info, c, x, y, i - 1, j - 1);
		//std::cout << " " << x[i];
		if (diff_info.empty() == false && diff_info.back().type == DIFF_INFO::TYPE::SAME) {
			diff_info.back().len++;
		}
		else {
			if (isWhiteSpace(x.str[i - 1])) {
				//
			}
			else {
				diff_info.push_back(DIFF_INFO(DIFF_INFO::TYPE::SAME, x.str + i - 1, 1));
			}
		}
	}
	else if (j > 0 && (i == 0 || c[i][j - 1] >= c[i - 1][j])) {
		printDiff(diff_info, c, x, y, i, j - 1);
		//std::cout << "+ " << y[j];
		if (diff_info.empty() == false && diff_info.back().type == DIFF_INFO::TYPE::ADDITION) {
			diff_info.back().len++;
		}
		else {
			if (isWhiteSpace(y.str[j - 1])) {
				//
			}
			else {
				diff_info.push_back(DIFF_INFO(DIFF_INFO::TYPE::ADDITION, y.str + j - 1, 1));
			}
		}
	}
	else if (i > 0 && (j == 0 || c[i][j - 1] < c[i - 1][j])) {
		printDiff(diff_info, c, x, y, i - 1, j);
		//std::cout << "- " << x[i];
		if (diff_info.empty() == false && diff_info.back().type == DIFF_INFO::TYPE::REMOVAL) {
			diff_info.back().len++;
		}
		else {
			if (isWhiteSpace(x.str[i - 1])) {
				//
			}
			else {
				diff_info.push_back(DIFF_INFO(DIFF_INFO::TYPE::REMOVAL, x.str + i - 1, 1));
			}
		}
	}
}

inline std::vector<DIFF_INFO> GetDiff(char* before, int before_len, char* now, int now_len) {
	C_STR_INFO x(before, before_len), y(now, now_len);

	auto c = LCSLength(x, y);
	std::vector<DIFF_INFO> diff_info;

	printDiff(diff_info, c, x, y, x.len, y.len);
	
	return diff_info;
}

void Do(std::vector<DIFF_INFO>& diff_info, std::ifstream& beforeFile, std::ifstream& nowFile, char*& beforeBuffer, char*& nowBuffer)
{
	long long _beforeLen;
	long long _nowLen;
	char emptyBuffer[] = "";

	{
		beforeFile.seekg(0, beforeFile.end);
		long long length = beforeFile.tellg();
		beforeFile.seekg(0, beforeFile.beg);

		beforeBuffer = new char[length + 1]; // 

									   // read data as a block:
		beforeFile.read(beforeBuffer, length);
		beforeFile.seekg(0, beforeFile.end);
		char temp;
		beforeFile >> temp;

		beforeBuffer[length] = '\0';

		_beforeLen = length;
	}
	{
		nowFile.seekg(0, nowFile.end);
		long long length = nowFile.tellg();
		nowFile.seekg(0, nowFile.beg);

		nowBuffer = new char[length + 1]; // 

											 // read data as a block:
		nowFile.read(nowBuffer, length);
		nowFile.seekg(0, nowFile.end);
		char temp;
		nowFile >> temp;

		nowBuffer[length] = '\0';

		_nowLen = length;
	}
	{
		char* before = beforeBuffer;
		char* now = nowBuffer;
		long long beforeLen = 0; // beforeTokenLen
		long long nowLen = 0; // nowTokenLen
		long long beforeEnterPos = -1, nowEnterPos = -1;
		long long i = 0, j = 0;

		while (i < _beforeLen || j < _nowLen) {
			if (i >= _beforeLen) {
				before = emptyBuffer;
				beforeLen = 0;
			}
			else {
				for (long long k = i; k < _beforeLen; ++k) {
					if (beforeBuffer[k] == '\n' || beforeBuffer[k] == '\0') {
						i = k;
						break;
					}
					else if (k == _beforeLen - 1) {
						i = k + 1;
					}
				}

				beforeLen = i - beforeEnterPos - 1;
				for (long long k = i - 1; k > beforeEnterPos; --k) {
					if (!isWhiteSpace(beforeBuffer[k])) {
						break;
					}
					else {
						beforeLen--;
					}
				}
				if (beforeLen > 0) {
					for (long long k = beforeEnterPos + 1; k <= i - 1; ++k) {
						if (!isWhiteSpace(beforeBuffer[k])) {
							break;
						}
						else {
							beforeLen--;
							before++;
						}
					}
				}
				beforeEnterPos = i;
			}
			if (j >= _nowLen) {
				now = emptyBuffer;
				nowLen = 0;
			}
			else {
				for (long long k = j; k < _nowLen; ++k) {
					if (nowBuffer[k] == '\n' || beforeBuffer[k] == '\0') {
						j = k;
						break;
					}
					else if (k == _nowLen - 1) {
						j = k + 1;
					}
				}

				nowLen = j - nowEnterPos - 1;

				for (long long k = j - 1; k > nowEnterPos; --k) {
					if (!isWhiteSpace(nowBuffer[k])) {
						break;
					}
					else {
						nowLen--;
					}
				}
				if (nowLen > 0) {
					for (long long k = nowEnterPos + 1; k <= j - 1; ++k) {
						if (!isWhiteSpace(nowBuffer[k])) {
							break;
						}
						else {
							nowLen--;
							now++;
						}
					}
				}
				nowEnterPos = j;
			}

			const int before_size = beforeLen;
			const int now_size = nowLen;

			if (before_size == 0 && now_size == 0) {
				before = beforeBuffer + i + 1;
				now = nowBuffer + j + 1;
				++i;
				++j;

				continue;
			}

			std::vector<DIFF_INFO> temp = GetDiff(before, before_size, now, now_size);
			static char empty[] = "";
	
			diff_info.insert(diff_info.begin() + diff_info.size(), std::make_move_iterator(temp.begin()), std::make_move_iterator(temp.end()));
			diff_info.push_back(DIFF_INFO(DIFF_INFO::TYPE::EOL, empty, 0));

			// debug
			//std::cout << C_STR_INFO(before, before_size) << std::endl;
			//std::cout << C_STR_INFO(now, now_size) << std::endl;

			before = beforeBuffer + i + 1;
			now = nowBuffer + j + 1;
			++i;
			++j;
		}
	}
}

// for ansi file..
int main(int argc, char* argv[])
{
	if (argc == 3) {
		std::ifstream beforeFile(argv[1], std::ios::binary);
		std::ifstream nowFile(argv[2], std::ios::binary);
		std::ofstream outFile("test.txt");


		if (!beforeFile || !nowFile) { outFile.close();  beforeFile.close(); nowFile.close(); return 1; }

		if (!outFile) {
			beforeFile.close(); nowFile.close();
			outFile.close();
		}

		std::vector<DIFF_INFO> diff_info;

		char* beforeBuffer = nullptr;
		char* nowBuffer = nullptr;

		Do(diff_info, beforeFile, nowFile, beforeBuffer, nowBuffer);

		beforeFile.close();
		nowFile.close();
		
		for (int i = 0; i < diff_info.size(); ++i) {
			switch (diff_info[i].type)
			{
			case DIFF_INFO::TYPE::SAME:
				outFile << "SAME : " << C_STR_INFO(diff_info[i].str, diff_info[i].len) << ENTER;
				break;

			case DIFF_INFO::TYPE::ADDITION:
				outFile << "ADDITION : " << C_STR_INFO(diff_info[i].str, diff_info[i].len) << ENTER;
				break;

			case DIFF_INFO::TYPE::REMOVAL:
				outFile  << "REMOVAL : " << C_STR_INFO(diff_info[i].str, diff_info[i].len) << ENTER;
				break;

			case DIFF_INFO::TYPE::EOL:
				outFile << ENTER;
				break;
			}
		}

		outFile.close();
		delete[] beforeBuffer;
		delete[] nowBuffer;
	}

	return 0;
}

