// BLADORV3.0.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
// Расчетный модуль. Расчет только для одной скорости полета

#include <iostream>
#include <fstream>
#include <string>
#include "XL.h"

constexpr auto PI = 3.14;

std::string CyDB = "CyDB.xlsx";
std::string CxDB = "CxDB.xlsx";
std::string BladesDB = "Blades.xlsx";

XL Cy_xl(CyDB);
XL Cx_xl(CxDB);
XL Blades_xl(BladesDB);

const wchar_t* str_to_wchar_t(std::string& str)
{
    std::wstring w_str = std::wstring(str.begin(), str.end());
    const wchar_t* wc_str = w_str.c_str();
    return wc_str;
}

std::string wchar_t_to_str(const wchar_t* wc_str)
{
    std::wstring wstr = wc_str;
    std::string str(wstr.begin(), wstr.end());
    return str;
}

/// <summary>
/// Создание двумерного массива строк string
/// </summary>
/// <param name="raws">количество строк</param>
/// <param name="cols">количество столбцов</param>
/// <returns>указатель на массив</returns>
std::string** create_2dstr_array(int& raws, int& cols)
{
    std::string** array = new std::string * [raws];
    for (int i = 0; i < raws; i++)
    {
        array[i] = new std::string[cols];
    }
    return array;
}

/// <summary>
/// Создание двумерного массива дробных чисел
/// </summary>
/// <param name="raws">количество строк</param>
/// <param name="cols">количество столбцов</param>
/// <returns>указатель на массив</returns>
float** create_2dflt_array(int& raws, int& cols)
{
    float** array = new float* [raws];
    for (int i = 0; i < raws; i++)
    {
        array[i] = new float[cols];
    }
    return array;
}

/// <summary>
/// Удаление двумерного массива
/// </summary>
/// <param name="array">массив</param>
/// <param name="raws">количество строк</param>
void release_2d_array(float** array, int& raws)
{
    for (int i = 0; i < raws; i++)
    {
        delete[] array[i];
    }
}

/// <summary>
/// Удаление двумерного массива
/// </summary>
/// <param name="array">массив</param>
/// <param name="raws">количество строк</param>
void release_2d_array(std::string** array, int& raws)
{
    for (int i = 0; i < raws; i++)
    {
        delete[] array[i];
    }
}

/// <summary>
/// Вычисление суммы элементов массива
/// </summary>
/// <param name="aray">массив</param>
/// <param name="arr_size">размер массива</param>
/// <returns>сумма элементов</returns>
float array_sum(float* aray, int& arr_size)
{
    float sum = 0;
    for (int i = 0; i < arr_size; i++)
    {
        sum += aray[i];
    }
    return sum;
}

/// <summary>
/// Округление до указанного знака
/// </summary>
/// <param name="value">округляемое значение</param>
/// <param name="count_symbs">количество знаков</param>
/// <returns>округленное значение</returns>
float round_to(float value, int count_symbs)
{
    float rnd_value = roundf(value * pow(10, count_symbs)) / pow(10, count_symbs);
    return rnd_value;
}

