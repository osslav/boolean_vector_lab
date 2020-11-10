#include <iostream>
using namespace std;

int const DEFAULT_COUNT = 8;

enum ErrorCode
{
	errorCountLessZero = 1,
	errorStringEmpty,
	errorSymbolString,

	errorIndexOutVector,
	errorRangeIndexOutVector,
};

// Вектор 1110110001 хранится в памяти как:  00000011 10110001 

class BoolVector
{
	unsigned char* vector_;    
	int count_;        //длина вектора
	int memory_;       //количество выделенных байт

public:
	BoolVector(int count = DEFAULT_COUNT);
	BoolVector(const char* stringVector);
	BoolVector(const BoolVector& copy);
	~BoolVector() { delete vector_; };

	void outputFull();

	friend ostream& operator << (ostream& f, BoolVector& vector);
	friend istream& operator >> (istream& f, BoolVector& vector);

	void invertVector();
	void invertInd(int index);

	void setZero();
	void setOne();
	void setZeroInd(int index, int count = 1);
	void setOneInd(int index, int count = 1);

	int searchWeight();

	BoolVector operator &(BoolVector term);
	void operator &=(BoolVector term);
	
	BoolVector operator |(BoolVector term);
	void operator |=(BoolVector term);
	
	BoolVector operator ^(BoolVector term);
	void operator ^=(BoolVector term);
	
	void operator =(const BoolVector& copy);

	bool operator [](int index);

	BoolVector operator <<(int count);

	BoolVector operator >>(int count);

	void operator <<=(int count);

	void operator >>=(int count);
};

BoolVector::BoolVector(int count)
{
	if (count < 0) throw errorCountLessZero;
	count_ = count;
	memory_ = (count + 7) / 8;

	vector_ = new unsigned char[memory_];

	for (int i = 0; i < memory_; i++) vector_[i] = 0;
}

BoolVector::BoolVector(const char* stringVector)
{
	count_ = 0;
	while (stringVector[count_] != '\0')
	{		
		if ((stringVector[count_] != '0') && (stringVector[count_] != '1')) throw errorSymbolString;
		count_++;
	}

	memory_ = (count_ + 7)/ 8;
	vector_ = new unsigned char[memory_];

	int indexVector = 0;
	vector_[0] = 0;
	for (int i = memory_ * 8 - count_; i < memory_ * 8; i++)
	{ 
		if (i % 8 == 0)
		{
			indexVector++;
			vector_[indexVector] = 0;
		}
		vector_[indexVector] <<= 1;
		vector_[indexVector] += stringVector[i - memory_ * 8 + count_] - '0';
	}
}

BoolVector::BoolVector(const BoolVector& copy)
{
	count_ = copy.count_;
	memory_ = copy.memory_;

	vector_ = new unsigned char[memory_];
	for (int i = 0; i < memory_; i++) vector_[i] = copy.vector_[i];
}

void BoolVector::outputFull()
{
	int index = 0;
	unsigned char mask = 1 << 7;
	while (index < memory_ * 8)
	{
		cout << '|';
		if ((mask & vector_[index / 8]) != 0) cout << '1';
		else cout << '0';	

		index++;
		mask = mask >> 1;
		if (mask == 0)
		{
			mask = 1 << 7;
			cout << "|  ";
		}
	}
	cout << '\n';
}

void BoolVector::invertVector()
{
	for (int i = 0; i < memory_; i++) vector_[i] = ~vector_[i];
	unsigned char mask = ~0;
	mask = mask >> (memory_ * 8 - count_);
	vector_[0] = vector_[0] & mask;
}

void BoolVector::invertInd(int index)
{
	if ((index >= count_) || (index < 0)) throw errorIndexOutVector;
	
	int byte = memory_ - 1 - index / 8;
	unsigned char mask = 1 << (index % 8);

	if ((vector_[byte] & mask) != 0) vector_[byte] = vector_[byte] & (~mask);
	else vector_[byte] = vector_[byte] | mask;

}

void BoolVector::setZero()
{
	for (int i = 0; i < memory_; i++) vector_[i] = 0;
}


void BoolVector::setOne()
{
	for (int i = 0; i < memory_; i++) vector_[i] = ~0;
	unsigned char mask = ~0;
	mask = mask >> (memory_ * 8 - count_);
	vector_[0] = vector_[0] & mask;
}

void BoolVector::setZeroInd(int index, int count)
{
	if (count < 0)
	{
		index += count + 1;
		count *= -1;
	}
	if ((index < 0) || (index + count >= count_)) throw errorRangeIndexOutVector;


	int byte = memory_ - 1 - index / 8;
	unsigned char mask = 1 << (index % 8);

	for (int i = 0; i < count; i++)
	{
		vector_[byte] = vector_[byte] & (~mask);

		mask = mask << 1;
		if (mask == 0)
		{
			mask = 1;
			byte--;
		}
	}
}


