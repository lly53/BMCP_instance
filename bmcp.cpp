//
//  main.cpp
//  DSUKP
//
//  Created by llw&wzq on 2019/11/19.
//  Copyright © 2019 UA. All rights reserved.
//

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string.h>
#include <stdio.h>
#include <iomanip>
#include <sstream>
using namespace std;

#define MININT -2147483648
#define SDF_MAX(X,Y)  ((X) > (Y) ? (Y) : (X))

char *instanceName;
int seed;
char *rep;
char nameFinalResult[256];
double maxRunTime;
double bestTime;
double beginTime;

int numItem;
int numElement;
int knapsackSize;
int *profit;
int *weight;
int **relationMatrix;

int totalProfit;       //record the total profit of current solution
int totalWeight;       //record the total weight of current solution
int *chosenItem;       //the selected items
int *repeatElement;       //number of each elements of the selected items
int **elementAddress;  //the elements correspond to each item
int *lengthEleAdd;       //number of elements correspond to each item

int bestTotalProfit;
int bestTotalWeight;
int *bestChosenItem;
int *bestRepeatElement;

double *pro_array;

void read_instance()
{
    ifstream FIC;
    FIC.open(instanceName);


    if(FIC.fail())
    {
        cout << "1-can not open the file " << instanceName << endl;
        exit(0);
    }
    if(FIC.eof())
    {
        cout << "2-can not open the file " << instanceName << endl;
    }

    numItem = 0;
    numElement = 0;
    knapsackSize = 0;

    char str_reading[100];        //read by every str
    char str_ilne[50];            //read by every line
    char *p;
    int str[50];
    int a = 0;

    //read numItem/numElement/knapsack size
    for(int i = 0; i < 3; i++)
        FIC.getline(str_ilne,sizeof(str_ilne));

    p = strtok(str_ilne,"=");
    while(p != NULL)
    {
        str[a] = atoi(p);
        p = strtok(NULL,"=");
        a++;
    }
    numItem = str[1];
    numElement = str[2];
    knapsackSize = str[3];
    cout << "read instance :" << "numItem= " << numItem << ", numElement= " << numElement << ", knapsackSize= " << knapsackSize << endl;

    chosenItem = new int[numItem];
    repeatElement = new int[numElement];
    lengthEleAdd = new int[numItem];
    bestChosenItem = new int[numItem];
    bestRepeatElement = new int[numElement];
    pro_array = new double[numItem];

//    read weight[] of items
    for(int i = 0; i < 5; i++)
        FIC >> str_reading;
    weight = new int[numItem + 1];            //ATTENTION: now add  +1 represent don't choose
    for(int i = 0; i < numItem; i++)
    {
        FIC >> str_reading;
        weight[i] = atoi(str_reading);
    }
    weight[numItem] = 0;

    //read profit[] of elements
    for(int i = 0; i < 5; i++)
        FIC >> str_reading;
    profit = new int[numElement];
    for(int i = 0; i < numElement; i++)
    {
        FIC >> str_reading;
        profit[i] = atoi(str_reading);
    }

    //read relation matrix[]
    a = 0;
    for(int i = 0; i < 2; i++)
        FIC >> str_reading;
    relationMatrix = new int *[numItem];
    for(int x = 0; x < numItem; x++)
        relationMatrix[x] = new int [numElement];
    for(int x = 0; x < numItem; x++)
        for(int y = 0; y < numElement; y++)
            relationMatrix[x][y] = 0;
    for(int x = 0; x < numItem; x++)
        for(int y = 0; y < numElement; y++)
        {
            FIC >> a;
            relationMatrix[x][y] = a;
        }

    cout << "Finish loading data~" << endl;
    FIC.close();
}