/// <summary>
/// Вычисление коэфициента c_y профиля по заданным относительной толщине и угле атаки
/// </summary>
/// <param name="c_rel">относительная толщина профиля</param>
/// <param name="alfa">угол атаки профиля</param>
/// <param name="Cy">массив коэффициентов c_y профиля</param>
/// <param name="profile_name">имя профиля</param>
/// <returns>коэфициент c_y профиля</returns>
float find_prof_Cy(float& c_rel, float& alfa, float** Cy, std::string& profile_name)
{
    float prof_Cy = 0;
    if (profile_name != "Втулка")
    {
        short i = 1;
        short j = 1;
        float c1 = 0;
        float c2 = 0;
        float alfa1 = 0;
        float alfa2 = 0;
        XL xl(CyDB);

        if (alfa > Cy[xl.table_raws - 1][0] && alfa < Cy[1][0]) // Интерполяция alfa
        {
            if (c_rel > Cy[0][xl.table_cols - 1] && c_rel < Cy[0][1]) // Интерполяция c_rel
            {
                for (; j < xl.table_cols; j++)
                {
                    if (c_rel >= Cy[0][j])
                    {
                        c1 = Cy[0][j];
                        c2 = Cy[0][j - 1];
                        break;
                    }
                }
                for (; i < xl.table_raws; i++)
                {
                    if (alfa >= Cy[i][0])
                    {
                        alfa1 = Cy[i][0];
                        alfa2 = Cy[i - 1][0];
                        break;
                    }
                }
                float Cy_c1_alfa = (alfa - alfa1) * (Cy[i - 1][j] - Cy[i][j]) / (alfa2 - alfa1) + Cy[i][j];
                float Cy_c2_alfa = (alfa - alfa1) * (Cy[i - 1][j - 1] - Cy[i][j - 1]) / (alfa2 - alfa1) + Cy[i][j - 1];
                prof_Cy = (c_rel - c1) * (Cy_c2_alfa - Cy_c1_alfa) / (c2 - c1) + Cy_c1_alfa;
            }
            if (c_rel >= Cy[0][1]) //Экстраполяция справа c_rel
            {
                for (; i < xl.table_raws; i++)
                {
                    if (alfa >= Cy[i][0])
                    {
                        alfa1 = Cy[i][0];
                        alfa2 = Cy[i - 1][0];
                        break;
                    }
                }
                c1 = Cy[0][2];
                c2 = Cy[0][1];
                float Cy_c_alfa1 = (Cy[i][1] - Cy[i][2]) * (c_rel - c1) / (c2 - c1) + Cy[i][2];
                float Cy_c_alfa2 = (Cy[i - 1][1] - Cy[i - 1][2]) * (c_rel - c1) / (c2 - c1) + Cy[i - 1][2];
                prof_Cy = (alfa - alfa1) * (Cy_c_alfa2 - Cy_c_alfa1) / (alfa2 - alfa1) + Cy_c_alfa1;
            }
            if (c_rel <= Cy[0][xl.table_cols - 1]) // Экстраполяция слева c_rel
            {
                for (; i < xl.table_raws; i++)
                {
                    if (alfa >= Cy[i][0])
                    {
                        alfa1 = Cy[i][0];
                        alfa2 = Cy[i - 1][0];
                        break;
                    }
                }
                c1 = Cy[0][xl.table_cols - 1];
                c2 = Cy[0][xl.table_cols - 2];
                float Cy_c_alfa1 = Cy[i][xl.table_cols - 1] * (c2 - c_rel) / (c2 - c1) + Cy[i][xl.table_cols - 2] * (c_rel - c1) / (c2 - c1);
                float Cy_c_alfa2 = Cy[i][1, xl.table_cols - 1] * (c2 - c_rel) / (c2 - c1) + Cy[i - 1][xl.table_cols - 2] * (c_rel - c1) / (c2 - c1);
                prof_Cy = (alfa - alfa1) * (Cy_c_alfa2 - Cy_c_alfa1) / (alfa2 - alfa1) + Cy_c_alfa1;
            }
        }
        if (alfa >= Cy[1][0]) // Экстраполяция справа alfa
        {
            if (c_rel > Cy[0][xl.table_cols - 1] && c_rel < Cy[0][1]) // Интерполяция c_rel
            {
                for (; j < xl.table_cols; j++)
                {
                    if (c_rel >= Cy[0][j])
                    {
                        c1 = Cy[0][j];
                        c2 = Cy[0][j - 1];
                        break;
                    }
                }
                alfa1 = Cy[2][0];
                alfa2 = Cy[1][0];
                float Cy_c1_alfa = (Cy[1][j] - Cy[2][j]) * (alfa - alfa1) / (alfa2 - alfa1) + Cy[2][j];
                float Cy_c2_alfa = (Cy[1][j - 1] - Cy[2][j - 1]) * (alfa - alfa1) / (alfa2 - alfa1) + Cy[2][j - 1];
                prof_Cy = (c_rel - c1) * (Cy_c2_alfa - Cy_c1_alfa) / (c2 - c1) + Cy_c1_alfa;
            }
            if (c_rel >= Cy[0][1]) //Экстраполяция справа c_rel
            {
                c1 = Cy[0][2];
                c2 = Cy[0][1];
                alfa1 = Cy[2][0];
                alfa2 = Cy[1][0];
                float Cy_c1_alfa = (Cy[1][2] - Cy[2][2]) * (alfa - alfa1) / (alfa2 - alfa1) + Cy[2][2];
                float Cy_c2_alfa = (Cy[1, 1] - Cy[2, 1]) * (alfa - alfa1) / (alfa2 - alfa1) + Cy[2][1];
                prof_Cy = (Cy_c2_alfa - Cy_c1_alfa) * (c_rel - c1) / (c2 - c1) + Cy_c1_alfa;
            }
            if (c_rel <= Cy[0][xl.table_cols - 1]) // Экстраполяция слева c_rel
            {
                //alfa1 = Cy[2, 0];
                //alfa2 = Cy[1, 0];
                c1 = Cy[0][xl.table_cols - 1];
                c2 = Cy[0][xl.table_cols - 2];
                float Cy_c_alfa1 = Cy[i][xl.table_cols - 1] * (c2 - c_rel) / (c2 - c1) + Cy[i][xl.table_cols - 2] * (c_rel - c1) / (c2 - c1);
                float Cy_c_alfa2 = Cy[i - 1][xl.table_cols - 1] * (c2 - c_rel) / (c2 - c1) + Cy[i - 1][xl.table_cols - 2] * (c_rel - c1) / (c2 - c1);
                prof_Cy = (Cy_c_alfa2 - Cy_c_alfa1) * (c_rel - c1) / (c2 - c1) + Cy_c_alfa1;
            }
        }
        if (alfa <= Cy[xl.table_raws - 1][0]) // Экстраполяция слева alfa
        {
            if (c_rel > Cy[0][xl.table_cols - 1] && c_rel < Cy[0][1]) // Интерполяция c_rel
            {
                for (; j < xl.table_cols; j++)
                {
                    if (c_rel >= Cy[0][j])
                    {
                        c1 = Cy[0][j];
                        c2 = Cy[0][j - 1];
                        break;
                    }
                }
                alfa1 = Cy[xl.table_raws - 1][0];
                alfa2 = Cy[xl.table_raws - 2][0];
                float Cy_c1_alfa = Cy[xl.table_raws - 2][j] * (alfa2 - alfa) / (alfa2 - alfa1) + Cy[xl.table_raws - 1][j] * (alfa - alfa1) / (alfa2 - alfa1);
                float Cy_c2_alfa = Cy[xl.table_raws - 2][j - 1] * (alfa2 - alfa) / (alfa2 - alfa1) + Cy[xl.table_raws - 1][j - 1] * (alfa - alfa1) / (alfa2 - alfa1);
                prof_Cy = (c_rel - c1) * (Cy_c2_alfa - Cy_c1_alfa) / (c2 - c1) + Cy_c1_alfa;
            }
            if (c_rel >= Cy[0][1]) //Экстраполяция справа c_rel
            {
                alfa1 = Cy[xl.table_raws - 1][0];
                alfa2 = Cy[xl.table_raws - 2][0];
                c1 = Cy[0][2];
                c2 = Cy[0][1];
                float Cy_c1_alfa = Cy[xl.table_raws - 1][2] * (alfa2 - alfa) / (alfa2 - alfa1) + Cy[xl.table_raws - 2][2] * (alfa - alfa1) / (alfa2 - alfa1);
                float Cy_c2_alfa = Cy[xl.table_raws - 1][1] * (alfa2 - alfa) / (alfa2 - alfa1) + Cy[xl.table_raws - 2][1] * (alfa - alfa1) / (alfa2 - alfa1);
                prof_Cy = (Cy_c2_alfa - Cy_c1_alfa) * (c_rel - c1) / (c2 - c1) + Cy_c1_alfa;
            }
            if (c_rel <= Cy[0][xl.table_cols - 1]) // Экстраполяция слева c_rel
            {
                c1 = Cy[0][xl.table_cols - 1];
                c2 = Cy[0][xl.table_cols - 2];
                alfa1 = Cy[xl.table_raws - 1][0];
                alfa2 = Cy[xl.table_raws - 2][0];
                float Cy_c1_alfa = Cy[xl.table_raws - 1][xl.table_cols - 1] * (alfa2 - alfa) / (alfa2 - alfa1) + Cy[xl.table_raws - 2][xl.table_cols - 1] * (alfa - alfa1) / (alfa2 - alfa1);
                float Cy_c2_alfa = Cy[xl.table_raws - 1][xl.table_cols - 2] * (alfa2 - alfa) / (alfa2 - alfa1) + Cy[xl.table_raws - 2][xl.table_cols - 2] * (alfa - alfa1) / (alfa2 - alfa1);
                prof_Cy = Cy_c1_alfa * (c2 - c_rel) / (c2 - c1) + Cy_c2_alfa * (c_rel - c1) / (c2 - c1);
            }
        }
    }
    else prof_Cy = 0;

    return prof_Cy;
}