void BoolVector::setOneInd(int index, int count)
{
	if (count < 0)
	{
		index += count + 1;
		count *= -1;
	}
	if ((index < 0) || (index + count >= count_)) throw errorRangeIndexOutVector;


	int byte = memory_ - 1 - index / 8;
	unsigned char mask = 1 << (index % 8);

	for (int i = 0; i < count; i++)
	{
		vector_[byte] = vector_[byte] | mask;

		mask = mask << 1;
		if (mask == 0)
		{
			mask = 1;
			byte--;
		}
	}
}


int BoolVector::searchWeight()
{
	int weight = 0;
	int index = 8 * memory_ - count_;
	unsigned char mask = 1 << (7 - index);
	while (index < memory_ * 8)
	{
		if ((mask & vector_[index / 8]) != 0) weight++;

		index++;
		mask = mask >> 1;
		if (mask == 0) mask = 1 << 7;
	}

	return weight;
}

BoolVector BoolVector::operator &(BoolVector term)
{
	if (memory_ > term.memory_)
	{
		BoolVector result(*this);
		for (int i = 0; i < term.memory_; i++) result.vector_[i + memory_ - term.memory_] &= term.vector_[i];
		for (int i = memory_ - term.memory_ - 1; i >= 0; i--) result.vector_[i] = 0;
		return result;
	}
	else
	{
		BoolVector result(term);
		for (int i = 0; i < memory_; i++) result.vector_[i + term.memory_ - memory_] &= vector_[i];
		for (int i = term.memory_ - memory_ - 1; i >= 0; i--) result.vector_[i] = 0;
		return result;
	}
}

void BoolVector::operator &=(BoolVector term)
{
	if (memory_ > term.memory_)
	{
		for (int i = 0; i < memory_ - term.memory_; i++) vector_[i] = 0;
		for (int i = 0; i < term.memory_; i++) vector_[i + memory_ - term.memory_] &= term.vector_[i];
	}
	else
	{
		unsigned char* result;
		result = new unsigned char[term.memory_];
		
		for (int i = 0; i < memory_ - term.memory_; i++) result[i] = 0;
		for (int i = term.memory_ - memory_; i < term.memory_; i++) result[i] = term.vector_[i];
		for (int i = 0; i < memory_; i++) result[i + term.memory_ - memory_] &= vector_[i];

		delete vector_;
		vector_ = result;
		memory_ = term.memory_;
		count_ = term.count_;
	}
}

BoolVector BoolVector::operator |(BoolVector term)
{
	if (memory_ > term.memory_)
	{
		BoolVector result(*this);
		for (int i = 0; i < term.memory_; i++) result.vector_[i + memory_ - term.memory_] |= term.vector_[i];
		return result;
	}
	else
	{
		BoolVector result(term);
		for (int i = 0; i < memory_; i++) result.vector_[i + term.memory_ - memory_] |= vector_[i];
		return result;
	}
}


void BoolVector::operator |=(BoolVector term)
{
	if (memory_ > term.memory_)  for (int i = 0; i < term.memory_; i++) vector_[i + memory_ - term.memory_] |= term.vector_[i];
	else
	{
		unsigned char* result;
		result = new unsigned char[term.memory_];
		for (int i = 0; i < term.memory_; i++) result[i] = term.vector_[i];
		for (int i = 0; i < memory_; i++) result[i + term.memory_ - memory_] |= vector_[i];

		delete vector_;
		vector_ = result;
		memory_ = term.memory_;
		count_ = term.count_;
	}
}


BoolVector BoolVector::operator ^(BoolVector term)
{
	if (memory_ > term.memory_)
	{
		BoolVector result(*this);
		for (int i = 0; i < term.memory_; i++) result.vector_[i + memory_ - term.memory_] ^= term.vector_[i];
		return result;
	}
	else
	{
		BoolVector result(term);
		for (int i = 0; i < memory_; i++) result.vector_[i + term.memory_ - memory_] ^= vector_[i];
		return result;
	}

}


void BoolVector::operator ^=(BoolVector term)
{
	if (memory_ > term.memory_)  for (int i = 0; i < term.memory_; i++) vector_[i + memory_ - term.memory_] ^= term.vector_[i];
	else
	{
		unsigned char* result;
		result = new unsigned char[term.memory_];
		for (int i = 0; i < term.memory_; i++) result[i] = term.vector_[i];
		for (int i = 0; i < memory_; i++) result[i + term.memory_ - memory_] ^= vector_[i];

		delete vector_;
		vector_ = result;
		memory_ = term.memory_;
		count_ = term.count_;
	}
}



bool BoolVector::operator [](int index)
{
	if ((index < 0) || (index >= count_)) throw errorRangeIndexOutVector;

	int byte = memory_ - index / 8 - 1;
	unsigned char mask = 1 << (index % 8);

	if ((mask & vector_[byte]) != 0) return true;
	else return false;
}

