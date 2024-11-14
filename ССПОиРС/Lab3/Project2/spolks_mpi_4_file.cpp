#include <chrono>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <random>
#include <thread>
#include <mpi.h>

using namespace std;

typedef double mtx_data;

enum class mode : uint64_t
{
	sync,
	async
};

template <typename T>
void mtx_multiply(const T* mtx_1, const T* mtx_2, T* mtx_3, uint64_t mx, uint64_t my, uint64_t mz)
{
	for (uint64_t m = 0; m < mx * mz; ++m)
		mtx_3[m] = T(0);

	for (uint64_t mi = 0; mi < mx; ++mi)
		for (uint64_t mk = 0; mk < my; ++mk)
			for (uint64_t mj = 0; mj < mz; ++mj)
				mtx_3[mi * mz + mj] += mtx_1[mi * my + mk] * mtx_2[mk * mz + mj];
}

template <typename T>
void mtx_master_worker
(
	string mtx_1_path,
	string mtx_2_path,
	string mtx_3_path,
	uint64_t dx,
	uint64_t dz,
	int base_tag,
	int slave_rank,
	int comm_tag
)
{
	MPI_Status mpi_status;

	MPI_Send((void*)&base_tag, (int)sizeof(int), MPI_BYTE, slave_rank, comm_tag, MPI_COMM_WORLD);
	base_tag += comm_tag;

	uint64_t d_xz[2] = { dx, dz };
	MPI_Send((void*)d_xz, (int)sizeof(d_xz), MPI_BYTE, slave_rank, base_tag + 0, MPI_COMM_WORLD);

	size_t mtx_1_path_length=mtx_1_path.size();
	MPI_Send((void*)&mtx_1_path_length, (int)sizeof(size_t), MPI_BYTE, slave_rank, base_tag + 1, MPI_COMM_WORLD);
	MPI_Send((void*)mtx_1_path.c_str(), (int)(mtx_1_path.size() * sizeof(char)), MPI_BYTE, slave_rank, base_tag + 2, MPI_COMM_WORLD);

	size_t mtx_2_path_length=mtx_2_path.size();
	MPI_Send((void*)&mtx_2_path_length, (int)sizeof(size_t), MPI_BYTE, slave_rank, base_tag + 3, MPI_COMM_WORLD);
	MPI_Send((void*)mtx_2_path.c_str(), (int)(mtx_2_path.size() * sizeof(char)), MPI_BYTE, slave_rank, base_tag + 4, MPI_COMM_WORLD);

	size_t mtx_3_path_length=mtx_3_path.size();
	MPI_Send((void*)&mtx_3_path_length, (int)sizeof(size_t), MPI_BYTE, slave_rank, base_tag + 5, MPI_COMM_WORLD);
	MPI_Send((void*)mtx_3_path.c_str(), (int)(mtx_3_path.size() * sizeof(char)), MPI_BYTE, slave_rank, base_tag + 6, MPI_COMM_WORLD);

	uint8_t garbage;
	MPI_Recv((void*)&garbage, (int)sizeof(uint8_t), MPI_BYTE, slave_rank, base_tag + 7, MPI_COMM_WORLD,&mpi_status);
}

