#include <iostream>
#include <math.h>
#include <stdarg.h>
#include <time.h>
#include <fstream>
#include <string>
using namespace std;

struct mat;

double act(double x)
{
	return (1 / (1 + exp(-x)));
}

double actsh(double x)
{
	return x * (1 - x);
}

struct vec
{
	int size;
	double* x;
	bool useBias = false;

	double& operator[](int idx)
	{
		return x[idx];
	}

	vec& operator =(vec a)
	{
		setSize(a.size);
		copyData(a);
		return *this;
	}


	vec operator +(vec &a)
	{
		vec res(a.size);
		for (int i = 0; i < a.size; i++)
		{
			res[i] = x[i] + a[i];
		}
		return res;
	}

	vec operator -(vec &a)
	{
		vec res(a.size);
		for (int i = 0; i < a.size; i++)
		{
			res[i] = x[i] - a[i];
		}
		return res;
	}

	void copyData(vec &a)
	{
		for (int i = 0; i < a.size; i++)
		{
			x[i] = a[i];
		}
	}

	void activate()
	{	
		if (useBias)
		{
			for (int i = 0; i < size - 1; i++)
			{
				x[i] = act(x[i]);
			}
		}
		else
		{
			for (int i = 0; i < size; i++)
			{
				x[i] = act(x[i]);
			}
		}
	}

	void setRandVal(double a, double b, int ton)
	{
		ton = pow(10, ton);
		for (int i = 0; i < size; i++)
		{
			x[i] = double(rand() % int((b - a) * ton) + a * ton) / ton;
		}
	}

	void clear()
	{
		for (int i = 0; i < size; i++)
		{
			x[i] = 0;
		}
	}

	void setSize(int size)
	{
		this->size = size;
		x = new double[size];
	}


	void deleteStruct()
	{
		delete[] x;
	}


	vec(const vec& a)
	{
		setSize(a.size);
		for (int i = 0; i < size; i++)
		{
			x[i] = a.x[i];
		}
	}

	vec(int size)
	{
		setSize(size);
	}
	vec()
	{
	}
	~vec()
	{
		deleteStruct();
	}

};

struct mat
{
	int l1size; // x[l1size - 1] - нейрон смещения
	int l2size;
	vec* x;


	vec& operator[](int idx)
	{
		return x[idx];
	}

	mat& operator =(mat a)
	{
		setSize(a.l1size, a.l2size);
		for (int i = 0; i < l1size; i++)
		{
			x[i] = a[i];
		}
		return *this;
	}



	vec operator *(vec &a)
	{
		vec res(l2size);
		for (int i = 0; i < l2size; i++)
		{
			res[i] = 0;
			for (int j = 0; j < l1size; j++)
			{
				res[i] += a[j] * x[j][i];
			}
		}
		return res;
	}

	mat swapLinesCol()
	{
		mat res(l2size, l1size);
		for (int i = 0; i < l2size; i++)
		{
			for (int j = 0; j < l1size; j++)
			{
				res[i][j] = x[j][i];
			}
		}
		return res;
	}

	void setRandVal(double a, double b, int ton)
	{
		for (int i = 0; i < l1size; i++)
		{
			x[i].setRandVal(a, b, ton);
		}
	}

	void setSize(int l1size, int l2size)
	{
		this->l1size = l1size;
		this->l2size = l2size;
		x = new vec[(l1size)];
		for (int i = 0; i < (l1size); i++)
		{
			x[i].setSize(l2size);
		}
	}

	void deleteStruct()
	{
		for (int i = 0; i < l1size; i++)
		{
			x[i].deleteStruct();
		}
		delete[] x;
	}


	mat(const mat& a)
	{
		setSize(a.l1size, a.l2size);
		for (int i = 0; i < l1size; i++)
		{
			x[i] = a.x[i];
		}
	}

	mat(int l1size, int l2size)
	{
		setSize(l1size, l2size);
	}
	mat()
	{
	}
	~mat()
	{
		deleteStruct();
	}
};