/// <summary>
/// Вычисление коэффициента c_y промежуточного профиля
/// </summary>
/// <param name="k">количество промежуточных профилей</param>
/// <param name="st_data">масив данных лопасти</param>
/// <param name="i">текущий индекс профиля</param>
/// <param name="new_Cy">массив коэффициентов c_y профилей</param>
/// <param name="alfa">угол атаки профиля</param>
/// <param name="Cy">массив коэффициентов c_y профиля</param>
/// <param name="filter_Cy"></param>
/// <param name="V0_i">скорость полета</param>
void calc_inter_prof_Cy(short& k, std::string** st_data, int& i, float** new_Cy, float& alfa, float** Cy, bool& filter_Cy, int& V0_i)
{
    int num_prof2 = std::stoi(st_data[k][0]);
    int num_prof1 = std::stoi(st_data[i - 1][0]);
    if (filter_Cy)
    {
        Cy_xl.write2array(Cy, st_data[k][7]); // array[k][7] - имя профиля
        filter_Cy = false;
    }
    float c_rel = std::stof(st_data[k][5]);
    st_data[k][8] = find_prof_Cy(c_rel, alfa, Cy, st_data[k][7]);
    for (int j = num_prof1; j < num_prof2 - 1; j++)
    {
        st_data[j][8] = std::to_string((std::stoi(st_data[j][0]) - num_prof1) * (std::stof(st_data[k][8]) - std::stof(st_data[i - 1][8])) /
            (num_prof2 - num_prof1) + std::stof(st_data[i - 1][8]));
        new_Cy[j][V0_i] = 0.95f * std::stof(st_data[j][8]);
    }
}

/// <summary>
/// Вычисление коэфициента c_y профиля
/// </summary>
/// <param name="alfa">угол атаки профиля</param>
/// <param name="st_data">масив данных лопасти</param>
/// <param name="i">текущий индекс профиля</param>
/// <param name="Cy">массив коэффициентов c_y профиля</param>
/// <param name="new_Cy">массив коэффициентов c_y профилей</param>
/// <param name="filter_Cy"></param>
/// <param name="V0_i">скорость полета</param>
void calc_prof_Cy(float& alfa, std::string** st_data, int& i, float** Cy, float** new_Cy, bool& filter_Cy, int& V0_i)
{
    short k = i;

    if (st_data[i][7] != "Втулка")
    {
        if (st_data[i][7] == "Профиль")
        {
            while (st_data[k][7] == "Профиль") // Кол-во Промеж. профилей
            {
                k++;
            }
            calc_inter_prof_Cy(k, st_data, i, new_Cy, alfa, Cy, filter_Cy, V0_i);
        }
        else
        {
            if (filter_Cy) // Подключает БД Cy
            {
                Cy_xl.write2array(Cy, st_data[k][7]); // array[k][7] - имя профиля
                filter_Cy = false;
            }
            float c_rel = std::stof(st_data[i][5]);
            st_data[i][8] = find_prof_Cy(c_rel, alfa, Cy, st_data[i][7]);
            if (i < (short)24) new_Cy[i][V0_i] = 0.95f * std::stof(st_data[i][8]);
            else new_Cy[i][V0_i] = 0.95f * std::stof(st_data[i][8]) * 0.0f;
        }
    }
    else new_Cy[i][V0_i] = 0;
}

