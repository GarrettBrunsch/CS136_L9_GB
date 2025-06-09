// Garrett Brunsch
// Lab #9 BST S25
// CS_136_14728SP25P
// Due 5/4 with cutoff 5/11

// Please note the original program was designed using Visual Studio. This line is now included to ensure compatability with both online GDB and Visual Studio (syntax changes were needed that triggers this warning)
#define _CRT_SECURE_NO_WARNINGS 

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cctype>
#include <cstring>
#include <string>
#include <limits> // GDB lacks inherent support; included for GDB compatability

using namespace std;

const char* IN_FILE = "in_data.txt";

enum MenuOption
{
    READ_UPDATE_FILE = 1,
    BUILD_BST,
    PRINT_BST,
    QUIT
};

class Node
{
public:
    int count = 1;
    char* word = nullptr;
    Node* left = nullptr;
    Node* right = nullptr;

    Node();
    Node(const char* w);
    Node(const Node& other);
    Node& operator=(const Node& other);
    ~Node();
};

class BST
{
private:
    Node* root = nullptr;

    void insertHelper(Node*& node, const char* word);
    void printHelper(Node* node);

    void destroy(Node* node);
    Node* copy(Node* other);

public:
    void print();
    void insert(const char* word);
    bool isEmpty() const { return root == nullptr; }

    BST();
    BST(const BST& other);
    BST& operator=(const BST& other);
    ~BST();
};

void displayMenu();
void clearInputError(string& error);

void toLowerStr(string& str);
bool isPunctuation(char ch);

bool updateFile(const char* IN_FILE);
void replaceShortWords(fstream& file);
void removePunctuation(fstream& file);
void resetFilePosition(fstream& file);

bool buildBSTFromFile(const char* IN_FILE, BST& tree);

int main()
{
    try
    {
        BST tree;
        int choice = 0;
        bool fileUpdated = false;

        while (choice != QUIT)
        {
            displayMenu();
            cin >> choice;

            switch (choice)
            {
            case READ_UPDATE_FILE:
                fileUpdated = updateFile(IN_FILE);
                break;
            case BUILD_BST:
                buildBSTFromFile(IN_FILE, tree);
                break;
            case PRINT_BST:
                tree.print();
                break;
            case QUIT:
                cout << "Now exiting program...\n";
                break;
            default:
                string error = "Invalid input detected. Please double check entry and try again\n";
                clearInputError(error);
                break;
            }
        }
    }
    catch (const exception& e)
    {
        cerr << "Error: " << e.what() << "\n";
    }
    return 0;
}

void displayMenu()
{
    cout << "\n\n=== Main Menu ===\n"
        "1. Read and update the input file\n"
        "2. Build BST from current file\n"
        "3. Print BST (words and frequency)\n"
        "4. Quit\n"
        "Enter your choice: ";
}

void clearInputError(string& error)
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << error;
}

void toLowerStr(string& str)
{
    for (size_t i = 0; i < str.length(); i++)
    {
        str[i] = static_cast<char>(tolower(static_cast<unsigned char>(str[i]))); // Cast for ascii inputs
    }
}

bool isPunctuation(char ch)
{
    return ispunct(static_cast<unsigned char>(ch));
}

//Replaces every punctuation with a single blank space and words of 3 chars or less with cs136
bool updateFile(const char* IN_FILE)
{
    // Tellg and tellp will be misaligned if bitwise OR isn't used here. fstream will mess with things 
    // with \n & \r\n. Apparently it's a windows specific issue
    fstream file(IN_FILE, ios::in | ios::out | ios::binary);
    bool success = false;
    if (file)
    {
        removePunctuation(file);
        replaceShortWords(file);
        file.close();
        success = true;
    }
    return success;
}

