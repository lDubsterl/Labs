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

typedef double MatrixData;

constexpr uint64_t chunk_size = 64; // Размер блока данных
constexpr chrono::milliseconds ChunkProcessingDelay(1); // Задержка обработки блока

enum class ProcessingMode : uint64_t
{
	Synchronous, // Синхронный режим
	Asynchronous  // Асинхронный режим
};


template <typename T>
void wrapMatrixData(
	const T* inputMatrix1,
	const T* inputMatrix2,
	T* outputMatrix,
	uint64_t numRows1,
	uint64_t numCols1,
	uint64_t numCols2,
	T*& matrix1,
	T*& matrix2,
	T*& matrix3,
	uint64_t& wrappedRows1,
	uint64_t& wrappedCols1,
	uint64_t& wrappedCols2
)
{
	// Функция для вычисления размера матрицы с учетом выравнивания
	function<uint64_t(uint64_t)> calculateWrappedSize = [](uint64_t size)
		{
			constexpr uint64_t doubleChunk = chunk_size * 2Ui64;
			constexpr uint64_t undoubleChunk = chunk_size * 2Ui64 - 1Ui64;
			return size == 0Ui64 ? doubleChunk : ((size + undoubleChunk) / doubleChunk) * doubleChunk;
		};

	// Вычисление выровненных размеров для матриц
	wrappedRows1 = calculateWrappedSize(numRows1);
	wrappedCols1 = calculateWrappedSize(numCols1);
	wrappedCols2 = calculateWrappedSize(numCols2);

	// Обработка первой матрицы
	if (wrappedRows1 == numRows1 && wrappedCols1 == numCols1)
		matrix1 = (T*)inputMatrix1;
	else
	{
		matrix1 = new T[wrappedRows1 * wrappedCols1];

		for (uint64_t row = 0; row < numRows1; ++row)
		{
			for (uint64_t col = 0; col < numCols1; ++col)
				matrix1[row * wrappedCols1 + col] = inputMatrix1[row * numCols1 + col];

			// Заполнение оставшейся части нулями
			for (uint64_t col = numCols1; col < wrappedCols1; ++col)
				matrix1[row * wrappedCols1 + col] = T(0);
		}

		// Заполнение оставшейся части матрицы нулями
		for (uint64_t i = numRows1 * wrappedCols1; i < wrappedRows1 * wrappedCols1; ++i)
			matrix1[i] = T(0);
	}

	// Обработка второй матрицы
	if (wrappedCols1 == numCols1 && wrappedCols2 == numCols2)
		matrix2 = (T*)inputMatrix2;
	else
	{
		matrix2 = new T[wrappedCols1 * wrappedCols2];

		for (uint64_t row = 0; row < numCols1; ++row)
		{
			for (uint64_t col = 0; col < numCols2; ++col)
				matrix2[row * wrappedCols2 + col] = inputMatrix2[row * numCols2 + col];

			// Заполнение оставшейся части нулями
			for (uint64_t col = numCols2; col < wrappedCols2; ++col)
				matrix2[row * wrappedCols2 + col] = T(0);
		}

		// Заполнение оставшейся части матрицы нулями
		for (uint64_t i = numCols1 * wrappedCols2; i < wrappedCols1 * wrappedCols2; ++i)
			matrix2[i] = T(0);
	}

	// Обработка третьей матрицы
	if (wrappedRows1 == numRows1 && wrappedCols2 == numCols2)
		matrix3 = (T*)outputMatrix;
	else
		matrix3 = new T[wrappedRows1 * wrappedCols2];
}


template <typename T>
void wrapMatrixDataEnd(
	const T* inputMatrix1,
	const T* inputMatrix2,
	T* outputMatrix,
	uint64_t numRows1,
	uint64_t numCols1,
	uint64_t numCols2,
	T* matrix1,
	T* matrix2,
	T* matrix3,
	uint64_t wrappedRows1,
	uint64_t wrappedCols1,
	uint64_t wrappedCols2
)
{
	// Очистка выделенной памяти для выровненных матриц, если размеры не совпадают с исходными
	if (!(wrappedRows1 == numRows1 && wrappedCols1 == numCols1))
		delete[] matrix1;

	if (!(wrappedCols1 == numCols1 && wrappedCols2 == numCols2))
		delete[] matrix2;

	if (!(wrappedRows1 == numRows1 && wrappedCols2 == numCols2))
	{
		// Перенос данных из выровненной третьей матрицы в исходную
		for (uint64_t row = 0; row < numRows1; ++row)
			for (uint64_t col = 0; col < numCols2; ++col)
				outputMatrix[row * numCols2 + col] = matrix3[row * wrappedCols2 + col];

		// Освобождение памяти для третьей матрицы
		delete[] matrix3;
	}
}