void check_result(int *test_chosenItem, int *test_repeatElement, int test_p, int test_w)
{
    int temp_w = 0, temp_p = 0;
    int temp_element[numElement];

    for(int i = 0; i < numElement; i++)
        temp_element[i] = 0;

    for(int i = 0; i < numItem; i++)
        if(test_chosenItem[i] == 1)
        {
            temp_w += weight[i];

            for(int j = 0; j < lengthEleAdd[i]; j++)
                temp_element[elementAddress[i][j]] += 1;
        }
    for(int j = 0; j < numElement; j++)
        if(test_repeatElement[j] > 0)
            temp_p += profit[j];

    if(temp_w == test_w && temp_p == test_p)
        cout << "Result is right after : ";
    else
    {
        cout << "Result is wrong!!! after";
//        cout << "temp_w = " << temp_w << ", totalWeight = " << totalWeight << endl;
//        cout << "temp_p = " << temp_p << ", totalProfit = " << totalProfit << endl;
    }

    if(temp_w > knapsackSize)
        cout << "This a infeasible sloution!" << endl;

    for(int j = 0; j < numElement; j++)
        if(temp_element[j] != test_repeatElement[j])
        {
            cout << "Result is wrong!!! temp_element[j] != test_repeatElement[j]" << endl;
            exit(-1);
        }

}

void output_result()
{
    cout << "best total weight = " << bestTotalWeight << ", bset total profit = " << bestTotalProfit << endl;

    cout << "best selected item : " <<endl;
    for(int i = 0; i < numItem; i++)
        if(bestChosenItem[i] == 1)
            cout << i << " " ;
    cout << endl;

    cout << "best selected element : " <<endl;
    for(int j = 0; j < numElement; j++)
        if(bestRepeatElement[j] > 0)
            cout << j << " " ;
    cout << endl;

    cout << "best UNselected element : " <<endl;
    for(int j = 0; j < numElement; j++)
        if(bestRepeatElement[j] == 0)
            cout << j << " " ;
    cout << endl;
}

void output_result2()
{
    cout << "total weight = " << totalWeight << ", total profit = " << totalProfit << endl;

    cout << "selected item : " <<endl;
    for(int i = 0; i < numItem; i++)
        if(chosenItem[i] == 1)
            cout << i << " " ;
    cout << endl;

    cout << "selected element : " <<endl;
    for(int j = 0; j < numElement; j++)
        if(repeatElement[j] > 0)
            cout << j << " " ;
    cout << endl;

    cout << "UNselected element : " <<endl;
    for(int j = 0; j < numElement; j++)
        if(repeatElement[j] == 0)
            cout << j << " " ;
    cout << endl;
}

void record_best_so_far()
{
    bestTime = (clock()-beginTime) / CLOCKS_PER_SEC;

    bestTotalProfit = totalProfit;
    bestTotalWeight = totalWeight;

    for(int i = 0; i < numItem; i++)
        bestChosenItem[i] = chosenItem[i];
    for(int j = 0; j < numElement; j++)
        bestRepeatElement[j] = repeatElement[j];
}

void qsort(int *s, int *address, int l, int r)
{
    if (l < r)
    {
        int  i = l, j = r;
        int x = s[l];
        int y = address[l];
        while (i < j)
        {
            while(i < j && s[j] <= x)
                j--;
            if(i < j)
            {
                s[i] = s[j];
                address[i] = address[j];
                i++;
            }

            while(i < j && s[i] > x)
                i++;
            if(i < j)
            {
                s[j] = s[i];
                address[j] = address[i];
                j--;
            }
        }
        s[i] = x;
        address[i] = y;

        qsort(s, address, l, i - 1);
        qsort(s, address, i + 1, r);
    }
}

