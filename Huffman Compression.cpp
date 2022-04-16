#include<cstdio>
#include<cstdlib>
#include<iostream>
#include<fstream>
#include<iomanip>
#include<bitset>
#include<string>
#include<vector>
using namespace std;
struct charNode
{
    unsigned char letter;
    long long frequency;
    long long left;
    long long right;
};
struct minHeap
{
    charNode* arr; long long cap; long long size;
    minHeap(long long capacity) { size = 0; cap = capacity; arr = new charNode[cap]; }
    void heapify(long long  i)
    {
        long long  l = 2 * i + 1; long long r = 2 * i + 2; long long smallest = i;
        if (l < size && arr[l].frequency < arr[i].frequency) smallest = l;
        if (r < size && arr[r].frequency < arr[smallest].frequency) smallest = r;
        if (smallest != i)
        {
            charNode temp = arr[i]; arr[i] = arr[smallest]; arr[smallest] = temp;
            heapify(smallest);
        }
    }
    charNode min()
    {
        if (size <= 0) return { NULL, NULL, -1, -1 };
        if (size == 1) { size--; return arr[0]; }
        charNode root = arr[0]; arr[0] = arr[size - 1]; size--;
        heapify(0);
        return root;
    }
    void insert(charNode k)
    {
        if (size == cap) return;
        size++; long long i = size - 1; arr[i] = k;
        while (i != 0 && arr[(i - 1) / 2].frequency > arr[i].frequency)
        {
            charNode temp = arr[i]; arr[i] = arr[(i - 1) / 2]; arr[(i - 1) / 2] = temp;
            i = (i - 1) / 2;
        }
    }
};
minHeap h(256);
vector<long long> frequency(256);
vector<charNode> originals;
vector<vector<bool>> key(256);
void findstr(string q, long long a)
{
    long long count = 0;
    if (originals[a].left != -1)
    {
        findstr(q + "0", originals[a].left);
    }
    else
    {
        count++;
    }
    if (originals[a].right != -1)
    {
        findstr(q + "1", originals[a].right);
    }
    else
    {
        count++;
    }
    if (count == 2)
    {
        for (int i = 0; i < q.length(); i++)
        {
            key[(unsigned char) originals[a].letter].push_back(q[i] - '0');
        }
    }
}
void compress(string path)
{
    string asd = path.substr(1, path.length() - 2);
    ifstream is(asd, ios::binary);
    string buffer;
    is.seekg(0, ios::end);
    buffer.resize(is.tellg());
    is.seekg(0, ios::beg);
    buffer.assign((istreambuf_iterator<char>(is)), istreambuf_iterator<char>());
    is.close();
    for (long long i = 0; i < (long long) buffer.length(); i++)
    {
        frequency[(unsigned char) buffer[i]]++;
    }
    long long total = 0;
    for (long long i = 0; i < frequency.size(); i++)
    {
        if (frequency[i] != 0)
        {
            total += frequency[i];
            h.insert({ (unsigned char)(i), frequency[i], -1, -1 });
        }
    }
    while (true)
    {
        originals.push_back(h.min());
        originals.push_back(h.min());
        if (originals[(long long) originals.size() - 1].frequency == NULL) break;
        h.insert({ NULL, originals[(long long) originals.size() - 2].frequency + originals[(long long) originals.size() - 1].frequency, (long long) originals.size() - 2, (long long) originals.size() - 1 });
    }
    findstr("", (long long) originals.size() - 2);
    string asfsd = path.substr(1, path.find('.') - 1) + " [compressed]" + ".txt";
    ofstream os(asfsd, ios::binary);
    os << total << endl;
    long long lenkey = 0;
    for (long long i = 0; i < (long long) key.size(); i++)
    {
        if (key[i].size() != 0)
        {
            lenkey++;
        }
    }
    os << lenkey << endl;
    for (long long i = 0; i < (long long) key.size(); i++)
    {
        if (key[i].size() != 0)
        {
            os << i << " ";
            for (int j = 0; j < key[i].size(); j++)
            {
                os << key[i][j];
            }
            os << endl;
        }
    }
    long long len = 0;
    for (long long i = 0; i < (long long) buffer.length(); i++)
    {
        len += key[(unsigned char) buffer[i]].size();
    }
    os << long long(ceil(len / 8.0)) << endl;
    long long current_bit = 0;
    unsigned char bit_buffer = 0;
    for (long long i = 0; i < (long long) buffer.length(); i++)
    {
        for (long long j = 0; j < (long long) key[(unsigned char) buffer[i]].size(); j++)
        {
            long long bit = key[(unsigned char) buffer[i]][j];
            if (bit) bit_buffer |= (1 << current_bit);
            current_bit++;
            if (current_bit == 8)
            {
                os << bit_buffer;
                current_bit = 0;
                bit_buffer = 0;
            }
        }
    }
    os << bit_buffer;
    os.close();
    ifstream in_file(asd, ios::binary);
    in_file.seekg(0, ios::end);
    long long oldfile = in_file.tellg();
    in_file.close();
    ifstream out_file(asfsd, ios::binary);
    out_file.seekg(0, ios::end);
    long long newfile = out_file.tellg();
    out_file.close();
    cout << "Compression rate: " << round(((newfile * 1.0) / oldfile) * 100) / 100 << endl;
}
vector<bool> code;
vector<bool> output;
bool match(long long j)
{
    if ((long long) output.size() != (long long) key[j].size())
    {
        return false;
    }
    for (long long i = 0; i < (long long) output.size(); i++)
    {
        if (output[i] != key[j][i])
        {
            return false;
        }
    }
    return true;
}
void decompress(string path)
{
    for (long long i = 0; i < (long long) key.size(); i++)
    {
        key[i].clear();
    }
    string asd = path.substr(1, path.length() - 2);
    ifstream infile(asd, ios::binary);
    long long q, lenkey;
    infile >> q;
    infile >> lenkey;
    for (long long i = 0; i < lenkey; i++)
    {
        long long a; string b;
        infile >> a >> b;
        for (long long j = 0; j < (long long) b.length(); j++)
        {
            key[a].push_back(b[j] - '0');
        }
    }
    long long length2;
    infile >> length2;
    infile.get();
    char buffer;
    for (long long i = 0; i < length2; i++)
    {
        infile.get(buffer);
        for (long long j = 0; j < 8; j++)
        {
            code.push_back((buffer >> (j)) & 1);
        }
    }
    infile.close();
    string asfsd = path.substr(1, path.find('.') - 1) + " [decompressed]" + ".txt";
    ofstream decompressed(asfsd, ios::binary);
    for (long long i = 0; i < (long long) code.size() && q > 0; i++)
    {
        output.push_back(code[i]);
        for (long long j = 0; j < (long long) key.size(); j++)
        {
            if (match(j))
            {
                decompressed << (unsigned char) (j);
                q--;
                output.clear();
                break;
            }
        }
    }
    decompressed.close();
}
void huffmancompression()
{
    string type;
    string pathin;
    cout << "Enter the action (compress or decompress) followed by the path of the input txt within quotes: ";
    cin >> type;
    cin.get();
    getline(cin, pathin);
    if (type == "compress")
    {
        compress(pathin);
    }
    else if (type == "decompress")
    {
        decompress(pathin);
    }
}