template <typename T>
void multiplyMatrices(
	const T* matrix1,
	const T* matrix2,
	T* resultMatrix,
	uint64_t rows1,
	uint64_t cols1,
	uint64_t cols2
)
{
	// Инициализация результата нулями
	for (uint64_t i = 0; i < rows1 * cols2; ++i)
		resultMatrix[i] = T(0);

	// Умножение матриц
	for (uint64_t i = 0; i < rows1; ++i)
		for (uint64_t k = 0; k < cols1; ++k)
			for (uint64_t j = 0; j < cols2; ++j)
				resultMatrix[i * cols2 + j] += matrix1[i * cols1 + k] * matrix2[k * cols2 + j];
}


template <typename T>
void multiplyMatricesWithChunks(
	const T* matrix1,
	const T* matrix2,
	T* resultMatrix,
	uint64_t rowsMatrix1,
	uint64_t colsMatrix1,
	uint64_t colsMatrix2,
	const bool* chunkStatus1 = nullptr,
	const bool* chunkStatus2 = nullptr,
	bool* chunkStatus3 = nullptr
)
{
	uint64_t chunkRowsMatrix1 = rowsMatrix1 / chunk_size, chunkColsMatrix1 = colsMatrix1 / chunk_size, chunkColsMatrix2 = colsMatrix2 / chunk_size;

	// Инициализация результата
	for (uint64_t i = 0; i < rowsMatrix1 * colsMatrix2; ++i)
		resultMatrix[i] = T(0);

	// Обработка блоками
	for (uint64_t rowStart = 0, chunkRowIndex = 0; rowStart < rowsMatrix1; rowStart += chunk_size, ++chunkRowIndex)
		for (uint64_t colStart = 0, chunkColIndex = 0; colStart < colsMatrix2; colStart += chunk_size, ++chunkColIndex)
		{
			// Ожидание готовности блоков для синхронизации
			if (chunkStatus1)
				while (!chunkStatus1[chunkRowIndex * chunkColsMatrix1 + chunkColIndex])
					this_thread::sleep_for(ChunkProcessingDelay);

			if (chunkStatus2)
				while (!chunkStatus2[chunkColIndex * chunkColsMatrix2 + chunkRowIndex])
					this_thread::sleep_for(ChunkProcessingDelay);

			// Умножение блоков матриц
			for (uint64_t i = rowStart; i < rowStart + chunk_size; ++i)
				for (uint64_t j = colStart; j < colStart + chunk_size; ++j)
					for (uint64_t k = 0; k < colsMatrix1; ++k)
						resultMatrix[i * colsMatrix2 + j] += matrix1[i * colsMatrix1 + k] * matrix2[k * colsMatrix2 + j];
			// Обновление статуса блока результата
			if (chunkStatus3)
				chunkStatus3[chunkRowIndex * chunkColsMatrix2 + chunkColIndex] = true;
		}
}