void descend_search()
{
    int is_improve = 1;
    int selected_address[numItem + 1], unselected_address[numItem + 1];
    int length1, length2;
    int count;
    int delta_weight, new_total_weight;
    int add_item, remove_item;
    int temp_re_ele[numElement], changed_element[numElement];
    int add_element, remove_element;
    int old_profit, new_profit;
    int delta_profit, max_delta_profit;
    int num_neighboor;
    int remove_neighboor[500], add_neighboor[500];
//    int best_new_total_profit[500];
    int index;



    while (is_improve != 0)
    {

        for(int x = 0; x < numItem + 1; x++)
        {
            selected_address[x] = 0;
            unselected_address[x] = 0;
        }

        count = length1 = 0;
        for(int j = 0; j < numItem; j++)    // length1 is for selected
            if(chosenItem[j] == 1)
            {
                selected_address[count] = j;
                count ++;
            }
        selected_address[count] = numItem;
        count ++;
        length1 = count;

        count = length2 = 0;
        for(int i = 0; i < numItem; i++)    // length2 is for UNselected
            if(chosenItem[i] == 0)
            {
                unselected_address[count] = i;
                count ++;
            }
        unselected_address[count] = numItem;
        count ++;
        length2 = count;

        max_delta_profit = num_neighboor = 0;
        for(int a = 0; a < length1; a ++)         // sel
            for(int b = 0; b < length2; b ++)        // UNsel
            {
                add_item = unselected_address[b];
                remove_item = selected_address[a];

                if(add_item == numItem)
                    continue;

                //exclude infeasible solution
                delta_weight = weight[add_item] - weight[remove_item];
                new_total_weight = totalWeight + delta_weight;
                if(new_total_weight > knapsackSize)
                    continue;

                //calculate delta profit
                for(int k = 0; k< numElement; k++)
                {
                    temp_re_ele[k] = repeatElement[k];
                    changed_element[k] = 0;
                }

                if(remove_item != numItem)
                    for(int p = 0; p < lengthEleAdd[remove_item]; p++)
                    {
                        remove_element = elementAddress[remove_item][p];
                        temp_re_ele[remove_element] -= 1;
                        changed_element[remove_element] = 1;
                    }

                for(int q = 0; q < lengthEleAdd[add_item]; q++)
                {
                    add_element = elementAddress[add_item][q];
                    temp_re_ele[add_element] += 1;
                    if(changed_element[add_element] == 0)
                        changed_element[add_element] = 1;
                }

                old_profit = new_profit = 0;
                for(int m = 0; m < numElement; m++)
                    if(changed_element[m] == 1)
                    {
                        if(repeatElement[m] > 0)
                            old_profit += profit[m];
                        if(temp_re_ele[m] > 0)
                            new_profit += profit[m];
                    }
                delta_profit = new_profit - old_profit;


                if(delta_profit > max_delta_profit)
                {
                    max_delta_profit = delta_profit;
//                    best_new_total_profit[0] = totalProfit + delta_profit;
                    remove_neighboor[0] = remove_item;
                    add_neighboor[0] = add_item;
                    num_neighboor = 1;
                }
                else if(delta_profit == max_delta_profit && num_neighboor < 500)
                {
//                    best_new_total_profit[num_best_neighboor] = totalProfit + delta_profit;
                    remove_neighboor[num_neighboor] = remove_item;
                    add_neighboor[num_neighboor] = add_item;
                    num_neighboor ++;
                }
            }

        //select a move and make a move
        if(max_delta_profit > 0 && num_neighboor > 0)
        {
            //select a move
            index = rand() % num_neighboor;
            remove_item = remove_neighboor[index];
            add_item = add_neighboor[index];

            if(remove_item != numItem)
            {
                chosenItem[remove_item] = 0;
                chosenItem[add_item] = 1;

                //update repeatElement array
                for(int p = 0; p < lengthEleAdd[remove_item]; p++)
                {
                    remove_element = elementAddress[remove_item][p];
                    repeatElement[remove_element] -= 1;
                }

                for(int q = 0; q < lengthEleAdd[add_item]; q++)
                {
                    add_element = elementAddress[add_item][q];
                    repeatElement[add_element] += 1;
                }
            }
            else
            {
                 chosenItem[add_item] = 1;

                for(int q = 0; q < lengthEleAdd[add_item]; q++)
               {
                   add_element = elementAddress[add_item][q];
                   repeatElement[add_element] += 1;
               }
            }

            totalProfit += max_delta_profit;
            totalWeight = totalWeight + weight[add_item] - weight[remove_item];

            bestTime = (clock()-beginTime) / CLOCKS_PER_SEC;
            is_improve = 1;
        }
        else
            is_improve = 0;
    }

//    check_result(chosenItem, repeatElement, totalProfit, totalWeight);
//    cout << "descent." << endl;
//    exit(-1);

//    cout << "descend totalweight = " << totalWeight << ", descend totalprofit = " << totalProfit << endl;
}