void replaceShortWords(fstream& file)
{
    resetFilePosition(file);

    string tempFileName = "temp.txt";
    ofstream tempFile(tempFileName, ios::binary);
    bool tempFileCreated = tempFile.is_open();

    if (tempFileCreated)
    {
        char tempCh;
        while (file.get(tempCh))
        {
            tempFile.put(tempCh);
        }
        tempFile.close();

        ifstream readFile(tempFileName, ios::binary);
        bool readFileOpened = readFile.is_open();

        if (readFileOpened)
        {
            // Reset the original file to overwrite
            resetFilePosition(file);
            file.clear();

            char ch;
            string word = "";
            streampos wordStart = 0;
            bool inWord = false;

            // Read from temp then write to original w/ replacements
            streampos writePos = 0;
            while (readFile.get(ch))
            {
                if (isalpha(static_cast<unsigned char>(ch)))
                {
                    if (!inWord)
                    {
                        inWord = true;
                        wordStart = writePos;
                    }
                    word += ch;
                    file.seekp(writePos);
                    file.put(ch);
                    writePos = static_cast<streampos>(static_cast<streamoff>(writePos) + 1);
                }
                else
                {
                    if (inWord)
                    {
                        inWord = false;

                        string lowerWord = word;
                        toLowerStr(lowerWord);

                        if (lowerWord.length() <= 3)
                        {
                            file.seekp(wordStart);
                            file.write("cs136", 5);
                            writePos = static_cast<streampos>(static_cast<streamoff>(wordStart) + 5);

                            if (!isspace(static_cast<unsigned char>(ch)))
                            {
                                file.seekp(writePos);
                                file.put(' ');
                                writePos = static_cast<streampos>(static_cast<streamoff>(writePos) + 1);
                            }
                        }
                        word.clear();
                    }

                    // Writes the nonalpha character
                    file.seekp(writePos);
                    file.put(ch);
                    writePos = static_cast<streampos>(static_cast<streamoff>(writePos) + 1);
                }
            }

            // last word
            if (inWord)
            {
                string lowerWord = word;
                toLowerStr(lowerWord);

                if (lowerWord.length() <= 3)
                {
                    file.seekp(wordStart);
                    file.write("cs136", 5);
                    writePos = static_cast<streampos>(static_cast<streamoff>(wordStart) + 5);
                    file.seekp(writePos);
                    file.put(' ');
                }
            }
            readFile.close();
        }
        remove(tempFileName.c_str());
    }
    resetFilePosition(file);
}

void removePunctuation(fstream& file)
{
    resetFilePosition(file);

    char ch;
    streampos readPos, writePos;

    while (file.get(ch))
    {
        readPos = file.tellg();

        if (isPunctuation(ch))
        {
            writePos = readPos - static_cast<streamoff>(1);
            file.seekp(writePos);
            file.put(' ');
            file.seekg(readPos);
        }
    }
    resetFilePosition(file);
}

void resetFilePosition(fstream& file)
{
    file.clear();
    file.seekg(0, ios::beg);
    file.seekp(0, ios::beg);
}

bool buildBSTFromFile(const char* IN_FILE, BST& tree)
{
    bool success = false;
    ifstream inFile(IN_FILE);
    if (inFile)
    {
        string word;
        while (inFile >> word)
        {
            toLowerStr(word);
            tree.insert(word.c_str());
        }
        inFile.close();
        success = true;
    }
    else
    {
        cerr << "Error:" << IN_FILE << " could not be opened for reading\n";
    }
    return success;
}

Node::Node() : word(nullptr), count(0), left(nullptr), right(nullptr) {} // should never be called

Node::Node(const char* w)
{
    count = 1;
    word = nullptr;
    left = nullptr;
    right = nullptr;

    try
    {
        if (w == nullptr)
        {
            throw invalid_argument("Word cannot be null");
        }
        size_t len = strlen(w) + 1;
        word = new char[len];
        strncpy(word, w, len - 1);
        word[len - 1] = '\0';
    }

    catch (...)
    {
        delete[] word;
        word = nullptr;
        count = 0;
    }
}

Node::Node(const Node& other)
{
    count = other.count;
    left = nullptr;
    right = nullptr;
    word = nullptr;

    if (other.word != nullptr)
    {
        size_t len = strlen(other.word) + 1;
        try
        {
            word = new char[len];
            strncpy(word, other.word, len - 1);
            word[len - 1] = '\0';

        }
        catch (...)
        {
            delete[] word;
            word = nullptr;
        }
    }
}

Node& Node::operator=(const Node& other)
{
    if (this != &other)
    {
        char* temp_word = nullptr;
        try
        {
            if (other.word != nullptr)
            {
                size_t len = strlen(other.word) + 1; // +1 used to account for null terminator with char
                temp_word = new char[len];
                strncpy(temp_word, other.word, len - 1);
                temp_word[len - 1] = '\0';

            }
            delete[] word;
            word = temp_word;
            count = other.count;
        }
        catch (const std::bad_alloc &e)
        {
            delete[] temp_word;
            cerr <<"Memory allocation failed during node assignment\n";
            word = nullptr;
            count = 0;
        }
    }
    return *this;
}

Node::~Node()
{
    if (word != nullptr)
    {
        delete[] word;
    }
}

BST::BST()
{
    root = nullptr;
}

BST::BST(const BST& other)
{
    root = nullptr;
    try
    {
        root = copy(other.root);
    }
    catch (const std::bad_alloc& e)
    {
        destroy(root);
        root = nullptr;
        cerr << "Memory allocation failed during BST copy\n";
    }
}

BST& BST::operator=(const BST& other)
{
    if (this != &other)
    {
        BST temp(other);
        swap(root, temp.root);
    }
    return *this;
}

BST::~BST()
{
    destroy(root);
}

void BST::insert(const char* word)
{
    if (word != nullptr)
    {
        try
        {
            insertHelper(root, word);
        }
        catch (const std::exception& e)
        {
            cerr << "Error inserting word: " << e.what() << "\n";
        }
    }
}