template <typename T>
void receiveMatrixChunks(
	T* matrix,
	uint64_t rows,
	uint64_t cols,
	MPI_Comm communicator,
	int sourceRank,
	int messageTag,
	bool inverseTraversalOrder,
	bool* chunkStatus = nullptr
)
{
	uint64_t chunkRows = rows / chunk_size, chunkCols = cols / chunk_size;
	T* buffer = new T[chunk_size * chunk_size];  // Буфер для приема данных

	if (inverseTraversalOrder)
	{
		// Получение блоков в обратном порядке
		for (uint64_t colStart = 0, chunkColIndex = 0; colStart < cols; colStart += chunk_size, ++chunkColIndex)
			for (uint64_t rowStart = 0, chunkRowIndex = 0; rowStart < rows; rowStart += chunk_size, ++chunkRowIndex)
			{
				// Получаем данные из MPI
				MPI_Status mpiStatus;
				MPI_Recv((void*)buffer, (int)(chunk_size * chunk_size * sizeof(T)), MPI_BYTE, sourceRank, messageTag, communicator, &mpiStatus);

				// Копируем данные в соответствующий участок матрицы
				for (uint64_t i = rowStart, bufRow = 0; i < rowStart + chunk_size; ++i, ++bufRow)
					for (uint64_t j = colStart, bufCol = 0; j < colStart + chunk_size; ++j, ++bufCol)
						matrix[i * cols + j] = buffer[bufRow * chunk_size + bufCol];

				if (chunkStatus)
					chunkStatus[chunkRowIndex * chunkCols + chunkColIndex] = true;
			}
	}
	else
	{
		// Получение блоков в обычном порядке
		for (uint64_t rowStart = 0, chunkRowIndex = 0; rowStart < rows; rowStart += chunk_size, ++chunkRowIndex)
			for (uint64_t colStart = 0, chunkColIndex = 0; colStart < cols; colStart += chunk_size, ++chunkColIndex)
			{
				// Получаем данные из MPI
				MPI_Status mpiStatus;
				MPI_Recv((void*)buffer, (int)(chunk_size * chunk_size * sizeof(T)), MPI_BYTE, sourceRank, messageTag, communicator, &mpiStatus);

				// Копируем данные в соответствующий участок матрицы
				for (uint64_t i = rowStart, bufRow = 0; i < rowStart + chunk_size; ++i, ++bufRow)
					for (uint64_t j = colStart, bufCol = 0; j < colStart + chunk_size; ++j, ++bufCol)
						matrix[i * cols + j] = buffer[bufRow * chunk_size + bufCol];

				if (chunkStatus)
					chunkStatus[chunkRowIndex * chunkCols + chunkColIndex] = true;
			}
	}

	// Освобождение памяти для буфера
	delete[] buffer;
}


template <typename T>
void sendMatrixChunks(
	const T* matrix,
	uint64_t numRows,
	uint64_t numCols,
	MPI_Comm communicator,
	int destinationRank,
	int messageTag,
	bool inverseTraversalOrder,
	const bool* chunkReadyStatus = nullptr
)
{
	uint64_t chunksInRows = numRows / chunk_size;
	uint64_t chunksInCols = numCols / chunk_size;
	T* buffer = new T[chunk_size * chunk_size];  // Буфер для хранения одного блока данных

	if (inverseTraversalOrder)
	{
		// Отправка блоков в обратном порядке
		for (uint64_t colStart = 0, chunkColIndex = 0; colStart < numCols; colStart += chunk_size, ++chunkColIndex)
			for (uint64_t rowStart = 0, chunkRowIndex = 0; rowStart < numRows; rowStart += chunk_size, ++chunkRowIndex)
			{
				// Ожидание готовности блока
				if (chunkReadyStatus)
					while (!chunkReadyStatus[chunkRowIndex * chunksInCols + chunkColIndex])
						this_thread::sleep_for(ChunkProcessingDelay);

				// Копирование данных в буфер
				for (uint64_t i = rowStart, bufRow = 0; i < rowStart + chunk_size; ++i, ++bufRow)
					for (uint64_t j = colStart, bufCol = 0; j < colStart + chunk_size; ++j, ++bufCol)
						buffer[bufRow * chunk_size + bufCol] = matrix[i * numCols + j];

				// Отправка данных через MPI
				MPI_Send((void*)buffer, (int)(chunk_size * chunk_size * sizeof(T)), MPI_BYTE, destinationRank, messageTag, communicator);
			}
	}
	else
	{
		// Отправка блоков в обычном порядке
		for (uint64_t rowStart = 0, chunkRowIndex = 0; rowStart < numRows; rowStart += chunk_size, ++chunkRowIndex)
			for (uint64_t colStart = 0, chunkColIndex = 0; colStart < numCols; colStart += chunk_size, ++chunkColIndex)
			{
				// Ожидание готовности блока
				if (chunkReadyStatus)
					while (!chunkReadyStatus[chunkRowIndex * chunksInCols + chunkColIndex])
						this_thread::sleep_for(ChunkProcessingDelay);

				// Копирование данных в буфер
				for (uint64_t i = rowStart, bufRow = 0; i < rowStart + chunk_size; ++i, ++bufRow)
					for (uint64_t j = colStart, bufCol = 0; j < colStart + chunk_size; ++j, ++bufCol)
						buffer[bufRow * chunk_size + bufCol] = matrix[i * numCols + j];

				// Отправка данных через MPI
				MPI_Send((void*)buffer, (int)(chunk_size * chunk_size * sizeof(T)), MPI_BYTE, destinationRank, messageTag, communicator);
			}
	}

	// Освобождение памяти для буфера
	delete[] buffer;
}