template <typename T>
void mtx_master
(
	string mtx_1_path,
	string mtx_2_path,
	string mtx_3_path,
	int range,
	int comm_tag,
	int base_rank,
	int group_count
)
{
	int base_tag = 1;

	//           1 1 0 0
	//           1 1 0 0
	//           1 1 0 0
	//           1 1 0 0
	//                  
	// 1 1 1 1   1 1 0 0
	// 1 1 1 1   1 1 0 0
	// 0 0 0 0   0 0 0 0
	// 0 0 0 0   0 0 0 0
	thread worker_1([=]()
		{
			mtx_master_worker<T>(mtx_1_path, mtx_2_path, mtx_3_path,0,0, base_tag + 8 * 0, ((0 % (range - 1)) + 1) * group_count + base_rank, comm_tag);
		});

	//           0 0 1 1
	//           0 0 1 1
	//           0 0 1 1
	//           0 0 1 1
	//                  
	// 1 1 1 1   0 0 1 1
	// 1 1 1 1   0 0 1 1
	// 0 0 0 0   0 0 0 0
	// 0 0 0 0   0 0 0 0
	thread worker_2([=]()
		{
			mtx_master_worker<T>(mtx_1_path, mtx_2_path, mtx_3_path,0,1, base_tag + 8 * 1, ((1 % (range - 1)) + 1) * group_count + base_rank, comm_tag);
		});

	//           1 1 0 0
	//           1 1 0 0
	//           1 1 0 0
	//           1 1 0 0
	//                  
	// 0 0 0 0   0 0 0 0
	// 0 0 0 0   0 0 0 0
	// 1 1 1 1   1 1 0 0
	// 1 1 1 1   1 1 0 0
	thread worker_3([=]()
		{
			mtx_master_worker<T>(mtx_1_path, mtx_2_path, mtx_3_path,1,0, base_tag + 8 * 2, ((2 % (range - 1)) + 1) * group_count + base_rank, comm_tag);
		});

	//           0 0 1 1
	//           0 0 1 1
	//           0 0 1 1
	//           0 0 1 1
	//                  
	// 0 0 0 0   0 0 0 0
	// 0 0 0 0   0 0 0 0
	// 1 1 1 1   0 0 1 1
	// 1 1 1 1   0 0 1 1
	thread worker_4([=]()
		{
			mtx_master_worker<T>(mtx_1_path, mtx_2_path, mtx_3_path,1,1, base_tag + 8 * 3, ((3 % (range - 1)) + 1) * group_count + base_rank, comm_tag);
		});

	worker_1.join();
	worker_2.join();
	worker_3.join();
	worker_4.join();
}

