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

constexpr uint64_t chunk_size(64);
constexpr chrono::milliseconds chunk_wait(1);

enum class mode : uint64_t
{
	sync,
	async
};

template <typename T>
void mtx_wrap_beg
(
	const T* n_mtx_1,
	const T* n_mtx_2,
	T* n_mtx_3,
	uint64_t nx,
	uint64_t ny,
	uint64_t nz,
	T*& mtx_1,
	T*& mtx_2,
	T*& mtx_3,
	uint64_t& mx,
	uint64_t& my,
	uint64_t& mz
)
{
	function<uint64_t(uint64_t)> mtx_wrap_size = [](uint64_t m)
	{
		constexpr uint64_t double_chunk = chunk_size * 2ULL, undouble_chunk = chunk_size * 2ULL - 1ULL;
		return m == 0ULL ? double_chunk : ((m + undouble_chunk) / double_chunk) * double_chunk;
	};

	mx = mtx_wrap_size(nx);
	my = mtx_wrap_size(ny);
	mz = mtx_wrap_size(nz);

	if (mx == nx && my == ny)
		mtx_1 = (T*)n_mtx_1;
	else
	{
		mtx_1 = new T[mx * my];

		for (uint64_t ni = 0; ni < nx; ++ni)
		{
			for (uint64_t nj = 0; nj < ny; ++nj)
				mtx_1[ni * my + nj] = n_mtx_1[ni * ny + nj];

			for (uint64_t mj = ny; mj < my; ++mj)
				mtx_1[ni * my + mj] = T(0);
		}

		for (uint64_t m = nx * my; m < mx * my; ++m)
			mtx_1[m] = T(0);
	}

	if (my == ny && mz == nz)
		mtx_2 = (T*)n_mtx_2;
	else
	{
		mtx_2 = new T[my * mz];

		for (uint64_t nj = 0; nj < ny; ++nj)
		{
			for (uint64_t nk = 0; nk < nz; ++nk)
				mtx_2[nj * mz + nk] = n_mtx_2[nj * nz + nk];

			for (uint64_t mk = nz; mk < mz; ++mk)
				mtx_2[nj * mz + mk] = T(0);
		}

		for (uint64_t m = ny * mz; m < my * mz; ++m)
			mtx_2[m] = T(0);
	}

	if (mx == nx && mz == nz)
		mtx_3 = (T*)n_mtx_3;
	else
		mtx_3 = new T[mx * mz];
}

template <typename T>
void mtx_wrap_end
(
	const T* n_mtx_1,
	const T* n_mtx_2,
	T* n_mtx_3,
	uint64_t nx,
	uint64_t ny,
	uint64_t nz,
	T* mtx_1,
	T* mtx_2,
	T* mtx_3,
	uint64_t mx,
	uint64_t my,
	uint64_t mz
)
{
	if (!(mx == nx && my == ny))
		delete[] mtx_1;

	if (!(my == ny && mz == nz))
		delete[] mtx_2;

	if (!(mx == nx && mz == nz))
	{
		for (uint64_t ni = 0; ni < nx; ++ni)
			for (uint64_t nk = 0; nk < nz; ++nk)
				n_mtx_3[ni * nz + nk] = mtx_3[ni * mz + nk];

		delete[] mtx_3;
	}
}

template <typename T>
void mtx_multiply_2(const T* mtx_1, const T* mtx_2, T* mtx_3, uint64_t mx, uint64_t my, uint64_t mz)
{
	for (uint64_t m = 0; m < mx * mz; ++m)
		mtx_3[m] = T(0);

	for (uint64_t mi = 0; mi < mx; ++mi)
		for (uint64_t mk = 0; mk < my; ++mk)
			for (uint64_t mj = 0; mj < mz; ++mj)
				mtx_3[mi * mz + mj] += mtx_1[mi * my + mk] * mtx_2[mk * mz + mj];
}