template <typename T>
void matrixMasterWorker(
	const T* inputMatrix1,
	const T* inputMatrix2,
	T* outputMatrix,
	uint64_t numRows1,   // mx
	uint64_t numCols1,   // my
	uint64_t numCols2,   // mz
	uint64_t offsetRows1, // dx
	uint64_t offsetCols2, // dz
	ProcessingMode executionMode,  // изменено на ProcessingMode
	int baseTag,
	int slaveRank
)
{
	MPI_Status mpiStatus;

	// Отправка метаданных: baseTag и режим работы
	MPI_Send((void*)&baseTag, (int)sizeof(int), MPI_BYTE, slaveRank, 0, MPI_COMM_WORLD);
	MPI_Send((void*)&executionMode, (int)sizeof(ProcessingMode), MPI_BYTE, slaveRank, baseTag + 0, MPI_COMM_WORLD);

	switch (executionMode)
	{
	case ProcessingMode::Synchronous:  // Синхронный режим
	{
		uint64_t xDim = numRows1 / 2, yDim = numCols1, zDim = numCols2 / 2;

		// Отправка размеров блоков
		MPI_Send((void*)&xDim, (int)sizeof(uint64_t), MPI_BYTE, slaveRank, baseTag + 1, MPI_COMM_WORLD);
		MPI_Send((void*)&yDim, (int)sizeof(uint64_t), MPI_BYTE, slaveRank, baseTag + 2, MPI_COMM_WORLD);
		MPI_Send((void*)&zDim, (int)sizeof(uint64_t), MPI_BYTE, slaveRank, baseTag + 3, MPI_COMM_WORLD);

		// Создание временных матриц для вычислений
		T* tempMatrix1 = new T[xDim * yDim];
		T* tempMatrix2 = new T[yDim * zDim];
		T* tempMatrix3 = new T[xDim * zDim];

		// Копирование данных из исходных матриц в временные
		for (uint64_t i = 0; i < xDim; ++i)
			for (uint64_t j = 0; j < yDim; ++j)
				tempMatrix1[i * yDim + j] = inputMatrix1[(offsetRows1 + i) * numCols1 + j];

		for (uint64_t i = 0; i < yDim; ++i)
			for (uint64_t j = 0; j < zDim; ++j)
				tempMatrix2[i * zDim + j] = inputMatrix2[i * numCols2 + (offsetCols2 + j)];

		// Отправка блоков данных на вычисление
		MPI_Send((void*)tempMatrix1, (int)(xDim * yDim * sizeof(T)), MPI_BYTE, slaveRank, baseTag + 4, MPI_COMM_WORLD);
		MPI_Send((void*)tempMatrix2, (int)(yDim * zDim * sizeof(T)), MPI_BYTE, slaveRank, baseTag + 5, MPI_COMM_WORLD);
		MPI_Recv((void*)tempMatrix3, (int)(xDim * zDim * sizeof(T)), MPI_BYTE, slaveRank, baseTag + 6, MPI_COMM_WORLD, &mpiStatus);

		// Копирование результата в итоговую матрицу
		for (uint64_t i = 0; i < xDim; ++i)
			for (uint64_t j = 0; j < zDim; ++j)
				outputMatrix[(offsetRows1 + i) * numCols2 + (offsetCols2 + j)] = tempMatrix3[i * zDim + j];

		// Освобождение временных матриц
		delete[] tempMatrix1;
		delete[] tempMatrix2;
		delete[] tempMatrix3;

		break;
	}

	case ProcessingMode::Asynchronous:  // Асинхронный режим
	{
		uint64_t xDim = numRows1 / 2, yDim = numCols1, zDim = numCols2 / 2;

		// Отправка размеров блоков
		MPI_Send((void*)&xDim, (int)sizeof(uint64_t), MPI_BYTE, slaveRank, baseTag + 1, MPI_COMM_WORLD);
		MPI_Send((void*)&yDim, (int)sizeof(uint64_t), MPI_BYTE, slaveRank, baseTag + 2, MPI_COMM_WORLD);
		MPI_Send((void*)&zDim, (int)sizeof(uint64_t), MPI_BYTE, slaveRank, baseTag + 3, MPI_COMM_WORLD);

		// Создание временных матриц для вычислений
		T* tempMatrix1 = new T[xDim * yDim];
		T* tempMatrix2 = new T[yDim * zDim];
		T* tempMatrix3 = new T[xDim * zDim];

		// Копирование данных из исходных матриц в временные
		for (uint64_t i = 0; i < xDim; ++i)
			for (uint64_t j = 0; j < yDim; ++j)
				tempMatrix1[i * yDim + j] = inputMatrix1[(offsetRows1 + i) * numCols1 + j];

		for (uint64_t i = 0; i < yDim; ++i)
			for (uint64_t j = 0; j < zDim; ++j)
				tempMatrix2[i * zDim + j] = inputMatrix2[i * numCols2 + (offsetCols2 + j)];

		// Отправка данных асинхронно с использованием потоков
		thread sender1([=]() { sendMatrixChunks(tempMatrix1, xDim, yDim, MPI_COMM_WORLD, slaveRank, baseTag + 4, false); });
		thread sender2([=]() { sendMatrixChunks(tempMatrix2, yDim, zDim, MPI_COMM_WORLD, slaveRank, baseTag + 5, true); });

		sender1.join();
		sender2.join();

		// Получение результата
		receiveMatrixChunks(tempMatrix3, xDim, zDim, MPI_COMM_WORLD, slaveRank, baseTag + 6, false);

		// Копирование результата в итоговую матрицу
		for (uint64_t i = 0; i < xDim; ++i)
			for (uint64_t j = 0; j < zDim; ++j)
				outputMatrix[(offsetRows1 + i) * numCols2 + (offsetCols2 + j)] = tempMatrix3[i * zDim + j];

		// Освобождение временных матриц
		delete[] tempMatrix1;
		delete[] tempMatrix2;
		delete[] tempMatrix3;

		break;
	}

	default:
		cout << "Invalid execution mode\n";
	}
}