class Network
{
public:

	vec* l;
	vec* lError;
	mat* w;
	int size;
	bool useBias = true;


	void saveToFile(string name)
	{
		fstream file;
		file.open(name, ios::out);
		if (!file)
		{
			ofstream fileCreate(name);
			file.open(name, ios::out);
		}
		file.close();
		file.open(name);
		file << size << " ";
		for (int i = 0; i < size - 1; i++)
		{
			file << l[i].size - 1 << " ";
		}
		file << l[size - 1].size << endl;

		for (int i = 0; i < size - 1; i++)
		{
			for (int j = 0; j < w[i].l2size; j++)
			{
				for (int k = 0; k < w[i].l1size; k++)
				{
					if (k == w[i].l1size - 1)
					{
						file << w[i][k][j];
					}
					else
					{
						file << w[i][k][j] << " ";
					}
				}
				file << endl;
			}
		}


	}

	void loadFromFile(string name)
	{
		fstream file;
		file.open(name);
		if (!file)
		{
			cout << "File not found" << endl;
		}
		else
		{
			int sizeNetwork;
			file >> sizeNetwork;
			int* masArch = new int[sizeNetwork];
			for (int i = 0; i < sizeNetwork; i++)
			{
				file >> masArch[i];
			}
			setArchArr(sizeNetwork, masArch);

			for (int i = 0; i < size - 1; i++)
			{
				for (int j = 0; j < w[i].l2size; j++)
				{
					for (int k = 0; k < w[i].l1size; k++)
					{
						file >> w[i][k][j];
					}
				}
			}
		}
	}

	void setInputLayer(vec &a)
	{
		/*vec fff(a.size + 1);
		for (int i = 0; i < a.size; i++)
		{
			fff[i] = a[i];
		}
		fff[a.size] = 1;
		l[0] = fff;*/
		for (int i = 0; i < a.size; i++)
		{
			l[0][i] = a[i];
		}
		if (useBias)
		{
			l[0][a.size] = 1;
		}
	}

	void setRandWeights(double a, double b, int ton)
	{
		for (int i = 0; i < size - 1; i++)
		{
			w[i].setRandVal(a, b, ton);
		}
	}

	void work()
	{
		for (int i = 1; i < size; i++)
		{
			/*vec layer = w[i - 1] * l[i - 1];
			layer.activate();
			l[i].copyData(layer);*/

			for (int j = 0; j < w[i - 1].l2size; j++)
			{
				l[i][j] = 0;
				for (int k = 0; k < w[i - 1].l1size; k++)
				{
					l[i][j] += l[i - 1][k] * w[i - 1][k][j];
				}
			}
			l[i].activate();
			

			/*l[i] = w[i - 1] * l[i - 1];
			l[i].activate();
			if (i != size - 1)
			{
				l[i][size - 1] = 1;
			}*/
			///l[i] = l[i] + w[i - 1][w[i - 1].l1size]; - уже не нужно(вроде)
		}
	}

	vec getOutputLayer()
	{
		return l[size - 1];
	}

	void setError(vec &out)
	{
		/*lError[size - 1] = out - l[size - 1];
		for (int j = 0; j < l[size - 1].size; j++)
		{
			lError[size - 1][j] = lError[size - 1][j] * actsh(l[size - 1][j]);
		}

		for (int i = size - 2; i >= 0; i--)
		{
			lError[i] = w[i].swapLinesCol() * lError[i + 1];
			for (int j = 0; j < l[i].size - 1; j++)    // Не знаю нужна ли -1
			{
				lError[i][j] = lError[i][j] * actsh(l[i][j]);
			}
		}*/

		
		for (int i = 0; i < lError[size - 1].size; i++)
		{
			lError[size - 1][i] = out[i] - l[size - 1][i];
		}

		for (int j = 0; j < l[size - 1].size; j++)
		{
			lError[size - 1][j] = lError[size - 1][j] * actsh(l[size - 1][j]);
		}

		for (int i = size - 2; i >= 0; i--)
		{
			for (int j = 0; j < w[i].l1size; j++)
			{
				lError[i][j] = 0;
				for (int k = 0; k < w[i].l2size; k++)
				{
					lError[i][j] += lError[i + 1][k] * w[i][j][k];
				}
			}

			for (int j = 0; j < l[i].size - 1; j++)    // Не знаю нужна ли -1
			{
				lError[i][j] = lError[i][j] * actsh(l[i][j]);
			}
		}




	}


