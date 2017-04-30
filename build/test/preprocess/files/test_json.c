#include "cJSON.h"
#include "unity.h"


void setUp(void) {

}



void tearDown(void) {

}



void test_json() {

  cJSON *root;

  cJSON *fmt;

  root = cJSON_CreateObject();

  cJSON_AddItemToObject(root, "name",

                        cJSON_CreateString("Jack (\"Bee\") Nimble"));

  cJSON_AddItemToObject(root, "format", fmt = cJSON_CreateObject());

  cJSON_AddItemToObject(fmt, "type", cJSON_CreateString("rect"));

  cJSON_AddItemToObject(fmt, "width", cJSON_CreateNumber(1920));

  cJSON_AddItemToObject(fmt, "height", cJSON_CreateNumber(1080));

  cJSON_AddItemToObject(fmt, "interlace", cJSON_CreateFalse());

  cJSON_AddItemToObject(fmt, "frame rate", cJSON_CreateNumber(24));

  printf(cJSON_Print(root));

}