template <typename T>
void matrixMaster(
	const T* inputMatrix1,   // Входная матрица 1
	const T* inputMatrix2,   // Входная матрица 2
	T* outputMatrix,         // Выходная матрица
	uint64_t numRows,        // Число строк
	uint64_t numCols1,       // Число столбцов первой матрицы
	uint64_t numCols2,       // Число столбцов второй матрицы
	ProcessingMode mode,     // Режим обработки
	int numSlaves            // Число рабочих процессов (слейвов)
)
{
	int baseTag = 1;
	uint64_t rows, cols1, cols2;

	T* tempMatrix1;
	T* tempMatrix2;
	T* tempMatrix3;

	// Обертка для матриц: извлечение подматриц для обработки
	wrapMatrixData(inputMatrix1, inputMatrix2, outputMatrix, numRows, numCols1, numCols2,
		tempMatrix1, tempMatrix2, tempMatrix3, rows, cols1, cols2);

	// Обработка первой четверти матрицы
	thread worker1([=]()
		{
			matrixMasterWorker(tempMatrix1, tempMatrix2, tempMatrix3, rows, cols1, cols2, 0, 0, mode, baseTag + 8 * 0, (0 % (numSlaves - 1)) + 1);
		});

	// Обработка второй четверти матрицы
	thread worker2([=]()
		{
			matrixMasterWorker(tempMatrix1, tempMatrix2, tempMatrix3, rows, cols1, cols2, 0, cols2 / 2, mode, baseTag + 8 * 1, (1 % (numSlaves - 1)) + 1);
		});

	// Обработка третьей четверти матрицы
	thread worker3([=]()
		{
			matrixMasterWorker(tempMatrix1, tempMatrix2, tempMatrix3, rows, cols1, cols2, rows / 2, 0, mode, baseTag + 8 * 2, (2 % (numSlaves - 1)) + 1);
		});

	// Обработка четвертой четверти матрицы
	thread worker4([=]()
		{
			matrixMasterWorker(tempMatrix1, tempMatrix2, tempMatrix3, rows, cols1, cols2, rows / 2, cols2 / 2, mode, baseTag + 8 * 3, (3 % (numSlaves - 1)) + 1);
		});

	// Ожидание завершения всех рабочих потоков
	worker1.join();
	worker2.join();
	worker3.join();
	worker4.join();

	// Завершающая обертка для матриц: извлечение результатов обработки
	wrapMatrixDataEnd(inputMatrix1, inputMatrix2, outputMatrix, numRows, numCols1, numCols2,
		tempMatrix1, tempMatrix2, tempMatrix3, rows, cols1, cols2);
}