	void updateWeights(double q)
	{
		for (int i = 0; i < size - 1; i++)
		{
			for (int j = 0; j < w[i].l2size; j++)
			{
				for (int k = 0; k < w[i].l1size; k++)
				{
					w[i][k][j] = w[i][k][j] + q * lError[i + 1][j] * l[i][k];
				}
			}
		}
	}

	void updateWeightsHebb(double a, double q)
	{
		for (int i = 0; i < size - 1; i++)
		{
			for (int j = 0; j < w[i].l2size; j++)
			{
				for (int k = 0; k < w[i].l1size; k++)
				{
					if (i == 0)
					{
						w[i][k][j] = a * (l[i][k] + w[i][k][j]);
					}
					else
					{
						w[i][k][j] = q * (l[i][k] + w[i][k][j]);
					}

				}
			}
		}

	}

	void setArchArr(int size, int* mas)
	{
		this->size = size;
		l = new vec[size];
		lError = new vec[size];
		for (int i = 0; i < this->size; i++)
		{
			int idx = mas[i];
			if (i < this->size - 1 && useBias)
			{
				l[i].setSize(idx + 1);
				l[i].useBias = true;
				lError[i].setSize(idx + 1);
				lError[i].useBias = true;
			}
			else
			{
				l[i].setSize(idx);
				lError[i].setSize(idx);
			}
		}
		if (useBias)
		{
			for (int i = 0; i < this->size - 1; i++)
			{
				l[i][l[i].size - 1] = 1;
				lError[i][lError[i].size - 1] = 1;
			}
		}
		w = new mat[size - 1];
		for (int i = 0; i < this->size - 1; i++)
		{
			if (i < this->size - 2 && useBias)
			{
				w[i].setSize(l[i].size, l[i + 1].size - 1);
			}
			else
			{
				w[i].setSize(l[i].size, l[i + 1].size);
			}
		}
	}

	void setArch(int size, ...)
	{
		this->size = size;
		l = new vec[size];
		lError = new vec[size];
		va_list va;
		va_start(va, size);
		for (int i = 0; i < this->size; i++)
		{
			int idx = va_arg(va, int);
			if (i < this->size - 1 && useBias)
			{
				
				l[i].setSize(idx + 1);
				l[i].useBias = true;
				lError[i].setSize(idx + 1);
				lError[i].useBias = true;			
			}
			else
			{
				l[i].setSize(idx);
				lError[i].setSize(idx);
			}
		}
		va_end(va);
		if (useBias)
		{
			for (int i = 0; i < this->size - 1; i++)
			{
				l[i][l[i].size - 1] = 1;
				lError[i][lError[i].size - 1] = 1;
			}
		}
		



		w = new mat[size - 1];
		for (int i = 0; i < this->size - 1; i++)
		{
			if (i < this->size - 2 && useBias)
			{
				w[i].setSize(l[i].size, l[i + 1].size - 1);
			}
			else
			{
				w[i].setSize(l[i].size, l[i + 1].size);
			}
		}

	}

	void deleteNetwork()
	{
		for (int i = 0; i < size; i++)
		{
			l[i].deleteStruct();
			lError[i].deleteStruct();
		}
		delete[] l;
		delete[] lError;
		for (int i = 0; i < size - 1; i++)
		{
			w[i].deleteStruct();
		}
		delete[] w;
	}

	~Network()
	{
		deleteNetwork();
	}

};
