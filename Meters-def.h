﻿#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

#define NUM_METERS 129
#define NUM_RUBAI_METERS 12
#define NUM_HINDI_METERS 11
#define NUM_FEET 32
#define TOTAL_METERS  (NUM_METERS + NUM_RUBAI_METERS + NUM_HINDI_METERS)

const std::unordered_map<int, int> Usage_map
{
    {13,   1},
    {14,   1},
    {15,   1},
    {16,   1},
    {17,   1},
    {2,	   1},
    {2,	   1},
    {4,	   1},
    {4,	   0},
    {4,	   0},
    {4,	   0},
    {18,   1},
    {19,   1},
    {3,	   1},
    {3,	   1},
    {20,   1},
    {21,   1},
    {22,   1},
    {23,   1},
    {5,	   1},
    {5,	   1},
    {5,	   1},
    {24,   1},
    {25,   1},
    {26,   1},
    {27,   1},
    {6,	   1},
    {6,	   1},
    {6,	   1},
    {6,	   1},
    {30,   1},
    {31,   1},
    {32,   1},
    {33,   1},
    {34,   1},
    {35,   1},
    {35,   1},
    {35,   1},
    {35,   1},
    {36,   1},
    {40,   1},
    {41,   1},
    {42,   1},
    {43,   1},
    {44,   1},
    {45,   1},
    {46,   1},
    {47,   1},
    {48,   1},
    {49,   1},
    {50,   1},
    {51,   1},
    {52,   1},
    {53,   1},
    {54,   1},
    {55,   1},
    {56,   1},
    {57,   1},
    {58,   1},
    {59,   1},
    {60,   1},
    {61,   1},
    {62,   1},
    {63,   1},
    {7,	   1},
    {103,  1},
    {64,   1},
    {65,   1},
    {8,	   1},
    {8,	   0},
    {8,	   0},
    {8,	   0},
    {9,	   1},
    {9,	   0},
    {9,	   1},
    {9,	   0},
    {10,   1},
    {10,   0},
    {66,   1},
    {67,   1},
    {68,   1},
    {69,   1},
    {70,   1},
    {71,   1},
    {72,   1},
    {73,   1},
    {74,   1},
    {75,   1},
    {76,   1},
    {77,   1},
    {1,	   1},
    {1,	   1},
    {1,	   1},
    {1,	   1},
    {11,   1},
    {11,   1},
    {78,   1},
    {79,   1},
    {80,   1},
    {81,   1},
    {12,   1},
    {12,   0},
    {12,   0},
    {12,   0},
    {12,   0},
    {82,   1},
    {83,   1},
    {84,   1},
    {85,   1},
    {86,   1},
    {87,   1},
    {88,   1},
    {89,   1},
    {90,   1},
    {91,   1},
    {92,   1},
    {93,   1},
    {94,   1},
    {95,   1},
    {36,   1},
    {96,   1},
    {97,   1},
    {98,   1},
    {99,   1},
    {100,  1},
    {101,  1},
    {102,  1},
    {103,  1},
    {104,  1}
};

const std::unordered_map<std::wstring, const std::wstring> RubaiMeter_map
{
    {L"10101/110101/110101/110",   L"ہزج مثمّن اخرب مکفوف مجبوب"},
    {L"10101/110101/1101010/10",   L"ہزج مثمّن اخرب مکفوف ابتر"},
    {L"10101/110110/1101010/10",   L"ہزج مثمّن اخرب مقبوض ابتر"},
    {L"10101/110110/110101/110",   L"ہزج مثمّن اخرب مقبوض مکفوف مجبوب"},
    {L"101010/10110/110101/110",   L"ہزج مثمّن اخرم اشتر مکفوف مجبوب"},
    {L"101010/10110/1101010/10",   L"ہزج مثمّن اخرم اشتر ابتر"},
    {L"10101/1101010/101010/10",   L"ہزج مثمّن اخرب اخرم ابتر"},
    {L"10101/1101010/10101/110",   L"ہزج مثمّن اخرب مجبوب"},
    {L"101010/101010/10101/110",   L"ہزج مثمّن اخرم اخرب مجبوب"},
    {L"101010/101010/101010/10",   L"ہزج مثمّن اخرم ابتر"},
    {L"101010/10101/1101010/10",   L"ہزج مثمّن اخرم اخرب ابتر"},
    {L"101010/10101/110101/110",   L"ہزج مثمّن اخرم اخرب مکفوف مجبوب"}
};