void initialization()
{
    int rand_item;
    int l_unselected_item = numItem;
    int temp_weight;
    int index;
    int temp_item[numItem];
    int temp;

    totalProfit = 0;
    totalWeight = 0;

    for(int i = 0; i < numItem; i++)
    {
        chosenItem[i] = 0;
        lengthEleAdd[i] = 0;
        temp_item[i] = i;
    }
    for(int j = 0; j < numElement; j++)
        repeatElement[j] = 0;

    elementAddress = new int *[numItem];
    for(int x = 0; x < numItem; x++)
        elementAddress[x] = new int [numElement];
    for(int x = 0; x < numItem; x++)
        for(int y = 0; y < numElement; y++)
            elementAddress[x][y] = 0;

    for(int x = 0; x < numItem; x++)
    {
        index = 0;
        for(int y = 0; y < numElement; y++)
            if(relationMatrix[x][y] == 1)
            {
                elementAddress[x][index] = y;
                index++;
            }
        lengthEleAdd[x] = index;
    }

    while(totalWeight <= knapsackSize)
    {
        temp = rand() % l_unselected_item;
        rand_item = temp_item[temp];

        temp_weight =totalWeight + weight[rand_item];

        if(temp_weight <= knapsackSize)
        {
            totalProfit = 0;

            chosenItem[rand_item] = 1;
            totalWeight += weight[rand_item];

            for(int i = 0; i < lengthEleAdd[rand_item]; i++)
                repeatElement[elementAddress[rand_item][i]] += 1;

            for(int j = 0; j < numElement; j++)
                if(repeatElement[j] > 0)
                    totalProfit += profit[j];

            temp_item[temp] =  temp_item[l_unselected_item - 1];
            l_unselected_item --;
        }
        else
            break;
    }

//    cout << "initial totalweight = " << totalWeight << ", initial totalprofit = " << totalProfit << endl;

//    check_result(chosenItem, repeatElement, totalProfit, totalWeight);
//    cout << "random initial." << endl;
//    exit(-1);

    descend_search();
}

