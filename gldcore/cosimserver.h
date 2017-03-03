
/********************************************************************/
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include "output.h"
#include "object.h"
#include "exception.h"
#include "random.h"
#include "globals.h"
#include "math.h"
#include "time.h"

// multithreading
#include "omp.h"
#include "pthread.h"

//  mesage formats
// #include "zmq.h"
#include "jansson.h"

//  stuff from server code
// JSON Parser Functions

/********************************************************************/

char *get_list;

json_t *root;
json_t *load_json_server(const char *text);
void json_get_list_server(json_t *root);
void json_set_list_server(json_t *root);

/********************************************************************/
/******convert a json string to a json object ******/
json_t *load_json_server(const char *text)
{
    json_t *root;
    json_error_t error;

    root = json_loads(text, 0, &error);

    if (root) {
        return root;
    } else {
        fprintf(stderr, "json error on line %d: %s\n", error.line, error.text);
        return (json_t *)0;
    }
}
/****************************************************************************************/

/****** Retuns a list of JSON objects over the ZMQ message bus ******/
void json_get_list_server(json_t *root)
{

    static int flag_array = 0;
    char gridlabd_response[2048] = "";
    switch(json_typeof(root))
    {
      case  JSON_OBJECT :{
          // printf("\n type : JSON_OBJECT");
          const char *key;
          json_t *value;
          OBJECT *obj;
          char property_name[200];
          char object_name[200];
          char property_value[200];


          json_object_foreach(root, key, value) {

                strcpy (property_name,json_string_value(json_object_get(root,"name")) );
                strcpy (object_name,json_string_value(json_object_get(root,"key")) );
                if (strcmp(object_name,"global") == 0)
                {
                  // global_setvar(property_name,property_value);
                  strcpy(property_value,"");
                  if (global_getvar(property_name,property_value,sizeof(property_value))==NULL)
               		{
               			output_error("global variable '%s' not found", property_name);
               			return ;
               		}
                  // output_verbose("\n json object : %s",json_dumps(root,JSON_ENCODE_ANY));
                  // strcpy(gridlabd_response,"GRIDLABD_RESPONSE$");
                  // strcat(gridlabd_response,json_dumps(root,JSON_ENCODE_ANY));
                  // zmq_send(requester,gridlabd_response, strlen(gridlabd_response), 0);
                  obj = NULL;
                  key = NULL;
                  value = NULL;
                  return;
                }


                else
                {
                obj = object_find_name(object_name);
                if (obj == NULL)
            			output_error("no such obkect %s",object_name);
                char buffer[1024]="";
                if ( !object_get_value_by_name(obj,property_name,buffer,sizeof(buffer)))
            		{
            			output_error("object '%s' property '%s' not found ", object_name, property_name);
            			return ;
            		}
                json_object_set(root,"value",json_string(buffer));

                if (flag_array == 0)
                {
                  // output_verbose("\n json object : %s",json_dumps(root,JSON_ENCODE_ANY));
                  //
                  // strcpy(gridlabd_response,"GRIDLABD_RESPONSE$");
                  // strcat(gridlabd_response,json_dumps(root,JSON_ENCODE_ANY));
                  // zmq_send(requester,gridlabd_response, strlen(gridlabd_response), 0);
                  ;;

                }

          }
          obj = NULL;
          key = NULL;
          value = NULL;
        }
      break;}
      case  JSON_ARRAY :{

        flag_array = 1;
        printf("\n type : JSON_ARRAY");
        size_t index;
        json_t *value;
        json_array_foreach(root,index,value)
        {
          json_get_list(value);
        }
        // output_verbose("\n json array : %s",json_dumps(root,JSON_ENCODE_ANY));
        // strcpy(gridlabd_response,"GRIDLABD_RESPONSE$");
        // strcat(gridlabd_response,json_dumps(root,JSON_ENCODE_ANY));
        // zmq_send(requester,gridlabd_response, strlen(gridlabd_response), 0);
        flag_array =0;

      break;}
      case  JSON_NULL :{ printf("\n type : JSON_NULL");break;}
    }

}
/****************************************************************************************/

/****** Sets a list of JSON objects, Retuns a list of
 JSON objects over the ZMQ message bus ******/
void json_set_list_server(json_t *root)
{
  char gridlabd_response[2048] = "";
    static int flag_array = 0;
    switch(json_typeof(root))
    {
      case  JSON_OBJECT :{
          printf("\n type : JSON_OBJECT");
          const char *key;
          json_t *value;
          OBJECT *obj;
          char property_name[200];
          char object_name[200];
          // char property_value[200];
          char property_value[200];
          char gridlabd_response[2048] = "";

          json_object_foreach(root, key, value) {

                strcpy (property_name,json_string_value(json_object_get(root,"name")) );
                strcpy (object_name,json_string_value(json_object_get(root,"key")) );
                strcpy (property_value,json_string_value(json_object_get(root,"value")) );

                if (strcmp(object_name,"global") == 0)
                {
                  global_setvar(property_name,property_value);
                  strcpy(property_value,"");
                  if (global_getvar(property_name,property_value,sizeof(property_value))==NULL)
               		{
               			output_error("global variable '%s' not found", property_name);
               			return ;
               		}
                  // output_verbose("\n json object : %s",json_dumps(root,JSON_ENCODE_ANY));
                  // strcpy(gridlabd_response,"GRIDLABD_RESPONSE$");
                  // strcat(gridlabd_response,json_dumps(root,JSON_ENCODE_ANY));
                  // zmq_send(requester,gridlabd_response, strlen(gridlabd_response), 0);
                  obj = NULL;
                  key = NULL;
                  value = NULL;
                  return;
                }

                else{

                obj = object_find_name(object_name);
                if (obj == NULL)
            			output_error("no such obkect %s",object_name);
                char buffer[1024]="";


                if (!object_set_value_by_name(obj,property_name,property_value))
                {
                  output_error("object '%s' property '%s' can't be set'", object_name, property_name);
                  return ;
                }

                if ( !object_get_value_by_name(obj,property_name,buffer,sizeof(buffer)))
            		{
            			output_error("object '%s' property '%s' not found ", object_name, property_name);
            			return ;
            		}
                json_object_set(root,"value",json_string(buffer));

                if (flag_array == 0)
                {
                  // output_verbose("\n json object : %s",json_dumps(root,JSON_ENCODE_ANY));
                  // strcpy(gridlabd_response,"GRIDLABD_RESPONSE$");
                  // strcat(gridlabd_response,json_dumps(root,JSON_ENCODE_ANY));
                  // zmq_send(requester,gridlabd_response, strlen(gridlabd_response), 0);
                  ;;

                }


          }
          obj = NULL;
      		key = NULL;
      		value = NULL;
        }

      break;}
      case  JSON_ARRAY :{

        flag_array = 1;
        // printf("\n type : JSON_ARRAY");
        size_t index;
        json_t *value;
        json_array_foreach(root,index,value)
        {
          json_set_list(value);
        }
        output_verbose("\n json array : %s",json_dumps(root,JSON_ENCODE_ANY));
        // strcpy(gridlabd_response,"GRIDLABD_RESPONSE$");
        // strcat(gridlabd_response,json_dumps(root,JSON_ENCODE_ANY));
        // zmq_send(requester,gridlabd_response, strlen(gridlabd_response), 0);
        flag_array =0;

      break;}
      case  JSON_NULL :{ printf("\n type : JSON_NULL");break;}
    }

}
/****************************************************************************************/