template <typename T>
void matrixSlaveAsyncWorker(
	T* matrix1,              // Матрица 1 (полученная из сети)
	T* matrix2,              // Матрица 2 (полученная из сети)
	const T* matrix3,        // Матрица 3 (для отправки в сеть)
	uint64_t rows,           // Количество строк для обработки
	uint64_t cols1,          // Количество столбцов для первой матрицы
	uint64_t cols2,          // Количество столбцов для второй матрицы
	bool* checkMatrix1,      // Массив для проверки матрицы 1
	bool* checkMatrix2,      // Массив для проверки матрицы 2
	const bool* checkMatrix3,// Массив для проверки матрицы 3
	int baseTag              // Базовый тег для MPI
)
{
	// Поток для получения данных для matrix1
	thread receiver1Thread([=]()
		{
			receiveMatrixChunks(matrix1, rows, cols1, MPI_COMM_WORLD, 0, baseTag + 4, false, checkMatrix1);
		});

	// Поток для получения данных для matrix2
	thread receiver2Thread([=]()
		{
			receiveMatrixChunks(matrix2, cols1, cols2, MPI_COMM_WORLD, 0, baseTag + 5, true, checkMatrix2);
		});

	// Ожидание завершения получения обеих частей данных
	receiver1Thread.join();
	receiver2Thread.join();

	// Отправка обработанных данных (matrix3)
	sendMatrixChunks(matrix3, rows, cols2, MPI_COMM_WORLD, 0, baseTag + 6, false, checkMatrix3);
}