void tabu_search()
{
    int selected_address[numItem + 1], unselected_address[numItem + 1];
    int length1, length2;
    int count;
    int tabu_depth, tabu_iter;
    int non_improve;
    int delta_profit, max_delta_profit;
    int num_neighboor;
    int tabu_tenure[numItem + 1];
    int add_item, remove_item;
    int delta_weight, new_total_weight;
    int temp_re_ele[numElement], changed_element[numElement];
    int add_element, remove_element;
    int old_profit, new_profit;
    int remove_neighboor[500], add_neighboor[500];
    int index;
    int t1, t2;
    double p1 = 0.5, p2 = 0.5; // for calculate pro_array

    //transmit best_so_far to current solution
    totalProfit = bestTotalProfit;
    totalWeight = bestTotalWeight;

    for(int i = 0; i < numItem; i++)
        chosenItem[i]= bestChosenItem[i];
    for(int j = 0; j < numElement; j++)
        repeatElement[j] = bestRepeatElement[j];

    //initial
    for(int x = 0; x < numItem; x++)
    {
        tabu_tenure[x] = 0;
        pro_array[x] = 0.5;
    }
    tabu_tenure[numItem] = 0;

    tabu_iter = non_improve = 0;
    tabu_depth = 100;
    while (non_improve < tabu_depth)
    {
        if((clock()-beginTime) / CLOCKS_PER_SEC > maxRunTime)
            return;

        for(int x = 0; x < numItem + 1; x++)
        {
          selected_address[x] = 0;
          unselected_address[x] = 0;
        }

        count = length1 = 0;
        for(int j = 0; j < numItem; j++)    // length1 is for selected
          if(chosenItem[j] == 1)
          {
              selected_address[count] = j;
              count ++;
          }
        selected_address[count] = numItem;
        count ++;
        length1 = count;

        count = length2 = 0;
        for(int i = 0; i < numItem; i++)    // length2 is for UNselected
          if(chosenItem[i] == 0)
          {
              unselected_address[count] = i;
              count ++;
          }
        unselected_address[count] = numItem;
        count ++;
        length2 = count;

        num_neighboor = 0;
        max_delta_profit = MININT;
        for(int a = 0; a < length1; a ++)         // sel
            for(int b = 0; b < length2; b ++)        // UNsel
            {
                add_item = unselected_address[b];
                remove_item = selected_address[a];

                if(tabu_iter < tabu_tenure[add_item] || tabu_iter < tabu_tenure[remove_item])
                    continue;

                if(add_item == numItem && remove_item == numItem)
                    continue;

                //exclude infeasible solution
                delta_weight = weight[add_item] - weight[remove_item];
                new_total_weight = totalWeight + delta_weight;
                if(new_total_weight > knapsackSize)
                    continue;

                //calculate delta profit
                for(int k = 0; k< numElement; k++)
                {
                    temp_re_ele[k] = repeatElement[k];
                    changed_element[k] = 0;
                }

                if(remove_item != numItem)
                    for(int p = 0; p < lengthEleAdd[remove_item]; p++)
                    {
                        remove_element = elementAddress[remove_item][p];
                        temp_re_ele[remove_element] -= 1;
                        changed_element[remove_element] = 1;
                    }

                if(add_item != numItem)
                    for(int q = 0; q < lengthEleAdd[add_item]; q++)
                    {
                        add_element = elementAddress[add_item][q];
                        temp_re_ele[add_element] += 1;
                        if(changed_element[add_element] == 0)
                            changed_element[add_element] = 1;
                    }

                old_profit = new_profit = 0;
                for(int m = 0; m < numElement; m++)
                    if(changed_element[m] == 1)
                    {
                        if(repeatElement[m] > 0)
                            old_profit += profit[m];
                        if(temp_re_ele[m] > 0)
                            new_profit += profit[m];
                    }
                delta_profit = new_profit - old_profit;

               if(delta_profit > max_delta_profit)
                {
                    max_delta_profit = delta_profit;
                    remove_neighboor[0] = remove_item;
                    add_neighboor[0] = add_item;
                    num_neighboor = 1;
                }
                else if(delta_profit == max_delta_profit && num_neighboor < 500)
                {
                    remove_neighboor[num_neighboor] = remove_item;
                    add_neighboor[num_neighboor] = add_item;
                    num_neighboor ++;
                }
            }

        if(num_neighboor > 0)
        {
            //select a move
            index = rand() % num_neighboor;
            remove_item = remove_neighboor[index];
            add_item = add_neighboor[index];

            if(remove_item != numItem && add_item != numItem)   //remove and add
            {
                chosenItem[remove_item] = 0;
                chosenItem[add_item] = 1;

                //update repeatElement array
                for(int p = 0; p < lengthEleAdd[remove_item]; p++)
                {
                    remove_element = elementAddress[remove_item][p];
                    repeatElement[remove_element] -= 1;
                }

                for(int q = 0; q < lengthEleAdd[add_item]; q++)
                {
                    add_element = elementAddress[add_item][q];
                    repeatElement[add_element] += 1;
                }

                //update pro_array
//                pro_array[remove_item] = pro_array[remove_item] * (1 - pro_array[remove_item]);
//                pro_array[add_item] = pro_array[add_item] * (1 + pro_array[add_item]);
                pro_array[add_item] = p1 + (1 - p1) * pro_array[add_item];
                pro_array[remove_item] = (1 - p2) * pro_array[remove_item];

            }
            else if(remove_item == numItem && add_item != numItem)  //no remove, add
            {
                chosenItem[add_item] = 1;

                for(int q = 0; q < lengthEleAdd[add_item]; q++)
                {
                    add_element = elementAddress[add_item][q];
                    repeatElement[add_element] += 1;
                }

                //update pro_array
//                pro_array[add_item] = pro_array[add_item] * (1 + pro_array[add_item]);
                 pro_array[add_item] = p1 + (1 - p1) * pro_array[add_item];
            }
            else if(remove_item != numItem && add_item == numItem) //remove, no add
            {
                chosenItem[remove_item] = 0;

                for(int p = 0; p < lengthEleAdd[remove_item]; p++)
                {
                    remove_element = elementAddress[remove_item][p];
                    repeatElement[remove_element] -= 1;
                }

                //update pro_array
//                pro_array[remove_item] = pro_array[remove_item] * (1 - pro_array[remove_item]);
                pro_array[remove_item] = (1 - p2) * pro_array[remove_item];
            }
            else
                cout << "WRONG : remove = add = numItem." << endl;

            totalProfit += max_delta_profit;
            totalWeight = totalWeight + weight[add_item] - weight[remove_item];

            //ts version 1
//            t1 = length1 * 0.4;
//            t2 = length2 * 0.2 * (100 / numItem);

            //ts version 2
//            t1 = 8 + rand() % 5;
//            t2 = 8 + rand() % 5;
//            cout << "l1 = " << length1 << ", l2= " <<length2 << endl;
//            cout << "remove : " << remove_item << ", add : " << add_item << endl;

             //ts version 3
            t1 = 4 + 1 * SDF_MAX(numItem, numElement)/100;  //4 = avd + std of all the 30 instances
            t2 = 4 + 1 * SDF_MAX(numItem, numElement)/100;

            if(t1 >= length1 || t2 >= length1)
            {
                cout << "ts_tenure is too long" << endl;
                cout << "t1 = " << t1 << ", length1 = " << length1 << endl;
            }

            if(remove_item != numItem)
                tabu_tenure[remove_item] = t1 + tabu_iter;
            if(add_item != numItem)
                tabu_tenure[add_item] = t2 + tabu_iter;
        }

        //record best so far
        if(totalProfit > bestTotalProfit)
        {

            bestTime = (clock()-beginTime) / CLOCKS_PER_SEC;

            bestTotalProfit = totalProfit;
            bestTotalWeight = totalWeight;

            for(int i = 0; i < numItem; i++)
                bestChosenItem[i] = chosenItem[i];
            for(int j = 0; j < numElement; j++)
                bestRepeatElement[j] = repeatElement[j];

            non_improve = 0;
        }
        else
            non_improve ++;

        tabu_iter ++;
    }

    //check pro_array
//     for(int i = 0; i < numItem; i++)
//         cout << pro_array[i] << " ";
//    cout << endl;

//        check_result(chosenItem, repeatElement, totalProfit, totalWeight);
//        cout << "TS()." << endl;

//    check_result(bestChosenItem, bestRepeatElement, bestTotalProfit, bestTotalWeight);
//    cout << "tabu." << endl;
//    exit(-1);
}


