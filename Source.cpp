#include <iostream>
#include <vector>
#include <list>
#include <fstream>
#include <string>
#include <limits>

using namespace std;

const int param_max = 100;

class Point {

public:

	double Get_x();
	double Get_y();
	double Get_z();
	double Get_param();

	void Set_x(double x_in);
	void Set_y(double y_in);
	void Set_z(double z_in);

	Point();
	Point(double x_in, double y_in, double z_in);
	Point(double x_in, double y_in, double z_in, unsigned int param_in);
	
private:

	double x, y, z;					// coords
	unsigned int param = 0;
	
};

ostream& operator<< (ostream& os, Point point_out)
{

	os << "\n(" << point_out.Get_x() << ", " << point_out.Get_y() << ", " << point_out.Get_z() << "), param = " << point_out.Get_param() << endl;

	return os;

}

double Point::Get_x()
{
	return x;
}
double Point::Get_y()
{
	return y;
}
double Point::Get_z()
{
	return z;
}

double Point::Get_param()
{
	return param;
}

void Point::Set_x(double x_in)
{
	x = x_in;
}
void Point::Set_y(double y_in)
{
	y = y_in;
}
void Point::Set_z(double z_in)
{
	z = z_in;
}

Point::Point() : x(0), y(0), z(0) {}

Point::Point(double x_in, double y_in, double z_in) : x(x_in), y(y_in), z(z_in) {}

Point::Point(double x_in, double y_in, double z_in, unsigned int param_in)
	: x(x_in), y(y_in), z(z_in), param(param_in) {}

Point param_mid(Point a, Point b, double curr_param)			// compute position if intermediate point between vertexes, depends of param
{

	Point param_mid(
		((b.Get_x() - a.Get_x()) / param_max) * curr_param + a.Get_x(),
		((b.Get_y() - a.Get_y()) / param_max) * curr_param + a.Get_y(),
		((b.Get_z() - a.Get_z()) / param_max) * curr_param + a.Get_z(),
		curr_param
	);

	return param_mid;

}

vector<Point> read_from_file(ifstream* file_in)
{
	string line_in;
	unsigned char pos1, pos2;			// comma positions in current line

	vector<Point> final;				// to return
	Point current;

	while (getline(*file_in, line_in))		// reading each line
	{
		pos1 = line_in.find_first_of(',');
		pos2 = line_in.find_last_of(',');

		current.Set_x(stod(line_in.substr(0, pos1)));
		current.Set_y(stod(line_in.substr(pos1 + 1, pos2 - pos1)));
		current.Set_z(stod(line_in.substr(pos2 + 1)));

		final.push_back(current);
	}

	return final;

}

vector<Point> curve_compute(vector<Point>& points_in)			// main part, curve points computing
{

	vector<Point> temp;					// to return

	unsigned char param = 0;			// 0 to 100
	unsigned char step = 1;				// step if param increasing in each step. Number of points in final curve is (param_max / step) + 1

	unsigned char counter = 0;			// counter of necessary number of points to build final curve
	
	unsigned char sz = points_in.size();

	vector<list<Point>> intermediate;
	intermediate.resize(sz - 1);

	for (int i = 0; i < (sz - 1); ++i) intermediate[i].resize(sz - i - 1);
	list<Point>::iterator it, it1, it2;								// iterators for point computation
	it1 = intermediate[0].begin();
	it2 = intermediate[0].begin();
	++it2;
	
	temp.push_back(points_in[0]);

	for (param = step; param <= param_max; param += step)			// main cycle of param
	{
		for (int i = 0; i < (sz - 1); ++i) {						// cycle of rows of points pyramid

			it = intermediate[i].begin();

			if (i > 0) {											// iterators for row 2 and above
				it1 = intermediate[i - 1].begin();
				it2 = intermediate[i - 1].begin();
				++it2;
			}

			for (int j = 0; j < sz - i - 1; ++j) {					// cycle of columns in i-th row of pyramid

				if (i == 0) *it = param_mid(points_in[j], points_in[j + 1], param);

				else {

					*it = param_mid(*it1, *it2, param);

					++it1;
					++it2;

				}

				++it;

			}												// j cycle ends here
		}													// i cycle ends here

		it = intermediate[sz - 2].begin();
		temp.push_back(*it);

	}														// main cycle of param end here

	return temp;

}

double distation(Point a, Point b)
{

	return sqrt(pow((a.Get_x() - b.Get_x()), 2) + pow((a.Get_y() - b.Get_y()), 2) + pow((a.Get_z() - b.Get_z()), 2));

}

vector<Point> sort_out(vector<Point>& a, vector<Point>& b)
{

	vector<Point> c;					// to return, 2 points

	double dist, curr_dist;
	Point a_candidate, b_candidate;

	dist = numeric_limits<decltype(dist)>::max();

	for (auto i : a) for (auto j : b)			// checking ALL combinations, O(n^2)
	{
		curr_dist = distation(i, j);

		if (curr_dist < dist)
		{
			dist = curr_dist;
			a_candidate = i;
			b_candidate = j;
		}
	}

	c.push_back(a_candidate);
	c.push_back(b_candidate);

	return c;

}

int main()
{
	ifstream In1;
	In1.open("1.txt");				// reading curve 1 file

	ifstream In2;
	In2.open("2.txt");				// reading curve 2 file

	vector<Point> points1 = read_from_file(&In1);		// points, loaded from file
	vector<Point> points2 = read_from_file(&In2);		

	vector<Point> curve1 = curve_compute(points1);				// points of final curve
	vector<Point> curve2 = curve_compute(points2);				// points of final curve
	
	vector<Point> finalists = sort_out(curve1, curve2);			// heavy part

	cout << "Closest points is " << finalists[0] << "and " << finalists[1] << "Distation is " << distation(finalists[0], finalists[1]);			// results
	
	return 12345;
}

