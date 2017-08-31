#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <fstream>

#define MIN_COUNT 50
#define MAX_COUNT 70
#define MIN_RADIUS 5.0
#define MAX_RADIUS 15.0
#define BORDER 150.0
#define DELTA 1
using namespace std;

struct District
{
	vector<vector<double>> points;
	vector<double> center;
	bool overlap = false, reflect = false;
	double radius;
};

int dis(int from, int to)
{
	random_device rd;  //Will be used to obtain a seed for the random number engine
	mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	uniform_int_distribution<> d(from, to);
	return d(gen);
}

double dist(double from, double to)
{
	random_device rd;  //Will be used to obtain a seed for the random number engine
	mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	uniform_real_distribution<double> d(from, to);
	return d(gen);
}

double normal_dist()
{
	random_device rd;  //Will be used to obtain a seed for the random number engine
	mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
	normal_distribution<double> d(0.0, 1.0);
	return d(gen);
}

double sign(double a)
{
	if (a == 0)
		return 0.0;
	if (a > 0)
		return 1.0;
	return -1.0;
}
int main(int argc, char* argv[])
{
	//-dim (dimension)   размерность пр-ва
	//-c   (count)       кол-во областей
	//-o   (overlapping) кол-во перекрывающих областей
	//-r   (reflecting)  кол-во касающихся обалстей
	vector<string> commands;
	commands.push_back("-dim");
	commands.push_back("-c");
	commands.push_back("-o");
	commands.push_back("-r");
	vector<int> params;
	params.assign(4, 0);
	string dest;
	for (int param_index = 1; param_index < argc; ++param_index)
	{
		string str = argv[param_index];
		if (str[0] == '-')
		{
			for (int command_index = 0; command_index < commands.size(); ++command_index)
			{
				int comparator = commands[command_index].compare(str);
				if (!comparator)
				{
					for (int value_index = param_index + 1; value_index < argc; ++value_index)
					{
						string value_string = argv[value_index];
						try
						{
							int a = std::stoi(value_string);
							params[command_index] = a;
							break;
						}
						catch (invalid_argument)
						{
							bool flag = false;
							for (int sub_index = 0; sub_index < commands.size(); ++sub_index)
								if (!(commands[sub_index].compare(value_string)))
									flag = true;
							if (flag) break;
							continue;
						}
					}
				}
			}
		}
	}

	if (params[2] > params[1] || params[3] > params[1])
	{
		cout << "Count of overlapping or reflecting districs have to less than general count of districts" << endl;
		return -1;
	}

	for (int param_index = 0; param_index < params.size(); ++param_index)
	{
		if (params[param_index] < 0)
		{
			cout << commands[param_index] << "can't be less 0" << endl;
			return -2;
		}
	}

	if (params[2] == 1 || params[3] == 1)
	{
		cout << "Count of overlapping or reflecting districs can't be equal 1" << endl;
		return -3;
	}

	if (argc == 1)
	{
		cout << "-dim (dimension)   Space dimension\n"
			 << "-c   (count)       Count of districts\n"
			 << "-o   (overlapping) Count of overlapping districts\n"
			 << "-r   (reflecting)  Count of reflecting districts\n";
		return 0;
	}
	else
	{
		if (params[0] == 0)
		{
			cout << "Dimention is equal zero" << endl;
			return -4;
		}
	}

	
	vector<District> districts;
	for (int i = 0; i < params[1]; ++i)
	{
		District temp;
		temp.points.resize(dis(MIN_COUNT, MAX_COUNT));
		temp.center.resize(params[0]);
		temp.radius = dist(MIN_RADIUS, MAX_RADIUS);
		for (int point_index = 0; point_index < temp.points.size(); ++point_index)
		{
			temp.points[point_index].resize(params[0]);
		}
		districts.push_back(temp);
	}
	int OLcounter, Rcounter;

	for (;;) {
		OLcounter = 0;
		Rcounter = 0;
		for (District& a : districts)
		{
			a.overlap = false;
			a.reflect = false;
		}
		vector<bool> conditions;
		for (int district_index = 0; district_index < districts.size(); ++district_index)
		{
			int OLcounter_local = 0, Rcounter_local = 0;
			District& currDistrict = districts[district_index];
			for(int coord_index = 0; coord_index <  params[0]; ++coord_index)
			{
				currDistrict.center[coord_index] = dist(0, BORDER);
			}
			conditions.clear();
			for (int district_state_index = 0; district_state_index <= district_index; district_state_index++)
			{
				conditions.push_back(districts[district_state_index].overlap);
				conditions.push_back(districts[district_state_index].reflect);

			}
			for (int j = district_index - 1; j >= 0; --j)
			{
				District& prevDistrict = districts[j];
				double norma = 0;
				for (int axis = 0; axis < params[0]; ++axis)
				{
					norma += (prevDistrict.center[axis] - currDistrict.center[axis]) * (prevDistrict.center[axis] - currDistrict.center[axis]);
				}
				norma = sqrt(norma);
				if ((prevDistrict.radius + currDistrict.radius - norma) < DELTA && (prevDistrict.radius + currDistrict.radius - norma) > 0)
				{
					if (!prevDistrict.reflect)
					{
						prevDistrict.reflect = true;
						Rcounter++;
						Rcounter_local++;
					}
					if(!currDistrict.reflect)
					{
						currDistrict.reflect = true;
						Rcounter++;
						Rcounter_local++;
					}
				}
				else if ((prevDistrict.radius + currDistrict.radius) > norma)
				{
					if (!prevDistrict.overlap)
					{
						prevDistrict.overlap = true;
						OLcounter++;
						OLcounter_local++;
					}
					if (!currDistrict.overlap)
					{
						currDistrict.overlap = true;
						OLcounter++;
						OLcounter_local++;
					}
				} 
				else if (!currDistrict.reflect && !currDistrict.overlap && Rcounter < params[3])
				{
					if (!prevDistrict.reflect)
					{
						prevDistrict.reflect = true;
						Rcounter++;
						Rcounter_local++;
					}
					if (Rcounter == params[3])
					{
						prevDistrict.reflect = false;
						Rcounter--;
						Rcounter_local--;
						continue;
					}
					else
					{
						for (int coord_index = 0; coord_index < params[0]; ++coord_index)
						{
							double destination = (prevDistrict.radius + currDistrict.radius) * (prevDistrict.center[coord_index] - currDistrict.center[coord_index]);
							currDistrict.center[coord_index] = prevDistrict.center[coord_index] + DELTA * sign(destination) - destination / norma;
						}
						currDistrict.reflect = true;
						Rcounter++;
						Rcounter_local++;
					}
				}
				if (Rcounter > params[3] || OLcounter > params[2])
				{
					Rcounter = Rcounter - Rcounter_local;
					OLcounter = OLcounter - OLcounter_local;
					for (int district_state_index = 0; district_state_index <= district_index; district_state_index++)
					{
						districts[district_state_index].overlap = conditions[2 * district_state_index];
						districts[district_state_index].reflect = conditions[2 * district_state_index + 1];
					}
					--district_index;
					break;
				}
			}
		}
		if (OLcounter == params[2] && Rcounter == params[3]) break;
	}

	for (District& currDistrict : districts)
	{
		double U, D;
		for(int point_index = 0; point_index < currDistrict.points.size(); ++point_index)
		{
			U = dist(0, 1);
			D = currDistrict.radius * pow(U, params[0]);
			double norma = 0;
			for (int axis_index = 0; axis_index < params[0]; ++axis_index)
			{
				currDistrict.points[point_index][axis_index] = dist(-currDistrict.radius, currDistrict.radius);
				norma += currDistrict.points[point_index][axis_index] * currDistrict.points[point_index][axis_index];

			}
			norma = sqrt(norma);
			for (int axis_index = 0; axis_index < params[0]; ++axis_index)
			{
				currDistrict.points[point_index][axis_index] = currDistrict.center[axis_index] + currDistrict.points[point_index][axis_index] * (D / norma);
			}
		}
	}

	ofstream myfile;
	myfile.open("2.txt");
	for (int district_index = 0; district_index < districts.size(); ++district_index)
	{
		for (vector<double>& point : districts[district_index].points)
		{
			for (double a : point)
				myfile << a << " ";
		}
	}
	myfile.close();
	return 0;
}