/// <summary>
/// Вычисление коэфициента c_x профиля по заданным относительной толщине и угле атаки
/// </summary>
/// <param name="c_rel">относительная толщина профиля</param>
/// <param name="alfa">угол атаки профиля</param>
/// <param name="Cx">массив коэффициентов c_x профиля</param>
/// <param name="profile_name">имя профиля</param>
/// <returns>коэфициент c_x профиля</returns>
float find_prof_Cx(float& c_rel, float& alfa, float** Cx, std::string& profile_name)
{
    float prof_Cx = 0;
    if (profile_name != "Втулка")
    {
        short i = 1;
        short j = 1;
        float c1 = 0;
        float c2 = 0;
        float alfa1 = 0;
        float alfa2 = 0;
        XL xl(CxDB);

        if (alfa > Cx[xl.table_raws - 1][0] && alfa < Cx[1][0]) // Интерполяция alfa
        {
            if (c_rel > Cx[0][xl.table_cols - 1] && c_rel < Cx[0][1]) // Интерполяция c_rel
            {
                for (; j < xl.table_cols; j++)
                {
                    if (c_rel >= Cx[0][j])
                    {
                        c1 = Cx[0][j];
                        c2 = Cx[0][j - 1];
                        break;
                    }
                }
                for (; i < xl.table_raws; i++)
                {
                    if (alfa >= Cx[i][0])
                    {
                        alfa1 = Cx[i][0];
                        alfa2 = Cx[i - 1][0];
                        break;
                    }
                }
                float Cx_c1_alfa = (alfa - alfa1) * (Cx[i - 1][j] - Cx[i][j]) / (alfa2 - alfa1) + Cx[i][j];
                float Cx_c2_alfa = (alfa - alfa1) * (Cx[i - 1][j - 1] - Cx[i][j - 1]) / (alfa2 - alfa1) + Cx[i][j - 1];
                prof_Cx = (c_rel - c1) * (Cx_c2_alfa - Cx_c1_alfa) / (c2 - c1) + Cx_c1_alfa;
            }
            if (c_rel >= Cx[0][1]) //Экстраполяция справа c_rel
            {
                for (; i < xl.table_raws; i++)
                {
                    if (alfa >= Cx[i][0])
                    {
                        alfa1 = Cx[i][0];
                        alfa2 = Cx[i - 1][0];
                        break;
                    }
                }
                c1 = Cx[0][2];
                c2 = Cx[0][1];
                float Cx_c_alfa1 = (Cx[i][1] - Cx[i][2]) * (c_rel - c1) / (c2 - c1) + Cx[i][2];
                float Cx_c_alfa2 = (Cx[i - 1][1] - Cx[i - 1][2]) * (c_rel - c1) / (c2 - c1) + Cx[i - 1][2];
                prof_Cx = (alfa - alfa1) * (Cx_c_alfa2 - Cx_c_alfa1) / (alfa2 - alfa1) + Cx_c_alfa1;
            }
            if (c_rel <= Cx[0][xl.table_cols - 1]) // Экстраполяция слева c_rel
            {
                for (; i < xl.table_raws; i++)
                {
                    if (alfa >= Cx[i][0])
                    {
                        alfa1 = Cx[i][0];
                        alfa2 = Cx[i - 1][0];
                        break;
                    }
                }
                c1 = Cx[0][xl.table_cols - 1];
                c2 = Cx[0][xl.table_cols - 2];
                float Cx_c_alfa1 = Cx[i][xl.table_cols - 1] * (c2 - c_rel) / (c2 - c1) + Cx[i][xl.table_cols - 2] * (c_rel - c1) / (c2 - c1);
                float Cx_c_alfa2 = Cx[i][1, xl.table_cols - 1] * (c2 - c_rel) / (c2 - c1) + Cx[i - 1][xl.table_cols - 2] * (c_rel - c1) / (c2 - c1);
                prof_Cx = (alfa - alfa1) * (Cx_c_alfa2 - Cx_c_alfa1) / (alfa2 - alfa1) + Cx_c_alfa1;
            }
        }
        if (alfa >= Cx[1][0]) // Экстраполяция справа alfa
        {
            if (c_rel > Cx[0][xl.table_cols - 1] && c_rel < Cx[0][1]) // Интерполяция c_rel
            {
                for (; j < xl.table_cols; j++)
                {
                    if (c_rel >= Cx[0][j])
                    {
                        c1 = Cx[0][j];
                        c2 = Cx[0][j - 1];
                        break;
                    }
                }
                alfa1 = Cx[2][0];
                alfa2 = Cx[1][0];
                float Cx_c1_alfa = (Cx[1][j] - Cx[2][j]) * (alfa - alfa1) / (alfa2 - alfa1) + Cx[2][j];
                float Cx_c2_alfa = (Cx[1][j - 1] - Cx[2][j - 1]) * (alfa - alfa1) / (alfa2 - alfa1) + Cx[2][j - 1];
                prof_Cx = (c_rel - c1) * (Cx_c2_alfa - Cx_c1_alfa) / (c2 - c1) + Cx_c1_alfa;
            }
            if (c_rel >= Cx[0][1]) //Экстраполяция справа c_rel
            {
                c1 = Cx[0][2];
                c2 = Cx[0][1];
                alfa1 = Cx[2][0];
                alfa2 = Cx[1][0];
                float Cx_c1_alfa = (Cx[1][2] - Cx[2][2]) * (alfa - alfa1) / (alfa2 - alfa1) + Cx[2][2];
                float Cx_c2_alfa = (Cx[1, 1] - Cx[2, 1]) * (alfa - alfa1) / (alfa2 - alfa1) + Cx[2][1];
                prof_Cx = (Cx_c2_alfa - Cx_c1_alfa) * (c_rel - c1) / (c2 - c1) + Cx_c1_alfa;
            }
            if (c_rel <= Cx[0][xl.table_cols - 1]) // Экстраполяция слева c_rel
            {
                //alfa1 = Cy[2, 0];
                //alfa2 = Cy[1, 0];
                c1 = Cx[0][xl.table_cols - 1];
                c2 = Cx[0][xl.table_cols - 2];
                float Cx_c_alfa1 = Cx[i][xl.table_cols - 1] * (c2 - c_rel) / (c2 - c1) + Cx[i][xl.table_cols - 2] * (c_rel - c1) / (c2 - c1);
                float Cx_c_alfa2 = Cx[i - 1][xl.table_cols - 1] * (c2 - c_rel) / (c2 - c1) + Cx[i - 1][xl.table_cols - 2] * (c_rel - c1) / (c2 - c1);
                prof_Cx = (Cx_c_alfa2 - Cx_c_alfa1) * (c_rel - c1) / (c2 - c1) + Cx_c_alfa1;
            }
        }
        if (alfa <= Cx[xl.table_raws - 1][0]) // Экстраполяция слева alfa
        {
            if (c_rel > Cx[0][xl.table_cols - 1] && c_rel < Cx[0][1]) // Интерполяция c_rel
            {
                for (; j < xl.table_cols; j++)
                {
                    if (c_rel >= Cx[0][j])
                    {
                        c1 = Cx[0][j];
                        c2 = Cx[0][j - 1];
                        break;
                    }
                }
                alfa1 = Cx[xl.table_raws - 1][0];
                alfa2 = Cx[xl.table_raws - 2][0];
                float Cx_c1_alfa = Cx[xl.table_raws - 2][j] * (alfa2 - alfa) / (alfa2 - alfa1) + Cx[xl.table_raws - 1][j] * (alfa - alfa1) / (alfa2 - alfa1);
                float Cx_c2_alfa = Cx[xl.table_raws - 2][j - 1] * (alfa2 - alfa) / (alfa2 - alfa1) + Cx[xl.table_raws - 1][j - 1] * (alfa - alfa1) / (alfa2 - alfa1);
                prof_Cx = (c_rel - c1) * (Cx_c2_alfa - Cx_c1_alfa) / (c2 - c1) + Cx_c1_alfa;
            }
            if (c_rel >= Cx[0][1]) //Экстраполяция справа c_rel
            {
                alfa1 = Cx[xl.table_raws - 1][0];
                alfa2 = Cx[xl.table_raws - 2][0];
                c1 = Cx[0][2];
                c2 = Cx[0][1];
                float Cx_c1_alfa = Cx[xl.table_raws - 1][2] * (alfa2 - alfa) / (alfa2 - alfa1) + Cx[xl.table_raws - 2][2] * (alfa - alfa1) / (alfa2 - alfa1);
                float Cx_c2_alfa = Cx[xl.table_raws - 1][1] * (alfa2 - alfa) / (alfa2 - alfa1) + Cx[xl.table_raws - 2][1] * (alfa - alfa1) / (alfa2 - alfa1);
                prof_Cx = (Cx_c2_alfa - Cx_c1_alfa) * (c_rel - c1) / (c2 - c1) + Cx_c1_alfa;
            }
            if (c_rel <= Cx[0][xl.table_cols - 1]) // Экстраполяция слева c_rel
            {
                c1 = Cx[0][xl.table_cols - 1];
                c2 = Cx[0][xl.table_cols - 2];
                alfa1 = Cx[xl.table_raws - 1][0];
                alfa2 = Cx[xl.table_raws - 2][0];
                float Cx_c1_alfa = Cx[xl.table_raws - 1][xl.table_cols - 1] * (alfa2 - alfa) / (alfa2 - alfa1) + Cx[xl.table_raws - 2][xl.table_cols - 1] * (alfa - alfa1) / (alfa2 - alfa1);
                float Cx_c2_alfa = Cx[xl.table_raws - 1][xl.table_cols - 2] * (alfa2 - alfa) / (alfa2 - alfa1) + Cx[xl.table_raws - 2][xl.table_cols - 2] * (alfa - alfa1) / (alfa2 - alfa1);
                prof_Cx = Cx_c1_alfa * (c2 - c_rel) / (c2 - c1) + Cx_c2_alfa * (c_rel - c1) / (c2 - c1);
            }
        }
    }
    else prof_Cx = 0;

    return prof_Cx;
}