void BoolVector::operator =(const BoolVector& copy)
{
	if (this != &copy)
	{
		count_ = copy.count_;
		memory_ = copy.memory_;
		delete vector_;
		vector_ = new unsigned char[memory_];
		for (int i = 0; i < memory_; i++) vector_[i] = copy.vector_[i];
	}	
}

ostream& operator << (ostream& f, BoolVector& vector)
{
	int index = 8 * vector.memory_ - vector.count_;
	unsigned char mask = 1 << (7 - index);
	while (index < vector.memory_ * 8)
	{
		if ((mask & vector.vector_[index / 8]) != 0) f << '1';
		else f << '0';

		index++;
		mask = mask >> 1;
		if (mask == 0) mask = 1 << 7;
	}
	return f;
}


istream& operator >> (istream& f, BoolVector& vector)
{
	cout << "Enter the number of bits of the boolean vector: ";
	f >> vector.count_;

	while (vector.count_ < 0)
	{
		cout << "Number of bits can't be < 0. Enter again:  ";
		f >> vector.count_;
	}

	if (vector.memory_ != (vector.count_ + 7) / 8)
	{
		vector.memory_ = (vector.count_ + 7) / 8;
		delete vector.vector_;
		vector.vector_ = new unsigned char[vector.memory_];
	}
	for (int i = 0; i < vector.memory_; i++) vector.vector_[i] = 0;

	char nextSymbol;
	f >> nextSymbol;
	for (int i = vector.memory_*8 - vector.count_; i < vector.memory_ * 8; i++)
	{
		if ((nextSymbol != '0') && (nextSymbol != '1'))
		{
			cout << "Error. Enter " << i << " bit again: ";
			f >> nextSymbol;
		}

		vector.vector_[i / 8] <<= 1;
		if (nextSymbol == '1') vector.vector_[i / 8]++;

		if (i != vector.memory_ * 8 - 1) f >> nextSymbol;
	}
	return f;
}

BoolVector BoolVector::operator >>(int count)
{
	BoolVector result(count_);

	int step = count / 8;
	count %= 8;

	unsigned char mask = ~0;
	mask >>= (8 - count);
	unsigned char saveBits = 0;

	for (int i = result.memory_ - 1; i >= step; i--)
	{
		result.vector_[i] = vector_[i - step] >> count;
		if (i != result.memory_ - 1)
		{
			saveBits = (vector_[i + step] & mask) << (8 - count);
			result.vector_[i + 1] |= saveBits;
		}
	}

	return result;
}

BoolVector BoolVector::operator <<(int count)
{
	BoolVector result(count_);

	int step = count / 8;
	count %= 8;

	unsigned char mask = ~0;
	mask <<= (8 - count);
	unsigned char saveBits = 0;

	for (int i = 0; i < (result.memory_ - step); i++)
	{
		result.vector_[i] = vector_[i + step] << count;
		if (i != 0)
		{
			saveBits = (vector_[i - step] & mask) >> (8 - count);
			result.vector_[i - 1] |= saveBits;
		}
		
	}

	mask = ~0;
	mask >>= (memory_ * 8 - count_);
	result.vector_[0] &= mask;

	return result;
}

void BoolVector::operator <<=(int count)
{
	BoolVector oldVector(*this);

	int step = count / 8;
	count %= 8;

	unsigned char mask = ~0;
	mask <<= (8 - count);
	unsigned char saveBits = 0;

	for (int i = 0; i < (memory_ - step); i++)
	{
		vector_[i] = oldVector.vector_[i + step] << count;
		if (i != 0)
		{
			saveBits = (oldVector.vector_[i - step] & mask) >> (8 - count);
			vector_[i - 1] |= saveBits;
		}

	}

	mask = ~0;
	mask >>= (memory_ * 8 - count_);
	vector_[0] &= mask;

	for (int i = memory_ - step; i < memory_; i++) vector_[i] = 0;
}

void BoolVector::operator >>=(int count)
{
	BoolVector oldVector(*this);

	int step = count / 8;
	count %= 8;

	unsigned char mask = ~0;
	mask >>= (8 - count);
	unsigned char saveBits = 0;

	for (int i = memory_ - 1; i >= step; i--)
	{
		vector_[i] = oldVector.vector_[i - step] >> count;
		if (i != memory_ - 1)
		{
			saveBits = (oldVector.vector_[i + step] & mask) << (8 - count);
			vector_[i + 1] |= saveBits;
		}
	}

	for (int i = step - 1; i >= 0; i--) vector_[i] = 0;
}


int main()
{
	try
	{
		BoolVector A("1010101010"), B("11000101100");
		A = A << 1;
		A.outputFull();
	
		//A <<= 11;
		
		//cout << B;
		//B.outputFull() ;
		//A.output();
		//C.output();
		

		
	}
	catch (ErrorCode a)
	{
		return a;
	}

	return 0;
}


