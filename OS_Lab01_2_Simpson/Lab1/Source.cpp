#include <iostream>

using namespace std;

double EPS = 1.0;

double linear(double x) {
	return x * x*x - 2 * x*x + 5;
}

double integrate(double(*f)(double), double leftX, double rightX, double dx = .0001) {

	if (abs(leftX - rightX) < EPS)
		return 0;

	int invert = 1;
	if (leftX > rightX) {
		swap(leftX, rightX);
		invert = -1;
	}

	double ans = 0;

	int N = (rightX - leftX) / dx;
	if (N % 2) N++;
	if (N < 2) N = 2;
	double h = (rightX - leftX) / N;

	double x = leftX;

	ans += f(x);
	x += h;
	for (size_t j = 1; j < N - 1; j += 2, x += h * 2)
	{
		ans += (f(x) * 4 + f(x + h) * 2);
	}
	ans += (f(x) * 4 + f(x + h));
	ans *= (h / 3);

	return ans * invert;
}

int main() {
	while (1.0 + 0.5 * EPS != 1.0) {
		EPS *= 0.5;
	}
	//cout << EPS << endl;
	cout << integrate(linear, 0, 6.28318530718) << endl;

	cout << integrate([](double x) -> double { return x * x*x*x - x * x * 2; }, 0, 3) << endl;

	system("pause");
}