void perturbation_best()
{
    int record_best[numItem];
    int record_non_best[numItem];
    int length_best = 0, length_non_best = 0;
    int drop_index, pick_index;
    int drop_item, pick_item;
    int temp_num;

//    for(int x = 0; x < numItem; x++)
//        cout << pro_array[x] << " ";
//    cout << endl;


    //transmit best_so_far to current solution
    totalProfit = bestTotalProfit;
    totalWeight = bestTotalWeight;

    for(int i = 0; i < numItem; i++)
       chosenItem[i]= bestChosenItem[i];
    for(int j = 0; j < numElement; j++)
       repeatElement[j] = bestRepeatElement[j];

     for(int x = 0; x < numItem; x++)
     {
         if(chosenItem[x] == 1)
         {
             record_best[length_best] = x;
             length_best ++;
         }
         else
         {
             record_non_best[length_non_best] = x;
             length_non_best ++;
         }
     }


//    cout << "check dorp num, before = " << length_best << endl;


    //drop according to pro_array
    while(length_best != 0)
    {
        drop_index = rand() % length_best;

        drop_item = record_best[drop_index];

        if(chosenItem[drop_item] == 0)
            cout <<"WRONG when finding selected item in perturbation_best()" << endl;

        temp_num = rand() % 100;

        // pro_array is the selected probability, here ">" euqal to 1-p, "<=" equal to p.
        if(temp_num <= pro_array[drop_item] * 100)
        {
            chosenItem[drop_item] = 0;

            totalWeight -= weight[drop_item];

            for(int i = 0; i < lengthEleAdd[drop_item]; i++)
              repeatElement[elementAddress[drop_item][i]] -= 1;

            totalProfit = 0;
            for(int j = 0; j < numElement; j++)
              if(repeatElement[j] > 0)
                  totalProfit += profit[j];
        }

        //here must be outside of if(), if pick_item is examined, then skip
        record_best[drop_index] = record_best[length_best - 1];
        length_best --;
    }

//    int ttt = 0;
//    for(int x = 0; x < numItem; x++)
//       if(chosenItem[x] == 1)
//           ttt ++;
//    cout << "check dorp num, after =" << ttt << endl;



    //repair v1 : DS
   // descend_search();

    //repair v2 : random pick from |UNselected| until knapsack capacity
   while (totalWeight <= knapsackSize)
   {
      pick_index = rand() % length_non_best;

      pick_item = record_non_best[pick_index];

      if(chosenItem[pick_item] == 1)
          cout <<"WRONG when finding |UNselected| item in perturbation_best()" << endl;

       if(totalWeight + weight[pick_item] <= knapsackSize)
       {
           chosenItem[pick_item] = 1;

           totalWeight += weight[pick_item];

           for(int i = 0; i < lengthEleAdd[pick_item]; i++)
            repeatElement[elementAddress[pick_item][i]] += 1;

           totalProfit = 0;
           for(int j = 0; j < numElement; j++)
            if(repeatElement[j] > 0)
                totalProfit += profit[j];

           record_non_best[pick_index] = record_non_best[length_non_best - 1];
           length_non_best --;
       }
       else
           break;
   }

    //repair v3 : pick from |UNselected| until knapsack capacity according to |pro_array|
    // while(totalWeight <= knapsackSize)
    // {
    //     pick_index = random() % length_non_best;

    //     pick_item = record_non_best[pick_index];

    //     if(chosenItem[pick_item] == 1)
    //     {
    //         cout <<"WRONG when finding |UNselected| item in perturbation_best()" << endl;
    //         cout << pick_item << endl;
    //         exit(-1);
    //     }

    //     temp_num = rand() % 100;

    //     // here ">" euqal to 1-p, "<=" equal to p.
    //     if(totalWeight + weight[pick_item] <= knapsackSize)
    //     {
    //         if(temp_num > pro_array[pick_item] * 100)
    //         {
    //             chosenItem[pick_item] = 1;

    //             totalWeight += weight[pick_item];

    //             for(int i = 0; i < lengthEleAdd[pick_item]; i++)
    //                 repeatElement[elementAddress[pick_item][i]] += 1;

    //             totalProfit = 0;
    //             for(int j = 0; j < numElement; j++)
    //              if(repeatElement[j] > 0)
    //                  totalProfit += profit[j];
    //         }

    //         //here must be outside of if(), if pick_item is examined, then skip
    //         record_non_best[pick_index] = record_non_best[length_non_best - 1];
    //         length_non_best --;
    //     }
    //     else
    //         break;
    // }

//    cout << totalProfit << endl;

//    check_result(chosenItem, repeatElement, totalProfit, totalWeight);
//    cout << "perturbation_best()" << endl;
//
//    output_result2();

//    exit(-1);

}