template <typename T>
void mtx_multiply
(
	const T* mtx_1,
	const T* mtx_2,
	T* mtx_3,
	uint64_t mx,
	uint64_t my,
	uint64_t mz,
	const bool* chk_1 = nullptr,
	const bool* chk_2 = nullptr,
	bool* chk_3 = nullptr
)
{
	uint64_t cx = mx / chunk_size, cy = my / chunk_size, cz = mz / chunk_size;

	for (uint64_t m = 0; m < mx * mz; ++m)
		mtx_3[m] = T(0);

	for (uint64_t mi = 0, ci = 0; mi < mx; mi += chunk_size, ++ci)
		for (uint64_t mk = 0, ck = 0; mk < mz; mk += chunk_size, ++ck)
		{
			for (uint64_t mj = 0, cj = 0; mj < my; mj += chunk_size, ++cj)
			{
				if (chk_1)
					while (!chk_1[ci * cy + cj])
						this_thread::sleep_for(chunk_wait);

				if (chk_2)
					while (!chk_2[cj * cz + ck])
						this_thread::sleep_for(chunk_wait);

				for (uint64_t i = mi; i < mi + chunk_size; ++i)
					for (uint64_t j = mj; j < mj + chunk_size; ++j)
						for (uint64_t k = mk; k < mk + chunk_size; ++k)
							mtx_3[i * mz + k] += mtx_1[i * my + j] * mtx_2[j * mz + k];
			}

			if (chk_3)
				chk_3[ci * cz + ck] = true;
		}
}

template <typename T>
void mtx_chunk_receiver
(
	T* mtx,
	uint64_t m1,
	uint64_t m2,
	MPI_Comm com,
	int src,
	int tag,
	bool inv_traversal,
	bool* chk = nullptr
)
{
	uint64_t c1 = m1 / chunk_size, c2 = m2 / chunk_size;
	T* buf = new T[chunk_size * chunk_size];

	if (inv_traversal)
		for (uint64_t mj = 0, cj = 0; mj < m2; mj += chunk_size, ++cj)
			for (uint64_t mi = 0, ci = 0; mi < m1; mi += chunk_size, ++ci)
			{
				MPI_Status mpi_status;
				MPI_Recv((void*)buf, (int)(chunk_size * chunk_size * sizeof(T)), MPI_BYTE, src, tag, com, &mpi_status);

				for (uint64_t i = mi, ii = 0; i < mi + chunk_size; ++i, ++ii)
					for (uint64_t j = mj, jj = 0; j < mj + chunk_size; ++j, ++jj)
						mtx[i * m2 + j] = buf[ii * chunk_size + jj];

				if (chk)
					chk[ci * c2 + cj] = true;
			}
	else
		for (uint64_t mi = 0, ci = 0; mi < m1; mi += chunk_size, ++ci)
			for (uint64_t mj = 0, cj = 0; mj < m2; mj += chunk_size, ++cj)
			{
				MPI_Status mpi_status;
				MPI_Recv((void*)buf, (int)(chunk_size * chunk_size * sizeof(T)), MPI_BYTE, src, tag, com, &mpi_status);

				for (uint64_t i = mi, ii = 0; i < mi + chunk_size; ++i, ++ii)
					for (uint64_t j = mj, jj = 0; j < mj + chunk_size; ++j, ++jj)
						mtx[i * m2 + j] = buf[ii * chunk_size + jj];

				if (chk)
					chk[ci * c2 + cj] = true;
			}

	delete[] buf;
}

