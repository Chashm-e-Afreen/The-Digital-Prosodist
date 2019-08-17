#include <iostream>
#include <string>
#include <fstream>
#include <locale>
#include <codecvt>

// Struct to hold "word" itself / "murrab" word with aarab / "weight" A binary encoding of word where mutaharik = '1' and sakin = '0'
struct Lafz
{
	std::wstring word;
	std::wstring murrab;
	std::wstring weight;
};

// Modifies passed wide string by removing leading and trailing whitespace
void trim_whitespace(std::wstring& wstr)
{
	if (wstr.empty()) return;

	wstr = wstr.substr(wstr.find_first_not_of(L' ')); // Trim the whitespace before word
	wstr = wstr.substr(0, wstr.find_last_not_of(L' ') + 1); // Trim the whitespace after word
}


// Returns a wide string of binary characters that encode Mutaharik to 1 and Sakin to 0 in given murrab (word)
// No need for it to return wide string , we are working with wide character streams so they don't accept normal strings
// Then there is type cast though. I am just goint this route.
std::wstring generate_weight(std::wstring murrab)
{
	trim_whitespace(murrab);
			
	if (murrab.size() == 0) // The word string we were provided to generate weight was empty
	{
		std::cerr << "Cannot generate weight: The word is empty. Make sure you do not have empty line at the end of file.\n";

		std::cout << "Press any key and enter to continue.";
		char temp;
		std::cin >> temp;

		exit(-1);
	}
	else if (murrab.size() == 1) // The word only had one character
	{
		return (murrab.back() == L'آ' ? L"10" : L"1"); // Return 10 if it is آ otherwise 1
	}
	else if (murrab.size() == 2) // The word has two character
	{
		if ((murrab.back() >= 1611 && murrab.back() <= 1631) || murrab.back() == 1648) // If second character is a symbol (i.e same case a murrab.size() == 1 : single letter)
		{
			return (murrab.front() == L'آ' ? L"10" : L"1"); // Return 10 if first is آ otherwise 1
		}
	}

	// We will get here if everything is valid

	std::wstring weight; // Storing the weight of word (murrab if this what you call them words with symbols lol)
	
	std::wstring accumulatedMurrab; // Accumulate the characters of murrab as we go through them | This variable will be used for case 11 (خَوا)

	bool foundTwoConsecSakins = false;

	int currentConsecSakinCount = 0;
	bool wasCurrSakin = false;

	bool stopCheckingCharacters = false;
	int wordSize = murrab.size(); // Size of word

	if (murrab.front() == L'آ')
	{
		weight += L"10"; // If first letter is 'آ' then we prepend weight with "10"

		wasCurrSakin = true; // Exceptional cases when first letter is 'آ'
		currentConsecSakinCount = 1; // We want to start count with 1
	}

	else weight += L"1"; // Otherwise, "1" because first letter will always be mutaharik

	accumulatedMurrab += murrab.front();

	for (int i = 1; i < wordSize - 1; i++) // loop through all except first and last characters in murrab
	{
		wchar_t character = murrab[i]; // character currently under consideration

		accumulatedMurrab += character;

		if ((character >= 1611 && character <= 1631) || character == 1648) continue; // Ignore the symbols

		switch (character) // checking the current character
		{
		case L' ': case L'ھ': case L'ں': // Ignore these characters
		{
			continue;
		} break;

		default: // character is an alphabet/letter
		{
			int succeedingCharacterIndex = i;

			// We want to skip the spaces, and characters that should not be considered between current alphabet and next
			while (murrab[succeedingCharacterIndex++] == L' ' || murrab[succeedingCharacterIndex] == L'ھ' ||
				   murrab[succeedingCharacterIndex] == L'ں');

			wchar_t succeedingCharacter = murrab[succeedingCharacterIndex];

			switch (succeedingCharacter) // checking character next to current letter (can be symbol or alphabet)
			{
			case L'/': // Succeeding character is forward slash meaning end of word
			{
				murrab = murrab.substr(0, i + 1); // Update murrab to conatain characters just upto forward slash
				trim_whitespace(murrab); // Just to be safe
				wordSize = murrab.size(); // Update size
				stopCheckingCharacters = true; // We have reached the end of word stop further character checking
			} break;

			case 1617: // current letter has a shad symbol over it (this is a tricky one because we can have "0" then shad = "001"-> >"011" or "00" then shad = "0001 -> 011")
			{
				if (currentConsecSakinCount == 1)
				{
					weight += L"11";
					currentConsecSakinCount = 0;
				}
				else if (foundTwoConsecSakins)
				{
					weight.pop_back();
					weight += L"11";

					foundTwoConsecSakins = false;
					currentConsecSakinCount = 0;
				}
				else
				{
					weight += L"01";
				}

				wasCurrSakin = false;
			} break;

			case 1618: // current letter is sakin as the character next to it is symbol "sukoon"
			{
				weight += L"0";
				wasCurrSakin = true;
			}	break;

			case 1648: // current letter has "khari zabr" symbol over it
			{
				weight += L"10";
				currentConsecSakinCount = 0;
				wasCurrSakin = true;
			} break;

			case 1611:case 1612:case 1613:case 1614:case 1615:case 1616:case 1619:case 1620:case 1621:
			case 1622:case 1623:case 1624:case 1625:case 1626:case 1627:case 1628:case 1629:case 1630:
			case 1631: // current letter has symbol other than sukoon,shad,khari zabr (so itغ is mutaharik)
			{
				weight += L"1";
				wasCurrSakin = false;
			} break;

			case L'ی': case L'و': case L'ا': case L'ے':// current letter has no symbol and succeeding character is one of the specified (it's mutaharik)
			{
				weight += (character != L'آ' ? L"1" : L"10");
				wasCurrSakin = (character == L'آ');
				currentConsecSakinCount = 0;
			} break;

			default: // current letter has no symbol and succeeding character is not 'ی' 'و' 'ا' (it's sakin)
			{	
				if (character == L'آ') 
				{
					if (foundTwoConsecSakins) // If succeeding
					{
						weight.back() = L'1';
						foundTwoConsecSakins = false;
					}

					weight += L"10";
					currentConsecSakinCount = 0;
				}
				else
				{
					weight += L"0";
				}

				wasCurrSakin = true;
			} break;

			} // end of nested swtich in default section of first(checking succeeding character)
		} // end of default section of first switch

		} // end of first swtich (checking first character)

		// Our accumulatedMurrab exactly matches the signature of 'خَوا' we have a exceptional case
		if (accumulatedMurrab == L"خَوا")
		{
			// خَوا is part of a word not whole word itself (we have to retain the weight state of alif and ignore weight of waoo
			
			wchar_t alifWeight = weight.back();
			weight.pop_back();
			weight.back() = alifWeight;
		}

		if (stopCheckingCharacters) break; // break out of loop if we don't want to check further characters in murrab
		

		if (wasCurrSakin) // current character was sakin
		{
			currentConsecSakinCount++; // Increment consective sakin count
		}
		else // current character was not a sakin
		{
			// If count is 2 then we just make the last 2nd last weight value to '1' : i.e (00 -> 01)
			if (foundTwoConsecSakins)
			{
				weight[weight.size() - 2] = L'1';
			}

			foundTwoConsecSakins = false; // Reset the flags
			currentConsecSakinCount = 0; // Reset the count
		}

		if (currentConsecSakinCount == 2) // setting flag to true (will be helpful later if consec sakin occur 2ndlast, 3rdlast
		{
			foundTwoConsecSakins = true;
		}
		else if (currentConsecSakinCount == 3) // we have found three consective sakin (implies: foundTwoConsecSakin is true)
		{
			weight.pop_back(); // Ignore one sakin
			weight.back() = L'1'; // add one

			foundTwoConsecSakins = false; // reset the flags
			currentConsecSakinCount = 0;
		}

	} // end of for loop

	if (murrab == L"خَوا") return L"10"; // Whole word was "خَوا" therefore we return "10" as per rule 11

	wchar_t lastCharacter = murrab.back();

	// We have found the last character to be a symbol or character we don't treat as valid letter
	if ((lastCharacter >= 1611 && lastCharacter <= 1631) || lastCharacter == L'ں' || lastCharacter == L'ھ')
	{
		if (foundTwoConsecSakins) // Just check if the 
		{
			weight.back() = L'1';
			return weight;
		}

		return weight;
	}

	// Last character was 'khari zabr' we need to reset (10) set in switch statements for it to (0) : i.e (10 -> 0)
	else if (lastCharacter == 1648)
	{
		weight.pop_back();
		weight.back() = L'0';
		return weight;
	}

	// We have some exceptional cases for specified last characters
	else if (lastCharacter == L'ہ')
	{
		wchar_t secondLastCharacter = murrab[wordSize - 2];

		if ((secondLastCharacter >= 1611 && secondLastCharacter <= 1631) || secondLastCharacter == 1648)
			secondLastCharacter = murrab[wordSize - 3];

		// If second last charcter is not one of specified
		if (secondLastCharacter != L'و' && secondLastCharacter != L'ی' && secondLastCharacter != L'ا')
		{
			weight.back() = L'1'; // change preceding weight to 1 "mutaharik"
		}
	}

	if (foundTwoConsecSakins) // have we found two sakins before assumed last sakin. Change to 01 : i.e. (000 -> 01)
	{
		weight.back() = L'1';
		return weight;
	}

	// return weight + "1" if 2nd last character is sakin and weight + "0" when it is not
	return weight + (weight.back() == L'0' ? L"1" : L"0");
}

