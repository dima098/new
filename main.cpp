#include <QCoreApplication>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <fstream>
#include <QDebug>

using namespace std;

int find_index (vector <int> &baraban, int value)
{
    for (int i = 0; i < baraban.size(); i++)
    {
        if (baraban[i] == value)
        {
            return i;
        }
    }
    return -1;
}


void create_barabans(int n, vector< vector <int> > &barabans)
{
    int value;
    for (int i = 0; i < 2 * n; i++)
    {
        vector<int> vec;
        srand(i);
        value = rand() % 256;
        while(vec.size() != 256)
        {
            if (find_index(vec, value) == -1)
            {
                vec.push_back(value);
            }
            value = rand() % 256;
        }

        barabans.push_back(vec);
    }
}

void write_bararabans(string file, vector< vector <int> > &barabans)
{
    fstream b_file;
    b_file.open(file.data(), ios :: in | ios :: out | ios :: binary | ios :: trunc);
    int size = barabans.size();
    b_file.write((char*)&size, 4);
    for (int i = 0; i < barabans.size(); i++)
    {
        for (int j = 0; j < barabans[i].size(); j++)
        {
            b_file.write((char*)&barabans[i][j], 4);
        }
    }
}

void read_bararabans(string file, vector< vector <int> > &barabans)
{
    fstream b_file;
    int n;
    b_file.open(file.data(), ios :: in | ios :: out | ios :: binary | ios :: app);
    b_file.seekp(0);
    b_file.read((char*)&n, 4);
    barabans.resize(n);
    for (int i = 0; i < n; i++)
    {
        barabans[i].resize(256);
        for (int j = 0; j < 256; j++)
        {
            b_file.read((char*)&barabans[i][j], 4);
        }
        cout << endl;
    }
}

void shift_baraban(vector <int> &baraban)
{
    int bottom = baraban[baraban.size() - 1];
    for (int i = baraban.size() - 1; i > 0; i--)
    {
        baraban[i] = baraban[i - 1];
    }
    baraban[0] = bottom;
}


unsigned char match_find(bool reverse, unsigned char find_value, vector< vector <int> > &barabans)
{
    int value = find_value, size = barabans.size(), i;

    for (reverse ? i = size - 2 : i = 1; reverse ? i >= 0 : i < size; reverse ? i-- : i++)
    {
        value = find_index(barabans[i], barabans[reverse ? i + 1 : i - 1][value]);
    }
    return value;
}

void crypt (string file_src, string file_dst, vector< vector <int> > &barabans)
{
    fstream src, dst;
    int size_file, size = barabans.size() / 2 - 1;
    char symbol;
    int *count_of_rotations = new int [size];
    for (int i = 0; i < size; i++)
    {
        count_of_rotations[i] = 0;
    }

    src.open(file_src.data(), ios :: in | ios :: out | ios :: binary | ios :: app);
    dst.open(file_dst.data(), ios :: in | ios :: out | ios :: binary | ios :: trunc);

    src.seekg(0, ios::end);
    size_file = src.tellg();

    src.seekg(0, ios :: beg);

    for (int i = 0; i < size_file; i++)
    {
        src.read((char*)&symbol, 1);
        symbol = (char)match_find(false, int(symbol), barabans);
        dst.write((char*)&symbol, 1);

        count_of_rotations[size - 1]++;
        shift_baraban(barabans[barabans.size() - 1]);
        shift_baraban(barabans[barabans.size() - 2]);
        for (int j = size - 1; j > 0; j--)
        {
            if (!(count_of_rotations[j] % 256))
            {
                count_of_rotations[j - 1]++;
                shift_baraban(barabans[2 * j]);
                shift_baraban(barabans[2 * j - 1]);
            }
        }
    }


    src.close();
    dst.close();
    delete count_of_rotations;
}

void crypt(string &text, vector< vector <int> > &barabans)
{
    for (int i = 0; i < text.size(); i++)
    {
        text[i] = match_find(false, (text[i]), barabans);
    }
}

void decrypt(string &text, vector< vector <int> > &barabans)
{
    for (int i = 0; i < text.size(); i++)
    {
        text[i] = match_find(true, (text[i]), barabans);
    }
}

void decrypt (string file_src, string file_dst, vector< vector <int> > &barabans)
{
    fstream src, dst;
    int size_file, size = barabans.size() / 2 - 1;
    char symbol;
    int *count_of_rotations = new int [size];
    for (int i = 0; i < size; i++)
    {
        count_of_rotations[i] = 0;
    }

    src.open(file_src.data(), ios :: in | ios :: out | ios :: binary | ios :: app);
    dst.open(file_dst.data(), ios :: in | ios :: out | ios :: binary | ios :: trunc);

    src.seekg(0, ios::end);
    size_file = src.tellg();

    src.seekg(0, ios :: beg);

    for (int i = 0; i < size_file; i++)
    {
        src.read((char*)&symbol, 1);
        symbol = (char)match_find(true, int(symbol), barabans);
        dst.write((char*)&symbol, 1);

        count_of_rotations[size - 1]++;
        shift_baraban(barabans[barabans.size() - 1]);
        shift_baraban(barabans[barabans.size() - 2]);
        for (int j = size - 1; j > 0; j--)
        {
            if (!(count_of_rotations[j] % 256))
            {
                count_of_rotations[j - 1]++;
                shift_baraban(barabans[2 * j]);
                shift_baraban(barabans[2 * j - 1]);
            }
        }
    }

    src.close();
    dst.close();
    delete count_of_rotations;
}

int main(int argc, char *argv[])
{
    vector < vector <int> > barabans;

    if (!strcmp(argv[1], "-e"))
    {

        read_bararabans(argv[4], barabans);
        crypt(argv[2], argv[3], barabans);
    }
    else
    {
        if (!strcmp(argv[1], "-d"))
        {
            read_bararabans(argv[4], barabans);
            decrypt(argv[2], argv[3], barabans);
        }
        else
        {
            if (!strcmp(argv[1], "-k"))
            {
                create_barabans(atoi(argv[3]), barabans);
                write_bararabans(argv[2], barabans);
            }
            else
            {
                cout << "ERROR" << endl;
            }
        }
    }
    return 0;
}