template <typename T>
void mtx_chunk_sender
(
	const T* mtx,
	uint64_t m1,
	uint64_t m2,
	MPI_Comm com,
	int dst,
	int tag,
	bool inv_traversal,
	const bool* chk = nullptr
)
{
	uint64_t c1 = m1 / chunk_size, c2 = m2 / chunk_size;
	T* buf = new T[chunk_size * chunk_size];

	if (inv_traversal)
		for (uint64_t mj = 0, cj = 0; mj < m2; mj += chunk_size, ++cj)
			for (uint64_t mi = 0, ci = 0; mi < m1; mi += chunk_size, ++ci)
			{
				if (chk)
					while (!chk[ci * c2 + cj])
						this_thread::sleep_for(chunk_wait);

				for (uint64_t i = mi, ii = 0; i < mi + chunk_size; ++i, ++ii)
					for (uint64_t j = mj, jj = 0; j < mj + chunk_size; ++j, ++jj)
						buf[ii * chunk_size + jj] = mtx[i * m2 + j];

				MPI_Send((void*)buf, (int)(chunk_size * chunk_size * sizeof(T)), MPI_BYTE, dst, tag, com);
			}
	else
		for (uint64_t mi = 0, ci = 0; mi < m1; mi += chunk_size, ++ci)
			for (uint64_t mj = 0, cj = 0; mj < m2; mj += chunk_size, ++cj)
			{
				if (chk)
					while (!chk[ci * c2 + cj])
						this_thread::sleep_for(chunk_wait);

				for (uint64_t i = mi, ii = 0; i < mi + chunk_size; ++i, ++ii)
					for (uint64_t j = mj, jj = 0; j < mj + chunk_size; ++j, ++jj)
						buf[ii * chunk_size + jj] = mtx[i * m2 + j];

				MPI_Send((void*)buf, (int)(chunk_size * chunk_size * sizeof(T)), MPI_BYTE, dst, tag, com);
			}

	delete[] buf;
}

template <typename T>
void mtx_master_worker
(
	const T* mtx_1,
	const T* mtx_2,
	T* mtx_3,
	uint64_t mx,
	uint64_t my,
	uint64_t mz,
	uint64_t dx,
	uint64_t dz,
	mode __mode,
	int base_tag,
	int slave_rank,
	MPI_Comm comm
)
{
	MPI_Status mpi_status;

	MPI_Send((void*)&base_tag, (int)sizeof(int), MPI_BYTE, slave_rank, 0, comm);
	MPI_Send((void*)&__mode, (int)sizeof(mode), MPI_BYTE, slave_rank, base_tag + 0, comm);

	switch (__mode)
	{
	case mode::sync:
	{
		uint64_t x = mx / 2, y = my, z = mz / 2;

		MPI_Send((void*)&x, (int)sizeof(uint64_t), MPI_BYTE, slave_rank, base_tag + 1, comm);
		MPI_Send((void*)&y, (int)sizeof(uint64_t), MPI_BYTE, slave_rank, base_tag + 2, comm);
		MPI_Send((void*)&z, (int)sizeof(uint64_t), MPI_BYTE, slave_rank, base_tag + 3, comm);

		T* __mtx_1 = new T[x * y];
		T* __mtx_2 = new T[y * z];
		T* __mtx_3 = new T[x * z];

		for (uint64_t i = 0; i < x; ++i)
			for (uint64_t j = 0; j < y; ++j)
				__mtx_1[i * y + j] = mtx_1[(dx + i) * my + j];

		for (uint64_t i = 0; i < y; ++i)
			for (uint64_t j = 0; j < z; ++j)
				__mtx_2[i * z + j] = mtx_2[i * mz + (dz + j)];

		MPI_Send((void*)__mtx_1, (int)(x * y * sizeof(T)), MPI_BYTE, slave_rank, base_tag + 4, comm);
		MPI_Send((void*)__mtx_2, (int)(y * z * sizeof(T)), MPI_BYTE, slave_rank, base_tag + 5, comm);
		MPI_Recv((void*)__mtx_3, (int)(x * z * sizeof(T)), MPI_BYTE, slave_rank, base_tag + 6, comm, &mpi_status);

		for (uint64_t i = 0; i < x; ++i)
			for (uint64_t j = 0; j < z; ++j)
				mtx_3[(dx + i) * mz + (dz + j)] = __mtx_3[i * z + j];

		delete[] __mtx_1;
		delete[] __mtx_2;
		delete[] __mtx_3;

		break;
	}

	case mode::async:
	{
		uint64_t x = mx / 2, y = my, z = mz / 2;

		MPI_Send((void*)&x, (int)sizeof(uint64_t), MPI_BYTE, slave_rank, base_tag + 1, comm);
		MPI_Send((void*)&y, (int)sizeof(uint64_t), MPI_BYTE, slave_rank, base_tag + 2, comm);
		MPI_Send((void*)&z, (int)sizeof(uint64_t), MPI_BYTE, slave_rank, base_tag + 3, comm);

		T* __mtx_1 = new T[x * y];
		T* __mtx_2 = new T[y * z];
		T* __mtx_3 = new T[x * z];

		for (uint64_t i = 0; i < x; ++i)
			for (uint64_t j = 0; j < y; ++j)
				__mtx_1[i * y + j] = mtx_1[(dx + i) * my + j];

		for (uint64_t i = 0; i < y; ++i)
			for (uint64_t j = 0; j < z; ++j)
				__mtx_2[i * z + j] = mtx_2[i * mz + (dz + j)];

		thread sender_1([=]()
			{
				mtx_chunk_sender(__mtx_1, x, y, comm, slave_rank, base_tag + 4, false);
			});
		thread sender_2([=]()
			{
				mtx_chunk_sender(__mtx_2, y, z, comm, slave_rank, base_tag + 5, true);
			});

		sender_1.join();
		sender_2.join();

		mtx_chunk_receiver(__mtx_3, x, z, comm, slave_rank, base_tag + 6, false);

		for (uint64_t i = 0; i < x; ++i)
			for (uint64_t j = 0; j < z; ++j)
				mtx_3[(dx + i) * mz + (dz + j)] = __mtx_3[i * z + j];

		delete[] __mtx_1;
		delete[] __mtx_2;
		delete[] __mtx_3;

		break;
	}

	default:
		cout << "Shit happens\n";
	}
}