//void perturbation_empty()
//{
//    int temp;
//    int temp_item[numItem];
//    int l_unselected_item = numItem;
//    int rand_item;
//    int temp_num;
//    int temp_weight;
//
//    //reinitial current solution to 0
//    totalProfit = 0;
//    totalWeight = 0;
//
//    for(int i = 0; i < numItem; i++)
//       chosenItem[i]= 0;
//    for(int j = 0; j < numElement; j++)
//       repeatElement[j] = 0;
//
//     for(int x = 0; x < numItem; x++)
//         temp_item[x] = x;
//
//
//    //V1: random pick until knapsack capacity
//    while(totalWeight <= knapsackSize)
//    {
//        temp = rand() % l_unselected_item;
//        rand_item = temp_item[temp];
//
//        temp_weight = totalWeight + weight[rand_item];
//
//        // here ">" euqal to 1-p, "<=" equal to p.
//        if(temp_weight <= knapsackSize)
//        {
//            totalProfit = 0;
//
//            chosenItem[rand_item] = 1;
//            totalWeight += weight[rand_item];
//
//            for(int i = 0; i < lengthEleAdd[rand_item]; i++)
//                repeatElement[elementAddress[rand_item][i]] += 1;
//
//            for(int j = 0; j < numElement; j++)
//                if(repeatElement[j] > 0)
//                        totalProfit += profit[j];
//
//            temp_item[temp] = temp_item[l_unselected_item - 1];
//            l_unselected_item --;
//        }
//        else
//            break;
//    }

    //V2: random pick until knapsack capacity according to |pro_array|