/// <summary>
/// Вычисление коэффициента c_x промежуточного профиля
/// </summary>
/// <param name="k">количество промежуточных профилей</param>
/// <param name="st_data">масив данных лопасти</param>
/// <param name="i">текущий индекс профиля</param>
/// <param name="new_Cx">массив коэффициентов c_x профилей</param>
/// <param name="alfa">угол атаки</param>
/// <param name="Cx">массив коэффициентов c_x профиля</param>
/// <param name="new_Cy">массив коэффициентов c_y профилей</param>
/// <param name="M">число махов полета</param>
/// <param name="filter_Cx"></param>
/// <param name="V0_i">скорость полета</param>
void calc_inter_prof_Cx(short& k, std::string** st_data, int& i, float** new_Cx, float& alfa, float** Cx, float** new_Cy, float& M, bool& filter_Cx, int& V0_i)
{
    int num_prof2 = std::stoi(st_data[k][0]);
    int num_prof1 = std::stoi(st_data[i - 1][0]);
    if (filter_Cx)
    {
        Cx_xl.write2array(Cx, st_data[k][7]); // array[k][7] - имя профиля
        filter_Cx = false;
    }
    float c_rel = std::stof(st_data[k][5]);
    st_data[k][9] = find_prof_Cy(c_rel, alfa, Cx, st_data[k][7]);
    for (int j = num_prof1; j < num_prof2 - 1; j++)
    {
        st_data[j][9] = std::to_string((std::stoi(st_data[j][0]) - num_prof1) * (std::stof(st_data[k][9]) - std::stof(st_data[i - 1][9])) /
            (num_prof2 - num_prof1) + std::stof(st_data[i - 1][9]));
        new_Cx[j][V0_i] = (float)(1 / 0.95f * std::stof(st_data[i][9]) * (1 + 20 * pow(new_Cy[i][V0_i], 2) * pow(M - 0.4, 2)) * (1 + 80 * pow(std::stof(st_data[i][5]), 2) * pow(M - 0.4, 2)));
    }
}

