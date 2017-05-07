/*********************************************************************************************
 ___                  _      ___   _____  ___
 |  _`\               ( )    (  _`\(_   _)(  _`\
 | (_) )   _      ___ | |/') | (_(_) | |  | (_(_)
 | ,  /  /'_`\  /'___)| , <  |  _)_  | |  `\__ \
 | |\ \ ( (_) )( (___ | |\`\ | (_( ) | |  ( )_) |
 (_) (_)`\___/'`\____)(_) (_)(____/' (_)  `\____)

 Copyright (c) 2016-2017
 All rights reserved

 RockETS, Montreal
 Ecole de Technologies Superieures
 *********************************************************************************************/
#include "Low_Pass_Filter.h"

void Init_LP_Filter(LP_Filter_t * temp_LP_filter){
  temp_LP_filter->Input[0] = 0;
  temp_LP_filter->Input[1] = 0;
  temp_LP_filter->Input[2] = 0;
  temp_LP_filter->Input[3] = 0;


  temp_LP_filter->Output[0] = 0;
  temp_LP_filter->Output[1] = 0;
  temp_LP_filter->Output[2] = 0;
  temp_LP_filter->Output[3] = 0;


  temp_LP_filter->isInitialized = 1;
}


float Update_LP_Filter(LP_Filter_t * temp_LP_filter, float input_value){
  temp_LP_filter->Input[3] = temp_LP_filter->Input[2];
  temp_LP_filter->Input[2] = temp_LP_filter->Input[1];
  temp_LP_filter->Input[1] = temp_LP_filter->Input[0];
  temp_LP_filter->Input[0] = input_value;

  temp_LP_filter->Output[3] = temp_LP_filter->Output[2];
  temp_LP_filter->Output[2] = temp_LP_filter->Output[1];
  temp_LP_filter->Output[1] = temp_LP_filter->Output[0];
  temp_LP_filter->Output[0] = (temp_LP_filter->Input[3]) + ( 3 * temp_LP_filter->Input[2]) + ( 3 * temp_LP_filter->Input[1]) + (temp_LP_filter->Input[0]) + ( 0 * temp_LP_filter->Output[3]) + ( -0.33333333 * temp_LP_filter->Output[2]) + ( 0 * temp_LP_filter->Output[1]);
  //see https://www-users.cs.york.ac.uk/~fisher/mkfilter/trad.html

  return temp_LP_filter->Output[0];
}