template <typename T>
void matrixSlaveWorker()
{
	ProcessingMode processingMode;
	int baseTag;
	MPI_Status mpiStatus;

	// Получение базового тега
	MPI_Recv((void*)&baseTag, (int)sizeof(int), MPI_BYTE, 0, 0, MPI_COMM_WORLD, &mpiStatus);

	// Проверка на ошибку
	if (baseTag == -1)
		throw exception();

	// Получение режима обработки
	MPI_Recv((void*)&processingMode, (int)sizeof(ProcessingMode), MPI_BYTE, 0, baseTag + 0, MPI_COMM_WORLD, &mpiStatus);

	switch (processingMode)
	{
	case ProcessingMode::Synchronous:
	{
		uint64_t rows, cols1, cols2;

		// Получение размеров матриц
		MPI_Recv((void*)&rows, (int)sizeof(uint64_t), MPI_BYTE, 0, baseTag + 1, MPI_COMM_WORLD, &mpiStatus);
		MPI_Recv((void*)&cols1, (int)sizeof(uint64_t), MPI_BYTE, 0, baseTag + 2, MPI_COMM_WORLD, &mpiStatus);
		MPI_Recv((void*)&cols2, (int)sizeof(uint64_t), MPI_BYTE, 0, baseTag + 3, MPI_COMM_WORLD, &mpiStatus);

		T* matrix1 = new T[rows * cols1];
		T* matrix2 = new T[cols1 * cols2];
		T* resultMatrix = new T[rows * cols2];

		// Получение данных для матриц
		MPI_Recv((void*)matrix1, (int)(rows * cols1 * sizeof(T)), MPI_BYTE, 0, baseTag + 4, MPI_COMM_WORLD, &mpiStatus);
		MPI_Recv((void*)matrix2, (int)(cols1 * cols2 * sizeof(T)), MPI_BYTE, 0, baseTag + 5, MPI_COMM_WORLD, &mpiStatus);

		// Умножение матриц
		multiplyMatricesWithChunks(matrix1, matrix2, resultMatrix, rows, cols1, cols2);

		// Отправка результата
		MPI_Send((void*)resultMatrix, (int)(rows * cols2 * sizeof(T)), MPI_BYTE, 0, baseTag + 6, MPI_COMM_WORLD);

		delete[] matrix1;
		delete[] matrix2;
		delete[] resultMatrix;

		break;
	}

	case ProcessingMode::Asynchronous:
	{
		uint64_t rows, cols1, cols2;

		// Получение размеров матриц
		MPI_Recv((void*)&rows, (int)sizeof(uint64_t), MPI_BYTE, 0, baseTag + 1, MPI_COMM_WORLD, &mpiStatus);
		MPI_Recv((void*)&cols1, (int)sizeof(uint64_t), MPI_BYTE, 0, baseTag + 2, MPI_COMM_WORLD, &mpiStatus);
		MPI_Recv((void*)&cols2, (int)sizeof(uint64_t), MPI_BYTE, 0, baseTag + 3, MPI_COMM_WORLD, &mpiStatus);

		uint64_t chunkRows = rows / chunk_size, chunkCols1 = cols1 / chunk_size, chunkCols2 = cols2 / chunk_size;

		T* matrix1 = new T[rows * cols1];
		T* matrix2 = new T[cols1 * cols2];
		T* resultMatrix = new T[rows * cols2];

		bool* checkMatrix1 = new bool[chunkRows * chunkCols1]();
		bool* checkMatrix2 = new bool[chunkCols1 * chunkCols2]();
		bool* checkMatrix3 = new bool[chunkRows * chunkCols2]();

		// Асинхронная обработка данных в отдельном потоке
		thread slaveAsyncWorkerThread([=]()
			{
				matrixSlaveAsyncWorker(matrix1, matrix2, resultMatrix, rows, cols1, cols2, checkMatrix1, checkMatrix2, checkMatrix3, baseTag);
			});

		// Умножение матриц
		multiplyMatricesWithChunks(matrix1, matrix2, resultMatrix, rows, cols1, cols2, checkMatrix1, checkMatrix2, checkMatrix3);

		// Ожидание завершения асинхронной работы
		slaveAsyncWorkerThread.join();

		delete[] matrix1;
		delete[] matrix2;
		delete[] resultMatrix;

		delete[] checkMatrix1;
		delete[] checkMatrix2;
		delete[] checkMatrix3;

		break;
	}

	default:
		cout << "Unexpected error occurred\n";
	}
}