template <typename T>
void mtx_master
(
	const T* n_mtx_1,
	const T* n_mtx_2,
	T* n_mtx_3,
	uint64_t nx,
	uint64_t ny,
	uint64_t nz,
	mode __mode,
	int range,
	MPI_Comm comm
)
{
	int base_tag = 1;
	uint64_t mx, my, mz;

	T* mtx_1;
	T* mtx_2;
	T* mtx_3;

	mtx_wrap_beg(n_mtx_1, n_mtx_2, n_mtx_3, nx, ny, nz, mtx_1, mtx_2, mtx_3, mx, my, mz);

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
			mtx_master_worker(mtx_1, mtx_2, mtx_3, mx, my, mz, 0, 0, __mode, base_tag + 8 * 0, (0 % (range - 1)) + 1, comm);
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
			mtx_master_worker(mtx_1, mtx_2, mtx_3, mx, my, mz, 0, mz / 2, __mode, base_tag + 8 * 1, (1 % (range - 1)) + 1, comm);
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
			mtx_master_worker(mtx_1, mtx_2, mtx_3, mx, my, mz, mx / 2, 0, __mode, base_tag + 8 * 2, (2 % (range - 1)) + 1, comm);
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
			mtx_master_worker(mtx_1, mtx_2, mtx_3, mx, my, mz, mx / 2, mz / 2, __mode, base_tag + 8 * 3, (3 % (range - 1)) + 1, comm);
		});

	worker_1.join();
	worker_2.join();
	worker_3.join();
	worker_4.join();

	mtx_wrap_end(n_mtx_1, n_mtx_2, n_mtx_3, nx, ny, nz, mtx_1, mtx_2, mtx_3, mx, my, mz);
}

