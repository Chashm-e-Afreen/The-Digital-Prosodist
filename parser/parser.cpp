#include <iostream>
#include <string>
#include <fstream>
#include <locale>
#include <algorithm>

// Struct to store self defining attributes
struct Lafz
{
    std::wstring word;
    std::wstring murrab;
    std::wstring weight;
};

// Modifies passed wide string by removing leading and trailing whitespace
void trim_whitespace(std::wstring &wstr)
{
    wstr = wstr.substr(wstr.find_first_not_of(L' '));       // Trim the whitespace before word
    wstr = wstr.substr(0, wstr.find_last_not_of(L' ') + 1); // Trim the whitespace after word
}

// Returns a wide string of binary characters that encode Mutaharik to 1 and Sakin to 0 in given murrab (word)
// No need for it to return wide string , we are woking with wide character streams so they don't accept normal strings
// Then there is type cast though. I am just goint this route.
std::wstring generate_weight(std::wstring murrab)
{
    trim_whitespace(murrab);

    if (murrab.size() == 0) // The word string we were provided to generate weight was empty
    {
        std::cerr << "Cannot generate weight: The word is empty\n";
        exit(-1);
    }
    else if (murrab.size() == 1) // The word only had one character
    {
        if (murrab[0] == L'آ') // Valid if that character is 'آ'
        {
            return L"10";
        }
        else // Otherwise, word cannot consist of only one character
        {
            std::cerr << "Cannot generate weight: The word is of character length 'one'";
            exit(-2);
        }
    }

    // We will get here if everything is valid

    std::wstring weight; // Storing the weight of word (murrab if this what you call them words with symbols lol)

    int wordSize = murrab.size(); // Size of word

    if (murrab[0] == L'آ')
        weight += L"10"; // If first letter is 'آ' then we prepend weight with "10"
    else
        weight += L"1"; // Otherwise, "1" because first letter will always

    for (int i = 1; i < wordSize - 1; i++) // loop through all except first and last characters
    {
        wchar_t character = murrab[i];

        if (character >= 1611 && character <= 1631)
            continue;

        switch (character) // checking the current character
        {
        case L' ':
        case L'ھ':
        case L'ں': // Ignore these characters
        {
            continue;
        }
        break;

        case L'/':
        {
            // Todo()
            weight.back() = L'0';
            return weight;
        }
        break;

        default: // character is an alphabet / letter
        {
            int succeedingCharacterIndex = i;

            while (murrab[++succeedingCharacterIndex] == L' ')
                ; // We want to skip the spaces between current alphabet and next alphabet

            wchar_t succeedingCharacter = murrab[succeedingCharacterIndex];

            switch (succeedingCharacter) // checking character next to current letter (can be symbol or alphabet)
            {
            case 1617: // current letter has a shad symbol over it
            {
                weight += L"01";
            }
            break;

            case 1618: // current letter was sakin as character next to it is symbol "sukoon"
            {
                weight += L"0";
            }
            break;

            case 1611:
            case 1612:
            case 1613:
            case 1614:
            case 1615:
            case 1616:
            case 1619:
            case 1620:
            case 1621:
            case 1622:
            case 1623:
            case 1624:
            case 1625:
            case 1626:
            case 1627:
            case 1628:
            case 1629:
            case 1630:
            case 1631: // current letter has symbol other than sukoon (so it is mutaharik)
            {
                weight += L"1";
            }
            break;

            case L'ی':
            case L'و':
            case L'ا': // current letter has no symbol and succeeding character is one of the specified (it's mutaharik)
            {
                weight += (character != L'آ' ? L"1" : L"10");
            }
            break;

            default: // current letter has no symbol on succeeding character is not 'ی' 'و' 'ا' (it's sakin)
            {
                weight += (character != L'آ' ? L"0" : L"10");
            }
            break;
            }
        }
        }
    }

    wchar_t lastCharacter = murrab.back();

    // We have found the last character to a symbol or character we don't need so we need to change the weight's last member to sakin
    if (lastCharacter >= 1611 && lastCharacter <= 1631 || lastCharacter == L'ں' || lastCharacter == L'ھ')
    {
        weight.back() = L'0';
        return weight;
    }

    return weight + (weight.back() == L'0' ? L"1" : L"0");
}

int main()
{
    std::wifstream file_read;
    std::wofstream file_write;

    SetConsoleOutputCP(CP_UTF8);

    file_read.imbue(std::locale("ur_PK.UTF8"));
    file_write.imbue(std::locale("ur_PK.UTF8"));

    file_read.open("C:/Users/Shayan Ali Abbasi/Desktop/File.txt");

    if (file_read.fail())
    {
        exit(-1);
    }

    file_write.open("C:/Users/Shayan Ali Abbasi/Desktop/words_weight.txt");

    wchar_t character = L'\0';

    Lafz lafz;

    bool foundCommaDelim = false;
    bool foundEndOfLine = false;
    bool foundEndOfFile = false;
    bool writeWord = false;

    file_read.get();

    while (character = file_read.get())
    {
        switch (character)
        {
        case L',':
        {
            foundCommaDelim = true;
            writeWord = true;
            continue;
        }
        break;

        case L'\n':
        {
            foundEndOfLine = true;
        }
        break;
        }

        if (file_read.eof())
        {
            foundEndOfFile = true;
        }

        if (!foundCommaDelim)
        {
            lafz.word += character;
        }

        if (foundCommaDelim)
        {
            if (!foundEndOfLine && !foundEndOfFile)
                lafz.murrab += character;
        }

        if (writeWord)
        {
            trim_whitespace(lafz.word);

            file_write << lafz.word << L",";

            writeWord = false;
        }

        if (foundEndOfLine || foundEndOfFile)
        {
            trim_whitespace(lafz.murrab);

            file_write << lafz.murrab << L",";

            lafz.weight = generate_weight(lafz.murrab);

            file_write << lafz.weight << L"\n";

            lafz = {};

            foundCommaDelim = false;
            writeWord = false;
            foundEndOfLine = false;

            if (foundEndOfFile)
                break;
        }
    }
}