#include <iostream>
#include <string.h>
#include <string>

using namespace std;

int charToInt(unsigned char c)
{
    return (int)c - 48;
}

unsigned char intToChar(int number)
{
    return (unsigned char)(48 + number);
}

long double intPow(long double a, int x)
{
    long double result = 1;

    for (int i = 0; i < x; i++)
    {
        result *= a;
    }

    return result;
}

class Array
{
private:
    int a[100];
    int count;

public:
    Array()
    {
        count = 0;
        for (int i = 0; i < 100; i++) a[i] = 0;
    }

    int operator [] (int i)
    {
        return a[i];
    }

    virtual void Append(int n)
    {
        a[count] = n;
        count += 1;
    }

    void SetI(int n, int i)
    {
        a[i] = n;
        if (i >= count) count = i + 1;
    }

    void Print()
    {
        for (int i = 0; i < count - 1; i++)
            cout << a[i] << ' ';
        cout << a[count - 1];
    }
};

enum Sign
{
    PLUS,
    MINUS
};

class Decimal : public Array
{
private:
    Sign sign = PLUS;
    unsigned char array_[100];
    int count;

public:
    Decimal()
    {
        count = 0;
        for (int i = 0; i < 100; i++)
        {
            array_[i] = '0';
        }
    }

    Decimal(Sign s)
    {
        sign = s;
        count = 0;
        for (int i = 0; i < 100; i++)
        {
            array_[i] = '0';
        }
    }

    Decimal(long double number)
    {
        char buff[101];
        sprintf_s(buff, "%.14f", number);

        sign = buff[0] == '-' ? MINUS : PLUS;
        int iStart = 0;
        if (buff[0] == '-') iStart += 1;
        for (count = 0; count < 100; count++)
        {
            if (buff[count + iStart] == '.') break;
        }

        for (int i = 0; i < count; i++)
        {
            array_[count - i - 1] = buff[i + iStart];
        }
    }

    Decimal(string str)
    {
        int len = (int)str.length();

        if (str[0] == '-') count = len - 1;
        else count = len;

        sign = str[0] == '-' ? MINUS : PLUS;

        for (int i = 0; i < count; i++)
        {
            array_[i] = str[len - i - 1];
        }
    }

    void UpdateCount()
    {
        for (int i = count - 1; i >= 0; i--)
        {
            if (array_[i] != '0')
            {
                count = i + 1;
                return;
            }
        }
    }

    void Append(unsigned char c)
    {
        array_[count] = c;
        count += 1;
        UpdateCount();
    }

    void SetI(unsigned char c, int i)
    {
        array_[i] = c;
        if (i >= count) count = i + 1;
        UpdateCount();
    }

    Decimal GetAddModule(Decimal amount)
    {
        Decimal result = Decimal(sign);
        int tail = 0;
        int c = (count > amount.count ? count : amount.count) + 1;

        for (int i = 0; i < c; i++)
        {
            int d1 = charToInt(array_[i]);
            int d2 = charToInt(amount.array_[i]);

            if (d1 + d2 == 0 && i == c - 1) break;

            int sum = d1 + d2 + tail;
            result.SetI(intToChar(sum % 10), i);
            tail = sum >= 10 ? 1 : 0;
        }

        if (tail != 0) result.SetI('1', c - 1);

        return result;
    }

    Decimal GetSubstractModule(Decimal amount, Sign s)
    {
        Decimal result = Decimal(s);
        int debt = 0;

        for (int i = 0; i < count; i++)
        {
            int d1 = charToInt(array_[i]);
            int d2 = charToInt(amount.array_[i]);

            int sum = d1 - d2 - debt;
            if (sum < 0)
            {
                result.SetI(intToChar(10 + sum), i);
                debt = 1;
            }
            else
            {
                result.SetI(intToChar(sum), i);
                debt = 0;
            }
        }
        return result;
    }

    bool operator > (Decimal other)
    {
        if (count != other.count) return count > other.count;
        for (int i = count - 1; i >= 0; i--)
        {
            if (array_[i] != other.array_[i]) return charToInt(array_[i]) > charToInt(other.array_[i]);
        }

        return false;
    }