template <typename T>
void mtx_slave_async_worker
(
	T* mtx_1,
	T* mtx_2,
	const T* mtx_3,
	uint64_t mx,
	uint64_t my,
	uint64_t mz,
	bool* chk_1,
	bool* chk_2,
	const bool* chk_3,
	int base_tag,
	MPI_Comm comm
)
{
	thread receiver_1_thread([=]()
		{
			mtx_chunk_receiver(mtx_1, mx, my, comm, 0, base_tag + 4, false, chk_1);
		});
	thread receiver_2_thread([=]()
		{
			mtx_chunk_receiver(mtx_2, my, mz, comm, 0, base_tag + 5, true, chk_2);
		});

	receiver_1_thread.join();
	receiver_2_thread.join();

	mtx_chunk_sender(mtx_3, mx, mz, comm, 0, base_tag + 6, false, chk_3);
}

template <typename T>
void mtx_slave_worker(MPI_Comm comm)
{
	mode __mode;
	int base_tag;
	MPI_Status mpi_status;

	MPI_Recv((void*)&base_tag, (int)sizeof(int), MPI_BYTE, 0, 0, comm, &mpi_status);

	if (base_tag == -1)
		throw exception();

	MPI_Recv((void*)&__mode, (int)sizeof(mode), MPI_BYTE, 0, base_tag + 0, comm, &mpi_status);

	switch (__mode)
	{
	case mode::sync:
	{
		uint64_t x, y, z;

		MPI_Recv((void*)&x, (int)sizeof(uint64_t), MPI_BYTE, 0, base_tag + 1, comm, &mpi_status);
		MPI_Recv((void*)&y, (int)sizeof(uint64_t), MPI_BYTE, 0, base_tag + 2, comm, &mpi_status);
		MPI_Recv((void*)&z, (int)sizeof(uint64_t), MPI_BYTE, 0, base_tag + 3, comm, &mpi_status);

		T* __mtx_1 = new T[x * y];
		T* __mtx_2 = new T[y * z];
		T* __mtx_3 = new T[x * z];

		MPI_Recv((void*)__mtx_1, (int)(x * y * sizeof(T)), MPI_BYTE, 0, base_tag + 4, comm, &mpi_status);
		MPI_Recv((void*)__mtx_2, (int)(y * z * sizeof(T)), MPI_BYTE, 0, base_tag + 5, comm, &mpi_status);

		mtx_multiply(__mtx_1, __mtx_2, __mtx_3, x, y, z);

		MPI_Send((void*)__mtx_3, (int)(x * z * sizeof(T)), MPI_BYTE, 0, base_tag + 6, comm);

		delete[] __mtx_1;
		delete[] __mtx_2;
		delete[] __mtx_3;

		break;
	}

	case mode::async:
	{
		uint64_t x, y, z;

		MPI_Recv((void*)&x, (int)sizeof(uint64_t), MPI_BYTE, 0, base_tag + 1, comm, &mpi_status);
		MPI_Recv((void*)&y, (int)sizeof(uint64_t), MPI_BYTE, 0, base_tag + 2, comm, &mpi_status);
		MPI_Recv((void*)&z, (int)sizeof(uint64_t), MPI_BYTE, 0, base_tag + 3, comm, &mpi_status);

		uint64_t cx = x / chunk_size, cy = y / chunk_size, cz = z / chunk_size;

		T* __mtx_1 = new T[x * y];
		T* __mtx_2 = new T[y * z];
		T* __mtx_3 = new T[x * z];

		bool* __chk_1 = new bool[cx * cy]();
		bool* __chk_2 = new bool[cy * cz]();
		bool* __chk_3 = new bool[cx * cz]();

		thread slave_async_worker_thread([=]()
			{
				mtx_slave_async_worker(__mtx_1, __mtx_2, __mtx_3, x, y, z, __chk_1, __chk_2, __chk_3, base_tag, comm);
			});

		mtx_multiply(__mtx_1, __mtx_2, __mtx_3, x, y, z, __chk_1, __chk_2, __chk_3);

		slave_async_worker_thread.join();

		delete[] __mtx_1;
		delete[] __mtx_2;
		delete[] __mtx_3;

		delete[] __chk_1;
		delete[] __chk_2;
		delete[] __chk_3;

		break;
	}

	default:
		cout << "Shit happens\n";
	}
}