//    while(totalWeight <= knapsackSize)
//    {
//        temp = rand() % l_unselected_item;
//        rand_item = temp_item[temp];
//
//        temp_weight = totalWeight + weight[rand_item];
//
//        temp_num = rand() % 100;
//
//        // here ">" euqal to 1-p, "<=" equal to p.
//        if(temp_weight <= knapsackSize)
//        {
//            if(temp_num > pro_array[rand_item] * 100)
//            {
//
//                totalProfit = 0;
//
//                chosenItem[rand_item] = 1;
//                totalWeight += weight[rand_item];
//
//                for(int i = 0; i < lengthEleAdd[rand_item]; i++)
//                    repeatElement[elementAddress[rand_item][i]] += 1;
//
//                for(int j = 0; j < numElement; j++)
//                    if(repeatElement[j] > 0)
//                            totalProfit += profit[j];
//            }
//
//            //here must be outside of if(), once pick_item is examined, then skip
//            temp_item[temp] = temp_item[l_unselected_item - 1];
//            l_unselected_item --;
//        }
//        else
//            break;
//
//    }

//    descend_search();

//    cout << totalProfit << endl;

//    check_result(chosenItem, repeatElement, totalProfit, totalWeight);
//    cout << "perturbation_empty()" << endl;



//    exit(-1);
//}


void ITS()
{
    int iter = 1;
    double run_time = 0.0;
    double golbal_time = 0.0;
    double beginTime = clock();

    int globalTotalProfit = 0;
    int globalTotalWeight = 0;
    int globalChosenItem[numItem];
    int globalRepeatElement[numElement];

    bestTime = 0.0;
    beginTime = clock();
//    bestTotalProfit = 0;

    initialization();

    while (run_time < maxRunTime)
    {

        record_best_so_far();

        tabu_search();

        if(bestTotalProfit > globalTotalProfit)
        {
            golbal_time = bestTime;
            globalTotalProfit = bestTotalProfit;
            globalTotalWeight = bestTotalWeight;

            for(int i = 0; i < numItem; i++)
                globalChosenItem[i] = bestChosenItem[i];
            for(int j = 0; j < numElement; j++)
                globalRepeatElement[j] = bestRepeatElement[j];

            ofstream allout(nameFinalResult, ios::out|ios::app);
            if(allout.is_open())
            {
                allout << iter << " " << globalTotalProfit <<" " << golbal_time << endl;
                allout.close();
            }

            cout << "iter= " << iter << endl;
            cout << "globalTotalProfit = " << globalTotalProfit << ", globalTotalWeight = " << globalTotalWeight << endl;
            cout << "golbal_time = " << golbal_time << endl;

            int ttt = 0;
            for(int k = 0; k < numItem; k++)
                if(globalChosenItem[k] == 1)
                    ttt ++;
            cout << "num of chosen item = " << ttt << endl;

//            cout <<"=====" << endl;
//            output_result();
        }


//        for(int a = 0; a < numItem; a++)
//            cout << pro_array[a] << " ";
//        cout << endl;

        perturbation_best();
//        perturbation_empty();


        iter ++;
        run_time = (clock() - beginTime) / CLOCKS_PER_SEC;
    }

    check_result(chosenItem, repeatElement, totalProfit, totalWeight);
    cout << "final global result." << endl;
//    exit(-1);
}

int main(int argc, char ** argv)
{
    if (argc == 3)
    {
        instanceName = argv[1];
        seed = atoi(argv[2]);
        rep = argv[2];
    }
    else
    {
        cout << "Enter some paraments to run thegas program: " << endl;
        cout<< "instanceName、number of item and elements" << endl;
        exit(0);
    }

    strcpy(nameFinalResult,instanceName);
    strcat(nameFinalResult,rep);
    // strcat(nameFinalResult,seperate);
    // strcat(nameFinalResult,rep);

    read_instance();

    //test instance
    int a = 0, b = 0;
    for(int i = 0; i < numItem; i++)
        a += weight[i];
     for(int j = 0; j < numElement; j++)
         b += profit[j];
    cout << "----------" << endl;
    cout << "INS sum weight = " << a << ", INS sum profit = " << b << endl;
    cout << "----------" << endl;

    maxRunTime = 600;

//    srand(seed);
    srand((unsigned)time(NULL));

    ITS();

    delete [] profit;
    delete [] weight;
    delete [] lengthEleAdd;
    delete [] chosenItem;
    delete [] repeatElement;
    delete [] bestChosenItem;
    delete [] bestRepeatElement;
    delete [] pro_array;

    for(int x = 0; x < numItem; x++)
        delete [] relationMatrix[x];
    for(int y = 0; y < numItem; y++)
        delete [] elementAddress[y];
    delete [] relationMatrix;
    delete [] elementAddress;

    return 0;
}