const std::unordered_map<std::wstring, const std::wstring> HindiMeter_map
{
    {L"10(10)/10(10)/10(10)/10(10)/10(10)/10(10)/10(10)/10",       L"بحرِ ہندی/ متقارب مثمن مضاعف"},
    {L"10(10)/10(10)/10(10)/10(10)/10(10)/10",					L"بحرِ ہندی/ متقارب مسدس مضاعف"},
    {L"10(10)/10(10)/10(10)/10(10)/10(10)/10(10)/10(10)/1010",		L"بحرِ ہندی/ متقارب اثرم مقبوض محذوف مضاعف"},
    {L"10(10)/10(10)/10(10)/10",							L"بحرِ ہندی/ متقارب مربع مضاعف"},
    {L"10(10)/10(10)/10(10)/1010",							L"بحرِ ہندی/ متقارب اثرم مقبوض محذوف"},
    {L"10(10)/10(10)/10",								L"بحرِ ہندی/ متقارب مثمن محذوف"},
    {L"10(10)/10(10)/10(10)/10(10)/10(10)/1010",				L"بحرِ ہندی/ متقارب مسدس محذوف"},
    {L"10(10)/10(10)",									L"بحرِ ہندی/ متقارب مربع محذوف"},
    {L"(10)10/(10)10/(10)10/(10)10/(10)10/(10)10/(10)10/(10)10",	L"بحرِ زمزمہ/ متدارک مثمن مضاعف"},
    {L"(10)10/(10)10/(10)10/(10)10/(10)10/(10)10",				L"بحرِ زمزمہ/ متدارک مسدس مضاعف"},
    {L"(10)10/(10)10/(10)10/(10)",							L"بحرِ زمزمہ/ متدارک مربع مضاعف"}
};