    Decimal operator + (Decimal other)
    {
        if (other.sign != sign)
        {
            if (*this > other) return GetSubstractModule(other, sign);
            return other.GetSubstractModule(*this, other.sign);
        }

        return GetAddModule(other);
    }

    Decimal operator - (Decimal other)
    {
        if (other.sign == sign)
        {
            if (*this > other) return GetSubstractModule(other, sign);
            return other.GetSubstractModule(*this, sign == MINUS ? PLUS : MINUS);
        }

        return GetAddModule(other);
    }

    Decimal operator * (Decimal other)
    {
        Decimal bigger;
        Decimal smaller;

        if (*this > other)
        {
            bigger = *this;
            smaller = other;
        }
        else
        {
            smaller = *this;
            bigger = other;
        }

        int len = count + other.count;
        int* arr = new int[len];

        for (int i = 0; i < len; i++)
        {
            arr[i] = 0;
        }


        for (int i = 0; i < smaller.count; i++)
        {
            int smallerValue = charToInt(smaller.array_[i]);

            for (int j = 0; j < bigger.count; j++)
            {
                arr[j + i] += charToInt(bigger.array_[j]) * smallerValue;
            }
        }

        for (int i = 0; i < len - 1; i++)
        {
            arr[i + 1] += arr[i] / 10;
            arr[i] = arr[i] % 10;
        }

        string strResult = sign == other.sign ? "" : "-";
        bool zeroSkiped = false;

        for (int i = len - 1; i >= 0; i--)
        {
            if (!zeroSkiped)
            {
                if (arr[i] != 0)
                {
                    strResult += intToChar(arr[i]);
                    zeroSkiped = true;
                }
                continue;
            }

            strResult += intToChar(arr[i]);
        }

        delete[] arr;
        return Decimal(strResult);
    }

    Decimal operator / (Decimal other)
    {
        return Decimal((int)(ToDouble() / other.ToDouble()));
    }

    long double ToDouble()
    {
        string str;
        if (sign == MINUS) str += '-';

        for (int i = count - 1; i >= 0; i--)
        {
            str += array_[i];
        }

        return stod(str);
    }

    void SetSign(Sign s)
    {
        sign = s;
    }

    void Read(int n)
    {
        int s;
        cin >> s;
        sign = s == -1 ? MINUS : PLUS;
        count = 0;
        for (int i = 0; i < 100; i++)
        {
            if (i >= n)
                array_[i] = '0';
            else
            {
                cin >> array_[i];
                count += 1;
            }
        }
        UpdateCount();
    }

    void Print()
    {
        if (sign == MINUS) cout << '-';
        for (int i = count - 1; i >= 0; i--)
            cout << array_[i];
    }
};

void printDecimalInfo(Decimal d1, Decimal d2)
{
    cout << "Decimal1 ";
    d1.Print();
    cout << " Decimal2 ";
    d2.Print();
    cout << "\n";

    Decimal results[4] = { d1 + d2, d1 - d2, d1 * d2, d1 / d2 };
    char op[4] = { '+', '-', '*', '/' };
    for (int i = 0; i < 4; i++)
    {
        cout << "Decimal1" << op[i] << "Decimal2" << (i == 3 ? ' ' : '\t');
        results[i].Print();
        cout << '\n';
    }
}

void printArrayInfo(Array a)
{
    cout << "Array ";
    a.Print();
    cout << endl;
}

int main()
{
    int n;
    Array a;
    cin >> n;
    for (int i = 0; i < n; i++)
    {
        int el;
        cin >> el;
        a.Append(el);
    }
    Decimal d[2];
    int dN;
    cin >> dN;
    for (int i = 0; i < 2; i++)
    {
        d[i].Read(dN);
    }

    printArrayInfo(a);
    printDecimalInfo(d[0], d[1]);

    bool doCycle = true;
    int op = 0;
    int changeI = 0;
    int value = 0;

    while (doCycle)
    {
        cin >> op;
        switch (op)
        {
        case(0):
            doCycle = false;
            break;
        case(1):
            cin >> changeI;
            cin >> value;
            a.SetI(value, changeI);
            printArrayInfo(a);
            break;
        case(2):
            d[0].Read(dN);
            printDecimalInfo(d[0], d[1]);
            break;
        case(3):
            d[1].Read(dN);
            printDecimalInfo(d[0], d[1]);
            break;
        }
    }
}