template <typename T>
void matrixSlave()
{
	while (true)
	{
		try
		{
			matrixSlaveWorker<T>();
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
		exit(-1);

	int process_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &process_rank);

	int total_processes;
	MPI_Comm_size(MPI_COMM_WORLD, &total_processes);

	if (process_rank == 0)
	{
		ProcessingMode processing_mode;
		uint64_t rows_A, cols_A, cols_B;
		string output_file_path;

		bool use_fill_values = false;
		double filler_value_A;
		double filler_value_B;

		try
		{
			if (argc < 5)
				throw exception();

			rows_A = stoull(argv[1]);
			cols_A = stoull(argv[2]);
			cols_B = stoull(argv[3]);

			if (argc >= 7)
			{
				try
				{
					filler_value_A = stod(argv[4]);
					filler_value_B = stod(argv[5]);

					use_fill_values = true;
				}
				catch (...) {}
			}

			string mode_str(argv[use_fill_values ? 6 : 4]);

			if (mode_str == "sync")
				processing_mode = ProcessingMode::Synchronous;
			else if (mode_str == "async")
				processing_mode = ProcessingMode::Asynchronous;
			else
				throw exception();

			if (argc >= (use_fill_values ? 8 : 6))
				output_file_path = argv[use_fill_values ? 7 : 5];
		}
		catch (...)
		{
			cout << "Usage: <uint64 rows_A> <uint64 cols_A> <uint64 cols_B> [<flt64 filler_A> <flt64 filler_B>] <mode> [<output_file_path>]\n"
				<< "\nAvailable modes: sync, async. If fillers are specified then the entire matrices will have the same elements,"
				<< "\notherwise UNIFORM_DISTRIBUTION[-1.0; +1.0]. All file paths, including the executable path, must be the same"
				<< "\non all machines and must be absolute. All files must have OPENMPIACC permissions for the requested actions (R/W/X)."
				<< "\nIf file path is specified, the master will (over)write the result matrix and the check matrix to it.";

			exit(-1);
		}

		MatrixData* matrix_A = new MatrixData[rows_A * cols_A];
		MatrixData* matrix_B = new MatrixData[cols_A * cols_B];
		MatrixData* result_matrix = new MatrixData[rows_A * cols_B];
		MatrixData* check_matrix = new MatrixData[rows_A * cols_B];

		if (use_fill_values)
		{
			for (uint64_t i = 0; i < rows_A * cols_A; ++i)
				matrix_A[i] = (MatrixData)filler_value_A;

			for (uint64_t i = 0; i < cols_A * cols_B; ++i)
				matrix_B[i] = (MatrixData)filler_value_B;
		}
		else
		{
			mt19937 generator((random_device())());
			uniform_real_distribution<MatrixData> distribution(-1.0, 1.0);

			for (uint64_t i = 0; i < rows_A * cols_A; ++i)
				matrix_A[i] = (MatrixData)distribution(generator);

			for (uint64_t i = 0; i < cols_A * cols_B; ++i)
				matrix_B[i] = (MatrixData)distribution(generator);
		}

		bool check_completed = false;
		thread check_thread([matrix_A, matrix_B, check_matrix, rows_A, cols_A, cols_B, &check_completed]()
			{
				auto start = chrono::high_resolution_clock::now();
				multiplyMatrices(matrix_A, matrix_B, check_matrix, rows_A, cols_A, cols_B);
				auto end = chrono::high_resolution_clock::now();

				cout << "MASTER TIME: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " milliseconds\n";
				check_completed = true;
			});

		auto start = chrono::high_resolution_clock::now();
		matrixMaster(matrix_A, matrix_B, result_matrix, rows_A, cols_A, cols_B, processing_mode, total_processes);
		auto end = chrono::high_resolution_clock::now();

		cout << "SLAVES TIME: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " milliseconds\n";

		if (!check_completed)
			cout << "Waiting for the master to finish checking...\n";

		check_thread.join();

		uint64_t mismatch_count = 0;

		for (uint64_t i = 0; i < rows_A * cols_B; ++i)
			if (!almost_equal(result_matrix[i], check_matrix[i]))
				++mismatch_count;

		cout << "End of check. Mismatch count: " << mismatch_count << "\n";

		if (output_file_path != "")
		{
			ofstream output(output_file_path);

			if (output.is_open())
			{
				output
					<< setw(10) << "ROW" << "\t"
					<< setw(10) << "COLUMN" << "\t"
					<< setw(10) << "SLAVES" << "\t"
					<< setw(10) << "MASTER" << "\n";

				for (uint64_t i = 0; i < rows_A * cols_B; ++i)
					output
					<< setw(10) << i / cols_B << "\t"
					<< setw(10) << i % cols_B << "\t"
					<< setw(10) << result_matrix[i] << "\t"
					<< setw(10) << check_matrix[i] << "\n";

				output.close();
			}
			else
				cout << "Wrong file path\n";
		}

		for (int i = 1, false_tag = -1; i < total_processes; ++i)
			MPI_Send((void*)&false_tag, (int)sizeof(int), MPI_BYTE, i, 0, MPI_COMM_WORLD);

		delete[] matrix_A;
		delete[] matrix_B;
		delete[] result_matrix;
		delete[] check_matrix;
	}
	else
		matrixSlave<MatrixData>();

	MPI_Finalize();
}
