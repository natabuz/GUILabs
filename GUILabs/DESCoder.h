// класс шифрования DES.
// основная работа с ним:
//	- создание экземпляра класса с заданным ключом
//	- вызов метода encode для шифрования 8-байтного 
//		сообщения. На выходе этого метода - 8 зашифрованных байт.
//	- вызов метода decode - противоположность encode
// Соответственно, вызывая метод encode для всех 8-байтовых участков входного сообщения,
// можно закодировать всё сообщение
// Аналогично можно дешифровать сообщение

#define WRONG_KEY_SIZE 100	// код исключения при неверном размере ключа
#define WRONG_DATA_SIZE 101	// код исключения при неверном размере данных для шифрования

#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

class DESCoder
{
public:
	// кодирование DES определяется ключом
	// поэтому создан только один конструктор на основе 8-байтного ключа
	// здесь не будем пользоваться сокращением vecUnsChar,
	// чтобы вне этого класса не пришлось угадывать, что это значит
	DESCoder(vector<unsigned char> key);
	
	// публичный метод - шифрование 8 байт, 
	// на выходе - зашифрованные 8 байт
	vector<unsigned char> encode(vector<unsigned char> data);
	
	// для дешифрования всё в точности наоборот
	vector<unsigned char> decode(vector<unsigned char> data);


	// весь алгоритм (функции и переменные) скрыт в области private
private:
	// так как шифрование и дешифрование DES отличаются только порядком передачи
	// ключей в 16 шагах алгоритма, обобщим эти 2 функции в одну
	vector<unsigned char> _process(vector<unsigned char> data, bool encode);

	vector<unsigned char> _dataBits;			// данные, которые необходимо закодировать (массив бит)
	vector<vector<unsigned char> > _keys;		// ключи, которые необходимо использовать для шифрования
	
	vector<unsigned char> _left;		// левая половина данных
	vector<unsigned char> _right;		// правая половина данных

	// начальная и конечная перестановка бит
	void _IP();
	void _inverseIP();

	// генерация ключей для каждого шага шифрования
	// на основе ключа, получаемого при создании шифратора
	void _generateKeys(vector<unsigned char> key64);
	vector<unsigned char> _shiftKeyLeft(vector<unsigned char> key56, int numShifts); // вспомогательная функция для генерации ключей

	// один шаг алгоритма DES
	// всего выполняется 16 шагов
	void _makeDesStep(vector<unsigned char> iKey);

	// шаг алгоритма DES состоит из следущих шагов:
	vector<unsigned char> _expansion (); // расширяет _right до 48 бит

	// после этого расширенный right48 ксорится с очередным ключом
	// затем 48-битный блок проходит S-блоки - substitution
	// на выходе которых - 32-битный блок
	vector<unsigned char> _substitution(vector<unsigned char> right48);

	// полученный после S-блоков 32-битный блок подвергается
	// очередной перестановке - P-блоку
	vector<unsigned char> _permutation (vector<unsigned char> right32);

	// после этого полученный блок ксорится с левой частью
	// и они меняются местами
	// на этом один из 16 шагов заканчивается

	
	// далее идут вспомогательные методы

	// вспомогательная функция представления байта как массив 0 и 1
	vector<unsigned char> _byteToBits (unsigned char byte); 
	
	// вспомогательная функция для представления массива байт как массив 0 и 1
	vector<unsigned char> _bytesArrayToBits(vector<unsigned char> bytes);

	// вспомогательная функция представления массива бит как единого байта
	unsigned char _bitsToByte (vector<unsigned char> bits); 
	
	// вспомогательная функция для представления массива бит как массив байт
	vector<unsigned char> _bitesArrayToBytes(vector<unsigned char> bites);
};