/// <summary>
/// 
/// </summary>
/// <param name="alfa">угол атаки</param>
/// <param name="st_data">масив данных лопасти</param>
/// <param name="i">текущий индекс профиля</param>
/// <param name="Cx">массив коэффициентов c_x профиля</param>
/// <param name="new_Cx">массив коэффициентов c_x профилей</param>
/// <param name="new_Cy">массив коэффициентов c_y профилей</param>
/// <param name="M">число махов полета</param>
/// <param name="filter_Cx"></param>
/// <param name="V0_i">скорость полета</param>
void calc_prof_Cx(float& alfa, std::string** st_data, int& i, float** Cx, float** new_Cx, float** new_Cy, float& M, bool& filter_Cx, int& V0_i)
{
    short k = i; // счетчик
    if (st_data[i][7] != "Втулка")
    {
        if (st_data[i][7] == "Профиль")
        {
            while (st_data[k][7] == "Профиль")
            {
                k++;
            }
            calc_inter_prof_Cx(k, st_data, i, new_Cx, alfa, Cx, new_Cy, M, filter_Cx, V0_i);
        }
        else
        {
            if (filter_Cx)
            {
                Cx_xl.write2array(Cx, st_data[k][7]); // array[k][7] - имя профиля
                filter_Cx = false;
            }
            float c_rel = std::stof(st_data[i][5]);
            st_data[i][9] = find_prof_Cx(c_rel, alfa, Cx, st_data[i][7]); // Cx профиля
            new_Cx[i][V0_i] = (float)(1 / 0.95f * std::stof(st_data[i][9]) * (1 + 20 * pow(new_Cy[i][V0_i], 2) * pow(M - 0.4, 2)) * (1 + 80 * pow(std::stof(st_data[i][5]), 2) * pow(M - 0.4, 2)));
        }
        if (new_Cx[i][V0_i] > 1.1f) new_Cx[i][V0_i] = 1.1f; // ограничитель
    }
    else
    {
        st_data[i][9] = 1.1f;
        new_Cx[i][V0_i] = std::stof(st_data[i][9]);
    }
}

struct input
{
    std::string blade_name;
    std::string replase; // мм
    std::string cutting; // мм
    std::string blade_length; // мм
    std::string fi_07; // гр
    std::string count_blades; // шт
    std::string redor_coeff;
    std::string CUA_redor;
    std::string turns; // об/мин
    std::string engine_pow; // л.с.
    std::string weight; // кг
    std::string V0; // км/ч
};