template <typename T>
void mtx_slave_worker(int comm_tag, int base_rank)
{
	int base_tag;
	MPI_Status mpi_status;

	MPI_Recv((void*)&base_tag, (int)sizeof(int), MPI_BYTE, base_rank, comm_tag, MPI_COMM_WORLD, &mpi_status);

	if (base_tag == -1)
		throw exception();
	else
		base_tag += comm_tag;

	uint64_t d_xz[2];
	MPI_Recv((void*)d_xz, (int)sizeof(d_xz), MPI_BYTE, base_rank, base_tag + 0, MPI_COMM_WORLD,&mpi_status);
	uint64_t dx=d_xz[0],dz=d_xz[1];

	size_t mtx_1_path_length;
	MPI_Recv((void*)&mtx_1_path_length, (int)sizeof(size_t), MPI_BYTE, base_rank, base_tag + 1, MPI_COMM_WORLD,&mpi_status);
	string mtx_1_path(mtx_1_path_length, (char)0);
	MPI_Recv((void*)mtx_1_path.c_str(), (int)(mtx_1_path_length * sizeof(char)), MPI_BYTE, base_rank, base_tag + 2, MPI_COMM_WORLD,&mpi_status);

	size_t mtx_2_path_length;
	MPI_Recv((void*)&mtx_2_path_length, (int)sizeof(size_t), MPI_BYTE, base_rank, base_tag + 3, MPI_COMM_WORLD,&mpi_status);
	string mtx_2_path(mtx_2_path_length, (char)0);
	MPI_Recv((void*)mtx_2_path.c_str(), (int)(mtx_2_path_length * sizeof(char)), MPI_BYTE, base_rank, base_tag + 4, MPI_COMM_WORLD,&mpi_status);

	size_t mtx_3_path_length;
	MPI_Recv((void*)&mtx_3_path_length, (int)sizeof(size_t), MPI_BYTE, base_rank, base_tag + 5, MPI_COMM_WORLD,&mpi_status);
	string mtx_3_path(mtx_3_path_length, (char)0);
	MPI_Recv((void*)mtx_3_path.c_str(), (int)(mtx_3_path_length * sizeof(char)), MPI_BYTE, base_rank, base_tag + 6, MPI_COMM_WORLD,&mpi_status);

	constexpr uint64_t mtx_sz_ag=sizeof(uint64_t)*2Ui64;

	ifstream mtx_1_file(mtx_1_path,ios::binary);
	ifstream mtx_2_file(mtx_2_path,ios::binary);

	uint64_t mx, my, mz, x, y, z;

	mtx_1_file.read((char*)&mx, sizeof(uint64_t));
	mtx_2_file.read((char*)&my, sizeof(uint64_t));
	mtx_2_file.read((char*)&mz, sizeof(uint64_t));

	if (dx == 0Ui64)
		x=mx/2Ui64;
	else
	{
		dx=mx/2Ui64;
		x=(mx+1Ui64)/2Ui64;
	}

	y=my;

	if (dz == 0Ui64)
		z=mz/2Ui64;
	else
	{
		dz=mz/2Ui64;
		z=(mz+1Ui64)/2Ui64;
	}

	T*mtx_1=new T[x*y];
	T*mtx_2=new T[y*z];
	T*mtx_3=new T[x*z];

	for(uint64_t i=0;i<x;++i)
	{
		mtx_1_file.seekg(((i+dx)*my)*sizeof(T)+mtx_sz_ag,ios::beg);
		mtx_1_file.read((char*)&(mtx_1[i*y]),y*sizeof(T));
	}

	for(uint64_t i=0;i<y;++i)
	{
		mtx_2_file.seekg((i*mz+dz)*sizeof(T)+mtx_sz_ag,ios::beg);
		mtx_2_file.read((char*)&(mtx_2[i*z]),z*sizeof(T));
	}

	mtx_1_file.close();
	mtx_2_file.close();

	mtx_multiply(mtx_1,mtx_2,mtx_3,x,y,z);

	while(true)
	{
		ofstream mtx_3_file(mtx_3_path,ios::binary|ios::in|ios::out);

		if(mtx_3_file.is_open())
		{
			for(uint64_t i=0;i<x;++i)
			{
				mtx_3_file.seekp(((i+dx)*mz+dz)*sizeof(T)+mtx_sz_ag,ios::beg);
				mtx_3_file.write((const char*)&(mtx_3[i*z]),z*sizeof(T));
			}

			mtx_3_file.close();
			break;
		}
		else
			this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	uint8_t garbage;
	MPI_Send((void*)&garbage, (int)sizeof(uint8_t), MPI_BYTE, base_rank, base_tag + 7, MPI_COMM_WORLD);
}

template <typename T>
void mtx_slave(int comm_tag, int base_rank)
{
	while (true)
	{
		try
		{
			mtx_slave_worker<T>(comm_tag, base_rank);
		}
		catch (...)
		{
			break;
		}
	}
}

int main(int argc, char* argv[], char* argp[])
{
	int provided;
	MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);

	if ((provided & MPI_THREAD_MULTIPLE) != MPI_THREAD_MULTIPLE)
		exit(666);

	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	uint64_t group_count;

	if (argc >= 2)
		group_count = stoull(argv[1]);
	else
	{
		if (world_rank == 0)
			cout << "Usage: <uint64 group count> <mtx1 file groupN path> <mtx2 file groupN path> <mtx3 file groupN path>\n";

		exit(666);
	}

	uint64_t group_num = (uint64_t)world_rank % group_count + 1Ui64;
	uint64_t group_rank = (uint64_t)world_rank / group_count;
	uint64_t group_size = (uint64_t)world_size / group_count;

	if (group_rank == 0)
	{
		string mtx_1_path, mtx_2_path, mtx_3_path;

		try
		{
			if (argc < 2 + group_count * 3)
				throw exception();

			mtx_1_path = argv[2 + (group_num - 1) * 3 + 0];
			mtx_2_path = argv[2 + (group_num - 1) * 3 + 1];
			mtx_3_path = argv[2 + (group_num - 1) * 3 + 2];
		}
		catch (...)
		{
			if (world_rank == 0)
				cout << "Usage: <uint64 group count> <mtx1 file groupN path> <mtx2 file groupN path> <mtx3 file groupN path>\n";

			exit(666);
		}

		auto start = chrono::high_resolution_clock::now();
		mtx_master<mtx_data>(mtx_1_path, mtx_2_path, mtx_3_path, group_size, (int)((group_num - 1Ui64) * 64Ui64), (int)(group_num - 1Ui64), (int)group_count);
		auto end = chrono::high_resolution_clock::now();

		cout << "GROUP #" << group_num << " SLAVES TIME: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " milliseconds\n";

		for (int i = 1, false_tag = -1; i < group_size; ++i)
			MPI_Send((void*)&false_tag, (int)sizeof(int), MPI_BYTE, i * (int)group_count + (int)(group_num - 1Ui64), (int)((group_num - 1Ui64) * 64Ui64), MPI_COMM_WORLD);
	}
	else
		mtx_slave<mtx_data>((int)((group_num - 1Ui64) * 64Ui64), (int)(group_num - 1Ui64));

	MPI_Finalize();
}