
// https://en.wikipedia.org/wiki/Longest_common_subsequence_problem
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

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
			std::cout << info.str[i];
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
			diff_info.push_back(DIFF_INFO(DIFF_INFO::TYPE::SAME, x.str + i - 1, 1));
		}
	}
	else if (j > 0 && (i == 0 || c[i][j - 1] >= c[i - 1][j])) {
		printDiff(diff_info, c, x, y, i, j - 1);
		//std::cout << "+ " << y[j];
		if (diff_info.empty() == false && diff_info.back().type == DIFF_INFO::TYPE::ADDITION) {
			diff_info.back().len++;
		}
		else {
			diff_info.push_back(DIFF_INFO(DIFF_INFO::TYPE::ADDITION, y.str + j - 1, 1));
		}
	}
	else if (i > 0 && (j == 0 || c[i][j - 1] < c[i - 1][j])) {
		printDiff(diff_info, c, x, y, i - 1, j);
		//std::cout << "- " << x[i];
		if (diff_info.empty() == false && diff_info.back().type == DIFF_INFO::TYPE::REMOVAL) {
			diff_info.back().len++;
		}
		else {
			diff_info.push_back(DIFF_INFO(DIFF_INFO::TYPE::REMOVAL, x.str + i - 1, 1));
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


int main(int argc, char* argv[])
{
	char before[] = "printDiff(diff_info, c, x, y, x.len, y.len);";
	char now[] = "printDiff2(diff_info, cd, xz, yk, x.len, y.len):";

	const int before_size = std::strlen(before);
	
	const int now_size = std::strlen(now);
	const int before_start = 0, now_start = 0;
	std::vector<DIFF_INFO> diff_info;

	std::vector<DIFF_INFO> temp = GetDiff(before + before_start, before_size, now + now_start, now_size);
	diff_info.insert(diff_info.begin() + diff_info.size(), std::make_move_iterator(temp.begin()), std::make_move_iterator(temp.end()));
	

	std::cout << before << std::endl;
	std::cout << now << std::endl;

	for (int i = 0; i < diff_info.size(); ++i) {
		switch (diff_info[i].type)
		{
			case DIFF_INFO::TYPE::SAME:
				std::cout << "SAME : " << C_STR_INFO(diff_info[i].str, diff_info[i].len) << std::endl;
				break;

			case DIFF_INFO::TYPE::ADDITION:
				std::cout << "ADDITION : " << C_STR_INFO(diff_info[i].str, diff_info[i].len) << std::endl;
				break;

			case DIFF_INFO::TYPE::REMOVAL:
				std::cout << "REMOVAL : " << C_STR_INFO(diff_info[i].str, diff_info[i].len) << std::endl;
				break;

			case DIFF_INFO::TYPE::EOL:
				std::cout << std::endl;
				break;
		}
	}


	return 0;
}

