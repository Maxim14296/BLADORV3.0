#pragma once
#include "libxl.h"
#include <string>

using namespace libxl;

constexpr auto COLS = 10;

class XL
{
	Book* wb = nullptr;
	Sheet* ws = nullptr;

	/// <summary>
	/// ¬ычисление индекса листа по его имени
	/// </summary>
	/// <param name="sheet_name">им€ листа</param>
	/// <returns>индекс</returns>
	int get_sheet_index(std::string& sheet_name)
	{
		int sheet_index = 0;
		const wchar_t* wc_sheet_name = str_to_wchar_t(sheet_name);
		int sheet_count = wb->sheetCount(); // не вычисл€етс€ кол-во листов
		for (; sheet_index < sheet_count; sheet_index++)
		{
			if (wb->getSheetName(sheet_index) == wc_sheet_name) break;
		}
		return sheet_index;
	}

	/// <summary>
	/// ѕреобразование строки string в строку const wchar_t*
	/// </summary>
	/// <param name="str">строка string</param>
	/// <returns>строка const wchar_t*</returns>
	const wchar_t* str_to_wchar_t(std::string& str)
	{
		std::wstring w_str = std::wstring(str.begin(), str.end());
		const wchar_t* wc_str = w_str.c_str();
		return wc_str;
	}

	/// <summary>
	/// ѕреобразование строки const wchar_t* в строку string
	/// </summary>
	/// <param name="wc_str">строка const wchar_t*</param>
	/// <returns>строка string</returns>
	std::string wchar_t_to_str(const wchar_t* wc_str)
	{
		std::wstring wstr = wc_str;
		std::string str(wstr.begin(), wstr.end());
		return str;
	}

public:
	int table_raws = 0;
	int table_cols = 0;

	XL(std::string file_name)
	{
		const wchar_t* wc_file_name = str_to_wchar_t(file_name);
		wb = xlCreateXMLBook(); // .xlsx
		wb->load(wc_file_name);
	};

	/// <summary>
	/// «апись таблицы в массив строк
	/// </summary>
	/// <param name="array">массив</param>
	/// <param name="sheet_name">им€ листа</param>
	void write2array(float** array, std::string& sheet_name)
	{
		if (wb != NULL)
		{
			int sheet_index = get_sheet_index(sheet_name);
			ws = wb->getSheet(sheet_index);
			for (short raw = 0; raw < table_raws; raw++)
			{
				for (short col = 0; col < table_cols; col++)
				{
					array[raw][col] = ws->readNum(raw, col);
				}
			}
		}
	}

	/// <summary>
	/// «апись таблицы в массив строк
	/// </summary>
	/// <param name="array">массив</param>
	/// <param name="sheet_num">номер листа (начинаетс€ с 0!)</param>
	void write2array(std::string** array, std::string& sheet_name)
	{
		if (wb != NULL)
		{
			int sheet_index = get_sheet_index(sheet_name);
			ws = wb->getSheet(sheet_index);
			if (table_cols != COLS) array[table_raws][table_cols];
			for (short raw = 0; raw < table_raws; raw++)
			{
				for (short col = 0; col < table_cols; col++)
				{
					std::string str = wchar_t_to_str(ws->readStr(raw, col));
					array[raw][col] = str;
				}
			}
		}
	}

	/// <summary>
	/// ќсвобождение ресурсов объекта
	/// </summary>
	void release()
	{
		if(wb != nullptr) wb->release();
		wb = nullptr;
		ws = nullptr;
		table_raws = 0;
		table_cols = 0;
	}

	/// <summary>
	/// ¬ычисление размера таблицы
	/// </summary>
	/// <param name="sheet_num">номер листа (начинаетс€ с 0!)</param>
	void find_table_size(std::string& sheet_name)
	{
		int sheet_num = get_sheet_index(sheet_name);
		if (wb != NULL)
		{
			ws = wb->getSheet(sheet_num); // ws = NULL
			table_raws = ws->lastFilledRow();// нумераци€ начинаетс€ с 1! Ќарушение доступа чтени€
			table_cols = ws->lastFilledCol();// нумераци€ начинаетс€ с 1!
		}
		else
		{
			Book* wb = xlCreateXMLBook();
			Sheet* ws = wb->getSheet(sheet_num);
			table_raws = ws->lastFilledRow();// нумераци€ начинаетс€ с 1!
			table_cols = ws->lastFilledCol();// нумераци€ начинаетс€ с 1!
		}
	}
};