void BST::insertHelper(Node*& node, const char* word)
{
    if (node == nullptr)
    {
        try
        {
            node = new Node(word);
        }
        catch (bad_alloc&)
        {
            cerr << "Memory allocation failed while inserting \"" << word << "\"\n";
        }
    }
    else
    {
        int cmp = strcmp(word, node->word);
        if (cmp == 0)
        {
            node->count++;
        }
        else if (cmp < 0)
        {
            insertHelper(node->left, word);
        }
        else
        {
            insertHelper(node->right, word);
        }
    }
}

void BST::destroy(Node* node)
{
    if (node != nullptr)
    {
        destroy(node->left);
        destroy(node->right);
        delete node;
    }
}

Node* BST::copy(Node* other)
{
    Node* newNode = nullptr;
    if (other != nullptr)
    {
        try
        {
            newNode = new Node(other->word);
            newNode->count = other->count;
            newNode->left = copy(other->left);
            newNode->right = copy(other->right);
        }
        catch (exception& e)
        {
            destroy(newNode);
            cerr << "Error copying node: " << e.what() << "\n";
            newNode = nullptr;
        }
    }
    return newNode;
}

void BST::print()
{
    cout << "\n" << left << setw(15) << "Word" << right << setw(15) << "Count\n"
        << string(30, '-') << "\n";

    printHelper(root);
}

void BST::printHelper(Node* node)
{
    if (node != nullptr)
    {
        printHelper(node->left);
        cout << left << setw(15) << node->word
            << right << setw(15) << node->count << "\n";
        printHelper(node->right);
    }
}

/*


=== Main Menu ===
1. Read and update the input file
2. Build BST from current file
3. Print BST (words and frequency)
4. Quit
Enter your choice: 1


=== Main Menu ===
1. Read and update the input file
2. Build BST from current file
3. Print BST (words and frequency)
4. Quit
Enter your choice: 2


=== Main Menu ===
1. Read and update the input file
2. Build BST from current file
3. Print BST (words and frequency)
4. Quit
Enter your choice: 3


TEST CASE 1: BASE CASE
Input : This is a basic sentence to ensure that the program works
Output :
Word                    Count
------------------------------
basic                        1
cs136                        4
ensure                       1
program                      1
sentence                     1
that                         1
this                         1
works                        1


TEST CASE 2 : STANDARD PUNCTUATION
Input : This test will, see, if commas and other.standard; punctuation marks "are "removed ? from the input
Output :
Word                    Count
------------------------------
commas                       1
cs136                        5
from                         1
input                        1
marks                        1
other                        1
punctuation                  1
removed                      1
standard                     1
test                         1
this                         1
will                         1


TEST CASE 3 : SHORT WORDS
Input : no a or an nor the to be is — cs136 should replace every nine instances plus singular example
Output :
Word                    Count
------------------------------
136                          1
cs136                       10
every                        1
example                      1
instances                    1
nine                         1
plus                         1
replace                      1
should                       1
singular                     1
ù                            1

TEST CASE 4 : RANDOM NON - ALPHA CHARACTERS
Input : tough—until should become tough until and other $random characters$ should % be removed!
Output :
Word                    Count
------------------------------
become                       1
characters                   1
cs136                        2
other                        1
random                       1
removed                      1
should                       2
tough                        1
toughùuntil                  1
until                        1


TEST CASE 5 : MULTIPLE SPACES
Input : Multiple     spaces     should     collapse     between words
Output :
Word                    Count
------------------------------
between                      1
collapse                     1
multiple                     1
should                       1
spaces                       1
words                        1


TEST CASE 6 : CAPITALIZATION
Input : VIDEO video ViDeO should all normalize to video and count as three instances of the same word
Output :
Word                    Count
------------------------------
count                        1
cs136                        6
instances                    1
normalize                    1
same                         1
should                       1
three                        1
video                        4
word                         1


TEST CASE 7 : NO ENTRIES
Input :
Output:
Word                    Count
------------------------------


TEST CASE 8 : LONG INPUT
Input : THIS is a RANDOM set of random $CHARACTERS $AND!worDS to ensure that thIS program cAN handle WORDS of any size and LENGTH, within, reasonable # or amounts @ the behest of  myself or MY own sanity.Wow what a run on sentence that was
Output :
Word                    Count
------------------------------
amounts                      1
behest                       1
characters                   1
cs136                       21
ensure                       1
handle                       1
length                       1
myself                       1
program                      1
random                       2
reasonable                   1
sanity                       1
sentence                     1
size                         1
that                         2
this                         2
what                         1
within                       1
words                        2


=== Main Menu ===
1. Read and update the input file
2. Build BST from current file
3. Print BST (words and frequency)
4. Quit
Enter your choice: g
Invalid input detected. Please double check entry and try again


=== Main Menu ===
1. Read and update the input file
2. Build BST from current file
3. Print BST (words and frequency)
4. Quit
Enter your choice: 4
Now exiting program...

*/