template <typename T>
void mtx_slave(MPI_Comm comm)
{
	while (true)
	{
		try
		{
			mtx_slave_worker<T>(comm);
		}
		catch (...)
		{
			break;
		}
	}
}

template <typename T>
inline T custom_fabs(T x)
{
	return x >= T(0) ? x : -x;
}

template <typename T>
inline bool almost_equal(T x, T y, T e = T(1e-5))
{
	return custom_fabs(x - y) <= e * custom_fabs(x + y);
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

	MPI_Comm group_comm;
	uint64_t group_count;

	if (argc >= 6)
		group_count = stoull(argv[4]);
	else
	{
		if (world_rank == 0)
			cout << "Usage: <uint64 x> <uint64 y> <uint64 z> <uint64 group count> <mode>\n";

		exit(666);
	}

	uint64_t group_num = world_rank % group_count + 1;
	MPI_Comm_split(MPI_COMM_WORLD, world_rank % group_count, world_rank, &group_comm);

	int group_rank;
	MPI_Comm_rank(group_comm, &group_rank);

	int group_size;
	MPI_Comm_size(group_comm, &group_size);

	if (group_rank == 0)
	{
		mode __mode;
		uint64_t x, y, z;

		try
		{
			if (argc < 6)
				throw exception();

			x = stoull(argv[1]);
			y = stoull(argv[2]);
			z = stoull(argv[3]);

			string mode_str(argv[5]);

			if (mode_str == "sync")
				__mode = mode::sync;
			else if (mode_str == "async")
				__mode = mode::async;
			else
				throw exception();
		}
		catch (...)
		{
			if (world_rank == 0)
				cout << "Usage: <uint64 x> <uint64 y> <uint64 z> <uint64 group count> <mode>\n";

			exit(666);
		}

		mtx_data* mtx_1 = new mtx_data[x * y];
		mtx_data* mtx_2 = new mtx_data[y * z];
		mtx_data* mtx_3 = new mtx_data[x * z];
		mtx_data* mtx_check = new mtx_data[x * z];

		mt19937 gen((random_device())());
		uniform_real_distribution<mtx_data> dis(-1.0, 1.0);

		for (uint64_t i = 0; i < x * y; ++i)
			mtx_1[i] = (mtx_data)dis(gen);

		for (uint64_t i = 0; i < y * z; ++i)
			mtx_2[i] = (mtx_data)dis(gen);

		bool check_done = false;
		thread check_worker([mtx_1, mtx_2, mtx_check, x, y, z, group_num, &check_done]()
			{
				auto start = chrono::high_resolution_clock::now();
				mtx_multiply_2(mtx_1, mtx_2, mtx_check, x, y, z);
				auto end = chrono::high_resolution_clock::now();

				cout << "GROUP #" << group_num << " MASTER TIME: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " milliseconds\n";
				check_done = true;
			});

		auto start = chrono::high_resolution_clock::now();
		mtx_master(mtx_1, mtx_2, mtx_3, x, y, z, __mode, group_size, group_comm);
		auto end = chrono::high_resolution_clock::now();

		cout << "GROUP #" << group_num << " SLAVES TIME: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " milliseconds\n";

		if (!check_done)
			cout << "GROUP #" << group_num << " Waiting master to finish checking...\n";

		check_worker.join();

		uint64_t mismatch_count = 0;

		for (uint64_t i = 0; i < x * z; ++i)
			if (!almost_equal(mtx_3[i], mtx_check[i]))
				++mismatch_count;

		cout << "GROUP #" << group_num << " End of check. Mismatch count: " << mismatch_count << "\n";

		for (int i = 1, false_tag = -1; i < group_size; ++i)
			MPI_Send((void*)&false_tag, (int)sizeof(int), MPI_BYTE, i, 0, group_comm);

		delete[] mtx_1;
		delete[] mtx_2;
		delete[] mtx_3;
		delete[] mtx_check;
	}
	else
		mtx_slave<mtx_data>(group_comm);

	MPI_Finalize();
}