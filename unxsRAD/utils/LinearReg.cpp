#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

//g++ -std=gnu++0x LinearReg.cpp
//y = mx +h; h = y when x=0;
//h = avg{y} - m  avg{x}
//
//latex:
// y = a + bx
//$ b = \frac{\sum_{i=1}^n x_i y_i - n \bar{x} \bar{y}}{\sum_{i=1}^n x_i^2 - n \bar{x}^2} $
//$ a = \bar{y} - b  \bar{x}$

double slope(const std::vector<double>& x, const std::vector<double>& y)
{
    const auto n    = x.size();
    const auto s_x  = std::accumulate(x.begin(), x.end(), 0.0);
    const auto s_y  = std::accumulate(y.begin(), y.end(), 0.0);
    const auto s_xx = std::inner_product(x.begin(), x.end(), x.begin(), 0.0);
    const auto s_xy = std::inner_product(x.begin(), x.end(), y.begin(), 0.0);
    const auto m    = (n * s_xy - s_x * s_y) / (n * s_xx - s_x * s_x);
    return m;
}

double intercept(const std::vector<double>& x, const std::vector<double>& y)
{
    const auto n    = x.size();
    const auto s_x  = std::accumulate(x.begin(), x.end(), 0.0);
    const auto s_y  = std::accumulate(y.begin(), y.end(), 0.0);
    const auto a_x  = s_x / n;
    const auto a_y  = s_y / n;
    const auto h    = a_y - ( slope(x,y) * a_x );
    return h;
}

int main(int iArgc, char *cArgv[])
{
	double dX=0.0;
	if(iArgc==2)
	{
		sscanf(cArgv[1],"%lf",&dX);
		std::cout << "dX=" << cArgv[1] << " " << dX << "\n";
	}
	else
	{
		std::cout << "usage: " << cArgv[0] << "<dX>" << "\n";
		exit(0);
	}
		
	std::vector<double> x{1, 2, 3, 4, 5, 6, 7};
	std::vector<double> y{167, 173, 186, 187, 201, 202, 210};
	const auto m = slope(x, y);
	const auto h = intercept(x, y);
	std::cout << "m=" << m << " h=" << h << "\n";
	std::cout << "dX=" << dX << "\n";
	const auto dY = (m * dX) + h;
	std::cout << "dY=" << dY << "\n";
}