const std::unordered_map<std::wstring, const std::wstring> Meter_map
{
    {L"1101010110101011010101101010",									L"مفاعیلن مفاعیلن مفاعیلن مفاعیلن"},
    {L"11010101101010110101011010",										L"مفاعیلن مفاعیلن مفاعیلن فعولن"},
    {L"110110110110110110110110",										L"مفاعلن مفاعلن مفاعلن مفاعلن"},
    {L"101101101010101101101010",										L"فاعلن مفاعیلن فاعلن مفاعیلن"},
    {L"11011011010101101101101010",										L"مفاعلن مفاعلن مفاعلن مفاعیلن"},
    {L"101011101011101011101010",										L"مفعول مفاعیل مفاعیل مفاعیلن"},
    {L"101011101010101011101010",										L"مفعول مفاعیلن مفعول مفاعیلن"},
    {L"1010111010111010111010",											L"مفعول مفاعیل مفاعیل فعولن"},
    {L"1010101010111010111010",											L"مفعولن مفعول مفاعیل فعولن"},
    {L"1010111010101010111010",											L"مفعول مفاعیلن مفعول فعولن"},
    {L"1010111010111010101010",											L"مفعول مفاعیل مفاعیلن فعلن"},
    {L"110101011010101101010",											L"مفاعیلن مفاعیلن مفاعیلن"},
    {L"1101010110101011010",												L"مفاعیلن مفاعیلن فعولن"},
    {L"1010111011011010",													L"مفعول مفاعلن فعولن"},
    {L"1010101011011010",													L"مفعولن فاعلن فعولن"},
    {L"1011011011010110110110",											L"فاعلن مفاعلن فاعلن مفاعلن"},
    {L"110101011010",														L"مفاعیلن فعولن"},
    {L"110101011010110101011010",										L"مفاعیلن فعولن مفاعیلن فعولن"},
    {L"1010110101011010101101010110",									L"مستفعلن مستفعلن مستفعلن مستفعلن"},
    {L"101110101110101110101110",										L"مفتَعِلن مفتَعِلن مفتَعِلن مفتَعِلن"},
    {L"101110110110101110110110",										L"مفتَعِلن مفاعلن مفتَعِلن مفاعلن"},
    {L"110110101110110110101110",										L"مفاعلن مفتَعِلن مفاعلن مفتَعِلن"},
    {L"101011010101101010110",											L"مستفعلن مستفعلن مستفعلن"},
    {L"101110101110101110",												L"مفتَعِلن مفتَعِلن مفتَعِلن"},
    {L"1011010101101010110101011010",									L" فاعلاتن فاعلاتن فاعلاتن فاعلاتن"},
    {L"10110101011010101101010110",										L"فاعلاتن فاعلاتن فاعلاتن فاعلن"},
    {L"10110101110101110101110",											L"فاعلاتن فعِلاتن فعِلاتن فعِلن"},
    {L"1110101110101110101110",											L"فعِلاتن فعِلاتن فعِلاتن فعِلن"},
    {L"10110101110101110101010",											L"فاعلاتن فعِلاتن فعِلاتن فعلن"},
    {L"1110101110101110101010",											L"فعِلاتن فعِلاتن فعِلاتن فعلن"},
    {L"111011011010111011011010",										L"فعِلات فاعلاتن فعِلات فاعلاتن"},
    {L"111010111010111010111010",										L"فعِلاتن فعِلاتن فعِلاتن فعِلاتن"},
    {L"101101010110101011010",											L"فاعلاتن فاعلاتن فاعلاتن"},
    {L"1011010101101010110",												L"فاعلاتن فاعلاتن فاعلن"},
    {L"10110101110101110",												L"فاعلاتن فعِلاتن فعِلن"},
    {L"10110101110101010",												L"فاعلاتن فعِلاتن فعلن"},
    {L"1110101110101110",													L"فعِلاتن فعِلاتن فعِلن"},
    {L"1110101110101010",													L"فعِلاتن فعِلاتن فعِلن"},
    {L"111010111010111010",												L"فعِلاتن  فعِلاتن فعِلاتن"},
    {L"11010110101101011010",											L"فعولن فعولن فعولن فعولن"},
    {L"1101011010110101101011010110101101011010",					L"فعولن فعولن فعولن فعولن فعولن فعولن فعولن فعولن"},
    {L"110101101011010110",												L"فعولن فعولن فعولن فعَل"},
    {L"1011101110111010",												L"فاع فعول فعول فعولن"},
    {L"10111011101110",													L"فاع فعول فعول فعَل"},
    {L"101110111011101110111011101110",								L"فاع فعول فعول فعول فعول فعول فعول فعَل"},
    {L"10111011101110111011101110111010",							L"فاع فعول فعول فعول فعول فعول فعول فعولن"},
    {L"110101101011010",													L"فعولن فعولن فعولن"},
    {L"1101011010110",													L"فعولن فعولن فعَل"},
    {L"101011010101011010",												L"فعلن فعولن فعلن فعولن"},
    {L"10110101101011010110",											L"فاعلن فاعلن فاعلن فاعلن"},
    {L"1110111011101110",												L"فعِلن فعِلن فعِلن فعِلن"},
    {L"11101110111011101110111011101110",							L"فعِلن فعِلن فعِلن فعِلن فعِلن فعِلن فعِلن فعِلن"},
    {L"1011010110101101110",												L"فاعلن فاعلن فاعلن فعِلن"},
    {L"101101011010110",													L"فاعلن فاعلن فاعلن"},
    {L"1011011010110110",												L"فاعلن فعَل فاعلن فعَل"},
    {L"1110110111011011101101110110",									L"متفاعلن متفاعلن متفاعلن متفاعلن"},
    {L"111011011101101110110",											L"متفاعلن متفاعلن متفاعلن"},
    {L"1101110110111011011101101110",									L"مفاعلتن مفاعلتن مفاعلتن مفاعلتن"},
    {L"110111011011101101110",											L"مفاعلتن مفاعلتن مفاعلتن"},
    {L"1101110110111011010",												L"مفاعلتن مفاعلتن فعولن"},
    //{L"1101010/1011010/1101010/1011010",									L"مفاعیلن فاعلاتن مفاعیلن فاعلاتن"},//
   // {L"110101/101101/110101/10110",											L"مضارع مثمن مکفوف محذوف"},//
    {L"101011011010101011011010",										L"مفعول فاعلاتن مفعول فاعلاتن"},
    {L"1010110110111010110110",											L"مفعول فاعلات مفاعیل فاعلن"},
    {L"1010110110101010110110",											L"مفعول فاعلاتن مفعول فاعلن"},
    //{L"10101/101101/1101010",												L"مضارع مسدس اخرب مکفوف"},//
    //{L"1010110/1011010/1010110/1011010",									L"مستفعلن فاعلاتن مستفعلن فاعلاتن"},//
    {L"110110111010110110111010",										L"مفاعلن فعِلاتن مفاعلن فعِلاتن"},
    {L"110110101010110110111010",										L"مفاعلن مفعولن مفاعلن فعِلاتن"},
   // {L"110110/111010/110110/101010",										L"مفاعلن فعِلاتن مفاعلن مفعولن"},
 //   {L"110110/101010/110110/101010",										L"مجتث مثمن مخبون"},
    {L"1101101110101101101110",											L"مفاعلن فعِلاتن مفاعلن فعِلن"},
 //   {L"110110/101010/110110/1110",											L"مجتث مثمن مخبون محذوف"},
    {L"1101101110101101101010",											L"مفاعلن فعِلاتن مفاعلن فعلن"},
    //{L"110110/101010/110110/1010",											L"مجتث مثمن مخبون محذوف مسکن"},//
    {L"110110/111010/110110",												L"مفاعلن فعِلاتن مفاعلن"},
    //{L"110110/101010/110110",												L"مجتث مسدس مخبون"},
    //{L"1010110/1010101/1010110/1010101",									L"مستفعلن مفعولاتُ مستفعلان مفعولاتُ"},
    {L"1011101011010111010110",											L"مفتعِلن فاعلن مفتعِلن فاعلن"},
    {L"10111010110110111010",											L"مفتعِلن فاعلات مفتعِلن فع"},
    {L"10111010110101110",												L"مفتعِلن فاعلن مفتعِلن"},
 //   {L"1010101/1010110/1010101/1010110",									L"مفعولاتُ مستفعلن مفتعولات مستفعلن"},
    {L"101101101110101101101110",										L"فاعلات مفتعِلن فاعلات مفتعِلن"},
   // {L"1010110/1010110/1010101",											L"مستفعلن مستفعلن مفعولات"},
    {L"10111010111010110",												L"مفتعِلن مفتعِلن فاعلن"},
 //   {L"1010110/1010110/11010",												L"مستفعلن مستفعلن فعولن"},
   // {L"1011010/1010110/1011010/1010110",									L"فاعلاتن مستفعلن فاعلاتن مستفعلن"},
  //  {L"1011010/1010110/1011010",											L"فاعلاتن مستفعلن فاعلاتن"},
 //   {L"111010/110110/111010",												L"خفیف مسدس مخبون"},
    {L"10110101101101110",												L"فاعلاتن مفاعلن فعِلن"},
    {L"1110101101101110",													L"فعِلاتن مفاعلن فعِلن"},
    {L"10110101101101010",												L"فاعلاتن مفاعلن فعلن"},
    {L"1110101101101010",													L"فعِلاتن مفاعلن فعلن"},
   {L"101101011011010",													L"فاعلاتن مفاعلن فع"},
    {L"11101011011010",													L"فعِلاتن مفاعلن فع"},
   // {L"1101010/11010/1101010",												L"مفاعیلن فعولن مفاعیلن"},
    {L"11010110101011010110110",											L"فعولن مفاعیلن فعولن مفاعیلن"},
  //  {L"11010/110110/11010/110110",											L"طویل مثمن مقبوض"},
    {L"101101010110101101010110",										L"فاعلاتن فاعلن فاعلاتن فاعلن"},
    {L"11101011101110101110",											L"فعِلاتن فعِلن فعِلاتن فعِلن"},
    {L"11101010101110101110",											L"فعلاتن فعلن فعلاتن فعلن"},
    //{L"101010/1110/111010/1110",											L"مدید مثمن مخبون"},
    //{L"111010/1110/101010/1110",											L"مدید مثمن مخبون"},
    //{L"111010/1110/111010/1010",											L"مدید مثمن مخبون"},
    {L"1011010111010110101110",											L"فاعلاتن فعِلن فاعلاتن فعِلن"},
    //{L"1010110/10110/1010110/10110",										L"مستفعلن فاعلن مستفعلن فاعلن"},
    //{L"110110/1110/110110/1110",											L"بسیط مثمن مخبون"},
    //{L"1101010/1101010/1011010",											L"مفاعیلن مفاعیلن فاعلاتن"},
   // {L"10101/110101/1011010",												L"قریب مسدس اخرب مکفوف"},
    //{L"1011010/1011010/1010110",											L"فاعلاتن فاعلاتن مستفعلن"},
   // {L"111010/111010/110110",												L"جدید مسدس مخبون"},
    //{L"1011010/1101010/1101010",											L"فاعلاتن مفاعیلن مفاعیلن"},
    {L"10110111010111010",												L"فاعلات مفاعیل فعولن"},
    {L"11011010110110101101101011011010",								L"فعول فعلن فعول فعلن فعول فعلن فعول فعلن"},
    {L"101101101010",														L"فاعلن مفاعیلن"},
    {L"10110110110",														L"فاعلن مفاعلن"},
    {L"11010101101010",													L"مفاعیلن مفاعیلن"},
    {L"110110110110110110110",											L"مفاعلن مفاعلن مفاعلن فعَل"},
    {L"1011010111010111010",												L"فاعلاتن فعِلاتن فعِلاتن"},
    {L"10110101011010",													L"فاعلاتن فاعلاتن"},
    {L"101101010110",														L"فاعلاتن فاعلن"},
    {L"1101011010",														L"فعولن فعولن"},
    {L"11101101110110",													L"متفاعلن متفاعلن"},
 //   {L"11010/1101010",														L"طویل مربع سالم"},
 //   {L"1011010/10110",														L"مدید مربع سالم"},
    {L"11010101101010110101011010101101010110101011010101101010",	L"مفاعیلن مفاعیلن مفاعیلن مفاعیلن مفاعیلن مفاعیلن مفاعیلن مفاعیلن"},
    {L"1101101011011010",													L"فعول فعلن" },
};