int main()
{
	// Wide character streams to read words,murrab csv file and output to a words,murrab,weight txt file (direct conversion to csv gives garbage values)
	std::wifstream file_read;
	std::wofstream file_write;

	// Making the file streams work with Urdu language
	file_read.imbue(std::locale(std::locale(), new std::codecvt_utf8_utf16<wchar_t, 0x10ffff, std::consume_header>));
	file_write.imbue(std::locale(std::locale(), new std::codecvt_utf8_utf16<wchar_t, 0x10ffff, std::consume_header>));

	// Open the file to convert and check for error
	file_read.open("../data/words_murrab_unique.csv");

	if (file_read.fail())
	{
		std::cerr << "Unable to open specified file for reading. Press any key and enter to continue.";

		char temp; // For pausing screen
		std::cin >> temp;

		exit(-1);
	}

	// Open the file to write and check for errors
	file_write.open("../data/words_murrab_weight_unique.txt");

	if (file_write.fail())
	{
		std::cerr << "Unable to open specified file for writing. Press any key and enter to continue.";

		char temp;
		std::cin >> temp; // For pausing screen

		exit(-2);
	}

	// This wchar variable will hold current character from the input stream as we go through all them in file
	wchar_t character = L'\0';

	// Out lafz object that will store a triplet of word,murrab,weight
	Lafz lafz;

	bool foundCommaDelim = false; // boolean to find out whether we have found a comma on current line (meaning end of word) 
	bool foundEndOfLine = false;  // boolean to find out whether we have found end of current line
	bool foundEndOfFile = false;  // boolean to find out whether we have reached end of file while parsing
	bool writeWord = false;		  // boolean to find out whether we can write our lafz.word to file 

	// while we can get a character from read stream : insert it into  character variable (doing 2 things in one statement)
	while (character = file_read.get())
	{
		switch (character) // Check the character we got from file
		{
		case L',': // We found the comma
		{
			foundCommaDelim = true; // set flag to true
			writeWord = true; // now we can write word to other file (wofstream) | set the flag to true
			continue; // We don't want comma to get into lafz.word therefore move to next character
		} break;

		case L'\n': // We found end of line
		{
			foundEndOfLine = true; // setting the flag to true

		} break;
		}

		if (file_read.eof()) // will set flag to true as we have reached end of file | will be used later so last character is not used two times
		{
			foundEndOfFile = true;
		}

		if (!foundCommaDelim) // We have'nt found the comma in current line? Put the characters in lafz.word as we are currently reading characters of word
		{
			lafz.word += character;
		}

		if (foundCommaDelim) // We have found comma in current line? Put the characters 
		{
			// Just making sure we add characters only if end of file is not reached or end of line: '\n' = end of line character will become part of murrab otherwise
			if (!foundEndOfLine && !foundEndOfFile) lafz.murrab += character;
		}

		if (writeWord) // We can write lafz.word? 
		{
			trim_whitespace(lafz.word); // Remove leading and trailing spaces

			file_write << lafz.word << L","; // Write lafz.word to file

			writeWord = false; // setting to false as we cannot write the word again as it is lafz.murrab turn now
		}

		if (foundEndOfLine || foundEndOfFile) // We found end of line (exception for last line where there will be no end of line therefore end of file for that case)?
		{
			trim_whitespace(lafz.murrab); // Again we don't need spaces

			file_write << lafz.murrab << L","; // Write murrab to file

			lafz.weight = generate_weight(lafz.murrab); // Get the weight of murrab

			file_write << lafz.weight << (!foundEndOfFile ? L"\n" : L""); // Write the weight to file

			lafz = {}; // Empty the lafz object

			// Resetting everything as we will move to a new line
			foundCommaDelim = false;
			writeWord = false;
			foundEndOfLine = false;

			// There is no new line but end of file? ( True For last line) Just break out of loop, we are finished
			if (foundEndOfFile) break;
		}
	}

	std::cout << "Conversion was successful. Press any key and enter to continue.";
	char temp;
	std::cin >> temp;
}