int main()
{
    //----ЗНАЧЕНИЯ ПО УМОЛЧАНИЮ-----
    const int frst_sec = 1; // первое сечение
    const int last_sec = 25; // последнее сечение
    const int H = 0; // высота полета, км
    const int p = 760; // атмосферное давление, мм.рт.ст.
    const int t = 15; // температура воздуха, гр. Цельсия
    const float scale_coeff = 1.0f; // коэффициент масштаба
    const float split_coeff = 0.0f; // коэффициент зазора
    //------------------------------

    setlocale(LC_ALL, "RU");
    input inpt;
    std::string calc_mode;
    //----ВВОД ИСХОДНЫХ ДАННЫХ------
    std::cout << "Введите имя лопасти: ", std::cin >> inpt.blade_name;
    std::cout << "Введите смещение, мм: ", std::cin >> inpt.replase;
    std::cout << "Введите обрезку, мм: ", std::cin >> inpt.cutting;
    std::cout << "Введите длину лопасти, мм: ", std::cin >> inpt.blade_length;
    std::cout << "Введите угол на 0,7R, гр: ", std::cin >> inpt.fi_07;
    std::cout << "Введите количество лопастей: ", std::cin >> inpt.count_blades;
    std::cout << "Введите коэффициент масштаба: ", std::cin >> inpt.count_blades;
    std::cout << "Введите передаточное число редуктора: ", std::cin >> inpt.redor_coeff;
    std::cout << "Введите КПД редуктора: ", std::cin >> inpt.CUA_redor;
    std::cout << "Введите число оборотов двигателя, об/мин: ", std::cin >> inpt.turns;
    std::cout << "Введите мощность двигателя, л.с.: ", std::cin >> inpt.engine_pow;
    std::cout << "Введите массу ЛА, кг: ", std::cin >> inpt.weight;
    std::cout << "Введите скорость полета, км/ч: ", std::cin >> inpt.V0;
    std::cout << "Введите режим расчета:\n" << "ПМ - подбор мощности\n" << "ПТ - подбор тяги\n" << "БП - без подбора\n" << "Режим: ";
    std::cin >> calc_mode;
    //ПРЕОБРАЗОВАНИЕ ТИПОВ ИСХОДНЫХ ДАННЫХ
    int count_V0 = 1; // количество скоростей
    float replace = std::stof(inpt.replase) / 1000; // смещение, м
    float cutting = std::stof(inpt.cutting) / 1000; // обрезка, м
    float blade_length = std::stof(inpt.blade_length) / 1000; // длина лопасти, м
    float* fi07 = new float[count_V0];
    fi07[0] = std::stof(inpt.fi_07); // угол на 0,7R, гр
    int k = std::stoi(inpt.count_blades); // количество лопастей
    float redor_coeff = std::stof(inpt.redor_coeff); // передаточное число редуктора
    float CUA_redor = std::stof(inpt.CUA_redor); // КПД редуктора
    float turns = std::stof(inpt.turns); // число оборотов двигателя, об/мин
    float engine_pow = std::stof(inpt.engine_pow); // мощность двигателя, л.с.
    float G = std::stof(inpt.weight); // массу ЛА, кг
    float* V0 = new float[count_V0];
    V0[0] = std::stof(inpt.V0) * 1000 / 3600; // скорость полета, м/с
    float R = blade_length + replace - cutting; // радиус винта, м
    float CRC = 0.004f;
    float dr = blade_length / 25;
    float U = PI * R * turns / (30 * redor_coeff); // окружная скорость, м/с
    Blades_xl.find_table_size(inpt.blade_name);
    std::string** StDt = create_2dstr_array(Blades_xl.table_raws, Blades_xl.table_cols);
    Blades_xl.write2array(StDt, inpt.blade_name);

    int count_out_pars = 5;
    float** output = create_2dflt_array(count_out_pars, count_V0); // массив выходных данных

    for (int i = 0; i < Blades_xl.table_raws; i++) // Перезапись (double)r_rel
    {
        StDt[i][1] = ((i + 1 - 0.5) * dr + replace) / R;
        float b = std::stof(StDt[i][2]); // хорды
        float c = std::stof(StDt[i][4]); // абсолютные толщины
        StDt[i][2] = b * scale_coeff;
        StDt[i][4] = c * scale_coeff;
    }
    float* sumdN = new float[Blades_xl.table_raws];
    float* sumdT = new float[Blades_xl.table_raws];
    float Np = 0; // передеется по ссылке
    float DEL = 1; // передается по ссылке
    float* T = new float[count_V0];
    float* CUA = new float[count_V0];
    float* CUA0 = new float[count_V0];
    // Другие данные
    float** Cy = create_2dflt_array(Cy_xl.table_raws, Cy_xl.table_cols);
    float** Cx = create_2dflt_array(Cx_xl.table_raws, Cx_xl.table_cols);
    bool ConnectCy = true;
    bool connect_Cx = true;
    // Массивы
    float** alfa = create_2dflt_array(Blades_xl.table_raws, count_V0);
    float** newCy = create_2dflt_array(Blades_xl.table_raws, count_V0);
    float** newCx = create_2dflt_array(Blades_xl.table_raws, count_V0);
    float** CRCp = create_2dflt_array(Blades_xl.table_raws, count_V0);
    float** Vy = create_2dflt_array(Blades_xl.table_raws, count_V0);
    float** dF = create_2dflt_array(Blades_xl.table_raws, count_V0);
    float** dQx = create_2dflt_array(Blades_xl.table_raws, count_V0);
    float** dN = create_2dflt_array(Blades_xl.table_raws, count_V0);
    float** dT = create_2dflt_array(Blades_xl.table_raws, count_V0);

    //bool sucsTrySelV0 = true;

    float a = (float)sqrt(402 * (t + 273 - 6.5 * H));
    float v = (float)(13.98 * pow(10, -6) / DEL * pow((t + 273 - 6.5 * H) / 273, 0.77));
    Np = (float)(engine_pow * CUA_redor * pow(DEL, 1.30285));
    for (int V0i = 0; V0i < count_V0; V0i++)
    {
        output[0][V0i] = V0[V0i] * 3600 / 1000;
        float V0_rel = V0[V0i] / U;
        float W = (float)sqrt(pow(V0[V0i], 2) + pow(U, 2)); // м/с, полная скорость
        if (W >= a) // Условие выполнения расчёта
        {
            std::cout << "Полная скорость винта превысила скорость звука! Расчет невозможен";
            release_2d_array(alfa, Blades_xl.table_raws);
            release_2d_array(newCy, Blades_xl.table_raws);
            release_2d_array(newCx, Blades_xl.table_raws);
            release_2d_array(CRCp, Blades_xl.table_raws);
            release_2d_array(Vy, Blades_xl.table_raws);
            release_2d_array(dF, Blades_xl.table_raws);
            release_2d_array(dQx, Blades_xl.table_raws);
            release_2d_array(dN, Blades_xl.table_raws);
            release_2d_array(dT, Blades_xl.table_raws);
            release_2d_array(alfa, Blades_xl.table_raws);
            release_2d_array(output, count_out_pars);
            delete[] fi07;
            delete[] newCy;
            delete[] newCx;
            delete[] CRCp;
            delete[] Vy;
            delete[] dF;
            delete[] dQx;
            delete[] dN;
            delete[] dT;
            delete[] sumdN;
            delete[] sumdT;
            delete[] T;
            delete[] CUA;
            delete[] CUA0;
            release_2d_array(StDt, Blades_xl.table_raws);
            release_2d_array(Cy, Cy_xl.table_raws);
            release_2d_array(Cx, Cx_xl.table_raws);
            Cy_xl.release();
            Cx_xl.release();
            Blades_xl.release();
            std::abort();
        }
     M2:
        for (int i = frst_sec - 1; i < last_sec; i++)
        {
        M1:
            float Vy_rel = (float)(std::stof(StDt[i][6]) * (V0_rel / 2 + sqrt(abs(pow(V0_rel, 2) / 4 + CRC * (1 - CRC)))));
            float Vx_rel = std::stof(StDt[i][1]) - CRC / std::stof(StDt[i][1]);
            float V_rel = (float)sqrt(pow(Vy_rel, 2) + pow(Vx_rel, 2));
            float V = V_rel * U;
            float M = V / a;
            float Re = V * std::stof(StDt[i][2]) / (v * 1000);
            float betta = (float)(180 / PI * atan2(Vy_rel, Vx_rel)); // градусы
            alfa[i][V0i] = std::stof(StDt[i][3]) + fi07[V0i] - betta;
            //---------Вычисление newCy, newCx-----------
            calc_prof_Cy(alfa[i][V0i], StDt, i, Cy, newCy, ConnectCy, V0i);
            calc_prof_Cx(alfa[i][V0i], StDt, i, Cx, newCx, newCy, M, connect_Cx, V0i);
            CRCp[i][V0i] = (float)(k * std::stof(StDt[i][2]) * V_rel * sqrt(pow(newCy[i][V0i], 2) + pow(newCx[i][V0i], 2)) / (8 * PI * R * 1000));
            if (abs(CRCp[i][V0i] - CRC) <= pow(10, -4)) // delCRC
            {
                Vy[i][V0i] = Vy_rel * U;
                dF[i][V0i] = (float)(DEL / 16000 * std::stof(StDt[i][2]) * pow(V, 2) * dr * sqrt(pow(newCy[i][V0i], 2) + pow(newCx[i][V0i], 2)));
                float gamma = (float)(betta * PI / 180 + atan2(newCx[i][V0i], newCy[i][V0i]));
                dQx[i][V0i] = dF[i][V0i] * (float)sin(gamma);
                dN[i][V0i] = U * std::stof(StDt[i][1]) * dQx[i][V0i] / 75;
                sumdN[i] = dN[i][V0i];
                dT[i][V0i] = (float)(dF[i][V0i] * cos(gamma));
                sumdT[i] = dT[i][V0i];
            }
            else
            {
                CRC = (CRCp[i][V0i] - CRC) / 2 + CRC;
                goto M1;
            }
        }
        float dN_sum = array_sum(sumdN, Blades_xl.table_raws);
        float dT_sum = array_sum(sumdT, Blades_xl.table_raws);
        float Nn = k * dN_sum;
        T[V0i] = k * dT_sum;
        output[4][V0i] = Nn;
        output[2][V0i] = T[V0i];
        if (calc_mode == "БП")
        {
            CUA0[V0i] = round_to(T[V0i] * sqrt(abs(T[V0i] / (DEL * PI))) / (37.5 * Nn * R), 2);
            CUA[V0i] = round_to(T[V0i] * V0[V0i] / (75 * Nn), 2);
            output[1][V0i] = round_to(fi07[V0i], 1);
            if (CUA0[V0i] >= CUA[V0i]) output[3][V0i] = CUA0[V0i];
            else output[3][V0i] = CUA[V0i];
        }
        else if (calc_mode == "ПМ")
        {
            if (abs(Np - Nn) < pow(10, -2))
            {
                CUA0[V0i] = round_to(T[V0i] * sqrt(abs(T[V0i] / (DEL * PI))) / (37.5 * Nn * R), 2);
                CUA[V0i] = round_to(T[V0i] * V0[V0i] / (75 * Nn), 2);
                output[1][V0i] = round_to(fi07[V0i], 1);
                if (CUA0[V0i] >= CUA[V0i]) output[3][V0i] = CUA0[V0i];
                else output[3][V0i] = CUA[V0i];
                //sucsTrySelV0 = true;
            }
            else
            {
                fi07[V0i] += round_to((Np - Nn) / Np, 4);
                //sucsTrySelV0 = false;
                goto M2;
            }
        }
        else if (calc_mode == "ПТ")
        {
            if (abs(G - T[V0i]) < pow(10, -2))
            {
                CUA0[V0i] = round_to(T[V0i] * sqrt(abs(T[V0i] / (DEL * PI))) / (37.5 * Nn * R), 2);
                CUA[V0i] = round_to(T[V0i] * V0[V0i] / (75 * Nn), 2);
                output[1][V0i] = round_to(fi07[V0i], 1);
                if (CUA0[V0i] >= CUA[V0i]) output[3][V0i] = CUA0[V0i];
                else output[3][V0i] = CUA[V0i];
                //sucsTrySelV0 = true;
            }
            else
            {
                fi07[V0i] += round_to((G - T[V0i]) / G, 4);
                //sucsTrySelV0 = false;
                goto M2;
            }
        }
    }
    //----------ВЫВОД ДАННЫХ---------
    std::cout << "Запись данных";
    std::ofstream calc_data;
    calc_data.open("Расчетные данные.txt");
    calc_data << "N, л.с. = " << engine_pow << " D, мм. = " << 2 * R << " n, об/мин. = " << turns
        << " Iред = " << redor_coeff << " U, м/с. = " << U;
    calc_data << "\nV0, км/ч\t";
    for (int i = 0; i < count_V0; i++)
    {
        calc_data << output[0][i] << "\t";
    }
    calc_data << "\nFi07, гр\t";
    for (int i = 0; i < count_V0; i++)
    {
        calc_data << output[1][i] << "\t";
    }
    calc_data << "\nT, кг\t";
    for (int i = 0; i < count_V0; i++)
    {
        calc_data << output[2][i] << "\t";
    }
    calc_data << "\nКПД\t";
    for (int i = 0; i < count_V0; i++)
    {
        calc_data << output[3][i] << "\t";
    }
    calc_data << "\nNп, л.с.\t";
    for (int i = 0; i < count_V0; i++)
    {
        calc_data << output[4][i] << "\t";
    }
    calc_data << std::endl;
    //-----------------------------
    calc_data.close();
    release_2d_array(alfa, Blades_xl.table_raws);
    release_2d_array(newCy, Blades_xl.table_raws);
    release_2d_array(newCx, Blades_xl.table_raws);
    release_2d_array(CRCp, Blades_xl.table_raws);
    release_2d_array(Vy, Blades_xl.table_raws);
    release_2d_array(dF, Blades_xl.table_raws);
    release_2d_array(dQx, Blades_xl.table_raws);
    release_2d_array(dN, Blades_xl.table_raws);
    release_2d_array(dT, Blades_xl.table_raws);
    release_2d_array(alfa, Blades_xl.table_raws);
    release_2d_array(output, count_out_pars);
    delete[] fi07;
    delete[] newCy;
    delete[] newCx;
    delete[] CRCp;
    delete[] Vy;
    delete[] dF;
    delete[] dQx;
    delete[] dN;
    delete[] dT;
    delete[] sumdN;
    delete[] sumdT;
    delete[] T;
    delete[] CUA;
    delete[] CUA0;
    release_2d_array(StDt, Blades_xl.table_raws);
    release_2d_array(Cy, Cy_xl.table_raws);
    release_2d_array(Cx, Cx_xl.table_raws);
    Cy_xl.release();
    Cx_xl.release();
    Blades_xl.release();
    std::cout << "Расчет завершен";
}