const std::unordered_map<std::wstring, const std::wstring> Arkan_map
{
    {L"101010",    L"مفعولن"},
    {L"1010110",   L"مستفعلن"},
    {L"10101",	   L"مفعول"},
    {L"1010",	   L"فِعْلن"},
    {L"1011010",   L"فاعلاتن"},
    {L"101101",	   L"فاعلاتُ"},
    {L"10110",	   L"فاعلن"},
    {L"101110",	   L"مفتَعِلن"},
    {L"101",	   L"فاع"},
    {L"10",        L"فِع"},
    {L"1101010",   L"مفاعیلن"},
    {L"110101",	   L"مفاعیل"},
    {L"11010",	   L"فعولن"},
    {L"110110",	   L"مفاعلن"},
    {L"1101",	   L"فعول"},
    {L"110",	   L"فَعَل"},
    {L"111010",	   L"فَعِلاتن"},
    {L"1110110",   L"متَفاعلن"},
    {L"11101",	   L"فَعِلات"},
    {L"1110",	   L"فَعِلن"},
    {L"11011010",  L"مَفاعلاتن"},
    {L"1010101",   L"مفعولاتُ"},
    {L"1101110",   L"مفاعِلَتن"},
    {L"10101101",  L"مستفعلان"},
    {L"10110101",  L"فاعلاتان"},
    {L"1011101",   L"مفتَعِلان"},
    {L"11010101",  L"مفاعیلان"},
    {L"1101101",   L"مفاعلان"},
    {L"1110101",   L"فَعِلاتان"},
    {L"11101101",  L"متَفاعلان"},
    {L"110110101", L"مَفاعلاتان"},
    {L"11011101",  L"مفاعِلَتان"},
    {L"1",  L"فِ"},
    {L"11",  L"فِعِ"},
};

const std::unordered_set<wchar_t> AllowedFirstLetter_set
{
    L'آ',
    L'ا',
    L'ب',
    L'پ',
    L'ت',
    L'ٹ',
    L'ث',
    L'ج',
    L'چ',
    L'ح',
    L'خ',
    L'د',
    L'ڈ',
    L'ذ',
    L'ر',
    L'ز',
    L'ژ',
    L'س',
    L'ش',
    L'ص',
    L'ض',
    L'ط',
    L'ظ',
    L'ع',
    L'غ',
    L'ف',
    L'ق',
    L'ک',
    L'گ',
    L'ل',
    L'م',
    L'ن',
    L'و',
    L'ہ',
    L'ی',
    L'ے'
};

const std::unordered_map<std::wstring, const std::wstring> Names_map
{
    {L"1101010110101011010101101010",									L"ہزج مثمن سالم"},
    {L"11010101101010110101011010",										L"ہزج مثمن محذوف"},
    {L"110110110110110110110110",										L"ہزج مثمن مقبوض"},
    {L"101101101010101101101010",										L"ہزج مثمن اشتر"},
    {L"11011011010101101101101010",										L"ہزج مثمن مقبوض سالم"},
    {L"101011101011101011101010",										L"ہزج مثمن اخرب مکفوف سالم"},
    {L"101011101010101011101010",										L"ہزج مثمن اخرب سالم"},
    {L"1010111010111010111010",											L"ہزج مثمن اخرب مکفوف محذوف"},
    {L"1010101010111010111010",											L"ہزج مثمن اخرب مکفوف محذوف"},
    {L"1010111010101010111010",											L"ہزج مثمن اخرب مکفوف محذوف"},
    {L"1010111010111010101010",											L"ہزج مثمن اخرب مکفوف محذوف"},
    {L"110101011010101101010",											L"ہزج مسدس سالم"},
    {L"1101010110101011010",												L"ہزج مسدس محذوف"},
    {L"1010111011011010",													L"ہزج مسدس اخرب مقبوض محذوف"},
    {L"1010101011011010",													L"ہزج مسدس اخرم اشتر محذوف"},
    {L"1011011011010110110110",											L"ہزج مربع اشتر مقبوض مضاعف"},
    {L"110101011010",														L"ہزج مربع محذوف"},
    {L"110101011010110101011010",										L"ہزج مربع محذوف مضاعف"},
    {L"1010110101011010101101010110",									L"رجز مثمن سالم"},
    {L"101110101110101110101110",										L"رجز مثمن مطوی"},
    {L"101110110110101110110110",										L"رجز مثمن مطوی مخبون"},
    {L"110110101110110110101110",										L"رجز مثمن مخبون مطوی"},
    {L"101011010101101010110",											L"رجز مسدس سالم"},
    {L"101110101110101110",												L"رجز مسدس مطوی"},
    {L"1011010101101010110101011010",									L"رمل مثمن سالم"},
    {L"10110101011010101101010110",										L"رمل مثمن محذوف"},
    {L"10110101110101110101110",											L"رمل مثمن سالم مخبون محذوف"},
    {L"1110101110101110101110",											L"رمل مثمن سالم مخبون محذوف"},
    {L"10110101110101110101010",											L"رمل مثمن مخبون محذوف مقطوع"},
    {L"1110101110101110101010",											L"رمل مثمن مخبون محذوف مقطوع"},
    {L"111011011010111011011010",										L"رمل مثمن مشکول"},
    {L"101011011010101011011010",										L"رمل مثمن مشکول مسکّن"},
    {L"111010111010111010111010",										L"رمل مثمن مخبون"},
    {L"101101010110101011010",											L"رمل مسدس سالم"},
    {L"1011010101101010110",												L"رمل مسدس محذوف"},
    {L"10110101110101110",												L"رمل مسدس مخبون محذوف"},
    {L"10110101110101010",												L"رمل مسدس مخبون محذوف مسکن"},
    {L"1110101110101110",													L"رمل مسدس مخبون محذوف"},
    {L"1110101110101010",													L"رمل مسدس مخبون محذوف مسکن"},
    {L"111010111010111010",												L"رمل مسدس مخبون"},
    {L"11010110101101011010",											L"متقارب مثمن سالم"},
    {L"1101011010110101101011010110101101011010",					L"متقارب مثمن سالم مضاعف"},
    {L"110101101011010110",												L"متقارب مثمن محذوف"},
    {L"1011101110111010",												L"متقارب مثمن اثرم مقبوض"},
    {L"10111011101110",													L"متقارب مثمن اثرم مقبوض محذوف"},
    {L"101110111011101110111011101110",								L"متقارب مثمن اثرم مقبوض مضاعف"},
    {L"10111011101110111011101110111010",							L"متقارب مثمن اثرم مقبوض محذوف مضاعف"},
    {L"110101101011010",													L"متقارب مسدس سالم"},
    {L"1101011010110",													L"متقارب مسدس محذوف"},
    {L"101011010101011010",												L"متقارب مربع اثلم سالم مضاعف"},
    {L"10110101101011010110",											L"متدارک مثمن سالم"},
    {L"1110111011101110",												L"متدارک مثمن مخبون"},
    {L"11101110111011101110111011101110",							L"متدارک مثمن مخبون مضاعف"},
    {L"1011010110101101110",												L"متدارک مثمن سالم مقطوع"},
    {L"101101011010110",													L"متدارک مسدس سالم"},
    {L"1011011010110110",												L"متدارک مربع مخلع مضاعف"},
    {L"1110110111011011101101110110",									L"کامل مثمن سالم"},
    {L"111011011101101110110",											L"کامل مسدس سالم"},
    {L"1101110110111011011101101110",									L"وافر مثمن سالم"},
    {L"110111011011101101110",											L"وافر مسدس سالم"},
    {L"1101110110111011010",												L"وافر مسدس مقطوف"},
    {L"1101010101101011010101011010",									L"مضارع مثمن سالم"},
    {L"11010110110111010110110",											L"مضارع مثمن مکفوف محذوف"},
    {L"101011011010101011011010",										L"مضارع مثمن اخرب"},
    {L"1010110110111010110110",											L"مضارع مثمن اخرب مکفوف محذوف"},
    {L"1010110110101010110110",											L"مضارع مثمن اخرب محذوف"},
    {L"101011011011101010",												L"مضارع مسدس اخرب مکفوف"},
    {L"1010110101101010101101011010",									L"مجتث مثمن سالم"},
    {L"110110111010110110111010",										L"مجتث مثمن مخبون"},
    {L"110110101010110110111010",										L"مجتث مثمن مخبون"},
    {L"110110111010110110101010",										L"مجتث مثمن مخبون"},
    {L"110110101010110110101010",										L"مجتث مثمن مخبون"},
    {L"1101101110101101101110",											L"مجتث مثمن مخبون محذوف"},
    {L"1101101010101101101110",											L"مجتث مثمن مخبون محذوف"},
    {L"1101101110101101101010",											L"مجتث مثمن مخبون محذوف مسکن"},
    {L"1101101010101101101010",											L"مجتث مثمن مخبون محذوف مسکن"},
    {L"110110111010110110",												L"مجتث مسدس مخبون"},
    {L"110110101010110110",												L"مجتث مسدس مخبون"},
    {L"1010110101010110101101010101",									L"منسرح مثمن سالم"},
    {L"1011101011010111010110",											L"منسرح مثمن مطوی مکسوف"},
    {L"10111010110110111010",											L"منسرح مثمن مطوی منحور"},
    {L"10111010110101110",												L"منسرح مسدس مطوی مکسوف"},
    {L"1010101101011010101011010110",									L"مقتضب مثمن سالم"},
    {L"101101101110101101101110",										L"مقتضب مثمن مطوی"},
    {L"101011010101101010101",											L"سریع مسدس سالم"},
    {L"10111010111010110",												L"سریع مسدس مطوی مکسوف"},
    {L"1010110101011011010",												L"سریع مسدس مخبون مکسوفL"},
    {L"1011010101011010110101010110",									L"خفیف مثمن سالم"},
    {L"101101010101101011010",											L"خفیف مسدس سالم"},
    {L"111010110110111010",												L"خفیف مسدس مخبون"},
    {L"10110101101101110",												L"خفیف مسدس مخبون محذوف"},
    {L"1110101101101110",													L"خفیف مسدس مخبون محذوف"},
    {L"10110101101101010",												L"خفیف مسدس مخبون محذوف مقطوع"},
    {L"1110101101101010",													L"خفیف مسدس مخبون محذوف مقطوع"},
    {L"101101011011010",													L"خفیف مسدس سالم مخبون محجوف"},
    {L"11101011011010",													L"خفیف مسدس مخبون محجوف"},
    {L"1101010110101101010",												L"طویل مثمن سالم"},
    {L"11010110101011010110110",											L"طویل مثمن سالم مقبوض"},
    {L"1101011011011010110110",											L"طویل مثمن مقبوض"},
    {L"101101010110101101010110",										L"مدید مثمن سالم"},
    {L"11101011101110101110",											L"مدید مثمن مخبون"},
    {L"11101010101110101110",											L"مدید مثمن مخبون"},
    {L"10101011101110101110",											L"مدید مثمن مخبون"},
    {L"11101011101010101110",											L"مدید مثمن مخبون"},
    {L"11101011101110101010",											L"مدید مثمن مخبون"},
    {L"1011010111010110101110",											L"مدید مثمن سالم مخبون"},
    {L"101011010110101011010110",										L"بسیط مثمن سالم"},
    {L"11011011101101101110",											L"بسیط مثمن مخبون"},
    {L"110101011010101011010",											L"قریب مسدس سالم"},
    {L"101011101011011010",												L"قریب مسدس اخرب مکفوف"},
    {L"101101010110101010110",											L"جدید مسدس سالم"},
    {L"111010111010110110",												L"جدید مسدس مخبون"},
    {L"101101011010101101010",											L"مشاکل مسدس سالم"},
    {L"10110111010111010",												L"مشاکل مسدس مکفوف محذوف"},
    {L"11011010110110101101101011011010",								L"جمیل مثمن سالم"},
    {L"101101101010",														L"ہزج مربع اشتر"},
    {L"10110110110",														L"ہزج مربع اشتر مقبوض"},
    {L"11010101101010",													L"ہزج مربع سالم"},
    {L"110110110110110110110",											L"ہزج مثمن مقبوض محذوف"},
    {L"1011010111010111010",												L"رمل مسدس مخبون"},
    {L"11010101101010",													L"ہزج مربع سالم"},
    {L"10110101011010",													L"رمل مربع سالم"},
    {L"101101010110",														L"ہزج مربع محذوف"},
    {L"1101011010",														L"متقارب مربع سالم"},
    {L"11101101110110",													L"کامل مربع سالم"},
    {L"110101101010",														L"طویل مربع سالم"},
    {L"101101010110",														L"مدید مربع سالم"},
    {L"11010101101010110101011010101101010110101011010101101010",	L"ہزج مثمن سالم مضاعف"},
    {L"1101101011011010",													L"جمیل مربع سالم" },
    {L"10101010101010101010101010101010" , L"بحرِ زمزمہ"},
    {L"10101010101010101010101010101110" , L"بحرِ زمزمہ"},
    {L"10101010101010101010101011101010" , L"بحرِ زمزمہ"},
    {L"10101010101010101010101011101110" , L"بحرِ زمزمہ"},
    {L"10101010101010101010111010101010" , L"بحرِ زمزمہ"},
    {L"10101010101010101010111011101110" , L"بحرِ زمزمہ"},
    {L"10101010101010101110101010101010" , L"بحرِ زمزمہ"},
    {L"10101010101010101110111011101110" , L"بحرِ زمزمہ"},
    {L"10101010101011101010101010101010" , L"بحرِ زمزمہ"},
    {L"10101010101011101110111011101110" , L"بحرِ زمزمہ"},
    {L"10101010111010101010101010101010" , L"بحرِ زمزمہ"},
    {L"10101010111011101110111011101110" , L"بحرِ زمزمہ"},
    {L"10101110101010101010101010101010" , L"بحرِ زمزمہ"},
    {L"10101110111011101110111011101110" , L"بحرِ زمزمہ"},
    {L"11101010101010101010101010101010" , L"بحرِ زمزمہ"},
    {L"11101010101010101010101010101110" , L"بحرِ زمزمہ"},
    {L"11101010101010101010101011101110" , L"بحرِ زمزمہ"},
    {L"11101010101010101010111011101110" , L"بحرِ زمزمہ"},
    {L"11101010101010101110111011101110" , L"بحرِ زمزمہ"},
    {L"11101010101011101110111011101110" , L"بحرِ زمزمہ"},
    {L"11101010111011101110111011101110" , L"بحرِ زمزمہ"},
    {L"11101110101010101010101010101010" , L"بحرِ زمزمہ"},
    {L"11101110101010101010101010101110" , L"بحرِ زمزمہ"},
    {L"11101110101010101010101011101110" , L"بحرِ زمزمہ"},
    {L"11101110101010101010111011101110" , L"بحرِ زمزمہ"},
    {L"11101110101010101110111011101110" , L"بحرِ زمزمہ"},
    {L"11101110101011101110111011101110" , L"بحرِ زمزمہ"},
    {L"11101110111010101010101010101010" , L"بحرِ زمزمہ"},
    {L"11101110111010101010101010101110" , L"بحرِ زمزمہ"},
    {L"11101110111010101010101011101110" , L"بحرِ زمزمہ"},
    {L"11101110111010101010111011101110" , L"بحرِ زمزمہ"},
    {L"11101110111010101110111011101110" , L"بحرِ زمزمہ"},
    {L"11101110111011101010101010101010" , L"بحرِ زمزمہ"},
    {L"11101110111011101010101010101110" , L"بحرِ زمزمہ"},
    {L"11101110111011101010101011101110" , L"بحرِ زمزمہ"},
    {L"11101110111011101010111011101110" , L"بحرِ زمزمہ"},
    {L"11101110111011101110101010101010" , L"بحرِ زمزمہ"},
    {L"11101110111011101110101010101110" , L"بحرِ زمزمہ"},
    {L"11101110111011101110101011101110" , L"بحرِ زمزمہ"},
    {L"11101110111011101110111010101010" , L"بحرِ زمزمہ"},
    {L"11101110111011101110111010101110" , L"بحرِ زمزمہ"},
    {L"11101110111011101110111011101010" , L"بحرِ زمزمہ"}
};
